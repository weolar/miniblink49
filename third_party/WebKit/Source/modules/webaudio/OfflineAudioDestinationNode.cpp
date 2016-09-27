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

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/OfflineAudioDestinationNode.h"

#include "core/dom/CrossThreadTask.h"
#include "modules/webaudio/AudioContext.h"
#include "platform/Task.h"
#include "platform/audio/AudioBus.h"
#include "platform/audio/HRTFDatabaseLoader.h"
#include "public/platform/Platform.h"
#include <algorithm>

namespace blink {

const size_t renderQuantumSize = 128;

OfflineAudioDestinationHandler::OfflineAudioDestinationHandler(AudioNode& node, AudioBuffer* renderTarget)
    : AudioDestinationHandler(node, renderTarget->sampleRate())
    , m_renderTarget(renderTarget)
    , m_startedRendering(false)
{
    m_renderBus = AudioBus::create(renderTarget->numberOfChannels(), renderQuantumSize);
}

PassRefPtr<OfflineAudioDestinationHandler> OfflineAudioDestinationHandler::create(AudioNode& node, AudioBuffer* renderTarget)
{
    return adoptRef(new OfflineAudioDestinationHandler(node, renderTarget));
}

OfflineAudioDestinationHandler::~OfflineAudioDestinationHandler()
{
    ASSERT(!isInitialized());
}

void OfflineAudioDestinationHandler::dispose()
{
    uninitialize();
    AudioDestinationHandler::dispose();
}

void OfflineAudioDestinationHandler::initialize()
{
    if (isInitialized())
        return;

    AudioHandler::initialize();
}

void OfflineAudioDestinationHandler::uninitialize()
{
    if (!isInitialized())
        return;

    if (m_renderThread)
        m_renderThread.clear();

    AudioHandler::uninitialize();
}

void OfflineAudioDestinationHandler::startRendering()
{
    ASSERT(isMainThread());
    ASSERT(m_renderTarget);
    if (!m_renderTarget)
        return;

    if (!m_startedRendering) {
        m_startedRendering = true;
        m_renderThread = adoptPtr(Platform::current()->createThread("Offline Audio Renderer"));
        m_renderThread->postTask(FROM_HERE, new Task(threadSafeBind(&OfflineAudioDestinationHandler::offlineRender, PassRefPtr<OfflineAudioDestinationHandler>(this))));
    }
}

void OfflineAudioDestinationHandler::stopRendering()
{
    ASSERT_NOT_REACHED();
}

void OfflineAudioDestinationHandler::offlineRender()
{
    offlineRenderInternal();
    context()->handlePostRenderTasks();
}

void OfflineAudioDestinationHandler::offlineRenderInternal()
{
    ASSERT(!isMainThread());
    ASSERT(m_renderBus);
    if (!m_renderBus)
        return;

    bool isAudioContextInitialized = context()->isInitialized();
    ASSERT(isAudioContextInitialized);
    if (!isAudioContextInitialized)
        return;

    bool channelsMatch = m_renderBus->numberOfChannels() == m_renderTarget->numberOfChannels();
    ASSERT(channelsMatch);
    if (!channelsMatch)
        return;

    bool isRenderBusAllocated = m_renderBus->length() >= renderQuantumSize;
    ASSERT(isRenderBusAllocated);
    if (!isRenderBusAllocated)
        return;

    // Break up the render target into smaller "render quantize" sized pieces.
    // Render until we're finished.
    size_t framesToProcess = m_renderTarget->length();
    unsigned numberOfChannels = m_renderTarget->numberOfChannels();

    unsigned n = 0;
    while (framesToProcess > 0) {
        // Render one render quantum.
        render(0, m_renderBus.get(), renderQuantumSize);

        size_t framesAvailableToCopy = std::min(framesToProcess, renderQuantumSize);

        for (unsigned channelIndex = 0; channelIndex < numberOfChannels; ++channelIndex) {
            const float* source = m_renderBus->channel(channelIndex)->data();
            float* destination = m_renderTarget->getChannelData(channelIndex)->data();
            memcpy(destination + n, source, sizeof(float) * framesAvailableToCopy);
        }

        n += framesAvailableToCopy;
        framesToProcess -= framesAvailableToCopy;
    }

    // Our work is done. Let the AudioContext know.
    if (context()->executionContext())
        context()->executionContext()->postTask(FROM_HERE, createCrossThreadTask(&OfflineAudioDestinationHandler::notifyComplete, PassRefPtr<OfflineAudioDestinationHandler>(this)));
}

void OfflineAudioDestinationHandler::notifyComplete()
{
    // The AudioContext might be gone.
    if (context())
        context()->fireCompletionEvent();
}

// ----------------------------------------------------------------

OfflineAudioDestinationNode::OfflineAudioDestinationNode(AudioContext& context, AudioBuffer* renderTarget)
    : AudioDestinationNode(context)
{
    setHandler(OfflineAudioDestinationHandler::create(*this, renderTarget));
}

OfflineAudioDestinationNode* OfflineAudioDestinationNode::create(AudioContext* context, AudioBuffer* renderTarget)
{
    return new OfflineAudioDestinationNode(*context, renderTarget);
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
