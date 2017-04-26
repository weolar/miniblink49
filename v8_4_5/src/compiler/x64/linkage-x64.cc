// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/assembler.h"
#include "src/code-stubs.h"
#include "src/compiler/linkage.h"
#include "src/compiler/linkage-impl.h"
#include "src/zone.h"

namespace v8 {
namespace internal {
namespace compiler {

#ifdef _WIN64
const bool kWin64 = true;
#else
const bool kWin64 = false;
#endif

struct X64LinkageHelperTraits {
  static Register ReturnValueReg() { return rax; }
  static Register ReturnValue2Reg() { return rdx; }
  static Register JSCallFunctionReg() { return rdi; }
  static Register ContextReg() { return rsi; }
  static Register RuntimeCallFunctionReg() { return rbx; }
  static Register RuntimeCallArgCountReg() { return rax; }
  static RegList CCalleeSaveRegisters() {
    if (kWin64) {
      return rbx.bit() | rdi.bit() | rsi.bit() | r12.bit() | r13.bit() |
             r14.bit() | r15.bit();
    } else {
      return rbx.bit() | r12.bit() | r13.bit() | r14.bit() | r15.bit();
    }
  }
  static RegList CCalleeSaveFPRegisters() {
    if (kWin64) {
      return (1 << xmm6.code()) | (1 << xmm7.code()) | (1 << xmm8.code()) |
             (1 << xmm9.code()) | (1 << xmm10.code()) | (1 << xmm11.code()) |
             (1 << xmm12.code()) | (1 << xmm13.code()) | (1 << xmm14.code()) |
             (1 << xmm15.code());
    } else {
      return 0;
    }
  }
  static Register CRegisterParameter(int i) {
    if (kWin64) {
      static Register register_parameters[] = {rcx, rdx, r8, r9};
      return register_parameters[i];
    } else {
      static Register register_parameters[] = {rdi, rsi, rdx, rcx, r8, r9};
      return register_parameters[i];
    }
  }
  static int CRegisterParametersLength() { return kWin64 ? 4 : 6; }
  static int CStackBackingStoreLength() { return kWin64 ? 4 : 0; }
};

typedef LinkageHelper<X64LinkageHelperTraits> LH;

CallDescriptor* Linkage::GetJSCallDescriptor(Zone* zone, bool is_osr,
                                             int parameter_count,
                                             CallDescriptor::Flags flags) {
  return LH::GetJSCallDescriptor(zone, is_osr, parameter_count, flags);
}


CallDescriptor* Linkage::GetRuntimeCallDescriptor(
    Zone* zone, Runtime::FunctionId function, int parameter_count,
    Operator::Properties properties) {
  return LH::GetRuntimeCallDescriptor(zone, function, parameter_count,
                                      properties);
}


CallDescriptor* Linkage::GetStubCallDescriptor(
    Isolate* isolate, Zone* zone, const CallInterfaceDescriptor& descriptor,
    int stack_parameter_count, CallDescriptor::Flags flags,
    Operator::Properties properties, MachineType return_type) {
  return LH::GetStubCallDescriptor(isolate, zone, descriptor,
                                   stack_parameter_count, flags, properties,
                                   return_type);
}


CallDescriptor* Linkage::GetSimplifiedCDescriptor(Zone* zone,
                                                  const MachineSignature* sig) {
  return LH::GetSimplifiedCDescriptor(zone, sig);
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
