// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/vr/VRDevice.h"

namespace blink {

VRDevice::VRDevice(VRHardwareUnit* hardwareUnit, unsigned deviceId)
    : m_hardwareUnit(hardwareUnit)
    , m_deviceId(String::number(deviceId))
{
}

void VRDevice::updateFromWebVRDevice(const WebVRDevice& device)
{
    m_deviceName = device.deviceName;
}

DEFINE_TRACE(VRDevice)
{
    visitor->trace(m_hardwareUnit);
}

} // namespace blink
