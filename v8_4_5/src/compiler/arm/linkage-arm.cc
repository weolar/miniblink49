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

struct ArmLinkageHelperTraits {
  static Register ReturnValueReg() { return r0; }
  static Register ReturnValue2Reg() { return r1; }
  static Register JSCallFunctionReg() { return r1; }
  static Register ContextReg() { return cp; }
  static Register RuntimeCallFunctionReg() { return r1; }
  static Register RuntimeCallArgCountReg() { return r0; }
  static RegList CCalleeSaveRegisters() {
    return r4.bit() | r5.bit() | r6.bit() | r7.bit() | r8.bit() | r9.bit() |
           r10.bit();
  }
  static RegList CCalleeSaveFPRegisters() {
    return (1 << d8.code()) | (1 << d9.code()) | (1 << d10.code()) |
           (1 << d11.code()) | (1 << d12.code()) | (1 << d13.code()) |
           (1 << d14.code()) | (1 << d15.code());
  }
  static Register CRegisterParameter(int i) {
    static Register register_parameters[] = {r0, r1, r2, r3};
    return register_parameters[i];
  }
  static int CRegisterParametersLength() { return 4; }
  static int CStackBackingStoreLength() { return 0; }
};


typedef LinkageHelper<ArmLinkageHelperTraits> LH;

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
