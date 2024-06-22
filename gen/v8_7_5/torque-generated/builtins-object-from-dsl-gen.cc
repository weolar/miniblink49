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

compiler::TNode<JSObject> ObjectBuiltinsFromDSLAssembler::ObjectFromEntriesFastCase(compiler::TNode<Context> p_context, compiler::TNode<Object> p_iterable, compiler::CodeAssemblerLabel* label_IfSlow) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArrayBase> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArrayBase, FixedArray> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArray, Smi, JSObject, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArray, Smi, JSObject, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArray, Smi, JSObject, Smi, Object, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArray, Smi, JSObject, Smi, Object, Object, Object, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArray, Smi, JSObject, Smi, Object, Object, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArray, Smi, JSObject, Smi, Object, Object, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArray, Smi, JSObject, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, FixedArray, Smi, JSObject, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSObject> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSObject> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_iterable);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 9);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 10);
    compiler::TNode<JSArray> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Cast34ATFastJSArrayWithNoCustomIteration(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block5, tmp0, tmp1, tmp1, tmp1, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp0, tmp1, tmp1, tmp1);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block6, &tmp3, &tmp4, &tmp5, &tmp6);
    ca_.Goto(&block4, tmp3, tmp4, tmp5);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<JSArray> tmp11;
    ca_.Bind(&block5, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 12);
    compiler::TNode<IntPtrT> tmp12 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp12);
    compiler::TNode<FixedArrayBase>tmp13 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp11, tmp12});
    compiler::TNode<FixedArray> tmp14;
    USE(tmp14);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp14 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<HeapObject>{tmp13}, &label0);
    ca_.Goto(&block7, tmp7, tmp8, tmp9, tmp11, tmp13, tmp14);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp7, tmp8, tmp9, tmp11, tmp13);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<JSArray> tmp18;
    compiler::TNode<FixedArrayBase> tmp19;
    ca_.Bind(&block8, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block1);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<JSArray> tmp23;
    compiler::TNode<FixedArrayBase> tmp24;
    compiler::TNode<FixedArray> tmp25;
    ca_.Bind(&block7, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 11);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 13);
    compiler::TNode<Smi> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadFastJSArrayLength(compiler::TNode<JSArray>{tmp23}));
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 14);
    compiler::TNode<JSObject> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<JSObject>(BaseBuiltinsFromDSLAssembler(state_).NewJSObject(compiler::TNode<Context>{tmp20}));
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 16);
    compiler::TNode<Smi> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.Goto(&block11, tmp20, tmp21, tmp22, tmp23, tmp25, tmp26, tmp27, tmp28);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<JSArray> tmp32;
    compiler::TNode<FixedArray> tmp33;
    compiler::TNode<Smi> tmp34;
    compiler::TNode<JSObject> tmp35;
    compiler::TNode<Smi> tmp36;
    ca_.Bind(&block11, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    compiler::TNode<BoolT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp36}, compiler::TNode<Smi>{tmp34}));
    ca_.Branch(tmp37, &block9, &block10, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Object> tmp40;
    compiler::TNode<JSArray> tmp41;
    compiler::TNode<FixedArray> tmp42;
    compiler::TNode<Smi> tmp43;
    compiler::TNode<JSObject> tmp44;
    compiler::TNode<Smi> tmp45;
    ca_.Bind(&block9, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 17);
    compiler::TNode<Object> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Object>(ArrayBuiltinsFromDSLAssembler(state_).LoadElementOrUndefined(compiler::TNode<FixedArray>{tmp42}, compiler::TNode<Smi>{tmp45}));
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 19);
    compiler::TNode<Object> tmp47;
    USE(tmp47);
    compiler::TNode<Object> tmp48;
    USE(tmp48);
    compiler::CodeAssemblerLabel label0(&ca_);
    std::tie(tmp47, tmp48) = CollectionsBuiltinsFromDSLAssembler(state_).LoadKeyValuePairNoSideEffects(compiler::TNode<Context>{tmp38}, compiler::TNode<Object>{tmp46}, &label0).Flatten();
    ca_.Goto(&block13, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp46, tmp47, tmp48);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block14, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp46);
    }
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<JSArray> tmp52;
    compiler::TNode<FixedArray> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<JSObject> tmp55;
    compiler::TNode<Smi> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<Object> tmp58;
    ca_.Bind(&block14, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    ca_.Goto(&block1);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<JSArray> tmp62;
    compiler::TNode<FixedArray> tmp63;
    compiler::TNode<Smi> tmp64;
    compiler::TNode<JSObject> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    ca_.Bind(&block13, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 18);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 24);
    compiler::TNode<BoolT> tmp71;
    USE(tmp71);
    tmp71 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).Is10JSReceiver20UT5ATSmi10HeapObject(compiler::TNode<Context>{tmp59}, compiler::TNode<Object>{tmp69}));
    ca_.Branch(tmp71, &block15, &block16, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp69, tmp70);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<JSArray> tmp75;
    compiler::TNode<FixedArray> tmp76;
    compiler::TNode<Smi> tmp77;
    compiler::TNode<JSObject> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    ca_.Bind(&block15, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82);
    ca_.Goto(&block1);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<JSArray> tmp86;
    compiler::TNode<FixedArray> tmp87;
    compiler::TNode<Smi> tmp88;
    compiler::TNode<JSObject> tmp89;
    compiler::TNode<Smi> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Object> tmp92;
    compiler::TNode<Object> tmp93;
    ca_.Bind(&block16, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 25);
    compiler::TNode<Object> tmp94;
    tmp94 = CodeStubAssembler(state_).CallBuiltin(Builtins::kFastCreateDataProperty, tmp83, tmp89, tmp92, tmp93);
    USE(tmp94);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 16);
    ca_.Goto(&block12, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<JSArray> tmp98;
    compiler::TNode<FixedArray> tmp99;
    compiler::TNode<Smi> tmp100;
    compiler::TNode<JSObject> tmp101;
    compiler::TNode<Smi> tmp102;
    ca_.Bind(&block12, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    compiler::TNode<Smi> tmp103;
    USE(tmp103);
    tmp103 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp104;
    USE(tmp104);
    tmp104 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp102}, compiler::TNode<Smi>{tmp103}));
    ca_.Goto(&block11, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp104);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<Object> tmp107;
    compiler::TNode<JSArray> tmp108;
    compiler::TNode<FixedArray> tmp109;
    compiler::TNode<Smi> tmp110;
    compiler::TNode<JSObject> tmp111;
    compiler::TNode<Smi> tmp112;
    ca_.Bind(&block10, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 27);
    ca_.Goto(&block2, tmp105, tmp106, tmp111);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<Object> tmp115;
    ca_.Bind(&block4, &tmp113, &tmp114, &tmp115);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 29);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 30);
    ca_.Goto(&block1);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp116;
    compiler::TNode<Object> tmp117;
    compiler::TNode<JSObject> tmp118;
    ca_.Bind(&block2, &tmp116, &tmp117, &tmp118);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 7);
    ca_.Goto(&block17, tmp116, tmp117, tmp118);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_IfSlow);
  }

    compiler::TNode<Context> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<JSObject> tmp121;
    ca_.Bind(&block17, &tmp119, &tmp120, &tmp121);
  return compiler::TNode<JSObject>{tmp121};
}

TF_BUILTIN(ObjectFromEntries, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, JSObject> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Object, Map> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object, Object> block18(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object, JSReceiver, Object, Map, Object> block23(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object, JSReceiver, Object, Map> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object, JSReceiver, Object, Map, Object> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object, Object> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object, Object, Object, Map, Object> block24(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object, Object, Object, Object, Object> block25(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object, Object, Object, Object, Object, JSObject, Object, Object, Object> block26(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSObject, Map, JSReceiver, Object, Object> block14(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 38);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 40);
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNullOrUndefined(compiler::TNode<Object>{tmp6}));
    ca_.Branch(tmp7, &block5, &block6, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<RawPtrT> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    compiler::TNode<Object> tmp13;
    ca_.Bind(&block5, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block2, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    compiler::TNode<Object> tmp19;
    ca_.Bind(&block6, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 41);
    compiler::TNode<JSObject> tmp20;
    USE(tmp20);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp20 = ObjectBuiltinsFromDSLAssembler(state_).ObjectFromEntriesFastCase(compiler::TNode<Context>{tmp14}, compiler::TNode<Object>{tmp19}, &label0);
    ca_.Goto(&block7, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp19, tmp20);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp19);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<RawPtrT> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    ca_.Bind(&block8, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block4, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<RawPtrT> tmp30;
    compiler::TNode<RawPtrT> tmp31;
    compiler::TNode<IntPtrT> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<JSObject> tmp35;
    ca_.Bind(&block7, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    arguments.PopAndReturn(tmp35);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<RawPtrT> tmp38;
    compiler::TNode<RawPtrT> tmp39;
    compiler::TNode<IntPtrT> tmp40;
    compiler::TNode<Object> tmp41;
    ca_.Bind(&block4, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 44);
    compiler::TNode<JSObject> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<JSObject>(BaseBuiltinsFromDSLAssembler(state_).NewJSObject(compiler::TNode<Context>{tmp36}));
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 46);
    compiler::TNode<Context> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Context>(CodeStubAssembler(state_).LoadNativeContext(compiler::TNode<Context>{tmp36}));
    compiler::TNode<IntPtrT> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr19ATNativeContextSlot29ATconstexpr_NativeContextSlot(Context::ITERATOR_RESULT_MAP_INDEX));
    compiler::TNode<Object> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).LoadContextElement(compiler::TNode<Context>{tmp43}, compiler::TNode<IntPtrT>{tmp44}));
    compiler::TNode<Map> tmp46;
    USE(tmp46);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp46 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATMap(compiler::TNode<Context>{tmp36}, compiler::TNode<Object>{tmp45}, &label0);
    ca_.Goto(&block11, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp45, tmp46);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block12, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp45);
    }
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<RawPtrT> tmp49;
    compiler::TNode<RawPtrT> tmp50;
    compiler::TNode<IntPtrT> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<JSObject> tmp53;
    compiler::TNode<Object> tmp54;
    ca_.Bind(&block12, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54);
    ca_.Goto(&block10, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<RawPtrT> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<IntPtrT> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<JSObject> tmp61;
    compiler::TNode<Object> tmp62;
    compiler::TNode<Map> tmp63;
    ca_.Bind(&block11, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63);
    ca_.Goto(&block9, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp63);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<RawPtrT> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<IntPtrT> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<JSObject> tmp70;
    ca_.Bind(&block10, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 47);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp71;
    compiler::TNode<Object> tmp72;
    compiler::TNode<RawPtrT> tmp73;
    compiler::TNode<RawPtrT> tmp74;
    compiler::TNode<IntPtrT> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<JSObject> tmp77;
    compiler::TNode<Map> tmp78;
    ca_.Bind(&block9, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 45);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 48);
    compiler::TNode<JSReceiver> tmp79;
    USE(tmp79);
    compiler::TNode<Object> tmp80;
    USE(tmp80);
    std::tie(tmp79, tmp80) = IteratorBuiltinsAssembler(state_).GetIterator(compiler::TNode<Context>{tmp71}, compiler::TNode<Object>{tmp76}).Flatten();
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 51);
    ca_.Goto(&block17, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<RawPtrT> tmp83;
    compiler::TNode<RawPtrT> tmp84;
    compiler::TNode<IntPtrT> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<JSObject> tmp87;
    compiler::TNode<Map> tmp88;
    compiler::TNode<JSReceiver> tmp89;
    compiler::TNode<Object> tmp90;
    ca_.Bind(&block17, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90);
    compiler::TNode<BoolT> tmp91;
    USE(tmp91);
    compiler::CodeAssemblerExceptionHandlerLabel catch92_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch92_label);
    tmp91 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    }
    if (catch92_label.is_used()) {
      compiler::CodeAssemblerLabel catch92_skip(&ca_);
      ca_.Goto(&catch92_skip);
      compiler::TNode<Object> catch92_exception_object;
      ca_.Bind(&catch92_label, &catch92_exception_object);
      ca_.Goto(&block18, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, catch92_exception_object);
      ca_.Bind(&catch92_skip);
    }
    ca_.Branch(tmp91, &block15, &block16, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<RawPtrT> tmp95;
    compiler::TNode<RawPtrT> tmp96;
    compiler::TNode<IntPtrT> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<JSObject> tmp99;
    compiler::TNode<Map> tmp100;
    compiler::TNode<JSReceiver> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    ca_.Bind(&block18, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103);
    ca_.Goto(&block14, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<RawPtrT> tmp106;
    compiler::TNode<RawPtrT> tmp107;
    compiler::TNode<IntPtrT> tmp108;
    compiler::TNode<Object> tmp109;
    compiler::TNode<JSObject> tmp110;
    compiler::TNode<Map> tmp111;
    compiler::TNode<JSReceiver> tmp112;
    compiler::TNode<Object> tmp113;
    ca_.Bind(&block15, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 52);
    compiler::TNode<Object> tmp114;
    USE(tmp114);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerExceptionHandlerLabel catch115_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch115_label);
    tmp114 = IteratorBuiltinsAssembler(state_).IteratorStep(compiler::TNode<Context>{tmp104}, IteratorBuiltinsFromDSLAssembler::IteratorRecord{compiler::TNode<JSReceiver>{tmp112}, compiler::TNode<Object>{tmp113}}, compiler::TNode<Map>{tmp111}, &label0);
    }
    if (catch115_label.is_used()) {
      compiler::CodeAssemblerLabel catch115_skip(&ca_);
      ca_.Goto(&catch115_skip);
      compiler::TNode<Object> catch115_exception_object;
      ca_.Bind(&catch115_label, &catch115_exception_object);
      ca_.Goto(&block23, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp112, tmp113, tmp111, catch115_exception_object);
      ca_.Bind(&catch115_skip);
    }
    ca_.Goto(&block21, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp112, tmp113, tmp111, tmp114);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block22, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp112, tmp113, tmp111);
    }
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp116;
    compiler::TNode<Object> tmp117;
    compiler::TNode<RawPtrT> tmp118;
    compiler::TNode<RawPtrT> tmp119;
    compiler::TNode<IntPtrT> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<JSObject> tmp122;
    compiler::TNode<Map> tmp123;
    compiler::TNode<JSReceiver> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<JSReceiver> tmp126;
    compiler::TNode<Object> tmp127;
    compiler::TNode<Map> tmp128;
    compiler::TNode<Object> tmp129;
    ca_.Bind(&block23, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129);
    ca_.Goto(&block14, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp129);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<RawPtrT> tmp132;
    compiler::TNode<RawPtrT> tmp133;
    compiler::TNode<IntPtrT> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<JSObject> tmp136;
    compiler::TNode<Map> tmp137;
    compiler::TNode<JSReceiver> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<JSReceiver> tmp140;
    compiler::TNode<Object> tmp141;
    compiler::TNode<Map> tmp142;
    ca_.Bind(&block22, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142);
    ca_.Goto(&block20, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<RawPtrT> tmp145;
    compiler::TNode<RawPtrT> tmp146;
    compiler::TNode<IntPtrT> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<JSObject> tmp149;
    compiler::TNode<Map> tmp150;
    compiler::TNode<JSReceiver> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<JSReceiver> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<Map> tmp155;
    compiler::TNode<Object> tmp156;
    ca_.Bind(&block21, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156);
    ca_.Goto(&block19, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp156);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp157;
    compiler::TNode<Object> tmp158;
    compiler::TNode<RawPtrT> tmp159;
    compiler::TNode<RawPtrT> tmp160;
    compiler::TNode<IntPtrT> tmp161;
    compiler::TNode<Object> tmp162;
    compiler::TNode<JSObject> tmp163;
    compiler::TNode<Map> tmp164;
    compiler::TNode<JSReceiver> tmp165;
    compiler::TNode<Object> tmp166;
    ca_.Bind(&block20, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 53);
    arguments.PopAndReturn(tmp163);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp167;
    compiler::TNode<Object> tmp168;
    compiler::TNode<RawPtrT> tmp169;
    compiler::TNode<RawPtrT> tmp170;
    compiler::TNode<IntPtrT> tmp171;
    compiler::TNode<Object> tmp172;
    compiler::TNode<JSObject> tmp173;
    compiler::TNode<Map> tmp174;
    compiler::TNode<JSReceiver> tmp175;
    compiler::TNode<Object> tmp176;
    compiler::TNode<Object> tmp177;
    ca_.Bind(&block19, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 52);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 55);
    compiler::TNode<Object> tmp178;
    USE(tmp178);
    compiler::CodeAssemblerExceptionHandlerLabel catch179_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch179_label);
    tmp178 = ca_.UncheckedCast<Object>(IteratorBuiltinsAssembler(state_).IteratorValue(compiler::TNode<Context>{tmp167}, compiler::TNode<Object>{tmp177}, compiler::TNode<Map>{tmp174}));
    }
    if (catch179_label.is_used()) {
      compiler::CodeAssemblerLabel catch179_skip(&ca_);
      ca_.Goto(&catch179_skip);
      compiler::TNode<Object> catch179_exception_object;
      ca_.Bind(&catch179_label, &catch179_exception_object);
      ca_.Goto(&block24, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp177, tmp174, catch179_exception_object);
      ca_.Bind(&catch179_skip);
    }
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 54);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 57);
    compiler::TNode<Object> tmp180;
    USE(tmp180);
    compiler::TNode<Object> tmp181;
    USE(tmp181);
    compiler::CodeAssemblerExceptionHandlerLabel catch182_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch182_label);
    std::tie(tmp180, tmp181) = CollectionsBuiltinsFromDSLAssembler(state_).LoadKeyValuePair(compiler::TNode<Context>{tmp167}, compiler::TNode<Object>{tmp178}).Flatten();
    }
    if (catch182_label.is_used()) {
      compiler::CodeAssemblerLabel catch182_skip(&ca_);
      ca_.Goto(&catch182_skip);
      compiler::TNode<Object> catch182_exception_object;
      ca_.Bind(&catch182_label, &catch182_exception_object);
      ca_.Goto(&block25, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp178, catch182_exception_object);
      ca_.Bind(&catch182_skip);
    }
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 56);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 58);
    compiler::TNode<Object> tmp183;
    compiler::CodeAssemblerExceptionHandlerLabel catch184_label(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    { compiler::CodeAssemblerScopedExceptionHandler s(&ca_, &catch184_label);
    tmp183 = CodeStubAssembler(state_).CallBuiltin(Builtins::kFastCreateDataProperty, tmp167, tmp173, tmp180, tmp181);
    USE(tmp183);
    }
    if (catch184_label.is_used()) {
      compiler::CodeAssemblerLabel catch184_skip(&ca_);
      ca_.Goto(&catch184_skip);
      compiler::TNode<Object> catch184_exception_object;
      ca_.Bind(&catch184_label, &catch184_exception_object);
      ca_.Goto(&block26, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp180, tmp181, tmp173, tmp180, tmp181, catch184_exception_object);
      ca_.Bind(&catch184_skip);
    }
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 51);
    ca_.Goto(&block17, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp185;
    compiler::TNode<Object> tmp186;
    compiler::TNode<RawPtrT> tmp187;
    compiler::TNode<RawPtrT> tmp188;
    compiler::TNode<IntPtrT> tmp189;
    compiler::TNode<Object> tmp190;
    compiler::TNode<JSObject> tmp191;
    compiler::TNode<Map> tmp192;
    compiler::TNode<JSReceiver> tmp193;
    compiler::TNode<Object> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<Object> tmp196;
    compiler::TNode<Map> tmp197;
    compiler::TNode<Object> tmp198;
    ca_.Bind(&block24, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 55);
    ca_.Goto(&block14, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194, tmp198);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp199;
    compiler::TNode<Object> tmp200;
    compiler::TNode<RawPtrT> tmp201;
    compiler::TNode<RawPtrT> tmp202;
    compiler::TNode<IntPtrT> tmp203;
    compiler::TNode<Object> tmp204;
    compiler::TNode<JSObject> tmp205;
    compiler::TNode<Map> tmp206;
    compiler::TNode<JSReceiver> tmp207;
    compiler::TNode<Object> tmp208;
    compiler::TNode<Object> tmp209;
    compiler::TNode<Object> tmp210;
    compiler::TNode<Object> tmp211;
    compiler::TNode<Object> tmp212;
    ca_.Bind(&block25, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 57);
    ca_.Goto(&block14, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp212);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp213;
    compiler::TNode<Object> tmp214;
    compiler::TNode<RawPtrT> tmp215;
    compiler::TNode<RawPtrT> tmp216;
    compiler::TNode<IntPtrT> tmp217;
    compiler::TNode<Object> tmp218;
    compiler::TNode<JSObject> tmp219;
    compiler::TNode<Map> tmp220;
    compiler::TNode<JSReceiver> tmp221;
    compiler::TNode<Object> tmp222;
    compiler::TNode<Object> tmp223;
    compiler::TNode<Object> tmp224;
    compiler::TNode<Object> tmp225;
    compiler::TNode<Object> tmp226;
    compiler::TNode<JSObject> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Object> tmp229;
    compiler::TNode<Object> tmp230;
    ca_.Bind(&block26, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 58);
    ca_.Goto(&block14, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp230);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp231;
    compiler::TNode<Object> tmp232;
    compiler::TNode<RawPtrT> tmp233;
    compiler::TNode<RawPtrT> tmp234;
    compiler::TNode<IntPtrT> tmp235;
    compiler::TNode<Object> tmp236;
    compiler::TNode<JSObject> tmp237;
    compiler::TNode<Map> tmp238;
    compiler::TNode<JSReceiver> tmp239;
    compiler::TNode<Object> tmp240;
    ca_.Bind(&block16, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 60);
    arguments.PopAndReturn(tmp237);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp241;
    compiler::TNode<Object> tmp242;
    compiler::TNode<RawPtrT> tmp243;
    compiler::TNode<RawPtrT> tmp244;
    compiler::TNode<IntPtrT> tmp245;
    compiler::TNode<Object> tmp246;
    compiler::TNode<JSObject> tmp247;
    compiler::TNode<Map> tmp248;
    compiler::TNode<JSReceiver> tmp249;
    compiler::TNode<Object> tmp250;
    compiler::TNode<Object> tmp251;
    ca_.Bind(&block14, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 62);
    IteratorBuiltinsAssembler(state_).IteratorCloseOnException(compiler::TNode<Context>{tmp241}, IteratorBuiltinsFromDSLAssembler::IteratorRecord{compiler::TNode<JSReceiver>{tmp249}, compiler::TNode<Object>{tmp250}}, compiler::TNode<Object>{tmp251});
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp252;
    compiler::TNode<Object> tmp253;
    compiler::TNode<RawPtrT> tmp254;
    compiler::TNode<RawPtrT> tmp255;
    compiler::TNode<IntPtrT> tmp256;
    compiler::TNode<Object> tmp257;
    ca_.Bind(&block2, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257);
    ca_.SetSourcePosition("../../v8/src/builtins/object-fromentries.tq", 66);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp252}, MessageTemplate::kNotIterable);
  }
}

}  // namespace internal
}  // namespace v8

