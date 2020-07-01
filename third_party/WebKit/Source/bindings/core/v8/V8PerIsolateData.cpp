/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "bindings/core/v8/V8PerIsolateData.h"

#include "bindings/core/v8/DOMDataStore.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "bindings/core/v8/V8ObjectConstructor.h"
#include "bindings/core/v8/V8RecursionScope.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "core/frame/UseCounter.h"
#include "core/inspector/ScriptDebuggerBase.h"
#include "public/platform/Platform.h"
#include "wtf/MainThread.h"

namespace blink {

static V8PerIsolateData* mainThreadPerIsolateData = 0;

#if ENABLE(ASSERT)
static void assertV8RecursionScope()
{
    int level = V8RecursionScope::recursionLevel(v8::Isolate::GetCurrent());
    int internalLevel = V8PerIsolateData::from(v8::Isolate::GetCurrent())->internalScriptRecursionLevel();
    if (level <= 0 && internalLevel <= 0)
        DebugBreak();
    ASSERT(V8RecursionScope::properlyUsed(v8::Isolate::GetCurrent()));
}

static bool runningUnitTest()
{
    return Platform::current()->unitTestSupport();
}
#endif

static void useCounterCallback(v8::Isolate* isolate, v8::Isolate::UseCounterFeature feature)
{
    switch (feature) {
    case v8::Isolate::kUseAsm:
        UseCounter::count(callingExecutionContext(isolate), UseCounter::UseAsm);
        break;
    case v8::Isolate::kBreakIterator:
        UseCounter::count(callingExecutionContext(isolate), UseCounter::BreakIterator);
        break;
    case v8::Isolate::kLegacyConst:
        UseCounter::count(callingExecutionContext(isolate), UseCounter::LegacyConst);
        break;
    case v8::Isolate::kObjectObserve:
        UseCounter::count(callingExecutionContext(isolate), UseCounter::ObjectObserve);
        break;
    default:
        // This can happen if V8 has added counters that this version of Blink
        // does not know about. It's harmless.
        break;
    }
}

V8PerIsolateData::V8PerIsolateData()
    : m_destructionPending(false)
#ifdef MINIBLINK_NOT_IMPLEMENTED
    , m_isolateHolder(adoptPtr(new gin::IsolateHolder()))
#else
	, m_isolateHolder(new gin::IsolateHolder())
#endif // MINIBLINK_NOT_IMPLEMENTED
    , m_stringCache(adoptPtr(new StringCache(isolate())))
    , m_hiddenValue(adoptPtr(new V8HiddenValue()))
    , m_constructorMode(ConstructorMode::CreateNewObject)
    , m_recursionLevel(0)
    , m_isHandlingRecursionLevelError(false)
    , m_isReportingException(false)
#if ENABLE(ASSERT)
    , m_internalScriptRecursionLevel(0)
#endif
    , m_performingMicrotaskCheckpoint(false)
    , m_scriptDebugger(nullptr)
{
    // FIXME: Remove once all v8::Isolate::GetCurrent() calls are gone.
    isolate()->Enter();
#if ENABLE(ASSERT)
    if (!runningUnitTest())
        isolate()->AddCallCompletedCallback(&assertV8RecursionScope);
#endif
    if (isMainThread())
        mainThreadPerIsolateData = this;
    isolate()->SetUseCounterCallback(&useCounterCallback);
}

V8PerIsolateData::~V8PerIsolateData()
{
#ifndef MINIBLINK_NOT_IMPLEMENTED
	delete m_isolateHolder;
#endif // MINIBLINK_NOT_IMPLEMENTED
}

v8::Isolate* V8PerIsolateData::mainThreadIsolate()
{
    ASSERT(isMainThread());
    ASSERT(mainThreadPerIsolateData);
    return mainThreadPerIsolateData->isolate();
}

v8::Isolate* V8PerIsolateData::initialize()
{
    V8PerIsolateData* data = new V8PerIsolateData();
    v8::Isolate* isolate = data->isolate();
    isolate->SetData(gin::kEmbedderBlink, data);
    return isolate;
}

v8::Persistent<v8::Value>& V8PerIsolateData::ensureLiveRoot()
{
    if (m_liveRoot.isEmpty())
        m_liveRoot.set(isolate(), v8::Null(isolate()));
    return m_liveRoot.getUnsafe();
}

void V8PerIsolateData::willBeDestroyed(v8::Isolate* isolate)
{
    V8PerIsolateData* data = from(isolate);

    ASSERT(!data->m_destructionPending);
    data->m_destructionPending = true;

    // Clear any data that may have handles into the heap,
    // prior to calling ThreadState::detach().
    data->clearEndOfScopeTasks();
}

void V8PerIsolateData::destroy(v8::Isolate* isolate)
{
#if ENABLE(ASSERT)
    if (!runningUnitTest())
        isolate->RemoveCallCompletedCallback(&assertV8RecursionScope);
#endif
    V8PerIsolateData* data = from(isolate);

    // Clear everything before exiting the Isolate.
    if (data->m_scriptRegexpScriptState)
        data->m_scriptRegexpScriptState->disposePerContextData();
    data->m_scriptDebugger.clear();
    data->m_liveRoot.clear();
    data->m_hiddenValue.clear();
    data->m_stringCache->dispose();
    data->m_stringCache.clear();
    data->m_toStringTemplate.clear();
    data->m_domTemplateMapForNonMainWorld.clear();
    data->m_domTemplateMapForMainWorld.clear();
    if (isMainThread())
        mainThreadPerIsolateData = 0;

    // FIXME: Remove once all v8::Isolate::GetCurrent() calls are gone.
    isolate->Exit();
    delete data;
}

V8PerIsolateData::DOMTemplateMap& V8PerIsolateData::currentDOMTemplateMap()
{
    if (DOMWrapperWorld::current(isolate()).isMainWorld())
        return m_domTemplateMapForMainWorld;
    return m_domTemplateMapForNonMainWorld;
}

v8::Local<v8::FunctionTemplate> V8PerIsolateData::domTemplate(const void* domTemplateKey, v8::FunctionCallback callback, v8::Local<v8::Value> data, v8::Local<v8::Signature> signature, int length)
{
    DOMTemplateMap& domTemplateMap = currentDOMTemplateMap();
    DOMTemplateMap::iterator result = domTemplateMap.find(domTemplateKey);
    if (result != domTemplateMap.end())
        return result->value.Get(isolate());

    v8::Local<v8::FunctionTemplate> templ = v8::FunctionTemplate::New(isolate(), callback, data, signature, length);
    domTemplateMap.add(domTemplateKey, v8::Eternal<v8::FunctionTemplate>(isolate(), templ));
    return templ;
}

v8::Local<v8::FunctionTemplate> V8PerIsolateData::existingDOMTemplate(const void* domTemplateKey)
{
    DOMTemplateMap& domTemplateMap = currentDOMTemplateMap();
    DOMTemplateMap::iterator result = domTemplateMap.find(domTemplateKey);
    if (result != domTemplateMap.end())
        return result->value.Get(isolate());
    return v8::Local<v8::FunctionTemplate>();
}

void V8PerIsolateData::setDOMTemplate(const void* domTemplateKey, v8::Local<v8::FunctionTemplate> templ)
{
    currentDOMTemplateMap().add(domTemplateKey, v8::Eternal<v8::FunctionTemplate>(isolate(), v8::Local<v8::FunctionTemplate>(templ)));
}

v8::Local<v8::Context> V8PerIsolateData::ensureScriptRegexpContext()
{
    if (!m_scriptRegexpScriptState) {
        v8::Local<v8::Context> context(v8::Context::New(isolate()));
        m_scriptRegexpScriptState = ScriptState::create(context, DOMWrapperWorld::create(isolate()));
    }
    return m_scriptRegexpScriptState->context();
}

bool V8PerIsolateData::hasInstance(const WrapperTypeInfo* untrustedWrapperTypeInfo, v8::Local<v8::Value> value)
{
    return hasInstance(untrustedWrapperTypeInfo, value, m_domTemplateMapForMainWorld)
        || hasInstance(untrustedWrapperTypeInfo, value, m_domTemplateMapForNonMainWorld);
}

bool V8PerIsolateData::hasInstance(const WrapperTypeInfo* untrustedWrapperTypeInfo, v8::Local<v8::Value> value, DOMTemplateMap& domTemplateMap)
{
    DOMTemplateMap::iterator result = domTemplateMap.find(untrustedWrapperTypeInfo);
    if (result == domTemplateMap.end())
        return false;
    v8::Local<v8::FunctionTemplate> templ = result->value.Get(isolate());
    return templ->HasInstance(value);
}

v8::Local<v8::Object> V8PerIsolateData::findInstanceInPrototypeChain(const WrapperTypeInfo* info, v8::Local<v8::Value> value)
{
    v8::Local<v8::Object> wrapper = findInstanceInPrototypeChain(info, value, m_domTemplateMapForMainWorld);
    if (!wrapper.IsEmpty())
        return wrapper;
    return findInstanceInPrototypeChain(info, value, m_domTemplateMapForNonMainWorld);
}

v8::Local<v8::Object> V8PerIsolateData::findInstanceInPrototypeChain(const WrapperTypeInfo* info, v8::Local<v8::Value> value, DOMTemplateMap& domTemplateMap)
{
    if (value.IsEmpty() || !value->IsObject())
        return v8::Local<v8::Object>();
    DOMTemplateMap::iterator result = domTemplateMap.find(info);
    if (result == domTemplateMap.end())
        return v8::Local<v8::Object>();
    v8::Local<v8::FunctionTemplate> templ = result->value.Get(isolate());
    return v8::Local<v8::Object>::Cast(value)->FindInstanceInPrototypeChain(templ);
}

static void constructorOfToString(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    // The DOM constructors' toString functions grab the current toString
    // for Functions by taking the toString function of itself and then
    // calling it with the constructor as its receiver. This means that
    // changes to the Function prototype chain or toString function are
    // reflected when printing DOM constructors. The only wart is that
    // changes to a DOM constructor's toString's toString will cause the
    // toString of the DOM constructor itself to change. This is extremely
    // obscure and unlikely to be a problem.
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Value> value;
    if (!info.Callee()->Get(isolate->GetCurrentContext(), v8AtomicString(isolate, "toString")).ToLocal(&value) || !value->IsFunction()) {
        v8SetReturnValue(info, v8::String::Empty(isolate));
        return;
    }
    v8::Local<v8::Value> result;
    if (V8ScriptRunner::callInternalFunction(v8::Local<v8::Function>::Cast(value), info.This(), 0, 0, isolate).ToLocal(&result))
        v8SetReturnValue(info, result);
}

v8::Local<v8::FunctionTemplate> V8PerIsolateData::toStringTemplate()
{
    if (m_toStringTemplate.isEmpty())
        m_toStringTemplate.set(isolate(), v8::FunctionTemplate::New(isolate(), constructorOfToString));
    return m_toStringTemplate.newLocal(isolate());
}

void V8PerIsolateData::addEndOfScopeTask(PassOwnPtr<EndOfScopeTask> task)
{
    m_endOfScopeTasks.append(task);
}

void V8PerIsolateData::runEndOfScopeTasks()
{
    Vector<OwnPtr<EndOfScopeTask>> tasks;
    tasks.swap(m_endOfScopeTasks);
    for (const auto& task : tasks)
        task->run();
    ASSERT(m_endOfScopeTasks.isEmpty());
}

void V8PerIsolateData::clearEndOfScopeTasks()
{
    m_endOfScopeTasks.clear();
}

void V8PerIsolateData::setScriptDebugger(PassOwnPtrWillBeRawPtr<ScriptDebuggerBase> debugger)
{
    ASSERT(!m_scriptDebugger);
    m_scriptDebugger = debugger;
}

} // namespace blink
