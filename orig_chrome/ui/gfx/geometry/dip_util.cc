// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/geometry/dip_util.h"

#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace gfx {

Point ConvertPointToDIP(float scale_factor, const Point& point_in_pixel)
{
    return ScaleToFlooredPoint(point_in_pixel, 1.0f / scale_factor);
}

PointF ConvertPointToDIP(float scale_factor, const PointF& point_in_pixel)
{
    return ScalePoint(point_in_pixel, 1.0f / scale_factor);
}

Size ConvertSizeToDIP(float scale_factor, const Size& size_in_pixel)
{
    return ScaleToFlooredSize(size_in_pixel, 1.0f / scale_factor);
}

Rect ConvertRectToDIP(float scale_factor, const Rect& rect_in_pixel)
{
    return ToFlooredRectDeprecated(
        ScaleRect(RectF(rect_in_pixel), 1.0f / scale_factor));
}

Point ConvertPointToPixel(float scale_factor, const Point& point_in_dip)
{
    return ScaleToFlooredPoint(point_in_dip, scale_factor);
}

Size ConvertSizeToPixel(float scale_factor, const Size& size_in_dip)
{
    return ScaleToFlooredSize(size_in_dip, scale_factor);
}

Rect ConvertRectToPixel(float scale_factor, const Rect& rect_in_dip)
{
    // Use ToEnclosingRect() to ensure we paint all the possible pixels
    // touched. ToEnclosingRect() floors the origin, and ceils the max
    // coordinate. To do otherwise (such as flooring the size) potentially
    // results in rounding down and not drawing all the pixels that are
    // touched.
    return ToEnclosingRect(
        RectF(ScalePoint(gfx::PointF(rect_in_dip.origin()), scale_factor),
            ScaleSize(gfx::SizeF(rect_in_dip.size()), scale_factor)));
}

} // namespace gfx
