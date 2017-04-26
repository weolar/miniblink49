// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/interface-descriptors.h"

namespace v8 {
namespace internal {

namespace {
// Constructors for common combined semantic and representation types.
Type* SmiType(Zone* zone) {
  return Type::Intersect(Type::SignedSmall(), Type::TaggedSigned(), zone);
}


Type* UntaggedIntegral32(Zone* zone) {
  return Type::Intersect(Type::Signed32(), Type::UntaggedIntegral32(), zone);
}


Type* AnyTagged(Zone* zone) {
  return Type::Intersect(
      Type::Any(),
      Type::Union(Type::TaggedPointer(), Type::TaggedSigned(), zone), zone);
}


Type* ExternalPointer(Zone* zone) {
  return Type::Intersect(Type::Internal(), Type::UntaggedPointer(), zone);
}
}  // namespace

FunctionType* CallInterfaceDescriptor::BuildDefaultFunctionType(
    Isolate* isolate, int parameter_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), parameter_count, zone)
          ->AsFunction();
  while (parameter_count-- != 0) {
    function->InitParameter(parameter_count, AnyTagged(zone));
  }
  return function;
}


void CallInterfaceDescriptorData::InitializePlatformSpecific(
    int register_parameter_count, Register* registers,
    PlatformInterfaceDescriptor* platform_descriptor) {
  platform_specific_descriptor_ = platform_descriptor;
  register_param_count_ = register_parameter_count;

  // InterfaceDescriptor owns a copy of the registers array.
  register_params_.Reset(NewArray<Register>(register_parameter_count));
  for (int i = 0; i < register_parameter_count; i++) {
    register_params_[i] = registers[i];
  }
}

const char* CallInterfaceDescriptor::DebugName(Isolate* isolate) const {
  CallInterfaceDescriptorData* start = isolate->call_descriptor_data(0);
  size_t index = data_ - start;
  DCHECK(index < CallDescriptors::NUMBER_OF_DESCRIPTORS);
  CallDescriptors::Key key = static_cast<CallDescriptors::Key>(index);
  switch (key) {
#define DEF_CASE(NAME)        \
  case CallDescriptors::NAME: \
    return #NAME " Descriptor";
    INTERFACE_DESCRIPTOR_LIST(DEF_CASE)
#undef DEF_CASE
    case CallDescriptors::NUMBER_OF_DESCRIPTORS:
      break;
  }
  return "";
}


void AllocateMutableHeapNumberDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  data->InitializePlatformSpecific(0, nullptr, nullptr);
}


void VoidDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  data->InitializePlatformSpecific(0, nullptr);
}

FunctionType* LoadDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 3, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));
  function->InitParameter(1, AnyTagged(zone));
  function->InitParameter(2, SmiType(zone));
  return function;
}


void LoadDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {ReceiverRegister(), NameRegister(), SlotRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}


void StoreDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {ReceiverRegister(), NameRegister(), ValueRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}


void StoreTransitionDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {ReceiverRegister(), NameRegister(), ValueRegister(),
                          MapRegister()};

  data->InitializePlatformSpecific(arraysize(registers), registers);
}


void VectorStoreTransitionDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  if (SlotRegister().is(no_reg)) {
    Register registers[] = {ReceiverRegister(), NameRegister(), ValueRegister(),
                            MapRegister(), VectorRegister()};
    data->InitializePlatformSpecific(arraysize(registers), registers);
  } else {
    Register registers[] = {ReceiverRegister(), NameRegister(),
                            ValueRegister(),    MapRegister(),
                            SlotRegister(),     VectorRegister()};
    data->InitializePlatformSpecific(arraysize(registers), registers);
  }
}

FunctionType*
StoreTransitionDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 4, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));  // Receiver
  function->InitParameter(1, AnyTagged(zone));  // Name
  function->InitParameter(2, AnyTagged(zone));  // Value
  function->InitParameter(3, AnyTagged(zone));  // Map
  return function;
}

FunctionType*
LoadGlobalViaContextDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 1, zone)->AsFunction();
  function->InitParameter(0, UntaggedIntegral32(zone));
  return function;
}


void LoadGlobalViaContextDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {SlotRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}

FunctionType*
StoreGlobalViaContextDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 2, zone)->AsFunction();
  function->InitParameter(0, UntaggedIntegral32(zone));
  function->InitParameter(1, AnyTagged(zone));
  return function;
}


void StoreGlobalViaContextDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {SlotRegister(), ValueRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}


void InstanceOfDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {LeftRegister(), RightRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}


void StringCompareDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {LeftRegister(), RightRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}

void TypeConversionDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {ArgumentRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}


void MathPowTaggedDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {exponent()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}


void MathPowIntegerDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {exponent()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}

FunctionType*
LoadWithVectorDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 4, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));
  function->InitParameter(1, AnyTagged(zone));
  function->InitParameter(2, SmiType(zone));
  function->InitParameter(3, AnyTagged(zone));
  return function;
}


void LoadWithVectorDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {ReceiverRegister(), NameRegister(), SlotRegister(),
                          VectorRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}

FunctionType*
VectorStoreTransitionDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  bool has_slot = !VectorStoreTransitionDescriptor::SlotRegister().is(no_reg);
  int arg_count = has_slot ? 6 : 5;
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), arg_count, zone)
          ->AsFunction();
  int index = 0;
  function->InitParameter(index++, AnyTagged(zone));  // receiver
  function->InitParameter(index++, AnyTagged(zone));  // name
  function->InitParameter(index++, AnyTagged(zone));  // value
  function->InitParameter(index++, AnyTagged(zone));  // map
  if (has_slot) {
    function->InitParameter(index++, SmiType(zone));  // slot
  }
  function->InitParameter(index++, AnyTagged(zone));  // vector
  return function;
}

FunctionType* VectorStoreICDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 5, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));
  function->InitParameter(1, AnyTagged(zone));
  function->InitParameter(2, AnyTagged(zone));
  function->InitParameter(3, SmiType(zone));
  function->InitParameter(4, AnyTagged(zone));
  return function;
}


void VectorStoreICDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {ReceiverRegister(), NameRegister(), ValueRegister(),
                          SlotRegister(), VectorRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}

FunctionType*
VectorStoreICTrampolineDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 4, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));
  function->InitParameter(1, AnyTagged(zone));
  function->InitParameter(2, AnyTagged(zone));
  function->InitParameter(3, SmiType(zone));
  return function;
}


void VectorStoreICTrampolineDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {ReceiverRegister(), NameRegister(), ValueRegister(),
                          SlotRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}

FunctionType* ApiGetterDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 1, zone)->AsFunction();
  function->InitParameter(0, ExternalPointer(zone));
  return function;
}


void ApiGetterDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {function_address()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}


void ContextOnlyDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  data->InitializePlatformSpecific(0, nullptr);
}


void GrowArrayElementsDescriptor::InitializePlatformSpecific(
    CallInterfaceDescriptorData* data) {
  Register registers[] = {ObjectRegister(), KeyRegister()};
  data->InitializePlatformSpecific(arraysize(registers), registers);
}

FunctionType* FastArrayPushDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), AnyTagged(zone), 1, zone)->AsFunction();
  function->InitParameter(0, UntaggedIntegral32(zone));  // actual #arguments
  return function;
}

FunctionType*
FastCloneRegExpDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 4, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));  // closure
  function->InitParameter(1, SmiType(zone));    // literal_index
  function->InitParameter(2, AnyTagged(zone));  // pattern
  function->InitParameter(3, AnyTagged(zone));  // flags
  return function;
}

FunctionType*
FastCloneShallowArrayDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 3, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));
  function->InitParameter(1, SmiType(zone));
  function->InitParameter(2, AnyTagged(zone));
  return function;
}

FunctionType*
CreateAllocationSiteDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 2, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));
  function->InitParameter(1, SmiType(zone));
  return function;
}

FunctionType*
CreateWeakCellDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 3, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));
  function->InitParameter(1, SmiType(zone));
  function->InitParameter(2, AnyTagged(zone));
  return function;
}

FunctionType*
CallTrampolineDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 2, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));           // target
  function->InitParameter(1, UntaggedIntegral32(zone));  // actual #arguments
  return function;
}

FunctionType* ConstructStubDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 4, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));           // target
  function->InitParameter(1, AnyTagged(zone));           // new.target
  function->InitParameter(2, UntaggedIntegral32(zone));  // actual #arguments
  function->InitParameter(3, AnyTagged(zone));           // opt. allocation site
  return function;
}

FunctionType*
ConstructTrampolineDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 3, zone)->AsFunction();
  function->InitParameter(0, AnyTagged(zone));           // target
  function->InitParameter(1, AnyTagged(zone));           // new.target
  function->InitParameter(2, UntaggedIntegral32(zone));  // actual #arguments
  return function;
}

FunctionType*
CallFunctionWithFeedbackDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 2, zone)->AsFunction();
  function->InitParameter(0, Type::Receiver());  // JSFunction
  function->InitParameter(1, SmiType(zone));
  return function;
}

FunctionType* CallFunctionWithFeedbackAndVectorDescriptor::
    BuildCallInterfaceDescriptorFunctionType(Isolate* isolate,
                                             int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 3, zone)->AsFunction();
  function->InitParameter(0, Type::Receiver());  // JSFunction
  function->InitParameter(1, SmiType(zone));
  function->InitParameter(2, AnyTagged(zone));
  return function;
}

FunctionType*
ArrayConstructorDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 3, zone)->AsFunction();
  function->InitParameter(0, Type::Receiver());  // JSFunction
  function->InitParameter(1, AnyTagged(zone));
  function->InitParameter(2, UntaggedIntegral32(zone));
  return function;
}

FunctionType*
InternalArrayConstructorDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 2, zone)->AsFunction();
  function->InitParameter(0, Type::Receiver());  // JSFunction
  function->InitParameter(1, UntaggedIntegral32(zone));
  return function;
}

FunctionType*
ArgumentAdaptorDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int paramater_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 4, zone)->AsFunction();
  function->InitParameter(0, Type::Receiver());          // JSFunction
  function->InitParameter(1, AnyTagged(zone));           // the new target
  function->InitParameter(2, UntaggedIntegral32(zone));  // actual #arguments
  function->InitParameter(3, UntaggedIntegral32(zone));  // expected #arguments
  return function;
}

CallInterfaceDescriptor ApiCallbackDescriptorBase::ForArgs(Isolate* isolate,
                                                           int argc) {
  switch (argc) {
    case 0:
      return ApiCallbackWith0ArgsDescriptor(isolate);
    case 1:
      return ApiCallbackWith1ArgsDescriptor(isolate);
    case 2:
      return ApiCallbackWith2ArgsDescriptor(isolate);
    case 3:
      return ApiCallbackWith3ArgsDescriptor(isolate);
    case 4:
      return ApiCallbackWith4ArgsDescriptor(isolate);
    case 5:
      return ApiCallbackWith5ArgsDescriptor(isolate);
    case 6:
      return ApiCallbackWith6ArgsDescriptor(isolate);
    case 7:
      return ApiCallbackWith7ArgsDescriptor(isolate);
    default:
      UNREACHABLE();
      return VoidDescriptor(isolate);
  }
}

FunctionType*
ApiCallbackDescriptorBase::BuildCallInterfaceDescriptorFunctionTypeWithArg(
    Isolate* isolate, int parameter_count, int argc) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 4 + argc, zone)
          ->AsFunction();
  function->InitParameter(0, AnyTagged(zone));        // callee
  function->InitParameter(1, AnyTagged(zone));        // call_data
  function->InitParameter(2, AnyTagged(zone));        // holder
  function->InitParameter(3, ExternalPointer(zone));  // api_function_address
  for (int i = 0; i < argc; i++) {
    function->InitParameter(i, AnyTagged(zone));
  }
  return function;
}

FunctionType*
InterpreterDispatchDescriptor::BuildCallInterfaceDescriptorFunctionType(
    Isolate* isolate, int parameter_count) {
  Zone* zone = isolate->interface_descriptor_zone();
  FunctionType* function =
      Type::Function(AnyTagged(zone), Type::Undefined(), 5, zone)->AsFunction();
  function->InitParameter(kAccumulatorParameter, AnyTagged(zone));
  function->InitParameter(kRegisterFileParameter, ExternalPointer(zone));
  function->InitParameter(kBytecodeOffsetParameter, UntaggedIntegral32(zone));
  function->InitParameter(kBytecodeArrayParameter, AnyTagged(zone));
  function->InitParameter(kDispatchTableParameter, AnyTagged(zone));
  return function;
}

}  // namespace internal
}  // namespace v8
