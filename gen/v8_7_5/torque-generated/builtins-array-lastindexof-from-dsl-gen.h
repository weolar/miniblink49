#ifndef V8_TORQUE_ARRAY_LASTINDEXOF_FROM_DSL_BASE_H__
#define V8_TORQUE_ARRAY_LASTINDEXOF_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArrayLastindexofBuiltinsFromDSLAssembler {
 public:
  explicit ArrayLastindexofBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  compiler::TNode<Object> LoadWithHoleCheck10FixedArray(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_index, compiler::CodeAssemblerLabel* label_IfHole);
  compiler::TNode<Object> LoadWithHoleCheck16FixedDoubleArray(compiler::TNode<Context> p_context, compiler::TNode<FixedArrayBase> p_elements, compiler::TNode<Smi> p_index, compiler::CodeAssemblerLabel* label_IfHole);
  compiler::TNode<Number> GetFromIndex(compiler::TNode<Context> p_context, compiler::TNode<Number> p_length, CodeStubArguments* p_arguments);
  compiler::TNode<Object> TryFastArrayLastIndexOf(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<Object> p_searchElement, compiler::TNode<Number> p_from, compiler::CodeAssemblerLabel* label_Slow);
  compiler::TNode<Object> GenericArrayLastIndexOf(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_object, compiler::TNode<Object> p_searchElement, compiler::TNode<Number> p_from);
  compiler::TNode<Smi> FastArrayLastIndexOf10FixedArray(compiler::TNode<Context> p_context, compiler::TNode<JSArray> p_array, compiler::TNode<Smi> p_from, compiler::TNode<Object> p_searchElement);
  compiler::TNode<Smi> FastArrayLastIndexOf16FixedDoubleArray(compiler::TNode<Context> p_context, compiler::TNode<JSArray> p_array, compiler::TNode<Smi> p_from, compiler::TNode<Object> p_searchElement);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARRAY_LASTINDEXOF_FROM_DSL_BASE_H__
