// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/compositorworker/CompositorWorkerGlobalScope.h"

#include "bindings/core/v8/SerializedScriptValue.h"
#include "core/workers/WorkerObjectProxy.h"
#include "core/workers/WorkerThreadStartupData.h"
#include "modules/EventTargetModules.h"
#include "modules/compositorworker/CompositorWorkerThread.h"

namespace blink {

PassRefPtrWillBeRawPtr<CompositorWorkerGlobalScope> CompositorWorkerGlobalScope::create(CompositorWorkerThread* thread, PassOwnPtr<WorkerThreadStartupData> startupData, double timeOrigin)
{
    // Note: startupData is finalized on return. After the relevant parts has been
    // passed along to the created 'context'.
    RefPtrWillBeRawPtr<CompositorWorkerGlobalScope> context = adoptRefWillBeNoop(new CompositorWorkerGlobalScope(startupData->m_scriptURL, startupData->m_userAgent, thread, timeOrigin, startupData->m_starterOrigin, startupData->m_workerClients.release()));
    context->applyContentSecurityPolicyFromVector(*startupData->m_contentSecurityPolicyHeaders);
    return context.release();
}

CompositorWorkerGlobalScope::CompositorWorkerGlobalScope(const KURL& url, const String& userAgent, CompositorWorkerThread* thread, double timeOrigin, const SecurityOrigin* starterOrigin, PassOwnPtrWillBeRawPtr<WorkerClients> workerClients)
    : WorkerGlobalScope(url, userAgent, thread, timeOrigin, starterOrigin, workerClients)
    , m_callbackCollection(this)
{
}

CompositorWorkerGlobalScope::~CompositorWorkerGlobalScope()
{
}

DEFINE_TRACE(CompositorWorkerGlobalScope)
{
#if ENABLE(OILPAN)
    visitor->trace(m_callbackCollection);
#endif
    WorkerGlobalScope::trace(visitor);
}

const AtomicString& CompositorWorkerGlobalScope::interfaceName() const
{
    return EventTargetNames::CompositorWorkerGlobalScope;
}

void CompositorWorkerGlobalScope::postMessage(ExecutionContext* executionContext, PassRefPtr<SerializedScriptValue> message, const MessagePortArray* ports, ExceptionState& exceptionState)
{
    // Disentangle the port in preparation for sending it to the remote context.
    OwnPtr<MessagePortChannelArray> channels = MessagePort::disentanglePorts(executionContext, ports, exceptionState);
    if (exceptionState.hadException())
        return;
    thread()->workerObjectProxy().postMessageToWorkerObject(message, channels.release());
}

int CompositorWorkerGlobalScope::requestAnimationFrame(FrameRequestCallback* callback)
{
    return m_callbackCollection.registerCallback(callback);
}

void CompositorWorkerGlobalScope::cancelAnimationFrame(int id)
{
    m_callbackCollection.cancelCallback(id);
}

void CompositorWorkerGlobalScope::executeAnimationFrameCallbacks(double highResTimeNow)
{
    m_callbackCollection.executeCallbacks(highResTimeNow, highResTimeNow);
}

CompositorWorkerThread* CompositorWorkerGlobalScope::thread() const
{
    return static_cast<CompositorWorkerThread*>(WorkerGlobalScope::thread());
}

} // namespace blink
