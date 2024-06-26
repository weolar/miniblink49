// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/render_surface_draw_properties.h"

namespace cc {

RenderSurfaceDrawProperties::RenderSurfaceDrawProperties()
    : draw_opacity(0.f)
    , is_clipped(false)
{
}

RenderSurfaceDrawProperties::~RenderSurfaceDrawProperties() { }

} // namespace cc
