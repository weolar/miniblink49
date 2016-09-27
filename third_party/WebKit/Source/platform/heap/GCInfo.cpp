// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/heap/GCInfo.h"

#include "platform/heap/Handle.h"
#include "platform/heap/Heap.h"

namespace blink {

// GCInfo indices start from 1 for heap objects, with 0 being treated
// specially as the index for freelist entries and large heap objects.
int GCInfoTable::s_gcInfoIndex = 0;

size_t GCInfoTable::s_gcInfoTableSize = 0;
GCInfo const** s_gcInfoTable = nullptr;

void GCInfoTable::ensureGCInfoIndex(const GCInfo* gcInfo, size_t* gcInfoIndexSlot)
{
    ASSERT(gcInfo);
    ASSERT(gcInfoIndexSlot);
    // Keep a global GCInfoTable lock while allocating a new slot.
    AtomicallyInitializedStaticReference(Mutex, mutex, new Mutex);
    MutexLocker locker(mutex);

    // If more than one thread ends up allocating a slot for
    // the same GCInfo, have later threads reuse the slot
    // allocated by the first.
    if (*gcInfoIndexSlot)
        return;

    int index = ++s_gcInfoIndex;
    size_t gcInfoIndex = static_cast<size_t>(index);
    RELEASE_ASSERT(gcInfoIndex < GCInfoTable::maxIndex);
    if (gcInfoIndex >= s_gcInfoTableSize)
        resize();

    s_gcInfoTable[gcInfoIndex] = gcInfo;
    releaseStore(reinterpret_cast<int*>(gcInfoIndexSlot), index);
}

void GCInfoTable::resize()
{
    static const int gcInfoZapValue = 0x33;
    // (Light) experimentation suggests that Blink doesn't need
    // more than this while handling content on popular web properties.
    const size_t initialSize = 512;

    size_t newSize = s_gcInfoTableSize ? 2 * s_gcInfoTableSize : initialSize;
    ASSERT(newSize < GCInfoTable::maxIndex);
    s_gcInfoTable = reinterpret_cast<GCInfo const**>(realloc(s_gcInfoTable, newSize * sizeof(GCInfo)));
    ASSERT(s_gcInfoTable);
    memset(reinterpret_cast<uint8_t*>(s_gcInfoTable) + s_gcInfoTableSize * sizeof(GCInfo), gcInfoZapValue, (newSize - s_gcInfoTableSize) * sizeof(GCInfo));
    s_gcInfoTableSize = newSize;
}

void GCInfoTable::init()
{
    RELEASE_ASSERT(!s_gcInfoTable);
    resize();
}

void GCInfoTable::shutdown()
{
    free(s_gcInfoTable);
    s_gcInfoTable = nullptr;
}

#if ENABLE(ASSERT)
void assertObjectHasGCInfo(const void* payload, size_t gcInfoIndex)
{
    ASSERT(HeapObjectHeader::fromPayload(payload)->checkHeader());
#if !defined(COMPONENT_BUILD)
    // On component builds we cannot compare the gcInfos as they are statically
    // defined in each of the components and hence will not match.
    BasePage* page = pageFromObject(payload);
    ASSERT(!page->orphaned());
    ASSERT(HeapObjectHeader::fromPayload(payload)->gcInfoIndex() == gcInfoIndex);
#endif
}
#endif

} // namespace blink
