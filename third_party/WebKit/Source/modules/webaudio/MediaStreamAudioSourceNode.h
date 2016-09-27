/*
 * Copyright (C) 2012, Google Inc. All rights reserved.
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

#ifndef MediaStreamAudioSourceNode_h
#define MediaStreamAudioSourceNode_h

#if ENABLE(WEB_AUDIO)

#include "modules/mediastream/MediaStream.h"
#include "modules/webaudio/AudioSourceNode.h"
#include "platform/audio/AudioSourceProvider.h"
#include "platform/audio/AudioSourceProviderClient.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/Threading.h"

namespace blink {

class AudioContext;

class MediaStreamAudioSourceHandler final : public AudioHandler {
public:
    static PassRefPtr<MediaStreamAudioSourceHandler> create(AudioNode&, MediaStream&, MediaStreamTrack*, PassOwnPtr<AudioSourceProvider>);
    ~MediaStreamAudioSourceHandler() override;

    MediaStream* mediaStream() { return m_mediaStream.get(); }

    // AudioHandler
    void process(size_t framesToProcess) override;

    // A helper for AudioSourceProviderClient implementation of
    // MediaStreamAudioSourceNode.
    void setFormat(size_t numberOfChannels, float sampleRate);

    AudioSourceProvider* audioSourceProvider() const { return m_audioSourceProvider.get(); }

private:
    MediaStreamAudioSourceHandler(AudioNode&, MediaStream&, MediaStreamTrack*, PassOwnPtr<AudioSourceProvider>);
    // As an audio source, we will never propagate silence.
    bool propagatesSilence() const override { return false; }

    // These Persistents don't make reference cycles including the owner
    // MediaStreamAudioSourceNode.
    Persistent<MediaStream> m_mediaStream;
    Persistent<MediaStreamTrack> m_audioTrack;
    OwnPtr<AudioSourceProvider> m_audioSourceProvider;

    Mutex m_processLock;

    unsigned m_sourceNumberOfChannels;
};

class MediaStreamAudioSourceNode final : public AudioSourceNode, public AudioSourceProviderClient {
    DEFINE_WRAPPERTYPEINFO();
    USING_GARBAGE_COLLECTED_MIXIN(MediaStreamAudioSourceNode);
public:
    static MediaStreamAudioSourceNode* create(AudioContext&, MediaStream&, MediaStreamTrack*, PassOwnPtr<AudioSourceProvider>);
    DECLARE_VIRTUAL_TRACE();
    MediaStreamAudioSourceHandler& mediaStreamAudioSourceHandler() const;

    MediaStream* mediaStream() const;

    // AudioSourceProviderClient functions:
    void setFormat(size_t numberOfChannels, float sampleRate) override;

private:
    MediaStreamAudioSourceNode(AudioContext&, MediaStream&, MediaStreamTrack*, PassOwnPtr<AudioSourceProvider>);
};

} // namespace blink

#endif // ENABLE(WEB_AUDIO)

#endif // MediaStreamAudioSourceNode_h
