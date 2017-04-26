// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_CODE_STUBS_H_
#define V8_CODE_STUBS_H_

#include "src/allocation.h"
#include "src/assembler.h"
#include "src/codegen.h"
#include "src/compiler/code-stub-assembler.h"
#include "src/globals.h"
#include "src/ic/ic-state.h"
#include "src/interface-descriptors.h"
#include "src/macro-assembler.h"
#include "src/ostreams.h"

namespace v8 {
namespace internal {

// List of code stubs used on all platforms.
#define CODE_STUB_LIST_ALL_PLATFORMS(V)     \
  /* PlatformCodeStubs */                   \
  V(ArrayConstructor)                       \
  V(BinaryOpICWithAllocationSite)           \
  V(CallApiCallback)                        \
  V(CallApiGetter)                          \
  V(CallConstruct)                          \
  V(CallIC)                                 \
  V(CEntry)                                 \
  V(CompareIC)                              \
  V(DoubleToI)                              \
  V(FunctionPrototype)                      \
  V(InstanceOf)                             \
  V(InternalArrayConstructor)               \
  V(JSEntry)                                \
  V(KeyedLoadICTrampoline)                  \
  V(LoadICTrampoline)                       \
  V(CallICTrampoline)                       \
  V(LoadIndexedString)                      \
  V(MathPow)                                \
  V(ProfileEntryHook)                       \
  V(RecordWrite)                            \
  V(RegExpExec)                             \
  V(StoreBufferOverflow)                    \
  V(StoreElement)                           \
  V(StubFailureTrampoline)                  \
  V(SubString)                              \
  V(ToNumber)                               \
  V(NonNumberToNumber)                      \
  V(StringToNumber)                         \
  V(ToString)                               \
  V(ToName)                                 \
  V(ToObject)                               \
  V(VectorStoreICTrampoline)                \
  V(VectorKeyedStoreICTrampoline)           \
  V(VectorStoreIC)                          \
  V(VectorKeyedStoreIC)                     \
  /* HydrogenCodeStubs */                   \
  V(AllocateInNewSpace)                     \
  V(ArrayNArgumentsConstructor)             \
  V(ArrayNoArgumentConstructor)             \
  V(ArraySingleArgumentConstructor)         \
  V(BinaryOpIC)                             \
  V(BinaryOpWithAllocationSite)             \
  V(CreateAllocationSite)                   \
  V(CreateWeakCell)                         \
  V(ElementsTransitionAndStore)             \
  V(FastArrayPush)                          \
  V(FastCloneRegExp)                        \
  V(FastCloneShallowArray)                  \
  V(FastCloneShallowObject)                 \
  V(FastNewClosure)                         \
  V(FastNewContext)                         \
  V(FastNewObject)                          \
  V(FastNewRestParameter)                   \
  V(FastNewSloppyArguments)                 \
  V(FastNewStrictArguments)                 \
  V(GrowArrayElements)                      \
  V(InternalArrayNArgumentsConstructor)     \
  V(InternalArrayNoArgumentConstructor)     \
  V(InternalArraySingleArgumentConstructor) \
  V(KeyedLoadGeneric)                       \
  V(LoadGlobalViaContext)                   \
  V(LoadScriptContextField)                 \
  V(LoadDictionaryElement)                  \
  V(NameDictionaryLookup)                   \
  V(NumberToString)                         \
  V(Typeof)                                 \
  V(RegExpConstructResult)                  \
  V(StoreFastElement)                       \
  V(StoreGlobalViaContext)                  \
  V(StoreScriptContextField)                \
  V(StringAdd)                              \
  V(ToBooleanIC)                            \
  V(TransitionElementsKind)                 \
  V(KeyedLoadIC)                            \
  V(LoadIC)                                 \
  /* TurboFanCodeStubs */                   \
  V(AllocateHeapNumber)                     \
  V(AllocateMutableHeapNumber)              \
  V(AllocateFloat32x4)                      \
  V(AllocateInt32x4)                        \
  V(AllocateUint32x4)                       \
  V(AllocateBool32x4)                       \
  V(AllocateInt16x8)                        \
  V(AllocateUint16x8)                       \
  V(AllocateBool16x8)                       \
  V(AllocateInt8x16)                        \
  V(AllocateUint8x16)                       \
  V(AllocateBool8x16)                       \
  V(StringLength)                           \
  V(Add)                                    \
  V(Subtract)                               \
  V(BitwiseAnd)                             \
  V(BitwiseOr)                              \
  V(BitwiseXor)                             \
  V(LessThan)                               \
  V(LessThanOrEqual)                        \
  V(GreaterThan)                            \
  V(GreaterThanOrEqual)                     \
  V(Equal)                                  \
  V(NotEqual)                               \
  V(StrictEqual)                            \
  V(StrictNotEqual)                         \
  V(StringEqual)                            \
  V(StringNotEqual)                         \
  V(StringLessThan)                         \
  V(StringLessThanOrEqual)                  \
  V(StringGreaterThan)                      \
  V(StringGreaterThanOrEqual)               \
  V(ToBoolean)                              \
  V(ToInteger)                              \
  V(ToLength)                               \
  /* IC Handler stubs */                    \
  V(ArrayBufferViewLoadField)               \
  V(LoadConstant)                           \
  V(LoadFastElement)                        \
  V(LoadField)                              \
  V(LoadIndexedInterceptor)                 \
  V(KeyedLoadSloppyArguments)               \
  V(KeyedStoreSloppyArguments)              \
  V(StoreField)                             \
  V(StoreInterceptor)                       \
  V(StoreGlobal)                            \
  V(StoreTransition)

// List of code stubs only used on ARM 32 bits platforms.
#if V8_TARGET_ARCH_ARM
#define CODE_STUB_LIST_ARM(V) V(DirectCEntry)

#else
#define CODE_STUB_LIST_ARM(V)
#endif

// List of code stubs only used on ARM 64 bits platforms.
#if V8_TARGET_ARCH_ARM64
#define CODE_STUB_LIST_ARM64(V) \
  V(DirectCEntry)               \
  V(RestoreRegistersState)      \
  V(StoreRegistersState)

#else
#define CODE_STUB_LIST_ARM64(V)
#endif

// List of code stubs only used on PPC platforms.
#ifdef V8_TARGET_ARCH_PPC
#define CODE_STUB_LIST_PPC(V) \
  V(DirectCEntry)             \
  V(StoreRegistersState)      \
  V(RestoreRegistersState)
#else
#define CODE_STUB_LIST_PPC(V)
#endif

// List of code stubs only used on MIPS platforms.
#if V8_TARGET_ARCH_MIPS
#define CODE_STUB_LIST_MIPS(V) \
  V(DirectCEntry)              \
  V(RestoreRegistersState)     \
  V(StoreRegistersState)
#elif V8_TARGET_ARCH_MIPS64
#define CODE_STUB_LIST_MIPS(V) \
  V(DirectCEntry)              \
  V(RestoreRegistersState)     \
  V(StoreRegistersState)
#else
#define CODE_STUB_LIST_MIPS(V)
#endif

// List of code stubs only used on S390 platforms.
#ifdef V8_TARGET_ARCH_S390
#define CODE_STUB_LIST_S390(V) \
  V(DirectCEntry)              \
  V(StoreRegistersState)       \
  V(RestoreRegistersState)
#else
#define CODE_STUB_LIST_S390(V)
#endif

// Combined list of code stubs.
#define CODE_STUB_LIST(V)         \
  CODE_STUB_LIST_ALL_PLATFORMS(V) \
  CODE_STUB_LIST_ARM(V)           \
  CODE_STUB_LIST_ARM64(V)         \
  CODE_STUB_LIST_PPC(V)           \
  CODE_STUB_LIST_MIPS(V)          \
  CODE_STUB_LIST_S390(V)

static const int kHasReturnedMinusZeroSentinel = 1;

// Stub is base classes of all stubs.
class CodeStub BASE_EMBEDDED {
 public:
  enum Major {
    // TODO(mvstanton): eliminate the NoCache key by getting rid
    //                  of the non-monomorphic-cache.
    NoCache = 0,  // marker for stubs that do custom caching]
#define DEF_ENUM(name) name,
    CODE_STUB_LIST(DEF_ENUM)
#undef DEF_ENUM
    NUMBER_OF_IDS
  };

  // Retrieve the code for the stub. Generate the code if needed.
  Handle<Code> GetCode();

  // Retrieve the code for the stub, make and return a copy of the code.
  Handle<Code> GetCodeCopy(const Code::FindAndReplacePattern& pattern);

  static Major MajorKeyFromKey(uint32_t key) {
    return static_cast<Major>(MajorKeyBits::decode(key));
  }
  static uint32_t MinorKeyFromKey(uint32_t key) {
    return MinorKeyBits::decode(key);
  }

  // Gets the major key from a code object that is a code stub or binary op IC.
  static Major GetMajorKey(Code* code_stub) {
    return MajorKeyFromKey(code_stub->stub_key());
  }

  static uint32_t NoCacheKey() { return MajorKeyBits::encode(NoCache); }

  static const char* MajorName(Major major_key);

  explicit CodeStub(Isolate* isolate) : minor_key_(0), isolate_(isolate) {}
  virtual ~CodeStub() {}

  static void GenerateStubsAheadOfTime(Isolate* isolate);
  static void GenerateFPStubs(Isolate* isolate);

  // Some stubs put untagged junk on the stack that cannot be scanned by the
  // GC.  This means that we must be statically sure that no GC can occur while
  // they are running.  If that is the case they should override this to return
  // true, which will cause an assertion if we try to call something that can
  // GC or if we try to put a stack frame on top of the junk, which would not
  // result in a traversable stack.
  virtual bool SometimesSetsUpAFrame() { return true; }

  // Lookup the code in the (possibly custom) cache.
  bool FindCodeInCache(Code** code_out);

  virtual CallInterfaceDescriptor GetCallInterfaceDescriptor() const = 0;

  virtual int GetStackParameterCount() const {
    return GetCallInterfaceDescriptor().GetStackParameterCount();
  }

  virtual void InitializeDescriptor(CodeStubDescriptor* descriptor) {}

  static void InitializeDescriptor(Isolate* isolate, uint32_t key,
                                   CodeStubDescriptor* desc);

  static MaybeHandle<Code> GetCode(Isolate* isolate, uint32_t key);

  // Returns information for computing the number key.
  virtual Major MajorKey() const = 0;
  uint32_t MinorKey() const { return minor_key_; }

  // BinaryOpStub needs to override this.
  virtual Code::Kind GetCodeKind() const;

  virtual InlineCacheState GetICState() const { return UNINITIALIZED; }
  virtual ExtraICState GetExtraICState() const { return kNoExtraICState; }
  virtual Code::StubType GetStubType() const { return Code::NORMAL; }

  Code::Flags GetCodeFlags() const;

  friend std::ostream& operator<<(std::ostream& os, const CodeStub& s) {
    s.PrintName(os);
    return os;
  }

  Isolate* isolate() const { return isolate_; }

 protected:
  CodeStub(uint32_t key, Isolate* isolate)
      : minor_key_(MinorKeyFromKey(key)), isolate_(isolate) {}

  // Generates the assembler code for the stub.
  virtual Handle<Code> GenerateCode() = 0;

  // Returns whether the code generated for this stub needs to be allocated as
  // a fixed (non-moveable) code object.
  virtual bool NeedsImmovableCode() { return false; }

  virtual void PrintName(std::ostream& os) const;        // NOLINT
  virtual void PrintBaseName(std::ostream& os) const;    // NOLINT
  virtual void PrintState(std::ostream& os) const { ; }  // NOLINT

  // Computes the key based on major and minor.
  uint32_t GetKey() {
    DCHECK(static_cast<int>(MajorKey()) < NUMBER_OF_IDS);
    return MinorKeyBits::encode(MinorKey()) | MajorKeyBits::encode(MajorKey());
  }

  uint32_t minor_key_;

 private:
  // Perform bookkeeping required after code generation when stub code is
  // initially generated.
  void RecordCodeGeneration(Handle<Code> code);

  // Finish the code object after it has been generated.
  virtual void FinishCode(Handle<Code> code) { }

  // Activate newly generated stub. Is called after
  // registering stub in the stub cache.
  virtual void Activate(Code* code) { }

  // Add the code to a specialized cache, specific to an individual
  // stub type. Please note, this method must add the code object to a
  // roots object, otherwise we will remove the code during GC.
  virtual void AddToSpecialCache(Handle<Code> new_object) { }

  // Find code in a specialized cache, work is delegated to the specific stub.
  virtual bool FindCodeInSpecialCache(Code** code_out) {
    return false;
  }

  // If a stub uses a special cache override this.
  virtual bool UseSpecialCache() { return false; }

  // We use this dispatch to statically instantiate the correct code stub for
  // the given stub key and call the passed function with that code stub.
  typedef void (*DispatchedCall)(CodeStub* stub, void** value_out);
  static void Dispatch(Isolate* isolate, uint32_t key, void** value_out,
                       DispatchedCall call);

  static void GetCodeDispatchCall(CodeStub* stub, void** value_out);

  STATIC_ASSERT(NUMBER_OF_IDS < (1 << kStubMajorKeyBits));
  class MajorKeyBits: public BitField<uint32_t, 0, kStubMajorKeyBits> {};
  class MinorKeyBits: public BitField<uint32_t,
      kStubMajorKeyBits, kStubMinorKeyBits> {};  // NOLINT

  friend class BreakPointIterator;

  Isolate* isolate_;
};


#define DEFINE_CODE_STUB_BASE(NAME, SUPER)                      \
 public:                                                        \
  NAME(uint32_t key, Isolate* isolate) : SUPER(key, isolate) {} \
                                                                \
 private:                                                       \
  DISALLOW_COPY_AND_ASSIGN(NAME)


#define DEFINE_CODE_STUB(NAME, SUPER)                      \
 public:                                                   \
  inline Major MajorKey() const override { return NAME; }; \
                                                           \
 protected:                                                \
  DEFINE_CODE_STUB_BASE(NAME##Stub, SUPER)


#define DEFINE_PLATFORM_CODE_STUB(NAME, SUPER)  \
 private:                                       \
  void Generate(MacroAssembler* masm) override; \
  DEFINE_CODE_STUB(NAME, SUPER)


#define DEFINE_HYDROGEN_CODE_STUB(NAME, SUPER)                        \
 public:                                                              \
  void InitializeDescriptor(CodeStubDescriptor* descriptor) override; \
  Handle<Code> GenerateCode() override;                               \
  DEFINE_CODE_STUB(NAME, SUPER)

#define DEFINE_TURBOFAN_CODE_STUB(NAME, SUPER)                  \
 public:                                                        \
  void GenerateAssembly(compiler::CodeStubAssembler* assembler) \
      const override;                                           \
  DEFINE_CODE_STUB(NAME, SUPER)

#define DEFINE_HANDLER_CODE_STUB(NAME, SUPER) \
 public:                                      \
  Handle<Code> GenerateCode() override;       \
  DEFINE_CODE_STUB(NAME, SUPER)

#define DEFINE_CALL_INTERFACE_DESCRIPTOR(NAME)                          \
 public:                                                                \
  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override { \
    return NAME##Descriptor(isolate());                                 \
  }

// There are some code stubs we just can't describe right now with a
// CallInterfaceDescriptor. Isolate behavior for those cases with this macro.
// An attempt to retrieve a descriptor will fail.
#define DEFINE_NULL_CALL_INTERFACE_DESCRIPTOR()                         \
 public:                                                                \
  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override { \
    UNREACHABLE();                                                      \
    return CallInterfaceDescriptor();                                   \
  }


class PlatformCodeStub : public CodeStub {
 public:
  // Retrieve the code for the stub. Generate the code if needed.
  Handle<Code> GenerateCode() override;

 protected:
  explicit PlatformCodeStub(Isolate* isolate) : CodeStub(isolate) {}

  // Generates the assembler code for the stub.
  virtual void Generate(MacroAssembler* masm) = 0;

  DEFINE_CODE_STUB_BASE(PlatformCodeStub, CodeStub);
};


enum StubFunctionMode { NOT_JS_FUNCTION_STUB_MODE, JS_FUNCTION_STUB_MODE };


class CodeStubDescriptor {
 public:
  explicit CodeStubDescriptor(CodeStub* stub);

  CodeStubDescriptor(Isolate* isolate, uint32_t stub_key);

  void Initialize(Address deoptimization_handler = NULL,
                  int hint_stack_parameter_count = -1,
                  StubFunctionMode function_mode = NOT_JS_FUNCTION_STUB_MODE);
  void Initialize(Register stack_parameter_count,
                  Address deoptimization_handler = NULL,
                  int hint_stack_parameter_count = -1,
                  StubFunctionMode function_mode = NOT_JS_FUNCTION_STUB_MODE);

  void SetMissHandler(ExternalReference handler) {
    miss_handler_ = handler;
    has_miss_handler_ = true;
    // Our miss handler infrastructure doesn't currently support
    // variable stack parameter counts.
    DCHECK(!stack_parameter_count_.is_valid());
  }

  void set_call_descriptor(CallInterfaceDescriptor d) { call_descriptor_ = d; }
  CallInterfaceDescriptor call_descriptor() const { return call_descriptor_; }

  int GetRegisterParameterCount() const {
    return call_descriptor().GetRegisterParameterCount();
  }

  int GetStackParameterCount() const {
    return call_descriptor().GetStackParameterCount();
  }

  int GetParameterCount() const {
    return call_descriptor().GetParameterCount();
  }

  Register GetRegisterParameter(int index) const {
    return call_descriptor().GetRegisterParameter(index);
  }

  Type* GetParameterType(int index) const {
    return call_descriptor().GetParameterType(index);
  }

  ExternalReference miss_handler() const {
    DCHECK(has_miss_handler_);
    return miss_handler_;
  }

  bool has_miss_handler() const {
    return has_miss_handler_;
  }

  int GetHandlerParameterCount() const {
    int params = GetParameterCount();
    if (PassesArgumentsToDeoptimizationHandler()) {
      params += 1;
    }
    return params;
  }

  int hint_stack_parameter_count() const { return hint_stack_parameter_count_; }
  Register stack_parameter_count() const { return stack_parameter_count_; }
  StubFunctionMode function_mode() const { return function_mode_; }
  Address deoptimization_handler() const { return deoptimization_handler_; }

 private:
  bool PassesArgumentsToDeoptimizationHandler() const {
    return stack_parameter_count_.is_valid();
  }

  CallInterfaceDescriptor call_descriptor_;
  Register stack_parameter_count_;
  // If hint_stack_parameter_count_ > 0, the code stub can optimize the
  // return sequence. Default value is -1, which means it is ignored.
  int hint_stack_parameter_count_;
  StubFunctionMode function_mode_;

  Address deoptimization_handler_;

  ExternalReference miss_handler_;
  bool has_miss_handler_;
};


class HydrogenCodeStub : public CodeStub {
 public:
  enum InitializationState {
    UNINITIALIZED,
    INITIALIZED
  };

  template<class SubClass>
  static Handle<Code> GetUninitialized(Isolate* isolate) {
    SubClass::GenerateAheadOfTime(isolate);
    return SubClass().GetCode(isolate);
  }

  // Retrieve the code for the stub. Generate the code if needed.
  Handle<Code> GenerateCode() override = 0;

  bool IsUninitialized() const { return IsMissBits::decode(minor_key_); }

  Handle<Code> GenerateLightweightMissCode(ExternalReference miss);

  template<class StateType>
  void TraceTransition(StateType from, StateType to);

 protected:
  explicit HydrogenCodeStub(Isolate* isolate,
                            InitializationState state = INITIALIZED)
      : CodeStub(isolate) {
    minor_key_ = IsMissBits::encode(state == UNINITIALIZED);
  }

  void set_sub_minor_key(uint32_t key) {
    minor_key_ = SubMinorKeyBits::update(minor_key_, key);
  }

  uint32_t sub_minor_key() const { return SubMinorKeyBits::decode(minor_key_); }

  static const int kSubMinorKeyBits = kStubMinorKeyBits - 1;

 private:
  class IsMissBits : public BitField<bool, kSubMinorKeyBits, 1> {};
  class SubMinorKeyBits : public BitField<int, 0, kSubMinorKeyBits> {};

  void GenerateLightweightMiss(MacroAssembler* masm, ExternalReference miss);

  DEFINE_CODE_STUB_BASE(HydrogenCodeStub, CodeStub);
};


class TurboFanCodeStub : public CodeStub {
 public:
  // Retrieve the code for the stub. Generate the code if needed.
  Handle<Code> GenerateCode() override;

  int GetStackParameterCount() const override {
    return GetCallInterfaceDescriptor().GetStackParameterCount();
  }

  Code::StubType GetStubType() const override { return Code::FAST; }

 protected:
  explicit TurboFanCodeStub(Isolate* isolate) : CodeStub(isolate) {}

  virtual void GenerateAssembly(
      compiler::CodeStubAssembler* assembler) const = 0;

 private:
  DEFINE_CODE_STUB_BASE(TurboFanCodeStub, CodeStub);
};


// Helper interface to prepare to/restore after making runtime calls.
class RuntimeCallHelper {
 public:
  virtual ~RuntimeCallHelper() {}

  virtual void BeforeCall(MacroAssembler* masm) const = 0;

  virtual void AfterCall(MacroAssembler* masm) const = 0;

 protected:
  RuntimeCallHelper() {}

 private:
  DISALLOW_COPY_AND_ASSIGN(RuntimeCallHelper);
};


}  // namespace internal
}  // namespace v8

#if V8_TARGET_ARCH_IA32
#include "src/ia32/code-stubs-ia32.h"
#elif V8_TARGET_ARCH_X64
#include "src/x64/code-stubs-x64.h"
#elif V8_TARGET_ARCH_ARM64
#include "src/arm64/code-stubs-arm64.h"
#elif V8_TARGET_ARCH_ARM
#include "src/arm/code-stubs-arm.h"
#elif V8_TARGET_ARCH_PPC
#include "src/ppc/code-stubs-ppc.h"
#elif V8_TARGET_ARCH_MIPS
#include "src/mips/code-stubs-mips.h"
#elif V8_TARGET_ARCH_MIPS64
#include "src/mips64/code-stubs-mips64.h"
#elif V8_TARGET_ARCH_S390
#include "src/s390/code-stubs-s390.h"
#elif V8_TARGET_ARCH_X87
#include "src/x87/code-stubs-x87.h"
#else
#error Unsupported target architecture.
#endif

namespace v8 {
namespace internal {


// RuntimeCallHelper implementation used in stubs: enters/leaves a
// newly created internal frame before/after the runtime call.
class StubRuntimeCallHelper : public RuntimeCallHelper {
 public:
  StubRuntimeCallHelper() {}

  void BeforeCall(MacroAssembler* masm) const override;

  void AfterCall(MacroAssembler* masm) const override;
};


// Trivial RuntimeCallHelper implementation.
class NopRuntimeCallHelper : public RuntimeCallHelper {
 public:
  NopRuntimeCallHelper() {}

  void BeforeCall(MacroAssembler* masm) const override {}

  void AfterCall(MacroAssembler* masm) const override {}
};


class StringLengthStub : public TurboFanCodeStub {
 public:
  explicit StringLengthStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  Code::Kind GetCodeKind() const override { return Code::HANDLER; }
  InlineCacheState GetICState() const override { return MONOMORPHIC; }
  ExtraICState GetExtraICState() const override { return Code::LOAD_IC; }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(LoadWithVector);
  DEFINE_TURBOFAN_CODE_STUB(StringLength, TurboFanCodeStub);
};

class AddStub final : public TurboFanCodeStub {
 public:
  explicit AddStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(BinaryOp);
  DEFINE_TURBOFAN_CODE_STUB(Add, TurboFanCodeStub);
};

class SubtractStub final : public TurboFanCodeStub {
 public:
  explicit SubtractStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(BinaryOp);
  DEFINE_TURBOFAN_CODE_STUB(Subtract, TurboFanCodeStub);
};

class BitwiseAndStub final : public TurboFanCodeStub {
 public:
  explicit BitwiseAndStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(BinaryOp);
  DEFINE_TURBOFAN_CODE_STUB(BitwiseAnd, TurboFanCodeStub);
};

class BitwiseOrStub final : public TurboFanCodeStub {
 public:
  explicit BitwiseOrStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(BinaryOp);
  DEFINE_TURBOFAN_CODE_STUB(BitwiseOr, TurboFanCodeStub);
};

class BitwiseXorStub final : public TurboFanCodeStub {
 public:
  explicit BitwiseXorStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(BinaryOp);
  DEFINE_TURBOFAN_CODE_STUB(BitwiseXor, TurboFanCodeStub);
};

class LessThanStub final : public TurboFanCodeStub {
 public:
  explicit LessThanStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(LessThan, TurboFanCodeStub);
};

class LessThanOrEqualStub final : public TurboFanCodeStub {
 public:
  explicit LessThanOrEqualStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(LessThanOrEqual, TurboFanCodeStub);
};

class GreaterThanStub final : public TurboFanCodeStub {
 public:
  explicit GreaterThanStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(GreaterThan, TurboFanCodeStub);
};

class GreaterThanOrEqualStub final : public TurboFanCodeStub {
 public:
  explicit GreaterThanOrEqualStub(Isolate* isolate)
      : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(GreaterThanOrEqual, TurboFanCodeStub);
};

class EqualStub final : public TurboFanCodeStub {
 public:
  explicit EqualStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(Equal, TurboFanCodeStub);
};

class NotEqualStub final : public TurboFanCodeStub {
 public:
  explicit NotEqualStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(NotEqual, TurboFanCodeStub);
};

class StrictEqualStub final : public TurboFanCodeStub {
 public:
  explicit StrictEqualStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(StrictEqual, TurboFanCodeStub);
};

class StrictNotEqualStub final : public TurboFanCodeStub {
 public:
  explicit StrictNotEqualStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(StrictNotEqual, TurboFanCodeStub);
};

class StringEqualStub final : public TurboFanCodeStub {
 public:
  explicit StringEqualStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(StringEqual, TurboFanCodeStub);
};

class StringNotEqualStub final : public TurboFanCodeStub {
 public:
  explicit StringNotEqualStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(StringNotEqual, TurboFanCodeStub);
};

class StringLessThanStub final : public TurboFanCodeStub {
 public:
  explicit StringLessThanStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(StringLessThan, TurboFanCodeStub);
};

class StringLessThanOrEqualStub final : public TurboFanCodeStub {
 public:
  explicit StringLessThanOrEqualStub(Isolate* isolate)
      : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(StringLessThanOrEqual, TurboFanCodeStub);
};

class StringGreaterThanStub final : public TurboFanCodeStub {
 public:
  explicit StringGreaterThanStub(Isolate* isolate)
      : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(StringGreaterThan, TurboFanCodeStub);
};

class StringGreaterThanOrEqualStub final : public TurboFanCodeStub {
 public:
  explicit StringGreaterThanOrEqualStub(Isolate* isolate)
      : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Compare);
  DEFINE_TURBOFAN_CODE_STUB(StringGreaterThanOrEqual, TurboFanCodeStub);
};

class ToBooleanStub final : public TurboFanCodeStub {
 public:
  explicit ToBooleanStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_TURBOFAN_CODE_STUB(ToBoolean, TurboFanCodeStub);
};

class ToIntegerStub final : public TurboFanCodeStub {
 public:
  explicit ToIntegerStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_TURBOFAN_CODE_STUB(ToInteger, TurboFanCodeStub);
};

class ToLengthStub final : public TurboFanCodeStub {
 public:
  explicit ToLengthStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_TURBOFAN_CODE_STUB(ToLength, TurboFanCodeStub);
};

class StoreInterceptorStub : public TurboFanCodeStub {
 public:
  explicit StoreInterceptorStub(Isolate* isolate) : TurboFanCodeStub(isolate) {}

  void GenerateAssembly(compiler::CodeStubAssembler* assember) const override;

  Code::Kind GetCodeKind() const override { return Code::HANDLER; }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Store);
  DEFINE_CODE_STUB(StoreInterceptor, TurboFanCodeStub);
};

class LoadIndexedInterceptorStub : public TurboFanCodeStub {
 public:
  explicit LoadIndexedInterceptorStub(Isolate* isolate)
      : TurboFanCodeStub(isolate) {}

  Code::Kind GetCodeKind() const override { return Code::HANDLER; }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(LoadWithVector);
  DEFINE_TURBOFAN_CODE_STUB(LoadIndexedInterceptor, TurboFanCodeStub);
};

enum StringAddFlags {
  // Omit both parameter checks.
  STRING_ADD_CHECK_NONE = 0,
  // Check left parameter.
  STRING_ADD_CHECK_LEFT = 1 << 0,
  // Check right parameter.
  STRING_ADD_CHECK_RIGHT = 1 << 1,
  // Check both parameters.
  STRING_ADD_CHECK_BOTH = STRING_ADD_CHECK_LEFT | STRING_ADD_CHECK_RIGHT,
  // Convert parameters when check fails (instead of throwing an exception).
  STRING_ADD_CONVERT = 1 << 2,
  STRING_ADD_CONVERT_LEFT = STRING_ADD_CHECK_LEFT | STRING_ADD_CONVERT,
  STRING_ADD_CONVERT_RIGHT = STRING_ADD_CHECK_RIGHT | STRING_ADD_CONVERT
};


std::ostream& operator<<(std::ostream& os, const StringAddFlags& flags);


class NumberToStringStub final : public HydrogenCodeStub {
 public:
  explicit NumberToStringStub(Isolate* isolate) : HydrogenCodeStub(isolate) {}

  // Parameters accessed via CodeStubGraphBuilder::GetParameter()
  static const int kNumber = 0;

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_HYDROGEN_CODE_STUB(NumberToString, HydrogenCodeStub);
};


class TypeofStub final : public HydrogenCodeStub {
 public:
  explicit TypeofStub(Isolate* isolate) : HydrogenCodeStub(isolate) {}

  // Parameters accessed via CodeStubGraphBuilder::GetParameter()
  static const int kObject = 0;

  static void GenerateAheadOfTime(Isolate* isolate);

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Typeof);
  DEFINE_HYDROGEN_CODE_STUB(Typeof, HydrogenCodeStub);
};


class FastNewClosureStub : public HydrogenCodeStub {
 public:
  FastNewClosureStub(Isolate* isolate, LanguageMode language_mode,
                     FunctionKind kind)
      : HydrogenCodeStub(isolate) {
    DCHECK(IsValidFunctionKind(kind));
    set_sub_minor_key(LanguageModeBits::encode(language_mode) |
                      FunctionKindBits::encode(kind));
  }

  LanguageMode language_mode() const {
    return LanguageModeBits::decode(sub_minor_key());
  }

  FunctionKind kind() const {
    return FunctionKindBits::decode(sub_minor_key());
  }

 private:
  STATIC_ASSERT(LANGUAGE_END == 3);
  class LanguageModeBits : public BitField<LanguageMode, 0, 2> {};
  class FunctionKindBits : public BitField<FunctionKind, 2, 8> {};

  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastNewClosure);
  DEFINE_HYDROGEN_CODE_STUB(FastNewClosure, HydrogenCodeStub);
};


class FastNewContextStub final : public HydrogenCodeStub {
 public:
  static const int kMaximumSlots = 64;

  FastNewContextStub(Isolate* isolate, int slots) : HydrogenCodeStub(isolate) {
    DCHECK(slots >= 0 && slots <= kMaximumSlots);
    set_sub_minor_key(SlotsBits::encode(slots));
  }

  int slots() const { return SlotsBits::decode(sub_minor_key()); }

  // Parameters accessed via CodeStubGraphBuilder::GetParameter()
  static const int kFunction = 0;

 private:
  class SlotsBits : public BitField<int, 0, 8> {};

  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastNewContext);
  DEFINE_HYDROGEN_CODE_STUB(FastNewContext, HydrogenCodeStub);
};


class FastNewObjectStub final : public PlatformCodeStub {
 public:
  explicit FastNewObjectStub(Isolate* isolate) : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastNewObject);
  DEFINE_PLATFORM_CODE_STUB(FastNewObject, PlatformCodeStub);
};


// TODO(turbofan): This stub should be possible to write in TurboFan
// using the CodeStubAssembler very soon in a way that is as efficient
// and easy as the current handwritten version, which is partly a copy
// of the strict arguments object materialization code.
class FastNewRestParameterStub final : public PlatformCodeStub {
 public:
  explicit FastNewRestParameterStub(Isolate* isolate)
      : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastNewRestParameter);
  DEFINE_PLATFORM_CODE_STUB(FastNewRestParameter, PlatformCodeStub);
};


// TODO(turbofan): This stub should be possible to write in TurboFan
// using the CodeStubAssembler very soon in a way that is as efficient
// and easy as the current handwritten version.
class FastNewSloppyArgumentsStub final : public PlatformCodeStub {
 public:
  explicit FastNewSloppyArgumentsStub(Isolate* isolate)
      : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastNewSloppyArguments);
  DEFINE_PLATFORM_CODE_STUB(FastNewSloppyArguments, PlatformCodeStub);
};


// TODO(turbofan): This stub should be possible to write in TurboFan
// using the CodeStubAssembler very soon in a way that is as efficient
// and easy as the current handwritten version.
class FastNewStrictArgumentsStub final : public PlatformCodeStub {
 public:
  explicit FastNewStrictArgumentsStub(Isolate* isolate)
      : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastNewStrictArguments);
  DEFINE_PLATFORM_CODE_STUB(FastNewStrictArguments, PlatformCodeStub);
};


class FastCloneRegExpStub final : public HydrogenCodeStub {
 public:
  explicit FastCloneRegExpStub(Isolate* isolate) : HydrogenCodeStub(isolate) {}

 private:
  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastCloneRegExp);
  DEFINE_HYDROGEN_CODE_STUB(FastCloneRegExp, HydrogenCodeStub);
};


class FastCloneShallowArrayStub : public HydrogenCodeStub {
 public:
  FastCloneShallowArrayStub(Isolate* isolate,
                            AllocationSiteMode allocation_site_mode)
      : HydrogenCodeStub(isolate) {
    set_sub_minor_key(AllocationSiteModeBits::encode(allocation_site_mode));
  }

  AllocationSiteMode allocation_site_mode() const {
    return AllocationSiteModeBits::decode(sub_minor_key());
  }

 private:
  class AllocationSiteModeBits: public BitField<AllocationSiteMode, 0, 1> {};

  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastCloneShallowArray);
  DEFINE_HYDROGEN_CODE_STUB(FastCloneShallowArray, HydrogenCodeStub);
};


class FastCloneShallowObjectStub : public HydrogenCodeStub {
 public:
  // Maximum number of properties in copied object.
  static const int kMaximumClonedProperties = 6;

  FastCloneShallowObjectStub(Isolate* isolate, int length)
      : HydrogenCodeStub(isolate) {
    DCHECK_GE(length, 0);
    DCHECK_LE(length, kMaximumClonedProperties);
    set_sub_minor_key(LengthBits::encode(length));
  }

  int length() const { return LengthBits::decode(sub_minor_key()); }

 private:
  class LengthBits : public BitField<int, 0, 4> {};

  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastCloneShallowObject);
  DEFINE_HYDROGEN_CODE_STUB(FastCloneShallowObject, HydrogenCodeStub);
};


class CreateAllocationSiteStub : public HydrogenCodeStub {
 public:
  explicit CreateAllocationSiteStub(Isolate* isolate)
      : HydrogenCodeStub(isolate) { }

  static void GenerateAheadOfTime(Isolate* isolate);

  DEFINE_CALL_INTERFACE_DESCRIPTOR(CreateAllocationSite);
  DEFINE_HYDROGEN_CODE_STUB(CreateAllocationSite, HydrogenCodeStub);
};


class CreateWeakCellStub : public HydrogenCodeStub {
 public:
  explicit CreateWeakCellStub(Isolate* isolate) : HydrogenCodeStub(isolate) {}

  static void GenerateAheadOfTime(Isolate* isolate);

  DEFINE_CALL_INTERFACE_DESCRIPTOR(CreateWeakCell);
  DEFINE_HYDROGEN_CODE_STUB(CreateWeakCell, HydrogenCodeStub);
};


class GrowArrayElementsStub : public HydrogenCodeStub {
 public:
  GrowArrayElementsStub(Isolate* isolate, bool is_js_array, ElementsKind kind)
      : HydrogenCodeStub(isolate) {
    set_sub_minor_key(ElementsKindBits::encode(kind) |
                      IsJsArrayBits::encode(is_js_array));
  }

  ElementsKind elements_kind() const {
    return ElementsKindBits::decode(sub_minor_key());
  }

  bool is_js_array() const { return IsJsArrayBits::decode(sub_minor_key()); }

 private:
  class ElementsKindBits : public BitField<ElementsKind, 0, 8> {};
  class IsJsArrayBits : public BitField<bool, ElementsKindBits::kNext, 1> {};

  DEFINE_CALL_INTERFACE_DESCRIPTOR(GrowArrayElements);
  DEFINE_HYDROGEN_CODE_STUB(GrowArrayElements, HydrogenCodeStub);
};

class FastArrayPushStub : public HydrogenCodeStub {
 public:
  explicit FastArrayPushStub(Isolate* isolate) : HydrogenCodeStub(isolate) {}

 private:
  DEFINE_CALL_INTERFACE_DESCRIPTOR(FastArrayPush);
  DEFINE_HYDROGEN_CODE_STUB(FastArrayPush, HydrogenCodeStub);
};

class InstanceOfStub final : public PlatformCodeStub {
 public:
  explicit InstanceOfStub(Isolate* isolate, bool es6_instanceof = false)
      : PlatformCodeStub(isolate) {
    minor_key_ = IsES6InstanceOfBits::encode(es6_instanceof);
  }

  bool is_es6_instanceof() const {
    return IsES6InstanceOfBits::decode(minor_key_);
  }

 private:
  class IsES6InstanceOfBits : public BitField<bool, 0, 1> {};

  DEFINE_CALL_INTERFACE_DESCRIPTOR(InstanceOf);
  DEFINE_PLATFORM_CODE_STUB(InstanceOf, PlatformCodeStub);
};


enum AllocationSiteOverrideMode {
  DONT_OVERRIDE,
  DISABLE_ALLOCATION_SITES,
  LAST_ALLOCATION_SITE_OVERRIDE_MODE = DISABLE_ALLOCATION_SITES
};


class ArrayConstructorStub: public PlatformCodeStub {
 public:
  enum ArgumentCountKey { ANY, NONE, ONE, MORE_THAN_ONE };

  ArrayConstructorStub(Isolate* isolate, int argument_count);

  explicit ArrayConstructorStub(Isolate* isolate);

 private:
  ArgumentCountKey argument_count() const {
    return ArgumentCountBits::decode(minor_key_);
  }

  void GenerateDispatchToArrayStub(MacroAssembler* masm,
                                   AllocationSiteOverrideMode mode);

  void PrintName(std::ostream& os) const override;  // NOLINT

  class ArgumentCountBits : public BitField<ArgumentCountKey, 0, 2> {};

  DEFINE_CALL_INTERFACE_DESCRIPTOR(ArrayConstructor);
  DEFINE_PLATFORM_CODE_STUB(ArrayConstructor, PlatformCodeStub);
};


class InternalArrayConstructorStub: public PlatformCodeStub {
 public:
  explicit InternalArrayConstructorStub(Isolate* isolate);

 private:
  void GenerateCase(MacroAssembler* masm, ElementsKind kind);

  DEFINE_CALL_INTERFACE_DESCRIPTOR(InternalArrayConstructor);
  DEFINE_PLATFORM_CODE_STUB(InternalArrayConstructor, PlatformCodeStub);
};


class MathPowStub: public PlatformCodeStub {
 public:
  enum ExponentType { INTEGER, DOUBLE, TAGGED, ON_STACK };

  MathPowStub(Isolate* isolate, ExponentType exponent_type)
      : PlatformCodeStub(isolate) {
    minor_key_ = ExponentTypeBits::encode(exponent_type);
  }

  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override {
    if (exponent_type() == TAGGED) {
      return MathPowTaggedDescriptor(isolate());
    } else if (exponent_type() == INTEGER) {
      return MathPowIntegerDescriptor(isolate());
    }
    // A CallInterfaceDescriptor doesn't specify double registers (yet).
    return ContextOnlyDescriptor(isolate());
  }

 private:
  ExponentType exponent_type() const {
    return ExponentTypeBits::decode(minor_key_);
  }

  class ExponentTypeBits : public BitField<ExponentType, 0, 2> {};

  DEFINE_PLATFORM_CODE_STUB(MathPow, PlatformCodeStub);
};


class CallICStub: public PlatformCodeStub {
 public:
  CallICStub(Isolate* isolate, const CallICState& state)
      : PlatformCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  Code::Kind GetCodeKind() const override { return Code::CALL_IC; }

  InlineCacheState GetICState() const override { return GENERIC; }

  ExtraICState GetExtraICState() const final {
    return static_cast<ExtraICState>(minor_key_);
  }

 protected:
  int arg_count() const { return state().argc(); }
  ConvertReceiverMode convert_mode() const { return state().convert_mode(); }
  TailCallMode tail_call_mode() const { return state().tail_call_mode(); }

  CallICState state() const {
    return CallICState(static_cast<ExtraICState>(minor_key_));
  }

  // Code generation helpers.
  void GenerateMiss(MacroAssembler* masm);
  void HandleArrayCase(MacroAssembler* masm, Label* miss);

 private:
  void PrintState(std::ostream& os) const override;  // NOLINT

  DEFINE_CALL_INTERFACE_DESCRIPTOR(CallFunctionWithFeedbackAndVector);
  DEFINE_PLATFORM_CODE_STUB(CallIC, PlatformCodeStub);
};


// TODO(verwaest): Translate to hydrogen code stub.
class FunctionPrototypeStub : public PlatformCodeStub {
 public:
  explicit FunctionPrototypeStub(Isolate* isolate)
      : PlatformCodeStub(isolate) {}

  Code::Kind GetCodeKind() const override { return Code::HANDLER; }

  // TODO(mvstanton): only the receiver register is accessed. When this is
  // translated to a hydrogen code stub, a new CallInterfaceDescriptor
  // should be created that just uses that register for more efficient code.
  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override {
    return LoadWithVectorDescriptor(isolate());
  }

  DEFINE_PLATFORM_CODE_STUB(FunctionPrototype, PlatformCodeStub);
};


class LoadIndexedStringStub : public PlatformCodeStub {
 public:
  explicit LoadIndexedStringStub(Isolate* isolate)
      : PlatformCodeStub(isolate) {}

  Code::Kind GetCodeKind() const override { return Code::HANDLER; }
  Code::StubType GetStubType() const override { return Code::FAST; }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Load);
  DEFINE_PLATFORM_CODE_STUB(LoadIndexedString, PlatformCodeStub);
};


class HandlerStub : public HydrogenCodeStub {
 public:
  Code::Kind GetCodeKind() const override { return Code::HANDLER; }
  ExtraICState GetExtraICState() const override { return kind(); }
  InlineCacheState GetICState() const override { return MONOMORPHIC; }

  void InitializeDescriptor(CodeStubDescriptor* descriptor) override;

  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override;

 protected:
  explicit HandlerStub(Isolate* isolate) : HydrogenCodeStub(isolate) {}

  virtual Code::Kind kind() const = 0;

  DEFINE_CODE_STUB_BASE(HandlerStub, HydrogenCodeStub);
};


class LoadFieldStub: public HandlerStub {
 public:
  LoadFieldStub(Isolate* isolate, FieldIndex index) : HandlerStub(isolate) {
    int property_index_key = index.GetFieldAccessStubKey();
    set_sub_minor_key(LoadFieldByIndexBits::encode(property_index_key));
  }

  FieldIndex index() const {
    int property_index_key = LoadFieldByIndexBits::decode(sub_minor_key());
    return FieldIndex::FromFieldAccessStubKey(property_index_key);
  }

 protected:
  Code::Kind kind() const override { return Code::LOAD_IC; }
  Code::StubType GetStubType() const override { return Code::FAST; }

 private:
  class LoadFieldByIndexBits : public BitField<int, 0, 13> {};

  DEFINE_HANDLER_CODE_STUB(LoadField, HandlerStub);
};


class ArrayBufferViewLoadFieldStub : public HandlerStub {
 public:
  ArrayBufferViewLoadFieldStub(Isolate* isolate, FieldIndex index)
      : HandlerStub(isolate) {
    int property_index_key = index.GetFieldAccessStubKey();
    set_sub_minor_key(
        ArrayBufferViewLoadFieldByIndexBits::encode(property_index_key));
  }

  FieldIndex index() const {
    int property_index_key =
        ArrayBufferViewLoadFieldByIndexBits::decode(sub_minor_key());
    return FieldIndex::FromFieldAccessStubKey(property_index_key);
  }

 protected:
  Code::Kind kind() const override { return Code::LOAD_IC; }
  Code::StubType GetStubType() const override { return Code::FAST; }

 private:
  class ArrayBufferViewLoadFieldByIndexBits : public BitField<int, 0, 13> {};

  DEFINE_HANDLER_CODE_STUB(ArrayBufferViewLoadField, HandlerStub);
};


class KeyedLoadSloppyArgumentsStub : public HandlerStub {
 public:
  explicit KeyedLoadSloppyArgumentsStub(Isolate* isolate)
      : HandlerStub(isolate) {}

 protected:
  Code::Kind kind() const override { return Code::KEYED_LOAD_IC; }
  Code::StubType GetStubType() const override { return Code::FAST; }

 private:
  DEFINE_HANDLER_CODE_STUB(KeyedLoadSloppyArguments, HandlerStub);
};


class CommonStoreModeBits : public BitField<KeyedAccessStoreMode, 0, 3> {};

class KeyedStoreSloppyArgumentsStub : public HandlerStub {
 public:
  explicit KeyedStoreSloppyArgumentsStub(Isolate* isolate,
                                         KeyedAccessStoreMode mode)
      : HandlerStub(isolate) {
    set_sub_minor_key(CommonStoreModeBits::encode(mode));
  }

 protected:
  Code::Kind kind() const override { return Code::KEYED_STORE_IC; }
  Code::StubType GetStubType() const override { return Code::FAST; }

 private:
  DEFINE_HANDLER_CODE_STUB(KeyedStoreSloppyArguments, HandlerStub);
};


class LoadConstantStub : public HandlerStub {
 public:
  LoadConstantStub(Isolate* isolate, int constant_index)
      : HandlerStub(isolate) {
    set_sub_minor_key(ConstantIndexBits::encode(constant_index));
  }

  int constant_index() const {
    return ConstantIndexBits::decode(sub_minor_key());
  }

 protected:
  Code::Kind kind() const override { return Code::LOAD_IC; }
  Code::StubType GetStubType() const override { return Code::FAST; }

 private:
  class ConstantIndexBits : public BitField<int, 0, kSubMinorKeyBits> {};

  DEFINE_HANDLER_CODE_STUB(LoadConstant, HandlerStub);
};


class StoreFieldStub : public HandlerStub {
 public:
  StoreFieldStub(Isolate* isolate, FieldIndex index,
                 Representation representation)
      : HandlerStub(isolate) {
    int property_index_key = index.GetFieldAccessStubKey();
    uint8_t repr = PropertyDetails::EncodeRepresentation(representation);
    set_sub_minor_key(StoreFieldByIndexBits::encode(property_index_key) |
                      RepresentationBits::encode(repr));
  }

  FieldIndex index() const {
    int property_index_key = StoreFieldByIndexBits::decode(sub_minor_key());
    return FieldIndex::FromFieldAccessStubKey(property_index_key);
  }

  Representation representation() {
    uint8_t repr = RepresentationBits::decode(sub_minor_key());
    return PropertyDetails::DecodeRepresentation(repr);
  }

 protected:
  Code::Kind kind() const override { return Code::STORE_IC; }
  Code::StubType GetStubType() const override { return Code::FAST; }

 private:
  class StoreFieldByIndexBits : public BitField<int, 0, 13> {};
  class RepresentationBits : public BitField<uint8_t, 13, 4> {};

  DEFINE_HANDLER_CODE_STUB(StoreField, HandlerStub);
};


// Register and parameter access methods are specified here instead of in
// the CallInterfaceDescriptor because the stub uses a different descriptor
// if FLAG_vector_stores is on.
class StoreTransitionHelper {
 public:
  static Register ReceiverRegister() {
    return StoreTransitionDescriptor::ReceiverRegister();
  }

  static Register NameRegister() {
    return StoreTransitionDescriptor::NameRegister();
  }

  static Register ValueRegister() {
    return StoreTransitionDescriptor::ValueRegister();
  }

  static Register SlotRegister() {
    return VectorStoreTransitionDescriptor::SlotRegister();
  }

  static Register VectorRegister() {
    return VectorStoreTransitionDescriptor::VectorRegister();
  }

  static Register MapRegister() {
    return VectorStoreTransitionDescriptor::MapRegister();
  }

  static int ReceiverIndex() {
    return StoreTransitionDescriptor::kReceiverIndex;
  }

  static int NameIndex() { return StoreTransitionDescriptor::kReceiverIndex; }

  static int ValueIndex() { return StoreTransitionDescriptor::kValueIndex; }

  static int MapIndex() {
    DCHECK(static_cast<int>(VectorStoreTransitionDescriptor::kMapIndex) ==
           static_cast<int>(StoreTransitionDescriptor::kMapIndex));
    return StoreTransitionDescriptor::kMapIndex;
  }

  static int VectorIndex() {
    if (HasVirtualSlotArg()) {
      return VectorStoreTransitionDescriptor::kVirtualSlotVectorIndex;
    }
    return VectorStoreTransitionDescriptor::kVectorIndex;
  }

  // Some platforms don't have a slot arg.
  static bool HasVirtualSlotArg() {
    return SlotRegister().is(no_reg);
  }
};


class StoreTransitionStub : public HandlerStub {
 public:
  enum StoreMode {
    StoreMapOnly,
    StoreMapAndValue,
    ExtendStorageAndStoreMapAndValue
  };

  explicit StoreTransitionStub(Isolate* isolate) : HandlerStub(isolate) {
    set_sub_minor_key(StoreModeBits::encode(StoreMapOnly));
  }

  StoreTransitionStub(Isolate* isolate, FieldIndex index,
                      Representation representation, StoreMode store_mode)
      : HandlerStub(isolate) {
    DCHECK(store_mode != StoreMapOnly);
    int property_index_key = index.GetFieldAccessStubKey();
    uint8_t repr = PropertyDetails::EncodeRepresentation(representation);
    set_sub_minor_key(StoreFieldByIndexBits::encode(property_index_key) |
                      RepresentationBits::encode(repr) |
                      StoreModeBits::encode(store_mode));
  }

  FieldIndex index() const {
    DCHECK(store_mode() != StoreMapOnly);
    int property_index_key = StoreFieldByIndexBits::decode(sub_minor_key());
    return FieldIndex::FromFieldAccessStubKey(property_index_key);
  }

  Representation representation() {
    DCHECK(store_mode() != StoreMapOnly);
    uint8_t repr = RepresentationBits::decode(sub_minor_key());
    return PropertyDetails::DecodeRepresentation(repr);
  }

  StoreMode store_mode() const {
    return StoreModeBits::decode(sub_minor_key());
  }

  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override;

 protected:
  Code::Kind kind() const override { return Code::STORE_IC; }
  Code::StubType GetStubType() const override { return Code::FAST; }

 private:
  class StoreFieldByIndexBits : public BitField<int, 0, 13> {};
  class RepresentationBits : public BitField<uint8_t, 13, 4> {};
  class StoreModeBits : public BitField<StoreMode, 17, 2> {};

  DEFINE_HANDLER_CODE_STUB(StoreTransition, HandlerStub);
};


class StoreGlobalStub : public HandlerStub {
 public:
  StoreGlobalStub(Isolate* isolate, PropertyCellType type,
                  Maybe<PropertyCellConstantType> constant_type,
                  bool check_global)
      : HandlerStub(isolate) {
    PropertyCellConstantType encoded_constant_type =
        constant_type.FromMaybe(PropertyCellConstantType::kSmi);
    set_sub_minor_key(CellTypeBits::encode(type) |
                      ConstantTypeBits::encode(encoded_constant_type) |
                      CheckGlobalBits::encode(check_global));
  }

  static Handle<HeapObject> property_cell_placeholder(Isolate* isolate) {
    return isolate->factory()->uninitialized_value();
  }

  static Handle<HeapObject> global_map_placeholder(Isolate* isolate) {
    return isolate->factory()->termination_exception();
  }

  Handle<Code> GetCodeCopyFromTemplate(Handle<JSGlobalObject> global,
                                       Handle<PropertyCell> cell) {
    Code::FindAndReplacePattern pattern;
    if (check_global()) {
      pattern.Add(handle(global_map_placeholder(isolate())->map()),
                  Map::WeakCellForMap(Handle<Map>(global->map())));
    }
    pattern.Add(handle(property_cell_placeholder(isolate())->map()),
                isolate()->factory()->NewWeakCell(cell));
    return CodeStub::GetCodeCopy(pattern);
  }

  Code::Kind kind() const override { return Code::STORE_IC; }

  PropertyCellType cell_type() const {
    return CellTypeBits::decode(sub_minor_key());
  }

  PropertyCellConstantType constant_type() const {
    DCHECK(PropertyCellType::kConstantType == cell_type());
    return ConstantTypeBits::decode(sub_minor_key());
  }

  bool check_global() const { return CheckGlobalBits::decode(sub_minor_key()); }

  Representation representation() {
    return Representation::FromKind(
        RepresentationBits::decode(sub_minor_key()));
  }

  void set_representation(Representation r) {
    set_sub_minor_key(RepresentationBits::update(sub_minor_key(), r.kind()));
  }

 private:
  class CellTypeBits : public BitField<PropertyCellType, 0, 2> {};
  class ConstantTypeBits : public BitField<PropertyCellConstantType, 2, 2> {};
  class RepresentationBits : public BitField<Representation::Kind, 4, 8> {};
  class CheckGlobalBits : public BitField<bool, 12, 1> {};

  DEFINE_HANDLER_CODE_STUB(StoreGlobal, HandlerStub);
};


class LoadGlobalViaContextStub final : public PlatformCodeStub {
 public:
  static const int kMaximumDepth = 15;

  LoadGlobalViaContextStub(Isolate* isolate, int depth)
      : PlatformCodeStub(isolate) {
    minor_key_ = DepthBits::encode(depth);
  }

  int depth() const { return DepthBits::decode(minor_key_); }

 private:
  class DepthBits : public BitField<int, 0, 4> {};
  STATIC_ASSERT(DepthBits::kMax == kMaximumDepth);

  DEFINE_CALL_INTERFACE_DESCRIPTOR(LoadGlobalViaContext);
  DEFINE_PLATFORM_CODE_STUB(LoadGlobalViaContext, PlatformCodeStub);
};


class StoreGlobalViaContextStub final : public PlatformCodeStub {
 public:
  static const int kMaximumDepth = 15;

  StoreGlobalViaContextStub(Isolate* isolate, int depth,
                            LanguageMode language_mode)
      : PlatformCodeStub(isolate) {
    minor_key_ =
        DepthBits::encode(depth) | LanguageModeBits::encode(language_mode);
  }

  int depth() const { return DepthBits::decode(minor_key_); }
  LanguageMode language_mode() const {
    return LanguageModeBits::decode(minor_key_);
  }

 private:
  class DepthBits : public BitField<int, 0, 4> {};
  STATIC_ASSERT(DepthBits::kMax == kMaximumDepth);
  class LanguageModeBits : public BitField<LanguageMode, 4, 2> {};
  STATIC_ASSERT(LANGUAGE_END == 3);

  DEFINE_CALL_INTERFACE_DESCRIPTOR(StoreGlobalViaContext);
  DEFINE_PLATFORM_CODE_STUB(StoreGlobalViaContext, PlatformCodeStub);
};

class CallApiCallbackStub : public PlatformCodeStub {
 public:
  static const int kArgBits = 3;
  static const int kArgMax = (1 << kArgBits) - 1;

  // CallApiCallbackStub for regular setters and getters.
  CallApiCallbackStub(Isolate* isolate, bool is_store, bool call_data_undefined,
                      bool is_lazy)
      : CallApiCallbackStub(isolate, is_store ? 1 : 0, is_store,
                            call_data_undefined, is_lazy) {}

  // CallApiCallbackStub for callback functions.
  CallApiCallbackStub(Isolate* isolate, int argc, bool call_data_undefined)
      : CallApiCallbackStub(isolate, argc, false, call_data_undefined, false) {}

  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override {
    return ApiCallbackDescriptorBase::ForArgs(isolate(), argc());
  }

 private:
  CallApiCallbackStub(Isolate* isolate, int argc, bool is_store,
                      bool call_data_undefined, bool is_lazy)
      : PlatformCodeStub(isolate) {
    CHECK(0 <= argc && argc <= kArgMax);
    minor_key_ = IsStoreBits::encode(is_store) |
                 CallDataUndefinedBits::encode(call_data_undefined) |
                 ArgumentBits::encode(argc) |
                 IsLazyAccessorBits::encode(is_lazy);
  }

  bool is_store() const { return IsStoreBits::decode(minor_key_); }
  bool is_lazy() const { return IsLazyAccessorBits::decode(minor_key_); }
  bool call_data_undefined() const {
    return CallDataUndefinedBits::decode(minor_key_);
  }
  int argc() const { return ArgumentBits::decode(minor_key_); }

  class IsStoreBits: public BitField<bool, 0, 1> {};
  class CallDataUndefinedBits: public BitField<bool, 1, 1> {};
  class ArgumentBits : public BitField<int, 2, kArgBits> {};
  class IsLazyAccessorBits : public BitField<bool, 3 + kArgBits, 1> {};

  DEFINE_PLATFORM_CODE_STUB(CallApiCallback, PlatformCodeStub);
};


class CallApiGetterStub : public PlatformCodeStub {
 public:
  explicit CallApiGetterStub(Isolate* isolate) : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(ApiGetter);
  DEFINE_PLATFORM_CODE_STUB(CallApiGetter, PlatformCodeStub);
};


class BinaryOpICStub : public HydrogenCodeStub {
 public:
  BinaryOpICStub(Isolate* isolate, Token::Value op)
      : HydrogenCodeStub(isolate, UNINITIALIZED) {
    BinaryOpICState state(isolate, op);
    set_sub_minor_key(state.GetExtraICState());
  }

  BinaryOpICStub(Isolate* isolate, const BinaryOpICState& state)
      : HydrogenCodeStub(isolate) {
    set_sub_minor_key(state.GetExtraICState());
  }

  static void GenerateAheadOfTime(Isolate* isolate);

  Code::Kind GetCodeKind() const override { return Code::BINARY_OP_IC; }

  InlineCacheState GetICState() const final { return state().GetICState(); }

  ExtraICState GetExtraICState() const final {
    return static_cast<ExtraICState>(sub_minor_key());
  }

  BinaryOpICState state() const {
    return BinaryOpICState(isolate(), GetExtraICState());
  }

  void PrintState(std::ostream& os) const final;  // NOLINT

  // Parameters accessed via CodeStubGraphBuilder::GetParameter()
  static const int kLeft = 0;
  static const int kRight = 1;

 private:
  static void GenerateAheadOfTime(Isolate* isolate,
                                  const BinaryOpICState& state);

  DEFINE_CALL_INTERFACE_DESCRIPTOR(BinaryOp);
  DEFINE_HYDROGEN_CODE_STUB(BinaryOpIC, HydrogenCodeStub);
};


// TODO(bmeurer): Merge this into the BinaryOpICStub once we have proper tail
// call support for stubs in Hydrogen.
class BinaryOpICWithAllocationSiteStub final : public PlatformCodeStub {
 public:
  BinaryOpICWithAllocationSiteStub(Isolate* isolate,
                                   const BinaryOpICState& state)
      : PlatformCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  static void GenerateAheadOfTime(Isolate* isolate);

  Handle<Code> GetCodeCopyFromTemplate(Handle<AllocationSite> allocation_site) {
    Code::FindAndReplacePattern pattern;
    pattern.Add(isolate()->factory()->undefined_map(), allocation_site);
    return CodeStub::GetCodeCopy(pattern);
  }

  Code::Kind GetCodeKind() const override { return Code::BINARY_OP_IC; }

  InlineCacheState GetICState() const override { return state().GetICState(); }

  ExtraICState GetExtraICState() const override {
    return static_cast<ExtraICState>(minor_key_);
  }

  void PrintState(std::ostream& os) const override;  // NOLINT

 private:
  BinaryOpICState state() const {
    return BinaryOpICState(isolate(), static_cast<ExtraICState>(minor_key_));
  }

  static void GenerateAheadOfTime(Isolate* isolate,
                                  const BinaryOpICState& state);

  DEFINE_CALL_INTERFACE_DESCRIPTOR(BinaryOpWithAllocationSite);
  DEFINE_PLATFORM_CODE_STUB(BinaryOpICWithAllocationSite, PlatformCodeStub);
};


class BinaryOpWithAllocationSiteStub final : public BinaryOpICStub {
 public:
  BinaryOpWithAllocationSiteStub(Isolate* isolate, Token::Value op)
      : BinaryOpICStub(isolate, op) {}

  BinaryOpWithAllocationSiteStub(Isolate* isolate, const BinaryOpICState& state)
      : BinaryOpICStub(isolate, state) {}

  Code::Kind GetCodeKind() const final { return Code::STUB; }

  // Parameters accessed via CodeStubGraphBuilder::GetParameter()
  static const int kAllocationSite = 0;
  static const int kLeft = 1;
  static const int kRight = 2;

  DEFINE_CALL_INTERFACE_DESCRIPTOR(BinaryOpWithAllocationSite);
  DEFINE_HYDROGEN_CODE_STUB(BinaryOpWithAllocationSite, BinaryOpICStub);
};


class StringAddStub final : public HydrogenCodeStub {
 public:
  StringAddStub(Isolate* isolate, StringAddFlags flags,
                PretenureFlag pretenure_flag)
      : HydrogenCodeStub(isolate) {
    set_sub_minor_key(StringAddFlagsBits::encode(flags) |
                      PretenureFlagBits::encode(pretenure_flag));
  }

  StringAddFlags flags() const {
    return StringAddFlagsBits::decode(sub_minor_key());
  }

  PretenureFlag pretenure_flag() const {
    return PretenureFlagBits::decode(sub_minor_key());
  }

  // Parameters accessed via CodeStubGraphBuilder::GetParameter()
  static const int kLeft = 0;
  static const int kRight = 1;

 private:
  class StringAddFlagsBits : public BitField<StringAddFlags, 0, 3> {};
  class PretenureFlagBits : public BitField<PretenureFlag, 3, 1> {};

  void PrintBaseName(std::ostream& os) const override;  // NOLINT

  DEFINE_CALL_INTERFACE_DESCRIPTOR(StringAdd);
  DEFINE_HYDROGEN_CODE_STUB(StringAdd, HydrogenCodeStub);
};


class CompareICStub : public PlatformCodeStub {
 public:
  CompareICStub(Isolate* isolate, Token::Value op, CompareICState::State left,
                CompareICState::State right, CompareICState::State state)
      : PlatformCodeStub(isolate) {
    DCHECK(Token::IsCompareOp(op));
    minor_key_ = OpBits::encode(op - Token::EQ) |
                 LeftStateBits::encode(left) | RightStateBits::encode(right) |
                 StateBits::encode(state);
  }

  void set_known_map(Handle<Map> map) { known_map_ = map; }

  InlineCacheState GetICState() const override;

  Token::Value op() const {
    return static_cast<Token::Value>(Token::EQ + OpBits::decode(minor_key_));
  }

  CompareICState::State left() const {
    return LeftStateBits::decode(minor_key_);
  }
  CompareICState::State right() const {
    return RightStateBits::decode(minor_key_);
  }
  CompareICState::State state() const { return StateBits::decode(minor_key_); }

 private:
  Code::Kind GetCodeKind() const override { return Code::COMPARE_IC; }

  void GenerateBooleans(MacroAssembler* masm);
  void GenerateSmis(MacroAssembler* masm);
  void GenerateNumbers(MacroAssembler* masm);
  void GenerateInternalizedStrings(MacroAssembler* masm);
  void GenerateStrings(MacroAssembler* masm);
  void GenerateUniqueNames(MacroAssembler* masm);
  void GenerateReceivers(MacroAssembler* masm);
  void GenerateMiss(MacroAssembler* masm);
  void GenerateKnownReceivers(MacroAssembler* masm);
  void GenerateGeneric(MacroAssembler* masm);

  bool strict() const { return op() == Token::EQ_STRICT; }
  Condition GetCondition() const;

  void AddToSpecialCache(Handle<Code> new_object) override;
  bool FindCodeInSpecialCache(Code** code_out) override;
  bool UseSpecialCache() override {
    return state() == CompareICState::KNOWN_RECEIVER;
  }

  class OpBits : public BitField<int, 0, 3> {};
  class LeftStateBits : public BitField<CompareICState::State, 3, 4> {};
  class RightStateBits : public BitField<CompareICState::State, 7, 4> {};
  class StateBits : public BitField<CompareICState::State, 11, 4> {};

  Handle<Map> known_map_;

  DEFINE_CALL_INTERFACE_DESCRIPTOR(BinaryOp);
  DEFINE_PLATFORM_CODE_STUB(CompareIC, PlatformCodeStub);
};


class CEntryStub : public PlatformCodeStub {
 public:
  CEntryStub(Isolate* isolate, int result_size,
             SaveFPRegsMode save_doubles = kDontSaveFPRegs,
             ArgvMode argv_mode = kArgvOnStack)
      : PlatformCodeStub(isolate) {
    minor_key_ = SaveDoublesBits::encode(save_doubles == kSaveFPRegs) |
                 ArgvMode::encode(argv_mode == kArgvInRegister);
    DCHECK(result_size == 1 || result_size == 2 || result_size == 3);
    minor_key_ = ResultSizeBits::update(minor_key_, result_size);
  }

  // The version of this stub that doesn't save doubles is generated ahead of
  // time, so it's OK to call it from other stubs that can't cope with GC during
  // their code generation.  On machines that always have gp registers (x64) we
  // can generate both variants ahead of time.
  static void GenerateAheadOfTime(Isolate* isolate);

 private:
  bool save_doubles() const { return SaveDoublesBits::decode(minor_key_); }
  bool argv_in_register() const { return ArgvMode::decode(minor_key_); }
  int result_size() const { return ResultSizeBits::decode(minor_key_); }

  bool NeedsImmovableCode() override;

  class SaveDoublesBits : public BitField<bool, 0, 1> {};
  class ArgvMode : public BitField<bool, 1, 1> {};
  class ResultSizeBits : public BitField<int, 2, 3> {};

  DEFINE_NULL_CALL_INTERFACE_DESCRIPTOR();
  DEFINE_PLATFORM_CODE_STUB(CEntry, PlatformCodeStub);
};


class JSEntryStub : public PlatformCodeStub {
 public:
  JSEntryStub(Isolate* isolate, StackFrame::Type type)
      : PlatformCodeStub(isolate) {
    DCHECK(type == StackFrame::ENTRY || type == StackFrame::ENTRY_CONSTRUCT);
    minor_key_ = StackFrameTypeBits::encode(type);
  }

 private:
  void FinishCode(Handle<Code> code) override;

  void PrintName(std::ostream& os) const override {  // NOLINT
    os << (type() == StackFrame::ENTRY ? "JSEntryStub"
                                       : "JSConstructEntryStub");
  }

  StackFrame::Type type() const {
    return StackFrameTypeBits::decode(minor_key_);
  }

  class StackFrameTypeBits : public BitField<StackFrame::Type, 0, 5> {};

  int handler_offset_;

  DEFINE_NULL_CALL_INTERFACE_DESCRIPTOR();
  DEFINE_PLATFORM_CODE_STUB(JSEntry, PlatformCodeStub);
};


class RegExpExecStub: public PlatformCodeStub {
 public:
  explicit RegExpExecStub(Isolate* isolate) : PlatformCodeStub(isolate) { }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(ContextOnly);
  DEFINE_PLATFORM_CODE_STUB(RegExpExec, PlatformCodeStub);
};


class RegExpConstructResultStub final : public HydrogenCodeStub {
 public:
  explicit RegExpConstructResultStub(Isolate* isolate)
      : HydrogenCodeStub(isolate) { }

  // Parameters accessed via CodeStubGraphBuilder::GetParameter()
  static const int kLength = 0;
  static const int kIndex = 1;
  static const int kInput = 2;

  DEFINE_CALL_INTERFACE_DESCRIPTOR(RegExpConstructResult);
  DEFINE_HYDROGEN_CODE_STUB(RegExpConstructResult, HydrogenCodeStub);
};


// TODO(bmeurer/mvstanton): Turn CallConstructStub into ConstructICStub.
class CallConstructStub final : public PlatformCodeStub {
 public:
  explicit CallConstructStub(Isolate* isolate) : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(CallConstruct);
  DEFINE_PLATFORM_CODE_STUB(CallConstruct, PlatformCodeStub);
};


enum StringIndexFlags {
  // Accepts smis or heap numbers.
  STRING_INDEX_IS_NUMBER,

  // Accepts smis or heap numbers that are valid array indices
  // (ECMA-262 15.4). Invalid indices are reported as being out of
  // range.
  STRING_INDEX_IS_ARRAY_INDEX
};


enum ReceiverCheckMode {
  // We don't know anything about the receiver.
  RECEIVER_IS_UNKNOWN,

  // We know the receiver is a string.
  RECEIVER_IS_STRING
};


enum EmbedMode {
  // The code being generated is part of an IC handler, which may MISS
  // to an IC in failure cases.
  PART_OF_IC_HANDLER,

  NOT_PART_OF_IC_HANDLER
};


// Generates code implementing String.prototype.charCodeAt.
//
// Only supports the case when the receiver is a string and the index
// is a number (smi or heap number) that is a valid index into the
// string. Additional index constraints are specified by the
// flags. Otherwise, bails out to the provided labels.
//
// Register usage: |object| may be changed to another string in a way
// that doesn't affect charCodeAt/charAt semantics, |index| is
// preserved, |scratch| and |result| are clobbered.
class StringCharCodeAtGenerator {
 public:
  StringCharCodeAtGenerator(Register object, Register index, Register result,
                            Label* receiver_not_string, Label* index_not_number,
                            Label* index_out_of_range,
                            StringIndexFlags index_flags,
                            ReceiverCheckMode check_mode = RECEIVER_IS_UNKNOWN)
      : object_(object),
        index_(index),
        result_(result),
        receiver_not_string_(receiver_not_string),
        index_not_number_(index_not_number),
        index_out_of_range_(index_out_of_range),
        index_flags_(index_flags),
        check_mode_(check_mode) {
    DCHECK(!result_.is(object_));
    DCHECK(!result_.is(index_));
  }

  // Generates the fast case code. On the fallthrough path |result|
  // register contains the result.
  void GenerateFast(MacroAssembler* masm);

  // Generates the slow case code. Must not be naturally
  // reachable. Expected to be put after a ret instruction (e.g., in
  // deferred code). Always jumps back to the fast case.
  void GenerateSlow(MacroAssembler* masm, EmbedMode embed_mode,
                    const RuntimeCallHelper& call_helper);

  // Skip handling slow case and directly jump to bailout.
  void SkipSlow(MacroAssembler* masm, Label* bailout) {
    masm->bind(&index_not_smi_);
    masm->bind(&call_runtime_);
    masm->jmp(bailout);
  }

 private:
  Register object_;
  Register index_;
  Register result_;

  Label* receiver_not_string_;
  Label* index_not_number_;
  Label* index_out_of_range_;

  StringIndexFlags index_flags_;
  ReceiverCheckMode check_mode_;

  Label call_runtime_;
  Label index_not_smi_;
  Label got_smi_index_;
  Label exit_;

  DISALLOW_COPY_AND_ASSIGN(StringCharCodeAtGenerator);
};


// Generates code for creating a one-char string from a char code.
class StringCharFromCodeGenerator {
 public:
  StringCharFromCodeGenerator(Register code,
                              Register result)
      : code_(code),
        result_(result) {
    DCHECK(!code_.is(result_));
  }

  // Generates the fast case code. On the fallthrough path |result|
  // register contains the result.
  void GenerateFast(MacroAssembler* masm);

  // Generates the slow case code. Must not be naturally
  // reachable. Expected to be put after a ret instruction (e.g., in
  // deferred code). Always jumps back to the fast case.
  void GenerateSlow(MacroAssembler* masm,
                    const RuntimeCallHelper& call_helper);

  // Skip handling slow case and directly jump to bailout.
  void SkipSlow(MacroAssembler* masm, Label* bailout) {
    masm->bind(&slow_case_);
    masm->jmp(bailout);
  }

 private:
  Register code_;
  Register result_;

  Label slow_case_;
  Label exit_;

  DISALLOW_COPY_AND_ASSIGN(StringCharFromCodeGenerator);
};


// Generates code implementing String.prototype.charAt.
//
// Only supports the case when the receiver is a string and the index
// is a number (smi or heap number) that is a valid index into the
// string. Additional index constraints are specified by the
// flags. Otherwise, bails out to the provided labels.
//
// Register usage: |object| may be changed to another string in a way
// that doesn't affect charCodeAt/charAt semantics, |index| is
// preserved, |scratch1|, |scratch2|, and |result| are clobbered.
class StringCharAtGenerator {
 public:
  StringCharAtGenerator(Register object, Register index, Register scratch,
                        Register result, Label* receiver_not_string,
                        Label* index_not_number, Label* index_out_of_range,
                        StringIndexFlags index_flags,
                        ReceiverCheckMode check_mode = RECEIVER_IS_UNKNOWN)
      : char_code_at_generator_(object, index, scratch, receiver_not_string,
                                index_not_number, index_out_of_range,
                                index_flags, check_mode),
        char_from_code_generator_(scratch, result) {}

  // Generates the fast case code. On the fallthrough path |result|
  // register contains the result.
  void GenerateFast(MacroAssembler* masm) {
    char_code_at_generator_.GenerateFast(masm);
    char_from_code_generator_.GenerateFast(masm);
  }

  // Generates the slow case code. Must not be naturally
  // reachable. Expected to be put after a ret instruction (e.g., in
  // deferred code). Always jumps back to the fast case.
  void GenerateSlow(MacroAssembler* masm, EmbedMode embed_mode,
                    const RuntimeCallHelper& call_helper) {
    char_code_at_generator_.GenerateSlow(masm, embed_mode, call_helper);
    char_from_code_generator_.GenerateSlow(masm, call_helper);
  }

  // Skip handling slow case and directly jump to bailout.
  void SkipSlow(MacroAssembler* masm, Label* bailout) {
    char_code_at_generator_.SkipSlow(masm, bailout);
    char_from_code_generator_.SkipSlow(masm, bailout);
  }

 private:
  StringCharCodeAtGenerator char_code_at_generator_;
  StringCharFromCodeGenerator char_from_code_generator_;

  DISALLOW_COPY_AND_ASSIGN(StringCharAtGenerator);
};


class LoadDictionaryElementStub : public HydrogenCodeStub {
 public:
  explicit LoadDictionaryElementStub(Isolate* isolate, const LoadICState& state)
      : HydrogenCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override {
    return LoadWithVectorDescriptor(isolate());
  }

  DEFINE_HYDROGEN_CODE_STUB(LoadDictionaryElement, HydrogenCodeStub);
};


class KeyedLoadGenericStub : public HydrogenCodeStub {
 public:
  explicit KeyedLoadGenericStub(Isolate* isolate, const LoadICState& state)
      : HydrogenCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  Code::Kind GetCodeKind() const override { return Code::KEYED_LOAD_IC; }
  InlineCacheState GetICState() const override { return GENERIC; }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Load);

  DEFINE_HYDROGEN_CODE_STUB(KeyedLoadGeneric, HydrogenCodeStub);
};


class LoadICTrampolineStub : public PlatformCodeStub {
 public:
  LoadICTrampolineStub(Isolate* isolate, const LoadICState& state)
      : PlatformCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  Code::Kind GetCodeKind() const override { return Code::LOAD_IC; }

  InlineCacheState GetICState() const final { return GENERIC; }

  ExtraICState GetExtraICState() const final {
    return static_cast<ExtraICState>(minor_key_);
  }

 protected:
  LoadICState state() const {
    return LoadICState(static_cast<ExtraICState>(minor_key_));
  }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(Load);
  DEFINE_PLATFORM_CODE_STUB(LoadICTrampoline, PlatformCodeStub);
};


class KeyedLoadICTrampolineStub : public LoadICTrampolineStub {
 public:
  explicit KeyedLoadICTrampolineStub(Isolate* isolate, const LoadICState& state)
      : LoadICTrampolineStub(isolate, state) {}

  Code::Kind GetCodeKind() const override { return Code::KEYED_LOAD_IC; }

  DEFINE_PLATFORM_CODE_STUB(KeyedLoadICTrampoline, LoadICTrampolineStub);
};


class VectorStoreICTrampolineStub : public PlatformCodeStub {
 public:
  VectorStoreICTrampolineStub(Isolate* isolate, const StoreICState& state)
      : PlatformCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  Code::Kind GetCodeKind() const override { return Code::STORE_IC; }

  InlineCacheState GetICState() const final { return GENERIC; }

  ExtraICState GetExtraICState() const final {
    return static_cast<ExtraICState>(minor_key_);
  }

 protected:
  StoreICState state() const {
    return StoreICState(static_cast<ExtraICState>(minor_key_));
  }

 private:
  DEFINE_CALL_INTERFACE_DESCRIPTOR(VectorStoreICTrampoline);
  DEFINE_PLATFORM_CODE_STUB(VectorStoreICTrampoline, PlatformCodeStub);
};


class VectorKeyedStoreICTrampolineStub : public VectorStoreICTrampolineStub {
 public:
  VectorKeyedStoreICTrampolineStub(Isolate* isolate, const StoreICState& state)
      : VectorStoreICTrampolineStub(isolate, state) {}

  Code::Kind GetCodeKind() const override { return Code::KEYED_STORE_IC; }

  DEFINE_PLATFORM_CODE_STUB(VectorKeyedStoreICTrampoline,
                            VectorStoreICTrampolineStub);
};


class CallICTrampolineStub : public PlatformCodeStub {
 public:
  CallICTrampolineStub(Isolate* isolate, const CallICState& state)
      : PlatformCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  Code::Kind GetCodeKind() const override { return Code::CALL_IC; }

  InlineCacheState GetICState() const final { return GENERIC; }

  ExtraICState GetExtraICState() const final {
    return static_cast<ExtraICState>(minor_key_);
  }

 protected:
  CallICState state() const {
    return CallICState(static_cast<ExtraICState>(minor_key_));
  }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(CallFunctionWithFeedback);
  DEFINE_PLATFORM_CODE_STUB(CallICTrampoline, PlatformCodeStub);
};


class LoadICStub : public PlatformCodeStub {
 public:
  explicit LoadICStub(Isolate* isolate, const LoadICState& state)
      : PlatformCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  void GenerateForTrampoline(MacroAssembler* masm);

  Code::Kind GetCodeKind() const override { return Code::LOAD_IC; }
  InlineCacheState GetICState() const final { return GENERIC; }
  ExtraICState GetExtraICState() const final {
    return static_cast<ExtraICState>(minor_key_);
  }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(LoadWithVector);
  DEFINE_PLATFORM_CODE_STUB(LoadIC, PlatformCodeStub);

 protected:
  void GenerateImpl(MacroAssembler* masm, bool in_frame);
};


class KeyedLoadICStub : public PlatformCodeStub {
 public:
  explicit KeyedLoadICStub(Isolate* isolate, const LoadICState& state)
      : PlatformCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  void GenerateForTrampoline(MacroAssembler* masm);

  Code::Kind GetCodeKind() const override { return Code::KEYED_LOAD_IC; }
  InlineCacheState GetICState() const final { return GENERIC; }
  ExtraICState GetExtraICState() const final {
    return static_cast<ExtraICState>(minor_key_);
  }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(LoadWithVector);
  DEFINE_PLATFORM_CODE_STUB(KeyedLoadIC, PlatformCodeStub);

 protected:
  void GenerateImpl(MacroAssembler* masm, bool in_frame);
};


class VectorStoreICStub : public PlatformCodeStub {
 public:
  VectorStoreICStub(Isolate* isolate, const StoreICState& state)
      : PlatformCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  void GenerateForTrampoline(MacroAssembler* masm);

  Code::Kind GetCodeKind() const final { return Code::STORE_IC; }
  InlineCacheState GetICState() const final { return GENERIC; }
  ExtraICState GetExtraICState() const final {
    return static_cast<ExtraICState>(minor_key_);
  }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(VectorStoreIC);
  DEFINE_PLATFORM_CODE_STUB(VectorStoreIC, PlatformCodeStub);

 protected:
  void GenerateImpl(MacroAssembler* masm, bool in_frame);
};


class VectorKeyedStoreICStub : public PlatformCodeStub {
 public:
  VectorKeyedStoreICStub(Isolate* isolate, const StoreICState& state)
      : PlatformCodeStub(isolate) {
    minor_key_ = state.GetExtraICState();
  }

  void GenerateForTrampoline(MacroAssembler* masm);

  Code::Kind GetCodeKind() const final { return Code::KEYED_STORE_IC; }
  InlineCacheState GetICState() const final { return GENERIC; }
  ExtraICState GetExtraICState() const final {
    return static_cast<ExtraICState>(minor_key_);
  }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(VectorStoreIC);
  DEFINE_PLATFORM_CODE_STUB(VectorKeyedStoreIC, PlatformCodeStub);

 protected:
  void GenerateImpl(MacroAssembler* masm, bool in_frame);
};


class DoubleToIStub : public PlatformCodeStub {
 public:
  DoubleToIStub(Isolate* isolate, Register source, Register destination,
                int offset, bool is_truncating, bool skip_fastpath = false)
      : PlatformCodeStub(isolate) {
    minor_key_ = SourceRegisterBits::encode(source.code()) |
                 DestinationRegisterBits::encode(destination.code()) |
                 OffsetBits::encode(offset) |
                 IsTruncatingBits::encode(is_truncating) |
                 SkipFastPathBits::encode(skip_fastpath) |
                 SSE3Bits::encode(CpuFeatures::IsSupported(SSE3) ? 1 : 0);
  }

  bool SometimesSetsUpAFrame() override { return false; }

 private:
  Register source() const {
    return Register::from_code(SourceRegisterBits::decode(minor_key_));
  }
  Register destination() const {
    return Register::from_code(DestinationRegisterBits::decode(minor_key_));
  }
  bool is_truncating() const { return IsTruncatingBits::decode(minor_key_); }
  bool skip_fastpath() const { return SkipFastPathBits::decode(minor_key_); }
  int offset() const { return OffsetBits::decode(minor_key_); }

  static const int kBitsPerRegisterNumber = 6;
  STATIC_ASSERT((1L << kBitsPerRegisterNumber) >= Register::kNumRegisters);
  class SourceRegisterBits:
      public BitField<int, 0, kBitsPerRegisterNumber> {};  // NOLINT
  class DestinationRegisterBits:
      public BitField<int, kBitsPerRegisterNumber,
        kBitsPerRegisterNumber> {};  // NOLINT
  class IsTruncatingBits:
      public BitField<bool, 2 * kBitsPerRegisterNumber, 1> {};  // NOLINT
  class OffsetBits:
      public BitField<int, 2 * kBitsPerRegisterNumber + 1, 3> {};  // NOLINT
  class SkipFastPathBits:
      public BitField<int, 2 * kBitsPerRegisterNumber + 4, 1> {};  // NOLINT
  class SSE3Bits:
      public BitField<int, 2 * kBitsPerRegisterNumber + 5, 1> {};  // NOLINT

  DEFINE_NULL_CALL_INTERFACE_DESCRIPTOR();
  DEFINE_PLATFORM_CODE_STUB(DoubleToI, PlatformCodeStub);
};


class ScriptContextFieldStub : public HandlerStub {
 public:
  ScriptContextFieldStub(Isolate* isolate,
                         const ScriptContextTable::LookupResult* lookup_result)
      : HandlerStub(isolate) {
    DCHECK(Accepted(lookup_result));
    STATIC_ASSERT(kContextIndexBits + kSlotIndexBits <= kSubMinorKeyBits);
    set_sub_minor_key(ContextIndexBits::encode(lookup_result->context_index) |
                      SlotIndexBits::encode(lookup_result->slot_index));
  }

  int context_index() const {
    return ContextIndexBits::decode(sub_minor_key());
  }

  int slot_index() const { return SlotIndexBits::decode(sub_minor_key()); }

  static bool Accepted(const ScriptContextTable::LookupResult* lookup_result) {
    return ContextIndexBits::is_valid(lookup_result->context_index) &&
           SlotIndexBits::is_valid(lookup_result->slot_index);
  }

 private:
  static const int kContextIndexBits = 9;
  static const int kSlotIndexBits = 13;
  class ContextIndexBits : public BitField<int, 0, kContextIndexBits> {};
  class SlotIndexBits
      : public BitField<int, kContextIndexBits, kSlotIndexBits> {};

  Code::StubType GetStubType() const override { return Code::FAST; }

  DEFINE_CODE_STUB_BASE(ScriptContextFieldStub, HandlerStub);
};


class LoadScriptContextFieldStub : public ScriptContextFieldStub {
 public:
  LoadScriptContextFieldStub(
      Isolate* isolate, const ScriptContextTable::LookupResult* lookup_result)
      : ScriptContextFieldStub(isolate, lookup_result) {}

 private:
  Code::Kind kind() const override { return Code::LOAD_IC; }

  DEFINE_HANDLER_CODE_STUB(LoadScriptContextField, ScriptContextFieldStub);
};


class StoreScriptContextFieldStub : public ScriptContextFieldStub {
 public:
  StoreScriptContextFieldStub(
      Isolate* isolate, const ScriptContextTable::LookupResult* lookup_result)
      : ScriptContextFieldStub(isolate, lookup_result) {}

 private:
  Code::Kind kind() const override { return Code::STORE_IC; }

  DEFINE_HANDLER_CODE_STUB(StoreScriptContextField, ScriptContextFieldStub);
};


class LoadFastElementStub : public HandlerStub {
 public:
  LoadFastElementStub(Isolate* isolate, bool is_js_array,
                      ElementsKind elements_kind,
                      bool convert_hole_to_undefined = false)
      : HandlerStub(isolate) {
    set_sub_minor_key(
        ElementsKindBits::encode(elements_kind) |
        IsJSArrayBits::encode(is_js_array) |
        CanConvertHoleToUndefined::encode(convert_hole_to_undefined));
  }

  Code::Kind kind() const override { return Code::KEYED_LOAD_IC; }

  bool is_js_array() const { return IsJSArrayBits::decode(sub_minor_key()); }
  bool convert_hole_to_undefined() const {
    return CanConvertHoleToUndefined::decode(sub_minor_key());
  }

  ElementsKind elements_kind() const {
    return ElementsKindBits::decode(sub_minor_key());
  }

 private:
  class ElementsKindBits: public BitField<ElementsKind, 0, 8> {};
  class IsJSArrayBits: public BitField<bool, 8, 1> {};
  class CanConvertHoleToUndefined : public BitField<bool, 9, 1> {};

  DEFINE_HANDLER_CODE_STUB(LoadFastElement, HandlerStub);
};


class StoreFastElementStub : public HydrogenCodeStub {
 public:
  StoreFastElementStub(Isolate* isolate, bool is_js_array,
                       ElementsKind elements_kind, KeyedAccessStoreMode mode)
      : HydrogenCodeStub(isolate) {
    set_sub_minor_key(CommonStoreModeBits::encode(mode) |
                      ElementsKindBits::encode(elements_kind) |
                      IsJSArrayBits::encode(is_js_array));
  }

  static void GenerateAheadOfTime(Isolate* isolate);

  bool is_js_array() const { return IsJSArrayBits::decode(sub_minor_key()); }

  ElementsKind elements_kind() const {
    return ElementsKindBits::decode(sub_minor_key());
  }

  KeyedAccessStoreMode store_mode() const {
    return CommonStoreModeBits::decode(sub_minor_key());
  }

  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override {
    return VectorStoreICDescriptor(isolate());
  }

  Code::Kind GetCodeKind() const override { return Code::HANDLER; }

 private:
  class ElementsKindBits : public BitField<ElementsKind, 3, 8> {};
  class IsJSArrayBits : public BitField<bool, 11, 1> {};

  DEFINE_HYDROGEN_CODE_STUB(StoreFastElement, HydrogenCodeStub);
};


class TransitionElementsKindStub : public HydrogenCodeStub {
 public:
  TransitionElementsKindStub(Isolate* isolate,
                             ElementsKind from_kind,
                             ElementsKind to_kind,
                             bool is_js_array) : HydrogenCodeStub(isolate) {
    set_sub_minor_key(FromKindBits::encode(from_kind) |
                      ToKindBits::encode(to_kind) |
                      IsJSArrayBits::encode(is_js_array));
  }

  ElementsKind from_kind() const {
    return FromKindBits::decode(sub_minor_key());
  }

  ElementsKind to_kind() const { return ToKindBits::decode(sub_minor_key()); }

  bool is_js_array() const { return IsJSArrayBits::decode(sub_minor_key()); }

 private:
  class FromKindBits: public BitField<ElementsKind, 8, 8> {};
  class ToKindBits: public BitField<ElementsKind, 0, 8> {};
  class IsJSArrayBits: public BitField<bool, 16, 1> {};

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TransitionElementsKind);
  DEFINE_HYDROGEN_CODE_STUB(TransitionElementsKind, HydrogenCodeStub);
};

class AllocateHeapNumberStub : public TurboFanCodeStub {
 public:
  explicit AllocateHeapNumberStub(Isolate* isolate)
      : TurboFanCodeStub(isolate) {}

  void InitializeDescriptor(CodeStubDescriptor* descriptor) override;
  void GenerateAssembly(compiler::CodeStubAssembler* assembler) const override;

  DEFINE_CALL_INTERFACE_DESCRIPTOR(AllocateHeapNumber);
  DEFINE_CODE_STUB(AllocateHeapNumber, TurboFanCodeStub);
};

class AllocateMutableHeapNumberStub : public TurboFanCodeStub {
 public:
  explicit AllocateMutableHeapNumberStub(Isolate* isolate)
      : TurboFanCodeStub(isolate) {}

  void InitializeDescriptor(CodeStubDescriptor* descriptor) override;
  void GenerateAssembly(compiler::CodeStubAssembler* assembler) const override;

  DEFINE_CALL_INTERFACE_DESCRIPTOR(AllocateMutableHeapNumber);
  DEFINE_CODE_STUB(AllocateMutableHeapNumber, TurboFanCodeStub);
};

#define SIMD128_ALLOC_STUB(TYPE, Type, type, lane_count, lane_type)     \
  class Allocate##Type##Stub : public TurboFanCodeStub {                \
   public:                                                              \
    explicit Allocate##Type##Stub(Isolate* isolate)                     \
        : TurboFanCodeStub(isolate) {}                                  \
                                                                        \
    void InitializeDescriptor(CodeStubDescriptor* descriptor) override; \
    void GenerateAssembly(                                              \
        compiler::CodeStubAssembler* assembler) const override;         \
                                                                        \
    DEFINE_CALL_INTERFACE_DESCRIPTOR(Allocate##Type);                   \
    DEFINE_CODE_STUB(Allocate##Type, TurboFanCodeStub);                 \
  };
SIMD128_TYPES(SIMD128_ALLOC_STUB)
#undef SIMD128_ALLOC_STUB

class AllocateInNewSpaceStub final : public HydrogenCodeStub {
 public:
  explicit AllocateInNewSpaceStub(Isolate* isolate)
      : HydrogenCodeStub(isolate) {}

 private:
  DEFINE_CALL_INTERFACE_DESCRIPTOR(AllocateInNewSpace);
  DEFINE_HYDROGEN_CODE_STUB(AllocateInNewSpace, HydrogenCodeStub);
};


class ArrayConstructorStubBase : public HydrogenCodeStub {
 public:
  ArrayConstructorStubBase(Isolate* isolate,
                           ElementsKind kind,
                           AllocationSiteOverrideMode override_mode)
      : HydrogenCodeStub(isolate) {
    // It only makes sense to override local allocation site behavior
    // if there is a difference between the global allocation site policy
    // for an ElementsKind and the desired usage of the stub.
    DCHECK(override_mode != DISABLE_ALLOCATION_SITES ||
           AllocationSite::GetMode(kind) == TRACK_ALLOCATION_SITE);
    set_sub_minor_key(ElementsKindBits::encode(kind) |
                      AllocationSiteOverrideModeBits::encode(override_mode));
  }

  ElementsKind elements_kind() const {
    return ElementsKindBits::decode(sub_minor_key());
  }

  AllocationSiteOverrideMode override_mode() const {
    return AllocationSiteOverrideModeBits::decode(sub_minor_key());
  }

  static void GenerateStubsAheadOfTime(Isolate* isolate);

  // Parameters accessed via CodeStubGraphBuilder::GetParameter()
  static const int kConstructor = 0;
  static const int kAllocationSite = 1;

 protected:
  std::ostream& BasePrintName(std::ostream& os,
                              const char* name) const;  // NOLINT

 private:
  // Ensure data fits within available bits.
  STATIC_ASSERT(LAST_ALLOCATION_SITE_OVERRIDE_MODE == 1);

  class ElementsKindBits: public BitField<ElementsKind, 0, 8> {};
  class AllocationSiteOverrideModeBits: public
      BitField<AllocationSiteOverrideMode, 8, 1> {};  // NOLINT

  DEFINE_CODE_STUB_BASE(ArrayConstructorStubBase, HydrogenCodeStub);
};


class ArrayNoArgumentConstructorStub : public ArrayConstructorStubBase {
 public:
  ArrayNoArgumentConstructorStub(
      Isolate* isolate,
      ElementsKind kind,
      AllocationSiteOverrideMode override_mode = DONT_OVERRIDE)
      : ArrayConstructorStubBase(isolate, kind, override_mode) {
  }

 private:
  void PrintName(std::ostream& os) const override {  // NOLINT
    BasePrintName(os, "ArrayNoArgumentConstructorStub");
  }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(ArrayConstructorConstantArgCount);
  DEFINE_HYDROGEN_CODE_STUB(ArrayNoArgumentConstructor,
                            ArrayConstructorStubBase);
};


class ArraySingleArgumentConstructorStub : public ArrayConstructorStubBase {
 public:
  ArraySingleArgumentConstructorStub(
      Isolate* isolate,
      ElementsKind kind,
      AllocationSiteOverrideMode override_mode = DONT_OVERRIDE)
      : ArrayConstructorStubBase(isolate, kind, override_mode) {
  }

 private:
  void PrintName(std::ostream& os) const override {  // NOLINT
    BasePrintName(os, "ArraySingleArgumentConstructorStub");
  }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(ArrayConstructor);
  DEFINE_HYDROGEN_CODE_STUB(ArraySingleArgumentConstructor,
                            ArrayConstructorStubBase);
};


class ArrayNArgumentsConstructorStub : public ArrayConstructorStubBase {
 public:
  ArrayNArgumentsConstructorStub(
      Isolate* isolate,
      ElementsKind kind,
      AllocationSiteOverrideMode override_mode = DONT_OVERRIDE)
      : ArrayConstructorStubBase(isolate, kind, override_mode) {
  }

 private:
  void PrintName(std::ostream& os) const override {  // NOLINT
    BasePrintName(os, "ArrayNArgumentsConstructorStub");
  }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(ArrayConstructor);
  DEFINE_HYDROGEN_CODE_STUB(ArrayNArgumentsConstructor,
                            ArrayConstructorStubBase);
};


class InternalArrayConstructorStubBase : public HydrogenCodeStub {
 public:
  InternalArrayConstructorStubBase(Isolate* isolate, ElementsKind kind)
      : HydrogenCodeStub(isolate) {
    set_sub_minor_key(ElementsKindBits::encode(kind));
  }

  static void GenerateStubsAheadOfTime(Isolate* isolate);

  // Parameters accessed via CodeStubGraphBuilder::GetParameter()
  static const int kConstructor = 0;

  ElementsKind elements_kind() const {
    return ElementsKindBits::decode(sub_minor_key());
  }

 private:
  class ElementsKindBits : public BitField<ElementsKind, 0, 8> {};

  DEFINE_CODE_STUB_BASE(InternalArrayConstructorStubBase, HydrogenCodeStub);
};


class InternalArrayNoArgumentConstructorStub : public
    InternalArrayConstructorStubBase {
 public:
  InternalArrayNoArgumentConstructorStub(Isolate* isolate,
                                         ElementsKind kind)
      : InternalArrayConstructorStubBase(isolate, kind) { }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(InternalArrayConstructorConstantArgCount);
  DEFINE_HYDROGEN_CODE_STUB(InternalArrayNoArgumentConstructor,
                            InternalArrayConstructorStubBase);
};


class InternalArraySingleArgumentConstructorStub : public
    InternalArrayConstructorStubBase {
 public:
  InternalArraySingleArgumentConstructorStub(Isolate* isolate,
                                             ElementsKind kind)
      : InternalArrayConstructorStubBase(isolate, kind) { }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(InternalArrayConstructor);
  DEFINE_HYDROGEN_CODE_STUB(InternalArraySingleArgumentConstructor,
                            InternalArrayConstructorStubBase);
};


class InternalArrayNArgumentsConstructorStub : public
    InternalArrayConstructorStubBase {
 public:
  InternalArrayNArgumentsConstructorStub(Isolate* isolate, ElementsKind kind)
      : InternalArrayConstructorStubBase(isolate, kind) { }

  DEFINE_CALL_INTERFACE_DESCRIPTOR(InternalArrayConstructor);
  DEFINE_HYDROGEN_CODE_STUB(InternalArrayNArgumentsConstructor,
                            InternalArrayConstructorStubBase);
};


class StoreElementStub : public PlatformCodeStub {
 public:
  StoreElementStub(Isolate* isolate, ElementsKind elements_kind,
                   KeyedAccessStoreMode mode)
      : PlatformCodeStub(isolate) {
    // TODO(jkummerow): Rename this stub to StoreSlowElementStub,
    // drop elements_kind parameter.
    DCHECK_EQ(DICTIONARY_ELEMENTS, elements_kind);
    minor_key_ = ElementsKindBits::encode(elements_kind) |
                 CommonStoreModeBits::encode(mode);
  }

  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override {
    return VectorStoreICDescriptor(isolate());
  }

  Code::Kind GetCodeKind() const override { return Code::HANDLER; }

 private:
  ElementsKind elements_kind() const {
    return ElementsKindBits::decode(minor_key_);
  }

  class ElementsKindBits : public BitField<ElementsKind, 3, 8> {};

  DEFINE_PLATFORM_CODE_STUB(StoreElement, PlatformCodeStub);
};

class ToBooleanICStub : public HydrogenCodeStub {
 public:
  enum Type {
    UNDEFINED,
    BOOLEAN,
    NULL_TYPE,
    SMI,
    SPEC_OBJECT,
    STRING,
    SYMBOL,
    HEAP_NUMBER,
    SIMD_VALUE,
    NUMBER_OF_TYPES
  };

  // At most 16 different types can be distinguished, because the Code object
  // only has room for two bytes to hold a set of these types. :-P
  STATIC_ASSERT(NUMBER_OF_TYPES <= 16);

  class Types : public EnumSet<Type, uint16_t> {
   public:
    Types() : EnumSet<Type, uint16_t>(0) {}
    explicit Types(uint16_t bits) : EnumSet<Type, uint16_t>(bits) {}

    bool UpdateStatus(Handle<Object> object);
    bool NeedsMap() const;
    bool CanBeUndetectable() const {
      return Contains(ToBooleanICStub::SPEC_OBJECT);
    }
    bool IsGeneric() const { return ToIntegral() == Generic().ToIntegral(); }

    static Types Generic() { return Types((1 << NUMBER_OF_TYPES) - 1); }
  };

  ToBooleanICStub(Isolate* isolate, ExtraICState state)
      : HydrogenCodeStub(isolate) {
    set_sub_minor_key(TypesBits::encode(static_cast<uint16_t>(state)));
  }

  bool UpdateStatus(Handle<Object> object);
  Types types() const { return Types(TypesBits::decode(sub_minor_key())); }

  Code::Kind GetCodeKind() const override { return Code::TO_BOOLEAN_IC; }
  void PrintState(std::ostream& os) const override;  // NOLINT

  bool SometimesSetsUpAFrame() override { return false; }

  static Handle<Code> GetUninitialized(Isolate* isolate) {
    return ToBooleanICStub(isolate, UNINITIALIZED).GetCode();
  }

  ExtraICState GetExtraICState() const override { return types().ToIntegral(); }

  InlineCacheState GetICState() const override {
    if (types().IsEmpty()) {
      return ::v8::internal::UNINITIALIZED;
    } else {
      return MONOMORPHIC;
    }
  }

 private:
  ToBooleanICStub(Isolate* isolate, InitializationState init_state)
      : HydrogenCodeStub(isolate, init_state) {}

  class TypesBits : public BitField<uint16_t, 0, NUMBER_OF_TYPES> {};

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_HYDROGEN_CODE_STUB(ToBooleanIC, HydrogenCodeStub);
};

std::ostream& operator<<(std::ostream& os, const ToBooleanICStub::Types& t);

class ElementsTransitionAndStoreStub : public HydrogenCodeStub {
 public:
  ElementsTransitionAndStoreStub(Isolate* isolate, ElementsKind from_kind,
                                 ElementsKind to_kind, bool is_jsarray,
                                 KeyedAccessStoreMode store_mode)
      : HydrogenCodeStub(isolate) {
    set_sub_minor_key(CommonStoreModeBits::encode(store_mode) |
                      FromBits::encode(from_kind) | ToBits::encode(to_kind) |
                      IsJSArrayBits::encode(is_jsarray));
  }

  ElementsKind from_kind() const { return FromBits::decode(sub_minor_key()); }
  ElementsKind to_kind() const { return ToBits::decode(sub_minor_key()); }
  bool is_jsarray() const { return IsJSArrayBits::decode(sub_minor_key()); }
  KeyedAccessStoreMode store_mode() const {
    return CommonStoreModeBits::decode(sub_minor_key());
  }

  CallInterfaceDescriptor GetCallInterfaceDescriptor() const override;
  Code::Kind GetCodeKind() const override { return Code::HANDLER; }

 private:
  class FromBits : public BitField<ElementsKind, 3, 8> {};
  class ToBits : public BitField<ElementsKind, 11, 8> {};
  class IsJSArrayBits : public BitField<bool, 19, 1> {};

  DEFINE_HYDROGEN_CODE_STUB(ElementsTransitionAndStore, HydrogenCodeStub);
};


class StubFailureTrampolineStub : public PlatformCodeStub {
 public:
  StubFailureTrampolineStub(Isolate* isolate, StubFunctionMode function_mode)
      : PlatformCodeStub(isolate) {
    minor_key_ = FunctionModeField::encode(function_mode);
  }

  static void GenerateAheadOfTime(Isolate* isolate);

 private:
  StubFunctionMode function_mode() const {
    return FunctionModeField::decode(minor_key_);
  }

  class FunctionModeField : public BitField<StubFunctionMode, 0, 1> {};

  DEFINE_NULL_CALL_INTERFACE_DESCRIPTOR();
  DEFINE_PLATFORM_CODE_STUB(StubFailureTrampoline, PlatformCodeStub);
};


class ProfileEntryHookStub : public PlatformCodeStub {
 public:
  explicit ProfileEntryHookStub(Isolate* isolate) : PlatformCodeStub(isolate) {}

  // The profile entry hook function is not allowed to cause a GC.
  bool SometimesSetsUpAFrame() override { return false; }

  // Generates a call to the entry hook if it's enabled.
  static void MaybeCallEntryHook(MacroAssembler* masm);

 private:
  static void EntryHookTrampoline(intptr_t function,
                                  intptr_t stack_pointer,
                                  Isolate* isolate);

  // ProfileEntryHookStub is called at the start of a function, so it has the
  // same register set.
  DEFINE_CALL_INTERFACE_DESCRIPTOR(CallFunction)
  DEFINE_PLATFORM_CODE_STUB(ProfileEntryHook, PlatformCodeStub);
};


class StoreBufferOverflowStub : public PlatformCodeStub {
 public:
  StoreBufferOverflowStub(Isolate* isolate, SaveFPRegsMode save_fp)
      : PlatformCodeStub(isolate) {
    minor_key_ = SaveDoublesBits::encode(save_fp == kSaveFPRegs);
  }

  static void GenerateFixedRegStubsAheadOfTime(Isolate* isolate);
  bool SometimesSetsUpAFrame() override { return false; }

 private:
  bool save_doubles() const { return SaveDoublesBits::decode(minor_key_); }

  class SaveDoublesBits : public BitField<bool, 0, 1> {};

  DEFINE_NULL_CALL_INTERFACE_DESCRIPTOR();
  DEFINE_PLATFORM_CODE_STUB(StoreBufferOverflow, PlatformCodeStub);
};


class SubStringStub : public PlatformCodeStub {
 public:
  explicit SubStringStub(Isolate* isolate) : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(ContextOnly);
  DEFINE_PLATFORM_CODE_STUB(SubString, PlatformCodeStub);
};


class ToNumberStub final : public PlatformCodeStub {
 public:
  explicit ToNumberStub(Isolate* isolate) : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_PLATFORM_CODE_STUB(ToNumber, PlatformCodeStub);
};

class NonNumberToNumberStub final : public PlatformCodeStub {
 public:
  explicit NonNumberToNumberStub(Isolate* isolate)
      : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_PLATFORM_CODE_STUB(NonNumberToNumber, PlatformCodeStub);
};

class StringToNumberStub final : public PlatformCodeStub {
 public:
  explicit StringToNumberStub(Isolate* isolate) : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_PLATFORM_CODE_STUB(StringToNumber, PlatformCodeStub);
};


class ToStringStub final : public PlatformCodeStub {
 public:
  explicit ToStringStub(Isolate* isolate) : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_PLATFORM_CODE_STUB(ToString, PlatformCodeStub);
};


class ToNameStub final : public PlatformCodeStub {
 public:
  explicit ToNameStub(Isolate* isolate) : PlatformCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_PLATFORM_CODE_STUB(ToName, PlatformCodeStub);
};


class ToObjectStub final : public HydrogenCodeStub {
 public:
  explicit ToObjectStub(Isolate* isolate) : HydrogenCodeStub(isolate) {}

  DEFINE_CALL_INTERFACE_DESCRIPTOR(TypeConversion);
  DEFINE_HYDROGEN_CODE_STUB(ToObject, HydrogenCodeStub);
};

#undef DEFINE_CALL_INTERFACE_DESCRIPTOR
#undef DEFINE_PLATFORM_CODE_STUB
#undef DEFINE_HANDLER_CODE_STUB
#undef DEFINE_HYDROGEN_CODE_STUB
#undef DEFINE_CODE_STUB
#undef DEFINE_CODE_STUB_BASE

extern Representation RepresentationFromType(Type* type);

}  // namespace internal
}  // namespace v8

#endif  // V8_CODE_STUBS_H_
