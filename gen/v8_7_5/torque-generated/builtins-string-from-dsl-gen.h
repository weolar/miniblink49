#ifndef V8_TORQUE_STRING_FROM_DSL_BASE_H__
#define V8_TORQUE_STRING_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class StringBuiltinsFromDSLAssembler {
 public:
  explicit StringBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  compiler::TNode<Oddball> TryFastStringCompareSequence(compiler::TNode<String> p_string, compiler::TNode<String> p_searchStr, compiler::TNode<Number> p_start, compiler::TNode<Smi> p_searchLength, compiler::CodeAssemblerLabel* label_Slow);
  compiler::TNode<Object> RequireObjectCoercible(compiler::TNode<Context> p_context, compiler::TNode<Object> p_argument);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_STRING_FROM_DSL_BASE_H__
