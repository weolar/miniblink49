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

#ifndef AudioDestinationNode_h
#define AudioDestinationNode_h

#include "modules/webaudio/AudioBuffer.h"
#include "modules/webaudio/AudioNode.h"
#include "platform/audio/AudioBus.h"
#include "platform/audio/AudioIOCallback.h"
#include "platform/audio/AudioSourceProvider.h"

namespace blink {

class AudioBus;
class AudioContext;

class AudioDestinationHandler : public AudioHandler, public AudioIOCallback {
public:
    AudioDestinationHandler(AudioNode&, float sampleRate);
    ~AudioDestinationHandler() override;

    // AudioHandler
    void process(size_t) final { } // we're pulled by hardware so this is never called

    // The audio hardware calls render() to get the next render quantum of audio into destinationBus.
    // It will optionally give us local/live audio input in sourceBus (if it's not 0).
    void render(AudioBus* sourceBus, AudioBus* destinationBus, size_t numberOfFrames) final;

    size_t currentSampleFrame() const { return acquireLoad(&m_currentSampleFrame); }
    double currentTime() const { return currentSampleFrame() / static_cast<double>(sampleRate()); }

    virtual unsigned long maxChannelCount() const { return 0; }

    virtual void startRendering() = 0;
    virtual void stopRendering() = 0;

protected:
    // LocalAudioInputProvider allows us to expose an AudioSourceProvider for local/live audio input.
    // If there is local/live audio input, we call set() with the audio input data every render quantum.
    class LocalAudioInputProvider final : public AudioSourceProvider {
    public:
        LocalAudioInputProvider()
            : m_sourceBus(AudioBus::create(2, ProcessingSizeInFrames)) // FIXME: handle non-stereo local input.
        {
        }

        void set(AudioBus* bus)
        {
            if (bus)
                m_sourceBus->copyFrom(*bus);
        }

        // AudioSourceProvider.
        void provideInput(AudioBus* destinationBus, size_t numberOfFrames) override
        {
            bool isGood = destinationBus && destinationBus->length() == numberOfFrames && m_sourceBus->length() == numberOfFrames;
            ASSERT(isGood);
            if (isGood)
                destinationBus->copyFrom(*m_sourceBus);
        }

    private:
        RefPtr<AudioBus> m_sourceBus;
    };

    // Counts the number of sample-frames processed by the destination.
    size_t m_currentSampleFrame;

    LocalAudioInputProvider m_localAudioInputProvider;
};

class AudioDestinationNode : public AudioNode {
    DEFINE_WRAPPERTYPEINFO();
public:
    AudioDestinationHandler& audioDestinationHandler() const;

    unsigned long maxChannelCount() const;

protected:
    AudioDestinationNode(AudioContext&);
};

} // namespace blink

#endif // AudioDestinationNode_h
