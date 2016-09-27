/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DeviceMotionEvent_h
#define DeviceMotionEvent_h

#include "modules/EventModules.h"
#include "platform/heap/Handle.h"

namespace blink {

class DeviceAcceleration;
class DeviceMotionData;
class DeviceRotationRate;

class DeviceMotionEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~DeviceMotionEvent() override;
    static PassRefPtrWillBeRawPtr<DeviceMotionEvent> create()
    {
        return adoptRefWillBeNoop(new DeviceMotionEvent);
    }
    static PassRefPtrWillBeRawPtr<DeviceMotionEvent> create(const AtomicString& eventType, DeviceMotionData* deviceMotionData)
    {
        return adoptRefWillBeNoop(new DeviceMotionEvent(eventType, deviceMotionData));
    }

    void initDeviceMotionEvent(const AtomicString& type, bool bubbles, bool cancelable, DeviceMotionData*);

    DeviceMotionData* deviceMotionData() const { return m_deviceMotionData.get(); }

    DeviceAcceleration* acceleration();
    DeviceAcceleration* accelerationIncludingGravity();
    DeviceRotationRate* rotationRate();
    double interval(bool& isNull) const;

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    DeviceMotionEvent();
    DeviceMotionEvent(const AtomicString& eventType, DeviceMotionData*);

    PersistentWillBeMember<DeviceMotionData> m_deviceMotionData;
    PersistentWillBeMember<DeviceAcceleration> m_acceleration;
    PersistentWillBeMember<DeviceAcceleration> m_accelerationIncludingGravity;
    PersistentWillBeMember<DeviceRotationRate> m_rotationRate;
};

DEFINE_TYPE_CASTS(DeviceMotionEvent, Event, event, event->interfaceName() == EventNames::DeviceMotionEvent, event.interfaceName() == EventNames::DeviceMotionEvent);

} // namespace blink

#endif // DeviceMotionEvent_h
