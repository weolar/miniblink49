// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/layer_test_common.h"

#include "cc/base/math_util.h"
#include "cc/base/region.h"
#include "cc/layers/append_quads_data.h"
#include "cc/quads/draw_quad.h"
#include "cc/quads/render_pass.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/mock_occlusion_tracker.h"
#include "cc/trees/layer_tree_host_common.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace cc {

// Align with expected and actual output.
const char* LayerTestCommon::quad_string = "    Quad: ";

static bool CanRectFBeSafelyRoundedToRect(const gfx::RectF& r)
{
    // Ensure that range of float values is not beyond integer range.
    if (!r.IsExpressibleAsRect())
        return false;

    // Ensure that the values are actually integers.
    gfx::RectF floored_rect(std::floor(r.x()), std::floor(r.y()),
        std::floor(r.width()), std::floor(r.height()));
    return floored_rect == r;
}

void LayerTestCommon::VerifyQuadsExactlyCoverRect(const QuadList& quads,
    const gfx::Rect& rect)
{
    Region remaining = rect;

    for (auto iter = quads.cbegin(); iter != quads.cend(); ++iter) {
        gfx::RectF quad_rectf = MathUtil::MapClippedRect(
            iter->shared_quad_state->quad_to_target_transform,
            gfx::RectF(iter->rect));

        // Before testing for exact coverage in the integer world, assert that
        // rounding will not round the rect incorrectly.
        ASSERT_TRUE(CanRectFBeSafelyRoundedToRect(quad_rectf));

        gfx::Rect quad_rect = gfx::ToEnclosingRect(quad_rectf);

        EXPECT_TRUE(rect.Contains(quad_rect)) << quad_string << iter.index()
                                              << " rect: " << rect.ToString()
                                              << " quad: " << quad_rect.ToString();
        EXPECT_TRUE(remaining.Contains(quad_rect))
            << quad_string << iter.index() << " remaining: " << remaining.ToString()
            << " quad: " << quad_rect.ToString();
        remaining.Subtract(quad_rect);
    }

    EXPECT_TRUE(remaining.IsEmpty());
}

// static
void LayerTestCommon::VerifyQuadsAreOccluded(const QuadList& quads,
    const gfx::Rect& occluded,
    size_t* partially_occluded_count)
{
    // No quad should exist if it's fully occluded.
    for (const auto& quad : quads) {
        gfx::Rect target_visible_rect = MathUtil::MapEnclosingClippedRect(
            quad->shared_quad_state->quad_to_target_transform, quad->visible_rect);
        EXPECT_FALSE(occluded.Contains(target_visible_rect));
    }

    // Quads that are fully occluded on one axis only should be shrunken.
    for (const auto& quad : quads) {
        gfx::Rect target_rect = MathUtil::MapEnclosingClippedRect(
            quad->shared_quad_state->quad_to_target_transform, quad->rect);
        if (!quad->shared_quad_state->quad_to_target_transform
                 .IsIdentityOrIntegerTranslation()) {
            DCHECK(quad->shared_quad_state->quad_to_target_transform
                       .IsPositiveScaleOrTranslation())
                << quad->shared_quad_state->quad_to_target_transform.ToString();
            gfx::RectF target_rectf = MathUtil::MapClippedRect(
                quad->shared_quad_state->quad_to_target_transform,
                gfx::RectF(quad->rect));
            // Scale transforms allowed, as long as the final transformed rect
            // ends up on integer boundaries for ease of testing.
            ASSERT_EQ(target_rectf, gfx::RectF(target_rect));
        }
        gfx::Rect target_visible_rect = MathUtil::MapEnclosingClippedRect(
            quad->shared_quad_state->quad_to_target_transform, quad->visible_rect);

        bool fully_occluded_horizontal = target_rect.x() >= occluded.x() && target_rect.right() <= occluded.right();
        bool fully_occluded_vertical = target_rect.y() >= occluded.y() && target_rect.bottom() <= occluded.bottom();
        bool should_be_occluded = target_rect.Intersects(occluded) && (fully_occluded_vertical || fully_occluded_horizontal);
        if (!should_be_occluded) {
            EXPECT_EQ(quad->rect.ToString(), quad->visible_rect.ToString());
        } else {
            EXPECT_NE(quad->rect.ToString(), quad->visible_rect.ToString());
            EXPECT_TRUE(quad->rect.Contains(quad->visible_rect));
            ++(*partially_occluded_count);
        }
    }
}

LayerTestCommon::LayerImplTest::LayerImplTest()
    : LayerImplTest(LayerTreeSettings())
{
}

LayerTestCommon::LayerImplTest::LayerImplTest(const LayerTreeSettings& settings)
    : client_(FakeLayerTreeHostClient::DIRECT_3D)
    , output_surface_(FakeOutputSurface::Create3d())
    , host_(FakeLayerTreeHost::Create(&client_, &task_graph_runner_, settings))
    , root_layer_impl_(LayerImpl::Create(host_->host_impl()->active_tree(), 1))
    , render_pass_(RenderPass::Create())
    , layer_impl_id_(2)
{
    root_layer_impl_->SetHasRenderSurface(true);
    host_->host_impl()->InitializeRenderer(output_surface_.get());
}

LayerTestCommon::LayerImplTest::~LayerImplTest() { }

void LayerTestCommon::LayerImplTest::CalcDrawProps(
    const gfx::Size& viewport_size)
{
    LayerImplList layer_list;
    LayerTreeHostCommon::CalcDrawPropsImplInputsForTesting inputs(
        root_layer_impl_.get(), viewport_size, &layer_list);
    LayerTreeHostCommon::CalculateDrawProperties(&inputs);
}

void LayerTestCommon::LayerImplTest::AppendQuadsWithOcclusion(
    LayerImpl* layer_impl,
    const gfx::Rect& occluded)
{
    AppendQuadsData data;

    render_pass_->quad_list.clear();
    render_pass_->shared_quad_state_list.clear();

    Occlusion occlusion(layer_impl->draw_transform(),
        SimpleEnclosedRegion(occluded),
        SimpleEnclosedRegion());
    layer_impl->draw_properties().occlusion_in_content_space = occlusion;

    layer_impl->WillDraw(DRAW_MODE_HARDWARE, resource_provider());
    layer_impl->AppendQuads(render_pass_.get(), &data);
    layer_impl->DidDraw(resource_provider());
}

void LayerTestCommon::LayerImplTest::AppendQuadsForPassWithOcclusion(
    LayerImpl* layer_impl,
    RenderPass* given_render_pass,
    const gfx::Rect& occluded)
{
    AppendQuadsData data;

    given_render_pass->quad_list.clear();
    given_render_pass->shared_quad_state_list.clear();

    Occlusion occlusion(layer_impl->draw_transform(),
        SimpleEnclosedRegion(occluded),
        SimpleEnclosedRegion());
    layer_impl->draw_properties().occlusion_in_content_space = occlusion;

    layer_impl->WillDraw(DRAW_MODE_HARDWARE, resource_provider());
    layer_impl->AppendQuads(given_render_pass, &data);
    layer_impl->DidDraw(resource_provider());
}

void LayerTestCommon::LayerImplTest::AppendSurfaceQuadsWithOcclusion(
    RenderSurfaceImpl* surface_impl,
    const gfx::Rect& occluded)
{
    AppendQuadsData data;

    render_pass_->quad_list.clear();
    render_pass_->shared_quad_state_list.clear();

    surface_impl->AppendQuads(
        render_pass_.get(), gfx::Transform(),
        Occlusion(gfx::Transform(), SimpleEnclosedRegion(occluded),
            SimpleEnclosedRegion()),
        SK_ColorBLACK, 1.f, nullptr, &data, RenderPassId(1, 1));
}

} // namespace cc
