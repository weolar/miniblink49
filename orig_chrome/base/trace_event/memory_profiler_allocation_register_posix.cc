// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/memory_profiler_allocation_register.h"

#include <sys/mman.h>
#include <unistd.h>

#include "base/basictypes.h"
#include "base/bits.h"
#include "base/logging.h"
#include "base/process/process_metrics.h"

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

namespace base {
namespace trace_event {

    namespace {
        size_t GetGuardSize()
        {
            return GetPageSize();
        }
    }

    // static
    void* AllocationRegister::AllocateVirtualMemory(size_t size)
    {
        size = bits::Align(size, GetPageSize());

        // Add space for a guard page at the end.
        size_t map_size = size + GetGuardSize();

        void* addr = mmap(nullptr, map_size, PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        PCHECK(addr != MAP_FAILED);

        // Mark the last page of the allocated address space as inaccessible
        // (PROT_NONE). The read/write accessible space is still at least |min_size|
        // bytes.
        void* guard_addr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) + size);
        int result = mprotect(guard_addr, GetGuardSize(), PROT_NONE);
        PCHECK(result == 0);

        return addr;
    }

    // static
    void AllocationRegister::FreeVirtualMemory(void* address,
        size_t allocated_size)
    {
        size_t size = bits::Align(allocated_size, GetPageSize()) + GetGuardSize();
        munmap(address, size);
    }

} // namespace trace_event
} // namespace base
