/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef content_browser_PlatformMessagePortChannel_h
#define content_browser_PlatformMessagePortChannel_h

//#include "third_party/WebKit/Source/bindings/core/v8/SerializedScriptValue.h"
#include "third_party/WebKit/public/platform/WebMessagePortChannel.h"
#include "third_party/WebKit/public/web/WebSerializedScriptValue.h"

#include "third_party/WebKit/Source/wtf/MessageQueue.h"
#include "third_party/WebKit/Source/wtf/PassRefPtr.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/wtf/ThreadSafeRefCounted.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"

namespace content {

class MessagePort;

// PlatformMessagePortChannel is a platform-dependent interface to the remote side of a message channel.
// This default implementation supports multiple threads running within a single process. Implementations for multi-process platforms should define these public APIs in their own platform-specific PlatformMessagePortChannel file.
// The goal of this implementation is to eliminate contention except when cloning or closing the port, so each side of the channel has its own separate mutex.
class PlatformMessagePortChannel : public NoBaseWillBeGarbageCollectedFinalized<PlatformMessagePortChannel> {
public:
    class EventData {
        WTF_MAKE_NONCOPYABLE(EventData); WTF_MAKE_FAST_ALLOCATED(EventData);
    public:
        EventData(const blink::WebSerializedScriptValue& message, WTF::PassOwnPtr<blink::WebMessagePortChannelArray>);

        const blink::WebSerializedScriptValue& message() { return m_message; }
        WTF::PassOwnPtr<blink::WebMessagePortChannelArray> channels() { return WTF::adoptPtr(m_channels.leakPtr()); }

    private:
        blink::WebSerializedScriptValue m_message;
        WTF::OwnPtr<blink::WebMessagePortChannelArray> m_channels;
    };

    // Wrapper for MessageQueue that allows us to do thread safe sharing by two proxies.
    class MessagePortQueue : public NoBaseWillBeGarbageCollectedFinalized<MessagePortQueue> {
    public:
        static MessagePortQueue* create() { return new MessagePortQueue(); }

        WTF::PassOwnPtr<PlatformMessagePortChannel::EventData> tryGetMessage()
        {
            return m_queue.tryGetMessage();
        }

        bool appendAndCheckEmpty(WTF::PassOwnPtr<PlatformMessagePortChannel::EventData> message)
        {
            return m_queue.appendAndCheckEmpty(WTF::adoptPtr(message.leakPtr()));
        }

        bool isEmpty()
        {
            return m_queue.isEmpty();
        }

        ~MessagePortQueue()
        {
//             String output = String::format("~MessagePortQueue: %p\n", this);
//             OutputDebugStringA(output.utf8().data());
        }

        DECLARE_TRACE();

    private:
        MessagePortQueue()
        {
//             String output = String::format("MessagePortQueue: %p\n", this);
//             OutputDebugStringA(output.utf8().data());
        }

        MessageQueue<PlatformMessagePortChannel::EventData> m_queue;
    };

    ~PlatformMessagePortChannel();

    DECLARE_TRACE();

    static WTF::PassOwnPtr<PlatformMessagePortChannel> create(MessagePortQueue* incoming, MessagePortQueue* outgoing);
    PlatformMessagePortChannel(MessagePortQueue* incoming, MessagePortQueue* outgoing);

    PlatformMessagePortChannel* entangledChannel();

    void setRemotePort(blink::WebMessagePortChannelClient* remotePort);
    void closeInternal();

    // Lock used to ensure exclusive access to the object internals.
    WTF::Mutex m_mutex;

    // Pointer to our entangled pair - cleared when close() is called.
    //RefPtr<PlatformMessagePortChannel> m_entangledChannel;
    PlatformMessagePortChannel* m_entangledChannel;

    // Reference to the message queue for the (local) entangled port.
    MessagePortQueue* m_incomingQueue;
    MessagePortQueue* m_outgoingQueue;

    // The port we are connected to (the remote port) - this is the port that is notified when new messages arrive.
    blink::WebMessagePortChannelClient* m_remotePort;
};

} // namespace content

#endif // content_browser_PlatformMessagePortChannel_h
