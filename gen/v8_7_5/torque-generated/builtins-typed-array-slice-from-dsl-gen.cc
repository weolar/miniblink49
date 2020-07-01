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

const char* TypedArraySliceBuiltinsFromDSLAssembler::kBuiltinName() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return "%TypedArray%.prototype.slice";
}

void TypedArraySliceBuiltinsFromDSLAssembler::FastCopy(compiler::TNode<JSTypedArray> p_src, compiler::TNode<JSTypedArray> p_dest, compiler::TNode<IntPtrT> p_k, compiler::TNode<Smi> p_count, compiler::CodeAssemblerLabel* label_IfSlow) {
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, UintPtrT> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, RawPtrT> block20(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, RawPtrT> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, RawPtrT> block22(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi, Int32T, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, RawPtrT> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSTypedArray, JSTypedArray, IntPtrT, Smi> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_src, p_dest, p_k, p_count);

  if (block0.is_used()) {
    compiler::TNode<JSTypedArray> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<IntPtrT> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 16);
    compiler::CodeAssemblerLabel label0(&ca_);
    CodeStubAssembler(state_).GotoIfForceSlowPath(&label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<JSTypedArray> tmp4;
    compiler::TNode<JSTypedArray> tmp5;
    compiler::TNode<IntPtrT> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block4, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.Goto(&block1);
  }

  if (block3.is_used()) {
    compiler::TNode<JSTypedArray> tmp8;
    compiler::TNode<JSTypedArray> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block3, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 18);
    compiler::TNode<Int32T> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadElementsKind(compiler::TNode<JSTypedArray>{tmp8}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 19);
    compiler::TNode<UintPtrT> tmp13;
    USE(tmp13);
    compiler::TNode<Map> tmp14;
    USE(tmp14);
    compiler::TNode<Int32T> tmp15;
    USE(tmp15);
    std::tie(tmp13, tmp14, tmp15) = TypedArrayBuiltinsAssembler(state_).GetTypedArrayElementsInfo(compiler::TNode<JSTypedArray>{tmp9}).Flatten();
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 25);
    compiler::TNode<BoolT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).ElementsKindNotEqual(compiler::TNode<Int32T>{tmp12}, compiler::TNode<Int32T>{tmp15}));
    ca_.Branch(tmp16, &block5, &block6, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15);
  }

  if (block5.is_used()) {
    compiler::TNode<JSTypedArray> tmp17;
    compiler::TNode<JSTypedArray> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<Int32T> tmp21;
    compiler::TNode<UintPtrT> tmp22;
    compiler::TNode<Map> tmp23;
    compiler::TNode<Int32T> tmp24;
    ca_.Bind(&block5, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block1);
  }

  if (block6.is_used()) {
    compiler::TNode<JSTypedArray> tmp25;
    compiler::TNode<JSTypedArray> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    compiler::TNode<Smi> tmp28;
    compiler::TNode<Int32T> tmp29;
    compiler::TNode<UintPtrT> tmp30;
    compiler::TNode<Map> tmp31;
    compiler::TNode<Int32T> tmp32;
    ca_.Bind(&block6, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 26);
    compiler::TNode<JSArrayBuffer> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp26}));
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).BitcastTaggedToWord(compiler::TNode<Object>{tmp33}));
    compiler::TNode<JSArrayBuffer> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<JSArrayBuffer>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewBuffer(compiler::TNode<JSArrayBufferView>{tmp25}));
    compiler::TNode<IntPtrT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).BitcastTaggedToWord(compiler::TNode<Object>{tmp35}));
    compiler::TNode<BoolT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp34}, compiler::TNode<IntPtrT>{tmp36}));
    ca_.Branch(tmp37, &block7, &block8, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32);
  }

  if (block7.is_used()) {
    compiler::TNode<JSTypedArray> tmp38;
    compiler::TNode<JSTypedArray> tmp39;
    compiler::TNode<IntPtrT> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<Int32T> tmp42;
    compiler::TNode<UintPtrT> tmp43;
    compiler::TNode<Map> tmp44;
    compiler::TNode<Int32T> tmp45;
    ca_.Bind(&block7, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 27);
    ca_.Goto(&block1);
  }

  if (block8.is_used()) {
    compiler::TNode<JSTypedArray> tmp46;
    compiler::TNode<JSTypedArray> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<Smi> tmp49;
    compiler::TNode<Int32T> tmp50;
    compiler::TNode<UintPtrT> tmp51;
    compiler::TNode<Map> tmp52;
    compiler::TNode<Int32T> tmp53;
    ca_.Bind(&block8, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 31);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 11);
    compiler::TNode<UintPtrT> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr13ATPositiveSmi(compiler::TNode<Smi>{tmp49}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 12);
    compiler::TNode<UintPtrT> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShl(compiler::TNode<UintPtrT>{tmp54}, compiler::TNode<UintPtrT>{tmp51}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 15);
    compiler::TNode<UintPtrT> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShr(compiler::TNode<UintPtrT>{tmp55}, compiler::TNode<UintPtrT>{tmp51}));
    compiler::TNode<BoolT> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<UintPtrT>{tmp56}, compiler::TNode<UintPtrT>{tmp54}));
    ca_.Branch(tmp57, &block12, &block13, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp51, tmp52, tmp53, tmp49, tmp49, tmp54, tmp55);
  }

  if (block12.is_used()) {
    compiler::TNode<JSTypedArray> tmp58;
    compiler::TNode<JSTypedArray> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Smi> tmp61;
    compiler::TNode<Int32T> tmp62;
    compiler::TNode<UintPtrT> tmp63;
    compiler::TNode<Map> tmp64;
    compiler::TNode<Int32T> tmp65;
    compiler::TNode<UintPtrT> tmp66;
    compiler::TNode<Map> tmp67;
    compiler::TNode<Int32T> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<UintPtrT> tmp71;
    compiler::TNode<UintPtrT> tmp72;
    ca_.Bind(&block12, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.Goto(&block10, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65);
  }

  if (block13.is_used()) {
    compiler::TNode<JSTypedArray> tmp73;
    compiler::TNode<JSTypedArray> tmp74;
    compiler::TNode<IntPtrT> tmp75;
    compiler::TNode<Smi> tmp76;
    compiler::TNode<Int32T> tmp77;
    compiler::TNode<UintPtrT> tmp78;
    compiler::TNode<Map> tmp79;
    compiler::TNode<Int32T> tmp80;
    compiler::TNode<UintPtrT> tmp81;
    compiler::TNode<Map> tmp82;
    compiler::TNode<Int32T> tmp83;
    compiler::TNode<Smi> tmp84;
    compiler::TNode<Smi> tmp85;
    compiler::TNode<UintPtrT> tmp86;
    compiler::TNode<UintPtrT> tmp87;
    ca_.Bind(&block13, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 16);
    ca_.Goto(&block11, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp87);
  }

  if (block11.is_used()) {
    compiler::TNode<JSTypedArray> tmp88;
    compiler::TNode<JSTypedArray> tmp89;
    compiler::TNode<IntPtrT> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<Int32T> tmp92;
    compiler::TNode<UintPtrT> tmp93;
    compiler::TNode<Map> tmp94;
    compiler::TNode<Int32T> tmp95;
    compiler::TNode<UintPtrT> tmp96;
    compiler::TNode<Map> tmp97;
    compiler::TNode<Int32T> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<Smi> tmp100;
    compiler::TNode<UintPtrT> tmp101;
    ca_.Bind(&block11, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 31);
    ca_.Goto(&block9, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp101);
  }

  if (block10.is_used()) {
    compiler::TNode<JSTypedArray> tmp102;
    compiler::TNode<JSTypedArray> tmp103;
    compiler::TNode<IntPtrT> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Int32T> tmp106;
    compiler::TNode<UintPtrT> tmp107;
    compiler::TNode<Map> tmp108;
    compiler::TNode<Int32T> tmp109;
    ca_.Bind(&block10, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/typed-array-slice.tq:31:55");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block9.is_used()) {
    compiler::TNode<JSTypedArray> tmp110;
    compiler::TNode<JSTypedArray> tmp111;
    compiler::TNode<IntPtrT> tmp112;
    compiler::TNode<Smi> tmp113;
    compiler::TNode<Int32T> tmp114;
    compiler::TNode<UintPtrT> tmp115;
    compiler::TNode<Map> tmp116;
    compiler::TNode<Int32T> tmp117;
    compiler::TNode<UintPtrT> tmp118;
    ca_.Bind(&block9, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 30);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 33);
    compiler::TNode<Smi> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert13ATPositiveSmi8ATintptr(compiler::TNode<IntPtrT>{tmp112}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 11);
    compiler::TNode<UintPtrT> tmp120;
    USE(tmp120);
    tmp120 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr13ATPositiveSmi(compiler::TNode<Smi>{tmp119}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 12);
    compiler::TNode<UintPtrT> tmp121;
    USE(tmp121);
    tmp121 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShl(compiler::TNode<UintPtrT>{tmp120}, compiler::TNode<UintPtrT>{tmp115}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 15);
    compiler::TNode<UintPtrT> tmp122;
    USE(tmp122);
    tmp122 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShr(compiler::TNode<UintPtrT>{tmp121}, compiler::TNode<UintPtrT>{tmp115}));
    compiler::TNode<BoolT> tmp123;
    USE(tmp123);
    tmp123 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<UintPtrT>{tmp122}, compiler::TNode<UintPtrT>{tmp120}));
    ca_.Branch(tmp123, &block17, &block18, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp115, tmp116, tmp117, tmp119, tmp119, tmp120, tmp121);
  }

  if (block17.is_used()) {
    compiler::TNode<JSTypedArray> tmp124;
    compiler::TNode<JSTypedArray> tmp125;
    compiler::TNode<IntPtrT> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Int32T> tmp128;
    compiler::TNode<UintPtrT> tmp129;
    compiler::TNode<Map> tmp130;
    compiler::TNode<Int32T> tmp131;
    compiler::TNode<UintPtrT> tmp132;
    compiler::TNode<UintPtrT> tmp133;
    compiler::TNode<Map> tmp134;
    compiler::TNode<Int32T> tmp135;
    compiler::TNode<Smi> tmp136;
    compiler::TNode<Smi> tmp137;
    compiler::TNode<UintPtrT> tmp138;
    compiler::TNode<UintPtrT> tmp139;
    ca_.Bind(&block17, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139);
    ca_.Goto(&block15, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132);
  }

  if (block18.is_used()) {
    compiler::TNode<JSTypedArray> tmp140;
    compiler::TNode<JSTypedArray> tmp141;
    compiler::TNode<IntPtrT> tmp142;
    compiler::TNode<Smi> tmp143;
    compiler::TNode<Int32T> tmp144;
    compiler::TNode<UintPtrT> tmp145;
    compiler::TNode<Map> tmp146;
    compiler::TNode<Int32T> tmp147;
    compiler::TNode<UintPtrT> tmp148;
    compiler::TNode<UintPtrT> tmp149;
    compiler::TNode<Map> tmp150;
    compiler::TNode<Int32T> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<UintPtrT> tmp154;
    compiler::TNode<UintPtrT> tmp155;
    ca_.Bind(&block18, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 16);
    ca_.Goto(&block16, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp155);
  }

  if (block16.is_used()) {
    compiler::TNode<JSTypedArray> tmp156;
    compiler::TNode<JSTypedArray> tmp157;
    compiler::TNode<IntPtrT> tmp158;
    compiler::TNode<Smi> tmp159;
    compiler::TNode<Int32T> tmp160;
    compiler::TNode<UintPtrT> tmp161;
    compiler::TNode<Map> tmp162;
    compiler::TNode<Int32T> tmp163;
    compiler::TNode<UintPtrT> tmp164;
    compiler::TNode<UintPtrT> tmp165;
    compiler::TNode<Map> tmp166;
    compiler::TNode<Int32T> tmp167;
    compiler::TNode<Smi> tmp168;
    compiler::TNode<Smi> tmp169;
    compiler::TNode<UintPtrT> tmp170;
    ca_.Bind(&block16, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 33);
    ca_.Goto(&block14, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp170);
  }

  if (block15.is_used()) {
    compiler::TNode<JSTypedArray> tmp171;
    compiler::TNode<JSTypedArray> tmp172;
    compiler::TNode<IntPtrT> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<Int32T> tmp175;
    compiler::TNode<UintPtrT> tmp176;
    compiler::TNode<Map> tmp177;
    compiler::TNode<Int32T> tmp178;
    compiler::TNode<UintPtrT> tmp179;
    ca_.Bind(&block15, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 34);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/typed-array-slice.tq:34:19");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block14.is_used()) {
    compiler::TNode<JSTypedArray> tmp180;
    compiler::TNode<JSTypedArray> tmp181;
    compiler::TNode<IntPtrT> tmp182;
    compiler::TNode<Smi> tmp183;
    compiler::TNode<Int32T> tmp184;
    compiler::TNode<UintPtrT> tmp185;
    compiler::TNode<Map> tmp186;
    compiler::TNode<Int32T> tmp187;
    compiler::TNode<UintPtrT> tmp188;
    compiler::TNode<UintPtrT> tmp189;
    ca_.Bind(&block14, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 32);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 35);
    compiler::TNode<RawPtrT> tmp190;
    USE(tmp190);
    tmp190 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp180}));
    compiler::TNode<IntPtrT> tmp191;
    USE(tmp191);
    tmp191 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr9ATuintptr(compiler::TNode<UintPtrT>{tmp189}));
    compiler::TNode<RawPtrT> tmp192;
    USE(tmp192);
    tmp192 = ca_.UncheckedCast<RawPtrT>(CodeStubAssembler(state_).RawPtrAdd(compiler::TNode<RawPtrT>{tmp190}, compiler::TNode<IntPtrT>{tmp191}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 37);
    compiler::TNode<UintPtrT> tmp193;
    USE(tmp193);
    tmp193 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewByteLength(compiler::TNode<JSArrayBufferView>{tmp181}));
    compiler::TNode<BoolT> tmp194;
    USE(tmp194);
    tmp194 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrLessThanOrEqual(compiler::TNode<UintPtrT>{tmp188}, compiler::TNode<UintPtrT>{tmp193}));
    ca_.Branch(tmp194, &block19, &block20, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp192);
  }

  if (block20.is_used()) {
    compiler::TNode<JSTypedArray> tmp195;
    compiler::TNode<JSTypedArray> tmp196;
    compiler::TNode<IntPtrT> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<Int32T> tmp199;
    compiler::TNode<UintPtrT> tmp200;
    compiler::TNode<Map> tmp201;
    compiler::TNode<Int32T> tmp202;
    compiler::TNode<UintPtrT> tmp203;
    compiler::TNode<UintPtrT> tmp204;
    compiler::TNode<RawPtrT> tmp205;
    ca_.Bind(&block20, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205);
    CodeStubAssembler(state_).FailAssert("Torque assert \'countBytes <= dest.byte_length\' failed", "../../src/builtins/typed-array-slice.tq", 37);
  }

  if (block19.is_used()) {
    compiler::TNode<JSTypedArray> tmp206;
    compiler::TNode<JSTypedArray> tmp207;
    compiler::TNode<IntPtrT> tmp208;
    compiler::TNode<Smi> tmp209;
    compiler::TNode<Int32T> tmp210;
    compiler::TNode<UintPtrT> tmp211;
    compiler::TNode<Map> tmp212;
    compiler::TNode<Int32T> tmp213;
    compiler::TNode<UintPtrT> tmp214;
    compiler::TNode<UintPtrT> tmp215;
    compiler::TNode<RawPtrT> tmp216;
    ca_.Bind(&block19, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 38);
    compiler::TNode<UintPtrT> tmp217;
    USE(tmp217);
    tmp217 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewByteLength(compiler::TNode<JSArrayBufferView>{tmp206}));
    compiler::TNode<UintPtrT> tmp218;
    USE(tmp218);
    tmp218 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrSub(compiler::TNode<UintPtrT>{tmp217}, compiler::TNode<UintPtrT>{tmp215}));
    compiler::TNode<BoolT> tmp219;
    USE(tmp219);
    tmp219 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrLessThanOrEqual(compiler::TNode<UintPtrT>{tmp214}, compiler::TNode<UintPtrT>{tmp218}));
    ca_.Branch(tmp219, &block21, &block22, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216);
  }

  if (block22.is_used()) {
    compiler::TNode<JSTypedArray> tmp220;
    compiler::TNode<JSTypedArray> tmp221;
    compiler::TNode<IntPtrT> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<Int32T> tmp224;
    compiler::TNode<UintPtrT> tmp225;
    compiler::TNode<Map> tmp226;
    compiler::TNode<Int32T> tmp227;
    compiler::TNode<UintPtrT> tmp228;
    compiler::TNode<UintPtrT> tmp229;
    compiler::TNode<RawPtrT> tmp230;
    ca_.Bind(&block22, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230);
    CodeStubAssembler(state_).FailAssert("Torque assert \'countBytes <= src.byte_length - startOffset\' failed", "../../src/builtins/typed-array-slice.tq", 38);
  }

  if (block21.is_used()) {
    compiler::TNode<JSTypedArray> tmp231;
    compiler::TNode<JSTypedArray> tmp232;
    compiler::TNode<IntPtrT> tmp233;
    compiler::TNode<Smi> tmp234;
    compiler::TNode<Int32T> tmp235;
    compiler::TNode<UintPtrT> tmp236;
    compiler::TNode<Map> tmp237;
    compiler::TNode<Int32T> tmp238;
    compiler::TNode<UintPtrT> tmp239;
    compiler::TNode<UintPtrT> tmp240;
    compiler::TNode<RawPtrT> tmp241;
    ca_.Bind(&block21, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 40);
    compiler::TNode<RawPtrT> tmp242;
    USE(tmp242);
    tmp242 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp232}));
    TypedArrayBuiltinsAssembler(state_).CallCMemmove(compiler::TNode<RawPtrT>{tmp242}, compiler::TNode<RawPtrT>{tmp241}, compiler::TNode<UintPtrT>{tmp239});
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 15);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 13);
    ca_.Goto(&block2, tmp231, tmp232, tmp233, tmp234);
  }

  if (block2.is_used()) {
    compiler::TNode<JSTypedArray> tmp243;
    compiler::TNode<JSTypedArray> tmp244;
    compiler::TNode<IntPtrT> tmp245;
    compiler::TNode<Smi> tmp246;
    ca_.Bind(&block2, &tmp243, &tmp244, &tmp245, &tmp246);
    ca_.Goto(&block23, tmp243, tmp244, tmp245, tmp246);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_IfSlow);
  }

    compiler::TNode<JSTypedArray> tmp247;
    compiler::TNode<JSTypedArray> tmp248;
    compiler::TNode<IntPtrT> tmp249;
    compiler::TNode<Smi> tmp250;
    ca_.Bind(&block23, &tmp247, &tmp248, &tmp249, &tmp250);
}

void TypedArraySliceBuiltinsFromDSLAssembler::SlowCopy(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_src, compiler::TNode<JSTypedArray> p_dest, compiler::TNode<IntPtrT> p_k, compiler::TNode<IntPtrT> p_final) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, IntPtrT, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, IntPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, IntPtrT, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, IntPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, IntPtrT, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_src, p_dest, p_k, p_final);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSTypedArray> tmp2;
    compiler::TNode<IntPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 45);
    compiler::TNode<Int32T> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadElementsKind(compiler::TNode<JSTypedArray>{tmp1}));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(TypedArrayBuiltinsAssembler(state_).IsBigInt64ElementsKind(compiler::TNode<Int32T>{tmp5}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 46);
    compiler::TNode<Int32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadElementsKind(compiler::TNode<JSTypedArray>{tmp2}));
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(TypedArrayBuiltinsAssembler(state_).IsBigInt64ElementsKind(compiler::TNode<Int32T>{tmp7}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 45);
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32NotEqual(compiler::TNode<BoolT>{tmp6}, compiler::TNode<BoolT>{tmp8}));
    ca_.Branch(tmp9, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSTypedArray> tmp11;
    compiler::TNode<JSTypedArray> tmp12;
    compiler::TNode<IntPtrT> tmp13;
    compiler::TNode<IntPtrT> tmp14;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 48);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp10}, MessageTemplate::kBigIntMixedTypes);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<JSTypedArray> tmp16;
    compiler::TNode<JSTypedArray> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    ca_.Bind(&block3, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 51);
    TypedArrayBuiltinsAssembler(state_).CallCCopyTypedArrayElementsSlice(compiler::TNode<JSTypedArray>{tmp16}, compiler::TNode<JSTypedArray>{tmp17}, compiler::TNode<IntPtrT>{tmp18}, compiler::TNode<IntPtrT>{tmp19});
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 43);
    ca_.Goto(&block1, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<JSTypedArray> tmp21;
    compiler::TNode<JSTypedArray> tmp22;
    compiler::TNode<IntPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    ca_.Bind(&block1, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23, tmp24);
  }

    compiler::TNode<Context> tmp25;
    compiler::TNode<JSTypedArray> tmp26;
    compiler::TNode<JSTypedArray> tmp27;
    compiler::TNode<IntPtrT> tmp28;
    compiler::TNode<IntPtrT> tmp29;
    ca_.Bind(&block4, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
}

TF_BUILTIN(TypedArrayPrototypeSlice, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray, JSTypedArray> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray, JSTypedArray, JSTypedArray> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray, JSTypedArray, JSTypedArray, JSTypedArray, IntPtrT, Smi> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray, JSTypedArray, JSTypedArray, JSTypedArray, IntPtrT, Smi> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray> block14(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray> block12(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, JSTypedArray> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 63);
    compiler::TNode<JSTypedArray> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<JSTypedArray>(TypedArrayBuiltinsAssembler(state_).ValidateTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, TypedArraySliceBuiltinsFromDSLAssembler(state_).kBuiltinName()));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 62);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 66);
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadJSTypedArrayLength(compiler::TNode<JSTypedArray>{tmp2}));
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 71);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp5}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 73);
    compiler::TNode<Oddball> tmp7;
    USE(tmp7);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp6}, compiler::TNode<HeapObject>{tmp7}));
    ca_.Branch(tmp8, &block1, &block2, tmp0, tmp1, tmp2, tmp4, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<JSTypedArray> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    compiler::TNode<Object> tmp13;
    ca_.Bind(&block1, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    compiler::TNode<IntPtrT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Context>{tmp9}, compiler::TNode<Object>{tmp13}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<JSTypedArray> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    compiler::TNode<Object> tmp19;
    ca_.Bind(&block2, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    compiler::TNode<IntPtrT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.Goto(&block3, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<JSTypedArray> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    ca_.Bind(&block4, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.Goto(&block3, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<JSTypedArray> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<IntPtrT> tmp32;
    ca_.Bind(&block3, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 72);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 79);
    compiler::TNode<IntPtrT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp33}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 81);
    compiler::TNode<Oddball> tmp35;
    USE(tmp35);
    tmp35 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp34}, compiler::TNode<HeapObject>{tmp35}));
    ca_.Branch(tmp36, &block5, &block6, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp34);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSTypedArray> tmp39;
    compiler::TNode<IntPtrT> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<IntPtrT> tmp42;
    compiler::TNode<Object> tmp43;
    ca_.Bind(&block5, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    compiler::TNode<IntPtrT> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Context>{tmp37}, compiler::TNode<Object>{tmp43}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<JSTypedArray> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<IntPtrT> tmp50;
    compiler::TNode<Object> tmp51;
    ca_.Bind(&block6, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.Goto(&block7, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<JSTypedArray> tmp54;
    compiler::TNode<IntPtrT> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<IntPtrT> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<IntPtrT> tmp59;
    ca_.Bind(&block8, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.Goto(&block7, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<JSTypedArray> tmp62;
    compiler::TNode<IntPtrT> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<IntPtrT> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<IntPtrT> tmp67;
    ca_.Bind(&block7, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 80);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 84);
    compiler::TNode<IntPtrT> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrSub(compiler::TNode<IntPtrT>{tmp67}, compiler::TNode<IntPtrT>{tmp65}));
    compiler::TNode<IntPtrT> tmp69;
    USE(tmp69);
    tmp69 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<IntPtrT> tmp70;
    USE(tmp70);
    tmp70 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMax(compiler::TNode<IntPtrT>{tmp68}, compiler::TNode<IntPtrT>{tmp69}));
    compiler::TNode<Smi> tmp71;
    USE(tmp71);
    tmp71 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert13ATPositiveSmi8ATintptr(compiler::TNode<IntPtrT>{tmp70}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 89);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 88);
    compiler::TNode<JSTypedArray> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<JSTypedArray>(TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).TypedArraySpeciesCreateByLength(compiler::TNode<Context>{tmp60}, TypedArraySliceBuiltinsFromDSLAssembler(state_).kBuiltinName(), compiler::TNode<JSTypedArray>{tmp62}, compiler::TNode<Smi>{tmp71}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 87);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 91);
    compiler::TNode<Smi> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp71}, compiler::TNode<Smi>{tmp73}));
    ca_.Branch(tmp74, &block9, &block10, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp71, tmp72);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<JSTypedArray> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<IntPtrT> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    compiler::TNode<Smi> tmp83;
    compiler::TNode<JSTypedArray> tmp84;
    ca_.Bind(&block9, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 93);
    compiler::TNode<JSTypedArray> tmp85;
    USE(tmp85);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp85 = TypedArrayBuiltinsFromDSLAssembler(state_).EnsureAttached(compiler::TNode<JSTypedArray>{tmp77}, &label0);
    ca_.Goto(&block15, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp77, tmp85);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block16, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp77);
    }
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<JSTypedArray> tmp88;
    compiler::TNode<IntPtrT> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<IntPtrT> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<IntPtrT> tmp93;
    compiler::TNode<Smi> tmp94;
    compiler::TNode<JSTypedArray> tmp95;
    compiler::TNode<JSTypedArray> tmp96;
    ca_.Bind(&block16, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96);
    ca_.Goto(&block14, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<JSTypedArray> tmp99;
    compiler::TNode<IntPtrT> tmp100;
    compiler::TNode<Object> tmp101;
    compiler::TNode<IntPtrT> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<IntPtrT> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<JSTypedArray> tmp106;
    compiler::TNode<JSTypedArray> tmp107;
    compiler::TNode<JSTypedArray> tmp108;
    ca_.Bind(&block15, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 95);
    compiler::CodeAssemblerLabel label0(&ca_);
    TypedArraySliceBuiltinsFromDSLAssembler(state_).FastCopy(compiler::TNode<JSTypedArray>{tmp108}, compiler::TNode<JSTypedArray>{tmp106}, compiler::TNode<IntPtrT>{tmp102}, compiler::TNode<Smi>{tmp105}, &label0);
    ca_.Goto(&block17, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp108, tmp108, tmp106, tmp102, tmp105);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block18, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp108, tmp108, tmp106, tmp102, tmp105);
    }
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<JSTypedArray> tmp111;
    compiler::TNode<IntPtrT> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<IntPtrT> tmp114;
    compiler::TNode<Object> tmp115;
    compiler::TNode<IntPtrT> tmp116;
    compiler::TNode<Smi> tmp117;
    compiler::TNode<JSTypedArray> tmp118;
    compiler::TNode<JSTypedArray> tmp119;
    compiler::TNode<JSTypedArray> tmp120;
    compiler::TNode<JSTypedArray> tmp121;
    compiler::TNode<IntPtrT> tmp122;
    compiler::TNode<Smi> tmp123;
    ca_.Bind(&block18, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123);
    ca_.Goto(&block12, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<JSTypedArray> tmp126;
    compiler::TNode<IntPtrT> tmp127;
    compiler::TNode<Object> tmp128;
    compiler::TNode<IntPtrT> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<IntPtrT> tmp131;
    compiler::TNode<Smi> tmp132;
    compiler::TNode<JSTypedArray> tmp133;
    compiler::TNode<JSTypedArray> tmp134;
    compiler::TNode<JSTypedArray> tmp135;
    compiler::TNode<JSTypedArray> tmp136;
    compiler::TNode<IntPtrT> tmp137;
    compiler::TNode<Smi> tmp138;
    ca_.Bind(&block17, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 92);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 97);
    ca_.Goto(&block13, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<JSTypedArray> tmp141;
    compiler::TNode<IntPtrT> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<IntPtrT> tmp144;
    compiler::TNode<Object> tmp145;
    compiler::TNode<IntPtrT> tmp146;
    compiler::TNode<Smi> tmp147;
    compiler::TNode<JSTypedArray> tmp148;
    ca_.Bind(&block14, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 98);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp139}, MessageTemplate::kDetachedOperation, TypedArraySliceBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp149;
    compiler::TNode<Object> tmp150;
    compiler::TNode<JSTypedArray> tmp151;
    compiler::TNode<IntPtrT> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<IntPtrT> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<IntPtrT> tmp156;
    compiler::TNode<Smi> tmp157;
    compiler::TNode<JSTypedArray> tmp158;
    ca_.Bind(&block13, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 100);
    ca_.Goto(&block11, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp159;
    compiler::TNode<Object> tmp160;
    compiler::TNode<JSTypedArray> tmp161;
    compiler::TNode<IntPtrT> tmp162;
    compiler::TNode<Object> tmp163;
    compiler::TNode<IntPtrT> tmp164;
    compiler::TNode<Object> tmp165;
    compiler::TNode<IntPtrT> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<JSTypedArray> tmp168;
    ca_.Bind(&block12, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 101);
    TypedArraySliceBuiltinsFromDSLAssembler(state_).SlowCopy(compiler::TNode<Context>{tmp159}, compiler::TNode<JSTypedArray>{tmp161}, compiler::TNode<JSTypedArray>{tmp168}, compiler::TNode<IntPtrT>{tmp164}, compiler::TNode<IntPtrT>{tmp166});
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 92);
    ca_.Goto(&block11, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp169;
    compiler::TNode<Object> tmp170;
    compiler::TNode<JSTypedArray> tmp171;
    compiler::TNode<IntPtrT> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<IntPtrT> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<IntPtrT> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<JSTypedArray> tmp178;
    ca_.Bind(&block11, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 91);
    ca_.Goto(&block10, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp179;
    compiler::TNode<Object> tmp180;
    compiler::TNode<JSTypedArray> tmp181;
    compiler::TNode<IntPtrT> tmp182;
    compiler::TNode<Object> tmp183;
    compiler::TNode<IntPtrT> tmp184;
    compiler::TNode<Object> tmp185;
    compiler::TNode<IntPtrT> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<JSTypedArray> tmp188;
    ca_.Bind(&block10, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188);
    ca_.SetSourcePosition("../../src/builtins/typed-array-slice.tq", 105);
    arguments->PopAndReturn(tmp188);
  }
}

}  // namespace internal
}  // namespace v8

