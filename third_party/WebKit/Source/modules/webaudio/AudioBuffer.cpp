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

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/AudioBuffer.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "modules/webaudio/AudioContext.h"
#include "platform/audio/AudioBus.h"
#include "platform/audio/AudioFileReader.h"
#include "platform/audio/AudioUtilities.h"
#include "wtf/Float32Array.h"

namespace blink {

AudioBuffer* AudioBuffer::create(unsigned numberOfChannels, size_t numberOfFrames, float sampleRate)
{
    if (!AudioUtilities::isValidAudioBufferSampleRate(sampleRate) || numberOfChannels > AudioContext::maxNumberOfChannels() || !numberOfChannels || !numberOfFrames)
        return nullptr;

    AudioBuffer* buffer = new AudioBuffer(numberOfChannels, numberOfFrames, sampleRate);

    if (!buffer->createdSuccessfully(numberOfChannels))
        return nullptr;
    return buffer;
}

AudioBuffer* AudioBuffer::create(unsigned numberOfChannels, size_t numberOfFrames, float sampleRate, ExceptionState& exceptionState)
{
    if (!numberOfChannels || numberOfChannels > AudioContext::maxNumberOfChannels()) {
        exceptionState.throwDOMException(
            NotSupportedError,
            ExceptionMessages::indexOutsideRange(
                "number of channels",
                numberOfChannels,
                1u,
                ExceptionMessages::InclusiveBound,
                AudioContext::maxNumberOfChannels(),
                ExceptionMessages::InclusiveBound));
        return nullptr;
    }

    if (!AudioUtilities::isValidAudioBufferSampleRate(sampleRate)) {
        exceptionState.throwDOMException(
            NotSupportedError,
            ExceptionMessages::indexOutsideRange(
                "sample rate",
                sampleRate,
                AudioUtilities::minAudioBufferSampleRate(),
                ExceptionMessages::InclusiveBound,
                AudioUtilities::maxAudioBufferSampleRate(),
                ExceptionMessages::InclusiveBound));
        return nullptr;
    }

    if (!numberOfFrames) {
        exceptionState.throwDOMException(
            NotSupportedError,
            ExceptionMessages::indexExceedsMinimumBound(
                "number of frames",
                numberOfFrames,
                static_cast<size_t>(0)));
        return nullptr;
    }

    AudioBuffer* audioBuffer = create(numberOfChannels, numberOfFrames, sampleRate);

    if (!audioBuffer) {
        exceptionState.throwDOMException(
            NotSupportedError,
            "createBuffer("
            + String::number(numberOfChannels) + ", "
            + String::number(numberOfFrames) + ", "
            + String::number(sampleRate)
            + ") failed.");
    }

    return audioBuffer;
}

AudioBuffer* AudioBuffer::createFromAudioFileData(const void* data, size_t dataSize, bool mixToMono, float sampleRate)
{
    RefPtr<AudioBus> bus = createBusFromInMemoryAudioFile(data, dataSize, mixToMono, sampleRate);
    if (bus) {
        AudioBuffer* buffer = new AudioBuffer(bus.get());
        if (buffer->createdSuccessfully(bus->numberOfChannels()))
            return buffer;
    }

    return nullptr;
}

AudioBuffer* AudioBuffer::createFromAudioBus(AudioBus* bus)
{
    if (!bus)
        return nullptr;
    AudioBuffer* buffer = new AudioBuffer(bus);
    if (buffer->createdSuccessfully(bus->numberOfChannels()))
        return buffer;
    return nullptr;
}

bool AudioBuffer::createdSuccessfully(unsigned desiredNumberOfChannels) const
{
    return numberOfChannels() == desiredNumberOfChannels;
}

PassRefPtr<DOMFloat32Array> AudioBuffer::createFloat32ArrayOrNull(size_t length)
{
    RefPtr<WTF::Float32Array> bufferView = WTF::Float32Array::createOrNull(length);
    if (!bufferView)
        return nullptr;
    return DOMFloat32Array::create(bufferView.release());
}

AudioBuffer::AudioBuffer(unsigned numberOfChannels, size_t numberOfFrames, float sampleRate)
    : m_sampleRate(sampleRate)
    , m_length(numberOfFrames)
{
    m_channels.reserveCapacity(numberOfChannels);

    for (unsigned i = 0; i < numberOfChannels; ++i) {
        RefPtr<DOMFloat32Array> channelDataArray = createFloat32ArrayOrNull(m_length);
        // If the channel data array could not be created, just return. The caller will need to
        // check that the desired number of channels were created.
        if (!channelDataArray) {
            return;
        }

        channelDataArray->setNeuterable(false);
        m_channels.append(channelDataArray);
    }
}

AudioBuffer::AudioBuffer(AudioBus* bus)
    : m_sampleRate(bus->sampleRate())
    , m_length(bus->length())
{
    // Copy audio data from the bus to the Float32Arrays we manage.
    unsigned numberOfChannels = bus->numberOfChannels();
    m_channels.reserveCapacity(numberOfChannels);
    for (unsigned i = 0; i < numberOfChannels; ++i) {
        RefPtr<DOMFloat32Array> channelDataArray = createFloat32ArrayOrNull(m_length);
        // If the channel data array could not be created, just return. The caller will need to
        // check that the desired number of channels were created.
        if (!channelDataArray)
            return;

        channelDataArray->setNeuterable(false);
        const float* src = bus->channel(i)->data();
        float* dst = channelDataArray->data();
        memmove(dst, src, m_length * sizeof(*dst));
        m_channels.append(channelDataArray);
    }
}

PassRefPtr<DOMFloat32Array> AudioBuffer::getChannelData(unsigned channelIndex, ExceptionState& exceptionState)
{
    if (channelIndex >= m_channels.size()) {
        exceptionState.throwDOMException(IndexSizeError, "channel index (" + String::number(channelIndex) + ") exceeds number of channels (" + String::number(m_channels.size()) + ")");
        return nullptr;
    }

    DOMFloat32Array* channelData = m_channels[channelIndex].get();
    return DOMFloat32Array::create(channelData->buffer(), channelData->byteOffset(), channelData->length());
}

DOMFloat32Array* AudioBuffer::getChannelData(unsigned channelIndex)
{
    if (channelIndex >= m_channels.size())
        return nullptr;

    return m_channels[channelIndex].get();
}

void AudioBuffer::copyFromChannel(DOMFloat32Array* destination, long channelNumber, ExceptionState& exceptionState)
{
    return copyFromChannel(destination, channelNumber, 0, exceptionState);
}

void AudioBuffer::copyFromChannel(DOMFloat32Array* destination, long channelNumber, unsigned long startInChannel, ExceptionState& exceptionState)
{
    if (!destination) {
        exceptionState.throwDOMException(
            TypeMismatchError,
            ExceptionMessages::argumentNullOrIncorrectType(
                1,
                "Float32Array"));
        return;
    }

    if (channelNumber < 0 || channelNumber >= static_cast<long>(m_channels.size())) {
        exceptionState.throwDOMException(
            IndexSizeError,
            ExceptionMessages::indexOutsideRange(
                "channelNumber",
                channelNumber,
                0L,
                ExceptionMessages::InclusiveBound,
                static_cast<long>(m_channels.size() - 1),
                ExceptionMessages::InclusiveBound));
        return;
    }

    DOMFloat32Array* channelData = m_channels[channelNumber].get();

    if (startInChannel >= channelData->length()) {
        exceptionState.throwDOMException(
            IndexSizeError,
            ExceptionMessages::indexOutsideRange(
                "startInChannel",
                startInChannel,
                0UL,
                ExceptionMessages::InclusiveBound,
                static_cast<unsigned long>(channelData->length()),
                ExceptionMessages::ExclusiveBound));

        return;
    }

    unsigned count = channelData->length() - startInChannel;
    count = std::min(destination->length(), count);

    const float* src = channelData->data();
    float* dst = destination->data();

    ASSERT(src);
    ASSERT(dst);

    memcpy(dst, src + startInChannel, count * sizeof(*src));
}

void AudioBuffer::copyToChannel(DOMFloat32Array* source, long channelNumber, ExceptionState& exceptionState)
{
    return copyToChannel(source, channelNumber, 0, exceptionState);
}

void AudioBuffer::copyToChannel(DOMFloat32Array* source, long channelNumber, unsigned long startInChannel, ExceptionState& exceptionState)
{
    if (!source) {
        exceptionState.throwDOMException(
            TypeMismatchError,
            ExceptionMessages::argumentNullOrIncorrectType(
                1,
                "Float32Array"));
        return;
    }

    if (channelNumber < 0 || channelNumber >= static_cast<long>(m_channels.size())) {
        exceptionState.throwDOMException(
            IndexSizeError,
            ExceptionMessages::indexOutsideRange(
                "channelNumber",
                channelNumber,
                0L,
                ExceptionMessages::InclusiveBound,
                static_cast<long>(m_channels.size() - 1),
                ExceptionMessages::InclusiveBound));
        return;
    }

    DOMFloat32Array* channelData = m_channels[channelNumber].get();

    if (startInChannel >= channelData->length()) {
        exceptionState.throwDOMException(
            IndexSizeError,
            ExceptionMessages::indexOutsideRange(
                "startInChannel",
                startInChannel,
                0UL,
                ExceptionMessages::InclusiveBound,
                static_cast<unsigned long>(channelData->length()),
                ExceptionMessages::ExclusiveBound));

        return;
    }

    unsigned count = channelData->length() - startInChannel;
    count = std::min(source->length(), count);

    const float* src = source->data();
    float* dst = channelData->data();

    ASSERT(src);
    ASSERT(dst);

    memcpy(dst + startInChannel, src, count * sizeof(*dst));
}

void AudioBuffer::zero()
{
    for (unsigned i = 0; i < m_channels.size(); ++i) {
        if (DOMFloat32Array* array = getChannelData(i)) {
            float* data = array->data();
            memset(data, 0, length() * sizeof(*data));
        }
    }
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
