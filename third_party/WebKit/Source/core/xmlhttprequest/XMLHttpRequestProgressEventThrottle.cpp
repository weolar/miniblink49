/*
 * Copyright (C) 2010 Julien Chaffraix <jchaffraix@webkit.org>  All right reserved.
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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

#include "config.h"
#include "core/xmlhttprequest/XMLHttpRequestProgressEventThrottle.h"

#include "core/EventTypeNames.h"
#include "core/xmlhttprequest/XMLHttpRequest.h"
#include "core/xmlhttprequest/XMLHttpRequestProgressEvent.h"
#include "wtf/Assertions.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class XMLHttpRequestProgressEventThrottle::DeferredEvent {
public:
    DeferredEvent() { clear(); }
    void set(bool lengthComputable, unsigned long long loaded, unsigned long long total)
    {
        m_isDeferred = true;
        m_lengthComputable = lengthComputable;
        m_loaded = loaded;
        m_total = total;
    }
    void clear()
    {
        m_isDeferred = false;
        m_lengthComputable = false;
        m_loaded = 0;
        m_total = 0;
    }
    bool isDeferred() const { return m_isDeferred; }
    bool lengthComputable() const { return m_lengthComputable; }
    unsigned long long loaded() const { return m_loaded; }
    unsigned long long total() const { return m_total; }

private:
    unsigned long long m_loaded;
    unsigned long long m_total;
    bool m_isDeferred;
    bool m_lengthComputable;
};

const double XMLHttpRequestProgressEventThrottle::minimumProgressEventDispatchingIntervalInSeconds = .05; // 50 ms per specification.

XMLHttpRequestProgressEventThrottle::XMLHttpRequestProgressEventThrottle(XMLHttpRequest* target)
    : m_target(target)
    , m_deferred(adoptPtr(new DeferredEvent))
{
    ASSERT(target);
}

XMLHttpRequestProgressEventThrottle::~XMLHttpRequestProgressEventThrottle()
{
}

void XMLHttpRequestProgressEventThrottle::dispatchProgressEvent(const AtomicString& type, bool lengthComputable, unsigned long long loaded, unsigned long long total)
{
    // Given that ResourceDispatcher doesn't deliver an event when suspended,
    // we don't have to worry about event dispatching while suspended.
    if (type != EventTypeNames::progress) {
        m_target->dispatchEvent(XMLHttpRequestProgressEvent::create(type, lengthComputable, loaded, total));
        return;
    }

    if (isActive()) {
        m_deferred->set(lengthComputable, loaded, total);
    } else {
        m_target->dispatchEvent(XMLHttpRequestProgressEvent::create(type, lengthComputable, loaded, total));
        startOneShot(minimumProgressEventDispatchingIntervalInSeconds, FROM_HERE);
    }
}

void XMLHttpRequestProgressEventThrottle::dispatchReadyStateChangeEvent(PassRefPtrWillBeRawPtr<Event> event, DeferredEventAction action)
{
    XMLHttpRequest::State state = m_target->readyState();
    // Given that ResourceDispatcher doesn't deliver an event when suspended,
    // we don't have to worry about event dispatching while suspended.
    if (action == Flush) {
        dispatchDeferredEvent();
        // |m_target| is protected by the caller.
        stop();
    } else if (action == Clear) {
        m_deferred->clear();
        stop();
    }

    if (state == m_target->readyState()) {
        // We don't dispatch the event when an event handler associated with
        // the previously dispatched event changes the readyState (e.g. when
        // the event handler calls xhr.abort()). In such cases a
        // readystatechange should have been already dispatched if necessary.
        m_target->dispatchEvent(event);
    }
}

void XMLHttpRequestProgressEventThrottle::dispatchDeferredEvent()
{
    if (m_deferred->isDeferred()) {
        m_target->dispatchEvent(XMLHttpRequestProgressEvent::create(EventTypeNames::progress, m_deferred->lengthComputable(), m_deferred->loaded(), m_deferred->total()));
        m_deferred->clear();
    }
}

void XMLHttpRequestProgressEventThrottle::fired()
{
    if (!m_deferred->isDeferred()) {
        // No "progress" event was queued since the previous dispatch, we can
        // safely stop the timer.
        return;
    }

    dispatchDeferredEvent();

    // Watch if another "progress" ProgressEvent arrives in the next 50ms.
    startOneShot(minimumProgressEventDispatchingIntervalInSeconds, FROM_HERE);
}

void XMLHttpRequestProgressEventThrottle::suspend()
{
    stop();
}

void XMLHttpRequestProgressEventThrottle::resume()
{
    if (!m_deferred->isDeferred())
        return;

    // Do not dispatch events inline here, since ExecutionContext is iterating
    // over the list of active DOM objects to resume them, and any activated JS
    // event-handler could insert new active DOM objects to the list.
    startOneShot(0, FROM_HERE);
}

DEFINE_TRACE(XMLHttpRequestProgressEventThrottle)
{
    visitor->trace(m_target);
}

} // namespace blink
