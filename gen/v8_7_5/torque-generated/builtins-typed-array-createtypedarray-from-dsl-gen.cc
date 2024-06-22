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

compiler::TNode<IntPtrT> TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::CalculateTotalElementsByteSize(compiler::TNode<IntPtrT> p_byteLength) {
  compiler::CodeAssemblerParameterizedLabel<IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<IntPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<IntPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_byteLength);

  if (block0.is_used()) {
    compiler::TNode<IntPtrT> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 31);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 30);
    compiler::TNode<IntPtrT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr18ATconstexpr_intptr((CodeStubAssembler(state_).ConstexprIntPtrAdd(FixedTypedArrayBase::kHeaderSize, kObjectAlignmentMask))));
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp1}, compiler::TNode<IntPtrT>{tmp0}));
    compiler::TNode<IntPtrT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr18ATconstexpr_intptr((CodeStubAssembler(state_).ConstexprWordNot(kObjectAlignmentMask))));
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).WordAnd(compiler::TNode<IntPtrT>{tmp2}, compiler::TNode<IntPtrT>{tmp3}));
    ca_.Goto(&block1, tmp0, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<IntPtrT> tmp5;
    compiler::TNode<IntPtrT> tmp6;
    ca_.Bind(&block1, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 29);
    ca_.Goto(&block2, tmp5, tmp6);
  }

    compiler::TNode<IntPtrT> tmp7;
    compiler::TNode<IntPtrT> tmp8;
    ca_.Bind(&block2, &tmp7, &tmp8);
  return compiler::TNode<IntPtrT>{tmp8};
}

compiler::TNode<UintPtrT> TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::TypedArrayInitialize(compiler::TNode<Context> p_context, bool p_initialize, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<Smi> p_length, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::TNode<JSReceiver> p_bufferConstructor) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver, IntPtrT, FixedTypedArrayBase> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver, IntPtrT, FixedTypedArrayBase> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver, IntPtrT, FixedTypedArrayBase> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver, Object, Object> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver, Object, Object, JSArrayBuffer> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver, Object> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver, Object, JSArrayBuffer> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT, Number, JSReceiver> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, UintPtrT, Map, Int32T, JSReceiver, UintPtrT> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_typedArray, p_length, p_elementsInfo.sizeLog2, p_elementsInfo.map, p_elementsInfo.kind, p_bufferConstructor);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<UintPtrT> tmp3;
    compiler::TNode<Map> tmp4;
    compiler::TNode<Int32T> tmp5;
    compiler::TNode<JSReceiver> tmp6;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 39);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 11);
    compiler::TNode<UintPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr13ATPositiveSmi(compiler::TNode<Smi>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 12);
    compiler::TNode<UintPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShl(compiler::TNode<UintPtrT>{tmp7}, compiler::TNode<UintPtrT>{tmp3}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 15);
    compiler::TNode<UintPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShr(compiler::TNode<UintPtrT>{tmp8}, compiler::TNode<UintPtrT>{tmp3}));
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<UintPtrT>{tmp9}, compiler::TNode<UintPtrT>{tmp7}));
    ca_.Branch(tmp10, &block5, &block6, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp3, tmp4, tmp5, tmp2, tmp2, tmp7, tmp8);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSTypedArray> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<UintPtrT> tmp14;
    compiler::TNode<Map> tmp15;
    compiler::TNode<Int32T> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<UintPtrT> tmp18;
    compiler::TNode<Map> tmp19;
    compiler::TNode<Int32T> tmp20;
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Smi> tmp22;
    compiler::TNode<UintPtrT> tmp23;
    compiler::TNode<UintPtrT> tmp24;
    ca_.Bind(&block5, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block3, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<JSTypedArray> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<UintPtrT> tmp28;
    compiler::TNode<Map> tmp29;
    compiler::TNode<Int32T> tmp30;
    compiler::TNode<JSReceiver> tmp31;
    compiler::TNode<UintPtrT> tmp32;
    compiler::TNode<Map> tmp33;
    compiler::TNode<Int32T> tmp34;
    compiler::TNode<Smi> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<UintPtrT> tmp37;
    compiler::TNode<UintPtrT> tmp38;
    ca_.Bind(&block6, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 16);
    ca_.Goto(&block4, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp38);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp39;
    compiler::TNode<JSTypedArray> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<UintPtrT> tmp42;
    compiler::TNode<Map> tmp43;
    compiler::TNode<Int32T> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<UintPtrT> tmp46;
    compiler::TNode<Map> tmp47;
    compiler::TNode<Int32T> tmp48;
    compiler::TNode<Smi> tmp49;
    compiler::TNode<Smi> tmp50;
    compiler::TNode<UintPtrT> tmp51;
    ca_.Bind(&block4, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 39);
    ca_.Goto(&block2, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp51);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<JSTypedArray> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<UintPtrT> tmp55;
    compiler::TNode<Map> tmp56;
    compiler::TNode<Int32T> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    ca_.Bind(&block3, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 40);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp52}, MessageTemplate::kInvalidArrayBufferLength);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<JSTypedArray> tmp60;
    compiler::TNode<Smi> tmp61;
    compiler::TNode<UintPtrT> tmp62;
    compiler::TNode<Map> tmp63;
    compiler::TNode<Int32T> tmp64;
    compiler::TNode<JSReceiver> tmp65;
    compiler::TNode<UintPtrT> tmp66;
    ca_.Bind(&block2, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 41);
    compiler::TNode<Number> tmp67;
    USE(tmp67);
    tmp67 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATuintptr(compiler::TNode<UintPtrT>{tmp66}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 42);
    compiler::TNode<JSReceiver> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<JSReceiver>(BaseBuiltinsFromDSLAssembler(state_).GetArrayBufferFunction(compiler::TNode<Context>{tmp59}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 45);
    compiler::TNode<BoolT> tmp69;
    USE(tmp69);
    tmp69 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<HeapObject>{tmp65}, compiler::TNode<HeapObject>{tmp68}));
    ca_.Branch(tmp69, &block11, &block12, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<JSTypedArray> tmp71;
    compiler::TNode<Smi> tmp72;
    compiler::TNode<UintPtrT> tmp73;
    compiler::TNode<Map> tmp74;
    compiler::TNode<Int32T> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    compiler::TNode<UintPtrT> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<JSReceiver> tmp79;
    ca_.Bind(&block11, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 47);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 46);
    compiler::TNode<JSReceiver> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ConstructWithTarget(compiler::TNode<Context>{tmp70}, compiler::TNode<JSReceiver>{tmp79}, compiler::TNode<JSReceiver>{tmp76}, compiler::TNode<Object>{tmp78}));
    ca_.Goto(&block8, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp81;
    compiler::TNode<JSTypedArray> tmp82;
    compiler::TNode<Smi> tmp83;
    compiler::TNode<UintPtrT> tmp84;
    compiler::TNode<Map> tmp85;
    compiler::TNode<Int32T> tmp86;
    compiler::TNode<JSReceiver> tmp87;
    compiler::TNode<UintPtrT> tmp88;
    compiler::TNode<Number> tmp89;
    compiler::TNode<JSReceiver> tmp90;
    ca_.Bind(&block12, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 50);
    compiler::TNode<UintPtrT> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(V8_TYPED_ARRAY_MAX_SIZE_IN_HEAP));
    compiler::TNode<BoolT> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrGreaterThan(compiler::TNode<UintPtrT>{tmp88}, compiler::TNode<UintPtrT>{tmp91}));
    ca_.Branch(tmp92, &block13, &block14, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp93;
    compiler::TNode<JSTypedArray> tmp94;
    compiler::TNode<Smi> tmp95;
    compiler::TNode<UintPtrT> tmp96;
    compiler::TNode<Map> tmp97;
    compiler::TNode<Int32T> tmp98;
    compiler::TNode<JSReceiver> tmp99;
    compiler::TNode<UintPtrT> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<JSReceiver> tmp102;
    ca_.Bind(&block13, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    ca_.Goto(&block10, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp103;
    compiler::TNode<JSTypedArray> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<UintPtrT> tmp106;
    compiler::TNode<Map> tmp107;
    compiler::TNode<Int32T> tmp108;
    compiler::TNode<JSReceiver> tmp109;
    compiler::TNode<UintPtrT> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<JSReceiver> tmp112;
    ca_.Bind(&block14, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 52);
    compiler::TNode<JSArrayBuffer> tmp113;
    USE(tmp113);
    tmp113 = ca_.UncheckedCast<JSArrayBuffer>(TypedArrayBuiltinsAssembler(state_).AllocateEmptyOnHeapBuffer(compiler::TNode<Context>{tmp103}, compiler::TNode<JSTypedArray>{tmp104}, compiler::TNode<UintPtrT>{tmp110}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 55);
    compiler::TNode<IntPtrT> tmp114;
    USE(tmp114);
    tmp114 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr9ATuintptr(compiler::TNode<UintPtrT>{tmp110}));
    compiler::TNode<IntPtrT> tmp115;
    USE(tmp115);
    tmp115 = ca_.UncheckedCast<IntPtrT>(TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).CalculateTotalElementsByteSize(compiler::TNode<IntPtrT>{tmp114}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 57);
    compiler::TNode<FixedTypedArrayBase> tmp116;
    USE(tmp116);
    tmp116 = ca_.UncheckedCast<FixedTypedArrayBase>(TypedArrayBuiltinsAssembler(state_).AllocateOnHeapElements(compiler::TNode<Map>{tmp107}, compiler::TNode<IntPtrT>{tmp115}, compiler::TNode<Number>{tmp105}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 58);
    compiler::TNode<IntPtrT> tmp117 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp117);
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp104, tmp117}, tmp116);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 60);
    if ((p_initialize)) {
      ca_.Goto(&block15, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp115, tmp116);
    } else {
      ca_.Goto(&block16, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp115, tmp116);
    }
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp118;
    compiler::TNode<JSTypedArray> tmp119;
    compiler::TNode<Smi> tmp120;
    compiler::TNode<UintPtrT> tmp121;
    compiler::TNode<Map> tmp122;
    compiler::TNode<Int32T> tmp123;
    compiler::TNode<JSReceiver> tmp124;
    compiler::TNode<UintPtrT> tmp125;
    compiler::TNode<Number> tmp126;
    compiler::TNode<JSReceiver> tmp127;
    compiler::TNode<IntPtrT> tmp128;
    compiler::TNode<FixedTypedArrayBase> tmp129;
    ca_.Bind(&block15, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 61);
    compiler::TNode<RawPtrT> tmp130;
    USE(tmp130);
    tmp130 = ca_.UncheckedCast<RawPtrT>(CodeStubAssembler(state_).LoadFixedTypedArrayOnHeapBackingStore(compiler::TNode<FixedTypedArrayBase>{tmp129}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 62);
    compiler::TNode<IntPtrT> tmp131;
    USE(tmp131);
    tmp131 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    TypedArrayBuiltinsAssembler(state_).CallCMemset(compiler::TNode<RawPtrT>{tmp130}, compiler::TNode<IntPtrT>{tmp131}, compiler::TNode<UintPtrT>{tmp125});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 60);
    ca_.Goto(&block17, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp132;
    compiler::TNode<JSTypedArray> tmp133;
    compiler::TNode<Smi> tmp134;
    compiler::TNode<UintPtrT> tmp135;
    compiler::TNode<Map> tmp136;
    compiler::TNode<Int32T> tmp137;
    compiler::TNode<JSReceiver> tmp138;
    compiler::TNode<UintPtrT> tmp139;
    compiler::TNode<Number> tmp140;
    compiler::TNode<JSReceiver> tmp141;
    compiler::TNode<IntPtrT> tmp142;
    compiler::TNode<FixedTypedArrayBase> tmp143;
    ca_.Bind(&block16, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143);
    ca_.Goto(&block17, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp144;
    compiler::TNode<JSTypedArray> tmp145;
    compiler::TNode<Smi> tmp146;
    compiler::TNode<UintPtrT> tmp147;
    compiler::TNode<Map> tmp148;
    compiler::TNode<Int32T> tmp149;
    compiler::TNode<JSReceiver> tmp150;
    compiler::TNode<UintPtrT> tmp151;
    compiler::TNode<Number> tmp152;
    compiler::TNode<JSReceiver> tmp153;
    compiler::TNode<IntPtrT> tmp154;
    compiler::TNode<FixedTypedArrayBase> tmp155;
    ca_.Bind(&block17, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 44);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 65);
    ca_.Goto(&block9, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp156;
    compiler::TNode<JSTypedArray> tmp157;
    compiler::TNode<Smi> tmp158;
    compiler::TNode<UintPtrT> tmp159;
    compiler::TNode<Map> tmp160;
    compiler::TNode<Int32T> tmp161;
    compiler::TNode<JSReceiver> tmp162;
    compiler::TNode<UintPtrT> tmp163;
    compiler::TNode<Number> tmp164;
    compiler::TNode<JSReceiver> tmp165;
    ca_.Bind(&block10, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 66);
    if ((p_initialize)) {
      ca_.Goto(&block18, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165);
    } else {
      ca_.Goto(&block19, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165);
    }
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp166;
    compiler::TNode<JSTypedArray> tmp167;
    compiler::TNode<Smi> tmp168;
    compiler::TNode<UintPtrT> tmp169;
    compiler::TNode<Map> tmp170;
    compiler::TNode<Int32T> tmp171;
    compiler::TNode<JSReceiver> tmp172;
    compiler::TNode<UintPtrT> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<JSReceiver> tmp175;
    ca_.Bind(&block18, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 67);
    compiler::TNode<JSReceiver> tmp176;
    USE(tmp176);
    tmp176 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).Construct(compiler::TNode<Context>{tmp166}, compiler::TNode<JSReceiver>{tmp175}, compiler::TNode<Object>{tmp174}));
    ca_.Goto(&block8, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp177;
    compiler::TNode<JSTypedArray> tmp178;
    compiler::TNode<Smi> tmp179;
    compiler::TNode<UintPtrT> tmp180;
    compiler::TNode<Map> tmp181;
    compiler::TNode<Int32T> tmp182;
    compiler::TNode<JSReceiver> tmp183;
    compiler::TNode<UintPtrT> tmp184;
    compiler::TNode<Number> tmp185;
    compiler::TNode<JSReceiver> tmp186;
    ca_.Bind(&block19, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 70);
    compiler::TNode<JSFunction> tmp187;
    USE(tmp187);
    tmp187 = ca_.UncheckedCast<JSFunction>(BaseBuiltinsFromDSLAssembler(state_).GetArrayBufferNoInitFunction(compiler::TNode<Context>{tmp177}));
    compiler::TNode<Oddball> tmp188;
    USE(tmp188);
    tmp188 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 69);
    compiler::TNode<Object> tmp189;
    USE(tmp189);
    tmp189 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp177}, compiler::TNode<JSReceiver>{tmp187}, compiler::TNode<Object>{tmp188}, compiler::TNode<Object>{tmp185}));
    ca_.Goto(&block8, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp189);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp190;
    compiler::TNode<JSTypedArray> tmp191;
    compiler::TNode<Smi> tmp192;
    compiler::TNode<UintPtrT> tmp193;
    compiler::TNode<Map> tmp194;
    compiler::TNode<Int32T> tmp195;
    compiler::TNode<JSReceiver> tmp196;
    compiler::TNode<UintPtrT> tmp197;
    compiler::TNode<Number> tmp198;
    compiler::TNode<JSReceiver> tmp199;
    ca_.Bind(&block9, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 73);
    ca_.Goto(&block7, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp200;
    compiler::TNode<JSTypedArray> tmp201;
    compiler::TNode<Smi> tmp202;
    compiler::TNode<UintPtrT> tmp203;
    compiler::TNode<Map> tmp204;
    compiler::TNode<Int32T> tmp205;
    compiler::TNode<JSReceiver> tmp206;
    compiler::TNode<UintPtrT> tmp207;
    compiler::TNode<Number> tmp208;
    compiler::TNode<JSReceiver> tmp209;
    compiler::TNode<Object> tmp210;
    ca_.Bind(&block8, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 74);
    compiler::TNode<JSArrayBuffer> tmp211;
    USE(tmp211);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp211 = BaseBuiltinsFromDSLAssembler(state_).Cast13JSArrayBuffer(compiler::TNode<Context>{tmp200}, compiler::TNode<Object>{tmp210}, &label0);
    ca_.Goto(&block23, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp210, tmp211);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block24, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp210);
    }
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp212;
    compiler::TNode<JSTypedArray> tmp213;
    compiler::TNode<Smi> tmp214;
    compiler::TNode<UintPtrT> tmp215;
    compiler::TNode<Map> tmp216;
    compiler::TNode<Int32T> tmp217;
    compiler::TNode<JSReceiver> tmp218;
    compiler::TNode<UintPtrT> tmp219;
    compiler::TNode<Number> tmp220;
    compiler::TNode<JSReceiver> tmp221;
    compiler::TNode<Object> tmp222;
    compiler::TNode<Object> tmp223;
    ca_.Bind(&block24, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223);
    ca_.Goto(&block22, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp224;
    compiler::TNode<JSTypedArray> tmp225;
    compiler::TNode<Smi> tmp226;
    compiler::TNode<UintPtrT> tmp227;
    compiler::TNode<Map> tmp228;
    compiler::TNode<Int32T> tmp229;
    compiler::TNode<JSReceiver> tmp230;
    compiler::TNode<UintPtrT> tmp231;
    compiler::TNode<Number> tmp232;
    compiler::TNode<JSReceiver> tmp233;
    compiler::TNode<Object> tmp234;
    compiler::TNode<Object> tmp235;
    compiler::TNode<JSArrayBuffer> tmp236;
    ca_.Bind(&block23, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236);
    ca_.Goto(&block21, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp236);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp237;
    compiler::TNode<JSTypedArray> tmp238;
    compiler::TNode<Smi> tmp239;
    compiler::TNode<UintPtrT> tmp240;
    compiler::TNode<Map> tmp241;
    compiler::TNode<Int32T> tmp242;
    compiler::TNode<JSReceiver> tmp243;
    compiler::TNode<UintPtrT> tmp244;
    compiler::TNode<Number> tmp245;
    compiler::TNode<JSReceiver> tmp246;
    compiler::TNode<Object> tmp247;
    ca_.Bind(&block22, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp248;
    compiler::TNode<JSTypedArray> tmp249;
    compiler::TNode<Smi> tmp250;
    compiler::TNode<UintPtrT> tmp251;
    compiler::TNode<Map> tmp252;
    compiler::TNode<Int32T> tmp253;
    compiler::TNode<JSReceiver> tmp254;
    compiler::TNode<UintPtrT> tmp255;
    compiler::TNode<Number> tmp256;
    compiler::TNode<JSReceiver> tmp257;
    compiler::TNode<Object> tmp258;
    compiler::TNode<JSArrayBuffer> tmp259;
    ca_.Bind(&block21, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 75);
    compiler::TNode<UintPtrT> tmp260;
    USE(tmp260);
    tmp260 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 76);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 77);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 76);
    BaseBuiltinsFromDSLAssembler(state_)._method_JSTypedArray_AttachOffHeapBuffer(compiler::TNode<JSTypedArray>{tmp249}, compiler::TNode<JSArrayBuffer>{tmp259}, compiler::TNode<Map>{tmp252}, compiler::TNode<Smi>{tmp250}, compiler::TNode<UintPtrT>{tmp260});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 73);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 44);
    ca_.Goto(&block7, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp261;
    compiler::TNode<JSTypedArray> tmp262;
    compiler::TNode<Smi> tmp263;
    compiler::TNode<UintPtrT> tmp264;
    compiler::TNode<Map> tmp265;
    compiler::TNode<Int32T> tmp266;
    compiler::TNode<JSReceiver> tmp267;
    compiler::TNode<UintPtrT> tmp268;
    compiler::TNode<Number> tmp269;
    compiler::TNode<JSReceiver> tmp270;
    ca_.Bind(&block7, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 80);
    compiler::TNode<UintPtrT> tmp271;
    USE(tmp271);
    tmp271 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 81);
    TypedArrayBuiltinsAssembler(state_).SetupTypedArray(compiler::TNode<JSTypedArray>{tmp262}, compiler::TNode<Smi>{tmp263}, compiler::TNode<UintPtrT>{tmp271}, compiler::TNode<UintPtrT>{tmp268});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 83);
    ca_.Goto(&block1, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp272;
    compiler::TNode<JSTypedArray> tmp273;
    compiler::TNode<Smi> tmp274;
    compiler::TNode<UintPtrT> tmp275;
    compiler::TNode<Map> tmp276;
    compiler::TNode<Int32T> tmp277;
    compiler::TNode<JSReceiver> tmp278;
    compiler::TNode<UintPtrT> tmp279;
    ca_.Bind(&block1, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 35);
    ca_.Goto(&block25, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279);
  }

    compiler::TNode<Context> tmp280;
    compiler::TNode<JSTypedArray> tmp281;
    compiler::TNode<Smi> tmp282;
    compiler::TNode<UintPtrT> tmp283;
    compiler::TNode<Map> tmp284;
    compiler::TNode<Int32T> tmp285;
    compiler::TNode<JSReceiver> tmp286;
    compiler::TNode<UintPtrT> tmp287;
    ca_.Bind(&block25, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287);
  return compiler::TNode<UintPtrT>{tmp287};
}

void TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::ConstructByLength(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<Object> p_length, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, UintPtrT, Map, Int32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, UintPtrT, Map, Int32T, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, UintPtrT, Map, Int32T, Number, Number, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, UintPtrT, Map, Int32T, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, UintPtrT, Map, Int32T, Number, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, UintPtrT, Map, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, UintPtrT, Map, Int32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_typedArray, p_length, p_elementsInfo.sizeLog2, p_elementsInfo.map, p_elementsInfo.kind);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<UintPtrT> tmp3;
    compiler::TNode<Map> tmp4;
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 92);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}, CodeStubAssembler::ToIntegerTruncationMode::kTruncateMinusZero));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 91);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 97);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).Cast13ATPositiveSmi(compiler::TNode<Object>{tmp6}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp6, tmp7);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp6);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<JSTypedArray> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<UintPtrT> tmp11;
    compiler::TNode<Map> tmp12;
    compiler::TNode<Int32T> tmp13;
    compiler::TNode<Number> tmp14;
    compiler::TNode<Number> tmp15;
    ca_.Bind(&block5, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.Goto(&block3, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<JSTypedArray> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<UintPtrT> tmp19;
    compiler::TNode<Map> tmp20;
    compiler::TNode<Int32T> tmp21;
    compiler::TNode<Number> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Smi> tmp24;
    ca_.Bind(&block4, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block2, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp24);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp25;
    compiler::TNode<JSTypedArray> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<UintPtrT> tmp28;
    compiler::TNode<Map> tmp29;
    compiler::TNode<Int32T> tmp30;
    compiler::TNode<Number> tmp31;
    ca_.Bind(&block3, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 98);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp25}, MessageTemplate::kInvalidTypedArrayLength, compiler::TNode<Object>{tmp27});
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSTypedArray> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<UintPtrT> tmp35;
    compiler::TNode<Map> tmp36;
    compiler::TNode<Int32T> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Smi> tmp39;
    ca_.Bind(&block2, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 97);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 99);
    compiler::TNode<JSReceiver> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<JSReceiver>(BaseBuiltinsFromDSLAssembler(state_).GetArrayBufferFunction(compiler::TNode<Context>{tmp32}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 102);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 103);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 101);
    compiler::TNode<UintPtrT> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<UintPtrT>(TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).TypedArrayInitialize(compiler::TNode<Context>{tmp32}, true, compiler::TNode<JSTypedArray>{tmp33}, compiler::TNode<Smi>{tmp39}, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo{compiler::TNode<UintPtrT>{tmp35}, compiler::TNode<Map>{tmp36}, compiler::TNode<Int32T>{tmp37}}, compiler::TNode<JSReceiver>{tmp40}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 90);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 88);
    ca_.Goto(&block1, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<JSTypedArray> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<UintPtrT> tmp45;
    compiler::TNode<Map> tmp46;
    compiler::TNode<Int32T> tmp47;
    ca_.Bind(&block1, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    ca_.Goto(&block6, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47);
  }

    compiler::TNode<Context> tmp48;
    compiler::TNode<JSTypedArray> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<UintPtrT> tmp51;
    compiler::TNode<Map> tmp52;
    compiler::TNode<Int32T> tmp53;
    ca_.Bind(&block6, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
}

void TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::ConstructByArrayLike(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<HeapObject> p_arrayLike, compiler::TNode<Object> p_initialLength, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::TNode<JSReceiver> p_bufferConstructor) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Object, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, HeapObject> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, HeapObject, JSTypedArray> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, JSTypedArray> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, JSTypedArray> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, JSTypedArray> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, JSTypedArray> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, JSTypedArray> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, JSTypedArray> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, JSTypedArray> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT, JSTypedArray> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver, Smi, UintPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, HeapObject, Object, UintPtrT, Map, Int32T, JSReceiver> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_typedArray, p_arrayLike, p_initialLength, p_elementsInfo.sizeLog2, p_elementsInfo.map, p_elementsInfo.kind, p_bufferConstructor);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<HeapObject> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<UintPtrT> tmp4;
    compiler::TNode<Map> tmp5;
    compiler::TNode<Int32T> tmp6;
    compiler::TNode<JSReceiver> tmp7;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 113);
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp8 = CodeStubAssembler(state_).ToSmiLength(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp3}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp3, tmp8);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp3);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    compiler::TNode<HeapObject> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<UintPtrT> tmp13;
    compiler::TNode<Map> tmp14;
    compiler::TNode<Int32T> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<Object> tmp17;
    ca_.Bind(&block5, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.Goto(&block3, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<JSTypedArray> tmp19;
    compiler::TNode<HeapObject> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<UintPtrT> tmp22;
    compiler::TNode<Map> tmp23;
    compiler::TNode<Int32T> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Smi> tmp27;
    ca_.Bind(&block4, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block2, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp27);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<JSTypedArray> tmp29;
    compiler::TNode<HeapObject> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<UintPtrT> tmp32;
    compiler::TNode<Map> tmp33;
    compiler::TNode<Int32T> tmp34;
    compiler::TNode<JSReceiver> tmp35;
    ca_.Bind(&block3, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 114);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp28}, MessageTemplate::kInvalidTypedArrayLength, compiler::TNode<Object>{tmp31});
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<JSTypedArray> tmp37;
    compiler::TNode<HeapObject> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<UintPtrT> tmp40;
    compiler::TNode<Map> tmp41;
    compiler::TNode<Int32T> tmp42;
    compiler::TNode<JSReceiver> tmp43;
    compiler::TNode<Smi> tmp44;
    ca_.Bind(&block2, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 113);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 117);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 116);
    compiler::TNode<UintPtrT> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<UintPtrT>(TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).TypedArrayInitialize(compiler::TNode<Context>{tmp36}, false, compiler::TNode<JSTypedArray>{tmp37}, compiler::TNode<Smi>{tmp44}, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo{compiler::TNode<UintPtrT>{tmp40}, compiler::TNode<Map>{tmp41}, compiler::TNode<Int32T>{tmp42}}, compiler::TNode<JSReceiver>{tmp43}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 120);
    compiler::TNode<JSTypedArray> tmp46;
    USE(tmp46);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp46 = BaseBuiltinsFromDSLAssembler(state_).Cast12JSTypedArray(compiler::TNode<HeapObject>{tmp38}, &label0);
    ca_.Goto(&block8, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp38, tmp46);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp38);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<JSTypedArray> tmp48;
    compiler::TNode<HeapObject> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<UintPtrT> tmp51;
    compiler::TNode<Map> tmp52;
    compiler::TNode<Int32T> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    compiler::TNode<Smi> tmp55;
    compiler::TNode<UintPtrT> tmp56;
    compiler::TNode<HeapObject> tmp57;
    ca_.Bind(&block9, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57);
    ca_.Goto(&block7, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp58;
    compiler::TNode<JSTypedArray> tmp59;
    compiler::TNode<HeapObject> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<UintPtrT> tmp62;
    compiler::TNode<Map> tmp63;
    compiler::TNode<Int32T> tmp64;
    compiler::TNode<JSReceiver> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<UintPtrT> tmp67;
    compiler::TNode<HeapObject> tmp68;
    compiler::TNode<JSTypedArray> tmp69;
    ca_.Bind(&block8, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 122);
    compiler::TNode<IntPtrT> tmp70 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp70);
    compiler::TNode<JSArrayBuffer>tmp71 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp69, tmp70});
    compiler::TNode<BoolT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp71}));
    ca_.Branch(tmp72, &block10, &block11, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp69);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp73;
    compiler::TNode<JSTypedArray> tmp74;
    compiler::TNode<HeapObject> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<UintPtrT> tmp77;
    compiler::TNode<Map> tmp78;
    compiler::TNode<Int32T> tmp79;
    compiler::TNode<JSReceiver> tmp80;
    compiler::TNode<Smi> tmp81;
    compiler::TNode<UintPtrT> tmp82;
    compiler::TNode<JSTypedArray> tmp83;
    ca_.Bind(&block10, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 123);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp73}, MessageTemplate::kDetachedOperation, "Construct");
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<JSTypedArray> tmp85;
    compiler::TNode<HeapObject> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<UintPtrT> tmp88;
    compiler::TNode<Map> tmp89;
    compiler::TNode<Int32T> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<Smi> tmp92;
    compiler::TNode<UintPtrT> tmp93;
    compiler::TNode<JSTypedArray> tmp94;
    ca_.Bind(&block11, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 125);
    compiler::TNode<Int32T> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).LoadElementsKind(compiler::TNode<JSTypedArray>{tmp94}));
    compiler::TNode<BoolT> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).ElementsKindNotEqual(compiler::TNode<Int32T>{tmp95}, compiler::TNode<Int32T>{tmp90}));
    ca_.Branch(tmp96, &block13, &block14, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<JSTypedArray> tmp98;
    compiler::TNode<HeapObject> tmp99;
    compiler::TNode<Object> tmp100;
    compiler::TNode<UintPtrT> tmp101;
    compiler::TNode<Map> tmp102;
    compiler::TNode<Int32T> tmp103;
    compiler::TNode<JSReceiver> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<UintPtrT> tmp106;
    compiler::TNode<JSTypedArray> tmp107;
    ca_.Bind(&block13, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 126);
    ca_.Goto(&block7, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp108;
    compiler::TNode<JSTypedArray> tmp109;
    compiler::TNode<HeapObject> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<UintPtrT> tmp112;
    compiler::TNode<Map> tmp113;
    compiler::TNode<Int32T> tmp114;
    compiler::TNode<JSReceiver> tmp115;
    compiler::TNode<Smi> tmp116;
    compiler::TNode<UintPtrT> tmp117;
    compiler::TNode<JSTypedArray> tmp118;
    ca_.Bind(&block14, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 128);
    compiler::TNode<Smi> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp120;
    USE(tmp120);
    tmp120 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp116}, compiler::TNode<Smi>{tmp119}));
    ca_.Branch(tmp120, &block16, &block17, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp121;
    compiler::TNode<JSTypedArray> tmp122;
    compiler::TNode<HeapObject> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<UintPtrT> tmp125;
    compiler::TNode<Map> tmp126;
    compiler::TNode<Int32T> tmp127;
    compiler::TNode<JSReceiver> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<UintPtrT> tmp130;
    compiler::TNode<JSTypedArray> tmp131;
    ca_.Bind(&block16, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 130);
    compiler::TNode<RawPtrT> tmp132;
    USE(tmp132);
    tmp132 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp122}));
    compiler::TNode<RawPtrT> tmp133;
    USE(tmp133);
    tmp133 = ca_.UncheckedCast<RawPtrT>(TypedArrayBuiltinsAssembler(state_).LoadDataPtr(compiler::TNode<JSTypedArray>{tmp131}));
    TypedArrayBuiltinsAssembler(state_).CallCMemcpy(compiler::TNode<RawPtrT>{tmp132}, compiler::TNode<RawPtrT>{tmp133}, compiler::TNode<UintPtrT>{tmp130});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 128);
    ca_.Goto(&block17, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp134;
    compiler::TNode<JSTypedArray> tmp135;
    compiler::TNode<HeapObject> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<UintPtrT> tmp138;
    compiler::TNode<Map> tmp139;
    compiler::TNode<Int32T> tmp140;
    compiler::TNode<JSReceiver> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<UintPtrT> tmp143;
    compiler::TNode<JSTypedArray> tmp144;
    ca_.Bind(&block17, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 125);
    ca_.Goto(&block15, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp145;
    compiler::TNode<JSTypedArray> tmp146;
    compiler::TNode<HeapObject> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<UintPtrT> tmp149;
    compiler::TNode<Map> tmp150;
    compiler::TNode<Int32T> tmp151;
    compiler::TNode<JSReceiver> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<UintPtrT> tmp154;
    compiler::TNode<JSTypedArray> tmp155;
    ca_.Bind(&block15, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 122);
    ca_.Goto(&block12, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp156;
    compiler::TNode<JSTypedArray> tmp157;
    compiler::TNode<HeapObject> tmp158;
    compiler::TNode<Object> tmp159;
    compiler::TNode<UintPtrT> tmp160;
    compiler::TNode<Map> tmp161;
    compiler::TNode<Int32T> tmp162;
    compiler::TNode<JSReceiver> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<UintPtrT> tmp165;
    compiler::TNode<JSTypedArray> tmp166;
    ca_.Bind(&block12, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 119);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 133);
    ca_.Goto(&block6, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp167;
    compiler::TNode<JSTypedArray> tmp168;
    compiler::TNode<HeapObject> tmp169;
    compiler::TNode<Object> tmp170;
    compiler::TNode<UintPtrT> tmp171;
    compiler::TNode<Map> tmp172;
    compiler::TNode<Int32T> tmp173;
    compiler::TNode<JSReceiver> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<UintPtrT> tmp176;
    ca_.Bind(&block7, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 134);
    compiler::TNode<Smi> tmp177;
    USE(tmp177);
    tmp177 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp178;
    USE(tmp178);
    tmp178 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp175}, compiler::TNode<Smi>{tmp177}));
    ca_.Branch(tmp178, &block18, &block19, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp179;
    compiler::TNode<JSTypedArray> tmp180;
    compiler::TNode<HeapObject> tmp181;
    compiler::TNode<Object> tmp182;
    compiler::TNode<UintPtrT> tmp183;
    compiler::TNode<Map> tmp184;
    compiler::TNode<Int32T> tmp185;
    compiler::TNode<JSReceiver> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<UintPtrT> tmp188;
    ca_.Bind(&block18, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 135);
    CodeStubAssembler(state_).CallRuntime(Runtime::kTypedArrayCopyElements, tmp179, tmp180, tmp181, tmp187);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 134);
    ca_.Goto(&block19, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp190;
    compiler::TNode<JSTypedArray> tmp191;
    compiler::TNode<HeapObject> tmp192;
    compiler::TNode<Object> tmp193;
    compiler::TNode<UintPtrT> tmp194;
    compiler::TNode<Map> tmp195;
    compiler::TNode<Int32T> tmp196;
    compiler::TNode<JSReceiver> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<UintPtrT> tmp199;
    ca_.Bind(&block19, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 119);
    ca_.Goto(&block6, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp200;
    compiler::TNode<JSTypedArray> tmp201;
    compiler::TNode<HeapObject> tmp202;
    compiler::TNode<Object> tmp203;
    compiler::TNode<UintPtrT> tmp204;
    compiler::TNode<Map> tmp205;
    compiler::TNode<Int32T> tmp206;
    compiler::TNode<JSReceiver> tmp207;
    compiler::TNode<Smi> tmp208;
    compiler::TNode<UintPtrT> tmp209;
    ca_.Bind(&block6, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 111);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 108);
    ca_.Goto(&block1, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp210;
    compiler::TNode<JSTypedArray> tmp211;
    compiler::TNode<HeapObject> tmp212;
    compiler::TNode<Object> tmp213;
    compiler::TNode<UintPtrT> tmp214;
    compiler::TNode<Map> tmp215;
    compiler::TNode<Int32T> tmp216;
    compiler::TNode<JSReceiver> tmp217;
    ca_.Bind(&block1, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217);
    ca_.Goto(&block20, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217);
  }

    compiler::TNode<Context> tmp218;
    compiler::TNode<JSTypedArray> tmp219;
    compiler::TNode<HeapObject> tmp220;
    compiler::TNode<Object> tmp221;
    compiler::TNode<UintPtrT> tmp222;
    compiler::TNode<Map> tmp223;
    compiler::TNode<Int32T> tmp224;
    compiler::TNode<JSReceiver> tmp225;
    ca_.Bind(&block20, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225);
}

void TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::ConstructByIterable(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<JSReceiver> p_iterable, compiler::TNode<JSReceiver> p_iteratorFn, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::CodeAssemblerLabel* label_IfConstructByArrayLike, compiler::TypedCodeAssemblerVariable<HeapObject>* label_IfConstructByArrayLike_parameter_0, compiler::TypedCodeAssemblerVariable<Object>* label_IfConstructByArrayLike_parameter_1, compiler::TypedCodeAssemblerVariable<JSReceiver>* label_IfConstructByArrayLike_parameter_2) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, JSReceiver, UintPtrT, Map, Int32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<HeapObject, Object, JSReceiver> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_typedArray, p_iterable, p_iteratorFn, p_elementsInfo.sizeLog2, p_elementsInfo.map, p_elementsInfo.kind);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<JSReceiver> tmp3;
    compiler::TNode<UintPtrT> tmp4;
    compiler::TNode<Map> tmp5;
    compiler::TNode<Int32T> tmp6;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 147);
    compiler::TNode<JSArray> tmp7;
    tmp7 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kIterableToListMayPreserveHoles, tmp0, tmp2, tmp3));
    USE(tmp7);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 146);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 148);
    compiler::TNode<IntPtrT> tmp8 = ca_.IntPtrConstant(JSArray::kLengthOffset);
    USE(tmp8);
    compiler::TNode<Number>tmp9 = CodeStubAssembler(state_).LoadReference<Number>(CodeStubAssembler::Reference{tmp7, tmp8});
    compiler::TNode<JSReceiver> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<JSReceiver>(BaseBuiltinsFromDSLAssembler(state_).GetArrayBufferFunction(compiler::TNode<Context>{tmp0}));
    ca_.Goto(&block1, tmp7, tmp9, tmp10);
  }

  if (block1.is_used()) {
    compiler::TNode<HeapObject> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 142);
    *label_IfConstructByArrayLike_parameter_2 = tmp13;
    *label_IfConstructByArrayLike_parameter_1 = tmp12;
    *label_IfConstructByArrayLike_parameter_0 = tmp11;
    ca_.Goto(label_IfConstructByArrayLike);
  }
}

void TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::ConstructByTypedArray(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<JSTypedArray> p_srcTypedArray, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::CodeAssemblerLabel* label_IfConstructByArrayLike, compiler::TypedCodeAssemblerVariable<HeapObject>* label_IfConstructByArrayLike_parameter_0, compiler::TypedCodeAssemblerVariable<Object>* label_IfConstructByArrayLike_parameter_1, compiler::TypedCodeAssemblerVariable<JSReceiver>* label_IfConstructByArrayLike_parameter_2) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T, JSReceiver, JSArrayBuffer> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T, JSReceiver, JSArrayBuffer> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T, JSReceiver, JSArrayBuffer> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T, JSReceiver, JSArrayBuffer, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T, JSReceiver, JSArrayBuffer, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T, JSReceiver, JSArrayBuffer, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T, JSReceiver, JSArrayBuffer, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T, JSReceiver, JSArrayBuffer, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<HeapObject, Object, JSReceiver> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_typedArray, p_srcTypedArray, p_elementsInfo.sizeLog2, p_elementsInfo.map, p_elementsInfo.kind);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSTypedArray> tmp2;
    compiler::TNode<UintPtrT> tmp3;
    compiler::TNode<Map> tmp4;
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 157);
    compiler::TNode<JSReceiver> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<JSReceiver>(BaseBuiltinsFromDSLAssembler(state_).GetArrayBufferFunction(compiler::TNode<Context>{tmp0}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 158);
    compiler::TNode<IntPtrT> tmp7 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp7);
    compiler::TNode<JSArrayBuffer>tmp8 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp2, tmp7});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 160);
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp8}));
    ca_.Branch(tmp9, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp8);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSTypedArray> tmp11;
    compiler::TNode<JSTypedArray> tmp12;
    compiler::TNode<UintPtrT> tmp13;
    compiler::TNode<Map> tmp14;
    compiler::TNode<Int32T> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<JSArrayBuffer> tmp17;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.Goto(&block5, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<JSTypedArray> tmp19;
    compiler::TNode<JSTypedArray> tmp20;
    compiler::TNode<UintPtrT> tmp21;
    compiler::TNode<Map> tmp22;
    compiler::TNode<Int32T> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<JSArrayBuffer> tmp25;
    ca_.Bind(&block3, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    compiler::TNode<IntPtrT> tmp26 = ca_.IntPtrConstant(JSTypedArray::kLengthOffset);
    USE(tmp26);
    compiler::TNode<Smi>tmp27 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp20, tmp26});
    ca_.Goto(&block4, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp27);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<JSTypedArray> tmp29;
    compiler::TNode<JSTypedArray> tmp30;
    compiler::TNode<UintPtrT> tmp31;
    compiler::TNode<Map> tmp32;
    compiler::TNode<Int32T> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    compiler::TNode<JSArrayBuffer> tmp35;
    ca_.Bind(&block5, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    compiler::TNode<Smi> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block4, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<JSTypedArray> tmp38;
    compiler::TNode<JSTypedArray> tmp39;
    compiler::TNode<UintPtrT> tmp40;
    compiler::TNode<Map> tmp41;
    compiler::TNode<Int32T> tmp42;
    compiler::TNode<JSReceiver> tmp43;
    compiler::TNode<JSArrayBuffer> tmp44;
    compiler::TNode<Smi> tmp45;
    ca_.Bind(&block4, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 165);
    compiler::TNode<BoolT> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<BoolT>(TypedArrayBuiltinsAssembler(state_).IsSharedArrayBuffer(compiler::TNode<JSArrayBuffer>{tmp44}));
    compiler::TNode<BoolT> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp46}));
    ca_.Branch(tmp47, &block6, &block7, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<JSTypedArray> tmp49;
    compiler::TNode<JSTypedArray> tmp50;
    compiler::TNode<UintPtrT> tmp51;
    compiler::TNode<Map> tmp52;
    compiler::TNode<Int32T> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    compiler::TNode<JSArrayBuffer> tmp55;
    compiler::TNode<Smi> tmp56;
    ca_.Bind(&block6, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 166);
    compiler::TNode<JSReceiver> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).SpeciesConstructor(compiler::TNode<Context>{tmp48}, compiler::TNode<Object>{tmp55}, compiler::TNode<JSReceiver>{tmp54}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 168);
    compiler::TNode<BoolT> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp55}));
    ca_.Branch(tmp58, &block8, &block9, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp57, tmp55, tmp56);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<JSTypedArray> tmp60;
    compiler::TNode<JSTypedArray> tmp61;
    compiler::TNode<UintPtrT> tmp62;
    compiler::TNode<Map> tmp63;
    compiler::TNode<Int32T> tmp64;
    compiler::TNode<JSReceiver> tmp65;
    compiler::TNode<JSArrayBuffer> tmp66;
    compiler::TNode<Smi> tmp67;
    ca_.Bind(&block8, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    compiler::TNode<Smi> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block9, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp68);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<JSTypedArray> tmp70;
    compiler::TNode<JSTypedArray> tmp71;
    compiler::TNode<UintPtrT> tmp72;
    compiler::TNode<Map> tmp73;
    compiler::TNode<Int32T> tmp74;
    compiler::TNode<JSReceiver> tmp75;
    compiler::TNode<JSArrayBuffer> tmp76;
    compiler::TNode<Smi> tmp77;
    ca_.Bind(&block9, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 165);
    ca_.Goto(&block7, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<JSTypedArray> tmp79;
    compiler::TNode<JSTypedArray> tmp80;
    compiler::TNode<UintPtrT> tmp81;
    compiler::TNode<Map> tmp82;
    compiler::TNode<Int32T> tmp83;
    compiler::TNode<JSReceiver> tmp84;
    compiler::TNode<JSArrayBuffer> tmp85;
    compiler::TNode<Smi> tmp86;
    ca_.Bind(&block7, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 170);
    ca_.Goto(&block1, tmp80, tmp86, tmp84);
  }

  if (block1.is_used()) {
    compiler::TNode<HeapObject> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<JSReceiver> tmp89;
    ca_.Bind(&block1, &tmp87, &tmp88, &tmp89);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 153);
    *label_IfConstructByArrayLike_parameter_2 = tmp89;
    *label_IfConstructByArrayLike_parameter_1 = tmp88;
    *label_IfConstructByArrayLike_parameter_0 = tmp87;
    ca_.Goto(label_IfConstructByArrayLike);
  }
}

void TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::ConstructByArrayBuffer(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<Object> p_byteOffset, compiler::TNode<Object> p_length, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Number, UintPtrT> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, UintPtrT> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, BoolT> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, Object> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, Object, Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, BoolT> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, UintPtrT> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, UintPtrT, Smi> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, Smi> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block41(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, UintPtrT, Smi, UintPtrT, UintPtrT> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T, String> block9(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block7(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block5(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSArrayBuffer, Object, Object, UintPtrT, Map, Int32T> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_typedArray, p_buffer, p_byteOffset, p_length, p_elementsInfo.sizeLog2, p_elementsInfo.map, p_elementsInfo.kind);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSArrayBuffer> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<UintPtrT> tmp5;
    compiler::TNode<Map> tmp6;
    compiler::TNode<Int32T> tmp7;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 179);
    compiler::TNode<UintPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 180);
    compiler::TNode<Oddball> tmp9;
    USE(tmp9);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp3}, compiler::TNode<HeapObject>{tmp9}));
    ca_.Branch(tmp10, &block10, &block11, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSTypedArray> tmp12;
    compiler::TNode<JSArrayBuffer> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<UintPtrT> tmp16;
    compiler::TNode<Map> tmp17;
    compiler::TNode<Int32T> tmp18;
    compiler::TNode<UintPtrT> tmp19;
    ca_.Bind(&block10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 183);
    compiler::TNode<Number> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp14}, CodeStubAssembler::ToIntegerTruncationMode::kTruncateMinusZero));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 182);
    compiler::TNode<UintPtrT> tmp21;
    USE(tmp21);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp21 = CodeStubAssembler(state_).TryNumberToUintPtr(compiler::TNode<Number>{tmp20}, &label0);
    ca_.Goto(&block14, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block15, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
    }
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<JSTypedArray> tmp23;
    compiler::TNode<JSArrayBuffer> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<UintPtrT> tmp27;
    compiler::TNode<Map> tmp28;
    compiler::TNode<Int32T> tmp29;
    compiler::TNode<UintPtrT> tmp30;
    compiler::TNode<Number> tmp31;
    ca_.Bind(&block15, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block13, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSTypedArray> tmp33;
    compiler::TNode<JSArrayBuffer> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<UintPtrT> tmp37;
    compiler::TNode<Map> tmp38;
    compiler::TNode<Int32T> tmp39;
    compiler::TNode<UintPtrT> tmp40;
    compiler::TNode<Number> tmp41;
    compiler::TNode<UintPtrT> tmp42;
    ca_.Bind(&block14, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    ca_.Goto(&block12, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp42);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp43;
    compiler::TNode<JSTypedArray> tmp44;
    compiler::TNode<JSArrayBuffer> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<UintPtrT> tmp48;
    compiler::TNode<Map> tmp49;
    compiler::TNode<Int32T> tmp50;
    compiler::TNode<UintPtrT> tmp51;
    ca_.Bind(&block13, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 184);
    ca_.Goto(&block3, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<JSTypedArray> tmp53;
    compiler::TNode<JSArrayBuffer> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<UintPtrT> tmp57;
    compiler::TNode<Map> tmp58;
    compiler::TNode<Int32T> tmp59;
    compiler::TNode<UintPtrT> tmp60;
    compiler::TNode<UintPtrT> tmp61;
    ca_.Bind(&block12, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 182);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 187);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 33);
    compiler::TNode<UintPtrT> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShl(compiler::TNode<UintPtrT>{tmp62}, compiler::TNode<UintPtrT>{tmp57}));
    compiler::TNode<UintPtrT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrSub(compiler::TNode<UintPtrT>{tmp63}, compiler::TNode<UintPtrT>{tmp64}));
    compiler::TNode<UintPtrT> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordAnd(compiler::TNode<UintPtrT>{tmp61}, compiler::TNode<UintPtrT>{tmp65}));
    compiler::TNode<UintPtrT> tmp67;
    USE(tmp67);
    tmp67 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<UintPtrT>{tmp66}, compiler::TNode<UintPtrT>{tmp67}));
    ca_.Goto(&block18, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp61, tmp57, tmp58, tmp59, tmp61, tmp61, tmp68);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp69;
    compiler::TNode<JSTypedArray> tmp70;
    compiler::TNode<JSArrayBuffer> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<UintPtrT> tmp74;
    compiler::TNode<Map> tmp75;
    compiler::TNode<Int32T> tmp76;
    compiler::TNode<UintPtrT> tmp77;
    compiler::TNode<UintPtrT> tmp78;
    compiler::TNode<Map> tmp79;
    compiler::TNode<Int32T> tmp80;
    compiler::TNode<UintPtrT> tmp81;
    compiler::TNode<UintPtrT> tmp82;
    compiler::TNode<BoolT> tmp83;
    ca_.Bind(&block18, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 187);
    ca_.Branch(tmp83, &block16, &block17, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<JSTypedArray> tmp85;
    compiler::TNode<JSArrayBuffer> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<UintPtrT> tmp89;
    compiler::TNode<Map> tmp90;
    compiler::TNode<Int32T> tmp91;
    compiler::TNode<UintPtrT> tmp92;
    ca_.Bind(&block16, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 188);
    compiler::TNode<String> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("start offset"));
    ca_.Goto(&block9, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp93);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp94;
    compiler::TNode<JSTypedArray> tmp95;
    compiler::TNode<JSArrayBuffer> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<UintPtrT> tmp99;
    compiler::TNode<Map> tmp100;
    compiler::TNode<Int32T> tmp101;
    compiler::TNode<UintPtrT> tmp102;
    ca_.Bind(&block17, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 180);
    ca_.Goto(&block11, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp103;
    compiler::TNode<JSTypedArray> tmp104;
    compiler::TNode<JSArrayBuffer> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<Object> tmp107;
    compiler::TNode<UintPtrT> tmp108;
    compiler::TNode<Map> tmp109;
    compiler::TNode<Int32T> tmp110;
    compiler::TNode<UintPtrT> tmp111;
    ca_.Bind(&block11, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 192);
    compiler::TNode<Smi> tmp112;
    USE(tmp112);
    tmp112 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr13ATPositiveSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 193);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 195);
    compiler::TNode<Oddball> tmp113;
    USE(tmp113);
    tmp113 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp114;
    USE(tmp114);
    tmp114 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp107}, compiler::TNode<HeapObject>{tmp113}));
    ca_.Branch(tmp114, &block19, &block20, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, ca_.Uninitialized<UintPtrT>());
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp115;
    compiler::TNode<JSTypedArray> tmp116;
    compiler::TNode<JSArrayBuffer> tmp117;
    compiler::TNode<Object> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<UintPtrT> tmp120;
    compiler::TNode<Map> tmp121;
    compiler::TNode<Int32T> tmp122;
    compiler::TNode<UintPtrT> tmp123;
    compiler::TNode<Smi> tmp124;
    compiler::TNode<UintPtrT> tmp125;
    ca_.Bind(&block19, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 197);
    compiler::TNode<Smi> tmp126;
    USE(tmp126);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp126 = CodeStubAssembler(state_).ToSmiIndex(compiler::TNode<Context>{tmp115}, compiler::TNode<Object>{tmp119}, &label0);
    ca_.Goto(&block21, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp119, tmp126);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block22, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp119);
    }
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp127;
    compiler::TNode<JSTypedArray> tmp128;
    compiler::TNode<JSArrayBuffer> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<UintPtrT> tmp132;
    compiler::TNode<Map> tmp133;
    compiler::TNode<Int32T> tmp134;
    compiler::TNode<UintPtrT> tmp135;
    compiler::TNode<Smi> tmp136;
    compiler::TNode<UintPtrT> tmp137;
    compiler::TNode<Object> tmp138;
    ca_.Bind(&block22, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138);
    ca_.Goto(&block5, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp139;
    compiler::TNode<JSTypedArray> tmp140;
    compiler::TNode<JSArrayBuffer> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<UintPtrT> tmp144;
    compiler::TNode<Map> tmp145;
    compiler::TNode<Int32T> tmp146;
    compiler::TNode<UintPtrT> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<UintPtrT> tmp149;
    compiler::TNode<Object> tmp150;
    compiler::TNode<Smi> tmp151;
    ca_.Bind(&block21, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 195);
    ca_.Goto(&block20, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp151, tmp149);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp152;
    compiler::TNode<JSTypedArray> tmp153;
    compiler::TNode<JSArrayBuffer> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<Object> tmp156;
    compiler::TNode<UintPtrT> tmp157;
    compiler::TNode<Map> tmp158;
    compiler::TNode<Int32T> tmp159;
    compiler::TNode<UintPtrT> tmp160;
    compiler::TNode<Smi> tmp161;
    compiler::TNode<UintPtrT> tmp162;
    ca_.Bind(&block20, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 201);
    compiler::TNode<BoolT> tmp163;
    USE(tmp163);
    tmp163 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp154}));
    ca_.Branch(tmp163, &block23, &block24, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp164;
    compiler::TNode<JSTypedArray> tmp165;
    compiler::TNode<JSArrayBuffer> tmp166;
    compiler::TNode<Object> tmp167;
    compiler::TNode<Object> tmp168;
    compiler::TNode<UintPtrT> tmp169;
    compiler::TNode<Map> tmp170;
    compiler::TNode<Int32T> tmp171;
    compiler::TNode<UintPtrT> tmp172;
    compiler::TNode<Smi> tmp173;
    compiler::TNode<UintPtrT> tmp174;
    ca_.Bind(&block23, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 202);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp164}, MessageTemplate::kDetachedOperation, "Construct");
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp175;
    compiler::TNode<JSTypedArray> tmp176;
    compiler::TNode<JSArrayBuffer> tmp177;
    compiler::TNode<Object> tmp178;
    compiler::TNode<Object> tmp179;
    compiler::TNode<UintPtrT> tmp180;
    compiler::TNode<Map> tmp181;
    compiler::TNode<Int32T> tmp182;
    compiler::TNode<UintPtrT> tmp183;
    compiler::TNode<Smi> tmp184;
    compiler::TNode<UintPtrT> tmp185;
    ca_.Bind(&block24, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 206);
    compiler::TNode<IntPtrT> tmp186 = ca_.IntPtrConstant(JSArrayBuffer::kByteLengthOffset);
    USE(tmp186);
    compiler::TNode<UintPtrT>tmp187 = CodeStubAssembler(state_).LoadReference<UintPtrT>(CodeStubAssembler::Reference{tmp177, tmp186});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 209);
    compiler::TNode<Oddball> tmp188;
    USE(tmp188);
    tmp188 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp189;
    USE(tmp189);
    tmp189 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<Object>{tmp179}, compiler::TNode<HeapObject>{tmp188}));
    ca_.Branch(tmp189, &block25, &block26, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp187);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp190;
    compiler::TNode<JSTypedArray> tmp191;
    compiler::TNode<JSArrayBuffer> tmp192;
    compiler::TNode<Object> tmp193;
    compiler::TNode<Object> tmp194;
    compiler::TNode<UintPtrT> tmp195;
    compiler::TNode<Map> tmp196;
    compiler::TNode<Int32T> tmp197;
    compiler::TNode<UintPtrT> tmp198;
    compiler::TNode<Smi> tmp199;
    compiler::TNode<UintPtrT> tmp200;
    compiler::TNode<UintPtrT> tmp201;
    ca_.Bind(&block25, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 212);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 33);
    compiler::TNode<UintPtrT> tmp202;
    USE(tmp202);
    tmp202 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp203;
    USE(tmp203);
    tmp203 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShl(compiler::TNode<UintPtrT>{tmp202}, compiler::TNode<UintPtrT>{tmp195}));
    compiler::TNode<UintPtrT> tmp204;
    USE(tmp204);
    tmp204 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp205;
    USE(tmp205);
    tmp205 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrSub(compiler::TNode<UintPtrT>{tmp203}, compiler::TNode<UintPtrT>{tmp204}));
    compiler::TNode<UintPtrT> tmp206;
    USE(tmp206);
    tmp206 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordAnd(compiler::TNode<UintPtrT>{tmp201}, compiler::TNode<UintPtrT>{tmp205}));
    compiler::TNode<UintPtrT> tmp207;
    USE(tmp207);
    tmp207 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp208;
    USE(tmp208);
    tmp208 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<UintPtrT>{tmp206}, compiler::TNode<UintPtrT>{tmp207}));
    ca_.Goto(&block30, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp195, tmp196, tmp197, tmp201, tmp201, tmp208);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp209;
    compiler::TNode<JSTypedArray> tmp210;
    compiler::TNode<JSArrayBuffer> tmp211;
    compiler::TNode<Object> tmp212;
    compiler::TNode<Object> tmp213;
    compiler::TNode<UintPtrT> tmp214;
    compiler::TNode<Map> tmp215;
    compiler::TNode<Int32T> tmp216;
    compiler::TNode<UintPtrT> tmp217;
    compiler::TNode<Smi> tmp218;
    compiler::TNode<UintPtrT> tmp219;
    compiler::TNode<UintPtrT> tmp220;
    compiler::TNode<UintPtrT> tmp221;
    compiler::TNode<Map> tmp222;
    compiler::TNode<Int32T> tmp223;
    compiler::TNode<UintPtrT> tmp224;
    compiler::TNode<UintPtrT> tmp225;
    compiler::TNode<BoolT> tmp226;
    ca_.Bind(&block30, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 212);
    ca_.Branch(tmp226, &block28, &block29, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp227;
    compiler::TNode<JSTypedArray> tmp228;
    compiler::TNode<JSArrayBuffer> tmp229;
    compiler::TNode<Object> tmp230;
    compiler::TNode<Object> tmp231;
    compiler::TNode<UintPtrT> tmp232;
    compiler::TNode<Map> tmp233;
    compiler::TNode<Int32T> tmp234;
    compiler::TNode<UintPtrT> tmp235;
    compiler::TNode<Smi> tmp236;
    compiler::TNode<UintPtrT> tmp237;
    compiler::TNode<UintPtrT> tmp238;
    ca_.Bind(&block28, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 213);
    compiler::TNode<String> tmp239;
    USE(tmp239);
    tmp239 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("byte length"));
    ca_.Goto(&block9, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp239);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp240;
    compiler::TNode<JSTypedArray> tmp241;
    compiler::TNode<JSArrayBuffer> tmp242;
    compiler::TNode<Object> tmp243;
    compiler::TNode<Object> tmp244;
    compiler::TNode<UintPtrT> tmp245;
    compiler::TNode<Map> tmp246;
    compiler::TNode<Int32T> tmp247;
    compiler::TNode<UintPtrT> tmp248;
    compiler::TNode<Smi> tmp249;
    compiler::TNode<UintPtrT> tmp250;
    compiler::TNode<UintPtrT> tmp251;
    ca_.Bind(&block29, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 218);
    compiler::TNode<BoolT> tmp252;
    USE(tmp252);
    tmp252 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrLessThan(compiler::TNode<UintPtrT>{tmp251}, compiler::TNode<UintPtrT>{tmp248}));
    ca_.Branch(tmp252, &block31, &block32, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp253;
    compiler::TNode<JSTypedArray> tmp254;
    compiler::TNode<JSArrayBuffer> tmp255;
    compiler::TNode<Object> tmp256;
    compiler::TNode<Object> tmp257;
    compiler::TNode<UintPtrT> tmp258;
    compiler::TNode<Map> tmp259;
    compiler::TNode<Int32T> tmp260;
    compiler::TNode<UintPtrT> tmp261;
    compiler::TNode<Smi> tmp262;
    compiler::TNode<UintPtrT> tmp263;
    compiler::TNode<UintPtrT> tmp264;
    ca_.Bind(&block31, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264);
    ca_.Goto(&block3, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp265;
    compiler::TNode<JSTypedArray> tmp266;
    compiler::TNode<JSArrayBuffer> tmp267;
    compiler::TNode<Object> tmp268;
    compiler::TNode<Object> tmp269;
    compiler::TNode<UintPtrT> tmp270;
    compiler::TNode<Map> tmp271;
    compiler::TNode<Int32T> tmp272;
    compiler::TNode<UintPtrT> tmp273;
    compiler::TNode<Smi> tmp274;
    compiler::TNode<UintPtrT> tmp275;
    compiler::TNode<UintPtrT> tmp276;
    ca_.Bind(&block32, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 222);
    compiler::TNode<UintPtrT> tmp277;
    USE(tmp277);
    tmp277 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrSub(compiler::TNode<UintPtrT>{tmp276}, compiler::TNode<UintPtrT>{tmp273}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 223);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 22);
    compiler::TNode<UintPtrT> tmp278;
    USE(tmp278);
    tmp278 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShr(compiler::TNode<UintPtrT>{tmp277}, compiler::TNode<UintPtrT>{tmp270}));
    compiler::TNode<Smi> tmp279;
    USE(tmp279);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp279 = BaseBuiltinsFromDSLAssembler(state_).TryUintPtrToPositiveSmi(compiler::TNode<UintPtrT>{tmp278}, &label0);
    ca_.Goto(&block34, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272, tmp273, tmp274, tmp277, tmp276, tmp270, tmp271, tmp272, tmp277, tmp277, tmp278, tmp279);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block35, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272, tmp273, tmp274, tmp277, tmp276, tmp270, tmp271, tmp272, tmp277, tmp277, tmp278);
    }
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp280;
    compiler::TNode<JSTypedArray> tmp281;
    compiler::TNode<JSArrayBuffer> tmp282;
    compiler::TNode<Object> tmp283;
    compiler::TNode<Object> tmp284;
    compiler::TNode<UintPtrT> tmp285;
    compiler::TNode<Map> tmp286;
    compiler::TNode<Int32T> tmp287;
    compiler::TNode<UintPtrT> tmp288;
    compiler::TNode<Smi> tmp289;
    compiler::TNode<UintPtrT> tmp290;
    compiler::TNode<UintPtrT> tmp291;
    compiler::TNode<UintPtrT> tmp292;
    compiler::TNode<Map> tmp293;
    compiler::TNode<Int32T> tmp294;
    compiler::TNode<UintPtrT> tmp295;
    compiler::TNode<UintPtrT> tmp296;
    compiler::TNode<UintPtrT> tmp297;
    ca_.Bind(&block35, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297);
    ca_.Goto(&block3, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp298;
    compiler::TNode<JSTypedArray> tmp299;
    compiler::TNode<JSArrayBuffer> tmp300;
    compiler::TNode<Object> tmp301;
    compiler::TNode<Object> tmp302;
    compiler::TNode<UintPtrT> tmp303;
    compiler::TNode<Map> tmp304;
    compiler::TNode<Int32T> tmp305;
    compiler::TNode<UintPtrT> tmp306;
    compiler::TNode<Smi> tmp307;
    compiler::TNode<UintPtrT> tmp308;
    compiler::TNode<UintPtrT> tmp309;
    compiler::TNode<UintPtrT> tmp310;
    compiler::TNode<Map> tmp311;
    compiler::TNode<Int32T> tmp312;
    compiler::TNode<UintPtrT> tmp313;
    compiler::TNode<UintPtrT> tmp314;
    compiler::TNode<UintPtrT> tmp315;
    compiler::TNode<Smi> tmp316;
    ca_.Bind(&block34, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316);
    ca_.Goto(&block33, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311, tmp312, tmp313, tmp314, tmp316);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp317;
    compiler::TNode<JSTypedArray> tmp318;
    compiler::TNode<JSArrayBuffer> tmp319;
    compiler::TNode<Object> tmp320;
    compiler::TNode<Object> tmp321;
    compiler::TNode<UintPtrT> tmp322;
    compiler::TNode<Map> tmp323;
    compiler::TNode<Int32T> tmp324;
    compiler::TNode<UintPtrT> tmp325;
    compiler::TNode<Smi> tmp326;
    compiler::TNode<UintPtrT> tmp327;
    compiler::TNode<UintPtrT> tmp328;
    compiler::TNode<UintPtrT> tmp329;
    compiler::TNode<Map> tmp330;
    compiler::TNode<Int32T> tmp331;
    compiler::TNode<UintPtrT> tmp332;
    compiler::TNode<UintPtrT> tmp333;
    compiler::TNode<Smi> tmp334;
    ca_.Bind(&block33, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 223);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 209);
    ca_.Goto(&block27, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp334, tmp327, tmp328);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp335;
    compiler::TNode<JSTypedArray> tmp336;
    compiler::TNode<JSArrayBuffer> tmp337;
    compiler::TNode<Object> tmp338;
    compiler::TNode<Object> tmp339;
    compiler::TNode<UintPtrT> tmp340;
    compiler::TNode<Map> tmp341;
    compiler::TNode<Int32T> tmp342;
    compiler::TNode<UintPtrT> tmp343;
    compiler::TNode<Smi> tmp344;
    compiler::TNode<UintPtrT> tmp345;
    compiler::TNode<UintPtrT> tmp346;
    ca_.Bind(&block26, &tmp335, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 229);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 11);
    compiler::TNode<UintPtrT> tmp347;
    USE(tmp347);
    tmp347 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr13ATPositiveSmi(compiler::TNode<Smi>{tmp344}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 12);
    compiler::TNode<UintPtrT> tmp348;
    USE(tmp348);
    tmp348 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShl(compiler::TNode<UintPtrT>{tmp347}, compiler::TNode<UintPtrT>{tmp340}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 15);
    compiler::TNode<UintPtrT> tmp349;
    USE(tmp349);
    tmp349 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShr(compiler::TNode<UintPtrT>{tmp348}, compiler::TNode<UintPtrT>{tmp340}));
    compiler::TNode<BoolT> tmp350;
    USE(tmp350);
    tmp350 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<UintPtrT>{tmp349}, compiler::TNode<UintPtrT>{tmp347}));
    ca_.Branch(tmp350, &block37, &block38, tmp335, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp340, tmp341, tmp342, tmp344, tmp344, tmp347, tmp348);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp351;
    compiler::TNode<JSTypedArray> tmp352;
    compiler::TNode<JSArrayBuffer> tmp353;
    compiler::TNode<Object> tmp354;
    compiler::TNode<Object> tmp355;
    compiler::TNode<UintPtrT> tmp356;
    compiler::TNode<Map> tmp357;
    compiler::TNode<Int32T> tmp358;
    compiler::TNode<UintPtrT> tmp359;
    compiler::TNode<Smi> tmp360;
    compiler::TNode<UintPtrT> tmp361;
    compiler::TNode<UintPtrT> tmp362;
    compiler::TNode<UintPtrT> tmp363;
    compiler::TNode<Map> tmp364;
    compiler::TNode<Int32T> tmp365;
    compiler::TNode<Smi> tmp366;
    compiler::TNode<Smi> tmp367;
    compiler::TNode<UintPtrT> tmp368;
    compiler::TNode<UintPtrT> tmp369;
    ca_.Bind(&block37, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369);
    ca_.Goto(&block7, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, tmp357, tmp358);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp370;
    compiler::TNode<JSTypedArray> tmp371;
    compiler::TNode<JSArrayBuffer> tmp372;
    compiler::TNode<Object> tmp373;
    compiler::TNode<Object> tmp374;
    compiler::TNode<UintPtrT> tmp375;
    compiler::TNode<Map> tmp376;
    compiler::TNode<Int32T> tmp377;
    compiler::TNode<UintPtrT> tmp378;
    compiler::TNode<Smi> tmp379;
    compiler::TNode<UintPtrT> tmp380;
    compiler::TNode<UintPtrT> tmp381;
    compiler::TNode<UintPtrT> tmp382;
    compiler::TNode<Map> tmp383;
    compiler::TNode<Int32T> tmp384;
    compiler::TNode<Smi> tmp385;
    compiler::TNode<Smi> tmp386;
    compiler::TNode<UintPtrT> tmp387;
    compiler::TNode<UintPtrT> tmp388;
    ca_.Bind(&block38, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 16);
    ca_.Goto(&block36, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384, tmp385, tmp386, tmp388);
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp389;
    compiler::TNode<JSTypedArray> tmp390;
    compiler::TNode<JSArrayBuffer> tmp391;
    compiler::TNode<Object> tmp392;
    compiler::TNode<Object> tmp393;
    compiler::TNode<UintPtrT> tmp394;
    compiler::TNode<Map> tmp395;
    compiler::TNode<Int32T> tmp396;
    compiler::TNode<UintPtrT> tmp397;
    compiler::TNode<Smi> tmp398;
    compiler::TNode<UintPtrT> tmp399;
    compiler::TNode<UintPtrT> tmp400;
    compiler::TNode<UintPtrT> tmp401;
    compiler::TNode<Map> tmp402;
    compiler::TNode<Int32T> tmp403;
    compiler::TNode<Smi> tmp404;
    compiler::TNode<Smi> tmp405;
    compiler::TNode<UintPtrT> tmp406;
    ca_.Bind(&block36, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 229);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 234);
    compiler::TNode<BoolT> tmp407;
    USE(tmp407);
    tmp407 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrLessThan(compiler::TNode<UintPtrT>{tmp400}, compiler::TNode<UintPtrT>{tmp406}));
    ca_.Branch(tmp407, &block39, &block41, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp396, tmp397, tmp398, tmp406, tmp400);
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp408;
    compiler::TNode<JSTypedArray> tmp409;
    compiler::TNode<JSArrayBuffer> tmp410;
    compiler::TNode<Object> tmp411;
    compiler::TNode<Object> tmp412;
    compiler::TNode<UintPtrT> tmp413;
    compiler::TNode<Map> tmp414;
    compiler::TNode<Int32T> tmp415;
    compiler::TNode<UintPtrT> tmp416;
    compiler::TNode<Smi> tmp417;
    compiler::TNode<UintPtrT> tmp418;
    compiler::TNode<UintPtrT> tmp419;
    ca_.Bind(&block41, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 235);
    compiler::TNode<UintPtrT> tmp420;
    USE(tmp420);
    tmp420 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrSub(compiler::TNode<UintPtrT>{tmp419}, compiler::TNode<UintPtrT>{tmp418}));
    compiler::TNode<BoolT> tmp421;
    USE(tmp421);
    tmp421 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).UintPtrGreaterThan(compiler::TNode<UintPtrT>{tmp416}, compiler::TNode<UintPtrT>{tmp420}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 234);
    ca_.Branch(tmp421, &block39, &block40, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp422;
    compiler::TNode<JSTypedArray> tmp423;
    compiler::TNode<JSArrayBuffer> tmp424;
    compiler::TNode<Object> tmp425;
    compiler::TNode<Object> tmp426;
    compiler::TNode<UintPtrT> tmp427;
    compiler::TNode<Map> tmp428;
    compiler::TNode<Int32T> tmp429;
    compiler::TNode<UintPtrT> tmp430;
    compiler::TNode<Smi> tmp431;
    compiler::TNode<UintPtrT> tmp432;
    compiler::TNode<UintPtrT> tmp433;
    ca_.Bind(&block39, &tmp422, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 236);
    ca_.Goto(&block5, tmp422, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp434;
    compiler::TNode<JSTypedArray> tmp435;
    compiler::TNode<JSArrayBuffer> tmp436;
    compiler::TNode<Object> tmp437;
    compiler::TNode<Object> tmp438;
    compiler::TNode<UintPtrT> tmp439;
    compiler::TNode<Map> tmp440;
    compiler::TNode<Int32T> tmp441;
    compiler::TNode<UintPtrT> tmp442;
    compiler::TNode<Smi> tmp443;
    compiler::TNode<UintPtrT> tmp444;
    compiler::TNode<UintPtrT> tmp445;
    ca_.Bind(&block40, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442, &tmp443, &tmp444, &tmp445);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 209);
    ca_.Goto(&block27, tmp434, tmp435, tmp436, tmp437, tmp438, tmp439, tmp440, tmp441, tmp442, tmp443, tmp444, tmp445);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp446;
    compiler::TNode<JSTypedArray> tmp447;
    compiler::TNode<JSArrayBuffer> tmp448;
    compiler::TNode<Object> tmp449;
    compiler::TNode<Object> tmp450;
    compiler::TNode<UintPtrT> tmp451;
    compiler::TNode<Map> tmp452;
    compiler::TNode<Int32T> tmp453;
    compiler::TNode<UintPtrT> tmp454;
    compiler::TNode<Smi> tmp455;
    compiler::TNode<UintPtrT> tmp456;
    compiler::TNode<UintPtrT> tmp457;
    ca_.Bind(&block27, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 239);
    TypedArrayBuiltinsAssembler(state_).SetupTypedArray(compiler::TNode<JSTypedArray>{tmp447}, compiler::TNode<Smi>{tmp455}, compiler::TNode<UintPtrT>{tmp454}, compiler::TNode<UintPtrT>{tmp456});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 240);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 241);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 240);
    BaseBuiltinsFromDSLAssembler(state_)._method_JSTypedArray_AttachOffHeapBuffer(compiler::TNode<JSTypedArray>{tmp447}, compiler::TNode<JSArrayBuffer>{tmp448}, compiler::TNode<Map>{tmp452}, compiler::TNode<Smi>{tmp455}, compiler::TNode<UintPtrT>{tmp454});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 178);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 243);
    ca_.Goto(&block8, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452, tmp453);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp458;
    compiler::TNode<JSTypedArray> tmp459;
    compiler::TNode<JSArrayBuffer> tmp460;
    compiler::TNode<Object> tmp461;
    compiler::TNode<Object> tmp462;
    compiler::TNode<UintPtrT> tmp463;
    compiler::TNode<Map> tmp464;
    compiler::TNode<Int32T> tmp465;
    compiler::TNode<String> tmp466;
    ca_.Bind(&block9, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463, &tmp464, &tmp465, &tmp466);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 244);
    compiler::TNode<IntPtrT> tmp467 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp467);
    compiler::TNode<Map>tmp468 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp459, tmp467});
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidTypedArrayAlignment, tmp458, tmp468, tmp466);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp470;
    compiler::TNode<JSTypedArray> tmp471;
    compiler::TNode<JSArrayBuffer> tmp472;
    compiler::TNode<Object> tmp473;
    compiler::TNode<Object> tmp474;
    compiler::TNode<UintPtrT> tmp475;
    compiler::TNode<Map> tmp476;
    compiler::TNode<Int32T> tmp477;
    ca_.Bind(&block8, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 246);
    ca_.Goto(&block6, tmp470, tmp471, tmp472, tmp473, tmp474, tmp475, tmp476, tmp477);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp478;
    compiler::TNode<JSTypedArray> tmp479;
    compiler::TNode<JSArrayBuffer> tmp480;
    compiler::TNode<Object> tmp481;
    compiler::TNode<Object> tmp482;
    compiler::TNode<UintPtrT> tmp483;
    compiler::TNode<Map> tmp484;
    compiler::TNode<Int32T> tmp485;
    ca_.Bind(&block7, &tmp478, &tmp479, &tmp480, &tmp481, &tmp482, &tmp483, &tmp484, &tmp485);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 247);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp478}, MessageTemplate::kInvalidArrayBufferLength);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp486;
    compiler::TNode<JSTypedArray> tmp487;
    compiler::TNode<JSArrayBuffer> tmp488;
    compiler::TNode<Object> tmp489;
    compiler::TNode<Object> tmp490;
    compiler::TNode<UintPtrT> tmp491;
    compiler::TNode<Map> tmp492;
    compiler::TNode<Int32T> tmp493;
    ca_.Bind(&block6, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 249);
    ca_.Goto(&block4, tmp486, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492, tmp493);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp494;
    compiler::TNode<JSTypedArray> tmp495;
    compiler::TNode<JSArrayBuffer> tmp496;
    compiler::TNode<Object> tmp497;
    compiler::TNode<Object> tmp498;
    compiler::TNode<UintPtrT> tmp499;
    compiler::TNode<Map> tmp500;
    compiler::TNode<Int32T> tmp501;
    ca_.Bind(&block5, &tmp494, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 250);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp494}, MessageTemplate::kInvalidTypedArrayLength, compiler::TNode<Object>{tmp498});
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp502;
    compiler::TNode<JSTypedArray> tmp503;
    compiler::TNode<JSArrayBuffer> tmp504;
    compiler::TNode<Object> tmp505;
    compiler::TNode<Object> tmp506;
    compiler::TNode<UintPtrT> tmp507;
    compiler::TNode<Map> tmp508;
    compiler::TNode<Int32T> tmp509;
    ca_.Bind(&block4, &tmp502, &tmp503, &tmp504, &tmp505, &tmp506, &tmp507, &tmp508, &tmp509);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 252);
    ca_.Goto(&block2, tmp502, tmp503, tmp504, tmp505, tmp506, tmp507, tmp508, tmp509);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp510;
    compiler::TNode<JSTypedArray> tmp511;
    compiler::TNode<JSArrayBuffer> tmp512;
    compiler::TNode<Object> tmp513;
    compiler::TNode<Object> tmp514;
    compiler::TNode<UintPtrT> tmp515;
    compiler::TNode<Map> tmp516;
    compiler::TNode<Int32T> tmp517;
    ca_.Bind(&block3, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 253);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp510}, MessageTemplate::kInvalidOffset, compiler::TNode<Object>{tmp513});
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp518;
    compiler::TNode<JSTypedArray> tmp519;
    compiler::TNode<JSArrayBuffer> tmp520;
    compiler::TNode<Object> tmp521;
    compiler::TNode<Object> tmp522;
    compiler::TNode<UintPtrT> tmp523;
    compiler::TNode<Map> tmp524;
    compiler::TNode<Int32T> tmp525;
    ca_.Bind(&block2, &tmp518, &tmp519, &tmp520, &tmp521, &tmp522, &tmp523, &tmp524, &tmp525);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 175);
    ca_.Goto(&block1, tmp518, tmp519, tmp520, tmp521, tmp522, tmp523, tmp524, tmp525);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp526;
    compiler::TNode<JSTypedArray> tmp527;
    compiler::TNode<JSArrayBuffer> tmp528;
    compiler::TNode<Object> tmp529;
    compiler::TNode<Object> tmp530;
    compiler::TNode<UintPtrT> tmp531;
    compiler::TNode<Map> tmp532;
    compiler::TNode<Int32T> tmp533;
    ca_.Bind(&block1, &tmp526, &tmp527, &tmp528, &tmp529, &tmp530, &tmp531, &tmp532, &tmp533);
    ca_.Goto(&block42, tmp526, tmp527, tmp528, tmp529, tmp530, tmp531, tmp532, tmp533);
  }

    compiler::TNode<Context> tmp534;
    compiler::TNode<JSTypedArray> tmp535;
    compiler::TNode<JSArrayBuffer> tmp536;
    compiler::TNode<Object> tmp537;
    compiler::TNode<Object> tmp538;
    compiler::TNode<UintPtrT> tmp539;
    compiler::TNode<Map> tmp540;
    compiler::TNode<Int32T> tmp541;
    ca_.Bind(&block42, &tmp534, &tmp535, &tmp536, &tmp537, &tmp538, &tmp539, &tmp540, &tmp541);
}

void TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::ConstructByJSReceiver(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_array, compiler::TNode<JSReceiver> p_obj, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::CodeAssemblerLabel* label_IfConstructByArrayLike, compiler::TypedCodeAssemblerVariable<HeapObject>* label_IfConstructByArrayLike_parameter_0, compiler::TypedCodeAssemblerVariable<Object>* label_IfConstructByArrayLike_parameter_1, compiler::TypedCodeAssemblerVariable<JSReceiver>* label_IfConstructByArrayLike_parameter_2) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, JSReceiver> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, JSReceiver, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object, Object, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object, JSReceiver> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object, JSReceiver, JSTypedArray, JSReceiver, JSReceiver, UintPtrT, Map, Int32T, HeapObject, Object, JSReceiver> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object, Object, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object, Smi> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<HeapObject, Object, JSReceiver> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_array, p_obj, p_elementsInfo.sizeLog2, p_elementsInfo.map, p_elementsInfo.kind);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<UintPtrT> tmp3;
    compiler::TNode<Map> tmp4;
    compiler::TNode<Int32T> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 263);
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp6 = CodeStubAssembler(state_).GetIteratorMethod(compiler::TNode<Context>{tmp0}, compiler::TNode<HeapObject>{tmp2}, &label0);
    ca_.Goto(&block6, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp2, tmp6);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp2);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSTypedArray> tmp8;
    compiler::TNode<JSReceiver> tmp9;
    compiler::TNode<UintPtrT> tmp10;
    compiler::TNode<Map> tmp11;
    compiler::TNode<Int32T> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    ca_.Bind(&block7, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block5, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<JSTypedArray> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<UintPtrT> tmp17;
    compiler::TNode<Map> tmp18;
    compiler::TNode<Int32T> tmp19;
    compiler::TNode<JSReceiver> tmp20;
    compiler::TNode<Object> tmp21;
    ca_.Bind(&block6, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 262);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 264);
    compiler::TNode<JSReceiver> tmp22;
    USE(tmp22);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp22 = BaseBuiltinsFromDSLAssembler(state_).Cast39UT15JSBoundFunction10JSFunction7JSProxy(compiler::TNode<Context>{tmp14}, compiler::TNode<Object>{tmp21}, &label0);
    ca_.Goto(&block10, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp21, tmp21, tmp22);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block11, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp21, tmp21);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSTypedArray> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<UintPtrT> tmp26;
    compiler::TNode<Map> tmp27;
    compiler::TNode<Int32T> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Object> tmp30;
    ca_.Bind(&block11, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.Goto(&block9, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<JSTypedArray> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<UintPtrT> tmp34;
    compiler::TNode<Map> tmp35;
    compiler::TNode<Int32T> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    ca_.Bind(&block10, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.Goto(&block8, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp39);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<JSTypedArray> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<UintPtrT> tmp43;
    compiler::TNode<Map> tmp44;
    compiler::TNode<Int32T> tmp45;
    compiler::TNode<Object> tmp46;
    ca_.Bind(&block9, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 265);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp40}, MessageTemplate::kIteratorSymbolNonCallable);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<JSTypedArray> tmp48;
    compiler::TNode<JSReceiver> tmp49;
    compiler::TNode<UintPtrT> tmp50;
    compiler::TNode<Map> tmp51;
    compiler::TNode<Int32T> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    ca_.Bind(&block8, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 264);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 266);
    compiler::TypedCodeAssemblerVariable<HeapObject> result_0_0(&ca_);
    compiler::TypedCodeAssemblerVariable<Object> result_0_1(&ca_);
    compiler::TypedCodeAssemblerVariable<JSReceiver> result_0_2(&ca_);
    compiler::CodeAssemblerLabel label0(&ca_);
    TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).ConstructByIterable(compiler::TNode<Context>{tmp47}, compiler::TNode<JSTypedArray>{tmp48}, compiler::TNode<JSReceiver>{tmp49}, compiler::TNode<JSReceiver>{tmp54}, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo{compiler::TNode<UintPtrT>{tmp50}, compiler::TNode<Map>{tmp51}, compiler::TNode<Int32T>{tmp52}}, &label0, &result_0_0, &result_0_1, &result_0_2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp48, tmp49, tmp54, tmp50, tmp51, tmp52, result_0_0.value(), result_0_1.value(), result_0_2.value());
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<JSTypedArray> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<UintPtrT> tmp58;
    compiler::TNode<Map> tmp59;
    compiler::TNode<Int32T> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<JSReceiver> tmp62;
    compiler::TNode<JSTypedArray> tmp63;
    compiler::TNode<JSReceiver> tmp64;
    compiler::TNode<JSReceiver> tmp65;
    compiler::TNode<UintPtrT> tmp66;
    compiler::TNode<Map> tmp67;
    compiler::TNode<Int32T> tmp68;
    compiler::TNode<HeapObject> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<JSReceiver> tmp71;
    ca_.Bind(&block12, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    ca_.Goto(&block1, tmp69, tmp70, tmp71);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<JSTypedArray> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    compiler::TNode<UintPtrT> tmp75;
    compiler::TNode<Map> tmp76;
    compiler::TNode<Int32T> tmp77;
    ca_.Bind(&block5, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 270);
    compiler::TNode<String> tmp78;
    USE(tmp78);
    tmp78 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    compiler::TNode<Object> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp72}, compiler::TNode<Object>{tmp74}, compiler::TNode<Object>{tmp78}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 271);
    compiler::TNode<Smi> tmp80;
    USE(tmp80);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp80 = CodeStubAssembler(state_).ToSmiLength(compiler::TNode<Context>{tmp72}, compiler::TNode<Object>{tmp79}, &label0);
    ca_.Goto(&block15, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp79, tmp79, tmp80);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block16, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp79, tmp79);
    }
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp81;
    compiler::TNode<JSTypedArray> tmp82;
    compiler::TNode<JSReceiver> tmp83;
    compiler::TNode<UintPtrT> tmp84;
    compiler::TNode<Map> tmp85;
    compiler::TNode<Int32T> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    ca_.Bind(&block16, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88);
    ca_.Goto(&block14, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp89;
    compiler::TNode<JSTypedArray> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<UintPtrT> tmp92;
    compiler::TNode<Map> tmp93;
    compiler::TNode<Int32T> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Smi> tmp97;
    ca_.Bind(&block15, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.Goto(&block13, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp97);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp98;
    compiler::TNode<JSTypedArray> tmp99;
    compiler::TNode<JSReceiver> tmp100;
    compiler::TNode<UintPtrT> tmp101;
    compiler::TNode<Map> tmp102;
    compiler::TNode<Int32T> tmp103;
    compiler::TNode<Object> tmp104;
    ca_.Bind(&block14, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 272);
    ca_.Goto(&block3, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp105;
    compiler::TNode<JSTypedArray> tmp106;
    compiler::TNode<JSReceiver> tmp107;
    compiler::TNode<UintPtrT> tmp108;
    compiler::TNode<Map> tmp109;
    compiler::TNode<Int32T> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Smi> tmp112;
    ca_.Bind(&block13, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 271);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 273);
    compiler::TNode<JSReceiver> tmp113;
    USE(tmp113);
    tmp113 = ca_.UncheckedCast<JSReceiver>(BaseBuiltinsFromDSLAssembler(state_).GetArrayBufferFunction(compiler::TNode<Context>{tmp105}));
    ca_.Goto(&block1, tmp107, tmp112, tmp113);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp114;
    compiler::TNode<JSTypedArray> tmp115;
    compiler::TNode<JSReceiver> tmp116;
    compiler::TNode<UintPtrT> tmp117;
    compiler::TNode<Map> tmp118;
    compiler::TNode<Int32T> tmp119;
    compiler::TNode<Object> tmp120;
    ca_.Bind(&block3, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 276);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp114}, MessageTemplate::kInvalidTypedArrayLength, compiler::TNode<Object>{tmp120});
  }

  if (block1.is_used()) {
    compiler::TNode<HeapObject> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<JSReceiver> tmp123;
    ca_.Bind(&block1, &tmp121, &tmp122, &tmp123);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 257);
    *label_IfConstructByArrayLike_parameter_2 = tmp123;
    *label_IfConstructByArrayLike_parameter_1 = tmp122;
    *label_IfConstructByArrayLike_parameter_0 = tmp121;
    ca_.Goto(label_IfConstructByArrayLike);
  }
}

TF_BUILTIN(CreateTypedArray, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<JSFunction> parameter1 = UncheckedCast<JSFunction>(Parameter(Descriptor::kTarget));
  USE(parameter1);
  compiler::TNode<JSReceiver> parameter2 = UncheckedCast<JSReceiver>(Parameter(Descriptor::kNewTarget));
  USE(parameter2);
  compiler::TNode<Object> parameter3 = UncheckedCast<Object>(Parameter(Descriptor::kArg1));
  USE(parameter3);
  compiler::TNode<Object> parameter4 = UncheckedCast<Object>(Parameter(Descriptor::kArg2));
  USE(parameter4);
  compiler::TNode<Object> parameter5 = UncheckedCast<Object>(Parameter(Descriptor::kArg3));
  USE(parameter5);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, Object, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, HeapObject> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, HeapObject, JSArrayBuffer> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, HeapObject> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, HeapObject, JSTypedArray> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, JSTypedArray, JSTypedArray, JSTypedArray, UintPtrT, Map, Int32T, HeapObject, Object, JSReceiver> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, HeapObject> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, HeapObject, JSReceiver> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object, JSReceiver, JSTypedArray, JSReceiver, UintPtrT, Map, Int32T, HeapObject, Object, JSReceiver> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T, HeapObject, Object, JSReceiver> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, JSReceiver, Object, Object, Object, JSTypedArray, UintPtrT, Map, Int32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3, parameter4, parameter5);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSFunction> tmp1;
    compiler::TNode<JSReceiver> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 288);
    compiler::TNode<JSTypedArray> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<JSTypedArray>(ConstructorBuiltinsAssembler(state_).EmitFastNewObject(compiler::TNode<Context>{tmp0}, compiler::TNode<JSFunction>{tmp1}, compiler::TNode<JSReceiver>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 294);
    compiler::TNode<IntPtrT> tmp7 = ca_.IntPtrConstant(JSArrayBufferView::kByteOffsetOffset);
    USE(tmp7);
    compiler::TNode<UintPtrT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp6, tmp7}, tmp8);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 295);
    compiler::TNode<IntPtrT> tmp9 = ca_.IntPtrConstant(JSArrayBufferView::kByteLengthOffset);
    USE(tmp9);
    compiler::TNode<UintPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(0));
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp6, tmp9}, tmp10);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 300);
    compiler::TNode<UintPtrT> tmp11;
    USE(tmp11);
    compiler::TNode<Map> tmp12;
    USE(tmp12);
    compiler::TNode<Int32T> tmp13;
    USE(tmp13);
    std::tie(tmp11, tmp12, tmp13) = TypedArrayBuiltinsAssembler(state_).GetTypedArrayElementsInfo(compiler::TNode<JSTypedArray>{tmp6}).Flatten();
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 299);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 303);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 304);
    compiler::TNode<Smi> tmp14;
    USE(tmp14);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp14 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp3}, &label0);
    ca_.Goto(&block7, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp11, tmp12, tmp13, tmp3, tmp3, tmp14);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp11, tmp12, tmp13, tmp3, tmp3);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<JSFunction> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<JSTypedArray> tmp21;
    compiler::TNode<UintPtrT> tmp22;
    compiler::TNode<Map> tmp23;
    compiler::TNode<Int32T> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    ca_.Bind(&block8, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.Goto(&block6, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<JSFunction> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<JSTypedArray> tmp33;
    compiler::TNode<UintPtrT> tmp34;
    compiler::TNode<Map> tmp35;
    compiler::TNode<Int32T> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Smi> tmp39;
    ca_.Bind(&block7, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 305);
    ca_.Goto(&block4, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp39);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<JSFunction> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<JSTypedArray> tmp46;
    compiler::TNode<UintPtrT> tmp47;
    compiler::TNode<Map> tmp48;
    compiler::TNode<Int32T> tmp49;
    compiler::TNode<Object> tmp50;
    ca_.Bind(&block6, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 307);
    compiler::TNode<JSArrayBuffer> tmp51;
    USE(tmp51);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp51 = BaseBuiltinsFromDSLAssembler(state_).Cast13JSArrayBuffer(compiler::TNode<HeapObject>{ca_.UncheckedCast<HeapObject>(tmp50)}, &label0);
    ca_.Goto(&block11, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, ca_.UncheckedCast<HeapObject>(tmp50), tmp51);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, ca_.UncheckedCast<HeapObject>(tmp50));
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<JSFunction> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<JSTypedArray> tmp58;
    compiler::TNode<UintPtrT> tmp59;
    compiler::TNode<Map> tmp60;
    compiler::TNode<Int32T> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<HeapObject> tmp63;
    ca_.Bind(&block12, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63);
    ca_.Goto(&block10, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<JSFunction> tmp65;
    compiler::TNode<JSReceiver> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<JSTypedArray> tmp70;
    compiler::TNode<UintPtrT> tmp71;
    compiler::TNode<Map> tmp72;
    compiler::TNode<Int32T> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<HeapObject> tmp75;
    compiler::TNode<JSArrayBuffer> tmp76;
    ca_.Bind(&block11, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 308);
    TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).ConstructByArrayBuffer(compiler::TNode<Context>{tmp64}, compiler::TNode<JSTypedArray>{tmp70}, compiler::TNode<JSArrayBuffer>{tmp76}, compiler::TNode<Object>{tmp68}, compiler::TNode<Object>{tmp69}, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo{compiler::TNode<UintPtrT>{tmp71}, compiler::TNode<Map>{tmp72}, compiler::TNode<Int32T>{tmp73}});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 307);
    ca_.Goto(&block9, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp77;
    compiler::TNode<JSFunction> tmp78;
    compiler::TNode<JSReceiver> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<JSTypedArray> tmp83;
    compiler::TNode<UintPtrT> tmp84;
    compiler::TNode<Map> tmp85;
    compiler::TNode<Int32T> tmp86;
    compiler::TNode<Object> tmp87;
    ca_.Bind(&block10, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 310);
    compiler::TNode<JSTypedArray> tmp88;
    USE(tmp88);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp88 = BaseBuiltinsFromDSLAssembler(state_).Cast12JSTypedArray(compiler::TNode<HeapObject>{ca_.UncheckedCast<HeapObject>(tmp87)}, &label0);
    ca_.Goto(&block15, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, ca_.UncheckedCast<HeapObject>(tmp87), tmp88);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block16, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, ca_.UncheckedCast<HeapObject>(tmp87));
    }
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp89;
    compiler::TNode<JSFunction> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<JSTypedArray> tmp95;
    compiler::TNode<UintPtrT> tmp96;
    compiler::TNode<Map> tmp97;
    compiler::TNode<Int32T> tmp98;
    compiler::TNode<Object> tmp99;
    compiler::TNode<HeapObject> tmp100;
    ca_.Bind(&block16, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100);
    ca_.Goto(&block14, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp101;
    compiler::TNode<JSFunction> tmp102;
    compiler::TNode<JSReceiver> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<JSTypedArray> tmp107;
    compiler::TNode<UintPtrT> tmp108;
    compiler::TNode<Map> tmp109;
    compiler::TNode<Int32T> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<HeapObject> tmp112;
    compiler::TNode<JSTypedArray> tmp113;
    ca_.Bind(&block15, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 311);
    compiler::TypedCodeAssemblerVariable<HeapObject> result_0_0(&ca_);
    compiler::TypedCodeAssemblerVariable<Object> result_0_1(&ca_);
    compiler::TypedCodeAssemblerVariable<JSReceiver> result_0_2(&ca_);
    compiler::CodeAssemblerLabel label0(&ca_);
    TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).ConstructByTypedArray(compiler::TNode<Context>{tmp101}, compiler::TNode<JSTypedArray>{tmp107}, compiler::TNode<JSTypedArray>{tmp113}, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo{compiler::TNode<UintPtrT>{tmp108}, compiler::TNode<Map>{tmp109}, compiler::TNode<Int32T>{tmp110}}, &label0, &result_0_0, &result_0_1, &result_0_2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block17, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp113, tmp107, tmp113, tmp108, tmp109, tmp110, result_0_0.value(), result_0_1.value(), result_0_2.value());
    }
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp114;
    compiler::TNode<JSFunction> tmp115;
    compiler::TNode<JSReceiver> tmp116;
    compiler::TNode<Object> tmp117;
    compiler::TNode<Object> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<JSTypedArray> tmp120;
    compiler::TNode<UintPtrT> tmp121;
    compiler::TNode<Map> tmp122;
    compiler::TNode<Int32T> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<JSTypedArray> tmp125;
    compiler::TNode<JSTypedArray> tmp126;
    compiler::TNode<JSTypedArray> tmp127;
    compiler::TNode<UintPtrT> tmp128;
    compiler::TNode<Map> tmp129;
    compiler::TNode<Int32T> tmp130;
    compiler::TNode<HeapObject> tmp131;
    compiler::TNode<Object> tmp132;
    compiler::TNode<JSReceiver> tmp133;
    ca_.Bind(&block17, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133);
    ca_.Goto(&block2, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp131, tmp132, tmp133);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp134;
    compiler::TNode<JSFunction> tmp135;
    compiler::TNode<JSReceiver> tmp136;
    compiler::TNode<Object> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<JSTypedArray> tmp140;
    compiler::TNode<UintPtrT> tmp141;
    compiler::TNode<Map> tmp142;
    compiler::TNode<Int32T> tmp143;
    compiler::TNode<Object> tmp144;
    ca_.Bind(&block14, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 314);
    compiler::TNode<JSReceiver> tmp145;
    USE(tmp145);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp145 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp134}, compiler::TNode<HeapObject>{ca_.UncheckedCast<HeapObject>(tmp144)}, &label0);
    ca_.Goto(&block20, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, ca_.UncheckedCast<HeapObject>(tmp144), tmp145);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block21, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, ca_.UncheckedCast<HeapObject>(tmp144));
    }
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp146;
    compiler::TNode<JSFunction> tmp147;
    compiler::TNode<JSReceiver> tmp148;
    compiler::TNode<Object> tmp149;
    compiler::TNode<Object> tmp150;
    compiler::TNode<Object> tmp151;
    compiler::TNode<JSTypedArray> tmp152;
    compiler::TNode<UintPtrT> tmp153;
    compiler::TNode<Map> tmp154;
    compiler::TNode<Int32T> tmp155;
    compiler::TNode<Object> tmp156;
    compiler::TNode<HeapObject> tmp157;
    ca_.Bind(&block21, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157);
    ca_.Goto(&block19, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp158;
    compiler::TNode<JSFunction> tmp159;
    compiler::TNode<JSReceiver> tmp160;
    compiler::TNode<Object> tmp161;
    compiler::TNode<Object> tmp162;
    compiler::TNode<Object> tmp163;
    compiler::TNode<JSTypedArray> tmp164;
    compiler::TNode<UintPtrT> tmp165;
    compiler::TNode<Map> tmp166;
    compiler::TNode<Int32T> tmp167;
    compiler::TNode<Object> tmp168;
    compiler::TNode<HeapObject> tmp169;
    compiler::TNode<JSReceiver> tmp170;
    ca_.Bind(&block20, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 315);
    compiler::TypedCodeAssemblerVariable<HeapObject> result_0_0(&ca_);
    compiler::TypedCodeAssemblerVariable<Object> result_0_1(&ca_);
    compiler::TypedCodeAssemblerVariable<JSReceiver> result_0_2(&ca_);
    compiler::CodeAssemblerLabel label0(&ca_);
    TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).ConstructByJSReceiver(compiler::TNode<Context>{tmp158}, compiler::TNode<JSTypedArray>{tmp164}, compiler::TNode<JSReceiver>{tmp170}, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo{compiler::TNode<UintPtrT>{tmp165}, compiler::TNode<Map>{tmp166}, compiler::TNode<Int32T>{tmp167}}, &label0, &result_0_0, &result_0_1, &result_0_2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block22, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp170, tmp164, tmp170, tmp165, tmp166, tmp167, result_0_0.value(), result_0_1.value(), result_0_2.value());
    }
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp171;
    compiler::TNode<JSFunction> tmp172;
    compiler::TNode<JSReceiver> tmp173;
    compiler::TNode<Object> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<Object> tmp176;
    compiler::TNode<JSTypedArray> tmp177;
    compiler::TNode<UintPtrT> tmp178;
    compiler::TNode<Map> tmp179;
    compiler::TNode<Int32T> tmp180;
    compiler::TNode<Object> tmp181;
    compiler::TNode<JSReceiver> tmp182;
    compiler::TNode<JSTypedArray> tmp183;
    compiler::TNode<JSReceiver> tmp184;
    compiler::TNode<UintPtrT> tmp185;
    compiler::TNode<Map> tmp186;
    compiler::TNode<Int32T> tmp187;
    compiler::TNode<HeapObject> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<JSReceiver> tmp190;
    ca_.Bind(&block22, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190);
    ca_.Goto(&block2, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp188, tmp189, tmp190);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp191;
    compiler::TNode<JSFunction> tmp192;
    compiler::TNode<JSReceiver> tmp193;
    compiler::TNode<Object> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<JSTypedArray> tmp197;
    compiler::TNode<UintPtrT> tmp198;
    compiler::TNode<Map> tmp199;
    compiler::TNode<Int32T> tmp200;
    compiler::TNode<Object> tmp201;
    ca_.Bind(&block19, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 320);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 321);
    ca_.Goto(&block4, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, ca_.UncheckedCast<HeapObject>(tmp201));
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp202;
    compiler::TNode<JSFunction> tmp203;
    compiler::TNode<JSReceiver> tmp204;
    compiler::TNode<Object> tmp205;
    compiler::TNode<Object> tmp206;
    compiler::TNode<Object> tmp207;
    compiler::TNode<JSTypedArray> tmp208;
    compiler::TNode<UintPtrT> tmp209;
    compiler::TNode<Map> tmp210;
    compiler::TNode<Int32T> tmp211;
    compiler::TNode<Object> tmp212;
    ca_.Bind(&block9, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 304);
    ca_.Goto(&block5, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp213;
    compiler::TNode<JSFunction> tmp214;
    compiler::TNode<JSReceiver> tmp215;
    compiler::TNode<Object> tmp216;
    compiler::TNode<Object> tmp217;
    compiler::TNode<Object> tmp218;
    compiler::TNode<JSTypedArray> tmp219;
    compiler::TNode<UintPtrT> tmp220;
    compiler::TNode<Map> tmp221;
    compiler::TNode<Int32T> tmp222;
    compiler::TNode<Object> tmp223;
    ca_.Bind(&block5, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 303);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 325);
    ca_.Goto(&block3, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp224;
    compiler::TNode<JSFunction> tmp225;
    compiler::TNode<JSReceiver> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Object> tmp229;
    compiler::TNode<JSTypedArray> tmp230;
    compiler::TNode<UintPtrT> tmp231;
    compiler::TNode<Map> tmp232;
    compiler::TNode<Int32T> tmp233;
    compiler::TNode<Object> tmp234;
    ca_.Bind(&block4, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 326);
    TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).ConstructByLength(compiler::TNode<Context>{tmp224}, compiler::TNode<JSTypedArray>{tmp230}, compiler::TNode<Object>{tmp234}, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo{compiler::TNode<UintPtrT>{tmp231}, compiler::TNode<Map>{tmp232}, compiler::TNode<Int32T>{tmp233}});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 302);
    ca_.Goto(&block3, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp235;
    compiler::TNode<JSFunction> tmp236;
    compiler::TNode<JSReceiver> tmp237;
    compiler::TNode<Object> tmp238;
    compiler::TNode<Object> tmp239;
    compiler::TNode<Object> tmp240;
    compiler::TNode<JSTypedArray> tmp241;
    compiler::TNode<UintPtrT> tmp242;
    compiler::TNode<Map> tmp243;
    compiler::TNode<Int32T> tmp244;
    ca_.Bind(&block3, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 328);
    ca_.Goto(&block1, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp245;
    compiler::TNode<JSFunction> tmp246;
    compiler::TNode<JSReceiver> tmp247;
    compiler::TNode<Object> tmp248;
    compiler::TNode<Object> tmp249;
    compiler::TNode<Object> tmp250;
    compiler::TNode<JSTypedArray> tmp251;
    compiler::TNode<UintPtrT> tmp252;
    compiler::TNode<Map> tmp253;
    compiler::TNode<Int32T> tmp254;
    compiler::TNode<HeapObject> tmp255;
    compiler::TNode<Object> tmp256;
    compiler::TNode<JSReceiver> tmp257;
    ca_.Bind(&block2, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 331);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 330);
    TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).ConstructByArrayLike(compiler::TNode<Context>{tmp245}, compiler::TNode<JSTypedArray>{tmp251}, compiler::TNode<HeapObject>{tmp255}, compiler::TNode<Object>{tmp256}, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo{compiler::TNode<UintPtrT>{tmp252}, compiler::TNode<Map>{tmp253}, compiler::TNode<Int32T>{tmp254}}, compiler::TNode<JSReceiver>{tmp257});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 302);
    ca_.Goto(&block1, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp258;
    compiler::TNode<JSFunction> tmp259;
    compiler::TNode<JSReceiver> tmp260;
    compiler::TNode<Object> tmp261;
    compiler::TNode<Object> tmp262;
    compiler::TNode<Object> tmp263;
    compiler::TNode<JSTypedArray> tmp264;
    compiler::TNode<UintPtrT> tmp265;
    compiler::TNode<Map> tmp266;
    compiler::TNode<Int32T> tmp267;
    ca_.Bind(&block1, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 333);
    CodeStubAssembler(state_).Return(tmp264);
  }
}

compiler::TNode<JSTypedArray> TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::TypedArraySpeciesCreate(compiler::TNode<Context> p_context, const char* p_methodName, int31_t p_numArgs, compiler::TNode<JSTypedArray> p_exemplar, compiler::TNode<Object> p_arg0, compiler::TNode<Object> p_arg1, compiler::TNode<Object> p_arg2) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction, JSReceiver> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction, JSReceiver, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction, JSReceiver> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction, JSReceiver, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction, JSReceiver, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSFunction, JSReceiver, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Object, Object, Object, JSTypedArray> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_exemplar, p_arg0, p_arg1, p_arg2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 340);
    compiler::TNode<JSFunction> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<JSFunction>(TypedArrayBuiltinsAssembler(state_).GetDefaultConstructor(compiler::TNode<Context>{tmp0}, compiler::TNode<JSTypedArray>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 343);
    compiler::TNode<IntPtrT> tmp6 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp6);
    compiler::TNode<Map>tmp7 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp1, tmp6});
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsPrototypeTypedArrayPrototype(compiler::TNode<Context>{tmp0}, compiler::TNode<Map>{tmp7}));
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp8}));
    ca_.Branch(tmp9, &block4, &block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSTypedArray> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<JSFunction> tmp15;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15);
    ca_.Goto(&block3, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<JSTypedArray> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<JSFunction> tmp21;
    ca_.Bind(&block5, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 344);
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsTypedArraySpeciesProtectorCellInvalid());
    ca_.Branch(tmp22, &block6, &block7, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSTypedArray> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<JSFunction> tmp28;
    ca_.Bind(&block6, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    ca_.Goto(&block3, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<JSTypedArray> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<JSFunction> tmp34;
    ca_.Bind(&block7, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 347);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 346);
    compiler::TNode<JSTypedArray> tmp35;
    tmp35 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateTypedArray, tmp29, tmp34, tmp34, tmp31, tmp32, tmp33));
    USE(tmp35);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 353);
    ca_.Goto(&block1, tmp29, tmp30, tmp31, tmp32, tmp33, tmp35);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<JSTypedArray> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<JSFunction> tmp41;
    ca_.Bind(&block3, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 357);
    compiler::TNode<JSReceiver> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).SpeciesConstructor(compiler::TNode<Context>{tmp36}, compiler::TNode<Object>{tmp37}, compiler::TNode<JSReceiver>{tmp41}));
    compiler::TNode<JSReceiver> tmp43;
    USE(tmp43);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp43 = BaseBuiltinsFromDSLAssembler(state_).Cast13ATConstructor(compiler::TNode<HeapObject>{tmp42}, &label0);
    ca_.Goto(&block10, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block11, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<JSTypedArray> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<JSFunction> tmp49;
    compiler::TNode<JSReceiver> tmp50;
    ca_.Bind(&block11, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.Goto(&block9, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<JSTypedArray> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<JSFunction> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    ca_.Bind(&block10, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    ca_.Goto(&block8, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp58);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<JSTypedArray> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Object> tmp63;
    compiler::TNode<JSFunction> tmp64;
    ca_.Bind(&block9, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 358);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<JSTypedArray> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<JSFunction> tmp70;
    compiler::TNode<JSReceiver> tmp71;
    ca_.Bind(&block8, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 362);
    compiler::TNode<Oddball> tmp72;
    USE(tmp72);
    tmp72 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 363);
    if (((CodeStubAssembler(state_).ConstexprInt31Equal(p_numArgs, 1)))) {
      ca_.Goto(&block12, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72);
    } else {
      ca_.Goto(&block13, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp73;
    compiler::TNode<JSTypedArray> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<JSFunction> tmp78;
    compiler::TNode<JSReceiver> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block12, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 364);
    compiler::TNode<JSReceiver> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).Construct(compiler::TNode<Context>{tmp73}, compiler::TNode<JSReceiver>{tmp79}, compiler::TNode<Object>{tmp75}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 363);
    ca_.Goto(&block14, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp81);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<JSTypedArray> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<JSFunction> tmp87;
    compiler::TNode<JSReceiver> tmp88;
    compiler::TNode<Object> tmp89;
    ca_.Bind(&block13, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 367);
    compiler::TNode<JSReceiver> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).Construct(compiler::TNode<Context>{tmp82}, compiler::TNode<JSReceiver>{tmp88}, compiler::TNode<Object>{tmp84}, compiler::TNode<Object>{tmp85}, compiler::TNode<Object>{tmp86}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 363);
    ca_.Goto(&block14, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp90);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp91;
    compiler::TNode<JSTypedArray> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<JSFunction> tmp96;
    compiler::TNode<JSReceiver> tmp97;
    compiler::TNode<Object> tmp98;
    ca_.Bind(&block14, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 370);
    compiler::TNode<JSTypedArray> tmp99;
    USE(tmp99);
    tmp99 = ca_.UncheckedCast<JSTypedArray>(TypedArrayBuiltinsAssembler(state_).ValidateTypedArray(compiler::TNode<Context>{tmp91}, compiler::TNode<Object>{tmp98}, p_methodName));
    ca_.Goto(&block1, tmp91, tmp92, tmp93, tmp94, tmp95, tmp99);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp100;
    compiler::TNode<JSTypedArray> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<JSTypedArray> tmp105;
    ca_.Bind(&block1, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 336);
    ca_.Goto(&block15, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105);
  }

    compiler::TNode<Context> tmp106;
    compiler::TNode<JSTypedArray> tmp107;
    compiler::TNode<Object> tmp108;
    compiler::TNode<Object> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<JSTypedArray> tmp111;
    ca_.Bind(&block15, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111);
  return compiler::TNode<JSTypedArray>{tmp111};
}

compiler::TNode<JSTypedArray> TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler::TypedArraySpeciesCreateByLength(compiler::TNode<Context> p_context, const char* p_methodName, compiler::TNode<JSTypedArray> p_exemplar, compiler::TNode<Smi> p_length) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, JSTypedArray> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, JSTypedArray> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSTypedArray, Smi, JSTypedArray> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_exemplar, p_length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSTypedArray> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 381);
    compiler::TNode<Oddball> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Oddball> tmp4;
    USE(tmp4);
    tmp4 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 380);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<JSTypedArray>(TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).TypedArraySpeciesCreate(compiler::TNode<Context>{tmp0}, p_methodName, 1, compiler::TNode<JSTypedArray>{tmp1}, compiler::TNode<Object>{tmp2}, compiler::TNode<Object>{tmp3}, compiler::TNode<Object>{tmp4}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 382);
    compiler::TNode<IntPtrT> tmp6 = ca_.IntPtrConstant(JSTypedArray::kLengthOffset);
    USE(tmp6);
    compiler::TNode<Smi>tmp7 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp5, tmp6});
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp7}, compiler::TNode<Smi>{tmp2}));
    ca_.Branch(tmp8, &block2, &block3, tmp0, tmp1, tmp2, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSTypedArray> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<JSTypedArray> tmp12;
    ca_.Bind(&block2, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 383);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp9}, MessageTemplate::kTypedArrayTooShort);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    compiler::TNode<Smi> tmp15;
    compiler::TNode<JSTypedArray> tmp16;
    ca_.Bind(&block3, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 386);
    ca_.Goto(&block1, tmp13, tmp14, tmp15, tmp16);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSTypedArray> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<JSTypedArray> tmp20;
    ca_.Bind(&block1, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-createtypedarray.tq", 374);
    ca_.Goto(&block4, tmp17, tmp18, tmp19, tmp20);
  }

    compiler::TNode<Context> tmp21;
    compiler::TNode<JSTypedArray> tmp22;
    compiler::TNode<Smi> tmp23;
    compiler::TNode<JSTypedArray> tmp24;
    ca_.Bind(&block4, &tmp21, &tmp22, &tmp23, &tmp24);
  return compiler::TNode<JSTypedArray>{tmp24};
}

}  // namespace internal
}  // namespace v8

