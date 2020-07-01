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

#ifndef WTF_PartitionAllocator_h
#define WTF_PartitionAllocator_h

// This is the allocator that is used for allocations that are not on the
// traced, garbage collected heap. It uses FastMalloc for collections,
// but uses the partition allocator for the backing store of the collections.

#include "wtf/Assertions.h"
#include "wtf/PartitionAlloc.h"
#include "wtf/Partitions.h"
#include "wtf/StdLibExtras.h"
#include "wtf/FastMalloc.h"

#include <string.h>

namespace WTF {

class PartitionAllocatorDummyVisitor {
private:
    void* operator new(size_t) = delete;
    void* operator new(size_t, NotNullTag, void*) = delete;
    void* operator new(size_t, void*) = delete;
};

class WTF_EXPORT PartitionAllocator {
public:
    typedef PartitionAllocatorDummyVisitor Visitor;
    static const bool isGarbageCollected = false;

    template<typename T>
    static size_t quantizedSize(size_t count)
    {
        RELEASE_ASSERT(count <= kGenericMaxDirectMapped / sizeof(T));
        return partitionAllocActualSize(Partitions::bufferPartition(), count * sizeof(T));
    }
    template <typename T>
    static T* allocateVectorBacking(size_t size)
    {
        return reinterpret_cast<T*>(allocateBacking(size, WTF_HEAP_PROFILER_TYPE_NAME(T)));
    }
    template <typename T>
    static T* allocateExpandedVectorBacking(size_t size)
    {
        return reinterpret_cast<T*>(allocateBacking(size, WTF_HEAP_PROFILER_TYPE_NAME(T)));
    }
    static void freeVectorBacking(void* address);
    static inline bool expandVectorBacking(void*, size_t)
    {
        return false;
    }
    static inline bool shrinkVectorBacking(void* address, size_t quantizedCurrentSize, size_t quantizedShrunkSize)
    {
        // Optimization: if we're downsizing inside the same allocator bucket,
        // we can skip reallocation.
        return quantizedCurrentSize == quantizedShrunkSize;
    }
    template <typename T>
    static T* allocateInlineVectorBacking(size_t size) { return allocateVectorBacking<T>(size); }
    static inline void freeInlineVectorBacking(void* address) { freeVectorBacking(address); }
    static inline bool expandInlineVectorBacking(void*, size_t) { return false; }
    static inline bool shrinkInlineVectorBacking(void* address, size_t quantizedCurrentSize, size_t quantizedShrunkSize) { return shrinkVectorBacking(address, quantizedCurrentSize, quantizedShrunkSize); }

    template <typename T, typename HashTable>
    static T* allocateHashTableBacking(size_t size)
    {
        return reinterpret_cast<T*>(allocateBacking(size, WTF_HEAP_PROFILER_TYPE_NAME(T)));
    }
    template <typename T, typename HashTable>
    static T* allocateZeroedHashTableBacking(size_t size)
    {
        void* result = allocateBacking(size, WTF_HEAP_PROFILER_TYPE_NAME(T));
        memset(result, 0, size);
        return reinterpret_cast<T*>(result);
    }
    static void freeHashTableBacking(void* address);

    template <typename Return, typename Metadata>
    static Return malloc(size_t size, const char* typeName)
    {
        return reinterpret_cast<Return>(Partitions::fastMalloc(size, typeName));
    }

    static inline bool expandHashTableBacking(void*, size_t)
    {
        return false;
    }
    static void free(void* address)
    {
        fastFree(address);
    }
    template<typename T>
    static void* newArray(size_t bytes)
    {
        return malloc<void*, void>(bytes, WTF_HEAP_PROFILER_TYPE_NAME(T));
    }
    static void
    deleteArray(void* ptr)
    {
        free(ptr); // Not the system free, the one from this class.
    }

    static bool isAllocationAllowed() { return true; }

    static void enterGCForbiddenScope() { }
    static void leaveGCForbiddenScope() { }

private:
    static void* allocateBacking(size_t, const char* typeName);
};

// Specializations for heap profiling, so type profiling of |char| is possible
// even in official builds (because |char| makes up a large portion of the heap.)
template <> WTF_EXPORT char* PartitionAllocator::allocateVectorBacking<char>(size_t);
template <> WTF_EXPORT char* PartitionAllocator::allocateExpandedVectorBacking<char>(size_t);

} // namespace WTF

#define WTF_USE_ALLOCATOR(ClassName, Allocator) \
public: \
    void* operator new(size_t size) \
    { \
        return Allocator::template malloc<void*, ClassName>(size, WTF_HEAP_PROFILER_TYPE_NAME(ClassName)); \
    } \
    void operator delete(void* p) { Allocator::free(p); } \
    void* operator new[](size_t size) { return Allocator::template newArray<ClassName>(size); } \
    void operator delete[](void* p) { Allocator::deleteArray(p); } \
    void* operator new(size_t, NotNullTag, void* location) \
    { \
        ASSERT(location); \
        return location; \
    } \
private: \
typedef int __thisIsHereToForceASemicolonAfterThisMacro

#define USE_ALLOCATOR(ClassName, Allocator) WTF_USE_ALLOCATOR(ClassName, Allocator)

using WTF::PartitionAllocator;

#endif // WTF_PartitionAllocator_h
