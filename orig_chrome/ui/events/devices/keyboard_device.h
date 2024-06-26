// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_DEVICES_KEYBOARD_DEVICE_H_
#define UI_EVENTS_DEVICES_KEYBOARD_DEVICE_H_

#include <string>

#include "ui/events/devices/events_devices_export.h"
#include "ui/events/devices/input_device.h"

namespace ui {

// Represents a Keyboard device state.
struct EVENTS_DEVICES_EXPORT KeyboardDevice : public InputDevice {
    KeyboardDevice(int id, InputDeviceType type, const std::string& name);
    explicit KeyboardDevice(const InputDevice& input_device);
};

} // namespace ui

#endif // UI_EVENTS_DEVICES_KEYBOARD_DEVICE_H_
