// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/vr/PositionSensorVRDevice.h"

#include "modules/vr/VRController.h"
#include "public/platform/Platform.h"

namespace blink {

PositionSensorVRDevice::PositionSensorVRDevice(VRHardwareUnit* hardwareUnit, unsigned deviceId)
    : VRDevice(hardwareUnit, deviceId)
{
}

VRPositionState* PositionSensorVRDevice::getState()
{
    // FIXME: This value should be stable for the duration of a requestAnimationFrame callback
    return hardwareUnit()->getSensorState();
}

VRPositionState* PositionSensorVRDevice::getImmediateState()
{
    return hardwareUnit()->getSensorState();
}

void PositionSensorVRDevice::resetSensor()
{
    controller()->resetSensor(index());
}

DEFINE_TRACE(PositionSensorVRDevice)
{
    VRDevice::trace(visitor);
}

} // namespace blink
