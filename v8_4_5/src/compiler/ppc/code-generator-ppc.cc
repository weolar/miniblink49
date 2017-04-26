// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/code-generator.h"

#include "src/compiler/code-generator-impl.h"
#include "src/compiler/gap-resolver.h"
#include "src/compiler/node-matchers.h"
#include "src/ppc/macro-assembler-ppc.h"
#include "src/scopes.h"

namespace v8 {
namespace internal {
namespace compiler {

#define __ masm()->


#define kScratchReg r11


// Adds PPC-specific methods to convert InstructionOperands.
class PPCOperandConverter final : public InstructionOperandConverter {
 public:
  PPCOperandConverter(CodeGenerator* gen, Instruction* instr)
      : InstructionOperandConverter(gen, instr) {}

  RCBit OutputRCBit() const {
    switch (instr_->flags_mode()) {
      case kFlags_branch:
      case kFlags_set:
        return SetRC;
      case kFlags_none:
        return LeaveRC;
    }
    UNREACHABLE();
    return LeaveRC;
  }

  bool CompareLogical() const {
    switch (instr_->flags_condition()) {
      case kUnsignedLessThan:
      case kUnsignedGreaterThanOrEqual:
      case kUnsignedLessThanOrEqual:
      case kUnsignedGreaterThan:
        return true;
      default:
        return false;
    }
    UNREACHABLE();
    return false;
  }

  Operand InputImmediate(size_t index) {
    Constant constant = ToConstant(instr_->InputAt(index));
    switch (constant.type()) {
      case Constant::kInt32:
        return Operand(constant.ToInt32());
      case Constant::kFloat32:
        return Operand(
            isolate()->factory()->NewNumber(constant.ToFloat32(), TENURED));
      case Constant::kFloat64:
        return Operand(
            isolate()->factory()->NewNumber(constant.ToFloat64(), TENURED));
      case Constant::kInt64:
#if V8_TARGET_ARCH_PPC64
        return Operand(constant.ToInt64());
#endif
      case Constant::kExternalReference:
      case Constant::kHeapObject:
      case Constant::kRpoNumber:
        break;
    }
    UNREACHABLE();
    return Operand::Zero();
  }

  MemOperand MemoryOperand(AddressingMode* mode, size_t* first_index) {
    const size_t index = *first_index;
    *mode = AddressingModeField::decode(instr_->opcode());
    switch (*mode) {
      case kMode_None:
        break;
      case kMode_MRI:
        *first_index += 2;
        return MemOperand(InputRegister(index + 0), InputInt32(index + 1));
      case kMode_MRR:
        *first_index += 2;
        return MemOperand(InputRegister(index + 0), InputRegister(index + 1));
    }
    UNREACHABLE();
    return MemOperand(r0);
  }

  MemOperand MemoryOperand(AddressingMode* mode, size_t first_index = 0) {
    return MemoryOperand(mode, &first_index);
  }

  MemOperand ToMemOperand(InstructionOperand* op) const {
    DCHECK(op != NULL);
    DCHECK(!op->IsRegister());
    DCHECK(!op->IsDoubleRegister());
    DCHECK(op->IsStackSlot() || op->IsDoubleStackSlot());
    // The linkage computes where all spill slots are located.
    FrameOffset offset = linkage()->GetFrameOffset(
        AllocatedOperand::cast(op)->index(), frame(), 0);
    return MemOperand(offset.from_stack_pointer() ? sp : fp, offset.offset());
  }
};


static inline bool HasRegisterInput(Instruction* instr, size_t index) {
  return instr->InputAt(index)->IsRegister();
}


namespace {

class OutOfLineLoadNAN32 final : public OutOfLineCode {
 public:
  OutOfLineLoadNAN32(CodeGenerator* gen, DoubleRegister result)
      : OutOfLineCode(gen), result_(result) {}

  void Generate() final {
    __ LoadDoubleLiteral(result_, std::numeric_limits<float>::quiet_NaN(),
                         kScratchReg);
  }

 private:
  DoubleRegister const result_;
};


class OutOfLineLoadNAN64 final : public OutOfLineCode {
 public:
  OutOfLineLoadNAN64(CodeGenerator* gen, DoubleRegister result)
      : OutOfLineCode(gen), result_(result) {}

  void Generate() final {
    __ LoadDoubleLiteral(result_, std::numeric_limits<double>::quiet_NaN(),
                         kScratchReg);
  }

 private:
  DoubleRegister const result_;
};


class OutOfLineLoadZero final : public OutOfLineCode {
 public:
  OutOfLineLoadZero(CodeGenerator* gen, Register result)
      : OutOfLineCode(gen), result_(result) {}

  void Generate() final { __ li(result_, Operand::Zero()); }

 private:
  Register const result_;
};


Condition FlagsConditionToCondition(FlagsCondition condition) {
  switch (condition) {
    case kEqual:
      return eq;
    case kNotEqual:
      return ne;
    case kSignedLessThan:
    case kUnsignedLessThan:
      return lt;
    case kSignedGreaterThanOrEqual:
    case kUnsignedGreaterThanOrEqual:
      return ge;
    case kSignedLessThanOrEqual:
    case kUnsignedLessThanOrEqual:
      return le;
    case kSignedGreaterThan:
    case kUnsignedGreaterThan:
      return gt;
    case kOverflow:
#if V8_TARGET_ARCH_PPC64
      return ne;
#else
      return lt;
#endif
    case kNotOverflow:
#if V8_TARGET_ARCH_PPC64
      return eq;
#else
      return ge;
#endif
    case kUnorderedEqual:
    case kUnorderedNotEqual:
      break;
  }
  UNREACHABLE();
  return kNoCondition;
}

}  // namespace

#define ASSEMBLE_FLOAT_UNOP_RC(asm_instr)                            \
  do {                                                               \
    __ asm_instr(i.OutputDoubleRegister(), i.InputDoubleRegister(0), \
                 i.OutputRCBit());                                   \
  } while (0)


#define ASSEMBLE_FLOAT_BINOP_RC(asm_instr)                           \
  do {                                                               \
    __ asm_instr(i.OutputDoubleRegister(), i.InputDoubleRegister(0), \
                 i.InputDoubleRegister(1), i.OutputRCBit());         \
  } while (0)


#define ASSEMBLE_BINOP(asm_instr_reg, asm_instr_imm)           \
  do {                                                         \
    if (HasRegisterInput(instr, 1)) {                          \
      __ asm_instr_reg(i.OutputRegister(), i.InputRegister(0), \
                       i.InputRegister(1));                    \
    } else {                                                   \
      __ asm_instr_imm(i.OutputRegister(), i.InputRegister(0), \
                       i.InputImmediate(1));                   \
    }                                                          \
  } while (0)


#define ASSEMBLE_BINOP_RC(asm_instr_reg, asm_instr_imm)        \
  do {                                                         \
    if (HasRegisterInput(instr, 1)) {                          \
      __ asm_instr_reg(i.OutputRegister(), i.InputRegister(0), \
                       i.InputRegister(1), i.OutputRCBit());   \
    } else {                                                   \
      __ asm_instr_imm(i.OutputRegister(), i.InputRegister(0), \
                       i.InputImmediate(1), i.OutputRCBit());  \
    }                                                          \
  } while (0)


#define ASSEMBLE_BINOP_INT_RC(asm_instr_reg, asm_instr_imm)    \
  do {                                                         \
    if (HasRegisterInput(instr, 1)) {                          \
      __ asm_instr_reg(i.OutputRegister(), i.InputRegister(0), \
                       i.InputRegister(1), i.OutputRCBit());   \
    } else {                                                   \
      __ asm_instr_imm(i.OutputRegister(), i.InputRegister(0), \
                       i.InputInt32(1), i.OutputRCBit());      \
    }                                                          \
  } while (0)


#if V8_TARGET_ARCH_PPC64
#define ASSEMBLE_ADD_WITH_OVERFLOW()             \
  do {                                           \
    ASSEMBLE_BINOP(add, addi);                   \
    __ TestIfInt32(i.OutputRegister(), r0, cr0); \
  } while (0)
#else
#define ASSEMBLE_ADD_WITH_OVERFLOW()                                    \
  do {                                                                  \
    if (HasRegisterInput(instr, 1)) {                                   \
      __ AddAndCheckForOverflow(i.OutputRegister(), i.InputRegister(0), \
                                i.InputRegister(1), kScratchReg, r0);   \
    } else {                                                            \
      __ AddAndCheckForOverflow(i.OutputRegister(), i.InputRegister(0), \
                                i.InputInt32(1), kScratchReg, r0);      \
    }                                                                   \
  } while (0)
#endif


#if V8_TARGET_ARCH_PPC64
#define ASSEMBLE_SUB_WITH_OVERFLOW()             \
  do {                                           \
    ASSEMBLE_BINOP(sub, subi);                   \
    __ TestIfInt32(i.OutputRegister(), r0, cr0); \
  } while (0)
#else
#define ASSEMBLE_SUB_WITH_OVERFLOW()                                    \
  do {                                                                  \
    if (HasRegisterInput(instr, 1)) {                                   \
      __ SubAndCheckForOverflow(i.OutputRegister(), i.InputRegister(0), \
                                i.InputRegister(1), kScratchReg, r0);   \
    } else {                                                            \
      __ AddAndCheckForOverflow(i.OutputRegister(), i.InputRegister(0), \
                                -i.InputInt32(1), kScratchReg, r0);     \
    }                                                                   \
  } while (0)
#endif


#define ASSEMBLE_COMPARE(cmp_instr, cmpl_instr)                        \
  do {                                                                 \
    const CRegister cr = cr0;                                          \
    if (HasRegisterInput(instr, 1)) {                                  \
      if (i.CompareLogical()) {                                        \
        __ cmpl_instr(i.InputRegister(0), i.InputRegister(1), cr);     \
      } else {                                                         \
        __ cmp_instr(i.InputRegister(0), i.InputRegister(1), cr);      \
      }                                                                \
    } else {                                                           \
      if (i.CompareLogical()) {                                        \
        __ cmpl_instr##i(i.InputRegister(0), i.InputImmediate(1), cr); \
      } else {                                                         \
        __ cmp_instr##i(i.InputRegister(0), i.InputImmediate(1), cr);  \
      }                                                                \
    }                                                                  \
    DCHECK_EQ(SetRC, i.OutputRCBit());                                 \
  } while (0)


#define ASSEMBLE_FLOAT_COMPARE(cmp_instr)                                 \
  do {                                                                    \
    const CRegister cr = cr0;                                             \
    __ cmp_instr(i.InputDoubleRegister(0), i.InputDoubleRegister(1), cr); \
    DCHECK_EQ(SetRC, i.OutputRCBit());                                    \
  } while (0)


#define ASSEMBLE_MODULO(div_instr, mul_instr)                        \
  do {                                                               \
    const Register scratch = kScratchReg;                            \
    __ div_instr(scratch, i.InputRegister(0), i.InputRegister(1));   \
    __ mul_instr(scratch, scratch, i.InputRegister(1));              \
    __ sub(i.OutputRegister(), i.InputRegister(0), scratch, LeaveOE, \
           i.OutputRCBit());                                         \
  } while (0)


#define ASSEMBLE_FLOAT_MODULO()                                               \
  do {                                                                        \
    FrameScope scope(masm(), StackFrame::MANUAL);                             \
    __ PrepareCallCFunction(0, 2, kScratchReg);                               \
    __ MovToFloatParameters(i.InputDoubleRegister(0),                         \
                            i.InputDoubleRegister(1));                        \
    __ CallCFunction(ExternalReference::mod_two_doubles_operation(isolate()), \
                     0, 2);                                                   \
    __ MovFromFloatResult(i.OutputDoubleRegister());                          \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                                      \
  } while (0)


#define ASSEMBLE_FLOAT_MAX(scratch_reg)                                       \
  do {                                                                        \
    __ fsub(scratch_reg, i.InputDoubleRegister(0), i.InputDoubleRegister(1)); \
    __ fsel(i.OutputDoubleRegister(), scratch_reg, i.InputDoubleRegister(0),  \
            i.InputDoubleRegister(1));                                        \
  } while (0)


#define ASSEMBLE_FLOAT_MIN(scratch_reg)                                       \
  do {                                                                        \
    __ fsub(scratch_reg, i.InputDoubleRegister(0), i.InputDoubleRegister(1)); \
    __ fsel(i.OutputDoubleRegister(), scratch_reg, i.InputDoubleRegister(1),  \
            i.InputDoubleRegister(0));                                        \
  } while (0)


#define ASSEMBLE_LOAD_FLOAT(asm_instr, asm_instrx)    \
  do {                                                \
    DoubleRegister result = i.OutputDoubleRegister(); \
    AddressingMode mode = kMode_None;                 \
    MemOperand operand = i.MemoryOperand(&mode);      \
    if (mode == kMode_MRI) {                          \
      __ asm_instr(result, operand);                  \
    } else {                                          \
      __ asm_instrx(result, operand);                 \
    }                                                 \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());              \
  } while (0)


#define ASSEMBLE_LOAD_INTEGER(asm_instr, asm_instrx) \
  do {                                               \
    Register result = i.OutputRegister();            \
    AddressingMode mode = kMode_None;                \
    MemOperand operand = i.MemoryOperand(&mode);     \
    if (mode == kMode_MRI) {                         \
      __ asm_instr(result, operand);                 \
    } else {                                         \
      __ asm_instrx(result, operand);                \
    }                                                \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());             \
  } while (0)


#define ASSEMBLE_STORE_FLOAT32()                         \
  do {                                                   \
    size_t index = 0;                                    \
    AddressingMode mode = kMode_None;                    \
    MemOperand operand = i.MemoryOperand(&mode, &index); \
    DoubleRegister value = i.InputDoubleRegister(index); \
    __ frsp(kScratchDoubleReg, value);                   \
    if (mode == kMode_MRI) {                             \
      __ stfs(kScratchDoubleReg, operand);               \
    } else {                                             \
      __ stfsx(kScratchDoubleReg, operand);              \
    }                                                    \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                 \
  } while (0)


#define ASSEMBLE_STORE_DOUBLE()                          \
  do {                                                   \
    size_t index = 0;                                    \
    AddressingMode mode = kMode_None;                    \
    MemOperand operand = i.MemoryOperand(&mode, &index); \
    DoubleRegister value = i.InputDoubleRegister(index); \
    if (mode == kMode_MRI) {                             \
      __ stfd(value, operand);                           \
    } else {                                             \
      __ stfdx(value, operand);                          \
    }                                                    \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                 \
  } while (0)


#define ASSEMBLE_STORE_INTEGER(asm_instr, asm_instrx)    \
  do {                                                   \
    size_t index = 0;                                    \
    AddressingMode mode = kMode_None;                    \
    MemOperand operand = i.MemoryOperand(&mode, &index); \
    Register value = i.InputRegister(index);             \
    if (mode == kMode_MRI) {                             \
      __ asm_instr(value, operand);                      \
    } else {                                             \
      __ asm_instrx(value, operand);                     \
    }                                                    \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                 \
  } while (0)


// TODO(mbrandy): fix paths that produce garbage in offset's upper 32-bits.
#define ASSEMBLE_CHECKED_LOAD_FLOAT(asm_instr, asm_instrx, width)  \
  do {                                                             \
    DoubleRegister result = i.OutputDoubleRegister();              \
    size_t index = 0;                                              \
    AddressingMode mode = kMode_None;                              \
    MemOperand operand = i.MemoryOperand(&mode, index);            \
    DCHECK_EQ(kMode_MRR, mode);                                    \
    Register offset = operand.rb();                                \
    __ extsw(offset, offset);                                      \
    if (HasRegisterInput(instr, 2)) {                              \
      __ cmplw(offset, i.InputRegister(2));                        \
    } else {                                                       \
      __ cmplwi(offset, i.InputImmediate(2));                      \
    }                                                              \
    auto ool = new (zone()) OutOfLineLoadNAN##width(this, result); \
    __ bge(ool->entry());                                          \
    if (mode == kMode_MRI) {                                       \
      __ asm_instr(result, operand);                               \
    } else {                                                       \
      __ asm_instrx(result, operand);                              \
    }                                                              \
    __ bind(ool->exit());                                          \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                           \
  } while (0)


// TODO(mbrandy): fix paths that produce garbage in offset's upper 32-bits.
#define ASSEMBLE_CHECKED_LOAD_INTEGER(asm_instr, asm_instrx) \
  do {                                                       \
    Register result = i.OutputRegister();                    \
    size_t index = 0;                                        \
    AddressingMode mode = kMode_None;                        \
    MemOperand operand = i.MemoryOperand(&mode, index);      \
    DCHECK_EQ(kMode_MRR, mode);                              \
    Register offset = operand.rb();                          \
    __ extsw(offset, offset);                                \
    if (HasRegisterInput(instr, 2)) {                        \
      __ cmplw(offset, i.InputRegister(2));                  \
    } else {                                                 \
      __ cmplwi(offset, i.InputImmediate(2));                \
    }                                                        \
    auto ool = new (zone()) OutOfLineLoadZero(this, result); \
    __ bge(ool->entry());                                    \
    if (mode == kMode_MRI) {                                 \
      __ asm_instr(result, operand);                         \
    } else {                                                 \
      __ asm_instrx(result, operand);                        \
    }                                                        \
    __ bind(ool->exit());                                    \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                     \
  } while (0)


// TODO(mbrandy): fix paths that produce garbage in offset's upper 32-bits.
#define ASSEMBLE_CHECKED_STORE_FLOAT32()                \
  do {                                                  \
    Label done;                                         \
    size_t index = 0;                                   \
    AddressingMode mode = kMode_None;                   \
    MemOperand operand = i.MemoryOperand(&mode, index); \
    DCHECK_EQ(kMode_MRR, mode);                         \
    Register offset = operand.rb();                     \
    __ extsw(offset, offset);                           \
    if (HasRegisterInput(instr, 2)) {                   \
      __ cmplw(offset, i.InputRegister(2));             \
    } else {                                            \
      __ cmplwi(offset, i.InputImmediate(2));           \
    }                                                   \
    __ bge(&done);                                      \
    DoubleRegister value = i.InputDoubleRegister(3);    \
    __ frsp(kScratchDoubleReg, value);                  \
    if (mode == kMode_MRI) {                            \
      __ stfs(kScratchDoubleReg, operand);              \
    } else {                                            \
      __ stfsx(kScratchDoubleReg, operand);             \
    }                                                   \
    __ bind(&done);                                     \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                \
  } while (0)


// TODO(mbrandy): fix paths that produce garbage in offset's upper 32-bits.
#define ASSEMBLE_CHECKED_STORE_DOUBLE()                 \
  do {                                                  \
    Label done;                                         \
    size_t index = 0;                                   \
    AddressingMode mode = kMode_None;                   \
    MemOperand operand = i.MemoryOperand(&mode, index); \
    DCHECK_EQ(kMode_MRR, mode);                         \
    Register offset = operand.rb();                     \
    __ extsw(offset, offset);                           \
    if (HasRegisterInput(instr, 2)) {                   \
      __ cmplw(offset, i.InputRegister(2));             \
    } else {                                            \
      __ cmplwi(offset, i.InputImmediate(2));           \
    }                                                   \
    __ bge(&done);                                      \
    DoubleRegister value = i.InputDoubleRegister(3);    \
    if (mode == kMode_MRI) {                            \
      __ stfd(value, operand);                          \
    } else {                                            \
      __ stfdx(value, operand);                         \
    }                                                   \
    __ bind(&done);                                     \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                \
  } while (0)


// TODO(mbrandy): fix paths that produce garbage in offset's upper 32-bits.
#define ASSEMBLE_CHECKED_STORE_INTEGER(asm_instr, asm_instrx) \
  do {                                                        \
    Label done;                                               \
    size_t index = 0;                                         \
    AddressingMode mode = kMode_None;                         \
    MemOperand operand = i.MemoryOperand(&mode, index);       \
    DCHECK_EQ(kMode_MRR, mode);                               \
    Register offset = operand.rb();                           \
    __ extsw(offset, offset);                                 \
    if (HasRegisterInput(instr, 2)) {                         \
      __ cmplw(offset, i.InputRegister(2));                   \
    } else {                                                  \
      __ cmplwi(offset, i.InputImmediate(2));                 \
    }                                                         \
    __ bge(&done);                                            \
    Register value = i.InputRegister(3);                      \
    if (mode == kMode_MRI) {                                  \
      __ asm_instr(value, operand);                           \
    } else {                                                  \
      __ asm_instrx(value, operand);                          \
    }                                                         \
    __ bind(&done);                                           \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                      \
  } while (0)


#define ASSEMBLE_STORE_WRITE_BARRIER()                                         \
  do {                                                                         \
    Register object = i.InputRegister(0);                                      \
    Register index = i.InputRegister(1);                                       \
    Register value = i.InputRegister(2);                                       \
    __ add(index, object, index);                                              \
    __ StoreP(value, MemOperand(index));                                       \
    SaveFPRegsMode mode =                                                      \
        frame()->DidAllocateDoubleRegisters() ? kSaveFPRegs : kDontSaveFPRegs; \
    LinkRegisterStatus lr_status = kLRHasNotBeenSaved;                         \
    __ RecordWrite(object, index, value, lr_status, mode);                     \
    DCHECK_EQ(LeaveRC, i.OutputRCBit());                                       \
  } while (0)


void CodeGenerator::AssembleDeconstructActivationRecord() {
  CallDescriptor* descriptor = linkage()->GetIncomingDescriptor();
  int stack_slots = frame()->GetSpillSlotCount();
  if (descriptor->IsJSFunctionCall() || stack_slots > 0) {
    __ LeaveFrame(StackFrame::MANUAL);
  }
}


// Assembles an instruction after register allocation, producing machine code.
void CodeGenerator::AssembleArchInstruction(Instruction* instr) {
  PPCOperandConverter i(this, instr);
  ArchOpcode opcode = ArchOpcodeField::decode(instr->opcode());

  switch (opcode) {
    case kArchCallCodeObject: {
      EnsureSpaceForLazyDeopt();
      if (HasRegisterInput(instr, 0)) {
        __ addi(ip, i.InputRegister(0),
                Operand(Code::kHeaderSize - kHeapObjectTag));
        __ Call(ip);
      } else {
        __ Call(Handle<Code>::cast(i.InputHeapObject(0)),
                RelocInfo::CODE_TARGET);
      }
      RecordCallPosition(instr);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    }
    case kArchTailCallCodeObject: {
      AssembleDeconstructActivationRecord();
      if (HasRegisterInput(instr, 0)) {
        __ addi(ip, i.InputRegister(0),
                Operand(Code::kHeaderSize - kHeapObjectTag));
        __ Jump(ip);
      } else {
        // We cannot use the constant pool to load the target since
        // we've already restored the caller's frame.
        ConstantPoolUnavailableScope constant_pool_unavailable(masm());
        __ Jump(Handle<Code>::cast(i.InputHeapObject(0)),
                RelocInfo::CODE_TARGET);
      }
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    }
    case kArchCallJSFunction: {
      EnsureSpaceForLazyDeopt();
      Register func = i.InputRegister(0);
      if (FLAG_debug_code) {
        // Check the function's context matches the context argument.
        __ LoadP(kScratchReg,
                 FieldMemOperand(func, JSFunction::kContextOffset));
        __ cmp(cp, kScratchReg);
        __ Assert(eq, kWrongFunctionContext);
      }
      __ LoadP(ip, FieldMemOperand(func, JSFunction::kCodeEntryOffset));
      __ Call(ip);
      RecordCallPosition(instr);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    }
    case kArchTailCallJSFunction: {
      Register func = i.InputRegister(0);
      if (FLAG_debug_code) {
        // Check the function's context matches the context argument.
        __ LoadP(kScratchReg,
                 FieldMemOperand(func, JSFunction::kContextOffset));
        __ cmp(cp, kScratchReg);
        __ Assert(eq, kWrongFunctionContext);
      }
      AssembleDeconstructActivationRecord();
      __ LoadP(ip, FieldMemOperand(func, JSFunction::kCodeEntryOffset));
      __ Jump(ip);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    }
    case kArchPrepareCallCFunction: {
      int const num_parameters = MiscField::decode(instr->opcode());
      __ PrepareCallCFunction(num_parameters, kScratchReg);
      break;
    }
    case kArchCallCFunction: {
      int const num_parameters = MiscField::decode(instr->opcode());
      if (instr->InputAt(0)->IsImmediate()) {
        ExternalReference ref = i.InputExternalReference(0);
        __ CallCFunction(ref, num_parameters);
      } else {
        Register func = i.InputRegister(0);
        __ CallCFunction(func, num_parameters);
      }
      break;
    }
    case kArchJmp:
      AssembleArchJump(i.InputRpo(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kArchLookupSwitch:
      AssembleArchLookupSwitch(instr);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kArchTableSwitch:
      AssembleArchTableSwitch(instr);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kArchNop:
      // don't emit code for nops.
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kArchDeoptimize: {
      int deopt_state_id =
          BuildTranslation(instr, -1, 0, OutputFrameStateCombine::Ignore());
      AssembleDeoptimizerCall(deopt_state_id, Deoptimizer::EAGER);
      break;
    }
    case kArchRet:
      AssembleReturn();
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kArchStackPointer:
      __ mr(i.OutputRegister(), sp);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kArchFramePointer:
      __ mr(i.OutputRegister(), fp);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kArchTruncateDoubleToI:
      // TODO(mbrandy): move slow call to stub out of line.
      __ TruncateDoubleToI(i.OutputRegister(), i.InputDoubleRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_And:
      if (HasRegisterInput(instr, 1)) {
        __ and_(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
                i.OutputRCBit());
      } else {
        __ andi(i.OutputRegister(), i.InputRegister(0), i.InputImmediate(1));
      }
      break;
    case kPPC_AndComplement:
      __ andc(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
              i.OutputRCBit());
      break;
    case kPPC_Or:
      if (HasRegisterInput(instr, 1)) {
        __ orx(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               i.OutputRCBit());
      } else {
        __ ori(i.OutputRegister(), i.InputRegister(0), i.InputImmediate(1));
        DCHECK_EQ(LeaveRC, i.OutputRCBit());
      }
      break;
    case kPPC_OrComplement:
      __ orc(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
             i.OutputRCBit());
      break;
    case kPPC_Xor:
      if (HasRegisterInput(instr, 1)) {
        __ xor_(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
                i.OutputRCBit());
      } else {
        __ xori(i.OutputRegister(), i.InputRegister(0), i.InputImmediate(1));
        DCHECK_EQ(LeaveRC, i.OutputRCBit());
      }
      break;
    case kPPC_ShiftLeft32:
      ASSEMBLE_BINOP_RC(slw, slwi);
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_ShiftLeft64:
      ASSEMBLE_BINOP_RC(sld, sldi);
      break;
#endif
    case kPPC_ShiftRight32:
      ASSEMBLE_BINOP_RC(srw, srwi);
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_ShiftRight64:
      ASSEMBLE_BINOP_RC(srd, srdi);
      break;
#endif
    case kPPC_ShiftRightAlg32:
      ASSEMBLE_BINOP_INT_RC(sraw, srawi);
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_ShiftRightAlg64:
      ASSEMBLE_BINOP_INT_RC(srad, sradi);
      break;
#endif
    case kPPC_RotRight32:
      if (HasRegisterInput(instr, 1)) {
        __ subfic(kScratchReg, i.InputRegister(1), Operand(32));
        __ rotlw(i.OutputRegister(), i.InputRegister(0), kScratchReg,
                 i.OutputRCBit());
      } else {
        int sh = i.InputInt32(1);
        __ rotrwi(i.OutputRegister(), i.InputRegister(0), sh, i.OutputRCBit());
      }
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_RotRight64:
      if (HasRegisterInput(instr, 1)) {
        __ subfic(kScratchReg, i.InputRegister(1), Operand(64));
        __ rotld(i.OutputRegister(), i.InputRegister(0), kScratchReg,
                 i.OutputRCBit());
      } else {
        int sh = i.InputInt32(1);
        __ rotrdi(i.OutputRegister(), i.InputRegister(0), sh, i.OutputRCBit());
      }
      break;
#endif
    case kPPC_Not:
      __ notx(i.OutputRegister(), i.InputRegister(0), i.OutputRCBit());
      break;
    case kPPC_RotLeftAndMask32:
      __ rlwinm(i.OutputRegister(), i.InputRegister(0), i.InputInt32(1),
                31 - i.InputInt32(2), 31 - i.InputInt32(3), i.OutputRCBit());
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_RotLeftAndClear64:
      __ rldic(i.OutputRegister(), i.InputRegister(0), i.InputInt32(1),
               63 - i.InputInt32(2), i.OutputRCBit());
      break;
    case kPPC_RotLeftAndClearLeft64:
      __ rldicl(i.OutputRegister(), i.InputRegister(0), i.InputInt32(1),
                63 - i.InputInt32(2), i.OutputRCBit());
      break;
    case kPPC_RotLeftAndClearRight64:
      __ rldicr(i.OutputRegister(), i.InputRegister(0), i.InputInt32(1),
                63 - i.InputInt32(2), i.OutputRCBit());
      break;
#endif
    case kPPC_Add:
      if (HasRegisterInput(instr, 1)) {
        __ add(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               LeaveOE, i.OutputRCBit());
      } else {
        __ addi(i.OutputRegister(), i.InputRegister(0), i.InputImmediate(1));
        DCHECK_EQ(LeaveRC, i.OutputRCBit());
      }
      break;
    case kPPC_AddWithOverflow32:
      ASSEMBLE_ADD_WITH_OVERFLOW();
      break;
    case kPPC_AddDouble:
      ASSEMBLE_FLOAT_BINOP_RC(fadd);
      break;
    case kPPC_Sub:
      if (HasRegisterInput(instr, 1)) {
        __ sub(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               LeaveOE, i.OutputRCBit());
      } else {
        __ subi(i.OutputRegister(), i.InputRegister(0), i.InputImmediate(1));
        DCHECK_EQ(LeaveRC, i.OutputRCBit());
      }
      break;
    case kPPC_SubWithOverflow32:
      ASSEMBLE_SUB_WITH_OVERFLOW();
      break;
    case kPPC_SubDouble:
      ASSEMBLE_FLOAT_BINOP_RC(fsub);
      break;
    case kPPC_Mul32:
      __ mullw(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               LeaveOE, i.OutputRCBit());
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_Mul64:
      __ mulld(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               LeaveOE, i.OutputRCBit());
      break;
#endif
    case kPPC_MulHigh32:
      __ mulhw(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
               i.OutputRCBit());
      break;
    case kPPC_MulHighU32:
      __ mulhwu(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1),
                i.OutputRCBit());
      break;
    case kPPC_MulDouble:
      ASSEMBLE_FLOAT_BINOP_RC(fmul);
      break;
    case kPPC_Div32:
      __ divw(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_Div64:
      __ divd(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
#endif
    case kPPC_DivU32:
      __ divwu(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_DivU64:
      __ divdu(i.OutputRegister(), i.InputRegister(0), i.InputRegister(1));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
#endif
    case kPPC_DivDouble:
      ASSEMBLE_FLOAT_BINOP_RC(fdiv);
      break;
    case kPPC_Mod32:
      ASSEMBLE_MODULO(divw, mullw);
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_Mod64:
      ASSEMBLE_MODULO(divd, mulld);
      break;
#endif
    case kPPC_ModU32:
      ASSEMBLE_MODULO(divwu, mullw);
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_ModU64:
      ASSEMBLE_MODULO(divdu, mulld);
      break;
#endif
    case kPPC_ModDouble:
      // TODO(bmeurer): We should really get rid of this special instruction,
      // and generate a CallAddress instruction instead.
      ASSEMBLE_FLOAT_MODULO();
      break;
    case kPPC_Neg:
      __ neg(i.OutputRegister(), i.InputRegister(0), LeaveOE, i.OutputRCBit());
      break;
    case kPPC_MaxDouble:
      ASSEMBLE_FLOAT_MAX(kScratchDoubleReg);
      break;
    case kPPC_MinDouble:
      ASSEMBLE_FLOAT_MIN(kScratchDoubleReg);
      break;
    case kPPC_AbsDouble:
      ASSEMBLE_FLOAT_UNOP_RC(fabs);
      break;
    case kPPC_SqrtDouble:
      ASSEMBLE_FLOAT_UNOP_RC(fsqrt);
      break;
    case kPPC_FloorDouble:
      ASSEMBLE_FLOAT_UNOP_RC(frim);
      break;
    case kPPC_CeilDouble:
      ASSEMBLE_FLOAT_UNOP_RC(frip);
      break;
    case kPPC_TruncateDouble:
      ASSEMBLE_FLOAT_UNOP_RC(friz);
      break;
    case kPPC_RoundDouble:
      ASSEMBLE_FLOAT_UNOP_RC(frin);
      break;
    case kPPC_NegDouble:
      ASSEMBLE_FLOAT_UNOP_RC(fneg);
      break;
    case kPPC_Cntlz32:
      __ cntlzw_(i.OutputRegister(), i.InputRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_Cmp32:
      ASSEMBLE_COMPARE(cmpw, cmplw);
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_Cmp64:
      ASSEMBLE_COMPARE(cmp, cmpl);
      break;
#endif
    case kPPC_CmpDouble:
      ASSEMBLE_FLOAT_COMPARE(fcmpu);
      break;
    case kPPC_Tst32:
      if (HasRegisterInput(instr, 1)) {
        __ and_(r0, i.InputRegister(0), i.InputRegister(1), i.OutputRCBit());
      } else {
        __ andi(r0, i.InputRegister(0), i.InputImmediate(1));
      }
#if V8_TARGET_ARCH_PPC64
      __ extsw(r0, r0, i.OutputRCBit());
#endif
      DCHECK_EQ(SetRC, i.OutputRCBit());
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_Tst64:
      if (HasRegisterInput(instr, 1)) {
        __ and_(r0, i.InputRegister(0), i.InputRegister(1), i.OutputRCBit());
      } else {
        __ andi(r0, i.InputRegister(0), i.InputImmediate(1));
      }
      DCHECK_EQ(SetRC, i.OutputRCBit());
      break;
#endif
    case kPPC_Push:
      __ Push(i.InputRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_PushFrame: {
      int num_slots = i.InputInt32(1);
      __ StorePU(i.InputRegister(0), MemOperand(sp, -num_slots * kPointerSize));
      break;
    }
    case kPPC_StoreToStackSlot: {
      int slot = i.InputInt32(1);
      __ StoreP(i.InputRegister(0), MemOperand(sp, slot * kPointerSize));
      break;
    }
    case kPPC_ExtendSignWord8:
      __ extsb(i.OutputRegister(), i.InputRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_ExtendSignWord16:
      __ extsh(i.OutputRegister(), i.InputRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_ExtendSignWord32:
      __ extsw(i.OutputRegister(), i.InputRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_Uint32ToUint64:
      // Zero extend
      __ clrldi(i.OutputRegister(), i.InputRegister(0), Operand(32));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_Int64ToInt32:
      // TODO(mbrandy): sign extend?
      __ Move(i.OutputRegister(), i.InputRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
#endif
    case kPPC_Int32ToDouble:
      __ ConvertIntToDouble(i.InputRegister(0), i.OutputDoubleRegister());
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_Uint32ToDouble:
      __ ConvertUnsignedIntToDouble(i.InputRegister(0),
                                    i.OutputDoubleRegister());
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_DoubleToInt32:
    case kPPC_DoubleToUint32:
      __ ConvertDoubleToInt64(i.InputDoubleRegister(0),
#if !V8_TARGET_ARCH_PPC64
                              kScratchReg,
#endif
                              i.OutputRegister(), kScratchDoubleReg);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_DoubleToFloat32:
      ASSEMBLE_FLOAT_UNOP_RC(frsp);
      break;
    case kPPC_Float32ToDouble:
      // Nothing to do.
      __ Move(i.OutputDoubleRegister(), i.InputDoubleRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_DoubleExtractLowWord32:
      __ MovDoubleLowToInt(i.OutputRegister(), i.InputDoubleRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_DoubleExtractHighWord32:
      __ MovDoubleHighToInt(i.OutputRegister(), i.InputDoubleRegister(0));
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_DoubleInsertLowWord32:
      __ InsertDoubleLow(i.OutputDoubleRegister(), i.InputRegister(1), r0);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_DoubleInsertHighWord32:
      __ InsertDoubleHigh(i.OutputDoubleRegister(), i.InputRegister(1), r0);
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_DoubleConstruct:
#if V8_TARGET_ARCH_PPC64
      __ MovInt64ComponentsToDouble(i.OutputDoubleRegister(),
                                    i.InputRegister(0), i.InputRegister(1), r0);
#else
      __ MovInt64ToDouble(i.OutputDoubleRegister(), i.InputRegister(0),
                          i.InputRegister(1));
#endif
      DCHECK_EQ(LeaveRC, i.OutputRCBit());
      break;
    case kPPC_LoadWordU8:
      ASSEMBLE_LOAD_INTEGER(lbz, lbzx);
      break;
    case kPPC_LoadWordS8:
      ASSEMBLE_LOAD_INTEGER(lbz, lbzx);
      __ extsb(i.OutputRegister(), i.OutputRegister());
      break;
    case kPPC_LoadWordU16:
      ASSEMBLE_LOAD_INTEGER(lhz, lhzx);
      break;
    case kPPC_LoadWordS16:
      ASSEMBLE_LOAD_INTEGER(lha, lhax);
      break;
    case kPPC_LoadWordS32:
      ASSEMBLE_LOAD_INTEGER(lwa, lwax);
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_LoadWord64:
      ASSEMBLE_LOAD_INTEGER(ld, ldx);
      break;
#endif
    case kPPC_LoadFloat32:
      ASSEMBLE_LOAD_FLOAT(lfs, lfsx);
      break;
    case kPPC_LoadDouble:
      ASSEMBLE_LOAD_FLOAT(lfd, lfdx);
      break;
    case kPPC_StoreWord8:
      ASSEMBLE_STORE_INTEGER(stb, stbx);
      break;
    case kPPC_StoreWord16:
      ASSEMBLE_STORE_INTEGER(sth, sthx);
      break;
    case kPPC_StoreWord32:
      ASSEMBLE_STORE_INTEGER(stw, stwx);
      break;
#if V8_TARGET_ARCH_PPC64
    case kPPC_StoreWord64:
      ASSEMBLE_STORE_INTEGER(std, stdx);
      break;
#endif
    case kPPC_StoreFloat32:
      ASSEMBLE_STORE_FLOAT32();
      break;
    case kPPC_StoreDouble:
      ASSEMBLE_STORE_DOUBLE();
      break;
    case kPPC_StoreWriteBarrier:
      ASSEMBLE_STORE_WRITE_BARRIER();
      break;
    case kCheckedLoadInt8:
      ASSEMBLE_CHECKED_LOAD_INTEGER(lbz, lbzx);
      __ extsb(i.OutputRegister(), i.OutputRegister());
      break;
    case kCheckedLoadUint8:
      ASSEMBLE_CHECKED_LOAD_INTEGER(lbz, lbzx);
      break;
    case kCheckedLoadInt16:
      ASSEMBLE_CHECKED_LOAD_INTEGER(lha, lhax);
      break;
    case kCheckedLoadUint16:
      ASSEMBLE_CHECKED_LOAD_INTEGER(lhz, lhzx);
      break;
    case kCheckedLoadWord32:
      ASSEMBLE_CHECKED_LOAD_INTEGER(lwa, lwax);
      break;
    case kCheckedLoadFloat32:
      ASSEMBLE_CHECKED_LOAD_FLOAT(lfs, lfsx, 32);
      break;
    case kCheckedLoadFloat64:
      ASSEMBLE_CHECKED_LOAD_FLOAT(lfd, lfdx, 64);
      break;
    case kCheckedStoreWord8:
      ASSEMBLE_CHECKED_STORE_INTEGER(stb, stbx);
      break;
    case kCheckedStoreWord16:
      ASSEMBLE_CHECKED_STORE_INTEGER(sth, sthx);
      break;
    case kCheckedStoreWord32:
      ASSEMBLE_CHECKED_STORE_INTEGER(stw, stwx);
      break;
    case kCheckedStoreFloat32:
      ASSEMBLE_CHECKED_STORE_FLOAT32();
      break;
    case kCheckedStoreFloat64:
      ASSEMBLE_CHECKED_STORE_DOUBLE();
      break;
    default:
      UNREACHABLE();
      break;
  }
}  // NOLINT(readability/fn_size)


// Assembles branches after an instruction.
void CodeGenerator::AssembleArchBranch(Instruction* instr, BranchInfo* branch) {
  PPCOperandConverter i(this, instr);
  Label* tlabel = branch->true_label;
  Label* flabel = branch->false_label;
  ArchOpcode op = instr->arch_opcode();
  FlagsCondition condition = branch->condition;
  CRegister cr = cr0;

  // Overflow checked for add/sub only.
  DCHECK((condition != kOverflow && condition != kNotOverflow) ||
         (op == kPPC_AddWithOverflow32 || op == kPPC_SubWithOverflow32));

  Condition cond = FlagsConditionToCondition(condition);
  if (op == kPPC_CmpDouble) {
    // check for unordered if necessary
    if (cond == le) {
      __ bunordered(flabel, cr);
      // Unnecessary for eq/lt since only FU bit will be set.
    } else if (cond == gt) {
      __ bunordered(tlabel, cr);
      // Unnecessary for ne/ge since only FU bit will be set.
    }
  }
  __ b(cond, tlabel, cr);
  if (!branch->fallthru) __ b(flabel);  // no fallthru to flabel.
}


void CodeGenerator::AssembleArchJump(RpoNumber target) {
  if (!IsNextInAssemblyOrder(target)) __ b(GetLabel(target));
}


// Assembles boolean materializations after an instruction.
void CodeGenerator::AssembleArchBoolean(Instruction* instr,
                                        FlagsCondition condition) {
  PPCOperandConverter i(this, instr);
  Label done;
  ArchOpcode op = instr->arch_opcode();
  bool check_unordered = (op == kPPC_CmpDouble);
  CRegister cr = cr0;

  // Overflow checked for add/sub only.
  DCHECK((condition != kOverflow && condition != kNotOverflow) ||
         (op == kPPC_AddWithOverflow32 || op == kPPC_SubWithOverflow32));

  // Materialize a full 32-bit 1 or 0 value. The result register is always the
  // last output of the instruction.
  DCHECK_NE(0u, instr->OutputCount());
  Register reg = i.OutputRegister(instr->OutputCount() - 1);

  Condition cond = FlagsConditionToCondition(condition);
  switch (cond) {
    case eq:
    case lt:
      __ li(reg, Operand::Zero());
      __ li(kScratchReg, Operand(1));
      __ isel(cond, reg, kScratchReg, reg, cr);
      break;
    case ne:
    case ge:
      __ li(reg, Operand(1));
      __ isel(NegateCondition(cond), reg, r0, reg, cr);
      break;
    case gt:
      if (check_unordered) {
        __ li(reg, Operand(1));
        __ li(kScratchReg, Operand::Zero());
        __ bunordered(&done, cr);
        __ isel(cond, reg, reg, kScratchReg, cr);
      } else {
        __ li(reg, Operand::Zero());
        __ li(kScratchReg, Operand(1));
        __ isel(cond, reg, kScratchReg, reg, cr);
      }
      break;
    case le:
      if (check_unordered) {
        __ li(reg, Operand::Zero());
        __ li(kScratchReg, Operand(1));
        __ bunordered(&done, cr);
        __ isel(NegateCondition(cond), reg, r0, kScratchReg, cr);
      } else {
        __ li(reg, Operand(1));
        __ isel(NegateCondition(cond), reg, r0, reg, cr);
      }
      break;
    default:
      UNREACHABLE();
      break;
  }
  __ bind(&done);
}


void CodeGenerator::AssembleArchLookupSwitch(Instruction* instr) {
  PPCOperandConverter i(this, instr);
  Register input = i.InputRegister(0);
  for (size_t index = 2; index < instr->InputCount(); index += 2) {
    __ Cmpi(input, Operand(i.InputInt32(index + 0)), r0);
    __ beq(GetLabel(i.InputRpo(index + 1)));
  }
  AssembleArchJump(i.InputRpo(1));
}


void CodeGenerator::AssembleArchTableSwitch(Instruction* instr) {
  PPCOperandConverter i(this, instr);
  Register input = i.InputRegister(0);
  int32_t const case_count = static_cast<int32_t>(instr->InputCount() - 2);
  Label** cases = zone()->NewArray<Label*>(case_count);
  for (int32_t index = 0; index < case_count; ++index) {
    cases[index] = GetLabel(i.InputRpo(index + 2));
  }
  Label* const table = AddJumpTable(cases, case_count);
  __ Cmpli(input, Operand(case_count), r0);
  __ bge(GetLabel(i.InputRpo(1)));
  __ mov_label_addr(kScratchReg, table);
  __ ShiftLeftImm(r0, input, Operand(kPointerSizeLog2));
  __ LoadPX(kScratchReg, MemOperand(kScratchReg, r0));
  __ Jump(kScratchReg);
}


void CodeGenerator::AssembleDeoptimizerCall(
    int deoptimization_id, Deoptimizer::BailoutType bailout_type) {
  Address deopt_entry = Deoptimizer::GetDeoptimizationEntry(
      isolate(), deoptimization_id, bailout_type);
  __ Call(deopt_entry, RelocInfo::RUNTIME_ENTRY);
}


void CodeGenerator::AssemblePrologue() {
  CallDescriptor* descriptor = linkage()->GetIncomingDescriptor();
  int stack_slots = frame()->GetSpillSlotCount();
  if (descriptor->kind() == CallDescriptor::kCallAddress) {
    __ function_descriptor();
    int register_save_area_size = 0;
    RegList frame_saves = fp.bit();
    __ mflr(r0);
    if (FLAG_enable_embedded_constant_pool) {
      __ Push(r0, fp, kConstantPoolRegister);
      // Adjust FP to point to saved FP.
      __ subi(fp, sp, Operand(StandardFrameConstants::kConstantPoolOffset));
      register_save_area_size += kPointerSize;
      frame_saves |= kConstantPoolRegister.bit();
    } else {
      __ Push(r0, fp);
      __ mr(fp, sp);
    }
    // Save callee-saved registers.
    const RegList saves = descriptor->CalleeSavedRegisters() & ~frame_saves;
    for (int i = Register::kNumRegisters - 1; i >= 0; i--) {
      if (!((1 << i) & saves)) continue;
      register_save_area_size += kPointerSize;
    }
    frame()->SetRegisterSaveAreaSize(register_save_area_size);
    __ MultiPush(saves);
  } else if (descriptor->IsJSFunctionCall()) {
    CompilationInfo* info = this->info();
    __ Prologue(info->IsCodePreAgingActive());
    frame()->SetRegisterSaveAreaSize(
        StandardFrameConstants::kFixedFrameSizeFromFp);
  } else if (needs_frame_) {
    __ StubPrologue();
    frame()->SetRegisterSaveAreaSize(
        StandardFrameConstants::kFixedFrameSizeFromFp);
  }

  if (info()->is_osr()) {
    // TurboFan OSR-compiled functions cannot be entered directly.
    __ Abort(kShouldNotDirectlyEnterOsrFunction);

    // Unoptimized code jumps directly to this entrypoint while the unoptimized
    // frame is still on the stack. Optimized code uses OSR values directly from
    // the unoptimized frame. Thus, all that needs to be done is to allocate the
    // remaining stack slots.
    if (FLAG_code_comments) __ RecordComment("-- OSR entrypoint --");
    osr_pc_offset_ = __ pc_offset();
    // TODO(titzer): cannot address target function == local #-1
    __ LoadP(r4, MemOperand(fp, JavaScriptFrameConstants::kFunctionOffset));
    DCHECK(stack_slots >= frame()->GetOsrStackSlotCount());
    stack_slots -= frame()->GetOsrStackSlotCount();
  }

  if (stack_slots > 0) {
    __ Add(sp, sp, -stack_slots * kPointerSize, r0);
  }
}


void CodeGenerator::AssembleReturn() {
  CallDescriptor* descriptor = linkage()->GetIncomingDescriptor();
  int stack_slots = frame()->GetSpillSlotCount();
  if (descriptor->kind() == CallDescriptor::kCallAddress) {
    if (frame()->GetRegisterSaveAreaSize() > 0) {
      // Remove this frame's spill slots first.
      if (stack_slots > 0) {
        __ Add(sp, sp, stack_slots * kPointerSize, r0);
      }
      // Restore registers.
      RegList frame_saves = fp.bit();
      if (FLAG_enable_embedded_constant_pool) {
        frame_saves |= kConstantPoolRegister.bit();
      }
      const RegList saves = descriptor->CalleeSavedRegisters() & ~frame_saves;
      if (saves != 0) {
        __ MultiPop(saves);
      }
    }
    __ LeaveFrame(StackFrame::MANUAL);
    __ Ret();
  } else if (descriptor->IsJSFunctionCall() || needs_frame_) {
    // Canonicalize JSFunction return sites for now.
    if (return_label_.is_bound()) {
      __ b(&return_label_);
    } else {
      __ bind(&return_label_);
      int pop_count = descriptor->IsJSFunctionCall()
                          ? static_cast<int>(descriptor->JSParameterCount())
                          : (info()->IsStub()
                                 ? info()->code_stub()->GetStackParameterCount()
                                 : 0);
      __ LeaveFrame(StackFrame::MANUAL, pop_count * kPointerSize);
      __ Ret();
    }
  } else {
    __ Ret();
  }
}


void CodeGenerator::AssembleMove(InstructionOperand* source,
                                 InstructionOperand* destination) {
  PPCOperandConverter g(this, NULL);
  // Dispatch on the source and destination operand kinds.  Not all
  // combinations are possible.
  if (source->IsRegister()) {
    DCHECK(destination->IsRegister() || destination->IsStackSlot());
    Register src = g.ToRegister(source);
    if (destination->IsRegister()) {
      __ Move(g.ToRegister(destination), src);
    } else {
      __ StoreP(src, g.ToMemOperand(destination), r0);
    }
  } else if (source->IsStackSlot()) {
    DCHECK(destination->IsRegister() || destination->IsStackSlot());
    MemOperand src = g.ToMemOperand(source);
    if (destination->IsRegister()) {
      __ LoadP(g.ToRegister(destination), src, r0);
    } else {
      Register temp = kScratchReg;
      __ LoadP(temp, src, r0);
      __ StoreP(temp, g.ToMemOperand(destination), r0);
    }
  } else if (source->IsConstant()) {
    Constant src = g.ToConstant(source);
    if (destination->IsRegister() || destination->IsStackSlot()) {
      Register dst =
          destination->IsRegister() ? g.ToRegister(destination) : kScratchReg;
      switch (src.type()) {
        case Constant::kInt32:
          __ mov(dst, Operand(src.ToInt32()));
          break;
        case Constant::kInt64:
          __ mov(dst, Operand(src.ToInt64()));
          break;
        case Constant::kFloat32:
          __ Move(dst,
                  isolate()->factory()->NewNumber(src.ToFloat32(), TENURED));
          break;
        case Constant::kFloat64:
          __ Move(dst,
                  isolate()->factory()->NewNumber(src.ToFloat64(), TENURED));
          break;
        case Constant::kExternalReference:
          __ mov(dst, Operand(src.ToExternalReference()));
          break;
        case Constant::kHeapObject: {
          Handle<HeapObject> src_object = src.ToHeapObject();
          Heap::RootListIndex index;
          int offset;
          if (IsMaterializableFromFrame(src_object, &offset)) {
            __ LoadP(dst, MemOperand(fp, offset));
          } else if (IsMaterializableFromRoot(src_object, &index)) {
            __ LoadRoot(dst, index);
          } else {
            __ Move(dst, src_object);
          }
          break;
        }
        case Constant::kRpoNumber:
          UNREACHABLE();  // TODO(dcarney): loading RPO constants on PPC.
          break;
      }
      if (destination->IsStackSlot()) {
        __ StoreP(dst, g.ToMemOperand(destination), r0);
      }
    } else {
      DoubleRegister dst = destination->IsDoubleRegister()
                               ? g.ToDoubleRegister(destination)
                               : kScratchDoubleReg;
      double value = (src.type() == Constant::kFloat32) ? src.ToFloat32()
                                                        : src.ToFloat64();
      __ LoadDoubleLiteral(dst, value, kScratchReg);
      if (destination->IsDoubleStackSlot()) {
        __ StoreDouble(dst, g.ToMemOperand(destination), r0);
      }
    }
  } else if (source->IsDoubleRegister()) {
    DoubleRegister src = g.ToDoubleRegister(source);
    if (destination->IsDoubleRegister()) {
      DoubleRegister dst = g.ToDoubleRegister(destination);
      __ Move(dst, src);
    } else {
      DCHECK(destination->IsDoubleStackSlot());
      __ StoreDouble(src, g.ToMemOperand(destination), r0);
    }
  } else if (source->IsDoubleStackSlot()) {
    DCHECK(destination->IsDoubleRegister() || destination->IsDoubleStackSlot());
    MemOperand src = g.ToMemOperand(source);
    if (destination->IsDoubleRegister()) {
      __ LoadDouble(g.ToDoubleRegister(destination), src, r0);
    } else {
      DoubleRegister temp = kScratchDoubleReg;
      __ LoadDouble(temp, src, r0);
      __ StoreDouble(temp, g.ToMemOperand(destination), r0);
    }
  } else {
    UNREACHABLE();
  }
}


void CodeGenerator::AssembleSwap(InstructionOperand* source,
                                 InstructionOperand* destination) {
  PPCOperandConverter g(this, NULL);
  // Dispatch on the source and destination operand kinds.  Not all
  // combinations are possible.
  if (source->IsRegister()) {
    // Register-register.
    Register temp = kScratchReg;
    Register src = g.ToRegister(source);
    if (destination->IsRegister()) {
      Register dst = g.ToRegister(destination);
      __ mr(temp, src);
      __ mr(src, dst);
      __ mr(dst, temp);
    } else {
      DCHECK(destination->IsStackSlot());
      MemOperand dst = g.ToMemOperand(destination);
      __ mr(temp, src);
      __ LoadP(src, dst);
      __ StoreP(temp, dst);
    }
#if V8_TARGET_ARCH_PPC64
  } else if (source->IsStackSlot() || source->IsDoubleStackSlot()) {
#else
  } else if (source->IsStackSlot()) {
    DCHECK(destination->IsStackSlot());
#endif
    Register temp_0 = kScratchReg;
    Register temp_1 = r0;
    MemOperand src = g.ToMemOperand(source);
    MemOperand dst = g.ToMemOperand(destination);
    __ LoadP(temp_0, src);
    __ LoadP(temp_1, dst);
    __ StoreP(temp_0, dst);
    __ StoreP(temp_1, src);
  } else if (source->IsDoubleRegister()) {
    DoubleRegister temp = kScratchDoubleReg;
    DoubleRegister src = g.ToDoubleRegister(source);
    if (destination->IsDoubleRegister()) {
      DoubleRegister dst = g.ToDoubleRegister(destination);
      __ fmr(temp, src);
      __ fmr(src, dst);
      __ fmr(dst, temp);
    } else {
      DCHECK(destination->IsDoubleStackSlot());
      MemOperand dst = g.ToMemOperand(destination);
      __ fmr(temp, src);
      __ lfd(src, dst);
      __ stfd(temp, dst);
    }
#if !V8_TARGET_ARCH_PPC64
  } else if (source->IsDoubleStackSlot()) {
    DCHECK(destination->IsDoubleStackSlot());
    DoubleRegister temp_0 = kScratchDoubleReg;
    DoubleRegister temp_1 = d0;
    MemOperand src = g.ToMemOperand(source);
    MemOperand dst = g.ToMemOperand(destination);
    __ lfd(temp_0, src);
    __ lfd(temp_1, dst);
    __ stfd(temp_0, dst);
    __ stfd(temp_1, src);
#endif
  } else {
    // No other combinations are possible.
    UNREACHABLE();
  }
}


void CodeGenerator::AssembleJumpTable(Label** targets, size_t target_count) {
  for (size_t index = 0; index < target_count; ++index) {
    __ emit_label_addr(targets[index]);
  }
}


void CodeGenerator::AddNopForSmiCodeInlining() {
  // We do not insert nops for inlined Smi code.
}


void CodeGenerator::EnsureSpaceForLazyDeopt() {
  int space_needed = Deoptimizer::patch_size();
  if (!info()->IsStub()) {
    // Ensure that we have enough space after the previous lazy-bailout
    // instruction for patching the code here.
    int current_pc = masm()->pc_offset();
    if (current_pc < last_lazy_deopt_pc_ + space_needed) {
      int padding_size = last_lazy_deopt_pc_ + space_needed - current_pc;
      DCHECK_EQ(0, padding_size % v8::internal::Assembler::kInstrSize);
      while (padding_size > 0) {
        __ nop();
        padding_size -= v8::internal::Assembler::kInstrSize;
      }
    }
  }
}

#undef __

}  // namespace compiler
}  // namespace internal
}  // namespace v8
