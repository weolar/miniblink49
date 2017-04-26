// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/common-operator.h"
#include "src/compiler/common-operator-reducer.h"
#include "src/compiler/machine-operator.h"
#include "src/compiler/machine-type.h"
#include "src/compiler/operator.h"
#include "src/compiler/simplified-operator.h"
#include "test/unittests/compiler/graph-reducer-unittest.h"
#include "test/unittests/compiler/graph-unittest.h"
#include "test/unittests/compiler/node-test-utils.h"

using testing::StrictMock;

namespace v8 {
namespace internal {
namespace compiler {

class CommonOperatorReducerTest : public GraphTest {
 public:
  explicit CommonOperatorReducerTest(int num_parameters = 1)
      : GraphTest(num_parameters), machine_(zone()), simplified_(zone()) {}
  ~CommonOperatorReducerTest() override {}

 protected:
  Reduction Reduce(
      AdvancedReducer::Editor* editor, Node* node,
      MachineOperatorBuilder::Flags flags = MachineOperatorBuilder::kNoFlags) {
    MachineOperatorBuilder machine(zone(), kMachPtr, flags);
    CommonOperatorReducer reducer(editor, graph(), common(), &machine);
    return reducer.Reduce(node);
  }

  Reduction Reduce(Node* node, MachineOperatorBuilder::Flags flags =
                                   MachineOperatorBuilder::kNoFlags) {
    StrictMock<MockAdvancedReducerEditor> editor;
    return Reduce(&editor, node, flags);
  }

  MachineOperatorBuilder* machine() { return &machine_; }
  SimplifiedOperatorBuilder* simplified() { return &simplified_; }

 private:
  MachineOperatorBuilder machine_;
  SimplifiedOperatorBuilder simplified_;
};


namespace {

const BranchHint kBranchHints[] = {BranchHint::kNone, BranchHint::kFalse,
                                   BranchHint::kTrue};


const MachineType kMachineTypes[] = {
    kMachFloat32, kMachFloat64,   kMachInt8,   kMachUint8,  kMachInt16,
    kMachUint16,  kMachInt32,     kMachUint32, kMachInt64,  kMachUint64,
    kMachPtr,     kMachAnyTagged, kRepBit,     kRepWord8,   kRepWord16,
    kRepWord32,   kRepWord64,     kRepFloat32, kRepFloat64, kRepTagged};


const Operator kOp0(0, Operator::kNoProperties, "Op0", 0, 0, 0, 1, 1, 0);

}  // namespace


// -----------------------------------------------------------------------------
// Branch


TEST_F(CommonOperatorReducerTest, BranchWithInt32ZeroConstant) {
  TRACED_FOREACH(BranchHint, hint, kBranchHints) {
    Node* const control = graph()->start();
    Node* const branch =
        graph()->NewNode(common()->Branch(hint), Int32Constant(0), control);
    Node* const if_true = graph()->NewNode(common()->IfTrue(), branch);
    Node* const if_false = graph()->NewNode(common()->IfFalse(), branch);
    StrictMock<MockAdvancedReducerEditor> editor;
    EXPECT_CALL(editor, Replace(if_true, IsDead()));
    EXPECT_CALL(editor, Replace(if_false, control));
    Reduction const r = Reduce(&editor, branch);
    ASSERT_TRUE(r.Changed());
    EXPECT_THAT(r.replacement(), IsDead());
  }
}


TEST_F(CommonOperatorReducerTest, BranchWithInt32OneConstant) {
  TRACED_FOREACH(BranchHint, hint, kBranchHints) {
    Node* const control = graph()->start();
    Node* const branch =
        graph()->NewNode(common()->Branch(hint), Int32Constant(1), control);
    Node* const if_true = graph()->NewNode(common()->IfTrue(), branch);
    Node* const if_false = graph()->NewNode(common()->IfFalse(), branch);
    StrictMock<MockAdvancedReducerEditor> editor;
    EXPECT_CALL(editor, Replace(if_true, control));
    EXPECT_CALL(editor, Replace(if_false, IsDead()));
    Reduction const r = Reduce(&editor, branch);
    ASSERT_TRUE(r.Changed());
    EXPECT_THAT(r.replacement(), IsDead());
  }
}


TEST_F(CommonOperatorReducerTest, BranchWithInt64ZeroConstant) {
  TRACED_FOREACH(BranchHint, hint, kBranchHints) {
    Node* const control = graph()->start();
    Node* const branch =
        graph()->NewNode(common()->Branch(hint), Int64Constant(0), control);
    Node* const if_true = graph()->NewNode(common()->IfTrue(), branch);
    Node* const if_false = graph()->NewNode(common()->IfFalse(), branch);
    StrictMock<MockAdvancedReducerEditor> editor;
    EXPECT_CALL(editor, Replace(if_true, IsDead()));
    EXPECT_CALL(editor, Replace(if_false, control));
    Reduction const r = Reduce(&editor, branch);
    ASSERT_TRUE(r.Changed());
    EXPECT_THAT(r.replacement(), IsDead());
  }
}


TEST_F(CommonOperatorReducerTest, BranchWithInt64OneConstant) {
  TRACED_FOREACH(BranchHint, hint, kBranchHints) {
    Node* const control = graph()->start();
    Node* const branch =
        graph()->NewNode(common()->Branch(hint), Int64Constant(1), control);
    Node* const if_true = graph()->NewNode(common()->IfTrue(), branch);
    Node* const if_false = graph()->NewNode(common()->IfFalse(), branch);
    StrictMock<MockAdvancedReducerEditor> editor;
    EXPECT_CALL(editor, Replace(if_true, control));
    EXPECT_CALL(editor, Replace(if_false, IsDead()));
    Reduction const r = Reduce(&editor, branch);
    ASSERT_TRUE(r.Changed());
    EXPECT_THAT(r.replacement(), IsDead());
  }
}


TEST_F(CommonOperatorReducerTest, BranchWithFalseConstant) {
  TRACED_FOREACH(BranchHint, hint, kBranchHints) {
    Node* const control = graph()->start();
    Node* const branch =
        graph()->NewNode(common()->Branch(hint), FalseConstant(), control);
    Node* const if_true = graph()->NewNode(common()->IfTrue(), branch);
    Node* const if_false = graph()->NewNode(common()->IfFalse(), branch);
    StrictMock<MockAdvancedReducerEditor> editor;
    EXPECT_CALL(editor, Replace(if_true, IsDead()));
    EXPECT_CALL(editor, Replace(if_false, control));
    Reduction const r = Reduce(&editor, branch);
    ASSERT_TRUE(r.Changed());
    EXPECT_THAT(r.replacement(), IsDead());
  }
}


TEST_F(CommonOperatorReducerTest, BranchWithTrueConstant) {
  TRACED_FOREACH(BranchHint, hint, kBranchHints) {
    Node* const control = graph()->start();
    Node* const branch =
        graph()->NewNode(common()->Branch(hint), TrueConstant(), control);
    Node* const if_true = graph()->NewNode(common()->IfTrue(), branch);
    Node* const if_false = graph()->NewNode(common()->IfFalse(), branch);
    StrictMock<MockAdvancedReducerEditor> editor;
    EXPECT_CALL(editor, Replace(if_true, control));
    EXPECT_CALL(editor, Replace(if_false, IsDead()));
    Reduction const r = Reduce(&editor, branch);
    ASSERT_TRUE(r.Changed());
    EXPECT_THAT(r.replacement(), IsDead());
  }
}


TEST_F(CommonOperatorReducerTest, BranchWithBooleanNot) {
  Node* const value = Parameter(0);
  TRACED_FOREACH(BranchHint, hint, kBranchHints) {
    Node* const control = graph()->start();
    Node* const branch = graph()->NewNode(
        common()->Branch(hint),
        graph()->NewNode(simplified()->BooleanNot(), value), control);
    Node* const if_true = graph()->NewNode(common()->IfTrue(), branch);
    Node* const if_false = graph()->NewNode(common()->IfFalse(), branch);
    Reduction const r = Reduce(branch);
    ASSERT_TRUE(r.Changed());
    EXPECT_EQ(branch, r.replacement());
    EXPECT_THAT(branch, IsBranch(value, control));
    EXPECT_THAT(if_false, IsIfTrue(branch));
    EXPECT_THAT(if_true, IsIfFalse(branch));
    EXPECT_EQ(NegateBranchHint(hint), BranchHintOf(branch->op()));
  }
}


// -----------------------------------------------------------------------------
// Merge


TEST_F(CommonOperatorReducerTest, MergeOfUnusedDiamond0) {
  Node* const value = Parameter(0);
  Node* const control = graph()->start();
  Node* const branch = graph()->NewNode(common()->Branch(), value, control);
  Node* const if_true = graph()->NewNode(common()->IfTrue(), branch);
  Node* const if_false = graph()->NewNode(common()->IfFalse(), branch);
  Reduction const r =
      Reduce(graph()->NewNode(common()->Merge(2), if_true, if_false));
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(control, r.replacement());
  EXPECT_THAT(branch, IsDead());
}


TEST_F(CommonOperatorReducerTest, MergeOfUnusedDiamond1) {
  Node* const value = Parameter(0);
  Node* const control = graph()->start();
  Node* const branch = graph()->NewNode(common()->Branch(), value, control);
  Node* const if_true = graph()->NewNode(common()->IfTrue(), branch);
  Node* const if_false = graph()->NewNode(common()->IfFalse(), branch);
  Reduction const r =
      Reduce(graph()->NewNode(common()->Merge(2), if_false, if_true));
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(control, r.replacement());
  EXPECT_THAT(branch, IsDead());
}


// -----------------------------------------------------------------------------
// EffectPhi


TEST_F(CommonOperatorReducerTest, EffectPhiWithMerge) {
  const int kMaxInputs = 64;
  Node* inputs[kMaxInputs];
  Node* const input = graph()->NewNode(&kOp0);
  TRACED_FORRANGE(int, input_count, 2, kMaxInputs - 1) {
    int const value_input_count = input_count - 1;
    for (int i = 0; i < value_input_count; ++i) {
      inputs[i] = graph()->start();
    }
    Node* const merge = graph()->NewNode(common()->Merge(value_input_count),
                                         value_input_count, inputs);
    for (int i = 0; i < value_input_count; ++i) {
      inputs[i] = input;
    }
    inputs[value_input_count] = merge;
    StrictMock<MockAdvancedReducerEditor> editor;
    EXPECT_CALL(editor, Revisit(merge));
    Reduction r =
        Reduce(&editor, graph()->NewNode(common()->EffectPhi(value_input_count),
                                         input_count, inputs));
    ASSERT_TRUE(r.Changed());
    EXPECT_EQ(input, r.replacement());
  }
}


TEST_F(CommonOperatorReducerTest, EffectPhiWithLoop) {
  Node* const e0 = graph()->NewNode(&kOp0);
  Node* const loop =
      graph()->NewNode(common()->Loop(2), graph()->start(), graph()->start());
  loop->ReplaceInput(1, loop);
  Node* const ephi = graph()->NewNode(common()->EffectPhi(2), e0, e0, loop);
  ephi->ReplaceInput(1, ephi);
  StrictMock<MockAdvancedReducerEditor> editor;
  EXPECT_CALL(editor, Revisit(loop));
  Reduction const r = Reduce(&editor, ephi);
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(e0, r.replacement());
}


// -----------------------------------------------------------------------------
// Phi


TEST_F(CommonOperatorReducerTest, PhiWithMerge) {
  const int kMaxInputs = 64;
  Node* inputs[kMaxInputs];
  Node* const input = graph()->NewNode(&kOp0);
  TRACED_FORRANGE(int, input_count, 2, kMaxInputs - 1) {
    int const value_input_count = input_count - 1;
    TRACED_FOREACH(MachineType, type, kMachineTypes) {
      for (int i = 0; i < value_input_count; ++i) {
        inputs[i] = graph()->start();
      }
      Node* const merge = graph()->NewNode(common()->Merge(value_input_count),
                                           value_input_count, inputs);
      for (int i = 0; i < value_input_count; ++i) {
        inputs[i] = input;
      }
      inputs[value_input_count] = merge;
      StrictMock<MockAdvancedReducerEditor> editor;
      EXPECT_CALL(editor, Revisit(merge));
      Reduction r = Reduce(
          &editor, graph()->NewNode(common()->Phi(type, value_input_count),
                                    input_count, inputs));
      ASSERT_TRUE(r.Changed());
      EXPECT_EQ(input, r.replacement());
    }
  }
}


TEST_F(CommonOperatorReducerTest, PhiWithLoop) {
  Node* const p0 = Parameter(0);
  Node* const loop =
      graph()->NewNode(common()->Loop(2), graph()->start(), graph()->start());
  loop->ReplaceInput(1, loop);
  Node* const phi =
      graph()->NewNode(common()->Phi(kMachAnyTagged, 2), p0, p0, loop);
  phi->ReplaceInput(1, phi);
  StrictMock<MockAdvancedReducerEditor> editor;
  EXPECT_CALL(editor, Revisit(loop));
  Reduction const r = Reduce(&editor, phi);
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(p0, r.replacement());
}


TEST_F(CommonOperatorReducerTest, PhiToFloat32Abs) {
  Node* p0 = Parameter(0);
  Node* c0 = Float32Constant(0.0);
  Node* check = graph()->NewNode(machine()->Float32LessThan(), c0, p0);
  Node* branch = graph()->NewNode(common()->Branch(), check, graph()->start());
  Node* if_true = graph()->NewNode(common()->IfTrue(), branch);
  Node* vtrue = p0;
  Node* if_false = graph()->NewNode(common()->IfFalse(), branch);
  Node* vfalse = graph()->NewNode(machine()->Float32Sub(), c0, p0);
  Node* merge = graph()->NewNode(common()->Merge(2), if_true, if_false);
  Node* phi =
      graph()->NewNode(common()->Phi(kMachFloat32, 2), vtrue, vfalse, merge);
  StrictMock<MockAdvancedReducerEditor> editor;
  EXPECT_CALL(editor, Revisit(merge));
  Reduction r = Reduce(&editor, phi);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat32Abs(p0));
}


TEST_F(CommonOperatorReducerTest, PhiToFloat64Abs) {
  Node* p0 = Parameter(0);
  Node* c0 = Float64Constant(0.0);
  Node* check = graph()->NewNode(machine()->Float64LessThan(), c0, p0);
  Node* branch = graph()->NewNode(common()->Branch(), check, graph()->start());
  Node* if_true = graph()->NewNode(common()->IfTrue(), branch);
  Node* vtrue = p0;
  Node* if_false = graph()->NewNode(common()->IfFalse(), branch);
  Node* vfalse = graph()->NewNode(machine()->Float64Sub(), c0, p0);
  Node* merge = graph()->NewNode(common()->Merge(2), if_true, if_false);
  Node* phi =
      graph()->NewNode(common()->Phi(kMachFloat64, 2), vtrue, vfalse, merge);
  StrictMock<MockAdvancedReducerEditor> editor;
  EXPECT_CALL(editor, Revisit(merge));
  Reduction r = Reduce(&editor, phi);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat64Abs(p0));
}


TEST_F(CommonOperatorReducerTest, PhiToFloat32Max) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* check = graph()->NewNode(machine()->Float32LessThan(), p0, p1);
  Node* branch = graph()->NewNode(common()->Branch(), check, graph()->start());
  Node* if_true = graph()->NewNode(common()->IfTrue(), branch);
  Node* if_false = graph()->NewNode(common()->IfFalse(), branch);
  Node* merge = graph()->NewNode(common()->Merge(2), if_true, if_false);
  Node* phi = graph()->NewNode(common()->Phi(kMachFloat32, 2), p1, p0, merge);
  StrictMock<MockAdvancedReducerEditor> editor;
  EXPECT_CALL(editor, Revisit(merge));
  Reduction r = Reduce(&editor, phi, MachineOperatorBuilder::kFloat32Max);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat32Max(p1, p0));
}


TEST_F(CommonOperatorReducerTest, PhiToFloat64Max) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* check = graph()->NewNode(machine()->Float64LessThan(), p0, p1);
  Node* branch = graph()->NewNode(common()->Branch(), check, graph()->start());
  Node* if_true = graph()->NewNode(common()->IfTrue(), branch);
  Node* if_false = graph()->NewNode(common()->IfFalse(), branch);
  Node* merge = graph()->NewNode(common()->Merge(2), if_true, if_false);
  Node* phi = graph()->NewNode(common()->Phi(kMachFloat64, 2), p1, p0, merge);
  StrictMock<MockAdvancedReducerEditor> editor;
  EXPECT_CALL(editor, Revisit(merge));
  Reduction r = Reduce(&editor, phi, MachineOperatorBuilder::kFloat64Max);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat64Max(p1, p0));
}


TEST_F(CommonOperatorReducerTest, PhiToFloat32Min) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* check = graph()->NewNode(machine()->Float32LessThan(), p0, p1);
  Node* branch = graph()->NewNode(common()->Branch(), check, graph()->start());
  Node* if_true = graph()->NewNode(common()->IfTrue(), branch);
  Node* if_false = graph()->NewNode(common()->IfFalse(), branch);
  Node* merge = graph()->NewNode(common()->Merge(2), if_true, if_false);
  Node* phi = graph()->NewNode(common()->Phi(kMachFloat32, 2), p0, p1, merge);
  StrictMock<MockAdvancedReducerEditor> editor;
  EXPECT_CALL(editor, Revisit(merge));
  Reduction r = Reduce(&editor, phi, MachineOperatorBuilder::kFloat32Min);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat32Min(p0, p1));
}


TEST_F(CommonOperatorReducerTest, PhiToFloat64Min) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* check = graph()->NewNode(machine()->Float64LessThan(), p0, p1);
  Node* branch = graph()->NewNode(common()->Branch(), check, graph()->start());
  Node* if_true = graph()->NewNode(common()->IfTrue(), branch);
  Node* if_false = graph()->NewNode(common()->IfFalse(), branch);
  Node* merge = graph()->NewNode(common()->Merge(2), if_true, if_false);
  Node* phi = graph()->NewNode(common()->Phi(kMachFloat64, 2), p0, p1, merge);
  StrictMock<MockAdvancedReducerEditor> editor;
  EXPECT_CALL(editor, Revisit(merge));
  Reduction r = Reduce(&editor, phi, MachineOperatorBuilder::kFloat64Min);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat64Min(p0, p1));
}


// -----------------------------------------------------------------------------
// Return


TEST_F(CommonOperatorReducerTest, ReturnWithPhiAndEffectPhiAndMerge) {
  Node* cond = Parameter(2);
  Node* branch = graph()->NewNode(common()->Branch(), cond, graph()->start());
  Node* if_true = graph()->NewNode(common()->IfTrue(), branch);
  Node* etrue = graph()->start();
  Node* vtrue = Parameter(0);
  Node* if_false = graph()->NewNode(common()->IfFalse(), branch);
  Node* efalse = graph()->start();
  Node* vfalse = Parameter(1);
  Node* merge = graph()->NewNode(common()->Merge(2), if_true, if_false);
  Node* ephi = graph()->NewNode(common()->EffectPhi(2), etrue, efalse, merge);
  Node* phi =
      graph()->NewNode(common()->Phi(kMachAnyTagged, 2), vtrue, vfalse, merge);
  Node* ret = graph()->NewNode(common()->Return(), phi, ephi, merge);
  graph()->SetEnd(graph()->NewNode(common()->End(1), ret));
  StrictMock<MockAdvancedReducerEditor> editor;
  EXPECT_CALL(editor, Replace(merge, IsDead()));
  Reduction const r = Reduce(&editor, ret);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsDead());
  EXPECT_THAT(graph()->end(), IsEnd(ret, IsReturn(vtrue, etrue, if_true),
                                    IsReturn(vfalse, efalse, if_false)));
}


// -----------------------------------------------------------------------------
// Select


TEST_F(CommonOperatorReducerTest, SelectWithSameThenAndElse) {
  Node* const input = graph()->NewNode(&kOp0);
  TRACED_FOREACH(BranchHint, hint, kBranchHints) {
    TRACED_FOREACH(MachineType, type, kMachineTypes) {
      Reduction r = Reduce(
          graph()->NewNode(common()->Select(type, hint), input, input, input));
      ASSERT_TRUE(r.Changed());
      EXPECT_EQ(input, r.replacement());
    }
  }
}


TEST_F(CommonOperatorReducerTest, SelectWithInt32ZeroConstant) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* select = graph()->NewNode(common()->Select(kMachAnyTagged),
                                  Int32Constant(0), p0, p1);
  Reduction r = Reduce(select);
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(p1, r.replacement());
}


TEST_F(CommonOperatorReducerTest, SelectWithInt32OneConstant) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* select = graph()->NewNode(common()->Select(kMachAnyTagged),
                                  Int32Constant(1), p0, p1);
  Reduction r = Reduce(select);
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(p0, r.replacement());
}


TEST_F(CommonOperatorReducerTest, SelectWithInt64ZeroConstant) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* select = graph()->NewNode(common()->Select(kMachAnyTagged),
                                  Int64Constant(0), p0, p1);
  Reduction r = Reduce(select);
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(p1, r.replacement());
}


TEST_F(CommonOperatorReducerTest, SelectWithInt64OneConstant) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* select = graph()->NewNode(common()->Select(kMachAnyTagged),
                                  Int64Constant(1), p0, p1);
  Reduction r = Reduce(select);
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(p0, r.replacement());
}


TEST_F(CommonOperatorReducerTest, SelectWithFalseConstant) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* select = graph()->NewNode(common()->Select(kMachAnyTagged),
                                  FalseConstant(), p0, p1);
  Reduction r = Reduce(select);
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(p1, r.replacement());
}


TEST_F(CommonOperatorReducerTest, SelectWithTrueConstant) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* select = graph()->NewNode(common()->Select(kMachAnyTagged),
                                  TrueConstant(), p0, p1);
  Reduction r = Reduce(select);
  ASSERT_TRUE(r.Changed());
  EXPECT_EQ(p0, r.replacement());
}


TEST_F(CommonOperatorReducerTest, SelectToFloat32Abs) {
  Node* p0 = Parameter(0);
  Node* c0 = Float32Constant(0.0);
  Node* check = graph()->NewNode(machine()->Float32LessThan(), c0, p0);
  Node* select =
      graph()->NewNode(common()->Select(kMachFloat32), check, p0,
                       graph()->NewNode(machine()->Float32Sub(), c0, p0));
  Reduction r = Reduce(select);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat32Abs(p0));
}


TEST_F(CommonOperatorReducerTest, SelectToFloat64Abs) {
  Node* p0 = Parameter(0);
  Node* c0 = Float64Constant(0.0);
  Node* check = graph()->NewNode(machine()->Float64LessThan(), c0, p0);
  Node* select =
      graph()->NewNode(common()->Select(kMachFloat64), check, p0,
                       graph()->NewNode(machine()->Float64Sub(), c0, p0));
  Reduction r = Reduce(select);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat64Abs(p0));
}


TEST_F(CommonOperatorReducerTest, SelectToFloat32Max) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* check = graph()->NewNode(machine()->Float32LessThan(), p0, p1);
  Node* select =
      graph()->NewNode(common()->Select(kMachFloat32), check, p1, p0);
  Reduction r = Reduce(select, MachineOperatorBuilder::kFloat32Max);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat32Max(p1, p0));
}


TEST_F(CommonOperatorReducerTest, SelectToFloat64Max) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* check = graph()->NewNode(machine()->Float64LessThan(), p0, p1);
  Node* select =
      graph()->NewNode(common()->Select(kMachFloat64), check, p1, p0);
  Reduction r = Reduce(select, MachineOperatorBuilder::kFloat64Max);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat64Max(p1, p0));
}


TEST_F(CommonOperatorReducerTest, SelectToFloat32Min) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* check = graph()->NewNode(machine()->Float32LessThan(), p0, p1);
  Node* select =
      graph()->NewNode(common()->Select(kMachFloat32), check, p0, p1);
  Reduction r = Reduce(select, MachineOperatorBuilder::kFloat32Min);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat32Min(p0, p1));
}


TEST_F(CommonOperatorReducerTest, SelectToFloat64Min) {
  Node* p0 = Parameter(0);
  Node* p1 = Parameter(1);
  Node* check = graph()->NewNode(machine()->Float64LessThan(), p0, p1);
  Node* select =
      graph()->NewNode(common()->Select(kMachFloat64), check, p0, p1);
  Reduction r = Reduce(select, MachineOperatorBuilder::kFloat64Min);
  ASSERT_TRUE(r.Changed());
  EXPECT_THAT(r.replacement(), IsFloat64Min(p0, p1));
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
