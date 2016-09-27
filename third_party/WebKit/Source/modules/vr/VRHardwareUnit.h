// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRHardwareUnit_h
#define VRHardwareUnit_h

#include "modules/vr/VRFieldOfView.h"
#include "modules/vr/VRPositionState.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class VRController;
class VRDevice;
class HMDVRDevice;
class NavigatorVRDevice;
class PositionSensorVRDevice;

enum VREye {
    VREyeLeft,
    VREyeRight,
    VREyeNone,
};

class VRHardwareUnit : public GarbageCollectedFinalized<VRHardwareUnit> {
public:
    explicit VRHardwareUnit(VRController*);
    virtual ~VRHardwareUnit();

    void updateFromWebVRDevice(const WebVRDevice&);

    void addDevicesToVector(HeapVector<Member<VRDevice>>&);

    virtual unsigned index() const { return m_index; }
    const String& hardwareUnitId() const { return m_hardwareUnitId; }

    unsigned frameIndex() const { return m_frameIndex; }

    VRController* controller();

    // VRController queries
    VRPositionState* getSensorState();

    HMDVRDevice* hmd() const { return m_hmd; }
    PositionSensorVRDevice* positionSensor() const { return m_positionSensor; }

    DECLARE_VIRTUAL_TRACE();

private:
    unsigned m_index;
    String m_hardwareUnitId;
    unsigned m_nextDeviceId;

    unsigned m_frameIndex;

    Member<VRController> m_controller;
    Member<VRPositionState> m_positionState;

    // Device types
    Member<HMDVRDevice> m_hmd;
    Member<PositionSensorVRDevice> m_positionSensor;
};

} // namespace blink

#endif // VRHardwareUnit_h
