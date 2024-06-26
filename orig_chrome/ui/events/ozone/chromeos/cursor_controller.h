// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_CHROMEOS_CURSOR_CONTROLLER_H_
#define UI_EVENTS_OZONE_CHROMEOS_CURSOR_CONTROLLER_H_

#include <map>

#include "base/event_types.h"
#include "base/macros.h"
#include "base/memory/singleton.h"
#include "base/synchronization/lock.h"
#include "ui/events/ozone/events_ozone_export.h"
#include "ui/gfx/display.h"
#include "ui/gfx/geometry/vector2d_f.h"
#include "ui/gfx/native_widget_types.h"

namespace ui {

// Manager for per-window cursor settings.
//
// This is used to apply a rotation & acceleration to each vector added to the
// cursor position on ChromeOS.
//
// This has 3 uses:
//
//  (1) Fixing cursor movement direction for rotated displays.
//  (2) Fixing cursor movement speed based on scale factor.
//  (3) Tweaking cursor movement speed on external displays.
//
// This HACK is necessary because ash handles rotation and handles scaling but
// does NOT handle the cursor movement (except that it sends a message to x11 or
// ozone to activate this hack).
//
// TODO(spang): Don't worry, we have a plan to remove this.
class EVENTS_OZONE_EXPORT CursorController {
public:
    static CursorController* GetInstance();

    // Changes the rotation & scale applied for a window.
    void SetCursorConfigForWindow(gfx::AcceleratedWidget widget,
        gfx::Display::Rotation rotation,
        float scale);

    // Cleans up all state associated with a window.
    void ClearCursorConfigForWindow(gfx::AcceleratedWidget widget);

    // Applies the current settings for a window to a cursor movement vector.
    //
    // The rotation applies counter-clockwise (to negate clockwise display
    // rotation) and the result is multiplied by scale.
    //
    // e.g. if (dx, dy) = (2, 3) and (scale, rotation) = (2.f, 90deg)
    //      then we set (dx, dy) = (-6, 4)
    //
    // Since scale generally includes DSF, you can think of the input
    // vector unit as DIP and the output vector unit as pixels.
    void ApplyCursorConfigForWindow(gfx::AcceleratedWidget widget,
        gfx::Vector2dF* delta) const;

private:
    CursorController();
    ~CursorController();
    friend struct base::DefaultSingletonTraits<CursorController>;

    struct PerWindowCursorConfiguration {
        gfx::Display::Rotation rotation;
        float scale;
    };

    typedef std::map<gfx::AcceleratedWidget, PerWindowCursorConfiguration>
        WindowToCursorConfigurationMap;

    WindowToCursorConfigurationMap window_to_cursor_configuration_map_;
    mutable base::Lock window_to_cursor_configuration_map_lock_;

    DISALLOW_COPY_AND_ASSIGN(CursorController);
};

} // namespace ui

#endif // UI_EVENTS_OZONE_CHROMEOS_CURSOR_CONTROLLER_H_
