// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/js-builtin-reducer.h"
#include "src/compiler/js-graph.h"
#include "src/compiler/node-properties.h"
#include "src/compiler/typer.h"
#include "test/unittests/compiler/graph-unittest.h"
#include "test/unittests/compiler/node-test-utils.h"
#include "testing/gmock-support.h"

using testing::BitEq;
using testing::Capture;

namespace v8 {
namespace internal {
namespace compiler {

class JSBuiltinReducerTest : public TypedGraphTest {
 public:
  JSBuiltinReducerTest() : javascript_(zone()) {}

 protected:
  Reduction Reduce(Node* node, MachineOperatorBuilder::Flags flags =
                                   MachineOperatorBuilder::Flag::kNoFlags) {
    MachineOperatorBuilder machine(zone(), kMachPtr, flags);
    JSGraph jsgraph(isolate(), graph(), common(), javascript(), &machine);
    // TODO(titzer): mock the GraphReducer here for better unit testing.
    GraphReducer graph_reducer(zone(), graph());
    JSBuiltinReducer reducer(&graph_reducer, &jsgraph);
    return reducer.Reduce(node);
  }

  Node* MathFunction(const char* name) {
    Handle<Object> m =
        JSObject::GetProperty(isolate()->global_object(),
                              isolate()->factory()->NewStringFromAsciiChecked(
                                  "Math")).ToHandleChecked();
    Handle<JSFunction> f = Handle<JSFunction>::cast(
        JSObject::GetProperty(
            m, isolate()->factory()->NewStringFromAsciiChecked(name))
            .ToHandleChecked());
    return HeapConstant(Unique<JSFunction>::CreateUninitialized(f));
  }

  JSOperatorBuilder* javascript() { return &javascript_; }

 private:
  JSOperatorBuilder javascript_;
};


namespace {

Type* const kIntegral32Types[] = {Type::UnsignedSmall(), Type::Negative32(),
                                  Type::Unsigned31(),    Type::SignedSmall(),
                                  Type::Signed32(),      Type::Unsigned32(),
                                  Type::Integral32()};


const LanguageMode kLanguageModes[] = {SLOPPY, STRICT, STRONG};


// TODO(mstarzinger): Find a common place and unify with test-js-typed-lowering.
Type* const kNumberTypes[] = {
    Type::UnsignedSmall(), Type::Negative32(),  Type::Unsigned31(),
    Type::SignedSmall(),   Type::Signed32(),    Type::Unsigned32(),
    Type::Integral32(),    Type::MinusZero(),   Type::NaN(),
    Type::OrderedNumber(), Type::PlainNumber(), Type::Number()};

}  // namespace


// -----------------------------------------------------------------------------
// Math.max


TEST_F(JSBuiltinReducerTest, MathMax0) {
  Node* function = MathFunction("max");

  Node* effect = graph()->start();
  Node* control = graph()->start();
  Node* frame_state = graph()->start();
  TRACED_FOREACH(LanguageMode, language_mode, kLanguageModes) {
    Node* call = graph()->NewNode(
        javascript()->CallFunction(2, NO_CALL_FUNCTION_FLAGS, language_mode),
        function, UndefinedConstant(), frame_state, frame_state, effect,
        control);
    Reduction r = Reduce(call);

    ASSERT_TRUE(r.Changed());
    EXPECT_THAT(r.replacement(), IsNumberConstant(-V8_INFINITY));
  }
}


TEST_F(JSBuiltinReducerTest, MathMax1) {
  Node* function = MathFunction("max");

  Node* effect = graph()->start();
  Node* control = graph()->start();
  Node* frame_state = graph()->start();
  TRACED_FOREACH(LanguageMode, language_mode, kLanguageModes) {
    TRACED_FOREACH(Type*, t0, kNumberTypes) {
      Node* p0 = Parameter(t0, 0);
      Node* call = graph()->NewNode(
          javascript()->CallFunction(3, NO_CALL_FUNCTION_FLAGS, language_mode),
          function, UndefinedConstant(), p0, frame_state, frame_state, effect,
          control);
      Reduction r = Reduce(call);

      ASSERT_TRUE(r.Changed());
      EXPECT_THAT(r.replacement(), p0);
    }
  }
}


TEST_F(JSBuiltinReducerTest, MathMax2) {
  Node* function = MathFunction("max");

  Node* effect = graph()->start();
  Node* control = graph()->start();
  Node* frame_state = graph()->start();
  TRACED_FOREACH(LanguageMode, language_mode, kLanguageModes) {
    TRACED_FOREACH(Type*, t0, kIntegral32Types) {
      TRACED_FOREACH(Type*, t1, kIntegral32Types) {
        Node* p0 = Parameter(t0, 0);
        Node* p1 = Parameter(t1, 1);
        Node* call =
            graph()->NewNode(javascript()->CallFunction(
                                 4, NO_CALL_FUNCTION_FLAGS, language_mode),
                             function, UndefinedConstant(), p0, p1, frame_state,
                             frame_state, effect, control);
        Reduction r = Reduce(call);

        ASSERT_TRUE(r.Changed());
        EXPECT_THAT(r.replacement(),
                    IsSelect(kMachNone, IsNumberLessThan(p1, p0), p0, p1));
      }
    }
  }
}


// -----------------------------------------------------------------------------
// Math.imul


TEST_F(JSBuiltinReducerTest, MathImul) {
  Node* function = MathFunction("imul");

  Node* effect = graph()->start();
  Node* control = graph()->start();
  Node* frame_state = graph()->start();
  TRACED_FOREACH(LanguageMode, language_mode, kLanguageModes) {
    TRACED_FOREACH(Type*, t0, kIntegral32Types) {
      TRACED_FOREACH(Type*, t1, kIntegral32Types) {
        Node* p0 = Parameter(t0, 0);
        Node* p1 = Parameter(t1, 1);
        Node* call =
            graph()->NewNode(javascript()->CallFunction(
                                 4, NO_CALL_FUNCTION_FLAGS, language_mode),
                             function, UndefinedConstant(), p0, p1, frame_state,
                             frame_state, effect, control);
        Reduction r = Reduce(call);

        ASSERT_TRUE(r.Changed());
        EXPECT_THAT(r.replacement(), IsInt32Mul(p0, p1));
      }
    }
  }
}


// -----------------------------------------------------------------------------
// Math.fround


TEST_F(JSBuiltinReducerTest, MathFround) {
  Node* function = MathFunction("fround");

  Node* effect = graph()->start();
  Node* control = graph()->start();
  Node* frame_state = graph()->start();
  TRACED_FOREACH(LanguageMode, language_mode, kLanguageModes) {
    TRACED_FOREACH(Type*, t0, kNumberTypes) {
      Node* p0 = Parameter(t0, 0);
      Node* call = graph()->NewNode(
          javascript()->CallFunction(3, NO_CALL_FUNCTION_FLAGS, language_mode),
          function, UndefinedConstant(), p0, frame_state, frame_state, effect,
          control);
      Reduction r = Reduce(call);

      ASSERT_TRUE(r.Changed());
      EXPECT_THAT(r.replacement(), IsTruncateFloat64ToFloat32(p0));
    }
  }
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
