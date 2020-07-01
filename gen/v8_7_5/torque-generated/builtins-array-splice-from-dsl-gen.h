#ifndef V8_TORQUE_ARRAY_SPLICE_FROM_DSL_BASE_H__
#define V8_TORQUE_ARRAY_SPLICE_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArraySpliceBuiltinsFromDSLAssembler {
 public:
  explicit ArraySpliceBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  compiler::TNode<FixedArray> Extract10FixedArray(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_first, compiler::TNode<Smi> p_count, compiler::TNode<Smi> p_capacity);
  compiler::TNode<FixedDoubleArray> Extract16FixedDoubleArray(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_first, compiler::TNode<Smi> p_count, compiler::TNode<Smi> p_capacity);
  compiler::TNode<Object> FastArraySplice(compiler::TNode<Context> p_context, CodeStubArguments* p_args, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_originalLengthNumber, compiler::TNode<Number> p_actualStartNumber, compiler::TNode<Smi> p_insertCount, compiler::TNode<Number> p_actualDeleteCountNumber, compiler::CodeAssemblerLabel* label_Bailout);
  compiler::TNode<Object> FillDeletedElementsArray(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_actualStart, compiler::TNode<Number> p_actualDeleteCount, compiler::TNode<JSReceiver> p_a);
  void HandleForwardCase(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Number> p_itemCount, compiler::TNode<Number> p_actualStart, compiler::TNode<Number> p_actualDeleteCount);
  void HandleBackwardCase(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Number> p_itemCount, compiler::TNode<Number> p_actualStart, compiler::TNode<Number> p_actualDeleteCount);
  compiler::TNode<Object> SlowSplice(compiler::TNode<Context> p_context, CodeStubArguments* p_arguments, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Number> p_actualStart, compiler::TNode<Smi> p_insertCount, compiler::TNode<Number> p_actualDeleteCount);
  void FastSplice10FixedArray20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, CodeStubArguments* p_args, compiler::TNode<JSArray> p_a, compiler::TNode<Smi> p_length, compiler::TNode<Smi> p_newLength, compiler::TNode<Smi> p_lengthDelta, compiler::TNode<Smi> p_actualStart, compiler::TNode<Smi> p_insertCount, compiler::TNode<Smi> p_actualDeleteCount, compiler::CodeAssemblerLabel* label_Bailout);
  void FastSplice16FixedDoubleArray20UT5ATSmi10HeapNumber(compiler::TNode<Context> p_context, CodeStubArguments* p_args, compiler::TNode<JSArray> p_a, compiler::TNode<Smi> p_length, compiler::TNode<Smi> p_newLength, compiler::TNode<Smi> p_lengthDelta, compiler::TNode<Smi> p_actualStart, compiler::TNode<Smi> p_insertCount, compiler::TNode<Smi> p_actualDeleteCount, compiler::CodeAssemblerLabel* label_Bailout);
  void DoMoveElements10FixedArray(compiler::TNode<FixedArray> p_elements, compiler::TNode<Smi> p_dstIndex, compiler::TNode<Smi> p_srcIndex, compiler::TNode<Smi> p_count);
  void StoreHoles10FixedArray(compiler::TNode<FixedArray> p_elements, compiler::TNode<Smi> p_holeStartIndex, compiler::TNode<Smi> p_holeEndIndex);
  void DoCopyElements10FixedArray(compiler::TNode<FixedArray> p_dstElements, compiler::TNode<Smi> p_dstIndex, compiler::TNode<FixedArray> p_srcElements, compiler::TNode<Smi> p_srcIndex, compiler::TNode<Smi> p_count);
  void DoMoveElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray> p_elements, compiler::TNode<Smi> p_dstIndex, compiler::TNode<Smi> p_srcIndex, compiler::TNode<Smi> p_count);
  void StoreHoles16FixedDoubleArray(compiler::TNode<FixedDoubleArray> p_elements, compiler::TNode<Smi> p_holeStartIndex, compiler::TNode<Smi> p_holeEndIndex);
  void DoCopyElements16FixedDoubleArray(compiler::TNode<FixedDoubleArray> p_dstElements, compiler::TNode<Smi> p_dstIndex, compiler::TNode<FixedDoubleArray> p_srcElements, compiler::TNode<Smi> p_srcIndex, compiler::TNode<Smi> p_count);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARRAY_SPLICE_FROM_DSL_BASE_H__
