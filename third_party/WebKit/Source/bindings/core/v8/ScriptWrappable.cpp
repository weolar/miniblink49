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

void ScriptWrappable::disposeWrapper(const v8::WeakCallbackInfo<ScriptWrappable>& data)
{
    auto scriptWrappable = reinterpret_cast<ScriptWrappable*>(data.GetInternalField(v8DOMWrapperObjectIndex));
    RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(scriptWrappable == this);
    RELEASE_ASSERT(containsWrapper());

    m_wrapper.Reset();
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
