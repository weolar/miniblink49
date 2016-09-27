// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/ScriptState.h"

#include "bindings/core/v8/V8Binding.h"
#include "core/dom/ExecutionContext.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"

namespace blink {

PassRefPtr<ScriptState> ScriptState::create(v8::Local<v8::Context> context, PassRefPtr<DOMWrapperWorld> world)
{
    RefPtr<ScriptState> scriptState = adoptRef(new ScriptState(context, world));
    // This ref() is for keeping this ScriptState alive as long as the v8::Context is alive.
    // This is deref()ed in the weak callback of the v8::Context.
    scriptState->ref();
    return scriptState;
}

static void derefCallback(const v8::WeakCallbackInfo<ScriptState>& data)
{
    data.GetParameter()->deref();
}

static void weakCallback(const v8::WeakCallbackInfo<ScriptState>& data)
{
    data.GetParameter()->clearContext();
    data.SetSecondPassCallback(derefCallback);
}

ScriptState::ScriptState(v8::Local<v8::Context> context, PassRefPtr<DOMWrapperWorld> world)
    : m_isolate(context->GetIsolate())
    , m_context(m_isolate, context)
    , m_world(world)
    , m_perContextData(V8PerContextData::create(context))
#if ENABLE(ASSERT)
    , m_globalObjectDetached(false)
#endif
{
    ASSERT(m_world);
    m_context.setWeak(this, &weakCallback);
    context->SetAlignedPointerInEmbedderData(v8ContextPerContextDataIndex, this);
}

ScriptState::~ScriptState()
{
    ASSERT(!m_perContextData);
    ASSERT(m_context.isEmpty());
}

void ScriptState::detachGlobalObject()
{
    ASSERT(!m_context.isEmpty());
    context()->DetachGlobal();
#if ENABLE(ASSERT)
    m_globalObjectDetached = true;
#endif
}

void ScriptState::disposePerContextData()
{
    Vector<Observer*> observers(m_observers);
    for (auto& observer : observers)
        observer->willDisposeScriptState(this);
    m_perContextData = nullptr;
}

void ScriptState::addObserver(Observer* observer)
{
    m_observers.append(observer);
}

void ScriptState::removeObserver(Observer* observer)
{
    size_t index = m_observers.find(observer);
    if (index != kNotFound)
        m_observers.remove(index);
}

bool ScriptState::evalEnabled() const
{
    v8::HandleScope handleScope(m_isolate);
    return context()->IsCodeGenerationFromStringsAllowed();
}

void ScriptState::setEvalEnabled(bool enabled)
{
    v8::HandleScope handleScope(m_isolate);
    return context()->AllowCodeGenerationFromStrings(enabled);
}

ScriptValue ScriptState::getFromGlobalObject(const char* name)
{
    v8::HandleScope handleScope(m_isolate);
    v8::Local<v8::Value> v8Value;
    if (!context()->Global()->Get(context(), v8AtomicString(isolate(), name)).ToLocal(&v8Value))
        return ScriptValue();
    return ScriptValue(this, v8Value);
}

ExecutionContext* ScriptState::executionContext() const
{
    v8::HandleScope scope(m_isolate);
    return toExecutionContext(context());
}

void ScriptState::setExecutionContext(ExecutionContext*)
{
    ASSERT_NOT_REACHED();
}

LocalDOMWindow* ScriptState::domWindow() const
{
    v8::HandleScope scope(m_isolate);
    return toLocalDOMWindow(toDOMWindow(context()));
}

ScriptState* ScriptState::forMainWorld(LocalFrame* frame)
{
    return ScriptState::forWorld(frame, DOMWrapperWorld::mainWorld());
}

ScriptState* ScriptState::forWorld(LocalFrame* frame, DOMWrapperWorld& world)
{
    ASSERT(frame);
    v8::Isolate* isolate = toIsolate(frame);
    v8::HandleScope handleScope(isolate);
    return ScriptState::from(toV8ContextEvenIfDetached(frame, world));
}

}
