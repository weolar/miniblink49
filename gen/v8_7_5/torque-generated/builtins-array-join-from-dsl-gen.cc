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

TF_BUILTIN(LoadJoinElement20ATDictionaryElements, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, NumberDictionary, NumberDictionary, IntPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, NumberDictionary, NumberDictionary, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, NumberDictionary, NumberDictionary, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, NumberDictionary> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, NumberDictionary> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 21);
    compiler::TNode<JSArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 22);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp3}));
    compiler::TNode<NumberDictionary> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<NumberDictionary>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast18ATNumberDictionary(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 24);
    compiler::TNode<UintPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr20UT5ATSmi10HeapNumber(compiler::TNode<Number>{tmp2}));
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).Signed(compiler::TNode<UintPtrT>{tmp6}));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    tmp8 = CodeStubAssembler(state_).BasicLoadNumberDictionaryElement(compiler::TNode<NumberDictionary>{tmp5}, compiler::TNode<IntPtrT>{tmp7}, &label0, &label1);
    ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp5, tmp5, tmp7, tmp8);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp0, tmp1, tmp2, tmp3, tmp5, tmp5, tmp7);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block7, tmp0, tmp1, tmp2, tmp3, tmp5, tmp5, tmp7);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<Number> tmp11;
    compiler::TNode<JSArray> tmp12;
    compiler::TNode<NumberDictionary> tmp13;
    compiler::TNode<NumberDictionary> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    ca_.Bind(&block6, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.Goto(&block4, tmp9, tmp10, tmp11, tmp12, tmp13);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<JSArray> tmp19;
    compiler::TNode<NumberDictionary> tmp20;
    compiler::TNode<NumberDictionary> tmp21;
    compiler::TNode<IntPtrT> tmp22;
    ca_.Bind(&block7, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block2, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<JSArray> tmp26;
    compiler::TNode<NumberDictionary> tmp27;
    compiler::TNode<NumberDictionary> tmp28;
    compiler::TNode<IntPtrT> tmp29;
    compiler::TNode<Object> tmp30;
    ca_.Bind(&block5, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    CodeStubAssembler(state_).Return(tmp30);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<JSArray> tmp34;
    compiler::TNode<NumberDictionary> tmp35;
    ca_.Bind(&block4, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 28);
    compiler::TNode<Object> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp31}, compiler::TNode<Object>{tmp32}, compiler::TNode<Object>{tmp33}));
    CodeStubAssembler(state_).Return(tmp36);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<JSReceiver> tmp38;
    compiler::TNode<Number> tmp39;
    compiler::TNode<JSArray> tmp40;
    compiler::TNode<NumberDictionary> tmp41;
    ca_.Bind(&block2, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 31);
    compiler::TNode<String> tmp42;
    USE(tmp42);
    tmp42 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    CodeStubAssembler(state_).Return(tmp42);
  }
}

TF_BUILTIN(LoadJoinElement25ATFastSmiOrObjectElements, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, FixedArray, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, FixedArray, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, FixedArray, Object, String> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, FixedArray, Object, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 37);
    compiler::TNode<JSArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 38);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp3}));
    compiler::TNode<FixedArray> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 39);
    compiler::TNode<Smi> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    compiler::TNode<Object> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp5}, compiler::TNode<Smi>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 40);
    compiler::TNode<Oddball> tmp8;
    USE(tmp8);
    tmp8 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp7}, compiler::TNode<HeapObject>{tmp8}));
    ca_.Branch(tmp9, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp5, tmp7);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<JSArray> tmp13;
    compiler::TNode<FixedArray> tmp14;
    compiler::TNode<Object> tmp15;
    ca_.Bind(&block1, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    compiler::TNode<String> tmp16;
    USE(tmp16);
    tmp16 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.Goto(&block4, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSReceiver> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<JSArray> tmp20;
    compiler::TNode<FixedArray> tmp21;
    compiler::TNode<Object> tmp22;
    ca_.Bind(&block2, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block3, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp22);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<JSArray> tmp26;
    compiler::TNode<FixedArray> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<String> tmp29;
    ca_.Bind(&block4, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.Goto(&block3, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<JSReceiver> tmp31;
    compiler::TNode<Number> tmp32;
    compiler::TNode<JSArray> tmp33;
    compiler::TNode<FixedArray> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Object> tmp36;
    ca_.Bind(&block3, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    CodeStubAssembler(state_).Return(tmp36);
  }
}

TF_BUILTIN(LoadJoinElement20ATFastDoubleElements, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, FixedDoubleArray, FixedDoubleArray, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, FixedDoubleArray, FixedDoubleArray, Smi, Float64T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, FixedDoubleArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSArray, FixedDoubleArray, Float64T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 45);
    compiler::TNode<JSArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 47);
    compiler::TNode<FixedArrayBase> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp3}));
    compiler::TNode<FixedDoubleArray> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 46);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 49);
    compiler::TNode<Smi> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 48);
    compiler::TNode<Float64T> tmp7;
    USE(tmp7);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp7 = CodeStubAssembler(state_).LoadDoubleWithHoleCheck(compiler::TNode<FixedDoubleArray>{tmp5}, compiler::TNode<Smi>{tmp6}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp5, tmp5, tmp6, tmp7);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp5, tmp5, tmp6);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<JSReceiver> tmp9;
    compiler::TNode<Number> tmp10;
    compiler::TNode<JSArray> tmp11;
    compiler::TNode<FixedDoubleArray> tmp12;
    compiler::TNode<FixedDoubleArray> tmp13;
    compiler::TNode<Smi> tmp14;
    ca_.Bind(&block4, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.Goto(&block2, tmp8, tmp9, tmp10, tmp11, tmp12);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<JSArray> tmp18;
    compiler::TNode<FixedDoubleArray> tmp19;
    compiler::TNode<FixedDoubleArray> tmp20;
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Float64T> tmp22;
    ca_.Bind(&block3, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block1, tmp15, tmp16, tmp17, tmp18, tmp19, tmp22);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<JSArray> tmp26;
    compiler::TNode<FixedDoubleArray> tmp27;
    ca_.Bind(&block2, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 49);
    compiler::TNode<String> tmp28;
    USE(tmp28);
    tmp28 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    CodeStubAssembler(state_).Return(tmp28);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<JSReceiver> tmp30;
    compiler::TNode<Number> tmp31;
    compiler::TNode<JSArray> tmp32;
    compiler::TNode<FixedDoubleArray> tmp33;
    compiler::TNode<Float64T> tmp34;
    ca_.Bind(&block1, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 48);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 50);
    compiler::TNode<HeapNumber> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<HeapNumber>(CodeStubAssembler(state_).AllocateHeapNumberWithValue(compiler::TNode<Float64T>{tmp34}));
    CodeStubAssembler(state_).Return(tmp35);
  }
}

TF_BUILTIN(ConvertToLocaleString, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Object> parameter1 = UncheckedCast<Object>(Parameter(Descriptor::kElement));
  USE(parameter1);
  compiler::TNode<Object> parameter2 = UncheckedCast<Object>(Parameter(Descriptor::kLocales));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kOptions));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 65);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{tmp1}));
    ca_.Branch(tmp4, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<Object> tmp8;
    ca_.Bind(&block1, &tmp5, &tmp6, &tmp7, &tmp8);
    compiler::TNode<String> tmp9;
    USE(tmp9);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    CodeStubAssembler(state_).Return(tmp9);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 67);
    compiler::TNode<Object> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapObject18ATconstexpr_string("toLocaleString"));
    compiler::TNode<Object> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp10}, compiler::TNode<Object>{tmp11}, compiler::TNode<Object>{tmp14}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 69);
    compiler::TNode<JSReceiver> tmp16;
    USE(tmp16);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp16 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp10}, compiler::TNode<Object>{tmp15}, &label0);
    ca_.Goto(&block5, tmp10, tmp11, tmp12, tmp13, tmp15, tmp15, tmp16);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp10, tmp11, tmp12, tmp13, tmp15, tmp15);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Object> tmp22;
    ca_.Bind(&block6, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block4, tmp17, tmp18, tmp19, tmp20, tmp21);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    ca_.Bind(&block5, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 70);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 71);
    compiler::TNode<BoolT> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{tmp25}));
    ca_.Branch(tmp30, &block7, &block8, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29, ca_.Uninitialized<Object>());
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<JSReceiver> tmp36;
    compiler::TNode<Object> tmp37;
    ca_.Bind(&block7, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 72);
    compiler::TNode<Object> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp31}, compiler::TNode<JSReceiver>{tmp36}, compiler::TNode<Object>{tmp32}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 71);
    ca_.Goto(&block9, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp38);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<JSReceiver> tmp44;
    compiler::TNode<Object> tmp45;
    ca_.Bind(&block8, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 73);
    compiler::TNode<BoolT> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{tmp42}));
    ca_.Branch(tmp46, &block10, &block11, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<JSReceiver> tmp52;
    compiler::TNode<Object> tmp53;
    ca_.Bind(&block10, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 74);
    compiler::TNode<Object> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp47}, compiler::TNode<JSReceiver>{tmp52}, compiler::TNode<Object>{tmp48}, compiler::TNode<Object>{tmp49}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 73);
    ca_.Goto(&block12, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp54);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<Object> tmp61;
    ca_.Bind(&block11, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 76);
    compiler::TNode<Object> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp55}, compiler::TNode<JSReceiver>{tmp60}, compiler::TNode<Object>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp58}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 73);
    ca_.Goto(&block12, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp62);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<JSReceiver> tmp68;
    compiler::TNode<Object> tmp69;
    ca_.Bind(&block12, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 71);
    ca_.Goto(&block9, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<JSReceiver> tmp75;
    compiler::TNode<Object> tmp76;
    ca_.Bind(&block9, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 78);
    compiler::TNode<String> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp70}, compiler::TNode<Object>{tmp76}));
    CodeStubAssembler(state_).Return(tmp77);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    ca_.Bind(&block4, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 81);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp78}, MessageTemplate::kCalledNonCallable, compiler::TNode<Object>{tmp82});
  }
}

void ArrayJoinBuiltinsFromDSLAssembler::CannotUseSameArrayAccessor7JSArray(compiler::TNode<Context> p_context, compiler::TNode<BuiltinPtr> p_loadFn, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<Map> p_originalMap, compiler::TNode<Number> p_originalLen, compiler::CodeAssemblerLabel* label_Cannot, compiler::CodeAssemblerLabel* label_Can) {
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number, JSArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number, JSArray> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number, JSArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number, JSArray> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number, JSArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number, JSArray> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_loadFn, p_receiver, p_originalMap, p_originalLen);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<BuiltinPtr> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<Map> tmp3;
    compiler::TNode<Number> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 96);
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp1}, compiler::TNode<Smi>{ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement25ATGenericElementsAccessor))}));
    ca_.Branch(tmp5, &block3, &block4, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<BuiltinPtr> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Map> tmp9;
    compiler::TNode<Number> tmp10;
    ca_.Bind(&block3, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.Goto(&block2);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<BuiltinPtr> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    compiler::TNode<Map> tmp14;
    compiler::TNode<Number> tmp15;
    ca_.Bind(&block4, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 98);
    compiler::TNode<JSArray> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<JSArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSArray(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 99);
    compiler::TNode<Map> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Map>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapObjectMap(compiler::TNode<HeapObject>{tmp16}));
    compiler::TNode<BoolT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<HeapObject>{tmp14}, compiler::TNode<HeapObject>{tmp17}));
    ca_.Branch(tmp18, &block5, &block6, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<BuiltinPtr> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    compiler::TNode<Map> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<JSArray> tmp24;
    ca_.Bind(&block5, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block1);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<BuiltinPtr> tmp26;
    compiler::TNode<JSReceiver> tmp27;
    compiler::TNode<Map> tmp28;
    compiler::TNode<Number> tmp29;
    compiler::TNode<JSArray> tmp30;
    ca_.Bind(&block6, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 100);
    compiler::TNode<Number> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayLength(compiler::TNode<JSArray>{tmp30}));
    compiler::TNode<BoolT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).IsNumberNotEqual(compiler::TNode<Number>{tmp29}, compiler::TNode<Number>{tmp31}));
    ca_.Branch(tmp32, &block7, &block8, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<BuiltinPtr> tmp34;
    compiler::TNode<JSReceiver> tmp35;
    compiler::TNode<Map> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<JSArray> tmp38;
    ca_.Bind(&block7, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.Goto(&block1);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<BuiltinPtr> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    compiler::TNode<Map> tmp42;
    compiler::TNode<Number> tmp43;
    compiler::TNode<JSArray> tmp44;
    ca_.Bind(&block8, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 101);
    compiler::TNode<BoolT> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNoElementsProtectorCellInvalid());
    ca_.Branch(tmp45, &block9, &block10, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp46;
    compiler::TNode<BuiltinPtr> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    compiler::TNode<Map> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<JSArray> tmp51;
    ca_.Bind(&block9, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.Goto(&block1);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<BuiltinPtr> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    compiler::TNode<Map> tmp55;
    compiler::TNode<Number> tmp56;
    compiler::TNode<JSArray> tmp57;
    ca_.Bind(&block10, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 102);
    ca_.Goto(&block2);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 87);
    ca_.Goto(label_Cannot);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.Goto(label_Can);
  }
}

void ArrayJoinBuiltinsFromDSLAssembler::CannotUseSameArrayAccessor12JSTypedArray(compiler::TNode<Context> p_context, compiler::TNode<BuiltinPtr> p_loadFn, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<Map> p_initialMap, compiler::TNode<Number> p_initialLen, compiler::CodeAssemblerLabel* label_Cannot, compiler::CodeAssemblerLabel* label_Can) {
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number, JSTypedArray> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, BuiltinPtr, JSReceiver, Map, Number, JSTypedArray> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_loadFn, p_receiver, p_initialMap, p_initialLen);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<BuiltinPtr> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<Map> tmp3;
    compiler::TNode<Number> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 109);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 110);
    compiler::TNode<JSArrayBuffer> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp5}));
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp6}));
    ca_.Branch(tmp7, &block3, &block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<BuiltinPtr> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<Map> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<JSTypedArray> tmp13;
    ca_.Bind(&block3, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block1);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<BuiltinPtr> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<Map> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<JSTypedArray> tmp19;
    ca_.Bind(&block4, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 111);
    ca_.Goto(&block2);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 87);
    ca_.Goto(label_Cannot);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.Goto(label_Can);
  }
}

compiler::TNode<IntPtrT> ArrayJoinBuiltinsFromDSLAssembler::AddStringLength(compiler::TNode<Context> p_context, compiler::TNode<IntPtrT> p_lenA, compiler::TNode<IntPtrT> p_lenB) {
  compiler::CodeAssemblerParameterizedLabel<Context, IntPtrT, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, IntPtrT, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, IntPtrT, IntPtrT, IntPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, IntPtrT, IntPtrT, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, IntPtrT, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, IntPtrT, IntPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, IntPtrT, IntPtrT, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_lenA, p_lenB);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<IntPtrT> tmp1;
    compiler::TNode<IntPtrT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 120);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp3 = CodeStubAssembler(state_).TryIntPtrAdd(compiler::TNode<IntPtrT>{tmp1}, compiler::TNode<IntPtrT>{tmp2}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp1, tmp2, tmp3);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp1, tmp2);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp4;
    compiler::TNode<IntPtrT> tmp5;
    compiler::TNode<IntPtrT> tmp6;
    compiler::TNode<IntPtrT> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    ca_.Bind(&block5, &tmp4, &tmp5, &tmp6, &tmp7, &tmp8);
    ca_.Goto(&block3, tmp4, tmp5, tmp6);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    compiler::TNode<IntPtrT> tmp13;
    compiler::TNode<IntPtrT> tmp14;
    ca_.Bind(&block4, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 121);
    compiler::TNode<IntPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(String::kMaxLength));
    compiler::TNode<BoolT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp14}, compiler::TNode<IntPtrT>{tmp15}));
    ca_.Branch(tmp16, &block6, &block7, tmp9, tmp10, tmp11, tmp14);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    compiler::TNode<IntPtrT> tmp20;
    ca_.Bind(&block6, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.Goto(&block3, tmp17, tmp18, tmp19);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<IntPtrT> tmp22;
    compiler::TNode<IntPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    ca_.Bind(&block7, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 122);
    ca_.Goto(&block1, tmp21, tmp22, tmp23, tmp24);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    ca_.Bind(&block3, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 125);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp25);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<IntPtrT> tmp32;
    ca_.Bind(&block1, &tmp29, &tmp30, &tmp31, &tmp32);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 117);
    ca_.Goto(&block8, tmp29, tmp30, tmp31, tmp32);
  }

    compiler::TNode<Context> tmp33;
    compiler::TNode<IntPtrT> tmp34;
    compiler::TNode<IntPtrT> tmp35;
    compiler::TNode<IntPtrT> tmp36;
    ca_.Bind(&block8, &tmp33, &tmp34, &tmp35, &tmp36);
  return compiler::TNode<IntPtrT>{tmp36};
}

ArrayJoinBuiltinsFromDSLAssembler::Buffer ArrayJoinBuiltinsFromDSLAssembler::NewBuffer(compiler::TNode<UintPtrT> p_len, compiler::TNode<String> p_sep) {
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String, IntPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String, FixedArray, IntPtrT, IntPtrT, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String, FixedArray, IntPtrT, IntPtrT, BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_len, p_sep);

  if (block0.is_used()) {
    compiler::TNode<UintPtrT> tmp0;
    compiler::TNode<String> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 217);
    compiler::TNode<UintPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(FixedArray::kMaxRegularLength));
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrGreaterThan(compiler::TNode<UintPtrT>{tmp0}, compiler::TNode<UintPtrT>{tmp2}));
    ca_.Branch(tmp3, &block2, &block3, tmp0, tmp1);
  }

  if (block2.is_used()) {
    compiler::TNode<UintPtrT> tmp4;
    compiler::TNode<String> tmp5;
    ca_.Bind(&block2, &tmp4, &tmp5);
    ca_.Goto(&block5, tmp4, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<UintPtrT> tmp6;
    compiler::TNode<String> tmp7;
    ca_.Bind(&block3, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 219);
    compiler::TNode<IntPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).Signed(compiler::TNode<UintPtrT>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 217);
    ca_.Goto(&block4, tmp6, tmp7, tmp8);
  }

  if (block5.is_used()) {
    compiler::TNode<UintPtrT> tmp9;
    compiler::TNode<String> tmp10;
    ca_.Bind(&block5, &tmp9, &tmp10);
    compiler::TNode<IntPtrT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(FixedArray::kMaxRegularLength));
    ca_.Goto(&block4, tmp9, tmp10, tmp11);
  }

  if (block4.is_used()) {
    compiler::TNode<UintPtrT> tmp12;
    compiler::TNode<String> tmp13;
    compiler::TNode<IntPtrT> tmp14;
    ca_.Bind(&block4, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 220);
    compiler::TNode<IntPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp14}, compiler::TNode<IntPtrT>{tmp15}));
    ca_.Branch(tmp16, &block6, &block7, tmp12, tmp13, tmp14);
  }

  if (block7.is_used()) {
    compiler::TNode<UintPtrT> tmp17;
    compiler::TNode<String> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    ca_.Bind(&block7, &tmp17, &tmp18, &tmp19);
    CodeStubAssembler(state_).FailAssert("Torque assert \'cappedBufferSize > 0\' failed", "../../src/builtins/array-join.tq", 220);
  }

  if (block6.is_used()) {
    compiler::TNode<UintPtrT> tmp20;
    compiler::TNode<String> tmp21;
    compiler::TNode<IntPtrT> tmp22;
    ca_.Bind(&block6, &tmp20, &tmp21, &tmp22);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 222);
    compiler::TNode<FixedArray> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateZeroedFixedArray(compiler::TNode<IntPtrT>{tmp22}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 225);
    compiler::TNode<Int32T> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadInstanceType(compiler::TNode<HeapObject>{tmp21}));
    compiler::TNode<BoolT> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsOneByteStringInstanceType(compiler::TNode<Int32T>{tmp24}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 221);
    compiler::TNode<IntPtrT> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<IntPtrT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.Goto(&block1, tmp20, tmp21, tmp23, tmp26, tmp27, tmp25);
  }

  if (block1.is_used()) {
    compiler::TNode<UintPtrT> tmp28;
    compiler::TNode<String> tmp29;
    compiler::TNode<FixedArray> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<IntPtrT> tmp32;
    compiler::TNode<BoolT> tmp33;
    ca_.Bind(&block1, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 216);
    ca_.Goto(&block8, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33);
  }

    compiler::TNode<UintPtrT> tmp34;
    compiler::TNode<String> tmp35;
    compiler::TNode<FixedArray> tmp36;
    compiler::TNode<IntPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<BoolT> tmp39;
    ca_.Bind(&block8, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
  return ArrayJoinBuiltinsFromDSLAssembler::Buffer{compiler::TNode<FixedArray>{tmp36}, compiler::TNode<IntPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}, compiler::TNode<BoolT>{tmp39}};
}

compiler::TNode<String> ArrayJoinBuiltinsFromDSLAssembler::BufferJoin(compiler::TNode<Context> p_context, ArrayJoinBuiltinsFromDSLAssembler::Buffer p_buffer, compiler::TNode<String> p_sep) {
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object, Object, String> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object, Object, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, Uint32T> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, Uint32T> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, Uint32T, String> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, Uint32T, String> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, String> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_buffer.fixedArray, p_buffer.index, p_buffer.totalStringLength, p_buffer.isOneByte, p_sep);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<FixedArray> tmp1;
    compiler::TNode<IntPtrT> tmp2;
    compiler::TNode<IntPtrT> tmp3;
    compiler::TNode<BoolT> tmp4;
    compiler::TNode<String> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 231);
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsValidPositiveSmi(compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp6, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<FixedArray> tmp8;
    compiler::TNode<IntPtrT> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<BoolT> tmp11;
    compiler::TNode<String> tmp12;
    ca_.Bind(&block3, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    CodeStubAssembler(state_).FailAssert("Torque assert \'IsValidPositiveSmi(buffer.totalStringLength)\' failed", "../../src/builtins/array-join.tq", 231);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<FixedArray> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    compiler::TNode<IntPtrT> tmp16;
    compiler::TNode<BoolT> tmp17;
    compiler::TNode<String> tmp18;
    ca_.Bind(&block2, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 232);
    compiler::TNode<IntPtrT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp16}, compiler::TNode<IntPtrT>{tmp19}));
    ca_.Branch(tmp20, &block4, &block5, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<FixedArray> tmp22;
    compiler::TNode<IntPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<BoolT> tmp25;
    compiler::TNode<String> tmp26;
    ca_.Bind(&block4, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    compiler::TNode<String> tmp27;
    USE(tmp27);
    tmp27 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.Goto(&block1, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<FixedArray> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<String> tmp33;
    ca_.Bind(&block5, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 235);
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp34}));
    ca_.Branch(tmp35, &block6, &block7, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<FixedArray> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<IntPtrT> tmp39;
    compiler::TNode<BoolT> tmp40;
    compiler::TNode<String> tmp41;
    ca_.Bind(&block6, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 236);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 237);
    compiler::TNode<Object> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp37}, 0));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 240);
    compiler::TNode<String> tmp43;
    USE(tmp43);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp43 = BaseBuiltinsFromDSLAssembler(state_).Cast6String(compiler::TNode<Context>{tmp36}, compiler::TNode<Object>{tmp42}, &label0);
    ca_.Goto(&block10, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp37, tmp42, tmp42, tmp43);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block11, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp37, tmp42, tmp42);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<FixedArray> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<IntPtrT> tmp47;
    compiler::TNode<BoolT> tmp48;
    compiler::TNode<String> tmp49;
    compiler::TNode<FixedArray> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Object> tmp52;
    ca_.Bind(&block11, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.Goto(&block9, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp53;
    compiler::TNode<FixedArray> tmp54;
    compiler::TNode<IntPtrT> tmp55;
    compiler::TNode<IntPtrT> tmp56;
    compiler::TNode<BoolT> tmp57;
    compiler::TNode<String> tmp58;
    compiler::TNode<FixedArray> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<String> tmp62;
    ca_.Bind(&block10, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 241);
    ca_.Goto(&block1, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<FixedArray> tmp64;
    compiler::TNode<IntPtrT> tmp65;
    compiler::TNode<IntPtrT> tmp66;
    compiler::TNode<BoolT> tmp67;
    compiler::TNode<String> tmp68;
    compiler::TNode<FixedArray> tmp69;
    compiler::TNode<Object> tmp70;
    ca_.Bind(&block9, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 246);
    compiler::TNode<Number> tmp71;
    USE(tmp71);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp71 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp70)}, &label0);
    ca_.Goto(&block14, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, ca_.UncheckedCast<Object>(tmp70), tmp71);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block15, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, ca_.UncheckedCast<Object>(tmp70));
    }
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<FixedArray> tmp73;
    compiler::TNode<IntPtrT> tmp74;
    compiler::TNode<IntPtrT> tmp75;
    compiler::TNode<BoolT> tmp76;
    compiler::TNode<String> tmp77;
    compiler::TNode<FixedArray> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block15, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    ca_.Goto(&block13, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp81;
    compiler::TNode<FixedArray> tmp82;
    compiler::TNode<IntPtrT> tmp83;
    compiler::TNode<IntPtrT> tmp84;
    compiler::TNode<BoolT> tmp85;
    compiler::TNode<String> tmp86;
    compiler::TNode<FixedArray> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<Number> tmp90;
    ca_.Bind(&block14, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 247);
    compiler::TNode<String> tmp91;
    tmp91 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kStringRepeat, tmp81, tmp86, tmp90));
    USE(tmp91);
    ca_.Goto(&block1, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp91);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp92;
    compiler::TNode<FixedArray> tmp93;
    compiler::TNode<IntPtrT> tmp94;
    compiler::TNode<IntPtrT> tmp95;
    compiler::TNode<BoolT> tmp96;
    compiler::TNode<String> tmp97;
    compiler::TNode<FixedArray> tmp98;
    compiler::TNode<Object> tmp99;
    ca_.Bind(&block13, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 249);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 250);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-join.tq:250:11");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp100;
    compiler::TNode<FixedArray> tmp101;
    compiler::TNode<IntPtrT> tmp102;
    compiler::TNode<IntPtrT> tmp103;
    compiler::TNode<BoolT> tmp104;
    compiler::TNode<String> tmp105;
    ca_.Bind(&block7, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 255);
    compiler::TNode<UintPtrT> tmp106;
    USE(tmp106);
    tmp106 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).Unsigned(compiler::TNode<IntPtrT>{tmp103}));
    compiler::TNode<Uint32T> tmp107;
    USE(tmp107);
    tmp107 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATuint329ATuintptr(compiler::TNode<UintPtrT>{tmp106}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 256);
    ca_.Branch(tmp104, &block16, &block17, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp107);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp108;
    compiler::TNode<FixedArray> tmp109;
    compiler::TNode<IntPtrT> tmp110;
    compiler::TNode<IntPtrT> tmp111;
    compiler::TNode<BoolT> tmp112;
    compiler::TNode<String> tmp113;
    compiler::TNode<Uint32T> tmp114;
    ca_.Bind(&block16, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114);
    compiler::TNode<String> tmp115;
    USE(tmp115);
    tmp115 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).AllocateSeqOneByteString(compiler::TNode<Context>{tmp108}, compiler::TNode<Uint32T>{tmp114}));
    ca_.Goto(&block19, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp116;
    compiler::TNode<FixedArray> tmp117;
    compiler::TNode<IntPtrT> tmp118;
    compiler::TNode<IntPtrT> tmp119;
    compiler::TNode<BoolT> tmp120;
    compiler::TNode<String> tmp121;
    compiler::TNode<Uint32T> tmp122;
    ca_.Bind(&block17, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 257);
    compiler::TNode<String> tmp123;
    USE(tmp123);
    tmp123 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).AllocateSeqTwoByteString(compiler::TNode<Context>{tmp116}, compiler::TNode<Uint32T>{tmp122}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 256);
    ca_.Goto(&block18, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp124;
    compiler::TNode<FixedArray> tmp125;
    compiler::TNode<IntPtrT> tmp126;
    compiler::TNode<IntPtrT> tmp127;
    compiler::TNode<BoolT> tmp128;
    compiler::TNode<String> tmp129;
    compiler::TNode<Uint32T> tmp130;
    compiler::TNode<String> tmp131;
    ca_.Bind(&block19, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131);
    ca_.Goto(&block18, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp132;
    compiler::TNode<FixedArray> tmp133;
    compiler::TNode<IntPtrT> tmp134;
    compiler::TNode<IntPtrT> tmp135;
    compiler::TNode<BoolT> tmp136;
    compiler::TNode<String> tmp137;
    compiler::TNode<Uint32T> tmp138;
    compiler::TNode<String> tmp139;
    ca_.Bind(&block18, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 259);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 258);
    compiler::TNode<String> tmp140;
    USE(tmp140);
    tmp140 = ca_.UncheckedCast<String>(ArrayBuiltinsAssembler(state_).CallJSArrayArrayJoinConcatToSequentialString(compiler::TNode<FixedArray>{tmp133}, compiler::TNode<IntPtrT>{tmp134}, compiler::TNode<String>{tmp137}, compiler::TNode<String>{tmp139}));
    ca_.Goto(&block1, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp140);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp141;
    compiler::TNode<FixedArray> tmp142;
    compiler::TNode<IntPtrT> tmp143;
    compiler::TNode<IntPtrT> tmp144;
    compiler::TNode<BoolT> tmp145;
    compiler::TNode<String> tmp146;
    compiler::TNode<String> tmp147;
    ca_.Bind(&block1, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 229);
    ca_.Goto(&block20, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147);
  }

    compiler::TNode<Context> tmp148;
    compiler::TNode<FixedArray> tmp149;
    compiler::TNode<IntPtrT> tmp150;
    compiler::TNode<IntPtrT> tmp151;
    compiler::TNode<BoolT> tmp152;
    compiler::TNode<String> tmp153;
    compiler::TNode<String> tmp154;
    ca_.Bind(&block20, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154);
  return compiler::TNode<String>{tmp154};
}

compiler::TNode<Object> ArrayJoinBuiltinsFromDSLAssembler::ArrayJoin7JSArray(compiler::TNode<Context> p_context, bool p_useToLocaleString, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<String> p_sep, compiler::TNode<Number> p_lenNumber, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSReceiver> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSReceiver, JSArray> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block18(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray, NumberDictionary, Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray, NumberDictionary, Smi> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray, NumberDictionary, Smi> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray, NumberDictionary, Smi, Number> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray, NumberDictionary, Smi, Number, Smi> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray, NumberDictionary, Smi> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray, NumberDictionary, Smi> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray, NumberDictionary, Smi> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr, JSArray> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Object> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver, p_sep, p_lenNumber, p_locales, p_options);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<String> tmp2;
    compiler::TNode<Number> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 333);
    compiler::TNode<Map> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Map>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapObjectMap(compiler::TNode<HeapObject>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 334);
    compiler::TNode<Int32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadMapElementsKind(compiler::TNode<Map>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 335);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 338);
    compiler::TNode<JSArray> tmp8;
    USE(tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp8 = BaseBuiltinsFromDSLAssembler(state_).Cast7JSArray(compiler::TNode<HeapObject>{tmp1}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, ca_.Uninitialized<BuiltinPtr>(), tmp1, tmp8);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, ca_.Uninitialized<BuiltinPtr>(), tmp1);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<String> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Map> tmp15;
    compiler::TNode<Int32T> tmp16;
    compiler::TNode<BuiltinPtr> tmp17;
    compiler::TNode<JSReceiver> tmp18;
    ca_.Bind(&block5, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.Goto(&block3, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<JSReceiver> tmp20;
    compiler::TNode<String> tmp21;
    compiler::TNode<Number> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Map> tmp25;
    compiler::TNode<Int32T> tmp26;
    compiler::TNode<BuiltinPtr> tmp27;
    compiler::TNode<JSReceiver> tmp28;
    compiler::TNode<JSArray> tmp29;
    ca_.Bind(&block4, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 339);
    compiler::TNode<Number> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayLength(compiler::TNode<JSArray>{tmp29}));
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).IsNumberNotEqual(compiler::TNode<Number>{tmp30}, compiler::TNode<Number>{tmp22}));
    ca_.Branch(tmp31, &block6, &block7, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<String> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Map> tmp38;
    compiler::TNode<Int32T> tmp39;
    compiler::TNode<BuiltinPtr> tmp40;
    compiler::TNode<JSArray> tmp41;
    ca_.Bind(&block6, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.Goto(&block3, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<JSReceiver> tmp43;
    compiler::TNode<String> tmp44;
    compiler::TNode<Number> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Map> tmp48;
    compiler::TNode<Int32T> tmp49;
    compiler::TNode<BuiltinPtr> tmp50;
    compiler::TNode<JSArray> tmp51;
    ca_.Bind(&block7, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 340);
    compiler::TNode<BoolT> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsPrototypeInitialArrayPrototype(compiler::TNode<Context>{tmp42}, compiler::TNode<Map>{tmp48}));
    compiler::TNode<BoolT> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp52}));
    ca_.Branch(tmp53, &block8, &block9, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<JSReceiver> tmp55;
    compiler::TNode<String> tmp56;
    compiler::TNode<Number> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Map> tmp60;
    compiler::TNode<Int32T> tmp61;
    compiler::TNode<BuiltinPtr> tmp62;
    compiler::TNode<JSArray> tmp63;
    ca_.Bind(&block8, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63);
    ca_.Goto(&block3, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<JSReceiver> tmp65;
    compiler::TNode<String> tmp66;
    compiler::TNode<Number> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Map> tmp70;
    compiler::TNode<Int32T> tmp71;
    compiler::TNode<BuiltinPtr> tmp72;
    compiler::TNode<JSArray> tmp73;
    ca_.Bind(&block9, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 341);
    compiler::TNode<BoolT> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNoElementsProtectorCellInvalid());
    ca_.Branch(tmp74, &block10, &block11, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    compiler::TNode<String> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Map> tmp81;
    compiler::TNode<Int32T> tmp82;
    compiler::TNode<BuiltinPtr> tmp83;
    compiler::TNode<JSArray> tmp84;
    ca_.Bind(&block10, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84);
    ca_.Goto(&block3, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp85;
    compiler::TNode<JSReceiver> tmp86;
    compiler::TNode<String> tmp87;
    compiler::TNode<Number> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<Map> tmp91;
    compiler::TNode<Int32T> tmp92;
    compiler::TNode<BuiltinPtr> tmp93;
    compiler::TNode<JSArray> tmp94;
    ca_.Bind(&block11, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 343);
    compiler::TNode<BoolT> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindLessThanOrEqual(compiler::TNode<Int32T>{tmp92}, HOLEY_ELEMENTS));
    ca_.Branch(tmp95, &block12, &block13, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp96;
    compiler::TNode<JSReceiver> tmp97;
    compiler::TNode<String> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<Object> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<Map> tmp102;
    compiler::TNode<Int32T> tmp103;
    compiler::TNode<BuiltinPtr> tmp104;
    compiler::TNode<JSArray> tmp105;
    ca_.Bind(&block12, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 344);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 343);
    ca_.Goto(&block14, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement25ATFastSmiOrObjectElements)), tmp105);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp106;
    compiler::TNode<JSReceiver> tmp107;
    compiler::TNode<String> tmp108;
    compiler::TNode<Number> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Map> tmp112;
    compiler::TNode<Int32T> tmp113;
    compiler::TNode<BuiltinPtr> tmp114;
    compiler::TNode<JSArray> tmp115;
    ca_.Bind(&block13, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 345);
    compiler::TNode<BoolT> tmp116;
    USE(tmp116);
    tmp116 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindLessThanOrEqual(compiler::TNode<Int32T>{tmp113}, HOLEY_DOUBLE_ELEMENTS));
    ca_.Branch(tmp116, &block15, &block16, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp117;
    compiler::TNode<JSReceiver> tmp118;
    compiler::TNode<String> tmp119;
    compiler::TNode<Number> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<Map> tmp123;
    compiler::TNode<Int32T> tmp124;
    compiler::TNode<BuiltinPtr> tmp125;
    compiler::TNode<JSArray> tmp126;
    ca_.Bind(&block15, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 346);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 345);
    ca_.Goto(&block17, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement20ATFastDoubleElements)), tmp126);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp127;
    compiler::TNode<JSReceiver> tmp128;
    compiler::TNode<String> tmp129;
    compiler::TNode<Number> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<Object> tmp132;
    compiler::TNode<Map> tmp133;
    compiler::TNode<Int32T> tmp134;
    compiler::TNode<BuiltinPtr> tmp135;
    compiler::TNode<JSArray> tmp136;
    ca_.Bind(&block16, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 347);
    compiler::TNode<Int32T> tmp137;
    USE(tmp137);
    tmp137 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(DICTIONARY_ELEMENTS));
    compiler::TNode<BoolT> tmp138;
    USE(tmp138);
    tmp138 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp134}, compiler::TNode<Int32T>{tmp137}));
    ca_.Branch(tmp138, &block18, &block19, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp139;
    compiler::TNode<JSReceiver> tmp140;
    compiler::TNode<String> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<Map> tmp145;
    compiler::TNode<Int32T> tmp146;
    compiler::TNode<BuiltinPtr> tmp147;
    compiler::TNode<JSArray> tmp148;
    ca_.Bind(&block18, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 350);
    compiler::TNode<FixedArrayBase> tmp149;
    USE(tmp149);
    tmp149 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp148}));
    compiler::TNode<NumberDictionary> tmp150;
    USE(tmp150);
    tmp150 = ca_.UncheckedCast<NumberDictionary>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast18ATNumberDictionary(compiler::TNode<Context>{tmp139}, compiler::TNode<Object>{tmp149}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 349);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 351);
    compiler::TNode<Smi> tmp151;
    USE(tmp151);
    tmp151 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).GetNumberDictionaryNumberOfElements(compiler::TNode<NumberDictionary>{tmp150}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 352);
    compiler::TNode<Smi> tmp152;
    USE(tmp152);
    tmp152 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp153;
    USE(tmp153);
    tmp153 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp151}, compiler::TNode<Smi>{tmp152}));
    ca_.Branch(tmp153, &block21, &block22, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp150, tmp151);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp154;
    compiler::TNode<JSReceiver> tmp155;
    compiler::TNode<String> tmp156;
    compiler::TNode<Number> tmp157;
    compiler::TNode<Object> tmp158;
    compiler::TNode<Object> tmp159;
    compiler::TNode<Map> tmp160;
    compiler::TNode<Int32T> tmp161;
    compiler::TNode<BuiltinPtr> tmp162;
    compiler::TNode<JSArray> tmp163;
    compiler::TNode<NumberDictionary> tmp164;
    compiler::TNode<Smi> tmp165;
    ca_.Bind(&block21, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 353);
    compiler::TNode<String> tmp166;
    USE(tmp166);
    tmp166 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp156}, compiler::TNode<HeapObject>{tmp166}));
    ca_.Branch(tmp167, &block24, &block25, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp168;
    compiler::TNode<JSReceiver> tmp169;
    compiler::TNode<String> tmp170;
    compiler::TNode<Number> tmp171;
    compiler::TNode<Object> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<Map> tmp174;
    compiler::TNode<Int32T> tmp175;
    compiler::TNode<BuiltinPtr> tmp176;
    compiler::TNode<JSArray> tmp177;
    compiler::TNode<NumberDictionary> tmp178;
    compiler::TNode<Smi> tmp179;
    ca_.Bind(&block24, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179);
    compiler::TNode<String> tmp180;
    USE(tmp180);
    tmp180 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.Goto(&block1, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp180);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp181;
    compiler::TNode<JSReceiver> tmp182;
    compiler::TNode<String> tmp183;
    compiler::TNode<Number> tmp184;
    compiler::TNode<Object> tmp185;
    compiler::TNode<Object> tmp186;
    compiler::TNode<Map> tmp187;
    compiler::TNode<Int32T> tmp188;
    compiler::TNode<BuiltinPtr> tmp189;
    compiler::TNode<JSArray> tmp190;
    compiler::TNode<NumberDictionary> tmp191;
    compiler::TNode<Smi> tmp192;
    ca_.Bind(&block25, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 356);
    compiler::TNode<Number> tmp193;
    USE(tmp193);
    tmp193 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp194;
    USE(tmp194);
    tmp194 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp184}, compiler::TNode<Number>{tmp193}));
    compiler::TNode<Smi> tmp195;
    USE(tmp195);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp195 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp194}, &label0);
    ca_.Goto(&block28, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp194, tmp195);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block29, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp194);
    }
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp196;
    compiler::TNode<JSReceiver> tmp197;
    compiler::TNode<String> tmp198;
    compiler::TNode<Number> tmp199;
    compiler::TNode<Object> tmp200;
    compiler::TNode<Object> tmp201;
    compiler::TNode<Map> tmp202;
    compiler::TNode<Int32T> tmp203;
    compiler::TNode<BuiltinPtr> tmp204;
    compiler::TNode<JSArray> tmp205;
    compiler::TNode<NumberDictionary> tmp206;
    compiler::TNode<Smi> tmp207;
    compiler::TNode<Number> tmp208;
    ca_.Bind(&block29, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208);
    ca_.Goto(&block27, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp209;
    compiler::TNode<JSReceiver> tmp210;
    compiler::TNode<String> tmp211;
    compiler::TNode<Number> tmp212;
    compiler::TNode<Object> tmp213;
    compiler::TNode<Object> tmp214;
    compiler::TNode<Map> tmp215;
    compiler::TNode<Int32T> tmp216;
    compiler::TNode<BuiltinPtr> tmp217;
    compiler::TNode<JSArray> tmp218;
    compiler::TNode<NumberDictionary> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<Number> tmp221;
    compiler::TNode<Smi> tmp222;
    ca_.Bind(&block28, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 355);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 357);
    compiler::TNode<String> tmp223;
    tmp223 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kStringRepeat, tmp209, tmp211, tmp222));
    USE(tmp223);
    ca_.Goto(&block1, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp223);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp224;
    compiler::TNode<JSReceiver> tmp225;
    compiler::TNode<String> tmp226;
    compiler::TNode<Number> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Object> tmp229;
    compiler::TNode<Map> tmp230;
    compiler::TNode<Int32T> tmp231;
    compiler::TNode<BuiltinPtr> tmp232;
    compiler::TNode<JSArray> tmp233;
    compiler::TNode<NumberDictionary> tmp234;
    compiler::TNode<Smi> tmp235;
    ca_.Bind(&block27, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 360);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp224);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp237;
    compiler::TNode<JSReceiver> tmp238;
    compiler::TNode<String> tmp239;
    compiler::TNode<Number> tmp240;
    compiler::TNode<Object> tmp241;
    compiler::TNode<Object> tmp242;
    compiler::TNode<Map> tmp243;
    compiler::TNode<Int32T> tmp244;
    compiler::TNode<BuiltinPtr> tmp245;
    compiler::TNode<JSArray> tmp246;
    compiler::TNode<NumberDictionary> tmp247;
    compiler::TNode<Smi> tmp248;
    ca_.Bind(&block22, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 363);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 352);
    ca_.Goto(&block23, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement20ATDictionaryElements)), tmp246, tmp247, tmp248);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp249;
    compiler::TNode<JSReceiver> tmp250;
    compiler::TNode<String> tmp251;
    compiler::TNode<Number> tmp252;
    compiler::TNode<Object> tmp253;
    compiler::TNode<Object> tmp254;
    compiler::TNode<Map> tmp255;
    compiler::TNode<Int32T> tmp256;
    compiler::TNode<BuiltinPtr> tmp257;
    compiler::TNode<JSArray> tmp258;
    compiler::TNode<NumberDictionary> tmp259;
    compiler::TNode<Smi> tmp260;
    ca_.Bind(&block23, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 348);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 347);
    ca_.Goto(&block20, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp261;
    compiler::TNode<JSReceiver> tmp262;
    compiler::TNode<String> tmp263;
    compiler::TNode<Number> tmp264;
    compiler::TNode<Object> tmp265;
    compiler::TNode<Object> tmp266;
    compiler::TNode<Map> tmp267;
    compiler::TNode<Int32T> tmp268;
    compiler::TNode<BuiltinPtr> tmp269;
    compiler::TNode<JSArray> tmp270;
    ca_.Bind(&block19, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 367);
    ca_.Goto(&block3, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp271;
    compiler::TNode<JSReceiver> tmp272;
    compiler::TNode<String> tmp273;
    compiler::TNode<Number> tmp274;
    compiler::TNode<Object> tmp275;
    compiler::TNode<Object> tmp276;
    compiler::TNode<Map> tmp277;
    compiler::TNode<Int32T> tmp278;
    compiler::TNode<BuiltinPtr> tmp279;
    compiler::TNode<JSArray> tmp280;
    ca_.Bind(&block20, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 345);
    ca_.Goto(&block17, tmp271, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp281;
    compiler::TNode<JSReceiver> tmp282;
    compiler::TNode<String> tmp283;
    compiler::TNode<Number> tmp284;
    compiler::TNode<Object> tmp285;
    compiler::TNode<Object> tmp286;
    compiler::TNode<Map> tmp287;
    compiler::TNode<Int32T> tmp288;
    compiler::TNode<BuiltinPtr> tmp289;
    compiler::TNode<JSArray> tmp290;
    ca_.Bind(&block17, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 343);
    ca_.Goto(&block14, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp291;
    compiler::TNode<JSReceiver> tmp292;
    compiler::TNode<String> tmp293;
    compiler::TNode<Number> tmp294;
    compiler::TNode<Object> tmp295;
    compiler::TNode<Object> tmp296;
    compiler::TNode<Map> tmp297;
    compiler::TNode<Int32T> tmp298;
    compiler::TNode<BuiltinPtr> tmp299;
    compiler::TNode<JSArray> tmp300;
    ca_.Bind(&block14, &tmp291, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 337);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 370);
    ca_.Goto(&block2, tmp291, tmp292, tmp293, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp301;
    compiler::TNode<JSReceiver> tmp302;
    compiler::TNode<String> tmp303;
    compiler::TNode<Number> tmp304;
    compiler::TNode<Object> tmp305;
    compiler::TNode<Object> tmp306;
    compiler::TNode<Map> tmp307;
    compiler::TNode<Int32T> tmp308;
    compiler::TNode<BuiltinPtr> tmp309;
    ca_.Bind(&block3, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 371);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 337);
    ca_.Goto(&block2, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement25ATGenericElementsAccessor)));
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp310;
    compiler::TNode<JSReceiver> tmp311;
    compiler::TNode<String> tmp312;
    compiler::TNode<Number> tmp313;
    compiler::TNode<Object> tmp314;
    compiler::TNode<Object> tmp315;
    compiler::TNode<Map> tmp316;
    compiler::TNode<Int32T> tmp317;
    compiler::TNode<BuiltinPtr> tmp318;
    ca_.Bind(&block2, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 374);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 373);
    compiler::TNode<String> tmp319;
    USE(tmp319);
    tmp319 = ca_.UncheckedCast<String>(ArrayJoinBuiltinsFromDSLAssembler(state_).ArrayJoinImpl7JSArray(compiler::TNode<Context>{tmp310}, compiler::TNode<JSReceiver>{tmp311}, compiler::TNode<String>{tmp312}, compiler::TNode<Number>{tmp313}, p_useToLocaleString, compiler::TNode<Object>{tmp314}, compiler::TNode<Object>{tmp315}, compiler::TNode<BuiltinPtr>{tmp318}));
    ca_.Goto(&block1, tmp310, tmp311, tmp312, tmp313, tmp314, tmp315, tmp319);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp320;
    compiler::TNode<JSReceiver> tmp321;
    compiler::TNode<String> tmp322;
    compiler::TNode<Number> tmp323;
    compiler::TNode<Object> tmp324;
    compiler::TNode<Object> tmp325;
    compiler::TNode<Object> tmp326;
    ca_.Bind(&block1, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 326);
    ca_.Goto(&block30, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326);
  }

    compiler::TNode<Context> tmp327;
    compiler::TNode<JSReceiver> tmp328;
    compiler::TNode<String> tmp329;
    compiler::TNode<Number> tmp330;
    compiler::TNode<Object> tmp331;
    compiler::TNode<Object> tmp332;
    compiler::TNode<Object> tmp333;
    ca_.Bind(&block30, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333);
  return compiler::TNode<Object>{tmp333};
}

compiler::TNode<Object> ArrayJoinBuiltinsFromDSLAssembler::ArrayJoin12JSTypedArray(compiler::TNode<Context> p_context, bool p_useToLocaleString, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<String> p_sep, compiler::TNode<Number> p_lenNumber, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Map, Int32T, BuiltinPtr> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, Object> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver, p_sep, p_lenNumber, p_locales, p_options);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<String> tmp2;
    compiler::TNode<Number> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 380);
    compiler::TNode<Map> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Map>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapObjectMap(compiler::TNode<HeapObject>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 381);
    compiler::TNode<Int32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadMapElementsKind(compiler::TNode<Map>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 382);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 384);
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindGreaterThan(compiler::TNode<Int32T>{tmp7}, UINT32_ELEMENTS));
    ca_.Branch(tmp8, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, ca_.Uninitialized<BuiltinPtr>());
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<String> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Map> tmp15;
    compiler::TNode<Int32T> tmp16;
    compiler::TNode<BuiltinPtr> tmp17;
    ca_.Bind(&block2, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 385);
    compiler::TNode<Int32T> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT32_ELEMENTS));
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp16}, compiler::TNode<Int32T>{tmp18}));
    ca_.Branch(tmp19, &block5, &block6, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    compiler::TNode<String> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Map> tmp26;
    compiler::TNode<Int32T> tmp27;
    compiler::TNode<BuiltinPtr> tmp28;
    ca_.Bind(&block5, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 386);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 385);
    ca_.Goto(&block7, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement17ATFixedInt32Array)));
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<JSReceiver> tmp30;
    compiler::TNode<String> tmp31;
    compiler::TNode<Number> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Map> tmp35;
    compiler::TNode<Int32T> tmp36;
    compiler::TNode<BuiltinPtr> tmp37;
    ca_.Bind(&block6, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 387);
    compiler::TNode<Int32T> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT32_ELEMENTS));
    compiler::TNode<BoolT> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp36}, compiler::TNode<Int32T>{tmp38}));
    ca_.Branch(tmp39, &block8, &block9, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    compiler::TNode<String> tmp42;
    compiler::TNode<Number> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Map> tmp46;
    compiler::TNode<Int32T> tmp47;
    compiler::TNode<BuiltinPtr> tmp48;
    ca_.Bind(&block8, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 388);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 387);
    ca_.Goto(&block10, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement19ATFixedFloat32Array)));
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<JSReceiver> tmp50;
    compiler::TNode<String> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Map> tmp55;
    compiler::TNode<Int32T> tmp56;
    compiler::TNode<BuiltinPtr> tmp57;
    ca_.Bind(&block9, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 389);
    compiler::TNode<Int32T> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT64_ELEMENTS));
    compiler::TNode<BoolT> tmp59;
    USE(tmp59);
    tmp59 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp56}, compiler::TNode<Int32T>{tmp58}));
    ca_.Branch(tmp59, &block11, &block12, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp60;
    compiler::TNode<JSReceiver> tmp61;
    compiler::TNode<String> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<Map> tmp66;
    compiler::TNode<Int32T> tmp67;
    compiler::TNode<BuiltinPtr> tmp68;
    ca_.Bind(&block11, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 390);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 389);
    ca_.Goto(&block13, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement19ATFixedFloat64Array)));
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<JSReceiver> tmp70;
    compiler::TNode<String> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<Map> tmp75;
    compiler::TNode<Int32T> tmp76;
    compiler::TNode<BuiltinPtr> tmp77;
    ca_.Bind(&block12, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 391);
    compiler::TNode<Int32T> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_CLAMPED_ELEMENTS));
    compiler::TNode<BoolT> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp76}, compiler::TNode<Int32T>{tmp78}));
    ca_.Branch(tmp79, &block14, &block15, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp80;
    compiler::TNode<JSReceiver> tmp81;
    compiler::TNode<String> tmp82;
    compiler::TNode<Number> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<Map> tmp86;
    compiler::TNode<Int32T> tmp87;
    compiler::TNode<BuiltinPtr> tmp88;
    ca_.Bind(&block14, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 392);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 391);
    ca_.Goto(&block16, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement24ATFixedUint8ClampedArray)));
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp89;
    compiler::TNode<JSReceiver> tmp90;
    compiler::TNode<String> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<Map> tmp95;
    compiler::TNode<Int32T> tmp96;
    compiler::TNode<BuiltinPtr> tmp97;
    ca_.Bind(&block15, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 393);
    compiler::TNode<Int32T> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGUINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp99;
    USE(tmp99);
    tmp99 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp96}, compiler::TNode<Int32T>{tmp98}));
    ca_.Branch(tmp99, &block17, &block18, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp100;
    compiler::TNode<JSReceiver> tmp101;
    compiler::TNode<String> tmp102;
    compiler::TNode<Number> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<Map> tmp106;
    compiler::TNode<Int32T> tmp107;
    compiler::TNode<BuiltinPtr> tmp108;
    ca_.Bind(&block17, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 394);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 393);
    ca_.Goto(&block19, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement21ATFixedBigUint64Array)));
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp109;
    compiler::TNode<JSReceiver> tmp110;
    compiler::TNode<String> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<Map> tmp115;
    compiler::TNode<Int32T> tmp116;
    compiler::TNode<BuiltinPtr> tmp117;
    ca_.Bind(&block18, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 395);
    compiler::TNode<Int32T> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp116}, compiler::TNode<Int32T>{tmp118}));
    ca_.Branch(tmp119, &block20, &block21, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp120;
    compiler::TNode<JSReceiver> tmp121;
    compiler::TNode<String> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<Map> tmp126;
    compiler::TNode<Int32T> tmp127;
    compiler::TNode<BuiltinPtr> tmp128;
    ca_.Bind(&block20, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 396);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 395);
    ca_.Goto(&block22, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement20ATFixedBigInt64Array)));
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp129;
    compiler::TNode<JSReceiver> tmp130;
    compiler::TNode<String> tmp131;
    compiler::TNode<Number> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Map> tmp135;
    compiler::TNode<Int32T> tmp136;
    compiler::TNode<BuiltinPtr> tmp137;
    ca_.Bind(&block21, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 398);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-join.tq:398:9");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp138;
    compiler::TNode<JSReceiver> tmp139;
    compiler::TNode<String> tmp140;
    compiler::TNode<Number> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<Map> tmp144;
    compiler::TNode<Int32T> tmp145;
    compiler::TNode<BuiltinPtr> tmp146;
    ca_.Bind(&block22, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 393);
    ca_.Goto(&block19, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp147;
    compiler::TNode<JSReceiver> tmp148;
    compiler::TNode<String> tmp149;
    compiler::TNode<Number> tmp150;
    compiler::TNode<Object> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<Map> tmp153;
    compiler::TNode<Int32T> tmp154;
    compiler::TNode<BuiltinPtr> tmp155;
    ca_.Bind(&block19, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 391);
    ca_.Goto(&block16, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp156;
    compiler::TNode<JSReceiver> tmp157;
    compiler::TNode<String> tmp158;
    compiler::TNode<Number> tmp159;
    compiler::TNode<Object> tmp160;
    compiler::TNode<Object> tmp161;
    compiler::TNode<Map> tmp162;
    compiler::TNode<Int32T> tmp163;
    compiler::TNode<BuiltinPtr> tmp164;
    ca_.Bind(&block16, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 389);
    ca_.Goto(&block13, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp165;
    compiler::TNode<JSReceiver> tmp166;
    compiler::TNode<String> tmp167;
    compiler::TNode<Number> tmp168;
    compiler::TNode<Object> tmp169;
    compiler::TNode<Object> tmp170;
    compiler::TNode<Map> tmp171;
    compiler::TNode<Int32T> tmp172;
    compiler::TNode<BuiltinPtr> tmp173;
    ca_.Bind(&block13, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 387);
    ca_.Goto(&block10, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp174;
    compiler::TNode<JSReceiver> tmp175;
    compiler::TNode<String> tmp176;
    compiler::TNode<Number> tmp177;
    compiler::TNode<Object> tmp178;
    compiler::TNode<Object> tmp179;
    compiler::TNode<Map> tmp180;
    compiler::TNode<Int32T> tmp181;
    compiler::TNode<BuiltinPtr> tmp182;
    ca_.Bind(&block10, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 385);
    ca_.Goto(&block7, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp183;
    compiler::TNode<JSReceiver> tmp184;
    compiler::TNode<String> tmp185;
    compiler::TNode<Number> tmp186;
    compiler::TNode<Object> tmp187;
    compiler::TNode<Object> tmp188;
    compiler::TNode<Map> tmp189;
    compiler::TNode<Int32T> tmp190;
    compiler::TNode<BuiltinPtr> tmp191;
    ca_.Bind(&block7, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 384);
    ca_.Goto(&block4, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp192;
    compiler::TNode<JSReceiver> tmp193;
    compiler::TNode<String> tmp194;
    compiler::TNode<Number> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<Object> tmp197;
    compiler::TNode<Map> tmp198;
    compiler::TNode<Int32T> tmp199;
    compiler::TNode<BuiltinPtr> tmp200;
    ca_.Bind(&block3, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 401);
    compiler::TNode<Int32T> tmp201;
    USE(tmp201);
    tmp201 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_ELEMENTS));
    compiler::TNode<BoolT> tmp202;
    USE(tmp202);
    tmp202 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp199}, compiler::TNode<Int32T>{tmp201}));
    ca_.Branch(tmp202, &block23, &block24, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp203;
    compiler::TNode<JSReceiver> tmp204;
    compiler::TNode<String> tmp205;
    compiler::TNode<Number> tmp206;
    compiler::TNode<Object> tmp207;
    compiler::TNode<Object> tmp208;
    compiler::TNode<Map> tmp209;
    compiler::TNode<Int32T> tmp210;
    compiler::TNode<BuiltinPtr> tmp211;
    ca_.Bind(&block23, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 402);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 401);
    ca_.Goto(&block25, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement17ATFixedUint8Array)));
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp212;
    compiler::TNode<JSReceiver> tmp213;
    compiler::TNode<String> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<Object> tmp216;
    compiler::TNode<Object> tmp217;
    compiler::TNode<Map> tmp218;
    compiler::TNode<Int32T> tmp219;
    compiler::TNode<BuiltinPtr> tmp220;
    ca_.Bind(&block24, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 403);
    compiler::TNode<Int32T> tmp221;
    USE(tmp221);
    tmp221 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT8_ELEMENTS));
    compiler::TNode<BoolT> tmp222;
    USE(tmp222);
    tmp222 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp219}, compiler::TNode<Int32T>{tmp221}));
    ca_.Branch(tmp222, &block26, &block27, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp223;
    compiler::TNode<JSReceiver> tmp224;
    compiler::TNode<String> tmp225;
    compiler::TNode<Number> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Map> tmp229;
    compiler::TNode<Int32T> tmp230;
    compiler::TNode<BuiltinPtr> tmp231;
    ca_.Bind(&block26, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 404);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 403);
    ca_.Goto(&block28, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement16ATFixedInt8Array)));
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp232;
    compiler::TNode<JSReceiver> tmp233;
    compiler::TNode<String> tmp234;
    compiler::TNode<Number> tmp235;
    compiler::TNode<Object> tmp236;
    compiler::TNode<Object> tmp237;
    compiler::TNode<Map> tmp238;
    compiler::TNode<Int32T> tmp239;
    compiler::TNode<BuiltinPtr> tmp240;
    ca_.Bind(&block27, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 405);
    compiler::TNode<Int32T> tmp241;
    USE(tmp241);
    tmp241 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT16_ELEMENTS));
    compiler::TNode<BoolT> tmp242;
    USE(tmp242);
    tmp242 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp239}, compiler::TNode<Int32T>{tmp241}));
    ca_.Branch(tmp242, &block29, &block30, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp243;
    compiler::TNode<JSReceiver> tmp244;
    compiler::TNode<String> tmp245;
    compiler::TNode<Number> tmp246;
    compiler::TNode<Object> tmp247;
    compiler::TNode<Object> tmp248;
    compiler::TNode<Map> tmp249;
    compiler::TNode<Int32T> tmp250;
    compiler::TNode<BuiltinPtr> tmp251;
    ca_.Bind(&block29, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 406);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 405);
    ca_.Goto(&block31, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement18ATFixedUint16Array)));
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp252;
    compiler::TNode<JSReceiver> tmp253;
    compiler::TNode<String> tmp254;
    compiler::TNode<Number> tmp255;
    compiler::TNode<Object> tmp256;
    compiler::TNode<Object> tmp257;
    compiler::TNode<Map> tmp258;
    compiler::TNode<Int32T> tmp259;
    compiler::TNode<BuiltinPtr> tmp260;
    ca_.Bind(&block30, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 407);
    compiler::TNode<Int32T> tmp261;
    USE(tmp261);
    tmp261 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT16_ELEMENTS));
    compiler::TNode<BoolT> tmp262;
    USE(tmp262);
    tmp262 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp259}, compiler::TNode<Int32T>{tmp261}));
    ca_.Branch(tmp262, &block32, &block33, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp263;
    compiler::TNode<JSReceiver> tmp264;
    compiler::TNode<String> tmp265;
    compiler::TNode<Number> tmp266;
    compiler::TNode<Object> tmp267;
    compiler::TNode<Object> tmp268;
    compiler::TNode<Map> tmp269;
    compiler::TNode<Int32T> tmp270;
    compiler::TNode<BuiltinPtr> tmp271;
    ca_.Bind(&block32, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 408);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 407);
    ca_.Goto(&block34, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement17ATFixedInt16Array)));
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp272;
    compiler::TNode<JSReceiver> tmp273;
    compiler::TNode<String> tmp274;
    compiler::TNode<Number> tmp275;
    compiler::TNode<Object> tmp276;
    compiler::TNode<Object> tmp277;
    compiler::TNode<Map> tmp278;
    compiler::TNode<Int32T> tmp279;
    compiler::TNode<BuiltinPtr> tmp280;
    ca_.Bind(&block33, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 409);
    compiler::TNode<Int32T> tmp281;
    USE(tmp281);
    tmp281 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT32_ELEMENTS));
    compiler::TNode<BoolT> tmp282;
    USE(tmp282);
    tmp282 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp279}, compiler::TNode<Int32T>{tmp281}));
    ca_.Branch(tmp282, &block35, &block36, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp283;
    compiler::TNode<JSReceiver> tmp284;
    compiler::TNode<String> tmp285;
    compiler::TNode<Number> tmp286;
    compiler::TNode<Object> tmp287;
    compiler::TNode<Object> tmp288;
    compiler::TNode<Map> tmp289;
    compiler::TNode<Int32T> tmp290;
    compiler::TNode<BuiltinPtr> tmp291;
    ca_.Bind(&block35, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 410);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 409);
    ca_.Goto(&block37, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement18ATFixedUint32Array)));
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp292;
    compiler::TNode<JSReceiver> tmp293;
    compiler::TNode<String> tmp294;
    compiler::TNode<Number> tmp295;
    compiler::TNode<Object> tmp296;
    compiler::TNode<Object> tmp297;
    compiler::TNode<Map> tmp298;
    compiler::TNode<Int32T> tmp299;
    compiler::TNode<BuiltinPtr> tmp300;
    ca_.Bind(&block36, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 412);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-join.tq:412:9");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp301;
    compiler::TNode<JSReceiver> tmp302;
    compiler::TNode<String> tmp303;
    compiler::TNode<Number> tmp304;
    compiler::TNode<Object> tmp305;
    compiler::TNode<Object> tmp306;
    compiler::TNode<Map> tmp307;
    compiler::TNode<Int32T> tmp308;
    compiler::TNode<BuiltinPtr> tmp309;
    ca_.Bind(&block37, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 407);
    ca_.Goto(&block34, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp310;
    compiler::TNode<JSReceiver> tmp311;
    compiler::TNode<String> tmp312;
    compiler::TNode<Number> tmp313;
    compiler::TNode<Object> tmp314;
    compiler::TNode<Object> tmp315;
    compiler::TNode<Map> tmp316;
    compiler::TNode<Int32T> tmp317;
    compiler::TNode<BuiltinPtr> tmp318;
    ca_.Bind(&block34, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 405);
    ca_.Goto(&block31, tmp310, tmp311, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp319;
    compiler::TNode<JSReceiver> tmp320;
    compiler::TNode<String> tmp321;
    compiler::TNode<Number> tmp322;
    compiler::TNode<Object> tmp323;
    compiler::TNode<Object> tmp324;
    compiler::TNode<Map> tmp325;
    compiler::TNode<Int32T> tmp326;
    compiler::TNode<BuiltinPtr> tmp327;
    ca_.Bind(&block31, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 403);
    ca_.Goto(&block28, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp328;
    compiler::TNode<JSReceiver> tmp329;
    compiler::TNode<String> tmp330;
    compiler::TNode<Number> tmp331;
    compiler::TNode<Object> tmp332;
    compiler::TNode<Object> tmp333;
    compiler::TNode<Map> tmp334;
    compiler::TNode<Int32T> tmp335;
    compiler::TNode<BuiltinPtr> tmp336;
    ca_.Bind(&block28, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 401);
    ca_.Goto(&block25, tmp328, tmp329, tmp330, tmp331, tmp332, tmp333, tmp334, tmp335, tmp336);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp337;
    compiler::TNode<JSReceiver> tmp338;
    compiler::TNode<String> tmp339;
    compiler::TNode<Number> tmp340;
    compiler::TNode<Object> tmp341;
    compiler::TNode<Object> tmp342;
    compiler::TNode<Map> tmp343;
    compiler::TNode<Int32T> tmp344;
    compiler::TNode<BuiltinPtr> tmp345;
    ca_.Bind(&block25, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 384);
    ca_.Goto(&block4, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp346;
    compiler::TNode<JSReceiver> tmp347;
    compiler::TNode<String> tmp348;
    compiler::TNode<Number> tmp349;
    compiler::TNode<Object> tmp350;
    compiler::TNode<Object> tmp351;
    compiler::TNode<Map> tmp352;
    compiler::TNode<Int32T> tmp353;
    compiler::TNode<BuiltinPtr> tmp354;
    ca_.Bind(&block4, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 416);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 415);
    compiler::TNode<String> tmp355;
    USE(tmp355);
    tmp355 = ca_.UncheckedCast<String>(ArrayJoinBuiltinsFromDSLAssembler(state_).ArrayJoinImpl12JSTypedArray(compiler::TNode<Context>{tmp346}, compiler::TNode<JSReceiver>{tmp347}, compiler::TNode<String>{tmp348}, compiler::TNode<Number>{tmp349}, p_useToLocaleString, compiler::TNode<Object>{tmp350}, compiler::TNode<Object>{tmp351}, compiler::TNode<BuiltinPtr>{tmp354}));
    ca_.Goto(&block1, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp355);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp356;
    compiler::TNode<JSReceiver> tmp357;
    compiler::TNode<String> tmp358;
    compiler::TNode<Number> tmp359;
    compiler::TNode<Object> tmp360;
    compiler::TNode<Object> tmp361;
    compiler::TNode<Object> tmp362;
    ca_.Bind(&block1, &tmp356, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 326);
    ca_.Goto(&block38, tmp356, tmp357, tmp358, tmp359, tmp360, tmp361, tmp362);
  }

    compiler::TNode<Context> tmp363;
    compiler::TNode<JSReceiver> tmp364;
    compiler::TNode<String> tmp365;
    compiler::TNode<Number> tmp366;
    compiler::TNode<Object> tmp367;
    compiler::TNode<Object> tmp368;
    compiler::TNode<Object> tmp369;
    ca_.Bind(&block38, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369);
  return compiler::TNode<Object>{tmp369};
}

compiler::TNode<FixedArray> ArrayJoinBuiltinsFromDSLAssembler::LoadJoinStack(compiler::TNode<Context> p_context, compiler::CodeAssemblerLabel* label_IfUninitialized) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Context, HeapObject> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Context, HeapObject> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Context, HeapObject> block6(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Context, HeapObject> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 428);
    compiler::TNode<Context> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Context>(CodeStubAssembler(state_).LoadNativeContext(compiler::TNode<Context>{tmp0}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 430);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr19ATNativeContextSlot29ATconstexpr_NativeContextSlot(Context::ARRAY_JOIN_STACK_INDEX));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadContextElement(compiler::TNode<Context>{tmp1}, compiler::TNode<IntPtrT>{tmp2}));
    compiler::TNode<HeapObject> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<HeapObject>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10HeapObject(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 429);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 431);
    compiler::TNode<Oddball> tmp5;
    USE(tmp5);
    tmp5 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp4}, compiler::TNode<HeapObject>{tmp5}));
    ca_.Branch(tmp6, &block3, &block4, tmp0, tmp1, tmp4);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Context> tmp8;
    compiler::TNode<HeapObject> tmp9;
    ca_.Bind(&block3, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block1);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Context> tmp11;
    compiler::TNode<HeapObject> tmp12;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 432);
    compiler::TNode<BoolT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsFixedArray(compiler::TNode<HeapObject>{tmp12}));
    ca_.Branch(tmp13, &block5, &block6, tmp10, tmp11, tmp12);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Context> tmp15;
    compiler::TNode<HeapObject> tmp16;
    ca_.Bind(&block6, &tmp14, &tmp15, &tmp16);
    CodeStubAssembler(state_).FailAssert("Torque assert \'IsFixedArray(stack)\' failed", "../../src/builtins/array-join.tq", 432);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<Context> tmp18;
    compiler::TNode<HeapObject> tmp19;
    ca_.Bind(&block5, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 433);
    compiler::TNode<FixedArray> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp17}, compiler::TNode<Object>{tmp19}));
    ca_.Goto(&block2, tmp17, tmp20);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<FixedArray> tmp22;
    ca_.Bind(&block2, &tmp21, &tmp22);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 426);
    ca_.Goto(&block7, tmp21, tmp22);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_IfUninitialized);
  }

    compiler::TNode<Context> tmp23;
    compiler::TNode<FixedArray> tmp24;
    ca_.Bind(&block7, &tmp23, &tmp24);
  return compiler::TNode<FixedArray>{tmp24};
}

void ArrayJoinBuiltinsFromDSLAssembler::SetJoinStack(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_stack) {
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_stack);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<FixedArray> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 437);
    compiler::TNode<Context> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Context>(CodeStubAssembler(state_).LoadNativeContext(compiler::TNode<Context>{tmp0}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 438);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr19ATNativeContextSlot29ATconstexpr_NativeContextSlot(Context::ARRAY_JOIN_STACK_INDEX));
    CodeStubAssembler(state_).StoreContextElement(compiler::TNode<Context>{tmp2}, compiler::TNode<IntPtrT>{tmp3}, compiler::TNode<Object>{tmp1});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 436);
    ca_.Goto(&block1, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp4;
    compiler::TNode<FixedArray> tmp5;
    ca_.Bind(&block1, &tmp4, &tmp5);
    ca_.Goto(&block2, tmp4, tmp5);
  }

    compiler::TNode<Context> tmp6;
    compiler::TNode<FixedArray> tmp7;
    ca_.Bind(&block2, &tmp6, &tmp7);
}

TF_BUILTIN(JoinStackPush, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<FixedArray> parameter1 = UncheckedCast<FixedArray>(Parameter(Descriptor::kStack));
  USE(parameter1);
  compiler::TNode<JSReceiver> parameter2 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<FixedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 446);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 447);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<FixedArray> tmp6;
    compiler::TNode<JSReceiver> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    compiler::TNode<IntPtrT> tmp9;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9);
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp9}, compiler::TNode<IntPtrT>{tmp8}));
    ca_.Branch(tmp10, &block1, &block2, tmp5, tmp6, tmp7, tmp8, tmp9);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<FixedArray> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    compiler::TNode<IntPtrT> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 448);
    compiler::TNode<Object> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp12}, compiler::TNode<IntPtrT>{tmp15}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 451);
    compiler::TNode<Oddball> tmp17;
    USE(tmp17);
    tmp17 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp16}, compiler::TNode<HeapObject>{tmp17}));
    ca_.Branch(tmp18, &block5, &block6, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<FixedArray> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    compiler::TNode<IntPtrT> tmp22;
    compiler::TNode<IntPtrT> tmp23;
    compiler::TNode<Object> tmp24;
    ca_.Bind(&block5, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 452);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp20}, compiler::TNode<IntPtrT>{tmp23}, compiler::TNode<HeapObject>{tmp21});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 453);
    compiler::TNode<Oddball> tmp25;
    USE(tmp25);
    tmp25 = BaseBuiltinsFromDSLAssembler(state_).True();
    CodeStubAssembler(state_).Return(tmp25);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<FixedArray> tmp27;
    compiler::TNode<JSReceiver> tmp28;
    compiler::TNode<IntPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block6, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 457);
    compiler::TNode<BoolT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp28}, compiler::TNode<Object>{tmp31}));
    ca_.Branch(tmp32, &block7, &block8, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<FixedArray> tmp34;
    compiler::TNode<JSReceiver> tmp35;
    compiler::TNode<IntPtrT> tmp36;
    compiler::TNode<IntPtrT> tmp37;
    compiler::TNode<Object> tmp38;
    ca_.Bind(&block7, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    compiler::TNode<Oddball> tmp39;
    USE(tmp39);
    tmp39 = BaseBuiltinsFromDSLAssembler(state_).False();
    CodeStubAssembler(state_).Return(tmp39);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<FixedArray> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<IntPtrT> tmp43;
    compiler::TNode<IntPtrT> tmp44;
    compiler::TNode<Object> tmp45;
    ca_.Bind(&block8, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 447);
    ca_.Goto(&block4, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp46;
    compiler::TNode<FixedArray> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    compiler::TNode<IntPtrT> tmp50;
    ca_.Bind(&block4, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    compiler::TNode<IntPtrT> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp50}, compiler::TNode<IntPtrT>{tmp51}));
    ca_.Goto(&block3, tmp46, tmp47, tmp48, tmp49, tmp52);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp53;
    compiler::TNode<FixedArray> tmp54;
    compiler::TNode<JSReceiver> tmp55;
    compiler::TNode<IntPtrT> tmp56;
    compiler::TNode<IntPtrT> tmp57;
    ca_.Bind(&block2, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 462);
    compiler::TNode<FixedArray> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray10JSReceiver(compiler::TNode<FixedArray>{tmp54}, compiler::TNode<IntPtrT>{tmp56}, compiler::TNode<JSReceiver>{tmp55}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 461);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 463);
    ArrayJoinBuiltinsFromDSLAssembler(state_).SetJoinStack(compiler::TNode<Context>{tmp53}, compiler::TNode<FixedArray>{tmp58});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 464);
    compiler::TNode<Oddball> tmp59;
    USE(tmp59);
    tmp59 = BaseBuiltinsFromDSLAssembler(state_).True();
    CodeStubAssembler(state_).Return(tmp59);
  }
}

void ArrayJoinBuiltinsFromDSLAssembler::JoinStackPushInline(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::CodeAssemblerLabel* label_ReceiverAdded, compiler::CodeAssemblerLabel* label_ReceiverNotAdded) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray> block10(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 474);
    compiler::TNode<FixedArray> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = ArrayJoinBuiltinsFromDSLAssembler(state_).LoadJoinStack(compiler::TNode<Context>{tmp0}, &label0);
    ca_.Goto(&block5, tmp0, tmp1, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp0, tmp1);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<JSReceiver> tmp4;
    ca_.Bind(&block6, &tmp3, &tmp4);
    ca_.Goto(&block4, tmp3, tmp4);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<JSReceiver> tmp6;
    compiler::TNode<FixedArray> tmp7;
    ca_.Bind(&block5, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 476);
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp7}, 0));
    compiler::TNode<Oddball> tmp9;
    USE(tmp9);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp8}, compiler::TNode<HeapObject>{tmp9}));
    ca_.Branch(tmp10, &block7, &block8, tmp5, tmp6, tmp7);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<FixedArray> tmp13;
    ca_.Bind(&block7, &tmp11, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 477);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp13}, 0, compiler::TNode<HeapObject>{tmp12});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 476);
    ca_.Goto(&block9, tmp11, tmp12, tmp13);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<FixedArray> tmp16;
    ca_.Bind(&block8, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 478);
    compiler::TNode<Oddball> tmp17;
    tmp17 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kJoinStackPush, tmp14, tmp16, tmp15));
    USE(tmp17);
    compiler::TNode<Oddball> tmp18;
    USE(tmp18);
    tmp18 = BaseBuiltinsFromDSLAssembler(state_).False();
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp17}, compiler::TNode<HeapObject>{tmp18}));
    ca_.Branch(tmp19, &block10, &block11, tmp14, tmp15, tmp16);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    compiler::TNode<FixedArray> tmp22;
    ca_.Bind(&block10, &tmp20, &tmp21, &tmp22);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 480);
    ca_.Goto(&block2);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<FixedArray> tmp25;
    ca_.Bind(&block11, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 476);
    ca_.Goto(&block9, tmp23, tmp24, tmp25);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<JSReceiver> tmp27;
    compiler::TNode<FixedArray> tmp28;
    ca_.Bind(&block9, &tmp26, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 473);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 483);
    ca_.Goto(&block3, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<JSReceiver> tmp30;
    ca_.Bind(&block4, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 485);
    compiler::TNode<IntPtrT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(JSArray::kMinJoinStackSize));
    compiler::TNode<FixedArray> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateFixedArrayWithHoles(compiler::TNode<IntPtrT>{tmp31}, CodeStubAssembler::kNone));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 484);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 486);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp32}, 0, compiler::TNode<HeapObject>{tmp30});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 487);
    ArrayJoinBuiltinsFromDSLAssembler(state_).SetJoinStack(compiler::TNode<Context>{tmp29}, compiler::TNode<FixedArray>{tmp32});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 483);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 473);
    ca_.Goto(&block3, tmp29, tmp30);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    ca_.Bind(&block3, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 489);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 470);
    ca_.Goto(label_ReceiverAdded);
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.Goto(label_ReceiverNotAdded);
  }
}

TF_BUILTIN(JoinStackPop, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<FixedArray> parameter1 = UncheckedCast<FixedArray>(Parameter(Descriptor::kStack));
  USE(parameter1);
  compiler::TNode<JSReceiver> parameter2 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, JSReceiver, IntPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<FixedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 496);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 497);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<FixedArray> tmp6;
    compiler::TNode<JSReceiver> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    compiler::TNode<IntPtrT> tmp9;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9);
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp9}, compiler::TNode<IntPtrT>{tmp8}));
    ca_.Branch(tmp10, &block1, &block2, tmp5, tmp6, tmp7, tmp8, tmp9);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<FixedArray> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    compiler::TNode<IntPtrT> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 498);
    compiler::TNode<Object> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp12}, compiler::TNode<IntPtrT>{tmp15}));
    compiler::TNode<BoolT> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp16}, compiler::TNode<HeapObject>{tmp13}));
    ca_.Branch(tmp17, &block5, &block6, tmp11, tmp12, tmp13, tmp14, tmp15);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<FixedArray> tmp19;
    compiler::TNode<JSReceiver> tmp20;
    compiler::TNode<IntPtrT> tmp21;
    compiler::TNode<IntPtrT> tmp22;
    ca_.Bind(&block5, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 501);
    compiler::TNode<IntPtrT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp22}, compiler::TNode<IntPtrT>{tmp23}));
    ca_.Branch(tmp24, &block9, &block8, tmp18, tmp19, tmp20, tmp21, tmp22);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<FixedArray> tmp26;
    compiler::TNode<JSReceiver> tmp27;
    compiler::TNode<IntPtrT> tmp28;
    compiler::TNode<IntPtrT> tmp29;
    ca_.Bind(&block9, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    compiler::TNode<IntPtrT> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(JSArray::kMinJoinStackSize));
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp28}, compiler::TNode<IntPtrT>{tmp30}));
    ca_.Branch(tmp31, &block7, &block8, tmp25, tmp26, tmp27, tmp28, tmp29);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<FixedArray> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    compiler::TNode<IntPtrT> tmp35;
    compiler::TNode<IntPtrT> tmp36;
    ca_.Bind(&block7, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 503);
    compiler::TNode<IntPtrT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(JSArray::kMinJoinStackSize));
    compiler::TNode<FixedArray> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateFixedArrayWithHoles(compiler::TNode<IntPtrT>{tmp37}, CodeStubAssembler::kNone));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 502);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 504);
    ArrayJoinBuiltinsFromDSLAssembler(state_).SetJoinStack(compiler::TNode<Context>{tmp32}, compiler::TNode<FixedArray>{tmp38});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 501);
    ca_.Goto(&block10, tmp32, tmp33, tmp34, tmp35, tmp36);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<FixedArray> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    compiler::TNode<IntPtrT> tmp42;
    compiler::TNode<IntPtrT> tmp43;
    ca_.Bind(&block8, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 507);
    compiler::TNode<Oddball> tmp44;
    USE(tmp44);
    tmp44 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp40}, compiler::TNode<IntPtrT>{tmp43}, compiler::TNode<HeapObject>{tmp44});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 501);
    ca_.Goto(&block10, tmp39, tmp40, tmp41, tmp42, tmp43);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<FixedArray> tmp46;
    compiler::TNode<JSReceiver> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    ca_.Bind(&block10, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 509);
    compiler::TNode<Oddball> tmp50;
    USE(tmp50);
    tmp50 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp50);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<FixedArray> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    compiler::TNode<IntPtrT> tmp54;
    compiler::TNode<IntPtrT> tmp55;
    ca_.Bind(&block6, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 497);
    ca_.Goto(&block4, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<FixedArray> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    compiler::TNode<IntPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    ca_.Bind(&block4, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    compiler::TNode<IntPtrT> tmp61;
    USE(tmp61);
    tmp61 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp61}));
    ca_.Goto(&block3, tmp56, tmp57, tmp58, tmp59, tmp62);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<FixedArray> tmp64;
    compiler::TNode<JSReceiver> tmp65;
    compiler::TNode<IntPtrT> tmp66;
    compiler::TNode<IntPtrT> tmp67;
    ca_.Bind(&block2, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 512);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-join.tq:512:5");
    CodeStubAssembler(state_).Unreachable();
  }
}

void ArrayJoinBuiltinsFromDSLAssembler::JoinStackPopInline(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray, IntPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, FixedArray, IntPtrT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 517);
    compiler::TNode<FixedArray> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = ArrayJoinBuiltinsFromDSLAssembler(state_).LoadJoinStack(compiler::TNode<Context>{tmp0}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<JSReceiver> tmp4;
    ca_.Bind(&block5, &tmp3, &tmp4);
    ca_.Goto(&block3, tmp3, tmp4);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<JSReceiver> tmp6;
    compiler::TNode<FixedArray> tmp7;
    ca_.Bind(&block4, &tmp5, &tmp6, &tmp7);
    ca_.Goto(&block2, tmp5, tmp6, tmp7);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<JSReceiver> tmp9;
    ca_.Bind(&block3, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 518);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-join.tq:518:19");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<FixedArray> tmp12;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 517);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 519);
    compiler::TNode<IntPtrT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp12}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 523);
    compiler::TNode<Object> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp12}, 0));
    compiler::TNode<BoolT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp14}, compiler::TNode<HeapObject>{tmp11}));
    ca_.Branch(tmp15, &block8, &block7, tmp10, tmp11, tmp12, tmp13);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<FixedArray> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    ca_.Bind(&block8, &tmp16, &tmp17, &tmp18, &tmp19);
    compiler::TNode<IntPtrT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(JSArray::kMinJoinStackSize));
    compiler::TNode<BoolT> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp19}, compiler::TNode<IntPtrT>{tmp20}));
    ca_.Branch(tmp21, &block6, &block7, tmp16, tmp17, tmp18, tmp19);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<JSReceiver> tmp23;
    compiler::TNode<FixedArray> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    ca_.Bind(&block6, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 524);
    compiler::TNode<Oddball> tmp26;
    USE(tmp26);
    tmp26 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<IntPtrT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp24}, compiler::TNode<IntPtrT>{tmp27}, compiler::TNode<Object>{tmp26}, SKIP_WRITE_BARRIER);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 523);
    ca_.Goto(&block9, tmp22, tmp23, tmp24, tmp25);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<FixedArray> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    ca_.Bind(&block7, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 527);
    compiler::TNode<Object> tmp32;
    tmp32 = CodeStubAssembler(state_).CallBuiltin(Builtins::kJoinStackPop, tmp28, tmp30, tmp29);
    USE(tmp32);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 523);
    ca_.Goto(&block9, tmp28, tmp29, tmp30, tmp31);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    compiler::TNode<FixedArray> tmp35;
    compiler::TNode<IntPtrT> tmp36;
    ca_.Bind(&block9, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 516);
    ca_.Goto(&block1, tmp33, tmp34);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<JSReceiver> tmp38;
    ca_.Bind(&block1, &tmp37, &tmp38);
    ca_.Goto(&block10, tmp37, tmp38);
  }

    compiler::TNode<Context> tmp39;
    compiler::TNode<JSReceiver> tmp40;
    ca_.Bind(&block10, &tmp39, &tmp40);
}

TF_BUILTIN(ArrayPrototypeJoin, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSReceiver, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSReceiver, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSReceiver, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSReceiver, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 561);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 564);
    compiler::TNode<JSReceiver> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 567);
    compiler::TNode<Number> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 571);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber18ATconstexpr_uint32((CodeStubAssembler(state_).ConstexprUint32Add(JSArray::kMaxArrayIndex, 1))));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp5}, compiler::TNode<Number>{tmp6}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block3, tmp0, tmp1, tmp3, tmp4, tmp5, tmp5);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block4, tmp0, tmp1, tmp3, tmp4, tmp5, tmp5);
    }
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<Number> tmp11;
    compiler::TNode<Number> tmp12;
    ca_.Bind(&block3, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.Goto(&block1, tmp7, tmp8, tmp9, tmp10, tmp11);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<Number> tmp18;
    ca_.Bind(&block4, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.Goto(&block2, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<Number> tmp23;
    ca_.Bind(&block1, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp19}, MessageTemplate::kInvalidArrayLength);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<JSReceiver> tmp27;
    compiler::TNode<Number> tmp28;
    ca_.Bind(&block2, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 574);
    compiler::TNode<Oddball> tmp29;
    USE(tmp29);
    tmp29 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Oddball> tmp30;
    USE(tmp30);
    tmp30 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 573);
    compiler::TNode<Object> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).CycleProtectedArrayJoin7JSArray(compiler::TNode<Context>{tmp24}, false, compiler::TNode<JSReceiver>{tmp27}, compiler::TNode<Number>{tmp28}, compiler::TNode<Object>{tmp26}, compiler::TNode<Object>{tmp29}, compiler::TNode<Object>{tmp30}));
    arguments->PopAndReturn(tmp31);
  }
}

TF_BUILTIN(ArrayPrototypeToLocaleString, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 580);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 581);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 584);
    compiler::TNode<JSReceiver> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 587);
    compiler::TNode<Number> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 591);
    compiler::TNode<Number> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber18ATconstexpr_uint32((CodeStubAssembler(state_).ConstexprUint32Add(JSArray::kMaxArrayIndex, 1))));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp7}, compiler::TNode<Number>{tmp8}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block3, tmp0, tmp1, tmp3, tmp5, tmp6, tmp7, tmp7);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block4, tmp0, tmp1, tmp3, tmp5, tmp6, tmp7, tmp7);
    }
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    compiler::TNode<Number> tmp14;
    compiler::TNode<Number> tmp15;
    ca_.Bind(&block3, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.Goto(&block1, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<JSReceiver> tmp20;
    compiler::TNode<Number> tmp21;
    compiler::TNode<Number> tmp22;
    ca_.Bind(&block4, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block2, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<JSReceiver> tmp27;
    compiler::TNode<Number> tmp28;
    ca_.Bind(&block1, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp23}, MessageTemplate::kInvalidArrayLength);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<Number> tmp34;
    ca_.Bind(&block2, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 594);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 593);
    compiler::TNode<Object> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapObject18ATconstexpr_string(","));
    compiler::TNode<Object> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).CycleProtectedArrayJoin7JSArray(compiler::TNode<Context>{tmp29}, true, compiler::TNode<JSReceiver>{tmp33}, compiler::TNode<Number>{tmp34}, compiler::TNode<Object>{tmp35}, compiler::TNode<Object>{tmp31}, compiler::TNode<Object>{tmp32}));
    arguments->PopAndReturn(tmp36);
  }
}

TF_BUILTIN(ArrayPrototypeToString, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Object, Object, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 601);
    compiler::TNode<JSReceiver> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 604);
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapObject18ATconstexpr_string("join"));
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}, compiler::TNode<Object>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 608);
    compiler::TNode<JSReceiver> tmp5;
    USE(tmp5);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp5 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp4, tmp4, tmp5);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp4, tmp4);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    ca_.Bind(&block4, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.Goto(&block2, tmp6, tmp7, tmp8, tmp9);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    ca_.Bind(&block3, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 611);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp11}, compiler::TNode<JSReceiver>{tmp16}, compiler::TNode<Object>{tmp13}));
    arguments->PopAndReturn(tmp17);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<JSReceiver> tmp20;
    compiler::TNode<Object> tmp21;
    ca_.Bind(&block2, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 614);
    compiler::TNode<Object> tmp22;
    tmp22 = CodeStubAssembler(state_).CallBuiltin(Builtins::kObjectToString, tmp18, tmp20);
    USE(tmp22);
    arguments->PopAndReturn(tmp22);
  }
}

TF_BUILTIN(TypedArrayPrototypeJoin, CodeStubAssembler) {
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
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 621);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 626);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 625);
    compiler::TNode<JSTypedArray> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSTypedArray>(TypedArrayBuiltinsAssembler(state_).ValidateTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, "%TypedArray%.prototype.join"));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 627);
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadJSTypedArrayLength(compiler::TNode<JSTypedArray>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 630);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 629);
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).CycleProtectedArrayJoin12JSTypedArray(compiler::TNode<Context>{tmp0}, false, compiler::TNode<JSReceiver>{tmp4}, compiler::TNode<Number>{tmp5}, compiler::TNode<Object>{tmp3}, compiler::TNode<Object>{tmp6}, compiler::TNode<Object>{tmp7}));
    arguments->PopAndReturn(tmp8);
  }
}

TF_BUILTIN(TypedArrayPrototypeToLocaleString, CodeStubAssembler) {
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
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 636);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 637);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 642);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 641);
    compiler::TNode<JSTypedArray> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<JSTypedArray>(TypedArrayBuiltinsAssembler(state_).ValidateTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, "%TypedArray%.prototype.toLocaleString"));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 643);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadJSTypedArrayLength(compiler::TNode<JSTypedArray>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 646);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 645);
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapObject18ATconstexpr_string(","));
    compiler::TNode<Object> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).CycleProtectedArrayJoin12JSTypedArray(compiler::TNode<Context>{tmp0}, true, compiler::TNode<JSReceiver>{tmp6}, compiler::TNode<Number>{tmp7}, compiler::TNode<Object>{tmp8}, compiler::TNode<Object>{tmp3}, compiler::TNode<Object>{tmp5}));
    arguments->PopAndReturn(tmp9);
  }
}

TF_BUILTIN(LoadJoinElement25ATGenericElementsAccessor, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 16);
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, compiler::TNode<Object>{tmp2}));
    CodeStubAssembler(state_).Return(tmp3);
  }
}

compiler::TNode<String> ArrayJoinBuiltinsFromDSLAssembler::ArrayJoinImpl7JSArray(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<String> p_sep, compiler::TNode<Number> p_lengthNumber, bool p_useToLocaleString, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options, compiler::TNode<BuiltinPtr> p_initialLoadFn) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, BuiltinPtr, JSReceiver, Map, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, BuiltinPtr, JSReceiver, Map, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, Object> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, Object, String> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, String> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, String> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, Object> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, Object, Number> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, HeapObject> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, HeapObject> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block33(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block35(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block41(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block43(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block44(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, String> block45(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver, p_sep, p_lengthNumber, p_locales, p_options, p_initialLoadFn);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<String> tmp2;
    compiler::TNode<Number> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<BuiltinPtr> tmp6;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 266);
    compiler::TNode<Map> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Map>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapObjectMap(compiler::TNode<HeapObject>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 267);
    compiler::TNode<UintPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr20UT5ATSmi10HeapNumber(compiler::TNode<Number>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 268);
    compiler::TNode<IntPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadStringLengthAsWord(compiler::TNode<String>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 269);
    compiler::TNode<IntPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 270);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 271);
    compiler::TNode<FixedArray> tmp11;
    USE(tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    compiler::TNode<IntPtrT> tmp13;
    USE(tmp13);
    compiler::TNode<BoolT> tmp14;
    USE(tmp14);
    std::tie(tmp11, tmp12, tmp13, tmp14) = ArrayJoinBuiltinsFromDSLAssembler(state_).NewBuffer(compiler::TNode<UintPtrT>{tmp8}, compiler::TNode<String>{tmp2}).Flatten();
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 274);
    compiler::TNode<UintPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 277);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp6, tmp11, tmp12, tmp13, tmp14, tmp15);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<String> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<BuiltinPtr> tmp22;
    compiler::TNode<Map> tmp23;
    compiler::TNode<UintPtrT> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<BuiltinPtr> tmp27;
    compiler::TNode<FixedArray> tmp28;
    compiler::TNode<IntPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<BoolT> tmp31;
    compiler::TNode<UintPtrT> tmp32;
    ca_.Bind(&block4, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32);
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrLessThan(compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<UintPtrT>{tmp24}));
    ca_.Branch(tmp33, &block2, &block3, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<JSReceiver> tmp35;
    compiler::TNode<String> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<BuiltinPtr> tmp40;
    compiler::TNode<Map> tmp41;
    compiler::TNode<UintPtrT> tmp42;
    compiler::TNode<IntPtrT> tmp43;
    compiler::TNode<IntPtrT> tmp44;
    compiler::TNode<BuiltinPtr> tmp45;
    compiler::TNode<FixedArray> tmp46;
    compiler::TNode<IntPtrT> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<BoolT> tmp49;
    compiler::TNode<UintPtrT> tmp50;
    ca_.Bind(&block2, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 279);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 278);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    ArrayJoinBuiltinsFromDSLAssembler(state_).CannotUseSameArrayAccessor7JSArray(compiler::TNode<Context>{tmp34}, compiler::TNode<BuiltinPtr>{tmp45}, compiler::TNode<JSReceiver>{tmp35}, compiler::TNode<Map>{tmp41}, compiler::TNode<Number>{tmp37}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp45, tmp35, tmp41, tmp37);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp45, tmp35, tmp41, tmp37);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<JSReceiver> tmp52;
    compiler::TNode<String> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<BuiltinPtr> tmp57;
    compiler::TNode<Map> tmp58;
    compiler::TNode<UintPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<IntPtrT> tmp61;
    compiler::TNode<BuiltinPtr> tmp62;
    compiler::TNode<FixedArray> tmp63;
    compiler::TNode<IntPtrT> tmp64;
    compiler::TNode<IntPtrT> tmp65;
    compiler::TNode<BoolT> tmp66;
    compiler::TNode<UintPtrT> tmp67;
    compiler::TNode<BuiltinPtr> tmp68;
    compiler::TNode<JSReceiver> tmp69;
    compiler::TNode<Map> tmp70;
    compiler::TNode<Number> tmp71;
    ca_.Bind(&block7, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    ca_.Goto(&block5, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<JSReceiver> tmp73;
    compiler::TNode<String> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<BuiltinPtr> tmp78;
    compiler::TNode<Map> tmp79;
    compiler::TNode<UintPtrT> tmp80;
    compiler::TNode<IntPtrT> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    compiler::TNode<BuiltinPtr> tmp83;
    compiler::TNode<FixedArray> tmp84;
    compiler::TNode<IntPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<BoolT> tmp87;
    compiler::TNode<UintPtrT> tmp88;
    compiler::TNode<BuiltinPtr> tmp89;
    compiler::TNode<JSReceiver> tmp90;
    compiler::TNode<Map> tmp91;
    compiler::TNode<Number> tmp92;
    ca_.Bind(&block8, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.Goto(&block6, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp93;
    compiler::TNode<JSReceiver> tmp94;
    compiler::TNode<String> tmp95;
    compiler::TNode<Number> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<BuiltinPtr> tmp99;
    compiler::TNode<Map> tmp100;
    compiler::TNode<UintPtrT> tmp101;
    compiler::TNode<IntPtrT> tmp102;
    compiler::TNode<IntPtrT> tmp103;
    compiler::TNode<BuiltinPtr> tmp104;
    compiler::TNode<FixedArray> tmp105;
    compiler::TNode<IntPtrT> tmp106;
    compiler::TNode<IntPtrT> tmp107;
    compiler::TNode<BoolT> tmp108;
    compiler::TNode<UintPtrT> tmp109;
    ca_.Bind(&block5, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 281);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 278);
    ca_.Goto(&block6, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement25ATGenericElementsAccessor)), tmp105, tmp106, tmp107, tmp108, tmp109);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<JSReceiver> tmp111;
    compiler::TNode<String> tmp112;
    compiler::TNode<Number> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<Object> tmp115;
    compiler::TNode<BuiltinPtr> tmp116;
    compiler::TNode<Map> tmp117;
    compiler::TNode<UintPtrT> tmp118;
    compiler::TNode<IntPtrT> tmp119;
    compiler::TNode<IntPtrT> tmp120;
    compiler::TNode<BuiltinPtr> tmp121;
    compiler::TNode<FixedArray> tmp122;
    compiler::TNode<IntPtrT> tmp123;
    compiler::TNode<IntPtrT> tmp124;
    compiler::TNode<BoolT> tmp125;
    compiler::TNode<UintPtrT> tmp126;
    ca_.Bind(&block6, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 284);
    compiler::TNode<UintPtrT> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp128;
    USE(tmp128);
    tmp128 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrGreaterThan(compiler::TNode<UintPtrT>{tmp126}, compiler::TNode<UintPtrT>{tmp127}));
    ca_.Branch(tmp128, &block9, &block10, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp129;
    compiler::TNode<JSReceiver> tmp130;
    compiler::TNode<String> tmp131;
    compiler::TNode<Number> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<BuiltinPtr> tmp135;
    compiler::TNode<Map> tmp136;
    compiler::TNode<UintPtrT> tmp137;
    compiler::TNode<IntPtrT> tmp138;
    compiler::TNode<IntPtrT> tmp139;
    compiler::TNode<BuiltinPtr> tmp140;
    compiler::TNode<FixedArray> tmp141;
    compiler::TNode<IntPtrT> tmp142;
    compiler::TNode<IntPtrT> tmp143;
    compiler::TNode<BoolT> tmp144;
    compiler::TNode<UintPtrT> tmp145;
    ca_.Bind(&block9, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 286);
    compiler::TNode<IntPtrT> tmp146;
    USE(tmp146);
    tmp146 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp147;
    USE(tmp147);
    tmp147 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp139}, compiler::TNode<IntPtrT>{tmp146}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 284);
    ca_.Goto(&block10, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp147, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp148;
    compiler::TNode<JSReceiver> tmp149;
    compiler::TNode<String> tmp150;
    compiler::TNode<Number> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<BuiltinPtr> tmp154;
    compiler::TNode<Map> tmp155;
    compiler::TNode<UintPtrT> tmp156;
    compiler::TNode<IntPtrT> tmp157;
    compiler::TNode<IntPtrT> tmp158;
    compiler::TNode<BuiltinPtr> tmp159;
    compiler::TNode<FixedArray> tmp160;
    compiler::TNode<IntPtrT> tmp161;
    compiler::TNode<IntPtrT> tmp162;
    compiler::TNode<BoolT> tmp163;
    compiler::TNode<UintPtrT> tmp164;
    ca_.Bind(&block10, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 290);
    compiler::TNode<UintPtrT> tmp165;
    USE(tmp165);
    tmp165 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp166;
    USE(tmp166);
    tmp166 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp164}, compiler::TNode<UintPtrT>{tmp165}));
    compiler::TNode<Number> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATuintptr(compiler::TNode<UintPtrT>{tmp164}));
    compiler::TNode<Object> tmp168 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(0)).descriptor(), tmp159, tmp148, tmp149, tmp167); 
    USE(tmp168);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 294);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 295);
    if ((p_useToLocaleString)) {
      ca_.Goto(&block11, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp166, tmp168, ca_.Uninitialized<String>());
    } else {
      ca_.Goto(&block12, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp166, tmp168, ca_.Uninitialized<String>());
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp169;
    compiler::TNode<JSReceiver> tmp170;
    compiler::TNode<String> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<Object> tmp174;
    compiler::TNode<BuiltinPtr> tmp175;
    compiler::TNode<Map> tmp176;
    compiler::TNode<UintPtrT> tmp177;
    compiler::TNode<IntPtrT> tmp178;
    compiler::TNode<IntPtrT> tmp179;
    compiler::TNode<BuiltinPtr> tmp180;
    compiler::TNode<FixedArray> tmp181;
    compiler::TNode<IntPtrT> tmp182;
    compiler::TNode<IntPtrT> tmp183;
    compiler::TNode<BoolT> tmp184;
    compiler::TNode<UintPtrT> tmp185;
    compiler::TNode<Object> tmp186;
    compiler::TNode<String> tmp187;
    ca_.Bind(&block11, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 296);
    compiler::TNode<String> tmp188;
    tmp188 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kConvertToLocaleString, tmp169, tmp186, tmp173, tmp174));
    USE(tmp188);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 297);
    compiler::TNode<String> tmp189;
    USE(tmp189);
    tmp189 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp190;
    USE(tmp190);
    tmp190 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp188}, compiler::TNode<HeapObject>{tmp189}));
    ca_.Branch(tmp190, &block14, &block15, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp188);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp191;
    compiler::TNode<JSReceiver> tmp192;
    compiler::TNode<String> tmp193;
    compiler::TNode<Number> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<BuiltinPtr> tmp197;
    compiler::TNode<Map> tmp198;
    compiler::TNode<UintPtrT> tmp199;
    compiler::TNode<IntPtrT> tmp200;
    compiler::TNode<IntPtrT> tmp201;
    compiler::TNode<BuiltinPtr> tmp202;
    compiler::TNode<FixedArray> tmp203;
    compiler::TNode<IntPtrT> tmp204;
    compiler::TNode<IntPtrT> tmp205;
    compiler::TNode<BoolT> tmp206;
    compiler::TNode<UintPtrT> tmp207;
    compiler::TNode<Object> tmp208;
    compiler::TNode<String> tmp209;
    ca_.Bind(&block14, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209);
    ca_.Goto(&block4, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp210;
    compiler::TNode<JSReceiver> tmp211;
    compiler::TNode<String> tmp212;
    compiler::TNode<Number> tmp213;
    compiler::TNode<Object> tmp214;
    compiler::TNode<Object> tmp215;
    compiler::TNode<BuiltinPtr> tmp216;
    compiler::TNode<Map> tmp217;
    compiler::TNode<UintPtrT> tmp218;
    compiler::TNode<IntPtrT> tmp219;
    compiler::TNode<IntPtrT> tmp220;
    compiler::TNode<BuiltinPtr> tmp221;
    compiler::TNode<FixedArray> tmp222;
    compiler::TNode<IntPtrT> tmp223;
    compiler::TNode<IntPtrT> tmp224;
    compiler::TNode<BoolT> tmp225;
    compiler::TNode<UintPtrT> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<String> tmp228;
    ca_.Bind(&block15, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 295);
    ca_.Goto(&block13, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp229;
    compiler::TNode<JSReceiver> tmp230;
    compiler::TNode<String> tmp231;
    compiler::TNode<Number> tmp232;
    compiler::TNode<Object> tmp233;
    compiler::TNode<Object> tmp234;
    compiler::TNode<BuiltinPtr> tmp235;
    compiler::TNode<Map> tmp236;
    compiler::TNode<UintPtrT> tmp237;
    compiler::TNode<IntPtrT> tmp238;
    compiler::TNode<IntPtrT> tmp239;
    compiler::TNode<BuiltinPtr> tmp240;
    compiler::TNode<FixedArray> tmp241;
    compiler::TNode<IntPtrT> tmp242;
    compiler::TNode<IntPtrT> tmp243;
    compiler::TNode<BoolT> tmp244;
    compiler::TNode<UintPtrT> tmp245;
    compiler::TNode<Object> tmp246;
    compiler::TNode<String> tmp247;
    ca_.Bind(&block12, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 299);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 300);
    compiler::TNode<String> tmp248;
    USE(tmp248);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp248 = BaseBuiltinsFromDSLAssembler(state_).Cast6String(compiler::TNode<Context>{tmp229}, compiler::TNode<Object>{tmp246}, &label0);
    ca_.Goto(&block18, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp246, tmp246, tmp248);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block19, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp246, tmp246);
    }
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp249;
    compiler::TNode<JSReceiver> tmp250;
    compiler::TNode<String> tmp251;
    compiler::TNode<Number> tmp252;
    compiler::TNode<Object> tmp253;
    compiler::TNode<Object> tmp254;
    compiler::TNode<BuiltinPtr> tmp255;
    compiler::TNode<Map> tmp256;
    compiler::TNode<UintPtrT> tmp257;
    compiler::TNode<IntPtrT> tmp258;
    compiler::TNode<IntPtrT> tmp259;
    compiler::TNode<BuiltinPtr> tmp260;
    compiler::TNode<FixedArray> tmp261;
    compiler::TNode<IntPtrT> tmp262;
    compiler::TNode<IntPtrT> tmp263;
    compiler::TNode<BoolT> tmp264;
    compiler::TNode<UintPtrT> tmp265;
    compiler::TNode<Object> tmp266;
    compiler::TNode<String> tmp267;
    compiler::TNode<Object> tmp268;
    compiler::TNode<Object> tmp269;
    ca_.Bind(&block19, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269);
    ca_.Goto(&block17, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp270;
    compiler::TNode<JSReceiver> tmp271;
    compiler::TNode<String> tmp272;
    compiler::TNode<Number> tmp273;
    compiler::TNode<Object> tmp274;
    compiler::TNode<Object> tmp275;
    compiler::TNode<BuiltinPtr> tmp276;
    compiler::TNode<Map> tmp277;
    compiler::TNode<UintPtrT> tmp278;
    compiler::TNode<IntPtrT> tmp279;
    compiler::TNode<IntPtrT> tmp280;
    compiler::TNode<BuiltinPtr> tmp281;
    compiler::TNode<FixedArray> tmp282;
    compiler::TNode<IntPtrT> tmp283;
    compiler::TNode<IntPtrT> tmp284;
    compiler::TNode<BoolT> tmp285;
    compiler::TNode<UintPtrT> tmp286;
    compiler::TNode<Object> tmp287;
    compiler::TNode<String> tmp288;
    compiler::TNode<Object> tmp289;
    compiler::TNode<Object> tmp290;
    compiler::TNode<String> tmp291;
    ca_.Bind(&block18, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 301);
    compiler::TNode<String> tmp292;
    USE(tmp292);
    tmp292 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp293;
    USE(tmp293);
    tmp293 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp291}, compiler::TNode<HeapObject>{tmp292}));
    ca_.Branch(tmp293, &block20, &block21, tmp270, tmp271, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp291);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp294;
    compiler::TNode<JSReceiver> tmp295;
    compiler::TNode<String> tmp296;
    compiler::TNode<Number> tmp297;
    compiler::TNode<Object> tmp298;
    compiler::TNode<Object> tmp299;
    compiler::TNode<BuiltinPtr> tmp300;
    compiler::TNode<Map> tmp301;
    compiler::TNode<UintPtrT> tmp302;
    compiler::TNode<IntPtrT> tmp303;
    compiler::TNode<IntPtrT> tmp304;
    compiler::TNode<BuiltinPtr> tmp305;
    compiler::TNode<FixedArray> tmp306;
    compiler::TNode<IntPtrT> tmp307;
    compiler::TNode<IntPtrT> tmp308;
    compiler::TNode<BoolT> tmp309;
    compiler::TNode<UintPtrT> tmp310;
    compiler::TNode<Object> tmp311;
    compiler::TNode<String> tmp312;
    compiler::TNode<Object> tmp313;
    compiler::TNode<String> tmp314;
    ca_.Bind(&block20, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314);
    ca_.Goto(&block4, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp315;
    compiler::TNode<JSReceiver> tmp316;
    compiler::TNode<String> tmp317;
    compiler::TNode<Number> tmp318;
    compiler::TNode<Object> tmp319;
    compiler::TNode<Object> tmp320;
    compiler::TNode<BuiltinPtr> tmp321;
    compiler::TNode<Map> tmp322;
    compiler::TNode<UintPtrT> tmp323;
    compiler::TNode<IntPtrT> tmp324;
    compiler::TNode<IntPtrT> tmp325;
    compiler::TNode<BuiltinPtr> tmp326;
    compiler::TNode<FixedArray> tmp327;
    compiler::TNode<IntPtrT> tmp328;
    compiler::TNode<IntPtrT> tmp329;
    compiler::TNode<BoolT> tmp330;
    compiler::TNode<UintPtrT> tmp331;
    compiler::TNode<Object> tmp332;
    compiler::TNode<String> tmp333;
    compiler::TNode<Object> tmp334;
    compiler::TNode<String> tmp335;
    ca_.Bind(&block21, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 302);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 300);
    ca_.Goto(&block16, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp335, tmp334);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp336;
    compiler::TNode<JSReceiver> tmp337;
    compiler::TNode<String> tmp338;
    compiler::TNode<Number> tmp339;
    compiler::TNode<Object> tmp340;
    compiler::TNode<Object> tmp341;
    compiler::TNode<BuiltinPtr> tmp342;
    compiler::TNode<Map> tmp343;
    compiler::TNode<UintPtrT> tmp344;
    compiler::TNode<IntPtrT> tmp345;
    compiler::TNode<IntPtrT> tmp346;
    compiler::TNode<BuiltinPtr> tmp347;
    compiler::TNode<FixedArray> tmp348;
    compiler::TNode<IntPtrT> tmp349;
    compiler::TNode<IntPtrT> tmp350;
    compiler::TNode<BoolT> tmp351;
    compiler::TNode<UintPtrT> tmp352;
    compiler::TNode<Object> tmp353;
    compiler::TNode<String> tmp354;
    compiler::TNode<Object> tmp355;
    ca_.Bind(&block17, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 304);
    compiler::TNode<Number> tmp356;
    USE(tmp356);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp356 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp355)}, &label0);
    ca_.Goto(&block24, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, ca_.UncheckedCast<Object>(tmp355), tmp356);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block25, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, ca_.UncheckedCast<Object>(tmp355));
    }
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp357;
    compiler::TNode<JSReceiver> tmp358;
    compiler::TNode<String> tmp359;
    compiler::TNode<Number> tmp360;
    compiler::TNode<Object> tmp361;
    compiler::TNode<Object> tmp362;
    compiler::TNode<BuiltinPtr> tmp363;
    compiler::TNode<Map> tmp364;
    compiler::TNode<UintPtrT> tmp365;
    compiler::TNode<IntPtrT> tmp366;
    compiler::TNode<IntPtrT> tmp367;
    compiler::TNode<BuiltinPtr> tmp368;
    compiler::TNode<FixedArray> tmp369;
    compiler::TNode<IntPtrT> tmp370;
    compiler::TNode<IntPtrT> tmp371;
    compiler::TNode<BoolT> tmp372;
    compiler::TNode<UintPtrT> tmp373;
    compiler::TNode<Object> tmp374;
    compiler::TNode<String> tmp375;
    compiler::TNode<Object> tmp376;
    compiler::TNode<Object> tmp377;
    ca_.Bind(&block25, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377);
    ca_.Goto(&block23, tmp357, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp378;
    compiler::TNode<JSReceiver> tmp379;
    compiler::TNode<String> tmp380;
    compiler::TNode<Number> tmp381;
    compiler::TNode<Object> tmp382;
    compiler::TNode<Object> tmp383;
    compiler::TNode<BuiltinPtr> tmp384;
    compiler::TNode<Map> tmp385;
    compiler::TNode<UintPtrT> tmp386;
    compiler::TNode<IntPtrT> tmp387;
    compiler::TNode<IntPtrT> tmp388;
    compiler::TNode<BuiltinPtr> tmp389;
    compiler::TNode<FixedArray> tmp390;
    compiler::TNode<IntPtrT> tmp391;
    compiler::TNode<IntPtrT> tmp392;
    compiler::TNode<BoolT> tmp393;
    compiler::TNode<UintPtrT> tmp394;
    compiler::TNode<Object> tmp395;
    compiler::TNode<String> tmp396;
    compiler::TNode<Object> tmp397;
    compiler::TNode<Object> tmp398;
    compiler::TNode<Number> tmp399;
    ca_.Bind(&block24, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 305);
    compiler::TNode<String> tmp400;
    USE(tmp400);
    tmp400 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).NumberToString(compiler::TNode<Number>{tmp399}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 304);
    ca_.Goto(&block22, tmp378, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp400, tmp397);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp401;
    compiler::TNode<JSReceiver> tmp402;
    compiler::TNode<String> tmp403;
    compiler::TNode<Number> tmp404;
    compiler::TNode<Object> tmp405;
    compiler::TNode<Object> tmp406;
    compiler::TNode<BuiltinPtr> tmp407;
    compiler::TNode<Map> tmp408;
    compiler::TNode<UintPtrT> tmp409;
    compiler::TNode<IntPtrT> tmp410;
    compiler::TNode<IntPtrT> tmp411;
    compiler::TNode<BuiltinPtr> tmp412;
    compiler::TNode<FixedArray> tmp413;
    compiler::TNode<IntPtrT> tmp414;
    compiler::TNode<IntPtrT> tmp415;
    compiler::TNode<BoolT> tmp416;
    compiler::TNode<UintPtrT> tmp417;
    compiler::TNode<Object> tmp418;
    compiler::TNode<String> tmp419;
    compiler::TNode<Object> tmp420;
    ca_.Bind(&block23, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 307);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 308);
    compiler::TNode<BoolT> tmp421;
    USE(tmp421);
    tmp421 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{ca_.UncheckedCast<HeapObject>(tmp420)}));
    ca_.Branch(tmp421, &block26, &block27, tmp401, tmp402, tmp403, tmp404, tmp405, tmp406, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420, ca_.UncheckedCast<HeapObject>(tmp420));
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp422;
    compiler::TNode<JSReceiver> tmp423;
    compiler::TNode<String> tmp424;
    compiler::TNode<Number> tmp425;
    compiler::TNode<Object> tmp426;
    compiler::TNode<Object> tmp427;
    compiler::TNode<BuiltinPtr> tmp428;
    compiler::TNode<Map> tmp429;
    compiler::TNode<UintPtrT> tmp430;
    compiler::TNode<IntPtrT> tmp431;
    compiler::TNode<IntPtrT> tmp432;
    compiler::TNode<BuiltinPtr> tmp433;
    compiler::TNode<FixedArray> tmp434;
    compiler::TNode<IntPtrT> tmp435;
    compiler::TNode<IntPtrT> tmp436;
    compiler::TNode<BoolT> tmp437;
    compiler::TNode<UintPtrT> tmp438;
    compiler::TNode<Object> tmp439;
    compiler::TNode<String> tmp440;
    compiler::TNode<Object> tmp441;
    compiler::TNode<HeapObject> tmp442;
    ca_.Bind(&block26, &tmp422, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442);
    ca_.Goto(&block4, tmp422, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp443;
    compiler::TNode<JSReceiver> tmp444;
    compiler::TNode<String> tmp445;
    compiler::TNode<Number> tmp446;
    compiler::TNode<Object> tmp447;
    compiler::TNode<Object> tmp448;
    compiler::TNode<BuiltinPtr> tmp449;
    compiler::TNode<Map> tmp450;
    compiler::TNode<UintPtrT> tmp451;
    compiler::TNode<IntPtrT> tmp452;
    compiler::TNode<IntPtrT> tmp453;
    compiler::TNode<BuiltinPtr> tmp454;
    compiler::TNode<FixedArray> tmp455;
    compiler::TNode<IntPtrT> tmp456;
    compiler::TNode<IntPtrT> tmp457;
    compiler::TNode<BoolT> tmp458;
    compiler::TNode<UintPtrT> tmp459;
    compiler::TNode<Object> tmp460;
    compiler::TNode<String> tmp461;
    compiler::TNode<Object> tmp462;
    compiler::TNode<HeapObject> tmp463;
    ca_.Bind(&block27, &tmp443, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 309);
    compiler::TNode<String> tmp464;
    tmp464 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kToString, tmp443, tmp463));
    USE(tmp464);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 304);
    ca_.Goto(&block22, tmp443, tmp444, tmp445, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452, tmp453, tmp454, tmp455, tmp456, tmp457, tmp458, tmp459, tmp460, tmp464, tmp462);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp465;
    compiler::TNode<JSReceiver> tmp466;
    compiler::TNode<String> tmp467;
    compiler::TNode<Number> tmp468;
    compiler::TNode<Object> tmp469;
    compiler::TNode<Object> tmp470;
    compiler::TNode<BuiltinPtr> tmp471;
    compiler::TNode<Map> tmp472;
    compiler::TNode<UintPtrT> tmp473;
    compiler::TNode<IntPtrT> tmp474;
    compiler::TNode<IntPtrT> tmp475;
    compiler::TNode<BuiltinPtr> tmp476;
    compiler::TNode<FixedArray> tmp477;
    compiler::TNode<IntPtrT> tmp478;
    compiler::TNode<IntPtrT> tmp479;
    compiler::TNode<BoolT> tmp480;
    compiler::TNode<UintPtrT> tmp481;
    compiler::TNode<Object> tmp482;
    compiler::TNode<String> tmp483;
    compiler::TNode<Object> tmp484;
    ca_.Bind(&block22, &tmp465, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478, &tmp479, &tmp480, &tmp481, &tmp482, &tmp483, &tmp484);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 300);
    ca_.Goto(&block16, tmp465, tmp466, tmp467, tmp468, tmp469, tmp470, tmp471, tmp472, tmp473, tmp474, tmp475, tmp476, tmp477, tmp478, tmp479, tmp480, tmp481, tmp482, tmp483, tmp484);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp485;
    compiler::TNode<JSReceiver> tmp486;
    compiler::TNode<String> tmp487;
    compiler::TNode<Number> tmp488;
    compiler::TNode<Object> tmp489;
    compiler::TNode<Object> tmp490;
    compiler::TNode<BuiltinPtr> tmp491;
    compiler::TNode<Map> tmp492;
    compiler::TNode<UintPtrT> tmp493;
    compiler::TNode<IntPtrT> tmp494;
    compiler::TNode<IntPtrT> tmp495;
    compiler::TNode<BuiltinPtr> tmp496;
    compiler::TNode<FixedArray> tmp497;
    compiler::TNode<IntPtrT> tmp498;
    compiler::TNode<IntPtrT> tmp499;
    compiler::TNode<BoolT> tmp500;
    compiler::TNode<UintPtrT> tmp501;
    compiler::TNode<Object> tmp502;
    compiler::TNode<String> tmp503;
    compiler::TNode<Object> tmp504;
    ca_.Bind(&block16, &tmp485, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501, &tmp502, &tmp503, &tmp504);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 299);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 295);
    ca_.Goto(&block13, tmp485, tmp486, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492, tmp493, tmp494, tmp495, tmp496, tmp497, tmp498, tmp499, tmp500, tmp501, tmp502, tmp503);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp505;
    compiler::TNode<JSReceiver> tmp506;
    compiler::TNode<String> tmp507;
    compiler::TNode<Number> tmp508;
    compiler::TNode<Object> tmp509;
    compiler::TNode<Object> tmp510;
    compiler::TNode<BuiltinPtr> tmp511;
    compiler::TNode<Map> tmp512;
    compiler::TNode<UintPtrT> tmp513;
    compiler::TNode<IntPtrT> tmp514;
    compiler::TNode<IntPtrT> tmp515;
    compiler::TNode<BuiltinPtr> tmp516;
    compiler::TNode<FixedArray> tmp517;
    compiler::TNode<IntPtrT> tmp518;
    compiler::TNode<IntPtrT> tmp519;
    compiler::TNode<BoolT> tmp520;
    compiler::TNode<UintPtrT> tmp521;
    compiler::TNode<Object> tmp522;
    compiler::TNode<String> tmp523;
    ca_.Bind(&block13, &tmp505, &tmp506, &tmp507, &tmp508, &tmp509, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518, &tmp519, &tmp520, &tmp521, &tmp522, &tmp523);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 315);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 158);
    compiler::TNode<IntPtrT> tmp524;
    USE(tmp524);
    tmp524 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp525;
    USE(tmp525);
    tmp525 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp518}, compiler::TNode<IntPtrT>{tmp524}));
    compiler::TNode<IntPtrT> tmp526;
    USE(tmp526);
    tmp526 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp527;
    USE(tmp527);
    tmp527 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp515}, compiler::TNode<IntPtrT>{tmp526}));
    compiler::TNode<BoolT> tmp528;
    USE(tmp528);
    tmp528 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Or(compiler::TNode<BoolT>{tmp525}, compiler::TNode<BoolT>{tmp527}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 159);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 171);
    compiler::TNode<IntPtrT> tmp529;
    USE(tmp529);
    tmp529 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp530;
    USE(tmp530);
    tmp530 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp515}, compiler::TNode<IntPtrT>{tmp529}));
    ca_.Branch(tmp530, &block30, &block32, tmp505, tmp506, tmp507, tmp508, tmp509, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517, tmp518, tmp519, tmp520, tmp521, tmp522, tmp523, tmp523, tmp515, tmp514, tmp505, tmp523, tmp515, tmp514, tmp528, tmp515, tmp514, tmp528, tmp505, tmp515, tmp514, tmp528);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp531;
    compiler::TNode<JSReceiver> tmp532;
    compiler::TNode<String> tmp533;
    compiler::TNode<Number> tmp534;
    compiler::TNode<Object> tmp535;
    compiler::TNode<Object> tmp536;
    compiler::TNode<BuiltinPtr> tmp537;
    compiler::TNode<Map> tmp538;
    compiler::TNode<UintPtrT> tmp539;
    compiler::TNode<IntPtrT> tmp540;
    compiler::TNode<IntPtrT> tmp541;
    compiler::TNode<BuiltinPtr> tmp542;
    compiler::TNode<FixedArray> tmp543;
    compiler::TNode<IntPtrT> tmp544;
    compiler::TNode<IntPtrT> tmp545;
    compiler::TNode<BoolT> tmp546;
    compiler::TNode<UintPtrT> tmp547;
    compiler::TNode<Object> tmp548;
    compiler::TNode<String> tmp549;
    compiler::TNode<String> tmp550;
    compiler::TNode<IntPtrT> tmp551;
    compiler::TNode<IntPtrT> tmp552;
    compiler::TNode<Context> tmp553;
    compiler::TNode<String> tmp554;
    compiler::TNode<IntPtrT> tmp555;
    compiler::TNode<IntPtrT> tmp556;
    compiler::TNode<BoolT> tmp557;
    compiler::TNode<IntPtrT> tmp558;
    compiler::TNode<IntPtrT> tmp559;
    compiler::TNode<BoolT> tmp560;
    compiler::TNode<Context> tmp561;
    compiler::TNode<IntPtrT> tmp562;
    compiler::TNode<IntPtrT> tmp563;
    compiler::TNode<BoolT> tmp564;
    ca_.Bind(&block32, &tmp531, &tmp532, &tmp533, &tmp534, &tmp535, &tmp536, &tmp537, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544, &tmp545, &tmp546, &tmp547, &tmp548, &tmp549, &tmp550, &tmp551, &tmp552, &tmp553, &tmp554, &tmp555, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563, &tmp564);
    compiler::TNode<IntPtrT> tmp565;
    USE(tmp565);
    tmp565 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp566;
    USE(tmp566);
    tmp566 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp563}, compiler::TNode<IntPtrT>{tmp565}));
    ca_.Branch(tmp566, &block30, &block31, tmp531, tmp532, tmp533, tmp534, tmp535, tmp536, tmp537, tmp538, tmp539, tmp540, tmp541, tmp542, tmp543, tmp544, tmp545, tmp546, tmp547, tmp548, tmp549, tmp550, tmp551, tmp552, tmp553, tmp554, tmp555, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562, tmp563, tmp564);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp567;
    compiler::TNode<JSReceiver> tmp568;
    compiler::TNode<String> tmp569;
    compiler::TNode<Number> tmp570;
    compiler::TNode<Object> tmp571;
    compiler::TNode<Object> tmp572;
    compiler::TNode<BuiltinPtr> tmp573;
    compiler::TNode<Map> tmp574;
    compiler::TNode<UintPtrT> tmp575;
    compiler::TNode<IntPtrT> tmp576;
    compiler::TNode<IntPtrT> tmp577;
    compiler::TNode<BuiltinPtr> tmp578;
    compiler::TNode<FixedArray> tmp579;
    compiler::TNode<IntPtrT> tmp580;
    compiler::TNode<IntPtrT> tmp581;
    compiler::TNode<BoolT> tmp582;
    compiler::TNode<UintPtrT> tmp583;
    compiler::TNode<Object> tmp584;
    compiler::TNode<String> tmp585;
    compiler::TNode<String> tmp586;
    compiler::TNode<IntPtrT> tmp587;
    compiler::TNode<IntPtrT> tmp588;
    compiler::TNode<Context> tmp589;
    compiler::TNode<String> tmp590;
    compiler::TNode<IntPtrT> tmp591;
    compiler::TNode<IntPtrT> tmp592;
    compiler::TNode<BoolT> tmp593;
    compiler::TNode<IntPtrT> tmp594;
    compiler::TNode<IntPtrT> tmp595;
    compiler::TNode<BoolT> tmp596;
    compiler::TNode<Context> tmp597;
    compiler::TNode<IntPtrT> tmp598;
    compiler::TNode<IntPtrT> tmp599;
    compiler::TNode<BoolT> tmp600;
    ca_.Bind(&block30, &tmp567, &tmp568, &tmp569, &tmp570, &tmp571, &tmp572, &tmp573, &tmp574, &tmp575, &tmp576, &tmp577, &tmp578, &tmp579, &tmp580, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585, &tmp586, &tmp587, &tmp588, &tmp589, &tmp590, &tmp591, &tmp592, &tmp593, &tmp594, &tmp595, &tmp596, &tmp597, &tmp598, &tmp599, &tmp600);
    ca_.Goto(&block29, tmp567, tmp568, tmp569, tmp570, tmp571, tmp572, tmp573, tmp574, tmp575, tmp576, tmp577, tmp578, tmp579, tmp580, tmp581, tmp582, tmp583, tmp584, tmp585, tmp586, tmp587, tmp588, tmp589, tmp590, tmp591, tmp592, tmp593, tmp594, tmp595, tmp596, tmp597, tmp598, tmp599, tmp600);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp601;
    compiler::TNode<JSReceiver> tmp602;
    compiler::TNode<String> tmp603;
    compiler::TNode<Number> tmp604;
    compiler::TNode<Object> tmp605;
    compiler::TNode<Object> tmp606;
    compiler::TNode<BuiltinPtr> tmp607;
    compiler::TNode<Map> tmp608;
    compiler::TNode<UintPtrT> tmp609;
    compiler::TNode<IntPtrT> tmp610;
    compiler::TNode<IntPtrT> tmp611;
    compiler::TNode<BuiltinPtr> tmp612;
    compiler::TNode<FixedArray> tmp613;
    compiler::TNode<IntPtrT> tmp614;
    compiler::TNode<IntPtrT> tmp615;
    compiler::TNode<BoolT> tmp616;
    compiler::TNode<UintPtrT> tmp617;
    compiler::TNode<Object> tmp618;
    compiler::TNode<String> tmp619;
    compiler::TNode<String> tmp620;
    compiler::TNode<IntPtrT> tmp621;
    compiler::TNode<IntPtrT> tmp622;
    compiler::TNode<Context> tmp623;
    compiler::TNode<String> tmp624;
    compiler::TNode<IntPtrT> tmp625;
    compiler::TNode<IntPtrT> tmp626;
    compiler::TNode<BoolT> tmp627;
    compiler::TNode<IntPtrT> tmp628;
    compiler::TNode<IntPtrT> tmp629;
    compiler::TNode<BoolT> tmp630;
    compiler::TNode<Context> tmp631;
    compiler::TNode<IntPtrT> tmp632;
    compiler::TNode<IntPtrT> tmp633;
    compiler::TNode<BoolT> tmp634;
    ca_.Bind(&block31, &tmp601, &tmp602, &tmp603, &tmp604, &tmp605, &tmp606, &tmp607, &tmp608, &tmp609, &tmp610, &tmp611, &tmp612, &tmp613, &tmp614, &tmp615, &tmp616, &tmp617, &tmp618, &tmp619, &tmp620, &tmp621, &tmp622, &tmp623, &tmp624, &tmp625, &tmp626, &tmp627, &tmp628, &tmp629, &tmp630, &tmp631, &tmp632, &tmp633, &tmp634);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 173);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 174);
    compiler::TNode<IntPtrT> tmp635;
    USE(tmp635);
    tmp635 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMul(compiler::TNode<IntPtrT>{tmp633}, compiler::TNode<IntPtrT>{tmp632}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 177);
    compiler::TNode<IntPtrT> tmp636;
    USE(tmp636);
    tmp636 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrDiv(compiler::TNode<IntPtrT>{tmp635}, compiler::TNode<IntPtrT>{tmp633}));
    compiler::TNode<BoolT> tmp637;
    USE(tmp637);
    tmp637 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<IntPtrT>{tmp636}, compiler::TNode<IntPtrT>{tmp632}));
    ca_.Branch(tmp637, &block33, &block34, tmp601, tmp602, tmp603, tmp604, tmp605, tmp606, tmp607, tmp608, tmp609, tmp610, tmp611, tmp612, tmp613, tmp614, tmp615, tmp616, tmp617, tmp618, tmp619, tmp620, tmp621, tmp622, tmp623, tmp624, tmp625, tmp626, tmp627, tmp628, tmp629, tmp630, tmp631, tmp632, tmp633, tmp634, tmp632, tmp635);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp638;
    compiler::TNode<JSReceiver> tmp639;
    compiler::TNode<String> tmp640;
    compiler::TNode<Number> tmp641;
    compiler::TNode<Object> tmp642;
    compiler::TNode<Object> tmp643;
    compiler::TNode<BuiltinPtr> tmp644;
    compiler::TNode<Map> tmp645;
    compiler::TNode<UintPtrT> tmp646;
    compiler::TNode<IntPtrT> tmp647;
    compiler::TNode<IntPtrT> tmp648;
    compiler::TNode<BuiltinPtr> tmp649;
    compiler::TNode<FixedArray> tmp650;
    compiler::TNode<IntPtrT> tmp651;
    compiler::TNode<IntPtrT> tmp652;
    compiler::TNode<BoolT> tmp653;
    compiler::TNode<UintPtrT> tmp654;
    compiler::TNode<Object> tmp655;
    compiler::TNode<String> tmp656;
    compiler::TNode<String> tmp657;
    compiler::TNode<IntPtrT> tmp658;
    compiler::TNode<IntPtrT> tmp659;
    compiler::TNode<Context> tmp660;
    compiler::TNode<String> tmp661;
    compiler::TNode<IntPtrT> tmp662;
    compiler::TNode<IntPtrT> tmp663;
    compiler::TNode<BoolT> tmp664;
    compiler::TNode<IntPtrT> tmp665;
    compiler::TNode<IntPtrT> tmp666;
    compiler::TNode<BoolT> tmp667;
    compiler::TNode<Context> tmp668;
    compiler::TNode<IntPtrT> tmp669;
    compiler::TNode<IntPtrT> tmp670;
    compiler::TNode<BoolT> tmp671;
    compiler::TNode<IntPtrT> tmp672;
    compiler::TNode<IntPtrT> tmp673;
    ca_.Bind(&block33, &tmp638, &tmp639, &tmp640, &tmp641, &tmp642, &tmp643, &tmp644, &tmp645, &tmp646, &tmp647, &tmp648, &tmp649, &tmp650, &tmp651, &tmp652, &tmp653, &tmp654, &tmp655, &tmp656, &tmp657, &tmp658, &tmp659, &tmp660, &tmp661, &tmp662, &tmp663, &tmp664, &tmp665, &tmp666, &tmp667, &tmp668, &tmp669, &tmp670, &tmp671, &tmp672, &tmp673);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 178);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp668);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp675;
    compiler::TNode<JSReceiver> tmp676;
    compiler::TNode<String> tmp677;
    compiler::TNode<Number> tmp678;
    compiler::TNode<Object> tmp679;
    compiler::TNode<Object> tmp680;
    compiler::TNode<BuiltinPtr> tmp681;
    compiler::TNode<Map> tmp682;
    compiler::TNode<UintPtrT> tmp683;
    compiler::TNode<IntPtrT> tmp684;
    compiler::TNode<IntPtrT> tmp685;
    compiler::TNode<BuiltinPtr> tmp686;
    compiler::TNode<FixedArray> tmp687;
    compiler::TNode<IntPtrT> tmp688;
    compiler::TNode<IntPtrT> tmp689;
    compiler::TNode<BoolT> tmp690;
    compiler::TNode<UintPtrT> tmp691;
    compiler::TNode<Object> tmp692;
    compiler::TNode<String> tmp693;
    compiler::TNode<String> tmp694;
    compiler::TNode<IntPtrT> tmp695;
    compiler::TNode<IntPtrT> tmp696;
    compiler::TNode<Context> tmp697;
    compiler::TNode<String> tmp698;
    compiler::TNode<IntPtrT> tmp699;
    compiler::TNode<IntPtrT> tmp700;
    compiler::TNode<BoolT> tmp701;
    compiler::TNode<IntPtrT> tmp702;
    compiler::TNode<IntPtrT> tmp703;
    compiler::TNode<BoolT> tmp704;
    compiler::TNode<Context> tmp705;
    compiler::TNode<IntPtrT> tmp706;
    compiler::TNode<IntPtrT> tmp707;
    compiler::TNode<BoolT> tmp708;
    compiler::TNode<IntPtrT> tmp709;
    compiler::TNode<IntPtrT> tmp710;
    ca_.Bind(&block34, &tmp675, &tmp676, &tmp677, &tmp678, &tmp679, &tmp680, &tmp681, &tmp682, &tmp683, &tmp684, &tmp685, &tmp686, &tmp687, &tmp688, &tmp689, &tmp690, &tmp691, &tmp692, &tmp693, &tmp694, &tmp695, &tmp696, &tmp697, &tmp698, &tmp699, &tmp700, &tmp701, &tmp702, &tmp703, &tmp704, &tmp705, &tmp706, &tmp707, &tmp708, &tmp709, &tmp710);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 181);
    compiler::TNode<IntPtrT> tmp711;
    USE(tmp711);
    tmp711 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp705}, compiler::TNode<IntPtrT>{tmp689}, compiler::TNode<IntPtrT>{tmp710}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 182);
    ca_.Branch(tmp708, &block35, &block36, tmp675, tmp676, tmp677, tmp678, tmp679, tmp680, tmp681, tmp682, tmp683, tmp684, tmp685, tmp686, tmp687, tmp688, tmp711, tmp690, tmp691, tmp692, tmp693, tmp694, tmp695, tmp696, tmp697, tmp698, tmp699, tmp700, tmp701, tmp702, tmp703, tmp704, tmp705, tmp706, tmp707, tmp708, tmp709, tmp710);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp712;
    compiler::TNode<JSReceiver> tmp713;
    compiler::TNode<String> tmp714;
    compiler::TNode<Number> tmp715;
    compiler::TNode<Object> tmp716;
    compiler::TNode<Object> tmp717;
    compiler::TNode<BuiltinPtr> tmp718;
    compiler::TNode<Map> tmp719;
    compiler::TNode<UintPtrT> tmp720;
    compiler::TNode<IntPtrT> tmp721;
    compiler::TNode<IntPtrT> tmp722;
    compiler::TNode<BuiltinPtr> tmp723;
    compiler::TNode<FixedArray> tmp724;
    compiler::TNode<IntPtrT> tmp725;
    compiler::TNode<IntPtrT> tmp726;
    compiler::TNode<BoolT> tmp727;
    compiler::TNode<UintPtrT> tmp728;
    compiler::TNode<Object> tmp729;
    compiler::TNode<String> tmp730;
    compiler::TNode<String> tmp731;
    compiler::TNode<IntPtrT> tmp732;
    compiler::TNode<IntPtrT> tmp733;
    compiler::TNode<Context> tmp734;
    compiler::TNode<String> tmp735;
    compiler::TNode<IntPtrT> tmp736;
    compiler::TNode<IntPtrT> tmp737;
    compiler::TNode<BoolT> tmp738;
    compiler::TNode<IntPtrT> tmp739;
    compiler::TNode<IntPtrT> tmp740;
    compiler::TNode<BoolT> tmp741;
    compiler::TNode<Context> tmp742;
    compiler::TNode<IntPtrT> tmp743;
    compiler::TNode<IntPtrT> tmp744;
    compiler::TNode<BoolT> tmp745;
    compiler::TNode<IntPtrT> tmp746;
    compiler::TNode<IntPtrT> tmp747;
    ca_.Bind(&block35, &tmp712, &tmp713, &tmp714, &tmp715, &tmp716, &tmp717, &tmp718, &tmp719, &tmp720, &tmp721, &tmp722, &tmp723, &tmp724, &tmp725, &tmp726, &tmp727, &tmp728, &tmp729, &tmp730, &tmp731, &tmp732, &tmp733, &tmp734, &tmp735, &tmp736, &tmp737, &tmp738, &tmp739, &tmp740, &tmp741, &tmp742, &tmp743, &tmp744, &tmp745, &tmp746, &tmp747);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 184);
    compiler::TNode<IntPtrT> tmp748;
    USE(tmp748);
    tmp748 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp749;
    USE(tmp749);
    tmp749 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp725}, compiler::TNode<IntPtrT>{tmp748}));
    compiler::TNode<Smi> tmp750;
    USE(tmp750);
    tmp750 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp746}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 183);
    compiler::TNode<FixedArray> tmp751;
    USE(tmp751);
    tmp751 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray>{tmp724}, compiler::TNode<IntPtrT>{tmp725}, compiler::TNode<Smi>{tmp750}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 182);
    ca_.Goto(&block36, tmp712, tmp713, tmp714, tmp715, tmp716, tmp717, tmp718, tmp719, tmp720, tmp721, tmp722, tmp723, tmp751, tmp749, tmp726, tmp727, tmp728, tmp729, tmp730, tmp731, tmp732, tmp733, tmp734, tmp735, tmp736, tmp737, tmp738, tmp739, tmp740, tmp741, tmp742, tmp743, tmp744, tmp745, tmp746, tmp747);
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp752;
    compiler::TNode<JSReceiver> tmp753;
    compiler::TNode<String> tmp754;
    compiler::TNode<Number> tmp755;
    compiler::TNode<Object> tmp756;
    compiler::TNode<Object> tmp757;
    compiler::TNode<BuiltinPtr> tmp758;
    compiler::TNode<Map> tmp759;
    compiler::TNode<UintPtrT> tmp760;
    compiler::TNode<IntPtrT> tmp761;
    compiler::TNode<IntPtrT> tmp762;
    compiler::TNode<BuiltinPtr> tmp763;
    compiler::TNode<FixedArray> tmp764;
    compiler::TNode<IntPtrT> tmp765;
    compiler::TNode<IntPtrT> tmp766;
    compiler::TNode<BoolT> tmp767;
    compiler::TNode<UintPtrT> tmp768;
    compiler::TNode<Object> tmp769;
    compiler::TNode<String> tmp770;
    compiler::TNode<String> tmp771;
    compiler::TNode<IntPtrT> tmp772;
    compiler::TNode<IntPtrT> tmp773;
    compiler::TNode<Context> tmp774;
    compiler::TNode<String> tmp775;
    compiler::TNode<IntPtrT> tmp776;
    compiler::TNode<IntPtrT> tmp777;
    compiler::TNode<BoolT> tmp778;
    compiler::TNode<IntPtrT> tmp779;
    compiler::TNode<IntPtrT> tmp780;
    compiler::TNode<BoolT> tmp781;
    compiler::TNode<Context> tmp782;
    compiler::TNode<IntPtrT> tmp783;
    compiler::TNode<IntPtrT> tmp784;
    compiler::TNode<BoolT> tmp785;
    compiler::TNode<IntPtrT> tmp786;
    compiler::TNode<IntPtrT> tmp787;
    ca_.Bind(&block36, &tmp752, &tmp753, &tmp754, &tmp755, &tmp756, &tmp757, &tmp758, &tmp759, &tmp760, &tmp761, &tmp762, &tmp763, &tmp764, &tmp765, &tmp766, &tmp767, &tmp768, &tmp769, &tmp770, &tmp771, &tmp772, &tmp773, &tmp774, &tmp775, &tmp776, &tmp777, &tmp778, &tmp779, &tmp780, &tmp781, &tmp782, &tmp783, &tmp784, &tmp785, &tmp786, &tmp787);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 170);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 159);
    ca_.Goto(&block29, tmp752, tmp753, tmp754, tmp755, tmp756, tmp757, tmp758, tmp759, tmp760, tmp761, tmp762, tmp763, tmp764, tmp765, tmp766, tmp767, tmp768, tmp769, tmp770, tmp771, tmp772, tmp773, tmp774, tmp775, tmp776, tmp777, tmp778, tmp779, tmp780, tmp781, tmp782, tmp783, tmp784, tmp785);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp788;
    compiler::TNode<JSReceiver> tmp789;
    compiler::TNode<String> tmp790;
    compiler::TNode<Number> tmp791;
    compiler::TNode<Object> tmp792;
    compiler::TNode<Object> tmp793;
    compiler::TNode<BuiltinPtr> tmp794;
    compiler::TNode<Map> tmp795;
    compiler::TNode<UintPtrT> tmp796;
    compiler::TNode<IntPtrT> tmp797;
    compiler::TNode<IntPtrT> tmp798;
    compiler::TNode<BuiltinPtr> tmp799;
    compiler::TNode<FixedArray> tmp800;
    compiler::TNode<IntPtrT> tmp801;
    compiler::TNode<IntPtrT> tmp802;
    compiler::TNode<BoolT> tmp803;
    compiler::TNode<UintPtrT> tmp804;
    compiler::TNode<Object> tmp805;
    compiler::TNode<String> tmp806;
    compiler::TNode<String> tmp807;
    compiler::TNode<IntPtrT> tmp808;
    compiler::TNode<IntPtrT> tmp809;
    compiler::TNode<Context> tmp810;
    compiler::TNode<String> tmp811;
    compiler::TNode<IntPtrT> tmp812;
    compiler::TNode<IntPtrT> tmp813;
    compiler::TNode<BoolT> tmp814;
    compiler::TNode<IntPtrT> tmp815;
    compiler::TNode<IntPtrT> tmp816;
    compiler::TNode<BoolT> tmp817;
    compiler::TNode<Context> tmp818;
    compiler::TNode<IntPtrT> tmp819;
    compiler::TNode<IntPtrT> tmp820;
    compiler::TNode<BoolT> tmp821;
    ca_.Bind(&block29, &tmp788, &tmp789, &tmp790, &tmp791, &tmp792, &tmp793, &tmp794, &tmp795, &tmp796, &tmp797, &tmp798, &tmp799, &tmp800, &tmp801, &tmp802, &tmp803, &tmp804, &tmp805, &tmp806, &tmp807, &tmp808, &tmp809, &tmp810, &tmp811, &tmp812, &tmp813, &tmp814, &tmp815, &tmp816, &tmp817, &tmp818, &tmp819, &tmp820, &tmp821);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 162);
    compiler::TNode<IntPtrT> tmp822;
    USE(tmp822);
    tmp822 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadStringLengthAsWord(compiler::TNode<String>{tmp811}));
    compiler::TNode<IntPtrT> tmp823;
    USE(tmp823);
    tmp823 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp810}, compiler::TNode<IntPtrT>{tmp802}, compiler::TNode<IntPtrT>{tmp822}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 161);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 164);
    compiler::TNode<IntPtrT> tmp824;
    USE(tmp824);
    tmp824 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp825;
    USE(tmp825);
    tmp825 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp801}, compiler::TNode<IntPtrT>{tmp824}));
    compiler::TNode<FixedArray> tmp826;
    USE(tmp826);
    tmp826 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray6String(compiler::TNode<FixedArray>{tmp800}, compiler::TNode<IntPtrT>{tmp801}, compiler::TNode<String>{tmp811}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 163);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 166);
    compiler::TNode<Int32T> tmp827;
    USE(tmp827);
    tmp827 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadInstanceType(compiler::TNode<HeapObject>{tmp811}));
    compiler::TNode<BoolT> tmp828;
    USE(tmp828);
    tmp828 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsOneByteStringInstanceType(compiler::TNode<Int32T>{tmp827}));
    compiler::TNode<BoolT> tmp829;
    USE(tmp829);
    tmp829 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32And(compiler::TNode<BoolT>{tmp828}, compiler::TNode<BoolT>{tmp803}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 165);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 156);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 315);
    ca_.Goto(&block28, tmp788, tmp789, tmp790, tmp791, tmp792, tmp793, tmp794, tmp795, tmp796, tmp797, tmp798, tmp799, tmp826, tmp825, tmp823, tmp829, tmp804, tmp805, tmp806, tmp807, tmp808, tmp809, tmp810, tmp811, tmp812, tmp813);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp830;
    compiler::TNode<JSReceiver> tmp831;
    compiler::TNode<String> tmp832;
    compiler::TNode<Number> tmp833;
    compiler::TNode<Object> tmp834;
    compiler::TNode<Object> tmp835;
    compiler::TNode<BuiltinPtr> tmp836;
    compiler::TNode<Map> tmp837;
    compiler::TNode<UintPtrT> tmp838;
    compiler::TNode<IntPtrT> tmp839;
    compiler::TNode<IntPtrT> tmp840;
    compiler::TNode<BuiltinPtr> tmp841;
    compiler::TNode<FixedArray> tmp842;
    compiler::TNode<IntPtrT> tmp843;
    compiler::TNode<IntPtrT> tmp844;
    compiler::TNode<BoolT> tmp845;
    compiler::TNode<UintPtrT> tmp846;
    compiler::TNode<Object> tmp847;
    compiler::TNode<String> tmp848;
    compiler::TNode<String> tmp849;
    compiler::TNode<IntPtrT> tmp850;
    compiler::TNode<IntPtrT> tmp851;
    compiler::TNode<Context> tmp852;
    compiler::TNode<String> tmp853;
    compiler::TNode<IntPtrT> tmp854;
    compiler::TNode<IntPtrT> tmp855;
    ca_.Bind(&block28, &tmp830, &tmp831, &tmp832, &tmp833, &tmp834, &tmp835, &tmp836, &tmp837, &tmp838, &tmp839, &tmp840, &tmp841, &tmp842, &tmp843, &tmp844, &tmp845, &tmp846, &tmp847, &tmp848, &tmp849, &tmp850, &tmp851, &tmp852, &tmp853, &tmp854, &tmp855);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 316);
    compiler::TNode<IntPtrT> tmp856;
    USE(tmp856);
    tmp856 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 277);
    ca_.Goto(&block4, tmp830, tmp831, tmp832, tmp833, tmp834, tmp835, tmp836, tmp837, tmp838, tmp839, tmp856, tmp841, tmp842, tmp843, tmp844, tmp845, tmp846);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp857;
    compiler::TNode<JSReceiver> tmp858;
    compiler::TNode<String> tmp859;
    compiler::TNode<Number> tmp860;
    compiler::TNode<Object> tmp861;
    compiler::TNode<Object> tmp862;
    compiler::TNode<BuiltinPtr> tmp863;
    compiler::TNode<Map> tmp864;
    compiler::TNode<UintPtrT> tmp865;
    compiler::TNode<IntPtrT> tmp866;
    compiler::TNode<IntPtrT> tmp867;
    compiler::TNode<BuiltinPtr> tmp868;
    compiler::TNode<FixedArray> tmp869;
    compiler::TNode<IntPtrT> tmp870;
    compiler::TNode<IntPtrT> tmp871;
    compiler::TNode<BoolT> tmp872;
    compiler::TNode<UintPtrT> tmp873;
    ca_.Bind(&block3, &tmp857, &tmp858, &tmp859, &tmp860, &tmp861, &tmp862, &tmp863, &tmp864, &tmp865, &tmp866, &tmp867, &tmp868, &tmp869, &tmp870, &tmp871, &tmp872, &tmp873);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 320);
    compiler::TNode<BoolT> tmp874;
    USE(tmp874);
    tmp874 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 171);
    compiler::TNode<IntPtrT> tmp875;
    USE(tmp875);
    tmp875 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp876;
    USE(tmp876);
    tmp876 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp867}, compiler::TNode<IntPtrT>{tmp875}));
    ca_.Branch(tmp876, &block38, &block40, tmp857, tmp858, tmp859, tmp860, tmp861, tmp862, tmp863, tmp864, tmp865, tmp866, tmp867, tmp868, tmp869, tmp870, tmp871, tmp872, tmp873, tmp867, tmp866, tmp857, tmp867, tmp866, tmp874);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp877;
    compiler::TNode<JSReceiver> tmp878;
    compiler::TNode<String> tmp879;
    compiler::TNode<Number> tmp880;
    compiler::TNode<Object> tmp881;
    compiler::TNode<Object> tmp882;
    compiler::TNode<BuiltinPtr> tmp883;
    compiler::TNode<Map> tmp884;
    compiler::TNode<UintPtrT> tmp885;
    compiler::TNode<IntPtrT> tmp886;
    compiler::TNode<IntPtrT> tmp887;
    compiler::TNode<BuiltinPtr> tmp888;
    compiler::TNode<FixedArray> tmp889;
    compiler::TNode<IntPtrT> tmp890;
    compiler::TNode<IntPtrT> tmp891;
    compiler::TNode<BoolT> tmp892;
    compiler::TNode<UintPtrT> tmp893;
    compiler::TNode<IntPtrT> tmp894;
    compiler::TNode<IntPtrT> tmp895;
    compiler::TNode<Context> tmp896;
    compiler::TNode<IntPtrT> tmp897;
    compiler::TNode<IntPtrT> tmp898;
    compiler::TNode<BoolT> tmp899;
    ca_.Bind(&block40, &tmp877, &tmp878, &tmp879, &tmp880, &tmp881, &tmp882, &tmp883, &tmp884, &tmp885, &tmp886, &tmp887, &tmp888, &tmp889, &tmp890, &tmp891, &tmp892, &tmp893, &tmp894, &tmp895, &tmp896, &tmp897, &tmp898, &tmp899);
    compiler::TNode<IntPtrT> tmp900;
    USE(tmp900);
    tmp900 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp901;
    USE(tmp901);
    tmp901 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp898}, compiler::TNode<IntPtrT>{tmp900}));
    ca_.Branch(tmp901, &block38, &block39, tmp877, tmp878, tmp879, tmp880, tmp881, tmp882, tmp883, tmp884, tmp885, tmp886, tmp887, tmp888, tmp889, tmp890, tmp891, tmp892, tmp893, tmp894, tmp895, tmp896, tmp897, tmp898, tmp899);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp902;
    compiler::TNode<JSReceiver> tmp903;
    compiler::TNode<String> tmp904;
    compiler::TNode<Number> tmp905;
    compiler::TNode<Object> tmp906;
    compiler::TNode<Object> tmp907;
    compiler::TNode<BuiltinPtr> tmp908;
    compiler::TNode<Map> tmp909;
    compiler::TNode<UintPtrT> tmp910;
    compiler::TNode<IntPtrT> tmp911;
    compiler::TNode<IntPtrT> tmp912;
    compiler::TNode<BuiltinPtr> tmp913;
    compiler::TNode<FixedArray> tmp914;
    compiler::TNode<IntPtrT> tmp915;
    compiler::TNode<IntPtrT> tmp916;
    compiler::TNode<BoolT> tmp917;
    compiler::TNode<UintPtrT> tmp918;
    compiler::TNode<IntPtrT> tmp919;
    compiler::TNode<IntPtrT> tmp920;
    compiler::TNode<Context> tmp921;
    compiler::TNode<IntPtrT> tmp922;
    compiler::TNode<IntPtrT> tmp923;
    compiler::TNode<BoolT> tmp924;
    ca_.Bind(&block38, &tmp902, &tmp903, &tmp904, &tmp905, &tmp906, &tmp907, &tmp908, &tmp909, &tmp910, &tmp911, &tmp912, &tmp913, &tmp914, &tmp915, &tmp916, &tmp917, &tmp918, &tmp919, &tmp920, &tmp921, &tmp922, &tmp923, &tmp924);
    ca_.Goto(&block37, tmp902, tmp903, tmp904, tmp905, tmp906, tmp907, tmp908, tmp909, tmp910, tmp911, tmp912, tmp913, tmp914, tmp915, tmp916, tmp917, tmp918, tmp919, tmp920, tmp921, tmp922, tmp923, tmp924);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp925;
    compiler::TNode<JSReceiver> tmp926;
    compiler::TNode<String> tmp927;
    compiler::TNode<Number> tmp928;
    compiler::TNode<Object> tmp929;
    compiler::TNode<Object> tmp930;
    compiler::TNode<BuiltinPtr> tmp931;
    compiler::TNode<Map> tmp932;
    compiler::TNode<UintPtrT> tmp933;
    compiler::TNode<IntPtrT> tmp934;
    compiler::TNode<IntPtrT> tmp935;
    compiler::TNode<BuiltinPtr> tmp936;
    compiler::TNode<FixedArray> tmp937;
    compiler::TNode<IntPtrT> tmp938;
    compiler::TNode<IntPtrT> tmp939;
    compiler::TNode<BoolT> tmp940;
    compiler::TNode<UintPtrT> tmp941;
    compiler::TNode<IntPtrT> tmp942;
    compiler::TNode<IntPtrT> tmp943;
    compiler::TNode<Context> tmp944;
    compiler::TNode<IntPtrT> tmp945;
    compiler::TNode<IntPtrT> tmp946;
    compiler::TNode<BoolT> tmp947;
    ca_.Bind(&block39, &tmp925, &tmp926, &tmp927, &tmp928, &tmp929, &tmp930, &tmp931, &tmp932, &tmp933, &tmp934, &tmp935, &tmp936, &tmp937, &tmp938, &tmp939, &tmp940, &tmp941, &tmp942, &tmp943, &tmp944, &tmp945, &tmp946, &tmp947);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 173);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 174);
    compiler::TNode<IntPtrT> tmp948;
    USE(tmp948);
    tmp948 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMul(compiler::TNode<IntPtrT>{tmp946}, compiler::TNode<IntPtrT>{tmp945}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 177);
    compiler::TNode<IntPtrT> tmp949;
    USE(tmp949);
    tmp949 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrDiv(compiler::TNode<IntPtrT>{tmp948}, compiler::TNode<IntPtrT>{tmp946}));
    compiler::TNode<BoolT> tmp950;
    USE(tmp950);
    tmp950 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<IntPtrT>{tmp949}, compiler::TNode<IntPtrT>{tmp945}));
    ca_.Branch(tmp950, &block41, &block42, tmp925, tmp926, tmp927, tmp928, tmp929, tmp930, tmp931, tmp932, tmp933, tmp934, tmp935, tmp936, tmp937, tmp938, tmp939, tmp940, tmp941, tmp942, tmp943, tmp944, tmp945, tmp946, tmp947, tmp945, tmp948);
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp951;
    compiler::TNode<JSReceiver> tmp952;
    compiler::TNode<String> tmp953;
    compiler::TNode<Number> tmp954;
    compiler::TNode<Object> tmp955;
    compiler::TNode<Object> tmp956;
    compiler::TNode<BuiltinPtr> tmp957;
    compiler::TNode<Map> tmp958;
    compiler::TNode<UintPtrT> tmp959;
    compiler::TNode<IntPtrT> tmp960;
    compiler::TNode<IntPtrT> tmp961;
    compiler::TNode<BuiltinPtr> tmp962;
    compiler::TNode<FixedArray> tmp963;
    compiler::TNode<IntPtrT> tmp964;
    compiler::TNode<IntPtrT> tmp965;
    compiler::TNode<BoolT> tmp966;
    compiler::TNode<UintPtrT> tmp967;
    compiler::TNode<IntPtrT> tmp968;
    compiler::TNode<IntPtrT> tmp969;
    compiler::TNode<Context> tmp970;
    compiler::TNode<IntPtrT> tmp971;
    compiler::TNode<IntPtrT> tmp972;
    compiler::TNode<BoolT> tmp973;
    compiler::TNode<IntPtrT> tmp974;
    compiler::TNode<IntPtrT> tmp975;
    ca_.Bind(&block41, &tmp951, &tmp952, &tmp953, &tmp954, &tmp955, &tmp956, &tmp957, &tmp958, &tmp959, &tmp960, &tmp961, &tmp962, &tmp963, &tmp964, &tmp965, &tmp966, &tmp967, &tmp968, &tmp969, &tmp970, &tmp971, &tmp972, &tmp973, &tmp974, &tmp975);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 178);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp970);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp977;
    compiler::TNode<JSReceiver> tmp978;
    compiler::TNode<String> tmp979;
    compiler::TNode<Number> tmp980;
    compiler::TNode<Object> tmp981;
    compiler::TNode<Object> tmp982;
    compiler::TNode<BuiltinPtr> tmp983;
    compiler::TNode<Map> tmp984;
    compiler::TNode<UintPtrT> tmp985;
    compiler::TNode<IntPtrT> tmp986;
    compiler::TNode<IntPtrT> tmp987;
    compiler::TNode<BuiltinPtr> tmp988;
    compiler::TNode<FixedArray> tmp989;
    compiler::TNode<IntPtrT> tmp990;
    compiler::TNode<IntPtrT> tmp991;
    compiler::TNode<BoolT> tmp992;
    compiler::TNode<UintPtrT> tmp993;
    compiler::TNode<IntPtrT> tmp994;
    compiler::TNode<IntPtrT> tmp995;
    compiler::TNode<Context> tmp996;
    compiler::TNode<IntPtrT> tmp997;
    compiler::TNode<IntPtrT> tmp998;
    compiler::TNode<BoolT> tmp999;
    compiler::TNode<IntPtrT> tmp1000;
    compiler::TNode<IntPtrT> tmp1001;
    ca_.Bind(&block42, &tmp977, &tmp978, &tmp979, &tmp980, &tmp981, &tmp982, &tmp983, &tmp984, &tmp985, &tmp986, &tmp987, &tmp988, &tmp989, &tmp990, &tmp991, &tmp992, &tmp993, &tmp994, &tmp995, &tmp996, &tmp997, &tmp998, &tmp999, &tmp1000, &tmp1001);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 181);
    compiler::TNode<IntPtrT> tmp1002;
    USE(tmp1002);
    tmp1002 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp996}, compiler::TNode<IntPtrT>{tmp991}, compiler::TNode<IntPtrT>{tmp1001}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 182);
    ca_.Branch(tmp999, &block43, &block44, tmp977, tmp978, tmp979, tmp980, tmp981, tmp982, tmp983, tmp984, tmp985, tmp986, tmp987, tmp988, tmp989, tmp990, tmp1002, tmp992, tmp993, tmp994, tmp995, tmp996, tmp997, tmp998, tmp999, tmp1000, tmp1001);
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp1003;
    compiler::TNode<JSReceiver> tmp1004;
    compiler::TNode<String> tmp1005;
    compiler::TNode<Number> tmp1006;
    compiler::TNode<Object> tmp1007;
    compiler::TNode<Object> tmp1008;
    compiler::TNode<BuiltinPtr> tmp1009;
    compiler::TNode<Map> tmp1010;
    compiler::TNode<UintPtrT> tmp1011;
    compiler::TNode<IntPtrT> tmp1012;
    compiler::TNode<IntPtrT> tmp1013;
    compiler::TNode<BuiltinPtr> tmp1014;
    compiler::TNode<FixedArray> tmp1015;
    compiler::TNode<IntPtrT> tmp1016;
    compiler::TNode<IntPtrT> tmp1017;
    compiler::TNode<BoolT> tmp1018;
    compiler::TNode<UintPtrT> tmp1019;
    compiler::TNode<IntPtrT> tmp1020;
    compiler::TNode<IntPtrT> tmp1021;
    compiler::TNode<Context> tmp1022;
    compiler::TNode<IntPtrT> tmp1023;
    compiler::TNode<IntPtrT> tmp1024;
    compiler::TNode<BoolT> tmp1025;
    compiler::TNode<IntPtrT> tmp1026;
    compiler::TNode<IntPtrT> tmp1027;
    ca_.Bind(&block43, &tmp1003, &tmp1004, &tmp1005, &tmp1006, &tmp1007, &tmp1008, &tmp1009, &tmp1010, &tmp1011, &tmp1012, &tmp1013, &tmp1014, &tmp1015, &tmp1016, &tmp1017, &tmp1018, &tmp1019, &tmp1020, &tmp1021, &tmp1022, &tmp1023, &tmp1024, &tmp1025, &tmp1026, &tmp1027);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 184);
    compiler::TNode<IntPtrT> tmp1028;
    USE(tmp1028);
    tmp1028 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp1029;
    USE(tmp1029);
    tmp1029 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp1016}, compiler::TNode<IntPtrT>{tmp1028}));
    compiler::TNode<Smi> tmp1030;
    USE(tmp1030);
    tmp1030 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp1026}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 183);
    compiler::TNode<FixedArray> tmp1031;
    USE(tmp1031);
    tmp1031 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray>{tmp1015}, compiler::TNode<IntPtrT>{tmp1016}, compiler::TNode<Smi>{tmp1030}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 182);
    ca_.Goto(&block44, tmp1003, tmp1004, tmp1005, tmp1006, tmp1007, tmp1008, tmp1009, tmp1010, tmp1011, tmp1012, tmp1013, tmp1014, tmp1031, tmp1029, tmp1017, tmp1018, tmp1019, tmp1020, tmp1021, tmp1022, tmp1023, tmp1024, tmp1025, tmp1026, tmp1027);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp1032;
    compiler::TNode<JSReceiver> tmp1033;
    compiler::TNode<String> tmp1034;
    compiler::TNode<Number> tmp1035;
    compiler::TNode<Object> tmp1036;
    compiler::TNode<Object> tmp1037;
    compiler::TNode<BuiltinPtr> tmp1038;
    compiler::TNode<Map> tmp1039;
    compiler::TNode<UintPtrT> tmp1040;
    compiler::TNode<IntPtrT> tmp1041;
    compiler::TNode<IntPtrT> tmp1042;
    compiler::TNode<BuiltinPtr> tmp1043;
    compiler::TNode<FixedArray> tmp1044;
    compiler::TNode<IntPtrT> tmp1045;
    compiler::TNode<IntPtrT> tmp1046;
    compiler::TNode<BoolT> tmp1047;
    compiler::TNode<UintPtrT> tmp1048;
    compiler::TNode<IntPtrT> tmp1049;
    compiler::TNode<IntPtrT> tmp1050;
    compiler::TNode<Context> tmp1051;
    compiler::TNode<IntPtrT> tmp1052;
    compiler::TNode<IntPtrT> tmp1053;
    compiler::TNode<BoolT> tmp1054;
    compiler::TNode<IntPtrT> tmp1055;
    compiler::TNode<IntPtrT> tmp1056;
    ca_.Bind(&block44, &tmp1032, &tmp1033, &tmp1034, &tmp1035, &tmp1036, &tmp1037, &tmp1038, &tmp1039, &tmp1040, &tmp1041, &tmp1042, &tmp1043, &tmp1044, &tmp1045, &tmp1046, &tmp1047, &tmp1048, &tmp1049, &tmp1050, &tmp1051, &tmp1052, &tmp1053, &tmp1054, &tmp1055, &tmp1056);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 170);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 320);
    ca_.Goto(&block37, tmp1032, tmp1033, tmp1034, tmp1035, tmp1036, tmp1037, tmp1038, tmp1039, tmp1040, tmp1041, tmp1042, tmp1043, tmp1044, tmp1045, tmp1046, tmp1047, tmp1048, tmp1049, tmp1050, tmp1051, tmp1052, tmp1053, tmp1054);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp1057;
    compiler::TNode<JSReceiver> tmp1058;
    compiler::TNode<String> tmp1059;
    compiler::TNode<Number> tmp1060;
    compiler::TNode<Object> tmp1061;
    compiler::TNode<Object> tmp1062;
    compiler::TNode<BuiltinPtr> tmp1063;
    compiler::TNode<Map> tmp1064;
    compiler::TNode<UintPtrT> tmp1065;
    compiler::TNode<IntPtrT> tmp1066;
    compiler::TNode<IntPtrT> tmp1067;
    compiler::TNode<BuiltinPtr> tmp1068;
    compiler::TNode<FixedArray> tmp1069;
    compiler::TNode<IntPtrT> tmp1070;
    compiler::TNode<IntPtrT> tmp1071;
    compiler::TNode<BoolT> tmp1072;
    compiler::TNode<UintPtrT> tmp1073;
    compiler::TNode<IntPtrT> tmp1074;
    compiler::TNode<IntPtrT> tmp1075;
    compiler::TNode<Context> tmp1076;
    compiler::TNode<IntPtrT> tmp1077;
    compiler::TNode<IntPtrT> tmp1078;
    compiler::TNode<BoolT> tmp1079;
    ca_.Bind(&block37, &tmp1057, &tmp1058, &tmp1059, &tmp1060, &tmp1061, &tmp1062, &tmp1063, &tmp1064, &tmp1065, &tmp1066, &tmp1067, &tmp1068, &tmp1069, &tmp1070, &tmp1071, &tmp1072, &tmp1073, &tmp1074, &tmp1075, &tmp1076, &tmp1077, &tmp1078, &tmp1079);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 323);
    compiler::TNode<String> tmp1080;
    USE(tmp1080);
    tmp1080 = ca_.UncheckedCast<String>(ArrayJoinBuiltinsFromDSLAssembler(state_).BufferJoin(compiler::TNode<Context>{tmp1057}, ArrayJoinBuiltinsFromDSLAssembler::Buffer{compiler::TNode<FixedArray>{tmp1069}, compiler::TNode<IntPtrT>{tmp1070}, compiler::TNode<IntPtrT>{tmp1071}, compiler::TNode<BoolT>{tmp1072}}, compiler::TNode<String>{tmp1059}));
    ca_.Goto(&block1, tmp1057, tmp1058, tmp1059, tmp1060, tmp1061, tmp1062, tmp1063, tmp1080);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp1081;
    compiler::TNode<JSReceiver> tmp1082;
    compiler::TNode<String> tmp1083;
    compiler::TNode<Number> tmp1084;
    compiler::TNode<Object> tmp1085;
    compiler::TNode<Object> tmp1086;
    compiler::TNode<BuiltinPtr> tmp1087;
    compiler::TNode<String> tmp1088;
    ca_.Bind(&block1, &tmp1081, &tmp1082, &tmp1083, &tmp1084, &tmp1085, &tmp1086, &tmp1087, &tmp1088);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 262);
    ca_.Goto(&block45, tmp1081, tmp1082, tmp1083, tmp1084, tmp1085, tmp1086, tmp1087, tmp1088);
  }

    compiler::TNode<Context> tmp1089;
    compiler::TNode<JSReceiver> tmp1090;
    compiler::TNode<String> tmp1091;
    compiler::TNode<Number> tmp1092;
    compiler::TNode<Object> tmp1093;
    compiler::TNode<Object> tmp1094;
    compiler::TNode<BuiltinPtr> tmp1095;
    compiler::TNode<String> tmp1096;
    ca_.Bind(&block45, &tmp1089, &tmp1090, &tmp1091, &tmp1092, &tmp1093, &tmp1094, &tmp1095, &tmp1096);
  return compiler::TNode<String>{tmp1096};
}

TF_BUILTIN(LoadJoinTypedElement17ATFixedInt32Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt32Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement19ATFixedFloat32Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat32Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement19ATFixedFloat64Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat64Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement24ATFixedUint8ClampedArray, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType24ATFixedUint8ClampedArray()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement21ATFixedBigUint64Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType21ATFixedBigUint64Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement20ATFixedBigInt64Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType20ATFixedBigInt64Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement17ATFixedUint8Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedUint8Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement16ATFixedInt8Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType16ATFixedInt8Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement18ATFixedUint16Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint16Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement17ATFixedInt16Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt16Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

TF_BUILTIN(LoadJoinTypedElement18ATFixedUint32Array, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Number> parameter2 = UncheckedCast<Number>(Parameter(Descriptor::kK));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 56);
    compiler::TNode<JSArrayBuffer> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp3}));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp4}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10);
    CodeStubAssembler(state_).FailAssert("Torque assert \'!IsDetachedBuffer(typedArray.buffer)\' failed", "../../src/builtins/array-join.tq", 56);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp15}, compiler::TNode<Smi>{tmp16}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint32Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp17);
  }
}

compiler::TNode<String> ArrayJoinBuiltinsFromDSLAssembler::ArrayJoinImpl12JSTypedArray(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<String> p_sep, compiler::TNode<Number> p_lengthNumber, bool p_useToLocaleString, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options, compiler::TNode<BuiltinPtr> p_initialLoadFn) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, BuiltinPtr, JSReceiver, Map, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, BuiltinPtr, JSReceiver, Map, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, Object> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, Object, String> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, String> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, String> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, Object> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, Object, Number> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, HeapObject> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object, HeapObject> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block33(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block35(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT, BoolT, Context, IntPtrT, IntPtrT, BoolT> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, Object, String, String, IntPtrT, IntPtrT, Context, String, IntPtrT, IntPtrT> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block41(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block43(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT, IntPtrT, IntPtrT> block44(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, Map, UintPtrT, IntPtrT, IntPtrT, BuiltinPtr, FixedArray, IntPtrT, IntPtrT, BoolT, UintPtrT, IntPtrT, IntPtrT, Context, IntPtrT, IntPtrT, BoolT> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, String, Number, Object, Object, BuiltinPtr, String> block45(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver, p_sep, p_lengthNumber, p_locales, p_options, p_initialLoadFn);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<String> tmp2;
    compiler::TNode<Number> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<BuiltinPtr> tmp6;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 266);
    compiler::TNode<Map> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Map>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapObjectMap(compiler::TNode<HeapObject>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 267);
    compiler::TNode<UintPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr20UT5ATSmi10HeapNumber(compiler::TNode<Number>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 268);
    compiler::TNode<IntPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadStringLengthAsWord(compiler::TNode<String>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 269);
    compiler::TNode<IntPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 270);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 271);
    compiler::TNode<FixedArray> tmp11;
    USE(tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    compiler::TNode<IntPtrT> tmp13;
    USE(tmp13);
    compiler::TNode<BoolT> tmp14;
    USE(tmp14);
    std::tie(tmp11, tmp12, tmp13, tmp14) = ArrayJoinBuiltinsFromDSLAssembler(state_).NewBuffer(compiler::TNode<UintPtrT>{tmp8}, compiler::TNode<String>{tmp2}).Flatten();
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 274);
    compiler::TNode<UintPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 277);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp6, tmp11, tmp12, tmp13, tmp14, tmp15);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<String> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<BuiltinPtr> tmp22;
    compiler::TNode<Map> tmp23;
    compiler::TNode<UintPtrT> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<BuiltinPtr> tmp27;
    compiler::TNode<FixedArray> tmp28;
    compiler::TNode<IntPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<BoolT> tmp31;
    compiler::TNode<UintPtrT> tmp32;
    ca_.Bind(&block4, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32);
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrLessThan(compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<UintPtrT>{tmp24}));
    ca_.Branch(tmp33, &block2, &block3, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<JSReceiver> tmp35;
    compiler::TNode<String> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<BuiltinPtr> tmp40;
    compiler::TNode<Map> tmp41;
    compiler::TNode<UintPtrT> tmp42;
    compiler::TNode<IntPtrT> tmp43;
    compiler::TNode<IntPtrT> tmp44;
    compiler::TNode<BuiltinPtr> tmp45;
    compiler::TNode<FixedArray> tmp46;
    compiler::TNode<IntPtrT> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<BoolT> tmp49;
    compiler::TNode<UintPtrT> tmp50;
    ca_.Bind(&block2, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 279);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 278);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    ArrayJoinBuiltinsFromDSLAssembler(state_).CannotUseSameArrayAccessor12JSTypedArray(compiler::TNode<Context>{tmp34}, compiler::TNode<BuiltinPtr>{tmp45}, compiler::TNode<JSReceiver>{tmp35}, compiler::TNode<Map>{tmp41}, compiler::TNode<Number>{tmp37}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp45, tmp35, tmp41, tmp37);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp45, tmp35, tmp41, tmp37);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<JSReceiver> tmp52;
    compiler::TNode<String> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<BuiltinPtr> tmp57;
    compiler::TNode<Map> tmp58;
    compiler::TNode<UintPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<IntPtrT> tmp61;
    compiler::TNode<BuiltinPtr> tmp62;
    compiler::TNode<FixedArray> tmp63;
    compiler::TNode<IntPtrT> tmp64;
    compiler::TNode<IntPtrT> tmp65;
    compiler::TNode<BoolT> tmp66;
    compiler::TNode<UintPtrT> tmp67;
    compiler::TNode<BuiltinPtr> tmp68;
    compiler::TNode<JSReceiver> tmp69;
    compiler::TNode<Map> tmp70;
    compiler::TNode<Number> tmp71;
    ca_.Bind(&block7, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    ca_.Goto(&block5, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<JSReceiver> tmp73;
    compiler::TNode<String> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<BuiltinPtr> tmp78;
    compiler::TNode<Map> tmp79;
    compiler::TNode<UintPtrT> tmp80;
    compiler::TNode<IntPtrT> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    compiler::TNode<BuiltinPtr> tmp83;
    compiler::TNode<FixedArray> tmp84;
    compiler::TNode<IntPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<BoolT> tmp87;
    compiler::TNode<UintPtrT> tmp88;
    compiler::TNode<BuiltinPtr> tmp89;
    compiler::TNode<JSReceiver> tmp90;
    compiler::TNode<Map> tmp91;
    compiler::TNode<Number> tmp92;
    ca_.Bind(&block8, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.Goto(&block6, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp93;
    compiler::TNode<JSReceiver> tmp94;
    compiler::TNode<String> tmp95;
    compiler::TNode<Number> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<BuiltinPtr> tmp99;
    compiler::TNode<Map> tmp100;
    compiler::TNode<UintPtrT> tmp101;
    compiler::TNode<IntPtrT> tmp102;
    compiler::TNode<IntPtrT> tmp103;
    compiler::TNode<BuiltinPtr> tmp104;
    compiler::TNode<FixedArray> tmp105;
    compiler::TNode<IntPtrT> tmp106;
    compiler::TNode<IntPtrT> tmp107;
    compiler::TNode<BoolT> tmp108;
    compiler::TNode<UintPtrT> tmp109;
    ca_.Bind(&block5, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 281);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 278);
    ca_.Goto(&block6, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement25ATGenericElementsAccessor)), tmp105, tmp106, tmp107, tmp108, tmp109);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<JSReceiver> tmp111;
    compiler::TNode<String> tmp112;
    compiler::TNode<Number> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<Object> tmp115;
    compiler::TNode<BuiltinPtr> tmp116;
    compiler::TNode<Map> tmp117;
    compiler::TNode<UintPtrT> tmp118;
    compiler::TNode<IntPtrT> tmp119;
    compiler::TNode<IntPtrT> tmp120;
    compiler::TNode<BuiltinPtr> tmp121;
    compiler::TNode<FixedArray> tmp122;
    compiler::TNode<IntPtrT> tmp123;
    compiler::TNode<IntPtrT> tmp124;
    compiler::TNode<BoolT> tmp125;
    compiler::TNode<UintPtrT> tmp126;
    ca_.Bind(&block6, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 284);
    compiler::TNode<UintPtrT> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp128;
    USE(tmp128);
    tmp128 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrGreaterThan(compiler::TNode<UintPtrT>{tmp126}, compiler::TNode<UintPtrT>{tmp127}));
    ca_.Branch(tmp128, &block9, &block10, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp129;
    compiler::TNode<JSReceiver> tmp130;
    compiler::TNode<String> tmp131;
    compiler::TNode<Number> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<BuiltinPtr> tmp135;
    compiler::TNode<Map> tmp136;
    compiler::TNode<UintPtrT> tmp137;
    compiler::TNode<IntPtrT> tmp138;
    compiler::TNode<IntPtrT> tmp139;
    compiler::TNode<BuiltinPtr> tmp140;
    compiler::TNode<FixedArray> tmp141;
    compiler::TNode<IntPtrT> tmp142;
    compiler::TNode<IntPtrT> tmp143;
    compiler::TNode<BoolT> tmp144;
    compiler::TNode<UintPtrT> tmp145;
    ca_.Bind(&block9, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 286);
    compiler::TNode<IntPtrT> tmp146;
    USE(tmp146);
    tmp146 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp147;
    USE(tmp147);
    tmp147 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp139}, compiler::TNode<IntPtrT>{tmp146}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 284);
    ca_.Goto(&block10, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp147, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp148;
    compiler::TNode<JSReceiver> tmp149;
    compiler::TNode<String> tmp150;
    compiler::TNode<Number> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<BuiltinPtr> tmp154;
    compiler::TNode<Map> tmp155;
    compiler::TNode<UintPtrT> tmp156;
    compiler::TNode<IntPtrT> tmp157;
    compiler::TNode<IntPtrT> tmp158;
    compiler::TNode<BuiltinPtr> tmp159;
    compiler::TNode<FixedArray> tmp160;
    compiler::TNode<IntPtrT> tmp161;
    compiler::TNode<IntPtrT> tmp162;
    compiler::TNode<BoolT> tmp163;
    compiler::TNode<UintPtrT> tmp164;
    ca_.Bind(&block10, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 290);
    compiler::TNode<UintPtrT> tmp165;
    USE(tmp165);
    tmp165 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp166;
    USE(tmp166);
    tmp166 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp164}, compiler::TNode<UintPtrT>{tmp165}));
    compiler::TNode<Number> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATuintptr(compiler::TNode<UintPtrT>{tmp164}));
    compiler::TNode<Object> tmp168 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(0)).descriptor(), tmp159, tmp148, tmp149, tmp167); 
    USE(tmp168);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 294);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 295);
    if ((p_useToLocaleString)) {
      ca_.Goto(&block11, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp166, tmp168, ca_.Uninitialized<String>());
    } else {
      ca_.Goto(&block12, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp166, tmp168, ca_.Uninitialized<String>());
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp169;
    compiler::TNode<JSReceiver> tmp170;
    compiler::TNode<String> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<Object> tmp174;
    compiler::TNode<BuiltinPtr> tmp175;
    compiler::TNode<Map> tmp176;
    compiler::TNode<UintPtrT> tmp177;
    compiler::TNode<IntPtrT> tmp178;
    compiler::TNode<IntPtrT> tmp179;
    compiler::TNode<BuiltinPtr> tmp180;
    compiler::TNode<FixedArray> tmp181;
    compiler::TNode<IntPtrT> tmp182;
    compiler::TNode<IntPtrT> tmp183;
    compiler::TNode<BoolT> tmp184;
    compiler::TNode<UintPtrT> tmp185;
    compiler::TNode<Object> tmp186;
    compiler::TNode<String> tmp187;
    ca_.Bind(&block11, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 296);
    compiler::TNode<String> tmp188;
    tmp188 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kConvertToLocaleString, tmp169, tmp186, tmp173, tmp174));
    USE(tmp188);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 297);
    compiler::TNode<String> tmp189;
    USE(tmp189);
    tmp189 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp190;
    USE(tmp190);
    tmp190 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp188}, compiler::TNode<HeapObject>{tmp189}));
    ca_.Branch(tmp190, &block14, &block15, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp188);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp191;
    compiler::TNode<JSReceiver> tmp192;
    compiler::TNode<String> tmp193;
    compiler::TNode<Number> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<BuiltinPtr> tmp197;
    compiler::TNode<Map> tmp198;
    compiler::TNode<UintPtrT> tmp199;
    compiler::TNode<IntPtrT> tmp200;
    compiler::TNode<IntPtrT> tmp201;
    compiler::TNode<BuiltinPtr> tmp202;
    compiler::TNode<FixedArray> tmp203;
    compiler::TNode<IntPtrT> tmp204;
    compiler::TNode<IntPtrT> tmp205;
    compiler::TNode<BoolT> tmp206;
    compiler::TNode<UintPtrT> tmp207;
    compiler::TNode<Object> tmp208;
    compiler::TNode<String> tmp209;
    ca_.Bind(&block14, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209);
    ca_.Goto(&block4, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp210;
    compiler::TNode<JSReceiver> tmp211;
    compiler::TNode<String> tmp212;
    compiler::TNode<Number> tmp213;
    compiler::TNode<Object> tmp214;
    compiler::TNode<Object> tmp215;
    compiler::TNode<BuiltinPtr> tmp216;
    compiler::TNode<Map> tmp217;
    compiler::TNode<UintPtrT> tmp218;
    compiler::TNode<IntPtrT> tmp219;
    compiler::TNode<IntPtrT> tmp220;
    compiler::TNode<BuiltinPtr> tmp221;
    compiler::TNode<FixedArray> tmp222;
    compiler::TNode<IntPtrT> tmp223;
    compiler::TNode<IntPtrT> tmp224;
    compiler::TNode<BoolT> tmp225;
    compiler::TNode<UintPtrT> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<String> tmp228;
    ca_.Bind(&block15, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 295);
    ca_.Goto(&block13, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp229;
    compiler::TNode<JSReceiver> tmp230;
    compiler::TNode<String> tmp231;
    compiler::TNode<Number> tmp232;
    compiler::TNode<Object> tmp233;
    compiler::TNode<Object> tmp234;
    compiler::TNode<BuiltinPtr> tmp235;
    compiler::TNode<Map> tmp236;
    compiler::TNode<UintPtrT> tmp237;
    compiler::TNode<IntPtrT> tmp238;
    compiler::TNode<IntPtrT> tmp239;
    compiler::TNode<BuiltinPtr> tmp240;
    compiler::TNode<FixedArray> tmp241;
    compiler::TNode<IntPtrT> tmp242;
    compiler::TNode<IntPtrT> tmp243;
    compiler::TNode<BoolT> tmp244;
    compiler::TNode<UintPtrT> tmp245;
    compiler::TNode<Object> tmp246;
    compiler::TNode<String> tmp247;
    ca_.Bind(&block12, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 299);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 300);
    compiler::TNode<String> tmp248;
    USE(tmp248);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp248 = BaseBuiltinsFromDSLAssembler(state_).Cast6String(compiler::TNode<Context>{tmp229}, compiler::TNode<Object>{tmp246}, &label0);
    ca_.Goto(&block18, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp246, tmp246, tmp248);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block19, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp246, tmp246);
    }
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp249;
    compiler::TNode<JSReceiver> tmp250;
    compiler::TNode<String> tmp251;
    compiler::TNode<Number> tmp252;
    compiler::TNode<Object> tmp253;
    compiler::TNode<Object> tmp254;
    compiler::TNode<BuiltinPtr> tmp255;
    compiler::TNode<Map> tmp256;
    compiler::TNode<UintPtrT> tmp257;
    compiler::TNode<IntPtrT> tmp258;
    compiler::TNode<IntPtrT> tmp259;
    compiler::TNode<BuiltinPtr> tmp260;
    compiler::TNode<FixedArray> tmp261;
    compiler::TNode<IntPtrT> tmp262;
    compiler::TNode<IntPtrT> tmp263;
    compiler::TNode<BoolT> tmp264;
    compiler::TNode<UintPtrT> tmp265;
    compiler::TNode<Object> tmp266;
    compiler::TNode<String> tmp267;
    compiler::TNode<Object> tmp268;
    compiler::TNode<Object> tmp269;
    ca_.Bind(&block19, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269);
    ca_.Goto(&block17, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp270;
    compiler::TNode<JSReceiver> tmp271;
    compiler::TNode<String> tmp272;
    compiler::TNode<Number> tmp273;
    compiler::TNode<Object> tmp274;
    compiler::TNode<Object> tmp275;
    compiler::TNode<BuiltinPtr> tmp276;
    compiler::TNode<Map> tmp277;
    compiler::TNode<UintPtrT> tmp278;
    compiler::TNode<IntPtrT> tmp279;
    compiler::TNode<IntPtrT> tmp280;
    compiler::TNode<BuiltinPtr> tmp281;
    compiler::TNode<FixedArray> tmp282;
    compiler::TNode<IntPtrT> tmp283;
    compiler::TNode<IntPtrT> tmp284;
    compiler::TNode<BoolT> tmp285;
    compiler::TNode<UintPtrT> tmp286;
    compiler::TNode<Object> tmp287;
    compiler::TNode<String> tmp288;
    compiler::TNode<Object> tmp289;
    compiler::TNode<Object> tmp290;
    compiler::TNode<String> tmp291;
    ca_.Bind(&block18, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 301);
    compiler::TNode<String> tmp292;
    USE(tmp292);
    tmp292 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp293;
    USE(tmp293);
    tmp293 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp291}, compiler::TNode<HeapObject>{tmp292}));
    ca_.Branch(tmp293, &block20, &block21, tmp270, tmp271, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp291);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp294;
    compiler::TNode<JSReceiver> tmp295;
    compiler::TNode<String> tmp296;
    compiler::TNode<Number> tmp297;
    compiler::TNode<Object> tmp298;
    compiler::TNode<Object> tmp299;
    compiler::TNode<BuiltinPtr> tmp300;
    compiler::TNode<Map> tmp301;
    compiler::TNode<UintPtrT> tmp302;
    compiler::TNode<IntPtrT> tmp303;
    compiler::TNode<IntPtrT> tmp304;
    compiler::TNode<BuiltinPtr> tmp305;
    compiler::TNode<FixedArray> tmp306;
    compiler::TNode<IntPtrT> tmp307;
    compiler::TNode<IntPtrT> tmp308;
    compiler::TNode<BoolT> tmp309;
    compiler::TNode<UintPtrT> tmp310;
    compiler::TNode<Object> tmp311;
    compiler::TNode<String> tmp312;
    compiler::TNode<Object> tmp313;
    compiler::TNode<String> tmp314;
    ca_.Bind(&block20, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314);
    ca_.Goto(&block4, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp315;
    compiler::TNode<JSReceiver> tmp316;
    compiler::TNode<String> tmp317;
    compiler::TNode<Number> tmp318;
    compiler::TNode<Object> tmp319;
    compiler::TNode<Object> tmp320;
    compiler::TNode<BuiltinPtr> tmp321;
    compiler::TNode<Map> tmp322;
    compiler::TNode<UintPtrT> tmp323;
    compiler::TNode<IntPtrT> tmp324;
    compiler::TNode<IntPtrT> tmp325;
    compiler::TNode<BuiltinPtr> tmp326;
    compiler::TNode<FixedArray> tmp327;
    compiler::TNode<IntPtrT> tmp328;
    compiler::TNode<IntPtrT> tmp329;
    compiler::TNode<BoolT> tmp330;
    compiler::TNode<UintPtrT> tmp331;
    compiler::TNode<Object> tmp332;
    compiler::TNode<String> tmp333;
    compiler::TNode<Object> tmp334;
    compiler::TNode<String> tmp335;
    ca_.Bind(&block21, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 302);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 300);
    ca_.Goto(&block16, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp335, tmp334);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp336;
    compiler::TNode<JSReceiver> tmp337;
    compiler::TNode<String> tmp338;
    compiler::TNode<Number> tmp339;
    compiler::TNode<Object> tmp340;
    compiler::TNode<Object> tmp341;
    compiler::TNode<BuiltinPtr> tmp342;
    compiler::TNode<Map> tmp343;
    compiler::TNode<UintPtrT> tmp344;
    compiler::TNode<IntPtrT> tmp345;
    compiler::TNode<IntPtrT> tmp346;
    compiler::TNode<BuiltinPtr> tmp347;
    compiler::TNode<FixedArray> tmp348;
    compiler::TNode<IntPtrT> tmp349;
    compiler::TNode<IntPtrT> tmp350;
    compiler::TNode<BoolT> tmp351;
    compiler::TNode<UintPtrT> tmp352;
    compiler::TNode<Object> tmp353;
    compiler::TNode<String> tmp354;
    compiler::TNode<Object> tmp355;
    ca_.Bind(&block17, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 304);
    compiler::TNode<Number> tmp356;
    USE(tmp356);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp356 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp355)}, &label0);
    ca_.Goto(&block24, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, ca_.UncheckedCast<Object>(tmp355), tmp356);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block25, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, ca_.UncheckedCast<Object>(tmp355));
    }
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp357;
    compiler::TNode<JSReceiver> tmp358;
    compiler::TNode<String> tmp359;
    compiler::TNode<Number> tmp360;
    compiler::TNode<Object> tmp361;
    compiler::TNode<Object> tmp362;
    compiler::TNode<BuiltinPtr> tmp363;
    compiler::TNode<Map> tmp364;
    compiler::TNode<UintPtrT> tmp365;
    compiler::TNode<IntPtrT> tmp366;
    compiler::TNode<IntPtrT> tmp367;
    compiler::TNode<BuiltinPtr> tmp368;
    compiler::TNode<FixedArray> tmp369;
    compiler::TNode<IntPtrT> tmp370;
    compiler::TNode<IntPtrT> tmp371;
    compiler::TNode<BoolT> tmp372;
    compiler::TNode<UintPtrT> tmp373;
    compiler::TNode<Object> tmp374;
    compiler::TNode<String> tmp375;
    compiler::TNode<Object> tmp376;
    compiler::TNode<Object> tmp377;
    ca_.Bind(&block25, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377);
    ca_.Goto(&block23, tmp357, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp378;
    compiler::TNode<JSReceiver> tmp379;
    compiler::TNode<String> tmp380;
    compiler::TNode<Number> tmp381;
    compiler::TNode<Object> tmp382;
    compiler::TNode<Object> tmp383;
    compiler::TNode<BuiltinPtr> tmp384;
    compiler::TNode<Map> tmp385;
    compiler::TNode<UintPtrT> tmp386;
    compiler::TNode<IntPtrT> tmp387;
    compiler::TNode<IntPtrT> tmp388;
    compiler::TNode<BuiltinPtr> tmp389;
    compiler::TNode<FixedArray> tmp390;
    compiler::TNode<IntPtrT> tmp391;
    compiler::TNode<IntPtrT> tmp392;
    compiler::TNode<BoolT> tmp393;
    compiler::TNode<UintPtrT> tmp394;
    compiler::TNode<Object> tmp395;
    compiler::TNode<String> tmp396;
    compiler::TNode<Object> tmp397;
    compiler::TNode<Object> tmp398;
    compiler::TNode<Number> tmp399;
    ca_.Bind(&block24, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 305);
    compiler::TNode<String> tmp400;
    USE(tmp400);
    tmp400 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).NumberToString(compiler::TNode<Number>{tmp399}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 304);
    ca_.Goto(&block22, tmp378, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp400, tmp397);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp401;
    compiler::TNode<JSReceiver> tmp402;
    compiler::TNode<String> tmp403;
    compiler::TNode<Number> tmp404;
    compiler::TNode<Object> tmp405;
    compiler::TNode<Object> tmp406;
    compiler::TNode<BuiltinPtr> tmp407;
    compiler::TNode<Map> tmp408;
    compiler::TNode<UintPtrT> tmp409;
    compiler::TNode<IntPtrT> tmp410;
    compiler::TNode<IntPtrT> tmp411;
    compiler::TNode<BuiltinPtr> tmp412;
    compiler::TNode<FixedArray> tmp413;
    compiler::TNode<IntPtrT> tmp414;
    compiler::TNode<IntPtrT> tmp415;
    compiler::TNode<BoolT> tmp416;
    compiler::TNode<UintPtrT> tmp417;
    compiler::TNode<Object> tmp418;
    compiler::TNode<String> tmp419;
    compiler::TNode<Object> tmp420;
    ca_.Bind(&block23, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 307);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 308);
    compiler::TNode<BoolT> tmp421;
    USE(tmp421);
    tmp421 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{ca_.UncheckedCast<HeapObject>(tmp420)}));
    ca_.Branch(tmp421, &block26, &block27, tmp401, tmp402, tmp403, tmp404, tmp405, tmp406, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420, ca_.UncheckedCast<HeapObject>(tmp420));
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp422;
    compiler::TNode<JSReceiver> tmp423;
    compiler::TNode<String> tmp424;
    compiler::TNode<Number> tmp425;
    compiler::TNode<Object> tmp426;
    compiler::TNode<Object> tmp427;
    compiler::TNode<BuiltinPtr> tmp428;
    compiler::TNode<Map> tmp429;
    compiler::TNode<UintPtrT> tmp430;
    compiler::TNode<IntPtrT> tmp431;
    compiler::TNode<IntPtrT> tmp432;
    compiler::TNode<BuiltinPtr> tmp433;
    compiler::TNode<FixedArray> tmp434;
    compiler::TNode<IntPtrT> tmp435;
    compiler::TNode<IntPtrT> tmp436;
    compiler::TNode<BoolT> tmp437;
    compiler::TNode<UintPtrT> tmp438;
    compiler::TNode<Object> tmp439;
    compiler::TNode<String> tmp440;
    compiler::TNode<Object> tmp441;
    compiler::TNode<HeapObject> tmp442;
    ca_.Bind(&block26, &tmp422, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442);
    ca_.Goto(&block4, tmp422, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp443;
    compiler::TNode<JSReceiver> tmp444;
    compiler::TNode<String> tmp445;
    compiler::TNode<Number> tmp446;
    compiler::TNode<Object> tmp447;
    compiler::TNode<Object> tmp448;
    compiler::TNode<BuiltinPtr> tmp449;
    compiler::TNode<Map> tmp450;
    compiler::TNode<UintPtrT> tmp451;
    compiler::TNode<IntPtrT> tmp452;
    compiler::TNode<IntPtrT> tmp453;
    compiler::TNode<BuiltinPtr> tmp454;
    compiler::TNode<FixedArray> tmp455;
    compiler::TNode<IntPtrT> tmp456;
    compiler::TNode<IntPtrT> tmp457;
    compiler::TNode<BoolT> tmp458;
    compiler::TNode<UintPtrT> tmp459;
    compiler::TNode<Object> tmp460;
    compiler::TNode<String> tmp461;
    compiler::TNode<Object> tmp462;
    compiler::TNode<HeapObject> tmp463;
    ca_.Bind(&block27, &tmp443, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 309);
    compiler::TNode<String> tmp464;
    tmp464 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kToString, tmp443, tmp463));
    USE(tmp464);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 304);
    ca_.Goto(&block22, tmp443, tmp444, tmp445, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452, tmp453, tmp454, tmp455, tmp456, tmp457, tmp458, tmp459, tmp460, tmp464, tmp462);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp465;
    compiler::TNode<JSReceiver> tmp466;
    compiler::TNode<String> tmp467;
    compiler::TNode<Number> tmp468;
    compiler::TNode<Object> tmp469;
    compiler::TNode<Object> tmp470;
    compiler::TNode<BuiltinPtr> tmp471;
    compiler::TNode<Map> tmp472;
    compiler::TNode<UintPtrT> tmp473;
    compiler::TNode<IntPtrT> tmp474;
    compiler::TNode<IntPtrT> tmp475;
    compiler::TNode<BuiltinPtr> tmp476;
    compiler::TNode<FixedArray> tmp477;
    compiler::TNode<IntPtrT> tmp478;
    compiler::TNode<IntPtrT> tmp479;
    compiler::TNode<BoolT> tmp480;
    compiler::TNode<UintPtrT> tmp481;
    compiler::TNode<Object> tmp482;
    compiler::TNode<String> tmp483;
    compiler::TNode<Object> tmp484;
    ca_.Bind(&block22, &tmp465, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478, &tmp479, &tmp480, &tmp481, &tmp482, &tmp483, &tmp484);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 300);
    ca_.Goto(&block16, tmp465, tmp466, tmp467, tmp468, tmp469, tmp470, tmp471, tmp472, tmp473, tmp474, tmp475, tmp476, tmp477, tmp478, tmp479, tmp480, tmp481, tmp482, tmp483, tmp484);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp485;
    compiler::TNode<JSReceiver> tmp486;
    compiler::TNode<String> tmp487;
    compiler::TNode<Number> tmp488;
    compiler::TNode<Object> tmp489;
    compiler::TNode<Object> tmp490;
    compiler::TNode<BuiltinPtr> tmp491;
    compiler::TNode<Map> tmp492;
    compiler::TNode<UintPtrT> tmp493;
    compiler::TNode<IntPtrT> tmp494;
    compiler::TNode<IntPtrT> tmp495;
    compiler::TNode<BuiltinPtr> tmp496;
    compiler::TNode<FixedArray> tmp497;
    compiler::TNode<IntPtrT> tmp498;
    compiler::TNode<IntPtrT> tmp499;
    compiler::TNode<BoolT> tmp500;
    compiler::TNode<UintPtrT> tmp501;
    compiler::TNode<Object> tmp502;
    compiler::TNode<String> tmp503;
    compiler::TNode<Object> tmp504;
    ca_.Bind(&block16, &tmp485, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501, &tmp502, &tmp503, &tmp504);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 299);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 295);
    ca_.Goto(&block13, tmp485, tmp486, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492, tmp493, tmp494, tmp495, tmp496, tmp497, tmp498, tmp499, tmp500, tmp501, tmp502, tmp503);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp505;
    compiler::TNode<JSReceiver> tmp506;
    compiler::TNode<String> tmp507;
    compiler::TNode<Number> tmp508;
    compiler::TNode<Object> tmp509;
    compiler::TNode<Object> tmp510;
    compiler::TNode<BuiltinPtr> tmp511;
    compiler::TNode<Map> tmp512;
    compiler::TNode<UintPtrT> tmp513;
    compiler::TNode<IntPtrT> tmp514;
    compiler::TNode<IntPtrT> tmp515;
    compiler::TNode<BuiltinPtr> tmp516;
    compiler::TNode<FixedArray> tmp517;
    compiler::TNode<IntPtrT> tmp518;
    compiler::TNode<IntPtrT> tmp519;
    compiler::TNode<BoolT> tmp520;
    compiler::TNode<UintPtrT> tmp521;
    compiler::TNode<Object> tmp522;
    compiler::TNode<String> tmp523;
    ca_.Bind(&block13, &tmp505, &tmp506, &tmp507, &tmp508, &tmp509, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518, &tmp519, &tmp520, &tmp521, &tmp522, &tmp523);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 315);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 158);
    compiler::TNode<IntPtrT> tmp524;
    USE(tmp524);
    tmp524 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp525;
    USE(tmp525);
    tmp525 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp518}, compiler::TNode<IntPtrT>{tmp524}));
    compiler::TNode<IntPtrT> tmp526;
    USE(tmp526);
    tmp526 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp527;
    USE(tmp527);
    tmp527 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp515}, compiler::TNode<IntPtrT>{tmp526}));
    compiler::TNode<BoolT> tmp528;
    USE(tmp528);
    tmp528 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Or(compiler::TNode<BoolT>{tmp525}, compiler::TNode<BoolT>{tmp527}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 159);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 171);
    compiler::TNode<IntPtrT> tmp529;
    USE(tmp529);
    tmp529 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp530;
    USE(tmp530);
    tmp530 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp515}, compiler::TNode<IntPtrT>{tmp529}));
    ca_.Branch(tmp530, &block30, &block32, tmp505, tmp506, tmp507, tmp508, tmp509, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517, tmp518, tmp519, tmp520, tmp521, tmp522, tmp523, tmp523, tmp515, tmp514, tmp505, tmp523, tmp515, tmp514, tmp528, tmp515, tmp514, tmp528, tmp505, tmp515, tmp514, tmp528);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp531;
    compiler::TNode<JSReceiver> tmp532;
    compiler::TNode<String> tmp533;
    compiler::TNode<Number> tmp534;
    compiler::TNode<Object> tmp535;
    compiler::TNode<Object> tmp536;
    compiler::TNode<BuiltinPtr> tmp537;
    compiler::TNode<Map> tmp538;
    compiler::TNode<UintPtrT> tmp539;
    compiler::TNode<IntPtrT> tmp540;
    compiler::TNode<IntPtrT> tmp541;
    compiler::TNode<BuiltinPtr> tmp542;
    compiler::TNode<FixedArray> tmp543;
    compiler::TNode<IntPtrT> tmp544;
    compiler::TNode<IntPtrT> tmp545;
    compiler::TNode<BoolT> tmp546;
    compiler::TNode<UintPtrT> tmp547;
    compiler::TNode<Object> tmp548;
    compiler::TNode<String> tmp549;
    compiler::TNode<String> tmp550;
    compiler::TNode<IntPtrT> tmp551;
    compiler::TNode<IntPtrT> tmp552;
    compiler::TNode<Context> tmp553;
    compiler::TNode<String> tmp554;
    compiler::TNode<IntPtrT> tmp555;
    compiler::TNode<IntPtrT> tmp556;
    compiler::TNode<BoolT> tmp557;
    compiler::TNode<IntPtrT> tmp558;
    compiler::TNode<IntPtrT> tmp559;
    compiler::TNode<BoolT> tmp560;
    compiler::TNode<Context> tmp561;
    compiler::TNode<IntPtrT> tmp562;
    compiler::TNode<IntPtrT> tmp563;
    compiler::TNode<BoolT> tmp564;
    ca_.Bind(&block32, &tmp531, &tmp532, &tmp533, &tmp534, &tmp535, &tmp536, &tmp537, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544, &tmp545, &tmp546, &tmp547, &tmp548, &tmp549, &tmp550, &tmp551, &tmp552, &tmp553, &tmp554, &tmp555, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563, &tmp564);
    compiler::TNode<IntPtrT> tmp565;
    USE(tmp565);
    tmp565 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp566;
    USE(tmp566);
    tmp566 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp563}, compiler::TNode<IntPtrT>{tmp565}));
    ca_.Branch(tmp566, &block30, &block31, tmp531, tmp532, tmp533, tmp534, tmp535, tmp536, tmp537, tmp538, tmp539, tmp540, tmp541, tmp542, tmp543, tmp544, tmp545, tmp546, tmp547, tmp548, tmp549, tmp550, tmp551, tmp552, tmp553, tmp554, tmp555, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562, tmp563, tmp564);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp567;
    compiler::TNode<JSReceiver> tmp568;
    compiler::TNode<String> tmp569;
    compiler::TNode<Number> tmp570;
    compiler::TNode<Object> tmp571;
    compiler::TNode<Object> tmp572;
    compiler::TNode<BuiltinPtr> tmp573;
    compiler::TNode<Map> tmp574;
    compiler::TNode<UintPtrT> tmp575;
    compiler::TNode<IntPtrT> tmp576;
    compiler::TNode<IntPtrT> tmp577;
    compiler::TNode<BuiltinPtr> tmp578;
    compiler::TNode<FixedArray> tmp579;
    compiler::TNode<IntPtrT> tmp580;
    compiler::TNode<IntPtrT> tmp581;
    compiler::TNode<BoolT> tmp582;
    compiler::TNode<UintPtrT> tmp583;
    compiler::TNode<Object> tmp584;
    compiler::TNode<String> tmp585;
    compiler::TNode<String> tmp586;
    compiler::TNode<IntPtrT> tmp587;
    compiler::TNode<IntPtrT> tmp588;
    compiler::TNode<Context> tmp589;
    compiler::TNode<String> tmp590;
    compiler::TNode<IntPtrT> tmp591;
    compiler::TNode<IntPtrT> tmp592;
    compiler::TNode<BoolT> tmp593;
    compiler::TNode<IntPtrT> tmp594;
    compiler::TNode<IntPtrT> tmp595;
    compiler::TNode<BoolT> tmp596;
    compiler::TNode<Context> tmp597;
    compiler::TNode<IntPtrT> tmp598;
    compiler::TNode<IntPtrT> tmp599;
    compiler::TNode<BoolT> tmp600;
    ca_.Bind(&block30, &tmp567, &tmp568, &tmp569, &tmp570, &tmp571, &tmp572, &tmp573, &tmp574, &tmp575, &tmp576, &tmp577, &tmp578, &tmp579, &tmp580, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585, &tmp586, &tmp587, &tmp588, &tmp589, &tmp590, &tmp591, &tmp592, &tmp593, &tmp594, &tmp595, &tmp596, &tmp597, &tmp598, &tmp599, &tmp600);
    ca_.Goto(&block29, tmp567, tmp568, tmp569, tmp570, tmp571, tmp572, tmp573, tmp574, tmp575, tmp576, tmp577, tmp578, tmp579, tmp580, tmp581, tmp582, tmp583, tmp584, tmp585, tmp586, tmp587, tmp588, tmp589, tmp590, tmp591, tmp592, tmp593, tmp594, tmp595, tmp596, tmp597, tmp598, tmp599, tmp600);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp601;
    compiler::TNode<JSReceiver> tmp602;
    compiler::TNode<String> tmp603;
    compiler::TNode<Number> tmp604;
    compiler::TNode<Object> tmp605;
    compiler::TNode<Object> tmp606;
    compiler::TNode<BuiltinPtr> tmp607;
    compiler::TNode<Map> tmp608;
    compiler::TNode<UintPtrT> tmp609;
    compiler::TNode<IntPtrT> tmp610;
    compiler::TNode<IntPtrT> tmp611;
    compiler::TNode<BuiltinPtr> tmp612;
    compiler::TNode<FixedArray> tmp613;
    compiler::TNode<IntPtrT> tmp614;
    compiler::TNode<IntPtrT> tmp615;
    compiler::TNode<BoolT> tmp616;
    compiler::TNode<UintPtrT> tmp617;
    compiler::TNode<Object> tmp618;
    compiler::TNode<String> tmp619;
    compiler::TNode<String> tmp620;
    compiler::TNode<IntPtrT> tmp621;
    compiler::TNode<IntPtrT> tmp622;
    compiler::TNode<Context> tmp623;
    compiler::TNode<String> tmp624;
    compiler::TNode<IntPtrT> tmp625;
    compiler::TNode<IntPtrT> tmp626;
    compiler::TNode<BoolT> tmp627;
    compiler::TNode<IntPtrT> tmp628;
    compiler::TNode<IntPtrT> tmp629;
    compiler::TNode<BoolT> tmp630;
    compiler::TNode<Context> tmp631;
    compiler::TNode<IntPtrT> tmp632;
    compiler::TNode<IntPtrT> tmp633;
    compiler::TNode<BoolT> tmp634;
    ca_.Bind(&block31, &tmp601, &tmp602, &tmp603, &tmp604, &tmp605, &tmp606, &tmp607, &tmp608, &tmp609, &tmp610, &tmp611, &tmp612, &tmp613, &tmp614, &tmp615, &tmp616, &tmp617, &tmp618, &tmp619, &tmp620, &tmp621, &tmp622, &tmp623, &tmp624, &tmp625, &tmp626, &tmp627, &tmp628, &tmp629, &tmp630, &tmp631, &tmp632, &tmp633, &tmp634);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 173);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 174);
    compiler::TNode<IntPtrT> tmp635;
    USE(tmp635);
    tmp635 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMul(compiler::TNode<IntPtrT>{tmp633}, compiler::TNode<IntPtrT>{tmp632}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 177);
    compiler::TNode<IntPtrT> tmp636;
    USE(tmp636);
    tmp636 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrDiv(compiler::TNode<IntPtrT>{tmp635}, compiler::TNode<IntPtrT>{tmp633}));
    compiler::TNode<BoolT> tmp637;
    USE(tmp637);
    tmp637 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<IntPtrT>{tmp636}, compiler::TNode<IntPtrT>{tmp632}));
    ca_.Branch(tmp637, &block33, &block34, tmp601, tmp602, tmp603, tmp604, tmp605, tmp606, tmp607, tmp608, tmp609, tmp610, tmp611, tmp612, tmp613, tmp614, tmp615, tmp616, tmp617, tmp618, tmp619, tmp620, tmp621, tmp622, tmp623, tmp624, tmp625, tmp626, tmp627, tmp628, tmp629, tmp630, tmp631, tmp632, tmp633, tmp634, tmp632, tmp635);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp638;
    compiler::TNode<JSReceiver> tmp639;
    compiler::TNode<String> tmp640;
    compiler::TNode<Number> tmp641;
    compiler::TNode<Object> tmp642;
    compiler::TNode<Object> tmp643;
    compiler::TNode<BuiltinPtr> tmp644;
    compiler::TNode<Map> tmp645;
    compiler::TNode<UintPtrT> tmp646;
    compiler::TNode<IntPtrT> tmp647;
    compiler::TNode<IntPtrT> tmp648;
    compiler::TNode<BuiltinPtr> tmp649;
    compiler::TNode<FixedArray> tmp650;
    compiler::TNode<IntPtrT> tmp651;
    compiler::TNode<IntPtrT> tmp652;
    compiler::TNode<BoolT> tmp653;
    compiler::TNode<UintPtrT> tmp654;
    compiler::TNode<Object> tmp655;
    compiler::TNode<String> tmp656;
    compiler::TNode<String> tmp657;
    compiler::TNode<IntPtrT> tmp658;
    compiler::TNode<IntPtrT> tmp659;
    compiler::TNode<Context> tmp660;
    compiler::TNode<String> tmp661;
    compiler::TNode<IntPtrT> tmp662;
    compiler::TNode<IntPtrT> tmp663;
    compiler::TNode<BoolT> tmp664;
    compiler::TNode<IntPtrT> tmp665;
    compiler::TNode<IntPtrT> tmp666;
    compiler::TNode<BoolT> tmp667;
    compiler::TNode<Context> tmp668;
    compiler::TNode<IntPtrT> tmp669;
    compiler::TNode<IntPtrT> tmp670;
    compiler::TNode<BoolT> tmp671;
    compiler::TNode<IntPtrT> tmp672;
    compiler::TNode<IntPtrT> tmp673;
    ca_.Bind(&block33, &tmp638, &tmp639, &tmp640, &tmp641, &tmp642, &tmp643, &tmp644, &tmp645, &tmp646, &tmp647, &tmp648, &tmp649, &tmp650, &tmp651, &tmp652, &tmp653, &tmp654, &tmp655, &tmp656, &tmp657, &tmp658, &tmp659, &tmp660, &tmp661, &tmp662, &tmp663, &tmp664, &tmp665, &tmp666, &tmp667, &tmp668, &tmp669, &tmp670, &tmp671, &tmp672, &tmp673);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 178);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp668);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp675;
    compiler::TNode<JSReceiver> tmp676;
    compiler::TNode<String> tmp677;
    compiler::TNode<Number> tmp678;
    compiler::TNode<Object> tmp679;
    compiler::TNode<Object> tmp680;
    compiler::TNode<BuiltinPtr> tmp681;
    compiler::TNode<Map> tmp682;
    compiler::TNode<UintPtrT> tmp683;
    compiler::TNode<IntPtrT> tmp684;
    compiler::TNode<IntPtrT> tmp685;
    compiler::TNode<BuiltinPtr> tmp686;
    compiler::TNode<FixedArray> tmp687;
    compiler::TNode<IntPtrT> tmp688;
    compiler::TNode<IntPtrT> tmp689;
    compiler::TNode<BoolT> tmp690;
    compiler::TNode<UintPtrT> tmp691;
    compiler::TNode<Object> tmp692;
    compiler::TNode<String> tmp693;
    compiler::TNode<String> tmp694;
    compiler::TNode<IntPtrT> tmp695;
    compiler::TNode<IntPtrT> tmp696;
    compiler::TNode<Context> tmp697;
    compiler::TNode<String> tmp698;
    compiler::TNode<IntPtrT> tmp699;
    compiler::TNode<IntPtrT> tmp700;
    compiler::TNode<BoolT> tmp701;
    compiler::TNode<IntPtrT> tmp702;
    compiler::TNode<IntPtrT> tmp703;
    compiler::TNode<BoolT> tmp704;
    compiler::TNode<Context> tmp705;
    compiler::TNode<IntPtrT> tmp706;
    compiler::TNode<IntPtrT> tmp707;
    compiler::TNode<BoolT> tmp708;
    compiler::TNode<IntPtrT> tmp709;
    compiler::TNode<IntPtrT> tmp710;
    ca_.Bind(&block34, &tmp675, &tmp676, &tmp677, &tmp678, &tmp679, &tmp680, &tmp681, &tmp682, &tmp683, &tmp684, &tmp685, &tmp686, &tmp687, &tmp688, &tmp689, &tmp690, &tmp691, &tmp692, &tmp693, &tmp694, &tmp695, &tmp696, &tmp697, &tmp698, &tmp699, &tmp700, &tmp701, &tmp702, &tmp703, &tmp704, &tmp705, &tmp706, &tmp707, &tmp708, &tmp709, &tmp710);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 181);
    compiler::TNode<IntPtrT> tmp711;
    USE(tmp711);
    tmp711 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp705}, compiler::TNode<IntPtrT>{tmp689}, compiler::TNode<IntPtrT>{tmp710}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 182);
    ca_.Branch(tmp708, &block35, &block36, tmp675, tmp676, tmp677, tmp678, tmp679, tmp680, tmp681, tmp682, tmp683, tmp684, tmp685, tmp686, tmp687, tmp688, tmp711, tmp690, tmp691, tmp692, tmp693, tmp694, tmp695, tmp696, tmp697, tmp698, tmp699, tmp700, tmp701, tmp702, tmp703, tmp704, tmp705, tmp706, tmp707, tmp708, tmp709, tmp710);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp712;
    compiler::TNode<JSReceiver> tmp713;
    compiler::TNode<String> tmp714;
    compiler::TNode<Number> tmp715;
    compiler::TNode<Object> tmp716;
    compiler::TNode<Object> tmp717;
    compiler::TNode<BuiltinPtr> tmp718;
    compiler::TNode<Map> tmp719;
    compiler::TNode<UintPtrT> tmp720;
    compiler::TNode<IntPtrT> tmp721;
    compiler::TNode<IntPtrT> tmp722;
    compiler::TNode<BuiltinPtr> tmp723;
    compiler::TNode<FixedArray> tmp724;
    compiler::TNode<IntPtrT> tmp725;
    compiler::TNode<IntPtrT> tmp726;
    compiler::TNode<BoolT> tmp727;
    compiler::TNode<UintPtrT> tmp728;
    compiler::TNode<Object> tmp729;
    compiler::TNode<String> tmp730;
    compiler::TNode<String> tmp731;
    compiler::TNode<IntPtrT> tmp732;
    compiler::TNode<IntPtrT> tmp733;
    compiler::TNode<Context> tmp734;
    compiler::TNode<String> tmp735;
    compiler::TNode<IntPtrT> tmp736;
    compiler::TNode<IntPtrT> tmp737;
    compiler::TNode<BoolT> tmp738;
    compiler::TNode<IntPtrT> tmp739;
    compiler::TNode<IntPtrT> tmp740;
    compiler::TNode<BoolT> tmp741;
    compiler::TNode<Context> tmp742;
    compiler::TNode<IntPtrT> tmp743;
    compiler::TNode<IntPtrT> tmp744;
    compiler::TNode<BoolT> tmp745;
    compiler::TNode<IntPtrT> tmp746;
    compiler::TNode<IntPtrT> tmp747;
    ca_.Bind(&block35, &tmp712, &tmp713, &tmp714, &tmp715, &tmp716, &tmp717, &tmp718, &tmp719, &tmp720, &tmp721, &tmp722, &tmp723, &tmp724, &tmp725, &tmp726, &tmp727, &tmp728, &tmp729, &tmp730, &tmp731, &tmp732, &tmp733, &tmp734, &tmp735, &tmp736, &tmp737, &tmp738, &tmp739, &tmp740, &tmp741, &tmp742, &tmp743, &tmp744, &tmp745, &tmp746, &tmp747);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 184);
    compiler::TNode<IntPtrT> tmp748;
    USE(tmp748);
    tmp748 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp749;
    USE(tmp749);
    tmp749 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp725}, compiler::TNode<IntPtrT>{tmp748}));
    compiler::TNode<Smi> tmp750;
    USE(tmp750);
    tmp750 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp746}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 183);
    compiler::TNode<FixedArray> tmp751;
    USE(tmp751);
    tmp751 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray>{tmp724}, compiler::TNode<IntPtrT>{tmp725}, compiler::TNode<Smi>{tmp750}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 182);
    ca_.Goto(&block36, tmp712, tmp713, tmp714, tmp715, tmp716, tmp717, tmp718, tmp719, tmp720, tmp721, tmp722, tmp723, tmp751, tmp749, tmp726, tmp727, tmp728, tmp729, tmp730, tmp731, tmp732, tmp733, tmp734, tmp735, tmp736, tmp737, tmp738, tmp739, tmp740, tmp741, tmp742, tmp743, tmp744, tmp745, tmp746, tmp747);
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp752;
    compiler::TNode<JSReceiver> tmp753;
    compiler::TNode<String> tmp754;
    compiler::TNode<Number> tmp755;
    compiler::TNode<Object> tmp756;
    compiler::TNode<Object> tmp757;
    compiler::TNode<BuiltinPtr> tmp758;
    compiler::TNode<Map> tmp759;
    compiler::TNode<UintPtrT> tmp760;
    compiler::TNode<IntPtrT> tmp761;
    compiler::TNode<IntPtrT> tmp762;
    compiler::TNode<BuiltinPtr> tmp763;
    compiler::TNode<FixedArray> tmp764;
    compiler::TNode<IntPtrT> tmp765;
    compiler::TNode<IntPtrT> tmp766;
    compiler::TNode<BoolT> tmp767;
    compiler::TNode<UintPtrT> tmp768;
    compiler::TNode<Object> tmp769;
    compiler::TNode<String> tmp770;
    compiler::TNode<String> tmp771;
    compiler::TNode<IntPtrT> tmp772;
    compiler::TNode<IntPtrT> tmp773;
    compiler::TNode<Context> tmp774;
    compiler::TNode<String> tmp775;
    compiler::TNode<IntPtrT> tmp776;
    compiler::TNode<IntPtrT> tmp777;
    compiler::TNode<BoolT> tmp778;
    compiler::TNode<IntPtrT> tmp779;
    compiler::TNode<IntPtrT> tmp780;
    compiler::TNode<BoolT> tmp781;
    compiler::TNode<Context> tmp782;
    compiler::TNode<IntPtrT> tmp783;
    compiler::TNode<IntPtrT> tmp784;
    compiler::TNode<BoolT> tmp785;
    compiler::TNode<IntPtrT> tmp786;
    compiler::TNode<IntPtrT> tmp787;
    ca_.Bind(&block36, &tmp752, &tmp753, &tmp754, &tmp755, &tmp756, &tmp757, &tmp758, &tmp759, &tmp760, &tmp761, &tmp762, &tmp763, &tmp764, &tmp765, &tmp766, &tmp767, &tmp768, &tmp769, &tmp770, &tmp771, &tmp772, &tmp773, &tmp774, &tmp775, &tmp776, &tmp777, &tmp778, &tmp779, &tmp780, &tmp781, &tmp782, &tmp783, &tmp784, &tmp785, &tmp786, &tmp787);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 170);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 159);
    ca_.Goto(&block29, tmp752, tmp753, tmp754, tmp755, tmp756, tmp757, tmp758, tmp759, tmp760, tmp761, tmp762, tmp763, tmp764, tmp765, tmp766, tmp767, tmp768, tmp769, tmp770, tmp771, tmp772, tmp773, tmp774, tmp775, tmp776, tmp777, tmp778, tmp779, tmp780, tmp781, tmp782, tmp783, tmp784, tmp785);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp788;
    compiler::TNode<JSReceiver> tmp789;
    compiler::TNode<String> tmp790;
    compiler::TNode<Number> tmp791;
    compiler::TNode<Object> tmp792;
    compiler::TNode<Object> tmp793;
    compiler::TNode<BuiltinPtr> tmp794;
    compiler::TNode<Map> tmp795;
    compiler::TNode<UintPtrT> tmp796;
    compiler::TNode<IntPtrT> tmp797;
    compiler::TNode<IntPtrT> tmp798;
    compiler::TNode<BuiltinPtr> tmp799;
    compiler::TNode<FixedArray> tmp800;
    compiler::TNode<IntPtrT> tmp801;
    compiler::TNode<IntPtrT> tmp802;
    compiler::TNode<BoolT> tmp803;
    compiler::TNode<UintPtrT> tmp804;
    compiler::TNode<Object> tmp805;
    compiler::TNode<String> tmp806;
    compiler::TNode<String> tmp807;
    compiler::TNode<IntPtrT> tmp808;
    compiler::TNode<IntPtrT> tmp809;
    compiler::TNode<Context> tmp810;
    compiler::TNode<String> tmp811;
    compiler::TNode<IntPtrT> tmp812;
    compiler::TNode<IntPtrT> tmp813;
    compiler::TNode<BoolT> tmp814;
    compiler::TNode<IntPtrT> tmp815;
    compiler::TNode<IntPtrT> tmp816;
    compiler::TNode<BoolT> tmp817;
    compiler::TNode<Context> tmp818;
    compiler::TNode<IntPtrT> tmp819;
    compiler::TNode<IntPtrT> tmp820;
    compiler::TNode<BoolT> tmp821;
    ca_.Bind(&block29, &tmp788, &tmp789, &tmp790, &tmp791, &tmp792, &tmp793, &tmp794, &tmp795, &tmp796, &tmp797, &tmp798, &tmp799, &tmp800, &tmp801, &tmp802, &tmp803, &tmp804, &tmp805, &tmp806, &tmp807, &tmp808, &tmp809, &tmp810, &tmp811, &tmp812, &tmp813, &tmp814, &tmp815, &tmp816, &tmp817, &tmp818, &tmp819, &tmp820, &tmp821);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 162);
    compiler::TNode<IntPtrT> tmp822;
    USE(tmp822);
    tmp822 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadStringLengthAsWord(compiler::TNode<String>{tmp811}));
    compiler::TNode<IntPtrT> tmp823;
    USE(tmp823);
    tmp823 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp810}, compiler::TNode<IntPtrT>{tmp802}, compiler::TNode<IntPtrT>{tmp822}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 161);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 164);
    compiler::TNode<IntPtrT> tmp824;
    USE(tmp824);
    tmp824 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp825;
    USE(tmp825);
    tmp825 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp801}, compiler::TNode<IntPtrT>{tmp824}));
    compiler::TNode<FixedArray> tmp826;
    USE(tmp826);
    tmp826 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray6String(compiler::TNode<FixedArray>{tmp800}, compiler::TNode<IntPtrT>{tmp801}, compiler::TNode<String>{tmp811}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 163);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 166);
    compiler::TNode<Int32T> tmp827;
    USE(tmp827);
    tmp827 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadInstanceType(compiler::TNode<HeapObject>{tmp811}));
    compiler::TNode<BoolT> tmp828;
    USE(tmp828);
    tmp828 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsOneByteStringInstanceType(compiler::TNode<Int32T>{tmp827}));
    compiler::TNode<BoolT> tmp829;
    USE(tmp829);
    tmp829 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32And(compiler::TNode<BoolT>{tmp828}, compiler::TNode<BoolT>{tmp803}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 165);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 156);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 315);
    ca_.Goto(&block28, tmp788, tmp789, tmp790, tmp791, tmp792, tmp793, tmp794, tmp795, tmp796, tmp797, tmp798, tmp799, tmp826, tmp825, tmp823, tmp829, tmp804, tmp805, tmp806, tmp807, tmp808, tmp809, tmp810, tmp811, tmp812, tmp813);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp830;
    compiler::TNode<JSReceiver> tmp831;
    compiler::TNode<String> tmp832;
    compiler::TNode<Number> tmp833;
    compiler::TNode<Object> tmp834;
    compiler::TNode<Object> tmp835;
    compiler::TNode<BuiltinPtr> tmp836;
    compiler::TNode<Map> tmp837;
    compiler::TNode<UintPtrT> tmp838;
    compiler::TNode<IntPtrT> tmp839;
    compiler::TNode<IntPtrT> tmp840;
    compiler::TNode<BuiltinPtr> tmp841;
    compiler::TNode<FixedArray> tmp842;
    compiler::TNode<IntPtrT> tmp843;
    compiler::TNode<IntPtrT> tmp844;
    compiler::TNode<BoolT> tmp845;
    compiler::TNode<UintPtrT> tmp846;
    compiler::TNode<Object> tmp847;
    compiler::TNode<String> tmp848;
    compiler::TNode<String> tmp849;
    compiler::TNode<IntPtrT> tmp850;
    compiler::TNode<IntPtrT> tmp851;
    compiler::TNode<Context> tmp852;
    compiler::TNode<String> tmp853;
    compiler::TNode<IntPtrT> tmp854;
    compiler::TNode<IntPtrT> tmp855;
    ca_.Bind(&block28, &tmp830, &tmp831, &tmp832, &tmp833, &tmp834, &tmp835, &tmp836, &tmp837, &tmp838, &tmp839, &tmp840, &tmp841, &tmp842, &tmp843, &tmp844, &tmp845, &tmp846, &tmp847, &tmp848, &tmp849, &tmp850, &tmp851, &tmp852, &tmp853, &tmp854, &tmp855);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 316);
    compiler::TNode<IntPtrT> tmp856;
    USE(tmp856);
    tmp856 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 277);
    ca_.Goto(&block4, tmp830, tmp831, tmp832, tmp833, tmp834, tmp835, tmp836, tmp837, tmp838, tmp839, tmp856, tmp841, tmp842, tmp843, tmp844, tmp845, tmp846);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp857;
    compiler::TNode<JSReceiver> tmp858;
    compiler::TNode<String> tmp859;
    compiler::TNode<Number> tmp860;
    compiler::TNode<Object> tmp861;
    compiler::TNode<Object> tmp862;
    compiler::TNode<BuiltinPtr> tmp863;
    compiler::TNode<Map> tmp864;
    compiler::TNode<UintPtrT> tmp865;
    compiler::TNode<IntPtrT> tmp866;
    compiler::TNode<IntPtrT> tmp867;
    compiler::TNode<BuiltinPtr> tmp868;
    compiler::TNode<FixedArray> tmp869;
    compiler::TNode<IntPtrT> tmp870;
    compiler::TNode<IntPtrT> tmp871;
    compiler::TNode<BoolT> tmp872;
    compiler::TNode<UintPtrT> tmp873;
    ca_.Bind(&block3, &tmp857, &tmp858, &tmp859, &tmp860, &tmp861, &tmp862, &tmp863, &tmp864, &tmp865, &tmp866, &tmp867, &tmp868, &tmp869, &tmp870, &tmp871, &tmp872, &tmp873);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 320);
    compiler::TNode<BoolT> tmp874;
    USE(tmp874);
    tmp874 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 171);
    compiler::TNode<IntPtrT> tmp875;
    USE(tmp875);
    tmp875 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp876;
    USE(tmp876);
    tmp876 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp867}, compiler::TNode<IntPtrT>{tmp875}));
    ca_.Branch(tmp876, &block38, &block40, tmp857, tmp858, tmp859, tmp860, tmp861, tmp862, tmp863, tmp864, tmp865, tmp866, tmp867, tmp868, tmp869, tmp870, tmp871, tmp872, tmp873, tmp867, tmp866, tmp857, tmp867, tmp866, tmp874);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp877;
    compiler::TNode<JSReceiver> tmp878;
    compiler::TNode<String> tmp879;
    compiler::TNode<Number> tmp880;
    compiler::TNode<Object> tmp881;
    compiler::TNode<Object> tmp882;
    compiler::TNode<BuiltinPtr> tmp883;
    compiler::TNode<Map> tmp884;
    compiler::TNode<UintPtrT> tmp885;
    compiler::TNode<IntPtrT> tmp886;
    compiler::TNode<IntPtrT> tmp887;
    compiler::TNode<BuiltinPtr> tmp888;
    compiler::TNode<FixedArray> tmp889;
    compiler::TNode<IntPtrT> tmp890;
    compiler::TNode<IntPtrT> tmp891;
    compiler::TNode<BoolT> tmp892;
    compiler::TNode<UintPtrT> tmp893;
    compiler::TNode<IntPtrT> tmp894;
    compiler::TNode<IntPtrT> tmp895;
    compiler::TNode<Context> tmp896;
    compiler::TNode<IntPtrT> tmp897;
    compiler::TNode<IntPtrT> tmp898;
    compiler::TNode<BoolT> tmp899;
    ca_.Bind(&block40, &tmp877, &tmp878, &tmp879, &tmp880, &tmp881, &tmp882, &tmp883, &tmp884, &tmp885, &tmp886, &tmp887, &tmp888, &tmp889, &tmp890, &tmp891, &tmp892, &tmp893, &tmp894, &tmp895, &tmp896, &tmp897, &tmp898, &tmp899);
    compiler::TNode<IntPtrT> tmp900;
    USE(tmp900);
    tmp900 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp901;
    USE(tmp901);
    tmp901 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp898}, compiler::TNode<IntPtrT>{tmp900}));
    ca_.Branch(tmp901, &block38, &block39, tmp877, tmp878, tmp879, tmp880, tmp881, tmp882, tmp883, tmp884, tmp885, tmp886, tmp887, tmp888, tmp889, tmp890, tmp891, tmp892, tmp893, tmp894, tmp895, tmp896, tmp897, tmp898, tmp899);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp902;
    compiler::TNode<JSReceiver> tmp903;
    compiler::TNode<String> tmp904;
    compiler::TNode<Number> tmp905;
    compiler::TNode<Object> tmp906;
    compiler::TNode<Object> tmp907;
    compiler::TNode<BuiltinPtr> tmp908;
    compiler::TNode<Map> tmp909;
    compiler::TNode<UintPtrT> tmp910;
    compiler::TNode<IntPtrT> tmp911;
    compiler::TNode<IntPtrT> tmp912;
    compiler::TNode<BuiltinPtr> tmp913;
    compiler::TNode<FixedArray> tmp914;
    compiler::TNode<IntPtrT> tmp915;
    compiler::TNode<IntPtrT> tmp916;
    compiler::TNode<BoolT> tmp917;
    compiler::TNode<UintPtrT> tmp918;
    compiler::TNode<IntPtrT> tmp919;
    compiler::TNode<IntPtrT> tmp920;
    compiler::TNode<Context> tmp921;
    compiler::TNode<IntPtrT> tmp922;
    compiler::TNode<IntPtrT> tmp923;
    compiler::TNode<BoolT> tmp924;
    ca_.Bind(&block38, &tmp902, &tmp903, &tmp904, &tmp905, &tmp906, &tmp907, &tmp908, &tmp909, &tmp910, &tmp911, &tmp912, &tmp913, &tmp914, &tmp915, &tmp916, &tmp917, &tmp918, &tmp919, &tmp920, &tmp921, &tmp922, &tmp923, &tmp924);
    ca_.Goto(&block37, tmp902, tmp903, tmp904, tmp905, tmp906, tmp907, tmp908, tmp909, tmp910, tmp911, tmp912, tmp913, tmp914, tmp915, tmp916, tmp917, tmp918, tmp919, tmp920, tmp921, tmp922, tmp923, tmp924);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp925;
    compiler::TNode<JSReceiver> tmp926;
    compiler::TNode<String> tmp927;
    compiler::TNode<Number> tmp928;
    compiler::TNode<Object> tmp929;
    compiler::TNode<Object> tmp930;
    compiler::TNode<BuiltinPtr> tmp931;
    compiler::TNode<Map> tmp932;
    compiler::TNode<UintPtrT> tmp933;
    compiler::TNode<IntPtrT> tmp934;
    compiler::TNode<IntPtrT> tmp935;
    compiler::TNode<BuiltinPtr> tmp936;
    compiler::TNode<FixedArray> tmp937;
    compiler::TNode<IntPtrT> tmp938;
    compiler::TNode<IntPtrT> tmp939;
    compiler::TNode<BoolT> tmp940;
    compiler::TNode<UintPtrT> tmp941;
    compiler::TNode<IntPtrT> tmp942;
    compiler::TNode<IntPtrT> tmp943;
    compiler::TNode<Context> tmp944;
    compiler::TNode<IntPtrT> tmp945;
    compiler::TNode<IntPtrT> tmp946;
    compiler::TNode<BoolT> tmp947;
    ca_.Bind(&block39, &tmp925, &tmp926, &tmp927, &tmp928, &tmp929, &tmp930, &tmp931, &tmp932, &tmp933, &tmp934, &tmp935, &tmp936, &tmp937, &tmp938, &tmp939, &tmp940, &tmp941, &tmp942, &tmp943, &tmp944, &tmp945, &tmp946, &tmp947);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 173);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 174);
    compiler::TNode<IntPtrT> tmp948;
    USE(tmp948);
    tmp948 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMul(compiler::TNode<IntPtrT>{tmp946}, compiler::TNode<IntPtrT>{tmp945}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 177);
    compiler::TNode<IntPtrT> tmp949;
    USE(tmp949);
    tmp949 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrDiv(compiler::TNode<IntPtrT>{tmp948}, compiler::TNode<IntPtrT>{tmp946}));
    compiler::TNode<BoolT> tmp950;
    USE(tmp950);
    tmp950 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<IntPtrT>{tmp949}, compiler::TNode<IntPtrT>{tmp945}));
    ca_.Branch(tmp950, &block41, &block42, tmp925, tmp926, tmp927, tmp928, tmp929, tmp930, tmp931, tmp932, tmp933, tmp934, tmp935, tmp936, tmp937, tmp938, tmp939, tmp940, tmp941, tmp942, tmp943, tmp944, tmp945, tmp946, tmp947, tmp945, tmp948);
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp951;
    compiler::TNode<JSReceiver> tmp952;
    compiler::TNode<String> tmp953;
    compiler::TNode<Number> tmp954;
    compiler::TNode<Object> tmp955;
    compiler::TNode<Object> tmp956;
    compiler::TNode<BuiltinPtr> tmp957;
    compiler::TNode<Map> tmp958;
    compiler::TNode<UintPtrT> tmp959;
    compiler::TNode<IntPtrT> tmp960;
    compiler::TNode<IntPtrT> tmp961;
    compiler::TNode<BuiltinPtr> tmp962;
    compiler::TNode<FixedArray> tmp963;
    compiler::TNode<IntPtrT> tmp964;
    compiler::TNode<IntPtrT> tmp965;
    compiler::TNode<BoolT> tmp966;
    compiler::TNode<UintPtrT> tmp967;
    compiler::TNode<IntPtrT> tmp968;
    compiler::TNode<IntPtrT> tmp969;
    compiler::TNode<Context> tmp970;
    compiler::TNode<IntPtrT> tmp971;
    compiler::TNode<IntPtrT> tmp972;
    compiler::TNode<BoolT> tmp973;
    compiler::TNode<IntPtrT> tmp974;
    compiler::TNode<IntPtrT> tmp975;
    ca_.Bind(&block41, &tmp951, &tmp952, &tmp953, &tmp954, &tmp955, &tmp956, &tmp957, &tmp958, &tmp959, &tmp960, &tmp961, &tmp962, &tmp963, &tmp964, &tmp965, &tmp966, &tmp967, &tmp968, &tmp969, &tmp970, &tmp971, &tmp972, &tmp973, &tmp974, &tmp975);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 178);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp970);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp977;
    compiler::TNode<JSReceiver> tmp978;
    compiler::TNode<String> tmp979;
    compiler::TNode<Number> tmp980;
    compiler::TNode<Object> tmp981;
    compiler::TNode<Object> tmp982;
    compiler::TNode<BuiltinPtr> tmp983;
    compiler::TNode<Map> tmp984;
    compiler::TNode<UintPtrT> tmp985;
    compiler::TNode<IntPtrT> tmp986;
    compiler::TNode<IntPtrT> tmp987;
    compiler::TNode<BuiltinPtr> tmp988;
    compiler::TNode<FixedArray> tmp989;
    compiler::TNode<IntPtrT> tmp990;
    compiler::TNode<IntPtrT> tmp991;
    compiler::TNode<BoolT> tmp992;
    compiler::TNode<UintPtrT> tmp993;
    compiler::TNode<IntPtrT> tmp994;
    compiler::TNode<IntPtrT> tmp995;
    compiler::TNode<Context> tmp996;
    compiler::TNode<IntPtrT> tmp997;
    compiler::TNode<IntPtrT> tmp998;
    compiler::TNode<BoolT> tmp999;
    compiler::TNode<IntPtrT> tmp1000;
    compiler::TNode<IntPtrT> tmp1001;
    ca_.Bind(&block42, &tmp977, &tmp978, &tmp979, &tmp980, &tmp981, &tmp982, &tmp983, &tmp984, &tmp985, &tmp986, &tmp987, &tmp988, &tmp989, &tmp990, &tmp991, &tmp992, &tmp993, &tmp994, &tmp995, &tmp996, &tmp997, &tmp998, &tmp999, &tmp1000, &tmp1001);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 181);
    compiler::TNode<IntPtrT> tmp1002;
    USE(tmp1002);
    tmp1002 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp996}, compiler::TNode<IntPtrT>{tmp991}, compiler::TNode<IntPtrT>{tmp1001}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 182);
    ca_.Branch(tmp999, &block43, &block44, tmp977, tmp978, tmp979, tmp980, tmp981, tmp982, tmp983, tmp984, tmp985, tmp986, tmp987, tmp988, tmp989, tmp990, tmp1002, tmp992, tmp993, tmp994, tmp995, tmp996, tmp997, tmp998, tmp999, tmp1000, tmp1001);
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp1003;
    compiler::TNode<JSReceiver> tmp1004;
    compiler::TNode<String> tmp1005;
    compiler::TNode<Number> tmp1006;
    compiler::TNode<Object> tmp1007;
    compiler::TNode<Object> tmp1008;
    compiler::TNode<BuiltinPtr> tmp1009;
    compiler::TNode<Map> tmp1010;
    compiler::TNode<UintPtrT> tmp1011;
    compiler::TNode<IntPtrT> tmp1012;
    compiler::TNode<IntPtrT> tmp1013;
    compiler::TNode<BuiltinPtr> tmp1014;
    compiler::TNode<FixedArray> tmp1015;
    compiler::TNode<IntPtrT> tmp1016;
    compiler::TNode<IntPtrT> tmp1017;
    compiler::TNode<BoolT> tmp1018;
    compiler::TNode<UintPtrT> tmp1019;
    compiler::TNode<IntPtrT> tmp1020;
    compiler::TNode<IntPtrT> tmp1021;
    compiler::TNode<Context> tmp1022;
    compiler::TNode<IntPtrT> tmp1023;
    compiler::TNode<IntPtrT> tmp1024;
    compiler::TNode<BoolT> tmp1025;
    compiler::TNode<IntPtrT> tmp1026;
    compiler::TNode<IntPtrT> tmp1027;
    ca_.Bind(&block43, &tmp1003, &tmp1004, &tmp1005, &tmp1006, &tmp1007, &tmp1008, &tmp1009, &tmp1010, &tmp1011, &tmp1012, &tmp1013, &tmp1014, &tmp1015, &tmp1016, &tmp1017, &tmp1018, &tmp1019, &tmp1020, &tmp1021, &tmp1022, &tmp1023, &tmp1024, &tmp1025, &tmp1026, &tmp1027);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 184);
    compiler::TNode<IntPtrT> tmp1028;
    USE(tmp1028);
    tmp1028 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp1029;
    USE(tmp1029);
    tmp1029 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp1016}, compiler::TNode<IntPtrT>{tmp1028}));
    compiler::TNode<Smi> tmp1030;
    USE(tmp1030);
    tmp1030 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp1026}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 183);
    compiler::TNode<FixedArray> tmp1031;
    USE(tmp1031);
    tmp1031 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray>{tmp1015}, compiler::TNode<IntPtrT>{tmp1016}, compiler::TNode<Smi>{tmp1030}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 182);
    ca_.Goto(&block44, tmp1003, tmp1004, tmp1005, tmp1006, tmp1007, tmp1008, tmp1009, tmp1010, tmp1011, tmp1012, tmp1013, tmp1014, tmp1031, tmp1029, tmp1017, tmp1018, tmp1019, tmp1020, tmp1021, tmp1022, tmp1023, tmp1024, tmp1025, tmp1026, tmp1027);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp1032;
    compiler::TNode<JSReceiver> tmp1033;
    compiler::TNode<String> tmp1034;
    compiler::TNode<Number> tmp1035;
    compiler::TNode<Object> tmp1036;
    compiler::TNode<Object> tmp1037;
    compiler::TNode<BuiltinPtr> tmp1038;
    compiler::TNode<Map> tmp1039;
    compiler::TNode<UintPtrT> tmp1040;
    compiler::TNode<IntPtrT> tmp1041;
    compiler::TNode<IntPtrT> tmp1042;
    compiler::TNode<BuiltinPtr> tmp1043;
    compiler::TNode<FixedArray> tmp1044;
    compiler::TNode<IntPtrT> tmp1045;
    compiler::TNode<IntPtrT> tmp1046;
    compiler::TNode<BoolT> tmp1047;
    compiler::TNode<UintPtrT> tmp1048;
    compiler::TNode<IntPtrT> tmp1049;
    compiler::TNode<IntPtrT> tmp1050;
    compiler::TNode<Context> tmp1051;
    compiler::TNode<IntPtrT> tmp1052;
    compiler::TNode<IntPtrT> tmp1053;
    compiler::TNode<BoolT> tmp1054;
    compiler::TNode<IntPtrT> tmp1055;
    compiler::TNode<IntPtrT> tmp1056;
    ca_.Bind(&block44, &tmp1032, &tmp1033, &tmp1034, &tmp1035, &tmp1036, &tmp1037, &tmp1038, &tmp1039, &tmp1040, &tmp1041, &tmp1042, &tmp1043, &tmp1044, &tmp1045, &tmp1046, &tmp1047, &tmp1048, &tmp1049, &tmp1050, &tmp1051, &tmp1052, &tmp1053, &tmp1054, &tmp1055, &tmp1056);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 170);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 320);
    ca_.Goto(&block37, tmp1032, tmp1033, tmp1034, tmp1035, tmp1036, tmp1037, tmp1038, tmp1039, tmp1040, tmp1041, tmp1042, tmp1043, tmp1044, tmp1045, tmp1046, tmp1047, tmp1048, tmp1049, tmp1050, tmp1051, tmp1052, tmp1053, tmp1054);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp1057;
    compiler::TNode<JSReceiver> tmp1058;
    compiler::TNode<String> tmp1059;
    compiler::TNode<Number> tmp1060;
    compiler::TNode<Object> tmp1061;
    compiler::TNode<Object> tmp1062;
    compiler::TNode<BuiltinPtr> tmp1063;
    compiler::TNode<Map> tmp1064;
    compiler::TNode<UintPtrT> tmp1065;
    compiler::TNode<IntPtrT> tmp1066;
    compiler::TNode<IntPtrT> tmp1067;
    compiler::TNode<BuiltinPtr> tmp1068;
    compiler::TNode<FixedArray> tmp1069;
    compiler::TNode<IntPtrT> tmp1070;
    compiler::TNode<IntPtrT> tmp1071;
    compiler::TNode<BoolT> tmp1072;
    compiler::TNode<UintPtrT> tmp1073;
    compiler::TNode<IntPtrT> tmp1074;
    compiler::TNode<IntPtrT> tmp1075;
    compiler::TNode<Context> tmp1076;
    compiler::TNode<IntPtrT> tmp1077;
    compiler::TNode<IntPtrT> tmp1078;
    compiler::TNode<BoolT> tmp1079;
    ca_.Bind(&block37, &tmp1057, &tmp1058, &tmp1059, &tmp1060, &tmp1061, &tmp1062, &tmp1063, &tmp1064, &tmp1065, &tmp1066, &tmp1067, &tmp1068, &tmp1069, &tmp1070, &tmp1071, &tmp1072, &tmp1073, &tmp1074, &tmp1075, &tmp1076, &tmp1077, &tmp1078, &tmp1079);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 323);
    compiler::TNode<String> tmp1080;
    USE(tmp1080);
    tmp1080 = ca_.UncheckedCast<String>(ArrayJoinBuiltinsFromDSLAssembler(state_).BufferJoin(compiler::TNode<Context>{tmp1057}, ArrayJoinBuiltinsFromDSLAssembler::Buffer{compiler::TNode<FixedArray>{tmp1069}, compiler::TNode<IntPtrT>{tmp1070}, compiler::TNode<IntPtrT>{tmp1071}, compiler::TNode<BoolT>{tmp1072}}, compiler::TNode<String>{tmp1059}));
    ca_.Goto(&block1, tmp1057, tmp1058, tmp1059, tmp1060, tmp1061, tmp1062, tmp1063, tmp1080);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp1081;
    compiler::TNode<JSReceiver> tmp1082;
    compiler::TNode<String> tmp1083;
    compiler::TNode<Number> tmp1084;
    compiler::TNode<Object> tmp1085;
    compiler::TNode<Object> tmp1086;
    compiler::TNode<BuiltinPtr> tmp1087;
    compiler::TNode<String> tmp1088;
    ca_.Bind(&block1, &tmp1081, &tmp1082, &tmp1083, &tmp1084, &tmp1085, &tmp1086, &tmp1087, &tmp1088);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 262);
    ca_.Goto(&block45, tmp1081, tmp1082, tmp1083, tmp1084, tmp1085, tmp1086, tmp1087, tmp1088);
  }

    compiler::TNode<Context> tmp1089;
    compiler::TNode<JSReceiver> tmp1090;
    compiler::TNode<String> tmp1091;
    compiler::TNode<Number> tmp1092;
    compiler::TNode<Object> tmp1093;
    compiler::TNode<Object> tmp1094;
    compiler::TNode<BuiltinPtr> tmp1095;
    compiler::TNode<String> tmp1096;
    ca_.Bind(&block45, &tmp1089, &tmp1090, &tmp1091, &tmp1092, &tmp1093, &tmp1094, &tmp1095, &tmp1096);
  return compiler::TNode<String>{tmp1096};
}

compiler::TNode<FixedArray> ArrayJoinBuiltinsFromDSLAssembler::StoreAndGrowFixedArray10JSReceiver(compiler::TNode<FixedArray> p_fixedArray, compiler::TNode<IntPtrT> p_index, compiler::TNode<JSReceiver> p_element) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, IntPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, IntPtrT, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, IntPtrT, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, FixedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_fixedArray, p_index, p_element);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<IntPtrT> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 134);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp0}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 135);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThanOrEqual(compiler::TNode<IntPtrT>{tmp1}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, tmp1, tmp2, tmp3);
  }

  if (block3.is_used()) {
    compiler::TNode<FixedArray> tmp5;
    compiler::TNode<IntPtrT> tmp6;
    compiler::TNode<JSReceiver> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7, &tmp8);
    CodeStubAssembler(state_).FailAssert("Torque assert \'index <= length\' failed", "../../src/builtins/array-join.tq", 135);
  }

  if (block2.is_used()) {
    compiler::TNode<FixedArray> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    ca_.Bind(&block2, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 136);
    compiler::TNode<BoolT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Branch(tmp13, &block4, &block5, tmp9, tmp10, tmp11, tmp12);
  }

  if (block4.is_used()) {
    compiler::TNode<FixedArray> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<IntPtrT> tmp17;
    ca_.Bind(&block4, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 137);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp14}, compiler::TNode<IntPtrT>{tmp15}, compiler::TNode<HeapObject>{tmp16});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 138);
    ca_.Goto(&block1, tmp14, tmp15, tmp16, tmp14);
  }

  if (block5.is_used()) {
    compiler::TNode<FixedArray> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    compiler::TNode<JSReceiver> tmp20;
    compiler::TNode<IntPtrT> tmp21;
    ca_.Bind(&block5, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 141);
    compiler::TNode<IntPtrT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<IntPtrT>{tmp21}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 142);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp19}, compiler::TNode<IntPtrT>{tmp22}));
    ca_.Branch(tmp23, &block7, &block8, tmp18, tmp19, tmp20, tmp21, tmp22);
  }

  if (block8.is_used()) {
    compiler::TNode<FixedArray> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<JSReceiver> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    compiler::TNode<IntPtrT> tmp28;
    ca_.Bind(&block8, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    CodeStubAssembler(state_).FailAssert("Torque assert \'index < newLength\' failed", "../../src/builtins/array-join.tq", 142);
  }

  if (block7.is_used()) {
    compiler::TNode<FixedArray> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<JSReceiver> tmp31;
    compiler::TNode<IntPtrT> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    ca_.Bind(&block7, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 144);
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<FixedArray> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArray>{tmp29}, compiler::TNode<IntPtrT>{tmp34}, compiler::TNode<IntPtrT>{tmp32}, compiler::TNode<IntPtrT>{tmp33}, CodeStubAssembler::ExtractFixedArrayFlag::kFixedArrays));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 143);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 145);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp35}, compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<HeapObject>{tmp31});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 146);
    ca_.Goto(&block1, tmp29, tmp30, tmp31, tmp35);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp36;
    compiler::TNode<IntPtrT> tmp37;
    compiler::TNode<JSReceiver> tmp38;
    compiler::TNode<FixedArray> tmp39;
    ca_.Bind(&block1, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 132);
    ca_.Goto(&block9, tmp36, tmp37, tmp38, tmp39);
  }

    compiler::TNode<FixedArray> tmp40;
    compiler::TNode<IntPtrT> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<FixedArray> tmp43;
    ca_.Bind(&block9, &tmp40, &tmp41, &tmp42, &tmp43);
  return compiler::TNode<FixedArray>{tmp43};
}

compiler::TNode<Object> ArrayJoinBuiltinsFromDSLAssembler::CycleProtectedArrayJoin7JSArray(compiler::TNode<Context> p_context, bool p_useToLocaleString, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Object> p_sepObj, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, JSReceiver, String, Number, Object, Object, Object> block16(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, Object, JSReceiver, Object> block17(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, Object> block15(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, Object> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_len, p_sepObj, p_locales, p_options);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 539);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp3}, compiler::TNode<HeapObject>{tmp6}));
    ca_.Branch(tmp7, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<JSReceiver> tmp9;
    compiler::TNode<Number> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block5, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    ca_.Bind(&block3, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    compiler::TNode<String> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp14}, compiler::TNode<Object>{tmp17}));
    ca_.Goto(&block4, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    ca_.Bind(&block5, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    compiler::TNode<String> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string(","));
    ca_.Goto(&block4, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<String> tmp34;
    ca_.Bind(&block4, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 538);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 543);
    compiler::TNode<Number> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp30}, compiler::TNode<Number>{tmp35}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp30);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block10, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp30);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<String> tmp42;
    compiler::TNode<Number> tmp43;
    ca_.Bind(&block9, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.Goto(&block8, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Number> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<String> tmp50;
    compiler::TNode<Number> tmp51;
    ca_.Bind(&block10, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.Goto(&block7, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<String> tmp58;
    ca_.Bind(&block8, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    ArrayJoinBuiltinsFromDSLAssembler(state_).JoinStackPushInline(compiler::TNode<Context>{tmp52}, compiler::TNode<JSReceiver>{tmp53}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block11, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp53);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block12, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp53);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<String> tmp65;
    compiler::TNode<JSReceiver> tmp66;
    ca_.Bind(&block11, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.Goto(&block6, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<JSReceiver> tmp68;
    compiler::TNode<Number> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<String> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    ca_.Bind(&block12, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    ca_.Goto(&block7, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<String> tmp81;
    ca_.Bind(&block6, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 546);
    compiler::TNode<Object> tmp82;
    USE(tmp82);
    compiler::CodeAssemblerExceptionHandlerLabel catch83_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch83_label);
    tmp82 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).ArrayJoin7JSArray(compiler::TNode<Context>{tmp75}, p_useToLocaleString, compiler::TNode<JSReceiver>{tmp76}, compiler::TNode<String>{tmp81}, compiler::TNode<Number>{tmp77}, compiler::TNode<Object>{tmp79}, compiler::TNode<Object>{tmp80}));
    }
    if (catch83_label.is_used()) {
      compiler::CodeAssemblerLabel catch83_skip(&ca_);
      ca_.Goto(&catch83_skip);
      compiler::TNode<Object> catch83_exception_object;
      ca_.Bind(&catch83_label, &catch83_exception_object);
      ca_.Goto(&block16, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp76, tmp81, tmp77, tmp79, tmp80, catch83_exception_object);
      ca_.Bind(&catch83_skip);
    }
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 545);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 547);
    compiler::CodeAssemblerExceptionHandlerLabel catch84_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch84_label);
    ArrayJoinBuiltinsFromDSLAssembler(state_).JoinStackPopInline(compiler::TNode<Context>{tmp75}, compiler::TNode<JSReceiver>{tmp76});
    }
    if (catch84_label.is_used()) {
      compiler::CodeAssemblerLabel catch84_skip(&ca_);
      ca_.Goto(&catch84_skip);
      compiler::TNode<Object> catch84_exception_object;
      ca_.Bind(&catch84_label, &catch84_exception_object);
      ca_.Goto(&block17, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp76, catch84_exception_object);
      ca_.Bind(&catch84_skip);
    }
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 548);
    ca_.Goto(&block1, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp82);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp85;
    compiler::TNode<JSReceiver> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<String> tmp91;
    compiler::TNode<JSReceiver> tmp92;
    compiler::TNode<String> tmp93;
    compiler::TNode<Number> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block16, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 546);
    ca_.Goto(&block15, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp97);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp98;
    compiler::TNode<JSReceiver> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<String> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<JSReceiver> tmp106;
    compiler::TNode<Object> tmp107;
    ca_.Bind(&block17, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 547);
    ca_.Goto(&block15, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp107);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp108;
    compiler::TNode<JSReceiver> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<String> tmp114;
    compiler::TNode<Object> tmp115;
    ca_.Bind(&block15, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 550);
    ArrayJoinBuiltinsFromDSLAssembler(state_).JoinStackPopInline(compiler::TNode<Context>{tmp108}, compiler::TNode<JSReceiver>{tmp109});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 551);
    CodeStubAssembler(state_).CallRuntime(Runtime::kReThrow, tmp108, tmp115);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp117;
    compiler::TNode<JSReceiver> tmp118;
    compiler::TNode<Number> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<String> tmp123;
    ca_.Bind(&block7, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 554);
    compiler::TNode<String> tmp124;
    USE(tmp124);
    tmp124 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.Goto(&block1, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp124);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp125;
    compiler::TNode<JSReceiver> tmp126;
    compiler::TNode<Number> tmp127;
    compiler::TNode<Object> tmp128;
    compiler::TNode<Object> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<Object> tmp131;
    ca_.Bind(&block1, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 532);
    ca_.Goto(&block18, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131);
  }

    compiler::TNode<Context> tmp132;
    compiler::TNode<JSReceiver> tmp133;
    compiler::TNode<Number> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<Object> tmp138;
    ca_.Bind(&block18, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138);
  return compiler::TNode<Object>{tmp138};
}

compiler::TNode<Object> ArrayJoinBuiltinsFromDSLAssembler::CycleProtectedArrayJoin12JSTypedArray(compiler::TNode<Context> p_context, bool p_useToLocaleString, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Object> p_sepObj, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, JSReceiver, String, Number, Object, Object, Object> block16(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, Object, JSReceiver, Object> block17(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String, Object> block15(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, String> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Object, Object, Object, Object> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_len, p_sepObj, p_locales, p_options);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 539);
    compiler::TNode<Oddball> tmp6;
    USE(tmp6);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp3}, compiler::TNode<HeapObject>{tmp6}));
    ca_.Branch(tmp7, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<JSReceiver> tmp9;
    compiler::TNode<Number> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block5, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    ca_.Bind(&block3, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    compiler::TNode<String> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp14}, compiler::TNode<Object>{tmp17}));
    ca_.Goto(&block4, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    ca_.Bind(&block5, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    compiler::TNode<String> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string(","));
    ca_.Goto(&block4, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<String> tmp34;
    ca_.Bind(&block4, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 538);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 543);
    compiler::TNode<Number> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp30}, compiler::TNode<Number>{tmp35}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp30);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block10, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp30);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<String> tmp42;
    compiler::TNode<Number> tmp43;
    ca_.Bind(&block9, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.Goto(&block8, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Number> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<String> tmp50;
    compiler::TNode<Number> tmp51;
    ca_.Bind(&block10, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.Goto(&block7, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<String> tmp58;
    ca_.Bind(&block8, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    ArrayJoinBuiltinsFromDSLAssembler(state_).JoinStackPushInline(compiler::TNode<Context>{tmp52}, compiler::TNode<JSReceiver>{tmp53}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block11, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp53);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block12, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp53);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<String> tmp65;
    compiler::TNode<JSReceiver> tmp66;
    ca_.Bind(&block11, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.Goto(&block6, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<JSReceiver> tmp68;
    compiler::TNode<Number> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<String> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    ca_.Bind(&block12, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    ca_.Goto(&block7, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<String> tmp81;
    ca_.Bind(&block6, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 546);
    compiler::TNode<Object> tmp82;
    USE(tmp82);
    compiler::CodeAssemblerExceptionHandlerLabel catch83_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch83_label);
    tmp82 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).ArrayJoin12JSTypedArray(compiler::TNode<Context>{tmp75}, p_useToLocaleString, compiler::TNode<JSReceiver>{tmp76}, compiler::TNode<String>{tmp81}, compiler::TNode<Number>{tmp77}, compiler::TNode<Object>{tmp79}, compiler::TNode<Object>{tmp80}));
    }
    if (catch83_label.is_used()) {
      compiler::CodeAssemblerLabel catch83_skip(&ca_);
      ca_.Goto(&catch83_skip);
      compiler::TNode<Object> catch83_exception_object;
      ca_.Bind(&catch83_label, &catch83_exception_object);
      ca_.Goto(&block16, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp76, tmp81, tmp77, tmp79, tmp80, catch83_exception_object);
      ca_.Bind(&catch83_skip);
    }
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 545);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 547);
    compiler::CodeAssemblerExceptionHandlerLabel catch84_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch84_label);
    ArrayJoinBuiltinsFromDSLAssembler(state_).JoinStackPopInline(compiler::TNode<Context>{tmp75}, compiler::TNode<JSReceiver>{tmp76});
    }
    if (catch84_label.is_used()) {
      compiler::CodeAssemblerLabel catch84_skip(&ca_);
      ca_.Goto(&catch84_skip);
      compiler::TNode<Object> catch84_exception_object;
      ca_.Bind(&catch84_label, &catch84_exception_object);
      ca_.Goto(&block17, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp76, catch84_exception_object);
      ca_.Bind(&catch84_skip);
    }
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 548);
    ca_.Goto(&block1, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp82);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp85;
    compiler::TNode<JSReceiver> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<String> tmp91;
    compiler::TNode<JSReceiver> tmp92;
    compiler::TNode<String> tmp93;
    compiler::TNode<Number> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block16, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 546);
    ca_.Goto(&block15, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp97);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp98;
    compiler::TNode<JSReceiver> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<String> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<JSReceiver> tmp106;
    compiler::TNode<Object> tmp107;
    ca_.Bind(&block17, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 547);
    ca_.Goto(&block15, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp107);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp108;
    compiler::TNode<JSReceiver> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<String> tmp114;
    compiler::TNode<Object> tmp115;
    ca_.Bind(&block15, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 550);
    ArrayJoinBuiltinsFromDSLAssembler(state_).JoinStackPopInline(compiler::TNode<Context>{tmp108}, compiler::TNode<JSReceiver>{tmp109});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 551);
    CodeStubAssembler(state_).CallRuntime(Runtime::kReThrow, tmp108, tmp115);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp117;
    compiler::TNode<JSReceiver> tmp118;
    compiler::TNode<Number> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<String> tmp123;
    ca_.Bind(&block7, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 554);
    compiler::TNode<String> tmp124;
    USE(tmp124);
    tmp124 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.Goto(&block1, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp124);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp125;
    compiler::TNode<JSReceiver> tmp126;
    compiler::TNode<Number> tmp127;
    compiler::TNode<Object> tmp128;
    compiler::TNode<Object> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<Object> tmp131;
    ca_.Bind(&block1, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 532);
    ca_.Goto(&block18, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131);
  }

    compiler::TNode<Context> tmp132;
    compiler::TNode<JSReceiver> tmp133;
    compiler::TNode<Number> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<Object> tmp138;
    ca_.Bind(&block18, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138);
  return compiler::TNode<Object>{tmp138};
}

compiler::TNode<FixedArray> ArrayJoinBuiltinsFromDSLAssembler::StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray> p_fixedArray, compiler::TNode<IntPtrT> p_index, compiler::TNode<Smi> p_element) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, IntPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, IntPtrT, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, IntPtrT, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, FixedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_fixedArray, p_index, p_element);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<IntPtrT> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 134);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp0}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 135);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThanOrEqual(compiler::TNode<IntPtrT>{tmp1}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, tmp1, tmp2, tmp3);
  }

  if (block3.is_used()) {
    compiler::TNode<FixedArray> tmp5;
    compiler::TNode<IntPtrT> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7, &tmp8);
    CodeStubAssembler(state_).FailAssert("Torque assert \'index <= length\' failed", "../../src/builtins/array-join.tq", 135);
  }

  if (block2.is_used()) {
    compiler::TNode<FixedArray> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    ca_.Bind(&block2, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 136);
    compiler::TNode<BoolT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Branch(tmp13, &block4, &block5, tmp9, tmp10, tmp11, tmp12);
  }

  if (block4.is_used()) {
    compiler::TNode<FixedArray> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<IntPtrT> tmp17;
    ca_.Bind(&block4, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 137);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp14}, compiler::TNode<IntPtrT>{tmp15}, compiler::TNode<Smi>{tmp16});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 138);
    ca_.Goto(&block1, tmp14, tmp15, tmp16, tmp14);
  }

  if (block5.is_used()) {
    compiler::TNode<FixedArray> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<IntPtrT> tmp21;
    ca_.Bind(&block5, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 141);
    compiler::TNode<IntPtrT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<IntPtrT>{tmp21}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 142);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp19}, compiler::TNode<IntPtrT>{tmp22}));
    ca_.Branch(tmp23, &block7, &block8, tmp18, tmp19, tmp20, tmp21, tmp22);
  }

  if (block8.is_used()) {
    compiler::TNode<FixedArray> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    compiler::TNode<IntPtrT> tmp28;
    ca_.Bind(&block8, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    CodeStubAssembler(state_).FailAssert("Torque assert \'index < newLength\' failed", "../../src/builtins/array-join.tq", 142);
  }

  if (block7.is_used()) {
    compiler::TNode<FixedArray> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Smi> tmp31;
    compiler::TNode<IntPtrT> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    ca_.Bind(&block7, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 144);
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<FixedArray> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArray>{tmp29}, compiler::TNode<IntPtrT>{tmp34}, compiler::TNode<IntPtrT>{tmp32}, compiler::TNode<IntPtrT>{tmp33}, CodeStubAssembler::ExtractFixedArrayFlag::kFixedArrays));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 143);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 145);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp35}, compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<Smi>{tmp31});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 146);
    ca_.Goto(&block1, tmp29, tmp30, tmp31, tmp35);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp36;
    compiler::TNode<IntPtrT> tmp37;
    compiler::TNode<Smi> tmp38;
    compiler::TNode<FixedArray> tmp39;
    ca_.Bind(&block1, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 132);
    ca_.Goto(&block9, tmp36, tmp37, tmp38, tmp39);
  }

    compiler::TNode<FixedArray> tmp40;
    compiler::TNode<IntPtrT> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<FixedArray> tmp43;
    ca_.Bind(&block9, &tmp40, &tmp41, &tmp42, &tmp43);
  return compiler::TNode<FixedArray>{tmp43};
}

compiler::TNode<FixedArray> ArrayJoinBuiltinsFromDSLAssembler::StoreAndGrowFixedArray6String(compiler::TNode<FixedArray> p_fixedArray, compiler::TNode<IntPtrT> p_index, compiler::TNode<String> p_element) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, IntPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, IntPtrT, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, IntPtrT, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, FixedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_fixedArray, p_index, p_element);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<IntPtrT> tmp1;
    compiler::TNode<String> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 134);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp0}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 135);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThanOrEqual(compiler::TNode<IntPtrT>{tmp1}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, tmp1, tmp2, tmp3);
  }

  if (block3.is_used()) {
    compiler::TNode<FixedArray> tmp5;
    compiler::TNode<IntPtrT> tmp6;
    compiler::TNode<String> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7, &tmp8);
    CodeStubAssembler(state_).FailAssert("Torque assert \'index <= length\' failed", "../../src/builtins/array-join.tq", 135);
  }

  if (block2.is_used()) {
    compiler::TNode<FixedArray> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<String> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    ca_.Bind(&block2, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 136);
    compiler::TNode<BoolT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Branch(tmp13, &block4, &block5, tmp9, tmp10, tmp11, tmp12);
  }

  if (block4.is_used()) {
    compiler::TNode<FixedArray> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    compiler::TNode<String> tmp16;
    compiler::TNode<IntPtrT> tmp17;
    ca_.Bind(&block4, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 137);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp14}, compiler::TNode<IntPtrT>{tmp15}, compiler::TNode<HeapObject>{tmp16});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 138);
    ca_.Goto(&block1, tmp14, tmp15, tmp16, tmp14);
  }

  if (block5.is_used()) {
    compiler::TNode<FixedArray> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    compiler::TNode<String> tmp20;
    compiler::TNode<IntPtrT> tmp21;
    ca_.Bind(&block5, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 141);
    compiler::TNode<IntPtrT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<IntPtrT>{tmp21}));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 142);
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp19}, compiler::TNode<IntPtrT>{tmp22}));
    ca_.Branch(tmp23, &block7, &block8, tmp18, tmp19, tmp20, tmp21, tmp22);
  }

  if (block8.is_used()) {
    compiler::TNode<FixedArray> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<String> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    compiler::TNode<IntPtrT> tmp28;
    ca_.Bind(&block8, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    CodeStubAssembler(state_).FailAssert("Torque assert \'index < newLength\' failed", "../../src/builtins/array-join.tq", 142);
  }

  if (block7.is_used()) {
    compiler::TNode<FixedArray> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<String> tmp31;
    compiler::TNode<IntPtrT> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    ca_.Bind(&block7, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 144);
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<FixedArray> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArray>{tmp29}, compiler::TNode<IntPtrT>{tmp34}, compiler::TNode<IntPtrT>{tmp32}, compiler::TNode<IntPtrT>{tmp33}, CodeStubAssembler::ExtractFixedArrayFlag::kFixedArrays));
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 143);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 145);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp35}, compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<HeapObject>{tmp31});
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 146);
    ca_.Goto(&block1, tmp29, tmp30, tmp31, tmp35);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp36;
    compiler::TNode<IntPtrT> tmp37;
    compiler::TNode<String> tmp38;
    compiler::TNode<FixedArray> tmp39;
    ca_.Bind(&block1, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../src/builtins/array-join.tq", 132);
    ca_.Goto(&block9, tmp36, tmp37, tmp38, tmp39);
  }

    compiler::TNode<FixedArray> tmp40;
    compiler::TNode<IntPtrT> tmp41;
    compiler::TNode<String> tmp42;
    compiler::TNode<FixedArray> tmp43;
    ca_.Bind(&block9, &tmp40, &tmp41, &tmp42, &tmp43);
  return compiler::TNode<FixedArray>{tmp43};
}

}  // namespace internal
}  // namespace v8

