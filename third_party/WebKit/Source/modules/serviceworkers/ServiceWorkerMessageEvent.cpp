// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/ServiceWorkerMessageEvent.h"

namespace blink {

ServiceWorkerMessageEvent::ServiceWorkerMessageEvent()
{
}

ServiceWorkerMessageEvent::ServiceWorkerMessageEvent(const AtomicString& type, const ServiceWorkerMessageEventInit& initializer)
    : Event(type, initializer)
{
    if (initializer.hasOrigin())
        m_origin = initializer.origin();
    if (initializer.hasLastEventId())
        m_lastEventId = initializer.lastEventId();
    if (initializer.hasSource()) {
        if (initializer.source().isServiceWorker())
            m_sourceAsServiceWorker = initializer.source().getAsServiceWorker();
        else if (initializer.source().isMessagePort())
            m_sourceAsMessagePort = initializer.source().getAsMessagePort();
    }
    if (initializer.hasPorts())
        m_ports = new MessagePortArray(initializer.ports());
    if (initializer.hasData())
        m_data = initializer.data();
}

ServiceWorkerMessageEvent::ServiceWorkerMessageEvent(PassRefPtr<SerializedScriptValue> data, const String& origin, const String& lastEventId, PassRefPtrWillBeRawPtr<ServiceWorker> source, MessagePortArray* ports)
    : Event(EventTypeNames::message, false, false)
    , m_serializedData(data)
    , m_origin(origin)
    , m_lastEventId(lastEventId)
    , m_sourceAsServiceWorker(source)
    , m_ports(ports)
{
    if (m_serializedData)
        m_serializedData->registerMemoryAllocatedWithCurrentScriptContext();
}

ServiceWorkerMessageEvent::~ServiceWorkerMessageEvent()
{
}

MessagePortArray ServiceWorkerMessageEvent::ports(bool& isNull) const
{
    if (m_ports) {
        isNull = false;
        return *m_ports;
    }
    isNull = true;
    return MessagePortArray();
}

MessagePortArray ServiceWorkerMessageEvent::ports() const
{
    bool unused;
    return ports(unused);
}

void ServiceWorkerMessageEvent::source(ServiceWorkerOrMessagePort& result) const
{
    if (m_sourceAsServiceWorker)
        result = ServiceWorkerOrMessagePort::fromServiceWorker(m_sourceAsServiceWorker);
    else if (m_sourceAsMessagePort)
        result = ServiceWorkerOrMessagePort::fromMessagePort(m_sourceAsMessagePort);
}

const AtomicString& ServiceWorkerMessageEvent::interfaceName() const
{
    return EventNames::ServiceWorkerMessageEvent;
}

DEFINE_TRACE(ServiceWorkerMessageEvent)
{
    visitor->trace(m_sourceAsServiceWorker);
    visitor->trace(m_sourceAsMessagePort);
#if ENABLE(OILPAN)
    visitor->trace(m_ports);
#endif
    Event::trace(visitor);
}

} // namespace blink
