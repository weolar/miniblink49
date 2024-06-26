// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/process_memory_dump.h"

#include <errno.h>
#include <vector>

#include "base/process/process_metrics.h"
#include "base/stl_util.h"
#include "base/trace_event/process_memory_totals.h"
#include "base/trace_event/trace_event_argument.h"

#if defined(OS_POSIX)
#include <sys/mman.h>
#endif

namespace base {
namespace trace_event {

    namespace {

        const char kEdgeTypeOwnership[] = "ownership";

        std::string GetSharedGlobalAllocatorDumpName(
            const MemoryAllocatorDumpGuid& guid)
        {
            return "global/" + guid.ToString();
        }

    } // namespace

#if defined(COUNT_RESIDENT_BYTES_SUPPORTED)
    // static
    size_t ProcessMemoryDump::CountResidentBytes(void* start_address,
        size_t mapped_size)
    {
        const size_t page_size = GetPageSize();
        const uintptr_t start_pointer = reinterpret_cast<uintptr_t>(start_address);
        DCHECK_EQ(0u, start_pointer % page_size);

        // This function allocates a char vector of size number of pages in the given
        // mapped_size. To avoid allocating a large array, the memory is split into
        // chunks. Maximum size of vector allocated, will be
        // kPageChunkSize / page_size.
        const size_t kMaxChunkSize = 32 * 1024 * 1024;
        size_t offset = 0;
        size_t total_resident_size = 0;
        int result = 0;
        while (offset < mapped_size) {
            void* chunk_start = reinterpret_cast<void*>(start_pointer + offset);
            const size_t chunk_size = std::min(mapped_size - offset, kMaxChunkSize);
            const size_t page_count = (chunk_size + page_size - 1) / page_size;
            size_t resident_page_count = 0;

#if defined(OS_MACOSX) || defined(OS_IOS)
            std::vector<char> vec(page_count + 1);
            // mincore in MAC does not fail with EAGAIN.
            result = mincore(chunk_start, chunk_size, vector_as_array(&vec));
            if (result)
                break;

            for (size_t i = 0; i < page_count; i++)
                resident_page_count += vec[i] & MINCORE_INCORE ? 1 : 0;
#else // defined(OS_MACOSX) || defined(OS_IOS)
            std::vector<unsigned char> vec(page_count + 1);
            int error_counter = 0;
            // HANDLE_EINTR tries for 100 times. So following the same pattern.
            do {
                result = mincore(chunk_start, chunk_size, vector_as_array(&vec));
            } while (result == -1 && errno == EAGAIN && error_counter++ < 100);
            if (result)
                break;

            for (size_t i = 0; i < page_count; i++)
                resident_page_count += vec[i];
#endif // defined(OS_MACOSX) || defined(OS_IOS)

            total_resident_size += resident_page_count * page_size;
            offset += kMaxChunkSize;
        }

        DCHECK_EQ(0, result);
        if (result) {
            total_resident_size = 0;
            LOG(ERROR) << "mincore() call failed. The resident size is invalid";
        }
        return total_resident_size;
    }
#endif // defined(COUNT_RESIDENT_BYTES_SUPPORTED)

    ProcessMemoryDump::ProcessMemoryDump(
        const scoped_refptr<MemoryDumpSessionState>& session_state)
        : has_process_totals_(false)
        , has_process_mmaps_(false)
        , session_state_(session_state)
    {
    }

    ProcessMemoryDump::~ProcessMemoryDump()
    {
    }

    MemoryAllocatorDump* ProcessMemoryDump::CreateAllocatorDump(
        const std::string& absolute_name)
    {
        MemoryAllocatorDump* mad = new MemoryAllocatorDump(absolute_name, this);
        AddAllocatorDumpInternal(mad); // Takes ownership of |mad|.
        return mad;
    }

    MemoryAllocatorDump* ProcessMemoryDump::CreateAllocatorDump(
        const std::string& absolute_name,
        const MemoryAllocatorDumpGuid& guid)
    {
        MemoryAllocatorDump* mad = new MemoryAllocatorDump(absolute_name, this, guid);
        AddAllocatorDumpInternal(mad); // Takes ownership of |mad|.
        return mad;
    }

    void ProcessMemoryDump::AddAllocatorDumpInternal(MemoryAllocatorDump* mad)
    {
        DCHECK_EQ(0ul, allocator_dumps_.count(mad->absolute_name()));
        allocator_dumps_storage_.push_back(mad);
        allocator_dumps_[mad->absolute_name()] = mad;
    }

    MemoryAllocatorDump* ProcessMemoryDump::GetAllocatorDump(
        const std::string& absolute_name) const
    {
        auto it = allocator_dumps_.find(absolute_name);
        return it == allocator_dumps_.end() ? nullptr : it->second;
    }

    MemoryAllocatorDump* ProcessMemoryDump::GetOrCreateAllocatorDump(
        const std::string& absolute_name)
    {
        MemoryAllocatorDump* mad = GetAllocatorDump(absolute_name);
        return mad ? mad : CreateAllocatorDump(absolute_name);
    }

    MemoryAllocatorDump* ProcessMemoryDump::CreateSharedGlobalAllocatorDump(
        const MemoryAllocatorDumpGuid& guid)
    {
        // A shared allocator dump can be shared within a process and the guid could
        // have been created already.
        MemoryAllocatorDump* allocator_dump = GetSharedGlobalAllocatorDump(guid);
        return allocator_dump ? allocator_dump
                              : CreateAllocatorDump(
                                  GetSharedGlobalAllocatorDumpName(guid), guid);
    }

    MemoryAllocatorDump* ProcessMemoryDump::GetSharedGlobalAllocatorDump(
        const MemoryAllocatorDumpGuid& guid) const
    {
        return GetAllocatorDump(GetSharedGlobalAllocatorDumpName(guid));
    }

    void ProcessMemoryDump::AddHeapDump(const std::string& absolute_name,
        scoped_refptr<TracedValue> heap_dump)
    {
        DCHECK_EQ(0ul, heap_dumps_.count(absolute_name));
        heap_dumps_[absolute_name] = heap_dump;
    }

    void ProcessMemoryDump::Clear()
    {
        if (has_process_totals_) {
            process_totals_.Clear();
            has_process_totals_ = false;
        }

        if (has_process_mmaps_) {
            process_mmaps_.Clear();
            has_process_mmaps_ = false;
        }

        allocator_dumps_storage_.clear();
        allocator_dumps_.clear();
        allocator_dumps_edges_.clear();
        heap_dumps_.clear();
    }

    void ProcessMemoryDump::TakeAllDumpsFrom(ProcessMemoryDump* other)
    {
        DCHECK(!other->has_process_totals() && !other->has_process_mmaps());

        // Moves the ownership of all MemoryAllocatorDump(s) contained in |other|
        // into this ProcessMemoryDump.
        for (MemoryAllocatorDump* mad : other->allocator_dumps_storage_) {
            // Check that we don't merge duplicates.
            DCHECK_EQ(0ul, allocator_dumps_.count(mad->absolute_name()));
            allocator_dumps_storage_.push_back(mad);
            allocator_dumps_[mad->absolute_name()] = mad;
        }
        other->allocator_dumps_storage_.weak_clear();
        other->allocator_dumps_.clear();

        // Move all the edges.
        allocator_dumps_edges_.insert(allocator_dumps_edges_.end(),
            other->allocator_dumps_edges_.begin(),
            other->allocator_dumps_edges_.end());
        other->allocator_dumps_edges_.clear();

        heap_dumps_.insert(other->heap_dumps_.begin(), other->heap_dumps_.end());
        other->heap_dumps_.clear();
    }

    void ProcessMemoryDump::AsValueInto(TracedValue* value) const
    {
        if (has_process_totals_) {
            value->BeginDictionary("process_totals");
            process_totals_.AsValueInto(value);
            value->EndDictionary();
        }

        if (has_process_mmaps_) {
            value->BeginDictionary("process_mmaps");
            process_mmaps_.AsValueInto(value);
            value->EndDictionary();
        }

        if (allocator_dumps_storage_.size() > 0) {
            value->BeginDictionary("allocators");
            for (const MemoryAllocatorDump* allocator_dump : allocator_dumps_storage_)
                allocator_dump->AsValueInto(value);
            value->EndDictionary();
        }

        if (heap_dumps_.size() > 0) {
            value->BeginDictionary("heaps");
            for (const auto& name_and_dump : heap_dumps_)
                value->SetValueWithCopiedName(name_and_dump.first, *name_and_dump.second);
            value->EndDictionary(); // "heaps"
        }

        value->BeginArray("allocators_graph");
        for (const MemoryAllocatorDumpEdge& edge : allocator_dumps_edges_) {
            value->BeginDictionary();
            value->SetString("source", edge.source.ToString());
            value->SetString("target", edge.target.ToString());
            value->SetInteger("importance", edge.importance);
            value->SetString("type", edge.type);
            value->EndDictionary();
        }
        value->EndArray();
    }

    void ProcessMemoryDump::AddOwnershipEdge(const MemoryAllocatorDumpGuid& source,
        const MemoryAllocatorDumpGuid& target,
        int importance)
    {
        allocator_dumps_edges_.push_back(
            { source, target, importance, kEdgeTypeOwnership });
    }

    void ProcessMemoryDump::AddOwnershipEdge(
        const MemoryAllocatorDumpGuid& source,
        const MemoryAllocatorDumpGuid& target)
    {
        AddOwnershipEdge(source, target, 0 /* importance */);
    }

    void ProcessMemoryDump::AddSuballocation(const MemoryAllocatorDumpGuid& source,
        const std::string& target_node_name)
    {
        std::string child_mad_name = target_node_name + "/__" + source.ToString();
        MemoryAllocatorDump* target_child_mad = CreateAllocatorDump(child_mad_name);
        AddOwnershipEdge(source, target_child_mad->guid());
    }

} // namespace trace_event
} // namespace base
