// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/v8.h"

#include "src/runtime/runtime.h"
#include "src/runtime/runtime-utils.h"

namespace v8 {
    namespace internal {

// Header of runtime functions.
#define F(name, number_of_args, result_size)                    \
  Object* Runtime_##name(int args_length, Object** args_object, \
                         Isolate* isolate);
FOR_EACH_INTRINSIC_RETURN_OBJECT(F)
#undef F

#define P(name, number_of_args, result_size)                       \
  ObjectPair Runtime_##name(int args_length, Object** args_object, \
                            Isolate* isolate);
FOR_EACH_INTRINSIC_RETURN_PAIR(P)
#undef P


#define F(name, number_of_args, result_size)                                  \
  {                                                                           \
    Runtime::k##name, Runtime::RUNTIME, #name, FUNCTION_ADDR(Runtime_##name), \
        number_of_args, result_size                                           \
  }                                                                           \
  ,


#define I(name, number_of_args, result_size)                       \
  {                                                                \
    Runtime::kInline##name, Runtime::INLINE, "_" #name,            \
        FUNCTION_ADDR(Runtime_##name), number_of_args, result_size \
  }                                                                \
  ,


static const Runtime::Function kIntrinsicFunctions[] = {
    FOR_EACH_INTRINSIC(F) FOR_EACH_INTRINSIC(I)
    //{ Runtime::kNumFunctions, Runtime::INLINE, nullptr, nullptr, 0, 0 } //
};

#undef I
#undef F


void Runtime::InitializeIntrinsicFunctionNames(Isolate* isolate,
                                               Handle<NameDictionary> dict) {
  DCHECK(dict->NumberOfElements() == 0);
  HandleScope scope(isolate);
  for (int i = 0; i < kNumFunctions; ++i) {
    const char* name = kIntrinsicFunctions[i].name;
    if (name == NULL) continue;
    Handle<NameDictionary> new_dict = NameDictionary::Add(
        dict, isolate->factory()->InternalizeUtf8String(name),
        Handle<Smi>(Smi::FromInt(i), isolate), PropertyDetails::Empty());
    // The dictionary does not need to grow.
    CHECK(new_dict.is_identical_to(dict));
  }
}


const Runtime::Function* Runtime::FunctionForName(Handle<String> name) {
  Heap* heap = name->GetHeap();
  int entry = heap->intrinsic_function_names()->FindEntry(name);
  if (entry != kNotFound) {
    Object* smi_index = heap->intrinsic_function_names()->ValueAt(entry);
    int function_index = Smi::cast(smi_index)->value();
    return &(kIntrinsicFunctions[function_index]);
  }
  return NULL;
}


const Runtime::Function* Runtime::FunctionForEntry(Address entry) {
  for (size_t i = 0; i < arraysize(kIntrinsicFunctions); ++i) {
    if (entry == kIntrinsicFunctions[i].entry) {
      return &(kIntrinsicFunctions[i]);
    }
  }
  return NULL;
}


const Runtime::Function* Runtime::FunctionForId(Runtime::FunctionId id) {
  return &(kIntrinsicFunctions[static_cast<int>(id)]);
}


std::ostream& operator<<(std::ostream& os, Runtime::FunctionId id) {
  return os << Runtime::FunctionForId(id)->name;
}

}  // namespace internal
}  // namespace v8
