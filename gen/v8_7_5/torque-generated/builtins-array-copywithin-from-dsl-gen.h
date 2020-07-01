#ifndef V8_TORQUE_ARRAY_COPYWITHIN_FROM_DSL_BASE_H__
#define V8_TORQUE_ARRAY_COPYWITHIN_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArrayCopywithinBuiltinsFromDSLAssembler {
 public:
  explicit ArrayCopywithinBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  compiler::TNode<Number> ConvertToRelativeIndex(compiler::TNode<Number> p_index, compiler::TNode<Number> p_length);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARRAY_COPYWITHIN_FROM_DSL_BASE_H__
