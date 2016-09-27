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

#ifndef DOMDataStore_h
#define DOMDataStore_h

#include "bindings/core/v8/DOMWrapperMap.h"
#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/WrapperTypeInfo.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/StdLibExtras.h"
#include <v8.h>

namespace blink {

class Node;

class DOMDataStore {
    WTF_MAKE_NONCOPYABLE(DOMDataStore);
public:
    DOMDataStore(v8::Isolate* isolate, bool isMainWorld)
        : m_isMainWorld(isMainWorld)
        // We never use |m_wrapperMap| when it's the main world.
        , m_wrapperMap(adoptPtr(
            isMainWorld
            ? nullptr
            : new DOMWrapperMap<ScriptWrappable>(isolate))) { }

    static DOMDataStore& current(v8::Isolate* isolate)
    {
        return DOMWrapperWorld::current(isolate).domDataStore();
    }

    static bool setReturnValue(v8::ReturnValue<v8::Value> returnValue, ScriptWrappable* object)
    {
        return current(returnValue.GetIsolate()).setReturnValueFrom(returnValue, object);
    }

    static bool setReturnValue(v8::ReturnValue<v8::Value> returnValue, Node* object)
    {
        if (canUseScriptWrappable(object))
            return ScriptWrappable::fromNode(object)->setReturnValue(returnValue);
        return current(returnValue.GetIsolate()).setReturnValueFrom(returnValue, ScriptWrappable::fromNode(object));
    }

    static bool setReturnValueForMainWorld(v8::ReturnValue<v8::Value> returnValue, ScriptWrappable* object)
    {
        return object->setReturnValue(returnValue);
    }

    static bool setReturnValueFast(v8::ReturnValue<v8::Value> returnValue, ScriptWrappable* object, v8::Local<v8::Object> holder, const ScriptWrappable* wrappable)
    {
        // The second fastest way to check if we're in the main world is to check if
        // the wrappable's wrapper is the same as the holder.
        if (holderContainsWrapper(holder, wrappable))
            return object->setReturnValue(returnValue);
        return current(returnValue.GetIsolate()).setReturnValueFrom(returnValue, object);
    }

    static bool setReturnValueFast(v8::ReturnValue<v8::Value> returnValue, Node* node, v8::Local<v8::Object> holder, const ScriptWrappable* wrappable)
    {
        if (canUseScriptWrappable(node)
            // The second fastest way to check if we're in the main world is to
            // check if the wrappable's wrapper is the same as the holder.
            || holderContainsWrapper(holder, wrappable))
            return ScriptWrappable::fromNode(node)->setReturnValue(returnValue);
        return current(returnValue.GetIsolate()).setReturnValueFrom(returnValue, ScriptWrappable::fromNode(node));
    }

    static v8::Local<v8::Object> getWrapper(ScriptWrappable* object, v8::Isolate* isolate)
    {
        return current(isolate).get(object, isolate);
    }

    static v8::Local<v8::Object> getWrapper(Node* node, v8::Isolate* isolate)
    {
        if (canUseScriptWrappable(node))
            return ScriptWrappable::fromNode(node)->newLocalWrapper(isolate);
        return current(isolate).get(ScriptWrappable::fromNode(node), isolate);
    }

    static void setWrapperReference(const v8::Persistent<v8::Object>& parent, ScriptWrappable* child, v8::Isolate* isolate)
    {
        current(isolate).setReference(parent, child, isolate);
    }

    static void setWrapperReference(const v8::Persistent<v8::Object>& parent, Node* child, v8::Isolate* isolate)
    {
        if (canUseScriptWrappable(child)) {
            ScriptWrappable::fromNode(child)->setReference(parent, isolate);
            return;
        }
        current(isolate).setReference(parent, ScriptWrappable::fromNode(child), isolate);
    }

    // Associates the given |object| with the given |wrapper| if the object is
    // not yet associated with any wrapper.  Returns true if the given wrapper
    // is associated with the object, or false if the object is already
    // associated with a wrapper.  In the latter case, |wrapper| will be updated
    // to the existing wrapper.
    static bool setWrapper(v8::Isolate* isolate, ScriptWrappable* object, const WrapperTypeInfo* wrapperTypeInfo, v8::Local<v8::Object>& wrapper) WARN_UNUSED_RETURN
    {
        return current(isolate).set(isolate, object, wrapperTypeInfo, wrapper);
    }

    static bool setWrapper(v8::Isolate* isolate, Node* node, const WrapperTypeInfo* wrapperTypeInfo, v8::Local<v8::Object>& wrapper) WARN_UNUSED_RETURN
    {
        if (canUseScriptWrappable(node))
            return ScriptWrappable::fromNode(node)->setWrapper(isolate, wrapperTypeInfo, wrapper);
        return current(isolate).set(isolate, ScriptWrappable::fromNode(node), wrapperTypeInfo, wrapper);
    }

    static bool containsWrapper(ScriptWrappable* object, v8::Isolate* isolate)
    {
        return current(isolate).containsWrapper(object);
    }

    v8::Local<v8::Object> get(ScriptWrappable* object, v8::Isolate* isolate)
    {
        if (m_isMainWorld)
            return object->newLocalWrapper(isolate);
        return m_wrapperMap->newLocal(isolate, object);
    }

    void setReference(const v8::Persistent<v8::Object>& parent, ScriptWrappable* child, v8::Isolate* isolate)
    {
        if (m_isMainWorld) {
            child->setReference(parent, isolate);
            return;
        }
        m_wrapperMap->setReference(isolate, parent, child);
    }

    bool setReturnValueFrom(v8::ReturnValue<v8::Value> returnValue, ScriptWrappable* object)
    {
        if (m_isMainWorld)
            return object->setReturnValue(returnValue);
        return m_wrapperMap->setReturnValueFrom(returnValue, object);
    }

    bool containsWrapper(ScriptWrappable* object)
    {
        if (m_isMainWorld)
            return object->containsWrapper();
        return m_wrapperMap->containsKey(object);
    }

private:
    bool set(v8::Isolate* isolate, ScriptWrappable* object, const WrapperTypeInfo* wrapperTypeInfo, v8::Local<v8::Object>& wrapper) WARN_UNUSED_RETURN
    {
        ASSERT(object);
        ASSERT(!wrapper.IsEmpty());
        if (m_isMainWorld)
            return object->setWrapper(isolate, wrapperTypeInfo, wrapper);
        return m_wrapperMap->set(object, wrapperTypeInfo, wrapper);
    }

    // We can use a wrapper stored in a ScriptWrappable when we're in the main world.
    // This method does the fast check if we're in the main world. If this method returns true,
    // it is guaranteed that we're in the main world. On the other hand, if this method returns
    // false, nothing is guaranteed (we might be in the main world).
    static bool canUseScriptWrappable(Node*)
    {
        // This helper function itself doesn't use the argument, but we have to
        // make sure that the argument is type of Node* because Node and its
        // subclasses satisfy the following two conditions.
        //   1. Nodes never exist in worker.
        //   2. Node inherits from ScriptWrappable.
        // and if there exists no isolated world, we're sure that we're in the
        // main world and we can use ScriptWrappable's wrapper.
        return !DOMWrapperWorld::isolatedWorldsExist();
    }

    static bool holderContainsWrapper(v8::Local<v8::Object> holder, const ScriptWrappable* wrappable)
    {
        // Verify our assumptions about the main world.
        ASSERT(wrappable);
        ASSERT(!wrappable->containsWrapper() || !wrappable->isEqualTo(holder) || current(v8::Isolate::GetCurrent()).m_isMainWorld);
        return wrappable->isEqualTo(holder);
    }

    bool m_isMainWorld;
    OwnPtr<DOMWrapperMap<ScriptWrappable>> m_wrapperMap;
};

template <>
inline void DOMWrapperMap<ScriptWrappable>::PersistentValueMapTraits::Dispose(
    v8::Isolate* isolate,
    v8::Global<v8::Object> value,
    ScriptWrappable* key)
{
    RELEASE_ASSERT(!value.IsEmpty()); // See crbug.com/368095.
    releaseObject(value);
}

template <>
inline void DOMWrapperMap<ScriptWrappable>::PersistentValueMapTraits::DisposeWeak(v8::Isolate* isolate, void* internalFields[v8::kInternalFieldsInWeakCallback], ScriptWrappable* key)
{
    auto typeInfo = reinterpret_cast<WrapperTypeInfo*>(internalFields[v8DOMWrapperTypeIndex]);
    typeInfo->derefObject(key);
}

} // namespace blink

#endif // DOMDataStore_h
