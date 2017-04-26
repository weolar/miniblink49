// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/snapshot/startup-serializer.h"

#include "src/objects-inl.h"
#include "src/v8threads.h"

namespace v8 {
namespace internal {

StartupSerializer::StartupSerializer(
    Isolate* isolate, SnapshotByteSink* sink,
    FunctionCodeHandling function_code_handling)
    : Serializer(isolate, sink),
      function_code_handling_(function_code_handling),
      serializing_builtins_(false) {
  InitializeCodeAddressMap();
}

StartupSerializer::~StartupSerializer() {
  OutputStatistics("StartupSerializer");
}

void StartupSerializer::SerializeObject(HeapObject* obj, HowToCode how_to_code,
                                        WhereToPoint where_to_point, int skip) {
  DCHECK(!obj->IsJSFunction());

  if (function_code_handling_ == CLEAR_FUNCTION_CODE) {
    if (obj->IsCode()) {
      Code* code = Code::cast(obj);
      // If the function code is compiled (either as native code or bytecode),
      // replace it with lazy-compile builtin. Only exception is when we are
      // serializing the canonical interpreter-entry-trampoline builtin.
      if (code->kind() == Code::FUNCTION ||
          (!serializing_builtins_ && code->is_interpreter_entry_trampoline())) {
        obj = isolate()->builtins()->builtin(Builtins::kCompileLazy);
      }
    } else if (obj->IsBytecodeArray()) {
      obj = isolate()->heap()->undefined_value();
    }
  } else if (obj->IsCode()) {
    DCHECK_EQ(KEEP_FUNCTION_CODE, function_code_handling_);
    Code* code = Code::cast(obj);
    if (code->kind() == Code::FUNCTION) {
      code->ClearInlineCaches();
      code->set_profiler_ticks(0);
    }
  }

  int root_index = root_index_map_.Lookup(obj);
  // We can only encode roots as such if it has already been serialized.
  // That applies to root indices below the wave front.
  if (root_index != RootIndexMap::kInvalidRootIndex) {
    if (root_has_been_serialized_.test(root_index)) {
      PutRoot(root_index, obj, how_to_code, where_to_point, skip);
      return;
    }
  }

  if (SerializeKnownObject(obj, how_to_code, where_to_point, skip)) return;

  FlushSkip(skip);

  // Object has not yet been serialized.  Serialize it here.
  ObjectSerializer object_serializer(this, obj, sink_, how_to_code,
                                     where_to_point);
  object_serializer.Serialize();

  if (serializing_immortal_immovables_roots_ &&
      root_index != RootIndexMap::kInvalidRootIndex) {
    // Make sure that the immortal immovable root has been included in the first
    // chunk of its reserved space , so that it is deserialized onto the first
    // page of its space and stays immortal immovable.
    BackReference ref = back_reference_map_.Lookup(obj);
    CHECK(ref.is_valid() && ref.chunk_index() == 0);
  }
}

void StartupSerializer::SerializeWeakReferencesAndDeferred() {
  // This comes right after serialization of the partial snapshot, where we
  // add entries to the partial snapshot cache of the startup snapshot. Add
  // one entry with 'undefined' to terminate the partial snapshot cache.
  Object* undefined = isolate()->heap()->undefined_value();
  VisitPointer(&undefined);
  isolate()->heap()->IterateWeakRoots(this, VISIT_ALL);
  SerializeDeferredObjects();
  Pad();
}

void StartupSerializer::Synchronize(VisitorSynchronization::SyncTag tag) {
  // We expect the builtins tag after builtins have been serialized.
  DCHECK(!serializing_builtins_ || tag == VisitorSynchronization::kBuiltins);
  serializing_builtins_ = (tag == VisitorSynchronization::kHandleScope);
  sink_->Put(kSynchronize, "Synchronize");
}

void StartupSerializer::SerializeStrongReferences() {
  Isolate* isolate = this->isolate();
  // No active threads.
  CHECK_NULL(isolate->thread_manager()->FirstThreadStateInUse());
  // No active or weak handles.
  CHECK(isolate->handle_scope_implementer()->blocks()->is_empty());
  CHECK_EQ(0, isolate->global_handles()->NumberOfWeakHandles());
  CHECK_EQ(0, isolate->eternal_handles()->NumberOfHandles());
  // We don't support serializing installed extensions.
  CHECK(!isolate->has_installed_extensions());
  // First visit immortal immovables to make sure they end up in the first page.
  serializing_immortal_immovables_roots_ = true;
  isolate->heap()->IterateStrongRoots(this, VISIT_ONLY_STRONG_ROOT_LIST);
  // Check that immortal immovable roots are allocated on the first page.
  CHECK(HasNotExceededFirstPageOfEachSpace());
  serializing_immortal_immovables_roots_ = false;
  // Visit the rest of the strong roots.
  // Clear the stack limits to make the snapshot reproducible.
  // Reset it again afterwards.
  isolate->heap()->ClearStackLimits();
  isolate->heap()->IterateSmiRoots(this);
  isolate->heap()->SetStackLimits();

  isolate->heap()->IterateStrongRoots(this,
                                      VISIT_ONLY_STRONG_FOR_SERIALIZATION);
}

void StartupSerializer::VisitPointers(Object** start, Object** end) {
  if (start == isolate()->heap()->roots_array_start()) {
    // Serializing the root list needs special handling:
    // - The first pass over the root list only serializes immortal immovables.
    // - The second pass over the root list serializes the rest.
    // - Only root list elements that have been fully serialized can be
    //   referenced via as root by using kRootArray bytecodes.
    int skip = 0;
    for (Object** current = start; current < end; current++) {
      int root_index = static_cast<int>(current - start);
      if (RootShouldBeSkipped(root_index)) {
        skip += kPointerSize;
        continue;
      } else {
        if ((*current)->IsSmi()) {
          FlushSkip(skip);
          PutSmi(Smi::cast(*current));
        } else {
          SerializeObject(HeapObject::cast(*current), kPlain, kStartOfObject,
                          skip);
        }
        root_has_been_serialized_.set(root_index);
        skip = 0;
      }
    }
    FlushSkip(skip);
  } else {
    Serializer::VisitPointers(start, end);
  }
}

bool StartupSerializer::RootShouldBeSkipped(int root_index) {
  if (root_index == Heap::kStackLimitRootIndex ||
      root_index == Heap::kRealStackLimitRootIndex) {
    return true;
  }
  return Heap::RootIsImmortalImmovable(root_index) !=
         serializing_immortal_immovables_roots_;
}

}  // namespace internal
}  // namespace v8
