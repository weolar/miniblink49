// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EventListenerInfo_h
#define EventListenerInfo_h

#include "core/InspectorTypeBuilder.h"
#include "core/events/EventListenerMap.h"
#include "core/events/RegisteredEventListener.h"
#include "wtf/Vector.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class EventTarget;
class ExecutionContext;
class InjectedScriptManager;

struct EventListenerInfo {
    EventListenerInfo(EventTarget* eventTarget, const AtomicString& eventType, const EventListenerVector& eventListenerVector)
        : eventTarget(eventTarget)
        , eventType(eventType)
        , eventListenerVector(eventListenerVector)
    {
    }

    EventTarget* eventTarget;
    const AtomicString eventType;
    const EventListenerVector eventListenerVector;

    static void getEventListeners(EventTarget*, Vector<EventListenerInfo>& listenersArray, bool includeAncestors);
};

class RegisteredEventListenerIterator {
    WTF_MAKE_NONCOPYABLE(RegisteredEventListenerIterator);
public:
    RegisteredEventListenerIterator(Vector<EventListenerInfo>& listenersArray)
        : m_listenersArray(listenersArray)
        , m_infoIndex(0)
        , m_listenerIndex(0)
        , m_isUseCapturePass(true)
    {
    }

    const RegisteredEventListener* nextRegisteredEventListener();
    const EventListenerInfo& currentEventListenerInfo();

private:
    Vector<EventListenerInfo>& m_listenersArray;
    unsigned m_infoIndex;
    unsigned m_listenerIndex;
    bool m_isUseCapturePass;
};

PassRefPtr<TypeBuilder::Runtime::RemoteObject> eventHandlerObject(ExecutionContext*, EventListener*, InjectedScriptManager*, const String* objectGroupId);

} // namespace blink

#endif // EventListenerInfo_h
