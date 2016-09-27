// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/heap/HeapAllocator.h"

namespace blink {

void HeapAllocator::backingFree(void* address)
{
    if (!address)
        return;

    ThreadState* state = ThreadState::current();
    if (state->sweepForbidden())
        return;
    ASSERT(!state->isInGC());

    // Don't promptly free large objects because their page is never reused.
    // Don't free backings allocated on other threads.
    BasePage* page = pageFromObject(address);
    if (page->isLargeObjectPage() || page->heap()->threadState() != state)
        return;

    HeapObjectHeader* header = HeapObjectHeader::fromPayload(address);
    ASSERT(header->checkHeader());
    NormalPageHeap* heap = static_cast<NormalPage*>(page)->heapForNormalPage();
    state->promptlyFreed(header->gcInfoIndex());
    heap->promptlyFreeObject(header);
}

void HeapAllocator::freeVectorBacking(void* address)
{
    backingFree(address);
}

void HeapAllocator::freeInlineVectorBacking(void* address)
{
    backingFree(address);
}

void HeapAllocator::freeHashTableBacking(void* address)
{
    backingFree(address);
}

bool HeapAllocator::backingExpand(void* address, size_t newSize)
{
    if (!address)
        return false;

    ThreadState* state = ThreadState::current();
    if (state->sweepForbidden())
        return false;
    ASSERT(!state->isInGC());
    ASSERT(state->isAllocationAllowed());

    // FIXME: Support expand for large objects.
    // Don't expand backings allocated on other threads.
    BasePage* page = pageFromObject(address);
    if (page->isLargeObjectPage() || page->heap()->threadState() != state)
        return false;

    HeapObjectHeader* header = HeapObjectHeader::fromPayload(address);
    ASSERT(header->checkHeader());
    NormalPageHeap* heap = static_cast<NormalPage*>(page)->heapForNormalPage();
    bool succeed = heap->expandObject(header, newSize);
    if (succeed)
        state->allocationPointAdjusted(heap->heapIndex());
    return succeed;
}

bool HeapAllocator::expandVectorBacking(void* address, size_t newSize)
{
    return backingExpand(address, newSize);
}

bool HeapAllocator::expandInlineVectorBacking(void* address, size_t newSize)
{
    return backingExpand(address, newSize);
}

bool HeapAllocator::expandHashTableBacking(void* address, size_t newSize)
{
    return backingExpand(address, newSize);
}

bool HeapAllocator::backingShrink(void* address, size_t quantizedCurrentSize, size_t quantizedShrunkSize)
{
    // We shrink the object only if the shrinking will make a non-small
    // prompt-free block.
    // FIXME: Optimize the threshold size.
    if (quantizedCurrentSize <= quantizedShrunkSize + sizeof(HeapObjectHeader) + sizeof(void*) * 32)
        return true;

    if (!address)
        return true;

    ThreadState* state = ThreadState::current();
    if (state->sweepForbidden())
        return false;
    ASSERT(!state->isInGC());
    ASSERT(state->isAllocationAllowed());

    // FIXME: Support shrink for large objects.
    // Don't shrink backings allocated on other threads.
    BasePage* page = pageFromObject(address);
    if (page->isLargeObjectPage() || page->heap()->threadState() != state)
        return false;

    HeapObjectHeader* header = HeapObjectHeader::fromPayload(address);
    ASSERT(header->checkHeader());
    NormalPageHeap* heap = static_cast<NormalPage*>(page)->heapForNormalPage();
    bool succeededAtAllocationPoint = heap->shrinkObject(header, quantizedShrunkSize);
    if (succeededAtAllocationPoint)
        state->allocationPointAdjusted(heap->heapIndex());
    return true;
}

bool HeapAllocator::shrinkVectorBacking(void* address, size_t quantizedCurrentSize, size_t quantizedShrunkSize)
{
    return backingShrink(address, quantizedCurrentSize, quantizedShrunkSize);
}

bool HeapAllocator::shrinkInlineVectorBacking(void* address, size_t quantizedCurrentSize, size_t quantizedShrunkSize)
{
    return backingShrink(address, quantizedCurrentSize, quantizedShrunkSize);
}

} // namespace blink
