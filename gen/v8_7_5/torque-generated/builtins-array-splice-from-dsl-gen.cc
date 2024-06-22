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

compiler::TNode<FixedArray> ArraySpliceBuiltinsFromDSLAssembler::Extract10FixedArray(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_first, compiler::TNode<Smi> p_count, compiler::TNode<Smi> p_capacity) {
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArrayBase, Smi, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArrayBase, Smi, Smi, Smi, FixedArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArrayBase, Smi, Smi, Smi, FixedArray> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_elements, p_first, p_count, p_capacity);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<FixedArrayBase> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    compiler::TNode<Smi> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 18);
    compiler::TNode<FixedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedArrayBase>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArrayBase>{tmp1}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Smi>{tmp3}, compiler::TNode<Smi>{tmp4}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 17);
    compiler::TNode<FixedArray> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.Goto(&block1, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<FixedArrayBase> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<FixedArray> tmp12;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 10);
    ca_.Goto(&block2, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12);
  }

    compiler::TNode<Context> tmp13;
    compiler::TNode<FixedArrayBase> tmp14;
    compiler::TNode<Smi> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Smi> tmp17;
    compiler::TNode<FixedArray> tmp18;
    ca_.Bind(&block2, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
  return compiler::TNode<FixedArray>{tmp18};
}

compiler::TNode<FixedDoubleArray> ArraySpliceBuiltinsFromDSLAssembler::Extract16FixedDoubleArray(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_first, compiler::TNode<Smi> p_count, compiler::TNode<Smi> p_capacity) {
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArrayBase, Smi, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArrayBase, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArrayBase, Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArrayBase, Smi, Smi, Smi, FixedDoubleArray> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, FixedArrayBase, Smi, Smi, Smi, FixedDoubleArray> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_elements, p_first, p_count, p_capacity);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<FixedArrayBase> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    compiler::TNode<Smi> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 24);
    compiler::TNode<FixedArray> tmp5;
    USE(tmp5);
    tmp5 = BaseBuiltinsFromDSLAssembler(state_).kEmptyFixedArray();
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp1}, compiler::TNode<HeapObject>{tmp5}));
    ca_.Branch(tmp6, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<FixedArrayBase> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block2, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 25);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp11}));
    compiler::TNode<FixedDoubleArray> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<FixedDoubleArray>(CodeStubAssembler(state_).AllocateZeroedFixedDoubleArray(compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block1, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<FixedArrayBase> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Smi> tmp17;
    compiler::TNode<Smi> tmp18;
    ca_.Bind(&block3, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 28);
    compiler::TNode<FixedArrayBase> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<FixedArrayBase>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArrayBase>{tmp15}, compiler::TNode<Smi>{tmp16}, compiler::TNode<Smi>{tmp17}, compiler::TNode<Smi>{tmp18}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 27);
    compiler::TNode<FixedDoubleArray> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp14}, compiler::TNode<Object>{tmp19}));
    ca_.Goto(&block1, tmp14, tmp15, tmp16, tmp17, tmp18, tmp20);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<FixedArrayBase> tmp22;
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Smi> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<FixedDoubleArray> tmp26;
    ca_.Bind(&block1, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 10);
    ca_.Goto(&block4, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26);
  }

    compiler::TNode<Context> tmp27;
    compiler::TNode<FixedArrayBase> tmp28;
    compiler::TNode<Smi> tmp29;
    compiler::TNode<Smi> tmp30;
    compiler::TNode<Smi> tmp31;
    compiler::TNode<FixedDoubleArray> tmp32;
    ca_.Bind(&block4, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32);
  return compiler::TNode<FixedDoubleArray>{tmp32};
}

compiler::TNode<Object> ArraySpliceBuiltinsFromDSLAssembler::FastArraySplice(compiler::TNode<Context> p_context, BaseBuiltinsFromDSLAssembler::Arguments p_args, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_originalLengthNumber, compiler::TNode<Number> p_actualStartNumber, compiler::TNode<Smi> p_insertCount, compiler::TNode<Number> p_actualDeleteCountNumber, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Number, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Number, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSReceiver, JSArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Map> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Map, Int32T> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object, HeapObject> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object, HeapObject> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object, HeapObject, Int32T> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object, HeapObject, Int32T> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, Object> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Number> block41(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Number, Smi> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi> block43(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block44(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block45(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block46(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block50(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block49(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block47(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block52(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block51(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block48(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Object> block53(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_args.frame, p_args.base, p_args.length, p_o, p_originalLengthNumber, p_actualStartNumber, p_insertCount, p_actualDeleteCountNumber);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<RawPtrT> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<IntPtrT> tmp3;
    compiler::TNode<JSReceiver> tmp4;
    compiler::TNode<Number> tmp5;
    compiler::TNode<Number> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Number> tmp8;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 116);
    compiler::TNode<Smi> tmp9;
    USE(tmp9);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp5}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp5, tmp9);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp5);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<RawPtrT> tmp11;
    compiler::TNode<RawPtrT> tmp12;
    compiler::TNode<IntPtrT> tmp13;
    compiler::TNode<JSReceiver> tmp14;
    compiler::TNode<Number> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<Smi> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Number> tmp19;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block1);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<RawPtrT> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<IntPtrT> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<Number> tmp28;
    compiler::TNode<Number> tmp29;
    compiler::TNode<Smi> tmp30;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 115);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 117);
    compiler::TNode<Smi> tmp31;
    USE(tmp31);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp31 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp26}, &label0);
    ca_.Goto(&block5, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp30, tmp26, tmp31);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp30, tmp26);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<RawPtrT> tmp33;
    compiler::TNode<RawPtrT> tmp34;
    compiler::TNode<IntPtrT> tmp35;
    compiler::TNode<JSReceiver> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Smi> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<Number> tmp42;
    ca_.Bind(&block6, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<JSReceiver> tmp47;
    compiler::TNode<Number> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Smi> tmp50;
    compiler::TNode<Number> tmp51;
    compiler::TNode<Smi> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Smi> tmp54;
    ca_.Bind(&block5, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 119);
    compiler::TNode<Smi> tmp55;
    USE(tmp55);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp55 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp51}, &label0);
    ca_.Goto(&block7, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp54, tmp51, tmp55);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp54, tmp51);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<RawPtrT> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<IntPtrT> tmp59;
    compiler::TNode<JSReceiver> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<Number> tmp64;
    compiler::TNode<Smi> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<Number> tmp67;
    ca_.Bind(&block8, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.Goto(&block1);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<RawPtrT> tmp69;
    compiler::TNode<RawPtrT> tmp70;
    compiler::TNode<IntPtrT> tmp71;
    compiler::TNode<JSReceiver> tmp72;
    compiler::TNode<Number> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Smi> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Smi> tmp77;
    compiler::TNode<Smi> tmp78;
    compiler::TNode<Number> tmp79;
    compiler::TNode<Smi> tmp80;
    ca_.Bind(&block7, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 118);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 120);
    compiler::TNode<Smi> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp75}, compiler::TNode<Smi>{tmp80}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 121);
    compiler::TNode<Smi> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp77}, compiler::TNode<Smi>{tmp81}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 123);
    compiler::TNode<JSArray> tmp83;
    USE(tmp83);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp83 = BaseBuiltinsFromDSLAssembler(state_).Cast7JSArray(compiler::TNode<HeapObject>{tmp72}, &label0);
    ca_.Goto(&block9, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp80, tmp81, tmp82, tmp72, tmp83);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp80, tmp81, tmp82, tmp72);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<RawPtrT> tmp85;
    compiler::TNode<RawPtrT> tmp86;
    compiler::TNode<IntPtrT> tmp87;
    compiler::TNode<JSReceiver> tmp88;
    compiler::TNode<Number> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<Smi> tmp93;
    compiler::TNode<Smi> tmp94;
    compiler::TNode<Smi> tmp95;
    compiler::TNode<Smi> tmp96;
    compiler::TNode<Smi> tmp97;
    compiler::TNode<JSReceiver> tmp98;
    ca_.Bind(&block10, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98);
    ca_.Goto(&block1);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp99;
    compiler::TNode<RawPtrT> tmp100;
    compiler::TNode<RawPtrT> tmp101;
    compiler::TNode<IntPtrT> tmp102;
    compiler::TNode<JSReceiver> tmp103;
    compiler::TNode<Number> tmp104;
    compiler::TNode<Number> tmp105;
    compiler::TNode<Smi> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<Smi> tmp108;
    compiler::TNode<Smi> tmp109;
    compiler::TNode<Smi> tmp110;
    compiler::TNode<Smi> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<JSReceiver> tmp113;
    compiler::TNode<JSArray> tmp114;
    ca_.Bind(&block9, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 125);
    compiler::TNode<IntPtrT> tmp115 = ca_.IntPtrConstant(HeapObject::kMapOffset);
    USE(tmp115);
    compiler::TNode<Map>tmp116 = CodeStubAssembler(state_).LoadReference<Map>(CodeStubAssembler::Reference{tmp114, tmp115});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 126);
    compiler::TNode<BoolT> tmp117;
    USE(tmp117);
    tmp117 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsPrototypeInitialArrayPrototype(compiler::TNode<Context>{tmp99}, compiler::TNode<Map>{tmp116}));
    compiler::TNode<BoolT> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp117}));
    ca_.Branch(tmp118, &block11, &block12, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp114, tmp116);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp119;
    compiler::TNode<RawPtrT> tmp120;
    compiler::TNode<RawPtrT> tmp121;
    compiler::TNode<IntPtrT> tmp122;
    compiler::TNode<JSReceiver> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Number> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Number> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<Smi> tmp130;
    compiler::TNode<Smi> tmp131;
    compiler::TNode<Smi> tmp132;
    compiler::TNode<JSArray> tmp133;
    compiler::TNode<Map> tmp134;
    ca_.Bind(&block11, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134);
    ca_.Goto(&block1);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp135;
    compiler::TNode<RawPtrT> tmp136;
    compiler::TNode<RawPtrT> tmp137;
    compiler::TNode<IntPtrT> tmp138;
    compiler::TNode<JSReceiver> tmp139;
    compiler::TNode<Number> tmp140;
    compiler::TNode<Number> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<Number> tmp143;
    compiler::TNode<Smi> tmp144;
    compiler::TNode<Smi> tmp145;
    compiler::TNode<Smi> tmp146;
    compiler::TNode<Smi> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<JSArray> tmp149;
    compiler::TNode<Map> tmp150;
    ca_.Bind(&block12, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 127);
    compiler::TNode<BoolT> tmp151;
    USE(tmp151);
    tmp151 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNoElementsProtectorCellInvalid());
    ca_.Branch(tmp151, &block13, &block14, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp152;
    compiler::TNode<RawPtrT> tmp153;
    compiler::TNode<RawPtrT> tmp154;
    compiler::TNode<IntPtrT> tmp155;
    compiler::TNode<JSReceiver> tmp156;
    compiler::TNode<Number> tmp157;
    compiler::TNode<Number> tmp158;
    compiler::TNode<Smi> tmp159;
    compiler::TNode<Number> tmp160;
    compiler::TNode<Smi> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<Smi> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<JSArray> tmp166;
    compiler::TNode<Map> tmp167;
    ca_.Bind(&block13, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167);
    ca_.Goto(&block1);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp168;
    compiler::TNode<RawPtrT> tmp169;
    compiler::TNode<RawPtrT> tmp170;
    compiler::TNode<IntPtrT> tmp171;
    compiler::TNode<JSReceiver> tmp172;
    compiler::TNode<Number> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<Number> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<Smi> tmp178;
    compiler::TNode<Smi> tmp179;
    compiler::TNode<Smi> tmp180;
    compiler::TNode<Smi> tmp181;
    compiler::TNode<JSArray> tmp182;
    compiler::TNode<Map> tmp183;
    ca_.Bind(&block14, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 128);
    compiler::TNode<BoolT> tmp184;
    USE(tmp184);
    tmp184 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsArraySpeciesProtectorCellInvalid());
    ca_.Branch(tmp184, &block15, &block16, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp185;
    compiler::TNode<RawPtrT> tmp186;
    compiler::TNode<RawPtrT> tmp187;
    compiler::TNode<IntPtrT> tmp188;
    compiler::TNode<JSReceiver> tmp189;
    compiler::TNode<Number> tmp190;
    compiler::TNode<Number> tmp191;
    compiler::TNode<Smi> tmp192;
    compiler::TNode<Number> tmp193;
    compiler::TNode<Smi> tmp194;
    compiler::TNode<Smi> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<Smi> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<JSArray> tmp199;
    compiler::TNode<Map> tmp200;
    ca_.Bind(&block15, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200);
    ca_.Goto(&block1);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp201;
    compiler::TNode<RawPtrT> tmp202;
    compiler::TNode<RawPtrT> tmp203;
    compiler::TNode<IntPtrT> tmp204;
    compiler::TNode<JSReceiver> tmp205;
    compiler::TNode<Number> tmp206;
    compiler::TNode<Number> tmp207;
    compiler::TNode<Smi> tmp208;
    compiler::TNode<Number> tmp209;
    compiler::TNode<Smi> tmp210;
    compiler::TNode<Smi> tmp211;
    compiler::TNode<Smi> tmp212;
    compiler::TNode<Smi> tmp213;
    compiler::TNode<Smi> tmp214;
    compiler::TNode<JSArray> tmp215;
    compiler::TNode<Map> tmp216;
    ca_.Bind(&block16, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 131);
    compiler::TNode<Int32T> tmp217;
    USE(tmp217);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp217 = CodeStubAssembler(state_).EnsureArrayPushable(compiler::TNode<Map>{tmp216}, &label0);
    ca_.Goto(&block17, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp216, tmp217);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block18, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp216);
    }
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp218;
    compiler::TNode<RawPtrT> tmp219;
    compiler::TNode<RawPtrT> tmp220;
    compiler::TNode<IntPtrT> tmp221;
    compiler::TNode<JSReceiver> tmp222;
    compiler::TNode<Number> tmp223;
    compiler::TNode<Number> tmp224;
    compiler::TNode<Smi> tmp225;
    compiler::TNode<Number> tmp226;
    compiler::TNode<Smi> tmp227;
    compiler::TNode<Smi> tmp228;
    compiler::TNode<Smi> tmp229;
    compiler::TNode<Smi> tmp230;
    compiler::TNode<Smi> tmp231;
    compiler::TNode<JSArray> tmp232;
    compiler::TNode<Map> tmp233;
    compiler::TNode<Map> tmp234;
    ca_.Bind(&block18, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234);
    ca_.Goto(&block1);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp235;
    compiler::TNode<RawPtrT> tmp236;
    compiler::TNode<RawPtrT> tmp237;
    compiler::TNode<IntPtrT> tmp238;
    compiler::TNode<JSReceiver> tmp239;
    compiler::TNode<Number> tmp240;
    compiler::TNode<Number> tmp241;
    compiler::TNode<Smi> tmp242;
    compiler::TNode<Number> tmp243;
    compiler::TNode<Smi> tmp244;
    compiler::TNode<Smi> tmp245;
    compiler::TNode<Smi> tmp246;
    compiler::TNode<Smi> tmp247;
    compiler::TNode<Smi> tmp248;
    compiler::TNode<JSArray> tmp249;
    compiler::TNode<Map> tmp250;
    compiler::TNode<Map> tmp251;
    compiler::TNode<Int32T> tmp252;
    ca_.Bind(&block17, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 132);
    compiler::TNode<BoolT> tmp253;
    USE(tmp253);
    tmp253 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsFastElementsKind(compiler::TNode<Int32T>{tmp252}));
    compiler::TNode<BoolT> tmp254;
    USE(tmp254);
    tmp254 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp253}));
    ca_.Branch(tmp254, &block19, &block20, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp252);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp255;
    compiler::TNode<RawPtrT> tmp256;
    compiler::TNode<RawPtrT> tmp257;
    compiler::TNode<IntPtrT> tmp258;
    compiler::TNode<JSReceiver> tmp259;
    compiler::TNode<Number> tmp260;
    compiler::TNode<Number> tmp261;
    compiler::TNode<Smi> tmp262;
    compiler::TNode<Number> tmp263;
    compiler::TNode<Smi> tmp264;
    compiler::TNode<Smi> tmp265;
    compiler::TNode<Smi> tmp266;
    compiler::TNode<Smi> tmp267;
    compiler::TNode<Smi> tmp268;
    compiler::TNode<JSArray> tmp269;
    compiler::TNode<Map> tmp270;
    compiler::TNode<Int32T> tmp271;
    ca_.Bind(&block19, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271);
    ca_.Goto(&block1);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp272;
    compiler::TNode<RawPtrT> tmp273;
    compiler::TNode<RawPtrT> tmp274;
    compiler::TNode<IntPtrT> tmp275;
    compiler::TNode<JSReceiver> tmp276;
    compiler::TNode<Number> tmp277;
    compiler::TNode<Number> tmp278;
    compiler::TNode<Smi> tmp279;
    compiler::TNode<Number> tmp280;
    compiler::TNode<Smi> tmp281;
    compiler::TNode<Smi> tmp282;
    compiler::TNode<Smi> tmp283;
    compiler::TNode<Smi> tmp284;
    compiler::TNode<Smi> tmp285;
    compiler::TNode<JSArray> tmp286;
    compiler::TNode<Map> tmp287;
    compiler::TNode<Int32T> tmp288;
    ca_.Bind(&block20, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 134);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 135);
    compiler::TNode<IntPtrT> tmp289;
    USE(tmp289);
    tmp289 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    ca_.Goto(&block23, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp288, tmp289);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp290;
    compiler::TNode<RawPtrT> tmp291;
    compiler::TNode<RawPtrT> tmp292;
    compiler::TNode<IntPtrT> tmp293;
    compiler::TNode<JSReceiver> tmp294;
    compiler::TNode<Number> tmp295;
    compiler::TNode<Number> tmp296;
    compiler::TNode<Smi> tmp297;
    compiler::TNode<Number> tmp298;
    compiler::TNode<Smi> tmp299;
    compiler::TNode<Smi> tmp300;
    compiler::TNode<Smi> tmp301;
    compiler::TNode<Smi> tmp302;
    compiler::TNode<Smi> tmp303;
    compiler::TNode<JSArray> tmp304;
    compiler::TNode<Map> tmp305;
    compiler::TNode<Int32T> tmp306;
    compiler::TNode<Int32T> tmp307;
    compiler::TNode<IntPtrT> tmp308;
    ca_.Bind(&block23, &tmp290, &tmp291, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308);
    compiler::TNode<BoolT> tmp309;
    USE(tmp309);
    tmp309 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp308}, compiler::TNode<IntPtrT>{tmp293}));
    ca_.Branch(tmp309, &block21, &block22, tmp290, tmp291, tmp292, tmp293, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp310;
    compiler::TNode<RawPtrT> tmp311;
    compiler::TNode<RawPtrT> tmp312;
    compiler::TNode<IntPtrT> tmp313;
    compiler::TNode<JSReceiver> tmp314;
    compiler::TNode<Number> tmp315;
    compiler::TNode<Number> tmp316;
    compiler::TNode<Smi> tmp317;
    compiler::TNode<Number> tmp318;
    compiler::TNode<Smi> tmp319;
    compiler::TNode<Smi> tmp320;
    compiler::TNode<Smi> tmp321;
    compiler::TNode<Smi> tmp322;
    compiler::TNode<Smi> tmp323;
    compiler::TNode<JSArray> tmp324;
    compiler::TNode<Map> tmp325;
    compiler::TNode<Int32T> tmp326;
    compiler::TNode<Int32T> tmp327;
    compiler::TNode<IntPtrT> tmp328;
    ca_.Bind(&block21, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 136);
    compiler::TNode<Object> tmp329;
    USE(tmp329);
    tmp329 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp311}, compiler::TNode<RawPtrT>{tmp312}, compiler::TNode<IntPtrT>{tmp313}}, compiler::TNode<IntPtrT>{tmp328}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 137);
    compiler::TNode<BoolT> tmp330;
    USE(tmp330);
    tmp330 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsFastSmiElementsKind(compiler::TNode<Int32T>{tmp326}));
    ca_.Branch(tmp330, &block25, &block26, tmp310, tmp311, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp331;
    compiler::TNode<RawPtrT> tmp332;
    compiler::TNode<RawPtrT> tmp333;
    compiler::TNode<IntPtrT> tmp334;
    compiler::TNode<JSReceiver> tmp335;
    compiler::TNode<Number> tmp336;
    compiler::TNode<Number> tmp337;
    compiler::TNode<Smi> tmp338;
    compiler::TNode<Number> tmp339;
    compiler::TNode<Smi> tmp340;
    compiler::TNode<Smi> tmp341;
    compiler::TNode<Smi> tmp342;
    compiler::TNode<Smi> tmp343;
    compiler::TNode<Smi> tmp344;
    compiler::TNode<JSArray> tmp345;
    compiler::TNode<Map> tmp346;
    compiler::TNode<Int32T> tmp347;
    compiler::TNode<Int32T> tmp348;
    compiler::TNode<IntPtrT> tmp349;
    compiler::TNode<Object> tmp350;
    ca_.Bind(&block25, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 138);
    compiler::TNode<BoolT> tmp351;
    USE(tmp351);
    tmp351 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).TaggedIsNotSmi(compiler::TNode<Object>{tmp350}));
    ca_.Branch(tmp351, &block28, &block29, tmp331, tmp332, tmp333, tmp334, tmp335, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp352;
    compiler::TNode<RawPtrT> tmp353;
    compiler::TNode<RawPtrT> tmp354;
    compiler::TNode<IntPtrT> tmp355;
    compiler::TNode<JSReceiver> tmp356;
    compiler::TNode<Number> tmp357;
    compiler::TNode<Number> tmp358;
    compiler::TNode<Smi> tmp359;
    compiler::TNode<Number> tmp360;
    compiler::TNode<Smi> tmp361;
    compiler::TNode<Smi> tmp362;
    compiler::TNode<Smi> tmp363;
    compiler::TNode<Smi> tmp364;
    compiler::TNode<Smi> tmp365;
    compiler::TNode<JSArray> tmp366;
    compiler::TNode<Map> tmp367;
    compiler::TNode<Int32T> tmp368;
    compiler::TNode<Int32T> tmp369;
    compiler::TNode<IntPtrT> tmp370;
    compiler::TNode<Object> tmp371;
    ca_.Bind(&block28, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368, &tmp369, &tmp370, &tmp371);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 139);
    compiler::TNode<HeapObject> tmp372;
    USE(tmp372);
    tmp372 = ca_.UncheckedCast<HeapObject>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10HeapObject(compiler::TNode<Context>{tmp352}, compiler::TNode<Object>{tmp371}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 140);
    compiler::TNode<BoolT> tmp373;
    USE(tmp373);
    tmp373 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsHeapNumber(compiler::TNode<HeapObject>{tmp372}));
    ca_.Branch(tmp373, &block30, &block31, tmp352, tmp353, tmp354, tmp355, tmp356, tmp357, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368, tmp369, tmp370, tmp371, tmp372);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp374;
    compiler::TNode<RawPtrT> tmp375;
    compiler::TNode<RawPtrT> tmp376;
    compiler::TNode<IntPtrT> tmp377;
    compiler::TNode<JSReceiver> tmp378;
    compiler::TNode<Number> tmp379;
    compiler::TNode<Number> tmp380;
    compiler::TNode<Smi> tmp381;
    compiler::TNode<Number> tmp382;
    compiler::TNode<Smi> tmp383;
    compiler::TNode<Smi> tmp384;
    compiler::TNode<Smi> tmp385;
    compiler::TNode<Smi> tmp386;
    compiler::TNode<Smi> tmp387;
    compiler::TNode<JSArray> tmp388;
    compiler::TNode<Map> tmp389;
    compiler::TNode<Int32T> tmp390;
    compiler::TNode<Int32T> tmp391;
    compiler::TNode<IntPtrT> tmp392;
    compiler::TNode<Object> tmp393;
    compiler::TNode<HeapObject> tmp394;
    ca_.Bind(&block30, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 141);
    compiler::TNode<Int32T> tmp395;
    USE(tmp395);
    tmp395 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).AllowDoubleElements(compiler::TNode<Int32T>{tmp390}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 140);
    ca_.Goto(&block33, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp396;
    compiler::TNode<RawPtrT> tmp397;
    compiler::TNode<RawPtrT> tmp398;
    compiler::TNode<IntPtrT> tmp399;
    compiler::TNode<JSReceiver> tmp400;
    compiler::TNode<Number> tmp401;
    compiler::TNode<Number> tmp402;
    compiler::TNode<Smi> tmp403;
    compiler::TNode<Number> tmp404;
    compiler::TNode<Smi> tmp405;
    compiler::TNode<Smi> tmp406;
    compiler::TNode<Smi> tmp407;
    compiler::TNode<Smi> tmp408;
    compiler::TNode<Smi> tmp409;
    compiler::TNode<JSArray> tmp410;
    compiler::TNode<Map> tmp411;
    compiler::TNode<Int32T> tmp412;
    compiler::TNode<Int32T> tmp413;
    compiler::TNode<IntPtrT> tmp414;
    compiler::TNode<Object> tmp415;
    compiler::TNode<HeapObject> tmp416;
    ca_.Bind(&block31, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 142);
    compiler::TNode<Int32T> tmp417;
    USE(tmp417);
    tmp417 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).AllowNonNumberElements(compiler::TNode<Int32T>{tmp412}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 140);
    ca_.Goto(&block32, tmp396, tmp397, tmp398, tmp399, tmp400, tmp401, tmp402, tmp403, tmp404, tmp405, tmp406, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp418;
    compiler::TNode<RawPtrT> tmp419;
    compiler::TNode<RawPtrT> tmp420;
    compiler::TNode<IntPtrT> tmp421;
    compiler::TNode<JSReceiver> tmp422;
    compiler::TNode<Number> tmp423;
    compiler::TNode<Number> tmp424;
    compiler::TNode<Smi> tmp425;
    compiler::TNode<Number> tmp426;
    compiler::TNode<Smi> tmp427;
    compiler::TNode<Smi> tmp428;
    compiler::TNode<Smi> tmp429;
    compiler::TNode<Smi> tmp430;
    compiler::TNode<Smi> tmp431;
    compiler::TNode<JSArray> tmp432;
    compiler::TNode<Map> tmp433;
    compiler::TNode<Int32T> tmp434;
    compiler::TNode<Int32T> tmp435;
    compiler::TNode<IntPtrT> tmp436;
    compiler::TNode<Object> tmp437;
    compiler::TNode<HeapObject> tmp438;
    compiler::TNode<Int32T> tmp439;
    ca_.Bind(&block33, &tmp418, &tmp419, &tmp420, &tmp421, &tmp422, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439);
    ca_.Goto(&block32, tmp418, tmp419, tmp420, tmp421, tmp422, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438, tmp439);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp440;
    compiler::TNode<RawPtrT> tmp441;
    compiler::TNode<RawPtrT> tmp442;
    compiler::TNode<IntPtrT> tmp443;
    compiler::TNode<JSReceiver> tmp444;
    compiler::TNode<Number> tmp445;
    compiler::TNode<Number> tmp446;
    compiler::TNode<Smi> tmp447;
    compiler::TNode<Number> tmp448;
    compiler::TNode<Smi> tmp449;
    compiler::TNode<Smi> tmp450;
    compiler::TNode<Smi> tmp451;
    compiler::TNode<Smi> tmp452;
    compiler::TNode<Smi> tmp453;
    compiler::TNode<JSArray> tmp454;
    compiler::TNode<Map> tmp455;
    compiler::TNode<Int32T> tmp456;
    compiler::TNode<Int32T> tmp457;
    compiler::TNode<IntPtrT> tmp458;
    compiler::TNode<Object> tmp459;
    compiler::TNode<HeapObject> tmp460;
    compiler::TNode<Int32T> tmp461;
    ca_.Bind(&block32, &tmp440, &tmp441, &tmp442, &tmp443, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 138);
    ca_.Goto(&block29, tmp440, tmp441, tmp442, tmp443, tmp444, tmp445, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452, tmp453, tmp454, tmp455, tmp461, tmp457, tmp458, tmp459);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp462;
    compiler::TNode<RawPtrT> tmp463;
    compiler::TNode<RawPtrT> tmp464;
    compiler::TNode<IntPtrT> tmp465;
    compiler::TNode<JSReceiver> tmp466;
    compiler::TNode<Number> tmp467;
    compiler::TNode<Number> tmp468;
    compiler::TNode<Smi> tmp469;
    compiler::TNode<Number> tmp470;
    compiler::TNode<Smi> tmp471;
    compiler::TNode<Smi> tmp472;
    compiler::TNode<Smi> tmp473;
    compiler::TNode<Smi> tmp474;
    compiler::TNode<Smi> tmp475;
    compiler::TNode<JSArray> tmp476;
    compiler::TNode<Map> tmp477;
    compiler::TNode<Int32T> tmp478;
    compiler::TNode<Int32T> tmp479;
    compiler::TNode<IntPtrT> tmp480;
    compiler::TNode<Object> tmp481;
    ca_.Bind(&block29, &tmp462, &tmp463, &tmp464, &tmp465, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478, &tmp479, &tmp480, &tmp481);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 137);
    ca_.Goto(&block27, tmp462, tmp463, tmp464, tmp465, tmp466, tmp467, tmp468, tmp469, tmp470, tmp471, tmp472, tmp473, tmp474, tmp475, tmp476, tmp477, tmp478, tmp479, tmp480, tmp481);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp482;
    compiler::TNode<RawPtrT> tmp483;
    compiler::TNode<RawPtrT> tmp484;
    compiler::TNode<IntPtrT> tmp485;
    compiler::TNode<JSReceiver> tmp486;
    compiler::TNode<Number> tmp487;
    compiler::TNode<Number> tmp488;
    compiler::TNode<Smi> tmp489;
    compiler::TNode<Number> tmp490;
    compiler::TNode<Smi> tmp491;
    compiler::TNode<Smi> tmp492;
    compiler::TNode<Smi> tmp493;
    compiler::TNode<Smi> tmp494;
    compiler::TNode<Smi> tmp495;
    compiler::TNode<JSArray> tmp496;
    compiler::TNode<Map> tmp497;
    compiler::TNode<Int32T> tmp498;
    compiler::TNode<Int32T> tmp499;
    compiler::TNode<IntPtrT> tmp500;
    compiler::TNode<Object> tmp501;
    ca_.Bind(&block26, &tmp482, &tmp483, &tmp484, &tmp485, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500, &tmp501);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 144);
    compiler::TNode<BoolT> tmp502;
    USE(tmp502);
    tmp502 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDoubleElementsKind(compiler::TNode<Int32T>{tmp498}));
    ca_.Branch(tmp502, &block34, &block35, tmp482, tmp483, tmp484, tmp485, tmp486, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492, tmp493, tmp494, tmp495, tmp496, tmp497, tmp498, tmp499, tmp500, tmp501);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp503;
    compiler::TNode<RawPtrT> tmp504;
    compiler::TNode<RawPtrT> tmp505;
    compiler::TNode<IntPtrT> tmp506;
    compiler::TNode<JSReceiver> tmp507;
    compiler::TNode<Number> tmp508;
    compiler::TNode<Number> tmp509;
    compiler::TNode<Smi> tmp510;
    compiler::TNode<Number> tmp511;
    compiler::TNode<Smi> tmp512;
    compiler::TNode<Smi> tmp513;
    compiler::TNode<Smi> tmp514;
    compiler::TNode<Smi> tmp515;
    compiler::TNode<Smi> tmp516;
    compiler::TNode<JSArray> tmp517;
    compiler::TNode<Map> tmp518;
    compiler::TNode<Int32T> tmp519;
    compiler::TNode<Int32T> tmp520;
    compiler::TNode<IntPtrT> tmp521;
    compiler::TNode<Object> tmp522;
    ca_.Bind(&block34, &tmp503, &tmp504, &tmp505, &tmp506, &tmp507, &tmp508, &tmp509, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518, &tmp519, &tmp520, &tmp521, &tmp522);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 145);
    compiler::TNode<BoolT> tmp523;
    USE(tmp523);
    tmp523 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNumber(compiler::TNode<Object>{tmp522}));
    compiler::TNode<BoolT> tmp524;
    USE(tmp524);
    tmp524 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp523}));
    ca_.Branch(tmp524, &block36, &block37, tmp503, tmp504, tmp505, tmp506, tmp507, tmp508, tmp509, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517, tmp518, tmp519, tmp520, tmp521, tmp522);
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp525;
    compiler::TNode<RawPtrT> tmp526;
    compiler::TNode<RawPtrT> tmp527;
    compiler::TNode<IntPtrT> tmp528;
    compiler::TNode<JSReceiver> tmp529;
    compiler::TNode<Number> tmp530;
    compiler::TNode<Number> tmp531;
    compiler::TNode<Smi> tmp532;
    compiler::TNode<Number> tmp533;
    compiler::TNode<Smi> tmp534;
    compiler::TNode<Smi> tmp535;
    compiler::TNode<Smi> tmp536;
    compiler::TNode<Smi> tmp537;
    compiler::TNode<Smi> tmp538;
    compiler::TNode<JSArray> tmp539;
    compiler::TNode<Map> tmp540;
    compiler::TNode<Int32T> tmp541;
    compiler::TNode<Int32T> tmp542;
    compiler::TNode<IntPtrT> tmp543;
    compiler::TNode<Object> tmp544;
    ca_.Bind(&block36, &tmp525, &tmp526, &tmp527, &tmp528, &tmp529, &tmp530, &tmp531, &tmp532, &tmp533, &tmp534, &tmp535, &tmp536, &tmp537, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 146);
    compiler::TNode<Int32T> tmp545;
    USE(tmp545);
    tmp545 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).AllowNonNumberElements(compiler::TNode<Int32T>{tmp541}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 145);
    ca_.Goto(&block37, tmp525, tmp526, tmp527, tmp528, tmp529, tmp530, tmp531, tmp532, tmp533, tmp534, tmp535, tmp536, tmp537, tmp538, tmp539, tmp540, tmp545, tmp542, tmp543, tmp544);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp546;
    compiler::TNode<RawPtrT> tmp547;
    compiler::TNode<RawPtrT> tmp548;
    compiler::TNode<IntPtrT> tmp549;
    compiler::TNode<JSReceiver> tmp550;
    compiler::TNode<Number> tmp551;
    compiler::TNode<Number> tmp552;
    compiler::TNode<Smi> tmp553;
    compiler::TNode<Number> tmp554;
    compiler::TNode<Smi> tmp555;
    compiler::TNode<Smi> tmp556;
    compiler::TNode<Smi> tmp557;
    compiler::TNode<Smi> tmp558;
    compiler::TNode<Smi> tmp559;
    compiler::TNode<JSArray> tmp560;
    compiler::TNode<Map> tmp561;
    compiler::TNode<Int32T> tmp562;
    compiler::TNode<Int32T> tmp563;
    compiler::TNode<IntPtrT> tmp564;
    compiler::TNode<Object> tmp565;
    ca_.Bind(&block37, &tmp546, &tmp547, &tmp548, &tmp549, &tmp550, &tmp551, &tmp552, &tmp553, &tmp554, &tmp555, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563, &tmp564, &tmp565);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 144);
    ca_.Goto(&block35, tmp546, tmp547, tmp548, tmp549, tmp550, tmp551, tmp552, tmp553, tmp554, tmp555, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562, tmp563, tmp564, tmp565);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp566;
    compiler::TNode<RawPtrT> tmp567;
    compiler::TNode<RawPtrT> tmp568;
    compiler::TNode<IntPtrT> tmp569;
    compiler::TNode<JSReceiver> tmp570;
    compiler::TNode<Number> tmp571;
    compiler::TNode<Number> tmp572;
    compiler::TNode<Smi> tmp573;
    compiler::TNode<Number> tmp574;
    compiler::TNode<Smi> tmp575;
    compiler::TNode<Smi> tmp576;
    compiler::TNode<Smi> tmp577;
    compiler::TNode<Smi> tmp578;
    compiler::TNode<Smi> tmp579;
    compiler::TNode<JSArray> tmp580;
    compiler::TNode<Map> tmp581;
    compiler::TNode<Int32T> tmp582;
    compiler::TNode<Int32T> tmp583;
    compiler::TNode<IntPtrT> tmp584;
    compiler::TNode<Object> tmp585;
    ca_.Bind(&block35, &tmp566, &tmp567, &tmp568, &tmp569, &tmp570, &tmp571, &tmp572, &tmp573, &tmp574, &tmp575, &tmp576, &tmp577, &tmp578, &tmp579, &tmp580, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 137);
    ca_.Goto(&block27, tmp566, tmp567, tmp568, tmp569, tmp570, tmp571, tmp572, tmp573, tmp574, tmp575, tmp576, tmp577, tmp578, tmp579, tmp580, tmp581, tmp582, tmp583, tmp584, tmp585);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp586;
    compiler::TNode<RawPtrT> tmp587;
    compiler::TNode<RawPtrT> tmp588;
    compiler::TNode<IntPtrT> tmp589;
    compiler::TNode<JSReceiver> tmp590;
    compiler::TNode<Number> tmp591;
    compiler::TNode<Number> tmp592;
    compiler::TNode<Smi> tmp593;
    compiler::TNode<Number> tmp594;
    compiler::TNode<Smi> tmp595;
    compiler::TNode<Smi> tmp596;
    compiler::TNode<Smi> tmp597;
    compiler::TNode<Smi> tmp598;
    compiler::TNode<Smi> tmp599;
    compiler::TNode<JSArray> tmp600;
    compiler::TNode<Map> tmp601;
    compiler::TNode<Int32T> tmp602;
    compiler::TNode<Int32T> tmp603;
    compiler::TNode<IntPtrT> tmp604;
    compiler::TNode<Object> tmp605;
    ca_.Bind(&block27, &tmp586, &tmp587, &tmp588, &tmp589, &tmp590, &tmp591, &tmp592, &tmp593, &tmp594, &tmp595, &tmp596, &tmp597, &tmp598, &tmp599, &tmp600, &tmp601, &tmp602, &tmp603, &tmp604, &tmp605);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 135);
    ca_.Goto(&block24, tmp586, tmp587, tmp588, tmp589, tmp590, tmp591, tmp592, tmp593, tmp594, tmp595, tmp596, tmp597, tmp598, tmp599, tmp600, tmp601, tmp602, tmp603, tmp604);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp606;
    compiler::TNode<RawPtrT> tmp607;
    compiler::TNode<RawPtrT> tmp608;
    compiler::TNode<IntPtrT> tmp609;
    compiler::TNode<JSReceiver> tmp610;
    compiler::TNode<Number> tmp611;
    compiler::TNode<Number> tmp612;
    compiler::TNode<Smi> tmp613;
    compiler::TNode<Number> tmp614;
    compiler::TNode<Smi> tmp615;
    compiler::TNode<Smi> tmp616;
    compiler::TNode<Smi> tmp617;
    compiler::TNode<Smi> tmp618;
    compiler::TNode<Smi> tmp619;
    compiler::TNode<JSArray> tmp620;
    compiler::TNode<Map> tmp621;
    compiler::TNode<Int32T> tmp622;
    compiler::TNode<Int32T> tmp623;
    compiler::TNode<IntPtrT> tmp624;
    ca_.Bind(&block24, &tmp606, &tmp607, &tmp608, &tmp609, &tmp610, &tmp611, &tmp612, &tmp613, &tmp614, &tmp615, &tmp616, &tmp617, &tmp618, &tmp619, &tmp620, &tmp621, &tmp622, &tmp623, &tmp624);
    compiler::TNode<IntPtrT> tmp625;
    USE(tmp625);
    tmp625 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp626;
    USE(tmp626);
    tmp626 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp624}, compiler::TNode<IntPtrT>{tmp625}));
    ca_.Goto(&block23, tmp606, tmp607, tmp608, tmp609, tmp610, tmp611, tmp612, tmp613, tmp614, tmp615, tmp616, tmp617, tmp618, tmp619, tmp620, tmp621, tmp622, tmp623, tmp626);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp627;
    compiler::TNode<RawPtrT> tmp628;
    compiler::TNode<RawPtrT> tmp629;
    compiler::TNode<IntPtrT> tmp630;
    compiler::TNode<JSReceiver> tmp631;
    compiler::TNode<Number> tmp632;
    compiler::TNode<Number> tmp633;
    compiler::TNode<Smi> tmp634;
    compiler::TNode<Number> tmp635;
    compiler::TNode<Smi> tmp636;
    compiler::TNode<Smi> tmp637;
    compiler::TNode<Smi> tmp638;
    compiler::TNode<Smi> tmp639;
    compiler::TNode<Smi> tmp640;
    compiler::TNode<JSArray> tmp641;
    compiler::TNode<Map> tmp642;
    compiler::TNode<Int32T> tmp643;
    compiler::TNode<Int32T> tmp644;
    compiler::TNode<IntPtrT> tmp645;
    ca_.Bind(&block22, &tmp627, &tmp628, &tmp629, &tmp630, &tmp631, &tmp632, &tmp633, &tmp634, &tmp635, &tmp636, &tmp637, &tmp638, &tmp639, &tmp640, &tmp641, &tmp642, &tmp643, &tmp644, &tmp645);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 151);
    compiler::TNode<BoolT> tmp646;
    USE(tmp646);
    tmp646 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).ElementsKindNotEqual(compiler::TNode<Int32T>{tmp643}, compiler::TNode<Int32T>{tmp644}));
    ca_.Branch(tmp646, &block38, &block39, tmp627, tmp628, tmp629, tmp630, tmp631, tmp632, tmp633, tmp634, tmp635, tmp636, tmp637, tmp638, tmp639, tmp640, tmp641, tmp642, tmp643, tmp644);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp647;
    compiler::TNode<RawPtrT> tmp648;
    compiler::TNode<RawPtrT> tmp649;
    compiler::TNode<IntPtrT> tmp650;
    compiler::TNode<JSReceiver> tmp651;
    compiler::TNode<Number> tmp652;
    compiler::TNode<Number> tmp653;
    compiler::TNode<Smi> tmp654;
    compiler::TNode<Number> tmp655;
    compiler::TNode<Smi> tmp656;
    compiler::TNode<Smi> tmp657;
    compiler::TNode<Smi> tmp658;
    compiler::TNode<Smi> tmp659;
    compiler::TNode<Smi> tmp660;
    compiler::TNode<JSArray> tmp661;
    compiler::TNode<Map> tmp662;
    compiler::TNode<Int32T> tmp663;
    compiler::TNode<Int32T> tmp664;
    ca_.Bind(&block38, &tmp647, &tmp648, &tmp649, &tmp650, &tmp651, &tmp652, &tmp653, &tmp654, &tmp655, &tmp656, &tmp657, &tmp658, &tmp659, &tmp660, &tmp661, &tmp662, &tmp663, &tmp664);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 152);
    compiler::TNode<Int32T> tmp665;
    USE(tmp665);
    tmp665 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).Convert7ATint3214ATElementsKind(compiler::TNode<Int32T>{tmp663}));
    compiler::TNode<Smi> tmp666;
    USE(tmp666);
    tmp666 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi7ATint32(compiler::TNode<Int32T>{tmp665}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 153);
    CodeStubAssembler(state_).CallRuntime(Runtime::kTransitionElementsKindWithKind, tmp647, tmp661, tmp666);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 151);
    ca_.Goto(&block39, tmp647, tmp648, tmp649, tmp650, tmp651, tmp652, tmp653, tmp654, tmp655, tmp656, tmp657, tmp658, tmp659, tmp660, tmp661, tmp662, tmp663, tmp664);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp668;
    compiler::TNode<RawPtrT> tmp669;
    compiler::TNode<RawPtrT> tmp670;
    compiler::TNode<IntPtrT> tmp671;
    compiler::TNode<JSReceiver> tmp672;
    compiler::TNode<Number> tmp673;
    compiler::TNode<Number> tmp674;
    compiler::TNode<Smi> tmp675;
    compiler::TNode<Number> tmp676;
    compiler::TNode<Smi> tmp677;
    compiler::TNode<Smi> tmp678;
    compiler::TNode<Smi> tmp679;
    compiler::TNode<Smi> tmp680;
    compiler::TNode<Smi> tmp681;
    compiler::TNode<JSArray> tmp682;
    compiler::TNode<Map> tmp683;
    compiler::TNode<Int32T> tmp684;
    compiler::TNode<Int32T> tmp685;
    ca_.Bind(&block39, &tmp668, &tmp669, &tmp670, &tmp671, &tmp672, &tmp673, &tmp674, &tmp675, &tmp676, &tmp677, &tmp678, &tmp679, &tmp680, &tmp681, &tmp682, &tmp683, &tmp684, &tmp685);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 157);
    compiler::TNode<IntPtrT> tmp686 = ca_.IntPtrConstant(JSArray::kLengthOffset);
    USE(tmp686);
    compiler::TNode<Number>tmp687 = CodeStubAssembler(state_).LoadReference<Number>(CodeStubAssembler::Reference{tmp682, tmp686});
    compiler::TNode<Smi> tmp688;
    USE(tmp688);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp688 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp687}, &label0);
    ca_.Goto(&block40, tmp668, tmp669, tmp670, tmp671, tmp672, tmp673, tmp674, tmp675, tmp676, tmp677, tmp678, tmp679, tmp680, tmp681, tmp682, tmp683, tmp684, tmp685, tmp687, tmp688);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block41, tmp668, tmp669, tmp670, tmp671, tmp672, tmp673, tmp674, tmp675, tmp676, tmp677, tmp678, tmp679, tmp680, tmp681, tmp682, tmp683, tmp684, tmp685, tmp687);
    }
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp689;
    compiler::TNode<RawPtrT> tmp690;
    compiler::TNode<RawPtrT> tmp691;
    compiler::TNode<IntPtrT> tmp692;
    compiler::TNode<JSReceiver> tmp693;
    compiler::TNode<Number> tmp694;
    compiler::TNode<Number> tmp695;
    compiler::TNode<Smi> tmp696;
    compiler::TNode<Number> tmp697;
    compiler::TNode<Smi> tmp698;
    compiler::TNode<Smi> tmp699;
    compiler::TNode<Smi> tmp700;
    compiler::TNode<Smi> tmp701;
    compiler::TNode<Smi> tmp702;
    compiler::TNode<JSArray> tmp703;
    compiler::TNode<Map> tmp704;
    compiler::TNode<Int32T> tmp705;
    compiler::TNode<Int32T> tmp706;
    compiler::TNode<Number> tmp707;
    ca_.Bind(&block41, &tmp689, &tmp690, &tmp691, &tmp692, &tmp693, &tmp694, &tmp695, &tmp696, &tmp697, &tmp698, &tmp699, &tmp700, &tmp701, &tmp702, &tmp703, &tmp704, &tmp705, &tmp706, &tmp707);
    ca_.Goto(&block1);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp708;
    compiler::TNode<RawPtrT> tmp709;
    compiler::TNode<RawPtrT> tmp710;
    compiler::TNode<IntPtrT> tmp711;
    compiler::TNode<JSReceiver> tmp712;
    compiler::TNode<Number> tmp713;
    compiler::TNode<Number> tmp714;
    compiler::TNode<Smi> tmp715;
    compiler::TNode<Number> tmp716;
    compiler::TNode<Smi> tmp717;
    compiler::TNode<Smi> tmp718;
    compiler::TNode<Smi> tmp719;
    compiler::TNode<Smi> tmp720;
    compiler::TNode<Smi> tmp721;
    compiler::TNode<JSArray> tmp722;
    compiler::TNode<Map> tmp723;
    compiler::TNode<Int32T> tmp724;
    compiler::TNode<Int32T> tmp725;
    compiler::TNode<Number> tmp726;
    compiler::TNode<Smi> tmp727;
    ca_.Bind(&block40, &tmp708, &tmp709, &tmp710, &tmp711, &tmp712, &tmp713, &tmp714, &tmp715, &tmp716, &tmp717, &tmp718, &tmp719, &tmp720, &tmp721, &tmp722, &tmp723, &tmp724, &tmp725, &tmp726, &tmp727);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 158);
    compiler::TNode<BoolT> tmp728;
    USE(tmp728);
    tmp728 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiNotEqual(compiler::TNode<Smi>{tmp717}, compiler::TNode<Smi>{tmp727}));
    ca_.Branch(tmp728, &block42, &block43, tmp708, tmp709, tmp710, tmp711, tmp712, tmp713, tmp714, tmp715, tmp716, tmp717, tmp718, tmp719, tmp720, tmp721, tmp722, tmp723, tmp724, tmp725, tmp727);
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp729;
    compiler::TNode<RawPtrT> tmp730;
    compiler::TNode<RawPtrT> tmp731;
    compiler::TNode<IntPtrT> tmp732;
    compiler::TNode<JSReceiver> tmp733;
    compiler::TNode<Number> tmp734;
    compiler::TNode<Number> tmp735;
    compiler::TNode<Smi> tmp736;
    compiler::TNode<Number> tmp737;
    compiler::TNode<Smi> tmp738;
    compiler::TNode<Smi> tmp739;
    compiler::TNode<Smi> tmp740;
    compiler::TNode<Smi> tmp741;
    compiler::TNode<Smi> tmp742;
    compiler::TNode<JSArray> tmp743;
    compiler::TNode<Map> tmp744;
    compiler::TNode<Int32T> tmp745;
    compiler::TNode<Int32T> tmp746;
    compiler::TNode<Smi> tmp747;
    ca_.Bind(&block42, &tmp729, &tmp730, &tmp731, &tmp732, &tmp733, &tmp734, &tmp735, &tmp736, &tmp737, &tmp738, &tmp739, &tmp740, &tmp741, &tmp742, &tmp743, &tmp744, &tmp745, &tmp746, &tmp747);
    ca_.Goto(&block1);
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp748;
    compiler::TNode<RawPtrT> tmp749;
    compiler::TNode<RawPtrT> tmp750;
    compiler::TNode<IntPtrT> tmp751;
    compiler::TNode<JSReceiver> tmp752;
    compiler::TNode<Number> tmp753;
    compiler::TNode<Number> tmp754;
    compiler::TNode<Smi> tmp755;
    compiler::TNode<Number> tmp756;
    compiler::TNode<Smi> tmp757;
    compiler::TNode<Smi> tmp758;
    compiler::TNode<Smi> tmp759;
    compiler::TNode<Smi> tmp760;
    compiler::TNode<Smi> tmp761;
    compiler::TNode<JSArray> tmp762;
    compiler::TNode<Map> tmp763;
    compiler::TNode<Int32T> tmp764;
    compiler::TNode<Int32T> tmp765;
    compiler::TNode<Smi> tmp766;
    ca_.Bind(&block43, &tmp748, &tmp749, &tmp750, &tmp751, &tmp752, &tmp753, &tmp754, &tmp755, &tmp756, &tmp757, &tmp758, &tmp759, &tmp760, &tmp761, &tmp762, &tmp763, &tmp764, &tmp765, &tmp766);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 161);
    compiler::TNode<JSArray> tmp767;
    tmp767 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kExtractFastJSArray, tmp748, tmp762, tmp758, tmp759));
    USE(tmp767);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 160);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 163);
    compiler::TNode<Smi> tmp768;
    USE(tmp768);
    tmp768 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp769;
    USE(tmp769);
    tmp769 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp761}, compiler::TNode<Smi>{tmp768}));
    ca_.Branch(tmp769, &block44, &block45, tmp748, tmp749, tmp750, tmp751, tmp752, tmp753, tmp754, tmp755, tmp756, tmp757, tmp758, tmp759, tmp760, tmp761, tmp762, tmp763, tmp764, tmp765, tmp766, tmp767);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp770;
    compiler::TNode<RawPtrT> tmp771;
    compiler::TNode<RawPtrT> tmp772;
    compiler::TNode<IntPtrT> tmp773;
    compiler::TNode<JSReceiver> tmp774;
    compiler::TNode<Number> tmp775;
    compiler::TNode<Number> tmp776;
    compiler::TNode<Smi> tmp777;
    compiler::TNode<Number> tmp778;
    compiler::TNode<Smi> tmp779;
    compiler::TNode<Smi> tmp780;
    compiler::TNode<Smi> tmp781;
    compiler::TNode<Smi> tmp782;
    compiler::TNode<Smi> tmp783;
    compiler::TNode<JSArray> tmp784;
    compiler::TNode<Map> tmp785;
    compiler::TNode<Int32T> tmp786;
    compiler::TNode<Int32T> tmp787;
    compiler::TNode<Smi> tmp788;
    compiler::TNode<JSArray> tmp789;
    ca_.Bind(&block44, &tmp770, &tmp771, &tmp772, &tmp773, &tmp774, &tmp775, &tmp776, &tmp777, &tmp778, &tmp779, &tmp780, &tmp781, &tmp782, &tmp783, &tmp784, &tmp785, &tmp786, &tmp787, &tmp788, &tmp789);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 164);
    compiler::TNode<IntPtrT> tmp790 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp790);
    compiler::TNode<FixedArray> tmp791;
    USE(tmp791);
    tmp791 = BaseBuiltinsFromDSLAssembler(state_).kEmptyFixedArray();
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp784, tmp790}, tmp791);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 165);
    compiler::TNode<IntPtrT> tmp792 = ca_.IntPtrConstant(JSArray::kLengthOffset);
    USE(tmp792);
    compiler::TNode<Number> tmp793;
    USE(tmp793);
    tmp793 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp784, tmp792}, tmp793);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 166);
    ca_.Goto(&block2, tmp770, tmp771, tmp772, tmp773, tmp774, tmp775, tmp776, tmp777, tmp778, tmp789);
  }

  if (block45.is_used()) {
    compiler::TNode<Context> tmp794;
    compiler::TNode<RawPtrT> tmp795;
    compiler::TNode<RawPtrT> tmp796;
    compiler::TNode<IntPtrT> tmp797;
    compiler::TNode<JSReceiver> tmp798;
    compiler::TNode<Number> tmp799;
    compiler::TNode<Number> tmp800;
    compiler::TNode<Smi> tmp801;
    compiler::TNode<Number> tmp802;
    compiler::TNode<Smi> tmp803;
    compiler::TNode<Smi> tmp804;
    compiler::TNode<Smi> tmp805;
    compiler::TNode<Smi> tmp806;
    compiler::TNode<Smi> tmp807;
    compiler::TNode<JSArray> tmp808;
    compiler::TNode<Map> tmp809;
    compiler::TNode<Int32T> tmp810;
    compiler::TNode<Int32T> tmp811;
    compiler::TNode<Smi> tmp812;
    compiler::TNode<JSArray> tmp813;
    ca_.Bind(&block45, &tmp794, &tmp795, &tmp796, &tmp797, &tmp798, &tmp799, &tmp800, &tmp801, &tmp802, &tmp803, &tmp804, &tmp805, &tmp806, &tmp807, &tmp808, &tmp809, &tmp810, &tmp811, &tmp812, &tmp813);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 169);
    compiler::TNode<BoolT> tmp814;
    USE(tmp814);
    tmp814 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsFastSmiOrTaggedElementsKind(compiler::TNode<Int32T>{tmp810}));
    ca_.Branch(tmp814, &block46, &block47, tmp794, tmp795, tmp796, tmp797, tmp798, tmp799, tmp800, tmp801, tmp802, tmp803, tmp804, tmp805, tmp806, tmp807, tmp808, tmp809, tmp810, tmp811, tmp812, tmp813);
  }

  if (block46.is_used()) {
    compiler::TNode<Context> tmp815;
    compiler::TNode<RawPtrT> tmp816;
    compiler::TNode<RawPtrT> tmp817;
    compiler::TNode<IntPtrT> tmp818;
    compiler::TNode<JSReceiver> tmp819;
    compiler::TNode<Number> tmp820;
    compiler::TNode<Number> tmp821;
    compiler::TNode<Smi> tmp822;
    compiler::TNode<Number> tmp823;
    compiler::TNode<Smi> tmp824;
    compiler::TNode<Smi> tmp825;
    compiler::TNode<Smi> tmp826;
    compiler::TNode<Smi> tmp827;
    compiler::TNode<Smi> tmp828;
    compiler::TNode<JSArray> tmp829;
    compiler::TNode<Map> tmp830;
    compiler::TNode<Int32T> tmp831;
    compiler::TNode<Int32T> tmp832;
    compiler::TNode<Smi> tmp833;
    compiler::TNode<JSArray> tmp834;
    ca_.Bind(&block46, &tmp815, &tmp816, &tmp817, &tmp818, &tmp819, &tmp820, &tmp821, &tmp822, &tmp823, &tmp824, &tmp825, &tmp826, &tmp827, &tmp828, &tmp829, &tmp830, &tmp831, &tmp832, &tmp833, &tmp834);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 171);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 172);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 170);
    compiler::CodeAssemblerLabel label0(&ca_);
    ArraySpliceBuiltinsFromDSLAssembler(state_).FastSplice10FixedArray20UT5ATSmi10HeapObject(compiler::TNode<Context>{tmp815}, BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp816}, compiler::TNode<RawPtrT>{tmp817}, compiler::TNode<IntPtrT>{tmp818}}, compiler::TNode<JSArray>{tmp829}, compiler::TNode<Smi>{tmp833}, compiler::TNode<Smi>{tmp828}, compiler::TNode<Smi>{tmp827}, compiler::TNode<Smi>{tmp825}, compiler::TNode<Smi>{tmp822}, compiler::TNode<Smi>{tmp826}, &label0);
    ca_.Goto(&block49, tmp815, tmp816, tmp817, tmp818, tmp819, tmp820, tmp821, tmp822, tmp823, tmp824, tmp825, tmp826, tmp827, tmp828, tmp829, tmp830, tmp831, tmp832, tmp833, tmp834, tmp816, tmp817, tmp818, tmp829, tmp833, tmp828, tmp827, tmp825, tmp822, tmp826);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block50, tmp815, tmp816, tmp817, tmp818, tmp819, tmp820, tmp821, tmp822, tmp823, tmp824, tmp825, tmp826, tmp827, tmp828, tmp829, tmp830, tmp831, tmp832, tmp833, tmp834, tmp816, tmp817, tmp818, tmp829, tmp833, tmp828, tmp827, tmp825, tmp822, tmp826);
    }
  }

  if (block50.is_used()) {
    compiler::TNode<Context> tmp835;
    compiler::TNode<RawPtrT> tmp836;
    compiler::TNode<RawPtrT> tmp837;
    compiler::TNode<IntPtrT> tmp838;
    compiler::TNode<JSReceiver> tmp839;
    compiler::TNode<Number> tmp840;
    compiler::TNode<Number> tmp841;
    compiler::TNode<Smi> tmp842;
    compiler::TNode<Number> tmp843;
    compiler::TNode<Smi> tmp844;
    compiler::TNode<Smi> tmp845;
    compiler::TNode<Smi> tmp846;
    compiler::TNode<Smi> tmp847;
    compiler::TNode<Smi> tmp848;
    compiler::TNode<JSArray> tmp849;
    compiler::TNode<Map> tmp850;
    compiler::TNode<Int32T> tmp851;
    compiler::TNode<Int32T> tmp852;
    compiler::TNode<Smi> tmp853;
    compiler::TNode<JSArray> tmp854;
    compiler::TNode<RawPtrT> tmp855;
    compiler::TNode<RawPtrT> tmp856;
    compiler::TNode<IntPtrT> tmp857;
    compiler::TNode<JSArray> tmp858;
    compiler::TNode<Smi> tmp859;
    compiler::TNode<Smi> tmp860;
    compiler::TNode<Smi> tmp861;
    compiler::TNode<Smi> tmp862;
    compiler::TNode<Smi> tmp863;
    compiler::TNode<Smi> tmp864;
    ca_.Bind(&block50, &tmp835, &tmp836, &tmp837, &tmp838, &tmp839, &tmp840, &tmp841, &tmp842, &tmp843, &tmp844, &tmp845, &tmp846, &tmp847, &tmp848, &tmp849, &tmp850, &tmp851, &tmp852, &tmp853, &tmp854, &tmp855, &tmp856, &tmp857, &tmp858, &tmp859, &tmp860, &tmp861, &tmp862, &tmp863, &tmp864);
    ca_.Goto(&block1);
  }

  if (block49.is_used()) {
    compiler::TNode<Context> tmp865;
    compiler::TNode<RawPtrT> tmp866;
    compiler::TNode<RawPtrT> tmp867;
    compiler::TNode<IntPtrT> tmp868;
    compiler::TNode<JSReceiver> tmp869;
    compiler::TNode<Number> tmp870;
    compiler::TNode<Number> tmp871;
    compiler::TNode<Smi> tmp872;
    compiler::TNode<Number> tmp873;
    compiler::TNode<Smi> tmp874;
    compiler::TNode<Smi> tmp875;
    compiler::TNode<Smi> tmp876;
    compiler::TNode<Smi> tmp877;
    compiler::TNode<Smi> tmp878;
    compiler::TNode<JSArray> tmp879;
    compiler::TNode<Map> tmp880;
    compiler::TNode<Int32T> tmp881;
    compiler::TNode<Int32T> tmp882;
    compiler::TNode<Smi> tmp883;
    compiler::TNode<JSArray> tmp884;
    compiler::TNode<RawPtrT> tmp885;
    compiler::TNode<RawPtrT> tmp886;
    compiler::TNode<IntPtrT> tmp887;
    compiler::TNode<JSArray> tmp888;
    compiler::TNode<Smi> tmp889;
    compiler::TNode<Smi> tmp890;
    compiler::TNode<Smi> tmp891;
    compiler::TNode<Smi> tmp892;
    compiler::TNode<Smi> tmp893;
    compiler::TNode<Smi> tmp894;
    ca_.Bind(&block49, &tmp865, &tmp866, &tmp867, &tmp868, &tmp869, &tmp870, &tmp871, &tmp872, &tmp873, &tmp874, &tmp875, &tmp876, &tmp877, &tmp878, &tmp879, &tmp880, &tmp881, &tmp882, &tmp883, &tmp884, &tmp885, &tmp886, &tmp887, &tmp888, &tmp889, &tmp890, &tmp891, &tmp892, &tmp893, &tmp894);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 169);
    ca_.Goto(&block48, tmp865, tmp866, tmp867, tmp868, tmp869, tmp870, tmp871, tmp872, tmp873, tmp874, tmp875, tmp876, tmp877, tmp878, tmp879, tmp880, tmp881, tmp882, tmp883, tmp884);
  }

  if (block47.is_used()) {
    compiler::TNode<Context> tmp895;
    compiler::TNode<RawPtrT> tmp896;
    compiler::TNode<RawPtrT> tmp897;
    compiler::TNode<IntPtrT> tmp898;
    compiler::TNode<JSReceiver> tmp899;
    compiler::TNode<Number> tmp900;
    compiler::TNode<Number> tmp901;
    compiler::TNode<Smi> tmp902;
    compiler::TNode<Number> tmp903;
    compiler::TNode<Smi> tmp904;
    compiler::TNode<Smi> tmp905;
    compiler::TNode<Smi> tmp906;
    compiler::TNode<Smi> tmp907;
    compiler::TNode<Smi> tmp908;
    compiler::TNode<JSArray> tmp909;
    compiler::TNode<Map> tmp910;
    compiler::TNode<Int32T> tmp911;
    compiler::TNode<Int32T> tmp912;
    compiler::TNode<Smi> tmp913;
    compiler::TNode<JSArray> tmp914;
    ca_.Bind(&block47, &tmp895, &tmp896, &tmp897, &tmp898, &tmp899, &tmp900, &tmp901, &tmp902, &tmp903, &tmp904, &tmp905, &tmp906, &tmp907, &tmp908, &tmp909, &tmp910, &tmp911, &tmp912, &tmp913, &tmp914);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 175);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 176);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 174);
    compiler::CodeAssemblerLabel label0(&ca_);
    ArraySpliceBuiltinsFromDSLAssembler(state_).FastSplice16FixedDoubleArray20UT5ATSmi10HeapNumber(compiler::TNode<Context>{tmp895}, BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp896}, compiler::TNode<RawPtrT>{tmp897}, compiler::TNode<IntPtrT>{tmp898}}, compiler::TNode<JSArray>{tmp909}, compiler::TNode<Smi>{tmp913}, compiler::TNode<Smi>{tmp908}, compiler::TNode<Smi>{tmp907}, compiler::TNode<Smi>{tmp905}, compiler::TNode<Smi>{tmp902}, compiler::TNode<Smi>{tmp906}, &label0);
    ca_.Goto(&block51, tmp895, tmp896, tmp897, tmp898, tmp899, tmp900, tmp901, tmp902, tmp903, tmp904, tmp905, tmp906, tmp907, tmp908, tmp909, tmp910, tmp911, tmp912, tmp913, tmp914, tmp896, tmp897, tmp898, tmp909, tmp913, tmp908, tmp907, tmp905, tmp902, tmp906);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block52, tmp895, tmp896, tmp897, tmp898, tmp899, tmp900, tmp901, tmp902, tmp903, tmp904, tmp905, tmp906, tmp907, tmp908, tmp909, tmp910, tmp911, tmp912, tmp913, tmp914, tmp896, tmp897, tmp898, tmp909, tmp913, tmp908, tmp907, tmp905, tmp902, tmp906);
    }
  }

  if (block52.is_used()) {
    compiler::TNode<Context> tmp915;
    compiler::TNode<RawPtrT> tmp916;
    compiler::TNode<RawPtrT> tmp917;
    compiler::TNode<IntPtrT> tmp918;
    compiler::TNode<JSReceiver> tmp919;
    compiler::TNode<Number> tmp920;
    compiler::TNode<Number> tmp921;
    compiler::TNode<Smi> tmp922;
    compiler::TNode<Number> tmp923;
    compiler::TNode<Smi> tmp924;
    compiler::TNode<Smi> tmp925;
    compiler::TNode<Smi> tmp926;
    compiler::TNode<Smi> tmp927;
    compiler::TNode<Smi> tmp928;
    compiler::TNode<JSArray> tmp929;
    compiler::TNode<Map> tmp930;
    compiler::TNode<Int32T> tmp931;
    compiler::TNode<Int32T> tmp932;
    compiler::TNode<Smi> tmp933;
    compiler::TNode<JSArray> tmp934;
    compiler::TNode<RawPtrT> tmp935;
    compiler::TNode<RawPtrT> tmp936;
    compiler::TNode<IntPtrT> tmp937;
    compiler::TNode<JSArray> tmp938;
    compiler::TNode<Smi> tmp939;
    compiler::TNode<Smi> tmp940;
    compiler::TNode<Smi> tmp941;
    compiler::TNode<Smi> tmp942;
    compiler::TNode<Smi> tmp943;
    compiler::TNode<Smi> tmp944;
    ca_.Bind(&block52, &tmp915, &tmp916, &tmp917, &tmp918, &tmp919, &tmp920, &tmp921, &tmp922, &tmp923, &tmp924, &tmp925, &tmp926, &tmp927, &tmp928, &tmp929, &tmp930, &tmp931, &tmp932, &tmp933, &tmp934, &tmp935, &tmp936, &tmp937, &tmp938, &tmp939, &tmp940, &tmp941, &tmp942, &tmp943, &tmp944);
    ca_.Goto(&block1);
  }

  if (block51.is_used()) {
    compiler::TNode<Context> tmp945;
    compiler::TNode<RawPtrT> tmp946;
    compiler::TNode<RawPtrT> tmp947;
    compiler::TNode<IntPtrT> tmp948;
    compiler::TNode<JSReceiver> tmp949;
    compiler::TNode<Number> tmp950;
    compiler::TNode<Number> tmp951;
    compiler::TNode<Smi> tmp952;
    compiler::TNode<Number> tmp953;
    compiler::TNode<Smi> tmp954;
    compiler::TNode<Smi> tmp955;
    compiler::TNode<Smi> tmp956;
    compiler::TNode<Smi> tmp957;
    compiler::TNode<Smi> tmp958;
    compiler::TNode<JSArray> tmp959;
    compiler::TNode<Map> tmp960;
    compiler::TNode<Int32T> tmp961;
    compiler::TNode<Int32T> tmp962;
    compiler::TNode<Smi> tmp963;
    compiler::TNode<JSArray> tmp964;
    compiler::TNode<RawPtrT> tmp965;
    compiler::TNode<RawPtrT> tmp966;
    compiler::TNode<IntPtrT> tmp967;
    compiler::TNode<JSArray> tmp968;
    compiler::TNode<Smi> tmp969;
    compiler::TNode<Smi> tmp970;
    compiler::TNode<Smi> tmp971;
    compiler::TNode<Smi> tmp972;
    compiler::TNode<Smi> tmp973;
    compiler::TNode<Smi> tmp974;
    ca_.Bind(&block51, &tmp945, &tmp946, &tmp947, &tmp948, &tmp949, &tmp950, &tmp951, &tmp952, &tmp953, &tmp954, &tmp955, &tmp956, &tmp957, &tmp958, &tmp959, &tmp960, &tmp961, &tmp962, &tmp963, &tmp964, &tmp965, &tmp966, &tmp967, &tmp968, &tmp969, &tmp970, &tmp971, &tmp972, &tmp973, &tmp974);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 169);
    ca_.Goto(&block48, tmp945, tmp946, tmp947, tmp948, tmp949, tmp950, tmp951, tmp952, tmp953, tmp954, tmp955, tmp956, tmp957, tmp958, tmp959, tmp960, tmp961, tmp962, tmp963, tmp964);
  }

  if (block48.is_used()) {
    compiler::TNode<Context> tmp975;
    compiler::TNode<RawPtrT> tmp976;
    compiler::TNode<RawPtrT> tmp977;
    compiler::TNode<IntPtrT> tmp978;
    compiler::TNode<JSReceiver> tmp979;
    compiler::TNode<Number> tmp980;
    compiler::TNode<Number> tmp981;
    compiler::TNode<Smi> tmp982;
    compiler::TNode<Number> tmp983;
    compiler::TNode<Smi> tmp984;
    compiler::TNode<Smi> tmp985;
    compiler::TNode<Smi> tmp986;
    compiler::TNode<Smi> tmp987;
    compiler::TNode<Smi> tmp988;
    compiler::TNode<JSArray> tmp989;
    compiler::TNode<Map> tmp990;
    compiler::TNode<Int32T> tmp991;
    compiler::TNode<Int32T> tmp992;
    compiler::TNode<Smi> tmp993;
    compiler::TNode<JSArray> tmp994;
    ca_.Bind(&block48, &tmp975, &tmp976, &tmp977, &tmp978, &tmp979, &tmp980, &tmp981, &tmp982, &tmp983, &tmp984, &tmp985, &tmp986, &tmp987, &tmp988, &tmp989, &tmp990, &tmp991, &tmp992, &tmp993, &tmp994);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 179);
    ca_.Goto(&block2, tmp975, tmp976, tmp977, tmp978, tmp979, tmp980, tmp981, tmp982, tmp983, tmp994);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp995;
    compiler::TNode<RawPtrT> tmp996;
    compiler::TNode<RawPtrT> tmp997;
    compiler::TNode<IntPtrT> tmp998;
    compiler::TNode<JSReceiver> tmp999;
    compiler::TNode<Number> tmp1000;
    compiler::TNode<Number> tmp1001;
    compiler::TNode<Smi> tmp1002;
    compiler::TNode<Number> tmp1003;
    compiler::TNode<Object> tmp1004;
    ca_.Bind(&block2, &tmp995, &tmp996, &tmp997, &tmp998, &tmp999, &tmp1000, &tmp1001, &tmp1002, &tmp1003, &tmp1004);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 110);
    ca_.Goto(&block53, tmp995, tmp996, tmp997, tmp998, tmp999, tmp1000, tmp1001, tmp1002, tmp1003, tmp1004);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp1005;
    compiler::TNode<RawPtrT> tmp1006;
    compiler::TNode<RawPtrT> tmp1007;
    compiler::TNode<IntPtrT> tmp1008;
    compiler::TNode<JSReceiver> tmp1009;
    compiler::TNode<Number> tmp1010;
    compiler::TNode<Number> tmp1011;
    compiler::TNode<Smi> tmp1012;
    compiler::TNode<Number> tmp1013;
    compiler::TNode<Object> tmp1014;
    ca_.Bind(&block53, &tmp1005, &tmp1006, &tmp1007, &tmp1008, &tmp1009, &tmp1010, &tmp1011, &tmp1012, &tmp1013, &tmp1014);
  return compiler::TNode<Object>{tmp1014};
}

compiler::TNode<Object> ArraySpliceBuiltinsFromDSLAssembler::FillDeletedElementsArray(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_actualStart, compiler::TNode<Number> p_actualDeleteCount, compiler::TNode<JSReceiver> p_a) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver, Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver, Number, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver, Number, Number, Oddball> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver, Number, Number, Oddball> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, JSReceiver, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_actualStart, p_actualDeleteCount, p_a);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Number> tmp3;
    compiler::TNode<JSReceiver> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 186);
    compiler::TNode<Number> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 189);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<JSReceiver> tmp7;
    compiler::TNode<Number> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<Number> tmp11;
    ca_.Bind(&block4, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp11}, compiler::TNode<Number>{tmp9}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp11, tmp9);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block6, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp11, tmp9);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<JSReceiver> tmp13;
    compiler::TNode<Number> tmp14;
    compiler::TNode<Number> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Number> tmp19;
    ca_.Bind(&block5, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block2, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<JSReceiver> tmp21;
    compiler::TNode<Number> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Number> tmp27;
    ca_.Bind(&block6, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<JSReceiver> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Number> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Number> tmp33;
    ca_.Bind(&block2, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 191);
    compiler::TNode<Number> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp30}, compiler::TNode<Number>{tmp33}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 194);
    compiler::TNode<Oddball> tmp35;
    tmp35 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp28, tmp29, tmp34));
    USE(tmp35);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 197);
    compiler::TNode<Oddball> tmp36;
    USE(tmp36);
    tmp36 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp35}, compiler::TNode<HeapObject>{tmp36}));
    ca_.Branch(tmp37, &block7, &block8, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Number> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<Number> tmp43;
    compiler::TNode<Number> tmp44;
    compiler::TNode<Oddball> tmp45;
    ca_.Bind(&block7, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 199);
    compiler::TNode<Object> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp38}, compiler::TNode<Object>{tmp39}, compiler::TNode<Object>{tmp44}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 202);
    compiler::TNode<Object> tmp47;
    tmp47 = CodeStubAssembler(state_).CallBuiltin(Builtins::kFastCreateDataProperty, tmp38, tmp42, tmp43, tmp46);
    USE(tmp47);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 197);
    ca_.Goto(&block8, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<JSReceiver> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<Number> tmp51;
    compiler::TNode<JSReceiver> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<Oddball> tmp55;
    ca_.Bind(&block8, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 206);
    compiler::TNode<Number> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp53}, compiler::TNode<Number>{tmp56}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 189);
    ca_.Goto(&block4, tmp48, tmp49, tmp50, tmp51, tmp52, tmp57);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp58;
    compiler::TNode<JSReceiver> tmp59;
    compiler::TNode<Number> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<JSReceiver> tmp62;
    compiler::TNode<Number> tmp63;
    ca_.Bind(&block3, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 209);
    compiler::TNode<String> tmp64;
    USE(tmp64);
    tmp64 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp58, tmp62, tmp64, tmp61);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 210);
    ca_.Goto(&block1, tmp58, tmp59, tmp60, tmp61, tmp62, tmp62);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<JSReceiver> tmp67;
    compiler::TNode<Number> tmp68;
    compiler::TNode<Number> tmp69;
    compiler::TNode<JSReceiver> tmp70;
    compiler::TNode<Object> tmp71;
    ca_.Bind(&block1, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 182);
    ca_.Goto(&block9, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71);
  }

    compiler::TNode<Context> tmp72;
    compiler::TNode<JSReceiver> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    compiler::TNode<Object> tmp77;
    ca_.Bind(&block9, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
  return compiler::TNode<Object>{tmp77};
}

void ArraySpliceBuiltinsFromDSLAssembler::HandleForwardCase(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Number> p_itemCount, compiler::TNode<Number> p_actualStart, compiler::TNode<Number> p_actualDeleteCount) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Oddball> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Oddball> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Oddball> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_len, p_itemCount, p_actualStart, p_actualDeleteCount);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Number> tmp3;
    compiler::TNode<Number> tmp4;
    compiler::TNode<Number> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 220);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 223);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp4);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp6;
    compiler::TNode<JSReceiver> tmp7;
    compiler::TNode<Number> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<Number> tmp10;
    compiler::TNode<Number> tmp11;
    compiler::TNode<Number> tmp12;
    ca_.Bind(&block4, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    compiler::TNode<Number> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp8}, compiler::TNode<Number>{tmp11}));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp12}, compiler::TNode<Number>{tmp13}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp12, tmp13);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block6, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp12, tmp13);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Number> tmp21;
    compiler::TNode<Number> tmp22;
    ca_.Bind(&block5, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block2, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Number> tmp27;
    compiler::TNode<Number> tmp28;
    compiler::TNode<Number> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Number> tmp31;
    ca_.Bind(&block6, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block3, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<Number> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Number> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Number> tmp38;
    ca_.Bind(&block2, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 225);
    compiler::TNode<Number> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp38}, compiler::TNode<Number>{tmp37}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 227);
    compiler::TNode<Number> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp38}, compiler::TNode<Number>{tmp35}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 230);
    compiler::TNode<Oddball> tmp41;
    tmp41 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp32, tmp33, tmp39));
    USE(tmp41);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 233);
    compiler::TNode<Oddball> tmp42;
    USE(tmp42);
    tmp42 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp41}, compiler::TNode<HeapObject>{tmp42}));
    ca_.Branch(tmp43, &block7, &block8, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Number> tmp46;
    compiler::TNode<Number> tmp47;
    compiler::TNode<Number> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<Number> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<Oddball> tmp53;
    ca_.Bind(&block7, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 235);
    compiler::TNode<Object> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp44}, compiler::TNode<Object>{tmp45}, compiler::TNode<Object>{tmp51}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 238);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp44, tmp45, tmp52, tmp54);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 233);
    ca_.Goto(&block9, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<Number> tmp59;
    compiler::TNode<Number> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Number> tmp64;
    compiler::TNode<Oddball> tmp65;
    ca_.Bind(&block8, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 243);
    compiler::TNode<Smi> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp56, tmp57, tmp64, tmp66);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 233);
    ca_.Goto(&block9, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<JSReceiver> tmp69;
    compiler::TNode<Number> tmp70;
    compiler::TNode<Number> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<Number> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Oddball> tmp77;
    ca_.Bind(&block9, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 246);
    compiler::TNode<Number> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp74}, compiler::TNode<Number>{tmp78}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 223);
    ca_.Goto(&block4, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp79);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp80;
    compiler::TNode<JSReceiver> tmp81;
    compiler::TNode<Number> tmp82;
    compiler::TNode<Number> tmp83;
    compiler::TNode<Number> tmp84;
    compiler::TNode<Number> tmp85;
    compiler::TNode<Number> tmp86;
    ca_.Bind(&block3, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 250);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 253);
    ca_.Goto(&block12, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp82);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp87;
    compiler::TNode<JSReceiver> tmp88;
    compiler::TNode<Number> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Number> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<Number> tmp93;
    ca_.Bind(&block12, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93);
    compiler::TNode<Number> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp89}, compiler::TNode<Number>{tmp92}));
    compiler::TNode<Number> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp94}, compiler::TNode<Number>{tmp90}));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp93}, compiler::TNode<Number>{tmp95}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block13, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp93, tmp95);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block14, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp93, tmp95);
    }
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp96;
    compiler::TNode<JSReceiver> tmp97;
    compiler::TNode<Number> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Number> tmp102;
    compiler::TNode<Number> tmp103;
    compiler::TNode<Number> tmp104;
    ca_.Bind(&block13, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104);
    ca_.Goto(&block10, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp105;
    compiler::TNode<JSReceiver> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<Number> tmp108;
    compiler::TNode<Number> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Number> tmp113;
    ca_.Bind(&block14, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113);
    ca_.Goto(&block11, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp114;
    compiler::TNode<JSReceiver> tmp115;
    compiler::TNode<Number> tmp116;
    compiler::TNode<Number> tmp117;
    compiler::TNode<Number> tmp118;
    compiler::TNode<Number> tmp119;
    compiler::TNode<Number> tmp120;
    ca_.Bind(&block10, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 255);
    compiler::TNode<Number> tmp121;
    USE(tmp121);
    tmp121 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp122;
    USE(tmp122);
    tmp122 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp120}, compiler::TNode<Number>{tmp121}));
    compiler::TNode<Smi> tmp123;
    USE(tmp123);
    tmp123 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp114, tmp115, tmp122, tmp123);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 257);
    compiler::TNode<Number> tmp125;
    USE(tmp125);
    tmp125 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp126;
    USE(tmp126);
    tmp126 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp120}, compiler::TNode<Number>{tmp125}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 253);
    ca_.Goto(&block12, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp126);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp127;
    compiler::TNode<JSReceiver> tmp128;
    compiler::TNode<Number> tmp129;
    compiler::TNode<Number> tmp130;
    compiler::TNode<Number> tmp131;
    compiler::TNode<Number> tmp132;
    compiler::TNode<Number> tmp133;
    ca_.Bind(&block11, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 217);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 215);
    ca_.Goto(&block1, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp134;
    compiler::TNode<JSReceiver> tmp135;
    compiler::TNode<Number> tmp136;
    compiler::TNode<Number> tmp137;
    compiler::TNode<Number> tmp138;
    compiler::TNode<Number> tmp139;
    ca_.Bind(&block1, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139);
    ca_.Goto(&block15, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139);
  }

    compiler::TNode<Context> tmp140;
    compiler::TNode<JSReceiver> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<Number> tmp143;
    compiler::TNode<Number> tmp144;
    compiler::TNode<Number> tmp145;
    ca_.Bind(&block15, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145);
}

void ArraySpliceBuiltinsFromDSLAssembler::HandleBackwardCase(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Number> p_itemCount, compiler::TNode<Number> p_actualStart, compiler::TNode<Number> p_actualDeleteCount) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Oddball> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Oddball> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Oddball> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_len, p_itemCount, p_actualStart, p_actualDeleteCount);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Number> tmp3;
    compiler::TNode<Number> tmp4;
    compiler::TNode<Number> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 268);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp2}, compiler::TNode<Number>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 271);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<Number> tmp10;
    compiler::TNode<Number> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<Number> tmp13;
    ca_.Bind(&block4, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp13}, compiler::TNode<Number>{tmp11}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp13, tmp11);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp13, tmp11);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Number> tmp21;
    compiler::TNode<Number> tmp22;
    ca_.Bind(&block5, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block2, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Number> tmp27;
    compiler::TNode<Number> tmp28;
    compiler::TNode<Number> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Number> tmp31;
    ca_.Bind(&block6, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block3, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<Number> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Number> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Number> tmp38;
    ca_.Bind(&block2, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 273);
    compiler::TNode<Number> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp38}, compiler::TNode<Number>{tmp37}));
    compiler::TNode<Number> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp39}, compiler::TNode<Number>{tmp40}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 276);
    compiler::TNode<Number> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp38}, compiler::TNode<Number>{tmp35}));
    compiler::TNode<Number> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp42}, compiler::TNode<Number>{tmp43}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 279);
    compiler::TNode<Oddball> tmp45;
    tmp45 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp32, tmp33, tmp41));
    USE(tmp45);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 282);
    compiler::TNode<Oddball> tmp46;
    USE(tmp46);
    tmp46 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp45}, compiler::TNode<HeapObject>{tmp46}));
    ca_.Branch(tmp47, &block7, &block8, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp41, tmp44, tmp45);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp48;
    compiler::TNode<JSReceiver> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<Number> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<Number> tmp55;
    compiler::TNode<Number> tmp56;
    compiler::TNode<Oddball> tmp57;
    ca_.Bind(&block7, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 284);
    compiler::TNode<Object> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp48}, compiler::TNode<Object>{tmp49}, compiler::TNode<Object>{tmp55}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 287);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp48, tmp49, tmp56, tmp58);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 282);
    ca_.Goto(&block9, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp60;
    compiler::TNode<JSReceiver> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Number> tmp64;
    compiler::TNode<Number> tmp65;
    compiler::TNode<Number> tmp66;
    compiler::TNode<Number> tmp67;
    compiler::TNode<Number> tmp68;
    compiler::TNode<Oddball> tmp69;
    ca_.Bind(&block8, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 292);
    compiler::TNode<Smi> tmp70;
    USE(tmp70);
    tmp70 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp60, tmp61, tmp68, tmp70);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 282);
    ca_.Goto(&block9, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<JSReceiver> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<Number> tmp79;
    compiler::TNode<Number> tmp80;
    compiler::TNode<Oddball> tmp81;
    ca_.Bind(&block9, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 296);
    compiler::TNode<Number> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp78}, compiler::TNode<Number>{tmp82}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 271);
    ca_.Goto(&block4, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp83);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<JSReceiver> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<Number> tmp88;
    compiler::TNode<Number> tmp89;
    compiler::TNode<Number> tmp90;
    ca_.Bind(&block3, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 265);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 263);
    ca_.Goto(&block1, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp91;
    compiler::TNode<JSReceiver> tmp92;
    compiler::TNode<Number> tmp93;
    compiler::TNode<Number> tmp94;
    compiler::TNode<Number> tmp95;
    compiler::TNode<Number> tmp96;
    ca_.Bind(&block1, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96);
    ca_.Goto(&block10, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96);
  }

    compiler::TNode<Context> tmp97;
    compiler::TNode<JSReceiver> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<Number> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Number> tmp102;
    ca_.Bind(&block10, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102);
}

compiler::TNode<Object> ArraySpliceBuiltinsFromDSLAssembler::SlowSplice(compiler::TNode<Context> p_context, BaseBuiltinsFromDSLAssembler::Arguments p_arguments, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Number> p_actualStart, compiler::TNode<Smi> p_insertCount, compiler::TNode<Number> p_actualDeleteCount) {
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, IntPtrT> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, IntPtrT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, IntPtrT> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, IntPtrT> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_arguments.frame, p_arguments.base, p_arguments.length, p_o, p_len, p_actualStart, p_insertCount, p_actualDeleteCount);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<RawPtrT> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<IntPtrT> tmp3;
    compiler::TNode<JSReceiver> tmp4;
    compiler::TNode<Number> tmp5;
    compiler::TNode<Number> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Number> tmp8;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7, &tmp8);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 304);
    compiler::TNode<Number> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp5}, compiler::TNode<Number>{tmp6}));
    compiler::TNode<Number> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp9}, compiler::TNode<Number>{tmp8}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 307);
    compiler::TNode<JSReceiver> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ArraySpeciesCreate(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp4}, compiler::TNode<Number>{tmp8}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 308);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 311);
    compiler::TNode<Object> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Object>(ArraySpliceBuiltinsFromDSLAssembler(state_).FillDeletedElementsArray(compiler::TNode<Context>{tmp0}, compiler::TNode<JSReceiver>{tmp4}, compiler::TNode<Number>{tmp6}, compiler::TNode<Number>{tmp8}, compiler::TNode<JSReceiver>{tmp11}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 321);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp7}, compiler::TNode<Number>{tmp8}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp10, tmp11, tmp7, tmp7, tmp8);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp10, tmp11, tmp7, tmp7, tmp8);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<RawPtrT> tmp15;
    compiler::TNode<IntPtrT> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<Number> tmp21;
    compiler::TNode<Number> tmp22;
    compiler::TNode<JSReceiver> tmp23;
    compiler::TNode<Number> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Number> tmp26;
    ca_.Bind(&block4, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.Goto(&block2, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<JSReceiver> tmp31;
    compiler::TNode<Number> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<Smi> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Number> tmp36;
    compiler::TNode<JSReceiver> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Number> tmp39;
    compiler::TNode<Number> tmp40;
    ca_.Bind(&block5, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.Goto(&block3, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<RawPtrT> tmp42;
    compiler::TNode<RawPtrT> tmp43;
    compiler::TNode<IntPtrT> tmp44;
    compiler::TNode<JSReceiver> tmp45;
    compiler::TNode<Number> tmp46;
    compiler::TNode<Number> tmp47;
    compiler::TNode<Smi> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<JSReceiver> tmp51;
    compiler::TNode<Number> tmp52;
    ca_.Bind(&block2, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 323);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 322);
    ArraySpliceBuiltinsFromDSLAssembler(state_).HandleForwardCase(compiler::TNode<Context>{tmp41}, compiler::TNode<JSReceiver>{tmp45}, compiler::TNode<Number>{tmp46}, compiler::TNode<Number>{tmp52}, compiler::TNode<Number>{tmp47}, compiler::TNode<Number>{tmp49});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 321);
    ca_.Goto(&block6, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp53;
    compiler::TNode<RawPtrT> tmp54;
    compiler::TNode<RawPtrT> tmp55;
    compiler::TNode<IntPtrT> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<Number> tmp59;
    compiler::TNode<Smi> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<JSReceiver> tmp63;
    compiler::TNode<Number> tmp64;
    ca_.Bind(&block3, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 325);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp64}, compiler::TNode<Number>{tmp61}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp64, tmp61);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block10, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp64, tmp61);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<RawPtrT> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<IntPtrT> tmp68;
    compiler::TNode<JSReceiver> tmp69;
    compiler::TNode<Number> tmp70;
    compiler::TNode<Number> tmp71;
    compiler::TNode<Smi> tmp72;
    compiler::TNode<Number> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<JSReceiver> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Number> tmp78;
    ca_.Bind(&block9, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78);
    ca_.Goto(&block7, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp79;
    compiler::TNode<RawPtrT> tmp80;
    compiler::TNode<RawPtrT> tmp81;
    compiler::TNode<IntPtrT> tmp82;
    compiler::TNode<JSReceiver> tmp83;
    compiler::TNode<Number> tmp84;
    compiler::TNode<Number> tmp85;
    compiler::TNode<Smi> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<Number> tmp88;
    compiler::TNode<JSReceiver> tmp89;
    compiler::TNode<Number> tmp90;
    compiler::TNode<Number> tmp91;
    compiler::TNode<Number> tmp92;
    ca_.Bind(&block10, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.Goto(&block8, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp93;
    compiler::TNode<RawPtrT> tmp94;
    compiler::TNode<RawPtrT> tmp95;
    compiler::TNode<IntPtrT> tmp96;
    compiler::TNode<JSReceiver> tmp97;
    compiler::TNode<Number> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<Smi> tmp100;
    compiler::TNode<Number> tmp101;
    compiler::TNode<Number> tmp102;
    compiler::TNode<JSReceiver> tmp103;
    compiler::TNode<Number> tmp104;
    ca_.Bind(&block7, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 327);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 326);
    ArraySpliceBuiltinsFromDSLAssembler(state_).HandleBackwardCase(compiler::TNode<Context>{tmp93}, compiler::TNode<JSReceiver>{tmp97}, compiler::TNode<Number>{tmp98}, compiler::TNode<Number>{tmp104}, compiler::TNode<Number>{tmp99}, compiler::TNode<Number>{tmp101});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 325);
    ca_.Goto(&block8, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp105;
    compiler::TNode<RawPtrT> tmp106;
    compiler::TNode<RawPtrT> tmp107;
    compiler::TNode<IntPtrT> tmp108;
    compiler::TNode<JSReceiver> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<Number> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<JSReceiver> tmp115;
    compiler::TNode<Number> tmp116;
    ca_.Bind(&block8, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 321);
    ca_.Goto(&block6, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp117;
    compiler::TNode<RawPtrT> tmp118;
    compiler::TNode<RawPtrT> tmp119;
    compiler::TNode<IntPtrT> tmp120;
    compiler::TNode<JSReceiver> tmp121;
    compiler::TNode<Number> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<Smi> tmp124;
    compiler::TNode<Number> tmp125;
    compiler::TNode<Number> tmp126;
    compiler::TNode<JSReceiver> tmp127;
    compiler::TNode<Number> tmp128;
    ca_.Bind(&block6, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 331);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 336);
    compiler::TNode<IntPtrT> tmp129;
    USE(tmp129);
    tmp129 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp130;
    USE(tmp130);
    tmp130 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp120}, compiler::TNode<IntPtrT>{tmp129}));
    ca_.Branch(tmp130, &block11, &block12, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp123);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp131;
    compiler::TNode<RawPtrT> tmp132;
    compiler::TNode<RawPtrT> tmp133;
    compiler::TNode<IntPtrT> tmp134;
    compiler::TNode<JSReceiver> tmp135;
    compiler::TNode<Number> tmp136;
    compiler::TNode<Number> tmp137;
    compiler::TNode<Smi> tmp138;
    compiler::TNode<Number> tmp139;
    compiler::TNode<Number> tmp140;
    compiler::TNode<JSReceiver> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<Number> tmp143;
    ca_.Bind(&block11, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 337);
    compiler::TNode<IntPtrT> tmp144;
    USE(tmp144);
    tmp144 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    ca_.Goto(&block15, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp145;
    compiler::TNode<RawPtrT> tmp146;
    compiler::TNode<RawPtrT> tmp147;
    compiler::TNode<IntPtrT> tmp148;
    compiler::TNode<JSReceiver> tmp149;
    compiler::TNode<Number> tmp150;
    compiler::TNode<Number> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Number> tmp153;
    compiler::TNode<Number> tmp154;
    compiler::TNode<JSReceiver> tmp155;
    compiler::TNode<Number> tmp156;
    compiler::TNode<Number> tmp157;
    compiler::TNode<IntPtrT> tmp158;
    ca_.Bind(&block15, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158);
    compiler::TNode<BoolT> tmp159;
    USE(tmp159);
    tmp159 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp158}, compiler::TNode<IntPtrT>{tmp148}));
    ca_.Branch(tmp159, &block13, &block14, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp160;
    compiler::TNode<RawPtrT> tmp161;
    compiler::TNode<RawPtrT> tmp162;
    compiler::TNode<IntPtrT> tmp163;
    compiler::TNode<JSReceiver> tmp164;
    compiler::TNode<Number> tmp165;
    compiler::TNode<Number> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<Number> tmp168;
    compiler::TNode<Number> tmp169;
    compiler::TNode<JSReceiver> tmp170;
    compiler::TNode<Number> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<IntPtrT> tmp173;
    ca_.Bind(&block13, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 338);
    compiler::TNode<Object> tmp174;
    USE(tmp174);
    tmp174 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp161}, compiler::TNode<RawPtrT>{tmp162}, compiler::TNode<IntPtrT>{tmp163}}, compiler::TNode<IntPtrT>{tmp173}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 340);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp160, tmp164, tmp172, tmp174);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 343);
    compiler::TNode<Number> tmp176;
    USE(tmp176);
    tmp176 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp177;
    USE(tmp177);
    tmp177 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp172}, compiler::TNode<Number>{tmp176}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 337);
    ca_.Goto(&block16, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp177, tmp173);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp178;
    compiler::TNode<RawPtrT> tmp179;
    compiler::TNode<RawPtrT> tmp180;
    compiler::TNode<IntPtrT> tmp181;
    compiler::TNode<JSReceiver> tmp182;
    compiler::TNode<Number> tmp183;
    compiler::TNode<Number> tmp184;
    compiler::TNode<Smi> tmp185;
    compiler::TNode<Number> tmp186;
    compiler::TNode<Number> tmp187;
    compiler::TNode<JSReceiver> tmp188;
    compiler::TNode<Number> tmp189;
    compiler::TNode<Number> tmp190;
    compiler::TNode<IntPtrT> tmp191;
    ca_.Bind(&block16, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191);
    compiler::TNode<IntPtrT> tmp192;
    USE(tmp192);
    tmp192 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp193;
    USE(tmp193);
    tmp193 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp191}, compiler::TNode<IntPtrT>{tmp192}));
    ca_.Goto(&block15, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp193);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp194;
    compiler::TNode<RawPtrT> tmp195;
    compiler::TNode<RawPtrT> tmp196;
    compiler::TNode<IntPtrT> tmp197;
    compiler::TNode<JSReceiver> tmp198;
    compiler::TNode<Number> tmp199;
    compiler::TNode<Number> tmp200;
    compiler::TNode<Smi> tmp201;
    compiler::TNode<Number> tmp202;
    compiler::TNode<Number> tmp203;
    compiler::TNode<JSReceiver> tmp204;
    compiler::TNode<Number> tmp205;
    compiler::TNode<Number> tmp206;
    compiler::TNode<IntPtrT> tmp207;
    ca_.Bind(&block14, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 336);
    ca_.Goto(&block12, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp208;
    compiler::TNode<RawPtrT> tmp209;
    compiler::TNode<RawPtrT> tmp210;
    compiler::TNode<IntPtrT> tmp211;
    compiler::TNode<JSReceiver> tmp212;
    compiler::TNode<Number> tmp213;
    compiler::TNode<Number> tmp214;
    compiler::TNode<Smi> tmp215;
    compiler::TNode<Number> tmp216;
    compiler::TNode<Number> tmp217;
    compiler::TNode<JSReceiver> tmp218;
    compiler::TNode<Number> tmp219;
    compiler::TNode<Number> tmp220;
    ca_.Bind(&block12, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 349);
    compiler::TNode<String> tmp221;
    USE(tmp221);
    tmp221 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    compiler::TNode<Number> tmp222;
    USE(tmp222);
    tmp222 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp213}, compiler::TNode<Number>{tmp216}));
    compiler::TNode<Number> tmp223;
    USE(tmp223);
    tmp223 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp222}, compiler::TNode<Number>{tmp219}));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp208, tmp212, tmp221, tmp223);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 351);
    ca_.Goto(&block1, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp218);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp225;
    compiler::TNode<RawPtrT> tmp226;
    compiler::TNode<RawPtrT> tmp227;
    compiler::TNode<IntPtrT> tmp228;
    compiler::TNode<JSReceiver> tmp229;
    compiler::TNode<Number> tmp230;
    compiler::TNode<Number> tmp231;
    compiler::TNode<Smi> tmp232;
    compiler::TNode<Number> tmp233;
    compiler::TNode<Object> tmp234;
    ca_.Bind(&block1, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 300);
    ca_.Goto(&block17, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234);
  }

    compiler::TNode<Context> tmp235;
    compiler::TNode<RawPtrT> tmp236;
    compiler::TNode<RawPtrT> tmp237;
    compiler::TNode<IntPtrT> tmp238;
    compiler::TNode<JSReceiver> tmp239;
    compiler::TNode<Number> tmp240;
    compiler::TNode<Number> tmp241;
    compiler::TNode<Smi> tmp242;
    compiler::TNode<Number> tmp243;
    compiler::TNode<Object> tmp244;
    ca_.Bind(&block17, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244);
  return compiler::TNode<Object>{tmp244};
}

TF_BUILTIN(ArrayPrototypeSplice, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number, Number, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number, Number, Number> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number, Number> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number, Number, Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number, Number, Context, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Smi, Number, Object> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number, Number> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Object, Number, Number, Smi, Number, Number> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 359);
    compiler::TNode<JSReceiver> tmp5;
    tmp5 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kToObject, tmp0, tmp1));
    USE(tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 362);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 365);
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 366);
    compiler::TNode<Number> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp8}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 371);
    compiler::TNode<Number> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp9}, compiler::TNode<Number>{tmp10}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp8, tmp9, tmp9);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block6, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp8, tmp9, tmp9);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    compiler::TNode<JSReceiver> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Number> tmp20;
    ca_.Bind(&block5, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.Goto(&block1, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<Object> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<RawPtrT> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<JSReceiver> tmp26;
    compiler::TNode<Number> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<Number> tmp29;
    compiler::TNode<Number> tmp30;
    ca_.Bind(&block6, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.Goto(&block2, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<RawPtrT> tmp33;
    compiler::TNode<RawPtrT> tmp34;
    compiler::TNode<IntPtrT> tmp35;
    compiler::TNode<JSReceiver> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<Number> tmp39;
    ca_.Bind(&block1, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 372);
    compiler::TNode<Number> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp37}, compiler::TNode<Number>{tmp39}));
    compiler::TNode<Number> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp40}, compiler::TNode<Number>{tmp41}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 371);
    ca_.Goto(&block4, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp42);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp43;
    compiler::TNode<Object> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<RawPtrT> tmp46;
    compiler::TNode<IntPtrT> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Number> tmp51;
    ca_.Bind(&block2, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 373);
    compiler::TNode<Number> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp51}, compiler::TNode<Number>{tmp49}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 371);
    ca_.Goto(&block3, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<RawPtrT> tmp55;
    compiler::TNode<RawPtrT> tmp56;
    compiler::TNode<IntPtrT> tmp57;
    compiler::TNode<JSReceiver> tmp58;
    compiler::TNode<Number> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Number> tmp62;
    ca_.Bind(&block4, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.Goto(&block3, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<Object> tmp64;
    compiler::TNode<RawPtrT> tmp65;
    compiler::TNode<RawPtrT> tmp66;
    compiler::TNode<IntPtrT> tmp67;
    compiler::TNode<JSReceiver> tmp68;
    compiler::TNode<Number> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Number> tmp71;
    compiler::TNode<Number> tmp72;
    ca_.Bind(&block3, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 375);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 376);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 378);
    compiler::TNode<IntPtrT> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp67}, compiler::TNode<IntPtrT>{tmp73}));
    ca_.Branch(tmp74, &block7, &block8, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, ca_.Uninitialized<Smi>(), ca_.Uninitialized<Number>());
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp75;
    compiler::TNode<Object> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<RawPtrT> tmp78;
    compiler::TNode<IntPtrT> tmp79;
    compiler::TNode<JSReceiver> tmp80;
    compiler::TNode<Number> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Number> tmp83;
    compiler::TNode<Number> tmp84;
    compiler::TNode<Smi> tmp85;
    compiler::TNode<Number> tmp86;
    ca_.Bind(&block7, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 380);
    compiler::TNode<Smi> tmp87;
    USE(tmp87);
    tmp87 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 382);
    compiler::TNode<Number> tmp88;
    USE(tmp88);
    tmp88 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 378);
    ca_.Goto(&block9, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp87, tmp88);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<RawPtrT> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<IntPtrT> tmp93;
    compiler::TNode<JSReceiver> tmp94;
    compiler::TNode<Number> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Number> tmp97;
    compiler::TNode<Number> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<Number> tmp100;
    ca_.Bind(&block8, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 384);
    compiler::TNode<IntPtrT> tmp101;
    USE(tmp101);
    tmp101 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp102;
    USE(tmp102);
    tmp102 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp93}, compiler::TNode<IntPtrT>{tmp101}));
    ca_.Branch(tmp102, &block10, &block11, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<RawPtrT> tmp105;
    compiler::TNode<RawPtrT> tmp106;
    compiler::TNode<IntPtrT> tmp107;
    compiler::TNode<JSReceiver> tmp108;
    compiler::TNode<Number> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Smi> tmp113;
    compiler::TNode<Number> tmp114;
    ca_.Bind(&block10, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 386);
    compiler::TNode<Smi> tmp115;
    USE(tmp115);
    tmp115 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 388);
    compiler::TNode<Number> tmp116;
    USE(tmp116);
    tmp116 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp109}, compiler::TNode<Number>{tmp112}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 384);
    ca_.Goto(&block12, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp115, tmp116);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp117;
    compiler::TNode<Object> tmp118;
    compiler::TNode<RawPtrT> tmp119;
    compiler::TNode<RawPtrT> tmp120;
    compiler::TNode<IntPtrT> tmp121;
    compiler::TNode<JSReceiver> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Number> tmp125;
    compiler::TNode<Number> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Number> tmp128;
    ca_.Bind(&block11, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 392);
    compiler::TNode<Smi> tmp129;
    USE(tmp129);
    tmp129 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp121}));
    compiler::TNode<Smi> tmp130;
    USE(tmp130);
    tmp130 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(2));
    compiler::TNode<Smi> tmp131;
    USE(tmp131);
    tmp131 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp129}, compiler::TNode<Smi>{tmp130}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 394);
    compiler::TNode<IntPtrT> tmp132;
    USE(tmp132);
    tmp132 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp133;
    USE(tmp133);
    tmp133 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp119}, compiler::TNode<RawPtrT>{tmp120}, compiler::TNode<IntPtrT>{tmp121}}, compiler::TNode<IntPtrT>{tmp132}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 395);
    compiler::TNode<Number> tmp134;
    USE(tmp134);
    tmp134 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp117}, compiler::TNode<Object>{tmp133}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 397);
    compiler::TNode<Number> tmp135;
    USE(tmp135);
    tmp135 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp136;
    USE(tmp136);
    tmp136 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp134}, compiler::TNode<Number>{tmp135}));
    compiler::TNode<Number> tmp137;
    USE(tmp137);
    tmp137 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp123}, compiler::TNode<Number>{tmp126}));
    compiler::TNode<Number> tmp138;
    USE(tmp138);
    tmp138 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp136}, compiler::TNode<Number>{tmp137}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 390);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 384);
    ca_.Goto(&block12, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp131, tmp138);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<RawPtrT> tmp141;
    compiler::TNode<RawPtrT> tmp142;
    compiler::TNode<IntPtrT> tmp143;
    compiler::TNode<JSReceiver> tmp144;
    compiler::TNode<Number> tmp145;
    compiler::TNode<Object> tmp146;
    compiler::TNode<Number> tmp147;
    compiler::TNode<Number> tmp148;
    compiler::TNode<Smi> tmp149;
    compiler::TNode<Number> tmp150;
    ca_.Bind(&block12, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 378);
    ca_.Goto(&block9, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp151;
    compiler::TNode<Object> tmp152;
    compiler::TNode<RawPtrT> tmp153;
    compiler::TNode<RawPtrT> tmp154;
    compiler::TNode<IntPtrT> tmp155;
    compiler::TNode<JSReceiver> tmp156;
    compiler::TNode<Number> tmp157;
    compiler::TNode<Object> tmp158;
    compiler::TNode<Number> tmp159;
    compiler::TNode<Number> tmp160;
    compiler::TNode<Smi> tmp161;
    compiler::TNode<Number> tmp162;
    ca_.Bind(&block9, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 402);
    compiler::TNode<Number> tmp163;
    USE(tmp163);
    tmp163 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp157}, compiler::TNode<Number>{tmp161}));
    compiler::TNode<Number> tmp164;
    USE(tmp164);
    tmp164 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp163}, compiler::TNode<Number>{tmp162}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 403);
    compiler::TNode<Number> tmp165;
    USE(tmp165);
    tmp165 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber19ATconstexpr_float64(kMaxSafeInteger));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp164}, compiler::TNode<Number>{tmp165}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block15, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp164, tmp164);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block16, tmp151, tmp152, tmp153, tmp154, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp164, tmp164);
    }
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp166;
    compiler::TNode<Object> tmp167;
    compiler::TNode<RawPtrT> tmp168;
    compiler::TNode<RawPtrT> tmp169;
    compiler::TNode<IntPtrT> tmp170;
    compiler::TNode<JSReceiver> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<Number> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<Number> tmp177;
    compiler::TNode<Number> tmp178;
    compiler::TNode<Number> tmp179;
    ca_.Bind(&block15, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179);
    ca_.Goto(&block13, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp180;
    compiler::TNode<Object> tmp181;
    compiler::TNode<RawPtrT> tmp182;
    compiler::TNode<RawPtrT> tmp183;
    compiler::TNode<IntPtrT> tmp184;
    compiler::TNode<JSReceiver> tmp185;
    compiler::TNode<Number> tmp186;
    compiler::TNode<Object> tmp187;
    compiler::TNode<Number> tmp188;
    compiler::TNode<Number> tmp189;
    compiler::TNode<Smi> tmp190;
    compiler::TNode<Number> tmp191;
    compiler::TNode<Number> tmp192;
    compiler::TNode<Number> tmp193;
    ca_.Bind(&block16, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193);
    ca_.Goto(&block14, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<RawPtrT> tmp196;
    compiler::TNode<RawPtrT> tmp197;
    compiler::TNode<IntPtrT> tmp198;
    compiler::TNode<JSReceiver> tmp199;
    compiler::TNode<Number> tmp200;
    compiler::TNode<Object> tmp201;
    compiler::TNode<Number> tmp202;
    compiler::TNode<Number> tmp203;
    compiler::TNode<Smi> tmp204;
    compiler::TNode<Number> tmp205;
    compiler::TNode<Number> tmp206;
    ca_.Bind(&block13, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 404);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp194}, MessageTemplate::kInvalidArrayLength, compiler::TNode<Object>{tmp201});
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp207;
    compiler::TNode<Object> tmp208;
    compiler::TNode<RawPtrT> tmp209;
    compiler::TNode<RawPtrT> tmp210;
    compiler::TNode<IntPtrT> tmp211;
    compiler::TNode<JSReceiver> tmp212;
    compiler::TNode<Number> tmp213;
    compiler::TNode<Object> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<Number> tmp216;
    compiler::TNode<Smi> tmp217;
    compiler::TNode<Number> tmp218;
    compiler::TNode<Number> tmp219;
    ca_.Bind(&block14, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 409);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 410);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 408);
    compiler::TNode<Object> tmp220;
    USE(tmp220);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp220 = ArraySpliceBuiltinsFromDSLAssembler(state_).FastArraySplice(compiler::TNode<Context>{tmp207}, BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp209}, compiler::TNode<RawPtrT>{tmp210}, compiler::TNode<IntPtrT>{tmp211}}, compiler::TNode<JSReceiver>{tmp212}, compiler::TNode<Number>{tmp213}, compiler::TNode<Number>{tmp216}, compiler::TNode<Smi>{tmp217}, compiler::TNode<Number>{tmp218}, &label0);
    ca_.Goto(&block19, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp207, tmp209, tmp210, tmp211, tmp212, tmp213, tmp216, tmp217, tmp218, tmp220);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block20, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp207, tmp209, tmp210, tmp211, tmp212, tmp213, tmp216, tmp217, tmp218);
    }
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp221;
    compiler::TNode<Object> tmp222;
    compiler::TNode<RawPtrT> tmp223;
    compiler::TNode<RawPtrT> tmp224;
    compiler::TNode<IntPtrT> tmp225;
    compiler::TNode<JSReceiver> tmp226;
    compiler::TNode<Number> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Number> tmp229;
    compiler::TNode<Number> tmp230;
    compiler::TNode<Smi> tmp231;
    compiler::TNode<Number> tmp232;
    compiler::TNode<Number> tmp233;
    compiler::TNode<Context> tmp234;
    compiler::TNode<RawPtrT> tmp235;
    compiler::TNode<RawPtrT> tmp236;
    compiler::TNode<IntPtrT> tmp237;
    compiler::TNode<JSReceiver> tmp238;
    compiler::TNode<Number> tmp239;
    compiler::TNode<Number> tmp240;
    compiler::TNode<Smi> tmp241;
    compiler::TNode<Number> tmp242;
    ca_.Bind(&block20, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242);
    ca_.Goto(&block18, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp243;
    compiler::TNode<Object> tmp244;
    compiler::TNode<RawPtrT> tmp245;
    compiler::TNode<RawPtrT> tmp246;
    compiler::TNode<IntPtrT> tmp247;
    compiler::TNode<JSReceiver> tmp248;
    compiler::TNode<Number> tmp249;
    compiler::TNode<Object> tmp250;
    compiler::TNode<Number> tmp251;
    compiler::TNode<Number> tmp252;
    compiler::TNode<Smi> tmp253;
    compiler::TNode<Number> tmp254;
    compiler::TNode<Number> tmp255;
    compiler::TNode<Context> tmp256;
    compiler::TNode<RawPtrT> tmp257;
    compiler::TNode<RawPtrT> tmp258;
    compiler::TNode<IntPtrT> tmp259;
    compiler::TNode<JSReceiver> tmp260;
    compiler::TNode<Number> tmp261;
    compiler::TNode<Number> tmp262;
    compiler::TNode<Smi> tmp263;
    compiler::TNode<Number> tmp264;
    compiler::TNode<Object> tmp265;
    ca_.Bind(&block19, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265);
    arguments.PopAndReturn(tmp265);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp266;
    compiler::TNode<Object> tmp267;
    compiler::TNode<RawPtrT> tmp268;
    compiler::TNode<RawPtrT> tmp269;
    compiler::TNode<IntPtrT> tmp270;
    compiler::TNode<JSReceiver> tmp271;
    compiler::TNode<Number> tmp272;
    compiler::TNode<Object> tmp273;
    compiler::TNode<Number> tmp274;
    compiler::TNode<Number> tmp275;
    compiler::TNode<Smi> tmp276;
    compiler::TNode<Number> tmp277;
    compiler::TNode<Number> tmp278;
    ca_.Bind(&block18, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 407);
    ca_.Goto(&block17, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp279;
    compiler::TNode<Object> tmp280;
    compiler::TNode<RawPtrT> tmp281;
    compiler::TNode<RawPtrT> tmp282;
    compiler::TNode<IntPtrT> tmp283;
    compiler::TNode<JSReceiver> tmp284;
    compiler::TNode<Number> tmp285;
    compiler::TNode<Object> tmp286;
    compiler::TNode<Number> tmp287;
    compiler::TNode<Number> tmp288;
    compiler::TNode<Smi> tmp289;
    compiler::TNode<Number> tmp290;
    compiler::TNode<Number> tmp291;
    ca_.Bind(&block17, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 417);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 418);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 416);
    compiler::TNode<Object> tmp292;
    USE(tmp292);
    tmp292 = ca_.UncheckedCast<Object>(ArraySpliceBuiltinsFromDSLAssembler(state_).SlowSplice(compiler::TNode<Context>{tmp279}, BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp281}, compiler::TNode<RawPtrT>{tmp282}, compiler::TNode<IntPtrT>{tmp283}}, compiler::TNode<JSReceiver>{tmp284}, compiler::TNode<Number>{tmp285}, compiler::TNode<Number>{tmp288}, compiler::TNode<Smi>{tmp289}, compiler::TNode<Number>{tmp290}));
    arguments.PopAndReturn(tmp292);
  }
}

void ArraySpliceBuiltinsFromDSLAssembler::FastSplice10FixedArray20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, BaseBuiltinsFromDSLAssembler::Arguments p_args, compiler::TNode<JSArray> p_a, compiler::TNode<Smi> p_length, compiler::TNode<Smi> p_newLength, compiler::TNode<Smi> p_lengthDelta, compiler::TNode<Smi> p_actualStart, compiler::TNode<Smi> p_insertCount, compiler::TNode<Smi> p_actualDeleteCount, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi, Smi, FixedArray> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi, Smi, FixedArray> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedArray, IntPtrT> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedArray, IntPtrT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedArray, IntPtrT> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedArray, IntPtrT> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_args.frame, p_args.base, p_args.length, p_a, p_length, p_newLength, p_lengthDelta, p_actualStart, p_insertCount, p_actualDeleteCount);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<RawPtrT> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<IntPtrT> tmp3;
    compiler::TNode<JSArray> tmp4;
    compiler::TNode<Smi> tmp5;
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Smi> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 60);
    ArrayBuiltinsFromDSLAssembler(state_).EnsureWriteableFastElements(compiler::TNode<Context>{tmp0}, compiler::TNode<JSArray>{tmp4});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 62);
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiNotEqual(compiler::TNode<Smi>{tmp9}, compiler::TNode<Smi>{tmp10}));
    ca_.Branch(tmp11, &block3, &block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    compiler::TNode<JSArray> tmp16;
    compiler::TNode<Smi> tmp17;
    compiler::TNode<Smi> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Smi> tmp22;
    ca_.Bind(&block3, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 63);
    compiler::TNode<IntPtrT> tmp23 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp23);
    compiler::TNode<FixedArrayBase>tmp24 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp16, tmp23});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 64);
    compiler::TNode<Smi> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp20}, compiler::TNode<Smi>{tmp21}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 65);
    compiler::TNode<Smi> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp20}, compiler::TNode<Smi>{tmp22}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 66);
    compiler::TNode<Smi> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp17}, compiler::TNode<Smi>{tmp22}));
    compiler::TNode<Smi> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp20}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 67);
    compiler::TNode<BoolT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp21}, compiler::TNode<Smi>{tmp22}));
    ca_.Branch(tmp29, &block5, &block6, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp24, tmp25, tmp26, tmp28);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<RawPtrT> tmp31;
    compiler::TNode<RawPtrT> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    compiler::TNode<JSArray> tmp34;
    compiler::TNode<Smi> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<Smi> tmp38;
    compiler::TNode<Smi> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<FixedArrayBase> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<Smi> tmp43;
    compiler::TNode<Smi> tmp44;
    ca_.Bind(&block5, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 70);
    compiler::TNode<FixedArray> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp30}, compiler::TNode<Object>{tmp41}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 69);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoMoveElements10FixedArray(compiler::TNode<FixedArray>{tmp45}, compiler::TNode<Smi>{tmp42}, compiler::TNode<Smi>{tmp43}, compiler::TNode<Smi>{tmp44});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 72);
    compiler::TNode<FixedArray> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp30}, compiler::TNode<Object>{tmp41}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 71);
    ArraySpliceBuiltinsFromDSLAssembler(state_).StoreHoles10FixedArray(compiler::TNode<FixedArray>{tmp46}, compiler::TNode<Smi>{tmp36}, compiler::TNode<Smi>{tmp35});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 67);
    ca_.Goto(&block7, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<RawPtrT> tmp48;
    compiler::TNode<RawPtrT> tmp49;
    compiler::TNode<IntPtrT> tmp50;
    compiler::TNode<JSArray> tmp51;
    compiler::TNode<Smi> tmp52;
    compiler::TNode<Smi> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<Smi> tmp55;
    compiler::TNode<Smi> tmp56;
    compiler::TNode<Smi> tmp57;
    compiler::TNode<FixedArrayBase> tmp58;
    compiler::TNode<Smi> tmp59;
    compiler::TNode<Smi> tmp60;
    compiler::TNode<Smi> tmp61;
    ca_.Bind(&block6, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 73);
    compiler::TNode<BoolT> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp56}, compiler::TNode<Smi>{tmp57}));
    ca_.Branch(tmp62, &block8, &block9, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<RawPtrT> tmp64;
    compiler::TNode<RawPtrT> tmp65;
    compiler::TNode<IntPtrT> tmp66;
    compiler::TNode<JSArray> tmp67;
    compiler::TNode<Smi> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<Smi> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<FixedArrayBase> tmp74;
    compiler::TNode<Smi> tmp75;
    compiler::TNode<Smi> tmp76;
    compiler::TNode<Smi> tmp77;
    ca_.Bind(&block8, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 75);
    compiler::TNode<IntPtrT> tmp78 = ca_.IntPtrConstant(FixedArrayBase::kLengthOffset);
    USE(tmp78);
    compiler::TNode<Smi>tmp79 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp74, tmp78});
    compiler::TNode<BoolT> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThanOrEqual(compiler::TNode<Smi>{tmp69}, compiler::TNode<Smi>{tmp79}));
    ca_.Branch(tmp80, &block10, &block11, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp81;
    compiler::TNode<RawPtrT> tmp82;
    compiler::TNode<RawPtrT> tmp83;
    compiler::TNode<IntPtrT> tmp84;
    compiler::TNode<JSArray> tmp85;
    compiler::TNode<Smi> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<Smi> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<Smi> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<FixedArrayBase> tmp92;
    compiler::TNode<Smi> tmp93;
    compiler::TNode<Smi> tmp94;
    compiler::TNode<Smi> tmp95;
    ca_.Bind(&block10, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 77);
    compiler::TNode<FixedArray> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp81}, compiler::TNode<Object>{tmp92}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 76);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoMoveElements10FixedArray(compiler::TNode<FixedArray>{tmp96}, compiler::TNode<Smi>{tmp93}, compiler::TNode<Smi>{tmp94}, compiler::TNode<Smi>{tmp95});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 75);
    ca_.Goto(&block12, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<RawPtrT> tmp98;
    compiler::TNode<RawPtrT> tmp99;
    compiler::TNode<IntPtrT> tmp100;
    compiler::TNode<JSArray> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<Smi> tmp103;
    compiler::TNode<Smi> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Smi> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<FixedArrayBase> tmp108;
    compiler::TNode<Smi> tmp109;
    compiler::TNode<Smi> tmp110;
    compiler::TNode<Smi> tmp111;
    ca_.Bind(&block11, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 80);
    compiler::TNode<Smi> tmp112;
    USE(tmp112);
    tmp112 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<Smi>{tmp103}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 82);
    compiler::TNode<Smi> tmp113;
    USE(tmp113);
    tmp113 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<FixedArray> tmp114;
    USE(tmp114);
    tmp114 = ca_.UncheckedCast<FixedArray>(ArraySpliceBuiltinsFromDSLAssembler(state_).Extract10FixedArray(compiler::TNode<Context>{tmp97}, compiler::TNode<FixedArrayBase>{tmp108}, compiler::TNode<Smi>{tmp113}, compiler::TNode<Smi>{tmp105}, compiler::TNode<Smi>{tmp112}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 81);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 83);
    compiler::TNode<IntPtrT> tmp115 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp115);
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp101, tmp115}, tmp114);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 84);
    compiler::TNode<IntPtrT> tmp116 = ca_.IntPtrConstant(FixedArrayBase::kLengthOffset);
    USE(tmp116);
    compiler::TNode<Smi>tmp117 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp108, tmp116});
    compiler::TNode<Smi> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp117}, compiler::TNode<Smi>{tmp118}));
    ca_.Branch(tmp119, &block13, &block14, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp114);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp120;
    compiler::TNode<RawPtrT> tmp121;
    compiler::TNode<RawPtrT> tmp122;
    compiler::TNode<IntPtrT> tmp123;
    compiler::TNode<JSArray> tmp124;
    compiler::TNode<Smi> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<Smi> tmp130;
    compiler::TNode<FixedArrayBase> tmp131;
    compiler::TNode<Smi> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<Smi> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<FixedArray> tmp136;
    ca_.Bind(&block13, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 86);
    compiler::TNode<FixedArray> tmp137;
    USE(tmp137);
    tmp137 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp120}, compiler::TNode<Object>{tmp131}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 87);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 85);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoCopyElements10FixedArray(compiler::TNode<FixedArray>{tmp136}, compiler::TNode<Smi>{tmp132}, compiler::TNode<FixedArray>{tmp137}, compiler::TNode<Smi>{tmp133}, compiler::TNode<Smi>{tmp134});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 84);
    ca_.Goto(&block14, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp138;
    compiler::TNode<RawPtrT> tmp139;
    compiler::TNode<RawPtrT> tmp140;
    compiler::TNode<IntPtrT> tmp141;
    compiler::TNode<JSArray> tmp142;
    compiler::TNode<Smi> tmp143;
    compiler::TNode<Smi> tmp144;
    compiler::TNode<Smi> tmp145;
    compiler::TNode<Smi> tmp146;
    compiler::TNode<Smi> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<FixedArrayBase> tmp149;
    compiler::TNode<Smi> tmp150;
    compiler::TNode<Smi> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<FixedArray> tmp154;
    ca_.Bind(&block14, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 78);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 75);
    ca_.Goto(&block12, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp155;
    compiler::TNode<RawPtrT> tmp156;
    compiler::TNode<RawPtrT> tmp157;
    compiler::TNode<IntPtrT> tmp158;
    compiler::TNode<JSArray> tmp159;
    compiler::TNode<Smi> tmp160;
    compiler::TNode<Smi> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<Smi> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<FixedArrayBase> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<Smi> tmp168;
    compiler::TNode<Smi> tmp169;
    ca_.Bind(&block12, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 73);
    ca_.Goto(&block9, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<RawPtrT> tmp171;
    compiler::TNode<RawPtrT> tmp172;
    compiler::TNode<IntPtrT> tmp173;
    compiler::TNode<JSArray> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<Smi> tmp178;
    compiler::TNode<Smi> tmp179;
    compiler::TNode<Smi> tmp180;
    compiler::TNode<FixedArrayBase> tmp181;
    compiler::TNode<Smi> tmp182;
    compiler::TNode<Smi> tmp183;
    compiler::TNode<Smi> tmp184;
    ca_.Bind(&block9, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 67);
    ca_.Goto(&block7, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp185;
    compiler::TNode<RawPtrT> tmp186;
    compiler::TNode<RawPtrT> tmp187;
    compiler::TNode<IntPtrT> tmp188;
    compiler::TNode<JSArray> tmp189;
    compiler::TNode<Smi> tmp190;
    compiler::TNode<Smi> tmp191;
    compiler::TNode<Smi> tmp192;
    compiler::TNode<Smi> tmp193;
    compiler::TNode<Smi> tmp194;
    compiler::TNode<Smi> tmp195;
    compiler::TNode<FixedArrayBase> tmp196;
    compiler::TNode<Smi> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<Smi> tmp199;
    ca_.Bind(&block7, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 62);
    ca_.Goto(&block4, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp200;
    compiler::TNode<RawPtrT> tmp201;
    compiler::TNode<RawPtrT> tmp202;
    compiler::TNode<IntPtrT> tmp203;
    compiler::TNode<JSArray> tmp204;
    compiler::TNode<Smi> tmp205;
    compiler::TNode<Smi> tmp206;
    compiler::TNode<Smi> tmp207;
    compiler::TNode<Smi> tmp208;
    compiler::TNode<Smi> tmp209;
    compiler::TNode<Smi> tmp210;
    ca_.Bind(&block4, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 94);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 95);
    compiler::TNode<Smi> tmp211;
    USE(tmp211);
    tmp211 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp212;
    USE(tmp212);
    tmp212 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp209}, compiler::TNode<Smi>{tmp211}));
    ca_.Branch(tmp212, &block15, &block16, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp208);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp213;
    compiler::TNode<RawPtrT> tmp214;
    compiler::TNode<RawPtrT> tmp215;
    compiler::TNode<IntPtrT> tmp216;
    compiler::TNode<JSArray> tmp217;
    compiler::TNode<Smi> tmp218;
    compiler::TNode<Smi> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<Smi> tmp221;
    compiler::TNode<Smi> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<Smi> tmp224;
    ca_.Bind(&block15, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 97);
    compiler::TNode<IntPtrT> tmp225 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp225);
    compiler::TNode<FixedArrayBase>tmp226 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp217, tmp225});
    compiler::TNode<FixedArray> tmp227;
    USE(tmp227);
    tmp227 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp213}, compiler::TNode<Object>{tmp226}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 96);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 98);
    compiler::TNode<IntPtrT> tmp228;
    USE(tmp228);
    tmp228 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    ca_.Goto(&block19, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp227, tmp228);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp229;
    compiler::TNode<RawPtrT> tmp230;
    compiler::TNode<RawPtrT> tmp231;
    compiler::TNode<IntPtrT> tmp232;
    compiler::TNode<JSArray> tmp233;
    compiler::TNode<Smi> tmp234;
    compiler::TNode<Smi> tmp235;
    compiler::TNode<Smi> tmp236;
    compiler::TNode<Smi> tmp237;
    compiler::TNode<Smi> tmp238;
    compiler::TNode<Smi> tmp239;
    compiler::TNode<Smi> tmp240;
    compiler::TNode<FixedArray> tmp241;
    compiler::TNode<IntPtrT> tmp242;
    ca_.Bind(&block19, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242);
    compiler::TNode<BoolT> tmp243;
    USE(tmp243);
    tmp243 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp242}, compiler::TNode<IntPtrT>{tmp232}));
    ca_.Branch(tmp243, &block17, &block18, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp244;
    compiler::TNode<RawPtrT> tmp245;
    compiler::TNode<RawPtrT> tmp246;
    compiler::TNode<IntPtrT> tmp247;
    compiler::TNode<JSArray> tmp248;
    compiler::TNode<Smi> tmp249;
    compiler::TNode<Smi> tmp250;
    compiler::TNode<Smi> tmp251;
    compiler::TNode<Smi> tmp252;
    compiler::TNode<Smi> tmp253;
    compiler::TNode<Smi> tmp254;
    compiler::TNode<Smi> tmp255;
    compiler::TNode<FixedArray> tmp256;
    compiler::TNode<IntPtrT> tmp257;
    ca_.Bind(&block17, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 99);
    compiler::TNode<Object> tmp258;
    USE(tmp258);
    tmp258 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp245}, compiler::TNode<RawPtrT>{tmp246}, compiler::TNode<IntPtrT>{tmp247}}, compiler::TNode<IntPtrT>{tmp257}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 102);
    compiler::TNode<Smi> tmp259;
    USE(tmp259);
    tmp259 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp260;
    USE(tmp260);
    tmp260 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp255}, compiler::TNode<Smi>{tmp259}));
    compiler::TNode<Object> tmp261;
    USE(tmp261);
    tmp261 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast20UT5ATSmi10HeapObject(compiler::TNode<Object>{tmp258}));
    BaseBuiltinsFromDSLAssembler(state_).StoreFixedArrayDirect(compiler::TNode<FixedArray>{tmp256}, compiler::TNode<Smi>{tmp255}, compiler::TNode<Object>{tmp261});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 98);
    ca_.Goto(&block20, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254, tmp260, tmp256, tmp257);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp262;
    compiler::TNode<RawPtrT> tmp263;
    compiler::TNode<RawPtrT> tmp264;
    compiler::TNode<IntPtrT> tmp265;
    compiler::TNode<JSArray> tmp266;
    compiler::TNode<Smi> tmp267;
    compiler::TNode<Smi> tmp268;
    compiler::TNode<Smi> tmp269;
    compiler::TNode<Smi> tmp270;
    compiler::TNode<Smi> tmp271;
    compiler::TNode<Smi> tmp272;
    compiler::TNode<Smi> tmp273;
    compiler::TNode<FixedArray> tmp274;
    compiler::TNode<IntPtrT> tmp275;
    ca_.Bind(&block20, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275);
    compiler::TNode<IntPtrT> tmp276;
    USE(tmp276);
    tmp276 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp277;
    USE(tmp277);
    tmp277 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp275}, compiler::TNode<IntPtrT>{tmp276}));
    ca_.Goto(&block19, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272, tmp273, tmp274, tmp277);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp278;
    compiler::TNode<RawPtrT> tmp279;
    compiler::TNode<RawPtrT> tmp280;
    compiler::TNode<IntPtrT> tmp281;
    compiler::TNode<JSArray> tmp282;
    compiler::TNode<Smi> tmp283;
    compiler::TNode<Smi> tmp284;
    compiler::TNode<Smi> tmp285;
    compiler::TNode<Smi> tmp286;
    compiler::TNode<Smi> tmp287;
    compiler::TNode<Smi> tmp288;
    compiler::TNode<Smi> tmp289;
    compiler::TNode<FixedArray> tmp290;
    compiler::TNode<IntPtrT> tmp291;
    ca_.Bind(&block18, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 95);
    ca_.Goto(&block16, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp292;
    compiler::TNode<RawPtrT> tmp293;
    compiler::TNode<RawPtrT> tmp294;
    compiler::TNode<IntPtrT> tmp295;
    compiler::TNode<JSArray> tmp296;
    compiler::TNode<Smi> tmp297;
    compiler::TNode<Smi> tmp298;
    compiler::TNode<Smi> tmp299;
    compiler::TNode<Smi> tmp300;
    compiler::TNode<Smi> tmp301;
    compiler::TNode<Smi> tmp302;
    compiler::TNode<Smi> tmp303;
    ca_.Bind(&block16, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 107);
    compiler::TNode<IntPtrT> tmp304 = ca_.IntPtrConstant(JSArray::kLengthOffset);
    USE(tmp304);
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp296, tmp304}, tmp298);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 58);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 54);
    ca_.Goto(&block2, tmp292, tmp293, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp305;
    compiler::TNode<RawPtrT> tmp306;
    compiler::TNode<RawPtrT> tmp307;
    compiler::TNode<IntPtrT> tmp308;
    compiler::TNode<JSArray> tmp309;
    compiler::TNode<Smi> tmp310;
    compiler::TNode<Smi> tmp311;
    compiler::TNode<Smi> tmp312;
    compiler::TNode<Smi> tmp313;
    compiler::TNode<Smi> tmp314;
    compiler::TNode<Smi> tmp315;
    ca_.Bind(&block2, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315);
    ca_.Goto(&block21, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311, tmp312, tmp313, tmp314, tmp315);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp316;
    compiler::TNode<RawPtrT> tmp317;
    compiler::TNode<RawPtrT> tmp318;
    compiler::TNode<IntPtrT> tmp319;
    compiler::TNode<JSArray> tmp320;
    compiler::TNode<Smi> tmp321;
    compiler::TNode<Smi> tmp322;
    compiler::TNode<Smi> tmp323;
    compiler::TNode<Smi> tmp324;
    compiler::TNode<Smi> tmp325;
    compiler::TNode<Smi> tmp326;
    ca_.Bind(&block21, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326);
}

void ArraySpliceBuiltinsFromDSLAssembler::FastSplice16FixedDoubleArray20UT5ATSmi10HeapNumber(compiler::TNode<Context> p_context, BaseBuiltinsFromDSLAssembler::Arguments p_args, compiler::TNode<JSArray> p_a, compiler::TNode<Smi> p_length, compiler::TNode<Smi> p_newLength, compiler::TNode<Smi> p_lengthDelta, compiler::TNode<Smi> p_actualStart, compiler::TNode<Smi> p_insertCount, compiler::TNode<Smi> p_actualDeleteCount, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi, Smi, FixedDoubleArray> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi, Smi, FixedDoubleArray> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedDoubleArray, IntPtrT> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedDoubleArray, IntPtrT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedDoubleArray, IntPtrT> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedDoubleArray, IntPtrT> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, RawPtrT, RawPtrT, IntPtrT, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_args.frame, p_args.base, p_args.length, p_a, p_length, p_newLength, p_lengthDelta, p_actualStart, p_insertCount, p_actualDeleteCount);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<RawPtrT> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<IntPtrT> tmp3;
    compiler::TNode<JSArray> tmp4;
    compiler::TNode<Smi> tmp5;
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Smi> tmp7;
    compiler::TNode<Smi> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 60);
    ArrayBuiltinsFromDSLAssembler(state_).EnsureWriteableFastElements(compiler::TNode<Context>{tmp0}, compiler::TNode<JSArray>{tmp4});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 62);
    compiler::TNode<BoolT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiNotEqual(compiler::TNode<Smi>{tmp9}, compiler::TNode<Smi>{tmp10}));
    ca_.Branch(tmp11, &block3, &block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<RawPtrT> tmp14;
    compiler::TNode<IntPtrT> tmp15;
    compiler::TNode<JSArray> tmp16;
    compiler::TNode<Smi> tmp17;
    compiler::TNode<Smi> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<Smi> tmp21;
    compiler::TNode<Smi> tmp22;
    ca_.Bind(&block3, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 63);
    compiler::TNode<IntPtrT> tmp23 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp23);
    compiler::TNode<FixedArrayBase>tmp24 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp16, tmp23});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 64);
    compiler::TNode<Smi> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp20}, compiler::TNode<Smi>{tmp21}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 65);
    compiler::TNode<Smi> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp20}, compiler::TNode<Smi>{tmp22}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 66);
    compiler::TNode<Smi> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp17}, compiler::TNode<Smi>{tmp22}));
    compiler::TNode<Smi> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp27}, compiler::TNode<Smi>{tmp20}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 67);
    compiler::TNode<BoolT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp21}, compiler::TNode<Smi>{tmp22}));
    ca_.Branch(tmp29, &block5, &block6, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp24, tmp25, tmp26, tmp28);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<RawPtrT> tmp31;
    compiler::TNode<RawPtrT> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    compiler::TNode<JSArray> tmp34;
    compiler::TNode<Smi> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<Smi> tmp38;
    compiler::TNode<Smi> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<FixedArrayBase> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<Smi> tmp43;
    compiler::TNode<Smi> tmp44;
    ca_.Bind(&block5, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 70);
    compiler::TNode<FixedDoubleArray> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp30}, compiler::TNode<Object>{tmp41}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 69);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoMoveElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray>{tmp45}, compiler::TNode<Smi>{tmp42}, compiler::TNode<Smi>{tmp43}, compiler::TNode<Smi>{tmp44});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 72);
    compiler::TNode<FixedDoubleArray> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp30}, compiler::TNode<Object>{tmp41}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 71);
    ArraySpliceBuiltinsFromDSLAssembler(state_).StoreHoles16FixedDoubleArray(compiler::TNode<FixedDoubleArray>{tmp46}, compiler::TNode<Smi>{tmp36}, compiler::TNode<Smi>{tmp35});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 67);
    ca_.Goto(&block7, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<RawPtrT> tmp48;
    compiler::TNode<RawPtrT> tmp49;
    compiler::TNode<IntPtrT> tmp50;
    compiler::TNode<JSArray> tmp51;
    compiler::TNode<Smi> tmp52;
    compiler::TNode<Smi> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<Smi> tmp55;
    compiler::TNode<Smi> tmp56;
    compiler::TNode<Smi> tmp57;
    compiler::TNode<FixedArrayBase> tmp58;
    compiler::TNode<Smi> tmp59;
    compiler::TNode<Smi> tmp60;
    compiler::TNode<Smi> tmp61;
    ca_.Bind(&block6, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 73);
    compiler::TNode<BoolT> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp56}, compiler::TNode<Smi>{tmp57}));
    ca_.Branch(tmp62, &block8, &block9, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<RawPtrT> tmp64;
    compiler::TNode<RawPtrT> tmp65;
    compiler::TNode<IntPtrT> tmp66;
    compiler::TNode<JSArray> tmp67;
    compiler::TNode<Smi> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<Smi> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<FixedArrayBase> tmp74;
    compiler::TNode<Smi> tmp75;
    compiler::TNode<Smi> tmp76;
    compiler::TNode<Smi> tmp77;
    ca_.Bind(&block8, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 75);
    compiler::TNode<IntPtrT> tmp78 = ca_.IntPtrConstant(FixedArrayBase::kLengthOffset);
    USE(tmp78);
    compiler::TNode<Smi>tmp79 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp74, tmp78});
    compiler::TNode<BoolT> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThanOrEqual(compiler::TNode<Smi>{tmp69}, compiler::TNode<Smi>{tmp79}));
    ca_.Branch(tmp80, &block10, &block11, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp81;
    compiler::TNode<RawPtrT> tmp82;
    compiler::TNode<RawPtrT> tmp83;
    compiler::TNode<IntPtrT> tmp84;
    compiler::TNode<JSArray> tmp85;
    compiler::TNode<Smi> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<Smi> tmp88;
    compiler::TNode<Smi> tmp89;
    compiler::TNode<Smi> tmp90;
    compiler::TNode<Smi> tmp91;
    compiler::TNode<FixedArrayBase> tmp92;
    compiler::TNode<Smi> tmp93;
    compiler::TNode<Smi> tmp94;
    compiler::TNode<Smi> tmp95;
    ca_.Bind(&block10, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 77);
    compiler::TNode<FixedDoubleArray> tmp96;
    USE(tmp96);
    tmp96 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp81}, compiler::TNode<Object>{tmp92}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 76);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoMoveElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray>{tmp96}, compiler::TNode<Smi>{tmp93}, compiler::TNode<Smi>{tmp94}, compiler::TNode<Smi>{tmp95});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 75);
    ca_.Goto(&block12, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp97;
    compiler::TNode<RawPtrT> tmp98;
    compiler::TNode<RawPtrT> tmp99;
    compiler::TNode<IntPtrT> tmp100;
    compiler::TNode<JSArray> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<Smi> tmp103;
    compiler::TNode<Smi> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Smi> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<FixedArrayBase> tmp108;
    compiler::TNode<Smi> tmp109;
    compiler::TNode<Smi> tmp110;
    compiler::TNode<Smi> tmp111;
    ca_.Bind(&block11, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 80);
    compiler::TNode<Smi> tmp112;
    USE(tmp112);
    tmp112 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<Smi>{tmp103}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 82);
    compiler::TNode<Smi> tmp113;
    USE(tmp113);
    tmp113 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<FixedDoubleArray> tmp114;
    USE(tmp114);
    tmp114 = ca_.UncheckedCast<FixedDoubleArray>(ArraySpliceBuiltinsFromDSLAssembler(state_).Extract16FixedDoubleArray(compiler::TNode<Context>{tmp97}, compiler::TNode<FixedArrayBase>{tmp108}, compiler::TNode<Smi>{tmp113}, compiler::TNode<Smi>{tmp105}, compiler::TNode<Smi>{tmp112}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 81);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 83);
    compiler::TNode<IntPtrT> tmp115 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp115);
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp101, tmp115}, tmp114);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 84);
    compiler::TNode<IntPtrT> tmp116 = ca_.IntPtrConstant(FixedArrayBase::kLengthOffset);
    USE(tmp116);
    compiler::TNode<Smi>tmp117 = CodeStubAssembler(state_).LoadReference<Smi>(CodeStubAssembler::Reference{tmp108, tmp116});
    compiler::TNode<Smi> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp119;
    USE(tmp119);
    tmp119 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp117}, compiler::TNode<Smi>{tmp118}));
    ca_.Branch(tmp119, &block13, &block14, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp114);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp120;
    compiler::TNode<RawPtrT> tmp121;
    compiler::TNode<RawPtrT> tmp122;
    compiler::TNode<IntPtrT> tmp123;
    compiler::TNode<JSArray> tmp124;
    compiler::TNode<Smi> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<Smi> tmp130;
    compiler::TNode<FixedArrayBase> tmp131;
    compiler::TNode<Smi> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<Smi> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<FixedDoubleArray> tmp136;
    ca_.Bind(&block13, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 86);
    compiler::TNode<FixedDoubleArray> tmp137;
    USE(tmp137);
    tmp137 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp120}, compiler::TNode<Object>{tmp131}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 87);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 85);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoCopyElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray>{tmp136}, compiler::TNode<Smi>{tmp132}, compiler::TNode<FixedDoubleArray>{tmp137}, compiler::TNode<Smi>{tmp133}, compiler::TNode<Smi>{tmp134});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 84);
    ca_.Goto(&block14, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp138;
    compiler::TNode<RawPtrT> tmp139;
    compiler::TNode<RawPtrT> tmp140;
    compiler::TNode<IntPtrT> tmp141;
    compiler::TNode<JSArray> tmp142;
    compiler::TNode<Smi> tmp143;
    compiler::TNode<Smi> tmp144;
    compiler::TNode<Smi> tmp145;
    compiler::TNode<Smi> tmp146;
    compiler::TNode<Smi> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<FixedArrayBase> tmp149;
    compiler::TNode<Smi> tmp150;
    compiler::TNode<Smi> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<FixedDoubleArray> tmp154;
    ca_.Bind(&block14, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 78);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 75);
    ca_.Goto(&block12, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp155;
    compiler::TNode<RawPtrT> tmp156;
    compiler::TNode<RawPtrT> tmp157;
    compiler::TNode<IntPtrT> tmp158;
    compiler::TNode<JSArray> tmp159;
    compiler::TNode<Smi> tmp160;
    compiler::TNode<Smi> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<Smi> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<FixedArrayBase> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<Smi> tmp168;
    compiler::TNode<Smi> tmp169;
    ca_.Bind(&block12, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 73);
    ca_.Goto(&block9, tmp155, tmp156, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<RawPtrT> tmp171;
    compiler::TNode<RawPtrT> tmp172;
    compiler::TNode<IntPtrT> tmp173;
    compiler::TNode<JSArray> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<Smi> tmp178;
    compiler::TNode<Smi> tmp179;
    compiler::TNode<Smi> tmp180;
    compiler::TNode<FixedArrayBase> tmp181;
    compiler::TNode<Smi> tmp182;
    compiler::TNode<Smi> tmp183;
    compiler::TNode<Smi> tmp184;
    ca_.Bind(&block9, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 67);
    ca_.Goto(&block7, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp185;
    compiler::TNode<RawPtrT> tmp186;
    compiler::TNode<RawPtrT> tmp187;
    compiler::TNode<IntPtrT> tmp188;
    compiler::TNode<JSArray> tmp189;
    compiler::TNode<Smi> tmp190;
    compiler::TNode<Smi> tmp191;
    compiler::TNode<Smi> tmp192;
    compiler::TNode<Smi> tmp193;
    compiler::TNode<Smi> tmp194;
    compiler::TNode<Smi> tmp195;
    compiler::TNode<FixedArrayBase> tmp196;
    compiler::TNode<Smi> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<Smi> tmp199;
    ca_.Bind(&block7, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 62);
    ca_.Goto(&block4, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp200;
    compiler::TNode<RawPtrT> tmp201;
    compiler::TNode<RawPtrT> tmp202;
    compiler::TNode<IntPtrT> tmp203;
    compiler::TNode<JSArray> tmp204;
    compiler::TNode<Smi> tmp205;
    compiler::TNode<Smi> tmp206;
    compiler::TNode<Smi> tmp207;
    compiler::TNode<Smi> tmp208;
    compiler::TNode<Smi> tmp209;
    compiler::TNode<Smi> tmp210;
    ca_.Bind(&block4, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 94);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 95);
    compiler::TNode<Smi> tmp211;
    USE(tmp211);
    tmp211 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp212;
    USE(tmp212);
    tmp212 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp209}, compiler::TNode<Smi>{tmp211}));
    ca_.Branch(tmp212, &block15, &block16, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp208);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp213;
    compiler::TNode<RawPtrT> tmp214;
    compiler::TNode<RawPtrT> tmp215;
    compiler::TNode<IntPtrT> tmp216;
    compiler::TNode<JSArray> tmp217;
    compiler::TNode<Smi> tmp218;
    compiler::TNode<Smi> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<Smi> tmp221;
    compiler::TNode<Smi> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<Smi> tmp224;
    ca_.Bind(&block15, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 97);
    compiler::TNode<IntPtrT> tmp225 = ca_.IntPtrConstant(JSObject::kElementsOffset);
    USE(tmp225);
    compiler::TNode<FixedArrayBase>tmp226 = CodeStubAssembler(state_).LoadReference<FixedArrayBase>(CodeStubAssembler::Reference{tmp217, tmp225});
    compiler::TNode<FixedDoubleArray> tmp227;
    USE(tmp227);
    tmp227 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp213}, compiler::TNode<Object>{tmp226}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 96);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 98);
    compiler::TNode<IntPtrT> tmp228;
    USE(tmp228);
    tmp228 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    ca_.Goto(&block19, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp227, tmp228);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp229;
    compiler::TNode<RawPtrT> tmp230;
    compiler::TNode<RawPtrT> tmp231;
    compiler::TNode<IntPtrT> tmp232;
    compiler::TNode<JSArray> tmp233;
    compiler::TNode<Smi> tmp234;
    compiler::TNode<Smi> tmp235;
    compiler::TNode<Smi> tmp236;
    compiler::TNode<Smi> tmp237;
    compiler::TNode<Smi> tmp238;
    compiler::TNode<Smi> tmp239;
    compiler::TNode<Smi> tmp240;
    compiler::TNode<FixedDoubleArray> tmp241;
    compiler::TNode<IntPtrT> tmp242;
    ca_.Bind(&block19, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242);
    compiler::TNode<BoolT> tmp243;
    USE(tmp243);
    tmp243 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp242}, compiler::TNode<IntPtrT>{tmp232}));
    ca_.Branch(tmp243, &block17, &block18, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp244;
    compiler::TNode<RawPtrT> tmp245;
    compiler::TNode<RawPtrT> tmp246;
    compiler::TNode<IntPtrT> tmp247;
    compiler::TNode<JSArray> tmp248;
    compiler::TNode<Smi> tmp249;
    compiler::TNode<Smi> tmp250;
    compiler::TNode<Smi> tmp251;
    compiler::TNode<Smi> tmp252;
    compiler::TNode<Smi> tmp253;
    compiler::TNode<Smi> tmp254;
    compiler::TNode<Smi> tmp255;
    compiler::TNode<FixedDoubleArray> tmp256;
    compiler::TNode<IntPtrT> tmp257;
    ca_.Bind(&block17, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 99);
    compiler::TNode<Object> tmp258;
    USE(tmp258);
    tmp258 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp245}, compiler::TNode<RawPtrT>{tmp246}, compiler::TNode<IntPtrT>{tmp247}}, compiler::TNode<IntPtrT>{tmp257}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 102);
    compiler::TNode<Smi> tmp259;
    USE(tmp259);
    tmp259 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp260;
    USE(tmp260);
    tmp260 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp255}, compiler::TNode<Smi>{tmp259}));
    compiler::TNode<Number> tmp261;
    USE(tmp261);
    tmp261 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast20UT5ATSmi10HeapNumber(compiler::TNode<Context>{tmp244}, compiler::TNode<Object>{tmp258}));
    BaseBuiltinsFromDSLAssembler(state_).StoreFixedDoubleArrayDirect(compiler::TNode<FixedDoubleArray>{tmp256}, compiler::TNode<Smi>{tmp255}, compiler::TNode<Number>{tmp261});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 98);
    ca_.Goto(&block20, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254, tmp260, tmp256, tmp257);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp262;
    compiler::TNode<RawPtrT> tmp263;
    compiler::TNode<RawPtrT> tmp264;
    compiler::TNode<IntPtrT> tmp265;
    compiler::TNode<JSArray> tmp266;
    compiler::TNode<Smi> tmp267;
    compiler::TNode<Smi> tmp268;
    compiler::TNode<Smi> tmp269;
    compiler::TNode<Smi> tmp270;
    compiler::TNode<Smi> tmp271;
    compiler::TNode<Smi> tmp272;
    compiler::TNode<Smi> tmp273;
    compiler::TNode<FixedDoubleArray> tmp274;
    compiler::TNode<IntPtrT> tmp275;
    ca_.Bind(&block20, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275);
    compiler::TNode<IntPtrT> tmp276;
    USE(tmp276);
    tmp276 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp277;
    USE(tmp277);
    tmp277 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp275}, compiler::TNode<IntPtrT>{tmp276}));
    ca_.Goto(&block19, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272, tmp273, tmp274, tmp277);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp278;
    compiler::TNode<RawPtrT> tmp279;
    compiler::TNode<RawPtrT> tmp280;
    compiler::TNode<IntPtrT> tmp281;
    compiler::TNode<JSArray> tmp282;
    compiler::TNode<Smi> tmp283;
    compiler::TNode<Smi> tmp284;
    compiler::TNode<Smi> tmp285;
    compiler::TNode<Smi> tmp286;
    compiler::TNode<Smi> tmp287;
    compiler::TNode<Smi> tmp288;
    compiler::TNode<Smi> tmp289;
    compiler::TNode<FixedDoubleArray> tmp290;
    compiler::TNode<IntPtrT> tmp291;
    ca_.Bind(&block18, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 95);
    ca_.Goto(&block16, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp292;
    compiler::TNode<RawPtrT> tmp293;
    compiler::TNode<RawPtrT> tmp294;
    compiler::TNode<IntPtrT> tmp295;
    compiler::TNode<JSArray> tmp296;
    compiler::TNode<Smi> tmp297;
    compiler::TNode<Smi> tmp298;
    compiler::TNode<Smi> tmp299;
    compiler::TNode<Smi> tmp300;
    compiler::TNode<Smi> tmp301;
    compiler::TNode<Smi> tmp302;
    compiler::TNode<Smi> tmp303;
    ca_.Bind(&block16, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 107);
    compiler::TNode<IntPtrT> tmp304 = ca_.IntPtrConstant(JSArray::kLengthOffset);
    USE(tmp304);
    CodeStubAssembler(state_).StoreReference(CodeStubAssembler::Reference{tmp296, tmp304}, tmp298);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 58);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 54);
    ca_.Goto(&block2, tmp292, tmp293, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp305;
    compiler::TNode<RawPtrT> tmp306;
    compiler::TNode<RawPtrT> tmp307;
    compiler::TNode<IntPtrT> tmp308;
    compiler::TNode<JSArray> tmp309;
    compiler::TNode<Smi> tmp310;
    compiler::TNode<Smi> tmp311;
    compiler::TNode<Smi> tmp312;
    compiler::TNode<Smi> tmp313;
    compiler::TNode<Smi> tmp314;
    compiler::TNode<Smi> tmp315;
    ca_.Bind(&block2, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315);
    ca_.Goto(&block21, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311, tmp312, tmp313, tmp314, tmp315);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp316;
    compiler::TNode<RawPtrT> tmp317;
    compiler::TNode<RawPtrT> tmp318;
    compiler::TNode<IntPtrT> tmp319;
    compiler::TNode<JSArray> tmp320;
    compiler::TNode<Smi> tmp321;
    compiler::TNode<Smi> tmp322;
    compiler::TNode<Smi> tmp323;
    compiler::TNode<Smi> tmp324;
    compiler::TNode<Smi> tmp325;
    compiler::TNode<Smi> tmp326;
    ca_.Bind(&block21, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326);
}

void ArraySpliceBuiltinsFromDSLAssembler::DoMoveElements10FixedArray(compiler::TNode<FixedArray> p_elements, compiler::TNode<Smi> p_dstIndex, compiler::TNode<Smi> p_srcIndex, compiler::TNode<Smi> p_count) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_elements, p_dstIndex, p_srcIndex, p_count);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<Smi> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 35);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp1}));
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 36);
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp3}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 34);
    BaseBuiltinsFromDSLAssembler(state_).TorqueMoveElements(compiler::TNode<FixedArray>{tmp0}, compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}, compiler::TNode<IntPtrT>{tmp6});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 31);
    ca_.Goto(&block1, tmp0, tmp1, tmp2, tmp3);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp7;
    compiler::TNode<Smi> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.Goto(&block2, tmp7, tmp8, tmp9, tmp10);
  }

    compiler::TNode<FixedArray> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    ca_.Bind(&block2, &tmp11, &tmp12, &tmp13, &tmp14);
}

void ArraySpliceBuiltinsFromDSLAssembler::StoreHoles10FixedArray(compiler::TNode<FixedArray> p_elements, compiler::TNode<Smi> p_holeStartIndex, compiler::TNode<Smi> p_holeEndIndex) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_elements, p_holeStartIndex, p_holeEndIndex);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<Smi> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 41);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp1);
  }

  if (block4.is_used()) {
    compiler::TNode<FixedArray> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Smi> tmp5;
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block4, &tmp3, &tmp4, &tmp5, &tmp6);
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp6}, compiler::TNode<Smi>{tmp5}));
    ca_.Branch(tmp7, &block2, &block3, tmp3, tmp4, tmp5, tmp6);
  }

  if (block2.is_used()) {
    compiler::TNode<FixedArray> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 42);
    ArrayBuiltinsFromDSLAssembler(state_).StoreArrayHole(compiler::TNode<FixedArray>{tmp8}, compiler::TNode<Smi>{tmp11});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 41);
    ca_.Goto(&block5, tmp8, tmp9, tmp10, tmp11);
  }

  if (block5.is_used()) {
    compiler::TNode<FixedArray> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Smi> tmp15;
    ca_.Bind(&block5, &tmp12, &tmp13, &tmp14, &tmp15);
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp15}, compiler::TNode<Smi>{tmp16}));
    ca_.Goto(&block4, tmp12, tmp13, tmp14, tmp17);
  }

  if (block3.is_used()) {
    compiler::TNode<FixedArray> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<Smi> tmp21;
    ca_.Bind(&block3, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 39);
    ca_.Goto(&block1, tmp18, tmp19, tmp20);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp22;
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Smi> tmp24;
    ca_.Bind(&block1, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block6, tmp22, tmp23, tmp24);
  }

    compiler::TNode<FixedArray> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<Smi> tmp27;
    ca_.Bind(&block6, &tmp25, &tmp26, &tmp27);
}

void ArraySpliceBuiltinsFromDSLAssembler::DoCopyElements10FixedArray(compiler::TNode<FixedArray> p_dstElements, compiler::TNode<Smi> p_dstIndex, compiler::TNode<FixedArray> p_srcElements, compiler::TNode<Smi> p_srcIndex, compiler::TNode<Smi> p_count) {
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, FixedArray, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, FixedArray, Smi, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedArray, Smi, FixedArray, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_dstElements, p_dstIndex, p_srcElements, p_srcIndex, p_count);

  if (block0.is_used()) {
    compiler::TNode<FixedArray> tmp0;
    compiler::TNode<Smi> tmp1;
    compiler::TNode<FixedArray> tmp2;
    compiler::TNode<Smi> tmp3;
    compiler::TNode<Smi> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 50);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 51);
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp3}));
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp4}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 49);
    BaseBuiltinsFromDSLAssembler(state_).TorqueCopyElements(compiler::TNode<FixedArray>{tmp0}, compiler::TNode<IntPtrT>{tmp5}, compiler::TNode<FixedArray>{tmp2}, compiler::TNode<IntPtrT>{tmp6}, compiler::TNode<IntPtrT>{tmp7});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 46);
    ca_.Goto(&block1, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedArray> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<FixedArray> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<Smi> tmp12;
    ca_.Bind(&block1, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.Goto(&block2, tmp8, tmp9, tmp10, tmp11, tmp12);
  }

    compiler::TNode<FixedArray> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<FixedArray> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Smi> tmp17;
    ca_.Bind(&block2, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
}

void ArraySpliceBuiltinsFromDSLAssembler::DoMoveElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray> p_elements, compiler::TNode<Smi> p_dstIndex, compiler::TNode<Smi> p_srcIndex, compiler::TNode<Smi> p_count) {
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_elements, p_dstIndex, p_srcIndex, p_count);

  if (block0.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp0;
    compiler::TNode<Smi> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 35);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp1}));
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 36);
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp3}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 34);
    BaseBuiltinsFromDSLAssembler(state_).TorqueMoveElements(compiler::TNode<FixedDoubleArray>{tmp0}, compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}, compiler::TNode<IntPtrT>{tmp6});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 31);
    ca_.Goto(&block1, tmp0, tmp1, tmp2, tmp3);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp7;
    compiler::TNode<Smi> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10);
    ca_.Goto(&block2, tmp7, tmp8, tmp9, tmp10);
  }

    compiler::TNode<FixedDoubleArray> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    ca_.Bind(&block2, &tmp11, &tmp12, &tmp13, &tmp14);
}

void ArraySpliceBuiltinsFromDSLAssembler::StoreHoles16FixedDoubleArray(compiler::TNode<FixedDoubleArray> p_elements, compiler::TNode<Smi> p_holeStartIndex, compiler::TNode<Smi> p_holeEndIndex) {
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_elements, p_holeStartIndex, p_holeEndIndex);

  if (block0.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp0;
    compiler::TNode<Smi> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 41);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp1);
  }

  if (block4.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Smi> tmp5;
    compiler::TNode<Smi> tmp6;
    ca_.Bind(&block4, &tmp3, &tmp4, &tmp5, &tmp6);
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp6}, compiler::TNode<Smi>{tmp5}));
    ca_.Branch(tmp7, &block2, &block3, tmp3, tmp4, tmp5, tmp6);
  }

  if (block2.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<Smi> tmp10;
    compiler::TNode<Smi> tmp11;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 42);
    ArrayBuiltinsFromDSLAssembler(state_).StoreArrayHole(compiler::TNode<FixedDoubleArray>{tmp8}, compiler::TNode<Smi>{tmp11});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 41);
    ca_.Goto(&block5, tmp8, tmp9, tmp10, tmp11);
  }

  if (block5.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Smi> tmp15;
    ca_.Bind(&block5, &tmp12, &tmp13, &tmp14, &tmp15);
    compiler::TNode<Smi> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp15}, compiler::TNode<Smi>{tmp16}));
    ca_.Goto(&block4, tmp12, tmp13, tmp14, tmp17);
  }

  if (block3.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp18;
    compiler::TNode<Smi> tmp19;
    compiler::TNode<Smi> tmp20;
    compiler::TNode<Smi> tmp21;
    ca_.Bind(&block3, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 39);
    ca_.Goto(&block1, tmp18, tmp19, tmp20);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp22;
    compiler::TNode<Smi> tmp23;
    compiler::TNode<Smi> tmp24;
    ca_.Bind(&block1, &tmp22, &tmp23, &tmp24);
    ca_.Goto(&block6, tmp22, tmp23, tmp24);
  }

    compiler::TNode<FixedDoubleArray> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<Smi> tmp27;
    ca_.Bind(&block6, &tmp25, &tmp26, &tmp27);
}

void ArraySpliceBuiltinsFromDSLAssembler::DoCopyElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray> p_dstElements, compiler::TNode<Smi> p_dstIndex, compiler::TNode<FixedDoubleArray> p_srcElements, compiler::TNode<Smi> p_srcIndex, compiler::TNode<Smi> p_count) {
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, FixedDoubleArray, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, FixedDoubleArray, Smi, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<FixedDoubleArray, Smi, FixedDoubleArray, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_dstElements, p_dstIndex, p_srcElements, p_srcIndex, p_count);

  if (block0.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp0;
    compiler::TNode<Smi> tmp1;
    compiler::TNode<FixedDoubleArray> tmp2;
    compiler::TNode<Smi> tmp3;
    compiler::TNode<Smi> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 50);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 51);
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp3}));
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp4}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 49);
    BaseBuiltinsFromDSLAssembler(state_).TorqueCopyElements(compiler::TNode<FixedDoubleArray>{tmp0}, compiler::TNode<IntPtrT>{tmp5}, compiler::TNode<FixedDoubleArray>{tmp2}, compiler::TNode<IntPtrT>{tmp6}, compiler::TNode<IntPtrT>{tmp7});
    ca_.SetSourcePosition("../../v8/src/builtins/array-splice.tq", 46);
    ca_.Goto(&block1, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<FixedDoubleArray> tmp8;
    compiler::TNode<Smi> tmp9;
    compiler::TNode<FixedDoubleArray> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<Smi> tmp12;
    ca_.Bind(&block1, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.Goto(&block2, tmp8, tmp9, tmp10, tmp11, tmp12);
  }

    compiler::TNode<FixedDoubleArray> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<FixedDoubleArray> tmp15;
    compiler::TNode<Smi> tmp16;
    compiler::TNode<Smi> tmp17;
    ca_.Bind(&block2, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
}

}  // namespace internal
}  // namespace v8

