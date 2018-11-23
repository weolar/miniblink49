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

#ifndef Partitions_h
#define Partitions_h

#include "wtf/WTF.h"
#include "wtf/WTFExport.h"
#include "wtf/PartitionAlloc.h"
#include <string.h>

namespace WTF {

class WTF_EXPORT Partitions {
public:
    typedef void (*ReportPartitionAllocSizeFunction)(size_t);

    // Name of allocator used by tracing for marking sub-allocations while take
    // memory snapshots.
    static const char* const kAllocatedObjectPoolName;

    static void initialize(ReportPartitionAllocSizeFunction);
    static void shutdown();

    static PartitionRootGeneric* bufferPartition();
    static PartitionRootGeneric* fastMallocPartition();

    static PartitionRoot* nodePartition();
    static PartitionRoot* layoutPartition();

    static size_t currentDOMMemoryUsage();

    static size_t totalSizeOfCommittedPages();

    static void decommitFreeableMemory();

    static void reportMemoryUsageHistogram();

    static void dumpMemoryStats(PartitionStatsDumper*);

    static void* bufferMalloc(size_t n, const char* typeName);
    static void bufferFree(void* p);
    static void* bufferRealloc(void* p, size_t n, const char* type_name);
    static size_t bufferActualSize(size_t n);
    static void handleOutOfMemory();

private:
    static SpinLock s_initializationLock;
    static bool s_initialized;

    // We have the following four partitions.
    //   - LayoutObject partition: A partition to allocate LayoutObjects.
    //     We prepare a dedicated partition for LayoutObjects because they
    //     are likely to be a source of use-after-frees. Another reason
    //     is for performance: As LayoutObjects are guaranteed to only be used
    //     by the main thread, we can bypass acquiring a lock. Also we can
    //     improve memory locality by putting LayoutObjects together.
    //   - Buffer partition: A partition to allocate objects that have a strong
    //     risk where the length and/or the contents are exploited from user
    //     scripts. Vectors, HashTables, ArrayBufferContents and Strings are
    //     allocated in the buffer partition.
    //   - Fast malloc partition: A partition to allocate all other objects.
    static PartitionAllocatorGeneric m_fastMallocAllocator;
    static PartitionAllocatorGeneric m_bufferAllocator;
    static SizeSpecificPartitionAllocator<1024> m_layoutAllocator;
    static ReportPartitionAllocSizeFunction m_reportSizeFunction;
};

} // namespace WTF

#endif // Partitions_h
