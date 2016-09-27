// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRHardwareUnitCollection_h
#define VRHardwareUnitCollection_h

#include "modules/vr/VRDevice.h"
#include "modules/vr/VRHardwareUnit.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebVector.h"

namespace blink {

struct WebVRDevice;

class VRHardwareUnitCollection final : public GarbageCollected<VRHardwareUnitCollection> {
public:
    explicit VRHardwareUnitCollection(VRController*);

    VRDeviceVector updateVRHardwareUnits(const WebVector<WebVRDevice>*);
    VRHardwareUnit* getHardwareUnitForIndex(unsigned index);

    DECLARE_VIRTUAL_TRACE();

private:
    Member<VRController> m_controller;
    HeapVector<Member<VRHardwareUnit>> m_hardwareUnits;
};

} // namespace blink

#endif // VRHardwareUnitCollection_h
