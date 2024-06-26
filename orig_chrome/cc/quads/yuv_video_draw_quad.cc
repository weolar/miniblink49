// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/yuv_video_draw_quad.h"

#include "base/logging.h"
#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"
#include "cc/base/math_util.h"

namespace cc {

YUVVideoDrawQuad::YUVVideoDrawQuad()
{
}
YUVVideoDrawQuad::~YUVVideoDrawQuad() { }

void YUVVideoDrawQuad::SetNew(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& opaque_rect,
    const gfx::Rect& visible_rect,
    const gfx::RectF& ya_tex_coord_rect,
    const gfx::RectF& uv_tex_coord_rect,
    const gfx::Size& ya_tex_size,
    const gfx::Size& uv_tex_size,
    unsigned y_plane_resource_id,
    unsigned u_plane_resource_id,
    unsigned v_plane_resource_id,
    unsigned a_plane_resource_id,
    ColorSpace color_space)
{
    bool needs_blending = false;
    DrawQuad::SetAll(shared_quad_state, DrawQuad::YUV_VIDEO_CONTENT, rect,
        opaque_rect, visible_rect, needs_blending);
    this->ya_tex_coord_rect = ya_tex_coord_rect;
    this->uv_tex_coord_rect = uv_tex_coord_rect;
    this->ya_tex_size = ya_tex_size;
    this->uv_tex_size = uv_tex_size;
    resources.ids[kYPlaneResourceIdIndex] = y_plane_resource_id;
    resources.ids[kUPlaneResourceIdIndex] = u_plane_resource_id;
    resources.ids[kVPlaneResourceIdIndex] = v_plane_resource_id;
    resources.ids[kAPlaneResourceIdIndex] = a_plane_resource_id;
    resources.count = a_plane_resource_id ? 4 : 3;
    this->color_space = color_space;
}

void YUVVideoDrawQuad::SetAll(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& opaque_rect,
    const gfx::Rect& visible_rect,
    bool needs_blending,
    const gfx::RectF& ya_tex_coord_rect,
    const gfx::RectF& uv_tex_coord_rect,
    const gfx::Size& ya_tex_size,
    const gfx::Size& uv_tex_size,
    unsigned y_plane_resource_id,
    unsigned u_plane_resource_id,
    unsigned v_plane_resource_id,
    unsigned a_plane_resource_id,
    ColorSpace color_space)
{
    DrawQuad::SetAll(shared_quad_state, DrawQuad::YUV_VIDEO_CONTENT, rect,
        opaque_rect, visible_rect, needs_blending);
    this->ya_tex_coord_rect = ya_tex_coord_rect;
    this->uv_tex_coord_rect = uv_tex_coord_rect;
    this->ya_tex_size = ya_tex_size;
    this->uv_tex_size = uv_tex_size;
    resources.ids[kYPlaneResourceIdIndex] = y_plane_resource_id;
    resources.ids[kUPlaneResourceIdIndex] = u_plane_resource_id;
    resources.ids[kVPlaneResourceIdIndex] = v_plane_resource_id;
    resources.ids[kAPlaneResourceIdIndex] = a_plane_resource_id;
    resources.count = resources.ids[kAPlaneResourceIdIndex] ? 4 : 3;
    this->color_space = color_space;
}

const YUVVideoDrawQuad* YUVVideoDrawQuad::MaterialCast(
    const DrawQuad* quad)
{
    DCHECK(quad->material == DrawQuad::YUV_VIDEO_CONTENT);
    return static_cast<const YUVVideoDrawQuad*>(quad);
}

void YUVVideoDrawQuad::ExtendValue(
    base::trace_event::TracedValue* value) const
{
    MathUtil::AddToTracedValue("ya_tex_coord_rect", ya_tex_coord_rect, value);
    MathUtil::AddToTracedValue("uv_tex_coord_rect", uv_tex_coord_rect, value);
    MathUtil::AddToTracedValue("ya_tex_size", ya_tex_size, value);
    MathUtil::AddToTracedValue("uv_tex_size", uv_tex_size, value);
    value->SetInteger("y_plane_resource_id",
        resources.ids[kYPlaneResourceIdIndex]);
    value->SetInteger("u_plane_resource_id",
        resources.ids[kUPlaneResourceIdIndex]);
    value->SetInteger("v_plane_resource_id",
        resources.ids[kVPlaneResourceIdIndex]);
    value->SetInteger("a_plane_resource_id",
        resources.ids[kAPlaneResourceIdIndex]);
}

} // namespace cc
