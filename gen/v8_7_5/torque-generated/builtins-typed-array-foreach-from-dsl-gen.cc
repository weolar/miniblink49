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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 13);
    compiler::TNode<JSTypedArray> tmp4;
    USE(tmp4);
    compiler::TNode<JSTypedArray> tmp5;
    USE(tmp5);
    compiler::TNode<BuiltinPtr> tmp6;
    USE(tmp6);
    std::tie(tmp4, tmp5, tmp6) = TypedArrayBuiltinsFromDSLAssembler(state_).NewAttachedJSTypedArrayWitness(compiler::TNode<JSTypedArray>{tmp1}).Flatten();
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 14);
    compiler::TNode<IntPtrT> tmp7 = ca_.IntPtrConstant(JSTypedArray::kLengthOffset);
    USE(tmp7);
    compiler::TNode<Smi>tmp8 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp1, tmp7});
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi5ATSmi(compiler::TNode<Smi>{tmp8}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 15);
    compiler::TNode<Smi> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp9, tmp10);
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
    compiler::TNode<Smi> tmp19;
    ca_.Bind(&block4, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    compiler::TNode<BoolT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp19}, compiler::TNode<Smi>{tmp18}));
    ca_.Branch(tmp20, &block2, &block3, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<JSTypedArray> tmp22;
    compiler::TNode<JSReceiver> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<JSTypedArray> tmp25;
    compiler::TNode<JSTypedArray> tmp26;
    compiler::TNode<BuiltinPtr> tmp27;
    compiler::TNode<Smi> tmp28;
    compiler::TNode<Smi> tmp29;
    ca_.Bind(&block2, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 96);
    compiler::TNode<IntPtrT> tmp30 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp30);
    compiler::TNode<JSArrayBuffer>tmp31 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp25, tmp30});
    compiler::TNode<BoolT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp31}));
    ca_.Branch(tmp32, &block9, &block10, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<JSTypedArray> tmp34;
    compiler::TNode<JSReceiver> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<JSTypedArray> tmp37;
    compiler::TNode<JSTypedArray> tmp38;
    compiler::TNode<BuiltinPtr> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<Smi> tmp41;
    ca_.Bind(&block9, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.Goto(&block7, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<JSTypedArray> tmp43;
    compiler::TNode<JSReceiver> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<JSTypedArray> tmp46;
    compiler::TNode<JSTypedArray> tmp47;
    compiler::TNode<BuiltinPtr> tmp48;
    compiler::TNode<Smi> tmp49;
    compiler::TNode<Smi> tmp50;
    ca_.Bind(&block10, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 97);
    compiler::TNode<JSTypedArray> tmp51;
    USE(tmp51);
    tmp51 = (compiler::TNode<JSTypedArray>{tmp46});
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 17);
    ca_.Goto(&block8, tmp42, tmp43, tmp44, tmp45, tmp46, tmp51, tmp48, tmp49, tmp50);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp52;
    compiler::TNode<JSTypedArray> tmp53;
    compiler::TNode<JSReceiver> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<JSTypedArray> tmp56;
    compiler::TNode<JSTypedArray> tmp57;
    compiler::TNode<BuiltinPtr> tmp58;
    compiler::TNode<Smi> tmp59;
    compiler::TNode<Smi> tmp60;
    ca_.Bind(&block8, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.Goto(&block6, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<JSTypedArray> tmp62;
    compiler::TNode<JSReceiver> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<JSTypedArray> tmp65;
    compiler::TNode<JSTypedArray> tmp66;
    compiler::TNode<BuiltinPtr> tmp67;
    compiler::TNode<Smi> tmp68;
    compiler::TNode<Smi> tmp69;
    ca_.Bind(&block7, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.Goto(&block3, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<JSTypedArray> tmp71;
    compiler::TNode<JSReceiver> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<JSTypedArray> tmp74;
    compiler::TNode<JSTypedArray> tmp75;
    compiler::TNode<BuiltinPtr> tmp76;
    compiler::TNode<Smi> tmp77;
    compiler::TNode<Smi> tmp78;
    ca_.Bind(&block6, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 18);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 101);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 102);
    compiler::TNode<Object> tmp79 = CodeStubAssembler(state_).CallBuiltinPointer(Builtins::CallableFor(ca_.isolate(),ExampleBuiltinForTorqueFunctionPointerType(1)).descriptor(), tmp76, tmp70, tmp75, tmp78); 
    USE(tmp79);
    ca_.Goto(&block11, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp78, tmp70, tmp78, tmp79);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp80;
    compiler::TNode<JSTypedArray> tmp81;
    compiler::TNode<JSReceiver> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<JSTypedArray> tmp84;
    compiler::TNode<JSTypedArray> tmp85;
    compiler::TNode<BuiltinPtr> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<Smi> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<Context> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<Object> tmp92;
    ca_.Bind(&block11, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 18);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 19);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array.tq", 92);
    ca_.Goto(&block12, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp92, tmp80, tmp82, tmp83, tmp92, tmp88, tmp84);
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
    compiler::TNode<Smi> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Context> tmp103;
    compiler::TNode<JSReceiver> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<JSTypedArray> tmp108;
    ca_.Bind(&block12, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 19);
    compiler::TNode<Object> tmp109;
    USE(tmp109);
    tmp109 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).Call(compiler::TNode<Context>{tmp103}, compiler::TNode<JSReceiver>{tmp104}, compiler::TNode<Object>{tmp105}, compiler::TNode<Object>{tmp106}, compiler::TNode<Object>{tmp107}, compiler::TNode<Object>{tmp108}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 15);
    ca_.Goto(&block5, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<JSTypedArray> tmp111;
    compiler::TNode<JSReceiver> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<JSTypedArray> tmp114;
    compiler::TNode<JSTypedArray> tmp115;
    compiler::TNode<BuiltinPtr> tmp116;
    compiler::TNode<Smi> tmp117;
    compiler::TNode<Smi> tmp118;
    ca_.Bind(&block5, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
    compiler::TNode<Smi> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp120;
    USE(tmp120);
    tmp120 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp118}, compiler::TNode<Smi>{tmp119}));
    ca_.Goto(&block4, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp120);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp121;
    compiler::TNode<JSTypedArray> tmp122;
    compiler::TNode<JSReceiver> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<JSTypedArray> tmp125;
    compiler::TNode<JSTypedArray> tmp126;
    compiler::TNode<BuiltinPtr> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Smi> tmp129;
    ca_.Bind(&block3, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 21);
    compiler::TNode<Oddball> tmp130;
    USE(tmp130);
    tmp130 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block1, tmp121, tmp122, tmp123, tmp124, tmp130);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp131;
    compiler::TNode<JSTypedArray> tmp132;
    compiler::TNode<JSReceiver> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Object> tmp135;
    ca_.Bind(&block1, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 10);
    ca_.Goto(&block13, tmp131, tmp132, tmp133, tmp134, tmp135);
  }

    compiler::TNode<Context> tmp136;
    compiler::TNode<JSTypedArray> tmp137;
    compiler::TNode<JSReceiver> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<Object> tmp140;
    ca_.Bind(&block13, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140);
  return compiler::TNode<Object>{tmp140};
}

TF_BUILTIN(TypedArrayPrototypeForEach, CodeStubAssembler) {
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 32);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 34);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 36);
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
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 37);
    compiler::TNode<IntPtrT> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp48}, compiler::TNode<RawPtrT>{tmp49}, compiler::TNode<IntPtrT>{tmp50}}, compiler::TNode<IntPtrT>{tmp55}));
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 38);
    compiler::TNode<Object> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<Object>(TypedArrayForeachBuiltinsFromDSLAssembler(state_).ForEachAllElements(compiler::TNode<Context>{tmp46}, compiler::TNode<JSTypedArray>{tmp52}, compiler::TNode<JSReceiver>{tmp54}, compiler::TNode<Object>{tmp56}));
    arguments.PopAndReturn(tmp57);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<RawPtrT> tmp60;
    compiler::TNode<RawPtrT> tmp61;
    compiler::TNode<IntPtrT> tmp62;
    ca_.Bind(&block6, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 41);
    compiler::TNode<IntPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp60}, compiler::TNode<RawPtrT>{tmp61}, compiler::TNode<IntPtrT>{tmp62}}, compiler::TNode<IntPtrT>{tmp63}));
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp58}, MessageTemplate::kCalledNonCallable, compiler::TNode<Object>{tmp64});
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    ca_.Bind(&block4, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 44);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp65}, MessageTemplate::kNotTypedArray, TypedArrayForeachBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp70;
    compiler::TNode<Object> tmp71;
    compiler::TNode<RawPtrT> tmp72;
    compiler::TNode<RawPtrT> tmp73;
    compiler::TNode<IntPtrT> tmp74;
    ca_.Bind(&block2, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    ca_.SetSourcePosition("../../v8/src/builtins/typed-array-foreach.tq", 47);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp70}, MessageTemplate::kDetachedOperation, TypedArrayForeachBuiltinsFromDSLAssembler(state_).kBuiltinName());
  }
}

}  // namespace internal
}  // namespace v8

