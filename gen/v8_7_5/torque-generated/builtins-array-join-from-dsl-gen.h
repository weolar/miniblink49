#ifndef V8_TORQUE_ARRAY_JOIN_FROM_DSL_BASE_H__
#define V8_TORQUE_ARRAY_JOIN_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArrayJoinBuiltinsFromDSLAssembler {
 public:
  explicit ArrayJoinBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  void CannotUseSameArrayAccessor7JSArray(compiler::TNode<Context> p_context, compiler::TNode<BuiltinPtr> p_loadFn, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<Map> p_originalMap, compiler::TNode<Number> p_originalLen, compiler::CodeAssemblerLabel* label_Cannot, compiler::CodeAssemblerLabel* label_Can);
  void CannotUseSameArrayAccessor12JSTypedArray(compiler::TNode<Context> p_context, compiler::TNode<BuiltinPtr> p_loadFn, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<Map> p_initialMap, compiler::TNode<Number> p_initialLen, compiler::CodeAssemblerLabel* label_Cannot, compiler::CodeAssemblerLabel* label_Can);
  compiler::TNode<IntPtrT> AddStringLength(compiler::TNode<Context> p_context, compiler::TNode<IntPtrT> p_lenA, compiler::TNode<IntPtrT> p_lenB);
  struct Buffer {
    compiler::TNode<FixedArray> fixedArray;
    compiler::TNode<IntPtrT> index;
    compiler::TNode<IntPtrT> totalStringLength;
    compiler::TNode<BoolT> isOneByte;

    std::tuple<compiler::TNode<FixedArray>, compiler::TNode<IntPtrT>, compiler::TNode<IntPtrT>, compiler::TNode<BoolT>> Flatten() const {
      return std::tuple_cat(std::make_tuple(fixedArray), std::make_tuple(index), std::make_tuple(totalStringLength), std::make_tuple(isOneByte));
    }
  };
  ArrayJoinBuiltinsFromDSLAssembler::Buffer NewBuffer(compiler::TNode<UintPtrT> p_len, compiler::TNode<String> p_sep);
  compiler::TNode<String> BufferJoin(compiler::TNode<Context> p_context, ArrayJoinBuiltinsFromDSLAssembler::Buffer p_buffer, compiler::TNode<String> p_sep);
  compiler::TNode<Object> ArrayJoin7JSArray(compiler::TNode<Context> p_context, bool p_useToLocaleString, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<String> p_sep, compiler::TNode<Number> p_lenNumber, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options);
  compiler::TNode<Object> ArrayJoin12JSTypedArray(compiler::TNode<Context> p_context, bool p_useToLocaleString, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<String> p_sep, compiler::TNode<Number> p_lenNumber, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options);
  compiler::TNode<FixedArray> LoadJoinStack(compiler::TNode<Context> p_context, compiler::CodeAssemblerLabel* label_IfUninitialized);
  void SetJoinStack(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_stack);
  void JoinStackPushInline(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::CodeAssemblerLabel* label_ReceiverAdded, compiler::CodeAssemblerLabel* label_ReceiverNotAdded);
  void JoinStackPopInline(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver);
  compiler::TNode<String> ArrayJoinImpl7JSArray(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<String> p_sep, compiler::TNode<Number> p_lengthNumber, bool p_useToLocaleString, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options, compiler::TNode<BuiltinPtr> p_initialLoadFn);
  compiler::TNode<String> ArrayJoinImpl12JSTypedArray(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<String> p_sep, compiler::TNode<Number> p_lengthNumber, bool p_useToLocaleString, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options, compiler::TNode<BuiltinPtr> p_initialLoadFn);
  compiler::TNode<FixedArray> StoreAndGrowFixedArray10JSReceiver(compiler::TNode<FixedArray> p_fixedArray, compiler::TNode<IntPtrT> p_index, compiler::TNode<JSReceiver> p_element);
  compiler::TNode<Object> CycleProtectedArrayJoin7JSArray(compiler::TNode<Context> p_context, bool p_useToLocaleString, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Object> p_sepObj, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options);
  compiler::TNode<Object> CycleProtectedArrayJoin12JSTypedArray(compiler::TNode<Context> p_context, bool p_useToLocaleString, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<Object> p_sepObj, compiler::TNode<Object> p_locales, compiler::TNode<Object> p_options);
  compiler::TNode<FixedArray> StoreAndGrowFixedArray5ATSmi(compiler::TNode<FixedArray> p_fixedArray, compiler::TNode<IntPtrT> p_index, compiler::TNode<Smi> p_element);
  compiler::TNode<FixedArray> StoreAndGrowFixedArray6String(compiler::TNode<FixedArray> p_fixedArray, compiler::TNode<IntPtrT> p_index, compiler::TNode<String> p_element);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARRAY_JOIN_FROM_DSL_BASE_H__
