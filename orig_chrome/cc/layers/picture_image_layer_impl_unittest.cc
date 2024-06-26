// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/picture_image_layer_impl.h"

#include "base/thread_task_runner_handle.h"
#include "cc/layers/append_quads_data.h"
#include "cc/quads/draw_quad.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/tiles/tile_priority.h"
#include "cc/trees/layer_tree_impl.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class TestablePictureImageLayerImpl : public PictureImageLayerImpl {
    public:
        TestablePictureImageLayerImpl(LayerTreeImpl* tree_impl, int id)
            : PictureImageLayerImpl(tree_impl, id, false)
        {
        }
        using PictureLayerImpl::MaximumTilingContentsScale;
        using PictureLayerImpl::UpdateIdealScales;

        PictureLayerTilingSet* tilings() { return tilings_.get(); }

        friend class PictureImageLayerImplTest;
    };

    class PictureLayerImplImageTestSettings : public LayerTreeSettings {
    public:
        PictureLayerImplImageTestSettings()
        {
            layer_transforms_should_scale_layer_contents = true;
        }
    };

    class PictureImageLayerImplTest : public testing::Test {
    public:
        PictureImageLayerImplTest()
            : proxy_(base::ThreadTaskRunnerHandle::Get())
            , output_surface_(FakeOutputSurface::Create3d())
            , host_impl_(PictureLayerImplImageTestSettings(),
                  &proxy_,
                  &shared_bitmap_manager_,
                  &task_graph_runner_)
        {
            host_impl_.CreatePendingTree();
            host_impl_.InitializeRenderer(output_surface_.get());
        }

        scoped_ptr<TestablePictureImageLayerImpl> CreateLayer(int id,
            WhichTree which_tree)
        {
            LayerTreeImpl* tree = nullptr;
            switch (which_tree) {
            case ACTIVE_TREE:
                tree = host_impl_.active_tree();
                break;
            case PENDING_TREE:
                tree = host_impl_.pending_tree();
                break;
            }
            TestablePictureImageLayerImpl* layer = new TestablePictureImageLayerImpl(tree, id);
            layer->raster_source_ = FakeDisplayListRasterSource::CreateInfiniteFilled();
            layer->SetBounds(layer->raster_source_->GetSize());
            return make_scoped_ptr(layer);
        }

        void SetupDrawPropertiesAndUpdateTiles(TestablePictureImageLayerImpl* layer,
            float ideal_contents_scale,
            float device_scale_factor,
            float page_scale_factor,
            float maximum_animation_contents_scale,
            bool animating_transform_to_screen,
            gfx::Rect viewport_rect)
        {
            gfx::Transform scale_transform;
            scale_transform.Scale(ideal_contents_scale, ideal_contents_scale);
            layer->draw_properties().target_space_transform = scale_transform;
            DCHECK_EQ(layer->GetIdealContentsScale(), ideal_contents_scale);
            layer->draw_properties().maximum_animation_contents_scale = maximum_animation_contents_scale;
            layer->draw_properties().screen_space_transform_is_animating = animating_transform_to_screen;
            layer->draw_properties().visible_layer_rect = viewport_rect;
            bool resourceless_software_draw = false;
            layer->UpdateTiles(resourceless_software_draw);
        }

    protected:
        FakeImplProxy proxy_;
        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        scoped_ptr<OutputSurface> output_surface_;
        FakeLayerTreeHostImpl host_impl_;
    };

    TEST_F(PictureImageLayerImplTest, CalculateContentsScale)
    {
        scoped_ptr<TestablePictureImageLayerImpl> layer(CreateLayer(1, PENDING_TREE));
        layer->SetDrawsContent(true);

        gfx::Rect viewport(100, 200);
        SetupDrawPropertiesAndUpdateTiles(
            layer.get(), 2.f, 3.f, 4.f, 1.f, false, viewport);
        EXPECT_FLOAT_EQ(1.f, layer->MaximumTilingContentsScale());
    }

    TEST_F(PictureImageLayerImplTest, IgnoreIdealContentScale)
    {
        scoped_ptr<TestablePictureImageLayerImpl> pending_layer(
            CreateLayer(1, PENDING_TREE));
        pending_layer->SetDrawsContent(true);

        gfx::Rect viewport(100, 200);

        // Set PictureLayerImpl::ideal_contents_scale_ to 2.f which is not equal
        // to the content scale used by PictureImageLayerImpl.
        const float suggested_ideal_contents_scale = 2.f;
        const float device_scale_factor = 3.f;
        const float page_scale_factor = 4.f;
        const float maximum_animation_contents_scale = 1.f;
        const bool animating_transform_to_screen = false;
        SetupDrawPropertiesAndUpdateTiles(pending_layer.get(),
            suggested_ideal_contents_scale,
            device_scale_factor,
            page_scale_factor,
            maximum_animation_contents_scale,
            animating_transform_to_screen,
            viewport);
        EXPECT_EQ(1.f, pending_layer->tilings()->tiling_at(0)->contents_scale());

        // Push to active layer.
        host_impl_.pending_tree()->SetRootLayer(pending_layer.Pass());
        host_impl_.ActivateSyncTree();

        TestablePictureImageLayerImpl* active_layer = static_cast<TestablePictureImageLayerImpl*>(
            host_impl_.active_tree()->root_layer());
        SetupDrawPropertiesAndUpdateTiles(active_layer,
            suggested_ideal_contents_scale,
            device_scale_factor,
            page_scale_factor,
            maximum_animation_contents_scale,
            animating_transform_to_screen,
            viewport);
        EXPECT_EQ(1.f, active_layer->tilings()->tiling_at(0)->contents_scale());

        // Create resources for the tiles.
        host_impl_.tile_manager()->InitializeTilesWithResourcesForTesting(
            active_layer->tilings()->tiling_at(0)->AllTilesForTesting());

        // Draw.
        scoped_ptr<RenderPass> render_pass = RenderPass::Create();
        AppendQuadsData data;
        active_layer->WillDraw(DRAW_MODE_SOFTWARE, nullptr);
        active_layer->AppendQuads(render_pass.get(), &data);
        active_layer->DidDraw(nullptr);

        EXPECT_EQ(DrawQuad::TILED_CONTENT, render_pass->quad_list.front()->material);

        // Tiles are ready at correct scale, so should not set had_incomplete_tile.
        EXPECT_EQ(0, data.num_incomplete_tiles);
    }

} // namespace
} // namespace cc
