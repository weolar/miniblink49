// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_INTERPRETER_BYTECODE_ARRAY_ITERATOR_H_
#define V8_INTERPRETER_BYTECODE_ARRAY_ITERATOR_H_

#include "src/handles.h"
#include "src/interpreter/bytecodes.h"
#include "src/objects.h"

namespace v8 {
namespace internal {
namespace interpreter {

class BytecodeArrayIterator {
 public:
  explicit BytecodeArrayIterator(Handle<BytecodeArray> bytecode_array);

  void Advance();
  bool done() const;
  Bytecode current_bytecode() const;
  int current_bytecode_size() const;
  int current_offset() const { return bytecode_offset_; }
  OperandScale current_operand_scale() const { return operand_scale_; }
  int current_prefix_offset() const { return prefix_offset_; }
  const Handle<BytecodeArray>& bytecode_array() const {
    return bytecode_array_;
  }

  uint32_t GetFlagOperand(int operand_index) const;
  int32_t GetImmediateOperand(int operand_index) const;
  uint32_t GetIndexOperand(int operand_index) const;
  uint32_t GetRegisterCountOperand(int operand_index) const;
  Register GetRegisterOperand(int operand_index) const;
  int GetRegisterOperandRange(int operand_index) const;
  uint32_t GetRuntimeIdOperand(int operand_index) const;
  Handle<Object> GetConstantForIndexOperand(int operand_index) const;

  // Returns the absolute offset of the branch target at the current
  // bytecode. It is an error to call this method if the bytecode is
  // not for a jump or conditional jump.
  int GetJumpTargetOffset() const;

 private:
  uint32_t GetUnsignedOperand(int operand_index,
                              OperandType operand_type) const;
  int32_t GetSignedOperand(int operand_index, OperandType operand_type) const;

  void UpdateOperandScale();

  Handle<BytecodeArray> bytecode_array_;
  int bytecode_offset_;
  OperandScale operand_scale_;
  int prefix_offset_;

  DISALLOW_COPY_AND_ASSIGN(BytecodeArrayIterator);
};

}  // namespace interpreter
}  // namespace internal
}  // namespace v8

#endif  // V8_INTERPRETER_BYTECODE_GRAPH_ITERATOR_H_
