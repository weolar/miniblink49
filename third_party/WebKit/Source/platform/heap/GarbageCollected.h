// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GarbageCollected_h
#define GarbageCollected_h

#include "platform/heap/ThreadState.h"
#include "wtf/Assertions.h"
#include "wtf/ListHashSet.h"
#include "wtf/TypeTraits.h"

namespace blink {

template<typename T> class GarbageCollected;
template<typename T, typename U, typename V, typename W, typename X> class HeapHashMap;
template<typename T, typename U, typename V> class HeapHashSet;
template<typename T, typename U, typename V> class HeapLinkedHashSet;
template<typename T, size_t inlineCapacity, typename U> class HeapListHashSet;
template<typename T, size_t inlineCapacity> class HeapVector;
template<typename T, size_t inlineCapacity> class HeapDeque;
template<typename T, typename U, typename V> class HeapHashCountedSet;
template<typename T> class HeapTerminatedArray;
template<typename T, typename Traits> class HeapVectorBacking;
template<typename Table> class HeapHashTableBacking;
template<typename ValueArg, size_t inlineCapacity> class HeapListHashSetAllocator;
class InlinedGlobalMarkingVisitor;
template<ThreadAffinity affinity> class ThreadLocalPersistents;
template<typename T> class Persistent;

// Template to determine if a class is a GarbageCollectedMixin by checking if it
// has IsGarbageCollectedMixinMarker
template<typename T>
struct IsGarbageCollectedMixin {
private:
    typedef char YesType;
    struct NoType {
        char padding[8];
    };

    template <typename U> static YesType checkMarker(typename U::IsGarbageCollectedMixinMarker*);
    template <typename U> static NoType checkMarker(...);

public:
    static const bool value = sizeof(checkMarker<T>(nullptr)) == sizeof(YesType);
};

template <typename T>
struct IsGarbageCollectedType {
    using TrueType = char;
    struct FalseType {
        char dummy[2];
    };

    using NonConstType = typename WTF::RemoveConst<T>::Type;
    using GarbageCollectedSubclass = WTF::IsSubclassOfTemplate<NonConstType, GarbageCollected>;
    using GarbageCollectedMixinSubclass = IsGarbageCollectedMixin<NonConstType>;
    using HeapHashSetSubclass = WTF::IsSubclassOfTemplate<NonConstType, HeapHashSet>;
    using HeapLinkedHashSetSubclass = WTF::IsSubclassOfTemplate<NonConstType, HeapLinkedHashSet>;
    using HeapListHashSetSubclass = WTF::IsSubclassOfTemplateTypenameSizeTypename<NonConstType, HeapListHashSet>;
    using HeapHashMapSubclass = WTF::IsSubclassOfTemplate<NonConstType, HeapHashMap>;
    using HeapVectorSubclass = WTF::IsSubclassOfTemplateTypenameSize<NonConstType, HeapVector>;
    using HeapDequeSubclass = WTF::IsSubclassOfTemplateTypenameSize<NonConstType, HeapDeque>;
    using HeapHashCountedSetSubclass = WTF::IsSubclassOfTemplate<NonConstType, HeapHashCountedSet>;
    using HeapTerminatedArraySubclass = WTF::IsSubclassOfTemplate<NonConstType, HeapTerminatedArray>;
    using HeapVectorBackingSubclass = WTF::IsSubclassOfTemplate<NonConstType, HeapVectorBacking>;
    using HeapHashTableBackingSubclass = WTF::IsSubclassOfTemplate<NonConstType, HeapHashTableBacking>;

    template<typename U, size_t inlineCapacity> static TrueType listHashSetNodeIsHeapAllocated(WTF::ListHashSetNode<U, HeapListHashSetAllocator<U, inlineCapacity>>*);
    static FalseType listHashSetNodeIsHeapAllocated(...);
    static const bool isHeapAllocatedListHashSetNode = sizeof(TrueType) == sizeof(listHashSetNodeIsHeapAllocated(reinterpret_cast<NonConstType*>(0)));

    static_assert(sizeof(T), "T must be fully defined");

    static const bool value =
        GarbageCollectedSubclass::value
        || GarbageCollectedMixinSubclass::value
        || HeapHashSetSubclass::value
        || HeapLinkedHashSetSubclass::value
        || HeapListHashSetSubclass::value
        || HeapHashMapSubclass::value
        || HeapVectorSubclass::value
        || HeapDequeSubclass::value
        || HeapHashCountedSetSubclass::value
        || HeapTerminatedArraySubclass::value
        || HeapVectorBackingSubclass::value
        || HeapHashTableBackingSubclass::value
        || isHeapAllocatedListHashSetNode;
};

// The GarbageCollectedMixin interface and helper macro
// USING_GARBAGE_COLLECTED_MIXIN can be used to automatically define
// TraceTrait/ObjectAliveTrait on non-leftmost deriving classes
// which need to be garbage collected.
//
// Consider the following case:
// class B {};
// class A : public GarbageCollected, public B {};
//
// We can't correctly handle "Member<B> p = &a" as we can't compute addr of
// object header statically. This can be solved by using GarbageCollectedMixin:
// class B : public GarbageCollectedMixin {};
// class A : public GarbageCollected, public B {
//   USING_GARBAGE_COLLECTED_MIXIN(A)
// };
//
// With the helper, as long as we are using Member<B>, TypeTrait<B> will
// dispatch adjustAndMark dynamically to find collect addr of the object header.
// Note that this is only enabled for Member<B>. For Member<A> which we can
// compute the object header addr statically, this dynamic dispatch is not used.
class PLATFORM_EXPORT GarbageCollectedMixin {
public:
    typedef int IsGarbageCollectedMixinMarker;
    virtual void adjustAndMark(Visitor*) const = 0;
    virtual void trace(Visitor*) { }
    virtual void adjustAndMark(InlinedGlobalMarkingVisitor) const = 0;
    virtual void trace(InlinedGlobalMarkingVisitor);
    virtual bool isHeapObjectAlive() const = 0;
};

#define DEFINE_GARBAGE_COLLECTED_MIXIN_METHODS(VISITOR, TYPE)           \
    public:                                                             \
    void adjustAndMark(VISITOR visitor) const override                  \
    {                                                                   \
        typedef WTF::IsSubclassOfTemplate<typename WTF::RemoveConst<TYPE>::Type, blink::GarbageCollected> IsSubclassOfGarbageCollected; \
        static_assert(IsSubclassOfGarbageCollected::value, "only garbage collected objects can have garbage collected mixins"); \
        if (TraceEagerlyTrait<TYPE>::value) {                           \
            if (visitor->ensureMarked(static_cast<const TYPE*>(this)))  \
                TraceTrait<TYPE>::trace(visitor, const_cast<TYPE*>(this)); \
            return;                                                     \
        }                                                               \
        visitor->mark(static_cast<const TYPE*>(this), &blink::TraceTrait<TYPE>::trace); \
    }                                                                   \
    private:

// A C++ object's vptr will be initialized to its leftmost base's vtable after
// the constructors of all its subclasses have run, so if a subclass constructor
// tries to access any of the vtbl entries of its leftmost base prematurely,
// it'll find an as-yet incorrect vptr and fail. Which is exactly what a
// garbage collector will try to do if it tries to access the leftmost base
// while one of the subclass constructors of a GC mixin object triggers a GC.
// It is consequently not safe to allow any GCs while these objects are under
// (sub constructor) construction.
//
// To prevent GCs in that restricted window of a mixin object's construction:
//
//  - The initial allocation of the mixin object will enter a no GC scope.
//    This is done by overriding 'operator new' for mixin instances.
//  - When the constructor for the mixin is invoked, after all the
//    derived constructors have run, it will invoke the constructor
//    for a field whose only purpose is to leave the GC scope.
//    GarbageCollectedMixinConstructorMarker's constructor takes care of
//    this and the field is declared by way of USING_GARBAGE_COLLECTED_MIXIN().

#define DEFINE_GARBAGE_COLLECTED_MIXIN_CONSTRUCTOR_MARKER(TYPE)         \
    public:                                                             \
    GC_PLUGIN_IGNORE("crbug.com/456823") NO_SANITIZE_UNRELATED_CAST     \
    void* operator new(size_t size)                                     \
{                                                                       \
        void* object = TYPE::allocateObject(size, IsEagerlyFinalizedType<TYPE>::value); \
        ThreadState* state = ThreadStateFor<ThreadingTrait<TYPE>::Affinity>::state();   \
        state->enterGCForbiddenScopeIfNeeded(&(reinterpret_cast<TYPE*>(object)->m_mixinConstructorMarker)); \
        return object;                                                  \
    }                                                                   \
    GarbageCollectedMixinConstructorMarker m_mixinConstructorMarker;    \
    private:

// Mixins that wrap/nest others requires extra handling:
//
//  class A : public GarbageCollected<A>, public GarbageCollectedMixin {
//  USING_GARBAGE_COLLECTED_MIXIN(A);
//  ...
//  }'
//  public B final : public A, public SomeOtherMixinInterface {
//  USING_GARBAGE_COLLECTED_MIXIN(B);
//  ...
//  };
//
// The "operator new" for B will enter the forbidden GC scope, but
// upon construction, two GarbageCollectedMixinConstructorMarker constructors
// will run -- one for A (first) and another for B (secondly). Only
// the second one should leave the forbidden GC scope. This is realized by
// recording the address of B's GarbageCollectedMixinConstructorMarker
// when the "operator new" for B runs, and leaving the forbidden GC scope
// when the constructor of the recorded GarbageCollectedMixinConstructorMarker
// runs.
#define USING_GARBAGE_COLLECTED_MIXIN(TYPE)                             \
    DEFINE_GARBAGE_COLLECTED_MIXIN_METHODS(blink::Visitor*, TYPE)       \
    DEFINE_GARBAGE_COLLECTED_MIXIN_METHODS(blink::InlinedGlobalMarkingVisitor, TYPE) \
    DEFINE_GARBAGE_COLLECTED_MIXIN_CONSTRUCTOR_MARKER(TYPE)             \
public:                                                                 \
    bool isHeapObjectAlive() const override                             \
    {                                                                   \
        return Heap::isHeapObjectAlive(this);                           \
    }                                                                   \
private:

#if ENABLE(OILPAN)
#define WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(TYPE) USING_GARBAGE_COLLECTED_MIXIN(TYPE)
#else
#define WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(TYPE)
#endif

// An empty class with a constructor that's arranged invoked when all derived constructors
// of a mixin instance have completed and it is safe to allow GCs again. See
// AllocateObjectTrait<> comment for more.
//
// USING_GARBAGE_COLLECTED_MIXIN() declares a GarbageCollectedMixinConstructorMarker<> private
// field. By following Blink convention of using the macro at the top of a class declaration,
// its constructor will run first.
class GarbageCollectedMixinConstructorMarker {
public:
    GarbageCollectedMixinConstructorMarker()
    {
        // FIXME: if prompt conservative GCs are needed, forced GCs that
        // were denied while within this scope, could now be performed.
        // For now, assume the next out-of-line allocation request will
        // happen soon enough and take care of it. Mixin objects aren't
        // overly common.
        ThreadState* state = ThreadState::current();
        state->leaveGCForbiddenScopeIfNeeded(this);
    }
};

// Base class for objects allocated in the Blink garbage-collected heap.
//
// Defines a 'new' operator that allocates the memory in the heap.  'delete'
// should not be called on objects that inherit from GarbageCollected.
//
// Instances of GarbageCollected will *NOT* get finalized.  Their destructor
// will not be called.  Therefore, only classes that have trivial destructors
// with no semantic meaning (including all their subclasses) should inherit from
// GarbageCollected.  If there are non-trival destructors in a given class or
// any of its subclasses, GarbageCollectedFinalized should be used which
// guarantees that the destructor is called on an instance when the garbage
// collector determines that it is no longer reachable.
template<typename T> class GarbageCollected;

// Base class for objects allocated in the Blink garbage-collected heap.
//
// Defines a 'new' operator that allocates the memory in the heap.  'delete'
// should not be called on objects that inherit from GarbageCollected.
//
// Instances of GarbageCollectedFinalized will have their destructor called when
// the garbage collector determines that the object is no longer reachable.
template<typename T>
class GarbageCollectedFinalized : public GarbageCollected<T> {
    WTF_MAKE_NONCOPYABLE(GarbageCollectedFinalized);

protected:
    // finalizeGarbageCollectedObject is called when the object is freed from
    // the heap.  By default finalization means calling the destructor on the
    // object.  finalizeGarbageCollectedObject can be overridden to support
    // calling the destructor of a subclass.  This is useful for objects without
    // vtables that require explicit dispatching.  The name is intentionally a
    // bit long to make name conflicts less likely.
    void finalizeGarbageCollectedObject()
    {
        static_cast<T*>(this)->~T();
    }

    GarbageCollectedFinalized() { }
    ~GarbageCollectedFinalized() { }

    template<typename U> friend struct HasFinalizer;
    template<typename U, bool> friend struct FinalizerTraitImpl;
};

// Base class for objects that are in the Blink garbage-collected heap
// and are still reference counted.
//
// This class should be used sparingly and only to gradually move
// objects from being reference counted to being managed by the blink
// garbage collector.
//
// While the current reference counting keeps one of these objects
// alive it will have a Persistent handle to itself allocated so we
// will not reclaim the memory.  When the reference count reaches 0 the
// persistent handle will be deleted.  When the garbage collector
// determines that there are no other references to the object it will
// be reclaimed and the destructor of the reclaimed object will be
// called at that time.
template<typename T>
class RefCountedGarbageCollected : public GarbageCollectedFinalized<T> {
    WTF_MAKE_NONCOPYABLE(RefCountedGarbageCollected);

public:
    RefCountedGarbageCollected()
        : m_refCount(0)
    {
    }

    // Implement method to increase reference count for use with RefPtrs.
    //
    // In contrast to the normal WTF::RefCounted, the reference count can reach
    // 0 and increase again.  This happens in the following scenario:
    //
    // (1) The reference count becomes 0, but members, persistents, or
    //     on-stack pointers keep references to the object.
    //
    // (2) The pointer is assigned to a RefPtr again and the reference
    //     count becomes 1.
    //
    // In this case, we have to resurrect m_keepAlive.
    void ref()
    {
        if (UNLIKELY(!m_refCount)) {
            ASSERT(ThreadState::current()->findPageFromAddress(reinterpret_cast<Address>(this)));
            makeKeepAlive();
        }
        ++m_refCount;
    }

    // Implement method to decrease reference count for use with RefPtrs.
    //
    // In contrast to the normal WTF::RefCounted implementation, the
    // object itself is not deleted when the reference count reaches
    // 0.  Instead, the keep-alive persistent handle is deallocated so
    // that the object can be reclaimed when the garbage collector
    // determines that there are no other references to the object.
    void deref()
    {
        ASSERT(m_refCount > 0);
        if (!--m_refCount) {
            delete m_keepAlive;
            m_keepAlive = 0;
        }
    }

    bool hasOneRef()
    {
        return m_refCount == 1;
    }

protected:
    ~RefCountedGarbageCollected() { }

private:
    void makeKeepAlive()
    {
        ASSERT(!m_keepAlive);
        m_keepAlive = new Persistent<T>(static_cast<T*>(this));
    }

    int m_refCount;
    Persistent<T>* m_keepAlive;
};

// Classes that contain heap references but aren't themselves heap allocated,
// have some extra macros available which allows their use to be restricted to
// cases where the garbage collector is able to discover their heap references.
//
// STACK_ALLOCATED(): Use if the object is only stack allocated.  Heap objects
// should be in Members but you do not need the trace method as they are on the
// stack.  (Down the line these might turn in to raw pointers, but for now
// Members indicates that we have thought about them and explicitly taken care
// of them.)
//
// DISALLOW_ALLOCATION(): Cannot be allocated with new operators but can be a
// part object.  If it has Members you need a trace method and the containing
// object needs to call that trace method.
//
// ALLOW_ONLY_INLINE_ALLOCATION(): Allows only placement new operator.  This
// disallows general allocation of this object but allows to put the object as a
// value object in collections.  If these have Members you need to have a trace
// method. That trace method will be called automatically by the Heap
// collections.
//
#define DISALLOW_ALLOCATION()                                   \
    private:                                                    \
        void* operator new(size_t) = delete;                    \
        void* operator new(size_t, NotNullTag, void*) = delete; \
        void* operator new(size_t, void*) = delete;

#define ALLOW_ONLY_INLINE_ALLOCATION()                                              \
    public:                                                                         \
        void* operator new(size_t, NotNullTag, void* location) { return location; } \
        void* operator new(size_t, void* location) { return location; }             \
    private:                                                                        \
        void* operator new(size_t) = delete;

#define STATIC_ONLY(Type) \
    private:              \
        Type() = delete;

// These macros insert annotations that the Blink GC plugin for clang uses for
// verification.  STACK_ALLOCATED is used to declare that objects of this type
// are always stack allocated.  GC_PLUGIN_IGNORE is used to make the plugin
// ignore a particular class or field when checking for proper usage.  When
// using GC_PLUGIN_IGNORE a bug-number should be provided as an argument where
// the bug describes what needs to happen to remove the GC_PLUGIN_IGNORE again.
#if COMPILER(CLANG)
#define STACK_ALLOCATED()                                       \
    private:                                                    \
        __attribute__((annotate("blink_stack_allocated")))      \
        void* operator new(size_t) = delete;                    \
        void* operator new(size_t, NotNullTag, void*) = delete; \
        void* operator new(size_t, void*) = delete;

#define GC_PLUGIN_IGNORE(bug)                           \
    __attribute__((annotate("blink_gc_plugin_ignore")))
#else
#define STACK_ALLOCATED() DISALLOW_ALLOCATION()
#define GC_PLUGIN_IGNORE(bug)
#endif

template<typename T, bool = WTF::IsSubclassOfTemplate<typename WTF::RemoveConst<T>::Type, GarbageCollected>::value> class NeedsAdjustAndMark;

template<typename T>
class NeedsAdjustAndMark<T, true> {
    static_assert(sizeof(T), "T must be fully defined");
public:
    static const bool value = false;
};
template <typename T> const bool NeedsAdjustAndMark<T, true>::value;

template<typename T>
class NeedsAdjustAndMark<T, false> {
    static_assert(sizeof(T), "T must be fully defined");
public:
    static const bool value = IsGarbageCollectedMixin<typename WTF::RemoveConst<T>::Type>::value;
};
template <typename T> const bool NeedsAdjustAndMark<T, false>::value;

// TODO(sof): migrate to wtf/TypeTraits.h
template<typename T>
class IsFullyDefined {
    using TrueType = char;
    struct FalseType {
        char dummy[2];
    };

    template<typename U, size_t sz = sizeof(U)> static TrueType isSizeofKnown(U*);
    static FalseType isSizeofKnown(...);
    static T& t;
public:
    static const bool value = sizeof(TrueType) == sizeof(isSizeofKnown(&t));
};

} // namespace blink

#endif
