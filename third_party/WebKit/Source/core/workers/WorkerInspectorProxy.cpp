// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/workers/WorkerInspectorProxy.h"

#include "core/dom/CrossThreadTask.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/inspector/WorkerInspectorController.h"
#include "core/workers/WorkerThread.h"
#include "platform/Task.h"
#include "platform/TraceEvent.h"
#include "platform/weborigin/KURL.h"
#include "public/platform/WebTraceLocation.h"

namespace blink {

WorkerInspectorProxy::WorkerInspectorProxy()
    : m_workerThread(nullptr)
    , m_executionContext(nullptr)
    , m_pageInspector(nullptr)
    , m_workerGlobalScopeProxy(nullptr)
{
}

PassOwnPtr<WorkerInspectorProxy> WorkerInspectorProxy::create()
{
    return adoptPtr(new WorkerInspectorProxy());
}

WorkerInspectorProxy::~WorkerInspectorProxy()
{
}

void WorkerInspectorProxy::workerThreadCreated(ExecutionContext* context, WorkerThread* workerThread, const KURL& url)
{
    m_workerThread = workerThread;
    m_executionContext = context;
    InspectorInstrumentation::didStartWorker(context, this, url);
}

void WorkerInspectorProxy::workerThreadTerminated()
{
    if (m_workerThread)
        InspectorInstrumentation::workerTerminated(m_executionContext, this);
    m_workerThread = nullptr;
    m_pageInspector = nullptr;
}

static void connectToWorkerGlobalScopeInspectorTask(WorkerThread* workerThread)
{
    workerThread->workerGlobalScope()->workerInspectorController()->connectFrontend();
}

void WorkerInspectorProxy::connectToInspector(WorkerInspectorProxy::PageInspector* pageInspector)
{
    if (!m_workerThread)
        return;
    ASSERT(!m_pageInspector);
    m_pageInspector = pageInspector;
    addDebuggerTaskForWorker(FROM_HERE, adoptPtr(new Task(threadSafeBind(connectToWorkerGlobalScopeInspectorTask, AllowCrossThreadAccess(m_workerThread)))));
}

static void disconnectFromWorkerGlobalScopeInspectorTask(WorkerThread* workerThread)
{
    workerThread->workerGlobalScope()->workerInspectorController()->disconnectFrontend();
}

void WorkerInspectorProxy::disconnectFromInspector()
{
    m_pageInspector = nullptr;
    if (!m_workerThread)
        return;
    addDebuggerTaskForWorker(FROM_HERE, adoptPtr(new Task(threadSafeBind(disconnectFromWorkerGlobalScopeInspectorTask, AllowCrossThreadAccess(m_workerThread)))));
}

static void dispatchOnInspectorBackendTask(const String& message, WorkerThread* workerThread)
{
    workerThread->workerGlobalScope()->workerInspectorController()->dispatchMessageFromFrontend(message);
}

void WorkerInspectorProxy::sendMessageToInspector(const String& message)
{
    if (!m_workerThread)
        return;
    addDebuggerTaskForWorker(FROM_HERE, adoptPtr(new Task(threadSafeBind(dispatchOnInspectorBackendTask, message, AllowCrossThreadAccess(m_workerThread)))));
    m_workerThread->interruptAndDispatchInspectorCommands();
}

void WorkerInspectorProxy::writeTimelineStartedEvent(const String& sessionId, const String& workerId)
{
    if (!m_workerThread)
        return;
    TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "TracingSessionIdForWorker", TRACE_EVENT_SCOPE_THREAD, "data", InspectorTracingSessionIdForWorkerEvent::data(sessionId, workerId, m_workerThread));
}

static void runDebuggerTaskForWorker(WorkerThread* workerThread)
{
    workerThread->runDebuggerTask(WorkerThread::DontWaitForMessage);
}

void WorkerInspectorProxy::addDebuggerTaskForWorker(const WebTraceLocation& location, PassOwnPtr<WebThread::Task> task)
{
    m_workerThread->appendDebuggerTask(task);
    m_workerThread->backingThread().postTask(location, new Task(threadSafeBind(&runDebuggerTaskForWorker, AllowCrossThreadAccess(m_workerThread))));
}

} // namespace blink
