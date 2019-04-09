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

compiler::TNode<BuiltinPtr> TypedArrayBuiltinsFromDSLAssembler::UnsafeCast54FT9ATContext12JSTypedArray5ATSmi20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, BuiltinPtr> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 71);
    compiler::TNode<BuiltinPtr> tmp2;
    USE(tmp2);
    tmp2 = TORQUE_CAST(compiler::TNode<Object>{tmp1});
    ca_.Goto(&block1, tmp0, tmp1, tmp2);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<BuiltinPtr> tmp5;
    ca_.Bind(&block1, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1680);
    ca_.Goto(&block2, tmp3, tmp4, tmp5);
  }

    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<BuiltinPtr> tmp8;
    ca_.Bind(&block2, &tmp6, &tmp7, &tmp8);
  return compiler::TNode<BuiltinPtr>{tmp8};
}

compiler::TNode<BuiltinPtr> TypedArrayBuiltinsFromDSLAssembler::UnsafeCast76FT9ATContext12JSTypedArray5ATSmi20UT5ATSmi10HeapObject20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, BuiltinPtr> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 74);
    compiler::TNode<BuiltinPtr> tmp2;
    USE(tmp2);
    tmp2 = TORQUE_CAST(compiler::TNode<Object>{tmp1});
    ca_.Goto(&block1, tmp0, tmp1, tmp2);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<BuiltinPtr> tmp5;
    ca_.Bind(&block1, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1680);
    ca_.Goto(&block2, tmp3, tmp4, tmp5);
  }

    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<BuiltinPtr> tmp8;
    ca_.Bind(&block2, &tmp6, &tmp7, &tmp8);
  return compiler::TNode<BuiltinPtr>{tmp8};
}

compiler::TNode<JSTypedArray> TypedArrayBuiltinsFromDSLAssembler::EnsureAttached(compiler::TNode<JSTypedArray> p_array, compiler::CodeAssemblerLabel* label_Detached) {
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_array);

  if (block0.is_used()) {
    compiler::TNode<JSTypedArray> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 82);
    compiler::TNode<JSArrayBuffer> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp0}));
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp1}));
    ca_.Branch(tmp2, &block3, &block4, tmp0);
  }

  if (block3.is_used()) {
    compiler::TNode<JSTypedArray> tmp3;
    ca_.Bind(&block3, &tmp3);
    ca_.Goto(&block1);
  }

  if (block4.is_used()) {
    compiler::TNode<JSTypedArray> tmp4;
    ca_.Bind(&block4, &tmp4);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 83);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    tmp5 = (compiler::TNode<JSTypedArray>{tmp4});
    ca_.Goto(&block2, tmp4, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<JSTypedArray> tmp6;
    compiler::TNode<JSTypedArray> tmp7;
    ca_.Bind(&block2, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 80);
    ca_.Goto(&block5, tmp6, tmp7);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Detached);
  }

    compiler::TNode<JSTypedArray> tmp8;
    compiler::TNode<JSTypedArray> tmp9;
    ca_.Bind(&block5, &tmp8, &tmp9);
  return compiler::TNode<JSTypedArray>{tmp9};
}

TypedArrayBuiltinsFromDSLAssembler::AttachedJSTypedArrayWitness TypedArrayBuiltinsFromDSLAssembler::NewAttachedJSTypedArrayWitness(compiler::TNode<JSTypedArray> p_array) {
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, JSTypedArray, BuiltinPtr> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, JSTypedArray, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_array);

  if (block0.is_used()) {
    compiler::TNode<JSTypedArray> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 112);
    compiler::TNode<Int32T> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadElementsKind(compiler::TNode<JSTypedArray>{tmp0}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 114);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 115);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 116);
    compiler::TNode<BuiltinPtr> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BuiltinPtr>(TypedArrayBuiltinsFromDSLAssembler(state_).GetLoadFnForElementsKind(compiler::TNode<Int32T>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 113);
    ca_.Goto(&block1, tmp0, tmp0, tmp0, tmp2);
  }

  if (block1.is_used()) {
    compiler::TNode<JSTypedArray> tmp3;
    compiler::TNode<JSTypedArray> tmp4;
    compiler::TNode<JSTypedArray> tmp5;
    compiler::TNode<BuiltinPtr> tmp6;
    ca_.Bind(&block1, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 110);
    ca_.Goto(&block2, tmp3, tmp4, tmp5, tmp6);
  }

    compiler::TNode<JSTypedArray> tmp7;
    compiler::TNode<JSTypedArray> tmp8;
    compiler::TNode<JSTypedArray> tmp9;
    compiler::TNode<BuiltinPtr> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
  return TypedArrayBuiltinsFromDSLAssembler::AttachedJSTypedArrayWitness{compiler::TNode<JSTypedArray>{tmp8}, compiler::TNode<JSTypedArray>{tmp9}, compiler::TNode<BuiltinPtr>{tmp10}};
}

compiler::TNode<BuiltinPtr> TypedArrayBuiltinsFromDSLAssembler::GetLoadFnForElementsKind(compiler::TNode<Int32T> p_elementsKind) {
  compiler::CodeAssemblerParameterizedLabel<Int32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T, BuiltinPtr> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Int32T, BuiltinPtr> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_elementsKind);

  if (block0.is_used()) {
    compiler::TNode<Int32T> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 121);
    compiler::TNode<BoolT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindGreaterThan(compiler::TNode<Int32T>{tmp0}, UINT32_ELEMENTS));
    ca_.Branch(tmp1, &block2, &block3, tmp0);
  }

  if (block2.is_used()) {
    compiler::TNode<Int32T> tmp2;
    ca_.Bind(&block2, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 122);
    compiler::TNode<Int32T> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT32_ELEMENTS));
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp2}, compiler::TNode<Int32T>{tmp3}));
    ca_.Branch(tmp4, &block5, &block6, tmp2);
  }

  if (block5.is_used()) {
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block5, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 123);
    ca_.Goto(&block1, tmp5, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedInt32Array)));
  }

  if (block6.is_used()) {
    compiler::TNode<Int32T> tmp6;
    ca_.Bind(&block6, &tmp6);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 124);
    compiler::TNode<Int32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT32_ELEMENTS));
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp6}, compiler::TNode<Int32T>{tmp7}));
    ca_.Branch(tmp8, &block8, &block9, tmp6);
  }

  if (block8.is_used()) {
    compiler::TNode<Int32T> tmp9;
    ca_.Bind(&block8, &tmp9);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 125);
    ca_.Goto(&block1, tmp9, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement19ATFixedFloat32Array)));
  }

  if (block9.is_used()) {
    compiler::TNode<Int32T> tmp10;
    ca_.Bind(&block9, &tmp10);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 126);
    compiler::TNode<Int32T> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT64_ELEMENTS));
    compiler::TNode<BoolT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp10}, compiler::TNode<Int32T>{tmp11}));
    ca_.Branch(tmp12, &block11, &block12, tmp10);
  }

  if (block11.is_used()) {
    compiler::TNode<Int32T> tmp13;
    ca_.Bind(&block11, &tmp13);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 127);
    ca_.Goto(&block1, tmp13, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement19ATFixedFloat64Array)));
  }

  if (block12.is_used()) {
    compiler::TNode<Int32T> tmp14;
    ca_.Bind(&block12, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 128);
    compiler::TNode<Int32T> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_CLAMPED_ELEMENTS));
    compiler::TNode<BoolT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp14}, compiler::TNode<Int32T>{tmp15}));
    ca_.Branch(tmp16, &block14, &block15, tmp14);
  }

  if (block14.is_used()) {
    compiler::TNode<Int32T> tmp17;
    ca_.Bind(&block14, &tmp17);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 129);
    ca_.Goto(&block1, tmp17, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement24ATFixedUint8ClampedArray)));
  }

  if (block15.is_used()) {
    compiler::TNode<Int32T> tmp18;
    ca_.Bind(&block15, &tmp18);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 130);
    compiler::TNode<Int32T> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGUINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp18}, compiler::TNode<Int32T>{tmp19}));
    ca_.Branch(tmp20, &block17, &block18, tmp18);
  }

  if (block17.is_used()) {
    compiler::TNode<Int32T> tmp21;
    ca_.Bind(&block17, &tmp21);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 131);
    ca_.Goto(&block1, tmp21, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement21ATFixedBigUint64Array)));
  }

  if (block18.is_used()) {
    compiler::TNode<Int32T> tmp22;
    ca_.Bind(&block18, &tmp22);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 132);
    compiler::TNode<Int32T> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp22}, compiler::TNode<Int32T>{tmp23}));
    ca_.Branch(tmp24, &block20, &block21, tmp22);
  }

  if (block20.is_used()) {
    compiler::TNode<Int32T> tmp25;
    ca_.Bind(&block20, &tmp25);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 133);
    ca_.Goto(&block1, tmp25, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement20ATFixedBigInt64Array)));
  }

  if (block21.is_used()) {
    compiler::TNode<Int32T> tmp26;
    ca_.Bind(&block21, &tmp26);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 135);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/typed-array.tq:135:9");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block3.is_used()) {
    compiler::TNode<Int32T> tmp27;
    ca_.Bind(&block3, &tmp27);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 138);
    compiler::TNode<Int32T> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_ELEMENTS));
    compiler::TNode<BoolT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp27}, compiler::TNode<Int32T>{tmp28}));
    ca_.Branch(tmp29, &block23, &block24, tmp27);
  }

  if (block23.is_used()) {
    compiler::TNode<Int32T> tmp30;
    ca_.Bind(&block23, &tmp30);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 139);
    ca_.Goto(&block1, tmp30, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedUint8Array)));
  }

  if (block24.is_used()) {
    compiler::TNode<Int32T> tmp31;
    ca_.Bind(&block24, &tmp31);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 140);
    compiler::TNode<Int32T> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT8_ELEMENTS));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp31}, compiler::TNode<Int32T>{tmp32}));
    ca_.Branch(tmp33, &block26, &block27, tmp31);
  }

  if (block26.is_used()) {
    compiler::TNode<Int32T> tmp34;
    ca_.Bind(&block26, &tmp34);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 141);
    ca_.Goto(&block1, tmp34, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement16ATFixedInt8Array)));
  }

  if (block27.is_used()) {
    compiler::TNode<Int32T> tmp35;
    ca_.Bind(&block27, &tmp35);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 142);
    compiler::TNode<Int32T> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT16_ELEMENTS));
    compiler::TNode<BoolT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp35}, compiler::TNode<Int32T>{tmp36}));
    ca_.Branch(tmp37, &block29, &block30, tmp35);
  }

  if (block29.is_used()) {
    compiler::TNode<Int32T> tmp38;
    ca_.Bind(&block29, &tmp38);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 143);
    ca_.Goto(&block1, tmp38, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement18ATFixedUint16Array)));
  }

  if (block30.is_used()) {
    compiler::TNode<Int32T> tmp39;
    ca_.Bind(&block30, &tmp39);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 144);
    compiler::TNode<Int32T> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT16_ELEMENTS));
    compiler::TNode<BoolT> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp39}, compiler::TNode<Int32T>{tmp40}));
    ca_.Branch(tmp41, &block32, &block33, tmp39);
  }

  if (block32.is_used()) {
    compiler::TNode<Int32T> tmp42;
    ca_.Bind(&block32, &tmp42);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 145);
    ca_.Goto(&block1, tmp42, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedInt16Array)));
  }

  if (block33.is_used()) {
    compiler::TNode<Int32T> tmp43;
    ca_.Bind(&block33, &tmp43);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 146);
    compiler::TNode<Int32T> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT32_ELEMENTS));
    compiler::TNode<BoolT> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp43}, compiler::TNode<Int32T>{tmp44}));
    ca_.Branch(tmp45, &block35, &block36, tmp43);
  }

  if (block35.is_used()) {
    compiler::TNode<Int32T> tmp46;
    ca_.Bind(&block35, &tmp46);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 147);
    ca_.Goto(&block1, tmp46, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement18ATFixedUint32Array)));
  }

  if (block36.is_used()) {
    compiler::TNode<Int32T> tmp47;
    ca_.Bind(&block36, &tmp47);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 149);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/typed-array.tq:149:9");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<Int32T> tmp48;
    compiler::TNode<BuiltinPtr> tmp49;
    ca_.Bind(&block1, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 120);
    ca_.Goto(&block38, tmp48, tmp49);
  }

    compiler::TNode<Int32T> tmp50;
    compiler::TNode<BuiltinPtr> tmp51;
    ca_.Bind(&block38, &tmp50, &tmp51);
  return compiler::TNode<BuiltinPtr>{tmp51};
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType17ATFixedUint8Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 156);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return UINT8_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType16ATFixedInt8Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 159);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return INT8_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType18ATFixedUint16Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 162);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return UINT16_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType17ATFixedInt16Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 165);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return INT16_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType18ATFixedUint32Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 168);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return UINT32_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType17ATFixedInt32Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 171);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return INT32_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType19ATFixedFloat32Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 174);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return FLOAT32_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType19ATFixedFloat64Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 177);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return FLOAT64_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType24ATFixedUint8ClampedArray() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 180);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return UINT8_CLAMPED_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType21ATFixedBigUint64Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 183);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return BIGUINT64_ELEMENTS;
}

ElementsKind TypedArrayBuiltinsFromDSLAssembler::KindForArrayType20ATFixedBigInt64Array() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 186);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 154);
    ca_.Goto(&block2);
  }

    ca_.Bind(&block2);
  return BIGINT64_ELEMENTS;
}

compiler::TNode<Number> TypedArrayBuiltinsFromDSLAssembler::CallCompare(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_array, compiler::TNode<JSReceiver> p_comparefn, compiler::TNode<Object> p_a, compiler::TNode<Object> p_b) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_array, p_comparefn, p_a, p_b);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 210);
    compiler::TNode<Oddball> tmp5;
    USE(tmp5);
    tmp5 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp0}, compiler::TNode<JSReceiver>{tmp2}, compiler::TNode<Object>{tmp5}, compiler::TNode<Object>{tmp3}, compiler::TNode<Object>{tmp4}));
    compiler::TNode<Number> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToNumber_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 209);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 213);
    compiler::TNode<JSArrayBuffer> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp1}));
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp8}));
    ca_.Branch(tmp9, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp7);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSTypedArray> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Number> tmp15;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 214);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp10}, MessageTemplate::kDetachedOperation, "%TypedArray%.prototype.sort");
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<JSTypedArray> tmp17;
    compiler::TNode<JSReceiver> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Number> tmp21;
    ca_.Bind(&block3, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 218);
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).NumberIsNaN(compiler::TNode<Number>{tmp21}));
    ca_.Branch(tmp22, &block4, &block5, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSTypedArray> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Number> tmp28;
    ca_.Bind(&block4, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    compiler::TNode<Number> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    ca_.Goto(&block1, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<JSTypedArray> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Number> tmp35;
    ca_.Bind(&block5, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 221);
    ca_.Goto(&block1, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<JSTypedArray> tmp37;
    compiler::TNode<JSReceiver> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Number> tmp41;
    ca_.Bind(&block1, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 205);
    ca_.Goto(&block6, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41);
  }

    compiler::TNode<Context> tmp42;
    compiler::TNode<JSTypedArray> tmp43;
    compiler::TNode<JSReceiver> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Number> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
  return compiler::TNode<Number>{tmp47};
}

void TypedArrayBuiltinsFromDSLAssembler::TypedArrayMerge(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_array, compiler::TNode<JSReceiver> p_comparefn, compiler::TNode<FixedArray> p_source, compiler::TNode<Smi> p_from, compiler::TNode<Smi> p_middle, compiler::TNode<Smi> p_to, compiler::TNode<FixedArray> p_target) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi, Object, Object, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi, Object, Object, Number> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi, Object, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi, Object, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi, Object, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block19(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_array, p_comparefn, p_source, p_from, p_middle, p_to, p_target);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<FixedArray> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Smi> tmp5;
    compiler::TNode<Smi> tmp6;
    compiler::TNode<FixedArray> tmp7;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 230);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 231);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 233);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp4, tmp5, tmp4);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<JSTypedArray> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<FixedArray> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<FixedArray> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Smi> tmp17;
    compiler::TNode<Smi> tmp18;
    ca_.Bind(&block4, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp18}, compiler::TNode<Smi>{tmp14}));
    ca_.Branch(tmp19, &block2, &block3, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<JSTypedArray> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<FixedArray> tmp23;
    compiler::TNode<Smi> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<FixedArray> tmp27;
    compiler::TNode<Smi> tmp28;
    compiler::TNode<Smi> tmp29;
    compiler::TNode<Smi> tmp30;
    ca_.Bind(&block2, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 234);
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp28}, compiler::TNode<Smi>{tmp25}));
    ca_.Branch(tmp31, &block8, &block7, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSTypedArray> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    compiler::TNode<FixedArray> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<Smi> tmp38;
    compiler::TNode<FixedArray> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<Smi> tmp42;
    ca_.Bind(&block8, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    compiler::TNode<BoolT> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThanOrEqual(compiler::TNode<Smi>{tmp41}, compiler::TNode<Smi>{tmp38}));
    ca_.Branch(tmp43, &block6, &block7, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<JSTypedArray> tmp45;
    compiler::TNode<JSReceiver> tmp46;
    compiler::TNode<FixedArray> tmp47;
    compiler::TNode<Smi> tmp48;
    compiler::TNode<Smi> tmp49;
    compiler::TNode<Smi> tmp50;
    compiler::TNode<FixedArray> tmp51;
    compiler::TNode<Smi> tmp52;
    compiler::TNode<Smi> tmp53;
    compiler::TNode<Smi> tmp54;
    ca_.Bind(&block6, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 237);
    compiler::TNode<Smi> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp52}, compiler::TNode<Smi>{tmp55}));
    compiler::TNode<Object> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp47}, compiler::TNode<Smi>{tmp52}));
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp51}, compiler::TNode<Smi>{tmp54}, compiler::TNode<Object>{tmp57});
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 234);
    ca_.Goto(&block9, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp56, tmp53, tmp54);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp58;
    compiler::TNode<JSTypedArray> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<FixedArray> tmp61;
    compiler::TNode<Smi> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<Smi> tmp64;
    compiler::TNode<FixedArray> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<Smi> tmp67;
    compiler::TNode<Smi> tmp68;
    ca_.Bind(&block7, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 238);
    compiler::TNode<BoolT> tmp69;
    USE(tmp69);
    tmp69 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp66}, compiler::TNode<Smi>{tmp63}));
    ca_.Branch(tmp69, &block10, &block11, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<JSTypedArray> tmp71;
    compiler::TNode<JSReceiver> tmp72;
    compiler::TNode<FixedArray> tmp73;
    compiler::TNode<Smi> tmp74;
    compiler::TNode<Smi> tmp75;
    compiler::TNode<Smi> tmp76;
    compiler::TNode<FixedArray> tmp77;
    compiler::TNode<Smi> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<Smi> tmp80;
    ca_.Bind(&block10, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 240);
    compiler::TNode<Object> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp73}, compiler::TNode<Smi>{tmp78}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 241);
    compiler::TNode<Object> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp73}, compiler::TNode<Smi>{tmp79}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 242);
    compiler::TNode<Number> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<Number>(TypedArrayBuiltinsFromDSLAssembler(state_).CallCompare(compiler::TNode<Context>{tmp70}, compiler::TNode<JSTypedArray>{tmp71}, compiler::TNode<JSReceiver>{tmp72}, compiler::TNode<Object>{tmp81}, compiler::TNode<Object>{tmp82}));
    compiler::TNode<Number> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThanOrEqual(compiler::TNode<Number>{tmp83}, compiler::TNode<Number>{tmp84}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block15, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block16, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83);
    }
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp85;
    compiler::TNode<JSTypedArray> tmp86;
    compiler::TNode<JSReceiver> tmp87;
    compiler::TNode<FixedArray> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<Smi> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<FixedArray> tmp92;
    compiler::TNode<Smi> tmp93;
    compiler::TNode<Smi> tmp94;
    compiler::TNode<Smi> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<Number> tmp98;
    ca_.Bind(&block15, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98);
    ca_.Goto(&block13, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp99;
    compiler::TNode<JSTypedArray> tmp100;
    compiler::TNode<JSReceiver> tmp101;
    compiler::TNode<FixedArray> tmp102;
    compiler::TNode<Smi> tmp103;
    compiler::TNode<Smi> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<FixedArray> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<Smi> tmp108;
    compiler::TNode<Smi> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Number> tmp112;
    ca_.Bind(&block16, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112);
    ca_.Goto(&block14, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp113;
    compiler::TNode<JSTypedArray> tmp114;
    compiler::TNode<JSReceiver> tmp115;
    compiler::TNode<FixedArray> tmp116;
    compiler::TNode<Smi> tmp117;
    compiler::TNode<Smi> tmp118;
    compiler::TNode<Smi> tmp119;
    compiler::TNode<FixedArray> tmp120;
    compiler::TNode<Smi> tmp121;
    compiler::TNode<Smi> tmp122;
    compiler::TNode<Smi> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Object> tmp125;
    ca_.Bind(&block13, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 243);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp120}, compiler::TNode<Smi>{tmp123}, compiler::TNode<Object>{tmp124});
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 244);
    compiler::TNode<Smi> tmp126;
    USE(tmp126);
    tmp126 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp121}, compiler::TNode<Smi>{tmp126}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 242);
    ca_.Goto(&block17, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp127, tmp122, tmp123, tmp124, tmp125);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp128;
    compiler::TNode<JSTypedArray> tmp129;
    compiler::TNode<JSReceiver> tmp130;
    compiler::TNode<FixedArray> tmp131;
    compiler::TNode<Smi> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<Smi> tmp134;
    compiler::TNode<FixedArray> tmp135;
    compiler::TNode<Smi> tmp136;
    compiler::TNode<Smi> tmp137;
    compiler::TNode<Smi> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<Object> tmp140;
    ca_.Bind(&block14, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 246);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp135}, compiler::TNode<Smi>{tmp138}, compiler::TNode<Object>{tmp140});
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 247);
    compiler::TNode<Smi> tmp141;
    USE(tmp141);
    tmp141 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp142;
    USE(tmp142);
    tmp142 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp137}, compiler::TNode<Smi>{tmp141}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 242);
    ca_.Goto(&block17, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp142, tmp138, tmp139, tmp140);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp143;
    compiler::TNode<JSTypedArray> tmp144;
    compiler::TNode<JSReceiver> tmp145;
    compiler::TNode<FixedArray> tmp146;
    compiler::TNode<Smi> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<Smi> tmp149;
    compiler::TNode<FixedArray> tmp150;
    compiler::TNode<Smi> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<Object> tmp155;
    ca_.Bind(&block17, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 238);
    ca_.Goto(&block12, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp156;
    compiler::TNode<JSTypedArray> tmp157;
    compiler::TNode<JSReceiver> tmp158;
    compiler::TNode<FixedArray> tmp159;
    compiler::TNode<Smi> tmp160;
    compiler::TNode<Smi> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<FixedArray> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<Smi> tmp166;
    ca_.Bind(&block11, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 252);
    compiler::TNode<BoolT> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp164}, compiler::TNode<Smi>{tmp161}));
    ca_.Branch(tmp167, &block18, &block19, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp168;
    compiler::TNode<JSTypedArray> tmp169;
    compiler::TNode<JSReceiver> tmp170;
    compiler::TNode<FixedArray> tmp171;
    compiler::TNode<Smi> tmp172;
    compiler::TNode<Smi> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<FixedArray> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<Smi> tmp178;
    ca_.Bind(&block19, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178);
    CodeStubAssembler(state_).FailAssert("Torque assert \'left == middle\' failed", "../../src/builtins/typed-array.tq", 252);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp179;
    compiler::TNode<JSTypedArray> tmp180;
    compiler::TNode<JSReceiver> tmp181;
    compiler::TNode<FixedArray> tmp182;
    compiler::TNode<Smi> tmp183;
    compiler::TNode<Smi> tmp184;
    compiler::TNode<Smi> tmp185;
    compiler::TNode<FixedArray> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<Smi> tmp188;
    compiler::TNode<Smi> tmp189;
    ca_.Bind(&block18, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 253);
    compiler::TNode<Smi> tmp190;
    USE(tmp190);
    tmp190 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp191;
    USE(tmp191);
    tmp191 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp188}, compiler::TNode<Smi>{tmp190}));
    compiler::TNode<Object> tmp192;
    USE(tmp192);
    tmp192 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp182}, compiler::TNode<Smi>{tmp188}));
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp186}, compiler::TNode<Smi>{tmp189}, compiler::TNode<Object>{tmp192});
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 238);
    ca_.Goto(&block12, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp191, tmp189);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp193;
    compiler::TNode<JSTypedArray> tmp194;
    compiler::TNode<JSReceiver> tmp195;
    compiler::TNode<FixedArray> tmp196;
    compiler::TNode<Smi> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<Smi> tmp199;
    compiler::TNode<FixedArray> tmp200;
    compiler::TNode<Smi> tmp201;
    compiler::TNode<Smi> tmp202;
    compiler::TNode<Smi> tmp203;
    ca_.Bind(&block12, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 234);
    ca_.Goto(&block9, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp204;
    compiler::TNode<JSTypedArray> tmp205;
    compiler::TNode<JSReceiver> tmp206;
    compiler::TNode<FixedArray> tmp207;
    compiler::TNode<Smi> tmp208;
    compiler::TNode<Smi> tmp209;
    compiler::TNode<Smi> tmp210;
    compiler::TNode<FixedArray> tmp211;
    compiler::TNode<Smi> tmp212;
    compiler::TNode<Smi> tmp213;
    compiler::TNode<Smi> tmp214;
    ca_.Bind(&block9, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 233);
    ca_.Goto(&block5, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp215;
    compiler::TNode<JSTypedArray> tmp216;
    compiler::TNode<JSReceiver> tmp217;
    compiler::TNode<FixedArray> tmp218;
    compiler::TNode<Smi> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<Smi> tmp221;
    compiler::TNode<FixedArray> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<Smi> tmp224;
    compiler::TNode<Smi> tmp225;
    ca_.Bind(&block5, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225);
    compiler::TNode<Smi> tmp226;
    USE(tmp226);
    tmp226 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp227;
    USE(tmp227);
    tmp227 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp225}, compiler::TNode<Smi>{tmp226}));
    ca_.Goto(&block4, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp227);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp228;
    compiler::TNode<JSTypedArray> tmp229;
    compiler::TNode<JSReceiver> tmp230;
    compiler::TNode<FixedArray> tmp231;
    compiler::TNode<Smi> tmp232;
    compiler::TNode<Smi> tmp233;
    compiler::TNode<Smi> tmp234;
    compiler::TNode<FixedArray> tmp235;
    compiler::TNode<Smi> tmp236;
    compiler::TNode<Smi> tmp237;
    compiler::TNode<Smi> tmp238;
    ca_.Bind(&block3, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 229);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 226);
    ca_.Goto(&block1, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp239;
    compiler::TNode<JSTypedArray> tmp240;
    compiler::TNode<JSReceiver> tmp241;
    compiler::TNode<FixedArray> tmp242;
    compiler::TNode<Smi> tmp243;
    compiler::TNode<Smi> tmp244;
    compiler::TNode<Smi> tmp245;
    compiler::TNode<FixedArray> tmp246;
    ca_.Bind(&block1, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246);
    ca_.Goto(&block20, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246);
  }

    compiler::TNode<Context> tmp247;
    compiler::TNode<JSTypedArray> tmp248;
    compiler::TNode<JSReceiver> tmp249;
    compiler::TNode<FixedArray> tmp250;
    compiler::TNode<Smi> tmp251;
    compiler::TNode<Smi> tmp252;
    compiler::TNode<Smi> tmp253;
    compiler::TNode<FixedArray> tmp254;
    ca_.Bind(&block20, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254);
}

TF_BUILTIN(TypedArrayMergeSort, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<JSReceiver> parameter2 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kComparefn));
  USE(parameter2);
  compiler::TNode<FixedArray> parameter3 = UncheckedCast<FixedArray>(Parameter(Descriptor::kSource));
  USE(parameter3);
  compiler::TNode<Smi> parameter4 = UncheckedCast<Smi>(Parameter(Descriptor::kFrom));
  USE(parameter4);
  compiler::TNode<Smi> parameter5 = UncheckedCast<Smi>(Parameter(Descriptor::kTo));
  USE(parameter5);
  compiler::TNode<FixedArray> parameter6 = UncheckedCast<FixedArray>(Parameter(Descriptor::kTarget));
  USE(parameter6);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3, parameter4, parameter5, parameter6);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<FixedArray> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Smi> tmp5;
    compiler::TNode<FixedArray> tmp6;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 262);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp5}, compiler::TNode<Smi>{tmp4}));
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp7}, compiler::TNode<Smi>{tmp8}));
    ca_.Branch(tmp9, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSTypedArray> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<FixedArray> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Smi> tmp15;
    compiler::TNode<FixedArray> tmp16;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    CodeStubAssembler(state_).FailAssert("Torque assert \'to - from > 1\' failed", "../../src/builtins/typed-array.tq", 262);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSTypedArray> tmp18;
    compiler::TNode<JSReceiver> tmp19;
    compiler::TNode<FixedArray> tmp20;
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Smi> tmp22;
    compiler::TNode<FixedArray> tmp23;
    ca_.Bind(&block1, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 263);
    compiler::TNode<Smi> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp22}, compiler::TNode<Smi>{tmp21}));
    compiler::TNode<Smi> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSar(compiler::TNode<Smi>{tmp24}, 1));
    compiler::TNode<Smi> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp21}, compiler::TNode<Smi>{tmp25}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 268);
    compiler::TNode<Smi> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp26}, compiler::TNode<Smi>{tmp21}));
    compiler::TNode<Smi> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp28}));
    ca_.Branch(tmp29, &block3, &block4, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp26);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<JSTypedArray> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<FixedArray> tmp33;
    compiler::TNode<Smi> tmp34;
    compiler::TNode<Smi> tmp35;
    compiler::TNode<FixedArray> tmp36;
    compiler::TNode<Smi> tmp37;
    ca_.Bind(&block3, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    compiler::TNode<Object> tmp38;
    tmp38 = CodeStubAssembler(state_).CallBuiltin(Builtins::kTypedArrayMergeSort, tmp30, tmp31, tmp32, tmp36, tmp34, tmp37, tmp33);
    USE(tmp38);
    ca_.Goto(&block4, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<JSTypedArray> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    compiler::TNode<FixedArray> tmp42;
    compiler::TNode<Smi> tmp43;
    compiler::TNode<Smi> tmp44;
    compiler::TNode<FixedArray> tmp45;
    compiler::TNode<Smi> tmp46;
    ca_.Bind(&block4, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 269);
    compiler::TNode<Smi> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp44}, compiler::TNode<Smi>{tmp46}));
    compiler::TNode<Smi> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp47}, compiler::TNode<Smi>{tmp48}));
    ca_.Branch(tmp49, &block5, &block6, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<JSTypedArray> tmp51;
    compiler::TNode<JSReceiver> tmp52;
    compiler::TNode<FixedArray> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<Smi> tmp55;
    compiler::TNode<FixedArray> tmp56;
    compiler::TNode<Smi> tmp57;
    ca_.Bind(&block5, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57);
    compiler::TNode<Object> tmp58;
    tmp58 = CodeStubAssembler(state_).CallBuiltin(Builtins::kTypedArrayMergeSort, tmp50, tmp51, tmp52, tmp56, tmp57, tmp55, tmp53);
    USE(tmp58);
    ca_.Goto(&block6, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<JSTypedArray> tmp60;
    compiler::TNode<JSReceiver> tmp61;
    compiler::TNode<FixedArray> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<Smi> tmp64;
    compiler::TNode<FixedArray> tmp65;
    compiler::TNode<Smi> tmp66;
    ca_.Bind(&block6, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 271);
    TypedArrayBuiltinsFromDSLAssembler(state_).TypedArrayMerge(compiler::TNode<Context>{tmp59}, compiler::TNode<JSTypedArray>{tmp60}, compiler::TNode<JSReceiver>{tmp61}, compiler::TNode<FixedArray>{tmp62}, compiler::TNode<Smi>{tmp63}, compiler::TNode<Smi>{tmp66}, compiler::TNode<Smi>{tmp64}, compiler::TNode<FixedArray>{tmp65});
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 273);
    compiler::TNode<Oddball> tmp67;
    USE(tmp67);
    tmp67 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp67);
  }
}

TF_BUILTIN(TypedArrayPrototypeSort, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, Object, JSReceiver> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block41(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block43(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block44(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block46(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block47(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block49(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block50(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block51(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block48(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block45(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block54(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block52(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block55(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block53(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block58(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block56(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block59(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block57(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 282);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(arguments));
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp2}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp4, &block1, &block2, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block1, &tmp5, &tmp6);
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp7}));
    ca_.Goto(&block4, tmp5, tmp6, tmp8);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<Object> tmp10;
    ca_.Bind(&block2, &tmp9, &tmp10);
    compiler::TNode<Oddball> tmp11;
    USE(tmp11);
    tmp11 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp9, tmp10, tmp11);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    ca_.Bind(&block4, &tmp12, &tmp13, &tmp14);
    ca_.Goto(&block3, tmp12, tmp13, tmp14);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Object> tmp17;
    ca_.Bind(&block3, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 281);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 283);
    compiler::TNode<Oddball> tmp18;
    USE(tmp18);
    tmp18 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp17}, compiler::TNode<HeapObject>{tmp18}));
    ca_.Branch(tmp19, &block7, &block6, tmp15, tmp16, tmp17);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Object> tmp22;
    ca_.Bind(&block7, &tmp20, &tmp21, &tmp22);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).TaggedIsCallable(compiler::TNode<Object>{tmp22}));
    compiler::TNode<BoolT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp23}));
    ca_.Branch(tmp24, &block5, &block6, tmp20, tmp21, tmp22);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    ca_.Bind(&block5, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 284);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp25}, MessageTemplate::kBadSortComparisonFunction, compiler::TNode<Object>{tmp27});
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Object> tmp30;
    ca_.Bind(&block6, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 288);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 293);
    compiler::TNode<JSTypedArray> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<JSTypedArray>(TypedArrayBuiltinsAssembler(state_).ValidateTypedArray(compiler::TNode<Context>{tmp28}, compiler::TNode<Object>{tmp29}, "%TypedArray%.prototype.sort"));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 292);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 296);
    compiler::TNode<Oddball> tmp32;
    USE(tmp32);
    tmp32 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp30}, compiler::TNode<HeapObject>{tmp32}));
    ca_.Branch(tmp33, &block8, &block9, tmp28, tmp29, tmp30, tmp29, tmp31);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<JSTypedArray> tmp38;
    ca_.Bind(&block8, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 297);
    compiler::TNode<JSTypedArray> tmp39;
    tmp39 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kTypedArraySortFast, tmp34, tmp37));
    USE(tmp39);
    arguments->PopAndReturn(tmp39);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<JSTypedArray> tmp44;
    ca_.Bind(&block9, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 301);
    compiler::TNode<Smi> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadJSTypedArrayLength(compiler::TNode<JSTypedArray>{tmp44}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 304);
    compiler::TNode<Smi> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp45}, compiler::TNode<Smi>{tmp46}));
    ca_.Branch(tmp47, &block10, &block11, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<JSTypedArray> tmp52;
    compiler::TNode<Smi> tmp53;
    ca_.Bind(&block10, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    arguments->PopAndReturn(tmp52);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<JSTypedArray> tmp58;
    compiler::TNode<Smi> tmp59;
    ca_.Bind(&block11, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 307);
    compiler::TNode<JSReceiver> tmp60;
    USE(tmp60);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp60 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp54}, compiler::TNode<Object>{tmp56}, &label0);
    ca_.Goto(&block14, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp56, tmp60);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block15, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp56);
    }
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<JSTypedArray> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<Object> tmp67;
    ca_.Bind(&block15, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.Goto(&block13, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<JSTypedArray> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<JSReceiver> tmp75;
    ca_.Bind(&block14, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    ca_.Goto(&block12, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp75);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<JSTypedArray> tmp80;
    compiler::TNode<Smi> tmp81;
    ca_.Bind(&block13, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/typed-array.tq:307:48");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<JSTypedArray> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<JSReceiver> tmp88;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 306);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 308);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 309);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 311);
    compiler::TNode<Int32T> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadElementsKind(compiler::TNode<JSTypedArray>{tmp86}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 313);
    compiler::TNode<BoolT> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindGreaterThan(compiler::TNode<Int32T>{tmp89}, UINT32_ELEMENTS));
    ca_.Branch(tmp90, &block16, &block17, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, ca_.Uninitialized<BuiltinPtr>(), ca_.Uninitialized<BuiltinPtr>(), tmp89);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<JSTypedArray> tmp95;
    compiler::TNode<Smi> tmp96;
    compiler::TNode<JSReceiver> tmp97;
    compiler::TNode<BuiltinPtr> tmp98;
    compiler::TNode<BuiltinPtr> tmp99;
    compiler::TNode<Int32T> tmp100;
    ca_.Bind(&block16, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 314);
    compiler::TNode<Int32T> tmp101;
    USE(tmp101);
    tmp101 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT32_ELEMENTS));
    compiler::TNode<BoolT> tmp102;
    USE(tmp102);
    tmp102 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp100}, compiler::TNode<Int32T>{tmp101}));
    ca_.Branch(tmp102, &block19, &block20, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<JSTypedArray> tmp107;
    compiler::TNode<Smi> tmp108;
    compiler::TNode<JSReceiver> tmp109;
    compiler::TNode<BuiltinPtr> tmp110;
    compiler::TNode<BuiltinPtr> tmp111;
    compiler::TNode<Int32T> tmp112;
    ca_.Bind(&block19, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 315);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 316);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 314);
    ca_.Goto(&block21, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedInt32Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement17ATFixedInt32Array)), tmp112);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<Object> tmp115;
    compiler::TNode<Object> tmp116;
    compiler::TNode<JSTypedArray> tmp117;
    compiler::TNode<Smi> tmp118;
    compiler::TNode<JSReceiver> tmp119;
    compiler::TNode<BuiltinPtr> tmp120;
    compiler::TNode<BuiltinPtr> tmp121;
    compiler::TNode<Int32T> tmp122;
    ca_.Bind(&block20, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 317);
    compiler::TNode<Int32T> tmp123;
    USE(tmp123);
    tmp123 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT32_ELEMENTS));
    compiler::TNode<BoolT> tmp124;
    USE(tmp124);
    tmp124 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp122}, compiler::TNode<Int32T>{tmp123}));
    ca_.Branch(tmp124, &block22, &block23, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp125;
    compiler::TNode<Object> tmp126;
    compiler::TNode<Object> tmp127;
    compiler::TNode<Object> tmp128;
    compiler::TNode<JSTypedArray> tmp129;
    compiler::TNode<Smi> tmp130;
    compiler::TNode<JSReceiver> tmp131;
    compiler::TNode<BuiltinPtr> tmp132;
    compiler::TNode<BuiltinPtr> tmp133;
    compiler::TNode<Int32T> tmp134;
    ca_.Bind(&block22, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 318);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 319);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 317);
    ca_.Goto(&block24, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement19ATFixedFloat32Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement19ATFixedFloat32Array)), tmp134);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<JSTypedArray> tmp139;
    compiler::TNode<Smi> tmp140;
    compiler::TNode<JSReceiver> tmp141;
    compiler::TNode<BuiltinPtr> tmp142;
    compiler::TNode<BuiltinPtr> tmp143;
    compiler::TNode<Int32T> tmp144;
    ca_.Bind(&block23, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 320);
    compiler::TNode<Int32T> tmp145;
    USE(tmp145);
    tmp145 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT64_ELEMENTS));
    compiler::TNode<BoolT> tmp146;
    USE(tmp146);
    tmp146 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp144}, compiler::TNode<Int32T>{tmp145}));
    ca_.Branch(tmp146, &block25, &block26, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<Object> tmp149;
    compiler::TNode<Object> tmp150;
    compiler::TNode<JSTypedArray> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<JSReceiver> tmp153;
    compiler::TNode<BuiltinPtr> tmp154;
    compiler::TNode<BuiltinPtr> tmp155;
    compiler::TNode<Int32T> tmp156;
    ca_.Bind(&block25, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 321);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 322);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 320);
    ca_.Goto(&block27, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement19ATFixedFloat64Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement19ATFixedFloat64Array)), tmp156);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp157;
    compiler::TNode<Object> tmp158;
    compiler::TNode<Object> tmp159;
    compiler::TNode<Object> tmp160;
    compiler::TNode<JSTypedArray> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<JSReceiver> tmp163;
    compiler::TNode<BuiltinPtr> tmp164;
    compiler::TNode<BuiltinPtr> tmp165;
    compiler::TNode<Int32T> tmp166;
    ca_.Bind(&block26, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 323);
    compiler::TNode<Int32T> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_CLAMPED_ELEMENTS));
    compiler::TNode<BoolT> tmp168;
    USE(tmp168);
    tmp168 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp166}, compiler::TNode<Int32T>{tmp167}));
    ca_.Branch(tmp168, &block28, &block29, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp169;
    compiler::TNode<Object> tmp170;
    compiler::TNode<Object> tmp171;
    compiler::TNode<Object> tmp172;
    compiler::TNode<JSTypedArray> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<JSReceiver> tmp175;
    compiler::TNode<BuiltinPtr> tmp176;
    compiler::TNode<BuiltinPtr> tmp177;
    compiler::TNode<Int32T> tmp178;
    ca_.Bind(&block28, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 324);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 325);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 323);
    ca_.Goto(&block30, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement24ATFixedUint8ClampedArray)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement24ATFixedUint8ClampedArray)), tmp178);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp179;
    compiler::TNode<Object> tmp180;
    compiler::TNode<Object> tmp181;
    compiler::TNode<Object> tmp182;
    compiler::TNode<JSTypedArray> tmp183;
    compiler::TNode<Smi> tmp184;
    compiler::TNode<JSReceiver> tmp185;
    compiler::TNode<BuiltinPtr> tmp186;
    compiler::TNode<BuiltinPtr> tmp187;
    compiler::TNode<Int32T> tmp188;
    ca_.Bind(&block29, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 326);
    compiler::TNode<Int32T> tmp189;
    USE(tmp189);
    tmp189 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGUINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp190;
    USE(tmp190);
    tmp190 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp188}, compiler::TNode<Int32T>{tmp189}));
    ca_.Branch(tmp190, &block31, &block32, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp191;
    compiler::TNode<Object> tmp192;
    compiler::TNode<Object> tmp193;
    compiler::TNode<Object> tmp194;
    compiler::TNode<JSTypedArray> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<JSReceiver> tmp197;
    compiler::TNode<BuiltinPtr> tmp198;
    compiler::TNode<BuiltinPtr> tmp199;
    compiler::TNode<Int32T> tmp200;
    ca_.Bind(&block31, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 327);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 328);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 326);
    ca_.Goto(&block33, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement21ATFixedBigUint64Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement21ATFixedBigUint64Array)), tmp200);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp201;
    compiler::TNode<Object> tmp202;
    compiler::TNode<Object> tmp203;
    compiler::TNode<Object> tmp204;
    compiler::TNode<JSTypedArray> tmp205;
    compiler::TNode<Smi> tmp206;
    compiler::TNode<JSReceiver> tmp207;
    compiler::TNode<BuiltinPtr> tmp208;
    compiler::TNode<BuiltinPtr> tmp209;
    compiler::TNode<Int32T> tmp210;
    ca_.Bind(&block32, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 329);
    compiler::TNode<Int32T> tmp211;
    USE(tmp211);
    tmp211 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp212;
    USE(tmp212);
    tmp212 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp210}, compiler::TNode<Int32T>{tmp211}));
    ca_.Branch(tmp212, &block34, &block35, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp213;
    compiler::TNode<Object> tmp214;
    compiler::TNode<Object> tmp215;
    compiler::TNode<Object> tmp216;
    compiler::TNode<JSTypedArray> tmp217;
    compiler::TNode<Smi> tmp218;
    compiler::TNode<JSReceiver> tmp219;
    compiler::TNode<BuiltinPtr> tmp220;
    compiler::TNode<BuiltinPtr> tmp221;
    compiler::TNode<Int32T> tmp222;
    ca_.Bind(&block34, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 330);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 331);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 329);
    ca_.Goto(&block36, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement20ATFixedBigInt64Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement20ATFixedBigInt64Array)), tmp222);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp223;
    compiler::TNode<Object> tmp224;
    compiler::TNode<Object> tmp225;
    compiler::TNode<Object> tmp226;
    compiler::TNode<JSTypedArray> tmp227;
    compiler::TNode<Smi> tmp228;
    compiler::TNode<JSReceiver> tmp229;
    compiler::TNode<BuiltinPtr> tmp230;
    compiler::TNode<BuiltinPtr> tmp231;
    compiler::TNode<Int32T> tmp232;
    ca_.Bind(&block35, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 333);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/typed-array.tq:333:9");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp233;
    compiler::TNode<Object> tmp234;
    compiler::TNode<Object> tmp235;
    compiler::TNode<Object> tmp236;
    compiler::TNode<JSTypedArray> tmp237;
    compiler::TNode<Smi> tmp238;
    compiler::TNode<JSReceiver> tmp239;
    compiler::TNode<BuiltinPtr> tmp240;
    compiler::TNode<BuiltinPtr> tmp241;
    compiler::TNode<Int32T> tmp242;
    ca_.Bind(&block36, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 326);
    ca_.Goto(&block33, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp243;
    compiler::TNode<Object> tmp244;
    compiler::TNode<Object> tmp245;
    compiler::TNode<Object> tmp246;
    compiler::TNode<JSTypedArray> tmp247;
    compiler::TNode<Smi> tmp248;
    compiler::TNode<JSReceiver> tmp249;
    compiler::TNode<BuiltinPtr> tmp250;
    compiler::TNode<BuiltinPtr> tmp251;
    compiler::TNode<Int32T> tmp252;
    ca_.Bind(&block33, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 323);
    ca_.Goto(&block30, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp253;
    compiler::TNode<Object> tmp254;
    compiler::TNode<Object> tmp255;
    compiler::TNode<Object> tmp256;
    compiler::TNode<JSTypedArray> tmp257;
    compiler::TNode<Smi> tmp258;
    compiler::TNode<JSReceiver> tmp259;
    compiler::TNode<BuiltinPtr> tmp260;
    compiler::TNode<BuiltinPtr> tmp261;
    compiler::TNode<Int32T> tmp262;
    ca_.Bind(&block30, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 320);
    ca_.Goto(&block27, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp263;
    compiler::TNode<Object> tmp264;
    compiler::TNode<Object> tmp265;
    compiler::TNode<Object> tmp266;
    compiler::TNode<JSTypedArray> tmp267;
    compiler::TNode<Smi> tmp268;
    compiler::TNode<JSReceiver> tmp269;
    compiler::TNode<BuiltinPtr> tmp270;
    compiler::TNode<BuiltinPtr> tmp271;
    compiler::TNode<Int32T> tmp272;
    ca_.Bind(&block27, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 317);
    ca_.Goto(&block24, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp273;
    compiler::TNode<Object> tmp274;
    compiler::TNode<Object> tmp275;
    compiler::TNode<Object> tmp276;
    compiler::TNode<JSTypedArray> tmp277;
    compiler::TNode<Smi> tmp278;
    compiler::TNode<JSReceiver> tmp279;
    compiler::TNode<BuiltinPtr> tmp280;
    compiler::TNode<BuiltinPtr> tmp281;
    compiler::TNode<Int32T> tmp282;
    ca_.Bind(&block24, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 314);
    ca_.Goto(&block21, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp283;
    compiler::TNode<Object> tmp284;
    compiler::TNode<Object> tmp285;
    compiler::TNode<Object> tmp286;
    compiler::TNode<JSTypedArray> tmp287;
    compiler::TNode<Smi> tmp288;
    compiler::TNode<JSReceiver> tmp289;
    compiler::TNode<BuiltinPtr> tmp290;
    compiler::TNode<BuiltinPtr> tmp291;
    compiler::TNode<Int32T> tmp292;
    ca_.Bind(&block21, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 313);
    ca_.Goto(&block18, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp291, tmp292);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp293;
    compiler::TNode<Object> tmp294;
    compiler::TNode<Object> tmp295;
    compiler::TNode<Object> tmp296;
    compiler::TNode<JSTypedArray> tmp297;
    compiler::TNode<Smi> tmp298;
    compiler::TNode<JSReceiver> tmp299;
    compiler::TNode<BuiltinPtr> tmp300;
    compiler::TNode<BuiltinPtr> tmp301;
    compiler::TNode<Int32T> tmp302;
    ca_.Bind(&block17, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 336);
    compiler::TNode<Int32T> tmp303;
    USE(tmp303);
    tmp303 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_ELEMENTS));
    compiler::TNode<BoolT> tmp304;
    USE(tmp304);
    tmp304 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp302}, compiler::TNode<Int32T>{tmp303}));
    ca_.Branch(tmp304, &block37, &block38, tmp293, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp305;
    compiler::TNode<Object> tmp306;
    compiler::TNode<Object> tmp307;
    compiler::TNode<Object> tmp308;
    compiler::TNode<JSTypedArray> tmp309;
    compiler::TNode<Smi> tmp310;
    compiler::TNode<JSReceiver> tmp311;
    compiler::TNode<BuiltinPtr> tmp312;
    compiler::TNode<BuiltinPtr> tmp313;
    compiler::TNode<Int32T> tmp314;
    ca_.Bind(&block37, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 337);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 338);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 336);
    ca_.Goto(&block39, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedUint8Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement17ATFixedUint8Array)), tmp314);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp315;
    compiler::TNode<Object> tmp316;
    compiler::TNode<Object> tmp317;
    compiler::TNode<Object> tmp318;
    compiler::TNode<JSTypedArray> tmp319;
    compiler::TNode<Smi> tmp320;
    compiler::TNode<JSReceiver> tmp321;
    compiler::TNode<BuiltinPtr> tmp322;
    compiler::TNode<BuiltinPtr> tmp323;
    compiler::TNode<Int32T> tmp324;
    ca_.Bind(&block38, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 339);
    compiler::TNode<Int32T> tmp325;
    USE(tmp325);
    tmp325 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT8_ELEMENTS));
    compiler::TNode<BoolT> tmp326;
    USE(tmp326);
    tmp326 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp324}, compiler::TNode<Int32T>{tmp325}));
    ca_.Branch(tmp326, &block40, &block41, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp327;
    compiler::TNode<Object> tmp328;
    compiler::TNode<Object> tmp329;
    compiler::TNode<Object> tmp330;
    compiler::TNode<JSTypedArray> tmp331;
    compiler::TNode<Smi> tmp332;
    compiler::TNode<JSReceiver> tmp333;
    compiler::TNode<BuiltinPtr> tmp334;
    compiler::TNode<BuiltinPtr> tmp335;
    compiler::TNode<Int32T> tmp336;
    ca_.Bind(&block40, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 340);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 341);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 339);
    ca_.Goto(&block42, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp333, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement16ATFixedInt8Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement16ATFixedInt8Array)), tmp336);
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp337;
    compiler::TNode<Object> tmp338;
    compiler::TNode<Object> tmp339;
    compiler::TNode<Object> tmp340;
    compiler::TNode<JSTypedArray> tmp341;
    compiler::TNode<Smi> tmp342;
    compiler::TNode<JSReceiver> tmp343;
    compiler::TNode<BuiltinPtr> tmp344;
    compiler::TNode<BuiltinPtr> tmp345;
    compiler::TNode<Int32T> tmp346;
    ca_.Bind(&block41, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 342);
    compiler::TNode<Int32T> tmp347;
    USE(tmp347);
    tmp347 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT16_ELEMENTS));
    compiler::TNode<BoolT> tmp348;
    USE(tmp348);
    tmp348 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp346}, compiler::TNode<Int32T>{tmp347}));
    ca_.Branch(tmp348, &block43, &block44, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346);
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp349;
    compiler::TNode<Object> tmp350;
    compiler::TNode<Object> tmp351;
    compiler::TNode<Object> tmp352;
    compiler::TNode<JSTypedArray> tmp353;
    compiler::TNode<Smi> tmp354;
    compiler::TNode<JSReceiver> tmp355;
    compiler::TNode<BuiltinPtr> tmp356;
    compiler::TNode<BuiltinPtr> tmp357;
    compiler::TNode<Int32T> tmp358;
    ca_.Bind(&block43, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 343);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 344);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 342);
    ca_.Goto(&block45, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement18ATFixedUint16Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement18ATFixedUint16Array)), tmp358);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp359;
    compiler::TNode<Object> tmp360;
    compiler::TNode<Object> tmp361;
    compiler::TNode<Object> tmp362;
    compiler::TNode<JSTypedArray> tmp363;
    compiler::TNode<Smi> tmp364;
    compiler::TNode<JSReceiver> tmp365;
    compiler::TNode<BuiltinPtr> tmp366;
    compiler::TNode<BuiltinPtr> tmp367;
    compiler::TNode<Int32T> tmp368;
    ca_.Bind(&block44, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 345);
    compiler::TNode<Int32T> tmp369;
    USE(tmp369);
    tmp369 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT16_ELEMENTS));
    compiler::TNode<BoolT> tmp370;
    USE(tmp370);
    tmp370 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp368}, compiler::TNode<Int32T>{tmp369}));
    ca_.Branch(tmp370, &block46, &block47, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368);
  }

  if (block46.is_used()) {
    compiler::TNode<Context> tmp371;
    compiler::TNode<Object> tmp372;
    compiler::TNode<Object> tmp373;
    compiler::TNode<Object> tmp374;
    compiler::TNode<JSTypedArray> tmp375;
    compiler::TNode<Smi> tmp376;
    compiler::TNode<JSReceiver> tmp377;
    compiler::TNode<BuiltinPtr> tmp378;
    compiler::TNode<BuiltinPtr> tmp379;
    compiler::TNode<Int32T> tmp380;
    ca_.Bind(&block46, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 346);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 347);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 345);
    ca_.Goto(&block48, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedInt16Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement17ATFixedInt16Array)), tmp380);
  }

  if (block47.is_used()) {
    compiler::TNode<Context> tmp381;
    compiler::TNode<Object> tmp382;
    compiler::TNode<Object> tmp383;
    compiler::TNode<Object> tmp384;
    compiler::TNode<JSTypedArray> tmp385;
    compiler::TNode<Smi> tmp386;
    compiler::TNode<JSReceiver> tmp387;
    compiler::TNode<BuiltinPtr> tmp388;
    compiler::TNode<BuiltinPtr> tmp389;
    compiler::TNode<Int32T> tmp390;
    ca_.Bind(&block47, &tmp381, &tmp382, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 348);
    compiler::TNode<Int32T> tmp391;
    USE(tmp391);
    tmp391 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT32_ELEMENTS));
    compiler::TNode<BoolT> tmp392;
    USE(tmp392);
    tmp392 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp390}, compiler::TNode<Int32T>{tmp391}));
    ca_.Branch(tmp392, &block49, &block50, tmp381, tmp382, tmp383, tmp384, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390);
  }

  if (block49.is_used()) {
    compiler::TNode<Context> tmp393;
    compiler::TNode<Object> tmp394;
    compiler::TNode<Object> tmp395;
    compiler::TNode<Object> tmp396;
    compiler::TNode<JSTypedArray> tmp397;
    compiler::TNode<Smi> tmp398;
    compiler::TNode<JSReceiver> tmp399;
    compiler::TNode<BuiltinPtr> tmp400;
    compiler::TNode<BuiltinPtr> tmp401;
    compiler::TNode<Int32T> tmp402;
    ca_.Bind(&block49, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 349);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 350);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 348);
    ca_.Goto(&block51, tmp393, tmp394, tmp395, tmp396, tmp397, tmp398, tmp399, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement18ATFixedUint32Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement18ATFixedUint32Array)), tmp402);
  }

  if (block50.is_used()) {
    compiler::TNode<Context> tmp403;
    compiler::TNode<Object> tmp404;
    compiler::TNode<Object> tmp405;
    compiler::TNode<Object> tmp406;
    compiler::TNode<JSTypedArray> tmp407;
    compiler::TNode<Smi> tmp408;
    compiler::TNode<JSReceiver> tmp409;
    compiler::TNode<BuiltinPtr> tmp410;
    compiler::TNode<BuiltinPtr> tmp411;
    compiler::TNode<Int32T> tmp412;
    ca_.Bind(&block50, &tmp403, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 352);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/typed-array.tq:352:9");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block51.is_used()) {
    compiler::TNode<Context> tmp413;
    compiler::TNode<Object> tmp414;
    compiler::TNode<Object> tmp415;
    compiler::TNode<Object> tmp416;
    compiler::TNode<JSTypedArray> tmp417;
    compiler::TNode<Smi> tmp418;
    compiler::TNode<JSReceiver> tmp419;
    compiler::TNode<BuiltinPtr> tmp420;
    compiler::TNode<BuiltinPtr> tmp421;
    compiler::TNode<Int32T> tmp422;
    ca_.Bind(&block51, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420, &tmp421, &tmp422);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 345);
    ca_.Goto(&block48, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420, tmp421, tmp422);
  }

  if (block48.is_used()) {
    compiler::TNode<Context> tmp423;
    compiler::TNode<Object> tmp424;
    compiler::TNode<Object> tmp425;
    compiler::TNode<Object> tmp426;
    compiler::TNode<JSTypedArray> tmp427;
    compiler::TNode<Smi> tmp428;
    compiler::TNode<JSReceiver> tmp429;
    compiler::TNode<BuiltinPtr> tmp430;
    compiler::TNode<BuiltinPtr> tmp431;
    compiler::TNode<Int32T> tmp432;
    ca_.Bind(&block48, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 342);
    ca_.Goto(&block45, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432);
  }

  if (block45.is_used()) {
    compiler::TNode<Context> tmp433;
    compiler::TNode<Object> tmp434;
    compiler::TNode<Object> tmp435;
    compiler::TNode<Object> tmp436;
    compiler::TNode<JSTypedArray> tmp437;
    compiler::TNode<Smi> tmp438;
    compiler::TNode<JSReceiver> tmp439;
    compiler::TNode<BuiltinPtr> tmp440;
    compiler::TNode<BuiltinPtr> tmp441;
    compiler::TNode<Int32T> tmp442;
    ca_.Bind(&block45, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 339);
    ca_.Goto(&block42, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438, tmp439, tmp440, tmp441, tmp442);
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp443;
    compiler::TNode<Object> tmp444;
    compiler::TNode<Object> tmp445;
    compiler::TNode<Object> tmp446;
    compiler::TNode<JSTypedArray> tmp447;
    compiler::TNode<Smi> tmp448;
    compiler::TNode<JSReceiver> tmp449;
    compiler::TNode<BuiltinPtr> tmp450;
    compiler::TNode<BuiltinPtr> tmp451;
    compiler::TNode<Int32T> tmp452;
    ca_.Bind(&block42, &tmp443, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 336);
    ca_.Goto(&block39, tmp443, tmp444, tmp445, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp453;
    compiler::TNode<Object> tmp454;
    compiler::TNode<Object> tmp455;
    compiler::TNode<Object> tmp456;
    compiler::TNode<JSTypedArray> tmp457;
    compiler::TNode<Smi> tmp458;
    compiler::TNode<JSReceiver> tmp459;
    compiler::TNode<BuiltinPtr> tmp460;
    compiler::TNode<BuiltinPtr> tmp461;
    compiler::TNode<Int32T> tmp462;
    ca_.Bind(&block39, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 313);
    ca_.Goto(&block18, tmp453, tmp454, tmp455, tmp456, tmp457, tmp458, tmp459, tmp460, tmp461, tmp462);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp463;
    compiler::TNode<Object> tmp464;
    compiler::TNode<Object> tmp465;
    compiler::TNode<Object> tmp466;
    compiler::TNode<JSTypedArray> tmp467;
    compiler::TNode<Smi> tmp468;
    compiler::TNode<JSReceiver> tmp469;
    compiler::TNode<BuiltinPtr> tmp470;
    compiler::TNode<BuiltinPtr> tmp471;
    compiler::TNode<Int32T> tmp472;
    ca_.Bind(&block18, &tmp463, &tmp464, &tmp465, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471, &tmp472);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 359);
    compiler::TNode<IntPtrT> tmp473;
    USE(tmp473);
    tmp473 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp468}));
    compiler::TNode<FixedArray> tmp474;
    USE(tmp474);
    tmp474 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateZeroedFixedArray(compiler::TNode<IntPtrT>{tmp473}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 360);
    compiler::TNode<IntPtrT> tmp475;
    USE(tmp475);
    tmp475 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp468}));
    compiler::TNode<FixedArray> tmp476;
    USE(tmp476);
    tmp476 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateZeroedFixedArray(compiler::TNode<IntPtrT>{tmp475}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 362);
    compiler::TNode<Smi> tmp477;
    USE(tmp477);
    tmp477 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block54, tmp463, tmp464, tmp465, tmp466, tmp467, tmp468, tmp469, tmp470, tmp471, tmp472, tmp474, tmp476, tmp477);
  }

  if (block54.is_used()) {
    compiler::TNode<Context> tmp478;
    compiler::TNode<Object> tmp479;
    compiler::TNode<Object> tmp480;
    compiler::TNode<Object> tmp481;
    compiler::TNode<JSTypedArray> tmp482;
    compiler::TNode<Smi> tmp483;
    compiler::TNode<JSReceiver> tmp484;
    compiler::TNode<BuiltinPtr> tmp485;
    compiler::TNode<BuiltinPtr> tmp486;
    compiler::TNode<Int32T> tmp487;
    compiler::TNode<FixedArray> tmp488;
    compiler::TNode<FixedArray> tmp489;
    compiler::TNode<Smi> tmp490;
    ca_.Bind(&block54, &tmp478, &tmp479, &tmp480, &tmp481, &tmp482, &tmp483, &tmp484, &tmp485, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490);
    compiler::TNode<BoolT> tmp491;
    USE(tmp491);
    tmp491 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp490}, compiler::TNode<Smi>{tmp483}));
    ca_.Branch(tmp491, &block52, &block53, tmp478, tmp479, tmp480, tmp481, tmp482, tmp483, tmp484, tmp485, tmp486, tmp487, tmp488, tmp489, tmp490);
  }

  if (block52.is_used()) {
    compiler::TNode<Context> tmp492;
    compiler::TNode<Object> tmp493;
    compiler::TNode<Object> tmp494;
    compiler::TNode<Object> tmp495;
    compiler::TNode<JSTypedArray> tmp496;
    compiler::TNode<Smi> tmp497;
    compiler::TNode<JSReceiver> tmp498;
    compiler::TNode<BuiltinPtr> tmp499;
    compiler::TNode<BuiltinPtr> tmp500;
    compiler::TNode<Int32T> tmp501;
    compiler::TNode<FixedArray> tmp502;
    compiler::TNode<FixedArray> tmp503;
    compiler::TNode<Smi> tmp504;
    ca_.Bind(&block52, &tmp492, &tmp493, &tmp494, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501, &tmp502, &tmp503, &tmp504);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 363);
    compiler::TNode<Object> tmp505 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(1)).descriptor(), tmp499, tmp492, tmp496, tmp504); 
    USE(tmp505);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 364);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp502}, compiler::TNode<Smi>{tmp504}, compiler::TNode<Object>{tmp505});
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 365);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp503}, compiler::TNode<Smi>{tmp504}, compiler::TNode<Object>{tmp505});
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 362);
    ca_.Goto(&block55, tmp492, tmp493, tmp494, tmp495, tmp496, tmp497, tmp498, tmp499, tmp500, tmp501, tmp502, tmp503, tmp504);
  }

  if (block55.is_used()) {
    compiler::TNode<Context> tmp506;
    compiler::TNode<Object> tmp507;
    compiler::TNode<Object> tmp508;
    compiler::TNode<Object> tmp509;
    compiler::TNode<JSTypedArray> tmp510;
    compiler::TNode<Smi> tmp511;
    compiler::TNode<JSReceiver> tmp512;
    compiler::TNode<BuiltinPtr> tmp513;
    compiler::TNode<BuiltinPtr> tmp514;
    compiler::TNode<Int32T> tmp515;
    compiler::TNode<FixedArray> tmp516;
    compiler::TNode<FixedArray> tmp517;
    compiler::TNode<Smi> tmp518;
    ca_.Bind(&block55, &tmp506, &tmp507, &tmp508, &tmp509, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518);
    compiler::TNode<Smi> tmp519;
    USE(tmp519);
    tmp519 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp520;
    USE(tmp520);
    tmp520 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp518}, compiler::TNode<Smi>{tmp519}));
    ca_.Goto(&block54, tmp506, tmp507, tmp508, tmp509, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517, tmp520);
  }

  if (block53.is_used()) {
    compiler::TNode<Context> tmp521;
    compiler::TNode<Object> tmp522;
    compiler::TNode<Object> tmp523;
    compiler::TNode<Object> tmp524;
    compiler::TNode<JSTypedArray> tmp525;
    compiler::TNode<Smi> tmp526;
    compiler::TNode<JSReceiver> tmp527;
    compiler::TNode<BuiltinPtr> tmp528;
    compiler::TNode<BuiltinPtr> tmp529;
    compiler::TNode<Int32T> tmp530;
    compiler::TNode<FixedArray> tmp531;
    compiler::TNode<FixedArray> tmp532;
    compiler::TNode<Smi> tmp533;
    ca_.Bind(&block53, &tmp521, &tmp522, &tmp523, &tmp524, &tmp525, &tmp526, &tmp527, &tmp528, &tmp529, &tmp530, &tmp531, &tmp532, &tmp533);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 368);
    compiler::TNode<Smi> tmp534;
    USE(tmp534);
    tmp534 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp535;
    tmp535 = CodeStubAssembler(state_).CallBuiltin(Builtins::kTypedArrayMergeSort, tmp521, tmp525, tmp527, tmp532, tmp534, tmp526, tmp531);
    USE(tmp535);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 371);
    compiler::TNode<Smi> tmp536;
    USE(tmp536);
    tmp536 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block58, tmp521, tmp522, tmp523, tmp524, tmp525, tmp526, tmp527, tmp528, tmp529, tmp530, tmp531, tmp532, tmp536);
  }

  if (block58.is_used()) {
    compiler::TNode<Context> tmp537;
    compiler::TNode<Object> tmp538;
    compiler::TNode<Object> tmp539;
    compiler::TNode<Object> tmp540;
    compiler::TNode<JSTypedArray> tmp541;
    compiler::TNode<Smi> tmp542;
    compiler::TNode<JSReceiver> tmp543;
    compiler::TNode<BuiltinPtr> tmp544;
    compiler::TNode<BuiltinPtr> tmp545;
    compiler::TNode<Int32T> tmp546;
    compiler::TNode<FixedArray> tmp547;
    compiler::TNode<FixedArray> tmp548;
    compiler::TNode<Smi> tmp549;
    ca_.Bind(&block58, &tmp537, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544, &tmp545, &tmp546, &tmp547, &tmp548, &tmp549);
    compiler::TNode<BoolT> tmp550;
    USE(tmp550);
    tmp550 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp549}, compiler::TNode<Smi>{tmp542}));
    ca_.Branch(tmp550, &block56, &block57, tmp537, tmp538, tmp539, tmp540, tmp541, tmp542, tmp543, tmp544, tmp545, tmp546, tmp547, tmp548, tmp549);
  }

  if (block56.is_used()) {
    compiler::TNode<Context> tmp551;
    compiler::TNode<Object> tmp552;
    compiler::TNode<Object> tmp553;
    compiler::TNode<Object> tmp554;
    compiler::TNode<JSTypedArray> tmp555;
    compiler::TNode<Smi> tmp556;
    compiler::TNode<JSReceiver> tmp557;
    compiler::TNode<BuiltinPtr> tmp558;
    compiler::TNode<BuiltinPtr> tmp559;
    compiler::TNode<Int32T> tmp560;
    compiler::TNode<FixedArray> tmp561;
    compiler::TNode<FixedArray> tmp562;
    compiler::TNode<Smi> tmp563;
    ca_.Bind(&block56, &tmp551, &tmp552, &tmp553, &tmp554, &tmp555, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 372);
    compiler::TNode<Object> tmp564;
    USE(tmp564);
    tmp564 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp561}, compiler::TNode<Smi>{tmp563}));
    compiler::TNode<Object> tmp565 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(2)).descriptor(), tmp559, tmp551, tmp555, tmp563, tmp564); 
    USE(tmp565);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 371);
    ca_.Goto(&block59, tmp551, tmp552, tmp553, tmp554, tmp555, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562, tmp563);
  }

  if (block59.is_used()) {
    compiler::TNode<Context> tmp566;
    compiler::TNode<Object> tmp567;
    compiler::TNode<Object> tmp568;
    compiler::TNode<Object> tmp569;
    compiler::TNode<JSTypedArray> tmp570;
    compiler::TNode<Smi> tmp571;
    compiler::TNode<JSReceiver> tmp572;
    compiler::TNode<BuiltinPtr> tmp573;
    compiler::TNode<BuiltinPtr> tmp574;
    compiler::TNode<Int32T> tmp575;
    compiler::TNode<FixedArray> tmp576;
    compiler::TNode<FixedArray> tmp577;
    compiler::TNode<Smi> tmp578;
    ca_.Bind(&block59, &tmp566, &tmp567, &tmp568, &tmp569, &tmp570, &tmp571, &tmp572, &tmp573, &tmp574, &tmp575, &tmp576, &tmp577, &tmp578);
    compiler::TNode<Smi> tmp579;
    USE(tmp579);
    tmp579 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp580;
    USE(tmp580);
    tmp580 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp578}, compiler::TNode<Smi>{tmp579}));
    ca_.Goto(&block58, tmp566, tmp567, tmp568, tmp569, tmp570, tmp571, tmp572, tmp573, tmp574, tmp575, tmp576, tmp577, tmp580);
  }

  if (block57.is_used()) {
    compiler::TNode<Context> tmp581;
    compiler::TNode<Object> tmp582;
    compiler::TNode<Object> tmp583;
    compiler::TNode<Object> tmp584;
    compiler::TNode<JSTypedArray> tmp585;
    compiler::TNode<Smi> tmp586;
    compiler::TNode<JSReceiver> tmp587;
    compiler::TNode<BuiltinPtr> tmp588;
    compiler::TNode<BuiltinPtr> tmp589;
    compiler::TNode<Int32T> tmp590;
    compiler::TNode<FixedArray> tmp591;
    compiler::TNode<FixedArray> tmp592;
    compiler::TNode<Smi> tmp593;
    ca_.Bind(&block57, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585, &tmp586, &tmp587, &tmp588, &tmp589, &tmp590, &tmp591, &tmp592, &tmp593);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 374);
    arguments->PopAndReturn(tmp585);
  }
}

TF_BUILTIN(LoadFixedElement17ATFixedInt32Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt32Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement19ATFixedFloat32Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat32Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement19ATFixedFloat64Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat64Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement24ATFixedUint8ClampedArray, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType24ATFixedUint8ClampedArray()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement21ATFixedBigUint64Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType21ATFixedBigUint64Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement20ATFixedBigInt64Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType20ATFixedBigInt64Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement17ATFixedUint8Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedUint8Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement16ATFixedInt8Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType16ATFixedInt8Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement18ATFixedUint16Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint16Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement17ATFixedInt16Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt16Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(LoadFixedElement18ATFixedUint32Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 191);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<Smi>{tmp2}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint32Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp4);
  }
}

TF_BUILTIN(StoreFixedElement17ATFixedInt32Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt32Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement19ATFixedFloat32Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat32Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement19ATFixedFloat64Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat64Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement24ATFixedUint8ClampedArray, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType24ATFixedUint8ClampedArray()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement21ATFixedBigUint64Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType21ATFixedBigUint64Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement20ATFixedBigInt64Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType20ATFixedBigInt64Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement17ATFixedUint8Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedUint8Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement16ATFixedInt8Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType16ATFixedInt8Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement18ATFixedUint16Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint16Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement17ATFixedInt16Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt16Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

TF_BUILTIN(StoreFixedElement18ATFixedUint32Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSTypedArray> parameter1 = UncheckedCast<JSTypedArray>(Parameter(Descriptor::kArray));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kIndex));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 199);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp1}));
    compiler::TNode<FixedTypedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp5}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint32Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp6);
  }
}

}  // namespace internal
}  // namespace v8

