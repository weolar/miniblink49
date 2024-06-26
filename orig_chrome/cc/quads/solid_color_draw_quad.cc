// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/solid_color_draw_quad.h"

#include "base/logging.h"
#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"

namespace cc {

SolidColorDrawQuad::SolidColorDrawQuad()
    : color(0)
    , force_anti_aliasing_off(false)
{
}

void SolidColorDrawQuad::SetNew(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& visible_rect,
    SkColor color,
    bool force_anti_aliasing_off)
{
    gfx::Rect opaque_rect = SkColorGetA(color) == 255 ? rect : gfx::Rect();
    bool needs_blending = false;
    DrawQuad::SetAll(shared_quad_state, DrawQuad::SOLID_COLOR, rect, opaque_rect,
        visible_rect, needs_blending);
    this->color = color;
    this->force_anti_aliasing_off = force_anti_aliasing_off;
}

void SolidColorDrawQuad::SetAll(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& opaque_rect,
    const gfx::Rect& visible_rect,
    bool needs_blending,
    SkColor color,
    bool force_anti_aliasing_off)
{
    DrawQuad::SetAll(shared_quad_state, DrawQuad::SOLID_COLOR, rect, opaque_rect,
        visible_rect, needs_blending);
    this->color = color;
    this->force_anti_aliasing_off = force_anti_aliasing_off;
}

const SolidColorDrawQuad* SolidColorDrawQuad::MaterialCast(
    const DrawQuad* quad)
{
    DCHECK(quad->material == DrawQuad::SOLID_COLOR);
    return static_cast<const SolidColorDrawQuad*>(quad);
}

void SolidColorDrawQuad::ExtendValue(
    base::trace_event::TracedValue* value) const
{
    value->SetInteger("color", color);
    value->SetBoolean("force_anti_aliasing_off", force_anti_aliasing_off);
}

} // namespace cc
