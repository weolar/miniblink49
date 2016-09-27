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

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/DeferredTaskHandler.h"

#include "modules/webaudio/AudioContext.h"
#include "modules/webaudio/AudioNode.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "platform/ThreadSafeFunctional.h"
#include "public/platform/Platform.h"
#include "wtf/MainThread.h"

namespace blink {

void DeferredTaskHandler::lock()
{
    // Don't allow regular lock in real-time audio thread.
    ASSERT(isMainThread());
    m_contextGraphMutex.lock();
}

bool DeferredTaskHandler::tryLock()
{
    // Try to catch cases of using try lock on main thread
    // - it should use regular lock.
    ASSERT(isAudioThread());
    if (!isAudioThread()) {
        // In release build treat tryLock() as lock() (since above
        // ASSERT(isAudioThread) never fires) - this is the best we can do.
        lock();
        return true;
    }
    return m_contextGraphMutex.tryLock();
}

void DeferredTaskHandler::unlock()
{
    m_contextGraphMutex.unlock();
}

bool DeferredTaskHandler::isAudioThread() const
{
    return currentThread() == m_audioThread;
}

#if ENABLE(ASSERT)
bool DeferredTaskHandler::isGraphOwner()
{
    return m_contextGraphMutex.locked();
}
#endif

void DeferredTaskHandler::addDeferredBreakConnection(AudioHandler& node)
{
    ASSERT(isAudioThread());
    m_deferredBreakConnectionList.append(&node);
}

void DeferredTaskHandler::breakConnections()
{
    ASSERT(isAudioThread());
    ASSERT(isGraphOwner());

    for (unsigned i = 0; i < m_deferredBreakConnectionList.size(); ++i)
        m_deferredBreakConnectionList[i]->breakConnectionWithLock();
    m_deferredBreakConnectionList.clear();
}

void DeferredTaskHandler::markSummingJunctionDirty(AudioSummingJunction* summingJunction)
{
    ASSERT(isGraphOwner());
    m_dirtySummingJunctions.add(summingJunction);
}

void DeferredTaskHandler::removeMarkedSummingJunction(AudioSummingJunction* summingJunction)
{
    ASSERT(isMainThread());
    AutoLocker locker(*this);
    m_dirtySummingJunctions.remove(summingJunction);
}

void DeferredTaskHandler::markAudioNodeOutputDirty(AudioNodeOutput* output)
{
    ASSERT(isGraphOwner());
    ASSERT(isMainThread());
    m_dirtyAudioNodeOutputs.add(output);
}

void DeferredTaskHandler::removeMarkedAudioNodeOutput(AudioNodeOutput* output)
{
    ASSERT(isGraphOwner());
    ASSERT(isMainThread());
    m_dirtyAudioNodeOutputs.remove(output);
}

void DeferredTaskHandler::handleDirtyAudioSummingJunctions()
{
    ASSERT(isGraphOwner());

    for (AudioSummingJunction* junction : m_dirtySummingJunctions)
        junction->updateRenderingState();
    m_dirtySummingJunctions.clear();
}

void DeferredTaskHandler::handleDirtyAudioNodeOutputs()
{
    ASSERT(isGraphOwner());

    for (AudioNodeOutput* output : m_dirtyAudioNodeOutputs)
        output->updateRenderingState();
    m_dirtyAudioNodeOutputs.clear();
}

void DeferredTaskHandler::addAutomaticPullNode(AudioHandler* node)
{
    ASSERT(isGraphOwner());

    if (!m_automaticPullNodes.contains(node)) {
        m_automaticPullNodes.add(node);
        m_automaticPullNodesNeedUpdating = true;
    }
}

void DeferredTaskHandler::removeAutomaticPullNode(AudioHandler* node)
{
    ASSERT(isGraphOwner());

    if (m_automaticPullNodes.contains(node)) {
        m_automaticPullNodes.remove(node);
        m_automaticPullNodesNeedUpdating = true;
    }
}

void DeferredTaskHandler::updateAutomaticPullNodes()
{
    ASSERT(isGraphOwner());

    if (m_automaticPullNodesNeedUpdating) {
        copyToVector(m_automaticPullNodes, m_renderingAutomaticPullNodes);
        m_automaticPullNodesNeedUpdating = false;
    }
}

void DeferredTaskHandler::processAutomaticPullNodes(size_t framesToProcess)
{
    ASSERT(isAudioThread());

    for (unsigned i = 0; i < m_renderingAutomaticPullNodes.size(); ++i)
        m_renderingAutomaticPullNodes[i]->processIfNecessary(framesToProcess);
}

void DeferredTaskHandler::addChangedChannelCountMode(AudioHandler* node)
{
    ASSERT(isGraphOwner());
    ASSERT(isMainThread());
    m_deferredCountModeChange.add(node);
}

void DeferredTaskHandler::removeChangedChannelCountMode(AudioHandler* node)
{
    ASSERT(isGraphOwner());

    m_deferredCountModeChange.remove(node);
}

void DeferredTaskHandler::updateChangedChannelCountMode()
{
    ASSERT(isGraphOwner());

    for (AudioHandler* node : m_deferredCountModeChange)
        node->updateChannelCountMode();
    m_deferredCountModeChange.clear();
}

DeferredTaskHandler::DeferredTaskHandler()
    : m_automaticPullNodesNeedUpdating(false)
    , m_audioThread(0)
{
}

PassRefPtr<DeferredTaskHandler> DeferredTaskHandler::create()
{
    return adoptRef(new DeferredTaskHandler());
}

DeferredTaskHandler::~DeferredTaskHandler()
{
    ASSERT(!m_automaticPullNodes.size());
    if (m_automaticPullNodesNeedUpdating)
        m_renderingAutomaticPullNodes.resize(m_automaticPullNodes.size());
    ASSERT(!m_renderingAutomaticPullNodes.size());
}

void DeferredTaskHandler::handleDeferredTasks()
{
    updateChangedChannelCountMode();
    handleDirtyAudioSummingJunctions();
    handleDirtyAudioNodeOutputs();
    updateAutomaticPullNodes();
}

void DeferredTaskHandler::contextWillBeDestroyed()
{
    for (auto& handler : m_renderingOrphanHandlers)
        handler->clearContext();
    for (auto& handler : m_deletableOrphanHandlers)
        handler->clearContext();
    clearHandlersToBeDeleted();
    // Some handlers might live because of their cross thread tasks.
}

DeferredTaskHandler::AutoLocker::AutoLocker(AudioContext* context)
    : m_handler(context->deferredTaskHandler())
{
    m_handler.lock();
}

void DeferredTaskHandler::addRenderingOrphanHandler(PassRefPtr<AudioHandler> handler)
{
    ASSERT(handler);
    ASSERT(!m_renderingOrphanHandlers.contains(handler));
    m_renderingOrphanHandlers.append(handler);
}

void DeferredTaskHandler::requestToDeleteHandlersOnMainThread()
{
    ASSERT(isGraphOwner());
    ASSERT(isAudioThread());
    if (m_renderingOrphanHandlers.isEmpty())
        return;
    m_deletableOrphanHandlers.appendVector(m_renderingOrphanHandlers);
    m_renderingOrphanHandlers.clear();
    Platform::current()->mainThread()->postTask(FROM_HERE, threadSafeBind(&DeferredTaskHandler::deleteHandlersOnMainThread, PassRefPtr<DeferredTaskHandler>(this)));
}

void DeferredTaskHandler::deleteHandlersOnMainThread()
{
    ASSERT(isMainThread());
    AutoLocker locker(*this);
    m_deletableOrphanHandlers.clear();
}

void DeferredTaskHandler::clearHandlersToBeDeleted()
{
    ASSERT(isMainThread());
    AutoLocker locker(*this);
    m_renderingOrphanHandlers.clear();
    m_deletableOrphanHandlers.clear();
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
