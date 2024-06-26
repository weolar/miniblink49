// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_DEVICES_INPUT_DEVICE_H_
#define UI_EVENTS_DEVICES_INPUT_DEVICE_H_

#include <stdint.h>
#include <string>

#include "base/files/file_path.h"
#include "ui/events/devices/events_devices_export.h"

namespace ui {

enum InputDeviceType {
    INPUT_DEVICE_INTERNAL, // Internally connected input device.
    INPUT_DEVICE_EXTERNAL, // Known externally connected input device.
    INPUT_DEVICE_UNKNOWN, // Device that may or may not be an external device.
};

// Represents an input device state.
struct EVENTS_DEVICES_EXPORT InputDevice {
    static const int kInvalidId;

    // Creates an invalid input device.
    InputDevice();

    InputDevice(int id, InputDeviceType type, const std::string& name);
    InputDevice(int id,
        InputDeviceType type,
        const std::string& name,
        const base::FilePath& sys_path,
        uint16_t vendor,
        uint16_t product);
    virtual ~InputDevice();

    // ID of the device. This ID is unique between all input devices.
    int id;

    // The type of the input device.
    InputDeviceType type;

    // Name of the device.
    std::string name;

    // The path to the input device in the sysfs filesystem.
    base::FilePath sys_path;

    // USB-style device identifiers, where available, or 0 if unavailable.
    uint16_t vendor_id;
    uint16_t product_id;
};

} // namespace ui

#endif // UI_EVENTS_DEVICES_INPUT_DEVICE_H_
