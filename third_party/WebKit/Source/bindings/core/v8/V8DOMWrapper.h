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

#ifndef V8DOMWrapper_h
#define V8DOMWrapper_h

#include "bindings/core/v8/DOMDataStore.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RawPtr.h"
#include "wtf/text/AtomicString.h"
#include <v8.h>

namespace blink {

class Node;
struct WrapperTypeInfo;

class V8DOMWrapper {
public:
    static v8::Local<v8::Object> createWrapper(v8::Isolate*, v8::Local<v8::Object> creationContext, const WrapperTypeInfo*, ScriptWrappable*);
    static bool isWrapper(v8::Isolate*, v8::Local<v8::Value>);

    // Associates the given ScriptWrappable with the given |wrapper| if the
    // ScriptWrappable is not yet associated with any wrapper.  Returns the
    // wrapper already associated or |wrapper| if not yet associated.
    // The caller should always use the returned value rather than |wrapper|.
    static v8::Local<v8::Object> associateObjectWithWrapper(v8::Isolate*, ScriptWrappable*, const WrapperTypeInfo*, v8::Local<v8::Object> wrapper) WARN_UNUSED_RETURN;
    static v8::Local<v8::Object> associateObjectWithWrapper(v8::Isolate*, Node*, const WrapperTypeInfo*, v8::Local<v8::Object> wrapper) WARN_UNUSED_RETURN;
    static void setNativeInfo(v8::Local<v8::Object>, const WrapperTypeInfo*, ScriptWrappable*);
    static void clearNativeInfo(v8::Local<v8::Object>, const WrapperTypeInfo*);
    // hasInternalFieldsSet only checks if the value has the internal fields for
    // wrapper obejct and type, and does not check if it's valid or not.  The
    // value may not be a Blink's wrapper object.  In order to make sure of it,
    // Use isWrapper function instead.
    static bool hasInternalFieldsSet(v8::Local<v8::Value>);
};

inline void V8DOMWrapper::setNativeInfo(v8::Local<v8::Object> wrapper, const WrapperTypeInfo* wrapperTypeInfo, ScriptWrappable* scriptWrappable)
{
    ASSERT(wrapper->InternalFieldCount() >= 2);
    ASSERT(scriptWrappable);
    ASSERT(wrapperTypeInfo);
    wrapper->SetAlignedPointerInInternalField(v8DOMWrapperObjectIndex, scriptWrappable);
    wrapper->SetAlignedPointerInInternalField(v8DOMWrapperTypeIndex, const_cast<WrapperTypeInfo*>(wrapperTypeInfo));
}

inline void V8DOMWrapper::clearNativeInfo(v8::Local<v8::Object> wrapper, const WrapperTypeInfo* wrapperTypeInfo)
{
    ASSERT(wrapper->InternalFieldCount() >= 2);
    ASSERT(wrapperTypeInfo);
    // clearNativeInfo() is used only by NP objects, which are not garbage collected.
    ASSERT(wrapperTypeInfo->gcType == WrapperTypeInfo::RefCountedObject);
    wrapper->SetAlignedPointerInInternalField(v8DOMWrapperTypeIndex, const_cast<WrapperTypeInfo*>(wrapperTypeInfo));
    wrapper->SetAlignedPointerInInternalField(v8DOMWrapperObjectIndex, 0);
}

inline v8::Local<v8::Object> V8DOMWrapper::associateObjectWithWrapper(v8::Isolate* isolate, ScriptWrappable* impl, const WrapperTypeInfo* wrapperTypeInfo, v8::Local<v8::Object> wrapper)
{
    if (DOMDataStore::setWrapper(isolate, impl, wrapperTypeInfo, wrapper)) {
        wrapperTypeInfo->refObject(impl);
        setNativeInfo(wrapper, wrapperTypeInfo, impl);
        ASSERT(hasInternalFieldsSet(wrapper));
    }
    RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(toScriptWrappable(wrapper) == impl);
    return wrapper;
}

inline v8::Local<v8::Object> V8DOMWrapper::associateObjectWithWrapper(v8::Isolate* isolate, Node* node, const WrapperTypeInfo* wrapperTypeInfo, v8::Local<v8::Object> wrapper)
{
    if (DOMDataStore::setWrapper(isolate, node, wrapperTypeInfo, wrapper)) {
        wrapperTypeInfo->refObject(ScriptWrappable::fromNode(node));
        setNativeInfo(wrapper, wrapperTypeInfo, ScriptWrappable::fromNode(node));
        ASSERT(hasInternalFieldsSet(wrapper));
    }
    RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(toScriptWrappable(wrapper) == ScriptWrappable::fromNode(node));
    return wrapper;
}

class V8WrapperInstantiationScope {
public:
    V8WrapperInstantiationScope(v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
        : m_didEnterContext(false)
        , m_context(isolate->GetCurrentContext())
    {
        // creationContext should not be empty. Because if we have an
        // empty creationContext, we will end up creating
        // a new object in the context currently entered. This is wrong.
        RELEASE_ASSERT(!creationContext.IsEmpty());
        v8::Local<v8::Context> contextForWrapper = creationContext->CreationContext();
        // For performance, we enter the context only if the currently running context
        // is different from the context that we are about to enter.
        if (contextForWrapper == m_context)
            return;
        m_context = v8::Local<v8::Context>::New(isolate, contextForWrapper);
        m_didEnterContext = true;
        m_context->Enter();
    }

    ~V8WrapperInstantiationScope()
    {
        if (!m_didEnterContext)
            return;
        m_context->Exit();
    }

    v8::Local<v8::Context> context() const { return m_context; }

private:
    bool m_didEnterContext;
    v8::Local<v8::Context> m_context;
};

} // namespace blink

#endif // V8DOMWrapper_h
