// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/surface_aggregator_test_helpers.h"

#include "base/format_macros.h"
#include "base/strings/stringprintf.h"
#include "cc/layers/append_quads_data.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/shared_quad_state.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/surface_draw_quad.h"
#include "cc/surfaces/surface.h"
#include "cc/test/render_pass_test_utils.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkXfermode.h"

namespace cc {
namespace test {

    void AddSurfaceQuad(RenderPass* pass,
        const gfx::Size& surface_size,
        float opacity,
        SurfaceId surface_id)
    {
        gfx::Transform layer_to_target_transform;
        gfx::Size layer_bounds = surface_size;
        gfx::Rect visible_layer_rect = gfx::Rect(surface_size);
        gfx::Rect clip_rect = gfx::Rect(surface_size);
        bool is_clipped = false;
        SkXfermode::Mode blend_mode = SkXfermode::kSrcOver_Mode;

        SharedQuadState* shared_quad_state = pass->CreateAndAppendSharedQuadState();
        shared_quad_state->SetAll(layer_to_target_transform, layer_bounds,
            visible_layer_rect, clip_rect, is_clipped, opacity,
            blend_mode, 0);

        SurfaceDrawQuad* surface_quad = pass->CreateAndAppendDrawQuad<SurfaceDrawQuad>();
        gfx::Rect quad_rect = gfx::Rect(surface_size);
        surface_quad->SetNew(pass->shared_quad_state_list.back(),
            gfx::Rect(surface_size), gfx::Rect(surface_size),
            surface_id);
    }
    void AddRenderPassQuad(RenderPass* pass, RenderPassId render_pass_id)
    {
        gfx::Rect output_rect = gfx::Rect(0, 0, 5, 5);
        SharedQuadState* shared_state = pass->CreateAndAppendSharedQuadState();
        shared_state->SetAll(gfx::Transform(), output_rect.size(), output_rect,
            output_rect, false, 1, SkXfermode::kSrcOver_Mode, 0);
        RenderPassDrawQuad* quad = pass->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        quad->SetNew(shared_state, output_rect, output_rect, render_pass_id, 0,
            gfx::Vector2dF(), gfx::Size(), FilterOperations(),
            gfx::Vector2dF(), FilterOperations());
    }

    void AddQuadInPass(RenderPass* pass, Quad desc)
    {
        switch (desc.material) {
        case DrawQuad::SOLID_COLOR:
            AddQuad(pass, gfx::Rect(0, 0, 5, 5), desc.color);
            break;
        case DrawQuad::SURFACE_CONTENT:
            AddSurfaceQuad(pass, gfx::Size(5, 5), desc.opacity, desc.surface_id);
            break;
        case DrawQuad::RENDER_PASS:
            AddRenderPassQuad(pass, desc.render_pass_id);
            break;
        default:
            NOTREACHED();
        }
    }

    void AddPasses(RenderPassList* pass_list,
        const gfx::Rect& output_rect,
        Pass* passes,
        size_t pass_count)
    {
        gfx::Transform root_transform;
        for (size_t i = 0; i < pass_count; ++i) {
            Pass pass = passes[i];
            RenderPass* test_pass = AddRenderPass(pass_list, pass.id, output_rect, root_transform);
            for (size_t j = 0; j < pass.quad_count; ++j) {
                AddQuadInPass(test_pass, pass.quads[j]);
            }
        }
    }

    void TestQuadMatchesExpectations(Quad expected_quad, const DrawQuad* quad)
    {
        switch (expected_quad.material) {
        case DrawQuad::SOLID_COLOR: {
            ASSERT_EQ(DrawQuad::SOLID_COLOR, quad->material);

            const SolidColorDrawQuad* solid_color_quad = SolidColorDrawQuad::MaterialCast(quad);

            EXPECT_EQ(expected_quad.color, solid_color_quad->color);
            break;
        }
        case DrawQuad::RENDER_PASS: {
            ASSERT_EQ(DrawQuad::RENDER_PASS, quad->material);

            const RenderPassDrawQuad* render_pass_quad = RenderPassDrawQuad::MaterialCast(quad);

            EXPECT_EQ(expected_quad.render_pass_id, render_pass_quad->render_pass_id);
            break;
        }
        default:
            NOTREACHED();
            break;
        }
    }

    void TestPassMatchesExpectations(Pass expected_pass, const RenderPass* pass)
    {
        ASSERT_EQ(expected_pass.quad_count, pass->quad_list.size());
        for (auto iter = pass->quad_list.cbegin(); iter != pass->quad_list.cend();
             ++iter) {
            SCOPED_TRACE(base::StringPrintf("Quad number %" PRIuS, iter.index()));
            TestQuadMatchesExpectations(expected_pass.quads[iter.index()], *iter);
        }
    }

    void TestPassesMatchExpectations(Pass* expected_passes,
        size_t expected_pass_count,
        const RenderPassList* passes)
    {
        ASSERT_EQ(expected_pass_count, passes->size());

        for (size_t i = 0; i < passes->size(); ++i) {
            SCOPED_TRACE(base::StringPrintf("Pass number %" PRIuS, i));
            RenderPass* pass = passes->at(i);
            TestPassMatchesExpectations(expected_passes[i], pass);
        }
    }

} // namespace test
} // namespace cc
