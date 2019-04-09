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

compiler::TNode<Number> ArrayCopywithinBuiltinsFromDSLAssembler::ConvertToRelativeIndex(compiler::TNode<Number> p_index, compiler::TNode<Number> p_length) {
  compiler::CodeAssemblerParameterizedLabel<Number, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_index, p_length);

  if (block0.is_used()) {
    compiler::TNode<Number> tmp0;
    compiler::TNode<Number> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 7);
    compiler::TNode<Number> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp0}, compiler::TNode<Number>{tmp2}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp0, tmp1, tmp0);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block7, tmp0, tmp1, tmp0);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Number> tmp3;
    compiler::TNode<Number> tmp4;
    compiler::TNode<Number> tmp5;
    ca_.Bind(&block6, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block2, tmp3, tmp4);
  }

  if (block7.is_used()) {
    compiler::TNode<Number> tmp6;
    compiler::TNode<Number> tmp7;
    compiler::TNode<Number> tmp8;
    ca_.Bind(&block7, &tmp6, &tmp7, &tmp8);
    ca_.Goto(&block3, tmp6, tmp7);
  }

  if (block2.is_used()) {
    compiler::TNode<Number> tmp9;
    compiler::TNode<Number> tmp10;
    ca_.Bind(&block2, &tmp9, &tmp10);
    compiler::TNode<Number> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp9}, compiler::TNode<Number>{tmp10}));
    compiler::TNode<Number> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp11}, compiler::TNode<Number>{tmp12}));
    ca_.Goto(&block5, tmp9, tmp10, tmp13);
  }

  if (block3.is_used()) {
    compiler::TNode<Number> tmp14;
    compiler::TNode<Number> tmp15;
    ca_.Bind(&block3, &tmp14, &tmp15);
    compiler::TNode<Number> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp14}, compiler::TNode<Number>{tmp15}));
    ca_.Goto(&block4, tmp14, tmp15, tmp16);
  }

  if (block5.is_used()) {
    compiler::TNode<Number> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Number> tmp19;
    ca_.Bind(&block5, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block4, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Number> tmp20;
    compiler::TNode<Number> tmp21;
    compiler::TNode<Number> tmp22;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block1, tmp20, tmp21, tmp22);
  }

  if (block1.is_used()) {
    compiler::TNode<Number> tmp23;
    compiler::TNode<Number> tmp24;
    compiler::TNode<Number> tmp25;
    ca_.Bind(&block1, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 6);
    ca_.Goto(&block8, tmp23, tmp24, tmp25);
  }

    compiler::TNode<Number> tmp26;
    compiler::TNode<Number> tmp27;
    compiler::TNode<Number> tmp28;
    ca_.Bind(&block8, &tmp26, &tmp27, &tmp28);
  return compiler::TNode<Number>{tmp28};
}

TF_BUILTIN(ArrayPrototypeCopyWithin, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Oddball> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Oddball> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Oddball> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 14);
    compiler::TNode<JSReceiver> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 17);
    compiler::TNode<Number> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 20);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp4}));
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 24);
    compiler::TNode<Number> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Number>(ArrayCopywithinBuiltinsFromDSLAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Number>{tmp6}, compiler::TNode<Number>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 27);
    compiler::TNode<IntPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp8}));
    compiler::TNode<Number> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp9}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 31);
    compiler::TNode<Number> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Number>(ArrayCopywithinBuiltinsFromDSLAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Number>{tmp10}, compiler::TNode<Number>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 35);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 36);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp12}));
    compiler::TNode<Oddball> tmp14;
    USE(tmp14);
    tmp14 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp13}, compiler::TNode<HeapObject>{tmp14}));
    ca_.Branch(tmp15, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp6, tmp7, tmp10, tmp11, tmp3);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<JSReceiver> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Number> tmp21;
    compiler::TNode<Number> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Number> tmp24;
    ca_.Bind(&block1, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 37);
    compiler::TNode<IntPtrT> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp25}));
    compiler::TNode<Number> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp16}, compiler::TNode<Object>{tmp26}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 36);
    ca_.Goto(&block2, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp27);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<JSReceiver> tmp30;
    compiler::TNode<Number> tmp31;
    compiler::TNode<Number> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<Number> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Number> tmp36;
    ca_.Bind(&block2, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 42);
    compiler::TNode<Number> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<Number>(ArrayCopywithinBuiltinsFromDSLAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Number>{tmp36}, compiler::TNode<Number>{tmp31}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 45);
    compiler::TNode<Number> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp37}, compiler::TNode<Number>{tmp35}));
    compiler::TNode<Number> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp31}, compiler::TNode<Number>{tmp33}));
    compiler::TNode<Number> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp38}, compiler::TNode<Number>{tmp39}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 48);
    compiler::TNode<Number> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 50);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp35}, compiler::TNode<Number>{tmp33}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp40, tmp41, tmp35, tmp33);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block7, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp40, tmp41, tmp35, tmp33);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<JSReceiver> tmp44;
    compiler::TNode<Number> tmp45;
    compiler::TNode<Number> tmp46;
    compiler::TNode<Number> tmp47;
    compiler::TNode<Number> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<Number> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<Number> tmp55;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block5, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    compiler::TNode<Number> tmp59;
    compiler::TNode<Number> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Number> tmp64;
    compiler::TNode<Number> tmp65;
    compiler::TNode<Number> tmp66;
    compiler::TNode<Number> tmp67;
    compiler::TNode<Number> tmp68;
    compiler::TNode<Number> tmp69;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.Goto(&block4, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<JSReceiver> tmp72;
    compiler::TNode<Number> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<Number> tmp79;
    compiler::TNode<Number> tmp80;
    compiler::TNode<Number> tmp81;
    ca_.Bind(&block5, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81);
    compiler::TNode<Number> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp77}, compiler::TNode<Number>{tmp80}));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp75}, compiler::TNode<Number>{tmp82}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp75, tmp82);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block9, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp75, tmp82);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<JSReceiver> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<Number> tmp88;
    compiler::TNode<Number> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Number> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<Number> tmp93;
    compiler::TNode<Number> tmp94;
    compiler::TNode<Number> tmp95;
    compiler::TNode<Number> tmp96;
    ca_.Bind(&block8, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96);
    ca_.Goto(&block3, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<JSReceiver> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Number> tmp102;
    compiler::TNode<Number> tmp103;
    compiler::TNode<Number> tmp104;
    compiler::TNode<Number> tmp105;
    compiler::TNode<Number> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<Number> tmp108;
    compiler::TNode<Number> tmp109;
    compiler::TNode<Number> tmp110;
    ca_.Bind(&block9, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110);
    ca_.Goto(&block4, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<JSReceiver> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<Number> tmp115;
    compiler::TNode<Number> tmp116;
    compiler::TNode<Number> tmp117;
    compiler::TNode<Number> tmp118;
    compiler::TNode<Number> tmp119;
    compiler::TNode<Number> tmp120;
    compiler::TNode<Number> tmp121;
    compiler::TNode<Number> tmp122;
    ca_.Bind(&block3, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 52);
    compiler::TNode<Number> tmp123;
    USE(tmp123);
    tmp123 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(-1));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 55);
    compiler::TNode<Number> tmp124;
    USE(tmp124);
    tmp124 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp118}, compiler::TNode<Number>{tmp121}));
    compiler::TNode<Number> tmp125;
    USE(tmp125);
    tmp125 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp126;
    USE(tmp126);
    tmp126 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp124}, compiler::TNode<Number>{tmp125}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 58);
    compiler::TNode<Number> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp116}, compiler::TNode<Number>{tmp121}));
    compiler::TNode<Number> tmp128;
    USE(tmp128);
    tmp128 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp129;
    USE(tmp129);
    tmp129 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp127}, compiler::TNode<Number>{tmp128}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 50);
    ca_.Goto(&block4, tmp111, tmp112, tmp113, tmp114, tmp115, tmp129, tmp117, tmp126, tmp119, tmp120, tmp121, tmp123);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<JSReceiver> tmp132;
    compiler::TNode<Number> tmp133;
    compiler::TNode<Number> tmp134;
    compiler::TNode<Number> tmp135;
    compiler::TNode<Number> tmp136;
    compiler::TNode<Number> tmp137;
    compiler::TNode<Number> tmp138;
    compiler::TNode<Number> tmp139;
    compiler::TNode<Number> tmp140;
    compiler::TNode<Number> tmp141;
    ca_.Bind(&block4, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 62);
    ca_.Goto(&block12, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<JSReceiver> tmp144;
    compiler::TNode<Number> tmp145;
    compiler::TNode<Number> tmp146;
    compiler::TNode<Number> tmp147;
    compiler::TNode<Number> tmp148;
    compiler::TNode<Number> tmp149;
    compiler::TNode<Number> tmp150;
    compiler::TNode<Number> tmp151;
    compiler::TNode<Number> tmp152;
    compiler::TNode<Number> tmp153;
    ca_.Bind(&block12, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153);
    compiler::TNode<Number> tmp154;
    USE(tmp154);
    tmp154 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp152}, compiler::TNode<Number>{tmp154}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block13, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp152);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block14, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp152);
    }
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp155;
    compiler::TNode<Object> tmp156;
    compiler::TNode<JSReceiver> tmp157;
    compiler::TNode<Number> tmp158;
    compiler::TNode<Number> tmp159;
    compiler::TNode<Number> tmp160;
    compiler::TNode<Number> tmp161;
    compiler::TNode<Number> tmp162;
    compiler::TNode<Number> tmp163;
    compiler::TNode<Number> tmp164;
    compiler::TNode<Number> tmp165;
    compiler::TNode<Number> tmp166;
    compiler::TNode<Number> tmp167;
    ca_.Bind(&block13, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167);
    ca_.Goto(&block10, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp168;
    compiler::TNode<Object> tmp169;
    compiler::TNode<JSReceiver> tmp170;
    compiler::TNode<Number> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<Number> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<Number> tmp175;
    compiler::TNode<Number> tmp176;
    compiler::TNode<Number> tmp177;
    compiler::TNode<Number> tmp178;
    compiler::TNode<Number> tmp179;
    compiler::TNode<Number> tmp180;
    ca_.Bind(&block14, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180);
    ca_.Goto(&block11, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp181;
    compiler::TNode<Object> tmp182;
    compiler::TNode<JSReceiver> tmp183;
    compiler::TNode<Number> tmp184;
    compiler::TNode<Number> tmp185;
    compiler::TNode<Number> tmp186;
    compiler::TNode<Number> tmp187;
    compiler::TNode<Number> tmp188;
    compiler::TNode<Number> tmp189;
    compiler::TNode<Number> tmp190;
    compiler::TNode<Number> tmp191;
    compiler::TNode<Number> tmp192;
    ca_.Bind(&block10, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 66);
    compiler::TNode<Oddball> tmp193;
    tmp193 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp181, tmp183, tmp188));
    USE(tmp193);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 69);
    compiler::TNode<Oddball> tmp194;
    USE(tmp194);
    tmp194 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp195;
    USE(tmp195);
    tmp195 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp193}, compiler::TNode<HeapObject>{tmp194}));
    ca_.Branch(tmp195, &block15, &block16, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp196;
    compiler::TNode<Object> tmp197;
    compiler::TNode<JSReceiver> tmp198;
    compiler::TNode<Number> tmp199;
    compiler::TNode<Number> tmp200;
    compiler::TNode<Number> tmp201;
    compiler::TNode<Number> tmp202;
    compiler::TNode<Number> tmp203;
    compiler::TNode<Number> tmp204;
    compiler::TNode<Number> tmp205;
    compiler::TNode<Number> tmp206;
    compiler::TNode<Number> tmp207;
    compiler::TNode<Oddball> tmp208;
    ca_.Bind(&block15, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 71);
    compiler::TNode<Object> tmp209;
    USE(tmp209);
    tmp209 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp196}, compiler::TNode<Object>{tmp198}, compiler::TNode<Object>{tmp203}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 74);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp196, tmp198, tmp201, tmp209);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 69);
    ca_.Goto(&block17, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp211;
    compiler::TNode<Object> tmp212;
    compiler::TNode<JSReceiver> tmp213;
    compiler::TNode<Number> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<Number> tmp216;
    compiler::TNode<Number> tmp217;
    compiler::TNode<Number> tmp218;
    compiler::TNode<Number> tmp219;
    compiler::TNode<Number> tmp220;
    compiler::TNode<Number> tmp221;
    compiler::TNode<Number> tmp222;
    compiler::TNode<Oddball> tmp223;
    ca_.Bind(&block16, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 77);
    compiler::TNode<Smi> tmp224;
    USE(tmp224);
    tmp224 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp211, tmp213, tmp216, tmp224);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 69);
    ca_.Goto(&block17, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<JSReceiver> tmp228;
    compiler::TNode<Number> tmp229;
    compiler::TNode<Number> tmp230;
    compiler::TNode<Number> tmp231;
    compiler::TNode<Number> tmp232;
    compiler::TNode<Number> tmp233;
    compiler::TNode<Number> tmp234;
    compiler::TNode<Number> tmp235;
    compiler::TNode<Number> tmp236;
    compiler::TNode<Number> tmp237;
    compiler::TNode<Oddball> tmp238;
    ca_.Bind(&block17, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 81);
    compiler::TNode<Number> tmp239;
    USE(tmp239);
    tmp239 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp233}, compiler::TNode<Number>{tmp237}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 84);
    compiler::TNode<Number> tmp240;
    USE(tmp240);
    tmp240 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp231}, compiler::TNode<Number>{tmp237}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 87);
    compiler::TNode<Number> tmp241;
    USE(tmp241);
    tmp241 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp242;
    USE(tmp242);
    tmp242 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp236}, compiler::TNode<Number>{tmp241}));
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 62);
    ca_.Goto(&block12, tmp226, tmp227, tmp228, tmp229, tmp230, tmp240, tmp232, tmp239, tmp234, tmp235, tmp242, tmp237);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp243;
    compiler::TNode<Object> tmp244;
    compiler::TNode<JSReceiver> tmp245;
    compiler::TNode<Number> tmp246;
    compiler::TNode<Number> tmp247;
    compiler::TNode<Number> tmp248;
    compiler::TNode<Number> tmp249;
    compiler::TNode<Number> tmp250;
    compiler::TNode<Number> tmp251;
    compiler::TNode<Number> tmp252;
    compiler::TNode<Number> tmp253;
    compiler::TNode<Number> tmp254;
    ca_.Bind(&block11, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254);
    ca_.SetSourcePosition("../../src/builtins/array-copywithin.tq", 91);
    arguments->PopAndReturn(tmp245);
  }
}

}  // namespace internal
}  // namespace v8

