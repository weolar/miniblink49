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
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 11);
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
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 13);
    compiler::TNode<Smi> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp12}, compiler::TNode<Smi>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 15);
    compiler::TNode<FixedArrayBase> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp11}));
    compiler::TNode<FixedArray> tmp16;
    USE(tmp16);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp16 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<HeapObject>{tmp15}, &label0);
    ca_.Goto(&block5, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSArgumentsObjectWithLength> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<Smi> tmp21;
    compiler::TNode<FixedArrayBase> tmp22;
    ca_.Bind(&block6, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSArgumentsObjectWithLength> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<FixedArrayBase> tmp28;
    compiler::TNode<FixedArray> tmp29;
    ca_.Bind(&block5, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 14);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 16);
    compiler::TNode<Smi> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp29}));
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiAbove(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp30}));
    ca_.Branch(tmp31, &block7, &block8, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSArgumentsObjectWithLength> tmp33;
    compiler::TNode<Smi> tmp34;
    compiler::TNode<Smi> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<FixedArray> tmp37;
    ca_.Bind(&block7, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    ca_.Goto(&block1);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<JSArgumentsObjectWithLength> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<FixedArray> tmp43;
    ca_.Bind(&block8, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 18);
    compiler::TNode<Map> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Map>(CodeStubAssembler(state_).LoadJSArrayElementsMap(HOLEY_ELEMENTS, compiler::TNode<Context>{tmp38}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 20);
    compiler::TNode<JSArray> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<JSArray>(CodeStubAssembler(state_).AllocateJSArray(HOLEY_ELEMENTS, compiler::TNode<Map>{tmp44}, compiler::TNode<Smi>{tmp41}, compiler::TNode<Smi>{tmp41}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 19);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 22);
    compiler::TNode<FixedArrayBase> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp45}));
    compiler::TNode<FixedArray> tmp47;
    USE(tmp47);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp47 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<HeapObject>{tmp46}, &label0);
    ca_.Goto(&block9, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<JSArgumentsObjectWithLength> tmp49;
    compiler::TNode<Smi> tmp50;
    compiler::TNode<Smi> tmp51;
    compiler::TNode<Smi> tmp52;
    compiler::TNode<FixedArray> tmp53;
    compiler::TNode<Map> tmp54;
    compiler::TNode<JSArray> tmp55;
    compiler::TNode<FixedArrayBase> tmp56;
    ca_.Bind(&block10, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
    ca_.Goto(&block1);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp57;
    compiler::TNode<JSArgumentsObjectWithLength> tmp58;
    compiler::TNode<Smi> tmp59;
    compiler::TNode<Smi> tmp60;
    compiler::TNode<Smi> tmp61;
    compiler::TNode<FixedArray> tmp62;
    compiler::TNode<Map> tmp63;
    compiler::TNode<JSArray> tmp64;
    compiler::TNode<FixedArrayBase> tmp65;
    compiler::TNode<FixedArray> tmp66;
    ca_.Bind(&block9, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 21);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 24);
    compiler::TNode<IntPtrT> tmp67;
    USE(tmp67);
    tmp67 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp59}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 25);
    compiler::TNode<IntPtrT> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp60}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 23);
    compiler::TNode<IntPtrT> tmp69;
    USE(tmp69);
    tmp69 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    CodeStubAssembler(state_).CopyElements(PACKED_ELEMENTS, compiler::TNode<FixedArrayBase>{tmp66}, compiler::TNode<IntPtrT>{tmp69}, compiler::TNode<FixedArrayBase>{tmp62}, compiler::TNode<IntPtrT>{tmp67}, compiler::TNode<IntPtrT>{tmp68});
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 26);
    ca_.Goto(&block2, tmp57, tmp58, tmp59, tmp60, tmp64);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<JSArgumentsObjectWithLength> tmp71;
    compiler::TNode<Smi> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<JSArray> tmp74;
    ca_.Bind(&block2, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 6);
    ca_.Goto(&block11, tmp70, tmp71, tmp72, tmp73, tmp74);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp75;
    compiler::TNode<JSArgumentsObjectWithLength> tmp76;
    compiler::TNode<Smi> tmp77;
    compiler::TNode<Smi> tmp78;
    compiler::TNode<JSArray> tmp79;
    ca_.Bind(&block11, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79);
  return compiler::TNode<JSArray>{tmp79};
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
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 34);
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
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 37);
    compiler::TNode<FixedArrayBase> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp11}));
    compiler::TNode<SloppyArgumentsElements> tmp15;
    USE(tmp15);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp15 = BaseBuiltinsFromDSLAssembler(state_).Cast23SloppyArgumentsElements(compiler::TNode<HeapObject>{tmp14}, &label0);
    ca_.Goto(&block5, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp10, tmp11, tmp12, tmp13, tmp14);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<JSArgumentsObjectWithLength> tmp17;
    compiler::TNode<Smi> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<FixedArrayBase> tmp20;
    ca_.Bind(&block6, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<JSArgumentsObjectWithLength> tmp22;
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Smi> tmp24;
    compiler::TNode<FixedArrayBase> tmp25;
    compiler::TNode<SloppyArgumentsElements> tmp26;
    ca_.Bind(&block5, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 36);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 38);
    compiler::TNode<Smi> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp26}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 40);
    compiler::TNode<Smi> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(SloppyArgumentsElements::kParameterMapStart));
    compiler::TNode<Smi> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp28}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 39);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 44);
    compiler::TNode<Smi> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp23}, compiler::TNode<Smi>{tmp24}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 46);
    compiler::TNode<Object> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp26}, SloppyArgumentsElements::kArgumentsIndex));
    compiler::TNode<FixedArray> tmp32;
    USE(tmp32);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp32 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<Context>{tmp21}, compiler::TNode<Object>{tmp31}, &label0);
    ca_.Goto(&block7, tmp21, tmp22, tmp23, tmp24, tmp26, tmp27, tmp29, tmp30, tmp31, tmp32);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp21, tmp22, tmp23, tmp24, tmp26, tmp27, tmp29, tmp30, tmp31);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<JSArgumentsObjectWithLength> tmp34;
    compiler::TNode<Smi> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<SloppyArgumentsElements> tmp37;
    compiler::TNode<Smi> tmp38;
    compiler::TNode<Smi> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<Object> tmp41;
    ca_.Bind(&block8, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.Goto(&block1);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<JSArgumentsObjectWithLength> tmp43;
    compiler::TNode<Smi> tmp44;
    compiler::TNode<Smi> tmp45;
    compiler::TNode<SloppyArgumentsElements> tmp46;
    compiler::TNode<Smi> tmp47;
    compiler::TNode<Smi> tmp48;
    compiler::TNode<Smi> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<FixedArray> tmp51;
    ca_.Bind(&block7, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 45);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 48);
    compiler::TNode<Smi> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp51}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 49);
    compiler::TNode<BoolT> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiAbove(compiler::TNode<Smi>{tmp49}, compiler::TNode<Smi>{tmp52}));
    ca_.Branch(tmp53, &block9, &block10, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp51, tmp52);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<JSArgumentsObjectWithLength> tmp55;
    compiler::TNode<Smi> tmp56;
    compiler::TNode<Smi> tmp57;
    compiler::TNode<SloppyArgumentsElements> tmp58;
    compiler::TNode<Smi> tmp59;
    compiler::TNode<Smi> tmp60;
    compiler::TNode<Smi> tmp61;
    compiler::TNode<FixedArray> tmp62;
    compiler::TNode<Smi> tmp63;
    ca_.Bind(&block9, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63);
    ca_.Goto(&block1);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<JSArgumentsObjectWithLength> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<Smi> tmp67;
    compiler::TNode<SloppyArgumentsElements> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<FixedArray> tmp72;
    compiler::TNode<Smi> tmp73;
    ca_.Bind(&block10, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 52);
    compiler::TNode<Object> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp68}, SloppyArgumentsElements::kContextIndex));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 51);
    compiler::TNode<Context> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<Context>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast9ATContext(compiler::TNode<Context>{tmp64}, compiler::TNode<Object>{tmp74}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 54);
    compiler::TNode<Map> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<Map>(CodeStubAssembler(state_).LoadJSArrayElementsMap(HOLEY_ELEMENTS, compiler::TNode<Context>{tmp64}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 56);
    compiler::TNode<JSArray> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<JSArray>(CodeStubAssembler(state_).AllocateJSArray(HOLEY_ELEMENTS, compiler::TNode<Map>{tmp76}, compiler::TNode<Smi>{tmp67}, compiler::TNode<Smi>{tmp67}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 55);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 58);
    compiler::TNode<Smi> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 59);
    compiler::TNode<FixedArrayBase> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp77}));
    compiler::TNode<FixedArray> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp64}, compiler::TNode<Object>{tmp79}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 60);
    compiler::TNode<Smi> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiMin(compiler::TNode<Smi>{tmp70}, compiler::TNode<Smi>{tmp71}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 63);
    ca_.Goto(&block13, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp75, tmp76, tmp77, tmp78, tmp80, tmp81, tmp66);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<JSArgumentsObjectWithLength> tmp83;
    compiler::TNode<Smi> tmp84;
    compiler::TNode<Smi> tmp85;
    compiler::TNode<SloppyArgumentsElements> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<Smi> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<FixedArray> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<Context> tmp92;
    compiler::TNode<Map> tmp93;
    compiler::TNode<JSArray> tmp94;
    compiler::TNode<Smi> tmp95;
    compiler::TNode<FixedArray> tmp96;
    compiler::TNode<Smi> tmp97;
    compiler::TNode<Smi> tmp98;
    ca_.Bind(&block13, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98);
    compiler::TNode<BoolT> tmp99;
    USE(tmp99);
    tmp99 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp98}, compiler::TNode<Smi>{tmp97}));
    ca_.Branch(tmp99, &block11, &block12, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp100;
    compiler::TNode<JSArgumentsObjectWithLength> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<Smi> tmp103;
    compiler::TNode<SloppyArgumentsElements> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Smi> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<FixedArray> tmp108;
    compiler::TNode<Smi> tmp109;
    compiler::TNode<Context> tmp110;
    compiler::TNode<Map> tmp111;
    compiler::TNode<JSArray> tmp112;
    compiler::TNode<Smi> tmp113;
    compiler::TNode<FixedArray> tmp114;
    compiler::TNode<Smi> tmp115;
    compiler::TNode<Smi> tmp116;
    ca_.Bind(&block11, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 65);
    compiler::TNode<Smi> tmp117;
    USE(tmp117);
    tmp117 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(SloppyArgumentsElements::kParameterMapStart));
    compiler::TNode<Smi> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp116}, compiler::TNode<Smi>{tmp117}));
    compiler::TNode<Object> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp104}, compiler::TNode<Smi>{tmp118}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 64);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 66);
    compiler::TNode<Oddball> tmp120;
    USE(tmp120);
    tmp120 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp121;
    USE(tmp121);
    tmp121 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp119}, compiler::TNode<HeapObject>{tmp120}));
    ca_.Branch(tmp121, &block15, &block16, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp119);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp122;
    compiler::TNode<JSArgumentsObjectWithLength> tmp123;
    compiler::TNode<Smi> tmp124;
    compiler::TNode<Smi> tmp125;
    compiler::TNode<SloppyArgumentsElements> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<FixedArray> tmp130;
    compiler::TNode<Smi> tmp131;
    compiler::TNode<Context> tmp132;
    compiler::TNode<Map> tmp133;
    compiler::TNode<JSArray> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<FixedArray> tmp136;
    compiler::TNode<Smi> tmp137;
    compiler::TNode<Smi> tmp138;
    compiler::TNode<Object> tmp139;
    ca_.Bind(&block15, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 67);
    compiler::TNode<Smi> tmp140;
    USE(tmp140);
    tmp140 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast5ATSmi(compiler::TNode<Context>{tmp122}, compiler::TNode<Object>{tmp139}));
    compiler::TNode<Object> tmp141;
    USE(tmp141);
    tmp141 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadContextElement(compiler::TNode<Context>{tmp132}, compiler::TNode<Smi>{tmp140}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 66);
    ca_.Goto(&block18, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp141);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp142;
    compiler::TNode<JSArgumentsObjectWithLength> tmp143;
    compiler::TNode<Smi> tmp144;
    compiler::TNode<Smi> tmp145;
    compiler::TNode<SloppyArgumentsElements> tmp146;
    compiler::TNode<Smi> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<Smi> tmp149;
    compiler::TNode<FixedArray> tmp150;
    compiler::TNode<Smi> tmp151;
    compiler::TNode<Context> tmp152;
    compiler::TNode<Map> tmp153;
    compiler::TNode<JSArray> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<FixedArray> tmp156;
    compiler::TNode<Smi> tmp157;
    compiler::TNode<Smi> tmp158;
    compiler::TNode<Object> tmp159;
    ca_.Bind(&block16, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 68);
    compiler::TNode<Object> tmp160;
    USE(tmp160);
    tmp160 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadFixedArrayElement(compiler::TNode<FixedArray>{tmp150}, compiler::TNode<Smi>{tmp158}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 66);
    ca_.Goto(&block17, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp161;
    compiler::TNode<JSArgumentsObjectWithLength> tmp162;
    compiler::TNode<Smi> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<SloppyArgumentsElements> tmp165;
    compiler::TNode<Smi> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<Smi> tmp168;
    compiler::TNode<FixedArray> tmp169;
    compiler::TNode<Smi> tmp170;
    compiler::TNode<Context> tmp171;
    compiler::TNode<Map> tmp172;
    compiler::TNode<JSArray> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<FixedArray> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<Object> tmp178;
    compiler::TNode<Object> tmp179;
    ca_.Bind(&block18, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179);
    ca_.Goto(&block17, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp180;
    compiler::TNode<JSArgumentsObjectWithLength> tmp181;
    compiler::TNode<Smi> tmp182;
    compiler::TNode<Smi> tmp183;
    compiler::TNode<SloppyArgumentsElements> tmp184;
    compiler::TNode<Smi> tmp185;
    compiler::TNode<Smi> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<FixedArray> tmp188;
    compiler::TNode<Smi> tmp189;
    compiler::TNode<Context> tmp190;
    compiler::TNode<Map> tmp191;
    compiler::TNode<JSArray> tmp192;
    compiler::TNode<Smi> tmp193;
    compiler::TNode<FixedArray> tmp194;
    compiler::TNode<Smi> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<Object> tmp197;
    compiler::TNode<Object> tmp198;
    ca_.Bind(&block17, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 70);
    compiler::TNode<Smi> tmp199;
    USE(tmp199);
    tmp199 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp200;
    USE(tmp200);
    tmp200 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp193}, compiler::TNode<Smi>{tmp199}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 69);
    CodeStubAssembler(state_).StoreFixedArrayElementSmi(compiler::TNode<FixedArray>{tmp194}, compiler::TNode<Smi>{tmp193}, compiler::TNode<Object>{tmp198}, SKIP_WRITE_BARRIER);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 63);
    ca_.Goto(&block14, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp200, tmp194, tmp195, tmp196);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp201;
    compiler::TNode<JSArgumentsObjectWithLength> tmp202;
    compiler::TNode<Smi> tmp203;
    compiler::TNode<Smi> tmp204;
    compiler::TNode<SloppyArgumentsElements> tmp205;
    compiler::TNode<Smi> tmp206;
    compiler::TNode<Smi> tmp207;
    compiler::TNode<Smi> tmp208;
    compiler::TNode<FixedArray> tmp209;
    compiler::TNode<Smi> tmp210;
    compiler::TNode<Context> tmp211;
    compiler::TNode<Map> tmp212;
    compiler::TNode<JSArray> tmp213;
    compiler::TNode<Smi> tmp214;
    compiler::TNode<FixedArray> tmp215;
    compiler::TNode<Smi> tmp216;
    compiler::TNode<Smi> tmp217;
    ca_.Bind(&block14, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217);
    compiler::TNode<Smi> tmp218;
    USE(tmp218);
    tmp218 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp219;
    USE(tmp219);
    tmp219 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp217}, compiler::TNode<Smi>{tmp218}));
    ca_.Goto(&block13, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp219);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp220;
    compiler::TNode<JSArgumentsObjectWithLength> tmp221;
    compiler::TNode<Smi> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<SloppyArgumentsElements> tmp224;
    compiler::TNode<Smi> tmp225;
    compiler::TNode<Smi> tmp226;
    compiler::TNode<Smi> tmp227;
    compiler::TNode<FixedArray> tmp228;
    compiler::TNode<Smi> tmp229;
    compiler::TNode<Context> tmp230;
    compiler::TNode<Map> tmp231;
    compiler::TNode<JSArray> tmp232;
    compiler::TNode<Smi> tmp233;
    compiler::TNode<FixedArray> tmp234;
    compiler::TNode<Smi> tmp235;
    compiler::TNode<Smi> tmp236;
    ca_.Bind(&block12, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 75);
    compiler::TNode<Smi> tmp237;
    USE(tmp237);
    tmp237 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiMax(compiler::TNode<Smi>{tmp226}, compiler::TNode<Smi>{tmp222}));
    compiler::TNode<Smi> tmp238;
    USE(tmp238);
    tmp238 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiMin(compiler::TNode<Smi>{tmp237}, compiler::TNode<Smi>{tmp227}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 76);
    compiler::TNode<Smi> tmp239;
    USE(tmp239);
    tmp239 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp227}, compiler::TNode<Smi>{tmp238}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 78);
    compiler::TNode<IntPtrT> tmp240;
    USE(tmp240);
    tmp240 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp233}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 79);
    compiler::TNode<IntPtrT> tmp241;
    USE(tmp241);
    tmp241 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp238}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 80);
    compiler::TNode<IntPtrT> tmp242;
    USE(tmp242);
    tmp242 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp239}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 77);
    CodeStubAssembler(state_).CopyElements(PACKED_ELEMENTS, compiler::TNode<FixedArrayBase>{tmp234}, compiler::TNode<IntPtrT>{tmp240}, compiler::TNode<FixedArrayBase>{tmp228}, compiler::TNode<IntPtrT>{tmp241}, compiler::TNode<IntPtrT>{tmp242});
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 81);
    ca_.Goto(&block2, tmp220, tmp221, tmp222, tmp223, tmp232);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp243;
    compiler::TNode<JSArgumentsObjectWithLength> tmp244;
    compiler::TNode<Smi> tmp245;
    compiler::TNode<Smi> tmp246;
    compiler::TNode<JSArray> tmp247;
    ca_.Bind(&block2, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 29);
    ca_.Goto(&block19, tmp243, tmp244, tmp245, tmp246, tmp247);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp248;
    compiler::TNode<JSArgumentsObjectWithLength> tmp249;
    compiler::TNode<Smi> tmp250;
    compiler::TNode<Smi> tmp251;
    compiler::TNode<JSArray> tmp252;
    ca_.Bind(&block19, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252);
  return compiler::TNode<JSArray>{tmp252};
}

compiler::TNode<JSArray> ArraySliceBuiltinsFromDSLAssembler::HandleFastSlice(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o, compiler::TNode<Number> p_startNumber, compiler::TNode<Number> p_countNumber, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Number, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Number, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, Object, JSArray> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArray> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArray> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, Object> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, Object, JSArgumentsObjectWithLength> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map, Context, JSArgumentsObjectWithLength, Smi, Smi> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map, Context, JSArgumentsObjectWithLength, Smi, Smi, JSArray> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map, Context, JSArgumentsObjectWithLength, Smi, Smi> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map, Context, JSArgumentsObjectWithLength, Smi, Smi, JSArray> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object, JSArgumentsObjectWithLength, Context, Map> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, Smi, Smi, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, JSArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number, Number, JSArray> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_startNumber, p_countNumber);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Number> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 88);
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
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 89);
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
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 90);
    compiler::TNode<Smi> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThanOrEqual(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp30}));
    ca_.Branch(tmp31, &block7, &block8, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Number> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Smi> tmp37;
    ca_.Bind(&block8, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    CodeStubAssembler(state_).FailAssert("Torque assert \'start >= 0\' failed", "../../src/builtins/array-slice.tq", 90);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Number> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<Smi> tmp43;
    ca_.Bind(&block7, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 92);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 93);
    compiler::TNode<JSArray> tmp44;
    USE(tmp44);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp44 = BaseBuiltinsFromDSLAssembler(state_).Cast20ATFastJSArrayForCopy(compiler::TNode<Context>{tmp38}, compiler::TNode<Object>{tmp39}, &label0);
    ca_.Goto(&block11, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp39, tmp39, tmp44);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp39, tmp39);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Number> tmp47;
    compiler::TNode<Number> tmp48;
    compiler::TNode<Smi> tmp49;
    compiler::TNode<Smi> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Object> tmp52;
    ca_.Bind(&block12, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.Goto(&block10, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Number> tmp55;
    compiler::TNode<Number> tmp56;
    compiler::TNode<Smi> tmp57;
    compiler::TNode<Smi> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<JSArray> tmp61;
    ca_.Bind(&block11, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 100);
    compiler::TNode<Smi> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp57}, compiler::TNode<Smi>{tmp58}));
    compiler::TNode<Smi> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadFastJSArrayLength(compiler::TNode<JSArray>{tmp61}));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiAbove(compiler::TNode<Smi>{tmp62}, compiler::TNode<Smi>{tmp63}));
    ca_.Branch(tmp64, &block13, &block14, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp61);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<Number> tmp67;
    compiler::TNode<Number> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<JSArray> tmp72;
    ca_.Bind(&block13, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.Goto(&block1);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Smi> tmp77;
    compiler::TNode<Smi> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<JSArray> tmp80;
    ca_.Bind(&block14, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 101);
    compiler::TNode<JSArray> tmp81;
    tmp81 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kExtractFastJSArray, tmp73, tmp80, tmp77, tmp78));
    USE(tmp81);
    ca_.Goto(&block2, tmp73, tmp74, tmp75, tmp76, tmp81);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<Number> tmp84;
    compiler::TNode<Number> tmp85;
    compiler::TNode<Smi> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<Object> tmp88;
    ca_.Bind(&block10, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 103);
    compiler::TNode<JSArgumentsObjectWithLength> tmp89;
    USE(tmp89);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp89 = BaseBuiltinsFromDSLAssembler(state_).Cast27JSArgumentsObjectWithLength(compiler::TNode<Context>{tmp82}, compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp88)}, &label0);
    ca_.Goto(&block17, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, ca_.UncheckedCast<Object>(tmp88), tmp89);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block18, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, ca_.UncheckedCast<Object>(tmp88));
    }
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<Number> tmp93;
    compiler::TNode<Smi> tmp94;
    compiler::TNode<Smi> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block18, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.Goto(&block16, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp98;
    compiler::TNode<Object> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<Smi> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<JSArgumentsObjectWithLength> tmp106;
    ca_.Bind(&block17, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 104);
    compiler::TNode<Context> tmp107;
    USE(tmp107);
    tmp107 = ca_.UncheckedCast<Context>(CodeStubAssembler(state_).LoadNativeContext(compiler::TNode<Context>{tmp98}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 105);
    compiler::TNode<Map> tmp108;
    USE(tmp108);
    tmp108 = ca_.UncheckedCast<Map>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapObjectMap(compiler::TNode<HeapObject>{tmp106}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 106);
    compiler::TNode<BoolT> tmp109;
    USE(tmp109);
    tmp109 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsFastAliasedArgumentsMap(compiler::TNode<Context>{tmp98}, compiler::TNode<Map>{tmp108}));
    ca_.Branch(tmp109, &block19, &block20, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp106, tmp107, tmp108);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Number> tmp113;
    compiler::TNode<Smi> tmp114;
    compiler::TNode<Smi> tmp115;
    compiler::TNode<Object> tmp116;
    compiler::TNode<JSArgumentsObjectWithLength> tmp117;
    compiler::TNode<Context> tmp118;
    compiler::TNode<Map> tmp119;
    ca_.Bind(&block19, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 107);
    compiler::TNode<JSArray> tmp120;
    USE(tmp120);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp120 = ArraySliceBuiltinsFromDSLAssembler(state_).HandleFastAliasedSloppyArgumentsSlice(compiler::TNode<Context>{tmp110}, compiler::TNode<JSArgumentsObjectWithLength>{tmp117}, compiler::TNode<Smi>{tmp114}, compiler::TNode<Smi>{tmp115}, &label0);
    ca_.Goto(&block22, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp110, tmp117, tmp114, tmp115, tmp120);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block23, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp110, tmp117, tmp114, tmp115);
    }
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Smi> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Object> tmp127;
    compiler::TNode<JSArgumentsObjectWithLength> tmp128;
    compiler::TNode<Context> tmp129;
    compiler::TNode<Map> tmp130;
    compiler::TNode<Context> tmp131;
    compiler::TNode<JSArgumentsObjectWithLength> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<Smi> tmp134;
    ca_.Bind(&block23, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134);
    ca_.Goto(&block1);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<Number> tmp137;
    compiler::TNode<Number> tmp138;
    compiler::TNode<Smi> tmp139;
    compiler::TNode<Smi> tmp140;
    compiler::TNode<Object> tmp141;
    compiler::TNode<JSArgumentsObjectWithLength> tmp142;
    compiler::TNode<Context> tmp143;
    compiler::TNode<Map> tmp144;
    compiler::TNode<Context> tmp145;
    compiler::TNode<JSArgumentsObjectWithLength> tmp146;
    compiler::TNode<Smi> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<JSArray> tmp149;
    ca_.Bind(&block22, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149);
    ca_.Goto(&block2, tmp135, tmp136, tmp137, tmp138, tmp149);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp150;
    compiler::TNode<Object> tmp151;
    compiler::TNode<Number> tmp152;
    compiler::TNode<Number> tmp153;
    compiler::TNode<Smi> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<Object> tmp156;
    compiler::TNode<JSArgumentsObjectWithLength> tmp157;
    compiler::TNode<Context> tmp158;
    compiler::TNode<Map> tmp159;
    ca_.Bind(&block20, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 109);
    compiler::TNode<BoolT> tmp160;
    USE(tmp160);
    tmp160 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsStrictArgumentsMap(compiler::TNode<Context>{tmp150}, compiler::TNode<Map>{tmp159}));
    ca_.Branch(tmp160, &block24, &block26, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp161;
    compiler::TNode<Object> tmp162;
    compiler::TNode<Number> tmp163;
    compiler::TNode<Number> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<Smi> tmp166;
    compiler::TNode<Object> tmp167;
    compiler::TNode<JSArgumentsObjectWithLength> tmp168;
    compiler::TNode<Context> tmp169;
    compiler::TNode<Map> tmp170;
    ca_.Bind(&block26, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170);
    compiler::TNode<BoolT> tmp171;
    USE(tmp171);
    tmp171 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsSloppyArgumentsMap(compiler::TNode<Context>{tmp161}, compiler::TNode<Map>{tmp170}));
    ca_.Branch(tmp171, &block24, &block25, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<Number> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<Object> tmp178;
    compiler::TNode<JSArgumentsObjectWithLength> tmp179;
    compiler::TNode<Context> tmp180;
    compiler::TNode<Map> tmp181;
    ca_.Bind(&block24, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 110);
    compiler::TNode<JSArray> tmp182;
    USE(tmp182);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp182 = ArraySliceBuiltinsFromDSLAssembler(state_).HandleSimpleArgumentsSlice(compiler::TNode<Context>{tmp172}, compiler::TNode<JSArgumentsObjectWithLength>{tmp179}, compiler::TNode<Smi>{tmp176}, compiler::TNode<Smi>{tmp177}, &label0);
    ca_.Goto(&block27, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp172, tmp179, tmp176, tmp177, tmp182);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block28, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp172, tmp179, tmp176, tmp177);
    }
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp183;
    compiler::TNode<Object> tmp184;
    compiler::TNode<Number> tmp185;
    compiler::TNode<Number> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<Smi> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<JSArgumentsObjectWithLength> tmp190;
    compiler::TNode<Context> tmp191;
    compiler::TNode<Map> tmp192;
    compiler::TNode<Context> tmp193;
    compiler::TNode<JSArgumentsObjectWithLength> tmp194;
    compiler::TNode<Smi> tmp195;
    compiler::TNode<Smi> tmp196;
    ca_.Bind(&block28, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196);
    ca_.Goto(&block1);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp197;
    compiler::TNode<Object> tmp198;
    compiler::TNode<Number> tmp199;
    compiler::TNode<Number> tmp200;
    compiler::TNode<Smi> tmp201;
    compiler::TNode<Smi> tmp202;
    compiler::TNode<Object> tmp203;
    compiler::TNode<JSArgumentsObjectWithLength> tmp204;
    compiler::TNode<Context> tmp205;
    compiler::TNode<Map> tmp206;
    compiler::TNode<Context> tmp207;
    compiler::TNode<JSArgumentsObjectWithLength> tmp208;
    compiler::TNode<Smi> tmp209;
    compiler::TNode<Smi> tmp210;
    compiler::TNode<JSArray> tmp211;
    ca_.Bind(&block27, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211);
    ca_.Goto(&block2, tmp197, tmp198, tmp199, tmp200, tmp211);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp212;
    compiler::TNode<Object> tmp213;
    compiler::TNode<Number> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<Smi> tmp216;
    compiler::TNode<Smi> tmp217;
    compiler::TNode<Object> tmp218;
    compiler::TNode<JSArgumentsObjectWithLength> tmp219;
    compiler::TNode<Context> tmp220;
    compiler::TNode<Map> tmp221;
    ca_.Bind(&block25, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 106);
    ca_.Goto(&block21, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp222;
    compiler::TNode<Object> tmp223;
    compiler::TNode<Number> tmp224;
    compiler::TNode<Number> tmp225;
    compiler::TNode<Smi> tmp226;
    compiler::TNode<Smi> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<JSArgumentsObjectWithLength> tmp229;
    compiler::TNode<Context> tmp230;
    compiler::TNode<Map> tmp231;
    ca_.Bind(&block21, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 103);
    ca_.Goto(&block15, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp232;
    compiler::TNode<Object> tmp233;
    compiler::TNode<Number> tmp234;
    compiler::TNode<Number> tmp235;
    compiler::TNode<Smi> tmp236;
    compiler::TNode<Smi> tmp237;
    compiler::TNode<Object> tmp238;
    ca_.Bind(&block16, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 114);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 103);
    ca_.Goto(&block15, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp239;
    compiler::TNode<Object> tmp240;
    compiler::TNode<Number> tmp241;
    compiler::TNode<Number> tmp242;
    compiler::TNode<Smi> tmp243;
    compiler::TNode<Smi> tmp244;
    compiler::TNode<Object> tmp245;
    ca_.Bind(&block15, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 93);
    ca_.Goto(&block9, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp246;
    compiler::TNode<Object> tmp247;
    compiler::TNode<Number> tmp248;
    compiler::TNode<Number> tmp249;
    compiler::TNode<Smi> tmp250;
    compiler::TNode<Smi> tmp251;
    compiler::TNode<Object> tmp252;
    ca_.Bind(&block9, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 92);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 117);
    ca_.Goto(&block1);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp253;
    compiler::TNode<Object> tmp254;
    compiler::TNode<Number> tmp255;
    compiler::TNode<Number> tmp256;
    compiler::TNode<JSArray> tmp257;
    ca_.Bind(&block2, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 84);
    ca_.Goto(&block29, tmp253, tmp254, tmp255, tmp256, tmp257);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp258;
    compiler::TNode<Object> tmp259;
    compiler::TNode<Number> tmp260;
    compiler::TNode<Number> tmp261;
    compiler::TNode<JSArray> tmp262;
    ca_.Bind(&block29, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262);
  return compiler::TNode<JSArray>{tmp262};
}

TF_BUILTIN(ArrayPrototypeSlice, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block24(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number, Number> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number, Number> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block28(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block32(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number, Number> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number, Number> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block36(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number> block41(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block40(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number, Number> block45(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Number, Number> block46(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block44(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block43(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Context, JSReceiver, Number, Number> block50(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, Context, JSReceiver, Number, Number, JSArray> block49(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block48(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number> block47(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number> block53(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number, Number, Number> block54(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number, Number, Number> block55(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number> block51(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number, Number, Oddball> block56(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number, Number, Oddball> block57(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Object, Number, Number, Number, JSReceiver, Number> block52(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 125);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(arguments));
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp2}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Branch(tmp4, &block1, &block2, tmp0, tmp1);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block1, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 126);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 127);
    compiler::TNode<JSArray> tmp7;
    USE(tmp7);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Cast20ATFastJSArrayForCopy(compiler::TNode<Context>{tmp5}, compiler::TNode<Object>{tmp6}, &label0);
    ca_.Goto(&block5, tmp5, tmp6, tmp6, tmp6, tmp7);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp5, tmp6, tmp6, tmp6);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    ca_.Bind(&block6, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.Goto(&block4, tmp8, tmp9, tmp10);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<JSArray> tmp16;
    ca_.Bind(&block5, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 128);
    compiler::TNode<JSArray> tmp17;
    tmp17 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCloneFastJSArray, tmp12, tmp16));
    USE(tmp17);
    arguments->PopAndReturn(tmp17);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    ca_.Bind(&block4, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 130);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 127);
    ca_.Goto(&block3, tmp18, tmp19, tmp20);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<Object> tmp23;
    ca_.Bind(&block3, &tmp21, &tmp22, &tmp23);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 126);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 125);
    ca_.Goto(&block2, tmp21, tmp22);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block2, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 136);
    compiler::TNode<JSReceiver> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp24}, compiler::TNode<Object>{tmp25}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 139);
    compiler::TNode<Number> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp24}, compiler::TNode<Object>{tmp26}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 142);
    compiler::TNode<IntPtrT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp28}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 143);
    compiler::TNode<Number> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp24}, compiler::TNode<Object>{tmp29}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 147);
    compiler::TNode<Number> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp30}, compiler::TNode<Number>{tmp31}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block11, tmp24, tmp25, tmp26, tmp27, tmp29, tmp30, tmp30);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block12, tmp24, tmp25, tmp26, tmp27, tmp29, tmp30, tmp30);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Number> tmp38;
    ca_.Bind(&block11, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.Goto(&block7, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    compiler::TNode<Number> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Number> tmp44;
    compiler::TNode<Number> tmp45;
    ca_.Bind(&block12, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.Goto(&block8, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Number> tmp51;
    ca_.Bind(&block7, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    compiler::TNode<Number> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp49}, compiler::TNode<Number>{tmp51}));
    compiler::TNode<Number> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp52}, compiler::TNode<Number>{tmp53}));
    ca_.Goto(&block10, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp54);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Number> tmp60;
    ca_.Bind(&block8, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 148);
    compiler::TNode<Number> tmp61;
    USE(tmp61);
    tmp61 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp60}, compiler::TNode<Number>{tmp58}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 147);
    ca_.Goto(&block9, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<JSReceiver> tmp64;
    compiler::TNode<Number> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<Number> tmp67;
    compiler::TNode<Number> tmp68;
    ca_.Bind(&block10, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68);
    ca_.Goto(&block9, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<JSReceiver> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Number> tmp75;
    ca_.Bind(&block9, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 152);
    compiler::TNode<IntPtrT> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp76}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 154);
    compiler::TNode<Oddball> tmp78;
    USE(tmp78);
    tmp78 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp77}, compiler::TNode<HeapObject>{tmp78}));
    ca_.Branch(tmp79, &block13, &block14, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp77);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<JSReceiver> tmp82;
    compiler::TNode<Number> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<Number> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Object> tmp87;
    ca_.Bind(&block13, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87);
    ca_.Goto(&block16, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp83);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<JSReceiver> tmp90;
    compiler::TNode<Number> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Number> tmp93;
    compiler::TNode<Number> tmp94;
    compiler::TNode<Object> tmp95;
    ca_.Bind(&block14, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95);
    compiler::TNode<Number> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp88}, compiler::TNode<Object>{tmp95}));
    ca_.Goto(&block15, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<JSReceiver> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<Number> tmp102;
    compiler::TNode<Number> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<Number> tmp105;
    ca_.Bind(&block16, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105);
    ca_.Goto(&block15, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp106;
    compiler::TNode<Object> tmp107;
    compiler::TNode<JSReceiver> tmp108;
    compiler::TNode<Number> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<Number> tmp114;
    ca_.Bind(&block15, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 153);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 159);
    compiler::TNode<Number> tmp115;
    USE(tmp115);
    tmp115 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp114}, compiler::TNode<Number>{tmp115}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block21, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp114);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block22, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp114);
    }
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp116;
    compiler::TNode<Object> tmp117;
    compiler::TNode<JSReceiver> tmp118;
    compiler::TNode<Number> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<Number> tmp121;
    compiler::TNode<Number> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Number> tmp125;
    ca_.Bind(&block21, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125);
    ca_.Goto(&block17, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp126;
    compiler::TNode<Object> tmp127;
    compiler::TNode<JSReceiver> tmp128;
    compiler::TNode<Number> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<Number> tmp131;
    compiler::TNode<Number> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<Number> tmp134;
    compiler::TNode<Number> tmp135;
    ca_.Bind(&block22, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135);
    ca_.Goto(&block18, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<JSReceiver> tmp138;
    compiler::TNode<Number> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<Number> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<Number> tmp144;
    ca_.Bind(&block17, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144);
    compiler::TNode<Number> tmp145;
    USE(tmp145);
    tmp145 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp139}, compiler::TNode<Number>{tmp144}));
    compiler::TNode<Number> tmp146;
    USE(tmp146);
    tmp146 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp147;
    USE(tmp147);
    tmp147 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp145}, compiler::TNode<Number>{tmp146}));
    ca_.Goto(&block20, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp147);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp148;
    compiler::TNode<Object> tmp149;
    compiler::TNode<JSReceiver> tmp150;
    compiler::TNode<Number> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<Number> tmp153;
    compiler::TNode<Number> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<Number> tmp156;
    ca_.Bind(&block18, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156);
    compiler::TNode<Number> tmp157;
    USE(tmp157);
    tmp157 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp156}, compiler::TNode<Number>{tmp151}));
    ca_.Goto(&block19, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp158;
    compiler::TNode<Object> tmp159;
    compiler::TNode<JSReceiver> tmp160;
    compiler::TNode<Number> tmp161;
    compiler::TNode<Object> tmp162;
    compiler::TNode<Number> tmp163;
    compiler::TNode<Number> tmp164;
    compiler::TNode<Object> tmp165;
    compiler::TNode<Number> tmp166;
    compiler::TNode<Number> tmp167;
    ca_.Bind(&block20, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167);
    ca_.Goto(&block19, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp168;
    compiler::TNode<Object> tmp169;
    compiler::TNode<JSReceiver> tmp170;
    compiler::TNode<Number> tmp171;
    compiler::TNode<Object> tmp172;
    compiler::TNode<Number> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<Number> tmp176;
    compiler::TNode<Number> tmp177;
    ca_.Bind(&block19, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 158);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 162);
    compiler::TNode<Number> tmp178;
    USE(tmp178);
    tmp178 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp177}, compiler::TNode<Number>{tmp174}));
    compiler::TNode<Number> tmp179;
    USE(tmp179);
    tmp179 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp180;
    USE(tmp180);
    tmp180 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp178}, compiler::TNode<Number>{tmp179}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 164);
    compiler::TNode<Number> tmp181;
    USE(tmp181);
    tmp181 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThanOrEqual(compiler::TNode<Number>{tmp181}, compiler::TNode<Number>{tmp174}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block25, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp180, tmp174);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block26, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp180, tmp174);
    }
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp182;
    compiler::TNode<Object> tmp183;
    compiler::TNode<JSReceiver> tmp184;
    compiler::TNode<Number> tmp185;
    compiler::TNode<Object> tmp186;
    compiler::TNode<Number> tmp187;
    compiler::TNode<Number> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<Number> tmp190;
    compiler::TNode<Number> tmp191;
    compiler::TNode<Number> tmp192;
    compiler::TNode<Number> tmp193;
    ca_.Bind(&block25, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193);
    ca_.Goto(&block23, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<JSReceiver> tmp196;
    compiler::TNode<Number> tmp197;
    compiler::TNode<Object> tmp198;
    compiler::TNode<Number> tmp199;
    compiler::TNode<Number> tmp200;
    compiler::TNode<Object> tmp201;
    compiler::TNode<Number> tmp202;
    compiler::TNode<Number> tmp203;
    compiler::TNode<Number> tmp204;
    compiler::TNode<Number> tmp205;
    ca_.Bind(&block26, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205);
    ca_.Goto(&block24, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp206;
    compiler::TNode<Object> tmp207;
    compiler::TNode<JSReceiver> tmp208;
    compiler::TNode<Number> tmp209;
    compiler::TNode<Object> tmp210;
    compiler::TNode<Number> tmp211;
    compiler::TNode<Number> tmp212;
    compiler::TNode<Object> tmp213;
    compiler::TNode<Number> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<Number> tmp216;
    ca_.Bind(&block24, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216);
    CodeStubAssembler(state_).FailAssert("Torque assert \'0 <= k\' failed", "../../src/builtins/array-slice.tq", 164);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp217;
    compiler::TNode<Object> tmp218;
    compiler::TNode<JSReceiver> tmp219;
    compiler::TNode<Number> tmp220;
    compiler::TNode<Object> tmp221;
    compiler::TNode<Number> tmp222;
    compiler::TNode<Number> tmp223;
    compiler::TNode<Object> tmp224;
    compiler::TNode<Number> tmp225;
    compiler::TNode<Number> tmp226;
    compiler::TNode<Number> tmp227;
    ca_.Bind(&block23, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 165);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThanOrEqual(compiler::TNode<Number>{tmp223}, compiler::TNode<Number>{tmp220}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block29, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp223, tmp220);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block30, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp223, tmp220);
    }
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp228;
    compiler::TNode<Object> tmp229;
    compiler::TNode<JSReceiver> tmp230;
    compiler::TNode<Number> tmp231;
    compiler::TNode<Object> tmp232;
    compiler::TNode<Number> tmp233;
    compiler::TNode<Number> tmp234;
    compiler::TNode<Object> tmp235;
    compiler::TNode<Number> tmp236;
    compiler::TNode<Number> tmp237;
    compiler::TNode<Number> tmp238;
    compiler::TNode<Number> tmp239;
    compiler::TNode<Number> tmp240;
    ca_.Bind(&block29, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240);
    ca_.Goto(&block27, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp241;
    compiler::TNode<Object> tmp242;
    compiler::TNode<JSReceiver> tmp243;
    compiler::TNode<Number> tmp244;
    compiler::TNode<Object> tmp245;
    compiler::TNode<Number> tmp246;
    compiler::TNode<Number> tmp247;
    compiler::TNode<Object> tmp248;
    compiler::TNode<Number> tmp249;
    compiler::TNode<Number> tmp250;
    compiler::TNode<Number> tmp251;
    compiler::TNode<Number> tmp252;
    compiler::TNode<Number> tmp253;
    ca_.Bind(&block30, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253);
    ca_.Goto(&block28, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp254;
    compiler::TNode<Object> tmp255;
    compiler::TNode<JSReceiver> tmp256;
    compiler::TNode<Number> tmp257;
    compiler::TNode<Object> tmp258;
    compiler::TNode<Number> tmp259;
    compiler::TNode<Number> tmp260;
    compiler::TNode<Object> tmp261;
    compiler::TNode<Number> tmp262;
    compiler::TNode<Number> tmp263;
    compiler::TNode<Number> tmp264;
    ca_.Bind(&block28, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264);
    CodeStubAssembler(state_).FailAssert("Torque assert \'k <= len\' failed", "../../src/builtins/array-slice.tq", 165);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp265;
    compiler::TNode<Object> tmp266;
    compiler::TNode<JSReceiver> tmp267;
    compiler::TNode<Number> tmp268;
    compiler::TNode<Object> tmp269;
    compiler::TNode<Number> tmp270;
    compiler::TNode<Number> tmp271;
    compiler::TNode<Object> tmp272;
    compiler::TNode<Number> tmp273;
    compiler::TNode<Number> tmp274;
    compiler::TNode<Number> tmp275;
    ca_.Bind(&block27, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 166);
    compiler::TNode<Number> tmp276;
    USE(tmp276);
    tmp276 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThanOrEqual(compiler::TNode<Number>{tmp276}, compiler::TNode<Number>{tmp274}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block33, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272, tmp273, tmp274, tmp275, tmp274);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block34, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272, tmp273, tmp274, tmp275, tmp274);
    }
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp277;
    compiler::TNode<Object> tmp278;
    compiler::TNode<JSReceiver> tmp279;
    compiler::TNode<Number> tmp280;
    compiler::TNode<Object> tmp281;
    compiler::TNode<Number> tmp282;
    compiler::TNode<Number> tmp283;
    compiler::TNode<Object> tmp284;
    compiler::TNode<Number> tmp285;
    compiler::TNode<Number> tmp286;
    compiler::TNode<Number> tmp287;
    compiler::TNode<Number> tmp288;
    ca_.Bind(&block33, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288);
    ca_.Goto(&block31, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp289;
    compiler::TNode<Object> tmp290;
    compiler::TNode<JSReceiver> tmp291;
    compiler::TNode<Number> tmp292;
    compiler::TNode<Object> tmp293;
    compiler::TNode<Number> tmp294;
    compiler::TNode<Number> tmp295;
    compiler::TNode<Object> tmp296;
    compiler::TNode<Number> tmp297;
    compiler::TNode<Number> tmp298;
    compiler::TNode<Number> tmp299;
    compiler::TNode<Number> tmp300;
    ca_.Bind(&block34, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300);
    ca_.Goto(&block32, tmp289, tmp290, tmp291, tmp292, tmp293, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp301;
    compiler::TNode<Object> tmp302;
    compiler::TNode<JSReceiver> tmp303;
    compiler::TNode<Number> tmp304;
    compiler::TNode<Object> tmp305;
    compiler::TNode<Number> tmp306;
    compiler::TNode<Number> tmp307;
    compiler::TNode<Object> tmp308;
    compiler::TNode<Number> tmp309;
    compiler::TNode<Number> tmp310;
    compiler::TNode<Number> tmp311;
    ca_.Bind(&block32, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311);
    CodeStubAssembler(state_).FailAssert("Torque assert \'0 <= final\' failed", "../../src/builtins/array-slice.tq", 166);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp312;
    compiler::TNode<Object> tmp313;
    compiler::TNode<JSReceiver> tmp314;
    compiler::TNode<Number> tmp315;
    compiler::TNode<Object> tmp316;
    compiler::TNode<Number> tmp317;
    compiler::TNode<Number> tmp318;
    compiler::TNode<Object> tmp319;
    compiler::TNode<Number> tmp320;
    compiler::TNode<Number> tmp321;
    compiler::TNode<Number> tmp322;
    ca_.Bind(&block31, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 167);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThanOrEqual(compiler::TNode<Number>{tmp321}, compiler::TNode<Number>{tmp315}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block37, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp321, tmp315);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block38, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp321, tmp315);
    }
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp323;
    compiler::TNode<Object> tmp324;
    compiler::TNode<JSReceiver> tmp325;
    compiler::TNode<Number> tmp326;
    compiler::TNode<Object> tmp327;
    compiler::TNode<Number> tmp328;
    compiler::TNode<Number> tmp329;
    compiler::TNode<Object> tmp330;
    compiler::TNode<Number> tmp331;
    compiler::TNode<Number> tmp332;
    compiler::TNode<Number> tmp333;
    compiler::TNode<Number> tmp334;
    compiler::TNode<Number> tmp335;
    ca_.Bind(&block37, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335);
    ca_.Goto(&block35, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp333);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp336;
    compiler::TNode<Object> tmp337;
    compiler::TNode<JSReceiver> tmp338;
    compiler::TNode<Number> tmp339;
    compiler::TNode<Object> tmp340;
    compiler::TNode<Number> tmp341;
    compiler::TNode<Number> tmp342;
    compiler::TNode<Object> tmp343;
    compiler::TNode<Number> tmp344;
    compiler::TNode<Number> tmp345;
    compiler::TNode<Number> tmp346;
    compiler::TNode<Number> tmp347;
    compiler::TNode<Number> tmp348;
    ca_.Bind(&block38, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348);
    ca_.Goto(&block36, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346);
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp349;
    compiler::TNode<Object> tmp350;
    compiler::TNode<JSReceiver> tmp351;
    compiler::TNode<Number> tmp352;
    compiler::TNode<Object> tmp353;
    compiler::TNode<Number> tmp354;
    compiler::TNode<Number> tmp355;
    compiler::TNode<Object> tmp356;
    compiler::TNode<Number> tmp357;
    compiler::TNode<Number> tmp358;
    compiler::TNode<Number> tmp359;
    ca_.Bind(&block36, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358, &tmp359);
    CodeStubAssembler(state_).FailAssert("Torque assert \'final <= len\' failed", "../../src/builtins/array-slice.tq", 167);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp360;
    compiler::TNode<Object> tmp361;
    compiler::TNode<JSReceiver> tmp362;
    compiler::TNode<Number> tmp363;
    compiler::TNode<Object> tmp364;
    compiler::TNode<Number> tmp365;
    compiler::TNode<Number> tmp366;
    compiler::TNode<Object> tmp367;
    compiler::TNode<Number> tmp368;
    compiler::TNode<Number> tmp369;
    compiler::TNode<Number> tmp370;
    ca_.Bind(&block35, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 168);
    compiler::TNode<Number> tmp371;
    USE(tmp371);
    tmp371 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThanOrEqual(compiler::TNode<Number>{tmp371}, compiler::TNode<Number>{tmp370}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block41, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp370);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block42, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp370);
    }
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp372;
    compiler::TNode<Object> tmp373;
    compiler::TNode<JSReceiver> tmp374;
    compiler::TNode<Number> tmp375;
    compiler::TNode<Object> tmp376;
    compiler::TNode<Number> tmp377;
    compiler::TNode<Number> tmp378;
    compiler::TNode<Object> tmp379;
    compiler::TNode<Number> tmp380;
    compiler::TNode<Number> tmp381;
    compiler::TNode<Number> tmp382;
    compiler::TNode<Number> tmp383;
    ca_.Bind(&block41, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383);
    ca_.Goto(&block39, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp380, tmp381, tmp382);
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp384;
    compiler::TNode<Object> tmp385;
    compiler::TNode<JSReceiver> tmp386;
    compiler::TNode<Number> tmp387;
    compiler::TNode<Object> tmp388;
    compiler::TNode<Number> tmp389;
    compiler::TNode<Number> tmp390;
    compiler::TNode<Object> tmp391;
    compiler::TNode<Number> tmp392;
    compiler::TNode<Number> tmp393;
    compiler::TNode<Number> tmp394;
    compiler::TNode<Number> tmp395;
    ca_.Bind(&block42, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395);
    ca_.Goto(&block40, tmp384, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp396;
    compiler::TNode<Object> tmp397;
    compiler::TNode<JSReceiver> tmp398;
    compiler::TNode<Number> tmp399;
    compiler::TNode<Object> tmp400;
    compiler::TNode<Number> tmp401;
    compiler::TNode<Number> tmp402;
    compiler::TNode<Object> tmp403;
    compiler::TNode<Number> tmp404;
    compiler::TNode<Number> tmp405;
    compiler::TNode<Number> tmp406;
    ca_.Bind(&block40, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406);
    CodeStubAssembler(state_).FailAssert("Torque assert \'0 <= count\' failed", "../../src/builtins/array-slice.tq", 168);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp407;
    compiler::TNode<Object> tmp408;
    compiler::TNode<JSReceiver> tmp409;
    compiler::TNode<Number> tmp410;
    compiler::TNode<Object> tmp411;
    compiler::TNode<Number> tmp412;
    compiler::TNode<Number> tmp413;
    compiler::TNode<Object> tmp414;
    compiler::TNode<Number> tmp415;
    compiler::TNode<Number> tmp416;
    compiler::TNode<Number> tmp417;
    ca_.Bind(&block39, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 169);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThanOrEqual(compiler::TNode<Number>{tmp417}, compiler::TNode<Number>{tmp410}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block45, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp417, tmp410);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block46, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp417, tmp410);
    }
  }

  if (block45.is_used()) {
    compiler::TNode<Context> tmp418;
    compiler::TNode<Object> tmp419;
    compiler::TNode<JSReceiver> tmp420;
    compiler::TNode<Number> tmp421;
    compiler::TNode<Object> tmp422;
    compiler::TNode<Number> tmp423;
    compiler::TNode<Number> tmp424;
    compiler::TNode<Object> tmp425;
    compiler::TNode<Number> tmp426;
    compiler::TNode<Number> tmp427;
    compiler::TNode<Number> tmp428;
    compiler::TNode<Number> tmp429;
    compiler::TNode<Number> tmp430;
    ca_.Bind(&block45, &tmp418, &tmp419, &tmp420, &tmp421, &tmp422, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430);
    ca_.Goto(&block43, tmp418, tmp419, tmp420, tmp421, tmp422, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428);
  }

  if (block46.is_used()) {
    compiler::TNode<Context> tmp431;
    compiler::TNode<Object> tmp432;
    compiler::TNode<JSReceiver> tmp433;
    compiler::TNode<Number> tmp434;
    compiler::TNode<Object> tmp435;
    compiler::TNode<Number> tmp436;
    compiler::TNode<Number> tmp437;
    compiler::TNode<Object> tmp438;
    compiler::TNode<Number> tmp439;
    compiler::TNode<Number> tmp440;
    compiler::TNode<Number> tmp441;
    compiler::TNode<Number> tmp442;
    compiler::TNode<Number> tmp443;
    ca_.Bind(&block46, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442, &tmp443);
    ca_.Goto(&block44, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438, tmp439, tmp440, tmp441);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp444;
    compiler::TNode<Object> tmp445;
    compiler::TNode<JSReceiver> tmp446;
    compiler::TNode<Number> tmp447;
    compiler::TNode<Object> tmp448;
    compiler::TNode<Number> tmp449;
    compiler::TNode<Number> tmp450;
    compiler::TNode<Object> tmp451;
    compiler::TNode<Number> tmp452;
    compiler::TNode<Number> tmp453;
    compiler::TNode<Number> tmp454;
    ca_.Bind(&block44, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454);
    CodeStubAssembler(state_).FailAssert("Torque assert \'count <= len\' failed", "../../src/builtins/array-slice.tq", 169);
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp455;
    compiler::TNode<Object> tmp456;
    compiler::TNode<JSReceiver> tmp457;
    compiler::TNode<Number> tmp458;
    compiler::TNode<Object> tmp459;
    compiler::TNode<Number> tmp460;
    compiler::TNode<Number> tmp461;
    compiler::TNode<Object> tmp462;
    compiler::TNode<Number> tmp463;
    compiler::TNode<Number> tmp464;
    compiler::TNode<Number> tmp465;
    ca_.Bind(&block43, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463, &tmp464, &tmp465);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 172);
    compiler::TNode<JSArray> tmp466;
    USE(tmp466);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp466 = ArraySliceBuiltinsFromDSLAssembler(state_).HandleFastSlice(compiler::TNode<Context>{tmp455}, compiler::TNode<Object>{tmp457}, compiler::TNode<Number>{tmp461}, compiler::TNode<Number>{tmp465}, &label0);
    ca_.Goto(&block49, tmp455, tmp456, tmp457, tmp458, tmp459, tmp460, tmp461, tmp462, tmp463, tmp464, tmp465, tmp455, tmp457, tmp461, tmp465, tmp466);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block50, tmp455, tmp456, tmp457, tmp458, tmp459, tmp460, tmp461, tmp462, tmp463, tmp464, tmp465, tmp455, tmp457, tmp461, tmp465);
    }
  }

  if (block50.is_used()) {
    compiler::TNode<Context> tmp467;
    compiler::TNode<Object> tmp468;
    compiler::TNode<JSReceiver> tmp469;
    compiler::TNode<Number> tmp470;
    compiler::TNode<Object> tmp471;
    compiler::TNode<Number> tmp472;
    compiler::TNode<Number> tmp473;
    compiler::TNode<Object> tmp474;
    compiler::TNode<Number> tmp475;
    compiler::TNode<Number> tmp476;
    compiler::TNode<Number> tmp477;
    compiler::TNode<Context> tmp478;
    compiler::TNode<JSReceiver> tmp479;
    compiler::TNode<Number> tmp480;
    compiler::TNode<Number> tmp481;
    ca_.Bind(&block50, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478, &tmp479, &tmp480, &tmp481);
    ca_.Goto(&block48, tmp467, tmp468, tmp469, tmp470, tmp471, tmp472, tmp473, tmp474, tmp475, tmp476, tmp477);
  }

  if (block49.is_used()) {
    compiler::TNode<Context> tmp482;
    compiler::TNode<Object> tmp483;
    compiler::TNode<JSReceiver> tmp484;
    compiler::TNode<Number> tmp485;
    compiler::TNode<Object> tmp486;
    compiler::TNode<Number> tmp487;
    compiler::TNode<Number> tmp488;
    compiler::TNode<Object> tmp489;
    compiler::TNode<Number> tmp490;
    compiler::TNode<Number> tmp491;
    compiler::TNode<Number> tmp492;
    compiler::TNode<Context> tmp493;
    compiler::TNode<JSReceiver> tmp494;
    compiler::TNode<Number> tmp495;
    compiler::TNode<Number> tmp496;
    compiler::TNode<JSArray> tmp497;
    ca_.Bind(&block49, &tmp482, &tmp483, &tmp484, &tmp485, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494, &tmp495, &tmp496, &tmp497);
    arguments->PopAndReturn(tmp497);
  }

  if (block48.is_used()) {
    compiler::TNode<Context> tmp498;
    compiler::TNode<Object> tmp499;
    compiler::TNode<JSReceiver> tmp500;
    compiler::TNode<Number> tmp501;
    compiler::TNode<Object> tmp502;
    compiler::TNode<Number> tmp503;
    compiler::TNode<Number> tmp504;
    compiler::TNode<Object> tmp505;
    compiler::TNode<Number> tmp506;
    compiler::TNode<Number> tmp507;
    compiler::TNode<Number> tmp508;
    ca_.Bind(&block48, &tmp498, &tmp499, &tmp500, &tmp501, &tmp502, &tmp503, &tmp504, &tmp505, &tmp506, &tmp507, &tmp508);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 171);
    ca_.Goto(&block47, tmp498, tmp499, tmp500, tmp501, tmp502, tmp503, tmp504, tmp505, tmp506, tmp507, tmp508);
  }

  if (block47.is_used()) {
    compiler::TNode<Context> tmp509;
    compiler::TNode<Object> tmp510;
    compiler::TNode<JSReceiver> tmp511;
    compiler::TNode<Number> tmp512;
    compiler::TNode<Object> tmp513;
    compiler::TNode<Number> tmp514;
    compiler::TNode<Number> tmp515;
    compiler::TNode<Object> tmp516;
    compiler::TNode<Number> tmp517;
    compiler::TNode<Number> tmp518;
    compiler::TNode<Number> tmp519;
    ca_.Bind(&block47, &tmp509, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518, &tmp519);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 177);
    compiler::TNode<JSReceiver> tmp520;
    USE(tmp520);
    tmp520 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ArraySpeciesCreate(compiler::TNode<Context>{tmp509}, compiler::TNode<Object>{tmp511}, compiler::TNode<Number>{tmp519}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 180);
    compiler::TNode<Number> tmp521;
    USE(tmp521);
    tmp521 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 183);
    ca_.Goto(&block53, tmp509, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517, tmp518, tmp519, tmp520, tmp521);
  }

  if (block53.is_used()) {
    compiler::TNode<Context> tmp522;
    compiler::TNode<Object> tmp523;
    compiler::TNode<JSReceiver> tmp524;
    compiler::TNode<Number> tmp525;
    compiler::TNode<Object> tmp526;
    compiler::TNode<Number> tmp527;
    compiler::TNode<Number> tmp528;
    compiler::TNode<Object> tmp529;
    compiler::TNode<Number> tmp530;
    compiler::TNode<Number> tmp531;
    compiler::TNode<Number> tmp532;
    compiler::TNode<JSReceiver> tmp533;
    compiler::TNode<Number> tmp534;
    ca_.Bind(&block53, &tmp522, &tmp523, &tmp524, &tmp525, &tmp526, &tmp527, &tmp528, &tmp529, &tmp530, &tmp531, &tmp532, &tmp533, &tmp534);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp528}, compiler::TNode<Number>{tmp531}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block54, tmp522, tmp523, tmp524, tmp525, tmp526, tmp527, tmp528, tmp529, tmp530, tmp531, tmp532, tmp533, tmp534, tmp528, tmp531);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block55, tmp522, tmp523, tmp524, tmp525, tmp526, tmp527, tmp528, tmp529, tmp530, tmp531, tmp532, tmp533, tmp534, tmp528, tmp531);
    }
  }

  if (block54.is_used()) {
    compiler::TNode<Context> tmp535;
    compiler::TNode<Object> tmp536;
    compiler::TNode<JSReceiver> tmp537;
    compiler::TNode<Number> tmp538;
    compiler::TNode<Object> tmp539;
    compiler::TNode<Number> tmp540;
    compiler::TNode<Number> tmp541;
    compiler::TNode<Object> tmp542;
    compiler::TNode<Number> tmp543;
    compiler::TNode<Number> tmp544;
    compiler::TNode<Number> tmp545;
    compiler::TNode<JSReceiver> tmp546;
    compiler::TNode<Number> tmp547;
    compiler::TNode<Number> tmp548;
    compiler::TNode<Number> tmp549;
    ca_.Bind(&block54, &tmp535, &tmp536, &tmp537, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544, &tmp545, &tmp546, &tmp547, &tmp548, &tmp549);
    ca_.Goto(&block51, tmp535, tmp536, tmp537, tmp538, tmp539, tmp540, tmp541, tmp542, tmp543, tmp544, tmp545, tmp546, tmp547);
  }

  if (block55.is_used()) {
    compiler::TNode<Context> tmp550;
    compiler::TNode<Object> tmp551;
    compiler::TNode<JSReceiver> tmp552;
    compiler::TNode<Number> tmp553;
    compiler::TNode<Object> tmp554;
    compiler::TNode<Number> tmp555;
    compiler::TNode<Number> tmp556;
    compiler::TNode<Object> tmp557;
    compiler::TNode<Number> tmp558;
    compiler::TNode<Number> tmp559;
    compiler::TNode<Number> tmp560;
    compiler::TNode<JSReceiver> tmp561;
    compiler::TNode<Number> tmp562;
    compiler::TNode<Number> tmp563;
    compiler::TNode<Number> tmp564;
    ca_.Bind(&block55, &tmp550, &tmp551, &tmp552, &tmp553, &tmp554, &tmp555, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563, &tmp564);
    ca_.Goto(&block52, tmp550, tmp551, tmp552, tmp553, tmp554, tmp555, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562);
  }

  if (block51.is_used()) {
    compiler::TNode<Context> tmp565;
    compiler::TNode<Object> tmp566;
    compiler::TNode<JSReceiver> tmp567;
    compiler::TNode<Number> tmp568;
    compiler::TNode<Object> tmp569;
    compiler::TNode<Number> tmp570;
    compiler::TNode<Number> tmp571;
    compiler::TNode<Object> tmp572;
    compiler::TNode<Number> tmp573;
    compiler::TNode<Number> tmp574;
    compiler::TNode<Number> tmp575;
    compiler::TNode<JSReceiver> tmp576;
    compiler::TNode<Number> tmp577;
    ca_.Bind(&block51, &tmp565, &tmp566, &tmp567, &tmp568, &tmp569, &tmp570, &tmp571, &tmp572, &tmp573, &tmp574, &tmp575, &tmp576, &tmp577);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 185);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 188);
    compiler::TNode<Oddball> tmp578;
    tmp578 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp565, tmp567, tmp571));
    USE(tmp578);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 191);
    compiler::TNode<Oddball> tmp579;
    USE(tmp579);
    tmp579 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp580;
    USE(tmp580);
    tmp580 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp578}, compiler::TNode<HeapObject>{tmp579}));
    ca_.Branch(tmp580, &block56, &block57, tmp565, tmp566, tmp567, tmp568, tmp569, tmp570, tmp571, tmp572, tmp573, tmp574, tmp575, tmp576, tmp577, tmp571, tmp578);
  }

  if (block56.is_used()) {
    compiler::TNode<Context> tmp581;
    compiler::TNode<Object> tmp582;
    compiler::TNode<JSReceiver> tmp583;
    compiler::TNode<Number> tmp584;
    compiler::TNode<Object> tmp585;
    compiler::TNode<Number> tmp586;
    compiler::TNode<Number> tmp587;
    compiler::TNode<Object> tmp588;
    compiler::TNode<Number> tmp589;
    compiler::TNode<Number> tmp590;
    compiler::TNode<Number> tmp591;
    compiler::TNode<JSReceiver> tmp592;
    compiler::TNode<Number> tmp593;
    compiler::TNode<Number> tmp594;
    compiler::TNode<Oddball> tmp595;
    ca_.Bind(&block56, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585, &tmp586, &tmp587, &tmp588, &tmp589, &tmp590, &tmp591, &tmp592, &tmp593, &tmp594, &tmp595);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 193);
    compiler::TNode<Object> tmp596;
    USE(tmp596);
    tmp596 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp581}, compiler::TNode<Object>{tmp583}, compiler::TNode<Object>{tmp594}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 196);
    compiler::TNode<Object> tmp597;
    tmp597 = CodeStubAssembler(state_).CallBuiltin(Builtins::kFastCreateDataProperty, tmp581, tmp592, tmp593, tmp596);
    USE(tmp597);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 191);
    ca_.Goto(&block57, tmp581, tmp582, tmp583, tmp584, tmp585, tmp586, tmp587, tmp588, tmp589, tmp590, tmp591, tmp592, tmp593, tmp594, tmp595);
  }

  if (block57.is_used()) {
    compiler::TNode<Context> tmp598;
    compiler::TNode<Object> tmp599;
    compiler::TNode<JSReceiver> tmp600;
    compiler::TNode<Number> tmp601;
    compiler::TNode<Object> tmp602;
    compiler::TNode<Number> tmp603;
    compiler::TNode<Number> tmp604;
    compiler::TNode<Object> tmp605;
    compiler::TNode<Number> tmp606;
    compiler::TNode<Number> tmp607;
    compiler::TNode<Number> tmp608;
    compiler::TNode<JSReceiver> tmp609;
    compiler::TNode<Number> tmp610;
    compiler::TNode<Number> tmp611;
    compiler::TNode<Oddball> tmp612;
    ca_.Bind(&block57, &tmp598, &tmp599, &tmp600, &tmp601, &tmp602, &tmp603, &tmp604, &tmp605, &tmp606, &tmp607, &tmp608, &tmp609, &tmp610, &tmp611, &tmp612);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 200);
    compiler::TNode<Number> tmp613;
    USE(tmp613);
    tmp613 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp614;
    USE(tmp614);
    tmp614 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp604}, compiler::TNode<Number>{tmp613}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 203);
    compiler::TNode<Number> tmp615;
    USE(tmp615);
    tmp615 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp616;
    USE(tmp616);
    tmp616 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp610}, compiler::TNode<Number>{tmp615}));
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 183);
    ca_.Goto(&block53, tmp598, tmp599, tmp600, tmp601, tmp602, tmp603, tmp614, tmp605, tmp606, tmp607, tmp608, tmp609, tmp616);
  }

  if (block52.is_used()) {
    compiler::TNode<Context> tmp617;
    compiler::TNode<Object> tmp618;
    compiler::TNode<JSReceiver> tmp619;
    compiler::TNode<Number> tmp620;
    compiler::TNode<Object> tmp621;
    compiler::TNode<Number> tmp622;
    compiler::TNode<Number> tmp623;
    compiler::TNode<Object> tmp624;
    compiler::TNode<Number> tmp625;
    compiler::TNode<Number> tmp626;
    compiler::TNode<Number> tmp627;
    compiler::TNode<JSReceiver> tmp628;
    compiler::TNode<Number> tmp629;
    ca_.Bind(&block52, &tmp617, &tmp618, &tmp619, &tmp620, &tmp621, &tmp622, &tmp623, &tmp624, &tmp625, &tmp626, &tmp627, &tmp628, &tmp629);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 207);
    compiler::TNode<String> tmp630;
    USE(tmp630);
    tmp630 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp617, tmp628, tmp630, tmp629);
    ca_.SetSourcePosition("../../src/builtins/array-slice.tq", 210);
    arguments->PopAndReturn(tmp628);
  }
}

}  // namespace internal
}  // namespace v8

