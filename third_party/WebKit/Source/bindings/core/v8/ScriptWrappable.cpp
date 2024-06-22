// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/ScriptWrappable.h"

#include "bindings/core/v8/DOMDataStore.h"
#include "bindings/core/v8/V8DOMWrapper.h"

namespace blink {

struct SameSizeAsScriptWrappable {
    virtual ~SameSizeAsScriptWrappable() { }
    v8::Persistent<v8::Object> m_wrapper;
#if V8_MAJOR_VERSION >= 7
    TraceWrapperV8Reference<v8::Object> m_mainWorldWrapper;
#endif
};

static_assert(sizeof(ScriptWrappable) <= sizeof(SameSizeAsScriptWrappable), "ScriptWrappable should stay small");

namespace {

class ScriptWrappableProtector final {
    WTF_MAKE_NONCOPYABLE(ScriptWrappableProtector);
public:
    ScriptWrappableProtector(ScriptWrappable* scriptWrappable, const WrapperTypeInfo* wrapperTypeInfo)
        : m_scriptWrappable(scriptWrappable), m_wrapperTypeInfo(wrapperTypeInfo)
    {
        m_wrapperTypeInfo->refObject(m_scriptWrappable);
    }
    ~ScriptWrappableProtector()
    {
        m_wrapperTypeInfo->derefObject(m_scriptWrappable);
    }

private:
    ScriptWrappable* m_scriptWrappable;
    const WrapperTypeInfo* m_wrapperTypeInfo;
};

} // namespace

ScriptWrappable::ScriptWrappable() 
{

}

#if !ENABLE(OILPAN)
ScriptWrappable::~ScriptWrappable()
{
    // We must not get deleted as long as we contain a wrapper. If this happens, we screwed up ref
    // counting somewhere. Crash here instead of crashing during a later gc cycle.
    RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(!containsWrapper());
}
#endif

#if V8_MAJOR_VERSION >= 7
void ScriptWrappable::traceWrapper(Visitor* visitor)
{
    //visitor->trace(m_mainWorldWrapper);
}
#endif

v8::Local<v8::Object> ScriptWrappable::wrap(v8::Isolate* isolate, v8::Local<v8::Object> creationContext)
{
    const WrapperTypeInfo* wrapperTypeInfo = this->wrapperTypeInfo();

    // It's possible that no one except for the new wrapper owns this object at
    // this moment, so we have to prevent GC to collect this object until the
    // object gets associated with the wrapper.
    ScriptWrappableProtector protect(this, wrapperTypeInfo);

    ASSERT(!DOMDataStore::containsWrapper(this, isolate));

    v8::Local<v8::Object> wrapper = V8DOMWrapper::createWrapper(isolate, creationContext, wrapperTypeInfo, this);
    if (UNLIKELY(wrapper.IsEmpty()))
        return wrapper;

    wrapperTypeInfo->installConditionallyEnabledProperties(wrapper, isolate);
    return associateWithWrapper(isolate, wrapperTypeInfo, wrapper);
}

v8::Local<v8::Object> ScriptWrappable::associateWithWrapper(v8::Isolate* isolate, const WrapperTypeInfo* wrapperTypeInfo, v8::Local<v8::Object> wrapper)
{
    return V8DOMWrapper::associateObjectWithWrapper(isolate, this, wrapperTypeInfo, wrapper);
}

bool ScriptWrappable::setWrapper(v8::Isolate* isolate, const WrapperTypeInfo* wrapperTypeInfo, v8::Local<v8::Object>& wrapper) WARN_UNUSED_RETURN
{
    ASSERT(!wrapper.IsEmpty());
    if (UNLIKELY(containsWrapper())) {
        wrapper = newLocalWrapper(isolate);
        return false;
    }
    m_wrapper.Reset(isolate, wrapper);
    wrapperTypeInfo->configureWrapper(&m_wrapper);
    m_wrapper.SetWeak(this, &firstWeakCallback, v8::WeakCallbackType::kInternalFields);

    ASSERT(containsWrapper());

#if V8_MAJOR_VERSION >= 7
    m_mainWorldWrapper.Set(isolate, wrapper);
    wrapperTypeInfo->configureWrapper(&m_mainWorldWrapper.Get());
#endif
    return true;
}

#if V8_MAJOR_VERSION >= 7
const v8::TracedGlobal<v8::Object>& ScriptWrappable::GetMainWorldWrapper() const
{
    return m_mainWorldWrapper.Get();
}
#endif

v8::Local<v8::Object> ScriptWrappable::newLocalWrapper(v8::Isolate* isolate) const
{
    return v8::Local<v8::Object>::New(isolate, m_wrapper);
}

bool ScriptWrappable::isEqualTo(const v8::Local<v8::Object>& other) const
{
    return m_wrapper == other;
}

bool ScriptWrappable::setReturnValue(v8::ReturnValue<v8::Value> returnValue)
{
    returnValue.Set(m_wrapper);
    return containsWrapper();
}

void ScriptWrappable::markAsDependentGroup(ScriptWrappable* groupRoot, v8::Isolate* isolate)
{
    ASSERT(containsWrapper());
    ASSERT(groupRoot && groupRoot->containsWrapper());

    // FIXME: There has to be a better way.
    v8::UniqueId groupId(*reinterpret_cast<intptr_t*>(&groupRoot->m_wrapper));
    //zero
#if !(V8_MINOR_VERSION == 7)
    m_wrapper.MarkPartiallyDependent();
#endif
    isolate->SetObjectGroupId(v8::Persistent<v8::Value>::Cast(m_wrapper), groupId);
}

void ScriptWrappable::setReference(const v8::Persistent<v8::Object>& parent, v8::Isolate* isolate)
{
    isolate->SetReference(parent, m_wrapper);
}

void ScriptWrappable::disposeWrapper(const v8::WeakCallbackInfo<ScriptWrappable>& data)
{
    auto scriptWrappable = reinterpret_cast<ScriptWrappable*>(data.GetInternalField(v8DOMWrapperObjectIndex));
    RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(scriptWrappable == this);
    RELEASE_ASSERT(containsWrapper());

    m_wrapper.Reset();
#if V8_MAJOR_VERSION >= 7
    m_mainWorldWrapper.Clear();
#endif
}

void ScriptWrappable::firstWeakCallback(const v8::WeakCallbackInfo<ScriptWrappable>& data)
{
    data.GetParameter()->disposeWrapper(data);
    data.SetSecondPassCallback(secondWeakCallback);
}

void ScriptWrappable::secondWeakCallback(const v8::WeakCallbackInfo<ScriptWrappable>& data)
{
    // FIXME: I noticed that 50%~ of minor GC cycle times can be consumed
    // inside data.GetParameter()->deref(), which causes Node destructions. We should
    // make Node destructions incremental.
    auto scriptWrappable = reinterpret_cast<ScriptWrappable*>(data.GetInternalField(v8DOMWrapperObjectIndex));
    auto typeInfo = reinterpret_cast<WrapperTypeInfo*>(data.GetInternalField(v8DOMWrapperTypeIndex));
    typeInfo->derefObject(scriptWrappable);
}

} // namespace blink
