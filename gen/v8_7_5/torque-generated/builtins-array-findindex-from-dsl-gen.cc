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

TF_BUILTIN(ArrayFindIndexLoopEagerDeoptContinuation, CodeStubAssembler) {
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
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 17);
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
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-findindex.tq:17:61");
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
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 18);
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
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-findindex.tq:18:59");
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
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 19);
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
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-findindex.tq:19:54");
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
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 20);
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
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-findindex.tq:20:57");
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
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 23);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 22);
    compiler::TNode<Number> tmp146;
    tmp146 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kArrayFindIndexLoopContinuation, tmp136, tmp142, tmp143, tmp139, tmp142, tmp144, tmp145));
    USE(tmp146);
    CodeStubAssembler(state_).Return(tmp146);
  }
}

TF_BUILTIN(ArrayFindIndexLoopLazyDeoptContinuation, CodeStubAssembler) {
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
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 33);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-findindex.tq:33:5");
    CodeStubAssembler(state_).Unreachable();
  }
}

TF_BUILTIN(ArrayFindIndexLoopAfterCallbackLazyDeoptContinuation, CodeStubAssembler) {
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
  compiler::TNode<Object> parameter6 = UncheckedCast<Object>(Parameter(Descriptor::kFoundValue));
  USE(parameter6);
  compiler::TNode<Object> parameter7 = UncheckedCast<Object>(Parameter(Descriptor::kIsFound));
  USE(parameter7);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, Object, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, Object, JSReceiver> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Object, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Object, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Number> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Number> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, Object, Object, JSReceiver, JSReceiver, Number, Number> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3, parameter4, parameter5, parameter6, parameter7);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<Object> tmp6;
    compiler::TNode<Object> tmp7;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 47);
    compiler::TNode<JSReceiver> tmp8;
    USE(tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp8 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp1, tmp8);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp1);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Object> tmp17;
    ca_.Bind(&block4, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.Goto(&block2, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<JSReceiver> tmp27;
    ca_.Bind(&block3, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block1, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp27);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    ca_.Bind(&block2, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-findindex.tq:47:61");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<JSReceiver> tmp44;
    ca_.Bind(&block1, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 48);
    compiler::TNode<JSReceiver> tmp45;
    USE(tmp45);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp45 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp36}, compiler::TNode<Object>{tmp38}, &label0);
    ca_.Goto(&block7, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp38, tmp45);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp38);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block6, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<JSReceiver> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<JSReceiver> tmp66;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.Goto(&block5, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp66);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<JSReceiver> tmp75;
    ca_.Bind(&block6, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-findindex.tq:48:59");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<JSReceiver> tmp84;
    compiler::TNode<JSReceiver> tmp85;
    ca_.Bind(&block5, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 49);
    compiler::TNode<Number> tmp86;
    USE(tmp86);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp86 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{tmp80}, &label0);
    ca_.Goto(&block11, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp80, tmp86);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp80);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<JSReceiver> tmp95;
    compiler::TNode<JSReceiver> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block12, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.Goto(&block10, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp98;
    compiler::TNode<Object> tmp99;
    compiler::TNode<Object> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<JSReceiver> tmp106;
    compiler::TNode<JSReceiver> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<Number> tmp109;
    ca_.Bind(&block11, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109);
    ca_.Goto(&block9, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp109);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<Object> tmp115;
    compiler::TNode<Object> tmp116;
    compiler::TNode<Object> tmp117;
    compiler::TNode<JSReceiver> tmp118;
    compiler::TNode<JSReceiver> tmp119;
    ca_.Bind(&block10, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-findindex.tq:49:52");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<Object> tmp126;
    compiler::TNode<Object> tmp127;
    compiler::TNode<JSReceiver> tmp128;
    compiler::TNode<JSReceiver> tmp129;
    compiler::TNode<Number> tmp130;
    ca_.Bind(&block9, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 50);
    compiler::TNode<Number> tmp131;
    USE(tmp131);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp131 = BaseBuiltinsFromDSLAssembler(state_).Cast20UT5ATSmi10HeapNumber(compiler::TNode<Object>{tmp125}, &label0);
    ca_.Goto(&block15, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp125, tmp131);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block16, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp125);
    }
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<JSReceiver> tmp140;
    compiler::TNode<JSReceiver> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<Object> tmp143;
    ca_.Bind(&block16, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143);
    ca_.Goto(&block14, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp144;
    compiler::TNode<Object> tmp145;
    compiler::TNode<Object> tmp146;
    compiler::TNode<Object> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<Object> tmp149;
    compiler::TNode<Object> tmp150;
    compiler::TNode<Object> tmp151;
    compiler::TNode<JSReceiver> tmp152;
    compiler::TNode<JSReceiver> tmp153;
    compiler::TNode<Number> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<Number> tmp156;
    ca_.Bind(&block15, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156);
    ca_.Goto(&block13, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp156);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp157;
    compiler::TNode<Object> tmp158;
    compiler::TNode<Object> tmp159;
    compiler::TNode<Object> tmp160;
    compiler::TNode<Object> tmp161;
    compiler::TNode<Object> tmp162;
    compiler::TNode<Object> tmp163;
    compiler::TNode<Object> tmp164;
    compiler::TNode<JSReceiver> tmp165;
    compiler::TNode<JSReceiver> tmp166;
    compiler::TNode<Number> tmp167;
    ca_.Bind(&block14, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/array-findindex.tq:50:57");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp168;
    compiler::TNode<Object> tmp169;
    compiler::TNode<Object> tmp170;
    compiler::TNode<Object> tmp171;
    compiler::TNode<Object> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<Object> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<JSReceiver> tmp176;
    compiler::TNode<JSReceiver> tmp177;
    compiler::TNode<Number> tmp178;
    compiler::TNode<Number> tmp179;
    ca_.Bind(&block13, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 57);
    compiler::TNode<BoolT> tmp180;
    USE(tmp180);
    tmp180 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).ToBoolean(compiler::TNode<Object>{tmp175}));
    ca_.Branch(tmp180, &block17, &block18, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp181;
    compiler::TNode<Object> tmp182;
    compiler::TNode<Object> tmp183;
    compiler::TNode<Object> tmp184;
    compiler::TNode<Object> tmp185;
    compiler::TNode<Object> tmp186;
    compiler::TNode<Object> tmp187;
    compiler::TNode<Object> tmp188;
    compiler::TNode<JSReceiver> tmp189;
    compiler::TNode<JSReceiver> tmp190;
    compiler::TNode<Number> tmp191;
    compiler::TNode<Number> tmp192;
    ca_.Bind(&block17, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 58);
    CodeStubAssembler(state_).Return(tmp187);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp193;
    compiler::TNode<Object> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<Object> tmp197;
    compiler::TNode<Object> tmp198;
    compiler::TNode<Object> tmp199;
    compiler::TNode<Object> tmp200;
    compiler::TNode<JSReceiver> tmp201;
    compiler::TNode<JSReceiver> tmp202;
    compiler::TNode<Number> tmp203;
    compiler::TNode<Number> tmp204;
    ca_.Bind(&block18, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 62);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 61);
    compiler::TNode<Number> tmp205;
    tmp205 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kArrayFindIndexLoopContinuation, tmp193, tmp201, tmp202, tmp196, tmp201, tmp203, tmp204));
    USE(tmp205);
    CodeStubAssembler(state_).Return(tmp205);
  }
}

TF_BUILTIN(ArrayFindIndexLoopContinuation, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSReceiver> parameter1 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<JSReceiver> parameter2 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kCallbackfn));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kThisArg));
  USE(parameter3);
  compiler::TNode<JSReceiver> parameter4 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kO));
  USE(parameter4);
  compiler::TNode<Number> parameter5 = UncheckedCast<Number>(Parameter(Descriptor::kInitialK));
  USE(parameter5);
  compiler::TNode<Number> parameter6 = UncheckedCast<Number>(Parameter(Descriptor::kLength));
  USE(parameter6);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, JSReceiver, Number, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, JSReceiver, Number, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, JSReceiver, Number, Number, Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, JSReceiver, Number, Number, Number, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, JSReceiver, Number, Number, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, JSReceiver, Number, Number, Number, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, JSReceiver, Number, Number, Number, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, JSReceiver, Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, Object, JSReceiver, Number, Number, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3, parameter4, parameter5, parameter6);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<JSReceiver> tmp4;
    compiler::TNode<Number> tmp5;
    compiler::TNode<Number> tmp6;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 71);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp5);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<JSReceiver> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<Number> tmp14;
    ca_.Bind(&block3, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp14}, compiler::TNode<Number>{tmp13}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp14, tmp13);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp14, tmp13);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<JSReceiver> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Number> tmp21;
    compiler::TNode<Number> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Number> tmp24;
    ca_.Bind(&block5, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block1, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<JSReceiver> tmp26;
    compiler::TNode<JSReceiver> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Number> tmp31;
    compiler::TNode<Number> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<Number> tmp34;
    ca_.Bind(&block6, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.Goto(&block2, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<JSReceiver> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Number> tmp41;
    compiler::TNode<Number> tmp42;
    ca_.Bind(&block1, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 77);
    compiler::TNode<Object> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp35}, compiler::TNode<Object>{tmp39}, compiler::TNode<Object>{tmp42}));
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 82);
    compiler::TNode<Object> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp35}, compiler::TNode<JSReceiver>{tmp37}, compiler::TNode<Object>{tmp38}, compiler::TNode<Object>{tmp43}, compiler::TNode<Object>{tmp42}, compiler::TNode<Object>{tmp39}));
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 81);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 85);
    compiler::TNode<BoolT> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).ToBoolean(compiler::TNode<Object>{tmp44}));
    ca_.Branch(tmp45, &block7, &block8, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp46;
    compiler::TNode<JSReceiver> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<JSReceiver> tmp50;
    compiler::TNode<Number> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block7, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 86);
    CodeStubAssembler(state_).Return(tmp53);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<Object> tmp65;
    ca_.Bind(&block8, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 71);
    ca_.Goto(&block4, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<JSReceiver> tmp67;
    compiler::TNode<JSReceiver> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<JSReceiver> tmp70;
    compiler::TNode<Number> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<Number> tmp73;
    ca_.Bind(&block4, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    compiler::TNode<Number> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp73}, compiler::TNode<Number>{tmp74}));
    ca_.Goto(&block3, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp75);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp76;
    compiler::TNode<JSReceiver> tmp77;
    compiler::TNode<JSReceiver> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<JSReceiver> tmp80;
    compiler::TNode<Number> tmp81;
    compiler::TNode<Number> tmp82;
    compiler::TNode<Number> tmp83;
    ca_.Bind(&block2, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 91);
    compiler::TNode<Smi> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi17ATconstexpr_int31(-1));
    CodeStubAssembler(state_).Return(tmp84);
  }
}

compiler::TNode<Number> ArrayFindindexBuiltinsFromDSLAssembler::FastArrayFindIndex(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_thisArg, compiler::CodeAssemblerLabel* label_Bailout, compiler::TypedCodeAssemblerVariable<Smi>* label_Bailout_parameter_0) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, JSArray, Smi> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, JSArray, Smi, Object> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, JSArray, Smi> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, JSArray, Smi, Object> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, Object> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Object> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Object, Context, JSReceiver, Object, Object, Smi, JSArray> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Object, Object> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Object, Object> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Smi, Smi, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, JSReceiver, Object, Number> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_len, p_callbackfn, p_thisArg);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<JSReceiver> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 97);
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 98);
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
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 99);
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
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 100);
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
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 103);
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
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1945);
    compiler::TNode<Map> tmp103;
    USE(tmp103);
    tmp103 = ca_.UncheckedCast<Map>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapObjectMap(compiler::TNode<HeapObject>{tmp97}));
    compiler::TNode<BoolT> tmp104;
    USE(tmp104);
    tmp104 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<HeapObject>{tmp103}, compiler::TNode<HeapObject>{tmp99}));
    ca_.Branch(tmp104, &block18, &block19, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp105;
    compiler::TNode<JSReceiver> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<JSReceiver> tmp108;
    compiler::TNode<Object> tmp109;
    compiler::TNode<Smi> tmp110;
    compiler::TNode<Smi> tmp111;
    compiler::TNode<JSArray> tmp112;
    compiler::TNode<JSArray> tmp113;
    compiler::TNode<JSArray> tmp114;
    compiler::TNode<Map> tmp115;
    compiler::TNode<BoolT> tmp116;
    compiler::TNode<BoolT> tmp117;
    compiler::TNode<BoolT> tmp118;
    ca_.Bind(&block18, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
    ca_.Goto(&block16, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp119;
    compiler::TNode<JSReceiver> tmp120;
    compiler::TNode<Number> tmp121;
    compiler::TNode<JSReceiver> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<Smi> tmp124;
    compiler::TNode<Smi> tmp125;
    compiler::TNode<JSArray> tmp126;
    compiler::TNode<JSArray> tmp127;
    compiler::TNode<JSArray> tmp128;
    compiler::TNode<Map> tmp129;
    compiler::TNode<BoolT> tmp130;
    compiler::TNode<BoolT> tmp131;
    compiler::TNode<BoolT> tmp132;
    ca_.Bind(&block19, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1952);
    compiler::TNode<BoolT> tmp133;
    USE(tmp133);
    tmp133 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNoElementsProtectorCellInvalid());
    ca_.Branch(tmp133, &block20, &block21, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp134;
    compiler::TNode<JSReceiver> tmp135;
    compiler::TNode<Number> tmp136;
    compiler::TNode<JSReceiver> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<Smi> tmp139;
    compiler::TNode<Smi> tmp140;
    compiler::TNode<JSArray> tmp141;
    compiler::TNode<JSArray> tmp142;
    compiler::TNode<JSArray> tmp143;
    compiler::TNode<Map> tmp144;
    compiler::TNode<BoolT> tmp145;
    compiler::TNode<BoolT> tmp146;
    compiler::TNode<BoolT> tmp147;
    ca_.Bind(&block20, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147);
    ca_.Goto(&block16, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp148;
    compiler::TNode<JSReceiver> tmp149;
    compiler::TNode<Number> tmp150;
    compiler::TNode<JSReceiver> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<Smi> tmp154;
    compiler::TNode<JSArray> tmp155;
    compiler::TNode<JSArray> tmp156;
    compiler::TNode<JSArray> tmp157;
    compiler::TNode<Map> tmp158;
    compiler::TNode<BoolT> tmp159;
    compiler::TNode<BoolT> tmp160;
    compiler::TNode<BoolT> tmp161;
    ca_.Bind(&block21, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1953);
    compiler::TNode<JSArray> tmp162;
    USE(tmp162);
    tmp162 = (compiler::TNode<JSArray>{tmp156});
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 104);
    ca_.Goto(&block17, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp162, tmp158, tmp159, tmp160, tmp161);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp163;
    compiler::TNode<JSReceiver> tmp164;
    compiler::TNode<Number> tmp165;
    compiler::TNode<JSReceiver> tmp166;
    compiler::TNode<Object> tmp167;
    compiler::TNode<Smi> tmp168;
    compiler::TNode<Smi> tmp169;
    compiler::TNode<JSArray> tmp170;
    compiler::TNode<JSArray> tmp171;
    compiler::TNode<JSArray> tmp172;
    compiler::TNode<Map> tmp173;
    compiler::TNode<BoolT> tmp174;
    compiler::TNode<BoolT> tmp175;
    compiler::TNode<BoolT> tmp176;
    ca_.Bind(&block17, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176);
    ca_.Goto(&block15, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp177;
    compiler::TNode<JSReceiver> tmp178;
    compiler::TNode<Number> tmp179;
    compiler::TNode<JSReceiver> tmp180;
    compiler::TNode<Object> tmp181;
    compiler::TNode<Smi> tmp182;
    compiler::TNode<Smi> tmp183;
    compiler::TNode<JSArray> tmp184;
    compiler::TNode<JSArray> tmp185;
    compiler::TNode<JSArray> tmp186;
    compiler::TNode<Map> tmp187;
    compiler::TNode<BoolT> tmp188;
    compiler::TNode<BoolT> tmp189;
    compiler::TNode<BoolT> tmp190;
    ca_.Bind(&block16, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190);
    ca_.Goto(&block1, tmp182);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp191;
    compiler::TNode<JSReceiver> tmp192;
    compiler::TNode<Number> tmp193;
    compiler::TNode<JSReceiver> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<Smi> tmp197;
    compiler::TNode<JSArray> tmp198;
    compiler::TNode<JSArray> tmp199;
    compiler::TNode<JSArray> tmp200;
    compiler::TNode<Map> tmp201;
    compiler::TNode<BoolT> tmp202;
    compiler::TNode<BoolT> tmp203;
    compiler::TNode<BoolT> tmp204;
    ca_.Bind(&block15, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 107);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1941);
    ca_.Goto(&block24, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp196, tmp200);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp205;
    compiler::TNode<JSReceiver> tmp206;
    compiler::TNode<Number> tmp207;
    compiler::TNode<JSReceiver> tmp208;
    compiler::TNode<Object> tmp209;
    compiler::TNode<Smi> tmp210;
    compiler::TNode<Smi> tmp211;
    compiler::TNode<JSArray> tmp212;
    compiler::TNode<JSArray> tmp213;
    compiler::TNode<JSArray> tmp214;
    compiler::TNode<Map> tmp215;
    compiler::TNode<BoolT> tmp216;
    compiler::TNode<BoolT> tmp217;
    compiler::TNode<BoolT> tmp218;
    compiler::TNode<Smi> tmp219;
    compiler::TNode<JSArray> tmp220;
    ca_.Bind(&block24, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 107);
    compiler::TNode<Smi> tmp221;
    USE(tmp221);
    tmp221 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadFastJSArrayLength(compiler::TNode<JSArray>{tmp220}));
    compiler::TNode<BoolT> tmp222;
    USE(tmp222);
    tmp222 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThanOrEqual(compiler::TNode<Smi>{tmp219}, compiler::TNode<Smi>{tmp221}));
    ca_.Branch(tmp222, &block22, &block23, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp223;
    compiler::TNode<JSReceiver> tmp224;
    compiler::TNode<Number> tmp225;
    compiler::TNode<JSReceiver> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<Smi> tmp228;
    compiler::TNode<Smi> tmp229;
    compiler::TNode<JSArray> tmp230;
    compiler::TNode<JSArray> tmp231;
    compiler::TNode<JSArray> tmp232;
    compiler::TNode<Map> tmp233;
    compiler::TNode<BoolT> tmp234;
    compiler::TNode<BoolT> tmp235;
    compiler::TNode<BoolT> tmp236;
    ca_.Bind(&block22, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236);
    ca_.Goto(&block1, tmp228);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp237;
    compiler::TNode<JSReceiver> tmp238;
    compiler::TNode<Number> tmp239;
    compiler::TNode<JSReceiver> tmp240;
    compiler::TNode<Object> tmp241;
    compiler::TNode<Smi> tmp242;
    compiler::TNode<Smi> tmp243;
    compiler::TNode<JSArray> tmp244;
    compiler::TNode<JSArray> tmp245;
    compiler::TNode<JSArray> tmp246;
    compiler::TNode<Map> tmp247;
    compiler::TNode<BoolT> tmp248;
    compiler::TNode<BoolT> tmp249;
    compiler::TNode<BoolT> tmp250;
    ca_.Bind(&block23, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 109);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1969);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1958);
    ca_.Branch(tmp248, &block29, &block30, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp242, tmp237, tmp242, tmp242, tmp237, tmp242);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp251;
    compiler::TNode<JSReceiver> tmp252;
    compiler::TNode<Number> tmp253;
    compiler::TNode<JSReceiver> tmp254;
    compiler::TNode<Object> tmp255;
    compiler::TNode<Smi> tmp256;
    compiler::TNode<Smi> tmp257;
    compiler::TNode<JSArray> tmp258;
    compiler::TNode<JSArray> tmp259;
    compiler::TNode<JSArray> tmp260;
    compiler::TNode<Map> tmp261;
    compiler::TNode<BoolT> tmp262;
    compiler::TNode<BoolT> tmp263;
    compiler::TNode<BoolT> tmp264;
    compiler::TNode<Smi> tmp265;
    compiler::TNode<Context> tmp266;
    compiler::TNode<Smi> tmp267;
    compiler::TNode<Smi> tmp268;
    compiler::TNode<Context> tmp269;
    compiler::TNode<Smi> tmp270;
    ca_.Bind(&block29, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1959);
    compiler::TNode<Object> tmp271;
    USE(tmp271);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp271 = BaseBuiltinsFromDSLAssembler(state_).LoadElementNoHole16FixedDoubleArray(compiler::TNode<Context>{tmp269}, compiler::TNode<JSArray>{tmp260}, compiler::TNode<Smi>{tmp270}, &label0);
    ca_.Goto(&block32, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp260, tmp270, tmp271);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block33, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp260, tmp270);
    }
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp272;
    compiler::TNode<JSReceiver> tmp273;
    compiler::TNode<Number> tmp274;
    compiler::TNode<JSReceiver> tmp275;
    compiler::TNode<Object> tmp276;
    compiler::TNode<Smi> tmp277;
    compiler::TNode<Smi> tmp278;
    compiler::TNode<JSArray> tmp279;
    compiler::TNode<JSArray> tmp280;
    compiler::TNode<JSArray> tmp281;
    compiler::TNode<Map> tmp282;
    compiler::TNode<BoolT> tmp283;
    compiler::TNode<BoolT> tmp284;
    compiler::TNode<BoolT> tmp285;
    compiler::TNode<Smi> tmp286;
    compiler::TNode<Context> tmp287;
    compiler::TNode<Smi> tmp288;
    compiler::TNode<Smi> tmp289;
    compiler::TNode<Context> tmp290;
    compiler::TNode<Smi> tmp291;
    compiler::TNode<JSArray> tmp292;
    compiler::TNode<Smi> tmp293;
    ca_.Bind(&block33, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293);
    ca_.Goto(&block27, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp294;
    compiler::TNode<JSReceiver> tmp295;
    compiler::TNode<Number> tmp296;
    compiler::TNode<JSReceiver> tmp297;
    compiler::TNode<Object> tmp298;
    compiler::TNode<Smi> tmp299;
    compiler::TNode<Smi> tmp300;
    compiler::TNode<JSArray> tmp301;
    compiler::TNode<JSArray> tmp302;
    compiler::TNode<JSArray> tmp303;
    compiler::TNode<Map> tmp304;
    compiler::TNode<BoolT> tmp305;
    compiler::TNode<BoolT> tmp306;
    compiler::TNode<BoolT> tmp307;
    compiler::TNode<Smi> tmp308;
    compiler::TNode<Context> tmp309;
    compiler::TNode<Smi> tmp310;
    compiler::TNode<Smi> tmp311;
    compiler::TNode<Context> tmp312;
    compiler::TNode<Smi> tmp313;
    compiler::TNode<JSArray> tmp314;
    compiler::TNode<Smi> tmp315;
    compiler::TNode<Object> tmp316;
    ca_.Bind(&block32, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316);
    ca_.Goto(&block28, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311, tmp312, tmp313, tmp316);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp317;
    compiler::TNode<JSReceiver> tmp318;
    compiler::TNode<Number> tmp319;
    compiler::TNode<JSReceiver> tmp320;
    compiler::TNode<Object> tmp321;
    compiler::TNode<Smi> tmp322;
    compiler::TNode<Smi> tmp323;
    compiler::TNode<JSArray> tmp324;
    compiler::TNode<JSArray> tmp325;
    compiler::TNode<JSArray> tmp326;
    compiler::TNode<Map> tmp327;
    compiler::TNode<BoolT> tmp328;
    compiler::TNode<BoolT> tmp329;
    compiler::TNode<BoolT> tmp330;
    compiler::TNode<Smi> tmp331;
    compiler::TNode<Context> tmp332;
    compiler::TNode<Smi> tmp333;
    compiler::TNode<Smi> tmp334;
    compiler::TNode<Context> tmp335;
    compiler::TNode<Smi> tmp336;
    ca_.Bind(&block30, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1962);
    compiler::TNode<Object> tmp337;
    USE(tmp337);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp337 = BaseBuiltinsFromDSLAssembler(state_).LoadElementNoHole10FixedArray(compiler::TNode<Context>{tmp335}, compiler::TNode<JSArray>{tmp326}, compiler::TNode<Smi>{tmp336}, &label0);
    ca_.Goto(&block34, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp333, tmp334, tmp335, tmp336, tmp326, tmp336, tmp337);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block35, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp333, tmp334, tmp335, tmp336, tmp326, tmp336);
    }
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp338;
    compiler::TNode<JSReceiver> tmp339;
    compiler::TNode<Number> tmp340;
    compiler::TNode<JSReceiver> tmp341;
    compiler::TNode<Object> tmp342;
    compiler::TNode<Smi> tmp343;
    compiler::TNode<Smi> tmp344;
    compiler::TNode<JSArray> tmp345;
    compiler::TNode<JSArray> tmp346;
    compiler::TNode<JSArray> tmp347;
    compiler::TNode<Map> tmp348;
    compiler::TNode<BoolT> tmp349;
    compiler::TNode<BoolT> tmp350;
    compiler::TNode<BoolT> tmp351;
    compiler::TNode<Smi> tmp352;
    compiler::TNode<Context> tmp353;
    compiler::TNode<Smi> tmp354;
    compiler::TNode<Smi> tmp355;
    compiler::TNode<Context> tmp356;
    compiler::TNode<Smi> tmp357;
    compiler::TNode<JSArray> tmp358;
    compiler::TNode<Smi> tmp359;
    ca_.Bind(&block35, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358, &tmp359);
    ca_.Goto(&block27, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp360;
    compiler::TNode<JSReceiver> tmp361;
    compiler::TNode<Number> tmp362;
    compiler::TNode<JSReceiver> tmp363;
    compiler::TNode<Object> tmp364;
    compiler::TNode<Smi> tmp365;
    compiler::TNode<Smi> tmp366;
    compiler::TNode<JSArray> tmp367;
    compiler::TNode<JSArray> tmp368;
    compiler::TNode<JSArray> tmp369;
    compiler::TNode<Map> tmp370;
    compiler::TNode<BoolT> tmp371;
    compiler::TNode<BoolT> tmp372;
    compiler::TNode<BoolT> tmp373;
    compiler::TNode<Smi> tmp374;
    compiler::TNode<Context> tmp375;
    compiler::TNode<Smi> tmp376;
    compiler::TNode<Smi> tmp377;
    compiler::TNode<Context> tmp378;
    compiler::TNode<Smi> tmp379;
    compiler::TNode<JSArray> tmp380;
    compiler::TNode<Smi> tmp381;
    compiler::TNode<Object> tmp382;
    ca_.Bind(&block34, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382);
    ca_.Goto(&block28, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp382);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp383;
    compiler::TNode<JSReceiver> tmp384;
    compiler::TNode<Number> tmp385;
    compiler::TNode<JSReceiver> tmp386;
    compiler::TNode<Object> tmp387;
    compiler::TNode<Smi> tmp388;
    compiler::TNode<Smi> tmp389;
    compiler::TNode<JSArray> tmp390;
    compiler::TNode<JSArray> tmp391;
    compiler::TNode<JSArray> tmp392;
    compiler::TNode<Map> tmp393;
    compiler::TNode<BoolT> tmp394;
    compiler::TNode<BoolT> tmp395;
    compiler::TNode<BoolT> tmp396;
    compiler::TNode<Smi> tmp397;
    compiler::TNode<Context> tmp398;
    compiler::TNode<Smi> tmp399;
    compiler::TNode<Smi> tmp400;
    compiler::TNode<Context> tmp401;
    compiler::TNode<Smi> tmp402;
    compiler::TNode<Object> tmp403;
    ca_.Bind(&block28, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402, &tmp403);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1969);
    ca_.Goto(&block25, tmp383, tmp384, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp396, tmp397, tmp398, tmp399, tmp403);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp404;
    compiler::TNode<JSReceiver> tmp405;
    compiler::TNode<Number> tmp406;
    compiler::TNode<JSReceiver> tmp407;
    compiler::TNode<Object> tmp408;
    compiler::TNode<Smi> tmp409;
    compiler::TNode<Smi> tmp410;
    compiler::TNode<JSArray> tmp411;
    compiler::TNode<JSArray> tmp412;
    compiler::TNode<JSArray> tmp413;
    compiler::TNode<Map> tmp414;
    compiler::TNode<BoolT> tmp415;
    compiler::TNode<BoolT> tmp416;
    compiler::TNode<BoolT> tmp417;
    compiler::TNode<Smi> tmp418;
    compiler::TNode<Context> tmp419;
    compiler::TNode<Smi> tmp420;
    ca_.Bind(&block27, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1972);
    compiler::TNode<Oddball> tmp421;
    USE(tmp421);
    tmp421 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block25, tmp404, tmp405, tmp406, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420, tmp421);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp422;
    compiler::TNode<JSReceiver> tmp423;
    compiler::TNode<Number> tmp424;
    compiler::TNode<JSReceiver> tmp425;
    compiler::TNode<Object> tmp426;
    compiler::TNode<Smi> tmp427;
    compiler::TNode<Smi> tmp428;
    compiler::TNode<JSArray> tmp429;
    compiler::TNode<JSArray> tmp430;
    compiler::TNode<JSArray> tmp431;
    compiler::TNode<Map> tmp432;
    compiler::TNode<BoolT> tmp433;
    compiler::TNode<BoolT> tmp434;
    compiler::TNode<BoolT> tmp435;
    compiler::TNode<Smi> tmp436;
    compiler::TNode<Context> tmp437;
    compiler::TNode<Smi> tmp438;
    compiler::TNode<Object> tmp439;
    ca_.Bind(&block25, &tmp422, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 109);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 111);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1941);
    ca_.Goto(&block36, tmp422, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp439, tmp422, tmp425, tmp426, tmp439, tmp427, tmp431);
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp440;
    compiler::TNode<JSReceiver> tmp441;
    compiler::TNode<Number> tmp442;
    compiler::TNode<JSReceiver> tmp443;
    compiler::TNode<Object> tmp444;
    compiler::TNode<Smi> tmp445;
    compiler::TNode<Smi> tmp446;
    compiler::TNode<JSArray> tmp447;
    compiler::TNode<JSArray> tmp448;
    compiler::TNode<JSArray> tmp449;
    compiler::TNode<Map> tmp450;
    compiler::TNode<BoolT> tmp451;
    compiler::TNode<BoolT> tmp452;
    compiler::TNode<BoolT> tmp453;
    compiler::TNode<Object> tmp454;
    compiler::TNode<Context> tmp455;
    compiler::TNode<JSReceiver> tmp456;
    compiler::TNode<Object> tmp457;
    compiler::TNode<Object> tmp458;
    compiler::TNode<Smi> tmp459;
    compiler::TNode<JSArray> tmp460;
    ca_.Bind(&block36, &tmp440, &tmp441, &tmp442, &tmp443, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 111);
    compiler::TNode<Object> tmp461;
    USE(tmp461);
    tmp461 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp455}, compiler::TNode<JSReceiver>{tmp456}, compiler::TNode<Object>{tmp457}, compiler::TNode<Object>{tmp458}, compiler::TNode<Object>{tmp459}, compiler::TNode<Object>{tmp460}));
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 110);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 112);
    compiler::TNode<BoolT> tmp462;
    USE(tmp462);
    tmp462 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).ToBoolean(compiler::TNode<Object>{tmp461}));
    ca_.Branch(tmp462, &block37, &block38, tmp440, tmp441, tmp442, tmp443, tmp444, tmp445, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452, tmp453, tmp454, tmp461);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp463;
    compiler::TNode<JSReceiver> tmp464;
    compiler::TNode<Number> tmp465;
    compiler::TNode<JSReceiver> tmp466;
    compiler::TNode<Object> tmp467;
    compiler::TNode<Smi> tmp468;
    compiler::TNode<Smi> tmp469;
    compiler::TNode<JSArray> tmp470;
    compiler::TNode<JSArray> tmp471;
    compiler::TNode<JSArray> tmp472;
    compiler::TNode<Map> tmp473;
    compiler::TNode<BoolT> tmp474;
    compiler::TNode<BoolT> tmp475;
    compiler::TNode<BoolT> tmp476;
    compiler::TNode<Object> tmp477;
    compiler::TNode<Object> tmp478;
    ca_.Bind(&block37, &tmp463, &tmp464, &tmp465, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 113);
    ca_.Goto(&block2, tmp463, tmp464, tmp465, tmp466, tmp467, tmp468);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp479;
    compiler::TNode<JSReceiver> tmp480;
    compiler::TNode<Number> tmp481;
    compiler::TNode<JSReceiver> tmp482;
    compiler::TNode<Object> tmp483;
    compiler::TNode<Smi> tmp484;
    compiler::TNode<Smi> tmp485;
    compiler::TNode<JSArray> tmp486;
    compiler::TNode<JSArray> tmp487;
    compiler::TNode<JSArray> tmp488;
    compiler::TNode<Map> tmp489;
    compiler::TNode<BoolT> tmp490;
    compiler::TNode<BoolT> tmp491;
    compiler::TNode<BoolT> tmp492;
    compiler::TNode<Object> tmp493;
    compiler::TNode<Object> tmp494;
    ca_.Bind(&block38, &tmp479, &tmp480, &tmp481, &tmp482, &tmp483, &tmp484, &tmp485, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 103);
    ca_.Goto(&block14, tmp479, tmp480, tmp481, tmp482, tmp483, tmp484, tmp485, tmp486, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp495;
    compiler::TNode<JSReceiver> tmp496;
    compiler::TNode<Number> tmp497;
    compiler::TNode<JSReceiver> tmp498;
    compiler::TNode<Object> tmp499;
    compiler::TNode<Smi> tmp500;
    compiler::TNode<Smi> tmp501;
    compiler::TNode<JSArray> tmp502;
    compiler::TNode<JSArray> tmp503;
    compiler::TNode<JSArray> tmp504;
    compiler::TNode<Map> tmp505;
    compiler::TNode<BoolT> tmp506;
    compiler::TNode<BoolT> tmp507;
    compiler::TNode<BoolT> tmp508;
    ca_.Bind(&block14, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501, &tmp502, &tmp503, &tmp504, &tmp505, &tmp506, &tmp507, &tmp508);
    compiler::TNode<Smi> tmp509;
    USE(tmp509);
    tmp509 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp510;
    USE(tmp510);
    tmp510 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp500}, compiler::TNode<Smi>{tmp509}));
    ca_.Goto(&block13, tmp495, tmp496, tmp497, tmp498, tmp499, tmp510, tmp501, tmp502, tmp503, tmp504, tmp505, tmp506, tmp507, tmp508);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp511;
    compiler::TNode<JSReceiver> tmp512;
    compiler::TNode<Number> tmp513;
    compiler::TNode<JSReceiver> tmp514;
    compiler::TNode<Object> tmp515;
    compiler::TNode<Smi> tmp516;
    compiler::TNode<Smi> tmp517;
    compiler::TNode<JSArray> tmp518;
    compiler::TNode<JSArray> tmp519;
    compiler::TNode<JSArray> tmp520;
    compiler::TNode<Map> tmp521;
    compiler::TNode<BoolT> tmp522;
    compiler::TNode<BoolT> tmp523;
    compiler::TNode<BoolT> tmp524;
    ca_.Bind(&block12, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518, &tmp519, &tmp520, &tmp521, &tmp522, &tmp523, &tmp524);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 116);
    compiler::TNode<Number> tmp525;
    USE(tmp525);
    tmp525 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(-1));
    ca_.Goto(&block2, tmp511, tmp512, tmp513, tmp514, tmp515, tmp525);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp526;
    compiler::TNode<JSReceiver> tmp527;
    compiler::TNode<Number> tmp528;
    compiler::TNode<JSReceiver> tmp529;
    compiler::TNode<Object> tmp530;
    compiler::TNode<Number> tmp531;
    ca_.Bind(&block2, &tmp526, &tmp527, &tmp528, &tmp529, &tmp530, &tmp531);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 94);
    ca_.Goto(&block39, tmp526, tmp527, tmp528, tmp529, tmp530, tmp531);
  }

  if (block1.is_used()) {
    compiler::TNode<Smi> tmp532;
    ca_.Bind(&block1, &tmp532);
    *label_Bailout_parameter_0 = tmp532;
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp533;
    compiler::TNode<JSReceiver> tmp534;
    compiler::TNode<Number> tmp535;
    compiler::TNode<JSReceiver> tmp536;
    compiler::TNode<Object> tmp537;
    compiler::TNode<Number> tmp538;
    ca_.Bind(&block39, &tmp533, &tmp534, &tmp535, &tmp536, &tmp537, &tmp538);
  return compiler::TNode<Number>{tmp538};
}

TF_BUILTIN(ArrayPrototypeFindIndex, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, JSReceiver> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, JSReceiver, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, JSReceiver, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, JSReceiver, Object, JSReceiver, Number, JSReceiver, Object, Smi> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, JSReceiver, Object, JSReceiver, Number, JSReceiver, Object, Number> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, JSReceiver, Object, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 124);
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{tmp1}));
    ca_.Branch(tmp2, &block5, &block6, tmp0, tmp1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block5, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 125);
    ca_.Goto(&block2, tmp3, tmp4);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block6, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 129);
    compiler::TNode<JSReceiver> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp5}, compiler::TNode<Object>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 132);
    compiler::TNode<Number> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp5}, compiler::TNode<Object>{tmp7}));
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 135);
    compiler::TNode<IntPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(arguments));
    compiler::TNode<IntPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp9}, compiler::TNode<IntPtrT>{tmp10}));
    ca_.Branch(tmp11, &block7, &block8, tmp5, tmp6, tmp7, tmp8);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<JSReceiver> tmp14;
    compiler::TNode<Number> tmp15;
    ca_.Bind(&block7, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 136);
    ca_.Goto(&block4, tmp12, tmp13);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<JSReceiver> tmp18;
    compiler::TNode<Number> tmp19;
    ca_.Bind(&block8, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 139);
    compiler::TNode<IntPtrT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp20}));
    compiler::TNode<JSReceiver> tmp22;
    USE(tmp22);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp22 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp16}, compiler::TNode<Object>{tmp21}, &label0);
    ca_.Goto(&block9, tmp16, tmp17, tmp18, tmp19, tmp21, tmp22);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp16, tmp17, tmp18, tmp19, tmp21);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Object> tmp27;
    ca_.Bind(&block10, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block4, tmp23, tmp24);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<JSReceiver> tmp30;
    compiler::TNode<Number> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    ca_.Bind(&block9, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 142);
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(arguments));
    compiler::TNode<IntPtrT> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp34}, compiler::TNode<IntPtrT>{tmp35}));
    ca_.Branch(tmp36, &block11, &block12, tmp28, tmp29, tmp30, tmp31, tmp33);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    ca_.Bind(&block11, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    compiler::TNode<IntPtrT> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp42}));
    ca_.Goto(&block14, tmp37, tmp38, tmp39, tmp40, tmp41, tmp43);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<JSReceiver> tmp46;
    compiler::TNode<Number> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    ca_.Bind(&block12, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
    compiler::TNode<Oddball> tmp49;
    USE(tmp49);
    tmp49 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block13, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<JSReceiver> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block14, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block13, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    compiler::TNode<Number> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<Object> tmp61;
    ca_.Bind(&block13, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 146);
    compiler::TNode<Number> tmp62;
    USE(tmp62);
    compiler::TypedCodeAssemblerVariable<Smi> result_0_0(&ca_);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp62 = ArrayFindindexBuiltinsFromDSLAssembler(state_).FastArrayFindIndex(compiler::TNode<Context>{tmp56}, compiler::TNode<JSReceiver>{tmp58}, compiler::TNode<Number>{tmp59}, compiler::TNode<JSReceiver>{tmp60}, compiler::TNode<Object>{tmp61}, &label0, &result_0_0);
    ca_.Goto(&block17, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp58, tmp59, tmp60, tmp61, tmp62);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block18, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp58, tmp59, tmp60, tmp61, result_0_0.value());
    }
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<JSReceiver> tmp65;
    compiler::TNode<Number> tmp66;
    compiler::TNode<JSReceiver> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<JSReceiver> tmp69;
    compiler::TNode<Number> tmp70;
    compiler::TNode<JSReceiver> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<Smi> tmp73;
    ca_.Bind(&block18, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    ca_.Goto(&block16, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp73);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<JSReceiver> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<JSReceiver> tmp80;
    compiler::TNode<Number> tmp81;
    compiler::TNode<JSReceiver> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<Number> tmp84;
    ca_.Bind(&block17, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84);
    arguments->PopAndReturn(tmp84);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<JSReceiver> tmp87;
    compiler::TNode<Number> tmp88;
    compiler::TNode<JSReceiver> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<Smi> tmp91;
    ca_.Bind(&block16, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 151);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 150);
    compiler::TNode<Number> tmp92;
    tmp92 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kArrayFindIndexLoopContinuation, tmp85, tmp87, tmp89, tmp90, tmp87, tmp91, tmp88));
    USE(tmp92);
    arguments->PopAndReturn(tmp92);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp93;
    compiler::TNode<Object> tmp94;
    ca_.Bind(&block4, &tmp93, &tmp94);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 155);
    compiler::TNode<IntPtrT> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp95}));
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp93}, MessageTemplate::kCalledNonCallable, compiler::TNode<Object>{tmp96});
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<Object> tmp98;
    ca_.Bind(&block2, &tmp97, &tmp98);
    ca_.SetSourcePosition("../../src/builtins/array-findindex.tq", 158);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp97}, MessageTemplate::kCalledOnNullOrUndefined, "Array.prototype.findIndex");
  }
}

}  // namespace internal
}  // namespace v8

