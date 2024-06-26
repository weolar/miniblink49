// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_PROCESS_MEMORY_DUMP_H_
#define BASE_TRACE_EVENT_PROCESS_MEMORY_DUMP_H_

#include <vector>

#include "base/base_export.h"
#include "base/containers/hash_tables.h"
#include "base/containers/small_map.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_vector.h"
#include "base/trace_event/memory_allocator_dump.h"
#include "base/trace_event/memory_allocator_dump_guid.h"
#include "base/trace_event/memory_dump_session_state.h"
#include "base/trace_event/process_memory_maps.h"
#include "base/trace_event/process_memory_totals.h"

// Define COUNT_RESIDENT_BYTES_SUPPORTED if platform supports counting of the
// resident memory.
// TODO(crbug.com/542671): COUNT_RESIDENT_BYTES_SUPPORTED is disabled on iOS
// as it cause memory corruption on iOS 9.0+ devices.
#if defined(OS_POSIX) && !defined(OS_NACL) && !defined(OS_IOS)
#define COUNT_RESIDENT_BYTES_SUPPORTED
#endif

namespace base {
namespace trace_event {

    class ConvertableToTraceFormat;
    class MemoryDumpManager;
    class MemoryDumpSessionState;
    class TracedValue;

    // ProcessMemoryDump is as a strongly typed container which holds the dumps
    // produced by the MemoryDumpProvider(s) for a specific process.
    class BASE_EXPORT ProcessMemoryDump {
    public:
        struct MemoryAllocatorDumpEdge {
            MemoryAllocatorDumpGuid source;
            MemoryAllocatorDumpGuid target;
            int importance;
            const char* type;
        };

        // Maps allocator dumps absolute names (allocator_name/heap/subheap) to
        // MemoryAllocatorDump instances.
        using AllocatorDumpsMap = SmallMap<hash_map<std::string, MemoryAllocatorDump*>>;

        using HeapDumpsMap = SmallMap<hash_map<std::string, scoped_refptr<TracedValue>>>;

#if defined(COUNT_RESIDENT_BYTES_SUPPORTED)
        // Returns the total bytes resident for a virtual address range, with given
        // |start_address| and |mapped_size|. |mapped_size| is specified in bytes. The
        // value returned is valid only if the given range is currently mmapped by the
        // process. The |start_address| must be page-aligned.
        static size_t CountResidentBytes(void* start_address, size_t mapped_size);
#endif

        ProcessMemoryDump(const scoped_refptr<MemoryDumpSessionState>& session_state);
        ~ProcessMemoryDump();

        // Creates a new MemoryAllocatorDump with the given name and returns the
        // empty object back to the caller.
        // Arguments:
        //   absolute_name: a name that uniquely identifies allocator dumps produced
        //       by this provider. It is possible to specify nesting by using a
        //       path-like string (e.g., v8/isolate1/heap1, v8/isolate1/heap2).
        //       Leading or trailing slashes are not allowed.
        //   guid: an optional identifier, unique among all processes within the
        //       scope of a global dump. This is only relevant when using
        //       AddOwnershipEdge() to express memory sharing. If omitted,
        //       it will be automatically generated.
        // ProcessMemoryDump handles the memory ownership of its MemoryAllocatorDumps.
        MemoryAllocatorDump* CreateAllocatorDump(const std::string& absolute_name);
        MemoryAllocatorDump* CreateAllocatorDump(const std::string& absolute_name,
            const MemoryAllocatorDumpGuid& guid);

        // Looks up a MemoryAllocatorDump given its allocator and heap names, or
        // nullptr if not found.
        MemoryAllocatorDump* GetAllocatorDump(const std::string& absolute_name) const;

        MemoryAllocatorDump* GetOrCreateAllocatorDump(
            const std::string& absolute_name);

        // Creates a shared MemoryAllocatorDump, to express cross-process sharing.
        // Shared allocator dumps are allowed to have duplicate guids within the
        // global scope, in order to reference the same dump from multiple processes.
        // See the design doc goo.gl/keU6Bf for reference usage patterns.
        MemoryAllocatorDump* CreateSharedGlobalAllocatorDump(
            const MemoryAllocatorDumpGuid& guid);

        // Looks up a shared MemoryAllocatorDump given its guid.
        MemoryAllocatorDump* GetSharedGlobalAllocatorDump(
            const MemoryAllocatorDumpGuid& guid) const;

        // Returns the map of the MemoryAllocatorDumps added to this dump.
        const AllocatorDumpsMap& allocator_dumps() const { return allocator_dumps_; }

        // Adds a heap dump for the allocator with |absolute_name|. The |TracedValue|
        // must have the correct format. |trace_event::HeapDumper| will generate such
        // a value from a |trace_event::AllocationRegister|.
        void AddHeapDump(const std::string& absolute_name,
            scoped_refptr<TracedValue> heap_dump);

        // Adds an ownership relationship between two MemoryAllocatorDump(s) with the
        // semantics: |source| owns |target|, and has the effect of attributing
        // the memory usage of |target| to |source|. |importance| is optional and
        // relevant only for the cases of co-ownership, where it acts as a z-index:
        // the owner with the highest importance will be attributed |target|'s memory.
        void AddOwnershipEdge(const MemoryAllocatorDumpGuid& source,
            const MemoryAllocatorDumpGuid& target,
            int importance);
        void AddOwnershipEdge(const MemoryAllocatorDumpGuid& source,
            const MemoryAllocatorDumpGuid& target);

        const std::vector<MemoryAllocatorDumpEdge>& allocator_dumps_edges() const
        {
            return allocator_dumps_edges_;
        }

        // Utility method to add a suballocation relationship with the following
        // semantics: |source| is suballocated from |target_node_name|.
        // This creates a child node of |target_node_name| and adds an ownership edge
        // between |source| and the new child node. As a result, the UI will not
        // account the memory of |source| in the target node.
        void AddSuballocation(const MemoryAllocatorDumpGuid& source,
            const std::string& target_node_name);

        const scoped_refptr<MemoryDumpSessionState>& session_state() const
        {
            return session_state_;
        }

        // Removes all the MemoryAllocatorDump(s) contained in this instance. This
        // ProcessMemoryDump can be safely reused as if it was new once this returns.
        void Clear();

        // Merges all MemoryAllocatorDump(s) contained in |other| inside this
        // ProcessMemoryDump, transferring their ownership to this instance.
        // |other| will be an empty ProcessMemoryDump after this method returns.
        // This is to allow dump providers to pre-populate ProcessMemoryDump instances
        // and later move their contents into the ProcessMemoryDump passed as argument
        // of the MemoryDumpProvider::OnMemoryDump(ProcessMemoryDump*) callback.
        void TakeAllDumpsFrom(ProcessMemoryDump* other);

        // Called at trace generation time to populate the TracedValue.
        void AsValueInto(TracedValue* value) const;

        ProcessMemoryTotals* process_totals() { return &process_totals_; }
        bool has_process_totals() const { return has_process_totals_; }
        void set_has_process_totals() { has_process_totals_ = true; }

        ProcessMemoryMaps* process_mmaps() { return &process_mmaps_; }
        bool has_process_mmaps() const { return has_process_mmaps_; }
        void set_has_process_mmaps() { has_process_mmaps_ = true; }

    private:
        void AddAllocatorDumpInternal(MemoryAllocatorDump* mad);

        ProcessMemoryTotals process_totals_;
        bool has_process_totals_;

        ProcessMemoryMaps process_mmaps_;
        bool has_process_mmaps_;

        AllocatorDumpsMap allocator_dumps_;
        HeapDumpsMap heap_dumps_;

        // ProcessMemoryDump handles the memory ownership of all its belongings.
        ScopedVector<MemoryAllocatorDump> allocator_dumps_storage_;

        // State shared among all PMDs instances created in a given trace session.
        scoped_refptr<MemoryDumpSessionState> session_state_;

        // Keeps track of relationships between MemoryAllocatorDump(s).
        std::vector<MemoryAllocatorDumpEdge> allocator_dumps_edges_;

        DISALLOW_COPY_AND_ASSIGN(ProcessMemoryDump);
    };

} // namespace trace_event
} // namespace base

#endif // BASE_TRACE_EVENT_PROCESS_MEMORY_DUMP_H_
