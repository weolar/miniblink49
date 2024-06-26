// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/win/screen_win_display.h"

#include "ui/gfx/win/display_info.h"
#include "ui/gfx/win/dpi.h"

namespace {

gfx::Display CreateDisplayFromDisplayInfo(
    const gfx::win::DisplayInfo& display_info)
{
    gfx::Display display(display_info.id());
    gfx::Rect dip_screen_bounds(
        gfx::win::ScreenToDIPRect(display_info.screen_rect()));
    display.set_bounds(dip_screen_bounds);
    display.set_work_area(
        gfx::win::ScreenToDIPRect(display_info.screen_work_rect()));
    display.SetScaleAndBounds(display_info.device_scale_factor(),
        display_info.screen_rect());
    display.set_rotation(display_info.rotation());
    return display;
}

} // namespace

namespace gfx {
namespace win {

    ScreenWinDisplay::ScreenWinDisplay() = default;

    ScreenWinDisplay::ScreenWinDisplay(const DisplayInfo& display_info)
        : display_(CreateDisplayFromDisplayInfo(display_info))
        , pixel_bounds_(display_info.screen_rect())
    {
    }

} // namespace win
} // namespace gfx
