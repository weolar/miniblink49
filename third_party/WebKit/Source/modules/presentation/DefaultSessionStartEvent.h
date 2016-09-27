// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DefaultSessionStartEvent_h
#define DefaultSessionStartEvent_h

#include "modules/EventModules.h"
#include "modules/presentation/PresentationSession.h"
#include "platform/heap/Handle.h"

namespace blink {

class DefaultSessionStartEventInit;

// Presentation API event to be fired when a presentation has been triggered
// by the embedder using the default presentation URL and id.
// See https://code.google.com/p/chromium/issues/detail?id=459001 for details.
class DefaultSessionStartEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~DefaultSessionStartEvent() override;

    static PassRefPtrWillBeRawPtr<DefaultSessionStartEvent> create()
    {
        return adoptRefWillBeNoop(new DefaultSessionStartEvent);
    }
    static PassRefPtrWillBeRawPtr<DefaultSessionStartEvent> create(const AtomicString& eventType, PresentationSession* session)
    {
        return adoptRefWillBeNoop(new DefaultSessionStartEvent(eventType, session));
    }
    static PassRefPtrWillBeRawPtr<DefaultSessionStartEvent> create(const AtomicString& eventType, const DefaultSessionStartEventInit& initializer)
    {
        return adoptRefWillBeNoop(new DefaultSessionStartEvent(eventType, initializer));
    }

    PresentationSession* session() { return m_session.get(); }

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    DefaultSessionStartEvent();
    DefaultSessionStartEvent(const AtomicString& eventType, PresentationSession*);
    DefaultSessionStartEvent(const AtomicString& eventType, const DefaultSessionStartEventInit& initializer);

    PersistentWillBeMember<PresentationSession> m_session;
};

DEFINE_TYPE_CASTS(DefaultSessionStartEvent, Event, event, event->interfaceName() == EventNames::DefaultSessionStartEvent, event.interfaceName() == EventNames::DefaultSessionStartEvent);

} // namespace blink

#endif // DefaultSessionStartEvent_h
