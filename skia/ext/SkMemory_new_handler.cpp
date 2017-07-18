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

#include "base/process/CallAddrsRecord.h"

// This implementation of sk_malloc_flags() and friends is identical to
// SkMemory_malloc.cpp, except that it disables the CRT's new_handler during
// malloc() and calloc() when SK_MALLOC_THROW is not set (because our normal
// new_handler itself will crash on failure when using tcmalloc).

SK_DECLARE_STATIC_MUTEX(gSkNewHandlerMutex);

#ifdef ENABLE_MEM_COUNT
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

struct MemoryHead {
    size_t magicNum;
    size_t size;
};

const size_t magicNum0 = 0x1122dd44;
const size_t magicNum1 = 0x11227788;

void* sk_realloc_throw(void* addr, size_t size) {
    if (0 == size)
        abort();

    void* result = nullptr;
#ifdef ENABLE_MEM_COUNT
    MemoryHead* ptr;
    if (!addr) {
        size += sizeof(MemoryHead);

        RECORD_LOCK();
        ptr = (MemoryHead*)malloc(size);
        RECORD_MALLOC(ptr, false);
        
        InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
        ptr->magicNum = magicNum0;
        ptr->size = size;
        result = ptr + 1;
        RECORD_UNLOCK();

        return result;
    }

    ptr = (MemoryHead*)addr - 1;
    if (ptr->magicNum != magicNum0)
        DebugBreak();
    ptr->magicNum = magicNum1;
    size_t oldSize = ptr->size;
    addr = ptr;
    size += sizeof(MemoryHead);
    RECORD_LOCK();
#endif

    result = realloc(addr, size);

#ifdef ENABLE_MEM_COUNT
    RECORD_REALLOC(addr, result);

    ptr = (MemoryHead*)result;
    ptr->magicNum = magicNum0;
    ptr->size = size;
    result = ptr + 1;
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size - oldSize));
    RECORD_UNLOCK();
#endif
    return result;
}

void sk_free(void* p) {
    if (!p)
        return;

#ifdef ENABLE_MEM_COUNT
    RECORD_LOCK();

    MemoryHead* ptr = (MemoryHead*)p - 1;
    if (ptr->magicNum != magicNum0)
        DebugBreak();
    ptr->magicNum = magicNum1;
    size_t size = ptr->size;
    p = ptr;

    RECORD_FREE(p);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), -static_cast<long>(size));
#endif
    free(p);

#ifdef ENABLE_MEM_COUNT
    RECORD_UNLOCK();
#endif

}

void* sk_malloc_throw(size_t size) {
    if (0 == size)
        return nullptr;

#ifdef ENABLE_MEM_COUNT
    size += sizeof(MemoryHead);
    RECORD_LOCK();
#endif

    void* result = malloc(size);

#ifdef ENABLE_MEM_COUNT
    MemoryHead* ptr = (MemoryHead*)result;
    RECORD_MALLOC(ptr, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
    ptr->magicNum = magicNum0;
    ptr->size = size;
    result = ptr + 1;

    RECORD_UNLOCK();
#endif
    return result;
}

static void* sk_malloc_nothrow(size_t size) {
    if (0 == size)
        return nullptr;

#ifdef ENABLE_MEM_COUNT
    size += sizeof(MemoryHead);
    RECORD_LOCK();
#endif

    void* result = malloc(size);

#ifdef ENABLE_MEM_COUNT
    MemoryHead* ptr = (MemoryHead*)result;
    RECORD_MALLOC(ptr, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
    ptr->magicNum = magicNum0;
    ptr->size = size;
    result = ptr + 1;

    RECORD_UNLOCK();
#endif
    return result;
}

void* sk_malloc_flags(size_t size, unsigned flags) {
    if (0 == size)
        return nullptr;

#ifdef ENABLE_MEM_COUNT
    size += sizeof(MemoryHead);
    RECORD_LOCK();
#endif

    void* result = malloc(size);

#ifdef ENABLE_MEM_COUNT
    MemoryHead* ptr = (MemoryHead*)result;
    RECORD_MALLOC(ptr, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
    ptr->magicNum = magicNum0;
    ptr->size = size;
    result = ptr + 1;

    RECORD_UNLOCK();
#endif
    return result;
}

void* sk_calloc_throw(size_t size) {
    if (0 == size)
        return nullptr;

#ifdef ENABLE_MEM_COUNT
    size += sizeof(MemoryHead);
    RECORD_LOCK();
#endif

    void* result = calloc(size, 1);

#ifdef ENABLE_MEM_COUNT
    MemoryHead* ptr = (MemoryHead*)result;
    RECORD_MALLOC(ptr, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
    ptr->magicNum = magicNum0;
    ptr->size = size;
    result = ptr + 1;

    RECORD_UNLOCK();
#endif
    return result;
}

void* sk_calloc(size_t size) {
    if (0 == size)
        return nullptr;

#ifdef ENABLE_MEM_COUNT
    size += sizeof(MemoryHead);
    RECORD_LOCK();
#endif

    void* result = calloc(size, 1);

#ifdef ENABLE_MEM_COUNT
    MemoryHead* ptr = (MemoryHead*)result;
    RECORD_MALLOC(ptr, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_skiaMemSize), static_cast<long>(size));
    ptr->magicNum = magicNum0;
    ptr->size = size;
    result = ptr + 1;

    RECORD_UNLOCK();
#endif
    return result;
}
