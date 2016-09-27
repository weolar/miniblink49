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

#include "core/inspector/InspectorWorkerAgent.h"

#include "core/InspectorFrontend.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/inspector/PageConsoleAgent.h"
#include "core/workers/WorkerInspectorProxy.h"
#include "platform/weborigin/KURL.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace WorkerAgentState {
static const char workerInspectionEnabled[] = "workerInspectionEnabled";
static const char autoconnectToWorkers[] = "autoconnectToWorkers";
};

class InspectorWorkerAgent::WorkerAgentClient final : public WorkerInspectorProxy::PageInspector {
    WTF_MAKE_FAST_ALLOCATED(InspectorWorkerAgent::WorkerAgentClient);
public:
    WorkerAgentClient(InspectorFrontend::Worker* frontend, WorkerInspectorProxy* proxy, const String& id, PageConsoleAgent* consoleAgent)
        : m_frontend(frontend)
        , m_proxy(proxy)
        , m_id(id)
        , m_connected(false)
        , m_consoleAgent(consoleAgent)
    {
        ASSERT(!proxy->pageInspector());
    }
    virtual ~WorkerAgentClient()
    {
        disconnectFromWorker();
    }

    String id() const { return m_id; }
    WorkerInspectorProxy* proxy() const { return m_proxy; }

    void connectToWorker()
    {
        if (m_connected)
            return;
        m_connected = true;
        m_proxy->connectToInspector(this);
    }

    void disconnectFromWorker()
    {
        if (!m_connected)
            return;
        m_connected = false;
        m_proxy->disconnectFromInspector();
    }

private:
    // WorkerInspectorProxy::PageInspector implementation
    virtual void dispatchMessageFromWorker(const String& message) override
    {
        m_frontend->dispatchMessageFromWorker(m_id, message);
    }
    // WorkerInspectorProxy::PageInspector implementation
    virtual void workerConsoleAgentEnabled(WorkerGlobalScopeProxy* proxy) override
    {
        m_consoleAgent->workerConsoleAgentEnabled(proxy);
    }

    InspectorFrontend::Worker* m_frontend;
    WorkerInspectorProxy* m_proxy;
    String m_id;
    bool m_connected;
    PageConsoleAgent* m_consoleAgent;
};

PassOwnPtrWillBeRawPtr<InspectorWorkerAgent> InspectorWorkerAgent::create(PageConsoleAgent* consoleAgent)
{
    return adoptPtrWillBeNoop(new InspectorWorkerAgent(consoleAgent));
}

InspectorWorkerAgent::InspectorWorkerAgent(PageConsoleAgent* consoleAgent)
    : InspectorBaseAgent<InspectorWorkerAgent, InspectorFrontend::Worker>("Worker")
    , m_consoleAgent(consoleAgent)
{
}

InspectorWorkerAgent::~InspectorWorkerAgent()
{
#if !ENABLE(OILPAN)
    m_instrumentingAgents->setInspectorWorkerAgent(0);
#endif
}

void InspectorWorkerAgent::init()
{
    m_instrumentingAgents->setInspectorWorkerAgent(this);
}

void InspectorWorkerAgent::restore()
{
    if (m_state->getBoolean(WorkerAgentState::workerInspectionEnabled))
        createWorkerAgentClientsForExistingWorkers();
}

void InspectorWorkerAgent::enable(ErrorString*)
{
    m_state->setBoolean(WorkerAgentState::workerInspectionEnabled, true);
    createWorkerAgentClientsForExistingWorkers();
}

void InspectorWorkerAgent::disable(ErrorString*)
{
    m_state->setBoolean(WorkerAgentState::workerInspectionEnabled, false);
    m_state->setBoolean(WorkerAgentState::autoconnectToWorkers, false);
    destroyWorkerAgentClients();
}

void InspectorWorkerAgent::connectToWorker(ErrorString* error, const String& workerId)
{
    WorkerAgentClient* client = m_idToClient.get(workerId);
    if (client)
        client->connectToWorker();
    else
        *error = "Worker is gone";
}

void InspectorWorkerAgent::disconnectFromWorker(ErrorString* error, const String& workerId)
{
    WorkerAgentClient* client = m_idToClient.get(workerId);
    if (client)
        client->disconnectFromWorker();
    else
        *error = "Worker is gone";
}

void InspectorWorkerAgent::sendMessageToWorker(ErrorString* error, const String& workerId, const String& message)
{
    WorkerAgentClient* client = m_idToClient.get(workerId);
    if (client)
        client->proxy()->sendMessageToInspector(message);
    else
        *error = "Worker is gone";
}

void InspectorWorkerAgent::setAutoconnectToWorkers(ErrorString*, bool value)
{
    m_state->setBoolean(WorkerAgentState::autoconnectToWorkers, value);
}

void InspectorWorkerAgent::setTracingSessionId(const String& sessionId)
{
    m_tracingSessionId = sessionId;
    if (sessionId.isEmpty())
        return;
    for (auto& info : m_workerInfos)
        info.key->writeTimelineStartedEvent(sessionId, info.value.id);
}

bool InspectorWorkerAgent::shouldPauseDedicatedWorkerOnStart()
{
    return m_state->getBoolean(WorkerAgentState::autoconnectToWorkers);
}

void InspectorWorkerAgent::didStartWorker(WorkerInspectorProxy* workerInspectorProxy, const KURL& url)
{
    String id = "dedicated:" + IdentifiersFactory::createIdentifier();
    m_workerInfos.set(workerInspectorProxy, WorkerInfo(url.string(), id));
    if (frontend() && m_state->getBoolean(WorkerAgentState::workerInspectionEnabled))
        createWorkerAgentClient(workerInspectorProxy, url.string(), id);
    if (!m_tracingSessionId.isEmpty())
        workerInspectorProxy->writeTimelineStartedEvent(m_tracingSessionId, id);
}

void InspectorWorkerAgent::workerTerminated(WorkerInspectorProxy* proxy)
{
    m_workerInfos.remove(proxy);
    for (WorkerClients::iterator it = m_idToClient.begin(); it != m_idToClient.end(); ++it) {
        if (proxy == it->value->proxy()) {
            frontend()->workerTerminated(it->key);
            delete it->value;
            m_idToClient.remove(it);
            return;
        }
    }
}

void InspectorWorkerAgent::createWorkerAgentClientsForExistingWorkers()
{
    for (auto& info : m_workerInfos)
        createWorkerAgentClient(info.key, info.value.url, info.value.id);
}

void InspectorWorkerAgent::destroyWorkerAgentClients()
{
    for (auto& client : m_idToClient) {
        client.value->disconnectFromWorker();
        delete client.value;
    }
    m_idToClient.clear();
}

void InspectorWorkerAgent::createWorkerAgentClient(WorkerInspectorProxy* workerInspectorProxy, const String& url, const String& id)
{
    WorkerAgentClient* client = new WorkerAgentClient(frontend(), workerInspectorProxy, id, m_consoleAgent);
    m_idToClient.set(id, client);

    ASSERT(frontend());
    bool autoconnectToWorkers = m_state->getBoolean(WorkerAgentState::autoconnectToWorkers);
    if (autoconnectToWorkers)
        client->connectToWorker();
    frontend()->workerCreated(id, url, autoconnectToWorkers);
}

} // namespace blink
