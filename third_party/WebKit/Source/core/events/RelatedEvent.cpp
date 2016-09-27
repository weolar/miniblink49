// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/events/RelatedEvent.h"

namespace blink {

RelatedEvent::~RelatedEvent()
{
}

PassRefPtrWillBeRawPtr<RelatedEvent> RelatedEvent::create()
{
    return adoptRefWillBeNoop(new RelatedEvent);
}

PassRefPtrWillBeRawPtr<RelatedEvent> RelatedEvent::create(const AtomicString& type, bool canBubble, bool cancelable, EventTarget* relatedTarget)
{
    return adoptRefWillBeNoop(new RelatedEvent(type, canBubble, cancelable, relatedTarget));
}

PassRefPtrWillBeRawPtr<RelatedEvent> RelatedEvent::create(const AtomicString& type, const RelatedEventInit& initializer)
{
    return adoptRefWillBeNoop(new RelatedEvent(type, initializer));
}

RelatedEvent::RelatedEvent()
{
}

RelatedEvent::RelatedEvent(const AtomicString& type, bool canBubble, bool cancelable, EventTarget* relatedTarget)
    : Event(type, canBubble, cancelable)
    , m_relatedTarget(relatedTarget)
{
}

RelatedEvent::RelatedEvent(const AtomicString& eventType, const RelatedEventInit& initializer)
    : Event(eventType, initializer)
{
    if (initializer.hasRelatedTarget())
        m_relatedTarget = initializer.relatedTarget();
}

DEFINE_TRACE(RelatedEvent)
{
    visitor->trace(m_relatedTarget);
    Event::trace(visitor);
}

} // namespace blink
