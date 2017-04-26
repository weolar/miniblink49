// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/code-generator.h"

#include "src/compiler/code-generator-impl.h"
#include "src/compiler/gap-resolver.h"
#include "src/compiler/node-matchers.h"
#include "src/scopes.h"
#include "src/x64/assembler-x64.h"
#include "src/x64/macro-assembler-x64.h"

namespace v8 {
namespace internal {
namespace compiler {

#define __ masm()->


#define kScratchDoubleReg xmm0


// Adds X64 specific methods for decoding operands.
class X64OperandConverter : public InstructionOperandConverter {
 public:
  X64OperandConverter(CodeGenerator* gen, Instruction* instr)
      : InstructionOperandConverter(gen, instr) {}

  Immediate InputImmediate(size_t index) {
    return ToImmediate(instr_->InputAt(index));
  }

  Operand InputOperand(size_t index, int extra = 0) {
    return ToOperand(instr_->InputAt(index), extra);
  }

  Operand OutputOperand() { return ToOperand(instr_->Output()); }

  Immediate ToImmediate(InstructionOperand* operand) {
    Constant constant = ToConstant(operand);
    if (constant.type() == Constant::kFloat64) {
      DCHECK_EQ(0, bit_cast<int64_t>(constant.ToFloat64()));
      return Immediate(0);
    }
    return Immediate(constant.ToInt32());
  }

  Operand ToOperand(InstructionOperand* op, int extra = 0) {
    DCHECK(op->IsStackSlot() || op->IsDoubleStackSlot());
    // The linkage computes where all spill slots are located.
    FrameOffset offset = linkage()->GetFrameOffset(
        AllocatedOperand::cast(op)->index(), frame(), extra);
    return Operand(offset.from_stack_pointer() ? rsp : rbp, offset.offset());
  }

  static size_t NextOffset(size_t* offset) {
    size_t i = *offset;
    (*offset)++;
    return i;
  }

  static ScaleFactor ScaleFor(AddressingMode one, AddressingMode mode) {
    STATIC_ASSERT(0 == static_cast<int>(times_1));
    STATIC_ASSERT(1 == static_cast<int>(times_2));
    STATIC_ASSERT(2 == static_cast<int>(times_4));
    STATIC_ASSERT(3 == static_cast<int>(times_8));
    int scale = static_cast<int>(mode - one);
    DCHECK(scale >= 0 && scale < 4);
    return static_cast<ScaleFactor>(scale);
  }

  Operand MemoryOperand(size_t* offset) {
    AddressingMode mode = AddressingModeField::decode(instr_->opcode());
    switch (mode) {
      case kMode_MR: {
        Register base = InputRegister(NextOffset(offset));
        int32_t disp = 0;
        return Operand(base, disp);
      }
      case kMode_MRI: {
        Register base = InputRegister(NextOffset(offset));
        int32_t disp = InputInt32(NextOffset(offset));
        return Operand(base, disp);
      }
      case kMode_MR1:
      case kMode_MR2:
      case kMode_MR4:
      case kMode_MR8: {
        Register base = InputRegister(NextOffset(offset));
        Register index = InputRegister(NextOffset(offset));
        ScaleFactor scale = ScaleFor(kMode_MR1, mode);
        int32_t disp = 0;
        return Operand(base, index, scale, disp);
      }
      case kMode_MR1I:
      case kMode_MR2I:
      case kMode_MR4I:
      case kMode_MR8I: {
        Register base = InputRegister(NextOffset(offset));
        Register index = InputRegister(NextOffset(offset));
        ScaleFactor scale = ScaleFor(kMode_MR1I, mode);
        int32_t disp = InputInt32(NextOffset(offset));
        return Operand(base, index, scale, disp);
      }
      case kMode_M1: {
        Register base = InputRegister(NextOffset(offset));
        int32_t disp = 0;
        return Operand(base, disp);
      }
      case kMode_M2:
        UNREACHABLE();  // Should use kModeMR with more compact encoding instead
        return Operand(no_reg, 0);
      case kMode_M4:
      case kMode_M8: {
        Register index = InputRegister(NextOffset(offset));
        ScaleFactor scale = ScaleFor(kMode_M1, mode);
        int32_t disp = 0;
        return Operand(index, scale, disp);
      }
      case kMode_M1I:
      case kMode_M2I:
      case kMode_M4I:
      case kMode_M8I: {
        Register index = InputRegister(NextOffset(offset));
        ScaleFactor scale = ScaleFor(kMode_M1I, mode);
        int32_t disp = InputInt32(NextOffset(offset));
        return Operand(index, scale, disp);
      }
      case kMode_None:
        UNREACHABLE();
        return Operand(no_reg, 0);
    }
    UNREACHABLE();
    return Operand(no_reg, 0);
  }

  Operand MemoryOperand(size_t first_input = 0) {
    return MemoryOperand(&first_input);
  }
};


namespace {

bool HasImmediateInput(Instruction* instr, size_t index) {
  return instr->InputAt(index)->IsImmediate();
}


class OutOfLineLoadZero final : public OutOfLineCode {
 public:
  OutOfLineLoadZero(CodeGenerator* gen, Register result)
      : OutOfLineCode(gen), result_(result) {}

  void Generate() final { __ xorl(result_, result_); }

 private:
  Register const result_;
};


class OutOfLineLoadNaN final : public OutOfLineCode {
 public:
  OutOfLineLoadNaN(CodeGenerator* gen, XMMRegister result)
      : OutOfLineCode(gen), result_(result) {}

  void Generate() final { __ pcmpeqd(result_, result_); }

 private:
  XMMRegister const result_;
};


class OutOfLineTruncateDoubleToI final : public OutOfLineCode {
 public:
  OutOfLineTruncateDoubleToI(CodeGenerator* gen, Register result,
                             XMMRegister input)
      : OutOfLineCode(gen), result_(result), input_(input) {}

  void Generate() final {
    __ subp(rsp, Immediate(kDoubleSize));
    __ movsd(MemOperand(rsp, 0), input_);
    __ SlowTruncateToI(result_, rsp, 0);
    __ addp(rsp, Immediate(kDoubleSize));
  }

 private:
  Register const result_;
  XMMRegister const input_;
};

}  // namespace


#define ASSEMBLE_UNOP(asm_instr)         \
  do {                                   \
    if (instr->Output()->IsRegister()) { \
      __ asm_instr(i.OutputRegister());  \
    } else {                             \
      __ asm_instr(i.OutputOperand());   \
    }                                    \
  } while (0)


#define ASSEMBLE_BINOP(asm_instr)                              \
  do {                                                         \
    if (HasImmediateInput(instr, 1)) {                         \
      if (instr->InputAt(0)->IsRegister()) {                   \
        __ asm_instr(i.InputRegister(0), i.InputImmediate(1)); \
      } else {                                                 \
        __ asm_instr(i.InputOperand(0), i.InputImmediate(1));  \
      }                                                        \
    } else {                                                   \
      if (instr->InputAt(1)->IsRegister()) {                   \
        __ asm_instr(i.InputRegister(0), i.InputRegister(1));  \
      } else {                                                 \
        __ asm_instr(i.InputRegister(0), i.InputOperand(1));   \
      }                                                        \
    }                                                          \
  } while (0)


#define ASSEMBLE_MULT(asm_instr)                              \
  do {                                                        \
    if (HasImmediateInput(instr, 1)) {                        \
      if (instr->InputAt(0)->IsRegister()) {                  \
        __ asm_instr(i.OutputRegister(), i.InputRegister(0),  \
                     i.InputImmediate(1));                    \
      } else {                                                \
        __ asm_instr(i.OutputRegister(), i.InputOperand(0),   \
                     i.InputImmediate(1));                    \
      }                                                       \
    } else {                                                  \
      if (instr->InputAt(1)->IsRegister()) {                  \
        __ asm_instr(i.OutputRegister(), i.InputRegister(1)); \
      } else {                                                \
        __ asm_instr(i.OutputRegister(), i.InputOperand(1));  \
      }                                                       \
    }                                                         \
  } while (0)


#define ASSEMBLE_SHIFT(asm_instr, width)                                   \
  do {                                                                     \
    if (HasImmediateInput(instr, 1)) {                                     \
      if (instr->Output()->IsRegister()) {                                 \
        __ asm_instr(i.OutputRegister(), Immediate(i.InputInt##width(1))); \
      } else {                                                             \
        __ asm_instr(i.OutputOperand(), Immediate(i.InputInt##width(1)));  \
      }                                                                    \
    } else {                                                               \
      if (instr->Output()->IsRegister()) {                                 \
        __ asm_instr##_cl(i.OutputRegister());                             \
      } else {                                                             \
        __ asm_instr##_cl(i.OutputOperand());                              \
      }                                                                    \
    }                                                                      \
  } while (0)


#define ASSEMBLE_MOVX(asm_instr)                            \
  do {                                                      \
    if (instr->addressing_mode() != kMode_None) {           \
      __ asm_instr(i.OutputRegister(), i.MemoryOperand());  \
    } else if (instr->InputAt(0)->IsRegister()) {           \
      __ asm_instr(i.OutputRegister(), i.InputRegister(0)); \
    } else {                                                \
      __ asm_instr(i.OutputRegister(), i.InputOperand(0));  \
    }                                                       \
  } while (0)


#define ASSEMBLE_SSE_BINOP(asm_instr)                                   \
  do {                                                                  \
    if (instr->InputAt(1)->IsDoubleRegister()) {                        \
      __ asm_instr(i.InputDoubleRegister(0), i.InputDoubleRegister(1)); \
    } else {                                                            \
      __ asm_instr(i.InputDoubleRegister(0), i.InputOperand(1));        \
    }                                                                   \
  } while (0)


#define ASSEMBLE_SSE_UNOP(asm_instr)                                    \
  do {                                                                  \
    if (instr->InputAt(0)->IsDoubleRegister()) {                        \
      __ asm_instr(i.OutputDoubleRegister(), i.InputDoubleRegister(0)); \
    } else {                                                            \
      __ asm_instr(i.OutputDoubleRegister(), i.InputOperand(0));        \
    }                                                                   \
  } while (0)


#define ASSEMBLE_AVX_BINOP(asm_instr)                                  \
  do {                                                                 \
    CpuFeatureScope avx_scope(masm(), AVX);                            \
    if (instr->InputAt(1)->IsDoubleRegister()) {                       \
      __ asm_instr(i.OutputDoubleRegister(), i.InputDoubleRegister(0), \
                   i.InputDoubleRegister(1));                          \
    } else {                                                           \
      __ asm_instr(i.OutputDoubleRegister(), i.InputDoubleRegister(0), \
                   i.InputOperand(1));                                 \
    }                                                                  \
  } while (0)


#define ASSEMBLE_CHECKED_LOAD_FLOAT(asm_instr)                               \
  do {                                                                       \
    auto result = i.OutputDoubleRegister();                                  \
    auto buffer = i.InputRegister(0);                                        \
    auto index1 = i.InputRegister(1);                                        \
    auto index2 = i.InputInt32(2);                                           \
    OutOfLineCode* ool;                                                      \
    if (instr->InputAt(3)->IsRegister()) {                                   \
      auto length = i.InputRegister(3);                                      \
      DCHECK_EQ(0, index2);                                                  \
      __ cmpl(index1, length);                                               \
      ool = new (zone()) OutOfLineLoadNaN(this, result);                     \
    } else {                                                                 \
      auto length = i.InputInt32(3);                                         \
      DCHECK_LE(index2, length);                                             \
      __ cmpq(index1, Immediate(length - index2));                           \
      class OutOfLineLoadFloat final : public OutOfLineCode {                \
       public:                                                               \
        OutOfLineLoadFloat(CodeGenerator* gen, XMMRegister result,           \
                           Register buffer, Register index1, int32_t index2, \
                           int32_t length)                                   \
            : OutOfLineCode(gen),                                            \
              result_(result),                                               \
              buffer_(buffer),                                               \
              index1_(index1),                                               \
              index2_(index2),                                               \
              length_(length) {}                                             \
                                                                             \
        void Generate() final {                                              \
          __ leal(kScratchRegister, Operand(index1_, index2_));              \
          __ pcmpeqd(result_, result_);                                      \
          __ cmpl(kScratchRegister, Immediate(length_));                     \
          __ j(above_equal, exit());                                         \
          __ asm_instr(result_,                                              \
                       Operand(buffer_, kScratchRegister, times_1, 0));      \
        }                                                                    \
                                                                             \
       private:                                                              \
        XMMRegister const result_;                                           \
        Register const buffer_;                                              \
        Register const index1_;                                              \
        int32_t const index2_;                                               \
        int32_t const length_;                                               \
      };                                                                     \
      ool = new (zone())                                                     \
          OutOfLineLoadFloat(this, result, buffer, index1, index2, length);  \
    }                                                                        \
    __ j(above_equal, ool->entry());                                         \
    __ asm_instr(result, Operand(buffer, index1, times_1, index2));          \
    __ bind(ool->exit());                                                    \
  } while (false)


#define ASSEMBLE_CHECKED_LOAD_INTEGER(asm_instr)                               \
  do {                                                                         \
    auto result = i.OutputRegister();                                          \
    auto buffer = i.InputRegister(0);                                          \
    auto index1 = i.InputRegister(1);                                          \
    auto index2 = i.InputInt32(2);                                             \
    OutOfLineCode* ool;                                                        \
    if (instr->InputAt(3)->IsRegister()) {                                     \
      auto length = i.InputRegister(3);                                        \
      DCHECK_EQ(0, index2);                                                    \
      __ cmpl(index1, length);                                                 \
      ool = new (zone()) OutOfLineLoadZero(this, result);                      \
    } else {                                                                   \
      auto length = i.InputInt32(3);                                           \
      DCHECK_LE(index2, length);                                               \
      __ cmpq(index1, Immediate(length - index2));                             \
      class OutOfLineLoadInteger final : public OutOfLineCode {                \
       public:                                                                 \
        OutOfLineLoadInteger(CodeGenerator* gen, Register result,              \
                             Register buffer, Register index1, int32_t index2, \
                             int32_t length)                                   \
            : OutOfLineCode(gen),                                              \
              result_(result),                                                 \
              buffer_(buffer),                                                 \
              index1_(index1),                                                 \
              index2_(index2),                                                 \
              length_(length) {}                                               \
                                                                               \
        void Generate() final {                                                \
          Label oob;                                                           \
          __ leal(kScratchRegister, Operand(index1_, index2_));                \
          __ cmpl(kScratchRegister, Immediate(length_));                       \
          __ j(above_equal, &oob, Label::kNear);                               \
          __ asm_instr(result_,                                                \
                       Operand(buffer_, kScratchRegister, times_1, 0));        \
          __ jmp(exit());                                                      \
          __ bind(&oob);                                                       \
          __ xorl(result_, result_);                                           \
        }                                                                      \
                                                                               \
       private:                                                                \
        Register const result_;                                                \
        Register const buffer_;                                                \
        Register const index1_;                                                \
        int32_t const index2_;                                                 \
        int32_t const length_;                                                 \
      };                                                                       \
      ool = new (zone())                                                       \
          OutOfLineLoadInteger(this, result, buffer, index1, index2, length);  \
    }                                                                          \
    __ j(above_equal, ool->entry());                                           \
    __ asm_instr(result, Operand(buffer, index1, times_1, index2));            \
    __ bind(ool->exit());                                                      \
  } while (false)


#define ASSEMBLE_CHECKED_STORE_FLOAT(asm_instr)                              \
  do {                                                                       \
    auto buffer = i.InputRegister(0);                                        \
    auto index1 = i.InputRegister(1);                                        \
    auto index2 = i.InputInt32(2);                                           \
    auto value = i.InputDoubleRegister(4);                                   \
    if (instr->InputAt(3)->IsRegister()) {                                   \
      auto length = i.InputRegister(3);                                      \
      DCHECK_EQ(0, index2);                                                  \
      Label done;                                                            \
      __ cmpl(index1, length);                                               \
      __ j(above_equal, &done, Label::kNear);                                \
      __ asm_instr(Operand(buffer, index1, times_1, index2), value);         \
      __ bind(&done);                                                        \
    } else {                                                                 \
      auto length = i.InputInt32(3);                                         \
      DCHECK_LE(index2, length);                                             \
      __ cmpq(index1, Immediate(length - index2));                           \
      class OutOfLineStoreFloat final : public OutOfLineCode {               \
       public:                                                               \
        OutOfLineStoreFloat(CodeGenerator* gen, Register buffer,             \
                            Register index1, int32_t index2, int32_t length, \
                            XMMRegister value)                               \
            : OutOfLineCode(gen),                                            \
              buffer_(buffer),                                               \
              index1_(index1),                                               \
              index2_(index2),                                               \
              length_(length),                                               \
              value_(value) {}                                               \
                                                                             \
        void Generate() final {                                              \
          __ leal(kScratchRegister, Operand(index1_, index2_));              \
          __ cmpl(kScratchRegister, Immediate(length_));                     \
          __ j(above_equal, exit());                                         \
          __ asm_instr(Operand(buffer_, kScratchRegister, times_1, 0),       \
                       value_);                                              \
        }                                                                    \
                                                                             \
       private:                                                              \
        Register const buffer_;                                              \
        Register const index1_;                                              \
        int32_t const index2_;                                               \
        int32_t const length_;                                               \
        XMMRegister const value_;                                            \
      };                                                                     \
      auto ool = new (zone())                                                \
          OutOfLineStoreFloat(this, buffer, index1, index2, length, value);  \
      __ j(above_equal, ool->entry());                                       \
      __ asm_instr(Operand(buffer, index1, times_1, index2), value);         \
      __ bind(ool->exit());                                                  \
    }                                                                        \
  } while (false)


#define ASSEMBLE_CHECKED_STORE_INTEGER_IMPL(asm_instr, Value)                  \
  do {                                                                         \
    auto buffer = i.InputRegister(0);                                          \
    auto index1 = i.InputRegister(1);                                          \
    auto index2 = i.InputInt32(2);                                             \
    if (instr->InputAt(3)->IsRegister()) {                                     \
      auto length = i.InputRegister(3);                                        \
      DCHECK_EQ(0, index2);                                                    \
      Label done;                                                              \
      __ cmpl(index1, length);                                                 \
      __ j(above_equal, &done, Label::kNear);                                  \
      __ asm_instr(Operand(buffer, index1, times_1, index2), value);           \
      __ bind(&done);                                                          \
    } else {                                                                   \
      auto length = i.InputInt32(3);                                           \
      DCHECK_LE(index2, length);                                               \
      __ cmpq(index1, Immediate(length - index2));                             \
      class OutOfLineStoreInteger final : public OutOfLineCode {               \
       public:                                                                 \
        OutOfLineStoreInteger(CodeGenerator* gen, Register buffer,             \
                              Register index1, int32_t index2, int32_t length, \
                              Value value)                                     \
            : OutOfLineCode(gen),                                              \
              buffer_(buffer),                                                 \
              index1_(index1),                                                 \
              index2_(index2),                                                 \
              length_(length),                                                 \
              value_(value) {}                                                 \
                                                                               \
        void Generate() final {                                                \
          __ leal(kScratchRegister, Operand(index1_, index2_));                \
          __ cmpl(kScratchRegister, Immediate(length_));                       \
          __ j(above_equal, exit());                                           \
          __ asm_instr(Operand(buffer_, kScratchRegister, times_1, 0),         \
                       value_);                                                \
        }                                                                      \
                                                                               \
       private:                                                                \
        Register const buffer_;                                                \
        Register const index1_;                                                \
        int32_t const index2_;                                                 \
        int32_t const length_;                                                 \
        Value const value_;                                                    \
      };                                                                       \
      auto ool = new (zone())                                                  \
          OutOfLineStoreInteger(this, buffer, index1, index2, length, value);  \
      __ j(above_equal, ool->entry());                                         \
      __ asm_instr(Operand(buffer, index1, times_1, index2), value);           \
      __ bind(ool->exit());                                                    \
    }                                                                          \
  } while (false)


#define ASSEMBLE_CHECKED_STORE_INTEGER(asm_instr)                \
  do {                                                           \
    if (instr->InputAt(4)->IsRegister()) {                       \
      Register value = i.InputRegister(4);                       \
      ASSEMBLE_CHECKED_STORE_INTEGER_IMPL(asm_instr, Register);  \
    } else {                                                     \
      Immediate value = i.InputImmediate(4);                     \
      ASSEMBLE_CHECKED_STORE_INTEGER_IMPL(asm_instr, Immediate); \
    }                                                            \
  } while (false)


void CodeGenerator::AssembleDeconstructActivationRecord() {
  CallDescriptor* descriptor = linkage()->GetIncomingDescriptor();
  int stack_slots = frame()->GetSpillSlotCount();
  if (descriptor->IsJSFunctionCall() || stack_slots > 0) {
    __ movq(rsp, rbp);
    __ popq(rbp);
  }
}


// Assembles an instruction after register allocation, producing machine code.
void CodeGenerator::AssembleArchInstruction(Instruction* instr) {
  X64OperandConverter i(this, instr);

  switch (ArchOpcodeField::decode(instr->opcode())) {
    case kArchCallCodeObject: {
      EnsureSpaceForLazyDeopt();
      if (HasImmediateInput(instr, 0)) {
        Handle<Code> code = Handle<Code>::cast(i.InputHeapObject(0));
        __ Call(code, RelocInfo::CODE_TARGET);
      } else {
        Register reg = i.InputRegister(0);
        int entry = Code::kHeaderSize - kHeapObjectTag;
        __ Call(Operand(reg, entry));
      }
      RecordCallPosition(instr);
      break;
    }
    case kArchTailCallCodeObject: {
      AssembleDeconstructActivationRecord();
      if (HasImmediateInput(instr, 0)) {
        Handle<Code> code = Handle<Code>::cast(i.InputHeapObject(0));
        __ jmp(code, RelocInfo::CODE_TARGET);
      } else {
        Register reg = i.InputRegister(0);
        int entry = Code::kHeaderSize - kHeapObjectTag;
        __ jmp(Operand(reg, entry));
      }
      break;
    }
    case kArchCallJSFunction: {
      EnsureSpaceForLazyDeopt();
      Register func = i.InputRegister(0);
      if (FLAG_debug_code) {
        // Check the function's context matches the context argument.
        __ cmpp(rsi, FieldOperand(func, JSFunction::kContextOffset));
        __ Assert(equal, kWrongFunctionContext);
      }
      __ Call(FieldOperand(func, JSFunction::kCodeEntryOffset));
      RecordCallPosition(instr);
      break;
    }
    case kArchTailCallJSFunction: {
      Register func = i.InputRegister(0);
      if (FLAG_debug_code) {
        // Check the function's context matches the context argument.
        __ cmpp(rsi, FieldOperand(func, JSFunction::kContextOffset));
        __ Assert(equal, kWrongFunctionContext);
      }
      AssembleDeconstructActivationRecord();
      __ jmp(FieldOperand(func, JSFunction::kCodeEntryOffset));
      break;
    }
    case kArchPrepareCallCFunction: {
      int const num_parameters = MiscField::decode(instr->opcode());
      __ PrepareCallCFunction(num_parameters);
      break;
    }
    case kArchCallCFunction: {
      int const num_parameters = MiscField::decode(instr->opcode());
      if (HasImmediateInput(instr, 0)) {
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
      break;
    case kArchLookupSwitch:
      AssembleArchLookupSwitch(instr);
      break;
    case kArchTableSwitch:
      AssembleArchTableSwitch(instr);
      break;
    case kArchNop:
      // don't emit code for nops.
      break;
    case kArchDeoptimize: {
      int deopt_state_id =
          BuildTranslation(instr, -1, 0, OutputFrameStateCombine::Ignore());
      AssembleDeoptimizerCall(deopt_state_id, Deoptimizer::EAGER);
      break;
    }
    case kArchRet:
      AssembleReturn();
      break;
    case kArchStackPointer:
      __ movq(i.OutputRegister(), rsp);
      break;
    case kArchFramePointer:
      __ movq(i.OutputRegister(), rbp);
      break;
    case kArchTruncateDoubleToI: {
      auto result = i.OutputRegister();
      auto input = i.InputDoubleRegister(0);
      auto ool = new (zone()) OutOfLineTruncateDoubleToI(this, result, input);
      __ cvttsd2siq(result, input);
      __ cmpq(result, Immediate(1));
      __ j(overflow, ool->entry());
      __ bind(ool->exit());
      break;
    }
    case kX64Add32:
      ASSEMBLE_BINOP(addl);
      break;
    case kX64Add:
      ASSEMBLE_BINOP(addq);
      break;
    case kX64Sub32:
      ASSEMBLE_BINOP(subl);
      break;
    case kX64Sub:
      ASSEMBLE_BINOP(subq);
      break;
    case kX64And32:
      ASSEMBLE_BINOP(andl);
      break;
    case kX64And:
      ASSEMBLE_BINOP(andq);
      break;
    case kX64Cmp32:
      ASSEMBLE_BINOP(cmpl);
      break;
    case kX64Cmp:
      ASSEMBLE_BINOP(cmpq);
      break;
    case kX64Test32:
      ASSEMBLE_BINOP(testl);
      break;
    case kX64Test:
      ASSEMBLE_BINOP(testq);
      break;
    case kX64Imul32:
      ASSEMBLE_MULT(imull);
      break;
    case kX64Imul:
      ASSEMBLE_MULT(imulq);
      break;
    case kX64ImulHigh32:
      if (instr->InputAt(1)->IsRegister()) {
        __ imull(i.InputRegister(1));
      } else {
        __ imull(i.InputOperand(1));
      }
      break;
    case kX64UmulHigh32:
      if (instr->InputAt(1)->IsRegister()) {
        __ mull(i.InputRegister(1));
      } else {
        __ mull(i.InputOperand(1));
      }
      break;
    case kX64Idiv32:
      __ cdq();
      __ idivl(i.InputRegister(1));
      break;
    case kX64Idiv:
      __ cqo();
      __ idivq(i.InputRegister(1));
      break;
    case kX64Udiv32:
      __ xorl(rdx, rdx);
      __ divl(i.InputRegister(1));
      break;
    case kX64Udiv:
      __ xorq(rdx, rdx);
      __ divq(i.InputRegister(1));
      break;
    case kX64Not:
      ASSEMBLE_UNOP(notq);
      break;
    case kX64Not32:
      ASSEMBLE_UNOP(notl);
      break;
    case kX64Neg:
      ASSEMBLE_UNOP(negq);
      break;
    case kX64Neg32:
      ASSEMBLE_UNOP(negl);
      break;
    case kX64Or32:
      ASSEMBLE_BINOP(orl);
      break;
    case kX64Or:
      ASSEMBLE_BINOP(orq);
      break;
    case kX64Xor32:
      ASSEMBLE_BINOP(xorl);
      break;
    case kX64Xor:
      ASSEMBLE_BINOP(xorq);
      break;
    case kX64Shl32:
      ASSEMBLE_SHIFT(shll, 5);
      break;
    case kX64Shl:
      ASSEMBLE_SHIFT(shlq, 6);
      break;
    case kX64Shr32:
      ASSEMBLE_SHIFT(shrl, 5);
      break;
    case kX64Shr:
      ASSEMBLE_SHIFT(shrq, 6);
      break;
    case kX64Sar32:
      ASSEMBLE_SHIFT(sarl, 5);
      break;
    case kX64Sar:
      ASSEMBLE_SHIFT(sarq, 6);
      break;
    case kX64Ror32:
      ASSEMBLE_SHIFT(rorl, 5);
      break;
    case kX64Ror:
      ASSEMBLE_SHIFT(rorq, 6);
      break;
    case kX64Lzcnt32:
      if (instr->InputAt(0)->IsRegister()) {
        __ Lzcntl(i.OutputRegister(), i.InputRegister(0));
      } else {
        __ Lzcntl(i.OutputRegister(), i.InputOperand(0));
      }
      break;
    case kSSEFloat32Cmp:
      ASSEMBLE_SSE_BINOP(ucomiss);
      break;
    case kSSEFloat32Add:
      ASSEMBLE_SSE_BINOP(addss);
      break;
    case kSSEFloat32Sub:
      ASSEMBLE_SSE_BINOP(subss);
      break;
    case kSSEFloat32Mul:
      ASSEMBLE_SSE_BINOP(mulss);
      break;
    case kSSEFloat32Div:
      ASSEMBLE_SSE_BINOP(divss);
      // Don't delete this mov. It may improve performance on some CPUs,
      // when there is a (v)mulss depending on the result.
      __ movaps(i.OutputDoubleRegister(), i.OutputDoubleRegister());
      break;
    case kSSEFloat32Abs: {
      // TODO(bmeurer): Use RIP relative 128-bit constants.
      __ pcmpeqd(kScratchDoubleReg, kScratchDoubleReg);
      __ psrlq(kScratchDoubleReg, 33);
      __ andps(i.OutputDoubleRegister(), kScratchDoubleReg);
      break;
    }
    case kSSEFloat32Neg: {
      // TODO(bmeurer): Use RIP relative 128-bit constants.
      __ pcmpeqd(kScratchDoubleReg, kScratchDoubleReg);
      __ psllq(kScratchDoubleReg, 31);
      __ xorps(i.OutputDoubleRegister(), kScratchDoubleReg);
      break;
    }
    case kSSEFloat32Sqrt:
      ASSEMBLE_SSE_UNOP(sqrtss);
      break;
    case kSSEFloat32Max:
      ASSEMBLE_SSE_BINOP(maxss);
      break;
    case kSSEFloat32Min:
      ASSEMBLE_SSE_BINOP(minss);
      break;
    case kSSEFloat32ToFloat64:
      ASSEMBLE_SSE_UNOP(cvtss2sd);
      break;
    case kSSEFloat64Cmp:
      ASSEMBLE_SSE_BINOP(ucomisd);
      break;
    case kSSEFloat64Add:
      ASSEMBLE_SSE_BINOP(addsd);
      break;
    case kSSEFloat64Sub:
      ASSEMBLE_SSE_BINOP(subsd);
      break;
    case kSSEFloat64Mul:
      ASSEMBLE_SSE_BINOP(mulsd);
      break;
    case kSSEFloat64Div:
      ASSEMBLE_SSE_BINOP(divsd);
      // Don't delete this mov. It may improve performance on some CPUs,
      // when there is a (v)mulsd depending on the result.
      __ movaps(i.OutputDoubleRegister(), i.OutputDoubleRegister());
      break;
    case kSSEFloat64Mod: {
      __ subq(rsp, Immediate(kDoubleSize));
      // Move values to st(0) and st(1).
      __ movsd(Operand(rsp, 0), i.InputDoubleRegister(1));
      __ fld_d(Operand(rsp, 0));
      __ movsd(Operand(rsp, 0), i.InputDoubleRegister(0));
      __ fld_d(Operand(rsp, 0));
      // Loop while fprem isn't done.
      Label mod_loop;
      __ bind(&mod_loop);
      // This instructions traps on all kinds inputs, but we are assuming the
      // floating point control word is set to ignore them all.
      __ fprem();
      // The following 2 instruction implicitly use rax.
      __ fnstsw_ax();
      if (CpuFeatures::IsSupported(SAHF)) {
        CpuFeatureScope sahf_scope(masm(), SAHF);
        __ sahf();
      } else {
        __ shrl(rax, Immediate(8));
        __ andl(rax, Immediate(0xFF));
        __ pushq(rax);
        __ popfq();
      }
      __ j(parity_even, &mod_loop);
      // Move output to stack and clean up.
      __ fstp(1);
      __ fstp_d(Operand(rsp, 0));
      __ movsd(i.OutputDoubleRegister(), Operand(rsp, 0));
      __ addq(rsp, Immediate(kDoubleSize));
      break;
    }
    case kSSEFloat64Max:
      ASSEMBLE_SSE_BINOP(maxsd);
      break;
    case kSSEFloat64Min:
      ASSEMBLE_SSE_BINOP(minsd);
      break;
    case kSSEFloat64Abs: {
      // TODO(bmeurer): Use RIP relative 128-bit constants.
      __ pcmpeqd(kScratchDoubleReg, kScratchDoubleReg);
      __ psrlq(kScratchDoubleReg, 1);
      __ andpd(i.OutputDoubleRegister(), kScratchDoubleReg);
      break;
    }
    case kSSEFloat64Neg: {
      // TODO(bmeurer): Use RIP relative 128-bit constants.
      __ pcmpeqd(kScratchDoubleReg, kScratchDoubleReg);
      __ psllq(kScratchDoubleReg, 63);
      __ xorpd(i.OutputDoubleRegister(), kScratchDoubleReg);
      break;
    }
    case kSSEFloat64Sqrt:
      ASSEMBLE_SSE_UNOP(sqrtsd);
      break;
    case kSSEFloat64Round: {
      CpuFeatureScope sse_scope(masm(), SSE4_1);
      RoundingMode const mode =
          static_cast<RoundingMode>(MiscField::decode(instr->opcode()));
      __ roundsd(i.OutputDoubleRegister(), i.InputDoubleRegister(0), mode);
      break;
    }
    case kSSEFloat64ToFloat32:
      ASSEMBLE_SSE_UNOP(cvtsd2ss);
      break;
    case kSSEFloat64ToInt32:
      if (instr->InputAt(0)->IsDoubleRegister()) {
        __ cvttsd2si(i.OutputRegister(), i.InputDoubleRegister(0));
      } else {
        __ cvttsd2si(i.OutputRegister(), i.InputOperand(0));
      }
      break;
    case kSSEFloat64ToUint32: {
      if (instr->InputAt(0)->IsDoubleRegister()) {
        __ cvttsd2siq(i.OutputRegister(), i.InputDoubleRegister(0));
      } else {
        __ cvttsd2siq(i.OutputRegister(), i.InputOperand(0));
      }
      __ AssertZeroExtended(i.OutputRegister());
      break;
    }
    case kSSEInt32ToFloat64:
      if (instr->InputAt(0)->IsRegister()) {
        __ cvtlsi2sd(i.OutputDoubleRegister(), i.InputRegister(0));
      } else {
        __ cvtlsi2sd(i.OutputDoubleRegister(), i.InputOperand(0));
      }
      break;
    case kSSEUint32ToFloat64:
      if (instr->InputAt(0)->IsRegister()) {
        __ movl(kScratchRegister, i.InputRegister(0));
      } else {
        __ movl(kScratchRegister, i.InputOperand(0));
      }
      __ cvtqsi2sd(i.OutputDoubleRegister(), kScratchRegister);
      break;
    case kSSEFloat64ExtractLowWord32:
      if (instr->InputAt(0)->IsDoubleStackSlot()) {
        __ movl(i.OutputRegister(), i.InputOperand(0));
      } else {
        __ movd(i.OutputRegister(), i.InputDoubleRegister(0));
      }
      break;
    case kSSEFloat64ExtractHighWord32:
      if (instr->InputAt(0)->IsDoubleStackSlot()) {
        __ movl(i.OutputRegister(), i.InputOperand(0, kDoubleSize / 2));
      } else {
        __ Pextrd(i.OutputRegister(), i.InputDoubleRegister(0), 1);
      }
      break;
    case kSSEFloat64InsertLowWord32:
      if (instr->InputAt(1)->IsRegister()) {
        __ Pinsrd(i.OutputDoubleRegister(), i.InputRegister(1), 0);
      } else {
        __ Pinsrd(i.OutputDoubleRegister(), i.InputOperand(1), 0);
      }
      break;
    case kSSEFloat64InsertHighWord32:
      if (instr->InputAt(1)->IsRegister()) {
        __ Pinsrd(i.OutputDoubleRegister(), i.InputRegister(1), 1);
      } else {
        __ Pinsrd(i.OutputDoubleRegister(), i.InputOperand(1), 1);
      }
      break;
    case kSSEFloat64LoadLowWord32:
      if (instr->InputAt(0)->IsRegister()) {
        __ movd(i.OutputDoubleRegister(), i.InputRegister(0));
      } else {
        __ movd(i.OutputDoubleRegister(), i.InputOperand(0));
      }
      break;
    case kAVXFloat32Cmp: {
      CpuFeatureScope avx_scope(masm(), AVX);
      if (instr->InputAt(1)->IsDoubleRegister()) {
        __ vucomiss(i.InputDoubleRegister(0), i.InputDoubleRegister(1));
      } else {
        __ vucomiss(i.InputDoubleRegister(0), i.InputOperand(1));
      }
      break;
    }
    case kAVXFloat32Add:
      ASSEMBLE_AVX_BINOP(vaddss);
      break;
    case kAVXFloat32Sub:
      ASSEMBLE_AVX_BINOP(vsubss);
      break;
    case kAVXFloat32Mul:
      ASSEMBLE_AVX_BINOP(vmulss);
      break;
    case kAVXFloat32Div:
      ASSEMBLE_AVX_BINOP(vdivss);
      // Don't delete this mov. It may improve performance on some CPUs,
      // when there is a (v)mulss depending on the result.
      __ movaps(i.OutputDoubleRegister(), i.OutputDoubleRegister());
      break;
    case kAVXFloat32Max:
      ASSEMBLE_AVX_BINOP(vmaxss);
      break;
    case kAVXFloat32Min:
      ASSEMBLE_AVX_BINOP(vminss);
      break;
    case kAVXFloat64Cmp: {
      CpuFeatureScope avx_scope(masm(), AVX);
      if (instr->InputAt(1)->IsDoubleRegister()) {
        __ vucomisd(i.InputDoubleRegister(0), i.InputDoubleRegister(1));
      } else {
        __ vucomisd(i.InputDoubleRegister(0), i.InputOperand(1));
      }
      break;
    }
    case kAVXFloat64Add:
      ASSEMBLE_AVX_BINOP(vaddsd);
      break;
    case kAVXFloat64Sub:
      ASSEMBLE_AVX_BINOP(vsubsd);
      break;
    case kAVXFloat64Mul:
      ASSEMBLE_AVX_BINOP(vmulsd);
      break;
    case kAVXFloat64Div:
      ASSEMBLE_AVX_BINOP(vdivsd);
      // Don't delete this mov. It may improve performance on some CPUs,
      // when there is a (v)mulsd depending on the result.
      __ movaps(i.OutputDoubleRegister(), i.OutputDoubleRegister());
      break;
    case kAVXFloat64Max:
      ASSEMBLE_AVX_BINOP(vmaxsd);
      break;
    case kAVXFloat64Min:
      ASSEMBLE_AVX_BINOP(vminsd);
      break;
    case kAVXFloat32Abs: {
      // TODO(bmeurer): Use RIP relative 128-bit constants.
      __ pcmpeqd(kScratchDoubleReg, kScratchDoubleReg);
      __ psrlq(kScratchDoubleReg, 33);
      CpuFeatureScope avx_scope(masm(), AVX);
      if (instr->InputAt(0)->IsDoubleRegister()) {
        __ vandps(i.OutputDoubleRegister(), kScratchDoubleReg,
                  i.InputDoubleRegister(0));
      } else {
        __ vandps(i.OutputDoubleRegister(), kScratchDoubleReg,
                  i.InputOperand(0));
      }
      break;
    }
    case kAVXFloat32Neg: {
      // TODO(bmeurer): Use RIP relative 128-bit constants.
      __ pcmpeqd(kScratchDoubleReg, kScratchDoubleReg);
      __ psllq(kScratchDoubleReg, 31);
      CpuFeatureScope avx_scope(masm(), AVX);
      if (instr->InputAt(0)->IsDoubleRegister()) {
        __ vxorps(i.OutputDoubleRegister(), kScratchDoubleReg,
                  i.InputDoubleRegister(0));
      } else {
        __ vxorps(i.OutputDoubleRegister(), kScratchDoubleReg,
                  i.InputOperand(0));
      }
      break;
    }
    case kAVXFloat64Abs: {
      // TODO(bmeurer): Use RIP relative 128-bit constants.
      __ pcmpeqd(kScratchDoubleReg, kScratchDoubleReg);
      __ psrlq(kScratchDoubleReg, 1);
      CpuFeatureScope avx_scope(masm(), AVX);
      if (instr->InputAt(0)->IsDoubleRegister()) {
        __ vandpd(i.OutputDoubleRegister(), kScratchDoubleReg,
                  i.InputDoubleRegister(0));
      } else {
        __ vandpd(i.OutputDoubleRegister(), kScratchDoubleReg,
                  i.InputOperand(0));
      }
      break;
    }
    case kAVXFloat64Neg: {
      // TODO(bmeurer): Use RIP relative 128-bit constants.
      __ pcmpeqd(kScratchDoubleReg, kScratchDoubleReg);
      __ psllq(kScratchDoubleReg, 63);
      CpuFeatureScope avx_scope(masm(), AVX);
      if (instr->InputAt(0)->IsDoubleRegister()) {
        __ vxorpd(i.OutputDoubleRegister(), kScratchDoubleReg,
                  i.InputDoubleRegister(0));
      } else {
        __ vxorpd(i.OutputDoubleRegister(), kScratchDoubleReg,
                  i.InputOperand(0));
      }
      break;
    }
    case kX64Movsxbl:
      ASSEMBLE_MOVX(movsxbl);
      __ AssertZeroExtended(i.OutputRegister());
      break;
    case kX64Movzxbl:
      ASSEMBLE_MOVX(movzxbl);
      __ AssertZeroExtended(i.OutputRegister());
      break;
    case kX64Movb: {
      size_t index = 0;
      Operand operand = i.MemoryOperand(&index);
      if (HasImmediateInput(instr, index)) {
        __ movb(operand, Immediate(i.InputInt8(index)));
      } else {
        __ movb(operand, i.InputRegister(index));
      }
      break;
    }
    case kX64Movsxwl:
      ASSEMBLE_MOVX(movsxwl);
      __ AssertZeroExtended(i.OutputRegister());
      break;
    case kX64Movzxwl:
      ASSEMBLE_MOVX(movzxwl);
      __ AssertZeroExtended(i.OutputRegister());
      break;
    case kX64Movw: {
      size_t index = 0;
      Operand operand = i.MemoryOperand(&index);
      if (HasImmediateInput(instr, index)) {
        __ movw(operand, Immediate(i.InputInt16(index)));
      } else {
        __ movw(operand, i.InputRegister(index));
      }
      break;
    }
    case kX64Movl:
      if (instr->HasOutput()) {
        if (instr->addressing_mode() == kMode_None) {
          if (instr->InputAt(0)->IsRegister()) {
            __ movl(i.OutputRegister(), i.InputRegister(0));
          } else {
            __ movl(i.OutputRegister(), i.InputOperand(0));
          }
        } else {
          __ movl(i.OutputRegister(), i.MemoryOperand());
        }
        __ AssertZeroExtended(i.OutputRegister());
      } else {
        size_t index = 0;
        Operand operand = i.MemoryOperand(&index);
        if (HasImmediateInput(instr, index)) {
          __ movl(operand, i.InputImmediate(index));
        } else {
          __ movl(operand, i.InputRegister(index));
        }
      }
      break;
    case kX64Movsxlq:
      ASSEMBLE_MOVX(movsxlq);
      break;
    case kX64Movq:
      if (instr->HasOutput()) {
        __ movq(i.OutputRegister(), i.MemoryOperand());
      } else {
        size_t index = 0;
        Operand operand = i.MemoryOperand(&index);
        if (HasImmediateInput(instr, index)) {
          __ movq(operand, i.InputImmediate(index));
        } else {
          __ movq(operand, i.InputRegister(index));
        }
      }
      break;
    case kX64Movss:
      if (instr->HasOutput()) {
        __ movss(i.OutputDoubleRegister(), i.MemoryOperand());
      } else {
        size_t index = 0;
        Operand operand = i.MemoryOperand(&index);
        __ movss(operand, i.InputDoubleRegister(index));
      }
      break;
    case kX64Movsd:
      if (instr->HasOutput()) {
        __ movsd(i.OutputDoubleRegister(), i.MemoryOperand());
      } else {
        size_t index = 0;
        Operand operand = i.MemoryOperand(&index);
        __ movsd(operand, i.InputDoubleRegister(index));
      }
      break;
    case kX64Lea32: {
      AddressingMode mode = AddressingModeField::decode(instr->opcode());
      // Shorten "leal" to "addl", "subl" or "shll" if the register allocation
      // and addressing mode just happens to work out. The "addl"/"subl" forms
      // in these cases are faster based on measurements.
      if (i.InputRegister(0).is(i.OutputRegister())) {
        if (mode == kMode_MRI) {
          int32_t constant_summand = i.InputInt32(1);
          if (constant_summand > 0) {
            __ addl(i.OutputRegister(), Immediate(constant_summand));
          } else if (constant_summand < 0) {
            __ subl(i.OutputRegister(), Immediate(-constant_summand));
          }
        } else if (mode == kMode_MR1) {
          if (i.InputRegister(1).is(i.OutputRegister())) {
            __ shll(i.OutputRegister(), Immediate(1));
          } else {
            __ leal(i.OutputRegister(), i.MemoryOperand());
          }
        } else if (mode == kMode_M2) {
          __ shll(i.OutputRegister(), Immediate(1));
        } else if (mode == kMode_M4) {
          __ shll(i.OutputRegister(), Immediate(2));
        } else if (mode == kMode_M8) {
          __ shll(i.OutputRegister(), Immediate(3));
        } else {
          __ leal(i.OutputRegister(), i.MemoryOperand());
        }
      } else {
        __ leal(i.OutputRegister(), i.MemoryOperand());
      }
      __ AssertZeroExtended(i.OutputRegister());
      break;
    }
    case kX64Lea:
      __ leaq(i.OutputRegister(), i.MemoryOperand());
      break;
    case kX64Dec32:
      __ decl(i.OutputRegister());
      break;
    case kX64Inc32:
      __ incl(i.OutputRegister());
      break;
    case kX64Push:
      if (HasImmediateInput(instr, 0)) {
        __ pushq(i.InputImmediate(0));
      } else {
        if (instr->InputAt(0)->IsRegister()) {
          __ pushq(i.InputRegister(0));
        } else {
          __ pushq(i.InputOperand(0));
        }
      }
      break;
    case kX64Poke: {
      int const slot = MiscField::decode(instr->opcode());
      if (HasImmediateInput(instr, 0)) {
        __ movq(Operand(rsp, slot * kPointerSize), i.InputImmediate(0));
      } else {
        __ movq(Operand(rsp, slot * kPointerSize), i.InputRegister(0));
      }
      break;
    }
    case kX64StoreWriteBarrier: {
      Register object = i.InputRegister(0);
      Register value = i.InputRegister(2);
      SaveFPRegsMode mode =
          frame()->DidAllocateDoubleRegisters() ? kSaveFPRegs : kDontSaveFPRegs;
      if (HasImmediateInput(instr, 1)) {
        int index = i.InputInt32(1);
        Register scratch = i.TempRegister(1);
        __ movq(Operand(object, index), value);
        __ RecordWriteContextSlot(object, index, value, scratch, mode);
      } else {
        Register index = i.InputRegister(1);
        __ movq(Operand(object, index, times_1, 0), value);
        __ leaq(index, Operand(object, index, times_1, 0));
        __ RecordWrite(object, index, value, mode);
      }
      break;
    }
    case kCheckedLoadInt8:
      ASSEMBLE_CHECKED_LOAD_INTEGER(movsxbl);
      break;
    case kCheckedLoadUint8:
      ASSEMBLE_CHECKED_LOAD_INTEGER(movzxbl);
      break;
    case kCheckedLoadInt16:
      ASSEMBLE_CHECKED_LOAD_INTEGER(movsxwl);
      break;
    case kCheckedLoadUint16:
      ASSEMBLE_CHECKED_LOAD_INTEGER(movzxwl);
      break;
    case kCheckedLoadWord32:
      ASSEMBLE_CHECKED_LOAD_INTEGER(movl);
      break;
    case kCheckedLoadFloat32:
      ASSEMBLE_CHECKED_LOAD_FLOAT(movss);
      break;
    case kCheckedLoadFloat64:
      ASSEMBLE_CHECKED_LOAD_FLOAT(movsd);
      break;
    case kCheckedStoreWord8:
      ASSEMBLE_CHECKED_STORE_INTEGER(movb);
      break;
    case kCheckedStoreWord16:
      ASSEMBLE_CHECKED_STORE_INTEGER(movw);
      break;
    case kCheckedStoreWord32:
      ASSEMBLE_CHECKED_STORE_INTEGER(movl);
      break;
    case kCheckedStoreFloat32:
      ASSEMBLE_CHECKED_STORE_FLOAT(movss);
      break;
    case kCheckedStoreFloat64:
      ASSEMBLE_CHECKED_STORE_FLOAT(movsd);
      break;
    case kX64StackCheck:
      __ CompareRoot(rsp, Heap::kStackLimitRootIndex);
      break;
  }
}  // NOLINT(readability/fn_size)


// Assembles branches after this instruction.
void CodeGenerator::AssembleArchBranch(Instruction* instr, BranchInfo* branch) {
  X64OperandConverter i(this, instr);
  Label::Distance flabel_distance =
      branch->fallthru ? Label::kNear : Label::kFar;
  Label* tlabel = branch->true_label;
  Label* flabel = branch->false_label;
  switch (branch->condition) {
    case kUnorderedEqual:
      __ j(parity_even, flabel, flabel_distance);
    // Fall through.
    case kEqual:
      __ j(equal, tlabel);
      break;
    case kUnorderedNotEqual:
      __ j(parity_even, tlabel);
    // Fall through.
    case kNotEqual:
      __ j(not_equal, tlabel);
      break;
    case kSignedLessThan:
      __ j(less, tlabel);
      break;
    case kSignedGreaterThanOrEqual:
      __ j(greater_equal, tlabel);
      break;
    case kSignedLessThanOrEqual:
      __ j(less_equal, tlabel);
      break;
    case kSignedGreaterThan:
      __ j(greater, tlabel);
      break;
    case kUnsignedLessThan:
      __ j(below, tlabel);
      break;
    case kUnsignedGreaterThanOrEqual:
      __ j(above_equal, tlabel);
      break;
    case kUnsignedLessThanOrEqual:
      __ j(below_equal, tlabel);
      break;
    case kUnsignedGreaterThan:
      __ j(above, tlabel);
      break;
    case kOverflow:
      __ j(overflow, tlabel);
      break;
    case kNotOverflow:
      __ j(no_overflow, tlabel);
      break;
  }
  if (!branch->fallthru) __ jmp(flabel, flabel_distance);
}


void CodeGenerator::AssembleArchJump(RpoNumber target) {
  if (!IsNextInAssemblyOrder(target)) __ jmp(GetLabel(target));
}


// Assembles boolean materializations after this instruction.
void CodeGenerator::AssembleArchBoolean(Instruction* instr,
                                        FlagsCondition condition) {
  X64OperandConverter i(this, instr);
  Label done;

  // Materialize a full 64-bit 1 or 0 value. The result register is always the
  // last output of the instruction.
  Label check;
  DCHECK_NE(0u, instr->OutputCount());
  Register reg = i.OutputRegister(instr->OutputCount() - 1);
  Condition cc = no_condition;
  switch (condition) {
    case kUnorderedEqual:
      __ j(parity_odd, &check, Label::kNear);
      __ movl(reg, Immediate(0));
      __ jmp(&done, Label::kNear);
    // Fall through.
    case kEqual:
      cc = equal;
      break;
    case kUnorderedNotEqual:
      __ j(parity_odd, &check, Label::kNear);
      __ movl(reg, Immediate(1));
      __ jmp(&done, Label::kNear);
    // Fall through.
    case kNotEqual:
      cc = not_equal;
      break;
    case kSignedLessThan:
      cc = less;
      break;
    case kSignedGreaterThanOrEqual:
      cc = greater_equal;
      break;
    case kSignedLessThanOrEqual:
      cc = less_equal;
      break;
    case kSignedGreaterThan:
      cc = greater;
      break;
    case kUnsignedLessThan:
      cc = below;
      break;
    case kUnsignedGreaterThanOrEqual:
      cc = above_equal;
      break;
    case kUnsignedLessThanOrEqual:
      cc = below_equal;
      break;
    case kUnsignedGreaterThan:
      cc = above;
      break;
    case kOverflow:
      cc = overflow;
      break;
    case kNotOverflow:
      cc = no_overflow;
      break;
  }
  __ bind(&check);
  __ setcc(cc, reg);
  __ movzxbl(reg, reg);
  __ bind(&done);
}


void CodeGenerator::AssembleArchLookupSwitch(Instruction* instr) {
  X64OperandConverter i(this, instr);
  Register input = i.InputRegister(0);
  for (size_t index = 2; index < instr->InputCount(); index += 2) {
    __ cmpl(input, Immediate(i.InputInt32(index + 0)));
    __ j(equal, GetLabel(i.InputRpo(index + 1)));
  }
  AssembleArchJump(i.InputRpo(1));
}


void CodeGenerator::AssembleArchTableSwitch(Instruction* instr) {
  X64OperandConverter i(this, instr);
  Register input = i.InputRegister(0);
  int32_t const case_count = static_cast<int32_t>(instr->InputCount() - 2);
  Label** cases = zone()->NewArray<Label*>(case_count);
  for (int32_t index = 0; index < case_count; ++index) {
    cases[index] = GetLabel(i.InputRpo(index + 2));
  }
  Label* const table = AddJumpTable(cases, case_count);
  __ cmpl(input, Immediate(case_count));
  __ j(above_equal, GetLabel(i.InputRpo(1)));
  __ leaq(kScratchRegister, Operand(table));
  __ jmp(Operand(kScratchRegister, input, times_8, 0));
}


void CodeGenerator::AssembleDeoptimizerCall(
    int deoptimization_id, Deoptimizer::BailoutType bailout_type) {
  Address deopt_entry = Deoptimizer::GetDeoptimizationEntry(
      isolate(), deoptimization_id, bailout_type);
  __ call(deopt_entry, RelocInfo::RUNTIME_ENTRY);
}


void CodeGenerator::AssemblePrologue() {
  CallDescriptor* descriptor = linkage()->GetIncomingDescriptor();
  int stack_slots = frame()->GetSpillSlotCount();
  if (descriptor->kind() == CallDescriptor::kCallAddress) {
    __ pushq(rbp);
    __ movq(rbp, rsp);
    int register_save_area_size = 0;
    const RegList saves = descriptor->CalleeSavedRegisters();
    if (saves != 0) {  // Save callee-saved registers.
      for (int i = Register::kNumRegisters - 1; i >= 0; i--) {
        if (!((1 << i) & saves)) continue;
        __ pushq(Register::from_code(i));
        register_save_area_size += kPointerSize;
      }
    }
    const RegList saves_fp = descriptor->CalleeSavedFPRegisters();
    if (saves_fp != 0) {  // Save callee-saved XMM registers.
      const uint32_t saves_fp_count = base::bits::CountPopulation32(saves_fp);
      const int stack_size = saves_fp_count * 16;
      // Adjust the stack pointer.
      __ subp(rsp, Immediate(stack_size));
      // Store the registers on the stack.
      int slot_idx = 0;
      for (int i = 0; i < XMMRegister::kMaxNumRegisters; i++) {
        if (!((1 << i) & saves_fp)) continue;
        __ movdqu(Operand(rsp, 16 * slot_idx), XMMRegister::from_code(i));
        slot_idx++;
      }
      register_save_area_size += stack_size;
    }
    if (register_save_area_size > 0) {
      frame()->SetRegisterSaveAreaSize(register_save_area_size);
    }
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
    __ movq(rdi, Operand(rbp, JavaScriptFrameConstants::kFunctionOffset));
    DCHECK(stack_slots >= frame()->GetOsrStackSlotCount());
    stack_slots -= frame()->GetOsrStackSlotCount();
  }

  if (stack_slots > 0) {
    __ subq(rsp, Immediate(stack_slots * kPointerSize));
  }
}


void CodeGenerator::AssembleReturn() {
  CallDescriptor* descriptor = linkage()->GetIncomingDescriptor();
  int stack_slots = frame()->GetSpillSlotCount();
  if (descriptor->kind() == CallDescriptor::kCallAddress) {
    if (frame()->GetRegisterSaveAreaSize() > 0) {
      // Remove this frame's spill slots first.
      if (stack_slots > 0) {
        __ addq(rsp, Immediate(stack_slots * kPointerSize));
      }
      // Restore registers.
      const RegList saves_fp = descriptor->CalleeSavedFPRegisters();
      if (saves_fp != 0) {
        const uint32_t saves_fp_count = base::bits::CountPopulation32(saves_fp);
        const int stack_size = saves_fp_count * 16;
        // Load the registers from the stack.
        int slot_idx = 0;
        for (int i = 0; i < XMMRegister::kMaxNumRegisters; i++) {
          if (!((1 << i) & saves_fp)) continue;
          __ movdqu(XMMRegister::from_code(i), Operand(rsp, 16 * slot_idx));
          slot_idx++;
        }
        // Adjust the stack pointer.
        __ addp(rsp, Immediate(stack_size));
      }
      const RegList saves = descriptor->CalleeSavedRegisters();
      if (saves != 0) {
        for (int i = 0; i < Register::kNumRegisters; i++) {
          if (!((1 << i) & saves)) continue;
          __ popq(Register::from_code(i));
        }
      }
      __ popq(rbp);  // Pop caller's frame pointer.
      __ ret(0);
    } else {
      // No saved registers.
      __ movq(rsp, rbp);  // Move stack pointer back to frame pointer.
      __ popq(rbp);       // Pop caller's frame pointer.
      __ ret(0);
    }
  } else if (descriptor->IsJSFunctionCall() || needs_frame_) {
    // Canonicalize JSFunction return sites for now.
    if (return_label_.is_bound()) {
      __ jmp(&return_label_);
    } else {
      __ bind(&return_label_);
      __ movq(rsp, rbp);  // Move stack pointer back to frame pointer.
      __ popq(rbp);       // Pop caller's frame pointer.
      int pop_count = descriptor->IsJSFunctionCall()
                          ? static_cast<int>(descriptor->JSParameterCount())
                          : (info()->IsStub()
                                 ? info()->code_stub()->GetStackParameterCount()
                                 : 0);
      if (pop_count == 0) {
        __ Ret();
      } else {
        __ Ret(pop_count * kPointerSize, rbx);
      }
    }
  } else {
    __ Ret();
  }
}


void CodeGenerator::AssembleMove(InstructionOperand* source,
                                 InstructionOperand* destination) {
  X64OperandConverter g(this, NULL);
  // Dispatch on the source and destination operand kinds.  Not all
  // combinations are possible.
  if (source->IsRegister()) {
    DCHECK(destination->IsRegister() || destination->IsStackSlot());
    Register src = g.ToRegister(source);
    if (destination->IsRegister()) {
      __ movq(g.ToRegister(destination), src);
    } else {
      __ movq(g.ToOperand(destination), src);
    }
  } else if (source->IsStackSlot()) {
    DCHECK(destination->IsRegister() || destination->IsStackSlot());
    Operand src = g.ToOperand(source);
    if (destination->IsRegister()) {
      Register dst = g.ToRegister(destination);
      __ movq(dst, src);
    } else {
      // Spill on demand to use a temporary register for memory-to-memory
      // moves.
      Register tmp = kScratchRegister;
      Operand dst = g.ToOperand(destination);
      __ movq(tmp, src);
      __ movq(dst, tmp);
    }
  } else if (source->IsConstant()) {
    ConstantOperand* constant_source = ConstantOperand::cast(source);
    Constant src = g.ToConstant(constant_source);
    if (destination->IsRegister() || destination->IsStackSlot()) {
      Register dst = destination->IsRegister() ? g.ToRegister(destination)
                                               : kScratchRegister;
      switch (src.type()) {
        case Constant::kInt32:
          // TODO(dcarney): don't need scratch in this case.
          __ Set(dst, src.ToInt32());
          break;
        case Constant::kInt64:
          __ Set(dst, src.ToInt64());
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
          __ Move(dst, src.ToExternalReference());
          break;
        case Constant::kHeapObject: {
          Handle<HeapObject> src_object = src.ToHeapObject();
          Heap::RootListIndex index;
          int offset;
          if (IsMaterializableFromFrame(src_object, &offset)) {
            __ movp(dst, Operand(rbp, offset));
          } else if (IsMaterializableFromRoot(src_object, &index)) {
            __ LoadRoot(dst, index);
          } else {
            __ Move(dst, src_object);
          }
          break;
        }
        case Constant::kRpoNumber:
          UNREACHABLE();  // TODO(dcarney): load of labels on x64.
          break;
      }
      if (destination->IsStackSlot()) {
        __ movq(g.ToOperand(destination), kScratchRegister);
      }
    } else if (src.type() == Constant::kFloat32) {
      // TODO(turbofan): Can we do better here?
      uint32_t src_const = bit_cast<uint32_t>(src.ToFloat32());
      if (destination->IsDoubleRegister()) {
        __ Move(g.ToDoubleRegister(destination), src_const);
      } else {
        DCHECK(destination->IsDoubleStackSlot());
        Operand dst = g.ToOperand(destination);
        __ movl(dst, Immediate(src_const));
      }
    } else {
      DCHECK_EQ(Constant::kFloat64, src.type());
      uint64_t src_const = bit_cast<uint64_t>(src.ToFloat64());
      if (destination->IsDoubleRegister()) {
        __ Move(g.ToDoubleRegister(destination), src_const);
      } else {
        DCHECK(destination->IsDoubleStackSlot());
        __ movq(kScratchRegister, src_const);
        __ movq(g.ToOperand(destination), kScratchRegister);
      }
    }
  } else if (source->IsDoubleRegister()) {
    XMMRegister src = g.ToDoubleRegister(source);
    if (destination->IsDoubleRegister()) {
      XMMRegister dst = g.ToDoubleRegister(destination);
      __ movaps(dst, src);
    } else {
      DCHECK(destination->IsDoubleStackSlot());
      Operand dst = g.ToOperand(destination);
      __ movsd(dst, src);
    }
  } else if (source->IsDoubleStackSlot()) {
    DCHECK(destination->IsDoubleRegister() || destination->IsDoubleStackSlot());
    Operand src = g.ToOperand(source);
    if (destination->IsDoubleRegister()) {
      XMMRegister dst = g.ToDoubleRegister(destination);
      __ movsd(dst, src);
    } else {
      // We rely on having xmm0 available as a fixed scratch register.
      Operand dst = g.ToOperand(destination);
      __ movsd(xmm0, src);
      __ movsd(dst, xmm0);
    }
  } else {
    UNREACHABLE();
  }
}


void CodeGenerator::AssembleSwap(InstructionOperand* source,
                                 InstructionOperand* destination) {
  X64OperandConverter g(this, NULL);
  // Dispatch on the source and destination operand kinds.  Not all
  // combinations are possible.
  if (source->IsRegister() && destination->IsRegister()) {
    // Register-register.
    __ xchgq(g.ToRegister(source), g.ToRegister(destination));
  } else if (source->IsRegister() && destination->IsStackSlot()) {
    Register src = g.ToRegister(source);
    Operand dst = g.ToOperand(destination);
    __ xchgq(src, dst);
  } else if ((source->IsStackSlot() && destination->IsStackSlot()) ||
             (source->IsDoubleStackSlot() &&
              destination->IsDoubleStackSlot())) {
    // Memory-memory.
    Register tmp = kScratchRegister;
    Operand src = g.ToOperand(source);
    Operand dst = g.ToOperand(destination);
    __ movq(tmp, dst);
    __ xchgq(tmp, src);
    __ movq(dst, tmp);
  } else if (source->IsDoubleRegister() && destination->IsDoubleRegister()) {
    // XMM register-register swap. We rely on having xmm0
    // available as a fixed scratch register.
    XMMRegister src = g.ToDoubleRegister(source);
    XMMRegister dst = g.ToDoubleRegister(destination);
    __ movaps(xmm0, src);
    __ movaps(src, dst);
    __ movaps(dst, xmm0);
  } else if (source->IsDoubleRegister() && destination->IsDoubleStackSlot()) {
    // XMM register-memory swap.  We rely on having xmm0
    // available as a fixed scratch register.
    XMMRegister src = g.ToDoubleRegister(source);
    Operand dst = g.ToOperand(destination);
    __ movsd(xmm0, src);
    __ movsd(src, dst);
    __ movsd(dst, xmm0);
  } else {
    // No other combinations are possible.
    UNREACHABLE();
  }
}


void CodeGenerator::AssembleJumpTable(Label** targets, size_t target_count) {
  for (size_t index = 0; index < target_count; ++index) {
    __ dq(targets[index]);
  }
}


void CodeGenerator::AddNopForSmiCodeInlining() { __ nop(); }


void CodeGenerator::EnsureSpaceForLazyDeopt() {
  int space_needed = Deoptimizer::patch_size();
  if (!info()->IsStub()) {
    // Ensure that we have enough space after the previous lazy-bailout
    // instruction for patching the code here.
    int current_pc = masm()->pc_offset();
    if (current_pc < last_lazy_deopt_pc_ + space_needed) {
      int padding_size = last_lazy_deopt_pc_ + space_needed - current_pc;
      __ Nop(padding_size);
    }
  }
}

#undef __

}  // namespace internal
}  // namespace compiler
}  // namespace v8
