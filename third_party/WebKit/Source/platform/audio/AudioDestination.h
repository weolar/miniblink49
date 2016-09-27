/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AudioDestination_h
#define AudioDestination_h

#include "platform/audio/AudioBus.h"
#include "platform/audio/AudioIOCallback.h"
#include "platform/audio/AudioSourceProvider.h"
#include "public/platform/WebAudioDevice.h"
#include "public/platform/WebVector.h"
#include "wtf/Noncopyable.h"
#include "wtf/text/WTFString.h"

namespace blink {

class AudioFIFO;
class AudioPullFIFO;

// An AudioDestination using Chromium's audio system

class PLATFORM_EXPORT AudioDestination : public WebAudioDevice::RenderCallback, public AudioSourceProvider {
    WTF_MAKE_NONCOPYABLE(AudioDestination);
public:
    AudioDestination(AudioIOCallback&, const String& inputDeviceId, unsigned numberOfInputChannels, unsigned numberOfOutputChannels, float sampleRate);
    ~AudioDestination() override;

    // Pass in (numberOfInputChannels > 0) if live/local audio input is desired.
    // Port-specific device identification information for live/local input streams can be passed in the inputDeviceId.
    static PassOwnPtr<AudioDestination> create(AudioIOCallback&, const String& inputDeviceId, unsigned numberOfInputChannels, unsigned numberOfOutputChannels, float sampleRate);

    virtual void start();
    virtual void stop();
    bool isPlaying() { return m_isPlaying; }

    float sampleRate() const { return m_sampleRate; }

    // WebAudioDevice::RenderCallback
    void render(const WebVector<float*>& sourceData, const WebVector<float*>& audioData, size_t numberOfFrames) override;

    // AudioSourceProvider
    void provideInput(AudioBus*, size_t framesToProcess) override;

    static float hardwareSampleRate();

    // maxChannelCount() returns the total number of output channels of the audio hardware.
    // A value of 0 indicates that the number of channels cannot be configured and
    // that only stereo (2-channel) destinations can be created.
    // The numberOfOutputChannels parameter of AudioDestination::create() is allowed to
    // be a value: 1 <= numberOfOutputChannels <= maxChannelCount(),
    // or if maxChannelCount() equals 0, then numberOfOutputChannels must be 2.
    static unsigned long maxChannelCount();

private:
    AudioIOCallback& m_callback;
    unsigned m_numberOfOutputChannels;
    RefPtr<AudioBus> m_inputBus;
    RefPtr<AudioBus> m_renderBus;
    float m_sampleRate;
    bool m_isPlaying;
    OwnPtr<WebAudioDevice> m_audioDevice;
    size_t m_callbackBufferSize;

    OwnPtr<AudioFIFO> m_inputFifo;
    OwnPtr<AudioPullFIFO> m_fifo;
};

} // namespace blink

#endif // AudioDestination_h
