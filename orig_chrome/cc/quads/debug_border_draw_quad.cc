// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/debug_border_draw_quad.h"

#include "base/logging.h"
#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"

namespace cc {

DebugBorderDrawQuad::DebugBorderDrawQuad()
    : color(0)
    , width(0)
{
}

void DebugBorderDrawQuad::SetNew(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& visible_rect,
    SkColor color,
    int width)
{
    gfx::Rect opaque_rect;
    bool needs_blending = SkColorGetA(color) < 255;
    DrawQuad::SetAll(shared_quad_state, DrawQuad::DEBUG_BORDER, rect, opaque_rect,
        visible_rect, needs_blending);
    this->color = color;
    this->width = width;
}

void DebugBorderDrawQuad::SetAll(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& opaque_rect,
    const gfx::Rect& visible_rect,
    bool needs_blending,
    SkColor color,
    int width)
{
    DrawQuad::SetAll(shared_quad_state, DrawQuad::DEBUG_BORDER, rect, opaque_rect,
        visible_rect, needs_blending);
    this->color = color;
    this->width = width;
}

const DebugBorderDrawQuad* DebugBorderDrawQuad::MaterialCast(
    const DrawQuad* quad)
{
    DCHECK(quad->material == DrawQuad::DEBUG_BORDER);
    return static_cast<const DebugBorderDrawQuad*>(quad);
}

void DebugBorderDrawQuad::ExtendValue(
    base::trace_event::TracedValue* value) const
{
    value->SetInteger("color", color);
    value->SetInteger("width", width);
}

} // namespace cc
