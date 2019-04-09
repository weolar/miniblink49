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

const char* TypedArrayForeachBuiltinsFromDSLAssembler::kBuiltinName() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return "%TypedArray%.prototype.forEach";
}

compiler::TNode<Object> TypedArrayForeachBuiltinsFromDSLAssembler::ForEachAllElements(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_array, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_thisArg) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi, Smi, Context, Smi, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi, Object, Context, JSReceiver, Object, Object, Smi, JSTypedArray> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_array, p_callbackfn, p_thisArg);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 13);
    compiler::TNode<JSTypedArray> tmp4;
    USE(tmp4);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    compiler::TNode<BuiltinPtr> tmp6;
    USE(tmp6);
    std::tie(tmp4, tmp5, tmp6) = TypedArrayBuiltinsFromDSLAssembler(state_).NewAttachedJSTypedArrayWitness(compiler::TNode<JSTypedArray>{tmp1}).Flatten();
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 14);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadJSTypedArrayLength(compiler::TNode<JSTypedArray>{tmp1}));
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi5ATSmi(compiler::TNode<Smi>{tmp7}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 15);
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp8, tmp9);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSTypedArray> tmp11;
    compiler::TNode<JSReceiver> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    compiler::TNode<JSTypedArray> tmp15;
    compiler::TNode<BuiltinPtr> tmp16;
    compiler::TNode<Smi> tmp17;
    compiler::TNode<Smi> tmp18;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp18}, compiler::TNode<Smi>{tmp17}));
    ca_.Branch(tmp19, &block2, &block3, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<JSTypedArray> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<JSTypedArray> tmp24;
    compiler::TNode<JSTypedArray> tmp25;
    compiler::TNode<BuiltinPtr> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<Smi> tmp28;
    ca_.Bind(&block2, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 96);
    compiler::TNode<JSArrayBuffer> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp24}));
    compiler::TNode<BoolT> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp29}));
    ca_.Branch(tmp30, &block9, &block10, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<JSTypedArray> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<JSTypedArray> tmp35;
    compiler::TNode<JSTypedArray> tmp36;
    compiler::TNode<BuiltinPtr> tmp37;
    compiler::TNode<Smi> tmp38;
    compiler::TNode<Smi> tmp39;
    ca_.Bind(&block9, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.Goto(&block7, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<JSTypedArray> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<JSTypedArray> tmp44;
    compiler::TNode<JSTypedArray> tmp45;
    compiler::TNode<BuiltinPtr> tmp46;
    compiler::TNode<Smi> tmp47;
    compiler::TNode<Smi> tmp48;
    ca_.Bind(&block10, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 97);
    compiler::TNode<JSTypedArray> tmp49;
    USE(tmp49);
    tmp49 = (compiler::TNode<JSTypedArray>{tmp44});
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 17);
    ca_.Goto(&block8, tmp40, tmp41, tmp42, tmp43, tmp44, tmp49, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<JSTypedArray> tmp51;
    compiler::TNode<JSReceiver> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<JSTypedArray> tmp54;
    compiler::TNode<JSTypedArray> tmp55;
    compiler::TNode<BuiltinPtr> tmp56;
    compiler::TNode<Smi> tmp57;
    compiler::TNode<Smi> tmp58;
    ca_.Bind(&block8, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    ca_.Goto(&block6, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<JSTypedArray> tmp60;
    compiler::TNode<JSReceiver> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<JSTypedArray> tmp63;
    compiler::TNode<JSTypedArray> tmp64;
    compiler::TNode<BuiltinPtr> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<Smi> tmp67;
    ca_.Bind(&block7, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.Goto(&block3, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<JSTypedArray> tmp69;
    compiler::TNode<JSReceiver> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<JSTypedArray> tmp72;
    compiler::TNode<JSTypedArray> tmp73;
    compiler::TNode<BuiltinPtr> tmp74;
    compiler::TNode<Smi> tmp75;
    compiler::TNode<Smi> tmp76;
    ca_.Bind(&block6, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 18);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 101);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 102);
    compiler::TNode<Object> tmp77 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(1)).descriptor(), tmp74, tmp68, tmp73, tmp76); 
    USE(tmp77);
    ca_.Goto(&block11, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp76, tmp68, tmp76, tmp77);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<JSTypedArray> tmp79;
    compiler::TNode<JSReceiver> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<JSTypedArray> tmp82;
    compiler::TNode<JSTypedArray> tmp83;
    compiler::TNode<BuiltinPtr> tmp84;
    compiler::TNode<Smi> tmp85;
    compiler::TNode<Smi> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<Context> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<Object> tmp90;
    ca_.Bind(&block11, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 18);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 19);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 92);
    ca_.Goto(&block12, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp90, tmp78, tmp80, tmp81, tmp90, tmp86, tmp82);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp91;
    compiler::TNode<JSTypedArray> tmp92;
    compiler::TNode<JSReceiver> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<JSTypedArray> tmp95;
    compiler::TNode<JSTypedArray> tmp96;
    compiler::TNode<BuiltinPtr> tmp97;
    compiler::TNode<Smi> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<Object> tmp100;
    compiler::TNode<Context> tmp101;
    compiler::TNode<JSReceiver> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<JSTypedArray> tmp106;
    ca_.Bind(&block12, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 19);
    compiler::TNode<Object> tmp107;
    USE(tmp107);
    tmp107 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp101}, compiler::TNode<JSReceiver>{tmp102}, compiler::TNode<Object>{tmp103}, compiler::TNode<Object>{tmp104}, compiler::TNode<Object>{tmp105}, compiler::TNode<Object>{tmp106}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 15);
    ca_.Goto(&block5, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp108;
    compiler::TNode<JSTypedArray> tmp109;
    compiler::TNode<JSReceiver> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<JSTypedArray> tmp112;
    compiler::TNode<JSTypedArray> tmp113;
    compiler::TNode<BuiltinPtr> tmp114;
    compiler::TNode<Smi> tmp115;
    compiler::TNode<Smi> tmp116;
    ca_.Bind(&block5, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116);
    compiler::TNode<Smi> tmp117;
    USE(tmp117);
    tmp117 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp116}, compiler::TNode<Smi>{tmp117}));
    ca_.Goto(&block4, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp118);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp119;
    compiler::TNode<JSTypedArray> tmp120;
    compiler::TNode<JSReceiver> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<JSTypedArray> tmp123;
    compiler::TNode<JSTypedArray> tmp124;
    compiler::TNode<BuiltinPtr> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Smi> tmp127;
    ca_.Bind(&block3, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 21);
    compiler::TNode<Oddball> tmp128;
    USE(tmp128);
    tmp128 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block1, tmp119, tmp120, tmp121, tmp122, tmp128);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp129;
    compiler::TNode<JSTypedArray> tmp130;
    compiler::TNode<JSReceiver> tmp131;
    compiler::TNode<Object> tmp132;
    compiler::TNode<Object> tmp133;
    ca_.Bind(&block1, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 10);
    ca_.Goto(&block13, tmp129, tmp130, tmp131, tmp132, tmp133);
  }

    compiler::TNode<Context> tmp134;
    compiler::TNode<JSTypedArray> tmp135;
    compiler::TNode<JSReceiver> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<Object> tmp138;
    ca_.Bind(&block13, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138);
  return compiler::TNode<Object>{tmp138};
}

TF_BUILTIN(TypedArrayPrototypeForEach, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSTypedArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSTypedArray> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSTypedArray, JSTypedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSTypedArray, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSTypedArray, Object, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block6(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 32);
    compiler::TNode<JSTypedArray> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Cast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block7, tmp0, tmp1, tmp1, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp0, tmp1, tmp1);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block8, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block4, tmp3, tmp4);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<JSTypedArray> tmp9;
    ca_.Bind(&block7, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 34);
    compiler::TNode<JSTypedArray> tmp10;
    USE(tmp10);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp10 = TypedArrayBuiltinsFromDSLAssembler(state_).EnsureAttached(compiler::TNode<JSTypedArray>{tmp9}, &label0);
    ca_.Goto(&block9, tmp6, tmp7, tmp9, tmp9, tmp10);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp6, tmp7, tmp9, tmp9);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<JSTypedArray> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.Goto(&block2, tmp11, tmp12);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<JSTypedArray> tmp17;
    compiler::TNode<JSTypedArray> tmp18;
    compiler::TNode<JSTypedArray> tmp19;
    ca_.Bind(&block9, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 36);
    compiler::TNode<IntPtrT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp20}));
    compiler::TNode<JSReceiver> tmp22;
    USE(tmp22);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp22 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp15}, compiler::TNode<Object>{tmp21}, &label0);
    ca_.Goto(&block11, tmp15, tmp16, tmp17, tmp19, tmp21, tmp22);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp15, tmp16, tmp17, tmp19, tmp21);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<JSTypedArray> tmp25;
    compiler::TNode<JSTypedArray> tmp26;
    compiler::TNode<Object> tmp27;
    ca_.Bind(&block12, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block6, tmp23, tmp24);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<JSTypedArray> tmp30;
    compiler::TNode<JSTypedArray> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    ca_.Bind(&block11, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 37);
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp34}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 38);
    compiler::TNode<Object> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<Object>(TypedArrayForeachBuiltinsFromDSLAssembler(state_).ForEachAllElements(compiler::TNode<Context>{tmp28}, compiler::TNode<JSTypedArray>{tmp31}, compiler::TNode<JSReceiver>{tmp33}, compiler::TNode<Object>{tmp35}));
    arguments->PopAndReturn(tmp36);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<Object> tmp38;
    ca_.Bind(&block6, &tmp37, &tmp38);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 41);
    compiler::TNode<IntPtrT> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp39}));
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp37}, MessageTemplate::kCalledNonCallable, compiler::TNode<Object>{tmp40});
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<Object> tmp42;
    ca_.Bind(&block4, &tmp41, &tmp42);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 44);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp41}, MessageTemplate::kNotTypedArray, TypedArrayForeachBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp43;
    compiler::TNode<Object> tmp44;
    ca_.Bind(&block2, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../src/builtins/typed-array-foreach.tq", 47);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp43}, MessageTemplate::kDetachedOperation, TypedArrayForeachBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }
}

}  // namespace internal
}  // namespace v8

