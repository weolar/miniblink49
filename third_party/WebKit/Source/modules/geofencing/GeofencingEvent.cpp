// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/geofencing/GeofencingEvent.h"

namespace blink {

GeofencingEvent::GeofencingEvent()
{
}

GeofencingEvent::GeofencingEvent(const AtomicString& type, const String& id, GeofencingRegion* region)
    : Event(type, /*canBubble=*/false, /*cancelable=*/false)
    , m_id(id)
    , m_region(region)
{
}

GeofencingEvent::~GeofencingEvent()
{
}

DEFINE_TRACE(GeofencingEvent)
{
    visitor->trace(m_region);
    Event::trace(visitor);
}

const AtomicString& GeofencingEvent::interfaceName() const
{
    return EventNames::GeofencingEvent;
}

} // namespace blink
