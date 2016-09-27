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

#ifndef RTCDTMFSender_h
#define RTCDTMFSender_h

#include "core/dom/ActiveDOMObject.h"
#include "modules/EventTargetModules.h"
#include "platform/Timer.h"
#include "public/platform/WebRTCDTMFSenderHandlerClient.h"

namespace blink {

class ExceptionState;
class MediaStreamTrack;
class WebRTCDTMFSenderHandler;
class WebRTCPeerConnectionHandler;

class RTCDTMFSender final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<RTCDTMFSender>
    , public WebRTCDTMFSenderHandlerClient
    , public ActiveDOMObject {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(RTCDTMFSender);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(RTCDTMFSender);
    DEFINE_WRAPPERTYPEINFO();
public:
    static RTCDTMFSender* create(ExecutionContext*, WebRTCPeerConnectionHandler*, MediaStreamTrack*, ExceptionState&);
    ~RTCDTMFSender() override;

    bool canInsertDTMF() const;
    MediaStreamTrack* track() const;
    String toneBuffer() const;
    int duration() const { return m_duration; }
    int interToneGap() const { return m_interToneGap; }

    void insertDTMF(const String& tones, ExceptionState&);
    void insertDTMF(const String& tones, int duration, ExceptionState&);
    void insertDTMF(const String& tones, int duration, int interToneGap, ExceptionState&);

    DEFINE_ATTRIBUTE_EVENT_LISTENER(tonechange);

    // EventTarget
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    // ActiveDOMObject
    void stop() override;

    // Oilpan: need to eagerly finalize m_handler
    EAGERLY_FINALIZE();
    DECLARE_VIRTUAL_TRACE();

private:
    RTCDTMFSender(ExecutionContext*, MediaStreamTrack*, PassOwnPtr<WebRTCDTMFSenderHandler>);

    void scheduleDispatchEvent(PassRefPtrWillBeRawPtr<Event>);
    void scheduledEventTimerFired(Timer<RTCDTMFSender>*);

    // WebRTCDTMFSenderHandlerClient
    void didPlayTone(const WebString&) override;

    Member<MediaStreamTrack> m_track;
    int m_duration;
    int m_interToneGap;

    OwnPtr<WebRTCDTMFSenderHandler> m_handler;

    bool m_stopped;

    Timer<RTCDTMFSender> m_scheduledEventTimer;
    WillBeHeapVector<RefPtrWillBeMember<Event>> m_scheduledEvents;
};

} // namespace blink

#endif // RTCDTMFSender_h
