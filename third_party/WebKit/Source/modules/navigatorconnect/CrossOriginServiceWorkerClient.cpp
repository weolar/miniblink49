// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/navigatorconnect/CrossOriginServiceWorkerClient.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/MessagePort.h"
#include "modules/EventTargetModules.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScopeClient.h"
#include "public/platform/WebCrossOriginServiceWorkerClient.h"

namespace blink {

CrossOriginServiceWorkerClient* CrossOriginServiceWorkerClient::create(const WebCrossOriginServiceWorkerClient& client)
{
    return new CrossOriginServiceWorkerClient(client);
}

CrossOriginServiceWorkerClient::CrossOriginServiceWorkerClient(const WebCrossOriginServiceWorkerClient& client)
    : m_webClient(client)
{
}

CrossOriginServiceWorkerClient::~CrossOriginServiceWorkerClient()
{
}

String CrossOriginServiceWorkerClient::origin() const
{
    return m_webClient.origin.string();
}

String CrossOriginServiceWorkerClient::targetUrl() const
{
    return m_webClient.targetURL.string();
}

void CrossOriginServiceWorkerClient::postMessage(ExecutionContext* executionContext, PassRefPtr<SerializedScriptValue> message, const MessagePortArray* ports, ExceptionState& exceptionState)
{
    OwnPtr<MessagePortChannelArray> channels;
    if (ports) {
        channels = MessagePort::disentanglePorts(executionContext, ports, exceptionState);
        if (exceptionState.hadException())
            return;
    }

    WebString messageString = message->toWireString();
    OwnPtr<WebMessagePortChannelArray> webChannels = MessagePort::toWebMessagePortChannelArray(channels.release());
    ServiceWorkerGlobalScopeClient::from(executionContext)->postMessageToCrossOriginClient(m_webClient, messageString, webChannels.release());
}

} // namespace blink
