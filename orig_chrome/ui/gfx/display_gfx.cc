// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/display.h"

#include <algorithm>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "build/build_config.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/geometry/point_conversions.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/switches.h"

namespace gfx {
namespace {

    // This variable tracks whether the forced device scale factor switch needs to
    // be read from the command line, i.e. if it is set to -1 then the command line
    // is checked.
    int g_has_forced_device_scale_factor = -1;

    // This variable caches the forced device scale factor value which is read off
    // the command line. If the cache is invalidated by setting this variable to
    // -1.0, we read the forced device scale factor again.
    float g_forced_device_scale_factor = -1.0;

    bool HasForceDeviceScaleFactorImpl()
    {
        return base::CommandLine::ForCurrentProcess()->HasSwitch(
            switches::kForceDeviceScaleFactor);
    }

    float GetForcedDeviceScaleFactorImpl()
    {
        double scale_in_double = 1.0;
        if (HasForceDeviceScaleFactorImpl()) {
            std::string value = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
                switches::kForceDeviceScaleFactor);
            if (!base::StringToDouble(value, &scale_in_double)) {
                LOG(ERROR) << "Failed to parse the default device scale factor:" << value;
                scale_in_double = 1.0;
            }
        }
        return static_cast<float>(scale_in_double);
    }

    int64_t internal_display_id_ = -1;

} // namespace

// static
float Display::GetForcedDeviceScaleFactor()
{
    if (g_forced_device_scale_factor < 0)
        g_forced_device_scale_factor = GetForcedDeviceScaleFactorImpl();
    return g_forced_device_scale_factor;
}

// static
bool Display::HasForceDeviceScaleFactor()
{
    if (g_has_forced_device_scale_factor == -1)
        g_has_forced_device_scale_factor = HasForceDeviceScaleFactorImpl();
    return !!g_has_forced_device_scale_factor;
}

// static
void Display::ResetForceDeviceScaleFactorForTesting()
{
    g_has_forced_device_scale_factor = -1;
    g_forced_device_scale_factor = -1.0;
}

Display::Display()
    : id_(kInvalidDisplayID)
    , device_scale_factor_(GetForcedDeviceScaleFactor())
    , rotation_(ROTATE_0)
    , touch_support_(TOUCH_SUPPORT_UNKNOWN)
{
}

Display::Display(int64_t id)
    : id_(id)
    , device_scale_factor_(GetForcedDeviceScaleFactor())
    , rotation_(ROTATE_0)
    , touch_support_(TOUCH_SUPPORT_UNKNOWN)
{
}

Display::Display(int64_t id, const gfx::Rect& bounds)
    : id_(id)
    , bounds_(bounds)
    , work_area_(bounds)
    , device_scale_factor_(GetForcedDeviceScaleFactor())
    , rotation_(ROTATE_0)
    , touch_support_(TOUCH_SUPPORT_UNKNOWN)
{
#if defined(USE_AURA)
    SetScaleAndBounds(device_scale_factor_, bounds);
#endif
}

Display::~Display()
{
}

int Display::RotationAsDegree() const
{
    switch (rotation_) {
    case ROTATE_0:
        return 0;
    case ROTATE_90:
        return 90;
    case ROTATE_180:
        return 180;
    case ROTATE_270:
        return 270;
    }

    NOTREACHED();
    return 0;
}

void Display::SetRotationAsDegree(int rotation)
{
    switch (rotation) {
    case 0:
        rotation_ = ROTATE_0;
        break;
    case 90:
        rotation_ = ROTATE_90;
        break;
    case 180:
        rotation_ = ROTATE_180;
        break;
    case 270:
        rotation_ = ROTATE_270;
        break;
    default:
        // We should not reach that but we will just ignore the call if we do.
        NOTREACHED();
    }
}

Insets Display::GetWorkAreaInsets() const
{
    return gfx::Insets(work_area_.y() - bounds_.y(),
        work_area_.x() - bounds_.x(),
        bounds_.bottom() - work_area_.bottom(),
        bounds_.right() - work_area_.right());
}

void Display::SetScaleAndBounds(
    float device_scale_factor,
    const gfx::Rect& bounds_in_pixel)
{
    Insets insets = bounds_.InsetsFrom(work_area_);
    if (!HasForceDeviceScaleFactor()) {
#if defined(OS_MACOSX)
        // Unless an explicit scale factor was provided for testing, ensure the
        // scale is integral.
        device_scale_factor = static_cast<int>(device_scale_factor);
#endif
        device_scale_factor_ = device_scale_factor;
    }
    device_scale_factor_ = std::max(1.0f, device_scale_factor_);
    bounds_ = gfx::Rect(gfx::ScaleToFlooredPoint(bounds_in_pixel.origin(),
                            1.0f / device_scale_factor_),
        gfx::ScaleToFlooredSize(bounds_in_pixel.size(),
            1.0f / device_scale_factor_));
    UpdateWorkAreaFromInsets(insets);
}

void Display::SetSize(const gfx::Size& size_in_pixel)
{
    gfx::Point origin = bounds_.origin();
#if defined(USE_AURA)
    origin = gfx::ScaleToFlooredPoint(origin, device_scale_factor_);
#endif
    SetScaleAndBounds(device_scale_factor_, gfx::Rect(origin, size_in_pixel));
}

void Display::UpdateWorkAreaFromInsets(const gfx::Insets& insets)
{
    work_area_ = bounds_;
    work_area_.Inset(insets);
}

gfx::Size Display::GetSizeInPixel() const
{
    return gfx::ScaleToFlooredSize(size(), device_scale_factor_);
}

std::string Display::ToString() const
{
    return base::StringPrintf(
        "Display[%lld] bounds=%s, workarea=%s, scale=%f, %s",
        static_cast<long long int>(id_),
        bounds_.ToString().c_str(),
        work_area_.ToString().c_str(),
        device_scale_factor_,
        IsInternal() ? "internal" : "external");
}

bool Display::IsInternal() const
{
    return is_valid() && (id_ == internal_display_id_);
}

// static
int64_t Display::InternalDisplayId()
{
    DCHECK_NE(kInvalidDisplayID, internal_display_id_);
    return internal_display_id_;
}

// static
void Display::SetInternalDisplayId(int64_t internal_display_id)
{
    internal_display_id_ = internal_display_id;
}

// static
bool Display::IsInternalDisplayId(int64_t display_id)
{
    DCHECK_NE(kInvalidDisplayID, display_id);
    return HasInternalDisplay() && internal_display_id_ == display_id;
}

// static
bool Display::HasInternalDisplay()
{
    return internal_display_id_ != kInvalidDisplayID;
}

} // namespace gfx
