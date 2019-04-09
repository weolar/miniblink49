#ifndef V8_TORQUE_ARRAY_REVERSE_FROM_DSL_BASE_H__
#define V8_TORQUE_ARRAY_REVERSE_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArrayReverseBuiltinsFromDSLAssembler {
 public:
  explicit ArrayReverseBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  compiler::TNode<Smi> LoadElement23ATFastPackedSmiElements5ATSmi(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_index);
  compiler::TNode<Object> LoadElement26ATFastPackedObjectElements20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_index);
  compiler::TNode<Float64T> LoadElement26ATFastPackedDoubleElements9ATfloat64(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_index);
  void StoreElement23ATFastPackedSmiElements5ATSmi(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_index, compiler::TNode<Smi> p_value);
  void StoreElement26ATFastPackedObjectElements20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_index, compiler::TNode<Object> p_value);
  void StoreElement26ATFastPackedDoubleElements9ATfloat64(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_index, compiler::TNode<Float64T> p_value);
  compiler::TNode<Object> GenericArrayReverse(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver);
  void TryFastPackedArrayReverse(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, compiler::CodeAssemblerLabel* label_Slow);
  void FastPackedArrayReverse23ATFastPackedSmiElements5ATSmi(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_length);
  void FastPackedArrayReverse26ATFastPackedObjectElements20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_length);
  void FastPackedArrayReverse26ATFastPackedDoubleElements9ATfloat64(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_length);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARRAY_REVERSE_FROM_DSL_BASE_H__
