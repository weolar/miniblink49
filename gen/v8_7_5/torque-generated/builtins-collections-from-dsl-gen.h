#ifndef V8_TORQUE_COLLECTIONS_FROM_DSL_BASE_H__
#define V8_TORQUE_COLLECTIONS_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class CollectionsBuiltinsFromDSLAssembler {
 public:
  explicit CollectionsBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  BaseBuiltinsFromDSLAssembler::KeyValuePair LoadKeyValuePairNoSideEffects(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o, compiler::CodeAssemblerLabel* label_MayHaveSideEffects);
  BaseBuiltinsFromDSLAssembler::KeyValuePair LoadKeyValuePair(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_COLLECTIONS_FROM_DSL_BASE_H__
