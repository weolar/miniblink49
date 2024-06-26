// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_surface_overlay.h"

#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/overlay_transform.h"
#include "ui/gl/gl_image.h"

namespace gfx {

GLSurfaceOverlay::GLSurfaceOverlay(int z_order,
    OverlayTransform transform,
    gl::GLImage* image,
    const Rect& bounds_rect,
    const RectF& crop_rect)
    : z_order_(z_order)
    , transform_(transform)
    , image_(image)
    , bounds_rect_(bounds_rect)
    , crop_rect_(crop_rect)
{
}

GLSurfaceOverlay::~GLSurfaceOverlay() { }

bool GLSurfaceOverlay::ScheduleOverlayPlane(AcceleratedWidget widget) const
{
    return image_->ScheduleOverlayPlane(widget, z_order_, transform_,
        bounds_rect_, crop_rect_);
}

} // namespace gfx
