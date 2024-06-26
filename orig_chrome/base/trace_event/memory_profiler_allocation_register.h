// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_MEMORY_PROFILER_ALLOCATION_REGISTER_H_
#define BASE_TRACE_EVENT_MEMORY_PROFILER_ALLOCATION_REGISTER_H_

#include <stdint.h>

#include "base/logging.h"
#include "base/trace_event/memory_profiler_allocation_context.h"

namespace base {
namespace trace_event {

    class TraceEventMemoryOverhead;

    // The allocation register keeps track of all allocations that have not been
    // freed. It is a memory map-backed hash table that stores size and context
    // indexed by address. The hash table is tailored specifically for this use
    // case. The common case is that an entry is inserted and removed after a
    // while, lookup without modifying the table is not an intended use case. The
    // hash table is implemented as an array of linked lists. The size of this
    // array is fixed, but it does not limit the amount of entries that can be
    // stored.
    //
    // Replaying a recording of Chrome's allocations and frees against this hash
    // table takes about 15% of the time that it takes to replay them against
    // |std::map|.
    class BASE_EXPORT AllocationRegister {
    public:
        // The data stored in the hash table;
        // contains the details about an allocation.
        struct Allocation {
            void* address;
            size_t size;
            AllocationContext context;
        };

        // An iterator that iterates entries in the hash table efficiently, but in no
        // particular order. It can do this by iterating the cells and ignoring the
        // linked lists altogether. Instead of checking whether a cell is in the free
        // list to see if it should be skipped, a null address is used to indicate
        // that a cell is free.
        class BASE_EXPORT ConstIterator {
        public:
            void operator++();
            bool operator!=(const ConstIterator& other) const;
            const Allocation& operator*() const;

        private:
            friend class AllocationRegister;
            using CellIndex = uint32_t;

            ConstIterator(const AllocationRegister& alloc_register, CellIndex index);

            const AllocationRegister& register_;
            CellIndex index_;
        };

        AllocationRegister();
        ~AllocationRegister();

        // Inserts allocation details into the table. If the address was present
        // already, its details are updated. |address| must not be null. (This is
        // because null is used to mark free cells, to allow efficient iteration of
        // the hash table.)
        void Insert(void* address, size_t size, AllocationContext context);

        // Removes the address from the table if it is present. It is ok to call this
        // with a null pointer.
        void Remove(void* address);

        ConstIterator begin() const;
        ConstIterator end() const;

        // Estimates memory overhead including |sizeof(AllocationRegister)|.
        void EstimateTraceMemoryOverhead(TraceEventMemoryOverhead* overhead) const;

    private:
        friend class AllocationRegisterTest;
        using CellIndex = uint32_t;

        // A cell can store allocation details (size and context) by address. Cells
        // are part of a linked list via the |next| member. This list is either the
        // list for a particular hash, or the free list. All cells are contiguous in
        // memory in one big array. Therefore, on 64-bit systems, space can be saved
        // by storing 32-bit indices instead of pointers as links. Index 0 is used as
        // the list terminator.
        struct Cell {
            CellIndex next;
            Allocation allocation;
        };

        // The number of buckets, 2^18, approximately 260 000, has been tuned for
        // Chrome's typical number of outstanding allocations. (This number varies
        // between processes. Most processes have a sustained load of ~30k unfreed
        // allocations, but some processes have peeks around 100k-400k allocations.)
        // Because of the size of the table, it is likely that every |buckets_|
        // access and every |cells_| access will incur a cache miss. Microbenchmarks
        // suggest that it is worthwile to use more memory for the table to avoid
        // chasing down the linked list, until the size is 2^18. The number of buckets
        // is a power of two so modular indexing can be done with bitwise and.
        static const uint32_t kNumBuckets = 0x40000;
        static const uint32_t kNumBucketsMask = kNumBuckets - 1;

        // Reserve address space to store at most this number of entries. High
        // capacity does not imply high memory usage due to the access pattern. The
        // only constraint on the number of cells is that on 32-bit systems address
        // space is scarce (i.e. reserving 2GiB of address space for the entries is
        // not an option). A value of ~3M entries is large enough to handle spikes in
        // the number of allocations, and modest enough to require no more than a few
        // dozens of MiB of address space.
        static const uint32_t kNumCells = kNumBuckets * 10;

        // Returns a value in the range [0, kNumBuckets - 1] (inclusive).
        static uint32_t Hash(void* address);

        // Allocates a region of virtual address space of |min_size| rounded up to the
        // system page size. The memory is zeroed by the system. A guard page is added
        // after the end.
        static void* AllocateVirtualMemory(size_t size);

        // Frees a region of virtual address space allocated by a call to
        // |AllocateVirtualMemory|.
        static void FreeVirtualMemory(void* address, size_t allocated_size);

        // Returns a pointer to the variable that contains or should contain the
        // index of the cell that stores the entry for |address|. The pointer may
        // point at an element of |buckets_| or at the |next| member of an element of
        // |cells_|. If the value pointed at is 0, |address| is not in the table.
        CellIndex* Lookup(void* address);

        // Takes a cell that is not being used to store an entry (either by recycling
        // from the free list or by taking a fresh cell) and returns its index.
        CellIndex GetFreeCell();

        // The array of cells. This array is backed by mmapped memory. Lower indices
        // are accessed first, higher indices are only accessed when required. In
        // this way, even if a huge amount of address space has been mmapped, only
        // the cells that are actually used will be backed by physical memory.
        Cell* const cells_;

        // The array of indices into |cells_|. |buckets_[Hash(address)]| will contain
        // the index of the head of the linked list for |Hash(key)|. A value of 0
        // indicates an empty list. This array is backed by mmapped memory.
        CellIndex* const buckets_;

        // The head of the free list. This is the index of the cell. A value of 0
        // means that the free list is empty.
        CellIndex free_list_;

        // The index of the first element of |cells_| that has not been used before.
        // If the free list is empty and a new cell is needed, the cell at this index
        // is used. This is the high water mark for the number of entries stored.
        CellIndex next_unused_cell_;

        DISALLOW_COPY_AND_ASSIGN(AllocationRegister);
    };

} // namespace trace_event
} // namespace base

#endif // BASE_TRACE_EVENT_MEMORY_PROFILER_ALLOCATION_REGISTER_H_
