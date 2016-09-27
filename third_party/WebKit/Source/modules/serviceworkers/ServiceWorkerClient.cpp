// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/ServiceWorkerClient.h"

#include "bindings/core/v8/CallbackPromiseAdapter.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "modules/serviceworkers/ServiceWorkerGlobalScopeClient.h"
#include "public/platform/WebString.h"
#include "wtf/RefPtr.h"

namespace blink {

ServiceWorkerClient* ServiceWorkerClient::create(const WebServiceWorkerClientInfo& info)
{
    return new ServiceWorkerClient(info);
}

ServiceWorkerClient::ServiceWorkerClient(const WebServiceWorkerClientInfo& info)
    : m_uuid(info.uuid)
    , m_url(info.url.string())
    , m_frameType(info.frameType)
{
}

ServiceWorkerClient::~ServiceWorkerClient()
{
}

String ServiceWorkerClient::frameType() const
{
    switch (m_frameType) {
    case WebURLRequest::FrameTypeAuxiliary:
        return "auxiliary";
    case WebURLRequest::FrameTypeNested:
        return "nested";
    case WebURLRequest::FrameTypeNone:
        return "none";
    case WebURLRequest::FrameTypeTopLevel:
        return "top-level";
    }

    ASSERT_NOT_REACHED();
    return String();
}

void ServiceWorkerClient::postMessage(ExecutionContext* context, PassRefPtr<SerializedScriptValue> message, const MessagePortArray* ports, ExceptionState& exceptionState)
{
    // Disentangle the port in preparation for sending it to the remote context.
    OwnPtr<MessagePortChannelArray> channels = MessagePort::disentanglePorts(context, ports, exceptionState);
    if (exceptionState.hadException())
        return;

    WebString messageString = message->toWireString();
    OwnPtr<WebMessagePortChannelArray> webChannels = MessagePort::toWebMessagePortChannelArray(channels.release());
    ServiceWorkerGlobalScopeClient::from(context)->postMessageToClient(m_uuid, messageString, webChannels.release());
}

} // namespace blink
