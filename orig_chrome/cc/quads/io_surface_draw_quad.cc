// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/io_surface_draw_quad.h"

#include "base/logging.h"
#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"
#include "cc/base/math_util.h"

namespace cc {

IOSurfaceDrawQuad::IOSurfaceDrawQuad()
    : orientation(FLIPPED)
    , allow_overlay(false)
{
}

void IOSurfaceDrawQuad::SetNew(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& opaque_rect,
    const gfx::Rect& visible_rect,
    const gfx::Size& io_surface_size,
    unsigned io_surface_resource_id,
    Orientation orientation,
    bool allow_overlay)
{
    bool needs_blending = false;
    DrawQuad::SetAll(shared_quad_state, DrawQuad::IO_SURFACE_CONTENT, rect,
        opaque_rect, visible_rect, needs_blending);
    this->io_surface_size = io_surface_size;
    resources.ids[kIOSurfaceResourceIdIndex] = io_surface_resource_id;
    resources.count = 1;
    this->orientation = orientation;
    this->allow_overlay = allow_overlay;
}

void IOSurfaceDrawQuad::SetAll(const SharedQuadState* shared_quad_state,
    const gfx::Rect& rect,
    const gfx::Rect& opaque_rect,
    const gfx::Rect& visible_rect,
    bool needs_blending,
    const gfx::Size& io_surface_size,
    unsigned io_surface_resource_id,
    Orientation orientation,
    bool allow_overlay)
{
    DrawQuad::SetAll(shared_quad_state, DrawQuad::IO_SURFACE_CONTENT, rect,
        opaque_rect, visible_rect, needs_blending);
    this->io_surface_size = io_surface_size;
    resources.ids[kIOSurfaceResourceIdIndex] = io_surface_resource_id;
    resources.count = 1;
    this->orientation = orientation;
    this->allow_overlay = allow_overlay;
}

const IOSurfaceDrawQuad* IOSurfaceDrawQuad::MaterialCast(
    const DrawQuad* quad)
{
    DCHECK(quad->material == DrawQuad::IO_SURFACE_CONTENT);
    return static_cast<const IOSurfaceDrawQuad*>(quad);
}

void IOSurfaceDrawQuad::ExtendValue(
    base::trace_event::TracedValue* value) const
{
    MathUtil::AddToTracedValue("io_surface_size", io_surface_size, value);

    value->SetInteger("io_surface_resource_id",
        resources.ids[kIOSurfaceResourceIdIndex]);
    const char* orientation_string = NULL;
    switch (orientation) {
    case FLIPPED:
        orientation_string = "flipped";
        break;
    case UNFLIPPED:
        orientation_string = "unflipped";
        break;
    }

    value->SetString("orientation", orientation_string);
}

} // namespace cc
