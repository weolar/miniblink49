// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/device_light/DeviceLightEvent.h"

namespace blink {

DeviceLightEvent::~DeviceLightEvent()
{
}

DeviceLightEvent::DeviceLightEvent()
    : m_value(std::numeric_limits<double>::infinity())
{
}

DeviceLightEvent::DeviceLightEvent(const AtomicString& eventType, double value)
    : Event(eventType, true, false) // The DeviceLightEvent bubbles but is not cancelable.
    , m_value(value)
{
}

DeviceLightEvent::DeviceLightEvent(const AtomicString& eventType, const DeviceLightEventInit& initializer)
    : Event(eventType, initializer)
    , m_value(initializer.hasValue() ? initializer.value() : std::numeric_limits<double>::infinity())
{
    setCanBubble(true);
}

const AtomicString& DeviceLightEvent::interfaceName() const
{
    return EventNames::DeviceLightEvent;
}

} // namespace blink




