// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/V8ObjectBuilder.h"

#include "bindings/core/v8/V8Binding.h"

namespace blink {

V8ObjectBuilder::V8ObjectBuilder(ScriptState* scriptState)
    : m_scriptState(scriptState)
    , m_object(v8::Object::New(scriptState->isolate()))
{
}

V8ObjectBuilder& V8ObjectBuilder::add(const String& name, const V8ObjectBuilder& value)
{
    addInternal(name, value.v8Value());
    return *this;
}

V8ObjectBuilder& V8ObjectBuilder::addNull(const String& name)
{
    addInternal(name, v8::Null(m_scriptState->isolate()));
    return *this;
}

V8ObjectBuilder& V8ObjectBuilder::addBoolean(const String& name, bool value)
{
    addInternal(name, value ? v8::True(m_scriptState->isolate()) : v8::False(m_scriptState->isolate()));
    return *this;
}

V8ObjectBuilder& V8ObjectBuilder::addNumber(const String& name, double value)
{
    addInternal(name, v8::Number::New(m_scriptState->isolate(), value));
    return *this;
}

V8ObjectBuilder& V8ObjectBuilder::addString(const String& name, const String& value)
{
    addInternal(name, v8String(m_scriptState->isolate(), value));
    return *this;
}

ScriptValue V8ObjectBuilder::scriptValue() const
{
    return ScriptValue(m_scriptState.get(), m_object);
}

void V8ObjectBuilder::addInternal(const String& name, v8::Local<v8::Value> value)
{
    if (m_object.IsEmpty())
        return;
    if (value.IsEmpty() || m_object->CreateDataProperty(m_scriptState->context(), v8String(m_scriptState->isolate(), name), value).IsNothing())
        m_object.Clear();
}

} // namespace blink
