// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_LINKAGE_IMPL_H_
#define V8_COMPILER_LINKAGE_IMPL_H_

#include "src/code-stubs.h"
#include "src/compiler/osr.h"

namespace v8 {
namespace internal {
namespace compiler {

// TODO(titzer): replace uses of int with size_t in LinkageHelper.
template <typename LinkageTraits>
class LinkageHelper {
 public:
  static const RegList kNoCalleeSaved = 0;

  static void AddReturnLocations(LocationSignature::Builder* locations) {
    DCHECK(locations->return_count_ <= 2);
    if (locations->return_count_ > 0) {
      locations->AddReturn(regloc(LinkageTraits::ReturnValueReg()));
    }
    if (locations->return_count_ > 1) {
      locations->AddReturn(regloc(LinkageTraits::ReturnValue2Reg()));
    }
  }

  // TODO(turbofan): cache call descriptors for JSFunction calls.
  static CallDescriptor* GetJSCallDescriptor(Zone* zone, bool is_osr,
                                             int js_parameter_count,
                                             CallDescriptor::Flags flags) {
    const size_t return_count = 1;
    const size_t context_count = 1;
    const size_t parameter_count = js_parameter_count + context_count;

    LocationSignature::Builder locations(zone, return_count, parameter_count);
    MachineSignature::Builder types(zone, return_count, parameter_count);

    // Add returns.
    AddReturnLocations(&locations);
    for (size_t i = 0; i < return_count; i++) {
      types.AddReturn(kMachAnyTagged);
    }

    // All parameters to JS calls go on the stack.
    for (int i = 0; i < js_parameter_count; i++) {
      int spill_slot_index = i - js_parameter_count;
      locations.AddParam(stackloc(spill_slot_index));
      types.AddParam(kMachAnyTagged);
    }
    // Add context.
    locations.AddParam(regloc(LinkageTraits::ContextReg()));
    types.AddParam(kMachAnyTagged);

    // The target for JS function calls is the JSFunction object.
    MachineType target_type = kMachAnyTagged;
    // TODO(titzer): When entering into an OSR function from unoptimized code,
    // the JSFunction is not in a register, but it is on the stack in an
    // unaddressable spill slot. We hack this in the OSR prologue. Fix.
    LinkageLocation target_loc = regloc(LinkageTraits::JSCallFunctionReg());
    return new (zone) CallDescriptor(     // --
        CallDescriptor::kCallJSFunction,  // kind
        target_type,                      // target MachineType
        target_loc,                       // target location
        types.Build(),                    // machine_sig
        locations.Build(),                // location_sig
        js_parameter_count,               // js_parameter_count
        Operator::kNoProperties,          // properties
        kNoCalleeSaved,                   // callee-saved
        kNoCalleeSaved,                   // callee-saved fp
        flags,                            // flags
        "js-call");
  }


  // TODO(turbofan): cache call descriptors for runtime calls.
  static CallDescriptor* GetRuntimeCallDescriptor(
      Zone* zone, Runtime::FunctionId function_id, int js_parameter_count,
      Operator::Properties properties) {
    const size_t function_count = 1;
    const size_t num_args_count = 1;
    const size_t context_count = 1;
    const size_t parameter_count = function_count +
                                   static_cast<size_t>(js_parameter_count) +
                                   num_args_count + context_count;

    const Runtime::Function* function = Runtime::FunctionForId(function_id);
    const size_t return_count = static_cast<size_t>(function->result_size);

    LocationSignature::Builder locations(zone, return_count, parameter_count);
    MachineSignature::Builder types(zone, return_count, parameter_count);

    // Add returns.
    AddReturnLocations(&locations);
    for (size_t i = 0; i < return_count; i++) {
      types.AddReturn(kMachAnyTagged);
    }

    // All parameters to the runtime call go on the stack.
    for (int i = 0; i < js_parameter_count; i++) {
      locations.AddParam(stackloc(i - js_parameter_count));
      types.AddParam(kMachAnyTagged);
    }
    // Add runtime function itself.
    locations.AddParam(regloc(LinkageTraits::RuntimeCallFunctionReg()));
    types.AddParam(kMachAnyTagged);

    // Add runtime call argument count.
    locations.AddParam(regloc(LinkageTraits::RuntimeCallArgCountReg()));
    types.AddParam(kMachPtr);

    // Add context.
    locations.AddParam(regloc(LinkageTraits::ContextReg()));
    types.AddParam(kMachAnyTagged);

    CallDescriptor::Flags flags = Linkage::FrameStateInputCount(function_id) > 0
                                      ? CallDescriptor::kNeedsFrameState
                                      : CallDescriptor::kNoFlags;

    // The target for runtime calls is a code object.
    MachineType target_type = kMachAnyTagged;
    LinkageLocation target_loc = LinkageLocation::AnyRegister();
    return new (zone) CallDescriptor(     // --
        CallDescriptor::kCallCodeObject,  // kind
        target_type,                      // target MachineType
        target_loc,                       // target location
        types.Build(),                    // machine_sig
        locations.Build(),                // location_sig
        js_parameter_count,               // js_parameter_count
        properties,                       // properties
        kNoCalleeSaved,                   // callee-saved
        kNoCalleeSaved,                   // callee-saved fp
        flags,                            // flags
        function->name);                  // debug name
  }


  // TODO(all): Add support for return representations/locations to
  // CallInterfaceDescriptor.
  // TODO(turbofan): cache call descriptors for code stub calls.
  static CallDescriptor* GetStubCallDescriptor(
      Isolate* isolate, Zone* zone, const CallInterfaceDescriptor& descriptor,
      int stack_parameter_count, CallDescriptor::Flags flags,
      Operator::Properties properties, MachineType return_type) {
    const int register_parameter_count = descriptor.GetRegisterParameterCount();
    const int js_parameter_count =
        register_parameter_count + stack_parameter_count;
    const int context_count = 1;
    const size_t return_count = 1;
    const size_t parameter_count =
        static_cast<size_t>(js_parameter_count + context_count);

    LocationSignature::Builder locations(zone, return_count, parameter_count);
    MachineSignature::Builder types(zone, return_count, parameter_count);

    // Add return location.
    AddReturnLocations(&locations);
    types.AddReturn(return_type);

    // Add parameters in registers and on the stack.
    for (int i = 0; i < js_parameter_count; i++) {
      if (i < register_parameter_count) {
        // The first parameters go in registers.
        Register reg = descriptor.GetRegisterParameter(i);
        Representation rep =
            RepresentationFromType(descriptor.GetParameterType(i));
        locations.AddParam(regloc(reg));
        types.AddParam(reptyp(rep));
      } else {
        // The rest of the parameters go on the stack.
        int stack_slot = i - register_parameter_count - stack_parameter_count;
        locations.AddParam(stackloc(stack_slot));
        types.AddParam(kMachAnyTagged);
      }
    }
    // Add context.
    locations.AddParam(regloc(LinkageTraits::ContextReg()));
    types.AddParam(kMachAnyTagged);

    // The target for stub calls is a code object.
    MachineType target_type = kMachAnyTagged;
    LinkageLocation target_loc = LinkageLocation::AnyRegister();
    return new (zone) CallDescriptor(     // --
        CallDescriptor::kCallCodeObject,  // kind
        target_type,                      // target MachineType
        target_loc,                       // target location
        types.Build(),                    // machine_sig
        locations.Build(),                // location_sig
        js_parameter_count,               // js_parameter_count
        properties,                       // properties
        kNoCalleeSaved,                   // callee-saved registers
        kNoCalleeSaved,                   // callee-saved fp
        flags,                            // flags
        descriptor.DebugName(isolate));
  }

  static CallDescriptor* GetSimplifiedCDescriptor(
      Zone* zone, const MachineSignature* msig) {
    LocationSignature::Builder locations(zone, msig->return_count(),
                                         msig->parameter_count());
    // Add return location(s).
    AddReturnLocations(&locations);

    // Add register and/or stack parameter(s).
    const int parameter_count = static_cast<int>(msig->parameter_count());
    int stack_offset = LinkageTraits::CStackBackingStoreLength();
    for (int i = 0; i < parameter_count; i++) {
      if (i < LinkageTraits::CRegisterParametersLength()) {
        locations.AddParam(regloc(LinkageTraits::CRegisterParameter(i)));
      } else {
        locations.AddParam(stackloc(-1 - stack_offset));
        stack_offset++;
      }
    }

    // The target for C calls is always an address (i.e. machine pointer).
    MachineType target_type = kMachPtr;
    LinkageLocation target_loc = LinkageLocation::AnyRegister();
    return new (zone) CallDescriptor(             // --
        CallDescriptor::kCallAddress,             // kind
        target_type,                              // target MachineType
        target_loc,                               // target location
        msig,                                     // machine_sig
        locations.Build(),                        // location_sig
        0,                                        // js_parameter_count
        Operator::kNoProperties,                  // properties
        LinkageTraits::CCalleeSaveRegisters(),    // callee-saved registers
        LinkageTraits::CCalleeSaveFPRegisters(),  // callee-saved fp regs
        CallDescriptor::kNoFlags,                 // flags
        "c-call");
  }

  static LinkageLocation regloc(Register reg) {
    return LinkageLocation(Register::ToAllocationIndex(reg));
  }

  static LinkageLocation stackloc(int i) {
    DCHECK_LT(i, 0);
    return LinkageLocation(i);
  }

  static MachineType reptyp(Representation representation) {
    switch (representation.kind()) {
      case Representation::kInteger8:
        return kMachInt8;
      case Representation::kUInteger8:
        return kMachUint8;
      case Representation::kInteger16:
        return kMachInt16;
      case Representation::kUInteger16:
        return kMachUint16;
      case Representation::kInteger32:
        return kMachInt32;
      case Representation::kSmi:
      case Representation::kTagged:
      case Representation::kHeapObject:
        return kMachAnyTagged;
      case Representation::kDouble:
        return kMachFloat64;
      case Representation::kExternal:
        return kMachPtr;
      case Representation::kNone:
      case Representation::kNumRepresentations:
        break;
    }
    UNREACHABLE();
    return kMachNone;
  }
};


LinkageLocation Linkage::GetOsrValueLocation(int index) const {
  CHECK(incoming_->IsJSFunctionCall());
  int parameter_count = static_cast<int>(incoming_->JSParameterCount() - 1);
  int first_stack_slot = OsrHelper::FirstStackSlotIndex(parameter_count);

  if (index == kOsrContextSpillSlotIndex) {
    // Context. Use the parameter location of the context spill slot.
    // Parameter (arity + 1) is special for the context of the function frame.
    int context_index = 1 + 1 + parameter_count;  // target + receiver + params
    return incoming_->GetInputLocation(context_index);
  } else if (index >= first_stack_slot) {
    // Local variable stored in this (callee) stack.
    int spill_index =
        LinkageLocation::ANY_REGISTER + 1 + index - first_stack_slot;
    // TODO(titzer): bailout instead of crashing here.
    CHECK(spill_index <= LinkageLocation::MAX_STACK_SLOT);
    return LinkageLocation(spill_index);
  } else {
    // Parameter. Use the assigned location from the incoming call descriptor.
    int parameter_index = 1 + index;  // skip index 0, which is the target.
    return incoming_->GetInputLocation(parameter_index);
  }
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_LINKAGE_IMPL_H_
