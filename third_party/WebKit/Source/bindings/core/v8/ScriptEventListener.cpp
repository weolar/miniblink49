/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "bindings/core/v8/ScriptEventListener.h"

#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/V8AbstractEventListener.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/WindowProxy.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentParser.h"
#include "core/dom/QualifiedName.h"
#include "core/events/EventListener.h"
#include "core/frame/LocalFrame.h"
#include <v8.h>

namespace blink {

PassRefPtr<V8LazyEventListener> createAttributeEventListener(Node* node, const QualifiedName& name, const AtomicString& value, const AtomicString& eventParameterName)
{
    ASSERT(node);
    if (value.isNull())
        return nullptr;

    // FIXME: Very strange: we initialize zero-based number with '1'.
    TextPosition position(OrdinalNumber::fromZeroBasedInt(1), OrdinalNumber::first());
    String sourceURL;

    v8::Isolate* isolate;
    if (LocalFrame* frame = node->document().frame()) {
        isolate = toIsolate(frame);
        ScriptController& scriptController = frame->script();
        if (!scriptController.canExecuteScripts(AboutToExecuteScript))
            return nullptr;
        position = scriptController.eventHandlerPosition();
        sourceURL = node->document().url().string();
    } else {
        isolate = v8::Isolate::GetCurrent();
    }

    return V8LazyEventListener::create(name.localName(), eventParameterName, value, sourceURL, position, node, isolate);
}

PassRefPtr<V8LazyEventListener> createAttributeEventListener(LocalFrame* frame, const QualifiedName& name, const AtomicString& value, const AtomicString& eventParameterName)
{
    if (!frame)
        return nullptr;

    if (value.isNull())
        return nullptr;

    ScriptController& scriptController = frame->script();
    if (!scriptController.canExecuteScripts(AboutToExecuteScript))
        return nullptr;

    TextPosition position = scriptController.eventHandlerPosition();
    String sourceURL = frame->document()->url().string();

    return V8LazyEventListener::create(name.localName(), eventParameterName, value, sourceURL, position, 0, toIsolate(frame));
}

static v8::MaybeLocal<v8::Function> eventListenerEffectiveFunction(v8::Isolate* isolate, v8::Local<v8::Context> context, v8::Local<v8::Object> listenerObject)
{
    if (listenerObject->IsFunction()) {
        return v8::MaybeLocal<v8::Function>(listenerObject.As<v8::Function>());
    } else if (listenerObject->IsObject()) {
        // Try the "handleEvent" method (EventListener interface).
        v8::Local<v8::Value> property;
        if (listenerObject->Get(context, v8AtomicString(isolate, "handleEvent")).ToLocal(&property) && property->IsFunction())
            return v8::MaybeLocal<v8::Function>(property.As<v8::Function>());
        // Fall back to the "constructor" property.
        if (listenerObject->Get(context, v8AtomicString(isolate, "constructor")).ToLocal(&property) && property->IsFunction())
            return v8::MaybeLocal<v8::Function>(property.As<v8::Function>());
    }
    return v8::MaybeLocal<v8::Function>();
}

ScriptValue eventListenerHandler(ExecutionContext* executionContext, EventListener* listener)
{
    if (listener->type() != EventListener::JSEventListenerType)
        return ScriptValue();

    v8::Isolate* isolate = toIsolate(executionContext);
    v8::HandleScope scope(isolate);
    V8AbstractEventListener* v8Listener = static_cast<V8AbstractEventListener*>(listener);
    v8::Local<v8::Context> context = toV8Context(executionContext, v8Listener->world());
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> function = v8Listener->getListenerObject(executionContext);
    if (function.IsEmpty())
        return ScriptValue();
    return ScriptValue(ScriptState::from(context), function);
}

ScriptState* eventListenerHandlerScriptState(LocalFrame* frame, EventListener* listener)
{
    if (listener->type() != EventListener::JSEventListenerType)
        return 0;
    V8AbstractEventListener* v8Listener = static_cast<V8AbstractEventListener*>(listener);
    v8::HandleScope scope(toIsolate(frame));
    v8::Local<v8::Context> v8Context = frame->script().windowProxy(v8Listener->world())->context();
    return ScriptState::from(v8Context);
}

bool eventListenerHandlerLocation(ExecutionContext* executionContext, EventListener* listener, String& scriptId, int& lineNumber, int& columnNumber)
{
    if (listener->type() != EventListener::JSEventListenerType)
        return false;

    v8::HandleScope scope(toIsolate(executionContext));
    V8AbstractEventListener* v8Listener = static_cast<V8AbstractEventListener*>(listener);
    v8::Local<v8::Context> context = toV8Context(executionContext, v8Listener->world());
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> object = v8Listener->getListenerObject(executionContext);
    if (object.IsEmpty())
        return false;
    v8::Local<v8::Function> function;
    if (!eventListenerEffectiveFunction(scope.GetIsolate(), context, object).ToLocal(&function))
        return false;
    v8::Local<v8::Function> originalFunction = getBoundFunction(function);
    int scriptIdValue = originalFunction->ScriptId();
    scriptId = String::number(scriptIdValue);
    lineNumber = originalFunction->GetScriptLineNumber();
    columnNumber = originalFunction->GetScriptColumnNumber();
    return true;
}

} // namespace blink
