// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/presentation/DefaultSessionStartEvent.h"

#include "modules/presentation/DefaultSessionStartEventInit.h"

namespace blink {

DefaultSessionStartEvent::~DefaultSessionStartEvent()
{
}

DefaultSessionStartEvent::DefaultSessionStartEvent()
{
}

DefaultSessionStartEvent::DefaultSessionStartEvent(const AtomicString& eventType, PresentationSession* session)
    : Event(eventType, false /* canBubble */, false /* cancelable */)
    , m_session(session)
{
}

DefaultSessionStartEvent::DefaultSessionStartEvent(const AtomicString& eventType, const DefaultSessionStartEventInit& initializer)
    : Event(eventType, initializer)
    , m_session(initializer.session())
{
}

const AtomicString& DefaultSessionStartEvent::interfaceName() const
{
    return EventNames::DefaultSessionStartEvent;
}

DEFINE_TRACE(DefaultSessionStartEvent)
{
    visitor->trace(m_session);
    Event::trace(visitor);
}

} // namespace blink
