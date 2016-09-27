// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/compositorworker/CompositorWorker.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/workers/WorkerClients.h"
#include "modules/EventTargetModules.h"
#include "modules/compositorworker/CompositorWorkerMessagingProxy.h"
#include "wtf/MainThread.h"

namespace blink {

inline CompositorWorker::CompositorWorker(ExecutionContext* context)
    : InProcessWorkerBase(context)
{
}

PassRefPtrWillBeRawPtr<CompositorWorker> CompositorWorker::create(ExecutionContext* context, const String& url, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    Document* document = toDocument(context);
    if (!document->page()) {
        exceptionState.throwDOMException(InvalidAccessError, "The context provided is invalid.");
        return nullptr;
    }
    RefPtrWillBeRawPtr<CompositorWorker> worker = adoptRefWillBeNoop(new CompositorWorker(context));
    if (worker->initialize(context, url, exceptionState))
        return worker.release();
    return nullptr;
}

CompositorWorker::~CompositorWorker()
{
    ASSERT(isMainThread());
}

const AtomicString& CompositorWorker::interfaceName() const
{
    return EventTargetNames::CompositorWorker;
}

WorkerGlobalScopeProxy* CompositorWorker::createWorkerGlobalScopeProxy(ExecutionContext* worker)
{
    ASSERT(executionContext()->isDocument());
    return new CompositorWorkerMessagingProxy(this);
}

} // namespace blink
