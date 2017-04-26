// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/js-graph.h"
#include "src/compiler/js-typed-lowering.h"
#include "src/compiler/machine-operator.h"
#include "src/compiler/node-properties.h"
#include "src/compiler/opcodes.h"
#include "src/compiler/operator-properties.h"
#include "src/compiler/typer.h"
#include "test/cctest/cctest.h"

using namespace v8::internal;
using namespace v8::internal::compiler;

#ifndef TEST_WITH_STRONG
#define TEST_WITH_STRONG(Name)                                                 \
  static void Test##Name();                                                    \
  static void TestWithStrong##Name(LanguageMode language_mode);                \
  CcTest register_test_##Name(Test##Name, __FILE__, #Name, NULL, true, true);  \
  static void Test##Name() {                                                   \
    TestWithStrong##Name(LanguageMode::SLOPPY);                                \
    TestWithStrong##Name(LanguageMode::STRONG);                                \
  }                                                                            \
  static void TestWithStrong##Name(LanguageMode language_mode)
#endif


class JSTypedLoweringTester : public HandleAndZoneScope {
 public:
  explicit JSTypedLoweringTester(int num_parameters = 0)
      : isolate(main_isolate()),
        binop(NULL),
        unop(NULL),
        javascript(main_zone()),
        machine(main_zone()),
        simplified(main_zone()),
        common(main_zone()),
        graph(main_zone()),
        typer(main_isolate(), &graph),
        context_node(NULL) {
    graph.SetStart(graph.NewNode(common.Start(num_parameters)));
    graph.SetEnd(graph.NewNode(common.End(1)));
    typer.Run();
  }

  Isolate* isolate;
  const Operator* binop;
  const Operator* unop;
  JSOperatorBuilder javascript;
  MachineOperatorBuilder machine;
  SimplifiedOperatorBuilder simplified;
  CommonOperatorBuilder common;
  Graph graph;
  Typer typer;
  Node* context_node;

  Node* Parameter(Type* t, int32_t index = 0) {
    Node* n = graph.NewNode(common.Parameter(index), graph.start());
    NodeProperties::SetBounds(n, Bounds(Type::None(), t));
    return n;
  }

  Node* UndefinedConstant() {
    Unique<HeapObject> unique = Unique<HeapObject>::CreateImmovable(
        isolate->factory()->undefined_value());
    return graph.NewNode(common.HeapConstant(unique));
  }

  Node* HeapConstant(Handle<HeapObject> constant) {
    Unique<HeapObject> unique =
        Unique<HeapObject>::CreateUninitialized(constant);
    return graph.NewNode(common.HeapConstant(unique));
  }

  Node* EmptyFrameState(Node* context) {
    Node* parameters = graph.NewNode(common.StateValues(0));
    Node* locals = graph.NewNode(common.StateValues(0));
    Node* stack = graph.NewNode(common.StateValues(0));

    Node* state_node = graph.NewNode(
        common.FrameState(BailoutId::None(), OutputFrameStateCombine::Ignore(),
                          nullptr),
        parameters, locals, stack, context, UndefinedConstant());

    return state_node;
  }

  Node* reduce(Node* node) {
    JSGraph jsgraph(main_isolate(), &graph, &common, &javascript, &machine);
    // TODO(titzer): mock the GraphReducer here for better unit testing.
    GraphReducer graph_reducer(main_zone(), &graph);
    JSTypedLowering reducer(&graph_reducer, &jsgraph, main_zone());
    Reduction reduction = reducer.Reduce(node);
    if (reduction.Changed()) return reduction.replacement();
    return node;
  }

  Node* start() { return graph.start(); }

  Node* context() {
    if (context_node == NULL) {
      context_node = graph.NewNode(common.Parameter(-1), graph.start());
    }
    return context_node;
  }

  Node* control() { return start(); }

  void CheckPureBinop(IrOpcode::Value expected, Node* node) {
    CHECK_EQ(expected, node->opcode());
    CHECK_EQ(2, node->InputCount());  // should not have context, effect, etc.
  }

  void CheckPureBinop(const Operator* expected, Node* node) {
    CHECK_EQ(expected->opcode(), node->op()->opcode());
    CHECK_EQ(2, node->InputCount());  // should not have context, effect, etc.
  }

  Node* ReduceUnop(const Operator* op, Type* input_type) {
    return reduce(Unop(op, Parameter(input_type)));
  }

  Node* ReduceBinop(const Operator* op, Type* left_type, Type* right_type) {
    return reduce(Binop(op, Parameter(left_type, 0), Parameter(right_type, 1)));
  }

  Node* Binop(const Operator* op, Node* left, Node* right) {
    // JS binops also require context, effect, and control
    if (OperatorProperties::GetFrameStateInputCount(op) == 1) {
      return graph.NewNode(op, left, right, context(),
                           EmptyFrameState(context()), start(), control());
    } else if (OperatorProperties::GetFrameStateInputCount(op) == 2) {
      return graph.NewNode(op, left, right, context(),
                           EmptyFrameState(context()),
                           EmptyFrameState(context()), start(), control());
    } else {
      return graph.NewNode(op, left, right, context(), start(), control());
    }
  }

  Node* Unop(const Operator* op, Node* input) {
    // JS unops also require context, effect, and control
    if (OperatorProperties::GetFrameStateInputCount(op) > 0) {
      DCHECK(OperatorProperties::GetFrameStateInputCount(op) == 1);
      return graph.NewNode(op, input, context(), EmptyFrameState(context()),
                           start(), control());
    } else {
      return graph.NewNode(op, input, context(), start(), control());
    }
  }

  Node* UseForEffect(Node* node) {
    // TODO(titzer): use EffectPhi after fixing EffectCount
    if (OperatorProperties::GetFrameStateInputCount(javascript.ToNumber()) >
        0) {
      DCHECK(OperatorProperties::GetFrameStateInputCount(
                 javascript.ToNumber()) == 1);
      return graph.NewNode(javascript.ToNumber(), node, context(),
                           EmptyFrameState(context()), node, control());
    } else {
      return graph.NewNode(javascript.ToNumber(), node, context(), node,
                           control());
    }
  }

  void CheckEffectInput(Node* effect, Node* use) {
    CHECK_EQ(effect, NodeProperties::GetEffectInput(use));
  }

  void CheckInt32Constant(int32_t expected, Node* result) {
    CHECK_EQ(IrOpcode::kInt32Constant, result->opcode());
    CHECK_EQ(expected, OpParameter<int32_t>(result));
  }

  void CheckNumberConstant(double expected, Node* result) {
    CHECK_EQ(IrOpcode::kNumberConstant, result->opcode());
    CHECK_EQ(expected, OpParameter<double>(result));
  }

  void CheckNaN(Node* result) {
    CHECK_EQ(IrOpcode::kNumberConstant, result->opcode());
    double value = OpParameter<double>(result);
    CHECK(std::isnan(value));
  }

  void CheckTrue(Node* result) {
    CheckHandle(isolate->factory()->true_value(), result);
  }

  void CheckFalse(Node* result) {
    CheckHandle(isolate->factory()->false_value(), result);
  }

  void CheckHandle(Handle<Object> expected, Node* result) {
    CHECK_EQ(IrOpcode::kHeapConstant, result->opcode());
    Handle<Object> value = OpParameter<Unique<Object> >(result).handle();
    CHECK_EQ(*expected, *value);
  }
};

static Type* kStringTypes[] = {Type::InternalizedString(), Type::OtherString(),
                               Type::String()};


static Type* kInt32Types[] = {Type::UnsignedSmall(), Type::Negative32(),
                              Type::Unsigned31(),    Type::SignedSmall(),
                              Type::Signed32(),      Type::Unsigned32(),
                              Type::Integral32()};


static Type* kNumberTypes[] = {
    Type::UnsignedSmall(), Type::Negative32(),  Type::Unsigned31(),
    Type::SignedSmall(),   Type::Signed32(),    Type::Unsigned32(),
    Type::Integral32(),    Type::MinusZero(),   Type::NaN(),
    Type::OrderedNumber(), Type::PlainNumber(), Type::Number()};


static Type* kJSTypes[] = {Type::Undefined(), Type::Null(),   Type::Boolean(),
                           Type::Number(),    Type::String(), Type::Object()};


static Type* I32Type(bool is_signed) {
  return is_signed ? Type::Signed32() : Type::Unsigned32();
}


static IrOpcode::Value NumberToI32(bool is_signed) {
  return is_signed ? IrOpcode::kNumberToInt32 : IrOpcode::kNumberToUint32;
}


// TODO(turbofan): Lowering of StringAdd is disabled for now.
#if 0
TEST_WITH_STRONG(StringBinops) {
  JSTypedLoweringTester R;

  for (size_t i = 0; i < arraysize(kStringTypes); ++i) {
    Node* p0 = R.Parameter(kStringTypes[i], 0);

    for (size_t j = 0; j < arraysize(kStringTypes); ++j) {
      Node* p1 = R.Parameter(kStringTypes[j], 1);

      Node* add = R.Binop(R.javascript.Add(language_mode), p0, p1);
      Node* r = R.reduce(add);

      R.CheckPureBinop(IrOpcode::kStringAdd, r);
      CHECK_EQ(p0, r->InputAt(0));
      CHECK_EQ(p1, r->InputAt(1));
    }
  }
}
#endif


TEST_WITH_STRONG(AddNumber1) {
  JSTypedLoweringTester R;
  for (size_t i = 0; i < arraysize(kNumberTypes); ++i) {
    Node* p0 = R.Parameter(kNumberTypes[i], 0);
    Node* p1 = R.Parameter(kNumberTypes[i], 1);
    Node* add = R.Binop(R.javascript.Add(language_mode), p0, p1);
    Node* r = R.reduce(add);

    R.CheckPureBinop(IrOpcode::kNumberAdd, r);
    CHECK_EQ(p0, r->InputAt(0));
    CHECK_EQ(p1, r->InputAt(1));
  }
}


TEST_WITH_STRONG(NumberBinops) {
  JSTypedLoweringTester R;
  const Operator* ops[] = {
      R.javascript.Add(language_mode),      R.simplified.NumberAdd(),
      R.javascript.Subtract(language_mode), R.simplified.NumberSubtract(),
      R.javascript.Multiply(language_mode), R.simplified.NumberMultiply(),
      R.javascript.Divide(language_mode),   R.simplified.NumberDivide(),
      R.javascript.Modulus(language_mode),  R.simplified.NumberModulus(),
  };

  for (size_t i = 0; i < arraysize(kNumberTypes); ++i) {
    Node* p0 = R.Parameter(kNumberTypes[i], 0);

    for (size_t j = 0; j < arraysize(kNumberTypes); ++j) {
      Node* p1 = R.Parameter(kNumberTypes[j], 1);

      for (size_t k = 0; k < arraysize(ops); k += 2) {
        Node* add = R.Binop(ops[k], p0, p1);
        Node* r = R.reduce(add);

        R.CheckPureBinop(ops[k + 1], r);
        CHECK_EQ(p0, r->InputAt(0));
        CHECK_EQ(p1, r->InputAt(1));
      }
    }
  }
}


static void CheckToI32(Node* old_input, Node* new_input, bool is_signed) {
  Type* old_type = NodeProperties::GetBounds(old_input).upper;
  Type* new_type = NodeProperties::GetBounds(new_input).upper;
  Type* expected_type = I32Type(is_signed);
  CHECK(new_type->Is(expected_type));
  if (old_type->Is(expected_type)) {
    CHECK_EQ(old_input, new_input);
  } else if (new_input->opcode() == IrOpcode::kNumberConstant) {
    double v = OpParameter<double>(new_input);
    double e = static_cast<double>(is_signed ? FastD2I(v) : FastD2UI(v));
    CHECK_EQ(e, v);
  }
}


// A helper class for testing lowering of bitwise shift operators.
class JSBitwiseShiftTypedLoweringTester : public JSTypedLoweringTester {
 public:
  explicit JSBitwiseShiftTypedLoweringTester(LanguageMode language_mode)
      : JSTypedLoweringTester(), language_mode_(language_mode) {
    int i = 0;
    set(i++, javascript.ShiftLeft(language_mode_), true);
    set(i++, simplified.NumberShiftLeft(), false);
    set(i++, javascript.ShiftRight(language_mode_), true);
    set(i++, simplified.NumberShiftRight(), false);
    set(i++, javascript.ShiftRightLogical(language_mode_), false);
    set(i++, simplified.NumberShiftRightLogical(), false);
  }
  static const int kNumberOps = 6;
  const Operator* ops[kNumberOps];
  bool signedness[kNumberOps];

 private:
  LanguageMode language_mode_;
  void set(int idx, const Operator* op, bool s) {
    ops[idx] = op;
    signedness[idx] = s;
  }
};


TEST(Int32BitwiseShifts) {
  JSBitwiseShiftTypedLoweringTester R(LanguageMode::SLOPPY);

  Type* types[] = {
      Type::SignedSmall(), Type::UnsignedSmall(), Type::Negative32(),
      Type::Unsigned31(),  Type::Unsigned32(),    Type::Signed32(),
      Type::MinusZero(),   Type::NaN(),           Type::Undefined(),
      Type::Null(),        Type::Boolean(),       Type::Number(),
      Type::PlainNumber(), Type::String()};

  for (size_t i = 0; i < arraysize(types); ++i) {
    Node* p0 = R.Parameter(types[i], 0);

    for (size_t j = 0; j < arraysize(types); ++j) {
      Node* p1 = R.Parameter(types[j], 1);

      for (int k = 0; k < R.kNumberOps; k += 2) {
        Node* add = R.Binop(R.ops[k], p0, p1);
        Node* r = R.reduce(add);

        R.CheckPureBinop(R.ops[k + 1], r);
        Node* r0 = r->InputAt(0);
        Node* r1 = r->InputAt(1);

        CheckToI32(p0, r0, R.signedness[k]);
        CheckToI32(p1, r1, false);
      }
    }
  }
}


// A helper class for testing lowering of bitwise operators.
class JSBitwiseTypedLoweringTester : public JSTypedLoweringTester {
 public:
  explicit JSBitwiseTypedLoweringTester(LanguageMode language_mode)
      : JSTypedLoweringTester(), language_mode_(language_mode) {
    int i = 0;
    set(i++, javascript.BitwiseOr(language_mode_), true);
    set(i++, machine.Word32Or(), true);
    set(i++, javascript.BitwiseXor(language_mode_), true);
    set(i++, machine.Word32Xor(), true);
    set(i++, javascript.BitwiseAnd(language_mode_), true);
    set(i++, machine.Word32And(), true);
  }
  static const int kNumberOps = 6;
  const Operator* ops[kNumberOps];
  bool signedness[kNumberOps];

 private:
  LanguageMode language_mode_;
  void set(int idx, const Operator* op, bool s) {
    ops[idx] = op;
    signedness[idx] = s;
  }
};


TEST(Int32BitwiseBinops) {
  JSBitwiseTypedLoweringTester R(LanguageMode::SLOPPY);

  Type* types[] = {
      Type::SignedSmall(),   Type::UnsignedSmall(), Type::Unsigned32(),
      Type::Signed32(),      Type::MinusZero(),     Type::NaN(),
      Type::OrderedNumber(), Type::PlainNumber(),   Type::Undefined(),
      Type::Null(),          Type::Boolean(),       Type::Number(),
      Type::String()};

  for (size_t i = 0; i < arraysize(types); ++i) {
    Node* p0 = R.Parameter(types[i], 0);

    for (size_t j = 0; j < arraysize(types); ++j) {
      Node* p1 = R.Parameter(types[j], 1);

      for (int k = 0; k < R.kNumberOps; k += 2) {
        Node* add = R.Binop(R.ops[k], p0, p1);
        Node* r = R.reduce(add);

        R.CheckPureBinop(R.ops[k + 1], r);

        CheckToI32(p0, r->InputAt(0), R.signedness[k]);
        CheckToI32(p1, r->InputAt(1), R.signedness[k + 1]);
      }
    }
  }
}


TEST(JSToNumber1) {
  JSTypedLoweringTester R;
  const Operator* ton = R.javascript.ToNumber();

  for (size_t i = 0; i < arraysize(kNumberTypes); i++) {  // ToNumber(number)
    Node* r = R.ReduceUnop(ton, kNumberTypes[i]);
    CHECK_EQ(IrOpcode::kParameter, r->opcode());
  }

  {  // ToNumber(undefined)
    Node* r = R.ReduceUnop(ton, Type::Undefined());
    R.CheckNaN(r);
  }

  {  // ToNumber(null)
    Node* r = R.ReduceUnop(ton, Type::Null());
    R.CheckNumberConstant(0.0, r);
  }
}


TEST(JSToNumber_replacement) {
  JSTypedLoweringTester R;

  Type* types[] = {Type::Null(), Type::Undefined(), Type::Number()};

  for (size_t i = 0; i < arraysize(types); i++) {
    Node* n = R.Parameter(types[i]);
    Node* c =
        R.graph.NewNode(R.javascript.ToNumber(), n, R.context(),
                        R.EmptyFrameState(R.context()), R.start(), R.start());
    Node* effect_use = R.UseForEffect(c);
    Node* add = R.graph.NewNode(R.simplified.ReferenceEqual(Type::Any()), n, c);

    R.CheckEffectInput(c, effect_use);
    Node* r = R.reduce(c);

    if (types[i]->Is(Type::Number())) {
      CHECK_EQ(n, r);
    } else {
      CHECK_EQ(IrOpcode::kNumberConstant, r->opcode());
    }

    CHECK_EQ(n, add->InputAt(0));
    CHECK_EQ(r, add->InputAt(1));
    R.CheckEffectInput(R.start(), effect_use);
  }
}


TEST(JSToNumberOfConstant) {
  JSTypedLoweringTester R;

  const Operator* ops[] = {
      R.common.NumberConstant(0), R.common.NumberConstant(-1),
      R.common.NumberConstant(0.1), R.common.Int32Constant(1177),
      R.common.Float64Constant(0.99)};

  for (size_t i = 0; i < arraysize(ops); i++) {
    Node* n = R.graph.NewNode(ops[i]);
    Node* convert = R.Unop(R.javascript.ToNumber(), n);
    Node* r = R.reduce(convert);
    // Note that either outcome below is correct. It only depends on whether
    // the types of constants are eagerly computed or only computed by the
    // typing pass.
    if (NodeProperties::GetBounds(n).upper->Is(Type::Number())) {
      // If number constants are eagerly typed, then reduction should
      // remove the ToNumber.
      CHECK_EQ(n, r);
    } else {
      // Otherwise, type-based lowering should only look at the type, and
      // *not* try to constant fold.
      CHECK_EQ(convert, r);
    }
  }
}


TEST(JSToNumberOfNumberOrOtherPrimitive) {
  JSTypedLoweringTester R;
  Type* others[] = {Type::Undefined(), Type::Null(), Type::Boolean(),
                    Type::String()};

  for (size_t i = 0; i < arraysize(others); i++) {
    Type* t = Type::Union(Type::Number(), others[i], R.main_zone());
    Node* r = R.ReduceUnop(R.javascript.ToNumber(), t);
    CHECK_EQ(IrOpcode::kJSToNumber, r->opcode());
  }
}


TEST(JSToString1) {
  JSTypedLoweringTester R;

  for (size_t i = 0; i < arraysize(kStringTypes); i++) {
    Node* r = R.ReduceUnop(R.javascript.ToString(), kStringTypes[i]);
    CHECK_EQ(IrOpcode::kParameter, r->opcode());
  }

  const Operator* op = R.javascript.ToString();

  {  // ToString(undefined) => "undefined"
    Node* r = R.ReduceUnop(op, Type::Undefined());
    R.CheckHandle(R.isolate->factory()->undefined_string(), r);
  }

  {  // ToString(null) => "null"
    Node* r = R.ReduceUnop(op, Type::Null());
    R.CheckHandle(R.isolate->factory()->null_string(), r);
  }

  {  // ToString(boolean)
    Node* r = R.ReduceUnop(op, Type::Boolean());
    // TODO(titzer): could be a branch
    CHECK_EQ(IrOpcode::kJSToString, r->opcode());
  }

  {  // ToString(number)
    Node* r = R.ReduceUnop(op, Type::Number());
    // TODO(titzer): could remove effects
    CHECK_EQ(IrOpcode::kJSToString, r->opcode());
  }

  {  // ToString(string)
    Node* r = R.ReduceUnop(op, Type::String());
    CHECK_EQ(IrOpcode::kParameter, r->opcode());  // No-op
  }

  {  // ToString(object)
    Node* r = R.ReduceUnop(op, Type::Object());
    CHECK_EQ(IrOpcode::kJSToString, r->opcode());  // No reduction.
  }
}


TEST(JSToString_replacement) {
  JSTypedLoweringTester R;

  Type* types[] = {Type::Null(), Type::Undefined(), Type::String()};

  for (size_t i = 0; i < arraysize(types); i++) {
    Node* n = R.Parameter(types[i]);
    Node* c = R.graph.NewNode(R.javascript.ToString(), n, R.context(),
                              R.start(), R.start());
    Node* effect_use = R.UseForEffect(c);
    Node* add = R.graph.NewNode(R.simplified.ReferenceEqual(Type::Any()), n, c);

    R.CheckEffectInput(c, effect_use);
    Node* r = R.reduce(c);

    if (types[i]->Is(Type::String())) {
      CHECK_EQ(n, r);
    } else {
      CHECK_EQ(IrOpcode::kHeapConstant, r->opcode());
    }

    CHECK_EQ(n, add->InputAt(0));
    CHECK_EQ(r, add->InputAt(1));
    R.CheckEffectInput(R.start(), effect_use);
  }
}


TEST_WITH_STRONG(StringComparison) {
  JSTypedLoweringTester R;

  const Operator* ops[] = {
      R.javascript.LessThan(language_mode), R.simplified.StringLessThan(),
      R.javascript.LessThanOrEqual(language_mode),
      R.simplified.StringLessThanOrEqual(),
      R.javascript.GreaterThan(language_mode), R.simplified.StringLessThan(),
      R.javascript.GreaterThanOrEqual(language_mode),
      R.simplified.StringLessThanOrEqual()};

  for (size_t i = 0; i < arraysize(kStringTypes); i++) {
    Node* p0 = R.Parameter(kStringTypes[i], 0);
    for (size_t j = 0; j < arraysize(kStringTypes); j++) {
      Node* p1 = R.Parameter(kStringTypes[j], 1);

      for (size_t k = 0; k < arraysize(ops); k += 2) {
        Node* cmp = R.Binop(ops[k], p0, p1);
        Node* r = R.reduce(cmp);

        R.CheckPureBinop(ops[k + 1], r);
        if (k >= 4) {
          // GreaterThan and GreaterThanOrEqual commute the inputs
          // and use the LessThan and LessThanOrEqual operators.
          CHECK_EQ(p1, r->InputAt(0));
          CHECK_EQ(p0, r->InputAt(1));
        } else {
          CHECK_EQ(p0, r->InputAt(0));
          CHECK_EQ(p1, r->InputAt(1));
        }
      }
    }
  }
}


static void CheckIsConvertedToNumber(Node* val, Node* converted) {
  if (NodeProperties::GetBounds(val).upper->Is(Type::Number())) {
    CHECK_EQ(val, converted);
  } else if (NodeProperties::GetBounds(val).upper->Is(Type::Boolean())) {
    CHECK_EQ(IrOpcode::kBooleanToNumber, converted->opcode());
    CHECK_EQ(val, converted->InputAt(0));
  } else {
    if (converted->opcode() == IrOpcode::kNumberConstant) return;
    CHECK_EQ(IrOpcode::kJSToNumber, converted->opcode());
    CHECK_EQ(val, converted->InputAt(0));
  }
}


TEST_WITH_STRONG(NumberComparison) {
  JSTypedLoweringTester R;

  const Operator* ops[] = {
      R.javascript.LessThan(language_mode), R.simplified.NumberLessThan(),
      R.javascript.LessThanOrEqual(language_mode),
      R.simplified.NumberLessThanOrEqual(),
      R.javascript.GreaterThan(language_mode), R.simplified.NumberLessThan(),
      R.javascript.GreaterThanOrEqual(language_mode),
      R.simplified.NumberLessThanOrEqual()};

  Node* const p0 = R.Parameter(Type::Number(), 0);
  Node* const p1 = R.Parameter(Type::Number(), 1);

  for (size_t k = 0; k < arraysize(ops); k += 2) {
    Node* cmp = R.Binop(ops[k], p0, p1);
    Node* r = R.reduce(cmp);

    R.CheckPureBinop(ops[k + 1], r);
    if (k >= 4) {
      // GreaterThan and GreaterThanOrEqual commute the inputs
      // and use the LessThan and LessThanOrEqual operators.
      CheckIsConvertedToNumber(p1, r->InputAt(0));
      CheckIsConvertedToNumber(p0, r->InputAt(1));
    } else {
      CheckIsConvertedToNumber(p0, r->InputAt(0));
      CheckIsConvertedToNumber(p1, r->InputAt(1));
    }
  }
}


TEST_WITH_STRONG(MixedComparison1) {
  JSTypedLoweringTester R;

  Type* types[] = {Type::Number(), Type::String(),
                   Type::Union(Type::Number(), Type::String(), R.main_zone())};

  for (size_t i = 0; i < arraysize(types); i++) {
    Node* p0 = R.Parameter(types[i], 0);

    for (size_t j = 0; j < arraysize(types); j++) {
      Node* p1 = R.Parameter(types[j], 1);
      {
        const Operator* less_than = R.javascript.LessThan(language_mode);
        Node* cmp = R.Binop(less_than, p0, p1);
        Node* r = R.reduce(cmp);
        if (types[i]->Is(Type::String()) && types[j]->Is(Type::String())) {
          R.CheckPureBinop(R.simplified.StringLessThan(), r);
        } else if ((types[i]->Is(Type::Number()) &&
                    types[j]->Is(Type::Number())) ||
                   (!is_strong(language_mode) &&
                    (!types[i]->Maybe(Type::String()) ||
                     !types[j]->Maybe(Type::String())))) {
          R.CheckPureBinop(R.simplified.NumberLessThan(), r);
        } else {
          // No reduction of mixed types.
          CHECK_EQ(r->op(), less_than);
        }
      }
    }
  }
}


TEST_WITH_STRONG(RemoveToNumberEffects) {
  JSTypedLoweringTester R;

  Node* effect_use = NULL;
  for (int i = 0; i < 10; i++) {
    Node* p0 = R.Parameter(Type::Number());
    Node* ton = R.Unop(R.javascript.ToNumber(), p0);
    Node* frame_state = R.EmptyFrameState(R.context());
    effect_use = NULL;

    switch (i) {
      case 0:
        DCHECK(OperatorProperties::GetFrameStateInputCount(
                   R.javascript.ToNumber()) == 1);
        effect_use = R.graph.NewNode(R.javascript.ToNumber(), p0, R.context(),
                                     frame_state, ton, R.start());
        break;
      case 1:
        DCHECK(OperatorProperties::GetFrameStateInputCount(
                   R.javascript.ToNumber()) == 1);
        effect_use = R.graph.NewNode(R.javascript.ToNumber(), ton, R.context(),
                                     frame_state, ton, R.start());
        break;
      case 2:
        effect_use = R.graph.NewNode(R.common.EffectPhi(1), ton, R.start());
      case 3:
        effect_use = R.graph.NewNode(R.javascript.Add(language_mode), ton, ton,
                                     R.context(), frame_state, frame_state, ton,
                                     R.start());
        break;
      case 4:
        effect_use = R.graph.NewNode(R.javascript.Add(language_mode), p0, p0,
                                     R.context(), frame_state, frame_state, ton,
                                     R.start());
        break;
      case 5:
        effect_use = R.graph.NewNode(R.common.Return(), p0, ton, R.start());
        break;
      case 6:
        effect_use = R.graph.NewNode(R.common.Return(), ton, ton, R.start());
    }

    R.CheckEffectInput(R.start(), ton);
    if (effect_use != NULL) R.CheckEffectInput(ton, effect_use);

    Node* r = R.reduce(ton);
    CHECK_EQ(p0, r);
    CHECK_NE(R.start(), r);

    if (effect_use != NULL) {
      R.CheckEffectInput(R.start(), effect_use);
      // Check that value uses of ToNumber() do not go to start().
      for (int i = 0; i < effect_use->op()->ValueInputCount(); i++) {
        CHECK_NE(R.start(), effect_use->InputAt(i));
      }
    }
  }

  CHECK(!effect_use);  // should have done all cases above.
}


// Helper class for testing the reduction of a single binop.
class BinopEffectsTester {
 public:
  explicit BinopEffectsTester(const Operator* op, Type* t0, Type* t1)
      : R(),
        p0(R.Parameter(t0, 0)),
        p1(R.Parameter(t1, 1)),
        binop(R.Binop(op, p0, p1)),
        effect_use(R.graph.NewNode(R.common.EffectPhi(1), binop, R.start())) {
    // Effects should be ordered start -> binop -> effect_use
    R.CheckEffectInput(R.start(), binop);
    R.CheckEffectInput(binop, effect_use);
    result = R.reduce(binop);
  }

  JSTypedLoweringTester R;
  Node* p0;
  Node* p1;
  Node* binop;
  Node* effect_use;
  Node* result;

  void CheckEffectsRemoved() { R.CheckEffectInput(R.start(), effect_use); }

  void CheckEffectOrdering(Node* n0) {
    R.CheckEffectInput(R.start(), n0);
    R.CheckEffectInput(n0, effect_use);
  }

  void CheckEffectOrdering(Node* n0, Node* n1) {
    R.CheckEffectInput(R.start(), n0);
    R.CheckEffectInput(n0, n1);
    R.CheckEffectInput(n1, effect_use);
  }

  Node* CheckConvertedInput(IrOpcode::Value opcode, int which, bool effects) {
    return CheckConverted(opcode, result->InputAt(which), effects);
  }

  Node* CheckConverted(IrOpcode::Value opcode, Node* node, bool effects) {
    CHECK_EQ(opcode, node->opcode());
    if (effects) {
      CHECK_LT(0, node->op()->EffectInputCount());
    } else {
      CHECK_EQ(0, node->op()->EffectInputCount());
    }
    return node;
  }

  Node* CheckNoOp(int which) {
    CHECK_EQ(which == 0 ? p0 : p1, result->InputAt(which));
    return result->InputAt(which);
  }
};


// Helper function for strict and non-strict equality reductions.
void CheckEqualityReduction(JSTypedLoweringTester* R, bool strict, Node* l,
                            Node* r, IrOpcode::Value expected) {
  for (int j = 0; j < 2; j++) {
    Node* p0 = j == 0 ? l : r;
    Node* p1 = j == 1 ? l : r;

    {
      Node* eq = strict ? R->graph.NewNode(R->javascript.StrictEqual(), p0, p1)
                        : R->Binop(R->javascript.Equal(), p0, p1);
      Node* r = R->reduce(eq);
      R->CheckPureBinop(expected, r);
    }

    {
      Node* ne = strict
                     ? R->graph.NewNode(R->javascript.StrictNotEqual(), p0, p1)
                     : R->Binop(R->javascript.NotEqual(), p0, p1);
      Node* n = R->reduce(ne);
      CHECK_EQ(IrOpcode::kBooleanNot, n->opcode());
      Node* r = n->InputAt(0);
      R->CheckPureBinop(expected, r);
    }
  }
}


TEST(EqualityForNumbers) {
  JSTypedLoweringTester R;

  Type* simple_number_types[] = {Type::UnsignedSmall(), Type::SignedSmall(),
                                 Type::Signed32(), Type::Unsigned32(),
                                 Type::Number()};


  for (size_t i = 0; i < arraysize(simple_number_types); ++i) {
    Node* p0 = R.Parameter(simple_number_types[i], 0);

    for (size_t j = 0; j < arraysize(simple_number_types); ++j) {
      Node* p1 = R.Parameter(simple_number_types[j], 1);

      CheckEqualityReduction(&R, true, p0, p1, IrOpcode::kNumberEqual);
      CheckEqualityReduction(&R, false, p0, p1, IrOpcode::kNumberEqual);
    }
  }
}


TEST(StrictEqualityForRefEqualTypes) {
  JSTypedLoweringTester R;

  Type* types[] = {Type::Undefined(), Type::Null(), Type::Boolean(),
                   Type::Object(), Type::Receiver()};

  Node* p0 = R.Parameter(Type::Any());
  for (size_t i = 0; i < arraysize(types); i++) {
    Node* p1 = R.Parameter(types[i]);
    CheckEqualityReduction(&R, true, p0, p1, IrOpcode::kReferenceEqual);
  }
  // TODO(titzer): Equal(RefEqualTypes)
}


TEST(StringEquality) {
  JSTypedLoweringTester R;
  Node* p0 = R.Parameter(Type::String());
  Node* p1 = R.Parameter(Type::String());

  CheckEqualityReduction(&R, true, p0, p1, IrOpcode::kStringEqual);
  CheckEqualityReduction(&R, false, p0, p1, IrOpcode::kStringEqual);
}


TEST_WITH_STRONG(RemovePureNumberBinopEffects) {
  JSTypedLoweringTester R;

  const Operator* ops[] = {
      R.javascript.Equal(),           R.simplified.NumberEqual(),
      R.javascript.Add(language_mode),      R.simplified.NumberAdd(),
      R.javascript.Subtract(language_mode), R.simplified.NumberSubtract(),
      R.javascript.Multiply(language_mode), R.simplified.NumberMultiply(),
      R.javascript.Divide(language_mode),   R.simplified.NumberDivide(),
      R.javascript.Modulus(language_mode),  R.simplified.NumberModulus(),
      R.javascript.LessThan(language_mode), R.simplified.NumberLessThan(),
      R.javascript.LessThanOrEqual(language_mode),
      R.simplified.NumberLessThanOrEqual(),
  };

  for (size_t j = 0; j < arraysize(ops); j += 2) {
    BinopEffectsTester B(ops[j], Type::Number(), Type::Number());
    CHECK_EQ(ops[j + 1]->opcode(), B.result->op()->opcode());

    B.R.CheckPureBinop(B.result->opcode(), B.result);

    B.CheckNoOp(0);
    B.CheckNoOp(1);

    B.CheckEffectsRemoved();
  }
}


TEST(OrderNumberBinopEffects1) {
  JSTypedLoweringTester R;

  const Operator* ops[] = {
      R.javascript.Subtract(LanguageMode::SLOPPY),
      R.simplified.NumberSubtract(),
      R.javascript.Multiply(LanguageMode::SLOPPY),
      R.simplified.NumberMultiply(),
      R.javascript.Divide(LanguageMode::SLOPPY),
      R.simplified.NumberDivide(),
  };

  for (size_t j = 0; j < arraysize(ops); j += 2) {
    BinopEffectsTester B(ops[j], Type::Symbol(), Type::Symbol());
    CHECK_EQ(ops[j + 1]->opcode(), B.result->op()->opcode());

    Node* i0 = B.CheckConvertedInput(IrOpcode::kJSToNumber, 0, true);
    Node* i1 = B.CheckConvertedInput(IrOpcode::kJSToNumber, 1, true);

    CHECK_EQ(B.p0, i0->InputAt(0));
    CHECK_EQ(B.p1, i1->InputAt(0));

    // Effects should be ordered start -> i0 -> i1 -> effect_use
    B.CheckEffectOrdering(i0, i1);
  }
}


TEST(OrderNumberBinopEffects2) {
  JSTypedLoweringTester R;

  const Operator* ops[] = {
      R.javascript.Add(LanguageMode::SLOPPY),
      R.simplified.NumberAdd(),
      R.javascript.Subtract(LanguageMode::SLOPPY),
      R.simplified.NumberSubtract(),
      R.javascript.Multiply(LanguageMode::SLOPPY),
      R.simplified.NumberMultiply(),
      R.javascript.Divide(LanguageMode::SLOPPY),
      R.simplified.NumberDivide(),
  };

  for (size_t j = 0; j < arraysize(ops); j += 2) {
    BinopEffectsTester B(ops[j], Type::Number(), Type::Symbol());

    Node* i0 = B.CheckNoOp(0);
    Node* i1 = B.CheckConvertedInput(IrOpcode::kJSToNumber, 1, true);

    CHECK_EQ(B.p0, i0);
    CHECK_EQ(B.p1, i1->InputAt(0));

    // Effects should be ordered start -> i1 -> effect_use
    B.CheckEffectOrdering(i1);
  }

  for (size_t j = 0; j < arraysize(ops); j += 2) {
    BinopEffectsTester B(ops[j], Type::Symbol(), Type::Number());

    Node* i0 = B.CheckConvertedInput(IrOpcode::kJSToNumber, 0, true);
    Node* i1 = B.CheckNoOp(1);

    CHECK_EQ(B.p0, i0->InputAt(0));
    CHECK_EQ(B.p1, i1);

    // Effects should be ordered start -> i0 -> effect_use
    B.CheckEffectOrdering(i0);
  }
}


TEST(OrderCompareEffects) {
  JSTypedLoweringTester R;

  const Operator* ops[] = {
      R.javascript.GreaterThan(LanguageMode::SLOPPY),
      R.simplified.NumberLessThan(),
      R.javascript.GreaterThanOrEqual(LanguageMode::SLOPPY),
      R.simplified.NumberLessThanOrEqual(),
  };

  for (size_t j = 0; j < arraysize(ops); j += 2) {
    BinopEffectsTester B(ops[j], Type::Symbol(), Type::String());
    CHECK_EQ(ops[j + 1]->opcode(), B.result->op()->opcode());

    Node* i0 = B.CheckConvertedInput(IrOpcode::kJSToNumber, 0, true);
    Node* i1 = B.CheckConvertedInput(IrOpcode::kJSToNumber, 1, true);

    // Inputs should be commuted.
    CHECK_EQ(B.p1, i0->InputAt(0));
    CHECK_EQ(B.p0, i1->InputAt(0));

    // But effects should be ordered start -> i1 -> effect_use
    B.CheckEffectOrdering(i1);
  }

  for (size_t j = 0; j < arraysize(ops); j += 2) {
    BinopEffectsTester B(ops[j], Type::Number(), Type::Symbol());

    Node* i0 = B.CheckConvertedInput(IrOpcode::kJSToNumber, 0, true);
    Node* i1 = B.result->InputAt(1);

    CHECK_EQ(B.p1, i0->InputAt(0));  // Should be commuted.
    CHECK_EQ(B.p0, i1);

    // Effects should be ordered start -> i1 -> effect_use
    B.CheckEffectOrdering(i0);
  }

  for (size_t j = 0; j < arraysize(ops); j += 2) {
    BinopEffectsTester B(ops[j], Type::Symbol(), Type::Number());

    Node* i0 = B.result->InputAt(0);
    Node* i1 = B.CheckConvertedInput(IrOpcode::kJSToNumber, 1, true);

    CHECK_EQ(B.p1, i0);  // Should be commuted.
    CHECK_EQ(B.p0, i1->InputAt(0));

    // Effects should be ordered start -> i0 -> effect_use
    B.CheckEffectOrdering(i1);
  }
}


TEST(Int32BinopEffects) {
  JSBitwiseTypedLoweringTester R(LanguageMode::SLOPPY);
  for (int j = 0; j < R.kNumberOps; j += 2) {
    bool signed_left = R.signedness[j], signed_right = R.signedness[j + 1];
    BinopEffectsTester B(R.ops[j], I32Type(signed_left), I32Type(signed_right));
    CHECK_EQ(R.ops[j + 1]->opcode(), B.result->op()->opcode());

    B.R.CheckPureBinop(B.result->opcode(), B.result);

    B.CheckNoOp(0);
    B.CheckNoOp(1);

    B.CheckEffectsRemoved();
  }

  for (int j = 0; j < R.kNumberOps; j += 2) {
    bool signed_left = R.signedness[j], signed_right = R.signedness[j + 1];
    BinopEffectsTester B(R.ops[j], Type::Number(), Type::Number());
    CHECK_EQ(R.ops[j + 1]->opcode(), B.result->op()->opcode());

    B.R.CheckPureBinop(B.result->opcode(), B.result);

    B.CheckConvertedInput(NumberToI32(signed_left), 0, false);
    B.CheckConvertedInput(NumberToI32(signed_right), 1, false);

    B.CheckEffectsRemoved();
  }

  for (int j = 0; j < R.kNumberOps; j += 2) {
    bool signed_left = R.signedness[j], signed_right = R.signedness[j + 1];
    BinopEffectsTester B(R.ops[j], Type::Number(), Type::Primitive());

    B.R.CheckPureBinop(B.result->opcode(), B.result);

    Node* i0 = B.CheckConvertedInput(NumberToI32(signed_left), 0, false);
    Node* i1 = B.CheckConvertedInput(NumberToI32(signed_right), 1, false);

    CHECK_EQ(B.p0, i0->InputAt(0));
    Node* ii1 = B.CheckConverted(IrOpcode::kJSToNumber, i1->InputAt(0), true);

    CHECK_EQ(B.p1, ii1->InputAt(0));

    B.CheckEffectOrdering(ii1);
  }

  for (int j = 0; j < R.kNumberOps; j += 2) {
    bool signed_left = R.signedness[j], signed_right = R.signedness[j + 1];
    BinopEffectsTester B(R.ops[j], Type::Primitive(), Type::Number());

    B.R.CheckPureBinop(B.result->opcode(), B.result);

    Node* i0 = B.CheckConvertedInput(NumberToI32(signed_left), 0, false);
    Node* i1 = B.CheckConvertedInput(NumberToI32(signed_right), 1, false);

    Node* ii0 = B.CheckConverted(IrOpcode::kJSToNumber, i0->InputAt(0), true);
    CHECK_EQ(B.p1, i1->InputAt(0));

    CHECK_EQ(B.p0, ii0->InputAt(0));

    B.CheckEffectOrdering(ii0);
  }

  for (int j = 0; j < R.kNumberOps; j += 2) {
    bool signed_left = R.signedness[j], signed_right = R.signedness[j + 1];
    BinopEffectsTester B(R.ops[j], Type::Primitive(), Type::Primitive());

    B.R.CheckPureBinop(B.result->opcode(), B.result);

    Node* i0 = B.CheckConvertedInput(NumberToI32(signed_left), 0, false);
    Node* i1 = B.CheckConvertedInput(NumberToI32(signed_right), 1, false);

    Node* ii0 = B.CheckConverted(IrOpcode::kJSToNumber, i0->InputAt(0), true);
    Node* ii1 = B.CheckConverted(IrOpcode::kJSToNumber, i1->InputAt(0), true);

    CHECK_EQ(B.p0, ii0->InputAt(0));
    CHECK_EQ(B.p1, ii1->InputAt(0));

    B.CheckEffectOrdering(ii0, ii1);
  }
}


TEST_WITH_STRONG(Int32AddNarrowing) {
  {
    JSBitwiseTypedLoweringTester R(language_mode);

    for (int o = 0; o < R.kNumberOps; o += 2) {
      for (size_t i = 0; i < arraysize(kInt32Types); i++) {
        Node* n0 = R.Parameter(kInt32Types[i]);
        for (size_t j = 0; j < arraysize(kInt32Types); j++) {
          Node* n1 = R.Parameter(kInt32Types[j]);
          Node* one = R.graph.NewNode(R.common.NumberConstant(1));

          for (int l = 0; l < 2; l++) {
            Node* add_node = R.Binop(R.simplified.NumberAdd(), n0, n1);
            Node* or_node =
                R.Binop(R.ops[o], l ? add_node : one, l ? one : add_node);
            Node* r = R.reduce(or_node);

            CHECK_EQ(R.ops[o + 1]->opcode(), r->op()->opcode());
            CHECK_EQ(IrOpcode::kNumberAdd, add_node->opcode());
          }
        }
      }
    }
  }
  {
    JSBitwiseShiftTypedLoweringTester R(language_mode);

    for (int o = 0; o < R.kNumberOps; o += 2) {
      for (size_t i = 0; i < arraysize(kInt32Types); i++) {
        Node* n0 = R.Parameter(kInt32Types[i]);
        for (size_t j = 0; j < arraysize(kInt32Types); j++) {
          Node* n1 = R.Parameter(kInt32Types[j]);
          Node* one = R.graph.NewNode(R.common.NumberConstant(1));

          for (int l = 0; l < 2; l++) {
            Node* add_node = R.Binop(R.simplified.NumberAdd(), n0, n1);
            Node* or_node =
                R.Binop(R.ops[o], l ? add_node : one, l ? one : add_node);
            Node* r = R.reduce(or_node);

            CHECK_EQ(R.ops[o + 1]->opcode(), r->op()->opcode());
            CHECK_EQ(IrOpcode::kNumberAdd, add_node->opcode());
          }
        }
      }
    }
  }
  {
    JSBitwiseTypedLoweringTester R(language_mode);

    for (int o = 0; o < R.kNumberOps; o += 2) {
      Node* n0 = R.Parameter(I32Type(R.signedness[o]));
      Node* n1 = R.Parameter(I32Type(R.signedness[o + 1]));
      Node* one = R.graph.NewNode(R.common.NumberConstant(1));

      Node* add_node = R.Binop(R.simplified.NumberAdd(), n0, n1);
      Node* or_node = R.Binop(R.ops[o], add_node, one);
      Node* other_use = R.Binop(R.simplified.NumberAdd(), add_node, one);
      Node* r = R.reduce(or_node);
      CHECK_EQ(R.ops[o + 1]->opcode(), r->op()->opcode());
      CHECK_EQ(IrOpcode::kNumberAdd, add_node->opcode());
      // Conversion to int32 should be done.
      CheckToI32(add_node, r->InputAt(0), R.signedness[o]);
      CheckToI32(one, r->InputAt(1), R.signedness[o + 1]);
      // The other use should also not be touched.
      CHECK_EQ(add_node, other_use->InputAt(0));
      CHECK_EQ(one, other_use->InputAt(1));
    }
  }
}


TEST_WITH_STRONG(Int32Comparisons) {
  JSTypedLoweringTester R;

  struct Entry {
    const Operator* js_op;
    const Operator* uint_op;
    const Operator* int_op;
    const Operator* num_op;
    bool commute;
  };

  Entry ops[] = {
      {R.javascript.LessThan(language_mode), R.machine.Uint32LessThan(),
       R.machine.Int32LessThan(), R.simplified.NumberLessThan(), false},
      {R.javascript.LessThanOrEqual(language_mode),
       R.machine.Uint32LessThanOrEqual(), R.machine.Int32LessThanOrEqual(),
       R.simplified.NumberLessThanOrEqual(), false},
      {R.javascript.GreaterThan(language_mode), R.machine.Uint32LessThan(),
       R.machine.Int32LessThan(), R.simplified.NumberLessThan(), true},
      {R.javascript.GreaterThanOrEqual(language_mode),
       R.machine.Uint32LessThanOrEqual(), R.machine.Int32LessThanOrEqual(),
       R.simplified.NumberLessThanOrEqual(), true}
  };

  for (size_t o = 0; o < arraysize(ops); o++) {
    for (size_t i = 0; i < arraysize(kNumberTypes); i++) {
      Type* t0 = kNumberTypes[i];
      Node* p0 = R.Parameter(t0, 0);

      for (size_t j = 0; j < arraysize(kNumberTypes); j++) {
        Type* t1 = kNumberTypes[j];
        Node* p1 = R.Parameter(t1, 1);

        Node* cmp = R.Binop(ops[o].js_op, p0, p1);
        Node* r = R.reduce(cmp);

        const Operator* expected;
        if (t0->Is(Type::Unsigned32()) && t1->Is(Type::Unsigned32())) {
          expected = ops[o].uint_op;
        } else if (t0->Is(Type::Signed32()) && t1->Is(Type::Signed32())) {
          expected = ops[o].int_op;
        } else {
          expected = ops[o].num_op;
        }
        R.CheckPureBinop(expected, r);
        if (ops[o].commute) {
          CHECK_EQ(p1, r->InputAt(0));
          CHECK_EQ(p0, r->InputAt(1));
        } else {
          CHECK_EQ(p0, r->InputAt(0));
          CHECK_EQ(p1, r->InputAt(1));
        }
      }
    }
  }
}
