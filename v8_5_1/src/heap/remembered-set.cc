// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/heap/remembered-set.h"
#include "src/heap/heap-inl.h"
#include "src/heap/heap.h"
#include "src/heap/mark-compact.h"
#include "src/heap/slot-set.h"
#include "src/heap/spaces.h"
#include "src/heap/store-buffer.h"

namespace v8 {
namespace internal {

template <PointerDirection direction>
void RememberedSet<direction>::ClearInvalidSlots(Heap* heap) {
  STATIC_ASSERT(direction == OLD_TO_NEW);
  PageIterator it(heap->old_space());
  MemoryChunk* chunk;
  while (it.has_next()) {
    chunk = it.next();
    SlotSet* slots = GetSlotSet(chunk);
    if (slots != nullptr) {
      slots->Iterate([heap, chunk](Address addr) {
        Object** slot = reinterpret_cast<Object**>(addr);
        return IsValidSlot(heap, chunk, slot) ? KEEP_SLOT : REMOVE_SLOT;
      });
    }
  }
}

template <PointerDirection direction>
void RememberedSet<direction>::VerifyValidSlots(Heap* heap) {
  Iterate(heap, [heap](Address addr) {
    HeapObject* obj =
        heap->mark_compact_collector()->FindBlackObjectBySlotSlow(addr);
    if (obj == nullptr) {
      // The slot is in dead object.
      MemoryChunk* chunk = MemoryChunk::FromAnyPointerAddress(heap, addr);
      AllocationSpace owner = chunk->owner()->identity();
      // The old to old remembered set should not have dead slots.
      CHECK_NE(direction, OLD_TO_OLD);
      // The old to new remembered set is allowed to have slots in dead
      // objects only in map and large object space because these space
      // cannot have raw untagged pointers.
      CHECK(owner == MAP_SPACE || owner == LO_SPACE);
    } else {
      int offset = static_cast<int>(addr - obj->address());
      CHECK(obj->IsValidSlot(offset));
    }
    return KEEP_SLOT;
  });
}

template <PointerDirection direction>
bool RememberedSet<direction>::IsValidSlot(Heap* heap, MemoryChunk* chunk,
                                           Object** slot) {
  STATIC_ASSERT(direction == OLD_TO_NEW);
  Object* object = *slot;
  if (!heap->InNewSpace(object)) {
    return false;
  }
  HeapObject* heap_object = HeapObject::cast(object);
  // If the target object is not black, the source slot must be part
  // of a non-black (dead) object.
  return Marking::IsBlack(Marking::MarkBitFrom(heap_object)) &&
         heap->mark_compact_collector()->IsSlotInBlackObject(
             chunk, reinterpret_cast<Address>(slot));
}

template void RememberedSet<OLD_TO_NEW>::ClearInvalidSlots(Heap* heap);
template void RememberedSet<OLD_TO_NEW>::VerifyValidSlots(Heap* heap);
template void RememberedSet<OLD_TO_OLD>::VerifyValidSlots(Heap* heap);

}  // namespace internal
}  // namespace v8
