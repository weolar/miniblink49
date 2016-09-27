// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/heap/PagePool.h"

#include "platform/heap/Heap.h"
#include "platform/heap/PageMemory.h"
#include "wtf/Assertions.h"

namespace blink {

FreePagePool::~FreePagePool()
{
    for (int index = 0; index < ThreadState::NumberOfHeaps; ++index) {
        while (PoolEntry* entry = m_pool[index]) {
            m_pool[index] = entry->next;
            PageMemory* memory = entry->data;
            ASSERT(memory);
            delete memory;
            delete entry;
        }
    }
}

void FreePagePool::addFreePage(int index, PageMemory* memory)
{
    // When adding a page to the pool we decommit it to ensure it is unused
    // while in the pool.  This also allows the physical memory, backing the
    // page, to be given back to the OS.
    memory->decommit();
    MutexLocker locker(m_mutex[index]);
    PoolEntry* entry = new PoolEntry(memory, m_pool[index]);
    m_pool[index] = entry;
}

PageMemory* FreePagePool::takeFreePage(int index)
{
    MutexLocker locker(m_mutex[index]);
    while (PoolEntry* entry = m_pool[index]) {
        m_pool[index] = entry->next;
        PageMemory* memory = entry->data;
        ASSERT(memory);
        delete entry;
        if (memory->commit())
            return memory;

        // We got some memory, but failed to commit it, try again.
        delete memory;
    }
    return nullptr;
}

OrphanedPagePool::~OrphanedPagePool()
{
    for (int index = 0; index < ThreadState::NumberOfHeaps; ++index) {
        while (PoolEntry* entry = m_pool[index]) {
            m_pool[index] = entry->next;
            BasePage* page = entry->data;
            delete entry;
            PageMemory* memory = page->storage();
            ASSERT(memory);
            page->~BasePage();
            delete memory;
        }
    }
}

void OrphanedPagePool::addOrphanedPage(int index, BasePage* page)
{
    page->markOrphaned();
    PoolEntry* entry = new PoolEntry(page, m_pool[index]);
    m_pool[index] = entry;
}

NO_SANITIZE_ADDRESS
void OrphanedPagePool::decommitOrphanedPages()
{
    ASSERT(ThreadState::current()->isInGC());

#if ENABLE(ASSERT)
    // No locking needed as all threads are at safepoints at this point in time.
    for (ThreadState* state : ThreadState::attachedThreads())
        ASSERT(state->isAtSafePoint());
#endif

    for (int index = 0; index < ThreadState::NumberOfHeaps; ++index) {
        PoolEntry* entry = m_pool[index];
        PoolEntry** prevNext = &m_pool[index];
        while (entry) {
            BasePage* page = entry->data;
            // Check if we should reuse the memory or just free it.
            // Large object memory is not reused but freed, normal blink heap
            // pages are reused.
            // NOTE: We call the destructor before freeing or adding to the
            // free page pool.
            PageMemory* memory = page->storage();
            if (page->isLargeObjectPage()) {
                page->~BasePage();
                delete memory;
            } else {
                page->~BasePage();
                clearMemory(memory);
                Heap::freePagePool()->addFreePage(index, memory);
            }

            PoolEntry* deadEntry = entry;
            entry = entry->next;
            *prevNext = entry;
            delete deadEntry;
        }
    }
}

// Make the compiler think that something is going on there.
static inline void breakOptimization(void *arg) {
#if !defined(_WIN32) || defined(__clang__)
    __asm__ __volatile__("" : : "r" (arg) : "memory");
#endif
}

NO_SANITIZE_ADDRESS
void OrphanedPagePool::asanDisabledMemset(Address address, char value, size_t size)
{
    // Don't use memset when running with ASan since this needs to zap
    // poisoned memory as well and the NO_SANITIZE_ADDRESS annotation
    // only works for code in this method and not for calls to memset.
    for (Address current = address; current < address + size; ++current) {
        breakOptimization(current);
        *current = value;
    }
}

void OrphanedPagePool::clearMemory(PageMemory* memory)
{
    asanDisabledMemset(memory->writableStart(), 0, blinkPagePayloadSize());
}

#if ENABLE(ASSERT)
bool OrphanedPagePool::contains(void* object)
{
    for (int index = 0; index < ThreadState::NumberOfHeaps; ++index) {
        for (PoolEntry* entry = m_pool[index]; entry; entry = entry->next) {
            BasePage* page = entry->data;
            if (page->contains(reinterpret_cast<Address>(object)))
                return true;
        }
    }
    return false;
}
#endif

} // namespace blink
