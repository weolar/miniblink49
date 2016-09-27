/*
 * Copyright (C) 2009, 2012 Ericsson AB. All rights reserved.
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Ericsson nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#ifndef EventSource_h
#define EventSource_h

#include "core/dom/ActiveDOMObject.h"
#include "core/events/EventTarget.h"
#include "core/loader/ThreadableLoaderClient.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class EventSourceInit;
class ExceptionState;
class MessageEvent;
class ResourceResponse;
class TextResourceDecoder;
class ThreadableLoader;

class EventSource final : public RefCountedGarbageCollectedEventTargetWithInlineData<EventSource>, private ThreadableLoaderClient, public ActiveDOMObject {
    DEFINE_WRAPPERTYPEINFO();
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(EventSource);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(EventSource);
public:
    static EventSource* create(ExecutionContext*, const String& url, const EventSourceInit&, ExceptionState&);
    virtual ~EventSource();

    static const unsigned long long defaultReconnectDelay;

    String url() const;
    bool withCredentials() const;

    typedef short State;
    static const State CONNECTING = 0;
    static const State OPEN = 1;
    static const State CLOSED = 2;

    State readyState() const;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(open);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(message);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(error);

    void close();

    virtual const AtomicString& interfaceName() const override;
    virtual ExecutionContext* executionContext() const override;

    // ActiveDOMObject
    //
    // Note: suspend() is noop since ScopedPageLoadDeferrer calls
    // Page::setDefersLoading() and it defers delivery of events from the
    // loader, and therefore the methods of this class for receiving
    // asynchronous events from the loader won't be invoked.
    virtual void stop() override;

    virtual bool hasPendingActivity() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    EventSource(ExecutionContext*, const KURL&, const EventSourceInit&);

    virtual void didReceiveResponse(unsigned long, const ResourceResponse&, PassOwnPtr<WebDataConsumerHandle>) override;
    virtual void didReceiveData(const char*, unsigned) override;
    virtual void didFinishLoading(unsigned long, double) override;
    virtual void didFail(const ResourceError&) override;
    virtual void didFailAccessControlCheck(const ResourceError&) override;
    virtual void didFailRedirectCheck() override;

    void scheduleInitialConnect();
    void connect();
    void networkRequestEnded();
    void scheduleReconnect();
    void connectTimerFired(Timer<EventSource>*);
    void abortConnectionAttempt();
    void parseEventStream();
    void parseEventStreamLine(unsigned pos, int fieldLength, int lineLength);
    PassRefPtrWillBeRawPtr<MessageEvent> createMessageEvent();

    KURL m_url;
    bool m_withCredentials;
    State m_state;

    OwnPtr<TextResourceDecoder> m_decoder;
    RefPtr<ThreadableLoader> m_loader;
    Timer<EventSource> m_connectTimer;
    Vector<UChar> m_receiveBuf;
    bool m_discardTrailingNewline;
    bool m_requestInFlight;

    AtomicString m_eventName;
    Vector<UChar> m_data;
    AtomicString m_currentlyParsedEventId;
    AtomicString m_lastEventId;
    unsigned long long m_reconnectDelay;
    String m_eventStreamOrigin;
};

} // namespace blink

#endif // EventSource_h
