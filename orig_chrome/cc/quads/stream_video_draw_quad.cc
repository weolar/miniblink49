// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/stream_video_draw_quad.h"

#include "base/logging.h"
#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"
#include "cc/base/math_util.h"

namespace cc {

StreamVideoDrawQuad::StreamVideoDrawQuad()
{
}

void StreamVideoDrawQuad::SetNew(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& opaque_rect,
    const gfx::Rect& visible_rect,
    unsigned resource_id,
    gfx::Size resource_size_in_pixels,
    bool allow_overlay,
    const gfx::Transform& matrix)
{
    bool needs_blending = false;
    DrawQuad::SetAll(shared_quad_state, DrawQuad::STREAM_VIDEO_CONTENT, rect,
        opaque_rect, visible_rect, needs_blending);
    resources.ids[kResourceIdIndex] = resource_id;
    overlay_resources.size_in_pixels[kResourceIdIndex] = resource_size_in_pixels;
    overlay_resources.allow_overlay[kResourceIdIndex] = allow_overlay;
    resources.count = 1;
    this->matrix = matrix;
}

void StreamVideoDrawQuad::SetAll(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& opaque_rect,
    const gfx::Rect& visible_rect,
    bool needs_blending,
    unsigned resource_id,
    gfx::Size resource_size_in_pixels,
    bool allow_overlay,
    const gfx::Transform& matrix)
{
    DrawQuad::SetAll(shared_quad_state, DrawQuad::STREAM_VIDEO_CONTENT, rect,
        opaque_rect, visible_rect, needs_blending);
    resources.ids[kResourceIdIndex] = resource_id;
    overlay_resources.size_in_pixels[kResourceIdIndex] = resource_size_in_pixels;
    overlay_resources.allow_overlay[kResourceIdIndex] = allow_overlay;
    resources.count = 1;
    this->matrix = matrix;
}

const StreamVideoDrawQuad* StreamVideoDrawQuad::MaterialCast(
    const DrawQuad* quad)
{
    DCHECK(quad->material == DrawQuad::STREAM_VIDEO_CONTENT);
    return static_cast<const StreamVideoDrawQuad*>(quad);
}

void StreamVideoDrawQuad::ExtendValue(
    base::trace_event::TracedValue* value) const
{
    value->SetInteger("resource_id", resources.ids[kResourceIdIndex]);
    MathUtil::AddToTracedValue("matrix", matrix, value);
}

StreamVideoDrawQuad::OverlayResources::OverlayResources()
{
    for (size_t i = 0; i < Resources::kMaxResourceIdCount; ++i)
        allow_overlay[i] = false;
}

} // namespace cc
