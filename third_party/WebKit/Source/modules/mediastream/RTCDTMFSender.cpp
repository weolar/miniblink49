/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/mediastream/RTCDTMFSender.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/mediastream/MediaStreamTrack.h"
#include "modules/mediastream/RTCDTMFToneChangeEvent.h"
#include "public/platform/WebMediaStreamTrack.h"
#include "public/platform/WebRTCDTMFSenderHandler.h"
#include "public/platform/WebRTCPeerConnectionHandler.h"

namespace blink {

static const int minToneDurationMs = 70;
static const int defaultToneDurationMs = 100;
static const int maxToneDurationMs = 6000;
static const int minInterToneGapMs = 50;
static const int defaultInterToneGapMs = 50;

RTCDTMFSender* RTCDTMFSender::create(ExecutionContext* context, WebRTCPeerConnectionHandler* peerConnectionHandler, MediaStreamTrack* track, ExceptionState& exceptionState)
{
    OwnPtr<WebRTCDTMFSenderHandler> handler = adoptPtr(peerConnectionHandler->createDTMFSender(track->component()));
    if (!handler) {
        exceptionState.throwDOMException(NotSupportedError, "The MediaStreamTrack provided is not an element of a MediaStream that's currently in the local streams set.");
        return nullptr;
    }

    RTCDTMFSender* dtmfSender = new RTCDTMFSender(context, track, handler.release());
    dtmfSender->suspendIfNeeded();
    return dtmfSender;
}

RTCDTMFSender::RTCDTMFSender(ExecutionContext* context, MediaStreamTrack* track, PassOwnPtr<WebRTCDTMFSenderHandler> handler)
    : ActiveDOMObject(context)
    , m_track(track)
    , m_duration(defaultToneDurationMs)
    , m_interToneGap(defaultInterToneGapMs)
    , m_handler(handler)
    , m_stopped(false)
    , m_scheduledEventTimer(this, &RTCDTMFSender::scheduledEventTimerFired)
{
    m_handler->setClient(this);
}

RTCDTMFSender::~RTCDTMFSender()
{
}

bool RTCDTMFSender::canInsertDTMF() const
{
    return m_handler->canInsertDTMF();
}

MediaStreamTrack* RTCDTMFSender::track() const
{
    return m_track.get();
}

String RTCDTMFSender::toneBuffer() const
{
    return m_handler->currentToneBuffer();
}

void RTCDTMFSender::insertDTMF(const String& tones, ExceptionState& exceptionState)
{
    insertDTMF(tones, defaultToneDurationMs, defaultInterToneGapMs, exceptionState);
}

void RTCDTMFSender::insertDTMF(const String& tones, int duration, ExceptionState& exceptionState)
{
    insertDTMF(tones, duration, defaultInterToneGapMs, exceptionState);
}

void RTCDTMFSender::insertDTMF(const String& tones, int duration, int interToneGap, ExceptionState& exceptionState)
{
    if (!canInsertDTMF()) {
        exceptionState.throwDOMException(NotSupportedError, "The 'canInsertDTMF' attribute is false: this sender cannot send DTMF.");
        return;
    }

    if (duration > maxToneDurationMs || duration < minToneDurationMs) {
        exceptionState.throwDOMException(SyntaxError, ExceptionMessages::indexOutsideRange("duration", duration, minToneDurationMs, ExceptionMessages::ExclusiveBound, maxToneDurationMs, ExceptionMessages::ExclusiveBound));
        return;
    }

    if (interToneGap < minInterToneGapMs) {
        exceptionState.throwDOMException(SyntaxError, ExceptionMessages::indexExceedsMinimumBound("intertone gap", interToneGap, minInterToneGapMs));
        return;
    }

    m_duration = duration;
    m_interToneGap = interToneGap;

    if (!m_handler->insertDTMF(tones, m_duration, m_interToneGap))
        exceptionState.throwDOMException(SyntaxError, "Could not send provided tones, '" + tones + "'.");
}

void RTCDTMFSender::didPlayTone(const WebString& tone)
{
    scheduleDispatchEvent(RTCDTMFToneChangeEvent::create(tone));
}

const AtomicString& RTCDTMFSender::interfaceName() const
{
    return EventTargetNames::RTCDTMFSender;
}

ExecutionContext* RTCDTMFSender::executionContext() const
{
    return ActiveDOMObject::executionContext();
}

void RTCDTMFSender::stop()
{
    m_stopped = true;
    m_handler->setClient(0);
}

void RTCDTMFSender::scheduleDispatchEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    m_scheduledEvents.append(event);

    if (!m_scheduledEventTimer.isActive())
        m_scheduledEventTimer.startOneShot(0, FROM_HERE);
}

void RTCDTMFSender::scheduledEventTimerFired(Timer<RTCDTMFSender>*)
{
    if (m_stopped)
        return;

    WillBeHeapVector<RefPtrWillBeMember<Event>> events;
    events.swap(m_scheduledEvents);

    WillBeHeapVector<RefPtrWillBeMember<Event>>::iterator it = events.begin();
    for (; it != events.end(); ++it)
        dispatchEvent((*it).release());
}

DEFINE_TRACE(RTCDTMFSender)
{
    visitor->trace(m_track);
    visitor->trace(m_scheduledEvents);
    RefCountedGarbageCollectedEventTargetWithInlineData<RTCDTMFSender>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
