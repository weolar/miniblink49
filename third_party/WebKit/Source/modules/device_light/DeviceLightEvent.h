// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DeviceLightEvent_h
#define DeviceLightEvent_h

#include "modules/EventModules.h"
#include "modules/device_light/DeviceLightEventInit.h"
#include "platform/heap/Handle.h"

namespace blink {

class DeviceLightEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~DeviceLightEvent() override;

    static PassRefPtrWillBeRawPtr<DeviceLightEvent> create()
    {
        return adoptRefWillBeNoop(new DeviceLightEvent);
    }
    static PassRefPtrWillBeRawPtr<DeviceLightEvent> create(const AtomicString& eventType, double value)
    {
        return adoptRefWillBeNoop(new DeviceLightEvent(eventType, value));
    }
    static PassRefPtrWillBeRawPtr<DeviceLightEvent> create(const AtomicString& eventType, const DeviceLightEventInit& initializer)
    {
        return adoptRefWillBeNoop(new DeviceLightEvent(eventType, initializer));
    }

    double value() const { return m_value; }

    const AtomicString& interfaceName() const override;

private:
    DeviceLightEvent();
    DeviceLightEvent(const AtomicString& eventType, double value);
    DeviceLightEvent(const AtomicString& eventType, const DeviceLightEventInit& initializer);

    double m_value;
};

DEFINE_TYPE_CASTS(DeviceLightEvent, Event, event, event->interfaceName() == EventNames::DeviceLightEvent, event.interfaceName() == EventNames::DeviceLightEvent);

} // namespace blink

#endif // DeviceLightEvent_h
