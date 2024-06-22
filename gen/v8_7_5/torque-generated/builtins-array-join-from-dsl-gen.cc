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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 21);
    compiler::TNode<JSArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 22);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp3, tmp4});
    compiler::TNode<NumberDictionary> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<NumberDictionary>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast18ATNumberDictionary(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 24);
    compiler::TNode<UintPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr20UT5ATSmi10HeapNumber(compiler::TNode<Number>{tmp2}));
    compiler::TNode<IntPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).Signed(compiler::TNode<UintPtrT>{tmp7}));
    compiler::TNode<Object> tmp9;
    USE(tmp9);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    tmp9 = CodeStubAssembler(state_).BasicLoadNumberDictionaryElement(compiler::TNode<NumberDictionary>{tmp6}, compiler::TNode<IntPtrT>{tmp8}, &label0, &label1);
    ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp6, tmp6, tmp8, tmp9);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp0, tmp1, tmp2, tmp3, tmp6, tmp6, tmp8);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block7, tmp0, tmp1, tmp2, tmp3, tmp6, tmp6, tmp8);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<JSArray> tmp13;
    compiler::TNode<NumberDictionary> tmp14;
    compiler::TNode<NumberDictionary> tmp15;
    compiler::TNode<IntPtrT> tmp16;
    ca_.Bind(&block6, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.Goto(&block4, tmp10, tmp11, tmp12, tmp13, tmp14);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSReceiver> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<JSArray> tmp20;
    compiler::TNode<NumberDictionary> tmp21;
    compiler::TNode<NumberDictionary> tmp22;
    compiler::TNode<IntPtrT> tmp23;
    ca_.Bind(&block7, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.Goto(&block2, tmp17, tmp18, tmp19, tmp20, tmp21);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<JSArray> tmp27;
    compiler::TNode<NumberDictionary> tmp28;
    compiler::TNode<NumberDictionary> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block5, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    CodeStubAssembler(state_).Return(tmp31);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<Number> tmp34;
    compiler::TNode<JSArray> tmp35;
    compiler::TNode<NumberDictionary> tmp36;
    ca_.Bind(&block4, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 28);
    compiler::TNode<Object> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp32}, compiler::TNode<Object>{tmp33}, compiler::TNode<Object>{tmp34}));
    CodeStubAssembler(state_).Return(tmp37);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<JSArray> tmp41;
    compiler::TNode<NumberDictionary> tmp42;
    ca_.Bind(&block2, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 31);
    compiler::TNode<String> tmp43;
    USE(tmp43);
    tmp43 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    CodeStubAssembler(state_).Return(tmp43);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 37);
    compiler::TNode<JSArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 38);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp3, tmp4});
    compiler::TNode<FixedArray> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 39);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp6}, compiler::TNode<Smi>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 40);
    compiler::TNode<Oddball> tmp9;
    USE(tmp9);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp8}, compiler::TNode<HeapObject>{tmp9}));
    ca_.Branch(tmp10, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp6, tmp8);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSArray> tmp14;
    compiler::TNode<FixedArray> tmp15;
    compiler::TNode<Object> tmp16;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    compiler::TNode<String> tmp17;
    USE(tmp17);
    tmp17 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.Goto(&block4, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<JSReceiver> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<JSArray> tmp21;
    compiler::TNode<FixedArray> tmp22;
    compiler::TNode<Object> tmp23;
    ca_.Bind(&block2, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.Goto(&block3, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp23);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<JSArray> tmp27;
    compiler::TNode<FixedArray> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<String> tmp30;
    ca_.Bind(&block4, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.Goto(&block3, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<JSArray> tmp34;
    compiler::TNode<FixedArray> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Object> tmp37;
    ca_.Bind(&block3, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    CodeStubAssembler(state_).Return(tmp37);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 45);
    compiler::TNode<JSArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 47);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp4);
    compiler::TNode<FixedArrayBase>tmp5 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp3, tmp4});
    compiler::TNode<FixedDoubleArray> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 46);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 49);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 48);
    compiler::TNode<Float64T> tmp8;
    USE(tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp8 = CodeStubAssembler(state_).LoadDoubleWithHoleCheck(compiler::TNode<FixedDoubleArray>{tmp6}, compiler::TNode<Smi>{tmp7}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp6, tmp6, tmp7, tmp8);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp6, tmp6, tmp7);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<Number> tmp11;
    compiler::TNode<JSArray> tmp12;
    compiler::TNode<FixedDoubleArray> tmp13;
    compiler::TNode<FixedDoubleArray> tmp14;
    compiler::TNode<Smi> tmp15;
    ca_.Bind(&block4, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.Goto(&block2, tmp9, tmp10, tmp11, tmp12, tmp13);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<JSArray> tmp19;
    compiler::TNode<FixedDoubleArray> tmp20;
    compiler::TNode<FixedDoubleArray> tmp21;
    compiler::TNode<Smi> tmp22;
    compiler::TNode<Float64T> tmp23;
    ca_.Bind(&block3, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.Goto(&block1, tmp16, tmp17, tmp18, tmp19, tmp20, tmp23);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<JSArray> tmp27;
    compiler::TNode<FixedDoubleArray> tmp28;
    ca_.Bind(&block2, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 49);
    compiler::TNode<String> tmp29;
    USE(tmp29);
    tmp29 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    CodeStubAssembler(state_).Return(tmp29);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<JSReceiver> tmp31;
    compiler::TNode<Number> tmp32;
    compiler::TNode<JSArray> tmp33;
    compiler::TNode<FixedDoubleArray> tmp34;
    compiler::TNode<Float64T> tmp35;
    ca_.Bind(&block1, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 48);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 50);
    compiler::TNode<HeapNumber> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<HeapNumber>(CodeStubAssembler(state_).AllocateHeapNumberWithValue(compiler::TNode<Float64T>{tmp35}));
    CodeStubAssembler(state_).Return(tmp36);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 65);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 67);
    compiler::TNode<Object> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapObject18ATconstexpr_string("toLocaleString"));
    compiler::TNode<Object> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp10}, compiler::TNode<Object>{tmp11}, compiler::TNode<Object>{tmp14}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 69);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 70);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 71);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 72);
    compiler::TNode<Object> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp31}, compiler::TNode<JSReceiver>{tmp36}, compiler::TNode<Object>{tmp32}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 71);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 73);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 74);
    compiler::TNode<Object> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp47}, compiler::TNode<JSReceiver>{tmp52}, compiler::TNode<Object>{tmp48}, compiler::TNode<Object>{tmp49}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 73);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 76);
    compiler::TNode<Object> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp55}, compiler::TNode<JSReceiver>{tmp60}, compiler::TNode<Object>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp58}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 73);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 71);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 78);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 81);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 96);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 98);
    compiler::TNode<JSArray> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<JSArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSArray(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp13}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 99);
    compiler::TNode<IntPtrT> tmp17 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp17);
    compiler::TNode<Map>tmp18 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp16, tmp17});
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<HeapObject>{tmp14}, compiler::TNode<HeapObject>{tmp18}));
    ca_.Branch(tmp19, &block5, &block6, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<BuiltinPtr> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<Map> tmp23;
    compiler::TNode<Number> tmp24;
    compiler::TNode<JSArray> tmp25;
    ca_.Bind(&block5, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block1);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<BuiltinPtr> tmp27;
    compiler::TNode<JSReceiver> tmp28;
    compiler::TNode<Map> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<JSArray> tmp31;
    ca_.Bind(&block6, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 100);
    compiler::TNode<IntPtrT> tmp32 = ca_.IntPtrConstant(JSArray::kLengthOffset);
    USE(tmp32);
    compiler::TNode<Number>tmp33 = CodeStubAssembler(state_).LoadReference<Number>(CodeStubAssembler::Reference{tmp31, tmp32});
    compiler::TNode<BoolT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).IsNumberNotEqual(compiler::TNode<Number>{tmp30}, compiler::TNode<Number>{tmp33}));
    ca_.Branch(tmp34, &block7, &block8, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<BuiltinPtr> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    compiler::TNode<Map> tmp38;
    compiler::TNode<Number> tmp39;
    compiler::TNode<JSArray> tmp40;
    ca_.Bind(&block7, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.Goto(&block1);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<BuiltinPtr> tmp42;
    compiler::TNode<JSReceiver> tmp43;
    compiler::TNode<Map> tmp44;
    compiler::TNode<Number> tmp45;
    compiler::TNode<JSArray> tmp46;
    ca_.Bind(&block8, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 101);
    compiler::TNode<BoolT> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNoElementsProtectorCellInvalid());
    ca_.Branch(tmp47, &block9, &block10, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<BuiltinPtr> tmp49;
    compiler::TNode<JSReceiver> tmp50;
    compiler::TNode<Map> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<JSArray> tmp53;
    ca_.Bind(&block9, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.Goto(&block1);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<BuiltinPtr> tmp55;
    compiler::TNode<JSReceiver> tmp56;
    compiler::TNode<Map> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<JSArray> tmp59;
    ca_.Bind(&block10, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 102);
    ca_.Goto(&block2);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 87);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 109);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 110);
    compiler::TNode<IntPtrT> tmp6 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp6);
    compiler::TNode<JSArrayBuffer>tmp7 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp5, tmp6});
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp7}));
    ca_.Branch(tmp8, &block3, &block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<BuiltinPtr> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<Map> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block3, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.Goto(&block1);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<BuiltinPtr> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Map> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<JSTypedArray> tmp20;
    ca_.Bind(&block4, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 111);
    ca_.Goto(&block2);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 87);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 120);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 121);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 122);
    ca_.Goto(&block1, tmp21, tmp22, tmp23, tmp24);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    ca_.Bind(&block3, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 125);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp25);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<IntPtrT> tmp32;
    ca_.Bind(&block1, &tmp29, &tmp30, &tmp31, &tmp32);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 117);
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
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String, FixedArray, IntPtrT, IntPtrT, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<UintPtrT, String, FixedArray, IntPtrT, IntPtrT, BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_len, p_sep);

  if (block0.is_used()) {
    compiler::TNode<UintPtrT> tmp0;
    compiler::TNode<String> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 217);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 219);
    compiler::TNode<IntPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).Signed(compiler::TNode<UintPtrT>{tmp6}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 217);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 222);
    compiler::TNode<FixedArray> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateZeroedFixedArray(compiler::TNode<IntPtrT>{tmp14}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 225);
    compiler::TNode<Int32T> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadInstanceType(compiler::TNode<HeapObject>{tmp13}));
    compiler::TNode<BoolT> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsOneByteStringInstanceType(compiler::TNode<Int32T>{tmp16}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 221);
    compiler::TNode<IntPtrT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<IntPtrT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.Goto(&block1, tmp12, tmp13, tmp15, tmp18, tmp19, tmp17);
  }

  if (block1.is_used()) {
    compiler::TNode<UintPtrT> tmp20;
    compiler::TNode<String> tmp21;
    compiler::TNode<FixedArray> tmp22;
    compiler::TNode<IntPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<BoolT> tmp25;
    ca_.Bind(&block1, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 216);
    ca_.Goto(&block6, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

    compiler::TNode<UintPtrT> tmp26;
    compiler::TNode<String> tmp27;
    compiler::TNode<FixedArray> tmp28;
    compiler::TNode<IntPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<BoolT> tmp31;
    ca_.Bind(&block6, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
  return ArrayJoinBuiltinsFromDSLAssembler::Buffer{compiler::TNode<FixedArray>{tmp28}, compiler::TNode<IntPtrT>{tmp29}, compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<BoolT>{tmp31}};
}

compiler::TNode<String> ArrayJoinBuiltinsFromDSLAssembler::BufferJoin(compiler::TNode<Context> p_context, ArrayJoinBuiltinsFromDSLAssembler::Buffer p_buffer, compiler::TNode<String> p_sep) {
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object, Object, String> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object, Object, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, FixedArray, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, Uint32T> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, Uint32T> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, Uint32T, String> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, Uint32T, String> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray, IntPtrT, IntPtrT, BoolT, String, String> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_buffer.fixedArray, p_buffer.index, p_buffer.totalStringLength, p_buffer.isOneByte, p_sep);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<FixedArray> tmp1;
    compiler::TNode<IntPtrT> tmp2;
    compiler::TNode<IntPtrT> tmp3;
    compiler::TNode<BoolT> tmp4;
    compiler::TNode<String> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 232);
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp6}));
    ca_.Branch(tmp7, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<FixedArray> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    compiler::TNode<BoolT> tmp12;
    compiler::TNode<String> tmp13;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    compiler::TNode<String> tmp14;
    USE(tmp14);
    tmp14 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.Goto(&block1, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<FixedArray> tmp16;
    compiler::TNode<IntPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    compiler::TNode<BoolT> tmp19;
    compiler::TNode<String> tmp20;
    ca_.Bind(&block3, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 235);
    compiler::TNode<IntPtrT> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp17}, compiler::TNode<IntPtrT>{tmp21}));
    ca_.Branch(tmp22, &block4, &block5, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<FixedArray> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<BoolT> tmp27;
    compiler::TNode<String> tmp28;
    ca_.Bind(&block4, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 236);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 237);
    compiler::TNode<Object> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp24}, 0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 240);
    compiler::TNode<String> tmp30;
    USE(tmp30);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp30 = BaseBuiltinsFromDSLAssembler(state_).Cast6String(compiler::TNode<Context>{tmp23}, compiler::TNode<Object>{tmp29}, &label0);
    ca_.Goto(&block8, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp24, tmp29, tmp29, tmp30);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp24, tmp29, tmp29);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<FixedArray> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    compiler::TNode<IntPtrT> tmp34;
    compiler::TNode<BoolT> tmp35;
    compiler::TNode<String> tmp36;
    compiler::TNode<FixedArray> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block9, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.Goto(&block7, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<FixedArray> tmp41;
    compiler::TNode<IntPtrT> tmp42;
    compiler::TNode<IntPtrT> tmp43;
    compiler::TNode<BoolT> tmp44;
    compiler::TNode<String> tmp45;
    compiler::TNode<FixedArray> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<String> tmp49;
    ca_.Bind(&block8, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 241);
    ca_.Goto(&block1, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp49);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<FixedArray> tmp51;
    compiler::TNode<IntPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<BoolT> tmp54;
    compiler::TNode<String> tmp55;
    compiler::TNode<FixedArray> tmp56;
    compiler::TNode<Object> tmp57;
    ca_.Bind(&block7, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 246);
    compiler::TNode<Number> tmp58;
    USE(tmp58);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp58 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp57)}, &label0);
    ca_.Goto(&block12, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, ca_.UncheckedCast<Object>(tmp57), tmp58);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block13, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, ca_.UncheckedCast<Object>(tmp57));
    }
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<FixedArray> tmp60;
    compiler::TNode<IntPtrT> tmp61;
    compiler::TNode<IntPtrT> tmp62;
    compiler::TNode<BoolT> tmp63;
    compiler::TNode<String> tmp64;
    compiler::TNode<FixedArray> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<Object> tmp67;
    ca_.Bind(&block13, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.Goto(&block11, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<FixedArray> tmp69;
    compiler::TNode<IntPtrT> tmp70;
    compiler::TNode<IntPtrT> tmp71;
    compiler::TNode<BoolT> tmp72;
    compiler::TNode<String> tmp73;
    compiler::TNode<FixedArray> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<Number> tmp77;
    ca_.Bind(&block12, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 247);
    compiler::TNode<String> tmp78;
    tmp78 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kStringRepeat, tmp68, tmp73, tmp77));
    USE(tmp78);
    ca_.Goto(&block1, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp78);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp79;
    compiler::TNode<FixedArray> tmp80;
    compiler::TNode<IntPtrT> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    compiler::TNode<BoolT> tmp83;
    compiler::TNode<String> tmp84;
    compiler::TNode<FixedArray> tmp85;
    compiler::TNode<Object> tmp86;
    ca_.Bind(&block11, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 249);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 250);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<FixedArray> tmp88;
    compiler::TNode<IntPtrT> tmp89;
    compiler::TNode<IntPtrT> tmp90;
    compiler::TNode<BoolT> tmp91;
    compiler::TNode<String> tmp92;
    ca_.Bind(&block5, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 255);
    compiler::TNode<UintPtrT> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).Unsigned(compiler::TNode<IntPtrT>{tmp90}));
    compiler::TNode<Uint32T> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATuint329ATuintptr(compiler::TNode<UintPtrT>{tmp93}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 256);
    ca_.Branch(tmp91, &block14, &block15, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp94);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp95;
    compiler::TNode<FixedArray> tmp96;
    compiler::TNode<IntPtrT> tmp97;
    compiler::TNode<IntPtrT> tmp98;
    compiler::TNode<BoolT> tmp99;
    compiler::TNode<String> tmp100;
    compiler::TNode<Uint32T> tmp101;
    ca_.Bind(&block14, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101);
    compiler::TNode<String> tmp102;
    USE(tmp102);
    tmp102 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).AllocateSeqOneByteString(compiler::TNode<Context>{tmp95}, compiler::TNode<Uint32T>{tmp101}));
    ca_.Goto(&block17, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp103;
    compiler::TNode<FixedArray> tmp104;
    compiler::TNode<IntPtrT> tmp105;
    compiler::TNode<IntPtrT> tmp106;
    compiler::TNode<BoolT> tmp107;
    compiler::TNode<String> tmp108;
    compiler::TNode<Uint32T> tmp109;
    ca_.Bind(&block15, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 257);
    compiler::TNode<String> tmp110;
    USE(tmp110);
    tmp110 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).AllocateSeqTwoByteString(compiler::TNode<Context>{tmp103}, compiler::TNode<Uint32T>{tmp109}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 256);
    ca_.Goto(&block16, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp111;
    compiler::TNode<FixedArray> tmp112;
    compiler::TNode<IntPtrT> tmp113;
    compiler::TNode<IntPtrT> tmp114;
    compiler::TNode<BoolT> tmp115;
    compiler::TNode<String> tmp116;
    compiler::TNode<Uint32T> tmp117;
    compiler::TNode<String> tmp118;
    ca_.Bind(&block17, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
    ca_.Goto(&block16, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp119;
    compiler::TNode<FixedArray> tmp120;
    compiler::TNode<IntPtrT> tmp121;
    compiler::TNode<IntPtrT> tmp122;
    compiler::TNode<BoolT> tmp123;
    compiler::TNode<String> tmp124;
    compiler::TNode<Uint32T> tmp125;
    compiler::TNode<String> tmp126;
    ca_.Bind(&block16, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 259);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 258);
    compiler::TNode<String> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<String>(ArrayBuiltinsAssembler(state_).CallJSArrayArrayJoinConcatToSequentialString(compiler::TNode<FixedArray>{tmp120}, compiler::TNode<IntPtrT>{tmp121}, compiler::TNode<String>{tmp124}, compiler::TNode<String>{tmp126}));
    ca_.Goto(&block1, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp127);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp128;
    compiler::TNode<FixedArray> tmp129;
    compiler::TNode<IntPtrT> tmp130;
    compiler::TNode<IntPtrT> tmp131;
    compiler::TNode<BoolT> tmp132;
    compiler::TNode<String> tmp133;
    compiler::TNode<String> tmp134;
    ca_.Bind(&block1, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 229);
    ca_.Goto(&block18, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134);
  }

    compiler::TNode<Context> tmp135;
    compiler::TNode<FixedArray> tmp136;
    compiler::TNode<IntPtrT> tmp137;
    compiler::TNode<IntPtrT> tmp138;
    compiler::TNode<BoolT> tmp139;
    compiler::TNode<String> tmp140;
    compiler::TNode<String> tmp141;
    ca_.Bind(&block18, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141);
  return compiler::TNode<String>{tmp141};
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 333);
    compiler::TNode<IntPtrT> tmp6 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp6);
    compiler::TNode<Map>tmp7 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp1, tmp6});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 334);
    compiler::TNode<Int32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadMapElementsKind(compiler::TNode<Map>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 335);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 338);
    compiler::TNode<JSArray> tmp9;
    USE(tmp9);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).Cast7JSArray(compiler::TNode<HeapObject>{tmp1}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp7, tmp8, ca_.Uninitialized<BuiltinPtr>(), tmp1, tmp9);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp7, tmp8, ca_.Uninitialized<BuiltinPtr>(), tmp1);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<String> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Map> tmp16;
    compiler::TNode<Int32T> tmp17;
    compiler::TNode<BuiltinPtr> tmp18;
    compiler::TNode<JSReceiver> tmp19;
    ca_.Bind(&block5, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block3, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    compiler::TNode<String> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Map> tmp26;
    compiler::TNode<Int32T> tmp27;
    compiler::TNode<BuiltinPtr> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<JSArray> tmp30;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 339);
    compiler::TNode<IntPtrT> tmp31 = ca_.IntPtrConstant(JSArray::kLengthOffset);
    USE(tmp31);
    compiler::TNode<Number>tmp32 = CodeStubAssembler(state_).LoadReference<Number>(CodeStubAssembler::Reference{tmp30, tmp31});
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).IsNumberNotEqual(compiler::TNode<Number>{tmp32}, compiler::TNode<Number>{tmp23}));
    ca_.Branch(tmp33, &block6, &block7, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp30);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<JSReceiver> tmp35;
    compiler::TNode<String> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Map> tmp40;
    compiler::TNode<Int32T> tmp41;
    compiler::TNode<BuiltinPtr> tmp42;
    compiler::TNode<JSArray> tmp43;
    ca_.Bind(&block6, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.Goto(&block3, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<String> tmp46;
    compiler::TNode<Number> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<Map> tmp50;
    compiler::TNode<Int32T> tmp51;
    compiler::TNode<BuiltinPtr> tmp52;
    compiler::TNode<JSArray> tmp53;
    ca_.Bind(&block7, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 340);
    compiler::TNode<BoolT> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsPrototypeInitialArrayPrototype(compiler::TNode<Context>{tmp44}, compiler::TNode<Map>{tmp50}));
    compiler::TNode<BoolT> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp54}));
    ca_.Branch(tmp55, &block8, &block9, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<String> tmp58;
    compiler::TNode<Number> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Map> tmp62;
    compiler::TNode<Int32T> tmp63;
    compiler::TNode<BuiltinPtr> tmp64;
    compiler::TNode<JSArray> tmp65;
    ca_.Bind(&block8, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65);
    ca_.Goto(&block3, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<JSReceiver> tmp67;
    compiler::TNode<String> tmp68;
    compiler::TNode<Number> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Map> tmp72;
    compiler::TNode<Int32T> tmp73;
    compiler::TNode<BuiltinPtr> tmp74;
    compiler::TNode<JSArray> tmp75;
    ca_.Bind(&block9, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 341);
    compiler::TNode<BoolT> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNoElementsProtectorCellInvalid());
    ca_.Branch(tmp76, &block10, &block11, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp77;
    compiler::TNode<JSReceiver> tmp78;
    compiler::TNode<String> tmp79;
    compiler::TNode<Number> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Map> tmp83;
    compiler::TNode<Int32T> tmp84;
    compiler::TNode<BuiltinPtr> tmp85;
    compiler::TNode<JSArray> tmp86;
    ca_.Bind(&block10, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86);
    ca_.Goto(&block3, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<JSReceiver> tmp88;
    compiler::TNode<String> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Map> tmp93;
    compiler::TNode<Int32T> tmp94;
    compiler::TNode<BuiltinPtr> tmp95;
    compiler::TNode<JSArray> tmp96;
    ca_.Bind(&block11, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 343);
    compiler::TNode<BoolT> tmp97;
    USE(tmp97);
    tmp97 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindLessThanOrEqual(compiler::TNode<Int32T>{tmp94}, HOLEY_ELEMENTS));
    ca_.Branch(tmp97, &block12, &block13, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp98;
    compiler::TNode<JSReceiver> tmp99;
    compiler::TNode<String> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Map> tmp104;
    compiler::TNode<Int32T> tmp105;
    compiler::TNode<BuiltinPtr> tmp106;
    compiler::TNode<JSArray> tmp107;
    ca_.Bind(&block12, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 344);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 343);
    ca_.Goto(&block14, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement25ATFastSmiOrObjectElements)), tmp107);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp108;
    compiler::TNode<JSReceiver> tmp109;
    compiler::TNode<String> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<Map> tmp114;
    compiler::TNode<Int32T> tmp115;
    compiler::TNode<BuiltinPtr> tmp116;
    compiler::TNode<JSArray> tmp117;
    ca_.Bind(&block13, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 345);
    compiler::TNode<BoolT> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindLessThanOrEqual(compiler::TNode<Int32T>{tmp115}, HOLEY_DOUBLE_ELEMENTS));
    ca_.Branch(tmp118, &block15, &block16, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp119;
    compiler::TNode<JSReceiver> tmp120;
    compiler::TNode<String> tmp121;
    compiler::TNode<Number> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Map> tmp125;
    compiler::TNode<Int32T> tmp126;
    compiler::TNode<BuiltinPtr> tmp127;
    compiler::TNode<JSArray> tmp128;
    ca_.Bind(&block15, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 346);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 345);
    ca_.Goto(&block17, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement20ATFastDoubleElements)), tmp128);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp129;
    compiler::TNode<JSReceiver> tmp130;
    compiler::TNode<String> tmp131;
    compiler::TNode<Number> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Map> tmp135;
    compiler::TNode<Int32T> tmp136;
    compiler::TNode<BuiltinPtr> tmp137;
    compiler::TNode<JSArray> tmp138;
    ca_.Bind(&block16, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 347);
    compiler::TNode<Int32T> tmp139;
    USE(tmp139);
    tmp139 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(DICTIONARY_ELEMENTS));
    compiler::TNode<BoolT> tmp140;
    USE(tmp140);
    tmp140 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp136}, compiler::TNode<Int32T>{tmp139}));
    ca_.Branch(tmp140, &block18, &block19, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp141;
    compiler::TNode<JSReceiver> tmp142;
    compiler::TNode<String> tmp143;
    compiler::TNode<Number> tmp144;
    compiler::TNode<Object> tmp145;
    compiler::TNode<Object> tmp146;
    compiler::TNode<Map> tmp147;
    compiler::TNode<Int32T> tmp148;
    compiler::TNode<BuiltinPtr> tmp149;
    compiler::TNode<JSArray> tmp150;
    ca_.Bind(&block18, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 350);
    compiler::TNode<IntPtrT> tmp151 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp151);
    compiler::TNode<FixedArrayBase>tmp152 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp150, tmp151});
    compiler::TNode<NumberDictionary> tmp153;
    USE(tmp153);
    tmp153 = ca_.UncheckedCast<NumberDictionary>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast18ATNumberDictionary(compiler::TNode<Context>{tmp141}, compiler::TNode<Object>{tmp152}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 349);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 351);
    compiler::TNode<Smi> tmp154;
    USE(tmp154);
    tmp154 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).GetNumberDictionaryNumberOfElements(compiler::TNode<NumberDictionary>{tmp153}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 352);
    compiler::TNode<Smi> tmp155;
    USE(tmp155);
    tmp155 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp156;
    USE(tmp156);
    tmp156 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp154}, compiler::TNode<Smi>{tmp155}));
    ca_.Branch(tmp156, &block21, &block22, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp153, tmp154);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp157;
    compiler::TNode<JSReceiver> tmp158;
    compiler::TNode<String> tmp159;
    compiler::TNode<Number> tmp160;
    compiler::TNode<Object> tmp161;
    compiler::TNode<Object> tmp162;
    compiler::TNode<Map> tmp163;
    compiler::TNode<Int32T> tmp164;
    compiler::TNode<BuiltinPtr> tmp165;
    compiler::TNode<JSArray> tmp166;
    compiler::TNode<NumberDictionary> tmp167;
    compiler::TNode<Smi> tmp168;
    ca_.Bind(&block21, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 353);
    compiler::TNode<String> tmp169;
    USE(tmp169);
    tmp169 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp170;
    USE(tmp170);
    tmp170 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp159}, compiler::TNode<HeapObject>{tmp169}));
    ca_.Branch(tmp170, &block24, &block25, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp171;
    compiler::TNode<JSReceiver> tmp172;
    compiler::TNode<String> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<Object> tmp176;
    compiler::TNode<Map> tmp177;
    compiler::TNode<Int32T> tmp178;
    compiler::TNode<BuiltinPtr> tmp179;
    compiler::TNode<JSArray> tmp180;
    compiler::TNode<NumberDictionary> tmp181;
    compiler::TNode<Smi> tmp182;
    ca_.Bind(&block24, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182);
    compiler::TNode<String> tmp183;
    USE(tmp183);
    tmp183 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.Goto(&block1, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp183);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp184;
    compiler::TNode<JSReceiver> tmp185;
    compiler::TNode<String> tmp186;
    compiler::TNode<Number> tmp187;
    compiler::TNode<Object> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<Map> tmp190;
    compiler::TNode<Int32T> tmp191;
    compiler::TNode<BuiltinPtr> tmp192;
    compiler::TNode<JSArray> tmp193;
    compiler::TNode<NumberDictionary> tmp194;
    compiler::TNode<Smi> tmp195;
    ca_.Bind(&block25, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 356);
    compiler::TNode<Number> tmp196;
    USE(tmp196);
    tmp196 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp197;
    USE(tmp197);
    tmp197 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp187}, compiler::TNode<Number>{tmp196}));
    compiler::TNode<Smi> tmp198;
    USE(tmp198);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp198 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp197}, &label0);
    ca_.Goto(&block28, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195, tmp197, tmp198);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block29, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195, tmp197);
    }
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp199;
    compiler::TNode<JSReceiver> tmp200;
    compiler::TNode<String> tmp201;
    compiler::TNode<Number> tmp202;
    compiler::TNode<Object> tmp203;
    compiler::TNode<Object> tmp204;
    compiler::TNode<Map> tmp205;
    compiler::TNode<Int32T> tmp206;
    compiler::TNode<BuiltinPtr> tmp207;
    compiler::TNode<JSArray> tmp208;
    compiler::TNode<NumberDictionary> tmp209;
    compiler::TNode<Smi> tmp210;
    compiler::TNode<Number> tmp211;
    ca_.Bind(&block29, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211);
    ca_.Goto(&block27, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp212;
    compiler::TNode<JSReceiver> tmp213;
    compiler::TNode<String> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<Object> tmp216;
    compiler::TNode<Object> tmp217;
    compiler::TNode<Map> tmp218;
    compiler::TNode<Int32T> tmp219;
    compiler::TNode<BuiltinPtr> tmp220;
    compiler::TNode<JSArray> tmp221;
    compiler::TNode<NumberDictionary> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<Number> tmp224;
    compiler::TNode<Smi> tmp225;
    ca_.Bind(&block28, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 355);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 357);
    compiler::TNode<String> tmp226;
    tmp226 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kStringRepeat, tmp212, tmp214, tmp225));
    USE(tmp226);
    ca_.Goto(&block1, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp226);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp227;
    compiler::TNode<JSReceiver> tmp228;
    compiler::TNode<String> tmp229;
    compiler::TNode<Number> tmp230;
    compiler::TNode<Object> tmp231;
    compiler::TNode<Object> tmp232;
    compiler::TNode<Map> tmp233;
    compiler::TNode<Int32T> tmp234;
    compiler::TNode<BuiltinPtr> tmp235;
    compiler::TNode<JSArray> tmp236;
    compiler::TNode<NumberDictionary> tmp237;
    compiler::TNode<Smi> tmp238;
    ca_.Bind(&block27, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 360);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp227);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp240;
    compiler::TNode<JSReceiver> tmp241;
    compiler::TNode<String> tmp242;
    compiler::TNode<Number> tmp243;
    compiler::TNode<Object> tmp244;
    compiler::TNode<Object> tmp245;
    compiler::TNode<Map> tmp246;
    compiler::TNode<Int32T> tmp247;
    compiler::TNode<BuiltinPtr> tmp248;
    compiler::TNode<JSArray> tmp249;
    compiler::TNode<NumberDictionary> tmp250;
    compiler::TNode<Smi> tmp251;
    ca_.Bind(&block22, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 363);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 352);
    ca_.Goto(&block23, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement20ATDictionaryElements)), tmp249, tmp250, tmp251);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp252;
    compiler::TNode<JSReceiver> tmp253;
    compiler::TNode<String> tmp254;
    compiler::TNode<Number> tmp255;
    compiler::TNode<Object> tmp256;
    compiler::TNode<Object> tmp257;
    compiler::TNode<Map> tmp258;
    compiler::TNode<Int32T> tmp259;
    compiler::TNode<BuiltinPtr> tmp260;
    compiler::TNode<JSArray> tmp261;
    compiler::TNode<NumberDictionary> tmp262;
    compiler::TNode<Smi> tmp263;
    ca_.Bind(&block23, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 348);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 347);
    ca_.Goto(&block20, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp264;
    compiler::TNode<JSReceiver> tmp265;
    compiler::TNode<String> tmp266;
    compiler::TNode<Number> tmp267;
    compiler::TNode<Object> tmp268;
    compiler::TNode<Object> tmp269;
    compiler::TNode<Map> tmp270;
    compiler::TNode<Int32T> tmp271;
    compiler::TNode<BuiltinPtr> tmp272;
    compiler::TNode<JSArray> tmp273;
    ca_.Bind(&block19, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 367);
    ca_.Goto(&block3, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp274;
    compiler::TNode<JSReceiver> tmp275;
    compiler::TNode<String> tmp276;
    compiler::TNode<Number> tmp277;
    compiler::TNode<Object> tmp278;
    compiler::TNode<Object> tmp279;
    compiler::TNode<Map> tmp280;
    compiler::TNode<Int32T> tmp281;
    compiler::TNode<BuiltinPtr> tmp282;
    compiler::TNode<JSArray> tmp283;
    ca_.Bind(&block20, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 345);
    ca_.Goto(&block17, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp284;
    compiler::TNode<JSReceiver> tmp285;
    compiler::TNode<String> tmp286;
    compiler::TNode<Number> tmp287;
    compiler::TNode<Object> tmp288;
    compiler::TNode<Object> tmp289;
    compiler::TNode<Map> tmp290;
    compiler::TNode<Int32T> tmp291;
    compiler::TNode<BuiltinPtr> tmp292;
    compiler::TNode<JSArray> tmp293;
    ca_.Bind(&block17, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 343);
    ca_.Goto(&block14, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp291, tmp292, tmp293);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp294;
    compiler::TNode<JSReceiver> tmp295;
    compiler::TNode<String> tmp296;
    compiler::TNode<Number> tmp297;
    compiler::TNode<Object> tmp298;
    compiler::TNode<Object> tmp299;
    compiler::TNode<Map> tmp300;
    compiler::TNode<Int32T> tmp301;
    compiler::TNode<BuiltinPtr> tmp302;
    compiler::TNode<JSArray> tmp303;
    ca_.Bind(&block14, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 337);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 370);
    ca_.Goto(&block2, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp304;
    compiler::TNode<JSReceiver> tmp305;
    compiler::TNode<String> tmp306;
    compiler::TNode<Number> tmp307;
    compiler::TNode<Object> tmp308;
    compiler::TNode<Object> tmp309;
    compiler::TNode<Map> tmp310;
    compiler::TNode<Int32T> tmp311;
    compiler::TNode<BuiltinPtr> tmp312;
    ca_.Bind(&block3, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 371);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 337);
    ca_.Goto(&block2, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement25ATGenericElementsAccessor)));
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp313;
    compiler::TNode<JSReceiver> tmp314;
    compiler::TNode<String> tmp315;
    compiler::TNode<Number> tmp316;
    compiler::TNode<Object> tmp317;
    compiler::TNode<Object> tmp318;
    compiler::TNode<Map> tmp319;
    compiler::TNode<Int32T> tmp320;
    compiler::TNode<BuiltinPtr> tmp321;
    ca_.Bind(&block2, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 374);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 373);
    compiler::TNode<String> tmp322;
    USE(tmp322);
    tmp322 = ca_.UncheckedCast<String>(ArrayJoinBuiltinsFromDSLAssembler(state_).ArrayJoinImpl7JSArray(compiler::TNode<Context>{tmp313}, compiler::TNode<JSReceiver>{tmp314}, compiler::TNode<String>{tmp315}, compiler::TNode<Number>{tmp316}, p_useToLocaleString, compiler::TNode<Object>{tmp317}, compiler::TNode<Object>{tmp318}, compiler::TNode<BuiltinPtr>{tmp321}));
    ca_.Goto(&block1, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp322);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp323;
    compiler::TNode<JSReceiver> tmp324;
    compiler::TNode<String> tmp325;
    compiler::TNode<Number> tmp326;
    compiler::TNode<Object> tmp327;
    compiler::TNode<Object> tmp328;
    compiler::TNode<Object> tmp329;
    ca_.Bind(&block1, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 326);
    ca_.Goto(&block30, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329);
  }

    compiler::TNode<Context> tmp330;
    compiler::TNode<JSReceiver> tmp331;
    compiler::TNode<String> tmp332;
    compiler::TNode<Number> tmp333;
    compiler::TNode<Object> tmp334;
    compiler::TNode<Object> tmp335;
    compiler::TNode<Object> tmp336;
    ca_.Bind(&block30, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336);
  return compiler::TNode<Object>{tmp336};
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 380);
    compiler::TNode<IntPtrT> tmp6 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp6);
    compiler::TNode<Map>tmp7 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp1, tmp6});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 381);
    compiler::TNode<Int32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadMapElementsKind(compiler::TNode<Map>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 382);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 384);
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsElementsKindGreaterThan(compiler::TNode<Int32T>{tmp8}, UINT32_ELEMENTS));
    ca_.Branch(tmp9, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp7, tmp8, ca_.Uninitialized<BuiltinPtr>());
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<String> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Map> tmp16;
    compiler::TNode<Int32T> tmp17;
    compiler::TNode<BuiltinPtr> tmp18;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 385);
    compiler::TNode<Int32T> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT32_ELEMENTS));
    compiler::TNode<BoolT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp17}, compiler::TNode<Int32T>{tmp19}));
    ca_.Branch(tmp20, &block5, &block6, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<String> tmp23;
    compiler::TNode<Number> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Map> tmp27;
    compiler::TNode<Int32T> tmp28;
    compiler::TNode<BuiltinPtr> tmp29;
    ca_.Bind(&block5, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 386);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 385);
    ca_.Goto(&block7, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement17ATFixedInt32Array)));
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<JSReceiver> tmp31;
    compiler::TNode<String> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Map> tmp36;
    compiler::TNode<Int32T> tmp37;
    compiler::TNode<BuiltinPtr> tmp38;
    ca_.Bind(&block6, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 387);
    compiler::TNode<Int32T> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT32_ELEMENTS));
    compiler::TNode<BoolT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp37}, compiler::TNode<Int32T>{tmp39}));
    ca_.Branch(tmp40, &block8, &block9, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<String> tmp43;
    compiler::TNode<Number> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Map> tmp47;
    compiler::TNode<Int32T> tmp48;
    compiler::TNode<BuiltinPtr> tmp49;
    ca_.Bind(&block8, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 388);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 387);
    ca_.Goto(&block10, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement19ATFixedFloat32Array)));
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<JSReceiver> tmp51;
    compiler::TNode<String> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Map> tmp56;
    compiler::TNode<Int32T> tmp57;
    compiler::TNode<BuiltinPtr> tmp58;
    ca_.Bind(&block9, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 389);
    compiler::TNode<Int32T> tmp59;
    USE(tmp59);
    tmp59 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(FLOAT64_ELEMENTS));
    compiler::TNode<BoolT> tmp60;
    USE(tmp60);
    tmp60 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp57}, compiler::TNode<Int32T>{tmp59}));
    ca_.Branch(tmp60, &block11, &block12, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<JSReceiver> tmp62;
    compiler::TNode<String> tmp63;
    compiler::TNode<Number> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<Map> tmp67;
    compiler::TNode<Int32T> tmp68;
    compiler::TNode<BuiltinPtr> tmp69;
    ca_.Bind(&block11, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 390);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 389);
    ca_.Goto(&block13, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement19ATFixedFloat64Array)));
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<JSReceiver> tmp71;
    compiler::TNode<String> tmp72;
    compiler::TNode<Number> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<Map> tmp76;
    compiler::TNode<Int32T> tmp77;
    compiler::TNode<BuiltinPtr> tmp78;
    ca_.Bind(&block12, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 391);
    compiler::TNode<Int32T> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_CLAMPED_ELEMENTS));
    compiler::TNode<BoolT> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp77}, compiler::TNode<Int32T>{tmp79}));
    ca_.Branch(tmp80, &block14, &block15, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp81;
    compiler::TNode<JSReceiver> tmp82;
    compiler::TNode<String> tmp83;
    compiler::TNode<Number> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<Map> tmp87;
    compiler::TNode<Int32T> tmp88;
    compiler::TNode<BuiltinPtr> tmp89;
    ca_.Bind(&block14, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 392);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 391);
    ca_.Goto(&block16, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement24ATFixedUint8ClampedArray)));
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<String> tmp92;
    compiler::TNode<Number> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Map> tmp96;
    compiler::TNode<Int32T> tmp97;
    compiler::TNode<BuiltinPtr> tmp98;
    ca_.Bind(&block15, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 393);
    compiler::TNode<Int32T> tmp99;
    USE(tmp99);
    tmp99 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGUINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp100;
    USE(tmp100);
    tmp100 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp97}, compiler::TNode<Int32T>{tmp99}));
    ca_.Branch(tmp100, &block17, &block18, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp101;
    compiler::TNode<JSReceiver> tmp102;
    compiler::TNode<String> tmp103;
    compiler::TNode<Number> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<Map> tmp107;
    compiler::TNode<Int32T> tmp108;
    compiler::TNode<BuiltinPtr> tmp109;
    ca_.Bind(&block17, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 394);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 393);
    ca_.Goto(&block19, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement21ATFixedBigUint64Array)));
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<JSReceiver> tmp111;
    compiler::TNode<String> tmp112;
    compiler::TNode<Number> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<Object> tmp115;
    compiler::TNode<Map> tmp116;
    compiler::TNode<Int32T> tmp117;
    compiler::TNode<BuiltinPtr> tmp118;
    ca_.Bind(&block18, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 395);
    compiler::TNode<Int32T> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(BIGINT64_ELEMENTS));
    compiler::TNode<BoolT> tmp120;
    USE(tmp120);
    tmp120 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp117}, compiler::TNode<Int32T>{tmp119}));
    ca_.Branch(tmp120, &block20, &block21, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp121;
    compiler::TNode<JSReceiver> tmp122;
    compiler::TNode<String> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<Object> tmp126;
    compiler::TNode<Map> tmp127;
    compiler::TNode<Int32T> tmp128;
    compiler::TNode<BuiltinPtr> tmp129;
    ca_.Bind(&block20, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 396);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 395);
    ca_.Goto(&block22, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement20ATFixedBigInt64Array)));
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp130;
    compiler::TNode<JSReceiver> tmp131;
    compiler::TNode<String> tmp132;
    compiler::TNode<Number> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<Map> tmp136;
    compiler::TNode<Int32T> tmp137;
    compiler::TNode<BuiltinPtr> tmp138;
    ca_.Bind(&block21, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 398);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp139;
    compiler::TNode<JSReceiver> tmp140;
    compiler::TNode<String> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<Map> tmp145;
    compiler::TNode<Int32T> tmp146;
    compiler::TNode<BuiltinPtr> tmp147;
    ca_.Bind(&block22, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 393);
    ca_.Goto(&block19, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp148;
    compiler::TNode<JSReceiver> tmp149;
    compiler::TNode<String> tmp150;
    compiler::TNode<Number> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<Map> tmp154;
    compiler::TNode<Int32T> tmp155;
    compiler::TNode<BuiltinPtr> tmp156;
    ca_.Bind(&block19, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 391);
    ca_.Goto(&block16, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp157;
    compiler::TNode<JSReceiver> tmp158;
    compiler::TNode<String> tmp159;
    compiler::TNode<Number> tmp160;
    compiler::TNode<Object> tmp161;
    compiler::TNode<Object> tmp162;
    compiler::TNode<Map> tmp163;
    compiler::TNode<Int32T> tmp164;
    compiler::TNode<BuiltinPtr> tmp165;
    ca_.Bind(&block16, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 389);
    ca_.Goto(&block13, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp166;
    compiler::TNode<JSReceiver> tmp167;
    compiler::TNode<String> tmp168;
    compiler::TNode<Number> tmp169;
    compiler::TNode<Object> tmp170;
    compiler::TNode<Object> tmp171;
    compiler::TNode<Map> tmp172;
    compiler::TNode<Int32T> tmp173;
    compiler::TNode<BuiltinPtr> tmp174;
    ca_.Bind(&block13, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 387);
    ca_.Goto(&block10, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp175;
    compiler::TNode<JSReceiver> tmp176;
    compiler::TNode<String> tmp177;
    compiler::TNode<Number> tmp178;
    compiler::TNode<Object> tmp179;
    compiler::TNode<Object> tmp180;
    compiler::TNode<Map> tmp181;
    compiler::TNode<Int32T> tmp182;
    compiler::TNode<BuiltinPtr> tmp183;
    ca_.Bind(&block10, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 385);
    ca_.Goto(&block7, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp184;
    compiler::TNode<JSReceiver> tmp185;
    compiler::TNode<String> tmp186;
    compiler::TNode<Number> tmp187;
    compiler::TNode<Object> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<Map> tmp190;
    compiler::TNode<Int32T> tmp191;
    compiler::TNode<BuiltinPtr> tmp192;
    ca_.Bind(&block7, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 384);
    ca_.Goto(&block4, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp193;
    compiler::TNode<JSReceiver> tmp194;
    compiler::TNode<String> tmp195;
    compiler::TNode<Number> tmp196;
    compiler::TNode<Object> tmp197;
    compiler::TNode<Object> tmp198;
    compiler::TNode<Map> tmp199;
    compiler::TNode<Int32T> tmp200;
    compiler::TNode<BuiltinPtr> tmp201;
    ca_.Bind(&block3, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 401);
    compiler::TNode<Int32T> tmp202;
    USE(tmp202);
    tmp202 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT8_ELEMENTS));
    compiler::TNode<BoolT> tmp203;
    USE(tmp203);
    tmp203 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp200}, compiler::TNode<Int32T>{tmp202}));
    ca_.Branch(tmp203, &block23, &block24, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp204;
    compiler::TNode<JSReceiver> tmp205;
    compiler::TNode<String> tmp206;
    compiler::TNode<Number> tmp207;
    compiler::TNode<Object> tmp208;
    compiler::TNode<Object> tmp209;
    compiler::TNode<Map> tmp210;
    compiler::TNode<Int32T> tmp211;
    compiler::TNode<BuiltinPtr> tmp212;
    ca_.Bind(&block23, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 402);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 401);
    ca_.Goto(&block25, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement17ATFixedUint8Array)));
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp213;
    compiler::TNode<JSReceiver> tmp214;
    compiler::TNode<String> tmp215;
    compiler::TNode<Number> tmp216;
    compiler::TNode<Object> tmp217;
    compiler::TNode<Object> tmp218;
    compiler::TNode<Map> tmp219;
    compiler::TNode<Int32T> tmp220;
    compiler::TNode<BuiltinPtr> tmp221;
    ca_.Bind(&block24, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 403);
    compiler::TNode<Int32T> tmp222;
    USE(tmp222);
    tmp222 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT8_ELEMENTS));
    compiler::TNode<BoolT> tmp223;
    USE(tmp223);
    tmp223 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp220}, compiler::TNode<Int32T>{tmp222}));
    ca_.Branch(tmp223, &block26, &block27, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp224;
    compiler::TNode<JSReceiver> tmp225;
    compiler::TNode<String> tmp226;
    compiler::TNode<Number> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Object> tmp229;
    compiler::TNode<Map> tmp230;
    compiler::TNode<Int32T> tmp231;
    compiler::TNode<BuiltinPtr> tmp232;
    ca_.Bind(&block26, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 404);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 403);
    ca_.Goto(&block28, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement16ATFixedInt8Array)));
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp233;
    compiler::TNode<JSReceiver> tmp234;
    compiler::TNode<String> tmp235;
    compiler::TNode<Number> tmp236;
    compiler::TNode<Object> tmp237;
    compiler::TNode<Object> tmp238;
    compiler::TNode<Map> tmp239;
    compiler::TNode<Int32T> tmp240;
    compiler::TNode<BuiltinPtr> tmp241;
    ca_.Bind(&block27, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 405);
    compiler::TNode<Int32T> tmp242;
    USE(tmp242);
    tmp242 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT16_ELEMENTS));
    compiler::TNode<BoolT> tmp243;
    USE(tmp243);
    tmp243 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp240}, compiler::TNode<Int32T>{tmp242}));
    ca_.Branch(tmp243, &block29, &block30, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp244;
    compiler::TNode<JSReceiver> tmp245;
    compiler::TNode<String> tmp246;
    compiler::TNode<Number> tmp247;
    compiler::TNode<Object> tmp248;
    compiler::TNode<Object> tmp249;
    compiler::TNode<Map> tmp250;
    compiler::TNode<Int32T> tmp251;
    compiler::TNode<BuiltinPtr> tmp252;
    ca_.Bind(&block29, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 406);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 405);
    ca_.Goto(&block31, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement18ATFixedUint16Array)));
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp253;
    compiler::TNode<JSReceiver> tmp254;
    compiler::TNode<String> tmp255;
    compiler::TNode<Number> tmp256;
    compiler::TNode<Object> tmp257;
    compiler::TNode<Object> tmp258;
    compiler::TNode<Map> tmp259;
    compiler::TNode<Int32T> tmp260;
    compiler::TNode<BuiltinPtr> tmp261;
    ca_.Bind(&block30, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 407);
    compiler::TNode<Int32T> tmp262;
    USE(tmp262);
    tmp262 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(INT16_ELEMENTS));
    compiler::TNode<BoolT> tmp263;
    USE(tmp263);
    tmp263 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp260}, compiler::TNode<Int32T>{tmp262}));
    ca_.Branch(tmp263, &block32, &block33, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp264;
    compiler::TNode<JSReceiver> tmp265;
    compiler::TNode<String> tmp266;
    compiler::TNode<Number> tmp267;
    compiler::TNode<Object> tmp268;
    compiler::TNode<Object> tmp269;
    compiler::TNode<Map> tmp270;
    compiler::TNode<Int32T> tmp271;
    compiler::TNode<BuiltinPtr> tmp272;
    ca_.Bind(&block32, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 408);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 407);
    ca_.Goto(&block34, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement17ATFixedInt16Array)));
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp273;
    compiler::TNode<JSReceiver> tmp274;
    compiler::TNode<String> tmp275;
    compiler::TNode<Number> tmp276;
    compiler::TNode<Object> tmp277;
    compiler::TNode<Object> tmp278;
    compiler::TNode<Map> tmp279;
    compiler::TNode<Int32T> tmp280;
    compiler::TNode<BuiltinPtr> tmp281;
    ca_.Bind(&block33, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 409);
    compiler::TNode<Int32T> tmp282;
    USE(tmp282);
    tmp282 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATElementsKind24ATconstexpr_ElementsKind(UINT32_ELEMENTS));
    compiler::TNode<BoolT> tmp283;
    USE(tmp283);
    tmp283 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).ElementsKindEqual(compiler::TNode<Int32T>{tmp280}, compiler::TNode<Int32T>{tmp282}));
    ca_.Branch(tmp283, &block35, &block36, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp284;
    compiler::TNode<JSReceiver> tmp285;
    compiler::TNode<String> tmp286;
    compiler::TNode<Number> tmp287;
    compiler::TNode<Object> tmp288;
    compiler::TNode<Object> tmp289;
    compiler::TNode<Map> tmp290;
    compiler::TNode<Int32T> tmp291;
    compiler::TNode<BuiltinPtr> tmp292;
    ca_.Bind(&block35, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 410);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 409);
    ca_.Goto(&block37, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp291, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinTypedElement18ATFixedUint32Array)));
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp293;
    compiler::TNode<JSReceiver> tmp294;
    compiler::TNode<String> tmp295;
    compiler::TNode<Number> tmp296;
    compiler::TNode<Object> tmp297;
    compiler::TNode<Object> tmp298;
    compiler::TNode<Map> tmp299;
    compiler::TNode<Int32T> tmp300;
    compiler::TNode<BuiltinPtr> tmp301;
    ca_.Bind(&block36, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 412);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp302;
    compiler::TNode<JSReceiver> tmp303;
    compiler::TNode<String> tmp304;
    compiler::TNode<Number> tmp305;
    compiler::TNode<Object> tmp306;
    compiler::TNode<Object> tmp307;
    compiler::TNode<Map> tmp308;
    compiler::TNode<Int32T> tmp309;
    compiler::TNode<BuiltinPtr> tmp310;
    ca_.Bind(&block37, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 407);
    ca_.Goto(&block34, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp311;
    compiler::TNode<JSReceiver> tmp312;
    compiler::TNode<String> tmp313;
    compiler::TNode<Number> tmp314;
    compiler::TNode<Object> tmp315;
    compiler::TNode<Object> tmp316;
    compiler::TNode<Map> tmp317;
    compiler::TNode<Int32T> tmp318;
    compiler::TNode<BuiltinPtr> tmp319;
    ca_.Bind(&block34, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 405);
    ca_.Goto(&block31, tmp311, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp319);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp320;
    compiler::TNode<JSReceiver> tmp321;
    compiler::TNode<String> tmp322;
    compiler::TNode<Number> tmp323;
    compiler::TNode<Object> tmp324;
    compiler::TNode<Object> tmp325;
    compiler::TNode<Map> tmp326;
    compiler::TNode<Int32T> tmp327;
    compiler::TNode<BuiltinPtr> tmp328;
    ca_.Bind(&block31, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 403);
    ca_.Goto(&block28, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp329;
    compiler::TNode<JSReceiver> tmp330;
    compiler::TNode<String> tmp331;
    compiler::TNode<Number> tmp332;
    compiler::TNode<Object> tmp333;
    compiler::TNode<Object> tmp334;
    compiler::TNode<Map> tmp335;
    compiler::TNode<Int32T> tmp336;
    compiler::TNode<BuiltinPtr> tmp337;
    ca_.Bind(&block28, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336, &tmp337);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 401);
    ca_.Goto(&block25, tmp329, tmp330, tmp331, tmp332, tmp333, tmp334, tmp335, tmp336, tmp337);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp338;
    compiler::TNode<JSReceiver> tmp339;
    compiler::TNode<String> tmp340;
    compiler::TNode<Number> tmp341;
    compiler::TNode<Object> tmp342;
    compiler::TNode<Object> tmp343;
    compiler::TNode<Map> tmp344;
    compiler::TNode<Int32T> tmp345;
    compiler::TNode<BuiltinPtr> tmp346;
    ca_.Bind(&block25, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 384);
    ca_.Goto(&block4, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp347;
    compiler::TNode<JSReceiver> tmp348;
    compiler::TNode<String> tmp349;
    compiler::TNode<Number> tmp350;
    compiler::TNode<Object> tmp351;
    compiler::TNode<Object> tmp352;
    compiler::TNode<Map> tmp353;
    compiler::TNode<Int32T> tmp354;
    compiler::TNode<BuiltinPtr> tmp355;
    ca_.Bind(&block4, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 416);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 415);
    compiler::TNode<String> tmp356;
    USE(tmp356);
    tmp356 = ca_.UncheckedCast<String>(ArrayJoinBuiltinsFromDSLAssembler(state_).ArrayJoinImpl12JSTypedArray(compiler::TNode<Context>{tmp347}, compiler::TNode<JSReceiver>{tmp348}, compiler::TNode<String>{tmp349}, compiler::TNode<Number>{tmp350}, p_useToLocaleString, compiler::TNode<Object>{tmp351}, compiler::TNode<Object>{tmp352}, compiler::TNode<BuiltinPtr>{tmp355}));
    ca_.Goto(&block1, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp356);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp357;
    compiler::TNode<JSReceiver> tmp358;
    compiler::TNode<String> tmp359;
    compiler::TNode<Number> tmp360;
    compiler::TNode<Object> tmp361;
    compiler::TNode<Object> tmp362;
    compiler::TNode<Object> tmp363;
    ca_.Bind(&block1, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 326);
    ca_.Goto(&block38, tmp357, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363);
  }

    compiler::TNode<Context> tmp364;
    compiler::TNode<JSReceiver> tmp365;
    compiler::TNode<String> tmp366;
    compiler::TNode<Number> tmp367;
    compiler::TNode<Object> tmp368;
    compiler::TNode<Object> tmp369;
    compiler::TNode<Object> tmp370;
    ca_.Bind(&block38, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370);
  return compiler::TNode<Object>{tmp370};
}

compiler::TNode<FixedArray> ArrayJoinBuiltinsFromDSLAssembler::LoadJoinStack(compiler::TNode<Context> p_context, compiler::CodeAssemblerLabel* label_IfUninitialized) {
  compiler::CodeAssemblerParameterizedLabel<Context> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Context, HeapObject> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Context, HeapObject> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 428);
    compiler::TNode<Context> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<Context>(CodeStubAssembler(state_).LoadNativeContext(compiler::TNode<Context>{tmp0}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 430);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr19ATNativeContextSlot29ATconstexpr_NativeContextSlot(Context::ARRAY_JOIN_STACK_INDEX));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadContextElement(compiler::TNode<Context>{tmp1}, compiler::TNode<IntPtrT>{tmp2}));
    compiler::TNode<HeapObject> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<HeapObject>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10HeapObject(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp3}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 429);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 431);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 433);
    compiler::TNode<FixedArray> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp10}, compiler::TNode<Object>{tmp12}));
    ca_.Goto(&block2, tmp10, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<FixedArray> tmp15;
    ca_.Bind(&block2, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 426);
    ca_.Goto(&block5, tmp14, tmp15);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_IfUninitialized);
  }

    compiler::TNode<Context> tmp16;
    compiler::TNode<FixedArray> tmp17;
    ca_.Bind(&block5, &tmp16, &tmp17);
  return compiler::TNode<FixedArray>{tmp17};
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 437);
    compiler::TNode<Context> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Context>(CodeStubAssembler(state_).LoadNativeContext(compiler::TNode<Context>{tmp0}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 438);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr19ATNativeContextSlot29ATconstexpr_NativeContextSlot(Context::ARRAY_JOIN_STACK_INDEX));
    CodeStubAssembler(state_).StoreContextElement(compiler::TNode<Context>{tmp2}, compiler::TNode<IntPtrT>{tmp3}, compiler::TNode<Object>{tmp1});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 436);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 446);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 447);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 448);
    compiler::TNode<Object> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp12}, compiler::TNode<IntPtrT>{tmp15}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 451);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 452);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp20}, compiler::TNode<IntPtrT>{tmp23}, compiler::TNode<HeapObject>{tmp21});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 453);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 457);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 447);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 462);
    compiler::TNode<FixedArray> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray10JSReceiver(compiler::TNode<FixedArray>{tmp54}, compiler::TNode<IntPtrT>{tmp56}, compiler::TNode<JSReceiver>{tmp55}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 461);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 463);
    ArrayJoinBuiltinsFromDSLAssembler(state_).SetJoinStack(compiler::TNode<Context>{tmp53}, compiler::TNode<FixedArray>{tmp58});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 464);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 474);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 476);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 477);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp13}, 0, compiler::TNode<HeapObject>{tmp12});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 476);
    ca_.Goto(&block9, tmp11, tmp12, tmp13);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<FixedArray> tmp16;
    ca_.Bind(&block8, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 478);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 480);
    ca_.Goto(&block2);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<FixedArray> tmp25;
    ca_.Bind(&block11, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 476);
    ca_.Goto(&block9, tmp23, tmp24, tmp25);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<JSReceiver> tmp27;
    compiler::TNode<FixedArray> tmp28;
    ca_.Bind(&block9, &tmp26, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 473);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 483);
    ca_.Goto(&block3, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<JSReceiver> tmp30;
    ca_.Bind(&block4, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 485);
    compiler::TNode<IntPtrT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(JSArray::kMinJoinStackSize));
    compiler::TNode<FixedArray> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateFixedArrayWithHoles(compiler::TNode<IntPtrT>{tmp31}, CodeStubAssembler::kNone));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 484);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 486);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp32}, 0, compiler::TNode<HeapObject>{tmp30});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 487);
    ArrayJoinBuiltinsFromDSLAssembler(state_).SetJoinStack(compiler::TNode<Context>{tmp29}, compiler::TNode<FixedArray>{tmp32});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 483);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 473);
    ca_.Goto(&block3, tmp29, tmp30);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    ca_.Bind(&block3, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 489);
    ca_.Goto(&block1);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 470);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 496);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 497);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 498);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 501);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 503);
    compiler::TNode<IntPtrT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(JSArray::kMinJoinStackSize));
    compiler::TNode<FixedArray> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).AllocateFixedArrayWithHoles(compiler::TNode<IntPtrT>{tmp37}, CodeStubAssembler::kNone));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 502);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 504);
    ArrayJoinBuiltinsFromDSLAssembler(state_).SetJoinStack(compiler::TNode<Context>{tmp32}, compiler::TNode<FixedArray>{tmp38});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 501);
    ca_.Goto(&block10, tmp32, tmp33, tmp34, tmp35, tmp36);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<FixedArray> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    compiler::TNode<IntPtrT> tmp42;
    compiler::TNode<IntPtrT> tmp43;
    ca_.Bind(&block8, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 507);
    compiler::TNode<Oddball> tmp44;
    USE(tmp44);
    tmp44 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp40}, compiler::TNode<IntPtrT>{tmp43}, compiler::TNode<HeapObject>{tmp44});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 501);
    ca_.Goto(&block10, tmp39, tmp40, tmp41, tmp42, tmp43);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<FixedArray> tmp46;
    compiler::TNode<JSReceiver> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    ca_.Bind(&block10, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 509);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 497);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 512);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 517);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 518);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<FixedArray> tmp12;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 517);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 519);
    compiler::TNode<IntPtrT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp12}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 523);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 524);
    compiler::TNode<Oddball> tmp26;
    USE(tmp26);
    tmp26 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<IntPtrT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp24}, compiler::TNode<IntPtrT>{tmp27}, compiler::TNode<Object>{tmp26}, SKIP_WRITE_BARRIER);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 523);
    ca_.Goto(&block9, tmp22, tmp23, tmp24, tmp25);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<FixedArray> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    ca_.Bind(&block7, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 527);
    compiler::TNode<Object> tmp32;
    tmp32 = CodeStubAssembler(state_).CallBuiltin(Builtins::kJoinStackPop, tmp28, tmp30, tmp29);
    USE(tmp32);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 523);
    ca_.Goto(&block9, tmp28, tmp29, tmp30, tmp31);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    compiler::TNode<FixedArray> tmp35;
    compiler::TNode<IntPtrT> tmp36;
    ca_.Bind(&block9, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 516);
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
  TNode<IntPtrT> arguments_length(ChangeInt32ToIntPtr(argc));
  TNode<RawPtrT> arguments_frame = UncheckedCast<RawPtrT>(LoadFramePointer());
  BaseBuiltinsFromDSLAssembler::Arguments torque_arguments(GetFrameArguments(arguments_frame, arguments_length));
  CodeStubArguments arguments(this, torque_arguments);
  TNode<Object> parameter1 = arguments.GetReceiver();
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 561);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 564);
    compiler::TNode<JSReceiver> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 567);
    compiler::TNode<Number> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 571);
    compiler::TNode<Number> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber18ATconstexpr_uint32((CodeStubAssembler(state_).ConstexprUint32Add(JSArray::kMaxArrayIndex, 1))));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp8}, compiler::TNode<Number>{tmp9}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, tmp7, tmp8, tmp8);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, tmp7, tmp8, tmp8);
    }
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<RawPtrT> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<IntPtrT> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<Number> tmp18;
    ca_.Bind(&block3, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.Goto(&block1, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<RawPtrT> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<IntPtrT> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Number> tmp27;
    ca_.Bind(&block4, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block2, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<RawPtrT> tmp30;
    compiler::TNode<RawPtrT> tmp31;
    compiler::TNode<IntPtrT> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    compiler::TNode<Number> tmp35;
    ca_.Bind(&block1, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp28}, MessageTemplate::kInvalidArrayLength);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<RawPtrT> tmp38;
    compiler::TNode<RawPtrT> tmp39;
    compiler::TNode<IntPtrT> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<Number> tmp43;
    ca_.Bind(&block2, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 574);
    compiler::TNode<Oddball> tmp44;
    USE(tmp44);
    tmp44 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Oddball> tmp45;
    USE(tmp45);
    tmp45 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 573);
    compiler::TNode<Object> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).CycleProtectedArrayJoin7JSArray(compiler::TNode<Context>{tmp36}, false, compiler::TNode<JSReceiver>{tmp42}, compiler::TNode<Number>{tmp43}, compiler::TNode<Object>{tmp41}, compiler::TNode<Object>{tmp44}, compiler::TNode<Object>{tmp45}));
    arguments.PopAndReturn(tmp46);
  }
}

TF_BUILTIN(ArrayPrototypeToLocaleString, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSReceiver, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSReceiver, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSReceiver, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSReceiver, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 580);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 581);
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 584);
    compiler::TNode<JSReceiver> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 587);
    compiler::TNode<Number> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp9}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 591);
    compiler::TNode<Number> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber18ATconstexpr_uint32((CodeStubAssembler(state_).ConstexprUint32Add(JSArray::kMaxArrayIndex, 1))));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp10}, compiler::TNode<Number>{tmp11}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, tmp8, tmp9, tmp10, tmp10);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, tmp8, tmp9, tmp10, tmp10);
    }
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<RawPtrT> tmp15;
    compiler::TNode<IntPtrT> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<JSReceiver> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Number> tmp21;
    ca_.Bind(&block3, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.Goto(&block1, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<RawPtrT> tmp24;
    compiler::TNode<RawPtrT> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Number> tmp31;
    ca_.Bind(&block4, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block2, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<RawPtrT> tmp34;
    compiler::TNode<RawPtrT> tmp35;
    compiler::TNode<IntPtrT> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Number> tmp40;
    ca_.Bind(&block1, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp32}, MessageTemplate::kInvalidArrayLength);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<RawPtrT> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<IntPtrT> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    compiler::TNode<Number> tmp49;
    ca_.Bind(&block2, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 594);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 593);
    compiler::TNode<Object> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapObject18ATconstexpr_string(","));
    compiler::TNode<Object> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).CycleProtectedArrayJoin7JSArray(compiler::TNode<Context>{tmp41}, true, compiler::TNode<JSReceiver>{tmp48}, compiler::TNode<Number>{tmp49}, compiler::TNode<Object>{tmp50}, compiler::TNode<Object>{tmp46}, compiler::TNode<Object>{tmp47}));
    arguments.PopAndReturn(tmp51);
  }
}

TF_BUILTIN(ArrayPrototypeToString, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Object, Object, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 601);
    compiler::TNode<JSReceiver> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 604);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapObject18ATconstexpr_string("join"));
    compiler::TNode<Object> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}, compiler::TNode<Object>{tmp6}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 608);
    compiler::TNode<JSReceiver> tmp8;
    USE(tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp8 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp7}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp7, tmp7, tmp8);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp7, tmp7);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<RawPtrT> tmp11;
    compiler::TNode<RawPtrT> tmp12;
    compiler::TNode<IntPtrT> tmp13;
    compiler::TNode<JSReceiver> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Object> tmp16;
    ca_.Bind(&block4, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.Goto(&block2, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<RawPtrT> tmp19;
    compiler::TNode<RawPtrT> tmp20;
    compiler::TNode<IntPtrT> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    ca_.Bind(&block3, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 611);
    compiler::TNode<Object> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp17}, compiler::TNode<JSReceiver>{tmp25}, compiler::TNode<Object>{tmp22}));
    arguments.PopAndReturn(tmp26);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<RawPtrT> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Object> tmp33;
    ca_.Bind(&block2, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 614);
    compiler::TNode<Object> tmp34;
    tmp34 = CodeStubAssembler(state_).CallBuiltin(Builtins::kObjectToString, tmp27, tmp32);
    USE(tmp34);
    arguments.PopAndReturn(tmp34);
  }
}

TF_BUILTIN(TypedArrayPrototypeJoin, CodeStubAssembler) {
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
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 621);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 626);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 625);
    compiler::TNode<JSTypedArray> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<JSTypedArray>(TypedArrayBuiltinsAssembler(state_).ValidateTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, "%TypedArray%.prototype.join"));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 627);
    compiler::TNode<IntPtrT> tmp8 = ca_.IntPtrConstant(JSTypedArray::kLengthOffset);
    USE(tmp8);
    compiler::TNode<Smi>tmp9 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp7, tmp8});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 630);
    compiler::TNode<Oddball> tmp10;
    USE(tmp10);
    tmp10 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Oddball> tmp11;
    USE(tmp11);
    tmp11 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 629);
    compiler::TNode<Object> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).CycleProtectedArrayJoin12JSTypedArray(compiler::TNode<Context>{tmp0}, false, compiler::TNode<JSReceiver>{tmp7}, compiler::TNode<Number>{tmp9}, compiler::TNode<Object>{tmp6}, compiler::TNode<Object>{tmp10}, compiler::TNode<Object>{tmp11}));
    arguments.PopAndReturn(tmp12);
  }
}

TF_BUILTIN(TypedArrayPrototypeToLocaleString, CodeStubAssembler) {
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
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 636);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 637);
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 642);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 641);
    compiler::TNode<JSTypedArray> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<JSTypedArray>(TypedArrayBuiltinsAssembler(state_).ValidateTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, "%TypedArray%.prototype.toLocaleString"));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 643);
    compiler::TNode<IntPtrT> tmp10 = ca_.IntPtrConstant(JSTypedArray::kLengthOffset);
    USE(tmp10);
    compiler::TNode<Smi>tmp11 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp9, tmp10});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 646);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 645);
    compiler::TNode<Object> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapObject18ATconstexpr_string(","));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(ArrayJoinBuiltinsFromDSLAssembler(state_).CycleProtectedArrayJoin12JSTypedArray(compiler::TNode<Context>{tmp0}, true, compiler::TNode<JSReceiver>{tmp9}, compiler::TNode<Number>{tmp11}, compiler::TNode<Object>{tmp12}, compiler::TNode<Object>{tmp6}, compiler::TNode<Object>{tmp8}));
    arguments.PopAndReturn(tmp13);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 16);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 266);
    compiler::TNode<IntPtrT> tmp7 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp7);
    compiler::TNode<Map>tmp8 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp1, tmp7});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 267);
    compiler::TNode<UintPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr20UT5ATSmi10HeapNumber(compiler::TNode<Number>{tmp3}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 268);
    compiler::TNode<IntPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadStringLengthAsWord(compiler::TNode<String>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 269);
    compiler::TNode<IntPtrT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 270);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 271);
    compiler::TNode<FixedArray> tmp12;
    USE(tmp12);
    compiler::TNode<IntPtrT> tmp13;
    USE(tmp13);
    compiler::TNode<IntPtrT> tmp14;
    USE(tmp14);
    compiler::TNode<BoolT> tmp15;
    USE(tmp15);
    std::tie(tmp12, tmp13, tmp14, tmp15) = ArrayJoinBuiltinsFromDSLAssembler(state_).NewBuffer(compiler::TNode<UintPtrT>{tmp9}, compiler::TNode<String>{tmp2}).Flatten();
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 274);
    compiler::TNode<UintPtrT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 277);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp8, tmp9, tmp10, tmp11, tmp6, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSReceiver> tmp18;
    compiler::TNode<String> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<BuiltinPtr> tmp23;
    compiler::TNode<Map> tmp24;
    compiler::TNode<UintPtrT> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    compiler::TNode<BuiltinPtr> tmp28;
    compiler::TNode<FixedArray> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<UintPtrT> tmp33;
    ca_.Bind(&block4, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    compiler::TNode<BoolT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrLessThan(compiler::TNode<UintPtrT>{tmp33}, compiler::TNode<UintPtrT>{tmp25}));
    ca_.Branch(tmp34, &block2, &block3, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<JSReceiver> tmp36;
    compiler::TNode<String> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<BuiltinPtr> tmp41;
    compiler::TNode<Map> tmp42;
    compiler::TNode<UintPtrT> tmp43;
    compiler::TNode<IntPtrT> tmp44;
    compiler::TNode<IntPtrT> tmp45;
    compiler::TNode<BuiltinPtr> tmp46;
    compiler::TNode<FixedArray> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    compiler::TNode<BoolT> tmp50;
    compiler::TNode<UintPtrT> tmp51;
    ca_.Bind(&block2, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 279);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 278);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    ArrayJoinBuiltinsFromDSLAssembler(state_).CannotUseSameArrayAccessor7JSArray(compiler::TNode<Context>{tmp35}, compiler::TNode<BuiltinPtr>{tmp46}, compiler::TNode<JSReceiver>{tmp36}, compiler::TNode<Map>{tmp42}, compiler::TNode<Number>{tmp38}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp46, tmp36, tmp42, tmp38);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block8, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp46, tmp36, tmp42, tmp38);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    compiler::TNode<String> tmp54;
    compiler::TNode<Number> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<BuiltinPtr> tmp58;
    compiler::TNode<Map> tmp59;
    compiler::TNode<UintPtrT> tmp60;
    compiler::TNode<IntPtrT> tmp61;
    compiler::TNode<IntPtrT> tmp62;
    compiler::TNode<BuiltinPtr> tmp63;
    compiler::TNode<FixedArray> tmp64;
    compiler::TNode<IntPtrT> tmp65;
    compiler::TNode<IntPtrT> tmp66;
    compiler::TNode<BoolT> tmp67;
    compiler::TNode<UintPtrT> tmp68;
    compiler::TNode<BuiltinPtr> tmp69;
    compiler::TNode<JSReceiver> tmp70;
    compiler::TNode<Map> tmp71;
    compiler::TNode<Number> tmp72;
    ca_.Bind(&block7, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.Goto(&block5, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    compiler::TNode<String> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<BuiltinPtr> tmp79;
    compiler::TNode<Map> tmp80;
    compiler::TNode<UintPtrT> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    compiler::TNode<IntPtrT> tmp83;
    compiler::TNode<BuiltinPtr> tmp84;
    compiler::TNode<FixedArray> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<IntPtrT> tmp87;
    compiler::TNode<BoolT> tmp88;
    compiler::TNode<UintPtrT> tmp89;
    compiler::TNode<BuiltinPtr> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<Map> tmp92;
    compiler::TNode<Number> tmp93;
    ca_.Bind(&block8, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93);
    ca_.Goto(&block6, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp94;
    compiler::TNode<JSReceiver> tmp95;
    compiler::TNode<String> tmp96;
    compiler::TNode<Number> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<Object> tmp99;
    compiler::TNode<BuiltinPtr> tmp100;
    compiler::TNode<Map> tmp101;
    compiler::TNode<UintPtrT> tmp102;
    compiler::TNode<IntPtrT> tmp103;
    compiler::TNode<IntPtrT> tmp104;
    compiler::TNode<BuiltinPtr> tmp105;
    compiler::TNode<FixedArray> tmp106;
    compiler::TNode<IntPtrT> tmp107;
    compiler::TNode<IntPtrT> tmp108;
    compiler::TNode<BoolT> tmp109;
    compiler::TNode<UintPtrT> tmp110;
    ca_.Bind(&block5, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 281);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 278);
    ca_.Goto(&block6, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement25ATGenericElementsAccessor)), tmp106, tmp107, tmp108, tmp109, tmp110);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp111;
    compiler::TNode<JSReceiver> tmp112;
    compiler::TNode<String> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<Object> tmp115;
    compiler::TNode<Object> tmp116;
    compiler::TNode<BuiltinPtr> tmp117;
    compiler::TNode<Map> tmp118;
    compiler::TNode<UintPtrT> tmp119;
    compiler::TNode<IntPtrT> tmp120;
    compiler::TNode<IntPtrT> tmp121;
    compiler::TNode<BuiltinPtr> tmp122;
    compiler::TNode<FixedArray> tmp123;
    compiler::TNode<IntPtrT> tmp124;
    compiler::TNode<IntPtrT> tmp125;
    compiler::TNode<BoolT> tmp126;
    compiler::TNode<UintPtrT> tmp127;
    ca_.Bind(&block6, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 284);
    compiler::TNode<UintPtrT> tmp128;
    USE(tmp128);
    tmp128 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp129;
    USE(tmp129);
    tmp129 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrGreaterThan(compiler::TNode<UintPtrT>{tmp127}, compiler::TNode<UintPtrT>{tmp128}));
    ca_.Branch(tmp129, &block9, &block10, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp130;
    compiler::TNode<JSReceiver> tmp131;
    compiler::TNode<String> tmp132;
    compiler::TNode<Number> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<BuiltinPtr> tmp136;
    compiler::TNode<Map> tmp137;
    compiler::TNode<UintPtrT> tmp138;
    compiler::TNode<IntPtrT> tmp139;
    compiler::TNode<IntPtrT> tmp140;
    compiler::TNode<BuiltinPtr> tmp141;
    compiler::TNode<FixedArray> tmp142;
    compiler::TNode<IntPtrT> tmp143;
    compiler::TNode<IntPtrT> tmp144;
    compiler::TNode<BoolT> tmp145;
    compiler::TNode<UintPtrT> tmp146;
    ca_.Bind(&block9, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 286);
    compiler::TNode<IntPtrT> tmp147;
    USE(tmp147);
    tmp147 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp148;
    USE(tmp148);
    tmp148 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp140}, compiler::TNode<IntPtrT>{tmp147}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 284);
    ca_.Goto(&block10, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp148, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp149;
    compiler::TNode<JSReceiver> tmp150;
    compiler::TNode<String> tmp151;
    compiler::TNode<Number> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<BuiltinPtr> tmp155;
    compiler::TNode<Map> tmp156;
    compiler::TNode<UintPtrT> tmp157;
    compiler::TNode<IntPtrT> tmp158;
    compiler::TNode<IntPtrT> tmp159;
    compiler::TNode<BuiltinPtr> tmp160;
    compiler::TNode<FixedArray> tmp161;
    compiler::TNode<IntPtrT> tmp162;
    compiler::TNode<IntPtrT> tmp163;
    compiler::TNode<BoolT> tmp164;
    compiler::TNode<UintPtrT> tmp165;
    ca_.Bind(&block10, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 290);
    compiler::TNode<UintPtrT> tmp166;
    USE(tmp166);
    tmp166 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp165}, compiler::TNode<UintPtrT>{tmp166}));
    compiler::TNode<Number> tmp168;
    USE(tmp168);
    tmp168 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATuintptr(compiler::TNode<UintPtrT>{tmp165}));
    compiler::TNode<Object> tmp169 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(0)).descriptor(), tmp160, tmp149, tmp150, tmp168); 
    USE(tmp169);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 294);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 295);
    if ((p_useToLocaleString)) {
      ca_.Goto(&block11, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp167, tmp169, ca_.Uninitialized<String>());
    } else {
      ca_.Goto(&block12, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp167, tmp169, ca_.Uninitialized<String>());
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<JSReceiver> tmp171;
    compiler::TNode<String> tmp172;
    compiler::TNode<Number> tmp173;
    compiler::TNode<Object> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<BuiltinPtr> tmp176;
    compiler::TNode<Map> tmp177;
    compiler::TNode<UintPtrT> tmp178;
    compiler::TNode<IntPtrT> tmp179;
    compiler::TNode<IntPtrT> tmp180;
    compiler::TNode<BuiltinPtr> tmp181;
    compiler::TNode<FixedArray> tmp182;
    compiler::TNode<IntPtrT> tmp183;
    compiler::TNode<IntPtrT> tmp184;
    compiler::TNode<BoolT> tmp185;
    compiler::TNode<UintPtrT> tmp186;
    compiler::TNode<Object> tmp187;
    compiler::TNode<String> tmp188;
    ca_.Bind(&block11, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 296);
    compiler::TNode<String> tmp189;
    tmp189 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kConvertToLocaleString, tmp170, tmp187, tmp174, tmp175));
    USE(tmp189);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 297);
    compiler::TNode<String> tmp190;
    USE(tmp190);
    tmp190 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp191;
    USE(tmp191);
    tmp191 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp189}, compiler::TNode<HeapObject>{tmp190}));
    ca_.Branch(tmp191, &block14, &block15, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp189);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp192;
    compiler::TNode<JSReceiver> tmp193;
    compiler::TNode<String> tmp194;
    compiler::TNode<Number> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<Object> tmp197;
    compiler::TNode<BuiltinPtr> tmp198;
    compiler::TNode<Map> tmp199;
    compiler::TNode<UintPtrT> tmp200;
    compiler::TNode<IntPtrT> tmp201;
    compiler::TNode<IntPtrT> tmp202;
    compiler::TNode<BuiltinPtr> tmp203;
    compiler::TNode<FixedArray> tmp204;
    compiler::TNode<IntPtrT> tmp205;
    compiler::TNode<IntPtrT> tmp206;
    compiler::TNode<BoolT> tmp207;
    compiler::TNode<UintPtrT> tmp208;
    compiler::TNode<Object> tmp209;
    compiler::TNode<String> tmp210;
    ca_.Bind(&block14, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210);
    ca_.Goto(&block4, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp211;
    compiler::TNode<JSReceiver> tmp212;
    compiler::TNode<String> tmp213;
    compiler::TNode<Number> tmp214;
    compiler::TNode<Object> tmp215;
    compiler::TNode<Object> tmp216;
    compiler::TNode<BuiltinPtr> tmp217;
    compiler::TNode<Map> tmp218;
    compiler::TNode<UintPtrT> tmp219;
    compiler::TNode<IntPtrT> tmp220;
    compiler::TNode<IntPtrT> tmp221;
    compiler::TNode<BuiltinPtr> tmp222;
    compiler::TNode<FixedArray> tmp223;
    compiler::TNode<IntPtrT> tmp224;
    compiler::TNode<IntPtrT> tmp225;
    compiler::TNode<BoolT> tmp226;
    compiler::TNode<UintPtrT> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<String> tmp229;
    ca_.Bind(&block15, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 295);
    ca_.Goto(&block13, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp230;
    compiler::TNode<JSReceiver> tmp231;
    compiler::TNode<String> tmp232;
    compiler::TNode<Number> tmp233;
    compiler::TNode<Object> tmp234;
    compiler::TNode<Object> tmp235;
    compiler::TNode<BuiltinPtr> tmp236;
    compiler::TNode<Map> tmp237;
    compiler::TNode<UintPtrT> tmp238;
    compiler::TNode<IntPtrT> tmp239;
    compiler::TNode<IntPtrT> tmp240;
    compiler::TNode<BuiltinPtr> tmp241;
    compiler::TNode<FixedArray> tmp242;
    compiler::TNode<IntPtrT> tmp243;
    compiler::TNode<IntPtrT> tmp244;
    compiler::TNode<BoolT> tmp245;
    compiler::TNode<UintPtrT> tmp246;
    compiler::TNode<Object> tmp247;
    compiler::TNode<String> tmp248;
    ca_.Bind(&block12, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 299);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 300);
    compiler::TNode<String> tmp249;
    USE(tmp249);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp249 = BaseBuiltinsFromDSLAssembler(state_).Cast6String(compiler::TNode<Context>{tmp230}, compiler::TNode<Object>{tmp247}, &label0);
    ca_.Goto(&block18, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp247, tmp247, tmp249);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block19, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp247, tmp247);
    }
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp250;
    compiler::TNode<JSReceiver> tmp251;
    compiler::TNode<String> tmp252;
    compiler::TNode<Number> tmp253;
    compiler::TNode<Object> tmp254;
    compiler::TNode<Object> tmp255;
    compiler::TNode<BuiltinPtr> tmp256;
    compiler::TNode<Map> tmp257;
    compiler::TNode<UintPtrT> tmp258;
    compiler::TNode<IntPtrT> tmp259;
    compiler::TNode<IntPtrT> tmp260;
    compiler::TNode<BuiltinPtr> tmp261;
    compiler::TNode<FixedArray> tmp262;
    compiler::TNode<IntPtrT> tmp263;
    compiler::TNode<IntPtrT> tmp264;
    compiler::TNode<BoolT> tmp265;
    compiler::TNode<UintPtrT> tmp266;
    compiler::TNode<Object> tmp267;
    compiler::TNode<String> tmp268;
    compiler::TNode<Object> tmp269;
    compiler::TNode<Object> tmp270;
    ca_.Bind(&block19, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270);
    ca_.Goto(&block17, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp271;
    compiler::TNode<JSReceiver> tmp272;
    compiler::TNode<String> tmp273;
    compiler::TNode<Number> tmp274;
    compiler::TNode<Object> tmp275;
    compiler::TNode<Object> tmp276;
    compiler::TNode<BuiltinPtr> tmp277;
    compiler::TNode<Map> tmp278;
    compiler::TNode<UintPtrT> tmp279;
    compiler::TNode<IntPtrT> tmp280;
    compiler::TNode<IntPtrT> tmp281;
    compiler::TNode<BuiltinPtr> tmp282;
    compiler::TNode<FixedArray> tmp283;
    compiler::TNode<IntPtrT> tmp284;
    compiler::TNode<IntPtrT> tmp285;
    compiler::TNode<BoolT> tmp286;
    compiler::TNode<UintPtrT> tmp287;
    compiler::TNode<Object> tmp288;
    compiler::TNode<String> tmp289;
    compiler::TNode<Object> tmp290;
    compiler::TNode<Object> tmp291;
    compiler::TNode<String> tmp292;
    ca_.Bind(&block18, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 301);
    compiler::TNode<String> tmp293;
    USE(tmp293);
    tmp293 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp294;
    USE(tmp294);
    tmp294 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp292}, compiler::TNode<HeapObject>{tmp293}));
    ca_.Branch(tmp294, &block20, &block21, tmp271, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp292);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp295;
    compiler::TNode<JSReceiver> tmp296;
    compiler::TNode<String> tmp297;
    compiler::TNode<Number> tmp298;
    compiler::TNode<Object> tmp299;
    compiler::TNode<Object> tmp300;
    compiler::TNode<BuiltinPtr> tmp301;
    compiler::TNode<Map> tmp302;
    compiler::TNode<UintPtrT> tmp303;
    compiler::TNode<IntPtrT> tmp304;
    compiler::TNode<IntPtrT> tmp305;
    compiler::TNode<BuiltinPtr> tmp306;
    compiler::TNode<FixedArray> tmp307;
    compiler::TNode<IntPtrT> tmp308;
    compiler::TNode<IntPtrT> tmp309;
    compiler::TNode<BoolT> tmp310;
    compiler::TNode<UintPtrT> tmp311;
    compiler::TNode<Object> tmp312;
    compiler::TNode<String> tmp313;
    compiler::TNode<Object> tmp314;
    compiler::TNode<String> tmp315;
    ca_.Bind(&block20, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315);
    ca_.Goto(&block4, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp316;
    compiler::TNode<JSReceiver> tmp317;
    compiler::TNode<String> tmp318;
    compiler::TNode<Number> tmp319;
    compiler::TNode<Object> tmp320;
    compiler::TNode<Object> tmp321;
    compiler::TNode<BuiltinPtr> tmp322;
    compiler::TNode<Map> tmp323;
    compiler::TNode<UintPtrT> tmp324;
    compiler::TNode<IntPtrT> tmp325;
    compiler::TNode<IntPtrT> tmp326;
    compiler::TNode<BuiltinPtr> tmp327;
    compiler::TNode<FixedArray> tmp328;
    compiler::TNode<IntPtrT> tmp329;
    compiler::TNode<IntPtrT> tmp330;
    compiler::TNode<BoolT> tmp331;
    compiler::TNode<UintPtrT> tmp332;
    compiler::TNode<Object> tmp333;
    compiler::TNode<String> tmp334;
    compiler::TNode<Object> tmp335;
    compiler::TNode<String> tmp336;
    ca_.Bind(&block21, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 302);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 300);
    ca_.Goto(&block16, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp333, tmp336, tmp335);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp337;
    compiler::TNode<JSReceiver> tmp338;
    compiler::TNode<String> tmp339;
    compiler::TNode<Number> tmp340;
    compiler::TNode<Object> tmp341;
    compiler::TNode<Object> tmp342;
    compiler::TNode<BuiltinPtr> tmp343;
    compiler::TNode<Map> tmp344;
    compiler::TNode<UintPtrT> tmp345;
    compiler::TNode<IntPtrT> tmp346;
    compiler::TNode<IntPtrT> tmp347;
    compiler::TNode<BuiltinPtr> tmp348;
    compiler::TNode<FixedArray> tmp349;
    compiler::TNode<IntPtrT> tmp350;
    compiler::TNode<IntPtrT> tmp351;
    compiler::TNode<BoolT> tmp352;
    compiler::TNode<UintPtrT> tmp353;
    compiler::TNode<Object> tmp354;
    compiler::TNode<String> tmp355;
    compiler::TNode<Object> tmp356;
    ca_.Bind(&block17, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 304);
    compiler::TNode<Number> tmp357;
    USE(tmp357);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp357 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp356)}, &label0);
    ca_.Goto(&block24, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, ca_.UncheckedCast<Object>(tmp356), tmp357);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block25, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, ca_.UncheckedCast<Object>(tmp356));
    }
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp358;
    compiler::TNode<JSReceiver> tmp359;
    compiler::TNode<String> tmp360;
    compiler::TNode<Number> tmp361;
    compiler::TNode<Object> tmp362;
    compiler::TNode<Object> tmp363;
    compiler::TNode<BuiltinPtr> tmp364;
    compiler::TNode<Map> tmp365;
    compiler::TNode<UintPtrT> tmp366;
    compiler::TNode<IntPtrT> tmp367;
    compiler::TNode<IntPtrT> tmp368;
    compiler::TNode<BuiltinPtr> tmp369;
    compiler::TNode<FixedArray> tmp370;
    compiler::TNode<IntPtrT> tmp371;
    compiler::TNode<IntPtrT> tmp372;
    compiler::TNode<BoolT> tmp373;
    compiler::TNode<UintPtrT> tmp374;
    compiler::TNode<Object> tmp375;
    compiler::TNode<String> tmp376;
    compiler::TNode<Object> tmp377;
    compiler::TNode<Object> tmp378;
    ca_.Bind(&block25, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378);
    ca_.Goto(&block23, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp379;
    compiler::TNode<JSReceiver> tmp380;
    compiler::TNode<String> tmp381;
    compiler::TNode<Number> tmp382;
    compiler::TNode<Object> tmp383;
    compiler::TNode<Object> tmp384;
    compiler::TNode<BuiltinPtr> tmp385;
    compiler::TNode<Map> tmp386;
    compiler::TNode<UintPtrT> tmp387;
    compiler::TNode<IntPtrT> tmp388;
    compiler::TNode<IntPtrT> tmp389;
    compiler::TNode<BuiltinPtr> tmp390;
    compiler::TNode<FixedArray> tmp391;
    compiler::TNode<IntPtrT> tmp392;
    compiler::TNode<IntPtrT> tmp393;
    compiler::TNode<BoolT> tmp394;
    compiler::TNode<UintPtrT> tmp395;
    compiler::TNode<Object> tmp396;
    compiler::TNode<String> tmp397;
    compiler::TNode<Object> tmp398;
    compiler::TNode<Object> tmp399;
    compiler::TNode<Number> tmp400;
    ca_.Bind(&block24, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 305);
    compiler::TNode<String> tmp401;
    USE(tmp401);
    tmp401 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).NumberToString(compiler::TNode<Number>{tmp400}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 304);
    ca_.Goto(&block22, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp396, tmp401, tmp398);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp402;
    compiler::TNode<JSReceiver> tmp403;
    compiler::TNode<String> tmp404;
    compiler::TNode<Number> tmp405;
    compiler::TNode<Object> tmp406;
    compiler::TNode<Object> tmp407;
    compiler::TNode<BuiltinPtr> tmp408;
    compiler::TNode<Map> tmp409;
    compiler::TNode<UintPtrT> tmp410;
    compiler::TNode<IntPtrT> tmp411;
    compiler::TNode<IntPtrT> tmp412;
    compiler::TNode<BuiltinPtr> tmp413;
    compiler::TNode<FixedArray> tmp414;
    compiler::TNode<IntPtrT> tmp415;
    compiler::TNode<IntPtrT> tmp416;
    compiler::TNode<BoolT> tmp417;
    compiler::TNode<UintPtrT> tmp418;
    compiler::TNode<Object> tmp419;
    compiler::TNode<String> tmp420;
    compiler::TNode<Object> tmp421;
    ca_.Bind(&block23, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420, &tmp421);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 307);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 308);
    compiler::TNode<BoolT> tmp422;
    USE(tmp422);
    tmp422 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{ca_.UncheckedCast<HeapObject>(tmp421)}));
    ca_.Branch(tmp422, &block26, &block27, tmp402, tmp403, tmp404, tmp405, tmp406, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420, tmp421, ca_.UncheckedCast<HeapObject>(tmp421));
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp423;
    compiler::TNode<JSReceiver> tmp424;
    compiler::TNode<String> tmp425;
    compiler::TNode<Number> tmp426;
    compiler::TNode<Object> tmp427;
    compiler::TNode<Object> tmp428;
    compiler::TNode<BuiltinPtr> tmp429;
    compiler::TNode<Map> tmp430;
    compiler::TNode<UintPtrT> tmp431;
    compiler::TNode<IntPtrT> tmp432;
    compiler::TNode<IntPtrT> tmp433;
    compiler::TNode<BuiltinPtr> tmp434;
    compiler::TNode<FixedArray> tmp435;
    compiler::TNode<IntPtrT> tmp436;
    compiler::TNode<IntPtrT> tmp437;
    compiler::TNode<BoolT> tmp438;
    compiler::TNode<UintPtrT> tmp439;
    compiler::TNode<Object> tmp440;
    compiler::TNode<String> tmp441;
    compiler::TNode<Object> tmp442;
    compiler::TNode<HeapObject> tmp443;
    ca_.Bind(&block26, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442, &tmp443);
    ca_.Goto(&block4, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438, tmp439);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp444;
    compiler::TNode<JSReceiver> tmp445;
    compiler::TNode<String> tmp446;
    compiler::TNode<Number> tmp447;
    compiler::TNode<Object> tmp448;
    compiler::TNode<Object> tmp449;
    compiler::TNode<BuiltinPtr> tmp450;
    compiler::TNode<Map> tmp451;
    compiler::TNode<UintPtrT> tmp452;
    compiler::TNode<IntPtrT> tmp453;
    compiler::TNode<IntPtrT> tmp454;
    compiler::TNode<BuiltinPtr> tmp455;
    compiler::TNode<FixedArray> tmp456;
    compiler::TNode<IntPtrT> tmp457;
    compiler::TNode<IntPtrT> tmp458;
    compiler::TNode<BoolT> tmp459;
    compiler::TNode<UintPtrT> tmp460;
    compiler::TNode<Object> tmp461;
    compiler::TNode<String> tmp462;
    compiler::TNode<Object> tmp463;
    compiler::TNode<HeapObject> tmp464;
    ca_.Bind(&block27, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463, &tmp464);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 309);
    compiler::TNode<String> tmp465;
    tmp465 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kToString, tmp444, tmp464));
    USE(tmp465);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 304);
    ca_.Goto(&block22, tmp444, tmp445, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452, tmp453, tmp454, tmp455, tmp456, tmp457, tmp458, tmp459, tmp460, tmp461, tmp465, tmp463);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp466;
    compiler::TNode<JSReceiver> tmp467;
    compiler::TNode<String> tmp468;
    compiler::TNode<Number> tmp469;
    compiler::TNode<Object> tmp470;
    compiler::TNode<Object> tmp471;
    compiler::TNode<BuiltinPtr> tmp472;
    compiler::TNode<Map> tmp473;
    compiler::TNode<UintPtrT> tmp474;
    compiler::TNode<IntPtrT> tmp475;
    compiler::TNode<IntPtrT> tmp476;
    compiler::TNode<BuiltinPtr> tmp477;
    compiler::TNode<FixedArray> tmp478;
    compiler::TNode<IntPtrT> tmp479;
    compiler::TNode<IntPtrT> tmp480;
    compiler::TNode<BoolT> tmp481;
    compiler::TNode<UintPtrT> tmp482;
    compiler::TNode<Object> tmp483;
    compiler::TNode<String> tmp484;
    compiler::TNode<Object> tmp485;
    ca_.Bind(&block22, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478, &tmp479, &tmp480, &tmp481, &tmp482, &tmp483, &tmp484, &tmp485);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 300);
    ca_.Goto(&block16, tmp466, tmp467, tmp468, tmp469, tmp470, tmp471, tmp472, tmp473, tmp474, tmp475, tmp476, tmp477, tmp478, tmp479, tmp480, tmp481, tmp482, tmp483, tmp484, tmp485);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp486;
    compiler::TNode<JSReceiver> tmp487;
    compiler::TNode<String> tmp488;
    compiler::TNode<Number> tmp489;
    compiler::TNode<Object> tmp490;
    compiler::TNode<Object> tmp491;
    compiler::TNode<BuiltinPtr> tmp492;
    compiler::TNode<Map> tmp493;
    compiler::TNode<UintPtrT> tmp494;
    compiler::TNode<IntPtrT> tmp495;
    compiler::TNode<IntPtrT> tmp496;
    compiler::TNode<BuiltinPtr> tmp497;
    compiler::TNode<FixedArray> tmp498;
    compiler::TNode<IntPtrT> tmp499;
    compiler::TNode<IntPtrT> tmp500;
    compiler::TNode<BoolT> tmp501;
    compiler::TNode<UintPtrT> tmp502;
    compiler::TNode<Object> tmp503;
    compiler::TNode<String> tmp504;
    compiler::TNode<Object> tmp505;
    ca_.Bind(&block16, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501, &tmp502, &tmp503, &tmp504, &tmp505);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 299);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 295);
    ca_.Goto(&block13, tmp486, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492, tmp493, tmp494, tmp495, tmp496, tmp497, tmp498, tmp499, tmp500, tmp501, tmp502, tmp503, tmp504);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp506;
    compiler::TNode<JSReceiver> tmp507;
    compiler::TNode<String> tmp508;
    compiler::TNode<Number> tmp509;
    compiler::TNode<Object> tmp510;
    compiler::TNode<Object> tmp511;
    compiler::TNode<BuiltinPtr> tmp512;
    compiler::TNode<Map> tmp513;
    compiler::TNode<UintPtrT> tmp514;
    compiler::TNode<IntPtrT> tmp515;
    compiler::TNode<IntPtrT> tmp516;
    compiler::TNode<BuiltinPtr> tmp517;
    compiler::TNode<FixedArray> tmp518;
    compiler::TNode<IntPtrT> tmp519;
    compiler::TNode<IntPtrT> tmp520;
    compiler::TNode<BoolT> tmp521;
    compiler::TNode<UintPtrT> tmp522;
    compiler::TNode<Object> tmp523;
    compiler::TNode<String> tmp524;
    ca_.Bind(&block13, &tmp506, &tmp507, &tmp508, &tmp509, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518, &tmp519, &tmp520, &tmp521, &tmp522, &tmp523, &tmp524);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 315);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 158);
    compiler::TNode<IntPtrT> tmp525;
    USE(tmp525);
    tmp525 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp526;
    USE(tmp526);
    tmp526 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp519}, compiler::TNode<IntPtrT>{tmp525}));
    compiler::TNode<IntPtrT> tmp527;
    USE(tmp527);
    tmp527 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp528;
    USE(tmp528);
    tmp528 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp516}, compiler::TNode<IntPtrT>{tmp527}));
    compiler::TNode<BoolT> tmp529;
    USE(tmp529);
    tmp529 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Or(compiler::TNode<BoolT>{tmp526}, compiler::TNode<BoolT>{tmp528}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 159);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 171);
    compiler::TNode<IntPtrT> tmp530;
    USE(tmp530);
    tmp530 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp531;
    USE(tmp531);
    tmp531 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp516}, compiler::TNode<IntPtrT>{tmp530}));
    ca_.Branch(tmp531, &block30, &block32, tmp506, tmp507, tmp508, tmp509, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517, tmp518, tmp519, tmp520, tmp521, tmp522, tmp523, tmp524, tmp524, tmp516, tmp515, tmp506, tmp524, tmp516, tmp515, tmp529, tmp516, tmp515, tmp529, tmp506, tmp516, tmp515, tmp529);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp532;
    compiler::TNode<JSReceiver> tmp533;
    compiler::TNode<String> tmp534;
    compiler::TNode<Number> tmp535;
    compiler::TNode<Object> tmp536;
    compiler::TNode<Object> tmp537;
    compiler::TNode<BuiltinPtr> tmp538;
    compiler::TNode<Map> tmp539;
    compiler::TNode<UintPtrT> tmp540;
    compiler::TNode<IntPtrT> tmp541;
    compiler::TNode<IntPtrT> tmp542;
    compiler::TNode<BuiltinPtr> tmp543;
    compiler::TNode<FixedArray> tmp544;
    compiler::TNode<IntPtrT> tmp545;
    compiler::TNode<IntPtrT> tmp546;
    compiler::TNode<BoolT> tmp547;
    compiler::TNode<UintPtrT> tmp548;
    compiler::TNode<Object> tmp549;
    compiler::TNode<String> tmp550;
    compiler::TNode<String> tmp551;
    compiler::TNode<IntPtrT> tmp552;
    compiler::TNode<IntPtrT> tmp553;
    compiler::TNode<Context> tmp554;
    compiler::TNode<String> tmp555;
    compiler::TNode<IntPtrT> tmp556;
    compiler::TNode<IntPtrT> tmp557;
    compiler::TNode<BoolT> tmp558;
    compiler::TNode<IntPtrT> tmp559;
    compiler::TNode<IntPtrT> tmp560;
    compiler::TNode<BoolT> tmp561;
    compiler::TNode<Context> tmp562;
    compiler::TNode<IntPtrT> tmp563;
    compiler::TNode<IntPtrT> tmp564;
    compiler::TNode<BoolT> tmp565;
    ca_.Bind(&block32, &tmp532, &tmp533, &tmp534, &tmp535, &tmp536, &tmp537, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544, &tmp545, &tmp546, &tmp547, &tmp548, &tmp549, &tmp550, &tmp551, &tmp552, &tmp553, &tmp554, &tmp555, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563, &tmp564, &tmp565);
    compiler::TNode<IntPtrT> tmp566;
    USE(tmp566);
    tmp566 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp567;
    USE(tmp567);
    tmp567 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp564}, compiler::TNode<IntPtrT>{tmp566}));
    ca_.Branch(tmp567, &block30, &block31, tmp532, tmp533, tmp534, tmp535, tmp536, tmp537, tmp538, tmp539, tmp540, tmp541, tmp542, tmp543, tmp544, tmp545, tmp546, tmp547, tmp548, tmp549, tmp550, tmp551, tmp552, tmp553, tmp554, tmp555, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562, tmp563, tmp564, tmp565);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp568;
    compiler::TNode<JSReceiver> tmp569;
    compiler::TNode<String> tmp570;
    compiler::TNode<Number> tmp571;
    compiler::TNode<Object> tmp572;
    compiler::TNode<Object> tmp573;
    compiler::TNode<BuiltinPtr> tmp574;
    compiler::TNode<Map> tmp575;
    compiler::TNode<UintPtrT> tmp576;
    compiler::TNode<IntPtrT> tmp577;
    compiler::TNode<IntPtrT> tmp578;
    compiler::TNode<BuiltinPtr> tmp579;
    compiler::TNode<FixedArray> tmp580;
    compiler::TNode<IntPtrT> tmp581;
    compiler::TNode<IntPtrT> tmp582;
    compiler::TNode<BoolT> tmp583;
    compiler::TNode<UintPtrT> tmp584;
    compiler::TNode<Object> tmp585;
    compiler::TNode<String> tmp586;
    compiler::TNode<String> tmp587;
    compiler::TNode<IntPtrT> tmp588;
    compiler::TNode<IntPtrT> tmp589;
    compiler::TNode<Context> tmp590;
    compiler::TNode<String> tmp591;
    compiler::TNode<IntPtrT> tmp592;
    compiler::TNode<IntPtrT> tmp593;
    compiler::TNode<BoolT> tmp594;
    compiler::TNode<IntPtrT> tmp595;
    compiler::TNode<IntPtrT> tmp596;
    compiler::TNode<BoolT> tmp597;
    compiler::TNode<Context> tmp598;
    compiler::TNode<IntPtrT> tmp599;
    compiler::TNode<IntPtrT> tmp600;
    compiler::TNode<BoolT> tmp601;
    ca_.Bind(&block30, &tmp568, &tmp569, &tmp570, &tmp571, &tmp572, &tmp573, &tmp574, &tmp575, &tmp576, &tmp577, &tmp578, &tmp579, &tmp580, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585, &tmp586, &tmp587, &tmp588, &tmp589, &tmp590, &tmp591, &tmp592, &tmp593, &tmp594, &tmp595, &tmp596, &tmp597, &tmp598, &tmp599, &tmp600, &tmp601);
    ca_.Goto(&block29, tmp568, tmp569, tmp570, tmp571, tmp572, tmp573, tmp574, tmp575, tmp576, tmp577, tmp578, tmp579, tmp580, tmp581, tmp582, tmp583, tmp584, tmp585, tmp586, tmp587, tmp588, tmp589, tmp590, tmp591, tmp592, tmp593, tmp594, tmp595, tmp596, tmp597, tmp598, tmp599, tmp600, tmp601);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp602;
    compiler::TNode<JSReceiver> tmp603;
    compiler::TNode<String> tmp604;
    compiler::TNode<Number> tmp605;
    compiler::TNode<Object> tmp606;
    compiler::TNode<Object> tmp607;
    compiler::TNode<BuiltinPtr> tmp608;
    compiler::TNode<Map> tmp609;
    compiler::TNode<UintPtrT> tmp610;
    compiler::TNode<IntPtrT> tmp611;
    compiler::TNode<IntPtrT> tmp612;
    compiler::TNode<BuiltinPtr> tmp613;
    compiler::TNode<FixedArray> tmp614;
    compiler::TNode<IntPtrT> tmp615;
    compiler::TNode<IntPtrT> tmp616;
    compiler::TNode<BoolT> tmp617;
    compiler::TNode<UintPtrT> tmp618;
    compiler::TNode<Object> tmp619;
    compiler::TNode<String> tmp620;
    compiler::TNode<String> tmp621;
    compiler::TNode<IntPtrT> tmp622;
    compiler::TNode<IntPtrT> tmp623;
    compiler::TNode<Context> tmp624;
    compiler::TNode<String> tmp625;
    compiler::TNode<IntPtrT> tmp626;
    compiler::TNode<IntPtrT> tmp627;
    compiler::TNode<BoolT> tmp628;
    compiler::TNode<IntPtrT> tmp629;
    compiler::TNode<IntPtrT> tmp630;
    compiler::TNode<BoolT> tmp631;
    compiler::TNode<Context> tmp632;
    compiler::TNode<IntPtrT> tmp633;
    compiler::TNode<IntPtrT> tmp634;
    compiler::TNode<BoolT> tmp635;
    ca_.Bind(&block31, &tmp602, &tmp603, &tmp604, &tmp605, &tmp606, &tmp607, &tmp608, &tmp609, &tmp610, &tmp611, &tmp612, &tmp613, &tmp614, &tmp615, &tmp616, &tmp617, &tmp618, &tmp619, &tmp620, &tmp621, &tmp622, &tmp623, &tmp624, &tmp625, &tmp626, &tmp627, &tmp628, &tmp629, &tmp630, &tmp631, &tmp632, &tmp633, &tmp634, &tmp635);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 173);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 174);
    compiler::TNode<IntPtrT> tmp636;
    USE(tmp636);
    tmp636 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMul(compiler::TNode<IntPtrT>{tmp634}, compiler::TNode<IntPtrT>{tmp633}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 177);
    compiler::TNode<IntPtrT> tmp637;
    USE(tmp637);
    tmp637 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrDiv(compiler::TNode<IntPtrT>{tmp636}, compiler::TNode<IntPtrT>{tmp634}));
    compiler::TNode<BoolT> tmp638;
    USE(tmp638);
    tmp638 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<IntPtrT>{tmp637}, compiler::TNode<IntPtrT>{tmp633}));
    ca_.Branch(tmp638, &block33, &block34, tmp602, tmp603, tmp604, tmp605, tmp606, tmp607, tmp608, tmp609, tmp610, tmp611, tmp612, tmp613, tmp614, tmp615, tmp616, tmp617, tmp618, tmp619, tmp620, tmp621, tmp622, tmp623, tmp624, tmp625, tmp626, tmp627, tmp628, tmp629, tmp630, tmp631, tmp632, tmp633, tmp634, tmp635, tmp633, tmp636);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp639;
    compiler::TNode<JSReceiver> tmp640;
    compiler::TNode<String> tmp641;
    compiler::TNode<Number> tmp642;
    compiler::TNode<Object> tmp643;
    compiler::TNode<Object> tmp644;
    compiler::TNode<BuiltinPtr> tmp645;
    compiler::TNode<Map> tmp646;
    compiler::TNode<UintPtrT> tmp647;
    compiler::TNode<IntPtrT> tmp648;
    compiler::TNode<IntPtrT> tmp649;
    compiler::TNode<BuiltinPtr> tmp650;
    compiler::TNode<FixedArray> tmp651;
    compiler::TNode<IntPtrT> tmp652;
    compiler::TNode<IntPtrT> tmp653;
    compiler::TNode<BoolT> tmp654;
    compiler::TNode<UintPtrT> tmp655;
    compiler::TNode<Object> tmp656;
    compiler::TNode<String> tmp657;
    compiler::TNode<String> tmp658;
    compiler::TNode<IntPtrT> tmp659;
    compiler::TNode<IntPtrT> tmp660;
    compiler::TNode<Context> tmp661;
    compiler::TNode<String> tmp662;
    compiler::TNode<IntPtrT> tmp663;
    compiler::TNode<IntPtrT> tmp664;
    compiler::TNode<BoolT> tmp665;
    compiler::TNode<IntPtrT> tmp666;
    compiler::TNode<IntPtrT> tmp667;
    compiler::TNode<BoolT> tmp668;
    compiler::TNode<Context> tmp669;
    compiler::TNode<IntPtrT> tmp670;
    compiler::TNode<IntPtrT> tmp671;
    compiler::TNode<BoolT> tmp672;
    compiler::TNode<IntPtrT> tmp673;
    compiler::TNode<IntPtrT> tmp674;
    ca_.Bind(&block33, &tmp639, &tmp640, &tmp641, &tmp642, &tmp643, &tmp644, &tmp645, &tmp646, &tmp647, &tmp648, &tmp649, &tmp650, &tmp651, &tmp652, &tmp653, &tmp654, &tmp655, &tmp656, &tmp657, &tmp658, &tmp659, &tmp660, &tmp661, &tmp662, &tmp663, &tmp664, &tmp665, &tmp666, &tmp667, &tmp668, &tmp669, &tmp670, &tmp671, &tmp672, &tmp673, &tmp674);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 178);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp669);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp676;
    compiler::TNode<JSReceiver> tmp677;
    compiler::TNode<String> tmp678;
    compiler::TNode<Number> tmp679;
    compiler::TNode<Object> tmp680;
    compiler::TNode<Object> tmp681;
    compiler::TNode<BuiltinPtr> tmp682;
    compiler::TNode<Map> tmp683;
    compiler::TNode<UintPtrT> tmp684;
    compiler::TNode<IntPtrT> tmp685;
    compiler::TNode<IntPtrT> tmp686;
    compiler::TNode<BuiltinPtr> tmp687;
    compiler::TNode<FixedArray> tmp688;
    compiler::TNode<IntPtrT> tmp689;
    compiler::TNode<IntPtrT> tmp690;
    compiler::TNode<BoolT> tmp691;
    compiler::TNode<UintPtrT> tmp692;
    compiler::TNode<Object> tmp693;
    compiler::TNode<String> tmp694;
    compiler::TNode<String> tmp695;
    compiler::TNode<IntPtrT> tmp696;
    compiler::TNode<IntPtrT> tmp697;
    compiler::TNode<Context> tmp698;
    compiler::TNode<String> tmp699;
    compiler::TNode<IntPtrT> tmp700;
    compiler::TNode<IntPtrT> tmp701;
    compiler::TNode<BoolT> tmp702;
    compiler::TNode<IntPtrT> tmp703;
    compiler::TNode<IntPtrT> tmp704;
    compiler::TNode<BoolT> tmp705;
    compiler::TNode<Context> tmp706;
    compiler::TNode<IntPtrT> tmp707;
    compiler::TNode<IntPtrT> tmp708;
    compiler::TNode<BoolT> tmp709;
    compiler::TNode<IntPtrT> tmp710;
    compiler::TNode<IntPtrT> tmp711;
    ca_.Bind(&block34, &tmp676, &tmp677, &tmp678, &tmp679, &tmp680, &tmp681, &tmp682, &tmp683, &tmp684, &tmp685, &tmp686, &tmp687, &tmp688, &tmp689, &tmp690, &tmp691, &tmp692, &tmp693, &tmp694, &tmp695, &tmp696, &tmp697, &tmp698, &tmp699, &tmp700, &tmp701, &tmp702, &tmp703, &tmp704, &tmp705, &tmp706, &tmp707, &tmp708, &tmp709, &tmp710, &tmp711);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 181);
    compiler::TNode<IntPtrT> tmp712;
    USE(tmp712);
    tmp712 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp706}, compiler::TNode<IntPtrT>{tmp690}, compiler::TNode<IntPtrT>{tmp711}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 182);
    ca_.Branch(tmp709, &block35, &block36, tmp676, tmp677, tmp678, tmp679, tmp680, tmp681, tmp682, tmp683, tmp684, tmp685, tmp686, tmp687, tmp688, tmp689, tmp712, tmp691, tmp692, tmp693, tmp694, tmp695, tmp696, tmp697, tmp698, tmp699, tmp700, tmp701, tmp702, tmp703, tmp704, tmp705, tmp706, tmp707, tmp708, tmp709, tmp710, tmp711);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp713;
    compiler::TNode<JSReceiver> tmp714;
    compiler::TNode<String> tmp715;
    compiler::TNode<Number> tmp716;
    compiler::TNode<Object> tmp717;
    compiler::TNode<Object> tmp718;
    compiler::TNode<BuiltinPtr> tmp719;
    compiler::TNode<Map> tmp720;
    compiler::TNode<UintPtrT> tmp721;
    compiler::TNode<IntPtrT> tmp722;
    compiler::TNode<IntPtrT> tmp723;
    compiler::TNode<BuiltinPtr> tmp724;
    compiler::TNode<FixedArray> tmp725;
    compiler::TNode<IntPtrT> tmp726;
    compiler::TNode<IntPtrT> tmp727;
    compiler::TNode<BoolT> tmp728;
    compiler::TNode<UintPtrT> tmp729;
    compiler::TNode<Object> tmp730;
    compiler::TNode<String> tmp731;
    compiler::TNode<String> tmp732;
    compiler::TNode<IntPtrT> tmp733;
    compiler::TNode<IntPtrT> tmp734;
    compiler::TNode<Context> tmp735;
    compiler::TNode<String> tmp736;
    compiler::TNode<IntPtrT> tmp737;
    compiler::TNode<IntPtrT> tmp738;
    compiler::TNode<BoolT> tmp739;
    compiler::TNode<IntPtrT> tmp740;
    compiler::TNode<IntPtrT> tmp741;
    compiler::TNode<BoolT> tmp742;
    compiler::TNode<Context> tmp743;
    compiler::TNode<IntPtrT> tmp744;
    compiler::TNode<IntPtrT> tmp745;
    compiler::TNode<BoolT> tmp746;
    compiler::TNode<IntPtrT> tmp747;
    compiler::TNode<IntPtrT> tmp748;
    ca_.Bind(&block35, &tmp713, &tmp714, &tmp715, &tmp716, &tmp717, &tmp718, &tmp719, &tmp720, &tmp721, &tmp722, &tmp723, &tmp724, &tmp725, &tmp726, &tmp727, &tmp728, &tmp729, &tmp730, &tmp731, &tmp732, &tmp733, &tmp734, &tmp735, &tmp736, &tmp737, &tmp738, &tmp739, &tmp740, &tmp741, &tmp742, &tmp743, &tmp744, &tmp745, &tmp746, &tmp747, &tmp748);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 184);
    compiler::TNode<IntPtrT> tmp749;
    USE(tmp749);
    tmp749 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp750;
    USE(tmp750);
    tmp750 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp726}, compiler::TNode<IntPtrT>{tmp749}));
    compiler::TNode<Smi> tmp751;
    USE(tmp751);
    tmp751 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp747}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 183);
    compiler::TNode<FixedArray> tmp752;
    USE(tmp752);
    tmp752 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray>{tmp725}, compiler::TNode<IntPtrT>{tmp726}, compiler::TNode<Smi>{tmp751}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 182);
    ca_.Goto(&block36, tmp713, tmp714, tmp715, tmp716, tmp717, tmp718, tmp719, tmp720, tmp721, tmp722, tmp723, tmp724, tmp752, tmp750, tmp727, tmp728, tmp729, tmp730, tmp731, tmp732, tmp733, tmp734, tmp735, tmp736, tmp737, tmp738, tmp739, tmp740, tmp741, tmp742, tmp743, tmp744, tmp745, tmp746, tmp747, tmp748);
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp753;
    compiler::TNode<JSReceiver> tmp754;
    compiler::TNode<String> tmp755;
    compiler::TNode<Number> tmp756;
    compiler::TNode<Object> tmp757;
    compiler::TNode<Object> tmp758;
    compiler::TNode<BuiltinPtr> tmp759;
    compiler::TNode<Map> tmp760;
    compiler::TNode<UintPtrT> tmp761;
    compiler::TNode<IntPtrT> tmp762;
    compiler::TNode<IntPtrT> tmp763;
    compiler::TNode<BuiltinPtr> tmp764;
    compiler::TNode<FixedArray> tmp765;
    compiler::TNode<IntPtrT> tmp766;
    compiler::TNode<IntPtrT> tmp767;
    compiler::TNode<BoolT> tmp768;
    compiler::TNode<UintPtrT> tmp769;
    compiler::TNode<Object> tmp770;
    compiler::TNode<String> tmp771;
    compiler::TNode<String> tmp772;
    compiler::TNode<IntPtrT> tmp773;
    compiler::TNode<IntPtrT> tmp774;
    compiler::TNode<Context> tmp775;
    compiler::TNode<String> tmp776;
    compiler::TNode<IntPtrT> tmp777;
    compiler::TNode<IntPtrT> tmp778;
    compiler::TNode<BoolT> tmp779;
    compiler::TNode<IntPtrT> tmp780;
    compiler::TNode<IntPtrT> tmp781;
    compiler::TNode<BoolT> tmp782;
    compiler::TNode<Context> tmp783;
    compiler::TNode<IntPtrT> tmp784;
    compiler::TNode<IntPtrT> tmp785;
    compiler::TNode<BoolT> tmp786;
    compiler::TNode<IntPtrT> tmp787;
    compiler::TNode<IntPtrT> tmp788;
    ca_.Bind(&block36, &tmp753, &tmp754, &tmp755, &tmp756, &tmp757, &tmp758, &tmp759, &tmp760, &tmp761, &tmp762, &tmp763, &tmp764, &tmp765, &tmp766, &tmp767, &tmp768, &tmp769, &tmp770, &tmp771, &tmp772, &tmp773, &tmp774, &tmp775, &tmp776, &tmp777, &tmp778, &tmp779, &tmp780, &tmp781, &tmp782, &tmp783, &tmp784, &tmp785, &tmp786, &tmp787, &tmp788);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 170);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 159);
    ca_.Goto(&block29, tmp753, tmp754, tmp755, tmp756, tmp757, tmp758, tmp759, tmp760, tmp761, tmp762, tmp763, tmp764, tmp765, tmp766, tmp767, tmp768, tmp769, tmp770, tmp771, tmp772, tmp773, tmp774, tmp775, tmp776, tmp777, tmp778, tmp779, tmp780, tmp781, tmp782, tmp783, tmp784, tmp785, tmp786);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp789;
    compiler::TNode<JSReceiver> tmp790;
    compiler::TNode<String> tmp791;
    compiler::TNode<Number> tmp792;
    compiler::TNode<Object> tmp793;
    compiler::TNode<Object> tmp794;
    compiler::TNode<BuiltinPtr> tmp795;
    compiler::TNode<Map> tmp796;
    compiler::TNode<UintPtrT> tmp797;
    compiler::TNode<IntPtrT> tmp798;
    compiler::TNode<IntPtrT> tmp799;
    compiler::TNode<BuiltinPtr> tmp800;
    compiler::TNode<FixedArray> tmp801;
    compiler::TNode<IntPtrT> tmp802;
    compiler::TNode<IntPtrT> tmp803;
    compiler::TNode<BoolT> tmp804;
    compiler::TNode<UintPtrT> tmp805;
    compiler::TNode<Object> tmp806;
    compiler::TNode<String> tmp807;
    compiler::TNode<String> tmp808;
    compiler::TNode<IntPtrT> tmp809;
    compiler::TNode<IntPtrT> tmp810;
    compiler::TNode<Context> tmp811;
    compiler::TNode<String> tmp812;
    compiler::TNode<IntPtrT> tmp813;
    compiler::TNode<IntPtrT> tmp814;
    compiler::TNode<BoolT> tmp815;
    compiler::TNode<IntPtrT> tmp816;
    compiler::TNode<IntPtrT> tmp817;
    compiler::TNode<BoolT> tmp818;
    compiler::TNode<Context> tmp819;
    compiler::TNode<IntPtrT> tmp820;
    compiler::TNode<IntPtrT> tmp821;
    compiler::TNode<BoolT> tmp822;
    ca_.Bind(&block29, &tmp789, &tmp790, &tmp791, &tmp792, &tmp793, &tmp794, &tmp795, &tmp796, &tmp797, &tmp798, &tmp799, &tmp800, &tmp801, &tmp802, &tmp803, &tmp804, &tmp805, &tmp806, &tmp807, &tmp808, &tmp809, &tmp810, &tmp811, &tmp812, &tmp813, &tmp814, &tmp815, &tmp816, &tmp817, &tmp818, &tmp819, &tmp820, &tmp821, &tmp822);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 162);
    compiler::TNode<IntPtrT> tmp823;
    USE(tmp823);
    tmp823 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadStringLengthAsWord(compiler::TNode<String>{tmp812}));
    compiler::TNode<IntPtrT> tmp824;
    USE(tmp824);
    tmp824 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp811}, compiler::TNode<IntPtrT>{tmp803}, compiler::TNode<IntPtrT>{tmp823}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 161);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 164);
    compiler::TNode<IntPtrT> tmp825;
    USE(tmp825);
    tmp825 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp826;
    USE(tmp826);
    tmp826 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp802}, compiler::TNode<IntPtrT>{tmp825}));
    compiler::TNode<FixedArray> tmp827;
    USE(tmp827);
    tmp827 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray6String(compiler::TNode<FixedArray>{tmp801}, compiler::TNode<IntPtrT>{tmp802}, compiler::TNode<String>{tmp812}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 163);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 166);
    compiler::TNode<Int32T> tmp828;
    USE(tmp828);
    tmp828 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadInstanceType(compiler::TNode<HeapObject>{tmp812}));
    compiler::TNode<BoolT> tmp829;
    USE(tmp829);
    tmp829 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsOneByteStringInstanceType(compiler::TNode<Int32T>{tmp828}));
    compiler::TNode<BoolT> tmp830;
    USE(tmp830);
    tmp830 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32And(compiler::TNode<BoolT>{tmp829}, compiler::TNode<BoolT>{tmp804}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 165);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 156);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 315);
    ca_.Goto(&block28, tmp789, tmp790, tmp791, tmp792, tmp793, tmp794, tmp795, tmp796, tmp797, tmp798, tmp799, tmp800, tmp827, tmp826, tmp824, tmp830, tmp805, tmp806, tmp807, tmp808, tmp809, tmp810, tmp811, tmp812, tmp813, tmp814);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp831;
    compiler::TNode<JSReceiver> tmp832;
    compiler::TNode<String> tmp833;
    compiler::TNode<Number> tmp834;
    compiler::TNode<Object> tmp835;
    compiler::TNode<Object> tmp836;
    compiler::TNode<BuiltinPtr> tmp837;
    compiler::TNode<Map> tmp838;
    compiler::TNode<UintPtrT> tmp839;
    compiler::TNode<IntPtrT> tmp840;
    compiler::TNode<IntPtrT> tmp841;
    compiler::TNode<BuiltinPtr> tmp842;
    compiler::TNode<FixedArray> tmp843;
    compiler::TNode<IntPtrT> tmp844;
    compiler::TNode<IntPtrT> tmp845;
    compiler::TNode<BoolT> tmp846;
    compiler::TNode<UintPtrT> tmp847;
    compiler::TNode<Object> tmp848;
    compiler::TNode<String> tmp849;
    compiler::TNode<String> tmp850;
    compiler::TNode<IntPtrT> tmp851;
    compiler::TNode<IntPtrT> tmp852;
    compiler::TNode<Context> tmp853;
    compiler::TNode<String> tmp854;
    compiler::TNode<IntPtrT> tmp855;
    compiler::TNode<IntPtrT> tmp856;
    ca_.Bind(&block28, &tmp831, &tmp832, &tmp833, &tmp834, &tmp835, &tmp836, &tmp837, &tmp838, &tmp839, &tmp840, &tmp841, &tmp842, &tmp843, &tmp844, &tmp845, &tmp846, &tmp847, &tmp848, &tmp849, &tmp850, &tmp851, &tmp852, &tmp853, &tmp854, &tmp855, &tmp856);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 316);
    compiler::TNode<IntPtrT> tmp857;
    USE(tmp857);
    tmp857 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 277);
    ca_.Goto(&block4, tmp831, tmp832, tmp833, tmp834, tmp835, tmp836, tmp837, tmp838, tmp839, tmp840, tmp857, tmp842, tmp843, tmp844, tmp845, tmp846, tmp847);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp858;
    compiler::TNode<JSReceiver> tmp859;
    compiler::TNode<String> tmp860;
    compiler::TNode<Number> tmp861;
    compiler::TNode<Object> tmp862;
    compiler::TNode<Object> tmp863;
    compiler::TNode<BuiltinPtr> tmp864;
    compiler::TNode<Map> tmp865;
    compiler::TNode<UintPtrT> tmp866;
    compiler::TNode<IntPtrT> tmp867;
    compiler::TNode<IntPtrT> tmp868;
    compiler::TNode<BuiltinPtr> tmp869;
    compiler::TNode<FixedArray> tmp870;
    compiler::TNode<IntPtrT> tmp871;
    compiler::TNode<IntPtrT> tmp872;
    compiler::TNode<BoolT> tmp873;
    compiler::TNode<UintPtrT> tmp874;
    ca_.Bind(&block3, &tmp858, &tmp859, &tmp860, &tmp861, &tmp862, &tmp863, &tmp864, &tmp865, &tmp866, &tmp867, &tmp868, &tmp869, &tmp870, &tmp871, &tmp872, &tmp873, &tmp874);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 320);
    compiler::TNode<BoolT> tmp875;
    USE(tmp875);
    tmp875 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 171);
    compiler::TNode<IntPtrT> tmp876;
    USE(tmp876);
    tmp876 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp877;
    USE(tmp877);
    tmp877 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp868}, compiler::TNode<IntPtrT>{tmp876}));
    ca_.Branch(tmp877, &block38, &block40, tmp858, tmp859, tmp860, tmp861, tmp862, tmp863, tmp864, tmp865, tmp866, tmp867, tmp868, tmp869, tmp870, tmp871, tmp872, tmp873, tmp874, tmp868, tmp867, tmp858, tmp868, tmp867, tmp875);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp878;
    compiler::TNode<JSReceiver> tmp879;
    compiler::TNode<String> tmp880;
    compiler::TNode<Number> tmp881;
    compiler::TNode<Object> tmp882;
    compiler::TNode<Object> tmp883;
    compiler::TNode<BuiltinPtr> tmp884;
    compiler::TNode<Map> tmp885;
    compiler::TNode<UintPtrT> tmp886;
    compiler::TNode<IntPtrT> tmp887;
    compiler::TNode<IntPtrT> tmp888;
    compiler::TNode<BuiltinPtr> tmp889;
    compiler::TNode<FixedArray> tmp890;
    compiler::TNode<IntPtrT> tmp891;
    compiler::TNode<IntPtrT> tmp892;
    compiler::TNode<BoolT> tmp893;
    compiler::TNode<UintPtrT> tmp894;
    compiler::TNode<IntPtrT> tmp895;
    compiler::TNode<IntPtrT> tmp896;
    compiler::TNode<Context> tmp897;
    compiler::TNode<IntPtrT> tmp898;
    compiler::TNode<IntPtrT> tmp899;
    compiler::TNode<BoolT> tmp900;
    ca_.Bind(&block40, &tmp878, &tmp879, &tmp880, &tmp881, &tmp882, &tmp883, &tmp884, &tmp885, &tmp886, &tmp887, &tmp888, &tmp889, &tmp890, &tmp891, &tmp892, &tmp893, &tmp894, &tmp895, &tmp896, &tmp897, &tmp898, &tmp899, &tmp900);
    compiler::TNode<IntPtrT> tmp901;
    USE(tmp901);
    tmp901 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp902;
    USE(tmp902);
    tmp902 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp899}, compiler::TNode<IntPtrT>{tmp901}));
    ca_.Branch(tmp902, &block38, &block39, tmp878, tmp879, tmp880, tmp881, tmp882, tmp883, tmp884, tmp885, tmp886, tmp887, tmp888, tmp889, tmp890, tmp891, tmp892, tmp893, tmp894, tmp895, tmp896, tmp897, tmp898, tmp899, tmp900);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp903;
    compiler::TNode<JSReceiver> tmp904;
    compiler::TNode<String> tmp905;
    compiler::TNode<Number> tmp906;
    compiler::TNode<Object> tmp907;
    compiler::TNode<Object> tmp908;
    compiler::TNode<BuiltinPtr> tmp909;
    compiler::TNode<Map> tmp910;
    compiler::TNode<UintPtrT> tmp911;
    compiler::TNode<IntPtrT> tmp912;
    compiler::TNode<IntPtrT> tmp913;
    compiler::TNode<BuiltinPtr> tmp914;
    compiler::TNode<FixedArray> tmp915;
    compiler::TNode<IntPtrT> tmp916;
    compiler::TNode<IntPtrT> tmp917;
    compiler::TNode<BoolT> tmp918;
    compiler::TNode<UintPtrT> tmp919;
    compiler::TNode<IntPtrT> tmp920;
    compiler::TNode<IntPtrT> tmp921;
    compiler::TNode<Context> tmp922;
    compiler::TNode<IntPtrT> tmp923;
    compiler::TNode<IntPtrT> tmp924;
    compiler::TNode<BoolT> tmp925;
    ca_.Bind(&block38, &tmp903, &tmp904, &tmp905, &tmp906, &tmp907, &tmp908, &tmp909, &tmp910, &tmp911, &tmp912, &tmp913, &tmp914, &tmp915, &tmp916, &tmp917, &tmp918, &tmp919, &tmp920, &tmp921, &tmp922, &tmp923, &tmp924, &tmp925);
    ca_.Goto(&block37, tmp903, tmp904, tmp905, tmp906, tmp907, tmp908, tmp909, tmp910, tmp911, tmp912, tmp913, tmp914, tmp915, tmp916, tmp917, tmp918, tmp919, tmp920, tmp921, tmp922, tmp923, tmp924, tmp925);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp926;
    compiler::TNode<JSReceiver> tmp927;
    compiler::TNode<String> tmp928;
    compiler::TNode<Number> tmp929;
    compiler::TNode<Object> tmp930;
    compiler::TNode<Object> tmp931;
    compiler::TNode<BuiltinPtr> tmp932;
    compiler::TNode<Map> tmp933;
    compiler::TNode<UintPtrT> tmp934;
    compiler::TNode<IntPtrT> tmp935;
    compiler::TNode<IntPtrT> tmp936;
    compiler::TNode<BuiltinPtr> tmp937;
    compiler::TNode<FixedArray> tmp938;
    compiler::TNode<IntPtrT> tmp939;
    compiler::TNode<IntPtrT> tmp940;
    compiler::TNode<BoolT> tmp941;
    compiler::TNode<UintPtrT> tmp942;
    compiler::TNode<IntPtrT> tmp943;
    compiler::TNode<IntPtrT> tmp944;
    compiler::TNode<Context> tmp945;
    compiler::TNode<IntPtrT> tmp946;
    compiler::TNode<IntPtrT> tmp947;
    compiler::TNode<BoolT> tmp948;
    ca_.Bind(&block39, &tmp926, &tmp927, &tmp928, &tmp929, &tmp930, &tmp931, &tmp932, &tmp933, &tmp934, &tmp935, &tmp936, &tmp937, &tmp938, &tmp939, &tmp940, &tmp941, &tmp942, &tmp943, &tmp944, &tmp945, &tmp946, &tmp947, &tmp948);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 173);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 174);
    compiler::TNode<IntPtrT> tmp949;
    USE(tmp949);
    tmp949 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMul(compiler::TNode<IntPtrT>{tmp947}, compiler::TNode<IntPtrT>{tmp946}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 177);
    compiler::TNode<IntPtrT> tmp950;
    USE(tmp950);
    tmp950 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrDiv(compiler::TNode<IntPtrT>{tmp949}, compiler::TNode<IntPtrT>{tmp947}));
    compiler::TNode<BoolT> tmp951;
    USE(tmp951);
    tmp951 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<IntPtrT>{tmp950}, compiler::TNode<IntPtrT>{tmp946}));
    ca_.Branch(tmp951, &block41, &block42, tmp926, tmp927, tmp928, tmp929, tmp930, tmp931, tmp932, tmp933, tmp934, tmp935, tmp936, tmp937, tmp938, tmp939, tmp940, tmp941, tmp942, tmp943, tmp944, tmp945, tmp946, tmp947, tmp948, tmp946, tmp949);
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp952;
    compiler::TNode<JSReceiver> tmp953;
    compiler::TNode<String> tmp954;
    compiler::TNode<Number> tmp955;
    compiler::TNode<Object> tmp956;
    compiler::TNode<Object> tmp957;
    compiler::TNode<BuiltinPtr> tmp958;
    compiler::TNode<Map> tmp959;
    compiler::TNode<UintPtrT> tmp960;
    compiler::TNode<IntPtrT> tmp961;
    compiler::TNode<IntPtrT> tmp962;
    compiler::TNode<BuiltinPtr> tmp963;
    compiler::TNode<FixedArray> tmp964;
    compiler::TNode<IntPtrT> tmp965;
    compiler::TNode<IntPtrT> tmp966;
    compiler::TNode<BoolT> tmp967;
    compiler::TNode<UintPtrT> tmp968;
    compiler::TNode<IntPtrT> tmp969;
    compiler::TNode<IntPtrT> tmp970;
    compiler::TNode<Context> tmp971;
    compiler::TNode<IntPtrT> tmp972;
    compiler::TNode<IntPtrT> tmp973;
    compiler::TNode<BoolT> tmp974;
    compiler::TNode<IntPtrT> tmp975;
    compiler::TNode<IntPtrT> tmp976;
    ca_.Bind(&block41, &tmp952, &tmp953, &tmp954, &tmp955, &tmp956, &tmp957, &tmp958, &tmp959, &tmp960, &tmp961, &tmp962, &tmp963, &tmp964, &tmp965, &tmp966, &tmp967, &tmp968, &tmp969, &tmp970, &tmp971, &tmp972, &tmp973, &tmp974, &tmp975, &tmp976);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 178);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp971);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp978;
    compiler::TNode<JSReceiver> tmp979;
    compiler::TNode<String> tmp980;
    compiler::TNode<Number> tmp981;
    compiler::TNode<Object> tmp982;
    compiler::TNode<Object> tmp983;
    compiler::TNode<BuiltinPtr> tmp984;
    compiler::TNode<Map> tmp985;
    compiler::TNode<UintPtrT> tmp986;
    compiler::TNode<IntPtrT> tmp987;
    compiler::TNode<IntPtrT> tmp988;
    compiler::TNode<BuiltinPtr> tmp989;
    compiler::TNode<FixedArray> tmp990;
    compiler::TNode<IntPtrT> tmp991;
    compiler::TNode<IntPtrT> tmp992;
    compiler::TNode<BoolT> tmp993;
    compiler::TNode<UintPtrT> tmp994;
    compiler::TNode<IntPtrT> tmp995;
    compiler::TNode<IntPtrT> tmp996;
    compiler::TNode<Context> tmp997;
    compiler::TNode<IntPtrT> tmp998;
    compiler::TNode<IntPtrT> tmp999;
    compiler::TNode<BoolT> tmp1000;
    compiler::TNode<IntPtrT> tmp1001;
    compiler::TNode<IntPtrT> tmp1002;
    ca_.Bind(&block42, &tmp978, &tmp979, &tmp980, &tmp981, &tmp982, &tmp983, &tmp984, &tmp985, &tmp986, &tmp987, &tmp988, &tmp989, &tmp990, &tmp991, &tmp992, &tmp993, &tmp994, &tmp995, &tmp996, &tmp997, &tmp998, &tmp999, &tmp1000, &tmp1001, &tmp1002);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 181);
    compiler::TNode<IntPtrT> tmp1003;
    USE(tmp1003);
    tmp1003 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp997}, compiler::TNode<IntPtrT>{tmp992}, compiler::TNode<IntPtrT>{tmp1002}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 182);
    ca_.Branch(tmp1000, &block43, &block44, tmp978, tmp979, tmp980, tmp981, tmp982, tmp983, tmp984, tmp985, tmp986, tmp987, tmp988, tmp989, tmp990, tmp991, tmp1003, tmp993, tmp994, tmp995, tmp996, tmp997, tmp998, tmp999, tmp1000, tmp1001, tmp1002);
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp1004;
    compiler::TNode<JSReceiver> tmp1005;
    compiler::TNode<String> tmp1006;
    compiler::TNode<Number> tmp1007;
    compiler::TNode<Object> tmp1008;
    compiler::TNode<Object> tmp1009;
    compiler::TNode<BuiltinPtr> tmp1010;
    compiler::TNode<Map> tmp1011;
    compiler::TNode<UintPtrT> tmp1012;
    compiler::TNode<IntPtrT> tmp1013;
    compiler::TNode<IntPtrT> tmp1014;
    compiler::TNode<BuiltinPtr> tmp1015;
    compiler::TNode<FixedArray> tmp1016;
    compiler::TNode<IntPtrT> tmp1017;
    compiler::TNode<IntPtrT> tmp1018;
    compiler::TNode<BoolT> tmp1019;
    compiler::TNode<UintPtrT> tmp1020;
    compiler::TNode<IntPtrT> tmp1021;
    compiler::TNode<IntPtrT> tmp1022;
    compiler::TNode<Context> tmp1023;
    compiler::TNode<IntPtrT> tmp1024;
    compiler::TNode<IntPtrT> tmp1025;
    compiler::TNode<BoolT> tmp1026;
    compiler::TNode<IntPtrT> tmp1027;
    compiler::TNode<IntPtrT> tmp1028;
    ca_.Bind(&block43, &tmp1004, &tmp1005, &tmp1006, &tmp1007, &tmp1008, &tmp1009, &tmp1010, &tmp1011, &tmp1012, &tmp1013, &tmp1014, &tmp1015, &tmp1016, &tmp1017, &tmp1018, &tmp1019, &tmp1020, &tmp1021, &tmp1022, &tmp1023, &tmp1024, &tmp1025, &tmp1026, &tmp1027, &tmp1028);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 184);
    compiler::TNode<IntPtrT> tmp1029;
    USE(tmp1029);
    tmp1029 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp1030;
    USE(tmp1030);
    tmp1030 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp1017}, compiler::TNode<IntPtrT>{tmp1029}));
    compiler::TNode<Smi> tmp1031;
    USE(tmp1031);
    tmp1031 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp1027}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 183);
    compiler::TNode<FixedArray> tmp1032;
    USE(tmp1032);
    tmp1032 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray>{tmp1016}, compiler::TNode<IntPtrT>{tmp1017}, compiler::TNode<Smi>{tmp1031}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 182);
    ca_.Goto(&block44, tmp1004, tmp1005, tmp1006, tmp1007, tmp1008, tmp1009, tmp1010, tmp1011, tmp1012, tmp1013, tmp1014, tmp1015, tmp1032, tmp1030, tmp1018, tmp1019, tmp1020, tmp1021, tmp1022, tmp1023, tmp1024, tmp1025, tmp1026, tmp1027, tmp1028);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp1033;
    compiler::TNode<JSReceiver> tmp1034;
    compiler::TNode<String> tmp1035;
    compiler::TNode<Number> tmp1036;
    compiler::TNode<Object> tmp1037;
    compiler::TNode<Object> tmp1038;
    compiler::TNode<BuiltinPtr> tmp1039;
    compiler::TNode<Map> tmp1040;
    compiler::TNode<UintPtrT> tmp1041;
    compiler::TNode<IntPtrT> tmp1042;
    compiler::TNode<IntPtrT> tmp1043;
    compiler::TNode<BuiltinPtr> tmp1044;
    compiler::TNode<FixedArray> tmp1045;
    compiler::TNode<IntPtrT> tmp1046;
    compiler::TNode<IntPtrT> tmp1047;
    compiler::TNode<BoolT> tmp1048;
    compiler::TNode<UintPtrT> tmp1049;
    compiler::TNode<IntPtrT> tmp1050;
    compiler::TNode<IntPtrT> tmp1051;
    compiler::TNode<Context> tmp1052;
    compiler::TNode<IntPtrT> tmp1053;
    compiler::TNode<IntPtrT> tmp1054;
    compiler::TNode<BoolT> tmp1055;
    compiler::TNode<IntPtrT> tmp1056;
    compiler::TNode<IntPtrT> tmp1057;
    ca_.Bind(&block44, &tmp1033, &tmp1034, &tmp1035, &tmp1036, &tmp1037, &tmp1038, &tmp1039, &tmp1040, &tmp1041, &tmp1042, &tmp1043, &tmp1044, &tmp1045, &tmp1046, &tmp1047, &tmp1048, &tmp1049, &tmp1050, &tmp1051, &tmp1052, &tmp1053, &tmp1054, &tmp1055, &tmp1056, &tmp1057);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 170);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 320);
    ca_.Goto(&block37, tmp1033, tmp1034, tmp1035, tmp1036, tmp1037, tmp1038, tmp1039, tmp1040, tmp1041, tmp1042, tmp1043, tmp1044, tmp1045, tmp1046, tmp1047, tmp1048, tmp1049, tmp1050, tmp1051, tmp1052, tmp1053, tmp1054, tmp1055);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp1058;
    compiler::TNode<JSReceiver> tmp1059;
    compiler::TNode<String> tmp1060;
    compiler::TNode<Number> tmp1061;
    compiler::TNode<Object> tmp1062;
    compiler::TNode<Object> tmp1063;
    compiler::TNode<BuiltinPtr> tmp1064;
    compiler::TNode<Map> tmp1065;
    compiler::TNode<UintPtrT> tmp1066;
    compiler::TNode<IntPtrT> tmp1067;
    compiler::TNode<IntPtrT> tmp1068;
    compiler::TNode<BuiltinPtr> tmp1069;
    compiler::TNode<FixedArray> tmp1070;
    compiler::TNode<IntPtrT> tmp1071;
    compiler::TNode<IntPtrT> tmp1072;
    compiler::TNode<BoolT> tmp1073;
    compiler::TNode<UintPtrT> tmp1074;
    compiler::TNode<IntPtrT> tmp1075;
    compiler::TNode<IntPtrT> tmp1076;
    compiler::TNode<Context> tmp1077;
    compiler::TNode<IntPtrT> tmp1078;
    compiler::TNode<IntPtrT> tmp1079;
    compiler::TNode<BoolT> tmp1080;
    ca_.Bind(&block37, &tmp1058, &tmp1059, &tmp1060, &tmp1061, &tmp1062, &tmp1063, &tmp1064, &tmp1065, &tmp1066, &tmp1067, &tmp1068, &tmp1069, &tmp1070, &tmp1071, &tmp1072, &tmp1073, &tmp1074, &tmp1075, &tmp1076, &tmp1077, &tmp1078, &tmp1079, &tmp1080);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 323);
    compiler::TNode<String> tmp1081;
    USE(tmp1081);
    tmp1081 = ca_.UncheckedCast<String>(ArrayJoinBuiltinsFromDSLAssembler(state_).BufferJoin(compiler::TNode<Context>{tmp1058}, ArrayJoinBuiltinsFromDSLAssembler::Buffer{compiler::TNode<FixedArray>{tmp1070}, compiler::TNode<IntPtrT>{tmp1071}, compiler::TNode<IntPtrT>{tmp1072}, compiler::TNode<BoolT>{tmp1073}}, compiler::TNode<String>{tmp1060}));
    ca_.Goto(&block1, tmp1058, tmp1059, tmp1060, tmp1061, tmp1062, tmp1063, tmp1064, tmp1081);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp1082;
    compiler::TNode<JSReceiver> tmp1083;
    compiler::TNode<String> tmp1084;
    compiler::TNode<Number> tmp1085;
    compiler::TNode<Object> tmp1086;
    compiler::TNode<Object> tmp1087;
    compiler::TNode<BuiltinPtr> tmp1088;
    compiler::TNode<String> tmp1089;
    ca_.Bind(&block1, &tmp1082, &tmp1083, &tmp1084, &tmp1085, &tmp1086, &tmp1087, &tmp1088, &tmp1089);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 262);
    ca_.Goto(&block45, tmp1082, tmp1083, tmp1084, tmp1085, tmp1086, tmp1087, tmp1088, tmp1089);
  }

    compiler::TNode<Context> tmp1090;
    compiler::TNode<JSReceiver> tmp1091;
    compiler::TNode<String> tmp1092;
    compiler::TNode<Number> tmp1093;
    compiler::TNode<Object> tmp1094;
    compiler::TNode<Object> tmp1095;
    compiler::TNode<BuiltinPtr> tmp1096;
    compiler::TNode<String> tmp1097;
    ca_.Bind(&block45, &tmp1090, &tmp1091, &tmp1092, &tmp1093, &tmp1094, &tmp1095, &tmp1096, &tmp1097);
  return compiler::TNode<String>{tmp1097};
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt32Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat32Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType19ATFixedFloat64Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType24ATFixedUint8ClampedArray()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType21ATFixedBigUint64Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType20ATFixedBigInt64Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedUint8Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType16ATFixedInt8Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint16Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType17ATFixedInt16Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 55);
    compiler::TNode<JSTypedArray> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSTypedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 58);
    compiler::TNode<RawPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp3}));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 57);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedTypedArrayElementAsTagged(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<Smi>{tmp5}, (TypedArrayBuiltinsFromDSLAssembler(state_).KindForArrayType18ATFixedUint32Array()), CodeStubAssembler::SMI_PARAMETERS));
    CodeStubAssembler(state_).Return(tmp6);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 266);
    compiler::TNode<IntPtrT> tmp7 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp7);
    compiler::TNode<Map>tmp8 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp1, tmp7});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 267);
    compiler::TNode<UintPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr20UT5ATSmi10HeapNumber(compiler::TNode<Number>{tmp3}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 268);
    compiler::TNode<IntPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadStringLengthAsWord(compiler::TNode<String>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 269);
    compiler::TNode<IntPtrT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 270);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 271);
    compiler::TNode<FixedArray> tmp12;
    USE(tmp12);
    compiler::TNode<IntPtrT> tmp13;
    USE(tmp13);
    compiler::TNode<IntPtrT> tmp14;
    USE(tmp14);
    compiler::TNode<BoolT> tmp15;
    USE(tmp15);
    std::tie(tmp12, tmp13, tmp14, tmp15) = ArrayJoinBuiltinsFromDSLAssembler(state_).NewBuffer(compiler::TNode<UintPtrT>{tmp9}, compiler::TNode<String>{tmp2}).Flatten();
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 274);
    compiler::TNode<UintPtrT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 277);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp8, tmp9, tmp10, tmp11, tmp6, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSReceiver> tmp18;
    compiler::TNode<String> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<BuiltinPtr> tmp23;
    compiler::TNode<Map> tmp24;
    compiler::TNode<UintPtrT> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    compiler::TNode<BuiltinPtr> tmp28;
    compiler::TNode<FixedArray> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<UintPtrT> tmp33;
    ca_.Bind(&block4, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    compiler::TNode<BoolT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrLessThan(compiler::TNode<UintPtrT>{tmp33}, compiler::TNode<UintPtrT>{tmp25}));
    ca_.Branch(tmp34, &block2, &block3, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<JSReceiver> tmp36;
    compiler::TNode<String> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<BuiltinPtr> tmp41;
    compiler::TNode<Map> tmp42;
    compiler::TNode<UintPtrT> tmp43;
    compiler::TNode<IntPtrT> tmp44;
    compiler::TNode<IntPtrT> tmp45;
    compiler::TNode<BuiltinPtr> tmp46;
    compiler::TNode<FixedArray> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    compiler::TNode<BoolT> tmp50;
    compiler::TNode<UintPtrT> tmp51;
    ca_.Bind(&block2, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 279);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 278);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    ArrayJoinBuiltinsFromDSLAssembler(state_).CannotUseSameArrayAccessor12JSTypedArray(compiler::TNode<Context>{tmp35}, compiler::TNode<BuiltinPtr>{tmp46}, compiler::TNode<JSReceiver>{tmp36}, compiler::TNode<Map>{tmp42}, compiler::TNode<Number>{tmp38}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp46, tmp36, tmp42, tmp38);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block8, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp46, tmp36, tmp42, tmp38);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    compiler::TNode<String> tmp54;
    compiler::TNode<Number> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<BuiltinPtr> tmp58;
    compiler::TNode<Map> tmp59;
    compiler::TNode<UintPtrT> tmp60;
    compiler::TNode<IntPtrT> tmp61;
    compiler::TNode<IntPtrT> tmp62;
    compiler::TNode<BuiltinPtr> tmp63;
    compiler::TNode<FixedArray> tmp64;
    compiler::TNode<IntPtrT> tmp65;
    compiler::TNode<IntPtrT> tmp66;
    compiler::TNode<BoolT> tmp67;
    compiler::TNode<UintPtrT> tmp68;
    compiler::TNode<BuiltinPtr> tmp69;
    compiler::TNode<JSReceiver> tmp70;
    compiler::TNode<Map> tmp71;
    compiler::TNode<Number> tmp72;
    ca_.Bind(&block7, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.Goto(&block5, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    compiler::TNode<String> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<BuiltinPtr> tmp79;
    compiler::TNode<Map> tmp80;
    compiler::TNode<UintPtrT> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    compiler::TNode<IntPtrT> tmp83;
    compiler::TNode<BuiltinPtr> tmp84;
    compiler::TNode<FixedArray> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<IntPtrT> tmp87;
    compiler::TNode<BoolT> tmp88;
    compiler::TNode<UintPtrT> tmp89;
    compiler::TNode<BuiltinPtr> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<Map> tmp92;
    compiler::TNode<Number> tmp93;
    ca_.Bind(&block8, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93);
    ca_.Goto(&block6, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp94;
    compiler::TNode<JSReceiver> tmp95;
    compiler::TNode<String> tmp96;
    compiler::TNode<Number> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<Object> tmp99;
    compiler::TNode<BuiltinPtr> tmp100;
    compiler::TNode<Map> tmp101;
    compiler::TNode<UintPtrT> tmp102;
    compiler::TNode<IntPtrT> tmp103;
    compiler::TNode<IntPtrT> tmp104;
    compiler::TNode<BuiltinPtr> tmp105;
    compiler::TNode<FixedArray> tmp106;
    compiler::TNode<IntPtrT> tmp107;
    compiler::TNode<IntPtrT> tmp108;
    compiler::TNode<BoolT> tmp109;
    compiler::TNode<UintPtrT> tmp110;
    ca_.Bind(&block5, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 281);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 278);
    ca_.Goto(&block6, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, ca_.UncheckedCast<BuiltinPtr>(ca_.SmiConstant(Builtins::kLoadJoinElement25ATGenericElementsAccessor)), tmp106, tmp107, tmp108, tmp109, tmp110);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp111;
    compiler::TNode<JSReceiver> tmp112;
    compiler::TNode<String> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<Object> tmp115;
    compiler::TNode<Object> tmp116;
    compiler::TNode<BuiltinPtr> tmp117;
    compiler::TNode<Map> tmp118;
    compiler::TNode<UintPtrT> tmp119;
    compiler::TNode<IntPtrT> tmp120;
    compiler::TNode<IntPtrT> tmp121;
    compiler::TNode<BuiltinPtr> tmp122;
    compiler::TNode<FixedArray> tmp123;
    compiler::TNode<IntPtrT> tmp124;
    compiler::TNode<IntPtrT> tmp125;
    compiler::TNode<BoolT> tmp126;
    compiler::TNode<UintPtrT> tmp127;
    ca_.Bind(&block6, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 284);
    compiler::TNode<UintPtrT> tmp128;
    USE(tmp128);
    tmp128 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp129;
    USE(tmp129);
    tmp129 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrGreaterThan(compiler::TNode<UintPtrT>{tmp127}, compiler::TNode<UintPtrT>{tmp128}));
    ca_.Branch(tmp129, &block9, &block10, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp130;
    compiler::TNode<JSReceiver> tmp131;
    compiler::TNode<String> tmp132;
    compiler::TNode<Number> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<BuiltinPtr> tmp136;
    compiler::TNode<Map> tmp137;
    compiler::TNode<UintPtrT> tmp138;
    compiler::TNode<IntPtrT> tmp139;
    compiler::TNode<IntPtrT> tmp140;
    compiler::TNode<BuiltinPtr> tmp141;
    compiler::TNode<FixedArray> tmp142;
    compiler::TNode<IntPtrT> tmp143;
    compiler::TNode<IntPtrT> tmp144;
    compiler::TNode<BoolT> tmp145;
    compiler::TNode<UintPtrT> tmp146;
    ca_.Bind(&block9, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 286);
    compiler::TNode<IntPtrT> tmp147;
    USE(tmp147);
    tmp147 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp148;
    USE(tmp148);
    tmp148 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp140}, compiler::TNode<IntPtrT>{tmp147}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 284);
    ca_.Goto(&block10, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp148, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp149;
    compiler::TNode<JSReceiver> tmp150;
    compiler::TNode<String> tmp151;
    compiler::TNode<Number> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<BuiltinPtr> tmp155;
    compiler::TNode<Map> tmp156;
    compiler::TNode<UintPtrT> tmp157;
    compiler::TNode<IntPtrT> tmp158;
    compiler::TNode<IntPtrT> tmp159;
    compiler::TNode<BuiltinPtr> tmp160;
    compiler::TNode<FixedArray> tmp161;
    compiler::TNode<IntPtrT> tmp162;
    compiler::TNode<IntPtrT> tmp163;
    compiler::TNode<BoolT> tmp164;
    compiler::TNode<UintPtrT> tmp165;
    ca_.Bind(&block10, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 290);
    compiler::TNode<UintPtrT> tmp166;
    USE(tmp166);
    tmp166 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp165}, compiler::TNode<UintPtrT>{tmp166}));
    compiler::TNode<Number> tmp168;
    USE(tmp168);
    tmp168 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATuintptr(compiler::TNode<UintPtrT>{tmp165}));
    compiler::TNode<Object> tmp169 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(0)).descriptor(), tmp160, tmp149, tmp150, tmp168); 
    USE(tmp169);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 294);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 295);
    if ((p_useToLocaleString)) {
      ca_.Goto(&block11, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp167, tmp169, ca_.Uninitialized<String>());
    } else {
      ca_.Goto(&block12, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp167, tmp169, ca_.Uninitialized<String>());
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<JSReceiver> tmp171;
    compiler::TNode<String> tmp172;
    compiler::TNode<Number> tmp173;
    compiler::TNode<Object> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<BuiltinPtr> tmp176;
    compiler::TNode<Map> tmp177;
    compiler::TNode<UintPtrT> tmp178;
    compiler::TNode<IntPtrT> tmp179;
    compiler::TNode<IntPtrT> tmp180;
    compiler::TNode<BuiltinPtr> tmp181;
    compiler::TNode<FixedArray> tmp182;
    compiler::TNode<IntPtrT> tmp183;
    compiler::TNode<IntPtrT> tmp184;
    compiler::TNode<BoolT> tmp185;
    compiler::TNode<UintPtrT> tmp186;
    compiler::TNode<Object> tmp187;
    compiler::TNode<String> tmp188;
    ca_.Bind(&block11, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 296);
    compiler::TNode<String> tmp189;
    tmp189 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kConvertToLocaleString, tmp170, tmp187, tmp174, tmp175));
    USE(tmp189);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 297);
    compiler::TNode<String> tmp190;
    USE(tmp190);
    tmp190 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp191;
    USE(tmp191);
    tmp191 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp189}, compiler::TNode<HeapObject>{tmp190}));
    ca_.Branch(tmp191, &block14, &block15, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp189);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp192;
    compiler::TNode<JSReceiver> tmp193;
    compiler::TNode<String> tmp194;
    compiler::TNode<Number> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<Object> tmp197;
    compiler::TNode<BuiltinPtr> tmp198;
    compiler::TNode<Map> tmp199;
    compiler::TNode<UintPtrT> tmp200;
    compiler::TNode<IntPtrT> tmp201;
    compiler::TNode<IntPtrT> tmp202;
    compiler::TNode<BuiltinPtr> tmp203;
    compiler::TNode<FixedArray> tmp204;
    compiler::TNode<IntPtrT> tmp205;
    compiler::TNode<IntPtrT> tmp206;
    compiler::TNode<BoolT> tmp207;
    compiler::TNode<UintPtrT> tmp208;
    compiler::TNode<Object> tmp209;
    compiler::TNode<String> tmp210;
    ca_.Bind(&block14, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210);
    ca_.Goto(&block4, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp211;
    compiler::TNode<JSReceiver> tmp212;
    compiler::TNode<String> tmp213;
    compiler::TNode<Number> tmp214;
    compiler::TNode<Object> tmp215;
    compiler::TNode<Object> tmp216;
    compiler::TNode<BuiltinPtr> tmp217;
    compiler::TNode<Map> tmp218;
    compiler::TNode<UintPtrT> tmp219;
    compiler::TNode<IntPtrT> tmp220;
    compiler::TNode<IntPtrT> tmp221;
    compiler::TNode<BuiltinPtr> tmp222;
    compiler::TNode<FixedArray> tmp223;
    compiler::TNode<IntPtrT> tmp224;
    compiler::TNode<IntPtrT> tmp225;
    compiler::TNode<BoolT> tmp226;
    compiler::TNode<UintPtrT> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<String> tmp229;
    ca_.Bind(&block15, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 295);
    ca_.Goto(&block13, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp230;
    compiler::TNode<JSReceiver> tmp231;
    compiler::TNode<String> tmp232;
    compiler::TNode<Number> tmp233;
    compiler::TNode<Object> tmp234;
    compiler::TNode<Object> tmp235;
    compiler::TNode<BuiltinPtr> tmp236;
    compiler::TNode<Map> tmp237;
    compiler::TNode<UintPtrT> tmp238;
    compiler::TNode<IntPtrT> tmp239;
    compiler::TNode<IntPtrT> tmp240;
    compiler::TNode<BuiltinPtr> tmp241;
    compiler::TNode<FixedArray> tmp242;
    compiler::TNode<IntPtrT> tmp243;
    compiler::TNode<IntPtrT> tmp244;
    compiler::TNode<BoolT> tmp245;
    compiler::TNode<UintPtrT> tmp246;
    compiler::TNode<Object> tmp247;
    compiler::TNode<String> tmp248;
    ca_.Bind(&block12, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 299);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 300);
    compiler::TNode<String> tmp249;
    USE(tmp249);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp249 = BaseBuiltinsFromDSLAssembler(state_).Cast6String(compiler::TNode<Context>{tmp230}, compiler::TNode<Object>{tmp247}, &label0);
    ca_.Goto(&block18, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp247, tmp247, tmp249);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block19, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp247, tmp247);
    }
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp250;
    compiler::TNode<JSReceiver> tmp251;
    compiler::TNode<String> tmp252;
    compiler::TNode<Number> tmp253;
    compiler::TNode<Object> tmp254;
    compiler::TNode<Object> tmp255;
    compiler::TNode<BuiltinPtr> tmp256;
    compiler::TNode<Map> tmp257;
    compiler::TNode<UintPtrT> tmp258;
    compiler::TNode<IntPtrT> tmp259;
    compiler::TNode<IntPtrT> tmp260;
    compiler::TNode<BuiltinPtr> tmp261;
    compiler::TNode<FixedArray> tmp262;
    compiler::TNode<IntPtrT> tmp263;
    compiler::TNode<IntPtrT> tmp264;
    compiler::TNode<BoolT> tmp265;
    compiler::TNode<UintPtrT> tmp266;
    compiler::TNode<Object> tmp267;
    compiler::TNode<String> tmp268;
    compiler::TNode<Object> tmp269;
    compiler::TNode<Object> tmp270;
    ca_.Bind(&block19, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270);
    ca_.Goto(&block17, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp271;
    compiler::TNode<JSReceiver> tmp272;
    compiler::TNode<String> tmp273;
    compiler::TNode<Number> tmp274;
    compiler::TNode<Object> tmp275;
    compiler::TNode<Object> tmp276;
    compiler::TNode<BuiltinPtr> tmp277;
    compiler::TNode<Map> tmp278;
    compiler::TNode<UintPtrT> tmp279;
    compiler::TNode<IntPtrT> tmp280;
    compiler::TNode<IntPtrT> tmp281;
    compiler::TNode<BuiltinPtr> tmp282;
    compiler::TNode<FixedArray> tmp283;
    compiler::TNode<IntPtrT> tmp284;
    compiler::TNode<IntPtrT> tmp285;
    compiler::TNode<BoolT> tmp286;
    compiler::TNode<UintPtrT> tmp287;
    compiler::TNode<Object> tmp288;
    compiler::TNode<String> tmp289;
    compiler::TNode<Object> tmp290;
    compiler::TNode<Object> tmp291;
    compiler::TNode<String> tmp292;
    ca_.Bind(&block18, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 301);
    compiler::TNode<String> tmp293;
    USE(tmp293);
    tmp293 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp294;
    USE(tmp294);
    tmp294 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp292}, compiler::TNode<HeapObject>{tmp293}));
    ca_.Branch(tmp294, &block20, &block21, tmp271, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp292);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp295;
    compiler::TNode<JSReceiver> tmp296;
    compiler::TNode<String> tmp297;
    compiler::TNode<Number> tmp298;
    compiler::TNode<Object> tmp299;
    compiler::TNode<Object> tmp300;
    compiler::TNode<BuiltinPtr> tmp301;
    compiler::TNode<Map> tmp302;
    compiler::TNode<UintPtrT> tmp303;
    compiler::TNode<IntPtrT> tmp304;
    compiler::TNode<IntPtrT> tmp305;
    compiler::TNode<BuiltinPtr> tmp306;
    compiler::TNode<FixedArray> tmp307;
    compiler::TNode<IntPtrT> tmp308;
    compiler::TNode<IntPtrT> tmp309;
    compiler::TNode<BoolT> tmp310;
    compiler::TNode<UintPtrT> tmp311;
    compiler::TNode<Object> tmp312;
    compiler::TNode<String> tmp313;
    compiler::TNode<Object> tmp314;
    compiler::TNode<String> tmp315;
    ca_.Bind(&block20, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315);
    ca_.Goto(&block4, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp316;
    compiler::TNode<JSReceiver> tmp317;
    compiler::TNode<String> tmp318;
    compiler::TNode<Number> tmp319;
    compiler::TNode<Object> tmp320;
    compiler::TNode<Object> tmp321;
    compiler::TNode<BuiltinPtr> tmp322;
    compiler::TNode<Map> tmp323;
    compiler::TNode<UintPtrT> tmp324;
    compiler::TNode<IntPtrT> tmp325;
    compiler::TNode<IntPtrT> tmp326;
    compiler::TNode<BuiltinPtr> tmp327;
    compiler::TNode<FixedArray> tmp328;
    compiler::TNode<IntPtrT> tmp329;
    compiler::TNode<IntPtrT> tmp330;
    compiler::TNode<BoolT> tmp331;
    compiler::TNode<UintPtrT> tmp332;
    compiler::TNode<Object> tmp333;
    compiler::TNode<String> tmp334;
    compiler::TNode<Object> tmp335;
    compiler::TNode<String> tmp336;
    ca_.Bind(&block21, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 302);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 300);
    ca_.Goto(&block16, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp333, tmp336, tmp335);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp337;
    compiler::TNode<JSReceiver> tmp338;
    compiler::TNode<String> tmp339;
    compiler::TNode<Number> tmp340;
    compiler::TNode<Object> tmp341;
    compiler::TNode<Object> tmp342;
    compiler::TNode<BuiltinPtr> tmp343;
    compiler::TNode<Map> tmp344;
    compiler::TNode<UintPtrT> tmp345;
    compiler::TNode<IntPtrT> tmp346;
    compiler::TNode<IntPtrT> tmp347;
    compiler::TNode<BuiltinPtr> tmp348;
    compiler::TNode<FixedArray> tmp349;
    compiler::TNode<IntPtrT> tmp350;
    compiler::TNode<IntPtrT> tmp351;
    compiler::TNode<BoolT> tmp352;
    compiler::TNode<UintPtrT> tmp353;
    compiler::TNode<Object> tmp354;
    compiler::TNode<String> tmp355;
    compiler::TNode<Object> tmp356;
    ca_.Bind(&block17, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 304);
    compiler::TNode<Number> tmp357;
    USE(tmp357);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp357 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp356)}, &label0);
    ca_.Goto(&block24, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, ca_.UncheckedCast<Object>(tmp356), tmp357);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block25, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, ca_.UncheckedCast<Object>(tmp356));
    }
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp358;
    compiler::TNode<JSReceiver> tmp359;
    compiler::TNode<String> tmp360;
    compiler::TNode<Number> tmp361;
    compiler::TNode<Object> tmp362;
    compiler::TNode<Object> tmp363;
    compiler::TNode<BuiltinPtr> tmp364;
    compiler::TNode<Map> tmp365;
    compiler::TNode<UintPtrT> tmp366;
    compiler::TNode<IntPtrT> tmp367;
    compiler::TNode<IntPtrT> tmp368;
    compiler::TNode<BuiltinPtr> tmp369;
    compiler::TNode<FixedArray> tmp370;
    compiler::TNode<IntPtrT> tmp371;
    compiler::TNode<IntPtrT> tmp372;
    compiler::TNode<BoolT> tmp373;
    compiler::TNode<UintPtrT> tmp374;
    compiler::TNode<Object> tmp375;
    compiler::TNode<String> tmp376;
    compiler::TNode<Object> tmp377;
    compiler::TNode<Object> tmp378;
    ca_.Bind(&block25, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378);
    ca_.Goto(&block23, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp379;
    compiler::TNode<JSReceiver> tmp380;
    compiler::TNode<String> tmp381;
    compiler::TNode<Number> tmp382;
    compiler::TNode<Object> tmp383;
    compiler::TNode<Object> tmp384;
    compiler::TNode<BuiltinPtr> tmp385;
    compiler::TNode<Map> tmp386;
    compiler::TNode<UintPtrT> tmp387;
    compiler::TNode<IntPtrT> tmp388;
    compiler::TNode<IntPtrT> tmp389;
    compiler::TNode<BuiltinPtr> tmp390;
    compiler::TNode<FixedArray> tmp391;
    compiler::TNode<IntPtrT> tmp392;
    compiler::TNode<IntPtrT> tmp393;
    compiler::TNode<BoolT> tmp394;
    compiler::TNode<UintPtrT> tmp395;
    compiler::TNode<Object> tmp396;
    compiler::TNode<String> tmp397;
    compiler::TNode<Object> tmp398;
    compiler::TNode<Object> tmp399;
    compiler::TNode<Number> tmp400;
    ca_.Bind(&block24, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 305);
    compiler::TNode<String> tmp401;
    USE(tmp401);
    tmp401 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).NumberToString(compiler::TNode<Number>{tmp400}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 304);
    ca_.Goto(&block22, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp396, tmp401, tmp398);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp402;
    compiler::TNode<JSReceiver> tmp403;
    compiler::TNode<String> tmp404;
    compiler::TNode<Number> tmp405;
    compiler::TNode<Object> tmp406;
    compiler::TNode<Object> tmp407;
    compiler::TNode<BuiltinPtr> tmp408;
    compiler::TNode<Map> tmp409;
    compiler::TNode<UintPtrT> tmp410;
    compiler::TNode<IntPtrT> tmp411;
    compiler::TNode<IntPtrT> tmp412;
    compiler::TNode<BuiltinPtr> tmp413;
    compiler::TNode<FixedArray> tmp414;
    compiler::TNode<IntPtrT> tmp415;
    compiler::TNode<IntPtrT> tmp416;
    compiler::TNode<BoolT> tmp417;
    compiler::TNode<UintPtrT> tmp418;
    compiler::TNode<Object> tmp419;
    compiler::TNode<String> tmp420;
    compiler::TNode<Object> tmp421;
    ca_.Bind(&block23, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420, &tmp421);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 307);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 308);
    compiler::TNode<BoolT> tmp422;
    USE(tmp422);
    tmp422 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{ca_.UncheckedCast<HeapObject>(tmp421)}));
    ca_.Branch(tmp422, &block26, &block27, tmp402, tmp403, tmp404, tmp405, tmp406, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420, tmp421, ca_.UncheckedCast<HeapObject>(tmp421));
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp423;
    compiler::TNode<JSReceiver> tmp424;
    compiler::TNode<String> tmp425;
    compiler::TNode<Number> tmp426;
    compiler::TNode<Object> tmp427;
    compiler::TNode<Object> tmp428;
    compiler::TNode<BuiltinPtr> tmp429;
    compiler::TNode<Map> tmp430;
    compiler::TNode<UintPtrT> tmp431;
    compiler::TNode<IntPtrT> tmp432;
    compiler::TNode<IntPtrT> tmp433;
    compiler::TNode<BuiltinPtr> tmp434;
    compiler::TNode<FixedArray> tmp435;
    compiler::TNode<IntPtrT> tmp436;
    compiler::TNode<IntPtrT> tmp437;
    compiler::TNode<BoolT> tmp438;
    compiler::TNode<UintPtrT> tmp439;
    compiler::TNode<Object> tmp440;
    compiler::TNode<String> tmp441;
    compiler::TNode<Object> tmp442;
    compiler::TNode<HeapObject> tmp443;
    ca_.Bind(&block26, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442, &tmp443);
    ca_.Goto(&block4, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438, tmp439);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp444;
    compiler::TNode<JSReceiver> tmp445;
    compiler::TNode<String> tmp446;
    compiler::TNode<Number> tmp447;
    compiler::TNode<Object> tmp448;
    compiler::TNode<Object> tmp449;
    compiler::TNode<BuiltinPtr> tmp450;
    compiler::TNode<Map> tmp451;
    compiler::TNode<UintPtrT> tmp452;
    compiler::TNode<IntPtrT> tmp453;
    compiler::TNode<IntPtrT> tmp454;
    compiler::TNode<BuiltinPtr> tmp455;
    compiler::TNode<FixedArray> tmp456;
    compiler::TNode<IntPtrT> tmp457;
    compiler::TNode<IntPtrT> tmp458;
    compiler::TNode<BoolT> tmp459;
    compiler::TNode<UintPtrT> tmp460;
    compiler::TNode<Object> tmp461;
    compiler::TNode<String> tmp462;
    compiler::TNode<Object> tmp463;
    compiler::TNode<HeapObject> tmp464;
    ca_.Bind(&block27, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463, &tmp464);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 309);
    compiler::TNode<String> tmp465;
    tmp465 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kToString, tmp444, tmp464));
    USE(tmp465);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 304);
    ca_.Goto(&block22, tmp444, tmp445, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452, tmp453, tmp454, tmp455, tmp456, tmp457, tmp458, tmp459, tmp460, tmp461, tmp465, tmp463);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp466;
    compiler::TNode<JSReceiver> tmp467;
    compiler::TNode<String> tmp468;
    compiler::TNode<Number> tmp469;
    compiler::TNode<Object> tmp470;
    compiler::TNode<Object> tmp471;
    compiler::TNode<BuiltinPtr> tmp472;
    compiler::TNode<Map> tmp473;
    compiler::TNode<UintPtrT> tmp474;
    compiler::TNode<IntPtrT> tmp475;
    compiler::TNode<IntPtrT> tmp476;
    compiler::TNode<BuiltinPtr> tmp477;
    compiler::TNode<FixedArray> tmp478;
    compiler::TNode<IntPtrT> tmp479;
    compiler::TNode<IntPtrT> tmp480;
    compiler::TNode<BoolT> tmp481;
    compiler::TNode<UintPtrT> tmp482;
    compiler::TNode<Object> tmp483;
    compiler::TNode<String> tmp484;
    compiler::TNode<Object> tmp485;
    ca_.Bind(&block22, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478, &tmp479, &tmp480, &tmp481, &tmp482, &tmp483, &tmp484, &tmp485);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 300);
    ca_.Goto(&block16, tmp466, tmp467, tmp468, tmp469, tmp470, tmp471, tmp472, tmp473, tmp474, tmp475, tmp476, tmp477, tmp478, tmp479, tmp480, tmp481, tmp482, tmp483, tmp484, tmp485);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp486;
    compiler::TNode<JSReceiver> tmp487;
    compiler::TNode<String> tmp488;
    compiler::TNode<Number> tmp489;
    compiler::TNode<Object> tmp490;
    compiler::TNode<Object> tmp491;
    compiler::TNode<BuiltinPtr> tmp492;
    compiler::TNode<Map> tmp493;
    compiler::TNode<UintPtrT> tmp494;
    compiler::TNode<IntPtrT> tmp495;
    compiler::TNode<IntPtrT> tmp496;
    compiler::TNode<BuiltinPtr> tmp497;
    compiler::TNode<FixedArray> tmp498;
    compiler::TNode<IntPtrT> tmp499;
    compiler::TNode<IntPtrT> tmp500;
    compiler::TNode<BoolT> tmp501;
    compiler::TNode<UintPtrT> tmp502;
    compiler::TNode<Object> tmp503;
    compiler::TNode<String> tmp504;
    compiler::TNode<Object> tmp505;
    ca_.Bind(&block16, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501, &tmp502, &tmp503, &tmp504, &tmp505);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 299);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 295);
    ca_.Goto(&block13, tmp486, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492, tmp493, tmp494, tmp495, tmp496, tmp497, tmp498, tmp499, tmp500, tmp501, tmp502, tmp503, tmp504);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp506;
    compiler::TNode<JSReceiver> tmp507;
    compiler::TNode<String> tmp508;
    compiler::TNode<Number> tmp509;
    compiler::TNode<Object> tmp510;
    compiler::TNode<Object> tmp511;
    compiler::TNode<BuiltinPtr> tmp512;
    compiler::TNode<Map> tmp513;
    compiler::TNode<UintPtrT> tmp514;
    compiler::TNode<IntPtrT> tmp515;
    compiler::TNode<IntPtrT> tmp516;
    compiler::TNode<BuiltinPtr> tmp517;
    compiler::TNode<FixedArray> tmp518;
    compiler::TNode<IntPtrT> tmp519;
    compiler::TNode<IntPtrT> tmp520;
    compiler::TNode<BoolT> tmp521;
    compiler::TNode<UintPtrT> tmp522;
    compiler::TNode<Object> tmp523;
    compiler::TNode<String> tmp524;
    ca_.Bind(&block13, &tmp506, &tmp507, &tmp508, &tmp509, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518, &tmp519, &tmp520, &tmp521, &tmp522, &tmp523, &tmp524);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 315);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 158);
    compiler::TNode<IntPtrT> tmp525;
    USE(tmp525);
    tmp525 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp526;
    USE(tmp526);
    tmp526 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp519}, compiler::TNode<IntPtrT>{tmp525}));
    compiler::TNode<IntPtrT> tmp527;
    USE(tmp527);
    tmp527 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp528;
    USE(tmp528);
    tmp528 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp516}, compiler::TNode<IntPtrT>{tmp527}));
    compiler::TNode<BoolT> tmp529;
    USE(tmp529);
    tmp529 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Or(compiler::TNode<BoolT>{tmp526}, compiler::TNode<BoolT>{tmp528}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 159);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 171);
    compiler::TNode<IntPtrT> tmp530;
    USE(tmp530);
    tmp530 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp531;
    USE(tmp531);
    tmp531 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp516}, compiler::TNode<IntPtrT>{tmp530}));
    ca_.Branch(tmp531, &block30, &block32, tmp506, tmp507, tmp508, tmp509, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517, tmp518, tmp519, tmp520, tmp521, tmp522, tmp523, tmp524, tmp524, tmp516, tmp515, tmp506, tmp524, tmp516, tmp515, tmp529, tmp516, tmp515, tmp529, tmp506, tmp516, tmp515, tmp529);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp532;
    compiler::TNode<JSReceiver> tmp533;
    compiler::TNode<String> tmp534;
    compiler::TNode<Number> tmp535;
    compiler::TNode<Object> tmp536;
    compiler::TNode<Object> tmp537;
    compiler::TNode<BuiltinPtr> tmp538;
    compiler::TNode<Map> tmp539;
    compiler::TNode<UintPtrT> tmp540;
    compiler::TNode<IntPtrT> tmp541;
    compiler::TNode<IntPtrT> tmp542;
    compiler::TNode<BuiltinPtr> tmp543;
    compiler::TNode<FixedArray> tmp544;
    compiler::TNode<IntPtrT> tmp545;
    compiler::TNode<IntPtrT> tmp546;
    compiler::TNode<BoolT> tmp547;
    compiler::TNode<UintPtrT> tmp548;
    compiler::TNode<Object> tmp549;
    compiler::TNode<String> tmp550;
    compiler::TNode<String> tmp551;
    compiler::TNode<IntPtrT> tmp552;
    compiler::TNode<IntPtrT> tmp553;
    compiler::TNode<Context> tmp554;
    compiler::TNode<String> tmp555;
    compiler::TNode<IntPtrT> tmp556;
    compiler::TNode<IntPtrT> tmp557;
    compiler::TNode<BoolT> tmp558;
    compiler::TNode<IntPtrT> tmp559;
    compiler::TNode<IntPtrT> tmp560;
    compiler::TNode<BoolT> tmp561;
    compiler::TNode<Context> tmp562;
    compiler::TNode<IntPtrT> tmp563;
    compiler::TNode<IntPtrT> tmp564;
    compiler::TNode<BoolT> tmp565;
    ca_.Bind(&block32, &tmp532, &tmp533, &tmp534, &tmp535, &tmp536, &tmp537, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544, &tmp545, &tmp546, &tmp547, &tmp548, &tmp549, &tmp550, &tmp551, &tmp552, &tmp553, &tmp554, &tmp555, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563, &tmp564, &tmp565);
    compiler::TNode<IntPtrT> tmp566;
    USE(tmp566);
    tmp566 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp567;
    USE(tmp567);
    tmp567 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp564}, compiler::TNode<IntPtrT>{tmp566}));
    ca_.Branch(tmp567, &block30, &block31, tmp532, tmp533, tmp534, tmp535, tmp536, tmp537, tmp538, tmp539, tmp540, tmp541, tmp542, tmp543, tmp544, tmp545, tmp546, tmp547, tmp548, tmp549, tmp550, tmp551, tmp552, tmp553, tmp554, tmp555, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562, tmp563, tmp564, tmp565);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp568;
    compiler::TNode<JSReceiver> tmp569;
    compiler::TNode<String> tmp570;
    compiler::TNode<Number> tmp571;
    compiler::TNode<Object> tmp572;
    compiler::TNode<Object> tmp573;
    compiler::TNode<BuiltinPtr> tmp574;
    compiler::TNode<Map> tmp575;
    compiler::TNode<UintPtrT> tmp576;
    compiler::TNode<IntPtrT> tmp577;
    compiler::TNode<IntPtrT> tmp578;
    compiler::TNode<BuiltinPtr> tmp579;
    compiler::TNode<FixedArray> tmp580;
    compiler::TNode<IntPtrT> tmp581;
    compiler::TNode<IntPtrT> tmp582;
    compiler::TNode<BoolT> tmp583;
    compiler::TNode<UintPtrT> tmp584;
    compiler::TNode<Object> tmp585;
    compiler::TNode<String> tmp586;
    compiler::TNode<String> tmp587;
    compiler::TNode<IntPtrT> tmp588;
    compiler::TNode<IntPtrT> tmp589;
    compiler::TNode<Context> tmp590;
    compiler::TNode<String> tmp591;
    compiler::TNode<IntPtrT> tmp592;
    compiler::TNode<IntPtrT> tmp593;
    compiler::TNode<BoolT> tmp594;
    compiler::TNode<IntPtrT> tmp595;
    compiler::TNode<IntPtrT> tmp596;
    compiler::TNode<BoolT> tmp597;
    compiler::TNode<Context> tmp598;
    compiler::TNode<IntPtrT> tmp599;
    compiler::TNode<IntPtrT> tmp600;
    compiler::TNode<BoolT> tmp601;
    ca_.Bind(&block30, &tmp568, &tmp569, &tmp570, &tmp571, &tmp572, &tmp573, &tmp574, &tmp575, &tmp576, &tmp577, &tmp578, &tmp579, &tmp580, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585, &tmp586, &tmp587, &tmp588, &tmp589, &tmp590, &tmp591, &tmp592, &tmp593, &tmp594, &tmp595, &tmp596, &tmp597, &tmp598, &tmp599, &tmp600, &tmp601);
    ca_.Goto(&block29, tmp568, tmp569, tmp570, tmp571, tmp572, tmp573, tmp574, tmp575, tmp576, tmp577, tmp578, tmp579, tmp580, tmp581, tmp582, tmp583, tmp584, tmp585, tmp586, tmp587, tmp588, tmp589, tmp590, tmp591, tmp592, tmp593, tmp594, tmp595, tmp596, tmp597, tmp598, tmp599, tmp600, tmp601);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp602;
    compiler::TNode<JSReceiver> tmp603;
    compiler::TNode<String> tmp604;
    compiler::TNode<Number> tmp605;
    compiler::TNode<Object> tmp606;
    compiler::TNode<Object> tmp607;
    compiler::TNode<BuiltinPtr> tmp608;
    compiler::TNode<Map> tmp609;
    compiler::TNode<UintPtrT> tmp610;
    compiler::TNode<IntPtrT> tmp611;
    compiler::TNode<IntPtrT> tmp612;
    compiler::TNode<BuiltinPtr> tmp613;
    compiler::TNode<FixedArray> tmp614;
    compiler::TNode<IntPtrT> tmp615;
    compiler::TNode<IntPtrT> tmp616;
    compiler::TNode<BoolT> tmp617;
    compiler::TNode<UintPtrT> tmp618;
    compiler::TNode<Object> tmp619;
    compiler::TNode<String> tmp620;
    compiler::TNode<String> tmp621;
    compiler::TNode<IntPtrT> tmp622;
    compiler::TNode<IntPtrT> tmp623;
    compiler::TNode<Context> tmp624;
    compiler::TNode<String> tmp625;
    compiler::TNode<IntPtrT> tmp626;
    compiler::TNode<IntPtrT> tmp627;
    compiler::TNode<BoolT> tmp628;
    compiler::TNode<IntPtrT> tmp629;
    compiler::TNode<IntPtrT> tmp630;
    compiler::TNode<BoolT> tmp631;
    compiler::TNode<Context> tmp632;
    compiler::TNode<IntPtrT> tmp633;
    compiler::TNode<IntPtrT> tmp634;
    compiler::TNode<BoolT> tmp635;
    ca_.Bind(&block31, &tmp602, &tmp603, &tmp604, &tmp605, &tmp606, &tmp607, &tmp608, &tmp609, &tmp610, &tmp611, &tmp612, &tmp613, &tmp614, &tmp615, &tmp616, &tmp617, &tmp618, &tmp619, &tmp620, &tmp621, &tmp622, &tmp623, &tmp624, &tmp625, &tmp626, &tmp627, &tmp628, &tmp629, &tmp630, &tmp631, &tmp632, &tmp633, &tmp634, &tmp635);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 173);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 174);
    compiler::TNode<IntPtrT> tmp636;
    USE(tmp636);
    tmp636 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMul(compiler::TNode<IntPtrT>{tmp634}, compiler::TNode<IntPtrT>{tmp633}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 177);
    compiler::TNode<IntPtrT> tmp637;
    USE(tmp637);
    tmp637 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrDiv(compiler::TNode<IntPtrT>{tmp636}, compiler::TNode<IntPtrT>{tmp634}));
    compiler::TNode<BoolT> tmp638;
    USE(tmp638);
    tmp638 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<IntPtrT>{tmp637}, compiler::TNode<IntPtrT>{tmp633}));
    ca_.Branch(tmp638, &block33, &block34, tmp602, tmp603, tmp604, tmp605, tmp606, tmp607, tmp608, tmp609, tmp610, tmp611, tmp612, tmp613, tmp614, tmp615, tmp616, tmp617, tmp618, tmp619, tmp620, tmp621, tmp622, tmp623, tmp624, tmp625, tmp626, tmp627, tmp628, tmp629, tmp630, tmp631, tmp632, tmp633, tmp634, tmp635, tmp633, tmp636);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp639;
    compiler::TNode<JSReceiver> tmp640;
    compiler::TNode<String> tmp641;
    compiler::TNode<Number> tmp642;
    compiler::TNode<Object> tmp643;
    compiler::TNode<Object> tmp644;
    compiler::TNode<BuiltinPtr> tmp645;
    compiler::TNode<Map> tmp646;
    compiler::TNode<UintPtrT> tmp647;
    compiler::TNode<IntPtrT> tmp648;
    compiler::TNode<IntPtrT> tmp649;
    compiler::TNode<BuiltinPtr> tmp650;
    compiler::TNode<FixedArray> tmp651;
    compiler::TNode<IntPtrT> tmp652;
    compiler::TNode<IntPtrT> tmp653;
    compiler::TNode<BoolT> tmp654;
    compiler::TNode<UintPtrT> tmp655;
    compiler::TNode<Object> tmp656;
    compiler::TNode<String> tmp657;
    compiler::TNode<String> tmp658;
    compiler::TNode<IntPtrT> tmp659;
    compiler::TNode<IntPtrT> tmp660;
    compiler::TNode<Context> tmp661;
    compiler::TNode<String> tmp662;
    compiler::TNode<IntPtrT> tmp663;
    compiler::TNode<IntPtrT> tmp664;
    compiler::TNode<BoolT> tmp665;
    compiler::TNode<IntPtrT> tmp666;
    compiler::TNode<IntPtrT> tmp667;
    compiler::TNode<BoolT> tmp668;
    compiler::TNode<Context> tmp669;
    compiler::TNode<IntPtrT> tmp670;
    compiler::TNode<IntPtrT> tmp671;
    compiler::TNode<BoolT> tmp672;
    compiler::TNode<IntPtrT> tmp673;
    compiler::TNode<IntPtrT> tmp674;
    ca_.Bind(&block33, &tmp639, &tmp640, &tmp641, &tmp642, &tmp643, &tmp644, &tmp645, &tmp646, &tmp647, &tmp648, &tmp649, &tmp650, &tmp651, &tmp652, &tmp653, &tmp654, &tmp655, &tmp656, &tmp657, &tmp658, &tmp659, &tmp660, &tmp661, &tmp662, &tmp663, &tmp664, &tmp665, &tmp666, &tmp667, &tmp668, &tmp669, &tmp670, &tmp671, &tmp672, &tmp673, &tmp674);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 178);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp669);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp676;
    compiler::TNode<JSReceiver> tmp677;
    compiler::TNode<String> tmp678;
    compiler::TNode<Number> tmp679;
    compiler::TNode<Object> tmp680;
    compiler::TNode<Object> tmp681;
    compiler::TNode<BuiltinPtr> tmp682;
    compiler::TNode<Map> tmp683;
    compiler::TNode<UintPtrT> tmp684;
    compiler::TNode<IntPtrT> tmp685;
    compiler::TNode<IntPtrT> tmp686;
    compiler::TNode<BuiltinPtr> tmp687;
    compiler::TNode<FixedArray> tmp688;
    compiler::TNode<IntPtrT> tmp689;
    compiler::TNode<IntPtrT> tmp690;
    compiler::TNode<BoolT> tmp691;
    compiler::TNode<UintPtrT> tmp692;
    compiler::TNode<Object> tmp693;
    compiler::TNode<String> tmp694;
    compiler::TNode<String> tmp695;
    compiler::TNode<IntPtrT> tmp696;
    compiler::TNode<IntPtrT> tmp697;
    compiler::TNode<Context> tmp698;
    compiler::TNode<String> tmp699;
    compiler::TNode<IntPtrT> tmp700;
    compiler::TNode<IntPtrT> tmp701;
    compiler::TNode<BoolT> tmp702;
    compiler::TNode<IntPtrT> tmp703;
    compiler::TNode<IntPtrT> tmp704;
    compiler::TNode<BoolT> tmp705;
    compiler::TNode<Context> tmp706;
    compiler::TNode<IntPtrT> tmp707;
    compiler::TNode<IntPtrT> tmp708;
    compiler::TNode<BoolT> tmp709;
    compiler::TNode<IntPtrT> tmp710;
    compiler::TNode<IntPtrT> tmp711;
    ca_.Bind(&block34, &tmp676, &tmp677, &tmp678, &tmp679, &tmp680, &tmp681, &tmp682, &tmp683, &tmp684, &tmp685, &tmp686, &tmp687, &tmp688, &tmp689, &tmp690, &tmp691, &tmp692, &tmp693, &tmp694, &tmp695, &tmp696, &tmp697, &tmp698, &tmp699, &tmp700, &tmp701, &tmp702, &tmp703, &tmp704, &tmp705, &tmp706, &tmp707, &tmp708, &tmp709, &tmp710, &tmp711);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 181);
    compiler::TNode<IntPtrT> tmp712;
    USE(tmp712);
    tmp712 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp706}, compiler::TNode<IntPtrT>{tmp690}, compiler::TNode<IntPtrT>{tmp711}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 182);
    ca_.Branch(tmp709, &block35, &block36, tmp676, tmp677, tmp678, tmp679, tmp680, tmp681, tmp682, tmp683, tmp684, tmp685, tmp686, tmp687, tmp688, tmp689, tmp712, tmp691, tmp692, tmp693, tmp694, tmp695, tmp696, tmp697, tmp698, tmp699, tmp700, tmp701, tmp702, tmp703, tmp704, tmp705, tmp706, tmp707, tmp708, tmp709, tmp710, tmp711);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp713;
    compiler::TNode<JSReceiver> tmp714;
    compiler::TNode<String> tmp715;
    compiler::TNode<Number> tmp716;
    compiler::TNode<Object> tmp717;
    compiler::TNode<Object> tmp718;
    compiler::TNode<BuiltinPtr> tmp719;
    compiler::TNode<Map> tmp720;
    compiler::TNode<UintPtrT> tmp721;
    compiler::TNode<IntPtrT> tmp722;
    compiler::TNode<IntPtrT> tmp723;
    compiler::TNode<BuiltinPtr> tmp724;
    compiler::TNode<FixedArray> tmp725;
    compiler::TNode<IntPtrT> tmp726;
    compiler::TNode<IntPtrT> tmp727;
    compiler::TNode<BoolT> tmp728;
    compiler::TNode<UintPtrT> tmp729;
    compiler::TNode<Object> tmp730;
    compiler::TNode<String> tmp731;
    compiler::TNode<String> tmp732;
    compiler::TNode<IntPtrT> tmp733;
    compiler::TNode<IntPtrT> tmp734;
    compiler::TNode<Context> tmp735;
    compiler::TNode<String> tmp736;
    compiler::TNode<IntPtrT> tmp737;
    compiler::TNode<IntPtrT> tmp738;
    compiler::TNode<BoolT> tmp739;
    compiler::TNode<IntPtrT> tmp740;
    compiler::TNode<IntPtrT> tmp741;
    compiler::TNode<BoolT> tmp742;
    compiler::TNode<Context> tmp743;
    compiler::TNode<IntPtrT> tmp744;
    compiler::TNode<IntPtrT> tmp745;
    compiler::TNode<BoolT> tmp746;
    compiler::TNode<IntPtrT> tmp747;
    compiler::TNode<IntPtrT> tmp748;
    ca_.Bind(&block35, &tmp713, &tmp714, &tmp715, &tmp716, &tmp717, &tmp718, &tmp719, &tmp720, &tmp721, &tmp722, &tmp723, &tmp724, &tmp725, &tmp726, &tmp727, &tmp728, &tmp729, &tmp730, &tmp731, &tmp732, &tmp733, &tmp734, &tmp735, &tmp736, &tmp737, &tmp738, &tmp739, &tmp740, &tmp741, &tmp742, &tmp743, &tmp744, &tmp745, &tmp746, &tmp747, &tmp748);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 184);
    compiler::TNode<IntPtrT> tmp749;
    USE(tmp749);
    tmp749 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp750;
    USE(tmp750);
    tmp750 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp726}, compiler::TNode<IntPtrT>{tmp749}));
    compiler::TNode<Smi> tmp751;
    USE(tmp751);
    tmp751 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp747}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 183);
    compiler::TNode<FixedArray> tmp752;
    USE(tmp752);
    tmp752 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray>{tmp725}, compiler::TNode<IntPtrT>{tmp726}, compiler::TNode<Smi>{tmp751}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 182);
    ca_.Goto(&block36, tmp713, tmp714, tmp715, tmp716, tmp717, tmp718, tmp719, tmp720, tmp721, tmp722, tmp723, tmp724, tmp752, tmp750, tmp727, tmp728, tmp729, tmp730, tmp731, tmp732, tmp733, tmp734, tmp735, tmp736, tmp737, tmp738, tmp739, tmp740, tmp741, tmp742, tmp743, tmp744, tmp745, tmp746, tmp747, tmp748);
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp753;
    compiler::TNode<JSReceiver> tmp754;
    compiler::TNode<String> tmp755;
    compiler::TNode<Number> tmp756;
    compiler::TNode<Object> tmp757;
    compiler::TNode<Object> tmp758;
    compiler::TNode<BuiltinPtr> tmp759;
    compiler::TNode<Map> tmp760;
    compiler::TNode<UintPtrT> tmp761;
    compiler::TNode<IntPtrT> tmp762;
    compiler::TNode<IntPtrT> tmp763;
    compiler::TNode<BuiltinPtr> tmp764;
    compiler::TNode<FixedArray> tmp765;
    compiler::TNode<IntPtrT> tmp766;
    compiler::TNode<IntPtrT> tmp767;
    compiler::TNode<BoolT> tmp768;
    compiler::TNode<UintPtrT> tmp769;
    compiler::TNode<Object> tmp770;
    compiler::TNode<String> tmp771;
    compiler::TNode<String> tmp772;
    compiler::TNode<IntPtrT> tmp773;
    compiler::TNode<IntPtrT> tmp774;
    compiler::TNode<Context> tmp775;
    compiler::TNode<String> tmp776;
    compiler::TNode<IntPtrT> tmp777;
    compiler::TNode<IntPtrT> tmp778;
    compiler::TNode<BoolT> tmp779;
    compiler::TNode<IntPtrT> tmp780;
    compiler::TNode<IntPtrT> tmp781;
    compiler::TNode<BoolT> tmp782;
    compiler::TNode<Context> tmp783;
    compiler::TNode<IntPtrT> tmp784;
    compiler::TNode<IntPtrT> tmp785;
    compiler::TNode<BoolT> tmp786;
    compiler::TNode<IntPtrT> tmp787;
    compiler::TNode<IntPtrT> tmp788;
    ca_.Bind(&block36, &tmp753, &tmp754, &tmp755, &tmp756, &tmp757, &tmp758, &tmp759, &tmp760, &tmp761, &tmp762, &tmp763, &tmp764, &tmp765, &tmp766, &tmp767, &tmp768, &tmp769, &tmp770, &tmp771, &tmp772, &tmp773, &tmp774, &tmp775, &tmp776, &tmp777, &tmp778, &tmp779, &tmp780, &tmp781, &tmp782, &tmp783, &tmp784, &tmp785, &tmp786, &tmp787, &tmp788);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 170);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 159);
    ca_.Goto(&block29, tmp753, tmp754, tmp755, tmp756, tmp757, tmp758, tmp759, tmp760, tmp761, tmp762, tmp763, tmp764, tmp765, tmp766, tmp767, tmp768, tmp769, tmp770, tmp771, tmp772, tmp773, tmp774, tmp775, tmp776, tmp777, tmp778, tmp779, tmp780, tmp781, tmp782, tmp783, tmp784, tmp785, tmp786);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp789;
    compiler::TNode<JSReceiver> tmp790;
    compiler::TNode<String> tmp791;
    compiler::TNode<Number> tmp792;
    compiler::TNode<Object> tmp793;
    compiler::TNode<Object> tmp794;
    compiler::TNode<BuiltinPtr> tmp795;
    compiler::TNode<Map> tmp796;
    compiler::TNode<UintPtrT> tmp797;
    compiler::TNode<IntPtrT> tmp798;
    compiler::TNode<IntPtrT> tmp799;
    compiler::TNode<BuiltinPtr> tmp800;
    compiler::TNode<FixedArray> tmp801;
    compiler::TNode<IntPtrT> tmp802;
    compiler::TNode<IntPtrT> tmp803;
    compiler::TNode<BoolT> tmp804;
    compiler::TNode<UintPtrT> tmp805;
    compiler::TNode<Object> tmp806;
    compiler::TNode<String> tmp807;
    compiler::TNode<String> tmp808;
    compiler::TNode<IntPtrT> tmp809;
    compiler::TNode<IntPtrT> tmp810;
    compiler::TNode<Context> tmp811;
    compiler::TNode<String> tmp812;
    compiler::TNode<IntPtrT> tmp813;
    compiler::TNode<IntPtrT> tmp814;
    compiler::TNode<BoolT> tmp815;
    compiler::TNode<IntPtrT> tmp816;
    compiler::TNode<IntPtrT> tmp817;
    compiler::TNode<BoolT> tmp818;
    compiler::TNode<Context> tmp819;
    compiler::TNode<IntPtrT> tmp820;
    compiler::TNode<IntPtrT> tmp821;
    compiler::TNode<BoolT> tmp822;
    ca_.Bind(&block29, &tmp789, &tmp790, &tmp791, &tmp792, &tmp793, &tmp794, &tmp795, &tmp796, &tmp797, &tmp798, &tmp799, &tmp800, &tmp801, &tmp802, &tmp803, &tmp804, &tmp805, &tmp806, &tmp807, &tmp808, &tmp809, &tmp810, &tmp811, &tmp812, &tmp813, &tmp814, &tmp815, &tmp816, &tmp817, &tmp818, &tmp819, &tmp820, &tmp821, &tmp822);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 162);
    compiler::TNode<IntPtrT> tmp823;
    USE(tmp823);
    tmp823 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadStringLengthAsWord(compiler::TNode<String>{tmp812}));
    compiler::TNode<IntPtrT> tmp824;
    USE(tmp824);
    tmp824 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp811}, compiler::TNode<IntPtrT>{tmp803}, compiler::TNode<IntPtrT>{tmp823}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 161);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 164);
    compiler::TNode<IntPtrT> tmp825;
    USE(tmp825);
    tmp825 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp826;
    USE(tmp826);
    tmp826 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp802}, compiler::TNode<IntPtrT>{tmp825}));
    compiler::TNode<FixedArray> tmp827;
    USE(tmp827);
    tmp827 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray6String(compiler::TNode<FixedArray>{tmp801}, compiler::TNode<IntPtrT>{tmp802}, compiler::TNode<String>{tmp812}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 163);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 166);
    compiler::TNode<Int32T> tmp828;
    USE(tmp828);
    tmp828 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadInstanceType(compiler::TNode<HeapObject>{tmp812}));
    compiler::TNode<BoolT> tmp829;
    USE(tmp829);
    tmp829 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsOneByteStringInstanceType(compiler::TNode<Int32T>{tmp828}));
    compiler::TNode<BoolT> tmp830;
    USE(tmp830);
    tmp830 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32And(compiler::TNode<BoolT>{tmp829}, compiler::TNode<BoolT>{tmp804}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 165);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 156);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 315);
    ca_.Goto(&block28, tmp789, tmp790, tmp791, tmp792, tmp793, tmp794, tmp795, tmp796, tmp797, tmp798, tmp799, tmp800, tmp827, tmp826, tmp824, tmp830, tmp805, tmp806, tmp807, tmp808, tmp809, tmp810, tmp811, tmp812, tmp813, tmp814);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp831;
    compiler::TNode<JSReceiver> tmp832;
    compiler::TNode<String> tmp833;
    compiler::TNode<Number> tmp834;
    compiler::TNode<Object> tmp835;
    compiler::TNode<Object> tmp836;
    compiler::TNode<BuiltinPtr> tmp837;
    compiler::TNode<Map> tmp838;
    compiler::TNode<UintPtrT> tmp839;
    compiler::TNode<IntPtrT> tmp840;
    compiler::TNode<IntPtrT> tmp841;
    compiler::TNode<BuiltinPtr> tmp842;
    compiler::TNode<FixedArray> tmp843;
    compiler::TNode<IntPtrT> tmp844;
    compiler::TNode<IntPtrT> tmp845;
    compiler::TNode<BoolT> tmp846;
    compiler::TNode<UintPtrT> tmp847;
    compiler::TNode<Object> tmp848;
    compiler::TNode<String> tmp849;
    compiler::TNode<String> tmp850;
    compiler::TNode<IntPtrT> tmp851;
    compiler::TNode<IntPtrT> tmp852;
    compiler::TNode<Context> tmp853;
    compiler::TNode<String> tmp854;
    compiler::TNode<IntPtrT> tmp855;
    compiler::TNode<IntPtrT> tmp856;
    ca_.Bind(&block28, &tmp831, &tmp832, &tmp833, &tmp834, &tmp835, &tmp836, &tmp837, &tmp838, &tmp839, &tmp840, &tmp841, &tmp842, &tmp843, &tmp844, &tmp845, &tmp846, &tmp847, &tmp848, &tmp849, &tmp850, &tmp851, &tmp852, &tmp853, &tmp854, &tmp855, &tmp856);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 316);
    compiler::TNode<IntPtrT> tmp857;
    USE(tmp857);
    tmp857 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 277);
    ca_.Goto(&block4, tmp831, tmp832, tmp833, tmp834, tmp835, tmp836, tmp837, tmp838, tmp839, tmp840, tmp857, tmp842, tmp843, tmp844, tmp845, tmp846, tmp847);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp858;
    compiler::TNode<JSReceiver> tmp859;
    compiler::TNode<String> tmp860;
    compiler::TNode<Number> tmp861;
    compiler::TNode<Object> tmp862;
    compiler::TNode<Object> tmp863;
    compiler::TNode<BuiltinPtr> tmp864;
    compiler::TNode<Map> tmp865;
    compiler::TNode<UintPtrT> tmp866;
    compiler::TNode<IntPtrT> tmp867;
    compiler::TNode<IntPtrT> tmp868;
    compiler::TNode<BuiltinPtr> tmp869;
    compiler::TNode<FixedArray> tmp870;
    compiler::TNode<IntPtrT> tmp871;
    compiler::TNode<IntPtrT> tmp872;
    compiler::TNode<BoolT> tmp873;
    compiler::TNode<UintPtrT> tmp874;
    ca_.Bind(&block3, &tmp858, &tmp859, &tmp860, &tmp861, &tmp862, &tmp863, &tmp864, &tmp865, &tmp866, &tmp867, &tmp868, &tmp869, &tmp870, &tmp871, &tmp872, &tmp873, &tmp874);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 320);
    compiler::TNode<BoolT> tmp875;
    USE(tmp875);
    tmp875 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 171);
    compiler::TNode<IntPtrT> tmp876;
    USE(tmp876);
    tmp876 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp877;
    USE(tmp877);
    tmp877 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp868}, compiler::TNode<IntPtrT>{tmp876}));
    ca_.Branch(tmp877, &block38, &block40, tmp858, tmp859, tmp860, tmp861, tmp862, tmp863, tmp864, tmp865, tmp866, tmp867, tmp868, tmp869, tmp870, tmp871, tmp872, tmp873, tmp874, tmp868, tmp867, tmp858, tmp868, tmp867, tmp875);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp878;
    compiler::TNode<JSReceiver> tmp879;
    compiler::TNode<String> tmp880;
    compiler::TNode<Number> tmp881;
    compiler::TNode<Object> tmp882;
    compiler::TNode<Object> tmp883;
    compiler::TNode<BuiltinPtr> tmp884;
    compiler::TNode<Map> tmp885;
    compiler::TNode<UintPtrT> tmp886;
    compiler::TNode<IntPtrT> tmp887;
    compiler::TNode<IntPtrT> tmp888;
    compiler::TNode<BuiltinPtr> tmp889;
    compiler::TNode<FixedArray> tmp890;
    compiler::TNode<IntPtrT> tmp891;
    compiler::TNode<IntPtrT> tmp892;
    compiler::TNode<BoolT> tmp893;
    compiler::TNode<UintPtrT> tmp894;
    compiler::TNode<IntPtrT> tmp895;
    compiler::TNode<IntPtrT> tmp896;
    compiler::TNode<Context> tmp897;
    compiler::TNode<IntPtrT> tmp898;
    compiler::TNode<IntPtrT> tmp899;
    compiler::TNode<BoolT> tmp900;
    ca_.Bind(&block40, &tmp878, &tmp879, &tmp880, &tmp881, &tmp882, &tmp883, &tmp884, &tmp885, &tmp886, &tmp887, &tmp888, &tmp889, &tmp890, &tmp891, &tmp892, &tmp893, &tmp894, &tmp895, &tmp896, &tmp897, &tmp898, &tmp899, &tmp900);
    compiler::TNode<IntPtrT> tmp901;
    USE(tmp901);
    tmp901 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp902;
    USE(tmp902);
    tmp902 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp899}, compiler::TNode<IntPtrT>{tmp901}));
    ca_.Branch(tmp902, &block38, &block39, tmp878, tmp879, tmp880, tmp881, tmp882, tmp883, tmp884, tmp885, tmp886, tmp887, tmp888, tmp889, tmp890, tmp891, tmp892, tmp893, tmp894, tmp895, tmp896, tmp897, tmp898, tmp899, tmp900);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp903;
    compiler::TNode<JSReceiver> tmp904;
    compiler::TNode<String> tmp905;
    compiler::TNode<Number> tmp906;
    compiler::TNode<Object> tmp907;
    compiler::TNode<Object> tmp908;
    compiler::TNode<BuiltinPtr> tmp909;
    compiler::TNode<Map> tmp910;
    compiler::TNode<UintPtrT> tmp911;
    compiler::TNode<IntPtrT> tmp912;
    compiler::TNode<IntPtrT> tmp913;
    compiler::TNode<BuiltinPtr> tmp914;
    compiler::TNode<FixedArray> tmp915;
    compiler::TNode<IntPtrT> tmp916;
    compiler::TNode<IntPtrT> tmp917;
    compiler::TNode<BoolT> tmp918;
    compiler::TNode<UintPtrT> tmp919;
    compiler::TNode<IntPtrT> tmp920;
    compiler::TNode<IntPtrT> tmp921;
    compiler::TNode<Context> tmp922;
    compiler::TNode<IntPtrT> tmp923;
    compiler::TNode<IntPtrT> tmp924;
    compiler::TNode<BoolT> tmp925;
    ca_.Bind(&block38, &tmp903, &tmp904, &tmp905, &tmp906, &tmp907, &tmp908, &tmp909, &tmp910, &tmp911, &tmp912, &tmp913, &tmp914, &tmp915, &tmp916, &tmp917, &tmp918, &tmp919, &tmp920, &tmp921, &tmp922, &tmp923, &tmp924, &tmp925);
    ca_.Goto(&block37, tmp903, tmp904, tmp905, tmp906, tmp907, tmp908, tmp909, tmp910, tmp911, tmp912, tmp913, tmp914, tmp915, tmp916, tmp917, tmp918, tmp919, tmp920, tmp921, tmp922, tmp923, tmp924, tmp925);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp926;
    compiler::TNode<JSReceiver> tmp927;
    compiler::TNode<String> tmp928;
    compiler::TNode<Number> tmp929;
    compiler::TNode<Object> tmp930;
    compiler::TNode<Object> tmp931;
    compiler::TNode<BuiltinPtr> tmp932;
    compiler::TNode<Map> tmp933;
    compiler::TNode<UintPtrT> tmp934;
    compiler::TNode<IntPtrT> tmp935;
    compiler::TNode<IntPtrT> tmp936;
    compiler::TNode<BuiltinPtr> tmp937;
    compiler::TNode<FixedArray> tmp938;
    compiler::TNode<IntPtrT> tmp939;
    compiler::TNode<IntPtrT> tmp940;
    compiler::TNode<BoolT> tmp941;
    compiler::TNode<UintPtrT> tmp942;
    compiler::TNode<IntPtrT> tmp943;
    compiler::TNode<IntPtrT> tmp944;
    compiler::TNode<Context> tmp945;
    compiler::TNode<IntPtrT> tmp946;
    compiler::TNode<IntPtrT> tmp947;
    compiler::TNode<BoolT> tmp948;
    ca_.Bind(&block39, &tmp926, &tmp927, &tmp928, &tmp929, &tmp930, &tmp931, &tmp932, &tmp933, &tmp934, &tmp935, &tmp936, &tmp937, &tmp938, &tmp939, &tmp940, &tmp941, &tmp942, &tmp943, &tmp944, &tmp945, &tmp946, &tmp947, &tmp948);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 173);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 174);
    compiler::TNode<IntPtrT> tmp949;
    USE(tmp949);
    tmp949 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMul(compiler::TNode<IntPtrT>{tmp947}, compiler::TNode<IntPtrT>{tmp946}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 177);
    compiler::TNode<IntPtrT> tmp950;
    USE(tmp950);
    tmp950 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrDiv(compiler::TNode<IntPtrT>{tmp949}, compiler::TNode<IntPtrT>{tmp947}));
    compiler::TNode<BoolT> tmp951;
    USE(tmp951);
    tmp951 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<IntPtrT>{tmp950}, compiler::TNode<IntPtrT>{tmp946}));
    ca_.Branch(tmp951, &block41, &block42, tmp926, tmp927, tmp928, tmp929, tmp930, tmp931, tmp932, tmp933, tmp934, tmp935, tmp936, tmp937, tmp938, tmp939, tmp940, tmp941, tmp942, tmp943, tmp944, tmp945, tmp946, tmp947, tmp948, tmp946, tmp949);
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp952;
    compiler::TNode<JSReceiver> tmp953;
    compiler::TNode<String> tmp954;
    compiler::TNode<Number> tmp955;
    compiler::TNode<Object> tmp956;
    compiler::TNode<Object> tmp957;
    compiler::TNode<BuiltinPtr> tmp958;
    compiler::TNode<Map> tmp959;
    compiler::TNode<UintPtrT> tmp960;
    compiler::TNode<IntPtrT> tmp961;
    compiler::TNode<IntPtrT> tmp962;
    compiler::TNode<BuiltinPtr> tmp963;
    compiler::TNode<FixedArray> tmp964;
    compiler::TNode<IntPtrT> tmp965;
    compiler::TNode<IntPtrT> tmp966;
    compiler::TNode<BoolT> tmp967;
    compiler::TNode<UintPtrT> tmp968;
    compiler::TNode<IntPtrT> tmp969;
    compiler::TNode<IntPtrT> tmp970;
    compiler::TNode<Context> tmp971;
    compiler::TNode<IntPtrT> tmp972;
    compiler::TNode<IntPtrT> tmp973;
    compiler::TNode<BoolT> tmp974;
    compiler::TNode<IntPtrT> tmp975;
    compiler::TNode<IntPtrT> tmp976;
    ca_.Bind(&block41, &tmp952, &tmp953, &tmp954, &tmp955, &tmp956, &tmp957, &tmp958, &tmp959, &tmp960, &tmp961, &tmp962, &tmp963, &tmp964, &tmp965, &tmp966, &tmp967, &tmp968, &tmp969, &tmp970, &tmp971, &tmp972, &tmp973, &tmp974, &tmp975, &tmp976);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 178);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp971);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp978;
    compiler::TNode<JSReceiver> tmp979;
    compiler::TNode<String> tmp980;
    compiler::TNode<Number> tmp981;
    compiler::TNode<Object> tmp982;
    compiler::TNode<Object> tmp983;
    compiler::TNode<BuiltinPtr> tmp984;
    compiler::TNode<Map> tmp985;
    compiler::TNode<UintPtrT> tmp986;
    compiler::TNode<IntPtrT> tmp987;
    compiler::TNode<IntPtrT> tmp988;
    compiler::TNode<BuiltinPtr> tmp989;
    compiler::TNode<FixedArray> tmp990;
    compiler::TNode<IntPtrT> tmp991;
    compiler::TNode<IntPtrT> tmp992;
    compiler::TNode<BoolT> tmp993;
    compiler::TNode<UintPtrT> tmp994;
    compiler::TNode<IntPtrT> tmp995;
    compiler::TNode<IntPtrT> tmp996;
    compiler::TNode<Context> tmp997;
    compiler::TNode<IntPtrT> tmp998;
    compiler::TNode<IntPtrT> tmp999;
    compiler::TNode<BoolT> tmp1000;
    compiler::TNode<IntPtrT> tmp1001;
    compiler::TNode<IntPtrT> tmp1002;
    ca_.Bind(&block42, &tmp978, &tmp979, &tmp980, &tmp981, &tmp982, &tmp983, &tmp984, &tmp985, &tmp986, &tmp987, &tmp988, &tmp989, &tmp990, &tmp991, &tmp992, &tmp993, &tmp994, &tmp995, &tmp996, &tmp997, &tmp998, &tmp999, &tmp1000, &tmp1001, &tmp1002);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 181);
    compiler::TNode<IntPtrT> tmp1003;
    USE(tmp1003);
    tmp1003 = ca_.UncheckedCast<IntPtrT>(ArrayJoinBuiltinsFromDSLAssembler(state_).AddStringLength(compiler::TNode<Context>{tmp997}, compiler::TNode<IntPtrT>{tmp992}, compiler::TNode<IntPtrT>{tmp1002}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 182);
    ca_.Branch(tmp1000, &block43, &block44, tmp978, tmp979, tmp980, tmp981, tmp982, tmp983, tmp984, tmp985, tmp986, tmp987, tmp988, tmp989, tmp990, tmp991, tmp1003, tmp993, tmp994, tmp995, tmp996, tmp997, tmp998, tmp999, tmp1000, tmp1001, tmp1002);
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp1004;
    compiler::TNode<JSReceiver> tmp1005;
    compiler::TNode<String> tmp1006;
    compiler::TNode<Number> tmp1007;
    compiler::TNode<Object> tmp1008;
    compiler::TNode<Object> tmp1009;
    compiler::TNode<BuiltinPtr> tmp1010;
    compiler::TNode<Map> tmp1011;
    compiler::TNode<UintPtrT> tmp1012;
    compiler::TNode<IntPtrT> tmp1013;
    compiler::TNode<IntPtrT> tmp1014;
    compiler::TNode<BuiltinPtr> tmp1015;
    compiler::TNode<FixedArray> tmp1016;
    compiler::TNode<IntPtrT> tmp1017;
    compiler::TNode<IntPtrT> tmp1018;
    compiler::TNode<BoolT> tmp1019;
    compiler::TNode<UintPtrT> tmp1020;
    compiler::TNode<IntPtrT> tmp1021;
    compiler::TNode<IntPtrT> tmp1022;
    compiler::TNode<Context> tmp1023;
    compiler::TNode<IntPtrT> tmp1024;
    compiler::TNode<IntPtrT> tmp1025;
    compiler::TNode<BoolT> tmp1026;
    compiler::TNode<IntPtrT> tmp1027;
    compiler::TNode<IntPtrT> tmp1028;
    ca_.Bind(&block43, &tmp1004, &tmp1005, &tmp1006, &tmp1007, &tmp1008, &tmp1009, &tmp1010, &tmp1011, &tmp1012, &tmp1013, &tmp1014, &tmp1015, &tmp1016, &tmp1017, &tmp1018, &tmp1019, &tmp1020, &tmp1021, &tmp1022, &tmp1023, &tmp1024, &tmp1025, &tmp1026, &tmp1027, &tmp1028);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 184);
    compiler::TNode<IntPtrT> tmp1029;
    USE(tmp1029);
    tmp1029 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp1030;
    USE(tmp1030);
    tmp1030 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp1017}, compiler::TNode<IntPtrT>{tmp1029}));
    compiler::TNode<Smi> tmp1031;
    USE(tmp1031);
    tmp1031 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp1027}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 183);
    compiler::TNode<FixedArray> tmp1032;
    USE(tmp1032);
    tmp1032 = ca_.UncheckedCast<FixedArray>(ArrayJoinBuiltinsFromDSLAssembler(state_).StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray>{tmp1016}, compiler::TNode<IntPtrT>{tmp1017}, compiler::TNode<Smi>{tmp1031}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 182);
    ca_.Goto(&block44, tmp1004, tmp1005, tmp1006, tmp1007, tmp1008, tmp1009, tmp1010, tmp1011, tmp1012, tmp1013, tmp1014, tmp1015, tmp1032, tmp1030, tmp1018, tmp1019, tmp1020, tmp1021, tmp1022, tmp1023, tmp1024, tmp1025, tmp1026, tmp1027, tmp1028);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp1033;
    compiler::TNode<JSReceiver> tmp1034;
    compiler::TNode<String> tmp1035;
    compiler::TNode<Number> tmp1036;
    compiler::TNode<Object> tmp1037;
    compiler::TNode<Object> tmp1038;
    compiler::TNode<BuiltinPtr> tmp1039;
    compiler::TNode<Map> tmp1040;
    compiler::TNode<UintPtrT> tmp1041;
    compiler::TNode<IntPtrT> tmp1042;
    compiler::TNode<IntPtrT> tmp1043;
    compiler::TNode<BuiltinPtr> tmp1044;
    compiler::TNode<FixedArray> tmp1045;
    compiler::TNode<IntPtrT> tmp1046;
    compiler::TNode<IntPtrT> tmp1047;
    compiler::TNode<BoolT> tmp1048;
    compiler::TNode<UintPtrT> tmp1049;
    compiler::TNode<IntPtrT> tmp1050;
    compiler::TNode<IntPtrT> tmp1051;
    compiler::TNode<Context> tmp1052;
    compiler::TNode<IntPtrT> tmp1053;
    compiler::TNode<IntPtrT> tmp1054;
    compiler::TNode<BoolT> tmp1055;
    compiler::TNode<IntPtrT> tmp1056;
    compiler::TNode<IntPtrT> tmp1057;
    ca_.Bind(&block44, &tmp1033, &tmp1034, &tmp1035, &tmp1036, &tmp1037, &tmp1038, &tmp1039, &tmp1040, &tmp1041, &tmp1042, &tmp1043, &tmp1044, &tmp1045, &tmp1046, &tmp1047, &tmp1048, &tmp1049, &tmp1050, &tmp1051, &tmp1052, &tmp1053, &tmp1054, &tmp1055, &tmp1056, &tmp1057);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 170);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 320);
    ca_.Goto(&block37, tmp1033, tmp1034, tmp1035, tmp1036, tmp1037, tmp1038, tmp1039, tmp1040, tmp1041, tmp1042, tmp1043, tmp1044, tmp1045, tmp1046, tmp1047, tmp1048, tmp1049, tmp1050, tmp1051, tmp1052, tmp1053, tmp1054, tmp1055);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp1058;
    compiler::TNode<JSReceiver> tmp1059;
    compiler::TNode<String> tmp1060;
    compiler::TNode<Number> tmp1061;
    compiler::TNode<Object> tmp1062;
    compiler::TNode<Object> tmp1063;
    compiler::TNode<BuiltinPtr> tmp1064;
    compiler::TNode<Map> tmp1065;
    compiler::TNode<UintPtrT> tmp1066;
    compiler::TNode<IntPtrT> tmp1067;
    compiler::TNode<IntPtrT> tmp1068;
    compiler::TNode<BuiltinPtr> tmp1069;
    compiler::TNode<FixedArray> tmp1070;
    compiler::TNode<IntPtrT> tmp1071;
    compiler::TNode<IntPtrT> tmp1072;
    compiler::TNode<BoolT> tmp1073;
    compiler::TNode<UintPtrT> tmp1074;
    compiler::TNode<IntPtrT> tmp1075;
    compiler::TNode<IntPtrT> tmp1076;
    compiler::TNode<Context> tmp1077;
    compiler::TNode<IntPtrT> tmp1078;
    compiler::TNode<IntPtrT> tmp1079;
    compiler::TNode<BoolT> tmp1080;
    ca_.Bind(&block37, &tmp1058, &tmp1059, &tmp1060, &tmp1061, &tmp1062, &tmp1063, &tmp1064, &tmp1065, &tmp1066, &tmp1067, &tmp1068, &tmp1069, &tmp1070, &tmp1071, &tmp1072, &tmp1073, &tmp1074, &tmp1075, &tmp1076, &tmp1077, &tmp1078, &tmp1079, &tmp1080);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 323);
    compiler::TNode<String> tmp1081;
    USE(tmp1081);
    tmp1081 = ca_.UncheckedCast<String>(ArrayJoinBuiltinsFromDSLAssembler(state_).BufferJoin(compiler::TNode<Context>{tmp1058}, ArrayJoinBuiltinsFromDSLAssembler::Buffer{compiler::TNode<FixedArray>{tmp1070}, compiler::TNode<IntPtrT>{tmp1071}, compiler::TNode<IntPtrT>{tmp1072}, compiler::TNode<BoolT>{tmp1073}}, compiler::TNode<String>{tmp1060}));
    ca_.Goto(&block1, tmp1058, tmp1059, tmp1060, tmp1061, tmp1062, tmp1063, tmp1064, tmp1081);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp1082;
    compiler::TNode<JSReceiver> tmp1083;
    compiler::TNode<String> tmp1084;
    compiler::TNode<Number> tmp1085;
    compiler::TNode<Object> tmp1086;
    compiler::TNode<Object> tmp1087;
    compiler::TNode<BuiltinPtr> tmp1088;
    compiler::TNode<String> tmp1089;
    ca_.Bind(&block1, &tmp1082, &tmp1083, &tmp1084, &tmp1085, &tmp1086, &tmp1087, &tmp1088, &tmp1089);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 262);
    ca_.Goto(&block45, tmp1082, tmp1083, tmp1084, tmp1085, tmp1086, tmp1087, tmp1088, tmp1089);
  }

    compiler::TNode<Context> tmp1090;
    compiler::TNode<JSReceiver> tmp1091;
    compiler::TNode<String> tmp1092;
    compiler::TNode<Number> tmp1093;
    compiler::TNode<Object> tmp1094;
    compiler::TNode<Object> tmp1095;
    compiler::TNode<BuiltinPtr> tmp1096;
    compiler::TNode<String> tmp1097;
    ca_.Bind(&block45, &tmp1090, &tmp1091, &tmp1092, &tmp1093, &tmp1094, &tmp1095, &tmp1096, &tmp1097);
  return compiler::TNode<String>{tmp1097};
}

compiler::TNode<FixedArray> ArrayJoinBuiltinsFromDSLAssembler::StoreAndGrowFixedArray10JSReceiver(compiler::TNode<FixedArray> p_fixedArray, compiler::TNode<IntPtrT> p_index, compiler::TNode<JSReceiver> p_element) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, JSReceiver, FixedArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_fixedArray, p_index, p_element);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<IntPtrT> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 134);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp0}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 136);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp1}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<FixedArray> tmp5;
    compiler::TNode<IntPtrT> tmp6;
    compiler::TNode<JSReceiver> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    ca_.Bind(&block2, &tmp5, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 137);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp5}, compiler::TNode<IntPtrT>{tmp6}, compiler::TNode<HeapObject>{tmp7});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 138);
    ca_.Goto(&block1, tmp5, tmp6, tmp7, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<FixedArray> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    ca_.Bind(&block3, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 141);
    compiler::TNode<IntPtrT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<IntPtrT>{tmp12}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 144);
    compiler::TNode<IntPtrT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<FixedArray> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArray>{tmp9}, compiler::TNode<IntPtrT>{tmp14}, compiler::TNode<IntPtrT>{tmp12}, compiler::TNode<IntPtrT>{tmp13}, CodeStubAssembler::ExtractFixedArrayFlag::kFixedArrays));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 143);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 145);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp15}, compiler::TNode<IntPtrT>{tmp10}, compiler::TNode<HeapObject>{tmp11});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 146);
    ca_.Goto(&block1, tmp9, tmp10, tmp11, tmp15);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp16;
    compiler::TNode<IntPtrT> tmp17;
    compiler::TNode<JSReceiver> tmp18;
    compiler::TNode<FixedArray> tmp19;
    ca_.Bind(&block1, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 132);
    ca_.Goto(&block5, tmp16, tmp17, tmp18, tmp19);
  }

    compiler::TNode<FixedArray> tmp20;
    compiler::TNode<IntPtrT> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<FixedArray> tmp23;
    ca_.Bind(&block5, &tmp20, &tmp21, &tmp22, &tmp23);
  return compiler::TNode<FixedArray>{tmp23};
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 539);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 538);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 543);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 546);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 545);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 547);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 548);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 546);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 547);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 550);
    ArrayJoinBuiltinsFromDSLAssembler(state_).JoinStackPopInline(compiler::TNode<Context>{tmp108}, compiler::TNode<JSReceiver>{tmp109});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 551);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 554);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 532);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 539);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 538);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 543);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 546);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 545);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 547);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 548);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 546);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 547);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 550);
    ArrayJoinBuiltinsFromDSLAssembler(state_).JoinStackPopInline(compiler::TNode<Context>{tmp108}, compiler::TNode<JSReceiver>{tmp109});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 551);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 554);
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
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 532);
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
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, Smi, FixedArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_fixedArray, p_index, p_element);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<IntPtrT> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 134);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp0}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 136);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp1}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<FixedArray> tmp5;
    compiler::TNode<IntPtrT> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    ca_.Bind(&block2, &tmp5, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 137);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp5}, compiler::TNode<IntPtrT>{tmp6}, compiler::TNode<Smi>{tmp7});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 138);
    ca_.Goto(&block1, tmp5, tmp6, tmp7, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<FixedArray> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    ca_.Bind(&block3, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 141);
    compiler::TNode<IntPtrT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<IntPtrT>{tmp12}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 144);
    compiler::TNode<IntPtrT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<FixedArray> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArray>{tmp9}, compiler::TNode<IntPtrT>{tmp14}, compiler::TNode<IntPtrT>{tmp12}, compiler::TNode<IntPtrT>{tmp13}, CodeStubAssembler::ExtractFixedArrayFlag::kFixedArrays));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 143);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 145);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp15}, compiler::TNode<IntPtrT>{tmp10}, compiler::TNode<Smi>{tmp11});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 146);
    ca_.Goto(&block1, tmp9, tmp10, tmp11, tmp15);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp16;
    compiler::TNode<IntPtrT> tmp17;
    compiler::TNode<Smi> tmp18;
    compiler::TNode<FixedArray> tmp19;
    ca_.Bind(&block1, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 132);
    ca_.Goto(&block5, tmp16, tmp17, tmp18, tmp19);
  }

    compiler::TNode<FixedArray> tmp20;
    compiler::TNode<IntPtrT> tmp21;
    compiler::TNode<Smi> tmp22;
    compiler::TNode<FixedArray> tmp23;
    ca_.Bind(&block5, &tmp20, &tmp21, &tmp22, &tmp23);
  return compiler::TNode<FixedArray>{tmp23};
}

compiler::TNode<FixedArray> ArrayJoinBuiltinsFromDSLAssembler::StoreAndGrowFixedArray6String(compiler::TNode<FixedArray> p_fixedArray, compiler::TNode<IntPtrT> p_index, compiler::TNode<String> p_element) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, IntPtrT, String, FixedArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_fixedArray, p_index, p_element);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<IntPtrT> tmp1;
    compiler::TNode<String> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 134);
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).LoadAndUntagFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp0}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 136);
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp1}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<FixedArray> tmp5;
    compiler::TNode<IntPtrT> tmp6;
    compiler::TNode<String> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    ca_.Bind(&block2, &tmp5, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 137);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp5}, compiler::TNode<IntPtrT>{tmp6}, compiler::TNode<HeapObject>{tmp7});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 138);
    ca_.Goto(&block1, tmp5, tmp6, tmp7, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<FixedArray> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<String> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    ca_.Bind(&block3, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 141);
    compiler::TNode<IntPtrT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<IntPtrT>{tmp12}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 144);
    compiler::TNode<IntPtrT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<FixedArray> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<FixedArray>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArray>{tmp9}, compiler::TNode<IntPtrT>{tmp14}, compiler::TNode<IntPtrT>{tmp12}, compiler::TNode<IntPtrT>{tmp13}, CodeStubAssembler::ExtractFixedArrayFlag::kFixedArrays));
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 143);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 145);
    CodeStubAssembler(state_).StoreFixedArrayElement(compiler::TNode<FixedArray>{tmp15}, compiler::TNode<IntPtrT>{tmp10}, compiler::TNode<HeapObject>{tmp11});
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 146);
    ca_.Goto(&block1, tmp9, tmp10, tmp11, tmp15);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp16;
    compiler::TNode<IntPtrT> tmp17;
    compiler::TNode<String> tmp18;
    compiler::TNode<FixedArray> tmp19;
    ca_.Bind(&block1, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../v8/src/builtins/array-join.tq", 132);
    ca_.Goto(&block5, tmp16, tmp17, tmp18, tmp19);
  }

    compiler::TNode<FixedArray> tmp20;
    compiler::TNode<IntPtrT> tmp21;
    compiler::TNode<String> tmp22;
    compiler::TNode<FixedArray> tmp23;
    ca_.Bind(&block5, &tmp20, &tmp21, &tmp22, &tmp23);
  return compiler::TNode<FixedArray>{tmp23};
}

}  // namespace internal
}  // namespace v8

