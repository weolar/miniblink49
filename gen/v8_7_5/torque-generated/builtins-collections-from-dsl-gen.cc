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
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 11);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 12);
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
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 13);
    compiler::TNode<Smi> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadFastJSArrayLength(compiler::TNode<JSArray>{tmp11}));
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 14);
    compiler::TNode<FixedArrayBase> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp11}));
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 15);
    compiler::TNode<FixedArray> tmp14;
    USE(tmp14);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp14 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<HeapObject>{tmp13}, &label0);
    ca_.Goto(&block9, tmp7, tmp8, tmp9, tmp11, tmp12, tmp13, tmp13, tmp14);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp7, tmp8, tmp9, tmp11, tmp12, tmp13, tmp13);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<JSArray> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<FixedArrayBase> tmp20;
    compiler::TNode<FixedArrayBase> tmp21;
    ca_.Bind(&block10, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.Goto(&block8, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<JSArray> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<FixedArrayBase> tmp27;
    compiler::TNode<FixedArrayBase> tmp28;
    compiler::TNode<FixedArray> tmp29;
    ca_.Bind(&block9, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 17);
    compiler::TNode<Smi> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp26}, compiler::TNode<Smi>{tmp30}));
    ca_.Branch(tmp31, &block11, &block12, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp29);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<JSArray> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<FixedArrayBase> tmp37;
    compiler::TNode<FixedArray> tmp38;
    ca_.Bind(&block11, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    compiler::TNode<Smi> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Object>(ArrayBuiltinsFromDSLAssembler(state_).LoadElementOrUndefined(compiler::TNode<FixedArray>{tmp38}, compiler::TNode<Smi>{tmp39}));
    ca_.Goto(&block14, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp40);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<JSArray> tmp44;
    compiler::TNode<Smi> tmp45;
    compiler::TNode<FixedArrayBase> tmp46;
    compiler::TNode<FixedArray> tmp47;
    ca_.Bind(&block12, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 18);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 17);
    ca_.Goto(&block13, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<JSArray> tmp52;
    compiler::TNode<Smi> tmp53;
    compiler::TNode<FixedArrayBase> tmp54;
    compiler::TNode<FixedArray> tmp55;
    compiler::TNode<Object> tmp56;
    ca_.Bind(&block14, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
    ca_.Goto(&block13, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<JSArray> tmp60;
    compiler::TNode<Smi> tmp61;
    compiler::TNode<FixedArrayBase> tmp62;
    compiler::TNode<FixedArray> tmp63;
    compiler::TNode<Object> tmp64;
    ca_.Bind(&block13, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 19);
    compiler::TNode<Smi> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp61}, compiler::TNode<Smi>{tmp65}));
    ca_.Branch(tmp66, &block15, &block16, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<JSArray> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<FixedArrayBase> tmp72;
    compiler::TNode<FixedArray> tmp73;
    compiler::TNode<Object> tmp74;
    ca_.Bind(&block15, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    compiler::TNode<Smi> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<Object>(ArrayBuiltinsFromDSLAssembler(state_).LoadElementOrUndefined(compiler::TNode<FixedArray>{tmp73}, compiler::TNode<Smi>{tmp75}));
    ca_.Goto(&block18, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp76);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp77;
    compiler::TNode<Object> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<JSArray> tmp80;
    compiler::TNode<Smi> tmp81;
    compiler::TNode<FixedArrayBase> tmp82;
    compiler::TNode<FixedArray> tmp83;
    compiler::TNode<Object> tmp84;
    ca_.Bind(&block16, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 20);
    compiler::TNode<Oddball> tmp85;
    USE(tmp85);
    tmp85 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 19);
    ca_.Goto(&block17, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<JSArray> tmp89;
    compiler::TNode<Smi> tmp90;
    compiler::TNode<FixedArrayBase> tmp91;
    compiler::TNode<FixedArray> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Object> tmp94;
    ca_.Bind(&block18, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94);
    ca_.Goto(&block17, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<JSArray> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<FixedArrayBase> tmp100;
    compiler::TNode<FixedArray> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    ca_.Bind(&block17, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 16);
    ca_.Goto(&block2, tmp95, tmp96, tmp102, tmp103);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<JSArray> tmp107;
    compiler::TNode<Smi> tmp108;
    compiler::TNode<FixedArrayBase> tmp109;
    ca_.Bind(&block8, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 23);
    compiler::TNode<FixedDoubleArray> tmp110;
    USE(tmp110);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp110 = BaseBuiltinsFromDSLAssembler(state_).Cast16FixedDoubleArray(compiler::TNode<HeapObject>{ca_.UncheckedCast<FixedArrayBase>(tmp109)}, &label0);
    ca_.Goto(&block21, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, ca_.UncheckedCast<FixedArrayBase>(tmp109), tmp110);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block22, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, ca_.UncheckedCast<FixedArrayBase>(tmp109));
    }
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<JSArray> tmp114;
    compiler::TNode<Smi> tmp115;
    compiler::TNode<FixedArrayBase> tmp116;
    compiler::TNode<FixedArrayBase> tmp117;
    ca_.Bind(&block22, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117);
    ca_.Goto(&block20, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<JSArray> tmp121;
    compiler::TNode<Smi> tmp122;
    compiler::TNode<FixedArrayBase> tmp123;
    compiler::TNode<FixedArrayBase> tmp124;
    compiler::TNode<FixedDoubleArray> tmp125;
    ca_.Bind(&block21, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 25);
    compiler::TNode<Smi> tmp126;
    USE(tmp126);
    tmp126 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp122}, compiler::TNode<Smi>{tmp126}));
    ca_.Branch(tmp127, &block23, &block24, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp125);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp128;
    compiler::TNode<Object> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<JSArray> tmp131;
    compiler::TNode<Smi> tmp132;
    compiler::TNode<FixedArrayBase> tmp133;
    compiler::TNode<FixedDoubleArray> tmp134;
    ca_.Bind(&block23, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134);
    compiler::TNode<Smi> tmp135;
    USE(tmp135);
    tmp135 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp136;
    USE(tmp136);
    tmp136 = ca_.UncheckedCast<Object>(ArrayBuiltinsFromDSLAssembler(state_).LoadElementOrUndefined(compiler::TNode<FixedDoubleArray>{tmp134}, compiler::TNode<Smi>{tmp135}));
    ca_.Goto(&block26, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp136);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp137;
    compiler::TNode<Object> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<JSArray> tmp140;
    compiler::TNode<Smi> tmp141;
    compiler::TNode<FixedArrayBase> tmp142;
    compiler::TNode<FixedDoubleArray> tmp143;
    ca_.Bind(&block24, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 26);
    compiler::TNode<Oddball> tmp144;
    USE(tmp144);
    tmp144 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 25);
    ca_.Goto(&block25, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp145;
    compiler::TNode<Object> tmp146;
    compiler::TNode<Object> tmp147;
    compiler::TNode<JSArray> tmp148;
    compiler::TNode<Smi> tmp149;
    compiler::TNode<FixedArrayBase> tmp150;
    compiler::TNode<FixedDoubleArray> tmp151;
    compiler::TNode<Object> tmp152;
    ca_.Bind(&block26, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152);
    ca_.Goto(&block25, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<JSArray> tmp156;
    compiler::TNode<Smi> tmp157;
    compiler::TNode<FixedArrayBase> tmp158;
    compiler::TNode<FixedDoubleArray> tmp159;
    compiler::TNode<Object> tmp160;
    ca_.Bind(&block25, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 27);
    compiler::TNode<Smi> tmp161;
    USE(tmp161);
    tmp161 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp162;
    USE(tmp162);
    tmp162 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp157}, compiler::TNode<Smi>{tmp161}));
    ca_.Branch(tmp162, &block27, &block28, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp163;
    compiler::TNode<Object> tmp164;
    compiler::TNode<Object> tmp165;
    compiler::TNode<JSArray> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<FixedArrayBase> tmp168;
    compiler::TNode<FixedDoubleArray> tmp169;
    compiler::TNode<Object> tmp170;
    ca_.Bind(&block27, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170);
    compiler::TNode<Smi> tmp171;
    USE(tmp171);
    tmp171 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp172;
    USE(tmp172);
    tmp172 = ca_.UncheckedCast<Object>(ArrayBuiltinsFromDSLAssembler(state_).LoadElementOrUndefined(compiler::TNode<FixedDoubleArray>{tmp169}, compiler::TNode<Smi>{tmp171}));
    ca_.Goto(&block30, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp172);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp173;
    compiler::TNode<Object> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<JSArray> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<FixedArrayBase> tmp178;
    compiler::TNode<FixedDoubleArray> tmp179;
    compiler::TNode<Object> tmp180;
    ca_.Bind(&block28, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 28);
    compiler::TNode<Oddball> tmp181;
    USE(tmp181);
    tmp181 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 27);
    ca_.Goto(&block29, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp182;
    compiler::TNode<Object> tmp183;
    compiler::TNode<Object> tmp184;
    compiler::TNode<JSArray> tmp185;
    compiler::TNode<Smi> tmp186;
    compiler::TNode<FixedArrayBase> tmp187;
    compiler::TNode<FixedDoubleArray> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<Object> tmp190;
    ca_.Bind(&block30, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190);
    ca_.Goto(&block29, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp191;
    compiler::TNode<Object> tmp192;
    compiler::TNode<Object> tmp193;
    compiler::TNode<JSArray> tmp194;
    compiler::TNode<Smi> tmp195;
    compiler::TNode<FixedArrayBase> tmp196;
    compiler::TNode<FixedDoubleArray> tmp197;
    compiler::TNode<Object> tmp198;
    compiler::TNode<Object> tmp199;
    ca_.Bind(&block29, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 24);
    ca_.Goto(&block2, tmp191, tmp192, tmp198, tmp199);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp200;
    compiler::TNode<Object> tmp201;
    compiler::TNode<Object> tmp202;
    compiler::TNode<JSArray> tmp203;
    compiler::TNode<Smi> tmp204;
    compiler::TNode<FixedArrayBase> tmp205;
    ca_.Bind(&block20, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 31);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 32);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/collections.tq:32:13");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp206;
    compiler::TNode<Object> tmp207;
    compiler::TNode<Object> tmp208;
    ca_.Bind(&block4, &tmp206, &tmp207, &tmp208);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 36);
    compiler::TNode<JSReceiver> tmp209;
    USE(tmp209);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp209 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSReceiver(compiler::TNode<Context>{tmp206}, compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp208)}, &label0);
    ca_.Goto(&block33, tmp206, tmp207, tmp208, ca_.UncheckedCast<Object>(tmp208), tmp209);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block34, tmp206, tmp207, tmp208, ca_.UncheckedCast<Object>(tmp208));
    }
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp210;
    compiler::TNode<Object> tmp211;
    compiler::TNode<Object> tmp212;
    compiler::TNode<Object> tmp213;
    ca_.Bind(&block34, &tmp210, &tmp211, &tmp212, &tmp213);
    ca_.Goto(&block32, tmp210, tmp211, tmp212);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp214;
    compiler::TNode<Object> tmp215;
    compiler::TNode<Object> tmp216;
    compiler::TNode<Object> tmp217;
    compiler::TNode<JSReceiver> tmp218;
    ca_.Bind(&block33, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 37);
    ca_.Goto(&block1);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp219;
    compiler::TNode<Object> tmp220;
    compiler::TNode<Object> tmp221;
    ca_.Bind(&block32, &tmp219, &tmp220, &tmp221);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 39);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 40);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp219}, MessageTemplate::kIteratorValueNotAnObject, compiler::TNode<Object>{ca_.UncheckedCast<Object>(tmp221)});
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp222;
    compiler::TNode<Object> tmp223;
    compiler::TNode<Object> tmp224;
    compiler::TNode<Object> tmp225;
    ca_.Bind(&block2, &tmp222, &tmp223, &tmp224, &tmp225);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 8);
    ca_.Goto(&block35, tmp222, tmp223, tmp224, tmp225);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_MayHaveSideEffects);
  }

    compiler::TNode<Context> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Object> tmp229;
    ca_.Bind(&block35, &tmp226, &tmp227, &tmp228, &tmp229);
  return BaseBuiltinsFromDSLAssembler::KeyValuePair{compiler::TNode<Object>{tmp228}, compiler::TNode<Object>{tmp229}};
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
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 48);
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
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 52);
    compiler::TNode<Smi> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp12}, compiler::TNode<Object>{tmp13}, compiler::TNode<Object>{tmp14}));
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 53);
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp12}, compiler::TNode<Object>{tmp13}, compiler::TNode<Object>{tmp16}));
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 51);
    ca_.Goto(&block1, tmp12, tmp13, tmp15, tmp17);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    ca_.Bind(&block1, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../src/builtins/collections.tq", 45);
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

