#ifndef V8_TORQUE_DATA_VIEW_FROM_DSL_BASE_H__
#define V8_TORQUE_DATA_VIEW_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class DataViewBuiltinsFromDSLAssembler {
 public:
  explicit DataViewBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  compiler::TNode<String> MakeDataViewGetterNameString(ElementsKind p_kind);
  compiler::TNode<String> MakeDataViewSetterNameString(ElementsKind p_kind);
  compiler::TNode<BoolT> WasNeutered(compiler::TNode<JSArrayBufferView> p_view);
  compiler::TNode<JSDataView> ValidateDataView(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o, compiler::TNode<String> p_method);
  compiler::TNode<Smi> LoadDataView8(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, bool p_signed);
  compiler::TNode<Number> LoadDataView16(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BoolT> p_requestedLittleEndian, bool p_signed);
  compiler::TNode<Number> LoadDataView32(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BoolT> p_requestedLittleEndian, ElementsKind p_kind);
  compiler::TNode<Number> LoadDataViewFloat64(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BoolT> p_requestedLittleEndian);
  bool kPositiveBigInt();
  bool kNegativeBigInt();
  int31_t kZeroDigitBigInt();
  int31_t kOneDigitBigInt();
  int31_t kTwoDigitBigInt();
  compiler::TNode<BigInt> CreateEmptyBigInt(compiler::TNode<BoolT> p_isPositive, int31_t p_length);
  compiler::TNode<BigInt> MakeBigIntOn64Bit(compiler::TNode<Uint32T> p_lowWord, compiler::TNode<Uint32T> p_highWord, bool p_signed);
  compiler::TNode<BigInt> MakeBigIntOn32Bit(compiler::TNode<Uint32T> p_lowWord, compiler::TNode<Uint32T> p_highWord, bool p_signed);
  compiler::TNode<BigInt> MakeBigInt(compiler::TNode<Uint32T> p_lowWord, compiler::TNode<Uint32T> p_highWord, bool p_signed);
  compiler::TNode<BigInt> LoadDataViewBigInt(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BoolT> p_requestedLittleEndian, bool p_signed);
  compiler::TNode<Numeric> DataViewGet(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, compiler::TNode<Object> p_offset, compiler::TNode<Object> p_requestedLittleEndian, ElementsKind p_kind);
  void StoreDataView8(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<Uint32T> p_value);
  void StoreDataView16(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<Uint32T> p_value, compiler::TNode<BoolT> p_requestedLittleEndian);
  void StoreDataView32(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<Uint32T> p_value, compiler::TNode<BoolT> p_requestedLittleEndian);
  void StoreDataView64(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<Uint32T> p_lowWord, compiler::TNode<Uint32T> p_highWord, compiler::TNode<BoolT> p_requestedLittleEndian);
  void StoreDataViewBigInt(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BigInt> p_bigIntValue, compiler::TNode<BoolT> p_requestedLittleEndian);
  compiler::TNode<Object> DataViewSet(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, compiler::TNode<Object> p_offset, compiler::TNode<Object> p_value, compiler::TNode<Object> p_requestedLittleEndian, ElementsKind p_kind);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_DATA_VIEW_FROM_DSL_BASE_H__
