#ifndef V8_TORQUE_TYPED_ARRAY_CREATETYPEDARRAY_FROM_DSL_BASE_H__
#define V8_TORQUE_TYPED_ARRAY_CREATETYPEDARRAY_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler {
 public:
  explicit TypedArrayCreatetypedarrayBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  compiler::TNode<IntPtrT> CalculateTotalElementsByteSize(compiler::TNode<IntPtrT> p_byteLength);
  compiler::TNode<UintPtrT> TypedArrayInitialize(compiler::TNode<Context> p_context, bool p_initialize, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<Smi> p_length, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::TNode<JSReceiver> p_bufferConstructor);
  void ConstructByLength(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<Object> p_length, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo);
  void ConstructByArrayLike(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<HeapObject> p_arrayLike, compiler::TNode<Object> p_initialLength, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::TNode<JSReceiver> p_bufferConstructor);
  void ConstructByIterable(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<JSReceiver> p_iterable, compiler::TNode<JSReceiver> p_iteratorFn, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::CodeAssemblerLabel* label_IfConstructByArrayLike, compiler::TypedCodeAssemblerVariable<HeapObject>* label_IfConstructByArrayLike_parameter_0, compiler::TypedCodeAssemblerVariable<Object>* label_IfConstructByArrayLike_parameter_1, compiler::TypedCodeAssemblerVariable<JSReceiver>* label_IfConstructByArrayLike_parameter_2);
  void ConstructByTypedArray(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<JSTypedArray> p_srcTypedArray, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::CodeAssemblerLabel* label_IfConstructByArrayLike, compiler::TypedCodeAssemblerVariable<HeapObject>* label_IfConstructByArrayLike_parameter_0, compiler::TypedCodeAssemblerVariable<Object>* label_IfConstructByArrayLike_parameter_1, compiler::TypedCodeAssemblerVariable<JSReceiver>* label_IfConstructByArrayLike_parameter_2);
  void ConstructByArrayBuffer(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_typedArray, compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<Object> p_byteOffset, compiler::TNode<Object> p_length, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo);
  void ConstructByJSReceiver(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_array, compiler::TNode<JSReceiver> p_obj, TypedArrayBuiltinsFromDSLAssembler::TypedArrayElementsInfo p_elementsInfo, compiler::CodeAssemblerLabel* label_IfConstructByArrayLike, compiler::TypedCodeAssemblerVariable<HeapObject>* label_IfConstructByArrayLike_parameter_0, compiler::TypedCodeAssemblerVariable<Object>* label_IfConstructByArrayLike_parameter_1, compiler::TypedCodeAssemblerVariable<JSReceiver>* label_IfConstructByArrayLike_parameter_2);
  compiler::TNode<JSTypedArray> TypedArraySpeciesCreate(compiler::TNode<Context> p_context, const char* p_methodName, int31_t p_numArgs, compiler::TNode<JSTypedArray> p_exemplar, compiler::TNode<Object> p_arg0, compiler::TNode<Object> p_arg1, compiler::TNode<Object> p_arg2);
  compiler::TNode<JSTypedArray> TypedArraySpeciesCreateByLength(compiler::TNode<Context> p_context, const char* p_methodName, compiler::TNode<JSTypedArray> p_exemplar, compiler::TNode<Smi> p_length);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_TYPED_ARRAY_CREATETYPEDARRAY_FROM_DSL_BASE_H__
