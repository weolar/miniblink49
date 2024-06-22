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

compiler::TNode<String> RegexpReplaceBuiltinsFromDSLAssembler::RegExpReplaceFastString(compiler::TNode<Context> p_context, compiler::TNode<JSRegExp> p_regexp, compiler::TNode<String> p_string, compiler::TNode<String> p_replaceString) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT, JSRegExp, String> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT, JSRegExp, String, RegExpMatchInfo> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT, RegExpMatchInfo> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT, RegExpMatchInfo, Smi, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT, RegExpMatchInfo, Smi, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT, RegExpMatchInfo, Smi, Smi> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT, RegExpMatchInfo, Smi, Smi> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT, RegExpMatchInfo, Smi, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT, RegExpMatchInfo, Smi, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String, Smi, BoolT, Smi, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, String, String> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_regexp, p_string, p_replaceString);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSRegExp> tmp1;
    compiler::TNode<String> tmp2;
    compiler::TNode<String> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 36);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 37);
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 38);
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 39);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp3}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 40);
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(RegExpBuiltinsAssembler(state_).FastFlagGetterGlobal(compiler::TNode<JSRegExp>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 42);
    ca_.Branch(tmp8, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSRegExp> tmp10;
    compiler::TNode<String> tmp11;
    compiler::TNode<String> tmp12;
    compiler::TNode<String> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<BoolT> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<BoolT> tmp17;
    ca_.Bind(&block2, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 43);
    compiler::TNode<BoolT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<BoolT>(RegExpBuiltinsAssembler(state_).FastFlagGetterUnicode(compiler::TNode<JSRegExp>{tmp10}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 44);
    compiler::TNode<Smi> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    RegExpBuiltinsAssembler(state_).FastStoreLastIndex(compiler::TNode<JSRegExp>{tmp10}, compiler::TNode<Smi>{tmp19});
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 42);
    ca_.Goto(&block3, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp18, tmp16, tmp17);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<JSRegExp> tmp21;
    compiler::TNode<String> tmp22;
    compiler::TNode<String> tmp23;
    compiler::TNode<String> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<BoolT> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<BoolT> tmp28;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 47);
    ca_.Goto(&block6, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<JSRegExp> tmp30;
    compiler::TNode<String> tmp31;
    compiler::TNode<String> tmp32;
    compiler::TNode<String> tmp33;
    compiler::TNode<Smi> tmp34;
    compiler::TNode<BoolT> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<BoolT> tmp37;
    ca_.Bind(&block6, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    compiler::TNode<BoolT> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Branch(tmp38, &block4, &block5, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<JSRegExp> tmp40;
    compiler::TNode<String> tmp41;
    compiler::TNode<String> tmp42;
    compiler::TNode<String> tmp43;
    compiler::TNode<Smi> tmp44;
    compiler::TNode<BoolT> tmp45;
    compiler::TNode<Smi> tmp46;
    compiler::TNode<BoolT> tmp47;
    ca_.Bind(&block4, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 49);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 48);
    compiler::TNode<RegExpMatchInfo> tmp48;
    USE(tmp48);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp48 = RegExpBuiltinsAssembler(state_).RegExpPrototypeExecBodyWithoutResultFast(compiler::TNode<Context>{tmp39}, compiler::TNode<JSReceiver>{tmp40}, compiler::TNode<String>{tmp41}, &label0);
    ca_.Goto(&block9, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp40, tmp41, tmp48);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp40, tmp41);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<JSRegExp> tmp50;
    compiler::TNode<String> tmp51;
    compiler::TNode<String> tmp52;
    compiler::TNode<String> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<BoolT> tmp55;
    compiler::TNode<Smi> tmp56;
    compiler::TNode<BoolT> tmp57;
    compiler::TNode<JSRegExp> tmp58;
    compiler::TNode<String> tmp59;
    ca_.Bind(&block10, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.Goto(&block8, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp60;
    compiler::TNode<JSRegExp> tmp61;
    compiler::TNode<String> tmp62;
    compiler::TNode<String> tmp63;
    compiler::TNode<String> tmp64;
    compiler::TNode<Smi> tmp65;
    compiler::TNode<BoolT> tmp66;
    compiler::TNode<Smi> tmp67;
    compiler::TNode<BoolT> tmp68;
    compiler::TNode<JSRegExp> tmp69;
    compiler::TNode<String> tmp70;
    compiler::TNode<RegExpMatchInfo> tmp71;
    ca_.Bind(&block9, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    ca_.Goto(&block7, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp71);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<JSRegExp> tmp73;
    compiler::TNode<String> tmp74;
    compiler::TNode<String> tmp75;
    compiler::TNode<String> tmp76;
    compiler::TNode<Smi> tmp77;
    compiler::TNode<BoolT> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<BoolT> tmp80;
    ca_.Bind(&block8, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 49);
    ca_.Goto(&block5, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp81;
    compiler::TNode<JSRegExp> tmp82;
    compiler::TNode<String> tmp83;
    compiler::TNode<String> tmp84;
    compiler::TNode<String> tmp85;
    compiler::TNode<Smi> tmp86;
    compiler::TNode<BoolT> tmp87;
    compiler::TNode<Smi> tmp88;
    compiler::TNode<BoolT> tmp89;
    compiler::TNode<RegExpMatchInfo> tmp90;
    ca_.Bind(&block7, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 48);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 50);
    compiler::TNode<Smi> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_)._method_RegExpMatchInfo_GetStartOfCapture(compiler::TNode<Context>{tmp81}, compiler::TNode<RegExpMatchInfo>{tmp90}, 0));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 51);
    compiler::TNode<Smi> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_)._method_RegExpMatchInfo_GetEndOfCapture(compiler::TNode<Context>{tmp81}, compiler::TNode<RegExpMatchInfo>{tmp90}, 0));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 55);
    compiler::TNode<String> tmp93;
    tmp93 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kSubString, tmp81, tmp83, tmp86, tmp91));
    USE(tmp93);
    compiler::TNode<String> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp81}, compiler::TNode<String>{tmp85}, compiler::TNode<String>{tmp93}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 56);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 58);
    compiler::TNode<Smi> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiNotEqual(compiler::TNode<Smi>{tmp88}, compiler::TNode<Smi>{tmp95}));
    ca_.Branch(tmp96, &block11, &block12, tmp81, tmp82, tmp83, tmp84, tmp94, tmp92, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<JSRegExp> tmp98;
    compiler::TNode<String> tmp99;
    compiler::TNode<String> tmp100;
    compiler::TNode<String> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<BoolT> tmp103;
    compiler::TNode<Smi> tmp104;
    compiler::TNode<BoolT> tmp105;
    compiler::TNode<RegExpMatchInfo> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<Smi> tmp108;
    ca_.Bind(&block11, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108);
    compiler::TNode<String> tmp109;
    USE(tmp109);
    tmp109 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp97}, compiler::TNode<String>{tmp101}, compiler::TNode<String>{tmp100}));
    ca_.Goto(&block12, tmp97, tmp98, tmp99, tmp100, tmp109, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<JSRegExp> tmp111;
    compiler::TNode<String> tmp112;
    compiler::TNode<String> tmp113;
    compiler::TNode<String> tmp114;
    compiler::TNode<Smi> tmp115;
    compiler::TNode<BoolT> tmp116;
    compiler::TNode<Smi> tmp117;
    compiler::TNode<BoolT> tmp118;
    compiler::TNode<RegExpMatchInfo> tmp119;
    compiler::TNode<Smi> tmp120;
    compiler::TNode<Smi> tmp121;
    ca_.Bind(&block12, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 61);
    compiler::TNode<BoolT> tmp122;
    USE(tmp122);
    tmp122 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp118}));
    ca_.Branch(tmp122, &block13, &block14, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp123;
    compiler::TNode<JSRegExp> tmp124;
    compiler::TNode<String> tmp125;
    compiler::TNode<String> tmp126;
    compiler::TNode<String> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<BoolT> tmp129;
    compiler::TNode<Smi> tmp130;
    compiler::TNode<BoolT> tmp131;
    compiler::TNode<RegExpMatchInfo> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<Smi> tmp134;
    ca_.Bind(&block13, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134);
    ca_.Goto(&block5, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp135;
    compiler::TNode<JSRegExp> tmp136;
    compiler::TNode<String> tmp137;
    compiler::TNode<String> tmp138;
    compiler::TNode<String> tmp139;
    compiler::TNode<Smi> tmp140;
    compiler::TNode<BoolT> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<BoolT> tmp143;
    compiler::TNode<RegExpMatchInfo> tmp144;
    compiler::TNode<Smi> tmp145;
    compiler::TNode<Smi> tmp146;
    ca_.Bind(&block14, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 64);
    compiler::TNode<BoolT> tmp147;
    USE(tmp147);
    tmp147 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp146}, compiler::TNode<Smi>{tmp145}));
    ca_.Branch(tmp147, &block15, &block16, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp148;
    compiler::TNode<JSRegExp> tmp149;
    compiler::TNode<String> tmp150;
    compiler::TNode<String> tmp151;
    compiler::TNode<String> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<BoolT> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<BoolT> tmp156;
    compiler::TNode<RegExpMatchInfo> tmp157;
    compiler::TNode<Smi> tmp158;
    compiler::TNode<Smi> tmp159;
    ca_.Bind(&block15, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 65);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 66);
    compiler::TNode<Smi> tmp160;
    USE(tmp160);
    tmp160 = ca_.UncheckedCast<Smi>(RegExpBuiltinsAssembler(state_).FastLoadLastIndex(compiler::TNode<JSRegExp>{tmp149}));
    compiler::TNode<Smi> tmp161;
    USE(tmp161);
    tmp161 = ca_.UncheckedCast<Smi>(RegExpBuiltinsAssembler(state_).AdvanceStringIndexFast(compiler::TNode<String>{tmp150}, compiler::TNode<Number>{tmp160}, compiler::TNode<BoolT>{tmp154}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 65);
    RegExpBuiltinsAssembler(state_).FastStoreLastIndex(compiler::TNode<JSRegExp>{tmp149}, compiler::TNode<Smi>{tmp161});
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 64);
    ca_.Goto(&block16, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp162;
    compiler::TNode<JSRegExp> tmp163;
    compiler::TNode<String> tmp164;
    compiler::TNode<String> tmp165;
    compiler::TNode<String> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<BoolT> tmp168;
    compiler::TNode<Smi> tmp169;
    compiler::TNode<BoolT> tmp170;
    compiler::TNode<RegExpMatchInfo> tmp171;
    compiler::TNode<Smi> tmp172;
    compiler::TNode<Smi> tmp173;
    ca_.Bind(&block16, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 47);
    ca_.Goto(&block6, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp174;
    compiler::TNode<JSRegExp> tmp175;
    compiler::TNode<String> tmp176;
    compiler::TNode<String> tmp177;
    compiler::TNode<String> tmp178;
    compiler::TNode<Smi> tmp179;
    compiler::TNode<BoolT> tmp180;
    compiler::TNode<Smi> tmp181;
    compiler::TNode<BoolT> tmp182;
    ca_.Bind(&block5, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 70);
    compiler::TNode<Smi> tmp183;
    USE(tmp183);
    tmp183 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadStringLengthAsSmi(compiler::TNode<String>{tmp176}));
    compiler::TNode<String> tmp184;
    tmp184 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kSubString, tmp174, tmp176, tmp179, tmp183));
    USE(tmp184);
    compiler::TNode<String> tmp185;
    USE(tmp185);
    tmp185 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp174}, compiler::TNode<String>{tmp178}, compiler::TNode<String>{tmp184}));
    ca_.Goto(&block1, tmp174, tmp175, tmp176, tmp177, tmp185);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp186;
    compiler::TNode<JSRegExp> tmp187;
    compiler::TNode<String> tmp188;
    compiler::TNode<String> tmp189;
    compiler::TNode<String> tmp190;
    ca_.Bind(&block1, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 31);
    ca_.Goto(&block17, tmp186, tmp187, tmp188, tmp189, tmp190);
  }

    compiler::TNode<Context> tmp191;
    compiler::TNode<JSRegExp> tmp192;
    compiler::TNode<String> tmp193;
    compiler::TNode<String> tmp194;
    compiler::TNode<String> tmp195;
    ca_.Bind(&block17, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195);
  return compiler::TNode<String>{tmp195};
}

TF_BUILTIN(RegExpReplace, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSRegExp> parameter1 = UncheckedCast<JSRegExp>(Parameter(Descriptor::kRegexp));
  USE(parameter1);
  compiler::TNode<String> parameter2 = UncheckedCast<String>(Parameter(Descriptor::kString));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kReplaceValue));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, Object, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, JSReceiver> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, JSReceiver> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, JSReceiver, String> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, JSReceiver, String> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, Object, JSRegExp, String, JSRegExp> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, Object, JSRegExp, String, JSRegExp, JSRegExp> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, Object, JSRegExp, String, JSRegExp> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, Object, JSRegExp, String, JSRegExp> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSRegExp, String, Object, Object, Object, JSRegExp, String> block10(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSRegExp> tmp1;
    compiler::TNode<String> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 80);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 81);
    compiler::TNode<JSReceiver> tmp4;
    USE(tmp4);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp3}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp3, tmp3, tmp4);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp3, tmp3);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<JSRegExp> tmp6;
    compiler::TNode<String> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    ca_.Bind(&block4, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.Goto(&block2, tmp5, tmp6, tmp7, tmp8, tmp9);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSRegExp> tmp12;
    compiler::TNode<String> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    ca_.Bind(&block3, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 82);
    compiler::TNode<BoolT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<BoolT>(RegExpBuiltinsAssembler(state_).FastFlagGetterGlobal(compiler::TNode<JSRegExp>{tmp12}));
    ca_.Branch(tmp18, &block5, &block6, tmp11, tmp12, tmp13, tmp14, tmp15, tmp17);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<JSRegExp> tmp20;
    compiler::TNode<String> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    ca_.Bind(&block5, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 83);
    compiler::TNode<String> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<String>(RegExpBuiltinsAssembler(state_).ReplaceGlobalCallableFastPath(compiler::TNode<Context>{tmp19}, compiler::TNode<JSRegExp>{tmp20}, compiler::TNode<String>{tmp21}, compiler::TNode<JSReceiver>{tmp24}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 82);
    ca_.Goto(&block8, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<JSRegExp> tmp27;
    compiler::TNode<String> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<JSReceiver> tmp31;
    ca_.Bind(&block6, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 84);
    compiler::TNode<String> tmp32;
    tmp32 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kStringReplaceNonGlobalRegExpWithFunction, tmp26, tmp28, tmp27, tmp31));
    USE(tmp32);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 82);
    ca_.Goto(&block7, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<JSRegExp> tmp34;
    compiler::TNode<String> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<JSReceiver> tmp38;
    compiler::TNode<String> tmp39;
    ca_.Bind(&block8, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.Goto(&block7, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<JSRegExp> tmp41;
    compiler::TNode<String> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<String> tmp46;
    ca_.Bind(&block7, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46);
    CodeStubAssembler(state_).Return(tmp46);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<JSRegExp> tmp48;
    compiler::TNode<String> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Object> tmp51;
    ca_.Bind(&block2, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 86);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 87);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 88);
    compiler::TNode<String> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp47}, compiler::TNode<Object>{tmp50}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 94);
    compiler::TNode<JSRegExp> tmp53;
    USE(tmp53);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp53 = BaseBuiltinsFromDSLAssembler(state_).Cast14ATFastJSRegExp(compiler::TNode<Context>{tmp47}, compiler::TNode<HeapObject>{tmp48}, &label0);
    ca_.Goto(&block11, tmp47, tmp48, tmp49, tmp50, tmp51, ca_.UncheckedCast<Object>(tmp51), tmp48, tmp52, tmp48, tmp53);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp47, tmp48, tmp49, tmp50, tmp51, ca_.UncheckedCast<Object>(tmp51), tmp48, tmp52, tmp48);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<JSRegExp> tmp55;
    compiler::TNode<String> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<JSRegExp> tmp60;
    compiler::TNode<String> tmp61;
    compiler::TNode<JSRegExp> tmp62;
    ca_.Bind(&block12, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.Goto(&block10, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<JSRegExp> tmp64;
    compiler::TNode<String> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<JSRegExp> tmp69;
    compiler::TNode<String> tmp70;
    compiler::TNode<JSRegExp> tmp71;
    compiler::TNode<JSRegExp> tmp72;
    ca_.Bind(&block11, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 96);
    compiler::TNode<String> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).SingleCharacterStringConstant("$"));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 95);
    compiler::TNode<Smi> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Smi> tmp75;
    tmp75 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kStringIndexOf, tmp63, tmp70, tmp73, tmp74));
    USE(tmp75);
    compiler::TNode<Smi> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(-1));
    compiler::TNode<BoolT> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiNotEqual(compiler::TNode<Smi>{tmp75}, compiler::TNode<Smi>{tmp76}));
    ca_.Branch(tmp77, &block13, &block14, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp72);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<JSRegExp> tmp79;
    compiler::TNode<String> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<JSRegExp> tmp84;
    compiler::TNode<String> tmp85;
    compiler::TNode<JSRegExp> tmp86;
    ca_.Bind(&block13, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 97);
    ca_.Goto(&block10, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<JSRegExp> tmp88;
    compiler::TNode<String> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<JSRegExp> tmp93;
    compiler::TNode<String> tmp94;
    compiler::TNode<JSRegExp> tmp95;
    ca_.Bind(&block14, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 100);
    compiler::TNode<String> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<String>(RegexpReplaceBuiltinsFromDSLAssembler(state_).RegExpReplaceFastString(compiler::TNode<Context>{tmp87}, compiler::TNode<JSRegExp>{tmp95}, compiler::TNode<String>{tmp89}, compiler::TNode<String>{tmp94}));
    CodeStubAssembler(state_).Return(tmp96);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<JSRegExp> tmp98;
    compiler::TNode<String> tmp99;
    compiler::TNode<Object> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<JSRegExp> tmp103;
    compiler::TNode<String> tmp104;
    ca_.Bind(&block10, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 103);
    compiler::TNode<String> tmp105;
    tmp105 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kRegExpReplaceRT, tmp97, tmp103, tmp99, tmp104));
    USE(tmp105);
    CodeStubAssembler(state_).Return(tmp105);
  }
}

TF_BUILTIN(RegExpPrototypeReplace, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSReceiver> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSReceiver, String, JSReceiver> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSReceiver, String, JSReceiver, JSRegExp> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSReceiver, String> block6(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 131);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 132);
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 136);
    compiler::TNode<JSReceiver> tmp9;
    USE(tmp9);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, tmp8, tmp1, tmp9);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, tmp8, tmp1);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<RawPtrT> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<IntPtrT> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Object> tmp17;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.Goto(&block2, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<RawPtrT> tmp20;
    compiler::TNode<RawPtrT> tmp21;
    compiler::TNode<IntPtrT> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<JSReceiver> tmp26;
    ca_.Bind(&block3, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.Goto(&block1, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp26);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<RawPtrT> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Object> tmp33;
    ca_.Bind(&block2, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 137);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp27}, MessageTemplate::kIncompatibleMethodReceiver, "RegExp.prototype.@@replace");
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    ca_.Bind(&block1, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 140);
    compiler::TNode<String> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp34}, compiler::TNode<Object>{tmp39}));
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 144);
    compiler::TNode<JSRegExp> tmp43;
    USE(tmp43);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp43 = BaseBuiltinsFromDSLAssembler(state_).Cast14ATFastJSRegExp(compiler::TNode<Context>{tmp34}, compiler::TNode<HeapObject>{tmp41}, &label0);
    ca_.Goto(&block7, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp41, tmp43);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp41);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<RawPtrT> tmp46;
    compiler::TNode<RawPtrT> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<JSReceiver> tmp51;
    compiler::TNode<String> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    ca_.Bind(&block8, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.Goto(&block6, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<RawPtrT> tmp56;
    compiler::TNode<RawPtrT> tmp57;
    compiler::TNode<IntPtrT> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<JSReceiver> tmp61;
    compiler::TNode<String> tmp62;
    compiler::TNode<JSReceiver> tmp63;
    compiler::TNode<JSRegExp> tmp64;
    ca_.Bind(&block7, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 145);
    compiler::TNode<String> tmp65;
    tmp65 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kRegExpReplace, tmp54, tmp64, tmp62, tmp60));
    USE(tmp65);
    arguments.PopAndReturn(tmp65);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<RawPtrT> tmp69;
    compiler::TNode<IntPtrT> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<JSReceiver> tmp73;
    compiler::TNode<String> tmp74;
    ca_.Bind(&block6, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    ca_.SetSourcePosition("../../v8/src/builtins/regexp-replace.tq", 148);
    compiler::TNode<String> tmp75;
    tmp75 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kRegExpReplaceRT, tmp66, tmp73, tmp74, tmp72));
    USE(tmp75);
    arguments.PopAndReturn(tmp75);
  }
}

}  // namespace internal
}  // namespace v8

