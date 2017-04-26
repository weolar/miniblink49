// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/interpreter/interpreter.h"

#include "src/code-factory.h"
#include "src/compiler.h"
#include "src/compiler/interpreter-assembler.h"
#include "src/factory.h"
#include "src/interpreter/bytecode-generator.h"
#include "src/interpreter/bytecodes.h"
#include "src/zone.h"

namespace v8 {
namespace internal {
namespace interpreter {

using compiler::Node;
#define __ assembler->


Interpreter::Interpreter(Isolate* isolate)
    : isolate_(isolate) {}


// static
Handle<FixedArray> Interpreter::CreateUninitializedInterpreterTable(
    Isolate* isolate) {
  Handle<FixedArray> handler_table = isolate->factory()->NewFixedArray(
      static_cast<int>(Bytecode::kLast) + 1, TENURED);
  // We rely on the interpreter handler table being immovable, so check that
  // it was allocated on the first page (which is always immovable).
  DCHECK(isolate->heap()->old_space()->FirstPage()->Contains(
      handler_table->address()));
  return handler_table;
}


void Interpreter::Initialize() {
  DCHECK(FLAG_ignition);
  Handle<FixedArray> handler_table = isolate_->factory()->interpreter_table();
  if (!IsInterpreterTableInitialized(handler_table)) {
    Zone zone;
    HandleScope scope(isolate_);

#define GENERATE_CODE(Name, ...)                                      \
    {                                                                 \
      compiler::InterpreterAssembler assembler(isolate_, &zone,       \
                                               Bytecode::k##Name);    \
      Do##Name(&assembler);                                           \
      Handle<Code> code = assembler.GenerateCode();                   \
      handler_table->set(static_cast<int>(Bytecode::k##Name), *code); \
    }
    BYTECODE_LIST(GENERATE_CODE)
#undef GENERATE_CODE
  }
}


bool Interpreter::MakeBytecode(CompilationInfo* info) {
  BytecodeGenerator generator(info->isolate(), info->zone());
  info->EnsureFeedbackVector();
  Handle<BytecodeArray> bytecodes = generator.MakeBytecode(info);
  if (FLAG_print_bytecode) {
    OFStream os(stdout);
    os << "Function: " << info->GetDebugName().get() << std::endl;
    bytecodes->Print(os);
    os << std::flush;
  }

  info->SetBytecodeArray(bytecodes);
  info->SetCode(info->isolate()->builtins()->InterpreterEntryTrampoline());
  return true;
}


bool Interpreter::IsInterpreterTableInitialized(
    Handle<FixedArray> handler_table) {
  DCHECK(handler_table->length() == static_cast<int>(Bytecode::kLast) + 1);
  return handler_table->get(0) != isolate_->heap()->undefined_value();
}


// LdaZero
//
// Load literal '0' into the accumulator.
void Interpreter::DoLdaZero(compiler::InterpreterAssembler* assembler) {
  Node* zero_value = __ NumberConstant(0.0);
  __ SetAccumulator(zero_value);
  __ Dispatch();
}


// LdaSmi8 <imm8>
//
// Load an 8-bit integer literal into the accumulator as a Smi.
void Interpreter::DoLdaSmi8(compiler::InterpreterAssembler* assembler) {
  Node* raw_int = __ BytecodeOperandImm(0);
  Node* smi_int = __ SmiTag(raw_int);
  __ SetAccumulator(smi_int);
  __ Dispatch();
}


void Interpreter::DoLoadConstant(compiler::InterpreterAssembler* assembler) {
  Node* index = __ BytecodeOperandIdx(0);
  Node* constant = __ LoadConstantPoolEntry(index);
  __ SetAccumulator(constant);
  __ Dispatch();
}


// LdaConstant <idx>
//
// Load constant literal at |idx| in the constant pool into the accumulator.
void Interpreter::DoLdaConstant(compiler::InterpreterAssembler* assembler) {
  DoLoadConstant(assembler);
}


// LdaConstantWide <idx>
//
// Load constant literal at |idx| in the constant pool into the accumulator.
void Interpreter::DoLdaConstantWide(compiler::InterpreterAssembler* assembler) {
  DoLoadConstant(assembler);
}


// LdaUndefined
//
// Load Undefined into the accumulator.
void Interpreter::DoLdaUndefined(compiler::InterpreterAssembler* assembler) {
  Node* undefined_value =
      __ HeapConstant(isolate_->factory()->undefined_value());
  __ SetAccumulator(undefined_value);
  __ Dispatch();
}


// LdaNull
//
// Load Null into the accumulator.
void Interpreter::DoLdaNull(compiler::InterpreterAssembler* assembler) {
  Node* null_value = __ HeapConstant(isolate_->factory()->null_value());
  __ SetAccumulator(null_value);
  __ Dispatch();
}


// LdaTheHole
//
// Load TheHole into the accumulator.
void Interpreter::DoLdaTheHole(compiler::InterpreterAssembler* assembler) {
  Node* the_hole_value = __ HeapConstant(isolate_->factory()->the_hole_value());
  __ SetAccumulator(the_hole_value);
  __ Dispatch();
}


// LdaTrue
//
// Load True into the accumulator.
void Interpreter::DoLdaTrue(compiler::InterpreterAssembler* assembler) {
  Node* true_value = __ HeapConstant(isolate_->factory()->true_value());
  __ SetAccumulator(true_value);
  __ Dispatch();
}


// LdaFalse
//
// Load False into the accumulator.
void Interpreter::DoLdaFalse(compiler::InterpreterAssembler* assembler) {
  Node* false_value = __ HeapConstant(isolate_->factory()->false_value());
  __ SetAccumulator(false_value);
  __ Dispatch();
}


// Ldar <src>
//
// Load accumulator with value from register <src>.
void Interpreter::DoLdar(compiler::InterpreterAssembler* assembler) {
  Node* reg_index = __ BytecodeOperandReg(0);
  Node* value = __ LoadRegister(reg_index);
  __ SetAccumulator(value);
  __ Dispatch();
}


// Star <dst>
//
// Store accumulator to register <dst>.
void Interpreter::DoStar(compiler::InterpreterAssembler* assembler) {
  Node* reg_index = __ BytecodeOperandReg(0);
  Node* accumulator = __ GetAccumulator();
  __ StoreRegister(accumulator, reg_index);
  __ Dispatch();
}


void Interpreter::DoLoadGlobal(Callable ic,
                               compiler::InterpreterAssembler* assembler) {
  // Get the global object.
  Node* context = __ GetContext();
  Node* global = __ LoadContextSlot(context, Context::GLOBAL_OBJECT_INDEX);

  // Load the global via the LoadIC.
  Node* code_target = __ HeapConstant(ic.code());
  Node* constant_index = __ BytecodeOperandIdx(0);
  Node* name = __ LoadConstantPoolEntry(constant_index);
  Node* raw_slot = __ BytecodeOperandIdx(1);
  Node* smi_slot = __ SmiTag(raw_slot);
  Node* type_feedback_vector = __ LoadTypeFeedbackVector();
  Node* result = __ CallIC(ic.descriptor(), code_target, global, name, smi_slot,
                           type_feedback_vector);
  __ SetAccumulator(result);

  __ Dispatch();
}


// LdaGlobalSloppy <name_index> <slot>
//
// Load the global with name in constant pool entry <name_index> into the
// accumulator using FeedBackVector slot <slot> in sloppy mode.
void Interpreter::DoLdaGlobalSloppy(compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, NOT_INSIDE_TYPEOF,
                                                   SLOPPY, UNINITIALIZED);
  DoLoadGlobal(ic, assembler);
}


// LdaGlobalSloppy <name_index> <slot>
//
// Load the global with name in constant pool entry <name_index> into the
// accumulator using FeedBackVector slot <slot> in strict mode.
void Interpreter::DoLdaGlobalStrict(compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, NOT_INSIDE_TYPEOF,
                                                   STRICT, UNINITIALIZED);
  DoLoadGlobal(ic, assembler);
}


// LdaGlobalInsideTypeofSloppy <name_index> <slot>
//
// Load the global with name in constant pool entry <name_index> into the
// accumulator using FeedBackVector slot <slot> in sloppy mode.
void Interpreter::DoLdaGlobalInsideTypeofSloppy(
    compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, INSIDE_TYPEOF,
                                                   SLOPPY, UNINITIALIZED);
  DoLoadGlobal(ic, assembler);
}


// LdaGlobalInsideTypeofStrict <name_index> <slot>
//
// Load the global with name in constant pool entry <name_index> into the
// accumulator using FeedBackVector slot <slot> in strict mode.
void Interpreter::DoLdaGlobalInsideTypeofStrict(
    compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, INSIDE_TYPEOF,
                                                   STRICT, UNINITIALIZED);
  DoLoadGlobal(ic, assembler);
}


// LdaGlobalSloppyWide <name_index> <slot>
//
// Load the global with name in constant pool entry <name_index> into the
// accumulator using FeedBackVector slot <slot> in sloppy mode.
void Interpreter::DoLdaGlobalSloppyWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, NOT_INSIDE_TYPEOF,
                                                   SLOPPY, UNINITIALIZED);
  DoLoadGlobal(ic, assembler);
}


// LdaGlobalSloppyWide <name_index> <slot>
//
// Load the global with name in constant pool entry <name_index> into the
// accumulator using FeedBackVector slot <slot> in strict mode.
void Interpreter::DoLdaGlobalStrictWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, NOT_INSIDE_TYPEOF,
                                                   STRICT, UNINITIALIZED);
  DoLoadGlobal(ic, assembler);
}


// LdaGlobalInsideTypeofSloppyWide <name_index> <slot>
//
// Load the global with name in constant pool entry <name_index> into the
// accumulator using FeedBackVector slot <slot> in sloppy mode.
void Interpreter::DoLdaGlobalInsideTypeofSloppyWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, INSIDE_TYPEOF,
                                                   SLOPPY, UNINITIALIZED);
  DoLoadGlobal(ic, assembler);
}


// LdaGlobalInsideTypeofSloppyWide <name_index> <slot>
//
// Load the global with name in constant pool entry <name_index> into the
// accumulator using FeedBackVector slot <slot> in strict mode.
void Interpreter::DoLdaGlobalInsideTypeofStrictWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, INSIDE_TYPEOF,
                                                   STRICT, UNINITIALIZED);
  DoLoadGlobal(ic, assembler);
}


void Interpreter::DoStoreGlobal(Callable ic,
                                compiler::InterpreterAssembler* assembler) {
  // Get the global object.
  Node* context = __ GetContext();
  Node* global = __ LoadContextSlot(context, Context::GLOBAL_OBJECT_INDEX);

  // Store the global via the StoreIC.
  Node* code_target = __ HeapConstant(ic.code());
  Node* constant_index = __ BytecodeOperandIdx(0);
  Node* name = __ LoadConstantPoolEntry(constant_index);
  Node* value = __ GetAccumulator();
  Node* raw_slot = __ BytecodeOperandIdx(1);
  Node* smi_slot = __ SmiTag(raw_slot);
  Node* type_feedback_vector = __ LoadTypeFeedbackVector();
  __ CallIC(ic.descriptor(), code_target, global, name, value, smi_slot,
            type_feedback_vector);

  __ Dispatch();
}


// StaGlobalSloppy <name_index> <slot>
//
// Store the value in the accumulator into the global with name in constant pool
// entry <name_index> using FeedBackVector slot <slot> in sloppy mode.
void Interpreter::DoStaGlobalSloppy(compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::StoreICInOptimizedCode(isolate_, SLOPPY, UNINITIALIZED);
  DoStoreGlobal(ic, assembler);
}


// StaGlobalStrict <name_index> <slot>
//
// Store the value in the accumulator into the global with name in constant pool
// entry <name_index> using FeedBackVector slot <slot> in strict mode.
void Interpreter::DoStaGlobalStrict(compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::StoreICInOptimizedCode(isolate_, STRICT, UNINITIALIZED);
  DoStoreGlobal(ic, assembler);
}


// StaGlobalSloppyWide <name_index> <slot>
//
// Store the value in the accumulator into the global with name in constant pool
// entry <name_index> using FeedBackVector slot <slot> in sloppy mode.
void Interpreter::DoStaGlobalSloppyWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::StoreICInOptimizedCode(isolate_, SLOPPY, UNINITIALIZED);
  DoStoreGlobal(ic, assembler);
}


// StaGlobalStrictWide <name_index> <slot>
//
// Store the value in the accumulator into the global with name in constant pool
// entry <name_index> using FeedBackVector slot <slot> in strict mode.
void Interpreter::DoStaGlobalStrictWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::StoreICInOptimizedCode(isolate_, STRICT, UNINITIALIZED);
  DoStoreGlobal(ic, assembler);
}


// LdaContextSlot <context> <slot_index>
//
// Load the object in |slot_index| of |context| into the accumulator.
void Interpreter::DoLdaContextSlot(compiler::InterpreterAssembler* assembler) {
  Node* reg_index = __ BytecodeOperandReg(0);
  Node* context = __ LoadRegister(reg_index);
  Node* slot_index = __ BytecodeOperandIdx(1);
  Node* result = __ LoadContextSlot(context, slot_index);
  __ SetAccumulator(result);
  __ Dispatch();
}


// StaContextSlot <context> <slot_index>
//
// Stores the object in the accumulator into |slot_index| of |context|.
void Interpreter::DoStaContextSlot(compiler::InterpreterAssembler* assembler) {
  Node* value = __ GetAccumulator();
  Node* reg_index = __ BytecodeOperandReg(0);
  Node* context = __ LoadRegister(reg_index);
  Node* slot_index = __ BytecodeOperandIdx(1);
  __ StoreContextSlot(context, slot_index, value);
  __ Dispatch();
}


void Interpreter::DoLoadIC(Callable ic,
                           compiler::InterpreterAssembler* assembler) {
  Node* code_target = __ HeapConstant(ic.code());
  Node* register_index = __ BytecodeOperandReg(0);
  Node* object = __ LoadRegister(register_index);
  Node* constant_index = __ BytecodeOperandIdx(1);
  Node* name = __ LoadConstantPoolEntry(constant_index);
  Node* raw_slot = __ BytecodeOperandIdx(2);
  Node* smi_slot = __ SmiTag(raw_slot);
  Node* type_feedback_vector = __ LoadTypeFeedbackVector();
  Node* result = __ CallIC(ic.descriptor(), code_target, object, name, smi_slot,
                           type_feedback_vector);
  __ SetAccumulator(result);
  __ Dispatch();
}


// LoadICSloppy <object> <name_index> <slot>
//
// Calls the sloppy mode LoadIC at FeedBackVector slot <slot> for <object> and
// the name at constant pool entry <name_index>.
void Interpreter::DoLoadICSloppy(compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, NOT_INSIDE_TYPEOF,
                                                   SLOPPY, UNINITIALIZED);
  DoLoadIC(ic, assembler);
}


// LoadICStrict <object> <name_index> <slot>
//
// Calls the sloppy mode LoadIC at FeedBackVector slot <slot> for <object> and
// the name at constant pool entry <name_index>.
void Interpreter::DoLoadICStrict(compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, NOT_INSIDE_TYPEOF,
                                                   STRICT, UNINITIALIZED);
  DoLoadIC(ic, assembler);
}


// LoadICSloppyWide <object> <name_index> <slot>
//
// Calls the sloppy mode LoadIC at FeedBackVector slot <slot> for <object> and
// the name at constant pool entry <name_index>.
void Interpreter::DoLoadICSloppyWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, NOT_INSIDE_TYPEOF,
                                                   SLOPPY, UNINITIALIZED);
  DoLoadIC(ic, assembler);
}


// LoadICStrictWide <object> <name_index> <slot>
//
// Calls the sloppy mode LoadIC at FeedBackVector slot <slot> for <object> and
// the name at constant pool entry <name_index>.
void Interpreter::DoLoadICStrictWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::LoadICInOptimizedCode(isolate_, NOT_INSIDE_TYPEOF,
                                                   STRICT, UNINITIALIZED);
  DoLoadIC(ic, assembler);
}


void Interpreter::DoKeyedLoadIC(Callable ic,
                                compiler::InterpreterAssembler* assembler) {
  Node* code_target = __ HeapConstant(ic.code());
  Node* reg_index = __ BytecodeOperandReg(0);
  Node* object = __ LoadRegister(reg_index);
  Node* name = __ GetAccumulator();
  Node* raw_slot = __ BytecodeOperandIdx(1);
  Node* smi_slot = __ SmiTag(raw_slot);
  Node* type_feedback_vector = __ LoadTypeFeedbackVector();
  Node* result = __ CallIC(ic.descriptor(), code_target, object, name, smi_slot,
                           type_feedback_vector);
  __ SetAccumulator(result);
  __ Dispatch();
}


// KeyedLoadICSloppy <object> <slot>
//
// Calls the sloppy mode KeyedLoadIC at FeedBackVector slot <slot> for <object>
// and the key in the accumulator.
void Interpreter::DoKeyedLoadICSloppy(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::KeyedLoadICInOptimizedCode(isolate_, SLOPPY, UNINITIALIZED);
  DoKeyedLoadIC(ic, assembler);
}


// KeyedLoadICStrict <object> <slot>
//
// Calls the strict mode KeyedLoadIC at FeedBackVector slot <slot> for <object>
// and the key in the accumulator.
void Interpreter::DoKeyedLoadICStrict(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::KeyedLoadICInOptimizedCode(isolate_, STRICT, UNINITIALIZED);
  DoKeyedLoadIC(ic, assembler);
}


// KeyedLoadICSloppyWide <object> <slot>
//
// Calls the sloppy mode KeyedLoadIC at FeedBackVector slot <slot> for <object>
// and the key in the accumulator.
void Interpreter::DoKeyedLoadICSloppyWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::KeyedLoadICInOptimizedCode(isolate_, SLOPPY, UNINITIALIZED);
  DoKeyedLoadIC(ic, assembler);
}


// KeyedLoadICStrictWide <object> <slot>
//
// Calls the strict mode KeyedLoadIC at FeedBackVector slot <slot> for <object>
// and the key in the accumulator.
void Interpreter::DoKeyedLoadICStrictWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::KeyedLoadICInOptimizedCode(isolate_, STRICT, UNINITIALIZED);
  DoKeyedLoadIC(ic, assembler);
}


void Interpreter::DoStoreIC(Callable ic,
                            compiler::InterpreterAssembler* assembler) {
  Node* code_target = __ HeapConstant(ic.code());
  Node* object_reg_index = __ BytecodeOperandReg(0);
  Node* object = __ LoadRegister(object_reg_index);
  Node* constant_index = __ BytecodeOperandIdx(1);
  Node* name = __ LoadConstantPoolEntry(constant_index);
  Node* value = __ GetAccumulator();
  Node* raw_slot = __ BytecodeOperandIdx(2);
  Node* smi_slot = __ SmiTag(raw_slot);
  Node* type_feedback_vector = __ LoadTypeFeedbackVector();
  __ CallIC(ic.descriptor(), code_target, object, name, value, smi_slot,
            type_feedback_vector);
  __ Dispatch();
}


// StoreICSloppy <object> <name_index> <slot>
//
// Calls the sloppy mode StoreIC at FeedBackVector slot <slot> for <object> and
// the name in constant pool entry <name_index> with the value in the
// accumulator.
void Interpreter::DoStoreICSloppy(compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::StoreICInOptimizedCode(isolate_, SLOPPY, UNINITIALIZED);
  DoStoreIC(ic, assembler);
}


// StoreICStrict <object> <name_index> <slot>
//
// Calls the strict mode StoreIC at FeedBackVector slot <slot> for <object> and
// the name in constant pool entry <name_index> with the value in the
// accumulator.
void Interpreter::DoStoreICStrict(compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::StoreICInOptimizedCode(isolate_, STRICT, UNINITIALIZED);
  DoStoreIC(ic, assembler);
}


// StoreICSloppyWide <object> <name_index> <slot>
//
// Calls the sloppy mode StoreIC at FeedBackVector slot <slot> for <object> and
// the name in constant pool entry <name_index> with the value in the
// accumulator.
void Interpreter::DoStoreICSloppyWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::StoreICInOptimizedCode(isolate_, SLOPPY, UNINITIALIZED);
  DoStoreIC(ic, assembler);
}


// StoreICStrictWide <object> <name_index> <slot>
//
// Calls the strict mode StoreIC at FeedBackVector slot <slot> for <object> and
// the name in constant pool entry <name_index> with the value in the
// accumulator.
void Interpreter::DoStoreICStrictWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::StoreICInOptimizedCode(isolate_, STRICT, UNINITIALIZED);
  DoStoreIC(ic, assembler);
}


void Interpreter::DoKeyedStoreIC(Callable ic,
                                 compiler::InterpreterAssembler* assembler) {
  Node* code_target = __ HeapConstant(ic.code());
  Node* object_reg_index = __ BytecodeOperandReg(0);
  Node* object = __ LoadRegister(object_reg_index);
  Node* name_reg_index = __ BytecodeOperandReg(1);
  Node* name = __ LoadRegister(name_reg_index);
  Node* value = __ GetAccumulator();
  Node* raw_slot = __ BytecodeOperandIdx(2);
  Node* smi_slot = __ SmiTag(raw_slot);
  Node* type_feedback_vector = __ LoadTypeFeedbackVector();
  __ CallIC(ic.descriptor(), code_target, object, name, value, smi_slot,
            type_feedback_vector);
  __ Dispatch();
}


// KeyedStoreICSloppy <object> <key> <slot>
//
// Calls the sloppy mode KeyStoreIC at FeedBackVector slot <slot> for <object>
// and the key <key> with the value in the accumulator.
void Interpreter::DoKeyedStoreICSloppy(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::KeyedStoreICInOptimizedCode(isolate_, SLOPPY, UNINITIALIZED);
  DoKeyedStoreIC(ic, assembler);
}


// KeyedStoreICStore <object> <key> <slot>
//
// Calls the strict mode KeyStoreIC at FeedBackVector slot <slot> for <object>
// and the key <key> with the value in the accumulator.
void Interpreter::DoKeyedStoreICStrict(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::KeyedStoreICInOptimizedCode(isolate_, STRICT, UNINITIALIZED);
  DoKeyedStoreIC(ic, assembler);
}


// KeyedStoreICSloppyWide <object> <key> <slot>
//
// Calls the sloppy mode KeyStoreIC at FeedBackVector slot <slot> for <object>
// and the key <key> with the value in the accumulator.
void Interpreter::DoKeyedStoreICSloppyWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::KeyedStoreICInOptimizedCode(isolate_, SLOPPY, UNINITIALIZED);
  DoKeyedStoreIC(ic, assembler);
}


// KeyedStoreICStoreWide <object> <key> <slot>
//
// Calls the strict mode KeyStoreIC at FeedBackVector slot <slot> for <object>
// and the key <key> with the value in the accumulator.
void Interpreter::DoKeyedStoreICStrictWide(
    compiler::InterpreterAssembler* assembler) {
  Callable ic =
      CodeFactory::KeyedStoreICInOptimizedCode(isolate_, STRICT, UNINITIALIZED);
  DoKeyedStoreIC(ic, assembler);
}


// PushContext <context>
//
// Pushes the accumulator as the current context, and saves it in <context>
void Interpreter::DoPushContext(compiler::InterpreterAssembler* assembler) {
  Node* reg_index = __ BytecodeOperandReg(0);
  Node* context = __ GetAccumulator();
  __ SetContext(context);
  __ StoreRegister(context, reg_index);
  __ Dispatch();
}


// PopContext <context>
//
// Pops the current context and sets <context> as the new context.
void Interpreter::DoPopContext(compiler::InterpreterAssembler* assembler) {
  Node* reg_index = __ BytecodeOperandReg(0);
  Node* context = __ LoadRegister(reg_index);
  __ SetContext(context);
  __ Dispatch();
}


void Interpreter::DoBinaryOp(Runtime::FunctionId function_id,
                             compiler::InterpreterAssembler* assembler) {
  // TODO(rmcilroy): Call ICs which back-patch bytecode with type specialized
  // operations, instead of calling builtins directly.
  Node* reg_index = __ BytecodeOperandReg(0);
  Node* lhs = __ LoadRegister(reg_index);
  Node* rhs = __ GetAccumulator();
  Node* result = __ CallRuntime(function_id, lhs, rhs);
  __ SetAccumulator(result);
  __ Dispatch();
}


// Add <src>
//
// Add register <src> to accumulator.
void Interpreter::DoAdd(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kAdd, assembler);
}


// Sub <src>
//
// Subtract register <src> from accumulator.
void Interpreter::DoSub(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kSubtract, assembler);
}


// Mul <src>
//
// Multiply accumulator by register <src>.
void Interpreter::DoMul(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kMultiply, assembler);
}


// Div <src>
//
// Divide register <src> by accumulator.
void Interpreter::DoDiv(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kDivide, assembler);
}


// Mod <src>
//
// Modulo register <src> by accumulator.
void Interpreter::DoMod(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kModulus, assembler);
}


// BitwiseOr <src>
//
// BitwiseOr register <src> to accumulator.
void Interpreter::DoBitwiseOr(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kBitwiseOr, assembler);
}


// BitwiseXor <src>
//
// BitwiseXor register <src> to accumulator.
void Interpreter::DoBitwiseXor(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kBitwiseXor, assembler);
}


// BitwiseAnd <src>
//
// BitwiseAnd register <src> to accumulator.
void Interpreter::DoBitwiseAnd(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kBitwiseAnd, assembler);
}


// ShiftLeft <src>
//
// Left shifts register <src> by the count specified in the accumulator.
// Register <src> is converted to an int32 and the accumulator to uint32
// before the operation. 5 lsb bits from the accumulator are used as count
// i.e. <src> << (accumulator & 0x1F).
void Interpreter::DoShiftLeft(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kShiftLeft, assembler);
}


// ShiftRight <src>
//
// Right shifts register <src> by the count specified in the accumulator.
// Result is sign extended. Register <src> is converted to an int32 and the
// accumulator to uint32 before the operation. 5 lsb bits from the accumulator
// are used as count i.e. <src> >> (accumulator & 0x1F).
void Interpreter::DoShiftRight(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kShiftRight, assembler);
}


// ShiftRightLogical <src>
//
// Right Shifts register <src> by the count specified in the accumulator.
// Result is zero-filled. The accumulator and register <src> are converted to
// uint32 before the operation 5 lsb bits from the accumulator are used as
// count i.e. <src> << (accumulator & 0x1F).
void Interpreter::DoShiftRightLogical(
    compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kShiftRightLogical, assembler);
}


void Interpreter::DoCountOp(Runtime::FunctionId function_id,
                            compiler::InterpreterAssembler* assembler) {
  Node* value = __ GetAccumulator();
  Node* one = __ NumberConstant(1);
  Node* result = __ CallRuntime(function_id, value, one);
  __ SetAccumulator(result);
  __ Dispatch();
}


// Inc
//
// Increments value in the accumulator by one.
void Interpreter::DoInc(compiler::InterpreterAssembler* assembler) {
  DoCountOp(Runtime::kAdd, assembler);
}


// Dec
//
// Decrements value in the accumulator by one.
void Interpreter::DoDec(compiler::InterpreterAssembler* assembler) {
  DoCountOp(Runtime::kSubtract, assembler);
}


// LogicalNot
//
// Perform logical-not on the accumulator, first casting the
// accumulator to a boolean value if required.
void Interpreter::DoLogicalNot(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* result = __ CallRuntime(Runtime::kInterpreterLogicalNot, accumulator);
  __ SetAccumulator(result);
  __ Dispatch();
}


// TypeOf
//
// Load the accumulator with the string representating type of the
// object in the accumulator.
void Interpreter::DoTypeOf(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* result = __ CallRuntime(Runtime::kInterpreterTypeOf, accumulator);
  __ SetAccumulator(result);
  __ Dispatch();
}


void Interpreter::DoDelete(Runtime::FunctionId function_id,
                           compiler::InterpreterAssembler* assembler) {
  Node* reg_index = __ BytecodeOperandReg(0);
  Node* object = __ LoadRegister(reg_index);
  Node* key = __ GetAccumulator();
  Node* result = __ CallRuntime(function_id, object, key);
  __ SetAccumulator(result);
  __ Dispatch();
}


// DeletePropertyStrict
//
// Delete the property specified in the accumulator from the object
// referenced by the register operand following strict mode semantics.
void Interpreter::DoDeletePropertyStrict(
    compiler::InterpreterAssembler* assembler) {
  DoDelete(Runtime::kDeleteProperty_Strict, assembler);
}


// DeletePropertySloppy
//
// Delete the property specified in the accumulator from the object
// referenced by the register operand following sloppy mode semantics.
void Interpreter::DoDeletePropertySloppy(
    compiler::InterpreterAssembler* assembler) {
  DoDelete(Runtime::kDeleteProperty_Sloppy, assembler);
}


// Call <callable> <receiver> <arg_count>
//
// Call a JSfunction or Callable in |callable| with the |receiver| and
// |arg_count| arguments in subsequent registers.
void Interpreter::DoCall(compiler::InterpreterAssembler* assembler) {
  Node* function_reg = __ BytecodeOperandReg(0);
  Node* function = __ LoadRegister(function_reg);
  Node* receiver_reg = __ BytecodeOperandReg(1);
  Node* first_arg = __ RegisterLocation(receiver_reg);
  Node* args_count = __ BytecodeOperandCount(2);
  Node* result = __ CallJS(function, first_arg, args_count);
  __ SetAccumulator(result);
  __ Dispatch();
}


// CallRuntime <function_id> <first_arg> <arg_count>
//
// Call the runtime function |function_id| with the first argument in
// register |first_arg| and |arg_count| arguments in subsequent
// registers.
void Interpreter::DoCallRuntime(compiler::InterpreterAssembler* assembler) {
  Node* function_id = __ BytecodeOperandIdx(0);
  Node* first_arg_reg = __ BytecodeOperandReg(1);
  Node* first_arg = __ RegisterLocation(first_arg_reg);
  Node* args_count = __ BytecodeOperandCount(2);
  Node* result = __ CallRuntime(function_id, first_arg, args_count);
  __ SetAccumulator(result);
  __ Dispatch();
}


// CallJSRuntime <context_index> <receiver> <arg_count>
//
// Call the JS runtime function that has the |context_index| with the receiver
// in register |receiver| and |arg_count| arguments in subsequent registers.
void Interpreter::DoCallJSRuntime(compiler::InterpreterAssembler* assembler) {
  Node* context_index = __ BytecodeOperandIdx(0);
  Node* receiver_reg = __ BytecodeOperandReg(1);
  Node* first_arg = __ RegisterLocation(receiver_reg);
  Node* args_count = __ BytecodeOperandCount(2);

  // Get the function to call from the native context.
  Node* context = __ GetContext();
  Node* global = __ LoadContextSlot(context, Context::GLOBAL_OBJECT_INDEX);
  Node* native_context =
      __ LoadObjectField(global, JSGlobalObject::kNativeContextOffset);
  Node* function = __ LoadContextSlot(native_context, context_index);

  // Call the function.
  Node* result = __ CallJS(function, first_arg, args_count);
  __ SetAccumulator(result);
  __ Dispatch();
}


// New <constructor> <arg_count>
//
// Call operator new with |constructor| and the first argument in
// register |first_arg| and |arg_count| arguments in subsequent
//
void Interpreter::DoNew(compiler::InterpreterAssembler* assembler) {
  Callable ic = CodeFactory::InterpreterPushArgsAndConstruct(isolate_);
  Node* constructor_reg = __ BytecodeOperandReg(0);
  Node* constructor = __ LoadRegister(constructor_reg);
  Node* first_arg_reg = __ BytecodeOperandReg(1);
  Node* first_arg = __ RegisterLocation(first_arg_reg);
  Node* args_count = __ BytecodeOperandCount(2);
  Node* result =
      __ CallConstruct(constructor, constructor, first_arg, args_count);
  __ SetAccumulator(result);
  __ Dispatch();
}


// TestEqual <src>
//
// Test if the value in the <src> register equals the accumulator.
void Interpreter::DoTestEqual(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kInterpreterEquals, assembler);
}


// TestNotEqual <src>
//
// Test if the value in the <src> register is not equal to the accumulator.
void Interpreter::DoTestNotEqual(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kInterpreterNotEquals, assembler);
}


// TestEqualStrict <src>
//
// Test if the value in the <src> register is strictly equal to the accumulator.
void Interpreter::DoTestEqualStrict(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kInterpreterStrictEquals, assembler);
}


// TestNotEqualStrict <src>
//
// Test if the value in the <src> register is not strictly equal to the
// accumulator.
void Interpreter::DoTestNotEqualStrict(
    compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kInterpreterStrictNotEquals, assembler);
}


// TestLessThan <src>
//
// Test if the value in the <src> register is less than the accumulator.
void Interpreter::DoTestLessThan(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kInterpreterLessThan, assembler);
}


// TestGreaterThan <src>
//
// Test if the value in the <src> register is greater than the accumulator.
void Interpreter::DoTestGreaterThan(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kInterpreterGreaterThan, assembler);
}


// TestLessThanOrEqual <src>
//
// Test if the value in the <src> register is less than or equal to the
// accumulator.
void Interpreter::DoTestLessThanOrEqual(
    compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kInterpreterLessThanOrEqual, assembler);
}


// TestGreaterThanOrEqual <src>
//
// Test if the value in the <src> register is greater than or equal to the
// accumulator.
void Interpreter::DoTestGreaterThanOrEqual(
    compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kInterpreterGreaterThanOrEqual, assembler);
}


// TestIn <src>
//
// Test if the object referenced by the register operand is a property of the
// object referenced by the accumulator.
void Interpreter::DoTestIn(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kHasProperty, assembler);
}


// TestInstanceOf <src>
//
// Test if the object referenced by the <src> register is an an instance of type
// referenced by the accumulator.
void Interpreter::DoTestInstanceOf(compiler::InterpreterAssembler* assembler) {
  DoBinaryOp(Runtime::kInstanceOf, assembler);
}


// ToBoolean
//
// Cast the object referenced by the accumulator to a boolean.
void Interpreter::DoToBoolean(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* result = __ CallRuntime(Runtime::kInterpreterToBoolean, accumulator);
  __ SetAccumulator(result);
  __ Dispatch();
}


// ToName
//
// Cast the object referenced by the accumulator to a name.
void Interpreter::DoToName(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* result = __ CallRuntime(Runtime::kToName, accumulator);
  __ SetAccumulator(result);
  __ Dispatch();
}


// ToNumber
//
// Cast the object referenced by the accumulator to a number.
void Interpreter::DoToNumber(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* result = __ CallRuntime(Runtime::kToNumber, accumulator);
  __ SetAccumulator(result);
  __ Dispatch();
}


// ToObject
//
// Cast the object referenced by the accumulator to a JSObject.
void Interpreter::DoToObject(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* result = __ CallRuntime(Runtime::kToObject, accumulator);
  __ SetAccumulator(result);
  __ Dispatch();
}


// Jump <imm8>
//
// Jump by number of bytes represented by the immediate operand |imm8|.
void Interpreter::DoJump(compiler::InterpreterAssembler* assembler) {
  Node* relative_jump = __ BytecodeOperandImm(0);
  __ Jump(relative_jump);
}


// JumpConstant <idx>
//
// Jump by number of bytes in the Smi in the |idx| entry in the constant pool.
void Interpreter::DoJumpConstant(compiler::InterpreterAssembler* assembler) {
  Node* index = __ BytecodeOperandIdx(0);
  Node* constant = __ LoadConstantPoolEntry(index);
  Node* relative_jump = __ SmiUntag(constant);
  __ Jump(relative_jump);
}


// JumpIfTrue <imm8>
//
// Jump by number of bytes represented by an immediate operand if the
// accumulator contains true.
void Interpreter::DoJumpIfTrue(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* relative_jump = __ BytecodeOperandImm(0);
  Node* true_value = __ BooleanConstant(true);
  __ JumpIfWordEqual(accumulator, true_value, relative_jump);
}


// JumpIfTrueConstant <idx>
//
// Jump by number of bytes in the Smi in the |idx| entry in the constant pool
// if the accumulator contains true.
void Interpreter::DoJumpIfTrueConstant(
    compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* index = __ BytecodeOperandIdx(0);
  Node* constant = __ LoadConstantPoolEntry(index);
  Node* relative_jump = __ SmiUntag(constant);
  Node* true_value = __ BooleanConstant(true);
  __ JumpIfWordEqual(accumulator, true_value, relative_jump);
}


// JumpIfFalse <imm8>
//
// Jump by number of bytes represented by an immediate operand if the
// accumulator contains false.
void Interpreter::DoJumpIfFalse(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* relative_jump = __ BytecodeOperandImm(0);
  Node* false_value = __ BooleanConstant(false);
  __ JumpIfWordEqual(accumulator, false_value, relative_jump);
}


// JumpIfFalseConstant <idx>
//
// Jump by number of bytes in the Smi in the |idx| entry in the constant pool
// if the accumulator contains false.
void Interpreter::DoJumpIfFalseConstant(
    compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* index = __ BytecodeOperandIdx(0);
  Node* constant = __ LoadConstantPoolEntry(index);
  Node* relative_jump = __ SmiUntag(constant);
  Node* false_value = __ BooleanConstant(false);
  __ JumpIfWordEqual(accumulator, false_value, relative_jump);
}


// JumpIfToBooleanTrue <imm8>
//
// Jump by number of bytes represented by an immediate operand if the object
// referenced by the accumulator is true when the object is cast to boolean.
void Interpreter::DoJumpIfToBooleanTrue(
    compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* relative_jump = __ BytecodeOperandImm(0);
  Node* to_boolean_value =
      __ CallRuntime(Runtime::kInterpreterToBoolean, accumulator);
  Node* true_value = __ BooleanConstant(true);
  __ JumpIfWordEqual(to_boolean_value, true_value, relative_jump);
}


// JumpIfToBooleanTrueConstant <idx>
//
// Jump by number of bytes in the Smi in the |idx| entry in the constant pool
// if the object referenced by the accumulator is true when the object is cast
// to boolean.
void Interpreter::DoJumpIfToBooleanTrueConstant(
    compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* to_boolean_value =
      __ CallRuntime(Runtime::kInterpreterToBoolean, accumulator);
  Node* index = __ BytecodeOperandIdx(0);
  Node* constant = __ LoadConstantPoolEntry(index);
  Node* relative_jump = __ SmiUntag(constant);
  Node* true_value = __ BooleanConstant(true);
  __ JumpIfWordEqual(to_boolean_value, true_value, relative_jump);
}


// JumpIfToBooleanFalse <imm8>
//
// Jump by number of bytes represented by an immediate operand if the object
// referenced by the accumulator is false when the object is cast to boolean.
void Interpreter::DoJumpIfToBooleanFalse(
    compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* relative_jump = __ BytecodeOperandImm(0);
  Node* to_boolean_value =
      __ CallRuntime(Runtime::kInterpreterToBoolean, accumulator);
  Node* false_value = __ BooleanConstant(false);
  __ JumpIfWordEqual(to_boolean_value, false_value, relative_jump);
}


// JumpIfToBooleanFalseConstant <idx>
//
// Jump by number of bytes in the Smi in the |idx| entry in the constant pool
// if the object referenced by the accumulator is false when the object is cast
// to boolean.
void Interpreter::DoJumpIfToBooleanFalseConstant(
    compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* to_boolean_value =
      __ CallRuntime(Runtime::kInterpreterToBoolean, accumulator);
  Node* index = __ BytecodeOperandIdx(0);
  Node* constant = __ LoadConstantPoolEntry(index);
  Node* relative_jump = __ SmiUntag(constant);
  Node* false_value = __ BooleanConstant(false);
  __ JumpIfWordEqual(to_boolean_value, false_value, relative_jump);
}


// JumpIfNull <imm8>
//
// Jump by number of bytes represented by an immediate operand if the object
// referenced by the accumulator is the null constant.
void Interpreter::DoJumpIfNull(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* null_value = __ HeapConstant(isolate_->factory()->null_value());
  Node* relative_jump = __ BytecodeOperandImm(0);
  __ JumpIfWordEqual(accumulator, null_value, relative_jump);
}


// JumpIfNullConstant <idx>
//
// Jump by number of bytes in the Smi in the |idx| entry in the constant pool
// if the object referenced by the accumulator is the null constant.
void Interpreter::DoJumpIfNullConstant(
    compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* null_value = __ HeapConstant(isolate_->factory()->null_value());
  Node* index = __ BytecodeOperandIdx(0);
  Node* constant = __ LoadConstantPoolEntry(index);
  Node* relative_jump = __ SmiUntag(constant);
  __ JumpIfWordEqual(accumulator, null_value, relative_jump);
}


// JumpIfUndefined <imm8>
//
// Jump by number of bytes represented by an immediate operand if the object
// referenced by the accumulator is the undefined constant.
void Interpreter::DoJumpIfUndefined(compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* undefined_value =
      __ HeapConstant(isolate_->factory()->undefined_value());
  Node* relative_jump = __ BytecodeOperandImm(0);
  __ JumpIfWordEqual(accumulator, undefined_value, relative_jump);
}


// JumpIfUndefinedConstant <idx>
//
// Jump by number of bytes in the Smi in the |idx| entry in the constant pool
// if the object referenced by the accumulator is the undefined constant.
void Interpreter::DoJumpIfUndefinedConstant(
    compiler::InterpreterAssembler* assembler) {
  Node* accumulator = __ GetAccumulator();
  Node* undefined_value =
      __ HeapConstant(isolate_->factory()->undefined_value());
  Node* index = __ BytecodeOperandIdx(0);
  Node* constant = __ LoadConstantPoolEntry(index);
  Node* relative_jump = __ SmiUntag(constant);
  __ JumpIfWordEqual(accumulator, undefined_value, relative_jump);
}


// CreateRegExpLiteral <idx> <flags_reg>
//
// Creates a regular expression literal for literal index <idx> with flags held
// in <flags_reg> and the pattern in the accumulator.
void Interpreter::DoCreateRegExpLiteral(
    compiler::InterpreterAssembler* assembler) {
  Node* pattern = __ GetAccumulator();
  Node* literal_index_raw = __ BytecodeOperandIdx(0);
  Node* literal_index = __ SmiTag(literal_index_raw);
  Node* flags_reg = __ BytecodeOperandReg(1);
  Node* flags = __ LoadRegister(flags_reg);
  Node* closure = __ LoadRegister(Register::function_closure());
  Node* literals_array =
      __ LoadObjectField(closure, JSFunction::kLiteralsOffset);
  Node* result = __ CallRuntime(Runtime::kMaterializeRegExpLiteral,
                                literals_array, literal_index, pattern, flags);
  __ SetAccumulator(result);
  __ Dispatch();
}


void Interpreter::DoCreateLiteral(Runtime::FunctionId function_id,
                                  compiler::InterpreterAssembler* assembler) {
  Node* constant_elements = __ GetAccumulator();
  Node* literal_index_raw = __ BytecodeOperandIdx(0);
  Node* literal_index = __ SmiTag(literal_index_raw);
  Node* flags_raw = __ BytecodeOperandImm(1);
  Node* flags = __ SmiTag(flags_raw);
  Node* closure = __ LoadRegister(Register::function_closure());
  Node* literals_array =
      __ LoadObjectField(closure, JSFunction::kLiteralsOffset);
  Node* result = __ CallRuntime(function_id, literals_array, literal_index,
                                constant_elements, flags);
  __ SetAccumulator(result);
  __ Dispatch();
}


// CreateArrayLiteral <idx> <flags>
//
// Creates an array literal for literal index <idx> with flags <flags> and
// constant elements in the accumulator.
void Interpreter::DoCreateArrayLiteral(
    compiler::InterpreterAssembler* assembler) {
  DoCreateLiteral(Runtime::kCreateArrayLiteral, assembler);
}


// CreateObjectLiteral <idx> <flags>
//
// Creates an object literal for literal index <idx> with flags <flags> and
// constant elements in the accumulator.
void Interpreter::DoCreateObjectLiteral(
    compiler::InterpreterAssembler* assembler) {
  DoCreateLiteral(Runtime::kCreateObjectLiteral, assembler);
}


// CreateClosure <tenured>
//
// Creates a new closure for SharedFunctionInfo in the accumulator with the
// PretenureFlag <tenured>.
void Interpreter::DoCreateClosure(compiler::InterpreterAssembler* assembler) {
  // TODO(rmcilroy): Possibly call FastNewClosureStub when possible instead of
  // calling into the runtime.
  Node* shared = __ GetAccumulator();
  Node* tenured_raw = __ BytecodeOperandImm(0);
  Node* tenured = __ SmiTag(tenured_raw);
  Node* result =
      __ CallRuntime(Runtime::kInterpreterNewClosure, shared, tenured);
  __ SetAccumulator(result);
  __ Dispatch();
}


// CreateMappedArguments
//
// Creates a new mapped arguments object.
void Interpreter::DoCreateMappedArguments(
    compiler::InterpreterAssembler* assembler) {
  Node* closure = __ LoadRegister(Register::function_closure());
  Node* result = __ CallRuntime(Runtime::kNewSloppyArguments_Generic, closure);
  __ SetAccumulator(result);
  __ Dispatch();
}


// CreateUnmappedArguments
//
// Creates a new unmapped arguments object.
void Interpreter::DoCreateUnmappedArguments(
    compiler::InterpreterAssembler* assembler) {
  Node* closure = __ LoadRegister(Register::function_closure());
  Node* result = __ CallRuntime(Runtime::kNewStrictArguments_Generic, closure);
  __ SetAccumulator(result);
  __ Dispatch();
}


// Throw
//
// Throws the exception in the accumulator.
void Interpreter::DoThrow(compiler::InterpreterAssembler* assembler) {
  Node* exception = __ GetAccumulator();
  __ CallRuntime(Runtime::kThrow, exception);
  // We shouldn't ever return from a throw.
  __ Abort(kUnexpectedReturnFromThrow);
}


// Return
//
// Return the value in the accumulator.
void Interpreter::DoReturn(compiler::InterpreterAssembler* assembler) {
  __ Return();
}


// ForInPrepare <receiver>
//
// Returns state for for..in loop execution based on the |receiver| and
// the property names in the accumulator.
void Interpreter::DoForInPrepare(compiler::InterpreterAssembler* assembler) {
  Node* receiver_reg = __ BytecodeOperandReg(0);
  Node* receiver = __ LoadRegister(receiver_reg);
  Node* property_names = __ GetAccumulator();
  Node* result = __ CallRuntime(Runtime::kInterpreterForInPrepare, receiver,
                                property_names);
  __ SetAccumulator(result);
  __ Dispatch();
}


// ForInNext <for_in_state> <index>
//
// Returns the next key in a for..in loop. The state associated with the
// iteration is contained in |for_in_state| and |index| is the current
// zero-based iteration count.
void Interpreter::DoForInNext(compiler::InterpreterAssembler* assembler) {
  Node* for_in_state_reg = __ BytecodeOperandReg(0);
  Node* for_in_state = __ LoadRegister(for_in_state_reg);
  Node* receiver = __ LoadFixedArrayElement(for_in_state, 0);
  Node* cache_array = __ LoadFixedArrayElement(for_in_state, 1);
  Node* cache_type = __ LoadFixedArrayElement(for_in_state, 2);
  Node* index_reg = __ BytecodeOperandReg(1);
  Node* index = __ LoadRegister(index_reg);
  Node* result = __ CallRuntime(Runtime::kForInNext, receiver, cache_array,
                                cache_type, index);
  __ SetAccumulator(result);
  __ Dispatch();
}


// ForInDone <for_in_state>
//
// Returns the next key in a for..in loop. The accumulator contains the current
// zero-based iteration count and |for_in_state| is the state returned by an
// earlier invocation of ForInPrepare.
void Interpreter::DoForInDone(compiler::InterpreterAssembler* assembler) {
  Node* index = __ GetAccumulator();
  Node* for_in_state_reg = __ BytecodeOperandReg(0);
  Node* for_in_state = __ LoadRegister(for_in_state_reg);
  Node* cache_length = __ LoadFixedArrayElement(for_in_state, 3);
  Node* result = __ CallRuntime(Runtime::kForInDone, index, cache_length);
  __ SetAccumulator(result);
  __ Dispatch();
}


}  // namespace interpreter
}  // namespace internal
}  // namespace v8
