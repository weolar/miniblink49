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

void ArrayUnshiftBuiltinsFromDSLAssembler::TryFastArrayUnshift(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, CodeStubArguments* p_arguments, compiler::CodeAssemblerLabel* label_Slow) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, JSArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, Map> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, Map> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, Map, Map> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSArray, Map, Map> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 11);
    compiler::TNode<JSArray> tmp2;
    USE(tmp2);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).Cast13ATFastJSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block2, tmp0, tmp1, tmp1, tmp2);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block3, tmp0, tmp1, tmp1);
    }
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp3;
    compiler::TNode<Object> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block3, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block1);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<JSArray> tmp9;
    ca_.Bind(&block2, &tmp6, &tmp7, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 12);
    ArrayBuiltinsFromDSLAssembler(state_).EnsureWriteableFastElements(compiler::TNode<Context>{tmp6}, compiler::TNode<JSArray>{tmp9});
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 14);
    compiler::TNode<Map> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Map>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapObjectMap(compiler::TNode<HeapObject>{tmp9}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 15);
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsExtensibleMap(compiler::TNode<Map>{tmp10}));
    compiler::TNode<BoolT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp11}));
    ca_.Branch(tmp12, &block4, &block5, tmp6, tmp7, tmp9, tmp10);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<JSArray> tmp15;
    compiler::TNode<Map> tmp16;
    ca_.Bind(&block4, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<JSArray> tmp19;
    compiler::TNode<Map> tmp20;
    ca_.Bind(&block5, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 16);
    compiler::CodeAssemblerLabel label0(&ca_);
    CodeStubAssembler(state_).EnsureArrayLengthWritable(compiler::TNode<Map>{tmp20}, &label0);
    ca_.Goto(&block6, tmp17, tmp18, tmp19, tmp20, tmp20);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp17, tmp18, tmp19, tmp20, tmp20);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<JSArray> tmp23;
    compiler::TNode<Map> tmp24;
    compiler::TNode<Map> tmp25;
    ca_.Bind(&block7, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block1);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<JSArray> tmp28;
    compiler::TNode<Map> tmp29;
    compiler::TNode<Map> tmp30;
    ca_.Bind(&block6, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 19);
    compiler::TNode<JSFunction> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<JSFunction>(BaseBuiltinsFromDSLAssembler(state_).LoadTargetFromFrame());
    compiler::TNode<Oddball> tmp32;
    USE(tmp32);
    tmp32 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 20);
    compiler::TNode<IntPtrT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(p_arguments));
    compiler::TNode<Int32T> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).Convert7ATint328ATintptr(compiler::TNode<IntPtrT>{tmp33}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 18);
   CodeStubAssembler(state_).TailCallBuiltin(Builtins::kArrayUnshift, tmp26, tmp31, tmp32, tmp34);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 8);
    ca_.Goto(label_Slow);
  }
}

compiler::TNode<Number> ArrayUnshiftBuiltinsFromDSLAssembler::GenericArrayUnshift(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, CodeStubArguments* p_arguments) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number, Number, Number, Oddball> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number, Number, Number, Oddball> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number, Number, Number, Oddball> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number, Smi> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi, Number, Smi> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Number> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 27);
    compiler::TNode<JSReceiver> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 30);
    compiler::TNode<Number> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 33);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(p_arguments));
    compiler::TNode<Smi> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 36);
    compiler::TNode<Smi> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp5}, compiler::TNode<Smi>{tmp6}));
    ca_.Branch(tmp7, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<Number> tmp11;
    compiler::TNode<Smi> tmp12;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 38);
    compiler::TNode<Number> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp11}, compiler::TNode<Number>{tmp12}));
    compiler::TNode<Number> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber19ATconstexpr_float64(kMaxSafeInteger));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp13}, compiler::TNode<Number>{tmp14}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Number> tmp20;
    ca_.Bind(&block6, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.Goto(&block4, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<JSReceiver> tmp23;
    compiler::TNode<Number> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<Number> tmp26;
    ca_.Bind(&block7, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.Goto(&block5, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Smi> tmp31;
    ca_.Bind(&block4, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 39);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp27}, MessageTemplate::kInvalidArrayLength);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Smi> tmp36;
    ca_.Bind(&block5, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 43);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 46);
    ca_.Goto(&block10, tmp32, tmp33, tmp34, tmp35, tmp36, tmp35);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<Number> tmp42;
    ca_.Bind(&block10, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    compiler::TNode<Number> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp42}, compiler::TNode<Number>{tmp43}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block11, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp42);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block12, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp42);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<JSReceiver> tmp46;
    compiler::TNode<Number> tmp47;
    compiler::TNode<Smi> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Number> tmp50;
    ca_.Bind(&block11, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.Goto(&block8, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<JSReceiver> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<Smi> tmp55;
    compiler::TNode<Number> tmp56;
    compiler::TNode<Number> tmp57;
    ca_.Bind(&block12, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57);
    ca_.Goto(&block9, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Smi> tmp62;
    compiler::TNode<Number> tmp63;
    ca_.Bind(&block8, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 48);
    compiler::TNode<Number> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp63}, compiler::TNode<Number>{tmp64}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 51);
    compiler::TNode<Number> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp63}, compiler::TNode<Number>{tmp62}));
    compiler::TNode<Number> tmp67;
    USE(tmp67);
    tmp67 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp68;
    USE(tmp68);
    tmp68 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp66}, compiler::TNode<Number>{tmp67}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 54);
    compiler::TNode<Oddball> tmp69;
    tmp69 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp58, tmp60, tmp65));
    USE(tmp69);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 57);
    compiler::TNode<Oddball> tmp70;
    USE(tmp70);
    tmp70 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp71;
    USE(tmp71);
    tmp71 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp69}, compiler::TNode<HeapObject>{tmp70}));
    ca_.Branch(tmp71, &block13, &block14, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp65, tmp68, tmp69);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Smi> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<Number> tmp79;
    compiler::TNode<Oddball> tmp80;
    ca_.Bind(&block13, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 59);
    compiler::TNode<Object> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp72}, compiler::TNode<Object>{tmp74}, compiler::TNode<Object>{tmp78}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 62);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp72, tmp74, tmp79, tmp81);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 57);
    ca_.Goto(&block15, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp83;
    compiler::TNode<Object> tmp84;
    compiler::TNode<JSReceiver> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<Number> tmp88;
    compiler::TNode<Number> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Oddball> tmp91;
    ca_.Bind(&block14, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 65);
    compiler::TNode<Smi> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp83, tmp85, tmp90, tmp92);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 57);
    ca_.Goto(&block15, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<JSReceiver> tmp96;
    compiler::TNode<Number> tmp97;
    compiler::TNode<Smi> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Oddball> tmp102;
    ca_.Bind(&block15, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 69);
    compiler::TNode<Number> tmp103;
    USE(tmp103);
    tmp103 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp104;
    USE(tmp104);
    tmp104 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp99}, compiler::TNode<Number>{tmp103}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 46);
    ca_.Goto(&block10, tmp94, tmp95, tmp96, tmp97, tmp98, tmp104);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp105;
    compiler::TNode<Object> tmp106;
    compiler::TNode<JSReceiver> tmp107;
    compiler::TNode<Number> tmp108;
    compiler::TNode<Smi> tmp109;
    compiler::TNode<Number> tmp110;
    ca_.Bind(&block9, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 73);
    compiler::TNode<Smi> tmp111;
    USE(tmp111);
    tmp111 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 78);
    ca_.Goto(&block18, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<JSReceiver> tmp114;
    compiler::TNode<Number> tmp115;
    compiler::TNode<Smi> tmp116;
    compiler::TNode<Number> tmp117;
    compiler::TNode<Smi> tmp118;
    ca_.Bind(&block18, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118);
    compiler::TNode<BoolT> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp118}, compiler::TNode<Smi>{tmp116}));
    ca_.Branch(tmp119, &block16, &block17, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<JSReceiver> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<Smi> tmp124;
    compiler::TNode<Number> tmp125;
    compiler::TNode<Smi> tmp126;
    ca_.Bind(&block16, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 80);
    compiler::TNode<IntPtrT> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp126}));
    compiler::TNode<Object> tmp128;
    USE(tmp128);
    tmp128 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(p_arguments, compiler::TNode<IntPtrT>{tmp127}));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp120, tmp122, tmp126, tmp128);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 83);
    compiler::TNode<Smi> tmp130;
    USE(tmp130);
    tmp130 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp131;
    USE(tmp131);
    tmp131 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp126}, compiler::TNode<Smi>{tmp130}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 78);
    ca_.Goto(&block18, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp131);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp132;
    compiler::TNode<Object> tmp133;
    compiler::TNode<JSReceiver> tmp134;
    compiler::TNode<Number> tmp135;
    compiler::TNode<Smi> tmp136;
    compiler::TNode<Number> tmp137;
    compiler::TNode<Smi> tmp138;
    ca_.Bind(&block17, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 36);
    ca_.Goto(&block3, tmp132, tmp133, tmp134, tmp135, tmp136);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<JSReceiver> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<Smi> tmp143;
    ca_.Bind(&block3, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 88);
    compiler::TNode<Number> tmp144;
    USE(tmp144);
    tmp144 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp142}, compiler::TNode<Number>{tmp143}));
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 89);
    compiler::TNode<String> tmp145;
    USE(tmp145);
    tmp145 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp139, tmp141, tmp145, tmp144);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 92);
    ca_.Goto(&block1, tmp139, tmp140, tmp144);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp147;
    compiler::TNode<Object> tmp148;
    compiler::TNode<Number> tmp149;
    ca_.Bind(&block1, &tmp147, &tmp148, &tmp149);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 23);
    ca_.Goto(&block19, tmp147, tmp148, tmp149);
  }

    compiler::TNode<Context> tmp150;
    compiler::TNode<Object> tmp151;
    compiler::TNode<Number> tmp152;
    ca_.Bind(&block19, &tmp150, &tmp151, &tmp152);
  return compiler::TNode<Number>{tmp152};
}

TF_BUILTIN(ArrayPrototypeUnshift, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Context, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 99);
    compiler::CodeAssemblerLabel label0(&ca_);
    ArrayUnshiftBuiltinsFromDSLAssembler(state_).TryFastArrayUnshift(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, arguments, &label0);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block3, tmp0, tmp1, tmp0, tmp1);
    }
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Context> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block3, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block2, tmp2, tmp3);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    ca_.Bind(&block2, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../src/builtins/array-unshift.tq", 102);
    compiler::TNode<Number> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Number>(ArrayUnshiftBuiltinsFromDSLAssembler(state_).GenericArrayUnshift(compiler::TNode<Context>{tmp6}, compiler::TNode<Object>{tmp7}, arguments));
    arguments->PopAndReturn(tmp8);
  }
}

}  // namespace internal
}  // namespace v8

