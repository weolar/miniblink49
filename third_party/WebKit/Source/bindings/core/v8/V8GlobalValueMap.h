// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8GlobalValueMap_h
#define V8GlobalValueMap_h

#include "wtf/HashMap.h"
#include "wtf/text/StringHash.h"
#include <v8-util.h>
#include <v8.h>

namespace blink {

/**
 * A Traits class for v8::GlobalValueMap that uses wtf/HashMap as a
 * backing store.
 *
 * The parameter is_weak will determine whether the references are 'weak'.
 * If so, entries will be removed from the map as the weak references are
 * collected.
 */
template <class KeyType, class ValueType, v8::PersistentContainerCallbackType type>
class V8GlobalValueMapTraits {
public:
    // Map traits:
    typedef HashMap<KeyType, v8::PersistentContainerValue> Impl;
    typedef typename Impl::iterator Iterator;
    static size_t Size(const Impl* impl) { return impl->size(); }
    static bool Empty(Impl* impl) { return impl->isEmpty(); }
    static void Swap(Impl& impl, Impl& other) { impl.swap(other); }
    static Iterator Begin(Impl* impl) { return impl->begin(); }
    static Iterator End(Impl* impl) { return impl->end(); }
    static v8::PersistentContainerValue Value(Iterator& iter)
    {
        return iter->value;
    }
    static KeyType Key(Iterator& iter) { return iter->key; }
    static v8::PersistentContainerValue Set(
        Impl* impl, KeyType key, v8::PersistentContainerValue value)
    {
        v8::PersistentContainerValue oldValue = Get(impl, key);
        impl->set(key, value);
        return oldValue;
    }
    static v8::PersistentContainerValue Get(const Impl* impl, KeyType key)
    {
        return impl->get(key);
    }

    static v8::PersistentContainerValue Remove(Impl* impl, KeyType key)
    {
        return impl->take(key);
    }

    // Weak traits:
    static const v8::PersistentContainerCallbackType kCallbackType = type;
    typedef v8::GlobalValueMap<KeyType, ValueType, V8GlobalValueMapTraits<KeyType, ValueType, type>> MapType;

    typedef void WeakCallbackDataType;

    static WeakCallbackDataType* WeakCallbackParameter(MapType* map, KeyType key, const v8::Local<ValueType>& value)
    {
        return 0;
    }

    static void DisposeCallbackData(WeakCallbackDataType* callbackData)
    {
    }

    static MapType* MapFromWeakCallbackInfo(
        const v8::WeakCallbackInfo<WeakCallbackDataType>& data)
    {
        return 0;
    }

    static KeyType KeyFromWeakCallbackInfo(
        const v8::WeakCallbackInfo<WeakCallbackDataType>& data)
    {
        return KeyType();
    }

    static void OnWeakCallback(const v8::WeakCallbackInfo<WeakCallbackDataType>& data) {}

    // Dispose traits:
    static void Dispose(v8::Isolate* isolate, v8::Global<ValueType> value, KeyType key) { }
    static void DisposeWeak(const v8::WeakCallbackInfo<WeakCallbackDataType>& data) { }
};

/**
 * A map for safely storing persistent V8 values, based on
 * v8::GlobalValueMap.
 */
template <class KeyType, class ValueType, v8::PersistentContainerCallbackType type>
class V8GlobalValueMap : public v8::GlobalValueMap<KeyType, ValueType, V8GlobalValueMapTraits<KeyType, ValueType, type>> {
public:
    typedef V8GlobalValueMapTraits<KeyType, ValueType, type> Traits;
    explicit V8GlobalValueMap(v8::Isolate* isolate)
        : v8::GlobalValueMap<KeyType, ValueType, Traits>(isolate)
    {
    }
};

} // namespace blink

#endif // V8GlobalValueMap_h
