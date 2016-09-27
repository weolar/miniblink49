/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RTCDataChannel_h
#define RTCDataChannel_h

#include "modules/EventTargetModules.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebRTCDataChannelHandler.h"
#include "public/platform/WebRTCDataChannelHandlerClient.h"

namespace blink {

class Blob;
class DOMArrayBuffer;
class DOMArrayBufferView;
class ExceptionState;
class RTCPeerConnection;
class WebRTCDataChannelHandler;
class WebRTCPeerConnectionHandler;
struct WebRTCDataChannelInit;

class RTCDataChannel final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<RTCDataChannel>
    , public WebRTCDataChannelHandlerClient {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(RTCDataChannel);
    DEFINE_WRAPPERTYPEINFO();
public:
    static RTCDataChannel* create(ExecutionContext*, RTCPeerConnection*, PassOwnPtr<WebRTCDataChannelHandler>);
    static RTCDataChannel* create(ExecutionContext*, RTCPeerConnection*, WebRTCPeerConnectionHandler*, const String& label, const WebRTCDataChannelInit&, ExceptionState&);
    ~RTCDataChannel() override;

    ReadyState getHandlerState() const;

    String label() const;

    // DEPRECATED
    bool reliable() const;

    bool ordered() const;
    unsigned short maxRetransmitTime() const;
    unsigned short maxRetransmits() const;
    String protocol() const;
    bool negotiated() const;
    unsigned short id() const;
    String readyState() const;
    unsigned bufferedAmount() const;

    String binaryType() const;
    void setBinaryType(const String&, ExceptionState&);

    void send(const String&, ExceptionState&);
    void send(PassRefPtr<DOMArrayBuffer>, ExceptionState&);
    void send(PassRefPtr<DOMArrayBufferView>, ExceptionState&);
    void send(Blob*, ExceptionState&);

    void close();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(open);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(error);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(close);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(message);

    void stop();

    // EventTarget
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    void clearWeakMembers(Visitor*);

    // Oilpan: need to eagerly finalize m_handler
    EAGERLY_FINALIZE();
    DECLARE_VIRTUAL_TRACE();

    // WebRTCDataChannelHandlerClient
    void didChangeReadyState(WebRTCDataChannelHandlerClient::ReadyState) override;
    void didReceiveStringData(const WebString&) override;
    void didReceiveRawData(const char*, size_t) override;
    void didDetectError() override;

private:
    RTCDataChannel(ExecutionContext*, RTCPeerConnection*, PassOwnPtr<WebRTCDataChannelHandler>);

    void scheduleDispatchEvent(PassRefPtrWillBeRawPtr<Event>);
    void scheduledEventTimerFired(Timer<RTCDataChannel>*);

    RawPtrWillBeMember<ExecutionContext> m_executionContext;

    OwnPtr<WebRTCDataChannelHandler> m_handler;

    bool m_stopped;

    WebRTCDataChannelHandlerClient::ReadyState m_readyState;

    enum BinaryType {
        BinaryTypeBlob,
        BinaryTypeArrayBuffer
    };
    BinaryType m_binaryType;

    Timer<RTCDataChannel> m_scheduledEventTimer;
    WillBeHeapVector<RefPtrWillBeMember<Event>> m_scheduledEvents;

    WeakMember<RTCPeerConnection> m_connection;
};

} // namespace blink

#endif // RTCDataChannel_h
