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

TF_BUILTIN(TypedArrayPrototypeSubArray, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSTypedArray> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT, Object, IntPtrT> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT, Object, IntPtrT> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT, UintPtrT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, UintPtrT, Map, Int32T, Smi, Smi, UintPtrT> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, UintPtrT, Map, Int32T, UintPtrT, UintPtrT> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSTypedArray, JSArrayBuffer, IntPtrT, Object, IntPtrT, Object, IntPtrT, Smi, UintPtrT, Map, Int32T, UintPtrT, UintPtrT, UintPtrT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 14);
    compiler::TNode<JSTypedArray> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Cast12JSTypedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp1, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp1);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block4, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block2, tmp3, tmp4);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<JSTypedArray> tmp9;
    ca_.Bind(&block3, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block1, tmp6, tmp7, tmp9);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    ca_.Bind(&block2, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 15);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp10}, MessageTemplate::kIncompatibleMethodReceiver, "%TypedArray%.prototype.subarray");
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<JSTypedArray> tmp14;
    ca_.Bind(&block1, &tmp12, &tmp13, &tmp14);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 18);
    compiler::TNode<JSArrayBuffer> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<JSArrayBuffer>(TypedArrayBuiltinsAssembler(state_).GetBuffer(compiler::TNode<Context>{tmp12}, compiler::TNode<JSTypedArray>{tmp14}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 21);
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadJSTypedArrayLength(compiler::TNode<JSTypedArray>{tmp14}));
    compiler::TNode<IntPtrT> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp16}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 27);
    compiler::TNode<IntPtrT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp18}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 29);
    compiler::TNode<Oddball> tmp20;
    USE(tmp20);
    tmp20 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp19}, compiler::TNode<HeapObject>{tmp20}));
    ca_.Branch(tmp21, &block5, &block6, tmp12, tmp13, tmp14, tmp15, tmp17, tmp19);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<JSTypedArray> tmp24;
    compiler::TNode<JSArrayBuffer> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<Object> tmp27;
    ca_.Bind(&block5, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    compiler::TNode<IntPtrT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Context>{tmp22}, compiler::TNode<Object>{tmp27}, compiler::TNode<IntPtrT>{tmp26}));
    ca_.Goto(&block8, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<JSTypedArray> tmp31;
    compiler::TNode<JSArrayBuffer> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    compiler::TNode<Object> tmp34;
    ca_.Bind(&block6, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    compiler::TNode<IntPtrT> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    ca_.Goto(&block7, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<JSTypedArray> tmp38;
    compiler::TNode<JSArrayBuffer> tmp39;
    compiler::TNode<IntPtrT> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<IntPtrT> tmp42;
    ca_.Bind(&block8, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    ca_.Goto(&block7, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<JSTypedArray> tmp45;
    compiler::TNode<JSArrayBuffer> tmp46;
    compiler::TNode<IntPtrT> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    ca_.Bind(&block7, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 28);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 35);
    compiler::TNode<IntPtrT> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp50}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 37);
    compiler::TNode<Oddball> tmp52;
    USE(tmp52);
    tmp52 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp51}, compiler::TNode<HeapObject>{tmp52}));
    ca_.Branch(tmp53, &block9, &block10, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp51);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<JSTypedArray> tmp56;
    compiler::TNode<JSArrayBuffer> tmp57;
    compiler::TNode<IntPtrT> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    ca_.Bind(&block9, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    compiler::TNode<IntPtrT> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Context>{tmp54}, compiler::TNode<Object>{tmp61}, compiler::TNode<IntPtrT>{tmp58}));
    ca_.Goto(&block12, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<JSTypedArray> tmp65;
    compiler::TNode<JSArrayBuffer> tmp66;
    compiler::TNode<IntPtrT> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    ca_.Bind(&block10, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70);
    ca_.Goto(&block11, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp67);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<JSTypedArray> tmp73;
    compiler::TNode<JSArrayBuffer> tmp74;
    compiler::TNode<IntPtrT> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<IntPtrT> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<IntPtrT> tmp79;
    ca_.Bind(&block12, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79);
    ca_.Goto(&block11, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<JSTypedArray> tmp82;
    compiler::TNode<JSArrayBuffer> tmp83;
    compiler::TNode<IntPtrT> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<IntPtrT> tmp88;
    ca_.Bind(&block11, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 36);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 40);
    compiler::TNode<IntPtrT> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrSub(compiler::TNode<IntPtrT>{tmp88}, compiler::TNode<IntPtrT>{tmp86}));
    compiler::TNode<IntPtrT> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<IntPtrT> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrMax(compiler::TNode<IntPtrT>{tmp89}, compiler::TNode<IntPtrT>{tmp90}));
    compiler::TNode<Smi> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert13ATPositiveSmi8ATintptr(compiler::TNode<IntPtrT>{tmp91}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 45);
    compiler::TNode<UintPtrT> tmp93;
    USE(tmp93);
    compiler::TNode<Map> tmp94;
    USE(tmp94);
    compiler::TNode<Int32T> tmp95;
    USE(tmp95);
    std::tie(tmp93, tmp94, tmp95) = TypedArrayBuiltinsAssembler(state_).GetTypedArrayElementsInfo(compiler::TNode<JSTypedArray>{tmp82}).Flatten();
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 48);
    compiler::TNode<UintPtrT> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayBufferViewByteOffset(compiler::TNode<JSArrayBufferView>{tmp82}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 51);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 52);
    compiler::TNode<Smi> tmp97;
    USE(tmp97);
    tmp97 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert13ATPositiveSmi8ATintptr(compiler::TNode<IntPtrT>{tmp86}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 11);
    compiler::TNode<UintPtrT> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr13ATPositiveSmi(compiler::TNode<Smi>{tmp97}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 12);
    compiler::TNode<UintPtrT> tmp99;
    USE(tmp99);
    tmp99 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShl(compiler::TNode<UintPtrT>{tmp98}, compiler::TNode<UintPtrT>{tmp93}));
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 15);
    compiler::TNode<UintPtrT> tmp100;
    USE(tmp100);
    tmp100 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShr(compiler::TNode<UintPtrT>{tmp99}, compiler::TNode<UintPtrT>{tmp93}));
    compiler::TNode<BoolT> tmp101;
    USE(tmp101);
    tmp101 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<UintPtrT>{tmp100}, compiler::TNode<UintPtrT>{tmp98}));
    ca_.Branch(tmp101, &block16, &block17, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp92, tmp93, tmp94, tmp95, tmp96, tmp96, tmp93, tmp94, tmp95, tmp97, tmp97, tmp98, tmp99);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<JSTypedArray> tmp104;
    compiler::TNode<JSArrayBuffer> tmp105;
    compiler::TNode<IntPtrT> tmp106;
    compiler::TNode<Object> tmp107;
    compiler::TNode<IntPtrT> tmp108;
    compiler::TNode<Object> tmp109;
    compiler::TNode<IntPtrT> tmp110;
    compiler::TNode<Smi> tmp111;
    compiler::TNode<UintPtrT> tmp112;
    compiler::TNode<Map> tmp113;
    compiler::TNode<Int32T> tmp114;
    compiler::TNode<UintPtrT> tmp115;
    compiler::TNode<UintPtrT> tmp116;
    compiler::TNode<UintPtrT> tmp117;
    compiler::TNode<Map> tmp118;
    compiler::TNode<Int32T> tmp119;
    compiler::TNode<Smi> tmp120;
    compiler::TNode<Smi> tmp121;
    compiler::TNode<UintPtrT> tmp122;
    compiler::TNode<UintPtrT> tmp123;
    ca_.Bind(&block16, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123);
    ca_.Goto(&block14, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<JSTypedArray> tmp126;
    compiler::TNode<JSArrayBuffer> tmp127;
    compiler::TNode<IntPtrT> tmp128;
    compiler::TNode<Object> tmp129;
    compiler::TNode<IntPtrT> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<IntPtrT> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<UintPtrT> tmp134;
    compiler::TNode<Map> tmp135;
    compiler::TNode<Int32T> tmp136;
    compiler::TNode<UintPtrT> tmp137;
    compiler::TNode<UintPtrT> tmp138;
    compiler::TNode<UintPtrT> tmp139;
    compiler::TNode<Map> tmp140;
    compiler::TNode<Int32T> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<Smi> tmp143;
    compiler::TNode<UintPtrT> tmp144;
    compiler::TNode<UintPtrT> tmp145;
    ca_.Bind(&block17, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145);
    ca_.SetSourcePosition("../../src/builtins/typed-array.tq", 16);
    ca_.Goto(&block15, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp145);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp146;
    compiler::TNode<Object> tmp147;
    compiler::TNode<JSTypedArray> tmp148;
    compiler::TNode<JSArrayBuffer> tmp149;
    compiler::TNode<IntPtrT> tmp150;
    compiler::TNode<Object> tmp151;
    compiler::TNode<IntPtrT> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<IntPtrT> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<UintPtrT> tmp156;
    compiler::TNode<Map> tmp157;
    compiler::TNode<Int32T> tmp158;
    compiler::TNode<UintPtrT> tmp159;
    compiler::TNode<UintPtrT> tmp160;
    compiler::TNode<UintPtrT> tmp161;
    compiler::TNode<Map> tmp162;
    compiler::TNode<Int32T> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<UintPtrT> tmp166;
    ca_.Bind(&block15, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 52);
    ca_.Goto(&block13, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp166);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp167;
    compiler::TNode<Object> tmp168;
    compiler::TNode<JSTypedArray> tmp169;
    compiler::TNode<JSArrayBuffer> tmp170;
    compiler::TNode<IntPtrT> tmp171;
    compiler::TNode<Object> tmp172;
    compiler::TNode<IntPtrT> tmp173;
    compiler::TNode<Object> tmp174;
    compiler::TNode<IntPtrT> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<UintPtrT> tmp177;
    compiler::TNode<Map> tmp178;
    compiler::TNode<Int32T> tmp179;
    compiler::TNode<UintPtrT> tmp180;
    compiler::TNode<UintPtrT> tmp181;
    ca_.Bind(&block14, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 53);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp167}, MessageTemplate::kInvalidArrayBufferLength);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp182;
    compiler::TNode<Object> tmp183;
    compiler::TNode<JSTypedArray> tmp184;
    compiler::TNode<JSArrayBuffer> tmp185;
    compiler::TNode<IntPtrT> tmp186;
    compiler::TNode<Object> tmp187;
    compiler::TNode<IntPtrT> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<IntPtrT> tmp190;
    compiler::TNode<Smi> tmp191;
    compiler::TNode<UintPtrT> tmp192;
    compiler::TNode<Map> tmp193;
    compiler::TNode<Int32T> tmp194;
    compiler::TNode<UintPtrT> tmp195;
    compiler::TNode<UintPtrT> tmp196;
    compiler::TNode<UintPtrT> tmp197;
    ca_.Bind(&block13, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 51);
    compiler::TNode<UintPtrT> tmp198;
    USE(tmp198);
    tmp198 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp196}, compiler::TNode<UintPtrT>{tmp197}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 56);
    compiler::TNode<Number> tmp199;
    USE(tmp199);
    tmp199 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATuintptr(compiler::TNode<UintPtrT>{tmp198}));
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 61);
    ca_.SetSourcePosition("../../src/builtins/typed-array-subarray.tq", 60);
    compiler::TNode<JSTypedArray> tmp200;
    USE(tmp200);
    tmp200 = ca_.UncheckedCast<JSTypedArray>(TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(state_).TypedArraySpeciesCreate(compiler::TNode<Context>{tmp182}, "%TypedArray%.prototype.subarray", 3, compiler::TNode<JSTypedArray>{tmp184}, compiler::TNode<Object>{tmp185}, compiler::TNode<Object>{tmp199}, compiler::TNode<Object>{tmp191}));
    arguments->PopAndReturn(tmp200);
  }
}

}  // namespace internal
}  // namespace v8

