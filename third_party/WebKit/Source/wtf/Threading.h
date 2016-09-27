/*
 * Copyright (C) 2007, 2008, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Justin Haygood (jhaygood@reaktix.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Threading_h
#define Threading_h

#include "wtf/Atomics.h"
#include "wtf/TypeTraits.h"
#include "wtf/WTFExport.h"
#include <stdint.h>

// For portability, we do not make use of C++11 thread-safe statics, as supported
// by some toolchains. Make use of double-checked locking to reduce overhead.
#define AtomicallyInitializedStaticReferenceInternal(T, name, initializer, LOCK, UNLOCK) \
    /* Init to nullptr is thread-safe on all implementations. */        \
    static void* name##Pointer = nullptr;                               \
    if (!WTF::acquireLoad(&name##Pointer)) {                            \
        LOCK;                                                           \
        if (!WTF::acquireLoad(&name##Pointer)) {                        \
            WTF::RemoveConst<T>::Type* initializerResult = initializer; \
            WTF::releaseStore(&name##Pointer, initializerResult);       \
        }                                                               \
        UNLOCK;                                                         \
    }                                                                   \
    T& name = *static_cast<T*>(name##Pointer)

// Uses system-wide default lock. This version cannot be used before
// WTF::initializeThreading() is called.
#define AtomicallyInitializedStaticReference(T, name, initializer)      \
    AtomicallyInitializedStaticReferenceInternal(                       \
        T, name, initializer, \
        WTF::lockAtomicallyInitializedStaticMutex(),                    \
        WTF::unlockAtomicallyInitializedStaticMutex())

// Same as above but uses a given lock.
#define AtomicallyInitializedStaticReferenceWithLock(T, name, initializer, lockable)  \
    AtomicallyInitializedStaticReferenceInternal(                       \
        T, name, initializer, lockable.lock(), lockable.unlock());

namespace WTF {

#if OS(WIN)
typedef uint32_t ThreadIdentifier;
#else
typedef intptr_t ThreadIdentifier;
#endif

WTF_EXPORT ThreadIdentifier currentThread();

WTF_EXPORT void lockAtomicallyInitializedStaticMutex();
WTF_EXPORT void unlockAtomicallyInitializedStaticMutex();

#if ENABLE(ASSERT)
WTF_EXPORT bool isAtomicallyInitializedStaticMutexLockHeld();
WTF_EXPORT bool isBeforeThreadCreated();
WTF_EXPORT void willCreateThread();
#endif

} // namespace WTF

using WTF::ThreadIdentifier;
using WTF::currentThread;

#endif // Threading_h
