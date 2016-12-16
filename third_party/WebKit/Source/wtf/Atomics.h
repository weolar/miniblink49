/*
 * Copyright (C) 2007, 2008, 2010, 2012 Apple Inc. All rights reserved.
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

#ifndef Atomics_h
#define Atomics_h

#include "wtf/AddressSanitizer.h"
#include "wtf/Assertions.h"
#include "wtf/CPU.h"

#include <stdint.h>

#if COMPILER(MSVC)
#include <windows.h>
#endif

#if defined(THREAD_SANITIZER)
#include <sanitizer/tsan_interface_atomic.h>
#endif

#if defined(ADDRESS_SANITIZER)
#include <sanitizer/asan_interface.h>
#endif

namespace WTF {

#if COMPILER(MSVC)

// atomicAdd returns the result of the addition.
ALWAYS_INLINE int atomicAdd(int volatile* addend, int increment)
{
    return InterlockedExchangeAdd(reinterpret_cast<long volatile*>(addend), static_cast<long>(increment)) + increment;
}
ALWAYS_INLINE unsigned atomicAdd(unsigned volatile* addend, unsigned increment)
{
    return InterlockedExchangeAdd(reinterpret_cast<long volatile*>(addend), static_cast<long>(increment)) + increment;
}
#if defined(_WIN64)
ALWAYS_INLINE unsigned long long atomicAdd(unsigned long long volatile* addend, unsigned long long increment)
{
    return InterlockedExchangeAdd64(reinterpret_cast<long long volatile*>(addend), static_cast<long long>(increment)) + increment;
}
#endif

// atomicSubtract returns the result of the subtraction.
ALWAYS_INLINE int atomicSubtract(int volatile* addend, int decrement)
{
    return InterlockedExchangeAdd(reinterpret_cast<long volatile*>(addend), static_cast<long>(-decrement)) - decrement;
}
ALWAYS_INLINE unsigned atomicSubtract(unsigned volatile* addend, unsigned decrement)
{
    return InterlockedExchangeAdd(reinterpret_cast<long volatile*>(addend), -static_cast<long>(decrement)) - decrement;
}
#if defined(_WIN64)
ALWAYS_INLINE unsigned long long atomicSubtract(unsigned long long volatile* addend, unsigned long long decrement)
{
    return InterlockedExchangeAdd64(reinterpret_cast<long long volatile*>(addend), -static_cast<long long>(decrement)) - decrement;
}
#endif

ALWAYS_INLINE int atomicIncrement(int volatile* addend) { return InterlockedIncrement(reinterpret_cast<long volatile*>(addend)); }
ALWAYS_INLINE int atomicDecrement(int volatile* addend) { return InterlockedDecrement(reinterpret_cast<long volatile*>(addend)); }
#if _WIN32_WINNT < 0x0A00 && !defined(_WIN64)
__inline LONGLONG _Add64(volatile LONGLONG * destination, LONGLONG value)
{
    __asm
    {
        mov eax, dword ptr[value];
        mov edx, dword ptr[value + 4];
        mov ebx, [destination];
        lock xadd dword ptr[ebx], eax;
        adc dword ptr[ebx + 4], edx;
    }
}
#endif
ALWAYS_INLINE int64_t atomicIncrement(int64_t volatile* addend)
{
#if _WIN32_WINNT >= 0x0A00 || defined(_WIN64)
    return InterlockedIncrement64(reinterpret_cast<long long volatile*>(addend));
#else
    __asm
    {
        mov eax, 1;
        mov ebx, [addend];
        lock xadd dword ptr[ebx], eax;
        adc dword ptr[ebx + 4], 0;
    }
#endif
}

ALWAYS_INLINE int64_t atomicDecrement(int64_t volatile* addend)
{
#if _WIN32_WINNT >= 0x0A00 || defined(_WIN64)
    return InterlockedDecrement64(reinterpret_cast<long long volatile*>(addend));
#else
    __asm
    {
        mov eax, 0xFFFFFFFF;
        mov ebx, [addend];
        lock xadd dword ptr[ebx], eax;
        adc dword ptr[ebx + 4], 0xFFFFFFFF;
    }
#endif
}

ALWAYS_INLINE int atomicTestAndSetToOne(int volatile* ptr)
{
    int ret = InterlockedExchange(reinterpret_cast<long volatile*>(ptr), 1);
    ASSERT(!ret || ret == 1);
    return ret;
}

ALWAYS_INLINE void atomicSetOneToZero(int volatile* ptr)
{
    ASSERT(*ptr == 1);
    InterlockedExchange(reinterpret_cast<long volatile*>(ptr), 0);
}

#else

// atomicAdd returns the result of the addition.
ALWAYS_INLINE int atomicAdd(int volatile* addend, int increment) { return __sync_add_and_fetch(addend, increment); }
ALWAYS_INLINE unsigned atomicAdd(unsigned volatile* addend, unsigned increment) { return __sync_add_and_fetch(addend, increment); }
ALWAYS_INLINE unsigned long atomicAdd(unsigned long volatile* addend, unsigned long increment) { return __sync_add_and_fetch(addend, increment); }
// atomicSubtract returns the result of the subtraction.
ALWAYS_INLINE int atomicSubtract(int volatile* addend, int decrement) { return __sync_sub_and_fetch(addend, decrement); }
ALWAYS_INLINE unsigned atomicSubtract(unsigned volatile* addend, unsigned decrement) { return __sync_sub_and_fetch(addend, decrement); }
ALWAYS_INLINE unsigned long atomicSubtract(unsigned long volatile* addend, unsigned long decrement) { return __sync_sub_and_fetch(addend, decrement); }

ALWAYS_INLINE int atomicIncrement(int volatile* addend) { return atomicAdd(addend, 1); }
ALWAYS_INLINE int atomicDecrement(int volatile* addend) { return atomicSubtract(addend, 1); }

ALWAYS_INLINE int64_t atomicIncrement(int64_t volatile* addend) { return __sync_add_and_fetch(addend, 1); }
ALWAYS_INLINE int64_t atomicDecrement(int64_t volatile* addend) { return __sync_sub_and_fetch(addend, 1); }

ALWAYS_INLINE int atomicTestAndSetToOne(int volatile* ptr)
{
    int ret = __sync_lock_test_and_set(ptr, 1);
    ASSERT(!ret || ret == 1);
    return ret;
}

ALWAYS_INLINE void atomicSetOneToZero(int volatile* ptr)
{
    ASSERT(*ptr == 1);
    __sync_lock_release(ptr);
}
#endif

#if defined(THREAD_SANITIZER)
// The definitions below assume an LP64 data model. This is fine because
// TSan is only supported on x86_64 Linux.
#if CPU(64BIT) && OS(LINUX)
ALWAYS_INLINE void releaseStore(volatile int* ptr, int value)
{
    __tsan_atomic32_store(ptr, value, __tsan_memory_order_release);
}
ALWAYS_INLINE void releaseStore(volatile unsigned* ptr, unsigned value)
{
    __tsan_atomic32_store(reinterpret_cast<volatile int*>(ptr), static_cast<int>(value), __tsan_memory_order_release);
}
ALWAYS_INLINE void releaseStore(volatile unsigned long* ptr, unsigned long value)
{
    __tsan_atomic64_store(reinterpret_cast<volatile __tsan_atomic64*>(ptr), static_cast<__tsan_atomic64>(value), __tsan_memory_order_release);
}
ALWAYS_INLINE void releaseStore(volatile unsigned long long* ptr, unsigned long long value)
{
    __tsan_atomic64_store(reinterpret_cast<volatile __tsan_atomic64*>(ptr), static_cast<__tsan_atomic64>(value), __tsan_memory_order_release);
}
ALWAYS_INLINE void releaseStore(void* volatile* ptr, void* value)
{
    __tsan_atomic64_store(reinterpret_cast<volatile __tsan_atomic64*>(ptr), reinterpret_cast<__tsan_atomic64>(value), __tsan_memory_order_release);
}

ALWAYS_INLINE int acquireLoad(volatile const int* ptr)
{
    return __tsan_atomic32_load(ptr, __tsan_memory_order_acquire);
}
ALWAYS_INLINE unsigned acquireLoad(volatile const unsigned* ptr)
{
    return static_cast<unsigned>(__tsan_atomic32_load(reinterpret_cast<volatile const int*>(ptr), __tsan_memory_order_acquire));
}
ALWAYS_INLINE unsigned long acquireLoad(volatile const unsigned long* ptr)
{
    return static_cast<unsigned long>(__tsan_atomic64_load(reinterpret_cast<volatile const __tsan_atomic64*>(ptr), __tsan_memory_order_acquire));
}
ALWAYS_INLINE void* acquireLoad(void* volatile const* ptr)
{
    return reinterpret_cast<void*>(__tsan_atomic64_load(reinterpret_cast<volatile const __tsan_atomic64*>(ptr), __tsan_memory_order_acquire));
}
#endif

#else // defined(THREAD_SANITIZER)

#if CPU(X86) || CPU(X86_64)
// Only compiler barrier is needed.
#if COMPILER(MSVC)
// Starting from Visual Studio 2005 compiler guarantees acquire and release
// semantics for operations on volatile variables. See MSDN entry for
// MemoryBarrier macro.
#define MEMORY_BARRIER()
#else
#define MEMORY_BARRIER() __asm__ __volatile__("" : : : "memory")
#endif
#elif CPU(ARM) && (OS(LINUX) || OS(ANDROID))
// On ARM __sync_synchronize generates dmb which is very expensive on single
// core devices which don't actually need it. Avoid the cost by calling into
// kuser_memory_barrier helper.
inline void memoryBarrier()
{
    // Note: This is a function call, which is also an implicit compiler barrier.
    typedef void (*KernelMemoryBarrierFunc)();
    ((KernelMemoryBarrierFunc)0xffff0fa0)();
}
#define MEMORY_BARRIER() memoryBarrier()
#else
// Fallback to the compiler intrinsic on all other platforms.
#define MEMORY_BARRIER() __sync_synchronize()
#endif

ALWAYS_INLINE void releaseStore(volatile int* ptr, int value)
{
    MEMORY_BARRIER();
    *ptr = value;
}
ALWAYS_INLINE void releaseStore(volatile unsigned* ptr, unsigned value)
{
    MEMORY_BARRIER();
    *ptr = value;
}
ALWAYS_INLINE void releaseStore(volatile unsigned long* ptr, unsigned long value)
{
    MEMORY_BARRIER();
    *ptr = value;
}
ALWAYS_INLINE void releaseStore(volatile unsigned long long* ptr, unsigned long long value)
{
    MEMORY_BARRIER();
    *ptr = value;
}
ALWAYS_INLINE void releaseStore(void* volatile* ptr, void* value)
{
    MEMORY_BARRIER();
    *ptr = value;
}

ALWAYS_INLINE int acquireLoad(volatile const int* ptr)
{
    int value = *ptr;
    MEMORY_BARRIER();
    return value;
}
ALWAYS_INLINE unsigned acquireLoad(volatile const unsigned* ptr)
{
    unsigned value = *ptr;
    MEMORY_BARRIER();
    return value;
}
ALWAYS_INLINE unsigned long acquireLoad(volatile const unsigned long* ptr)
{
    unsigned long value = *ptr;
    MEMORY_BARRIER();
    return value;
}
ALWAYS_INLINE unsigned long long acquireLoad(volatile const unsigned long long* ptr)
{
    unsigned long long value = *ptr;
    MEMORY_BARRIER();
    return value;
}
ALWAYS_INLINE void* acquireLoad(void* volatile const* ptr)
{
    void* value = *ptr;
    MEMORY_BARRIER();
    return value;
}

#if defined(ADDRESS_SANITIZER)

NO_SANITIZE_ADDRESS ALWAYS_INLINE void asanUnsafeReleaseStore(volatile unsigned* ptr, unsigned value)
{
    MEMORY_BARRIER();
    *ptr = value;
}

NO_SANITIZE_ADDRESS ALWAYS_INLINE unsigned asanUnsafeAcquireLoad(volatile const unsigned* ptr)
{
    unsigned value = *ptr;
    MEMORY_BARRIER();
    return value;
}

#endif // defined(ADDRESS_SANITIZER)

#undef MEMORY_BARRIER

#endif

#if !defined(ADDRESS_SANITIZER)

ALWAYS_INLINE void asanUnsafeReleaseStore(volatile unsigned* ptr, unsigned value)
{
    releaseStore(ptr, value);
}

ALWAYS_INLINE unsigned asanUnsafeAcquireLoad(volatile const unsigned* ptr)
{
    return acquireLoad(ptr);
}

#endif

} // namespace WTF

using WTF::atomicAdd;
using WTF::atomicSubtract;
using WTF::atomicDecrement;
using WTF::atomicIncrement;
using WTF::atomicTestAndSetToOne;
using WTF::atomicSetOneToZero;
using WTF::acquireLoad;
using WTF::releaseStore;

// These methods allow loading from and storing to poisoned memory. Only
// use these methods if you know what you are doing since they will
// silence use-after-poison errors from ASan.
using WTF::asanUnsafeAcquireLoad;
using WTF::asanUnsafeReleaseStore;

#endif // Atomics_h
