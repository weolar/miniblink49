/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#include "core/inspector/WorkerInspectorController.h"

#include "core/InspectorBackendDispatcher.h"
#include "core/InspectorFrontend.h"
#include "core/inspector/AsyncCallTracker.h"
#include "core/inspector/InjectedScriptHost.h"
#include "core/inspector/InjectedScriptManager.h"
#include "core/inspector/InspectorConsoleAgent.h"
#include "core/inspector/InspectorFrontendChannel.h"
#include "core/inspector/InspectorHeapProfilerAgent.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorProfilerAgent.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InspectorStateClient.h"
#include "core/inspector/InspectorTaskRunner.h"
#include "core/inspector/InspectorTimelineAgent.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/inspector/WorkerConsoleAgent.h"
#include "core/inspector/WorkerDebuggerAgent.h"
#include "core/inspector/WorkerRuntimeAgent.h"
#include "core/inspector/WorkerThreadDebugger.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/workers/WorkerReportingProxy.h"
#include "core/workers/WorkerThread.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

namespace {

class PageInspectorProxy final : public InspectorFrontendChannel {
    WTF_MAKE_FAST_ALLOCATED(PageInspectorProxy);
public:
    explicit PageInspectorProxy(WorkerGlobalScope* workerGlobalScope) : m_workerGlobalScope(workerGlobalScope) { }
    virtual ~PageInspectorProxy() { }
private:
    virtual void sendProtocolResponse(int callId, PassRefPtr<JSONObject> message) override
    {
        // Worker messages are wrapped, no need to handle callId.
        m_workerGlobalScope->thread()->workerReportingProxy().postMessageToPageInspector(message->toJSONString());
    }
    virtual void sendProtocolNotification(PassRefPtr<JSONObject> message) override
    {
        m_workerGlobalScope->thread()->workerReportingProxy().postMessageToPageInspector(message->toJSONString());
    }
    virtual void flush() override { }
    WorkerGlobalScope* m_workerGlobalScope;
};

class WorkerStateClient final : public InspectorStateClient {
    WTF_MAKE_FAST_ALLOCATED(WorkerStateClient);
public:
    WorkerStateClient(WorkerGlobalScope* context) { }
    virtual ~WorkerStateClient() { }

private:
    virtual void updateInspectorStateCookie(const String& cookie) override { }
};

class RunInspectorCommandsTask final : public InspectorTaskRunner::Task {
public:
    explicit RunInspectorCommandsTask(WorkerThread* thread)
        : m_thread(thread) { }
    virtual ~RunInspectorCommandsTask() { }
    virtual void run() override
    {
        // Process all queued debugger commands. WorkerThread is certainly
        // alive if this task is being executed.
        m_thread->willEnterNestedLoop();
        while (MessageQueueMessageReceived == m_thread->runDebuggerTask(WorkerThread::DontWaitForMessage)) { }
        m_thread->didLeaveNestedLoop();
    }

private:
    WorkerThread* m_thread;
};

}

class WorkerInjectedScriptHostClient: public InjectedScriptHostClient {
public:
    WorkerInjectedScriptHostClient() { }

    ~WorkerInjectedScriptHostClient() override { }
};

WorkerInspectorController::WorkerInspectorController(WorkerGlobalScope* workerGlobalScope)
    : m_workerGlobalScope(workerGlobalScope)
    , m_stateClient(adoptPtr(new WorkerStateClient(workerGlobalScope)))
    , m_state(adoptPtrWillBeNoop(new InspectorCompositeState(m_stateClient.get())))
    , m_instrumentingAgents(InstrumentingAgents::create())
    , m_injectedScriptManager(InjectedScriptManager::createForWorker())
    , m_workerThreadDebugger(WorkerThreadDebugger::create(workerGlobalScope))
    , m_agents(m_instrumentingAgents.get(), m_state.get())
    , m_inspectorTaskRunner(adoptPtr(new InspectorTaskRunner(v8::Isolate::GetCurrent())))
    , m_beforeInitlizedScope(adoptPtr(new InspectorTaskRunner::IgnoreInterruptsScope(m_inspectorTaskRunner.get())))
    , m_paused(false)
{
    OwnPtrWillBeRawPtr<WorkerRuntimeAgent> workerRuntimeAgent = WorkerRuntimeAgent::create(m_injectedScriptManager.get(), m_workerThreadDebugger->debugger(), workerGlobalScope, this);
    m_workerRuntimeAgent = workerRuntimeAgent.get();
    m_agents.append(workerRuntimeAgent.release());

    OwnPtrWillBeRawPtr<WorkerDebuggerAgent> workerDebuggerAgent = WorkerDebuggerAgent::create(m_workerThreadDebugger.get(), workerGlobalScope, m_injectedScriptManager.get());
    m_workerDebuggerAgent = workerDebuggerAgent.get();
    m_agents.append(workerDebuggerAgent.release());
    m_asyncCallTracker = adoptPtrWillBeNoop(new AsyncCallTracker(m_workerDebuggerAgent, m_instrumentingAgents.get()));

    m_agents.append(InspectorProfilerAgent::create(m_injectedScriptManager.get(), 0));
    m_agents.append(InspectorHeapProfilerAgent::create(m_injectedScriptManager.get()));

    OwnPtrWillBeRawPtr<WorkerConsoleAgent> workerConsoleAgent = WorkerConsoleAgent::create(m_injectedScriptManager.get(), workerGlobalScope);
    WorkerConsoleAgent* workerConsoleAgentPtr = workerConsoleAgent.get();
    workerConsoleAgentPtr->setDebuggerAgent(m_workerDebuggerAgent);
    m_agents.append(workerConsoleAgent.release());

    m_agents.append(InspectorTimelineAgent::create());

    m_injectedScriptManager->injectedScriptHost()->init(workerConsoleAgentPtr, m_workerDebuggerAgent, nullptr, m_workerThreadDebugger->debugger(), adoptPtr(new WorkerInjectedScriptHostClient()));
}

WorkerInspectorController::~WorkerInspectorController()
{
}

void WorkerInspectorController::registerModuleAgent(PassOwnPtrWillBeRawPtr<InspectorAgent> agent)
{
    m_agents.append(agent);
}

void WorkerInspectorController::connectFrontend()
{
    ASSERT(!m_frontend);
    m_state->unmute();
    m_frontendChannel = adoptPtr(new PageInspectorProxy(m_workerGlobalScope));
    m_frontend = adoptPtr(new InspectorFrontend(m_frontendChannel.get()));
    m_backendDispatcher = InspectorBackendDispatcher::create(m_frontendChannel.get());
    m_agents.registerInDispatcher(m_backendDispatcher.get());
    m_agents.setFrontend(m_frontend.get());
    InspectorInstrumentation::frontendCreated();
}

void WorkerInspectorController::disconnectFrontend()
{
    if (!m_frontend)
        return;
    m_backendDispatcher->clearFrontend();
    m_backendDispatcher.clear();
    // Destroying agents would change the state, but we don't want that.
    // Pre-disconnect state will be used to restore inspector agents.
    m_state->mute();
    m_agents.clearFrontend();
    m_frontend.clear();
    InspectorInstrumentation::frontendDeleted();
    m_frontendChannel.clear();
}

void WorkerInspectorController::restoreInspectorStateFromCookie(const String& inspectorCookie)
{
    ASSERT(!m_frontend);
    connectFrontend();
    m_state->loadFromCookie(inspectorCookie);

    m_agents.restore();
}

void WorkerInspectorController::dispatchMessageFromFrontend(const String& message)
{
    InspectorTaskRunner::IgnoreInterruptsScope scope(m_inspectorTaskRunner.get());
    if (m_backendDispatcher)
        m_backendDispatcher->dispatch(message);
}

void WorkerInspectorController::dispose()
{
    m_instrumentingAgents->reset();
    disconnectFrontend();
}

void WorkerInspectorController::interruptAndDispatchInspectorCommands()
{
    m_inspectorTaskRunner->interruptAndRun(adoptPtr(new RunInspectorCommandsTask(m_workerGlobalScope->thread())));
}

void WorkerInspectorController::resumeStartup()
{
    m_paused = false;
}

bool WorkerInspectorController::isRunRequired()
{
    return m_paused;
}

void WorkerInspectorController::workerContextInitialized(bool shouldPauseOnStart)
{
    m_beforeInitlizedScope.clear();
    if (shouldPauseOnStart)
        pauseOnStart();
}

void WorkerInspectorController::pauseOnStart()
{
    m_paused = true;
    MessageQueueWaitResult result;
    m_workerGlobalScope->thread()->willEnterNestedLoop();
    do {
        result = m_workerGlobalScope->thread()->runDebuggerTask();
    // Keep waiting until execution is resumed.
    } while (result == MessageQueueMessageReceived && m_paused);
    m_workerGlobalScope->thread()->didLeaveNestedLoop();
}

DEFINE_TRACE(WorkerInspectorController)
{
    visitor->trace(m_workerGlobalScope);
    visitor->trace(m_state);
    visitor->trace(m_instrumentingAgents);
    visitor->trace(m_injectedScriptManager);
    visitor->trace(m_workerThreadDebugger);
    visitor->trace(m_backendDispatcher);
    visitor->trace(m_agents);
    visitor->trace(m_workerDebuggerAgent);
    visitor->trace(m_asyncCallTracker);
    visitor->trace(m_workerRuntimeAgent);
}

} // namespace blink
