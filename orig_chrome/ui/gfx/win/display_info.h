// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_WIN_DISPLAY_INFO_H_
#define UI_GFX_WIN_DISPLAY_INFO_H_

#include <stdint.h>
#include <windows.h>

#include "ui/gfx/display.h"
#include "ui/gfx/gfx_export.h"

namespace gfx {
namespace win {

    // Gathers the parameters necessary to create a gfx::win::ScreenWinDisplay.
    class GFX_EXPORT DisplayInfo final {
    public:
        DisplayInfo(const MONITORINFOEX& monitor_info, float device_scale_factor);
        DisplayInfo(const MONITORINFOEX& monitor_info,
            float device_scale_factor,
            gfx::Display::Rotation rotation);

        static int64_t DeviceIdFromDeviceName(const wchar_t* device_name);

        int64_t id() const { return id_; }
        gfx::Display::Rotation rotation() const { return rotation_; }
        const gfx::Rect& screen_rect() const { return screen_rect_; }
        const gfx::Rect& screen_work_rect() const { return screen_work_rect_; }
        float device_scale_factor() const { return device_scale_factor_; }

    private:
        int64_t id_;
        gfx::Display::Rotation rotation_;
        gfx::Rect screen_rect_;
        gfx::Rect screen_work_rect_;
        float device_scale_factor_;
    };

} // namespace win
} // namespace gfx

#endif // UI_GFX_WIN_DISPLAY_INFO_H_
