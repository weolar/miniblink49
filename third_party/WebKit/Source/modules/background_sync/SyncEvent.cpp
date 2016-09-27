// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/background_sync/SyncEvent.h"

namespace blink {

SyncEvent::SyncEvent()
{
}

SyncEvent::SyncEvent(const AtomicString& type, SyncRegistration* syncRegistration, WaitUntilObserver* observer)
    : ExtendableEvent(type, ExtendableEventInit(), observer)
    , m_syncRegistration(syncRegistration)
{
}

SyncEvent::SyncEvent(const AtomicString& type, const SyncEventInit& init)
    : ExtendableEvent(type, init)
{
    m_syncRegistration = init.registration();
}

SyncEvent::~SyncEvent()
{
}

const AtomicString& SyncEvent::interfaceName() const
{
    return EventNames::SyncEvent;
}

SyncRegistration* SyncEvent::registration()
{
    return m_syncRegistration.get();
}

DEFINE_TRACE(SyncEvent)
{
    visitor->trace(m_syncRegistration);
    ExtendableEvent::trace(visitor);
}

} // namespace blink
