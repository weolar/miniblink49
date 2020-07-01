#ifndef V8_TORQUE_ARGUMENTS_FROM_DSL_BASE_H__
#define V8_TORQUE_ARGUMENTS_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArgumentsBuiltinsFromDSLAssembler {
 public:
  explicit ArgumentsBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  struct ArgumentsInfo {
    compiler::TNode<RawPtrT> frame;
    compiler::TNode<BInt> argument_count;
    compiler::TNode<BInt> formal_parameter_count;

    std::tuple<compiler::TNode<RawPtrT>, compiler::TNode<BInt>, compiler::TNode<BInt>> Flatten() const {
      return std::tuple_cat(std::make_tuple(frame), std::make_tuple(argument_count), std::make_tuple(formal_parameter_count));
    }
  };
  ArgumentsBuiltinsFromDSLAssembler::ArgumentsInfo GetArgumentsFrameAndCount(compiler::TNode<Context> p_context, compiler::TNode<JSFunction> p_f);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARGUMENTS_FROM_DSL_BASE_H__
