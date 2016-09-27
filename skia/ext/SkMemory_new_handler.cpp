// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <new>

#include "base/process/memory.h"

#include "third_party/skia/include/core/SkTypes.h"
#include "third_party/skia/include/ports/SkMutex_win.h"
#include "third_party/skia/include/core/SkMutex.h"

#ifdef _DEBUG
#include "base/process/CallAddrsRecord.h"
#endif

// This implementation of sk_malloc_flags() and friends is identical to
// SkMemory_malloc.cpp, except that it disables the CRT's new_handler during
// malloc() and calloc() when SK_MALLOC_THROW is not set (because our normal
// new_handler itself will crash on failure when using tcmalloc).

SK_DECLARE_STATIC_MUTEX(gSkNewHandlerMutex);

#ifdef _DEBUG
size_t g_skiaMemSize = 0;
#endif

static inline void* throw_on_failure(size_t size, void* p) {
    if (size > 0 && p == NULL) {
        // If we've got a NULL here, the only reason we should have failed is running out of RAM.
        sk_out_of_memory();
    }
    return p;
}

void sk_throw() {
    SkASSERT(!"sk_throw");
    abort();
}

void sk_out_of_memory(void) {
    SkASSERT(!"sk_out_of_memory");
    abort();
}

static void* sk_malloc_nothrow(size_t size);

void* sk_realloc_throw(void* addr, size_t size) {
    void* result = nullptr;
#ifdef _DEBUG
    size_t* ptr;
    if (!addr) {
        result = sk_malloc_nothrow(size);
        ptr = (size_t*)result;
        RECORD_MALLOC(ptr - 1, true);
        return result;
    }

    ptr = (size_t*)addr;
    --ptr;
    size_t oldSize = *ptr;
    addr = ptr;
    size += sizeof(size_t);
    
#endif
    result = realloc(addr, size);
#ifdef _DEBUG
    RECORD_REALLOC(addr, result);

    *(size_t*)result = size;
    result = (char*)result + sizeof(size_t);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size - oldSize));    
#endif
    return throw_on_failure(size, result);
}

void sk_free(void* p) {
    if (!p)
        return;

#ifdef _DEBUG
    size_t* ptr = (size_t*)p;
    --ptr;
    size_t size = *ptr;
    p = ptr;

    RECORD_FREE(p);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), -static_cast<long>(size));
#endif
    free(p);
}

void* sk_malloc_throw(size_t size) {
#ifdef _DEBUG // weolar
    size += sizeof(size_t);
#endif
    void* result = malloc(size);
#ifdef _DEBUG
    RECORD_MALLOC(result, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
    *(size_t*)result = size;
    result = (char*)result + sizeof(size_t);
#endif
    return throw_on_failure(size, result);
}

static void* sk_malloc_nothrow(size_t size) {
#ifdef _DEBUG // weolar
    size += sizeof(size_t);
#endif

    void* result;
    // TODO(b.kelemen): we should always use UncheckedMalloc but currently it
    // doesn't work as intended everywhere.
#if  defined(LIBC_GLIBC) || defined(USE_TCMALLOC) || \
     (defined(OS_MACOSX) && !defined(OS_IOS)) || defined(OS_ANDROID)
    // It's the responsibility of the caller to check the return value.
    ignore_result(base::UncheckedMalloc(size, &result));
#else
    // This is not really thread safe.  It only won't collide with itself, but we're totally
    // unprotected from races with other code that calls set_new_handler.
    SkAutoMutexAcquire lock(gSkNewHandlerMutex);
    std::new_handler old_handler = std::set_new_handler(NULL);
    result = malloc(size);
    std::set_new_handler(old_handler);
    return result;
#endif

#ifdef _DEBUG
    RECORD_MALLOC(result, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
    *(size_t*)result = size;
    result = (char*)result + sizeof(size_t);
#endif
    return result;
}

void* sk_malloc_flags(size_t size, unsigned flags) {
    void* result;
    if (flags & SK_MALLOC_THROW)
        result = sk_malloc_throw(size);
    else
        result = sk_malloc_nothrow(size);
#ifdef _DEBUG
    RECORD_MALLOC(((size_t*)result) - 1, true);
#endif
    return result;
}

void* sk_calloc_throw(size_t size) {
#ifdef _DEBUG // weolar
    size += sizeof(size_t);
#endif
    void* result = calloc(size, 1);
#ifdef _DEBUG
    RECORD_MALLOC(result, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
    *(size_t*)result = size;
    result = (char*)result + sizeof(size_t);
#endif
    return throw_on_failure(size, result);
}

void* sk_calloc(size_t size) {
#ifdef _DEBUG // weolar
    size += sizeof(size_t);
#endif

    void* result;
    // TODO(b.kelemen): we should always use UncheckedCalloc but currently it
    // doesn't work as intended everywhere.
#if  defined(LIBC_GLIBC) || defined(USE_TCMALLOC) || \
     (defined(OS_MACOSX) && !defined(OS_IOS)) || defined(OS_ANDROID)
    
    // It's the responsibility of the caller to check the return value.
    ignore_result(base::UncheckedCalloc(size, 1, &result));
#else
    SkAutoMutexAcquire lock(gSkNewHandlerMutex);
    std::new_handler old_handler = std::set_new_handler(NULL);
    result = calloc(size, 1);
    std::set_new_handler(old_handler);
#endif

#ifdef _DEBUG
    RECORD_MALLOC(result, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
    *(size_t*)result = size;
    result = (char*)result + sizeof(size_t);
#endif

    return result;
}
