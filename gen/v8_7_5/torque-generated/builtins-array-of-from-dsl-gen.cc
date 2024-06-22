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

TF_BUILTIN(ArrayOf, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Smi, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Smi, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Object, Object, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Smi, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Smi, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Smi, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Smi, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Smi, RawPtrT, RawPtrT, IntPtrT, Object, JSReceiver, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 10);
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp4}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 13);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 16);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 18);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 21);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 22);
    compiler::TNode<JSReceiver> tmp6;
    USE(tmp6);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Cast13ATConstructor(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp2, tmp3, tmp4, tmp1, ca_.Uninitialized<JSReceiver>(), tmp1, tmp1, tmp6);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp2, tmp3, tmp4, tmp1, ca_.Uninitialized<JSReceiver>(), tmp1, tmp1);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    ca_.Bind(&block4, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block2, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<RawPtrT> tmp26;
    compiler::TNode<RawPtrT> tmp27;
    compiler::TNode<IntPtrT> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<JSReceiver> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 24);
    compiler::TNode<JSReceiver> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).Construct(compiler::TNode<Context>{tmp20}, compiler::TNode<JSReceiver>{tmp33}, compiler::TNode<Object>{tmp25}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 22);
    ca_.Goto(&block1, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp34, tmp31);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<RawPtrT> tmp38;
    compiler::TNode<IntPtrT> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<RawPtrT> tmp41;
    compiler::TNode<RawPtrT> tmp42;
    compiler::TNode<IntPtrT> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Object> tmp46;
    ca_.Bind(&block2, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 26);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 28);
    compiler::TNode<JSArray> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<JSArray>(CodeStubAssembler(state_).ArrayCreate(compiler::TNode<Context>{tmp35}, compiler::TNode<Number>{tmp40}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 22);
    ca_.Goto(&block1, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp47, tmp46);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<RawPtrT> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<IntPtrT> tmp52;
    compiler::TNode<Smi> tmp53;
    compiler::TNode<RawPtrT> tmp54;
    compiler::TNode<RawPtrT> tmp55;
    compiler::TNode<IntPtrT> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    compiler::TNode<Object> tmp59;
    ca_.Bind(&block1, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 21);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 33);
    compiler::TNode<Smi> tmp60;
    USE(tmp60);
    tmp60 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 36);
    ca_.Goto(&block7, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp60);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<RawPtrT> tmp63;
    compiler::TNode<RawPtrT> tmp64;
    compiler::TNode<IntPtrT> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<JSReceiver> tmp71;
    compiler::TNode<Smi> tmp72;
    ca_.Bind(&block7, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    compiler::TNode<BoolT> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp72}, compiler::TNode<Smi>{tmp66}));
    ca_.Branch(tmp73, &block5, &block6, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<RawPtrT> tmp80;
    compiler::TNode<RawPtrT> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<JSReceiver> tmp84;
    compiler::TNode<Smi> tmp85;
    ca_.Bind(&block5, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 38);
    compiler::TNode<IntPtrT> tmp86;
    USE(tmp86);
    tmp86 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp85}));
    compiler::TNode<Object> tmp87;
    USE(tmp87);
    tmp87 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp80}, compiler::TNode<RawPtrT>{tmp81}, compiler::TNode<IntPtrT>{tmp82}}, compiler::TNode<IntPtrT>{tmp86}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 42);
    compiler::TNode<Object> tmp88;
    tmp88 = CodeStubAssembler(state_).CallBuiltin(Builtins::kFastCreateDataProperty, tmp74, tmp84, tmp85, tmp87);
    USE(tmp88);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 45);
    compiler::TNode<Smi> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp85}, compiler::TNode<Smi>{tmp89}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 36);
    ca_.Goto(&block7, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp90);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<RawPtrT> tmp94;
    compiler::TNode<IntPtrT> tmp95;
    compiler::TNode<Smi> tmp96;
    compiler::TNode<RawPtrT> tmp97;
    compiler::TNode<RawPtrT> tmp98;
    compiler::TNode<IntPtrT> tmp99;
    compiler::TNode<Object> tmp100;
    compiler::TNode<JSReceiver> tmp101;
    compiler::TNode<Smi> tmp102;
    ca_.Bind(&block6, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 49);
    CodeStubAssembler(state_).SetPropertyLength(compiler::TNode<Context>{tmp91}, compiler::TNode<Object>{tmp101}, compiler::TNode<Number>{tmp96});
    ca_.SetSourcePosition("../../v8/src/builtins/array-of.tq", 52);
    arguments.PopAndReturn(tmp101);
  }
}

}  // namespace internal
}  // namespace v8

