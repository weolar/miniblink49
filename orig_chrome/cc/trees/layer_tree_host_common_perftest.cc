// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/layer_tree_host_common.h"

#include <sstream>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/memory/scoped_ptr.h"
#include "base/path_service.h"
#include "base/strings/string_piece.h"
#include "base/threading/thread.h"
#include "base/time/time.h"
#include "cc/base/scoped_ptr_deque.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/debug/lap_timer.h"
#include "cc/layers/layer.h"
#include "cc/output/bsp_tree.h"
#include "cc/quads/draw_polygon.h"
#include "cc/quads/draw_quad.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/fake_layer_tree_host_client.h"
#include "cc/test/layer_tree_json_parser.h"
#include "cc/test/layer_tree_test.h"
#include "cc/test/paths.h"
#include "cc/trees/layer_tree_impl.h"
#include "testing/perf/perf_test.h"

namespace cc {
namespace {

    static const int kTimeLimitMillis = 2000;
    static const int kWarmupRuns = 5;
    static const int kTimeCheckInterval = 10;

    class LayerTreeHostCommonPerfTest : public LayerTreeTest {
    public:
        LayerTreeHostCommonPerfTest()
            : timer_(kWarmupRuns,
                base::TimeDelta::FromMilliseconds(kTimeLimitMillis),
                kTimeCheckInterval)
        {
        }

        void ReadTestFile(const std::string& name)
        {
            base::FilePath test_data_dir;
            ASSERT_TRUE(PathService::Get(CCPaths::DIR_TEST_DATA, &test_data_dir));
            base::FilePath json_file = test_data_dir.AppendASCII(name + ".json");
            ASSERT_TRUE(base::ReadFileToString(json_file, &json_));
        }

        void SetupTree() override
        {
            gfx::Size viewport = gfx::Size(720, 1038);
            layer_tree_host()->SetViewportSize(viewport);
            scoped_refptr<Layer> root = ParseTreeFromJson(json_, &content_layer_client_);
            ASSERT_TRUE(root.get());
            layer_tree_host()->SetRootLayer(root);
        }

        void SetTestName(const std::string& name) { test_name_ = name; }

        void AfterTest() override
        {
            CHECK(!test_name_.empty()) << "Must SetTestName() before TearDown().";
            perf_test::PrintResult("calc_draw_props_time",
                "",
                test_name_,
                1000 * timer_.MsPerLap(),
                "us",
                true);
        }

    protected:
        FakeContentLayerClient content_layer_client_;
        LapTimer timer_;
        std::string test_name_;
        std::string json_;
    };

    class CalcDrawPropsTest : public LayerTreeHostCommonPerfTest {
    public:
        void RunCalcDrawProps() { RunTest(false, false); }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DrawLayersOnThread(LayerTreeHostImpl* host_impl) override
        {
            timer_.Reset();
            LayerTreeImpl* active_tree = host_impl->active_tree();

            do {
                bool can_render_to_separate_surface = true;
                int max_texture_size = 8096;
                DoCalcDrawPropertiesImpl(can_render_to_separate_surface,
                    max_texture_size,
                    active_tree,
                    host_impl);

                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            EndTest();
        }

        void DoCalcDrawPropertiesImpl(bool can_render_to_separate_surface,
            int max_texture_size,
            LayerTreeImpl* active_tree,
            LayerTreeHostImpl* host_impl)
        {
            LayerImplList update_list;
            PropertyTrees property_trees;
            bool verify_property_trees = false;
            LayerTreeHostCommon::CalcDrawPropsImplInputs inputs(
                active_tree->root_layer(), active_tree->DrawViewportSize(),
                host_impl->DrawTransform(), active_tree->device_scale_factor(),
                active_tree->current_page_scale_factor(),
                active_tree->InnerViewportContainerLayer(),
                active_tree->InnerViewportScrollLayer(),
                active_tree->OuterViewportScrollLayer(),
                active_tree->elastic_overscroll()->Current(active_tree->IsActiveTree()),
                active_tree->OverscrollElasticityLayer(), max_texture_size,
                host_impl->settings().can_use_lcd_text,
                host_impl->settings().layers_always_allowed_lcd_text,
                can_render_to_separate_surface,
                host_impl->settings().layer_transforms_should_scale_layer_contents,
                verify_property_trees, &update_list, 0, &property_trees);
            LayerTreeHostCommon::CalculateDrawProperties(&inputs);
        }
    };

    class BspTreePerfTest : public CalcDrawPropsTest {
    public:
        BspTreePerfTest()
            : num_duplicates_(1)
        {
        }
        void RunSortLayers() { RunTest(false, false); }

        void SetNumberOfDuplicates(int num_duplicates)
        {
            num_duplicates_ = num_duplicates;
        }

        void BeginTest() override { PostSetNeedsCommitToMainThread(); }

        void DrawLayersOnThread(LayerTreeHostImpl* host_impl) override
        {
            LayerTreeImpl* active_tree = host_impl->active_tree();
            // First build the tree and then we'll start running tests on layersorter
            // itself
            bool can_render_to_separate_surface = true;
            int max_texture_size = 8096;
            DoCalcDrawPropertiesImpl(can_render_to_separate_surface,
                max_texture_size,
                active_tree,
                host_impl);

            LayerImplList base_list;
            BuildLayerImplList(active_tree->root_layer(), &base_list);

            int polygon_counter = 0;
            ScopedPtrVector<DrawPolygon> polygon_list;
            for (LayerImplList::iterator it = base_list.begin(); it != base_list.end();
                 ++it) {
                DrawPolygon* draw_polygon = new DrawPolygon(NULL, gfx::RectF(gfx::SizeF((*it)->bounds())),
                    (*it)->draw_transform(), polygon_counter++);
                polygon_list.push_back(scoped_ptr<DrawPolygon>(draw_polygon));
            }

            timer_.Reset();
            do {
                ScopedPtrDeque<DrawPolygon> test_list;
                for (int i = 0; i < num_duplicates_; i++) {
                    for (size_t i = 0; i < polygon_list.size(); i++) {
                        test_list.push_back(polygon_list[i]->CreateCopy());
                    }
                }
                BspTree bsp_tree(&test_list);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            EndTest();
        }

        void BuildLayerImplList(LayerImpl* layer, LayerImplList* list)
        {
            if (layer->Is3dSorted()) {
                list->push_back(layer);
            }

            for (size_t i = 0; i < layer->children().size(); i++) {
                BuildLayerImplList(layer->children()[i], list);
            }
        }

    private:
        LayerImplList base_list_;
        int num_duplicates_;
    };

    TEST_F(CalcDrawPropsTest, TenTen)
    {
        SetTestName("10_10");
        ReadTestFile("10_10_layer_tree");
        RunCalcDrawProps();
    }

    TEST_F(CalcDrawPropsTest, HeavyPage)
    {
        SetTestName("heavy_page");
        ReadTestFile("heavy_layer_tree");
        RunCalcDrawProps();
    }

    TEST_F(CalcDrawPropsTest, TouchRegionLight)
    {
        SetTestName("touch_region_light");
        ReadTestFile("touch_region_light");
        RunCalcDrawProps();
    }

    TEST_F(CalcDrawPropsTest, TouchRegionHeavy)
    {
        SetTestName("touch_region_heavy");
        ReadTestFile("touch_region_heavy");
        RunCalcDrawProps();
    }

    TEST_F(BspTreePerfTest, LayerSorterCubes)
    {
        SetTestName("layer_sort_cubes");
        ReadTestFile("layer_sort_cubes");
        RunSortLayers();
    }

    TEST_F(BspTreePerfTest, LayerSorterRubik)
    {
        SetTestName("layer_sort_rubik");
        ReadTestFile("layer_sort_rubik");
        // TODO(vollick): Remove verify_property_trees setting after
        // crbug.com/444219 is fixed.
        bool old_verify_property_trees = verify_property_trees();
        set_verify_property_trees(false);
        RunSortLayers();
        set_verify_property_trees(old_verify_property_trees);
    }

    TEST_F(BspTreePerfTest, BspTreeCubes)
    {
        SetTestName("bsp_tree_cubes");
        SetNumberOfDuplicates(1);
        ReadTestFile("layer_sort_cubes");
        RunSortLayers();
    }

    TEST_F(BspTreePerfTest, BspTreeRubik)
    {
        SetTestName("bsp_tree_rubik");
        SetNumberOfDuplicates(1);
        ReadTestFile("layer_sort_rubik");
        // TODO(vollick): Remove verify_property_trees setting after
        // crbug.com/444219 is fixed.
        bool old_verify_property_trees = verify_property_trees();
        set_verify_property_trees(false);
        RunSortLayers();
        set_verify_property_trees(old_verify_property_trees);
    }

    TEST_F(BspTreePerfTest, BspTreeCubes_2)
    {
        SetTestName("bsp_tree_cubes_2");
        SetNumberOfDuplicates(2);
        ReadTestFile("layer_sort_cubes");
        RunSortLayers();
    }

    TEST_F(BspTreePerfTest, BspTreeCubes_4)
    {
        SetTestName("bsp_tree_cubes_4");
        SetNumberOfDuplicates(4);
        ReadTestFile("layer_sort_cubes");
        RunSortLayers();
    }

} // namespace
} // namespace cc
