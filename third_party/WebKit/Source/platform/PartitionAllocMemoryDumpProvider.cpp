// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "Source/platform/PartitionAllocMemoryDumpProvider.h"

#include "public/platform/WebMemoryAllocatorDump.h"
#include "public/platform/WebProcessMemoryDump.h"
#include "wtf/Partitions.h"
#include "wtf/Threading.h"

namespace blink {

namespace {

using namespace WTF;

// This class is used to invert the dependency of PartitionAlloc on the
// PartitionAllocMemoryDumpProvider. This implements an interface that will
// be called with memory statistics for each bucket in the allocator.
class PartitionStatsDumperImpl final : public PartitionStatsDumper {
public:
    explicit PartitionStatsDumperImpl(WebProcessMemoryDump* memoryDump)
        : m_memoryDump(memoryDump), m_uid(0) { }

    void partitionDumpTotals(const char* partitionName, const PartitionMemoryStats*) override;

    // PartitionStatsDumper implementation.
    void partitionsDumpBucketStats(const char* partitionName, const PartitionBucketMemoryStats*) override;

private:
    WebProcessMemoryDump* m_memoryDump;
    size_t m_uid;
};

void PartitionStatsDumperImpl::partitionDumpTotals(const char* partitionName, const PartitionMemoryStats*)
{
    ;
}

void PartitionStatsDumperImpl::partitionsDumpBucketStats(const char* partitionName, const PartitionBucketMemoryStats* memoryStats)
{
    ASSERT(memoryStats->isValid);
    String dumpName;
    if (memoryStats->isDirectMap)
        dumpName = String::format("partition_alloc/thread_%lu/%s/directMap_%lu", static_cast<unsigned long>(WTF::currentThread()), partitionName, static_cast<unsigned long>(++m_uid));
    else
        dumpName = String::format("partition_alloc/thread_%lu/%s/bucket_%u", static_cast<unsigned long>(WTF::currentThread()), partitionName, static_cast<unsigned>(memoryStats->bucketSlotSize));

    WebMemoryAllocatorDump* allocatorDump = m_memoryDump->createMemoryAllocatorDump(dumpName);
    allocatorDump->AddScalar("size", "bytes", memoryStats->residentBytes);
    allocatorDump->AddScalar("slot_size", "bytes", memoryStats->bucketSlotSize);
    allocatorDump->AddScalar("active_size", "bytes", memoryStats->activeBytes);
    allocatorDump->AddScalar("resident_size", "bytes", memoryStats->residentBytes);
    allocatorDump->AddScalar("decommittable_size", "bytes", memoryStats->decommittableBytes);
    allocatorDump->AddScalar("discardable_size", "bytes", memoryStats->discardableBytes);
    allocatorDump->AddScalar("num_active", "objects", memoryStats->numActivePages);
    allocatorDump->AddScalar("num_full", "objects", memoryStats->numFullPages);
    allocatorDump->AddScalar("num_empty", "objects", memoryStats->numEmptyPages);
    allocatorDump->AddScalar("num_decommitted", "objects", memoryStats->numDecommittedPages);
    allocatorDump->AddScalar("page_size", "bytes", memoryStats->allocatedPageSize);

    dumpName = dumpName + "/allocated_objects";
    WebMemoryAllocatorDump* objectsDump = m_memoryDump->createMemoryAllocatorDump(dumpName);
    objectsDump->AddScalar("size", "bytes", memoryStats->activeBytes);
}

} // namespace

PartitionAllocMemoryDumpProvider* PartitionAllocMemoryDumpProvider::instance()
{
    DEFINE_STATIC_LOCAL(PartitionAllocMemoryDumpProvider, instance, ());
    return &instance;
}

bool PartitionAllocMemoryDumpProvider::onMemoryDump(blink::WebProcessMemoryDump* memoryDump)
{
    PartitionStatsDumperImpl partitionStatsDumper(memoryDump);

    // This method calls memoryStats.partitionsDumpBucketStats with memory statistics.
    WTF::Partitions::dumpMemoryStats(&partitionStatsDumper);
    return true;
}

PartitionAllocMemoryDumpProvider::PartitionAllocMemoryDumpProvider()
{
}

PartitionAllocMemoryDumpProvider::~PartitionAllocMemoryDumpProvider()
{
}

} // namespace blink
