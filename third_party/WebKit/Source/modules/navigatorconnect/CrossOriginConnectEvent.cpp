// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/navigatorconnect/CrossOriginConnectEvent.h"

#include "modules/navigatorconnect/AcceptConnectionObserver.h"
#include "modules/navigatorconnect/CrossOriginServiceWorkerClient.h"

namespace blink {

PassRefPtrWillBeRawPtr<CrossOriginConnectEvent> CrossOriginConnectEvent::create()
{
    return adoptRefWillBeNoop(new CrossOriginConnectEvent());
}

PassRefPtrWillBeRawPtr<CrossOriginConnectEvent> CrossOriginConnectEvent::create(AcceptConnectionObserver* observer, CrossOriginServiceWorkerClient* client)
{
    return adoptRefWillBeNoop(new CrossOriginConnectEvent(observer, client));
}

CrossOriginServiceWorkerClient* CrossOriginConnectEvent::client() const
{
    return m_client;
}

void CrossOriginConnectEvent::acceptConnection(ScriptState* scriptState, const ScriptPromise& value , ExceptionState& exceptionState)
{
    stopImmediatePropagation();
    if (m_observer)
        m_observer->acceptConnection(scriptState, value, exceptionState);
}

const AtomicString& CrossOriginConnectEvent::interfaceName() const
{
    return EventNames::CrossOriginConnectEvent;
}

CrossOriginConnectEvent::CrossOriginConnectEvent()
{
}

CrossOriginConnectEvent::CrossOriginConnectEvent(AcceptConnectionObserver* observer, CrossOriginServiceWorkerClient* client)
    : Event(EventTypeNames::crossoriginconnect, /*canBubble=*/false, /*cancelable=*/true)
    , m_observer(observer)
    , m_client(client)
{
}

DEFINE_TRACE(CrossOriginConnectEvent)
{
    visitor->trace(m_client);
    visitor->trace(m_observer);
    Event::trace(visitor);
}

} // namespace blink
