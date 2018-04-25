/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "content/browser/PlatformMessagePortChannel.h"

namespace content {

PlatformMessagePortChannel::EventData::EventData(const blink::WebSerializedScriptValue& message, WTF::PassOwnPtr<blink::WebMessagePortChannelArray> channels)
    : m_message(message)
    , m_channels(WTF::adoptPtr(channels.leakPtr()))
{
}

WTF::PassOwnPtr<PlatformMessagePortChannel> PlatformMessagePortChannel::create(MessagePortQueue* incoming, MessagePortQueue* outgoing)
{
    return WTF::adoptPtr(new PlatformMessagePortChannel(incoming, outgoing));
}

PlatformMessagePortChannel::PlatformMessagePortChannel(MessagePortQueue* incoming, MessagePortQueue* outgoing)
    : m_incomingQueue(incoming)
    , m_outgoingQueue(outgoing)
    , m_remotePort(0)
{
}

PlatformMessagePortChannel::~PlatformMessagePortChannel()
{
}

void PlatformMessagePortChannel::setRemotePort(blink::WebMessagePortChannelClient* remotePort)
{
    WTF::MutexLocker autoLock(m_mutex);
    // Should never set port if it is already set.
    ASSERT(!remotePort || !m_remotePort);
    m_remotePort = remotePort;
}

PlatformMessagePortChannel* PlatformMessagePortChannel::entangledChannel()
{
    // FIXME: What guarantees that the result remains the same after we release the lock?
    // This lock only guarantees that the returned pointer will not be pointing to released memory,
    // but not that it will still be pointing to this object's entangled port channel.
    WTF::MutexLocker lock(m_mutex);
    return m_entangledChannel;
}

void PlatformMessagePortChannel::closeInternal()
{
    WTF::MutexLocker lock(m_mutex);
    // Disentangle ourselves from the other end. We still maintain a reference to our incoming queue, since previously-existing messages should still be delivered.
    m_remotePort = nullptr;
    m_entangledChannel = nullptr;
    m_outgoingQueue = nullptr;
}

using namespace blink;

DEFINE_TRACE(PlatformMessagePortChannel)
{
    visitor->trace(m_entangledChannel);
    visitor->trace(m_outgoingQueue);
    visitor->trace(m_incomingQueue);
}

DEFINE_TRACE(PlatformMessagePortChannel::MessagePortQueue)
{

}

} // namespace content
