/*
 * Copyright (C) 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Matt Lilek <webkit@mattlilek.com>
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/inspector/InspectorInspectorAgent.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptController.h"
#include "core/InspectorFrontend.h"
#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/InjectedScriptHost.h"
#include "core/inspector/InjectedScriptManager.h"
#include "core/inspector/InspectorState.h"
#include "core/loader/DocumentLoader.h"
#include "core/page/Page.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

namespace InspectorAgentState {
static const char inspectorAgentEnabled[] = "inspectorAgentEnabled";
}

InspectorInspectorAgent::InspectorInspectorAgent(InjectedScriptManager* injectedScriptManager)
    : InspectorBaseAgent<InspectorInspectorAgent, InspectorFrontend::Inspector>("Inspector")
    , m_injectedScriptManager(injectedScriptManager)
{
}

InspectorInspectorAgent::~InspectorInspectorAgent()
{
}

DEFINE_TRACE(InspectorInspectorAgent)
{
    visitor->trace(m_injectedScriptManager);
    InspectorBaseAgent::trace(visitor);
}

void InspectorInspectorAgent::enable(ErrorString*)
{
    m_state->setBoolean(InspectorAgentState::inspectorAgentEnabled, true);
    for (Vector<pair<long, String>>::iterator it = m_pendingEvaluateTestCommands.begin(); frontend() && it != m_pendingEvaluateTestCommands.end(); ++it)
        frontend()->evaluateForTestInFrontend(static_cast<int>((*it).first), (*it).second);
    m_pendingEvaluateTestCommands.clear();
}

void InspectorInspectorAgent::disable(ErrorString*)
{
    m_state->setBoolean(InspectorAgentState::inspectorAgentEnabled, false);
    m_pendingEvaluateTestCommands.clear();
    m_injectedScriptManager->injectedScriptHost()->clearInspectedObjects();
    m_injectedScriptManager->discardInjectedScripts();
}

void InspectorInspectorAgent::didCommitLoadForLocalFrame(LocalFrame* frame)
{
    if (frame != frame->localFrameRoot())
        return;

    m_injectedScriptManager->injectedScriptHost()->clearInspectedObjects();
}

void InspectorInspectorAgent::restore()
{
    if (m_state->getBoolean(InspectorAgentState::inspectorAgentEnabled)) {
        ErrorString error;
        enable(&error);
    }
}

void InspectorInspectorAgent::evaluateForTestInFrontend(long callId, const String& script)
{
    if (m_state->getBoolean(InspectorAgentState::inspectorAgentEnabled)) {
        frontend()->evaluateForTestInFrontend(static_cast<int>(callId), script);
        frontend()->flush();
    } else {
        m_pendingEvaluateTestCommands.append(pair<long, String>(callId, script));
    }
}

void InspectorInspectorAgent::inspect(PassRefPtr<TypeBuilder::Runtime::RemoteObject> objectToInspect, PassRefPtr<JSONObject> hints)
{
    if (frontend() && m_state->getBoolean(InspectorAgentState::inspectorAgentEnabled))
        frontend()->inspect(objectToInspect, hints);
}

} // namespace blink
