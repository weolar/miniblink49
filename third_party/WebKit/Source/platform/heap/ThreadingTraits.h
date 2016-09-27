// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ThreadingTraits_h
#define ThreadingTraits_h

#include "wtf/Deque.h"
#include "wtf/HashCountedSet.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/HashTable.h"
#include "wtf/LinkedHashSet.h"
#include "wtf/ListHashSet.h"
#include "wtf/TypeTraits.h"
#include "wtf/Vector.h"

namespace blink {

// ThreadAffinity indicates which threads objects can be used on. We
// distinguish between objects that can be used on the main thread
// only and objects that can be used on any thread.
//
// For objects that can only be used on the main thread, we avoid going
// through thread-local storage to get to the thread state. This is
// important for performance.
enum ThreadAffinity {
    AnyThread,
    MainThreadOnly,
};

// TODO(haraken): These forward declarations violate dependency rules.
// Remove them.
class Node;
class NodeList;

template<typename T,
    bool mainThreadOnly = WTF::IsSubclass<typename WTF::RemoveConst<T>::Type, Node>::value
        || WTF::IsSubclass<typename WTF::RemoveConst<T>::Type, NodeList>::value> struct DefaultThreadingTrait;

template<typename T>
struct DefaultThreadingTrait<T, false> {
    static const ThreadAffinity Affinity = AnyThread;
};

template<typename T>
struct DefaultThreadingTrait<T, true> {
    static const ThreadAffinity Affinity = MainThreadOnly;
};

class HeapAllocator;
template<typename Table> class HeapHashTableBacking;
template<typename T, typename Traits> class HeapVectorBacking;
template<typename T> class Member;
template<typename T> class WeakMember;

template<typename T>
struct ThreadingTrait {
    static const ThreadAffinity Affinity = DefaultThreadingTrait<T>::Affinity;
};

template<typename U> class ThreadingTrait<const U> : public ThreadingTrait<U> { };

template<typename T>
struct ThreadingTrait<Member<T>> {
    static const ThreadAffinity Affinity = ThreadingTrait<T>::Affinity;
};

template<typename T>
struct ThreadingTrait<WeakMember<T>> {
    static const ThreadAffinity Affinity = ThreadingTrait<T>::Affinity;
};

template<typename Key, typename Value, typename T, typename U, typename V>
struct ThreadingTrait<HashMap<Key, Value, T, U, V, HeapAllocator>> {
    static const ThreadAffinity Affinity =
        (ThreadingTrait<Key>::Affinity == MainThreadOnly)
        && (ThreadingTrait<Value>::Affinity == MainThreadOnly) ? MainThreadOnly : AnyThread;
};

template<typename First, typename Second>
struct ThreadingTrait<WTF::KeyValuePair<First, Second>> {
    static const ThreadAffinity Affinity =
        (ThreadingTrait<First>::Affinity == MainThreadOnly)
        && (ThreadingTrait<Second>::Affinity == MainThreadOnly) ? MainThreadOnly : AnyThread;
};

template<typename T, typename U, typename V>
struct ThreadingTrait<HashSet<T, U, V, HeapAllocator>> {
    static const ThreadAffinity Affinity = ThreadingTrait<T>::Affinity;
};

template<typename T, size_t inlineCapacity>
struct ThreadingTrait<Vector<T, inlineCapacity, HeapAllocator>> {
    static const ThreadAffinity Affinity = ThreadingTrait<T>::Affinity;
};

template<typename T, typename Traits>
struct ThreadingTrait<HeapVectorBacking<T, Traits>> {
    static const ThreadAffinity Affinity = ThreadingTrait<T>::Affinity;
};

template<typename T, size_t inlineCapacity>
struct ThreadingTrait<Deque<T, inlineCapacity, HeapAllocator>> {
    static const ThreadAffinity Affinity = ThreadingTrait<T>::Affinity;
};

template<typename T, typename U, typename V>
struct ThreadingTrait<HashCountedSet<T, U, V, HeapAllocator>> {
    static const ThreadAffinity Affinity = ThreadingTrait<T>::Affinity;
};

template<typename Table>
struct ThreadingTrait<HeapHashTableBacking<Table>> {
    using Key = typename Table::KeyType;
    using Value = typename Table::ValueType;
    static const ThreadAffinity Affinity =
        (ThreadingTrait<Key>::Affinity == MainThreadOnly)
        && (ThreadingTrait<Value>::Affinity == MainThreadOnly) ? MainThreadOnly : AnyThread;
};

template<typename T, typename U, typename V, typename W, typename X> class HeapHashMap;
template<typename T, typename U, typename V> class HeapHashSet;
template<typename T, size_t inlineCapacity> class HeapVector;
template<typename T, size_t inlineCapacity> class HeapDeque;
template<typename T, typename U, typename V> class HeapHashCountedSet;

template<typename T, typename U, typename V, typename W, typename X>
struct ThreadingTrait<HeapHashMap<T, U, V, W, X>> : public ThreadingTrait<HashMap<T, U, V, W, X, HeapAllocator>> { };
template<typename T, typename U, typename V>
struct ThreadingTrait<HeapHashSet<T, U, V>> : public ThreadingTrait<HashSet<T, U, V, HeapAllocator>> { };
template<typename T, size_t inlineCapacity>
struct ThreadingTrait<HeapVector<T, inlineCapacity>> : public ThreadingTrait<Vector<T, inlineCapacity, HeapAllocator>> { };
template<typename T, size_t inlineCapacity>
struct ThreadingTrait<HeapDeque<T, inlineCapacity>> : public ThreadingTrait<Deque<T, inlineCapacity, HeapAllocator>> { };
template<typename T, typename U, typename V>
struct ThreadingTrait<HeapHashCountedSet<T, U, V>> : public ThreadingTrait<HashCountedSet<T, U, V, HeapAllocator>> { };

} // namespace blink

#endif
