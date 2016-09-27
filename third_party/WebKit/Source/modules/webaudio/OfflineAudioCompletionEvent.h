/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
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

#ifndef OfflineAudioCompletionEvent_h
#define OfflineAudioCompletionEvent_h

#include "modules/EventModules.h"
#include "modules/webaudio/AudioBuffer.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class AudioBuffer;

class OfflineAudioCompletionEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<OfflineAudioCompletionEvent> create();
    static PassRefPtrWillBeRawPtr<OfflineAudioCompletionEvent> create(AudioBuffer* renderedBuffer);

    ~OfflineAudioCompletionEvent() override;

    AudioBuffer* renderedBuffer() { return m_renderedBuffer.get(); }

    const AtomicString& interfaceName() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    OfflineAudioCompletionEvent();
    explicit OfflineAudioCompletionEvent(AudioBuffer* renderedBuffer);

    PersistentWillBeMember<AudioBuffer> m_renderedBuffer;
};

} // namespace blink

#endif // OfflineAudioCompletionEvent_h
