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

compiler::TNode<Object> ArrayShiftBuiltinsFromDSLAssembler::TryFastArrayShift(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, CodeStubArguments* p_arguments, compiler::CodeAssemblerLabel* label_Slow) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Context, Map> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Context, Map, Int32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Context> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, JSArray, Smi> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, JSArray, Smi, Object> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, JSArray, Smi> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, JSArray, Smi, Object> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Smi, Context, Smi, Object> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Context, Smi, Object> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, Smi, Smi> block29(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, Smi, Smi> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, Smi, Smi> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block32(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT, FixedArrayBase> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT, FixedArrayBase, FixedDoubleArray> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT, FixedDoubleArray> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT, FixedArrayBase> block43(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT, FixedArrayBase, FixedArray> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block41(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT, FixedArray> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT, FixedArray> block44(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT, FixedArray> block45(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT, FixedArray> block46(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT, Smi, Object, IntPtrT, IntPtrT, IntPtrT, IntPtrT> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, JSArray, JSArray, Map, BoolT, BoolT, BoolT> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block47(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 11);
    compiler::TNode<JSArray> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Cast13ATFastJSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
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
    ca_.Goto(&block1);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<JSArray> tmp9;
    ca_.Bind(&block3, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 12);
    compiler::TNode<JSArray> tmp10;
    USE(tmp10);
    compiler::TNode<JSArray> tmp11;
    USE(tmp11);
    compiler::TNode<Map> tmp12;
    USE(tmp12);
    compiler::TNode<BoolT> tmp13;
    USE(tmp13);
    compiler::TNode<BoolT> tmp14;
    USE(tmp14);
    compiler::TNode<BoolT> tmp15;
    USE(tmp15);
    std::tie(tmp10, tmp11, tmp12, tmp13, tmp14, tmp15) = BaseBuiltinsFromDSLAssembler(state_).NewFastJSArrayWitness(compiler::TNode<JSArray>{tmp9}).Flatten();
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 14);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1977);
    compiler::TNode<Int32T> tmp16;
    USE(tmp16);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp16 = CodeStubAssembler(state_).EnsureArrayPushable(compiler::TNode<Map>{tmp12}, &label0);
    ca_.Goto(&block6, tmp6, tmp7, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp6, tmp12, tmp16);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp6, tmp7, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp6, tmp12);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<JSArray> tmp19;
    compiler::TNode<JSArray> tmp20;
    compiler::TNode<JSArray> tmp21;
    compiler::TNode<Map> tmp22;
    compiler::TNode<BoolT> tmp23;
    compiler::TNode<BoolT> tmp24;
    compiler::TNode<BoolT> tmp25;
    compiler::TNode<Context> tmp26;
    compiler::TNode<Map> tmp27;
    ca_.Bind(&block7, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block1);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<JSArray> tmp30;
    compiler::TNode<JSArray> tmp31;
    compiler::TNode<JSArray> tmp32;
    compiler::TNode<Map> tmp33;
    compiler::TNode<BoolT> tmp34;
    compiler::TNode<BoolT> tmp35;
    compiler::TNode<BoolT> tmp36;
    compiler::TNode<Context> tmp37;
    compiler::TNode<Map> tmp38;
    compiler::TNode<Int32T> tmp39;
    ca_.Bind(&block6, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1978);
    ArrayBuiltinsFromDSLAssembler(state_).EnsureWriteableFastElements(compiler::TNode<Context>{tmp37}, compiler::TNode<JSArray>{tmp32});
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1979);
    compiler::TNode<BoolT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 14);
    ca_.Goto(&block5, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp40, tmp37);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<JSArray> tmp43;
    compiler::TNode<JSArray> tmp44;
    compiler::TNode<JSArray> tmp45;
    compiler::TNode<Map> tmp46;
    compiler::TNode<BoolT> tmp47;
    compiler::TNode<BoolT> tmp48;
    compiler::TNode<BoolT> tmp49;
    compiler::TNode<Context> tmp50;
    ca_.Bind(&block5, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 16);
    compiler::TNode<Smi> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadFastJSArrayLength(compiler::TNode<JSArray>{tmp43}));
    compiler::TNode<Smi> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp51}, compiler::TNode<Smi>{tmp52}));
    ca_.Branch(tmp53, &block8, &block9, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<JSArray> tmp56;
    compiler::TNode<JSArray> tmp57;
    compiler::TNode<JSArray> tmp58;
    compiler::TNode<Map> tmp59;
    compiler::TNode<BoolT> tmp60;
    compiler::TNode<BoolT> tmp61;
    compiler::TNode<BoolT> tmp62;
    ca_.Bind(&block8, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 17);
    compiler::TNode<Oddball> tmp63;
    USE(tmp63);
    tmp63 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block2, tmp54, tmp55, tmp63);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<JSArray> tmp66;
    compiler::TNode<JSArray> tmp67;
    compiler::TNode<JSArray> tmp68;
    compiler::TNode<Map> tmp69;
    compiler::TNode<BoolT> tmp70;
    compiler::TNode<BoolT> tmp71;
    compiler::TNode<BoolT> tmp72;
    ca_.Bind(&block9, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 21);
    compiler::TNode<Smi> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).LoadFastJSArrayLength(compiler::TNode<JSArray>{tmp66}));
    compiler::TNode<Smi> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp73}, compiler::TNode<Smi>{tmp74}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 25);
    compiler::TNode<Smi> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp75}, compiler::TNode<Smi>{tmp75}));
    compiler::TNode<Smi> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(JSObject::kMinAddedElementsCapacity));
    compiler::TNode<Smi> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp76}, compiler::TNode<Smi>{tmp77}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 26);
    compiler::TNode<FixedArrayBase> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp66}));
    compiler::TNode<Smi> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp79}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 25);
    compiler::TNode<BoolT> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp78}, compiler::TNode<Smi>{tmp80}));
    ca_.Branch(tmp81, &block12, &block13, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp75);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<JSArray> tmp84;
    compiler::TNode<JSArray> tmp85;
    compiler::TNode<JSArray> tmp86;
    compiler::TNode<Map> tmp87;
    compiler::TNode<BoolT> tmp88;
    compiler::TNode<BoolT> tmp89;
    compiler::TNode<BoolT> tmp90;
    compiler::TNode<Smi> tmp91;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 27);
    ca_.Goto(&block11, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<JSArray> tmp94;
    compiler::TNode<JSArray> tmp95;
    compiler::TNode<JSArray> tmp96;
    compiler::TNode<Map> tmp97;
    compiler::TNode<BoolT> tmp98;
    compiler::TNode<BoolT> tmp99;
    compiler::TNode<BoolT> tmp100;
    compiler::TNode<Smi> tmp101;
    ca_.Bind(&block13, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 32);
    compiler::TNode<Smi> tmp102;
    USE(tmp102);
    tmp102 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(JSArray::kMaxCopyElements));
    compiler::TNode<BoolT> tmp103;
    USE(tmp103);
    tmp103 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp101}, compiler::TNode<Smi>{tmp102}));
    ca_.Branch(tmp103, &block14, &block15, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<JSArray> tmp106;
    compiler::TNode<JSArray> tmp107;
    compiler::TNode<JSArray> tmp108;
    compiler::TNode<Map> tmp109;
    compiler::TNode<BoolT> tmp110;
    compiler::TNode<BoolT> tmp111;
    compiler::TNode<BoolT> tmp112;
    compiler::TNode<Smi> tmp113;
    ca_.Bind(&block14, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113);
    ca_.Goto(&block11, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp114;
    compiler::TNode<Object> tmp115;
    compiler::TNode<JSArray> tmp116;
    compiler::TNode<JSArray> tmp117;
    compiler::TNode<JSArray> tmp118;
    compiler::TNode<Map> tmp119;
    compiler::TNode<BoolT> tmp120;
    compiler::TNode<BoolT> tmp121;
    compiler::TNode<BoolT> tmp122;
    compiler::TNode<Smi> tmp123;
    ca_.Bind(&block15, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 34);
    compiler::TNode<Smi> tmp124;
    USE(tmp124);
    tmp124 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1969);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1958);
    ca_.Branch(tmp120, &block20, &block21, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp114, tmp124, tmp124, tmp114, tmp124);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp125;
    compiler::TNode<Object> tmp126;
    compiler::TNode<JSArray> tmp127;
    compiler::TNode<JSArray> tmp128;
    compiler::TNode<JSArray> tmp129;
    compiler::TNode<Map> tmp130;
    compiler::TNode<BoolT> tmp131;
    compiler::TNode<BoolT> tmp132;
    compiler::TNode<BoolT> tmp133;
    compiler::TNode<Smi> tmp134;
    compiler::TNode<Context> tmp135;
    compiler::TNode<Smi> tmp136;
    compiler::TNode<Smi> tmp137;
    compiler::TNode<Context> tmp138;
    compiler::TNode<Smi> tmp139;
    ca_.Bind(&block20, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1959);
    compiler::TNode<Object> tmp140;
    USE(tmp140);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp140 = BaseBuiltinsFromDSLAssembler(state_).LoadElementNoHole16FixedDoubleArray(compiler::TNode<Context>{tmp138}, compiler::TNode<JSArray>{tmp129}, compiler::TNode<Smi>{tmp139}, &label0);
    ca_.Goto(&block23, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp129, tmp139, tmp140);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block24, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp129, tmp139);
    }
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<JSArray> tmp143;
    compiler::TNode<JSArray> tmp144;
    compiler::TNode<JSArray> tmp145;
    compiler::TNode<Map> tmp146;
    compiler::TNode<BoolT> tmp147;
    compiler::TNode<BoolT> tmp148;
    compiler::TNode<BoolT> tmp149;
    compiler::TNode<Smi> tmp150;
    compiler::TNode<Context> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<Context> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<JSArray> tmp156;
    compiler::TNode<Smi> tmp157;
    ca_.Bind(&block24, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157);
    ca_.Goto(&block18, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp158;
    compiler::TNode<Object> tmp159;
    compiler::TNode<JSArray> tmp160;
    compiler::TNode<JSArray> tmp161;
    compiler::TNode<JSArray> tmp162;
    compiler::TNode<Map> tmp163;
    compiler::TNode<BoolT> tmp164;
    compiler::TNode<BoolT> tmp165;
    compiler::TNode<BoolT> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<Context> tmp168;
    compiler::TNode<Smi> tmp169;
    compiler::TNode<Smi> tmp170;
    compiler::TNode<Context> tmp171;
    compiler::TNode<Smi> tmp172;
    compiler::TNode<JSArray> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<Object> tmp175;
    ca_.Bind(&block23, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175);
    ca_.Goto(&block19, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp175);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp176;
    compiler::TNode<Object> tmp177;
    compiler::TNode<JSArray> tmp178;
    compiler::TNode<JSArray> tmp179;
    compiler::TNode<JSArray> tmp180;
    compiler::TNode<Map> tmp181;
    compiler::TNode<BoolT> tmp182;
    compiler::TNode<BoolT> tmp183;
    compiler::TNode<BoolT> tmp184;
    compiler::TNode<Smi> tmp185;
    compiler::TNode<Context> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<Smi> tmp188;
    compiler::TNode<Context> tmp189;
    compiler::TNode<Smi> tmp190;
    ca_.Bind(&block21, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1962);
    compiler::TNode<Object> tmp191;
    USE(tmp191);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp191 = BaseBuiltinsFromDSLAssembler(state_).LoadElementNoHole10FixedArray(compiler::TNode<Context>{tmp189}, compiler::TNode<JSArray>{tmp180}, compiler::TNode<Smi>{tmp190}, &label0);
    ca_.Goto(&block25, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp180, tmp190, tmp191);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block26, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp180, tmp190);
    }
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp192;
    compiler::TNode<Object> tmp193;
    compiler::TNode<JSArray> tmp194;
    compiler::TNode<JSArray> tmp195;
    compiler::TNode<JSArray> tmp196;
    compiler::TNode<Map> tmp197;
    compiler::TNode<BoolT> tmp198;
    compiler::TNode<BoolT> tmp199;
    compiler::TNode<BoolT> tmp200;
    compiler::TNode<Smi> tmp201;
    compiler::TNode<Context> tmp202;
    compiler::TNode<Smi> tmp203;
    compiler::TNode<Smi> tmp204;
    compiler::TNode<Context> tmp205;
    compiler::TNode<Smi> tmp206;
    compiler::TNode<JSArray> tmp207;
    compiler::TNode<Smi> tmp208;
    ca_.Bind(&block26, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208);
    ca_.Goto(&block18, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp209;
    compiler::TNode<Object> tmp210;
    compiler::TNode<JSArray> tmp211;
    compiler::TNode<JSArray> tmp212;
    compiler::TNode<JSArray> tmp213;
    compiler::TNode<Map> tmp214;
    compiler::TNode<BoolT> tmp215;
    compiler::TNode<BoolT> tmp216;
    compiler::TNode<BoolT> tmp217;
    compiler::TNode<Smi> tmp218;
    compiler::TNode<Context> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<Smi> tmp221;
    compiler::TNode<Context> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<JSArray> tmp224;
    compiler::TNode<Smi> tmp225;
    compiler::TNode<Object> tmp226;
    ca_.Bind(&block25, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226);
    ca_.Goto(&block19, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp226);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<JSArray> tmp229;
    compiler::TNode<JSArray> tmp230;
    compiler::TNode<JSArray> tmp231;
    compiler::TNode<Map> tmp232;
    compiler::TNode<BoolT> tmp233;
    compiler::TNode<BoolT> tmp234;
    compiler::TNode<BoolT> tmp235;
    compiler::TNode<Smi> tmp236;
    compiler::TNode<Context> tmp237;
    compiler::TNode<Smi> tmp238;
    compiler::TNode<Smi> tmp239;
    compiler::TNode<Context> tmp240;
    compiler::TNode<Smi> tmp241;
    compiler::TNode<Object> tmp242;
    ca_.Bind(&block19, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1969);
    ca_.Goto(&block16, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp242);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp243;
    compiler::TNode<Object> tmp244;
    compiler::TNode<JSArray> tmp245;
    compiler::TNode<JSArray> tmp246;
    compiler::TNode<JSArray> tmp247;
    compiler::TNode<Map> tmp248;
    compiler::TNode<BoolT> tmp249;
    compiler::TNode<BoolT> tmp250;
    compiler::TNode<BoolT> tmp251;
    compiler::TNode<Smi> tmp252;
    compiler::TNode<Context> tmp253;
    compiler::TNode<Smi> tmp254;
    ca_.Bind(&block18, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1972);
    compiler::TNode<Oddball> tmp255;
    USE(tmp255);
    tmp255 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block16, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp256;
    compiler::TNode<Object> tmp257;
    compiler::TNode<JSArray> tmp258;
    compiler::TNode<JSArray> tmp259;
    compiler::TNode<JSArray> tmp260;
    compiler::TNode<Map> tmp261;
    compiler::TNode<BoolT> tmp262;
    compiler::TNode<BoolT> tmp263;
    compiler::TNode<BoolT> tmp264;
    compiler::TNode<Smi> tmp265;
    compiler::TNode<Context> tmp266;
    compiler::TNode<Smi> tmp267;
    compiler::TNode<Object> tmp268;
    ca_.Bind(&block16, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 34);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 35);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1983);
    ca_.Branch(tmp264, &block28, &block29, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp268, tmp265, tmp265);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp269;
    compiler::TNode<Object> tmp270;
    compiler::TNode<JSArray> tmp271;
    compiler::TNode<JSArray> tmp272;
    compiler::TNode<JSArray> tmp273;
    compiler::TNode<Map> tmp274;
    compiler::TNode<BoolT> tmp275;
    compiler::TNode<BoolT> tmp276;
    compiler::TNode<BoolT> tmp277;
    compiler::TNode<Smi> tmp278;
    compiler::TNode<Object> tmp279;
    compiler::TNode<Smi> tmp280;
    compiler::TNode<Smi> tmp281;
    ca_.Bind(&block29, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281);
    CodeStubAssembler(state_).FailAssert("Torque assert \'this.arrayIsPushable\' failed", "../../src/builtins/base.tq", 1983);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp282;
    compiler::TNode<Object> tmp283;
    compiler::TNode<JSArray> tmp284;
    compiler::TNode<JSArray> tmp285;
    compiler::TNode<JSArray> tmp286;
    compiler::TNode<Map> tmp287;
    compiler::TNode<BoolT> tmp288;
    compiler::TNode<BoolT> tmp289;
    compiler::TNode<BoolT> tmp290;
    compiler::TNode<Smi> tmp291;
    compiler::TNode<Object> tmp292;
    compiler::TNode<Smi> tmp293;
    compiler::TNode<Smi> tmp294;
    ca_.Bind(&block28, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293, &tmp294);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 1984);
    BaseBuiltinsFromDSLAssembler(state_).StoreJSArrayLength(compiler::TNode<JSArray>{tmp286}, compiler::TNode<Number>{tmp294});
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 35);
    ca_.Goto(&block27, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp291, tmp292, tmp293, tmp294);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp295;
    compiler::TNode<Object> tmp296;
    compiler::TNode<JSArray> tmp297;
    compiler::TNode<JSArray> tmp298;
    compiler::TNode<JSArray> tmp299;
    compiler::TNode<Map> tmp300;
    compiler::TNode<BoolT> tmp301;
    compiler::TNode<BoolT> tmp302;
    compiler::TNode<BoolT> tmp303;
    compiler::TNode<Smi> tmp304;
    compiler::TNode<Object> tmp305;
    compiler::TNode<Smi> tmp306;
    compiler::TNode<Smi> tmp307;
    ca_.Bind(&block27, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 36);
    compiler::TNode<IntPtrT> tmp308;
    USE(tmp308);
    tmp308 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp304}));
    compiler::TNode<IntPtrT> tmp309;
    USE(tmp309);
    tmp309 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<IntPtrT> tmp310;
    USE(tmp310);
    tmp310 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2005);
    ca_.Branch(tmp303, &block31, &block32, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp308, tmp309, tmp310, tmp308);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp311;
    compiler::TNode<Object> tmp312;
    compiler::TNode<JSArray> tmp313;
    compiler::TNode<JSArray> tmp314;
    compiler::TNode<JSArray> tmp315;
    compiler::TNode<Map> tmp316;
    compiler::TNode<BoolT> tmp317;
    compiler::TNode<BoolT> tmp318;
    compiler::TNode<BoolT> tmp319;
    compiler::TNode<Smi> tmp320;
    compiler::TNode<Object> tmp321;
    compiler::TNode<IntPtrT> tmp322;
    compiler::TNode<IntPtrT> tmp323;
    compiler::TNode<IntPtrT> tmp324;
    compiler::TNode<IntPtrT> tmp325;
    ca_.Bind(&block32, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325);
    CodeStubAssembler(state_).FailAssert("Torque assert \'this.arrayIsPushable\' failed", "../../src/builtins/base.tq", 2005);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp326;
    compiler::TNode<Object> tmp327;
    compiler::TNode<JSArray> tmp328;
    compiler::TNode<JSArray> tmp329;
    compiler::TNode<JSArray> tmp330;
    compiler::TNode<Map> tmp331;
    compiler::TNode<BoolT> tmp332;
    compiler::TNode<BoolT> tmp333;
    compiler::TNode<BoolT> tmp334;
    compiler::TNode<Smi> tmp335;
    compiler::TNode<Object> tmp336;
    compiler::TNode<IntPtrT> tmp337;
    compiler::TNode<IntPtrT> tmp338;
    compiler::TNode<IntPtrT> tmp339;
    compiler::TNode<IntPtrT> tmp340;
    ca_.Bind(&block31, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2006);
    ca_.Branch(tmp332, &block33, &block34, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331, tmp332, tmp333, tmp334, tmp335, tmp336, tmp337, tmp338, tmp339, tmp340);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp341;
    compiler::TNode<Object> tmp342;
    compiler::TNode<JSArray> tmp343;
    compiler::TNode<JSArray> tmp344;
    compiler::TNode<JSArray> tmp345;
    compiler::TNode<Map> tmp346;
    compiler::TNode<BoolT> tmp347;
    compiler::TNode<BoolT> tmp348;
    compiler::TNode<BoolT> tmp349;
    compiler::TNode<Smi> tmp350;
    compiler::TNode<Object> tmp351;
    compiler::TNode<IntPtrT> tmp352;
    compiler::TNode<IntPtrT> tmp353;
    compiler::TNode<IntPtrT> tmp354;
    compiler::TNode<IntPtrT> tmp355;
    ca_.Bind(&block33, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2008);
    compiler::TNode<FixedArrayBase> tmp356;
    USE(tmp356);
    tmp356 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp345}));
    compiler::TNode<FixedDoubleArray> tmp357;
    USE(tmp357);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp357 = BaseBuiltinsFromDSLAssembler(state_).Cast16FixedDoubleArray(compiler::TNode<HeapObject>{tmp356}, &label0);
    ca_.Goto(&block38, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, tmp357);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block39, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356);
    }
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp358;
    compiler::TNode<Object> tmp359;
    compiler::TNode<JSArray> tmp360;
    compiler::TNode<JSArray> tmp361;
    compiler::TNode<JSArray> tmp362;
    compiler::TNode<Map> tmp363;
    compiler::TNode<BoolT> tmp364;
    compiler::TNode<BoolT> tmp365;
    compiler::TNode<BoolT> tmp366;
    compiler::TNode<Smi> tmp367;
    compiler::TNode<Object> tmp368;
    compiler::TNode<IntPtrT> tmp369;
    compiler::TNode<IntPtrT> tmp370;
    compiler::TNode<IntPtrT> tmp371;
    compiler::TNode<IntPtrT> tmp372;
    compiler::TNode<FixedArrayBase> tmp373;
    ca_.Bind(&block39, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373);
    ca_.Goto(&block37, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp371, tmp372);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp374;
    compiler::TNode<Object> tmp375;
    compiler::TNode<JSArray> tmp376;
    compiler::TNode<JSArray> tmp377;
    compiler::TNode<JSArray> tmp378;
    compiler::TNode<Map> tmp379;
    compiler::TNode<BoolT> tmp380;
    compiler::TNode<BoolT> tmp381;
    compiler::TNode<BoolT> tmp382;
    compiler::TNode<Smi> tmp383;
    compiler::TNode<Object> tmp384;
    compiler::TNode<IntPtrT> tmp385;
    compiler::TNode<IntPtrT> tmp386;
    compiler::TNode<IntPtrT> tmp387;
    compiler::TNode<IntPtrT> tmp388;
    compiler::TNode<FixedArrayBase> tmp389;
    compiler::TNode<FixedDoubleArray> tmp390;
    ca_.Bind(&block38, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390);
    ca_.Goto(&block36, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384, tmp385, tmp386, tmp387, tmp388, tmp390);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp391;
    compiler::TNode<Object> tmp392;
    compiler::TNode<JSArray> tmp393;
    compiler::TNode<JSArray> tmp394;
    compiler::TNode<JSArray> tmp395;
    compiler::TNode<Map> tmp396;
    compiler::TNode<BoolT> tmp397;
    compiler::TNode<BoolT> tmp398;
    compiler::TNode<BoolT> tmp399;
    compiler::TNode<Smi> tmp400;
    compiler::TNode<Object> tmp401;
    compiler::TNode<IntPtrT> tmp402;
    compiler::TNode<IntPtrT> tmp403;
    compiler::TNode<IntPtrT> tmp404;
    compiler::TNode<IntPtrT> tmp405;
    ca_.Bind(&block37, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2009);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/base.tq:2009:21");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp406;
    compiler::TNode<Object> tmp407;
    compiler::TNode<JSArray> tmp408;
    compiler::TNode<JSArray> tmp409;
    compiler::TNode<JSArray> tmp410;
    compiler::TNode<Map> tmp411;
    compiler::TNode<BoolT> tmp412;
    compiler::TNode<BoolT> tmp413;
    compiler::TNode<BoolT> tmp414;
    compiler::TNode<Smi> tmp415;
    compiler::TNode<Object> tmp416;
    compiler::TNode<IntPtrT> tmp417;
    compiler::TNode<IntPtrT> tmp418;
    compiler::TNode<IntPtrT> tmp419;
    compiler::TNode<IntPtrT> tmp420;
    compiler::TNode<FixedDoubleArray> tmp421;
    ca_.Bind(&block36, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420, &tmp421);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2007);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2010);
    BaseBuiltinsFromDSLAssembler(state_).TorqueMoveElements(compiler::TNode<FixedDoubleArray>{tmp421}, compiler::TNode<IntPtrT>{tmp418}, compiler::TNode<IntPtrT>{tmp419}, compiler::TNode<IntPtrT>{tmp420});
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2006);
    ca_.Goto(&block35, tmp406, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp422;
    compiler::TNode<Object> tmp423;
    compiler::TNode<JSArray> tmp424;
    compiler::TNode<JSArray> tmp425;
    compiler::TNode<JSArray> tmp426;
    compiler::TNode<Map> tmp427;
    compiler::TNode<BoolT> tmp428;
    compiler::TNode<BoolT> tmp429;
    compiler::TNode<BoolT> tmp430;
    compiler::TNode<Smi> tmp431;
    compiler::TNode<Object> tmp432;
    compiler::TNode<IntPtrT> tmp433;
    compiler::TNode<IntPtrT> tmp434;
    compiler::TNode<IntPtrT> tmp435;
    compiler::TNode<IntPtrT> tmp436;
    ca_.Bind(&block34, &tmp422, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2012);
    compiler::TNode<FixedArrayBase> tmp437;
    USE(tmp437);
    tmp437 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp426}));
    compiler::TNode<FixedArray> tmp438;
    USE(tmp438);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp438 = BaseBuiltinsFromDSLAssembler(state_).Cast10FixedArray(compiler::TNode<HeapObject>{tmp437}, &label0);
    ca_.Goto(&block42, tmp422, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block43, tmp422, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437);
    }
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp439;
    compiler::TNode<Object> tmp440;
    compiler::TNode<JSArray> tmp441;
    compiler::TNode<JSArray> tmp442;
    compiler::TNode<JSArray> tmp443;
    compiler::TNode<Map> tmp444;
    compiler::TNode<BoolT> tmp445;
    compiler::TNode<BoolT> tmp446;
    compiler::TNode<BoolT> tmp447;
    compiler::TNode<Smi> tmp448;
    compiler::TNode<Object> tmp449;
    compiler::TNode<IntPtrT> tmp450;
    compiler::TNode<IntPtrT> tmp451;
    compiler::TNode<IntPtrT> tmp452;
    compiler::TNode<IntPtrT> tmp453;
    compiler::TNode<FixedArrayBase> tmp454;
    ca_.Bind(&block43, &tmp439, &tmp440, &tmp441, &tmp442, &tmp443, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454);
    ca_.Goto(&block41, tmp439, tmp440, tmp441, tmp442, tmp443, tmp444, tmp445, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452, tmp453);
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp455;
    compiler::TNode<Object> tmp456;
    compiler::TNode<JSArray> tmp457;
    compiler::TNode<JSArray> tmp458;
    compiler::TNode<JSArray> tmp459;
    compiler::TNode<Map> tmp460;
    compiler::TNode<BoolT> tmp461;
    compiler::TNode<BoolT> tmp462;
    compiler::TNode<BoolT> tmp463;
    compiler::TNode<Smi> tmp464;
    compiler::TNode<Object> tmp465;
    compiler::TNode<IntPtrT> tmp466;
    compiler::TNode<IntPtrT> tmp467;
    compiler::TNode<IntPtrT> tmp468;
    compiler::TNode<IntPtrT> tmp469;
    compiler::TNode<FixedArrayBase> tmp470;
    compiler::TNode<FixedArray> tmp471;
    ca_.Bind(&block42, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463, &tmp464, &tmp465, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471);
    ca_.Goto(&block40, tmp455, tmp456, tmp457, tmp458, tmp459, tmp460, tmp461, tmp462, tmp463, tmp464, tmp465, tmp466, tmp467, tmp468, tmp469, tmp471);
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp472;
    compiler::TNode<Object> tmp473;
    compiler::TNode<JSArray> tmp474;
    compiler::TNode<JSArray> tmp475;
    compiler::TNode<JSArray> tmp476;
    compiler::TNode<Map> tmp477;
    compiler::TNode<BoolT> tmp478;
    compiler::TNode<BoolT> tmp479;
    compiler::TNode<BoolT> tmp480;
    compiler::TNode<Smi> tmp481;
    compiler::TNode<Object> tmp482;
    compiler::TNode<IntPtrT> tmp483;
    compiler::TNode<IntPtrT> tmp484;
    compiler::TNode<IntPtrT> tmp485;
    compiler::TNode<IntPtrT> tmp486;
    ca_.Bind(&block41, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478, &tmp479, &tmp480, &tmp481, &tmp482, &tmp483, &tmp484, &tmp485, &tmp486);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2013);
    CodeStubAssembler(state_).Print("halting because of \'unreachable\' at ../../src/builtins/base.tq:2013:21");
    CodeStubAssembler(state_).Unreachable();
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp487;
    compiler::TNode<Object> tmp488;
    compiler::TNode<JSArray> tmp489;
    compiler::TNode<JSArray> tmp490;
    compiler::TNode<JSArray> tmp491;
    compiler::TNode<Map> tmp492;
    compiler::TNode<BoolT> tmp493;
    compiler::TNode<BoolT> tmp494;
    compiler::TNode<BoolT> tmp495;
    compiler::TNode<Smi> tmp496;
    compiler::TNode<Object> tmp497;
    compiler::TNode<IntPtrT> tmp498;
    compiler::TNode<IntPtrT> tmp499;
    compiler::TNode<IntPtrT> tmp500;
    compiler::TNode<IntPtrT> tmp501;
    compiler::TNode<FixedArray> tmp502;
    ca_.Bind(&block40, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501, &tmp502);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2012);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2014);
    ca_.Branch(tmp494, &block44, &block45, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492, tmp493, tmp494, tmp495, tmp496, tmp497, tmp498, tmp499, tmp500, tmp501, tmp502);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp503;
    compiler::TNode<Object> tmp504;
    compiler::TNode<JSArray> tmp505;
    compiler::TNode<JSArray> tmp506;
    compiler::TNode<JSArray> tmp507;
    compiler::TNode<Map> tmp508;
    compiler::TNode<BoolT> tmp509;
    compiler::TNode<BoolT> tmp510;
    compiler::TNode<BoolT> tmp511;
    compiler::TNode<Smi> tmp512;
    compiler::TNode<Object> tmp513;
    compiler::TNode<IntPtrT> tmp514;
    compiler::TNode<IntPtrT> tmp515;
    compiler::TNode<IntPtrT> tmp516;
    compiler::TNode<IntPtrT> tmp517;
    compiler::TNode<FixedArray> tmp518;
    ca_.Bind(&block44, &tmp503, &tmp504, &tmp505, &tmp506, &tmp507, &tmp508, &tmp509, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2015);
    BaseBuiltinsFromDSLAssembler(state_).TorqueMoveElementsSmi(compiler::TNode<FixedArray>{tmp518}, compiler::TNode<IntPtrT>{tmp515}, compiler::TNode<IntPtrT>{tmp516}, compiler::TNode<IntPtrT>{tmp517});
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2014);
    ca_.Goto(&block46, tmp503, tmp504, tmp505, tmp506, tmp507, tmp508, tmp509, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517, tmp518);
  }

  if (block45.is_used()) {
    compiler::TNode<Context> tmp519;
    compiler::TNode<Object> tmp520;
    compiler::TNode<JSArray> tmp521;
    compiler::TNode<JSArray> tmp522;
    compiler::TNode<JSArray> tmp523;
    compiler::TNode<Map> tmp524;
    compiler::TNode<BoolT> tmp525;
    compiler::TNode<BoolT> tmp526;
    compiler::TNode<BoolT> tmp527;
    compiler::TNode<Smi> tmp528;
    compiler::TNode<Object> tmp529;
    compiler::TNode<IntPtrT> tmp530;
    compiler::TNode<IntPtrT> tmp531;
    compiler::TNode<IntPtrT> tmp532;
    compiler::TNode<IntPtrT> tmp533;
    compiler::TNode<FixedArray> tmp534;
    ca_.Bind(&block45, &tmp519, &tmp520, &tmp521, &tmp522, &tmp523, &tmp524, &tmp525, &tmp526, &tmp527, &tmp528, &tmp529, &tmp530, &tmp531, &tmp532, &tmp533, &tmp534);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2017);
    BaseBuiltinsFromDSLAssembler(state_).TorqueMoveElements(compiler::TNode<FixedArray>{tmp534}, compiler::TNode<IntPtrT>{tmp531}, compiler::TNode<IntPtrT>{tmp532}, compiler::TNode<IntPtrT>{tmp533});
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2014);
    ca_.Goto(&block46, tmp519, tmp520, tmp521, tmp522, tmp523, tmp524, tmp525, tmp526, tmp527, tmp528, tmp529, tmp530, tmp531, tmp532, tmp533, tmp534);
  }

  if (block46.is_used()) {
    compiler::TNode<Context> tmp535;
    compiler::TNode<Object> tmp536;
    compiler::TNode<JSArray> tmp537;
    compiler::TNode<JSArray> tmp538;
    compiler::TNode<JSArray> tmp539;
    compiler::TNode<Map> tmp540;
    compiler::TNode<BoolT> tmp541;
    compiler::TNode<BoolT> tmp542;
    compiler::TNode<BoolT> tmp543;
    compiler::TNode<Smi> tmp544;
    compiler::TNode<Object> tmp545;
    compiler::TNode<IntPtrT> tmp546;
    compiler::TNode<IntPtrT> tmp547;
    compiler::TNode<IntPtrT> tmp548;
    compiler::TNode<IntPtrT> tmp549;
    compiler::TNode<FixedArray> tmp550;
    ca_.Bind(&block46, &tmp535, &tmp536, &tmp537, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544, &tmp545, &tmp546, &tmp547, &tmp548, &tmp549, &tmp550);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2011);
    ca_.SetSourcePosition("../../src/builtins/base.tq", 2006);
    ca_.Goto(&block35, tmp535, tmp536, tmp537, tmp538, tmp539, tmp540, tmp541, tmp542, tmp543, tmp544, tmp545, tmp546, tmp547, tmp548, tmp549);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp551;
    compiler::TNode<Object> tmp552;
    compiler::TNode<JSArray> tmp553;
    compiler::TNode<JSArray> tmp554;
    compiler::TNode<JSArray> tmp555;
    compiler::TNode<Map> tmp556;
    compiler::TNode<BoolT> tmp557;
    compiler::TNode<BoolT> tmp558;
    compiler::TNode<BoolT> tmp559;
    compiler::TNode<Smi> tmp560;
    compiler::TNode<Object> tmp561;
    compiler::TNode<IntPtrT> tmp562;
    compiler::TNode<IntPtrT> tmp563;
    compiler::TNode<IntPtrT> tmp564;
    compiler::TNode<IntPtrT> tmp565;
    ca_.Bind(&block35, &tmp551, &tmp552, &tmp553, &tmp554, &tmp555, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563, &tmp564, &tmp565);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 36);
    ca_.Goto(&block30, tmp551, tmp552, tmp553, tmp554, tmp555, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562, tmp563, tmp564, tmp565);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp566;
    compiler::TNode<Object> tmp567;
    compiler::TNode<JSArray> tmp568;
    compiler::TNode<JSArray> tmp569;
    compiler::TNode<JSArray> tmp570;
    compiler::TNode<Map> tmp571;
    compiler::TNode<BoolT> tmp572;
    compiler::TNode<BoolT> tmp573;
    compiler::TNode<BoolT> tmp574;
    compiler::TNode<Smi> tmp575;
    compiler::TNode<Object> tmp576;
    compiler::TNode<IntPtrT> tmp577;
    compiler::TNode<IntPtrT> tmp578;
    compiler::TNode<IntPtrT> tmp579;
    compiler::TNode<IntPtrT> tmp580;
    ca_.Bind(&block30, &tmp566, &tmp567, &tmp568, &tmp569, &tmp570, &tmp571, &tmp572, &tmp573, &tmp574, &tmp575, &tmp576, &tmp577, &tmp578, &tmp579, &tmp580);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 37);
    ca_.Goto(&block2, tmp566, tmp567, tmp576);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp581;
    compiler::TNode<Object> tmp582;
    compiler::TNode<JSArray> tmp583;
    compiler::TNode<JSArray> tmp584;
    compiler::TNode<JSArray> tmp585;
    compiler::TNode<Map> tmp586;
    compiler::TNode<BoolT> tmp587;
    compiler::TNode<BoolT> tmp588;
    compiler::TNode<BoolT> tmp589;
    ca_.Bind(&block11, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585, &tmp586, &tmp587, &tmp588, &tmp589);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 41);
    compiler::TNode<JSFunction> tmp590;
    USE(tmp590);
    tmp590 = ca_.UncheckedCast<JSFunction>(BaseBuiltinsFromDSLAssembler(state_).LoadTargetFromFrame());
    compiler::TNode<Oddball> tmp591;
    USE(tmp591);
    tmp591 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 42);
    compiler::TNode<IntPtrT> tmp592;
    USE(tmp592);
    tmp592 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(p_arguments));
    compiler::TNode<Int32T> tmp593;
    USE(tmp593);
    tmp593 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).Convert7ATint328ATintptr(compiler::TNode<IntPtrT>{tmp592}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 40);
   CodeStubAssembler(state_).TailCallBuiltin(Builtins::kArrayShift, tmp581, tmp590, tmp591, tmp593);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp594;
    compiler::TNode<Object> tmp595;
    compiler::TNode<Object> tmp596;
    ca_.Bind(&block2, &tmp594, &tmp595, &tmp596);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 8);
    ca_.Goto(&block47, tmp594, tmp595, tmp596);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Slow);
  }

    compiler::TNode<Context> tmp597;
    compiler::TNode<Object> tmp598;
    compiler::TNode<Object> tmp599;
    ca_.Bind(&block47, &tmp597, &tmp598, &tmp599);
  return compiler::TNode<Object>{tmp599};
}

compiler::TNode<Object> ArrayShiftBuiltinsFromDSLAssembler::GenericArrayShift(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Number, Oddball> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Number, Oddball> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Number, Oddball> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 49);
    compiler::TNode<JSReceiver> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 52);
    compiler::TNode<Number> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 55);
    compiler::TNode<Number> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).IsNumberEqual(compiler::TNode<Number>{tmp3}, compiler::TNode<Number>{tmp4}));
    ca_.Branch(tmp5, &block2, &block3, tmp0, tmp1, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    ca_.Bind(&block2, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 57);
    compiler::TNode<String> tmp10;
    USE(tmp10);
    tmp10 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    compiler::TNode<Smi> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi17ATconstexpr_int31(0));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp6, tmp8, tmp10, tmp11);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 59);
    compiler::TNode<Oddball> tmp13;
    USE(tmp13);
    tmp13 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block1, tmp6, tmp7, tmp13);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<Number> tmp17;
    ca_.Bind(&block3, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 63);
    compiler::TNode<Smi> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp14}, compiler::TNode<Object>{tmp16}, compiler::TNode<Object>{tmp18}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 65);
    compiler::TNode<Number> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 67);
    ca_.Goto(&block6, tmp14, tmp15, tmp16, tmp17, tmp19, tmp20);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<JSReceiver> tmp23;
    compiler::TNode<Number> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Number> tmp26;
    ca_.Bind(&block6, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp26}, compiler::TNode<Number>{tmp24}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp26, tmp24);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block8, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp26, tmp24);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Number> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<Number> tmp34;
    ca_.Bind(&block7, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.Goto(&block4, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Number> tmp41;
    compiler::TNode<Number> tmp42;
    ca_.Bind(&block8, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    ca_.Goto(&block5, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Number> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Number> tmp48;
    ca_.Bind(&block4, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 69);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 72);
    compiler::TNode<Number> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp48}, compiler::TNode<Number>{tmp49}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 75);
    compiler::TNode<Oddball> tmp51;
    tmp51 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp43, tmp45, tmp48));
    USE(tmp51);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 78);
    compiler::TNode<Oddball> tmp52;
    USE(tmp52);
    tmp52 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp51}, compiler::TNode<HeapObject>{tmp52}));
    ca_.Branch(tmp53, &block9, &block10, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp48, tmp50, tmp51);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<JSReceiver> tmp56;
    compiler::TNode<Number> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<Number> tmp59;
    compiler::TNode<Number> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Oddball> tmp62;
    ca_.Bind(&block9, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 80);
    compiler::TNode<Object> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp54}, compiler::TNode<Object>{tmp56}, compiler::TNode<Object>{tmp60}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 83);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp54, tmp56, tmp61, tmp63);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 78);
    ca_.Goto(&block11, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<JSReceiver> tmp67;
    compiler::TNode<Number> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Number> tmp70;
    compiler::TNode<Number> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<Oddball> tmp73;
    ca_.Bind(&block10, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 86);
    compiler::TNode<Smi> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp65, tmp67, tmp72, tmp74);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 78);
    ca_.Goto(&block11, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp76;
    compiler::TNode<Object> tmp77;
    compiler::TNode<JSReceiver> tmp78;
    compiler::TNode<Number> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<Number> tmp81;
    compiler::TNode<Number> tmp82;
    compiler::TNode<Number> tmp83;
    compiler::TNode<Oddball> tmp84;
    ca_.Bind(&block11, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 90);
    compiler::TNode<Number> tmp85;
    USE(tmp85);
    tmp85 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp86;
    USE(tmp86);
    tmp86 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp81}, compiler::TNode<Number>{tmp85}));
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 67);
    ca_.Goto(&block6, tmp76, tmp77, tmp78, tmp79, tmp80, tmp86);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<JSReceiver> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<Number> tmp92;
    ca_.Bind(&block5, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 94);
    compiler::TNode<Number> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp90}, compiler::TNode<Number>{tmp93}));
    compiler::TNode<Smi> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp87, tmp89, tmp94, tmp95);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 97);
    compiler::TNode<String> tmp97;
    USE(tmp97);
    tmp97 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    compiler::TNode<Number> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp99;
    USE(tmp99);
    tmp99 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp90}, compiler::TNode<Number>{tmp98}));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp87, tmp89, tmp97, tmp99);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 100);
    ca_.Goto(&block1, tmp87, tmp88, tmp91);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    ca_.Bind(&block1, &tmp101, &tmp102, &tmp103);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 46);
    ca_.Goto(&block12, tmp101, tmp102, tmp103);
  }

    compiler::TNode<Context> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<Object> tmp106;
    ca_.Bind(&block12, &tmp104, &tmp105, &tmp106);
  return compiler::TNode<Object>{tmp106};
}

TF_BUILTIN(ArrayPrototypeShift, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 107);
    compiler::TNode<Object> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = ArrayShiftBuiltinsFromDSLAssembler(state_).TryFastArrayShift(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, arguments, &label0);
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
    compiler::TNode<Object> tmp9;
    ca_.Bind(&block3, &tmp6, &tmp7, &tmp8, &tmp9);
    arguments->PopAndReturn(tmp9);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    ca_.Bind(&block2, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../src/builtins/array-shift.tq", 110);
    compiler::TNode<Object> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Object>(ArrayShiftBuiltinsFromDSLAssembler(state_).GenericArrayShift(compiler::TNode<Context>{tmp10}, compiler::TNode<Object>{tmp11}));
    arguments->PopAndReturn(tmp12);
  }
}

}  // namespace internal
}  // namespace v8

