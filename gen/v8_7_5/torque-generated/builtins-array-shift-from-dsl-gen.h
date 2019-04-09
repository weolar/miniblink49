#ifndef V8_TORQUE_ARRAY_SHIFT_FROM_DSL_BASE_H__
#define V8_TORQUE_ARRAY_SHIFT_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArrayShiftBuiltinsFromDSLAssembler {
 public:
  explicit ArrayShiftBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  compiler::TNode<Object> TryFastArrayShift(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, CodeStubArguments* p_arguments, compiler::CodeAssemblerLabel* label_Slow);
  compiler::TNode<Object> GenericArrayShift(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARRAY_SHIFT_FROM_DSL_BASE_H__
