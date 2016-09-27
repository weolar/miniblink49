/*
 * Copyright 2014 Google Inc. All rights reserved.
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

#ifndef MediaStreamWebAudioSource_h
#define MediaStreamWebAudioSource_h

#include "platform/audio/AudioSourceProvider.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

class WebAudioSourceProvider;

class MediaStreamWebAudioSource : public AudioSourceProvider {
    WTF_MAKE_NONCOPYABLE(MediaStreamWebAudioSource);
public:
    static PassOwnPtr<MediaStreamWebAudioSource> create(PassOwnPtr<WebAudioSourceProvider> provider) { return adoptPtr(new MediaStreamWebAudioSource(provider)); }

    ~MediaStreamWebAudioSource() override;

private:
    explicit MediaStreamWebAudioSource(PassOwnPtr<WebAudioSourceProvider>);

    // blink::AudioSourceProvider implementation.
    void provideInput(AudioBus*, size_t framesToProcess) override;

    OwnPtr<WebAudioSourceProvider> m_webAudioSourceProvider;
};

} // namespace blink

#endif // MediaStreamWebAudioSource_h
