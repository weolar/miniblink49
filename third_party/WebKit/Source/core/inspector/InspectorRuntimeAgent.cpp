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
#include "core/inspector/InspectorRuntimeAgent.h"

#include "bindings/core/v8/ScriptState.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InjectedScriptManager.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/V8Debugger.h"
#include "platform/JSONValues.h"

using blink::TypeBuilder::Runtime::ExecutionContextDescription;

namespace blink {

namespace InspectorRuntimeAgentState {
static const char runtimeEnabled[] = "runtimeEnabled";
};

class InspectorRuntimeAgent::InjectedScriptCallScope {
public:
    InjectedScriptCallScope(InspectorRuntimeAgent* agent, bool doNotPauseOnExceptionsAndMuteConsole)
        : m_agent(agent)
        , m_doNotPauseOnExceptionsAndMuteConsole(doNotPauseOnExceptionsAndMuteConsole)
        , m_previousPauseOnExceptionsState(V8Debugger::DontPauseOnExceptions)
    {
        if (!m_doNotPauseOnExceptionsAndMuteConsole)
            return;
        m_previousPauseOnExceptionsState = setPauseOnExceptionsState(V8Debugger::DontPauseOnExceptions);
        m_agent->muteConsole();
    }
    ~InjectedScriptCallScope()
    {
        if (!m_doNotPauseOnExceptionsAndMuteConsole)
            return;
        m_agent->unmuteConsole();
        setPauseOnExceptionsState(m_previousPauseOnExceptionsState);
    }

private:
    V8Debugger::PauseOnExceptionsState setPauseOnExceptionsState(V8Debugger::PauseOnExceptionsState newState)
    {
        V8Debugger* debugger = m_agent->m_debugger;
        ASSERT(debugger);
        if (!debugger->enabled())
            return newState;
        V8Debugger::PauseOnExceptionsState presentState = debugger->pauseOnExceptionsState();
        if (presentState != newState)
            debugger->setPauseOnExceptionsState(newState);
        return presentState;
    }

    InspectorRuntimeAgent* m_agent;
    bool m_doNotPauseOnExceptionsAndMuteConsole;
    V8Debugger::PauseOnExceptionsState m_previousPauseOnExceptionsState;
};

InspectorRuntimeAgent::InspectorRuntimeAgent(InjectedScriptManager* injectedScriptManager, V8Debugger* debugger, Client* client)
    : InspectorBaseAgent<InspectorRuntimeAgent, InspectorFrontend::Runtime>("Runtime")
    , m_enabled(false)
    , m_injectedScriptManager(injectedScriptManager)
    , m_debugger(debugger)
    , m_client(client)
{
}

InspectorRuntimeAgent::~InspectorRuntimeAgent()
{
}

DEFINE_TRACE(InspectorRuntimeAgent)
{
    visitor->trace(m_injectedScriptManager);
    InspectorBaseAgent::trace(visitor);
}

void InspectorRuntimeAgent::evaluate(ErrorString* errorString, const String& expression, const String* const objectGroup, const bool* const includeCommandLineAPI, const bool* const doNotPauseOnExceptionsAndMuteConsole, const int* executionContextId, const bool* const returnByValue, const bool* generatePreview, RefPtr<TypeBuilder::Runtime::RemoteObject>& result, TypeBuilder::OptOutput<bool>* wasThrown, RefPtr<TypeBuilder::Debugger::ExceptionDetails>& exceptionDetails)
{
    InjectedScript injectedScript = injectedScriptForEval(errorString, executionContextId);
    if (injectedScript.isEmpty())
        return;

    InjectedScriptCallScope callScope(this, asBool(doNotPauseOnExceptionsAndMuteConsole));
    injectedScript.evaluate(errorString, expression, objectGroup ? *objectGroup : "", asBool(includeCommandLineAPI), asBool(returnByValue), asBool(generatePreview), &result, wasThrown, &exceptionDetails);
}

void InspectorRuntimeAgent::callFunctionOn(ErrorString* errorString, const String& objectId, const String& expression, const RefPtr<JSONArray>* const optionalArguments, const bool* const doNotPauseOnExceptionsAndMuteConsole, const bool* const returnByValue, const bool* generatePreview, RefPtr<TypeBuilder::Runtime::RemoteObject>& result, TypeBuilder::OptOutput<bool>* wasThrown)
{
    InjectedScript injectedScript = m_injectedScriptManager->injectedScriptForObjectId(objectId);
    if (injectedScript.isEmpty()) {
        *errorString = "Inspected frame has gone";
        return;
    }
    String arguments;
    if (optionalArguments)
        arguments = (*optionalArguments)->toJSONString();

    InjectedScriptCallScope callScope(this, asBool(doNotPauseOnExceptionsAndMuteConsole));
    injectedScript.callFunctionOn(errorString, objectId, expression, arguments, asBool(returnByValue), asBool(generatePreview), &result, wasThrown);
}

void InspectorRuntimeAgent::getProperties(ErrorString* errorString, const String& objectId, const bool* ownProperties, const bool* accessorPropertiesOnly, const bool* generatePreview, RefPtr<TypeBuilder::Array<TypeBuilder::Runtime::PropertyDescriptor>>& result, RefPtr<TypeBuilder::Array<TypeBuilder::Runtime::InternalPropertyDescriptor>>& internalProperties, RefPtr<TypeBuilder::Debugger::ExceptionDetails>& exceptionDetails)
{
    InjectedScript injectedScript = m_injectedScriptManager->injectedScriptForObjectId(objectId);
    if (injectedScript.isEmpty()) {
        *errorString = "Inspected frame has gone";
        return;
    }

    InjectedScriptCallScope callScope(this, true);

    injectedScript.getProperties(errorString, objectId, asBool(ownProperties), asBool(accessorPropertiesOnly), asBool(generatePreview), &result, &exceptionDetails);

    if (!exceptionDetails && !asBool(accessorPropertiesOnly))
        injectedScript.getInternalProperties(errorString, objectId, &internalProperties, &exceptionDetails);
}

void InspectorRuntimeAgent::releaseObject(ErrorString*, const String& objectId)
{
    InjectedScript injectedScript = m_injectedScriptManager->injectedScriptForObjectId(objectId);
    if (injectedScript.isEmpty())
        return;
    bool pausingOnNextStatement = m_debugger->pausingOnNextStatement();
    if (pausingOnNextStatement)
        m_debugger->setPauseOnNextStatement(false);
    injectedScript.releaseObject(objectId);
    if (pausingOnNextStatement)
        m_debugger->setPauseOnNextStatement(true);
}

void InspectorRuntimeAgent::releaseObjectGroup(ErrorString*, const String& objectGroup)
{
    bool pausingOnNextStatement = m_debugger->pausingOnNextStatement();
    if (pausingOnNextStatement)
        m_debugger->setPauseOnNextStatement(false);
    m_injectedScriptManager->releaseObjectGroup(objectGroup);
    if (pausingOnNextStatement)
        m_debugger->setPauseOnNextStatement(true);
}

void InspectorRuntimeAgent::run(ErrorString*)
{
    m_client->resumeStartup();
}

void InspectorRuntimeAgent::isRunRequired(ErrorString*, bool* out_result)
{
    *out_result = m_client->isRunRequired();
}

void InspectorRuntimeAgent::setCustomObjectFormatterEnabled(ErrorString*, bool enabled)
{
    injectedScriptManager()->setCustomObjectFormatterEnabled(enabled);
}

void InspectorRuntimeAgent::restore()
{
    if (m_state->getBoolean(InspectorRuntimeAgentState::runtimeEnabled)) {
        frontend()->executionContextsCleared();
        String error;
        enable(&error);
    }
}

void InspectorRuntimeAgent::enable(ErrorString* errorString)
{
    if (m_enabled)
        return;

    m_enabled = true;
    m_state->setBoolean(InspectorRuntimeAgentState::runtimeEnabled, true);
}

void InspectorRuntimeAgent::disable(ErrorString* errorString)
{
    if (!m_enabled)
        return;

    m_enabled = false;
    m_state->setBoolean(InspectorRuntimeAgentState::runtimeEnabled, false);
}

void InspectorRuntimeAgent::addExecutionContextToFrontend(int executionContextId, const String& type, const String& origin, const String& humanReadableName, const String& frameId)
{
    RefPtr<ExecutionContextDescription> description = ExecutionContextDescription::create()
        .setId(executionContextId)
        .setName(humanReadableName)
        .setOrigin(origin)
        .setFrameId(frameId);
    if (!type.isEmpty())
        description->setType(type);
    frontend()->executionContextCreated(description.release());
}

} // namespace blink
