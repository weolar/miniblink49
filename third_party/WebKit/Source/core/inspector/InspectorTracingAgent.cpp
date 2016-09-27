//
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "config.h"

#include "core/inspector/InspectorTracingAgent.h"

#include "core/frame/LocalFrame.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/inspector/InspectorWorkerAgent.h"
#include "platform/TraceEvent.h"

namespace blink {

namespace TracingAgentState {
const char sessionId[] = "sessionId";
}

namespace {
const char devtoolsMetadataEventCategory[] = TRACE_DISABLED_BY_DEFAULT("devtools.timeline");
}

InspectorTracingAgent::InspectorTracingAgent(Client* client, InspectorWorkerAgent* workerAgent, InspectorPageAgent* pageAgent)
    : InspectorBaseAgent<InspectorTracingAgent, InspectorFrontend::Tracing>("Tracing")
    , m_layerTreeId(0)
    , m_client(client)
    , m_workerAgent(workerAgent)
    , m_pageAgent(pageAgent)
{
}

DEFINE_TRACE(InspectorTracingAgent)
{
    visitor->trace(m_workerAgent);
    visitor->trace(m_pageAgent);
    InspectorBaseAgent::trace(visitor);
}

void InspectorTracingAgent::restore()
{
    emitMetadataEvents();
}

void InspectorTracingAgent::start(ErrorString*, const String* categoryFilter, const String*, const double*, PassRefPtrWillBeRawPtr<StartCallback> callback)
{
    ASSERT(m_state->getString(TracingAgentState::sessionId).isEmpty());
    m_state->setString(TracingAgentState::sessionId, IdentifiersFactory::createIdentifier());
    m_client->enableTracing(categoryFilter ? *categoryFilter : String());
    emitMetadataEvents();
    callback->sendSuccess();
}

void InspectorTracingAgent::end(ErrorString* errorString, PassRefPtrWillBeRawPtr<EndCallback> callback)
{
    m_client->disableTracing();
    resetSessionId();
    callback->sendSuccess();
}

String InspectorTracingAgent::sessionId()
{
    return m_state->getString(TracingAgentState::sessionId);
}

void InspectorTracingAgent::emitMetadataEvents()
{
    TRACE_EVENT_INSTANT1(devtoolsMetadataEventCategory, "TracingStartedInPage", TRACE_EVENT_SCOPE_THREAD, "data", InspectorTracingStartedInFrame::data(sessionId(), m_pageAgent->inspectedFrame()));
    if (m_layerTreeId)
        setLayerTreeId(m_layerTreeId);
    m_workerAgent->setTracingSessionId(sessionId());
}

void InspectorTracingAgent::setLayerTreeId(int layerTreeId)
{
    m_layerTreeId = layerTreeId;
    TRACE_EVENT_INSTANT1(devtoolsMetadataEventCategory, "SetLayerTreeId", TRACE_EVENT_SCOPE_THREAD, "data", InspectorSetLayerTreeId::data(sessionId(), m_layerTreeId));
}

void InspectorTracingAgent::disable(ErrorString*)
{
    resetSessionId();
}

void InspectorTracingAgent::resetSessionId()
{
    m_state->remove(TracingAgentState::sessionId);
    m_workerAgent->setTracingSessionId(sessionId());
}

}
