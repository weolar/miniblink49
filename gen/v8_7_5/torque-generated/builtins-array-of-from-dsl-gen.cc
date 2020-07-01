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

TF_BUILTIN(ArrayOf, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Smi, Object, JSReceiver, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Smi, Object, JSReceiver, Object, Object, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Smi, Object, JSReceiver, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Smi, Object, JSReceiver, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Smi, Object, JSReceiver, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Smi, Object, JSReceiver, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Smi, Object, JSReceiver, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 10);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(arguments));
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 16);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 18);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 21);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 22);
    compiler::TNode<JSReceiver> tmp4;
    USE(tmp4);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).Cast13ATConstructor(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp3, tmp1, ca_.Uninitialized<JSReceiver>(), tmp1, tmp1, tmp4);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp3, tmp1, ca_.Uninitialized<JSReceiver>(), tmp1, tmp1);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<JSReceiver> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    ca_.Bind(&block4, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.Goto(&block2, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<JSReceiver> tmp19;
    ca_.Bind(&block3, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 24);
    compiler::TNode<JSReceiver> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).Construct(compiler::TNode<Context>{tmp12}, compiler::TNode<JSReceiver>{tmp19}, compiler::TNode<Object>{tmp14}));
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 22);
    ca_.Goto(&block1, tmp12, tmp13, tmp14, tmp15, tmp20, tmp17);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Object> tmp26;
    ca_.Bind(&block2, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 26);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 28);
    compiler::TNode<JSArray> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<JSArray>(CodeStubAssembler(state_).ArrayCreate(compiler::TNode<Context>{tmp21}, compiler::TNode<Number>{tmp23}));
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 22);
    ca_.Goto(&block1, tmp21, tmp22, tmp23, tmp24, tmp27, tmp26);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Smi> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Object> tmp33;
    ca_.Bind(&block1, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 21);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 33);
    compiler::TNode<Smi> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 36);
    ca_.Goto(&block7, tmp28, tmp29, tmp30, tmp31, tmp32, tmp34);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Smi> tmp40;
    ca_.Bind(&block7, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    compiler::TNode<BoolT> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp40}, compiler::TNode<Smi>{tmp37}));
    ca_.Branch(tmp41, &block5, &block6, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Smi> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<JSReceiver> tmp46;
    compiler::TNode<Smi> tmp47;
    ca_.Bind(&block5, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 38);
    compiler::TNode<IntPtrT> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp47}));
    compiler::TNode<Object> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp48}));
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 42);
    compiler::TNode<Object> tmp50;
    tmp50 = CodeStubAssembler(state_).CallBuiltin(Builtins::kFastCreateDataProperty, tmp42, tmp46, tmp47, tmp49);
    USE(tmp50);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 45);
    compiler::TNode<Smi> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp47}, compiler::TNode<Smi>{tmp51}));
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 36);
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp52);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Smi> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<Smi> tmp58;
    ca_.Bind(&block6, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 49);
    CodeStubAssembler(state_).SetPropertyLength(compiler::TNode<Context>{tmp53}, compiler::TNode<Object>{tmp57}, compiler::TNode<Number>{tmp55});
    ca_.SetSourcePosition("../../src/builtins/array-of.tq", 52);
    arguments->PopAndReturn(tmp57);
  }
}

}  // namespace internal
}  // namespace v8

