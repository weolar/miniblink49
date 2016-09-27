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

#ifndef MediaElementAudioSourceNode_h
#define MediaElementAudioSourceNode_h

#if ENABLE(WEB_AUDIO)

#include "modules/webaudio/AudioSourceNode.h"
#include "platform/audio/AudioSourceProviderClient.h"
#include "platform/audio/MultiChannelResampler.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

class AudioContext;
class HTMLMediaElement;

class MediaElementAudioSourceHandler final : public AudioHandler {
public:
    static PassRefPtr<MediaElementAudioSourceHandler> create(AudioNode&, HTMLMediaElement&);
    ~MediaElementAudioSourceHandler() override;

    HTMLMediaElement* mediaElement() { return m_mediaElement.get(); }

    // AudioHandler
    void dispose() override;
    void process(size_t framesToProcess) override;

    // Helpers for AudioSourceProviderClient implementation of
    // MediaElementAudioSourceNode.
    void setFormat(size_t numberOfChannels, float sampleRate);
    void onCurrentSrcChanged(const KURL& currentSrc);
    void lock();
    void unlock();

private:
    MediaElementAudioSourceHandler(AudioNode&, HTMLMediaElement&);
    // As an audio source, we will never propagate silence.
    bool propagatesSilence() const override { return false; }

    // Must be called only on the audio thread.
    bool passesCORSAccessCheck();

    // Must be called only on the main thread.
    bool passesCurrentSrcCORSAccessCheck(const KURL& currentSrc);

    // Print warning if CORS restrictions cause MediaElementAudioSource to output zeroes.
    void printCORSMessage(const String& message);

    // This Persistent doesn't make a reference cycle. The reference from
    // HTMLMediaElement to AudioSourceProvideClient, which
    // MediaElementAudioSourceNode implements, is weak.
    RefPtrWillBePersistent<HTMLMediaElement> m_mediaElement;
    Mutex m_processLock;

    unsigned m_sourceNumberOfChannels;
    double m_sourceSampleRate;

    OwnPtr<MultiChannelResampler> m_multiChannelResampler;

    // |m_passesCurrentSrcCORSAccessCheck| holds the value of
    // context()->securityOrigin() && context()->securityOrigin()->canRequest(mediaElement()->currentSrc()),
    // updated in the ctor and onCurrentSrcChanged() on the main thread and
    // used in passesCORSAccessCheck() on the audio thread,
    // protected by |m_processLock|.
    bool m_passesCurrentSrcCORSAccessCheck;

    // Indicates if we need to print a CORS message if the current source has changed and we have no
    // access to it. Must be protected by |m_processLock|.
    bool m_maybePrintCORSMessage;

    // The value of mediaElement()->currentSrc().string() in the ctor and onCurrentSrcChanged().
    // Protected by |m_processLock|.
    String m_currentSrcString;
};

class MediaElementAudioSourceNode final : public AudioSourceNode, public AudioSourceProviderClient {
    DEFINE_WRAPPERTYPEINFO();
    USING_GARBAGE_COLLECTED_MIXIN(MediaElementAudioSourceNode);
public:
    static MediaElementAudioSourceNode* create(AudioContext&, HTMLMediaElement&);
    DECLARE_VIRTUAL_TRACE();
    MediaElementAudioSourceHandler& mediaElementAudioSourceHandler() const;

    HTMLMediaElement* mediaElement() const;

    // AudioSourceProviderClient functions:
    void setFormat(size_t numberOfChannels, float sampleRate) override;
    void onCurrentSrcChanged(const KURL& currentSrc) override;
    void lock() override;
    void unlock() override;

private:
    MediaElementAudioSourceNode(AudioContext&, HTMLMediaElement&);
};

} // namespace blink

#endif // ENABLE(WEB_AUDIO)

#endif // MediaElementAudioSourceNode_h
