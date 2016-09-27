/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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
#include "core/workers/SharedWorker.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/dom/MessageChannel.h"
#include "core/dom/MessagePort.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/UseCounter.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/workers/SharedWorkerRepositoryClient.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

inline SharedWorker::SharedWorker(ExecutionContext* context)
    : AbstractWorker(context)
    , m_isBeingConnected(false)
{
}

PassRefPtrWillBeRawPtr<SharedWorker> SharedWorker::create(ExecutionContext* context, const String& url, const String& name, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    ASSERT_WITH_SECURITY_IMPLICATION(context->isDocument());

    UseCounter::count(context, UseCounter::SharedWorkerStart);

    RefPtrWillBeRawPtr<SharedWorker> worker = adoptRefWillBeNoop(new SharedWorker(context));

    MessageChannel* channel = MessageChannel::create(context);
    worker->m_port = channel->port1();
    OwnPtr<WebMessagePortChannel> remotePort = channel->port2()->disentangle();
    ASSERT(remotePort);

    worker->suspendIfNeeded();

    // We don't currently support nested workers, so workers can only be created from documents.
    Document* document = toDocument(context);
    if (!document->securityOrigin()->canAccessSharedWorkers()) {
        exceptionState.throwSecurityError("Access to shared workers is denied to origin '" + document->securityOrigin()->toString() + "'.");
        return nullptr;
    }

    KURL scriptURL = worker->resolveURL(url, exceptionState);
    if (scriptURL.isEmpty())
        return nullptr;

    if (document->frame()->loader().client()->sharedWorkerRepositoryClient())
        document->frame()->loader().client()->sharedWorkerRepositoryClient()->connect(worker.get(), remotePort.release(), scriptURL, name, exceptionState);

    return worker.release();
}

SharedWorker::~SharedWorker()
{
}

const AtomicString& SharedWorker::interfaceName() const
{
    return EventTargetNames::SharedWorker;
}

bool SharedWorker::hasPendingActivity() const
{
    return m_isBeingConnected;
}

DEFINE_TRACE(SharedWorker)
{
#if ENABLE(OILPAN)
    visitor->trace(m_port);
    HeapSupplementable<SharedWorker>::trace(visitor);
#endif
    AbstractWorker::trace(visitor);
}

} // namespace blink
