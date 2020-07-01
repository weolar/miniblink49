/*
 * Copyright (C) 2008, 2009 Google Inc. All rights reserved.
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
#include "bindings/core/v8/V8NodeFilterCondition.h"

#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "bindings/core/v8/V8Node.h"
#include "core/dom/Node.h"
#include "core/dom/NodeFilter.h"
#include "wtf/OwnPtr.h"

namespace blink {

V8NodeFilterCondition::V8NodeFilterCondition(v8::Local<v8::Value> filter, v8::Local<v8::Object> owner, ScriptState* scriptState)
    : m_scriptState(scriptState)
{
    // ..acceptNode(..) will only dispatch m_filter if m_filter->IsObject().
    // We'll make sure m_filter is either usable by acceptNode or empty.
    // (See the fast/dom/node-filter-gc test for a case where 'empty' happens.)
    if (!filter.IsEmpty() && filter->IsObject()) {
        V8HiddenValue::setHiddenValue(scriptState->isolate(), owner, V8HiddenValue::condition(scriptState->isolate()), filter);
        m_filter.set(scriptState->isolate(), filter);
        m_filter.setWeak(this, &setWeakCallback);
    }
}

V8NodeFilterCondition::~V8NodeFilterCondition()
{
}

unsigned V8NodeFilterCondition::acceptNode(Node* node, ExceptionState& exceptionState) const
{
    v8::Isolate* isolate = m_scriptState->isolate();
    ASSERT(!m_scriptState->context().IsEmpty());
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Value> filter = m_filter.newLocal(isolate);

    ASSERT(filter.IsEmpty() || filter->IsObject());
    if (filter.IsEmpty())
        return NodeFilter::FILTER_ACCEPT;


    v8::TryCatch exceptionCatcher(isolate);
    v8::Local<v8::Function> callback;
    v8::Local<v8::Value> receiver;
    if (filter->IsFunction()) {
        callback = v8::Local<v8::Function>::Cast(filter);
        receiver = v8::Undefined(isolate);
    } else {
        v8::Local<v8::Object> filterObject;
        if (!filter->ToObject(m_scriptState->context()).ToLocal(&filterObject)) {
            exceptionState.throwTypeError("NodeFilter is not an object");
            return NodeFilter::FILTER_REJECT;
        }
        v8::Local<v8::Value> value;
        if (!filterObject->Get(m_scriptState->context(), v8AtomicString(isolate, "acceptNode")).ToLocal(&value) || !value->IsFunction()) {
            exceptionState.throwTypeError("NodeFilter object does not have an acceptNode function");
            return NodeFilter::FILTER_REJECT;
        }
        callback = v8::Local<v8::Function>::Cast(value);
        receiver = filter;
    }

    OwnPtr<v8::Local<v8::Value>[]> info = adoptArrayPtr(new v8::Local<v8::Value>[1]);
    info[0] = toV8(node, m_scriptState->context()->Global(), isolate);
    if (info[0].IsEmpty()) {
        if (exceptionCatcher.HasCaught())
            exceptionState.rethrowV8Exception(exceptionCatcher.Exception());
        return NodeFilter::FILTER_REJECT;
    }

    v8::Local<v8::Value> result;
    if (!ScriptController::callFunction(m_scriptState->executionContext(), callback, receiver, 1, info.get(), isolate).ToLocal(&result)) {
        exceptionState.rethrowV8Exception(exceptionCatcher.Exception());
        return NodeFilter::FILTER_REJECT;
    }

    ASSERT(!result.IsEmpty());

    uint32_t uint32Value;
    if (!v8Call(result->Uint32Value(m_scriptState->context()), uint32Value, exceptionCatcher)) {
        exceptionState.rethrowV8Exception(exceptionCatcher.Exception());
        return NodeFilter::FILTER_REJECT;
    }
    return uint32Value;
}

void V8NodeFilterCondition::setWeakCallback(const v8::WeakCallbackInfo<V8NodeFilterCondition>& data)
{
    data.GetParameter()->m_filter.clear();
}

} // namespace blink
