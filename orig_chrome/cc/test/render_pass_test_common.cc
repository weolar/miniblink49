// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/render_pass_test_common.h"

#include "base/bind.h"
#include "cc/quads/checkerboard_draw_quad.h"
#include "cc/quads/debug_border_draw_quad.h"
#include "cc/quads/io_surface_draw_quad.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/shared_quad_state.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/stream_video_draw_quad.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/quads/tile_draw_quad.h"
#include "cc/quads/yuv_video_draw_quad.h"
#include "cc/resources/resource_provider.h"
#include "cc/trees/blocking_task_runner.h"
#include "ui/gfx/transform.h"

namespace cc {

static void EmptyReleaseCallback(uint32 sync_point,
    bool lost_resource,
    BlockingTaskRunner* main_thread_task_runner)
{
}

void TestRenderPass::AppendOneOfEveryQuadType(
    ResourceProvider* resource_provider,
    RenderPassId child_pass)
{
    gfx::Rect rect(0, 0, 100, 100);
    gfx::Rect opaque_rect(10, 10, 80, 80);
    gfx::Rect visible_rect(0, 0, 100, 100);
    const float vertex_opacity[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    ResourceId resource1 = resource_provider->CreateResource(
        gfx::Size(45, 5), GL_CLAMP_TO_EDGE,
        ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        resource_provider->best_texture_format());
    resource_provider->AllocateForTesting(resource1);
    ResourceId resource2 = resource_provider->CreateResource(
        gfx::Size(346, 61), GL_CLAMP_TO_EDGE,
        ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        resource_provider->best_texture_format());
    resource_provider->AllocateForTesting(resource2);
    ResourceId resource3 = resource_provider->CreateResource(
        gfx::Size(12, 134), GL_CLAMP_TO_EDGE,
        ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        resource_provider->best_texture_format());
    resource_provider->AllocateForTesting(resource3);
    ResourceId resource4 = resource_provider->CreateResource(
        gfx::Size(56, 12), GL_CLAMP_TO_EDGE,
        ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        resource_provider->best_texture_format());
    resource_provider->AllocateForTesting(resource4);
    gfx::Size resource5_size(73, 26);
    ResourceId resource5 = resource_provider->CreateResource(
        resource5_size, GL_CLAMP_TO_EDGE,
        ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        resource_provider->best_texture_format());
    resource_provider->AllocateForTesting(resource5);
    ResourceId resource6 = resource_provider->CreateResource(
        gfx::Size(64, 92), GL_CLAMP_TO_EDGE,
        ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        resource_provider->best_texture_format());
    resource_provider->AllocateForTesting(resource6);
    ResourceId resource7 = resource_provider->CreateResource(
        gfx::Size(9, 14), GL_CLAMP_TO_EDGE,
        ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        resource_provider->best_texture_format());
    resource_provider->AllocateForTesting(resource7);

    unsigned target = GL_TEXTURE_2D;
    gpu::Mailbox gpu_mailbox;
    memcpy(gpu_mailbox.name, "Hello world", strlen("Hello world") + 1);
    scoped_ptr<SingleReleaseCallbackImpl> callback = SingleReleaseCallbackImpl::Create(base::Bind(&EmptyReleaseCallback));
    TextureMailbox mailbox(gpu_mailbox, target, kSyncPointForMailboxTextureQuad);
    ResourceId resource8 = resource_provider->CreateResourceFromTextureMailbox(
        mailbox, callback.Pass());
    resource_provider->AllocateForTesting(resource8);

    SharedQuadState* shared_state = this->CreateAndAppendSharedQuadState();
    shared_state->SetAll(gfx::Transform(),
        rect.size(),
        rect,
        rect,
        false,
        1,
        SkXfermode::kSrcOver_Mode,
        0);

    CheckerboardDrawQuad* checkerboard_quad = this->CreateAndAppendDrawQuad<CheckerboardDrawQuad>();
    checkerboard_quad->SetNew(shared_state, rect, visible_rect, SK_ColorRED, 1.f);

    DebugBorderDrawQuad* debug_border_quad = this->CreateAndAppendDrawQuad<DebugBorderDrawQuad>();
    debug_border_quad->SetNew(shared_state, rect, visible_rect, SK_ColorRED, 1);

    IOSurfaceDrawQuad* io_surface_quad = this->CreateAndAppendDrawQuad<IOSurfaceDrawQuad>();
    io_surface_quad->SetNew(shared_state,
        rect,
        opaque_rect,
        visible_rect,
        gfx::Size(50, 50),
        resource7,
        IOSurfaceDrawQuad::FLIPPED);

    if (child_pass.layer_id) {
        RenderPassDrawQuad* render_pass_quad = this->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        render_pass_quad->SetNew(shared_state,
            rect,
            visible_rect,
            child_pass,
            resource5,
            gfx::Vector2dF(1.f, 1.f),
            resource5_size,
            FilterOperations(),
            gfx::Vector2dF(),
            FilterOperations());

        RenderPassDrawQuad* render_pass_replica_quad = this->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        render_pass_replica_quad->SetNew(shared_state,
            rect,
            visible_rect,
            child_pass,
            resource5,
            gfx::Vector2dF(1.f, 1.f),
            resource5_size,
            FilterOperations(),
            gfx::Vector2dF(),
            FilterOperations());
    }

    SolidColorDrawQuad* solid_color_quad = this->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
    solid_color_quad->SetNew(
        shared_state, rect, visible_rect, SK_ColorRED, false);

    StreamVideoDrawQuad* stream_video_quad = this->CreateAndAppendDrawQuad<StreamVideoDrawQuad>();
    stream_video_quad->SetNew(shared_state, rect, opaque_rect, visible_rect,
        resource6, gfx::Size(), false, gfx::Transform());

    TextureDrawQuad* texture_quad = this->CreateAndAppendDrawQuad<TextureDrawQuad>();
    texture_quad->SetNew(shared_state,
        rect,
        opaque_rect,
        visible_rect,
        resource1,
        false,
        gfx::PointF(0.f, 0.f),
        gfx::PointF(1.f, 1.f),
        SK_ColorTRANSPARENT,
        vertex_opacity,
        false,
        false);

    TextureDrawQuad* mailbox_texture_quad = this->CreateAndAppendDrawQuad<TextureDrawQuad>();
    mailbox_texture_quad->SetNew(shared_state,
        rect,
        opaque_rect,
        visible_rect,
        resource8,
        false,
        gfx::PointF(0.f, 0.f),
        gfx::PointF(1.f, 1.f),
        SK_ColorTRANSPARENT,
        vertex_opacity,
        false,
        false);

    TileDrawQuad* scaled_tile_quad = this->CreateAndAppendDrawQuad<TileDrawQuad>();
    scaled_tile_quad->SetNew(shared_state,
        rect,
        opaque_rect,
        visible_rect,
        resource2,
        gfx::RectF(0, 0, 50, 50),
        gfx::Size(50, 50),
        false,
        false);

    SharedQuadState* transformed_state = this->CreateAndAppendSharedQuadState();
    transformed_state->CopyFrom(shared_state);
    gfx::Transform rotation;
    rotation.Rotate(45);
    transformed_state->quad_to_target_transform = transformed_state->quad_to_target_transform * rotation;
    TileDrawQuad* transformed_tile_quad = this->CreateAndAppendDrawQuad<TileDrawQuad>();
    transformed_tile_quad->SetNew(transformed_state,
        rect,
        opaque_rect,
        visible_rect,
        resource3,
        gfx::RectF(0, 0, 100, 100),
        gfx::Size(100, 100),
        false,
        false);

    SharedQuadState* shared_state2 = this->CreateAndAppendSharedQuadState();
    shared_state->SetAll(gfx::Transform(),
        rect.size(),
        rect,
        rect,
        false,
        1,
        SkXfermode::kSrcOver_Mode,
        0);

    TileDrawQuad* tile_quad = this->CreateAndAppendDrawQuad<TileDrawQuad>();
    tile_quad->SetNew(shared_state2,
        rect,
        opaque_rect,
        visible_rect,
        resource4,
        gfx::RectF(0, 0, 100, 100),
        gfx::Size(100, 100),
        false,
        false);

    ResourceId plane_resources[4];
    for (int i = 0; i < 4; ++i) {
        plane_resources[i] = resource_provider->CreateResource(
            gfx::Size(20, 12), GL_CLAMP_TO_EDGE,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            resource_provider->best_texture_format());
        resource_provider->AllocateForTesting(plane_resources[i]);
    }
    YUVVideoDrawQuad::ColorSpace color_space = YUVVideoDrawQuad::REC_601;
    YUVVideoDrawQuad* yuv_quad = this->CreateAndAppendDrawQuad<YUVVideoDrawQuad>();
    yuv_quad->SetNew(shared_state2, rect, opaque_rect, visible_rect,
        gfx::RectF(.0f, .0f, 100.0f, 100.0f),
        gfx::RectF(.0f, .0f, 50.0f, 50.0f), gfx::Size(100, 100),
        gfx::Size(50, 50), plane_resources[0], plane_resources[1],
        plane_resources[2], plane_resources[3], color_space);
}

} // namespace cc
