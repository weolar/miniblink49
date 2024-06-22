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

TF_BUILTIN(ArrayForEachLoopEagerDeoptContinuation, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Object> parameter1 = UncheckedCast<Object>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Object> parameter2 = UncheckedCast<Object>(Parameter(Descriptor::kCallback));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kThisArg));
  USE(parameter3);
  compiler::TNode<Object> parameter4 = UncheckedCast<Object>(Parameter(Descriptor::kInitialK));
  USE(parameter4);
  compiler::TNode<Object> parameter5 = UncheckedCast<Object>(Parameter(Descriptor::kLength));
  USE(parameter5);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, Object, JSReceiver> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, JSReceiver> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Object, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Object, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Number> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3, parameter4, parameter5);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 13);
    compiler::TNode<JSReceiver> tmp6;
    USE(tmp6);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp1, tmp6);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp1);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    ca_.Bind(&block4, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block2, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    ca_.Bind(&block3, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.Goto(&block1, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp21);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    ca_.Bind(&block2, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    ca_.Bind(&block1, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 14);
    compiler::TNode<JSReceiver> tmp35;
    USE(tmp35);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp35 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp28}, compiler::TNode<Object>{tmp30}, &label0);
    ca_.Goto(&block7, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp30, tmp35);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp30);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<Object> tmp43;
    ca_.Bind(&block8, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.Goto(&block6, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<JSReceiver> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<JSReceiver> tmp52;
    ca_.Bind(&block7, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.Goto(&block5, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp52);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<JSReceiver> tmp59;
    ca_.Bind(&block6, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<JSReceiver> tmp66;
    compiler::TNode<JSReceiver> tmp67;
    ca_.Bind(&block5, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 15);
    compiler::TNode<Number> tmp68;
    USE(tmp68);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp68 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{tmp64}, &label0);
    ca_.Goto(&block11, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp64, tmp68);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp64);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<JSReceiver> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    compiler::TNode<Object> tmp77;
    ca_.Bind(&block12, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.Goto(&block10, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<JSReceiver> tmp84;
    compiler::TNode<JSReceiver> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<Number> tmp87;
    ca_.Bind(&block11, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87);
    ca_.Goto(&block9, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp87);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<JSReceiver> tmp94;
    compiler::TNode<JSReceiver> tmp95;
    ca_.Bind(&block10, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<Object> tmp99;
    compiler::TNode<Object> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<JSReceiver> tmp102;
    compiler::TNode<JSReceiver> tmp103;
    compiler::TNode<Number> tmp104;
    ca_.Bind(&block9, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 16);
    compiler::TNode<Number> tmp105;
    USE(tmp105);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp105 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{tmp101}, &label0);
    ca_.Goto(&block15, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp101, tmp105);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block16, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp101);
    }
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp106;
    compiler::TNode<Object> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<Object> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<JSReceiver> tmp112;
    compiler::TNode<JSReceiver> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<Object> tmp115;
    ca_.Bind(&block16, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115);
    ca_.Goto(&block14, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp116;
    compiler::TNode<Object> tmp117;
    compiler::TNode<Object> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<JSReceiver> tmp122;
    compiler::TNode<JSReceiver> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<Number> tmp126;
    ca_.Bind(&block15, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.Goto(&block13, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp126);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp127;
    compiler::TNode<Object> tmp128;
    compiler::TNode<Object> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<Object> tmp132;
    compiler::TNode<JSReceiver> tmp133;
    compiler::TNode<JSReceiver> tmp134;
    compiler::TNode<Number> tmp135;
    ca_.Bind(&block14, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<Object> tmp141;
    compiler::TNode<JSReceiver> tmp142;
    compiler::TNode<JSReceiver> tmp143;
    compiler::TNode<Number> tmp144;
    compiler::TNode<Number> tmp145;
    ca_.Bind(&block13, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 19);
    compiler::TNode<Oddball> tmp146;
    USE(tmp146);
    tmp146 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 20);
    compiler::TNode<Oddball> tmp147;
    USE(tmp147);
    tmp147 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 18);
    compiler::TNode<Object> tmp148;
    tmp148 = CodeStubAssembler(state_).CallBuiltin(Builtins::kArrayForEachLoopContinuation, tmp136, tmp142, tmp143, tmp139, tmp146, tmp142, tmp144, tmp145, tmp147);
    USE(tmp148);
    CodeStubAssembler(state_).Return(tmp148);
  }
}

TF_BUILTIN(ArrayForEachLoopLazyDeoptContinuation, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Object> parameter1 = UncheckedCast<Object>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Object> parameter2 = UncheckedCast<Object>(Parameter(Descriptor::kCallback));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kThisArg));
  USE(parameter3);
  compiler::TNode<Object> parameter4 = UncheckedCast<Object>(Parameter(Descriptor::kInitialK));
  USE(parameter4);
  compiler::TNode<Object> parameter5 = UncheckedCast<Object>(Parameter(Descriptor::kLength));
  USE(parameter5);
  compiler::TNode<Object> parameter6 = UncheckedCast<Object>(Parameter(Descriptor::kResult));
  USE(parameter6);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, Object, JSReceiver> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Object, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Object, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Number> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3, parameter4, parameter5, parameter6);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 30);
    compiler::TNode<JSReceiver> tmp7;
    USE(tmp7);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp1, tmp7);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp1);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    ca_.Bind(&block4, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.Goto(&block2, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    ca_.Bind(&block3, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block1, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp24);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block2, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    ca_.Bind(&block1, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 31);
    compiler::TNode<JSReceiver> tmp40;
    USE(tmp40);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp40 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp32}, compiler::TNode<Object>{tmp34}, &label0);
    ca_.Goto(&block7, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp34, tmp40);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp34);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    compiler::TNode<Object> tmp49;
    ca_.Bind(&block8, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.Goto(&block6, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<JSReceiver> tmp59;
    ca_.Bind(&block7, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.Goto(&block5, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp59);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<JSReceiver> tmp67;
    ca_.Bind(&block6, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<JSReceiver> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    ca_.Bind(&block5, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 32);
    compiler::TNode<Number> tmp77;
    USE(tmp77);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp77 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{tmp72}, &label0);
    ca_.Goto(&block11, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp72, tmp77);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp72);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<JSReceiver> tmp85;
    compiler::TNode<JSReceiver> tmp86;
    compiler::TNode<Object> tmp87;
    ca_.Bind(&block12, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87);
    ca_.Goto(&block10, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<JSReceiver> tmp95;
    compiler::TNode<JSReceiver> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<Number> tmp98;
    ca_.Bind(&block11, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98);
    ca_.Goto(&block9, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp98);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp99;
    compiler::TNode<Object> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<JSReceiver> tmp106;
    compiler::TNode<JSReceiver> tmp107;
    ca_.Bind(&block10, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp108;
    compiler::TNode<Object> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<JSReceiver> tmp115;
    compiler::TNode<JSReceiver> tmp116;
    compiler::TNode<Number> tmp117;
    ca_.Bind(&block9, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 33);
    compiler::TNode<Number> tmp118;
    USE(tmp118);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp118 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{tmp113}, &label0);
    ca_.Goto(&block15, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp113, tmp118);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block16, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp113);
    }
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<JSReceiver> tmp126;
    compiler::TNode<JSReceiver> tmp127;
    compiler::TNode<Number> tmp128;
    compiler::TNode<Object> tmp129;
    ca_.Bind(&block16, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129);
    ca_.Goto(&block14, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<Object> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<JSReceiver> tmp137;
    compiler::TNode<JSReceiver> tmp138;
    compiler::TNode<Number> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<Number> tmp141;
    ca_.Bind(&block15, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141);
    ca_.Goto(&block13, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp141);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<Object> tmp145;
    compiler::TNode<Object> tmp146;
    compiler::TNode<Object> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<JSReceiver> tmp149;
    compiler::TNode<JSReceiver> tmp150;
    compiler::TNode<Number> tmp151;
    ca_.Bind(&block14, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<Object> tmp156;
    compiler::TNode<Object> tmp157;
    compiler::TNode<Object> tmp158;
    compiler::TNode<JSReceiver> tmp159;
    compiler::TNode<JSReceiver> tmp160;
    compiler::TNode<Number> tmp161;
    compiler::TNode<Number> tmp162;
    ca_.Bind(&block13, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 36);
    compiler::TNode<Oddball> tmp163;
    USE(tmp163);
    tmp163 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 37);
    compiler::TNode<Oddball> tmp164;
    USE(tmp164);
    tmp164 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 35);
    compiler::TNode<Object> tmp165;
    tmp165 = CodeStubAssembler(state_).CallBuiltin(Builtins::kArrayForEachLoopContinuation, tmp152, tmp159, tmp160, tmp155, tmp163, tmp159, tmp161, tmp162, tmp164);
    USE(tmp165);
    CodeStubAssembler(state_).Return(tmp165);
  }
}

TF_BUILTIN(ArrayForEachLoopContinuation, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<JSReceiver> parameter2 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kCallbackfn));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kThisArg));
  USE(parameter3);
  compiler::TNode<Object> parameter4 = UncheckedCast<Object>(Parameter(Descriptor::kArray));
  USE(parameter4);
  compiler::TNode<JSReceiver> parameter5 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kO));
  USE(parameter5);
  compiler::TNode<Number> parameter6 = UncheckedCast<Number>(Parameter(Descriptor::kInitialK));
  USE(parameter6);
  compiler::TNode<Number> parameter7 = UncheckedCast<Number>(Parameter(Descriptor::kLen));
  USE(parameter7);
  compiler::TNode<Object> parameter8 = UncheckedCast<Object>(Parameter(Descriptor::kTo));
  USE(parameter8);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, Object, JSReceiver, Number, Number, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, Object, JSReceiver, Number, Number, Object, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, Object, JSReceiver, Number, Number, Object, Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, Object, JSReceiver, Number, Number, Object, Number, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, Object, JSReceiver, Number, Number, Object, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, Object, JSReceiver, Number, Number, Object, Number, Oddball> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, Object, JSReceiver, Number, Number, Object, Number, Oddball> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, Object, JSReceiver, Number, Number, Object, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, Object, JSReceiver, Number, Number, Object, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3, parameter4, parameter5, parameter6, parameter7, parameter8);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<JSReceiver> tmp5;
    compiler::TNode<Number> tmp6;
    compiler::TNode<Number> tmp7;
    compiler::TNode<Object> tmp8;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 48);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp6);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<JSReceiver> tmp14;
    compiler::TNode<Number> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Number> tmp18;
    ca_.Bind(&block3, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp18}, compiler::TNode<Number>{tmp16}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp18, tmp16);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block6, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp18, tmp16);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<JSReceiver> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Number> tmp28;
    compiler::TNode<Number> tmp29;
    compiler::TNode<Number> tmp30;
    ca_.Bind(&block5, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.Goto(&block1, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<JSReceiver> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Number> tmp41;
    compiler::TNode<Number> tmp42;
    ca_.Bind(&block6, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    ca_.Goto(&block2, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp43;
    compiler::TNode<JSReceiver> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Number> tmp52;
    ca_.Bind(&block1, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 54);
    compiler::TNode<Oddball> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<Oddball>(CodeStubAssembler(state_).HasProperty_Inline(compiler::TNode<Context>{tmp43}, compiler::TNode<JSReceiver>{tmp48}, compiler::TNode<Object>{tmp52}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 57);
    compiler::TNode<Oddball> tmp54;
    USE(tmp54);
    tmp54 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp53}, compiler::TNode<HeapObject>{tmp54}));
    ca_.Branch(tmp55, &block7, &block8, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<JSReceiver> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<Number> tmp65;
    compiler::TNode<Oddball> tmp66;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 59);
    compiler::TNode<Object> tmp67;
    USE(tmp67);
    tmp67 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp65}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 62);
    compiler::TNode<Object> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp56}, compiler::TNode<JSReceiver>{tmp58}, compiler::TNode<Object>{tmp59}, compiler::TNode<Object>{tmp67}, compiler::TNode<Object>{tmp65}, compiler::TNode<Object>{tmp61}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 57);
    ca_.Goto(&block8, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<JSReceiver> tmp70;
    compiler::TNode<JSReceiver> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<Oddball> tmp79;
    ca_.Bind(&block8, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 48);
    ca_.Goto(&block4, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp80;
    compiler::TNode<JSReceiver> tmp81;
    compiler::TNode<JSReceiver> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<JSReceiver> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Number> tmp89;
    ca_.Bind(&block4, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    compiler::TNode<Number> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp89}, compiler::TNode<Number>{tmp90}));
    ca_.Goto(&block3, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp91);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp92;
    compiler::TNode<JSReceiver> tmp93;
    compiler::TNode<JSReceiver> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<JSReceiver> tmp97;
    compiler::TNode<Number> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<Object> tmp100;
    compiler::TNode<Number> tmp101;
    ca_.Bind(&block2, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 67);
    compiler::TNode<Oddball> tmp102;
    USE(tmp102);
    tmp102 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp102);
  }
}

compiler::TNode<Object> ArrayForeachBuiltinsFromDSLAssembler::FastArrayForEach(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_thisArg, compiler::CodeAssemblerLabel* label_Bailout, compiler::TypedCodeAssemblerVariable<Smi>* label_Bailout_parameter_0) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Number, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSReceiver, JSArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, JSArray> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, JSArray, Smi> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, JSArray, Smi, Object> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, JSArray, Smi> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, JSArray, Smi, Object> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Object> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Object> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Object, Context, JSReceiver, Object, Object, Smi, JSArray> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Object> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_len, p_callbackfn, p_thisArg);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<JSReceiver> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 73);
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 74);
    compiler::TNode<Smi> tmp6;
    USE(tmp6);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp2}, &label0);
    ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp2, tmp6);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp2);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Number> tmp13;
    ca_.Bind(&block6, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Smi> tmp21;
    ca_.Bind(&block5, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp21);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<JSReceiver> tmp23;
    compiler::TNode<Number> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Smi> tmp27;
    ca_.Bind(&block4, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block1, tmp27);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<JSReceiver> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Smi> tmp33;
    compiler::TNode<Smi> tmp34;
    ca_.Bind(&block3, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 75);
    compiler::TNode<JSArray> tmp35;
    USE(tmp35);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp35 = BaseBuiltinsFromDSLAssembler(state_).Cast13ATFastJSArray(compiler::TNode<Context>{tmp28}, compiler::TNode<HeapObject>{tmp29}, &label0);
    ca_.Goto(&block9, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp29, tmp35);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp29);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<JSReceiver> tmp43;
    ca_.Bind(&block10, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.Goto(&block8, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Number> tmp46;
    compiler::TNode<JSReceiver> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<Smi> tmp49;
    compiler::TNode<Smi> tmp50;
    compiler::TNode<JSReceiver> tmp51;
    compiler::TNode<JSArray> tmp52;
    ca_.Bind(&block9, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.Goto(&block7, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp52);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    compiler::TNode<Number> tmp55;
    compiler::TNode<JSReceiver> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<Smi> tmp58;
    compiler::TNode<Smi> tmp59;
    ca_.Bind(&block8, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.Goto(&block1, tmp58);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp60;
    compiler::TNode<JSReceiver> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<JSReceiver> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<Smi> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<JSArray> tmp67;
    ca_.Bind(&block7, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 76);
    compiler::TNode<JSArray> tmp68;
    USE(tmp68);
    compiler::TNode<JSArray> tmp69;
    USE(tmp69);
    compiler::TNode<Map> tmp70;
    USE(tmp70);
    compiler::TNode<BoolT> tmp71;
    USE(tmp71);
    compiler::TNode<BoolT> tmp72;
    USE(tmp72);
    compiler::TNode<BoolT> tmp73;
    USE(tmp73);
    std::tie(tmp68, tmp69, tmp70, tmp71, tmp72, tmp73) = BaseBuiltinsFromDSLAssembler(state_).NewFastJSArrayWitness(compiler::TNode<JSArray>{tmp67}).Flatten();
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 79);
    ca_.Goto(&block13, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<JSReceiver> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<JSReceiver> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<Smi> tmp80;
    compiler::TNode<JSArray> tmp81;
    compiler::TNode<JSArray> tmp82;
    compiler::TNode<JSArray> tmp83;
    compiler::TNode<Map> tmp84;
    compiler::TNode<BoolT> tmp85;
    compiler::TNode<BoolT> tmp86;
    compiler::TNode<BoolT> tmp87;
    ca_.Bind(&block13, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87);
    compiler::TNode<BoolT> tmp88;
    USE(tmp88);
    tmp88 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp79}, compiler::TNode<Smi>{tmp80}));
    ca_.Branch(tmp88, &block11, &block12, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp89;
    compiler::TNode<JSReceiver> tmp90;
    compiler::TNode<Number> tmp91;
    compiler::TNode<JSReceiver> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Smi> tmp94;
    compiler::TNode<Smi> tmp95;
    compiler::TNode<JSArray> tmp96;
    compiler::TNode<JSArray> tmp97;
    compiler::TNode<JSArray> tmp98;
    compiler::TNode<Map> tmp99;
    compiler::TNode<BoolT> tmp100;
    compiler::TNode<BoolT> tmp101;
    compiler::TNode<BoolT> tmp102;
    ca_.Bind(&block11, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 2167);
    compiler::TNode<IntPtrT> tmp103 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp103);
    compiler::TNode<Map>tmp104 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp97, tmp103});
    compiler::TNode<BoolT> tmp105;
    USE(tmp105);
    tmp105 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<HeapObject>{tmp104}, compiler::TNode<HeapObject>{tmp99}));
    ca_.Branch(tmp105, &block18, &block19, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp106;
    compiler::TNode<JSReceiver> tmp107;
    compiler::TNode<Number> tmp108;
    compiler::TNode<JSReceiver> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<Smi> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<JSArray> tmp113;
    compiler::TNode<JSArray> tmp114;
    compiler::TNode<JSArray> tmp115;
    compiler::TNode<Map> tmp116;
    compiler::TNode<BoolT> tmp117;
    compiler::TNode<BoolT> tmp118;
    compiler::TNode<BoolT> tmp119;
    ca_.Bind(&block18, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119);
    ca_.Goto(&block16, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp120;
    compiler::TNode<JSReceiver> tmp121;
    compiler::TNode<Number> tmp122;
    compiler::TNode<JSReceiver> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Smi> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<JSArray> tmp127;
    compiler::TNode<JSArray> tmp128;
    compiler::TNode<JSArray> tmp129;
    compiler::TNode<Map> tmp130;
    compiler::TNode<BoolT> tmp131;
    compiler::TNode<BoolT> tmp132;
    compiler::TNode<BoolT> tmp133;
    ca_.Bind(&block19, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133);
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 2174);
    compiler::TNode<BoolT> tmp134;
    USE(tmp134);
    tmp134 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNoElementsProtectorCellInvalid());
    ca_.Branch(tmp134, &block20, &block21, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp135;
    compiler::TNode<JSReceiver> tmp136;
    compiler::TNode<Number> tmp137;
    compiler::TNode<JSReceiver> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<Smi> tmp140;
    compiler::TNode<Smi> tmp141;
    compiler::TNode<JSArray> tmp142;
    compiler::TNode<JSArray> tmp143;
    compiler::TNode<JSArray> tmp144;
    compiler::TNode<Map> tmp145;
    compiler::TNode<BoolT> tmp146;
    compiler::TNode<BoolT> tmp147;
    compiler::TNode<BoolT> tmp148;
    ca_.Bind(&block20, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148);
    ca_.Goto(&block16, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp149;
    compiler::TNode<JSReceiver> tmp150;
    compiler::TNode<Number> tmp151;
    compiler::TNode<JSReceiver> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<Smi> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<JSArray> tmp156;
    compiler::TNode<JSArray> tmp157;
    compiler::TNode<JSArray> tmp158;
    compiler::TNode<Map> tmp159;
    compiler::TNode<BoolT> tmp160;
    compiler::TNode<BoolT> tmp161;
    compiler::TNode<BoolT> tmp162;
    ca_.Bind(&block21, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162);
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 2175);
    compiler::TNode<JSArray> tmp163;
    USE(tmp163);
    tmp163 = (compiler::TNode<JSArray>{tmp157});
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 80);
    ca_.Goto(&block17, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp163, tmp159, tmp160, tmp161, tmp162);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp164;
    compiler::TNode<JSReceiver> tmp165;
    compiler::TNode<Number> tmp166;
    compiler::TNode<JSReceiver> tmp167;
    compiler::TNode<Object> tmp168;
    compiler::TNode<Smi> tmp169;
    compiler::TNode<Smi> tmp170;
    compiler::TNode<JSArray> tmp171;
    compiler::TNode<JSArray> tmp172;
    compiler::TNode<JSArray> tmp173;
    compiler::TNode<Map> tmp174;
    compiler::TNode<BoolT> tmp175;
    compiler::TNode<BoolT> tmp176;
    compiler::TNode<BoolT> tmp177;
    ca_.Bind(&block17, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177);
    ca_.Goto(&block15, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp178;
    compiler::TNode<JSReceiver> tmp179;
    compiler::TNode<Number> tmp180;
    compiler::TNode<JSReceiver> tmp181;
    compiler::TNode<Object> tmp182;
    compiler::TNode<Smi> tmp183;
    compiler::TNode<Smi> tmp184;
    compiler::TNode<JSArray> tmp185;
    compiler::TNode<JSArray> tmp186;
    compiler::TNode<JSArray> tmp187;
    compiler::TNode<Map> tmp188;
    compiler::TNode<BoolT> tmp189;
    compiler::TNode<BoolT> tmp190;
    compiler::TNode<BoolT> tmp191;
    ca_.Bind(&block16, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191);
    ca_.Goto(&block1, tmp183);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp192;
    compiler::TNode<JSReceiver> tmp193;
    compiler::TNode<Number> tmp194;
    compiler::TNode<JSReceiver> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<Smi> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<JSArray> tmp199;
    compiler::TNode<JSArray> tmp200;
    compiler::TNode<JSArray> tmp201;
    compiler::TNode<Map> tmp202;
    compiler::TNode<BoolT> tmp203;
    compiler::TNode<BoolT> tmp204;
    compiler::TNode<BoolT> tmp205;
    ca_.Bind(&block15, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 83);
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 2163);
    ca_.Goto(&block24, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp197, tmp201);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp206;
    compiler::TNode<JSReceiver> tmp207;
    compiler::TNode<Number> tmp208;
    compiler::TNode<JSReceiver> tmp209;
    compiler::TNode<Object> tmp210;
    compiler::TNode<Smi> tmp211;
    compiler::TNode<Smi> tmp212;
    compiler::TNode<JSArray> tmp213;
    compiler::TNode<JSArray> tmp214;
    compiler::TNode<JSArray> tmp215;
    compiler::TNode<Map> tmp216;
    compiler::TNode<BoolT> tmp217;
    compiler::TNode<BoolT> tmp218;
    compiler::TNode<BoolT> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<JSArray> tmp221;
    ca_.Bind(&block24, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 83);
    compiler::TNode<Smi> tmp222;
    USE(tmp222);
    tmp222 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadFastJSArrayLength(compiler::TNode<JSArray>{tmp221}));
    compiler::TNode<BoolT> tmp223;
    USE(tmp223);
    tmp223 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThanOrEqual(compiler::TNode<Smi>{tmp220}, compiler::TNode<Smi>{tmp222}));
    ca_.Branch(tmp223, &block22, &block23, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp224;
    compiler::TNode<JSReceiver> tmp225;
    compiler::TNode<Number> tmp226;
    compiler::TNode<JSReceiver> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Smi> tmp229;
    compiler::TNode<Smi> tmp230;
    compiler::TNode<JSArray> tmp231;
    compiler::TNode<JSArray> tmp232;
    compiler::TNode<JSArray> tmp233;
    compiler::TNode<Map> tmp234;
    compiler::TNode<BoolT> tmp235;
    compiler::TNode<BoolT> tmp236;
    compiler::TNode<BoolT> tmp237;
    ca_.Bind(&block22, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237);
    ca_.Goto(&block1, tmp229);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp238;
    compiler::TNode<JSReceiver> tmp239;
    compiler::TNode<Number> tmp240;
    compiler::TNode<JSReceiver> tmp241;
    compiler::TNode<Object> tmp242;
    compiler::TNode<Smi> tmp243;
    compiler::TNode<Smi> tmp244;
    compiler::TNode<JSArray> tmp245;
    compiler::TNode<JSArray> tmp246;
    compiler::TNode<JSArray> tmp247;
    compiler::TNode<Map> tmp248;
    compiler::TNode<BoolT> tmp249;
    compiler::TNode<BoolT> tmp250;
    compiler::TNode<BoolT> tmp251;
    ca_.Bind(&block23, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 84);
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 2180);
    ca_.Branch(tmp249, &block28, &block29, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, tmp243, tmp238, tmp243);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp252;
    compiler::TNode<JSReceiver> tmp253;
    compiler::TNode<Number> tmp254;
    compiler::TNode<JSReceiver> tmp255;
    compiler::TNode<Object> tmp256;
    compiler::TNode<Smi> tmp257;
    compiler::TNode<Smi> tmp258;
    compiler::TNode<JSArray> tmp259;
    compiler::TNode<JSArray> tmp260;
    compiler::TNode<JSArray> tmp261;
    compiler::TNode<Map> tmp262;
    compiler::TNode<BoolT> tmp263;
    compiler::TNode<BoolT> tmp264;
    compiler::TNode<BoolT> tmp265;
    compiler::TNode<Smi> tmp266;
    compiler::TNode<Context> tmp267;
    compiler::TNode<Smi> tmp268;
    ca_.Bind(&block28, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268);
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 2181);
    compiler::TNode<Object> tmp269;
    USE(tmp269);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp269 = BaseBuiltinsFromDSLAssembler(state_).LoadElementNoHole16FixedDoubleArray(compiler::TNode<Context>{tmp267}, compiler::TNode<JSArray>{tmp261}, compiler::TNode<Smi>{tmp268}, &label0);
    ca_.Goto(&block31, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp261, tmp268, tmp269);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block32, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp261, tmp268);
    }
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp270;
    compiler::TNode<JSReceiver> tmp271;
    compiler::TNode<Number> tmp272;
    compiler::TNode<JSReceiver> tmp273;
    compiler::TNode<Object> tmp274;
    compiler::TNode<Smi> tmp275;
    compiler::TNode<Smi> tmp276;
    compiler::TNode<JSArray> tmp277;
    compiler::TNode<JSArray> tmp278;
    compiler::TNode<JSArray> tmp279;
    compiler::TNode<Map> tmp280;
    compiler::TNode<BoolT> tmp281;
    compiler::TNode<BoolT> tmp282;
    compiler::TNode<BoolT> tmp283;
    compiler::TNode<Smi> tmp284;
    compiler::TNode<Context> tmp285;
    compiler::TNode<Smi> tmp286;
    compiler::TNode<JSArray> tmp287;
    compiler::TNode<Smi> tmp288;
    ca_.Bind(&block32, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288);
    ca_.Goto(&block26, tmp270, tmp271, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp289;
    compiler::TNode<JSReceiver> tmp290;
    compiler::TNode<Number> tmp291;
    compiler::TNode<JSReceiver> tmp292;
    compiler::TNode<Object> tmp293;
    compiler::TNode<Smi> tmp294;
    compiler::TNode<Smi> tmp295;
    compiler::TNode<JSArray> tmp296;
    compiler::TNode<JSArray> tmp297;
    compiler::TNode<JSArray> tmp298;
    compiler::TNode<Map> tmp299;
    compiler::TNode<BoolT> tmp300;
    compiler::TNode<BoolT> tmp301;
    compiler::TNode<BoolT> tmp302;
    compiler::TNode<Smi> tmp303;
    compiler::TNode<Context> tmp304;
    compiler::TNode<Smi> tmp305;
    compiler::TNode<JSArray> tmp306;
    compiler::TNode<Smi> tmp307;
    compiler::TNode<Object> tmp308;
    ca_.Bind(&block31, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308);
    ca_.Goto(&block27, tmp289, tmp290, tmp291, tmp292, tmp293, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp308);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp309;
    compiler::TNode<JSReceiver> tmp310;
    compiler::TNode<Number> tmp311;
    compiler::TNode<JSReceiver> tmp312;
    compiler::TNode<Object> tmp313;
    compiler::TNode<Smi> tmp314;
    compiler::TNode<Smi> tmp315;
    compiler::TNode<JSArray> tmp316;
    compiler::TNode<JSArray> tmp317;
    compiler::TNode<JSArray> tmp318;
    compiler::TNode<Map> tmp319;
    compiler::TNode<BoolT> tmp320;
    compiler::TNode<BoolT> tmp321;
    compiler::TNode<BoolT> tmp322;
    compiler::TNode<Smi> tmp323;
    compiler::TNode<Context> tmp324;
    compiler::TNode<Smi> tmp325;
    ca_.Bind(&block29, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325);
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 2184);
    compiler::TNode<Object> tmp326;
    USE(tmp326);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp326 = BaseBuiltinsFromDSLAssembler(state_).LoadElementNoHole10FixedArray(compiler::TNode<Context>{tmp324}, compiler::TNode<JSArray>{tmp318}, compiler::TNode<Smi>{tmp325}, &label0);
    ca_.Goto(&block33, tmp309, tmp310, tmp311, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp318, tmp325, tmp326);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block34, tmp309, tmp310, tmp311, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp318, tmp325);
    }
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp327;
    compiler::TNode<JSReceiver> tmp328;
    compiler::TNode<Number> tmp329;
    compiler::TNode<JSReceiver> tmp330;
    compiler::TNode<Object> tmp331;
    compiler::TNode<Smi> tmp332;
    compiler::TNode<Smi> tmp333;
    compiler::TNode<JSArray> tmp334;
    compiler::TNode<JSArray> tmp335;
    compiler::TNode<JSArray> tmp336;
    compiler::TNode<Map> tmp337;
    compiler::TNode<BoolT> tmp338;
    compiler::TNode<BoolT> tmp339;
    compiler::TNode<BoolT> tmp340;
    compiler::TNode<Smi> tmp341;
    compiler::TNode<Context> tmp342;
    compiler::TNode<Smi> tmp343;
    compiler::TNode<JSArray> tmp344;
    compiler::TNode<Smi> tmp345;
    ca_.Bind(&block34, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345);
    ca_.Goto(&block26, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp333, tmp334, tmp335, tmp336, tmp337, tmp338, tmp339, tmp340);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp346;
    compiler::TNode<JSReceiver> tmp347;
    compiler::TNode<Number> tmp348;
    compiler::TNode<JSReceiver> tmp349;
    compiler::TNode<Object> tmp350;
    compiler::TNode<Smi> tmp351;
    compiler::TNode<Smi> tmp352;
    compiler::TNode<JSArray> tmp353;
    compiler::TNode<JSArray> tmp354;
    compiler::TNode<JSArray> tmp355;
    compiler::TNode<Map> tmp356;
    compiler::TNode<BoolT> tmp357;
    compiler::TNode<BoolT> tmp358;
    compiler::TNode<BoolT> tmp359;
    compiler::TNode<Smi> tmp360;
    compiler::TNode<Context> tmp361;
    compiler::TNode<Smi> tmp362;
    compiler::TNode<JSArray> tmp363;
    compiler::TNode<Smi> tmp364;
    compiler::TNode<Object> tmp365;
    ca_.Bind(&block33, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365);
    ca_.Goto(&block27, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, tmp357, tmp358, tmp359, tmp360, tmp361, tmp362, tmp365);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp366;
    compiler::TNode<JSReceiver> tmp367;
    compiler::TNode<Number> tmp368;
    compiler::TNode<JSReceiver> tmp369;
    compiler::TNode<Object> tmp370;
    compiler::TNode<Smi> tmp371;
    compiler::TNode<Smi> tmp372;
    compiler::TNode<JSArray> tmp373;
    compiler::TNode<JSArray> tmp374;
    compiler::TNode<JSArray> tmp375;
    compiler::TNode<Map> tmp376;
    compiler::TNode<BoolT> tmp377;
    compiler::TNode<BoolT> tmp378;
    compiler::TNode<BoolT> tmp379;
    compiler::TNode<Smi> tmp380;
    compiler::TNode<Context> tmp381;
    compiler::TNode<Smi> tmp382;
    compiler::TNode<Object> tmp383;
    ca_.Bind(&block27, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 84);
    ca_.Goto(&block25, tmp366, tmp367, tmp368, tmp369, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp383);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp384;
    compiler::TNode<JSReceiver> tmp385;
    compiler::TNode<Number> tmp386;
    compiler::TNode<JSReceiver> tmp387;
    compiler::TNode<Object> tmp388;
    compiler::TNode<Smi> tmp389;
    compiler::TNode<Smi> tmp390;
    compiler::TNode<JSArray> tmp391;
    compiler::TNode<JSArray> tmp392;
    compiler::TNode<JSArray> tmp393;
    compiler::TNode<Map> tmp394;
    compiler::TNode<BoolT> tmp395;
    compiler::TNode<BoolT> tmp396;
    compiler::TNode<BoolT> tmp397;
    ca_.Bind(&block26, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 85);
    ca_.Goto(&block14, tmp384, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp396, tmp397);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp398;
    compiler::TNode<JSReceiver> tmp399;
    compiler::TNode<Number> tmp400;
    compiler::TNode<JSReceiver> tmp401;
    compiler::TNode<Object> tmp402;
    compiler::TNode<Smi> tmp403;
    compiler::TNode<Smi> tmp404;
    compiler::TNode<JSArray> tmp405;
    compiler::TNode<JSArray> tmp406;
    compiler::TNode<JSArray> tmp407;
    compiler::TNode<Map> tmp408;
    compiler::TNode<BoolT> tmp409;
    compiler::TNode<BoolT> tmp410;
    compiler::TNode<BoolT> tmp411;
    compiler::TNode<Object> tmp412;
    ca_.Bind(&block25, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 84);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 86);
    ca_.SetSourcePosition("../../v8/src/builtins/base.tq", 2163);
    ca_.Goto(&block35, tmp398, tmp399, tmp400, tmp401, tmp402, tmp403, tmp404, tmp405, tmp406, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp398, tmp401, tmp402, tmp412, tmp403, tmp407);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp413;
    compiler::TNode<JSReceiver> tmp414;
    compiler::TNode<Number> tmp415;
    compiler::TNode<JSReceiver> tmp416;
    compiler::TNode<Object> tmp417;
    compiler::TNode<Smi> tmp418;
    compiler::TNode<Smi> tmp419;
    compiler::TNode<JSArray> tmp420;
    compiler::TNode<JSArray> tmp421;
    compiler::TNode<JSArray> tmp422;
    compiler::TNode<Map> tmp423;
    compiler::TNode<BoolT> tmp424;
    compiler::TNode<BoolT> tmp425;
    compiler::TNode<BoolT> tmp426;
    compiler::TNode<Object> tmp427;
    compiler::TNode<Context> tmp428;
    compiler::TNode<JSReceiver> tmp429;
    compiler::TNode<Object> tmp430;
    compiler::TNode<Object> tmp431;
    compiler::TNode<Smi> tmp432;
    compiler::TNode<JSArray> tmp433;
    ca_.Bind(&block35, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420, &tmp421, &tmp422, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 86);
    compiler::TNode<Object> tmp434;
    USE(tmp434);
    tmp434 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp428}, compiler::TNode<JSReceiver>{tmp429}, compiler::TNode<Object>{tmp430}, compiler::TNode<Object>{tmp431}, compiler::TNode<Object>{tmp432}, compiler::TNode<Object>{tmp433}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 79);
    ca_.Goto(&block14, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420, tmp421, tmp422, tmp423, tmp424, tmp425, tmp426);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp435;
    compiler::TNode<JSReceiver> tmp436;
    compiler::TNode<Number> tmp437;
    compiler::TNode<JSReceiver> tmp438;
    compiler::TNode<Object> tmp439;
    compiler::TNode<Smi> tmp440;
    compiler::TNode<Smi> tmp441;
    compiler::TNode<JSArray> tmp442;
    compiler::TNode<JSArray> tmp443;
    compiler::TNode<JSArray> tmp444;
    compiler::TNode<Map> tmp445;
    compiler::TNode<BoolT> tmp446;
    compiler::TNode<BoolT> tmp447;
    compiler::TNode<BoolT> tmp448;
    ca_.Bind(&block14, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442, &tmp443, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448);
    compiler::TNode<Smi> tmp449;
    USE(tmp449);
    tmp449 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp450;
    USE(tmp450);
    tmp450 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp440}, compiler::TNode<Smi>{tmp449}));
    ca_.Goto(&block13, tmp435, tmp436, tmp437, tmp438, tmp439, tmp450, tmp441, tmp442, tmp443, tmp444, tmp445, tmp446, tmp447, tmp448);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp451;
    compiler::TNode<JSReceiver> tmp452;
    compiler::TNode<Number> tmp453;
    compiler::TNode<JSReceiver> tmp454;
    compiler::TNode<Object> tmp455;
    compiler::TNode<Smi> tmp456;
    compiler::TNode<Smi> tmp457;
    compiler::TNode<JSArray> tmp458;
    compiler::TNode<JSArray> tmp459;
    compiler::TNode<JSArray> tmp460;
    compiler::TNode<Map> tmp461;
    compiler::TNode<BoolT> tmp462;
    compiler::TNode<BoolT> tmp463;
    compiler::TNode<BoolT> tmp464;
    ca_.Bind(&block12, &tmp451, &tmp452, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463, &tmp464);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 88);
    compiler::TNode<Oddball> tmp465;
    USE(tmp465);
    tmp465 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block2, tmp451, tmp452, tmp453, tmp454, tmp455, tmp465);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp466;
    compiler::TNode<JSReceiver> tmp467;
    compiler::TNode<Number> tmp468;
    compiler::TNode<JSReceiver> tmp469;
    compiler::TNode<Object> tmp470;
    compiler::TNode<Object> tmp471;
    ca_.Bind(&block2, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 70);
    ca_.Goto(&block36, tmp466, tmp467, tmp468, tmp469, tmp470, tmp471);
  }

  if (block1.is_used()) {
    compiler::TNode<Smi> tmp472;
    ca_.Bind(&block1, &tmp472);
    *label_Bailout_parameter_0 = tmp472;
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp473;
    compiler::TNode<JSReceiver> tmp474;
    compiler::TNode<Number> tmp475;
    compiler::TNode<JSReceiver> tmp476;
    compiler::TNode<Object> tmp477;
    compiler::TNode<Object> tmp478;
    ca_.Bind(&block36, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478);
  return compiler::TNode<Object>{tmp478};
}

TF_BUILTIN(ArrayForEach, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, JSReceiver> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, JSReceiver, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, JSReceiver, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, JSReceiver, Object, Number, JSReceiver, Number, JSReceiver, Object, Smi> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, JSReceiver, Object, Number, JSReceiver, Number, JSReceiver, Object, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, JSReceiver, Object, Number, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, JSReceiver, Object, Number, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 95);
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{tmp1}));
    ca_.Branch(tmp5, &block5, &block6, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<RawPtrT> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    ca_.Bind(&block5, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 96);
    ca_.Goto(&block2, tmp6, tmp7, tmp8, tmp9, tmp10);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    ca_.Bind(&block6, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 100);
    compiler::TNode<JSReceiver> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp12}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 103);
    compiler::TNode<Number> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp16}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 106);
    compiler::TNode<IntPtrT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp15}, compiler::TNode<IntPtrT>{tmp18}));
    ca_.Branch(tmp19, &block7, &block8, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Number> tmp26;
    ca_.Bind(&block7, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 107);
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23, tmp24);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<RawPtrT> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Number> tmp33;
    ca_.Bind(&block8, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 109);
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp29}, compiler::TNode<RawPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp31}}, compiler::TNode<IntPtrT>{tmp34}));
    compiler::TNode<JSReceiver> tmp36;
    USE(tmp36);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp36 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp27}, compiler::TNode<Object>{tmp35}, &label0);
    ca_.Goto(&block9, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp35, tmp36);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp35);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<RawPtrT> tmp39;
    compiler::TNode<RawPtrT> tmp40;
    compiler::TNode<IntPtrT> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<Number> tmp43;
    compiler::TNode<Object> tmp44;
    ca_.Bind(&block10, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.Goto(&block4, tmp37, tmp38, tmp39, tmp40, tmp41);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<RawPtrT> tmp47;
    compiler::TNode<RawPtrT> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    compiler::TNode<JSReceiver> tmp50;
    compiler::TNode<Number> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    ca_.Bind(&block9, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 112);
    compiler::TNode<IntPtrT> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp49}, compiler::TNode<IntPtrT>{tmp54}));
    ca_.Branch(tmp55, &block11, &block12, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp53);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<JSReceiver> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<JSReceiver> tmp63;
    ca_.Bind(&block11, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63);
    compiler::TNode<IntPtrT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp58}, compiler::TNode<RawPtrT>{tmp59}, compiler::TNode<IntPtrT>{tmp60}}, compiler::TNode<IntPtrT>{tmp64}));
    ca_.Goto(&block14, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp65);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<RawPtrT> tmp69;
    compiler::TNode<IntPtrT> tmp70;
    compiler::TNode<JSReceiver> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<JSReceiver> tmp73;
    ca_.Bind(&block12, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    compiler::TNode<Oddball> tmp74;
    USE(tmp74);
    tmp74 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block13, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<RawPtrT> tmp78;
    compiler::TNode<IntPtrT> tmp79;
    compiler::TNode<JSReceiver> tmp80;
    compiler::TNode<Number> tmp81;
    compiler::TNode<JSReceiver> tmp82;
    compiler::TNode<Object> tmp83;
    ca_.Bind(&block14, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83);
    ca_.Goto(&block13, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<RawPtrT> tmp86;
    compiler::TNode<RawPtrT> tmp87;
    compiler::TNode<IntPtrT> tmp88;
    compiler::TNode<JSReceiver> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<Object> tmp92;
    ca_.Bind(&block13, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 115);
    compiler::TNode<Number> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 117);
    compiler::TNode<Object> tmp94;
    USE(tmp94);
    compiler::TypedCodeAssemblerVariable<Smi> result_0_0(&ca_);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp94 = ArrayForeachBuiltinsFromDSLAssembler(state_).FastArrayForEach(compiler::TNode<Context>{tmp84}, compiler::TNode<JSReceiver>{tmp89}, compiler::TNode<Number>{tmp90}, compiler::TNode<JSReceiver>{tmp91}, compiler::TNode<Object>{tmp92}, &label0, &result_0_0);
    ca_.Goto(&block17, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp89, tmp90, tmp91, tmp92, tmp94);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block18, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp89, tmp90, tmp91, tmp92, result_0_0.value());
    }
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<RawPtrT> tmp97;
    compiler::TNode<RawPtrT> tmp98;
    compiler::TNode<IntPtrT> tmp99;
    compiler::TNode<JSReceiver> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<JSReceiver> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Number> tmp104;
    compiler::TNode<JSReceiver> tmp105;
    compiler::TNode<Number> tmp106;
    compiler::TNode<JSReceiver> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<Smi> tmp109;
    ca_.Bind(&block18, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109);
    ca_.Goto(&block16, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp109);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<RawPtrT> tmp112;
    compiler::TNode<RawPtrT> tmp113;
    compiler::TNode<IntPtrT> tmp114;
    compiler::TNode<JSReceiver> tmp115;
    compiler::TNode<Number> tmp116;
    compiler::TNode<JSReceiver> tmp117;
    compiler::TNode<Object> tmp118;
    compiler::TNode<Number> tmp119;
    compiler::TNode<JSReceiver> tmp120;
    compiler::TNode<Number> tmp121;
    compiler::TNode<JSReceiver> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<Object> tmp124;
    ca_.Bind(&block17, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124);
    arguments.PopAndReturn(tmp124);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp125;
    compiler::TNode<Object> tmp126;
    compiler::TNode<RawPtrT> tmp127;
    compiler::TNode<RawPtrT> tmp128;
    compiler::TNode<IntPtrT> tmp129;
    compiler::TNode<JSReceiver> tmp130;
    compiler::TNode<Number> tmp131;
    compiler::TNode<JSReceiver> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<Number> tmp134;
    compiler::TNode<Smi> tmp135;
    ca_.Bind(&block16, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 121);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 116);
    ca_.Goto(&block15, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp135, tmp135);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<RawPtrT> tmp138;
    compiler::TNode<RawPtrT> tmp139;
    compiler::TNode<IntPtrT> tmp140;
    compiler::TNode<JSReceiver> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<JSReceiver> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<Number> tmp145;
    compiler::TNode<Smi> tmp146;
    ca_.Bind(&block15, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 125);
    compiler::TNode<Oddball> tmp147;
    USE(tmp147);
    tmp147 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Oddball> tmp148;
    USE(tmp148);
    tmp148 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 124);
    compiler::TNode<Object> tmp149;
    tmp149 = CodeStubAssembler(state_).CallBuiltin(Builtins::kArrayForEachLoopContinuation, tmp136, tmp141, tmp143, tmp144, tmp147, tmp141, tmp145, tmp142, tmp148);
    USE(tmp149);
    arguments.PopAndReturn(tmp149);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp150;
    compiler::TNode<Object> tmp151;
    compiler::TNode<RawPtrT> tmp152;
    compiler::TNode<RawPtrT> tmp153;
    compiler::TNode<IntPtrT> tmp154;
    ca_.Bind(&block4, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 128);
    compiler::TNode<IntPtrT> tmp155;
    USE(tmp155);
    tmp155 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp156;
    USE(tmp156);
    tmp156 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp152}, compiler::TNode<RawPtrT>{tmp153}, compiler::TNode<IntPtrT>{tmp154}}, compiler::TNode<IntPtrT>{tmp155}));
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp150}, MessageTemplate::kCalledNonCallable, compiler::TNode<Object>{tmp156});
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp157;
    compiler::TNode<Object> tmp158;
    compiler::TNode<RawPtrT> tmp159;
    compiler::TNode<RawPtrT> tmp160;
    compiler::TNode<IntPtrT> tmp161;
    ca_.Bind(&block2, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161);
    ca_.SetSourcePosition("../../v8/src/builtins/array-foreach.tq", 131);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp157}, MessageTemplate::kCalledOnNullOrUndefined, "Array.prototype.forEach");
  }
}

}  // namespace internal
}  // namespace v8

