// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/lazy_instance.h"
#include "base/location.h"
#include "base/thread_task_runner_handle.h"
#include "base/time/time.h"
#include "cc/debug/lap_timer.h"
#include "cc/raster/raster_buffer.h"
#include "cc/test/begin_frame_args_test.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/fake_picture_layer_impl.h"
#include "cc/test/fake_tile_manager.h"
#include "cc/test/fake_tile_manager_client.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/test/test_tile_priorities.h"
#include "cc/tiles/tile.h"
#include "cc/tiles/tile_priority.h"
#include "cc/trees/layer_tree_impl.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/perf/perf_test.h"

namespace cc {
namespace {

    static const int kTimeLimitMillis = 2000;
    static const int kWarmupRuns = 5;
    static const int kTimeCheckInterval = 10;

    class FakeTileTaskRunnerImpl : public TileTaskRunner, public TileTaskClient {
    public:
        // Overridden from TileTaskRunner:
        void SetClient(TileTaskRunnerClient* client) override { }
        void Shutdown() override { }
        void ScheduleTasks(TileTaskQueue* queue) override
        {
            for (TileTaskQueue::Item::Vector::const_iterator it = queue->items.begin();
                 it != queue->items.end(); ++it) {
                RasterTask* task = it->task;

                task->WillSchedule();
                task->ScheduleOnOriginThread(this);
                task->DidSchedule();

                completed_tasks_.push_back(task);
            }
        }
        void CheckForCompletedTasks() override
        {
            for (RasterTask::Vector::iterator it = completed_tasks_.begin();
                 it != completed_tasks_.end();
                 ++it) {
                RasterTask* task = it->get();

                task->WillComplete();
                task->CompleteOnOriginThread(this);
                task->DidComplete();
            }
            completed_tasks_.clear();
        }
        ResourceFormat GetResourceFormat(bool must_support_alpha) const override
        {
            return RGBA_8888;
        }
        bool GetResourceRequiresSwizzle(bool must_support_alpha) const override
        {
            return !PlatformColor::SameComponentOrder(
                GetResourceFormat(must_support_alpha));
        }

        // Overridden from TileTaskClient:
        scoped_ptr<RasterBuffer> AcquireBufferForRaster(
            const Resource* resource,
            uint64_t new_content_id,
            uint64_t previous_content_id) override
        {
            return nullptr;
        }
        void ReleaseBufferForRaster(scoped_ptr<RasterBuffer> buffer) override { }

    private:
        RasterTask::Vector completed_tasks_;
    };
    base::LazyInstance<FakeTileTaskRunnerImpl> g_fake_tile_task_runner = LAZY_INSTANCE_INITIALIZER;

    class TileManagerPerfTest : public testing::Test {
    public:
        TileManagerPerfTest()
            : memory_limit_policy_(ALLOW_ANYTHING)
            , max_tiles_(10000)
            , id_(7)
            , proxy_(base::ThreadTaskRunnerHandle::Get())
            , output_surface_(FakeOutputSurface::Create3d())
            , host_impl_(LayerTreeSettings(),
                  &proxy_,
                  &shared_bitmap_manager_,
                  &task_graph_runner_)
            , timer_(kWarmupRuns,
                  base::TimeDelta::FromMilliseconds(kTimeLimitMillis),
                  kTimeCheckInterval)
        {
        }

        void SetTreePriority(TreePriority tree_priority)
        {
            GlobalStateThatImpactsTilePriority state;
            gfx::Size tile_size(256, 256);

            state.soft_memory_limit_in_bytes = 100 * 1000 * 1000;
            state.num_resources_limit = max_tiles_;
            state.hard_memory_limit_in_bytes = state.soft_memory_limit_in_bytes * 2;
            state.memory_limit_policy = memory_limit_policy_;
            state.tree_priority = tree_priority;

            global_state_ = state;
            host_impl_.resource_pool()->SetResourceUsageLimits(
                state.soft_memory_limit_in_bytes, state.num_resources_limit);
            host_impl_.tile_manager()->SetGlobalStateForTesting(state);
        }

        void SetUp() override
        {
            InitializeRenderer();
            SetTreePriority(SAME_PRIORITY_FOR_BOTH_TREES);
        }

        virtual void InitializeRenderer()
        {
            host_impl_.InitializeRenderer(output_surface_.get());
            tile_manager()->SetTileTaskRunnerForTesting(
                g_fake_tile_task_runner.Pointer());
        }

        void SetupDefaultTrees(const gfx::Size& layer_bounds)
        {
            scoped_refptr<FakeDisplayListRasterSource> pending_raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
            scoped_refptr<FakeDisplayListRasterSource> active_raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);

            SetupTrees(pending_raster_source, active_raster_source);
        }

        void ActivateTree()
        {
            host_impl_.ActivateSyncTree();
            CHECK(!host_impl_.pending_tree());
            pending_root_layer_ = NULL;
            active_root_layer_ = static_cast<FakePictureLayerImpl*>(
                host_impl_.active_tree()->LayerById(id_));
        }

        void SetupDefaultTreesWithFixedTileSize(const gfx::Size& layer_bounds,
            const gfx::Size& tile_size)
        {
            SetupDefaultTrees(layer_bounds);
            pending_root_layer_->set_fixed_tile_size(tile_size);
            active_root_layer_->set_fixed_tile_size(tile_size);
        }

        void SetupTrees(scoped_refptr<RasterSource> pending_raster_source,
            scoped_refptr<RasterSource> active_raster_source)
        {
            SetupPendingTree(active_raster_source);
            ActivateTree();
            SetupPendingTree(pending_raster_source);
        }

        void SetupPendingTree(scoped_refptr<RasterSource> raster_source)
        {
            host_impl_.CreatePendingTree();
            LayerTreeImpl* pending_tree = host_impl_.pending_tree();
            // Clear recycled tree.
            pending_tree->DetachLayerTree();

            scoped_ptr<FakePictureLayerImpl> pending_layer = FakePictureLayerImpl::CreateWithRasterSource(pending_tree, id_,
                raster_source);
            pending_layer->SetDrawsContent(true);
            pending_layer->SetHasRenderSurface(true);
            pending_tree->SetRootLayer(pending_layer.Pass());

            pending_root_layer_ = static_cast<FakePictureLayerImpl*>(
                host_impl_.pending_tree()->LayerById(id_));
        }

        void RunRasterQueueConstructTest(const std::string& test_name,
            int layer_count)
        {
            TreePriority priorities[] = { SAME_PRIORITY_FOR_BOTH_TREES,
                SMOOTHNESS_TAKES_PRIORITY,
                NEW_CONTENT_TAKES_PRIORITY };
            int priority_count = 0;

            std::vector<FakePictureLayerImpl*> layers = CreateLayers(layer_count, 10);
            bool resourceless_software_draw = false;
            for (const auto& layer : layers)
                layer->UpdateTiles(resourceless_software_draw);

            timer_.Reset();
            do {
                scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
                    priorities[priority_count], RasterTilePriorityQueue::Type::ALL));
                priority_count = (priority_count + 1) % arraysize(priorities);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult("tile_manager_raster_tile_queue_construct",
                "",
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

        void RunRasterQueueConstructAndIterateTest(const std::string& test_name,
            int layer_count,
            int tile_count)
        {
            TreePriority priorities[] = { SAME_PRIORITY_FOR_BOTH_TREES,
                SMOOTHNESS_TAKES_PRIORITY,
                NEW_CONTENT_TAKES_PRIORITY };

            std::vector<FakePictureLayerImpl*> layers = CreateLayers(layer_count, 100);
            bool resourceless_software_draw = false;
            for (const auto& layer : layers)
                layer->UpdateTiles(resourceless_software_draw);

            int priority_count = 0;
            timer_.Reset();
            do {
                int count = tile_count;
                scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
                    priorities[priority_count], RasterTilePriorityQueue::Type::ALL));
                while (count--) {
                    ASSERT_FALSE(queue->IsEmpty());
                    ASSERT_TRUE(queue->Top().tile());
                    queue->Pop();
                }
                priority_count = (priority_count + 1) % arraysize(priorities);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult(
                "tile_manager_raster_tile_queue_construct_and_iterate",
                "",
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

        void RunEvictionQueueConstructTest(const std::string& test_name,
            int layer_count)
        {
            TreePriority priorities[] = { SAME_PRIORITY_FOR_BOTH_TREES,
                SMOOTHNESS_TAKES_PRIORITY,
                NEW_CONTENT_TAKES_PRIORITY };
            int priority_count = 0;

            std::vector<FakePictureLayerImpl*> layers = CreateLayers(layer_count, 10);
            bool resourceless_software_draw = false;
            for (const auto& layer : layers) {
                layer->UpdateTiles(resourceless_software_draw);
                for (size_t i = 0; i < layer->num_tilings(); ++i) {
                    tile_manager()->InitializeTilesWithResourcesForTesting(
                        layer->tilings()->tiling_at(i)->AllTilesForTesting());
                }
            }

            timer_.Reset();
            do {
                scoped_ptr<EvictionTilePriorityQueue> queue(
                    host_impl_.BuildEvictionQueue(priorities[priority_count]));
                priority_count = (priority_count + 1) % arraysize(priorities);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult("tile_manager_eviction_tile_queue_construct",
                "",
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

        void RunEvictionQueueConstructAndIterateTest(const std::string& test_name,
            int layer_count,
            int tile_count)
        {
            TreePriority priorities[] = { SAME_PRIORITY_FOR_BOTH_TREES,
                SMOOTHNESS_TAKES_PRIORITY,
                NEW_CONTENT_TAKES_PRIORITY };
            int priority_count = 0;

            std::vector<FakePictureLayerImpl*> layers = CreateLayers(layer_count, tile_count);
            bool resourceless_software_draw = false;
            for (const auto& layer : layers) {
                layer->UpdateTiles(resourceless_software_draw);
                for (size_t i = 0; i < layer->num_tilings(); ++i) {
                    tile_manager()->InitializeTilesWithResourcesForTesting(
                        layer->tilings()->tiling_at(i)->AllTilesForTesting());
                }
            }

            timer_.Reset();
            do {
                int count = tile_count;
                scoped_ptr<EvictionTilePriorityQueue> queue(
                    host_impl_.BuildEvictionQueue(priorities[priority_count]));
                while (count--) {
                    ASSERT_FALSE(queue->IsEmpty());
                    ASSERT_TRUE(queue->Top().tile());
                    queue->Pop();
                }
                priority_count = (priority_count + 1) % arraysize(priorities);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult(
                "tile_manager_eviction_tile_queue_construct_and_iterate",
                "",
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

        std::vector<FakePictureLayerImpl*> CreateLayers(int layer_count,
            int tiles_per_layer_count)
        {
            // Compute the width/height required for high res to get
            // tiles_per_layer_count tiles.
            float width = std::sqrt(static_cast<float>(tiles_per_layer_count));
            float height = tiles_per_layer_count / width;

            // Adjust the width and height to account for the fact that tiles
            // are bigger than 1x1. Also, account for the fact that that we
            // will be creating one high res and one low res tiling. That is,
            // width and height should be smaller by sqrt(1 + low_res_scale).
            // This gives us _approximately_ correct counts.
            width *= settings_.default_tile_size.width() / std::sqrt(1 + settings_.low_res_contents_scale_factor);
            height *= settings_.default_tile_size.height() / std::sqrt(1 + settings_.low_res_contents_scale_factor);

            // Ensure that we start with blank trees and no tiles.
            host_impl_.ResetTreesForTesting();
            tile_manager()->FreeResourcesAndCleanUpReleasedTilesForTesting();

            gfx::Size layer_bounds(width, height);
            gfx::Size viewport(width / 5, height / 5);
            host_impl_.SetViewportSize(viewport);
            SetupDefaultTreesWithFixedTileSize(layer_bounds,
                settings_.default_tile_size);

            std::vector<FakePictureLayerImpl*> layers;

            // Pending layer counts as one layer.
            layers.push_back(pending_root_layer_);
            int next_id = id_ + 1;

            // Create the rest of the layers as children of the root layer.
            scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
            while (static_cast<int>(layers.size()) < layer_count) {
                scoped_ptr<FakePictureLayerImpl> layer = FakePictureLayerImpl::CreateWithRasterSource(
                    host_impl_.pending_tree(), next_id, raster_source);
                layer->SetBounds(layer_bounds);
                layer->SetDrawsContent(true);
                layers.push_back(layer.get());
                pending_root_layer_->AddChild(layer.Pass());
                ++next_id;
            }

            bool update_lcd_text = false;
            host_impl_.pending_tree()->UpdateDrawProperties(update_lcd_text);
            for (FakePictureLayerImpl* layer : layers)
                layer->CreateAllTiles();

            return layers;
        }

        GlobalStateThatImpactsTilePriority GlobalStateForTest()
        {
            GlobalStateThatImpactsTilePriority state;
            gfx::Size tile_size = settings_.default_tile_size;
            state.soft_memory_limit_in_bytes = 10000u * 4u * static_cast<size_t>(tile_size.width() * tile_size.height());
            state.hard_memory_limit_in_bytes = state.soft_memory_limit_in_bytes;
            state.num_resources_limit = 10000;
            state.memory_limit_policy = ALLOW_ANYTHING;
            state.tree_priority = SMOOTHNESS_TAKES_PRIORITY;
            return state;
        }

        void RunPrepareTilesTest(const std::string& test_name,
            int layer_count,
            int approximate_tile_count_per_layer)
        {
            std::vector<FakePictureLayerImpl*> layers = CreateLayers(layer_count, approximate_tile_count_per_layer);

            timer_.Reset();
            bool resourceless_software_draw = false;
            do {
                host_impl_.AdvanceToNextFrame(base::TimeDelta::FromMilliseconds(1));
                for (const auto& layer : layers)
                    layer->UpdateTiles(resourceless_software_draw);

                GlobalStateThatImpactsTilePriority global_state(GlobalStateForTest());
                tile_manager()->PrepareTiles(global_state);
                tile_manager()->Flush();
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult("prepare_tiles", "", test_name,
                timer_.LapsPerSecond(), "runs/s", true);
        }

        TileManager* tile_manager() { return host_impl_.tile_manager(); }

    protected:
        GlobalStateThatImpactsTilePriority global_state_;

        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        TileMemoryLimitPolicy memory_limit_policy_;
        int max_tiles_;
        int id_;
        FakeImplProxy proxy_;
        scoped_ptr<OutputSurface> output_surface_;
        FakeLayerTreeHostImpl host_impl_;
        FakePictureLayerImpl* pending_root_layer_;
        FakePictureLayerImpl* active_root_layer_;
        LapTimer timer_;
        LayerTreeSettings settings_;
    };

    TEST_F(TileManagerPerfTest, PrepareTiles)
    {
        RunPrepareTilesTest("2_100", 2, 100);
        RunPrepareTilesTest("2_500", 2, 500);
        RunPrepareTilesTest("2_1000", 2, 1000);
        RunPrepareTilesTest("10_100", 10, 100);
        RunPrepareTilesTest("10_500", 10, 500);
        RunPrepareTilesTest("10_1000", 10, 1000);
        RunPrepareTilesTest("50_100", 100, 100);
        RunPrepareTilesTest("50_500", 100, 500);
        RunPrepareTilesTest("50_1000", 100, 1000);
    }

    TEST_F(TileManagerPerfTest, RasterTileQueueConstruct)
    {
        RunRasterQueueConstructTest("2", 2);
        RunRasterQueueConstructTest("10", 10);
        RunRasterQueueConstructTest("50", 50);
    }

    TEST_F(TileManagerPerfTest, RasterTileQueueConstructAndIterate)
    {
        RunRasterQueueConstructAndIterateTest("2_16", 2, 16);
        RunRasterQueueConstructAndIterateTest("2_32", 2, 32);
        RunRasterQueueConstructAndIterateTest("2_64", 2, 64);
        RunRasterQueueConstructAndIterateTest("2_128", 2, 128);
        RunRasterQueueConstructAndIterateTest("10_16", 10, 16);
        RunRasterQueueConstructAndIterateTest("10_32", 10, 32);
        RunRasterQueueConstructAndIterateTest("10_64", 10, 64);
        RunRasterQueueConstructAndIterateTest("10_128", 10, 128);
        RunRasterQueueConstructAndIterateTest("50_16", 50, 16);
        RunRasterQueueConstructAndIterateTest("50_32", 50, 32);
        RunRasterQueueConstructAndIterateTest("50_64", 50, 64);
        RunRasterQueueConstructAndIterateTest("50_128", 50, 128);
    }

    TEST_F(TileManagerPerfTest, EvictionTileQueueConstruct)
    {
        RunEvictionQueueConstructTest("2", 2);
        RunEvictionQueueConstructTest("10", 10);
        RunEvictionQueueConstructTest("50", 50);
    }

    TEST_F(TileManagerPerfTest, EvictionTileQueueConstructAndIterate)
    {
        RunEvictionQueueConstructAndIterateTest("2_16", 2, 16);
        RunEvictionQueueConstructAndIterateTest("2_32", 2, 32);
        RunEvictionQueueConstructAndIterateTest("2_64", 2, 64);
        RunEvictionQueueConstructAndIterateTest("2_128", 2, 128);
        RunEvictionQueueConstructAndIterateTest("10_16", 10, 16);
        RunEvictionQueueConstructAndIterateTest("10_32", 10, 32);
        RunEvictionQueueConstructAndIterateTest("10_64", 10, 64);
        RunEvictionQueueConstructAndIterateTest("10_128", 10, 128);
        RunEvictionQueueConstructAndIterateTest("50_16", 50, 16);
        RunEvictionQueueConstructAndIterateTest("50_32", 50, 32);
        RunEvictionQueueConstructAndIterateTest("50_64", 50, 64);
        RunEvictionQueueConstructAndIterateTest("50_128", 50, 128);
    }

} // namespace
} // namespace cc
