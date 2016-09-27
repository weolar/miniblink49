// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InProcessWorkerBase_h
#define InProcessWorkerBase_h

#include "core/CoreExport.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/MessagePort.h"
#include "core/events/EventListener.h"
#include "core/events/EventTarget.h"
#include "core/workers/AbstractWorker.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/AtomicStringHash.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class WorkerGlobalScopeProxy;
class WorkerScriptLoader;

// Base class for workers that operate in the same process as the document that
// creates them.
class CORE_EXPORT InProcessWorkerBase : public AbstractWorker {
public:
    ~InProcessWorkerBase() override;

    void postMessage(ExecutionContext*, PassRefPtr<SerializedScriptValue> message, const MessagePortArray*, ExceptionState&);
    void terminate();

    // ActiveDOMObject
    void stop() override;
    bool hasPendingActivity() const override;

    PassRefPtr<ContentSecurityPolicy> contentSecurityPolicy();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(message);

    DECLARE_VIRTUAL_TRACE();

protected:
    explicit InProcessWorkerBase(ExecutionContext*);
    bool initialize(ExecutionContext*, const String&, ExceptionState&);

    // Creates a proxy to allow communicating with the worker's global scope. InProcessWorkerBase does not take ownership of the
    // created proxy. The proxy is expected to manage its own lifetime, and delete itself in response to terminateWorkerGlobalScope().
    virtual WorkerGlobalScopeProxy* createWorkerGlobalScopeProxy(ExecutionContext*) = 0;

private:
    // Callbacks for m_scriptLoader.
    void onResponse();
    void onFinished();

    OwnPtr<WorkerScriptLoader> m_scriptLoader;
    RefPtr<ContentSecurityPolicy> m_contentSecurityPolicy;
    WorkerGlobalScopeProxy* m_contextProxy; // The proxy outlives the worker to perform thread shutdown.
};

} // namespace blink

#endif // InProcessWorkerBase_h
