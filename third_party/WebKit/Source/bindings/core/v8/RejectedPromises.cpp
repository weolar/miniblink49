// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/RejectedPromises.h"

#include "bindings/core/v8/ScopedPersistent.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8Binding.h"
#include "core/dom/ExecutionContext.h"
#include "core/events/EventTarget.h"
#include "core/events/PromiseRejectionEvent.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/ScriptArguments.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

static const unsigned maxReportedHandlersPendingResolution = 1000;

class RejectedPromises::Message final : public NoBaseWillBeGarbageCollectedFinalized<RejectedPromises::Message> {
public:
    static PassOwnPtrWillBeRawPtr<Message> create(ScriptState* scriptState, v8::Local<v8::Promise> promise, const ScriptValue& exception, const String& errorMessage, const String& resourceName, int scriptId, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack)
    {
        return adoptPtrWillBeNoop(new Message(scriptState, promise, exception, errorMessage, resourceName, scriptId, lineNumber, columnNumber, callStack));
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_callStack);
    }

    bool isCollected()
    {
        return m_collected || !m_scriptState->contextIsValid();
    }

    bool hasPromise(v8::Local<v8::Value> promise)
    {
        ScriptState::Scope scope(m_scriptState);
        return promise == m_promise.newLocal(m_scriptState->isolate());
    }

    void report()
    {
        if (!m_scriptState->contextIsValid())
            return;
        // If execution termination has been triggered, quietly bail out.
        if (v8::V8::IsExecutionTerminating(m_scriptState->isolate()))
            return;
        ExecutionContext* executionContext = m_scriptState->executionContext();
        if (!executionContext)
            return;

        ScriptState::Scope scope(m_scriptState);
        v8::Local<v8::Value> value = m_promise.newLocal(m_scriptState->isolate());
        // Either collected or https://crbug.com/450330
        if (value.IsEmpty() || !value->IsPromise())
            return;
        ASSERT(!v8::Local<v8::Promise>::Cast(value)->HasHandler());

        EventTarget* target = executionContext->errorEventTarget();
        if (RuntimeEnabledFeatures::promiseRejectionEventEnabled() && target) {
            PromiseRejectionEventInit init;
            init.setPromise(ScriptPromise(m_scriptState, value));
            init.setReason(m_exception);
            init.setCancelable(true);
            RefPtrWillBeRawPtr<PromiseRejectionEvent> event = PromiseRejectionEvent::create(m_scriptState, EventTypeNames::unhandledrejection, init);
            // Log to console if event was not preventDefault()'ed.
            m_shouldLogToConsole = target->dispatchEvent(event);
        }

        if (m_shouldLogToConsole) {
            const String errorMessage = "Uncaught (in promise)";
            Vector<ScriptValue> args;
            args.append(ScriptValue(m_scriptState, v8String(m_scriptState->isolate(), errorMessage)));
            args.append(m_exception);
            RefPtrWillBeRawPtr<ScriptArguments> arguments = ScriptArguments::create(m_scriptState, args);

            String embedderErrorMessage = m_errorMessage;
            if (embedderErrorMessage.isEmpty())
                embedderErrorMessage = errorMessage;
            else if (embedderErrorMessage.startsWith("Uncaught "))
                embedderErrorMessage.insert(" (in promise)", 8);

            RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = ConsoleMessage::create(JSMessageSource, ErrorMessageLevel, embedderErrorMessage, m_resourceName, m_lineNumber, m_columnNumber);
            consoleMessage->setScriptArguments(arguments);
            consoleMessage->setCallStack(m_callStack);
            consoleMessage->setScriptId(m_scriptId);
            m_consoleMessageId = consoleMessage->assignMessageId();
            executionContext->addConsoleMessage(consoleMessage.release());
        }

        m_callStack.clear();
    }

    void revoke()
    {
        ExecutionContext* executionContext = m_scriptState->executionContext();
        if (!executionContext)
            return;

        ScriptState::Scope scope(m_scriptState);
        v8::Local<v8::Value> value = m_promise.newLocal(m_scriptState->isolate());
        // Either collected or https://crbug.com/450330
        if (value.IsEmpty() || !value->IsPromise())
            return;

        EventTarget* target = executionContext->errorEventTarget();
        if (RuntimeEnabledFeatures::promiseRejectionEventEnabled() && target) {
            PromiseRejectionEventInit init;
            init.setPromise(ScriptPromise(m_scriptState, value));
            init.setReason(m_exception);
            RefPtrWillBeRawPtr<PromiseRejectionEvent> event = PromiseRejectionEvent::create(m_scriptState, EventTypeNames::rejectionhandled, init);
            target->dispatchEvent(event);
        }

        if (m_shouldLogToConsole) {
            RefPtrWillBeRawPtr<ConsoleMessage> consoleMessage = ConsoleMessage::create(JSMessageSource, RevokedErrorMessageLevel, "Handler added to rejected promise");
            consoleMessage->setRelatedMessageId(m_consoleMessageId);
            executionContext->addConsoleMessage(consoleMessage.release());
        }
    }

private:
    Message(ScriptState* scriptState, v8::Local<v8::Promise> promise, const ScriptValue& exception, const String& errorMessage, const String& resourceName, int scriptId, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack)
        : m_scriptState(scriptState)
        , m_promise(scriptState->isolate(), promise)
        , m_exception(exception)
        , m_errorMessage(errorMessage)
        , m_resourceName(resourceName)
        , m_scriptId(scriptId)
        , m_lineNumber(lineNumber)
        , m_columnNumber(columnNumber)
        , m_callStack(callStack)
        , m_consoleMessageId(0)
        , m_collected(false)
        , m_shouldLogToConsole(true)
    {
        m_promise.setWeak(this, &Message::didCollectPromise);
    }

    static void didCollectPromise(const v8::WeakCallbackInfo<Message>& data)
    {
        data.GetParameter()->m_collected = true;
        data.GetParameter()->m_promise.clear();
    }

    ScriptState* m_scriptState;
    ScopedPersistent<v8::Promise> m_promise;
    ScriptValue m_exception;
    String m_errorMessage;
    String m_resourceName;
    int m_scriptId;
    int m_lineNumber;
    int m_columnNumber;
    RefPtrWillBeMember<ScriptCallStack> m_callStack;
    unsigned m_consoleMessageId;
    bool m_collected;
    bool m_shouldLogToConsole;
};

RejectedPromises::RejectedPromises()
{
}

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(RejectedPromises);

DEFINE_TRACE(RejectedPromises)
{
    visitor->trace(m_queue);
    visitor->trace(m_reportedAsErrors);
}

void RejectedPromises::rejectedWithNoHandler(ScriptState* scriptState, v8::PromiseRejectMessage data, const String& errorMessage, const String& resourceName, int scriptId, int lineNumber, int columnNumber, PassRefPtrWillBeRawPtr<ScriptCallStack> callStack)
{
    m_queue.append(Message::create(scriptState, data.GetPromise(), ScriptValue(scriptState, data.GetValue()), errorMessage, resourceName, scriptId, lineNumber, columnNumber, callStack));
}

void RejectedPromises::handlerAdded(v8::PromiseRejectMessage data)
{
    // First look it up in the pending messages and fast return, it'll be covered by processQueue().
    for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
        if (!(*it)->isCollected() && (*it)->hasPromise(data.GetPromise())) {
            m_queue.remove(it);
            return;
        }
    }

    // Then look it up in the reported errors.
    for (size_t i = 0; i < m_reportedAsErrors.size(); ++i) {
        auto& message = m_reportedAsErrors.at(i);
        if (!message->isCollected() && message->hasPromise(data.GetPromise())) {
            message->revoke();
            m_reportedAsErrors.remove(i);
            return;
        }
    }
}

void RejectedPromises::dispose()
{
    processQueue();
}

void RejectedPromises::processQueue()
{
    // Remove collected handlers.
    for (size_t i = 0; i < m_reportedAsErrors.size();) {
        if (m_reportedAsErrors.at(i)->isCollected())
            m_reportedAsErrors.remove(i);
        else
            ++i;
    }

    while (!m_queue.isEmpty()) {
        OwnPtrWillBeRawPtr<Message> message = m_queue.takeFirst();
        if (message->isCollected())
            continue;

        message->report();
        m_reportedAsErrors.append(message.release());
        if (m_reportedAsErrors.size() > maxReportedHandlersPendingResolution)
            m_reportedAsErrors.remove(0, maxReportedHandlersPendingResolution / 10);
    }
}

} // namespace blink
