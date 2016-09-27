/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef AsyncCallTracker_h
#define AsyncCallTracker_h

#include "core/CoreExport.h"
#include "core/inspector/InspectorDebuggerAgent.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"

namespace blink {

class Event;
class EventListener;
class EventTarget;
class ExecutionContext;
class ExecutionContextTask;
class FormData;
class HTTPHeaderMap;
class InspectorDebuggerAgent;
class KURL;
class MutationObserver;
class ThreadableLoaderClient;
class XMLHttpRequest;

class CORE_EXPORT AsyncCallTracker final : public NoBaseWillBeGarbageCollectedFinalized<AsyncCallTracker>, public InspectorDebuggerAgent::AsyncCallTrackingListener {
    WTF_MAKE_NONCOPYABLE(AsyncCallTracker);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(AsyncCallTracker);
public:
    AsyncCallTracker(InspectorDebuggerAgent*, InstrumentingAgents*);
    virtual ~AsyncCallTracker();

    // InspectorDebuggerAgent::AsyncCallTrackingListener implementation:
    void asyncCallTrackingStateChanged(bool tracking) override;
    void resetAsyncOperations() override;

    void didInstallTimer(ExecutionContext*, int timerId, int timeout, bool singleShot);
    void didRemoveTimer(ExecutionContext*, int timerId);
    bool willFireTimer(ExecutionContext*, int timerId);
    void didFireTimer() { didFireAsyncCall(); }

    void didRequestAnimationFrame(ExecutionContext*, int callbackId);
    void didCancelAnimationFrame(ExecutionContext*, int callbackId);
    bool willFireAnimationFrame(ExecutionContext*, int callbackId);
    void didFireAnimationFrame() { didFireAsyncCall(); }

    void didEnqueueEvent(EventTarget*, Event*);
    void didRemoveEvent(EventTarget*, Event*);
    void willHandleEvent(EventTarget*, Event*, EventListener*, bool useCapture);
    void didHandleEvent() { didFireAsyncCall(); }

    void willLoadXHR(XMLHttpRequest*, ThreadableLoaderClient*, const AtomicString& method, const KURL&, bool async, PassRefPtr<FormData> body, const HTTPHeaderMap& headers, bool includeCrendentials);
    void didDispatchXHRLoadendEvent(XMLHttpRequest*);

    void didEnqueueMutationRecord(ExecutionContext*, MutationObserver*);
    void didClearAllMutationRecords(ExecutionContext*, MutationObserver*);
    void willDeliverMutationRecords(ExecutionContext*, MutationObserver*);
    void didDeliverMutationRecords() { didFireAsyncCall(); }

    void didPostExecutionContextTask(ExecutionContext*, ExecutionContextTask*);
    void didKillAllExecutionContextTasks(ExecutionContext*);
    void willPerformExecutionContextTask(ExecutionContext*, ExecutionContextTask*);
    void didPerformExecutionContextTask() { didFireAsyncCall(); }

    int traceAsyncOperationStarting(ExecutionContext*, const String& operationName, int prevOperationId = 0);
    void traceAsyncOperationCompleted(ExecutionContext*, int operationId);
    void traceAsyncOperationCompletedCallbackStarting(ExecutionContext*, int operationId);
    void traceAsyncCallbackStarting(ExecutionContext*, int operationId);
    void traceAsyncCallbackCompleted() { didFireAsyncCall(); }

    DECLARE_VIRTUAL_TRACE();

    class ExecutionContextData;

private:
    void willHandleXHREvent(XMLHttpRequest*, Event*);

    bool isKnownAsyncOperationId(ExecutionContext*, int operationId) const;
    void willFireAsyncCall(int operationId);
    void didFireAsyncCall();

    ExecutionContextData* createContextDataIfNeeded(ExecutionContext*);

    using ExecutionContextDataMap = WillBeHeapHashMap<RawPtrWillBeMember<ExecutionContext>, OwnPtrWillBeMember<ExecutionContextData>>;
    ExecutionContextDataMap m_executionContextDataMap;
    RawPtrWillBeMember<InspectorDebuggerAgent> m_debuggerAgent;
    RawPtrWillBeMember<InstrumentingAgents> m_instrumentingAgents;
};

} // namespace blink

#endif // AsyncCallTracker_h
