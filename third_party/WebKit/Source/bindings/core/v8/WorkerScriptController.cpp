/*
 * Copyright (C) 2009, 2012 Google Inc. All rights reserved.
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

#include "bindings/core/v8/WorkerScriptController.h"

#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8DedicatedWorkerGlobalScope.h"
#include "bindings/core/v8/V8ErrorHandler.h"
#include "bindings/core/v8/V8Initializer.h"
#include "bindings/core/v8/V8ObjectConstructor.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "bindings/core/v8/V8SharedWorkerGlobalScope.h"
#include "bindings/core/v8/V8WorkerGlobalScope.h"
#include "bindings/core/v8/WrapperTypeInfo.h"
#include "core/events/ErrorEvent.h"
#include "core/frame/DOMTimer.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/inspector/WorkerThreadDebugger.h"
#include "core/workers/SharedWorkerGlobalScope.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/workers/WorkerObjectProxy.h"
#include "core/workers/WorkerThread.h"
#include "platform/heap/ThreadState.h"
#include "public/platform/Platform.h"
#include <v8.h>

namespace blink {

class WorkerScriptController::WorkerGlobalScopeExecutionState final {
    STACK_ALLOCATED();
public:
    explicit WorkerGlobalScopeExecutionState(WorkerScriptController* controller)
        : hadException(false)
        , lineNumber(0)
        , columnNumber(0)
        , m_controller(controller)
        , m_outerState(controller->m_globalScopeExecutionState)
    {
        m_controller->m_globalScopeExecutionState = this;
    }

    ~WorkerGlobalScopeExecutionState()
    {
        m_controller->m_globalScopeExecutionState = m_outerState;
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_errorEventFromImportedScript);
    }

    bool hadException;
    String errorMessage;
    int lineNumber;
    int columnNumber;
    String sourceURL;
    ScriptValue exception;
    RefPtrWillBeMember<ErrorEvent> m_errorEventFromImportedScript;

    // A WorkerGlobalScopeExecutionState context is stack allocated by
    // WorkerScriptController::evaluate(), with the contoller using it
    // during script evaluation. To handle nested evaluate() uses,
    // WorkerGlobalScopeExecutionStates are chained together;
    // |m_outerState| keeps a pointer to the context object one level out
    // (or 0, if outermost.) Upon return from evaluate(), the
    // WorkerScriptController's WorkerGlobalScopeExecutionState is popped
    // and the previous one restored (see above dtor.)
    //
    // With Oilpan, |m_outerState| isn't traced. It'll be "up the stack"
    // and its fields will be traced when scanning the stack.
    WorkerScriptController* m_controller;
    WorkerGlobalScopeExecutionState* m_outerState;
};

WorkerScriptController::WorkerScriptController(WorkerGlobalScope& workerGlobalScope, v8::Isolate* isolate)
    : m_workerGlobalScope(workerGlobalScope)
    , m_executionForbidden(false)
    , m_executionScheduledToTerminate(false)
    , m_rejectedPromises(RejectedPromises::create())
    , m_globalScopeExecutionState(0)
{
    ASSERT(isolate);
    m_world = DOMWrapperWorld::create(isolate, WorkerWorldId);
}

WorkerScriptController::~WorkerScriptController()
{
    m_rejectedPromises->dispose();
    m_rejectedPromises.clear();

    m_world->dispose();

    // The corresponding call to didStartRunLoop() is in WorkerThread::initialize().
    // See http://webkit.org/b/83104#c14 for why this is here.
    m_workerGlobalScope.thread()->didStopRunLoop();

    if (isContextInitialized())
        m_scriptState->disposePerContextData();
}

bool WorkerScriptController::initializeContextIfNeeded()
{
    v8::HandleScope handleScope(isolate());

    if (isContextInitialized())
        return true;

    v8::Local<v8::Context> context = v8::Context::New(isolate());
    if (context.IsEmpty())
        return false;

    m_scriptState = ScriptState::create(context, m_world);

    ScriptState::Scope scope(m_scriptState.get());

    // Name new context for debugging.
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WorkerThreadDebugger::setContextDebugData(context);
#endif // MINIBLINK_NOT_IMPLEMENTED

    // Create a new JS object and use it as the prototype for the shadow global object.
    const WrapperTypeInfo* wrapperTypeInfo = m_workerGlobalScope.wrapperTypeInfo();
    v8::Local<v8::Function> workerGlobalScopeConstructor = m_scriptState->perContextData()->constructorForType(wrapperTypeInfo);
    if (workerGlobalScopeConstructor.IsEmpty())
        return false;
    v8::Local<v8::Object> jsWorkerGlobalScope;
    if (!V8ObjectConstructor::newInstance(isolate(), workerGlobalScopeConstructor).ToLocal(&jsWorkerGlobalScope)) {
        m_scriptState->disposePerContextData();
        return false;
    }

    jsWorkerGlobalScope = V8DOMWrapper::associateObjectWithWrapper(isolate(), &m_workerGlobalScope, wrapperTypeInfo, jsWorkerGlobalScope);

    // Insert the object instance as the prototype of the shadow object.
    v8::Local<v8::Object> globalObject = v8::Local<v8::Object>::Cast(m_scriptState->context()->Global()->GetPrototype());
    return v8CallBoolean(globalObject->SetPrototype(context, jsWorkerGlobalScope));
}

v8::Isolate* WorkerScriptController::isolate() const
{
    return m_workerGlobalScope.thread()->isolate();
}

ScriptValue WorkerScriptController::evaluate(const String& script, const String& fileName, const TextPosition& scriptStartPosition, CachedMetadataHandler* cacheHandler, V8CacheOptions v8CacheOptions)
{
    if (!initializeContextIfNeeded())
        return ScriptValue();

    ScriptState::Scope scope(m_scriptState.get());

    if (!m_disableEvalPending.isEmpty()) {
        m_scriptState->context()->AllowCodeGenerationFromStrings(false);
        m_scriptState->context()->SetErrorMessageForCodeGenerationFromStrings(v8String(isolate(), m_disableEvalPending));
        m_disableEvalPending = String();
    }

    v8::TryCatch block;

    v8::Local<v8::Script> compiledScript;
    v8::MaybeLocal<v8::Value> maybeResult;
    if (v8Call(V8ScriptRunner::compileScript(script, fileName, String(), scriptStartPosition, isolate(), cacheHandler, SharableCrossOrigin, v8CacheOptions), compiledScript, block))
        maybeResult = V8ScriptRunner::runCompiledScript(isolate(), compiledScript, &m_workerGlobalScope);

    if (!block.CanContinue()) {
        forbidExecution();
        return ScriptValue();
    }

    if (block.HasCaught()) {
        v8::Local<v8::Message> message = block.Message();
        m_globalScopeExecutionState->hadException = true;
        m_globalScopeExecutionState->errorMessage = toCoreString(message->Get());
        if (v8Call(message->GetLineNumber(m_scriptState->context()), m_globalScopeExecutionState->lineNumber)
            && v8Call(message->GetStartColumn(m_scriptState->context()), m_globalScopeExecutionState->columnNumber)) {
            ++m_globalScopeExecutionState->columnNumber;
        } else {
            m_globalScopeExecutionState->lineNumber = 0;
            m_globalScopeExecutionState->columnNumber = 0;
        }

        TOSTRING_DEFAULT(V8StringResource<>, sourceURL, message->GetScriptOrigin().ResourceName(), ScriptValue());
        m_globalScopeExecutionState->sourceURL = sourceURL;
        m_globalScopeExecutionState->exception = ScriptValue(m_scriptState.get(), block.Exception());
        block.Reset();
    } else {
        m_globalScopeExecutionState->hadException = false;
    }

    v8::Local<v8::Value> result;
    if (!maybeResult.ToLocal(&result) || result->IsUndefined())
        return ScriptValue();

    return ScriptValue(m_scriptState.get(), result);
}

bool WorkerScriptController::evaluate(const ScriptSourceCode& sourceCode, RefPtrWillBeRawPtr<ErrorEvent>* errorEvent, CachedMetadataHandler* cacheHandler, V8CacheOptions v8CacheOptions)
{
    if (isExecutionForbidden())
        return false;

    WorkerGlobalScopeExecutionState state(this);
    evaluate(sourceCode.source(), sourceCode.url().string(), sourceCode.startPosition(), cacheHandler, v8CacheOptions);
    if (isExecutionForbidden())
        return false;
    if (state.hadException) {
        if (errorEvent) {
            if (state.m_errorEventFromImportedScript) {
                // Propagate inner error event outwards.
                *errorEvent = state.m_errorEventFromImportedScript.release();
                return false;
            }
            if (m_workerGlobalScope.shouldSanitizeScriptError(state.sourceURL, NotSharableCrossOrigin))
                *errorEvent = ErrorEvent::createSanitizedError(m_world.get());
            else
                *errorEvent = ErrorEvent::create(state.errorMessage, state.sourceURL, state.lineNumber, state.columnNumber, m_world.get());
            V8ErrorHandler::storeExceptionOnErrorEventWrapper(isolate(), errorEvent->get(), state.exception.v8Value(), m_scriptState->context()->Global());
        } else {
            ASSERT(!m_workerGlobalScope.shouldSanitizeScriptError(state.sourceURL, NotSharableCrossOrigin));
            RefPtrWillBeRawPtr<ErrorEvent> event = nullptr;
            if (state.m_errorEventFromImportedScript)
                event = state.m_errorEventFromImportedScript.release();
            else
                event = ErrorEvent::create(state.errorMessage, state.sourceURL, state.lineNumber, state.columnNumber, m_world.get());
            m_workerGlobalScope.reportException(event, 0, nullptr, NotSharableCrossOrigin);
        }
        return false;
    }
    return true;
}

void WorkerScriptController::willScheduleExecutionTermination()
{
    // The mutex provides a memory barrier to ensure that once
    // termination is scheduled, isExecutionTerminating will
    // accurately reflect that state when called from another thread.
    MutexLocker locker(m_scheduledTerminationMutex);
    m_executionScheduledToTerminate = true;
}

bool WorkerScriptController::isExecutionTerminating() const
{
    // See comments in willScheduleExecutionTermination regarding mutex usage.
    MutexLocker locker(m_scheduledTerminationMutex);
    return m_executionScheduledToTerminate;
}

void WorkerScriptController::forbidExecution()
{
    ASSERT(m_workerGlobalScope.isContextThread());
    m_executionForbidden = true;
}

bool WorkerScriptController::isExecutionForbidden() const
{
    ASSERT(m_workerGlobalScope.isContextThread());
    return m_executionForbidden;
}

void WorkerScriptController::disableEval(const String& errorMessage)
{
    m_disableEvalPending = errorMessage;
}

void WorkerScriptController::rethrowExceptionFromImportedScript(PassRefPtrWillBeRawPtr<ErrorEvent> errorEvent, ExceptionState& exceptionState)
{
    const String& errorMessage = errorEvent->message();
    if (m_globalScopeExecutionState)
        m_globalScopeExecutionState->m_errorEventFromImportedScript = errorEvent;
    exceptionState.rethrowV8Exception(V8ThrowException::createGeneralError(isolate(), errorMessage));
}

} // namespace blink
