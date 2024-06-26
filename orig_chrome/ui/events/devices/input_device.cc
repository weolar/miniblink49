// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/devices/input_device.h"

#include <string>

namespace ui {

// static
const int InputDevice::kInvalidId = 0;

InputDevice::InputDevice()
    : id(kInvalidId)
    , type(InputDeviceType::INPUT_DEVICE_UNKNOWN)
{
}

InputDevice::InputDevice(int id, InputDeviceType type, const std::string& name)
    : id(id)
    , type(type)
    , name(name)
    , vendor_id(0)
    , product_id(0)
{
}

InputDevice::InputDevice(int id,
    InputDeviceType type,
    const std::string& name,
    const base::FilePath& sys_path,
    uint16_t vendor,
    uint16_t product)
    : id(id)
    , type(type)
    , name(name)
    , sys_path(sys_path)
    , vendor_id(vendor)
    , product_id(product)
{
}

InputDevice::~InputDevice()
{
}

} // namespace ui
