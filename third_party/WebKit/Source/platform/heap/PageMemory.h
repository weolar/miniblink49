// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PageMemory_h
#define PageMemory_h

#include "wtf/Assertions.h"
#include "wtf/PageAllocator.h"

#if OS(POSIX)
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace blink {

static size_t roundToOsPageSize(size_t size)
{
    return (size + WTF::kSystemPageSize - 1) & ~(WTF::kSystemPageSize - 1);
}

class MemoryRegion {
public:
    MemoryRegion(Address base, size_t size)
        : m_base(base)
        , m_size(size)
    {
        ASSERT(size > 0);
    }

    bool contains(Address addr) const
    {
        return m_base <= addr && addr < (m_base + m_size);
    }

    bool contains(const MemoryRegion& other) const
    {
        return contains(other.m_base) && contains(other.m_base + other.m_size - 1);
    }

    void release()
    {
        WTF::freePages(m_base, m_size);
    }

    WARN_UNUSED_RETURN bool commit()
    {
        WTF::recommitSystemPages(m_base, m_size);
        return WTF::setSystemPagesAccessible(m_base, m_size);
    }

    void decommit()
    {
        WTF::decommitSystemPages(m_base, m_size);
        WTF::setSystemPagesInaccessible(m_base, m_size);
    }

    Address base() const { return m_base; }
    size_t size() const { return m_size; }

private:
    Address m_base;
    size_t m_size;
};

// TODO(haraken): Like partitionOutOfMemoryWithLotsOfUncommitedPages(),
// we should probably have a way to distinguish physical memory OOM from
// virtual address space OOM.
static NEVER_INLINE void blinkGCOutOfMemory()
{
    IMMEDIATE_CRASH();
}

// A PageMemoryRegion represents a chunk of reserved virtual address
// space containing a number of blink heap pages. On Windows, reserved
// virtual address space can only be given back to the system as a
// whole. The PageMemoryRegion allows us to do that by keeping track
// of the number of pages using it in order to be able to release all
// of the virtual address space when there are no more pages using it.
class PageMemoryRegion : public MemoryRegion {
public:
    ~PageMemoryRegion()
    {
        release();
    }

    void pageDeleted(Address page)
    {
        markPageUnused(page);
        if (!--m_numPages) {
            Heap::removePageMemoryRegion(this);
            delete this;
        }
    }

    void markPageUsed(Address page)
    {
        ASSERT(!m_inUse[index(page)]);
        m_inUse[index(page)] = true;
    }

    void markPageUnused(Address page)
    {
        m_inUse[index(page)] = false;
    }

    static PageMemoryRegion* allocateLargePage(size_t size)
    {
        return allocate(size, 1);
    }

    static PageMemoryRegion* allocateNormalPages()
    {
        return allocate(blinkPageSize * blinkPagesPerRegion, blinkPagesPerRegion);
    }

    BasePage* pageFromAddress(Address address)
    {
        ASSERT(contains(address));
        if (!m_inUse[index(address)])
            return nullptr;
        if (m_isLargePage)
            return pageFromObject(base());
        return pageFromObject(address);
    }

private:
    PageMemoryRegion(Address base, size_t size, unsigned numPages)
        : MemoryRegion(base, size)
        , m_isLargePage(numPages == 1)
        , m_numPages(numPages)
    {
        for (size_t i = 0; i < blinkPagesPerRegion; ++i)
            m_inUse[i] = false;
    }

    unsigned index(Address address)
    {
        ASSERT(contains(address));
        if (m_isLargePage)
            return 0;
        size_t offset = blinkPageAddress(address) - base();
        ASSERT(offset % blinkPageSize == 0);
        return offset / blinkPageSize;
    }

    static PageMemoryRegion* allocate(size_t size, unsigned numPages)
    {
        // Round size up to the allocation granularity.
        size = (size + WTF::kPageAllocationGranularityOffsetMask) & WTF::kPageAllocationGranularityBaseMask;
        Address base = static_cast<Address>(WTF::allocPages(nullptr, size, blinkPageSize, WTF::PageInaccessible));
        if (!base)
            blinkGCOutOfMemory();
        return new PageMemoryRegion(base, size, numPages);
    }

    bool m_isLargePage;
    bool m_inUse[blinkPagesPerRegion];
    unsigned m_numPages;
};

// Representation of the memory used for a Blink heap page.
//
// The representation keeps track of two memory regions:
//
// 1. The virtual memory reserved from the system in order to be able
//    to free all the virtual memory reserved.  Multiple PageMemory
//    instances can share the same reserved memory region and
//    therefore notify the reserved memory region on destruction so
//    that the system memory can be given back when all PageMemory
//    instances for that memory are gone.
//
// 2. The writable memory (a sub-region of the reserved virtual
//    memory region) that is used for the actual heap page payload.
//
// Guard pages are created before and after the writable memory.
class PageMemory {
public:
    ~PageMemory()
    {
        __lsan_unregister_root_region(m_writable.base(), m_writable.size());
        m_reserved->pageDeleted(writableStart());
    }

    WARN_UNUSED_RETURN bool commit()
    {
        m_reserved->markPageUsed(writableStart());
        return m_writable.commit();
    }

    void decommit()
    {
        m_reserved->markPageUnused(writableStart());
        m_writable.decommit();
    }

    void markUnused() { m_reserved->markPageUnused(writableStart()); }

    PageMemoryRegion* region() { return m_reserved; }

    Address writableStart() { return m_writable.base(); }

    static PageMemory* setupPageMemoryInRegion(PageMemoryRegion* region, size_t pageOffset, size_t payloadSize)
    {
        // Setup the payload one OS page into the page memory. The
        // first os page is the guard page.
        Address payloadAddress = region->base() + pageOffset + WTF::kSystemPageSize;
        return new PageMemory(region, MemoryRegion(payloadAddress, payloadSize));
    }

    // Allocate a virtual address space for one blink page with the
    // following layout:
    //
    //    [ guard os page | ... payload ... | guard os page ]
    //    ^---{ aligned to blink page size }
    //
    // The returned page memory region will be zeroed.
    //
    static PageMemory* allocate(size_t payloadSize)
    {
        ASSERT(payloadSize > 0);

        // Virtual memory allocation routines operate in OS page sizes.
        // Round up the requested size to nearest os page size.
        payloadSize = roundToOsPageSize(payloadSize);

        // Overallocate by 2 times OS page size to have space for a
        // guard page at the beginning and end of blink heap page.
        size_t allocationSize = payloadSize + 2 * WTF::kSystemPageSize;
        PageMemoryRegion* pageMemoryRegion = PageMemoryRegion::allocateLargePage(allocationSize);
        PageMemory* storage = setupPageMemoryInRegion(pageMemoryRegion, 0, payloadSize);
        RELEASE_ASSERT(storage->commit());
        return storage;
    }

private:
    PageMemory(PageMemoryRegion* reserved, const MemoryRegion& writable)
        : m_reserved(reserved)
        , m_writable(writable)
    {
        ASSERT(reserved->contains(writable));

        // Register the writable area of the memory as part of the LSan root set.
        // Only the writable area is mapped and can contain C++ objects.  Those
        // C++ objects can contain pointers to objects outside of the heap and
        // should therefore be part of the LSan root set.
        __lsan_register_root_region(m_writable.base(), m_writable.size());
    }


    PageMemoryRegion* m_reserved;
    MemoryRegion m_writable;
};

} // namespace blink

#endif
