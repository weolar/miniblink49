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

#ifndef WTF_PartitionAlloc_h
#define WTF_PartitionAlloc_h

// DESCRIPTION
// partitionAlloc() / partitionAllocGeneric() and partitionFree() /
// partitionFreeGeneric() are approximately analagous to malloc() and free().
//
// The main difference is that a PartitionRoot / PartitionRootGeneric object
// must be supplied to these functions, representing a specific "heap partition"
// that will be used to satisfy the allocation. Different partitions are
// guaranteed to exist in separate address spaces, including being separate from
// the main system heap. If the contained objects are all freed, physical memory
// is returned to the system but the address space remains reserved.
// See PartitionAlloc.md for other security properties PartitionAlloc provides.
//
// THE ONLY LEGITIMATE WAY TO OBTAIN A PartitionRoot IS THROUGH THE
// SizeSpecificPartitionAllocator / PartitionAllocatorGeneric classes. To
// minimize the instruction count to the fullest extent possible, the
// PartitionRoot is really just a header adjacent to other data areas provided
// by the allocator class.
//
// The partitionAlloc() variant of the API has the following caveats:
// - Allocations and frees against a single partition must be single threaded.
// - Allocations must not exceed a max size, chosen at compile-time via a
// templated parameter to PartitionAllocator.
// - Allocation sizes must be aligned to the system pointer size.
// - Allocations are bucketed exactly according to size.
//
// And for partitionAllocGeneric():
// - Multi-threaded use against a single partition is ok; locking is handled.
// - Allocations of any arbitrary size can be handled (subject to a limit of
// INT_MAX bytes for security reasons).
// - Bucketing is by approximate size, for example an allocation of 4000 bytes
// might be placed into a 4096-byte bucket. Bucket sizes are chosen to try and
// keep worst-case waste to ~10%.
//
// The allocators are designed to be extremely fast, thanks to the following
// properties and design:
// - Just two single (reasonably predicatable) branches in the hot / fast path for
// both allocating and (significantly) freeing.
// - A minimal number of operations in the hot / fast path, with the slow paths
// in separate functions, leading to the possibility of inlining.
// - Each partition page (which is usually multiple physical pages) has a
// metadata structure which allows fast mapping of free() address to an
// underlying bucket.
// - Supports a lock-free API for fast performance in single-threaded cases.
// - The freelist for a given bucket is split across a number of partition
// pages, enabling various simple tricks to try and minimize fragmentation.
// - Fine-grained bucket sizes leading to less waste and better packing.
//
// The following security properties could be investigated in the future:
// - Per-object bucketing (instead of per-size) is mostly available at the API,
// but not used yet.
// - No randomness of freelist entries or bucket position.
// - Better checking for wild pointers in free().
// - Better freelist masking function to guarantee fault on 32-bit.

#include "wtf/Assertions.h"
#include "wtf/BitwiseOperations.h"
#include "wtf/ByteSwap.h"
#include "wtf/CPU.h"
#include "wtf/SpinLock.h"
#include "wtf/TypeTraits.h"
#include "wtf/PageAllocator.h"

#include <limits.h>

#if defined(MEMORY_TOOL_REPLACES_ALLOCATOR)
#include <stdlib.h>
#endif

#if ENABLE(ASSERT)
#include <string.h>
#endif

namespace WTF {

// Allocation granularity of sizeof(void*) bytes.
static const size_t kAllocationGranularity = sizeof(void*);
static const size_t kAllocationGranularityMask = kAllocationGranularity - 1;
static const size_t kBucketShift = (kAllocationGranularity == 8) ? 3 : 2;

// Underlying partition storage pages are a power-of-two size. It is typical
// for a partition page to be based on multiple system pages. Most references to
// "page" refer to partition pages.
// We also have the concept of "super pages" -- these are the underlying system
// allocations we make. Super pages contain multiple partition pages inside them
// and include space for a small amount of metadata per partition page.
// Inside super pages, we store "slot spans". A slot span is a continguous range
// of one or more partition pages that stores allocations of the same size.
// Slot span sizes are adjusted depending on the allocation size, to make sure
// the packing does not lead to unused (wasted) space at the end of the last
// system page of the span. For our current max slot span size of 64k and other
// constant values, we pack _all_ partitionAllocGeneric() sizes perfectly up
// against the end of a system page.
static const size_t kPartitionPageShift = 14; // 16KB
static const size_t kPartitionPageSize = 1 << kPartitionPageShift;
static const size_t kPartitionPageOffsetMask = kPartitionPageSize - 1;
static const size_t kPartitionPageBaseMask = ~kPartitionPageOffsetMask;
static const size_t kMaxPartitionPagesPerSlotSpan = 4;

// To avoid fragmentation via never-used freelist entries, we hand out partition
// freelist sections gradually, in units of the dominant system page size.
// What we're actually doing is avoiding filling the full partition page (16 KB)
// with freelist pointers right away. Writing freelist pointers will fault and
// dirty a private page, which is very wasteful if we never actually store
// objects there.
static const size_t kNumSystemPagesPerPartitionPage = kPartitionPageSize / kSystemPageSize;
static const size_t kMaxSystemPagesPerSlotSpan = kNumSystemPagesPerPartitionPage * kMaxPartitionPagesPerSlotSpan;

// We reserve virtual address space in 2MB chunks (aligned to 2MB as well).
// These chunks are called "super pages". We do this so that we can store
// metadata in the first few pages of each 2MB aligned section. This leads to
// a very fast free(). We specifically choose 2MB because this virtual address
// block represents a full but single PTE allocation on ARM, ia32 and x64.
//
// The layout of the super page is as follows. The sizes below are the same
// for 32 bit and 64 bit.
//
//   | Guard page (4KB) | Metadata page (4KB) | Guard pages (8KB) | Slot span | Slot span | ... | Slot span | Guard page (4KB) |
//
//   - Each slot span is a contiguous range of one or more PartitionPages.
//   - The metadata page has the following format. Note that the PartitionPage
//     that is not at the head of a slot span is "unused". In other words,
//     the metadata for the slot span is stored only in the first PartitionPage
//     of the slot span. Metadata accesses to other PartitionPages are
//     redirected to the first PartitionPage.
//
//     | SuperPageExtentEntry (32B) | PartitionPage of slot span 1 (32B, used) | PartitionPage of slot span 1 (32B, unused) | PartitionPage of slot span 1 (32B, unused) | PartitionPage of slot span 2 (32B, used) | PartitionPage of slot span 3 (32B, used) | ... | PartitionPage of slot span N (32B, unused) |
//
// A direct mapped page has a similar layout to fake it looking like a super page:
//
//     | Guard page (4KB) | Metadata page (4KB) | Guard pages (8KB) | Direct mapped object | Guard page (4KB) |
//
//    - The metadata page has the following layout:
//
//     | SuperPageExtentEntry (32B) | PartitionPage (32B) | PartitionBucket (32B) | PartitionDirectMapExtent (8B) |
static const size_t kSuperPageShift = 21; // 2MB
static const size_t kSuperPageSize = 1 << kSuperPageShift;
static const size_t kSuperPageOffsetMask = kSuperPageSize - 1;
static const size_t kSuperPageBaseMask = ~kSuperPageOffsetMask;
static const size_t kNumPartitionPagesPerSuperPage = kSuperPageSize / kPartitionPageSize;

static const size_t kPageMetadataShift = 5; // 32 bytes per partition page.
static const size_t kPageMetadataSize = 1 << kPageMetadataShift;

// The following kGeneric* constants apply to the generic variants of the API.
// The "order" of an allocation is closely related to the power-of-two size of
// the allocation. More precisely, the order is the bit index of the
// most-significant-bit in the allocation size, where the bit numbers starts
// at index 1 for the least-significant-bit.
// In terms of allocation sizes, order 0 covers 0, order 1 covers 1, order 2
// covers 2->3, order 3 covers 4->7, order 4 covers 8->15.
static const size_t kGenericMinBucketedOrder = 4; // 8 bytes.
static const size_t kGenericMaxBucketedOrder = 20; // Largest bucketed order is 1<<(20-1) (storing 512KB -> almost 1MB)
static const size_t kGenericNumBucketedOrders = (kGenericMaxBucketedOrder - kGenericMinBucketedOrder) + 1;
static const size_t kGenericNumBucketsPerOrderBits = 3; // Eight buckets per order (for the higher orders), e.g. order 8 is 128, 144, 160, ..., 240
static const size_t kGenericNumBucketsPerOrder = 1 << kGenericNumBucketsPerOrderBits;
static const size_t kGenericNumBuckets = kGenericNumBucketedOrders * kGenericNumBucketsPerOrder;
static const size_t kGenericSmallestBucket = 1 << (kGenericMinBucketedOrder - 1);
static const size_t kGenericMaxBucketSpacing = 1 << ((kGenericMaxBucketedOrder - 1) - kGenericNumBucketsPerOrderBits);
static const size_t kGenericMaxBucketed = (1 << (kGenericMaxBucketedOrder - 1)) + ((kGenericNumBucketsPerOrder - 1) * kGenericMaxBucketSpacing);
static const size_t kGenericMinDirectMappedDownsize = kGenericMaxBucketed + 1; // Limit when downsizing a direct mapping using realloc().
static const size_t kGenericMaxDirectMapped = INT_MAX - kSystemPageSize;
static const size_t kBitsPerSizet = sizeof(void*) * CHAR_BIT;

// Constants for the memory reclaim logic.
static const size_t kMaxFreeableSpans = 16;

// If the total size in bytes of allocated but not committed pages exceeds this
// value (probably it is a "out of virtual address space" crash),
// a special crash stack trace is generated at |partitionOutOfMemory|.
// This is to distinguish "out of virtual address space" from
// "out of physical memory" in crash reports.
static const size_t kReasonableSizeOfUnusedPages = 1024 * 1024 * 1024; // 1GiB

#if ENABLE(ASSERT)
// These two byte values match tcmalloc.
static const unsigned char kUninitializedByte = 0xAB;
static const unsigned char kFreedByte = 0xCD;
static const size_t kCookieSize = 16; // Handles alignment up to XMM instructions on Intel.
static const unsigned char kCookieValue[kCookieSize] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xD0, 0x0D, 0x13, 0x37, 0xF0, 0x05, 0xBA, 0x11, 0xAB, 0x1E };
#endif

struct PartitionBucket;
struct PartitionRootBase;

struct PartitionFreelistEntry {
    PartitionFreelistEntry* next;
};

// Some notes on page states. A page can be in one of four major states:
// 1) Active.
// 2) Full.
// 3) Empty.
// 4) Decommitted.
// An active page has available free slots. A full page has no free slots. An
// empty page has no free slots, and a decommitted page is an empty page that
// had its backing memory released back to the system.
// There are two linked lists tracking the pages. The "active page" list is an
// approximation of a list of active pages. It is an approximation because
// full, empty and decommitted pages may briefly be present in the list until
// we next do a scan over it.
// The "empty page" list is an accurate list of pages which are either empty
// or decommitted.
//
// The significant page transitions are:
// - free() will detect when a full page has a slot free()'d and immediately
// return the page to the head of the active list.
// - free() will detect when a page is fully emptied. It _may_ add it to the
// empty list or it _may_ leave it on the active list until a future list scan.
// - malloc() _may_ scan the active page list in order to fulfil the request.
// If it does this, full, empty and decommitted pages encountered will be
// booted out of the active list. If there are no suitable active pages found,
// an empty or decommitted page (if one exists) will be pulled from the empty
// list on to the active list.
struct PartitionPage {
    PartitionFreelistEntry* freelistHead;
    PartitionPage* nextPage;
    PartitionBucket* bucket;
    int16_t numAllocatedSlots; // Deliberately signed, 0 for empty or decommitted page, -n for full pages.
    uint16_t numUnprovisionedSlots;
    uint16_t pageOffset;
    int16_t emptyCacheIndex; // -1 if not in the empty cache.
};

struct PartitionBucket {
    PartitionPage* activePagesHead; // Accessed most in hot path => goes first.
    PartitionPage* emptyPagesHead;
    PartitionPage* decommittedPagesHead;
    uint32_t slotSize;
    unsigned numSystemPagesPerSlotSpan : 8;
    unsigned numFullPages : 24;
};

// An "extent" is a span of consecutive superpages. We link to the partition's
// next extent (if there is one) at the very start of a superpage's metadata
// area.
struct PartitionSuperPageExtentEntry {
    PartitionRootBase* root;
    char* superPageBase;
    char* superPagesEnd;
    PartitionSuperPageExtentEntry* next;
};

struct PartitionDirectMapExtent {
    PartitionDirectMapExtent* nextExtent;
    PartitionDirectMapExtent* prevExtent;
    PartitionBucket* bucket;
    size_t mapSize; // Mapped size, not including guard pages and meta-data.
};

struct WTF_EXPORT PartitionRootBase {
    size_t totalSizeOfCommittedPages;
    size_t totalSizeOfSuperPages;
    size_t totalSizeOfDirectMappedPages;
    // Invariant: totalSizeOfCommittedPages <= totalSizeOfSuperPages + totalSizeOfDirectMappedPages.
    unsigned numBuckets;
    unsigned maxAllocation;
    bool initialized;
    char* nextSuperPage;
    char* nextPartitionPage;
    char* nextPartitionPageEnd;
    PartitionSuperPageExtentEntry* currentExtent;
    PartitionSuperPageExtentEntry* firstExtent;
    PartitionDirectMapExtent* directMapList;
    PartitionPage* globalEmptyPageRing[kMaxFreeableSpans];
    int16_t globalEmptyPageRingIndex;
    uintptr_t invertedSelf;

    static SpinLock gInitializedLock;
    static bool gInitialized;
    // gSeedPage is used as a sentinel to indicate that there is no page
    // in the active page list. We can use nullptr, but in that case we need
    // to add a null-check branch to the hot allocation path. We want to avoid
    // that.
    static PartitionPage gSeedPage;
    static PartitionBucket gPagedBucket;
    // gOomHandlingFunction is invoked when ParitionAlloc hits OutOfMemory.
    static void (*gOomHandlingFunction)();
};

// Never instantiate a PartitionRoot directly, instead use PartitionAlloc.
struct PartitionRoot : public PartitionRootBase {
    // The PartitionAlloc templated class ensures the following is correct.
    ALWAYS_INLINE PartitionBucket* buckets() { return reinterpret_cast<PartitionBucket*>(this + 1); }
    ALWAYS_INLINE const PartitionBucket* buckets() const { return reinterpret_cast<const PartitionBucket*>(this + 1); }
};

// Never instantiate a PartitionRootGeneric directly, instead use PartitionAllocatorGeneric.
struct PartitionRootGeneric : public PartitionRootBase {
    SpinLock lock;
    // Some pre-computed constants.
    size_t orderIndexShifts[kBitsPerSizet + 1];
    size_t orderSubIndexMasks[kBitsPerSizet + 1];
    // The bucket lookup table lets us map a size_t to a bucket quickly.
    // The trailing +1 caters for the overflow case for very large allocation sizes.
    // It is one flat array instead of a 2D array because in the 2D world, we'd
    // need to index array[blah][max+1] which risks undefined behavior.
    PartitionBucket* bucketLookups[((kBitsPerSizet + 1) * kGenericNumBucketsPerOrder) + 1];
    PartitionBucket buckets[kGenericNumBuckets];
};

// Flags for partitionAllocGenericFlags.
enum PartitionAllocFlags {
    PartitionAllocReturnNull = 1 << 0,
};

// Struct used to retrieve total memory usage of a partition. Used by
// PartitionStatsDumper implementation.
struct PartitionMemoryStats {
    size_t totalMmappedBytes; // Total bytes mmaped from the system.
    size_t totalCommittedBytes; // Total size of commmitted pages.
    size_t totalResidentBytes; // Total bytes provisioned by the partition.
    size_t totalActiveBytes; // Total active bytes in the partition.
    size_t totalDecommittableBytes; // Total bytes that could be decommitted.
    size_t totalDiscardableBytes; // Total bytes that could be discarded.
};

// Struct used to retrieve memory statistics about a partition bucket. Used by
// PartitionStatsDumper implementation.
struct PartitionBucketMemoryStats {
    bool isValid; // Used to check if the stats is valid.
    bool isDirectMap; // True if this is a direct mapping; size will not be unique.
    uint32_t bucketSlotSize; // The size of the slot in bytes.
    uint32_t allocatedPageSize; // Total size the partition page allocated from the system.
    uint32_t activeBytes; // Total active bytes used in the bucket.
    uint32_t residentBytes; // Total bytes provisioned in the bucket.
    uint32_t decommittableBytes; // Total bytes that could be decommitted.
    uint32_t discardableBytes; // Total bytes that could be discarded.
    uint32_t numFullPages; // Number of pages with all slots allocated.
    uint32_t numActivePages; // Number of pages that have at least one provisioned slot.
    uint32_t numEmptyPages; // Number of pages that are empty but not decommitted.
    uint32_t numDecommittedPages; // Number of pages that are empty and decommitted.
};

// Interface that is passed to partitionDumpStats and
// partitionDumpStatsGeneric for using the memory statistics.
class WTF_EXPORT PartitionStatsDumper {
public:
    // Called to dump total memory used by partition, once per partition.
    virtual void partitionDumpTotals(const char* partitionName, const PartitionMemoryStats*) = 0;

    // Called to dump stats about buckets, for each bucket.
    virtual void partitionsDumpBucketStats(const char* partitionName, const PartitionBucketMemoryStats*) = 0;
};

WTF_EXPORT void partitionAllocGlobalInit(void (*oomHandlingFunction)());
WTF_EXPORT void partitionAllocInit(PartitionRoot*, size_t numBuckets, size_t maxAllocation);
WTF_EXPORT bool partitionAllocShutdown(PartitionRoot*);
WTF_EXPORT void partitionAllocGenericInit(PartitionRootGeneric*);
WTF_EXPORT bool partitionAllocGenericShutdown(PartitionRootGeneric*);

enum PartitionPurgeFlags {
    // Decommitting the ring list of empty pages is reasonably fast.
    PartitionPurgeDecommitEmptyPages = 1 << 0,
    // Discarding unused system pages is slower, because it involves walking all
    // freelists in all active partition pages of all buckets >= system page
    // size. It often frees a similar amount of memory to decommitting the empty
    // pages, though.
    PartitionPurgeDiscardUnusedSystemPages = 1 << 1,
};

WTF_EXPORT void partitionPurgeMemory(PartitionRoot*, int);
WTF_EXPORT void partitionPurgeMemoryGeneric(PartitionRootGeneric*, int);

WTF_EXPORT NEVER_INLINE void* partitionAllocSlowPath(PartitionRootBase*, int, size_t, PartitionBucket*);
WTF_EXPORT NEVER_INLINE void partitionFreeSlowPath(PartitionPage*);
WTF_EXPORT NEVER_INLINE void* partitionReallocGeneric(PartitionRootGeneric*, void*, size_t, const char* typeName);

WTF_EXPORT void partitionDumpStats(PartitionRoot*, const char* partitionName, bool isLightDump, PartitionStatsDumper*);
WTF_EXPORT void partitionDumpStatsGeneric(PartitionRootGeneric*, const char* partitionName, bool isLightDump, PartitionStatsDumper*);

class WTF_EXPORT PartitionAllocHooks {
public:
    typedef void AllocationHook(void* address, size_t, const char* typeName);
    typedef void FreeHook(void* address);

    static void setAllocationHook(AllocationHook* hook) { m_allocationHook = hook; }
    static void setFreeHook(FreeHook* hook) { m_freeHook = hook; }

    static void allocationHookIfEnabled(void* address, size_t size, const char* typeName)
    {
        AllocationHook* allocationHook = m_allocationHook;
        if (UNLIKELY(allocationHook != nullptr))
            allocationHook(address, size, typeName);
    }

    static void freeHookIfEnabled(void* address)
    {
        FreeHook* freeHook = m_freeHook;
        if (UNLIKELY(freeHook != nullptr))
            freeHook(address);
    }

    static void reallocHookIfEnabled(void* oldAddress, void* newAddress, size_t size, const char* typeName)
    {
        // Report a reallocation as a free followed by an allocation.
        AllocationHook* allocationHook = m_allocationHook;
        FreeHook* freeHook = m_freeHook;
        if (UNLIKELY(allocationHook && freeHook)) {
            freeHook(oldAddress);
            allocationHook(newAddress, size, typeName);
        }
    }

private:
    // Pointers to hook functions that PartitionAlloc will call on allocation and
    // free if the pointers are non-null.
    static AllocationHook* m_allocationHook;
    static FreeHook* m_freeHook;
};

// In official builds, do not include type info string literals to avoid
// bloating the binary.
#if defined(OFFICIAL_BUILD)
#define WTF_HEAP_PROFILER_TYPE_NAME(T) nullptr
#else
#define WTF_HEAP_PROFILER_TYPE_NAME(T) ::WTF::getStringWithTypeName<T>()
#endif

PartitionFreelistEntry* partitionFreelistMask(PartitionFreelistEntry* ptr);

size_t partitionCookieSizeAdjustAdd(size_t size);

size_t partitionCookieSizeAdjustSubtract(size_t size);

void* partitionCookieFreePointerAdjust(void* ptr);

void partitionCookieWriteValue(void* ptr);

void partitionCookieCheckValue(void* ptr);

char* partitionSuperPageToMetadataArea(char* ptr);

PartitionPage* partitionPointerToPageNoAlignmentCheck(void* ptr);

void* partitionPageToPointer(const PartitionPage* page);

PartitionPage* partitionPointerToPage(void* ptr);

bool partitionBucketIsDirectMapped(const PartitionBucket* bucket);

size_t partitionBucketBytes(const PartitionBucket* bucket);

uint16_t partitionBucketSlots(const PartitionBucket* bucket);

size_t* partitionPageGetRawSizePtr(PartitionPage* page);

size_t partitionPageGetRawSize(PartitionPage* page);

PartitionRootBase* partitionPageToRoot(PartitionPage* page);

bool partitionPointerIsValid(void* ptr);

void* partitionBucketAlloc(PartitionRootBase* root, int flags, size_t size, PartitionBucket* bucket);

void* partitionAlloc(PartitionRoot* root, size_t size, const char* typeName);

void partitionFreeWithPage(void* ptr, PartitionPage* page);

void partitionFree(void* ptr);

PartitionBucket* partitionGenericSizeToBucket(PartitionRootGeneric* root, size_t size);

void* partitionAllocGenericFlags(PartitionRootGeneric* root, int flags, size_t size, const char* typeName);

void* partitionAllocGeneric(PartitionRootGeneric* root, size_t size, const char* typeName);

void partitionFreeGeneric(PartitionRootGeneric* root, void* ptr);

size_t partitionDirectMapSize(size_t size);

size_t partitionAllocActualSize(PartitionRootGeneric* root, size_t size);

bool partitionAllocSupportsGetSize();

size_t partitionAllocGetSize(void* ptr);

// N (or more accurately, N - sizeof(void*)) represents the largest size in
// bytes that will be handled by a SizeSpecificPartitionAllocator.
// Attempts to partitionAlloc() more than this amount will fail.
template <size_t N>
class SizeSpecificPartitionAllocator {
public:
    static const size_t kMaxAllocation = N - kAllocationGranularity;
    static const size_t kNumBuckets = N / kAllocationGranularity;
    void init() { partitionAllocInit(&m_partitionRoot, kNumBuckets, kMaxAllocation); }
    bool shutdown() { return partitionAllocShutdown(&m_partitionRoot); }
    ALWAYS_INLINE PartitionRoot* root() { return &m_partitionRoot; }
private:
    PartitionRoot m_partitionRoot;
    PartitionBucket m_actualBuckets[kNumBuckets];
};

class PartitionAllocatorGeneric {
public:
    void init() { partitionAllocGenericInit(&m_partitionRoot); }
    bool shutdown() { return partitionAllocGenericShutdown(&m_partitionRoot); }
    ALWAYS_INLINE PartitionRootGeneric* root() { return &m_partitionRoot; }
private:
    PartitionRootGeneric m_partitionRoot;
};

} // namespace WTF

using WTF::SizeSpecificPartitionAllocator;
using WTF::PartitionAllocatorGeneric;
using WTF::PartitionRoot;
using WTF::partitionAllocInit;
using WTF::partitionAllocShutdown;
using WTF::partitionAlloc;
using WTF::partitionFree;
using WTF::partitionAllocGeneric;
using WTF::partitionFreeGeneric;
using WTF::partitionReallocGeneric;
using WTF::partitionAllocActualSize;
using WTF::partitionAllocSupportsGetSize;
using WTF::partitionAllocGetSize;

#endif // WTF_PartitionAlloc_h
