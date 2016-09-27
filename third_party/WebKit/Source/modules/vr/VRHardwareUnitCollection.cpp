// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/vr/VRHardwareUnitCollection.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Navigator.h"
#include "core/page/Page.h"
#include "modules/vr/HMDVRDevice.h"
#include "modules/vr/NavigatorVRDevice.h"
#include "modules/vr/PositionSensorVRDevice.h"
#include "modules/vr/VRController.h"
#include "modules/vr/VRGetDevicesCallback.h"
#include "modules/vr/VRHardwareUnit.h"
#include "modules/vr/VRPositionState.h"

namespace blink {

VRHardwareUnitCollection::VRHardwareUnitCollection(VRController* controller)
    : m_controller(controller)
{
}

HeapVector<Member<VRDevice>> VRHardwareUnitCollection::updateVRHardwareUnits(const WebVector<WebVRDevice>* devices)
{
    VRDeviceVector vrDevices;

    if (!devices)
        return vrDevices;

    for (size_t i = 0; i < devices->size(); ++i) {
        const WebVRDevice& device = (*devices)[i];

        VRHardwareUnit* hardwareUnit = getHardwareUnitForIndex(device.index);
        if (!hardwareUnit) {
            hardwareUnit = new VRHardwareUnit(m_controller);
            m_hardwareUnits.append(hardwareUnit);
        }

        hardwareUnit->updateFromWebVRDevice(device);
        hardwareUnit->addDevicesToVector(vrDevices);
    }

    return vrDevices;
}

VRHardwareUnit* VRHardwareUnitCollection::getHardwareUnitForIndex(unsigned index)
{
    VRHardwareUnit* hardwareUnit;
    for (size_t i = 0; i < m_hardwareUnits.size(); ++i) {
        hardwareUnit = m_hardwareUnits[i];
        if (hardwareUnit->index() == index) {
            return hardwareUnit;
        }
    }

    return 0;
}

DEFINE_TRACE(VRHardwareUnitCollection)
{
    visitor->trace(m_controller);
    visitor->trace(m_hardwareUnits);
}

} // namespace blink
