/*
 * Copyright (C) 2008, 2009, 2011 Google Inc. All rights reserved.
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
#include "bindings/core/v8/ScriptValue.h"

#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "bindings/core/v8/V8Binding.h"
#include "platform/JSONValues.h"

namespace blink {

v8::Local<v8::Value> ScriptValue::v8Value() const
{
    if (isEmpty())
        return v8::Local<v8::Value>();

    ASSERT(isolate()->InContext());

    // This is a check to validate that you don't return a ScriptValue to a world different
    // from the world that created the ScriptValue.
    // Probably this could be:
    //   if (&m_scriptState->world() == &DOMWrapperWorld::current(isolate()))
    //       return v8::Local<v8::Value>();
    // instead of triggering RELEASE_ASSERT.
    RELEASE_ASSERT(&m_scriptState->world() == &DOMWrapperWorld::current(isolate()));
    return m_value->newLocal(isolate());
}

v8::Local<v8::Value> ScriptValue::v8ValueUnsafe() const
{
    if (isEmpty())
        return v8::Local<v8::Value>();
    return m_value->newLocal(isolate());
}

v8::Local<v8::Value> ScriptValue::v8ValueFor(ScriptState* targetScriptState)
{
    if (isEmpty())
        return v8::Local<v8::Value>();
    v8::Isolate* isolate = targetScriptState->isolate();
    if (&m_scriptState->world() == &targetScriptState->world())
        return m_value->newLocal(isolate);

    ASSERT(isolate->InContext());
    v8::Local<v8::Value> value = m_value->newLocal(isolate);
    RefPtr<SerializedScriptValue> serialized = SerializedScriptValueFactory::instance().createAndSwallowExceptions(isolate, value);
    return serialized->deserialize();
}

bool ScriptValue::toString(String& result) const
{
    if (isEmpty())
        return false;

    ScriptState::Scope scope(m_scriptState.get());
    v8::Local<v8::Value> string = v8Value();
    if (string.IsEmpty() || !string->IsString())
        return false;
    result = toCoreString(v8::Local<v8::String>::Cast(string));
    return true;
}

ScriptValue ScriptValue::createNull(ScriptState* scriptState)
{
    return ScriptValue(scriptState, v8::Null(scriptState->isolate()));
}

} // namespace blink
