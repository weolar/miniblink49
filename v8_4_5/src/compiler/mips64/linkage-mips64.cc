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

struct MipsLinkageHelperTraits {
  static Register ReturnValueReg() { return v0; }
  static Register ReturnValue2Reg() { return v1; }
  static Register JSCallFunctionReg() { return a1; }
  static Register ContextReg() { return cp; }
  static Register RuntimeCallFunctionReg() { return a1; }
  static Register RuntimeCallArgCountReg() { return a0; }
  static RegList CCalleeSaveRegisters() {
    return s0.bit() | s1.bit() | s2.bit() | s3.bit() | s4.bit() | s5.bit() |
           s6.bit() | s7.bit();
  }
  static RegList CCalleeSaveFPRegisters() {
    return f20.bit() | f22.bit() | f24.bit() | f26.bit() | f28.bit() |
           f30.bit();
  }
  static Register CRegisterParameter(int i) {
    static Register register_parameters[] = {a0, a1, a2, a3, a4, a5, a6, a7};
    return register_parameters[i];
  }
  static int CRegisterParametersLength() { return 8; }
  static int CStackBackingStoreLength() { return 0; }
};


typedef LinkageHelper<MipsLinkageHelperTraits> LH;

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
