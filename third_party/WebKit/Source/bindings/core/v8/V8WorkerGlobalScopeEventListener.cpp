/*
 * Copyright (C) 2006, 2007, 2008, 2009 Google Inc. All rights reserved.
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

#include "bindings/core/v8/V8WorkerGlobalScopeEventListener.h"

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8DOMWrapper.h"
#include "bindings/core/v8/V8Event.h"
#include "bindings/core/v8/V8EventTarget.h"
#include "bindings/core/v8/V8GCController.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "bindings/core/v8/WorkerScriptController.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/workers/WorkerGlobalScope.h"

namespace blink {

V8WorkerGlobalScopeEventListener::V8WorkerGlobalScopeEventListener(v8::Local<v8::Object> listener, bool isInline, ScriptState* scriptState)
    : V8EventListener(listener, isInline, scriptState)
{
}

void V8WorkerGlobalScopeEventListener::handleEvent(ScriptState* scriptState, Event* event)
{
    // The callback function on XMLHttpRequest can clear the event listener and destroys 'this' object. Keep a local reference to it.
    // See issue 889829.
    RefPtr<V8AbstractEventListener> protect(this);

    WorkerScriptController* script = toWorkerGlobalScope(scriptState->executionContext())->script();
    if (!script)
        return;

    ScriptState::Scope scope(scriptState);

    // Get the V8 wrapper for the event object.
    v8::Local<v8::Value> jsEvent = toV8(event, scriptState->context()->Global(), isolate());
    if (jsEvent.IsEmpty())
        return;

    invokeEventHandler(scriptState, event, v8::Local<v8::Value>::New(isolate(), jsEvent));
}

v8::Local<v8::Value> V8WorkerGlobalScopeEventListener::callListenerFunction(ScriptState* scriptState, v8::Local<v8::Value> jsEvent, Event* event)
{
    ASSERT(!jsEvent.IsEmpty());
    v8::Local<v8::Function> handlerFunction = getListenerFunction(scriptState);
    v8::Local<v8::Object> receiver = getReceiverObject(scriptState, event);
    if (handlerFunction.IsEmpty() || receiver.IsEmpty())
        return v8::Local<v8::Value>();

    InspectorInstrumentationCookie cookie = InspectorInstrumentation::willCallFunction(scriptState->executionContext(), DevToolsFunctionInfo(handlerFunction));

    v8::Local<v8::Value> parameters[1] = { jsEvent };
    v8::MaybeLocal<v8::Value> maybeResult = V8ScriptRunner::callFunction(handlerFunction, scriptState->executionContext(), receiver, WTF_ARRAY_LENGTH(parameters), parameters, isolate());

    InspectorInstrumentation::didCallFunction(cookie);
    TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "UpdateCounters", TRACE_EVENT_SCOPE_THREAD, "data", InspectorUpdateCountersEvent::data());

    v8::Local<v8::Value> result;
    if (!maybeResult.ToLocal(&result))
        return v8::Local<v8::Value>();
    return result;
}

// FIXME: Remove getReceiverObject().
// This is almost identical to V8AbstractEventListener::getReceiverObject().
v8::Local<v8::Object> V8WorkerGlobalScopeEventListener::getReceiverObject(ScriptState* scriptState, Event* event)
{
    v8::Local<v8::Object> listener = getListenerObject(scriptState->executionContext());

    if (!listener.IsEmpty() && !listener->IsFunction())
        return listener;

    EventTarget* target = event->currentTarget();
    v8::Local<v8::Value> value = toV8(target, scriptState->context()->Global(), isolate());
    if (value.IsEmpty())
        return v8::Local<v8::Object>();
    return v8::Local<v8::Object>::New(isolate(), v8::Local<v8::Object>::Cast(value));
}

} // namespace blink
