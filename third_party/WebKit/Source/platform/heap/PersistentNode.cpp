// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/heap/PersistentNode.h"

namespace blink {

PersistentRegion::~PersistentRegion()
{
    PersistentNodeSlots* slots = m_slots;
    while (slots) {
        PersistentNodeSlots* deadSlots = slots;
        slots = slots->m_next;
        delete deadSlots;
    }
}

int PersistentRegion::numberOfPersistents()
{
    int persistentCount = 0;
    for (PersistentNodeSlots* slots = m_slots; slots; slots = slots->m_next) {
        for (int i = 0; i < PersistentNodeSlots::slotCount; ++i) {
            if (!slots->m_slot[i].isUnused())
                ++persistentCount;
        }
    }
    ASSERT(persistentCount == m_persistentCount);
    return persistentCount;
}

void PersistentRegion::ensurePersistentNodeSlots(void* self, TraceCallback trace)
{
    ASSERT(!m_freeListHead);
    PersistentNodeSlots* slots = new PersistentNodeSlots;
    for (int i = 0; i < PersistentNodeSlots::slotCount; ++i) {
        PersistentNode* node = &slots->m_slot[i];
        node->setFreeListNext(m_freeListHead);
        m_freeListHead = node;
        ASSERT(node->isUnused());
    }
    slots->m_next = m_slots;
    m_slots = slots;
}

// This function traces all PersistentNodes. If we encounter
// a PersistentNodeSlot that contains only freed PersistentNodes,
// we delete the PersistentNodeSlot. This function rebuilds the free
// list of PersistentNodes.
void PersistentRegion::tracePersistentNodes(Visitor* visitor)
{
    m_freeListHead = nullptr;
    int persistentCount = 0;
    PersistentNodeSlots** prevNext = &m_slots;
    PersistentNodeSlots* slots = m_slots;
    while (slots) {
        PersistentNode* freeListNext = nullptr;
        PersistentNode* freeListLast = nullptr;
        int freeCount = 0;
        for (int i = 0; i < PersistentNodeSlots::slotCount; ++i) {
            PersistentNode* node = &slots->m_slot[i];
            if (node->isUnused()) {
                if (!freeListNext)
                    freeListLast = node;
                node->setFreeListNext(freeListNext);
                freeListNext = node;
                ++freeCount;
            } else {
                node->tracePersistentNode(visitor);
                ++persistentCount;
            }
        }
        if (freeCount == PersistentNodeSlots::slotCount) {
            PersistentNodeSlots* deadSlots = slots;
            *prevNext = slots->m_next;
            slots = slots->m_next;
            delete deadSlots;
        } else {
            if (freeListLast) {
                ASSERT(freeListNext);
                ASSERT(!freeListLast->freeListNext());
                freeListLast->setFreeListNext(m_freeListHead);
                m_freeListHead = freeListNext;
            }
            prevNext = &slots->m_next;
            slots = slots->m_next;
        }
    }
    ASSERT(persistentCount == m_persistentCount);
}

} // namespace blink
