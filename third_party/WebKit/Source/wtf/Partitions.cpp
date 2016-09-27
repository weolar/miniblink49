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

#include "config.h"
#include "wtf/Partitions.h"

#include "wtf/DefaultAllocator.h"
#include "wtf/FastMalloc.h"
#include "wtf/MainThread.h"

namespace WTF {

int Partitions::s_initializationLock = 0;
bool Partitions::s_initialized = false;

PartitionAllocatorGeneric Partitions::m_fastMallocAllocator;
PartitionAllocatorGeneric Partitions::m_bufferAllocator;
SizeSpecificPartitionAllocator<3328> Partitions::m_objectModelAllocator;
SizeSpecificPartitionAllocator<1024> Partitions::m_layoutAllocator;
HistogramEnumerationFunction Partitions::m_histogramEnumeration = nullptr;

void Partitions::initialize()
{
    spinLockLock(&s_initializationLock);

    if (!s_initialized) {
        m_fastMallocAllocator.init();
        m_bufferAllocator.init();
        m_objectModelAllocator.init();
        m_layoutAllocator.init();
        s_initialized = true;
    }

    spinLockUnlock(&s_initializationLock);
}

void Partitions::setHistogramEnumeration(HistogramEnumerationFunction histogramEnumeration)
{
    ASSERT(!m_histogramEnumeration);
    m_histogramEnumeration = histogramEnumeration;
}

void Partitions::shutdown()
{
    // We could ASSERT here for a memory leak within the partition, but it leads
    // to very hard to diagnose ASSERTs, so it's best to leave leak checking for
    // the valgrind and heapcheck bots, which run without partitions.
    (void) m_layoutAllocator.shutdown();
    (void) m_objectModelAllocator.shutdown();
    (void) m_bufferAllocator.shutdown();
    (void) m_fastMallocAllocator.shutdown();
}

void Partitions::decommitFreeableMemory()
{
    ASSERT(isMainThread());

    partitionPurgeMemoryGeneric(bufferPartition(), PartitionPurgeDecommitEmptyPages);
    partitionPurgeMemoryGeneric(fastMallocPartition(), PartitionPurgeDecommitEmptyPages);
    partitionPurgeMemory(objectModelPartition(), PartitionPurgeDecommitEmptyPages);
    partitionPurgeMemory(layoutPartition(), PartitionPurgeDecommitEmptyPages);
}

void Partitions::reportMemoryUsageHistogram()
{
    static size_t supportedMaxSizeInMB = 4 * 1024;
    static size_t observedMaxSizeInMB = 0;

    if (!m_histogramEnumeration)
        return;
    // We only report the memory in the main thread.
    if (!isMainThread())
        return;
    // +1 is for rounding up the sizeInMB.
    size_t sizeInMB = Partitions::totalSizeOfCommittedPages() / 1024 / 1024 + 1;
    if (sizeInMB >= supportedMaxSizeInMB)
        sizeInMB = supportedMaxSizeInMB - 1;
    if (sizeInMB > observedMaxSizeInMB) {
        // Send a UseCounter only when we see the highest memory usage
        // we've ever seen.
        m_histogramEnumeration("PartitionAlloc.CommittedSize", sizeInMB, supportedMaxSizeInMB);
        observedMaxSizeInMB = sizeInMB;
    }
}

void Partitions::dumpMemoryStats(PartitionStatsDumper* partitionStatsDumper)
{
    // Object model and rendering partitions are not thread safe and can be
    // accessed only on the main thread.
    ASSERT(isMainThread());

    partitionDumpStatsGeneric(fastMallocPartition(), "fast_malloc_partition", partitionStatsDumper);
    partitionDumpStatsGeneric(bufferPartition(), "buffer_partition", partitionStatsDumper);
    partitionDumpStats(objectModelPartition(), "object_model_partition", partitionStatsDumper);
    partitionDumpStats(layoutPartition(), "layout_partition", partitionStatsDumper);
}

} // namespace WTF
