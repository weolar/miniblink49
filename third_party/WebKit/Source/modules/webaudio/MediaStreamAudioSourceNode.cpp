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

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/MediaStreamAudioSourceNode.h"

#include "modules/webaudio/AudioContext.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "platform/Logging.h"
#include "wtf/Locker.h"

namespace blink {

MediaStreamAudioSourceHandler::MediaStreamAudioSourceHandler(AudioNode& node, MediaStream& mediaStream, MediaStreamTrack* audioTrack, PassOwnPtr<AudioSourceProvider> audioSourceProvider)
    : AudioHandler(NodeTypeMediaStreamAudioSource, node, node.context()->sampleRate())
    , m_mediaStream(mediaStream)
    , m_audioTrack(audioTrack)
    , m_audioSourceProvider(audioSourceProvider)
    , m_sourceNumberOfChannels(0)
{
    // Default to stereo. This could change depending on the format of the
    // MediaStream's audio track.
    addOutput(2);

    initialize();
}

PassRefPtr<MediaStreamAudioSourceHandler> MediaStreamAudioSourceHandler::create(AudioNode& node, MediaStream& mediaStream, MediaStreamTrack* audioTrack, PassOwnPtr<AudioSourceProvider> audioSourceProvider)
{
    return adoptRef(new MediaStreamAudioSourceHandler(node, mediaStream, audioTrack, audioSourceProvider));
}

MediaStreamAudioSourceHandler::~MediaStreamAudioSourceHandler()
{
    uninitialize();
}

void MediaStreamAudioSourceHandler::setFormat(size_t numberOfChannels, float sourceSampleRate)
{
    if (numberOfChannels != m_sourceNumberOfChannels || sourceSampleRate != sampleRate()) {
        // The sample-rate must be equal to the context's sample-rate.
        if (!numberOfChannels || numberOfChannels > AudioContext::maxNumberOfChannels() || sourceSampleRate != sampleRate()) {
            // process() will generate silence for these uninitialized values.
            WTF_LOG(Media, "MediaStreamAudioSourceNode::setFormat(%u, %f) - unhandled format change", static_cast<unsigned>(numberOfChannels), sourceSampleRate);
            m_sourceNumberOfChannels = 0;
            return;
        }

        // Synchronize with process().
        MutexLocker locker(m_processLock);

        m_sourceNumberOfChannels = numberOfChannels;

        {
            // The context must be locked when changing the number of output channels.
            AudioContext::AutoLocker contextLocker(context());

            // Do any necesssary re-configuration to the output's number of channels.
            output(0).setNumberOfChannels(numberOfChannels);
        }
    }
}

void MediaStreamAudioSourceHandler::process(size_t numberOfFrames)
{
    AudioBus* outputBus = output(0).bus();

    if (!audioSourceProvider()) {
        outputBus->zero();
        return;
    }

    if (!mediaStream() || m_sourceNumberOfChannels != outputBus->numberOfChannels()) {
        outputBus->zero();
        return;
    }

    // Use a tryLock() to avoid contention in the real-time audio thread.
    // If we fail to acquire the lock then the MediaStream must be in the middle of
    // a format change, so we output silence in this case.
    MutexTryLocker tryLocker(m_processLock);
    if (tryLocker.locked()) {
        audioSourceProvider()->provideInput(outputBus, numberOfFrames);
    } else {
        // We failed to acquire the lock.
        outputBus->zero();
    }
}

// ----------------------------------------------------------------

MediaStreamAudioSourceNode::MediaStreamAudioSourceNode(AudioContext& context, MediaStream& mediaStream, MediaStreamTrack* audioTrack, PassOwnPtr<AudioSourceProvider> audioSourceProvider)
    : AudioSourceNode(context)
{
    setHandler(MediaStreamAudioSourceHandler::create(*this, mediaStream, audioTrack, audioSourceProvider));
}

MediaStreamAudioSourceNode* MediaStreamAudioSourceNode::create(AudioContext& context, MediaStream& mediaStream, MediaStreamTrack* audioTrack, PassOwnPtr<AudioSourceProvider> audioSourceProvider)
{
    return new MediaStreamAudioSourceNode(context, mediaStream, audioTrack, audioSourceProvider);
}

DEFINE_TRACE(MediaStreamAudioSourceNode)
{
    AudioSourceProviderClient::trace(visitor);
    AudioSourceNode::trace(visitor);
}

MediaStreamAudioSourceHandler& MediaStreamAudioSourceNode::mediaStreamAudioSourceHandler() const
{
    return static_cast<MediaStreamAudioSourceHandler&>(handler());
}

MediaStream* MediaStreamAudioSourceNode::mediaStream() const
{
    return mediaStreamAudioSourceHandler().mediaStream();
}

void MediaStreamAudioSourceNode::setFormat(size_t numberOfChannels, float sourceSampleRate)
{
    mediaStreamAudioSourceHandler().setFormat(numberOfChannels, sourceSampleRate);
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
