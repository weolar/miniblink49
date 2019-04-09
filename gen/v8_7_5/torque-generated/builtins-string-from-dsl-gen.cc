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

compiler::TNode<Oddball> StringBuiltinsFromDSLAssembler::TryFastStringCompareSequence(compiler::TNode<String> p_string, compiler::TNode<String> p_searchStr, compiler::TNode<Number> p_start, compiler::TNode<Smi> p_searchLength, compiler::CodeAssemblerLabel* label_Slow) {
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String, String> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String, Number, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String, Smi, IntPtrT, IntPtrT, IntPtrT> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String, Smi, IntPtrT, IntPtrT, IntPtrT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String, Smi, IntPtrT, IntPtrT, IntPtrT> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String, Smi, IntPtrT, IntPtrT, IntPtrT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, String, String, Smi, IntPtrT, IntPtrT, IntPtrT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, Oddball> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String, String, Number, Smi, Oddball> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_string, p_searchStr, p_start, p_searchLength);

  if (block0.is_used()) {
    compiler::TNode<String> tmp0;
    compiler::TNode<String> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 9);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).Cast14ATDirectString(compiler::TNode<HeapObject>{tmp0}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp0, tmp4);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp0);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<String> tmp5;
    compiler::TNode<String> tmp6;
    compiler::TNode<Number> tmp7;
    compiler::TNode<Smi> tmp8;
    compiler::TNode<String> tmp9;
    ca_.Bind(&block4, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block1);
  }

  if (block3.is_used()) {
    compiler::TNode<String> tmp10;
    compiler::TNode<String> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<String> tmp14;
    compiler::TNode<String> tmp15;
    ca_.Bind(&block3, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 10);
    compiler::TNode<String> tmp16;
    USE(tmp16);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp16 = BaseBuiltinsFromDSLAssembler(state_).Cast14ATDirectString(compiler::TNode<HeapObject>{tmp11}, &label0);
    ca_.Goto(&block5, tmp10, tmp11, tmp12, tmp13, tmp15, tmp11, tmp16);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp10, tmp11, tmp12, tmp13, tmp15, tmp11);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<String> tmp17;
    compiler::TNode<String> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<String> tmp21;
    compiler::TNode<String> tmp22;
    ca_.Bind(&block6, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<String> tmp23;
    compiler::TNode<String> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<String> tmp27;
    compiler::TNode<String> tmp28;
    compiler::TNode<String> tmp29;
    ca_.Bind(&block5, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 11);
    compiler::TNode<Smi> tmp30;
    USE(tmp30);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp30 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp25}, &label0);
    ca_.Goto(&block7, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29, tmp25, tmp30);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29, tmp25);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<String> tmp31;
    compiler::TNode<String> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<Smi> tmp34;
    compiler::TNode<String> tmp35;
    compiler::TNode<String> tmp36;
    compiler::TNode<Number> tmp37;
    ca_.Bind(&block8, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    ca_.Goto(&block1);
  }

  if (block7.is_used()) {
    compiler::TNode<String> tmp38;
    compiler::TNode<String> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<String> tmp42;
    compiler::TNode<String> tmp43;
    compiler::TNode<Number> tmp44;
    compiler::TNode<Smi> tmp45;
    ca_.Bind(&block7, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 13);
    compiler::TNode<IntPtrT> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 14);
    compiler::TNode<IntPtrT> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp45}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 15);
    compiler::TNode<IntPtrT> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp41}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 17);
    ca_.Goto(&block11, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp45, tmp46, tmp47, tmp48);
  }

  if (block11.is_used()) {
    compiler::TNode<String> tmp49;
    compiler::TNode<String> tmp50;
    compiler::TNode<Number> tmp51;
    compiler::TNode<Smi> tmp52;
    compiler::TNode<String> tmp53;
    compiler::TNode<String> tmp54;
    compiler::TNode<Smi> tmp55;
    compiler::TNode<IntPtrT> tmp56;
    compiler::TNode<IntPtrT> tmp57;
    compiler::TNode<IntPtrT> tmp58;
    ca_.Bind(&block11, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    compiler::TNode<BoolT> tmp59;
    USE(tmp59);
    tmp59 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp56}, compiler::TNode<IntPtrT>{tmp58}));
    ca_.Branch(tmp59, &block9, &block10, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58);
  }

  if (block9.is_used()) {
    compiler::TNode<String> tmp60;
    compiler::TNode<String> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<String> tmp64;
    compiler::TNode<String> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<IntPtrT> tmp67;
    compiler::TNode<IntPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    ca_.Bind(&block9, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 18);
    compiler::TNode<Int32T> tmp70;
    USE(tmp70);
    tmp70 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).StringCharCodeAt(compiler::TNode<String>{tmp65}, compiler::TNode<IntPtrT>{tmp67}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 19);
    compiler::TNode<Int32T> tmp71;
    USE(tmp71);
    tmp71 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).StringCharCodeAt(compiler::TNode<String>{tmp64}, compiler::TNode<IntPtrT>{tmp68}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 18);
    compiler::TNode<BoolT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32NotEqual(compiler::TNode<Int32T>{tmp70}, compiler::TNode<Int32T>{tmp71}));
    ca_.Branch(tmp72, &block12, &block13, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69);
  }

  if (block12.is_used()) {
    compiler::TNode<String> tmp73;
    compiler::TNode<String> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Smi> tmp76;
    compiler::TNode<String> tmp77;
    compiler::TNode<String> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<IntPtrT> tmp80;
    compiler::TNode<IntPtrT> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    ca_.Bind(&block12, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 20);
    compiler::TNode<Oddball> tmp83;
    USE(tmp83);
    tmp83 = BaseBuiltinsFromDSLAssembler(state_).False();
    ca_.Goto(&block2, tmp73, tmp74, tmp75, tmp76, tmp83);
  }

  if (block13.is_used()) {
    compiler::TNode<String> tmp84;
    compiler::TNode<String> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<String> tmp88;
    compiler::TNode<String> tmp89;
    compiler::TNode<Smi> tmp90;
    compiler::TNode<IntPtrT> tmp91;
    compiler::TNode<IntPtrT> tmp92;
    compiler::TNode<IntPtrT> tmp93;
    ca_.Bind(&block13, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 23);
    compiler::TNode<IntPtrT> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp91}, compiler::TNode<IntPtrT>{tmp94}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 24);
    compiler::TNode<IntPtrT> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp97;
    USE(tmp97);
    tmp97 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp92}, compiler::TNode<IntPtrT>{tmp96}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 17);
    ca_.Goto(&block11, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp95, tmp97, tmp93);
  }

  if (block10.is_used()) {
    compiler::TNode<String> tmp98;
    compiler::TNode<String> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Smi> tmp101;
    compiler::TNode<String> tmp102;
    compiler::TNode<String> tmp103;
    compiler::TNode<Smi> tmp104;
    compiler::TNode<IntPtrT> tmp105;
    compiler::TNode<IntPtrT> tmp106;
    compiler::TNode<IntPtrT> tmp107;
    ca_.Bind(&block10, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 26);
    compiler::TNode<Oddball> tmp108;
    USE(tmp108);
    tmp108 = BaseBuiltinsFromDSLAssembler(state_).True();
    ca_.Goto(&block2, tmp98, tmp99, tmp100, tmp101, tmp108);
  }

  if (block2.is_used()) {
    compiler::TNode<String> tmp109;
    compiler::TNode<String> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<Oddball> tmp113;
    ca_.Bind(&block2, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 6);
    ca_.Goto(&block14, tmp109, tmp110, tmp111, tmp112, tmp113);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Slow);
  }

    compiler::TNode<String> tmp114;
    compiler::TNode<String> tmp115;
    compiler::TNode<Number> tmp116;
    compiler::TNode<Smi> tmp117;
    compiler::TNode<Oddball> tmp118;
    ca_.Bind(&block14, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
  return compiler::TNode<Oddball>{tmp118};
}

TF_BUILTIN(StringPrototypeEndsWith, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, String, String, Number, Smi> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, String, String, Number, Smi, Oddball> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 32);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 33);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 36);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(StringBuiltinsFromDSLAssembler(state_).RequireObjectCoercible(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 39);
    compiler::TNode<String> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 43);
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(RegExpBuiltinsAssembler(state_).IsRegExp(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp3}));
    ca_.Branch(tmp8, &block1, &block2, tmp0, tmp1, tmp3, tmp5, tmp6, tmp7);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<String> tmp14;
    ca_.Bind(&block1, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 44);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp9}, MessageTemplate::kFirstArgumentNotRegExp, "String.prototype.endsWith");
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<String> tmp20;
    ca_.Bind(&block2, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 48);
    compiler::TNode<String> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp15}, compiler::TNode<Object>{tmp17}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 51);
    compiler::TNode<Smi> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp20}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 55);
    compiler::TNode<Oddball> tmp23;
    USE(tmp23);
    tmp23 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp18}, compiler::TNode<HeapObject>{tmp23}));
    ca_.Branch(tmp24, &block3, &block4, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<String> tmp30;
    compiler::TNode<String> tmp31;
    compiler::TNode<Number> tmp32;
    ca_.Bind(&block3, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 56);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 55);
    ca_.Goto(&block6, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp32);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<String> tmp38;
    compiler::TNode<String> tmp39;
    compiler::TNode<Number> tmp40;
    ca_.Bind(&block4, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 57);
    compiler::TNode<Number> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp33}, compiler::TNode<Object>{tmp36}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 55);
    ca_.Goto(&block5, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<String> tmp47;
    compiler::TNode<String> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Number> tmp50;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.Goto(&block5, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<String> tmp56;
    compiler::TNode<String> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<Number> tmp59;
    ca_.Bind(&block5, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 60);
    compiler::TNode<Number> tmp60;
    USE(tmp60);
    tmp60 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp61;
    USE(tmp61);
    tmp61 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberMax(compiler::TNode<Number>{tmp59}, compiler::TNode<Number>{tmp60}));
    compiler::TNode<Number> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberMin(compiler::TNode<Number>{tmp61}, compiler::TNode<Number>{tmp58}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 63);
    compiler::TNode<Smi> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp57}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 66);
    compiler::TNode<Number> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp62}, compiler::TNode<Number>{tmp63}));
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 69);
    compiler::TNode<Number> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp64}, compiler::TNode<Number>{tmp65}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp62, tmp63, tmp64, tmp64);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block10, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp62, tmp63, tmp64, tmp64);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<String> tmp71;
    compiler::TNode<String> tmp72;
    compiler::TNode<Number> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Smi> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Number> tmp78;
    ca_.Bind(&block9, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78);
    ca_.Goto(&block7, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<String> tmp84;
    compiler::TNode<String> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<Number> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Number> tmp91;
    ca_.Bind(&block10, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91);
    ca_.Goto(&block8, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<String> tmp97;
    compiler::TNode<String> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<Number> tmp103;
    ca_.Bind(&block7, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103);
    compiler::TNode<Oddball> tmp104;
    USE(tmp104);
    tmp104 = BaseBuiltinsFromDSLAssembler(state_).False();
    arguments->PopAndReturn(tmp104);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<Object> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<Object> tmp109;
    compiler::TNode<String> tmp110;
    compiler::TNode<String> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Number> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<Smi> tmp115;
    compiler::TNode<Number> tmp116;
    ca_.Bind(&block8, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 78);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 77);
    compiler::TNode<Oddball> tmp117;
    USE(tmp117);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp117 = StringBuiltinsFromDSLAssembler(state_).TryFastStringCompareSequence(compiler::TNode<String>{tmp110}, compiler::TNode<String>{tmp111}, compiler::TNode<Number>{tmp116}, compiler::TNode<Smi>{tmp115}, &label0);
    ca_.Goto(&block13, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp110, tmp111, tmp116, tmp115, tmp117);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block14, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp110, tmp111, tmp116, tmp115);
    }
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<String> tmp123;
    compiler::TNode<String> tmp124;
    compiler::TNode<Number> tmp125;
    compiler::TNode<Number> tmp126;
    compiler::TNode<Number> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Number> tmp129;
    compiler::TNode<String> tmp130;
    compiler::TNode<String> tmp131;
    compiler::TNode<Number> tmp132;
    compiler::TNode<Smi> tmp133;
    ca_.Bind(&block14, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133);
    ca_.Goto(&block12, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<String> tmp139;
    compiler::TNode<String> tmp140;
    compiler::TNode<Number> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<Number> tmp143;
    compiler::TNode<Smi> tmp144;
    compiler::TNode<Number> tmp145;
    compiler::TNode<String> tmp146;
    compiler::TNode<String> tmp147;
    compiler::TNode<Number> tmp148;
    compiler::TNode<Smi> tmp149;
    compiler::TNode<Oddball> tmp150;
    ca_.Bind(&block13, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150);
    arguments->PopAndReturn(tmp150);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<String> tmp156;
    compiler::TNode<String> tmp157;
    compiler::TNode<Number> tmp158;
    compiler::TNode<Number> tmp159;
    compiler::TNode<Number> tmp160;
    compiler::TNode<Smi> tmp161;
    compiler::TNode<Number> tmp162;
    ca_.Bind(&block12, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162);
    ca_.SetSourcePosition("../../src/builtins/string-endswith.tq", 82);
    compiler::TNode<Oddball> tmp163;
    tmp163 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kStringCompareSequence, tmp151, tmp156, tmp157, tmp162));
    USE(tmp163);
    arguments->PopAndReturn(tmp163);
  }
}

compiler::TNode<Object> StringBuiltinsFromDSLAssembler::RequireObjectCoercible(compiler::TNode<Context> p_context, compiler::TNode<Object> p_argument) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_argument);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 14);
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{tmp1}));
    ca_.Branch(tmp2, &block2, &block3, tmp0, tmp1);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 15);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp3}, MessageTemplate::kCalledOnNullOrUndefined, "String.prototype.startsWith");
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block3, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 17);
    ca_.Goto(&block1, tmp5, tmp6, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 12);
    ca_.Goto(&block4, tmp7, tmp8, tmp9);
  }

    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12);
  return compiler::TNode<Object>{tmp12};
}

TF_BUILTIN(StringPrototypeStartsWith, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, String, String, Number, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi, String, String, Number, Smi, Oddball> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, String, String, Number, Number, Number, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 23);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 24);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 27);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(StringBuiltinsFromDSLAssembler(state_).RequireObjectCoercible(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 30);
    compiler::TNode<String> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp6}));
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 34);
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(RegExpBuiltinsAssembler(state_).IsRegExp(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp3}));
    ca_.Branch(tmp8, &block1, &block2, tmp0, tmp1, tmp3, tmp5, tmp6, tmp7);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<String> tmp14;
    ca_.Bind(&block1, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 35);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp9}, MessageTemplate::kFirstArgumentNotRegExp, "String.prototype.startsWith");
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<String> tmp20;
    ca_.Bind(&block2, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 39);
    compiler::TNode<String> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp15}, compiler::TNode<Object>{tmp17}));
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 42);
    compiler::TNode<Number> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp15}, compiler::TNode<Object>{tmp18}));
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 46);
    compiler::TNode<Smi> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp20}));
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 49);
    compiler::TNode<Number> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberMax(compiler::TNode<Number>{tmp22}, compiler::TNode<Number>{tmp24}));
    compiler::TNode<Number> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberMin(compiler::TNode<Number>{tmp25}, compiler::TNode<Number>{tmp23}));
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 52);
    compiler::TNode<Smi> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp21}));
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 55);
    compiler::TNode<Number> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp27}, compiler::TNode<Number>{tmp26}));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp28}, compiler::TNode<Number>{tmp23}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp26, tmp27, tmp28, tmp23);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block6, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp26, tmp27, tmp28, tmp23);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<String> tmp34;
    compiler::TNode<String> tmp35;
    compiler::TNode<Number> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Smi> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Number> tmp41;
    ca_.Bind(&block5, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.Goto(&block3, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<String> tmp47;
    compiler::TNode<String> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<Number> tmp51;
    compiler::TNode<Smi> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Number> tmp54;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54);
    ca_.Goto(&block4, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<String> tmp60;
    compiler::TNode<String> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Number> tmp64;
    compiler::TNode<Smi> tmp65;
    ca_.Bind(&block3, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65);
    compiler::TNode<Oddball> tmp66;
    USE(tmp66);
    tmp66 = BaseBuiltinsFromDSLAssembler(state_).False();
    arguments->PopAndReturn(tmp66);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<String> tmp72;
    compiler::TNode<String> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Smi> tmp77;
    ca_.Bind(&block4, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 64);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 63);
    compiler::TNode<Oddball> tmp78;
    USE(tmp78);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp78 = StringBuiltinsFromDSLAssembler(state_).TryFastStringCompareSequence(compiler::TNode<String>{tmp72}, compiler::TNode<String>{tmp73}, compiler::TNode<Number>{tmp76}, compiler::TNode<Smi>{tmp77}, &label0);
    ca_.Goto(&block9, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp72, tmp73, tmp76, tmp77, tmp78);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp72, tmp73, tmp76, tmp77);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<String> tmp84;
    compiler::TNode<String> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<Number> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<String> tmp90;
    compiler::TNode<String> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<Smi> tmp93;
    ca_.Bind(&block10, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93);
    ca_.Goto(&block8, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<String> tmp99;
    compiler::TNode<String> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Number> tmp102;
    compiler::TNode<Number> tmp103;
    compiler::TNode<Smi> tmp104;
    compiler::TNode<String> tmp105;
    compiler::TNode<String> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<Smi> tmp108;
    compiler::TNode<Oddball> tmp109;
    ca_.Bind(&block9, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109);
    arguments->PopAndReturn(tmp109);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<String> tmp115;
    compiler::TNode<String> tmp116;
    compiler::TNode<Number> tmp117;
    compiler::TNode<Number> tmp118;
    compiler::TNode<Number> tmp119;
    compiler::TNode<Smi> tmp120;
    ca_.Bind(&block8, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120);
    ca_.SetSourcePosition("../../src/builtins/string-startswith.tq", 68);
    compiler::TNode<Oddball> tmp121;
    tmp121 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kStringCompareSequence, tmp110, tmp115, tmp116, tmp119));
    USE(tmp121);
    arguments->PopAndReturn(tmp121);
  }
}

}  // namespace internal
}  // namespace v8

