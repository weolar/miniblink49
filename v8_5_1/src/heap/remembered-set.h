// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_REMEMBERED_SET_H
#define V8_REMEMBERED_SET_H

#include "src/heap/heap.h"
#include "src/heap/slot-set.h"
#include "src/heap/spaces.h"

namespace v8 {
namespace internal {

enum PointerDirection { OLD_TO_OLD, OLD_TO_NEW };

template <PointerDirection direction>
class RememberedSet {
 public:
  // Given a page and a slot in that page, this function adds the slot to the
  // remembered set.
  static void Insert(Page* page, Address slot_addr) {
    DCHECK(page->Contains(slot_addr));
    SlotSet* slot_set = GetSlotSet(page);
    if (slot_set == nullptr) {
      slot_set = AllocateSlotSet(page);
    }
    uintptr_t offset = slot_addr - page->address();
    slot_set[offset / Page::kPageSize].Insert(offset % Page::kPageSize);
  }

  // Given a page and a slot in that page, this function removes the slot from
  // the remembered set.
  // If the slot was never added, then the function does nothing.
  static void Remove(Page* page, Address slot_addr) {
    DCHECK(page->Contains(slot_addr));
    SlotSet* slot_set = GetSlotSet(page);
    if (slot_set != nullptr) {
      uintptr_t offset = slot_addr - page->address();
      slot_set[offset / Page::kPageSize].Remove(offset % Page::kPageSize);
    }
  }

  // Given a page and a range of slots in that page, this function removes the
  // slots from the remembered set.
  static void RemoveRange(Page* page, Address start, Address end) {
    SlotSet* slot_set = GetSlotSet(page);
    if (slot_set != nullptr) {
      uintptr_t start_offset = start - page->address();
      uintptr_t end_offset = end - page->address();
      DCHECK_LT(start_offset, end_offset);
      DCHECK_LE(end_offset, static_cast<uintptr_t>(Page::kPageSize));
      slot_set->RemoveRange(static_cast<uint32_t>(start_offset),
                            static_cast<uint32_t>(end_offset));
    }
  }

  // Iterates and filters the remembered set with the given callback.
  // The callback should take (Address slot) and return SlotCallbackResult.
  template <typename Callback>
  static void Iterate(Heap* heap, Callback callback) {
    IterateMemoryChunks(
        heap, [callback](MemoryChunk* chunk) { Iterate(chunk, callback); });
  }

  // Iterates over all memory chunks that contains non-empty slot sets.
  // The callback should take (MemoryChunk* chunk) and return void.
  template <typename Callback>
  static void IterateMemoryChunks(Heap* heap, Callback callback) {
    MemoryChunkIterator it(heap, direction == OLD_TO_OLD
                                     ? MemoryChunkIterator::ALL
                                     : MemoryChunkIterator::ALL_BUT_CODE_SPACE);
    MemoryChunk* chunk;
    while ((chunk = it.next()) != nullptr) {
      SlotSet* slots = GetSlotSet(chunk);
      TypedSlotSet* typed_slots = GetTypedSlotSet(chunk);
      if (slots != nullptr || typed_slots != nullptr) {
        callback(chunk);
      }
    }
  }

  // Iterates and filters the remembered set in the given memory chunk with
  // the given callback. The callback should take (Address slot) and return
  // SlotCallbackResult.
  template <typename Callback>
  static void Iterate(MemoryChunk* chunk, Callback callback) {
    SlotSet* slots = GetSlotSet(chunk);
    if (slots != nullptr) {
      size_t pages = (chunk->size() + Page::kPageSize - 1) / Page::kPageSize;
      int new_count = 0;
      for (size_t page = 0; page < pages; page++) {
        new_count += slots[page].Iterate(callback);
      }
      if (new_count == 0) {
        ReleaseSlotSet(chunk);
      }
    }
  }

  // Iterates and filters the remembered set with the given callback.
  // The callback should take (HeapObject** slot, HeapObject* target) and
  // update the slot.
  // A special wrapper takes care of filtering the slots based on their values.
  // For OLD_TO_NEW case: slots that do not point to the ToSpace after
  // callback invocation will be removed from the set.
  template <typename Callback>
  static void IterateWithWrapper(Heap* heap, Callback callback) {
    Iterate(heap, [heap, callback](Address addr) {
      return Wrapper(heap, addr, callback);
    });
  }

  template <typename Callback>
  static void IterateWithWrapper(Heap* heap, MemoryChunk* chunk,
                                 Callback callback) {
    Iterate(chunk, [heap, callback](Address addr) {
      return Wrapper(heap, addr, callback);
    });
  }

  // Given a page and a typed slot in that page, this function adds the slot
  // to the remembered set.
  static void InsertTyped(Page* page, SlotType slot_type, Address slot_addr) {
    STATIC_ASSERT(direction == OLD_TO_OLD);
    TypedSlotSet* slot_set = page->typed_old_to_old_slots();
    if (slot_set == nullptr) {
      page->AllocateTypedOldToOldSlots();
      slot_set = page->typed_old_to_old_slots();
    }
    uintptr_t offset = slot_addr - page->address();
    DCHECK_LT(offset, static_cast<uintptr_t>(TypedSlotSet::kMaxOffset));
    slot_set->Insert(slot_type, static_cast<uint32_t>(offset));
  }

  // Given a page and a range of typed slots in that page, this function removes
  // the slots from the remembered set.
  static void RemoveRangeTyped(Page* page, Address start, Address end) {
    TypedSlotSet* slots = page->typed_old_to_old_slots();
    if (slots != nullptr) {
      slots->Iterate([start, end](SlotType slot_type, Address slot_addr) {
        return start <= slot_addr && slot_addr < end ? REMOVE_SLOT : KEEP_SLOT;
      });
    }
  }

  // Iterates and filters typed old to old pointers in the given memory chunk
  // with the given callback. The callback should take (SlotType slot_type,
  // Address slot_addr) and return SlotCallbackResult.
  template <typename Callback>
  static void IterateTyped(MemoryChunk* chunk, Callback callback) {
    TypedSlotSet* slots = chunk->typed_old_to_old_slots();
    if (slots != nullptr) {
      int new_count = slots->Iterate(callback);
      if (new_count == 0) {
        chunk->ReleaseTypedOldToOldSlots();
      }
    }
  }

  // Clear all old to old slots from the remembered set.
  static void ClearAll(Heap* heap) {
    STATIC_ASSERT(direction == OLD_TO_OLD);
    MemoryChunkIterator it(heap, MemoryChunkIterator::ALL);
    MemoryChunk* chunk;
    while ((chunk = it.next()) != nullptr) {
      chunk->ReleaseOldToOldSlots();
      chunk->ReleaseTypedOldToOldSlots();
    }
  }

  // Eliminates all stale slots from the remembered set, i.e.
  // slots that are not part of live objects anymore. This method must be
  // called after marking, when the whole transitive closure is known and
  // must be called before sweeping when mark bits are still intact.
  static void ClearInvalidSlots(Heap* heap);

  static void VerifyValidSlots(Heap* heap);

 private:
  static SlotSet* GetSlotSet(MemoryChunk* chunk) {
    if (direction == OLD_TO_OLD) {
      return chunk->old_to_old_slots();
    } else {
      return chunk->old_to_new_slots();
    }
  }

  static TypedSlotSet* GetTypedSlotSet(MemoryChunk* chunk) {
    if (direction == OLD_TO_OLD) {
      return chunk->typed_old_to_old_slots();
    } else {
      return nullptr;
    }
  }

  static void ReleaseSlotSet(MemoryChunk* chunk) {
    if (direction == OLD_TO_OLD) {
      chunk->ReleaseOldToOldSlots();
    } else {
      chunk->ReleaseOldToNewSlots();
    }
  }

  static SlotSet* AllocateSlotSet(MemoryChunk* chunk) {
    if (direction == OLD_TO_OLD) {
      chunk->AllocateOldToOldSlots();
      return chunk->old_to_old_slots();
    } else {
      chunk->AllocateOldToNewSlots();
      return chunk->old_to_new_slots();
    }
  }

  template <typename Callback>
  static SlotCallbackResult Wrapper(Heap* heap, Address slot_address,
                                    Callback slot_callback) {
    STATIC_ASSERT(direction == OLD_TO_NEW);
    Object** slot = reinterpret_cast<Object**>(slot_address);
    Object* object = *slot;
    if (heap->InFromSpace(object)) {
      HeapObject* heap_object = reinterpret_cast<HeapObject*>(object);
      DCHECK(heap_object->IsHeapObject());
      slot_callback(reinterpret_cast<HeapObject**>(slot), heap_object);
      object = *slot;
      // If the object was in from space before and is after executing the
      // callback in to space, the object is still live.
      // Unfortunately, we do not know about the slot. It could be in a
      // just freed free space object.
      if (heap->InToSpace(object)) {
        return KEEP_SLOT;
      }
    } else {
      DCHECK(!heap->InNewSpace(object));
    }
    return REMOVE_SLOT;
  }

  static bool IsValidSlot(Heap* heap, MemoryChunk* chunk, Object** slot);
};

}  // namespace internal
}  // namespace v8

#endif  // V8_REMEMBERED_SET_H
