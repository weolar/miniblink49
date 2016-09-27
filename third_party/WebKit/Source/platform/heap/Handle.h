/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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

#ifndef Handle_h
#define Handle_h

#include "platform/heap/Heap.h"
#include "platform/heap/HeapAllocator.h"
#include "platform/heap/InlinedGlobalMarkingVisitor.h"
#include "platform/heap/PersistentNode.h"
#include "platform/heap/ThreadState.h"
#include "platform/heap/TraceTraits.h"
#include "platform/heap/Visitor.h"
#include "wtf/Functional.h"
#include "wtf/HashFunctions.h"
#include "wtf/Locker.h"
#include "wtf/MainThread.h"
#include "wtf/RawPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/TypeTraits.h"

namespace blink {

template<typename T> class HeapTerminatedArray;

// Persistent handles are used to store pointers into the
// managed heap. As long as the Persistent handle is alive
// the GC will keep the object pointed to alive. Persistent
// handles can be stored in objects and they are not scoped.
// Persistent handles must not be used to contain pointers
// between objects that are in the managed heap. They are only
// meant to point to managed heap objects from variables/members
// outside the managed heap.
//
// A Persistent is always a GC root from the point of view of
// the garbage collector.
//
// We have to construct and destruct Persistent in the same thread.
template<typename T>
class Persistent final {
public:
    Persistent() : m_raw(nullptr)
    {
        initialize();
    }

    Persistent(std::nullptr_t) : m_raw(nullptr)
    {
        initialize();
    }

    Persistent(T* raw) : m_raw(raw)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    Persistent(T& raw) : m_raw(&raw)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    Persistent(const Persistent& other) : m_raw(other)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    template<typename U>
    Persistent(const Persistent<U>& other) : m_raw(other)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    template<typename U>
    Persistent(const Member<U>& other) : m_raw(other)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    template<typename U>
    Persistent(const RawPtr<U>& other) : m_raw(other.get())
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    void clear() { m_raw = nullptr; }

    ~Persistent()
    {
        uninitialize();
        m_raw = nullptr;
    }

    template<typename VisitorDispatcher>
    void trace(VisitorDispatcher visitor)
    {
        static_assert(sizeof(T), "T must be fully defined");
        static_assert(IsGarbageCollectedType<T>::value, "T needs to be a garbage collected object");
        visitor->mark(m_raw);
    }

    RawPtr<T> release()
    {
        RawPtr<T> result = m_raw;
        m_raw = nullptr;
        return result;
    }

    T& operator*() const { return *m_raw; }

    bool operator!() const { return !m_raw; }

    operator T*() const { return m_raw; }
    operator RawPtr<T>() const { return m_raw; }

    T* operator->() const { return *this; }

    template<typename U>
    Persistent& operator=(U* other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    Persistent& operator=(std::nullptr_t)
    {
        m_raw = nullptr;
        return *this;
    }

    Persistent& operator=(const Persistent& other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    template<typename U>
    Persistent& operator=(const Persistent<U>& other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    template<typename U>
    Persistent& operator=(const Member<U>& other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    template<typename U>
    Persistent& operator=(const RawPtr<U>& other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    T* get() const { return m_raw; }

private:
    NO_LAZY_SWEEP_SANITIZE_ADDRESS
    void initialize()
    {
        ThreadState* state = ThreadStateFor<ThreadingTrait<T>::Affinity>::state();
        ASSERT(state->checkThread());
        m_persistentNode = state->persistentRegion()->allocatePersistentNode(this, TraceMethodDelegate<Persistent<T>, &Persistent<T>::trace>::trampoline);
#if ENABLE(ASSERT)
        m_state = state;
#endif
    }

    void uninitialize()
    {
        ThreadState* state = ThreadStateFor<ThreadingTrait<T>::Affinity>::state();
        ASSERT(state->checkThread());
        // Persistent handle must be created and destructed in the same thread.
        ASSERT(m_state == state);
        state->persistentRegion()->freePersistentNode(m_persistentNode);
    }

    void checkPointer()
    {
#if ENABLE(ASSERT)
        if (!m_raw)
            return;

        // Heap::isHeapObjectAlive(m_raw) checks that m_raw is a traceable
        // object. In other words, it checks that the pointer is either of:
        //
        //   (a) a pointer to the head of an on-heap object.
        //   (b) a pointer to the head of an on-heap mixin object.
        //
        // Otherwise, Heap::isHeapObjectAlive will crash when it calls
        // header->checkHeader().
        Heap::isHeapObjectAlive(m_raw);
#endif
    }

#if ENABLE(GC_PROFILING)
    void recordBacktrace()
    {
        if (m_raw)
            m_tracingName = Heap::createBacktraceString();
    }

    String m_tracingName;
#else
    inline void recordBacktrace() const { }
#endif
    // m_raw is accessed most, so put it at the first field.
    T* m_raw;
    PersistentNode* m_persistentNode;
#if ENABLE(ASSERT)
    ThreadState* m_state;
#endif
};

// Unlike Persistent, we can destruct a CrossThreadPersistent in a thread
// different from the construction thread.
template<typename T>
class CrossThreadPersistent final {
public:
    CrossThreadPersistent() : m_raw(nullptr)
    {
        initialize();
    }

    CrossThreadPersistent(std::nullptr_t) : m_raw(nullptr)
    {
        initialize();
    }

    CrossThreadPersistent(T* raw) : m_raw(raw)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    CrossThreadPersistent(T& raw) : m_raw(&raw)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    CrossThreadPersistent(const CrossThreadPersistent& other) : m_raw(other)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    template<typename U>
    CrossThreadPersistent(const CrossThreadPersistent<U>& other) : m_raw(other)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    template<typename U>
    CrossThreadPersistent(const Member<U>& other) : m_raw(other)
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    template<typename U>
    CrossThreadPersistent(const RawPtr<U>& other) : m_raw(other.get())
    {
        initialize();
        checkPointer();
        recordBacktrace();
    }

    void clear() { m_raw = nullptr; }

    ~CrossThreadPersistent()
    {
        uninitialize();
        m_raw = nullptr;
    }

    template<typename VisitorDispatcher>
    void trace(VisitorDispatcher visitor)
    {
        static_assert(sizeof(T), "T must be fully defined");
        static_assert(IsGarbageCollectedType<T>::value, "T needs to be a garbage collected object");
        visitor->mark(m_raw);
    }

    RawPtr<T> release()
    {
        RawPtr<T> result = m_raw;
        m_raw = nullptr;
        return result;
    }

    T& operator*() const { return *m_raw; }

    bool operator!() const { return !m_raw; }

    operator T*() const { return m_raw; }
    operator RawPtr<T>() const { return m_raw; }

    T* operator->() const { return *this; }

    template<typename U>
    CrossThreadPersistent& operator=(U* other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    CrossThreadPersistent& operator=(std::nullptr_t)
    {
        m_raw = nullptr;
        return *this;
    }

    CrossThreadPersistent& operator=(const CrossThreadPersistent& other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    template<typename U>
    CrossThreadPersistent& operator=(const CrossThreadPersistent<U>& other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    template<typename U>
    CrossThreadPersistent& operator=(const Member<U>& other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    template<typename U>
    CrossThreadPersistent& operator=(const RawPtr<U>& other)
    {
        m_raw = other;
        checkPointer();
        recordBacktrace();
        return *this;
    }

    T* get() const { return m_raw; }

private:
    NO_LAZY_SWEEP_SANITIZE_ADDRESS
    void initialize()
    {
        m_persistentNode = ThreadState::crossThreadPersistentRegion().allocatePersistentNode(this, TraceMethodDelegate<CrossThreadPersistent<T>, &CrossThreadPersistent<T>::trace>::trampoline);
    }

    void uninitialize()
    {
        ThreadState::crossThreadPersistentRegion().freePersistentNode(m_persistentNode);
    }

    void checkPointer()
    {
#if ENABLE(ASSERT)
        if (!m_raw)
            return;
        // Heap::isHeapObjectAlive(m_raw) checks that m_raw is a traceable
        // object. In other words, it checks that the pointer is either of:
        //
        //   (a) a pointer to the head of an on-heap object.
        //   (b) a pointer to the head of an on-heap mixin object.
        //
        // Otherwise, Heap::isHeapObjectAlive will crash when it calls
        // header->checkHeader().
        Heap::isHeapObjectAlive(m_raw);
#endif
    }

#if ENABLE(GC_PROFILING)
    void recordBacktrace()
    {
        if (m_raw)
            m_tracingName = Heap::createBacktraceString();
    }

    String m_tracingName;
#else
    inline void recordBacktrace() const { }
#endif
    // m_raw is accessed most, so put it at the first field.
    T* m_raw;
    PersistentNode* m_persistentNode;
};

// PersistentNode must be the left-most class to let the
// visitor->trace(static_cast<Collection*>(this)) trace the correct position.
// FIXME: derive affinity based on the collection.
template<typename Collection>
class PersistentHeapCollectionBase : public Collection {
    // We overload the various new and delete operators with using the WTF DefaultAllocator to ensure persistent
    // heap collections are always allocated off-heap. This allows persistent collections to be used in
    // DEFINE_STATIC_LOCAL et. al.
    WTF_USE_ALLOCATOR(PersistentHeapCollectionBase, WTF::DefaultAllocator);
public:
    PersistentHeapCollectionBase()
    {
        initialize();
    }

    PersistentHeapCollectionBase(const PersistentHeapCollectionBase& other) : Collection(other)
    {
        initialize();
    }

    template<typename OtherCollection>
    PersistentHeapCollectionBase(const OtherCollection& other) : Collection(other)
    {
        initialize();
    }

    ~PersistentHeapCollectionBase()
    {
        uninitialize();
    }

    template<typename VisitorDispatcher>
    void trace(VisitorDispatcher visitor)
    {
        static_assert(sizeof(Collection), "Collection must be fully defined");
        visitor->trace(*static_cast<Collection*>(this));
    }

private:
    NO_LAZY_SWEEP_SANITIZE_ADDRESS
    void initialize()
    {
        ThreadState* state = ThreadState::current();
        ASSERT(state->checkThread());
        m_persistentNode = state->persistentRegion()->allocatePersistentNode(this, TraceMethodDelegate<PersistentHeapCollectionBase<Collection>, &PersistentHeapCollectionBase<Collection>::trace>::trampoline);
#if ENABLE(ASSERT)
        m_state = state;
#endif
    }

    void uninitialize()
    {
        ThreadState* state = ThreadState::current();
        ASSERT(state->checkThread());
        // Persistent handle must be created and destructed in the same thread.
        ASSERT(m_state == state);
        state->persistentRegion()->freePersistentNode(m_persistentNode);
    }

    PersistentNode* m_persistentNode;
#if ENABLE(ASSERT)
    ThreadState* m_state;
#endif
};

template<
    typename KeyArg,
    typename MappedArg,
    typename HashArg = typename DefaultHash<KeyArg>::Hash,
    typename KeyTraitsArg = HashTraits<KeyArg>,
    typename MappedTraitsArg = HashTraits<MappedArg>>
class PersistentHeapHashMap : public PersistentHeapCollectionBase<HeapHashMap<KeyArg, MappedArg, HashArg, KeyTraitsArg, MappedTraitsArg>> { };

template<
    typename ValueArg,
    typename HashArg = typename DefaultHash<ValueArg>::Hash,
    typename TraitsArg = HashTraits<ValueArg>>
class PersistentHeapHashSet : public PersistentHeapCollectionBase<HeapHashSet<ValueArg, HashArg, TraitsArg>> { };

template<
    typename ValueArg,
    typename HashArg = typename DefaultHash<ValueArg>::Hash,
    typename TraitsArg = HashTraits<ValueArg>>
class PersistentHeapLinkedHashSet : public PersistentHeapCollectionBase<HeapLinkedHashSet<ValueArg, HashArg, TraitsArg>> { };

template<
    typename ValueArg,
    size_t inlineCapacity = 0,
    typename HashArg = typename DefaultHash<ValueArg>::Hash>
class PersistentHeapListHashSet : public PersistentHeapCollectionBase<HeapListHashSet<ValueArg, inlineCapacity, HashArg>> { };

template<typename T, typename U, typename V>
class PersistentHeapHashCountedSet : public PersistentHeapCollectionBase<HeapHashCountedSet<T, U, V>> { };

template<typename T, size_t inlineCapacity = 0>
class PersistentHeapVector : public PersistentHeapCollectionBase<HeapVector<T, inlineCapacity>> {
public:
    PersistentHeapVector() { }

    template<size_t otherCapacity>
    PersistentHeapVector(const HeapVector<T, otherCapacity>& other)
        : PersistentHeapCollectionBase<HeapVector<T, inlineCapacity>>(other)
    {
    }
};

template<typename T, size_t inlineCapacity = 0>
class PersistentHeapDeque : public PersistentHeapCollectionBase<HeapDeque<T, inlineCapacity>> {
public:
    PersistentHeapDeque() { }

    template<size_t otherCapacity>
    PersistentHeapDeque(const HeapDeque<T, otherCapacity>& other)
        : PersistentHeapCollectionBase<HeapDeque<T, inlineCapacity>>(other)
    {
    }
};

// Members are used in classes to contain strong pointers to other oilpan heap
// allocated objects.
// All Member fields of a class must be traced in the class' trace method.
// During the mark phase of the GC all live objects are marked as live and
// all Member fields of a live object will be traced marked as live as well.
template<typename T>
class Member {
public:
    Member() : m_raw(nullptr)
    {
    }

    Member(std::nullptr_t) : m_raw(nullptr)
    {
    }

    Member(T* raw) : m_raw(raw)
    {
        checkPointer();
    }

    explicit Member(T& raw) : m_raw(&raw)
    {
        checkPointer();
    }

    template<typename U>
    Member(const RawPtr<U>& other) : m_raw(other.get())
    {
        checkPointer();
    }

    Member(WTF::HashTableDeletedValueType) : m_raw(reinterpret_cast<T*>(-1))
    {
    }

    bool isHashTableDeletedValue() const { return m_raw == reinterpret_cast<T*>(-1); }

    template<typename U>
    Member(const Persistent<U>& other) : m_raw(other)
    {
        checkPointer();
    }

    Member(const Member& other) : m_raw(other)
    {
        checkPointer();
    }

    template<typename U>
    Member(const Member<U>& other) : m_raw(other)
    {
        checkPointer();
    }

    T* release()
    {
        T* result = m_raw;
        m_raw = nullptr;
        return result;
    }

    bool operator!() const { return !m_raw; }

    operator T*() const { return m_raw; }

    T* operator->() const { return m_raw; }
    T& operator*() const { return *m_raw; }
    template<typename U>
    operator RawPtr<U>() const { return m_raw; }

    template<typename U>
    Member& operator=(const Persistent<U>& other)
    {
        m_raw = other;
        checkPointer();
        return *this;
    }

    template<typename U>
    Member& operator=(const Member<U>& other)
    {
        m_raw = other;
        checkPointer();
        return *this;
    }

    template<typename U>
    Member& operator=(U* other)
    {
        m_raw = other;
        checkPointer();
        return *this;
    }

    template<typename U>
    Member& operator=(RawPtr<U> other)
    {
        m_raw = other;
        checkPointer();
        return *this;
    }

    Member& operator=(std::nullptr_t)
    {
        m_raw = nullptr;
        return *this;
    }

    void swap(Member<T>& other)
    {
        std::swap(m_raw, other.m_raw);
        checkPointer();
    }

    T* get() const { return m_raw; }

    void clear() { m_raw = nullptr; }


protected:
    void checkPointer()
    {
#if ENABLE(ASSERT)
        if (!m_raw)
            return;
        // HashTable can store a special value (which is not aligned to the
        // allocation granularity) to Member<> to represent a deleted entry.
        // Thus we treat a pointer that is not aligned to the granularity
        // as a valid pointer.
        if (reinterpret_cast<intptr_t>(m_raw) % allocationGranularity)
            return;

        // TODO(haraken): What we really want to check here is that the pointer
        // is a traceable object. In other words, the pointer is either of:
        //
        //   (a) a pointer to the head of an on-heap object.
        //   (b) a pointer to the head of an on-heap mixin object.
        //
        // We can check it by calling Heap::isHeapObjectAlive(m_raw),
        // but we cannot call it here because it requires to include T.h.
        // So we currently only try to implement the check for (a), but do
        // not insist that T's definition is in scope.
        if (IsFullyDefined<T>::value && !IsGarbageCollectedMixin<T>::value)
            ASSERT(HeapObjectHeader::fromPayload(m_raw)->checkHeader());
#endif
    }

    T* m_raw;

    template<bool x, WTF::WeakHandlingFlag y, WTF::ShouldWeakPointersBeMarkedStrongly z, typename U, typename V> friend struct CollectionBackingTraceTrait;
    friend class Visitor;

};

// WeakMember is similar to Member in that it is used to point to other oilpan
// heap allocated objects.
// However instead of creating a strong pointer to the object, the WeakMember creates
// a weak pointer, which does not keep the pointee alive. Hence if all pointers to
// to a heap allocated object are weak the object will be garbage collected. At the
// time of GC the weak pointers will automatically be set to null.
template<typename T>
class WeakMember : public Member<T> {
public:
    WeakMember() : Member<T>() { }

    WeakMember(std::nullptr_t) : Member<T>(nullptr) { }

    WeakMember(T* raw) : Member<T>(raw) { }

    WeakMember(WTF::HashTableDeletedValueType x) : Member<T>(x) { }

    template<typename U>
    WeakMember(const Persistent<U>& other) : Member<T>(other) { }

    template<typename U>
    WeakMember(const Member<U>& other) : Member<T>(other) { }

    template<typename U>
    WeakMember& operator=(const Persistent<U>& other)
    {
        this->m_raw = other;
        this->checkPointer();
        return *this;
    }

    template<typename U>
    WeakMember& operator=(const Member<U>& other)
    {
        this->m_raw = other;
        this->checkPointer();
        return *this;
    }

    template<typename U>
    WeakMember& operator=(U* other)
    {
        this->m_raw = other;
        this->checkPointer();
        return *this;
    }

    template<typename U>
    WeakMember& operator=(const RawPtr<U>& other)
    {
        this->m_raw = other;
        this->checkPointer();
        return *this;
    }

    WeakMember& operator=(std::nullptr_t)
    {
        this->m_raw = nullptr;
        return *this;
    }

private:
    T** cell() const { return const_cast<T**>(&this->m_raw); }

    template<typename Derived> friend class VisitorHelper;
};

// Comparison operators between (Weak)Members and Persistents
template<typename T, typename U> inline bool operator==(const Member<T>& a, const Member<U>& b) { return a.get() == b.get(); }
template<typename T, typename U> inline bool operator!=(const Member<T>& a, const Member<U>& b) { return a.get() != b.get(); }
template<typename T, typename U> inline bool operator==(const Member<T>& a, const Persistent<U>& b) { return a.get() == b.get(); }
template<typename T, typename U> inline bool operator!=(const Member<T>& a, const Persistent<U>& b) { return a.get() != b.get(); }
template<typename T, typename U> inline bool operator==(const Persistent<T>& a, const Member<U>& b) { return a.get() == b.get(); }
template<typename T, typename U> inline bool operator!=(const Persistent<T>& a, const Member<U>& b) { return a.get() != b.get(); }
template<typename T, typename U> inline bool operator==(const Persistent<T>& a, const Persistent<U>& b) { return a.get() == b.get(); }
template<typename T, typename U> inline bool operator!=(const Persistent<T>& a, const Persistent<U>& b) { return a.get() != b.get(); }

template<typename T>
class DummyBase {
public:
    DummyBase() { }
    ~DummyBase() { }
};

// CPP-defined type names for the transition period where we want to
// support both reference counting and garbage collection based on a
// compile-time flag.
//
// C++11 template aliases were initially used (with clang only, not
// with GCC nor MSVC.) However, supporting both CPP defines and
// template aliases is problematic from outside a WebCore namespace
// when Oilpan is disabled: e.g.,
// blink::RefCountedWillBeGarbageCollected as a template alias would
// uniquely resolve from within any namespace, but if it is backed by
// a CPP #define, it would expand to blink::RefCounted, and not the
// required WTF::RefCounted.
//
// Having the CPP expansion instead be fully namespace qualified, and the
// transition type be unqualified, would dually not work for template
// aliases. So, slightly unfortunately, fall back/down to the lowest
// commmon denominator of using CPP macros only.
#if ENABLE(OILPAN)
#define PassRefPtrWillBeRawPtr WTF::RawPtr
#define RefCountedWillBeGarbageCollected blink::GarbageCollected
#define RefCountedWillBeGarbageCollectedFinalized blink::GarbageCollectedFinalized
#define RefCountedWillBeRefCountedGarbageCollected blink::RefCountedGarbageCollected
#define RefCountedGarbageCollectedWillBeGarbageCollectedFinalized blink::GarbageCollectedFinalized
#define RefCountedWillBeNoBase blink::DummyBase
#define RefCountedGarbageCollectedWillBeNoBase blink::DummyBase
#define ThreadSafeRefCountedWillBeGarbageCollected blink::GarbageCollected
#define ThreadSafeRefCountedWillBeGarbageCollectedFinalized blink::GarbageCollectedFinalized
#define PersistentWillBeMember blink::Member
#define CrossThreadPersistentWillBeMember blink::Member
#define RefPtrWillBePersistent blink::Persistent
#define RefPtrWillBeRawPtr WTF::RawPtr
#define RefPtrWillBeMember blink::Member
#define RefPtrWillBeWeakMember blink::WeakMember
#define RefPtrWillBeCrossThreadPersistent blink::CrossThreadPersistent
#define RawPtrWillBeMember blink::Member
#define RawPtrWillBePersistent blink::Persistent
#define RawPtrWillBeWeakMember blink::WeakMember
#define OwnPtrWillBeCrossThreadPersistent blink::CrossThreadPersistent
#define OwnPtrWillBeMember blink::Member
#define OwnPtrWillBePersistent blink::Persistent
#define OwnPtrWillBeRawPtr WTF::RawPtr
#define PassOwnPtrWillBeRawPtr WTF::RawPtr
#define WeakPtrWillBeMember blink::Member
#define WeakPtrWillBeRawPtr WTF::RawPtr
#define WeakPtrWillBeWeakMember blink::WeakMember
#define NoBaseWillBeGarbageCollected blink::GarbageCollected
#define NoBaseWillBeGarbageCollectedFinalized blink::GarbageCollectedFinalized
#define NoBaseWillBeRefCountedGarbageCollected blink::RefCountedGarbageCollected
#define WillBeHeapHashMap blink::HeapHashMap
#define WillBePersistentHeapHashMap blink::PersistentHeapHashMap
#define WillBeHeapHashSet blink::HeapHashSet
#define WillBePersistentHeapHashSet blink::PersistentHeapHashSet
#define WillBeHeapLinkedHashSet blink::HeapLinkedHashSet
#define WillBePersistentHeapLinkedHashSet blink::PersistentHeapLinkedHashSet
#define WillBeHeapListHashSet blink::HeapListHashSet
#define WillBePersistentHeapListHashSet blink::PersistentHeapListHashSet
#define WillBeHeapVector blink::HeapVector
#define WillBePersistentHeapVector blink::PersistentHeapVector
#define WillBeHeapDeque blink::HeapDeque
#define WillBePersistentHeapDeque blink::PersistentHeapDeque
#define WillBeHeapHashCountedSet blink::HeapHashCountedSet
#define WillBePersistentHeapHashCountedSet blink::PersistentHeapHashCountedSet
#define WillBeGarbageCollectedMixin blink::GarbageCollectedMixin
#define WillBeHeapSupplement blink::HeapSupplement
#define WillBeHeapSupplementable blink::HeapSupplementable
#define WillBeHeapTerminatedArray blink::HeapTerminatedArray
#define WillBeHeapTerminatedArrayBuilder blink::HeapTerminatedArrayBuilder
#define WillBeHeapLinkedStack blink::HeapLinkedStack
#define PersistentHeapHashMapWillBeHeapHashMap blink::HeapHashMap
#define PersistentHeapHashSetWillBeHeapHashSet blink::HeapHashSet
#define PersistentHeapDequeWillBeHeapDeque blink::HeapDeque
#define PersistentHeapVectorWillBeHeapVector blink::HeapVector

template<typename T> T* adoptRefWillBeNoop(T* ptr)
{
    static const bool notRefCounted = !WTF::IsSubclassOfTemplate<typename WTF::RemoveConst<T>::Type, RefCounted>::value;
    static_assert(notRefCounted, "you must adopt");
    return ptr;
}

template<typename T> T* adoptPtrWillBeNoop(T* ptr)
{
    static const bool notRefCounted = !WTF::IsSubclassOfTemplate<typename WTF::RemoveConst<T>::Type, RefCounted>::value;
    static_assert(notRefCounted, "you must adopt");
    return ptr;
}

#define WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(type) // do nothing when oilpan is enabled.
#define DECLARE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(type) // do nothing
#define DECLARE_EMPTY_VIRTUAL_DESTRUCTOR_WILL_BE_REMOVED(type) // do nothing
#define DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(type) // do nothing

#define DEFINE_STATIC_REF_WILL_BE_PERSISTENT(type, name, arguments) \
    static type* name = (new Persistent<type>(arguments))->get();

#else // !ENABLE(OILPAN)

#define PassRefPtrWillBeRawPtr WTF::PassRefPtr
#define RefCountedWillBeGarbageCollected WTF::RefCounted
#define RefCountedWillBeGarbageCollectedFinalized WTF::RefCounted
#define RefCountedWillBeRefCountedGarbageCollected WTF::RefCounted
#define RefCountedGarbageCollectedWillBeGarbageCollectedFinalized blink::RefCountedGarbageCollected
#define RefCountedWillBeNoBase WTF::RefCounted
#define RefCountedGarbageCollectedWillBeNoBase blink::RefCountedGarbageCollected
#define ThreadSafeRefCountedWillBeGarbageCollected WTF::ThreadSafeRefCounted
#define ThreadSafeRefCountedWillBeGarbageCollectedFinalized WTF::ThreadSafeRefCounted
#define PersistentWillBeMember blink::Persistent
#define CrossThreadPersistentWillBeMember blink::CrossThreadPersistent
#define RefPtrWillBePersistent WTF::RefPtr
#define RefPtrWillBeRawPtr WTF::RefPtr
#define RefPtrWillBeMember WTF::RefPtr
#define RefPtrWillBeWeakMember WTF::RefPtr
#define RefPtrWillBeCrossThreadPersistent WTF::RefPtr
#define RawPtrWillBeMember WTF::RawPtr
#define RawPtrWillBePersistent WTF::RawPtr
#define RawPtrWillBeWeakMember WTF::RawPtr
#define OwnPtrWillBeCrossThreadPersistent WTF::OwnPtr
#define OwnPtrWillBeMember WTF::OwnPtr
#define OwnPtrWillBePersistent WTF::OwnPtr
#define OwnPtrWillBeRawPtr WTF::OwnPtr
#define PassOwnPtrWillBeRawPtr WTF::PassOwnPtr
#define WeakPtrWillBeMember WTF::WeakPtr
#define WeakPtrWillBeRawPtr WTF::WeakPtr
#define WeakPtrWillBeWeakMember WTF::WeakPtr
#define NoBaseWillBeGarbageCollected blink::DummyBase
#define NoBaseWillBeGarbageCollectedFinalized blink::DummyBase
#define NoBaseWillBeRefCountedGarbageCollected blink::DummyBase
#define WillBeHeapHashMap WTF::HashMap
#define WillBePersistentHeapHashMap WTF::HashMap
#define WillBeHeapHashSet WTF::HashSet
#define WillBePersistentHeapHashSet WTF::HashSet
#define WillBeHeapLinkedHashSet WTF::LinkedHashSet
#define WillBePersistentLinkedHeapHashSet WTF::LinkedHashSet
#define WillBeHeapListHashSet WTF::ListHashSet
#define WillBePersistentListHeapHashSet WTF::ListHashSet
#define WillBeHeapVector WTF::Vector
#define WillBePersistentHeapVector WTF::Vector
#define WillBeHeapDeque WTF::Deque
#define WillBePersistentHeapDeque WTF::Deque
#define WillBeHeapHashCountedSet WTF::HashCountedSet
#define WillBePersistentHeapHashCountedSet WTF::HashCountedSet
#define WillBeGarbageCollectedMixin blink::DummyBase<void>
#define WillBeHeapSupplement blink::Supplement
#define WillBeHeapSupplementable blink::Supplementable
#define WillBeHeapTerminatedArray WTF::TerminatedArray
#define WillBeHeapTerminatedArrayBuilder WTF::TerminatedArrayBuilder
#define WillBeHeapLinkedStack WTF::LinkedStack
#define PersistentHeapHashMapWillBeHeapHashMap blink::PersistentHeapHashMap
#define PersistentHeapHashSetWillBeHeapHashSet blink::PersistentHeapHashSet
#define PersistentHeapDequeWillBeHeapDeque blink::PersistentHeapDeque
#define PersistentHeapVectorWillBeHeapVector blink::PersistentHeapVector

template<typename T> PassRefPtrWillBeRawPtr<T> adoptRefWillBeNoop(T* ptr) { return adoptRef(ptr); }
template<typename T> PassOwnPtrWillBeRawPtr<T> adoptPtrWillBeNoop(T* ptr) { return adoptPtr(ptr); }

#define WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(type) WTF_MAKE_FAST_ALLOCATED(type)
#define DECLARE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(type) \
    public:                                            \
        ~type();                                       \
    private:
#define DECLARE_EMPTY_VIRTUAL_DESTRUCTOR_WILL_BE_REMOVED(type) \
    public:                                                    \
        virtual ~type();                                       \
    private:

#define DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(type) \
    type::~type() { }

#define DEFINE_STATIC_REF_WILL_BE_PERSISTENT(type, name, arguments) \
    DEFINE_STATIC_REF(type, name, arguments)

#endif // ENABLE(OILPAN)

// Abstraction for injecting calls to an object's 'dispose()' method
// on leaving a stack scope, ensuring earlier release of resources
// than waiting until the object is eventually GCed.
template<typename T>
class ScopedDisposal {
    STACK_ALLOCATED();
public:
    ScopedDisposal(T* object)
        : m_object(object)
    {
        static_assert(IsGarbageCollectedType<T>::value, "can only be used with garbage collected types");
    }

    ~ScopedDisposal()
    {
        if (m_object)
            m_object->dispose();
    }

    void clear() { m_object.clear(); }

private:
    Member<T> m_object;
};

} // namespace blink

namespace WTF {

template <typename T> struct VectorTraits<blink::Member<T>> : VectorTraitsBase<blink::Member<T>> {
    static const bool needsDestruction = false;
    static const bool canInitializeWithMemset = true;
    static const bool canClearUnusedSlotsWithMemset = true;
    static const bool canMoveWithMemcpy = true;
};

template <typename T> struct VectorTraits<blink::WeakMember<T>> : VectorTraitsBase<blink::WeakMember<T>> {
    static const bool needsDestruction = false;
    static const bool canInitializeWithMemset = true;
    static const bool canClearUnusedSlotsWithMemset = true;
    static const bool canMoveWithMemcpy = true;
};

template <typename T> struct VectorTraits<blink::HeapVector<T, 0>> : VectorTraitsBase<blink::HeapVector<T, 0>> {
    static const bool needsDestruction = false;
    static const bool canInitializeWithMemset = true;
    static const bool canClearUnusedSlotsWithMemset = true;
    static const bool canMoveWithMemcpy = true;
};

template <typename T> struct VectorTraits<blink::HeapDeque<T, 0>> : VectorTraitsBase<blink::HeapDeque<T, 0>> {
    static const bool needsDestruction = false;
    static const bool canInitializeWithMemset = true;
    static const bool canClearUnusedSlotsWithMemset = true;
    static const bool canMoveWithMemcpy = true;
};

template <typename T, size_t inlineCapacity> struct VectorTraits<blink::HeapVector<T, inlineCapacity>> : VectorTraitsBase<blink::HeapVector<T, inlineCapacity>> {
    static const bool needsDestruction = VectorTraits<T>::needsDestruction;
    static const bool canInitializeWithMemset = VectorTraits<T>::canInitializeWithMemset;
    static const bool canClearUnusedSlotsWithMemset = VectorTraits<T>::canClearUnusedSlotsWithMemset;
    static const bool canMoveWithMemcpy = VectorTraits<T>::canMoveWithMemcpy;
};

template <typename T, size_t inlineCapacity> struct VectorTraits<blink::HeapDeque<T, inlineCapacity>> : VectorTraitsBase<blink::HeapDeque<T, inlineCapacity>> {
    static const bool needsDestruction = VectorTraits<T>::needsDestruction;
    static const bool canInitializeWithMemset = VectorTraits<T>::canInitializeWithMemset;
    static const bool canClearUnusedSlotsWithMemset = VectorTraits<T>::canClearUnusedSlotsWithMemset;
    static const bool canMoveWithMemcpy = VectorTraits<T>::canMoveWithMemcpy;
};

template<typename T> struct HashTraits<blink::Member<T>> : SimpleClassHashTraits<blink::Member<T>> {
    // FIXME: The distinction between PeekInType and PassInType is there for
    // the sake of the reference counting handles. When they are gone the two
    // types can be merged into PassInType.
    // FIXME: Implement proper const'ness for iterator types. Requires support
    // in the marking Visitor.
    using PeekInType = RawPtr<T>;
    using PassInType = RawPtr<T>;
    using IteratorGetType = blink::Member<T>*;
    using IteratorConstGetType = const blink::Member<T>*;
    using IteratorReferenceType = blink::Member<T>&;
    using IteratorConstReferenceType = const blink::Member<T>&;
    static IteratorReferenceType getToReferenceConversion(IteratorGetType x) { return *x; }
    static IteratorConstReferenceType getToReferenceConstConversion(IteratorConstGetType x) { return *x; }
    // FIXME: Similarly, there is no need for a distinction between PeekOutType
    // and PassOutType without reference counting.
    using PeekOutType = T*;
    using PassOutType = T*;

    template<typename U>
    static void store(const U& value, blink::Member<T>& storage) { storage = value; }

    static PeekOutType peek(const blink::Member<T>& value) { return value; }
    static PassOutType passOut(const blink::Member<T>& value) { return value; }
};

template<typename T> struct HashTraits<blink::WeakMember<T>> : SimpleClassHashTraits<blink::WeakMember<T>> {
    static const bool needsDestruction = false;
    // FIXME: The distinction between PeekInType and PassInType is there for
    // the sake of the reference counting handles. When they are gone the two
    // types can be merged into PassInType.
    // FIXME: Implement proper const'ness for iterator types. Requires support
    // in the marking Visitor.
    using PeekInType = RawPtr<T>;
    using PassInType = RawPtr<T>;
    using IteratorGetType = blink::WeakMember<T>*;
    using IteratorConstGetType = const blink::WeakMember<T>*;
    using IteratorReferenceType = blink::WeakMember<T>&;
    using IteratorConstReferenceType = const blink::WeakMember<T>&;
    static IteratorReferenceType getToReferenceConversion(IteratorGetType x) { return *x; }
    static IteratorConstReferenceType getToReferenceConstConversion(IteratorConstGetType x) { return *x; }
    // FIXME: Similarly, there is no need for a distinction between PeekOutType
    // and PassOutType without reference counting.
    using PeekOutType = T*;
    using PassOutType = T*;

    template<typename U>
    static void store(const U& value, blink::WeakMember<T>& storage) { storage = value; }

    static PeekOutType peek(const blink::WeakMember<T>& value) { return value; }
    static PassOutType passOut(const blink::WeakMember<T>& value) { return value; }

    template<typename VisitorDispatcher>
    static bool traceInCollection(VisitorDispatcher visitor, blink::WeakMember<T>& weakMember, ShouldWeakPointersBeMarkedStrongly strongify)
    {
        if (strongify == WeakPointersActStrong) {
            visitor->trace(weakMember.get()); // Strongified visit.
            return false;
        }
        return !blink::Heap::isHeapObjectAlive(weakMember);
    }
};

template<typename T> struct PtrHash<blink::Member<T>> : PtrHash<T*> {
    template<typename U>
    static unsigned hash(const U& key) { return PtrHash<T*>::hash(key); }
    static bool equal(T* a, const blink::Member<T>& b) { return a == b; }
    static bool equal(const blink::Member<T>& a, T* b) { return a == b; }
    template<typename U, typename V>
    static bool equal(const U& a, const V& b) { return a == b; }
};

template<typename T> struct PtrHash<blink::WeakMember<T>> : PtrHash<blink::Member<T>> {
};

template<typename P> struct PtrHash<blink::Persistent<P>> : PtrHash<P*> {
    using PtrHash<P*>::hash;
    static unsigned hash(const RefPtr<P>& key) { return hash(key.get()); }
    using PtrHash<P*>::equal;
    static bool equal(const RefPtr<P>& a, const RefPtr<P>& b) { return a == b; }
    static bool equal(P* a, const RefPtr<P>& b) { return a == b; }
    static bool equal(const RefPtr<P>& a, P* b) { return a == b; }
};

// PtrHash is the default hash for hash tables with members.
template<typename T> struct DefaultHash<blink::Member<T>> {
    using Hash = PtrHash<blink::Member<T>>;
};

template<typename T> struct DefaultHash<blink::WeakMember<T>> {
    using Hash = PtrHash<blink::WeakMember<T>>;
};

template<typename T> struct DefaultHash<blink::Persistent<T>> {
    using Hash = PtrHash<blink::Persistent<T>>;
};

template<typename T>
struct NeedsTracing<blink::Member<T>> {
    static const bool value = true;
};

template<typename T>
struct IsWeak<blink::WeakMember<T>> {
    static const bool value = true;
};

template<typename T> inline T* getPtr(const blink::Member<T>& p)
{
    return p.get();
}

template<typename T> inline T* getPtr(const blink::Persistent<T>& p)
{
    return p.get();
}

template<typename T, size_t inlineCapacity>
struct NeedsTracing<ListHashSetNode<T, blink::HeapListHashSetAllocator<T, inlineCapacity>> *> {
    static_assert(sizeof(T), "T must be fully defined");
    // All heap allocated node pointers need visiting to keep the nodes alive,
    // regardless of whether they contain pointers to other heap allocated
    // objects.
    static const bool value = true;
};

// For wtf/Functional.h
template<typename T, bool isGarbageCollected> struct PointerParamStorageTraits;

template<typename T>
struct PointerParamStorageTraits<T*, false> {
    static_assert(sizeof(T), "T must be fully defined");
    using StorageType = T*;

    static StorageType wrap(T* value) { return value; }
    static T* unwrap(const StorageType& value) { return value; }
};

template<typename T>
struct PointerParamStorageTraits<T*, true> {
    static_assert(sizeof(T), "T must be fully defined");
    using StorageType = blink::CrossThreadPersistent<T>;

    static StorageType wrap(T* value) { return value; }
    static T* unwrap(const StorageType& value) { return value.get(); }
};

template<typename T>
struct ParamStorageTraits<T*> : public PointerParamStorageTraits<T*, blink::IsGarbageCollectedType<T>::value> {
    static_assert(sizeof(T), "T must be fully defined");
};

template<typename T>
struct ParamStorageTraits<RawPtr<T>> : public PointerParamStorageTraits<T*, blink::IsGarbageCollectedType<T>::value> {
    static_assert(sizeof(T), "T must be fully defined");
};

template<typename T>
PassRefPtr<T> adoptRef(blink::RefCountedGarbageCollected<T>*) = delete;

} // namespace WTF

#endif
