#ifndef V8_TORQUE_TYPED_ARRAY_FROM_DSL_BASE_H__
#define V8_TORQUE_TYPED_ARRAY_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class TypedArrayBuiltinsFromDSLAssembler {
 public:
  explicit TypedArrayBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  struct TypedArrayElementsInfo {
    compiler::TNode<UintPtrT> sizeLog2;
    compiler::TNode<Map> map;
    compiler::TNode<Int32T> kind;

    std::tuple<compiler::TNode<UintPtrT>, compiler::TNode<Map>, compiler::TNode<Int32T>> Flatten() const {
      return std::tuple_cat(std::make_tuple(sizeLog2), std::make_tuple(map), std::make_tuple(kind));
    }
  };
  compiler::TNode<BuiltinPtr> UnsafeCast54FT9ATContext12JSTypedArray5ATSmi20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o);
  compiler::TNode<BuiltinPtr> UnsafeCast76FT9ATContext12JSTypedArray5ATSmi20UT5ATSmi10HeapObject20UT5ATSmi10HeapObject(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o);
  compiler::TNode<JSTypedArray> EnsureAttached(compiler::TNode<JSTypedArray> p_array, compiler::CodeAssemblerLabel* label_Detached);
  struct AttachedJSTypedArrayWitness {
    compiler::TNode<JSTypedArray> stable;
    compiler::TNode<JSTypedArray> unstable;
    compiler::TNode<BuiltinPtr> loadfn;

    std::tuple<compiler::TNode<JSTypedArray>, compiler::TNode<JSTypedArray>, compiler::TNode<BuiltinPtr>> Flatten() const {
      return std::tuple_cat(std::make_tuple(stable), std::make_tuple(unstable), std::make_tuple(loadfn));
    }
  };
  TypedArrayBuiltinsFromDSLAssembler::AttachedJSTypedArrayWitness NewAttachedJSTypedArrayWitness(compiler::TNode<JSTypedArray> p_array);
  compiler::TNode<BuiltinPtr> GetLoadFnForElementsKind(compiler::TNode<Int32T> p_elementsKind);
  ElementsKind KindForArrayType17ATFixedUint8Array();
  ElementsKind KindForArrayType16ATFixedInt8Array();
  ElementsKind KindForArrayType18ATFixedUint16Array();
  ElementsKind KindForArrayType17ATFixedInt16Array();
  ElementsKind KindForArrayType18ATFixedUint32Array();
  ElementsKind KindForArrayType17ATFixedInt32Array();
  ElementsKind KindForArrayType19ATFixedFloat32Array();
  ElementsKind KindForArrayType19ATFixedFloat64Array();
  ElementsKind KindForArrayType24ATFixedUint8ClampedArray();
  ElementsKind KindForArrayType21ATFixedBigUint64Array();
  ElementsKind KindForArrayType20ATFixedBigInt64Array();
  compiler::TNode<Number> CallCompare(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_array, compiler::TNode<JSReceiver> p_comparefn, compiler::TNode<Object> p_a, compiler::TNode<Object> p_b);
  void TypedArrayMerge(compiler::TNode<Context> p_context, compiler::TNode<JSTypedArray> p_array, compiler::TNode<JSReceiver> p_comparefn, compiler::TNode<FixedArray> p_source, compiler::TNode<Smi> p_from, compiler::TNode<Smi> p_middle, compiler::TNode<Smi> p_to, compiler::TNode<FixedArray> p_target);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_TYPED_ARRAY_FROM_DSL_BASE_H__
