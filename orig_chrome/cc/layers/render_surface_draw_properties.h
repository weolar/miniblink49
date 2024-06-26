// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_RENDER_SURFACE_DRAW_PROPERTIES_H_
#define CC_LAYERS_RENDER_SURFACE_DRAW_PROPERTIES_H_

#include "cc/base/cc_export.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/transform.h"

namespace cc {

// Container for properties that render surfaces need to compute before they can
// be drawn.
struct CC_EXPORT RenderSurfaceDrawProperties {
    RenderSurfaceDrawProperties();
    ~RenderSurfaceDrawProperties();

    float draw_opacity;

    // Transforms from the surface's own space to the space of its target surface.
    gfx::Transform draw_transform;

    // Transforms from the surface's own space to the viewport.
    gfx::Transform screen_space_transform;

    // If the surface has a replica, these transform from the replica's space to
    // the space of the target surface and the viewport.
    gfx::Transform replica_draw_transform;
    gfx::Transform replica_screen_space_transform;

    // This is in the surface's own space.
    gfx::Rect content_rect;

    // This is in the space of the surface's target surface.
    gfx::Rect clip_rect;

    // True if the surface needs to be clipped by clip_rect.
    bool is_clipped;
};

} // namespace cc

#endif // CC_LAYERS_RENDER_SURFACE_DRAW_PROPERTIES_H_
