/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

#include "config.h"

#if ENABLE(WEB_AUDIO)

#include "platform/audio/AudioDSPKernelProcessor.h"

#include "platform/audio/AudioDSPKernel.h"
#include "wtf/MainThread.h"

namespace blink {

// setNumberOfChannels() may later be called if the object is not yet in an "initialized" state.
AudioDSPKernelProcessor::AudioDSPKernelProcessor(float sampleRate, unsigned numberOfChannels)
    : AudioProcessor(sampleRate, numberOfChannels)
    , m_hasJustReset(true)
{
}

void AudioDSPKernelProcessor::initialize()
{
    if (isInitialized())
        return;

    MutexLocker locker(m_processLock);
    ASSERT(!m_kernels.size());

    // Create processing kernels, one per channel.
    for (unsigned i = 0; i < numberOfChannels(); ++i)
        m_kernels.append(createKernel());

    m_initialized = true;
    m_hasJustReset = true;
}

void AudioDSPKernelProcessor::uninitialize()
{
    if (!isInitialized())
        return;

    MutexLocker locker(m_processLock);
    m_kernels.clear();

    m_initialized = false;
}

void AudioDSPKernelProcessor::process(const AudioBus* source, AudioBus* destination, size_t framesToProcess)
{
    ASSERT(source && destination);
    if (!source || !destination)
        return;

    if (!isInitialized()) {
        destination->zero();
        return;
    }

    MutexTryLocker tryLocker(m_processLock);
    if (tryLocker.locked()) {
        bool channelCountMatches = source->numberOfChannels() == destination->numberOfChannels() && source->numberOfChannels() == m_kernels.size();
        ASSERT(channelCountMatches);
        if (!channelCountMatches)
            return;

        for (unsigned i = 0; i < m_kernels.size(); ++i)
            m_kernels[i]->process(source->channel(i)->data(), destination->channel(i)->mutableData(), framesToProcess);
    } else {
        // Unfortunately, the kernel is being processed by another thread.
        // See also ConvolverNode::process().
        destination->zero();
    }
}

// Resets filter state
void AudioDSPKernelProcessor::reset()
{
    ASSERT(isMainThread());
    if (!isInitialized())
        return;

    // Forces snap to parameter values - first time.
    // Any processing depending on this value must set it to false at the appropriate time.
    m_hasJustReset = true;

    MutexLocker locker(m_processLock);
    for (unsigned i = 0; i < m_kernels.size(); ++i)
        m_kernels[i]->reset();
}

void AudioDSPKernelProcessor::setNumberOfChannels(unsigned numberOfChannels)
{
    if (numberOfChannels == m_numberOfChannels)
        return;

    ASSERT(!isInitialized());
    if (!isInitialized())
        m_numberOfChannels = numberOfChannels;
}

double AudioDSPKernelProcessor::tailTime() const
{
    ASSERT(!isMainThread());
    MutexTryLocker tryLocker(m_processLock);
    if (tryLocker.locked()) {
        // It is expected that all the kernels have the same tailTime.
        return !m_kernels.isEmpty() ? m_kernels.first()->tailTime() : 0;
    }
    // Since we don't want to block the Audio Device thread, we return a large value
    // instead of trying to acquire the lock.
    return std::numeric_limits<double>::infinity();
}

double AudioDSPKernelProcessor::latencyTime() const
{
    ASSERT(!isMainThread());
    MutexTryLocker tryLocker(m_processLock);
    if (tryLocker.locked()) {
        // It is expected that all the kernels have the same latencyTime.
        return !m_kernels.isEmpty() ? m_kernels.first()->latencyTime() : 0;
    }
    // Since we don't want to block the Audio Device thread, we return a large value
    // instead of trying to acquire the lock.
    return std::numeric_limits<double>::infinity();
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
