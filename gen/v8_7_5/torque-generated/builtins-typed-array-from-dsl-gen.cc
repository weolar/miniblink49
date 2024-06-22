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
#include "src/builtins/builtins-regexp-gen.h"
#include "src/builtins/builtins-array-gen.h"
#include "src/builtins/builtins-collections-gen.h"
#include "src/builtins/builtins-data-view-gen.h"
#include "src/builtins/builtins-iterator-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-regexp-gen.h"
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

compiler::TNode<BuiltinPtr> TypedArrayBuiltinsFromDSLAssembler::UnsafeCast52FT7Context12JSTypedArray5ATSmi20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, BuiltinPtr> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 71);
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
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 1890);
    ca_.Goto(&block2, tmp3, tmp4, tmp5);
  }

    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<BuiltinPtr> tmp8;
    ca_.Bind(&block2, &tmp6, &tmp7, &tmp8);
  return compiler::TNode<BuiltinPtr>{tmp8};
}

compiler::TNode<BuiltinPtr> TypedArrayBuiltinsFromDSLAssembler::UnsafeCast74FT7Context12JSTypedArray5ATSmi20UT5ATSmi10HeapObject20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, BuiltinPtr> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 74);
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
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 1890);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 82);
    compiler::TNode<IntPtrT> tmp1 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp1);
    compiler::TNode<JSArrayBuffer>tmp2 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp0, tmp1});
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp2}));
    ca_.Branch(tmp3, &block3, &block4, tmp0);
  }

  if (block3.is_used()) {
    compiler::TNode<JSTypedArray> tmp4;
    ca_.Bind(&block3, &tmp4);
    ca_.Goto(&block1);
  }

  if (block4.is_used()) {
    compiler::TNode<JSTypedArray> tmp5;
    ca_.Bind(&block4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 83);
    compiler::TNode<JSTypedArray> tmp6;
    USE(tmp6);
    tmp6 = (compiler::TNode<JSTypedArray>{tmp5});
    ca_.Goto(&block2, tmp5, tmp6);
  }

  if (block2.is_used()) {
    compiler::TNode<JSTypedArray> tmp7;
    compiler::TNode<JSTypedArray> tmp8;
    ca_.Bind(&block2, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 80);
    ca_.Goto(&block5, tmp7, tmp8);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Detached);
  }

    compiler::TNode<JSTypedArray> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block5, &tmp9, &tmp10);
  return compiler::TNode<JSTypedArray>{tmp10};
}

TypedArrayBuiltinsFromDSLAssembler::AttachedJSTypedArrayWitness TypedArrayBuiltinsFromDSLAssembler::NewAttachedJSTypedArrayWitness(compiler::TNode<JSTypedArray> p_array) {
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, JSTypedArray, BuiltinPtr> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, JSTypedArray, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_array);

  if (block0.is_used()) {
    compiler::TNode<JSTypedArray> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 112);
    compiler::TNode<Int32T> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadElementsKind(compiler::TNode<JSTypedArray>{tmp0}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 114);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 115);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 116);
    compiler::TNode<BuiltinPtr> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BuiltinPtr>(TypedArrayBuiltinsFromDSLAssembler(state_).GetLoadFnForElementsKind(compiler::TNode<Int32T>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 113);
    ca_.Goto(&block1, tmp0, tmp0, tmp0, tmp2);
  }

  if (block1.is_used()) {
    compiler::TNode<JSTypedArray> tmp3;
    compiler::TNode<JSTypedArray> tmp4;
    compiler::TNode<JSTypedArray> tmp5;
    compiler::TNode<BuiltinPtr> tmp6;
    ca_.Bind(&block1, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 110);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 121);
    compiler::TNode<BoolT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindGreaterThan(compiler::TNode<Int32T>{tmp0}, UINT32_ELEMENTS));
    ca_.Branch(tmp1, &block2, &block3, tmp0);
  }

  if (block2.is_used()) {
    compiler::TNode<Int32T> tmp2;
    ca_.Bind(&block2, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 122);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 123);
    ca_.Goto(&block1, tmp5, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedInt32Array)));
  }

  if (block6.is_used()) {
    compiler::TNode<Int32T> tmp6;
    ca_.Bind(&block6, &tmp6);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 124);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 125);
    ca_.Goto(&block1, tmp9, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement19ATFixedFloat32Array)));
  }

  if (block9.is_used()) {
    compiler::TNode<Int32T> tmp10;
    ca_.Bind(&block9, &tmp10);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 126);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 127);
    ca_.Goto(&block1, tmp13, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement19ATFixedFloat64Array)));
  }

  if (block12.is_used()) {
    compiler::TNode<Int32T> tmp14;
    ca_.Bind(&block12, &tmp14);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 128);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 129);
    ca_.Goto(&block1, tmp17, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement24ATFixedUint8ClampedArray)));
  }

  if (block15.is_used()) {
    compiler::TNode<Int32T> tmp18;
    ca_.Bind(&block15, &tmp18);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 130);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 131);
    ca_.Goto(&block1, tmp21, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement21ATFixedBigUint64Array)));
  }

  if (block18.is_used()) {
    compiler::TNode<Int32T> tmp22;
    ca_.Bind(&block18, &tmp22);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 132);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 133);
    ca_.Goto(&block1, tmp25, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement20ATFixedBigInt64Array)));
  }

  if (block21.is_used()) {
    compiler::TNode<Int32T> tmp26;
    ca_.Bind(&block21, &tmp26);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 135);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block3.is_used()) {
    compiler::TNode<Int32T> tmp27;
    ca_.Bind(&block3, &tmp27);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 138);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 139);
    ca_.Goto(&block1, tmp30, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedUint8Array)));
  }

  if (block24.is_used()) {
    compiler::TNode<Int32T> tmp31;
    ca_.Bind(&block24, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 140);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 141);
    ca_.Goto(&block1, tmp34, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement16ATFixedInt8Array)));
  }

  if (block27.is_used()) {
    compiler::TNode<Int32T> tmp35;
    ca_.Bind(&block27, &tmp35);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 142);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 143);
    ca_.Goto(&block1, tmp38, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement18ATFixedUint16Array)));
  }

  if (block30.is_used()) {
    compiler::TNode<Int32T> tmp39;
    ca_.Bind(&block30, &tmp39);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 144);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 145);
    ca_.Goto(&block1, tmp42, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedInt16Array)));
  }

  if (block33.is_used()) {
    compiler::TNode<Int32T> tmp43;
    ca_.Bind(&block33, &tmp43);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 146);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 147);
    ca_.Goto(&block1, tmp46, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement18ATFixedUint32Array)));
  }

  if (block36.is_used()) {
    compiler::TNode<Int32T> tmp47;
    ca_.Bind(&block36, &tmp47);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 149);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<Int32T> tmp48;
    compiler::TNode<BuiltinPtr> tmp49;
    ca_.Bind(&block1, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 120);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 156);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 159);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 162);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 165);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 168);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 171);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 174);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 177);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 180);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 183);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 186);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 154);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 210);
    compiler::TNode<Oddball> tmp5;
    USE(tmp5);
    tmp5 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp0}, compiler::TNode<JSReceiver>{tmp2}, compiler::TNode<Object>{tmp5}, compiler::TNode<Object>{tmp3}, compiler::TNode<Object>{tmp4}));
    compiler::TNode<Number> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToNumber_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp6}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 209);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 213);
    compiler::TNode<IntPtrT> tmp8 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp8);
    compiler::TNode<JSArrayBuffer>tmp9 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp1, tmp8});
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp9}));
    ca_.Branch(tmp10, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp7);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSTypedArray> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Number> tmp16;
    ca_.Bind(&block2, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 214);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp11}, MessageTemplate::kDetachedOperation, "%TypedArray%.prototype.sort");
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSTypedArray> tmp18;
    compiler::TNode<JSReceiver> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Number> tmp22;
    ca_.Bind(&block3, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 218);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).NumberIsNaN(compiler::TNode<Number>{tmp22}));
    ca_.Branch(tmp23, &block4, &block5, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<JSTypedArray> tmp25;
    compiler::TNode<JSReceiver> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<Number> tmp29;
    ca_.Bind(&block4, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    compiler::TNode<Number> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    ca_.Goto(&block1, tmp24, tmp25, tmp26, tmp27, tmp28, tmp30);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<JSTypedArray> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Number> tmp36;
    ca_.Bind(&block5, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 221);
    ca_.Goto(&block1, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<JSTypedArray> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<Number> tmp42;
    ca_.Bind(&block1, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 205);
    ca_.Goto(&block6, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42);
  }

    compiler::TNode<Context> tmp43;
    compiler::TNode<JSTypedArray> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Number> tmp48;
    ca_.Bind(&block6, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
  return compiler::TNode<Number>{tmp48};
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
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray, Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, Smi, FixedArray> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 230);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 231);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 233);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 234);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 237);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 234);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 238);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 240);
    compiler::TNode<Object> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp73}, compiler::TNode<Smi>{tmp78}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 241);
    compiler::TNode<Object> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp73}, compiler::TNode<Smi>{tmp79}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 242);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 243);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp120}, compiler::TNode<Smi>{tmp123}, compiler::TNode<Object>{tmp124});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 244);
    compiler::TNode<Smi> tmp126;
    USE(tmp126);
    tmp126 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp121}, compiler::TNode<Smi>{tmp126}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 242);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 246);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp135}, compiler::TNode<Smi>{tmp138}, compiler::TNode<Object>{tmp140});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 247);
    compiler::TNode<Smi> tmp141;
    USE(tmp141);
    tmp141 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp142;
    USE(tmp142);
    tmp142 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp137}, compiler::TNode<Smi>{tmp141}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 242);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 238);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 253);
    compiler::TNode<Smi> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp168;
    USE(tmp168);
    tmp168 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp165}, compiler::TNode<Smi>{tmp167}));
    compiler::TNode<Object> tmp169;
    USE(tmp169);
    tmp169 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp159}, compiler::TNode<Smi>{tmp165}));
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp163}, compiler::TNode<Smi>{tmp166}, compiler::TNode<Object>{tmp169});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 238);
    ca_.Goto(&block12, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp168, tmp166);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<JSTypedArray> tmp171;
    compiler::TNode<JSReceiver> tmp172;
    compiler::TNode<FixedArray> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<FixedArray> tmp177;
    compiler::TNode<Smi> tmp178;
    compiler::TNode<Smi> tmp179;
    compiler::TNode<Smi> tmp180;
    ca_.Bind(&block12, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 234);
    ca_.Goto(&block9, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp181;
    compiler::TNode<JSTypedArray> tmp182;
    compiler::TNode<JSReceiver> tmp183;
    compiler::TNode<FixedArray> tmp184;
    compiler::TNode<Smi> tmp185;
    compiler::TNode<Smi> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<FixedArray> tmp188;
    compiler::TNode<Smi> tmp189;
    compiler::TNode<Smi> tmp190;
    compiler::TNode<Smi> tmp191;
    ca_.Bind(&block9, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 233);
    ca_.Goto(&block5, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp192;
    compiler::TNode<JSTypedArray> tmp193;
    compiler::TNode<JSReceiver> tmp194;
    compiler::TNode<FixedArray> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<Smi> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<FixedArray> tmp199;
    compiler::TNode<Smi> tmp200;
    compiler::TNode<Smi> tmp201;
    compiler::TNode<Smi> tmp202;
    ca_.Bind(&block5, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202);
    compiler::TNode<Smi> tmp203;
    USE(tmp203);
    tmp203 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp204;
    USE(tmp204);
    tmp204 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp202}, compiler::TNode<Smi>{tmp203}));
    ca_.Goto(&block4, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp204);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp205;
    compiler::TNode<JSTypedArray> tmp206;
    compiler::TNode<JSReceiver> tmp207;
    compiler::TNode<FixedArray> tmp208;
    compiler::TNode<Smi> tmp209;
    compiler::TNode<Smi> tmp210;
    compiler::TNode<Smi> tmp211;
    compiler::TNode<FixedArray> tmp212;
    compiler::TNode<Smi> tmp213;
    compiler::TNode<Smi> tmp214;
    compiler::TNode<Smi> tmp215;
    ca_.Bind(&block3, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 229);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 226);
    ca_.Goto(&block1, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp216;
    compiler::TNode<JSTypedArray> tmp217;
    compiler::TNode<JSReceiver> tmp218;
    compiler::TNode<FixedArray> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<Smi> tmp221;
    compiler::TNode<Smi> tmp222;
    compiler::TNode<FixedArray> tmp223;
    ca_.Bind(&block1, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223);
    ca_.Goto(&block18, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223);
  }

    compiler::TNode<Context> tmp224;
    compiler::TNode<JSTypedArray> tmp225;
    compiler::TNode<JSReceiver> tmp226;
    compiler::TNode<FixedArray> tmp227;
    compiler::TNode<Smi> tmp228;
    compiler::TNode<Smi> tmp229;
    compiler::TNode<Smi> tmp230;
    compiler::TNode<FixedArray> tmp231;
    ca_.Bind(&block18, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231);
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
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, FixedArray, Smi, Smi, FixedArray, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 263);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp5}, compiler::TNode<Smi>{tmp4}));
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSar(compiler::TNode<Smi>{tmp7}, 1));
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp4}, compiler::TNode<Smi>{tmp8}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 268);
    compiler::TNode<Smi> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp9}, compiler::TNode<Smi>{tmp4}));
    compiler::TNode<Smi> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp10}, compiler::TNode<Smi>{tmp11}));
    ca_.Branch(tmp12, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp9);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<FixedArray> tmp16;
    compiler::TNode<Smi> tmp17;
    compiler::TNode<Smi> tmp18;
    compiler::TNode<FixedArray> tmp19;
    compiler::TNode<Smi> tmp20;
    ca_.Bind(&block1, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    compiler::TNode<Object> tmp21;
    tmp21 = CodeStubAssembler(state_).CallBuiltin(Builtins::kTypedArrayMergeSort, tmp13, tmp14, tmp15, tmp19, tmp17, tmp20, tmp16);
    USE(tmp21);
    ca_.Goto(&block2, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<JSTypedArray> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<FixedArray> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<FixedArray> tmp28;
    compiler::TNode<Smi> tmp29;
    ca_.Bind(&block2, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 269);
    compiler::TNode<Smi> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp29}));
    compiler::TNode<Smi> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp30}, compiler::TNode<Smi>{tmp31}));
    ca_.Branch(tmp32, &block3, &block4, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<JSTypedArray> tmp34;
    compiler::TNode<JSReceiver> tmp35;
    compiler::TNode<FixedArray> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<Smi> tmp38;
    compiler::TNode<FixedArray> tmp39;
    compiler::TNode<Smi> tmp40;
    ca_.Bind(&block3, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    compiler::TNode<Object> tmp41;
    tmp41 = CodeStubAssembler(state_).CallBuiltin(Builtins::kTypedArrayMergeSort, tmp33, tmp34, tmp35, tmp39, tmp40, tmp38, tmp36);
    USE(tmp41);
    ca_.Goto(&block4, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<JSTypedArray> tmp43;
    compiler::TNode<JSReceiver> tmp44;
    compiler::TNode<FixedArray> tmp45;
    compiler::TNode<Smi> tmp46;
    compiler::TNode<Smi> tmp47;
    compiler::TNode<FixedArray> tmp48;
    compiler::TNode<Smi> tmp49;
    ca_.Bind(&block4, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 271);
    TypedArrayBuiltinsFromDSLAssembler(state_).TypedArrayMerge(compiler::TNode<Context>{tmp42}, compiler::TNode<JSTypedArray>{tmp43}, compiler::TNode<JSReceiver>{tmp44}, compiler::TNode<FixedArray>{tmp45}, compiler::TNode<Smi>{tmp46}, compiler::TNode<Smi>{tmp49}, compiler::TNode<Smi>{tmp47}, compiler::TNode<FixedArray>{tmp48});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 273);
    compiler::TNode<Oddball> tmp50;
    USE(tmp50);
    tmp50 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp50);
  }
}

TF_BUILTIN(TypedArrayPrototypeSort, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  TNode<IntPtrT> arguments_length(ChangeInt32ToIntPtr(argc));
  TNode<RawPtrT> arguments_frame = UncheckedCast<RawPtrT>(LoadFramePointer());
  BaseBuiltinsFromDSLAssembler::Arguments torque_arguments(GetFrameArguments(arguments_frame, arguments_length));
  CodeStubArguments arguments(this, torque_arguments);
  TNode<Object> parameter1 = arguments.GetReceiver();
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, Object, JSReceiver> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block41(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block43(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block44(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block46(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block47(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block49(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block50(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block51(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block48(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block45(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block54(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block52(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block55(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block53(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block58(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block56(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block59(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSTypedArray, Smi, JSReceiver, BuiltinPtr, BuiltinPtr, Int32T, FixedArray, FixedArray, Smi> block57(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 282);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 281);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 283);
    compiler::TNode<Oddball> tmp32;
    USE(tmp32);
    tmp32 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp31}, compiler::TNode<HeapObject>{tmp32}));
    ca_.Branch(tmp33, &block7, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block7, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<BoolT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).TaggedIsCallable(compiler::TNode<Object>{tmp39}));
    compiler::TNode<BoolT> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp40}));
    ca_.Branch(tmp41, &block5, &block6, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block5, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 284);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp42}, MessageTemplate::kBadSortComparisonFunction, compiler::TNode<Object>{tmp47});
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<RawPtrT> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<IntPtrT> tmp52;
    compiler::TNode<Object> tmp53;
    ca_.Bind(&block6, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 288);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 293);
    compiler::TNode<JSTypedArray> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<JSTypedArray>(TypedArrayBuiltinsAssembler(state_).ValidateTypedArray(compiler::TNode<Context>{tmp48}, compiler::TNode<Object>{tmp49}, "%TypedArray%.prototype.sort"));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 292);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 296);
    compiler::TNode<Oddball> tmp55;
    USE(tmp55);
    tmp55 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp53}, compiler::TNode<HeapObject>{tmp55}));
    ca_.Branch(tmp56, &block8, &block9, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp49, tmp54);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<RawPtrT> tmp60;
    compiler::TNode<IntPtrT> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<JSTypedArray> tmp64;
    ca_.Bind(&block8, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 297);
    compiler::TNode<JSTypedArray> tmp65;
    tmp65 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kTypedArraySortFast, tmp57, tmp63));
    USE(tmp65);
    arguments.PopAndReturn(tmp65);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<RawPtrT> tmp69;
    compiler::TNode<IntPtrT> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<JSTypedArray> tmp73;
    ca_.Bind(&block9, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 301);
    compiler::TNode<IntPtrT> tmp74 = ca_.IntPtrConstant(JSTypedArray::kLengthOffset);
    USE(tmp74);
    compiler::TNode<Smi>tmp75 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp73, tmp74});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 304);
    compiler::TNode<Smi> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp75}, compiler::TNode<Smi>{tmp76}));
    ca_.Branch(tmp77, &block10, &block11, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp75);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<RawPtrT> tmp80;
    compiler::TNode<RawPtrT> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<JSTypedArray> tmp85;
    compiler::TNode<Smi> tmp86;
    ca_.Bind(&block10, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86);
    arguments.PopAndReturn(tmp85);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<RawPtrT> tmp89;
    compiler::TNode<RawPtrT> tmp90;
    compiler::TNode<IntPtrT> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<JSTypedArray> tmp94;
    compiler::TNode<Smi> tmp95;
    ca_.Bind(&block11, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 307);
    compiler::TNode<JSReceiver> tmp96;
    USE(tmp96);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp96 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp87}, compiler::TNode<Object>{tmp92}, &label0);
    ca_.Goto(&block14, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp92, tmp96);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block15, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp92);
    }
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<RawPtrT> tmp99;
    compiler::TNode<RawPtrT> tmp100;
    compiler::TNode<IntPtrT> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<JSTypedArray> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Object> tmp106;
    ca_.Bind(&block15, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106);
    ca_.Goto(&block13, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<RawPtrT> tmp109;
    compiler::TNode<RawPtrT> tmp110;
    compiler::TNode<IntPtrT> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<JSTypedArray> tmp114;
    compiler::TNode<Smi> tmp115;
    compiler::TNode<Object> tmp116;
    compiler::TNode<JSReceiver> tmp117;
    ca_.Bind(&block14, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117);
    ca_.Goto(&block12, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp117);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<RawPtrT> tmp120;
    compiler::TNode<RawPtrT> tmp121;
    compiler::TNode<IntPtrT> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<JSTypedArray> tmp125;
    compiler::TNode<Smi> tmp126;
    ca_.Bind(&block13, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp127;
    compiler::TNode<Object> tmp128;
    compiler::TNode<RawPtrT> tmp129;
    compiler::TNode<RawPtrT> tmp130;
    compiler::TNode<IntPtrT> tmp131;
    compiler::TNode<Object> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<JSTypedArray> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<JSReceiver> tmp136;
    ca_.Bind(&block12, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 306);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 308);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 309);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 311);
    compiler::TNode<Int32T> tmp137;
    USE(tmp137);
    tmp137 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadElementsKind(compiler::TNode<JSTypedArray>{tmp134}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 313);
    compiler::TNode<BoolT> tmp138;
    USE(tmp138);
    tmp138 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindGreaterThan(compiler::TNode<Int32T>{tmp137}, UINT32_ELEMENTS));
    ca_.Branch(tmp138, &block16, &block17, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, ca_.Uninitialized<BuiltinPtr>(), ca_.Uninitialized<BuiltinPtr>(), tmp137);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<RawPtrT> tmp141;
    compiler::TNode<RawPtrT> tmp142;
    compiler::TNode<IntPtrT> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<Object> tmp145;
    compiler::TNode<JSTypedArray> tmp146;
    compiler::TNode<Smi> tmp147;
    compiler::TNode<JSReceiver> tmp148;
    compiler::TNode<BuiltinPtr> tmp149;
    compiler::TNode<BuiltinPtr> tmp150;
    compiler::TNode<Int32T> tmp151;
    ca_.Bind(&block16, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 314);
    compiler::TNode<Int32T> tmp152;
    USE(tmp152);
    tmp152 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT32_ELEMENTS));
    compiler::TNode<BoolT> tmp153;
    USE(tmp153);
    tmp153 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp151}, compiler::TNode<Int32T>{tmp152}));
    ca_.Branch(tmp153, &block19, &block20, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<RawPtrT> tmp156;
    compiler::TNode<RawPtrT> tmp157;
    compiler::TNode<IntPtrT> tmp158;
    compiler::TNode<Object> tmp159;
    compiler::TNode<Object> tmp160;
    compiler::TNode<JSTypedArray> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<JSReceiver> tmp163;
    compiler::TNode<BuiltinPtr> tmp164;
    compiler::TNode<BuiltinPtr> tmp165;
    compiler::TNode<Int32T> tmp166;
    ca_.Bind(&block19, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 315);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 316);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 314);
    ca_.Goto(&block21, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedInt32Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement17ATFixedInt32Array)), tmp166);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp167;
    compiler::TNode<Object> tmp168;
    compiler::TNode<RawPtrT> tmp169;
    compiler::TNode<RawPtrT> tmp170;
    compiler::TNode<IntPtrT> tmp171;
    compiler::TNode<Object> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<JSTypedArray> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<JSReceiver> tmp176;
    compiler::TNode<BuiltinPtr> tmp177;
    compiler::TNode<BuiltinPtr> tmp178;
    compiler::TNode<Int32T> tmp179;
    ca_.Bind(&block20, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 317);
    compiler::TNode<Int32T> tmp180;
    USE(tmp180);
    tmp180 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT32_ELEMENTS));
    compiler::TNode<BoolT> tmp181;
    USE(tmp181);
    tmp181 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp179}, compiler::TNode<Int32T>{tmp180}));
    ca_.Branch(tmp181, &block22, &block23, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp182;
    compiler::TNode<Object> tmp183;
    compiler::TNode<RawPtrT> tmp184;
    compiler::TNode<RawPtrT> tmp185;
    compiler::TNode<IntPtrT> tmp186;
    compiler::TNode<Object> tmp187;
    compiler::TNode<Object> tmp188;
    compiler::TNode<JSTypedArray> tmp189;
    compiler::TNode<Smi> tmp190;
    compiler::TNode<JSReceiver> tmp191;
    compiler::TNode<BuiltinPtr> tmp192;
    compiler::TNode<BuiltinPtr> tmp193;
    compiler::TNode<Int32T> tmp194;
    ca_.Bind(&block22, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 318);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 319);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 317);
    ca_.Goto(&block24, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement19ATFixedFloat32Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement19ATFixedFloat32Array)), tmp194);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<RawPtrT> tmp197;
    compiler::TNode<RawPtrT> tmp198;
    compiler::TNode<IntPtrT> tmp199;
    compiler::TNode<Object> tmp200;
    compiler::TNode<Object> tmp201;
    compiler::TNode<JSTypedArray> tmp202;
    compiler::TNode<Smi> tmp203;
    compiler::TNode<JSReceiver> tmp204;
    compiler::TNode<BuiltinPtr> tmp205;
    compiler::TNode<BuiltinPtr> tmp206;
    compiler::TNode<Int32T> tmp207;
    ca_.Bind(&block23, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 320);
    compiler::TNode<Int32T> tmp208;
    USE(tmp208);
    tmp208 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT64_ELEMENTS));
    compiler::TNode<BoolT> tmp209;
    USE(tmp209);
    tmp209 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp207}, compiler::TNode<Int32T>{tmp208}));
    ca_.Branch(tmp209, &block25, &block26, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp210;
    compiler::TNode<Object> tmp211;
    compiler::TNode<RawPtrT> tmp212;
    compiler::TNode<RawPtrT> tmp213;
    compiler::TNode<IntPtrT> tmp214;
    compiler::TNode<Object> tmp215;
    compiler::TNode<Object> tmp216;
    compiler::TNode<JSTypedArray> tmp217;
    compiler::TNode<Smi> tmp218;
    compiler::TNode<JSReceiver> tmp219;
    compiler::TNode<BuiltinPtr> tmp220;
    compiler::TNode<BuiltinPtr> tmp221;
    compiler::TNode<Int32T> tmp222;
    ca_.Bind(&block25, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 321);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 322);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 320);
    ca_.Goto(&block27, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement19ATFixedFloat64Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement19ATFixedFloat64Array)), tmp222);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp223;
    compiler::TNode<Object> tmp224;
    compiler::TNode<RawPtrT> tmp225;
    compiler::TNode<RawPtrT> tmp226;
    compiler::TNode<IntPtrT> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Object> tmp229;
    compiler::TNode<JSTypedArray> tmp230;
    compiler::TNode<Smi> tmp231;
    compiler::TNode<JSReceiver> tmp232;
    compiler::TNode<BuiltinPtr> tmp233;
    compiler::TNode<BuiltinPtr> tmp234;
    compiler::TNode<Int32T> tmp235;
    ca_.Bind(&block26, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 323);
    compiler::TNode<Int32T> tmp236;
    USE(tmp236);
    tmp236 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_CLAMPED_ELEMENTS));
    compiler::TNode<BoolT> tmp237;
    USE(tmp237);
    tmp237 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp235}, compiler::TNode<Int32T>{tmp236}));
    ca_.Branch(tmp237, &block28, &block29, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp238;
    compiler::TNode<Object> tmp239;
    compiler::TNode<RawPtrT> tmp240;
    compiler::TNode<RawPtrT> tmp241;
    compiler::TNode<IntPtrT> tmp242;
    compiler::TNode<Object> tmp243;
    compiler::TNode<Object> tmp244;
    compiler::TNode<JSTypedArray> tmp245;
    compiler::TNode<Smi> tmp246;
    compiler::TNode<JSReceiver> tmp247;
    compiler::TNode<BuiltinPtr> tmp248;
    compiler::TNode<BuiltinPtr> tmp249;
    compiler::TNode<Int32T> tmp250;
    ca_.Bind(&block28, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 324);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 325);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 323);
    ca_.Goto(&block30, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement24ATFixedUint8ClampedArray)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement24ATFixedUint8ClampedArray)), tmp250);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp251;
    compiler::TNode<Object> tmp252;
    compiler::TNode<RawPtrT> tmp253;
    compiler::TNode<RawPtrT> tmp254;
    compiler::TNode<IntPtrT> tmp255;
    compiler::TNode<Object> tmp256;
    compiler::TNode<Object> tmp257;
    compiler::TNode<JSTypedArray> tmp258;
    compiler::TNode<Smi> tmp259;
    compiler::TNode<JSReceiver> tmp260;
    compiler::TNode<BuiltinPtr> tmp261;
    compiler::TNode<BuiltinPtr> tmp262;
    compiler::TNode<Int32T> tmp263;
    ca_.Bind(&block29, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 326);
    compiler::TNode<Int32T> tmp264;
    USE(tmp264);
    tmp264 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGUINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp265;
    USE(tmp265);
    tmp265 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp263}, compiler::TNode<Int32T>{tmp264}));
    ca_.Branch(tmp265, &block31, &block32, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp266;
    compiler::TNode<Object> tmp267;
    compiler::TNode<RawPtrT> tmp268;
    compiler::TNode<RawPtrT> tmp269;
    compiler::TNode<IntPtrT> tmp270;
    compiler::TNode<Object> tmp271;
    compiler::TNode<Object> tmp272;
    compiler::TNode<JSTypedArray> tmp273;
    compiler::TNode<Smi> tmp274;
    compiler::TNode<JSReceiver> tmp275;
    compiler::TNode<BuiltinPtr> tmp276;
    compiler::TNode<BuiltinPtr> tmp277;
    compiler::TNode<Int32T> tmp278;
    ca_.Bind(&block31, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 327);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 328);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 326);
    ca_.Goto(&block33, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272, tmp273, tmp274, tmp275, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement21ATFixedBigUint64Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement21ATFixedBigUint64Array)), tmp278);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp279;
    compiler::TNode<Object> tmp280;
    compiler::TNode<RawPtrT> tmp281;
    compiler::TNode<RawPtrT> tmp282;
    compiler::TNode<IntPtrT> tmp283;
    compiler::TNode<Object> tmp284;
    compiler::TNode<Object> tmp285;
    compiler::TNode<JSTypedArray> tmp286;
    compiler::TNode<Smi> tmp287;
    compiler::TNode<JSReceiver> tmp288;
    compiler::TNode<BuiltinPtr> tmp289;
    compiler::TNode<BuiltinPtr> tmp290;
    compiler::TNode<Int32T> tmp291;
    ca_.Bind(&block32, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 329);
    compiler::TNode<Int32T> tmp292;
    USE(tmp292);
    tmp292 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp293;
    USE(tmp293);
    tmp293 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp291}, compiler::TNode<Int32T>{tmp292}));
    ca_.Branch(tmp293, &block34, &block35, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp291);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp294;
    compiler::TNode<Object> tmp295;
    compiler::TNode<RawPtrT> tmp296;
    compiler::TNode<RawPtrT> tmp297;
    compiler::TNode<IntPtrT> tmp298;
    compiler::TNode<Object> tmp299;
    compiler::TNode<Object> tmp300;
    compiler::TNode<JSTypedArray> tmp301;
    compiler::TNode<Smi> tmp302;
    compiler::TNode<JSReceiver> tmp303;
    compiler::TNode<BuiltinPtr> tmp304;
    compiler::TNode<BuiltinPtr> tmp305;
    compiler::TNode<Int32T> tmp306;
    ca_.Bind(&block34, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 330);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 331);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 329);
    ca_.Goto(&block36, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement20ATFixedBigInt64Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement20ATFixedBigInt64Array)), tmp306);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp307;
    compiler::TNode<Object> tmp308;
    compiler::TNode<RawPtrT> tmp309;
    compiler::TNode<RawPtrT> tmp310;
    compiler::TNode<IntPtrT> tmp311;
    compiler::TNode<Object> tmp312;
    compiler::TNode<Object> tmp313;
    compiler::TNode<JSTypedArray> tmp314;
    compiler::TNode<Smi> tmp315;
    compiler::TNode<JSReceiver> tmp316;
    compiler::TNode<BuiltinPtr> tmp317;
    compiler::TNode<BuiltinPtr> tmp318;
    compiler::TNode<Int32T> tmp319;
    ca_.Bind(&block35, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 333);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp320;
    compiler::TNode<Object> tmp321;
    compiler::TNode<RawPtrT> tmp322;
    compiler::TNode<RawPtrT> tmp323;
    compiler::TNode<IntPtrT> tmp324;
    compiler::TNode<Object> tmp325;
    compiler::TNode<Object> tmp326;
    compiler::TNode<JSTypedArray> tmp327;
    compiler::TNode<Smi> tmp328;
    compiler::TNode<JSReceiver> tmp329;
    compiler::TNode<BuiltinPtr> tmp330;
    compiler::TNode<BuiltinPtr> tmp331;
    compiler::TNode<Int32T> tmp332;
    ca_.Bind(&block36, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 326);
    ca_.Goto(&block33, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp333;
    compiler::TNode<Object> tmp334;
    compiler::TNode<RawPtrT> tmp335;
    compiler::TNode<RawPtrT> tmp336;
    compiler::TNode<IntPtrT> tmp337;
    compiler::TNode<Object> tmp338;
    compiler::TNode<Object> tmp339;
    compiler::TNode<JSTypedArray> tmp340;
    compiler::TNode<Smi> tmp341;
    compiler::TNode<JSReceiver> tmp342;
    compiler::TNode<BuiltinPtr> tmp343;
    compiler::TNode<BuiltinPtr> tmp344;
    compiler::TNode<Int32T> tmp345;
    ca_.Bind(&block33, &tmp333, &tmp334, &tmp335, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 323);
    ca_.Goto(&block30, tmp333, tmp334, tmp335, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp346;
    compiler::TNode<Object> tmp347;
    compiler::TNode<RawPtrT> tmp348;
    compiler::TNode<RawPtrT> tmp349;
    compiler::TNode<IntPtrT> tmp350;
    compiler::TNode<Object> tmp351;
    compiler::TNode<Object> tmp352;
    compiler::TNode<JSTypedArray> tmp353;
    compiler::TNode<Smi> tmp354;
    compiler::TNode<JSReceiver> tmp355;
    compiler::TNode<BuiltinPtr> tmp356;
    compiler::TNode<BuiltinPtr> tmp357;
    compiler::TNode<Int32T> tmp358;
    ca_.Bind(&block30, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 320);
    ca_.Goto(&block27, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, tmp357, tmp358);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp359;
    compiler::TNode<Object> tmp360;
    compiler::TNode<RawPtrT> tmp361;
    compiler::TNode<RawPtrT> tmp362;
    compiler::TNode<IntPtrT> tmp363;
    compiler::TNode<Object> tmp364;
    compiler::TNode<Object> tmp365;
    compiler::TNode<JSTypedArray> tmp366;
    compiler::TNode<Smi> tmp367;
    compiler::TNode<JSReceiver> tmp368;
    compiler::TNode<BuiltinPtr> tmp369;
    compiler::TNode<BuiltinPtr> tmp370;
    compiler::TNode<Int32T> tmp371;
    ca_.Bind(&block27, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370, &tmp371);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 317);
    ca_.Goto(&block24, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp371);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp372;
    compiler::TNode<Object> tmp373;
    compiler::TNode<RawPtrT> tmp374;
    compiler::TNode<RawPtrT> tmp375;
    compiler::TNode<IntPtrT> tmp376;
    compiler::TNode<Object> tmp377;
    compiler::TNode<Object> tmp378;
    compiler::TNode<JSTypedArray> tmp379;
    compiler::TNode<Smi> tmp380;
    compiler::TNode<JSReceiver> tmp381;
    compiler::TNode<BuiltinPtr> tmp382;
    compiler::TNode<BuiltinPtr> tmp383;
    compiler::TNode<Int32T> tmp384;
    ca_.Bind(&block24, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 314);
    ca_.Goto(&block21, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp385;
    compiler::TNode<Object> tmp386;
    compiler::TNode<RawPtrT> tmp387;
    compiler::TNode<RawPtrT> tmp388;
    compiler::TNode<IntPtrT> tmp389;
    compiler::TNode<Object> tmp390;
    compiler::TNode<Object> tmp391;
    compiler::TNode<JSTypedArray> tmp392;
    compiler::TNode<Smi> tmp393;
    compiler::TNode<JSReceiver> tmp394;
    compiler::TNode<BuiltinPtr> tmp395;
    compiler::TNode<BuiltinPtr> tmp396;
    compiler::TNode<Int32T> tmp397;
    ca_.Bind(&block21, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 313);
    ca_.Goto(&block18, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp396, tmp397);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp398;
    compiler::TNode<Object> tmp399;
    compiler::TNode<RawPtrT> tmp400;
    compiler::TNode<RawPtrT> tmp401;
    compiler::TNode<IntPtrT> tmp402;
    compiler::TNode<Object> tmp403;
    compiler::TNode<Object> tmp404;
    compiler::TNode<JSTypedArray> tmp405;
    compiler::TNode<Smi> tmp406;
    compiler::TNode<JSReceiver> tmp407;
    compiler::TNode<BuiltinPtr> tmp408;
    compiler::TNode<BuiltinPtr> tmp409;
    compiler::TNode<Int32T> tmp410;
    ca_.Bind(&block17, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 336);
    compiler::TNode<Int32T> tmp411;
    USE(tmp411);
    tmp411 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_ELEMENTS));
    compiler::TNode<BoolT> tmp412;
    USE(tmp412);
    tmp412 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp410}, compiler::TNode<Int32T>{tmp411}));
    ca_.Branch(tmp412, &block37, &block38, tmp398, tmp399, tmp400, tmp401, tmp402, tmp403, tmp404, tmp405, tmp406, tmp407, tmp408, tmp409, tmp410);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp413;
    compiler::TNode<Object> tmp414;
    compiler::TNode<RawPtrT> tmp415;
    compiler::TNode<RawPtrT> tmp416;
    compiler::TNode<IntPtrT> tmp417;
    compiler::TNode<Object> tmp418;
    compiler::TNode<Object> tmp419;
    compiler::TNode<JSTypedArray> tmp420;
    compiler::TNode<Smi> tmp421;
    compiler::TNode<JSReceiver> tmp422;
    compiler::TNode<BuiltinPtr> tmp423;
    compiler::TNode<BuiltinPtr> tmp424;
    compiler::TNode<Int32T> tmp425;
    ca_.Bind(&block37, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420, &tmp421, &tmp422, &tmp423, &tmp424, &tmp425);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 337);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 338);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 336);
    ca_.Goto(&block39, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420, tmp421, tmp422, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedUint8Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement17ATFixedUint8Array)), tmp425);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp426;
    compiler::TNode<Object> tmp427;
    compiler::TNode<RawPtrT> tmp428;
    compiler::TNode<RawPtrT> tmp429;
    compiler::TNode<IntPtrT> tmp430;
    compiler::TNode<Object> tmp431;
    compiler::TNode<Object> tmp432;
    compiler::TNode<JSTypedArray> tmp433;
    compiler::TNode<Smi> tmp434;
    compiler::TNode<JSReceiver> tmp435;
    compiler::TNode<BuiltinPtr> tmp436;
    compiler::TNode<BuiltinPtr> tmp437;
    compiler::TNode<Int32T> tmp438;
    ca_.Bind(&block38, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 339);
    compiler::TNode<Int32T> tmp439;
    USE(tmp439);
    tmp439 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT8_ELEMENTS));
    compiler::TNode<BoolT> tmp440;
    USE(tmp440);
    tmp440 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp438}, compiler::TNode<Int32T>{tmp439}));
    ca_.Branch(tmp440, &block40, &block41, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp441;
    compiler::TNode<Object> tmp442;
    compiler::TNode<RawPtrT> tmp443;
    compiler::TNode<RawPtrT> tmp444;
    compiler::TNode<IntPtrT> tmp445;
    compiler::TNode<Object> tmp446;
    compiler::TNode<Object> tmp447;
    compiler::TNode<JSTypedArray> tmp448;
    compiler::TNode<Smi> tmp449;
    compiler::TNode<JSReceiver> tmp450;
    compiler::TNode<BuiltinPtr> tmp451;
    compiler::TNode<BuiltinPtr> tmp452;
    compiler::TNode<Int32T> tmp453;
    ca_.Bind(&block40, &tmp441, &tmp442, &tmp443, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 340);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 341);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 339);
    ca_.Goto(&block42, tmp441, tmp442, tmp443, tmp444, tmp445, tmp446, tmp447, tmp448, tmp449, tmp450, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement16ATFixedInt8Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement16ATFixedInt8Array)), tmp453);
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp454;
    compiler::TNode<Object> tmp455;
    compiler::TNode<RawPtrT> tmp456;
    compiler::TNode<RawPtrT> tmp457;
    compiler::TNode<IntPtrT> tmp458;
    compiler::TNode<Object> tmp459;
    compiler::TNode<Object> tmp460;
    compiler::TNode<JSTypedArray> tmp461;
    compiler::TNode<Smi> tmp462;
    compiler::TNode<JSReceiver> tmp463;
    compiler::TNode<BuiltinPtr> tmp464;
    compiler::TNode<BuiltinPtr> tmp465;
    compiler::TNode<Int32T> tmp466;
    ca_.Bind(&block41, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463, &tmp464, &tmp465, &tmp466);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 342);
    compiler::TNode<Int32T> tmp467;
    USE(tmp467);
    tmp467 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT16_ELEMENTS));
    compiler::TNode<BoolT> tmp468;
    USE(tmp468);
    tmp468 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp466}, compiler::TNode<Int32T>{tmp467}));
    ca_.Branch(tmp468, &block43, &block44, tmp454, tmp455, tmp456, tmp457, tmp458, tmp459, tmp460, tmp461, tmp462, tmp463, tmp464, tmp465, tmp466);
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp469;
    compiler::TNode<Object> tmp470;
    compiler::TNode<RawPtrT> tmp471;
    compiler::TNode<RawPtrT> tmp472;
    compiler::TNode<IntPtrT> tmp473;
    compiler::TNode<Object> tmp474;
    compiler::TNode<Object> tmp475;
    compiler::TNode<JSTypedArray> tmp476;
    compiler::TNode<Smi> tmp477;
    compiler::TNode<JSReceiver> tmp478;
    compiler::TNode<BuiltinPtr> tmp479;
    compiler::TNode<BuiltinPtr> tmp480;
    compiler::TNode<Int32T> tmp481;
    ca_.Bind(&block43, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478, &tmp479, &tmp480, &tmp481);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 343);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 344);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 342);
    ca_.Goto(&block45, tmp469, tmp470, tmp471, tmp472, tmp473, tmp474, tmp475, tmp476, tmp477, tmp478, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement18ATFixedUint16Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement18ATFixedUint16Array)), tmp481);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp482;
    compiler::TNode<Object> tmp483;
    compiler::TNode<RawPtrT> tmp484;
    compiler::TNode<RawPtrT> tmp485;
    compiler::TNode<IntPtrT> tmp486;
    compiler::TNode<Object> tmp487;
    compiler::TNode<Object> tmp488;
    compiler::TNode<JSTypedArray> tmp489;
    compiler::TNode<Smi> tmp490;
    compiler::TNode<JSReceiver> tmp491;
    compiler::TNode<BuiltinPtr> tmp492;
    compiler::TNode<BuiltinPtr> tmp493;
    compiler::TNode<Int32T> tmp494;
    ca_.Bind(&block44, &tmp482, &tmp483, &tmp484, &tmp485, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 345);
    compiler::TNode<Int32T> tmp495;
    USE(tmp495);
    tmp495 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT16_ELEMENTS));
    compiler::TNode<BoolT> tmp496;
    USE(tmp496);
    tmp496 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp494}, compiler::TNode<Int32T>{tmp495}));
    ca_.Branch(tmp496, &block46, &block47, tmp482, tmp483, tmp484, tmp485, tmp486, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492, tmp493, tmp494);
  }

  if (block46.is_used()) {
    compiler::TNode<Context> tmp497;
    compiler::TNode<Object> tmp498;
    compiler::TNode<RawPtrT> tmp499;
    compiler::TNode<RawPtrT> tmp500;
    compiler::TNode<IntPtrT> tmp501;
    compiler::TNode<Object> tmp502;
    compiler::TNode<Object> tmp503;
    compiler::TNode<JSTypedArray> tmp504;
    compiler::TNode<Smi> tmp505;
    compiler::TNode<JSReceiver> tmp506;
    compiler::TNode<BuiltinPtr> tmp507;
    compiler::TNode<BuiltinPtr> tmp508;
    compiler::TNode<Int32T> tmp509;
    ca_.Bind(&block46, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501, &tmp502, &tmp503, &tmp504, &tmp505, &tmp506, &tmp507, &tmp508, &tmp509);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 346);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 347);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 345);
    ca_.Goto(&block48, tmp497, tmp498, tmp499, tmp500, tmp501, tmp502, tmp503, tmp504, tmp505, tmp506, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement17ATFixedInt16Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement17ATFixedInt16Array)), tmp509);
  }

  if (block47.is_used()) {
    compiler::TNode<Context> tmp510;
    compiler::TNode<Object> tmp511;
    compiler::TNode<RawPtrT> tmp512;
    compiler::TNode<RawPtrT> tmp513;
    compiler::TNode<IntPtrT> tmp514;
    compiler::TNode<Object> tmp515;
    compiler::TNode<Object> tmp516;
    compiler::TNode<JSTypedArray> tmp517;
    compiler::TNode<Smi> tmp518;
    compiler::TNode<JSReceiver> tmp519;
    compiler::TNode<BuiltinPtr> tmp520;
    compiler::TNode<BuiltinPtr> tmp521;
    compiler::TNode<Int32T> tmp522;
    ca_.Bind(&block47, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518, &tmp519, &tmp520, &tmp521, &tmp522);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 348);
    compiler::TNode<Int32T> tmp523;
    USE(tmp523);
    tmp523 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT32_ELEMENTS));
    compiler::TNode<BoolT> tmp524;
    USE(tmp524);
    tmp524 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp522}, compiler::TNode<Int32T>{tmp523}));
    ca_.Branch(tmp524, &block49, &block50, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517, tmp518, tmp519, tmp520, tmp521, tmp522);
  }

  if (block49.is_used()) {
    compiler::TNode<Context> tmp525;
    compiler::TNode<Object> tmp526;
    compiler::TNode<RawPtrT> tmp527;
    compiler::TNode<RawPtrT> tmp528;
    compiler::TNode<IntPtrT> tmp529;
    compiler::TNode<Object> tmp530;
    compiler::TNode<Object> tmp531;
    compiler::TNode<JSTypedArray> tmp532;
    compiler::TNode<Smi> tmp533;
    compiler::TNode<JSReceiver> tmp534;
    compiler::TNode<BuiltinPtr> tmp535;
    compiler::TNode<BuiltinPtr> tmp536;
    compiler::TNode<Int32T> tmp537;
    ca_.Bind(&block49, &tmp525, &tmp526, &tmp527, &tmp528, &tmp529, &tmp530, &tmp531, &tmp532, &tmp533, &tmp534, &tmp535, &tmp536, &tmp537);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 349);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 350);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 348);
    ca_.Goto(&block51, tmp525, tmp526, tmp527, tmp528, tmp529, tmp530, tmp531, tmp532, tmp533, tmp534, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadFixedElement18ATFixedUint32Array)), ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kStoreFixedElement18ATFixedUint32Array)), tmp537);
  }

  if (block50.is_used()) {
    compiler::TNode<Context> tmp538;
    compiler::TNode<Object> tmp539;
    compiler::TNode<RawPtrT> tmp540;
    compiler::TNode<RawPtrT> tmp541;
    compiler::TNode<IntPtrT> tmp542;
    compiler::TNode<Object> tmp543;
    compiler::TNode<Object> tmp544;
    compiler::TNode<JSTypedArray> tmp545;
    compiler::TNode<Smi> tmp546;
    compiler::TNode<JSReceiver> tmp547;
    compiler::TNode<BuiltinPtr> tmp548;
    compiler::TNode<BuiltinPtr> tmp549;
    compiler::TNode<Int32T> tmp550;
    ca_.Bind(&block50, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544, &tmp545, &tmp546, &tmp547, &tmp548, &tmp549, &tmp550);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 352);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block51.is_used()) {
    compiler::TNode<Context> tmp551;
    compiler::TNode<Object> tmp552;
    compiler::TNode<RawPtrT> tmp553;
    compiler::TNode<RawPtrT> tmp554;
    compiler::TNode<IntPtrT> tmp555;
    compiler::TNode<Object> tmp556;
    compiler::TNode<Object> tmp557;
    compiler::TNode<JSTypedArray> tmp558;
    compiler::TNode<Smi> tmp559;
    compiler::TNode<JSReceiver> tmp560;
    compiler::TNode<BuiltinPtr> tmp561;
    compiler::TNode<BuiltinPtr> tmp562;
    compiler::TNode<Int32T> tmp563;
    ca_.Bind(&block51, &tmp551, &tmp552, &tmp553, &tmp554, &tmp555, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 345);
    ca_.Goto(&block48, tmp551, tmp552, tmp553, tmp554, tmp555, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562, tmp563);
  }

  if (block48.is_used()) {
    compiler::TNode<Context> tmp564;
    compiler::TNode<Object> tmp565;
    compiler::TNode<RawPtrT> tmp566;
    compiler::TNode<RawPtrT> tmp567;
    compiler::TNode<IntPtrT> tmp568;
    compiler::TNode<Object> tmp569;
    compiler::TNode<Object> tmp570;
    compiler::TNode<JSTypedArray> tmp571;
    compiler::TNode<Smi> tmp572;
    compiler::TNode<JSReceiver> tmp573;
    compiler::TNode<BuiltinPtr> tmp574;
    compiler::TNode<BuiltinPtr> tmp575;
    compiler::TNode<Int32T> tmp576;
    ca_.Bind(&block48, &tmp564, &tmp565, &tmp566, &tmp567, &tmp568, &tmp569, &tmp570, &tmp571, &tmp572, &tmp573, &tmp574, &tmp575, &tmp576);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 342);
    ca_.Goto(&block45, tmp564, tmp565, tmp566, tmp567, tmp568, tmp569, tmp570, tmp571, tmp572, tmp573, tmp574, tmp575, tmp576);
  }

  if (block45.is_used()) {
    compiler::TNode<Context> tmp577;
    compiler::TNode<Object> tmp578;
    compiler::TNode<RawPtrT> tmp579;
    compiler::TNode<RawPtrT> tmp580;
    compiler::TNode<IntPtrT> tmp581;
    compiler::TNode<Object> tmp582;
    compiler::TNode<Object> tmp583;
    compiler::TNode<JSTypedArray> tmp584;
    compiler::TNode<Smi> tmp585;
    compiler::TNode<JSReceiver> tmp586;
    compiler::TNode<BuiltinPtr> tmp587;
    compiler::TNode<BuiltinPtr> tmp588;
    compiler::TNode<Int32T> tmp589;
    ca_.Bind(&block45, &tmp577, &tmp578, &tmp579, &tmp580, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585, &tmp586, &tmp587, &tmp588, &tmp589);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 339);
    ca_.Goto(&block42, tmp577, tmp578, tmp579, tmp580, tmp581, tmp582, tmp583, tmp584, tmp585, tmp586, tmp587, tmp588, tmp589);
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp590;
    compiler::TNode<Object> tmp591;
    compiler::TNode<RawPtrT> tmp592;
    compiler::TNode<RawPtrT> tmp593;
    compiler::TNode<IntPtrT> tmp594;
    compiler::TNode<Object> tmp595;
    compiler::TNode<Object> tmp596;
    compiler::TNode<JSTypedArray> tmp597;
    compiler::TNode<Smi> tmp598;
    compiler::TNode<JSReceiver> tmp599;
    compiler::TNode<BuiltinPtr> tmp600;
    compiler::TNode<BuiltinPtr> tmp601;
    compiler::TNode<Int32T> tmp602;
    ca_.Bind(&block42, &tmp590, &tmp591, &tmp592, &tmp593, &tmp594, &tmp595, &tmp596, &tmp597, &tmp598, &tmp599, &tmp600, &tmp601, &tmp602);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 336);
    ca_.Goto(&block39, tmp590, tmp591, tmp592, tmp593, tmp594, tmp595, tmp596, tmp597, tmp598, tmp599, tmp600, tmp601, tmp602);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp603;
    compiler::TNode<Object> tmp604;
    compiler::TNode<RawPtrT> tmp605;
    compiler::TNode<RawPtrT> tmp606;
    compiler::TNode<IntPtrT> tmp607;
    compiler::TNode<Object> tmp608;
    compiler::TNode<Object> tmp609;
    compiler::TNode<JSTypedArray> tmp610;
    compiler::TNode<Smi> tmp611;
    compiler::TNode<JSReceiver> tmp612;
    compiler::TNode<BuiltinPtr> tmp613;
    compiler::TNode<BuiltinPtr> tmp614;
    compiler::TNode<Int32T> tmp615;
    ca_.Bind(&block39, &tmp603, &tmp604, &tmp605, &tmp606, &tmp607, &tmp608, &tmp609, &tmp610, &tmp611, &tmp612, &tmp613, &tmp614, &tmp615);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 313);
    ca_.Goto(&block18, tmp603, tmp604, tmp605, tmp606, tmp607, tmp608, tmp609, tmp610, tmp611, tmp612, tmp613, tmp614, tmp615);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp616;
    compiler::TNode<Object> tmp617;
    compiler::TNode<RawPtrT> tmp618;
    compiler::TNode<RawPtrT> tmp619;
    compiler::TNode<IntPtrT> tmp620;
    compiler::TNode<Object> tmp621;
    compiler::TNode<Object> tmp622;
    compiler::TNode<JSTypedArray> tmp623;
    compiler::TNode<Smi> tmp624;
    compiler::TNode<JSReceiver> tmp625;
    compiler::TNode<BuiltinPtr> tmp626;
    compiler::TNode<BuiltinPtr> tmp627;
    compiler::TNode<Int32T> tmp628;
    ca_.Bind(&block18, &tmp616, &tmp617, &tmp618, &tmp619, &tmp620, &tmp621, &tmp622, &tmp623, &tmp624, &tmp625, &tmp626, &tmp627, &tmp628);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 359);
    compiler::TNode<IntPtrT> tmp629;
    USE(tmp629);
    tmp629 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp624}));
    compiler::TNode<FixedArray> tmp630;
    USE(tmp630);
    tmp630 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateZeroedFixedArray(compiler::TNode<IntPtrT>{tmp629}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 360);
    compiler::TNode<IntPtrT> tmp631;
    USE(tmp631);
    tmp631 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp624}));
    compiler::TNode<FixedArray> tmp632;
    USE(tmp632);
    tmp632 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateZeroedFixedArray(compiler::TNode<IntPtrT>{tmp631}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 362);
    compiler::TNode<Smi> tmp633;
    USE(tmp633);
    tmp633 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block54, tmp616, tmp617, tmp618, tmp619, tmp620, tmp621, tmp622, tmp623, tmp624, tmp625, tmp626, tmp627, tmp628, tmp630, tmp632, tmp633);
  }

  if (block54.is_used()) {
    compiler::TNode<Context> tmp634;
    compiler::TNode<Object> tmp635;
    compiler::TNode<RawPtrT> tmp636;
    compiler::TNode<RawPtrT> tmp637;
    compiler::TNode<IntPtrT> tmp638;
    compiler::TNode<Object> tmp639;
    compiler::TNode<Object> tmp640;
    compiler::TNode<JSTypedArray> tmp641;
    compiler::TNode<Smi> tmp642;
    compiler::TNode<JSReceiver> tmp643;
    compiler::TNode<BuiltinPtr> tmp644;
    compiler::TNode<BuiltinPtr> tmp645;
    compiler::TNode<Int32T> tmp646;
    compiler::TNode<FixedArray> tmp647;
    compiler::TNode<FixedArray> tmp648;
    compiler::TNode<Smi> tmp649;
    ca_.Bind(&block54, &tmp634, &tmp635, &tmp636, &tmp637, &tmp638, &tmp639, &tmp640, &tmp641, &tmp642, &tmp643, &tmp644, &tmp645, &tmp646, &tmp647, &tmp648, &tmp649);
    compiler::TNode<BoolT> tmp650;
    USE(tmp650);
    tmp650 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp649}, compiler::TNode<Smi>{tmp642}));
    ca_.Branch(tmp650, &block52, &block53, tmp634, tmp635, tmp636, tmp637, tmp638, tmp639, tmp640, tmp641, tmp642, tmp643, tmp644, tmp645, tmp646, tmp647, tmp648, tmp649);
  }

  if (block52.is_used()) {
    compiler::TNode<Context> tmp651;
    compiler::TNode<Object> tmp652;
    compiler::TNode<RawPtrT> tmp653;
    compiler::TNode<RawPtrT> tmp654;
    compiler::TNode<IntPtrT> tmp655;
    compiler::TNode<Object> tmp656;
    compiler::TNode<Object> tmp657;
    compiler::TNode<JSTypedArray> tmp658;
    compiler::TNode<Smi> tmp659;
    compiler::TNode<JSReceiver> tmp660;
    compiler::TNode<BuiltinPtr> tmp661;
    compiler::TNode<BuiltinPtr> tmp662;
    compiler::TNode<Int32T> tmp663;
    compiler::TNode<FixedArray> tmp664;
    compiler::TNode<FixedArray> tmp665;
    compiler::TNode<Smi> tmp666;
    ca_.Bind(&block52, &tmp651, &tmp652, &tmp653, &tmp654, &tmp655, &tmp656, &tmp657, &tmp658, &tmp659, &tmp660, &tmp661, &tmp662, &tmp663, &tmp664, &tmp665, &tmp666);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 363);
    compiler::TNode<Object> tmp667 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(1)).descriptor(), tmp661, tmp651, tmp658, tmp666); 
    USE(tmp667);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 364);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp664}, compiler::TNode<Smi>{tmp666}, compiler::TNode<Object>{tmp667});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 365);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp665}, compiler::TNode<Smi>{tmp666}, compiler::TNode<Object>{tmp667});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 362);
    ca_.Goto(&block55, tmp651, tmp652, tmp653, tmp654, tmp655, tmp656, tmp657, tmp658, tmp659, tmp660, tmp661, tmp662, tmp663, tmp664, tmp665, tmp666);
  }

  if (block55.is_used()) {
    compiler::TNode<Context> tmp668;
    compiler::TNode<Object> tmp669;
    compiler::TNode<RawPtrT> tmp670;
    compiler::TNode<RawPtrT> tmp671;
    compiler::TNode<IntPtrT> tmp672;
    compiler::TNode<Object> tmp673;
    compiler::TNode<Object> tmp674;
    compiler::TNode<JSTypedArray> tmp675;
    compiler::TNode<Smi> tmp676;
    compiler::TNode<JSReceiver> tmp677;
    compiler::TNode<BuiltinPtr> tmp678;
    compiler::TNode<BuiltinPtr> tmp679;
    compiler::TNode<Int32T> tmp680;
    compiler::TNode<FixedArray> tmp681;
    compiler::TNode<FixedArray> tmp682;
    compiler::TNode<Smi> tmp683;
    ca_.Bind(&block55, &tmp668, &tmp669, &tmp670, &tmp671, &tmp672, &tmp673, &tmp674, &tmp675, &tmp676, &tmp677, &tmp678, &tmp679, &tmp680, &tmp681, &tmp682, &tmp683);
    compiler::TNode<Smi> tmp684;
    USE(tmp684);
    tmp684 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp685;
    USE(tmp685);
    tmp685 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp683}, compiler::TNode<Smi>{tmp684}));
    ca_.Goto(&block54, tmp668, tmp669, tmp670, tmp671, tmp672, tmp673, tmp674, tmp675, tmp676, tmp677, tmp678, tmp679, tmp680, tmp681, tmp682, tmp685);
  }

  if (block53.is_used()) {
    compiler::TNode<Context> tmp686;
    compiler::TNode<Object> tmp687;
    compiler::TNode<RawPtrT> tmp688;
    compiler::TNode<RawPtrT> tmp689;
    compiler::TNode<IntPtrT> tmp690;
    compiler::TNode<Object> tmp691;
    compiler::TNode<Object> tmp692;
    compiler::TNode<JSTypedArray> tmp693;
    compiler::TNode<Smi> tmp694;
    compiler::TNode<JSReceiver> tmp695;
    compiler::TNode<BuiltinPtr> tmp696;
    compiler::TNode<BuiltinPtr> tmp697;
    compiler::TNode<Int32T> tmp698;
    compiler::TNode<FixedArray> tmp699;
    compiler::TNode<FixedArray> tmp700;
    compiler::TNode<Smi> tmp701;
    ca_.Bind(&block53, &tmp686, &tmp687, &tmp688, &tmp689, &tmp690, &tmp691, &tmp692, &tmp693, &tmp694, &tmp695, &tmp696, &tmp697, &tmp698, &tmp699, &tmp700, &tmp701);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 368);
    compiler::TNode<Smi> tmp702;
    USE(tmp702);
    tmp702 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp703;
    tmp703 = CodeStubAssembler(state_).CallBuiltin(Builtins::kTypedArrayMergeSort, tmp686, tmp693, tmp695, tmp700, tmp702, tmp694, tmp699);
    USE(tmp703);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 371);
    compiler::TNode<Smi> tmp704;
    USE(tmp704);
    tmp704 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block58, tmp686, tmp687, tmp688, tmp689, tmp690, tmp691, tmp692, tmp693, tmp694, tmp695, tmp696, tmp697, tmp698, tmp699, tmp700, tmp704);
  }

  if (block58.is_used()) {
    compiler::TNode<Context> tmp705;
    compiler::TNode<Object> tmp706;
    compiler::TNode<RawPtrT> tmp707;
    compiler::TNode<RawPtrT> tmp708;
    compiler::TNode<IntPtrT> tmp709;
    compiler::TNode<Object> tmp710;
    compiler::TNode<Object> tmp711;
    compiler::TNode<JSTypedArray> tmp712;
    compiler::TNode<Smi> tmp713;
    compiler::TNode<JSReceiver> tmp714;
    compiler::TNode<BuiltinPtr> tmp715;
    compiler::TNode<BuiltinPtr> tmp716;
    compiler::TNode<Int32T> tmp717;
    compiler::TNode<FixedArray> tmp718;
    compiler::TNode<FixedArray> tmp719;
    compiler::TNode<Smi> tmp720;
    ca_.Bind(&block58, &tmp705, &tmp706, &tmp707, &tmp708, &tmp709, &tmp710, &tmp711, &tmp712, &tmp713, &tmp714, &tmp715, &tmp716, &tmp717, &tmp718, &tmp719, &tmp720);
    compiler::TNode<BoolT> tmp721;
    USE(tmp721);
    tmp721 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp720}, compiler::TNode<Smi>{tmp713}));
    ca_.Branch(tmp721, &block56, &block57, tmp705, tmp706, tmp707, tmp708, tmp709, tmp710, tmp711, tmp712, tmp713, tmp714, tmp715, tmp716, tmp717, tmp718, tmp719, tmp720);
  }

  if (block56.is_used()) {
    compiler::TNode<Context> tmp722;
    compiler::TNode<Object> tmp723;
    compiler::TNode<RawPtrT> tmp724;
    compiler::TNode<RawPtrT> tmp725;
    compiler::TNode<IntPtrT> tmp726;
    compiler::TNode<Object> tmp727;
    compiler::TNode<Object> tmp728;
    compiler::TNode<JSTypedArray> tmp729;
    compiler::TNode<Smi> tmp730;
    compiler::TNode<JSReceiver> tmp731;
    compiler::TNode<BuiltinPtr> tmp732;
    compiler::TNode<BuiltinPtr> tmp733;
    compiler::TNode<Int32T> tmp734;
    compiler::TNode<FixedArray> tmp735;
    compiler::TNode<FixedArray> tmp736;
    compiler::TNode<Smi> tmp737;
    ca_.Bind(&block56, &tmp722, &tmp723, &tmp724, &tmp725, &tmp726, &tmp727, &tmp728, &tmp729, &tmp730, &tmp731, &tmp732, &tmp733, &tmp734, &tmp735, &tmp736, &tmp737);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 372);
    compiler::TNode<Object> tmp738;
    USE(tmp738);
    tmp738 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp735}, compiler::TNode<Smi>{tmp737}));
    compiler::TNode<Object> tmp739 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(2)).descriptor(), tmp733, tmp722, tmp729, tmp737, tmp738); 
    USE(tmp739);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 371);
    ca_.Goto(&block59, tmp722, tmp723, tmp724, tmp725, tmp726, tmp727, tmp728, tmp729, tmp730, tmp731, tmp732, tmp733, tmp734, tmp735, tmp736, tmp737);
  }

  if (block59.is_used()) {
    compiler::TNode<Context> tmp740;
    compiler::TNode<Object> tmp741;
    compiler::TNode<RawPtrT> tmp742;
    compiler::TNode<RawPtrT> tmp743;
    compiler::TNode<IntPtrT> tmp744;
    compiler::TNode<Object> tmp745;
    compiler::TNode<Object> tmp746;
    compiler::TNode<JSTypedArray> tmp747;
    compiler::TNode<Smi> tmp748;
    compiler::TNode<JSReceiver> tmp749;
    compiler::TNode<BuiltinPtr> tmp750;
    compiler::TNode<BuiltinPtr> tmp751;
    compiler::TNode<Int32T> tmp752;
    compiler::TNode<FixedArray> tmp753;
    compiler::TNode<FixedArray> tmp754;
    compiler::TNode<Smi> tmp755;
    ca_.Bind(&block59, &tmp740, &tmp741, &tmp742, &tmp743, &tmp744, &tmp745, &tmp746, &tmp747, &tmp748, &tmp749, &tmp750, &tmp751, &tmp752, &tmp753, &tmp754, &tmp755);
    compiler::TNode<Smi> tmp756;
    USE(tmp756);
    tmp756 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp757;
    USE(tmp757);
    tmp757 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp755}, compiler::TNode<Smi>{tmp756}));
    ca_.Goto(&block58, tmp740, tmp741, tmp742, tmp743, tmp744, tmp745, tmp746, tmp747, tmp748, tmp749, tmp750, tmp751, tmp752, tmp753, tmp754, tmp757);
  }

  if (block57.is_used()) {
    compiler::TNode<Context> tmp758;
    compiler::TNode<Object> tmp759;
    compiler::TNode<RawPtrT> tmp760;
    compiler::TNode<RawPtrT> tmp761;
    compiler::TNode<IntPtrT> tmp762;
    compiler::TNode<Object> tmp763;
    compiler::TNode<Object> tmp764;
    compiler::TNode<JSTypedArray> tmp765;
    compiler::TNode<Smi> tmp766;
    compiler::TNode<JSReceiver> tmp767;
    compiler::TNode<BuiltinPtr> tmp768;
    compiler::TNode<BuiltinPtr> tmp769;
    compiler::TNode<Int32T> tmp770;
    compiler::TNode<FixedArray> tmp771;
    compiler::TNode<FixedArray> tmp772;
    compiler::TNode<Smi> tmp773;
    ca_.Bind(&block57, &tmp758, &tmp759, &tmp760, &tmp761, &tmp762, &tmp763, &tmp764, &tmp765, &tmp766, &tmp767, &tmp768, &tmp769, &tmp770, &tmp771, &tmp772, &tmp773);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 374);
    arguments.PopAndReturn(tmp765);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 192);
    compiler::TNode<RawPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 191);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt32Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat32Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat64Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType24ATFixedUint8ClampedArray()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType21ATFixedBigUint64Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType20ATFixedBigInt64Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedUint8Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType16ATFixedInt8Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint16Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt16Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 199);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp1, tmp4});
    compiler::TNode<FixedTypedArrayBase> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedTypedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast19FixedTypedArrayBase(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 200);
    CodeStubAssembler(state_).StoreFixedTypedArrayElementFromTagged(compiler::TNode<Context>{tmp0}, compiler::TNode<FixedTypedArrayBase>{tmp6}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Object>{tmp3}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint32Array()), CodeStubAssembler::SMI_PARAMETERS);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 202);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp7);
  }
}

}  // namespace internal
}  // namespace v8

