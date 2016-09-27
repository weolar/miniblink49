/*
 * Copyright (C) 2010 Julien Chaffraix <jchaffraix@webkit.org>
 * All right reserved.
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

#ifndef XMLHttpRequestProgressEventThrottle_h
#define XMLHttpRequestProgressEventThrottle_h

#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class Event;
class XMLHttpRequest;

// This class implements the XHR2 ProgressEvent dispatching:
//   "dispatch a progress event named progress about every 50ms or for every
//   byte received, whichever is least frequent".
//
// readystatechange events also go through this class since ProgressEvents and
// readystatechange events affect each other.
//
// In the comments for this class:
// - "progress" event means an event named "progress"
// - ProgressEvent means an event using the ProgressEvent interface defined in
//   the spec.
class XMLHttpRequestProgressEventThrottle final : public GarbageCollectedFinalized<XMLHttpRequestProgressEventThrottle>, public TimerBase {
public:
    static XMLHttpRequestProgressEventThrottle* create(XMLHttpRequest* eventTarget)
    {
        return new XMLHttpRequestProgressEventThrottle(eventTarget);
    }
    ~XMLHttpRequestProgressEventThrottle() override;

    enum DeferredEventAction {
        Ignore,
        Clear,
        Flush,
    };

    // Dispatches a ProgressEvent.
    //
    // Special treatment for events named "progress" is implemented to dispatch
    // them at the required frequency. If this object is suspended, the given
    // ProgressEvent overwrites the existing. I.e. only the latest one gets
    // queued. If the timer is running, this method just updates
    // m_lengthComputable, m_loaded and m_total. They'll be used on next
    // fired() call.
    void dispatchProgressEvent(const AtomicString&, bool lengthComputable, unsigned long long loaded, unsigned long long total);
    // Dispatches the given event after operation about the "progress" event
    // depending on the value of the ProgressEventAction argument.
    void dispatchReadyStateChangeEvent(PassRefPtrWillBeRawPtr<Event>, DeferredEventAction);

    void suspend();
    void resume();

    // Need to promptly stop this timer when it is deemed finalizable.
    EAGERLY_FINALIZE();
    DECLARE_TRACE();

private:
    explicit XMLHttpRequestProgressEventThrottle(XMLHttpRequest*);

    // The main purpose of this class is to throttle the "progress"
    // ProgressEvent dispatching. This class represents such a deferred
    // "progress" ProgressEvent.
    class DeferredEvent;
    static const double minimumProgressEventDispatchingIntervalInSeconds;

    void fired() override;
    void dispatchDeferredEvent();

    // Non-Oilpan, keep a weak pointer to our XMLHttpRequest object as it is
    // the one holding us. With Oilpan, a simple strong Member can be used -
    // this XMLHttpRequestProgressEventThrottle (part) object dies together
    // with the XMLHttpRequest object.
    Member<XMLHttpRequest> m_target;

    // A slot for the deferred "progress" ProgressEvent. When multiple events
    // arrive, only the last one is stored and others are discarded.
    const OwnPtr<DeferredEvent> m_deferred;
};

} // namespace blink

#endif // XMLHttpRequestProgressEventThrottle_h
