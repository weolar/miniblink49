// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/draw_quad.h"

#include <algorithm>

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "cc/base/math_util.h"
#include "cc/output/filter_operations.h"
#include "cc/quads/debug_border_draw_quad.h"
#include "cc/quads/io_surface_draw_quad.h"
#include "cc/quads/largest_draw_quad.h"
#include "cc/quads/picture_draw_quad.h"
#include "cc/quads/render_pass.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/stream_video_draw_quad.h"
#include "cc/quads/surface_draw_quad.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/quads/tile_draw_quad.h"
#include "cc/quads/yuv_video_draw_quad.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/geometry_test_utils.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/effects/SkBlurImageFilter.h"
#include "ui/gfx/transform.h"

namespace cc {
namespace {

    TEST(DrawQuadTest, CopySharedQuadState)
    {
        gfx::Transform quad_transform = gfx::Transform(1.0, 0.0, 0.5, 1.0, 0.5, 0.0);
        gfx::Size layer_bounds(26, 28);
        gfx::Rect visible_layer_rect(10, 12, 14, 16);
        gfx::Rect clip_rect(19, 21, 23, 25);
        bool is_clipped = true;
        float opacity = 0.25f;
        SkXfermode::Mode blend_mode = SkXfermode::kMultiply_Mode;
        int sorting_context_id = 65536;

        scoped_ptr<SharedQuadState> state(new SharedQuadState);
        state->SetAll(quad_transform, layer_bounds, visible_layer_rect, clip_rect,
            is_clipped, opacity, blend_mode, sorting_context_id);

        scoped_ptr<SharedQuadState> copy(new SharedQuadState);
        copy->CopyFrom(state.get());
        EXPECT_EQ(quad_transform, copy->quad_to_target_transform);
        EXPECT_EQ(visible_layer_rect, copy->visible_quad_layer_rect);
        EXPECT_EQ(opacity, copy->opacity);
        EXPECT_EQ(clip_rect, copy->clip_rect);
        EXPECT_EQ(is_clipped, copy->is_clipped);
        EXPECT_EQ(blend_mode, copy->blend_mode);
    }

    SharedQuadState* CreateSharedQuadState(RenderPass* render_pass)
    {
        gfx::Transform quad_transform = gfx::Transform(1.0, 0.0, 0.5, 1.0, 0.5, 0.0);
        gfx::Size layer_bounds(26, 28);
        gfx::Rect visible_layer_rect(10, 12, 14, 16);
        gfx::Rect clip_rect(19, 21, 23, 25);
        bool is_clipped = false;
        float opacity = 1.f;
        int sorting_context_id = 65536;
        SkXfermode::Mode blend_mode = SkXfermode::kSrcOver_Mode;

        SharedQuadState* state = render_pass->CreateAndAppendSharedQuadState();
        state->SetAll(quad_transform, layer_bounds, visible_layer_rect, clip_rect,
            is_clipped, opacity, blend_mode, sorting_context_id);
        return state;
    }

    void CompareDrawQuad(DrawQuad* quad,
        DrawQuad* copy,
        SharedQuadState* copy_shared_state)
    {
        EXPECT_EQ(quad->material, copy->material);
        EXPECT_EQ(quad->rect, copy->rect);
        EXPECT_EQ(quad->visible_rect, copy->visible_rect);
        EXPECT_EQ(quad->opaque_rect, copy->opaque_rect);
        EXPECT_EQ(quad->needs_blending, copy->needs_blending);
        EXPECT_EQ(copy_shared_state, copy->shared_quad_state);
    }

#define CREATE_SHARED_STATE()                                                           \
    scoped_ptr<RenderPass> render_pass = RenderPass::Create();                          \
    SharedQuadState* shared_state(CreateSharedQuadState(render_pass.get()));            \
    SharedQuadState* copy_shared_state = render_pass->CreateAndAppendSharedQuadState(); \
    copy_shared_state->CopyFrom(shared_state);

#define QUAD_DATA                                \
    gfx::Rect quad_rect(30, 40, 50, 60);         \
    gfx::Rect quad_visible_rect(40, 50, 30, 20); \
    gfx::Rect quad_opaque_rect(60, 55, 10, 10);  \
    ALLOW_UNUSED_LOCAL(quad_opaque_rect);        \
    bool needs_blending = true;                  \
    ALLOW_UNUSED_LOCAL(needs_blending);

#define SETUP_AND_COPY_QUAD_NEW(Type, quad)                                                   \
    DrawQuad* copy_new = render_pass->CopyFromAndAppendDrawQuad(quad_new, copy_shared_state); \
    CompareDrawQuad(quad_new, copy_new, copy_shared_state);                                   \
    const Type* copy_quad = Type::MaterialCast(copy_new);                                     \
    ALLOW_UNUSED_LOCAL(copy_quad);

#define SETUP_AND_COPY_QUAD_ALL(Type, quad)                                                   \
    DrawQuad* copy_all = render_pass->CopyFromAndAppendDrawQuad(quad_all, copy_shared_state); \
    CompareDrawQuad(quad_all, copy_all, copy_shared_state);                                   \
    copy_quad = Type::MaterialCast(copy_all);

#define SETUP_AND_COPY_QUAD_NEW_RP(Type, quad, a)                          \
    DrawQuad* copy_new = render_pass->CopyFromAndAppendRenderPassDrawQuad( \
        quad_new, copy_shared_state, a);                                   \
    CompareDrawQuad(quad_new, copy_new, copy_shared_state);                \
    const Type* copy_quad = Type::MaterialCast(copy_new);                  \
    ALLOW_UNUSED_LOCAL(copy_quad);

#define SETUP_AND_COPY_QUAD_ALL_RP(Type, quad, a)                          \
    DrawQuad* copy_all = render_pass->CopyFromAndAppendRenderPassDrawQuad( \
        quad_all, copy_shared_state, a);                                   \
    CompareDrawQuad(quad_all, copy_all, copy_shared_state);                \
    copy_quad = Type::MaterialCast(copy_all);

#define CREATE_QUAD_1_NEW(Type, a)                                 \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_new->SetNew(shared_state, quad_rect, a);    \
    }                                                              \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_1_ALL(Type, a)                                 \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_all->SetAll(shared_state,                   \
            quad_rect,                                             \
            quad_opaque_rect,                                      \
            quad_visible_rect,                                     \
            needs_blending,                                        \
            a);                                                    \
    }                                                              \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_2_NEW(Type, a, b)                              \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_new->SetNew(shared_state, quad_rect, a, b); \
    }                                                              \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_2_ALL(Type, a, b)                              \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_all->SetAll(shared_state,                   \
            quad_rect,                                             \
            quad_opaque_rect,                                      \
            quad_visible_rect,                                     \
            needs_blending,                                        \
            a,                                                     \
            b);                                                    \
    }                                                              \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_3_NEW(Type, a, b, c)                              \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>();    \
    {                                                                 \
        QUAD_DATA quad_new->SetNew(shared_state, quad_rect, a, b, c); \
    }                                                                 \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_3_ALL(Type, a, b, c)                           \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_all->SetAll(shared_state,                   \
            quad_rect,                                             \
            quad_opaque_rect,                                      \
            quad_visible_rect,                                     \
            needs_blending,                                        \
            a,                                                     \
            b,                                                     \
            c);                                                    \
    }                                                              \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_4_NEW(Type, a, b, c, d)                              \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>();       \
    {                                                                    \
        QUAD_DATA quad_new->SetNew(shared_state, quad_rect, a, b, c, d); \
    }                                                                    \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_4_ALL(Type, a, b, c, d)                        \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_all->SetAll(shared_state,                   \
            quad_rect,                                             \
            quad_opaque_rect,                                      \
            quad_visible_rect,                                     \
            needs_blending,                                        \
            a,                                                     \
            b,                                                     \
            c,                                                     \
            d);                                                    \
    }                                                              \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_5_NEW(Type, a, b, c, d, e)                              \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>();          \
    {                                                                       \
        QUAD_DATA quad_new->SetNew(shared_state, quad_rect, a, b, c, d, e); \
    }                                                                       \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_5_ALL(Type, a, b, c, d, e)                     \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_all->SetAll(shared_state,                   \
            quad_rect,                                             \
            quad_opaque_rect,                                      \
            quad_visible_rect,                                     \
            needs_blending,                                        \
            a,                                                     \
            b,                                                     \
            c,                                                     \
            d,                                                     \
            e);                                                    \
    }                                                              \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_6_NEW(Type, a, b, c, d, e, f)                              \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>();             \
    {                                                                          \
        QUAD_DATA quad_new->SetNew(shared_state, quad_rect, a, b, c, d, e, f); \
    }                                                                          \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_6_ALL(Type, a, b, c, d, e, f)                  \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_all->SetAll(shared_state,                   \
            quad_rect,                                             \
            quad_opaque_rect,                                      \
            quad_visible_rect,                                     \
            needs_blending,                                        \
            a,                                                     \
            b,                                                     \
            c,                                                     \
            d,                                                     \
            e,                                                     \
            f);                                                    \
    }                                                              \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_7_NEW(Type, a, b, c, d, e, f, g)                              \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>();                \
    {                                                                             \
        QUAD_DATA quad_new->SetNew(shared_state, quad_rect, a, b, c, d, e, f, g); \
    }                                                                             \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_7_ALL(Type, a, b, c, d, e, f, g)               \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_all->SetAll(shared_state,                   \
            quad_rect,                                             \
            quad_opaque_rect,                                      \
            quad_visible_rect,                                     \
            needs_blending,                                        \
            a,                                                     \
            b,                                                     \
            c,                                                     \
            d,                                                     \
            e,                                                     \
            f,                                                     \
            g);                                                    \
    }                                                              \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_8_NEW(Type, a, b, c, d, e, f, g, h)            \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_new->SetNew(                                \
            shared_state, quad_rect, a, b, c, d, e, f, g, h);      \
    }                                                              \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_8_ALL(Type, a, b, c, d, e, f, g, h)            \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_all->SetAll(shared_state,                   \
            quad_rect,                                             \
            quad_opaque_rect,                                      \
            quad_visible_rect,                                     \
            needs_blending,                                        \
            a,                                                     \
            b,                                                     \
            c,                                                     \
            d,                                                     \
            e,                                                     \
            f,                                                     \
            g,                                                     \
            h);                                                    \
    }                                                              \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_9_NEW(Type, a, b, c, d, e, f, g, h, i)         \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_new->SetNew(                                \
            shared_state, quad_rect, a, b, c, d, e, f, g, h, i);   \
    }                                                              \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_9_ALL(Type, a, b, c, d, e, f, g, h, i)                    \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>();            \
    {                                                                         \
        QUAD_DATA quad_all->SetAll(shared_state, quad_rect, quad_opaque_rect, \
            quad_visible_rect, needs_blending, a, b, c, d,                    \
            e, f, g, h, i);                                                   \
    }                                                                         \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_10_NEW(Type, a, b, c, d, e, f, g, h, i, j)      \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>();  \
    {                                                               \
        QUAD_DATA quad_new->SetNew(                                 \
            shared_state, quad_rect, a, b, c, d, e, f, g, h, i, j); \
    }                                                               \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_10_ALL(Type, a, b, c, d, e, f, g, h, i, j)                \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>();            \
    {                                                                         \
        QUAD_DATA quad_all->SetAll(shared_state, quad_rect, quad_opaque_rect, \
            quad_visible_rect, needs_blending, a, b, c, d,                    \
            e, f, g, h, i, j);                                                \
    }                                                                         \
    SETUP_AND_COPY_QUAD_ALL(Type, quad_all);

#define CREATE_QUAD_11_NEW(Type, a, b, c, d, e, f, g, h, i, j, k)                \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>();               \
    {                                                                            \
        QUAD_DATA quad_new->SetNew(shared_state, quad_rect, a, b, c, d, e, f, g, \
            h, i, j, k);                                                         \
    }                                                                            \
    SETUP_AND_COPY_QUAD_NEW(Type, quad_new);

#define CREATE_QUAD_ALL_RP(Type, a, b, c, d, e, f, g, copy_a)      \
    Type* quad_all = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_all->SetAll(shared_state,                   \
            quad_rect,                                             \
            quad_opaque_rect,                                      \
            quad_visible_rect,                                     \
            needs_blending,                                        \
            a,                                                     \
            b,                                                     \
            c,                                                     \
            d,                                                     \
            e,                                                     \
            f,                                                     \
            g);                                                    \
    }                                                              \
    SETUP_AND_COPY_QUAD_ALL_RP(Type, quad_all, copy_a);

#define CREATE_QUAD_NEW_RP(Type, a, b, c, d, e, f, g, h, copy_a)   \
    Type* quad_new = render_pass->CreateAndAppendDrawQuad<Type>(); \
    {                                                              \
        QUAD_DATA quad_new->SetNew(                                \
            shared_state, quad_rect, a, b, c, d, e, f, g, h);      \
    }                                                              \
    SETUP_AND_COPY_QUAD_NEW_RP(Type, quad_new, copy_a);

    TEST(DrawQuadTest, CopyDebugBorderDrawQuad)
    {
        gfx::Rect visible_rect(40, 50, 30, 20);
        SkColor color = 0xfabb0011;
        int width = 99;
        CREATE_SHARED_STATE();

        CREATE_QUAD_3_NEW(DebugBorderDrawQuad, visible_rect, color, width);
        EXPECT_EQ(DrawQuad::DEBUG_BORDER, copy_quad->material);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(color, copy_quad->color);
        EXPECT_EQ(width, copy_quad->width);

        CREATE_QUAD_2_ALL(DebugBorderDrawQuad, color, width);
        EXPECT_EQ(DrawQuad::DEBUG_BORDER, copy_quad->material);
        EXPECT_EQ(color, copy_quad->color);
        EXPECT_EQ(width, copy_quad->width);
    }

    TEST(DrawQuadTest, CopyIOSurfaceDrawQuad)
    {
        gfx::Rect opaque_rect(33, 47, 10, 12);
        gfx::Rect visible_rect(40, 50, 30, 20);
        gfx::Size size(58, 95);
        ResourceId resource_id = 72;
        IOSurfaceDrawQuad::Orientation orientation = IOSurfaceDrawQuad::UNFLIPPED;
        bool allow_overlay = true;
        CREATE_SHARED_STATE();

        CREATE_QUAD_6_NEW(IOSurfaceDrawQuad,
            opaque_rect,
            visible_rect,
            size,
            resource_id,
            orientation,
            allow_overlay);
        EXPECT_EQ(DrawQuad::IO_SURFACE_CONTENT, copy_quad->material);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(opaque_rect, copy_quad->opaque_rect);
        EXPECT_EQ(size, copy_quad->io_surface_size);
        EXPECT_EQ(resource_id, copy_quad->io_surface_resource_id());
        EXPECT_EQ(orientation, copy_quad->orientation);
        EXPECT_EQ(allow_overlay, copy_quad->allow_overlay);

        CREATE_QUAD_4_ALL(IOSurfaceDrawQuad,
            size,
            resource_id,
            orientation,
            allow_overlay);
        EXPECT_EQ(DrawQuad::IO_SURFACE_CONTENT, copy_quad->material);
        EXPECT_EQ(size, copy_quad->io_surface_size);
        EXPECT_EQ(resource_id, copy_quad->io_surface_resource_id());
        EXPECT_EQ(orientation, copy_quad->orientation);
        EXPECT_EQ(allow_overlay, copy_quad->allow_overlay);
    }

    TEST(DrawQuadTest, CopyRenderPassDrawQuad)
    {
        gfx::Rect visible_rect(40, 50, 30, 20);
        RenderPassId render_pass_id(22, 64);
        ResourceId mask_resource_id = 78;
        gfx::Vector2dF mask_uv_scale(33.f, 19.f);
        gfx::Size mask_texture_size(128, 134);
        FilterOperations filters;
        filters.Append(FilterOperation::CreateBlurFilter(1.f));
        gfx::Vector2dF filters_scale;
        FilterOperations background_filters;
        background_filters.Append(
            FilterOperation::CreateGrayscaleFilter(1.f));

        RenderPassId copied_render_pass_id(235, 11);
        CREATE_SHARED_STATE();

        CREATE_QUAD_NEW_RP(RenderPassDrawQuad,
            visible_rect,
            render_pass_id,
            mask_resource_id,
            mask_uv_scale,
            mask_texture_size,
            filters,
            filters_scale,
            background_filters,
            copied_render_pass_id);
        EXPECT_EQ(DrawQuad::RENDER_PASS, copy_quad->material);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(copied_render_pass_id, copy_quad->render_pass_id);
        EXPECT_EQ(mask_resource_id, copy_quad->mask_resource_id());
        EXPECT_EQ(mask_uv_scale.ToString(), copy_quad->mask_uv_scale.ToString());
        EXPECT_EQ(mask_texture_size.ToString(),
            copy_quad->mask_texture_size.ToString());
        EXPECT_EQ(filters, copy_quad->filters);
        EXPECT_EQ(filters_scale, copy_quad->filters_scale);
        EXPECT_EQ(background_filters, copy_quad->background_filters);

        CREATE_QUAD_ALL_RP(RenderPassDrawQuad,
            render_pass_id,
            mask_resource_id,
            mask_uv_scale,
            mask_texture_size,
            filters,
            filters_scale,
            background_filters,
            copied_render_pass_id);
        EXPECT_EQ(DrawQuad::RENDER_PASS, copy_quad->material);
        EXPECT_EQ(copied_render_pass_id, copy_quad->render_pass_id);
        EXPECT_EQ(mask_resource_id, copy_quad->mask_resource_id());
        EXPECT_EQ(mask_uv_scale.ToString(), copy_quad->mask_uv_scale.ToString());
        EXPECT_EQ(mask_texture_size.ToString(),
            copy_quad->mask_texture_size.ToString());
        EXPECT_EQ(filters, copy_quad->filters);
        EXPECT_EQ(filters_scale, copy_quad->filters_scale);
        EXPECT_EQ(background_filters, copy_quad->background_filters);
    }

    TEST(DrawQuadTest, CopySolidColorDrawQuad)
    {
        gfx::Rect visible_rect(40, 50, 30, 20);
        SkColor color = 0x49494949;
        bool force_anti_aliasing_off = false;
        CREATE_SHARED_STATE();

        CREATE_QUAD_3_NEW(
            SolidColorDrawQuad, visible_rect, color, force_anti_aliasing_off);
        EXPECT_EQ(DrawQuad::SOLID_COLOR, copy_quad->material);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(color, copy_quad->color);
        EXPECT_EQ(force_anti_aliasing_off, copy_quad->force_anti_aliasing_off);

        CREATE_QUAD_2_ALL(SolidColorDrawQuad, color, force_anti_aliasing_off);
        EXPECT_EQ(DrawQuad::SOLID_COLOR, copy_quad->material);
        EXPECT_EQ(color, copy_quad->color);
        EXPECT_EQ(force_anti_aliasing_off, copy_quad->force_anti_aliasing_off);
    }

    TEST(DrawQuadTest, CopyStreamVideoDrawQuad)
    {
        gfx::Rect opaque_rect(33, 47, 10, 12);
        gfx::Rect visible_rect(40, 50, 30, 20);
        ResourceId resource_id = 64;
        gfx::Size resource_size_in_pixels = gfx::Size(40, 41);
        bool allow_overlay = true;
        gfx::Transform matrix = gfx::Transform(0.5, 0.25, 1, 0.75, 0, 1);
        CREATE_SHARED_STATE();

        CREATE_QUAD_6_NEW(StreamVideoDrawQuad, opaque_rect, visible_rect, resource_id,
            resource_size_in_pixels, allow_overlay, matrix);
        EXPECT_EQ(DrawQuad::STREAM_VIDEO_CONTENT, copy_quad->material);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(opaque_rect, copy_quad->opaque_rect);
        EXPECT_EQ(resource_id, copy_quad->resource_id());
        EXPECT_EQ(allow_overlay, copy_quad->allow_overlay());
        EXPECT_EQ(resource_size_in_pixels, copy_quad->resource_size_in_pixels());
        EXPECT_EQ(matrix, copy_quad->matrix);

        CREATE_QUAD_4_ALL(StreamVideoDrawQuad, resource_id, resource_size_in_pixels,
            allow_overlay, matrix);
        EXPECT_EQ(DrawQuad::STREAM_VIDEO_CONTENT, copy_quad->material);
        EXPECT_EQ(resource_id, copy_quad->resource_id());
        EXPECT_EQ(allow_overlay, copy_quad->allow_overlay());
        EXPECT_EQ(resource_size_in_pixels, copy_quad->resource_size_in_pixels());
        EXPECT_EQ(matrix, copy_quad->matrix);
    }

    TEST(DrawQuadTest, CopySurfaceDrawQuad)
    {
        gfx::Rect visible_rect(40, 50, 30, 20);
        SurfaceId surface_id(1234);
        CREATE_SHARED_STATE();

        CREATE_QUAD_2_NEW(SurfaceDrawQuad, visible_rect, surface_id);
        EXPECT_EQ(DrawQuad::SURFACE_CONTENT, copy_quad->material);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(surface_id, copy_quad->surface_id);

        CREATE_QUAD_1_ALL(SurfaceDrawQuad, surface_id);
        EXPECT_EQ(DrawQuad::SURFACE_CONTENT, copy_quad->material);
        EXPECT_EQ(surface_id, copy_quad->surface_id);
    }

    TEST(DrawQuadTest, CopyTextureDrawQuad)
    {
        gfx::Rect opaque_rect(33, 47, 10, 12);
        gfx::Rect visible_rect(40, 50, 30, 20);
        unsigned resource_id = 82;
        gfx::Size resource_size_in_pixels = gfx::Size(40, 41);
        bool allow_overlay = true;
        bool premultiplied_alpha = true;
        gfx::PointF uv_top_left(0.5f, 224.f);
        gfx::PointF uv_bottom_right(51.5f, 260.f);
        const float vertex_opacity[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        bool y_flipped = true;
        bool nearest_neighbor = true;
        CREATE_SHARED_STATE();

        CREATE_QUAD_10_NEW(TextureDrawQuad,
            opaque_rect,
            visible_rect,
            resource_id,
            premultiplied_alpha,
            uv_top_left,
            uv_bottom_right,
            SK_ColorTRANSPARENT,
            vertex_opacity,
            y_flipped,
            nearest_neighbor);
        EXPECT_EQ(DrawQuad::TEXTURE_CONTENT, copy_quad->material);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(opaque_rect, copy_quad->opaque_rect);
        EXPECT_EQ(resource_id, copy_quad->resource_id());
        EXPECT_EQ(premultiplied_alpha, copy_quad->premultiplied_alpha);
        EXPECT_EQ(uv_top_left, copy_quad->uv_top_left);
        EXPECT_EQ(uv_bottom_right, copy_quad->uv_bottom_right);
        EXPECT_FLOAT_ARRAY_EQ(vertex_opacity, copy_quad->vertex_opacity, 4);
        EXPECT_EQ(y_flipped, copy_quad->y_flipped);
        EXPECT_EQ(nearest_neighbor, copy_quad->nearest_neighbor);

        CREATE_QUAD_10_ALL(TextureDrawQuad, resource_id, resource_size_in_pixels,
            allow_overlay, premultiplied_alpha, uv_top_left,
            uv_bottom_right, SK_ColorTRANSPARENT, vertex_opacity,
            y_flipped, nearest_neighbor);
        EXPECT_EQ(DrawQuad::TEXTURE_CONTENT, copy_quad->material);
        EXPECT_EQ(resource_id, copy_quad->resource_id());
        EXPECT_EQ(allow_overlay, copy_quad->allow_overlay());
        EXPECT_EQ(resource_size_in_pixels, copy_quad->resource_size_in_pixels());
        EXPECT_EQ(premultiplied_alpha, copy_quad->premultiplied_alpha);
        EXPECT_EQ(uv_top_left, copy_quad->uv_top_left);
        EXPECT_EQ(uv_bottom_right, copy_quad->uv_bottom_right);
        EXPECT_FLOAT_ARRAY_EQ(vertex_opacity, copy_quad->vertex_opacity, 4);
        EXPECT_EQ(y_flipped, copy_quad->y_flipped);
        EXPECT_EQ(nearest_neighbor, copy_quad->nearest_neighbor);
    }

    TEST(DrawQuadTest, CopyTileDrawQuad)
    {
        gfx::Rect opaque_rect(33, 44, 22, 33);
        gfx::Rect visible_rect(40, 50, 30, 20);
        unsigned resource_id = 104;
        gfx::RectF tex_coord_rect(31.f, 12.f, 54.f, 20.f);
        gfx::Size texture_size(85, 32);
        bool swizzle_contents = true;
        bool nearest_neighbor = true;
        CREATE_SHARED_STATE();

        CREATE_QUAD_7_NEW(TileDrawQuad,
            opaque_rect,
            visible_rect,
            resource_id,
            tex_coord_rect,
            texture_size,
            swizzle_contents,
            nearest_neighbor);
        EXPECT_EQ(DrawQuad::TILED_CONTENT, copy_quad->material);
        EXPECT_EQ(opaque_rect, copy_quad->opaque_rect);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(resource_id, copy_quad->resource_id());
        EXPECT_EQ(tex_coord_rect, copy_quad->tex_coord_rect);
        EXPECT_EQ(texture_size, copy_quad->texture_size);
        EXPECT_EQ(swizzle_contents, copy_quad->swizzle_contents);
        EXPECT_EQ(nearest_neighbor, copy_quad->nearest_neighbor);

        CREATE_QUAD_5_ALL(TileDrawQuad,
            resource_id,
            tex_coord_rect,
            texture_size,
            swizzle_contents,
            nearest_neighbor);
        EXPECT_EQ(DrawQuad::TILED_CONTENT, copy_quad->material);
        EXPECT_EQ(resource_id, copy_quad->resource_id());
        EXPECT_EQ(tex_coord_rect, copy_quad->tex_coord_rect);
        EXPECT_EQ(texture_size, copy_quad->texture_size);
        EXPECT_EQ(swizzle_contents, copy_quad->swizzle_contents);
        EXPECT_EQ(nearest_neighbor, copy_quad->nearest_neighbor);
    }

    TEST(DrawQuadTest, CopyYUVVideoDrawQuad)
    {
        gfx::Rect opaque_rect(33, 47, 10, 12);
        gfx::Rect visible_rect(40, 50, 30, 20);
        gfx::RectF ya_tex_coord_rect(40, 50, 30, 20);
        gfx::RectF uv_tex_coord_rect(20, 25, 15, 10);
        gfx::Size ya_tex_size(32, 68);
        gfx::Size uv_tex_size(41, 51);
        ResourceId y_plane_resource_id = 45;
        ResourceId u_plane_resource_id = 532;
        ResourceId v_plane_resource_id = 4;
        ResourceId a_plane_resource_id = 63;
        YUVVideoDrawQuad::ColorSpace color_space = YUVVideoDrawQuad::JPEG;
        CREATE_SHARED_STATE();

        CREATE_QUAD_11_NEW(YUVVideoDrawQuad, opaque_rect, visible_rect,
            ya_tex_coord_rect, uv_tex_coord_rect, ya_tex_size,
            uv_tex_size, y_plane_resource_id, u_plane_resource_id,
            v_plane_resource_id, a_plane_resource_id, color_space);
        EXPECT_EQ(DrawQuad::YUV_VIDEO_CONTENT, copy_quad->material);
        EXPECT_EQ(opaque_rect, copy_quad->opaque_rect);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(ya_tex_coord_rect, copy_quad->ya_tex_coord_rect);
        EXPECT_EQ(uv_tex_coord_rect, copy_quad->uv_tex_coord_rect);
        EXPECT_EQ(ya_tex_size, copy_quad->ya_tex_size);
        EXPECT_EQ(uv_tex_size, copy_quad->uv_tex_size);
        EXPECT_EQ(y_plane_resource_id, copy_quad->y_plane_resource_id());
        EXPECT_EQ(u_plane_resource_id, copy_quad->u_plane_resource_id());
        EXPECT_EQ(v_plane_resource_id, copy_quad->v_plane_resource_id());
        EXPECT_EQ(a_plane_resource_id, copy_quad->a_plane_resource_id());
        EXPECT_EQ(color_space, copy_quad->color_space);

        CREATE_QUAD_9_ALL(YUVVideoDrawQuad, ya_tex_coord_rect, uv_tex_coord_rect,
            ya_tex_size, uv_tex_size, y_plane_resource_id,
            u_plane_resource_id, v_plane_resource_id,
            a_plane_resource_id, color_space);
        EXPECT_EQ(DrawQuad::YUV_VIDEO_CONTENT, copy_quad->material);
        EXPECT_EQ(ya_tex_coord_rect, copy_quad->ya_tex_coord_rect);
        EXPECT_EQ(uv_tex_coord_rect, copy_quad->uv_tex_coord_rect);
        EXPECT_EQ(ya_tex_size, copy_quad->ya_tex_size);
        EXPECT_EQ(uv_tex_size, copy_quad->uv_tex_size);
        EXPECT_EQ(y_plane_resource_id, copy_quad->y_plane_resource_id());
        EXPECT_EQ(u_plane_resource_id, copy_quad->u_plane_resource_id());
        EXPECT_EQ(v_plane_resource_id, copy_quad->v_plane_resource_id());
        EXPECT_EQ(a_plane_resource_id, copy_quad->a_plane_resource_id());
        EXPECT_EQ(color_space, copy_quad->color_space);
    }

    TEST(DrawQuadTest, CopyPictureDrawQuad)
    {
        gfx::Rect opaque_rect(33, 44, 22, 33);
        gfx::Rect visible_rect(40, 50, 30, 20);
        gfx::RectF tex_coord_rect(31.f, 12.f, 54.f, 20.f);
        gfx::Size texture_size(85, 32);
        bool nearest_neighbor = true;
        ResourceFormat texture_format = RGBA_8888;
        gfx::Rect content_rect(30, 40, 20, 30);
        float contents_scale = 3.141592f;
        scoped_refptr<RasterSource> raster_source = FakeDisplayListRasterSource::CreateEmpty(gfx::Size(100, 100));
        CREATE_SHARED_STATE();

        CREATE_QUAD_9_NEW(PictureDrawQuad, opaque_rect, visible_rect, tex_coord_rect,
            texture_size, nearest_neighbor, texture_format,
            content_rect, contents_scale, raster_source);
        EXPECT_EQ(DrawQuad::PICTURE_CONTENT, copy_quad->material);
        EXPECT_EQ(opaque_rect, copy_quad->opaque_rect);
        EXPECT_EQ(visible_rect, copy_quad->visible_rect);
        EXPECT_EQ(tex_coord_rect, copy_quad->tex_coord_rect);
        EXPECT_EQ(texture_size, copy_quad->texture_size);
        EXPECT_EQ(nearest_neighbor, copy_quad->nearest_neighbor);
        EXPECT_EQ(texture_format, copy_quad->texture_format);
        EXPECT_EQ(content_rect, copy_quad->content_rect);
        EXPECT_EQ(contents_scale, copy_quad->contents_scale);
        EXPECT_EQ(raster_source, copy_quad->raster_source);

        CREATE_QUAD_7_ALL(PictureDrawQuad, tex_coord_rect, texture_size,
            nearest_neighbor, texture_format, content_rect,
            contents_scale, raster_source);
        EXPECT_EQ(DrawQuad::PICTURE_CONTENT, copy_quad->material);
        EXPECT_EQ(tex_coord_rect, copy_quad->tex_coord_rect);
        EXPECT_EQ(texture_size, copy_quad->texture_size);
        EXPECT_EQ(nearest_neighbor, copy_quad->nearest_neighbor);
        EXPECT_EQ(texture_format, copy_quad->texture_format);
        EXPECT_EQ(content_rect, copy_quad->content_rect);
        EXPECT_EQ(contents_scale, copy_quad->contents_scale);
        EXPECT_EQ(raster_source, copy_quad->raster_source);
    }

    class DrawQuadIteratorTest : public testing::Test {
    protected:
        int IterateAndCount(DrawQuad* quad)
        {
            num_resources_ = 0;
            for (ResourceId& resource_id : quad->resources) {
                ++num_resources_;
                ++resource_id;
            }
            return num_resources_;
        }

    private:
        int num_resources_;
    };

    TEST_F(DrawQuadIteratorTest, DebugBorderDrawQuad)
    {
        gfx::Rect visible_rect(40, 50, 30, 20);
        SkColor color = 0xfabb0011;
        int width = 99;

        CREATE_SHARED_STATE();
        CREATE_QUAD_3_NEW(DebugBorderDrawQuad, visible_rect, color, width);
        EXPECT_EQ(0, IterateAndCount(quad_new));
    }

    TEST_F(DrawQuadIteratorTest, IOSurfaceDrawQuad)
    {
        gfx::Rect opaque_rect(33, 47, 10, 12);
        gfx::Rect visible_rect(40, 50, 30, 20);
        gfx::Size size(58, 95);
        ResourceId resource_id = 72;
        IOSurfaceDrawQuad::Orientation orientation = IOSurfaceDrawQuad::UNFLIPPED;
        bool allow_overlay = true;

        CREATE_SHARED_STATE();
        CREATE_QUAD_6_NEW(IOSurfaceDrawQuad,
            opaque_rect,
            visible_rect,
            size,
            resource_id,
            orientation,
            allow_overlay);
        EXPECT_EQ(resource_id, quad_new->io_surface_resource_id());
        EXPECT_EQ(1, IterateAndCount(quad_new));
        EXPECT_EQ(resource_id + 1, quad_new->io_surface_resource_id());
        EXPECT_EQ(allow_overlay, copy_quad->allow_overlay);
    }

    TEST_F(DrawQuadIteratorTest, RenderPassDrawQuad)
    {
        gfx::Rect visible_rect(40, 50, 30, 20);
        RenderPassId render_pass_id(22, 64);
        ResourceId mask_resource_id = 78;
        gfx::Vector2dF mask_uv_scale(33.f, 19.f);
        gfx::Size mask_texture_size(128, 134);
        FilterOperations filters;
        filters.Append(FilterOperation::CreateBlurFilter(1.f));
        gfx::Vector2dF filters_scale(2.f, 3.f);
        FilterOperations background_filters;
        background_filters.Append(
            FilterOperation::CreateGrayscaleFilter(1.f));

        RenderPassId copied_render_pass_id(235, 11);

        CREATE_SHARED_STATE();
        CREATE_QUAD_NEW_RP(RenderPassDrawQuad,
            visible_rect,
            render_pass_id,
            mask_resource_id,
            mask_uv_scale,
            mask_texture_size,
            filters,
            filters_scale,
            background_filters,
            copied_render_pass_id);
        EXPECT_EQ(mask_resource_id, quad_new->mask_resource_id());
        EXPECT_EQ(1, IterateAndCount(quad_new));
        EXPECT_EQ(mask_resource_id + 1, quad_new->mask_resource_id());

        ResourceId new_mask_resource_id = 0;
        gfx::Rect quad_rect(30, 40, 50, 60);
        quad_new->SetNew(shared_state, quad_rect, visible_rect, render_pass_id,
            new_mask_resource_id, mask_uv_scale, mask_texture_size,
            filters, filters_scale, background_filters);
        EXPECT_EQ(0, IterateAndCount(quad_new));
        EXPECT_EQ(0u, quad_new->mask_resource_id());
    }

    TEST_F(DrawQuadIteratorTest, SolidColorDrawQuad)
    {
        gfx::Rect visible_rect(40, 50, 30, 20);
        SkColor color = 0x49494949;
        bool force_anti_aliasing_off = false;

        CREATE_SHARED_STATE();
        CREATE_QUAD_3_NEW(
            SolidColorDrawQuad, visible_rect, color, force_anti_aliasing_off);
        EXPECT_EQ(0, IterateAndCount(quad_new));
    }

    TEST_F(DrawQuadIteratorTest, StreamVideoDrawQuad)
    {
        gfx::Rect opaque_rect(33, 47, 10, 12);
        gfx::Rect visible_rect(40, 50, 30, 20);
        ResourceId resource_id = 64;
        gfx::Size resource_size_in_pixels = gfx::Size(40, 41);
        bool allow_overlay = true;
        gfx::Transform matrix = gfx::Transform(0.5, 0.25, 1, 0.75, 0, 1);

        CREATE_SHARED_STATE();
        CREATE_QUAD_6_NEW(StreamVideoDrawQuad, opaque_rect, visible_rect, resource_id,
            resource_size_in_pixels, allow_overlay, matrix);
        EXPECT_EQ(resource_id, quad_new->resource_id());
        EXPECT_EQ(allow_overlay, quad_new->allow_overlay());
        EXPECT_EQ(resource_size_in_pixels, quad_new->resource_size_in_pixels());
        EXPECT_EQ(1, IterateAndCount(quad_new));
        EXPECT_EQ(resource_id + 1, quad_new->resource_id());
    }

    TEST_F(DrawQuadIteratorTest, SurfaceDrawQuad)
    {
        gfx::Rect visible_rect(40, 50, 30, 20);
        SurfaceId surface_id(4321);

        CREATE_SHARED_STATE();
        CREATE_QUAD_2_NEW(SurfaceDrawQuad, visible_rect, surface_id);
        EXPECT_EQ(0, IterateAndCount(quad_new));
    }

    TEST_F(DrawQuadIteratorTest, TextureDrawQuad)
    {
        gfx::Rect opaque_rect(33, 47, 10, 12);
        gfx::Rect visible_rect(40, 50, 30, 20);
        unsigned resource_id = 82;
        bool premultiplied_alpha = true;
        gfx::PointF uv_top_left(0.5f, 224.f);
        gfx::PointF uv_bottom_right(51.5f, 260.f);
        const float vertex_opacity[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        bool y_flipped = true;
        bool nearest_neighbor = true;

        CREATE_SHARED_STATE();
        CREATE_QUAD_10_NEW(TextureDrawQuad,
            opaque_rect,
            visible_rect,
            resource_id,
            premultiplied_alpha,
            uv_top_left,
            uv_bottom_right,
            SK_ColorTRANSPARENT,
            vertex_opacity,
            y_flipped,
            nearest_neighbor);
        EXPECT_EQ(resource_id, quad_new->resource_id());
        EXPECT_EQ(1, IterateAndCount(quad_new));
        EXPECT_EQ(resource_id + 1, quad_new->resource_id());
    }

    TEST_F(DrawQuadIteratorTest, TileDrawQuad)
    {
        gfx::Rect opaque_rect(33, 44, 22, 33);
        gfx::Rect visible_rect(40, 50, 30, 20);
        unsigned resource_id = 104;
        gfx::RectF tex_coord_rect(31.f, 12.f, 54.f, 20.f);
        gfx::Size texture_size(85, 32);
        bool swizzle_contents = true;
        bool nearest_neighbor = true;

        CREATE_SHARED_STATE();
        CREATE_QUAD_7_NEW(TileDrawQuad,
            opaque_rect,
            visible_rect,
            resource_id,
            tex_coord_rect,
            texture_size,
            swizzle_contents,
            nearest_neighbor);
        EXPECT_EQ(resource_id, quad_new->resource_id());
        EXPECT_EQ(1, IterateAndCount(quad_new));
        EXPECT_EQ(resource_id + 1, quad_new->resource_id());
    }

    TEST_F(DrawQuadIteratorTest, YUVVideoDrawQuad)
    {
        gfx::Rect opaque_rect(33, 47, 10, 12);
        gfx::Rect visible_rect(40, 50, 30, 20);
        gfx::RectF ya_tex_coord_rect(0.0f, 0.0f, 0.75f, 0.5f);
        gfx::RectF uv_tex_coord_rect(0.0f, 0.0f, 0.375f, 0.25f);
        gfx::Size ya_tex_size(32, 68);
        gfx::Size uv_tex_size(41, 51);
        ResourceId y_plane_resource_id = 45;
        ResourceId u_plane_resource_id = 532;
        ResourceId v_plane_resource_id = 4;
        ResourceId a_plane_resource_id = 63;
        YUVVideoDrawQuad::ColorSpace color_space = YUVVideoDrawQuad::JPEG;

        CREATE_SHARED_STATE();
        CREATE_QUAD_11_NEW(YUVVideoDrawQuad, opaque_rect, visible_rect,
            ya_tex_coord_rect, uv_tex_coord_rect, ya_tex_size,
            uv_tex_size, y_plane_resource_id, u_plane_resource_id,
            v_plane_resource_id, a_plane_resource_id, color_space);
        EXPECT_EQ(DrawQuad::YUV_VIDEO_CONTENT, copy_quad->material);
        EXPECT_EQ(y_plane_resource_id, quad_new->y_plane_resource_id());
        EXPECT_EQ(u_plane_resource_id, quad_new->u_plane_resource_id());
        EXPECT_EQ(v_plane_resource_id, quad_new->v_plane_resource_id());
        EXPECT_EQ(a_plane_resource_id, quad_new->a_plane_resource_id());
        EXPECT_EQ(color_space, quad_new->color_space);
        EXPECT_EQ(4, IterateAndCount(quad_new));
        EXPECT_EQ(y_plane_resource_id + 1, quad_new->y_plane_resource_id());
        EXPECT_EQ(u_plane_resource_id + 1, quad_new->u_plane_resource_id());
        EXPECT_EQ(v_plane_resource_id + 1, quad_new->v_plane_resource_id());
        EXPECT_EQ(a_plane_resource_id + 1, quad_new->a_plane_resource_id());
    }

    // Disabled until picture draw quad is supported for ubercomp: crbug.com/231715
    TEST_F(DrawQuadIteratorTest, DISABLED_PictureDrawQuad)
    {
        gfx::Rect opaque_rect(33, 44, 22, 33);
        gfx::Rect visible_rect(40, 50, 30, 20);
        gfx::RectF tex_coord_rect(31.f, 12.f, 54.f, 20.f);
        gfx::Size texture_size(85, 32);
        bool nearest_neighbor = true;
        ResourceFormat texture_format = RGBA_8888;
        gfx::Rect content_rect(30, 40, 20, 30);
        float contents_scale = 3.141592f;
        scoped_refptr<RasterSource> raster_source = FakeDisplayListRasterSource::CreateEmpty(gfx::Size(100, 100));

        CREATE_SHARED_STATE();
        CREATE_QUAD_9_NEW(PictureDrawQuad, opaque_rect, visible_rect, tex_coord_rect,
            texture_size, nearest_neighbor, texture_format,
            content_rect, contents_scale, raster_source);
        EXPECT_EQ(0, IterateAndCount(quad_new));
    }

    TEST(DrawQuadTest, LargestQuadType)
    {
        size_t largest = 0;

        for (int i = 0; i <= DrawQuad::MATERIAL_LAST; ++i) {
            switch (static_cast<DrawQuad::Material>(i)) {
            case DrawQuad::DEBUG_BORDER:
                largest = std::max(largest, sizeof(DebugBorderDrawQuad));
                break;
            case DrawQuad::IO_SURFACE_CONTENT:
                largest = std::max(largest, sizeof(IOSurfaceDrawQuad));
                break;
            case DrawQuad::PICTURE_CONTENT:
                largest = std::max(largest, sizeof(PictureDrawQuad));
                break;
            case DrawQuad::TEXTURE_CONTENT:
                largest = std::max(largest, sizeof(TextureDrawQuad));
                break;
            case DrawQuad::RENDER_PASS:
                largest = std::max(largest, sizeof(RenderPassDrawQuad));
                break;
            case DrawQuad::SOLID_COLOR:
                largest = std::max(largest, sizeof(SolidColorDrawQuad));
                break;
            case DrawQuad::SURFACE_CONTENT:
                largest = std::max(largest, sizeof(SurfaceDrawQuad));
                break;
            case DrawQuad::TILED_CONTENT:
                largest = std::max(largest, sizeof(TileDrawQuad));
                break;
            case DrawQuad::STREAM_VIDEO_CONTENT:
                largest = std::max(largest, sizeof(StreamVideoDrawQuad));
                break;
            case DrawQuad::YUV_VIDEO_CONTENT:
                largest = std::max(largest, sizeof(YUVVideoDrawQuad));
                break;
            case DrawQuad::INVALID:
                break;
            }
        }
        EXPECT_EQ(LargestDrawQuadSize(), largest);

        if (!HasFailure())
            return;

        // On failure, output the size of all quads for debugging.
        LOG(ERROR) << "largest " << largest;
        LOG(ERROR) << "kLargestDrawQuad " << LargestDrawQuadSize();
        for (int i = 0; i <= DrawQuad::MATERIAL_LAST; ++i) {
            switch (static_cast<DrawQuad::Material>(i)) {
            case DrawQuad::DEBUG_BORDER:
                LOG(ERROR) << "DebugBorderDrawQuad " << sizeof(DebugBorderDrawQuad);
                break;
            case DrawQuad::IO_SURFACE_CONTENT:
                LOG(ERROR) << "IOSurfaceDrawQuad " << sizeof(IOSurfaceDrawQuad);
                break;
            case DrawQuad::PICTURE_CONTENT:
                LOG(ERROR) << "PictureDrawQuad " << sizeof(PictureDrawQuad);
                break;
            case DrawQuad::TEXTURE_CONTENT:
                LOG(ERROR) << "TextureDrawQuad " << sizeof(TextureDrawQuad);
                break;
            case DrawQuad::RENDER_PASS:
                LOG(ERROR) << "RenderPassDrawQuad " << sizeof(RenderPassDrawQuad);
                break;
            case DrawQuad::SOLID_COLOR:
                LOG(ERROR) << "SolidColorDrawQuad " << sizeof(SolidColorDrawQuad);
                break;
            case DrawQuad::SURFACE_CONTENT:
                LOG(ERROR) << "SurfaceDrawQuad " << sizeof(SurfaceDrawQuad);
                break;
            case DrawQuad::TILED_CONTENT:
                LOG(ERROR) << "TileDrawQuad " << sizeof(TileDrawQuad);
                break;
            case DrawQuad::STREAM_VIDEO_CONTENT:
                LOG(ERROR) << "StreamVideoDrawQuad " << sizeof(StreamVideoDrawQuad);
                break;
            case DrawQuad::YUV_VIDEO_CONTENT:
                LOG(ERROR) << "YUVVideoDrawQuad " << sizeof(YUVVideoDrawQuad);
                break;
            case DrawQuad::INVALID:
                break;
            }
        }
    }

} // namespace
} // namespace cc
