// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_DEVICES_TOUCHSCREEN_DEVICE_H_
#define UI_EVENTS_DEVICES_TOUCHSCREEN_DEVICE_H_

#include <string>

#include "ui/events/devices/events_devices_export.h"
#include "ui/events/devices/input_device.h"
#include "ui/gfx/geometry/size.h"

namespace ui {

// Represents a Touchscreen device state.
struct EVENTS_DEVICES_EXPORT TouchscreenDevice : public InputDevice {
    // Creates an invalid touchscreen device.
    TouchscreenDevice();

    TouchscreenDevice(int id,
        InputDeviceType type,
        const std::string& name,
        const gfx::Size& size,
        int touch_points);

    TouchscreenDevice(const InputDevice& input_device,
        const gfx::Size& size,
        int touch_points);

    gfx::Size size; // Size of the touch screen area.
    int touch_points; // The number of touch points this device supports (0 if
        // unknown).
};

} // namespace ui

#endif // UI_EVENTS_DEVICES_TOUCHSCREEN_DEVICE_H_
