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
#include "core/inspector/PageDebuggerAgent.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "core/dom/Document.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorOverlay.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/inspector/MainThreadDebugger.h"
#include "core/loader/DocumentLoader.h"
#include "core/page/Page.h"

using blink::TypeBuilder::Debugger::ExceptionDetails;
using blink::TypeBuilder::Debugger::ScriptId;
using blink::TypeBuilder::Runtime::RemoteObject;

namespace blink {

PassOwnPtrWillBeRawPtr<PageDebuggerAgent> PageDebuggerAgent::create(MainThreadDebugger* MainThreadDebugger, InspectorPageAgent* pageAgent, InjectedScriptManager* injectedScriptManager, InspectorOverlay* overlay)
{
    return adoptPtrWillBeNoop(new PageDebuggerAgent(MainThreadDebugger, pageAgent, injectedScriptManager, overlay));
}

PageDebuggerAgent::PageDebuggerAgent(MainThreadDebugger* MainThreadDebugger, InspectorPageAgent* pageAgent, InjectedScriptManager* injectedScriptManager, InspectorOverlay* overlay)
    : InspectorDebuggerAgent(injectedScriptManager, MainThreadDebugger->debugger()->isolate())
    , m_mainThreadDebugger(MainThreadDebugger)
    , m_pageAgent(pageAgent)
    , m_overlay(overlay)
{
    m_overlay->setListener(this);
}

PageDebuggerAgent::~PageDebuggerAgent()
{
}

DEFINE_TRACE(PageDebuggerAgent)
{
    visitor->trace(m_mainThreadDebugger);
    visitor->trace(m_pageAgent);
    visitor->trace(m_overlay);
    InspectorDebuggerAgent::trace(visitor);
}

bool PageDebuggerAgent::canExecuteScripts() const
{
    ScriptController& scriptController = m_pageAgent->inspectedFrame()->script();
    return scriptController.canExecuteScripts(NotAboutToExecuteScript);
}

void PageDebuggerAgent::enable(ErrorString* errorString)
{
    if (!canExecuteScripts()) {
        *errorString = "Script execution is prohibited";
        return;
    }
    InspectorDebuggerAgent::enable(errorString);
}

void PageDebuggerAgent::restore()
{
    if (canExecuteScripts())
        InspectorDebuggerAgent::restore();
}

void PageDebuggerAgent::enable()
{
    ASSERT(canExecuteScripts());
    InspectorDebuggerAgent::enable();
    m_instrumentingAgents->setPageDebuggerAgent(this);
}

void PageDebuggerAgent::disable()
{
    InspectorDebuggerAgent::disable();
    m_instrumentingAgents->setPageDebuggerAgent(0);
    m_compiledScriptURLs.clear();
}

void PageDebuggerAgent::startListeningV8Debugger()
{
    m_mainThreadDebugger->addListener(this, m_pageAgent->inspectedFrame());
}

void PageDebuggerAgent::stopListeningV8Debugger()
{
    m_mainThreadDebugger->removeListener(this, m_pageAgent->inspectedFrame());
}

V8Debugger& PageDebuggerAgent::debugger()
{
    return *(m_mainThreadDebugger->debugger());
}

void PageDebuggerAgent::muteConsole()
{
    FrameConsole::mute();
}

void PageDebuggerAgent::unmuteConsole()
{
    FrameConsole::unmute();
}

void PageDebuggerAgent::overlayResumed()
{
    ErrorString error;
    resume(&error);
}

void PageDebuggerAgent::overlaySteppedOver()
{
    ErrorString error;
    stepOver(&error);
}

InjectedScript PageDebuggerAgent::injectedScriptForEval(ErrorString* errorString, const int* executionContextId)
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

void PageDebuggerAgent::didStartProvisionalLoad(LocalFrame* frame)
{
    if (frame == m_pageAgent->inspectedFrame()) {
        ErrorString error;
        resume(&error);
    }
}

void PageDebuggerAgent::didClearDocumentOfWindowObject(LocalFrame* frame)
{
    // FIXME: what about nested objects?
    if (frame != m_pageAgent->inspectedFrame())
        return;
    reset();
}

void PageDebuggerAgent::didCommitLoadForLocalFrame(LocalFrame*)
{
    resetModifiedSources();
}

void PageDebuggerAgent::compileScript(ErrorString* errorString, const String& expression, const String& sourceURL, bool persistScript, const int* executionContextId, TypeBuilder::OptOutput<ScriptId>* scriptId, RefPtr<ExceptionDetails>& exceptionDetails)
{
    InjectedScript injectedScript = injectedScriptForEval(errorString, executionContextId);
    if (injectedScript.isEmpty()) {
        *errorString = "Inspected frame has gone";
        return;
    }
    ExecutionContext* executionContext = injectedScript.scriptState()->executionContext();
    RefPtrWillBeRawPtr<LocalFrame> protect(toDocument(executionContext)->frame());
    InspectorDebuggerAgent::compileScript(errorString, expression, sourceURL, persistScript, executionContextId, scriptId, exceptionDetails);
    if (!scriptId->isAssigned())
        return;

    String scriptIdValue = scriptId->getValue();
    if (!scriptIdValue.isEmpty())
        m_compiledScriptURLs.set(scriptId->getValue(), sourceURL);
}

void PageDebuggerAgent::runScript(ErrorString* errorString, const ScriptId& scriptId, const int* executionContextId, const String* const objectGroup, const bool* const doNotPauseOnExceptionsAndMuteConsole, RefPtr<RemoteObject>& result, RefPtr<ExceptionDetails>& exceptionDetails)
{
    InjectedScript injectedScript = injectedScriptForEval(errorString, executionContextId);
    if (injectedScript.isEmpty()) {
        *errorString = "Inspected frame has gone";
        return;
    }
    ExecutionContext* executionContext = injectedScript.scriptState()->executionContext();

    String sourceURL = m_compiledScriptURLs.take(scriptId);
    LocalFrame* frame = toDocument(executionContext)->frame();
    TRACE_EVENT1("devtools.timeline", "EvaluateScript", "data", InspectorEvaluateScriptEvent::data(frame, sourceURL, TextPosition::minimumPosition().m_line.oneBasedInt()));
    InspectorInstrumentationCookie cookie;
    if (frame)
        cookie = InspectorInstrumentation::willEvaluateScript(frame, sourceURL, TextPosition::minimumPosition().m_line.oneBasedInt());

    RefPtrWillBeRawPtr<LocalFrame> protect(frame);
    InspectorDebuggerAgent::runScript(errorString, scriptId, executionContextId, objectGroup, doNotPauseOnExceptionsAndMuteConsole, result, exceptionDetails);

    if (frame)
        InspectorInstrumentation::didEvaluateScript(cookie);
    TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "UpdateCounters", TRACE_EVENT_SCOPE_THREAD, "data", InspectorUpdateCountersEvent::data());

}

} // namespace blink
