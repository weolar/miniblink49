/*
 * Copyright 2010, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DeviceOrientationEvent_h
#define DeviceOrientationEvent_h

#include "bindings/core/v8/Nullable.h"
#include "modules/EventModules.h"
#include "platform/heap/Handle.h"

namespace blink {

class DeviceOrientationData;

class DeviceOrientationEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~DeviceOrientationEvent() override;
    static PassRefPtrWillBeRawPtr<DeviceOrientationEvent> create()
    {
        return adoptRefWillBeNoop(new DeviceOrientationEvent);
    }
    static PassRefPtrWillBeRawPtr<DeviceOrientationEvent> create(const AtomicString& eventType, DeviceOrientationData* orientation)
    {
        return adoptRefWillBeNoop(new DeviceOrientationEvent(eventType, orientation));
    }

    void initDeviceOrientationEvent(const AtomicString& type, bool bubbles, bool cancelable, const Nullable<double>& alpha, const Nullable<double>& beta, const Nullable<double>& gamma, const Nullable<bool>& absolute);

    DeviceOrientationData* orientation() const { return m_orientation.get(); }

    double alpha(bool& isNull) const;
    double beta(bool& isNull) const;
    double gamma(bool& isNull) const;
    bool absolute(bool& isNull) const;

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    DeviceOrientationEvent();
    DeviceOrientationEvent(const AtomicString& eventType, DeviceOrientationData*);

    PersistentWillBeMember<DeviceOrientationData> m_orientation;
};

DEFINE_TYPE_CASTS(DeviceOrientationEvent, Event, event, event->interfaceName() == EventNames::DeviceOrientationEvent, event.interfaceName() == EventNames::DeviceOrientationEvent);

} // namespace blink

#endif // DeviceOrientationEvent_h
