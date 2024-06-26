// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_GEOMETRY_DIP_UTIL_H_
#define UI_GFX_GEOMETRY_DIP_UTIL_H_

#include "ui/gfx/gfx_export.h"

namespace gfx {

class Point;
class PointF;
class Rect;
class Size;

GFX_EXPORT gfx::Point ConvertPointToDIP(float scale_factor,
    const gfx::Point& point_in_pixel);
GFX_EXPORT gfx::PointF ConvertPointToDIP(float scale_factor,
    const gfx::PointF& point_in_pixel);
GFX_EXPORT gfx::Size ConvertSizeToDIP(float scale_factor,
    const gfx::Size& size_in_pixel);
GFX_EXPORT gfx::Rect ConvertRectToDIP(float scale_factor,
    const gfx::Rect& rect_in_pixel);

GFX_EXPORT gfx::Point ConvertPointToPixel(float scale_factor,
    const gfx::Point& point_in_pixel);
GFX_EXPORT gfx::Size ConvertSizeToPixel(float scale_factor,
    const gfx::Size& size_in_pixel);
GFX_EXPORT gfx::Rect ConvertRectToPixel(float scale_factor,
    const gfx::Rect& rect_in_dip);
} // gfx

#endif // UI_GFX_GEOMETRY_DIP_UTIL_H_
