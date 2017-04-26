// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test/unittests/compiler/instruction-selector-unittest.h"

#include "src/compiler/node-matchers.h"

namespace v8 {
namespace internal {
namespace compiler {

// -----------------------------------------------------------------------------
// Conversions.


TEST_F(InstructionSelectorTest, ChangeFloat32ToFloat64WithParameter) {
  StreamBuilder m(this, kMachFloat32, kMachFloat64);
  m.Return(m.ChangeFloat32ToFloat64(m.Parameter(0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kSSEFloat32ToFloat64, s[0]->arch_opcode());
  EXPECT_EQ(1U, s[0]->InputCount());
  EXPECT_EQ(1U, s[0]->OutputCount());
}


TEST_F(InstructionSelectorTest, ChangeInt32ToInt64WithParameter) {
  StreamBuilder m(this, kMachInt64, kMachInt32);
  m.Return(m.ChangeInt32ToInt64(m.Parameter(0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Movsxlq, s[0]->arch_opcode());
}


TEST_F(InstructionSelectorTest, ChangeUint32ToFloat64WithParameter) {
  StreamBuilder m(this, kMachFloat64, kMachUint32);
  m.Return(m.ChangeUint32ToFloat64(m.Parameter(0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kSSEUint32ToFloat64, s[0]->arch_opcode());
}


TEST_F(InstructionSelectorTest, ChangeUint32ToUint64WithParameter) {
  StreamBuilder m(this, kMachUint64, kMachUint32);
  m.Return(m.ChangeUint32ToUint64(m.Parameter(0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Movl, s[0]->arch_opcode());
}


TEST_F(InstructionSelectorTest, TruncateFloat64ToFloat32WithParameter) {
  StreamBuilder m(this, kMachFloat64, kMachFloat32);
  m.Return(m.TruncateFloat64ToFloat32(m.Parameter(0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kSSEFloat64ToFloat32, s[0]->arch_opcode());
  EXPECT_EQ(1U, s[0]->InputCount());
  EXPECT_EQ(1U, s[0]->OutputCount());
}


TEST_F(InstructionSelectorTest, TruncateInt64ToInt32WithParameter) {
  StreamBuilder m(this, kMachInt32, kMachInt64);
  m.Return(m.TruncateInt64ToInt32(m.Parameter(0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Movl, s[0]->arch_opcode());
}


// -----------------------------------------------------------------------------
// Loads and stores


namespace {

struct MemoryAccess {
  MachineType type;
  ArchOpcode load_opcode;
  ArchOpcode store_opcode;
};


std::ostream& operator<<(std::ostream& os, const MemoryAccess& memacc) {
  return os << memacc.type;
}


static const MemoryAccess kMemoryAccesses[] = {
    {kMachInt8, kX64Movsxbl, kX64Movb},
    {kMachUint8, kX64Movzxbl, kX64Movb},
    {kMachInt16, kX64Movsxwl, kX64Movw},
    {kMachUint16, kX64Movzxwl, kX64Movw},
    {kMachInt32, kX64Movl, kX64Movl},
    {kMachUint32, kX64Movl, kX64Movl},
    {kMachInt64, kX64Movq, kX64Movq},
    {kMachUint64, kX64Movq, kX64Movq},
    {kMachFloat32, kX64Movss, kX64Movss},
    {kMachFloat64, kX64Movsd, kX64Movsd}};

}  // namespace


typedef InstructionSelectorTestWithParam<MemoryAccess>
    InstructionSelectorMemoryAccessTest;


TEST_P(InstructionSelectorMemoryAccessTest, LoadWithParameters) {
  const MemoryAccess memacc = GetParam();
  StreamBuilder m(this, memacc.type, kMachPtr, kMachInt32);
  m.Return(m.Load(memacc.type, m.Parameter(0), m.Parameter(1)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(memacc.load_opcode, s[0]->arch_opcode());
  EXPECT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(1U, s[0]->OutputCount());
}


TEST_P(InstructionSelectorMemoryAccessTest, StoreWithParameters) {
  const MemoryAccess memacc = GetParam();
  StreamBuilder m(this, kMachInt32, kMachPtr, kMachInt32, memacc.type);
  m.Store(memacc.type, m.Parameter(0), m.Parameter(1), m.Parameter(2));
  m.Return(m.Int32Constant(0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(memacc.store_opcode, s[0]->arch_opcode());
  EXPECT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(0U, s[0]->OutputCount());
}


INSTANTIATE_TEST_CASE_P(InstructionSelectorTest,
                        InstructionSelectorMemoryAccessTest,
                        ::testing::ValuesIn(kMemoryAccesses));


// -----------------------------------------------------------------------------
// ChangeUint32ToUint64.


namespace {

typedef Node* (RawMachineAssembler::*Constructor)(Node*, Node*);


struct BinaryOperation {
  Constructor constructor;
  const char* constructor_name;
};


std::ostream& operator<<(std::ostream& os, const BinaryOperation& bop) {
  return os << bop.constructor_name;
}


const BinaryOperation kWord32BinaryOperations[] = {
    {&RawMachineAssembler::Word32And, "Word32And"},
    {&RawMachineAssembler::Word32Or, "Word32Or"},
    {&RawMachineAssembler::Word32Xor, "Word32Xor"},
    {&RawMachineAssembler::Word32Shl, "Word32Shl"},
    {&RawMachineAssembler::Word32Shr, "Word32Shr"},
    {&RawMachineAssembler::Word32Sar, "Word32Sar"},
    {&RawMachineAssembler::Word32Ror, "Word32Ror"},
    {&RawMachineAssembler::Word32Equal, "Word32Equal"},
    {&RawMachineAssembler::Int32Add, "Int32Add"},
    {&RawMachineAssembler::Int32Sub, "Int32Sub"},
    {&RawMachineAssembler::Int32Mul, "Int32Mul"},
    {&RawMachineAssembler::Int32MulHigh, "Int32MulHigh"},
    {&RawMachineAssembler::Int32Div, "Int32Div"},
    {&RawMachineAssembler::Int32LessThan, "Int32LessThan"},
    {&RawMachineAssembler::Int32LessThanOrEqual, "Int32LessThanOrEqual"},
    {&RawMachineAssembler::Int32Mod, "Int32Mod"},
    {&RawMachineAssembler::Uint32Div, "Uint32Div"},
    {&RawMachineAssembler::Uint32LessThan, "Uint32LessThan"},
    {&RawMachineAssembler::Uint32LessThanOrEqual, "Uint32LessThanOrEqual"},
    {&RawMachineAssembler::Uint32Mod, "Uint32Mod"}};

}  // namespace


typedef InstructionSelectorTestWithParam<BinaryOperation>
    InstructionSelectorChangeUint32ToUint64Test;


TEST_P(InstructionSelectorChangeUint32ToUint64Test, ChangeUint32ToUint64) {
  const BinaryOperation& bop = GetParam();
  StreamBuilder m(this, kMachUint64, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  m.Return(m.ChangeUint32ToUint64((m.*bop.constructor)(p0, p1)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
}


INSTANTIATE_TEST_CASE_P(InstructionSelectorTest,
                        InstructionSelectorChangeUint32ToUint64Test,
                        ::testing::ValuesIn(kWord32BinaryOperations));


// -----------------------------------------------------------------------------
// TruncateInt64ToInt32.


TEST_F(InstructionSelectorTest, TruncateInt64ToInt32WithWord64Sar) {
  StreamBuilder m(this, kMachInt32, kMachInt64);
  Node* const p = m.Parameter(0);
  Node* const t = m.TruncateInt64ToInt32(m.Word64Sar(p, m.Int64Constant(32)));
  m.Return(t);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Shr, s[0]->arch_opcode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(32, s.ToInt32(s[0]->InputAt(1)));
  ASSERT_EQ(1U, s[0]->OutputCount());
  EXPECT_TRUE(s.IsSameAsFirst(s[0]->OutputAt(0)));
  EXPECT_EQ(s.ToVreg(t), s.ToVreg(s[0]->OutputAt(0)));
}


TEST_F(InstructionSelectorTest, TruncateInt64ToInt32WithWord64Shr) {
  StreamBuilder m(this, kMachInt32, kMachInt64);
  Node* const p = m.Parameter(0);
  Node* const t = m.TruncateInt64ToInt32(m.Word64Shr(p, m.Int64Constant(32)));
  m.Return(t);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Shr, s[0]->arch_opcode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(32, s.ToInt32(s[0]->InputAt(1)));
  ASSERT_EQ(1U, s[0]->OutputCount());
  EXPECT_TRUE(s.IsSameAsFirst(s[0]->OutputAt(0)));
  EXPECT_EQ(s.ToVreg(t), s.ToVreg(s[0]->OutputAt(0)));
}


// -----------------------------------------------------------------------------
// Addition.


TEST_F(InstructionSelectorTest, Int32AddWithInt32ParametersLea) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const a0 = m.Int32Add(p0, p1);
  // Additional uses of input to add chooses lea
  Node* const a1 = m.Int32Div(p0, p1);
  m.Return(m.Int32Div(a0, a1));
  Stream s = m.Build();
  ASSERT_EQ(3U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
}


TEST_F(InstructionSelectorTest, Int32AddConstantAsLeaSingle) {
  StreamBuilder m(this, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const c0 = m.Int32Constant(15);
  // If one of the add's operands is only used once, use an "leal", even though
  // an "addl" could be used. The "leal" has proven faster--out best guess is
  // that it gives the register allocation more freedom and it doesn't set
  // flags, reducing pressure in the CPU's pipeline. If we're lucky with
  // register allocation, then code generation will select an "addl" later for
  // the cases that have been measured to be faster.
  Node* const v0 = m.Int32Add(p0, c0);
  m.Return(v0);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MRI, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_TRUE(s[0]->InputAt(1)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddConstantAsAdd) {
  StreamBuilder m(this, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const c0 = m.Int32Constant(1);
  // If there is only a single use of an add's input and the immediate constant
  // for the add is 1, don't use an inc. It is much slower on modern Intel
  // architectures.
  m.Return(m.Int32Add(p0, c0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MRI, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_TRUE(s[0]->InputAt(1)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddConstantAsLeaDouble) {
  StreamBuilder m(this, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const c0 = m.Int32Constant(15);
  // A second use of an add's input uses lea
  Node* const a0 = m.Int32Add(p0, c0);
  m.Return(m.Int32Div(a0, p0));
  Stream s = m.Build();
  ASSERT_EQ(2U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MRI, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_TRUE(s[0]->InputAt(1)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddCommutedConstantAsLeaSingle) {
  StreamBuilder m(this, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const c0 = m.Int32Constant(15);
  // If one of the add's operands is only used once, use an "leal", even though
  // an "addl" could be used. The "leal" has proven faster--out best guess is
  // that it gives the register allocation more freedom and it doesn't set
  // flags, reducing pressure in the CPU's pipeline. If we're lucky with
  // register allocation, then code generation will select an "addl" later for
  // the cases that have been measured to be faster.
  m.Return(m.Int32Add(c0, p0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MRI, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_TRUE(s[0]->InputAt(1)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddCommutedConstantAsLeaDouble) {
  StreamBuilder m(this, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const c0 = m.Int32Constant(15);
  // A second use of an add's input uses lea
  Node* const a0 = m.Int32Add(c0, p0);
  USE(a0);
  m.Return(m.Int32Div(a0, p0));
  Stream s = m.Build();
  ASSERT_EQ(2U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MRI, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_TRUE(s[0]->InputAt(1)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddSimpleAsAdd) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  // If one of the add's operands is only used once, use an "leal", even though
  // an "addl" could be used. The "leal" has proven faster--out best guess is
  // that it gives the register allocation more freedom and it doesn't set
  // flags, reducing pressure in the CPU's pipeline. If we're lucky with
  // register allocation, then code generation will select an "addl" later for
  // the cases that have been measured to be faster.
  m.Return(m.Int32Add(p0, p1));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR1, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddSimpleAsLea) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  // If all of of the add's operands are used multiple times, use an "leal".
  Node* const v1 = m.Int32Add(p0, p1);
  m.Return(m.Int32Add(m.Int32Add(v1, p1), p0));
  Stream s = m.Build();
  ASSERT_EQ(3U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR1, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddScaled2Mul) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(2));
  m.Return(m.Int32Add(p0, s0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddCommutedScaled2Mul) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(2));
  m.Return(m.Int32Add(s0, p0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddScaled2Shl) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Word32Shl(p1, m.Int32Constant(1));
  m.Return(m.Int32Add(p0, s0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddCommutedScaled2Shl) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Word32Shl(p1, m.Int32Constant(1));
  m.Return(m.Int32Add(s0, p0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddScaled4Mul) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(4));
  m.Return(m.Int32Add(p0, s0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR4, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddScaled4Shl) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Word32Shl(p1, m.Int32Constant(2));
  m.Return(m.Int32Add(p0, s0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR4, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddScaled8Mul) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(8));
  m.Return(m.Int32Add(p0, s0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR8, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddScaled8Shl) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Word32Shl(p1, m.Int32Constant(3));
  m.Return(m.Int32Add(p0, s0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR8, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32AddScaled2MulWithConstant) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(2));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(c0, m.Int32Add(p0, s0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled2MulWithConstantShuffle1) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(2));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(p0, m.Int32Add(s0, c0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled2MulWithConstantShuffle2) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(2));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(s0, m.Int32Add(c0, p0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled2MulWithConstantShuffle3) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(2));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(m.Int32Add(s0, c0), p0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled2MulWithConstantShuffle4) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(2));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(m.Int32Add(c0, p0), s0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled2MulWithConstantShuffle5) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(2));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(m.Int32Add(p0, s0), c0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled2ShlWithConstant) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Word32Shl(p1, m.Int32Constant(1));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(c0, m.Int32Add(p0, s0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled4MulWithConstant) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(4));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(c0, m.Int32Add(p0, s0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR4I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled4ShlWithConstant) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Word32Shl(p1, m.Int32Constant(2));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(c0, m.Int32Add(p0, s0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR4I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled8MulWithConstant) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(8));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(c0, m.Int32Add(p0, s0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR8I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled8ShlWithConstant) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const s0 = m.Word32Shl(p1, m.Int32Constant(3));
  Node* const c0 = m.Int32Constant(15);
  m.Return(m.Int32Add(c0, m.Int32Add(p0, s0)));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR8I, s[0]->addressing_mode());
  ASSERT_EQ(3U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(s[0]->InputAt(2)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32SubConstantAsSub) {
  StreamBuilder m(this, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const c0 = m.Int32Constant(-1);
  // If there is only a single use of on of the sub's non-constant input, use a
  // "subl" instruction.
  m.Return(m.Int32Sub(p0, c0));
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MRI, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_TRUE(s[0]->InputAt(1)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32SubConstantAsLea) {
  StreamBuilder m(this, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const c0 = m.Int32Constant(-1);
  // If there are multiple uses of on of the sub's non-constant input, use a
  // "leal" instruction.
  Node* const v0 = m.Int32Sub(p0, c0);
  m.Return(m.Int32Div(p0, v0));
  Stream s = m.Build();
  ASSERT_EQ(2U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MRI, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_TRUE(s[0]->InputAt(1)->IsImmediate());
}


TEST_F(InstructionSelectorTest, Int32AddScaled2Other) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const p2 = m.Parameter(2);
  Node* const s0 = m.Int32Mul(p1, m.Int32Constant(2));
  Node* const a0 = m.Int32Add(s0, p2);
  Node* const a1 = m.Int32Add(p0, a0);
  m.Return(a1);
  Stream s = m.Build();
  ASSERT_EQ(2U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p2), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_EQ(s.ToVreg(a0), s.ToVreg(s[0]->OutputAt(0)));
  ASSERT_EQ(2U, s[1]->InputCount());
  EXPECT_EQ(kX64Lea32, s[1]->arch_opcode());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[1]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(a0), s.ToVreg(s[1]->InputAt(1)));
  EXPECT_EQ(s.ToVreg(a1), s.ToVreg(s[1]->OutputAt(0)));
}


// -----------------------------------------------------------------------------
// Multiplication.


TEST_F(InstructionSelectorTest, Int32MulWithInt32MulWithParameters) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const m0 = m.Int32Mul(p0, p1);
  m.Return(m.Int32Mul(m0, p0));
  Stream s = m.Build();
  ASSERT_EQ(2U, s.size());
  EXPECT_EQ(kX64Imul32, s[0]->arch_opcode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(1)));
  ASSERT_EQ(1U, s[0]->OutputCount());
  EXPECT_EQ(s.ToVreg(m0), s.ToVreg(s[0]->OutputAt(0)));
  EXPECT_EQ(kX64Imul32, s[1]->arch_opcode());
  ASSERT_EQ(2U, s[1]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[1]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(m0), s.ToVreg(s[1]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32MulHigh) {
  StreamBuilder m(this, kMachInt32, kMachInt32, kMachInt32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const n = m.Int32MulHigh(p0, p1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64ImulHigh32, s[0]->arch_opcode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_TRUE(s.IsFixed(s[0]->InputAt(0), rax));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(!s.IsUsedAtStart(s[0]->InputAt(1)));
  ASSERT_LE(1U, s[0]->OutputCount());
  EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
  EXPECT_TRUE(s.IsFixed(s[0]->OutputAt(0), rdx));
}


TEST_F(InstructionSelectorTest, Uint32MulHigh) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const p1 = m.Parameter(1);
  Node* const n = m.Uint32MulHigh(p0, p1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64UmulHigh32, s[0]->arch_opcode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_TRUE(s.IsFixed(s[0]->InputAt(0), rax));
  EXPECT_EQ(s.ToVreg(p1), s.ToVreg(s[0]->InputAt(1)));
  EXPECT_TRUE(!s.IsUsedAtStart(s[0]->InputAt(1)));
  ASSERT_LE(1U, s[0]->OutputCount());
  EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
  EXPECT_TRUE(s.IsFixed(s[0]->OutputAt(0), rdx));
}


TEST_F(InstructionSelectorTest, Int32Mul2BecomesLea) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const c1 = m.Int32Constant(2);
  Node* const n = m.Int32Mul(p0, c1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR1, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32Mul3BecomesLea) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const c1 = m.Int32Constant(3);
  Node* const n = m.Int32Mul(p0, c1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR2, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32Mul4BecomesLea) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const c1 = m.Int32Constant(4);
  Node* const n = m.Int32Mul(p0, c1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_M4, s[0]->addressing_mode());
  ASSERT_EQ(1U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
}


TEST_F(InstructionSelectorTest, Int32Mul5BecomesLea) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const c1 = m.Int32Constant(5);
  Node* const n = m.Int32Mul(p0, c1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR4, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32Mul8BecomesLea) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const c1 = m.Int32Constant(8);
  Node* const n = m.Int32Mul(p0, c1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_M8, s[0]->addressing_mode());
  ASSERT_EQ(1U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
}


TEST_F(InstructionSelectorTest, Int32Mul9BecomesLea) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const c1 = m.Int32Constant(9);
  Node* const n = m.Int32Mul(p0, c1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR8, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(1)));
}


// -----------------------------------------------------------------------------
// Word32Shl.


TEST_F(InstructionSelectorTest, Int32Shl1BecomesLea) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const c1 = m.Int32Constant(1);
  Node* const n = m.Word32Shl(p0, c1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_MR1, s[0]->addressing_mode());
  ASSERT_EQ(2U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(1)));
}


TEST_F(InstructionSelectorTest, Int32Shl2BecomesLea) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const c1 = m.Int32Constant(2);
  Node* const n = m.Word32Shl(p0, c1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_M4, s[0]->addressing_mode());
  ASSERT_EQ(1U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
}


TEST_F(InstructionSelectorTest, Int32Shl4BecomesLea) {
  StreamBuilder m(this, kMachUint32, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const c1 = m.Int32Constant(3);
  Node* const n = m.Word32Shl(p0, c1);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lea32, s[0]->arch_opcode());
  EXPECT_EQ(kMode_M8, s[0]->addressing_mode());
  ASSERT_EQ(1U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
}


// -----------------------------------------------------------------------------
// Floating point operations.


TEST_F(InstructionSelectorTest, Float32Abs) {
  {
    StreamBuilder m(this, kMachFloat32, kMachFloat32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Float32Abs(p0);
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kSSEFloat32Abs, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_TRUE(s.IsSameAsFirst(s[0]->Output()));
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
    EXPECT_EQ(kFlags_none, s[0]->flags_mode());
  }
  {
    StreamBuilder m(this, kMachFloat32, kMachFloat32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Float32Abs(p0);
    m.Return(n);
    Stream s = m.Build(AVX);
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kAVXFloat32Abs, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
    EXPECT_EQ(kFlags_none, s[0]->flags_mode());
  }
}


TEST_F(InstructionSelectorTest, Float64Abs) {
  {
    StreamBuilder m(this, kMachFloat64, kMachFloat64);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Float64Abs(p0);
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kSSEFloat64Abs, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_TRUE(s.IsSameAsFirst(s[0]->Output()));
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
    EXPECT_EQ(kFlags_none, s[0]->flags_mode());
  }
  {
    StreamBuilder m(this, kMachFloat64, kMachFloat64);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Float64Abs(p0);
    m.Return(n);
    Stream s = m.Build(AVX);
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kAVXFloat64Abs, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
    EXPECT_EQ(kFlags_none, s[0]->flags_mode());
  }
}


TEST_F(InstructionSelectorTest, Float64BinopArithmetic) {
  {
    StreamBuilder m(this, kMachFloat64, kMachFloat64, kMachFloat64);
    Node* add = m.Float64Add(m.Parameter(0), m.Parameter(1));
    Node* mul = m.Float64Mul(add, m.Parameter(1));
    Node* sub = m.Float64Sub(mul, add);
    Node* ret = m.Float64Div(mul, sub);
    m.Return(ret);
    Stream s = m.Build(AVX);
    ASSERT_EQ(4U, s.size());
    EXPECT_EQ(kAVXFloat64Add, s[0]->arch_opcode());
    EXPECT_EQ(kAVXFloat64Mul, s[1]->arch_opcode());
    EXPECT_EQ(kAVXFloat64Sub, s[2]->arch_opcode());
    EXPECT_EQ(kAVXFloat64Div, s[3]->arch_opcode());
  }
  {
    StreamBuilder m(this, kMachFloat64, kMachFloat64, kMachFloat64);
    Node* add = m.Float64Add(m.Parameter(0), m.Parameter(1));
    Node* mul = m.Float64Mul(add, m.Parameter(1));
    Node* sub = m.Float64Sub(mul, add);
    Node* ret = m.Float64Div(mul, sub);
    m.Return(ret);
    Stream s = m.Build();
    ASSERT_EQ(4U, s.size());
    EXPECT_EQ(kSSEFloat64Add, s[0]->arch_opcode());
    EXPECT_EQ(kSSEFloat64Mul, s[1]->arch_opcode());
    EXPECT_EQ(kSSEFloat64Sub, s[2]->arch_opcode());
    EXPECT_EQ(kSSEFloat64Div, s[3]->arch_opcode());
  }
}


TEST_F(InstructionSelectorTest, Float32SubWithMinusZeroAndParameter) {
  {
    StreamBuilder m(this, kMachFloat32, kMachFloat32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Float32Sub(m.Float32Constant(-0.0f), p0);
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kSSEFloat32Neg, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
    EXPECT_EQ(kFlags_none, s[0]->flags_mode());
  }
  {
    StreamBuilder m(this, kMachFloat32, kMachFloat32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Float32Sub(m.Float32Constant(-0.0f), p0);
    m.Return(n);
    Stream s = m.Build(AVX);
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kAVXFloat32Neg, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
    EXPECT_EQ(kFlags_none, s[0]->flags_mode());
  }
}


TEST_F(InstructionSelectorTest, Float64SubWithMinusZeroAndParameter) {
  {
    StreamBuilder m(this, kMachFloat64, kMachFloat64);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Float64Sub(m.Float64Constant(-0.0), p0);
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kSSEFloat64Neg, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
    EXPECT_EQ(kFlags_none, s[0]->flags_mode());
  }
  {
    StreamBuilder m(this, kMachFloat64, kMachFloat64);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Float64Sub(m.Float64Constant(-0.0), p0);
    m.Return(n);
    Stream s = m.Build(AVX);
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kAVXFloat64Neg, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
    EXPECT_EQ(kFlags_none, s[0]->flags_mode());
  }
}


// -----------------------------------------------------------------------------
// Miscellaneous.


TEST_F(InstructionSelectorTest, Uint64LessThanWithLoadAndLoadStackPointer) {
  StreamBuilder m(this, kMachBool);
  Node* const sl = m.Load(
      kMachPtr,
      m.ExternalConstant(ExternalReference::address_of_stack_limit(isolate())));
  Node* const sp = m.LoadStackPointer();
  Node* const n = m.Uint64LessThan(sl, sp);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64StackCheck, s[0]->arch_opcode());
  ASSERT_EQ(0U, s[0]->InputCount());
  ASSERT_EQ(1U, s[0]->OutputCount());
  EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
  EXPECT_EQ(kFlags_set, s[0]->flags_mode());
  EXPECT_EQ(kUnsignedGreaterThan, s[0]->flags_condition());
}


TEST_F(InstructionSelectorTest, Word64ShlWithChangeInt32ToInt64) {
  TRACED_FORRANGE(int64_t, x, 32, 63) {
    StreamBuilder m(this, kMachInt64, kMachInt32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Word64Shl(m.ChangeInt32ToInt64(p0), m.Int64Constant(x));
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kX64Shl, s[0]->arch_opcode());
    ASSERT_EQ(2U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    EXPECT_EQ(x, s.ToInt32(s[0]->InputAt(1)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_TRUE(s.IsSameAsFirst(s[0]->Output()));
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
  }
}


TEST_F(InstructionSelectorTest, Word64ShlWithChangeUint32ToUint64) {
  TRACED_FORRANGE(int64_t, x, 32, 63) {
    StreamBuilder m(this, kMachInt64, kMachUint32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Word64Shl(m.ChangeUint32ToUint64(p0), m.Int64Constant(x));
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kX64Shl, s[0]->arch_opcode());
    ASSERT_EQ(2U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    EXPECT_EQ(x, s.ToInt32(s[0]->InputAt(1)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_TRUE(s.IsSameAsFirst(s[0]->Output()));
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
  }
}


TEST_F(InstructionSelectorTest, Word32AndWith0xff) {
  {
    StreamBuilder m(this, kMachInt32, kMachInt32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Word32And(p0, m.Int32Constant(0xff));
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kX64Movzxbl, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
  }
  {
    StreamBuilder m(this, kMachInt32, kMachInt32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Word32And(m.Int32Constant(0xff), p0);
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kX64Movzxbl, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
  }
}


TEST_F(InstructionSelectorTest, Word32AndWith0xffff) {
  {
    StreamBuilder m(this, kMachInt32, kMachInt32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Word32And(p0, m.Int32Constant(0xffff));
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kX64Movzxwl, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
  }
  {
    StreamBuilder m(this, kMachInt32, kMachInt32);
    Node* const p0 = m.Parameter(0);
    Node* const n = m.Word32And(m.Int32Constant(0xffff), p0);
    m.Return(n);
    Stream s = m.Build();
    ASSERT_EQ(1U, s.size());
    EXPECT_EQ(kX64Movzxwl, s[0]->arch_opcode());
    ASSERT_EQ(1U, s[0]->InputCount());
    EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
    ASSERT_EQ(1U, s[0]->OutputCount());
    EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
  }
}


TEST_F(InstructionSelectorTest, Word32Clz) {
  StreamBuilder m(this, kMachUint32, kMachUint32);
  Node* const p0 = m.Parameter(0);
  Node* const n = m.Word32Clz(p0);
  m.Return(n);
  Stream s = m.Build();
  ASSERT_EQ(1U, s.size());
  EXPECT_EQ(kX64Lzcnt32, s[0]->arch_opcode());
  ASSERT_EQ(1U, s[0]->InputCount());
  EXPECT_EQ(s.ToVreg(p0), s.ToVreg(s[0]->InputAt(0)));
  ASSERT_EQ(1U, s[0]->OutputCount());
  EXPECT_EQ(s.ToVreg(n), s.ToVreg(s[0]->Output()));
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
