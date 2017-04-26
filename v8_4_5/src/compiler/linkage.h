// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_LINKAGE_H_
#define V8_COMPILER_LINKAGE_H_

#include "src/base/flags.h"
#include "src/compiler/frame.h"
#include "src/compiler/machine-type.h"
#include "src/compiler/operator.h"
#include "src/frames.h"
#include "src/runtime/runtime.h"
#include "src/zone.h"

namespace v8 {
namespace internal {

class CallInterfaceDescriptor;

namespace compiler {

class Node;
class OsrHelper;

// Describes the location for a parameter or a return value to a call.
class LinkageLocation {
 public:
  explicit LinkageLocation(int location) : location_(location) {}

  bool is_register() const {
    return 0 <= location_ && location_ <= ANY_REGISTER;
  }

  static const int16_t ANY_REGISTER = 1023;
  static const int16_t MAX_STACK_SLOT = 32767;

  static LinkageLocation AnyRegister() { return LinkageLocation(ANY_REGISTER); }

  bool operator==(const LinkageLocation& other) const {
    return location_ == other.location_;
  }

  bool operator!=(const LinkageLocation& other) const {
    return !(*this == other);
  }

 private:
  friend class CallDescriptor;
  friend class OperandGenerator;
  //         location < 0     -> a stack slot on the caller frame
  // 0    <= location < 1023  -> a specific machine register
  // 1023 <= location < 1024  -> any machine register
  // 1024 <= location         -> a stack slot in the callee frame
  int16_t location_;
};

typedef Signature<LinkageLocation> LocationSignature;

// Describes a call to various parts of the compiler. Every call has the notion
// of a "target", which is the first input to the call.
class CallDescriptor final : public ZoneObject {
 public:
  // Describes the kind of this call, which determines the target.
  enum Kind {
    kCallCodeObject,  // target is a Code object
    kCallJSFunction,  // target is a JSFunction object
    kCallAddress      // target is a machine pointer
  };

  enum Flag {
    kNoFlags = 0u,
    kNeedsFrameState = 1u << 0,
    kPatchableCallSite = 1u << 1,
    kNeedsNopAfterCall = 1u << 2,
    kHasExceptionHandler = 1u << 3,
    kHasLocalCatchHandler = 1u << 4,
    kSupportsTailCalls = 1u << 5,
    kPatchableCallSiteWithNop = kPatchableCallSite | kNeedsNopAfterCall
  };
  typedef base::Flags<Flag> Flags;

  CallDescriptor(Kind kind, MachineType target_type, LinkageLocation target_loc,
                 const MachineSignature* machine_sig,
                 LocationSignature* location_sig, size_t js_param_count,
                 Operator::Properties properties,
                 RegList callee_saved_registers,
                 RegList callee_saved_fp_registers, Flags flags,
                 const char* debug_name = "")
      : kind_(kind),
        target_type_(target_type),
        target_loc_(target_loc),
        machine_sig_(machine_sig),
        location_sig_(location_sig),
        js_param_count_(js_param_count),
        properties_(properties),
        callee_saved_registers_(callee_saved_registers),
        callee_saved_fp_registers_(callee_saved_fp_registers),
        flags_(flags),
        debug_name_(debug_name) {
    DCHECK(machine_sig->return_count() == location_sig->return_count());
    DCHECK(machine_sig->parameter_count() == location_sig->parameter_count());
  }

  // Returns the kind of this call.
  Kind kind() const { return kind_; }

  // Returns {true} if this descriptor is a call to a C function.
  bool IsCFunctionCall() const { return kind_ == kCallAddress; }

  // Returns {true} if this descriptor is a call to a JSFunction.
  bool IsJSFunctionCall() const { return kind_ == kCallJSFunction; }

  // The number of return values from this call.
  size_t ReturnCount() const { return machine_sig_->return_count(); }

  // The number of C parameters to this call.
  size_t CParameterCount() const { return machine_sig_->parameter_count(); }

  // The number of JavaScript parameters to this call, including the receiver
  // object.
  size_t JSParameterCount() const { return js_param_count_; }

  // The total number of inputs to this call, which includes the target,
  // receiver, context, etc.
  // TODO(titzer): this should input the framestate input too.
  size_t InputCount() const { return 1 + machine_sig_->parameter_count(); }

  size_t FrameStateCount() const { return NeedsFrameState() ? 1 : 0; }

  Flags flags() const { return flags_; }

  bool NeedsFrameState() const { return flags() & kNeedsFrameState; }
  bool SupportsTailCalls() const { return flags() & kSupportsTailCalls; }

  LinkageLocation GetReturnLocation(size_t index) const {
    return location_sig_->GetReturn(index);
  }

  LinkageLocation GetInputLocation(size_t index) const {
    if (index == 0) return target_loc_;
    return location_sig_->GetParam(index - 1);
  }

  const MachineSignature* GetMachineSignature() const { return machine_sig_; }

  MachineType GetReturnType(size_t index) const {
    return machine_sig_->GetReturn(index);
  }

  MachineType GetInputType(size_t index) const {
    if (index == 0) return target_type_;
    return machine_sig_->GetParam(index - 1);
  }

  // Operator properties describe how this call can be optimized, if at all.
  Operator::Properties properties() const { return properties_; }

  // Get the callee-saved registers, if any, across this call.
  RegList CalleeSavedRegisters() const { return callee_saved_registers_; }

  // Get the callee-saved FP registers, if any, across this call.
  RegList CalleeSavedFPRegisters() const { return callee_saved_fp_registers_; }

  const char* debug_name() const { return debug_name_; }

  bool UsesOnlyRegisters() const;

  bool HasSameReturnLocationsAs(const CallDescriptor* other) const;

  bool CanTailCall(const Node* call) const;

 private:
  friend class Linkage;

  const Kind kind_;
  const MachineType target_type_;
  const LinkageLocation target_loc_;
  const MachineSignature* const machine_sig_;
  const LocationSignature* const location_sig_;
  const size_t js_param_count_;
  const Operator::Properties properties_;
  const RegList callee_saved_registers_;
  const RegList callee_saved_fp_registers_;
  const Flags flags_;
  const char* const debug_name_;

  DISALLOW_COPY_AND_ASSIGN(CallDescriptor);
};

DEFINE_OPERATORS_FOR_FLAGS(CallDescriptor::Flags)

std::ostream& operator<<(std::ostream& os, const CallDescriptor& d);
std::ostream& operator<<(std::ostream& os, const CallDescriptor::Kind& k);

// Defines the linkage for a compilation, including the calling conventions
// for incoming parameters and return value(s) as well as the outgoing calling
// convention for any kind of call. Linkage is generally architecture-specific.
//
// Can be used to translate {arg_index} (i.e. index of the call node input) as
// well as {param_index} (i.e. as stored in parameter nodes) into an operator
// representing the architecture-specific location. The following call node
// layouts are supported (where {n} is the number value inputs):
//
//                  #0          #1     #2     #3     [...]             #n
// Call[CodeStub]   code,       arg 1, arg 2, arg 3, [...],            context
// Call[JSFunction] function,   rcvr,  arg 1, arg 2, [...],            context
// Call[Runtime]    CEntryStub, arg 1, arg 2, arg 3, [...], fun, #arg, context
class Linkage : public ZoneObject {
 public:
  explicit Linkage(CallDescriptor* incoming) : incoming_(incoming) {}

  static CallDescriptor* ComputeIncoming(Zone* zone, CompilationInfo* info);

  // The call descriptor for this compilation unit describes the locations
  // of incoming parameters and the outgoing return value(s).
  CallDescriptor* GetIncomingDescriptor() const { return incoming_; }
  static CallDescriptor* GetJSCallDescriptor(Zone* zone, bool is_osr,
                                             int parameter_count,
                                             CallDescriptor::Flags flags);
  static CallDescriptor* GetRuntimeCallDescriptor(
      Zone* zone, Runtime::FunctionId function, int parameter_count,
      Operator::Properties properties);

  static CallDescriptor* GetStubCallDescriptor(
      Isolate* isolate, Zone* zone, const CallInterfaceDescriptor& descriptor,
      int stack_parameter_count, CallDescriptor::Flags flags,
      Operator::Properties properties = Operator::kNoProperties,
      MachineType return_type = kMachAnyTagged);

  // Creates a call descriptor for simplified C calls that is appropriate
  // for the host platform. This simplified calling convention only supports
  // integers and pointers of one word size each, i.e. no floating point,
  // structs, pointers to members, etc.
  static CallDescriptor* GetSimplifiedCDescriptor(Zone* zone,
                                                  const MachineSignature* sig);

  // Get the location of an (incoming) parameter to this function.
  LinkageLocation GetParameterLocation(int index) const {
    return incoming_->GetInputLocation(index + 1);  // + 1 to skip target.
  }

  // Get the machine type of an (incoming) parameter to this function.
  MachineType GetParameterType(int index) const {
    return incoming_->GetInputType(index + 1);  // + 1 to skip target.
  }

  // Get the location where this function should place its return value.
  LinkageLocation GetReturnLocation() const {
    return incoming_->GetReturnLocation(0);
  }

  // Get the machine type of this function's return value.
  MachineType GetReturnType() const { return incoming_->GetReturnType(0); }

  // Get the frame offset for a given spill slot. The location depends on the
  // calling convention and the specific frame layout, and may thus be
  // architecture-specific. Negative spill slots indicate arguments on the
  // caller's frame. The {extra} parameter indicates an additional offset from
  // the frame offset, e.g. to index into part of a double slot.
  FrameOffset GetFrameOffset(int spill_slot, Frame* frame, int extra = 0) const;

  static int FrameStateInputCount(Runtime::FunctionId function);

  // Get the location where an incoming OSR value is stored.
  LinkageLocation GetOsrValueLocation(int index) const;

  // A special parameter index for JSCalls that represents the closure.
  static const int kJSFunctionCallClosureParamIndex = -1;

  // A special {OsrValue} index to indicate the context spill slot.
  static const int kOsrContextSpillSlotIndex = -1;

 private:
  CallDescriptor* const incoming_;

  DISALLOW_COPY_AND_ASSIGN(Linkage);
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_LINKAGE_H_
