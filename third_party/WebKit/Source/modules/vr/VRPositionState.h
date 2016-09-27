// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRPositionState_h
#define VRPositionState_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/DOMPoint.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/vr/WebVR.h"
#include "wtf/Forward.h"
#include "wtf/text/WTFString.h"

namespace blink {

class VRPositionState final : public GarbageCollected<VRPositionState>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static VRPositionState* create()
    {
        return new VRPositionState();
    }

    double timeStamp() const { return m_timeStamp; }
    DOMPoint* orientation() const { return m_orientation; }
    DOMPoint* position() const { return m_position; }
    DOMPoint* angularVelocity() const { return m_angularVelocity; }
    DOMPoint* linearVelocity() const { return m_linearVelocity; }
    DOMPoint* angularAcceleration() const { return m_angularAcceleration; }
    DOMPoint* linearAcceleration() const { return m_linearAcceleration; }

    void setState(const WebHMDSensorState&);

    DECLARE_VIRTUAL_TRACE();

private:
    VRPositionState();

    double m_timeStamp;
    Member<DOMPoint> m_orientation;
    Member<DOMPoint> m_position;
    Member<DOMPoint> m_angularVelocity;
    Member<DOMPoint> m_linearVelocity;
    Member<DOMPoint> m_angularAcceleration;
    Member<DOMPoint> m_linearAcceleration;
};

} // namespace blink

#endif // VRPositionState_h
