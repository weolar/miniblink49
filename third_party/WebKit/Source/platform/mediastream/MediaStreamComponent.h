/*
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef MediaStreamComponent_h
#define MediaStreamComponent_h

#include "platform/audio/AudioSourceProvider.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/ThreadingPrimitives.h"
#include "wtf/text/WTFString.h"

namespace blink {

class MediaStreamSource;
class WebAudioSourceProvider;

class PLATFORM_EXPORT MediaStreamComponent final : public RefCounted<MediaStreamComponent> {
public:
    class ExtraData {
    public:
        virtual ~ExtraData() { }
    };

    static PassRefPtr<MediaStreamComponent> create(PassRefPtr<MediaStreamSource>);
    static PassRefPtr<MediaStreamComponent> create(const String& id, PassRefPtr<MediaStreamSource>);

    MediaStreamSource* source() const { return m_source.get(); }

    String id() const { return m_id; }
    bool enabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool muted() const { return m_muted; }
    void setMuted(bool muted) { m_muted = muted; }

#if ENABLE(WEB_AUDIO)
    AudioSourceProvider* audioSourceProvider() { return &m_sourceProvider; }
    void setSourceProvider(WebAudioSourceProvider* provider) { m_sourceProvider.wrap(provider); }
#endif // ENABLE(WEB_AUDIO)

    ExtraData* extraData() const { return m_extraData.get(); }
    void setExtraData(PassOwnPtr<ExtraData> extraData) { m_extraData = extraData; }

private:
    MediaStreamComponent(const String& id, PassRefPtr<MediaStreamSource>);

#if ENABLE(WEB_AUDIO)
    // AudioSourceProviderImpl wraps a WebAudioSourceProvider::provideInput()
    // calls into chromium to get a rendered audio stream.

    class PLATFORM_EXPORT AudioSourceProviderImpl final: public AudioSourceProvider {
    public:
        AudioSourceProviderImpl()
            : m_webAudioSourceProvider(0)
        {
        }

        ~AudioSourceProviderImpl() override {}

        // Wraps the given blink::WebAudioSourceProvider to blink::AudioSourceProvider.
        void wrap(WebAudioSourceProvider*);

        // blink::AudioSourceProvider
        void provideInput(AudioBus*, size_t framesToProcess) override;

    private:
        WebAudioSourceProvider* m_webAudioSourceProvider;
        Mutex m_provideInputLock;
    };

    AudioSourceProviderImpl m_sourceProvider;
#endif // ENABLE(WEB_AUDIO)

    RefPtr<MediaStreamSource> m_source;
    String m_id;
    bool m_enabled;
    bool m_muted;
    OwnPtr<ExtraData> m_extraData;
};

typedef Vector<RefPtr<MediaStreamComponent>> MediaStreamComponentVector;

} // namespace blink

#endif // MediaStreamComponent_h
