// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/vr/VRHardwareUnit.h"

#include "modules/vr/HMDVRDevice.h"
#include "modules/vr/NavigatorVRDevice.h"
#include "modules/vr/PositionSensorVRDevice.h"
#include "modules/vr/VRController.h"
#include "modules/vr/VRDevice.h"
#include "public/platform/Platform.h"

namespace blink {

VRHardwareUnit::VRHardwareUnit(VRController* controller)
    : m_nextDeviceId(1)
    , m_frameIndex(0)
    , m_controller(controller)
{
    m_positionState = VRPositionState::create();
}

VRHardwareUnit::~VRHardwareUnit()
{
}

void VRHardwareUnit::updateFromWebVRDevice(const WebVRDevice& device)
{
    m_index = device.index;
    m_hardwareUnitId = String::number(device.index);

    if (device.flags & WebVRDeviceTypeHMD) {
        if (!m_hmd)
            m_hmd = new HMDVRDevice(this, m_nextDeviceId++);
        m_hmd->updateFromWebVRDevice(device);
    } else if (m_hmd) {
        m_hmd.clear();
    }

    if (device.flags & WebVRDeviceTypePosition) {
        if (!m_positionSensor)
            m_positionSensor = new PositionSensorVRDevice(this, m_nextDeviceId++);
        m_positionSensor->updateFromWebVRDevice(device);
    } else if (m_positionSensor) {
        m_positionSensor.clear();
    }
}

void VRHardwareUnit::addDevicesToVector(HeapVector<Member<VRDevice>>& vrDevices)
{
    if (m_hmd)
        vrDevices.append(m_hmd);

    if (m_positionSensor)
        vrDevices.append(m_positionSensor);
}

VRController* VRHardwareUnit::controller()
{
    return m_controller;
}

VRPositionState* VRHardwareUnit::getSensorState()
{
    WebHMDSensorState state;
    m_controller->getSensorState(m_index, state);
    m_positionState->setState(state);
    m_frameIndex = state.frameIndex;
    return m_positionState;
}

DEFINE_TRACE(VRHardwareUnit)
{
    visitor->trace(m_controller);
    visitor->trace(m_positionState);
    visitor->trace(m_hmd);
    visitor->trace(m_positionSensor);
}

} // namespace blink
