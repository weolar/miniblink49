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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 18);
    compiler::TNode<FixedArrayBase> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<FixedArrayBase>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArrayBase>{tmp1}, compiler::TNode<Smi>{tmp2}, compiler::TNode<Smi>{tmp3}, compiler::TNode<Smi>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 17);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 10);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 24);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 25);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 28);
    compiler::TNode<FixedArrayBase> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<FixedArrayBase>(CodeStubAssembler(state_).ExtractFixedArray(compiler::TNode<FixedArrayBase>{tmp15}, compiler::TNode<Smi>{tmp16}, compiler::TNode<Smi>{tmp17}, compiler::TNode<Smi>{tmp18}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 27);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 10);
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

compiler::TNode<Object> ArraySpliceBuiltinsFromDSLAssembler::FastArraySplice(compiler::TNode<Context> p_context, CodeStubArguments* p_args, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_originalLengthNumber, compiler::TNode<Number> p_actualStartNumber, compiler::TNode<Smi> p_insertCount, compiler::TNode<Number> p_actualDeleteCountNumber, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Number, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Number, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSReceiver> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSReceiver, JSArray> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Map> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Map, Int32T> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object, HeapObject> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object, HeapObject> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object, HeapObject, Int32T> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object, HeapObject, Int32T> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT, Object> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, IntPtrT, IntPtrT> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T> block39(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Number> block41(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Number, Smi> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi> block42(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi> block43(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block44(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block45(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block46(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block50(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block49(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block47(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block52(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block51(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Smi, Smi, Smi, Smi, Smi, JSArray, Map, Int32T, Int32T, Smi, JSArray> block48(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Object> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Object> block53(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_originalLengthNumber, p_actualStartNumber, p_insertCount, p_actualDeleteCountNumber);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Number> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Number> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 115);
    compiler::TNode<Smi> tmp6;
    USE(tmp6);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp2}, &label0);
    ca_.Goto(&block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp2, tmp6);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp2);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<JSReceiver> tmp8;
    compiler::TNode<Number> tmp9;
    compiler::TNode<Number> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<Number> tmp13;
    ca_.Bind(&block4, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.Goto(&block1);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<JSReceiver> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<Smi> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Smi> tmp21;
    ca_.Bind(&block3, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 114);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 116);
    compiler::TNode<Smi> tmp22;
    USE(tmp22);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp22 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp17}, &label0);
    ca_.Goto(&block5, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp21, tmp17, tmp22);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp21, tmp17);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<Number> tmp28;
    compiler::TNode<Smi> tmp29;
    compiler::TNode<Number> tmp30;
    ca_.Bind(&block6, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.Goto(&block1);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<JSReceiver> tmp32;
    compiler::TNode<Number> tmp33;
    compiler::TNode<Number> tmp34;
    compiler::TNode<Smi> tmp35;
    compiler::TNode<Number> tmp36;
    compiler::TNode<Smi> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<Smi> tmp39;
    ca_.Bind(&block5, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 118);
    compiler::TNode<Smi> tmp40;
    USE(tmp40);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp40 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp36}, &label0);
    ca_.Goto(&block7, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp39, tmp36, tmp40);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp39, tmp36);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<Number> tmp43;
    compiler::TNode<Number> tmp44;
    compiler::TNode<Smi> tmp45;
    compiler::TNode<Number> tmp46;
    compiler::TNode<Smi> tmp47;
    compiler::TNode<Smi> tmp48;
    compiler::TNode<Number> tmp49;
    ca_.Bind(&block8, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.Goto(&block1);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<JSReceiver> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<Number> tmp55;
    compiler::TNode<Smi> tmp56;
    compiler::TNode<Smi> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<Smi> tmp59;
    ca_.Bind(&block7, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 117);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 119);
    compiler::TNode<Smi> tmp60;
    USE(tmp60);
    tmp60 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp54}, compiler::TNode<Smi>{tmp59}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 120);
    compiler::TNode<Smi> tmp61;
    USE(tmp61);
    tmp61 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp56}, compiler::TNode<Smi>{tmp60}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 122);
    compiler::TNode<JSArray> tmp62;
    USE(tmp62);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp62 = BaseBuiltinsFromDSLAssembler(state_).Cast7JSArray(compiler::TNode<HeapObject>{tmp51}, &label0);
    ca_.Goto(&block9, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp59, tmp60, tmp61, tmp51, tmp62);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp59, tmp60, tmp61, tmp51);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp63;
    compiler::TNode<JSReceiver> tmp64;
    compiler::TNode<Number> tmp65;
    compiler::TNode<Number> tmp66;
    compiler::TNode<Smi> tmp67;
    compiler::TNode<Number> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<Smi> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<JSReceiver> tmp74;
    ca_.Bind(&block10, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    ca_.Goto(&block1);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp75;
    compiler::TNode<JSReceiver> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<Number> tmp80;
    compiler::TNode<Smi> tmp81;
    compiler::TNode<Smi> tmp82;
    compiler::TNode<Smi> tmp83;
    compiler::TNode<Smi> tmp84;
    compiler::TNode<Smi> tmp85;
    compiler::TNode<JSReceiver> tmp86;
    compiler::TNode<JSArray> tmp87;
    ca_.Bind(&block9, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 124);
    compiler::TNode<Map> tmp88;
    USE(tmp88);
    tmp88 = ca_.UncheckedCast<Map>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapObjectMap(compiler::TNode<HeapObject>{tmp87}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 125);
    compiler::TNode<BoolT> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsPrototypeInitialArrayPrototype(compiler::TNode<Context>{tmp75}, compiler::TNode<Map>{tmp88}));
    compiler::TNode<BoolT> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp89}));
    ca_.Branch(tmp90, &block11, &block12, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp87, tmp88);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp91;
    compiler::TNode<JSReceiver> tmp92;
    compiler::TNode<Number> tmp93;
    compiler::TNode<Number> tmp94;
    compiler::TNode<Smi> tmp95;
    compiler::TNode<Number> tmp96;
    compiler::TNode<Smi> tmp97;
    compiler::TNode<Smi> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<Smi> tmp100;
    compiler::TNode<Smi> tmp101;
    compiler::TNode<JSArray> tmp102;
    compiler::TNode<Map> tmp103;
    ca_.Bind(&block11, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103);
    ca_.Goto(&block1);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp104;
    compiler::TNode<JSReceiver> tmp105;
    compiler::TNode<Number> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<Smi> tmp108;
    compiler::TNode<Number> tmp109;
    compiler::TNode<Smi> tmp110;
    compiler::TNode<Smi> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<Smi> tmp113;
    compiler::TNode<Smi> tmp114;
    compiler::TNode<JSArray> tmp115;
    compiler::TNode<Map> tmp116;
    ca_.Bind(&block12, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 126);
    compiler::TNode<BoolT> tmp117;
    USE(tmp117);
    tmp117 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNoElementsProtectorCellInvalid());
    ca_.Branch(tmp117, &block13, &block14, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp118;
    compiler::TNode<JSReceiver> tmp119;
    compiler::TNode<Number> tmp120;
    compiler::TNode<Number> tmp121;
    compiler::TNode<Smi> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<Smi> tmp124;
    compiler::TNode<Smi> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<JSArray> tmp129;
    compiler::TNode<Map> tmp130;
    ca_.Bind(&block13, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130);
    ca_.Goto(&block1);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp131;
    compiler::TNode<JSReceiver> tmp132;
    compiler::TNode<Number> tmp133;
    compiler::TNode<Number> tmp134;
    compiler::TNode<Smi> tmp135;
    compiler::TNode<Number> tmp136;
    compiler::TNode<Smi> tmp137;
    compiler::TNode<Smi> tmp138;
    compiler::TNode<Smi> tmp139;
    compiler::TNode<Smi> tmp140;
    compiler::TNode<Smi> tmp141;
    compiler::TNode<JSArray> tmp142;
    compiler::TNode<Map> tmp143;
    ca_.Bind(&block14, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 127);
    compiler::TNode<BoolT> tmp144;
    USE(tmp144);
    tmp144 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsArraySpeciesProtectorCellInvalid());
    ca_.Branch(tmp144, &block15, &block16, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp145;
    compiler::TNode<JSReceiver> tmp146;
    compiler::TNode<Number> tmp147;
    compiler::TNode<Number> tmp148;
    compiler::TNode<Smi> tmp149;
    compiler::TNode<Number> tmp150;
    compiler::TNode<Smi> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<Smi> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<JSArray> tmp156;
    compiler::TNode<Map> tmp157;
    ca_.Bind(&block15, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157);
    ca_.Goto(&block1);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp158;
    compiler::TNode<JSReceiver> tmp159;
    compiler::TNode<Number> tmp160;
    compiler::TNode<Number> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<Number> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<Smi> tmp166;
    compiler::TNode<Smi> tmp167;
    compiler::TNode<Smi> tmp168;
    compiler::TNode<JSArray> tmp169;
    compiler::TNode<Map> tmp170;
    ca_.Bind(&block16, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 130);
    compiler::TNode<Int32T> tmp171;
    USE(tmp171);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp171 = CodeStubAssembler(state_).EnsureArrayPushable(compiler::TNode<Map>{tmp170}, &label0);
    ca_.Goto(&block17, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp170, tmp171);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block18, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp170);
    }
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp172;
    compiler::TNode<JSReceiver> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<Number> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<Number> tmp177;
    compiler::TNode<Smi> tmp178;
    compiler::TNode<Smi> tmp179;
    compiler::TNode<Smi> tmp180;
    compiler::TNode<Smi> tmp181;
    compiler::TNode<Smi> tmp182;
    compiler::TNode<JSArray> tmp183;
    compiler::TNode<Map> tmp184;
    compiler::TNode<Map> tmp185;
    ca_.Bind(&block18, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185);
    ca_.Goto(&block1);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp186;
    compiler::TNode<JSReceiver> tmp187;
    compiler::TNode<Number> tmp188;
    compiler::TNode<Number> tmp189;
    compiler::TNode<Smi> tmp190;
    compiler::TNode<Number> tmp191;
    compiler::TNode<Smi> tmp192;
    compiler::TNode<Smi> tmp193;
    compiler::TNode<Smi> tmp194;
    compiler::TNode<Smi> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<JSArray> tmp197;
    compiler::TNode<Map> tmp198;
    compiler::TNode<Map> tmp199;
    compiler::TNode<Int32T> tmp200;
    ca_.Bind(&block17, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 131);
    compiler::TNode<BoolT> tmp201;
    USE(tmp201);
    tmp201 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsFastElementsKind(compiler::TNode<Int32T>{tmp200}));
    compiler::TNode<BoolT> tmp202;
    USE(tmp202);
    tmp202 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp201}));
    ca_.Branch(tmp202, &block19, &block20, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp200);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp203;
    compiler::TNode<JSReceiver> tmp204;
    compiler::TNode<Number> tmp205;
    compiler::TNode<Number> tmp206;
    compiler::TNode<Smi> tmp207;
    compiler::TNode<Number> tmp208;
    compiler::TNode<Smi> tmp209;
    compiler::TNode<Smi> tmp210;
    compiler::TNode<Smi> tmp211;
    compiler::TNode<Smi> tmp212;
    compiler::TNode<Smi> tmp213;
    compiler::TNode<JSArray> tmp214;
    compiler::TNode<Map> tmp215;
    compiler::TNode<Int32T> tmp216;
    ca_.Bind(&block19, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216);
    ca_.Goto(&block1);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp217;
    compiler::TNode<JSReceiver> tmp218;
    compiler::TNode<Number> tmp219;
    compiler::TNode<Number> tmp220;
    compiler::TNode<Smi> tmp221;
    compiler::TNode<Number> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<Smi> tmp224;
    compiler::TNode<Smi> tmp225;
    compiler::TNode<Smi> tmp226;
    compiler::TNode<Smi> tmp227;
    compiler::TNode<JSArray> tmp228;
    compiler::TNode<Map> tmp229;
    compiler::TNode<Int32T> tmp230;
    ca_.Bind(&block20, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 133);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 134);
    compiler::TNode<IntPtrT> tmp231;
    USE(tmp231);
    tmp231 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(p_args));
    compiler::TNode<IntPtrT> tmp232;
    USE(tmp232);
    tmp232 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    ca_.Goto(&block24, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp230, tmp231, tmp232);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp233;
    compiler::TNode<JSReceiver> tmp234;
    compiler::TNode<Number> tmp235;
    compiler::TNode<Number> tmp236;
    compiler::TNode<Smi> tmp237;
    compiler::TNode<Number> tmp238;
    compiler::TNode<Smi> tmp239;
    compiler::TNode<Smi> tmp240;
    compiler::TNode<Smi> tmp241;
    compiler::TNode<Smi> tmp242;
    compiler::TNode<Smi> tmp243;
    compiler::TNode<JSArray> tmp244;
    compiler::TNode<Map> tmp245;
    compiler::TNode<Int32T> tmp246;
    compiler::TNode<Int32T> tmp247;
    compiler::TNode<IntPtrT> tmp248;
    compiler::TNode<IntPtrT> tmp249;
    ca_.Bind(&block24, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249);
    compiler::TNode<BoolT> tmp250;
    USE(tmp250);
    tmp250 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp249}, compiler::TNode<IntPtrT>{tmp248}));
    ca_.Branch(tmp250, &block21, &block23, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp251;
    compiler::TNode<JSReceiver> tmp252;
    compiler::TNode<Number> tmp253;
    compiler::TNode<Number> tmp254;
    compiler::TNode<Smi> tmp255;
    compiler::TNode<Number> tmp256;
    compiler::TNode<Smi> tmp257;
    compiler::TNode<Smi> tmp258;
    compiler::TNode<Smi> tmp259;
    compiler::TNode<Smi> tmp260;
    compiler::TNode<Smi> tmp261;
    compiler::TNode<JSArray> tmp262;
    compiler::TNode<Map> tmp263;
    compiler::TNode<Int32T> tmp264;
    compiler::TNode<Int32T> tmp265;
    compiler::TNode<IntPtrT> tmp266;
    compiler::TNode<IntPtrT> tmp267;
    ca_.Bind(&block21, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267);
    compiler::TNode<Object> tmp268;
    USE(tmp268);
    tmp268 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(p_args, compiler::TNode<IntPtrT>{tmp267}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 135);
    compiler::TNode<BoolT> tmp269;
    USE(tmp269);
    tmp269 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsFastSmiElementsKind(compiler::TNode<Int32T>{tmp264}));
    ca_.Branch(tmp269, &block25, &block26, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp270;
    compiler::TNode<JSReceiver> tmp271;
    compiler::TNode<Number> tmp272;
    compiler::TNode<Number> tmp273;
    compiler::TNode<Smi> tmp274;
    compiler::TNode<Number> tmp275;
    compiler::TNode<Smi> tmp276;
    compiler::TNode<Smi> tmp277;
    compiler::TNode<Smi> tmp278;
    compiler::TNode<Smi> tmp279;
    compiler::TNode<Smi> tmp280;
    compiler::TNode<JSArray> tmp281;
    compiler::TNode<Map> tmp282;
    compiler::TNode<Int32T> tmp283;
    compiler::TNode<Int32T> tmp284;
    compiler::TNode<IntPtrT> tmp285;
    compiler::TNode<IntPtrT> tmp286;
    compiler::TNode<Object> tmp287;
    ca_.Bind(&block25, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 136);
    compiler::TNode<BoolT> tmp288;
    USE(tmp288);
    tmp288 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).TaggedIsNotSmi(compiler::TNode<Object>{tmp287}));
    ca_.Branch(tmp288, &block28, &block29, tmp270, tmp271, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp289;
    compiler::TNode<JSReceiver> tmp290;
    compiler::TNode<Number> tmp291;
    compiler::TNode<Number> tmp292;
    compiler::TNode<Smi> tmp293;
    compiler::TNode<Number> tmp294;
    compiler::TNode<Smi> tmp295;
    compiler::TNode<Smi> tmp296;
    compiler::TNode<Smi> tmp297;
    compiler::TNode<Smi> tmp298;
    compiler::TNode<Smi> tmp299;
    compiler::TNode<JSArray> tmp300;
    compiler::TNode<Map> tmp301;
    compiler::TNode<Int32T> tmp302;
    compiler::TNode<Int32T> tmp303;
    compiler::TNode<IntPtrT> tmp304;
    compiler::TNode<IntPtrT> tmp305;
    compiler::TNode<Object> tmp306;
    ca_.Bind(&block28, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 137);
    compiler::TNode<HeapObject> tmp307;
    USE(tmp307);
    tmp307 = ca_.UncheckedCast<HeapObject>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10HeapObject(compiler::TNode<Context>{tmp289}, compiler::TNode<Object>{tmp306}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 138);
    compiler::TNode<BoolT> tmp308;
    USE(tmp308);
    tmp308 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsHeapNumber(compiler::TNode<HeapObject>{tmp307}));
    ca_.Branch(tmp308, &block30, &block31, tmp289, tmp290, tmp291, tmp292, tmp293, tmp294, tmp295, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307);
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp309;
    compiler::TNode<JSReceiver> tmp310;
    compiler::TNode<Number> tmp311;
    compiler::TNode<Number> tmp312;
    compiler::TNode<Smi> tmp313;
    compiler::TNode<Number> tmp314;
    compiler::TNode<Smi> tmp315;
    compiler::TNode<Smi> tmp316;
    compiler::TNode<Smi> tmp317;
    compiler::TNode<Smi> tmp318;
    compiler::TNode<Smi> tmp319;
    compiler::TNode<JSArray> tmp320;
    compiler::TNode<Map> tmp321;
    compiler::TNode<Int32T> tmp322;
    compiler::TNode<Int32T> tmp323;
    compiler::TNode<IntPtrT> tmp324;
    compiler::TNode<IntPtrT> tmp325;
    compiler::TNode<Object> tmp326;
    compiler::TNode<HeapObject> tmp327;
    ca_.Bind(&block30, &tmp309, &tmp310, &tmp311, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 139);
    compiler::TNode<Int32T> tmp328;
    USE(tmp328);
    tmp328 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).AllowDoubleElements(compiler::TNode<Int32T>{tmp322}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 138);
    ca_.Goto(&block33, tmp309, tmp310, tmp311, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp329;
    compiler::TNode<JSReceiver> tmp330;
    compiler::TNode<Number> tmp331;
    compiler::TNode<Number> tmp332;
    compiler::TNode<Smi> tmp333;
    compiler::TNode<Number> tmp334;
    compiler::TNode<Smi> tmp335;
    compiler::TNode<Smi> tmp336;
    compiler::TNode<Smi> tmp337;
    compiler::TNode<Smi> tmp338;
    compiler::TNode<Smi> tmp339;
    compiler::TNode<JSArray> tmp340;
    compiler::TNode<Map> tmp341;
    compiler::TNode<Int32T> tmp342;
    compiler::TNode<Int32T> tmp343;
    compiler::TNode<IntPtrT> tmp344;
    compiler::TNode<IntPtrT> tmp345;
    compiler::TNode<Object> tmp346;
    compiler::TNode<HeapObject> tmp347;
    ca_.Bind(&block31, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 140);
    compiler::TNode<Int32T> tmp348;
    USE(tmp348);
    tmp348 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).AllowNonNumberElements(compiler::TNode<Int32T>{tmp342}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 138);
    ca_.Goto(&block32, tmp329, tmp330, tmp331, tmp332, tmp333, tmp334, tmp335, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348);
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp349;
    compiler::TNode<JSReceiver> tmp350;
    compiler::TNode<Number> tmp351;
    compiler::TNode<Number> tmp352;
    compiler::TNode<Smi> tmp353;
    compiler::TNode<Number> tmp354;
    compiler::TNode<Smi> tmp355;
    compiler::TNode<Smi> tmp356;
    compiler::TNode<Smi> tmp357;
    compiler::TNode<Smi> tmp358;
    compiler::TNode<Smi> tmp359;
    compiler::TNode<JSArray> tmp360;
    compiler::TNode<Map> tmp361;
    compiler::TNode<Int32T> tmp362;
    compiler::TNode<Int32T> tmp363;
    compiler::TNode<IntPtrT> tmp364;
    compiler::TNode<IntPtrT> tmp365;
    compiler::TNode<Object> tmp366;
    compiler::TNode<HeapObject> tmp367;
    compiler::TNode<Int32T> tmp368;
    ca_.Bind(&block33, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368);
    ca_.Goto(&block32, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, tmp357, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367, tmp368);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp369;
    compiler::TNode<JSReceiver> tmp370;
    compiler::TNode<Number> tmp371;
    compiler::TNode<Number> tmp372;
    compiler::TNode<Smi> tmp373;
    compiler::TNode<Number> tmp374;
    compiler::TNode<Smi> tmp375;
    compiler::TNode<Smi> tmp376;
    compiler::TNode<Smi> tmp377;
    compiler::TNode<Smi> tmp378;
    compiler::TNode<Smi> tmp379;
    compiler::TNode<JSArray> tmp380;
    compiler::TNode<Map> tmp381;
    compiler::TNode<Int32T> tmp382;
    compiler::TNode<Int32T> tmp383;
    compiler::TNode<IntPtrT> tmp384;
    compiler::TNode<IntPtrT> tmp385;
    compiler::TNode<Object> tmp386;
    compiler::TNode<HeapObject> tmp387;
    compiler::TNode<Int32T> tmp388;
    ca_.Bind(&block32, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 136);
    ca_.Goto(&block29, tmp369, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp380, tmp381, tmp388, tmp383, tmp384, tmp385, tmp386);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp389;
    compiler::TNode<JSReceiver> tmp390;
    compiler::TNode<Number> tmp391;
    compiler::TNode<Number> tmp392;
    compiler::TNode<Smi> tmp393;
    compiler::TNode<Number> tmp394;
    compiler::TNode<Smi> tmp395;
    compiler::TNode<Smi> tmp396;
    compiler::TNode<Smi> tmp397;
    compiler::TNode<Smi> tmp398;
    compiler::TNode<Smi> tmp399;
    compiler::TNode<JSArray> tmp400;
    compiler::TNode<Map> tmp401;
    compiler::TNode<Int32T> tmp402;
    compiler::TNode<Int32T> tmp403;
    compiler::TNode<IntPtrT> tmp404;
    compiler::TNode<IntPtrT> tmp405;
    compiler::TNode<Object> tmp406;
    ca_.Bind(&block29, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400, &tmp401, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 135);
    ca_.Goto(&block27, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp396, tmp397, tmp398, tmp399, tmp400, tmp401, tmp402, tmp403, tmp404, tmp405, tmp406);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp407;
    compiler::TNode<JSReceiver> tmp408;
    compiler::TNode<Number> tmp409;
    compiler::TNode<Number> tmp410;
    compiler::TNode<Smi> tmp411;
    compiler::TNode<Number> tmp412;
    compiler::TNode<Smi> tmp413;
    compiler::TNode<Smi> tmp414;
    compiler::TNode<Smi> tmp415;
    compiler::TNode<Smi> tmp416;
    compiler::TNode<Smi> tmp417;
    compiler::TNode<JSArray> tmp418;
    compiler::TNode<Map> tmp419;
    compiler::TNode<Int32T> tmp420;
    compiler::TNode<Int32T> tmp421;
    compiler::TNode<IntPtrT> tmp422;
    compiler::TNode<IntPtrT> tmp423;
    compiler::TNode<Object> tmp424;
    ca_.Bind(&block26, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418, &tmp419, &tmp420, &tmp421, &tmp422, &tmp423, &tmp424);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 142);
    compiler::TNode<BoolT> tmp425;
    USE(tmp425);
    tmp425 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDoubleElementsKind(compiler::TNode<Int32T>{tmp420}));
    ca_.Branch(tmp425, &block34, &block35, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418, tmp419, tmp420, tmp421, tmp422, tmp423, tmp424);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp426;
    compiler::TNode<JSReceiver> tmp427;
    compiler::TNode<Number> tmp428;
    compiler::TNode<Number> tmp429;
    compiler::TNode<Smi> tmp430;
    compiler::TNode<Number> tmp431;
    compiler::TNode<Smi> tmp432;
    compiler::TNode<Smi> tmp433;
    compiler::TNode<Smi> tmp434;
    compiler::TNode<Smi> tmp435;
    compiler::TNode<Smi> tmp436;
    compiler::TNode<JSArray> tmp437;
    compiler::TNode<Map> tmp438;
    compiler::TNode<Int32T> tmp439;
    compiler::TNode<Int32T> tmp440;
    compiler::TNode<IntPtrT> tmp441;
    compiler::TNode<IntPtrT> tmp442;
    compiler::TNode<Object> tmp443;
    ca_.Bind(&block34, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442, &tmp443);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 143);
    compiler::TNode<BoolT> tmp444;
    USE(tmp444);
    tmp444 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNumber(compiler::TNode<Object>{tmp443}));
    compiler::TNode<BoolT> tmp445;
    USE(tmp445);
    tmp445 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32BinaryNot(compiler::TNode<BoolT>{tmp444}));
    ca_.Branch(tmp445, &block36, &block37, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435, tmp436, tmp437, tmp438, tmp439, tmp440, tmp441, tmp442, tmp443);
  }

  if (block36.is_used()) {
    compiler::TNode<Context> tmp446;
    compiler::TNode<JSReceiver> tmp447;
    compiler::TNode<Number> tmp448;
    compiler::TNode<Number> tmp449;
    compiler::TNode<Smi> tmp450;
    compiler::TNode<Number> tmp451;
    compiler::TNode<Smi> tmp452;
    compiler::TNode<Smi> tmp453;
    compiler::TNode<Smi> tmp454;
    compiler::TNode<Smi> tmp455;
    compiler::TNode<Smi> tmp456;
    compiler::TNode<JSArray> tmp457;
    compiler::TNode<Map> tmp458;
    compiler::TNode<Int32T> tmp459;
    compiler::TNode<Int32T> tmp460;
    compiler::TNode<IntPtrT> tmp461;
    compiler::TNode<IntPtrT> tmp462;
    compiler::TNode<Object> tmp463;
    ca_.Bind(&block36, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461, &tmp462, &tmp463);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 144);
    compiler::TNode<Int32T> tmp464;
    USE(tmp464);
    tmp464 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).AllowNonNumberElements(compiler::TNode<Int32T>{tmp459}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 143);
    ca_.Goto(&block37, tmp446, tmp447, tmp448, tmp449, tmp450, tmp451, tmp452, tmp453, tmp454, tmp455, tmp456, tmp457, tmp458, tmp464, tmp460, tmp461, tmp462, tmp463);
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp465;
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
    compiler::TNode<IntPtrT> tmp481;
    compiler::TNode<Object> tmp482;
    ca_.Bind(&block37, &tmp465, &tmp466, &tmp467, &tmp468, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474, &tmp475, &tmp476, &tmp477, &tmp478, &tmp479, &tmp480, &tmp481, &tmp482);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 142);
    ca_.Goto(&block35, tmp465, tmp466, tmp467, tmp468, tmp469, tmp470, tmp471, tmp472, tmp473, tmp474, tmp475, tmp476, tmp477, tmp478, tmp479, tmp480, tmp481, tmp482);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp483;
    compiler::TNode<JSReceiver> tmp484;
    compiler::TNode<Number> tmp485;
    compiler::TNode<Number> tmp486;
    compiler::TNode<Smi> tmp487;
    compiler::TNode<Number> tmp488;
    compiler::TNode<Smi> tmp489;
    compiler::TNode<Smi> tmp490;
    compiler::TNode<Smi> tmp491;
    compiler::TNode<Smi> tmp492;
    compiler::TNode<Smi> tmp493;
    compiler::TNode<JSArray> tmp494;
    compiler::TNode<Map> tmp495;
    compiler::TNode<Int32T> tmp496;
    compiler::TNode<Int32T> tmp497;
    compiler::TNode<IntPtrT> tmp498;
    compiler::TNode<IntPtrT> tmp499;
    compiler::TNode<Object> tmp500;
    ca_.Bind(&block35, &tmp483, &tmp484, &tmp485, &tmp486, &tmp487, &tmp488, &tmp489, &tmp490, &tmp491, &tmp492, &tmp493, &tmp494, &tmp495, &tmp496, &tmp497, &tmp498, &tmp499, &tmp500);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 135);
    ca_.Goto(&block27, tmp483, tmp484, tmp485, tmp486, tmp487, tmp488, tmp489, tmp490, tmp491, tmp492, tmp493, tmp494, tmp495, tmp496, tmp497, tmp498, tmp499, tmp500);
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp501;
    compiler::TNode<JSReceiver> tmp502;
    compiler::TNode<Number> tmp503;
    compiler::TNode<Number> tmp504;
    compiler::TNode<Smi> tmp505;
    compiler::TNode<Number> tmp506;
    compiler::TNode<Smi> tmp507;
    compiler::TNode<Smi> tmp508;
    compiler::TNode<Smi> tmp509;
    compiler::TNode<Smi> tmp510;
    compiler::TNode<Smi> tmp511;
    compiler::TNode<JSArray> tmp512;
    compiler::TNode<Map> tmp513;
    compiler::TNode<Int32T> tmp514;
    compiler::TNode<Int32T> tmp515;
    compiler::TNode<IntPtrT> tmp516;
    compiler::TNode<IntPtrT> tmp517;
    compiler::TNode<Object> tmp518;
    ca_.Bind(&block27, &tmp501, &tmp502, &tmp503, &tmp504, &tmp505, &tmp506, &tmp507, &tmp508, &tmp509, &tmp510, &tmp511, &tmp512, &tmp513, &tmp514, &tmp515, &tmp516, &tmp517, &tmp518);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 134);
    ca_.Goto(&block22, tmp501, tmp502, tmp503, tmp504, tmp505, tmp506, tmp507, tmp508, tmp509, tmp510, tmp511, tmp512, tmp513, tmp514, tmp515, tmp516, tmp517);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp519;
    compiler::TNode<JSReceiver> tmp520;
    compiler::TNode<Number> tmp521;
    compiler::TNode<Number> tmp522;
    compiler::TNode<Smi> tmp523;
    compiler::TNode<Number> tmp524;
    compiler::TNode<Smi> tmp525;
    compiler::TNode<Smi> tmp526;
    compiler::TNode<Smi> tmp527;
    compiler::TNode<Smi> tmp528;
    compiler::TNode<Smi> tmp529;
    compiler::TNode<JSArray> tmp530;
    compiler::TNode<Map> tmp531;
    compiler::TNode<Int32T> tmp532;
    compiler::TNode<Int32T> tmp533;
    compiler::TNode<IntPtrT> tmp534;
    compiler::TNode<IntPtrT> tmp535;
    ca_.Bind(&block22, &tmp519, &tmp520, &tmp521, &tmp522, &tmp523, &tmp524, &tmp525, &tmp526, &tmp527, &tmp528, &tmp529, &tmp530, &tmp531, &tmp532, &tmp533, &tmp534, &tmp535);
    compiler::TNode<IntPtrT> tmp536;
    USE(tmp536);
    tmp536 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp537;
    USE(tmp537);
    tmp537 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp535}, compiler::TNode<IntPtrT>{tmp536}));
    ca_.Goto(&block24, tmp519, tmp520, tmp521, tmp522, tmp523, tmp524, tmp525, tmp526, tmp527, tmp528, tmp529, tmp530, tmp531, tmp532, tmp533, tmp534, tmp537);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp538;
    compiler::TNode<JSReceiver> tmp539;
    compiler::TNode<Number> tmp540;
    compiler::TNode<Number> tmp541;
    compiler::TNode<Smi> tmp542;
    compiler::TNode<Number> tmp543;
    compiler::TNode<Smi> tmp544;
    compiler::TNode<Smi> tmp545;
    compiler::TNode<Smi> tmp546;
    compiler::TNode<Smi> tmp547;
    compiler::TNode<Smi> tmp548;
    compiler::TNode<JSArray> tmp549;
    compiler::TNode<Map> tmp550;
    compiler::TNode<Int32T> tmp551;
    compiler::TNode<Int32T> tmp552;
    compiler::TNode<IntPtrT> tmp553;
    compiler::TNode<IntPtrT> tmp554;
    ca_.Bind(&block23, &tmp538, &tmp539, &tmp540, &tmp541, &tmp542, &tmp543, &tmp544, &tmp545, &tmp546, &tmp547, &tmp548, &tmp549, &tmp550, &tmp551, &tmp552, &tmp553, &tmp554);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 149);
    compiler::TNode<BoolT> tmp555;
    USE(tmp555);
    tmp555 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).ElementsKindNotEqual(compiler::TNode<Int32T>{tmp551}, compiler::TNode<Int32T>{tmp552}));
    ca_.Branch(tmp555, &block38, &block39, tmp538, tmp539, tmp540, tmp541, tmp542, tmp543, tmp544, tmp545, tmp546, tmp547, tmp548, tmp549, tmp550, tmp551, tmp552);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp556;
    compiler::TNode<JSReceiver> tmp557;
    compiler::TNode<Number> tmp558;
    compiler::TNode<Number> tmp559;
    compiler::TNode<Smi> tmp560;
    compiler::TNode<Number> tmp561;
    compiler::TNode<Smi> tmp562;
    compiler::TNode<Smi> tmp563;
    compiler::TNode<Smi> tmp564;
    compiler::TNode<Smi> tmp565;
    compiler::TNode<Smi> tmp566;
    compiler::TNode<JSArray> tmp567;
    compiler::TNode<Map> tmp568;
    compiler::TNode<Int32T> tmp569;
    compiler::TNode<Int32T> tmp570;
    ca_.Bind(&block38, &tmp556, &tmp557, &tmp558, &tmp559, &tmp560, &tmp561, &tmp562, &tmp563, &tmp564, &tmp565, &tmp566, &tmp567, &tmp568, &tmp569, &tmp570);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 150);
    compiler::TNode<Int32T> tmp571;
    USE(tmp571);
    tmp571 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).Convert7ATint3214ATElementsKind(compiler::TNode<Int32T>{tmp569}));
    compiler::TNode<Smi> tmp572;
    USE(tmp572);
    tmp572 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi7ATint32(compiler::TNode<Int32T>{tmp571}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 151);
    CodeStubAssembler(state_).CallRuntime(Runtime::kTransitionElementsKindWithKind, tmp556, tmp567, tmp572);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 149);
    ca_.Goto(&block39, tmp556, tmp557, tmp558, tmp559, tmp560, tmp561, tmp562, tmp563, tmp564, tmp565, tmp566, tmp567, tmp568, tmp569, tmp570);
  }

  if (block39.is_used()) {
    compiler::TNode<Context> tmp574;
    compiler::TNode<JSReceiver> tmp575;
    compiler::TNode<Number> tmp576;
    compiler::TNode<Number> tmp577;
    compiler::TNode<Smi> tmp578;
    compiler::TNode<Number> tmp579;
    compiler::TNode<Smi> tmp580;
    compiler::TNode<Smi> tmp581;
    compiler::TNode<Smi> tmp582;
    compiler::TNode<Smi> tmp583;
    compiler::TNode<Smi> tmp584;
    compiler::TNode<JSArray> tmp585;
    compiler::TNode<Map> tmp586;
    compiler::TNode<Int32T> tmp587;
    compiler::TNode<Int32T> tmp588;
    ca_.Bind(&block39, &tmp574, &tmp575, &tmp576, &tmp577, &tmp578, &tmp579, &tmp580, &tmp581, &tmp582, &tmp583, &tmp584, &tmp585, &tmp586, &tmp587, &tmp588);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 155);
    compiler::TNode<Number> tmp589;
    USE(tmp589);
    tmp589 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).LoadJSArrayLength(compiler::TNode<JSArray>{tmp585}));
    compiler::TNode<Smi> tmp590;
    USE(tmp590);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp590 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp589}, &label0);
    ca_.Goto(&block40, tmp574, tmp575, tmp576, tmp577, tmp578, tmp579, tmp580, tmp581, tmp582, tmp583, tmp584, tmp585, tmp586, tmp587, tmp588, tmp589, tmp590);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block41, tmp574, tmp575, tmp576, tmp577, tmp578, tmp579, tmp580, tmp581, tmp582, tmp583, tmp584, tmp585, tmp586, tmp587, tmp588, tmp589);
    }
  }

  if (block41.is_used()) {
    compiler::TNode<Context> tmp591;
    compiler::TNode<JSReceiver> tmp592;
    compiler::TNode<Number> tmp593;
    compiler::TNode<Number> tmp594;
    compiler::TNode<Smi> tmp595;
    compiler::TNode<Number> tmp596;
    compiler::TNode<Smi> tmp597;
    compiler::TNode<Smi> tmp598;
    compiler::TNode<Smi> tmp599;
    compiler::TNode<Smi> tmp600;
    compiler::TNode<Smi> tmp601;
    compiler::TNode<JSArray> tmp602;
    compiler::TNode<Map> tmp603;
    compiler::TNode<Int32T> tmp604;
    compiler::TNode<Int32T> tmp605;
    compiler::TNode<Number> tmp606;
    ca_.Bind(&block41, &tmp591, &tmp592, &tmp593, &tmp594, &tmp595, &tmp596, &tmp597, &tmp598, &tmp599, &tmp600, &tmp601, &tmp602, &tmp603, &tmp604, &tmp605, &tmp606);
    ca_.Goto(&block1);
  }

  if (block40.is_used()) {
    compiler::TNode<Context> tmp607;
    compiler::TNode<JSReceiver> tmp608;
    compiler::TNode<Number> tmp609;
    compiler::TNode<Number> tmp610;
    compiler::TNode<Smi> tmp611;
    compiler::TNode<Number> tmp612;
    compiler::TNode<Smi> tmp613;
    compiler::TNode<Smi> tmp614;
    compiler::TNode<Smi> tmp615;
    compiler::TNode<Smi> tmp616;
    compiler::TNode<Smi> tmp617;
    compiler::TNode<JSArray> tmp618;
    compiler::TNode<Map> tmp619;
    compiler::TNode<Int32T> tmp620;
    compiler::TNode<Int32T> tmp621;
    compiler::TNode<Number> tmp622;
    compiler::TNode<Smi> tmp623;
    ca_.Bind(&block40, &tmp607, &tmp608, &tmp609, &tmp610, &tmp611, &tmp612, &tmp613, &tmp614, &tmp615, &tmp616, &tmp617, &tmp618, &tmp619, &tmp620, &tmp621, &tmp622, &tmp623);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 156);
    compiler::TNode<BoolT> tmp624;
    USE(tmp624);
    tmp624 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiNotEqual(compiler::TNode<Smi>{tmp613}, compiler::TNode<Smi>{tmp623}));
    ca_.Branch(tmp624, &block42, &block43, tmp607, tmp608, tmp609, tmp610, tmp611, tmp612, tmp613, tmp614, tmp615, tmp616, tmp617, tmp618, tmp619, tmp620, tmp621, tmp623);
  }

  if (block42.is_used()) {
    compiler::TNode<Context> tmp625;
    compiler::TNode<JSReceiver> tmp626;
    compiler::TNode<Number> tmp627;
    compiler::TNode<Number> tmp628;
    compiler::TNode<Smi> tmp629;
    compiler::TNode<Number> tmp630;
    compiler::TNode<Smi> tmp631;
    compiler::TNode<Smi> tmp632;
    compiler::TNode<Smi> tmp633;
    compiler::TNode<Smi> tmp634;
    compiler::TNode<Smi> tmp635;
    compiler::TNode<JSArray> tmp636;
    compiler::TNode<Map> tmp637;
    compiler::TNode<Int32T> tmp638;
    compiler::TNode<Int32T> tmp639;
    compiler::TNode<Smi> tmp640;
    ca_.Bind(&block42, &tmp625, &tmp626, &tmp627, &tmp628, &tmp629, &tmp630, &tmp631, &tmp632, &tmp633, &tmp634, &tmp635, &tmp636, &tmp637, &tmp638, &tmp639, &tmp640);
    ca_.Goto(&block1);
  }

  if (block43.is_used()) {
    compiler::TNode<Context> tmp641;
    compiler::TNode<JSReceiver> tmp642;
    compiler::TNode<Number> tmp643;
    compiler::TNode<Number> tmp644;
    compiler::TNode<Smi> tmp645;
    compiler::TNode<Number> tmp646;
    compiler::TNode<Smi> tmp647;
    compiler::TNode<Smi> tmp648;
    compiler::TNode<Smi> tmp649;
    compiler::TNode<Smi> tmp650;
    compiler::TNode<Smi> tmp651;
    compiler::TNode<JSArray> tmp652;
    compiler::TNode<Map> tmp653;
    compiler::TNode<Int32T> tmp654;
    compiler::TNode<Int32T> tmp655;
    compiler::TNode<Smi> tmp656;
    ca_.Bind(&block43, &tmp641, &tmp642, &tmp643, &tmp644, &tmp645, &tmp646, &tmp647, &tmp648, &tmp649, &tmp650, &tmp651, &tmp652, &tmp653, &tmp654, &tmp655, &tmp656);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 159);
    compiler::TNode<JSArray> tmp657;
    tmp657 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kExtractFastJSArray, tmp641, tmp652, tmp648, tmp649));
    USE(tmp657);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 158);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 161);
    compiler::TNode<Smi> tmp658;
    USE(tmp658);
    tmp658 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp659;
    USE(tmp659);
    tmp659 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp651}, compiler::TNode<Smi>{tmp658}));
    ca_.Branch(tmp659, &block44, &block45, tmp641, tmp642, tmp643, tmp644, tmp645, tmp646, tmp647, tmp648, tmp649, tmp650, tmp651, tmp652, tmp653, tmp654, tmp655, tmp656, tmp657);
  }

  if (block44.is_used()) {
    compiler::TNode<Context> tmp660;
    compiler::TNode<JSReceiver> tmp661;
    compiler::TNode<Number> tmp662;
    compiler::TNode<Number> tmp663;
    compiler::TNode<Smi> tmp664;
    compiler::TNode<Number> tmp665;
    compiler::TNode<Smi> tmp666;
    compiler::TNode<Smi> tmp667;
    compiler::TNode<Smi> tmp668;
    compiler::TNode<Smi> tmp669;
    compiler::TNode<Smi> tmp670;
    compiler::TNode<JSArray> tmp671;
    compiler::TNode<Map> tmp672;
    compiler::TNode<Int32T> tmp673;
    compiler::TNode<Int32T> tmp674;
    compiler::TNode<Smi> tmp675;
    compiler::TNode<JSArray> tmp676;
    ca_.Bind(&block44, &tmp660, &tmp661, &tmp662, &tmp663, &tmp664, &tmp665, &tmp666, &tmp667, &tmp668, &tmp669, &tmp670, &tmp671, &tmp672, &tmp673, &tmp674, &tmp675, &tmp676);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 162);
    compiler::TNode<FixedArray> tmp677;
    USE(tmp677);
    tmp677 = BaseBuiltinsFromDSLAssembler(state_).kEmptyFixedArray();
    BaseBuiltinsFromDSLAssembler(state_).StoreJSObjectElements(compiler::TNode<JSObject>{tmp671}, compiler::TNode<FixedArrayBase>{tmp677});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 163);
    compiler::TNode<Number> tmp678;
    USE(tmp678);
    tmp678 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    BaseBuiltinsFromDSLAssembler(state_).StoreJSArrayLength(compiler::TNode<JSArray>{tmp671}, compiler::TNode<Number>{tmp678});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 164);
    ca_.Goto(&block2, tmp660, tmp661, tmp662, tmp663, tmp664, tmp665, tmp676);
  }

  if (block45.is_used()) {
    compiler::TNode<Context> tmp679;
    compiler::TNode<JSReceiver> tmp680;
    compiler::TNode<Number> tmp681;
    compiler::TNode<Number> tmp682;
    compiler::TNode<Smi> tmp683;
    compiler::TNode<Number> tmp684;
    compiler::TNode<Smi> tmp685;
    compiler::TNode<Smi> tmp686;
    compiler::TNode<Smi> tmp687;
    compiler::TNode<Smi> tmp688;
    compiler::TNode<Smi> tmp689;
    compiler::TNode<JSArray> tmp690;
    compiler::TNode<Map> tmp691;
    compiler::TNode<Int32T> tmp692;
    compiler::TNode<Int32T> tmp693;
    compiler::TNode<Smi> tmp694;
    compiler::TNode<JSArray> tmp695;
    ca_.Bind(&block45, &tmp679, &tmp680, &tmp681, &tmp682, &tmp683, &tmp684, &tmp685, &tmp686, &tmp687, &tmp688, &tmp689, &tmp690, &tmp691, &tmp692, &tmp693, &tmp694, &tmp695);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 167);
    compiler::TNode<BoolT> tmp696;
    USE(tmp696);
    tmp696 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsFastSmiOrTaggedElementsKind(compiler::TNode<Int32T>{tmp692}));
    ca_.Branch(tmp696, &block46, &block47, tmp679, tmp680, tmp681, tmp682, tmp683, tmp684, tmp685, tmp686, tmp687, tmp688, tmp689, tmp690, tmp691, tmp692, tmp693, tmp694, tmp695);
  }

  if (block46.is_used()) {
    compiler::TNode<Context> tmp697;
    compiler::TNode<JSReceiver> tmp698;
    compiler::TNode<Number> tmp699;
    compiler::TNode<Number> tmp700;
    compiler::TNode<Smi> tmp701;
    compiler::TNode<Number> tmp702;
    compiler::TNode<Smi> tmp703;
    compiler::TNode<Smi> tmp704;
    compiler::TNode<Smi> tmp705;
    compiler::TNode<Smi> tmp706;
    compiler::TNode<Smi> tmp707;
    compiler::TNode<JSArray> tmp708;
    compiler::TNode<Map> tmp709;
    compiler::TNode<Int32T> tmp710;
    compiler::TNode<Int32T> tmp711;
    compiler::TNode<Smi> tmp712;
    compiler::TNode<JSArray> tmp713;
    ca_.Bind(&block46, &tmp697, &tmp698, &tmp699, &tmp700, &tmp701, &tmp702, &tmp703, &tmp704, &tmp705, &tmp706, &tmp707, &tmp708, &tmp709, &tmp710, &tmp711, &tmp712, &tmp713);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 169);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 170);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 168);
    compiler::CodeAssemblerLabel label0(&ca_);
    ArraySpliceBuiltinsFromDSLAssembler(state_).FastSplice10FixedArray20UT5ATSmi10HeapObject(compiler::TNode<Context>{tmp697}, p_args, compiler::TNode<JSArray>{tmp708}, compiler::TNode<Smi>{tmp712}, compiler::TNode<Smi>{tmp707}, compiler::TNode<Smi>{tmp706}, compiler::TNode<Smi>{tmp704}, compiler::TNode<Smi>{tmp701}, compiler::TNode<Smi>{tmp705}, &label0);
    ca_.Goto(&block49, tmp697, tmp698, tmp699, tmp700, tmp701, tmp702, tmp703, tmp704, tmp705, tmp706, tmp707, tmp708, tmp709, tmp710, tmp711, tmp712, tmp713, tmp708, tmp712, tmp707, tmp706, tmp704, tmp701, tmp705);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block50, tmp697, tmp698, tmp699, tmp700, tmp701, tmp702, tmp703, tmp704, tmp705, tmp706, tmp707, tmp708, tmp709, tmp710, tmp711, tmp712, tmp713, tmp708, tmp712, tmp707, tmp706, tmp704, tmp701, tmp705);
    }
  }

  if (block50.is_used()) {
    compiler::TNode<Context> tmp714;
    compiler::TNode<JSReceiver> tmp715;
    compiler::TNode<Number> tmp716;
    compiler::TNode<Number> tmp717;
    compiler::TNode<Smi> tmp718;
    compiler::TNode<Number> tmp719;
    compiler::TNode<Smi> tmp720;
    compiler::TNode<Smi> tmp721;
    compiler::TNode<Smi> tmp722;
    compiler::TNode<Smi> tmp723;
    compiler::TNode<Smi> tmp724;
    compiler::TNode<JSArray> tmp725;
    compiler::TNode<Map> tmp726;
    compiler::TNode<Int32T> tmp727;
    compiler::TNode<Int32T> tmp728;
    compiler::TNode<Smi> tmp729;
    compiler::TNode<JSArray> tmp730;
    compiler::TNode<JSArray> tmp731;
    compiler::TNode<Smi> tmp732;
    compiler::TNode<Smi> tmp733;
    compiler::TNode<Smi> tmp734;
    compiler::TNode<Smi> tmp735;
    compiler::TNode<Smi> tmp736;
    compiler::TNode<Smi> tmp737;
    ca_.Bind(&block50, &tmp714, &tmp715, &tmp716, &tmp717, &tmp718, &tmp719, &tmp720, &tmp721, &tmp722, &tmp723, &tmp724, &tmp725, &tmp726, &tmp727, &tmp728, &tmp729, &tmp730, &tmp731, &tmp732, &tmp733, &tmp734, &tmp735, &tmp736, &tmp737);
    ca_.Goto(&block1);
  }

  if (block49.is_used()) {
    compiler::TNode<Context> tmp738;
    compiler::TNode<JSReceiver> tmp739;
    compiler::TNode<Number> tmp740;
    compiler::TNode<Number> tmp741;
    compiler::TNode<Smi> tmp742;
    compiler::TNode<Number> tmp743;
    compiler::TNode<Smi> tmp744;
    compiler::TNode<Smi> tmp745;
    compiler::TNode<Smi> tmp746;
    compiler::TNode<Smi> tmp747;
    compiler::TNode<Smi> tmp748;
    compiler::TNode<JSArray> tmp749;
    compiler::TNode<Map> tmp750;
    compiler::TNode<Int32T> tmp751;
    compiler::TNode<Int32T> tmp752;
    compiler::TNode<Smi> tmp753;
    compiler::TNode<JSArray> tmp754;
    compiler::TNode<JSArray> tmp755;
    compiler::TNode<Smi> tmp756;
    compiler::TNode<Smi> tmp757;
    compiler::TNode<Smi> tmp758;
    compiler::TNode<Smi> tmp759;
    compiler::TNode<Smi> tmp760;
    compiler::TNode<Smi> tmp761;
    ca_.Bind(&block49, &tmp738, &tmp739, &tmp740, &tmp741, &tmp742, &tmp743, &tmp744, &tmp745, &tmp746, &tmp747, &tmp748, &tmp749, &tmp750, &tmp751, &tmp752, &tmp753, &tmp754, &tmp755, &tmp756, &tmp757, &tmp758, &tmp759, &tmp760, &tmp761);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 167);
    ca_.Goto(&block48, tmp738, tmp739, tmp740, tmp741, tmp742, tmp743, tmp744, tmp745, tmp746, tmp747, tmp748, tmp749, tmp750, tmp751, tmp752, tmp753, tmp754);
  }

  if (block47.is_used()) {
    compiler::TNode<Context> tmp762;
    compiler::TNode<JSReceiver> tmp763;
    compiler::TNode<Number> tmp764;
    compiler::TNode<Number> tmp765;
    compiler::TNode<Smi> tmp766;
    compiler::TNode<Number> tmp767;
    compiler::TNode<Smi> tmp768;
    compiler::TNode<Smi> tmp769;
    compiler::TNode<Smi> tmp770;
    compiler::TNode<Smi> tmp771;
    compiler::TNode<Smi> tmp772;
    compiler::TNode<JSArray> tmp773;
    compiler::TNode<Map> tmp774;
    compiler::TNode<Int32T> tmp775;
    compiler::TNode<Int32T> tmp776;
    compiler::TNode<Smi> tmp777;
    compiler::TNode<JSArray> tmp778;
    ca_.Bind(&block47, &tmp762, &tmp763, &tmp764, &tmp765, &tmp766, &tmp767, &tmp768, &tmp769, &tmp770, &tmp771, &tmp772, &tmp773, &tmp774, &tmp775, &tmp776, &tmp777, &tmp778);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 173);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 174);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 172);
    compiler::CodeAssemblerLabel label0(&ca_);
    ArraySpliceBuiltinsFromDSLAssembler(state_).FastSplice16FixedDoubleArray20UT5ATSmi10HeapNumber(compiler::TNode<Context>{tmp762}, p_args, compiler::TNode<JSArray>{tmp773}, compiler::TNode<Smi>{tmp777}, compiler::TNode<Smi>{tmp772}, compiler::TNode<Smi>{tmp771}, compiler::TNode<Smi>{tmp769}, compiler::TNode<Smi>{tmp766}, compiler::TNode<Smi>{tmp770}, &label0);
    ca_.Goto(&block51, tmp762, tmp763, tmp764, tmp765, tmp766, tmp767, tmp768, tmp769, tmp770, tmp771, tmp772, tmp773, tmp774, tmp775, tmp776, tmp777, tmp778, tmp773, tmp777, tmp772, tmp771, tmp769, tmp766, tmp770);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block52, tmp762, tmp763, tmp764, tmp765, tmp766, tmp767, tmp768, tmp769, tmp770, tmp771, tmp772, tmp773, tmp774, tmp775, tmp776, tmp777, tmp778, tmp773, tmp777, tmp772, tmp771, tmp769, tmp766, tmp770);
    }
  }

  if (block52.is_used()) {
    compiler::TNode<Context> tmp779;
    compiler::TNode<JSReceiver> tmp780;
    compiler::TNode<Number> tmp781;
    compiler::TNode<Number> tmp782;
    compiler::TNode<Smi> tmp783;
    compiler::TNode<Number> tmp784;
    compiler::TNode<Smi> tmp785;
    compiler::TNode<Smi> tmp786;
    compiler::TNode<Smi> tmp787;
    compiler::TNode<Smi> tmp788;
    compiler::TNode<Smi> tmp789;
    compiler::TNode<JSArray> tmp790;
    compiler::TNode<Map> tmp791;
    compiler::TNode<Int32T> tmp792;
    compiler::TNode<Int32T> tmp793;
    compiler::TNode<Smi> tmp794;
    compiler::TNode<JSArray> tmp795;
    compiler::TNode<JSArray> tmp796;
    compiler::TNode<Smi> tmp797;
    compiler::TNode<Smi> tmp798;
    compiler::TNode<Smi> tmp799;
    compiler::TNode<Smi> tmp800;
    compiler::TNode<Smi> tmp801;
    compiler::TNode<Smi> tmp802;
    ca_.Bind(&block52, &tmp779, &tmp780, &tmp781, &tmp782, &tmp783, &tmp784, &tmp785, &tmp786, &tmp787, &tmp788, &tmp789, &tmp790, &tmp791, &tmp792, &tmp793, &tmp794, &tmp795, &tmp796, &tmp797, &tmp798, &tmp799, &tmp800, &tmp801, &tmp802);
    ca_.Goto(&block1);
  }

  if (block51.is_used()) {
    compiler::TNode<Context> tmp803;
    compiler::TNode<JSReceiver> tmp804;
    compiler::TNode<Number> tmp805;
    compiler::TNode<Number> tmp806;
    compiler::TNode<Smi> tmp807;
    compiler::TNode<Number> tmp808;
    compiler::TNode<Smi> tmp809;
    compiler::TNode<Smi> tmp810;
    compiler::TNode<Smi> tmp811;
    compiler::TNode<Smi> tmp812;
    compiler::TNode<Smi> tmp813;
    compiler::TNode<JSArray> tmp814;
    compiler::TNode<Map> tmp815;
    compiler::TNode<Int32T> tmp816;
    compiler::TNode<Int32T> tmp817;
    compiler::TNode<Smi> tmp818;
    compiler::TNode<JSArray> tmp819;
    compiler::TNode<JSArray> tmp820;
    compiler::TNode<Smi> tmp821;
    compiler::TNode<Smi> tmp822;
    compiler::TNode<Smi> tmp823;
    compiler::TNode<Smi> tmp824;
    compiler::TNode<Smi> tmp825;
    compiler::TNode<Smi> tmp826;
    ca_.Bind(&block51, &tmp803, &tmp804, &tmp805, &tmp806, &tmp807, &tmp808, &tmp809, &tmp810, &tmp811, &tmp812, &tmp813, &tmp814, &tmp815, &tmp816, &tmp817, &tmp818, &tmp819, &tmp820, &tmp821, &tmp822, &tmp823, &tmp824, &tmp825, &tmp826);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 167);
    ca_.Goto(&block48, tmp803, tmp804, tmp805, tmp806, tmp807, tmp808, tmp809, tmp810, tmp811, tmp812, tmp813, tmp814, tmp815, tmp816, tmp817, tmp818, tmp819);
  }

  if (block48.is_used()) {
    compiler::TNode<Context> tmp827;
    compiler::TNode<JSReceiver> tmp828;
    compiler::TNode<Number> tmp829;
    compiler::TNode<Number> tmp830;
    compiler::TNode<Smi> tmp831;
    compiler::TNode<Number> tmp832;
    compiler::TNode<Smi> tmp833;
    compiler::TNode<Smi> tmp834;
    compiler::TNode<Smi> tmp835;
    compiler::TNode<Smi> tmp836;
    compiler::TNode<Smi> tmp837;
    compiler::TNode<JSArray> tmp838;
    compiler::TNode<Map> tmp839;
    compiler::TNode<Int32T> tmp840;
    compiler::TNode<Int32T> tmp841;
    compiler::TNode<Smi> tmp842;
    compiler::TNode<JSArray> tmp843;
    ca_.Bind(&block48, &tmp827, &tmp828, &tmp829, &tmp830, &tmp831, &tmp832, &tmp833, &tmp834, &tmp835, &tmp836, &tmp837, &tmp838, &tmp839, &tmp840, &tmp841, &tmp842, &tmp843);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 177);
    ca_.Goto(&block2, tmp827, tmp828, tmp829, tmp830, tmp831, tmp832, tmp843);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp844;
    compiler::TNode<JSReceiver> tmp845;
    compiler::TNode<Number> tmp846;
    compiler::TNode<Number> tmp847;
    compiler::TNode<Smi> tmp848;
    compiler::TNode<Number> tmp849;
    compiler::TNode<Object> tmp850;
    ca_.Bind(&block2, &tmp844, &tmp845, &tmp846, &tmp847, &tmp848, &tmp849, &tmp850);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 109);
    ca_.Goto(&block53, tmp844, tmp845, tmp846, tmp847, tmp848, tmp849, tmp850);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp851;
    compiler::TNode<JSReceiver> tmp852;
    compiler::TNode<Number> tmp853;
    compiler::TNode<Number> tmp854;
    compiler::TNode<Smi> tmp855;
    compiler::TNode<Number> tmp856;
    compiler::TNode<Object> tmp857;
    ca_.Bind(&block53, &tmp851, &tmp852, &tmp853, &tmp854, &tmp855, &tmp856, &tmp857);
  return compiler::TNode<Object>{tmp857};
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 184);
    compiler::TNode<Number> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 187);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 189);
    compiler::TNode<Number> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp30}, compiler::TNode<Number>{tmp33}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 192);
    compiler::TNode<Oddball> tmp35;
    tmp35 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp28, tmp29, tmp34));
    USE(tmp35);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 195);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 197);
    compiler::TNode<Object> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp38}, compiler::TNode<Object>{tmp39}, compiler::TNode<Object>{tmp44}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 200);
    compiler::TNode<Object> tmp47;
    tmp47 = CodeStubAssembler(state_).CallBuiltin(Builtins::kFastCreateDataProperty, tmp38, tmp42, tmp43, tmp46);
    USE(tmp47);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 195);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 204);
    compiler::TNode<Number> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp53}, compiler::TNode<Number>{tmp56}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 187);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 207);
    compiler::TNode<String> tmp64;
    USE(tmp64);
    tmp64 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp58, tmp62, tmp64, tmp61);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 208);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 180);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 218);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 221);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 223);
    compiler::TNode<Number> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp38}, compiler::TNode<Number>{tmp37}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 225);
    compiler::TNode<Number> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp38}, compiler::TNode<Number>{tmp35}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 228);
    compiler::TNode<Oddball> tmp41;
    tmp41 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp32, tmp33, tmp39));
    USE(tmp41);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 231);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 233);
    compiler::TNode<Object> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp44}, compiler::TNode<Object>{tmp45}, compiler::TNode<Object>{tmp51}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 236);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp44, tmp45, tmp52, tmp54);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 231);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 241);
    compiler::TNode<Smi> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp56, tmp57, tmp64, tmp66);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 231);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 244);
    compiler::TNode<Number> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp74}, compiler::TNode<Number>{tmp78}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 221);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 248);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 251);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 253);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 255);
    compiler::TNode<Number> tmp125;
    USE(tmp125);
    tmp125 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp126;
    USE(tmp126);
    tmp126 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp120}, compiler::TNode<Number>{tmp125}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 251);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 215);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 213);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 266);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp2}, compiler::TNode<Number>{tmp5}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 269);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 271);
    compiler::TNode<Number> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp38}, compiler::TNode<Number>{tmp37}));
    compiler::TNode<Number> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp39}, compiler::TNode<Number>{tmp40}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 274);
    compiler::TNode<Number> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp38}, compiler::TNode<Number>{tmp35}));
    compiler::TNode<Number> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp42}, compiler::TNode<Number>{tmp43}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 277);
    compiler::TNode<Oddball> tmp45;
    tmp45 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp32, tmp33, tmp41));
    USE(tmp45);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 280);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 282);
    compiler::TNode<Object> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp48}, compiler::TNode<Object>{tmp49}, compiler::TNode<Object>{tmp55}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 285);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp48, tmp49, tmp56, tmp58);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 280);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 290);
    compiler::TNode<Smi> tmp70;
    USE(tmp70);
    tmp70 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp60, tmp61, tmp68, tmp70);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 280);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 294);
    compiler::TNode<Number> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp78}, compiler::TNode<Number>{tmp82}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 269);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 263);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 261);
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

compiler::TNode<Object> ArraySpliceBuiltinsFromDSLAssembler::SlowSplice(compiler::TNode<Context> p_context, CodeStubArguments* p_arguments, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Number> p_actualStart, compiler::TNode<Smi> p_insertCount, compiler::TNode<Number> p_actualDeleteCount) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, IntPtrT, IntPtrT> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, IntPtrT, IntPtrT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, IntPtrT, IntPtrT> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number, IntPtrT, IntPtrT> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Number, JSReceiver, Number, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSReceiver, Number, Number, Smi, Number, Object> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_len, p_actualStart, p_insertCount, p_actualDeleteCount);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSReceiver> tmp1;
    compiler::TNode<Number> tmp2;
    compiler::TNode<Number> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Number> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 302);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp2}, compiler::TNode<Number>{tmp3}));
    compiler::TNode<Number> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp6}, compiler::TNode<Number>{tmp5}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 305);
    compiler::TNode<JSReceiver> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ArraySpeciesCreate(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, compiler::TNode<Number>{tmp5}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 306);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 309);
    compiler::TNode<Object> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Object>(ArraySpliceBuiltinsFromDSLAssembler(state_).FillDeletedElementsArray(compiler::TNode<Context>{tmp0}, compiler::TNode<JSReceiver>{tmp1}, compiler::TNode<Number>{tmp3}, compiler::TNode<Number>{tmp5}, compiler::TNode<JSReceiver>{tmp8}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 319);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp4}, compiler::TNode<Number>{tmp5}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp7, tmp8, tmp4, tmp4, tmp5);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp7, tmp8, tmp4, tmp4, tmp5);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp10;
    compiler::TNode<JSReceiver> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Number> tmp15;
    compiler::TNode<Number> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Number> tmp19;
    compiler::TNode<Number> tmp20;
    ca_.Bind(&block4, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.Goto(&block2, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<JSReceiver> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Number> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Number> tmp27;
    compiler::TNode<JSReceiver> tmp28;
    compiler::TNode<Number> tmp29;
    compiler::TNode<Number> tmp30;
    compiler::TNode<Number> tmp31;
    ca_.Bind(&block5, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block3, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<Number> tmp34;
    compiler::TNode<Number> tmp35;
    compiler::TNode<Smi> tmp36;
    compiler::TNode<Number> tmp37;
    compiler::TNode<Number> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Number> tmp40;
    ca_.Bind(&block2, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 321);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 320);
    ArraySpliceBuiltinsFromDSLAssembler(state_).HandleForwardCase(compiler::TNode<Context>{tmp32}, compiler::TNode<JSReceiver>{tmp33}, compiler::TNode<Number>{tmp34}, compiler::TNode<Number>{tmp40}, compiler::TNode<Number>{tmp35}, compiler::TNode<Number>{tmp37});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 319);
    ca_.Goto(&block6, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<JSReceiver> tmp42;
    compiler::TNode<Number> tmp43;
    compiler::TNode<Number> tmp44;
    compiler::TNode<Smi> tmp45;
    compiler::TNode<Number> tmp46;
    compiler::TNode<Number> tmp47;
    compiler::TNode<JSReceiver> tmp48;
    compiler::TNode<Number> tmp49;
    ca_.Bind(&block3, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 323);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp49}, compiler::TNode<Number>{tmp46}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block9, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp49, tmp46);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block10, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp49, tmp46);
    }
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp50;
    compiler::TNode<JSReceiver> tmp51;
    compiler::TNode<Number> tmp52;
    compiler::TNode<Number> tmp53;
    compiler::TNode<Smi> tmp54;
    compiler::TNode<Number> tmp55;
    compiler::TNode<Number> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<Number> tmp59;
    compiler::TNode<Number> tmp60;
    ca_.Bind(&block9, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.Goto(&block7, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<JSReceiver> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Number> tmp64;
    compiler::TNode<Smi> tmp65;
    compiler::TNode<Number> tmp66;
    compiler::TNode<Number> tmp67;
    compiler::TNode<JSReceiver> tmp68;
    compiler::TNode<Number> tmp69;
    compiler::TNode<Number> tmp70;
    compiler::TNode<Number> tmp71;
    ca_.Bind(&block10, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    ca_.Goto(&block8, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<JSReceiver> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Smi> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<JSReceiver> tmp79;
    compiler::TNode<Number> tmp80;
    ca_.Bind(&block7, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 325);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 324);
    ArraySpliceBuiltinsFromDSLAssembler(state_).HandleBackwardCase(compiler::TNode<Context>{tmp72}, compiler::TNode<JSReceiver>{tmp73}, compiler::TNode<Number>{tmp74}, compiler::TNode<Number>{tmp80}, compiler::TNode<Number>{tmp75}, compiler::TNode<Number>{tmp77});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 323);
    ca_.Goto(&block8, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp81;
    compiler::TNode<JSReceiver> tmp82;
    compiler::TNode<Number> tmp83;
    compiler::TNode<Number> tmp84;
    compiler::TNode<Smi> tmp85;
    compiler::TNode<Number> tmp86;
    compiler::TNode<Number> tmp87;
    compiler::TNode<JSReceiver> tmp88;
    compiler::TNode<Number> tmp89;
    ca_.Bind(&block8, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 319);
    ca_.Goto(&block6, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<Number> tmp93;
    compiler::TNode<Smi> tmp94;
    compiler::TNode<Number> tmp95;
    compiler::TNode<Number> tmp96;
    compiler::TNode<JSReceiver> tmp97;
    compiler::TNode<Number> tmp98;
    ca_.Bind(&block6, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 329);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 334);
    compiler::TNode<IntPtrT> tmp99;
    USE(tmp99);
    tmp99 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(p_arguments));
    compiler::TNode<IntPtrT> tmp100;
    USE(tmp100);
    tmp100 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp101;
    USE(tmp101);
    tmp101 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp99}, compiler::TNode<IntPtrT>{tmp100}));
    ca_.Branch(tmp101, &block11, &block12, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp93);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp102;
    compiler::TNode<JSReceiver> tmp103;
    compiler::TNode<Number> tmp104;
    compiler::TNode<Number> tmp105;
    compiler::TNode<Smi> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<Number> tmp108;
    compiler::TNode<JSReceiver> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Number> tmp111;
    ca_.Bind(&block11, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 335);
    compiler::TNode<IntPtrT> tmp112;
    USE(tmp112);
    tmp112 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(p_arguments));
    compiler::TNode<IntPtrT> tmp113;
    USE(tmp113);
    tmp113 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    ca_.Goto(&block16, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp114;
    compiler::TNode<JSReceiver> tmp115;
    compiler::TNode<Number> tmp116;
    compiler::TNode<Number> tmp117;
    compiler::TNode<Smi> tmp118;
    compiler::TNode<Number> tmp119;
    compiler::TNode<Number> tmp120;
    compiler::TNode<JSReceiver> tmp121;
    compiler::TNode<Number> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<IntPtrT> tmp124;
    compiler::TNode<IntPtrT> tmp125;
    ca_.Bind(&block16, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125);
    compiler::TNode<BoolT> tmp126;
    USE(tmp126);
    tmp126 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp125}, compiler::TNode<IntPtrT>{tmp124}));
    ca_.Branch(tmp126, &block13, &block15, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp127;
    compiler::TNode<JSReceiver> tmp128;
    compiler::TNode<Number> tmp129;
    compiler::TNode<Number> tmp130;
    compiler::TNode<Smi> tmp131;
    compiler::TNode<Number> tmp132;
    compiler::TNode<Number> tmp133;
    compiler::TNode<JSReceiver> tmp134;
    compiler::TNode<Number> tmp135;
    compiler::TNode<Number> tmp136;
    compiler::TNode<IntPtrT> tmp137;
    compiler::TNode<IntPtrT> tmp138;
    ca_.Bind(&block13, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138);
    compiler::TNode<Object> tmp139;
    USE(tmp139);
    tmp139 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(p_arguments, compiler::TNode<IntPtrT>{tmp138}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 337);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp127, tmp128, tmp136, tmp139);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 340);
    compiler::TNode<Number> tmp141;
    USE(tmp141);
    tmp141 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp142;
    USE(tmp142);
    tmp142 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp136}, compiler::TNode<Number>{tmp141}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 335);
    ca_.Goto(&block14, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp142, tmp137, tmp138);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp143;
    compiler::TNode<JSReceiver> tmp144;
    compiler::TNode<Number> tmp145;
    compiler::TNode<Number> tmp146;
    compiler::TNode<Smi> tmp147;
    compiler::TNode<Number> tmp148;
    compiler::TNode<Number> tmp149;
    compiler::TNode<JSReceiver> tmp150;
    compiler::TNode<Number> tmp151;
    compiler::TNode<Number> tmp152;
    compiler::TNode<IntPtrT> tmp153;
    compiler::TNode<IntPtrT> tmp154;
    ca_.Bind(&block14, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154);
    compiler::TNode<IntPtrT> tmp155;
    USE(tmp155);
    tmp155 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp156;
    USE(tmp156);
    tmp156 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp154}, compiler::TNode<IntPtrT>{tmp155}));
    ca_.Goto(&block16, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp156);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp157;
    compiler::TNode<JSReceiver> tmp158;
    compiler::TNode<Number> tmp159;
    compiler::TNode<Number> tmp160;
    compiler::TNode<Smi> tmp161;
    compiler::TNode<Number> tmp162;
    compiler::TNode<Number> tmp163;
    compiler::TNode<JSReceiver> tmp164;
    compiler::TNode<Number> tmp165;
    compiler::TNode<Number> tmp166;
    compiler::TNode<IntPtrT> tmp167;
    compiler::TNode<IntPtrT> tmp168;
    ca_.Bind(&block15, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 334);
    ca_.Goto(&block12, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp169;
    compiler::TNode<JSReceiver> tmp170;
    compiler::TNode<Number> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<Smi> tmp173;
    compiler::TNode<Number> tmp174;
    compiler::TNode<Number> tmp175;
    compiler::TNode<JSReceiver> tmp176;
    compiler::TNode<Number> tmp177;
    compiler::TNode<Number> tmp178;
    ca_.Bind(&block12, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 346);
    compiler::TNode<String> tmp179;
    USE(tmp179);
    tmp179 = BaseBuiltinsFromDSLAssembler(state_).kLengthString();
    compiler::TNode<Number> tmp180;
    USE(tmp180);
    tmp180 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp171}, compiler::TNode<Number>{tmp174}));
    compiler::TNode<Number> tmp181;
    USE(tmp181);
    tmp181 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp180}, compiler::TNode<Number>{tmp177}));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp169, tmp170, tmp179, tmp181);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 348);
    ca_.Goto(&block1, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp176);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp183;
    compiler::TNode<JSReceiver> tmp184;
    compiler::TNode<Number> tmp185;
    compiler::TNode<Number> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<Number> tmp188;
    compiler::TNode<Object> tmp189;
    ca_.Bind(&block1, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 298);
    ca_.Goto(&block17, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189);
  }

    compiler::TNode<Context> tmp190;
    compiler::TNode<JSReceiver> tmp191;
    compiler::TNode<Number> tmp192;
    compiler::TNode<Number> tmp193;
    compiler::TNode<Smi> tmp194;
    compiler::TNode<Number> tmp195;
    compiler::TNode<Object> tmp196;
    ca_.Bind(&block17, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196);
  return compiler::TNode<Object>{tmp196};
}

TF_BUILTIN(ArrayPrototypeSplice, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number, Number, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number, Number, Number> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number, Number> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number, Number, Context, JSReceiver, Number, Number, Smi, Number> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number, Number, Context, JSReceiver, Number, Number, Smi, Number, Object> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number, Number> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, JSReceiver, Number, Object, Number, Number, Smi, Number, Number> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 356);
    compiler::TNode<JSReceiver> tmp2;
    tmp2 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kToObject, tmp0, tmp1));
    USE(tmp2);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 359);
    compiler::TNode<Number> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 362);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 363);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 368);
    compiler::TNode<Number> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp6}, compiler::TNode<Number>{tmp7}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp5, tmp6, tmp6);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block6, tmp0, tmp1, tmp2, tmp3, tmp5, tmp6, tmp6);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<JSReceiver> tmp10;
    compiler::TNode<Number> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<Number> tmp13;
    compiler::TNode<Number> tmp14;
    ca_.Bind(&block5, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.Goto(&block1, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<JSReceiver> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Number> tmp21;
    ca_.Bind(&block6, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21);
    ca_.Goto(&block2, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Number> tmp27;
    ca_.Bind(&block1, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 369);
    compiler::TNode<Number> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp25}, compiler::TNode<Number>{tmp27}));
    compiler::TNode<Number> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp28}, compiler::TNode<Number>{tmp29}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 368);
    ca_.Goto(&block4, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp30);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<JSReceiver> tmp33;
    compiler::TNode<Number> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Number> tmp36;
    ca_.Bind(&block2, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 370);
    compiler::TNode<Number> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp36}, compiler::TNode<Number>{tmp34}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 368);
    ca_.Goto(&block3, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<Object> tmp39;
    compiler::TNode<JSReceiver> tmp40;
    compiler::TNode<Number> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<Number> tmp43;
    compiler::TNode<Number> tmp44;
    ca_.Bind(&block4, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.Goto(&block3, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<JSReceiver> tmp47;
    compiler::TNode<Number> tmp48;
    compiler::TNode<Object> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<Number> tmp51;
    ca_.Bind(&block3, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 372);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 373);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 375);
    compiler::TNode<IntPtrT> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(arguments));
    compiler::TNode<IntPtrT> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp52}, compiler::TNode<IntPtrT>{tmp53}));
    ca_.Branch(tmp54, &block7, &block8, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, ca_.Uninitialized<Smi>(), ca_.Uninitialized<Number>());
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<JSReceiver> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<Number> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Smi> tmp62;
    compiler::TNode<Number> tmp63;
    ca_.Bind(&block7, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 377);
    compiler::TNode<Smi> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 379);
    compiler::TNode<Number> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 375);
    ca_.Goto(&block9, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp64, tmp65);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<JSReceiver> tmp68;
    compiler::TNode<Number> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Number> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<Number> tmp74;
    ca_.Bind(&block8, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 381);
    compiler::TNode<IntPtrT> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(arguments));
    compiler::TNode<IntPtrT> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp75}, compiler::TNode<IntPtrT>{tmp76}));
    ca_.Branch(tmp77, &block10, &block11, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<JSReceiver> tmp80;
    compiler::TNode<Number> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Number> tmp83;
    compiler::TNode<Number> tmp84;
    compiler::TNode<Smi> tmp85;
    compiler::TNode<Number> tmp86;
    ca_.Bind(&block10, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 383);
    compiler::TNode<Smi> tmp87;
    USE(tmp87);
    tmp87 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 385);
    compiler::TNode<Number> tmp88;
    USE(tmp88);
    tmp88 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp81}, compiler::TNode<Number>{tmp84}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 381);
    ca_.Goto(&block12, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp87, tmp88);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<JSReceiver> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<Object> tmp93;
    compiler::TNode<Number> tmp94;
    compiler::TNode<Number> tmp95;
    compiler::TNode<Smi> tmp96;
    compiler::TNode<Number> tmp97;
    ca_.Bind(&block11, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 389);
    compiler::TNode<IntPtrT> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(arguments));
    compiler::TNode<Smi> tmp99;
    USE(tmp99);
    tmp99 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATintptr(compiler::TNode<IntPtrT>{tmp98}));
    compiler::TNode<Smi> tmp100;
    USE(tmp100);
    tmp100 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(2));
    compiler::TNode<Smi> tmp101;
    USE(tmp101);
    tmp101 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp99}, compiler::TNode<Smi>{tmp100}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 391);
    compiler::TNode<IntPtrT> tmp102;
    USE(tmp102);
    tmp102 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp103;
    USE(tmp103);
    tmp103 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp102}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 392);
    compiler::TNode<Number> tmp104;
    USE(tmp104);
    tmp104 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp89}, compiler::TNode<Object>{tmp103}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 394);
    compiler::TNode<Number> tmp105;
    USE(tmp105);
    tmp105 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp106;
    USE(tmp106);
    tmp106 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp104}, compiler::TNode<Number>{tmp105}));
    compiler::TNode<Number> tmp107;
    USE(tmp107);
    tmp107 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp92}, compiler::TNode<Number>{tmp95}));
    compiler::TNode<Number> tmp108;
    USE(tmp108);
    tmp108 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp106}, compiler::TNode<Number>{tmp107}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 387);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 381);
    ca_.Goto(&block12, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp101, tmp108);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<JSReceiver> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Object> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<Number> tmp115;
    compiler::TNode<Smi> tmp116;
    compiler::TNode<Number> tmp117;
    ca_.Bind(&block12, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 375);
    ca_.Goto(&block9, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<JSReceiver> tmp120;
    compiler::TNode<Number> tmp121;
    compiler::TNode<Object> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Smi> tmp125;
    compiler::TNode<Number> tmp126;
    ca_.Bind(&block9, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 399);
    compiler::TNode<Number> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp121}, compiler::TNode<Number>{tmp125}));
    compiler::TNode<Number> tmp128;
    USE(tmp128);
    tmp128 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp127}, compiler::TNode<Number>{tmp126}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 400);
    compiler::TNode<Number> tmp129;
    USE(tmp129);
    tmp129 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber19ATconstexpr_float64(kMaxSafeInteger));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp128}, compiler::TNode<Number>{tmp129}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block15, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp128, tmp128);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block16, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp128, tmp128);
    }
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<JSReceiver> tmp132;
    compiler::TNode<Number> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Number> tmp135;
    compiler::TNode<Number> tmp136;
    compiler::TNode<Smi> tmp137;
    compiler::TNode<Number> tmp138;
    compiler::TNode<Number> tmp139;
    compiler::TNode<Number> tmp140;
    ca_.Bind(&block15, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140);
    ca_.Goto(&block13, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<JSReceiver> tmp143;
    compiler::TNode<Number> tmp144;
    compiler::TNode<Object> tmp145;
    compiler::TNode<Number> tmp146;
    compiler::TNode<Number> tmp147;
    compiler::TNode<Smi> tmp148;
    compiler::TNode<Number> tmp149;
    compiler::TNode<Number> tmp150;
    compiler::TNode<Number> tmp151;
    ca_.Bind(&block16, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151);
    ca_.Goto(&block14, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<JSReceiver> tmp154;
    compiler::TNode<Number> tmp155;
    compiler::TNode<Object> tmp156;
    compiler::TNode<Number> tmp157;
    compiler::TNode<Number> tmp158;
    compiler::TNode<Smi> tmp159;
    compiler::TNode<Number> tmp160;
    compiler::TNode<Number> tmp161;
    ca_.Bind(&block13, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 401);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp152}, MessageTemplate::kInvalidArrayLength, compiler::TNode<Object>{tmp156});
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp162;
    compiler::TNode<Object> tmp163;
    compiler::TNode<JSReceiver> tmp164;
    compiler::TNode<Number> tmp165;
    compiler::TNode<Object> tmp166;
    compiler::TNode<Number> tmp167;
    compiler::TNode<Number> tmp168;
    compiler::TNode<Smi> tmp169;
    compiler::TNode<Number> tmp170;
    compiler::TNode<Number> tmp171;
    ca_.Bind(&block14, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 406);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 407);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 405);
    compiler::TNode<Object> tmp172;
    USE(tmp172);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp172 = ArraySpliceBuiltinsFromDSLAssembler(state_).FastArraySplice(compiler::TNode<Context>{tmp162}, arguments, compiler::TNode<JSReceiver>{tmp164}, compiler::TNode<Number>{tmp165}, compiler::TNode<Number>{tmp168}, compiler::TNode<Smi>{tmp169}, compiler::TNode<Number>{tmp170}, &label0);
    ca_.Goto(&block19, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp162, tmp164, tmp165, tmp168, tmp169, tmp170, tmp172);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block20, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171, tmp162, tmp164, tmp165, tmp168, tmp169, tmp170);
    }
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp173;
    compiler::TNode<Object> tmp174;
    compiler::TNode<JSReceiver> tmp175;
    compiler::TNode<Number> tmp176;
    compiler::TNode<Object> tmp177;
    compiler::TNode<Number> tmp178;
    compiler::TNode<Number> tmp179;
    compiler::TNode<Smi> tmp180;
    compiler::TNode<Number> tmp181;
    compiler::TNode<Number> tmp182;
    compiler::TNode<Context> tmp183;
    compiler::TNode<JSReceiver> tmp184;
    compiler::TNode<Number> tmp185;
    compiler::TNode<Number> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<Number> tmp188;
    ca_.Bind(&block20, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188);
    ca_.Goto(&block18, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp189;
    compiler::TNode<Object> tmp190;
    compiler::TNode<JSReceiver> tmp191;
    compiler::TNode<Number> tmp192;
    compiler::TNode<Object> tmp193;
    compiler::TNode<Number> tmp194;
    compiler::TNode<Number> tmp195;
    compiler::TNode<Smi> tmp196;
    compiler::TNode<Number> tmp197;
    compiler::TNode<Number> tmp198;
    compiler::TNode<Context> tmp199;
    compiler::TNode<JSReceiver> tmp200;
    compiler::TNode<Number> tmp201;
    compiler::TNode<Number> tmp202;
    compiler::TNode<Smi> tmp203;
    compiler::TNode<Number> tmp204;
    compiler::TNode<Object> tmp205;
    ca_.Bind(&block19, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205);
    arguments->PopAndReturn(tmp205);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp206;
    compiler::TNode<Object> tmp207;
    compiler::TNode<JSReceiver> tmp208;
    compiler::TNode<Number> tmp209;
    compiler::TNode<Object> tmp210;
    compiler::TNode<Number> tmp211;
    compiler::TNode<Number> tmp212;
    compiler::TNode<Smi> tmp213;
    compiler::TNode<Number> tmp214;
    compiler::TNode<Number> tmp215;
    ca_.Bind(&block18, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 404);
    ca_.Goto(&block17, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp216;
    compiler::TNode<Object> tmp217;
    compiler::TNode<JSReceiver> tmp218;
    compiler::TNode<Number> tmp219;
    compiler::TNode<Object> tmp220;
    compiler::TNode<Number> tmp221;
    compiler::TNode<Number> tmp222;
    compiler::TNode<Smi> tmp223;
    compiler::TNode<Number> tmp224;
    compiler::TNode<Number> tmp225;
    ca_.Bind(&block17, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 414);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 415);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 413);
    compiler::TNode<Object> tmp226;
    USE(tmp226);
    tmp226 = ca_.UncheckedCast<Object>(ArraySpliceBuiltinsFromDSLAssembler(state_).SlowSplice(compiler::TNode<Context>{tmp216}, arguments, compiler::TNode<JSReceiver>{tmp218}, compiler::TNode<Number>{tmp219}, compiler::TNode<Number>{tmp222}, compiler::TNode<Smi>{tmp223}, compiler::TNode<Number>{tmp224}));
    arguments->PopAndReturn(tmp226);
  }
}

void ArraySpliceBuiltinsFromDSLAssembler::FastSplice10FixedArray20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, CodeStubArguments* p_args, compiler::TNode<JSArray> p_a, compiler::TNode<Smi> p_length, compiler::TNode<Smi> p_newLength, compiler::TNode<Smi> p_lengthDelta, compiler::TNode<Smi> p_actualStart, compiler::TNode<Smi> p_insertCount, compiler::TNode<Smi> p_actualDeleteCount, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi, Smi, FixedArray> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi, Smi, FixedArray> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedArray, IntPtrT, IntPtrT> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedArray, IntPtrT, IntPtrT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedArray, IntPtrT, IntPtrT> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedArray, IntPtrT, IntPtrT> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_a, p_length, p_newLength, p_lengthDelta, p_actualStart, p_insertCount, p_actualDeleteCount);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Smi> tmp5;
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 60);
    ArrayBuiltinsFromDSLAssembler(state_).EnsureWriteableFastElements(compiler::TNode<Context>{tmp0}, compiler::TNode<JSArray>{tmp1});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 62);
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiNotEqual(compiler::TNode<Smi>{tmp6}, compiler::TNode<Smi>{tmp7}));
    ca_.Branch(tmp8, &block3, &block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSArray> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Smi> tmp15;
    compiler::TNode<Smi> tmp16;
    ca_.Bind(&block3, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 63);
    compiler::TNode<FixedArrayBase> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp10}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 64);
    compiler::TNode<Smi> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp14}, compiler::TNode<Smi>{tmp15}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 65);
    compiler::TNode<Smi> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp14}, compiler::TNode<Smi>{tmp16}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 66);
    compiler::TNode<Smi> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp11}, compiler::TNode<Smi>{tmp16}));
    compiler::TNode<Smi> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp20}, compiler::TNode<Smi>{tmp14}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 67);
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp15}, compiler::TNode<Smi>{tmp16}));
    ca_.Branch(tmp22, &block5, &block6, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp21);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSArray> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<Smi> tmp28;
    compiler::TNode<Smi> tmp29;
    compiler::TNode<Smi> tmp30;
    compiler::TNode<FixedArrayBase> tmp31;
    compiler::TNode<Smi> tmp32;
    compiler::TNode<Smi> tmp33;
    compiler::TNode<Smi> tmp34;
    ca_.Bind(&block5, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 70);
    compiler::TNode<FixedArray> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp23}, compiler::TNode<Object>{tmp31}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 69);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoMoveElements10FixedArray(compiler::TNode<FixedArray>{tmp35}, compiler::TNode<Smi>{tmp32}, compiler::TNode<Smi>{tmp33}, compiler::TNode<Smi>{tmp34});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 72);
    compiler::TNode<FixedArray> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp23}, compiler::TNode<Object>{tmp31}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 71);
    ArraySpliceBuiltinsFromDSLAssembler(state_).StoreHoles10FixedArray(compiler::TNode<FixedArray>{tmp36}, compiler::TNode<Smi>{tmp26}, compiler::TNode<Smi>{tmp25});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 67);
    ca_.Goto(&block7, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<JSArray> tmp38;
    compiler::TNode<Smi> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<Smi> tmp43;
    compiler::TNode<Smi> tmp44;
    compiler::TNode<FixedArrayBase> tmp45;
    compiler::TNode<Smi> tmp46;
    compiler::TNode<Smi> tmp47;
    compiler::TNode<Smi> tmp48;
    ca_.Bind(&block6, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 73);
    compiler::TNode<BoolT> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp43}, compiler::TNode<Smi>{tmp44}));
    ca_.Branch(tmp49, &block8, &block9, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp50;
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
    ca_.Bind(&block8, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 75);
    compiler::TNode<Smi> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp58}));
    compiler::TNode<BoolT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThanOrEqual(compiler::TNode<Smi>{tmp53}, compiler::TNode<Smi>{tmp62}));
    ca_.Branch(tmp63, &block10, &block11, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<JSArray> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<Smi> tmp67;
    compiler::TNode<Smi> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<FixedArrayBase> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<Smi> tmp74;
    compiler::TNode<Smi> tmp75;
    ca_.Bind(&block10, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 77);
    compiler::TNode<FixedArray> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp64}, compiler::TNode<Object>{tmp72}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 76);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoMoveElements10FixedArray(compiler::TNode<FixedArray>{tmp76}, compiler::TNode<Smi>{tmp73}, compiler::TNode<Smi>{tmp74}, compiler::TNode<Smi>{tmp75});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 75);
    ca_.Goto(&block12, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp77;
    compiler::TNode<JSArray> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<Smi> tmp80;
    compiler::TNode<Smi> tmp81;
    compiler::TNode<Smi> tmp82;
    compiler::TNode<Smi> tmp83;
    compiler::TNode<Smi> tmp84;
    compiler::TNode<FixedArrayBase> tmp85;
    compiler::TNode<Smi> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<Smi> tmp88;
    ca_.Bind(&block11, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 80);
    compiler::TNode<Smi> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<Smi>{tmp80}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 82);
    compiler::TNode<Smi> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<FixedArray> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<FixedArray>(ArraySpliceBuiltinsFromDSLAssembler(state_).Extract10FixedArray(compiler::TNode<Context>{tmp77}, compiler::TNode<FixedArrayBase>{tmp85}, compiler::TNode<Smi>{tmp90}, compiler::TNode<Smi>{tmp82}, compiler::TNode<Smi>{tmp89}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 81);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 83);
    BaseBuiltinsFromDSLAssembler(state_).StoreJSObjectElements(compiler::TNode<JSObject>{tmp78}, compiler::TNode<FixedArrayBase>{tmp91});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 84);
    compiler::TNode<Smi> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp85}));
    compiler::TNode<Smi> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp92}, compiler::TNode<Smi>{tmp93}));
    ca_.Branch(tmp94, &block13, &block14, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp91);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp95;
    compiler::TNode<JSArray> tmp96;
    compiler::TNode<Smi> tmp97;
    compiler::TNode<Smi> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<Smi> tmp100;
    compiler::TNode<Smi> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<FixedArrayBase> tmp103;
    compiler::TNode<Smi> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Smi> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<FixedArray> tmp108;
    ca_.Bind(&block13, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 86);
    compiler::TNode<FixedArray> tmp109;
    USE(tmp109);
    tmp109 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp95}, compiler::TNode<Object>{tmp103}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 87);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 85);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoCopyElements10FixedArray(compiler::TNode<FixedArray>{tmp108}, compiler::TNode<Smi>{tmp104}, compiler::TNode<FixedArray>{tmp109}, compiler::TNode<Smi>{tmp105}, compiler::TNode<Smi>{tmp106});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 84);
    ca_.Goto(&block14, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<JSArray> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<Smi> tmp113;
    compiler::TNode<Smi> tmp114;
    compiler::TNode<Smi> tmp115;
    compiler::TNode<Smi> tmp116;
    compiler::TNode<Smi> tmp117;
    compiler::TNode<FixedArrayBase> tmp118;
    compiler::TNode<Smi> tmp119;
    compiler::TNode<Smi> tmp120;
    compiler::TNode<Smi> tmp121;
    compiler::TNode<Smi> tmp122;
    compiler::TNode<FixedArray> tmp123;
    ca_.Bind(&block14, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 78);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 75);
    ca_.Goto(&block12, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp124;
    compiler::TNode<JSArray> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<Smi> tmp130;
    compiler::TNode<Smi> tmp131;
    compiler::TNode<FixedArrayBase> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<Smi> tmp134;
    compiler::TNode<Smi> tmp135;
    ca_.Bind(&block12, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 73);
    ca_.Goto(&block9, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp136;
    compiler::TNode<JSArray> tmp137;
    compiler::TNode<Smi> tmp138;
    compiler::TNode<Smi> tmp139;
    compiler::TNode<Smi> tmp140;
    compiler::TNode<Smi> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<Smi> tmp143;
    compiler::TNode<FixedArrayBase> tmp144;
    compiler::TNode<Smi> tmp145;
    compiler::TNode<Smi> tmp146;
    compiler::TNode<Smi> tmp147;
    ca_.Bind(&block9, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 67);
    ca_.Goto(&block7, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp148;
    compiler::TNode<JSArray> tmp149;
    compiler::TNode<Smi> tmp150;
    compiler::TNode<Smi> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<Smi> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<FixedArrayBase> tmp156;
    compiler::TNode<Smi> tmp157;
    compiler::TNode<Smi> tmp158;
    compiler::TNode<Smi> tmp159;
    ca_.Bind(&block7, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 62);
    ca_.Goto(&block4, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp160;
    compiler::TNode<JSArray> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<Smi> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<Smi> tmp166;
    compiler::TNode<Smi> tmp167;
    ca_.Bind(&block4, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 94);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 95);
    compiler::TNode<Smi> tmp168;
    USE(tmp168);
    tmp168 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp169;
    USE(tmp169);
    tmp169 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp166}, compiler::TNode<Smi>{tmp168}));
    ca_.Branch(tmp169, &block15, &block16, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp165);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<JSArray> tmp171;
    compiler::TNode<Smi> tmp172;
    compiler::TNode<Smi> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<Smi> tmp178;
    ca_.Bind(&block15, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 97);
    compiler::TNode<FixedArrayBase> tmp179;
    USE(tmp179);
    tmp179 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp171}));
    compiler::TNode<FixedArray> tmp180;
    USE(tmp180);
    tmp180 = ca_.UncheckedCast<FixedArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast10FixedArray(compiler::TNode<Context>{tmp170}, compiler::TNode<Object>{tmp179}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 96);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 98);
    compiler::TNode<IntPtrT> tmp181;
    USE(tmp181);
    tmp181 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(p_args));
    compiler::TNode<IntPtrT> tmp182;
    USE(tmp182);
    tmp182 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    ca_.Goto(&block20, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp180, tmp181, tmp182);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp183;
    compiler::TNode<JSArray> tmp184;
    compiler::TNode<Smi> tmp185;
    compiler::TNode<Smi> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<Smi> tmp188;
    compiler::TNode<Smi> tmp189;
    compiler::TNode<Smi> tmp190;
    compiler::TNode<Smi> tmp191;
    compiler::TNode<FixedArray> tmp192;
    compiler::TNode<IntPtrT> tmp193;
    compiler::TNode<IntPtrT> tmp194;
    ca_.Bind(&block20, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194);
    compiler::TNode<BoolT> tmp195;
    USE(tmp195);
    tmp195 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp194}, compiler::TNode<IntPtrT>{tmp193}));
    ca_.Branch(tmp195, &block17, &block19, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp196;
    compiler::TNode<JSArray> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<Smi> tmp199;
    compiler::TNode<Smi> tmp200;
    compiler::TNode<Smi> tmp201;
    compiler::TNode<Smi> tmp202;
    compiler::TNode<Smi> tmp203;
    compiler::TNode<Smi> tmp204;
    compiler::TNode<FixedArray> tmp205;
    compiler::TNode<IntPtrT> tmp206;
    compiler::TNode<IntPtrT> tmp207;
    ca_.Bind(&block17, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207);
    compiler::TNode<Object> tmp208;
    USE(tmp208);
    tmp208 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(p_args, compiler::TNode<IntPtrT>{tmp207}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 101);
    compiler::TNode<Smi> tmp209;
    USE(tmp209);
    tmp209 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp210;
    USE(tmp210);
    tmp210 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp204}, compiler::TNode<Smi>{tmp209}));
    compiler::TNode<Object> tmp211;
    USE(tmp211);
    tmp211 = ca_.UncheckedCast<Object>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast20UT5ATSmi10HeapObject(compiler::TNode<Object>{tmp208}));
    BaseBuiltinsFromDSLAssembler(state_).StoreFixedArrayDirect(compiler::TNode<FixedArray>{tmp205}, compiler::TNode<Smi>{tmp204}, compiler::TNode<Object>{tmp211});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 98);
    ca_.Goto(&block18, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp210, tmp205, tmp206, tmp207);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp212;
    compiler::TNode<JSArray> tmp213;
    compiler::TNode<Smi> tmp214;
    compiler::TNode<Smi> tmp215;
    compiler::TNode<Smi> tmp216;
    compiler::TNode<Smi> tmp217;
    compiler::TNode<Smi> tmp218;
    compiler::TNode<Smi> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<FixedArray> tmp221;
    compiler::TNode<IntPtrT> tmp222;
    compiler::TNode<IntPtrT> tmp223;
    ca_.Bind(&block18, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223);
    compiler::TNode<IntPtrT> tmp224;
    USE(tmp224);
    tmp224 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp225;
    USE(tmp225);
    tmp225 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp223}, compiler::TNode<IntPtrT>{tmp224}));
    ca_.Goto(&block20, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp225);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp226;
    compiler::TNode<JSArray> tmp227;
    compiler::TNode<Smi> tmp228;
    compiler::TNode<Smi> tmp229;
    compiler::TNode<Smi> tmp230;
    compiler::TNode<Smi> tmp231;
    compiler::TNode<Smi> tmp232;
    compiler::TNode<Smi> tmp233;
    compiler::TNode<Smi> tmp234;
    compiler::TNode<FixedArray> tmp235;
    compiler::TNode<IntPtrT> tmp236;
    compiler::TNode<IntPtrT> tmp237;
    ca_.Bind(&block19, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 95);
    ca_.Goto(&block16, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp238;
    compiler::TNode<JSArray> tmp239;
    compiler::TNode<Smi> tmp240;
    compiler::TNode<Smi> tmp241;
    compiler::TNode<Smi> tmp242;
    compiler::TNode<Smi> tmp243;
    compiler::TNode<Smi> tmp244;
    compiler::TNode<Smi> tmp245;
    compiler::TNode<Smi> tmp246;
    ca_.Bind(&block16, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 106);
    BaseBuiltinsFromDSLAssembler(state_).StoreJSArrayLength(compiler::TNode<JSArray>{tmp239}, compiler::TNode<Number>{tmp241});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 58);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 54);
    ca_.Goto(&block2, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp247;
    compiler::TNode<JSArray> tmp248;
    compiler::TNode<Smi> tmp249;
    compiler::TNode<Smi> tmp250;
    compiler::TNode<Smi> tmp251;
    compiler::TNode<Smi> tmp252;
    compiler::TNode<Smi> tmp253;
    compiler::TNode<Smi> tmp254;
    ca_.Bind(&block2, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254);
    ca_.Goto(&block21, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp255;
    compiler::TNode<JSArray> tmp256;
    compiler::TNode<Smi> tmp257;
    compiler::TNode<Smi> tmp258;
    compiler::TNode<Smi> tmp259;
    compiler::TNode<Smi> tmp260;
    compiler::TNode<Smi> tmp261;
    compiler::TNode<Smi> tmp262;
    ca_.Bind(&block21, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262);
}

void ArraySpliceBuiltinsFromDSLAssembler::FastSplice16FixedDoubleArray20UT5ATSmi10HeapNumber(compiler::TNode<Context> p_context, CodeStubArguments* p_args, compiler::TNode<JSArray> p_a, compiler::TNode<Smi> p_length, compiler::TNode<Smi> p_newLength, compiler::TNode<Smi> p_lengthDelta, compiler::TNode<Smi> p_actualStart, compiler::TNode<Smi> p_insertCount, compiler::TNode<Smi> p_actualDeleteCount, compiler::CodeAssemblerLabel* label_Bailout) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi, Smi, FixedDoubleArray> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi, Smi, FixedDoubleArray> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, FixedArrayBase, Smi, Smi, Smi> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedDoubleArray, IntPtrT, IntPtrT> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedDoubleArray, IntPtrT, IntPtrT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedDoubleArray, IntPtrT, IntPtrT> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi, FixedDoubleArray, IntPtrT, IntPtrT> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSArray, Smi, Smi, Smi, Smi, Smi, Smi> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_a, p_length, p_newLength, p_lengthDelta, p_actualStart, p_insertCount, p_actualDeleteCount);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSArray> tmp1;
    compiler::TNode<Smi> tmp2;
    compiler::TNode<Smi> tmp3;
    compiler::TNode<Smi> tmp4;
    compiler::TNode<Smi> tmp5;
    compiler::TNode<Smi> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 60);
    ArrayBuiltinsFromDSLAssembler(state_).EnsureWriteableFastElements(compiler::TNode<Context>{tmp0}, compiler::TNode<JSArray>{tmp1});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 62);
    compiler::TNode<BoolT> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiNotEqual(compiler::TNode<Smi>{tmp6}, compiler::TNode<Smi>{tmp7}));
    ca_.Branch(tmp8, &block3, &block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp9;
    compiler::TNode<JSArray> tmp10;
    compiler::TNode<Smi> tmp11;
    compiler::TNode<Smi> tmp12;
    compiler::TNode<Smi> tmp13;
    compiler::TNode<Smi> tmp14;
    compiler::TNode<Smi> tmp15;
    compiler::TNode<Smi> tmp16;
    ca_.Bind(&block3, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 63);
    compiler::TNode<FixedArrayBase> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp10}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 64);
    compiler::TNode<Smi> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp14}, compiler::TNode<Smi>{tmp15}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 65);
    compiler::TNode<Smi> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp14}, compiler::TNode<Smi>{tmp16}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 66);
    compiler::TNode<Smi> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp11}, compiler::TNode<Smi>{tmp16}));
    compiler::TNode<Smi> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiSub(compiler::TNode<Smi>{tmp20}, compiler::TNode<Smi>{tmp14}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 67);
    compiler::TNode<BoolT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp15}, compiler::TNode<Smi>{tmp16}));
    ca_.Branch(tmp22, &block5, &block6, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19, tmp21);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp23;
    compiler::TNode<JSArray> tmp24;
    compiler::TNode<Smi> tmp25;
    compiler::TNode<Smi> tmp26;
    compiler::TNode<Smi> tmp27;
    compiler::TNode<Smi> tmp28;
    compiler::TNode<Smi> tmp29;
    compiler::TNode<Smi> tmp30;
    compiler::TNode<FixedArrayBase> tmp31;
    compiler::TNode<Smi> tmp32;
    compiler::TNode<Smi> tmp33;
    compiler::TNode<Smi> tmp34;
    ca_.Bind(&block5, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 70);
    compiler::TNode<FixedDoubleArray> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp23}, compiler::TNode<Object>{tmp31}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 69);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoMoveElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray>{tmp35}, compiler::TNode<Smi>{tmp32}, compiler::TNode<Smi>{tmp33}, compiler::TNode<Smi>{tmp34});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 72);
    compiler::TNode<FixedDoubleArray> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp23}, compiler::TNode<Object>{tmp31}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 71);
    ArraySpliceBuiltinsFromDSLAssembler(state_).StoreHoles16FixedDoubleArray(compiler::TNode<FixedDoubleArray>{tmp36}, compiler::TNode<Smi>{tmp26}, compiler::TNode<Smi>{tmp25});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 67);
    ca_.Goto(&block7, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp37;
    compiler::TNode<JSArray> tmp38;
    compiler::TNode<Smi> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<Smi> tmp41;
    compiler::TNode<Smi> tmp42;
    compiler::TNode<Smi> tmp43;
    compiler::TNode<Smi> tmp44;
    compiler::TNode<FixedArrayBase> tmp45;
    compiler::TNode<Smi> tmp46;
    compiler::TNode<Smi> tmp47;
    compiler::TNode<Smi> tmp48;
    ca_.Bind(&block6, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 73);
    compiler::TNode<BoolT> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp43}, compiler::TNode<Smi>{tmp44}));
    ca_.Branch(tmp49, &block8, &block9, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp50;
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
    ca_.Bind(&block8, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 75);
    compiler::TNode<Smi> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp58}));
    compiler::TNode<BoolT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThanOrEqual(compiler::TNode<Smi>{tmp53}, compiler::TNode<Smi>{tmp62}));
    ca_.Branch(tmp63, &block10, &block11, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp64;
    compiler::TNode<JSArray> tmp65;
    compiler::TNode<Smi> tmp66;
    compiler::TNode<Smi> tmp67;
    compiler::TNode<Smi> tmp68;
    compiler::TNode<Smi> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<Smi> tmp71;
    compiler::TNode<FixedArrayBase> tmp72;
    compiler::TNode<Smi> tmp73;
    compiler::TNode<Smi> tmp74;
    compiler::TNode<Smi> tmp75;
    ca_.Bind(&block10, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 77);
    compiler::TNode<FixedDoubleArray> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp64}, compiler::TNode<Object>{tmp72}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 76);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoMoveElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray>{tmp76}, compiler::TNode<Smi>{tmp73}, compiler::TNode<Smi>{tmp74}, compiler::TNode<Smi>{tmp75});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 75);
    ca_.Goto(&block12, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp77;
    compiler::TNode<JSArray> tmp78;
    compiler::TNode<Smi> tmp79;
    compiler::TNode<Smi> tmp80;
    compiler::TNode<Smi> tmp81;
    compiler::TNode<Smi> tmp82;
    compiler::TNode<Smi> tmp83;
    compiler::TNode<Smi> tmp84;
    compiler::TNode<FixedArrayBase> tmp85;
    compiler::TNode<Smi> tmp86;
    compiler::TNode<Smi> tmp87;
    compiler::TNode<Smi> tmp88;
    ca_.Bind(&block11, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 80);
    compiler::TNode<Smi> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).CalculateNewElementsCapacity(compiler::TNode<Smi>{tmp80}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 82);
    compiler::TNode<Smi> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<FixedDoubleArray> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<FixedDoubleArray>(ArraySpliceBuiltinsFromDSLAssembler(state_).Extract16FixedDoubleArray(compiler::TNode<Context>{tmp77}, compiler::TNode<FixedArrayBase>{tmp85}, compiler::TNode<Smi>{tmp90}, compiler::TNode<Smi>{tmp82}, compiler::TNode<Smi>{tmp89}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 81);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 83);
    BaseBuiltinsFromDSLAssembler(state_).StoreJSObjectElements(compiler::TNode<JSObject>{tmp78}, compiler::TNode<FixedArrayBase>{tmp91});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 84);
    compiler::TNode<Smi> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadFixedArrayBaseLength(compiler::TNode<FixedArrayBase>{tmp85}));
    compiler::TNode<Smi> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp94;
    USE(tmp94);
    tmp94 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp92}, compiler::TNode<Smi>{tmp93}));
    ca_.Branch(tmp94, &block13, &block14, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp91);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp95;
    compiler::TNode<JSArray> tmp96;
    compiler::TNode<Smi> tmp97;
    compiler::TNode<Smi> tmp98;
    compiler::TNode<Smi> tmp99;
    compiler::TNode<Smi> tmp100;
    compiler::TNode<Smi> tmp101;
    compiler::TNode<Smi> tmp102;
    compiler::TNode<FixedArrayBase> tmp103;
    compiler::TNode<Smi> tmp104;
    compiler::TNode<Smi> tmp105;
    compiler::TNode<Smi> tmp106;
    compiler::TNode<Smi> tmp107;
    compiler::TNode<FixedDoubleArray> tmp108;
    ca_.Bind(&block13, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 86);
    compiler::TNode<FixedDoubleArray> tmp109;
    USE(tmp109);
    tmp109 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp95}, compiler::TNode<Object>{tmp103}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 87);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 85);
    ArraySpliceBuiltinsFromDSLAssembler(state_).DoCopyElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray>{tmp108}, compiler::TNode<Smi>{tmp104}, compiler::TNode<FixedDoubleArray>{tmp109}, compiler::TNode<Smi>{tmp105}, compiler::TNode<Smi>{tmp106});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 84);
    ca_.Goto(&block14, tmp95, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp110;
    compiler::TNode<JSArray> tmp111;
    compiler::TNode<Smi> tmp112;
    compiler::TNode<Smi> tmp113;
    compiler::TNode<Smi> tmp114;
    compiler::TNode<Smi> tmp115;
    compiler::TNode<Smi> tmp116;
    compiler::TNode<Smi> tmp117;
    compiler::TNode<FixedArrayBase> tmp118;
    compiler::TNode<Smi> tmp119;
    compiler::TNode<Smi> tmp120;
    compiler::TNode<Smi> tmp121;
    compiler::TNode<Smi> tmp122;
    compiler::TNode<FixedDoubleArray> tmp123;
    ca_.Bind(&block14, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 78);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 75);
    ca_.Goto(&block12, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp124;
    compiler::TNode<JSArray> tmp125;
    compiler::TNode<Smi> tmp126;
    compiler::TNode<Smi> tmp127;
    compiler::TNode<Smi> tmp128;
    compiler::TNode<Smi> tmp129;
    compiler::TNode<Smi> tmp130;
    compiler::TNode<Smi> tmp131;
    compiler::TNode<FixedArrayBase> tmp132;
    compiler::TNode<Smi> tmp133;
    compiler::TNode<Smi> tmp134;
    compiler::TNode<Smi> tmp135;
    ca_.Bind(&block12, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 73);
    ca_.Goto(&block9, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132, tmp133, tmp134, tmp135);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp136;
    compiler::TNode<JSArray> tmp137;
    compiler::TNode<Smi> tmp138;
    compiler::TNode<Smi> tmp139;
    compiler::TNode<Smi> tmp140;
    compiler::TNode<Smi> tmp141;
    compiler::TNode<Smi> tmp142;
    compiler::TNode<Smi> tmp143;
    compiler::TNode<FixedArrayBase> tmp144;
    compiler::TNode<Smi> tmp145;
    compiler::TNode<Smi> tmp146;
    compiler::TNode<Smi> tmp147;
    ca_.Bind(&block9, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 67);
    ca_.Goto(&block7, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp148;
    compiler::TNode<JSArray> tmp149;
    compiler::TNode<Smi> tmp150;
    compiler::TNode<Smi> tmp151;
    compiler::TNode<Smi> tmp152;
    compiler::TNode<Smi> tmp153;
    compiler::TNode<Smi> tmp154;
    compiler::TNode<Smi> tmp155;
    compiler::TNode<FixedArrayBase> tmp156;
    compiler::TNode<Smi> tmp157;
    compiler::TNode<Smi> tmp158;
    compiler::TNode<Smi> tmp159;
    ca_.Bind(&block7, &tmp148, &tmp149, &tmp150, &tmp151, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 62);
    ca_.Goto(&block4, tmp148, tmp149, tmp150, tmp151, tmp152, tmp153, tmp154, tmp155);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp160;
    compiler::TNode<JSArray> tmp161;
    compiler::TNode<Smi> tmp162;
    compiler::TNode<Smi> tmp163;
    compiler::TNode<Smi> tmp164;
    compiler::TNode<Smi> tmp165;
    compiler::TNode<Smi> tmp166;
    compiler::TNode<Smi> tmp167;
    ca_.Bind(&block4, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 94);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 95);
    compiler::TNode<Smi> tmp168;
    USE(tmp168);
    tmp168 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp169;
    USE(tmp169);
    tmp169 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp166}, compiler::TNode<Smi>{tmp168}));
    ca_.Branch(tmp169, &block15, &block16, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp165);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp170;
    compiler::TNode<JSArray> tmp171;
    compiler::TNode<Smi> tmp172;
    compiler::TNode<Smi> tmp173;
    compiler::TNode<Smi> tmp174;
    compiler::TNode<Smi> tmp175;
    compiler::TNode<Smi> tmp176;
    compiler::TNode<Smi> tmp177;
    compiler::TNode<Smi> tmp178;
    ca_.Bind(&block15, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 97);
    compiler::TNode<FixedArrayBase> tmp179;
    USE(tmp179);
    tmp179 = ca_.UncheckedCast<FixedArrayBase>(BaseBuiltinsFromDSLAssembler(state_).LoadJSObjectElements(compiler::TNode<JSObject>{tmp171}));
    compiler::TNode<FixedDoubleArray> tmp180;
    USE(tmp180);
    tmp180 = ca_.UncheckedCast<FixedDoubleArray>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast16FixedDoubleArray(compiler::TNode<Context>{tmp170}, compiler::TNode<Object>{tmp179}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 96);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 98);
    compiler::TNode<IntPtrT> tmp181;
    USE(tmp181);
    tmp181 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).GetArgumentsLength(p_args));
    compiler::TNode<IntPtrT> tmp182;
    USE(tmp182);
    tmp182 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    ca_.Goto(&block20, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp180, tmp181, tmp182);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp183;
    compiler::TNode<JSArray> tmp184;
    compiler::TNode<Smi> tmp185;
    compiler::TNode<Smi> tmp186;
    compiler::TNode<Smi> tmp187;
    compiler::TNode<Smi> tmp188;
    compiler::TNode<Smi> tmp189;
    compiler::TNode<Smi> tmp190;
    compiler::TNode<Smi> tmp191;
    compiler::TNode<FixedDoubleArray> tmp192;
    compiler::TNode<IntPtrT> tmp193;
    compiler::TNode<IntPtrT> tmp194;
    ca_.Bind(&block20, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194);
    compiler::TNode<BoolT> tmp195;
    USE(tmp195);
    tmp195 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp194}, compiler::TNode<IntPtrT>{tmp193}));
    ca_.Branch(tmp195, &block17, &block19, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp196;
    compiler::TNode<JSArray> tmp197;
    compiler::TNode<Smi> tmp198;
    compiler::TNode<Smi> tmp199;
    compiler::TNode<Smi> tmp200;
    compiler::TNode<Smi> tmp201;
    compiler::TNode<Smi> tmp202;
    compiler::TNode<Smi> tmp203;
    compiler::TNode<Smi> tmp204;
    compiler::TNode<FixedDoubleArray> tmp205;
    compiler::TNode<IntPtrT> tmp206;
    compiler::TNode<IntPtrT> tmp207;
    ca_.Bind(&block17, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207);
    compiler::TNode<Object> tmp208;
    USE(tmp208);
    tmp208 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(p_args, compiler::TNode<IntPtrT>{tmp207}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 101);
    compiler::TNode<Smi> tmp209;
    USE(tmp209);
    tmp209 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(1));
    compiler::TNode<Smi> tmp210;
    USE(tmp210);
    tmp210 = ca_.UncheckedCast<Smi>(CodeStubAssembler(state_).SmiAdd(compiler::TNode<Smi>{tmp204}, compiler::TNode<Smi>{tmp209}));
    compiler::TNode<Number> tmp211;
    USE(tmp211);
    tmp211 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).UnsafeCast20UT5ATSmi10HeapNumber(compiler::TNode<Context>{tmp196}, compiler::TNode<Object>{tmp208}));
    BaseBuiltinsFromDSLAssembler(state_).StoreFixedDoubleArrayDirect(compiler::TNode<FixedDoubleArray>{tmp205}, compiler::TNode<Smi>{tmp204}, compiler::TNode<Number>{tmp211});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 98);
    ca_.Goto(&block18, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp210, tmp205, tmp206, tmp207);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp212;
    compiler::TNode<JSArray> tmp213;
    compiler::TNode<Smi> tmp214;
    compiler::TNode<Smi> tmp215;
    compiler::TNode<Smi> tmp216;
    compiler::TNode<Smi> tmp217;
    compiler::TNode<Smi> tmp218;
    compiler::TNode<Smi> tmp219;
    compiler::TNode<Smi> tmp220;
    compiler::TNode<FixedDoubleArray> tmp221;
    compiler::TNode<IntPtrT> tmp222;
    compiler::TNode<IntPtrT> tmp223;
    ca_.Bind(&block18, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223);
    compiler::TNode<IntPtrT> tmp224;
    USE(tmp224);
    tmp224 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp225;
    USE(tmp225);
    tmp225 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp223}, compiler::TNode<IntPtrT>{tmp224}));
    ca_.Goto(&block20, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp225);
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp226;
    compiler::TNode<JSArray> tmp227;
    compiler::TNode<Smi> tmp228;
    compiler::TNode<Smi> tmp229;
    compiler::TNode<Smi> tmp230;
    compiler::TNode<Smi> tmp231;
    compiler::TNode<Smi> tmp232;
    compiler::TNode<Smi> tmp233;
    compiler::TNode<Smi> tmp234;
    compiler::TNode<FixedDoubleArray> tmp235;
    compiler::TNode<IntPtrT> tmp236;
    compiler::TNode<IntPtrT> tmp237;
    ca_.Bind(&block19, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 95);
    ca_.Goto(&block16, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp238;
    compiler::TNode<JSArray> tmp239;
    compiler::TNode<Smi> tmp240;
    compiler::TNode<Smi> tmp241;
    compiler::TNode<Smi> tmp242;
    compiler::TNode<Smi> tmp243;
    compiler::TNode<Smi> tmp244;
    compiler::TNode<Smi> tmp245;
    compiler::TNode<Smi> tmp246;
    ca_.Bind(&block16, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 106);
    BaseBuiltinsFromDSLAssembler(state_).StoreJSArrayLength(compiler::TNode<JSArray>{tmp239}, compiler::TNode<Number>{tmp241});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 58);
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 54);
    ca_.Goto(&block2, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp247;
    compiler::TNode<JSArray> tmp248;
    compiler::TNode<Smi> tmp249;
    compiler::TNode<Smi> tmp250;
    compiler::TNode<Smi> tmp251;
    compiler::TNode<Smi> tmp252;
    compiler::TNode<Smi> tmp253;
    compiler::TNode<Smi> tmp254;
    ca_.Bind(&block2, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254);
    ca_.Goto(&block21, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254);
  }

  if (block1.is_used()) {
    ca_.Bind(&block1);
    ca_.Goto(label_Bailout);
  }

    compiler::TNode<Context> tmp255;
    compiler::TNode<JSArray> tmp256;
    compiler::TNode<Smi> tmp257;
    compiler::TNode<Smi> tmp258;
    compiler::TNode<Smi> tmp259;
    compiler::TNode<Smi> tmp260;
    compiler::TNode<Smi> tmp261;
    compiler::TNode<Smi> tmp262;
    ca_.Bind(&block21, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 35);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp1}));
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 36);
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 34);
    BaseBuiltinsFromDSLAssembler(state_).TorqueMoveElements(compiler::TNode<FixedArray>{tmp0}, compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}, compiler::TNode<IntPtrT>{tmp6});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 31);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 41);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 42);
    ArrayBuiltinsFromDSLAssembler(state_).StoreArrayHole(compiler::TNode<FixedArray>{tmp8}, compiler::TNode<Smi>{tmp11});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 41);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 39);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 50);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 51);
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp3}));
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 49);
    BaseBuiltinsFromDSLAssembler(state_).TorqueCopyElements(compiler::TNode<FixedArray>{tmp0}, compiler::TNode<IntPtrT>{tmp5}, compiler::TNode<FixedArray>{tmp2}, compiler::TNode<IntPtrT>{tmp6}, compiler::TNode<IntPtrT>{tmp7});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 46);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 35);
    compiler::TNode<IntPtrT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp1}));
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 36);
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 34);
    BaseBuiltinsFromDSLAssembler(state_).TorqueMoveElements(compiler::TNode<FixedDoubleArray>{tmp0}, compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}, compiler::TNode<IntPtrT>{tmp6});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 31);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 41);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 42);
    ArrayBuiltinsFromDSLAssembler(state_).StoreArrayHole(compiler::TNode<FixedDoubleArray>{tmp8}, compiler::TNode<Smi>{tmp11});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 41);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 39);
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
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 50);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp1}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 51);
    compiler::TNode<IntPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp3}));
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp4}));
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 49);
    BaseBuiltinsFromDSLAssembler(state_).TorqueCopyElements(compiler::TNode<FixedDoubleArray>{tmp0}, compiler::TNode<IntPtrT>{tmp5}, compiler::TNode<FixedDoubleArray>{tmp2}, compiler::TNode<IntPtrT>{tmp6}, compiler::TNode<IntPtrT>{tmp7});
    ca_.SetSourcePosition("../../src/builtins/array-splice.tq", 46);
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

