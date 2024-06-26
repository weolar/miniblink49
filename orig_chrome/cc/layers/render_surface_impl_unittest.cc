// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/render_surface_impl.h"

#include "cc/test/layer_test_common.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    TEST(RenderSurfaceLayerImplTest, Occlusion)
    {
        gfx::Size layer_size(1000, 1000);
        gfx::Size viewport_size(1000, 1000);

        LayerTestCommon::LayerImplTest impl;

        LayerImpl* owning_layer_impl = impl.AddChildToRoot<LayerImpl>();
        owning_layer_impl->SetBounds(layer_size);
        owning_layer_impl->SetDrawsContent(true);
        owning_layer_impl->SetHasRenderSurface(true);

        impl.CalcDrawProps(viewport_size);

        RenderSurfaceImpl* render_surface_impl = owning_layer_impl->render_surface();
        ASSERT_TRUE(render_surface_impl);

        {
            SCOPED_TRACE("No occlusion");
            gfx::Rect occluded;
            impl.AppendSurfaceQuadsWithOcclusion(render_surface_impl, occluded);

            LayerTestCommon::VerifyQuadsExactlyCoverRect(impl.quad_list(),
                gfx::Rect(layer_size));
            EXPECT_EQ(1u, impl.quad_list().size());
        }

        {
            SCOPED_TRACE("Full occlusion");
            gfx::Rect occluded(owning_layer_impl->visible_layer_rect());
            impl.AppendSurfaceQuadsWithOcclusion(render_surface_impl, occluded);

            LayerTestCommon::VerifyQuadsExactlyCoverRect(impl.quad_list(), gfx::Rect());
            EXPECT_EQ(impl.quad_list().size(), 0u);
        }

        {
            SCOPED_TRACE("Partial occlusion");
            gfx::Rect occluded(200, 0, 800, 1000);
            impl.AppendSurfaceQuadsWithOcclusion(render_surface_impl, occluded);

            size_t partially_occluded_count = 0;
            LayerTestCommon::VerifyQuadsAreOccluded(
                impl.quad_list(), occluded, &partially_occluded_count);
            // The layer outputs one quad, which is partially occluded.
            EXPECT_EQ(1u, impl.quad_list().size());
            EXPECT_EQ(1u, partially_occluded_count);
        }
    }

} // namespace
} // namespace cc
