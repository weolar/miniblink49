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

#ifndef Heap_h
#define Heap_h

#include "platform/PlatformExport.h"
#include "platform/heap/GCInfo.h"
#include "platform/heap/ThreadState.h"
#include "platform/heap/Visitor.h"
#include "public/platform/WebThread.h"
#include "wtf/AddressSanitizer.h"
#include "wtf/Assertions.h"
#include "wtf/Atomics.h"
#include "wtf/ContainerAnnotations.h"
#include "wtf/Forward.h"
#include "wtf/PageAllocator.h"
#include <stdint.h>

namespace blink {

const size_t blinkPageSizeLog2 = 17;
const size_t blinkPageSize = 1 << blinkPageSizeLog2;
const size_t blinkPageOffsetMask = blinkPageSize - 1;
const size_t blinkPageBaseMask = ~blinkPageOffsetMask;

// We allocate pages at random addresses but in groups of
// blinkPagesPerRegion at a given random address. We group pages to
// not spread out too much over the address space which would blow
// away the page tables and lead to bad performance.
const size_t blinkPagesPerRegion = 10;

// Double precision floats are more efficient when 8 byte aligned, so we 8 byte
// align all allocations even on 32 bit.
const size_t allocationGranularity = 8;
const size_t allocationMask = allocationGranularity - 1;
const size_t objectStartBitMapSize = (blinkPageSize + ((8 * allocationGranularity) - 1)) / (8 * allocationGranularity);
const size_t reservedForObjectBitMap = ((objectStartBitMapSize + allocationMask) & ~allocationMask);
const size_t maxHeapObjectSizeLog2 = 27;
const size_t maxHeapObjectSize = 1 << maxHeapObjectSizeLog2;
const size_t largeObjectSizeThreshold = blinkPageSize / 2;

// A zap value used for freed memory that is allowed to be added to the free
// list in the next addToFreeList().
const uint8_t reuseAllowedZapValue = 0x2a;
// A zap value used for freed memory that is forbidden to be added to the free
// list in the next addToFreeList().
const uint8_t reuseForbiddenZapValue = 0x2c;

// In non-production builds, memory is zapped when it's freed. The zapped
// memory is zeroed out when the memory is reused in Heap::allocateObject().
// In production builds, memory is not zapped (for performance). The memory
// is just zeroed out when it is added to the free list.
#if ENABLE(ASSERT) || defined(LEAK_SANITIZER) || defined(ADDRESS_SANITIZER)
#define FILL_ZERO_IF_PRODUCTION(address, size) FreeList::zapFreedMemory(address, size)
#define FILL_ZERO_IF_NOT_PRODUCTION(address, size) memset((address), 0, (size))
#else
#define FILL_ZERO_IF_PRODUCTION(address, size) memset((address), 0, (size))
#define FILL_ZERO_IF_NOT_PRODUCTION(address, size) do { } while (false)
#endif

class CallbackStack;
class FreePagePool;
class NormalPageHeap;
class OrphanedPagePool;
class PageMemory;
class WebProcessMemoryDump;

#if ENABLE(GC_PROFILING)
class TracedValue;
#endif

// HeapObjectHeader is 4 byte (32 bit) that has the following layout:
//
// | gcInfoIndex (14 bit) | DOM mark bit (1 bit) | size (14 bit) | dead bit (1 bit) | freed bit (1 bit) | mark bit (1 bit) |
//
// - For non-large objects, 14 bit is enough for |size| because the blink
//   page size is 2^17 byte and each object is guaranteed to be aligned with
//   2^3 byte.
// - For large objects, |size| is 0. The actual size of a large object is
//   stored in LargeObjectPage::m_payloadSize.
// - 1 bit used to mark DOM trees for V8.
// - 14 bit is enough for gcInfoIndex because there are less than 2^14 types
//   in Blink.
const size_t headerDOMMarkBitMask = 1u << 17;
const size_t headerGCInfoIndexShift = 18;
const size_t headerGCInfoIndexMask = (static_cast<size_t>((1 << 14) - 1)) << headerGCInfoIndexShift;
const size_t headerSizeMask = (static_cast<size_t>((1 << 14) - 1)) << 3;
const size_t headerMarkBitMask = 1;
const size_t headerFreedBitMask = 2;
// The dead bit is used for objects that have gone through a GC marking, but did
// not get swept before a new GC started. In that case we set the dead bit on
// objects that were not marked in the previous GC to ensure we are not tracing
// them via a conservatively found pointer. Tracing dead objects could lead to
// tracing of already finalized objects in another thread's heap which is a
// use-after-free situation.
const size_t headerDeadBitMask = 4;
// On free-list entries we reuse the dead bit to distinguish a normal free-list
// entry from one that has been promptly freed.
const size_t headerPromptlyFreedBitMask = headerFreedBitMask | headerDeadBitMask;
const size_t largeObjectSizeInHeader = 0;
const size_t gcInfoIndexForFreeListHeader = 0;
const size_t nonLargeObjectPageSizeMax = 1 << 17;

static_assert(nonLargeObjectPageSizeMax >= blinkPageSize, "max size supported by HeapObjectHeader must at least be blinkPageSize");

class PLATFORM_EXPORT HeapObjectHeader {
public:
    // If gcInfoIndex is 0, this header is interpreted as a free list header.
    NO_SANITIZE_ADDRESS
    HeapObjectHeader(size_t size, size_t gcInfoIndex)
    {
#if ENABLE(ASSERT)
        m_magic = magic;
#endif
#if ENABLE(GC_PROFILING)
        m_age = 0;
#endif
        // sizeof(HeapObjectHeader) must be equal to or smaller than
        // allocationGranurarity, because HeapObjectHeader is used as a header
        // for an freed entry.  Given that the smallest entry size is
        // allocationGranurarity, HeapObjectHeader must fit into the size.
        static_assert(sizeof(HeapObjectHeader) <= allocationGranularity, "size of HeapObjectHeader must be smaller than allocationGranularity");
#if CPU(64BIT)
        static_assert(sizeof(HeapObjectHeader) == 8, "size of HeapObjectHeader must be 8 byte aligned");
#endif

        ASSERT(gcInfoIndex < GCInfoTable::maxIndex);
        ASSERT(size < nonLargeObjectPageSizeMax);
        ASSERT(!(size & allocationMask));
        m_encoded = (gcInfoIndex << headerGCInfoIndexShift) | size | (gcInfoIndex ? 0 : headerFreedBitMask);
    }

    NO_SANITIZE_ADDRESS
    bool isFree() const { return m_encoded & headerFreedBitMask; }
    NO_SANITIZE_ADDRESS
    bool isPromptlyFreed() const { return (m_encoded & headerPromptlyFreedBitMask) == headerPromptlyFreedBitMask; }
    NO_SANITIZE_ADDRESS
    void markPromptlyFreed() { m_encoded |= headerPromptlyFreedBitMask; }
    size_t size() const;

    NO_SANITIZE_ADDRESS
    size_t gcInfoIndex() const { return (m_encoded & headerGCInfoIndexMask) >> headerGCInfoIndexShift; }
    NO_SANITIZE_ADDRESS
    void setSize(size_t size) { m_encoded = size | (m_encoded & ~headerSizeMask); }
    bool isMarked() const;
    void mark();
    void unmark();
    void markDead();
    bool isDead() const;

    Address payload();
    size_t payloadSize();
    Address payloadEnd();

#if ENABLE(ASSERT)
    bool checkHeader() const;
    // Zap magic number with a new magic number that means there was once an
    // object allocated here, but it was freed because nobody marked it during
    // GC.
    void zapMagic();
#endif

    void finalize(Address, size_t);
    static HeapObjectHeader* fromPayload(const void*);

    static const uint16_t magic = 0xfff1;
    static const uint16_t zappedMagic = 0x4321;

#if ENABLE(GC_PROFILING)
    NO_SANITIZE_ADDRESS
    size_t encodedSize() const { return m_encoded; }

    NO_SANITIZE_ADDRESS
    size_t age() const { return m_age; }

    NO_SANITIZE_ADDRESS
    void incrementAge()
    {
        if (m_age < maxHeapObjectAge)
            m_age++;
    }
#endif

#if !ENABLE(ASSERT) && !ENABLE(GC_PROFILING) && CPU(64BIT)
    // This method is needed just to avoid compilers from removing m_padding.
    uint64_t unusedMethod() const { return m_padding; }
#endif

private:
    uint32_t m_encoded;
#if ENABLE(ASSERT)
    uint16_t m_magic;
#endif
#if ENABLE(GC_PROFILING)
    uint8_t m_age;
#endif

    // In 64 bit architectures, we intentionally add 4 byte padding immediately
    // after the HeapHeaderObject. This is because:
    //
    // | HeapHeaderObject (4 byte) | padding (4 byte) | object payload (8 * n byte) |
    // ^8 byte aligned                                ^8 byte aligned
    //
    // is better than:
    //
    // | HeapHeaderObject (4 byte) | object payload (8 * n byte) | padding (4 byte) |
    // ^4 byte aligned             ^8 byte aligned               ^4 byte aligned
    //
    // since the former layout aligns both header and payload to 8 byte.
#if !ENABLE(ASSERT) && !ENABLE(GC_PROFILING) && CPU(64BIT)
    uint32_t m_padding;
#endif
};

class FreeListEntry final : public HeapObjectHeader {
public:
    NO_SANITIZE_ADDRESS
    explicit FreeListEntry(size_t size)
        : HeapObjectHeader(size, gcInfoIndexForFreeListHeader)
        , m_next(nullptr)
    {
#if ENABLE(ASSERT)
        ASSERT(size >= sizeof(HeapObjectHeader));
        zapMagic();
#endif
    }

    Address address() { return reinterpret_cast<Address>(this); }

    NO_SANITIZE_ADDRESS
    void unlink(FreeListEntry** prevNext)
    {
        *prevNext = m_next;
        m_next = nullptr;
    }

    NO_SANITIZE_ADDRESS
    void link(FreeListEntry** prevNext)
    {
        m_next = *prevNext;
        *prevNext = this;
    }

    NO_SANITIZE_ADDRESS
    FreeListEntry* next() const { return m_next; }

    NO_SANITIZE_ADDRESS
    void append(FreeListEntry* next)
    {
        ASSERT(!m_next);
        m_next = next;
    }

private:
    FreeListEntry* m_next;
};

// Blink heap pages are set up with a guard page before and after the payload.
inline size_t blinkPagePayloadSize()
{
    return blinkPageSize - 2 * WTF::kSystemPageSize;
}

// Blink heap pages are aligned to the Blink heap page size.
// Therefore, the start of a Blink page can be obtained by
// rounding down to the Blink page size.
inline Address roundToBlinkPageStart(Address address)
{
    return reinterpret_cast<Address>(reinterpret_cast<uintptr_t>(address) & blinkPageBaseMask);
}

inline Address roundToBlinkPageEnd(Address address)
{
    return reinterpret_cast<Address>(reinterpret_cast<uintptr_t>(address - 1) & blinkPageBaseMask) + blinkPageSize;
}

// Masks an address down to the enclosing blink page base address.
inline Address blinkPageAddress(Address address)
{
    return reinterpret_cast<Address>(reinterpret_cast<uintptr_t>(address) & blinkPageBaseMask);
}

inline bool vTableInitialized(void* objectPointer)
{
    return !!(*reinterpret_cast<Address*>(objectPointer));
}

#if ENABLE(ASSERT)
// Sanity check for a page header address: the address of the page
// header should be OS page size away from being Blink page size
// aligned.
inline bool isPageHeaderAddress(Address address)
{
    return !((reinterpret_cast<uintptr_t>(address) & blinkPageOffsetMask) - WTF::kSystemPageSize);
}
#endif

// BasePage is a base class for NormalPage and LargeObjectPage.
//
// - NormalPage is a page whose size is |blinkPageSize|. NormalPage can contain
//   multiple objects in the page. An object whose size is smaller than
//   |largeObjectSizeThreshold| is stored in NormalPage.
//
// - LargeObjectPage is a page that contains only one object. The object size
//   is arbitrary. An object whose size is larger than |blinkPageSize| is stored
//   as a single project in LargeObjectPage.
//
// Note: An object whose size is between |largeObjectSizeThreshold| and
// |blinkPageSize| can go to either of NormalPage or LargeObjectPage.
class BasePage {
public:
    BasePage(PageMemory*, BaseHeap*);
    virtual ~BasePage() { }

    void link(BasePage** previousNext)
    {
        m_next = *previousNext;
        *previousNext = this;
    }
    void unlink(BasePage** previousNext)
    {
        *previousNext = m_next;
        m_next = nullptr;
    }
    BasePage* next() const { return m_next; }

    // virtual methods are slow. So performance-sensitive methods
    // should be defined as non-virtual methods on NormalPage and LargeObjectPage.
    // The following methods are not performance-sensitive.
    virtual size_t objectPayloadSizeForTesting() = 0;
    virtual bool isEmpty() = 0;
    virtual void removeFromHeap() = 0;
    virtual void sweep() = 0;
    virtual void makeConsistentForGC() = 0;
    virtual void makeConsistentForMutator() = 0;

#if defined(ADDRESS_SANITIZER)
    virtual void poisonObjects(ThreadState::ObjectsToPoison, ThreadState::Poisoning) = 0;
#endif
    // Check if the given address points to an object in this
    // heap page. If so, find the start of that object and mark it
    // using the given Visitor. Otherwise do nothing. The pointer must
    // be within the same aligned blinkPageSize as the this-pointer.
    //
    // This is used during conservative stack scanning to
    // conservatively mark all objects that could be referenced from
    // the stack.
    virtual void checkAndMarkPointer(Visitor*, Address) = 0;
    virtual void markOrphaned();

    virtual void takeSnapshot(String dumpBaseName, size_t pageIndex, size_t* outFreeSize, size_t* outFreeCount) = 0;
#if ENABLE(GC_PROFILING)
    virtual const GCInfo* findGCInfo(Address) = 0;
    virtual void snapshot(TracedValue*, ThreadState::SnapshotInfo*) = 0;
    virtual void incrementMarkedObjectsAge() = 0;
    virtual void countMarkedObjects(ClassAgeCountsMap&) = 0;
    virtual void countObjectsToSweep(ClassAgeCountsMap&) = 0;
#endif
#if ENABLE(ASSERT) || ENABLE(GC_PROFILING)
    virtual bool contains(Address) = 0;
#endif
    virtual size_t size() = 0;
    virtual bool isLargeObjectPage() { return false; }

    Address address() { return reinterpret_cast<Address>(this); }
    PageMemory* storage() const { return m_storage; }
    BaseHeap* heap() const { return m_heap; }
    bool orphaned() { return !m_heap; }
    bool terminating() { return m_terminating; }
    void setTerminating() { m_terminating = true; }

    // Returns true if this page has been swept by the ongoing lazy sweep.
    bool hasBeenSwept() const { return m_swept; }

    void markAsSwept()
    {
        ASSERT(!m_swept);
        m_swept = true;
    }

    void markAsUnswept()
    {
        ASSERT(m_swept);
        m_swept = false;
    }

private:
    PageMemory* m_storage;
    BaseHeap* m_heap;
    BasePage* m_next;
    // Whether the page is part of a terminating thread or not.
    bool m_terminating;

    // Track the sweeping state of a page. Set to true once
    // the lazy sweep completes has processed it.
    //
    // Set to false at the start of a sweep, true  upon completion
    // of lazy sweeping.
    bool m_swept;
    friend class BaseHeap;
};

class NormalPage final : public BasePage {
public:
    NormalPage(PageMemory*, BaseHeap*);

    Address payload()
    {
        return address() + pageHeaderSize();
    }
    size_t payloadSize()
    {
        return (blinkPagePayloadSize() - pageHeaderSize()) & ~allocationMask;
    }
    Address payloadEnd() { return payload() + payloadSize(); }
    bool containedInObjectPayload(Address address)
    {
        return payload() <= address && address < payloadEnd();
    }

    size_t objectPayloadSizeForTesting() override;
    bool isEmpty() override;
    void removeFromHeap() override;
    void sweep() override;
    void makeConsistentForGC() override;
    void makeConsistentForMutator() override;
#if defined(ADDRESS_SANITIZER)
    void poisonObjects(ThreadState::ObjectsToPoison, ThreadState::Poisoning) override;
#endif
    void checkAndMarkPointer(Visitor*, Address) override;
    void markOrphaned() override;

    void takeSnapshot(String dumpBaseName, size_t pageIndex, size_t* outFreeSize, size_t* outFreeCount) override;
#if ENABLE(GC_PROFILING)
    const GCInfo* findGCInfo(Address) override;
    void snapshot(TracedValue*, ThreadState::SnapshotInfo*) override;
    void incrementMarkedObjectsAge() override;
    void countMarkedObjects(ClassAgeCountsMap&) override;
    void countObjectsToSweep(ClassAgeCountsMap&) override;
#endif
#if ENABLE(ASSERT) || ENABLE(GC_PROFILING)
    // Returns true for the whole blinkPageSize page that the page is on, even
    // for the header, and the unmapped guard page at the start. That ensures
    // the result can be used to populate the negative page cache.
    bool contains(Address) override;
#endif
    size_t size() override { return blinkPageSize; }
    static size_t pageHeaderSize()
    {
        // Compute the amount of padding we have to add to a header to make
        // the size of the header plus the padding a multiple of 8 bytes.
        size_t paddingSize = (sizeof(NormalPage) + allocationGranularity - (sizeof(HeapObjectHeader) % allocationGranularity)) % allocationGranularity;
        return sizeof(NormalPage) + paddingSize;
    }


    NormalPageHeap* heapForNormalPage();
    void clearObjectStartBitMap();

private:
    HeapObjectHeader* findHeaderFromAddress(Address);
    void populateObjectStartBitMap();
    bool isObjectStartBitMapComputed() { return m_objectStartBitMapComputed; }

    bool m_objectStartBitMapComputed;
    uint8_t m_objectStartBitMap[reservedForObjectBitMap];
};

// Large allocations are allocated as separate objects and linked in a list.
//
// In order to use the same memory allocation routines for everything allocated
// in the heap, large objects are considered heap pages containing only one
// object.
class LargeObjectPage final : public BasePage {
public:
    LargeObjectPage(PageMemory*, BaseHeap*, size_t);

    Address payload() { return heapObjectHeader()->payload(); }
    size_t payloadSize() { return m_payloadSize; }
    Address payloadEnd() { return payload() + payloadSize(); }
    bool containedInObjectPayload(Address address)
    {
        return payload() <= address && address < payloadEnd();
    }

    size_t objectPayloadSizeForTesting() override;
    bool isEmpty() override;
    void removeFromHeap() override;
    void sweep() override;
    void makeConsistentForGC() override;
    void makeConsistentForMutator() override;
#if defined(ADDRESS_SANITIZER)
    void poisonObjects(ThreadState::ObjectsToPoison, ThreadState::Poisoning) override;
#endif
    void checkAndMarkPointer(Visitor*, Address) override;
    void markOrphaned() override;

    void takeSnapshot(String dumpBaseName, size_t pageIndex, size_t* outFreeSize, size_t* outFreeCount) override;
#if ENABLE(GC_PROFILING)
    const GCInfo* findGCInfo(Address) override;
    void snapshot(TracedValue*, ThreadState::SnapshotInfo*) override;
    void incrementMarkedObjectsAge() override;
    void countMarkedObjects(ClassAgeCountsMap&) override;
    void countObjectsToSweep(ClassAgeCountsMap&) override;
#endif
#if ENABLE(ASSERT) || ENABLE(GC_PROFILING)
    // Returns true for any address that is on one of the pages that this
    // large object uses. That ensures that we can use a negative result to
    // populate the negative page cache.
    bool contains(Address) override;
#endif
    virtual size_t size() override
    {
        return pageHeaderSize() +  sizeof(HeapObjectHeader) + m_payloadSize;
    }
    static size_t pageHeaderSize()
    {
        // Compute the amount of padding we have to add to a header to make
        // the size of the header plus the padding a multiple of 8 bytes.
        size_t paddingSize = (sizeof(LargeObjectPage) + allocationGranularity - (sizeof(HeapObjectHeader) % allocationGranularity)) % allocationGranularity;
        return sizeof(LargeObjectPage) + paddingSize;
    }
    bool isLargeObjectPage() override { return true; }

    HeapObjectHeader* heapObjectHeader()
    {
        Address headerAddress = address() + pageHeaderSize();
        return reinterpret_cast<HeapObjectHeader*>(headerAddress);
    }

#ifdef ANNOTATE_CONTIGUOUS_CONTAINER
    void setIsVectorBackingPage() { m_isVectorBackingPage = true; }
    bool isVectorBackingPage() const { return m_isVectorBackingPage; }
#endif

private:

    size_t m_payloadSize;
#ifdef ANNOTATE_CONTIGUOUS_CONTAINER
    bool m_isVectorBackingPage;
#endif
};

// A HeapDoesNotContainCache provides a fast way of taking an arbitrary
// pointer-sized word, and determining whether it cannot be interpreted as a
// pointer to an area that is managed by the garbage collected Blink heap.  This
// is a cache of 'pages' that have previously been determined to be wholly
// outside of the heap.  The size of these pages must be smaller than the
// allocation alignment of the heap pages.  We determine off-heap-ness by
// rounding down the pointer to the nearest page and looking up the page in the
// cache.  If there is a miss in the cache we can determine the status of the
// pointer precisely using the heap RegionTree.
//
// The HeapDoesNotContainCache is a negative cache, so it must be flushed when
// memory is added to the heap.
class HeapDoesNotContainCache {
public:
    HeapDoesNotContainCache()
        : m_entries(adoptArrayPtr(new Address[HeapDoesNotContainCache::numberOfEntries]))
        , m_hasEntries(false)
    {
        // Start by flushing the cache in a non-empty state to initialize all the cache entries.
        for (int i = 0; i < numberOfEntries; ++i)
            m_entries[i] = nullptr;
    }

    void flush();
    bool isEmpty() { return !m_hasEntries; }

    // Perform a lookup in the cache.
    //
    // If lookup returns false, the argument address was not found in
    // the cache and it is unknown if the address is in the Blink
    // heap.
    //
    // If lookup returns true, the argument address was found in the
    // cache which means the address is not in the heap.
    PLATFORM_EXPORT bool lookup(Address);

    // Add an entry to the cache.
    PLATFORM_EXPORT void addEntry(Address);

private:
    static const int numberOfEntriesLog2 = 12;
    static const int numberOfEntries = 1 << numberOfEntriesLog2;

    static size_t hash(Address);

    WTF::OwnPtr<Address[]> m_entries;
    bool m_hasEntries;
};

class FreeList {
public:
    FreeList();

    void addToFreeList(Address, size_t);
    void clear();

    // Returns a bucket number for inserting a FreeListEntry of a given size.
    // All FreeListEntries in the given bucket, n, have size >= 2^n.
    static int bucketIndexForSize(size_t);

    // Returns true if the freelist snapshot is captured.
    bool takeSnapshot(const String& dumpBaseName);

#if ENABLE(GC_PROFILING)
    struct PerBucketFreeListStats {
        size_t entryCount;
        size_t freeSize;

        PerBucketFreeListStats() : entryCount(0), freeSize(0) { }
    };

    void getFreeSizeStats(PerBucketFreeListStats bucketStats[], size_t& totalSize) const;
#endif

#if ENABLE(ASSERT) || defined(LEAK_SANITIZER) || defined(ADDRESS_SANITIZER)
    static void zapFreedMemory(Address, size_t);
#endif

private:
    int m_biggestFreeListIndex;

    // All FreeListEntries in the nth list have size >= 2^n.
    FreeListEntry* m_freeLists[blinkPageSizeLog2];

    friend class NormalPageHeap;
};

// Each thread has a number of thread heaps (e.g., Generic heaps,
// typed heaps for Node, heaps for collection backings etc)
// and BaseHeap represents each thread heap.
//
// BaseHeap is a parent class of NormalPageHeap and LargeObjectHeap.
// NormalPageHeap represents a heap that contains NormalPages
// and LargeObjectHeap represents a heap that contains LargeObjectPages.
class PLATFORM_EXPORT BaseHeap {
public:
    BaseHeap(ThreadState*, int);
    virtual ~BaseHeap();
    void cleanupPages();

    void takeSnapshot(const String& dumpBaseName);
#if ENABLE(ASSERT) || ENABLE(GC_PROFILING)
    BasePage* findPageFromAddress(Address);
#endif
    virtual void takeFreelistSnapshot(const String& dumpBaseName) { }
#if ENABLE(GC_PROFILING)
    void snapshot(TracedValue*, ThreadState::SnapshotInfo*);
    virtual void snapshotFreeList(TracedValue&) { }

    void countMarkedObjects(ClassAgeCountsMap&) const;
    void countObjectsToSweep(ClassAgeCountsMap&) const;
    void incrementMarkedObjectsAge();
#endif

    virtual void clearFreeLists() { }
    void makeConsistentForGC();
    void makeConsistentForMutator();
#if ENABLE(ASSERT)
    virtual bool isConsistentForGC() = 0;
#endif
    size_t objectPayloadSizeForTesting();
    void prepareHeapForTermination();
    void prepareForSweep();
#if defined(ADDRESS_SANITIZER)
    void poisonHeap(ThreadState::ObjectsToPoison, ThreadState::Poisoning);
#endif
    Address lazySweep(size_t, size_t gcInfoIndex);
    void sweepUnsweptPage();
    // Returns true if we have swept all pages within the deadline.
    // Returns false otherwise.
    bool lazySweepWithDeadline(double deadlineSeconds);
    void completeSweep();

    ThreadState* threadState() { return m_threadState; }
    int heapIndex() const { return m_index; }

protected:
    BasePage* m_firstPage;
    BasePage* m_firstUnsweptPage;

private:
    virtual Address lazySweepPages(size_t, size_t gcInfoIndex) = 0;

    ThreadState* m_threadState;

    // Index into the page pools.  This is used to ensure that the pages of the
    // same type go into the correct page pool and thus avoid type confusion.
    int m_index;
};

class PLATFORM_EXPORT NormalPageHeap final : public BaseHeap {
public:
    NormalPageHeap(ThreadState*, int);
    void addToFreeList(Address address, size_t size)
    {
        ASSERT(findPageFromAddress(address));
        ASSERT(findPageFromAddress(address + size - 1));
        m_freeList.addToFreeList(address, size);
    }
    void clearFreeLists() override;
#if ENABLE(ASSERT)
    bool isConsistentForGC() override;
    bool pagesToBeSweptContains(Address);
#endif
    void takeFreelistSnapshot(const String& dumpBaseName) override;
#if ENABLE(GC_PROFILING)
    void snapshotFreeList(TracedValue&) override;
#endif

    Address allocateObject(size_t allocationSize, size_t gcInfoIndex);

    void freePage(NormalPage*);

    bool coalesce();
    void promptlyFreeObject(HeapObjectHeader*);
    bool expandObject(HeapObjectHeader*, size_t);
    bool shrinkObject(HeapObjectHeader*, size_t);
    void decreasePromptlyFreedSize(size_t size) { m_promptlyFreedSize -= size; }

private:
    void allocatePage();
    Address lazySweepPages(size_t, size_t gcInfoIndex) override;
    Address outOfLineAllocate(size_t allocationSize, size_t gcInfoIndex);
    Address currentAllocationPoint() const { return m_currentAllocationPoint; }
    size_t remainingAllocationSize() const { return m_remainingAllocationSize; }
    bool hasCurrentAllocationArea() const { return currentAllocationPoint() && remainingAllocationSize(); }
    void setAllocationPoint(Address, size_t);
    void updateRemainingAllocationSize();
    Address allocateFromFreeList(size_t, size_t gcInfoIndex);

    FreeList m_freeList;
    Address m_currentAllocationPoint;
    size_t m_remainingAllocationSize;
    size_t m_lastRemainingAllocationSize;

    // The size of promptly freed objects in the heap.
    size_t m_promptlyFreedSize;

#if ENABLE(GC_PROFILING)
    size_t m_cumulativeAllocationSize;
    size_t m_allocationCount;
    size_t m_inlineAllocationCount;
#endif
};

class LargeObjectHeap final : public BaseHeap {
public:
    LargeObjectHeap(ThreadState*, int);
    Address allocateLargeObjectPage(size_t, size_t gcInfoIndex);
    void freeLargeObjectPage(LargeObjectPage*);
#if ENABLE(ASSERT)
    bool isConsistentForGC() override { return true; }
#endif
private:
    Address doAllocateLargeObjectPage(size_t, size_t gcInfoIndex);
    Address lazySweepPages(size_t, size_t gcInfoIndex) override;
};

// Mask an address down to the enclosing oilpan heap base page.  All oilpan heap
// pages are aligned at blinkPageBase plus an OS page size.
// FIXME: Remove PLATFORM_EXPORT once we get a proper public interface to our
// typed heaps.  This is only exported to enable tests in HeapTest.cpp.
PLATFORM_EXPORT inline BasePage* pageFromObject(const void* object)
{
    Address address = reinterpret_cast<Address>(const_cast<void*>(object));
    BasePage* page = reinterpret_cast<BasePage*>(blinkPageAddress(address) + WTF::kSystemPageSize);
    ASSERT(page->contains(address));
    return page;
}

template<typename T, bool = NeedsAdjustAndMark<T>::value> class ObjectAliveTrait;

template<typename T>
class ObjectAliveTrait<T, false> {
public:
    static bool isHeapObjectAlive(T* object)
    {
        static_assert(sizeof(T), "T must be fully defined");
        return HeapObjectHeader::fromPayload(object)->isMarked();
    }
};

template<typename T>
class ObjectAliveTrait<T, true> {
public:
    static bool isHeapObjectAlive(T* object)
    {
        static_assert(sizeof(T), "T must be fully defined");
        return object->isHeapObjectAlive();
    }
};

class PLATFORM_EXPORT Heap {
public:
    static void init();
    static void shutdown();
    static void doShutdown();

#if ENABLE(ASSERT) || ENABLE(GC_PROFILING)
    static BasePage* findPageFromAddress(Address);
    static BasePage* findPageFromAddress(const void* pointer) { return findPageFromAddress(reinterpret_cast<Address>(const_cast<void*>(pointer))); }
#endif

    template<typename T>
    static inline bool isHeapObjectAlive(T* object)
    {
        static_assert(sizeof(T), "T must be fully defined");
        // The strongification of collections relies on the fact that once a
        // collection has been strongified, there is no way that it can contain
        // non-live entries, so no entries will be removed. Since you can't set
        // the mark bit on a null pointer, that means that null pointers are
        // always 'alive'.
        if (!object)
            return true;
        return ObjectAliveTrait<T>::isHeapObjectAlive(object);
    }
    template<typename T>
    static inline bool isHeapObjectAlive(const Member<T>& member)
    {
        return isHeapObjectAlive(member.get());
    }
    template<typename T>
    static inline bool isHeapObjectAlive(const WeakMember<T>& member)
    {
        return isHeapObjectAlive(member.get());
    }
    template<typename T>
    static inline bool isHeapObjectAlive(const RawPtr<T>& ptr)
    {
        return isHeapObjectAlive(ptr.get());
    }

    // Is the finalizable GC object still alive, but slated for lazy sweeping?
    // If a lazy sweep is in progress, returns true if the object was found
    // to be not reachable during the marking phase, but it has yet to be swept
    // and finalized. The predicate returns false in all other cases.
    //
    // Holding a reference to an already-dead object is not a valid state
    // to be in; willObjectBeLazilySwept() has undefined behavior if passed
    // such a reference.
    template<typename T>
    NO_LAZY_SWEEP_SANITIZE_ADDRESS
    static bool willObjectBeLazilySwept(const T* objectPointer)
    {
        static_assert(IsGarbageCollectedType<T>::value, "only objects deriving from GarbageCollected can be used.");
#if ENABLE(LAZY_SWEEPING)
        BasePage* page = pageFromObject(objectPointer);
        if (page->hasBeenSwept())
            return false;
        ASSERT(page->heap()->threadState()->isSweepingInProgress());

        return !Heap::isHeapObjectAlive(const_cast<T*>(objectPointer));
#else
        return false;
#endif
    }

    // Push a trace callback on the marking stack.
    static void pushTraceCallback(void* containerObject, TraceCallback);

    // Push a trace callback on the post-marking callback stack.  These
    // callbacks are called after normal marking (including ephemeron
    // iteration).
    static void pushPostMarkingCallback(void*, TraceCallback);

    // Add a weak pointer callback to the weak callback work list.  General
    // object pointer callbacks are added to a thread local weak callback work
    // list and the callback is called on the thread that owns the object, with
    // the closure pointer as an argument.  Most of the time, the closure and
    // the containerObject can be the same thing, but the containerObject is
    // constrained to be on the heap, since the heap is used to identify the
    // correct thread.
    static void pushThreadLocalWeakCallback(void* closure, void* containerObject, WeakCallback);

    // Similar to the more general pushThreadLocalWeakCallback, but cell
    // pointer callbacks are added to a static callback work list and the weak
    // callback is performed on the thread performing garbage collection.  This
    // is OK because cells are just cleared and no deallocation can happen.
    static void pushGlobalWeakCallback(void** cell, WeakCallback);

    // Pop the top of a marking stack and call the callback with the visitor
    // and the object.  Returns false when there is nothing more to do.
    static bool popAndInvokeTraceCallback(Visitor*);

    // Remove an item from the post-marking callback stack and call
    // the callback with the visitor and the object pointer.  Returns
    // false when there is nothing more to do.
    static bool popAndInvokePostMarkingCallback(Visitor*);

    // Remove an item from the weak callback work list and call the callback
    // with the visitor and the closure pointer.  Returns false when there is
    // nothing more to do.
    static bool popAndInvokeGlobalWeakCallback(Visitor*);

    // Register an ephemeron table for fixed-point iteration.
    static void registerWeakTable(void* containerObject, EphemeronCallback, EphemeronCallback);
#if ENABLE(ASSERT)
    static bool weakTableRegistered(const void*);
#endif

    static inline size_t allocationSizeFromSize(size_t size)
    {
        // Check the size before computing the actual allocation size.  The
        // allocation size calculation can overflow for large sizes and the check
        // therefore has to happen before any calculation on the size.
        RELEASE_ASSERT(size < maxHeapObjectSize);

        // Add space for header.
        size_t allocationSize = size + sizeof(HeapObjectHeader);
        // Align size with allocation granularity.
        allocationSize = (allocationSize + allocationMask) & ~allocationMask;
        return allocationSize;
    }
    static Address allocateOnHeapIndex(ThreadState*, size_t, int heapIndex, size_t gcInfoIndex);
    template<typename T> static Address allocate(size_t, bool eagerlySweep = false);
    template<typename T> static Address reallocate(void* previous, size_t);

    enum GCReason {
        IdleGC,
        PreciseGC,
        ConservativeGC,
        ForcedGC,
        NumberOfGCReason
    };
    static const char* gcReasonString(GCReason);
    static void collectGarbage(ThreadState::StackState, ThreadState::GCType, GCReason);
    static void collectGarbageForTerminatingThread(ThreadState*);
    static void collectAllGarbage();

    static void processMarkingStack(Visitor*);
    static void postMarkingProcessing(Visitor*);
    static void globalWeakProcessing(Visitor*);
    static void setForcePreciseGCForTesting();

    static void preGC();
    static void postGC(ThreadState::GCType);

    // Conservatively checks whether an address is a pointer in any of the
    // thread heaps.  If so marks the object pointed to as live.
    static Address checkAndMarkPointer(Visitor*, Address);

#if ENABLE(GC_PROFILING)
    // Dump the path to specified object on the next GC.  This method is to be
    // invoked from GDB.
    static void dumpPathToObjectOnNextGC(void* p);

    // Forcibly find GCInfo of the object at Address.  This is slow and should
    // only be used for debug purposes.  It involves finding the heap page and
    // scanning the heap page for an object header.
    static const GCInfo* findGCInfo(Address);

    static String createBacktraceString();
#endif

    static size_t objectPayloadSizeForTesting();

    static void flushHeapDoesNotContainCache();

    static FreePagePool* freePagePool() { return s_freePagePool; }
    static OrphanedPagePool* orphanedPagePool() { return s_orphanedPagePool; }

    // This look-up uses the region search tree and a negative contains cache to
    // provide an efficient mapping from arbitrary addresses to the containing
    // heap-page if one exists.
    static BasePage* lookup(Address);
    static void addPageMemoryRegion(PageMemoryRegion*);
    static void removePageMemoryRegion(PageMemoryRegion*);

    static const GCInfo* gcInfo(size_t gcInfoIndex)
    {
        ASSERT(gcInfoIndex >= 1);
        ASSERT(gcInfoIndex < GCInfoTable::maxIndex);
        ASSERT(s_gcInfoTable);
        const GCInfo* info = s_gcInfoTable[gcInfoIndex];
        ASSERT(info);
        return info;
    }

    static void increaseAllocatedObjectSize(size_t delta) { atomicAdd(&s_allocatedObjectSize, static_cast<long>(delta)); }
    static void decreaseAllocatedObjectSize(size_t delta) { atomicSubtract(&s_allocatedObjectSize, static_cast<long>(delta)); }
    static size_t allocatedObjectSize() { return acquireLoad(&s_allocatedObjectSize); }
    static void increaseMarkedObjectSize(size_t delta) { atomicAdd(&s_markedObjectSize, static_cast<long>(delta)); }
    static size_t markedObjectSize() { return acquireLoad(&s_markedObjectSize); }
    static void increaseAllocatedSpace(size_t delta) { atomicAdd(&s_allocatedSpace, static_cast<long>(delta)); }
    static void decreaseAllocatedSpace(size_t delta) { atomicSubtract(&s_allocatedSpace, static_cast<long>(delta)); }
    static size_t allocatedSpace() { return acquireLoad(&s_allocatedSpace); }
    static size_t estimatedLiveObjectSize() { return acquireLoad(&s_estimatedLiveObjectSize); }
    static void setEstimatedLiveObjectSize(size_t size) { releaseStore(&s_estimatedLiveObjectSize, size); }
    static size_t externalObjectSizeAtLastGC() { return acquireLoad(&s_externalObjectSizeAtLastGC); }

    static double estimatedMarkingTime();
    static void reportMemoryUsageHistogram();

#if ENABLE(GC_PROFILING)
    static void reportMemoryUsageForTracing();
#else
    static void reportMemoryUsageForTracing() { }
#endif

private:
    // A RegionTree is a simple binary search tree of PageMemoryRegions sorted
    // by base addresses.
    class RegionTree {
    public:
        explicit RegionTree(PageMemoryRegion* region) : m_region(region), m_left(nullptr), m_right(nullptr) { }
        ~RegionTree()
        {
            delete m_left;
            delete m_right;
        }
        PageMemoryRegion* lookup(Address);
        static void add(RegionTree*, RegionTree**);
        static void remove(PageMemoryRegion*, RegionTree**);
    private:
        PageMemoryRegion* m_region;
        RegionTree* m_left;
        RegionTree* m_right;
    };

    // Reset counters that track live and allocated-since-last-GC sizes.
    static void resetHeapCounters();

    static int heapIndexForObjectSize(size_t);
    static bool isNormalHeapIndex(int);

    static CallbackStack* s_markingStack;
    static CallbackStack* s_postMarkingCallbackStack;
    static CallbackStack* s_globalWeakCallbackStack;
    static CallbackStack* s_ephemeronStack;
    static HeapDoesNotContainCache* s_heapDoesNotContainCache;
    static bool s_shutdownCalled;
    static FreePagePool* s_freePagePool;
    static OrphanedPagePool* s_orphanedPagePool;
    static RegionTree* s_regionTree;
    static size_t s_allocatedSpace;
    static size_t s_allocatedObjectSize;
    static size_t s_markedObjectSize;
    static size_t s_estimatedLiveObjectSize;
    static size_t s_externalObjectSizeAtLastGC;
    static double s_estimatedMarkingTimePerByte;

    friend class ThreadState;
};

template<typename T>
struct IsEagerlyFinalizedType {
private:
    typedef char YesType;
    struct NoType {
        char padding[8];
    };

    template <typename U> static YesType checkMarker(typename U::IsEagerlyFinalizedMarker*);
    template <typename U> static NoType checkMarker(...);

public:
    static const bool value = sizeof(checkMarker<T>(nullptr)) == sizeof(YesType);
};

template<typename T> class GarbageCollected {
    WTF_MAKE_NONCOPYABLE(GarbageCollected);

    // For now direct allocation of arrays on the heap is not allowed.
    void* operator new[](size_t size);

#if OS(WIN) && COMPILER(MSVC)
    // Due to some quirkiness in the MSVC compiler we have to provide
    // the delete[] operator in the GarbageCollected subclasses as it
    // is called when a class is exported in a DLL.
protected:
    void operator delete[](void* p)
    {
        ASSERT_NOT_REACHED();
    }
#else
    void operator delete[](void* p);
#endif

public:
    using GarbageCollectedBase = T;

    void* operator new(size_t size)
    {
        return allocateObject(size, IsEagerlyFinalizedType<T>::value);
    }

    static void* allocateObject(size_t size, bool eagerlySweep)
    {
        return Heap::allocate<T>(size, eagerlySweep);
    }

    void operator delete(void* p)
    {
        ASSERT_NOT_REACHED();
    }

protected:
    GarbageCollected()
    {
    }
};

// Assigning class types to their heaps.
//
// We use sized heaps for most 'normal' objects to improve memory locality.
// It seems that the same type of objects are likely to be accessed together,
// which means that we want to group objects by type. That's one reason
// why we provide dedicated heaps for popular types (e.g., Node, CSSValue),
// but it's not practical to prepare dedicated heaps for all types.
// Thus we group objects by their sizes, hoping that this will approximately
// group objects by their types.
//
// An exception to the use of sized heaps is made for class types that
// require prompt finalization after a garbage collection. That is, their
// instances have to be finalized early and cannot be delayed until lazy
// sweeping kicks in for their heap and page. The EAGERLY_FINALIZE()
// macro is used to declare a class (and its derived classes) as being
// in need of eager finalization. Must be defined with 'public' visibility
// for a class.
//

inline int Heap::heapIndexForObjectSize(size_t size)
{
    if (size < 64) {
        if (size < 32)
            return ThreadState::NormalPage1HeapIndex;
        return ThreadState::NormalPage2HeapIndex;
    }
    if (size < 128)
        return ThreadState::NormalPage3HeapIndex;
    return ThreadState::NormalPage4HeapIndex;
}

inline bool Heap::isNormalHeapIndex(int index)
{
    return index >= ThreadState::NormalPage1HeapIndex && index <= ThreadState::NormalPage4HeapIndex;
}

#define DECLARE_EAGER_FINALIZATION_OPERATOR_NEW() \
public:                                           \
    GC_PLUGIN_IGNORE("491488")                    \
    void* operator new(size_t size)               \
    {                                             \
        return allocateObject(size, true);        \
    }

#define IS_EAGERLY_FINALIZED() (pageFromObject(this)->heap()->heapIndex() == ThreadState::EagerSweepHeapIndex)
#if ENABLE(ASSERT) && ENABLE(OILPAN)
class VerifyEagerFinalization {
public:
    ~VerifyEagerFinalization()
    {
        // If this assert triggers, the class annotated as eagerly
        // finalized ended up not being allocated on the heap
        // set aside for eager finalization. The reason is most
        // likely that the effective 'operator new' overload for
        // this class' leftmost base is for a class that is not
        // eagerly finalized. Declaring and defining an 'operator new'
        // for this class is what's required -- consider using
        // DECLARE_EAGER_FINALIZATION_OPERATOR_NEW().
        ASSERT(IS_EAGERLY_FINALIZED());
    }
};
#define EAGERLY_FINALIZE()                             \
private:                                               \
    VerifyEagerFinalization m_verifyEagerFinalization; \
public:                                                \
    typedef int IsEagerlyFinalizedMarker
#else
#define EAGERLY_FINALIZE() typedef int IsEagerlyFinalizedMarker
#endif

#if !ENABLE(OILPAN) && ENABLE(LAZY_SWEEPING)
#define EAGERLY_FINALIZE_WILL_BE_REMOVED() EAGERLY_FINALIZE()
#else
#define EAGERLY_FINALIZE_WILL_BE_REMOVED()
#endif

NO_SANITIZE_ADDRESS inline
size_t HeapObjectHeader::size() const
{
    size_t result = m_encoded & headerSizeMask;
    // Large objects should not refer to header->size().
    // The actual size of a large object is stored in
    // LargeObjectPage::m_payloadSize.
    ASSERT(result != largeObjectSizeInHeader);
    ASSERT(!pageFromObject(this)->isLargeObjectPage());
    return result;
}

#if ENABLE(ASSERT)
NO_SANITIZE_ADDRESS inline
bool HeapObjectHeader::checkHeader() const
{
    return !pageFromObject(this)->orphaned() && m_magic == magic;
}
#endif

inline Address HeapObjectHeader::payload()
{
    return reinterpret_cast<Address>(this) + sizeof(HeapObjectHeader);
}

inline Address HeapObjectHeader::payloadEnd()
{
    return reinterpret_cast<Address>(this) + size();
}

NO_SANITIZE_ADDRESS inline
size_t HeapObjectHeader::payloadSize()
{
    size_t size = m_encoded & headerSizeMask;
    if (UNLIKELY(size == largeObjectSizeInHeader)) {
        ASSERT(pageFromObject(this)->isLargeObjectPage());
        return static_cast<LargeObjectPage*>(pageFromObject(this))->payloadSize();
    }
    ASSERT(!pageFromObject(this)->isLargeObjectPage());
    return size - sizeof(HeapObjectHeader);
}

inline HeapObjectHeader* HeapObjectHeader::fromPayload(const void* payload)
{
    Address addr = reinterpret_cast<Address>(const_cast<void*>(payload));
    HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(addr - sizeof(HeapObjectHeader));
    ASSERT(header->checkHeader());
    return header;
}

NO_SANITIZE_ADDRESS inline
bool HeapObjectHeader::isMarked() const
{
    ASSERT(checkHeader());
    return m_encoded & headerMarkBitMask;
}

NO_SANITIZE_ADDRESS inline
void HeapObjectHeader::mark()
{
    ASSERT(checkHeader());
    ASSERT(!isMarked());
    m_encoded = m_encoded | headerMarkBitMask;
}

NO_SANITIZE_ADDRESS inline
void HeapObjectHeader::unmark()
{
    ASSERT(checkHeader());
    ASSERT(isMarked());
    m_encoded &= ~headerMarkBitMask;
}

NO_SANITIZE_ADDRESS inline
bool HeapObjectHeader::isDead() const
{
    ASSERT(checkHeader());
    return m_encoded & headerDeadBitMask;
}

NO_SANITIZE_ADDRESS inline
void HeapObjectHeader::markDead()
{
    ASSERT(checkHeader());
    ASSERT(!isMarked());
    m_encoded |= headerDeadBitMask;
}

inline Address NormalPageHeap::allocateObject(size_t allocationSize, size_t gcInfoIndex)
{
#if ENABLE(GC_PROFILING)
    m_cumulativeAllocationSize += allocationSize;
    ++m_allocationCount;
#endif

    if (LIKELY(allocationSize <= m_remainingAllocationSize)) {
#if ENABLE(GC_PROFILING)
        ++m_inlineAllocationCount;
#endif
        Address headerAddress = m_currentAllocationPoint;
        m_currentAllocationPoint += allocationSize;
        m_remainingAllocationSize -= allocationSize;
        ASSERT(gcInfoIndex > 0);
        new (NotNull, headerAddress) HeapObjectHeader(allocationSize, gcInfoIndex);
        Address result = headerAddress + sizeof(HeapObjectHeader);
        ASSERT(!(reinterpret_cast<uintptr_t>(result) & allocationMask));

        // Unpoison the memory used for the object (payload).
        ASAN_UNPOISON_MEMORY_REGION(result, allocationSize - sizeof(HeapObjectHeader));
        FILL_ZERO_IF_NOT_PRODUCTION(result, allocationSize - sizeof(HeapObjectHeader));
        ASSERT(findPageFromAddress(headerAddress + allocationSize - 1));
        return result;
    }
    return outOfLineAllocate(allocationSize, gcInfoIndex);
}

template<typename Derived>
template<typename T>
void VisitorHelper<Derived>::handleWeakCell(Visitor* self, void* object)
{
    T** cell = reinterpret_cast<T**>(object);
    if (*cell && !ObjectAliveTrait<T>::isHeapObjectAlive(*cell))
        *cell = nullptr;
}

inline Address Heap::allocateOnHeapIndex(ThreadState* state, size_t size, int heapIndex, size_t gcInfoIndex)
{
    ASSERT(state->isAllocationAllowed());
    ASSERT(heapIndex != ThreadState::LargeObjectHeapIndex);
    NormalPageHeap* heap = static_cast<NormalPageHeap*>(state->heap(heapIndex));
    return heap->allocateObject(allocationSizeFromSize(size), gcInfoIndex);
}

template<typename T>
Address Heap::allocate(size_t size, bool eagerlySweep)
{
    ThreadState* state = ThreadStateFor<ThreadingTrait<T>::Affinity>::state();
    return Heap::allocateOnHeapIndex(state, size, eagerlySweep ? ThreadState::EagerSweepHeapIndex : Heap::heapIndexForObjectSize(size), GCInfoTrait<T>::index());
}

template<typename T>
Address Heap::reallocate(void* previous, size_t size)
{
    // Not intended to be a full C realloc() substitute;
    // realloc(nullptr, size) is not a supported alias for malloc(size).

    // TODO(sof): promptly free the previous object.
    if (!size) {
        // If the new size is 0 this is considered equivalent to free(previous).
        return nullptr;
    }

    ThreadState* state = ThreadStateFor<ThreadingTrait<T>::Affinity>::state();
    HeapObjectHeader* previousHeader = HeapObjectHeader::fromPayload(previous);
    BasePage* page = pageFromObject(previousHeader);
    ASSERT(page);
    int heapIndex = page->heap()->heapIndex();
    // Recompute the effective heap index if previous allocation
    // was on the normal heaps or a large object.
    if (isNormalHeapIndex(heapIndex) || heapIndex == ThreadState::LargeObjectHeapIndex)
        heapIndex = heapIndexForObjectSize(size);

    // TODO(haraken): We don't support reallocate() for finalizable objects.
    ASSERT(!Heap::gcInfo(previousHeader->gcInfoIndex())->hasFinalizer());
    ASSERT(previousHeader->gcInfoIndex() == GCInfoTrait<T>::index());
    Address address = Heap::allocateOnHeapIndex(state, size, heapIndex, GCInfoTrait<T>::index());
    size_t copySize = previousHeader->payloadSize();
    if (copySize > size)
        copySize = size;
    memcpy(address, previous, copySize);
    return address;
}

} // namespace blink

#endif // Heap_h
