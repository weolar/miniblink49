// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRDevice_h
#define VRDevice_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/vr/VRHardwareUnit.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/text/WTFString.h"

namespace blink {

class VRController;

class VRDevice : public GarbageCollectedFinalized<VRDevice>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    unsigned index() const { return m_hardwareUnit->index(); }
    const String& hardwareUnitId() const { return m_hardwareUnit->hardwareUnitId(); }
    const String& deviceId() const { return m_deviceId; }
    const String& deviceName() const { return m_deviceName; }

    VRHardwareUnit* hardwareUnit() { return m_hardwareUnit; }
    const VRHardwareUnit* hardwareUnit() const { return m_hardwareUnit; }

    VRController* controller() { return m_hardwareUnit->controller(); }

    virtual void updateFromWebVRDevice(const WebVRDevice&);

    DECLARE_VIRTUAL_TRACE();

protected:
    VRDevice(VRHardwareUnit*, unsigned);

private:
    Member<VRHardwareUnit> m_hardwareUnit;
    String m_deviceId;
    String m_deviceName;
};

using VRDeviceVector = HeapVector<Member<VRDevice>>;

} // namespace blink

#endif // VRDevice_h
