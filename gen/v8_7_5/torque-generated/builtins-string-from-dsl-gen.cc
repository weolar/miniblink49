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
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 9);
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
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 10);
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
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 11);
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
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 13);
    compiler::TNode<IntPtrT> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 14);
    compiler::TNode<IntPtrT> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp45}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 15);
    compiler::TNode<IntPtrT> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp41}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 17);
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
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 18);
    compiler::TNode<Int32T> tmp70;
    USE(tmp70);
    tmp70 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).StringCharCodeAt(compiler::TNode<String>{tmp65}, compiler::TNode<IntPtrT>{tmp67}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 19);
    compiler::TNode<Int32T> tmp71;
    USE(tmp71);
    tmp71 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).StringCharCodeAt(compiler::TNode<String>{tmp64}, compiler::TNode<IntPtrT>{tmp68}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 18);
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
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 20);
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
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 23);
    compiler::TNode<IntPtrT> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp91}, compiler::TNode<IntPtrT>{tmp94}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 24);
    compiler::TNode<IntPtrT> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp97;
    USE(tmp97);
    tmp97 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp92}, compiler::TNode<IntPtrT>{tmp96}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 17);
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
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 26);
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
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 6);
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
  TNode<IntPtrT> arguments_length(ChangeInt32ToIntPtr(argc));
  TNode<RawPtrT> arguments_frame = UncheckedCast<RawPtrT>(LoadFramePointer());
  BaseBuiltinsFromDSLAssembler::Arguments torque_arguments(GetFrameArguments(arguments_frame, arguments_length));
  CodeStubArguments arguments(this, torque_arguments);
  TNode<Object> parameter1 = arguments.GetReceiver();
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, String, String, Number, Smi> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, String, String, Number, Smi, Oddball> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 32);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 33);
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 36);
    compiler::TNode<Object> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Object>(StringBuiltinsFromDSLAssembler(state_).RequireObjectCoercible(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 39);
    compiler::TNode<String> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp9}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 43);
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(RegExpBuiltinsAssembler(state_).IsRegExp(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp6}));
    ca_.Branch(tmp11, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, tmp8, tmp9, tmp10);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<RawPtrT> tmp15;
    compiler::TNode<IntPtrT> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<String> tmp20;
    ca_.Bind(&block1, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 44);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp12}, MessageTemplate::kFirstArgumentNotRegExp, "String.prototype.endsWith");
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<RawPtrT> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<String> tmp29;
    ca_.Bind(&block2, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 48);
    compiler::TNode<String> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp21}, compiler::TNode<Object>{tmp26}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 51);
    compiler::TNode<Smi> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp29}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 55);
    compiler::TNode<Oddball> tmp32;
    USE(tmp32);
    tmp32 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp27}, compiler::TNode<HeapObject>{tmp32}));
    ca_.Branch(tmp33, &block3, &block4, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<String> tmp42;
    compiler::TNode<String> tmp43;
    compiler::TNode<Number> tmp44;
    ca_.Bind(&block3, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 56);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 55);
    ca_.Goto(&block6, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp44);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<RawPtrT> tmp47;
    compiler::TNode<RawPtrT> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<String> tmp53;
    compiler::TNode<String> tmp54;
    compiler::TNode<Number> tmp55;
    ca_.Bind(&block4, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 57);
    compiler::TNode<Number> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp45}, compiler::TNode<Object>{tmp51}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 55);
    ca_.Goto(&block5, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<RawPtrT> tmp60;
    compiler::TNode<IntPtrT> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<String> tmp65;
    compiler::TNode<String> tmp66;
    compiler::TNode<Number> tmp67;
    compiler::TNode<Number> tmp68;
    ca_.Bind(&block6, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68);
    ca_.Goto(&block5, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<RawPtrT> tmp71;
    compiler::TNode<RawPtrT> tmp72;
    compiler::TNode<IntPtrT> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<String> tmp77;
    compiler::TNode<String> tmp78;
    compiler::TNode<Number> tmp79;
    compiler::TNode<Number> tmp80;
    ca_.Bind(&block5, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 60);
    compiler::TNode<Number> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberMax(compiler::TNode<Number>{tmp80}, compiler::TNode<Number>{tmp81}));
    compiler::TNode<Number> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberMin(compiler::TNode<Number>{tmp82}, compiler::TNode<Number>{tmp79}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 63);
    compiler::TNode<Smi> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp78}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 66);
    compiler::TNode<Number> tmp85;
    USE(tmp85);
    tmp85 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp83}, compiler::TNode<Number>{tmp84}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 69);
    compiler::TNode<Number> tmp86;
    USE(tmp86);
    tmp86 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp85}, compiler::TNode<Number>{tmp86}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp83, tmp84, tmp85, tmp85);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block10, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp83, tmp84, tmp85, tmp85);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<RawPtrT> tmp89;
    compiler::TNode<RawPtrT> tmp90;
    compiler::TNode<IntPtrT> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<String> tmp95;
    compiler::TNode<String> tmp96;
    compiler::TNode<Number> tmp97;
    compiler::TNode<Number> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<Smi> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Number> tmp102;
    ca_.Bind(&block9, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    ca_.Goto(&block7, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<RawPtrT> tmp105;
    compiler::TNode<RawPtrT> tmp106;
    compiler::TNode<IntPtrT> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<Object> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<String> tmp111;
    compiler::TNode<String> tmp112;
    compiler::TNode<Number> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<Number> tmp115;
    compiler::TNode<Smi> tmp116;
    compiler::TNode<Number> tmp117;
    compiler::TNode<Number> tmp118;
    ca_.Bind(&block10, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
    ca_.Goto(&block8, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<RawPtrT> tmp121;
    compiler::TNode<RawPtrT> tmp122;
    compiler::TNode<IntPtrT> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<Object> tmp126;
    compiler::TNode<String> tmp127;
    compiler::TNode<String> tmp128;
    compiler::TNode<Number> tmp129;
    compiler::TNode<Number> tmp130;
    compiler::TNode<Number> tmp131;
    compiler::TNode<Smi> tmp132;
    compiler::TNode<Number> tmp133;
    ca_.Bind(&block7, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133);
    compiler::TNode<Oddball> tmp134;
    USE(tmp134);
    tmp134 = BaseBuiltinsFromDSLAssembler(state_).False();
    arguments.PopAndReturn(tmp134);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<RawPtrT> tmp137;
    compiler::TNode<RawPtrT> tmp138;
    compiler::TNode<IntPtrT> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<Object> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<String> tmp143;
    compiler::TNode<String> tmp144;
    compiler::TNode<Number> tmp145;
    compiler::TNode<Number> tmp146;
    compiler::TNode<Number> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<Number> tmp149;
    ca_.Bind(&block8, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 78);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 77);
    compiler::TNode<Oddball> tmp150;
    USE(tmp150);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp150 = StringBuiltinsFromDSLAssembler(state_).TryFastStringCompareSequence(compiler::TNode<String>{tmp143}, compiler::TNode<String>{tmp144}, compiler::TNode<Number>{tmp149}, compiler::TNode<Smi>{tmp148}, &label0);
    ca_.Goto(&block13, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp143, tmp144, tmp149, tmp148, tmp150);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block14, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp143, tmp144, tmp149, tmp148);
    }
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<RawPtrT> tmp153;
    compiler::TNode<RawPtrT> tmp154;
    compiler::TNode<IntPtrT> tmp155;
    compiler::TNode<Object> tmp156;
    compiler::TNode<Object> tmp157;
    compiler::TNode<Object> tmp158;
    compiler::TNode<String> tmp159;
    compiler::TNode<String> tmp160;
    compiler::TNode<Number> tmp161;
    compiler::TNode<Number> tmp162;
    compiler::TNode<Number> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Number> tmp165;
    compiler::TNode<String> tmp166;
    compiler::TNode<String> tmp167;
    compiler::TNode<Number> tmp168;
    compiler::TNode<Smi> tmp169;
    ca_.Bind(&block14, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169);
    ca_.Goto(&block12, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<Object> tmp171;
    compiler::TNode<RawPtrT> tmp172;
    compiler::TNode<RawPtrT> tmp173;
    compiler::TNode<IntPtrT> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<Object> tmp176;
    compiler::TNode<Object> tmp177;
    compiler::TNode<String> tmp178;
    compiler::TNode<String> tmp179;
    compiler::TNode<Number> tmp180;
    compiler::TNode<Number> tmp181;
    compiler::TNode<Number> tmp182;
    compiler::TNode<Smi> tmp183;
    compiler::TNode<Number> tmp184;
    compiler::TNode<String> tmp185;
    compiler::TNode<String> tmp186;
    compiler::TNode<Number> tmp187;
    compiler::TNode<Smi> tmp188;
    compiler::TNode<Oddball> tmp189;
    ca_.Bind(&block13, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189);
    arguments.PopAndReturn(tmp189);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp190;
    compiler::TNode<Object> tmp191;
    compiler::TNode<RawPtrT> tmp192;
    compiler::TNode<RawPtrT> tmp193;
    compiler::TNode<IntPtrT> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<Object> tmp197;
    compiler::TNode<String> tmp198;
    compiler::TNode<String> tmp199;
    compiler::TNode<Number> tmp200;
    compiler::TNode<Number> tmp201;
    compiler::TNode<Number> tmp202;
    compiler::TNode<Smi> tmp203;
    compiler::TNode<Number> tmp204;
    ca_.Bind(&block12, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204);
    ca_.SetSourcePosition("../../v8/src/builtins/string-endswith.tq", 82);
    compiler::TNode<Oddball> tmp205;
    tmp205 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kStringCompareSequence, tmp190, tmp198, tmp199, tmp204));
    USE(tmp205);
    arguments.PopAndReturn(tmp205);
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
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 14);
    compiler::TNode<BoolT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{tmp1}));
    ca_.Branch(tmp2, &block2, &block3, tmp0, tmp1);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 15);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp3}, MessageTemplate::kCalledOnNullOrUndefined, "String.prototype.startsWith");
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block3, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 17);
    ca_.Goto(&block1, tmp5, tmp6, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 12);
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
  TNode<IntPtrT> arguments_length(ChangeInt32ToIntPtr(argc));
  TNode<RawPtrT> arguments_frame = UncheckedCast<RawPtrT>(LoadFramePointer());
  BaseBuiltinsFromDSLAssembler::Arguments torque_arguments(GetFrameArguments(arguments_frame, arguments_length));
  CodeStubArguments arguments(this, torque_arguments);
  TNode<Object> parameter1 = arguments.GetReceiver();
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, String, String, Number, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi, String, String, Number, Smi, Oddball> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, String, String, Number, Number, Number, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 23);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 24);
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 27);
    compiler::TNode<Object> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Object>(StringBuiltinsFromDSLAssembler(state_).RequireObjectCoercible(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 30);
    compiler::TNode<String> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp9}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 34);
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(RegExpBuiltinsAssembler(state_).IsRegExp(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp6}));
    ca_.Branch(tmp11, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, tmp8, tmp9, tmp10);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<RawPtrT> tmp15;
    compiler::TNode<IntPtrT> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<String> tmp20;
    ca_.Bind(&block1, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 35);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp12}, MessageTemplate::kFirstArgumentNotRegExp, "String.prototype.startsWith");
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<RawPtrT> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<String> tmp29;
    ca_.Bind(&block2, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 39);
    compiler::TNode<String> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp21}, compiler::TNode<Object>{tmp26}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 42);
    compiler::TNode<Number> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp21}, compiler::TNode<Object>{tmp27}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 46);
    compiler::TNode<Smi> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp29}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 49);
    compiler::TNode<Number> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberMax(compiler::TNode<Number>{tmp31}, compiler::TNode<Number>{tmp33}));
    compiler::TNode<Number> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberMin(compiler::TNode<Number>{tmp34}, compiler::TNode<Number>{tmp32}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 52);
    compiler::TNode<Smi> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp30}));
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 55);
    compiler::TNode<Number> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp36}, compiler::TNode<Number>{tmp35}));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp37}, compiler::TNode<Number>{tmp32}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp35, tmp36, tmp37, tmp32);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block6, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp35, tmp36, tmp37, tmp32);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<RawPtrT> tmp40;
    compiler::TNode<RawPtrT> tmp41;
    compiler::TNode<IntPtrT> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<String> tmp46;
    compiler::TNode<String> tmp47;
    compiler::TNode<Number> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<Smi> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<Number> tmp53;
    ca_.Bind(&block5, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.Goto(&block3, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<RawPtrT> tmp56;
    compiler::TNode<RawPtrT> tmp57;
    compiler::TNode<IntPtrT> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<String> tmp62;
    compiler::TNode<String> tmp63;
    compiler::TNode<Number> tmp64;
    compiler::TNode<Number> tmp65;
    compiler::TNode<Number> tmp66;
    compiler::TNode<Smi> tmp67;
    compiler::TNode<Number> tmp68;
    compiler::TNode<Number> tmp69;
    ca_.Bind(&block6, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.Goto(&block4, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<RawPtrT> tmp72;
    compiler::TNode<RawPtrT> tmp73;
    compiler::TNode<IntPtrT> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<String> tmp78;
    compiler::TNode<String> tmp79;
    compiler::TNode<Number> tmp80;
    compiler::TNode<Number> tmp81;
    compiler::TNode<Number> tmp82;
    compiler::TNode<Smi> tmp83;
    ca_.Bind(&block3, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83);
    compiler::TNode<Oddball> tmp84;
    USE(tmp84);
    tmp84 = BaseBuiltinsFromDSLAssembler(state_).False();
    arguments.PopAndReturn(tmp84);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<RawPtrT> tmp87;
    compiler::TNode<RawPtrT> tmp88;
    compiler::TNode<IntPtrT> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<String> tmp93;
    compiler::TNode<String> tmp94;
    compiler::TNode<Number> tmp95;
    compiler::TNode<Number> tmp96;
    compiler::TNode<Number> tmp97;
    compiler::TNode<Smi> tmp98;
    ca_.Bind(&block4, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98);
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 64);
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 63);
    compiler::TNode<Oddball> tmp99;
    USE(tmp99);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp99 = StringBuiltinsFromDSLAssembler(state_).TryFastStringCompareSequence(compiler::TNode<String>{tmp93}, compiler::TNode<String>{tmp94}, compiler::TNode<Number>{tmp97}, compiler::TNode<Smi>{tmp98}, &label0);
    ca_.Goto(&block9, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp93, tmp94, tmp97, tmp98, tmp99);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp93, tmp94, tmp97, tmp98);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<RawPtrT> tmp102;
    compiler::TNode<RawPtrT> tmp103;
    compiler::TNode<IntPtrT> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<Object> tmp107;
    compiler::TNode<String> tmp108;
    compiler::TNode<String> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Smi> tmp113;
    compiler::TNode<String> tmp114;
    compiler::TNode<String> tmp115;
    compiler::TNode<Number> tmp116;
    compiler::TNode<Smi> tmp117;
    ca_.Bind(&block10, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117);
    ca_.Goto(&block8, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<RawPtrT> tmp120;
    compiler::TNode<RawPtrT> tmp121;
    compiler::TNode<IntPtrT> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<String> tmp126;
    compiler::TNode<String> tmp127;
    compiler::TNode<Number> tmp128;
    compiler::TNode<Number> tmp129;
    compiler::TNode<Number> tmp130;
    compiler::TNode<Smi> tmp131;
    compiler::TNode<String> tmp132;
    compiler::TNode<String> tmp133;
    compiler::TNode<Number> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<Oddball> tmp136;
    ca_.Bind(&block9, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136);
    arguments.PopAndReturn(tmp136);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<RawPtrT> tmp139;
    compiler::TNode<RawPtrT> tmp140;
    compiler::TNode<IntPtrT> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<String> tmp145;
    compiler::TNode<String> tmp146;
    compiler::TNode<Number> tmp147;
    compiler::TNode<Number> tmp148;
    compiler::TNode<Number> tmp149;
    compiler::TNode<Smi> tmp150;
    ca_.Bind(&block8, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150);
    ca_.SetSourcePosition("../../v8/src/builtins/string-startswith.tq", 68);
    compiler::TNode<Oddball> tmp151;
    tmp151 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kStringCompareSequence, tmp137, tmp145, tmp146, tmp149));
    USE(tmp151);
    arguments.PopAndReturn(tmp151);
  }
}

}  // namespace internal
}  // namespace v8

