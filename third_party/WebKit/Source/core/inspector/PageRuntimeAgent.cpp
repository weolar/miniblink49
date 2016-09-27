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
#include "core/inspector/PageRuntimeAgent.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InjectedScriptManager.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/page/Page.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

PageRuntimeAgent::PageRuntimeAgent(InjectedScriptManager* injectedScriptManager, Client* client, V8Debugger* debugger, InspectorPageAgent* pageAgent)
    : InspectorRuntimeAgent(injectedScriptManager, debugger, client)
    , m_pageAgent(pageAgent)
    , m_mainWorldContextCreated(false)
{
}

PageRuntimeAgent::~PageRuntimeAgent()
{
#if !ENABLE(OILPAN)
    m_instrumentingAgents->setPageRuntimeAgent(0);
#endif
}

DEFINE_TRACE(PageRuntimeAgent)
{
    visitor->trace(m_pageAgent);
    InspectorRuntimeAgent::trace(visitor);
}

void PageRuntimeAgent::init()
{
    m_instrumentingAgents->setPageRuntimeAgent(this);
}

void PageRuntimeAgent::enable(ErrorString* errorString)
{
    if (m_enabled)
        return;

    InspectorRuntimeAgent::enable(errorString);

    // Only report existing contexts if the page did commit load, otherwise we may
    // unintentionally initialize contexts in the frames which may trigger some listeners
    // that are expected to be triggered only after the load is committed, see http://crbug.com/131623
    if (m_mainWorldContextCreated)
        reportExecutionContextCreation();
}

void PageRuntimeAgent::disable(ErrorString* errorString)
{
    if (!m_enabled)
        return;
    m_scriptStateToId.clear();
    InspectorRuntimeAgent::disable(errorString);
}

void PageRuntimeAgent::didClearDocumentOfWindowObject(LocalFrame* frame)
{
    m_mainWorldContextCreated = true;

    if (!m_enabled)
        return;
    ASSERT(frontend());

    frame->script().initializeMainWorld();
}

void PageRuntimeAgent::didCreateScriptContext(LocalFrame* frame, ScriptState* scriptState, SecurityOrigin* origin, int worldId)
{
    if (!m_enabled)
        return;
    ASSERT(frontend());
    bool isMainWorld = worldId == MainWorldId;
    String originString = origin ? origin->toRawString() : "";
    String frameId = IdentifiersFactory::frameId(frame);
    reportExecutionContext(scriptState, isMainWorld, originString, frameId);
}

void PageRuntimeAgent::willReleaseScriptContext(LocalFrame* frame, ScriptState* scriptState)
{
    injectedScriptManager()->discardInjectedScriptFor(scriptState);
    ScriptStateToId::iterator it = m_scriptStateToId.find(scriptState);
    if (it == m_scriptStateToId.end())
        return;
    int id = it->value;
    m_scriptStateToId.remove(scriptState);
    frontend()->executionContextDestroyed(id);
}

InjectedScript PageRuntimeAgent::injectedScriptForEval(ErrorString* errorString, const int* executionContextId)
{
    if (!executionContextId) {
        ScriptState* scriptState = ScriptState::forMainWorld(m_pageAgent->inspectedFrame());
        InjectedScript result = injectedScriptManager()->injectedScriptFor(scriptState);
        if (result.isEmpty())
            *errorString = "Internal error: main world execution context not found.";
        return result;
    }
    InjectedScript injectedScript = injectedScriptManager()->injectedScriptForId(*executionContextId);
    if (injectedScript.isEmpty())
        *errorString = "Execution context with given id not found.";
    return injectedScript;
}

void PageRuntimeAgent::muteConsole()
{
    FrameConsole::mute();
}

void PageRuntimeAgent::unmuteConsole()
{
    FrameConsole::unmute();
}

void PageRuntimeAgent::reportExecutionContextCreation()
{
    Vector<std::pair<ScriptState*, SecurityOrigin*> > isolatedContexts;
    for (Frame* frame = m_pageAgent->inspectedFrame(); frame; frame = frame->tree().traverseNext(m_pageAgent->inspectedFrame())) {
        if (!frame->isLocalFrame())
            continue;
        LocalFrame* localFrame = toLocalFrame(frame);
        if (!localFrame->script().canExecuteScripts(NotAboutToExecuteScript))
            continue;
        String frameId = IdentifiersFactory::frameId(localFrame);

        // Ensure execution context is created.
        // If initializeMainWorld returns true, then is registered by didCreateScriptContext
        if (!localFrame->script().initializeMainWorld())
            reportExecutionContext(ScriptState::forMainWorld(localFrame), true, "", frameId);
        localFrame->script().collectIsolatedContexts(isolatedContexts);
        if (isolatedContexts.isEmpty())
            continue;
        for (const auto& pair : isolatedContexts) {
            String originString = pair.second ? pair.second->toRawString() : "";
            reportExecutionContext(pair.first, false, originString, frameId);
        }
        isolatedContexts.clear();
    }
}

void PageRuntimeAgent::reportExecutionContext(ScriptState* scriptState, bool isPageContext, const String& origin, const String& frameId)
{
    int executionContextId = injectedScriptManager()->injectedScriptIdFor(scriptState);
    m_scriptStateToId.set(scriptState, executionContextId);
    DOMWrapperWorld& world = scriptState->world();
    String humanReadableName = world.isIsolatedWorld() ? world.isolatedWorldHumanReadableName() : "";
    String type = isPageContext ? "" : "Extension";
    InspectorRuntimeAgent::addExecutionContextToFrontend(executionContextId, type, origin, humanReadableName, frameId);
}

} // namespace blink

