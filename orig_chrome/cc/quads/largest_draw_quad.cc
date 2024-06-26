// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/largest_draw_quad.h"

#include <algorithm>

#include "cc/quads/debug_border_draw_quad.h"
#include "cc/quads/io_surface_draw_quad.h"
#include "cc/quads/picture_draw_quad.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/stream_video_draw_quad.h"
#include "cc/quads/surface_draw_quad.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/quads/tile_draw_quad.h"
#include "cc/quads/yuv_video_draw_quad.h"

namespace {
const size_t kLargestDrawQuadSize = sizeof(cc::RenderPassDrawQuad) > sizeof(cc::StreamVideoDrawQuad)
    ? sizeof(cc::RenderPassDrawQuad)
    : sizeof(cc::StreamVideoDrawQuad);
} // namespace

namespace cc {

size_t LargestDrawQuadSize()
{
    // Currently the largest quad is either a RenderPassDrawQuad or a
    // StreamVideoDrawQuad depends on hardware structure.

    // Use compile assert to make sure largest is actually larger than all other
    // type of draw quads.
    static_assert(sizeof(DebugBorderDrawQuad) <= kLargestDrawQuadSize,
        "Largest Draw Quad size needs update. DebugBorderDrawQuad is "
        "currently largest.");
    static_assert(sizeof(IOSurfaceDrawQuad) <= kLargestDrawQuadSize,
        "Largest Draw Quad size needs update. IOSurfaceDrawQuad is "
        "currently largest.");
    static_assert(sizeof(PictureDrawQuad) <= kLargestDrawQuadSize,
        "Largest Draw Quad size needs update. PictureDrawQuad is "
        "currently largest.");
    static_assert(sizeof(TextureDrawQuad) <= kLargestDrawQuadSize,
        "Largest Draw Quad size needs update. TextureDrawQuad is "
        "currently largest.");
    static_assert(sizeof(SolidColorDrawQuad) <= kLargestDrawQuadSize,
        "Largest Draw Quad size needs update. SolidColorDrawQuad is "
        "currently largest.");
    static_assert(sizeof(SurfaceDrawQuad) <= kLargestDrawQuadSize,
        "Largest Draw Quad size needs update. SurfaceDrawQuad is "
        "currently largest.");
    static_assert(sizeof(TileDrawQuad) <= kLargestDrawQuadSize,
        "Largest Draw Quad size needs update. TileDrawQuad is "
        "currently largest.");
    static_assert(sizeof(YUVVideoDrawQuad) <= kLargestDrawQuadSize,
        "Largest Draw Quad size needs update. YUVVideoDrawQuad is "
        "currently largest.");

    return kLargestDrawQuadSize;
}

} // namespace cc
