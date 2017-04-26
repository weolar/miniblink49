// Copyright 2011 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_BUILTINS_H_
#define V8_BUILTINS_H_

#include "src/handles.h"

namespace v8 {
namespace internal {

// Specifies extra arguments required by a C++ builtin.
enum BuiltinExtraArguments {
  NO_EXTRA_ARGUMENTS = 0,
  NEEDS_CALLED_FUNCTION = 1
};


#define CODE_AGE_LIST_WITH_ARG(V, A)     \
  V(Quadragenarian, A)                   \
  V(Quinquagenarian, A)                  \
  V(Sexagenarian, A)                     \
  V(Septuagenarian, A)                   \
  V(Octogenarian, A)

#define CODE_AGE_LIST_IGNORE_ARG(X, V) V(X)

#define CODE_AGE_LIST(V) \
  CODE_AGE_LIST_WITH_ARG(CODE_AGE_LIST_IGNORE_ARG, V)

#define CODE_AGE_LIST_COMPLETE(V)                  \
  V(ToBeExecutedOnce)                              \
  V(NotExecuted)                                   \
  V(ExecutedOnce)                                  \
  V(NoAge)                                         \
  CODE_AGE_LIST_WITH_ARG(CODE_AGE_LIST_IGNORE_ARG, V)

#define DECLARE_CODE_AGE_BUILTIN(C, V)             \
  V(Make##C##CodeYoungAgainOddMarking, BUILTIN,    \
    UNINITIALIZED, kNoExtraICState)                \
  V(Make##C##CodeYoungAgainEvenMarking, BUILTIN,   \
    UNINITIALIZED, kNoExtraICState)


// Define list of builtins implemented in C++.
#define BUILTIN_LIST_C(V)                                    \
  V(Illegal, NO_EXTRA_ARGUMENTS)                             \
                                                             \
  V(EmptyFunction, NO_EXTRA_ARGUMENTS)                       \
                                                             \
  V(ArrayPush, NO_EXTRA_ARGUMENTS)                           \
  V(ArrayPop, NO_EXTRA_ARGUMENTS)                            \
  V(ArrayShift, NO_EXTRA_ARGUMENTS)                          \
  V(ArrayUnshift, NO_EXTRA_ARGUMENTS)                        \
  V(ArraySlice, NO_EXTRA_ARGUMENTS)                          \
  V(ArraySplice, NO_EXTRA_ARGUMENTS)                         \
  V(ArrayConcat, NO_EXTRA_ARGUMENTS)                         \
                                                             \
  V(DateToPrimitive, NO_EXTRA_ARGUMENTS)                     \
                                                             \
  V(ReflectDefineProperty, NO_EXTRA_ARGUMENTS)               \
  V(ReflectDeleteProperty, NO_EXTRA_ARGUMENTS)               \
  V(ReflectGet, NO_EXTRA_ARGUMENTS)                          \
  V(ReflectGetOwnPropertyDescriptor, NO_EXTRA_ARGUMENTS)     \
  V(ReflectGetPrototypeOf, NO_EXTRA_ARGUMENTS)               \
  V(ReflectHas, NO_EXTRA_ARGUMENTS)                          \
  V(ReflectIsExtensible, NO_EXTRA_ARGUMENTS)                 \
  V(ReflectPreventExtensions, NO_EXTRA_ARGUMENTS)            \
  V(ReflectSet, NO_EXTRA_ARGUMENTS)                          \
  V(ReflectSetPrototypeOf, NO_EXTRA_ARGUMENTS)               \
                                                             \
  V(SymbolConstructor, NO_EXTRA_ARGUMENTS)                   \
  V(SymbolConstructor_ConstructStub, NO_EXTRA_ARGUMENTS)     \
                                                             \
  V(HandleApiCall, NEEDS_CALLED_FUNCTION)                    \
  V(HandleApiCallConstruct, NEEDS_CALLED_FUNCTION)           \
  V(HandleApiCallAsFunction, NO_EXTRA_ARGUMENTS)             \
  V(HandleApiCallAsConstructor, NO_EXTRA_ARGUMENTS)          \
                                                             \
  V(RestrictedFunctionPropertiesThrower, NO_EXTRA_ARGUMENTS) \
  V(RestrictedStrictArgumentsPropertiesThrower, NO_EXTRA_ARGUMENTS)

// Define list of builtins implemented in assembly.
#define BUILTIN_LIST_A(V)                                                     \
  V(ArgumentsAdaptorTrampoline, BUILTIN, UNINITIALIZED, kNoExtraICState)      \
                                                                              \
  V(CallFunction_ReceiverIsNullOrUndefined, BUILTIN, UNINITIALIZED,           \
    kNoExtraICState)                                                          \
  V(CallFunction_ReceiverIsNotNullOrUndefined, BUILTIN, UNINITIALIZED,        \
    kNoExtraICState)                                                          \
  V(CallFunction_ReceiverIsAny, BUILTIN, UNINITIALIZED, kNoExtraICState)      \
  V(Call_ReceiverIsNullOrUndefined, BUILTIN, UNINITIALIZED, kNoExtraICState)  \
  V(Call_ReceiverIsNotNullOrUndefined, BUILTIN, UNINITIALIZED,                \
    kNoExtraICState)                                                          \
  V(Call_ReceiverIsAny, BUILTIN, UNINITIALIZED, kNoExtraICState)              \
                                                                              \
  V(ConstructFunction, BUILTIN, UNINITIALIZED, kNoExtraICState)               \
  V(ConstructProxy, BUILTIN, UNINITIALIZED, kNoExtraICState)                  \
  V(Construct, BUILTIN, UNINITIALIZED, kNoExtraICState)                       \
                                                                              \
  V(InOptimizationQueue, BUILTIN, UNINITIALIZED, kNoExtraICState)             \
  V(JSConstructStubGeneric, BUILTIN, UNINITIALIZED, kNoExtraICState)          \
  V(JSConstructStubForDerived, BUILTIN, UNINITIALIZED, kNoExtraICState)       \
  V(JSConstructStubApi, BUILTIN, UNINITIALIZED, kNoExtraICState)              \
  V(JSEntryTrampoline, BUILTIN, UNINITIALIZED, kNoExtraICState)               \
  V(JSConstructEntryTrampoline, BUILTIN, UNINITIALIZED, kNoExtraICState)      \
  V(CompileLazy, BUILTIN, UNINITIALIZED, kNoExtraICState)                     \
  V(CompileOptimized, BUILTIN, UNINITIALIZED, kNoExtraICState)                \
  V(CompileOptimizedConcurrent, BUILTIN, UNINITIALIZED, kNoExtraICState)      \
  V(NotifyDeoptimized, BUILTIN, UNINITIALIZED, kNoExtraICState)               \
  V(NotifySoftDeoptimized, BUILTIN, UNINITIALIZED, kNoExtraICState)           \
  V(NotifyLazyDeoptimized, BUILTIN, UNINITIALIZED, kNoExtraICState)           \
  V(NotifyStubFailure, BUILTIN, UNINITIALIZED, kNoExtraICState)               \
  V(NotifyStubFailureSaveDoubles, BUILTIN, UNINITIALIZED, kNoExtraICState)    \
                                                                              \
  V(InterpreterEntryTrampoline, BUILTIN, UNINITIALIZED, kNoExtraICState)      \
  V(InterpreterExitTrampoline, BUILTIN, UNINITIALIZED, kNoExtraICState)       \
  V(InterpreterPushArgsAndCall, BUILTIN, UNINITIALIZED, kNoExtraICState)      \
  V(InterpreterPushArgsAndConstruct, BUILTIN, UNINITIALIZED, kNoExtraICState) \
                                                                              \
  V(LoadIC_Miss, BUILTIN, UNINITIALIZED, kNoExtraICState)                     \
  V(KeyedLoadIC_Miss, BUILTIN, UNINITIALIZED, kNoExtraICState)                \
  V(StoreIC_Miss, BUILTIN, UNINITIALIZED, kNoExtraICState)                    \
  V(KeyedStoreIC_Miss, BUILTIN, UNINITIALIZED, kNoExtraICState)               \
  V(LoadIC_Getter_ForDeopt, LOAD_IC, MONOMORPHIC, kNoExtraICState)            \
  V(KeyedLoadIC_Megamorphic, KEYED_LOAD_IC, MEGAMORPHIC, kNoExtraICState)     \
                                                                              \
  V(KeyedLoadIC_Megamorphic_Strong, KEYED_LOAD_IC, MEGAMORPHIC,               \
    LoadICState::kStrongModeState)                                            \
                                                                              \
  V(StoreIC_Setter_ForDeopt, STORE_IC, MONOMORPHIC,                           \
    StoreICState::kStrictModeState)                                           \
                                                                              \
  V(KeyedStoreIC_Initialize, KEYED_STORE_IC, UNINITIALIZED, kNoExtraICState)  \
  V(KeyedStoreIC_PreMonomorphic, KEYED_STORE_IC, PREMONOMORPHIC,              \
    kNoExtraICState)                                                          \
  V(KeyedStoreIC_Megamorphic, KEYED_STORE_IC, MEGAMORPHIC, kNoExtraICState)   \
                                                                              \
  V(KeyedStoreIC_Initialize_Strict, KEYED_STORE_IC, UNINITIALIZED,            \
    StoreICState::kStrictModeState)                                           \
  V(KeyedStoreIC_PreMonomorphic_Strict, KEYED_STORE_IC, PREMONOMORPHIC,       \
    StoreICState::kStrictModeState)                                           \
  V(KeyedStoreIC_Megamorphic_Strict, KEYED_STORE_IC, MEGAMORPHIC,             \
    StoreICState::kStrictModeState)                                           \
                                                                              \
  V(FunctionCall, BUILTIN, UNINITIALIZED, kNoExtraICState)                    \
  V(FunctionApply, BUILTIN, UNINITIALIZED, kNoExtraICState)                   \
  V(ReflectApply, BUILTIN, UNINITIALIZED, kNoExtraICState)                    \
  V(ReflectConstruct, BUILTIN, UNINITIALIZED, kNoExtraICState)                \
                                                                              \
  V(InternalArrayCode, BUILTIN, UNINITIALIZED, kNoExtraICState)               \
  V(ArrayCode, BUILTIN, UNINITIALIZED, kNoExtraICState)                       \
                                                                              \
  V(StringConstructor, BUILTIN, UNINITIALIZED, kNoExtraICState)               \
  V(StringConstructor_ConstructStub, BUILTIN, UNINITIALIZED, kNoExtraICState) \
                                                                              \
  V(OnStackReplacement, BUILTIN, UNINITIALIZED, kNoExtraICState)              \
  V(InterruptCheck, BUILTIN, UNINITIALIZED, kNoExtraICState)                  \
  V(OsrAfterStackCheck, BUILTIN, UNINITIALIZED, kNoExtraICState)              \
  V(StackCheck, BUILTIN, UNINITIALIZED, kNoExtraICState)                      \
                                                                              \
  V(MarkCodeAsToBeExecutedOnce, BUILTIN, UNINITIALIZED, kNoExtraICState)      \
  V(MarkCodeAsExecutedOnce, BUILTIN, UNINITIALIZED, kNoExtraICState)          \
  V(MarkCodeAsExecutedTwice, BUILTIN, UNINITIALIZED, kNoExtraICState)         \
  CODE_AGE_LIST_WITH_ARG(DECLARE_CODE_AGE_BUILTIN, V)

// Define list of builtin handlers implemented in assembly.
#define BUILTIN_LIST_H(V)                    \
  V(LoadIC_Slow,             LOAD_IC)        \
  V(LoadIC_Slow_Strong,      LOAD_IC)        \
  V(KeyedLoadIC_Slow,        KEYED_LOAD_IC)  \
  V(KeyedLoadIC_Slow_Strong, KEYED_LOAD_IC)  \
  V(StoreIC_Slow,            STORE_IC)       \
  V(KeyedStoreIC_Slow,       KEYED_STORE_IC) \
  V(LoadIC_Normal,           LOAD_IC)        \
  V(LoadIC_Normal_Strong,    LOAD_IC)        \
  V(StoreIC_Normal,          STORE_IC)

// Define list of builtins used by the debugger implemented in assembly.
#define BUILTIN_LIST_DEBUG_A(V)                                 \
  V(Return_DebugBreak, BUILTIN, DEBUG_STUB, kNoExtraICState)    \
  V(Slot_DebugBreak, BUILTIN, DEBUG_STUB, kNoExtraICState)      \
  V(PlainReturn_LiveEdit, BUILTIN, DEBUG_STUB, kNoExtraICState) \
  V(FrameDropper_LiveEdit, BUILTIN, DEBUG_STUB, kNoExtraICState)


class BuiltinFunctionTable;
class ObjectVisitor;


class Builtins {
 public:
  ~Builtins();

  // Generate all builtin code objects. Should be called once during
  // isolate initialization.
  void SetUp(Isolate* isolate, bool create_heap_objects);
  void TearDown();

  // Garbage collection support.
  void IterateBuiltins(ObjectVisitor* v);

  // Disassembler support.
  const char* Lookup(byte* pc);

  enum Name {
#define DEF_ENUM_C(name, ignore) k##name,
#define DEF_ENUM_A(name, kind, state, extra) k##name,
#define DEF_ENUM_H(name, kind) k##name,
    BUILTIN_LIST_C(DEF_ENUM_C)
    BUILTIN_LIST_A(DEF_ENUM_A)
    BUILTIN_LIST_H(DEF_ENUM_H)
    BUILTIN_LIST_DEBUG_A(DEF_ENUM_A)
#undef DEF_ENUM_C
#undef DEF_ENUM_A
    builtin_count
  };

  enum CFunctionId {
#define DEF_ENUM_C(name, ignore) c_##name,
    BUILTIN_LIST_C(DEF_ENUM_C)
#undef DEF_ENUM_C
    cfunction_count
  };

#define DECLARE_BUILTIN_ACCESSOR_C(name, ignore) Handle<Code> name();
#define DECLARE_BUILTIN_ACCESSOR_A(name, kind, state, extra) \
  Handle<Code> name();
#define DECLARE_BUILTIN_ACCESSOR_H(name, kind) Handle<Code> name();
  BUILTIN_LIST_C(DECLARE_BUILTIN_ACCESSOR_C)
  BUILTIN_LIST_A(DECLARE_BUILTIN_ACCESSOR_A)
  BUILTIN_LIST_H(DECLARE_BUILTIN_ACCESSOR_H)
  BUILTIN_LIST_DEBUG_A(DECLARE_BUILTIN_ACCESSOR_A)
#undef DECLARE_BUILTIN_ACCESSOR_C
#undef DECLARE_BUILTIN_ACCESSOR_A

  // Convenience wrappers.
  Handle<Code> CallFunction(ConvertReceiverMode = ConvertReceiverMode::kAny);
  Handle<Code> Call(ConvertReceiverMode = ConvertReceiverMode::kAny);

  Code* builtin(Name name) {
    // Code::cast cannot be used here since we access builtins
    // during the marking phase of mark sweep. See IC::Clear.
    return reinterpret_cast<Code*>(builtins_[name]);
  }

  Address builtin_address(Name name) {
    return reinterpret_cast<Address>(&builtins_[name]);
  }

  static Address c_function_address(CFunctionId id) {
    return c_functions_[id];
  }

  const char* name(int index) {
    DCHECK(index >= 0);
    DCHECK(index < builtin_count);
    return names_[index];
  }

  bool is_initialized() const { return initialized_; }

  MUST_USE_RESULT static MaybeHandle<Object> InvokeApiFunction(
      Handle<JSFunction> function, Handle<Object> receiver, int argc,
      Handle<Object> args[]);

 private:
  Builtins();

  // The external C++ functions called from the code.
  static Address const c_functions_[cfunction_count];

  // Note: These are always Code objects, but to conform with
  // IterateBuiltins() above which assumes Object**'s for the callback
  // function f, we use an Object* array here.
  Object* builtins_[builtin_count];
  const char* names_[builtin_count];

  static void Generate_Adaptor(MacroAssembler* masm,
                               CFunctionId id,
                               BuiltinExtraArguments extra_args);
  static void Generate_CompileLazy(MacroAssembler* masm);
  static void Generate_InOptimizationQueue(MacroAssembler* masm);
  static void Generate_CompileOptimized(MacroAssembler* masm);
  static void Generate_CompileOptimizedConcurrent(MacroAssembler* masm);
  static void Generate_JSConstructStubGeneric(MacroAssembler* masm);
  static void Generate_JSConstructStubForDerived(MacroAssembler* masm);
  static void Generate_JSConstructStubApi(MacroAssembler* masm);
  static void Generate_JSEntryTrampoline(MacroAssembler* masm);
  static void Generate_JSConstructEntryTrampoline(MacroAssembler* masm);
  static void Generate_NotifyDeoptimized(MacroAssembler* masm);
  static void Generate_NotifySoftDeoptimized(MacroAssembler* masm);
  static void Generate_NotifyLazyDeoptimized(MacroAssembler* masm);
  static void Generate_NotifyStubFailure(MacroAssembler* masm);
  static void Generate_NotifyStubFailureSaveDoubles(MacroAssembler* masm);
  static void Generate_ArgumentsAdaptorTrampoline(MacroAssembler* masm);

  // ES6 section 9.2.1 [[Call]] ( thisArgument, argumentsList)
  static void Generate_CallFunction(MacroAssembler* masm,
                                    ConvertReceiverMode mode);
  static void Generate_CallFunction_ReceiverIsNullOrUndefined(
      MacroAssembler* masm) {
    Generate_CallFunction(masm, ConvertReceiverMode::kNullOrUndefined);
  }
  static void Generate_CallFunction_ReceiverIsNotNullOrUndefined(
      MacroAssembler* masm) {
    Generate_CallFunction(masm, ConvertReceiverMode::kNotNullOrUndefined);
  }
  static void Generate_CallFunction_ReceiverIsAny(MacroAssembler* masm) {
    Generate_CallFunction(masm, ConvertReceiverMode::kAny);
  }
  // ES6 section 7.3.12 Call(F, V, [argumentsList])
  static void Generate_Call(MacroAssembler* masm, ConvertReceiverMode mode);
  static void Generate_Call_ReceiverIsNullOrUndefined(MacroAssembler* masm) {
    Generate_Call(masm, ConvertReceiverMode::kNullOrUndefined);
  }
  static void Generate_Call_ReceiverIsNotNullOrUndefined(MacroAssembler* masm) {
    Generate_Call(masm, ConvertReceiverMode::kNotNullOrUndefined);
  }
  static void Generate_Call_ReceiverIsAny(MacroAssembler* masm) {
    Generate_Call(masm, ConvertReceiverMode::kAny);
  }

  // ES6 section 9.2.2 [[Construct]] ( argumentsList, newTarget)
  static void Generate_ConstructFunction(MacroAssembler* masm);
  // ES6 section 9.5.14 [[Construct]] ( argumentsList, newTarget)
  static void Generate_ConstructProxy(MacroAssembler* masm);
  // ES6 section 7.3.13 Construct (F, [argumentsList], [newTarget])
  static void Generate_Construct(MacroAssembler* masm);

  static void Generate_FunctionCall(MacroAssembler* masm);
  static void Generate_FunctionApply(MacroAssembler* masm);
  static void Generate_ReflectApply(MacroAssembler* masm);
  static void Generate_ReflectConstruct(MacroAssembler* masm);

  static void Generate_InternalArrayCode(MacroAssembler* masm);
  static void Generate_ArrayCode(MacroAssembler* masm);

  static void Generate_StringConstructor(MacroAssembler* masm);
  static void Generate_StringConstructor_ConstructStub(MacroAssembler* masm);
  static void Generate_OnStackReplacement(MacroAssembler* masm);
  static void Generate_OsrAfterStackCheck(MacroAssembler* masm);
  static void Generate_InterruptCheck(MacroAssembler* masm);
  static void Generate_StackCheck(MacroAssembler* masm);

  static void Generate_InterpreterEntryTrampoline(MacroAssembler* masm);
  static void Generate_InterpreterExitTrampoline(MacroAssembler* masm);
  static void Generate_InterpreterPushArgsAndCall(MacroAssembler* masm);
  static void Generate_InterpreterPushArgsAndConstruct(MacroAssembler* masm);

#define DECLARE_CODE_AGE_BUILTIN_GENERATOR(C)                \
  static void Generate_Make##C##CodeYoungAgainEvenMarking(   \
      MacroAssembler* masm);                                 \
  static void Generate_Make##C##CodeYoungAgainOddMarking(    \
      MacroAssembler* masm);
  CODE_AGE_LIST(DECLARE_CODE_AGE_BUILTIN_GENERATOR)
#undef DECLARE_CODE_AGE_BUILTIN_GENERATOR

  static void Generate_MarkCodeAsToBeExecutedOnce(MacroAssembler* masm);
  static void Generate_MarkCodeAsExecutedOnce(MacroAssembler* masm);
  static void Generate_MarkCodeAsExecutedTwice(MacroAssembler* masm);

  static void InitBuiltinFunctionTable();

  bool initialized_;

  friend class BuiltinFunctionTable;
  friend class Isolate;

  DISALLOW_COPY_AND_ASSIGN(Builtins);
};

}  // namespace internal
}  // namespace v8

#endif  // V8_BUILTINS_H_
