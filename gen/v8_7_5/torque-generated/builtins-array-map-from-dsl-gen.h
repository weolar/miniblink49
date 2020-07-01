#ifndef V8_TORQUE_ARRAY_MAP_FROM_DSL_BASE_H__
#define V8_TORQUE_ARRAY_MAP_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArrayMapBuiltinsFromDSLAssembler {
 public:
  explicit ArrayMapBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  struct Vector {
    compiler::TNode<FixedArray> fixedArray;
    compiler::TNode<BoolT> onlySmis;
    compiler::TNode<BoolT> onlyNumbers;
    compiler::TNode<BoolT> skippedElements;

    std::tuple<compiler::TNode<FixedArray>, compiler::TNode<BoolT>, compiler::TNode<BoolT>, compiler::TNode<BoolT>> Flatten() const {
      return std::tuple_cat(std::make_tuple(fixedArray), std::make_tuple(onlySmis), std::make_tuple(onlyNumbers), std::make_tuple(skippedElements));
    }
  };
  ArrayMapBuiltinsFromDSLAssembler::Vector NewVector(compiler::TNode<Context> p_context, compiler::TNode<Smi> p_length);
  compiler::TNode<JSArray> FastArrayMap(compiler::TNode<Context> p_context, compiler::TNode<JSArray> p_fastO, compiler::TNode<Smi> p_len, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_thisArg, compiler::CodeAssemblerLabel* label_Bailout, compiler::TypedCodeAssemblerVariable<JSArray>* label_Bailout_parameter_0, compiler::TypedCodeAssemblerVariable<Smi>* label_Bailout_parameter_1);
  compiler::TNode<JSArray> FastMapSpeciesCreate(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<Number> p_length, compiler::CodeAssemblerLabel* label_Bailout);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARRAY_MAP_FROM_DSL_BASE_H__
