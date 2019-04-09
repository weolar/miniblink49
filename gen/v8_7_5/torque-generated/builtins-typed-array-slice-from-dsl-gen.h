#ifndef V8_TORQUE_TYPED_ARRAY_SLICE_FROM_DSL_BASE_H__
#define V8_TORQUE_TYPED_ARRAY_SLICE_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class TypedArraySliceBuiltinsFromDSLAssembler {
 public:
  explicit TypedArraySliceBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  const char* kBuiltinName();
  void FastCopy(compiler::TNode<JSTypedArray> p_src, compiler::TNode<JSTypedArray> p_dest, compiler::TNode<IntPtrT> p_k, compiler::TNode<Smi> p_count, compiler::CodeAssemblerLabel* label_IfSlow);
  void SlowCopy(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_src, compiler::TNode<JSTypedArray> p_dest, compiler::TNode<IntPtrT> p_k, compiler::TNode<IntPtrT> p_final);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_TYPED_ARRAY_SLICE_FROM_DSL_BASE_H__
