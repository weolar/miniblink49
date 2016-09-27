// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/navigatorconnect/ServicePort.h"

#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "core/dom/MessagePort.h"
#include "modules/navigatorconnect/ServicePortCollection.h"
#include "public/platform/Platform.h"
#include "public/platform/modules/navigator_services/WebServicePortProvider.h"

namespace blink {

ServicePort* ServicePort::create(ServicePortCollection* collection, const WebServicePort& port)
{
    return new ServicePort(collection, port);
}

ServicePort::~ServicePort()
{
}

String ServicePort::targetURL() const
{
    return m_port.targetUrl.string();
}

String ServicePort::name() const
{
    return m_port.name;
}

ScriptValue ServicePort::data(ScriptState* scriptState) const
{
    if (!m_serializedData)
        return ScriptValue::createNull(scriptState);

    return ScriptValue(scriptState, m_serializedData->deserialize(scriptState->isolate()));
}

void ServicePort::postMessage(ExecutionContext* executionContext, PassRefPtr<SerializedScriptValue> message, const MessagePortArray* ports, ExceptionState& exceptionState)
{
    OwnPtr<MessagePortChannelArray> channels;
    if (ports) {
        channels = MessagePort::disentanglePorts(executionContext, ports, exceptionState);
        if (exceptionState.hadException())
            return;
    }

    WebString messageString = message->toWireString();
    OwnPtr<WebMessagePortChannelArray> webChannels = MessagePort::toWebMessagePortChannelArray(channels.release());
    if (m_collection) {
        WebServicePortProvider* provider = m_collection->provider();
        provider->postMessage(m_port.id, messageString, webChannels.leakPtr());
    }
}

void ServicePort::close()
{
    // TODO(mek): Figure out if this should throw instead of just quietly fail.
    if (!m_isOpen)
        return;

    m_collection->closePort(this);
    m_collection = nullptr;
    m_isOpen = false;
}

DEFINE_TRACE(ServicePort)
{
    visitor->trace(m_collection);
}

ServicePort::ServicePort(ServicePortCollection* collection, const WebServicePort& port)
    : m_isOpen(true), m_port(port), m_collection(collection)
{
    if (!m_port.data.isEmpty()) {
        m_serializedData = SerializedScriptValueFactory::instance().createFromWire(m_port.data);
    }
}

} // namespace blink

