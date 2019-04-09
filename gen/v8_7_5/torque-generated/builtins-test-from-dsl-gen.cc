#include "src/builtins/builtins-utils-gen.h"
#include "src/builtins/builtins.h"
#include "src/code-factory.h"
#include "src/elements-kind.h"
#include "src/heap/factory-inl.h"
#include "src/objects.h"
#include "src/objects/arguments.h"
#include "src/objects/bigint.h"
#include "src/objects/free-space.h"
#include "src/objects/js-generator.h"
#include "src/objects/js-promise.h"
#include "src/objects/js-regexp-string-iterator.h"
#include "src/objects/module.h"
#include "src/objects/stack-frame-info.h"
#include "src/builtins/builtins-array-gen.h"
#include "src/builtins/builtins-collections-gen.h"
#include "src/builtins/builtins-data-view-gen.h"
#include "src/builtins/builtins-iterator-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-regexp-gen.h"
#include "src/builtins/builtins-regexp-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-constructor-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "torque-generated/builtins-base-from-dsl-gen.h"
#include "torque-generated/builtins-growable-fixed-array-from-dsl-gen.h"
#include "torque-generated/builtins-arguments-from-dsl-gen.h"
#include "torque-generated/builtins-array-from-dsl-gen.h"
#include "torque-generated/builtins-array-copywithin-from-dsl-gen.h"
#include "torque-generated/builtins-array-filter-from-dsl-gen.h"
#include "torque-generated/builtins-array-find-from-dsl-gen.h"
#include "torque-generated/builtins-array-findindex-from-dsl-gen.h"
#include "torque-generated/builtins-array-foreach-from-dsl-gen.h"
#include "torque-generated/builtins-array-join-from-dsl-gen.h"
#include "torque-generated/builtins-array-lastindexof-from-dsl-gen.h"
#include "torque-generated/builtins-array-of-from-dsl-gen.h"
#include "torque-generated/builtins-array-map-from-dsl-gen.h"
#include "torque-generated/builtins-array-reverse-from-dsl-gen.h"
#include "torque-generated/builtins-array-shift-from-dsl-gen.h"
#include "torque-generated/builtins-array-slice-from-dsl-gen.h"
#include "torque-generated/builtins-array-splice-from-dsl-gen.h"
#include "torque-generated/builtins-array-unshift-from-dsl-gen.h"
#include "torque-generated/builtins-collections-from-dsl-gen.h"
#include "torque-generated/builtins-data-view-from-dsl-gen.h"
#include "torque-generated/builtins-extras-utils-from-dsl-gen.h"
#include "torque-generated/builtins-iterator-from-dsl-gen.h"
#include "torque-generated/builtins-object-from-dsl-gen.h"
#include "torque-generated/builtins-proxy-from-dsl-gen.h"
#include "torque-generated/builtins-regexp-from-dsl-gen.h"
#include "torque-generated/builtins-regexp-replace-from-dsl-gen.h"
#include "torque-generated/builtins-string-from-dsl-gen.h"
#include "torque-generated/builtins-string-html-from-dsl-gen.h"
#include "torque-generated/builtins-string-repeat-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-createtypedarray-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-every-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-filter-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-find-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-findindex-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-foreach-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-reduce-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-reduceright-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-slice-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-some-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-subarray-from-dsl-gen.h"
#include "torque-generated/builtins-test-from-dsl-gen.h"

namespace v8 {
namespace internal {

compiler::TNode<BoolT> TestBuiltinsFromDSLAssembler::ElementsKindTestHelper1(ElementsKind p_kind) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 7);
    if ((((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT8_ELEMENTS)) || (CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT16_ELEMENTS))))) {
      ca_.Goto(&block3);
    } else {
      ca_.Goto(&block4);
    }
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 8);
    compiler::TNode<BoolT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block1, tmp0);
  }

  if (block4.is_used()) {
    ca_.Bind(&block4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 10);
    compiler::TNode<BoolT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block1, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 6);
    ca_.Goto(&block6, tmp2);
  }

    compiler::TNode<BoolT> tmp3;
    ca_.Bind(&block6, &tmp3);
  return compiler::TNode<BoolT>{tmp3};
}

compiler::TNode<BoolT> TestBuiltinsFromDSLAssembler::ElementsKindTestHelper2(ElementsKind p_kind) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 15);
    compiler::TNode<BoolT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT8_ELEMENTS)) || (CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT16_ELEMENTS)))));
    ca_.Goto(&block1, tmp0);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp1;
    ca_.Bind(&block1, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 14);
    ca_.Goto(&block3, tmp1);
  }

    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block3, &tmp2);
  return compiler::TNode<BoolT>{tmp2};
}

bool TestBuiltinsFromDSLAssembler::ElementsKindTestHelper3(ElementsKind p_kind) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 19);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 18);
    ca_.Goto(&block3);
  }

    ca_.Bind(&block3);
  return ((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT8_ELEMENTS)) || (CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT16_ELEMENTS)));
}

void TestBuiltinsFromDSLAssembler::LabelTestHelper1(compiler::CodeAssemblerLabel* label_Label1) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 24);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 22);
    ca_.Goto(label_Label1);
  }
}

void TestBuiltinsFromDSLAssembler::LabelTestHelper2(compiler::CodeAssemblerLabel* label_Label2, compiler::TypedCodeAssemblerVariable<Smi>* label_Label2_parameter_0) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 29);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(42));
    ca_.Goto(&block1, tmp0);
  }

  if (block1.is_used()) {
    compiler::TNode<Smi> tmp1;
    ca_.Bind(&block1, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 27);
    *label_Label2_parameter_0 = tmp1;
    ca_.Goto(label_Label2);
  }
}

void TestBuiltinsFromDSLAssembler::LabelTestHelper3(compiler::CodeAssemblerLabel* label_Label3, compiler::TypedCodeAssemblerVariable<Oddball>* label_Label3_parameter_0, compiler::TypedCodeAssemblerVariable<Smi>* label_Label3_parameter_1) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 34);
    compiler::TNode<Oddball> tmp0;
    USE(tmp0);
    tmp0 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(7));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<Oddball> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 32);
    *label_Label3_parameter_1 = tmp3;
    *label_Label3_parameter_0 = tmp2;
    ca_.Goto(label_Label3);
  }
}

void TestBuiltinsFromDSLAssembler::TestConstexpr1() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 38);
    compiler::TNode<BoolT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool((CodeStubAssembler(state_).IsFastElementsKind(PACKED_SMI_ELEMENTS))));
    ca_.Branch(tmp0, &block2, &block3);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    CodeStubAssembler(state_).FailAssert("Torque assert \'FromConstexpr<bool>(IsFastElementsKind(PACKED_SMI_ELEMENTS))\' failed", "../../test/torque/test-torque.tq", 38);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 37);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block4);
  }

    ca_.Bind(&block4);
}

void TestBuiltinsFromDSLAssembler::TestConstexprIf() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 42);
    compiler::TNode<BoolT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).ElementsKindTestHelper1(UINT8_ELEMENTS));
    ca_.Branch(tmp0, &block2, &block3);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    CodeStubAssembler(state_).FailAssert("Torque assert \'ElementsKindTestHelper1(UINT8_ELEMENTS)\' failed", "../../test/torque/test-torque.tq", 42);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 43);
    compiler::TNode<BoolT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).ElementsKindTestHelper1(UINT16_ELEMENTS));
    ca_.Branch(tmp1, &block4, &block5);
  }

  if (block5.is_used()) {
    ca_.Bind(&block5);
    CodeStubAssembler(state_).FailAssert("Torque assert \'ElementsKindTestHelper1(UINT16_ELEMENTS)\' failed", "../../test/torque/test-torque.tq", 43);
  }

  if (block4.is_used()) {
    ca_.Bind(&block4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 44);
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).ElementsKindTestHelper1(UINT32_ELEMENTS));
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp2}));
    ca_.Branch(tmp3, &block6, &block7);
  }

  if (block7.is_used()) {
    ca_.Bind(&block7);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!ElementsKindTestHelper1(UINT32_ELEMENTS)\' failed", "../../test/torque/test-torque.tq", 44);
  }

  if (block6.is_used()) {
    ca_.Bind(&block6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 41);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block8);
  }

    ca_.Bind(&block8);
}

void TestBuiltinsFromDSLAssembler::TestConstexprReturn() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 48);
    compiler::TNode<BoolT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool((TestBuiltinsFromDSLAssembler(state_).ElementsKindTestHelper3(UINT8_ELEMENTS))));
    ca_.Branch(tmp0, &block2, &block3);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    CodeStubAssembler(state_).FailAssert("Torque assert \'FromConstexpr<bool>(ElementsKindTestHelper3(UINT8_ELEMENTS))\' failed", "../../test/torque/test-torque.tq", 48);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 49);
    compiler::TNode<BoolT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool((TestBuiltinsFromDSLAssembler(state_).ElementsKindTestHelper3(UINT16_ELEMENTS))));
    ca_.Branch(tmp1, &block4, &block5);
  }

  if (block5.is_used()) {
    ca_.Bind(&block5);
    CodeStubAssembler(state_).FailAssert("Torque assert \'FromConstexpr<bool>(ElementsKindTestHelper3(UINT16_ELEMENTS))\' failed", "../../test/torque/test-torque.tq", 49);
  }

  if (block4.is_used()) {
    ca_.Bind(&block4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 50);
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool((TestBuiltinsFromDSLAssembler(state_).ElementsKindTestHelper3(UINT32_ELEMENTS))));
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp2}));
    ca_.Branch(tmp3, &block6, &block7);
  }

  if (block7.is_used()) {
    ca_.Bind(&block7);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!FromConstexpr<bool>(ElementsKindTestHelper3(UINT32_ELEMENTS))\' failed", "../../test/torque/test-torque.tq", 50);
  }

  if (block6.is_used()) {
    ca_.Bind(&block6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 51);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool((CodeStubAssembler(state_).ConstexprBoolNot((TestBuiltinsFromDSLAssembler(state_).ElementsKindTestHelper3(UINT32_ELEMENTS))))));
    ca_.Branch(tmp4, &block8, &block9);
  }

  if (block9.is_used()) {
    ca_.Bind(&block9);
    CodeStubAssembler(state_).FailAssert("Torque assert \'FromConstexpr<bool>(!ElementsKindTestHelper3(UINT32_ELEMENTS))\' failed", "../../test/torque/test-torque.tq", 51);
  }

  if (block8.is_used()) {
    ca_.Bind(&block8);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 47);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block10);
  }

    ca_.Bind(&block10);
}

compiler::TNode<Oddball> TestBuiltinsFromDSLAssembler::TestGotoLabel() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 56);
    compiler::CodeAssemblerLabel label0(&ca_);
    TestBuiltinsFromDSLAssembler(state_).LabelTestHelper1(&label0);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4);
    }
  }

  if (block4.is_used()) {
    ca_.Bind(&block4);
    ca_.Goto(&block3);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 59);
    compiler::TNode<Oddball> tmp0;
    USE(tmp0);
    tmp0 = BaseBuiltinsFromDSLAssembler(state_).True();
    ca_.Goto(&block1, tmp0);
  }

  if (block1.is_used()) {
    compiler::TNode<Oddball> tmp1;
    ca_.Bind(&block1, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 54);
    ca_.Goto(&block5, tmp1);
  }

    compiler::TNode<Oddball> tmp2;
    ca_.Bind(&block5, &tmp2);
  return compiler::TNode<Oddball>{tmp2};
}

compiler::TNode<Oddball> TestBuiltinsFromDSLAssembler::TestGotoLabelWithOneParameter() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block6(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 65);
    compiler::TypedCodeAssemblerVariable<Smi> result_0_0(&ca_);
    compiler::CodeAssemblerLabel label0(&ca_);
    TestBuiltinsFromDSLAssembler(state_).LabelTestHelper2(&label0, &result_0_0);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, result_0_0.value());
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp0;
    ca_.Bind(&block4, &tmp0);
    ca_.Goto(&block3, tmp0);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp1;
    ca_.Bind(&block3, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 68);
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(42));
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp1}, compiler::TNode<Smi>{tmp2}));
    ca_.Branch(tmp3, &block5, &block6, tmp1);
  }

  if (block6.is_used()) {
    compiler::TNode<Smi> tmp4;
    ca_.Bind(&block6, &tmp4);
    CodeStubAssembler(state_).FailAssert("Torque assert \'smi == 42\' failed", "../../test/torque/test-torque.tq", 68);
  }

  if (block5.is_used()) {
    compiler::TNode<Smi> tmp5;
    ca_.Bind(&block5, &tmp5);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 69);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).True();
    ca_.Goto(&block1, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Oddball> tmp7;
    ca_.Bind(&block1, &tmp7);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 63);
    ca_.Goto(&block7, tmp7);
  }

    compiler::TNode<Oddball> tmp8;
    ca_.Bind(&block7, &tmp8);
  return compiler::TNode<Oddball>{tmp8};
}

compiler::TNode<Oddball> TestBuiltinsFromDSLAssembler::TestGotoLabelWithTwoParameters() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 75);
    compiler::TypedCodeAssemblerVariable<Oddball> result_0_0(&ca_);
    compiler::TypedCodeAssemblerVariable<Smi> result_0_1(&ca_);
    compiler::CodeAssemblerLabel label0(&ca_);
    TestBuiltinsFromDSLAssembler(state_).LabelTestHelper3(&label0, &result_0_0, &result_0_1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, result_0_0.value(), result_0_1.value());
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Oddball> tmp0;
    compiler::TNode<Smi> tmp1;
    ca_.Bind(&block4, &tmp0, &tmp1);
    ca_.Goto(&block3, tmp0, tmp1);
  }

  if (block3.is_used()) {
    compiler::TNode<Oddball> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block3, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 78);
    compiler::TNode<Oddball> tmp4;
    USE(tmp4);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp2}, compiler::TNode<HeapObject>{tmp4}));
    ca_.Branch(tmp5, &block5, &block6, tmp2, tmp3);
  }

  if (block6.is_used()) {
    compiler::TNode<Oddball> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block6, &tmp6, &tmp7);
    CodeStubAssembler(state_).FailAssert("Torque assert \'o == Null\' failed", "../../test/torque/test-torque.tq", 78);
  }

  if (block5.is_used()) {
    compiler::TNode<Oddball> tmp8;
    compiler::TNode<Smi> tmp9;
    ca_.Bind(&block5, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 79);
    compiler::TNode<Smi> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(7));
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp9}, compiler::TNode<Smi>{tmp10}));
    ca_.Branch(tmp11, &block7, &block8, tmp8, tmp9);
  }

  if (block8.is_used()) {
    compiler::TNode<Oddball> tmp12;
    compiler::TNode<Smi> tmp13;
    ca_.Bind(&block8, &tmp12, &tmp13);
    CodeStubAssembler(state_).FailAssert("Torque assert \'smi == 7\' failed", "../../test/torque/test-torque.tq", 79);
  }

  if (block7.is_used()) {
    compiler::TNode<Oddball> tmp14;
    compiler::TNode<Smi> tmp15;
    ca_.Bind(&block7, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 80);
    compiler::TNode<Oddball> tmp16;
    USE(tmp16);
    tmp16 = BaseBuiltinsFromDSLAssembler(state_).True();
    ca_.Goto(&block1, tmp16);
  }

  if (block1.is_used()) {
    compiler::TNode<Oddball> tmp17;
    ca_.Bind(&block1, &tmp17);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 73);
    ca_.Goto(&block9, tmp17);
  }

    compiler::TNode<Oddball> tmp18;
    ca_.Bind(&block9, &tmp18);
  return compiler::TNode<Oddball>{tmp18};
}

TF_BUILTIN(GenericBuiltinTest20UT5ATSmi10HeapObject, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Object> parameter1 = UncheckedCast<Object>(Parameter(Descriptor::kParam));
  USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 89);
    CodeStubAssembler(state_).Return(tmp1);
  }
}

void TestBuiltinsFromDSLAssembler::TestBuiltinSpecialization(compiler::TNode<Context> p_c) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_c);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 93);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp2;
    tmp2 = CodeStubAssembler(state_).CallBuiltin(Builtins::kGenericBuiltinTest5ATSmi, tmp0, tmp1);
    USE(tmp2);
    compiler::TNode<Oddball> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp2}, compiler::TNode<HeapObject>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    ca_.Bind(&block3, &tmp5);
    CodeStubAssembler(state_).FailAssert("Torque assert \'GenericBuiltinTest<Smi>(c, 0) == Null\' failed", "../../test/torque/test-torque.tq", 93);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp6;
    ca_.Bind(&block2, &tmp6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 94);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp8;
    tmp8 = CodeStubAssembler(state_).CallBuiltin(Builtins::kGenericBuiltinTest5ATSmi, tmp6, tmp7);
    USE(tmp8);
    compiler::TNode<Oddball> tmp9;
    USE(tmp9);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp8}, compiler::TNode<HeapObject>{tmp9}));
    ca_.Branch(tmp10, &block4, &block5, tmp6);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp11;
    ca_.Bind(&block5, &tmp11);
    CodeStubAssembler(state_).FailAssert("Torque assert \'GenericBuiltinTest<Smi>(c, 1) == Null\' failed", "../../test/torque/test-torque.tq", 94);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp12;
    ca_.Bind(&block4, &tmp12);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 95);
    compiler::TNode<Oddball> tmp13;
    USE(tmp13);
    tmp13 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Object> tmp14;
    tmp14 = CodeStubAssembler(state_).CallBuiltin(Builtins::kGenericBuiltinTest20UT5ATSmi10HeapObject, tmp12, tmp13);
    USE(tmp14);
    compiler::TNode<Oddball> tmp15;
    USE(tmp15);
    tmp15 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp14}, compiler::TNode<HeapObject>{tmp15}));
    ca_.Branch(tmp16, &block6, &block7, tmp12);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp17;
    ca_.Bind(&block7, &tmp17);
    CodeStubAssembler(state_).FailAssert("Torque assert \'GenericBuiltinTest<Object>(c, Undefined) == Undefined\' failed", "../../test/torque/test-torque.tq", 95);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp18;
    ca_.Bind(&block6, &tmp18);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 96);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Object> tmp20;
    tmp20 = CodeStubAssembler(state_).CallBuiltin(Builtins::kGenericBuiltinTest20UT5ATSmi10HeapObject, tmp18, tmp19);
    USE(tmp20);
    compiler::TNode<Oddball> tmp21;
    USE(tmp21);
    tmp21 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp20}, compiler::TNode<HeapObject>{tmp21}));
    ca_.Branch(tmp22, &block8, &block9, tmp18);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp23;
    ca_.Bind(&block9, &tmp23);
    CodeStubAssembler(state_).FailAssert("Torque assert \'GenericBuiltinTest<Object>(c, Undefined) == Undefined\' failed", "../../test/torque/test-torque.tq", 96);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp24;
    ca_.Bind(&block8, &tmp24);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 92);
    ca_.Goto(&block1, tmp24);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp25;
    ca_.Bind(&block1, &tmp25);
    ca_.Goto(&block10, tmp25);
  }

    compiler::TNode<Context> tmp26;
    ca_.Bind(&block10, &tmp26);
}

void TestBuiltinsFromDSLAssembler::LabelTestHelper4(bool p_flag, compiler::CodeAssemblerLabel* label_Label4, compiler::CodeAssemblerLabel* label_Label5) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 101);
    if ((p_flag)) {
      ca_.Goto(&block3);
    } else {
      ca_.Goto(&block4);
    }
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 102);
    ca_.Goto(&block1);
  }

  if (block4.is_used()) {
    ca_.Bind(&block4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 104);
    ca_.Goto(&block2);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 99);
    ca_.Goto(label_Label4);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.Goto(label_Label5);
  }
}

compiler::TNode<BoolT> TestBuiltinsFromDSLAssembler::CallLabelTestHelper4(bool p_flag) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 110);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    TestBuiltinsFromDSLAssembler(state_).LabelTestHelper4(p_flag, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block7);
    }
  }

  if (block6.is_used()) {
    ca_.Bind(&block6);
    ca_.Goto(&block5);
  }

  if (block7.is_used()) {
    ca_.Bind(&block7);
    ca_.Goto(&block3);
  }

  if (block5.is_used()) {
    ca_.Bind(&block5);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 113);
    compiler::TNode<BoolT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block1, tmp0);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 116);
    compiler::TNode<BoolT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block1, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 108);
    ca_.Goto(&block8, tmp2);
  }

    compiler::TNode<BoolT> tmp3;
    ca_.Bind(&block8, &tmp3);
  return compiler::TNode<BoolT>{tmp3};
}

compiler::TNode<Oddball> TestBuiltinsFromDSLAssembler::TestPartiallyUnusedLabel() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Oddball> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 121);
    compiler::TNode<BoolT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).CallLabelTestHelper4(true));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 122);
    compiler::TNode<BoolT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).CallLabelTestHelper4(false));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 124);
    ca_.Branch(tmp0, &block4, &block3, tmp0, tmp1);
  }

  if (block4.is_used()) {
    compiler::TNode<BoolT> tmp2;
    compiler::TNode<BoolT> tmp3;
    ca_.Bind(&block4, &tmp2, &tmp3);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<BoolT> tmp5;
    compiler::TNode<BoolT> tmp6;
    ca_.Bind(&block2, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 125);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).True();
    ca_.Goto(&block1, tmp7);
  }

  if (block3.is_used()) {
    compiler::TNode<BoolT> tmp8;
    compiler::TNode<BoolT> tmp9;
    ca_.Bind(&block3, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 127);
    compiler::TNode<Oddball> tmp10;
    USE(tmp10);
    tmp10 = BaseBuiltinsFromDSLAssembler(state_).False();
    ca_.Goto(&block1, tmp10);
  }

  if (block1.is_used()) {
    compiler::TNode<Oddball> tmp11;
    ca_.Bind(&block1, &tmp11);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 120);
    ca_.Goto(&block6, tmp11);
  }

    compiler::TNode<Oddball> tmp12;
    ca_.Bind(&block6, &tmp12);
  return compiler::TNode<Oddball>{tmp12};
}

compiler::TNode<Object> TestBuiltinsFromDSLAssembler::GenericMacroTest20UT5ATSmi10HeapObject(compiler::TNode<Object> p_param2) {
  compiler::CodeAssemblerParameterizedLabel<Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_param2);

  if (block0.is_used()) {
    compiler::TNode<Object> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 136);
    ca_.Goto(&block1, tmp0, tmp0);
  }

  if (block1.is_used()) {
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    ca_.Bind(&block1, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 131);
    ca_.Goto(&block2, tmp1, tmp2);
  }

    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block2, &tmp3, &tmp4);
  return compiler::TNode<Object>{tmp4};
}

compiler::TNode<Object> TestBuiltinsFromDSLAssembler::GenericMacroTestWithLabels20UT5ATSmi10HeapObject(compiler::TNode<Object> p_param2, compiler::CodeAssemblerLabel* label_Y) {
  compiler::CodeAssemblerParameterizedLabel<Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_param2);

  if (block0.is_used()) {
    compiler::TNode<Object> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 146);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp1 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp0}, &label0);
    ca_.Goto(&block3, tmp0, tmp0, tmp1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp0);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block4, &tmp2, &tmp3);
    ca_.Goto(&block1);
  }

  if (block3.is_used()) {
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block3, &tmp4, &tmp5, &tmp6);
    ca_.Goto(&block2, tmp4, tmp6);
  }

  if (block2.is_used()) {
    compiler::TNode<Object> tmp7;
    compiler::TNode<Object> tmp8;
    ca_.Bind(&block2, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 139);
    ca_.Goto(&block5, tmp7, tmp8);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Y);
  }

    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    ca_.Bind(&block5, &tmp9, &tmp10);
  return compiler::TNode<Object>{tmp10};
}

void TestBuiltinsFromDSLAssembler::TestMacroSpecialization() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block11(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block13(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block15(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Object> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block19(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Oddball> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Oddball, Object> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 151);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 152);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Object>(TestBuiltinsFromDSLAssembler(state_).GenericMacroTest5ATSmi(compiler::TNode<Smi>{tmp1}));
    compiler::TNode<Oddball> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp2}, compiler::TNode<HeapObject>{tmp3}));
    ca_.Branch(tmp4, &block4, &block5, tmp0);
  }

  if (block5.is_used()) {
    compiler::TNode<Smi> tmp5;
    ca_.Bind(&block5, &tmp5);
    CodeStubAssembler(state_).FailAssert("Torque assert \'GenericMacroTest<Smi>(0) == Undefined\' failed", "../../test/torque/test-torque.tq", 152);
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block4, &tmp6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 153);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(TestBuiltinsFromDSLAssembler(state_).GenericMacroTest5ATSmi(compiler::TNode<Smi>{tmp7}));
    compiler::TNode<Oddball> tmp9;
    USE(tmp9);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp8}, compiler::TNode<HeapObject>{tmp9}));
    ca_.Branch(tmp10, &block6, &block7, tmp6);
  }

  if (block7.is_used()) {
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block7, &tmp11);
    CodeStubAssembler(state_).FailAssert("Torque assert \'GenericMacroTest<Smi>(1) == Undefined\' failed", "../../test/torque/test-torque.tq", 153);
  }

  if (block6.is_used()) {
    compiler::TNode<Smi> tmp12;
    ca_.Bind(&block6, &tmp12);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 154);
    compiler::TNode<Oddball> tmp13;
    USE(tmp13);
    tmp13 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<Object> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Object>(TestBuiltinsFromDSLAssembler(state_).GenericMacroTest20UT5ATSmi10HeapObject(compiler::TNode<Object>{tmp13}));
    compiler::TNode<Oddball> tmp15;
    USE(tmp15);
    tmp15 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<BoolT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp14}, compiler::TNode<HeapObject>{tmp15}));
    ca_.Branch(tmp16, &block8, &block9, tmp12);
  }

  if (block9.is_used()) {
    compiler::TNode<Smi> tmp17;
    ca_.Bind(&block9, &tmp17);
    CodeStubAssembler(state_).FailAssert("Torque assert \'GenericMacroTest<Object>(Null) == Null\' failed", "../../test/torque/test-torque.tq", 154);
  }

  if (block8.is_used()) {
    compiler::TNode<Smi> tmp18;
    ca_.Bind(&block8, &tmp18);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 155);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).False();
    compiler::TNode<Object> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Object>(TestBuiltinsFromDSLAssembler(state_).GenericMacroTest20UT5ATSmi10HeapObject(compiler::TNode<Object>{tmp19}));
    compiler::TNode<Oddball> tmp21;
    USE(tmp21);
    tmp21 = BaseBuiltinsFromDSLAssembler(state_).False();
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp20}, compiler::TNode<HeapObject>{tmp21}));
    ca_.Branch(tmp22, &block10, &block11, tmp18);
  }

  if (block11.is_used()) {
    compiler::TNode<Smi> tmp23;
    ca_.Bind(&block11, &tmp23);
    CodeStubAssembler(state_).FailAssert("Torque assert \'GenericMacroTest<Object>(False) == False\' failed", "../../test/torque/test-torque.tq", 155);
  }

  if (block10.is_used()) {
    compiler::TNode<Smi> tmp24;
    ca_.Bind(&block10, &tmp24);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 156);
    compiler::TNode<Oddball> tmp25;
    USE(tmp25);
    tmp25 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<Object> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Object>(TestBuiltinsFromDSLAssembler(state_).GenericMacroTest20UT5ATSmi10HeapObject(compiler::TNode<Object>{tmp25}));
    compiler::TNode<Oddball> tmp27;
    USE(tmp27);
    tmp27 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp26}, compiler::TNode<HeapObject>{tmp27}));
    ca_.Branch(tmp28, &block12, &block13, tmp24);
  }

  if (block13.is_used()) {
    compiler::TNode<Smi> tmp29;
    ca_.Bind(&block13, &tmp29);
    CodeStubAssembler(state_).FailAssert("Torque assert \'GenericMacroTest<Object>(True) == True\' failed", "../../test/torque/test-torque.tq", 156);
  }

  if (block12.is_used()) {
    compiler::TNode<Smi> tmp30;
    ca_.Bind(&block12, &tmp30);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 157);
    compiler::TNode<Smi> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp32;
    USE(tmp32);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp32 = TestBuiltinsFromDSLAssembler(state_).GenericMacroTestWithLabels5ATSmi(compiler::TNode<Smi>{tmp31}, &label0);
    ca_.Goto(&block16, tmp30, tmp32);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block17, tmp30);
    }
  }

  if (block17.is_used()) {
    compiler::TNode<Smi> tmp33;
    ca_.Bind(&block17, &tmp33);
    ca_.Goto(&block3);
  }

  if (block16.is_used()) {
    compiler::TNode<Smi> tmp34;
    compiler::TNode<Object> tmp35;
    ca_.Bind(&block16, &tmp34, &tmp35);
    compiler::TNode<Oddball> tmp36;
    USE(tmp36);
    tmp36 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp35}, compiler::TNode<HeapObject>{tmp36}));
    ca_.Branch(tmp37, &block14, &block15, tmp34);
  }

  if (block15.is_used()) {
    compiler::TNode<Smi> tmp38;
    ca_.Bind(&block15, &tmp38);
    CodeStubAssembler(state_).FailAssert("Torque assert \'(GenericMacroTestWithLabels<Smi>(0) otherwise Fail) == Undefined\' failed", "../../test/torque/test-torque.tq", 157);
  }

  if (block14.is_used()) {
    compiler::TNode<Smi> tmp39;
    ca_.Bind(&block14, &tmp39);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 158);
    compiler::TNode<Smi> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp41 = TestBuiltinsFromDSLAssembler(state_).GenericMacroTestWithLabels5ATSmi(compiler::TNode<Smi>{tmp40}, &label0);
    ca_.Goto(&block20, tmp39, tmp41);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block21, tmp39);
    }
  }

  if (block21.is_used()) {
    compiler::TNode<Smi> tmp42;
    ca_.Bind(&block21, &tmp42);
    ca_.Goto(&block3);
  }

  if (block20.is_used()) {
    compiler::TNode<Smi> tmp43;
    compiler::TNode<Object> tmp44;
    ca_.Bind(&block20, &tmp43, &tmp44);
    compiler::TNode<Oddball> tmp45;
    USE(tmp45);
    tmp45 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp44}, compiler::TNode<HeapObject>{tmp45}));
    ca_.Branch(tmp46, &block18, &block19, tmp43);
  }

  if (block19.is_used()) {
    compiler::TNode<Smi> tmp47;
    ca_.Bind(&block19, &tmp47);
    CodeStubAssembler(state_).FailAssert("Torque assert \'(GenericMacroTestWithLabels<Smi>(0) otherwise Fail) == Undefined\' failed", "../../test/torque/test-torque.tq", 158);
  }

  if (block18.is_used()) {
    compiler::TNode<Smi> tmp48;
    ca_.Bind(&block18, &tmp48);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 160);
    compiler::TNode<Oddball> tmp49;
    USE(tmp49);
    tmp49 = BaseBuiltinsFromDSLAssembler(state_).False();
    compiler::TNode<Object> tmp50;
    USE(tmp50);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp50 = TestBuiltinsFromDSLAssembler(state_).GenericMacroTestWithLabels20UT5ATSmi10HeapObject(compiler::TNode<Object>{tmp49}, &label0);
    ca_.Goto(&block24, tmp48, tmp49, tmp50);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block25, tmp48, tmp49);
    }
  }

  if (block25.is_used()) {
    compiler::TNode<Smi> tmp51;
    compiler::TNode<Oddball> tmp52;
    ca_.Bind(&block25, &tmp51, &tmp52);
    ca_.Goto(&block23, tmp51);
  }

  if (block24.is_used()) {
    compiler::TNode<Smi> tmp53;
    compiler::TNode<Oddball> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block24, &tmp53, &tmp54, &tmp55);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 162);
    ca_.Goto(&block22, tmp53);
  }

  if (block23.is_used()) {
    compiler::TNode<Smi> tmp56;
    ca_.Bind(&block23, &tmp56);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 159);
    ca_.Goto(&block22, tmp56);
  }

  if (block22.is_used()) {
    compiler::TNode<Smi> tmp57;
    ca_.Bind(&block22, &tmp57);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 150);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 164);
    ca_.Goto(&block2);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 165);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../test/torque/test-torque.tq:165:7");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 149);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block26);
  }

    ca_.Bind(&block26);
}

TF_BUILTIN(TestHelperPlus1, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Smi> parameter1 = UncheckedCast<Smi>(Parameter(Descriptor::kX));
  USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Smi> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 170);
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp1}, compiler::TNode<Smi>{tmp2}));
    CodeStubAssembler(state_).Return(tmp3);
  }
}

TF_BUILTIN(TestHelperPlus2, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Smi> parameter1 = UncheckedCast<Smi>(Parameter(Descriptor::kX));
  USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Smi> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 173);
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(2));
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp1}, compiler::TNode<Smi>{tmp2}));
    CodeStubAssembler(state_).Return(tmp3);
  }
}

compiler::TNode<Oddball> TestBuiltinsFromDSLAssembler::TestFunctionPointers(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Oddball> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Oddball> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 177);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 178);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(42));
    compiler::TNode<Smi> tmp2 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(8)).descriptor(), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kTestHelperPlus1)), tmp0, tmp1)); 
    USE(tmp2);
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(43));
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp2}, compiler::TNode<Smi>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kTestHelperPlus1)));
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<BuiltinPtr> tmp6;
    ca_.Bind(&block3, &tmp5, &tmp6);
    CodeStubAssembler(state_).FailAssert("Torque assert \'fptr(context, 42) == 43\' failed", "../../test/torque/test-torque.tq", 178);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<BuiltinPtr> tmp8;
    ca_.Bind(&block2, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 179);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 180);
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(42));
    compiler::TNode<Smi> tmp10 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(8)).descriptor(), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kTestHelperPlus2)), tmp7, tmp9)); 
    USE(tmp10);
    compiler::TNode<Smi> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(44));
    compiler::TNode<BoolT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp10}, compiler::TNode<Smi>{tmp11}));
    ca_.Branch(tmp12, &block4, &block5, tmp7, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kTestHelperPlus2)));
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<BuiltinPtr> tmp14;
    ca_.Bind(&block5, &tmp13, &tmp14);
    CodeStubAssembler(state_).FailAssert("Torque assert \'fptr(context, 42) == 44\' failed", "../../test/torque/test-torque.tq", 180);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<BuiltinPtr> tmp16;
    ca_.Bind(&block4, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 181);
    compiler::TNode<Oddball> tmp17;
    USE(tmp17);
    tmp17 = BaseBuiltinsFromDSLAssembler(state_).True();
    ca_.Goto(&block1, tmp15, tmp17);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Oddball> tmp19;
    ca_.Bind(&block1, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 176);
    ca_.Goto(&block6, tmp18, tmp19);
  }

    compiler::TNode<Context> tmp20;
    compiler::TNode<Oddball> tmp21;
    ca_.Bind(&block6, &tmp20, &tmp21);
  return compiler::TNode<Oddball>{tmp21};
}

compiler::TNode<Oddball> TestBuiltinsFromDSLAssembler::TestVariableRedeclaration(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Oddball> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Oddball> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 185);
    compiler::TNode<BoolT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool((CodeStubAssembler(state_).ConstexprInt31Equal(42, 0))));
    ca_.Branch(tmp1, &block2, &block3, tmp0);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp2;
    ca_.Bind(&block2, &tmp2);
    ca_.Goto(&block5, tmp2);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp3;
    ca_.Bind(&block3, &tmp3);
    compiler::TNode<Int32T> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3117ATconstexpr_int31(1));
    ca_.Goto(&block4, tmp3, tmp4);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp5;
    ca_.Bind(&block5, &tmp5);
    compiler::TNode<Int32T> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3117ATconstexpr_int31(0));
    ca_.Goto(&block4, tmp5, tmp6);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Int32T> tmp8;
    ca_.Bind(&block4, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 186);
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool((CodeStubAssembler(state_).ConstexprInt31Equal(42, 0))));
    ca_.Branch(tmp9, &block6, &block7, tmp7, tmp8);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Int32T> tmp11;
    ca_.Bind(&block6, &tmp10, &tmp11);
    ca_.Goto(&block9, tmp10, tmp11);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Int32T> tmp13;
    ca_.Bind(&block7, &tmp12, &tmp13);
    compiler::TNode<Int32T> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3117ATconstexpr_int31(0));
    ca_.Goto(&block8, tmp12, tmp13, tmp14);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Int32T> tmp16;
    ca_.Bind(&block9, &tmp15, &tmp16);
    compiler::TNode<Int32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3117ATconstexpr_int31(1));
    ca_.Goto(&block8, tmp15, tmp16, tmp17);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Int32T> tmp19;
    compiler::TNode<Int32T> tmp20;
    ca_.Bind(&block8, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 187);
    compiler::TNode<Oddball> tmp21;
    USE(tmp21);
    tmp21 = BaseBuiltinsFromDSLAssembler(state_).True();
    ca_.Goto(&block1, tmp18, tmp21);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<Oddball> tmp23;
    ca_.Bind(&block1, &tmp22, &tmp23);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 184);
    ca_.Goto(&block10, tmp22, tmp23);
  }

    compiler::TNode<Context> tmp24;
    compiler::TNode<Oddball> tmp25;
    ca_.Bind(&block10, &tmp24, &tmp25);
  return compiler::TNode<Oddball>{tmp25};
}

compiler::TNode<Smi> TestBuiltinsFromDSLAssembler::TestTernaryOperator(compiler::TNode<Smi> p_x) {
  compiler::CodeAssemblerParameterizedLabel<Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, BoolT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, BoolT, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, BoolT, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x);

  if (block0.is_used()) {
    compiler::TNode<Smi> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 191);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp0}, compiler::TNode<Smi>{tmp1}));
    ca_.Branch(tmp2, &block2, &block3, tmp0);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block2, &tmp3);
    ca_.Goto(&block5, tmp3);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp4;
    ca_.Bind(&block3, &tmp4);
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block4, tmp4, tmp5);
  }

  if (block5.is_used()) {
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block5, &tmp6);
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block4, tmp6, tmp7);
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp8;
    compiler::TNode<BoolT> tmp9;
    ca_.Bind(&block4, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 192);
    ca_.Branch(tmp9, &block6, &block7, tmp8, tmp9);
  }

  if (block6.is_used()) {
    compiler::TNode<Smi> tmp10;
    compiler::TNode<BoolT> tmp11;
    ca_.Bind(&block6, &tmp10, &tmp11);
    compiler::TNode<Smi> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<Smi> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp10}, compiler::TNode<Smi>{tmp12}));
    ca_.Goto(&block9, tmp10, tmp11, tmp13);
  }

  if (block7.is_used()) {
    compiler::TNode<Smi> tmp14;
    compiler::TNode<BoolT> tmp15;
    ca_.Bind(&block7, &tmp14, &tmp15);
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(100));
    compiler::TNode<Smi> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp14}, compiler::TNode<Smi>{tmp16}));
    ca_.Goto(&block8, tmp14, tmp15, tmp17);
  }

  if (block9.is_used()) {
    compiler::TNode<Smi> tmp18;
    compiler::TNode<BoolT> tmp19;
    compiler::TNode<Smi> tmp20;
    ca_.Bind(&block9, &tmp18, &tmp19, &tmp20);
    ca_.Goto(&block8, tmp18, tmp19, tmp20);
  }

  if (block8.is_used()) {
    compiler::TNode<Smi> tmp21;
    compiler::TNode<BoolT> tmp22;
    compiler::TNode<Smi> tmp23;
    ca_.Bind(&block8, &tmp21, &tmp22, &tmp23);
    ca_.Goto(&block1, tmp21, tmp23);
  }

  if (block1.is_used()) {
    compiler::TNode<Smi> tmp24;
    compiler::TNode<Smi> tmp25;
    ca_.Bind(&block1, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 190);
    ca_.Goto(&block10, tmp24, tmp25);
  }

    compiler::TNode<Smi> tmp26;
    compiler::TNode<Smi> tmp27;
    ca_.Bind(&block10, &tmp26, &tmp27);
  return compiler::TNode<Smi>{tmp27};
}

void TestBuiltinsFromDSLAssembler::TestFunctionPointerToGeneric(compiler::TNode<Context> p_c) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, BuiltinPtr> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, BuiltinPtr> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, BuiltinPtr> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, BuiltinPtr> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, BuiltinPtr> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, BuiltinPtr> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, BuiltinPtr> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_c);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 196);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 197);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 199);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp2 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(9)).descriptor(), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kGenericBuiltinTest5ATSmi)), tmp0, tmp1); 
    USE(tmp2);
    compiler::TNode<Oddball> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp2}, compiler::TNode<HeapObject>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kGenericBuiltinTest5ATSmi)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kGenericBuiltinTest20UT5ATSmi10HeapObject)));
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<BuiltinPtr> tmp6;
    compiler::TNode<BuiltinPtr> tmp7;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7);
    CodeStubAssembler(state_).FailAssert("Torque assert \'fptr1(c, 0) == Null\' failed", "../../test/torque/test-torque.tq", 199);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<BuiltinPtr> tmp9;
    compiler::TNode<BuiltinPtr> tmp10;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 200);
    compiler::TNode<Smi> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp12 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(9)).descriptor(), tmp9, tmp8, tmp11); 
    USE(tmp12);
    compiler::TNode<Oddball> tmp13;
    USE(tmp13);
    tmp13 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<BoolT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp12}, compiler::TNode<HeapObject>{tmp13}));
    ca_.Branch(tmp14, &block4, &block5, tmp8, tmp9, tmp10);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<BuiltinPtr> tmp16;
    compiler::TNode<BuiltinPtr> tmp17;
    ca_.Bind(&block5, &tmp15, &tmp16, &tmp17);
    CodeStubAssembler(state_).FailAssert("Torque assert \'fptr1(c, 1) == Null\' failed", "../../test/torque/test-torque.tq", 200);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<BuiltinPtr> tmp19;
    compiler::TNode<BuiltinPtr> tmp20;
    ca_.Bind(&block4, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 201);
    compiler::TNode<Oddball> tmp21;
    USE(tmp21);
    tmp21 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Object> tmp22 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(3)).descriptor(), tmp20, tmp18, tmp21); 
    USE(tmp22);
    compiler::TNode<Oddball> tmp23;
    USE(tmp23);
    tmp23 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp22}, compiler::TNode<HeapObject>{tmp23}));
    ca_.Branch(tmp24, &block6, &block7, tmp18, tmp19, tmp20);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<BuiltinPtr> tmp26;
    compiler::TNode<BuiltinPtr> tmp27;
    ca_.Bind(&block7, &tmp25, &tmp26, &tmp27);
    CodeStubAssembler(state_).FailAssert("Torque assert \'fptr2(c, Undefined) == Undefined\' failed", "../../test/torque/test-torque.tq", 201);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<BuiltinPtr> tmp29;
    compiler::TNode<BuiltinPtr> tmp30;
    ca_.Bind(&block6, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 202);
    compiler::TNode<Oddball> tmp31;
    USE(tmp31);
    tmp31 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Object> tmp32 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(3)).descriptor(), tmp30, tmp28, tmp31); 
    USE(tmp32);
    compiler::TNode<Oddball> tmp33;
    USE(tmp33);
    tmp33 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp32}, compiler::TNode<HeapObject>{tmp33}));
    ca_.Branch(tmp34, &block8, &block9, tmp28, tmp29, tmp30);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<BuiltinPtr> tmp36;
    compiler::TNode<BuiltinPtr> tmp37;
    ca_.Bind(&block9, &tmp35, &tmp36, &tmp37);
    CodeStubAssembler(state_).FailAssert("Torque assert \'fptr2(c, Undefined) == Undefined\' failed", "../../test/torque/test-torque.tq", 202);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<BuiltinPtr> tmp39;
    compiler::TNode<BuiltinPtr> tmp40;
    ca_.Bind(&block8, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 195);
    ca_.Goto(&block1, tmp38);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp41;
    ca_.Bind(&block1, &tmp41);
    ca_.Goto(&block10, tmp41);
  }

    compiler::TNode<Context> tmp42;
    ca_.Bind(&block10, &tmp42);
}

compiler::TNode<BuiltinPtr> TestBuiltinsFromDSLAssembler::TestTypeAlias(compiler::TNode<BuiltinPtr> p_x) {
  compiler::CodeAssemblerParameterizedLabel<BuiltinPtr> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BuiltinPtr, BuiltinPtr> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BuiltinPtr, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x);

  if (block0.is_used()) {
    compiler::TNode<BuiltinPtr> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 207);
    ca_.Goto(&block1, tmp0, tmp0);
  }

  if (block1.is_used()) {
    compiler::TNode<BuiltinPtr> tmp1;
    compiler::TNode<BuiltinPtr> tmp2;
    ca_.Bind(&block1, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 206);
    ca_.Goto(&block2, tmp1, tmp2);
  }

    compiler::TNode<BuiltinPtr> tmp3;
    compiler::TNode<BuiltinPtr> tmp4;
    ca_.Bind(&block2, &tmp3, &tmp4);
  return compiler::TNode<BuiltinPtr>{tmp4};
}

compiler::TNode<Oddball> TestBuiltinsFromDSLAssembler::TestUnsafeCast(compiler::TNode<Context> p_context, compiler::TNode<Number> p_n) {
  compiler::CodeAssemblerParameterizedLabel<Context, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Number, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Number, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Number, Oddball> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Number, Oddball> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_n);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Number> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 211);
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).TaggedIsSmi(compiler::TNode<Object>{tmp1}));
    ca_.Branch(tmp2, &block2, &block3, tmp0, tmp1);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Number> tmp4;
    ca_.Bind(&block2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 212);
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp3}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 214);
    compiler::TNode<Smi> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kTestHelperPlus1, tmp3, tmp5));
    USE(tmp6);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(11));
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp6}, compiler::TNode<Smi>{tmp7}));
    ca_.Branch(tmp8, &block4, &block5, tmp3, tmp4, tmp5);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<Number> tmp10;
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block5, &tmp9, &tmp10, &tmp11);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestHelperPlus1(context, m) == 11\' failed", "../../test/torque/test-torque.tq", 214);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<Smi> tmp14;
    ca_.Bind(&block4, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 215);
    compiler::TNode<Oddball> tmp15;
    USE(tmp15);
    tmp15 = BaseBuiltinsFromDSLAssembler(state_).True();
    ca_.Goto(&block1, tmp12, tmp13, tmp15);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<Number> tmp17;
    ca_.Bind(&block3, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 217);
    compiler::TNode<Oddball> tmp18;
    USE(tmp18);
    tmp18 = BaseBuiltinsFromDSLAssembler(state_).False();
    ca_.Goto(&block1, tmp16, tmp17, tmp18);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Oddball> tmp21;
    ca_.Bind(&block1, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 210);
    ca_.Goto(&block6, tmp19, tmp20, tmp21);
  }

    compiler::TNode<Context> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Oddball> tmp24;
    ca_.Bind(&block6, &tmp22, &tmp23, &tmp24);
  return compiler::TNode<Oddball>{tmp24};
}

void TestBuiltinsFromDSLAssembler::TestHexLiteral() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 221);
    compiler::TNode<IntPtrT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr17ATconstexpr_int31(0xffff));
    compiler::TNode<IntPtrT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp0}, compiler::TNode<IntPtrT>{tmp1}));
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0x10000));
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp2}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    CodeStubAssembler(state_).FailAssert("Torque assert \'Convert<intptr>(0xffff) + 1 == 0x10000\' failed", "../../test/torque/test-torque.tq", 221);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 222);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr17ATconstexpr_int31(-0xffff));
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(-65535));
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp5}, compiler::TNode<IntPtrT>{tmp6}));
    ca_.Branch(tmp7, &block4, &block5);
  }

  if (block5.is_used()) {
    ca_.Bind(&block5);
    CodeStubAssembler(state_).FailAssert("Torque assert \'Convert<intptr>(-0xffff) == -65535\' failed", "../../test/torque/test-torque.tq", 222);
  }

  if (block4.is_used()) {
    ca_.Bind(&block4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 220);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block6);
  }

    ca_.Bind(&block6);
}

void TestBuiltinsFromDSLAssembler::TestLargeIntegerLiterals(compiler::TNode<Context> p_c) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_c);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 226);
    compiler::TNode<Int32T> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int32(0x40000000));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 227);
    compiler::TNode<Int32T> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int32(0x7fffffff));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 225);
    ca_.Goto(&block1, tmp0);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp3;
    ca_.Bind(&block1, &tmp3);
    ca_.Goto(&block2, tmp3);
  }

    compiler::TNode<Context> tmp4;
    ca_.Bind(&block2, &tmp4);
}

void TestBuiltinsFromDSLAssembler::TestMultilineAssert() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 231);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 233);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp0}, compiler::TNode<Smi>{tmp1}));
    ca_.Branch(tmp2, &block4, &block3, tmp0);
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block4, &tmp3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 234);
    compiler::TNode<Smi> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp3}, compiler::TNode<Smi>{tmp4}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 233);
    ca_.Branch(tmp5, &block2, &block3, tmp3);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block3, &tmp6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 232);
    CodeStubAssembler(state_).FailAssert("Torque assert \'someVeryLongVariableNameThatWillCauseLineBreaks > 0 && someVeryLongVariableNameThatWillCauseLineBreaks < 10\' failed", "../../test/torque/test-torque.tq", 232);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block2, &tmp7);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 230);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block5);
  }

    ca_.Bind(&block5);
}

void TestBuiltinsFromDSLAssembler::TestNewlineInString() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 238);
    CodeStubAssembler(state_).Print("Hello, World!\n");
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 237);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
}

int31_t TestBuiltinsFromDSLAssembler::kConstexprConst() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return 5;
}

compiler::TNode<IntPtrT> TestBuiltinsFromDSLAssembler::kIntptrConst() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 242);
    compiler::TNode<IntPtrT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(4));
return compiler::TNode<IntPtrT>{tmp0};
}

compiler::TNode<Smi> TestBuiltinsFromDSLAssembler::kSmiConst() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 243);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
return compiler::TNode<Smi>{tmp0};
}

void TestBuiltinsFromDSLAssembler::TestModuleConstBindings() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 246);
    compiler::TNode<Int32T> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Constant(5));
    compiler::TNode<Int32T> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(TestBuiltinsFromDSLAssembler(state_).kConstexprConst()));
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp1}, compiler::TNode<Int32T>{tmp0}));
    ca_.Branch(tmp2, &block2, &block3);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    CodeStubAssembler(state_).FailAssert("Torque assert \'kConstexprConst == Int32Constant(5)\' failed", "../../test/torque/test-torque.tq", 246);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 247);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = TestBuiltinsFromDSLAssembler(state_).kIntptrConst();
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(4));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}));
    ca_.Branch(tmp5, &block4, &block5);
  }

  if (block5.is_used()) {
    ca_.Bind(&block5);
    CodeStubAssembler(state_).FailAssert("Torque assert \'kIntptrConst == 4\' failed", "../../test/torque/test-torque.tq", 247);
  }

  if (block4.is_used()) {
    ca_.Bind(&block4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 248);
    compiler::TNode<Smi> tmp6;
    USE(tmp6);
    tmp6 = TestBuiltinsFromDSLAssembler(state_).kSmiConst();
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp6}, compiler::TNode<Smi>{tmp7}));
    ca_.Branch(tmp8, &block6, &block7);
  }

  if (block7.is_used()) {
    ca_.Bind(&block7);
    CodeStubAssembler(state_).FailAssert("Torque assert \'kSmiConst == 3\' failed", "../../test/torque/test-torque.tq", 248);
  }

  if (block6.is_used()) {
    ca_.Bind(&block6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 245);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block8);
  }

    ca_.Bind(&block8);
}

void TestBuiltinsFromDSLAssembler::TestLocalConstBindings() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block11(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 253);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 255);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp2}, compiler::TNode<Smi>{tmp1}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 256);
    compiler::TNode<Smi> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp0}, compiler::TNode<Smi>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp3}, compiler::TNode<Smi>{tmp5}));
    ca_.Branch(tmp6, &block2, &block3, tmp0, tmp3);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Smi> tmp8;
    ca_.Bind(&block3, &tmp7, &tmp8);
    CodeStubAssembler(state_).FailAssert("Torque assert \'x == xSmi + 1\' failed", "../../test/torque/test-torque.tq", 256);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block2, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 257);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 258);
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp10}, compiler::TNode<Smi>{tmp10}));
    ca_.Branch(tmp11, &block4, &block5, tmp9, tmp10, tmp10);
  }

  if (block5.is_used()) {
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    ca_.Bind(&block5, &tmp12, &tmp13, &tmp14);
    CodeStubAssembler(state_).FailAssert("Torque assert \'x == xSmi\' failed", "../../test/torque/test-torque.tq", 258);
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Smi> tmp17;
    ca_.Bind(&block4, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 259);
    compiler::TNode<Smi> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(4));
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp16}, compiler::TNode<Smi>{tmp18}));
    ca_.Branch(tmp19, &block6, &block7, tmp15, tmp16, tmp17);
  }

  if (block7.is_used()) {
    compiler::TNode<Smi> tmp20;
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Smi> tmp22;
    ca_.Bind(&block7, &tmp20, &tmp21, &tmp22);
    CodeStubAssembler(state_).FailAssert("Torque assert \'x == 4\' failed", "../../test/torque/test-torque.tq", 259);
  }

  if (block6.is_used()) {
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Smi> tmp24;
    compiler::TNode<Smi> tmp25;
    ca_.Bind(&block6, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 254);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 261);
    compiler::TNode<Smi> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    compiler::TNode<BoolT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp23}, compiler::TNode<Smi>{tmp26}));
    ca_.Branch(tmp27, &block8, &block9, tmp23);
  }

  if (block9.is_used()) {
    compiler::TNode<Smi> tmp28;
    ca_.Bind(&block9, &tmp28);
    CodeStubAssembler(state_).FailAssert("Torque assert \'xSmi == 3\' failed", "../../test/torque/test-torque.tq", 261);
  }

  if (block8.is_used()) {
    compiler::TNode<Smi> tmp29;
    ca_.Bind(&block8, &tmp29);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 262);
    compiler::TNode<Smi> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp30}, compiler::TNode<Smi>{tmp29}));
    ca_.Branch(tmp31, &block10, &block11, tmp29);
  }

  if (block11.is_used()) {
    compiler::TNode<Smi> tmp32;
    ca_.Bind(&block11, &tmp32);
    CodeStubAssembler(state_).FailAssert("Torque assert \'x == xSmi\' failed", "../../test/torque/test-torque.tq", 262);
  }

  if (block10.is_used()) {
    compiler::TNode<Smi> tmp33;
    ca_.Bind(&block10, &tmp33);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 251);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block12);
  }

    ca_.Bind(&block12);
}

compiler::TNode<Smi> TestBuiltinsFromDSLAssembler::TestStruct1(TestBuiltinsFromDSLAssembler::TestStructA p_i) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Number, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Number, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_i.indexes, p_i.i, p_i.k);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<Smi> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 277);
    ca_.Goto(&block1, tmp0, tmp1, tmp2, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Number> tmp5;
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block1, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 276);
    ca_.Goto(&block2, tmp3, tmp4, tmp5, tmp6);
  }

    compiler::TNode<FixedArray> tmp7;
    compiler::TNode<Smi> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
  return compiler::TNode<Smi>{tmp10};
}

TestBuiltinsFromDSLAssembler::TestStructA TestBuiltinsFromDSLAssembler::TestStruct2(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, Smi, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, Smi, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 281);
    compiler::TNode<FixedArray> tmp1;
    USE(tmp1);
    tmp1 = BaseBuiltinsFromDSLAssembler(state_).kEmptyFixedArray();
    compiler::TNode<FixedArray> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(27));
    compiler::TNode<Number> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(31));
    ca_.Goto(&block1, tmp0, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<FixedArray> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Number> tmp8;
    ca_.Bind(&block1, &tmp5, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 280);
    ca_.Goto(&block2, tmp5, tmp6, tmp7, tmp8);
  }

    compiler::TNode<Context> tmp9;
    compiler::TNode<FixedArray> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<Number> tmp12;
    ca_.Bind(&block2, &tmp9, &tmp10, &tmp11, &tmp12);
  return TestBuiltinsFromDSLAssembler::TestStructA{compiler::TNode<FixedArray>{tmp10}, compiler::TNode<Smi>{tmp11}, compiler::TNode<Number>{tmp12}};
}

TestBuiltinsFromDSLAssembler::TestStructA TestBuiltinsFromDSLAssembler::TestStruct3(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, Smi, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, Smi, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 286);
    compiler::TNode<FixedArray> tmp1;
    USE(tmp1);
    tmp1 = BaseBuiltinsFromDSLAssembler(state_).kEmptyFixedArray();
    compiler::TNode<FixedArray> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(13));
    compiler::TNode<Number> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(5));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 285);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 287);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 288);
    compiler::TNode<FixedArray> tmp5;
    USE(tmp5);
    compiler::TNode<Smi> tmp6;
    USE(tmp6);
    compiler::TNode<Number> tmp7;
    USE(tmp7);
    std::tie(tmp5, tmp6, tmp7) = TestBuiltinsFromDSLAssembler(state_).TestStruct2(compiler::TNode<Context>{tmp0}).Flatten();
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 289);
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Smi>(TestBuiltinsFromDSLAssembler(state_).TestStruct1(TestBuiltinsFromDSLAssembler::TestStructA{compiler::TNode<FixedArray>{tmp5}, compiler::TNode<Smi>{tmp6}, compiler::TNode<Number>{tmp7}}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 290);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 291);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 292);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 293);
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(7));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 294);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 296);
    compiler::TNode<FixedArray> tmp10;
    USE(tmp10);
    tmp10 = BaseBuiltinsFromDSLAssembler(state_).kEmptyFixedArray();
    compiler::TNode<FixedArray> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp10}));
    compiler::TNode<Smi> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(27));
    compiler::TNode<Number> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(31));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 295);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 297);
    compiler::TNode<FixedArray> tmp14;
    USE(tmp14);
    compiler::TNode<Smi> tmp15;
    USE(tmp15);
    compiler::TNode<Number> tmp16;
    USE(tmp16);
    std::tie(tmp14, tmp15, tmp16) = TestBuiltinsFromDSLAssembler(state_).TestStruct2(compiler::TNode<Context>{tmp0}).Flatten();
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 298);
    ca_.Goto(&block1, tmp0, tmp2, tmp8, tmp8);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<FixedArray> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Number> tmp20;
    ca_.Bind(&block1, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 284);
    ca_.Goto(&block2, tmp17, tmp18, tmp19, tmp20);
  }

    compiler::TNode<Context> tmp21;
    compiler::TNode<FixedArray> tmp22;
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Number> tmp24;
    ca_.Bind(&block2, &tmp21, &tmp22, &tmp23, &tmp24);
  return TestBuiltinsFromDSLAssembler::TestStructA{compiler::TNode<FixedArray>{tmp22}, compiler::TNode<Smi>{tmp23}, compiler::TNode<Number>{tmp24}};
}

TestBuiltinsFromDSLAssembler::TestStructC TestBuiltinsFromDSLAssembler::TestStruct4(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, Smi, Number, FixedArray, Smi, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, Smi, Number, FixedArray, Smi, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 307);
    compiler::TNode<FixedArray> tmp1;
    USE(tmp1);
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    compiler::TNode<Number> tmp3;
    USE(tmp3);
    std::tie(tmp1, tmp2, tmp3) = TestBuiltinsFromDSLAssembler(state_).TestStruct2(compiler::TNode<Context>{tmp0}).Flatten();
    compiler::TNode<FixedArray> tmp4;
    USE(tmp4);
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    std::tie(tmp4, tmp5, tmp6) = TestBuiltinsFromDSLAssembler(state_).TestStruct2(compiler::TNode<Context>{tmp0}).Flatten();
    ca_.Goto(&block1, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<FixedArray> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Number> tmp10;
    compiler::TNode<FixedArray> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Number> tmp13;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 306);
    ca_.Goto(&block2, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13);
  }

    compiler::TNode<Context> tmp14;
    compiler::TNode<FixedArray> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<FixedArray> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Number> tmp20;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
  return TestBuiltinsFromDSLAssembler::TestStructC{TestBuiltinsFromDSLAssembler::TestStructA{compiler::TNode<FixedArray>{tmp15}, compiler::TNode<Smi>{tmp16}, compiler::TNode<Number>{tmp17}}, TestBuiltinsFromDSLAssembler::TestStructA{compiler::TNode<FixedArray>{tmp18}, compiler::TNode<Smi>{tmp19}, compiler::TNode<Number>{tmp20}}};
}

void TestBuiltinsFromDSLAssembler::CallTestStructInLabel(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, Smi, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, Smi, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 316);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 312);
    compiler::TNode<FixedArray> tmp1;
    USE(tmp1);
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    compiler::TNode<Number> tmp3;
    USE(tmp3);
    std::tie(tmp1, tmp2, tmp3) = TestBuiltinsFromDSLAssembler(state_).TestStruct2(compiler::TNode<Context>{tmp0}).Flatten();
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp4;
    compiler::TNode<FixedArray> tmp5;
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Number> tmp7;
    ca_.Bind(&block3, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 315);
    ca_.Goto(&block2, tmp4, tmp5, tmp6, tmp7);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<FixedArray> tmp9;
    compiler::TNode<Smi> tmp10;
    compiler::TNode<Number> tmp11;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 314);
    ca_.Goto(&block1, tmp8);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp12;
    ca_.Bind(&block1, &tmp12);
    ca_.Goto(&block4, tmp12);
  }

    compiler::TNode<Context> tmp13;
    ca_.Bind(&block4, &tmp13);
}

void TestBuiltinsFromDSLAssembler::TestForLoop() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block13(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block18(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block26(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block33(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block41(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block44(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block46(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block47(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block45(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block43(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block49(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block48(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block52(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block50(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block53(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block54(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block51(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block56(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block55(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block59(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block57(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block61(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block62(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block63(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block64(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block60(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block58(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block66(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block65(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block69(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block67(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block70(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block71(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block72(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block73(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block68(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block75(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block74(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block80(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block78(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block81(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block82(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block79(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block77(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block84(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block83(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block76(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi, Smi> block87(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi, Smi> block85(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi, Smi> block88(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi, Smi> block86(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block89(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 324);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 325);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block4, tmp0, tmp1);
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block4, &tmp2, &tmp3);
    compiler::TNode<Smi> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp3}, compiler::TNode<Smi>{tmp4}));
    ca_.Branch(tmp5, &block2, &block3, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block2, &tmp6, &tmp7);
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp6}, compiler::TNode<Smi>{tmp7}));
    ca_.Goto(&block5, tmp8, tmp7);
  }

  if (block5.is_used()) {
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block5, &tmp9, &tmp10);
    compiler::TNode<Smi> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp10}, compiler::TNode<Smi>{tmp11}));
    ca_.Goto(&block4, tmp9, tmp12);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    ca_.Bind(&block3, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 326);
    compiler::TNode<Smi> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp13}, compiler::TNode<Smi>{tmp15}));
    ca_.Branch(tmp16, &block6, &block7, tmp13);
  }

  if (block7.is_used()) {
    compiler::TNode<Smi> tmp17;
    ca_.Bind(&block7, &tmp17);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 10\' failed", "../../test/torque/test-torque.tq", 326);
  }

  if (block6.is_used()) {
    compiler::TNode<Smi> tmp18;
    ca_.Bind(&block6, &tmp18);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 328);
    compiler::TNode<Smi> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 329);
    compiler::TNode<Smi> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 330);
    ca_.Goto(&block10, tmp19, tmp20);
  }

  if (block10.is_used()) {
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Smi> tmp22;
    ca_.Bind(&block10, &tmp21, &tmp22);
    compiler::TNode<Smi> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp22}, compiler::TNode<Smi>{tmp23}));
    ca_.Branch(tmp24, &block8, &block9, tmp21, tmp22);
  }

  if (block8.is_used()) {
    compiler::TNode<Smi> tmp25;
    compiler::TNode<Smi> tmp26;
    ca_.Bind(&block8, &tmp25, &tmp26);
    compiler::TNode<Smi> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp25}, compiler::TNode<Smi>{tmp26}));
    ca_.Goto(&block11, tmp27, tmp26);
  }

  if (block11.is_used()) {
    compiler::TNode<Smi> tmp28;
    compiler::TNode<Smi> tmp29;
    ca_.Bind(&block11, &tmp28, &tmp29);
    compiler::TNode<Smi> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp29}, compiler::TNode<Smi>{tmp30}));
    ca_.Goto(&block10, tmp28, tmp31);
  }

  if (block9.is_used()) {
    compiler::TNode<Smi> tmp32;
    compiler::TNode<Smi> tmp33;
    ca_.Bind(&block9, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 331);
    compiler::TNode<Smi> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp32}, compiler::TNode<Smi>{tmp34}));
    ca_.Branch(tmp35, &block12, &block13, tmp32, tmp33);
  }

  if (block13.is_used()) {
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Smi> tmp37;
    ca_.Bind(&block13, &tmp36, &tmp37);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 10\' failed", "../../test/torque/test-torque.tq", 331);
  }

  if (block12.is_used()) {
    compiler::TNode<Smi> tmp38;
    compiler::TNode<Smi> tmp39;
    ca_.Bind(&block12, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 333);
    compiler::TNode<Smi> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 334);
    compiler::TNode<Smi> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 335);
    ca_.Goto(&block16, tmp40, tmp41);
  }

  if (block16.is_used()) {
    compiler::TNode<Smi> tmp42;
    compiler::TNode<Smi> tmp43;
    ca_.Bind(&block16, &tmp42, &tmp43);
    compiler::TNode<Smi> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp43}, compiler::TNode<Smi>{tmp44}));
    ca_.Branch(tmp45, &block14, &block15, tmp42, tmp43);
  }

  if (block14.is_used()) {
    compiler::TNode<Smi> tmp46;
    compiler::TNode<Smi> tmp47;
    ca_.Bind(&block14, &tmp46, &tmp47);
    compiler::TNode<Smi> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp47}, compiler::TNode<Smi>{tmp48}));
    compiler::TNode<Smi> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp46}, compiler::TNode<Smi>{tmp47}));
    ca_.Goto(&block16, tmp50, tmp49);
  }

  if (block15.is_used()) {
    compiler::TNode<Smi> tmp51;
    compiler::TNode<Smi> tmp52;
    ca_.Bind(&block15, &tmp51, &tmp52);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 336);
    compiler::TNode<Smi> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp51}, compiler::TNode<Smi>{tmp53}));
    ca_.Branch(tmp54, &block17, &block18, tmp51, tmp52);
  }

  if (block18.is_used()) {
    compiler::TNode<Smi> tmp55;
    compiler::TNode<Smi> tmp56;
    ca_.Bind(&block18, &tmp55, &tmp56);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 10\' failed", "../../test/torque/test-torque.tq", 336);
  }

  if (block17.is_used()) {
    compiler::TNode<Smi> tmp57;
    compiler::TNode<Smi> tmp58;
    ca_.Bind(&block17, &tmp57, &tmp58);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 339);
    compiler::TNode<Smi> tmp59;
    USE(tmp59);
    tmp59 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 340);
    compiler::TNode<Smi> tmp60;
    USE(tmp60);
    tmp60 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block21, tmp59, tmp58, tmp60);
  }

  if (block21.is_used()) {
    compiler::TNode<Smi> tmp61;
    compiler::TNode<Smi> tmp62;
    compiler::TNode<Smi> tmp63;
    ca_.Bind(&block21, &tmp61, &tmp62, &tmp63);
    ca_.Goto(&block19, tmp61, tmp62, tmp63);
  }

  if (block19.is_used()) {
    compiler::TNode<Smi> tmp64;
    compiler::TNode<Smi> tmp65;
    compiler::TNode<Smi> tmp66;
    ca_.Bind(&block19, &tmp64, &tmp65, &tmp66);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 341);
    compiler::TNode<Smi> tmp67;
    USE(tmp67);
    tmp67 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp66}, compiler::TNode<Smi>{tmp67}));
    ca_.Branch(tmp68, &block23, &block24, tmp64, tmp65, tmp66);
  }

  if (block23.is_used()) {
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<Smi> tmp71;
    ca_.Bind(&block23, &tmp69, &tmp70, &tmp71);
    ca_.Goto(&block20, tmp69, tmp70, tmp71);
  }

  if (block24.is_used()) {
    compiler::TNode<Smi> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<Smi> tmp74;
    ca_.Bind(&block24, &tmp72, &tmp73, &tmp74);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 342);
    compiler::TNode<Smi> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp72}, compiler::TNode<Smi>{tmp74}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 340);
    ca_.Goto(&block22, tmp75, tmp73, tmp74);
  }

  if (block22.is_used()) {
    compiler::TNode<Smi> tmp76;
    compiler::TNode<Smi> tmp77;
    compiler::TNode<Smi> tmp78;
    ca_.Bind(&block22, &tmp76, &tmp77, &tmp78);
    compiler::TNode<Smi> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp78}, compiler::TNode<Smi>{tmp79}));
    ca_.Goto(&block21, tmp76, tmp77, tmp80);
  }

  if (block20.is_used()) {
    compiler::TNode<Smi> tmp81;
    compiler::TNode<Smi> tmp82;
    compiler::TNode<Smi> tmp83;
    ca_.Bind(&block20, &tmp81, &tmp82, &tmp83);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 344);
    compiler::TNode<Smi> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp85;
    USE(tmp85);
    tmp85 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp81}, compiler::TNode<Smi>{tmp84}));
    ca_.Branch(tmp85, &block25, &block26, tmp81, tmp82);
  }

  if (block26.is_used()) {
    compiler::TNode<Smi> tmp86;
    compiler::TNode<Smi> tmp87;
    ca_.Bind(&block26, &tmp86, &tmp87);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 10\' failed", "../../test/torque/test-torque.tq", 344);
  }

  if (block25.is_used()) {
    compiler::TNode<Smi> tmp88;
    compiler::TNode<Smi> tmp89;
    ca_.Bind(&block25, &tmp88, &tmp89);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 346);
    compiler::TNode<Smi> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 347);
    compiler::TNode<Smi> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 348);
    ca_.Goto(&block29, tmp90, tmp91);
  }

  if (block29.is_used()) {
    compiler::TNode<Smi> tmp92;
    compiler::TNode<Smi> tmp93;
    ca_.Bind(&block29, &tmp92, &tmp93);
    ca_.Goto(&block27, tmp92, tmp93);
  }

  if (block27.is_used()) {
    compiler::TNode<Smi> tmp94;
    compiler::TNode<Smi> tmp95;
    ca_.Bind(&block27, &tmp94, &tmp95);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 349);
    compiler::TNode<Smi> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp97;
    USE(tmp97);
    tmp97 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp95}, compiler::TNode<Smi>{tmp96}));
    ca_.Branch(tmp97, &block30, &block31, tmp94, tmp95);
  }

  if (block30.is_used()) {
    compiler::TNode<Smi> tmp98;
    compiler::TNode<Smi> tmp99;
    ca_.Bind(&block30, &tmp98, &tmp99);
    ca_.Goto(&block28, tmp98, tmp99);
  }

  if (block31.is_used()) {
    compiler::TNode<Smi> tmp100;
    compiler::TNode<Smi> tmp101;
    ca_.Bind(&block31, &tmp100, &tmp101);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 350);
    compiler::TNode<Smi> tmp102;
    USE(tmp102);
    tmp102 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp100}, compiler::TNode<Smi>{tmp101}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 351);
    compiler::TNode<Smi> tmp103;
    USE(tmp103);
    tmp103 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp104;
    USE(tmp104);
    tmp104 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp101}, compiler::TNode<Smi>{tmp103}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 348);
    ca_.Goto(&block29, tmp102, tmp104);
  }

  if (block28.is_used()) {
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Smi> tmp106;
    ca_.Bind(&block28, &tmp105, &tmp106);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 353);
    compiler::TNode<Smi> tmp107;
    USE(tmp107);
    tmp107 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp108;
    USE(tmp108);
    tmp108 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp105}, compiler::TNode<Smi>{tmp107}));
    ca_.Branch(tmp108, &block32, &block33, tmp105, tmp106);
  }

  if (block33.is_used()) {
    compiler::TNode<Smi> tmp109;
    compiler::TNode<Smi> tmp110;
    ca_.Bind(&block33, &tmp109, &tmp110);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 10\' failed", "../../test/torque/test-torque.tq", 353);
  }

  if (block32.is_used()) {
    compiler::TNode<Smi> tmp111;
    compiler::TNode<Smi> tmp112;
    ca_.Bind(&block32, &tmp111, &tmp112);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 357);
    compiler::TNode<Smi> tmp113;
    USE(tmp113);
    tmp113 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 358);
    compiler::TNode<Smi> tmp114;
    USE(tmp114);
    tmp114 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block36, tmp113, tmp112, tmp114);
  }

  if (block36.is_used()) {
    compiler::TNode<Smi> tmp115;
    compiler::TNode<Smi> tmp116;
    compiler::TNode<Smi> tmp117;
    ca_.Bind(&block36, &tmp115, &tmp116, &tmp117);
    compiler::TNode<Smi> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp117}, compiler::TNode<Smi>{tmp118}));
    ca_.Branch(tmp119, &block34, &block35, tmp115, tmp116, tmp117);
  }

  if (block34.is_used()) {
    compiler::TNode<Smi> tmp120;
    compiler::TNode<Smi> tmp121;
    compiler::TNode<Smi> tmp122;
    ca_.Bind(&block34, &tmp120, &tmp121, &tmp122);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 359);
    compiler::TNode<Smi> tmp123;
    USE(tmp123);
    tmp123 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    compiler::TNode<BoolT> tmp124;
    USE(tmp124);
    tmp124 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp122}, compiler::TNode<Smi>{tmp123}));
    ca_.Branch(tmp124, &block38, &block39, tmp120, tmp121, tmp122);
  }

  if (block38.is_used()) {
    compiler::TNode<Smi> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Smi> tmp127;
    ca_.Bind(&block38, &tmp125, &tmp126, &tmp127);
    ca_.Goto(&block37, tmp125, tmp126, tmp127);
  }

  if (block39.is_used()) {
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<Smi> tmp130;
    ca_.Bind(&block39, &tmp128, &tmp129, &tmp130);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 360);
    compiler::TNode<Smi> tmp131;
    USE(tmp131);
    tmp131 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp128}, compiler::TNode<Smi>{tmp130}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 358);
    ca_.Goto(&block37, tmp131, tmp129, tmp130);
  }

  if (block37.is_used()) {
    compiler::TNode<Smi> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<Smi> tmp134;
    ca_.Bind(&block37, &tmp132, &tmp133, &tmp134);
    compiler::TNode<Smi> tmp135;
    USE(tmp135);
    tmp135 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp136;
    USE(tmp136);
    tmp136 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp134}, compiler::TNode<Smi>{tmp135}));
    ca_.Goto(&block36, tmp132, tmp133, tmp136);
  }

  if (block35.is_used()) {
    compiler::TNode<Smi> tmp137;
    compiler::TNode<Smi> tmp138;
    compiler::TNode<Smi> tmp139;
    ca_.Bind(&block35, &tmp137, &tmp138, &tmp139);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 362);
    compiler::TNode<Smi> tmp140;
    USE(tmp140);
    tmp140 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(7));
    compiler::TNode<BoolT> tmp141;
    USE(tmp141);
    tmp141 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp137}, compiler::TNode<Smi>{tmp140}));
    ca_.Branch(tmp141, &block40, &block41, tmp137, tmp138);
  }

  if (block41.is_used()) {
    compiler::TNode<Smi> tmp142;
    compiler::TNode<Smi> tmp143;
    ca_.Bind(&block41, &tmp142, &tmp143);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 7\' failed", "../../test/torque/test-torque.tq", 362);
  }

  if (block40.is_used()) {
    compiler::TNode<Smi> tmp144;
    compiler::TNode<Smi> tmp145;
    ca_.Bind(&block40, &tmp144, &tmp145);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 364);
    compiler::TNode<Smi> tmp146;
    USE(tmp146);
    tmp146 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 365);
    compiler::TNode<Smi> tmp147;
    USE(tmp147);
    tmp147 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 366);
    ca_.Goto(&block44, tmp146, tmp147);
  }

  if (block44.is_used()) {
    compiler::TNode<Smi> tmp148;
    compiler::TNode<Smi> tmp149;
    ca_.Bind(&block44, &tmp148, &tmp149);
    compiler::TNode<Smi> tmp150;
    USE(tmp150);
    tmp150 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp151;
    USE(tmp151);
    tmp151 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp149}, compiler::TNode<Smi>{tmp150}));
    ca_.Branch(tmp151, &block42, &block43, tmp148, tmp149);
  }

  if (block42.is_used()) {
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    ca_.Bind(&block42, &tmp152, &tmp153);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 367);
    compiler::TNode<Smi> tmp154;
    USE(tmp154);
    tmp154 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    compiler::TNode<BoolT> tmp155;
    USE(tmp155);
    tmp155 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp153}, compiler::TNode<Smi>{tmp154}));
    ca_.Branch(tmp155, &block46, &block47, tmp152, tmp153);
  }

  if (block46.is_used()) {
    compiler::TNode<Smi> tmp156;
    compiler::TNode<Smi> tmp157;
    ca_.Bind(&block46, &tmp156, &tmp157);
    ca_.Goto(&block45, tmp156, tmp157);
  }

  if (block47.is_used()) {
    compiler::TNode<Smi> tmp158;
    compiler::TNode<Smi> tmp159;
    ca_.Bind(&block47, &tmp158, &tmp159);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 368);
    compiler::TNode<Smi> tmp160;
    USE(tmp160);
    tmp160 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp158}, compiler::TNode<Smi>{tmp159}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 366);
    ca_.Goto(&block45, tmp160, tmp159);
  }

  if (block45.is_used()) {
    compiler::TNode<Smi> tmp161;
    compiler::TNode<Smi> tmp162;
    ca_.Bind(&block45, &tmp161, &tmp162);
    compiler::TNode<Smi> tmp163;
    USE(tmp163);
    tmp163 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp164;
    USE(tmp164);
    tmp164 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp162}, compiler::TNode<Smi>{tmp163}));
    ca_.Goto(&block44, tmp161, tmp164);
  }

  if (block43.is_used()) {
    compiler::TNode<Smi> tmp165;
    compiler::TNode<Smi> tmp166;
    ca_.Bind(&block43, &tmp165, &tmp166);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 370);
    compiler::TNode<Smi> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(7));
    compiler::TNode<BoolT> tmp168;
    USE(tmp168);
    tmp168 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp165}, compiler::TNode<Smi>{tmp167}));
    ca_.Branch(tmp168, &block48, &block49, tmp165, tmp166);
  }

  if (block49.is_used()) {
    compiler::TNode<Smi> tmp169;
    compiler::TNode<Smi> tmp170;
    ca_.Bind(&block49, &tmp169, &tmp170);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 7\' failed", "../../test/torque/test-torque.tq", 370);
  }

  if (block48.is_used()) {
    compiler::TNode<Smi> tmp171;
    compiler::TNode<Smi> tmp172;
    ca_.Bind(&block48, &tmp171, &tmp172);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 372);
    compiler::TNode<Smi> tmp173;
    USE(tmp173);
    tmp173 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 373);
    compiler::TNode<Smi> tmp174;
    USE(tmp174);
    tmp174 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 374);
    ca_.Goto(&block52, tmp173, tmp174);
  }

  if (block52.is_used()) {
    compiler::TNode<Smi> tmp175;
    compiler::TNode<Smi> tmp176;
    ca_.Bind(&block52, &tmp175, &tmp176);
    compiler::TNode<Smi> tmp177;
    USE(tmp177);
    tmp177 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp178;
    USE(tmp178);
    tmp178 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp176}, compiler::TNode<Smi>{tmp177}));
    ca_.Branch(tmp178, &block50, &block51, tmp175, tmp176);
  }

  if (block50.is_used()) {
    compiler::TNode<Smi> tmp179;
    compiler::TNode<Smi> tmp180;
    ca_.Bind(&block50, &tmp179, &tmp180);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 375);
    compiler::TNode<Smi> tmp181;
    USE(tmp181);
    tmp181 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    compiler::TNode<BoolT> tmp182;
    USE(tmp182);
    tmp182 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp180}, compiler::TNode<Smi>{tmp181}));
    ca_.Branch(tmp182, &block53, &block54, tmp179, tmp180);
  }

  if (block53.is_used()) {
    compiler::TNode<Smi> tmp183;
    compiler::TNode<Smi> tmp184;
    ca_.Bind(&block53, &tmp183, &tmp184);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 376);
    compiler::TNode<Smi> tmp185;
    USE(tmp185);
    tmp185 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp186;
    USE(tmp186);
    tmp186 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp184}, compiler::TNode<Smi>{tmp185}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 377);
    ca_.Goto(&block52, tmp183, tmp186);
  }

  if (block54.is_used()) {
    compiler::TNode<Smi> tmp187;
    compiler::TNode<Smi> tmp188;
    ca_.Bind(&block54, &tmp187, &tmp188);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 379);
    compiler::TNode<Smi> tmp189;
    USE(tmp189);
    tmp189 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp187}, compiler::TNode<Smi>{tmp188}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 380);
    compiler::TNode<Smi> tmp190;
    USE(tmp190);
    tmp190 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp191;
    USE(tmp191);
    tmp191 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp188}, compiler::TNode<Smi>{tmp190}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 374);
    ca_.Goto(&block52, tmp189, tmp191);
  }

  if (block51.is_used()) {
    compiler::TNode<Smi> tmp192;
    compiler::TNode<Smi> tmp193;
    ca_.Bind(&block51, &tmp192, &tmp193);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 382);
    compiler::TNode<Smi> tmp194;
    USE(tmp194);
    tmp194 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(7));
    compiler::TNode<BoolT> tmp195;
    USE(tmp195);
    tmp195 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp192}, compiler::TNode<Smi>{tmp194}));
    ca_.Branch(tmp195, &block55, &block56, tmp192, tmp193);
  }

  if (block56.is_used()) {
    compiler::TNode<Smi> tmp196;
    compiler::TNode<Smi> tmp197;
    ca_.Bind(&block56, &tmp196, &tmp197);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 7\' failed", "../../test/torque/test-torque.tq", 382);
  }

  if (block55.is_used()) {
    compiler::TNode<Smi> tmp198;
    compiler::TNode<Smi> tmp199;
    ca_.Bind(&block55, &tmp198, &tmp199);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 384);
    compiler::TNode<Smi> tmp200;
    USE(tmp200);
    tmp200 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 385);
    compiler::TNode<Smi> tmp201;
    USE(tmp201);
    tmp201 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block59, tmp200, tmp199, tmp201);
  }

  if (block59.is_used()) {
    compiler::TNode<Smi> tmp202;
    compiler::TNode<Smi> tmp203;
    compiler::TNode<Smi> tmp204;
    ca_.Bind(&block59, &tmp202, &tmp203, &tmp204);
    ca_.Goto(&block57, tmp202, tmp203, tmp204);
  }

  if (block57.is_used()) {
    compiler::TNode<Smi> tmp205;
    compiler::TNode<Smi> tmp206;
    compiler::TNode<Smi> tmp207;
    ca_.Bind(&block57, &tmp205, &tmp206, &tmp207);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 386);
    compiler::TNode<Smi> tmp208;
    USE(tmp208);
    tmp208 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    compiler::TNode<BoolT> tmp209;
    USE(tmp209);
    tmp209 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp207}, compiler::TNode<Smi>{tmp208}));
    ca_.Branch(tmp209, &block61, &block62, tmp205, tmp206, tmp207);
  }

  if (block61.is_used()) {
    compiler::TNode<Smi> tmp210;
    compiler::TNode<Smi> tmp211;
    compiler::TNode<Smi> tmp212;
    ca_.Bind(&block61, &tmp210, &tmp211, &tmp212);
    ca_.Goto(&block60, tmp210, tmp211, tmp212);
  }

  if (block62.is_used()) {
    compiler::TNode<Smi> tmp213;
    compiler::TNode<Smi> tmp214;
    compiler::TNode<Smi> tmp215;
    ca_.Bind(&block62, &tmp213, &tmp214, &tmp215);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 387);
    compiler::TNode<Smi> tmp216;
    USE(tmp216);
    tmp216 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp217;
    USE(tmp217);
    tmp217 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp215}, compiler::TNode<Smi>{tmp216}));
    ca_.Branch(tmp217, &block63, &block64, tmp213, tmp214, tmp215);
  }

  if (block63.is_used()) {
    compiler::TNode<Smi> tmp218;
    compiler::TNode<Smi> tmp219;
    compiler::TNode<Smi> tmp220;
    ca_.Bind(&block63, &tmp218, &tmp219, &tmp220);
    ca_.Goto(&block58, tmp218, tmp219, tmp220);
  }

  if (block64.is_used()) {
    compiler::TNode<Smi> tmp221;
    compiler::TNode<Smi> tmp222;
    compiler::TNode<Smi> tmp223;
    ca_.Bind(&block64, &tmp221, &tmp222, &tmp223);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 388);
    compiler::TNode<Smi> tmp224;
    USE(tmp224);
    tmp224 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp221}, compiler::TNode<Smi>{tmp223}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 385);
    ca_.Goto(&block60, tmp224, tmp222, tmp223);
  }

  if (block60.is_used()) {
    compiler::TNode<Smi> tmp225;
    compiler::TNode<Smi> tmp226;
    compiler::TNode<Smi> tmp227;
    ca_.Bind(&block60, &tmp225, &tmp226, &tmp227);
    compiler::TNode<Smi> tmp228;
    USE(tmp228);
    tmp228 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp229;
    USE(tmp229);
    tmp229 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp227}, compiler::TNode<Smi>{tmp228}));
    ca_.Goto(&block59, tmp225, tmp226, tmp229);
  }

  if (block58.is_used()) {
    compiler::TNode<Smi> tmp230;
    compiler::TNode<Smi> tmp231;
    compiler::TNode<Smi> tmp232;
    ca_.Bind(&block58, &tmp230, &tmp231, &tmp232);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 390);
    compiler::TNode<Smi> tmp233;
    USE(tmp233);
    tmp233 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(7));
    compiler::TNode<BoolT> tmp234;
    USE(tmp234);
    tmp234 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp230}, compiler::TNode<Smi>{tmp233}));
    ca_.Branch(tmp234, &block65, &block66, tmp230, tmp231);
  }

  if (block66.is_used()) {
    compiler::TNode<Smi> tmp235;
    compiler::TNode<Smi> tmp236;
    ca_.Bind(&block66, &tmp235, &tmp236);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 7\' failed", "../../test/torque/test-torque.tq", 390);
  }

  if (block65.is_used()) {
    compiler::TNode<Smi> tmp237;
    compiler::TNode<Smi> tmp238;
    ca_.Bind(&block65, &tmp237, &tmp238);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 392);
    compiler::TNode<Smi> tmp239;
    USE(tmp239);
    tmp239 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 393);
    compiler::TNode<Smi> tmp240;
    USE(tmp240);
    tmp240 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 394);
    ca_.Goto(&block69, tmp239, tmp240);
  }

  if (block69.is_used()) {
    compiler::TNode<Smi> tmp241;
    compiler::TNode<Smi> tmp242;
    ca_.Bind(&block69, &tmp241, &tmp242);
    ca_.Goto(&block67, tmp241, tmp242);
  }

  if (block67.is_used()) {
    compiler::TNode<Smi> tmp243;
    compiler::TNode<Smi> tmp244;
    ca_.Bind(&block67, &tmp243, &tmp244);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 395);
    compiler::TNode<Smi> tmp245;
    USE(tmp245);
    tmp245 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(3));
    compiler::TNode<BoolT> tmp246;
    USE(tmp246);
    tmp246 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp244}, compiler::TNode<Smi>{tmp245}));
    ca_.Branch(tmp246, &block70, &block71, tmp243, tmp244);
  }

  if (block70.is_used()) {
    compiler::TNode<Smi> tmp247;
    compiler::TNode<Smi> tmp248;
    ca_.Bind(&block70, &tmp247, &tmp248);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 396);
    compiler::TNode<Smi> tmp249;
    USE(tmp249);
    tmp249 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp250;
    USE(tmp250);
    tmp250 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp248}, compiler::TNode<Smi>{tmp249}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 397);
    ca_.Goto(&block69, tmp247, tmp250);
  }

  if (block71.is_used()) {
    compiler::TNode<Smi> tmp251;
    compiler::TNode<Smi> tmp252;
    ca_.Bind(&block71, &tmp251, &tmp252);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 400);
    compiler::TNode<Smi> tmp253;
    USE(tmp253);
    tmp253 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp254;
    USE(tmp254);
    tmp254 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp252}, compiler::TNode<Smi>{tmp253}));
    ca_.Branch(tmp254, &block72, &block73, tmp251, tmp252);
  }

  if (block72.is_used()) {
    compiler::TNode<Smi> tmp255;
    compiler::TNode<Smi> tmp256;
    ca_.Bind(&block72, &tmp255, &tmp256);
    ca_.Goto(&block68, tmp255, tmp256);
  }

  if (block73.is_used()) {
    compiler::TNode<Smi> tmp257;
    compiler::TNode<Smi> tmp258;
    ca_.Bind(&block73, &tmp257, &tmp258);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 401);
    compiler::TNode<Smi> tmp259;
    USE(tmp259);
    tmp259 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp257}, compiler::TNode<Smi>{tmp258}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 402);
    compiler::TNode<Smi> tmp260;
    USE(tmp260);
    tmp260 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp261;
    USE(tmp261);
    tmp261 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp258}, compiler::TNode<Smi>{tmp260}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 394);
    ca_.Goto(&block69, tmp259, tmp261);
  }

  if (block68.is_used()) {
    compiler::TNode<Smi> tmp262;
    compiler::TNode<Smi> tmp263;
    ca_.Bind(&block68, &tmp262, &tmp263);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 404);
    compiler::TNode<Smi> tmp264;
    USE(tmp264);
    tmp264 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(7));
    compiler::TNode<BoolT> tmp265;
    USE(tmp265);
    tmp265 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp262}, compiler::TNode<Smi>{tmp264}));
    ca_.Branch(tmp265, &block74, &block75, tmp262, tmp263);
  }

  if (block75.is_used()) {
    compiler::TNode<Smi> tmp266;
    compiler::TNode<Smi> tmp267;
    ca_.Bind(&block75, &tmp266, &tmp267);
    CodeStubAssembler(state_).FailAssert("Torque assert \'sum == 7\' failed", "../../test/torque/test-torque.tq", 404);
  }

  if (block74.is_used()) {
    compiler::TNode<Smi> tmp268;
    compiler::TNode<Smi> tmp269;
    ca_.Bind(&block74, &tmp268, &tmp269);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 406);
    compiler::TNode<Smi> tmp270;
    USE(tmp270);
    tmp270 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 408);
    ca_.Goto(&block80, tmp268, tmp270);
  }

  if (block80.is_used()) {
    compiler::TNode<Smi> tmp271;
    compiler::TNode<Smi> tmp272;
    ca_.Bind(&block80, &tmp271, &tmp272);
    ca_.Goto(&block78, tmp271, tmp272);
  }

  if (block78.is_used()) {
    compiler::TNode<Smi> tmp273;
    compiler::TNode<Smi> tmp274;
    ca_.Bind(&block78, &tmp273, &tmp274);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 409);
    compiler::TNode<Smi> tmp275;
    USE(tmp275);
    tmp275 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp276;
    USE(tmp276);
    tmp276 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp274}, compiler::TNode<Smi>{tmp275}));
    compiler::TNode<Smi> tmp277;
    USE(tmp277);
    tmp277 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp278;
    USE(tmp278);
    tmp278 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp276}, compiler::TNode<Smi>{tmp277}));
    ca_.Branch(tmp278, &block81, &block82, tmp273, tmp276);
  }

  if (block81.is_used()) {
    compiler::TNode<Smi> tmp279;
    compiler::TNode<Smi> tmp280;
    ca_.Bind(&block81, &tmp279, &tmp280);
    ca_.Goto(&block77, tmp279, tmp280);
  }

  if (block82.is_used()) {
    compiler::TNode<Smi> tmp281;
    compiler::TNode<Smi> tmp282;
    ca_.Bind(&block82, &tmp281, &tmp282);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 408);
    ca_.Goto(&block80, tmp281, tmp282);
  }

  if (block79.is_used()) {
    compiler::TNode<Smi> tmp283;
    compiler::TNode<Smi> tmp284;
    ca_.Bind(&block79, &tmp283, &tmp284);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 412);
    ca_.Goto(&block76, tmp283, tmp284);
  }

  if (block77.is_used()) {
    compiler::TNode<Smi> tmp285;
    compiler::TNode<Smi> tmp286;
    ca_.Bind(&block77, &tmp285, &tmp286);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 413);
    compiler::TNode<Smi> tmp287;
    USE(tmp287);
    tmp287 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp288;
    USE(tmp288);
    tmp288 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp286}, compiler::TNode<Smi>{tmp287}));
    ca_.Branch(tmp288, &block83, &block84, tmp285, tmp286);
  }

  if (block84.is_used()) {
    compiler::TNode<Smi> tmp289;
    compiler::TNode<Smi> tmp290;
    ca_.Bind(&block84, &tmp289, &tmp290);
    CodeStubAssembler(state_).FailAssert("Torque assert \'j == 10\' failed", "../../test/torque/test-torque.tq", 413);
  }

  if (block83.is_used()) {
    compiler::TNode<Smi> tmp291;
    compiler::TNode<Smi> tmp292;
    ca_.Bind(&block83, &tmp291, &tmp292);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 407);
    ca_.Goto(&block76, tmp291, tmp292);
  }

  if (block76.is_used()) {
    compiler::TNode<Smi> tmp293;
    compiler::TNode<Smi> tmp294;
    ca_.Bind(&block76, &tmp293, &tmp294);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 417);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 418);
    compiler::TNode<Smi> tmp295;
    USE(tmp295);
    tmp295 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block87, tmp293, tmp294, ca_.Uninitialized<Smi>(), tmp295);
  }

  if (block87.is_used()) {
    compiler::TNode<Smi> tmp296;
    compiler::TNode<Smi> tmp297;
    compiler::TNode<Smi> tmp298;
    compiler::TNode<Smi> tmp299;
    ca_.Bind(&block87, &tmp296, &tmp297, &tmp298, &tmp299);
    compiler::TNode<Smi> tmp300;
    USE(tmp300);
    tmp300 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp301;
    USE(tmp301);
    tmp301 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp299}, compiler::TNode<Smi>{tmp300}));
    ca_.Branch(tmp301, &block85, &block86, tmp296, tmp297, tmp298, tmp299);
  }

  if (block85.is_used()) {
    compiler::TNode<Smi> tmp302;
    compiler::TNode<Smi> tmp303;
    compiler::TNode<Smi> tmp304;
    compiler::TNode<Smi> tmp305;
    ca_.Bind(&block85, &tmp302, &tmp303, &tmp304, &tmp305);
    ca_.Goto(&block88, tmp302, tmp303, tmp304, tmp305);
  }

  if (block88.is_used()) {
    compiler::TNode<Smi> tmp306;
    compiler::TNode<Smi> tmp307;
    compiler::TNode<Smi> tmp308;
    compiler::TNode<Smi> tmp309;
    ca_.Bind(&block88, &tmp306, &tmp307, &tmp308, &tmp309);
    compiler::TNode<Smi> tmp310;
    USE(tmp310);
    tmp310 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp311;
    USE(tmp311);
    tmp311 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp309}, compiler::TNode<Smi>{tmp310}));
    ca_.Goto(&block87, tmp306, tmp307, tmp308, tmp311);
  }

  if (block86.is_used()) {
    compiler::TNode<Smi> tmp312;
    compiler::TNode<Smi> tmp313;
    compiler::TNode<Smi> tmp314;
    compiler::TNode<Smi> tmp315;
    ca_.Bind(&block86, &tmp312, &tmp313, &tmp314, &tmp315);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 323);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block89);
  }

    ca_.Bind(&block89);
}

void TestBuiltinsFromDSLAssembler::TestSubtyping(compiler::TNode<Smi> p_x) {
  compiler::CodeAssemblerParameterizedLabel<Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x);

  if (block0.is_used()) {
    compiler::TNode<Smi> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 423);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 422);
    ca_.Goto(&block1, tmp0);
  }

  if (block1.is_used()) {
    compiler::TNode<Smi> tmp1;
    ca_.Bind(&block1, &tmp1);
    ca_.Goto(&block2, tmp1);
  }

    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block2, &tmp2);
}

compiler::TNode<Int32T> TestBuiltinsFromDSLAssembler::TypeswitchExample(compiler::TNode<Context> p_context, compiler::TNode<Object> p_x) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object, Object, FixedArray> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object, Object, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object, HeapObject> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object, HeapObject, FixedArray> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Int32T> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_x);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 440);
    compiler::TNode<Int32T> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 441);
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(TestBuiltinsFromDSLAssembler(state_).IncrementIfSmi32UT5ATSmi10FixedArray10HeapNumber(compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 442);
    compiler::TNode<FixedArray> tmp4;
    USE(tmp4);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp3}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp3, tmp4);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp3);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    compiler::TNode<Int32T> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    ca_.Bind(&block5, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block3, tmp5, tmp6, tmp7, tmp8);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Int32T> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<FixedArray> tmp15;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 443);
    compiler::TNode<Int32T> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(1));
    compiler::TNode<Int32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Add(compiler::TNode<Int32T>{tmp12}, compiler::TNode<Int32T>{tmp16}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 442);
    ca_.Goto(&block2, tmp10, tmp11, tmp17, tmp13);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Int32T> tmp20;
    compiler::TNode<Object> tmp21;
    ca_.Bind(&block3, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 445);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 446);
    compiler::TNode<Int32T> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(2));
    compiler::TNode<Int32T> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Add(compiler::TNode<Int32T>{tmp20}, compiler::TNode<Int32T>{tmp22}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 442);
    ca_.Goto(&block2, tmp18, tmp19, tmp23, tmp21);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Int32T> tmp26;
    compiler::TNode<Object> tmp27;
    ca_.Bind(&block2, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 441);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 450);
    compiler::TNode<Int32T> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(10));
    compiler::TNode<Int32T> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Mul(compiler::TNode<Int32T>{tmp26}, compiler::TNode<Int32T>{tmp28}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 452);
    compiler::TNode<Object> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Object>(TestBuiltinsFromDSLAssembler(state_).IncrementIfSmi32UT5ATSmi10FixedArray10HeapNumber(compiler::TNode<Object>{tmp25}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 453);
    compiler::TNode<Smi> tmp31;
    USE(tmp31);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp31 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp30}, &label0);
    ca_.Goto(&block8, tmp24, tmp25, tmp29, tmp30, tmp30, tmp31);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp24, tmp25, tmp29, tmp30, tmp30);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Int32T> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Object> tmp36;
    ca_.Bind(&block9, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.Goto(&block7, tmp32, tmp33, tmp34, tmp35);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Int32T> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<Smi> tmp42;
    ca_.Bind(&block8, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 454);
    compiler::TNode<Int32T> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).Convert7ATint325ATSmi(compiler::TNode<Smi>{tmp42}));
    compiler::TNode<Int32T> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Add(compiler::TNode<Int32T>{tmp39}, compiler::TNode<Int32T>{tmp43}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 453);
    ca_.Goto(&block6, tmp37, tmp38, tmp44, tmp40);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Int32T> tmp47;
    compiler::TNode<Object> tmp48;
    ca_.Bind(&block7, &tmp45, &tmp46, &tmp47, &tmp48);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 456);
    compiler::TNode<FixedArray> tmp49;
    USE(tmp49);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp49 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<HeapObject>{ca_.UncheckedCast<HeapObject>(tmp48)}, &label0);
    ca_.Goto(&block12, tmp45, tmp46, tmp47, tmp48, ca_.UncheckedCast<HeapObject>(tmp48), tmp49);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block13, tmp45, tmp46, tmp47, tmp48, ca_.UncheckedCast<HeapObject>(tmp48));
    }
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Int32T> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<HeapObject> tmp54;
    ca_.Bind(&block13, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54);
    ca_.Goto(&block11, tmp50, tmp51, tmp52, tmp53);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Int32T> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<HeapObject> tmp59;
    compiler::TNode<FixedArray> tmp60;
    ca_.Bind(&block12, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 457);
    compiler::TNode<Smi> tmp61;
    USE(tmp61);
    tmp61 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp60}));
    compiler::TNode<Int32T> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).Convert7ATint325ATSmi(compiler::TNode<Smi>{tmp61}));
    compiler::TNode<Int32T> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Add(compiler::TNode<Int32T>{tmp57}, compiler::TNode<Int32T>{tmp62}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 456);
    ca_.Goto(&block10, tmp55, tmp56, tmp63, tmp58);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<Int32T> tmp66;
    compiler::TNode<Object> tmp67;
    ca_.Bind(&block11, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 459);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 460);
    compiler::TNode<Int32T> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(7));
    compiler::TNode<Int32T> tmp69;
    USE(tmp69);
    tmp69 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Add(compiler::TNode<Int32T>{tmp66}, compiler::TNode<Int32T>{tmp68}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 456);
    ca_.Goto(&block10, tmp64, tmp65, tmp69, tmp67);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Int32T> tmp72;
    compiler::TNode<Object> tmp73;
    ca_.Bind(&block10, &tmp70, &tmp71, &tmp72, &tmp73);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 453);
    ca_.Goto(&block6, tmp70, tmp71, tmp72, tmp73);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<Int32T> tmp76;
    compiler::TNode<Object> tmp77;
    ca_.Bind(&block6, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 452);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 464);
    ca_.Goto(&block1, tmp74, tmp75, tmp76);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Int32T> tmp80;
    ca_.Bind(&block1, &tmp78, &tmp79, &tmp80);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 438);
    ca_.Goto(&block14, tmp78, tmp79, tmp80);
  }

    compiler::TNode<Context> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Int32T> tmp83;
    ca_.Bind(&block14, &tmp81, &tmp82, &tmp83);
  return compiler::TNode<Int32T>{tmp83};
}

void TestBuiltinsFromDSLAssembler::TestTypeswitch(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 468);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<Int32T> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Int32T>(TestBuiltinsFromDSLAssembler(state_).TypeswitchExample(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    compiler::TNode<Int32T> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(26));
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp2}, compiler::TNode<Int32T>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    ca_.Bind(&block3, &tmp5);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TypeswitchExample(FromConstexpr<Smi>(5)) == 26\' failed", "../../test/torque/test-torque.tq", 468);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp6;
    ca_.Bind(&block2, &tmp6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 469);
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(3));
    compiler::TNode<FixedArray> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateZeroedFixedArray(compiler::TNode<IntPtrT>{tmp7}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 470);
    compiler::TNode<Int32T> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Int32T>(TestBuiltinsFromDSLAssembler(state_).TypeswitchExample(compiler::TNode<Context>{tmp6}, compiler::TNode<Object>{tmp8}));
    compiler::TNode<Int32T> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(13));
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp9}, compiler::TNode<Int32T>{tmp10}));
    ca_.Branch(tmp11, &block4, &block5, tmp6, tmp8);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<FixedArray> tmp13;
    ca_.Bind(&block5, &tmp12, &tmp13);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TypeswitchExample(a) == 13\' failed", "../../test/torque/test-torque.tq", 470);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<FixedArray> tmp15;
    ca_.Bind(&block4, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 471);
    compiler::TNode<Number> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber19ATconstexpr_float64(0.5));
    compiler::TNode<Int32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Int32T>(TestBuiltinsFromDSLAssembler(state_).TypeswitchExample(compiler::TNode<Context>{tmp14}, compiler::TNode<Object>{tmp16}));
    compiler::TNode<Int32T> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(27));
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp17}, compiler::TNode<Int32T>{tmp18}));
    ca_.Branch(tmp19, &block6, &block7, tmp14, tmp15);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<FixedArray> tmp21;
    ca_.Bind(&block7, &tmp20, &tmp21);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TypeswitchExample(FromConstexpr<Number>(0.5)) == 27\' failed", "../../test/torque/test-torque.tq", 471);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<FixedArray> tmp23;
    ca_.Bind(&block6, &tmp22, &tmp23);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 467);
    ca_.Goto(&block1, tmp22);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp24;
    ca_.Bind(&block1, &tmp24);
    ca_.Goto(&block8, tmp24);
  }

    compiler::TNode<Context> tmp25;
    ca_.Bind(&block8, &tmp25);
}

void TestBuiltinsFromDSLAssembler::TestTypeswitchAsanLsanFailure(compiler::TNode<Context> p_context, compiler::TNode<Object> p_obj) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, HeapObject> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, HeapObject, JSTypedArray> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, HeapObject> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, HeapObject, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_obj);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 475);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 476);
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp1, tmp1, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp1, tmp1);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block5, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.Goto(&block3, tmp3, tmp4, tmp5);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block4, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.Goto(&block2, tmp7, tmp8, tmp9);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    ca_.Bind(&block3, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 478);
    compiler::TNode<JSTypedArray> tmp15;
    USE(tmp15);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp15 = BaseBuiltinsFromDSLAssembler(state_).Cast12JSTypedArray(compiler::TNode<HeapObject>{ca_.UncheckedCast<HeapObject>(tmp14)}, &label0);
    ca_.Goto(&block8, tmp12, tmp13, tmp14, ca_.UncheckedCast<HeapObject>(tmp14), tmp15);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp12, tmp13, tmp14, ca_.UncheckedCast<HeapObject>(tmp14));
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<HeapObject> tmp19;
    ca_.Bind(&block9, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block7, tmp16, tmp17, tmp18);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<HeapObject> tmp23;
    compiler::TNode<JSTypedArray> tmp24;
    ca_.Bind(&block8, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block6, tmp20, tmp21, tmp22);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    ca_.Bind(&block7, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 480);
    compiler::TNode<JSReceiver> tmp28;
    USE(tmp28);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp28 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp25}, compiler::TNode<HeapObject>{ca_.UncheckedCast<HeapObject>(tmp27)}, &label0);
    ca_.Goto(&block12, tmp25, tmp26, tmp27, ca_.UncheckedCast<HeapObject>(tmp27), tmp28);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block13, tmp25, tmp26, tmp27, ca_.UncheckedCast<HeapObject>(tmp27));
    }
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<HeapObject> tmp32;
    ca_.Bind(&block13, &tmp29, &tmp30, &tmp31, &tmp32);
    ca_.Goto(&block11, tmp29, tmp30, tmp31);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<HeapObject> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    ca_.Bind(&block12, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    ca_.Goto(&block10, tmp33, tmp34, tmp35);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    ca_.Bind(&block11, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 482);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 480);
    ca_.Goto(&block10, tmp38, tmp39, tmp40);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<Object> tmp43;
    ca_.Bind(&block10, &tmp41, &tmp42, &tmp43);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 478);
    ca_.Goto(&block6, tmp41, tmp42, tmp43);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Object> tmp46;
    ca_.Bind(&block6, &tmp44, &tmp45, &tmp46);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 476);
    ca_.Goto(&block2, tmp44, tmp45, tmp46);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<Object> tmp49;
    ca_.Bind(&block2, &tmp47, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 475);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 474);
    ca_.Goto(&block1, tmp47, tmp48);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<Object> tmp51;
    ca_.Bind(&block1, &tmp50, &tmp51);
    ca_.Goto(&block14, tmp50, tmp51);
  }

    compiler::TNode<Context> tmp52;
    compiler::TNode<Object> tmp53;
    ca_.Bind(&block14, &tmp52, &tmp53);
}

void TestBuiltinsFromDSLAssembler::TestGenericOverload(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 495);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 496);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 497);
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Smi>(TestBuiltinsFromDSLAssembler(state_).ExampleGenericOverload5ATSmi(compiler::TNode<Smi>{tmp1}));
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(6));
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp2}, compiler::TNode<Smi>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, tmp1, tmp1);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Object> tmp7;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7);
    CodeStubAssembler(state_).FailAssert("Torque assert \'ExampleGenericOverload<Smi>(xSmi) == 6\' failed", "../../test/torque/test-torque.tq", 497);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Object> tmp10;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 498);
    compiler::TNode<Object> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Object>(TestBuiltinsFromDSLAssembler(state_).ExampleGenericOverload20UT5ATSmi10HeapObject(compiler::TNode<Object>{tmp10}));
    compiler::TNode<Smi> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp8}, compiler::TNode<Object>{tmp11}));
    compiler::TNode<Smi> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp12}, compiler::TNode<Smi>{tmp13}));
    ca_.Branch(tmp14, &block4, &block5, tmp8, tmp9, tmp10);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Object> tmp17;
    ca_.Bind(&block5, &tmp15, &tmp16, &tmp17);
    CodeStubAssembler(state_).FailAssert("Torque assert \'UnsafeCast<Smi>(ExampleGenericOverload<Object>(xObject)) == 5\' failed", "../../test/torque/test-torque.tq", 498);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Object> tmp20;
    ca_.Bind(&block4, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 494);
    ca_.Goto(&block1, tmp18);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp21;
    ca_.Bind(&block1, &tmp21);
    ca_.Goto(&block6, tmp21);
  }

    compiler::TNode<Context> tmp22;
    ca_.Bind(&block6, &tmp22);
}

void TestBuiltinsFromDSLAssembler::TestEquality(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BoolT, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BoolT, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 503);
    compiler::TNode<Float64T> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6419ATconstexpr_float64(0.5));
    compiler::TNode<HeapNumber> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<HeapNumber>(CodeStubAssembler(state_).AllocateHeapNumberWithValue(compiler::TNode<Float64T>{tmp1}));
    compiler::TNode<Float64T> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6419ATconstexpr_float64(0.5));
    compiler::TNode<HeapNumber> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<HeapNumber>(CodeStubAssembler(state_).AllocateHeapNumberWithValue(compiler::TNode<Float64T>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).IsNumberNotEqual(compiler::TNode<Number>{tmp2}, compiler::TNode<Number>{tmp4}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 502);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 504);
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block2, &block3, tmp0, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<BoolT> tmp8;
    ca_.Bind(&block3, &tmp7, &tmp8);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!notEqual\' failed", "../../test/torque/test-torque.tq", 504);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<BoolT> tmp10;
    ca_.Bind(&block2, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 506);
    compiler::TNode<Float64T> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6419ATconstexpr_float64(0.5));
    compiler::TNode<HeapNumber> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<HeapNumber>(CodeStubAssembler(state_).AllocateHeapNumberWithValue(compiler::TNode<Float64T>{tmp11}));
    compiler::TNode<Float64T> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6419ATconstexpr_float64(0.5));
    compiler::TNode<HeapNumber> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<HeapNumber>(CodeStubAssembler(state_).AllocateHeapNumberWithValue(compiler::TNode<Float64T>{tmp13}));
    compiler::TNode<BoolT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).IsNumberEqual(compiler::TNode<Number>{tmp12}, compiler::TNode<Number>{tmp14}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 505);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 507);
    ca_.Branch(tmp15, &block4, &block5, tmp9, tmp10, tmp15);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<BoolT> tmp17;
    compiler::TNode<BoolT> tmp18;
    ca_.Bind(&block5, &tmp16, &tmp17, &tmp18);
    CodeStubAssembler(state_).FailAssert("Torque assert \'equal\' failed", "../../test/torque/test-torque.tq", 507);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<BoolT> tmp20;
    compiler::TNode<BoolT> tmp21;
    ca_.Bind(&block4, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 501);
    ca_.Goto(&block1, tmp19);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp22;
    ca_.Bind(&block1, &tmp22);
    ca_.Goto(&block6, tmp22);
  }

    compiler::TNode<Context> tmp23;
    ca_.Bind(&block6, &tmp23);
}

void TestBuiltinsFromDSLAssembler::BoolToBranch(compiler::TNode<BoolT> p_x, compiler::CodeAssemblerLabel* label_Taken, compiler::CodeAssemblerLabel* label_NotTaken) {
  compiler::CodeAssemblerParameterizedLabel<BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x);

  if (block0.is_used()) {
    compiler::TNode<BoolT> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 512);
    ca_.Branch(tmp0, &block3, &block4, tmp0);
  }

  if (block3.is_used()) {
    compiler::TNode<BoolT> tmp1;
    ca_.Bind(&block3, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 513);
    ca_.Goto(&block1);
  }

  if (block4.is_used()) {
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block4, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 515);
    ca_.Goto(&block2);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 510);
    ca_.Goto(label_Taken);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.Goto(label_NotTaken);
  }
}

compiler::TNode<BoolT> TestBuiltinsFromDSLAssembler::TestOrAnd1(compiler::TNode<BoolT> p_x, compiler::TNode<BoolT> p_y, compiler::TNode<BoolT> p_z) {
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x, p_y, p_z);

  if (block0.is_used()) {
    compiler::TNode<BoolT> tmp0;
    compiler::TNode<BoolT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 520);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    TestBuiltinsFromDSLAssembler(state_).BoolToBranch(compiler::TNode<BoolT>{tmp0}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp0, tmp1, tmp2, tmp0);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block8, tmp0, tmp1, tmp2, tmp0);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<BoolT> tmp3;
    compiler::TNode<BoolT> tmp4;
    compiler::TNode<BoolT> tmp5;
    compiler::TNode<BoolT> tmp6;
    ca_.Bind(&block7, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.Goto(&block2, tmp3, tmp4, tmp5);
  }

  if (block8.is_used()) {
    compiler::TNode<BoolT> tmp7;
    compiler::TNode<BoolT> tmp8;
    compiler::TNode<BoolT> tmp9;
    compiler::TNode<BoolT> tmp10;
    ca_.Bind(&block8, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.Goto(&block6, tmp7, tmp8, tmp9);
  }

  if (block6.is_used()) {
    compiler::TNode<BoolT> tmp11;
    compiler::TNode<BoolT> tmp12;
    compiler::TNode<BoolT> tmp13;
    ca_.Bind(&block6, &tmp11, &tmp12, &tmp13);
    ca_.Branch(tmp12, &block9, &block3, tmp11, tmp12, tmp13);
  }

  if (block9.is_used()) {
    compiler::TNode<BoolT> tmp14;
    compiler::TNode<BoolT> tmp15;
    compiler::TNode<BoolT> tmp16;
    ca_.Bind(&block9, &tmp14, &tmp15, &tmp16);
    ca_.Branch(tmp16, &block2, &block3, tmp14, tmp15, tmp16);
  }

  if (block2.is_used()) {
    compiler::TNode<BoolT> tmp17;
    compiler::TNode<BoolT> tmp18;
    compiler::TNode<BoolT> tmp19;
    ca_.Bind(&block2, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block5, tmp17, tmp18, tmp19);
  }

  if (block3.is_used()) {
    compiler::TNode<BoolT> tmp20;
    compiler::TNode<BoolT> tmp21;
    compiler::TNode<BoolT> tmp22;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23);
  }

  if (block5.is_used()) {
    compiler::TNode<BoolT> tmp24;
    compiler::TNode<BoolT> tmp25;
    compiler::TNode<BoolT> tmp26;
    ca_.Bind(&block5, &tmp24, &tmp25, &tmp26);
    compiler::TNode<BoolT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block4, tmp24, tmp25, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<BoolT> tmp28;
    compiler::TNode<BoolT> tmp29;
    compiler::TNode<BoolT> tmp30;
    compiler::TNode<BoolT> tmp31;
    ca_.Bind(&block4, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block1, tmp28, tmp29, tmp30, tmp31);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<BoolT> tmp33;
    compiler::TNode<BoolT> tmp34;
    compiler::TNode<BoolT> tmp35;
    ca_.Bind(&block1, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 519);
    ca_.Goto(&block10, tmp32, tmp33, tmp34, tmp35);
  }

    compiler::TNode<BoolT> tmp36;
    compiler::TNode<BoolT> tmp37;
    compiler::TNode<BoolT> tmp38;
    compiler::TNode<BoolT> tmp39;
    ca_.Bind(&block10, &tmp36, &tmp37, &tmp38, &tmp39);
  return compiler::TNode<BoolT>{tmp39};
}

compiler::TNode<BoolT> TestBuiltinsFromDSLAssembler::TestOrAnd2(compiler::TNode<BoolT> p_x, compiler::TNode<BoolT> p_y, compiler::TNode<BoolT> p_z) {
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x, p_y, p_z);

  if (block0.is_used()) {
    compiler::TNode<BoolT> tmp0;
    compiler::TNode<BoolT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 524);
    ca_.Branch(tmp0, &block2, &block6, tmp0, tmp1, tmp2);
  }

  if (block6.is_used()) {
    compiler::TNode<BoolT> tmp3;
    compiler::TNode<BoolT> tmp4;
    compiler::TNode<BoolT> tmp5;
    ca_.Bind(&block6, &tmp3, &tmp4, &tmp5);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    TestBuiltinsFromDSLAssembler(state_).BoolToBranch(compiler::TNode<BoolT>{tmp4}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp3, tmp4, tmp5, tmp4);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block9, tmp3, tmp4, tmp5, tmp4);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<BoolT> tmp6;
    compiler::TNode<BoolT> tmp7;
    compiler::TNode<BoolT> tmp8;
    compiler::TNode<BoolT> tmp9;
    ca_.Bind(&block8, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block7, tmp6, tmp7, tmp8);
  }

  if (block9.is_used()) {
    compiler::TNode<BoolT> tmp10;
    compiler::TNode<BoolT> tmp11;
    compiler::TNode<BoolT> tmp12;
    compiler::TNode<BoolT> tmp13;
    ca_.Bind(&block9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block3, tmp10, tmp11, tmp12);
  }

  if (block7.is_used()) {
    compiler::TNode<BoolT> tmp14;
    compiler::TNode<BoolT> tmp15;
    compiler::TNode<BoolT> tmp16;
    ca_.Bind(&block7, &tmp14, &tmp15, &tmp16);
    ca_.Branch(tmp16, &block2, &block3, tmp14, tmp15, tmp16);
  }

  if (block2.is_used()) {
    compiler::TNode<BoolT> tmp17;
    compiler::TNode<BoolT> tmp18;
    compiler::TNode<BoolT> tmp19;
    ca_.Bind(&block2, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block5, tmp17, tmp18, tmp19);
  }

  if (block3.is_used()) {
    compiler::TNode<BoolT> tmp20;
    compiler::TNode<BoolT> tmp21;
    compiler::TNode<BoolT> tmp22;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23);
  }

  if (block5.is_used()) {
    compiler::TNode<BoolT> tmp24;
    compiler::TNode<BoolT> tmp25;
    compiler::TNode<BoolT> tmp26;
    ca_.Bind(&block5, &tmp24, &tmp25, &tmp26);
    compiler::TNode<BoolT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block4, tmp24, tmp25, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<BoolT> tmp28;
    compiler::TNode<BoolT> tmp29;
    compiler::TNode<BoolT> tmp30;
    compiler::TNode<BoolT> tmp31;
    ca_.Bind(&block4, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block1, tmp28, tmp29, tmp30, tmp31);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<BoolT> tmp33;
    compiler::TNode<BoolT> tmp34;
    compiler::TNode<BoolT> tmp35;
    ca_.Bind(&block1, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 523);
    ca_.Goto(&block10, tmp32, tmp33, tmp34, tmp35);
  }

    compiler::TNode<BoolT> tmp36;
    compiler::TNode<BoolT> tmp37;
    compiler::TNode<BoolT> tmp38;
    compiler::TNode<BoolT> tmp39;
    ca_.Bind(&block10, &tmp36, &tmp37, &tmp38, &tmp39);
  return compiler::TNode<BoolT>{tmp39};
}

compiler::TNode<BoolT> TestBuiltinsFromDSLAssembler::TestOrAnd3(compiler::TNode<BoolT> p_x, compiler::TNode<BoolT> p_y, compiler::TNode<BoolT> p_z) {
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x, p_y, p_z);

  if (block0.is_used()) {
    compiler::TNode<BoolT> tmp0;
    compiler::TNode<BoolT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 528);
    ca_.Branch(tmp0, &block2, &block6, tmp0, tmp1, tmp2);
  }

  if (block6.is_used()) {
    compiler::TNode<BoolT> tmp3;
    compiler::TNode<BoolT> tmp4;
    compiler::TNode<BoolT> tmp5;
    ca_.Bind(&block6, &tmp3, &tmp4, &tmp5);
    ca_.Branch(tmp4, &block7, &block3, tmp3, tmp4, tmp5);
  }

  if (block7.is_used()) {
    compiler::TNode<BoolT> tmp6;
    compiler::TNode<BoolT> tmp7;
    compiler::TNode<BoolT> tmp8;
    ca_.Bind(&block7, &tmp6, &tmp7, &tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    TestBuiltinsFromDSLAssembler(state_).BoolToBranch(compiler::TNode<BoolT>{tmp8}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp6, tmp7, tmp8, tmp8);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block9, tmp6, tmp7, tmp8, tmp8);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<BoolT> tmp9;
    compiler::TNode<BoolT> tmp10;
    compiler::TNode<BoolT> tmp11;
    compiler::TNode<BoolT> tmp12;
    ca_.Bind(&block8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.Goto(&block2, tmp9, tmp10, tmp11);
  }

  if (block9.is_used()) {
    compiler::TNode<BoolT> tmp13;
    compiler::TNode<BoolT> tmp14;
    compiler::TNode<BoolT> tmp15;
    compiler::TNode<BoolT> tmp16;
    ca_.Bind(&block9, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.Goto(&block3, tmp13, tmp14, tmp15);
  }

  if (block2.is_used()) {
    compiler::TNode<BoolT> tmp17;
    compiler::TNode<BoolT> tmp18;
    compiler::TNode<BoolT> tmp19;
    ca_.Bind(&block2, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block5, tmp17, tmp18, tmp19);
  }

  if (block3.is_used()) {
    compiler::TNode<BoolT> tmp20;
    compiler::TNode<BoolT> tmp21;
    compiler::TNode<BoolT> tmp22;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23);
  }

  if (block5.is_used()) {
    compiler::TNode<BoolT> tmp24;
    compiler::TNode<BoolT> tmp25;
    compiler::TNode<BoolT> tmp26;
    ca_.Bind(&block5, &tmp24, &tmp25, &tmp26);
    compiler::TNode<BoolT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block4, tmp24, tmp25, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<BoolT> tmp28;
    compiler::TNode<BoolT> tmp29;
    compiler::TNode<BoolT> tmp30;
    compiler::TNode<BoolT> tmp31;
    ca_.Bind(&block4, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block1, tmp28, tmp29, tmp30, tmp31);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<BoolT> tmp33;
    compiler::TNode<BoolT> tmp34;
    compiler::TNode<BoolT> tmp35;
    ca_.Bind(&block1, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 527);
    ca_.Goto(&block10, tmp32, tmp33, tmp34, tmp35);
  }

    compiler::TNode<BoolT> tmp36;
    compiler::TNode<BoolT> tmp37;
    compiler::TNode<BoolT> tmp38;
    compiler::TNode<BoolT> tmp39;
    ca_.Bind(&block10, &tmp36, &tmp37, &tmp38, &tmp39);
  return compiler::TNode<BoolT>{tmp39};
}

compiler::TNode<BoolT> TestBuiltinsFromDSLAssembler::TestAndOr1(compiler::TNode<BoolT> p_x, compiler::TNode<BoolT> p_y, compiler::TNode<BoolT> p_z) {
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x, p_y, p_z);

  if (block0.is_used()) {
    compiler::TNode<BoolT> tmp0;
    compiler::TNode<BoolT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 532);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    TestBuiltinsFromDSLAssembler(state_).BoolToBranch(compiler::TNode<BoolT>{tmp0}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp0, tmp1, tmp2, tmp0);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block9, tmp0, tmp1, tmp2, tmp0);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<BoolT> tmp3;
    compiler::TNode<BoolT> tmp4;
    compiler::TNode<BoolT> tmp5;
    compiler::TNode<BoolT> tmp6;
    ca_.Bind(&block8, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.Goto(&block7, tmp3, tmp4, tmp5);
  }

  if (block9.is_used()) {
    compiler::TNode<BoolT> tmp7;
    compiler::TNode<BoolT> tmp8;
    compiler::TNode<BoolT> tmp9;
    compiler::TNode<BoolT> tmp10;
    ca_.Bind(&block9, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.Goto(&block6, tmp7, tmp8, tmp9);
  }

  if (block7.is_used()) {
    compiler::TNode<BoolT> tmp11;
    compiler::TNode<BoolT> tmp12;
    compiler::TNode<BoolT> tmp13;
    ca_.Bind(&block7, &tmp11, &tmp12, &tmp13);
    ca_.Branch(tmp12, &block2, &block6, tmp11, tmp12, tmp13);
  }

  if (block6.is_used()) {
    compiler::TNode<BoolT> tmp14;
    compiler::TNode<BoolT> tmp15;
    compiler::TNode<BoolT> tmp16;
    ca_.Bind(&block6, &tmp14, &tmp15, &tmp16);
    ca_.Branch(tmp16, &block2, &block3, tmp14, tmp15, tmp16);
  }

  if (block2.is_used()) {
    compiler::TNode<BoolT> tmp17;
    compiler::TNode<BoolT> tmp18;
    compiler::TNode<BoolT> tmp19;
    ca_.Bind(&block2, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block5, tmp17, tmp18, tmp19);
  }

  if (block3.is_used()) {
    compiler::TNode<BoolT> tmp20;
    compiler::TNode<BoolT> tmp21;
    compiler::TNode<BoolT> tmp22;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23);
  }

  if (block5.is_used()) {
    compiler::TNode<BoolT> tmp24;
    compiler::TNode<BoolT> tmp25;
    compiler::TNode<BoolT> tmp26;
    ca_.Bind(&block5, &tmp24, &tmp25, &tmp26);
    compiler::TNode<BoolT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block4, tmp24, tmp25, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<BoolT> tmp28;
    compiler::TNode<BoolT> tmp29;
    compiler::TNode<BoolT> tmp30;
    compiler::TNode<BoolT> tmp31;
    ca_.Bind(&block4, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block1, tmp28, tmp29, tmp30, tmp31);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<BoolT> tmp33;
    compiler::TNode<BoolT> tmp34;
    compiler::TNode<BoolT> tmp35;
    ca_.Bind(&block1, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 531);
    ca_.Goto(&block10, tmp32, tmp33, tmp34, tmp35);
  }

    compiler::TNode<BoolT> tmp36;
    compiler::TNode<BoolT> tmp37;
    compiler::TNode<BoolT> tmp38;
    compiler::TNode<BoolT> tmp39;
    ca_.Bind(&block10, &tmp36, &tmp37, &tmp38, &tmp39);
  return compiler::TNode<BoolT>{tmp39};
}

compiler::TNode<BoolT> TestBuiltinsFromDSLAssembler::TestAndOr2(compiler::TNode<BoolT> p_x, compiler::TNode<BoolT> p_y, compiler::TNode<BoolT> p_z) {
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x, p_y, p_z);

  if (block0.is_used()) {
    compiler::TNode<BoolT> tmp0;
    compiler::TNode<BoolT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 536);
    ca_.Branch(tmp0, &block7, &block6, tmp0, tmp1, tmp2);
  }

  if (block7.is_used()) {
    compiler::TNode<BoolT> tmp3;
    compiler::TNode<BoolT> tmp4;
    compiler::TNode<BoolT> tmp5;
    ca_.Bind(&block7, &tmp3, &tmp4, &tmp5);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    TestBuiltinsFromDSLAssembler(state_).BoolToBranch(compiler::TNode<BoolT>{tmp4}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp3, tmp4, tmp5, tmp4);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block9, tmp3, tmp4, tmp5, tmp4);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<BoolT> tmp6;
    compiler::TNode<BoolT> tmp7;
    compiler::TNode<BoolT> tmp8;
    compiler::TNode<BoolT> tmp9;
    ca_.Bind(&block8, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block2, tmp6, tmp7, tmp8);
  }

  if (block9.is_used()) {
    compiler::TNode<BoolT> tmp10;
    compiler::TNode<BoolT> tmp11;
    compiler::TNode<BoolT> tmp12;
    compiler::TNode<BoolT> tmp13;
    ca_.Bind(&block9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block6, tmp10, tmp11, tmp12);
  }

  if (block6.is_used()) {
    compiler::TNode<BoolT> tmp14;
    compiler::TNode<BoolT> tmp15;
    compiler::TNode<BoolT> tmp16;
    ca_.Bind(&block6, &tmp14, &tmp15, &tmp16);
    ca_.Branch(tmp16, &block2, &block3, tmp14, tmp15, tmp16);
  }

  if (block2.is_used()) {
    compiler::TNode<BoolT> tmp17;
    compiler::TNode<BoolT> tmp18;
    compiler::TNode<BoolT> tmp19;
    ca_.Bind(&block2, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block5, tmp17, tmp18, tmp19);
  }

  if (block3.is_used()) {
    compiler::TNode<BoolT> tmp20;
    compiler::TNode<BoolT> tmp21;
    compiler::TNode<BoolT> tmp22;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23);
  }

  if (block5.is_used()) {
    compiler::TNode<BoolT> tmp24;
    compiler::TNode<BoolT> tmp25;
    compiler::TNode<BoolT> tmp26;
    ca_.Bind(&block5, &tmp24, &tmp25, &tmp26);
    compiler::TNode<BoolT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block4, tmp24, tmp25, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<BoolT> tmp28;
    compiler::TNode<BoolT> tmp29;
    compiler::TNode<BoolT> tmp30;
    compiler::TNode<BoolT> tmp31;
    ca_.Bind(&block4, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block1, tmp28, tmp29, tmp30, tmp31);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<BoolT> tmp33;
    compiler::TNode<BoolT> tmp34;
    compiler::TNode<BoolT> tmp35;
    ca_.Bind(&block1, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 535);
    ca_.Goto(&block10, tmp32, tmp33, tmp34, tmp35);
  }

    compiler::TNode<BoolT> tmp36;
    compiler::TNode<BoolT> tmp37;
    compiler::TNode<BoolT> tmp38;
    compiler::TNode<BoolT> tmp39;
    ca_.Bind(&block10, &tmp36, &tmp37, &tmp38, &tmp39);
  return compiler::TNode<BoolT>{tmp39};
}

compiler::TNode<BoolT> TestBuiltinsFromDSLAssembler::TestAndOr3(compiler::TNode<BoolT> p_x, compiler::TNode<BoolT> p_y, compiler::TNode<BoolT> p_z) {
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BoolT, BoolT, BoolT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x, p_y, p_z);

  if (block0.is_used()) {
    compiler::TNode<BoolT> tmp0;
    compiler::TNode<BoolT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 540);
    ca_.Branch(tmp0, &block7, &block6, tmp0, tmp1, tmp2);
  }

  if (block7.is_used()) {
    compiler::TNode<BoolT> tmp3;
    compiler::TNode<BoolT> tmp4;
    compiler::TNode<BoolT> tmp5;
    ca_.Bind(&block7, &tmp3, &tmp4, &tmp5);
    ca_.Branch(tmp4, &block2, &block6, tmp3, tmp4, tmp5);
  }

  if (block6.is_used()) {
    compiler::TNode<BoolT> tmp6;
    compiler::TNode<BoolT> tmp7;
    compiler::TNode<BoolT> tmp8;
    ca_.Bind(&block6, &tmp6, &tmp7, &tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    TestBuiltinsFromDSLAssembler(state_).BoolToBranch(compiler::TNode<BoolT>{tmp8}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp6, tmp7, tmp8, tmp8);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block9, tmp6, tmp7, tmp8, tmp8);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<BoolT> tmp9;
    compiler::TNode<BoolT> tmp10;
    compiler::TNode<BoolT> tmp11;
    compiler::TNode<BoolT> tmp12;
    ca_.Bind(&block8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.Goto(&block2, tmp9, tmp10, tmp11);
  }

  if (block9.is_used()) {
    compiler::TNode<BoolT> tmp13;
    compiler::TNode<BoolT> tmp14;
    compiler::TNode<BoolT> tmp15;
    compiler::TNode<BoolT> tmp16;
    ca_.Bind(&block9, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.Goto(&block3, tmp13, tmp14, tmp15);
  }

  if (block2.is_used()) {
    compiler::TNode<BoolT> tmp17;
    compiler::TNode<BoolT> tmp18;
    compiler::TNode<BoolT> tmp19;
    ca_.Bind(&block2, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block5, tmp17, tmp18, tmp19);
  }

  if (block3.is_used()) {
    compiler::TNode<BoolT> tmp20;
    compiler::TNode<BoolT> tmp21;
    compiler::TNode<BoolT> tmp22;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23);
  }

  if (block5.is_used()) {
    compiler::TNode<BoolT> tmp24;
    compiler::TNode<BoolT> tmp25;
    compiler::TNode<BoolT> tmp26;
    ca_.Bind(&block5, &tmp24, &tmp25, &tmp26);
    compiler::TNode<BoolT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block4, tmp24, tmp25, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<BoolT> tmp28;
    compiler::TNode<BoolT> tmp29;
    compiler::TNode<BoolT> tmp30;
    compiler::TNode<BoolT> tmp31;
    ca_.Bind(&block4, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block1, tmp28, tmp29, tmp30, tmp31);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<BoolT> tmp33;
    compiler::TNode<BoolT> tmp34;
    compiler::TNode<BoolT> tmp35;
    ca_.Bind(&block1, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 539);
    ca_.Goto(&block10, tmp32, tmp33, tmp34, tmp35);
  }

    compiler::TNode<BoolT> tmp36;
    compiler::TNode<BoolT> tmp37;
    compiler::TNode<BoolT> tmp38;
    compiler::TNode<BoolT> tmp39;
    ca_.Bind(&block10, &tmp36, &tmp37, &tmp38, &tmp39);
  return compiler::TNode<BoolT>{tmp39};
}

void TestBuiltinsFromDSLAssembler::TestLogicalOperators() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block11(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block13(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block15(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block17(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block19(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block21(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block23(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block25(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block27(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block29(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block31(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block33(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block35(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block37(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block39(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block41(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block43(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block45(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block44(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block47(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block46(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block49(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block48(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block51(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block50(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block53(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block52(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block55(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block54(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block57(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block56(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block59(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block58(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block61(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block60(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block63(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block62(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block65(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block64(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block67(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block66(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block69(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block68(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block71(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block70(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block73(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block72(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block75(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block74(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block77(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block76(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block79(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block78(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block81(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block80(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block83(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block82(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block85(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block84(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block87(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block86(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block89(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block88(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block91(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block90(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block93(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block92(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block95(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block94(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block97(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block96(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block98(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 544);
    compiler::TNode<BoolT> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr1(compiler::TNode<BoolT>{tmp0}, compiler::TNode<BoolT>{tmp1}, compiler::TNode<BoolT>{tmp2}));
    ca_.Branch(tmp3, &block2, &block3);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr1(true, true, true)\' failed", "../../test/torque/test-torque.tq", 544);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 545);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr2(compiler::TNode<BoolT>{tmp4}, compiler::TNode<BoolT>{tmp5}, compiler::TNode<BoolT>{tmp6}));
    ca_.Branch(tmp7, &block4, &block5);
  }

  if (block5.is_used()) {
    ca_.Bind(&block5);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr2(true, true, true)\' failed", "../../test/torque/test-torque.tq", 545);
  }

  if (block4.is_used()) {
    ca_.Bind(&block4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 546);
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr3(compiler::TNode<BoolT>{tmp8}, compiler::TNode<BoolT>{tmp9}, compiler::TNode<BoolT>{tmp10}));
    ca_.Branch(tmp11, &block6, &block7);
  }

  if (block7.is_used()) {
    ca_.Bind(&block7);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr3(true, true, true)\' failed", "../../test/torque/test-torque.tq", 546);
  }

  if (block6.is_used()) {
    ca_.Bind(&block6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 547);
    compiler::TNode<BoolT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr1(compiler::TNode<BoolT>{tmp12}, compiler::TNode<BoolT>{tmp13}, compiler::TNode<BoolT>{tmp14}));
    ca_.Branch(tmp15, &block8, &block9);
  }

  if (block9.is_used()) {
    ca_.Bind(&block9);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr1(true, true, false)\' failed", "../../test/torque/test-torque.tq", 547);
  }

  if (block8.is_used()) {
    ca_.Bind(&block8);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 548);
    compiler::TNode<BoolT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr2(compiler::TNode<BoolT>{tmp16}, compiler::TNode<BoolT>{tmp17}, compiler::TNode<BoolT>{tmp18}));
    ca_.Branch(tmp19, &block10, &block11);
  }

  if (block11.is_used()) {
    ca_.Bind(&block11);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr2(true, true, false)\' failed", "../../test/torque/test-torque.tq", 548);
  }

  if (block10.is_used()) {
    ca_.Bind(&block10);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 549);
    compiler::TNode<BoolT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr3(compiler::TNode<BoolT>{tmp20}, compiler::TNode<BoolT>{tmp21}, compiler::TNode<BoolT>{tmp22}));
    ca_.Branch(tmp23, &block12, &block13);
  }

  if (block13.is_used()) {
    ca_.Bind(&block13);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr3(true, true, false)\' failed", "../../test/torque/test-torque.tq", 549);
  }

  if (block12.is_used()) {
    ca_.Bind(&block12);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 550);
    compiler::TNode<BoolT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr1(compiler::TNode<BoolT>{tmp24}, compiler::TNode<BoolT>{tmp25}, compiler::TNode<BoolT>{tmp26}));
    ca_.Branch(tmp27, &block14, &block15);
  }

  if (block15.is_used()) {
    ca_.Bind(&block15);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr1(true, false, true)\' failed", "../../test/torque/test-torque.tq", 550);
  }

  if (block14.is_used()) {
    ca_.Bind(&block14);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 551);
    compiler::TNode<BoolT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr2(compiler::TNode<BoolT>{tmp28}, compiler::TNode<BoolT>{tmp29}, compiler::TNode<BoolT>{tmp30}));
    ca_.Branch(tmp31, &block16, &block17);
  }

  if (block17.is_used()) {
    ca_.Bind(&block17);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr2(true, false, true)\' failed", "../../test/torque/test-torque.tq", 551);
  }

  if (block16.is_used()) {
    ca_.Bind(&block16);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 552);
    compiler::TNode<BoolT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr3(compiler::TNode<BoolT>{tmp32}, compiler::TNode<BoolT>{tmp33}, compiler::TNode<BoolT>{tmp34}));
    ca_.Branch(tmp35, &block18, &block19);
  }

  if (block19.is_used()) {
    ca_.Bind(&block19);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr3(true, false, true)\' failed", "../../test/torque/test-torque.tq", 552);
  }

  if (block18.is_used()) {
    ca_.Bind(&block18);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 553);
    compiler::TNode<BoolT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr1(compiler::TNode<BoolT>{tmp36}, compiler::TNode<BoolT>{tmp37}, compiler::TNode<BoolT>{tmp38}));
    compiler::TNode<BoolT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp39}));
    ca_.Branch(tmp40, &block20, &block21);
  }

  if (block21.is_used()) {
    ca_.Bind(&block21);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestAndOr1(true, false, false)\' failed", "../../test/torque/test-torque.tq", 553);
  }

  if (block20.is_used()) {
    ca_.Bind(&block20);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 554);
    compiler::TNode<BoolT> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr2(compiler::TNode<BoolT>{tmp41}, compiler::TNode<BoolT>{tmp42}, compiler::TNode<BoolT>{tmp43}));
    compiler::TNode<BoolT> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp44}));
    ca_.Branch(tmp45, &block22, &block23);
  }

  if (block23.is_used()) {
    ca_.Bind(&block23);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestAndOr2(true, false, false)\' failed", "../../test/torque/test-torque.tq", 554);
  }

  if (block22.is_used()) {
    ca_.Bind(&block22);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 555);
    compiler::TNode<BoolT> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr3(compiler::TNode<BoolT>{tmp46}, compiler::TNode<BoolT>{tmp47}, compiler::TNode<BoolT>{tmp48}));
    compiler::TNode<BoolT> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp49}));
    ca_.Branch(tmp50, &block24, &block25);
  }

  if (block25.is_used()) {
    ca_.Bind(&block25);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestAndOr3(true, false, false)\' failed", "../../test/torque/test-torque.tq", 555);
  }

  if (block24.is_used()) {
    ca_.Bind(&block24);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 556);
    compiler::TNode<BoolT> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr1(compiler::TNode<BoolT>{tmp51}, compiler::TNode<BoolT>{tmp52}, compiler::TNode<BoolT>{tmp53}));
    ca_.Branch(tmp54, &block26, &block27);
  }

  if (block27.is_used()) {
    ca_.Bind(&block27);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr1(false, true, true)\' failed", "../../test/torque/test-torque.tq", 556);
  }

  if (block26.is_used()) {
    ca_.Bind(&block26);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 557);
    compiler::TNode<BoolT> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr2(compiler::TNode<BoolT>{tmp55}, compiler::TNode<BoolT>{tmp56}, compiler::TNode<BoolT>{tmp57}));
    ca_.Branch(tmp58, &block28, &block29);
  }

  if (block29.is_used()) {
    ca_.Bind(&block29);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr2(false, true, true)\' failed", "../../test/torque/test-torque.tq", 557);
  }

  if (block28.is_used()) {
    ca_.Bind(&block28);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 558);
    compiler::TNode<BoolT> tmp59;
    USE(tmp59);
    tmp59 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp60;
    USE(tmp60);
    tmp60 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp61;
    USE(tmp61);
    tmp61 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr3(compiler::TNode<BoolT>{tmp59}, compiler::TNode<BoolT>{tmp60}, compiler::TNode<BoolT>{tmp61}));
    ca_.Branch(tmp62, &block30, &block31);
  }

  if (block31.is_used()) {
    ca_.Bind(&block31);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr3(false, true, true)\' failed", "../../test/torque/test-torque.tq", 558);
  }

  if (block30.is_used()) {
    ca_.Bind(&block30);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 559);
    compiler::TNode<BoolT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr1(compiler::TNode<BoolT>{tmp63}, compiler::TNode<BoolT>{tmp64}, compiler::TNode<BoolT>{tmp65}));
    compiler::TNode<BoolT> tmp67;
    USE(tmp67);
    tmp67 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp66}));
    ca_.Branch(tmp67, &block32, &block33);
  }

  if (block33.is_used()) {
    ca_.Bind(&block33);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestAndOr1(false, true, false)\' failed", "../../test/torque/test-torque.tq", 559);
  }

  if (block32.is_used()) {
    ca_.Bind(&block32);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 560);
    compiler::TNode<BoolT> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp69;
    USE(tmp69);
    tmp69 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp70;
    USE(tmp70);
    tmp70 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp71;
    USE(tmp71);
    tmp71 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr2(compiler::TNode<BoolT>{tmp68}, compiler::TNode<BoolT>{tmp69}, compiler::TNode<BoolT>{tmp70}));
    compiler::TNode<BoolT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp71}));
    ca_.Branch(tmp72, &block34, &block35);
  }

  if (block35.is_used()) {
    ca_.Bind(&block35);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestAndOr2(false, true, false)\' failed", "../../test/torque/test-torque.tq", 560);
  }

  if (block34.is_used()) {
    ca_.Bind(&block34);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 561);
    compiler::TNode<BoolT> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr3(compiler::TNode<BoolT>{tmp73}, compiler::TNode<BoolT>{tmp74}, compiler::TNode<BoolT>{tmp75}));
    compiler::TNode<BoolT> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp76}));
    ca_.Branch(tmp77, &block36, &block37);
  }

  if (block37.is_used()) {
    ca_.Bind(&block37);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestAndOr3(false, true, false)\' failed", "../../test/torque/test-torque.tq", 561);
  }

  if (block36.is_used()) {
    ca_.Bind(&block36);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 562);
    compiler::TNode<BoolT> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr1(compiler::TNode<BoolT>{tmp78}, compiler::TNode<BoolT>{tmp79}, compiler::TNode<BoolT>{tmp80}));
    ca_.Branch(tmp81, &block38, &block39);
  }

  if (block39.is_used()) {
    ca_.Bind(&block39);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr1(false, false, true)\' failed", "../../test/torque/test-torque.tq", 562);
  }

  if (block38.is_used()) {
    ca_.Bind(&block38);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 563);
    compiler::TNode<BoolT> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp85;
    USE(tmp85);
    tmp85 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr2(compiler::TNode<BoolT>{tmp82}, compiler::TNode<BoolT>{tmp83}, compiler::TNode<BoolT>{tmp84}));
    ca_.Branch(tmp85, &block40, &block41);
  }

  if (block41.is_used()) {
    ca_.Bind(&block41);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr2(false, false, true)\' failed", "../../test/torque/test-torque.tq", 563);
  }

  if (block40.is_used()) {
    ca_.Bind(&block40);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 564);
    compiler::TNode<BoolT> tmp86;
    USE(tmp86);
    tmp86 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp87;
    USE(tmp87);
    tmp87 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp88;
    USE(tmp88);
    tmp88 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr3(compiler::TNode<BoolT>{tmp86}, compiler::TNode<BoolT>{tmp87}, compiler::TNode<BoolT>{tmp88}));
    ca_.Branch(tmp89, &block42, &block43);
  }

  if (block43.is_used()) {
    ca_.Bind(&block43);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestAndOr3(false, false, true)\' failed", "../../test/torque/test-torque.tq", 564);
  }

  if (block42.is_used()) {
    ca_.Bind(&block42);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 565);
    compiler::TNode<BoolT> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr1(compiler::TNode<BoolT>{tmp90}, compiler::TNode<BoolT>{tmp91}, compiler::TNode<BoolT>{tmp92}));
    compiler::TNode<BoolT> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp93}));
    ca_.Branch(tmp94, &block44, &block45);
  }

  if (block45.is_used()) {
    ca_.Bind(&block45);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestAndOr1(false, false, false)\' failed", "../../test/torque/test-torque.tq", 565);
  }

  if (block44.is_used()) {
    ca_.Bind(&block44);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 566);
    compiler::TNode<BoolT> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp97;
    USE(tmp97);
    tmp97 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr2(compiler::TNode<BoolT>{tmp95}, compiler::TNode<BoolT>{tmp96}, compiler::TNode<BoolT>{tmp97}));
    compiler::TNode<BoolT> tmp99;
    USE(tmp99);
    tmp99 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp98}));
    ca_.Branch(tmp99, &block46, &block47);
  }

  if (block47.is_used()) {
    ca_.Bind(&block47);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestAndOr2(false, false, false)\' failed", "../../test/torque/test-torque.tq", 566);
  }

  if (block46.is_used()) {
    ca_.Bind(&block46);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 567);
    compiler::TNode<BoolT> tmp100;
    USE(tmp100);
    tmp100 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp101;
    USE(tmp101);
    tmp101 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp102;
    USE(tmp102);
    tmp102 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp103;
    USE(tmp103);
    tmp103 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestAndOr3(compiler::TNode<BoolT>{tmp100}, compiler::TNode<BoolT>{tmp101}, compiler::TNode<BoolT>{tmp102}));
    compiler::TNode<BoolT> tmp104;
    USE(tmp104);
    tmp104 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp103}));
    ca_.Branch(tmp104, &block48, &block49);
  }

  if (block49.is_used()) {
    ca_.Bind(&block49);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestAndOr3(false, false, false)\' failed", "../../test/torque/test-torque.tq", 567);
  }

  if (block48.is_used()) {
    ca_.Bind(&block48);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 568);
    compiler::TNode<BoolT> tmp105;
    USE(tmp105);
    tmp105 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp106;
    USE(tmp106);
    tmp106 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp107;
    USE(tmp107);
    tmp107 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp108;
    USE(tmp108);
    tmp108 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd1(compiler::TNode<BoolT>{tmp105}, compiler::TNode<BoolT>{tmp106}, compiler::TNode<BoolT>{tmp107}));
    ca_.Branch(tmp108, &block50, &block51);
  }

  if (block51.is_used()) {
    ca_.Bind(&block51);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd1(true, true, true)\' failed", "../../test/torque/test-torque.tq", 568);
  }

  if (block50.is_used()) {
    ca_.Bind(&block50);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 569);
    compiler::TNode<BoolT> tmp109;
    USE(tmp109);
    tmp109 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp110;
    USE(tmp110);
    tmp110 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp111;
    USE(tmp111);
    tmp111 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp112;
    USE(tmp112);
    tmp112 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd2(compiler::TNode<BoolT>{tmp109}, compiler::TNode<BoolT>{tmp110}, compiler::TNode<BoolT>{tmp111}));
    ca_.Branch(tmp112, &block52, &block53);
  }

  if (block53.is_used()) {
    ca_.Bind(&block53);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd2(true, true, true)\' failed", "../../test/torque/test-torque.tq", 569);
  }

  if (block52.is_used()) {
    ca_.Bind(&block52);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 570);
    compiler::TNode<BoolT> tmp113;
    USE(tmp113);
    tmp113 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp114;
    USE(tmp114);
    tmp114 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp115;
    USE(tmp115);
    tmp115 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp116;
    USE(tmp116);
    tmp116 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd3(compiler::TNode<BoolT>{tmp113}, compiler::TNode<BoolT>{tmp114}, compiler::TNode<BoolT>{tmp115}));
    ca_.Branch(tmp116, &block54, &block55);
  }

  if (block55.is_used()) {
    ca_.Bind(&block55);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd3(true, true, true)\' failed", "../../test/torque/test-torque.tq", 570);
  }

  if (block54.is_used()) {
    ca_.Bind(&block54);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 571);
    compiler::TNode<BoolT> tmp117;
    USE(tmp117);
    tmp117 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp120;
    USE(tmp120);
    tmp120 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd1(compiler::TNode<BoolT>{tmp117}, compiler::TNode<BoolT>{tmp118}, compiler::TNode<BoolT>{tmp119}));
    ca_.Branch(tmp120, &block56, &block57);
  }

  if (block57.is_used()) {
    ca_.Bind(&block57);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd1(true, true, false)\' failed", "../../test/torque/test-torque.tq", 571);
  }

  if (block56.is_used()) {
    ca_.Bind(&block56);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 572);
    compiler::TNode<BoolT> tmp121;
    USE(tmp121);
    tmp121 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp122;
    USE(tmp122);
    tmp122 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp123;
    USE(tmp123);
    tmp123 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp124;
    USE(tmp124);
    tmp124 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd2(compiler::TNode<BoolT>{tmp121}, compiler::TNode<BoolT>{tmp122}, compiler::TNode<BoolT>{tmp123}));
    ca_.Branch(tmp124, &block58, &block59);
  }

  if (block59.is_used()) {
    ca_.Bind(&block59);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd2(true, true, false)\' failed", "../../test/torque/test-torque.tq", 572);
  }

  if (block58.is_used()) {
    ca_.Bind(&block58);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 573);
    compiler::TNode<BoolT> tmp125;
    USE(tmp125);
    tmp125 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp126;
    USE(tmp126);
    tmp126 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp128;
    USE(tmp128);
    tmp128 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd3(compiler::TNode<BoolT>{tmp125}, compiler::TNode<BoolT>{tmp126}, compiler::TNode<BoolT>{tmp127}));
    ca_.Branch(tmp128, &block60, &block61);
  }

  if (block61.is_used()) {
    ca_.Bind(&block61);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd3(true, true, false)\' failed", "../../test/torque/test-torque.tq", 573);
  }

  if (block60.is_used()) {
    ca_.Bind(&block60);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 574);
    compiler::TNode<BoolT> tmp129;
    USE(tmp129);
    tmp129 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp130;
    USE(tmp130);
    tmp130 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp131;
    USE(tmp131);
    tmp131 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp132;
    USE(tmp132);
    tmp132 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd1(compiler::TNode<BoolT>{tmp129}, compiler::TNode<BoolT>{tmp130}, compiler::TNode<BoolT>{tmp131}));
    ca_.Branch(tmp132, &block62, &block63);
  }

  if (block63.is_used()) {
    ca_.Bind(&block63);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd1(true, false, true)\' failed", "../../test/torque/test-torque.tq", 574);
  }

  if (block62.is_used()) {
    ca_.Bind(&block62);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 575);
    compiler::TNode<BoolT> tmp133;
    USE(tmp133);
    tmp133 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp134;
    USE(tmp134);
    tmp134 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp135;
    USE(tmp135);
    tmp135 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp136;
    USE(tmp136);
    tmp136 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd2(compiler::TNode<BoolT>{tmp133}, compiler::TNode<BoolT>{tmp134}, compiler::TNode<BoolT>{tmp135}));
    ca_.Branch(tmp136, &block64, &block65);
  }

  if (block65.is_used()) {
    ca_.Bind(&block65);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd2(true, false, true)\' failed", "../../test/torque/test-torque.tq", 575);
  }

  if (block64.is_used()) {
    ca_.Bind(&block64);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 576);
    compiler::TNode<BoolT> tmp137;
    USE(tmp137);
    tmp137 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp138;
    USE(tmp138);
    tmp138 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp139;
    USE(tmp139);
    tmp139 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp140;
    USE(tmp140);
    tmp140 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd3(compiler::TNode<BoolT>{tmp137}, compiler::TNode<BoolT>{tmp138}, compiler::TNode<BoolT>{tmp139}));
    ca_.Branch(tmp140, &block66, &block67);
  }

  if (block67.is_used()) {
    ca_.Bind(&block67);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd3(true, false, true)\' failed", "../../test/torque/test-torque.tq", 576);
  }

  if (block66.is_used()) {
    ca_.Bind(&block66);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 577);
    compiler::TNode<BoolT> tmp141;
    USE(tmp141);
    tmp141 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp142;
    USE(tmp142);
    tmp142 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp143;
    USE(tmp143);
    tmp143 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp144;
    USE(tmp144);
    tmp144 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd1(compiler::TNode<BoolT>{tmp141}, compiler::TNode<BoolT>{tmp142}, compiler::TNode<BoolT>{tmp143}));
    ca_.Branch(tmp144, &block68, &block69);
  }

  if (block69.is_used()) {
    ca_.Bind(&block69);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd1(true, false, false)\' failed", "../../test/torque/test-torque.tq", 577);
  }

  if (block68.is_used()) {
    ca_.Bind(&block68);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 578);
    compiler::TNode<BoolT> tmp145;
    USE(tmp145);
    tmp145 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp146;
    USE(tmp146);
    tmp146 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp147;
    USE(tmp147);
    tmp147 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp148;
    USE(tmp148);
    tmp148 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd2(compiler::TNode<BoolT>{tmp145}, compiler::TNode<BoolT>{tmp146}, compiler::TNode<BoolT>{tmp147}));
    ca_.Branch(tmp148, &block70, &block71);
  }

  if (block71.is_used()) {
    ca_.Bind(&block71);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd2(true, false, false)\' failed", "../../test/torque/test-torque.tq", 578);
  }

  if (block70.is_used()) {
    ca_.Bind(&block70);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 579);
    compiler::TNode<BoolT> tmp149;
    USE(tmp149);
    tmp149 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp150;
    USE(tmp150);
    tmp150 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp151;
    USE(tmp151);
    tmp151 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp152;
    USE(tmp152);
    tmp152 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd3(compiler::TNode<BoolT>{tmp149}, compiler::TNode<BoolT>{tmp150}, compiler::TNode<BoolT>{tmp151}));
    ca_.Branch(tmp152, &block72, &block73);
  }

  if (block73.is_used()) {
    ca_.Bind(&block73);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd3(true, false, false)\' failed", "../../test/torque/test-torque.tq", 579);
  }

  if (block72.is_used()) {
    ca_.Bind(&block72);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 580);
    compiler::TNode<BoolT> tmp153;
    USE(tmp153);
    tmp153 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp154;
    USE(tmp154);
    tmp154 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp155;
    USE(tmp155);
    tmp155 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp156;
    USE(tmp156);
    tmp156 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd1(compiler::TNode<BoolT>{tmp153}, compiler::TNode<BoolT>{tmp154}, compiler::TNode<BoolT>{tmp155}));
    ca_.Branch(tmp156, &block74, &block75);
  }

  if (block75.is_used()) {
    ca_.Bind(&block75);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd1(false, true, true)\' failed", "../../test/torque/test-torque.tq", 580);
  }

  if (block74.is_used()) {
    ca_.Bind(&block74);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 581);
    compiler::TNode<BoolT> tmp157;
    USE(tmp157);
    tmp157 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp158;
    USE(tmp158);
    tmp158 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp159;
    USE(tmp159);
    tmp159 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp160;
    USE(tmp160);
    tmp160 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd2(compiler::TNode<BoolT>{tmp157}, compiler::TNode<BoolT>{tmp158}, compiler::TNode<BoolT>{tmp159}));
    ca_.Branch(tmp160, &block76, &block77);
  }

  if (block77.is_used()) {
    ca_.Bind(&block77);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd2(false, true, true)\' failed", "../../test/torque/test-torque.tq", 581);
  }

  if (block76.is_used()) {
    ca_.Bind(&block76);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 582);
    compiler::TNode<BoolT> tmp161;
    USE(tmp161);
    tmp161 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp162;
    USE(tmp162);
    tmp162 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp163;
    USE(tmp163);
    tmp163 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp164;
    USE(tmp164);
    tmp164 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd3(compiler::TNode<BoolT>{tmp161}, compiler::TNode<BoolT>{tmp162}, compiler::TNode<BoolT>{tmp163}));
    ca_.Branch(tmp164, &block78, &block79);
  }

  if (block79.is_used()) {
    ca_.Bind(&block79);
    CodeStubAssembler(state_).FailAssert("Torque assert \'TestOrAnd3(false, true, true)\' failed", "../../test/torque/test-torque.tq", 582);
  }

  if (block78.is_used()) {
    ca_.Bind(&block78);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 583);
    compiler::TNode<BoolT> tmp165;
    USE(tmp165);
    tmp165 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp166;
    USE(tmp166);
    tmp166 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp168;
    USE(tmp168);
    tmp168 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd1(compiler::TNode<BoolT>{tmp165}, compiler::TNode<BoolT>{tmp166}, compiler::TNode<BoolT>{tmp167}));
    compiler::TNode<BoolT> tmp169;
    USE(tmp169);
    tmp169 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp168}));
    ca_.Branch(tmp169, &block80, &block81);
  }

  if (block81.is_used()) {
    ca_.Bind(&block81);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestOrAnd1(false, true, false)\' failed", "../../test/torque/test-torque.tq", 583);
  }

  if (block80.is_used()) {
    ca_.Bind(&block80);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 584);
    compiler::TNode<BoolT> tmp170;
    USE(tmp170);
    tmp170 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp171;
    USE(tmp171);
    tmp171 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp172;
    USE(tmp172);
    tmp172 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp173;
    USE(tmp173);
    tmp173 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd2(compiler::TNode<BoolT>{tmp170}, compiler::TNode<BoolT>{tmp171}, compiler::TNode<BoolT>{tmp172}));
    compiler::TNode<BoolT> tmp174;
    USE(tmp174);
    tmp174 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp173}));
    ca_.Branch(tmp174, &block82, &block83);
  }

  if (block83.is_used()) {
    ca_.Bind(&block83);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestOrAnd2(false, true, false)\' failed", "../../test/torque/test-torque.tq", 584);
  }

  if (block82.is_used()) {
    ca_.Bind(&block82);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 585);
    compiler::TNode<BoolT> tmp175;
    USE(tmp175);
    tmp175 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp176;
    USE(tmp176);
    tmp176 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp177;
    USE(tmp177);
    tmp177 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp178;
    USE(tmp178);
    tmp178 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd3(compiler::TNode<BoolT>{tmp175}, compiler::TNode<BoolT>{tmp176}, compiler::TNode<BoolT>{tmp177}));
    compiler::TNode<BoolT> tmp179;
    USE(tmp179);
    tmp179 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp178}));
    ca_.Branch(tmp179, &block84, &block85);
  }

  if (block85.is_used()) {
    ca_.Bind(&block85);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestOrAnd3(false, true, false)\' failed", "../../test/torque/test-torque.tq", 585);
  }

  if (block84.is_used()) {
    ca_.Bind(&block84);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 586);
    compiler::TNode<BoolT> tmp180;
    USE(tmp180);
    tmp180 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp181;
    USE(tmp181);
    tmp181 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp182;
    USE(tmp182);
    tmp182 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp183;
    USE(tmp183);
    tmp183 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd1(compiler::TNode<BoolT>{tmp180}, compiler::TNode<BoolT>{tmp181}, compiler::TNode<BoolT>{tmp182}));
    compiler::TNode<BoolT> tmp184;
    USE(tmp184);
    tmp184 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp183}));
    ca_.Branch(tmp184, &block86, &block87);
  }

  if (block87.is_used()) {
    ca_.Bind(&block87);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestOrAnd1(false, false, true)\' failed", "../../test/torque/test-torque.tq", 586);
  }

  if (block86.is_used()) {
    ca_.Bind(&block86);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 587);
    compiler::TNode<BoolT> tmp185;
    USE(tmp185);
    tmp185 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp186;
    USE(tmp186);
    tmp186 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp187;
    USE(tmp187);
    tmp187 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp188;
    USE(tmp188);
    tmp188 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd2(compiler::TNode<BoolT>{tmp185}, compiler::TNode<BoolT>{tmp186}, compiler::TNode<BoolT>{tmp187}));
    compiler::TNode<BoolT> tmp189;
    USE(tmp189);
    tmp189 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp188}));
    ca_.Branch(tmp189, &block88, &block89);
  }

  if (block89.is_used()) {
    ca_.Bind(&block89);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestOrAnd2(false, false, true)\' failed", "../../test/torque/test-torque.tq", 587);
  }

  if (block88.is_used()) {
    ca_.Bind(&block88);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 588);
    compiler::TNode<BoolT> tmp190;
    USE(tmp190);
    tmp190 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp191;
    USE(tmp191);
    tmp191 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp192;
    USE(tmp192);
    tmp192 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    compiler::TNode<BoolT> tmp193;
    USE(tmp193);
    tmp193 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd3(compiler::TNode<BoolT>{tmp190}, compiler::TNode<BoolT>{tmp191}, compiler::TNode<BoolT>{tmp192}));
    compiler::TNode<BoolT> tmp194;
    USE(tmp194);
    tmp194 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp193}));
    ca_.Branch(tmp194, &block90, &block91);
  }

  if (block91.is_used()) {
    ca_.Bind(&block91);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestOrAnd3(false, false, true)\' failed", "../../test/torque/test-torque.tq", 588);
  }

  if (block90.is_used()) {
    ca_.Bind(&block90);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 589);
    compiler::TNode<BoolT> tmp195;
    USE(tmp195);
    tmp195 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp196;
    USE(tmp196);
    tmp196 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp197;
    USE(tmp197);
    tmp197 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp198;
    USE(tmp198);
    tmp198 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd1(compiler::TNode<BoolT>{tmp195}, compiler::TNode<BoolT>{tmp196}, compiler::TNode<BoolT>{tmp197}));
    compiler::TNode<BoolT> tmp199;
    USE(tmp199);
    tmp199 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp198}));
    ca_.Branch(tmp199, &block92, &block93);
  }

  if (block93.is_used()) {
    ca_.Bind(&block93);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestOrAnd1(false, false, false)\' failed", "../../test/torque/test-torque.tq", 589);
  }

  if (block92.is_used()) {
    ca_.Bind(&block92);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 590);
    compiler::TNode<BoolT> tmp200;
    USE(tmp200);
    tmp200 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp201;
    USE(tmp201);
    tmp201 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp202;
    USE(tmp202);
    tmp202 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp203;
    USE(tmp203);
    tmp203 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd2(compiler::TNode<BoolT>{tmp200}, compiler::TNode<BoolT>{tmp201}, compiler::TNode<BoolT>{tmp202}));
    compiler::TNode<BoolT> tmp204;
    USE(tmp204);
    tmp204 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp203}));
    ca_.Branch(tmp204, &block94, &block95);
  }

  if (block95.is_used()) {
    ca_.Bind(&block95);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestOrAnd2(false, false, false)\' failed", "../../test/torque/test-torque.tq", 590);
  }

  if (block94.is_used()) {
    ca_.Bind(&block94);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 591);
    compiler::TNode<BoolT> tmp205;
    USE(tmp205);
    tmp205 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp206;
    USE(tmp206);
    tmp206 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp207;
    USE(tmp207);
    tmp207 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    compiler::TNode<BoolT> tmp208;
    USE(tmp208);
    tmp208 = ca_.UncheckedCast<BoolT>(TestBuiltinsFromDSLAssembler(state_).TestOrAnd3(compiler::TNode<BoolT>{tmp205}, compiler::TNode<BoolT>{tmp206}, compiler::TNode<BoolT>{tmp207}));
    compiler::TNode<BoolT> tmp209;
    USE(tmp209);
    tmp209 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp208}));
    ca_.Branch(tmp209, &block96, &block97);
  }

  if (block97.is_used()) {
    ca_.Bind(&block97);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!TestOrAnd3(false, false, false)\' failed", "../../test/torque/test-torque.tq", 591);
  }

  if (block96.is_used()) {
    ca_.Bind(&block96);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 543);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block98);
  }

    ca_.Bind(&block98);
}

compiler::TNode<Smi> TestBuiltinsFromDSLAssembler::TestCall(compiler::TNode<Smi> p_i, compiler::CodeAssemblerLabel* label_A) {
  compiler::CodeAssemblerParameterizedLabel<Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_i);

  if (block0.is_used()) {
    compiler::TNode<Smi> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 596);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp0}, compiler::TNode<Smi>{tmp1}));
    ca_.Branch(tmp2, &block3, &block4, tmp0);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block3, &tmp3);
    ca_.Goto(&block2, tmp3, tmp3);
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp4;
    ca_.Bind(&block4, &tmp4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 597);
    ca_.Goto(&block1);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp5;
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block2, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 594);
    ca_.Goto(&block5, tmp5, tmp6);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_A);
  }

    compiler::TNode<Smi> tmp7;
    compiler::TNode<Smi> tmp8;
    ca_.Bind(&block5, &tmp7, &tmp8);
  return compiler::TNode<Smi>{tmp8};
}

void TestBuiltinsFromDSLAssembler::TestOtherwiseWithCode1() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 601);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 602);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 604);
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp3 = TestBuiltinsFromDSLAssembler(state_).TestCall(compiler::TNode<Smi>{tmp2}, &label0);
    ca_.Goto(&block6, tmp0, tmp1, tmp3);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp0, tmp1);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Smi> tmp5;
    ca_.Bind(&block7, &tmp4, &tmp5);
    ca_.Goto(&block5, tmp4, tmp5);
  }

  if (block6.is_used()) {
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Smi> tmp8;
    ca_.Bind(&block6, &tmp6, &tmp7, &tmp8);
    ca_.Goto(&block4, tmp6, tmp7, tmp8);
  }

  if (block5.is_used()) {
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block5, &tmp9, &tmp10);
    compiler::TNode<Smi> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp10}, compiler::TNode<Smi>{tmp11}));
    ca_.Goto(&block3, tmp9, tmp12, tmp12);
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Smi> tmp15;
    ca_.Bind(&block4, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 606);
    ca_.Goto(&block2, tmp13, tmp14);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Smi> tmp17;
    compiler::TNode<Smi> tmp18;
    ca_.Bind(&block3, &tmp16, &tmp17, &tmp18);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 607);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 603);
    ca_.Goto(&block2, tmp18, tmp17);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Smi> tmp20;
    ca_.Bind(&block2, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 609);
    compiler::TNode<Smi> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp19}, compiler::TNode<Smi>{tmp21}));
    ca_.Branch(tmp22, &block8, &block9, tmp19, tmp20);
  }

  if (block9.is_used()) {
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Smi> tmp24;
    ca_.Bind(&block9, &tmp23, &tmp24);
    CodeStubAssembler(state_).FailAssert("Torque assert \'v == 2\' failed", "../../test/torque/test-torque.tq", 609);
  }

  if (block8.is_used()) {
    compiler::TNode<Smi> tmp25;
    compiler::TNode<Smi> tmp26;
    ca_.Bind(&block8, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 600);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block10);
  }

    ca_.Bind(&block10);
}

void TestBuiltinsFromDSLAssembler::TestOtherwiseWithCode2() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block11(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 613);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 614);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block4, tmp0, tmp1);
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block4, &tmp2, &tmp3);
    compiler::TNode<Smi> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp3}, compiler::TNode<Smi>{tmp4}));
    ca_.Branch(tmp5, &block2, &block3, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block2, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 615);
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp8 = TestBuiltinsFromDSLAssembler(state_).TestCall(compiler::TNode<Smi>{tmp7}, &label0);
    ca_.Goto(&block8, tmp6, tmp7, tmp7, tmp8);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp6, tmp7, tmp7);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block9, &tmp9, &tmp10, &tmp11);
    ca_.Goto(&block7, tmp9, tmp10);
  }

  if (block8.is_used()) {
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Smi> tmp15;
    ca_.Bind(&block8, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.Goto(&block6, tmp12, tmp13, tmp15);
  }

  if (block7.is_used()) {
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Smi> tmp17;
    ca_.Bind(&block7, &tmp16, &tmp17);
    ca_.Goto(&block3, tmp16, tmp17);
  }

  if (block6.is_used()) {
    compiler::TNode<Smi> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Smi> tmp20;
    ca_.Bind(&block6, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 616);
    compiler::TNode<Smi> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp18}, compiler::TNode<Smi>{tmp21}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 614);
    ca_.Goto(&block5, tmp22, tmp19);
  }

  if (block5.is_used()) {
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Smi> tmp24;
    ca_.Bind(&block5, &tmp23, &tmp24);
    compiler::TNode<Smi> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp24}, compiler::TNode<Smi>{tmp25}));
    ca_.Goto(&block4, tmp23, tmp26);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp27;
    compiler::TNode<Smi> tmp28;
    ca_.Bind(&block3, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 618);
    compiler::TNode<Smi> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp29}));
    ca_.Branch(tmp30, &block10, &block11, tmp27);
  }

  if (block11.is_used()) {
    compiler::TNode<Smi> tmp31;
    ca_.Bind(&block11, &tmp31);
    CodeStubAssembler(state_).FailAssert("Torque assert \'s == 5\' failed", "../../test/torque/test-torque.tq", 618);
  }

  if (block10.is_used()) {
    compiler::TNode<Smi> tmp32;
    ca_.Bind(&block10, &tmp32);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 612);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block12);
  }

    ca_.Bind(&block12);
}

void TestBuiltinsFromDSLAssembler::TestOtherwiseWithCode3() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block11(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 622);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 623);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block4, tmp0, tmp1);
  }

  if (block4.is_used()) {
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block4, &tmp2, &tmp3);
    compiler::TNode<Smi> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp3}, compiler::TNode<Smi>{tmp4}));
    ca_.Branch(tmp5, &block2, &block3, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block2, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 624);
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp8 = TestBuiltinsFromDSLAssembler(state_).TestCall(compiler::TNode<Smi>{tmp7}, &label0);
    ca_.Goto(&block8, tmp6, tmp7, tmp6, tmp7, tmp8);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp6, tmp7, tmp6, tmp7);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<Smi> tmp12;
    ca_.Bind(&block9, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.Goto(&block7, tmp9, tmp10, tmp11);
  }

  if (block8.is_used()) {
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Smi> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Smi> tmp17;
    ca_.Bind(&block8, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.Goto(&block6, tmp13, tmp14, tmp15, tmp17);
  }

  if (block7.is_used()) {
    compiler::TNode<Smi> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Smi> tmp20;
    ca_.Bind(&block7, &tmp18, &tmp19, &tmp20);
    ca_.Goto(&block3, tmp18, tmp19);
  }

  if (block6.is_used()) {
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Smi> tmp22;
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Smi> tmp24;
    ca_.Bind(&block6, &tmp21, &tmp22, &tmp23, &tmp24);
    compiler::TNode<Smi> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp23}, compiler::TNode<Smi>{tmp24}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 623);
    ca_.Goto(&block5, tmp25, tmp22);
  }

  if (block5.is_used()) {
    compiler::TNode<Smi> tmp26;
    compiler::TNode<Smi> tmp27;
    ca_.Bind(&block5, &tmp26, &tmp27);
    compiler::TNode<Smi> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp28}));
    ca_.Goto(&block4, tmp26, tmp29);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp30;
    compiler::TNode<Smi> tmp31;
    ca_.Bind(&block3, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 626);
    compiler::TNode<Smi> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(10));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp30}, compiler::TNode<Smi>{tmp32}));
    ca_.Branch(tmp33, &block10, &block11, tmp30);
  }

  if (block11.is_used()) {
    compiler::TNode<Smi> tmp34;
    ca_.Bind(&block11, &tmp34);
    CodeStubAssembler(state_).FailAssert("Torque assert \'s == 10\' failed", "../../test/torque/test-torque.tq", 626);
  }

  if (block10.is_used()) {
    compiler::TNode<Smi> tmp35;
    ca_.Bind(&block10, &tmp35);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 621);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block12);
  }

    ca_.Bind(&block12);
}

void TestBuiltinsFromDSLAssembler::TestForwardLabel() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 631);
    ca_.Goto(&block5);
  }

  if (block5.is_used()) {
    ca_.Bind(&block5);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 634);
    compiler::TNode<Smi> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    ca_.Goto(&block3, tmp0);
  }

  if (block3.is_used()) {
    compiler::TNode<Smi> tmp1;
    ca_.Bind(&block3, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 637);
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp1}, compiler::TNode<Smi>{tmp2}));
    ca_.Branch(tmp3, &block6, &block7, tmp1);
  }

  if (block7.is_used()) {
    compiler::TNode<Smi> tmp4;
    ca_.Bind(&block7, &tmp4);
    CodeStubAssembler(state_).FailAssert("Torque assert \'b == 5\' failed", "../../test/torque/test-torque.tq", 637);
  }

  if (block6.is_used()) {
    compiler::TNode<Smi> tmp5;
    ca_.Bind(&block6, &tmp5);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 630);
    ca_.Goto(&block2, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block2, &tmp6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 629);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(&block8);
  }

    ca_.Bind(&block8);
}

void TestBuiltinsFromDSLAssembler::TestQualifiedAccess(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 642);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 643);
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(ArrayBuiltinsFromDSLAssembler(state_).IsJSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp2}));
    ca_.Branch(tmp3, &block2, &block3, tmp0, tmp1);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp4;
    compiler::TNode<Smi> tmp5;
    ca_.Bind(&block3, &tmp4, &tmp5);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!array::IsJSArray(s)\' failed", "../../test/torque/test-torque.tq", 643);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block2, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 641);
    ca_.Goto(&block1, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp8;
    ca_.Bind(&block1, &tmp8);
    ca_.Goto(&block4, tmp8);
  }

    compiler::TNode<Context> tmp9;
    ca_.Bind(&block4, &tmp9);
}

compiler::TNode<Smi> TestBuiltinsFromDSLAssembler::TestCatch1(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 647);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 649);
    compiler::CodeAssemblerExceptionHandlerLabel catch2_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch2_label);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp0}, MessageTemplate::kInvalidArrayLength);
    }
    if (catch2_label.is_used()) {
      compiler::CodeAssemblerLabel catch2_skip(&ca_);
      compiler::TNode<Object> catch2_exception_object;
      ca_.Bind(&catch2_label, &catch2_exception_object);
      ca_.Goto(&block4, tmp0, tmp1, catch2_exception_object);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block4, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block3, tmp3, tmp4, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Object> tmp8;
    ca_.Bind(&block3, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 651);
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 652);
    ca_.Goto(&block1, tmp6, tmp9);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block1, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 646);
    ca_.Goto(&block5, tmp10, tmp11);
  }

    compiler::TNode<Context> tmp12;
    compiler::TNode<Smi> tmp13;
    ca_.Bind(&block5, &tmp12, &tmp13);
  return compiler::TNode<Smi>{tmp13};
}

void TestBuiltinsFromDSLAssembler::TestCatch2Wrapper(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 657);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp0}, MessageTemplate::kInvalidArrayLength);
  }
}

compiler::TNode<Smi> TestBuiltinsFromDSLAssembler::TestCatch2(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 661);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 663);
    compiler::CodeAssemblerExceptionHandlerLabel catch2_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch2_label);
    TestBuiltinsFromDSLAssembler(state_).TestCatch2Wrapper(compiler::TNode<Context>{tmp0});
    }
    if (catch2_label.is_used()) {
      compiler::CodeAssemblerLabel catch2_skip(&ca_);
      compiler::TNode<Object> catch2_exception_object;
      ca_.Bind(&catch2_label, &catch2_exception_object);
      ca_.Goto(&block4, tmp0, tmp1, catch2_exception_object);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block4, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block3, tmp3, tmp4, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Object> tmp8;
    ca_.Bind(&block3, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 665);
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(2));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 666);
    ca_.Goto(&block1, tmp6, tmp9);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block1, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 660);
    ca_.Goto(&block5, tmp10, tmp11);
  }

    compiler::TNode<Context> tmp12;
    compiler::TNode<Smi> tmp13;
    ca_.Bind(&block5, &tmp12, &tmp13);
  return compiler::TNode<Smi>{tmp13};
}

void TestBuiltinsFromDSLAssembler::TestCatch3WrapperWithLabel(compiler::TNode<Context> p_context, compiler::CodeAssemblerLabel* label_Abort) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 672);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp0}, MessageTemplate::kInvalidArrayLength);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 670);
    ca_.Goto(label_Abort);
  }
}

compiler::TNode<Smi> TestBuiltinsFromDSLAssembler::TestCatch3(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block8(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 676);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 678);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerExceptionHandlerLabel catch2_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch2_label);
    TestBuiltinsFromDSLAssembler(state_).TestCatch3WrapperWithLabel(compiler::TNode<Context>{tmp0}, &label0);
    }
    if (catch2_label.is_used()) {
      compiler::CodeAssemblerLabel catch2_skip(&ca_);
      compiler::TNode<Object> catch2_exception_object;
      ca_.Bind(&catch2_label, &catch2_exception_object);
      ca_.Goto(&block7, tmp0, tmp1, catch2_exception_object);
    }
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp0, tmp1);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block7, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block3, tmp3, tmp4, tmp5);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block6, &tmp6, &tmp7);
    ca_.Goto(&block5, tmp6, tmp7);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Smi> tmp9;
    ca_.Bind(&block5, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 681);
    compiler::TNode<Smi> tmp10;
    USE(tmp10);
    compiler::CodeAssemblerExceptionHandlerLabel catch11_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch11_label);
    tmp10 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(-1));
    }
    if (catch11_label.is_used()) {
      compiler::CodeAssemblerLabel catch11_skip(&ca_);
      ca_.Goto(&catch11_skip);
      compiler::TNode<Object> catch11_exception_object;
      ca_.Bind(&catch11_label, &catch11_exception_object);
      ca_.Goto(&block8, tmp8, tmp9, catch11_exception_object);
      ca_.Bind(&catch11_skip);
    }
    ca_.Goto(&block1, tmp8, tmp10);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Object> tmp14;
    ca_.Bind(&block8, &tmp12, &tmp13, &tmp14);
    ca_.Goto(&block3, tmp12, tmp13, tmp14);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Object> tmp17;
    ca_.Bind(&block3, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 684);
    compiler::TNode<Smi> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(2));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 685);
    ca_.Goto(&block1, tmp15, tmp18);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<Smi> tmp20;
    ca_.Bind(&block1, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 675);
    ca_.Goto(&block9, tmp19, tmp20);
  }

    compiler::TNode<Context> tmp21;
    compiler::TNode<Smi> tmp22;
    ca_.Bind(&block9, &tmp21, &tmp22);
  return compiler::TNode<Smi>{tmp22};
}

void TestBuiltinsFromDSLAssembler::TestIterator(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o, compiler::TNode<Map> p_map) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Map> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Map, Object, JSReceiver, Object, JSReceiver, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Map, Object, JSReceiver, Object, JSReceiver, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Map, Object, JSReceiver, Object, Object, JSReceiver, Object, Map> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Map, Object, JSReceiver, Object, Object, JSReceiver, Object, Map, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Map> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Map> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Map> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Map> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_map);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Map> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 695);
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(IteratorBuiltinsAssembler(state_).GetIteratorMethod(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 696);
    compiler::TNode<JSReceiver> tmp4;
    USE(tmp4);
    compiler::TNode<Object> tmp5;
    USE(tmp5);
    std::tie(tmp4, tmp5) = IteratorBuiltinsAssembler(state_).GetIterator(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}).Flatten();
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 698);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp6 = IteratorBuiltinsAssembler(state_).IteratorStep(compiler::TNode<Context>{tmp0}, IteratorBuiltinsFromDSLAssembler::IteratorRecord{compiler::TNode<JSReceiver>{tmp4}, compiler::TNode<Object>{tmp5}}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp4, tmp5, tmp6);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp4, tmp5);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Map> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    compiler::TNode<Object> tmp14;
    ca_.Bind(&block5, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.Goto(&block3, tmp7, tmp8, tmp9);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Map> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<JSReceiver> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<Object> tmp23;
    ca_.Bind(&block4, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 699);
    compiler::TNode<Object> tmp24;
    USE(tmp24);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp24 = IteratorBuiltinsAssembler(state_).IteratorStep(compiler::TNode<Context>{tmp15}, IteratorBuiltinsFromDSLAssembler::IteratorRecord{compiler::TNode<JSReceiver>{tmp19}, compiler::TNode<Object>{tmp20}}, compiler::TNode<Map>{tmp17}, &label0);
    ca_.Goto(&block6, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp23, tmp19, tmp20, tmp17, tmp24);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp23, tmp19, tmp20, tmp17);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Map> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Map> tmp34;
    ca_.Bind(&block7, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.Goto(&block3, tmp25, tmp26, tmp27);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Map> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Map> tmp44;
    compiler::TNode<Object> tmp45;
    ca_.Bind(&block6, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 701);
    compiler::TNode<Object> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Object>(IteratorBuiltinsAssembler(state_).IteratorValue(compiler::TNode<Context>{tmp35}, compiler::TNode<Object>{tmp45}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 702);
    compiler::TNode<Object> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<Object>(IteratorBuiltinsAssembler(state_).IteratorValue(compiler::TNode<Context>{tmp35}, compiler::TNode<Object>{tmp45}, compiler::TNode<Map>{tmp37}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 704);
    compiler::TNode<JSArray> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<JSArray>(IteratorBuiltinsAssembler(state_).IterableToList(compiler::TNode<Context>{tmp35}, compiler::TNode<Object>{tmp38}, compiler::TNode<Object>{tmp38}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 706);
    IteratorBuiltinsAssembler(state_).IteratorCloseOnException(compiler::TNode<Context>{tmp35}, IteratorBuiltinsFromDSLAssembler::IteratorRecord{compiler::TNode<JSReceiver>{tmp39}, compiler::TNode<Object>{tmp40}}, compiler::TNode<Object>{tmp46});
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Map> tmp51;
    ca_.Bind(&block3, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 694);
    ca_.Goto(&block2, tmp49, tmp50, tmp51);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<Map> tmp54;
    ca_.Bind(&block2, &tmp52, &tmp53, &tmp54);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 693);
    ca_.Goto(&block1, tmp52, tmp53, tmp54);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Map> tmp57;
    ca_.Bind(&block1, &tmp55, &tmp56, &tmp57);
    ca_.Goto(&block8, tmp55, tmp56, tmp57);
  }

    compiler::TNode<Context> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Map> tmp60;
    ca_.Bind(&block8, &tmp58, &tmp59, &tmp60);
}

void TestBuiltinsFromDSLAssembler::TestFrame1(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Object, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi, RawPtrT, RawPtrT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi, RawPtrT, RawPtrT, RawPtrT> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi, RawPtrT> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi, RawPtrT, RawPtrT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi, RawPtrT, RawPtrT, RawPtrT> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi, RawPtrT> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi, RawPtrT> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, Smi, RawPtrT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 712);
    compiler::TNode<RawPtrT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<RawPtrT>(CodeStubAssembler(state_).LoadFramePointer());
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 714);
    compiler::TNode<Object> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).LoadContextOrFrameTypeFromFrame(compiler::TNode<Context>{tmp0}, compiler::TNode<RawPtrT>{tmp1}));
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).Cast11ATFrameType(compiler::TNode<Object>{tmp2}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp4;
    compiler::TNode<RawPtrT> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block5, &tmp4, &tmp5, &tmp6);
    ca_.Goto(&block3, tmp4, tmp5);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<RawPtrT> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block4, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.Goto(&block2, tmp7, tmp8, tmp10);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<RawPtrT> tmp12;
    ca_.Bind(&block3, &tmp11, &tmp12);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../test/torque/test-torque.tq:714:60");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<Smi> tmp15;
    ca_.Bind(&block2, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 713);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 715);
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr11ATFrameType21ATconstexpr_FrameType(StackFrame::STUB));
    compiler::TNode<BoolT> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FrameTypeEquals(compiler::TNode<Smi>{tmp15}, compiler::TNode<Smi>{tmp16}));
    ca_.Branch(tmp17, &block6, &block7, tmp13, tmp14, tmp15);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<RawPtrT> tmp19;
    compiler::TNode<Smi> tmp20;
    ca_.Bind(&block7, &tmp18, &tmp19, &tmp20);
    CodeStubAssembler(state_).FailAssert("Torque assert \'frameType == STUB_FRAME\' failed", "../../test/torque/test-torque.tq", 715);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<Smi> tmp23;
    ca_.Bind(&block6, &tmp21, &tmp22, &tmp23);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 716);
    compiler::TNode<RawPtrT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<RawPtrT>(BaseBuiltinsFromDSLAssembler(state_).LoadCallerFromFrame(compiler::TNode<RawPtrT>{tmp22}));
    compiler::TNode<RawPtrT> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<RawPtrT>(CodeStubAssembler(state_).LoadParentFramePointer());
    compiler::TNode<BoolT> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<RawPtrT>{tmp24}, compiler::TNode<RawPtrT>{tmp25}));
    ca_.Branch(tmp26, &block8, &block9, tmp21, tmp22, tmp23);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<Smi> tmp29;
    ca_.Bind(&block9, &tmp27, &tmp28, &tmp29);
    CodeStubAssembler(state_).FailAssert("Torque assert \'f.caller == LoadParentFramePointer()\' failed", "../../test/torque/test-torque.tq", 716);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<RawPtrT> tmp31;
    compiler::TNode<Smi> tmp32;
    ca_.Bind(&block8, &tmp30, &tmp31, &tmp32);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 717);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 718);
    compiler::TNode<RawPtrT> tmp33;
    USE(tmp33);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp33 = BaseBuiltinsFromDSLAssembler(state_).Cast15ATStandardFrame(compiler::TNode<Context>{tmp30}, compiler::TNode<RawPtrT>{tmp31}, &label0);
    ca_.Goto(&block12, tmp30, tmp31, tmp32, tmp31, tmp31, tmp33);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block13, tmp30, tmp31, tmp32, tmp31, tmp31);
    }
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<RawPtrT> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<RawPtrT> tmp38;
    ca_.Bind(&block13, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.Goto(&block11, tmp34, tmp35, tmp36, tmp37);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<RawPtrT> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<RawPtrT> tmp42;
    compiler::TNode<RawPtrT> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    ca_.Bind(&block12, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 719);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../test/torque/test-torque.tq:719:9");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<RawPtrT> tmp46;
    compiler::TNode<Smi> tmp47;
    compiler::TNode<RawPtrT> tmp48;
    ca_.Bind(&block11, &tmp45, &tmp46, &tmp47, &tmp48);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 721);
    compiler::TNode<RawPtrT> tmp49;
    USE(tmp49);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp49 = BaseBuiltinsFromDSLAssembler(state_).Cast23ATArgumentsAdaptorFrame(compiler::TNode<Context>{tmp45}, compiler::TNode<RawPtrT>{ca_.UncheckedCast<RawPtrT>(tmp48)}, &label0);
    ca_.Goto(&block16, tmp45, tmp46, tmp47, tmp48, ca_.UncheckedCast<RawPtrT>(tmp48), tmp49);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block17, tmp45, tmp46, tmp47, tmp48, ca_.UncheckedCast<RawPtrT>(tmp48));
    }
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<Smi> tmp52;
    compiler::TNode<RawPtrT> tmp53;
    compiler::TNode<RawPtrT> tmp54;
    ca_.Bind(&block17, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54);
    ca_.Goto(&block15, tmp50, tmp51, tmp52, tmp53);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<RawPtrT> tmp56;
    compiler::TNode<Smi> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<RawPtrT> tmp60;
    ca_.Bind(&block16, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 722);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../test/torque/test-torque.tq:722:9");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<RawPtrT> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<RawPtrT> tmp64;
    ca_.Bind(&block15, &tmp61, &tmp62, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 724);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 721);
    ca_.Goto(&block14, tmp61, tmp62, tmp63, tmp64);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<RawPtrT> tmp66;
    compiler::TNode<Smi> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    ca_.Bind(&block14, &tmp65, &tmp66, &tmp67, &tmp68);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 718);
    ca_.Goto(&block10, tmp65, tmp66, tmp67, tmp68);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<RawPtrT> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<RawPtrT> tmp72;
    ca_.Bind(&block10, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 717);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 711);
    ca_.Goto(&block1, tmp69);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp73;
    ca_.Bind(&block1, &tmp73);
    ca_.Goto(&block18, tmp73);
  }

    compiler::TNode<Context> tmp74;
    ca_.Bind(&block18, &tmp74);
}

void TestBuiltinsFromDSLAssembler::TestNew(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 730);
    compiler::TNode<JSArray> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<JSArray>(BaseBuiltinsFromDSLAssembler(state_).NewJSArray(compiler::TNode<Context>{tmp0}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 731);
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_)._method_JSArray_IsEmpty(compiler::TNode<JSArray>{tmp1}));
    ca_.Branch(tmp2, &block2, &block3, tmp0, tmp1);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<JSArray> tmp4;
    ca_.Bind(&block3, &tmp3, &tmp4);
    CodeStubAssembler(state_).FailAssert("Torque assert \'f.IsEmpty()\' failed", "../../test/torque/test-torque.tq", 731);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<JSArray> tmp6;
    ca_.Bind(&block2, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 732);
    compiler::TNode<Number> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    BaseBuiltinsFromDSLAssembler(state_).StoreJSArrayLength(compiler::TNode<JSArray>{tmp6}, compiler::TNode<Number>{tmp7});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 729);
    ca_.Goto(&block1, tmp5);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp8;
    ca_.Bind(&block1, &tmp8);
    ca_.Goto(&block4, tmp8);
  }

    compiler::TNode<Context> tmp9;
    ca_.Bind(&block4, &tmp9);
}

void TestBuiltinsFromDSLAssembler::TestStructConstructor(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block11(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T, Int32T> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block14(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T, Int32T> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block16(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Int32T, Int32T, Int32T, Int32T> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 754);
    compiler::TNode<Int32T> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(6));
    compiler::TNode<Int32T> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(7));
    compiler::TNode<Int32T> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(5));
    compiler::TNode<Int32T> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(8));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 755);
    compiler::TNode<Int32T> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp3}, compiler::TNode<Int32T>{tmp5}));
    ca_.Branch(tmp6, &block2, &block3, tmp0, tmp3, tmp1, tmp2, tmp4);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Int32T> tmp8;
    compiler::TNode<Int32T> tmp9;
    compiler::TNode<Int32T> tmp10;
    compiler::TNode<Int32T> tmp11;
    ca_.Bind(&block3, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    CodeStubAssembler(state_).FailAssert("Torque assert \'a.a == 5\' failed", "../../test/torque/test-torque.tq", 755);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Int32T> tmp13;
    compiler::TNode<Int32T> tmp14;
    compiler::TNode<Int32T> tmp15;
    compiler::TNode<Int32T> tmp16;
    ca_.Bind(&block2, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 756);
    compiler::TNode<Int32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(6));
    compiler::TNode<BoolT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp14}, compiler::TNode<Int32T>{tmp17}));
    ca_.Branch(tmp18, &block4, &block5, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<Int32T> tmp20;
    compiler::TNode<Int32T> tmp21;
    compiler::TNode<Int32T> tmp22;
    compiler::TNode<Int32T> tmp23;
    ca_.Bind(&block5, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    CodeStubAssembler(state_).FailAssert("Torque assert \'a.b.x == 6\' failed", "../../test/torque/test-torque.tq", 756);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<Int32T> tmp25;
    compiler::TNode<Int32T> tmp26;
    compiler::TNode<Int32T> tmp27;
    compiler::TNode<Int32T> tmp28;
    ca_.Bind(&block4, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 757);
    compiler::TNode<Int32T> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(7));
    compiler::TNode<BoolT> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp27}, compiler::TNode<Int32T>{tmp29}));
    ca_.Branch(tmp30, &block6, &block7, tmp24, tmp25, tmp26, tmp27, tmp28);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<Int32T> tmp32;
    compiler::TNode<Int32T> tmp33;
    compiler::TNode<Int32T> tmp34;
    compiler::TNode<Int32T> tmp35;
    ca_.Bind(&block7, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    CodeStubAssembler(state_).FailAssert("Torque assert \'a.b.y == 7\' failed", "../../test/torque/test-torque.tq", 757);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<Int32T> tmp37;
    compiler::TNode<Int32T> tmp38;
    compiler::TNode<Int32T> tmp39;
    compiler::TNode<Int32T> tmp40;
    ca_.Bind(&block6, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 758);
    compiler::TNode<Int32T> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(8));
    compiler::TNode<BoolT> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp40}, compiler::TNode<Int32T>{tmp41}));
    ca_.Branch(tmp42, &block8, &block9, tmp36, tmp37, tmp38, tmp39, tmp40);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp43;
    compiler::TNode<Int32T> tmp44;
    compiler::TNode<Int32T> tmp45;
    compiler::TNode<Int32T> tmp46;
    compiler::TNode<Int32T> tmp47;
    ca_.Bind(&block9, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    CodeStubAssembler(state_).FailAssert("Torque assert \'a.c == 8\' failed", "../../test/torque/test-torque.tq", 758);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<Int32T> tmp49;
    compiler::TNode<Int32T> tmp50;
    compiler::TNode<Int32T> tmp51;
    compiler::TNode<Int32T> tmp52;
    ca_.Bind(&block8, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 759);
    compiler::TNode<Int32T> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(1));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 760);
    compiler::TNode<Int32T> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp53}, compiler::TNode<Int32T>{tmp54}));
    ca_.Branch(tmp55, &block10, &block11, tmp48, tmp49, tmp53, tmp51, tmp52);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Int32T> tmp57;
    compiler::TNode<Int32T> tmp58;
    compiler::TNode<Int32T> tmp59;
    compiler::TNode<Int32T> tmp60;
    ca_.Bind(&block11, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    CodeStubAssembler(state_).FailAssert("Torque assert \'a.b.x == 1\' failed", "../../test/torque/test-torque.tq", 760);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<Int32T> tmp62;
    compiler::TNode<Int32T> tmp63;
    compiler::TNode<Int32T> tmp64;
    compiler::TNode<Int32T> tmp65;
    ca_.Bind(&block10, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 761);
    compiler::TNode<Int32T> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(2));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 737);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 761);
    ca_.Goto(&block12, tmp61, tmp62, tmp66, tmp64, tmp65, tmp66);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<Int32T> tmp68;
    compiler::TNode<Int32T> tmp69;
    compiler::TNode<Int32T> tmp70;
    compiler::TNode<Int32T> tmp71;
    compiler::TNode<Int32T> tmp72;
    ca_.Bind(&block12, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 762);
    compiler::TNode<Int32T> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp69}, compiler::TNode<Int32T>{tmp73}));
    ca_.Branch(tmp74, &block13, &block14, tmp67, tmp68, tmp69, tmp70, tmp71);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp75;
    compiler::TNode<Int32T> tmp76;
    compiler::TNode<Int32T> tmp77;
    compiler::TNode<Int32T> tmp78;
    compiler::TNode<Int32T> tmp79;
    ca_.Bind(&block14, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79);
    CodeStubAssembler(state_).FailAssert("Torque assert \'a.b.x == 2\' failed", "../../test/torque/test-torque.tq", 762);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp80;
    compiler::TNode<Int32T> tmp81;
    compiler::TNode<Int32T> tmp82;
    compiler::TNode<Int32T> tmp83;
    compiler::TNode<Int32T> tmp84;
    ca_.Bind(&block13, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 740);
    ca_.Goto(&block17, tmp80, tmp81, tmp82, tmp83, tmp84, tmp82);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp85;
    compiler::TNode<Int32T> tmp86;
    compiler::TNode<Int32T> tmp87;
    compiler::TNode<Int32T> tmp88;
    compiler::TNode<Int32T> tmp89;
    compiler::TNode<Int32T> tmp90;
    ca_.Bind(&block17, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 763);
    compiler::TNode<Int32T> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Int32T>{tmp90}, compiler::TNode<Int32T>{tmp91}));
    ca_.Branch(tmp92, &block15, &block16, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp93;
    compiler::TNode<Int32T> tmp94;
    compiler::TNode<Int32T> tmp95;
    compiler::TNode<Int32T> tmp96;
    compiler::TNode<Int32T> tmp97;
    ca_.Bind(&block16, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    CodeStubAssembler(state_).FailAssert("Torque assert \'a.b.GetX() == 2\' failed", "../../test/torque/test-torque.tq", 763);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp98;
    compiler::TNode<Int32T> tmp99;
    compiler::TNode<Int32T> tmp100;
    compiler::TNode<Int32T> tmp101;
    compiler::TNode<Int32T> tmp102;
    ca_.Bind(&block15, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 752);
    ca_.Goto(&block1, tmp98);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp103;
    ca_.Bind(&block1, &tmp103);
    ca_.Goto(&block18, tmp103);
  }

    compiler::TNode<Context> tmp104;
    ca_.Bind(&block18, &tmp104);
}

void TestBuiltinsFromDSLAssembler::TestClassWithAllTypesLoadsAndStores(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_t, compiler::TNode<RawPtrT> p_r, compiler::TNode<Int32T> p_v1, compiler::TNode<Uint32T> p_v2, compiler::TNode<Int32T> p_v3, compiler::TNode<Uint32T> p_v4) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, RawPtrT, Int32T, Uint32T, Int32T, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, RawPtrT, Int32T, Uint32T, Int32T, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, RawPtrT, Int32T, Uint32T, Int32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_t, p_r, p_v1, p_v2, p_v3, p_v4);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<RawPtrT> tmp1;
    compiler::TNode<Int32T> tmp2;
    compiler::TNode<Uint32T> tmp3;
    compiler::TNode<Int32T> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 783);
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesA(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Int32T>{tmp2});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 784);
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesB(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Uint32T>{tmp3});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 785);
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesC(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Int32T>{tmp4});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 786);
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesD(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Uint32T>{tmp5});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 787);
    compiler::TNode<Int32T> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesE(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Int32T>{tmp6});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 788);
    compiler::TNode<Uint32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesF(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Uint32T>{tmp7});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 789);
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesG(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<RawPtrT>{tmp1});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 790);
    compiler::TNode<IntPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesH(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<IntPtrT>{tmp8});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 791);
    compiler::TNode<UintPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesI(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<UintPtrT>{tmp9});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 792);
    compiler::TNode<Int32T> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Int32T>(TestBuiltinsFromDSLAssembler(state_).LoadTestClassWithAllTypesA(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesA(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Int32T>{tmp10});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 793);
    compiler::TNode<Uint32T> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Uint32T>(TestBuiltinsFromDSLAssembler(state_).LoadTestClassWithAllTypesB(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesB(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Uint32T>{tmp11});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 794);
    compiler::TNode<Int32T> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Int32T>(TestBuiltinsFromDSLAssembler(state_).LoadTestClassWithAllTypesC(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesC(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Int32T>{tmp12});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 795);
    compiler::TNode<Uint32T> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Uint32T>(TestBuiltinsFromDSLAssembler(state_).LoadTestClassWithAllTypesD(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesD(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Uint32T>{tmp13});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 796);
    compiler::TNode<Int32T> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Int32T>(TestBuiltinsFromDSLAssembler(state_).LoadTestClassWithAllTypesE(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesE(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Int32T>{tmp14});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 797);
    compiler::TNode<Uint32T> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Uint32T>(TestBuiltinsFromDSLAssembler(state_).LoadTestClassWithAllTypesF(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesF(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<Uint32T>{tmp15});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 798);
    compiler::TNode<RawPtrT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<RawPtrT>(TestBuiltinsFromDSLAssembler(state_).LoadTestClassWithAllTypesG(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesG(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<RawPtrT>{tmp16});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 799);
    compiler::TNode<IntPtrT> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<IntPtrT>(TestBuiltinsFromDSLAssembler(state_).LoadTestClassWithAllTypesH(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesH(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<IntPtrT>{tmp17});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 800);
    compiler::TNode<UintPtrT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<UintPtrT>(TestBuiltinsFromDSLAssembler(state_).LoadTestClassWithAllTypesI(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}));
    TestBuiltinsFromDSLAssembler(state_).StoreTestClassWithAllTypesI(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes>{tmp0}, compiler::TNode<UintPtrT>{tmp18});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 780);
    ca_.Goto(&block1, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp19;
    compiler::TNode<RawPtrT> tmp20;
    compiler::TNode<Int32T> tmp21;
    compiler::TNode<Uint32T> tmp22;
    compiler::TNode<Int32T> tmp23;
    compiler::TNode<Uint32T> tmp24;
    ca_.Bind(&block1, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block2, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp25;
    compiler::TNode<RawPtrT> tmp26;
    compiler::TNode<Int32T> tmp27;
    compiler::TNode<Uint32T> tmp28;
    compiler::TNode<Int32T> tmp29;
    compiler::TNode<Uint32T> tmp30;
    ca_.Bind(&block2, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
}

compiler::TNode<FixedArray> TestBuiltinsFromDSLAssembler::NewInternalClass(compiler::TNode<Smi> p_x) {
  compiler::CodeAssemblerParameterizedLabel<Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, FixedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x);

  if (block0.is_used()) {
    compiler::TNode<Smi> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 814);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp0}, compiler::TNode<Smi>{tmp1}));
    compiler::TNode<FixedArray> tmp3;
    USE(tmp3);
    tmp3 = CodeStubAssembler(state_).AllocateUninitializedFixedArray(2);
    CodeStubAssembler(state_).UnsafeStoreFixedArrayElement(tmp3, 0, tmp0);
    CodeStubAssembler(state_).UnsafeStoreFixedArrayElement(tmp3, 1, tmp2);
    ca_.Goto(&block1, tmp0, tmp3);
  }

  if (block1.is_used()) {
    compiler::TNode<Smi> tmp4;
    compiler::TNode<FixedArray> tmp5;
    ca_.Bind(&block1, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 813);
    ca_.Goto(&block2, tmp4, tmp5);
  }

    compiler::TNode<Smi> tmp6;
    compiler::TNode<FixedArray> tmp7;
    ca_.Bind(&block2, &tmp6, &tmp7);
  return compiler::TNode<FixedArray>{tmp7};
}

void TestBuiltinsFromDSLAssembler::TestInternalClass(compiler::TNode<Context> p_context) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, FixedArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, FixedArray> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 818);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(5));
    compiler::TNode<FixedArray> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<FixedArray>(TestBuiltinsFromDSLAssembler(state_).NewInternalClass(compiler::TNode<Smi>{tmp1}));
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 819);
    compiler::CodeAssemblerLabel label0(&ca_);
    TestBuiltinsFromDSLAssembler(state_)._method_InternalClass_Flip(compiler::TNode<FixedArray>{tmp2}, &label0);
    ca_.Goto(&block4, tmp0, tmp2, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp2, tmp2);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<FixedArray> tmp4;
    compiler::TNode<FixedArray> tmp5;
    ca_.Bind(&block5, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block3, tmp3, tmp4);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<FixedArray> tmp7;
    compiler::TNode<FixedArray> tmp8;
    ca_.Bind(&block4, &tmp6, &tmp7, &tmp8);
    ca_.Goto(&block2, tmp6, tmp7);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<FixedArray> tmp10;
    ca_.Bind(&block3, &tmp9, &tmp10);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../test/torque/test-torque.tq:819:24");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<FixedArray> tmp12;
    ca_.Bind(&block2, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 820);
    compiler::TNode<Smi> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Smi>(TestBuiltinsFromDSLAssembler(state_).LoadInternalClassA(compiler::TNode<FixedArray>{tmp12}));
    compiler::TNode<Smi> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(6));
    compiler::TNode<BoolT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp13}, compiler::TNode<Smi>{tmp14}));
    ca_.Branch(tmp15, &block6, &block7, tmp11, tmp12);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<FixedArray> tmp17;
    ca_.Bind(&block7, &tmp16, &tmp17);
    CodeStubAssembler(state_).FailAssert("Torque assert \'o.a == 6\' failed", "../../test/torque/test-torque.tq", 820);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<FixedArray> tmp19;
    ca_.Bind(&block6, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 821);
    compiler::TNode<Number> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Number>(TestBuiltinsFromDSLAssembler(state_).LoadInternalClassB(compiler::TNode<FixedArray>{tmp19}));
    compiler::TNode<Number> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(5));
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).IsNumberEqual(compiler::TNode<Number>{tmp20}, compiler::TNode<Number>{tmp21}));
    ca_.Branch(tmp22, &block8, &block9, tmp18, tmp19);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<FixedArray> tmp24;
    ca_.Bind(&block9, &tmp23, &tmp24);
    CodeStubAssembler(state_).FailAssert("Torque assert \'o.b == 5\' failed", "../../test/torque/test-torque.tq", 821);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<FixedArray> tmp26;
    ca_.Bind(&block8, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 817);
    ca_.Goto(&block1, tmp25);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp27;
    ca_.Bind(&block1, &tmp27);
    ca_.Goto(&block10, tmp27);
  }

    compiler::TNode<Context> tmp28;
    ca_.Bind(&block10, &tmp28);
}

compiler::TNode<Int32T> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesA(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 767);
compiler::TNode<Int32T> tmp1 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kAOffset, MachineType::Int8()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Int32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Int32T>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesA(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<Int32T> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<Int32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 767);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 24, tmp1, MachineType::Int8().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Int32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<Uint32T> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesB(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 768);
compiler::TNode<Uint32T> tmp1 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kBOffset, MachineType::Uint8()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Uint32T>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesB(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<Uint32T> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<Uint32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 768);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 25, tmp1, MachineType::Uint8().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<Uint32T> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesB2(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 769);
compiler::TNode<Uint32T> tmp1 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kB2Offset, MachineType::Uint8()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Uint32T>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesB2(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<Uint32T> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<Uint32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 769);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 26, tmp1, MachineType::Uint8().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<Uint32T> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesB3(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 770);
compiler::TNode<Uint32T> tmp1 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kB3Offset, MachineType::Uint8()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Uint32T>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesB3(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<Uint32T> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<Uint32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 770);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 27, tmp1, MachineType::Uint8().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<Int32T> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesC(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 771);
compiler::TNode<Int32T> tmp1 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kCOffset, MachineType::Int16()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Int32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Int32T>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesC(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<Int32T> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<Int32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 771);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 28, tmp1, MachineType::Int16().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Int32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<Uint32T> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesD(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 772);
compiler::TNode<Uint32T> tmp1 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kDOffset, MachineType::Uint16()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Uint32T>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesD(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<Uint32T> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<Uint32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 772);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 30, tmp1, MachineType::Uint16().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<Int32T> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesE(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 773);
compiler::TNode<Int32T> tmp1 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kEOffset, MachineType::Int32()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Int32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Int32T>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesE(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<Int32T> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<Int32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 773);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 32, tmp1, MachineType::Int32().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Int32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<Uint32T> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesF(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 774);
compiler::TNode<Uint32T> tmp1 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kFOffset, MachineType::Uint32()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Uint32T>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesF(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<Uint32T> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<Uint32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 774);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 36, tmp1, MachineType::Uint32().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<RawPtrT> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesG(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, RawPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, RawPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 775);
compiler::TNode<RawPtrT> tmp1 = ca_.UncheckedCast<RawPtrT>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kGOffset, MachineType::Pointer()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<RawPtrT> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<RawPtrT>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesG(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<RawPtrT> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, RawPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, RawPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, RawPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<RawPtrT> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 775);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 40, tmp1, MachineType::Pointer().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<RawPtrT> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<IntPtrT> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesH(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 776);
compiler::TNode<IntPtrT> tmp1 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kHOffset, MachineType::IntPtr()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<IntPtrT> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<IntPtrT> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<IntPtrT>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesH(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<IntPtrT> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<IntPtrT> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 776);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 48, tmp1, MachineType::IntPtr().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<IntPtrT> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<IntPtrT> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<UintPtrT> TestBuiltinsFromDSLAssembler::LoadTestClassWithAllTypesI(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, UintPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, UintPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 777);
compiler::TNode<UintPtrT> tmp1 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).LoadObjectField(tmp0, TestBuiltinsFromDSLAssembler::TestClassWithAllTypes::kIOffset, MachineType::IntPtr()));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<UintPtrT> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<UintPtrT> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<UintPtrT>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreTestClassWithAllTypesI(compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> p_o, compiler::TNode<UintPtrT> p_v) {
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, UintPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, UintPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes, UintPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 777);
    CodeStubAssembler(state_).StoreObjectFieldNoWriteBarrier(tmp0, 56, tmp1, MachineType::IntPtr().representation());
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp2;
    compiler::TNode<UintPtrT> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<TestBuiltinsFromDSLAssembler::TestClassWithAllTypes> tmp4;
    compiler::TNode<UintPtrT> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<Smi> TestBuiltinsFromDSLAssembler::LoadInternalClassA(compiler::TNode<FixedArray> p_o) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 809);
compiler::TNode<Smi> tmp1 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).UnsafeLoadFixedArrayElement(tmp0, 0));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<FixedArray> tmp4;
    compiler::TNode<Smi> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Smi>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreInternalClassA(compiler::TNode<FixedArray> p_o, compiler::TNode<Smi> p_v) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<Smi> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 809);
    CodeStubAssembler(state_).UnsafeStoreFixedArrayElement(tmp0, 0, tmp1);
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<FixedArray> tmp4;
    compiler::TNode<Smi> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

compiler::TNode<Number> TestBuiltinsFromDSLAssembler::LoadInternalClassB(compiler::TNode<FixedArray> p_o) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 810);
compiler::TNode<Number> tmp1 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).UnsafeLoadFixedArrayElement(tmp0, 1));
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp2;
    compiler::TNode<Number> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<FixedArray> tmp4;
    compiler::TNode<Number> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Number>{tmp5};
}

void TestBuiltinsFromDSLAssembler::StoreInternalClassB(compiler::TNode<FixedArray> p_o, compiler::TNode<Number> p_v) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o, p_v);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<Number> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 810);
    CodeStubAssembler(state_).UnsafeStoreFixedArrayElement(tmp0, 1, tmp1);
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp2;
    compiler::TNode<Number> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<FixedArray> tmp4;
    compiler::TNode<Number> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
}

void TestBuiltinsFromDSLAssembler::_method_InternalClass_Flip(compiler::TNode<FixedArray> p_this, compiler::CodeAssemblerLabel* label_NotASmi) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Number, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_this);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 805);
    compiler::TNode<Number> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Number>(TestBuiltinsFromDSLAssembler(state_).LoadInternalClassB(compiler::TNode<FixedArray>{tmp0}));
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<FixedArray> tmp3;
    compiler::TNode<Number> tmp4;
    ca_.Bind(&block4, &tmp3, &tmp4);
    ca_.Goto(&block1);
  }

  if (block3.is_used()) {
    compiler::TNode<FixedArray> tmp5;
    compiler::TNode<Number> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 806);
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Smi>(TestBuiltinsFromDSLAssembler(state_).LoadInternalClassA(compiler::TNode<FixedArray>{tmp5}));
    TestBuiltinsFromDSLAssembler(state_).StoreInternalClassB(compiler::TNode<FixedArray>{tmp5}, compiler::TNode<Number>{tmp8});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 807);
    TestBuiltinsFromDSLAssembler(state_).StoreInternalClassA(compiler::TNode<FixedArray>{tmp5}, compiler::TNode<Smi>{tmp7});
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 804);
    ca_.Goto(&block2, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<FixedArray> tmp9;
    ca_.Bind(&block2, &tmp9);
    ca_.Goto(&block5, tmp9);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_NotASmi);
  }

    compiler::TNode<FixedArray> tmp10;
    ca_.Bind(&block5, &tmp10);
}

TF_BUILTIN(GenericBuiltinTest5ATSmi, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Smi> parameter1 = UncheckedCast<Smi>(Parameter(Descriptor::kParam));
  USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Smi> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 85);
    compiler::TNode<Oddball> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Null();
    CodeStubAssembler(state_).Return(tmp2);
  }
}

compiler::TNode<Object> TestBuiltinsFromDSLAssembler::GenericMacroTest5ATSmi(compiler::TNode<Smi> p_param) {
  compiler::CodeAssemblerParameterizedLabel<Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_param);

  if (block0.is_used()) {
    compiler::TNode<Smi> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 132);
    compiler::TNode<Oddball> tmp1;
    USE(tmp1);
    tmp1 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 131);
    ca_.Goto(&block2, tmp2, tmp3);
  }

    compiler::TNode<Smi> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
  return compiler::TNode<Object>{tmp5};
}

compiler::TNode<Object> TestBuiltinsFromDSLAssembler::GenericMacroTestWithLabels5ATSmi(compiler::TNode<Smi> p_param, compiler::CodeAssemblerLabel* label_X) {
  compiler::CodeAssemblerParameterizedLabel<Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_param);

  if (block0.is_used()) {
    compiler::TNode<Smi> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 141);
    compiler::TNode<Oddball> tmp1;
    USE(tmp1);
    tmp1 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block2, tmp0, tmp1);
  }

  if (block2.is_used()) {
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block2, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 139);
    ca_.Goto(&block3, tmp2, tmp3);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_X);
  }

    compiler::TNode<Smi> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block3, &tmp4, &tmp5);
  return compiler::TNode<Object>{tmp5};
}

compiler::TNode<Object> TestBuiltinsFromDSLAssembler::IncrementIfSmi32UT5ATSmi10FixedArray10HeapNumber(compiler::TNode<Object> p_x) {
  compiler::CodeAssemblerParameterizedLabel<Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object, Object, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_x);

  if (block0.is_used()) {
    compiler::TNode<Object> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 427);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 428);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp1 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp0}, &label0);
    ca_.Goto(&block4, tmp0, tmp0, tmp0, tmp1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp0, tmp0);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block5, &tmp2, &tmp3, &tmp4);
    ca_.Goto(&block3, tmp2, tmp3);
  }

  if (block4.is_used()) {
    compiler::TNode<Object> tmp5;
    compiler::TNode<Object> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<Smi> tmp8;
    ca_.Bind(&block4, &tmp5, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 429);
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp8}, compiler::TNode<Smi>{tmp9}));
    ca_.Goto(&block1, tmp5, tmp10);
  }

  if (block3.is_used()) {
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    ca_.Bind(&block3, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 431);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 432);
    ca_.Goto(&block1, tmp11, ca_.UncheckedCast<HeapObject>(tmp12));
  }

  if (block1.is_used()) {
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    ca_.Bind(&block1, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 426);
    ca_.Goto(&block6, tmp13, tmp14);
  }

    compiler::TNode<Object> tmp15;
    compiler::TNode<Object> tmp16;
    ca_.Bind(&block6, &tmp15, &tmp16);
  return compiler::TNode<Object>{tmp16};
}

compiler::TNode<Smi> TestBuiltinsFromDSLAssembler::ExampleGenericOverload5ATSmi(compiler::TNode<Smi> p_o) {
  compiler::CodeAssemblerParameterizedLabel<Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<Smi> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 491);
    compiler::TNode<Smi> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp0}, compiler::TNode<Smi>{tmp1}));
    ca_.Goto(&block1, tmp0, tmp2);
  }

  if (block1.is_used()) {
    compiler::TNode<Smi> tmp3;
    compiler::TNode<Smi> tmp4;
    ca_.Bind(&block1, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 490);
    ca_.Goto(&block2, tmp3, tmp4);
  }

    compiler::TNode<Smi> tmp5;
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block2, &tmp5, &tmp6);
  return compiler::TNode<Smi>{tmp6};
}

compiler::TNode<Object> TestBuiltinsFromDSLAssembler::ExampleGenericOverload20UT5ATSmi10HeapObject(compiler::TNode<Object> p_o) {
  compiler::CodeAssemblerParameterizedLabel<Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_o);

  if (block0.is_used()) {
    compiler::TNode<Object> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 488);
    ca_.Goto(&block1, tmp0, tmp0);
  }

  if (block1.is_used()) {
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    ca_.Bind(&block1, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../test/torque/test-torque.tq", 487);
    ca_.Goto(&block2, tmp1, tmp2);
  }

    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block2, &tmp3, &tmp4);
  return compiler::TNode<Object>{tmp4};
}

}  // namespace internal
}  // namespace v8

