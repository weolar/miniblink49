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

const char* TypedArrayReduceBuiltinsFromDSLAssembler::kBuiltinName() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return "%TypedArray%.prototype.reduce";
}

compiler::TNode<Object> TypedArrayReduceBuiltinsFromDSLAssembler::ReduceAllElements(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_array, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_initialValue) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Smi, Context, Smi, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Object, Context, JSReceiver, Oddball, Object, Object, Smi, JSTypedArray> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, JSTypedArray, JSTypedArray, BuiltinPtr, Smi, Object> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, Object, Object> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_array, p_callbackfn, p_initialValue);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 13);
    compiler::TNode<JSTypedArray> tmp4;
    USE(tmp4);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    compiler::TNode<BuiltinPtr> tmp6;
    USE(tmp6);
    std::tie(tmp4, tmp5, tmp6) = TypedArrayBuiltinsFromDSLAssembler(state_).NewAttachedJSTypedArrayWitness(compiler::TNode<JSTypedArray>{tmp1}).Flatten();
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 88);
    ca_.Goto(&block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSTypedArray> tmp8;
    compiler::TNode<JSReceiver> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<JSTypedArray> tmp11;
    compiler::TNode<JSTypedArray> tmp12;
    compiler::TNode<BuiltinPtr> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 14);
    compiler::TNode<IntPtrT> tmp15 = ca_.IntPtrConstant(JSTypedArray::kLengthOffset);
    USE(tmp15);
    compiler::TNode<Smi>tmp16 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp14, tmp15});
    compiler::TNode<Smi> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi5ATSmi(compiler::TNode<Smi>{tmp16}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 15);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 16);
    compiler::TNode<Smi> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block5, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp17, tmp10, tmp18);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<JSTypedArray> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<JSTypedArray> tmp23;
    compiler::TNode<JSTypedArray> tmp24;
    compiler::TNode<BuiltinPtr> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<Smi> tmp28;
    ca_.Bind(&block5, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    compiler::TNode<BoolT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp28}, compiler::TNode<Smi>{tmp26}));
    ca_.Branch(tmp29, &block3, &block4, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<JSTypedArray> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<JSTypedArray> tmp34;
    compiler::TNode<JSTypedArray> tmp35;
    compiler::TNode<BuiltinPtr> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Smi> tmp39;
    ca_.Bind(&block3, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 96);
    compiler::TNode<IntPtrT> tmp40 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp40);
    compiler::TNode<JSArrayBuffer>tmp41 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp34, tmp40});
    compiler::TNode<BoolT> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp41}));
    ca_.Branch(tmp42, &block10, &block11, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp43;
    compiler::TNode<JSTypedArray> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<JSTypedArray> tmp47;
    compiler::TNode<JSTypedArray> tmp48;
    compiler::TNode<BuiltinPtr> tmp49;
    compiler::TNode<Smi> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Smi> tmp52;
    ca_.Bind(&block10, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.Goto(&block8, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp53;
    compiler::TNode<JSTypedArray> tmp54;
    compiler::TNode<JSReceiver> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<JSTypedArray> tmp57;
    compiler::TNode<JSTypedArray> tmp58;
    compiler::TNode<BuiltinPtr> tmp59;
    compiler::TNode<Smi> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Smi> tmp62;
    ca_.Bind(&block11, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 97);
    compiler::TNode<JSTypedArray> tmp63;
    USE(tmp63);
    tmp63 = (compiler::TNode<JSTypedArray>{tmp57});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 18);
    ca_.Goto(&block9, tmp53, tmp54, tmp55, tmp56, tmp57, tmp63, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<JSTypedArray> tmp65;
    compiler::TNode<JSReceiver> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<JSTypedArray> tmp68;
    compiler::TNode<JSTypedArray> tmp69;
    compiler::TNode<BuiltinPtr> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<Smi> tmp73;
    ca_.Bind(&block9, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    ca_.Goto(&block7, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<JSTypedArray> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<JSTypedArray> tmp78;
    compiler::TNode<JSTypedArray> tmp79;
    compiler::TNode<BuiltinPtr> tmp80;
    compiler::TNode<Smi> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Smi> tmp83;
    ca_.Bind(&block8, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83);
    ca_.Goto(&block4, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<JSTypedArray> tmp85;
    compiler::TNode<JSReceiver> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<JSTypedArray> tmp88;
    compiler::TNode<JSTypedArray> tmp89;
    compiler::TNode<BuiltinPtr> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Smi> tmp93;
    ca_.Bind(&block7, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 19);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 101);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 102);
    compiler::TNode<Object> tmp94 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(1)).descriptor(), tmp90, tmp84, tmp89, tmp93); 
    USE(tmp94);
    ca_.Goto(&block12, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp93, tmp84, tmp93, tmp94);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp95;
    compiler::TNode<JSTypedArray> tmp96;
    compiler::TNode<JSReceiver> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<JSTypedArray> tmp99;
    compiler::TNode<JSTypedArray> tmp100;
    compiler::TNode<BuiltinPtr> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Smi> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Context> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<Object> tmp108;
    ca_.Bind(&block12, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 19);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 20);
    compiler::TNode<Oddball> tmp109;
    USE(tmp109);
    tmp109 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp110;
    USE(tmp110);
    tmp110 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp103}, compiler::TNode<HeapObject>{tmp109}));
    ca_.Branch(tmp110, &block13, &block14, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp108);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp111;
    compiler::TNode<JSTypedArray> tmp112;
    compiler::TNode<JSReceiver> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<JSTypedArray> tmp115;
    compiler::TNode<JSTypedArray> tmp116;
    compiler::TNode<BuiltinPtr> tmp117;
    compiler::TNode<Smi> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<Smi> tmp120;
    compiler::TNode<Object> tmp121;
    ca_.Bind(&block13, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 21);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 20);
    ca_.Goto(&block15, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp121, tmp120, tmp121);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp122;
    compiler::TNode<JSTypedArray> tmp123;
    compiler::TNode<JSReceiver> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<JSTypedArray> tmp126;
    compiler::TNode<JSTypedArray> tmp127;
    compiler::TNode<BuiltinPtr> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<Smi> tmp131;
    compiler::TNode<Object> tmp132;
    ca_.Bind(&block14, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 24);
    compiler::TNode<Oddball> tmp133;
    USE(tmp133);
    tmp133 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 92);
    ca_.Goto(&block16, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp122, tmp124, tmp133, tmp130, tmp132, tmp131, tmp126);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp134;
    compiler::TNode<JSTypedArray> tmp135;
    compiler::TNode<JSReceiver> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<JSTypedArray> tmp138;
    compiler::TNode<JSTypedArray> tmp139;
    compiler::TNode<BuiltinPtr> tmp140;
    compiler::TNode<Smi> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<Smi> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<Context> tmp145;
    compiler::TNode<JSReceiver> tmp146;
    compiler::TNode<Oddball> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<Object> tmp149;
    compiler::TNode<Smi> tmp150;
    compiler::TNode<JSTypedArray> tmp151;
    ca_.Bind(&block16, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 23);
    compiler::TNode<Object> tmp152;
    USE(tmp152);
    tmp152 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp145}, compiler::TNode<JSReceiver>{tmp146}, compiler::TNode<Object>{tmp147}, compiler::TNode<Object>{tmp148}, compiler::TNode<Object>{tmp149}, compiler::TNode<Object>{tmp150}, compiler::TNode<Object>{tmp151}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 20);
    ca_.Goto(&block15, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp152, tmp143, tmp144);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp153;
    compiler::TNode<JSTypedArray> tmp154;
    compiler::TNode<JSReceiver> tmp155;
    compiler::TNode<Object> tmp156;
    compiler::TNode<JSTypedArray> tmp157;
    compiler::TNode<JSTypedArray> tmp158;
    compiler::TNode<BuiltinPtr> tmp159;
    compiler::TNode<Smi> tmp160;
    compiler::TNode<Object> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<Object> tmp163;
    ca_.Bind(&block15, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 16);
    ca_.Goto(&block6, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp164;
    compiler::TNode<JSTypedArray> tmp165;
    compiler::TNode<JSReceiver> tmp166;
    compiler::TNode<Object> tmp167;
    compiler::TNode<JSTypedArray> tmp168;
    compiler::TNode<JSTypedArray> tmp169;
    compiler::TNode<BuiltinPtr> tmp170;
    compiler::TNode<Smi> tmp171;
    compiler::TNode<Object> tmp172;
    compiler::TNode<Smi> tmp173;
    ca_.Bind(&block6, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173);
    compiler::TNode<Smi> tmp174;
    USE(tmp174);
    tmp174 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp175;
    USE(tmp175);
    tmp175 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp173}, compiler::TNode<Smi>{tmp174}));
    ca_.Goto(&block5, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp175);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp176;
    compiler::TNode<JSTypedArray> tmp177;
    compiler::TNode<JSReceiver> tmp178;
    compiler::TNode<Object> tmp179;
    compiler::TNode<JSTypedArray> tmp180;
    compiler::TNode<JSTypedArray> tmp181;
    compiler::TNode<BuiltinPtr> tmp182;
    compiler::TNode<Smi> tmp183;
    compiler::TNode<Object> tmp184;
    compiler::TNode<Smi> tmp185;
    ca_.Bind(&block4, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 28);
    compiler::TNode<Oddball> tmp186;
    USE(tmp186);
    tmp186 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp187;
    USE(tmp187);
    tmp187 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp184}, compiler::TNode<HeapObject>{tmp186}));
    ca_.Branch(tmp187, &block17, &block18, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp188;
    compiler::TNode<JSTypedArray> tmp189;
    compiler::TNode<JSReceiver> tmp190;
    compiler::TNode<Object> tmp191;
    compiler::TNode<JSTypedArray> tmp192;
    compiler::TNode<JSTypedArray> tmp193;
    compiler::TNode<BuiltinPtr> tmp194;
    compiler::TNode<Smi> tmp195;
    compiler::TNode<Object> tmp196;
    ca_.Bind(&block17, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 29);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp188}, MessageTemplate::kReduceNoInitial, TypedArrayReduceBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp197;
    compiler::TNode<JSTypedArray> tmp198;
    compiler::TNode<JSReceiver> tmp199;
    compiler::TNode<Object> tmp200;
    compiler::TNode<JSTypedArray> tmp201;
    compiler::TNode<JSTypedArray> tmp202;
    compiler::TNode<BuiltinPtr> tmp203;
    compiler::TNode<Smi> tmp204;
    compiler::TNode<Object> tmp205;
    ca_.Bind(&block18, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 31);
    ca_.Goto(&block1, tmp197, tmp198, tmp199, tmp200, tmp205);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp206;
    compiler::TNode<JSTypedArray> tmp207;
    compiler::TNode<JSReceiver> tmp208;
    compiler::TNode<Object> tmp209;
    compiler::TNode<Object> tmp210;
    ca_.Bind(&block1, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 10);
    ca_.Goto(&block19, tmp206, tmp207, tmp208, tmp209, tmp210);
  }

    compiler::TNode<Context> tmp211;
    compiler::TNode<JSTypedArray> tmp212;
    compiler::TNode<JSReceiver> tmp213;
    compiler::TNode<Object> tmp214;
    compiler::TNode<Object> tmp215;
    ca_.Bind(&block19, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215);
  return compiler::TNode<Object>{tmp215};
}

TF_BUILTIN(TypedArrayPrototypeReduce, CodeStubAssembler) {
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 41);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 43);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 45);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 46);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 47);
    compiler::TNode<Object> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<Object>(TypedArrayReduceBuiltinsFromDSLAssembler(state_).ReduceAllElements(compiler::TNode<Context>{tmp85}, compiler::TNode<JSTypedArray>{tmp91}, compiler::TNode<JSReceiver>{tmp92}, compiler::TNode<Object>{tmp93}));
    arguments.PopAndReturn(tmp94);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<RawPtrT> tmp97;
    compiler::TNode<RawPtrT> tmp98;
    compiler::TNode<IntPtrT> tmp99;
    ca_.Bind(&block6, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 50);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 53);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp102}, MessageTemplate::kNotTypedArray, TypedArrayReduceBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<RawPtrT> tmp109;
    compiler::TNode<RawPtrT> tmp110;
    compiler::TNode<IntPtrT> tmp111;
    ca_.Bind(&block2, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-reduce.tq", 56);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp107}, MessageTemplate::kDetachedOperation, TypedArrayReduceBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }
}

}  // namespace internal
}  // namespace v8

