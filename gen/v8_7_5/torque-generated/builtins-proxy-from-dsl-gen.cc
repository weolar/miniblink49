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

compiler::TNode<JSProxy> ProxyBuiltinsFromDSLAssembler::ValidateProxy(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o, compiler::TNode<String> p_method) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, Object, JSProxy> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, JSProxy> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, JSProxy> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_method);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<String> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 11);
    compiler::TNode<JSProxy> tmp3;
    USE(tmp3);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).Cast7JSProxy(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp1, tmp3);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp1);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<String> tmp6;
    compiler::TNode<Object> tmp7;
    ca_.Bind(&block5, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.Goto(&block3, tmp4, tmp5, tmp6);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<String> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<JSProxy> tmp12;
    ca_.Bind(&block4, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.Goto(&block1, tmp8, tmp9, tmp10, tmp12);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<String> tmp15;
    ca_.Bind(&block3, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 14);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp13}, MessageTemplate::kIncompatibleMethodReceiver, compiler::TNode<Object>{tmp15});
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<String> tmp18;
    compiler::TNode<JSProxy> tmp19;
    ca_.Bind(&block1, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 9);
    ca_.Goto(&block6, tmp16, tmp17, tmp18, tmp19);
  }

    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<String> tmp22;
    compiler::TNode<JSProxy> tmp23;
    ca_.Bind(&block6, &tmp20, &tmp21, &tmp22, &tmp23);
  return compiler::TNode<JSProxy>{tmp23};
}

TF_BUILTIN(ProxyRevoke, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Object> parameter1 = UncheckedCast<Object>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 24);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr13ATContextSlot23ATconstexpr_ContextSlot(Context::MIN_CONTEXT_SLOTS));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadContextElement(compiler::TNode<Context>{tmp0}, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 27);
    compiler::TNode<Oddball> tmp4;
    USE(tmp4);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp3}, compiler::TNode<HeapObject>{tmp4}));
    ca_.Branch(tmp5, &block1, &block2, tmp0, tmp1, tmp3);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<Object> tmp8;
    ca_.Bind(&block1, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 28);
    compiler::TNode<Oddball> tmp9;
    USE(tmp9);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp9);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 32);
    compiler::TNode<Oddball> tmp13;
    USE(tmp13);
    tmp13 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<IntPtrT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr13ATContextSlot23ATconstexpr_ContextSlot(Context::MIN_CONTEXT_SLOTS));
    CodeStubAssembler(state_).StoreContextElement(compiler::TNode<Context>{tmp10}, compiler::TNode<IntPtrT>{tmp14}, compiler::TNode<Object>{tmp13});
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 35);
    compiler::TNode<JSProxy> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<JSProxy>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSProxy(compiler::TNode<Context>{tmp10}, compiler::TNode<Object>{tmp12}));
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 38);
    compiler::TNode<Oddball> tmp16;
    USE(tmp16);
    tmp16 = BaseBuiltinsFromDSLAssembler(state_).Null();
    BaseBuiltinsFromDSLAssembler(state_).StoreJSProxyTarget(compiler::TNode<JSProxy>{tmp15}, compiler::TNode<Object>{tmp16});
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 41);
    compiler::TNode<Oddball> tmp17;
    USE(tmp17);
    tmp17 = BaseBuiltinsFromDSLAssembler(state_).Null();
    BaseBuiltinsFromDSLAssembler(state_).StoreJSProxyHandler(compiler::TNode<JSProxy>{tmp15}, compiler::TNode<Object>{tmp17});
    ca_.SetSourcePosition("../../src/builtins/proxy-revoke.tq", 44);
    compiler::TNode<Oddball> tmp18;
    USE(tmp18);
    tmp18 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp18);
  }
}

}  // namespace internal
}  // namespace v8

