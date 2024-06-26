// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/containers/hash_tables.h"
#include "cc/layers/append_quads_data.h"
#include "cc/layers/nine_patch_layer_impl.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/resources/ui_resource_bitmap.h"
#include "cc/resources/ui_resource_client.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_ui_resource_layer_tree_host_impl.h"
#include "cc/test/geometry_test_utils.h"
#include "cc/test/layer_test_common.h"
#include "cc/trees/single_thread_proxy.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/safe_integer_conversions.h"
#include "ui/gfx/transform.h"

namespace cc {
namespace {

    gfx::Rect ToRoundedIntRect(const gfx::RectF& rect_f)
    {
        return gfx::Rect(gfx::ToRoundedInt(rect_f.x()),
            gfx::ToRoundedInt(rect_f.y()),
            gfx::ToRoundedInt(rect_f.width()),
            gfx::ToRoundedInt(rect_f.height()));
    }

    void NinePatchLayerLayoutTest(const gfx::Size& bitmap_size,
        const gfx::Rect& aperture_rect,
        const gfx::Size& layer_size,
        const gfx::Rect& border,
        bool fill_center,
        size_t expected_quad_size)
    {
        scoped_ptr<RenderPass> render_pass = RenderPass::Create();
        gfx::Rect visible_layer_rect(layer_size);
        gfx::Rect expected_remaining(border.x(),
            border.y(),
            layer_size.width() - border.width(),
            layer_size.height() - border.height());

        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        scoped_ptr<OutputSurface> output_surface = FakeOutputSurface::Create3d();
        FakeUIResourceLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);
        host_impl.InitializeRenderer(output_surface.get());

        scoped_ptr<NinePatchLayerImpl> layer = NinePatchLayerImpl::Create(host_impl.active_tree(), 1);
        layer->draw_properties().visible_layer_rect = visible_layer_rect;
        layer->SetBounds(layer_size);
        layer->SetHasRenderSurface(true);
        layer->draw_properties().render_target = layer.get();

        UIResourceId uid = 1;
        bool is_opaque = false;
        UIResourceBitmap bitmap(bitmap_size, is_opaque);

        host_impl.CreateUIResource(uid, bitmap);
        layer->SetUIResourceId(uid);
        layer->SetImageBounds(bitmap_size);
        layer->SetLayout(aperture_rect, border, fill_center);
        AppendQuadsData data;
        layer->AppendQuads(render_pass.get(), &data);

        // Verify quad rects
        const QuadList& quads = render_pass->quad_list;
        EXPECT_EQ(expected_quad_size, quads.size());

        Region remaining(visible_layer_rect);
        for (auto iter = quads.cbegin(); iter != quads.cend(); ++iter) {
            gfx::Rect quad_rect = iter->rect;

            EXPECT_TRUE(visible_layer_rect.Contains(quad_rect)) << iter.index();
            EXPECT_TRUE(remaining.Contains(quad_rect)) << iter.index();
            remaining.Subtract(Region(quad_rect));
        }

        // Check if the left-over quad is the same size as the mapped aperture quad in
        // layer space.
        if (!fill_center) {
            EXPECT_EQ(expected_remaining, remaining.bounds());
        } else {
            EXPECT_TRUE(remaining.bounds().IsEmpty());
        }

        // Verify UV rects
        gfx::Rect bitmap_rect(bitmap_size);
        Region tex_remaining(bitmap_rect);
        for (const auto& quad : quads) {
            const TextureDrawQuad* tex_quad = TextureDrawQuad::MaterialCast(quad);
            gfx::RectF tex_rect = gfx::BoundingRect(tex_quad->uv_top_left, tex_quad->uv_bottom_right);
            tex_rect.Scale(bitmap_size.width(), bitmap_size.height());
            tex_remaining.Subtract(Region(ToRoundedIntRect(tex_rect)));
        }

        if (!fill_center) {
            EXPECT_EQ(aperture_rect, tex_remaining.bounds());
            Region aperture_region(aperture_rect);
            EXPECT_EQ(aperture_region, tex_remaining);
        } else {
            EXPECT_TRUE(remaining.bounds().IsEmpty());
        }
    }

    TEST(NinePatchLayerImplTest, VerifyDrawQuads)
    {
        // Input is a 100x100 bitmap with a 40x50 aperture at x=20, y=30.
        // The bounds of the layer are set to 400x400.
        gfx::Size bitmap_size(100, 100);
        gfx::Size layer_size(400, 500);
        gfx::Rect aperture_rect(20, 30, 40, 50);
        gfx::Rect border(40, 40, 80, 80);
        bool fill_center = false;
        size_t expected_quad_size = 8;
        NinePatchLayerLayoutTest(bitmap_size,
            aperture_rect,
            layer_size,
            border,
            fill_center,
            expected_quad_size);

        // The bounds of the layer are set to less than the bitmap size.
        bitmap_size = gfx::Size(100, 100);
        layer_size = gfx::Size(40, 50);
        aperture_rect = gfx::Rect(20, 30, 40, 50);
        border = gfx::Rect(10, 10, 25, 15);
        fill_center = true;
        expected_quad_size = 9;
        NinePatchLayerLayoutTest(bitmap_size,
            aperture_rect,
            layer_size,
            border,
            fill_center,
            expected_quad_size);

        // Layer and image sizes are equal.
        bitmap_size = gfx::Size(100, 100);
        layer_size = gfx::Size(100, 100);
        aperture_rect = gfx::Rect(20, 30, 40, 50);
        border = gfx::Rect(20, 30, 40, 50);
        fill_center = true;
        expected_quad_size = 9;
        NinePatchLayerLayoutTest(bitmap_size,
            aperture_rect,
            layer_size,
            border,
            fill_center,
            expected_quad_size);
    }

    TEST(NinePatchLayerImplTest, VerifyDrawQuadsWithEmptyPatches)
    {
        // The top component of the 9-patch is empty, so there should be no quads for
        // the top three components.
        gfx::Size bitmap_size(100, 100);
        gfx::Size layer_size(100, 100);
        gfx::Rect aperture_rect(10, 0, 80, 90);
        gfx::Rect border(10, 0, 20, 10);
        bool fill_center = false;
        size_t expected_quad_size = 5;
        NinePatchLayerLayoutTest(bitmap_size,
            aperture_rect,
            layer_size,
            border,
            fill_center,
            expected_quad_size);

        // The top and left components of the 9-patch are empty, so there should be no
        // quads for the left and top components.
        bitmap_size = gfx::Size(100, 100);
        layer_size = gfx::Size(100, 100);
        aperture_rect = gfx::Rect(0, 0, 90, 90);
        border = gfx::Rect(0, 0, 10, 10);
        fill_center = false;
        expected_quad_size = 3;
        NinePatchLayerLayoutTest(bitmap_size,
            aperture_rect,
            layer_size,
            border,
            fill_center,
            expected_quad_size);

        // The aperture is the size of the bitmap and the center doesn't draw.
        bitmap_size = gfx::Size(100, 100);
        layer_size = gfx::Size(100, 100);
        aperture_rect = gfx::Rect(0, 0, 100, 100);
        border = gfx::Rect(0, 0, 0, 0);
        fill_center = false;
        expected_quad_size = 0;
        NinePatchLayerLayoutTest(bitmap_size,
            aperture_rect,
            layer_size,
            border,
            fill_center,
            expected_quad_size);

        // The aperture is the size of the bitmap and the center does draw.
        bitmap_size = gfx::Size(100, 100);
        layer_size = gfx::Size(100, 100);
        aperture_rect = gfx::Rect(0, 0, 100, 100);
        border = gfx::Rect(0, 0, 0, 0);
        fill_center = true;
        expected_quad_size = 1;
        NinePatchLayerLayoutTest(bitmap_size,
            aperture_rect,
            layer_size,
            border,
            fill_center,
            expected_quad_size);
    }

    TEST(NinePatchLayerImplTest, Occlusion)
    {
        gfx::Size layer_size(1000, 1000);
        gfx::Size viewport_size(1000, 1000);

        LayerTestCommon::LayerImplTest impl;

        SkBitmap sk_bitmap;
        sk_bitmap.allocN32Pixels(10, 10);
        sk_bitmap.setImmutable();
        UIResourceId uid = 5;
        UIResourceBitmap bitmap(sk_bitmap);
        impl.host_impl()->CreateUIResource(uid, bitmap);

        NinePatchLayerImpl* nine_patch_layer_impl = impl.AddChildToRoot<NinePatchLayerImpl>();
        nine_patch_layer_impl->SetBounds(layer_size);
        nine_patch_layer_impl->SetDrawsContent(true);
        nine_patch_layer_impl->SetUIResourceId(uid);
        nine_patch_layer_impl->SetImageBounds(gfx::Size(10, 10));

        gfx::Rect aperture = gfx::Rect(3, 3, 4, 4);
        gfx::Rect border = gfx::Rect(300, 300, 400, 400);
        nine_patch_layer_impl->SetLayout(aperture, border, true);

        impl.CalcDrawProps(viewport_size);

        {
            SCOPED_TRACE("No occlusion");
            gfx::Rect occluded;
            impl.AppendQuadsWithOcclusion(nine_patch_layer_impl, occluded);

            LayerTestCommon::VerifyQuadsExactlyCoverRect(impl.quad_list(),
                gfx::Rect(layer_size));
            EXPECT_EQ(9u, impl.quad_list().size());
        }

        {
            SCOPED_TRACE("Full occlusion");
            gfx::Rect occluded(nine_patch_layer_impl->visible_layer_rect());
            impl.AppendQuadsWithOcclusion(nine_patch_layer_impl, occluded);

            LayerTestCommon::VerifyQuadsExactlyCoverRect(impl.quad_list(), gfx::Rect());
            EXPECT_EQ(impl.quad_list().size(), 0u);
        }

        {
            SCOPED_TRACE("Partial occlusion");
            gfx::Rect occluded(0, 0, 500, 1000);
            impl.AppendQuadsWithOcclusion(nine_patch_layer_impl, occluded);

            size_t partially_occluded_count = 0;
            LayerTestCommon::VerifyQuadsAreOccluded(
                impl.quad_list(), occluded, &partially_occluded_count);
            // The layer outputs nine quads, three of which are partially occluded, and
            // three fully occluded.
            EXPECT_EQ(6u, impl.quad_list().size());
            EXPECT_EQ(3u, partially_occluded_count);
        }
    }

    TEST(NinePatchLayerImplTest, OpaqueRect)
    {
        gfx::Size layer_size(1000, 1000);
        gfx::Size viewport_size(1000, 1000);

        LayerTestCommon::LayerImplTest impl;

        SkBitmap sk_bitmap_opaque;
        sk_bitmap_opaque.allocN32Pixels(10, 10);
        sk_bitmap_opaque.setImmutable();
        sk_bitmap_opaque.setAlphaType(kOpaque_SkAlphaType);

        UIResourceId uid_opaque = 6;
        UIResourceBitmap bitmap_opaque(sk_bitmap_opaque);
        impl.host_impl()->CreateUIResource(uid_opaque, bitmap_opaque);

        SkBitmap sk_bitmap_alpha;
        sk_bitmap_alpha.allocN32Pixels(10, 10);
        sk_bitmap_alpha.setImmutable();
        sk_bitmap_alpha.setAlphaType(kUnpremul_SkAlphaType);

        UIResourceId uid_alpha = 7;
        UIResourceBitmap bitmap_alpha(sk_bitmap_alpha);

        impl.host_impl()->CreateUIResource(uid_alpha, bitmap_alpha);

        NinePatchLayerImpl* nine_patch_layer_impl = impl.AddChildToRoot<NinePatchLayerImpl>();
        nine_patch_layer_impl->SetBounds(layer_size);
        nine_patch_layer_impl->SetDrawsContent(true);

        impl.CalcDrawProps(viewport_size);

        {
            SCOPED_TRACE("Use opaque image");

            nine_patch_layer_impl->SetUIResourceId(uid_opaque);
            nine_patch_layer_impl->SetImageBounds(gfx::Size(10, 10));

            gfx::Rect aperture = gfx::Rect(3, 3, 4, 4);
            gfx::Rect border = gfx::Rect(300, 300, 400, 400);
            nine_patch_layer_impl->SetLayout(aperture, border, true);

            impl.AppendQuadsWithOcclusion(nine_patch_layer_impl, gfx::Rect());

            const QuadList& quad_list = impl.quad_list();
            for (QuadList::ConstBackToFrontIterator it = quad_list.BackToFrontBegin();
                 it != quad_list.BackToFrontEnd(); ++it)
                EXPECT_FALSE(it->ShouldDrawWithBlending());
        }

        {
            SCOPED_TRACE("Use tranparent image");

            nine_patch_layer_impl->SetUIResourceId(uid_alpha);

            impl.AppendQuadsWithOcclusion(nine_patch_layer_impl, gfx::Rect());

            const QuadList& quad_list = impl.quad_list();
            for (QuadList::ConstBackToFrontIterator it = quad_list.BackToFrontBegin();
                 it != quad_list.BackToFrontEnd(); ++it)
                EXPECT_TRUE(it->ShouldDrawWithBlending());
        }
    }

} // namespace
} // namespace cc
