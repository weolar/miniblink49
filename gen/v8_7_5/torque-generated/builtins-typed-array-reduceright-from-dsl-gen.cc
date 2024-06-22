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

const char* TypedArrayReducerightBuiltinsFromDSLAssembler::kBuiltinName() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return "%TypedArray%.prototype.reduceRight";
}

compiler::TNode<Object> TypedArrayReducerightBuiltinsFromDSLAssembler::ReduceRightAllElements(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_array, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_initialValue) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Smi, Context, Smi, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Object, Context, JSReceiver, Oddball, Object, Object, Smi, JSTypedArray> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_array, p_callbackfn, p_initialValue);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 13);
    compiler::TNode<JSTypedArray> tmp4;
    USE(tmp4);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    compiler::TNode<BuiltinPtr> tmp6;
    USE(tmp6);
    std::tie(tmp4, tmp5, tmp6) = TypedArrayBuiltinsFromDSLAssembler(state_).NewAttachedJSTypedArrayWitness(compiler::TNode<JSTypedArray>{tmp1}).Flatten();
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 14);
    compiler::TNode<IntPtrT> tmp7 = ca_.IntPtrConstant(JSTypedArray::kLengthOffset);
    USE(tmp7);
    compiler::TNode<Smi>tmp8 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp1, tmp7});
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi5ATSmi(compiler::TNode<Smi>{tmp8}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 15);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 16);
    compiler::TNode<Smi> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp9}, compiler::TNode<Smi>{tmp10}));
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp9, tmp3, tmp11);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<JSTypedArray> tmp13;
    compiler::TNode<JSReceiver> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<JSTypedArray> tmp16;
    compiler::TNode<JSTypedArray> tmp17;
    compiler::TNode<BuiltinPtr> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Smi> tmp21;
    ca_.Bind(&block4, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    compiler::TNode<Smi> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThanOrEqual(compiler::TNode<Smi>{tmp21}, compiler::TNode<Smi>{tmp22}));
    ca_.Branch(tmp23, &block2, &block3, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<JSTypedArray> tmp25;
    compiler::TNode<JSReceiver> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<JSTypedArray> tmp28;
    compiler::TNode<JSTypedArray> tmp29;
    compiler::TNode<BuiltinPtr> tmp30;
    compiler::TNode<Smi> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Smi> tmp33;
    ca_.Bind(&block2, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 96);
    compiler::TNode<IntPtrT> tmp34 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp34);
    compiler::TNode<JSArrayBuffer>tmp35 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp28, tmp34});
    compiler::TNode<BoolT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp35}));
    ca_.Branch(tmp36, &block9, &block10, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<JSTypedArray> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<JSTypedArray> tmp41;
    compiler::TNode<JSTypedArray> tmp42;
    compiler::TNode<BuiltinPtr> tmp43;
    compiler::TNode<Smi> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Smi> tmp46;
    ca_.Bind(&block9, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46);
    ca_.Goto(&block7, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<JSTypedArray> tmp48;
    compiler::TNode<JSReceiver> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<JSTypedArray> tmp51;
    compiler::TNode<JSTypedArray> tmp52;
    compiler::TNode<BuiltinPtr> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Smi> tmp56;
    ca_.Bind(&block10, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 97);
    compiler::TNode<JSTypedArray> tmp57;
    USE(tmp57);
    tmp57 = (compiler::TNode<JSTypedArray>{tmp51});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 18);
    ca_.Goto(&block8, tmp47, tmp48, tmp49, tmp50, tmp51, tmp57, tmp53, tmp54, tmp55, tmp56);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp58;
    compiler::TNode<JSTypedArray> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<JSTypedArray> tmp62;
    compiler::TNode<JSTypedArray> tmp63;
    compiler::TNode<BuiltinPtr> tmp64;
    compiler::TNode<Smi> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<Smi> tmp67;
    ca_.Bind(&block8, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.Goto(&block6, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<JSTypedArray> tmp69;
    compiler::TNode<JSReceiver> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<JSTypedArray> tmp72;
    compiler::TNode<JSTypedArray> tmp73;
    compiler::TNode<BuiltinPtr> tmp74;
    compiler::TNode<Smi> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<Smi> tmp77;
    ca_.Bind(&block7, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.Goto(&block3, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<JSTypedArray> tmp79;
    compiler::TNode<JSReceiver> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<JSTypedArray> tmp82;
    compiler::TNode<JSTypedArray> tmp83;
    compiler::TNode<BuiltinPtr> tmp84;
    compiler::TNode<Smi> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<Smi> tmp87;
    ca_.Bind(&block6, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 19);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 101);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 102);
    compiler::TNode<Object> tmp88 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(1)).descriptor(), tmp84, tmp78, tmp83, tmp87); 
    USE(tmp88);
    ca_.Goto(&block11, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp87, tmp78, tmp87, tmp88);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp89;
    compiler::TNode<JSTypedArray> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<JSTypedArray> tmp93;
    compiler::TNode<JSTypedArray> tmp94;
    compiler::TNode<BuiltinPtr> tmp95;
    compiler::TNode<Smi> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<Smi> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<Context> tmp100;
    compiler::TNode<Smi> tmp101;
    compiler::TNode<Object> tmp102;
    ca_.Bind(&block11, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 19);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 20);
    compiler::TNode<Oddball> tmp103;
    USE(tmp103);
    tmp103 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp104;
    USE(tmp104);
    tmp104 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp97}, compiler::TNode<HeapObject>{tmp103}));
    ca_.Branch(tmp104, &block12, &block13, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp102);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp105;
    compiler::TNode<JSTypedArray> tmp106;
    compiler::TNode<JSReceiver> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<JSTypedArray> tmp109;
    compiler::TNode<JSTypedArray> tmp110;
    compiler::TNode<BuiltinPtr> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<Smi> tmp114;
    compiler::TNode<Object> tmp115;
    ca_.Bind(&block12, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 21);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 20);
    ca_.Goto(&block14, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp115, tmp114, tmp115);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp116;
    compiler::TNode<JSTypedArray> tmp117;
    compiler::TNode<JSReceiver> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<JSTypedArray> tmp120;
    compiler::TNode<JSTypedArray> tmp121;
    compiler::TNode<BuiltinPtr> tmp122;
    compiler::TNode<Smi> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Smi> tmp125;
    compiler::TNode<Object> tmp126;
    ca_.Bind(&block13, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 24);
    compiler::TNode<Oddball> tmp127;
    USE(tmp127);
    tmp127 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 92);
    ca_.Goto(&block15, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp116, tmp118, tmp127, tmp124, tmp126, tmp125, tmp120);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp128;
    compiler::TNode<JSTypedArray> tmp129;
    compiler::TNode<JSReceiver> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<JSTypedArray> tmp132;
    compiler::TNode<JSTypedArray> tmp133;
    compiler::TNode<BuiltinPtr> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<Smi> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<Context> tmp139;
    compiler::TNode<JSReceiver> tmp140;
    compiler::TNode<Oddball> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<Smi> tmp144;
    compiler::TNode<JSTypedArray> tmp145;
    ca_.Bind(&block15, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 23);
    compiler::TNode<Object> tmp146;
    USE(tmp146);
    tmp146 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp139}, compiler::TNode<JSReceiver>{tmp140}, compiler::TNode<Object>{tmp141}, compiler::TNode<Object>{tmp142}, compiler::TNode<Object>{tmp143}, compiler::TNode<Object>{tmp144}, compiler::TNode<Object>{tmp145}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 20);
    ca_.Goto(&block14, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp146, tmp137, tmp138);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp147;
    compiler::TNode<JSTypedArray> tmp148;
    compiler::TNode<JSReceiver> tmp149;
    compiler::TNode<Object> tmp150;
    compiler::TNode<JSTypedArray> tmp151;
    compiler::TNode<JSTypedArray> tmp152;
    compiler::TNode<BuiltinPtr> tmp153;
    compiler::TNode<Smi> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<Smi> tmp156;
    compiler::TNode<Object> tmp157;
    ca_.Bind(&block14, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 16);
    ca_.Goto(&block5, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp158;
    compiler::TNode<JSTypedArray> tmp159;
    compiler::TNode<JSReceiver> tmp160;
    compiler::TNode<Object> tmp161;
    compiler::TNode<JSTypedArray> tmp162;
    compiler::TNode<JSTypedArray> tmp163;
    compiler::TNode<BuiltinPtr> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<Object> tmp166;
    compiler::TNode<Smi> tmp167;
    ca_.Bind(&block5, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167);
    compiler::TNode<Smi> tmp168;
    USE(tmp168);
    tmp168 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp169;
    USE(tmp169);
    tmp169 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp167}, compiler::TNode<Smi>{tmp168}));
    ca_.Goto(&block4, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp169);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<JSTypedArray> tmp171;
    compiler::TNode<JSReceiver> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<JSTypedArray> tmp174;
    compiler::TNode<JSTypedArray> tmp175;
    compiler::TNode<BuiltinPtr> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<Object> tmp178;
    compiler::TNode<Smi> tmp179;
    ca_.Bind(&block3, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 28);
    compiler::TNode<Oddball> tmp180;
    USE(tmp180);
    tmp180 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp181;
    USE(tmp181);
    tmp181 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp178}, compiler::TNode<HeapObject>{tmp180}));
    ca_.Branch(tmp181, &block16, &block17, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp182;
    compiler::TNode<JSTypedArray> tmp183;
    compiler::TNode<JSReceiver> tmp184;
    compiler::TNode<Object> tmp185;
    compiler::TNode<JSTypedArray> tmp186;
    compiler::TNode<JSTypedArray> tmp187;
    compiler::TNode<BuiltinPtr> tmp188;
    compiler::TNode<Smi> tmp189;
    compiler::TNode<Object> tmp190;
    ca_.Bind(&block16, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 29);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp182}, MessageTemplate::kReduceNoInitial, TypedArrayReducerightBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp191;
    compiler::TNode<JSTypedArray> tmp192;
    compiler::TNode<JSReceiver> tmp193;
    compiler::TNode<Object> tmp194;
    compiler::TNode<JSTypedArray> tmp195;
    compiler::TNode<JSTypedArray> tmp196;
    compiler::TNode<BuiltinPtr> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<Object> tmp199;
    ca_.Bind(&block17, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 31);
    ca_.Goto(&block1, tmp191, tmp192, tmp193, tmp194, tmp199);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp200;
    compiler::TNode<JSTypedArray> tmp201;
    compiler::TNode<JSReceiver> tmp202;
    compiler::TNode<Object> tmp203;
    compiler::TNode<Object> tmp204;
    ca_.Bind(&block1, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 10);
    ca_.Goto(&block18, tmp200, tmp201, tmp202, tmp203, tmp204);
  }

    compiler::TNode<Context> tmp205;
    compiler::TNode<JSTypedArray> tmp206;
    compiler::TNode<JSReceiver> tmp207;
    compiler::TNode<Object> tmp208;
    compiler::TNode<Object> tmp209;
    ca_.Bind(&block18, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209);
  return compiler::TNode<Object>{tmp209};
}

TF_BUILTIN(TypedArrayPrototypeReduceRight, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSTypedArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSTypedArray, JSTypedArray> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSTypedArray, JSTypedArray, JSTypedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSTypedArray, JSTypedArray, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSTypedArray, JSTypedArray, Object, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSTypedArray, JSTypedArray, JSReceiver> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSTypedArray, JSTypedArray, JSReceiver> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSTypedArray, JSTypedArray, JSReceiver, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSTypedArray, JSTypedArray, JSReceiver, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kDeferred);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 41);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp5 = BaseBuiltinsFromDSLAssembler(state_).Cast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block7, tmp0, tmp1, tmp2, tmp3, tmp4, tmp1, tmp5);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp0, tmp1, tmp2, tmp3, tmp4, tmp1);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<RawPtrT> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<Object> tmp11;
    ca_.Bind(&block8, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.Goto(&block4, tmp6, tmp7, tmp8, tmp9, tmp10);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<RawPtrT> tmp15;
    compiler::TNode<IntPtrT> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<JSTypedArray> tmp18;
    ca_.Bind(&block7, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 43);
    compiler::TNode<JSTypedArray> tmp19;
    USE(tmp19);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp19 = TypedArrayBuiltinsFromDSLAssembler(state_).EnsureAttached(compiler::TNode<JSTypedArray>{tmp18}, &label0);
    ca_.Goto(&block9, tmp12, tmp13, tmp14, tmp15, tmp16, tmp18, tmp18, tmp19);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp12, tmp13, tmp14, tmp15, tmp16, tmp18, tmp18);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<JSTypedArray> tmp25;
    compiler::TNode<JSTypedArray> tmp26;
    ca_.Bind(&block10, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.Goto(&block2, tmp20, tmp21, tmp22, tmp23, tmp24);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<RawPtrT> tmp30;
    compiler::TNode<IntPtrT> tmp31;
    compiler::TNode<JSTypedArray> tmp32;
    compiler::TNode<JSTypedArray> tmp33;
    compiler::TNode<JSTypedArray> tmp34;
    ca_.Bind(&block9, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 45);
    compiler::TNode<IntPtrT> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp29}, compiler::TNode<RawPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp31}}, compiler::TNode<IntPtrT>{tmp35}));
    compiler::TNode<JSReceiver> tmp37;
    USE(tmp37);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp37 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp27}, compiler::TNode<Object>{tmp36}, &label0);
    ca_.Goto(&block11, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp34, tmp36, tmp37);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp34, tmp36);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<RawPtrT> tmp40;
    compiler::TNode<RawPtrT> tmp41;
    compiler::TNode<IntPtrT> tmp42;
    compiler::TNode<JSTypedArray> tmp43;
    compiler::TNode<JSTypedArray> tmp44;
    compiler::TNode<Object> tmp45;
    ca_.Bind(&block12, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.Goto(&block6, tmp38, tmp39, tmp40, tmp41, tmp42);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<RawPtrT> tmp48;
    compiler::TNode<RawPtrT> tmp49;
    compiler::TNode<IntPtrT> tmp50;
    compiler::TNode<JSTypedArray> tmp51;
    compiler::TNode<JSTypedArray> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    ca_.Bind(&block11, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 46);
    compiler::TNode<IntPtrT> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThanOrEqual(compiler::TNode<IntPtrT>{tmp50}, compiler::TNode<IntPtrT>{tmp55}));
    ca_.Branch(tmp56, &block13, &block14, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp54);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<RawPtrT> tmp60;
    compiler::TNode<IntPtrT> tmp61;
    compiler::TNode<JSTypedArray> tmp62;
    compiler::TNode<JSTypedArray> tmp63;
    compiler::TNode<JSReceiver> tmp64;
    ca_.Bind(&block13, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64);
    compiler::TNode<IntPtrT> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp59}, compiler::TNode<RawPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp61}}, compiler::TNode<IntPtrT>{tmp65}));
    ca_.Goto(&block16, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp66);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<RawPtrT> tmp69;
    compiler::TNode<RawPtrT> tmp70;
    compiler::TNode<IntPtrT> tmp71;
    compiler::TNode<JSTypedArray> tmp72;
    compiler::TNode<JSTypedArray> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    ca_.Bind(&block14, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    compiler::TNode<Oddball> tmp75;
    USE(tmp75);
    tmp75 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    ca_.Goto(&block15, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<RawPtrT> tmp78;
    compiler::TNode<RawPtrT> tmp79;
    compiler::TNode<IntPtrT> tmp80;
    compiler::TNode<JSTypedArray> tmp81;
    compiler::TNode<JSTypedArray> tmp82;
    compiler::TNode<JSReceiver> tmp83;
    compiler::TNode<Object> tmp84;
    ca_.Bind(&block16, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84);
    ca_.Goto(&block15, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<RawPtrT> tmp87;
    compiler::TNode<RawPtrT> tmp88;
    compiler::TNode<IntPtrT> tmp89;
    compiler::TNode<JSTypedArray> tmp90;
    compiler::TNode<JSTypedArray> tmp91;
    compiler::TNode<JSReceiver> tmp92;
    compiler::TNode<Object> tmp93;
    ca_.Bind(&block15, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 48);
    compiler::TNode<Object> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<Object>(TypedArrayReducerightBuiltinsFromDSLAssembler(state_).ReduceRightAllElements(compiler::TNode<Context>{tmp85}, compiler::TNode<JSTypedArray>{tmp91}, compiler::TNode<JSReceiver>{tmp92}, compiler::TNode<Object>{tmp93}));
    arguments.PopAndReturn(tmp94);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<RawPtrT> tmp97;
    compiler::TNode<RawPtrT> tmp98;
    compiler::TNode<IntPtrT> tmp99;
    ca_.Bind(&block6, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 51);
    compiler::TNode<IntPtrT> tmp100;
    USE(tmp100);
    tmp100 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp101;
    USE(tmp101);
    tmp101 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp97}, compiler::TNode<RawPtrT>{tmp98}, compiler::TNode<IntPtrT>{tmp99}}, compiler::TNode<IntPtrT>{tmp100}));
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp95}, MessageTemplate::kCalledNonCallable, compiler::TNode<Object>{tmp101});
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<RawPtrT> tmp104;
    compiler::TNode<RawPtrT> tmp105;
    compiler::TNode<IntPtrT> tmp106;
    ca_.Bind(&block4, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 54);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp102}, MessageTemplate::kNotTypedArray, TypedArrayReducerightBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<RawPtrT> tmp109;
    compiler::TNode<RawPtrT> tmp110;
    compiler::TNode<IntPtrT> tmp111;
    ca_.Bind(&block2, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduceright.tq", 57);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp107}, MessageTemplate::kDetachedOperation, TypedArrayReducerightBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }
}

}  // namespace internal
}  // namespace v8

