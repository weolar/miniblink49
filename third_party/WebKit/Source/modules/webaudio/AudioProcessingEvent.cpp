/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/AudioProcessingEvent.h"

namespace blink {

PassRefPtrWillBeRawPtr<AudioProcessingEvent> AudioProcessingEvent::create()
{
    return adoptRefWillBeNoop(new AudioProcessingEvent);
}

PassRefPtrWillBeRawPtr<AudioProcessingEvent> AudioProcessingEvent::create(AudioBuffer* inputBuffer, AudioBuffer* outputBuffer, double playbackTime)
{
    return adoptRefWillBeNoop(new AudioProcessingEvent(inputBuffer, outputBuffer, playbackTime));
}

AudioProcessingEvent::AudioProcessingEvent()
{
}

AudioProcessingEvent::AudioProcessingEvent(AudioBuffer* inputBuffer, AudioBuffer* outputBuffer, double playbackTime)
    : Event(EventTypeNames::audioprocess, true, false)
    , m_inputBuffer(inputBuffer)
    , m_outputBuffer(outputBuffer)
    , m_playbackTime(playbackTime)
{
}

AudioProcessingEvent::~AudioProcessingEvent()
{
}

const AtomicString& AudioProcessingEvent::interfaceName() const
{
    return EventNames::AudioProcessingEvent;
}

DEFINE_TRACE(AudioProcessingEvent)
{
    visitor->trace(m_inputBuffer);
    visitor->trace(m_outputBuffer);
    Event::trace(visitor);
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
