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

BaseBuiltinsFromDSLAssembler::KeyValuePair CollectionsBuiltinsFromDSLAssembler::LoadKeyValuePairNoSideEffects(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o, compiler::CodeAssemblerLabel* label_MayHaveSideEffects) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSArray> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArrayBase> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArrayBase, FixedArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArray> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArray> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArray, Object> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArray, Object> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArray, Object> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArray, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArray, Object, Object> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArray, Object, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArrayBase> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedArrayBase, FixedDoubleArray> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedDoubleArray> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedDoubleArray> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedDoubleArray, Object> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedDoubleArray, Object> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedDoubleArray, Object> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedDoubleArray, Object> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedDoubleArray, Object, Object> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase, FixedDoubleArray, Object, Object> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray, Smi, FixedArrayBase> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSReceiver> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 11);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 12);
    compiler::TNode<JSArray> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Cast13ATFastJSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
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
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 13);
    compiler::TNode<Smi> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadFastJSArrayLength(compiler::TNode<JSArray>{tmp11}));
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 14);
    compiler::TNode<IntPtrT> tmp13 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp13);
    compiler::TNode<FixedArrayBase>tmp14 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp11, tmp13});
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 15);
    compiler::TNode<FixedArray> tmp15;
    USE(tmp15);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp15 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<HeapObject>{tmp14}, &label0);
    ca_.Goto(&block9, tmp7, tmp8, tmp9, tmp11, tmp12, tmp14, tmp14, tmp15);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp7, tmp8, tmp9, tmp11, tmp12, tmp14, tmp14);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<JSArray> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<FixedArrayBase> tmp21;
    compiler::TNode<FixedArrayBase> tmp22;
    ca_.Bind(&block10, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block8, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<JSArray> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<FixedArrayBase> tmp28;
    compiler::TNode<FixedArrayBase> tmp29;
    compiler::TNode<FixedArray> tmp30;
    ca_.Bind(&block9, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 17);
    compiler::TNode<Smi> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp31}));
    ca_.Branch(tmp32, &block11, &block12, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp30);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<JSArray> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<FixedArrayBase> tmp38;
    compiler::TNode<FixedArray> tmp39;
    ca_.Bind(&block11, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<Smi> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(ArrayBuiltinsFromDSLAssembler(state_).LoadElementOrUndefined(compiler::TNode<FixedArray>{tmp39}, compiler::TNode<Smi>{tmp40}));
    ca_.Goto(&block14, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<JSArray> tmp45;
    compiler::TNode<Smi> tmp46;
    compiler::TNode<FixedArrayBase> tmp47;
    compiler::TNode<FixedArray> tmp48;
    ca_.Bind(&block12, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 18);
    compiler::TNode<Oddball> tmp49;
    USE(tmp49);
    tmp49 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 17);
    ca_.Goto(&block13, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<JSArray> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<FixedArrayBase> tmp55;
    compiler::TNode<FixedArray> tmp56;
    compiler::TNode<Object> tmp57;
    ca_.Bind(&block14, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57);
    ca_.Goto(&block13, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<JSArray> tmp61;
    compiler::TNode<Smi> tmp62;
    compiler::TNode<FixedArrayBase> tmp63;
    compiler::TNode<FixedArray> tmp64;
    compiler::TNode<Object> tmp65;
    ca_.Bind(&block13, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 19);
    compiler::TNode<Smi> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp67;
    USE(tmp67);
    tmp67 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp62}, compiler::TNode<Smi>{tmp66}));
    ca_.Branch(tmp67, &block15, &block16, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<JSArray> tmp71;
    compiler::TNode<Smi> tmp72;
    compiler::TNode<FixedArrayBase> tmp73;
    compiler::TNode<FixedArray> tmp74;
    compiler::TNode<Object> tmp75;
    ca_.Bind(&block15, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    compiler::TNode<Smi> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<Object>(ArrayBuiltinsFromDSLAssembler(state_).LoadElementOrUndefined(compiler::TNode<FixedArray>{tmp74}, compiler::TNode<Smi>{tmp76}));
    ca_.Goto(&block18, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp77);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<JSArray> tmp81;
    compiler::TNode<Smi> tmp82;
    compiler::TNode<FixedArrayBase> tmp83;
    compiler::TNode<FixedArray> tmp84;
    compiler::TNode<Object> tmp85;
    ca_.Bind(&block16, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 20);
    compiler::TNode<Oddball> tmp86;
    USE(tmp86);
    tmp86 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 19);
    ca_.Goto(&block17, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<JSArray> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<FixedArrayBase> tmp92;
    compiler::TNode<FixedArray> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<Object> tmp95;
    ca_.Bind(&block18, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95);
    ca_.Goto(&block17, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<Object> tmp98;
    compiler::TNode<JSArray> tmp99;
    compiler::TNode<Smi> tmp100;
    compiler::TNode<FixedArrayBase> tmp101;
    compiler::TNode<FixedArray> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Object> tmp104;
    ca_.Bind(&block17, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 16);
    ca_.Goto(&block2, tmp96, tmp97, tmp103, tmp104);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<Object> tmp107;
    compiler::TNode<JSArray> tmp108;
    compiler::TNode<Smi> tmp109;
    compiler::TNode<FixedArrayBase> tmp110;
    ca_.Bind(&block8, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 23);
    compiler::TNode<FixedDoubleArray> tmp111;
    USE(tmp111);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp111 = BaseBuiltinsFromDSLAssembler(state_).Cast16FixedDoubleArray(compiler::TNode<HeapObject>{ca_.UncheckedCast<FixedArrayBase>(tmp110)}, &label0);
    ca_.Goto(&block21, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, ca_.UncheckedCast<FixedArrayBase>(tmp110), tmp111);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block22, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, ca_.UncheckedCast<FixedArrayBase>(tmp110));
    }
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<Object> tmp114;
    compiler::TNode<JSArray> tmp115;
    compiler::TNode<Smi> tmp116;
    compiler::TNode<FixedArrayBase> tmp117;
    compiler::TNode<FixedArrayBase> tmp118;
    ca_.Bind(&block22, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
    ca_.Goto(&block20, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<JSArray> tmp122;
    compiler::TNode<Smi> tmp123;
    compiler::TNode<FixedArrayBase> tmp124;
    compiler::TNode<FixedArrayBase> tmp125;
    compiler::TNode<FixedDoubleArray> tmp126;
    ca_.Bind(&block21, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 25);
    compiler::TNode<Smi> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp128;
    USE(tmp128);
    tmp128 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp123}, compiler::TNode<Smi>{tmp127}));
    ca_.Branch(tmp128, &block23, &block24, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp126);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<JSArray> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<FixedArrayBase> tmp134;
    compiler::TNode<FixedDoubleArray> tmp135;
    ca_.Bind(&block23, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135);
    compiler::TNode<Smi> tmp136;
    USE(tmp136);
    tmp136 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp137;
    USE(tmp137);
    tmp137 = ca_.UncheckedCast<Object>(ArrayBuiltinsFromDSLAssembler(state_).LoadElementOrUndefined(compiler::TNode<FixedDoubleArray>{tmp135}, compiler::TNode<Smi>{tmp136}));
    ca_.Goto(&block26, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp137);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<JSArray> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<FixedArrayBase> tmp143;
    compiler::TNode<FixedDoubleArray> tmp144;
    ca_.Bind(&block24, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 26);
    compiler::TNode<Oddball> tmp145;
    USE(tmp145);
    tmp145 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 25);
    ca_.Goto(&block25, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp146;
    compiler::TNode<Object> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<JSArray> tmp149;
    compiler::TNode<Smi> tmp150;
    compiler::TNode<FixedArrayBase> tmp151;
    compiler::TNode<FixedDoubleArray> tmp152;
    compiler::TNode<Object> tmp153;
    ca_.Bind(&block26, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153);
    ca_.Goto(&block25, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<Object> tmp156;
    compiler::TNode<JSArray> tmp157;
    compiler::TNode<Smi> tmp158;
    compiler::TNode<FixedArrayBase> tmp159;
    compiler::TNode<FixedDoubleArray> tmp160;
    compiler::TNode<Object> tmp161;
    ca_.Bind(&block25, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 27);
    compiler::TNode<Smi> tmp162;
    USE(tmp162);
    tmp162 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp163;
    USE(tmp163);
    tmp163 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp158}, compiler::TNode<Smi>{tmp162}));
    ca_.Branch(tmp163, &block27, &block28, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp164;
    compiler::TNode<Object> tmp165;
    compiler::TNode<Object> tmp166;
    compiler::TNode<JSArray> tmp167;
    compiler::TNode<Smi> tmp168;
    compiler::TNode<FixedArrayBase> tmp169;
    compiler::TNode<FixedDoubleArray> tmp170;
    compiler::TNode<Object> tmp171;
    ca_.Bind(&block27, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171);
    compiler::TNode<Smi> tmp172;
    USE(tmp172);
    tmp172 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp173;
    USE(tmp173);
    tmp173 = ca_.UncheckedCast<Object>(ArrayBuiltinsFromDSLAssembler(state_).LoadElementOrUndefined(compiler::TNode<FixedDoubleArray>{tmp170}, compiler::TNode<Smi>{tmp172}));
    ca_.Goto(&block30, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp173);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<Object> tmp176;
    compiler::TNode<JSArray> tmp177;
    compiler::TNode<Smi> tmp178;
    compiler::TNode<FixedArrayBase> tmp179;
    compiler::TNode<FixedDoubleArray> tmp180;
    compiler::TNode<Object> tmp181;
    ca_.Bind(&block28, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 28);
    compiler::TNode<Oddball> tmp182;
    USE(tmp182);
    tmp182 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 27);
    ca_.Goto(&block29, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp183;
    compiler::TNode<Object> tmp184;
    compiler::TNode<Object> tmp185;
    compiler::TNode<JSArray> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<FixedArrayBase> tmp188;
    compiler::TNode<FixedDoubleArray> tmp189;
    compiler::TNode<Object> tmp190;
    compiler::TNode<Object> tmp191;
    ca_.Bind(&block30, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191);
    ca_.Goto(&block29, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp192;
    compiler::TNode<Object> tmp193;
    compiler::TNode<Object> tmp194;
    compiler::TNode<JSArray> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<FixedArrayBase> tmp197;
    compiler::TNode<FixedDoubleArray> tmp198;
    compiler::TNode<Object> tmp199;
    compiler::TNode<Object> tmp200;
    ca_.Bind(&block29, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 24);
    ca_.Goto(&block2, tmp192, tmp193, tmp199, tmp200);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp201;
    compiler::TNode<Object> tmp202;
    compiler::TNode<Object> tmp203;
    compiler::TNode<JSArray> tmp204;
    compiler::TNode<Smi> tmp205;
    compiler::TNode<FixedArrayBase> tmp206;
    ca_.Bind(&block20, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 31);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 32);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp207;
    compiler::TNode<Object> tmp208;
    compiler::TNode<Object> tmp209;
    ca_.Bind(&block4, &tmp207, &tmp208, &tmp209);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 36);
    compiler::TNode<JSReceiver> tmp210;
    USE(tmp210);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp210 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp207}, compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp209)}, &label0);
    ca_.Goto(&block33, tmp207, tmp208, tmp209, ca_.UncheckedCast<Object>(tmp209), tmp210);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block34, tmp207, tmp208, tmp209, ca_.UncheckedCast<Object>(tmp209));
    }
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp211;
    compiler::TNode<Object> tmp212;
    compiler::TNode<Object> tmp213;
    compiler::TNode<Object> tmp214;
    ca_.Bind(&block34, &tmp211, &tmp212, &tmp213, &tmp214);
    ca_.Goto(&block32, tmp211, tmp212, tmp213);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp215;
    compiler::TNode<Object> tmp216;
    compiler::TNode<Object> tmp217;
    compiler::TNode<Object> tmp218;
    compiler::TNode<JSReceiver> tmp219;
    ca_.Bind(&block33, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 37);
    ca_.Goto(&block1);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp220;
    compiler::TNode<Object> tmp221;
    compiler::TNode<Object> tmp222;
    ca_.Bind(&block32, &tmp220, &tmp221, &tmp222);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 39);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 40);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp220}, MessageTemplate::kIteratorValueNotAnObject, compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp222)});
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp223;
    compiler::TNode<Object> tmp224;
    compiler::TNode<Object> tmp225;
    compiler::TNode<Object> tmp226;
    ca_.Bind(&block2, &tmp223, &tmp224, &tmp225, &tmp226);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 8);
    ca_.Goto(&block35, tmp223, tmp224, tmp225, tmp226);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_MayHaveSideEffects);
  }

    compiler::TNode<Context> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Object> tmp229;
    compiler::TNode<Object> tmp230;
    ca_.Bind(&block35, &tmp227, &tmp228, &tmp229, &tmp230);
  return BaseBuiltinsFromDSLAssembler::KeyValuePair{compiler::TNode<Object>{tmp229}, compiler::TNode<Object>{tmp230}};
}

BaseBuiltinsFromDSLAssembler::KeyValuePair CollectionsBuiltinsFromDSLAssembler::LoadKeyValuePair(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 48);
    compiler::TNode<Object> tmp2;
    USE(tmp2);
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    compiler::CodeAssemblerLabel label0(&ca_);
    std::tie(tmp2, tmp3) = CollectionsBuiltinsFromDSLAssembler(state_).LoadKeyValuePairNoSideEffects(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0).Flatten();
    ca_.Goto(&block4, tmp0, tmp1, tmp1, tmp2, tmp3);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp1);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<Object> tmp6;
    ca_.Bind(&block5, &tmp4, &tmp5, &tmp6);
    ca_.Goto(&block3, tmp4, tmp5);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    ca_.Bind(&block4, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.Goto(&block1, tmp7, tmp8, tmp10, tmp11);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    ca_.Bind(&block3, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 52);
    compiler::TNode<Smi> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp12}, compiler::TNode<Object>{tmp13}, compiler::TNode<Object>{tmp14}));
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 53);
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp12}, compiler::TNode<Object>{tmp13}, compiler::TNode<Object>{tmp16}));
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 51);
    ca_.Goto(&block1, tmp12, tmp13, tmp15, tmp17);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    ca_.Bind(&block1, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../v8/src/builtins/collections.tq", 45);
    ca_.Goto(&block6, tmp18, tmp19, tmp20, tmp21);
  }

    compiler::TNode<Context> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block6, &tmp22, &tmp23, &tmp24, &tmp25);
  return BaseBuiltinsFromDSLAssembler::KeyValuePair{compiler::TNode<Object>{tmp24}, compiler::TNode<Object>{tmp25}};
}

}  // namespace internal
}  // namespace v8

