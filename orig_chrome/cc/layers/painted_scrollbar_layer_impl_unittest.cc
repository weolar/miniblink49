// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/painted_scrollbar_layer_impl.h"

#include "cc/quads/draw_quad.h"
#include "cc/test/layer_test_common.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    TEST(PaintedScrollbarLayerImplTest, Occlusion)
    {
        gfx::Size layer_size(10, 1000);
        float scale = 2.f;
        gfx::Size scaled_layer_size(20, 2000);
        gfx::Size viewport_size(1000, 1000);

        LayerTestCommon::LayerImplTest impl;

        SkBitmap thumb_sk_bitmap;
        thumb_sk_bitmap.allocN32Pixels(10, 10);
        thumb_sk_bitmap.setImmutable();
        UIResourceId thumb_uid = 5;
        UIResourceBitmap thumb_bitmap(thumb_sk_bitmap);
        impl.host_impl()->CreateUIResource(thumb_uid, thumb_bitmap);

        SkBitmap track_sk_bitmap;
        track_sk_bitmap.allocN32Pixels(10, 10);
        track_sk_bitmap.setImmutable();
        UIResourceId track_uid = 6;
        UIResourceBitmap track_bitmap(track_sk_bitmap);
        impl.host_impl()->CreateUIResource(track_uid, track_bitmap);

        ScrollbarOrientation orientation = VERTICAL;

        PaintedScrollbarLayerImpl* scrollbar_layer_impl = impl.AddChildToRoot<PaintedScrollbarLayerImpl>(orientation);
        scrollbar_layer_impl->SetBounds(layer_size);
        scrollbar_layer_impl->SetContentsOpaque(true);
        scrollbar_layer_impl->set_internal_contents_scale_and_bounds(
            scale, scaled_layer_size);
        scrollbar_layer_impl->SetDrawsContent(true);
        scrollbar_layer_impl->SetThumbThickness(layer_size.width());
        scrollbar_layer_impl->SetThumbLength(500);
        scrollbar_layer_impl->SetTrackLength(layer_size.height());
        scrollbar_layer_impl->SetCurrentPos(100.f / 4);
        scrollbar_layer_impl->SetMaximum(100);
        scrollbar_layer_impl->SetVisibleToTotalLengthRatio(1.f / 2);
        scrollbar_layer_impl->set_track_ui_resource_id(track_uid);
        scrollbar_layer_impl->set_thumb_ui_resource_id(thumb_uid);

        impl.CalcDrawProps(viewport_size);

        gfx::Rect thumb_rect = scrollbar_layer_impl->ComputeThumbQuadRect();
        EXPECT_EQ(gfx::Rect(0, 500 / 4, 10, layer_size.height() / 2).ToString(),
            thumb_rect.ToString());

        {
            SCOPED_TRACE("No occlusion");
            gfx::Rect occluded;
            impl.AppendQuadsWithOcclusion(scrollbar_layer_impl, occluded);

            size_t partially_occluded_count = 0;
            LayerTestCommon::VerifyQuadsAreOccluded(
                impl.quad_list(), occluded, &partially_occluded_count);
            EXPECT_EQ(2u, impl.quad_list().size());
            EXPECT_EQ(0u, partially_occluded_count);

            // Note: this is also testing that the thumb and track are both
            // scaled by the internal contents scale.  It's not occlusion-related
            // but is easy to verify here.
            const DrawQuad* thumb_quad = impl.quad_list().ElementAt(0);
            const DrawQuad* track_quad = impl.quad_list().ElementAt(1);

            gfx::Rect scaled_thumb_rect = gfx::ScaleToEnclosingRect(thumb_rect, scale);
            EXPECT_EQ(track_quad->rect.ToString(),
                gfx::Rect(scaled_layer_size).ToString());
            EXPECT_EQ(track_quad->opaque_rect.ToString(),
                gfx::Rect(scaled_layer_size).ToString());
            EXPECT_EQ(track_quad->visible_rect.ToString(),
                gfx::Rect(scaled_layer_size).ToString());
            EXPECT_EQ(thumb_quad->rect.ToString(), scaled_thumb_rect.ToString());
            EXPECT_EQ(thumb_quad->visible_rect.ToString(),
                scaled_thumb_rect.ToString());
        }

        {
            SCOPED_TRACE("Full occlusion");
            gfx::Rect occluded(scrollbar_layer_impl->visible_layer_rect());
            impl.AppendQuadsWithOcclusion(scrollbar_layer_impl, occluded);

            LayerTestCommon::VerifyQuadsExactlyCoverRect(impl.quad_list(), gfx::Rect());
            EXPECT_EQ(impl.quad_list().size(), 0u);
        }

        {
            SCOPED_TRACE("Partial occlusion");
            gfx::Rect occluded(0, 0, 5, 1000);
            impl.AppendQuadsWithOcclusion(scrollbar_layer_impl, occluded);

            size_t partially_occluded_count = 0;
            LayerTestCommon::VerifyQuadsAreOccluded(
                impl.quad_list(), occluded, &partially_occluded_count);
            // The layer outputs two quads, which is partially occluded.
            EXPECT_EQ(2u, impl.quad_list().size());
            EXPECT_EQ(2u, partially_occluded_count);
        }
    }

} // namespace
} // namespace cc
