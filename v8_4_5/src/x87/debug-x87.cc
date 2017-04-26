// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/v8.h"

#if V8_TARGET_ARCH_X87

#include "src/codegen.h"
#include "src/debug.h"


namespace v8 {
namespace internal {

// Patch the code at the current PC with a call to the target address.
// Additional guard int3 instructions can be added if required.
void PatchCodeWithCall(Address pc, Address target, int guard_bytes) {
  // Call instruction takes up 5 bytes and int3 takes up one byte.
  static const int kCallCodeSize = 5;
  int code_size = kCallCodeSize + guard_bytes;

  // Create a code patcher.
  CodePatcher patcher(pc, code_size);

// Add a label for checking the size of the code used for returning.
#ifdef DEBUG
  Label check_codesize;
  patcher.masm()->bind(&check_codesize);
#endif

  // Patch the code.
  patcher.masm()->call(target, RelocInfo::NONE32);

  // Check that the size of the code generated is as expected.
  DCHECK_EQ(kCallCodeSize,
            patcher.masm()->SizeOfCodeGeneratedSince(&check_codesize));

  // Add the requested number of int3 instructions after the call.
  DCHECK_GE(guard_bytes, 0);
  for (int i = 0; i < guard_bytes; i++) {
    patcher.masm()->int3();
  }

  CpuFeatures::FlushICache(pc, code_size);
}


// Patch the JS frame exit code with a debug break call. See
// CodeGenerator::VisitReturnStatement and VirtualFrame::Exit in codegen-x87.cc
// for the precise return instructions sequence.
void BreakLocation::SetDebugBreakAtReturn() {
  DCHECK(Assembler::kJSReturnSequenceLength >=
         Assembler::kCallInstructionLength);
  PatchCodeWithCall(
      pc(), debug_info_->GetIsolate()->builtins()->Return_DebugBreak()->entry(),
      Assembler::kJSReturnSequenceLength - Assembler::kCallInstructionLength);
}


void BreakLocation::SetDebugBreakAtSlot() {
  DCHECK(IsDebugBreakSlot());
  Isolate* isolate = debug_info_->GetIsolate();
  PatchCodeWithCall(
      pc(), isolate->builtins()->Slot_DebugBreak()->entry(),
      Assembler::kDebugBreakSlotLength - Assembler::kCallInstructionLength);
}


#define __ ACCESS_MASM(masm)

static void Generate_DebugBreakCallHelper(MacroAssembler* masm,
                                          RegList object_regs,
                                          RegList non_object_regs,
                                          bool convert_call_to_jmp) {
  // Enter an internal frame.
  {
    FrameScope scope(masm, StackFrame::INTERNAL);

    // Load padding words on stack.
    for (int i = 0; i < LiveEdit::kFramePaddingInitialSize; i++) {
      __ push(Immediate(Smi::FromInt(LiveEdit::kFramePaddingValue)));
    }
    __ push(Immediate(Smi::FromInt(LiveEdit::kFramePaddingInitialSize)));

    // Store the registers containing live values on the expression stack to
    // make sure that these are correctly updated during GC. Non object values
    // are stored as a smi causing it to be untouched by GC.
    DCHECK((object_regs & ~kJSCallerSaved) == 0);
    DCHECK((non_object_regs & ~kJSCallerSaved) == 0);
    DCHECK((object_regs & non_object_regs) == 0);
    for (int i = 0; i < kNumJSCallerSaved; i++) {
      int r = JSCallerSavedCode(i);
      Register reg = { r };
      if ((object_regs & (1 << r)) != 0) {
        __ push(reg);
      }
      if ((non_object_regs & (1 << r)) != 0) {
        if (FLAG_debug_code) {
          __ test(reg, Immediate(0xc0000000));
          __ Assert(zero, kUnableToEncodeValueAsSmi);
        }
        __ SmiTag(reg);
        __ push(reg);
      }
    }

#ifdef DEBUG
    __ RecordComment("// Calling from debug break to runtime - come in - over");
#endif
    __ Move(eax, Immediate(0));  // No arguments.
    __ mov(ebx, Immediate(ExternalReference::debug_break(masm->isolate())));

    CEntryStub ceb(masm->isolate(), 1);
    __ CallStub(&ceb);

    // Automatically find register that could be used after register restore.
    // We need one register for padding skip instructions.
    Register unused_reg = { -1 };

    // Restore the register values containing object pointers from the
    // expression stack.
    for (int i = kNumJSCallerSaved; --i >= 0;) {
      int r = JSCallerSavedCode(i);
      Register reg = { r };
      if (FLAG_debug_code) {
        __ Move(reg, Immediate(kDebugZapValue));
      }
      bool taken = reg.code() == esi.code();
      if ((object_regs & (1 << r)) != 0) {
        __ pop(reg);
        taken = true;
      }
      if ((non_object_regs & (1 << r)) != 0) {
        __ pop(reg);
        __ SmiUntag(reg);
        taken = true;
      }
      if (!taken) {
        unused_reg = reg;
      }
    }

    DCHECK(unused_reg.code() != -1);

    // Read current padding counter and skip corresponding number of words.
    __ pop(unused_reg);
    // We divide stored value by 2 (untagging) and multiply it by word's size.
    STATIC_ASSERT(kSmiTagSize == 1 && kSmiShiftSize == 0);
    __ lea(esp, Operand(esp, unused_reg, times_half_pointer_size, 0));

    // Get rid of the internal frame.
  }

  // If this call did not replace a call but patched other code then there will
  // be an unwanted return address left on the stack. Here we get rid of that.
  if (convert_call_to_jmp) {
    __ add(esp, Immediate(kPointerSize));
  }

  // Now that the break point has been handled, resume normal execution by
  // jumping to the target address intended by the caller and that was
  // overwritten by the address of DebugBreakXXX.
  ExternalReference after_break_target =
      ExternalReference::debug_after_break_target_address(masm->isolate());
  __ jmp(Operand::StaticVariable(after_break_target));
}


void DebugCodegen::GenerateCallICStubDebugBreak(MacroAssembler* masm) {
  // Register state for CallICStub
  // ----------- S t a t e -------------
  //  -- edx    : type feedback slot (smi)
  //  -- edi    : function
  // -----------------------------------
  Generate_DebugBreakCallHelper(masm, edx.bit() | edi.bit(),
                                0, false);
}


void DebugCodegen::GenerateReturnDebugBreak(MacroAssembler* masm) {
  // Register state just before return from JS function (from codegen-x87.cc).
  // ----------- S t a t e -------------
  //  -- eax: return value
  // -----------------------------------
  Generate_DebugBreakCallHelper(masm, eax.bit(), 0, true);
}


void DebugCodegen::GenerateCallFunctionStubDebugBreak(MacroAssembler* masm) {
  // Register state for CallFunctionStub (from code-stubs-x87.cc).
  // ----------- S t a t e -------------
  //  -- edi: function
  // -----------------------------------
  Generate_DebugBreakCallHelper(masm, edi.bit(), 0, false);
}


void DebugCodegen::GenerateCallConstructStubDebugBreak(MacroAssembler* masm) {
  // Register state for CallConstructStub (from code-stubs-x87.cc).
  // eax is the actual number of arguments not encoded as a smi see comment
  // above IC call.
  // ----------- S t a t e -------------
  //  -- eax: number of arguments (not smi)
  //  -- edi: constructor function
  // -----------------------------------
  // The number of arguments in eax is not smi encoded.
  Generate_DebugBreakCallHelper(masm, edi.bit(), eax.bit(), false);
}


void DebugCodegen::GenerateCallConstructStubRecordDebugBreak(
    MacroAssembler* masm) {
  // Register state for CallConstructStub (from code-stubs-x87.cc).
  // eax is the actual number of arguments not encoded as a smi see comment
  // above IC call.
  // ----------- S t a t e -------------
  //  -- eax: number of arguments (not smi)
  //  -- ebx: feedback array
  //  -- edx: feedback slot (smi)
  //  -- edi: constructor function
  // -----------------------------------
  // The number of arguments in eax is not smi encoded.
  Generate_DebugBreakCallHelper(masm, ebx.bit() | edx.bit() | edi.bit(),
                                eax.bit(), false);
}


void DebugCodegen::GenerateSlot(MacroAssembler* masm) {
  // Generate enough nop's to make space for a call instruction.
  Label check_codesize;
  __ bind(&check_codesize);
  __ RecordDebugBreakSlot();
  __ Nop(Assembler::kDebugBreakSlotLength);
  DCHECK_EQ(Assembler::kDebugBreakSlotLength,
            masm->SizeOfCodeGeneratedSince(&check_codesize));
}


void DebugCodegen::GenerateSlotDebugBreak(MacroAssembler* masm) {
  Generate_DebugBreakCallHelper(masm, 0, 0, true);
}


void DebugCodegen::GeneratePlainReturnLiveEdit(MacroAssembler* masm) {
  masm->ret(0);
}


void DebugCodegen::GenerateFrameDropperLiveEdit(MacroAssembler* masm) {
  ExternalReference restarter_frame_function_slot =
      ExternalReference::debug_restarter_frame_function_pointer_address(
          masm->isolate());
  __ mov(Operand::StaticVariable(restarter_frame_function_slot), Immediate(0));

  // We do not know our frame height, but set esp based on ebp.
  __ lea(esp, Operand(ebp, -1 * kPointerSize));

  __ pop(edi);  // Function.
  __ pop(ebp);

  // Load context from the function.
  __ mov(esi, FieldOperand(edi, JSFunction::kContextOffset));

  // Get function code.
  __ mov(edx, FieldOperand(edi, JSFunction::kSharedFunctionInfoOffset));
  __ mov(edx, FieldOperand(edx, SharedFunctionInfo::kCodeOffset));
  __ lea(edx, FieldOperand(edx, Code::kHeaderSize));

  // Re-run JSFunction, edi is function, esi is context.
  __ jmp(edx);
}


const bool LiveEdit::kFrameDropperSupported = true;

#undef __

}  // namespace internal
}  // namespace v8

#endif  // V8_TARGET_ARCH_X87
