// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/V8BindingForTesting.h"

#include "bindings/core/v8/DOMWrapperWorld.h"

namespace blink {

PassRefPtr<ScriptStateForTesting> ScriptStateForTesting::create(v8::Local<v8::Context> context, PassRefPtr<DOMWrapperWorld> world)
{
    RefPtr<ScriptStateForTesting> scriptState = adoptRef(new ScriptStateForTesting(context, world));
    // This ref() is for keeping this ScriptState alive as long as the v8::Context is alive.
    // This is deref()ed in the weak callback of the v8::Context.
    scriptState->ref();
    return scriptState;
}

ScriptStateForTesting::ScriptStateForTesting(v8::Local<v8::Context> context, PassRefPtr<DOMWrapperWorld> world)
    : ScriptState(context, world)
{
}

ExecutionContext* ScriptStateForTesting::executionContext() const
{
    return m_executionContext;
}

void ScriptStateForTesting::setExecutionContext(ExecutionContext* executionContext)
{
    m_executionContext = executionContext;
}

V8TestingScope::V8TestingScope(v8::Isolate* isolate)
    : m_handleScope(isolate)
    , m_contextScope(v8::Context::New(isolate))
    // We reuse the main world since the main world is guaranteed to be registered to ScriptController.
    , m_scriptState(ScriptStateForTesting::create(isolate->GetCurrentContext(), &DOMWrapperWorld::mainWorld()))
{
}

V8TestingScope::~V8TestingScope()
{
    m_scriptState->disposePerContextData();
}

ScriptState* V8TestingScope::scriptState() const
{
    return m_scriptState.get();
}

v8::Isolate* V8TestingScope::isolate() const
{
    return m_scriptState->isolate();
}

v8::Local<v8::Context> V8TestingScope::context() const
{
    return m_scriptState->context();
}

} // namespace blink
