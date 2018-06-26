// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/web_impl_win/WebMessagePortChannelImpl.h"

#include "third_party/WebKit/public/platform/WebMessagePortChannelClient.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/web/WebSerializedScriptValue.h"
#include "v8.h"

using blink::WebMessagePortChannel;
using blink::WebMessagePortChannelArray;
using blink::WebMessagePortChannelClient;
using blink::WebString;

namespace content {

WebMessagePortChannelImpl::WebMessagePortChannelImpl(
    PlatformMessagePortChannel::MessagePortQueue* incoming, 
    PlatformMessagePortChannel::MessagePortQueue* outgoing)
    : m_channel((PlatformMessagePortChannel::create(incoming, outgoing).leakPtr()))
{
    String output = String::format("WebMessagePortChannelImpl::WebMessagePortChannelImpl: %p\n", this);
    OutputDebugStringA(output.utf8().data());

    m_keepAlive = this;
}

WebMessagePortChannelImpl::~WebMessagePortChannelImpl()
{
    String output = String::format("WebMessagePortChannelImpl::~WebMessagePortChannelImpl: %p\n", this);
    OutputDebugStringA(output.utf8().data());
}

void WebMessagePortChannelImpl::setClient(blink::WebMessagePortChannelClient* client)
{
    RELEASE_ASSERT (WTF::isMainThread()); // 暂时不支持多线程
    // Must lock here since m_client is called on the main thread.
    m_channel->setRemotePort(client);

    String output = String::format("WebMessagePortChannelImpl::setClient: this:%p, MessagePort:%p\n", this, m_channel->m_remotePort);
    OutputDebugStringA(output.utf8().data());
}

void WebMessagePortChannelImpl::destroy()
{
    RELEASE_ASSERT(WTF::isMainThread());

    m_keepAlive = nullptr;
    PlatformMessagePortChannel* remote = m_channel->entangledChannel();
    if (!remote)
        return;
    m_channel->closeInternal();
    remote->closeInternal();
}

void WebMessagePortChannelImpl::postMessage(const blink::WebString& message, blink::WebMessagePortChannelArray* channels)
{
    RELEASE_ASSERT(WTF::isMainThread());
    blink::WebSerializedScriptValue serializedValue = blink::WebSerializedScriptValue::fromString(message);

    WTF::MutexLocker autoLock(m_channel->m_mutex);
    if (!m_channel->m_outgoingQueue)
        return;

    bool wasEmpty = m_channel->m_outgoingQueue->appendAndCheckEmpty(WTF::adoptPtr(new PlatformMessagePortChannel::EventData(serializedValue, WTF::adoptPtr(channels))));
    if (wasEmpty && m_channel->m_entangledChannel && m_channel->m_entangledChannel->m_remotePort) {

//         String output = String::format("WebMessagePortChannelImpl::postMessage: this:%p, MessagePort:%p\n", this, m_channel->m_remotePort);
//         OutputDebugStringA(output.utf8().data());

        m_channel->m_entangledChannel->m_remotePort->messageAvailable();
    }
}

bool WebMessagePortChannelImpl::tryGetMessage(blink::WebString* message, blink::WebMessagePortChannelArray& channels)
{
    WTF::MutexLocker autoLock(m_channel->m_mutex);
    WTF::PassOwnPtr<PlatformMessagePortChannel::EventData> result = m_channel->m_incomingQueue->tryGetMessage();
    if (!result)
        return false;

    *message = result->message().toString();

    WTF::PassOwnPtr<blink::WebMessagePortChannelArray> resultChannels = result->channels();
    if (!!resultChannels)
        channels.assign(resultChannels->data(), resultChannels->size());

    return true;
}

using namespace blink;

DEFINE_TRACE(WebMessagePortChannelImpl)
{
    visitor->trace(m_channel);
}

}