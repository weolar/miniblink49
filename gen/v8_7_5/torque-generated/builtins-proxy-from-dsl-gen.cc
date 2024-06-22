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

compiler::TNode<JSProxy> ProxyBuiltinsFromDSLAssembler::AllocateProxy(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_target, compiler::TNode<JSReceiver> p_handler) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, JSProxy> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, JSProxy> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_target, p_handler);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy.tq", 13);
    compiler::TNode<JSProxy> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSProxy>(ProxiesCodeStubAssembler(state_).AllocateProxy(compiler::TNode<JSReceiver>{tmp1}, compiler::TNode<JSReceiver>{tmp2}, compiler::TNode<Context>{tmp0}));
    ca_.Goto(&block1, tmp0, tmp1, tmp2, tmp3);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp4;
    compiler::TNode<JSReceiver> tmp5;
    compiler::TNode<JSReceiver> tmp6;
    compiler::TNode<JSProxy> tmp7;
    ca_.Bind(&block1, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy.tq", 11);
    ca_.Goto(&block2, tmp4, tmp5, tmp6, tmp7);
  }

    compiler::TNode<Context> tmp8;
    compiler::TNode<JSReceiver> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<JSProxy> tmp11;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10, &tmp11);
  return compiler::TNode<JSProxy>{tmp11};
}

compiler::TNode<BoolT> ProxyBuiltinsFromDSLAssembler::IsRevokedProxy(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSReceiver, JSProxy> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSProxy> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSProxy, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSProxy, Object, JSReceiver> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSProxy> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, JSProxy, JSReceiver> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, BoolT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy.tq", 17);
    compiler::TNode<JSProxy> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Cast7JSProxy(compiler::TNode<HeapObject>{tmp1}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp1, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp1);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<JSReceiver> tmp4;
    compiler::TNode<JSReceiver> tmp5;
    ca_.Bind(&block5, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block3, tmp3, tmp4);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<JSReceiver> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<JSProxy> tmp9;
    ca_.Bind(&block4, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block2, tmp6, tmp7, tmp9);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    ca_.Bind(&block3, &tmp10, &tmp11);
    compiler::TNode<BoolT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block1, tmp10, tmp11, tmp12);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<JSReceiver> tmp14;
    compiler::TNode<JSProxy> tmp15;
    ca_.Bind(&block2, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy.tq", 19);
    compiler::TNode<IntPtrT> tmp16 = ca_.IntPtrConstant(JSProxy::kHandlerOffset);
    USE(tmp16);
    compiler::TNode<Object>tmp17 = CodeStubAssembler(state_).LoadReference<Object>(CodeStubAssembler::Reference{tmp15, tmp16});
    compiler::TNode<JSReceiver> tmp18;
    USE(tmp18);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp18 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp13}, compiler::TNode<Object>{tmp17}, &label0);
    ca_.Goto(&block8, tmp13, tmp14, tmp15, tmp17, tmp18);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp13, tmp14, tmp15, tmp17);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<JSReceiver> tmp20;
    compiler::TNode<JSProxy> tmp21;
    compiler::TNode<Object> tmp22;
    ca_.Bind(&block9, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block7, tmp19, tmp20, tmp21);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<JSProxy> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<JSReceiver> tmp27;
    ca_.Bind(&block8, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block6, tmp23, tmp24, tmp25, tmp27);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<JSProxy> tmp30;
    ca_.Bind(&block7, &tmp28, &tmp29, &tmp30);
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Goto(&block1, tmp28, tmp29, tmp31);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<JSProxy> tmp34;
    compiler::TNode<JSReceiver> tmp35;
    ca_.Bind(&block6, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy.tq", 18);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy.tq", 20);
    compiler::TNode<BoolT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.Goto(&block1, tmp32, tmp33, tmp36);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<JSReceiver> tmp38;
    compiler::TNode<BoolT> tmp39;
    ca_.Bind(&block1, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy.tq", 16);
    ca_.Goto(&block10, tmp37, tmp38, tmp39);
  }

    compiler::TNode<Context> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    compiler::TNode<BoolT> tmp42;
    ca_.Bind(&block10, &tmp40, &tmp41, &tmp42);
  return compiler::TNode<BoolT>{tmp42};
}

TF_BUILTIN(ProxyConstructor, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Object> parameter1 = UncheckedCast<Object>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Object> parameter2 = UncheckedCast<Object>(Parameter(Descriptor::kTarget));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kHandler));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object, JSReceiver> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, Object, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, JSReceiver> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver, JSReceiver> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 19);
    compiler::TNode<Object> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Object>(ProxiesCodeStubAssembler(state_).GetProxyConstructorJSNewTarget());
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 20);
    compiler::TNode<Oddball> tmp5;
    USE(tmp5);
    tmp5 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp4}, compiler::TNode<HeapObject>{tmp5}));
    ca_.Branch(tmp6, &block5, &block6, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    ca_.Bind(&block5, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 21);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp7}, MessageTemplate::kConstructorNotFunction, "Proxy");
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Object> tmp16;
    ca_.Bind(&block6, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 33);
    compiler::TNode<JSReceiver> tmp17;
    USE(tmp17);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp17 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp12}, compiler::TNode<Object>{tmp14}, &label0);
    ca_.Goto(&block7, tmp12, tmp13, tmp14, tmp15, tmp16, tmp14, tmp17);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp12, tmp13, tmp14, tmp15, tmp16, tmp14);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<Object> tmp23;
    ca_.Bind(&block8, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.Goto(&block4, tmp18, tmp19, tmp20, tmp21);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<JSReceiver> tmp30;
    ca_.Bind(&block7, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 34);
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(ProxyBuiltinsFromDSLAssembler(state_).IsRevokedProxy(compiler::TNode<Context>{tmp24}, compiler::TNode<JSReceiver>{tmp30}));
    ca_.Branch(tmp31, &block9, &block10, tmp24, tmp25, tmp26, tmp27, tmp28, tmp30);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    ca_.Bind(&block9, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 35);
    ca_.Goto(&block2, tmp32, tmp33, tmp34, tmp35);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<JSReceiver> tmp43;
    ca_.Bind(&block10, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 39);
    compiler::TNode<JSReceiver> tmp44;
    USE(tmp44);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp44 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp38}, compiler::TNode<Object>{tmp41}, &label0);
    ca_.Goto(&block11, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp41, tmp44);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp41);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<JSReceiver> tmp50;
    compiler::TNode<Object> tmp51;
    ca_.Bind(&block12, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.Goto(&block4, tmp45, tmp46, tmp47, tmp48);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<JSReceiver> tmp59;
    ca_.Bind(&block11, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 40);
    compiler::TNode<BoolT> tmp60;
    USE(tmp60);
    tmp60 = ca_.UncheckedCast<BoolT>(ProxyBuiltinsFromDSLAssembler(state_).IsRevokedProxy(compiler::TNode<Context>{tmp52}, compiler::TNode<JSReceiver>{tmp59}));
    ca_.Branch(tmp60, &block13, &block14, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp59);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<JSReceiver> tmp66;
    compiler::TNode<JSReceiver> tmp67;
    ca_.Bind(&block13, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 41);
    ca_.Goto(&block2, tmp61, tmp62, tmp63, tmp64);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<JSReceiver> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    ca_.Bind(&block14, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 54);
    compiler::TNode<JSProxy> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<JSProxy>(ProxyBuiltinsFromDSLAssembler(state_).AllocateProxy(compiler::TNode<Context>{tmp68}, compiler::TNode<JSReceiver>{tmp73}, compiler::TNode<JSReceiver>{tmp74}));
    CodeStubAssembler(state_).Return(tmp75);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<Object> tmp79;
    ca_.Bind(&block4, &tmp76, &tmp77, &tmp78, &tmp79);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 57);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp76}, MessageTemplate::kProxyNonObject);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Object> tmp83;
    ca_.Bind(&block2, &tmp80, &tmp81, &tmp82, &tmp83);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-constructor.tq", 60);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp80}, MessageTemplate::kProxyHandlerOrTargetRevoked);
  }
}

compiler::TNode<JSFunction> ProxyBuiltinsFromDSLAssembler::AllocateProxyRevokeFunction(compiler::TNode<Context> p_context, compiler::TNode<JSProxy> p_proxy) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSProxy> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSProxy, JSFunction> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSProxy, JSFunction> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_proxy);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSProxy> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 13);
    compiler::TNode<JSFunction> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<JSFunction>(ProxiesCodeStubAssembler(state_).AllocateProxyRevokeFunction(compiler::TNode<Object>{tmp1}, compiler::TNode<Object>{tmp0}));
    ca_.Goto(&block1, tmp0, tmp1, tmp2);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<JSProxy> tmp4;
    compiler::TNode<JSFunction> tmp5;
    ca_.Bind(&block1, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 11);
    ca_.Goto(&block2, tmp3, tmp4, tmp5);
  }

    compiler::TNode<Context> tmp6;
    compiler::TNode<JSProxy> tmp7;
    compiler::TNode<JSFunction> tmp8;
    ca_.Bind(&block2, &tmp6, &tmp7, &tmp8);
  return compiler::TNode<JSFunction>{tmp8};
}

TF_BUILTIN(ProxyRevocable, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Object> parameter1 = UncheckedCast<Object>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Object> parameter2 = UncheckedCast<Object>(Parameter(Descriptor::kTarget));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kHandler));
  USE(parameter3);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSReceiver> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver, Object, JSReceiver> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 24);
    compiler::TNode<JSReceiver> tmp4;
    USE(tmp4);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}, &label0);
    ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp2, tmp4);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp0, tmp1, tmp2, tmp3, tmp2);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    ca_.Bind(&block6, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block4, tmp5, tmp6, tmp7, tmp8);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    ca_.Bind(&block5, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 25);
    compiler::TNode<BoolT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<BoolT>(ProxyBuiltinsFromDSLAssembler(state_).IsRevokedProxy(compiler::TNode<Context>{tmp10}, compiler::TNode<JSReceiver>{tmp15}));
    ca_.Branch(tmp16, &block7, &block8, tmp10, tmp11, tmp12, tmp13, tmp15);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    ca_.Bind(&block7, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 26);
    ca_.Goto(&block2, tmp17, tmp18, tmp19, tmp20);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<JSReceiver> tmp26;
    ca_.Bind(&block8, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 30);
    compiler::TNode<JSReceiver> tmp27;
    USE(tmp27);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp27 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp22}, compiler::TNode<Object>{tmp25}, &label0);
    ca_.Goto(&block9, tmp22, tmp23, tmp24, tmp25, tmp26, tmp25, tmp27);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp22, tmp23, tmp24, tmp25, tmp26, tmp25);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Object> tmp33;
    ca_.Bind(&block10, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.Goto(&block4, tmp28, tmp29, tmp30, tmp31);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<JSReceiver> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<JSReceiver> tmp40;
    ca_.Bind(&block9, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 31);
    compiler::TNode<BoolT> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<BoolT>(ProxyBuiltinsFromDSLAssembler(state_).IsRevokedProxy(compiler::TNode<Context>{tmp34}, compiler::TNode<JSReceiver>{tmp40}));
    ca_.Branch(tmp41, &block11, &block12, tmp34, tmp35, tmp36, tmp37, tmp38, tmp40);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<JSReceiver> tmp46;
    compiler::TNode<JSReceiver> tmp47;
    ca_.Bind(&block11, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 32);
    ca_.Goto(&block2, tmp42, tmp43, tmp44, tmp45);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<JSReceiver> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    ca_.Bind(&block12, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 36);
    compiler::TNode<JSProxy> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<JSProxy>(ProxyBuiltinsFromDSLAssembler(state_).AllocateProxy(compiler::TNode<Context>{tmp48}, compiler::TNode<JSReceiver>{tmp52}, compiler::TNode<JSReceiver>{tmp53}));
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 42);
    compiler::TNode<JSFunction> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<JSFunction>(ProxyBuiltinsFromDSLAssembler(state_).AllocateProxyRevokeFunction(compiler::TNode<Context>{tmp48}, compiler::TNode<JSProxy>{tmp54}));
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 48);
    compiler::TNode<JSProxyRevocableResult> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<JSProxyRevocableResult>(BaseBuiltinsFromDSLAssembler(state_).NewJSProxyRevocableResult(compiler::TNode<Context>{tmp48}, compiler::TNode<JSProxy>{tmp54}, compiler::TNode<JSFunction>{tmp55}));
    CodeStubAssembler(state_).Return(tmp56);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Object> tmp60;
    ca_.Bind(&block4, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 51);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp57}, MessageTemplate::kProxyNonObject, "Proxy.revocable");
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<Object> tmp64;
    ca_.Bind(&block2, &tmp61, &tmp62, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revocable.tq", 54);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp61}, MessageTemplate::kProxyHandlerOrTargetRevoked, "Proxy.revocable");
  }
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
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revoke.tq", 15);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr13ATContextSlot23ATconstexpr_ContextSlot(Context::MIN_CONTEXT_SLOTS));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadContextElement(compiler::TNode<Context>{tmp0}, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revoke.tq", 18);
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
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revoke.tq", 19);
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
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revoke.tq", 23);
    compiler::TNode<Oddball> tmp13;
    USE(tmp13);
    tmp13 = BaseBuiltinsFromDSLAssembler(state_).Null();
    compiler::TNode<IntPtrT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr13ATContextSlot23ATconstexpr_ContextSlot(Context::MIN_CONTEXT_SLOTS));
    CodeStubAssembler(state_).StoreContextElement(compiler::TNode<Context>{tmp10}, compiler::TNode<IntPtrT>{tmp14}, compiler::TNode<Object>{tmp13});
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revoke.tq", 26);
    compiler::TNode<JSProxy> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<JSProxy>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast7JSProxy(compiler::TNode<Context>{tmp10}, compiler::TNode<Object>{tmp12}));
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revoke.tq", 29);
    compiler::TNode<IntPtrT> tmp16 = ca_.IntPtrConstant(JSProxy::kTargetOffset);
    USE(tmp16);
    compiler::TNode<Oddball> tmp17;
    USE(tmp17);
    tmp17 = BaseBuiltinsFromDSLAssembler(state_).Null();
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp15, tmp16}, tmp17);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revoke.tq", 32);
    compiler::TNode<IntPtrT> tmp18 = ca_.IntPtrConstant(JSProxy::kHandlerOffset);
    USE(tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Null();
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp15, tmp18}, tmp19);
    ca_.SetSourcePosition("../../v8/src/builtins/proxy-revoke.tq", 35);
    compiler::TNode<Oddball> tmp20;
    USE(tmp20);
    tmp20 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    CodeStubAssembler(state_).Return(tmp20);
  }
}

}  // namespace internal
}  // namespace v8

