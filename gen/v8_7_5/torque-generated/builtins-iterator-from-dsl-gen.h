#ifndef V8_TORQUE_ITERATOR_FROM_DSL_BASE_H__
#define V8_TORQUE_ITERATOR_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class IteratorBuiltinsFromDSLAssembler {
 public:
  explicit IteratorBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  struct IteratorRecord {
    compiler::TNode<JSReceiver> object;
    compiler::TNode<Object> next;

    std::tuple<compiler::TNode<JSReceiver>, compiler::TNode<Object>> Flatten() const {
      return std::tuple_cat(std::make_tuple(object), std::make_tuple(next));
    }
  };
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ITERATOR_FROM_DSL_BASE_H__
