/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "core/workers/DedicatedWorkerGlobalScope.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/workers/DedicatedWorkerThread.h"
#include "core/workers/WorkerClients.h"
#include "core/workers/WorkerObjectProxy.h"
#include "core/workers/WorkerThreadStartupData.h"

namespace blink {

PassRefPtrWillBeRawPtr<DedicatedWorkerGlobalScope> DedicatedWorkerGlobalScope::create(DedicatedWorkerThread* thread, PassOwnPtr<WorkerThreadStartupData> startupData, double timeOrigin)
{
    // Note: startupData is finalized on return. After the relevant parts has been
    // passed along to the created 'context'.
    RefPtrWillBeRawPtr<DedicatedWorkerGlobalScope> context = adoptRefWillBeNoop(new DedicatedWorkerGlobalScope(startupData->m_scriptURL, startupData->m_userAgent, thread, timeOrigin, startupData->m_starterOrigin, startupData->m_workerClients.release()));
    context->applyContentSecurityPolicyFromVector(*startupData->m_contentSecurityPolicyHeaders);
    return context.release();
}

DedicatedWorkerGlobalScope::DedicatedWorkerGlobalScope(const KURL& url, const String& userAgent, DedicatedWorkerThread* thread, double timeOrigin, const SecurityOrigin* starterOrigin, PassOwnPtrWillBeRawPtr<WorkerClients> workerClients)
    : WorkerGlobalScope(url, userAgent, thread, timeOrigin, starterOrigin, workerClients)
{
}

DedicatedWorkerGlobalScope::~DedicatedWorkerGlobalScope()
{
}

const AtomicString& DedicatedWorkerGlobalScope::interfaceName() const
{
    return EventTargetNames::DedicatedWorkerGlobalScope;
}

void DedicatedWorkerGlobalScope::postMessage(ExecutionContext* context, PassRefPtr<SerializedScriptValue> message, const MessagePortArray* ports, ExceptionState& exceptionState)
{
    // Disentangle the port in preparation for sending it to the remote context.
    OwnPtr<MessagePortChannelArray> channels = MessagePort::disentanglePorts(context, ports, exceptionState);
    if (exceptionState.hadException())
        return;
    thread()->workerObjectProxy().postMessageToWorkerObject(message, channels.release());
}

void DedicatedWorkerGlobalScope::importScripts(const Vector<String>& urls, ExceptionState& exceptionState)
{
    Base::importScripts(urls, exceptionState);
    thread()->workerObjectProxy().reportPendingActivity(hasPendingActivity());
}

DedicatedWorkerThread* DedicatedWorkerGlobalScope::thread() const
{
    return static_cast<DedicatedWorkerThread*>(Base::thread());
}

class UseCounterTask : public ExecutionContextTask {
public:
    static PassOwnPtr<UseCounterTask> createCount(UseCounter::Feature feature) { return adoptPtr(new UseCounterTask(feature, false)); }
    static PassOwnPtr<UseCounterTask> createDeprecation(UseCounter::Feature feature) { return adoptPtr(new UseCounterTask(feature, true)); }

private:
    UseCounterTask(UseCounter::Feature feature, bool isDeprecation)
        : m_feature(feature)
        , m_isDeprecation(isDeprecation)
    {
    }

    virtual void performTask(ExecutionContext* context) override
    {
        ASSERT(context->isDocument());
        if (m_isDeprecation)
            UseCounter::countDeprecation(context, m_feature);
        else
            UseCounter::count(context, m_feature);
    }

    UseCounter::Feature m_feature;
    bool m_isDeprecation;
};

void DedicatedWorkerGlobalScope::countFeature(UseCounter::Feature feature) const
{
    thread()->workerObjectProxy().postTaskToMainExecutionContext(UseCounterTask::createCount(feature));
}

void DedicatedWorkerGlobalScope::countDeprecation(UseCounter::Feature feature) const
{
    thread()->workerObjectProxy().postTaskToMainExecutionContext(UseCounterTask::createDeprecation(feature));
}

DEFINE_TRACE(DedicatedWorkerGlobalScope)
{
    WorkerGlobalScope::trace(visitor);
}

} // namespace blink
