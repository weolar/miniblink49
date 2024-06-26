// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/run_loop.h"
#include "base/thread_task_runner_handle.h"
#include "cc/playback/display_list_raster_source.h"
#include "cc/playback/display_list_recording_source.h"
#include "cc/raster/raster_buffer.h"
#include "cc/resources/resource_pool.h"
#include "cc/test/begin_frame_args_test.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/fake_display_list_recording_source.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/fake_picture_layer_impl.h"
#include "cc/test/fake_picture_layer_tiling_client.h"
#include "cc/test/fake_tile_manager.h"
#include "cc/test/test_gpu_memory_buffer_manager.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/test/test_tile_priorities.h"
#include "cc/tiles/eviction_tile_priority_queue.h"
#include "cc/tiles/raster_tile_priority_queue.h"
#include "cc/tiles/tile.h"
#include "cc/tiles/tile_priority.h"
#include "cc/tiles/tiling_set_raster_queue_all.h"
#include "cc/trees/layer_tree_impl.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkSurface.h"

namespace cc {
namespace {

    class LowResTilingsSettings : public LayerTreeSettings {
    public:
        LowResTilingsSettings() { create_low_res_tiling = true; }
    };

    class TileManagerTilePriorityQueueTest : public testing::Test {
    public:
        TileManagerTilePriorityQueueTest()
            : memory_limit_policy_(ALLOW_ANYTHING)
            , max_tiles_(10000)
            , ready_to_activate_(false)
            , id_(7)
            , proxy_(base::ThreadTaskRunnerHandle::Get())
            , output_surface_(FakeOutputSurface::Create3d())
            , host_impl_(LowResTilingsSettings(),
                  &proxy_,
                  &shared_bitmap_manager_,
                  &task_graph_runner_,
                  &gpu_memory_buffer_manager_)
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
                state.soft_memory_limit_in_bytes,
                state.num_resources_limit);
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
        }

        void SetupDefaultTrees(const gfx::Size& layer_bounds)
        {
            scoped_refptr<FakeDisplayListRasterSource> pending_raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
            scoped_refptr<FakeDisplayListRasterSource> active_raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);

            SetupTrees(pending_raster_source, active_raster_source);
        }

        // This matches picture_layer_impl_unittest's ActivateTree.
        void ActivateTree()
        {
            host_impl_.ActivateSyncTree();
            CHECK(!host_impl_.pending_tree());
            pending_layer_ = NULL;
            active_layer_ = static_cast<FakePictureLayerImpl*>(
                host_impl_.active_tree()->LayerById(id_));
            bool update_lcd_text = false;
            host_impl_.active_tree()->UpdateDrawProperties(update_lcd_text);
        }

        void SetupDefaultTreesWithFixedTileSize(const gfx::Size& layer_bounds,
            const gfx::Size& tile_size)
        {
            SetupDefaultTrees(layer_bounds);
            pending_layer_->set_fixed_tile_size(tile_size);
            active_layer_->set_fixed_tile_size(tile_size);
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

            // Steal from the recycled tree.
            scoped_ptr<LayerImpl> old_pending_root = pending_tree->DetachLayerTree();
            DCHECK_IMPLIES(old_pending_root, old_pending_root->id() == id_);

            scoped_ptr<FakePictureLayerImpl> pending_layer;
            if (old_pending_root) {
                pending_layer.reset(
                    static_cast<FakePictureLayerImpl*>(old_pending_root.release()));
                pending_layer->SetRasterSourceOnPending(raster_source, Region());
            } else {
                pending_layer = FakePictureLayerImpl::CreateWithRasterSource(
                    pending_tree, id_, raster_source);
                pending_layer->SetDrawsContent(true);
                pending_layer->SetHasRenderSurface(true);
            }
            // The bounds() just mirror the raster source size.
            pending_layer->SetBounds(pending_layer->raster_source()->GetSize());
            pending_tree->SetRootLayer(pending_layer.Pass());

            pending_layer_ = static_cast<FakePictureLayerImpl*>(
                host_impl_.pending_tree()->LayerById(id_));

            // Add tilings/tiles for the layer.
            bool update_lcd_text = false;
            host_impl_.pending_tree()->UpdateDrawProperties(update_lcd_text);
        }

        TileManager* tile_manager() { return host_impl_.tile_manager(); }

    protected:
        GlobalStateThatImpactsTilePriority global_state_;

        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        TestGpuMemoryBufferManager gpu_memory_buffer_manager_;
        TileMemoryLimitPolicy memory_limit_policy_;
        int max_tiles_;
        bool ready_to_activate_;
        int id_;
        FakeImplProxy proxy_;
        scoped_ptr<OutputSurface> output_surface_;
        FakeLayerTreeHostImpl host_impl_;
        FakePictureLayerImpl* pending_layer_;
        FakePictureLayerImpl* active_layer_;
    };

    TEST_F(TileManagerTilePriorityQueueTest, RasterTilePriorityQueue)
    {
        const gfx::Size layer_bounds(1000, 1000);
        host_impl_.SetViewportSize(layer_bounds);
        SetupDefaultTrees(layer_bounds);

        scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        EXPECT_FALSE(queue->IsEmpty());

        size_t tile_count = 0;
        std::set<Tile*> all_tiles;
        while (!queue->IsEmpty()) {
            EXPECT_TRUE(queue->Top().tile());
            all_tiles.insert(queue->Top().tile());
            ++tile_count;
            queue->Pop();
        }

        EXPECT_EQ(tile_count, all_tiles.size());
        EXPECT_EQ(16u, tile_count);

        // Sanity check, all tiles should be visible.
        std::set<Tile*> smoothness_tiles;
        queue = host_impl_.BuildRasterQueue(SMOOTHNESS_TAKES_PRIORITY,
            RasterTilePriorityQueue::Type::ALL);
        bool had_low_res = false;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile());
            EXPECT_EQ(TilePriority::NOW, prioritized_tile.priority().priority_bin);
            if (prioritized_tile.priority().resolution == LOW_RESOLUTION)
                had_low_res = true;
            else
                smoothness_tiles.insert(prioritized_tile.tile());
            queue->Pop();
        }
        EXPECT_EQ(all_tiles, smoothness_tiles);
        EXPECT_TRUE(had_low_res);

        // Check that everything is required for activation.
        queue = host_impl_.BuildRasterQueue(
            SMOOTHNESS_TAKES_PRIORITY,
            RasterTilePriorityQueue::Type::REQUIRED_FOR_ACTIVATION);
        std::set<Tile*> required_for_activation_tiles;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile()->required_for_activation());
            required_for_activation_tiles.insert(prioritized_tile.tile());
            queue->Pop();
        }
        EXPECT_EQ(all_tiles, required_for_activation_tiles);

        // Check that everything is required for draw.
        queue = host_impl_.BuildRasterQueue(
            SMOOTHNESS_TAKES_PRIORITY,
            RasterTilePriorityQueue::Type::REQUIRED_FOR_DRAW);
        std::set<Tile*> required_for_draw_tiles;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile()->required_for_draw());
            required_for_draw_tiles.insert(prioritized_tile.tile());
            queue->Pop();
        }
        EXPECT_EQ(all_tiles, required_for_draw_tiles);

        Region invalidation(gfx::Rect(0, 0, 500, 500));

        // Invalidate the pending tree.
        pending_layer_->set_invalidation(invalidation);
        pending_layer_->HighResTiling()->Invalidate(invalidation);

        // Renew all of the tile priorities.
        gfx::Rect viewport(50, 50, 100, 100);
        pending_layer_->HighResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());
        active_layer_->HighResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());
        active_layer_->LowResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());

        // Populate all tiles directly from the tilings.
        all_tiles.clear();
        std::set<Tile*> high_res_tiles;
        std::vector<Tile*> pending_high_res_tiles = pending_layer_->HighResTiling()->AllTilesForTesting();
        for (size_t i = 0; i < pending_high_res_tiles.size(); ++i) {
            all_tiles.insert(pending_high_res_tiles[i]);
            high_res_tiles.insert(pending_high_res_tiles[i]);
        }

        std::vector<Tile*> active_high_res_tiles = active_layer_->HighResTiling()->AllTilesForTesting();
        for (size_t i = 0; i < active_high_res_tiles.size(); ++i) {
            all_tiles.insert(active_high_res_tiles[i]);
            high_res_tiles.insert(active_high_res_tiles[i]);
        }

        std::vector<Tile*> active_low_res_tiles = active_layer_->LowResTiling()->AllTilesForTesting();
        for (size_t i = 0; i < active_low_res_tiles.size(); ++i)
            all_tiles.insert(active_low_res_tiles[i]);

        PrioritizedTile last_tile;
        smoothness_tiles.clear();
        tile_count = 0;
        size_t correct_order_tiles = 0u;
        // Here we expect to get increasing ACTIVE_TREE priority_bin.
        queue = host_impl_.BuildRasterQueue(SMOOTHNESS_TAKES_PRIORITY,
            RasterTilePriorityQueue::Type::ALL);
        std::set<Tile*> expected_required_for_draw_tiles;
        std::set<Tile*> expected_required_for_activation_tiles;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile());

            if (!last_tile.tile())
                last_tile = prioritized_tile;

            EXPECT_LE(last_tile.priority().priority_bin,
                prioritized_tile.priority().priority_bin);
            bool skip_updating_last_tile = false;
            if (last_tile.priority().priority_bin == prioritized_tile.priority().priority_bin) {
                correct_order_tiles += last_tile.priority().distance_to_visible <= prioritized_tile.priority().distance_to_visible;
            } else if (prioritized_tile.priority().priority_bin == TilePriority::NOW) {
                // Since we'd return pending tree now tiles before the eventually tiles on
                // the active tree, update the value.
                ++correct_order_tiles;
                skip_updating_last_tile = true;
            }

            if (prioritized_tile.priority().priority_bin == TilePriority::NOW && last_tile.priority().resolution != prioritized_tile.priority().resolution) {
                // Low resolution should come first.
                EXPECT_EQ(LOW_RESOLUTION, last_tile.priority().resolution);
            }

            if (!skip_updating_last_tile)
                last_tile = prioritized_tile;
            ++tile_count;
            smoothness_tiles.insert(prioritized_tile.tile());
            if (prioritized_tile.tile()->required_for_draw())
                expected_required_for_draw_tiles.insert(prioritized_tile.tile());
            if (prioritized_tile.tile()->required_for_activation())
                expected_required_for_activation_tiles.insert(prioritized_tile.tile());
            queue->Pop();
        }

        EXPECT_EQ(tile_count, smoothness_tiles.size());
        EXPECT_EQ(all_tiles, smoothness_tiles);
        // Since we don't guarantee increasing distance due to spiral iterator, we
        // should check that we're _mostly_ right.
        EXPECT_GT(correct_order_tiles, 3 * tile_count / 4);

        // Check that we have consistent required_for_activation tiles.
        queue = host_impl_.BuildRasterQueue(
            SMOOTHNESS_TAKES_PRIORITY,
            RasterTilePriorityQueue::Type::REQUIRED_FOR_ACTIVATION);
        required_for_activation_tiles.clear();
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile()->required_for_activation());
            required_for_activation_tiles.insert(prioritized_tile.tile());
            queue->Pop();
        }
        EXPECT_EQ(expected_required_for_activation_tiles,
            required_for_activation_tiles);
        EXPECT_NE(all_tiles, required_for_activation_tiles);

        // Check that we have consistent required_for_draw tiles.
        queue = host_impl_.BuildRasterQueue(
            SMOOTHNESS_TAKES_PRIORITY,
            RasterTilePriorityQueue::Type::REQUIRED_FOR_DRAW);
        required_for_draw_tiles.clear();
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile()->required_for_draw());
            required_for_draw_tiles.insert(prioritized_tile.tile());
            queue->Pop();
        }
        EXPECT_EQ(expected_required_for_draw_tiles, required_for_draw_tiles);
        EXPECT_NE(all_tiles, required_for_draw_tiles);

        std::set<Tile*> new_content_tiles;
        last_tile = PrioritizedTile();
        size_t increasing_distance_tiles = 0u;
        // Here we expect to get increasing PENDING_TREE priority_bin.
        queue = host_impl_.BuildRasterQueue(NEW_CONTENT_TAKES_PRIORITY,
            RasterTilePriorityQueue::Type::ALL);
        tile_count = 0;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile());

            if (!last_tile.tile())
                last_tile = prioritized_tile;

            EXPECT_LE(last_tile.priority().priority_bin,
                prioritized_tile.priority().priority_bin);
            if (last_tile.priority().priority_bin == prioritized_tile.priority().priority_bin) {
                increasing_distance_tiles += last_tile.priority().distance_to_visible <= prioritized_tile.priority().distance_to_visible;
            }

            if (prioritized_tile.priority().priority_bin == TilePriority::NOW && last_tile.priority().resolution != prioritized_tile.priority().resolution) {
                // High resolution should come first.
                EXPECT_EQ(HIGH_RESOLUTION, last_tile.priority().resolution);
            }

            last_tile = prioritized_tile;
            new_content_tiles.insert(prioritized_tile.tile());
            ++tile_count;
            queue->Pop();
        }

        EXPECT_EQ(tile_count, new_content_tiles.size());
        EXPECT_EQ(high_res_tiles, new_content_tiles);
        // Since we don't guarantee increasing distance due to spiral iterator, we
        // should check that we're _mostly_ right.
        EXPECT_GE(increasing_distance_tiles, 3 * tile_count / 4);

        // Check that we have consistent required_for_activation tiles.
        queue = host_impl_.BuildRasterQueue(
            NEW_CONTENT_TAKES_PRIORITY,
            RasterTilePriorityQueue::Type::REQUIRED_FOR_ACTIVATION);
        required_for_activation_tiles.clear();
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile()->required_for_activation());
            required_for_activation_tiles.insert(prioritized_tile.tile());
            queue->Pop();
        }
        EXPECT_EQ(expected_required_for_activation_tiles,
            required_for_activation_tiles);
        EXPECT_NE(new_content_tiles, required_for_activation_tiles);

        // Check that we have consistent required_for_draw tiles.
        queue = host_impl_.BuildRasterQueue(
            NEW_CONTENT_TAKES_PRIORITY,
            RasterTilePriorityQueue::Type::REQUIRED_FOR_DRAW);
        required_for_draw_tiles.clear();
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile()->required_for_draw());
            required_for_draw_tiles.insert(prioritized_tile.tile());
            queue->Pop();
        }
        EXPECT_EQ(expected_required_for_draw_tiles, required_for_draw_tiles);
        EXPECT_NE(new_content_tiles, required_for_draw_tiles);
    }

    TEST_F(TileManagerTilePriorityQueueTest,
        RasterTilePriorityQueueHighNonIdealTilings)
    {
        const gfx::Size layer_bounds(1000, 1000);
        const gfx::Size viewport(800, 800);
        host_impl_.SetViewportSize(viewport);
        SetupDefaultTrees(layer_bounds);

        pending_layer_->tilings()->AddTiling(1.5f, pending_layer_->raster_source());
        active_layer_->tilings()->AddTiling(1.5f, active_layer_->raster_source());
        pending_layer_->tilings()->AddTiling(1.7f, pending_layer_->raster_source());
        active_layer_->tilings()->AddTiling(1.7f, active_layer_->raster_source());

        pending_layer_->tilings()->UpdateTilePriorities(gfx::Rect(viewport), 1.f, 5.0,
            Occlusion(), true);
        active_layer_->tilings()->UpdateTilePriorities(gfx::Rect(viewport), 1.f, 5.0,
            Occlusion(), true);

        std::set<Tile*> all_expected_tiles;
        for (size_t i = 0; i < pending_layer_->num_tilings(); ++i) {
            PictureLayerTiling* tiling = pending_layer_->tilings()->tiling_at(i);
            if (tiling->contents_scale() == 1.f) {
                tiling->set_resolution(HIGH_RESOLUTION);
                const auto& all_tiles = tiling->AllTilesForTesting();
                all_expected_tiles.insert(all_tiles.begin(), all_tiles.end());
            } else {
                tiling->set_resolution(NON_IDEAL_RESOLUTION);
            }
        }

        for (size_t i = 0; i < active_layer_->num_tilings(); ++i) {
            PictureLayerTiling* tiling = active_layer_->tilings()->tiling_at(i);
            if (tiling->contents_scale() == 1.5f) {
                tiling->set_resolution(HIGH_RESOLUTION);
                const auto& all_tiles = tiling->AllTilesForTesting();
                all_expected_tiles.insert(all_tiles.begin(), all_tiles.end());
            } else {
                tiling->set_resolution(NON_IDEAL_RESOLUTION);
                // Non ideal tilings with a high res pending twin have to be processed
                // because of possible activation tiles.
                if (tiling->contents_scale() == 1.f) {
                    tiling->UpdateAndGetAllPrioritizedTilesForTesting();
                    const auto& all_tiles = tiling->AllTilesForTesting();
                    for (auto* tile : all_tiles)
                        EXPECT_TRUE(tile->required_for_activation());
                    all_expected_tiles.insert(all_tiles.begin(), all_tiles.end());
                }
            }
        }

        scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
            SMOOTHNESS_TAKES_PRIORITY, RasterTilePriorityQueue::Type::ALL));
        EXPECT_FALSE(queue->IsEmpty());

        size_t tile_count = 0;
        std::set<Tile*> all_actual_tiles;
        while (!queue->IsEmpty()) {
            EXPECT_TRUE(queue->Top().tile());
            all_actual_tiles.insert(queue->Top().tile());
            ++tile_count;
            queue->Pop();
        }

        EXPECT_EQ(tile_count, all_actual_tiles.size());
        EXPECT_EQ(all_expected_tiles.size(), all_actual_tiles.size());
        EXPECT_EQ(all_expected_tiles, all_actual_tiles);
    }

    TEST_F(TileManagerTilePriorityQueueTest, RasterTilePriorityQueueInvalidation)
    {
        const gfx::Size layer_bounds(1000, 1000);
        host_impl_.SetViewportSize(gfx::Size(500, 500));
        SetupDefaultTrees(layer_bounds);

        // Use a tile's content rect as an invalidation. We should inset it a bit to
        // ensure that border math doesn't invalidate neighbouring tiles.
        gfx::Rect invalidation = active_layer_->HighResTiling()->TileAt(1, 0)->content_rect();
        invalidation.Inset(2, 2);

        pending_layer_->set_invalidation(invalidation);
        pending_layer_->HighResTiling()->Invalidate(invalidation);
        pending_layer_->HighResTiling()->CreateMissingTilesInLiveTilesRect();

        // Sanity checks: Tile at 0, 0 not exist on the pending tree (it's not
        // invalidated). Tile 1, 0 should exist on both.
        EXPECT_FALSE(pending_layer_->HighResTiling()->TileAt(0, 0));
        EXPECT_TRUE(active_layer_->HighResTiling()->TileAt(0, 0));
        EXPECT_TRUE(pending_layer_->HighResTiling()->TileAt(1, 0));
        EXPECT_TRUE(active_layer_->HighResTiling()->TileAt(1, 0));
        EXPECT_NE(pending_layer_->HighResTiling()->TileAt(1, 0),
            active_layer_->HighResTiling()->TileAt(1, 0));

        std::set<Tile*> expected_now_tiles;
        std::set<Tile*> expected_required_for_draw_tiles;
        std::set<Tile*> expected_required_for_activation_tiles;
        for (int i = 0; i <= 1; ++i) {
            for (int j = 0; j <= 1; ++j) {
                bool have_pending_tile = false;
                if (pending_layer_->HighResTiling()->TileAt(i, j)) {
                    expected_now_tiles.insert(
                        pending_layer_->HighResTiling()->TileAt(i, j));
                    expected_required_for_activation_tiles.insert(
                        pending_layer_->HighResTiling()->TileAt(i, j));
                    have_pending_tile = true;
                }
                Tile* active_tile = active_layer_->HighResTiling()->TileAt(i, j);
                EXPECT_TRUE(active_tile);
                expected_now_tiles.insert(active_tile);
                expected_required_for_draw_tiles.insert(active_tile);
                if (!have_pending_tile)
                    expected_required_for_activation_tiles.insert(active_tile);
            }
        }
        // Expect 3 shared tiles and 1 unshared tile in total.
        EXPECT_EQ(5u, expected_now_tiles.size());
        // Expect 4 tiles for each draw and activation, but not all the same.
        EXPECT_EQ(4u, expected_required_for_activation_tiles.size());
        EXPECT_EQ(4u, expected_required_for_draw_tiles.size());
        EXPECT_NE(expected_required_for_draw_tiles,
            expected_required_for_activation_tiles);

        std::set<Tile*> expected_all_tiles;
        for (int i = 0; i <= 3; ++i) {
            for (int j = 0; j <= 3; ++j) {
                if (pending_layer_->HighResTiling()->TileAt(i, j))
                    expected_all_tiles.insert(
                        pending_layer_->HighResTiling()->TileAt(i, j));
                EXPECT_TRUE(active_layer_->HighResTiling()->TileAt(i, j));
                expected_all_tiles.insert(active_layer_->HighResTiling()->TileAt(i, j));
            }
        }
        // Expect 15 shared tiles and 1 unshared tile.
        EXPECT_EQ(17u, expected_all_tiles.size());

        // The actual test will now build different queues and verify that the queues
        // return the same information as computed manually above.
        scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        std::set<Tile*> actual_now_tiles;
        std::set<Tile*> actual_all_tiles;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            queue->Pop();
            if (prioritized_tile.priority().priority_bin == TilePriority::NOW)
                actual_now_tiles.insert(prioritized_tile.tile());
            actual_all_tiles.insert(prioritized_tile.tile());
        }
        EXPECT_EQ(expected_now_tiles, actual_now_tiles);
        EXPECT_EQ(expected_all_tiles, actual_all_tiles);

        queue = host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES,
            RasterTilePriorityQueue::Type::REQUIRED_FOR_DRAW);
        std::set<Tile*> actual_required_for_draw_tiles;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            queue->Pop();
            actual_required_for_draw_tiles.insert(prioritized_tile.tile());
        }
        EXPECT_EQ(expected_required_for_draw_tiles, actual_required_for_draw_tiles);

        queue = host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES,
            RasterTilePriorityQueue::Type::REQUIRED_FOR_ACTIVATION);
        std::set<Tile*> actual_required_for_activation_tiles;
        while (!queue->IsEmpty()) {
            Tile* tile = queue->Top().tile();
            queue->Pop();
            actual_required_for_activation_tiles.insert(tile);
        }
        EXPECT_EQ(expected_required_for_activation_tiles,
            actual_required_for_activation_tiles);
    }

    TEST_F(TileManagerTilePriorityQueueTest, ActivationComesBeforeEventually)
    {
        host_impl_.AdvanceToNextFrame(base::TimeDelta::FromMilliseconds(1));

        gfx::Size layer_bounds(1000, 1000);
        SetupDefaultTrees(layer_bounds);

        // Create a pending child layer.
        scoped_refptr<FakeDisplayListRasterSource> pending_raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        scoped_ptr<FakePictureLayerImpl> pending_child = FakePictureLayerImpl::CreateWithRasterSource(
            host_impl_.pending_tree(), id_ + 1, pending_raster_source);
        FakePictureLayerImpl* pending_child_raw = pending_child.get();
        pending_child_raw->SetDrawsContent(true);
        pending_layer_->AddChild(pending_child.Pass());

        // Set a small viewport, so we have soon and eventually tiles.
        host_impl_.SetViewportSize(gfx::Size(200, 200));
        host_impl_.AdvanceToNextFrame(base::TimeDelta::FromMilliseconds(1));
        bool update_lcd_text = false;
        host_impl_.pending_tree()->UpdateDrawProperties(update_lcd_text);

        host_impl_.SetRequiresHighResToDraw();
        scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
            SMOOTHNESS_TAKES_PRIORITY, RasterTilePriorityQueue::Type::ALL));
        EXPECT_FALSE(queue->IsEmpty());

        // Get all the tiles that are NOW or SOON and make sure they are ready to
        // draw.
        std::vector<Tile*> all_tiles;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            if (prioritized_tile.priority().priority_bin >= TilePriority::EVENTUALLY)
                break;

            all_tiles.push_back(prioritized_tile.tile());
            queue->Pop();
        }

        tile_manager()->InitializeTilesWithResourcesForTesting(
            std::vector<Tile*>(all_tiles.begin(), all_tiles.end()));

        // Ensure we can activate.
        EXPECT_TRUE(tile_manager()->IsReadyToActivate());
    }

    TEST_F(TileManagerTilePriorityQueueTest, EvictionTilePriorityQueue)
    {
        const gfx::Size layer_bounds(1000, 1000);
        host_impl_.SetViewportSize(layer_bounds);
        SetupDefaultTrees(layer_bounds);
        ASSERT_TRUE(active_layer_->HighResTiling());
        ASSERT_TRUE(active_layer_->LowResTiling());
        ASSERT_TRUE(pending_layer_->HighResTiling());
        EXPECT_FALSE(pending_layer_->LowResTiling());

        scoped_ptr<EvictionTilePriorityQueue> empty_queue(
            host_impl_.BuildEvictionQueue(SAME_PRIORITY_FOR_BOTH_TREES));
        EXPECT_TRUE(empty_queue->IsEmpty());
        std::set<Tile*> all_tiles;
        size_t tile_count = 0;

        scoped_ptr<RasterTilePriorityQueue> raster_queue(host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        while (!raster_queue->IsEmpty()) {
            ++tile_count;
            EXPECT_TRUE(raster_queue->Top().tile());
            all_tiles.insert(raster_queue->Top().tile());
            raster_queue->Pop();
        }

        EXPECT_EQ(tile_count, all_tiles.size());
        EXPECT_EQ(16u, tile_count);

        tile_manager()->InitializeTilesWithResourcesForTesting(
            std::vector<Tile*>(all_tiles.begin(), all_tiles.end()));

        scoped_ptr<EvictionTilePriorityQueue> queue(
            host_impl_.BuildEvictionQueue(SMOOTHNESS_TAKES_PRIORITY));
        EXPECT_FALSE(queue->IsEmpty());

        // Sanity check, all tiles should be visible.
        std::set<Tile*> smoothness_tiles;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            EXPECT_TRUE(prioritized_tile.tile());
            EXPECT_EQ(TilePriority::NOW, prioritized_tile.priority().priority_bin);
            EXPECT_TRUE(prioritized_tile.tile()->draw_info().has_resource());
            smoothness_tiles.insert(prioritized_tile.tile());
            queue->Pop();
        }
        EXPECT_EQ(all_tiles, smoothness_tiles);

        tile_manager()->ReleaseTileResourcesForTesting(
            std::vector<Tile*>(all_tiles.begin(), all_tiles.end()));

        Region invalidation(gfx::Rect(0, 0, 500, 500));

        // Invalidate the pending tree.
        pending_layer_->set_invalidation(invalidation);
        pending_layer_->HighResTiling()->Invalidate(invalidation);
        pending_layer_->HighResTiling()->CreateMissingTilesInLiveTilesRect();
        EXPECT_FALSE(pending_layer_->LowResTiling());

        // Renew all of the tile priorities.
        gfx::Rect viewport(50, 50, 100, 100);
        pending_layer_->HighResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());
        active_layer_->HighResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());
        active_layer_->LowResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());

        // Populate all tiles directly from the tilings.
        all_tiles.clear();
        std::vector<Tile*> pending_high_res_tiles = pending_layer_->HighResTiling()->AllTilesForTesting();
        for (size_t i = 0; i < pending_high_res_tiles.size(); ++i)
            all_tiles.insert(pending_high_res_tiles[i]);

        std::vector<Tile*> active_high_res_tiles = active_layer_->HighResTiling()->AllTilesForTesting();
        for (size_t i = 0; i < active_high_res_tiles.size(); ++i)
            all_tiles.insert(active_high_res_tiles[i]);

        std::vector<Tile*> active_low_res_tiles = active_layer_->LowResTiling()->AllTilesForTesting();
        for (size_t i = 0; i < active_low_res_tiles.size(); ++i)
            all_tiles.insert(active_low_res_tiles[i]);

        tile_manager()->InitializeTilesWithResourcesForTesting(
            std::vector<Tile*>(all_tiles.begin(), all_tiles.end()));

        PrioritizedTile last_tile;
        smoothness_tiles.clear();
        tile_count = 0;
        // Here we expect to get increasing combined priority_bin.
        queue = host_impl_.BuildEvictionQueue(SMOOTHNESS_TAKES_PRIORITY);
        int distance_increasing = 0;
        int distance_decreasing = 0;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            Tile* tile = prioritized_tile.tile();
            EXPECT_TRUE(tile);
            EXPECT_TRUE(tile->draw_info().has_resource());

            if (!last_tile.tile())
                last_tile = prioritized_tile;

            const TilePriority& last_priority = last_tile.priority();
            const TilePriority& priority = prioritized_tile.priority();

            EXPECT_GE(last_priority.priority_bin, priority.priority_bin);
            if (last_priority.priority_bin == priority.priority_bin) {
                EXPECT_LE(last_tile.tile()->required_for_activation(),
                    tile->required_for_activation());
                if (last_tile.tile()->required_for_activation() == tile->required_for_activation()) {
                    if (last_priority.distance_to_visible >= priority.distance_to_visible)
                        ++distance_decreasing;
                    else
                        ++distance_increasing;
                }
            }

            last_tile = prioritized_tile;
            ++tile_count;
            smoothness_tiles.insert(tile);
            queue->Pop();
        }

        // Ensure that the distance is decreasing many more times than increasing.
        EXPECT_EQ(3, distance_increasing);
        EXPECT_EQ(16, distance_decreasing);
        EXPECT_EQ(tile_count, smoothness_tiles.size());
        EXPECT_EQ(all_tiles, smoothness_tiles);

        std::set<Tile*> new_content_tiles;
        last_tile = PrioritizedTile();
        // Again, we expect to get increasing combined priority_bin.
        queue = host_impl_.BuildEvictionQueue(NEW_CONTENT_TAKES_PRIORITY);
        distance_decreasing = 0;
        distance_increasing = 0;
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            Tile* tile = prioritized_tile.tile();
            EXPECT_TRUE(tile);

            if (!last_tile.tile())
                last_tile = prioritized_tile;

            const TilePriority& last_priority = last_tile.priority();
            const TilePriority& priority = prioritized_tile.priority();

            EXPECT_GE(last_priority.priority_bin, priority.priority_bin);
            if (last_priority.priority_bin == priority.priority_bin) {
                EXPECT_LE(last_tile.tile()->required_for_activation(),
                    tile->required_for_activation());
                if (last_tile.tile()->required_for_activation() == tile->required_for_activation()) {
                    if (last_priority.distance_to_visible >= priority.distance_to_visible)
                        ++distance_decreasing;
                    else
                        ++distance_increasing;
                }
            }

            last_tile = prioritized_tile;
            new_content_tiles.insert(tile);
            queue->Pop();
        }

        // Ensure that the distance is decreasing many more times than increasing.
        EXPECT_EQ(3, distance_increasing);
        EXPECT_EQ(16, distance_decreasing);
        EXPECT_EQ(tile_count, new_content_tiles.size());
        EXPECT_EQ(all_tiles, new_content_tiles);
    }

    TEST_F(TileManagerTilePriorityQueueTest,
        EvictionTilePriorityQueueWithOcclusion)
    {
        host_impl_.AdvanceToNextFrame(base::TimeDelta::FromMilliseconds(1));

        gfx::Size layer_bounds(1000, 1000);

        host_impl_.SetViewportSize(layer_bounds);

        scoped_refptr<FakeDisplayListRasterSource> pending_raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        SetupPendingTree(pending_raster_source);

        scoped_ptr<FakePictureLayerImpl> pending_child = FakePictureLayerImpl::CreateWithRasterSource(host_impl_.pending_tree(), 2,
            pending_raster_source);
        pending_layer_->AddChild(pending_child.Pass());

        FakePictureLayerImpl* pending_child_layer = static_cast<FakePictureLayerImpl*>(pending_layer_->children()[0]);
        pending_child_layer->SetDrawsContent(true);

        host_impl_.AdvanceToNextFrame(base::TimeDelta::FromMilliseconds(1));
        bool update_lcd_text = false;
        host_impl_.pending_tree()->UpdateDrawProperties(update_lcd_text);

        ActivateTree();
        SetupPendingTree(pending_raster_source);

        FakePictureLayerImpl* active_child_layer = static_cast<FakePictureLayerImpl*>(active_layer_->children()[0]);

        std::set<Tile*> all_tiles;
        size_t tile_count = 0;
        scoped_ptr<RasterTilePriorityQueue> raster_queue(host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        while (!raster_queue->IsEmpty()) {
            ++tile_count;
            EXPECT_TRUE(raster_queue->Top().tile());
            all_tiles.insert(raster_queue->Top().tile());
            raster_queue->Pop();
        }
        EXPECT_EQ(tile_count, all_tiles.size());
        EXPECT_EQ(32u, tile_count);

        // Renew all of the tile priorities.
        gfx::Rect viewport(layer_bounds);
        pending_layer_->HighResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());
        pending_child_layer->HighResTiling()->ComputeTilePriorityRects(
            viewport, 1.0f, 1.0, Occlusion());

        active_layer_->HighResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());
        active_layer_->LowResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());
        active_child_layer->HighResTiling()->ComputeTilePriorityRects(
            viewport, 1.0f, 1.0, Occlusion());
        active_child_layer->LowResTiling()->ComputeTilePriorityRects(
            viewport, 1.0f, 1.0, Occlusion());

        // Populate all tiles directly from the tilings.
        all_tiles.clear();
        std::vector<Tile*> pending_high_res_tiles = pending_layer_->HighResTiling()->AllTilesForTesting();
        all_tiles.insert(pending_high_res_tiles.begin(),
            pending_high_res_tiles.end());

        // Set all tiles on the pending_child_layer as occluded on the pending tree.
        std::vector<Tile*> pending_child_high_res_tiles = pending_child_layer->HighResTiling()->AllTilesForTesting();
        pending_child_layer->HighResTiling()->SetAllTilesOccludedForTesting();
        active_child_layer->HighResTiling()->SetAllTilesOccludedForTesting();
        active_child_layer->LowResTiling()->SetAllTilesOccludedForTesting();

        tile_manager()->InitializeTilesWithResourcesForTesting(
            std::vector<Tile*>(all_tiles.begin(), all_tiles.end()));

        // Verify occlusion is considered by EvictionTilePriorityQueue.
        TreePriority tree_priority = NEW_CONTENT_TAKES_PRIORITY;
        size_t occluded_count = 0u;
        PrioritizedTile last_tile;
        scoped_ptr<EvictionTilePriorityQueue> queue(
            host_impl_.BuildEvictionQueue(tree_priority));
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            if (!last_tile.tile())
                last_tile = prioritized_tile;

            bool tile_is_occluded = prioritized_tile.is_occluded();

            // The only way we will encounter an occluded tile after an unoccluded
            // tile is if the priorty bin decreased, the tile is required for
            // activation, or the scale changed.
            if (tile_is_occluded) {
                occluded_count++;

                bool last_tile_is_occluded = last_tile.is_occluded();
                if (!last_tile_is_occluded) {
                    TilePriority::PriorityBin tile_priority_bin = prioritized_tile.priority().priority_bin;
                    TilePriority::PriorityBin last_tile_priority_bin = last_tile.priority().priority_bin;

                    EXPECT_TRUE((tile_priority_bin < last_tile_priority_bin) || prioritized_tile.tile()->required_for_activation() || (prioritized_tile.tile()->contents_scale() != last_tile.tile()->contents_scale()));
                }
            }
            last_tile = prioritized_tile;
            queue->Pop();
        }
        size_t expected_occluded_count = pending_child_high_res_tiles.size();
        EXPECT_EQ(expected_occluded_count, occluded_count);
    }

    TEST_F(TileManagerTilePriorityQueueTest,
        EvictionTilePriorityQueueWithTransparentLayer)
    {
        host_impl_.AdvanceToNextFrame(base::TimeDelta::FromMilliseconds(1));

        gfx::Size layer_bounds(1000, 1000);

        scoped_refptr<FakeDisplayListRasterSource> pending_raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        SetupPendingTree(pending_raster_source);

        scoped_ptr<FakePictureLayerImpl> pending_child = FakePictureLayerImpl::CreateWithRasterSource(host_impl_.pending_tree(), 2,
            pending_raster_source);
        FakePictureLayerImpl* pending_child_layer = pending_child.get();
        pending_layer_->AddChild(pending_child.Pass());

        // Create a fully transparent child layer so that its tile priorities are not
        // considered to be valid.
        pending_child_layer->SetDrawsContent(true);

        host_impl_.AdvanceToNextFrame(base::TimeDelta::FromMilliseconds(1));
        bool update_lcd_text = false;
        host_impl_.pending_tree()->UpdateDrawProperties(update_lcd_text);

        pending_child_layer->SetOpacity(0.0);

        host_impl_.AdvanceToNextFrame(base::TimeDelta::FromMilliseconds(1));
        host_impl_.pending_tree()->UpdateDrawProperties(update_lcd_text);

        // Renew all of the tile priorities.
        gfx::Rect viewport(layer_bounds);
        pending_layer_->HighResTiling()->ComputeTilePriorityRects(viewport, 1.0f, 1.0,
            Occlusion());
        pending_child_layer->HighResTiling()->ComputeTilePriorityRects(
            viewport, 1.0f, 1.0, Occlusion());

        // Populate all tiles directly from the tilings.
        std::set<Tile*> all_pending_tiles;
        std::vector<Tile*> pending_high_res_tiles = pending_layer_->HighResTiling()->AllTilesForTesting();
        all_pending_tiles.insert(pending_high_res_tiles.begin(),
            pending_high_res_tiles.end());
        EXPECT_EQ(16u, pending_high_res_tiles.size());

        std::set<Tile*> all_pending_child_tiles;
        std::vector<Tile*> pending_child_high_res_tiles = pending_child_layer->HighResTiling()->AllTilesForTesting();
        all_pending_child_tiles.insert(pending_child_high_res_tiles.begin(),
            pending_child_high_res_tiles.end());
        EXPECT_EQ(16u, pending_child_high_res_tiles.size());

        std::set<Tile*> all_tiles = all_pending_tiles;
        all_tiles.insert(all_pending_child_tiles.begin(),
            all_pending_child_tiles.end());

        tile_manager()->InitializeTilesWithResourcesForTesting(
            std::vector<Tile*>(all_tiles.begin(), all_tiles.end()));

        EXPECT_TRUE(pending_layer_->HasValidTilePriorities());
        EXPECT_FALSE(pending_child_layer->HasValidTilePriorities());

        // Verify that eviction queue returns tiles also from layers without valid
        // tile priorities and that the tile priority bin of those tiles is (at most)
        // EVENTUALLY.
        TreePriority tree_priority = NEW_CONTENT_TAKES_PRIORITY;
        std::set<Tile*> new_content_tiles;
        size_t tile_count = 0;
        scoped_ptr<EvictionTilePriorityQueue> queue(
            host_impl_.BuildEvictionQueue(tree_priority));
        while (!queue->IsEmpty()) {
            PrioritizedTile prioritized_tile = queue->Top();
            Tile* tile = prioritized_tile.tile();
            const TilePriority& pending_priority = prioritized_tile.priority();
            EXPECT_NE(std::numeric_limits<float>::infinity(),
                pending_priority.distance_to_visible);
            if (all_pending_child_tiles.find(tile) != all_pending_child_tiles.end())
                EXPECT_EQ(TilePriority::EVENTUALLY, pending_priority.priority_bin);
            else
                EXPECT_EQ(TilePriority::NOW, pending_priority.priority_bin);
            new_content_tiles.insert(tile);
            ++tile_count;
            queue->Pop();
        }
        EXPECT_EQ(tile_count, new_content_tiles.size());
        EXPECT_EQ(all_tiles, new_content_tiles);
    }

    TEST_F(TileManagerTilePriorityQueueTest, RasterTilePriorityQueueEmptyLayers)
    {
        const gfx::Size layer_bounds(1000, 1000);
        host_impl_.SetViewportSize(layer_bounds);
        SetupDefaultTrees(layer_bounds);

        scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        EXPECT_FALSE(queue->IsEmpty());

        size_t tile_count = 0;
        std::set<Tile*> all_tiles;
        while (!queue->IsEmpty()) {
            EXPECT_TRUE(queue->Top().tile());
            all_tiles.insert(queue->Top().tile());
            ++tile_count;
            queue->Pop();
        }

        EXPECT_EQ(tile_count, all_tiles.size());
        EXPECT_EQ(16u, tile_count);

        for (int i = 1; i < 10; ++i) {
            scoped_ptr<FakePictureLayerImpl> pending_layer = FakePictureLayerImpl::Create(host_impl_.pending_tree(), id_ + i);
            pending_layer->SetDrawsContent(true);
            pending_layer->set_has_valid_tile_priorities(true);
            pending_layer_->AddChild(pending_layer.Pass());
        }

        queue = host_impl_.BuildRasterQueue(SAME_PRIORITY_FOR_BOTH_TREES,
            RasterTilePriorityQueue::Type::ALL);
        EXPECT_FALSE(queue->IsEmpty());

        tile_count = 0;
        all_tiles.clear();
        while (!queue->IsEmpty()) {
            EXPECT_TRUE(queue->Top().tile());
            all_tiles.insert(queue->Top().tile());
            ++tile_count;
            queue->Pop();
        }
        EXPECT_EQ(tile_count, all_tiles.size());
        EXPECT_EQ(16u, tile_count);
    }

    TEST_F(TileManagerTilePriorityQueueTest, EvictionTilePriorityQueueEmptyLayers)
    {
        const gfx::Size layer_bounds(1000, 1000);
        host_impl_.SetViewportSize(layer_bounds);
        SetupDefaultTrees(layer_bounds);

        scoped_ptr<RasterTilePriorityQueue> raster_queue(host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        EXPECT_FALSE(raster_queue->IsEmpty());

        size_t tile_count = 0;
        std::set<Tile*> all_tiles;
        while (!raster_queue->IsEmpty()) {
            EXPECT_TRUE(raster_queue->Top().tile());
            all_tiles.insert(raster_queue->Top().tile());
            ++tile_count;
            raster_queue->Pop();
        }
        EXPECT_EQ(tile_count, all_tiles.size());
        EXPECT_EQ(16u, tile_count);

        std::vector<Tile*> tiles(all_tiles.begin(), all_tiles.end());
        host_impl_.tile_manager()->InitializeTilesWithResourcesForTesting(tiles);

        for (int i = 1; i < 10; ++i) {
            scoped_ptr<FakePictureLayerImpl> pending_layer = FakePictureLayerImpl::Create(host_impl_.pending_tree(), id_ + i);
            pending_layer->SetDrawsContent(true);
            pending_layer->set_has_valid_tile_priorities(true);
            pending_layer_->AddChild(pending_layer.Pass());
        }

        scoped_ptr<EvictionTilePriorityQueue> queue(
            host_impl_.BuildEvictionQueue(SAME_PRIORITY_FOR_BOTH_TREES));
        EXPECT_FALSE(queue->IsEmpty());

        tile_count = 0;
        all_tiles.clear();
        while (!queue->IsEmpty()) {
            EXPECT_TRUE(queue->Top().tile());
            all_tiles.insert(queue->Top().tile());
            ++tile_count;
            queue->Pop();
        }
        EXPECT_EQ(tile_count, all_tiles.size());
        EXPECT_EQ(16u, tile_count);
    }

    TEST_F(TileManagerTilePriorityQueueTest,
        RasterTilePriorityQueueStaticViewport)
    {
        FakePictureLayerTilingClient client;

        gfx::Rect viewport(50, 50, 500, 500);
        gfx::Size layer_bounds(1600, 1600);

        float inset = PictureLayerTiling::CalculateSoonBorderDistance(viewport, 1.0f);
        gfx::Rect soon_rect = viewport;
        soon_rect.Inset(-inset, -inset);

        client.SetTileSize(gfx::Size(30, 30));
        LayerTreeSettings settings;

        scoped_ptr<PictureLayerTilingSet> tiling_set = PictureLayerTilingSet::Create(
            ACTIVE_TREE, &client, settings.tiling_interest_area_padding,
            settings.skewport_target_time_in_seconds,
            settings.skewport_extrapolation_limit_in_content_pixels);

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        PictureLayerTiling* tiling = tiling_set->AddTiling(1.0f, raster_source);
        tiling->set_resolution(HIGH_RESOLUTION);

        tiling_set->UpdateTilePriorities(viewport, 1.0f, 1.0, Occlusion(), true);
        std::vector<Tile*> all_tiles = tiling->AllTilesForTesting();
        // Sanity check.
        EXPECT_EQ(3364u, all_tiles.size());

        // The explanation of each iteration is as follows:
        // 1. First iteration tests that we can get all of the tiles correctly.
        // 2. Second iteration ensures that we can get all of the tiles again (first
        //    iteration didn't change any tiles), as well set all tiles to be ready to
        //    draw.
        // 3. Third iteration ensures that no tiles are returned, since they were all
        //    marked as ready to draw.
        for (int i = 0; i < 3; ++i) {
            scoped_ptr<TilingSetRasterQueueAll> queue(
                new TilingSetRasterQueueAll(tiling_set.get(), false));

            // There are 3 bins in TilePriority.
            bool have_tiles[3] = {};

            // On the third iteration, we should get no tiles since everything was
            // marked as ready to draw.
            if (i == 2) {
                EXPECT_TRUE(queue->IsEmpty());
                continue;
            }

            EXPECT_FALSE(queue->IsEmpty());
            std::set<Tile*> unique_tiles;
            unique_tiles.insert(queue->Top().tile());
            PrioritizedTile last_tile = queue->Top();
            have_tiles[last_tile.priority().priority_bin] = true;

            // On the second iteration, mark everything as ready to draw (solid color).
            if (i == 1) {
                TileDrawInfo& draw_info = last_tile.tile()->draw_info();
                draw_info.SetSolidColorForTesting(SK_ColorRED);
            }
            queue->Pop();
            int eventually_bin_order_correct_count = 0;
            int eventually_bin_order_incorrect_count = 0;
            while (!queue->IsEmpty()) {
                PrioritizedTile new_tile = queue->Top();
                queue->Pop();
                unique_tiles.insert(new_tile.tile());

                TilePriority last_priority = last_tile.priority();
                TilePriority new_priority = new_tile.priority();
                EXPECT_LE(last_priority.priority_bin, new_priority.priority_bin);
                if (last_priority.priority_bin == new_priority.priority_bin) {
                    if (last_priority.priority_bin == TilePriority::EVENTUALLY) {
                        bool order_correct = last_priority.distance_to_visible <= new_priority.distance_to_visible;
                        eventually_bin_order_correct_count += order_correct;
                        eventually_bin_order_incorrect_count += !order_correct;
                    } else if (!soon_rect.Intersects(new_tile.tile()->content_rect()) && !soon_rect.Intersects(last_tile.tile()->content_rect())) {
                        EXPECT_LE(last_priority.distance_to_visible,
                            new_priority.distance_to_visible);
                        EXPECT_EQ(TilePriority::NOW, new_priority.priority_bin);
                    } else if (new_priority.distance_to_visible > 0.f) {
                        EXPECT_EQ(TilePriority::SOON, new_priority.priority_bin);
                    }
                }
                have_tiles[new_priority.priority_bin] = true;

                last_tile = new_tile;

                // On the second iteration, mark everything as ready to draw (solid
                // color).
                if (i == 1) {
                    TileDrawInfo& draw_info = last_tile.tile()->draw_info();
                    draw_info.SetSolidColorForTesting(SK_ColorRED);
                }
            }

            EXPECT_GT(eventually_bin_order_correct_count,
                eventually_bin_order_incorrect_count);

            // We should have now and eventually tiles, as well as soon tiles from
            // the border region.
            EXPECT_TRUE(have_tiles[TilePriority::NOW]);
            EXPECT_TRUE(have_tiles[TilePriority::SOON]);
            EXPECT_TRUE(have_tiles[TilePriority::EVENTUALLY]);

            EXPECT_EQ(unique_tiles.size(), all_tiles.size());
        }
    }

    TEST_F(TileManagerTilePriorityQueueTest,
        RasterTilePriorityQueueMovingViewport)
    {
        FakePictureLayerTilingClient client;

        gfx::Rect viewport(50, 0, 100, 100);
        gfx::Rect moved_viewport(50, 0, 100, 500);
        gfx::Size layer_bounds(1000, 1000);

        client.SetTileSize(gfx::Size(30, 30));
        LayerTreeSettings settings;

        scoped_ptr<PictureLayerTilingSet> tiling_set = PictureLayerTilingSet::Create(
            ACTIVE_TREE, &client, settings.tiling_interest_area_padding,
            settings.skewport_target_time_in_seconds,
            settings.skewport_extrapolation_limit_in_content_pixels);

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);
        PictureLayerTiling* tiling = tiling_set->AddTiling(1.0f, raster_source);
        tiling->set_resolution(HIGH_RESOLUTION);

        tiling_set->UpdateTilePriorities(viewport, 1.0f, 1.0, Occlusion(), true);
        tiling_set->UpdateTilePriorities(moved_viewport, 1.0f, 2.0, Occlusion(),
            true);

        float inset = PictureLayerTiling::CalculateSoonBorderDistance(moved_viewport, 1.0f);
        gfx::Rect soon_rect = moved_viewport;
        soon_rect.Inset(-inset, -inset);

        // There are 3 bins in TilePriority.
        bool have_tiles[3] = {};
        PrioritizedTile last_tile;
        int eventually_bin_order_correct_count = 0;
        int eventually_bin_order_incorrect_count = 0;
        scoped_ptr<TilingSetRasterQueueAll> queue(
            new TilingSetRasterQueueAll(tiling_set.get(), false));
        for (; !queue->IsEmpty(); queue->Pop()) {
            if (!last_tile.tile())
                last_tile = queue->Top();

            const PrioritizedTile& new_tile = queue->Top();

            TilePriority last_priority = last_tile.priority();
            TilePriority new_priority = new_tile.priority();

            have_tiles[new_priority.priority_bin] = true;

            EXPECT_LE(last_priority.priority_bin, new_priority.priority_bin);
            if (last_priority.priority_bin == new_priority.priority_bin) {
                if (last_priority.priority_bin == TilePriority::EVENTUALLY) {
                    bool order_correct = last_priority.distance_to_visible <= new_priority.distance_to_visible;
                    eventually_bin_order_correct_count += order_correct;
                    eventually_bin_order_incorrect_count += !order_correct;
                } else if (!soon_rect.Intersects(new_tile.tile()->content_rect()) && !soon_rect.Intersects(last_tile.tile()->content_rect())) {
                    EXPECT_LE(last_priority.distance_to_visible,
                        new_priority.distance_to_visible);
                } else if (new_priority.distance_to_visible > 0.f) {
                    EXPECT_EQ(TilePriority::SOON, new_priority.priority_bin);
                }
            }
            last_tile = new_tile;
        }

        EXPECT_GT(eventually_bin_order_correct_count,
            eventually_bin_order_incorrect_count);

        EXPECT_TRUE(have_tiles[TilePriority::NOW]);
        EXPECT_TRUE(have_tiles[TilePriority::SOON]);
        EXPECT_TRUE(have_tiles[TilePriority::EVENTUALLY]);
    }

    TEST_F(TileManagerTilePriorityQueueTest, SetIsLikelyToRequireADraw)
    {
        const gfx::Size layer_bounds(1000, 1000);
        host_impl_.SetViewportSize(layer_bounds);
        SetupDefaultTrees(layer_bounds);

        // Verify that the queue has a required for draw tile at Top.
        scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        EXPECT_FALSE(queue->IsEmpty());
        EXPECT_TRUE(queue->Top().tile()->required_for_draw());

        EXPECT_FALSE(host_impl_.is_likely_to_require_a_draw());
        host_impl_.tile_manager()->PrepareTiles(host_impl_.global_tile_state());
        EXPECT_TRUE(host_impl_.is_likely_to_require_a_draw());
    }

    TEST_F(TileManagerTilePriorityQueueTest,
        SetIsLikelyToRequireADrawOnZeroMemoryBudget)
    {
        const gfx::Size layer_bounds(1000, 1000);
        host_impl_.SetViewportSize(layer_bounds);
        SetupDefaultTrees(layer_bounds);

        // Verify that the queue has a required for draw tile at Top.
        scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        EXPECT_FALSE(queue->IsEmpty());
        EXPECT_TRUE(queue->Top().tile()->required_for_draw());

        ManagedMemoryPolicy policy = host_impl_.ActualManagedMemoryPolicy();
        policy.bytes_limit_when_visible = 0;
        host_impl_.SetMemoryPolicy(policy);

        EXPECT_FALSE(host_impl_.is_likely_to_require_a_draw());
        host_impl_.tile_manager()->PrepareTiles(host_impl_.global_tile_state());
        EXPECT_FALSE(host_impl_.is_likely_to_require_a_draw());
    }

    TEST_F(TileManagerTilePriorityQueueTest,
        SetIsLikelyToRequireADrawOnLimitedMemoryBudget)
    {
        const gfx::Size layer_bounds(1000, 1000);
        host_impl_.SetViewportSize(layer_bounds);
        SetupDefaultTrees(layer_bounds);

        // Verify that the queue has a required for draw tile at Top.
        scoped_ptr<RasterTilePriorityQueue> queue(host_impl_.BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        EXPECT_FALSE(queue->IsEmpty());
        EXPECT_TRUE(queue->Top().tile()->required_for_draw());
        EXPECT_EQ(gfx::Size(256, 256), queue->Top().tile()->desired_texture_size());
        EXPECT_EQ(RGBA_8888, host_impl_.resource_provider()->best_texture_format());

        ManagedMemoryPolicy policy = host_impl_.ActualManagedMemoryPolicy();
        policy.bytes_limit_when_visible = ResourceUtil::UncheckedSizeInBytes<size_t>(
            gfx::Size(256, 256), RGBA_8888);
        host_impl_.SetMemoryPolicy(policy);

        EXPECT_FALSE(host_impl_.is_likely_to_require_a_draw());
        host_impl_.tile_manager()->PrepareTiles(host_impl_.global_tile_state());
        EXPECT_TRUE(host_impl_.is_likely_to_require_a_draw());

        Resource* resource = host_impl_.resource_pool()->AcquireResource(
            gfx::Size(256, 256), RGBA_8888);

        host_impl_.tile_manager()->CheckIfMoreTilesNeedToBePreparedForTesting();
        EXPECT_FALSE(host_impl_.is_likely_to_require_a_draw());

        host_impl_.resource_pool()->ReleaseResource(resource, 0);
    }

    TEST_F(TileManagerTilePriorityQueueTest, RasterQueueAllUsesCorrectTileBounds)
    {
        // Verify that we use the real tile bounds when advancing phases during the
        // tile iteration.
        gfx::Size layer_bounds(1, 1);

        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFilled(layer_bounds);

        FakePictureLayerTilingClient pending_client;
        pending_client.SetTileSize(gfx::Size(64, 64));

        scoped_ptr<PictureLayerTilingSet> tiling_set = PictureLayerTilingSet::Create(
            WhichTree::ACTIVE_TREE, &pending_client, 1.0f, 1.0f, 1000);
        pending_client.set_twin_tiling_set(tiling_set.get());

        auto* tiling = tiling_set->AddTiling(1.0f, raster_source);

        tiling->set_resolution(HIGH_RESOLUTION);
        tiling->CreateAllTilesForTesting();

        // The tile is (0, 0, 1, 1), create an intersecting and non-intersecting
        // rectangle to test the advance phase with. The tile size is (64, 64), so
        // both rectangles intersect the tile content size, but only one should
        // intersect the actual size.
        gfx::Rect non_intersecting_rect(2, 2, 10, 10);
        gfx::Rect intersecting_rect(0, 0, 10, 10);
        {
            tiling->SetTilePriorityRectsForTesting(
                non_intersecting_rect, // Visible rect.
                intersecting_rect, // Skewport rect.
                intersecting_rect, // Soon rect.
                intersecting_rect); // Eventually rect.
            scoped_ptr<TilingSetRasterQueueAll> queue(
                new TilingSetRasterQueueAll(tiling_set.get(), false));
            EXPECT_FALSE(queue->IsEmpty());
        }
        {
            tiling->SetTilePriorityRectsForTesting(
                non_intersecting_rect, // Visible rect.
                non_intersecting_rect, // Skewport rect.
                intersecting_rect, // Soon rect.
                intersecting_rect); // Eventually rect.
            scoped_ptr<TilingSetRasterQueueAll> queue(
                new TilingSetRasterQueueAll(tiling_set.get(), false));
            EXPECT_FALSE(queue->IsEmpty());
        }
        {
            tiling->SetTilePriorityRectsForTesting(
                non_intersecting_rect, // Visible rect.
                non_intersecting_rect, // Skewport rect.
                non_intersecting_rect, // Soon rect.
                intersecting_rect); // Eventually rect.
            scoped_ptr<TilingSetRasterQueueAll> queue(
                new TilingSetRasterQueueAll(tiling_set.get(), false));
            EXPECT_FALSE(queue->IsEmpty());
        }
    }

    class TileManagerTest : public testing::Test {
    public:
        TileManagerTest()
            : output_surface_(FakeOutputSurface::CreateSoftware(
                make_scoped_ptr(new SoftwareOutputDevice)))
            , host_impl_(new MockLayerTreeHostImpl(&proxy_,
                  &shared_bitmap_manager_,
                  &task_graph_runner_))
        {
            host_impl_->InitializeRenderer(output_surface_.get());
        }

    protected:
        // MockLayerTreeHostImpl allows us to intercept tile manager callbacks.
        class MockLayerTreeHostImpl : public FakeLayerTreeHostImpl {
        public:
            MockLayerTreeHostImpl(Proxy* proxy,
                SharedBitmapManager* manager,
                TaskGraphRunner* task_graph_runner)
                : FakeLayerTreeHostImpl(proxy, manager, task_graph_runner)
            {
            }

            MOCK_METHOD0(NotifyAllTileTasksCompleted, void());
        };

        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        FakeImplProxy proxy_;
        scoped_ptr<OutputSurface> output_surface_;
        scoped_ptr<MockLayerTreeHostImpl> host_impl_;
    };

    // Test to ensure that we call NotifyAllTileTasksCompleted when PrepareTiles is
    // called.
    TEST_F(TileManagerTest, AllWorkFinishedTest)
    {
        // Check with no tile work enqueued.
        {
            base::RunLoop run_loop;
            EXPECT_FALSE(host_impl_->tile_manager()->HasScheduledTileTasksForTesting());
            EXPECT_CALL(*host_impl_, NotifyAllTileTasksCompleted())
                .WillOnce(testing::Invoke([&run_loop]() { run_loop.Quit(); }));
            host_impl_->tile_manager()->PrepareTiles(host_impl_->global_tile_state());
            EXPECT_TRUE(host_impl_->tile_manager()->HasScheduledTileTasksForTesting());
            run_loop.Run();
        }

        // Check that the "schedule more work" path also triggers the expected
        // callback.
        {
            base::RunLoop run_loop;
            EXPECT_FALSE(host_impl_->tile_manager()->HasScheduledTileTasksForTesting());
            EXPECT_CALL(*host_impl_, NotifyAllTileTasksCompleted())
                .WillOnce(testing::Invoke([&run_loop]() { run_loop.Quit(); }));
            host_impl_->tile_manager()->PrepareTiles(host_impl_->global_tile_state());
            host_impl_->tile_manager()->SetMoreTilesNeedToBeRasterizedForTesting();
            EXPECT_TRUE(host_impl_->tile_manager()->HasScheduledTileTasksForTesting());
            run_loop.Run();
        }
    }

    TEST_F(TileManagerTest, LowResHasNoImage)
    {
        gfx::Size size(10, 12);
        TileResolution resolutions[] = { HIGH_RESOLUTION, LOW_RESOLUTION };

        for (size_t i = 0; i < arraysize(resolutions); ++i) {
            SCOPED_TRACE(resolutions[i]);

            // Make a RasterSource that will draw a blue bitmap image.
            skia::RefPtr<SkSurface> surface = skia::AdoptRef(
                SkSurface::NewRasterN32Premul(size.width(), size.height()));
            ASSERT_NE(surface, nullptr);
            surface->getCanvas()->clear(SK_ColorBLUE);
            skia::RefPtr<SkImage> blue_image = skia::AdoptRef(surface->newImageSnapshot());

            scoped_ptr<FakeDisplayListRecordingSource> recording_source = FakeDisplayListRecordingSource::CreateFilledRecordingSource(size);
            recording_source->SetBackgroundColor(SK_ColorTRANSPARENT);
            recording_source->SetRequiresClear(true);
            recording_source->SetClearCanvasWithDebugColor(false);
            SkPaint paint;
            paint.setColor(SK_ColorGREEN);
            recording_source->add_draw_rect_with_paint(gfx::Rect(size), paint);
            recording_source->add_draw_image(blue_image.get(), gfx::Point());
            recording_source->Rerecord();
            scoped_refptr<DisplayListRasterSource> raster = DisplayListRasterSource::CreateFromDisplayListRecordingSource(
                recording_source.get(), false);

            FakePictureLayerTilingClient tiling_client;
            tiling_client.SetTileSize(size);

            scoped_ptr<PictureLayerImpl> layer = PictureLayerImpl::Create(host_impl_->active_tree(), 1, false, nullptr);
            PictureLayerTilingSet* tiling_set = layer->picture_layer_tiling_set();

            auto* tiling = tiling_set->AddTiling(1.0f, raster);
            tiling->set_resolution(resolutions[i]);
            tiling->CreateAllTilesForTesting();
            tiling->SetTilePriorityRectsForTesting(
                gfx::Rect(size), // Visible rect.
                gfx::Rect(size), // Skewport rect.
                gfx::Rect(size), // Soon rect.
                gfx::Rect(size)); // Eventually rect.

            // SMOOTHNESS_TAKES_PRIORITY ensures that we will actually raster
            // LOW_RESOLUTION tiles, otherwise they are skipped.
            host_impl_->SetTreePriority(SMOOTHNESS_TAKES_PRIORITY);

            // Call PrepareTiles and wait for it to complete.
            auto* tile_manager = host_impl_->tile_manager();
            base::RunLoop run_loop;
            EXPECT_CALL(*host_impl_, NotifyAllTileTasksCompleted())
                .WillOnce(testing::Invoke([&run_loop]() { run_loop.Quit(); }));
            tile_manager->PrepareTiles(host_impl_->global_tile_state());
            run_loop.Run();
            tile_manager->Flush();

            Tile* tile = tiling->TileAt(0, 0);
            // The tile in the tiling was rastered.
            EXPECT_EQ(TileDrawInfo::RESOURCE_MODE, tile->draw_info().mode());
            EXPECT_TRUE(tile->draw_info().IsReadyToDraw());

            ResourceProvider::ScopedReadLockSoftware lock(
                host_impl_->resource_provider(), tile->draw_info().resource_id());
            const SkBitmap* bitmap = lock.sk_bitmap();
            for (int x = 0; x < size.width(); ++x) {
                for (int y = 0; y < size.height(); ++y) {
                    SCOPED_TRACE(y);
                    SCOPED_TRACE(x);
                    if (resolutions[i] == LOW_RESOLUTION) {
                        // Since it's low res, the bitmap was not drawn, and the background
                        // (green) is visible instead.
                        ASSERT_EQ(SK_ColorGREEN, bitmap->getColor(x, y));
                    } else {
                        EXPECT_EQ(HIGH_RESOLUTION, resolutions[i]);
                        // Since it's high res, the bitmap (blue) was drawn, and the
                        // background is not visible.
                        ASSERT_EQ(SK_ColorBLUE, bitmap->getColor(x, y));
                    }
                }
            }
        }
    }

    // Fake TileTaskRunner that just cancels all scheduled tasks immediately.
    class CancellingTileTaskRunner : public TileTaskRunner, public TileTaskClient {
    public:
        CancellingTileTaskRunner() { }

        // TileTaskRunner methods.
        void SetClient(TileTaskRunnerClient* client) override { }
        void Shutdown() override { }
        void CheckForCompletedTasks() override { }
        ResourceFormat GetResourceFormat(bool must_support_alpha) const override
        {
            return ResourceFormat::RGBA_8888;
        }
        bool GetResourceRequiresSwizzle(bool must_support_alpha) const override
        {
            return false;
        }

        void ScheduleTasks(TileTaskQueue* queue) override
        {
            // Just call CompleteOnOriginThread on each item in the queue. As none of
            // these items have run yet, they will be treated as cancelled tasks.
            for (const auto& task : queue->items) {
                task.task->CompleteOnOriginThread(this);
            }
        }

        // TileTaskClient methods.
        scoped_ptr<RasterBuffer> AcquireBufferForRaster(
            const Resource* resource,
            uint64_t resource_content_id,
            uint64_t previous_content_id) override
        {
            NOTREACHED();
            return nullptr;
        }
        void ReleaseBufferForRaster(scoped_ptr<RasterBuffer> buffer) override { }

        ~CancellingTileTaskRunner() override { }
    };

    // Ensures that if a raster task is cancelled, it gets returned to the resource
    // pool with an invalid content ID, not with its invalidated content ID.
    TEST_F(TileManagerTest, CancelledTasksHaveNoContentId)
    {
        // Create a CancellingTaskRunner and set it on the tile manager so that all
        // scheduled work is immediately cancelled.
        CancellingTileTaskRunner cancelling_runner;
        host_impl_->tile_manager()->SetTileTaskRunnerForTesting(&cancelling_runner);

        // Pick arbitrary IDs - they don't really matter as long as they're constant.
        int layer_id = 7;
        int invalidated_id = 43;

        scoped_refptr<FakeDisplayListRasterSource> pending_raster_source = FakeDisplayListRasterSource::CreateFilled(gfx::Size(128, 128));
        host_impl_->CreatePendingTree();
        LayerTreeImpl* pending_tree = host_impl_->pending_tree();

        // Steal from the recycled tree.
        scoped_ptr<FakePictureLayerImpl> pending_layer = FakePictureLayerImpl::CreateWithRasterSource(pending_tree, layer_id,
            pending_raster_source);
        pending_layer->SetDrawsContent(true);
        pending_layer->SetHasRenderSurface(true);

        // The bounds() just mirror the raster source size.
        pending_layer->SetBounds(pending_layer->raster_source()->GetSize());
        pending_tree->SetRootLayer(pending_layer.Pass());

        // Add tilings/tiles for the layer.
        host_impl_->pending_tree()->UpdateDrawProperties(false /* update_lcd_text */);

        // Build the raster queue and invalidate the top tile.
        scoped_ptr<RasterTilePriorityQueue> queue(host_impl_->BuildRasterQueue(
            SAME_PRIORITY_FOR_BOTH_TREES, RasterTilePriorityQueue::Type::ALL));
        EXPECT_FALSE(queue->IsEmpty());
        queue->Top().tile()->SetInvalidated(gfx::Rect(), invalidated_id);

        // PrepareTiles to schedule tasks. Due to the CancellingTileTaskRunner, these
        // tasks will immediately be canceled.
        host_impl_->tile_manager()->PrepareTiles(host_impl_->global_tile_state());

        // Make sure that the tile we invalidated above was not returned to the pool
        // with its invalidated resource ID.
        host_impl_->resource_pool()->CheckBusyResources();
        EXPECT_FALSE(host_impl_->resource_pool()->TryAcquireResourceWithContentId(
            invalidated_id));

        // Free our host_impl_ before the cancelling_runner we passed it, as it will
        // use that class in clean up.
        host_impl_ = nullptr;
    }

} // namespace
} // namespace cc
