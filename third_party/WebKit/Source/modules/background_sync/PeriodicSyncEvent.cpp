// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/background_sync/PeriodicSyncEvent.h"

#include "modules/background_sync/PeriodicSyncEventInit.h"
#include "modules/background_sync/PeriodicSyncRegistration.h"

namespace blink {

PeriodicSyncEvent::PeriodicSyncEvent()
{
}

PeriodicSyncEvent::PeriodicSyncEvent(const AtomicString& type, PeriodicSyncRegistration* periodicRegistration, WaitUntilObserver* observer)
    : ExtendableEvent(type, ExtendableEventInit(), observer)
    , m_periodicRegistration(periodicRegistration)
{
}

PeriodicSyncEvent::PeriodicSyncEvent(const AtomicString& type, const PeriodicSyncEventInit& init)
    : ExtendableEvent(type, init)
{
    m_periodicRegistration = init.registration();
}

PeriodicSyncEvent::~PeriodicSyncEvent()
{
}

const AtomicString& PeriodicSyncEvent::interfaceName() const
{
    return EventNames::PeriodicSyncEvent;
}

PeriodicSyncRegistration* PeriodicSyncEvent::registration()
{
    return m_periodicRegistration.get();
}

DEFINE_TRACE(PeriodicSyncEvent)
{
    visitor->trace(m_periodicRegistration);
    ExtendableEvent::trace(visitor);
}

} // namespace blink
