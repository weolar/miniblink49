// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/memory_profiler_allocation_register.h"

#include "base/trace_event/trace_event_memory_overhead.h"

namespace base {
namespace trace_event {

    AllocationRegister::AllocationRegister()
        // Reserve enough address space to store |kNumCells| entries if necessary,
        // with a guard page after it to crash the program when attempting to store
        // more entries.
        : cells_(static_cast<Cell*>(AllocateVirtualMemory(kNumCells * sizeof(Cell))))
        , buckets_(static_cast<CellIndex*>(
              AllocateVirtualMemory(kNumBuckets * sizeof(CellIndex))))
        ,

        // The free list is empty. The first unused cell is cell 1, because index
        // 0 is used as list terminator.
        free_list_(0)
        , next_unused_cell_(1)
    {
    }

    AllocationRegister::~AllocationRegister()
    {
        FreeVirtualMemory(buckets_, kNumBuckets * sizeof(CellIndex));
        FreeVirtualMemory(cells_, kNumCells * sizeof(Cell));
    }

    void AllocationRegister::Insert(void* address,
        size_t size,
        AllocationContext context)
    {
        DCHECK(address != nullptr);

        CellIndex* idx_ptr = Lookup(address);

        // If the index is 0, the address is not yet present, so insert it.
        if (*idx_ptr == 0) {
            *idx_ptr = GetFreeCell();

            cells_[*idx_ptr].allocation.address = address;
            cells_[*idx_ptr].next = 0;
        }

        cells_[*idx_ptr].allocation.size = size;
        cells_[*idx_ptr].allocation.context = context;
    }

    void AllocationRegister::Remove(void* address)
    {
        // Get a pointer to the index of the cell that stores |address|. The index can
        // be an element of |buckets_| or the |next| member of a cell.
        CellIndex* idx_ptr = Lookup(address);
        CellIndex freed_idx = *idx_ptr;

        // If the index is 0, the address was not there in the first place.
        if (freed_idx == 0)
            return;

        // The cell at the index is now free, remove it from the linked list for
        // |Hash(address)|.
        Cell* freed_cell = &cells_[freed_idx];
        *idx_ptr = freed_cell->next;

        // Put the free cell at the front of the free list.
        freed_cell->next = free_list_;
        free_list_ = freed_idx;

        // Reset the address, so that on iteration the free cell is ignored.
        freed_cell->allocation.address = nullptr;
    }

    AllocationRegister::ConstIterator AllocationRegister::begin() const
    {
        // Initialize the iterator's index to 0. Cell 0 never stores an entry.
        ConstIterator iterator(*this, 0);
        // Incrementing will advance the iterator to the first used cell.
        ++iterator;
        return iterator;
    }

    AllocationRegister::ConstIterator AllocationRegister::end() const
    {
        // Cell |next_unused_cell_ - 1| is the last cell that could contain an entry,
        // so index |next_unused_cell_| is an iterator past the last element, in line
        // with the STL iterator conventions.
        return ConstIterator(*this, next_unused_cell_);
    }

    AllocationRegister::ConstIterator::ConstIterator(
        const AllocationRegister& alloc_register,
        CellIndex index)
        : register_(alloc_register)
        , index_(index)
    {
    }

    void AllocationRegister::ConstIterator::operator++()
    {
        // Find the next cell with a non-null address until all cells that could
        // possibly be used have been iterated. A null address indicates a free cell.
        do {
            index_++;
        } while (index_ < register_.next_unused_cell_ && register_.cells_[index_].allocation.address == nullptr);
    }

    bool AllocationRegister::ConstIterator::operator!=(
        const ConstIterator& other) const
    {
        return index_ != other.index_;
    }

    const AllocationRegister::Allocation& AllocationRegister::ConstIterator::
    operator*() const
    {
        return register_.cells_[index_].allocation;
    }

    AllocationRegister::CellIndex* AllocationRegister::Lookup(void* address)
    {
        // The list head is in |buckets_| at the hash offset.
        CellIndex* idx_ptr = &buckets_[Hash(address)];

        // Chase down the list until the cell that holds |key| is found,
        // or until the list ends.
        while (*idx_ptr != 0 && cells_[*idx_ptr].allocation.address != address)
            idx_ptr = &cells_[*idx_ptr].next;

        return idx_ptr;
    }

    AllocationRegister::CellIndex AllocationRegister::GetFreeCell()
    {
        // First try to re-use a cell from the freelist.
        if (free_list_) {
            CellIndex idx = free_list_;
            free_list_ = cells_[idx].next;
            return idx;
        }

        // Otherwise pick the next cell that has not been touched before.
        CellIndex idx = next_unused_cell_;
        next_unused_cell_++;

        // If the hash table has too little capacity (when too little address space
        // was reserved for |cells_|), |next_unused_cell_| can be an index outside of
        // the allocated storage. A guard page is allocated there to crash the
        // program in that case. There are alternative solutions:
        // - Deal with it, increase capacity by reallocating |cells_|.
        // - Refuse to insert and let the caller deal with it.
        // Because free cells are re-used before accessing fresh cells with a higher
        // index, and because reserving address space without touching it is cheap,
        // the simplest solution is to just allocate a humongous chunk of address
        // space.

        DCHECK_LT(next_unused_cell_, kNumCells + 1);

        return idx;
    }

    // static
    uint32_t AllocationRegister::Hash(void* address)
    {
        // The multiplicative hashing scheme from [Knuth 1998]. The value of |a| has
        // been chosen carefully based on measurements with real-word data (addresses
        // recorded from a Chrome trace run). It is the first prime after 2^17. For
        // |shift|, 13, 14 and 15 yield good results. These values are tuned to 2^18
        // buckets. Microbenchmarks show that this simple scheme outperforms fancy
        // hashes like Murmur3 by 20 to 40 percent.
        const uintptr_t key = reinterpret_cast<uintptr_t>(address);
        const uintptr_t a = 131101;
        const uintptr_t shift = 14;
        const uintptr_t h = (key * a) >> shift;
        return static_cast<uint32_t>(h) & kNumBucketsMask;
    }

    void AllocationRegister::EstimateTraceMemoryOverhead(
        TraceEventMemoryOverhead* overhead) const
    {
        // Estimate memory overhead by counting all of the cells that have ever been
        // touched. Don't report mmapped memory as allocated, because it has not been
        // allocated by malloc.
        size_t allocated = sizeof(AllocationRegister);
        size_t resident = sizeof(AllocationRegister)
            // Include size of touched cells (size of |*cells_|).
            + sizeof(Cell) * next_unused_cell_
            // Size of |*buckets_|.
            + sizeof(CellIndex) * kNumBuckets;
        overhead->Add("AllocationRegister", allocated, resident);
    }

} // namespace trace_event
} // namespace base
