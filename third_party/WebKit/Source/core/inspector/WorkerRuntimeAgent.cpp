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

#include "core/inspector/WorkerRuntimeAgent.h"

#include "bindings/core/v8/ScriptState.h"
#include "core/inspector/InjectedScript.h"
#include "core/workers/WorkerGlobalScope.h"

namespace blink {

WorkerRuntimeAgent::WorkerRuntimeAgent(InjectedScriptManager* injectedScriptManager, V8Debugger* debugger, WorkerGlobalScope* workerGlobalScope, InspectorRuntimeAgent::Client* client)
    : InspectorRuntimeAgent(injectedScriptManager, debugger, client)
    , m_workerGlobalScope(workerGlobalScope)
{
}

WorkerRuntimeAgent::~WorkerRuntimeAgent()
{
}

DEFINE_TRACE(WorkerRuntimeAgent)
{
    visitor->trace(m_workerGlobalScope);
    InspectorRuntimeAgent::trace(visitor);
}

void WorkerRuntimeAgent::enable(ErrorString* errorString)
{
    if (m_enabled)
        return;

    InspectorRuntimeAgent::enable(errorString);
    ScriptState* scriptState = m_workerGlobalScope->script()->scriptState();
    int executionContextId = injectedScriptManager()->injectedScriptIdFor(scriptState);
    addExecutionContextToFrontend(executionContextId, "", m_workerGlobalScope->url(), "", "");
}

InjectedScript WorkerRuntimeAgent::injectedScriptForEval(ErrorString* error, const int* executionContextId)
{
    if (!executionContextId)
        return injectedScriptManager()->injectedScriptFor(m_workerGlobalScope->script()->scriptState());

    InjectedScript injectedScript = injectedScriptManager()->injectedScriptForId(*executionContextId);
    if (injectedScript.isEmpty())
        *error = "Execution context with given id not found.";
    return injectedScript;
}

void WorkerRuntimeAgent::muteConsole()
{
    // We don't need to mute console for workers.
}

void WorkerRuntimeAgent::unmuteConsole()
{
    // We don't need to mute console for workers.
}

} // namespace blink
