// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/font_render_params.h"

#include "base/logging.h"

namespace gfx {

FontRenderParams::FontRenderParams()
    : antialiasing(true)
    , subpixel_positioning(true)
    , autohinter(false)
    , use_bitmaps(false)
    , hinting(HINTING_MEDIUM)
    , subpixel_rendering(SUBPIXEL_RENDERING_NONE)
{
}

FontRenderParams::~FontRenderParams() { }

// static
SkFontHost::LCDOrder FontRenderParams::SubpixelRenderingToSkiaLCDOrder(
    FontRenderParams::SubpixelRendering subpixel_rendering)
{
    switch (subpixel_rendering) {
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_NONE:
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_RGB:
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_VRGB:
        return SkFontHost::kRGB_LCDOrder;
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_BGR:
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_VBGR:
        return SkFontHost::kBGR_LCDOrder;
    }

    NOTREACHED();
    return SkFontHost::kRGB_LCDOrder;
}

// static
SkFontHost::LCDOrientation
FontRenderParams::SubpixelRenderingToSkiaLCDOrientation(
    FontRenderParams::SubpixelRendering subpixel_rendering)
{
    switch (subpixel_rendering) {
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_NONE:
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_RGB:
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_BGR:
        return SkFontHost::kHorizontal_LCDOrientation;
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_VRGB:
    case gfx::FontRenderParams::SUBPIXEL_RENDERING_VBGR:
        return SkFontHost::kVertical_LCDOrientation;
    }

    NOTREACHED();
    return SkFontHost::kHorizontal_LCDOrientation;
}

FontRenderParamsQuery::FontRenderParamsQuery()
    : pixel_size(0)
    , point_size(0)
    , style(-1)
    , device_scale_factor(0)
{
}

FontRenderParamsQuery::~FontRenderParamsQuery() { }

} // namespace gfx
