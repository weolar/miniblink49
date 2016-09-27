// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RelatedEvent_h
#define RelatedEvent_h

#include "core/events/Event.h"
#include "core/events/RelatedEventInit.h"

namespace blink {

class RelatedEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<RelatedEvent> create();
    static PassRefPtrWillBeRawPtr<RelatedEvent> create(const AtomicString& type, bool canBubble, bool cancelable, EventTarget* relatedTarget);
    static PassRefPtrWillBeRawPtr<RelatedEvent> create(const AtomicString& eventType, const RelatedEventInit&);

    virtual ~RelatedEvent();

    EventTarget* relatedTarget() const { return m_relatedTarget.get(); }

    virtual const AtomicString& interfaceName() const override { return EventNames::RelatedEvent; }
    virtual bool isRelatedEvent() const override { return true; }

    DECLARE_VIRTUAL_TRACE();

private:
    RelatedEvent();
    RelatedEvent(const AtomicString& type, bool canBubble, bool cancelable, EventTarget*);
    RelatedEvent(const AtomicString& type, const RelatedEventInit&);

    RefPtrWillBeMember<EventTarget> m_relatedTarget;
};

DEFINE_EVENT_TYPE_CASTS(RelatedEvent);

} // namespace blink

#endif // RelatedEvent_h
