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
#include "platform/heap/Heap.h"

#include "platform/ScriptForbiddenScope.h"
#include "platform/Task.h"
#include "platform/TraceEvent.h"
#include "platform/heap/BlinkGCMemoryDumpProvider.h"
#include "platform/heap/CallbackStack.h"
#include "platform/heap/MarkingVisitor.h"
#include "platform/heap/PageMemory.h"
#include "platform/heap/PagePool.h"
#include "platform/heap/SafePoint.h"
#include "platform/heap/ThreadState.h"
#include "public/platform/Platform.h"
#include "public/platform/WebMemoryAllocatorDump.h"
#include "public/platform/WebProcessMemoryDump.h"
#include "wtf/Assertions.h"
#include "wtf/ContainerAnnotations.h"
#include "wtf/LeakAnnotations.h"
#include "wtf/MainThread.h"
#include "wtf/PageAllocator.h"
#include "wtf/Partitions.h"
#include "wtf/PassOwnPtr.h"
#if ENABLE(GC_PROFILING)
#include "platform/TracedValue.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/StringHash.h"
#include <stdio.h>
#include <utility>
#endif

#if OS(POSIX)
#include <sys/mman.h>
#include <unistd.h>
#elif OS(WIN)
#include <windows.h>
#endif

#ifdef ANNOTATE_CONTIGUOUS_CONTAINER
// FIXME: have ContainerAnnotations.h define an ENABLE_-style name instead.
#define ENABLE_ASAN_CONTAINER_ANNOTATIONS 1

// When finalizing a non-inlined vector backing store/container, remove
// its contiguous container annotation. Required as it will not be destructed
// from its Vector.
#define ASAN_RETIRE_CONTAINER_ANNOTATION(object, objectSize)                          \
    do {                                                                              \
        BasePage* page = pageFromObject(object);                                      \
        ASSERT(page);                                                                 \
        bool isContainer = ThreadState::isVectorHeapIndex(page->heap()->heapIndex()); \
        if (!isContainer && page->isLargeObjectPage())                                \
            isContainer = static_cast<LargeObjectPage*>(page)->isVectorBackingPage(); \
        if (isContainer)                                                              \
            ANNOTATE_DELETE_BUFFER(object, objectSize, 0);                            \
    } while (0)

// A vector backing store represented by a large object is marked
// so that when it is finalized, its ASan annotation will be
// correctly retired.
#define ASAN_MARK_LARGE_VECTOR_CONTAINER(heap, largeObject)                 \
    if (ThreadState::isVectorHeapIndex(heap->heapIndex())) {                \
        BasePage* largePage = pageFromObject(largeObject);                  \
        ASSERT(largePage->isLargeObjectPage());                             \
        static_cast<LargeObjectPage*>(largePage)->setIsVectorBackingPage(); \
    }
#else
#define ENABLE_ASAN_CONTAINER_ANNOTATIONS 0
#define ASAN_RETIRE_CONTAINER_ANNOTATION(payload, payloadSize)
#define ASAN_MARK_LARGE_VECTOR_CONTAINER(heap, largeObject)
#endif

namespace blink {

#if ENABLE(GC_PROFILING)
static String classOf(const void* object)
{
    if (const GCInfo* gcInfo = Heap::findGCInfo(reinterpret_cast<Address>(const_cast<void*>(object))))
        return gcInfo->m_className;
    return "unknown";
}
#endif

class GCForbiddenScope final {
public:
    explicit GCForbiddenScope(ThreadState* state)
        : m_state(state)
    {
        // Prevent nested collectGarbage() invocations.
        m_state->enterGCForbiddenScope();
    }

    ~GCForbiddenScope()
    {
        m_state->leaveGCForbiddenScope();
    }

private:
    ThreadState* m_state;
};

class GCScope final {
public:
    GCScope(ThreadState* state, ThreadState::StackState stackState, ThreadState::GCType gcType)
        : m_state(state)
        , m_gcForbiddenScope(state)
        // See collectGarbageForTerminatingThread() comment on why a
        // safepoint scope isn't entered for its GCScope.
        , m_safePointScope(stackState, gcType != ThreadState::ThreadTerminationGC ? state : nullptr)
        , m_gcType(gcType)
        , m_parkedAllThreads(false)
    {
        TRACE_EVENT0("blink_gc", "Heap::GCScope");
#ifdef MINIBLINK_NOT_IMPLEMENTED
        const char* samplingState = TRACE_EVENT_GET_SAMPLING_STATE();

        if (m_state->isMainThread())
            TRACE_EVENT_SET_SAMPLING_STATE("blink_gc", "BlinkGCWaiting");
#endif
        ASSERT(m_state->checkThread());

        // TODO(haraken): In an unlikely coincidence that two threads decide
        // to collect garbage at the same time, avoid doing two GCs in
        // a row.
        if (LIKELY(gcType != ThreadState::ThreadTerminationGC && ThreadState::stopThreads()))
            m_parkedAllThreads = true;

        switch (gcType) {
        case ThreadState::GCWithSweep:
        case ThreadState::GCWithoutSweep:
            m_visitor = adoptPtr(new MarkingVisitor<Visitor::GlobalMarking>());
            break;
        case ThreadState::TakeSnapshot:
            m_visitor = adoptPtr(new MarkingVisitor<Visitor::SnapshotMarking>());
            break;
        case ThreadState::ThreadTerminationGC:
            m_visitor = adoptPtr(new MarkingVisitor<Visitor::ThreadLocalMarking>());
            break;
        default:
            ASSERT_NOT_REACHED();
        }
#ifdef MINIBLINK_NOT_IMPLEMENTED
        if (m_state->isMainThread())
            TRACE_EVENT_SET_NONCONST_SAMPLING_STATE(samplingState);
#endif
    }

    bool allThreadsParked() const { return m_parkedAllThreads; }
    Visitor* visitor() const { return m_visitor.get(); }

    ~GCScope()
    {
        // Only cleanup if we parked all threads in which case the GC happened
        // and we need to resume the other threads.
        if (LIKELY(m_gcType != ThreadState::ThreadTerminationGC && m_parkedAllThreads))
            ThreadState::resumeThreads();
    }

private:
    ThreadState* m_state;
    // The ordering of the two scope objects matters: GCs must first be forbidden
    // before entering the safe point scope. Prior to reaching the safe point,
    // ThreadState::runScheduledGC() is called. See its comment why we need
    // to be in a GC forbidden scope when doing so.
    GCForbiddenScope m_gcForbiddenScope;
    SafePointScope m_safePointScope;
    ThreadState::GCType m_gcType;
    OwnPtr<Visitor> m_visitor;
    bool m_parkedAllThreads; // False if we fail to park all threads
};

#if ENABLE(ASSERT)
NO_SANITIZE_ADDRESS
void HeapObjectHeader::zapMagic()
{
    ASSERT(checkHeader());
    m_magic = zappedMagic;
}
#endif

void HeapObjectHeader::finalize(Address object, size_t objectSize)
{
    const GCInfo* gcInfo = Heap::gcInfo(gcInfoIndex());
    if (gcInfo->hasFinalizer())
        gcInfo->m_finalize(object);

    ASAN_RETIRE_CONTAINER_ANNOTATION(object, objectSize);
}

BaseHeap::BaseHeap(ThreadState* state, int index)
    : m_firstPage(nullptr)
    , m_firstUnsweptPage(nullptr)
    , m_threadState(state)
    , m_index(index)
{
}

BaseHeap::~BaseHeap()
{
    ASSERT(!m_firstPage);
    ASSERT(!m_firstUnsweptPage);
}

void BaseHeap::cleanupPages()
{
    clearFreeLists();

    ASSERT(!m_firstUnsweptPage);
    // Add the BaseHeap's pages to the orphanedPagePool.
    for (BasePage* page = m_firstPage; page; page = page->next()) {
        Heap::decreaseAllocatedSpace(page->size());
        Heap::orphanedPagePool()->addOrphanedPage(heapIndex(), page);
    }
    m_firstPage = nullptr;
}

void BaseHeap::takeSnapshot(const String& dumpBaseName)
{
    // |dumpBaseName| at this point is "blink_gc/thread_X/heaps/HeapName"
    WebMemoryAllocatorDump* allocatorDump = BlinkGCMemoryDumpProvider::instance()->createMemoryAllocatorDumpForCurrentGC(dumpBaseName);
    size_t pageIndex = 0;
    size_t heapTotalFreeSize = 0;
    size_t heapTotalFreeCount = 0;
    for (BasePage* page = m_firstUnsweptPage; page; page = page->next()) {
        size_t heapPageFreeSize = 0;
        size_t heapPageFreeCount = 0;
        page->takeSnapshot(dumpBaseName, pageIndex, &heapPageFreeSize, &heapPageFreeCount);
        heapTotalFreeSize += heapPageFreeSize;
        heapTotalFreeCount += heapPageFreeCount;
        pageIndex++;
    }
    allocatorDump->AddScalar("blink_page_count", "objects", pageIndex);

    // When taking a full dump (w/ freelist), both the /buckets and /pages
    // report their free size but they are not meant to be added together.
    // Therefore, here we override the free_size of the parent heap to be
    // equal to the free_size of the sum of its heap pages.
    allocatorDump->AddScalar("free_size", "bytes", heapTotalFreeSize);
    allocatorDump->AddScalar("free_count", "objects", heapTotalFreeCount);
}

#if ENABLE(ASSERT) || ENABLE(GC_PROFILING)
BasePage* BaseHeap::findPageFromAddress(Address address)
{
    for (BasePage* page = m_firstPage; page; page = page->next()) {
        if (page->contains(address))
            return page;
    }
    for (BasePage* page = m_firstUnsweptPage; page; page = page->next()) {
        if (page->contains(address))
            return page;
    }
    return nullptr;
}
#endif

#if ENABLE(GC_PROFILING)
#define GC_PROFILE_HEAP_PAGE_SNAPSHOT_THRESHOLD 0
void BaseHeap::snapshot(TracedValue* json, ThreadState::SnapshotInfo* info)
{
    ASSERT(isConsistentForGC());
    size_t previousPageCount = info->pageCount;

    json->beginArray("pages");
    for (BasePage* page = m_firstPage; page; page = page->next(), ++info->pageCount) {
        // FIXME: To limit the size of the snapshot we only output "threshold" many page snapshots.
        if (info->pageCount < GC_PROFILE_HEAP_PAGE_SNAPSHOT_THRESHOLD) {
            json->beginArray();
            json->pushInteger(reinterpret_cast<intptr_t>(page));
            page->snapshot(json, info);
            json->endArray();
        } else {
            page->snapshot(nullptr, info);
        }
    }
    json->endArray();

    json->setInteger("pageCount", info->pageCount - previousPageCount);
}

void BaseHeap::countMarkedObjects(ClassAgeCountsMap& classAgeCounts) const
{
    for (BasePage* page = m_firstPage; page; page = page->next())
        page->countMarkedObjects(classAgeCounts);
}

void BaseHeap::countObjectsToSweep(ClassAgeCountsMap& classAgeCounts) const
{
    for (BasePage* page = m_firstPage; page; page = page->next())
        page->countObjectsToSweep(classAgeCounts);
}

void BaseHeap::incrementMarkedObjectsAge()
{
    for (BasePage* page = m_firstPage; page; page = page->next())
        page->incrementMarkedObjectsAge();
}
#endif

void BaseHeap::makeConsistentForGC()
{
    clearFreeLists();
    ASSERT(isConsistentForGC());
    for (BasePage* page = m_firstPage; page; page = page->next())
        page->markAsUnswept();

    // If a new GC is requested before this thread got around to sweep,
    // ie. due to the thread doing a long running operation, we clear
    // the mark bits and mark any of the dead objects as dead. The latter
    // is used to ensure the next GC marking does not trace already dead
    // objects. If we trace a dead object we could end up tracing into
    // garbage or the middle of another object via the newly conservatively
    // found object.
    BasePage* previousPage = nullptr;
    for (BasePage* page = m_firstUnsweptPage; page; previousPage = page, page = page->next()) {
        page->makeConsistentForGC();
        ASSERT(!page->hasBeenSwept());
    }
    if (previousPage) {
        ASSERT(m_firstUnsweptPage);
        previousPage->m_next = m_firstPage;
        m_firstPage = m_firstUnsweptPage;
        m_firstUnsweptPage = nullptr;
    }
    ASSERT(!m_firstUnsweptPage);
}

void BaseHeap::makeConsistentForMutator()
{
    clearFreeLists();
    ASSERT(isConsistentForGC());
    ASSERT(!m_firstPage);

    // Drop marks from marked objects and rebuild free lists in preparation for
    // resuming the executions of mutators.
    BasePage* previousPage = nullptr;
    for (BasePage* page = m_firstUnsweptPage; page; previousPage = page, page = page->next()) {
        page->makeConsistentForMutator();
        page->markAsSwept();
    }
    if (previousPage) {
        ASSERT(m_firstUnsweptPage);
        previousPage->m_next = m_firstPage;
        m_firstPage = m_firstUnsweptPage;
        m_firstUnsweptPage = nullptr;
    }
    ASSERT(!m_firstUnsweptPage);
}

size_t BaseHeap::objectPayloadSizeForTesting()
{
    ASSERT(isConsistentForGC());
    ASSERT(!m_firstUnsweptPage);

    size_t objectPayloadSize = 0;
    for (BasePage* page = m_firstPage; page; page = page->next())
        objectPayloadSize += page->objectPayloadSizeForTesting();
    return objectPayloadSize;
}

void BaseHeap::prepareHeapForTermination()
{
    ASSERT(!m_firstUnsweptPage);
    for (BasePage* page = m_firstPage; page; page = page->next()) {
        page->setTerminating();
    }
}

void BaseHeap::prepareForSweep()
{
    ASSERT(threadState()->isInGC());
    ASSERT(!m_firstUnsweptPage);

    // Move all pages to a list of unswept pages.
    m_firstUnsweptPage = m_firstPage;
    m_firstPage = nullptr;
}

#if defined(ADDRESS_SANITIZER)
void BaseHeap::poisonHeap(ThreadState::ObjectsToPoison objectsToPoison, ThreadState::Poisoning poisoning)
{
    // TODO(sof): support complete poisoning of all heaps.
    ASSERT(objectsToPoison != ThreadState::MarkedAndUnmarked || heapIndex() == ThreadState::EagerSweepHeapIndex);

    // This method may either be called to poison (SetPoison) heap
    // object payloads prior to sweeping, or it may be called at
    // the completion of a sweep to unpoison (ClearPoison) the
    // objects remaining in the heap. Those will all be live and unmarked.
    //
    // Poisoning may be limited to unmarked objects only, or apply to all.
    if (poisoning == ThreadState::SetPoison) {
        for (BasePage* page = m_firstUnsweptPage; page; page = page->next())
            page->poisonObjects(objectsToPoison, poisoning);
        return;
    }
    // Support clearing of poisoning after sweeping has completed,
    // in which case the pages of the live objects are reachable
    // via m_firstPage.
    ASSERT(!m_firstUnsweptPage);
    for (BasePage* page = m_firstPage; page; page = page->next())
        page->poisonObjects(objectsToPoison, poisoning);
}
#endif

Address BaseHeap::lazySweep(size_t allocationSize, size_t gcInfoIndex)
{
    // If there are no pages to be swept, return immediately.
    if (!m_firstUnsweptPage)
        return nullptr;

    RELEASE_ASSERT(threadState()->isSweepingInProgress());

    // lazySweepPages() can be called recursively if finalizers invoked in
    // page->sweep() allocate memory and the allocation triggers
    // lazySweepPages(). This check prevents the sweeping from being executed
    // recursively.
    if (threadState()->sweepForbidden())
        return nullptr;

    TRACE_EVENT0("blink_gc", "BaseHeap::lazySweepPages");
    ThreadState::SweepForbiddenScope scope(threadState());

    if (threadState()->isMainThread())
        ScriptForbiddenScope::enter();

    Address result = lazySweepPages(allocationSize, gcInfoIndex);

    if (threadState()->isMainThread())
        ScriptForbiddenScope::exit();

    Heap::reportMemoryUsageForTracing();

    return result;
}

void BaseHeap::sweepUnsweptPage()
{
    BasePage* page = m_firstUnsweptPage;
    if (page->isEmpty()) {
        page->unlink(&m_firstUnsweptPage);
        page->removeFromHeap();
    } else {
        // Sweep a page and move the page from m_firstUnsweptPages to
        // m_firstPages.
        page->sweep();
        page->unlink(&m_firstUnsweptPage);
        page->link(&m_firstPage);
        page->markAsSwept();
    }
}

bool BaseHeap::lazySweepWithDeadline(double deadlineSeconds)
{
    // It might be heavy to call Platform::current()->monotonicallyIncreasingTime()
    // per page (i.e., 128 KB sweep or one LargeObject sweep), so we check
    // the deadline per 10 pages.
    static const int deadlineCheckInterval = 10;

    RELEASE_ASSERT(threadState()->isSweepingInProgress());
    ASSERT(threadState()->sweepForbidden());
    ASSERT(!threadState()->isMainThread() || ScriptForbiddenScope::isScriptForbidden());

    int pageCount = 1;
    while (m_firstUnsweptPage) {
        sweepUnsweptPage();
        if (pageCount % deadlineCheckInterval == 0) {
            if (deadlineSeconds <= Platform::current()->monotonicallyIncreasingTime()) {
                // Deadline has come.
                Heap::reportMemoryUsageForTracing();
                return !m_firstUnsweptPage;
            }
        }
        pageCount++;
    }
    Heap::reportMemoryUsageForTracing();
    return true;
}

void BaseHeap::completeSweep()
{
    RELEASE_ASSERT(threadState()->isSweepingInProgress());
    ASSERT(threadState()->sweepForbidden());
    ASSERT(!threadState()->isMainThread() || ScriptForbiddenScope::isScriptForbidden());

    while (m_firstUnsweptPage) {
        sweepUnsweptPage();
    }

    Heap::reportMemoryUsageForTracing();
}

NormalPageHeap::NormalPageHeap(ThreadState* state, int index)
    : BaseHeap(state, index)
    , m_currentAllocationPoint(nullptr)
    , m_remainingAllocationSize(0)
    , m_lastRemainingAllocationSize(0)
    , m_promptlyFreedSize(0)
#if ENABLE(GC_PROFILING)
    , m_cumulativeAllocationSize(0)
    , m_allocationCount(0)
    , m_inlineAllocationCount(0)
#endif
{
    clearFreeLists();
}

void NormalPageHeap::clearFreeLists()
{
    setAllocationPoint(nullptr, 0);
    m_freeList.clear();
}

#if ENABLE(ASSERT)
bool NormalPageHeap::isConsistentForGC()
{
    // A thread heap is consistent for sweeping if none of the pages to be swept
    // contain a freelist block or the current allocation point.
    for (size_t i = 0; i < blinkPageSizeLog2; ++i) {
        for (FreeListEntry* freeListEntry = m_freeList.m_freeLists[i]; freeListEntry; freeListEntry = freeListEntry->next()) {
            if (pagesToBeSweptContains(freeListEntry->address()))
                return false;
        }
    }
    if (hasCurrentAllocationArea()) {
        if (pagesToBeSweptContains(currentAllocationPoint()))
            return false;
    }
    return true;
}

bool NormalPageHeap::pagesToBeSweptContains(Address address)
{
    for (BasePage* page = m_firstUnsweptPage; page; page = page->next()) {
        if (page->contains(address))
            return true;
    }
    return false;
}
#endif

void NormalPageHeap::takeFreelistSnapshot(const String& dumpName)
{
    if (m_freeList.takeSnapshot(dumpName)) {
        WebMemoryAllocatorDump* bucketsDump = BlinkGCMemoryDumpProvider::instance()->createMemoryAllocatorDumpForCurrentGC(dumpName + "/buckets");
        WebMemoryAllocatorDump* pagesDump = BlinkGCMemoryDumpProvider::instance()->createMemoryAllocatorDumpForCurrentGC(dumpName + "/pages");
        BlinkGCMemoryDumpProvider::instance()->currentProcessMemoryDump()->AddOwnershipEdge(pagesDump->guid(), bucketsDump->guid());
    }
}

#if ENABLE(GC_PROFILING)
void NormalPageHeap::snapshotFreeList(TracedValue& json)
{
    json.setInteger("cumulativeAllocationSize", m_cumulativeAllocationSize);
    json.setDouble("inlineAllocationRate", static_cast<double>(m_inlineAllocationCount) / m_allocationCount);
    json.setInteger("inlineAllocationCount", m_inlineAllocationCount);
    json.setInteger("allocationCount", m_allocationCount);
    size_t pageCount = 0;
    size_t totalPageSize = 0;
    for (NormalPage* page = static_cast<NormalPage*>(m_firstPage); page; page = static_cast<NormalPage*>(page->next())) {
        ++pageCount;
        totalPageSize += page->payloadSize();
    }
    json.setInteger("pageCount", pageCount);
    json.setInteger("totalPageSize", totalPageSize);

    FreeList::PerBucketFreeListStats bucketStats[blinkPageSizeLog2];
    size_t totalFreeSize;
    m_freeList.getFreeSizeStats(bucketStats, totalFreeSize);
    json.setInteger("totalFreeSize", totalFreeSize);

    json.beginArray("perBucketEntryCount");
    for (size_t i = 0; i < blinkPageSizeLog2; ++i)
        json.pushInteger(bucketStats[i].entryCount);
    json.endArray();

    json.beginArray("perBucketFreeSize");
    for (size_t i = 0; i < blinkPageSizeLog2; ++i)
        json.pushInteger(bucketStats[i].freeSize);
    json.endArray();
}
#endif

void NormalPageHeap::allocatePage()
{
    threadState()->shouldFlushHeapDoesNotContainCache();
    PageMemory* pageMemory = Heap::freePagePool()->takeFreePage(heapIndex());
    // We continue allocating page memory until we succeed in committing one.
    while (!pageMemory) {
        // Allocate a memory region for blinkPagesPerRegion pages that
        // will each have the following layout.
        //
        //    [ guard os page | ... payload ... | guard os page ]
        //    ^---{ aligned to blink page size }
        PageMemoryRegion* region = PageMemoryRegion::allocateNormalPages();
        threadState()->allocatedRegionsSinceLastGC().append(region);

        // Setup the PageMemory object for each of the pages in the region.
        size_t offset = 0;
        for (size_t i = 0; i < blinkPagesPerRegion; ++i) {
            PageMemory* memory = PageMemory::setupPageMemoryInRegion(region, offset, blinkPagePayloadSize());
            // Take the first possible page ensuring that this thread actually
            // gets a page and add the rest to the page pool.
            if (!pageMemory) {
                if (memory->commit())
                    pageMemory = memory;
                else
                    delete memory;
            } else {
                Heap::freePagePool()->addFreePage(heapIndex(), memory);
            }
            offset += blinkPageSize;
        }
    }
    NormalPage* page = new (pageMemory->writableStart()) NormalPage(pageMemory, this);
    page->link(&m_firstPage);

    Heap::increaseAllocatedSpace(page->size());
#if ENABLE(ASSERT) || defined(LEAK_SANITIZER) || defined(ADDRESS_SANITIZER)
    // Allow the following addToFreeList() to add the newly allocated memory
    // to the free list.
    ASAN_UNPOISON_MEMORY_REGION(page->payload(), page->payloadSize());
    Address address = page->payload();
    for (size_t i = 0; i < page->payloadSize(); i++)
        address[i] = reuseAllowedZapValue;
    ASAN_POISON_MEMORY_REGION(page->payload(), page->payloadSize());
#endif
    addToFreeList(page->payload(), page->payloadSize());
}

void NormalPageHeap::freePage(NormalPage* page)
{
    Heap::decreaseAllocatedSpace(page->size());

    if (page->terminating()) {
        // The thread is shutting down and this page is being removed as a part
        // of the thread local GC.  In that case the object could be traced in
        // the next global GC if there is a dangling pointer from a live thread
        // heap to this dead thread heap.  To guard against this, we put the
        // page into the orphaned page pool and zap the page memory.  This
        // ensures that tracing the dangling pointer in the next global GC just
        // crashes instead of causing use-after-frees.  After the next global
        // GC, the orphaned pages are removed.
        Heap::orphanedPagePool()->addOrphanedPage(heapIndex(), page);
    } else {
        PageMemory* memory = page->storage();
        page->~NormalPage();
        Heap::freePagePool()->addFreePage(heapIndex(), memory);
    }
}

bool NormalPageHeap::coalesce()
{
    // Don't coalesce heaps if there are not enough promptly freed entries
    // to be coalesced.
    //
    // FIXME: This threshold is determined just to optimize blink_perf
    // benchmarks. Coalescing is very sensitive to the threashold and
    // we need further investigations on the coalescing scheme.
    if (m_promptlyFreedSize < 1024 * 1024)
        return false;

    if (threadState()->sweepForbidden())
        return false;

    ASSERT(!hasCurrentAllocationArea());
    TRACE_EVENT0("blink_gc", "BaseHeap::coalesce");

    // Rebuild free lists.
    m_freeList.clear();
    size_t freedSize = 0;
    for (NormalPage* page = static_cast<NormalPage*>(m_firstPage); page; page = static_cast<NormalPage*>(page->next())) {
        page->clearObjectStartBitMap();
        Address startOfGap = page->payload();
        for (Address headerAddress = startOfGap; headerAddress < page->payloadEnd(); ) {
            HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(headerAddress);
            size_t size = header->size();
            ASSERT(size > 0);
            ASSERT(size < blinkPagePayloadSize());

            if (header->isPromptlyFreed()) {
                ASSERT(size >= sizeof(HeapObjectHeader));
                FILL_ZERO_IF_PRODUCTION(headerAddress, sizeof(HeapObjectHeader));
                freedSize += size;
                headerAddress += size;
                continue;
            }
            if (header->isFree()) {
                // Zero the memory in the free list header to maintain the
                // invariant that memory on the free list is zero filled.
                // The rest of the memory is already on the free list and is
                // therefore already zero filled.
                FILL_ZERO_IF_PRODUCTION(headerAddress, size < sizeof(FreeListEntry) ? size : sizeof(FreeListEntry));
                headerAddress += size;
                continue;
            }
            ASSERT(header->checkHeader());
            if (startOfGap != headerAddress)
                addToFreeList(startOfGap, headerAddress - startOfGap);

            headerAddress += size;
            startOfGap = headerAddress;
        }

        if (startOfGap != page->payloadEnd())
            addToFreeList(startOfGap, page->payloadEnd() - startOfGap);
    }
    Heap::decreaseAllocatedObjectSize(freedSize);
    ASSERT(m_promptlyFreedSize == freedSize);
    m_promptlyFreedSize = 0;
    return true;
}

void NormalPageHeap::promptlyFreeObject(HeapObjectHeader* header)
{
    ASSERT(!threadState()->sweepForbidden());
    ASSERT(header->checkHeader());
    Address address = reinterpret_cast<Address>(header);
    Address payload = header->payload();
    size_t size = header->size();
    size_t payloadSize = header->payloadSize();
    ASSERT(size > 0);
    ASSERT(pageFromObject(address) == findPageFromAddress(address));

    {
        ThreadState::SweepForbiddenScope forbiddenScope(threadState());
        header->finalize(payload, payloadSize);
        if (address + size == m_currentAllocationPoint) {
            m_currentAllocationPoint = address;
            if (m_lastRemainingAllocationSize == m_remainingAllocationSize) {
                Heap::decreaseAllocatedObjectSize(size);
                m_lastRemainingAllocationSize += size;
            }
            m_remainingAllocationSize += size;
            FILL_ZERO_IF_PRODUCTION(address, size);
            ASAN_POISON_MEMORY_REGION(address, size);
            return;
        }
        FILL_ZERO_IF_PRODUCTION(payload, payloadSize);
        header->markPromptlyFreed();
    }

    m_promptlyFreedSize += size;
}

bool NormalPageHeap::expandObject(HeapObjectHeader* header, size_t newSize)
{
    // It's possible that Vector requests a smaller expanded size because
    // Vector::shrinkCapacity can set a capacity smaller than the actual payload
    // size.
    ASSERT(header->checkHeader());
    if (header->payloadSize() >= newSize)
        return true;
    size_t allocationSize = Heap::allocationSizeFromSize(newSize);
    ASSERT(allocationSize > header->size());
    size_t expandSize = allocationSize - header->size();
    if (header->payloadEnd() == m_currentAllocationPoint && expandSize <= m_remainingAllocationSize) {
        m_currentAllocationPoint += expandSize;
        m_remainingAllocationSize -= expandSize;

        // Unpoison the memory used for the object (payload).
        ASAN_UNPOISON_MEMORY_REGION(header->payloadEnd(), expandSize);
        FILL_ZERO_IF_NOT_PRODUCTION(header->payloadEnd(), expandSize);
        header->setSize(allocationSize);
        ASSERT(findPageFromAddress(header->payloadEnd() - 1));
        return true;
    }
    return false;
}

bool NormalPageHeap::shrinkObject(HeapObjectHeader* header, size_t newSize)
{
    ASSERT(header->checkHeader());
    ASSERT(header->payloadSize() > newSize);
    size_t allocationSize = Heap::allocationSizeFromSize(newSize);
    ASSERT(header->size() > allocationSize);
    size_t shrinkSize = header->size() - allocationSize;
    if (header->payloadEnd() == m_currentAllocationPoint) {
        m_currentAllocationPoint -= shrinkSize;
        m_remainingAllocationSize += shrinkSize;
        FILL_ZERO_IF_PRODUCTION(m_currentAllocationPoint, shrinkSize);
        ASAN_POISON_MEMORY_REGION(m_currentAllocationPoint, shrinkSize);
        header->setSize(allocationSize);
        return true;
    }
    ASSERT(shrinkSize >= sizeof(HeapObjectHeader));
    ASSERT(header->gcInfoIndex() > 0);
    Address shrinkAddress = header->payloadEnd() - shrinkSize;
    FILL_ZERO_IF_PRODUCTION(shrinkAddress, shrinkSize);
    ASAN_POISON_MEMORY_REGION(shrinkAddress, shrinkSize);
    HeapObjectHeader* freedHeader = new (NotNull, shrinkAddress) HeapObjectHeader(shrinkSize, header->gcInfoIndex());
    freedHeader->markPromptlyFreed();
    ASSERT(pageFromObject(reinterpret_cast<Address>(header)) == findPageFromAddress(reinterpret_cast<Address>(header)));
    m_promptlyFreedSize += shrinkSize;
    header->setSize(allocationSize);
    return false;
}

Address NormalPageHeap::lazySweepPages(size_t allocationSize, size_t gcInfoIndex)
{
    ASSERT(!hasCurrentAllocationArea());
    Address result = nullptr;
    while (m_firstUnsweptPage) {
        BasePage* page = m_firstUnsweptPage;
        if (page->isEmpty()) {
            page->unlink(&m_firstUnsweptPage);
            page->removeFromHeap();
        } else {
            // Sweep a page and move the page from m_firstUnsweptPages to
            // m_firstPages.
            page->sweep();
            page->unlink(&m_firstUnsweptPage);
            page->link(&m_firstPage);
            page->markAsSwept();

            // For NormalPage, stop lazy sweeping once we find a slot to
            // allocate a new object.
            result = allocateFromFreeList(allocationSize, gcInfoIndex);
            if (result)
                break;
        }
    }
    return result;
}

void NormalPageHeap::updateRemainingAllocationSize()
{
    if (m_lastRemainingAllocationSize > remainingAllocationSize()) {
        Heap::increaseAllocatedObjectSize(m_lastRemainingAllocationSize - remainingAllocationSize());
        m_lastRemainingAllocationSize = remainingAllocationSize();
    }
    ASSERT(m_lastRemainingAllocationSize == remainingAllocationSize());
}

void NormalPageHeap::setAllocationPoint(Address point, size_t size)
{
#if ENABLE(ASSERT)
    if (point) {
        ASSERT(size);
        BasePage* page = pageFromObject(point);
        ASSERT(!page->isLargeObjectPage());
        ASSERT(size <= static_cast<NormalPage*>(page)->payloadSize());
    }
#endif
    if (hasCurrentAllocationArea()) {
        addToFreeList(currentAllocationPoint(), remainingAllocationSize());
    }
    updateRemainingAllocationSize();
    m_currentAllocationPoint = point;
    m_lastRemainingAllocationSize = m_remainingAllocationSize = size;
}

Address NormalPageHeap::outOfLineAllocate(size_t allocationSize, size_t gcInfoIndex)
{
    ASSERT(allocationSize > remainingAllocationSize());
    ASSERT(allocationSize >= allocationGranularity);

#if ENABLE(GC_PROFILING)
    threadState()->snapshotFreeListIfNecessary();
#endif

    // 1. If this allocation is big enough, allocate a large object.
    if (allocationSize >= largeObjectSizeThreshold) {
        // TODO(sof): support eagerly finalized large objects, if ever needed.
        RELEASE_ASSERT(heapIndex() != ThreadState::EagerSweepHeapIndex);
        LargeObjectHeap* largeObjectHeap = static_cast<LargeObjectHeap*>(threadState()->heap(ThreadState::LargeObjectHeapIndex));
        Address largeObject = largeObjectHeap->allocateLargeObjectPage(allocationSize, gcInfoIndex);
        ASAN_MARK_LARGE_VECTOR_CONTAINER(this, largeObject);
        return largeObject;
    }

    // 2. Try to allocate from a free list.
    updateRemainingAllocationSize();
    Address result = allocateFromFreeList(allocationSize, gcInfoIndex);
    if (result)
        return result;

    // 3. Reset the allocation point.
    setAllocationPoint(nullptr, 0);

    // 4. Lazily sweep pages of this heap until we find a freed area for
    // this allocation or we finish sweeping all pages of this heap.
    result = lazySweep(allocationSize, gcInfoIndex);
    if (result)
        return result;

    // 5. Coalesce promptly freed areas and then try to allocate from a free
    // list.
    if (coalesce()) {
        result = allocateFromFreeList(allocationSize, gcInfoIndex);
        if (result)
            return result;
    }

    // 6. Complete sweeping.
    threadState()->completeSweep();

    // 7. Check if we should trigger a GC.
    threadState()->scheduleGCIfNeeded();

    // 8. Add a new page to this heap.
    allocatePage();

    // 9. Try to allocate from a free list. This allocation must succeed.
    result = allocateFromFreeList(allocationSize, gcInfoIndex);
    RELEASE_ASSERT(result);
    return result;
}

Address NormalPageHeap::allocateFromFreeList(size_t allocationSize, size_t gcInfoIndex)
{
    // Try reusing a block from the largest bin. The underlying reasoning
    // being that we want to amortize this slow allocation call by carving
    // off as a large a free block as possible in one go; a block that will
    // service this block and let following allocations be serviced quickly
    // by bump allocation.
    size_t bucketSize = 1 << m_freeList.m_biggestFreeListIndex;
    int index = m_freeList.m_biggestFreeListIndex;
    for (; index > 0; --index, bucketSize >>= 1) {
        FreeListEntry* entry = m_freeList.m_freeLists[index];
        if (allocationSize > bucketSize) {
            // Final bucket candidate; check initial entry if it is able
            // to service this allocation. Do not perform a linear scan,
            // as it is considered too costly.
            if (!entry || entry->size() < allocationSize)
                break;
        }
        if (entry) {
            entry->unlink(&m_freeList.m_freeLists[index]);
            setAllocationPoint(entry->address(), entry->size());
            ASSERT(hasCurrentAllocationArea());
            ASSERT(remainingAllocationSize() >= allocationSize);
            m_freeList.m_biggestFreeListIndex = index;
            return allocateObject(allocationSize, gcInfoIndex);
        }
    }
    m_freeList.m_biggestFreeListIndex = index;
    return nullptr;
}

LargeObjectHeap::LargeObjectHeap(ThreadState* state, int index)
    : BaseHeap(state, index)
{
}

Address LargeObjectHeap::allocateLargeObjectPage(size_t allocationSize, size_t gcInfoIndex)
{
    // Caller already added space for object header and rounded up to allocation
    // alignment
    ASSERT(!(allocationSize & allocationMask));

    // 1. Try to sweep large objects more than allocationSize bytes
    // before allocating a new large object.
    Address result = lazySweep(allocationSize, gcInfoIndex);
    if (result)
        return result;

    // 2. If we have failed in sweeping allocationSize bytes,
    // we complete sweeping before allocating this large object.
    threadState()->completeSweep();

    // 3. Check if we should trigger a GC.
    threadState()->scheduleGCIfNeeded();

    return doAllocateLargeObjectPage(allocationSize, gcInfoIndex);
}

Address LargeObjectHeap::doAllocateLargeObjectPage(size_t allocationSize, size_t gcInfoIndex)
{
    size_t largeObjectSize = LargeObjectPage::pageHeaderSize() + allocationSize;
    // If ASan is supported we add allocationGranularity bytes to the allocated
    // space and poison that to detect overflows
#if defined(ADDRESS_SANITIZER)
    largeObjectSize += allocationGranularity;
#endif

    threadState()->shouldFlushHeapDoesNotContainCache();
    PageMemory* pageMemory = PageMemory::allocate(largeObjectSize);
    threadState()->allocatedRegionsSinceLastGC().append(pageMemory->region());
    Address largeObjectAddress = pageMemory->writableStart();
    Address headerAddress = largeObjectAddress + LargeObjectPage::pageHeaderSize();
#if ENABLE(ASSERT)
    // Verify that the allocated PageMemory is expectedly zeroed.
    for (size_t i = 0; i < largeObjectSize; ++i)
        ASSERT(!largeObjectAddress[i]);
#endif
    ASSERT(gcInfoIndex > 0);
    HeapObjectHeader* header = new (NotNull, headerAddress) HeapObjectHeader(largeObjectSizeInHeader, gcInfoIndex);
    Address result = headerAddress + sizeof(*header);
    ASSERT(!(reinterpret_cast<uintptr_t>(result) & allocationMask));
    LargeObjectPage* largeObject = new (largeObjectAddress) LargeObjectPage(pageMemory, this, allocationSize);
    ASSERT(header->checkHeader());

    // Poison the object header and allocationGranularity bytes after the object
    ASAN_POISON_MEMORY_REGION(header, sizeof(*header));
    ASAN_POISON_MEMORY_REGION(largeObject->address() + largeObject->size(), allocationGranularity);

    largeObject->link(&m_firstPage);

    Heap::increaseAllocatedSpace(largeObject->size());
    Heap::increaseAllocatedObjectSize(largeObject->size());
    return result;
}

void LargeObjectHeap::freeLargeObjectPage(LargeObjectPage* object)
{
    ASAN_UNPOISON_MEMORY_REGION(object->payload(), object->payloadSize());
    object->heapObjectHeader()->finalize(object->payload(), object->payloadSize());
    Heap::decreaseAllocatedSpace(object->size());

    // Unpoison the object header and allocationGranularity bytes after the
    // object before freeing.
    ASAN_UNPOISON_MEMORY_REGION(object->heapObjectHeader(), sizeof(HeapObjectHeader));
    ASAN_UNPOISON_MEMORY_REGION(object->address() + object->size(), allocationGranularity);

    if (object->terminating()) {
        ASSERT(ThreadState::current()->isTerminating());
        // The thread is shutting down and this page is being removed as a part
        // of the thread local GC.  In that case the object could be traced in
        // the next global GC if there is a dangling pointer from a live thread
        // heap to this dead thread heap.  To guard against this, we put the
        // page into the orphaned page pool and zap the page memory.  This
        // ensures that tracing the dangling pointer in the next global GC just
        // crashes instead of causing use-after-frees.  After the next global
        // GC, the orphaned pages are removed.
        Heap::orphanedPagePool()->addOrphanedPage(heapIndex(), object);
    } else {
        ASSERT(!ThreadState::current()->isTerminating());
        PageMemory* memory = object->storage();
        object->~LargeObjectPage();
        delete memory;
    }
}

Address LargeObjectHeap::lazySweepPages(size_t allocationSize, size_t gcInfoIndex)
{
    Address result = nullptr;
    size_t sweptSize = 0;
    while (m_firstUnsweptPage) {
        BasePage* page = m_firstUnsweptPage;
        if (page->isEmpty()) {
            sweptSize += static_cast<LargeObjectPage*>(page)->payloadSize() + sizeof(HeapObjectHeader);
            page->unlink(&m_firstUnsweptPage);
            page->removeFromHeap();
            // For LargeObjectPage, stop lazy sweeping once we have swept
            // more than allocationSize bytes.
            if (sweptSize >= allocationSize) {
                result = doAllocateLargeObjectPage(allocationSize, gcInfoIndex);
                ASSERT(result);
                break;
            }
        } else {
            // Sweep a page and move the page from m_firstUnsweptPages to
            // m_firstPages.
            page->sweep();
            page->unlink(&m_firstUnsweptPage);
            page->link(&m_firstPage);
            page->markAsSwept();
        }
    }
    return result;
}

FreeList::FreeList()
    : m_biggestFreeListIndex(0)
{
}

void FreeList::addToFreeList(Address address, size_t size)
{
    ASSERT(size < blinkPagePayloadSize());
    // The free list entries are only pointer aligned (but when we allocate
    // from them we are 8 byte aligned due to the header size).
    ASSERT(!((reinterpret_cast<uintptr_t>(address) + sizeof(HeapObjectHeader)) & allocationMask));
    ASSERT(!(size & allocationMask));
    ASAN_UNPOISON_MEMORY_REGION(address, size);
    FreeListEntry* entry;
    if (size < sizeof(*entry)) {
        // Create a dummy header with only a size and freelist bit set.
        ASSERT(size >= sizeof(HeapObjectHeader));
        // Free list encode the size to mark the lost memory as freelist memory.
        new (NotNull, address) HeapObjectHeader(size, gcInfoIndexForFreeListHeader);

        ASAN_POISON_MEMORY_REGION(address, size);
        // This memory gets lost. Sweeping can reclaim it.
        return;
    }
    entry = new (NotNull, address) FreeListEntry(size);

#if ENABLE(ASSERT) || defined(LEAK_SANITIZER) || defined(ADDRESS_SANITIZER)
    // The following logic delays reusing free lists for (at least) one GC
    // cycle or coalescing. This is helpful to detect use-after-free errors
    // that could be caused by lazy sweeping etc.
    size_t allowedCount = 0;
    size_t forbiddenCount = 0;
    for (size_t i = sizeof(FreeListEntry); i < size; i++) {
        if (address[i] == reuseAllowedZapValue) {
            allowedCount++;
        } else if (address[i] == reuseForbiddenZapValue) {
            forbiddenCount++;
        } else {
            ASSERT_NOT_REACHED();
        }
    }
    size_t entryCount = size - sizeof(FreeListEntry);
    if (forbiddenCount == entryCount) {
        // If all values in the memory region are reuseForbiddenZapValue,
        // we flip them to reuseAllowedZapValue. This allows the next
        // addToFreeList() to add the memory region to the free list
        // (unless someone concatenates the memory region with another memory
        // region that contains reuseForbiddenZapValue.)
        for (size_t i = sizeof(FreeListEntry); i < size; i++)
            address[i] = reuseAllowedZapValue;
        ASAN_POISON_MEMORY_REGION(address, size);
        // Don't add the memory region to the free list in this addToFreeList().
        return;
    }
    if (allowedCount != entryCount) {
        // If the memory region mixes reuseForbiddenZapValue and
        // reuseAllowedZapValue, we (conservatively) flip all the values
        // to reuseForbiddenZapValue. These values will be changed to
        // reuseAllowedZapValue in the next addToFreeList().
        for (size_t i = sizeof(FreeListEntry); i < size; i++)
            address[i] = reuseForbiddenZapValue;
        ASAN_POISON_MEMORY_REGION(address, size);
        // Don't add the memory region to the free list in this addToFreeList().
        return;
    }
    // We reach here only when all the values in the memory region are
    // reuseAllowedZapValue. In this case, we are allowed to add the memory
    // region to the free list and reuse it for another object.
#endif
    ASAN_POISON_MEMORY_REGION(address, size);

    int index = bucketIndexForSize(size);
    entry->link(&m_freeLists[index]);
    if (index > m_biggestFreeListIndex)
        m_biggestFreeListIndex = index;
}

#if ENABLE(ASSERT) || defined(LEAK_SANITIZER) || defined(ADDRESS_SANITIZER)
NO_SANITIZE_ADDRESS
void NEVER_INLINE FreeList::zapFreedMemory(Address address, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        // See the comment in addToFreeList().
        if (address[i] != reuseAllowedZapValue)
            address[i] = reuseForbiddenZapValue;
    }
}
#endif

void FreeList::clear()
{
    m_biggestFreeListIndex = 0;
    for (size_t i = 0; i < blinkPageSizeLog2; ++i)
        m_freeLists[i] = nullptr;
}

int FreeList::bucketIndexForSize(size_t size)
{
    ASSERT(size > 0);
    int index = -1;
    while (size) {
        size >>= 1;
        index++;
    }
    return index;
}

bool FreeList::takeSnapshot(const String& dumpBaseName)
{
    bool didDumpBucketStats = false;
    for (size_t i = 0; i < blinkPageSizeLog2; ++i) {
        size_t entryCount = 0;
        size_t freeSize = 0;
        for (FreeListEntry* entry = m_freeLists[i]; entry; entry = entry->next()) {
            ++entryCount;
            freeSize += entry->size();
        }

        String dumpName = dumpBaseName + String::format("/buckets/bucket_%lu", static_cast<unsigned long>(1 << i));
        WebMemoryAllocatorDump* bucketDump = BlinkGCMemoryDumpProvider::instance()->createMemoryAllocatorDumpForCurrentGC(dumpName);
        bucketDump->AddScalar("free_count", "objects", entryCount);
        bucketDump->AddScalar("free_size", "bytes", freeSize);
        didDumpBucketStats = true;
    }
    return didDumpBucketStats;
}

#if ENABLE(GC_PROFILING)
void FreeList::getFreeSizeStats(PerBucketFreeListStats bucketStats[], size_t& totalFreeSize) const
{
    totalFreeSize = 0;
    for (size_t i = 0; i < blinkPageSizeLog2; i++) {
        size_t& entryCount = bucketStats[i].entryCount;
        size_t& freeSize = bucketStats[i].freeSize;
        for (FreeListEntry* entry = m_freeLists[i]; entry; entry = entry->next()) {
            ++entryCount;
            freeSize += entry->size();
        }
        totalFreeSize += freeSize;
    }
}
#endif

BasePage::BasePage(PageMemory* storage, BaseHeap* heap)
    : m_storage(storage)
    , m_heap(heap)
    , m_next(nullptr)
    , m_terminating(false)
    , m_swept(true)
{
    ASSERT(isPageHeaderAddress(reinterpret_cast<Address>(this)));
}

void BasePage::markOrphaned()
{
    m_heap = nullptr;
    m_terminating = false;
    // Since we zap the page payload for orphaned pages we need to mark it as
    // unused so a conservative pointer won't interpret the object headers.
    storage()->markUnused();
}

NormalPage::NormalPage(PageMemory* storage, BaseHeap* heap)
    : BasePage(storage, heap)
{
    m_objectStartBitMapComputed = false;
    ASSERT(isPageHeaderAddress(reinterpret_cast<Address>(this)));
}

size_t NormalPage::objectPayloadSizeForTesting()
{
    size_t objectPayloadSize = 0;
    Address headerAddress = payload();
    markAsSwept();
    ASSERT(headerAddress != payloadEnd());
    do {
        HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(headerAddress);
        if (!header->isFree()) {
            ASSERT(header->checkHeader());
            objectPayloadSize += header->payloadSize();
        }
        ASSERT(header->size() < blinkPagePayloadSize());
        headerAddress += header->size();
        ASSERT(headerAddress <= payloadEnd());
    } while (headerAddress < payloadEnd());
    return objectPayloadSize;
}

bool NormalPage::isEmpty()
{
    HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(payload());
    return header->isFree() && header->size() == payloadSize();
}

void NormalPage::removeFromHeap()
{
    heapForNormalPage()->freePage(this);
}

void NormalPage::sweep()
{
    clearObjectStartBitMap();

    size_t markedObjectSize = 0;
    Address startOfGap = payload();
    for (Address headerAddress = startOfGap; headerAddress < payloadEnd(); ) {
        HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(headerAddress);
        ASSERT(header->size() > 0);
        ASSERT(header->size() < blinkPagePayloadSize());

        if (header->isPromptlyFreed())
            heapForNormalPage()->decreasePromptlyFreedSize(header->size());
        if (header->isFree()) {
            size_t size = header->size();
            // Zero the memory in the free list header to maintain the
            // invariant that memory on the free list is zero filled.
            // The rest of the memory is already on the free list and is
            // therefore already zero filled.
            FILL_ZERO_IF_PRODUCTION(headerAddress, size < sizeof(FreeListEntry) ? size : sizeof(FreeListEntry));
            headerAddress += size;
            continue;
        }
        ASSERT(header->checkHeader());

        if (!header->isMarked()) {
            size_t size = header->size();
            // This is a fast version of header->payloadSize().
            size_t payloadSize = size - sizeof(HeapObjectHeader);
            Address payload = header->payload();
            // For ASan we unpoison the specific object when calling the
            // finalizer and poison it again when done to allow the object's own
            // finalizer to operate on the object. Given all other unmarked
            // objects are poisoned, ASan will detect an error if the finalizer
            // touches any other on-heap object that die at the same GC cycle.
            ASAN_UNPOISON_MEMORY_REGION(payload, payloadSize);
            header->finalize(payload, payloadSize);
            // This memory will be added to the freelist. Maintain the invariant
            // that memory on the freelist is zero filled.
            FILL_ZERO_IF_PRODUCTION(headerAddress, size);
            ASAN_POISON_MEMORY_REGION(payload, payloadSize);
            headerAddress += size;
            continue;
        }
        if (startOfGap != headerAddress)
            heapForNormalPage()->addToFreeList(startOfGap, headerAddress - startOfGap);
        header->unmark();
        headerAddress += header->size();
        markedObjectSize += header->size();
        startOfGap = headerAddress;
    }
    if (startOfGap != payloadEnd())
        heapForNormalPage()->addToFreeList(startOfGap, payloadEnd() - startOfGap);

    if (markedObjectSize)
        Heap::increaseMarkedObjectSize(markedObjectSize);
}

void NormalPage::makeConsistentForGC()
{
    size_t markedObjectSize = 0;
    for (Address headerAddress = payload(); headerAddress < payloadEnd();) {
        HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(headerAddress);
        ASSERT(header->size() < blinkPagePayloadSize());
        // Check if a free list entry first since we cannot call
        // isMarked on a free list entry.
        if (header->isFree()) {
            headerAddress += header->size();
            continue;
        }
        ASSERT(header->checkHeader());
        if (header->isMarked()) {
            header->unmark();
            markedObjectSize += header->size();
        } else {
            header->markDead();
        }
        headerAddress += header->size();
    }
    if (markedObjectSize)
        Heap::increaseMarkedObjectSize(markedObjectSize);
}

void NormalPage::makeConsistentForMutator()
{
    Address startOfGap = payload();
    for (Address headerAddress = payload(); headerAddress < payloadEnd();) {
        HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(headerAddress);
        size_t size = header->size();
        ASSERT(size < blinkPagePayloadSize());
        if (header->isPromptlyFreed())
            heapForNormalPage()->decreasePromptlyFreedSize(size);
        if (header->isFree()) {
            // Zero the memory in the free list header to maintain the
            // invariant that memory on the free list is zero filled.
            // The rest of the memory is already on the free list and is
            // therefore already zero filled.
            FILL_ZERO_IF_PRODUCTION(headerAddress, size < sizeof(FreeListEntry) ? size : sizeof(FreeListEntry));
            headerAddress += size;
            continue;
        }
        ASSERT(header->checkHeader());

        if (startOfGap != headerAddress)
            heapForNormalPage()->addToFreeList(startOfGap, headerAddress - startOfGap);
        if (header->isMarked())
            header->unmark();
        headerAddress += size;
        startOfGap = headerAddress;
        ASSERT(headerAddress <= payloadEnd());
    }
    if (startOfGap != payloadEnd())
        heapForNormalPage()->addToFreeList(startOfGap, payloadEnd() - startOfGap);
}

#if defined(ADDRESS_SANITIZER)
void NormalPage::poisonObjects(ThreadState::ObjectsToPoison objectsToPoison, ThreadState::Poisoning poisoning)
{
    for (Address headerAddress = payload(); headerAddress < payloadEnd();) {
        HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(headerAddress);
        ASSERT(header->size() < blinkPagePayloadSize());
        // Check if a free list entry first since we cannot call
        // isMarked on a free list entry.
        if (header->isFree()) {
            headerAddress += header->size();
            continue;
        }
        ASSERT(header->checkHeader());
        if (objectsToPoison == ThreadState::MarkedAndUnmarked || !header->isMarked()) {
            if (poisoning == ThreadState::SetPoison)
                ASAN_POISON_MEMORY_REGION(header->payload(), header->payloadSize());
            else
                ASAN_UNPOISON_MEMORY_REGION(header->payload(), header->payloadSize());
        }
        headerAddress += header->size();
    }
}
#endif

void NormalPage::populateObjectStartBitMap()
{
    memset(&m_objectStartBitMap, 0, objectStartBitMapSize);
    Address start = payload();
    for (Address headerAddress = start; headerAddress < payloadEnd();) {
        HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(headerAddress);
        size_t objectOffset = headerAddress - start;
        ASSERT(!(objectOffset & allocationMask));
        size_t objectStartNumber = objectOffset / allocationGranularity;
        size_t mapIndex = objectStartNumber / 8;
        ASSERT(mapIndex < objectStartBitMapSize);
        m_objectStartBitMap[mapIndex] |= (1 << (objectStartNumber & 7));
        headerAddress += header->size();
        ASSERT(headerAddress <= payloadEnd());
    }
    m_objectStartBitMapComputed = true;
}

void NormalPage::clearObjectStartBitMap()
{
    m_objectStartBitMapComputed = false;
}

static int numberOfLeadingZeroes(uint8_t byte)
{
    if (!byte)
        return 8;
    int result = 0;
    if (byte <= 0x0F) {
        result += 4;
        byte = byte << 4;
    }
    if (byte <= 0x3F) {
        result += 2;
        byte = byte << 2;
    }
    if (byte <= 0x7F)
        result++;
    return result;
}

HeapObjectHeader* NormalPage::findHeaderFromAddress(Address address)
{
    if (address < payload())
        return nullptr;
    if (!isObjectStartBitMapComputed())
        populateObjectStartBitMap();
    size_t objectOffset = address - payload();
    size_t objectStartNumber = objectOffset / allocationGranularity;
    size_t mapIndex = objectStartNumber / 8;
    ASSERT(mapIndex < objectStartBitMapSize);
    size_t bit = objectStartNumber & 7;
    uint8_t byte = m_objectStartBitMap[mapIndex] & ((1 << (bit + 1)) - 1);
    while (!byte) {
        ASSERT(mapIndex > 0);
        byte = m_objectStartBitMap[--mapIndex];
    }
    int leadingZeroes = numberOfLeadingZeroes(byte);
    objectStartNumber = (mapIndex * 8) + 7 - leadingZeroes;
    objectOffset = objectStartNumber * allocationGranularity;
    Address objectAddress = objectOffset + payload();
    HeapObjectHeader* header = reinterpret_cast<HeapObjectHeader*>(objectAddress);
    if (header->isFree())
        return nullptr;
    ASSERT(header->checkHeader());
    return header;
}

#if ENABLE(ASSERT)
static bool isUninitializedMemory(void* objectPointer, size_t objectSize)
{
    // Scan through the object's fields and check that they are all zero.
    Address* objectFields = reinterpret_cast<Address*>(objectPointer);
    for (size_t i = 0; i < objectSize / sizeof(Address); ++i) {
        if (objectFields[i] != 0)
            return false;
    }
    return true;
}
#endif

static void markPointer(Visitor* visitor, HeapObjectHeader* header)
{
    ASSERT(header->checkHeader());
    const GCInfo* gcInfo = Heap::gcInfo(header->gcInfoIndex());
    if (gcInfo->hasVTable() && !vTableInitialized(header->payload())) {
        // We hit this branch when a GC strikes before GarbageCollected<>'s
        // constructor runs.
        //
        // class A : public GarbageCollected<A> { virtual void f() = 0; };
        // class B : public A {
        //   B() : A(foo()) { };
        // };
        //
        // If foo() allocates something and triggers a GC, the vtable of A
        // has not yet been initialized. In this case, we should mark the A
        // object without tracing any member of the A object.
        visitor->markHeaderNoTracing(header);
        ASSERT(isUninitializedMemory(header->payload(), header->payloadSize()));
    } else {
        visitor->markHeader(header, gcInfo->m_trace);
    }
}

void NormalPage::checkAndMarkPointer(Visitor* visitor, Address address)
{
    ASSERT(contains(address));
    HeapObjectHeader* header = findHeaderFromAddress(address);
    if (!header || header->isDead())
        return;
    markPointer(visitor, header);
}

void NormalPage::markOrphaned()
{
    // Zap the payload with a recognizable value to detect any incorrect
    // cross thread pointer usage.
#if defined(ADDRESS_SANITIZER)
    // This needs to zap poisoned memory as well.
    // Force unpoison memory before memset.
    ASAN_UNPOISON_MEMORY_REGION(payload(), payloadSize());
#endif
    OrphanedPagePool::asanDisabledMemset(payload(), OrphanedPagePool::orphanedZapValue, payloadSize());
    BasePage::markOrphaned();
}

void NormalPage::takeSnapshot(String dumpName, size_t pageIndex, size_t* outFreeSize, size_t* outFreeCount)
{
    dumpName.append(String::format("/pages/page_%lu", static_cast<unsigned long>(pageIndex)));
    WebMemoryAllocatorDump* pageDump = BlinkGCMemoryDumpProvider::instance()->createMemoryAllocatorDumpForCurrentGC(dumpName);

    HeapObjectHeader* header = nullptr;
    size_t liveCount = 0;
    size_t deadCount = 0;
    size_t freeCount = 0;
    size_t liveSize = 0;
    size_t deadSize = 0;
    size_t freeSize = 0;
    for (Address headerAddress = payload(); headerAddress < payloadEnd(); headerAddress += header->size()) {
        header = reinterpret_cast<HeapObjectHeader*>(headerAddress);
        if (header->isFree()) {
            freeCount++;
            freeSize += header->size();
        } else if (header->isMarked()) {
            liveCount++;
            liveSize += header->size();
        } else {
            deadCount++;
            deadSize += header->size();
        }
    }

    pageDump->AddScalar("live_count", "objects", liveCount);
    pageDump->AddScalar("dead_count", "objects", deadCount);
    pageDump->AddScalar("free_count", "objects", freeCount);
    pageDump->AddScalar("live_size", "bytes", liveSize);
    pageDump->AddScalar("dead_size", "bytes", deadSize);
    pageDump->AddScalar("free_size", "bytes", freeSize);
    *outFreeSize = freeSize;
    *outFreeCount = freeCount;
}

#if ENABLE(GC_PROFILING)
const GCInfo* NormalPage::findGCInfo(Address address)
{
    if (address < payload())
        return nullptr;

    HeapObjectHeader* header = findHeaderFromAddress(address);
    if (!header)
        return nullptr;

    return Heap::gcInfo(header->gcInfoIndex());
}
#endif

#if ENABLE(GC_PROFILING)
void NormalPage::snapshot(TracedValue* json, ThreadState::SnapshotInfo* info)
{
    HeapObjectHeader* header = nullptr;
    for (Address addr = payload(); addr < payloadEnd(); addr += header->size()) {
        header = reinterpret_cast<HeapObjectHeader*>(addr);
        if (json)
            json->pushInteger(header->encodedSize());
        if (header->isFree()) {
            info->freeSize += header->size();
            continue;
        }
        ASSERT(header->checkHeader());

        size_t tag = info->getClassTag(Heap::gcInfo(header->gcInfoIndex()));
        size_t age = header->age();
        if (json)
            json->pushInteger(tag);
        if (header->isMarked()) {
            info->liveCount[tag] += 1;
            info->liveSize[tag] += header->size();
            // Count objects that are live when promoted to the final generation.
            if (age == maxHeapObjectAge - 1)
                info->generations[tag][maxHeapObjectAge] += 1;
        } else {
            info->deadCount[tag] += 1;
            info->deadSize[tag] += header->size();
            // Count objects that are dead before the final generation.
            if (age < maxHeapObjectAge)
                info->generations[tag][age] += 1;
        }
    }
}

void NormalPage::incrementMarkedObjectsAge()
{
    HeapObjectHeader* header = nullptr;
    for (Address address = payload(); address < payloadEnd(); address += header->size()) {
        header = reinterpret_cast<HeapObjectHeader*>(address);
        if (header->isMarked())
            header->incrementAge();
    }
}

void NormalPage::countMarkedObjects(ClassAgeCountsMap& classAgeCounts)
{
    HeapObjectHeader* header = nullptr;
    for (Address address = payload(); address < payloadEnd(); address += header->size()) {
        header = reinterpret_cast<HeapObjectHeader*>(address);
        if (header->isMarked()) {
            String className(classOf(header->payload()));
            ++(classAgeCounts.add(className, AgeCounts()).storedValue->value.ages[header->age()]);
        }
    }
}

void NormalPage::countObjectsToSweep(ClassAgeCountsMap& classAgeCounts)
{
    HeapObjectHeader* header = nullptr;
    for (Address address = payload(); address < payloadEnd(); address += header->size()) {
        header = reinterpret_cast<HeapObjectHeader*>(address);
        if (!header->isFree() && !header->isMarked()) {
            String className(classOf(header->payload()));
            ++(classAgeCounts.add(className, AgeCounts()).storedValue->value.ages[header->age()]);
        }
    }
}
#endif

#if ENABLE(ASSERT) || ENABLE(GC_PROFILING)
bool NormalPage::contains(Address addr)
{
    Address blinkPageStart = roundToBlinkPageStart(address());
    ASSERT(blinkPageStart == address() - WTF::kSystemPageSize); // Page is at aligned address plus guard page size.
    return blinkPageStart <= addr && addr < blinkPageStart + blinkPageSize;
}
#endif

NormalPageHeap* NormalPage::heapForNormalPage()
{
    return static_cast<NormalPageHeap*>(heap());
}

LargeObjectPage::LargeObjectPage(PageMemory* storage, BaseHeap* heap, size_t payloadSize)
    : BasePage(storage, heap)
    , m_payloadSize(payloadSize)
#if ENABLE(ASAN_CONTAINER_ANNOTATIONS)
    , m_isVectorBackingPage(false)
#endif
{
}

size_t LargeObjectPage::objectPayloadSizeForTesting()
{
    markAsSwept();
    return payloadSize();
}

bool LargeObjectPage::isEmpty()
{
    return !heapObjectHeader()->isMarked();
}

void LargeObjectPage::removeFromHeap()
{
    static_cast<LargeObjectHeap*>(heap())->freeLargeObjectPage(this);
}

void LargeObjectPage::sweep()
{
    heapObjectHeader()->unmark();
    Heap::increaseMarkedObjectSize(size());
}

void LargeObjectPage::makeConsistentForGC()
{
    HeapObjectHeader* header = heapObjectHeader();
    if (header->isMarked()) {
        header->unmark();
        Heap::increaseMarkedObjectSize(size());
    } else {
        header->markDead();
    }
}

void LargeObjectPage::makeConsistentForMutator()
{
    HeapObjectHeader* header = heapObjectHeader();
    if (header->isMarked())
        header->unmark();
}

#if defined(ADDRESS_SANITIZER)
void LargeObjectPage::poisonObjects(ThreadState::ObjectsToPoison objectsToPoison, ThreadState::Poisoning poisoning)
{
    HeapObjectHeader* header = heapObjectHeader();
    if (objectsToPoison == ThreadState::MarkedAndUnmarked || !header->isMarked()) {
        if (poisoning == ThreadState::SetPoison)
            ASAN_POISON_MEMORY_REGION(header->payload(), header->payloadSize());
        else
            ASAN_UNPOISON_MEMORY_REGION(header->payload(), header->payloadSize());
    }
}
#endif

void LargeObjectPage::checkAndMarkPointer(Visitor* visitor, Address address)
{
    ASSERT(contains(address));
    if (!containedInObjectPayload(address) || heapObjectHeader()->isDead())
        return;
    markPointer(visitor, heapObjectHeader());
}

void LargeObjectPage::markOrphaned()
{
    // Zap the payload with a recognizable value to detect any incorrect
    // cross thread pointer usage.
    OrphanedPagePool::asanDisabledMemset(payload(), OrphanedPagePool::orphanedZapValue, payloadSize());
    BasePage::markOrphaned();
}

void LargeObjectPage::takeSnapshot(String dumpName, size_t pageIndex, size_t* outFreeSize, size_t* outFreeCount)
{
    dumpName.append(String::format("/pages/page_%lu", static_cast<unsigned long>(pageIndex)));
    WebMemoryAllocatorDump* pageDump = BlinkGCMemoryDumpProvider::instance()->createMemoryAllocatorDumpForCurrentGC(dumpName);

    size_t liveSize = 0;
    size_t deadSize = 0;
    size_t liveCount = 0;
    size_t deadCount = 0;
    HeapObjectHeader* header = heapObjectHeader();
    if (header->isMarked()) {
        liveCount = 1;
        liveSize += header->payloadSize();
    } else {
        deadCount = 1;
        deadSize += header->payloadSize();
    }

    pageDump->AddScalar("live_count", "objects", liveCount);
    pageDump->AddScalar("dead_count", "objects", deadCount);
    pageDump->AddScalar("live_size", "bytes", liveSize);
    pageDump->AddScalar("dead_size", "bytes", deadSize);
}

#if ENABLE(GC_PROFILING)
const GCInfo* LargeObjectPage::findGCInfo(Address address)
{
    if (!containedInObjectPayload(address))
        return nullptr;
    HeapObjectHeader* header = heapObjectHeader();
    return Heap::gcInfo(header->gcInfoIndex());
}

void LargeObjectPage::snapshot(TracedValue* json, ThreadState::SnapshotInfo* info)
{
    HeapObjectHeader* header = heapObjectHeader();
    size_t tag = info->getClassTag(Heap::gcInfo(header->gcInfoIndex()));
    size_t age = header->age();
    if (header->isMarked()) {
        info->liveCount[tag] += 1;
        info->liveSize[tag] += 0; // weolar: header->size() is bad case
        // Count objects that are live when promoted to the final generation.
        if (age == maxHeapObjectAge - 1)
            info->generations[tag][maxHeapObjectAge] += 1;
    } else {
        info->deadCount[tag] += 1;
        info->deadSize[tag] += header->size();
        // Count objects that are dead before the final generation.
        if (age < maxHeapObjectAge)
            info->generations[tag][age] += 1;
    }

    if (json) {
        json->setInteger("class", tag);
        json->setInteger("size", 0); // weolar: header->size() is bad
        json->setInteger("isMarked", header->isMarked());
    }
}

void LargeObjectPage::incrementMarkedObjectsAge()
{
    HeapObjectHeader* header = heapObjectHeader();
    if (header->isMarked())
        header->incrementAge();
}

void LargeObjectPage::countMarkedObjects(ClassAgeCountsMap& classAgeCounts)
{
    HeapObjectHeader* header = heapObjectHeader();
    if (header->isMarked()) {
        String className(classOf(header->payload()));
        ++(classAgeCounts.add(className, AgeCounts()).storedValue->value.ages[header->age()]);
    }
}

void LargeObjectPage::countObjectsToSweep(ClassAgeCountsMap& classAgeCounts)
{
    HeapObjectHeader* header = heapObjectHeader();
    if (!header->isFree() && !header->isMarked()) {
        String className(classOf(header->payload()));
        ++(classAgeCounts.add(className, AgeCounts()).storedValue->value.ages[header->age()]);
    }
}
#endif

#if ENABLE(ASSERT) || ENABLE(GC_PROFILING)
bool LargeObjectPage::contains(Address object)
{
    return roundToBlinkPageStart(address()) <= object && object < roundToBlinkPageEnd(address() + size());
}
#endif

void HeapDoesNotContainCache::flush()
{
    if (m_hasEntries) {
        for (int i = 0; i < numberOfEntries; ++i)
            m_entries[i] = nullptr;
        m_hasEntries = false;
    }
}

size_t HeapDoesNotContainCache::hash(Address address)
{
    size_t value = (reinterpret_cast<size_t>(address) >> blinkPageSizeLog2);
    value ^= value >> numberOfEntriesLog2;
    value ^= value >> (numberOfEntriesLog2 * 2);
    value &= numberOfEntries - 1;
    return value & ~1; // Returns only even number.
}

bool HeapDoesNotContainCache::lookup(Address address)
{
    ASSERT(ThreadState::current()->isInGC());

    size_t index = hash(address);
    ASSERT(!(index & 1));
    Address cachePage = roundToBlinkPageStart(address);
    if (m_entries[index] == cachePage)
        return m_entries[index];
    if (m_entries[index + 1] == cachePage)
        return m_entries[index + 1];
    return false;
}

void HeapDoesNotContainCache::addEntry(Address address)
{
    ASSERT(ThreadState::current()->isInGC());

    m_hasEntries = true;
    size_t index = hash(address);
    ASSERT(!(index & 1));
    Address cachePage = roundToBlinkPageStart(address);
    m_entries[index + 1] = m_entries[index];
    m_entries[index] = cachePage;
}

void Heap::flushHeapDoesNotContainCache()
{
    s_heapDoesNotContainCache->flush();
}

void Heap::init()
{
    ThreadState::init();
    s_markingStack = new CallbackStack();
    s_postMarkingCallbackStack = new CallbackStack();
    s_globalWeakCallbackStack = new CallbackStack();
    s_ephemeronStack = new CallbackStack();
    s_heapDoesNotContainCache = new HeapDoesNotContainCache();
    s_freePagePool = new FreePagePool();
    s_orphanedPagePool = new OrphanedPagePool();
    s_allocatedObjectSize = 0;
    s_allocatedSpace = 0;
    s_markedObjectSize = 0;
    s_estimatedMarkingTimePerByte = 0.0;

    GCInfoTable::init();

    if (Platform::current() && Platform::current()->currentThread())
        Platform::current()->registerMemoryDumpProvider(BlinkGCMemoryDumpProvider::instance());
}

void Heap::shutdown()
{
    if (Platform::current() && Platform::current()->currentThread())
        Platform::current()->unregisterMemoryDumpProvider(BlinkGCMemoryDumpProvider::instance());
    s_shutdownCalled = true;
    ThreadState::shutdownHeapIfNecessary();
}

void Heap::doShutdown()
{
    // We don't want to call doShutdown() twice.
    if (!s_markingStack)
        return;

    ASSERT(!ThreadState::attachedThreads().size());
    delete s_heapDoesNotContainCache;
    s_heapDoesNotContainCache = nullptr;
    delete s_freePagePool;
    s_freePagePool = nullptr;
    delete s_orphanedPagePool;
    s_orphanedPagePool = nullptr;
    delete s_globalWeakCallbackStack;
    s_globalWeakCallbackStack = nullptr;
    delete s_postMarkingCallbackStack;
    s_postMarkingCallbackStack = nullptr;
    delete s_markingStack;
    s_markingStack = nullptr;
    delete s_ephemeronStack;
    s_ephemeronStack = nullptr;
    delete s_regionTree;
    s_regionTree = nullptr;
    GCInfoTable::shutdown();
    ThreadState::shutdown();
    ASSERT(Heap::allocatedSpace() == 0);
}

#if ENABLE(ASSERT)
BasePage* Heap::findPageFromAddress(Address address)
{
    MutexLocker lock(ThreadState::threadAttachMutex());
    for (ThreadState* state : ThreadState::attachedThreads()) {
        if (BasePage* page = state->findPageFromAddress(address))
            return page;
    }
    return nullptr;
}
#endif

Address Heap::checkAndMarkPointer(Visitor* visitor, Address address)
{
    ASSERT(ThreadState::current()->isInGC());

#if !ENABLE(ASSERT)
    if (s_heapDoesNotContainCache->lookup(address))
        return nullptr;
#endif

    if (BasePage* page = lookup(address)) {
        ASSERT(page->contains(address));
        ASSERT(!page->orphaned());
        ASSERT(!s_heapDoesNotContainCache->lookup(address));
        page->checkAndMarkPointer(visitor, address);
        return address;
    }

#if !ENABLE(ASSERT)
    s_heapDoesNotContainCache->addEntry(address);
#else
    if (!s_heapDoesNotContainCache->lookup(address))
        s_heapDoesNotContainCache->addEntry(address);
#endif
    return nullptr;
}

#if ENABLE(GC_PROFILING)
const GCInfo* Heap::findGCInfo(Address address)
{
    return ThreadState::findGCInfoFromAllThreads(address);
}
#endif

#if ENABLE(GC_PROFILING)
String Heap::createBacktraceString()
{
    int framesToShow = 3;
    int stackFrameSize = 16;
    ASSERT(stackFrameSize >= framesToShow);
    using FramePointer = void*;
    FramePointer* stackFrame = static_cast<FramePointer*>(alloca(sizeof(FramePointer) * stackFrameSize));
    WTFGetBacktrace(stackFrame, &stackFrameSize);

    StringBuilder builder;
    builder.append("Persistent");
    bool didAppendFirstName = false;
    // Skip frames before/including "blink::Persistent".
    bool didSeePersistent = false;
    for (int i = 0; i < stackFrameSize && framesToShow > 0; ++i) {
        FrameToNameScope frameToName(stackFrame[i]);
        if (!frameToName.nullableName())
            continue;
        if (strstr(frameToName.nullableName(), "blink::Persistent")) {
            didSeePersistent = true;
            continue;
        }
        if (!didSeePersistent)
            continue;
        if (!didAppendFirstName) {
            didAppendFirstName = true;
            builder.append(" ... Backtrace:");
        }
        builder.append("\n\t");
        builder.append(frameToName.nullableName());
        --framesToShow;
    }
    return builder.toString().replace("blink::", "");
}
#endif

void Heap::pushTraceCallback(void* object, TraceCallback callback)
{
    ASSERT(ThreadState::current()->isInGC());

    // Trace should never reach an orphaned page.
    ASSERT(!Heap::orphanedPagePool()->contains(object));
    CallbackStack::Item* slot = s_markingStack->allocateEntry();
    *slot = CallbackStack::Item(object, callback);
}

bool Heap::popAndInvokeTraceCallback(Visitor* visitor)
{
    CallbackStack::Item* item = s_markingStack->pop();
    if (!item)
        return false;
    item->call(visitor);
    return true;
}

void Heap::pushPostMarkingCallback(void* object, TraceCallback callback)
{
    ASSERT(ThreadState::current()->isInGC());

    // Trace should never reach an orphaned page.
    ASSERT(!Heap::orphanedPagePool()->contains(object));
    CallbackStack::Item* slot = s_postMarkingCallbackStack->allocateEntry();
    *slot = CallbackStack::Item(object, callback);
}

bool Heap::popAndInvokePostMarkingCallback(Visitor* visitor)
{
    if (CallbackStack::Item* item = s_postMarkingCallbackStack->pop()) {
        item->call(visitor);
        return true;
    }
    return false;
}

void Heap::pushGlobalWeakCallback(void** cell, WeakCallback callback)
{
    ASSERT(ThreadState::current()->isInGC());

    // Trace should never reach an orphaned page.
    ASSERT(!Heap::orphanedPagePool()->contains(cell));
    CallbackStack::Item* slot = s_globalWeakCallbackStack->allocateEntry();
    *slot = CallbackStack::Item(cell, callback);
}

void Heap::pushThreadLocalWeakCallback(void* closure, void* object, WeakCallback callback)
{
    ASSERT(ThreadState::current()->isInGC());

    // Trace should never reach an orphaned page.
    ASSERT(!Heap::orphanedPagePool()->contains(object));
    ThreadState* state = pageFromObject(object)->heap()->threadState();
    state->pushThreadLocalWeakCallback(closure, callback);
}

bool Heap::popAndInvokeGlobalWeakCallback(Visitor* visitor)
{
    if (CallbackStack::Item* item = s_globalWeakCallbackStack->pop()) {
        item->call(visitor);
        return true;
    }
    return false;
}

void Heap::registerWeakTable(void* table, EphemeronCallback iterationCallback, EphemeronCallback iterationDoneCallback)
{
    ASSERT(ThreadState::current()->isInGC());

    // Trace should never reach an orphaned page.
    ASSERT(!Heap::orphanedPagePool()->contains(table));
    CallbackStack::Item* slot = s_ephemeronStack->allocateEntry();
    *slot = CallbackStack::Item(table, iterationCallback);

    // Register a post-marking callback to tell the tables that
    // ephemeron iteration is complete.
    pushPostMarkingCallback(table, iterationDoneCallback);
}

#if ENABLE(ASSERT)
bool Heap::weakTableRegistered(const void* table)
{
    ASSERT(s_ephemeronStack);
    return s_ephemeronStack->hasCallbackForObject(table);
}
#endif

void Heap::preGC()
{
    ASSERT(!ThreadState::current()->isInGC());
    for (ThreadState* state : ThreadState::attachedThreads())
        state->preGC();
}

void Heap::postGC(ThreadState::GCType gcType)
{
    ASSERT(ThreadState::current()->isInGC());
    for (ThreadState* state : ThreadState::attachedThreads())
        state->postGC(gcType);
}

const char* Heap::gcReasonString(GCReason reason)
{
    switch (reason) {
#define STRINGIFY_REASON(reason) case reason: return #reason;
        STRINGIFY_REASON(IdleGC);
        STRINGIFY_REASON(PreciseGC);
        STRINGIFY_REASON(ConservativeGC);
        STRINGIFY_REASON(ForcedGC);
#undef STRINGIFY_REASON
    case NumberOfGCReason: ASSERT_NOT_REACHED();
    }
    return "<Unknown>";
}

void Heap::collectGarbage(ThreadState::StackState stackState, ThreadState::GCType gcType, GCReason reason)
{
    ThreadState* state = ThreadState::current();
    // Nested collectGarbage() invocations aren't supported.
    RELEASE_ASSERT(!state->isGCForbidden());
    state->completeSweep();

    GCScope gcScope(state, stackState, gcType);
    // Check if we successfully parked the other threads.  If not we bail out of
    // the GC.
    if (!gcScope.allThreadsParked())
        return;

    if (state->isMainThread())
        ScriptForbiddenScope::enter();

    TRACE_EVENT2("blink_gc", "Heap::collectGarbage",
        "lazySweeping", gcType == ThreadState::GCWithoutSweep,
        "gcReason", gcReasonString(reason));
    TRACE_EVENT_SCOPED_SAMPLING_STATE("blink_gc", "BlinkGC");
    double timeStamp = WTF::currentTimeMS();

    if (gcType == ThreadState::TakeSnapshot)
        BlinkGCMemoryDumpProvider::instance()->clearProcessDumpForCurrentGC();

    // Disallow allocation during garbage collection (but not during the
    // finalization that happens when the gcScope is torn down).
    ThreadState::NoAllocationScope noAllocationScope(state);

    preGC();

    StackFrameDepthScope stackDepthScope;

    size_t totalObjectSize = Heap::allocatedObjectSize() + Heap::markedObjectSize();
    if (gcType != ThreadState::TakeSnapshot)
        Heap::resetHeapCounters();

    // 1. Trace persistent roots.
    ThreadState::visitPersistentRoots(gcScope.visitor());

    // 2. Trace objects reachable from the stack.  We do this independent of the
    // given stackState since other threads might have a different stack state.
    ThreadState::visitStackRoots(gcScope.visitor());

    // 3. Transitive closure to trace objects including ephemerons.
    processMarkingStack(gcScope.visitor());

    postMarkingProcessing(gcScope.visitor());
    globalWeakProcessing(gcScope.visitor());

    // Now we can delete all orphaned pages because there are no dangling
    // pointers to the orphaned pages.  (If we have such dangling pointers,
    // we should have crashed during marking before getting here.)
    orphanedPagePool()->decommitOrphanedPages();

    double markingTimeInMilliseconds = WTF::currentTimeMS() - timeStamp;
    s_estimatedMarkingTimePerByte = totalObjectSize ? (markingTimeInMilliseconds / 1000 / totalObjectSize) : 0;

    Platform::current()->histogramCustomCounts("BlinkGC.CollectGarbage", markingTimeInMilliseconds, 0, 10 * 1000, 50);
    Platform::current()->histogramCustomCounts("BlinkGC.TotalObjectSpace", Heap::allocatedObjectSize() / 1024, 0, 4 * 1024 * 1024, 50);
    Platform::current()->histogramCustomCounts("BlinkGC.TotalAllocatedSpace", Heap::allocatedSpace() / 1024, 0, 4 * 1024 * 1024, 50);
    Platform::current()->histogramEnumeration("BlinkGC.GCReason", reason, NumberOfGCReason);
    Heap::reportMemoryUsageHistogram();
    WTF::Partitions::reportMemoryUsageHistogram();

    postGC(gcType);

    if (state->isMainThread())
        ScriptForbiddenScope::exit();
}

void Heap::collectGarbageForTerminatingThread(ThreadState* state)
{
    {
        // A thread-specific termination GC must not allow other global GCs to go
        // ahead while it is running, hence the termination GC does not enter a
        // safepoint. GCScope will not enter also a safepoint scope for
        // ThreadTerminationGC.
        GCScope gcScope(state, ThreadState::NoHeapPointersOnStack, ThreadState::ThreadTerminationGC);

        ThreadState::NoAllocationScope noAllocationScope(state);

        state->preGC();
        StackFrameDepthScope stackDepthScope;

        // 1. Trace the thread local persistent roots. For thread local GCs we
        // don't trace the stack (ie. no conservative scanning) since this is
        // only called during thread shutdown where there should be no objects
        // on the stack.
        // We also assume that orphaned pages have no objects reachable from
        // persistent handles on other threads or CrossThreadPersistents.  The
        // only cases where this could happen is if a subsequent conservative
        // global GC finds a "pointer" on the stack or due to a programming
        // error where an object has a dangling cross-thread pointer to an
        // object on this heap.
        state->visitPersistents(gcScope.visitor());

        // 2. Trace objects reachable from the thread's persistent roots
        // including ephemerons.
        processMarkingStack(gcScope.visitor());

        postMarkingProcessing(gcScope.visitor());
        globalWeakProcessing(gcScope.visitor());

        state->postGC(ThreadState::GCWithSweep);
    }
    state->preSweep();
}

void Heap::processMarkingStack(Visitor* visitor)
{
    // Ephemeron fixed point loop.
    do {
        {
            // Iteratively mark all objects that are reachable from the objects
            // currently pushed onto the marking stack.
            TRACE_EVENT0("blink_gc", "Heap::processMarkingStackSingleThreaded");
            while (popAndInvokeTraceCallback(visitor)) { }
        }

        {
            // Mark any strong pointers that have now become reachable in
            // ephemeron maps.
            TRACE_EVENT0("blink_gc", "Heap::processEphemeronStack");
            s_ephemeronStack->invokeEphemeronCallbacks(visitor);
        }

        // Rerun loop if ephemeron processing queued more objects for tracing.
    } while (!s_markingStack->isEmpty());
}

void Heap::postMarkingProcessing(Visitor* visitor)
{
    TRACE_EVENT0("blink_gc", "Heap::postMarkingProcessing");
    // Call post-marking callbacks including:
    // 1. the ephemeronIterationDone callbacks on weak tables to do cleanup
    //    (specifically to clear the queued bits for weak hash tables), and
    // 2. the markNoTracing callbacks on collection backings to mark them
    //    if they are only reachable from their front objects.
    while (popAndInvokePostMarkingCallback(visitor)) { }

    s_ephemeronStack->clear();

    // Post-marking callbacks should not trace any objects and
    // therefore the marking stack should be empty after the
    // post-marking callbacks.
    ASSERT(s_markingStack->isEmpty());
}

void Heap::globalWeakProcessing(Visitor* visitor)
{
    TRACE_EVENT0("blink_gc", "Heap::globalWeakProcessing");
    // Call weak callbacks on objects that may now be pointing to dead objects.
    while (popAndInvokeGlobalWeakCallback(visitor)) { }

    // It is not permitted to trace pointers of live objects in the weak
    // callback phase, so the marking stack should still be empty here.
    ASSERT(s_markingStack->isEmpty());
}

void Heap::collectAllGarbage()
{
    // FIXME: Oilpan: we should perform a single GC and everything
    // should die. Unfortunately it is not the case for all objects
    // because the hierarchy was not completely moved to the heap and
    // some heap allocated objects own objects that contain persistents
    // pointing to other heap allocated objects.
    size_t previousLiveObjects = 0;
    for (int i = 0; i < 5; ++i) {
        collectGarbage(ThreadState::NoHeapPointersOnStack, ThreadState::GCWithSweep, ForcedGC);
        size_t liveObjects = Heap::markedObjectSize();
        if (liveObjects == previousLiveObjects)
            break;
        previousLiveObjects = liveObjects;
    }
}

double Heap::estimatedMarkingTime()
{
    ASSERT(ThreadState::current()->isMainThread());

    // Use 8 ms as initial estimated marking time.
    // 8 ms is long enough for low-end mobile devices to mark common
    // real-world object graphs.
    if (s_estimatedMarkingTimePerByte == 0)
        return 0.008;

    // Assuming that the collection rate of this GC will be mostly equal to
    // the collection rate of the last GC, estimate the marking time of this GC.
    return s_estimatedMarkingTimePerByte * (Heap::allocatedObjectSize() + Heap::markedObjectSize());
}

void Heap::reportMemoryUsageHistogram()
{
    static size_t supportedMaxSizeInMB = 4 * 1024;
    static size_t observedMaxSizeInMB = 0;

    // We only report the memory in the main thread.
    if (!isMainThread())
        return;
    // +1 is for rounding up the sizeInMB.
    size_t sizeInMB = Heap::allocatedSpace() / 1024 / 1024 + 1;
    if (sizeInMB >= supportedMaxSizeInMB)
        sizeInMB = supportedMaxSizeInMB - 1;
    if (sizeInMB > observedMaxSizeInMB) {
        // Send a UseCounter only when we see the highest memory usage
        // we've ever seen.
        Platform::current()->histogramEnumeration("BlinkGC.CommittedSize", sizeInMB, supportedMaxSizeInMB);
        observedMaxSizeInMB = sizeInMB;
    }
}

#if ENABLE(GC_PROFILING)
void Heap::reportMemoryUsageForTracing()
{
    // These values are divided by 1024 to avoid overflow in practical cases (TRACE_COUNTER values are 32-bit ints).
    // They are capped to INT_MAX just in case.
    TRACE_COUNTER1("blink_gc", "Heap::estimatedLiveObjectSizeKB", std::min(Heap::estimatedLiveObjectSize() / 1024, static_cast<size_t>(INT_MAX)));
    TRACE_COUNTER1("blink_gc", "Heap::allocatedObjectSizeKB", std::min(Heap::allocatedObjectSize() / 1024, static_cast<size_t>(INT_MAX)));
    TRACE_COUNTER1("blink_gc", "Heap::markedObjectSizeKB", std::min(Heap::markedObjectSize() / 1024, static_cast<size_t>(INT_MAX)));
    TRACE_COUNTER1("blink_gc", "Partitions::totalSizeOfCommittedPagesKB", std::min(WTF::Partitions::totalSizeOfCommittedPages() / 1024, static_cast<size_t>(INT_MAX)));
}
#endif

size_t Heap::objectPayloadSizeForTesting()
{
    size_t objectPayloadSize = 0;
    for (ThreadState* state : ThreadState::attachedThreads()) {
        state->setGCState(ThreadState::GCRunning);
        state->makeConsistentForGC();
        objectPayloadSize += state->objectPayloadSizeForTesting();
        state->setGCState(ThreadState::EagerSweepScheduled);
        state->setGCState(ThreadState::Sweeping);
        state->setGCState(ThreadState::NoGCScheduled);
    }
    return objectPayloadSize;
}

BasePage* Heap::lookup(Address address)
{
    ASSERT(ThreadState::current()->isInGC());
    if (!s_regionTree)
        return nullptr;
    if (PageMemoryRegion* region = s_regionTree->lookup(address)) {
        BasePage* page = region->pageFromAddress(address);
        return page && !page->orphaned() ? page : nullptr;
    }
    return nullptr;
}

static Mutex& regionTreeMutex()
{
    AtomicallyInitializedStaticReference(Mutex, mutex, new Mutex);
    return mutex;
}

void Heap::removePageMemoryRegion(PageMemoryRegion* region)
{
    // Deletion of large objects (and thus their regions) can happen
    // concurrently on sweeper threads.  Removal can also happen during thread
    // shutdown, but that case is safe.  Regardless, we make all removals
    // mutually exclusive.
    MutexLocker locker(regionTreeMutex());
    RegionTree::remove(region, &s_regionTree);
}

void Heap::addPageMemoryRegion(PageMemoryRegion* region)
{
    RegionTree::add(new RegionTree(region), &s_regionTree);
}

PageMemoryRegion* Heap::RegionTree::lookup(Address address)
{
    RegionTree* current = s_regionTree;
    while (current) {
        Address base = current->m_region->base();
        if (address < base) {
            current = current->m_left;
            continue;
        }
        if (address >= base + current->m_region->size()) {
            current = current->m_right;
            continue;
        }
        ASSERT(current->m_region->contains(address));
        return current->m_region;
    }
    return nullptr;
}

void Heap::RegionTree::add(RegionTree* newTree, RegionTree** context)
{
    ASSERT(newTree);
    Address base = newTree->m_region->base();
    for (RegionTree* current = *context; current; current = *context) {
        ASSERT(!current->m_region->contains(base));
        context = (base < current->m_region->base()) ? &current->m_left : &current->m_right;
    }
    *context = newTree;
}

void Heap::RegionTree::remove(PageMemoryRegion* region, RegionTree** context)
{
    ASSERT(region);
    ASSERT(context);
    Address base = region->base();
    RegionTree* current = *context;
    for (; current; current = *context) {
        if (region == current->m_region)
            break;
        context = (base < current->m_region->base()) ? &current->m_left : &current->m_right;
    }

    // Shutdown via detachMainThread might not have populated the region tree.
    if (!current)
        return;

    *context = nullptr;
    if (current->m_left) {
        add(current->m_left, context);
        current->m_left = nullptr;
    }
    if (current->m_right) {
        add(current->m_right, context);
        current->m_right = nullptr;
    }
    delete current;
}

void Heap::resetHeapCounters()
{
    ASSERT(ThreadState::current()->isInGC());

    s_allocatedObjectSize = 0;
    s_markedObjectSize = 0;
    s_externalObjectSizeAtLastGC = WTF::Partitions::totalSizeOfCommittedPages();
}

CallbackStack* Heap::s_markingStack;
CallbackStack* Heap::s_postMarkingCallbackStack;
CallbackStack* Heap::s_globalWeakCallbackStack;
CallbackStack* Heap::s_ephemeronStack;
HeapDoesNotContainCache* Heap::s_heapDoesNotContainCache;
bool Heap::s_shutdownCalled = false;
FreePagePool* Heap::s_freePagePool;
OrphanedPagePool* Heap::s_orphanedPagePool;
Heap::RegionTree* Heap::s_regionTree = nullptr;
size_t Heap::s_allocatedObjectSize = 0;
size_t Heap::s_allocatedSpace = 0;
size_t Heap::s_markedObjectSize = 0;
// We don't want to use 0 KB for the initial value because it may end up
// triggering the first GC of some thread too prematurely.
size_t Heap::s_estimatedLiveObjectSize = 512 * 1024;
size_t Heap::s_externalObjectSizeAtLastGC = 0;
double Heap::s_estimatedMarkingTimePerByte = 0.0;

} // namespace blink
