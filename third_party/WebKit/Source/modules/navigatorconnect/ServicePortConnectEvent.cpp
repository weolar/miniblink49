// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/navigatorconnect/ServicePortConnectEvent.h"

#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "modules/navigatorconnect/AcceptConnectionObserver.h"
#include "modules/navigatorconnect/ServicePortConnectEventInit.h"

namespace blink {

PassRefPtrWillBeRawPtr<ServicePortConnectEvent> ServicePortConnectEvent::create()
{
    return adoptRefWillBeNoop(new ServicePortConnectEvent());
}

PassRefPtrWillBeRawPtr<ServicePortConnectEvent> ServicePortConnectEvent::create(const AtomicString& type, const ServicePortConnectEventInit& initializer)
{
    return adoptRefWillBeNoop(new ServicePortConnectEvent(type, initializer, nullptr));
}

PassRefPtrWillBeRawPtr<ServicePortConnectEvent> ServicePortConnectEvent::create(const AtomicString& type, const ServicePortConnectEventInit& initializer, AcceptConnectionObserver* observer)
{
    return adoptRefWillBeNoop(new ServicePortConnectEvent(type, initializer, observer));
}

ScriptPromise ServicePortConnectEvent::respondWith(ScriptState* scriptState, const ScriptPromise& response, ExceptionState& exceptionState)
{
    stopImmediatePropagation();
    if (m_observer)
        return m_observer->respondWith(scriptState, response, exceptionState);
    return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(NotSupportedError));
}

const AtomicString& ServicePortConnectEvent::interfaceName() const
{
    return EventNames::ServicePortConnectEvent;
}

DEFINE_TRACE(ServicePortConnectEvent)
{
    visitor->trace(m_observer);
    ExtendableEvent::trace(visitor);
}

ServicePortConnectEvent::ServicePortConnectEvent()
{
}

ServicePortConnectEvent::ServicePortConnectEvent(const AtomicString& type, const ServicePortConnectEventInit& initializer, AcceptConnectionObserver* observer)
    : ExtendableEvent(type, initializer)
    , m_observer(observer)
    , m_targetURL(initializer.targetURL())
    , m_origin(initializer.origin())
{
}

} // namespace blink

