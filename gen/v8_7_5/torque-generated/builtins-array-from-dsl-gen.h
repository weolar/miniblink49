#ifndef V8_TORQUE_ARRAY_FROM_DSL_BASE_H__
#define V8_TORQUE_ARRAY_FROM_DSL_BASE_H__

#include "src/compiler/code-assembler.h"
#include "src/code-stub-assembler.h"
#include "src/utils.h"
#include "torque-generated/class-definitions-from-dsl.h"

namespace v8 {
namespace internal {

class ArrayBuiltinsFromDSLAssembler {
 public:
  explicit ArrayBuiltinsFromDSLAssembler(compiler::CodeAssemblerState* state) : state_(state), ca_(state) { USE(state_, ca_); }
  void EnsureWriteableFastElements(compiler::TNode<Context> p_context, compiler::TNode<JSArray> p_array);
  compiler::TNode<BoolT> IsJSArray(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o);
  compiler::TNode<Object> LoadElementOrUndefined(compiler::TNode<FixedArray> p_a, compiler::TNode<Smi> p_i);
  compiler::TNode<Object> LoadElementOrUndefined(compiler::TNode<FixedDoubleArray> p_a, compiler::TNode<Smi> p_i);
  void StoreArrayHole(compiler::TNode<FixedDoubleArray> p_elements, compiler::TNode<Smi> p_k);
  void StoreArrayHole(compiler::TNode<FixedArray> p_elements, compiler::TNode<Smi> p_k);
  void CopyArrayElement(compiler::TNode<FixedArray> p_elements, compiler::TNode<FixedArray> p_newElements, compiler::TNode<Smi> p_from, compiler::TNode<Smi> p_to);
  void CopyArrayElement(compiler::TNode<FixedDoubleArray> p_elements, compiler::TNode<FixedDoubleArray> p_newElements, compiler::TNode<Smi> p_from, compiler::TNode<Smi> p_to);
  compiler::TNode<Object> FastArrayEvery(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_thisArg, compiler::CodeAssemblerLabel* label_Bailout, compiler::TypedCodeAssemblerVariable<Smi>* label_Bailout_parameter_0);
  compiler::TNode<Object> FastArrayReduce(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_initialAccumulator, compiler::CodeAssemblerLabel* label_Bailout, compiler::TypedCodeAssemblerVariable<Number>* label_Bailout_parameter_0, compiler::TypedCodeAssemblerVariable<Object>* label_Bailout_parameter_1);
  compiler::TNode<Object> FastArrayReduceRight(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_initialAccumulator, compiler::CodeAssemblerLabel* label_Bailout, compiler::TypedCodeAssemblerVariable<Number>* label_Bailout_parameter_0, compiler::TypedCodeAssemblerVariable<Object>* label_Bailout_parameter_1);
  compiler::TNode<Object> FastArraySome(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_o, compiler::TNode<Number> p_len, compiler::TNode<JSReceiver> p_callbackfn, compiler::TNode<Object> p_thisArg, compiler::CodeAssemblerLabel* label_Bailout, compiler::TypedCodeAssemblerVariable<Smi>* label_Bailout_parameter_0);
  compiler::TNode<FixedArray> NewSortState(compiler::TNode<Context> p_context, compiler::TNode<JSReceiver> p_receiver, compiler::TNode<HeapObject> p_comparefn, compiler::TNode<Number> p_initialReceiverLength, compiler::TNode<Smi> p_sortLength, bool p_forceGeneric);
  compiler::TNode<Smi> kFailure();
  compiler::TNode<Smi> kSuccess();
  int31_t kMaxMergePending();
  int31_t kMinGallopWins();
  compiler::TNode<Smi> kSortStateTempSize();
  compiler::TNode<Smi> GetPendingRunsSize(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState);
  compiler::TNode<Smi> GetPendingRunBase(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_pendingRuns, compiler::TNode<Smi> p_run);
  void SetPendingRunBase(compiler::TNode<FixedArray> p_pendingRuns, compiler::TNode<Smi> p_run, compiler::TNode<Smi> p_value);
  compiler::TNode<Smi> GetPendingRunLength(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_pendingRuns, compiler::TNode<Smi> p_run);
  void SetPendingRunLength(compiler::TNode<FixedArray> p_pendingRuns, compiler::TNode<Smi> p_run, compiler::TNode<Smi> p_value);
  void PushRun(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<Smi> p_base, compiler::TNode<Smi> p_length);
  compiler::TNode<FixedArray> GetTempArray(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<Smi> p_requestedSize);
  void EnsureSuccess(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::CodeAssemblerLabel* label_Bailout);
  compiler::TNode<Smi> Failure(compiler::TNode<FixedArray> p_sortState);
  compiler::TNode<Object> CallLoad(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<BuiltinPtr> p_load, compiler::TNode<Smi> p_index, compiler::CodeAssemblerLabel* label_Bailout);
  void CallStore(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<BuiltinPtr> p_store, compiler::TNode<Smi> p_index, compiler::TNode<Object> p_value, compiler::CodeAssemblerLabel* label_Bailout);
  void BinaryInsertionSort(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<Smi> p_low, compiler::TNode<Smi> p_startArg, compiler::TNode<Smi> p_high);
  compiler::TNode<Smi> CountAndMakeRun(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<Smi> p_lowArg, compiler::TNode<Smi> p_high);
  void ReverseRange(compiler::TNode<FixedArray> p_array, compiler::TNode<Smi> p_from, compiler::TNode<Smi> p_to);
  void MergeLow(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<Smi> p_baseA, compiler::TNode<Smi> p_lengthAArg, compiler::TNode<Smi> p_baseB, compiler::TNode<Smi> p_lengthBArg);
  void MergeHigh(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<Smi> p_baseA, compiler::TNode<Smi> p_lengthAArg, compiler::TNode<Smi> p_baseB, compiler::TNode<Smi> p_lengthBArg);
  compiler::TNode<Smi> ComputeMinRunLength(compiler::TNode<Smi> p_nArg);
  compiler::TNode<BoolT> RunInvariantEstablished(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_pendingRuns, compiler::TNode<Smi> p_n);
  void MergeCollapse(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState);
  void MergeForceCollapse(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState);
  void ArrayTimSortImpl(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<Smi> p_length);
  void CopyReceiverElementsToWorkArray(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<Smi> p_length);
  void CopyWorkArrayToReceiver(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_sortState, compiler::TNode<Smi> p_length);
  compiler::TNode<JSReceiver> LoadSortStateReceiver(compiler::TNode<FixedArray> p_o);
  void StoreSortStateReceiver(compiler::TNode<FixedArray> p_o, compiler::TNode<JSReceiver> p_v);
  compiler::TNode<Map> LoadSortStateInitialReceiverMap(compiler::TNode<FixedArray> p_o);
  void StoreSortStateInitialReceiverMap(compiler::TNode<FixedArray> p_o, compiler::TNode<Map> p_v);
  compiler::TNode<Number> LoadSortStateInitialReceiverLength(compiler::TNode<FixedArray> p_o);
  void StoreSortStateInitialReceiverLength(compiler::TNode<FixedArray> p_o, compiler::TNode<Number> p_v);
  compiler::TNode<HeapObject> LoadSortStateUserCmpFn(compiler::TNode<FixedArray> p_o);
  void StoreSortStateUserCmpFn(compiler::TNode<FixedArray> p_o, compiler::TNode<HeapObject> p_v);
  compiler::TNode<BuiltinPtr> LoadSortStateSortComparePtr(compiler::TNode<FixedArray> p_o);
  void StoreSortStateSortComparePtr(compiler::TNode<FixedArray> p_o, compiler::TNode<BuiltinPtr> p_v);
  compiler::TNode<BuiltinPtr> LoadSortStateLoadFn(compiler::TNode<FixedArray> p_o);
  void StoreSortStateLoadFn(compiler::TNode<FixedArray> p_o, compiler::TNode<BuiltinPtr> p_v);
  compiler::TNode<BuiltinPtr> LoadSortStateStoreFn(compiler::TNode<FixedArray> p_o);
  void StoreSortStateStoreFn(compiler::TNode<FixedArray> p_o, compiler::TNode<BuiltinPtr> p_v);
  compiler::TNode<BuiltinPtr> LoadSortStateCanUseSameAccessorFn(compiler::TNode<FixedArray> p_o);
  void StoreSortStateCanUseSameAccessorFn(compiler::TNode<FixedArray> p_o, compiler::TNode<BuiltinPtr> p_v);
  compiler::TNode<Smi> LoadSortStateBailoutStatus(compiler::TNode<FixedArray> p_o);
  void StoreSortStateBailoutStatus(compiler::TNode<FixedArray> p_o, compiler::TNode<Smi> p_v);
  compiler::TNode<Smi> LoadSortStateMinGallop(compiler::TNode<FixedArray> p_o);
  void StoreSortStateMinGallop(compiler::TNode<FixedArray> p_o, compiler::TNode<Smi> p_v);
  compiler::TNode<Smi> LoadSortStatePendingRunsSize(compiler::TNode<FixedArray> p_o);
  void StoreSortStatePendingRunsSize(compiler::TNode<FixedArray> p_o, compiler::TNode<Smi> p_v);
  compiler::TNode<FixedArray> LoadSortStatePendingRuns(compiler::TNode<FixedArray> p_o);
  void StoreSortStatePendingRuns(compiler::TNode<FixedArray> p_o, compiler::TNode<FixedArray> p_v);
  compiler::TNode<FixedArray> LoadSortStateWorkArray(compiler::TNode<FixedArray> p_o);
  void StoreSortStateWorkArray(compiler::TNode<FixedArray> p_o, compiler::TNode<FixedArray> p_v);
  compiler::TNode<FixedArray> LoadSortStateTempArray(compiler::TNode<FixedArray> p_o);
  void StoreSortStateTempArray(compiler::TNode<FixedArray> p_o, compiler::TNode<FixedArray> p_v);
  compiler::TNode<Number> _method_SortState_Compare(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_this, compiler::TNode<Object> p_x, compiler::TNode<Object> p_y);
  void _method_SortState_CheckAccessor(compiler::TNode<Context> p_context, compiler::TNode<FixedArray> p_this, compiler::CodeAssemblerLabel* label_Bailout);
  void _method_SortState_ResetToGenericAccessor(compiler::TNode<FixedArray> p_this);
 private:
  compiler::CodeAssemblerState* const state_;
  compiler::CodeAssembler ca_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_ARRAY_FROM_DSL_BASE_H__
