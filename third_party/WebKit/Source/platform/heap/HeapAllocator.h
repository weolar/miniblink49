// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HeapAllocator_h
#define HeapAllocator_h

#include "platform/heap/Heap.h"
#include "platform/heap/TraceTraits.h"
#include "wtf/Assertions.h"
#include "wtf/Atomics.h"
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

// This is a static-only class used as a trait on collections to make them heap
// allocated.  However see also HeapListHashSetAllocator.
class PLATFORM_EXPORT HeapAllocator {
public:
    using Visitor = blink::Visitor;
    static const bool isGarbageCollected = true;

    template<typename T>
    static size_t quantizedSize(size_t count)
    {
        RELEASE_ASSERT(count <= maxHeapObjectSize / sizeof(T));
        return Heap::allocationSizeFromSize(count * sizeof(T)) - sizeof(HeapObjectHeader);
    }
    template <typename T>
    static T* allocateVectorBacking(size_t size)
    {
        ThreadState* state = ThreadStateFor<ThreadingTrait<T>::Affinity>::state();
        ASSERT(state->isAllocationAllowed());
        size_t gcInfoIndex = GCInfoTrait<HeapVectorBacking<T, VectorTraits<T>>>::index();
        NormalPageHeap* heap = static_cast<NormalPageHeap*>(state->vectorBackingHeap(gcInfoIndex));
        return reinterpret_cast<T*>(heap->allocateObject(Heap::allocationSizeFromSize(size), gcInfoIndex));
    }
    template <typename T>
    static T* allocateExpandedVectorBacking(size_t size)
    {
        ThreadState* state = ThreadStateFor<ThreadingTrait<T>::Affinity>::state();
        ASSERT(state->isAllocationAllowed());
        size_t gcInfoIndex = GCInfoTrait<HeapVectorBacking<T, VectorTraits<T>>>::index();
        NormalPageHeap* heap = static_cast<NormalPageHeap*>(state->expandedVectorBackingHeap(gcInfoIndex));
        return reinterpret_cast<T*>(heap->allocateObject(Heap::allocationSizeFromSize(size), gcInfoIndex));
    }
    static void freeVectorBacking(void*);
    static bool expandVectorBacking(void*, size_t);
    static bool shrinkVectorBacking(void* address, size_t quantizedCurrentSize, size_t quantizedShrunkSize);
    template <typename T>
    static T* allocateInlineVectorBacking(size_t size)
    {
        size_t gcInfoIndex = GCInfoTrait<HeapVectorBacking<T, VectorTraits<T>>>::index();
        ThreadState* state = ThreadStateFor<ThreadingTrait<T>::Affinity>::state();
        return reinterpret_cast<T*>(Heap::allocateOnHeapIndex(state, size, ThreadState::InlineVectorHeapIndex, gcInfoIndex));
    }
    static void freeInlineVectorBacking(void*);
    static bool expandInlineVectorBacking(void*, size_t);
    static bool shrinkInlineVectorBacking(void* address, size_t quantizedCurrentSize, size_t quantizedShrunkSize);

    template <typename T, typename HashTable>
    static T* allocateHashTableBacking(size_t size)
    {
        size_t gcInfoIndex = GCInfoTrait<HeapHashTableBacking<HashTable>>::index();
        ThreadState* state = ThreadStateFor<ThreadingTrait<T>::Affinity>::state();
        return reinterpret_cast<T*>(Heap::allocateOnHeapIndex(state, size, ThreadState::HashTableHeapIndex, gcInfoIndex));
    }
    template <typename T, typename HashTable>
    static T* allocateZeroedHashTableBacking(size_t size)
    {
        return allocateHashTableBacking<T, HashTable>(size);
    }
    static void freeHashTableBacking(void* address);
    static bool expandHashTableBacking(void*, size_t);

    template <typename Return, typename Metadata>
    static Return malloc(size_t size)
    {
        return reinterpret_cast<Return>(Heap::allocate<Metadata>(size, IsEagerlyFinalizedType<Metadata>::value));
    }
    static void free(void* address) { }
    template<typename T>
    static void* newArray(size_t bytes)
    {
        ASSERT_NOT_REACHED();
        return 0;
    }

    static void deleteArray(void* ptr)
    {
        ASSERT_NOT_REACHED();
    }

    static bool isAllocationAllowed()
    {
        return ThreadState::current()->isAllocationAllowed();
    }

    template<typename T>
    static bool isHeapObjectAlive(T* object)
    {
        return Heap::isHeapObjectAlive(object);
    }

    template<typename VisitorDispatcher>
    static void markNoTracing(VisitorDispatcher visitor, const void* t) { visitor->markNoTracing(t); }

    template<typename VisitorDispatcher, typename T, typename Traits>
    static void trace(VisitorDispatcher visitor, T& t)
    {
        TraceCollectionIfEnabled<WTF::ShouldBeTraced<Traits>::value, Traits::weakHandlingFlag, WTF::WeakPointersActWeak, T, Traits>::trace(visitor, t);
    }

    template<typename VisitorDispatcher>
    static void registerDelayedMarkNoTracing(VisitorDispatcher visitor, const void* object)
    {
        visitor->registerDelayedMarkNoTracing(object);
    }

    template<typename VisitorDispatcher>
    static void registerWeakMembers(VisitorDispatcher visitor, const void* closure, const void* object, WeakCallback callback)
    {
        visitor->registerWeakMembers(closure, object, callback);
    }

    template<typename VisitorDispatcher>
    static void registerWeakTable(VisitorDispatcher visitor, const void* closure, EphemeronCallback iterationCallback, EphemeronCallback iterationDoneCallback)
    {
        visitor->registerWeakTable(closure, iterationCallback, iterationDoneCallback);
    }

#if ENABLE(ASSERT)
    template<typename VisitorDispatcher>
    static bool weakTableRegistered(VisitorDispatcher visitor, const void* closure)
    {
        return visitor->weakTableRegistered(closure);
    }
#endif

    template<typename T>
    struct ResultType {
        using Type = T*;
    };

    template<typename T>
    struct OtherType {
        using Type = T*;
    };

    template<typename T>
    static T& getOther(T* other)
    {
        return *other;
    }

    static void enterNoAllocationScope()
    {
#if ENABLE(ASSERT)
        ThreadState::current()->enterNoAllocationScope();
#endif
    }

    static void leaveNoAllocationScope()
    {
#if ENABLE(ASSERT)
        ThreadState::current()->leaveNoAllocationScope();
#endif
    }

    static void enterGCForbiddenScope()
    {
        ThreadState::current()->enterGCForbiddenScope();
    }

    static void leaveGCForbiddenScope()
    {
        ThreadState::current()->leaveGCForbiddenScope();
    }

private:
    static void backingFree(void*);
    static bool backingExpand(void*, size_t);
    static bool backingShrink(void*, size_t quantizedCurrentSize, size_t quantizedShrunkSize);

    template<typename T, size_t u, typename V> friend class WTF::Vector;
    template<typename T, typename U, typename V, typename W> friend class WTF::HashSet;
    template<typename T, typename U, typename V, typename W, typename X, typename Y> friend class WTF::HashMap;
};

template<typename VisitorDispatcher, typename Value>
static void traceListHashSetValue(VisitorDispatcher visitor, Value& value)
{
    // We use the default hash traits for the value in the node, because
    // ListHashSet does not let you specify any specific ones.
    // We don't allow ListHashSet of WeakMember, so we set that one false
    // (there's an assert elsewhere), but we have to specify some value for the
    // strongify template argument, so we specify WTF::WeakPointersActWeak,
    // arbitrarily.
    TraceCollectionIfEnabled<WTF::ShouldBeTraced<WTF::HashTraits<Value>>::value, WTF::NoWeakHandlingInCollections, WTF::WeakPointersActWeak, Value, WTF::HashTraits<Value>>::trace(visitor, value);
}

// The inline capacity is just a dummy template argument to match the off-heap
// allocator.
// This inherits from the static-only HeapAllocator trait class, but we do
// declare pointers to instances.  These pointers are always null, and no
// objects are instantiated.
template<typename ValueArg, size_t inlineCapacity>
class HeapListHashSetAllocator : public HeapAllocator {
public:
    using TableAllocator = HeapAllocator;
    using Node = WTF::ListHashSetNode<ValueArg, HeapListHashSetAllocator>;

    class AllocatorProvider {
    public:
        // For the heap allocation we don't need an actual allocator object, so
        // we just return null.
        HeapListHashSetAllocator* get() const { return 0; }

        // No allocator object is needed.
        void createAllocatorIfNeeded() { }
        void releaseAllocator() { }

        // There is no allocator object in the HeapListHashSet (unlike in the
        // regular ListHashSet) so there is nothing to swap.
        void swap(AllocatorProvider& other) { }
    };

    void deallocate(void* dummy) { }

    // This is not a static method even though it could be, because it needs to
    // match the one that the (off-heap) ListHashSetAllocator has.  The 'this'
    // pointer will always be null.
    void* allocateNode()
    {
        // Consider using a LinkedHashSet instead if this compile-time assert fails:
        static_assert(!WTF::IsWeak<ValueArg>::value, "weak pointers in a ListHashSet will result in null entries in the set");

        return malloc<void*, Node>(sizeof(Node));
    }

    template<typename VisitorDispatcher>
    static void traceValue(VisitorDispatcher visitor, Node* node)
    {
        traceListHashSetValue(visitor, node->m_value);
    }
};

template<typename T, typename Traits = WTF::VectorTraits<T>> class HeapVectorBacking {
public:
    static void finalize(void* pointer);
    void finalizeGarbageCollectedObject() { finalize(this); }
};

template<typename T, typename Traits>
void HeapVectorBacking<T, Traits>::finalize(void* pointer)
{
    static_assert(Traits::needsDestruction, "Only vector buffers with items requiring destruction should be finalized");
    // See the comment in HeapVectorBacking::trace.
    static_assert(Traits::canClearUnusedSlotsWithMemset || WTF::IsPolymorphic<T>::value, "HeapVectorBacking doesn't support objects that cannot be cleared as unused with memset or don't have a vtable");

    ASSERT(!WTF::IsTriviallyDestructible<T>::value);
    HeapObjectHeader* header = HeapObjectHeader::fromPayload(pointer);
    ASSERT(header->checkHeader());
    // Use the payload size as recorded by the heap to determine how many
    // elements to finalize.
    size_t length = header->payloadSize() / sizeof(T);
    T* buffer = reinterpret_cast<T*>(pointer);
#ifdef ANNOTATE_CONTIGUOUS_CONTAINER
    // As commented above, HeapVectorBacking calls finalizers for unused slots
    // (which are already zeroed out).
    ANNOTATE_CHANGE_SIZE(buffer, length, 0, length);
#endif
    if (WTF::IsPolymorphic<T>::value) {
        for (unsigned i = 0; i < length; ++i) {
            if (blink::vTableInitialized(&buffer[i]))
                buffer[i].~T();
        }
    } else {
        for (unsigned i = 0; i < length; ++i) {
            buffer[i].~T();
        }
    }
}

template<typename Table> class HeapHashTableBacking {
public:
    static void finalize(void* pointer);
    void finalizeGarbageCollectedObject() { finalize(this); }
};

template<typename Table>
void HeapHashTableBacking<Table>::finalize(void* pointer)
{
    using Value = typename Table::ValueType;
    ASSERT(!WTF::IsTriviallyDestructible<Value>::value);
    HeapObjectHeader* header = HeapObjectHeader::fromPayload(pointer);
    ASSERT(header->checkHeader());
    // Use the payload size as recorded by the heap to determine how many
    // elements to finalize.
    size_t length = header->payloadSize() / sizeof(Value);
    Value* table = reinterpret_cast<Value*>(pointer);
    for (unsigned i = 0; i < length; ++i) {
        if (!Table::isEmptyOrDeletedBucket(table[i]))
            table[i].~Value();
    }
}

// FIXME: These should just be template aliases:
//
// template<typename T, size_t inlineCapacity = 0>
// using HeapVector = Vector<T, inlineCapacity, HeapAllocator>;
//
// as soon as all the compilers we care about support that.
// MSVC supports it only in MSVC 2013.
template<
    typename KeyArg,
    typename MappedArg,
    typename HashArg = typename DefaultHash<KeyArg>::Hash,
    typename KeyTraitsArg = HashTraits<KeyArg>,
    typename MappedTraitsArg = HashTraits<MappedArg>>
class HeapHashMap : public HashMap<KeyArg, MappedArg, HashArg, KeyTraitsArg, MappedTraitsArg, HeapAllocator> { };

template<
    typename ValueArg,
    typename HashArg = typename DefaultHash<ValueArg>::Hash,
    typename TraitsArg = HashTraits<ValueArg>>
class HeapHashSet : public HashSet<ValueArg, HashArg, TraitsArg, HeapAllocator> { };

template<
    typename ValueArg,
    typename HashArg = typename DefaultHash<ValueArg>::Hash,
    typename TraitsArg = HashTraits<ValueArg>>
class HeapLinkedHashSet : public LinkedHashSet<ValueArg, HashArg, TraitsArg, HeapAllocator> { };

template<
    typename ValueArg,
    size_t inlineCapacity = 0, // The inlineCapacity is just a dummy to match ListHashSet (off-heap).
    typename HashArg = typename DefaultHash<ValueArg>::Hash>
class HeapListHashSet : public ListHashSet<ValueArg, inlineCapacity, HashArg, HeapListHashSetAllocator<ValueArg, inlineCapacity>> { };

template<
    typename Value,
    typename HashFunctions = typename DefaultHash<Value>::Hash,
    typename Traits = HashTraits<Value>>
class HeapHashCountedSet : public HashCountedSet<Value, HashFunctions, Traits, HeapAllocator> { };

template<typename T, size_t inlineCapacity = 0>
class HeapVector : public Vector<T, inlineCapacity, HeapAllocator> {
public:
    HeapVector() { }

    explicit HeapVector(size_t size) : Vector<T, inlineCapacity, HeapAllocator>(size)
    {
    }

    HeapVector(size_t size, const T& val) : Vector<T, inlineCapacity, HeapAllocator>(size, val)
    {
    }

    template<size_t otherCapacity>
    HeapVector(const HeapVector<T, otherCapacity>& other)
        : Vector<T, inlineCapacity, HeapAllocator>(other)
    {
    }

    template<typename U>
    void append(const U* data, size_t dataSize)
    {
        Vector<T, inlineCapacity, HeapAllocator>::append(data, dataSize);
    }

    template<typename U>
    void append(const U& other)
    {
        Vector<T, inlineCapacity, HeapAllocator>::append(other);
    }

    template<typename U, size_t otherCapacity>
    void appendVector(const HeapVector<U, otherCapacity>& other)
    {
        const Vector<U, otherCapacity, HeapAllocator>& otherVector = other;
        Vector<T, inlineCapacity, HeapAllocator>::appendVector(otherVector);
    }
};

template<typename T, size_t inlineCapacity = 0>
class HeapDeque : public Deque<T, inlineCapacity, HeapAllocator> {
public:
    HeapDeque() { }

    explicit HeapDeque(size_t size) : Deque<T, inlineCapacity, HeapAllocator>(size)
    {
    }

    HeapDeque(size_t size, const T& val) : Deque<T, inlineCapacity, HeapAllocator>(size, val)
    {
    }

    // FIXME: Doesn't work if there is an inline buffer, due to crbug.com/360572
    HeapDeque<T, 0>& operator=(const HeapDeque& other)
    {
        HeapDeque<T> copy(other);
        swap(copy);
        return *this;
    }

    // FIXME: Doesn't work if there is an inline buffer, due to crbug.com/360572
    void swap(HeapDeque& other)
    {
        Deque<T, inlineCapacity, HeapAllocator>::swap(other);
    }

    template<size_t otherCapacity>
    HeapDeque(const HeapDeque<T, otherCapacity>& other)
        : Deque<T, inlineCapacity, HeapAllocator>(other)
    {
    }

    template<typename U>
    void append(const U& other)
    {
        Deque<T, inlineCapacity, HeapAllocator>::append(other);
    }
};

template<typename T, size_t i>
inline void swap(HeapVector<T, i>& a, HeapVector<T, i>& b) { a.swap(b); }
template<typename T, size_t i>
inline void swap(HeapDeque<T, i>& a, HeapDeque<T, i>& b) { a.swap(b); }
template<typename T, typename U, typename V>
inline void swap(HeapHashSet<T, U, V>& a, HeapHashSet<T, U, V>& b) { a.swap(b); }
template<typename T, typename U, typename V, typename W, typename X>
inline void swap(HeapHashMap<T, U, V, W, X>& a, HeapHashMap<T, U, V, W, X>& b) { a.swap(b); }
template<typename T, size_t i, typename U>
inline void swap(HeapListHashSet<T, i, U>& a, HeapListHashSet<T, i, U>& b) { a.swap(b); }
template<typename T, typename U, typename V>
inline void swap(HeapLinkedHashSet<T, U, V>& a, HeapLinkedHashSet<T, U, V>& b) { a.swap(b); }
template<typename T, typename U, typename V>
inline void swap(HeapHashCountedSet<T, U, V>& a, HeapHashCountedSet<T, U, V>& b) { a.swap(b); }

} // namespace blink

#endif
