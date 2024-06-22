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

void ArrayUnshiftBuiltinsFromDSLAssembler::TryFastArrayUnshift(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, BaseBuiltinsFromDSLAssembler::Arguments p_arguments, compiler::CodeAssemblerLabel* label_Slow) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, JSArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSArray, Map> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSArray, Map> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSArray, Map, Map> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSArray, Map, Map> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver, p_arguments.frame, p_arguments.base, p_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 11);
    compiler::TNode<JSArray> tmp5;
    USE(tmp5);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp5 = BaseBuiltinsFromDSLAssembler(state_).Cast13ATFastJSArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp1, tmp5);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp1);
    }
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<Object> tmp7;
    compiler::TNode<RawPtrT> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<IntPtrT> tmp10;
    compiler::TNode<Object> tmp11;
    ca_.Bind(&block3, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.Goto(&block1);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<RawPtrT> tmp15;
    compiler::TNode<IntPtrT> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<JSArray> tmp18;
    ca_.Bind(&block2, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 12);
    ArrayBuiltinsFromDSLAssembler(state_).EnsureWriteableFastElements(compiler::TNode<Context>{tmp12}, compiler::TNode<JSArray>{tmp18});
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 14);
    compiler::TNode<IntPtrT> tmp19 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp19);
    compiler::TNode<Map>tmp20 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp18, tmp19});
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 15);
    compiler::TNode<BoolT> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsExtensibleMap(compiler::TNode<Map>{tmp20}));
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp21}));
    ca_.Branch(tmp22, &block4, &block5, tmp12, tmp13, tmp14, tmp15, tmp16, tmp18, tmp20);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<RawPtrT> tmp25;
    compiler::TNode<RawPtrT> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    compiler::TNode<JSArray> tmp28;
    compiler::TNode<Map> tmp29;
    ca_.Bind(&block4, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<RawPtrT> tmp32;
    compiler::TNode<RawPtrT> tmp33;
    compiler::TNode<IntPtrT> tmp34;
    compiler::TNode<JSArray> tmp35;
    compiler::TNode<Map> tmp36;
    ca_.Bind(&block5, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 16);
    compiler::CodeAssemblerLabel label0(&ca_);
    CodeStubAssembler(state_).EnsureArrayLengthWritable(compiler::TNode<Map>{tmp36}, &label0);
    ca_.Goto(&block6, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp36);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp36);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<RawPtrT> tmp39;
    compiler::TNode<RawPtrT> tmp40;
    compiler::TNode<IntPtrT> tmp41;
    compiler::TNode<JSArray> tmp42;
    compiler::TNode<Map> tmp43;
    compiler::TNode<Map> tmp44;
    ca_.Bind(&block7, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.Goto(&block1);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<RawPtrT> tmp47;
    compiler::TNode<RawPtrT> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    compiler::TNode<JSArray> tmp50;
    compiler::TNode<Map> tmp51;
    compiler::TNode<Map> tmp52;
    ca_.Bind(&block6, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 19);
    compiler::TNode<JSFunction> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<JSFunction>(BaseBuiltinsFromDSLAssembler(state_).LoadTargetFromFrame());
    compiler::TNode<Oddball> tmp54;
    USE(tmp54);
    tmp54 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 20);
    compiler::TNode<Int32T> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).Convert7ATint328ATintptr(compiler::TNode<IntPtrT>{tmp49}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 18);
   CodeStubAssembler(state_).TailCallBuiltin(Builtins::kArrayUnshift, tmp45, tmp53, tmp54, tmp55);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 8);
    ca_.Goto(label_Slow);
  }
}

compiler::TNode<Number> ArrayUnshiftBuiltinsFromDSLAssembler::GenericArrayUnshift(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, BaseBuiltinsFromDSLAssembler::Arguments p_arguments) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number, Number, Number, Oddball> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number, Number, Number, Oddball> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number, Number, Number, Oddball> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number, Smi> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi, Number, Smi> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Number> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver, p_arguments.frame, p_arguments.base, p_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 26);
    compiler::TNode<JSReceiver> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 29);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 32);
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp4}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 35);
    compiler::TNode<Smi> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp7}, compiler::TNode<Smi>{tmp8}));
    ca_.Branch(tmp9, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<RawPtrT> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<IntPtrT> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<Smi> tmp17;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 37);
    compiler::TNode<Number> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp16}, compiler::TNode<Number>{tmp17}));
    compiler::TNode<Number> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber19ATconstexpr_float64(kMaxSafeInteger));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp18}, compiler::TNode<Number>{tmp19}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block7, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<JSReceiver> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<Number> tmp28;
    ca_.Bind(&block6, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28);
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<RawPtrT> tmp31;
    compiler::TNode<RawPtrT> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    compiler::TNode<JSReceiver> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Number> tmp37;
    ca_.Bind(&block7, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    ca_.Goto(&block5, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<RawPtrT> tmp40;
    compiler::TNode<RawPtrT> tmp41;
    compiler::TNode<IntPtrT> tmp42;
    compiler::TNode<JSReceiver> tmp43;
    compiler::TNode<Number> tmp44;
    compiler::TNode<Smi> tmp45;
    ca_.Bind(&block4, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 38);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp38}, MessageTemplate::kInvalidArrayLength);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<RawPtrT> tmp48;
    compiler::TNode<RawPtrT> tmp49;
    compiler::TNode<IntPtrT> tmp50;
    compiler::TNode<JSReceiver> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<Smi> tmp53;
    ca_.Bind(&block5, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 42);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 45);
    ca_.Goto(&block10, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp52);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp54;
    compiler::TNode<Object> tmp55;
    compiler::TNode<RawPtrT> tmp56;
    compiler::TNode<RawPtrT> tmp57;
    compiler::TNode<IntPtrT> tmp58;
    compiler::TNode<JSReceiver> tmp59;
    compiler::TNode<Number> tmp60;
    compiler::TNode<Smi> tmp61;
    compiler::TNode<Number> tmp62;
    ca_.Bind(&block10, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    compiler::TNode<Number> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp62}, compiler::TNode<Number>{tmp63}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block11, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp62);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block12, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp62);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<Object> tmp65;
    compiler::TNode<RawPtrT> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<IntPtrT> tmp68;
    compiler::TNode<JSReceiver> tmp69;
    compiler::TNode<Number> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<Number> tmp73;
    ca_.Bind(&block11, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    ca_.Goto(&block8, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<JSReceiver> tmp79;
    compiler::TNode<Number> tmp80;
    compiler::TNode<Smi> tmp81;
    compiler::TNode<Number> tmp82;
    compiler::TNode<Number> tmp83;
    ca_.Bind(&block12, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83);
    ca_.Goto(&block9, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<RawPtrT> tmp86;
    compiler::TNode<RawPtrT> tmp87;
    compiler::TNode<IntPtrT> tmp88;
    compiler::TNode<JSReceiver> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<Number> tmp92;
    ca_.Bind(&block8, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 47);
    compiler::TNode<Number> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp92}, compiler::TNode<Number>{tmp93}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 50);
    compiler::TNode<Number> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp92}, compiler::TNode<Number>{tmp91}));
    compiler::TNode<Number> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp97;
    USE(tmp97);
    tmp97 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp95}, compiler::TNode<Number>{tmp96}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 53);
    compiler::TNode<Oddball> tmp98;
    tmp98 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp84, tmp89, tmp94));
    USE(tmp98);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 56);
    compiler::TNode<Oddball> tmp99;
    USE(tmp99);
    tmp99 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp100;
    USE(tmp100);
    tmp100 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp98}, compiler::TNode<HeapObject>{tmp99}));
    ca_.Branch(tmp100, &block13, &block14, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp94, tmp97, tmp98);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<RawPtrT> tmp103;
    compiler::TNode<RawPtrT> tmp104;
    compiler::TNode<IntPtrT> tmp105;
    compiler::TNode<JSReceiver> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<Smi> tmp108;
    compiler::TNode<Number> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Oddball> tmp112;
    ca_.Bind(&block13, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 58);
    compiler::TNode<Object> tmp113;
    USE(tmp113);
    tmp113 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp101}, compiler::TNode<Object>{tmp106}, compiler::TNode<Object>{tmp110}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 61);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp101, tmp106, tmp111, tmp113);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 56);
    ca_.Goto(&block15, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp115;
    compiler::TNode<Object> tmp116;
    compiler::TNode<RawPtrT> tmp117;
    compiler::TNode<RawPtrT> tmp118;
    compiler::TNode<IntPtrT> tmp119;
    compiler::TNode<JSReceiver> tmp120;
    compiler::TNode<Number> tmp121;
    compiler::TNode<Smi> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Number> tmp125;
    compiler::TNode<Oddball> tmp126;
    ca_.Bind(&block14, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 64);
    compiler::TNode<Smi> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp115, tmp120, tmp125, tmp127);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 56);
    ca_.Goto(&block15, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<RawPtrT> tmp131;
    compiler::TNode<RawPtrT> tmp132;
    compiler::TNode<IntPtrT> tmp133;
    compiler::TNode<JSReceiver> tmp134;
    compiler::TNode<Number> tmp135;
    compiler::TNode<Smi> tmp136;
    compiler::TNode<Number> tmp137;
    compiler::TNode<Number> tmp138;
    compiler::TNode<Number> tmp139;
    compiler::TNode<Oddball> tmp140;
    ca_.Bind(&block15, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 68);
    compiler::TNode<Number> tmp141;
    USE(tmp141);
    tmp141 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp142;
    USE(tmp142);
    tmp142 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp137}, compiler::TNode<Number>{tmp141}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 45);
    ca_.Goto(&block10, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp142);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<RawPtrT> tmp145;
    compiler::TNode<RawPtrT> tmp146;
    compiler::TNode<IntPtrT> tmp147;
    compiler::TNode<JSReceiver> tmp148;
    compiler::TNode<Number> tmp149;
    compiler::TNode<Smi> tmp150;
    compiler::TNode<Number> tmp151;
    ca_.Bind(&block9, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 72);
    compiler::TNode<Smi> tmp152;
    USE(tmp152);
    tmp152 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 77);
    ca_.Goto(&block18, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<RawPtrT> tmp155;
    compiler::TNode<RawPtrT> tmp156;
    compiler::TNode<IntPtrT> tmp157;
    compiler::TNode<JSReceiver> tmp158;
    compiler::TNode<Number> tmp159;
    compiler::TNode<Smi> tmp160;
    compiler::TNode<Number> tmp161;
    compiler::TNode<Smi> tmp162;
    ca_.Bind(&block18, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162);
    compiler::TNode<BoolT> tmp163;
    USE(tmp163);
    tmp163 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp162}, compiler::TNode<Smi>{tmp160}));
    ca_.Branch(tmp163, &block16, &block17, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp164;
    compiler::TNode<Object> tmp165;
    compiler::TNode<RawPtrT> tmp166;
    compiler::TNode<RawPtrT> tmp167;
    compiler::TNode<IntPtrT> tmp168;
    compiler::TNode<JSReceiver> tmp169;
    compiler::TNode<Number> tmp170;
    compiler::TNode<Smi> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<Smi> tmp173;
    ca_.Bind(&block16, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 79);
    compiler::TNode<IntPtrT> tmp174;
    USE(tmp174);
    tmp174 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp173}));
    compiler::TNode<Object> tmp175;
    USE(tmp175);
    tmp175 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp166}, compiler::TNode<RawPtrT>{tmp167}, compiler::TNode<IntPtrT>{tmp168}}, compiler::TNode<IntPtrT>{tmp174}));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp164, tmp169, tmp173, tmp175);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 82);
    compiler::TNode<Smi> tmp177;
    USE(tmp177);
    tmp177 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp178;
    USE(tmp178);
    tmp178 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp173}, compiler::TNode<Smi>{tmp177}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 77);
    ca_.Goto(&block18, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp178);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp179;
    compiler::TNode<Object> tmp180;
    compiler::TNode<RawPtrT> tmp181;
    compiler::TNode<RawPtrT> tmp182;
    compiler::TNode<IntPtrT> tmp183;
    compiler::TNode<JSReceiver> tmp184;
    compiler::TNode<Number> tmp185;
    compiler::TNode<Smi> tmp186;
    compiler::TNode<Number> tmp187;
    compiler::TNode<Smi> tmp188;
    ca_.Bind(&block17, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 35);
    ca_.Goto(&block3, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp189;
    compiler::TNode<Object> tmp190;
    compiler::TNode<RawPtrT> tmp191;
    compiler::TNode<RawPtrT> tmp192;
    compiler::TNode<IntPtrT> tmp193;
    compiler::TNode<JSReceiver> tmp194;
    compiler::TNode<Number> tmp195;
    compiler::TNode<Smi> tmp196;
    ca_.Bind(&block3, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 87);
    compiler::TNode<Number> tmp197;
    USE(tmp197);
    tmp197 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp195}, compiler::TNode<Number>{tmp196}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 88);
    compiler::TNode<String> tmp198;
    USE(tmp198);
    tmp198 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp189, tmp194, tmp198, tmp197);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 91);
    ca_.Goto(&block1, tmp189, tmp190, tmp191, tmp192, tmp193, tmp197);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp200;
    compiler::TNode<Object> tmp201;
    compiler::TNode<RawPtrT> tmp202;
    compiler::TNode<RawPtrT> tmp203;
    compiler::TNode<IntPtrT> tmp204;
    compiler::TNode<Number> tmp205;
    ca_.Bind(&block1, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 23);
    ca_.Goto(&block19, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205);
  }

    compiler::TNode<Context> tmp206;
    compiler::TNode<Object> tmp207;
    compiler::TNode<RawPtrT> tmp208;
    compiler::TNode<RawPtrT> tmp209;
    compiler::TNode<IntPtrT> tmp210;
    compiler::TNode<Number> tmp211;
    ca_.Bind(&block19, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211);
  return compiler::TNode<Number>{tmp211};
}

TF_BUILTIN(ArrayPrototypeUnshift, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Context, Object, RawPtrT, RawPtrT, IntPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 98);
    compiler::CodeAssemblerLabel label0(&ca_);
    ArrayUnshiftBuiltinsFromDSLAssembler(state_).TryFastArrayUnshift(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, &label0);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp0, tmp1, tmp2, tmp3, tmp4);
    }
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<Object> tmp6;
    compiler::TNode<RawPtrT> tmp7;
    compiler::TNode<RawPtrT> tmp8;
    compiler::TNode<IntPtrT> tmp9;
    compiler::TNode<Context> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<RawPtrT> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<IntPtrT> tmp14;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.Goto(&block2, tmp5, tmp6, tmp7, tmp8, tmp9);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<RawPtrT> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    ca_.Bind(&block2, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../v8/src/builtins/array-unshift.tq", 101);
    compiler::TNode<Number> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Number>(ArrayUnshiftBuiltinsFromDSLAssembler(state_).GenericArrayUnshift(compiler::TNode<Context>{tmp15}, compiler::TNode<Object>{tmp16}, BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp17}, compiler::TNode<RawPtrT>{tmp18}, compiler::TNode<IntPtrT>{tmp19}}));
    arguments.PopAndReturn(tmp20);
  }
}

}  // namespace internal
}  // namespace v8

