// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/code-stubs.h"
#include "src/compiler.h"
#include "src/compiler/common-operator.h"
#include "src/compiler/linkage.h"
#include "src/compiler/node.h"
#include "src/compiler/pipeline.h"
#include "src/scopes.h"

namespace v8 {
namespace internal {
namespace compiler {


std::ostream& operator<<(std::ostream& os, const CallDescriptor::Kind& k) {
  switch (k) {
    case CallDescriptor::kCallCodeObject:
      os << "Code";
      break;
    case CallDescriptor::kCallJSFunction:
      os << "JS";
      break;
    case CallDescriptor::kCallAddress:
      os << "Addr";
      break;
  }
  return os;
}


std::ostream& operator<<(std::ostream& os, const CallDescriptor& d) {
  // TODO(svenpanne) Output properties etc. and be less cryptic.
  return os << d.kind() << ":" << d.debug_name() << ":r" << d.ReturnCount()
            << "j" << d.JSParameterCount() << "i" << d.InputCount() << "f"
            << d.FrameStateCount() << "t" << d.SupportsTailCalls();
}


bool CallDescriptor::HasSameReturnLocationsAs(
    const CallDescriptor* other) const {
  if (ReturnCount() != other->ReturnCount()) return false;
  for (size_t i = 0; i < ReturnCount(); ++i) {
    if (GetReturnLocation(i) != other->GetReturnLocation(i)) return false;
  }
  return true;
}


bool CallDescriptor::CanTailCall(const Node* node) const {
  // Tail calling is currently allowed if return locations match and all
  // parameters are either in registers or on the stack but match exactly in
  // number and content.
  CallDescriptor const* other = OpParameter<CallDescriptor const*>(node);
  if (!HasSameReturnLocationsAs(other)) return false;
  size_t current_input = 0;
  size_t other_input = 0;
  size_t stack_parameter = 0;
  while (true) {
    if (other_input >= other->InputCount()) {
      while (current_input <= InputCount()) {
        if (!GetInputLocation(current_input).is_register()) {
          return false;
        }
        ++current_input;
      }
      return true;
    }
    if (current_input >= InputCount()) {
      while (other_input < other->InputCount()) {
        if (!other->GetInputLocation(other_input).is_register()) {
          return false;
        }
        ++other_input;
      }
      return true;
    }
    if (GetInputLocation(current_input).is_register()) {
      ++current_input;
      continue;
    }
    if (other->GetInputLocation(other_input).is_register()) {
      ++other_input;
      continue;
    }
    if (GetInputLocation(current_input) !=
        other->GetInputLocation(other_input)) {
      return false;
    }
    Node* input = node->InputAt(static_cast<int>(other_input));
    if (input->opcode() != IrOpcode::kParameter) {
      return false;
    }
    size_t param_index = ParameterIndexOf(input->op());
    if (param_index != stack_parameter) {
      return false;
    }
    ++stack_parameter;
    ++current_input;
    ++other_input;
  }
  UNREACHABLE();
  return false;
}


CallDescriptor* Linkage::ComputeIncoming(Zone* zone, CompilationInfo* info) {
  if (info->code_stub() != NULL) {
    // Use the code stub interface descriptor.
    CodeStub* stub = info->code_stub();
    CallInterfaceDescriptor descriptor = stub->GetCallInterfaceDescriptor();
    return GetStubCallDescriptor(
        info->isolate(), zone, descriptor, stub->GetStackParameterCount(),
        CallDescriptor::kNoFlags, Operator::kNoProperties);
  }
  if (info->function() != NULL) {
    // If we already have the function literal, use the number of parameters
    // plus the receiver.
    return GetJSCallDescriptor(zone, info->is_osr(),
                               1 + info->function()->parameter_count(),
                               CallDescriptor::kNoFlags);
  }
  if (!info->closure().is_null()) {
    // If we are compiling a JS function, use a JS call descriptor,
    // plus the receiver.
    SharedFunctionInfo* shared = info->closure()->shared();
    return GetJSCallDescriptor(zone, info->is_osr(),
                               1 + shared->internal_formal_parameter_count(),
                               CallDescriptor::kNoFlags);
  }
  return NULL;  // TODO(titzer): ?
}


FrameOffset Linkage::GetFrameOffset(int spill_slot, Frame* frame,
                                    int extra) const {
  if (frame->GetSpillSlotCount() > 0 || incoming_->IsJSFunctionCall() ||
      incoming_->kind() == CallDescriptor::kCallAddress) {
    int offset;
    int register_save_area_size = frame->GetRegisterSaveAreaSize();
    if (spill_slot >= 0) {
      // Local or spill slot. Skip the frame pointer, function, and
      // context in the fixed part of the frame.
      offset =
          -(spill_slot + 1) * kPointerSize - register_save_area_size + extra;
    } else {
      // Incoming parameter. Skip the return address.
      offset = -(spill_slot + 1) * kPointerSize + kFPOnStackSize +
               kPCOnStackSize + extra;
    }
    return FrameOffset::FromFramePointer(offset);
  } else {
    // No frame. Retrieve all parameters relative to stack pointer.
    DCHECK(spill_slot < 0);  // Must be a parameter.
    int register_save_area_size = frame->GetRegisterSaveAreaSize();
    int offset = register_save_area_size - (spill_slot + 1) * kPointerSize +
                 kPCOnStackSize + extra;
    return FrameOffset::FromStackPointer(offset);
  }
}


// static
int Linkage::FrameStateInputCount(Runtime::FunctionId function) {
  // Most runtime functions need a FrameState. A few chosen ones that we know
  // not to call into arbitrary JavaScript, not to throw, and not to deoptimize
  // are blacklisted here and can be called without a FrameState.
  switch (function) {
    case Runtime::kAllocateInTargetSpace:
    case Runtime::kDateField:
    case Runtime::kDefineClassMethod:              // TODO(jarin): Is it safe?
    case Runtime::kDefineGetterPropertyUnchecked:  // TODO(jarin): Is it safe?
    case Runtime::kDefineSetterPropertyUnchecked:  // TODO(jarin): Is it safe?
    case Runtime::kForInDone:
    case Runtime::kForInStep:
    case Runtime::kGetOriginalConstructor:
    case Runtime::kNewArguments:
    case Runtime::kNewClosure:
    case Runtime::kNewFunctionContext:
    case Runtime::kNewRestParamSlow:
    case Runtime::kPushBlockContext:
    case Runtime::kPushCatchContext:
    case Runtime::kReThrow:
    case Runtime::kStringCompareRT:
    case Runtime::kStringEquals:
    case Runtime::kToFastProperties:  // TODO(jarin): Is it safe?
    case Runtime::kTraceEnter:
    case Runtime::kTraceExit:
      return 0;
    case Runtime::kInlineArguments:
    case Runtime::kInlineCallFunction:
    case Runtime::kInlineGetCallerJSFunction:
    case Runtime::kInlineGetPrototype:
    case Runtime::kInlineRegExpExec:
      return 1;
    case Runtime::kInlineDeoptimizeNow:
    case Runtime::kInlineThrowNotDateError:
      return 2;
    default:
      break;
  }

  // Most inlined runtime functions (except the ones listed above) can be called
  // without a FrameState or will be lowered by JSIntrinsicLowering internally.
  const Runtime::Function* const f = Runtime::FunctionForId(function);
  if (f->intrinsic_type == Runtime::IntrinsicType::INLINE) return 0;

  return 1;
}


bool CallDescriptor::UsesOnlyRegisters() const {
  for (size_t i = 0; i < InputCount(); ++i) {
    if (!GetInputLocation(i).is_register()) return false;
  }
  for (size_t i = 0; i < ReturnCount(); ++i) {
    if (!GetReturnLocation(i).is_register()) return false;
  }
  return true;
}


//==============================================================================
// Provide unimplemented methods on unsupported architectures, to at least link.
//==============================================================================
#if !V8_TURBOFAN_BACKEND
CallDescriptor* Linkage::GetJSCallDescriptor(Zone* zone, bool is_osr,
                                             int parameter_count,
                                             CallDescriptor::Flags flags) {
  UNIMPLEMENTED();
  return NULL;
}


LinkageLocation Linkage::GetOsrValueLocation(int index) const {
  UNIMPLEMENTED();
  return LinkageLocation(-1);  // Dummy value
}


CallDescriptor* Linkage::GetRuntimeCallDescriptor(
    Zone* zone, Runtime::FunctionId function, int parameter_count,
    Operator::Properties properties) {
  UNIMPLEMENTED();
  return NULL;
}


CallDescriptor* Linkage::GetStubCallDescriptor(
    Isolate* isolate, Zone* zone, const CallInterfaceDescriptor& descriptor,
    int stack_parameter_count, CallDescriptor::Flags flags,
    Operator::Properties properties, MachineType return_type) {
  UNIMPLEMENTED();
  return NULL;
}


CallDescriptor* Linkage::GetSimplifiedCDescriptor(Zone* zone,
                                                  const MachineSignature* sig) {
  UNIMPLEMENTED();
  return NULL;
}
#endif  // !V8_TURBOFAN_BACKEND
}  // namespace compiler
}  // namespace internal
}  // namespace v8
