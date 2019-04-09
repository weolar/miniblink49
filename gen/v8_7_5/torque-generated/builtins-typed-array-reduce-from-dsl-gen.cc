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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 13);
    compiler::TNode<JSTypedArray> tmp4;
    USE(tmp4);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    compiler::TNode<BuiltinPtr> tmp6;
    USE(tmp6);
    std::tie(tmp4, tmp5, tmp6) = TypedArrayBuiltinsFromDSLAssembler(state_).NewAttachedJSTypedArrayWitness(compiler::TNode<JSTypedArray>{tmp1}).Flatten();
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 88);
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 14);
    compiler::TNode<Smi> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadJSTypedArrayLength(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi5ATSmi(compiler::TNode<Smi>{tmp15}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 15);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 16);
    compiler::TNode<Smi> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block5, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp16, tmp10, tmp17);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<JSTypedArray> tmp19;
    compiler::TNode<JSReceiver> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<JSTypedArray> tmp22;
    compiler::TNode<JSTypedArray> tmp23;
    compiler::TNode<BuiltinPtr> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Smi> tmp27;
    ca_.Bind(&block5, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    compiler::TNode<BoolT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp25}));
    ca_.Branch(tmp28, &block3, &block4, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<JSTypedArray> tmp30;
    compiler::TNode<JSReceiver> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<JSTypedArray> tmp33;
    compiler::TNode<JSTypedArray> tmp34;
    compiler::TNode<BuiltinPtr> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Smi> tmp38;
    ca_.Bind(&block3, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 96);
    compiler::TNode<JSArrayBuffer> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp33}));
    compiler::TNode<BoolT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp39}));
    ca_.Branch(tmp40, &block10, &block11, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<JSTypedArray> tmp42;
    compiler::TNode<JSReceiver> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<JSTypedArray> tmp45;
    compiler::TNode<JSTypedArray> tmp46;
    compiler::TNode<BuiltinPtr> tmp47;
    compiler::TNode<Smi> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<Smi> tmp50;
    ca_.Bind(&block10, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.Goto(&block8, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<JSTypedArray> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<JSTypedArray> tmp55;
    compiler::TNode<JSTypedArray> tmp56;
    compiler::TNode<BuiltinPtr> tmp57;
    compiler::TNode<Smi> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Smi> tmp60;
    ca_.Bind(&block11, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 97);
    compiler::TNode<JSTypedArray> tmp61;
    USE(tmp61);
    tmp61 = (compiler::TNode<JSTypedArray>{tmp55});
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 18);
    ca_.Goto(&block9, tmp51, tmp52, tmp53, tmp54, tmp55, tmp61, tmp57, tmp58, tmp59, tmp60);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp62;
    compiler::TNode<JSTypedArray> tmp63;
    compiler::TNode<JSReceiver> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<JSTypedArray> tmp66;
    compiler::TNode<JSTypedArray> tmp67;
    compiler::TNode<BuiltinPtr> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Smi> tmp71;
    ca_.Bind(&block9, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    ca_.Goto(&block7, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<JSTypedArray> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<JSTypedArray> tmp76;
    compiler::TNode<JSTypedArray> tmp77;
    compiler::TNode<BuiltinPtr> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Smi> tmp81;
    ca_.Bind(&block8, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81);
    ca_.Goto(&block4, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<JSTypedArray> tmp83;
    compiler::TNode<JSReceiver> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<JSTypedArray> tmp86;
    compiler::TNode<JSTypedArray> tmp87;
    compiler::TNode<BuiltinPtr> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<Smi> tmp91;
    ca_.Bind(&block7, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 19);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 101);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 102);
    compiler::TNode<Object> tmp92 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(1)).descriptor(), tmp88, tmp82, tmp87, tmp91); 
    USE(tmp92);
    ca_.Goto(&block12, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp91, tmp82, tmp91, tmp92);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp93;
    compiler::TNode<JSTypedArray> tmp94;
    compiler::TNode<JSReceiver> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<JSTypedArray> tmp97;
    compiler::TNode<JSTypedArray> tmp98;
    compiler::TNode<BuiltinPtr> tmp99;
    compiler::TNode<Smi> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<Smi> tmp103;
    compiler::TNode<Context> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Object> tmp106;
    ca_.Bind(&block12, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 19);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 20);
    compiler::TNode<Oddball> tmp107;
    USE(tmp107);
    tmp107 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp108;
    USE(tmp108);
    tmp108 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp101}, compiler::TNode<HeapObject>{tmp107}));
    ca_.Branch(tmp108, &block13, &block14, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp106);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp109;
    compiler::TNode<JSTypedArray> tmp110;
    compiler::TNode<JSReceiver> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<JSTypedArray> tmp113;
    compiler::TNode<JSTypedArray> tmp114;
    compiler::TNode<BuiltinPtr> tmp115;
    compiler::TNode<Smi> tmp116;
    compiler::TNode<Object> tmp117;
    compiler::TNode<Smi> tmp118;
    compiler::TNode<Object> tmp119;
    ca_.Bind(&block13, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 21);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 20);
    ca_.Goto(&block15, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp119, tmp118, tmp119);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp120;
    compiler::TNode<JSTypedArray> tmp121;
    compiler::TNode<JSReceiver> tmp122;
    compiler::TNode<Object> tmp123;
    compiler::TNode<JSTypedArray> tmp124;
    compiler::TNode<JSTypedArray> tmp125;
    compiler::TNode<BuiltinPtr> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Object> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<Object> tmp130;
    ca_.Bind(&block14, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 24);
    compiler::TNode<Oddball> tmp131;
    USE(tmp131);
    tmp131 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 92);
    ca_.Goto(&block16, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp120, tmp122, tmp131, tmp128, tmp130, tmp129, tmp124);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp132;
    compiler::TNode<JSTypedArray> tmp133;
    compiler::TNode<JSReceiver> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<JSTypedArray> tmp136;
    compiler::TNode<JSTypedArray> tmp137;
    compiler::TNode<BuiltinPtr> tmp138;
    compiler::TNode<Smi> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<Smi> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<Context> tmp143;
    compiler::TNode<JSReceiver> tmp144;
    compiler::TNode<Oddball> tmp145;
    compiler::TNode<Object> tmp146;
    compiler::TNode<Object> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<JSTypedArray> tmp149;
    ca_.Bind(&block16, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 23);
    compiler::TNode<Object> tmp150;
    USE(tmp150);
    tmp150 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp143}, compiler::TNode<JSReceiver>{tmp144}, compiler::TNode<Object>{tmp145}, compiler::TNode<Object>{tmp146}, compiler::TNode<Object>{tmp147}, compiler::TNode<Object>{tmp148}, compiler::TNode<Object>{tmp149}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 20);
    ca_.Goto(&block15, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp150, tmp141, tmp142);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp151;
    compiler::TNode<JSTypedArray> tmp152;
    compiler::TNode<JSReceiver> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<JSTypedArray> tmp155;
    compiler::TNode<JSTypedArray> tmp156;
    compiler::TNode<BuiltinPtr> tmp157;
    compiler::TNode<Smi> tmp158;
    compiler::TNode<Object> tmp159;
    compiler::TNode<Smi> tmp160;
    compiler::TNode<Object> tmp161;
    ca_.Bind(&block15, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 16);
    ca_.Goto(&block6, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp162;
    compiler::TNode<JSTypedArray> tmp163;
    compiler::TNode<JSReceiver> tmp164;
    compiler::TNode<Object> tmp165;
    compiler::TNode<JSTypedArray> tmp166;
    compiler::TNode<JSTypedArray> tmp167;
    compiler::TNode<BuiltinPtr> tmp168;
    compiler::TNode<Smi> tmp169;
    compiler::TNode<Object> tmp170;
    compiler::TNode<Smi> tmp171;
    ca_.Bind(&block6, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171);
    compiler::TNode<Smi> tmp172;
    USE(tmp172);
    tmp172 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp173;
    USE(tmp173);
    tmp173 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp171}, compiler::TNode<Smi>{tmp172}));
    ca_.Goto(&block5, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp173);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp174;
    compiler::TNode<JSTypedArray> tmp175;
    compiler::TNode<JSReceiver> tmp176;
    compiler::TNode<Object> tmp177;
    compiler::TNode<JSTypedArray> tmp178;
    compiler::TNode<JSTypedArray> tmp179;
    compiler::TNode<BuiltinPtr> tmp180;
    compiler::TNode<Smi> tmp181;
    compiler::TNode<Object> tmp182;
    compiler::TNode<Smi> tmp183;
    ca_.Bind(&block4, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 28);
    compiler::TNode<Oddball> tmp184;
    USE(tmp184);
    tmp184 = BaseBuiltinsFromDSLAssembler(state_).Hole();
    compiler::TNode<BoolT> tmp185;
    USE(tmp185);
    tmp185 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp182}, compiler::TNode<HeapObject>{tmp184}));
    ca_.Branch(tmp185, &block17, &block18, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp186;
    compiler::TNode<JSTypedArray> tmp187;
    compiler::TNode<JSReceiver> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<JSTypedArray> tmp190;
    compiler::TNode<JSTypedArray> tmp191;
    compiler::TNode<BuiltinPtr> tmp192;
    compiler::TNode<Smi> tmp193;
    compiler::TNode<Object> tmp194;
    ca_.Bind(&block17, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 29);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp186}, MessageTemplate::kReduceNoInitial, TypedArrayReduceBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp195;
    compiler::TNode<JSTypedArray> tmp196;
    compiler::TNode<JSReceiver> tmp197;
    compiler::TNode<Object> tmp198;
    compiler::TNode<JSTypedArray> tmp199;
    compiler::TNode<JSTypedArray> tmp200;
    compiler::TNode<BuiltinPtr> tmp201;
    compiler::TNode<Smi> tmp202;
    compiler::TNode<Object> tmp203;
    ca_.Bind(&block18, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 31);
    ca_.Goto(&block1, tmp195, tmp196, tmp197, tmp198, tmp203);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp204;
    compiler::TNode<JSTypedArray> tmp205;
    compiler::TNode<JSReceiver> tmp206;
    compiler::TNode<Object> tmp207;
    compiler::TNode<Object> tmp208;
    ca_.Bind(&block1, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 10);
    ca_.Goto(&block19, tmp204, tmp205, tmp206, tmp207, tmp208);
  }

    compiler::TNode<Context> tmp209;
    compiler::TNode<JSTypedArray> tmp210;
    compiler::TNode<JSReceiver> tmp211;
    compiler::TNode<Object> tmp212;
    compiler::TNode<Object> tmp213;
    ca_.Bind(&block19, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213);
  return compiler::TNode<Object>{tmp213};
}

TF_BUILTIN(TypedArrayPrototypeReduce, CodeStubAssembler) {
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 41);
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 43);
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 45);
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 46);
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 47);
    compiler::TNode<Object> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<Object>(TypedArrayReduceBuiltinsFromDSLAssembler(state_).ReduceAllElements(compiler::TNode<Context>{tmp56}, compiler::TNode<JSTypedArray>{tmp59}, compiler::TNode<JSReceiver>{tmp60}, compiler::TNode<Object>{tmp61}));
    arguments->PopAndReturn(tmp62);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<Object> tmp64;
    ca_.Bind(&block6, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 50);
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
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 53);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp67}, MessageTemplate::kNotTypedArray, TypedArrayReduceBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<Object> tmp70;
    ca_.Bind(&block2, &tmp69, &tmp70);
    ca_.SetSourcePosition("../../src/builtins/typed-array-reduce.tq", 56);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp69}, MessageTemplate::kDetachedOperation, TypedArrayReduceBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }
}

}  // namespace internal
}  // namespace v8

