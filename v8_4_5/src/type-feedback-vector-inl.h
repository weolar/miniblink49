// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_TYPE_FEEDBACK_VECTOR_INL_H_
#define V8_TYPE_FEEDBACK_VECTOR_INL_H_

#include "src/type-feedback-vector.h"

namespace v8 {
namespace internal {

int TypeFeedbackVector::ic_metadata_length() const {
  return VectorICComputer::word_count(ICSlots());
}


Handle<Object> TypeFeedbackVector::UninitializedSentinel(Isolate* isolate) {
  return isolate->factory()->uninitialized_symbol();
}


Handle<Object> TypeFeedbackVector::MegamorphicSentinel(Isolate* isolate) {
  return isolate->factory()->megamorphic_symbol();
}


Handle<Object> TypeFeedbackVector::PremonomorphicSentinel(Isolate* isolate) {
  return isolate->factory()->premonomorphic_symbol();
}


Object* TypeFeedbackVector::RawUninitializedSentinel(Heap* heap) {
  return heap->uninitialized_symbol();
}
}
}  // namespace v8::internal

#endif  // V8_TYPE_FEEDBACK_VECTOR_INL_H_
