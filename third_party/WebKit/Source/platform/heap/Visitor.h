/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef Visitor_h
#define Visitor_h

#include "platform/PlatformExport.h"
#include "platform/heap/GarbageCollected.h"
#include "platform/heap/StackFrameDepth.h"
#include "platform/heap/ThreadState.h"
#include "wtf/Assertions.h"
#include "wtf/Atomics.h"
#include "wtf/Deque.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/HashTraits.h"
#include "wtf/TypeTraits.h"
#if ENABLE(GC_PROFILING)
#include "wtf/InstanceCounter.h"
#include "wtf/text/WTFString.h"
#endif

namespace blink {

template<typename T> class GarbageCollected;
class HeapObjectHeader;
class InlinedGlobalMarkingVisitor;
template<typename T> class TraceTrait;
template<typename T> class TraceEagerlyTrait;
class Visitor;

// The TraceMethodDelegate is used to convert a trace method for type T to a TraceCallback.
// This allows us to pass a type's trace method as a parameter to the PersistentNode
// constructor. The PersistentNode constructor needs the specific trace method due an issue
// with the Windows compiler which instantiates even unused variables. This causes problems
// in header files where we have only forward declarations of classes.
template<typename T, void (T::*method)(Visitor*)>
struct TraceMethodDelegate {
    static void trampoline(Visitor* visitor, void* self)
    {
        (reinterpret_cast<T*>(self)->*method)(visitor);
    }
};

// HasInlinedTraceMethod<T>::value is true for T supporting
// T::trace(InlinedGlobalMarkingVisitor).
// The template works by checking if T::HasInlinedTraceMethodMarker type is
// available using SFINAE. The HasInlinedTraceMethodMarker type is defined
// by DECLARE_TRACE and DEFINE_INLINE_TRACE helper macros, which are used to
// define trace methods supporting both inlined/uninlined tracing.
template <typename T>
struct HasInlinedTraceMethod {
private:
    typedef char YesType;
    struct NoType {
        char padding[8];
    };

    template <typename U> static YesType checkMarker(typename U::HasInlinedTraceMethodMarker*);
    template <typename U> static NoType checkMarker(...);
public:
    static const bool value = sizeof(checkMarker<T>(nullptr)) == sizeof(YesType);
};

#define DECLARE_TRACE_IMPL(maybevirtual)                                     \
public:                                                                      \
    typedef int HasInlinedTraceMethodMarker;                                 \
    maybevirtual void trace(Visitor*);                                       \
    maybevirtual void trace(InlinedGlobalMarkingVisitor);                    \
                                                                             \
private:                                                                     \
    template <typename VisitorDispatcher> void traceImpl(VisitorDispatcher); \
                                                                             \
public:
#define DEFINE_TRACE(T)                                                        \
    void T::trace(Visitor* visitor) { traceImpl(visitor); }                    \
    void T::trace(InlinedGlobalMarkingVisitor visitor) { traceImpl(visitor); } \
    template <typename VisitorDispatcher>                                      \
    ALWAYS_INLINE void T::traceImpl(VisitorDispatcher visitor)

#define DEFINE_INLINE_TRACE_IMPL(maybevirtual)                                           \
    typedef int HasInlinedTraceMethodMarker;                                             \
    maybevirtual void trace(Visitor* visitor) { traceImpl(visitor); }                    \
    maybevirtual void trace(InlinedGlobalMarkingVisitor visitor) { traceImpl(visitor); } \
    template <typename VisitorDispatcher>                                                \
    inline void traceImpl(VisitorDispatcher visitor)

#define DECLARE_TRACE_AFTER_DISPATCH()                                                    \
public:                                                                                   \
    typedef int HasInlinedTraceAfterDispatchMethodMarker;                                 \
    void traceAfterDispatch(Visitor*);                                                    \
    void traceAfterDispatch(InlinedGlobalMarkingVisitor);                                 \
private:                                                                                  \
    template <typename VisitorDispatcher> void traceAfterDispatchImpl(VisitorDispatcher); \
public:

#define DEFINE_TRACE_AFTER_DISPATCH(T)                                                                   \
    void T::traceAfterDispatch(Visitor* visitor) { traceAfterDispatchImpl(visitor); }                    \
    void T::traceAfterDispatch(InlinedGlobalMarkingVisitor visitor) { traceAfterDispatchImpl(visitor); } \
    template <typename VisitorDispatcher>                                                                \
    ALWAYS_INLINE void T::traceAfterDispatchImpl(VisitorDispatcher visitor)

#define DEFINE_INLINE_TRACE_AFTER_DISPATCH()                                                          \
    typedef int HasInlinedTraceAfterDispatchMethodMarker;                                             \
    void traceAfterDispatch(Visitor* visitor) { traceAfterDispatchImpl(visitor); }                    \
    void traceAfterDispatch(InlinedGlobalMarkingVisitor visitor) { traceAfterDispatchImpl(visitor); } \
    template <typename VisitorDispatcher>                                                             \
    inline void traceAfterDispatchImpl(VisitorDispatcher visitor)

#define EMPTY_MACRO_ARGUMENT

#define DECLARE_TRACE() DECLARE_TRACE_IMPL(EMPTY_MACRO_ARGUMENT)
#define DECLARE_VIRTUAL_TRACE() DECLARE_TRACE_IMPL(virtual)
#define DEFINE_INLINE_TRACE() DEFINE_INLINE_TRACE_IMPL(EMPTY_MACRO_ARGUMENT)
#define DEFINE_INLINE_VIRTUAL_TRACE() DEFINE_INLINE_TRACE_IMPL(virtual)

// VisitorHelper contains common implementation of Visitor helper methods.
//
// VisitorHelper avoids virtual methods by using CRTP.
// c.f. http://en.wikipedia.org/wiki/Curiously_Recurring_Template_Pattern
template<typename Derived>
class VisitorHelper {
public:
    // One-argument templated mark method. This uses the static type of
    // the argument to get the TraceTrait. By default, the mark method
    // of the TraceTrait just calls the virtual two-argument mark method on this
    // visitor, where the second argument is the static trace method of the trait.
    template<typename T>
    void mark(T* t)
    {
        static_assert(sizeof(T), "T must be fully defined");
        static_assert(IsGarbageCollectedType<T>::value, "T needs to be a garbage collected object");
        if (!t)
            return;
        TraceTrait<T>::mark(Derived::fromHelper(this), t);
    }

    // Member version of the one-argument templated trace method.
    template<typename T>
    void trace(const Member<T>& t)
    {
        mark(t.get());
    }

    // Fallback method used only when we need to trace raw pointers of T.
    // This is the case when a member is a union where we do not support members.
    template<typename T>
    void trace(const T* t)
    {
        mark(const_cast<T*>(t));
    }
    template<typename T>
    void trace(T* t)
    {
        mark(t);
    }

    // WeakMember version of the templated trace method. It doesn't keep
    // the traced thing alive, but will write null to the WeakMember later
    // if the pointed-to object is dead. It's lying for this to be const,
    // but the overloading resolver prioritizes constness too high when
    // picking the correct overload, so all these trace methods have to have
    // the same constness on their argument to allow the type to decide.
    template<typename T>
    void trace(const WeakMember<T>& t)
    {
        static_assert(sizeof(T), "T must be fully defined");
        static_assert(IsGarbageCollectedType<T>::value, "T needs to be a garbage collected object");
        registerWeakCell(const_cast<WeakMember<T>&>(t).cell());
    }

    template<typename T>
    void traceInCollection(T& t, WTF::ShouldWeakPointersBeMarkedStrongly strongify)
    {
        HashTraits<T>::traceInCollection(Derived::fromHelper(this), t, strongify);
    }

    // Fallback trace method for part objects to allow individual trace methods
    // to trace through a part object with visitor->trace(m_partObject). This
    // takes a const argument, because otherwise it will match too eagerly: a
    // non-const argument would match a non-const Vector<T>& argument better
    // than the specialization that takes const Vector<T>&. For a similar reason,
    // the other specializations take a const argument even though they are
    // usually used with non-const arguments, otherwise this function would match
    // too well.
    template<typename T>
    void trace(const T& t)
    {
        static_assert(sizeof(T), "T must be fully defined");
        if (WTF::IsPolymorphic<T>::value) {
            intptr_t vtable = *reinterpret_cast<const intptr_t*>(&t);
            if (!vtable)
                return;
        }
        TraceTrait<T>::trace(Derived::fromHelper(this), &const_cast<T&>(t));
    }

#if !ENABLE(OILPAN)
    // These trace methods are needed to allow compiling and calling trace on
    // transition types. We need to support calls in the non-oilpan build
    // because a fully transitioned type (which will have its trace method
    // called) might trace a field that is in transition. Once transition types
    // are removed these can be removed.
    template<typename T> void trace(const OwnPtr<T>&) { }
    template<typename T> void trace(const RefPtr<T>&) { }
    template<typename T> void trace(const RawPtr<T>&) { }
    template<typename T> void trace(const WeakPtr<T>&) { }

    // On non-oilpan builds, it is convenient to allow calling trace on
    // WillBeHeap{Vector,Deque}<FooPtrWillBeMember<T>>.
    // Forbid tracing on-heap objects in off-heap collections.
    // This is forbidden because convservative marking cannot identify
    // those off-heap collection backing stores.
    template<typename T, size_t inlineCapacity> void trace(const Vector<OwnPtr<T>, inlineCapacity>& vector)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Vector");
    }
    template<typename T, size_t inlineCapacity> void trace(const Vector<RefPtr<T>, inlineCapacity>& vector)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Vector");
    }
    template<typename T, size_t inlineCapacity> void trace(const Vector<RawPtr<T>, inlineCapacity>& vector)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Vector");
    }
    template<typename T, size_t inlineCapacity> void trace(const Vector<WeakPtr<T>, inlineCapacity>& vector)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Vector");
    }
    template<typename T, size_t inlineCapacity> void trace(const Vector<T, inlineCapacity>& vector)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Vector");
    }
    template<typename T, size_t N> void trace(const Deque<OwnPtr<T>, N>& deque)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Deque");
    }
    template<typename T, size_t N> void trace(const Deque<RefPtr<T>, N>& deque)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Deque");
    }
    template<typename T, size_t N> void trace(const Deque<RawPtr<T>, N>& deque)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Deque");
    }
    template<typename T, size_t N> void trace(const Deque<WeakPtr<T>, N>& deque)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Deque");
    }
    template<typename T, size_t N> void trace(const Deque<T, N>& deque)
    {
        static_assert(!IsGarbageCollectedType<T>::value, "cannot trace garbage collected object inside Deque");
    }
#endif

    void markNoTracing(const void* pointer) { Derived::fromHelper(this)->mark(pointer, reinterpret_cast<TraceCallback>(0)); }
    void markHeaderNoTracing(HeapObjectHeader* header) { Derived::fromHelper(this)->markHeader(header, reinterpret_cast<TraceCallback>(0)); }

    // For simple cases where you just want to zero out a cell when the thing
    // it is pointing at is garbage, you can use this. This will register a
    // callback for each cell that needs to be zeroed, so if you have a lot of
    // weak cells in your object you should still consider using
    // registerWeakMembers above.
    //
    // In contrast to registerWeakMembers, the weak cell callbacks are
    // run on the thread performing garbage collection. Therefore, all
    // threads are stopped during weak cell callbacks.
    template<typename T>
    void registerWeakCell(T** cell)
    {
        Derived::fromHelper(this)->registerWeakCellWithCallback(reinterpret_cast<void**>(cell), &handleWeakCell<T>);
    }

    template<typename T, void (T::*method)(Visitor*)>
    void registerWeakMembers(const T* obj)
    {
        registerWeakMembers(obj, &TraceMethodDelegate<T, method>::trampoline);
    }
    void registerWeakMembers(const void* object, WeakCallback callback)
    {
        Derived::fromHelper(this)->registerWeakMembers(object, object, callback);
    }

private:
    template<typename T>
    static void handleWeakCell(Visitor* self, void* object);
};

// Visitor is used to traverse the Blink object graph. Used for the
// marking phase of the mark-sweep garbage collector.
//
// Pointers are marked and pushed on the marking stack by calling the
// |mark| method with the pointer as an argument.
//
// Pointers within objects are traced by calling the |trace| methods
// with the object as an argument. Tracing objects will mark all of the
// contained pointers and push them on the marking stack.
class PLATFORM_EXPORT Visitor : public VisitorHelper<Visitor> {
public:
    friend class VisitorHelper<Visitor>;
    friend class InlinedGlobalMarkingVisitor;

    enum MarkingMode {
        // This is a default visitor. This is used for GCType=GCWithSweep
        // and GCType=GCWithoutSweep.
        GlobalMarking,
        // This visitor does not trace objects outside the heap of the
        // GCing thread. This is used for GCType=ThreadTerminationGC.
        ThreadLocalMarking,
        // This visitor just marks objects and ignores weak processing.
        // This is used for GCType=TakeSnapshot.
        SnapshotMarking,
        // This visitor is used to trace objects during weak processing.
        // This visitor is allowed to trace only already marked objects.
        WeakProcessing,
    };

    virtual ~Visitor() { }

    using VisitorHelper<Visitor>::mark;

    // This method marks an object and adds it to the set of objects
    // that should have their trace method called. Since not all
    // objects have vtables we have to have the callback as an
    // explicit argument, but we can use the templated one-argument
    // mark method above to automatically provide the callback
    // function.
    virtual void mark(const void*, TraceCallback) = 0;

    // Used to mark objects during conservative scanning.
    virtual void markHeader(HeapObjectHeader*, TraceCallback) = 0;

    // Used to delay the marking of objects until the usual marking
    // including emphemeron iteration is done. This is used to delay
    // the marking of collection backing stores until we know if they
    // are reachable from locations other than the collection front
    // object. If collection backings are reachable from other
    // locations we strongify them to avoid issues with iterators and
    // weak processing.
    virtual void registerDelayedMarkNoTracing(const void*) = 0;

    // If the object calls this during the regular trace callback, then the
    // WeakCallback argument may be called later, when the strong roots
    // have all been found. The WeakCallback will normally use isAlive
    // to find out whether some pointers are pointing to dying objects. When
    // the WeakCallback is done the object must have purged all pointers
    // to objects where isAlive returned false. In the weak callback it is not
    // allowed to touch other objects (except using isAlive) or to allocate on
    // the GC heap. Note that even removing things from HeapHashSet or
    // HeapHashMap can cause an allocation if the backing store resizes, but
    // these collections know to remove WeakMember elements safely.
    //
    // The weak pointer callbacks are run on the thread that owns the
    // object and other threads are not stopped during the
    // callbacks. Since isAlive is used in the callback to determine
    // if objects pointed to are alive it is crucial that the object
    // pointed to belong to the same thread as the object receiving
    // the weak callback. Since other threads have been resumed the
    // mark bits are not valid for objects from other threads.
    virtual void registerWeakMembers(const void*, const void*, WeakCallback) = 0;
    using VisitorHelper<Visitor>::registerWeakMembers;

    virtual void registerWeakTable(const void*, EphemeronCallback, EphemeronCallback) = 0;
#if ENABLE(ASSERT)
    virtual bool weakTableRegistered(const void*) = 0;
#endif

    virtual bool ensureMarked(const void*) = 0;

    inline MarkingMode markingMode() const { return m_markingMode; }

protected:
    explicit Visitor(MarkingMode markingMode)
        : m_markingMode(markingMode)
    { }

    virtual void registerWeakCellWithCallback(void**, WeakCallback) = 0;

private:
    static Visitor* fromHelper(VisitorHelper<Visitor>* helper) { return static_cast<Visitor*>(helper); }

    const MarkingMode m_markingMode;
    bool m_isGlobalMarkingVisitor;
};

#if ENABLE(GC_PROFILING)
template<typename T>
struct TypenameStringTrait {
    static const String& get()
    {
        DEFINE_STATIC_LOCAL(String, typenameString, (WTF::extractTypeNameFromFunctionName(WTF::extractNameFunction<T>())));
        return typenameString;
    }
};
#endif

} // namespace blink

#endif // Visitor_h
