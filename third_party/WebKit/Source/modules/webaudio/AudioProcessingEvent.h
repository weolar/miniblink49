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

#ifndef AudioProcessingEvent_h
#define AudioProcessingEvent_h

#include "modules/EventModules.h"
#include "modules/webaudio/AudioBuffer.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class AudioBuffer;

class AudioProcessingEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<AudioProcessingEvent> create();
    static PassRefPtrWillBeRawPtr<AudioProcessingEvent> create(AudioBuffer* inputBuffer, AudioBuffer* outputBuffer, double playbackTime);

    ~AudioProcessingEvent() override;

    AudioBuffer* inputBuffer() { return m_inputBuffer.get(); }
    AudioBuffer* outputBuffer() { return m_outputBuffer.get(); }
    double playbackTime() const { return m_playbackTime; }

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    AudioProcessingEvent();
    AudioProcessingEvent(AudioBuffer* inputBuffer, AudioBuffer* outputBuffer, double playbackTime);

    PersistentWillBeMember<AudioBuffer> m_inputBuffer;
    PersistentWillBeMember<AudioBuffer> m_outputBuffer;
    double m_playbackTime;
};

} // namespace blink

#endif // AudioProcessingEvent_h
