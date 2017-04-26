// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/v8.h"

#include "src/api.h"
#include "src/api-natives.h"
#include "src/arguments.h"
#include "src/base/once.h"
#include "src/bootstrapper.h"
#include "src/builtins.h"
#include "src/cpu-profiler.h"
#include "src/elements.h"
#include "src/gdb-jit.h"
#include "src/heap/mark-compact.h"
#include "src/heap-profiler.h"
#include "src/ic/handler-compiler.h"
#include "src/ic/ic.h"
#include "src/messages.h"
#include "src/prototype.h"
#include "src/vm-state-inl.h"

namespace v8 {
namespace internal {

namespace {

// Arguments object passed to C++ builtins.
template <BuiltinExtraArguments extra_args>
class BuiltinArguments : public Arguments {
 public:
  BuiltinArguments(int length, Object** arguments)
      : Arguments(length, arguments) { }

  Object*& operator[] (int index) {
    DCHECK(index < length());
    return Arguments::operator[](index);
  }

  template <class S> Handle<S> at(int index) {
    DCHECK(index < length());
    return Arguments::at<S>(index);
  }

  Handle<Object> receiver() {
    return Arguments::at<Object>(0);
  }

  Handle<JSFunction> called_function() {
    STATIC_ASSERT(extra_args == NEEDS_CALLED_FUNCTION);
    return Arguments::at<JSFunction>(Arguments::length() - 1);
  }

  // Gets the total number of arguments including the receiver (but
  // excluding extra arguments).
  int length() const {
    STATIC_ASSERT(extra_args == NO_EXTRA_ARGUMENTS);
    return Arguments::length();
  }

#ifdef DEBUG
  void Verify() {
    // Check we have at least the receiver.
    DCHECK(Arguments::length() >= 1);
  }
#endif
};


// Specialize BuiltinArguments for the called function extra argument.

template <>
int BuiltinArguments<NEEDS_CALLED_FUNCTION>::length() const {
  return Arguments::length() - 1;
}

#ifdef DEBUG
template <>
void BuiltinArguments<NEEDS_CALLED_FUNCTION>::Verify() {
  // Check we have at least the receiver and the called function.
  DCHECK(Arguments::length() >= 2);
  // Make sure cast to JSFunction succeeds.
  called_function();
}
#endif


#define DEF_ARG_TYPE(name, spec)                      \
  typedef BuiltinArguments<spec> name##ArgumentsType;
BUILTIN_LIST_C(DEF_ARG_TYPE)
#undef DEF_ARG_TYPE

}  // namespace

// ----------------------------------------------------------------------------
// Support macro for defining builtins in C++.
// ----------------------------------------------------------------------------
//
// A builtin function is defined by writing:
//
//   BUILTIN(name) {
//     ...
//   }
//
// In the body of the builtin function the arguments can be accessed
// through the BuiltinArguments object args.

#ifdef DEBUG

#define BUILTIN(name)                                            \
  MUST_USE_RESULT static Object* Builtin_Impl_##name(            \
      name##ArgumentsType args, Isolate* isolate);               \
  MUST_USE_RESULT static Object* Builtin_##name(                 \
      int args_length, Object** args_object, Isolate* isolate) { \
    name##ArgumentsType args(args_length, args_object);          \
    args.Verify();                                               \
    return Builtin_Impl_##name(args, isolate);                   \
  }                                                              \
  MUST_USE_RESULT static Object* Builtin_Impl_##name(            \
      name##ArgumentsType args, Isolate* isolate)

#else  // For release mode.

#define BUILTIN(name)                                            \
  static Object* Builtin_impl##name(                             \
      name##ArgumentsType args, Isolate* isolate);               \
  static Object* Builtin_##name(                                 \
      int args_length, Object** args_object, Isolate* isolate) { \
    name##ArgumentsType args(args_length, args_object);          \
    return Builtin_impl##name(args, isolate);                    \
  }                                                              \
  static Object* Builtin_impl##name(                             \
      name##ArgumentsType args, Isolate* isolate)
#endif


#ifdef DEBUG
static inline bool CalledAsConstructor(Isolate* isolate) {
  // Calculate the result using a full stack frame iterator and check
  // that the state of the stack is as we assume it to be in the
  // code below.
  StackFrameIterator it(isolate);
  DCHECK(it.frame()->is_exit());
  it.Advance();
  StackFrame* frame = it.frame();
  bool reference_result = frame->is_construct();
  Address fp = Isolate::c_entry_fp(isolate->thread_local_top());
  // Because we know fp points to an exit frame we can use the relevant
  // part of ExitFrame::ComputeCallerState directly.
  const int kCallerOffset = ExitFrameConstants::kCallerFPOffset;
  Address caller_fp = Memory::Address_at(fp + kCallerOffset);
  // This inlines the part of StackFrame::ComputeType that grabs the
  // type of the current frame.  Note that StackFrame::ComputeType
  // has been specialized for each architecture so if any one of them
  // changes this code has to be changed as well.
  const int kMarkerOffset = StandardFrameConstants::kMarkerOffset;
  const Smi* kConstructMarker = Smi::FromInt(StackFrame::CONSTRUCT);
  Object* marker = Memory::Object_at(caller_fp + kMarkerOffset);
  bool result = (marker == kConstructMarker);
  DCHECK_EQ(result, reference_result);
  return result;
}
#endif


// ----------------------------------------------------------------------------

BUILTIN(Illegal) {
  UNREACHABLE();
  return isolate->heap()->undefined_value();  // Make compiler happy.
}


BUILTIN(EmptyFunction) {
  return isolate->heap()->undefined_value();
}


static void MoveDoubleElements(FixedDoubleArray* dst, int dst_index,
                               FixedDoubleArray* src, int src_index, int len) {
  if (len == 0) return;
  MemMove(dst->data_start() + dst_index, src->data_start() + src_index,
          len * kDoubleSize);
}


static bool ArrayPrototypeHasNoElements(PrototypeIterator* iter) {
  DisallowHeapAllocation no_gc;
  for (; !iter->IsAtEnd(); iter->Advance()) {
    if (iter->GetCurrent()->IsJSProxy()) return false;
    JSObject* current = JSObject::cast(iter->GetCurrent());
    if (current->IsAccessCheckNeeded()) return false;
    if (current->HasIndexedInterceptor()) return false;
    if (current->elements()->length() != 0) return false;
  }
  return true;
}


static inline bool IsJSArrayFastElementMovingAllowed(Isolate* isolate,
                                                     JSArray* receiver) {
  DisallowHeapAllocation no_gc;
  // If the array prototype chain is intact (and free of elements), and if the
  // receiver's prototype is the array prototype, then we are done.
  Object* prototype = receiver->map()->prototype();
  if (prototype->IsJSArray() &&
      isolate->is_initial_array_prototype(JSArray::cast(prototype)) &&
      isolate->IsFastArrayConstructorPrototypeChainIntact()) {
    return true;
  }

  // Slow case.
  PrototypeIterator iter(isolate, receiver);
  return ArrayPrototypeHasNoElements(&iter);
}


// Returns empty handle if not applicable.
MUST_USE_RESULT
static inline MaybeHandle<FixedArrayBase> EnsureJSArrayWithWritableFastElements(
    Isolate* isolate,
    Handle<Object> receiver,
    Arguments* args,
    int first_added_arg) {
  if (!receiver->IsJSArray()) return MaybeHandle<FixedArrayBase>();
  Handle<JSArray> array = Handle<JSArray>::cast(receiver);
  // If there may be elements accessors in the prototype chain, the fast path
  // cannot be used if there arguments to add to the array.
  Heap* heap = isolate->heap();
  if (args != NULL && !IsJSArrayFastElementMovingAllowed(isolate, *array)) {
    return MaybeHandle<FixedArrayBase>();
  }
  if (array->map()->is_observed()) return MaybeHandle<FixedArrayBase>();
  if (!array->map()->is_extensible()) return MaybeHandle<FixedArrayBase>();
  Handle<FixedArrayBase> elms(array->elements(), isolate);
  Map* map = elms->map();
  if (map == heap->fixed_array_map()) {
    if (args == NULL || array->HasFastObjectElements()) return elms;
  } else if (map == heap->fixed_cow_array_map()) {
    elms = JSObject::EnsureWritableFastElements(array);
    if (args == NULL || array->HasFastObjectElements()) return elms;
  } else if (map == heap->fixed_double_array_map()) {
    if (args == NULL) return elms;
  } else {
    return MaybeHandle<FixedArrayBase>();
  }

  // Adding elements to the array prototype would break code that makes sure
  // it has no elements. Handle that elsewhere.
  if (isolate->IsAnyInitialArrayPrototype(array)) {
    return MaybeHandle<FixedArrayBase>();
  }

  // Need to ensure that the arguments passed in args can be contained in
  // the array.
  int args_length = args->length();
  if (first_added_arg >= args_length) return handle(array->elements(), isolate);

  ElementsKind origin_kind = array->map()->elements_kind();
  DCHECK(!IsFastObjectElementsKind(origin_kind));
  ElementsKind target_kind = origin_kind;
  {
    DisallowHeapAllocation no_gc;
    int arg_count = args->length() - first_added_arg;
    Object** arguments = args->arguments() - first_added_arg - (arg_count - 1);
    for (int i = 0; i < arg_count; i++) {
      Object* arg = arguments[i];
      if (arg->IsHeapObject()) {
        if (arg->IsHeapNumber()) {
          target_kind = FAST_DOUBLE_ELEMENTS;
        } else {
          target_kind = FAST_ELEMENTS;
          break;
        }
      }
    }
  }
  if (target_kind != origin_kind) {
    JSObject::TransitionElementsKind(array, target_kind);
    return handle(array->elements(), isolate);
  }
  return elms;
}


MUST_USE_RESULT static Object* CallJsBuiltin(
    Isolate* isolate,
    const char* name,
    BuiltinArguments<NO_EXTRA_ARGUMENTS> args) {
  HandleScope handleScope(isolate);

  Handle<Object> js_builtin = Object::GetProperty(
      isolate,
      handle(isolate->native_context()->builtins(), isolate),
      name).ToHandleChecked();
  Handle<JSFunction> function = Handle<JSFunction>::cast(js_builtin);
  int argc = args.length() - 1;
  ScopedVector<Handle<Object> > argv(argc);
  for (int i = 0; i < argc; ++i) {
    argv[i] = args.at<Object>(i + 1);
  }
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Execution::Call(isolate,
                      function,
                      args.receiver(),
                      argc,
                      argv.start()));
  return *result;
}


BUILTIN(ArrayPush) {
  HandleScope scope(isolate);
  Handle<Object> receiver = args.receiver();
  MaybeHandle<FixedArrayBase> maybe_elms_obj =
      EnsureJSArrayWithWritableFastElements(isolate, receiver, &args, 1);
  Handle<FixedArrayBase> elms_obj;
  if (!maybe_elms_obj.ToHandle(&elms_obj)) {
    return CallJsBuiltin(isolate, "$arrayPush", args);
  }

  Handle<JSArray> array = Handle<JSArray>::cast(receiver);
  int len = Smi::cast(array->length())->value();
  int to_add = args.length() - 1;
  if (to_add > 0 && JSArray::WouldChangeReadOnlyLength(array, len + to_add)) {
    return CallJsBuiltin(isolate, "$arrayPush", args);
  }
  DCHECK(!array->map()->is_observed());

  ElementsKind kind = array->GetElementsKind();

  if (IsFastSmiOrObjectElementsKind(kind)) {
    Handle<FixedArray> elms = Handle<FixedArray>::cast(elms_obj);
    if (to_add == 0) {
      return Smi::FromInt(len);
    }
    // Currently fixed arrays cannot grow too big, so
    // we should never hit this case.
    DCHECK(to_add <= (Smi::kMaxValue - len));

    int new_length = len + to_add;

    if (new_length > elms->length()) {
      // New backing storage is needed.
      int capacity = new_length + (new_length >> 1) + 16;
      Handle<FixedArray> new_elms =
          isolate->factory()->NewUninitializedFixedArray(capacity);

      ElementsAccessor* accessor = array->GetElementsAccessor();
      accessor->CopyElements(
          elms_obj, 0, kind, new_elms, 0,
          ElementsAccessor::kCopyToEndAndInitializeToHole);

      elms = new_elms;
    }

    // Add the provided values.
    DisallowHeapAllocation no_gc;
    WriteBarrierMode mode = elms->GetWriteBarrierMode(no_gc);
    for (int index = 0; index < to_add; index++) {
      elms->set(index + len, args[index + 1], mode);
    }

    if (*elms != array->elements()) {
      array->set_elements(*elms);
    }

    // Set the length.
    array->set_length(Smi::FromInt(new_length));
    return Smi::FromInt(new_length);
  } else {
    int elms_len = elms_obj->length();
    if (to_add == 0) {
      return Smi::FromInt(len);
    }
    // Currently fixed arrays cannot grow too big, so
    // we should never hit this case.
    DCHECK(to_add <= (Smi::kMaxValue - len));

    int new_length = len + to_add;

    Handle<FixedDoubleArray> new_elms;

    if (new_length > elms_len) {
      // New backing storage is needed.
      int capacity = new_length + (new_length >> 1) + 16;
      // Create new backing store; since capacity > 0, we can
      // safely cast to FixedDoubleArray.
      new_elms = Handle<FixedDoubleArray>::cast(
          isolate->factory()->NewFixedDoubleArray(capacity));

      ElementsAccessor* accessor = array->GetElementsAccessor();
      accessor->CopyElements(
          elms_obj, 0, kind, new_elms, 0,
          ElementsAccessor::kCopyToEndAndInitializeToHole);

    } else {
      // to_add is > 0 and new_length <= elms_len, so elms_obj cannot be the
      // empty_fixed_array.
      new_elms = Handle<FixedDoubleArray>::cast(elms_obj);
    }

    // Add the provided values.
    DisallowHeapAllocation no_gc;
    int index;
    for (index = 0; index < to_add; index++) {
      Object* arg = args[index + 1];
      new_elms->set(index + len, arg->Number());
    }

    if (*new_elms != array->elements()) {
      array->set_elements(*new_elms);
    }

    // Set the length.
    array->set_length(Smi::FromInt(new_length));
    return Smi::FromInt(new_length);
  }
}


BUILTIN(ArrayPop) {
  HandleScope scope(isolate);
  Handle<Object> receiver = args.receiver();
  MaybeHandle<FixedArrayBase> maybe_elms_obj =
      EnsureJSArrayWithWritableFastElements(isolate, receiver, NULL, 0);
  Handle<FixedArrayBase> elms_obj;
  if (!maybe_elms_obj.ToHandle(&elms_obj)) {
    return CallJsBuiltin(isolate, "$arrayPop", args);
  }

  Handle<JSArray> array = Handle<JSArray>::cast(receiver);
  DCHECK(!array->map()->is_observed());

  uint32_t len = static_cast<uint32_t>(Smi::cast(array->length())->value());
  if (len == 0) return isolate->heap()->undefined_value();

  if (JSArray::HasReadOnlyLength(array)) {
    return CallJsBuiltin(isolate, "$arrayPop", args);
  }

  uint32_t new_length = len - 1;
  Handle<Object> element;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, element, Object::GetElement(isolate, array, new_length));

  JSArray::SetLength(array, new_length);
  return *element;
}


BUILTIN(ArrayShift) {
  HandleScope scope(isolate);
  Heap* heap = isolate->heap();
  Handle<Object> receiver = args.receiver();
  MaybeHandle<FixedArrayBase> maybe_elms_obj =
      EnsureJSArrayWithWritableFastElements(isolate, receiver, NULL, 0);
  Handle<FixedArrayBase> elms_obj;
  if (!maybe_elms_obj.ToHandle(&elms_obj) ||
      !IsJSArrayFastElementMovingAllowed(isolate, JSArray::cast(*receiver))) {
    return CallJsBuiltin(isolate, "$arrayShift", args);
  }
  Handle<JSArray> array = Handle<JSArray>::cast(receiver);
  DCHECK(!array->map()->is_observed());

  int len = Smi::cast(array->length())->value();
  if (len == 0) return heap->undefined_value();

  if (JSArray::HasReadOnlyLength(array)) {
    return CallJsBuiltin(isolate, "$arrayShift", args);
  }

  // Get first element
  Handle<Object> first;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, first,
                                     Object::GetElement(isolate, array, 0));

  if (heap->CanMoveObjectStart(*elms_obj)) {
    array->set_elements(heap->LeftTrimFixedArray(*elms_obj, 1));
  } else {
    // Shift the elements.
    if (elms_obj->IsFixedArray()) {
      Handle<FixedArray> elms = Handle<FixedArray>::cast(elms_obj);
      DisallowHeapAllocation no_gc;
      heap->MoveElements(*elms, 0, 1, len - 1);
      elms->set(len - 1, heap->the_hole_value());
    } else {
      Handle<FixedDoubleArray> elms = Handle<FixedDoubleArray>::cast(elms_obj);
      MoveDoubleElements(*elms, 0, *elms, 1, len - 1);
      elms->set_the_hole(len - 1);
    }
  }

  // Set the length.
  array->set_length(Smi::FromInt(len - 1));

  return *first;
}


BUILTIN(ArrayUnshift) {
  HandleScope scope(isolate);
  Heap* heap = isolate->heap();
  Handle<Object> receiver = args.receiver();
  MaybeHandle<FixedArrayBase> maybe_elms_obj =
      EnsureJSArrayWithWritableFastElements(isolate, receiver, &args, 1);
  Handle<FixedArrayBase> elms_obj;
  if (!maybe_elms_obj.ToHandle(&elms_obj)) {
    return CallJsBuiltin(isolate, "$arrayUnshift", args);
  }
  Handle<JSArray> array = Handle<JSArray>::cast(receiver);
  DCHECK(!array->map()->is_observed());
  if (!array->HasFastSmiOrObjectElements()) {
    return CallJsBuiltin(isolate, "$arrayUnshift", args);
  }
  int len = Smi::cast(array->length())->value();
  int to_add = args.length() - 1;
  int new_length = len + to_add;
  // Currently fixed arrays cannot grow too big, so
  // we should never hit this case.
  DCHECK(to_add <= (Smi::kMaxValue - len));

  if (to_add > 0 && JSArray::WouldChangeReadOnlyLength(array, len + to_add)) {
    return CallJsBuiltin(isolate, "$arrayUnshift", args);
  }

  Handle<FixedArray> elms = Handle<FixedArray>::cast(elms_obj);

  if (new_length > elms->length()) {
    // New backing storage is needed.
    int capacity = new_length + (new_length >> 1) + 16;
    Handle<FixedArray> new_elms =
        isolate->factory()->NewUninitializedFixedArray(capacity);

    ElementsKind kind = array->GetElementsKind();
    ElementsAccessor* accessor = array->GetElementsAccessor();
    accessor->CopyElements(
        elms, 0, kind, new_elms, to_add,
        ElementsAccessor::kCopyToEndAndInitializeToHole);

    elms = new_elms;
    array->set_elements(*elms);
  } else {
    DisallowHeapAllocation no_gc;
    heap->MoveElements(*elms, to_add, 0, len);
  }

  // Add the provided values.
  DisallowHeapAllocation no_gc;
  WriteBarrierMode mode = elms->GetWriteBarrierMode(no_gc);
  for (int i = 0; i < to_add; i++) {
    elms->set(i, args[i + 1], mode);
  }

  // Set the length.
  array->set_length(Smi::FromInt(new_length));
  return Smi::FromInt(new_length);
}


BUILTIN(ArraySlice) {
  HandleScope scope(isolate);
  Handle<Object> receiver = args.receiver();
  int len = -1;
  int relative_start = 0;
  int relative_end = 0;
  {
    DisallowHeapAllocation no_gc;
    if (receiver->IsJSArray()) {
      JSArray* array = JSArray::cast(*receiver);
      if (!IsJSArrayFastElementMovingAllowed(isolate, array)) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arraySlice", args);
      }

      if (!array->HasFastElements()) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arraySlice", args);
      }

      len = Smi::cast(array->length())->value();
    } else {
      // Array.slice(arguments, ...) is quite a common idiom (notably more
      // than 50% of invocations in Web apps).  Treat it in C++ as well.
      Map* arguments_map =
          isolate->context()->native_context()->sloppy_arguments_map();

      bool is_arguments_object_with_fast_elements =
          receiver->IsJSObject() &&
          JSObject::cast(*receiver)->map() == arguments_map;
      if (!is_arguments_object_with_fast_elements) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arraySlice", args);
      }
      JSObject* object = JSObject::cast(*receiver);

      if (!object->HasFastElements()) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arraySlice", args);
      }

      Object* len_obj = object->InObjectPropertyAt(Heap::kArgumentsLengthIndex);
      if (!len_obj->IsSmi()) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arraySlice", args);
      }
      len = Smi::cast(len_obj)->value();
      if (len > object->elements()->length()) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arraySlice", args);
      }
    }

    DCHECK(len >= 0);
    int n_arguments = args.length() - 1;

    // Note carefully choosen defaults---if argument is missing,
    // it's undefined which gets converted to 0 for relative_start
    // and to len for relative_end.
    relative_start = 0;
    relative_end = len;
    if (n_arguments > 0) {
      Object* arg1 = args[1];
      if (arg1->IsSmi()) {
        relative_start = Smi::cast(arg1)->value();
      } else if (arg1->IsHeapNumber()) {
        double start = HeapNumber::cast(arg1)->value();
        if (start < kMinInt || start > kMaxInt) {
          AllowHeapAllocation allow_allocation;
          return CallJsBuiltin(isolate, "$arraySlice", args);
        }
        relative_start = std::isnan(start) ? 0 : static_cast<int>(start);
      } else if (!arg1->IsUndefined()) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arraySlice", args);
      }
      if (n_arguments > 1) {
        Object* arg2 = args[2];
        if (arg2->IsSmi()) {
          relative_end = Smi::cast(arg2)->value();
        } else if (arg2->IsHeapNumber()) {
          double end = HeapNumber::cast(arg2)->value();
          if (end < kMinInt || end > kMaxInt) {
            AllowHeapAllocation allow_allocation;
            return CallJsBuiltin(isolate, "$arraySlice", args);
          }
          relative_end = std::isnan(end) ? 0 : static_cast<int>(end);
        } else if (!arg2->IsUndefined()) {
          AllowHeapAllocation allow_allocation;
          return CallJsBuiltin(isolate, "$arraySlice", args);
        }
      }
    }
  }

  // ECMAScript 232, 3rd Edition, Section 15.4.4.10, step 6.
  int k = (relative_start < 0) ? Max(len + relative_start, 0)
                               : Min(relative_start, len);

  // ECMAScript 232, 3rd Edition, Section 15.4.4.10, step 8.
  int final = (relative_end < 0) ? Max(len + relative_end, 0)
                                 : Min(relative_end, len);

  // Calculate the length of result array.
  int result_len = Max(final - k, 0);

  Handle<JSObject> object = Handle<JSObject>::cast(receiver);
  Handle<FixedArrayBase> elms(object->elements(), isolate);

  ElementsKind kind = object->GetElementsKind();
  if (IsHoleyElementsKind(kind)) {
    DisallowHeapAllocation no_gc;
    bool packed = true;
    ElementsAccessor* accessor = ElementsAccessor::ForKind(kind);
    for (int i = k; i < final; i++) {
      if (!accessor->HasElement(object, i, elms)) {
        packed = false;
        break;
      }
    }
    if (packed) {
      kind = GetPackedElementsKind(kind);
    } else if (!receiver->IsJSArray()) {
      AllowHeapAllocation allow_allocation;
      return CallJsBuiltin(isolate, "$arraySlice", args);
    }
  }

  Handle<JSArray> result_array =
      isolate->factory()->NewJSArray(kind, result_len, result_len);

  DisallowHeapAllocation no_gc;
  if (result_len == 0) return *result_array;

  ElementsAccessor* accessor = object->GetElementsAccessor();
  accessor->CopyElements(
      elms, k, kind, handle(result_array->elements(), isolate), 0, result_len);
  return *result_array;
}


BUILTIN(ArraySplice) {
  HandleScope scope(isolate);
  Heap* heap = isolate->heap();
  Handle<Object> receiver = args.receiver();
  MaybeHandle<FixedArrayBase> maybe_elms_obj =
      EnsureJSArrayWithWritableFastElements(isolate, receiver, &args, 3);
  Handle<FixedArrayBase> elms_obj;
  if (!maybe_elms_obj.ToHandle(&elms_obj)) {
    return CallJsBuiltin(isolate, "$arraySplice", args);
  }
  Handle<JSArray> array = Handle<JSArray>::cast(receiver);
  DCHECK(!array->map()->is_observed());

  int len = Smi::cast(array->length())->value();

  int n_arguments = args.length() - 1;

  int relative_start = 0;
  if (n_arguments > 0) {
    DisallowHeapAllocation no_gc;
    Object* arg1 = args[1];
    if (arg1->IsSmi()) {
      relative_start = Smi::cast(arg1)->value();
    } else if (arg1->IsHeapNumber()) {
      double start = HeapNumber::cast(arg1)->value();
      if (start < kMinInt || start > kMaxInt) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arraySplice", args);
      }
      relative_start = std::isnan(start) ? 0 : static_cast<int>(start);
    } else if (!arg1->IsUndefined()) {
      AllowHeapAllocation allow_allocation;
      return CallJsBuiltin(isolate, "$arraySplice", args);
    }
  }
  int actual_start = (relative_start < 0) ? Max(len + relative_start, 0)
                                          : Min(relative_start, len);

  // SpiderMonkey, TraceMonkey and JSC treat the case where no delete count is
  // given as a request to delete all the elements from the start.
  // And it differs from the case of undefined delete count.
  // This does not follow ECMA-262, but we do the same for
  // compatibility.
  int actual_delete_count;
  if (n_arguments == 1) {
    DCHECK(len - actual_start >= 0);
    actual_delete_count = len - actual_start;
  } else {
    int value = 0;  // ToInteger(undefined) == 0
    if (n_arguments > 1) {
      DisallowHeapAllocation no_gc;
      Object* arg2 = args[2];
      if (arg2->IsSmi()) {
        value = Smi::cast(arg2)->value();
      } else {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arraySplice", args);
      }
    }
    actual_delete_count = Min(Max(value, 0), len - actual_start);
  }

  ElementsKind elements_kind = array->GetElementsKind();

  int item_count = (n_arguments > 1) ? (n_arguments - 2) : 0;
  int new_length = len - actual_delete_count + item_count;

  // For double mode we do not support changing the length.
  if (new_length > len && IsFastDoubleElementsKind(elements_kind)) {
    return CallJsBuiltin(isolate, "$arraySplice", args);
  }

  if (new_length != len && JSArray::HasReadOnlyLength(array)) {
    AllowHeapAllocation allow_allocation;
    return CallJsBuiltin(isolate, "$arraySplice", args);
  }

  if (new_length == 0) {
    Handle<JSArray> result = isolate->factory()->NewJSArrayWithElements(
        elms_obj, elements_kind, actual_delete_count);
    array->set_elements(heap->empty_fixed_array());
    array->set_length(Smi::FromInt(0));
    return *result;
  }

  Handle<JSArray> result_array =
      isolate->factory()->NewJSArray(elements_kind,
                                     actual_delete_count,
                                     actual_delete_count);

  if (actual_delete_count > 0) {
    DisallowHeapAllocation no_gc;
    ElementsAccessor* accessor = array->GetElementsAccessor();
    accessor->CopyElements(
        elms_obj, actual_start, elements_kind,
        handle(result_array->elements(), isolate), 0, actual_delete_count);
  }

  bool elms_changed = false;
  if (item_count < actual_delete_count) {
    // Shrink the array.
    const bool trim_array = !heap->lo_space()->Contains(*elms_obj) &&
      ((actual_start + item_count) <
          (len - actual_delete_count - actual_start));
    if (trim_array) {
      const int delta = actual_delete_count - item_count;

      if (elms_obj->IsFixedDoubleArray()) {
        Handle<FixedDoubleArray> elms =
            Handle<FixedDoubleArray>::cast(elms_obj);
        MoveDoubleElements(*elms, delta, *elms, 0, actual_start);
      } else {
        Handle<FixedArray> elms = Handle<FixedArray>::cast(elms_obj);
        DisallowHeapAllocation no_gc;
        heap->MoveElements(*elms, delta, 0, actual_start);
      }

      if (heap->CanMoveObjectStart(*elms_obj)) {
        // On the fast path we move the start of the object in memory.
        elms_obj = handle(heap->LeftTrimFixedArray(*elms_obj, delta));
      } else {
        // This is the slow path. We are going to move the elements to the left
        // by copying them. For trimmed values we store the hole.
        if (elms_obj->IsFixedDoubleArray()) {
          Handle<FixedDoubleArray> elms =
              Handle<FixedDoubleArray>::cast(elms_obj);
          MoveDoubleElements(*elms, 0, *elms, delta, len - delta);
          elms->FillWithHoles(len - delta, len);
        } else {
          Handle<FixedArray> elms = Handle<FixedArray>::cast(elms_obj);
          DisallowHeapAllocation no_gc;
          heap->MoveElements(*elms, 0, delta, len - delta);
          elms->FillWithHoles(len - delta, len);
        }
      }
      elms_changed = true;
    } else {
      if (elms_obj->IsFixedDoubleArray()) {
        Handle<FixedDoubleArray> elms =
            Handle<FixedDoubleArray>::cast(elms_obj);
        MoveDoubleElements(*elms, actual_start + item_count,
                           *elms, actual_start + actual_delete_count,
                           (len - actual_delete_count - actual_start));
        elms->FillWithHoles(new_length, len);
      } else {
        Handle<FixedArray> elms = Handle<FixedArray>::cast(elms_obj);
        DisallowHeapAllocation no_gc;
        heap->MoveElements(*elms, actual_start + item_count,
                           actual_start + actual_delete_count,
                           (len - actual_delete_count - actual_start));
        elms->FillWithHoles(new_length, len);
      }
    }
  } else if (item_count > actual_delete_count) {
    Handle<FixedArray> elms = Handle<FixedArray>::cast(elms_obj);
    // Currently fixed arrays cannot grow too big, so
    // we should never hit this case.
    DCHECK((item_count - actual_delete_count) <= (Smi::kMaxValue - len));

    // Check if array need to grow.
    if (new_length > elms->length()) {
      // New backing storage is needed.
      int capacity = new_length + (new_length >> 1) + 16;
      Handle<FixedArray> new_elms =
          isolate->factory()->NewUninitializedFixedArray(capacity);

      DisallowHeapAllocation no_gc;

      ElementsKind kind = array->GetElementsKind();
      ElementsAccessor* accessor = array->GetElementsAccessor();
      if (actual_start > 0) {
        // Copy the part before actual_start as is.
        accessor->CopyElements(
            elms, 0, kind, new_elms, 0, actual_start);
      }
      accessor->CopyElements(
          elms, actual_start + actual_delete_count, kind,
          new_elms, actual_start + item_count,
          ElementsAccessor::kCopyToEndAndInitializeToHole);

      elms_obj = new_elms;
      elms_changed = true;
    } else {
      DisallowHeapAllocation no_gc;
      heap->MoveElements(*elms, actual_start + item_count,
                         actual_start + actual_delete_count,
                         (len - actual_delete_count - actual_start));
    }
  }

  if (IsFastDoubleElementsKind(elements_kind)) {
    Handle<FixedDoubleArray> elms = Handle<FixedDoubleArray>::cast(elms_obj);
    for (int k = actual_start; k < actual_start + item_count; k++) {
      Object* arg = args[3 + k - actual_start];
      if (arg->IsSmi()) {
        elms->set(k, Smi::cast(arg)->value());
      } else {
        elms->set(k, HeapNumber::cast(arg)->value());
      }
    }
  } else {
    Handle<FixedArray> elms = Handle<FixedArray>::cast(elms_obj);
    DisallowHeapAllocation no_gc;
    WriteBarrierMode mode = elms->GetWriteBarrierMode(no_gc);
    for (int k = actual_start; k < actual_start + item_count; k++) {
      elms->set(k, args[3 + k - actual_start], mode);
    }
  }

  if (elms_changed) {
    array->set_elements(*elms_obj);
  }
  // Set the length.
  array->set_length(Smi::FromInt(new_length));

  return *result_array;
}


BUILTIN(ArrayConcat) {
  HandleScope scope(isolate);

  int n_arguments = args.length();
  int result_len = 0;
  ElementsKind elements_kind = GetInitialFastElementsKind();
  bool has_double = false;
  {
    DisallowHeapAllocation no_gc;
    Context* native_context = isolate->context()->native_context();
    Object* array_proto = native_context->array_function()->prototype();
    PrototypeIterator iter(isolate, array_proto,
                           PrototypeIterator::START_AT_RECEIVER);
    if (!ArrayPrototypeHasNoElements(&iter)) {
      AllowHeapAllocation allow_allocation;
      return CallJsBuiltin(isolate, "$arrayConcat", args);
    }

    // Iterate through all the arguments performing checks
    // and calculating total length.
    bool is_holey = false;
    for (int i = 0; i < n_arguments; i++) {
      Object* arg = args[i];
      PrototypeIterator iter(isolate, arg);
      if (!arg->IsJSArray() || !JSArray::cast(arg)->HasFastElements() ||
          iter.GetCurrent() != array_proto) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arrayConcat", args);
      }
      int len = Smi::cast(JSArray::cast(arg)->length())->value();

      // We shouldn't overflow when adding another len.
      const int kHalfOfMaxInt = 1 << (kBitsPerInt - 2);
      STATIC_ASSERT(FixedArray::kMaxLength < kHalfOfMaxInt);
      USE(kHalfOfMaxInt);
      result_len += len;
      DCHECK(result_len >= 0);

      if (result_len > FixedDoubleArray::kMaxLength) {
        AllowHeapAllocation allow_allocation;
        return CallJsBuiltin(isolate, "$arrayConcat", args);
      }

      ElementsKind arg_kind = JSArray::cast(arg)->map()->elements_kind();
      has_double = has_double || IsFastDoubleElementsKind(arg_kind);
      is_holey = is_holey || IsFastHoleyElementsKind(arg_kind);
      if (IsMoreGeneralElementsKindTransition(elements_kind, arg_kind)) {
        elements_kind = arg_kind;
      }
    }
    if (is_holey) elements_kind = GetHoleyElementsKind(elements_kind);
  }

  // If a double array is concatted into a fast elements array, the fast
  // elements array needs to be initialized to contain proper holes, since
  // boxing doubles may cause incremental marking.
  ArrayStorageAllocationMode mode =
      has_double && IsFastObjectElementsKind(elements_kind)
      ? INITIALIZE_ARRAY_ELEMENTS_WITH_HOLE : DONT_INITIALIZE_ARRAY_ELEMENTS;
  Handle<JSArray> result_array = isolate->factory()->NewJSArray(
      elements_kind, result_len, result_len, Strength::WEAK, mode);
  if (result_len == 0) return *result_array;

  int j = 0;
  Handle<FixedArrayBase> storage(result_array->elements(), isolate);
  ElementsAccessor* accessor = ElementsAccessor::ForKind(elements_kind);
  for (int i = 0; i < n_arguments; i++) {
    // It is crucial to keep |array| in a raw pointer form to avoid performance
    // degradation.
    JSArray* array = JSArray::cast(args[i]);
    int len = Smi::cast(array->length())->value();
    if (len > 0) {
      ElementsKind from_kind = array->GetElementsKind();
      accessor->CopyElements(array, 0, from_kind, storage, j, len);
      j += len;
    }
  }

  DCHECK(j == result_len);

  return *result_array;
}


// -----------------------------------------------------------------------------
// Throwers for restricted function properties and strict arguments object
// properties


BUILTIN(RestrictedFunctionPropertiesThrower) {
  HandleScope scope(isolate);
  THROW_NEW_ERROR_RETURN_FAILURE(
      isolate, NewTypeError(MessageTemplate::kRestrictedFunctionProperties));
}


BUILTIN(RestrictedStrictArgumentsPropertiesThrower) {
  HandleScope scope(isolate);
  THROW_NEW_ERROR_RETURN_FAILURE(
      isolate, NewTypeError(MessageTemplate::kStrictPoisonPill));
}


// -----------------------------------------------------------------------------
//


template <bool is_construct>
MUST_USE_RESULT static MaybeHandle<Object> HandleApiCallHelper(
    Isolate* isolate, BuiltinArguments<NEEDS_CALLED_FUNCTION>& args) {
  HandleScope scope(isolate);
  Handle<JSFunction> function = args.called_function();
  // TODO(ishell): turn this back to a DCHECK.
  CHECK(function->shared()->IsApiFunction());

  Handle<FunctionTemplateInfo> fun_data(
      function->shared()->get_api_func_data(), isolate);
  if (is_construct) {
    ASSIGN_RETURN_ON_EXCEPTION(
        isolate, fun_data,
        ApiNatives::ConfigureInstance(isolate, fun_data,
                                      Handle<JSObject>::cast(args.receiver())),
        Object);
  }

  DCHECK(!args[0]->IsNull());
  if (args[0]->IsUndefined()) args[0] = function->global_proxy();

  if (!is_construct && !fun_data->accept_any_receiver()) {
    Handle<Object> receiver(&args[0]);
    if (receiver->IsJSObject() && receiver->IsAccessCheckNeeded()) {
      Handle<JSObject> js_receiver = Handle<JSObject>::cast(receiver);
      if (!isolate->MayAccess(js_receiver)) {
        isolate->ReportFailedAccessCheck(js_receiver);
        RETURN_EXCEPTION_IF_SCHEDULED_EXCEPTION(isolate, Object);
      }
    }
  }

  Object* raw_holder = fun_data->GetCompatibleReceiver(isolate, args[0]);

  if (raw_holder->IsNull()) {
    // This function cannot be called with the given receiver.  Abort!
    THROW_NEW_ERROR(isolate, NewTypeError(MessageTemplate::kIllegalInvocation),
                    Object);
  }

  Object* raw_call_data = fun_data->call_code();
  if (!raw_call_data->IsUndefined()) {
    // TODO(ishell): remove this debugging code.
    CHECK(raw_call_data->IsCallHandlerInfo());
    CallHandlerInfo* call_data = CallHandlerInfo::cast(raw_call_data);
    Object* callback_obj = call_data->callback();
    v8::FunctionCallback callback =
        v8::ToCData<v8::FunctionCallback>(callback_obj);
    Object* data_obj = call_data->data();

    LOG(isolate, ApiObjectAccess("call", JSObject::cast(*args.receiver())));
    DCHECK(raw_holder->IsJSObject());

    FunctionCallbackArguments custom(isolate,
                                     data_obj,
                                     *function,
                                     raw_holder,
                                     &args[0] - 1,
                                     args.length() - 1,
                                     is_construct);

    v8::Local<v8::Value> value = custom.Call(callback);
    Handle<Object> result;
    if (value.IsEmpty()) {
      result = isolate->factory()->undefined_value();
    } else {
      result = v8::Utils::OpenHandle(*value);
      result->VerifyApiCallResultType();
    }

    RETURN_EXCEPTION_IF_SCHEDULED_EXCEPTION(isolate, Object);
    if (!is_construct || result->IsJSObject()) {
      return scope.CloseAndEscape(result);
    }
  }

  return scope.CloseAndEscape(args.receiver());
}


BUILTIN(HandleApiCall) {
  HandleScope scope(isolate);
  DCHECK(!CalledAsConstructor(isolate));
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result,
                                     HandleApiCallHelper<false>(isolate, args));
  return *result;
}


BUILTIN(HandleApiCallConstruct) {
  HandleScope scope(isolate);
  DCHECK(CalledAsConstructor(isolate));
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result,
                                     HandleApiCallHelper<true>(isolate, args));
  return *result;
}


namespace {

class RelocatableArguments : public BuiltinArguments<NEEDS_CALLED_FUNCTION>,
                             public Relocatable {
 public:
  RelocatableArguments(Isolate* isolate, int length, Object** arguments)
      : BuiltinArguments<NEEDS_CALLED_FUNCTION>(length, arguments),
        Relocatable(isolate) {}

  virtual inline void IterateInstance(ObjectVisitor* v) {
    if (length() == 0) return;
    v->VisitPointers(lowest_address(), highest_address() + 1);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(RelocatableArguments);
};

}  // namespace


MaybeHandle<Object> Builtins::InvokeApiFunction(Handle<JSFunction> function,
                                                Handle<Object> receiver,
                                                int argc,
                                                Handle<Object> args[]) {
  // Construct BuiltinArguments object: function, arguments reversed, receiver.
  const int kBufferSize = 32;
  Object* small_argv[kBufferSize];
  Object** argv;
  if (argc + 2 <= kBufferSize) {
    argv = small_argv;
  } else {
    argv = new Object* [argc + 2];
  }
  argv[argc + 1] = *receiver;
  for (int i = 0; i < argc; ++i) {
    argv[argc - i] = *args[i];
  }
  argv[0] = *function;
  MaybeHandle<Object> result;
  {
    auto isolate = function->GetIsolate();
    RelocatableArguments arguments(isolate, argc + 2, &argv[argc + 1]);
    result = HandleApiCallHelper<false>(isolate, arguments);
  }
  if (argv != small_argv) {
    delete[] argv;
  }
  return result;
}


// Helper function to handle calls to non-function objects created through the
// API. The object can be called as either a constructor (using new) or just as
// a function (without new).
MUST_USE_RESULT static Object* HandleApiCallAsFunctionOrConstructor(
    Isolate* isolate,
    bool is_construct_call,
    BuiltinArguments<NO_EXTRA_ARGUMENTS> args) {
  // Non-functions are never called as constructors. Even if this is an object
  // called as a constructor the delegate call is not a construct call.
  DCHECK(!CalledAsConstructor(isolate));
  Heap* heap = isolate->heap();

  Handle<Object> receiver = args.receiver();

  // Get the object called.
  JSObject* obj = JSObject::cast(*receiver);

  // Get the invocation callback from the function descriptor that was
  // used to create the called object.
  DCHECK(obj->map()->has_instance_call_handler());
  JSFunction* constructor = JSFunction::cast(obj->map()->GetConstructor());
  // TODO(ishell): turn this back to a DCHECK.
  CHECK(constructor->shared()->IsApiFunction());
  Object* handler =
      constructor->shared()->get_api_func_data()->instance_call_handler();
  DCHECK(!handler->IsUndefined());
  // TODO(ishell): remove this debugging code.
  CHECK(handler->IsCallHandlerInfo());
  CallHandlerInfo* call_data = CallHandlerInfo::cast(handler);
  Object* callback_obj = call_data->callback();
  v8::FunctionCallback callback =
      v8::ToCData<v8::FunctionCallback>(callback_obj);

  // Get the data for the call and perform the callback.
  Object* result;
  {
    HandleScope scope(isolate);
    LOG(isolate, ApiObjectAccess("call non-function", obj));

    FunctionCallbackArguments custom(isolate,
                                     call_data->data(),
                                     constructor,
                                     obj,
                                     &args[0] - 1,
                                     args.length() - 1,
                                     is_construct_call);
    v8::Local<v8::Value> value = custom.Call(callback);
    if (value.IsEmpty()) {
      result = heap->undefined_value();
    } else {
      result = *reinterpret_cast<Object**>(*value);
      result->VerifyApiCallResultType();
    }
  }
  // Check for exceptions and return result.
  RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);
  return result;
}


// Handle calls to non-function objects created through the API. This delegate
// function is used when the call is a normal function call.
BUILTIN(HandleApiCallAsFunction) {
  return HandleApiCallAsFunctionOrConstructor(isolate, false, args);
}


// Handle calls to non-function objects created through the API. This delegate
// function is used when the call is a construct call.
BUILTIN(HandleApiCallAsConstructor) {
  return HandleApiCallAsFunctionOrConstructor(isolate, true, args);
}


static void Generate_LoadIC_Miss(MacroAssembler* masm) {
  LoadIC::GenerateMiss(masm);
}


static void Generate_LoadIC_Normal(MacroAssembler* masm) {
  LoadIC::GenerateNormal(masm, SLOPPY);
}


static void Generate_LoadIC_Normal_Strong(MacroAssembler* masm) {
  LoadIC::GenerateNormal(masm, STRONG);
}


static void Generate_LoadIC_Getter_ForDeopt(MacroAssembler* masm) {
  NamedLoadHandlerCompiler::GenerateLoadViaGetterForDeopt(masm);
}


static void Generate_LoadIC_Slow(MacroAssembler* masm) {
  LoadIC::GenerateRuntimeGetProperty(masm, SLOPPY);
}


static void Generate_LoadIC_Slow_Strong(MacroAssembler* masm) {
  LoadIC::GenerateRuntimeGetProperty(masm, STRONG);
}


static void Generate_KeyedLoadIC_Slow(MacroAssembler* masm) {
  KeyedLoadIC::GenerateRuntimeGetProperty(masm, SLOPPY);
}


static void Generate_KeyedLoadIC_Slow_Strong(MacroAssembler* masm) {
  KeyedLoadIC::GenerateRuntimeGetProperty(masm, STRONG);
}


static void Generate_KeyedLoadIC_Miss(MacroAssembler* masm) {
  KeyedLoadIC::GenerateMiss(masm);
}


static void Generate_KeyedLoadIC_Megamorphic(MacroAssembler* masm) {
  KeyedLoadIC::GenerateMegamorphic(masm, SLOPPY);
}


static void Generate_KeyedLoadIC_Megamorphic_Strong(MacroAssembler* masm) {
  KeyedLoadIC::GenerateMegamorphic(masm, STRONG);
}


static void Generate_StoreIC_Miss(MacroAssembler* masm) {
  StoreIC::GenerateMiss(masm);
}


static void Generate_StoreIC_Normal(MacroAssembler* masm) {
  StoreIC::GenerateNormal(masm);
}


static void Generate_StoreIC_Slow(MacroAssembler* masm) {
  NamedStoreHandlerCompiler::GenerateSlow(masm);
}


static void Generate_KeyedStoreIC_Slow(MacroAssembler* masm) {
  ElementHandlerCompiler::GenerateStoreSlow(masm);
}


static void Generate_StoreIC_Setter_ForDeopt(MacroAssembler* masm) {
  NamedStoreHandlerCompiler::GenerateStoreViaSetterForDeopt(masm);
}


static void Generate_KeyedStoreIC_Megamorphic(MacroAssembler* masm) {
  KeyedStoreIC::GenerateMegamorphic(masm, SLOPPY);
}


static void Generate_KeyedStoreIC_Megamorphic_Strict(MacroAssembler* masm) {
  KeyedStoreIC::GenerateMegamorphic(masm, STRICT);
}


static void Generate_KeyedStoreIC_Miss(MacroAssembler* masm) {
  KeyedStoreIC::GenerateMiss(masm);
}


static void Generate_KeyedStoreIC_Initialize(MacroAssembler* masm) {
  KeyedStoreIC::GenerateInitialize(masm);
}


static void Generate_KeyedStoreIC_Initialize_Strict(MacroAssembler* masm) {
  KeyedStoreIC::GenerateInitialize(masm);
}


static void Generate_KeyedStoreIC_PreMonomorphic(MacroAssembler* masm) {
  KeyedStoreIC::GeneratePreMonomorphic(masm);
}


static void Generate_KeyedStoreIC_PreMonomorphic_Strict(MacroAssembler* masm) {
  KeyedStoreIC::GeneratePreMonomorphic(masm);
}


static void Generate_CallICStub_DebugBreak(MacroAssembler* masm) {
  DebugCodegen::GenerateCallICStubDebugBreak(masm);
}


static void Generate_Return_DebugBreak(MacroAssembler* masm) {
  DebugCodegen::GenerateReturnDebugBreak(masm);
}


static void Generate_CallFunctionStub_DebugBreak(MacroAssembler* masm) {
  DebugCodegen::GenerateCallFunctionStubDebugBreak(masm);
}


static void Generate_CallConstructStub_DebugBreak(MacroAssembler* masm) {
  DebugCodegen::GenerateCallConstructStubDebugBreak(masm);
}


static void Generate_CallConstructStub_Recording_DebugBreak(
    MacroAssembler* masm) {
  DebugCodegen::GenerateCallConstructStubRecordDebugBreak(masm);
}


static void Generate_Slot_DebugBreak(MacroAssembler* masm) {
  DebugCodegen::GenerateSlotDebugBreak(masm);
}


static void Generate_PlainReturn_LiveEdit(MacroAssembler* masm) {
  DebugCodegen::GeneratePlainReturnLiveEdit(masm);
}


static void Generate_FrameDropper_LiveEdit(MacroAssembler* masm) {
  DebugCodegen::GenerateFrameDropperLiveEdit(masm);
}


Builtins::Builtins() : initialized_(false) {
  memset(builtins_, 0, sizeof(builtins_[0]) * builtin_count);
  memset(names_, 0, sizeof(names_[0]) * builtin_count);
}


Builtins::~Builtins() {
}


#define DEF_ENUM_C(name, ignore) FUNCTION_ADDR(Builtin_##name),
Address const Builtins::c_functions_[cfunction_count] = {
  BUILTIN_LIST_C(DEF_ENUM_C)
};
#undef DEF_ENUM_C

#define DEF_JS_NAME(name, ignore) #name,
#define DEF_JS_ARGC(ignore, argc) argc,
const char* const Builtins::javascript_names_[id_count] = {
  BUILTINS_LIST_JS(DEF_JS_NAME)
};

int const Builtins::javascript_argc_[id_count] = {
  BUILTINS_LIST_JS(DEF_JS_ARGC)
};
#undef DEF_JS_NAME
#undef DEF_JS_ARGC

struct BuiltinDesc {
  byte* generator;
  byte* c_code;
  const char* s_name;  // name is only used for generating log information.
  int name;
  Code::Flags flags;
  BuiltinExtraArguments extra_args;
};

#define BUILTIN_FUNCTION_TABLE_INIT { V8_ONCE_INIT, {} }

class BuiltinFunctionTable {
 public:
  BuiltinDesc* functions() {
    base::CallOnce(&once_, &Builtins::InitBuiltinFunctionTable);
    return functions_;
  }

  base::OnceType once_;
  BuiltinDesc functions_[Builtins::builtin_count + 1];

  friend class Builtins;
};

static BuiltinFunctionTable builtin_function_table =
    BUILTIN_FUNCTION_TABLE_INIT;

// Define array of pointers to generators and C builtin functions.
// We do this in a sort of roundabout way so that we can do the initialization
// within the lexical scope of Builtins:: and within a context where
// Code::Flags names a non-abstract type.
void Builtins::InitBuiltinFunctionTable() {
  BuiltinDesc* functions = builtin_function_table.functions_;
  functions[builtin_count].generator = NULL;
  functions[builtin_count].c_code = NULL;
  functions[builtin_count].s_name = NULL;
  functions[builtin_count].name = builtin_count;
  functions[builtin_count].flags = static_cast<Code::Flags>(0);
  functions[builtin_count].extra_args = NO_EXTRA_ARGUMENTS;

#define DEF_FUNCTION_PTR_C(aname, aextra_args)                         \
    functions->generator = FUNCTION_ADDR(Generate_Adaptor);            \
    functions->c_code = FUNCTION_ADDR(Builtin_##aname);                \
    functions->s_name = #aname;                                        \
    functions->name = c_##aname;                                       \
    functions->flags = Code::ComputeFlags(Code::BUILTIN);              \
    functions->extra_args = aextra_args;                               \
    ++functions;

#define DEF_FUNCTION_PTR_A(aname, kind, state, extra)                       \
    functions->generator = FUNCTION_ADDR(Generate_##aname);                 \
    functions->c_code = NULL;                                               \
    functions->s_name = #aname;                                             \
    functions->name = k##aname;                                             \
    functions->flags = Code::ComputeFlags(Code::kind,                       \
                                          state,                            \
                                          extra);                           \
    functions->extra_args = NO_EXTRA_ARGUMENTS;                             \
    ++functions;

#define DEF_FUNCTION_PTR_H(aname, kind)                                     \
    functions->generator = FUNCTION_ADDR(Generate_##aname);                 \
    functions->c_code = NULL;                                               \
    functions->s_name = #aname;                                             \
    functions->name = k##aname;                                             \
    functions->flags = Code::ComputeHandlerFlags(Code::kind);               \
    functions->extra_args = NO_EXTRA_ARGUMENTS;                             \
    ++functions;

  BUILTIN_LIST_C(DEF_FUNCTION_PTR_C)
  BUILTIN_LIST_A(DEF_FUNCTION_PTR_A)
  BUILTIN_LIST_H(DEF_FUNCTION_PTR_H)
  BUILTIN_LIST_DEBUG_A(DEF_FUNCTION_PTR_A)

#undef DEF_FUNCTION_PTR_C
#undef DEF_FUNCTION_PTR_A
}


void Builtins::SetUp(Isolate* isolate, bool create_heap_objects) {
  DCHECK(!initialized_);

  // Create a scope for the handles in the builtins.
  HandleScope scope(isolate);

  const BuiltinDesc* functions = builtin_function_table.functions();

  // For now we generate builtin adaptor code into a stack-allocated
  // buffer, before copying it into individual code objects. Be careful
  // with alignment, some platforms don't like unaligned code.
#ifdef DEBUG
  // We can generate a lot of debug code on Arm64.
  const size_t buffer_size = 32*KB;
#else
  const size_t buffer_size = 8*KB;
#endif
  union { int force_alignment; byte buffer[buffer_size]; } u;

  // Traverse the list of builtins and generate an adaptor in a
  // separate code object for each one.
  for (int i = 0; i < builtin_count; i++) {
    if (create_heap_objects) {
      MacroAssembler masm(isolate, u.buffer, sizeof u.buffer);
      // Generate the code/adaptor.
      typedef void (*Generator)(MacroAssembler*, int, BuiltinExtraArguments);
      Generator g = FUNCTION_CAST<Generator>(functions[i].generator);
      // We pass all arguments to the generator, but it may not use all of
      // them.  This works because the first arguments are on top of the
      // stack.
      DCHECK(!masm.has_frame());
      g(&masm, functions[i].name, functions[i].extra_args);
      // Move the code into the object heap.
      CodeDesc desc;
      masm.GetCode(&desc);
      Code::Flags flags = functions[i].flags;
      Handle<Code> code =
          isolate->factory()->NewCode(desc, flags, masm.CodeObject());
      // Log the event and add the code to the builtins array.
      PROFILE(isolate,
              CodeCreateEvent(Logger::BUILTIN_TAG, *code, functions[i].s_name));
      builtins_[i] = *code;
      code->set_builtin_index(i);
#ifdef ENABLE_DISASSEMBLER
      if (FLAG_print_builtin_code) {
        CodeTracer::Scope trace_scope(isolate->GetCodeTracer());
        OFStream os(trace_scope.file());
        os << "Builtin: " << functions[i].s_name << "\n";
        code->Disassemble(functions[i].s_name, os);
        os << "\n";
      }
#endif
    } else {
      // Deserializing. The values will be filled in during IterateBuiltins.
      builtins_[i] = NULL;
    }
    names_[i] = functions[i].s_name;
  }

  // Mark as initialized.
  initialized_ = true;
}


void Builtins::TearDown() {
  initialized_ = false;
}


void Builtins::IterateBuiltins(ObjectVisitor* v) {
  v->VisitPointers(&builtins_[0], &builtins_[0] + builtin_count);
}


const char* Builtins::Lookup(byte* pc) {
  // may be called during initialization (disassembler!)
  if (initialized_) {
    for (int i = 0; i < builtin_count; i++) {
      Code* entry = Code::cast(builtins_[i]);
      if (entry->contains(pc)) {
        return names_[i];
      }
    }
  }
  return NULL;
}


void Builtins::Generate_InterruptCheck(MacroAssembler* masm) {
  masm->TailCallRuntime(Runtime::kInterrupt, 0, 1);
}


void Builtins::Generate_StackCheck(MacroAssembler* masm) {
  masm->TailCallRuntime(Runtime::kStackGuard, 0, 1);
}


#define DEFINE_BUILTIN_ACCESSOR_C(name, ignore)               \
Handle<Code> Builtins::name() {                               \
  Code** code_address =                                       \
      reinterpret_cast<Code**>(builtin_address(k##name));     \
  return Handle<Code>(code_address);                          \
}
#define DEFINE_BUILTIN_ACCESSOR_A(name, kind, state, extra) \
Handle<Code> Builtins::name() {                             \
  Code** code_address =                                     \
      reinterpret_cast<Code**>(builtin_address(k##name));   \
  return Handle<Code>(code_address);                        \
}
#define DEFINE_BUILTIN_ACCESSOR_H(name, kind)               \
Handle<Code> Builtins::name() {                             \
  Code** code_address =                                     \
      reinterpret_cast<Code**>(builtin_address(k##name));   \
  return Handle<Code>(code_address);                        \
}
BUILTIN_LIST_C(DEFINE_BUILTIN_ACCESSOR_C)
BUILTIN_LIST_A(DEFINE_BUILTIN_ACCESSOR_A)
BUILTIN_LIST_H(DEFINE_BUILTIN_ACCESSOR_H)
BUILTIN_LIST_DEBUG_A(DEFINE_BUILTIN_ACCESSOR_A)
#undef DEFINE_BUILTIN_ACCESSOR_C
#undef DEFINE_BUILTIN_ACCESSOR_A


}  // namespace internal
}  // namespace v8
