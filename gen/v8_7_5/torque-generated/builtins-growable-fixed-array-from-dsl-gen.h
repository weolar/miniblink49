#ifndef V8_TORQUE_GROWABLE_FIXED_ARRAY_FROM_DSL_BASE_H__
#define V8_TORQUE_GROWABLE_FIXED_ARRAY_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class GrowableFixedArrayBuiltinsFromDSLAssembler {
 public:
  explicit GrowableFixedArrayBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  struct GrowableFixedArray {
    compiler::TNode<FixedArray> array;
    compiler::TNode<IntPtrT> capacity;
    compiler::TNode<IntPtrT> length;

    std::tuple<compiler::TNode<FixedArray>, compiler::TNode<IntPtrT>, compiler::TNode<IntPtrT>> Flatten() const {
      return std::tuple_cat(std::make_tuple(array), std::make_tuple(capacity), std::make_tuple(length));
    }
  };
  GrowableFixedArrayBuiltinsFromDSLAssembler::GrowableFixedArray NewGrowableFixedArray();
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_GROWABLE_FIXED_ARRAY_FROM_DSL_BASE_H__
