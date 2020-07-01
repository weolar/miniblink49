// Copyright (c) 2005, 2007, Google Inc.
// All rights reserved.
// Copyright (C) 2005, 2006, 2007, 2008, 2009, 2011 Apple Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "config.h"
#include "wtf/FastMalloc.h"

#include "wtf/Partitions.h"
#include <string.h>

size_t g_blinkMemSize = 0;
#include "base/process/CallAddrsRecord.h"

namespace WTF {

void* fastZeroedMalloc(size_t n)
{
#ifdef ENABLE_MEM_COUNT
    RECORD_LOCK();
#endif
    void* result = fastMalloc(n);
#ifdef ENABLE_MEM_COUNT
    RECORD_MALLOC(((size_t*)result) - 1, true);
    RECORD_UNLOCK();
#endif
    memset(result, 0, n);
    return result;
}

char* fastStrDup(const char* src)
{
#ifdef ENABLE_MEM_COUNT
    RECORD_LOCK();
#endif
    size_t len = strlen(src) + 1;
    char* dup = static_cast<char*>(fastMalloc(len));
#ifdef ENABLE_MEM_COUNT
    RECORD_MALLOC(((size_t*)dup) - 1, true);
    RECORD_UNLOCK();
#endif
    memcpy(dup, src, len);
    return dup;
}

void* fastMalloc(size_t n)
{
#ifdef ENABLE_MEM_COUNT
    RECORD_LOCK();
    n += sizeof(size_t);
#endif
    void* result = partitionAllocGeneric(Partitions::fastMallocPartition(), n, "fastMalloc");
#ifdef ENABLE_MEM_COUNT
    RECORD_MALLOC(result, false);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_blinkMemSize), static_cast<long>(n));
    *(size_t*)result = n;
    result = (char*)result + sizeof(size_t);
    RECORD_UNLOCK();
#endif
    return result;
}

void fastFree(void* p)
{
#ifdef ENABLE_MEM_COUNT
    if (!p)
        return;
    RECORD_LOCK();
    size_t* ptr = (size_t*)p;
    --ptr;
    size_t size = *ptr;
    p = ptr;
    RECORD_FREE(p);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_blinkMemSize), -static_cast<long>(size));
#endif
    partitionFreeGeneric(Partitions::fastMallocPartition(), p);

#ifdef ENABLE_MEM_COUNT
    RECORD_UNLOCK();
#endif
}

void* fastRealloc(void* p, size_t n)
{
    void* result = nullptr;
#ifdef ENABLE_MEM_COUNT
    size_t* ptr = nullptr;
    if (!p) {
        RECORD_LOCK();
        result = fastMalloc(n);
        ptr = (size_t*)result;
        RECORD_MALLOC(ptr - 1, true);
        RECORD_UNLOCK();
        return result;
    }
    
    RECORD_LOCK();
    ptr = (size_t*)p;
    --ptr;
    size_t size = *ptr;
    p = ptr;
    n += sizeof(size_t);
#endif
    result = partitionReallocGeneric(Partitions::fastMallocPartition(), p, n, "fastRealloc");
#ifdef ENABLE_MEM_COUNT
    RECORD_REALLOC(p, result);
    *(size_t*)result = n;
    result = (char*)result + sizeof(size_t);
    InterlockedExchangeAdd(reinterpret_cast<long volatile*>(&g_blinkMemSize), static_cast<long>(n - size));
    RECORD_UNLOCK();
#endif
    return result;
}

} // namespace WTF
