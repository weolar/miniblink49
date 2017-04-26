// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/js-builtin-reducer.h"
#include "src/compiler/js-graph.h"
#include "src/compiler/node-matchers.h"
#include "src/compiler/node-properties.h"
#include "src/compiler/simplified-operator.h"
#include "src/objects-inl.h"
#include "src/type-cache.h"
#include "src/types.h"

namespace v8 {
namespace internal {
namespace compiler {


// Helper class to access JSCallFunction nodes that are potential candidates
// for reduction when they have a BuiltinFunctionId associated with them.
class JSCallReduction {
 public:
  explicit JSCallReduction(Node* node) : node_(node) {}

  // Determines whether the node is a JSCallFunction operation that targets a
  // constant callee being a well-known builtin with a BuiltinFunctionId.
  bool HasBuiltinFunctionId() {
    if (node_->opcode() != IrOpcode::kJSCallFunction) return false;
    HeapObjectMatcher m(NodeProperties::GetValueInput(node_, 0));
    if (!m.HasValue() || !m.Value()->IsJSFunction()) return false;
    Handle<JSFunction> function = Handle<JSFunction>::cast(m.Value());
    return function->shared()->HasBuiltinFunctionId();
  }

  // Retrieves the BuiltinFunctionId as described above.
  BuiltinFunctionId GetBuiltinFunctionId() {
    DCHECK_EQ(IrOpcode::kJSCallFunction, node_->opcode());
    HeapObjectMatcher m(NodeProperties::GetValueInput(node_, 0));
    Handle<JSFunction> function = Handle<JSFunction>::cast(m.Value());
    return function->shared()->builtin_function_id();
  }

  // Determines whether the call takes zero inputs.
  bool InputsMatchZero() { return GetJSCallArity() == 0; }

  // Determines whether the call takes one input of the given type.
  bool InputsMatchOne(Type* t1) {
    return GetJSCallArity() == 1 &&
           NodeProperties::GetType(GetJSCallInput(0))->Is(t1);
  }

  // Determines whether the call takes two inputs of the given types.
  bool InputsMatchTwo(Type* t1, Type* t2) {
    return GetJSCallArity() == 2 &&
           NodeProperties::GetType(GetJSCallInput(0))->Is(t1) &&
           NodeProperties::GetType(GetJSCallInput(1))->Is(t2);
  }

  // Determines whether the call takes inputs all of the given type.
  bool InputsMatchAll(Type* t) {
    for (int i = 0; i < GetJSCallArity(); i++) {
      if (!NodeProperties::GetType(GetJSCallInput(i))->Is(t)) {
        return false;
      }
    }
    return true;
  }

  Node* left() { return GetJSCallInput(0); }
  Node* right() { return GetJSCallInput(1); }

  int GetJSCallArity() {
    DCHECK_EQ(IrOpcode::kJSCallFunction, node_->opcode());
    // Skip first (i.e. callee) and second (i.e. receiver) operand.
    return node_->op()->ValueInputCount() - 2;
  }

  Node* GetJSCallInput(int index) {
    DCHECK_EQ(IrOpcode::kJSCallFunction, node_->opcode());
    DCHECK_LT(index, GetJSCallArity());
    // Skip first (i.e. callee) and second (i.e. receiver) operand.
    return NodeProperties::GetValueInput(node_, index + 2);
  }

 private:
  Node* node_;
};

JSBuiltinReducer::JSBuiltinReducer(Editor* editor, JSGraph* jsgraph)
    : AdvancedReducer(editor),
      jsgraph_(jsgraph),
      type_cache_(TypeCache::Get()) {}

// ECMA-262, section 15.8.2.11.
Reduction JSBuiltinReducer::ReduceMathMax(Node* node) {
  JSCallReduction r(node);
  if (r.InputsMatchZero()) {
    // Math.max() -> -Infinity
    return Replace(jsgraph()->Constant(-V8_INFINITY));
  }
  if (r.InputsMatchOne(Type::Number())) {
    // Math.max(a:number) -> a
    return Replace(r.left());
  }
  if (r.InputsMatchAll(Type::Integral32())) {
    // Math.max(a:int32, b:int32, ...)
    Node* value = r.GetJSCallInput(0);
    for (int i = 1; i < r.GetJSCallArity(); i++) {
      Node* const input = r.GetJSCallInput(i);
      value = graph()->NewNode(
          common()->Select(MachineRepresentation::kNone),
          graph()->NewNode(simplified()->NumberLessThan(), input, value), value,
          input);
    }
    return Replace(value);
  }
  return NoChange();
}

// ES6 section 20.2.2.19 Math.imul ( x, y )
Reduction JSBuiltinReducer::ReduceMathImul(Node* node) {
  JSCallReduction r(node);
  if (r.InputsMatchTwo(Type::Number(), Type::Number())) {
    // Math.imul(a:number, b:number) -> NumberImul(NumberToUint32(a),
    //                                             NumberToUint32(b))
    Node* a = graph()->NewNode(simplified()->NumberToUint32(), r.left());
    Node* b = graph()->NewNode(simplified()->NumberToUint32(), r.right());
    Node* value = graph()->NewNode(simplified()->NumberImul(), a, b);
    return Replace(value);
  }
  return NoChange();
}

// ES6 section 20.2.2.10 Math.ceil ( x )
Reduction JSBuiltinReducer::ReduceMathCeil(Node* node) {
  JSCallReduction r(node);
  if (r.InputsMatchOne(Type::Number())) {
    // Math.ceil(a:number) -> NumberCeil(a)
    Node* value = graph()->NewNode(simplified()->NumberCeil(), r.left());
    return Replace(value);
  }
  return NoChange();
}

// ES6 section 20.2.2.11 Math.clz32 ( x )
Reduction JSBuiltinReducer::ReduceMathClz32(Node* node) {
  JSCallReduction r(node);
  if (r.InputsMatchOne(Type::Unsigned32())) {
    // Math.clz32(a:unsigned32) -> NumberClz32(a)
    Node* value = graph()->NewNode(simplified()->NumberClz32(), r.left());
    return Replace(value);
  }
  if (r.InputsMatchOne(Type::Number())) {
    // Math.clz32(a:number) -> NumberClz32(NumberToUint32(a))
    Node* value = graph()->NewNode(
        simplified()->NumberClz32(),
        graph()->NewNode(simplified()->NumberToUint32(), r.left()));
    return Replace(value);
  }
  return NoChange();
}

// ES6 draft 08-24-14, section 20.2.2.16.
Reduction JSBuiltinReducer::ReduceMathFloor(Node* node) {
  JSCallReduction r(node);
  if (r.InputsMatchOne(Type::Number())) {
    // Math.floor(a:number) -> NumberFloor(a)
    Node* value = graph()->NewNode(simplified()->NumberFloor(), r.left());
    return Replace(value);
  }
  return NoChange();
}

// ES6 draft 08-24-14, section 20.2.2.17.
Reduction JSBuiltinReducer::ReduceMathFround(Node* node) {
  JSCallReduction r(node);
  if (r.InputsMatchOne(Type::Number())) {
    // Math.fround(a:number) -> TruncateFloat64ToFloat32(a)
    Node* value =
        graph()->NewNode(machine()->TruncateFloat64ToFloat32(), r.left());
    return Replace(value);
  }
  return NoChange();
}

// ES6 section 20.2.2.28 Math.round ( x )
Reduction JSBuiltinReducer::ReduceMathRound(Node* node) {
  JSCallReduction r(node);
  if (r.InputsMatchOne(Type::Number())) {
    // Math.round(a:number) -> NumberRound(a)
    Node* value = graph()->NewNode(simplified()->NumberRound(), r.left());
    return Replace(value);
  }
  return NoChange();
}

// ES6 section 20.2.2.32 Math.sqrt ( x )
Reduction JSBuiltinReducer::ReduceMathSqrt(Node* node) {
  JSCallReduction r(node);
  if (r.InputsMatchOne(Type::Number())) {
    // Math.sqrt(a:number) -> Float64Sqrt(a)
    Node* value = graph()->NewNode(machine()->Float64Sqrt(), r.left());
    return Replace(value);
  }
  return NoChange();
}

// ES6 section 20.2.2.35 Math.trunc ( x )
Reduction JSBuiltinReducer::ReduceMathTrunc(Node* node) {
  JSCallReduction r(node);
  if (r.InputsMatchOne(Type::Number())) {
    // Math.trunc(a:number) -> NumberTrunc(a)
    Node* value = graph()->NewNode(simplified()->NumberTrunc(), r.left());
    return Replace(value);
  }
  return NoChange();
}

Reduction JSBuiltinReducer::Reduce(Node* node) {
  Reduction reduction = NoChange();
  JSCallReduction r(node);

  // Dispatch according to the BuiltinFunctionId if present.
  if (!r.HasBuiltinFunctionId()) return NoChange();
  switch (r.GetBuiltinFunctionId()) {
    case kMathMax:
      reduction = ReduceMathMax(node);
      break;
    case kMathImul:
      reduction = ReduceMathImul(node);
      break;
    case kMathClz32:
      reduction = ReduceMathClz32(node);
      break;
    case kMathCeil:
      reduction = ReduceMathCeil(node);
      break;
    case kMathFloor:
      reduction = ReduceMathFloor(node);
      break;
    case kMathFround:
      reduction = ReduceMathFround(node);
      break;
    case kMathRound:
      reduction = ReduceMathRound(node);
      break;
    case kMathSqrt:
      reduction = ReduceMathSqrt(node);
      break;
    case kMathTrunc:
      reduction = ReduceMathTrunc(node);
      break;
    default:
      break;
  }

  // Replace builtin call assuming replacement nodes are pure values that don't
  // produce an effect. Replaces {node} with {reduction} and relaxes effects.
  if (reduction.Changed()) ReplaceWithValue(node, reduction.replacement());

  return reduction;
}


Graph* JSBuiltinReducer::graph() const { return jsgraph()->graph(); }


Isolate* JSBuiltinReducer::isolate() const { return jsgraph()->isolate(); }


CommonOperatorBuilder* JSBuiltinReducer::common() const {
  return jsgraph()->common();
}


MachineOperatorBuilder* JSBuiltinReducer::machine() const {
  return jsgraph()->machine();
}


SimplifiedOperatorBuilder* JSBuiltinReducer::simplified() const {
  return jsgraph()->simplified();
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
