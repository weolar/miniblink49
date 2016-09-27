// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/StashedMessagePort.h"

#include "bindings/core/v8/SerializedScriptValue.h"
#include "core/dom/CrossThreadTask.h"
#include "core/events/MessageEvent.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScope.h"
#include "modules/serviceworkers/StashedPortCollection.h"
#include "public/platform/WebTraceLocation.h"

namespace blink {

StashedMessagePort* StashedMessagePort::create(ExecutionContext& executionContext, PassOwnPtr<WebMessagePortChannel> remote, const String& name)
{
    StashedMessagePort* port = new StashedMessagePort(executionContext, remote, name);
    port->suspendIfNeeded();
    return port;
}

StashedMessagePort::StashedMessagePort(ExecutionContext& executionContext, PassOwnPtr<WebMessagePortChannel> remote, const String& name)
    : MessagePort(executionContext), m_name(name), m_weakFactory(this)
{
    entangle(remote);
    start();
}

StashedMessagePort::~StashedMessagePort()
{
}

StashedMessagePortArray* StashedMessagePort::toStashedMessagePortArray(ExecutionContext* context, const WebMessagePortChannelArray& webChannels, const WebVector<WebString>& channelKeys)
{
    StashedMessagePortArray* ports = new StashedMessagePortArray(webChannels.size());
    for (size_t i = 0; i < webChannels.size(); ++i) {
        OwnPtr<WebMessagePortChannel> channel(adoptPtr(webChannels[i]));
        (*ports)[i] = StashedMessagePort::create(*context, channel.release(), channelKeys[i]);
    }
    return ports;
}

String StashedMessagePort::name() const
{
    return m_name;
}

void StashedMessagePort::messageAvailable()
{
    ASSERT(executionContext());
    executionContext()->postTask(FROM_HERE, createCrossThreadTask(&StashedMessagePort::dispatchMessages, m_weakFactory.createWeakPtr()));
}

void StashedMessagePort::dispatchMessages()
{
    if (!isEntangled())
        return;
    ASSERT(executionContext()->isServiceWorkerGlobalScope());
    RefPtrWillBeRawPtr<StashedPortCollection> stashedPorts = toServiceWorkerGlobalScope(executionContext())->ports();

    RefPtr<SerializedScriptValue> message;
    OwnPtr<MessagePortChannelArray> channels;
    while (tryGetMessage(message, channels)) {
        MessagePortArray* ports = MessagePort::entanglePorts(*executionContext(), channels.release());
        RefPtrWillBeRawPtr<Event> evt = MessageEvent::create(ports, message.release(), String(), String(), this);

        stashedPorts->dispatchEvent(evt.release(), ASSERT_NO_EXCEPTION);
    }
}

} // namespace blink
