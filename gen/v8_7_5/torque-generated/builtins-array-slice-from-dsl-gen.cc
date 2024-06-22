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

compiler::TNode<JSArray> ArraySliceBuiltinsFromDSLAssembler::HandleSimpleArgumentsSlice(compiler::TNode<Context> p_context, compiler::TNode<JSArgumentsObjectWithLength> p_args, compiler::TNode<Smi> p_start, compiler::TNode<Smi> p_count, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, Smi, FixedArrayBase> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, Smi, FixedArrayBase, FixedArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, Smi, FixedArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, Smi, FixedArray> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, Smi, FixedArray, Map, JSArray, FixedArrayBase> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, Smi, FixedArray, Map, JSArray, FixedArrayBase, FixedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, JSArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, JSArray> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_args, p_start, p_count);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSArgumentsObjectWithLength> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 11);
    compiler::TNode<Smi> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(FixedArray::kMaxRegularLength));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThanOrEqual(compiler::TNode<Smi>{tmp3}, compiler::TNode<Smi>{tmp4}));
    ca_.Branch(tmp5, &block3, &block4, tmp0, tmp1, tmp2, tmp3);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<JSArgumentsObjectWithLength> tmp7;
    compiler::TNode<Smi> tmp8;
    compiler::TNode<Smi> tmp9;
    ca_.Bind(&block3, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block1);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSArgumentsObjectWithLength> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Smi> tmp13;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 13);
    compiler::TNode<Smi> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp12}, compiler::TNode<Smi>{tmp13}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 15);
    compiler::TNode<IntPtrT> tmp15 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp15);
    compiler::TNode<FixedArrayBase>tmp16 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp11, tmp15});
    compiler::TNode<FixedArray> tmp17;
    USE(tmp17);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp17 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<HeapObject>{tmp16}, &label0);
    ca_.Goto(&block5, tmp10, tmp11, tmp12, tmp13, tmp14, tmp16, tmp17);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp10, tmp11, tmp12, tmp13, tmp14, tmp16);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<JSArgumentsObjectWithLength> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Smi> tmp22;
    compiler::TNode<FixedArrayBase> tmp23;
    ca_.Bind(&block6, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<JSArgumentsObjectWithLength> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<Smi> tmp28;
    compiler::TNode<FixedArrayBase> tmp29;
    compiler::TNode<FixedArray> tmp30;
    ca_.Bind(&block5, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 14);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 16);
    compiler::TNode<IntPtrT> tmp31 = ca_.IntPtrConstant(FixedArrayBase::kLengthOffset);
    USE(tmp31);
    compiler::TNode<Smi>tmp32 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp30, tmp31});
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiAbove(compiler::TNode<Smi>{tmp28}, compiler::TNode<Smi>{tmp32}));
    ca_.Branch(tmp33, &block7, &block8, tmp24, tmp25, tmp26, tmp27, tmp28, tmp30);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<JSArgumentsObjectWithLength> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<Smi> tmp38;
    compiler::TNode<FixedArray> tmp39;
    ca_.Bind(&block7, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.Goto(&block1);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<JSArgumentsObjectWithLength> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<Smi> tmp43;
    compiler::TNode<Smi> tmp44;
    compiler::TNode<FixedArray> tmp45;
    ca_.Bind(&block8, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 18);
    compiler::TNode<Map> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Map>(CodeStubAssembler(state_).LoadJSArrayElementsMap(HOLEY_ELEMENTS, compiler::TNode<Context>{tmp40}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 20);
    compiler::TNode<JSArray> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<JSArray>(CodeStubAssembler(state_).AllocateJSArray(HOLEY_ELEMENTS, compiler::TNode<Map>{tmp46}, compiler::TNode<Smi>{tmp43}, compiler::TNode<Smi>{tmp43}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 19);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 22);
    compiler::TNode<IntPtrT> tmp48 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp48);
    compiler::TNode<FixedArrayBase>tmp49 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp47, tmp48});
    compiler::TNode<FixedArray> tmp50;
    USE(tmp50);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp50 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<HeapObject>{tmp49}, &label0);
    ca_.Goto(&block9, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp49, tmp50);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp49);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<JSArgumentsObjectWithLength> tmp52;
    compiler::TNode<Smi> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<Smi> tmp55;
    compiler::TNode<FixedArray> tmp56;
    compiler::TNode<Map> tmp57;
    compiler::TNode<JSArray> tmp58;
    compiler::TNode<FixedArrayBase> tmp59;
    ca_.Bind(&block10, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.Goto(&block1);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp60;
    compiler::TNode<JSArgumentsObjectWithLength> tmp61;
    compiler::TNode<Smi> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<Smi> tmp64;
    compiler::TNode<FixedArray> tmp65;
    compiler::TNode<Map> tmp66;
    compiler::TNode<JSArray> tmp67;
    compiler::TNode<FixedArrayBase> tmp68;
    compiler::TNode<FixedArray> tmp69;
    ca_.Bind(&block9, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 21);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 24);
    compiler::TNode<IntPtrT> tmp70;
    USE(tmp70);
    tmp70 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp62}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 25);
    compiler::TNode<IntPtrT> tmp71;
    USE(tmp71);
    tmp71 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp63}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 23);
    compiler::TNode<IntPtrT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    CodeStubAssembler(state_).CopyElements(PACKED_ELEMENTS, compiler::TNode<FixedArrayBase>{tmp69}, compiler::TNode<IntPtrT>{tmp72}, compiler::TNode<FixedArrayBase>{tmp65}, compiler::TNode<IntPtrT>{tmp70}, compiler::TNode<IntPtrT>{tmp71});
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 26);
    ca_.Goto(&block2, tmp60, tmp61, tmp62, tmp63, tmp67);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp73;
    compiler::TNode<JSArgumentsObjectWithLength> tmp74;
    compiler::TNode<Smi> tmp75;
    compiler::TNode<Smi> tmp76;
    compiler::TNode<JSArray> tmp77;
    ca_.Bind(&block2, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 6);
    ca_.Goto(&block11, tmp73, tmp74, tmp75, tmp76, tmp77);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp78;
    compiler::TNode<JSArgumentsObjectWithLength> tmp79;
    compiler::TNode<Smi> tmp80;
    compiler::TNode<Smi> tmp81;
    compiler::TNode<JSArray> tmp82;
    ca_.Bind(&block11, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82);
  return compiler::TNode<JSArray>{tmp82};
}

compiler::TNode<JSArray> ArraySliceBuiltinsFromDSLAssembler::HandleFastAliasedSloppyArgumentsSlice(compiler::TNode<Context> p_context, compiler::TNode<JSArgumentsObjectWithLength> p_args, compiler::TNode<Smi> p_start, compiler::TNode<Smi> p_count, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, FixedArrayBase> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, FixedArrayBase, SloppyArgumentsElements> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, Object, FixedArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi, Context, Map, JSArray, Smi, FixedArray, Smi, Smi> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi, Context, Map, JSArray, Smi, FixedArray, Smi, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi, Context, Map, JSArray, Smi, FixedArray, Smi, Smi, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi, Context, Map, JSArray, Smi, FixedArray, Smi, Smi, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi, Context, Map, JSArray, Smi, FixedArray, Smi, Smi, Object, Object> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi, Context, Map, JSArray, Smi, FixedArray, Smi, Smi, Object, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi, Context, Map, JSArray, Smi, FixedArray, Smi, Smi> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, SloppyArgumentsElements, Smi, Smi, Smi, FixedArray, Smi, Context, Map, JSArray, Smi, FixedArray, Smi, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, JSArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArgumentsObjectWithLength, Smi, Smi, JSArray> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_args, p_start, p_count);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSArgumentsObjectWithLength> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 34);
    compiler::TNode<Smi> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(FixedArray::kMaxRegularLength));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThanOrEqual(compiler::TNode<Smi>{tmp3}, compiler::TNode<Smi>{tmp4}));
    ca_.Branch(tmp5, &block3, &block4, tmp0, tmp1, tmp2, tmp3);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<JSArgumentsObjectWithLength> tmp7;
    compiler::TNode<Smi> tmp8;
    compiler::TNode<Smi> tmp9;
    ca_.Bind(&block3, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block1);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSArgumentsObjectWithLength> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Smi> tmp13;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 37);
    compiler::TNode<IntPtrT> tmp14 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp14);
    compiler::TNode<FixedArrayBase>tmp15 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp11, tmp14});
    compiler::TNode<SloppyArgumentsElements> tmp16;
    USE(tmp16);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp16 = BaseBuiltinsFromDSLAssembler(state_).Cast23SloppyArgumentsElements(compiler::TNode<HeapObject>{tmp15}, &label0);
    ca_.Goto(&block5, tmp10, tmp11, tmp12, tmp13, tmp15, tmp16);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp10, tmp11, tmp12, tmp13, tmp15);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSArgumentsObjectWithLength> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<FixedArrayBase> tmp21;
    ca_.Bind(&block6, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<JSArgumentsObjectWithLength> tmp23;
    compiler::TNode<Smi> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<FixedArrayBase> tmp26;
    compiler::TNode<SloppyArgumentsElements> tmp27;
    ca_.Bind(&block5, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 36);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 38);
    compiler::TNode<IntPtrT> tmp28 = ca_.IntPtrConstant(FixedArrayBase::kLengthOffset);
    USE(tmp28);
    compiler::TNode<Smi>tmp29 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp27, tmp28});
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 40);
    compiler::TNode<Smi> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(SloppyArgumentsElements::kParameterMapStart));
    compiler::TNode<Smi> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp29}, compiler::TNode<Smi>{tmp30}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 39);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 44);
    compiler::TNode<Smi> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp24}, compiler::TNode<Smi>{tmp25}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 46);
    compiler::TNode<Object> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp27}, SloppyArgumentsElements::kArgumentsIndex));
    compiler::TNode<FixedArray> tmp34;
    USE(tmp34);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp34 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<Context>{tmp22}, compiler::TNode<Object>{tmp33}, &label0);
    ca_.Goto(&block7, tmp22, tmp23, tmp24, tmp25, tmp27, tmp29, tmp31, tmp32, tmp33, tmp34);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp22, tmp23, tmp24, tmp25, tmp27, tmp29, tmp31, tmp32, tmp33);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<JSArgumentsObjectWithLength> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<Smi> tmp38;
    compiler::TNode<SloppyArgumentsElements> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<Object> tmp43;
    ca_.Bind(&block8, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.Goto(&block1);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<JSArgumentsObjectWithLength> tmp45;
    compiler::TNode<Smi> tmp46;
    compiler::TNode<Smi> tmp47;
    compiler::TNode<SloppyArgumentsElements> tmp48;
    compiler::TNode<Smi> tmp49;
    compiler::TNode<Smi> tmp50;
    compiler::TNode<Smi> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<FixedArray> tmp53;
    ca_.Bind(&block7, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 45);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 48);
    compiler::TNode<IntPtrT> tmp54 = ca_.IntPtrConstant(FixedArrayBase::kLengthOffset);
    USE(tmp54);
    compiler::TNode<Smi>tmp55 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp53, tmp54});
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 49);
    compiler::TNode<BoolT> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiAbove(compiler::TNode<Smi>{tmp51}, compiler::TNode<Smi>{tmp55}));
    ca_.Branch(tmp56, &block9, &block10, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp53, tmp55);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp57;
    compiler::TNode<JSArgumentsObjectWithLength> tmp58;
    compiler::TNode<Smi> tmp59;
    compiler::TNode<Smi> tmp60;
    compiler::TNode<SloppyArgumentsElements> tmp61;
    compiler::TNode<Smi> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<Smi> tmp64;
    compiler::TNode<FixedArray> tmp65;
    compiler::TNode<Smi> tmp66;
    ca_.Bind(&block9, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.Goto(&block1);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<JSArgumentsObjectWithLength> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<SloppyArgumentsElements> tmp71;
    compiler::TNode<Smi> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<Smi> tmp74;
    compiler::TNode<FixedArray> tmp75;
    compiler::TNode<Smi> tmp76;
    ca_.Bind(&block10, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 52);
    compiler::TNode<Object> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp71}, SloppyArgumentsElements::kContextIndex));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 51);
    compiler::TNode<Context> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<Context>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7Context(compiler::TNode<Context>{tmp67}, compiler::TNode<Object>{tmp77}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 54);
    compiler::TNode<Map> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<Map>(CodeStubAssembler(state_).LoadJSArrayElementsMap(HOLEY_ELEMENTS, compiler::TNode<Context>{tmp67}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 56);
    compiler::TNode<JSArray> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<JSArray>(CodeStubAssembler(state_).AllocateJSArray(HOLEY_ELEMENTS, compiler::TNode<Map>{tmp79}, compiler::TNode<Smi>{tmp70}, compiler::TNode<Smi>{tmp70}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 55);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 58);
    compiler::TNode<Smi> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 59);
    compiler::TNode<IntPtrT> tmp82 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp82);
    compiler::TNode<FixedArrayBase>tmp83 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp80, tmp82});
    compiler::TNode<FixedArray> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp67}, compiler::TNode<Object>{tmp83}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 60);
    compiler::TNode<Smi> tmp85;
    USE(tmp85);
    tmp85 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiMin(compiler::TNode<Smi>{tmp73}, compiler::TNode<Smi>{tmp74}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 63);
    ca_.Goto(&block13, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp78, tmp79, tmp80, tmp81, tmp84, tmp85, tmp69);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp86;
    compiler::TNode<JSArgumentsObjectWithLength> tmp87;
    compiler::TNode<Smi> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<SloppyArgumentsElements> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<Smi> tmp92;
    compiler::TNode<Smi> tmp93;
    compiler::TNode<FixedArray> tmp94;
    compiler::TNode<Smi> tmp95;
    compiler::TNode<Context> tmp96;
    compiler::TNode<Map> tmp97;
    compiler::TNode<JSArray> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<FixedArray> tmp100;
    compiler::TNode<Smi> tmp101;
    compiler::TNode<Smi> tmp102;
    ca_.Bind(&block13, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    compiler::TNode<BoolT> tmp103;
    USE(tmp103);
    tmp103 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp102}, compiler::TNode<Smi>{tmp101}));
    ca_.Branch(tmp103, &block11, &block12, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp104;
    compiler::TNode<JSArgumentsObjectWithLength> tmp105;
    compiler::TNode<Smi> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<SloppyArgumentsElements> tmp108;
    compiler::TNode<Smi> tmp109;
    compiler::TNode<Smi> tmp110;
    compiler::TNode<Smi> tmp111;
    compiler::TNode<FixedArray> tmp112;
    compiler::TNode<Smi> tmp113;
    compiler::TNode<Context> tmp114;
    compiler::TNode<Map> tmp115;
    compiler::TNode<JSArray> tmp116;
    compiler::TNode<Smi> tmp117;
    compiler::TNode<FixedArray> tmp118;
    compiler::TNode<Smi> tmp119;
    compiler::TNode<Smi> tmp120;
    ca_.Bind(&block11, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 65);
    compiler::TNode<Smi> tmp121;
    USE(tmp121);
    tmp121 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(SloppyArgumentsElements::kParameterMapStart));
    compiler::TNode<Smi> tmp122;
    USE(tmp122);
    tmp122 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp120}, compiler::TNode<Smi>{tmp121}));
    compiler::TNode<Object> tmp123;
    USE(tmp123);
    tmp123 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp108}, compiler::TNode<Smi>{tmp122}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 64);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 66);
    compiler::TNode<Oddball> tmp124;
    USE(tmp124);
    tmp124 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp125;
    USE(tmp125);
    tmp125 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp123}, compiler::TNode<HeapObject>{tmp124}));
    ca_.Branch(tmp125, &block15, &block16, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp123);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp126;
    compiler::TNode<JSArgumentsObjectWithLength> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<SloppyArgumentsElements> tmp130;
    compiler::TNode<Smi> tmp131;
    compiler::TNode<Smi> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<FixedArray> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<Context> tmp136;
    compiler::TNode<Map> tmp137;
    compiler::TNode<JSArray> tmp138;
    compiler::TNode<Smi> tmp139;
    compiler::TNode<FixedArray> tmp140;
    compiler::TNode<Smi> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<Object> tmp143;
    ca_.Bind(&block15, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 67);
    compiler::TNode<Smi> tmp144;
    USE(tmp144);
    tmp144 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp126}, compiler::TNode<Object>{tmp143}));
    compiler::TNode<Object> tmp145;
    USE(tmp145);
    tmp145 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadContextElement(compiler::TNode<Context>{tmp136}, compiler::TNode<Smi>{tmp144}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 66);
    ca_.Goto(&block18, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp145);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp146;
    compiler::TNode<JSArgumentsObjectWithLength> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<Smi> tmp149;
    compiler::TNode<SloppyArgumentsElements> tmp150;
    compiler::TNode<Smi> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<FixedArray> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<Context> tmp156;
    compiler::TNode<Map> tmp157;
    compiler::TNode<JSArray> tmp158;
    compiler::TNode<Smi> tmp159;
    compiler::TNode<FixedArray> tmp160;
    compiler::TNode<Smi> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<Object> tmp163;
    ca_.Bind(&block16, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 68);
    compiler::TNode<Object> tmp164;
    USE(tmp164);
    tmp164 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp154}, compiler::TNode<Smi>{tmp162}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 66);
    ca_.Goto(&block17, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp165;
    compiler::TNode<JSArgumentsObjectWithLength> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<Smi> tmp168;
    compiler::TNode<SloppyArgumentsElements> tmp169;
    compiler::TNode<Smi> tmp170;
    compiler::TNode<Smi> tmp171;
    compiler::TNode<Smi> tmp172;
    compiler::TNode<FixedArray> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<Context> tmp175;
    compiler::TNode<Map> tmp176;
    compiler::TNode<JSArray> tmp177;
    compiler::TNode<Smi> tmp178;
    compiler::TNode<FixedArray> tmp179;
    compiler::TNode<Smi> tmp180;
    compiler::TNode<Smi> tmp181;
    compiler::TNode<Object> tmp182;
    compiler::TNode<Object> tmp183;
    ca_.Bind(&block18, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183);
    ca_.Goto(&block17, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp184;
    compiler::TNode<JSArgumentsObjectWithLength> tmp185;
    compiler::TNode<Smi> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<SloppyArgumentsElements> tmp188;
    compiler::TNode<Smi> tmp189;
    compiler::TNode<Smi> tmp190;
    compiler::TNode<Smi> tmp191;
    compiler::TNode<FixedArray> tmp192;
    compiler::TNode<Smi> tmp193;
    compiler::TNode<Context> tmp194;
    compiler::TNode<Map> tmp195;
    compiler::TNode<JSArray> tmp196;
    compiler::TNode<Smi> tmp197;
    compiler::TNode<FixedArray> tmp198;
    compiler::TNode<Smi> tmp199;
    compiler::TNode<Smi> tmp200;
    compiler::TNode<Object> tmp201;
    compiler::TNode<Object> tmp202;
    ca_.Bind(&block17, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 70);
    compiler::TNode<Smi> tmp203;
    USE(tmp203);
    tmp203 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp204;
    USE(tmp204);
    tmp204 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp197}, compiler::TNode<Smi>{tmp203}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 69);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp198}, compiler::TNode<Smi>{tmp197}, compiler::TNode<Object>{tmp202}, SKIP_WRITE_BARRIER);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 63);
    ca_.Goto(&block14, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp204, tmp198, tmp199, tmp200);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp205;
    compiler::TNode<JSArgumentsObjectWithLength> tmp206;
    compiler::TNode<Smi> tmp207;
    compiler::TNode<Smi> tmp208;
    compiler::TNode<SloppyArgumentsElements> tmp209;
    compiler::TNode<Smi> tmp210;
    compiler::TNode<Smi> tmp211;
    compiler::TNode<Smi> tmp212;
    compiler::TNode<FixedArray> tmp213;
    compiler::TNode<Smi> tmp214;
    compiler::TNode<Context> tmp215;
    compiler::TNode<Map> tmp216;
    compiler::TNode<JSArray> tmp217;
    compiler::TNode<Smi> tmp218;
    compiler::TNode<FixedArray> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<Smi> tmp221;
    ca_.Bind(&block14, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221);
    compiler::TNode<Smi> tmp222;
    USE(tmp222);
    tmp222 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp223;
    USE(tmp223);
    tmp223 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp221}, compiler::TNode<Smi>{tmp222}));
    ca_.Goto(&block13, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp223);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp224;
    compiler::TNode<JSArgumentsObjectWithLength> tmp225;
    compiler::TNode<Smi> tmp226;
    compiler::TNode<Smi> tmp227;
    compiler::TNode<SloppyArgumentsElements> tmp228;
    compiler::TNode<Smi> tmp229;
    compiler::TNode<Smi> tmp230;
    compiler::TNode<Smi> tmp231;
    compiler::TNode<FixedArray> tmp232;
    compiler::TNode<Smi> tmp233;
    compiler::TNode<Context> tmp234;
    compiler::TNode<Map> tmp235;
    compiler::TNode<JSArray> tmp236;
    compiler::TNode<Smi> tmp237;
    compiler::TNode<FixedArray> tmp238;
    compiler::TNode<Smi> tmp239;
    compiler::TNode<Smi> tmp240;
    ca_.Bind(&block12, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 75);
    compiler::TNode<Smi> tmp241;
    USE(tmp241);
    tmp241 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiMax(compiler::TNode<Smi>{tmp230}, compiler::TNode<Smi>{tmp226}));
    compiler::TNode<Smi> tmp242;
    USE(tmp242);
    tmp242 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiMin(compiler::TNode<Smi>{tmp241}, compiler::TNode<Smi>{tmp231}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 76);
    compiler::TNode<Smi> tmp243;
    USE(tmp243);
    tmp243 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp231}, compiler::TNode<Smi>{tmp242}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 78);
    compiler::TNode<IntPtrT> tmp244;
    USE(tmp244);
    tmp244 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp237}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 79);
    compiler::TNode<IntPtrT> tmp245;
    USE(tmp245);
    tmp245 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp242}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 80);
    compiler::TNode<IntPtrT> tmp246;
    USE(tmp246);
    tmp246 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp243}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 77);
    CodeStubAssembler(state_).CopyElements(PACKED_ELEMENTS, compiler::TNode<FixedArrayBase>{tmp238}, compiler::TNode<IntPtrT>{tmp244}, compiler::TNode<FixedArrayBase>{tmp232}, compiler::TNode<IntPtrT>{tmp245}, compiler::TNode<IntPtrT>{tmp246});
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 81);
    ca_.Goto(&block2, tmp224, tmp225, tmp226, tmp227, tmp236);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp247;
    compiler::TNode<JSArgumentsObjectWithLength> tmp248;
    compiler::TNode<Smi> tmp249;
    compiler::TNode<Smi> tmp250;
    compiler::TNode<JSArray> tmp251;
    ca_.Bind(&block2, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 29);
    ca_.Goto(&block19, tmp247, tmp248, tmp249, tmp250, tmp251);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp252;
    compiler::TNode<JSArgumentsObjectWithLength> tmp253;
    compiler::TNode<Smi> tmp254;
    compiler::TNode<Smi> tmp255;
    compiler::TNode<JSArray> tmp256;
    ca_.Bind(&block19, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256);
  return compiler::TNode<JSArray>{tmp256};
}

compiler::TNode<JSArray> ArraySliceBuiltinsFromDSLAssembler::HandleFastSlice(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o, compiler::TNode<Number> p_startNumber, compiler::TNode<Number> p_countNumber, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Number, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Number, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, Object, JSArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArray> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArray> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, Object, JSArgumentsObjectWithLength> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map, Context, JSArgumentsObjectWithLength, Smi, Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map, Context, JSArgumentsObjectWithLength, Smi, Smi, JSArray> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map, Context, JSArgumentsObjectWithLength, Smi, Smi> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map, Context, JSArgumentsObjectWithLength, Smi, Smi, JSArray> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, JSArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, JSArray> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_startNumber, p_countNumber);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Number> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 88);
    compiler::TNode<Smi> tmp4;
    USE(tmp4);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp2}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp2, tmp4);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp2);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    compiler::TNode<Number> tmp7;
    compiler::TNode<Number> tmp8;
    compiler::TNode<Number> tmp9;
    ca_.Bind(&block4, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block1);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<Number> tmp14;
    compiler::TNode<Smi> tmp15;
    ca_.Bind(&block3, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 89);
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp16 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp13}, &label0);
    ca_.Goto(&block5, tmp10, tmp11, tmp12, tmp13, tmp15, tmp13, tmp16);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp10, tmp11, tmp12, tmp13, tmp15, tmp13);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Number> tmp22;
    ca_.Bind(&block6, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<Number> tmp28;
    compiler::TNode<Smi> tmp29;
    ca_.Bind(&block5, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 92);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 93);
    compiler::TNode<JSArray> tmp30;
    USE(tmp30);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp30 = BaseBuiltinsFromDSLAssembler(state_).Cast20ATFastJSArrayForCopy(compiler::TNode<Context>{tmp23}, compiler::TNode<Object>{tmp24}, &label0);
    ca_.Goto(&block9, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29, tmp24, tmp24, tmp30);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29, tmp24, tmp24);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<Number> tmp34;
    compiler::TNode<Smi> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Object> tmp38;
    ca_.Bind(&block10, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.Goto(&block8, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Number> tmp41;
    compiler::TNode<Number> tmp42;
    compiler::TNode<Smi> tmp43;
    compiler::TNode<Smi> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<JSArray> tmp47;
    ca_.Bind(&block9, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 100);
    compiler::TNode<Smi> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp43}, compiler::TNode<Smi>{tmp44}));
    compiler::TNode<Smi> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadFastJSArrayLength(compiler::TNode<JSArray>{tmp47}));
    compiler::TNode<BoolT> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiAbove(compiler::TNode<Smi>{tmp48}, compiler::TNode<Smi>{tmp49}));
    ca_.Branch(tmp50, &block11, &block12, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp47);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<Smi> tmp55;
    compiler::TNode<Smi> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<JSArray> tmp58;
    ca_.Bind(&block11, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    ca_.Goto(&block1);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<Smi> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<JSArray> tmp66;
    ca_.Bind(&block12, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 101);
    compiler::TNode<JSArray> tmp67;
    tmp67 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kExtractFastJSArray, tmp59, tmp66, tmp63, tmp64));
    USE(tmp67);
    ca_.Goto(&block2, tmp59, tmp60, tmp61, tmp62, tmp67);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Number> tmp70;
    compiler::TNode<Number> tmp71;
    compiler::TNode<Smi> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<Object> tmp74;
    ca_.Bind(&block8, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 103);
    compiler::TNode<JSArgumentsObjectWithLength> tmp75;
    USE(tmp75);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp75 = BaseBuiltinsFromDSLAssembler(state_).Cast27JSArgumentsObjectWithLength(compiler::TNode<Context>{tmp68}, compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp74)}, &label0);
    ca_.Goto(&block15, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, ca_.UncheckedCast<Object>(tmp74), tmp75);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block16, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, ca_.UncheckedCast<Object>(tmp74));
    }
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<Number> tmp79;
    compiler::TNode<Smi> tmp80;
    compiler::TNode<Smi> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Object> tmp83;
    ca_.Bind(&block16, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83);
    ca_.Goto(&block14, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<Smi> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<JSArgumentsObjectWithLength> tmp92;
    ca_.Bind(&block15, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 104);
    compiler::TNode<Context> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<Context>(CodeStubAssembler(state_).LoadNativeContext(compiler::TNode<Context>{tmp84}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 105);
    compiler::TNode<IntPtrT> tmp94 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp94);
    compiler::TNode<Map>tmp95 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp92, tmp94});
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 106);
    compiler::TNode<BoolT> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsFastAliasedArgumentsMap(compiler::TNode<Context>{tmp84}, compiler::TNode<Map>{tmp95}));
    ca_.Branch(tmp96, &block17, &block18, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp92, tmp93, tmp95);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Smi> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<JSArgumentsObjectWithLength> tmp104;
    compiler::TNode<Context> tmp105;
    compiler::TNode<Map> tmp106;
    ca_.Bind(&block17, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 107);
    compiler::TNode<JSArray> tmp107;
    USE(tmp107);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp107 = ArraySliceBuiltinsFromDSLAssembler(state_).HandleFastAliasedSloppyArgumentsSlice(compiler::TNode<Context>{tmp97}, compiler::TNode<JSArgumentsObjectWithLength>{tmp104}, compiler::TNode<Smi>{tmp101}, compiler::TNode<Smi>{tmp102}, &label0);
    ca_.Goto(&block20, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp97, tmp104, tmp101, tmp102, tmp107);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block21, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp97, tmp104, tmp101, tmp102);
    }
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp108;
    compiler::TNode<Object> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<Smi> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<JSArgumentsObjectWithLength> tmp115;
    compiler::TNode<Context> tmp116;
    compiler::TNode<Map> tmp117;
    compiler::TNode<Context> tmp118;
    compiler::TNode<JSArgumentsObjectWithLength> tmp119;
    compiler::TNode<Smi> tmp120;
    compiler::TNode<Smi> tmp121;
    ca_.Bind(&block21, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121);
    ca_.Goto(&block1);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Number> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Object> tmp128;
    compiler::TNode<JSArgumentsObjectWithLength> tmp129;
    compiler::TNode<Context> tmp130;
    compiler::TNode<Map> tmp131;
    compiler::TNode<Context> tmp132;
    compiler::TNode<JSArgumentsObjectWithLength> tmp133;
    compiler::TNode<Smi> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<JSArray> tmp136;
    ca_.Bind(&block20, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136);
    ca_.Goto(&block2, tmp122, tmp123, tmp124, tmp125, tmp136);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<Number> tmp139;
    compiler::TNode<Number> tmp140;
    compiler::TNode<Smi> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<JSArgumentsObjectWithLength> tmp144;
    compiler::TNode<Context> tmp145;
    compiler::TNode<Map> tmp146;
    ca_.Bind(&block18, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 109);
    compiler::TNode<BoolT> tmp147;
    USE(tmp147);
    tmp147 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsStrictArgumentsMap(compiler::TNode<Context>{tmp137}, compiler::TNode<Map>{tmp146}));
    ca_.Branch(tmp147, &block22, &block24, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp148;
    compiler::TNode<Object> tmp149;
    compiler::TNode<Number> tmp150;
    compiler::TNode<Number> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<JSArgumentsObjectWithLength> tmp155;
    compiler::TNode<Context> tmp156;
    compiler::TNode<Map> tmp157;
    ca_.Bind(&block24, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157);
    compiler::TNode<BoolT> tmp158;
    USE(tmp158);
    tmp158 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsSloppyArgumentsMap(compiler::TNode<Context>{tmp148}, compiler::TNode<Map>{tmp157}));
    ca_.Branch(tmp158, &block22, &block23, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp159;
    compiler::TNode<Object> tmp160;
    compiler::TNode<Number> tmp161;
    compiler::TNode<Number> tmp162;
    compiler::TNode<Smi> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Object> tmp165;
    compiler::TNode<JSArgumentsObjectWithLength> tmp166;
    compiler::TNode<Context> tmp167;
    compiler::TNode<Map> tmp168;
    ca_.Bind(&block22, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 110);
    compiler::TNode<JSArray> tmp169;
    USE(tmp169);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp169 = ArraySliceBuiltinsFromDSLAssembler(state_).HandleSimpleArgumentsSlice(compiler::TNode<Context>{tmp159}, compiler::TNode<JSArgumentsObjectWithLength>{tmp166}, compiler::TNode<Smi>{tmp163}, compiler::TNode<Smi>{tmp164}, &label0);
    ca_.Goto(&block25, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp159, tmp166, tmp163, tmp164, tmp169);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block26, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp159, tmp166, tmp163, tmp164);
    }
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<Object> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<Number> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<Object> tmp176;
    compiler::TNode<JSArgumentsObjectWithLength> tmp177;
    compiler::TNode<Context> tmp178;
    compiler::TNode<Map> tmp179;
    compiler::TNode<Context> tmp180;
    compiler::TNode<JSArgumentsObjectWithLength> tmp181;
    compiler::TNode<Smi> tmp182;
    compiler::TNode<Smi> tmp183;
    ca_.Bind(&block26, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183);
    ca_.Goto(&block1);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp184;
    compiler::TNode<Object> tmp185;
    compiler::TNode<Number> tmp186;
    compiler::TNode<Number> tmp187;
    compiler::TNode<Smi> tmp188;
    compiler::TNode<Smi> tmp189;
    compiler::TNode<Object> tmp190;
    compiler::TNode<JSArgumentsObjectWithLength> tmp191;
    compiler::TNode<Context> tmp192;
    compiler::TNode<Map> tmp193;
    compiler::TNode<Context> tmp194;
    compiler::TNode<JSArgumentsObjectWithLength> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<Smi> tmp197;
    compiler::TNode<JSArray> tmp198;
    ca_.Bind(&block25, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198);
    ca_.Goto(&block2, tmp184, tmp185, tmp186, tmp187, tmp198);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp199;
    compiler::TNode<Object> tmp200;
    compiler::TNode<Number> tmp201;
    compiler::TNode<Number> tmp202;
    compiler::TNode<Smi> tmp203;
    compiler::TNode<Smi> tmp204;
    compiler::TNode<Object> tmp205;
    compiler::TNode<JSArgumentsObjectWithLength> tmp206;
    compiler::TNode<Context> tmp207;
    compiler::TNode<Map> tmp208;
    ca_.Bind(&block23, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 106);
    ca_.Goto(&block19, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp209;
    compiler::TNode<Object> tmp210;
    compiler::TNode<Number> tmp211;
    compiler::TNode<Number> tmp212;
    compiler::TNode<Smi> tmp213;
    compiler::TNode<Smi> tmp214;
    compiler::TNode<Object> tmp215;
    compiler::TNode<JSArgumentsObjectWithLength> tmp216;
    compiler::TNode<Context> tmp217;
    compiler::TNode<Map> tmp218;
    ca_.Bind(&block19, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 103);
    ca_.Goto(&block13, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp219;
    compiler::TNode<Object> tmp220;
    compiler::TNode<Number> tmp221;
    compiler::TNode<Number> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<Smi> tmp224;
    compiler::TNode<Object> tmp225;
    ca_.Bind(&block14, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 114);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 103);
    ca_.Goto(&block13, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<Number> tmp228;
    compiler::TNode<Number> tmp229;
    compiler::TNode<Smi> tmp230;
    compiler::TNode<Smi> tmp231;
    compiler::TNode<Object> tmp232;
    ca_.Bind(&block13, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 93);
    ca_.Goto(&block7, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp233;
    compiler::TNode<Object> tmp234;
    compiler::TNode<Number> tmp235;
    compiler::TNode<Number> tmp236;
    compiler::TNode<Smi> tmp237;
    compiler::TNode<Smi> tmp238;
    compiler::TNode<Object> tmp239;
    ca_.Bind(&block7, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 92);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 117);
    ca_.Goto(&block1);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp240;
    compiler::TNode<Object> tmp241;
    compiler::TNode<Number> tmp242;
    compiler::TNode<Number> tmp243;
    compiler::TNode<JSArray> tmp244;
    ca_.Bind(&block2, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 84);
    ca_.Goto(&block27, tmp240, tmp241, tmp242, tmp243, tmp244);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp245;
    compiler::TNode<Object> tmp246;
    compiler::TNode<Number> tmp247;
    compiler::TNode<Number> tmp248;
    compiler::TNode<JSArray> tmp249;
    ca_.Bind(&block27, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249);
  return compiler::TNode<JSArray>{tmp249};
}

TF_BUILTIN(ArrayPrototypeSlice, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Context, JSReceiver, Number, Number> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Context, JSReceiver, Number, Number, JSArray> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number, Number, Number> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number, Number, Number> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number, Number, Oddball> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number, Number, Oddball> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 125);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 126);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 127);
    compiler::TNode<JSArray> tmp12;
    USE(tmp12);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp12 = BaseBuiltinsFromDSLAssembler(state_).Cast20ATFastJSArrayForCopy(compiler::TNode<Context>{tmp7}, compiler::TNode<Object>{tmp8}, &label0);
    ca_.Goto(&block5, tmp7, tmp8, tmp9, tmp10, tmp11, tmp8, tmp8, tmp12);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp8, tmp8);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<RawPtrT> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<IntPtrT> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    ca_.Bind(&block6, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block4, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<JSArray> tmp27;
    ca_.Bind(&block5, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 128);
    compiler::TNode<JSArray> tmp28;
    tmp28 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCloneFastJSArray, tmp20, tmp27));
    USE(tmp28);
    arguments.PopAndReturn(tmp28);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<RawPtrT> tmp31;
    compiler::TNode<RawPtrT> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    compiler::TNode<Object> tmp34;
    ca_.Bind(&block4, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 130);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 127);
    ca_.Goto(&block3, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<RawPtrT> tmp38;
    compiler::TNode<IntPtrT> tmp39;
    compiler::TNode<Object> tmp40;
    ca_.Bind(&block3, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 126);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 125);
    ca_.Goto(&block2, tmp35, tmp36, tmp37, tmp38, tmp39);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<RawPtrT> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<IntPtrT> tmp45;
    ca_.Bind(&block2, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 136);
    compiler::TNode<JSReceiver> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp41}, compiler::TNode<Object>{tmp42}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 139);
    compiler::TNode<Number> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp41}, compiler::TNode<Object>{tmp46}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 142);
    compiler::TNode<IntPtrT> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp43}, compiler::TNode<RawPtrT>{tmp44}, compiler::TNode<IntPtrT>{tmp45}}, compiler::TNode<IntPtrT>{tmp48}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 143);
    compiler::TNode<Number> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp41}, compiler::TNode<Object>{tmp49}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 147);
    compiler::TNode<Number> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp50}, compiler::TNode<Number>{tmp51}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block11, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp49, tmp50, tmp50);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block12, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp49, tmp50, tmp50);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<RawPtrT> tmp54;
    compiler::TNode<RawPtrT> tmp55;
    compiler::TNode<IntPtrT> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Number> tmp60;
    compiler::TNode<Number> tmp61;
    ca_.Bind(&block11, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.Goto(&block7, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<RawPtrT> tmp64;
    compiler::TNode<RawPtrT> tmp65;
    compiler::TNode<IntPtrT> tmp66;
    compiler::TNode<JSReceiver> tmp67;
    compiler::TNode<Number> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Number> tmp70;
    compiler::TNode<Number> tmp71;
    ca_.Bind(&block12, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    ca_.Goto(&block8, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<RawPtrT> tmp74;
    compiler::TNode<RawPtrT> tmp75;
    compiler::TNode<IntPtrT> tmp76;
    compiler::TNode<JSReceiver> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Number> tmp80;
    ca_.Bind(&block7, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    compiler::TNode<Number> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp78}, compiler::TNode<Number>{tmp80}));
    compiler::TNode<Number> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp81}, compiler::TNode<Number>{tmp82}));
    ca_.Goto(&block10, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp83);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<RawPtrT> tmp86;
    compiler::TNode<RawPtrT> tmp87;
    compiler::TNode<IntPtrT> tmp88;
    compiler::TNode<JSReceiver> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Number> tmp92;
    ca_.Bind(&block8, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 148);
    compiler::TNode<Number> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp92}, compiler::TNode<Number>{tmp90}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 147);
    ca_.Goto(&block9, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<RawPtrT> tmp96;
    compiler::TNode<RawPtrT> tmp97;
    compiler::TNode<IntPtrT> tmp98;
    compiler::TNode<JSReceiver> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<Number> tmp102;
    compiler::TNode<Number> tmp103;
    ca_.Bind(&block10, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103);
    ca_.Goto(&block9, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<RawPtrT> tmp106;
    compiler::TNode<RawPtrT> tmp107;
    compiler::TNode<IntPtrT> tmp108;
    compiler::TNode<JSReceiver> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Number> tmp113;
    ca_.Bind(&block9, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 152);
    compiler::TNode<IntPtrT> tmp114;
    USE(tmp114);
    tmp114 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp115;
    USE(tmp115);
    tmp115 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp106}, compiler::TNode<RawPtrT>{tmp107}, compiler::TNode<IntPtrT>{tmp108}}, compiler::TNode<IntPtrT>{tmp114}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 154);
    compiler::TNode<Oddball> tmp116;
    USE(tmp116);
    tmp116 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp117;
    USE(tmp117);
    tmp117 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp115}, compiler::TNode<HeapObject>{tmp116}));
    ca_.Branch(tmp117, &block13, &block14, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp115);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<RawPtrT> tmp120;
    compiler::TNode<RawPtrT> tmp121;
    compiler::TNode<IntPtrT> tmp122;
    compiler::TNode<JSReceiver> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<Number> tmp126;
    compiler::TNode<Number> tmp127;
    compiler::TNode<Object> tmp128;
    ca_.Bind(&block13, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128);
    ca_.Goto(&block16, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp124);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<RawPtrT> tmp131;
    compiler::TNode<RawPtrT> tmp132;
    compiler::TNode<IntPtrT> tmp133;
    compiler::TNode<JSReceiver> tmp134;
    compiler::TNode<Number> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<Number> tmp137;
    compiler::TNode<Number> tmp138;
    compiler::TNode<Object> tmp139;
    ca_.Bind(&block14, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139);
    compiler::TNode<Number> tmp140;
    USE(tmp140);
    tmp140 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp129}, compiler::TNode<Object>{tmp139}));
    ca_.Goto(&block15, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<RawPtrT> tmp143;
    compiler::TNode<RawPtrT> tmp144;
    compiler::TNode<IntPtrT> tmp145;
    compiler::TNode<JSReceiver> tmp146;
    compiler::TNode<Number> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<Number> tmp149;
    compiler::TNode<Number> tmp150;
    compiler::TNode<Object> tmp151;
    compiler::TNode<Number> tmp152;
    ca_.Bind(&block16, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152);
    ca_.Goto(&block15, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<RawPtrT> tmp155;
    compiler::TNode<RawPtrT> tmp156;
    compiler::TNode<IntPtrT> tmp157;
    compiler::TNode<JSReceiver> tmp158;
    compiler::TNode<Number> tmp159;
    compiler::TNode<Object> tmp160;
    compiler::TNode<Number> tmp161;
    compiler::TNode<Number> tmp162;
    compiler::TNode<Object> tmp163;
    compiler::TNode<Number> tmp164;
    ca_.Bind(&block15, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 153);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 159);
    compiler::TNode<Number> tmp165;
    USE(tmp165);
    tmp165 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp164}, compiler::TNode<Number>{tmp165}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block21, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp164);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block22, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp164);
    }
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp166;
    compiler::TNode<Object> tmp167;
    compiler::TNode<RawPtrT> tmp168;
    compiler::TNode<RawPtrT> tmp169;
    compiler::TNode<IntPtrT> tmp170;
    compiler::TNode<JSReceiver> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<Number> tmp175;
    compiler::TNode<Object> tmp176;
    compiler::TNode<Number> tmp177;
    compiler::TNode<Number> tmp178;
    ca_.Bind(&block21, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178);
    ca_.Goto(&block17, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp179;
    compiler::TNode<Object> tmp180;
    compiler::TNode<RawPtrT> tmp181;
    compiler::TNode<RawPtrT> tmp182;
    compiler::TNode<IntPtrT> tmp183;
    compiler::TNode<JSReceiver> tmp184;
    compiler::TNode<Number> tmp185;
    compiler::TNode<Object> tmp186;
    compiler::TNode<Number> tmp187;
    compiler::TNode<Number> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<Number> tmp190;
    compiler::TNode<Number> tmp191;
    ca_.Bind(&block22, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191);
    ca_.Goto(&block18, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp192;
    compiler::TNode<Object> tmp193;
    compiler::TNode<RawPtrT> tmp194;
    compiler::TNode<RawPtrT> tmp195;
    compiler::TNode<IntPtrT> tmp196;
    compiler::TNode<JSReceiver> tmp197;
    compiler::TNode<Number> tmp198;
    compiler::TNode<Object> tmp199;
    compiler::TNode<Number> tmp200;
    compiler::TNode<Number> tmp201;
    compiler::TNode<Object> tmp202;
    compiler::TNode<Number> tmp203;
    ca_.Bind(&block17, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203);
    compiler::TNode<Number> tmp204;
    USE(tmp204);
    tmp204 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp198}, compiler::TNode<Number>{tmp203}));
    compiler::TNode<Number> tmp205;
    USE(tmp205);
    tmp205 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp206;
    USE(tmp206);
    tmp206 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp204}, compiler::TNode<Number>{tmp205}));
    ca_.Goto(&block20, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp206);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp207;
    compiler::TNode<Object> tmp208;
    compiler::TNode<RawPtrT> tmp209;
    compiler::TNode<RawPtrT> tmp210;
    compiler::TNode<IntPtrT> tmp211;
    compiler::TNode<JSReceiver> tmp212;
    compiler::TNode<Number> tmp213;
    compiler::TNode<Object> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<Number> tmp216;
    compiler::TNode<Object> tmp217;
    compiler::TNode<Number> tmp218;
    ca_.Bind(&block18, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218);
    compiler::TNode<Number> tmp219;
    USE(tmp219);
    tmp219 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp218}, compiler::TNode<Number>{tmp213}));
    ca_.Goto(&block19, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp220;
    compiler::TNode<Object> tmp221;
    compiler::TNode<RawPtrT> tmp222;
    compiler::TNode<RawPtrT> tmp223;
    compiler::TNode<IntPtrT> tmp224;
    compiler::TNode<JSReceiver> tmp225;
    compiler::TNode<Number> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<Number> tmp228;
    compiler::TNode<Number> tmp229;
    compiler::TNode<Object> tmp230;
    compiler::TNode<Number> tmp231;
    compiler::TNode<Number> tmp232;
    ca_.Bind(&block20, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232);
    ca_.Goto(&block19, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp233;
    compiler::TNode<Object> tmp234;
    compiler::TNode<RawPtrT> tmp235;
    compiler::TNode<RawPtrT> tmp236;
    compiler::TNode<IntPtrT> tmp237;
    compiler::TNode<JSReceiver> tmp238;
    compiler::TNode<Number> tmp239;
    compiler::TNode<Object> tmp240;
    compiler::TNode<Number> tmp241;
    compiler::TNode<Number> tmp242;
    compiler::TNode<Object> tmp243;
    compiler::TNode<Number> tmp244;
    compiler::TNode<Number> tmp245;
    ca_.Bind(&block19, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 158);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 162);
    compiler::TNode<Number> tmp246;
    USE(tmp246);
    tmp246 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp245}, compiler::TNode<Number>{tmp242}));
    compiler::TNode<Number> tmp247;
    USE(tmp247);
    tmp247 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp248;
    USE(tmp248);
    tmp248 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp246}, compiler::TNode<Number>{tmp247}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 172);
    compiler::TNode<JSArray> tmp249;
    USE(tmp249);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp249 = ArraySliceBuiltinsFromDSLAssembler(state_).HandleFastSlice(compiler::TNode<Context>{tmp233}, compiler::TNode<Object>{tmp238}, compiler::TNode<Number>{tmp242}, compiler::TNode<Number>{tmp248}, &label0);
    ca_.Goto(&block25, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp248, tmp233, tmp238, tmp242, tmp248, tmp249);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block26, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp248, tmp233, tmp238, tmp242, tmp248);
    }
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp250;
    compiler::TNode<Object> tmp251;
    compiler::TNode<RawPtrT> tmp252;
    compiler::TNode<RawPtrT> tmp253;
    compiler::TNode<IntPtrT> tmp254;
    compiler::TNode<JSReceiver> tmp255;
    compiler::TNode<Number> tmp256;
    compiler::TNode<Object> tmp257;
    compiler::TNode<Number> tmp258;
    compiler::TNode<Number> tmp259;
    compiler::TNode<Object> tmp260;
    compiler::TNode<Number> tmp261;
    compiler::TNode<Number> tmp262;
    compiler::TNode<Number> tmp263;
    compiler::TNode<Context> tmp264;
    compiler::TNode<JSReceiver> tmp265;
    compiler::TNode<Number> tmp266;
    compiler::TNode<Number> tmp267;
    ca_.Bind(&block26, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267);
    ca_.Goto(&block24, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp268;
    compiler::TNode<Object> tmp269;
    compiler::TNode<RawPtrT> tmp270;
    compiler::TNode<RawPtrT> tmp271;
    compiler::TNode<IntPtrT> tmp272;
    compiler::TNode<JSReceiver> tmp273;
    compiler::TNode<Number> tmp274;
    compiler::TNode<Object> tmp275;
    compiler::TNode<Number> tmp276;
    compiler::TNode<Number> tmp277;
    compiler::TNode<Object> tmp278;
    compiler::TNode<Number> tmp279;
    compiler::TNode<Number> tmp280;
    compiler::TNode<Number> tmp281;
    compiler::TNode<Context> tmp282;
    compiler::TNode<JSReceiver> tmp283;
    compiler::TNode<Number> tmp284;
    compiler::TNode<Number> tmp285;
    compiler::TNode<JSArray> tmp286;
    ca_.Bind(&block25, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286);
    arguments.PopAndReturn(tmp286);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp287;
    compiler::TNode<Object> tmp288;
    compiler::TNode<RawPtrT> tmp289;
    compiler::TNode<RawPtrT> tmp290;
    compiler::TNode<IntPtrT> tmp291;
    compiler::TNode<JSReceiver> tmp292;
    compiler::TNode<Number> tmp293;
    compiler::TNode<Object> tmp294;
    compiler::TNode<Number> tmp295;
    compiler::TNode<Number> tmp296;
    compiler::TNode<Object> tmp297;
    compiler::TNode<Number> tmp298;
    compiler::TNode<Number> tmp299;
    compiler::TNode<Number> tmp300;
    ca_.Bind(&block24, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 171);
    ca_.Goto(&block23, tmp287, tmp288, tmp289, tmp290, tmp291, tmp292, tmp293, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp301;
    compiler::TNode<Object> tmp302;
    compiler::TNode<RawPtrT> tmp303;
    compiler::TNode<RawPtrT> tmp304;
    compiler::TNode<IntPtrT> tmp305;
    compiler::TNode<JSReceiver> tmp306;
    compiler::TNode<Number> tmp307;
    compiler::TNode<Object> tmp308;
    compiler::TNode<Number> tmp309;
    compiler::TNode<Number> tmp310;
    compiler::TNode<Object> tmp311;
    compiler::TNode<Number> tmp312;
    compiler::TNode<Number> tmp313;
    compiler::TNode<Number> tmp314;
    ca_.Bind(&block23, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 177);
    compiler::TNode<JSReceiver> tmp315;
    USE(tmp315);
    tmp315 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ArraySpeciesCreate(compiler::TNode<Context>{tmp301}, compiler::TNode<Object>{tmp306}, compiler::TNode<Number>{tmp314}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 180);
    compiler::TNode<Number> tmp316;
    USE(tmp316);
    tmp316 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 183);
    ca_.Goto(&block29, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311, tmp312, tmp313, tmp314, tmp315, tmp316);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp317;
    compiler::TNode<Object> tmp318;
    compiler::TNode<RawPtrT> tmp319;
    compiler::TNode<RawPtrT> tmp320;
    compiler::TNode<IntPtrT> tmp321;
    compiler::TNode<JSReceiver> tmp322;
    compiler::TNode<Number> tmp323;
    compiler::TNode<Object> tmp324;
    compiler::TNode<Number> tmp325;
    compiler::TNode<Number> tmp326;
    compiler::TNode<Object> tmp327;
    compiler::TNode<Number> tmp328;
    compiler::TNode<Number> tmp329;
    compiler::TNode<Number> tmp330;
    compiler::TNode<JSReceiver> tmp331;
    compiler::TNode<Number> tmp332;
    ca_.Bind(&block29, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp326}, compiler::TNode<Number>{tmp329}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block30, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp326, tmp329);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block31, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp326, tmp329);
    }
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp333;
    compiler::TNode<Object> tmp334;
    compiler::TNode<RawPtrT> tmp335;
    compiler::TNode<RawPtrT> tmp336;
    compiler::TNode<IntPtrT> tmp337;
    compiler::TNode<JSReceiver> tmp338;
    compiler::TNode<Number> tmp339;
    compiler::TNode<Object> tmp340;
    compiler::TNode<Number> tmp341;
    compiler::TNode<Number> tmp342;
    compiler::TNode<Object> tmp343;
    compiler::TNode<Number> tmp344;
    compiler::TNode<Number> tmp345;
    compiler::TNode<Number> tmp346;
    compiler::TNode<JSReceiver> tmp347;
    compiler::TNode<Number> tmp348;
    compiler::TNode<Number> tmp349;
    compiler::TNode<Number> tmp350;
    ca_.Bind(&block30, &tmp333, &tmp334, &tmp335, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350);
    ca_.Goto(&block27, tmp333, tmp334, tmp335, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp351;
    compiler::TNode<Object> tmp352;
    compiler::TNode<RawPtrT> tmp353;
    compiler::TNode<RawPtrT> tmp354;
    compiler::TNode<IntPtrT> tmp355;
    compiler::TNode<JSReceiver> tmp356;
    compiler::TNode<Number> tmp357;
    compiler::TNode<Object> tmp358;
    compiler::TNode<Number> tmp359;
    compiler::TNode<Number> tmp360;
    compiler::TNode<Object> tmp361;
    compiler::TNode<Number> tmp362;
    compiler::TNode<Number> tmp363;
    compiler::TNode<Number> tmp364;
    compiler::TNode<JSReceiver> tmp365;
    compiler::TNode<Number> tmp366;
    compiler::TNode<Number> tmp367;
    compiler::TNode<Number> tmp368;
    ca_.Bind(&block31, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368);
    ca_.Goto(&block28, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, tmp357, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp369;
    compiler::TNode<Object> tmp370;
    compiler::TNode<RawPtrT> tmp371;
    compiler::TNode<RawPtrT> tmp372;
    compiler::TNode<IntPtrT> tmp373;
    compiler::TNode<JSReceiver> tmp374;
    compiler::TNode<Number> tmp375;
    compiler::TNode<Object> tmp376;
    compiler::TNode<Number> tmp377;
    compiler::TNode<Number> tmp378;
    compiler::TNode<Object> tmp379;
    compiler::TNode<Number> tmp380;
    compiler::TNode<Number> tmp381;
    compiler::TNode<Number> tmp382;
    compiler::TNode<JSReceiver> tmp383;
    compiler::TNode<Number> tmp384;
    ca_.Bind(&block27, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 185);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 188);
    compiler::TNode<Oddball> tmp385;
    tmp385 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp369, tmp374, tmp378));
    USE(tmp385);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 191);
    compiler::TNode<Oddball> tmp386;
    USE(tmp386);
    tmp386 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp387;
    USE(tmp387);
    tmp387 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp385}, compiler::TNode<HeapObject>{tmp386}));
    ca_.Branch(tmp387, &block32, &block33, tmp369, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384, tmp378, tmp385);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp388;
    compiler::TNode<Object> tmp389;
    compiler::TNode<RawPtrT> tmp390;
    compiler::TNode<RawPtrT> tmp391;
    compiler::TNode<IntPtrT> tmp392;
    compiler::TNode<JSReceiver> tmp393;
    compiler::TNode<Number> tmp394;
    compiler::TNode<Object> tmp395;
    compiler::TNode<Number> tmp396;
    compiler::TNode<Number> tmp397;
    compiler::TNode<Object> tmp398;
    compiler::TNode<Number> tmp399;
    compiler::TNode<Number> tmp400;
    compiler::TNode<Number> tmp401;
    compiler::TNode<JSReceiver> tmp402;
    compiler::TNode<Number> tmp403;
    compiler::TNode<Number> tmp404;
    compiler::TNode<Oddball> tmp405;
    ca_.Bind(&block32, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 193);
    compiler::TNode<Object> tmp406;
    USE(tmp406);
    tmp406 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp388}, compiler::TNode<Object>{tmp393}, compiler::TNode<Object>{tmp404}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 196);
    compiler::TNode<Object> tmp407;
    tmp407 = CodeStubAssembler(state_).CallBuiltin(Builtins::kFastCreateDataProperty, tmp388, tmp402, tmp403, tmp406);
    USE(tmp407);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 191);
    ca_.Goto(&block33, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp396, tmp397, tmp398, tmp399, tmp400, tmp401, tmp402, tmp403, tmp404, tmp405);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp408;
    compiler::TNode<Object> tmp409;
    compiler::TNode<RawPtrT> tmp410;
    compiler::TNode<RawPtrT> tmp411;
    compiler::TNode<IntPtrT> tmp412;
    compiler::TNode<JSReceiver> tmp413;
    compiler::TNode<Number> tmp414;
    compiler::TNode<Object> tmp415;
    compiler::TNode<Number> tmp416;
    compiler::TNode<Number> tmp417;
    compiler::TNode<Object> tmp418;
    compiler::TNode<Number> tmp419;
    compiler::TNode<Number> tmp420;
    compiler::TNode<Number> tmp421;
    compiler::TNode<JSReceiver> tmp422;
    compiler::TNode<Number> tmp423;
    compiler::TNode<Number> tmp424;
    compiler::TNode<Oddball> tmp425;
    ca_.Bind(&block33, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420, &tmp421, &tmp422, &tmp423, &tmp424, &tmp425);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 200);
    compiler::TNode<Number> tmp426;
    USE(tmp426);
    tmp426 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp427;
    USE(tmp427);
    tmp427 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp417}, compiler::TNode<Number>{tmp426}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 203);
    compiler::TNode<Number> tmp428;
    USE(tmp428);
    tmp428 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp429;
    USE(tmp429);
    tmp429 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp423}, compiler::TNode<Number>{tmp428}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 183);
    ca_.Goto(&block29, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp427, tmp418, tmp419, tmp420, tmp421, tmp422, tmp429);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp430;
    compiler::TNode<Object> tmp431;
    compiler::TNode<RawPtrT> tmp432;
    compiler::TNode<RawPtrT> tmp433;
    compiler::TNode<IntPtrT> tmp434;
    compiler::TNode<JSReceiver> tmp435;
    compiler::TNode<Number> tmp436;
    compiler::TNode<Object> tmp437;
    compiler::TNode<Number> tmp438;
    compiler::TNode<Number> tmp439;
    compiler::TNode<Object> tmp440;
    compiler::TNode<Number> tmp441;
    compiler::TNode<Number> tmp442;
    compiler::TNode<Number> tmp443;
    compiler::TNode<JSReceiver> tmp444;
    compiler::TNode<Number> tmp445;
    ca_.Bind(&block28, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442, &tmp443, &tmp444, &tmp445);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 207);
    compiler::TNode<String> tmp446;
    USE(tmp446);
    tmp446 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp430, tmp444, tmp446, tmp445);
    ca_.SetSourcePosition("../../v8/src/builtins/array-slice.tq", 210);
    arguments.PopAndReturn(tmp444);
  }
}

}  // namespace internal
}  // namespace v8

