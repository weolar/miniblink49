/*
 * Copyright (C) 2012 Google, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Supplementable_h
#define Supplementable_h

#include "platform/PlatformExport.h"
#include "platform/heap/Handle.h"
#include "wtf/Assertions.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"

#if ENABLE(ASSERT)
#include "wtf/Threading.h"
#endif

namespace blink {

// What you should know about Supplementable and Supplement
// ========================================================
// Supplementable and Supplement instances are meant to be thread local. They
// should only be accessed from within the thread that created them. The
// 2 classes are not designed for safe access from another thread. Violating
// this design assumption can result in memory corruption and unpredictable
// behavior.
//
// What you should know about the Supplement keys
// ==============================================
// The Supplement is expected to use the same const char* string instance
// as its key. The Supplementable's SupplementMap will use the address of the
// string as the key and not the characters themselves. Hence, 2 strings with
// the same characters will be treated as 2 different keys.
//
// In practice, it is recommended that Supplements implements a static method
// for returning its key to use. For example:
//
//     class MyClass : public Supplement<MySupplementable> {
//         ...
//         static const char* supplementName();
//     }
//
//     const char* MyClass::supplementName()
//     {
//         return "MyClass";
//     }
//
// An example of the using the key:
//
//     MyClass* MyClass::from(MySupplementable* host)
//     {
//         return reinterpret_cast<MyClass*>(Supplement<MySupplementable>::from(host, supplementName()));
//     }
//
// What you should know about thread checks
// ========================================
// When assertion is enabled this class performs thread-safety check so that
// provideTo and from happen on the same thread. If you want to provide
// some value for Workers this thread check may not work very well though,
// since in most case you'd provide the value while worker preparation is
// being done on the main thread, even before the worker thread is started.
// If that's the case you can explicitly call reattachThread() when the
// Supplementable object is passed to the final destination thread (i.e.
// worker thread). Please be extremely careful to use the method though,
// as randomly calling the method could easily cause racy condition.
//
// Note that reattachThread() does nothing if assertion is not enabled.
//

template<typename T, bool isGarbageCollected>
class SupplementBase;

template<typename T, bool isGarbageCollected>
class SupplementableBase;

template<typename T, bool isGarbageCollected>
struct SupplementableTraits;

template<typename T>
struct SupplementableTraits<T, true> {
    typedef RawPtr<SupplementBase<T, true>> SupplementArgumentType;
};

template<typename T>
struct SupplementableTraits<T, false> {
    typedef PassOwnPtr<SupplementBase<T, false>> SupplementArgumentType;
};

template<bool>
class SupplementTracing;

template<>
class PLATFORM_EXPORT SupplementTracing<true> : public GarbageCollectedMixin { };

template<>
class GC_PLUGIN_IGNORE("crbug.com/476419") PLATFORM_EXPORT SupplementTracing<false> {
public:
    virtual ~SupplementTracing() { }
    // FIXME: Oilpan: this trace() method is only provided to minimize
    // the use of ENABLE(OILPAN) for Supplements deriving from the
    // transition type WillBeHeapSupplement<>.
    //
    // When that transition type is removed (or its use is substantially
    // reduced), remove this dummy trace method also.
    DEFINE_INLINE_VIRTUAL_TRACE() { }
};

template<typename T, bool isGarbageCollected = false>
class SupplementBase : public SupplementTracing<isGarbageCollected> {
public:
#if ENABLE(SECURITY_ASSERT)
    virtual bool isRefCountedWrapper() const { return false; }
#endif

    static void provideTo(SupplementableBase<T, isGarbageCollected>& host, const char* key, typename SupplementableTraits<T, isGarbageCollected>::SupplementArgumentType supplement)
    {
        host.provideSupplement(key, supplement);
    }

    static SupplementBase<T, isGarbageCollected>* from(SupplementableBase<T, isGarbageCollected>& host, const char* key)
    {
        return host.requireSupplement(key);
    }

    static SupplementBase<T, isGarbageCollected>* from(SupplementableBase<T, isGarbageCollected>* host, const char* key)
    {
        return host ? host->requireSupplement(key) : 0;
    }
};

template<typename T, bool isGarbageCollected>
class SupplementableTracing;

template<typename T>
class SupplementableTracing<T, true> : public GarbageCollectedMixin {
    WTF_MAKE_NONCOPYABLE(SupplementableTracing);
public:
    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_supplements);
    }

protected:
    SupplementableTracing() { }
    typedef HeapHashMap<const char*, Member<SupplementBase<T, true>>, PtrHash<const char*>> SupplementMap;
    SupplementMap m_supplements;
};

template<typename T>
class SupplementableTracing<T, false> {
    WTF_MAKE_NONCOPYABLE(SupplementableTracing);
protected:
    SupplementableTracing() { }
    typedef HashMap<const char*, OwnPtr<SupplementBase<T, false>>, PtrHash<const char*>> SupplementMap;
    SupplementMap m_supplements;
};

// Helper class for implementing Supplementable and HeapSupplementable.
template<typename T, bool isGarbageCollected = false>
class SupplementableBase : public SupplementableTracing<T, isGarbageCollected> {
public:
    void provideSupplement(const char* key, typename SupplementableTraits<T, isGarbageCollected>::SupplementArgumentType supplement)
    {
        ASSERT(m_threadId == currentThread());
        ASSERT(!this->m_supplements.get(key));
        this->m_supplements.set(key, supplement);
    }

    void removeSupplement(const char* key)
    {
        ASSERT(m_threadId == currentThread());
        this->m_supplements.remove(key);
    }

    SupplementBase<T, isGarbageCollected>* requireSupplement(const char* key)
    {
        ASSERT(m_threadId == currentThread());
        return this->m_supplements.get(key);
    }

    void reattachThread()
    {
#if ENABLE(ASSERT)
        m_threadId = currentThread();
#endif
    }

#if ENABLE(ASSERT)
protected:
    SupplementableBase() : m_threadId(currentThread()) { }

private:
    ThreadIdentifier m_threadId;
#endif
};

template<typename T>
class HeapSupplement : public SupplementBase<T, true> { };

template<typename T>
class HeapSupplementable : public SupplementableBase<T, true> { };

template<typename T>
class Supplement : public SupplementBase<T, false> { };

template<typename T>
class Supplementable : public SupplementableBase<T, false> { };

template<typename T>
struct ThreadingTrait<SupplementBase<T, true>> {
    static const ThreadAffinity Affinity = ThreadingTrait<T>::Affinity;
};

template<typename T>
struct ThreadingTrait<SupplementableBase<T, true>> {
    static const ThreadAffinity Affinity = ThreadingTrait<T>::Affinity;
};

} // namespace blink

#endif // Supplementable_h
