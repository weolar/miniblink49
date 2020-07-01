#ifndef V8_TORQUE_ARRAY_FILTER_FROM_DSL_BASE_H__
#define V8_TORQUE_ARRAY_FILTER_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArrayFilterBuiltinsFromDSLAssembler {
 public:
  explicit ArrayFilterBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  void FastArrayFilter(compiler::TNode<Context> p_context, compiler::TNode<JSArray> p_fastO, compiler::TNode<Smi> p_len, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_thisArg, compiler::TNode<JSArray> p_output, compiler::CodeAssemblerLabel* label_Bailout, compiler::TypedCodeAssemblerVariable<Number>* label_Bailout_parameter_0, compiler::TypedCodeAssemblerVariable<Number>* label_Bailout_parameter_1);
  compiler::TNode<JSReceiver> FastFilterSpeciesCreate(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::CodeAssemblerLabel* label_Slow);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARRAY_FILTER_FROM_DSL_BASE_H__
