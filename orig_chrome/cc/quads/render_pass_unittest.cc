// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/render_pass.h"

#include "cc/base/math_util.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/output/copy_output_request.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/test/geometry_test_utils.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/effects/SkBlurImageFilter.h"
#include "ui/gfx/transform.h"

namespace cc {
namespace {

    struct RenderPassSize {
        // If you add a new field to this class, make sure to add it to the
        // Copy() tests.
        RenderPassId id;
        QuadList quad_list;
        SharedQuadStateList shared_quad_state_list;
        gfx::Transform transform_to_root_target;
        gfx::Rect output_rect;
        gfx::Rect damage_rect;
        bool has_transparent_background;
        std::vector<SurfaceId> referenced_surfaces;
        ScopedPtrVector<CopyOutputRequest> copy_callbacks;
    };

    static void CompareRenderPassLists(const RenderPassList& expected_list,
        const RenderPassList& actual_list)
    {
        EXPECT_EQ(expected_list.size(), actual_list.size());
        for (size_t i = 0; i < actual_list.size(); ++i) {
            RenderPass* expected = expected_list[i];
            RenderPass* actual = actual_list[i];

            EXPECT_EQ(expected->id, actual->id);
            EXPECT_EQ(expected->output_rect, actual->output_rect);
            EXPECT_EQ(expected->transform_to_root_target,
                actual->transform_to_root_target);
            EXPECT_EQ(expected->damage_rect, actual->damage_rect);
            EXPECT_EQ(expected->has_transparent_background,
                actual->has_transparent_background);

            EXPECT_EQ(expected->shared_quad_state_list.size(),
                actual->shared_quad_state_list.size());
            EXPECT_EQ(expected->quad_list.size(), actual->quad_list.size());
            EXPECT_EQ(expected->referenced_surfaces, actual->referenced_surfaces);

            for (auto exp_iter = expected->quad_list.cbegin(),
                      act_iter = actual->quad_list.cbegin();
                 exp_iter != expected->quad_list.cend();
                 ++exp_iter, ++act_iter) {
                EXPECT_EQ(exp_iter->rect.ToString(), act_iter->rect.ToString());
                EXPECT_EQ(exp_iter->shared_quad_state->quad_layer_bounds.ToString(),
                    act_iter->shared_quad_state->quad_layer_bounds.ToString());
            }
        }
    }

    TEST(RenderPassTest, CopyShouldBeIdenticalExceptIdAndQuads)
    {
        RenderPassId id(3, 2);
        gfx::Rect output_rect(45, 22, 120, 13);
        gfx::Transform transform_to_root = gfx::Transform(1.0, 0.5, 0.5, -0.5, -1.0, 0.0);
        gfx::Rect damage_rect(56, 123, 19, 43);
        bool has_transparent_background = true;

        scoped_ptr<RenderPass> pass = RenderPass::Create();
        pass->SetAll(id,
            output_rect,
            damage_rect,
            transform_to_root,
            has_transparent_background);
        pass->copy_requests.push_back(CopyOutputRequest::CreateEmptyRequest());

        // Stick a quad in the pass, this should not get copied.
        SharedQuadState* shared_state = pass->CreateAndAppendSharedQuadState();
        shared_state->SetAll(gfx::Transform(),
            gfx::Size(),
            gfx::Rect(),
            gfx::Rect(),
            false,
            1,
            SkXfermode::kSrcOver_Mode,
            0);

        SolidColorDrawQuad* color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(pass->shared_quad_state_list.back(), gfx::Rect(),
            gfx::Rect(), SkColor(), false);

        RenderPassId new_id(63, 4);

        scoped_ptr<RenderPass> copy = pass->Copy(new_id);
        EXPECT_EQ(new_id, copy->id);
        EXPECT_EQ(pass->output_rect, copy->output_rect);
        EXPECT_EQ(pass->transform_to_root_target, copy->transform_to_root_target);
        EXPECT_EQ(pass->damage_rect, copy->damage_rect);
        EXPECT_EQ(pass->has_transparent_background, copy->has_transparent_background);
        EXPECT_EQ(0u, copy->quad_list.size());
        EXPECT_EQ(0u, copy->referenced_surfaces.size());

        // The copy request should not be copied/duplicated.
        EXPECT_EQ(1u, pass->copy_requests.size());
        EXPECT_EQ(0u, copy->copy_requests.size());

        EXPECT_EQ(sizeof(RenderPassSize), sizeof(RenderPass));
    }

    TEST(RenderPassTest, CopyAllShouldBeIdentical)
    {
        RenderPassList pass_list;

        RenderPassId id(3, 2);
        gfx::Rect output_rect(45, 22, 120, 13);
        gfx::Transform transform_to_root = gfx::Transform(1.0, 0.5, 0.5, -0.5, -1.0, 0.0);
        gfx::Rect damage_rect(56, 123, 19, 43);
        bool has_transparent_background = true;

        scoped_ptr<RenderPass> pass = RenderPass::Create();
        pass->SetAll(id,
            output_rect,
            damage_rect,
            transform_to_root,
            has_transparent_background);

        // Two quads using one shared state.
        SharedQuadState* shared_state1 = pass->CreateAndAppendSharedQuadState();
        shared_state1->SetAll(gfx::Transform(),
            gfx::Size(1, 1),
            gfx::Rect(),
            gfx::Rect(),
            false,
            1,
            SkXfermode::kSrcOver_Mode,
            0);

        SolidColorDrawQuad* color_quad1 = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad1->SetNew(pass->shared_quad_state_list.back(),
            gfx::Rect(1, 1, 1, 1), gfx::Rect(1, 1, 1, 1), SkColor(),
            false);

        SolidColorDrawQuad* color_quad2 = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad2->SetNew(pass->shared_quad_state_list.back(),
            gfx::Rect(2, 2, 2, 2), gfx::Rect(2, 2, 2, 2), SkColor(),
            false);

        // And two quads using another shared state.
        SharedQuadState* shared_state2 = pass->CreateAndAppendSharedQuadState();
        shared_state2->SetAll(gfx::Transform(),
            gfx::Size(2, 2),
            gfx::Rect(),
            gfx::Rect(),
            false,
            1,
            SkXfermode::kSrcOver_Mode,
            0);

        SolidColorDrawQuad* color_quad3 = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad3->SetNew(pass->shared_quad_state_list.back(),
            gfx::Rect(3, 3, 3, 3), gfx::Rect(3, 3, 3, 3), SkColor(),
            false);

        SolidColorDrawQuad* color_quad4 = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad4->SetNew(pass->shared_quad_state_list.back(),
            gfx::Rect(4, 4, 4, 4), gfx::Rect(4, 4, 4, 4), SkColor(),
            false);

        // A second render pass with a quad.
        RenderPassId contrib_id(4, 1);
        gfx::Rect contrib_output_rect(10, 15, 12, 17);
        gfx::Transform contrib_transform_to_root = gfx::Transform(1.0, 0.5, 0.5, -0.5, -1.0, 0.0);
        gfx::Rect contrib_damage_rect(11, 16, 10, 15);
        bool contrib_has_transparent_background = true;

        scoped_ptr<RenderPass> contrib = RenderPass::Create();
        contrib->SetAll(contrib_id,
            contrib_output_rect,
            contrib_damage_rect,
            contrib_transform_to_root,
            contrib_has_transparent_background);

        SharedQuadState* contrib_shared_state = contrib->CreateAndAppendSharedQuadState();
        contrib_shared_state->SetAll(gfx::Transform(),
            gfx::Size(2, 2),
            gfx::Rect(),
            gfx::Rect(),
            false,
            1,
            SkXfermode::kSrcOver_Mode,
            0);

        SolidColorDrawQuad* contrib_quad = contrib->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        contrib_quad->SetNew(contrib->shared_quad_state_list.back(),
            gfx::Rect(3, 3, 3, 3), gfx::Rect(3, 3, 3, 3), SkColor(),
            false);

        // And a RenderPassDrawQuad for the contributing pass.
        scoped_ptr<RenderPassDrawQuad> pass_quad = make_scoped_ptr(new RenderPassDrawQuad);
        pass_quad->SetNew(pass->shared_quad_state_list.back(),
            contrib_output_rect,
            contrib_output_rect,
            contrib_id,
            0,
            gfx::Vector2dF(),
            gfx::Size(),
            FilterOperations(),
            gfx::Vector2dF(),
            FilterOperations());

        pass_list.push_back(pass.Pass());
        pass_list.push_back(contrib.Pass());

        // Make a copy with CopyAll().
        RenderPassList copy_list;
        RenderPass::CopyAll(pass_list, &copy_list);

        CompareRenderPassLists(pass_list, copy_list);
    }

    TEST(RenderPassTest, CopyAllWithCulledQuads)
    {
        RenderPassList pass_list;

        RenderPassId id(3, 2);
        gfx::Rect output_rect(45, 22, 120, 13);
        gfx::Transform transform_to_root = gfx::Transform(1.0, 0.5, 0.5, -0.5, -1.0, 0.0);
        gfx::Rect damage_rect(56, 123, 19, 43);
        bool has_transparent_background = true;

        scoped_ptr<RenderPass> pass = RenderPass::Create();
        pass->SetAll(id,
            output_rect,
            damage_rect,
            transform_to_root,
            has_transparent_background);

        // A shared state with a quad.
        SharedQuadState* shared_state1 = pass->CreateAndAppendSharedQuadState();
        shared_state1->SetAll(gfx::Transform(),
            gfx::Size(1, 1),
            gfx::Rect(),
            gfx::Rect(),
            false,
            1,
            SkXfermode::kSrcOver_Mode,
            0);

        SolidColorDrawQuad* color_quad1 = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad1->SetNew(pass->shared_quad_state_list.back(),
            gfx::Rect(1, 1, 1, 1), gfx::Rect(1, 1, 1, 1), SkColor(),
            false);

        // A shared state with no quads, they were culled.
        SharedQuadState* shared_state2 = pass->CreateAndAppendSharedQuadState();
        shared_state2->SetAll(gfx::Transform(),
            gfx::Size(2, 2),
            gfx::Rect(),
            gfx::Rect(),
            false,
            1,
            SkXfermode::kSrcOver_Mode,
            0);

        // A second shared state with no quads.
        SharedQuadState* shared_state3 = pass->CreateAndAppendSharedQuadState();
        shared_state3->SetAll(gfx::Transform(),
            gfx::Size(2, 2),
            gfx::Rect(),
            gfx::Rect(),
            false,
            1,
            SkXfermode::kSrcOver_Mode,
            0);

        // A last shared state with a quad again.
        SharedQuadState* shared_state4 = pass->CreateAndAppendSharedQuadState();
        shared_state4->SetAll(gfx::Transform(),
            gfx::Size(2, 2),
            gfx::Rect(),
            gfx::Rect(),
            false,
            1,
            SkXfermode::kSrcOver_Mode,
            0);

        SolidColorDrawQuad* color_quad2 = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad2->SetNew(pass->shared_quad_state_list.back(),
            gfx::Rect(3, 3, 3, 3), gfx::Rect(3, 3, 3, 3), SkColor(),
            false);

        pass_list.push_back(pass.Pass());

        // Make a copy with CopyAll().
        RenderPassList copy_list;
        RenderPass::CopyAll(pass_list, &copy_list);

        CompareRenderPassLists(pass_list, copy_list);
    }

} // namespace
} // namespace cc
