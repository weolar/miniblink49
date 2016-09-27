// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/V8AsyncCallTracker.h"

#include "bindings/core/v8/V8PerContextData.h"
#include "core/inspector/AsyncOperationMap.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/StringHash.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace {

static const char v8AsyncTaskEventEnqueue[] = "enqueue";
static const char v8AsyncTaskEventWillHandle[] = "willHandle";
static const char v8AsyncTaskEventDidHandle[] = "didHandle";

}

class V8AsyncCallTracker::V8ContextAsyncOperations final : public NoBaseWillBeGarbageCollectedFinalized<V8AsyncCallTracker::V8ContextAsyncOperations> {
    WTF_MAKE_NONCOPYABLE(V8ContextAsyncOperations);
public:
    explicit V8ContextAsyncOperations(InspectorDebuggerAgent* debuggerAgent)
        : m_v8AsyncOperations(debuggerAgent)
    {
    }

    ~V8ContextAsyncOperations()
    {
        ASSERT(m_v8AsyncOperations.hasBeenDisposed());
    }

    void dispose()
    {
        // FIXME: get rid of the dispose method and this class altogether once AsyncOperationMap is always allocated on C++ heap.
        m_v8AsyncOperations.dispose();
    }

    DEFINE_INLINE_TRACE()
    {
#if ENABLE(OILPAN)
        visitor->trace(m_v8AsyncOperations);
#endif
    }

    AsyncOperationMap<String> m_v8AsyncOperations;
};

static String makeV8AsyncTaskUniqueId(const String& eventName, int id)
{
    StringBuilder builder;
    builder.append(eventName);
    builder.append(" -> ");
    builder.appendNumber(id);
    return builder.toString();
}

V8AsyncCallTracker::V8AsyncCallTracker(InspectorDebuggerAgent* debuggerAgent) : m_debuggerAgent(debuggerAgent)
{
    m_debuggerAgent->addAsyncCallTrackingListener(this);
}

V8AsyncCallTracker::~V8AsyncCallTracker()
{
    ASSERT(m_contextAsyncOperationMap.isEmpty());
#if !ENABLE(OILPAN)
    m_debuggerAgent->removeAsyncCallTrackingListener(this);
#endif
}

DEFINE_TRACE(V8AsyncCallTracker)
{
#if ENABLE(OILPAN)
    visitor->trace(m_contextAsyncOperationMap);
    visitor->trace(m_debuggerAgent);
#endif
    InspectorDebuggerAgent::AsyncCallTrackingListener::trace(visitor);
}

void V8AsyncCallTracker::asyncCallTrackingStateChanged(bool)
{
}

void V8AsyncCallTracker::resetAsyncOperations()
{
    for (auto& it : m_contextAsyncOperationMap) {
        it.key->removeObserver(this);
        it.value->dispose();
    }
    m_contextAsyncOperationMap.clear();
}

void V8AsyncCallTracker::willDisposeScriptState(ScriptState* state)
{
    m_contextAsyncOperationMap.remove(state);
}

void V8AsyncCallTracker::didReceiveV8AsyncTaskEvent(ScriptState* state, const String& eventType, const String& eventName, int id)
{
    ASSERT(m_debuggerAgent->trackingAsyncCalls());
    if (eventType == v8AsyncTaskEventEnqueue)
        didEnqueueV8AsyncTask(state, eventName, id);
    else if (eventType == v8AsyncTaskEventWillHandle)
        willHandleV8AsyncTask(state, eventName, id);
    else if (eventType == v8AsyncTaskEventDidHandle)
        m_debuggerAgent->traceAsyncCallbackCompleted();
    else
        ASSERT_NOT_REACHED();
}

void V8AsyncCallTracker::didEnqueueV8AsyncTask(ScriptState* state, const String& eventName, int id)
{
    ASSERT(state);
    ASSERT(m_debuggerAgent->trackingAsyncCalls());
    int operationId = m_debuggerAgent->traceAsyncOperationStarting(eventName);
    if (!operationId)
        return;
    V8ContextAsyncOperations* contextCallChains = m_contextAsyncOperationMap.get(state);
    if (!contextCallChains)
        contextCallChains = m_contextAsyncOperationMap.set(state, adoptPtrWillBeNoop(new V8ContextAsyncOperations(m_debuggerAgent))).storedValue->value.get();
    contextCallChains->m_v8AsyncOperations.set(makeV8AsyncTaskUniqueId(eventName, id), operationId);
}

void V8AsyncCallTracker::willHandleV8AsyncTask(ScriptState* state, const String& eventName, int id)
{
    ASSERT(state);
    ASSERT(m_debuggerAgent->trackingAsyncCalls());
    if (V8ContextAsyncOperations* contextCallChains = m_contextAsyncOperationMap.get(state)) {
        String taskId = makeV8AsyncTaskUniqueId(eventName, id);
        m_debuggerAgent->traceAsyncCallbackStarting(contextCallChains->m_v8AsyncOperations.get(taskId));
        contextCallChains->m_v8AsyncOperations.remove(taskId);
    } else {
        m_debuggerAgent->traceAsyncCallbackStarting(InspectorDebuggerAgent::unknownAsyncOperationId);
    }
}

} // namespace blink
