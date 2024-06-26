// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/win/dpi.h"

#include "base/win/scoped_hdc.h"
#include "ui/gfx/display.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/size_conversions.h"
#include <windows.h>

namespace {

const float kDefaultDPI = 96.f;

float g_device_scale_factor = 0.f;

float GetUnforcedDeviceScaleFactor()
{
    return g_device_scale_factor ? g_device_scale_factor : static_cast<float>(gfx::GetDPI().width()) / kDefaultDPI;
}

} // namespace

namespace gfx {

void SetDefaultDeviceScaleFactor(float scale)
{
    DCHECK_NE(0.f, scale);
    g_device_scale_factor = scale;
}

Size GetDPI()
{
    static int dpi_x = 0;
    static int dpi_y = 0;
    static bool should_initialize = true;

    if (should_initialize) {
        should_initialize = false;
        base::win::ScopedGetDC screen_dc(NULL);
        // This value is safe to cache for the life time of the app since the
        // user must logout to change the DPI setting. This value also applies
        // to all screens.
        dpi_x = GetDeviceCaps(screen_dc, LOGPIXELSX);
        dpi_y = GetDeviceCaps(screen_dc, LOGPIXELSY);
    }
    return Size(dpi_x, dpi_y);
}

float GetDPIScale()
{
    if (gfx::Display::HasForceDeviceScaleFactor())
        return gfx::Display::GetForcedDeviceScaleFactor();
    float dpi_scale = GetUnforcedDeviceScaleFactor();
    return (dpi_scale <= 1.25f) ? 1.f : dpi_scale;
}

namespace win {

    Point ScreenToDIPPoint(const Point& pixel_point)
    {
        return ScaleToFlooredPoint(pixel_point, 1.0f / GetDPIScale());
    }

    Point DIPToScreenPoint(const Point& dip_point)
    {
        return ScaleToFlooredPoint(dip_point, GetDPIScale());
    }

    Rect ScreenToDIPRect(const Rect& pixel_bounds)
    {
        // It's important we scale the origin and size separately. If we instead
        // calculated the size from the floored origin and ceiled right the size could
        // vary depending upon where the two points land. That would cause problems
        // for the places this code is used (in particular mapping from native window
        // bounds to DIPs).
        return Rect(ScreenToDIPPoint(pixel_bounds.origin()),
            ScreenToDIPSize(pixel_bounds.size()));
    }

    Rect DIPToScreenRect(const Rect& dip_bounds)
    {
        // See comment in ScreenToDIPRect for why we calculate size like this.
        return Rect(DIPToScreenPoint(dip_bounds.origin()),
            DIPToScreenSize(dip_bounds.size()));
    }

    Size ScreenToDIPSize(const Size& size_in_pixels)
    {
        // Always ceil sizes. Otherwise we may be leaving off part of the bounds.
        return ScaleToCeiledSize(size_in_pixels, 1.0f / GetDPIScale());
    }

    Size DIPToScreenSize(const Size& dip_size)
    {
        // Always ceil sizes. Otherwise we may be leaving off part of the bounds.
        return ScaleToCeiledSize(dip_size, GetDPIScale());
    }

    int GetSystemMetricsInDIP(int metric)
    {
        // The system metrics always reflect the system DPI, not whatever scale we've
        // forced or decided to use.
        return static_cast<int>(
            std::round(GetSystemMetrics(metric) / GetUnforcedDeviceScaleFactor()));
    }

} // namespace win
} // namespace gfx
