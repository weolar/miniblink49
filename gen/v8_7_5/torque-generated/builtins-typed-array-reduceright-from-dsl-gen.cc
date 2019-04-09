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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 13);
    compiler::TNode<JSTypedArray> tmp4;
    USE(tmp4);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    compiler::TNode<BuiltinPtr> tmp6;
    USE(tmp6);
    std::tie(tmp4, tmp5, tmp6) = TypedArrayBuiltinsFromDSLAssembler(state_).NewAttachedJSTypedArrayWitness(compiler::TNode<JSTypedArray>{tmp1}).Flatten();
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 14);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadJSTypedArrayLength(compiler::TNode<JSTypedArray>{tmp1}));
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi5ATSmi(compiler::TNode<Smi>{tmp7}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 15);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 16);
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp8}, compiler::TNode<Smi>{tmp9}));
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp8, tmp3, tmp10);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSTypedArray> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<JSTypedArray> tmp15;
    compiler::TNode<JSTypedArray> tmp16;
    compiler::TNode<BuiltinPtr> tmp17;
    compiler::TNode<Smi> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Smi> tmp20;
    ca_.Bind(&block4, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    compiler::TNode<Smi> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThanOrEqual(compiler::TNode<Smi>{tmp20}, compiler::TNode<Smi>{tmp21}));
    ca_.Branch(tmp22, &block2, &block3, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSTypedArray> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<JSTypedArray> tmp27;
    compiler::TNode<JSTypedArray> tmp28;
    compiler::TNode<BuiltinPtr> tmp29;
    compiler::TNode<Smi> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Smi> tmp32;
    ca_.Bind(&block2, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 96);
    compiler::TNode<JSArrayBuffer> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp27}));
    compiler::TNode<BoolT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp33}));
    ca_.Branch(tmp34, &block9, &block10, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<JSTypedArray> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSTypedArray> tmp39;
    compiler::TNode<JSTypedArray> tmp40;
    compiler::TNode<BuiltinPtr> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Smi> tmp44;
    ca_.Bind(&block9, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.Goto(&block7, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<JSTypedArray> tmp46;
    compiler::TNode<JSReceiver> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<JSTypedArray> tmp49;
    compiler::TNode<JSTypedArray> tmp50;
    compiler::TNode<BuiltinPtr> tmp51;
    compiler::TNode<Smi> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<Smi> tmp54;
    ca_.Bind(&block10, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 97);
    compiler::TNode<JSTypedArray> tmp55;
    USE(tmp55);
    tmp55 = (compiler::TNode<JSTypedArray>{tmp49});
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 18);
    ca_.Goto(&block8, tmp45, tmp46, tmp47, tmp48, tmp49, tmp55, tmp51, tmp52, tmp53, tmp54);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<JSTypedArray> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<JSTypedArray> tmp60;
    compiler::TNode<JSTypedArray> tmp61;
    compiler::TNode<BuiltinPtr> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<Smi> tmp65;
    ca_.Bind(&block8, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65);
    ca_.Goto(&block6, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<JSTypedArray> tmp67;
    compiler::TNode<JSReceiver> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<JSTypedArray> tmp70;
    compiler::TNode<JSTypedArray> tmp71;
    compiler::TNode<BuiltinPtr> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<Smi> tmp75;
    ca_.Bind(&block7, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    ca_.Goto(&block3, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp76;
    compiler::TNode<JSTypedArray> tmp77;
    compiler::TNode<JSReceiver> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<JSTypedArray> tmp80;
    compiler::TNode<JSTypedArray> tmp81;
    compiler::TNode<BuiltinPtr> tmp82;
    compiler::TNode<Smi> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<Smi> tmp85;
    ca_.Bind(&block6, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 19);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 101);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 102);
    compiler::TNode<Object> tmp86 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(1)).descriptor(), tmp82, tmp76, tmp81, tmp85); 
    USE(tmp86);
    ca_.Goto(&block11, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp85, tmp76, tmp85, tmp86);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<JSTypedArray> tmp88;
    compiler::TNode<JSReceiver> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<JSTypedArray> tmp91;
    compiler::TNode<JSTypedArray> tmp92;
    compiler::TNode<BuiltinPtr> tmp93;
    compiler::TNode<Smi> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Smi> tmp96;
    compiler::TNode<Smi> tmp97;
    compiler::TNode<Context> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<Object> tmp100;
    ca_.Bind(&block11, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 19);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 20);
    compiler::TNode<Oddball> tmp101;
    USE(tmp101);
    tmp101 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp102;
    USE(tmp102);
    tmp102 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp95}, compiler::TNode<HeapObject>{tmp101}));
    ca_.Branch(tmp102, &block12, &block13, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp100);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp103;
    compiler::TNode<JSTypedArray> tmp104;
    compiler::TNode<JSReceiver> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<JSTypedArray> tmp107;
    compiler::TNode<JSTypedArray> tmp108;
    compiler::TNode<BuiltinPtr> tmp109;
    compiler::TNode<Smi> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<Object> tmp113;
    ca_.Bind(&block12, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 21);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 20);
    ca_.Goto(&block14, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp113, tmp112, tmp113);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp114;
    compiler::TNode<JSTypedArray> tmp115;
    compiler::TNode<JSReceiver> tmp116;
    compiler::TNode<Object> tmp117;
    compiler::TNode<JSTypedArray> tmp118;
    compiler::TNode<JSTypedArray> tmp119;
    compiler::TNode<BuiltinPtr> tmp120;
    compiler::TNode<Smi> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<Smi> tmp123;
    compiler::TNode<Object> tmp124;
    ca_.Bind(&block13, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 24);
    compiler::TNode<Oddball> tmp125;
    USE(tmp125);
    tmp125 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 92);
    ca_.Goto(&block15, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp114, tmp116, tmp125, tmp122, tmp124, tmp123, tmp118);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp126;
    compiler::TNode<JSTypedArray> tmp127;
    compiler::TNode<JSReceiver> tmp128;
    compiler::TNode<Object> tmp129;
    compiler::TNode<JSTypedArray> tmp130;
    compiler::TNode<JSTypedArray> tmp131;
    compiler::TNode<BuiltinPtr> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<Context> tmp137;
    compiler::TNode<JSReceiver> tmp138;
    compiler::TNode<Oddball> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<Object> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<JSTypedArray> tmp143;
    ca_.Bind(&block15, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 23);
    compiler::TNode<Object> tmp144;
    USE(tmp144);
    tmp144 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp137}, compiler::TNode<JSReceiver>{tmp138}, compiler::TNode<Object>{tmp139}, compiler::TNode<Object>{tmp140}, compiler::TNode<Object>{tmp141}, compiler::TNode<Object>{tmp142}, compiler::TNode<Object>{tmp143}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 20);
    ca_.Goto(&block14, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp144, tmp135, tmp136);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp145;
    compiler::TNode<JSTypedArray> tmp146;
    compiler::TNode<JSReceiver> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<JSTypedArray> tmp149;
    compiler::TNode<JSTypedArray> tmp150;
    compiler::TNode<BuiltinPtr> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<Smi> tmp154;
    compiler::TNode<Object> tmp155;
    ca_.Bind(&block14, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 16);
    ca_.Goto(&block5, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp156;
    compiler::TNode<JSTypedArray> tmp157;
    compiler::TNode<JSReceiver> tmp158;
    compiler::TNode<Object> tmp159;
    compiler::TNode<JSTypedArray> tmp160;
    compiler::TNode<JSTypedArray> tmp161;
    compiler::TNode<BuiltinPtr> tmp162;
    compiler::TNode<Smi> tmp163;
    compiler::TNode<Object> tmp164;
    compiler::TNode<Smi> tmp165;
    ca_.Bind(&block5, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165);
    compiler::TNode<Smi> tmp166;
    USE(tmp166);
    tmp166 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp167;
    USE(tmp167);
    tmp167 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp165}, compiler::TNode<Smi>{tmp166}));
    ca_.Goto(&block4, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp167);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp168;
    compiler::TNode<JSTypedArray> tmp169;
    compiler::TNode<JSReceiver> tmp170;
    compiler::TNode<Object> tmp171;
    compiler::TNode<JSTypedArray> tmp172;
    compiler::TNode<JSTypedArray> tmp173;
    compiler::TNode<BuiltinPtr> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<Object> tmp176;
    compiler::TNode<Smi> tmp177;
    ca_.Bind(&block3, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 28);
    compiler::TNode<Oddball> tmp178;
    USE(tmp178);
    tmp178 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp179;
    USE(tmp179);
    tmp179 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp176}, compiler::TNode<HeapObject>{tmp178}));
    ca_.Branch(tmp179, &block16, &block17, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp180;
    compiler::TNode<JSTypedArray> tmp181;
    compiler::TNode<JSReceiver> tmp182;
    compiler::TNode<Object> tmp183;
    compiler::TNode<JSTypedArray> tmp184;
    compiler::TNode<JSTypedArray> tmp185;
    compiler::TNode<BuiltinPtr> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<Object> tmp188;
    ca_.Bind(&block16, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 29);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp180}, MessageTemplate::kReduceNoInitial, TypedArrayReducerightBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp189;
    compiler::TNode<JSTypedArray> tmp190;
    compiler::TNode<JSReceiver> tmp191;
    compiler::TNode<Object> tmp192;
    compiler::TNode<JSTypedArray> tmp193;
    compiler::TNode<JSTypedArray> tmp194;
    compiler::TNode<BuiltinPtr> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<Object> tmp197;
    ca_.Bind(&block17, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 31);
    ca_.Goto(&block1, tmp189, tmp190, tmp191, tmp192, tmp197);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp198;
    compiler::TNode<JSTypedArray> tmp199;
    compiler::TNode<JSReceiver> tmp200;
    compiler::TNode<Object> tmp201;
    compiler::TNode<Object> tmp202;
    ca_.Bind(&block1, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 10);
    ca_.Goto(&block18, tmp198, tmp199, tmp200, tmp201, tmp202);
  }

    compiler::TNode<Context> tmp203;
    compiler::TNode<JSTypedArray> tmp204;
    compiler::TNode<JSReceiver> tmp205;
    compiler::TNode<Object> tmp206;
    compiler::TNode<Object> tmp207;
    ca_.Bind(&block18, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207);
  return compiler::TNode<Object>{tmp207};
}

TF_BUILTIN(TypedArrayPrototypeReduceRight, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSTypedArray, JSReceiver> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSTypedArray, JSReceiver> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSTypedArray, JSReceiver, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSTypedArray, JSReceiver, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block6(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 41);
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 43);
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 45);
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 46);
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(arguments));
    compiler::TNode<IntPtrT> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThanOrEqual(compiler::TNode<IntPtrT>{tmp34}, compiler::TNode<IntPtrT>{tmp35}));
    ca_.Branch(tmp36, &block13, &block14, tmp28, tmp29, tmp30, tmp31, tmp33);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSTypedArray> tmp39;
    compiler::TNode<JSTypedArray> tmp40;
    compiler::TNode<JSReceiver> tmp41;
    ca_.Bind(&block13, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    compiler::TNode<IntPtrT> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp42}));
    ca_.Goto(&block16, tmp37, tmp38, tmp39, tmp40, tmp41, tmp43);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<JSTypedArray> tmp46;
    compiler::TNode<JSTypedArray> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    ca_.Bind(&block14, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
    compiler::TNode<Oddball> tmp49;
    USE(tmp49);
    tmp49 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    ca_.Goto(&block15, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<JSTypedArray> tmp52;
    compiler::TNode<JSTypedArray> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block16, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block15, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<JSTypedArray> tmp58;
    compiler::TNode<JSTypedArray> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<Object> tmp61;
    ca_.Bind(&block15, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 48);
    compiler::TNode<Object> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<Object>(TypedArrayReducerightBuiltinsFromDSLAssembler(state_).ReduceRightAllElements(compiler::TNode<Context>{tmp56}, compiler::TNode<JSTypedArray>{tmp59}, compiler::TNode<JSReceiver>{tmp60}, compiler::TNode<Object>{tmp61}));
    arguments->PopAndReturn(tmp62);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<Object> tmp64;
    ca_.Bind(&block6, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 51);
    compiler::TNode<IntPtrT> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp65}));
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp63}, MessageTemplate::kCalledNonCallable, compiler::TNode<Object>{tmp66});
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<Object> tmp68;
    ca_.Bind(&block4, &tmp67, &tmp68);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 54);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp67}, MessageTemplate::kNotTypedArray, TypedArrayReducerightBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<Object> tmp70;
    ca_.Bind(&block2, &tmp69, &tmp70);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduceright.tq", 57);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp69}, MessageTemplate::kDetachedOperation, TypedArrayReducerightBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }
}

}  // namespace internal
}  // namespace v8

