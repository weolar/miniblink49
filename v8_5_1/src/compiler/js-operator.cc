// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/js-operator.h"

#include <limits>

#include "src/base/lazy-instance.h"
#include "src/compiler/opcodes.h"
#include "src/compiler/operator.h"
#include "src/objects-inl.h"  // TODO(mstarzinger): Temporary cycle breaker!
#include "src/type-feedback-vector-inl.h"

namespace v8 {
namespace internal {
namespace compiler {

VectorSlotPair::VectorSlotPair() {}


int VectorSlotPair::index() const {
  return vector_.is_null() ? -1 : vector_->GetIndex(slot_);
}


bool operator==(VectorSlotPair const& lhs, VectorSlotPair const& rhs) {
  return lhs.slot() == rhs.slot() &&
         lhs.vector().location() == rhs.vector().location();
}


bool operator!=(VectorSlotPair const& lhs, VectorSlotPair const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(VectorSlotPair const& p) {
  return base::hash_combine(p.slot(), p.vector().location());
}


ConvertReceiverMode ConvertReceiverModeOf(Operator const* op) {
  DCHECK_EQ(IrOpcode::kJSConvertReceiver, op->opcode());
  return OpParameter<ConvertReceiverMode>(op);
}


ToBooleanHints ToBooleanHintsOf(Operator const* op) {
  DCHECK_EQ(IrOpcode::kJSToBoolean, op->opcode());
  return OpParameter<ToBooleanHints>(op);
}


bool operator==(CallConstructParameters const& lhs,
                CallConstructParameters const& rhs) {
  return lhs.arity() == rhs.arity() && lhs.feedback() == rhs.feedback();
}


bool operator!=(CallConstructParameters const& lhs,
                CallConstructParameters const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(CallConstructParameters const& p) {
  return base::hash_combine(p.arity(), p.feedback());
}


std::ostream& operator<<(std::ostream& os, CallConstructParameters const& p) {
  return os << p.arity();
}


CallConstructParameters const& CallConstructParametersOf(Operator const* op) {
  DCHECK_EQ(IrOpcode::kJSCallConstruct, op->opcode());
  return OpParameter<CallConstructParameters>(op);
}


std::ostream& operator<<(std::ostream& os, CallFunctionParameters const& p) {
  os << p.arity() << ", " << p.convert_mode() << ", " << p.tail_call_mode();
  return os;
}


const CallFunctionParameters& CallFunctionParametersOf(const Operator* op) {
  DCHECK_EQ(IrOpcode::kJSCallFunction, op->opcode());
  return OpParameter<CallFunctionParameters>(op);
}


bool operator==(CallRuntimeParameters const& lhs,
                CallRuntimeParameters const& rhs) {
  return lhs.id() == rhs.id() && lhs.arity() == rhs.arity();
}


bool operator!=(CallRuntimeParameters const& lhs,
                CallRuntimeParameters const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(CallRuntimeParameters const& p) {
  return base::hash_combine(p.id(), p.arity());
}


std::ostream& operator<<(std::ostream& os, CallRuntimeParameters const& p) {
  return os << p.id() << ", " << p.arity();
}


const CallRuntimeParameters& CallRuntimeParametersOf(const Operator* op) {
  DCHECK_EQ(IrOpcode::kJSCallRuntime, op->opcode());
  return OpParameter<CallRuntimeParameters>(op);
}


ContextAccess::ContextAccess(size_t depth, size_t index, bool immutable)
    : immutable_(immutable),
      depth_(static_cast<uint16_t>(depth)),
      index_(static_cast<uint32_t>(index)) {
  DCHECK(depth <= std::numeric_limits<uint16_t>::max());
  DCHECK(index <= std::numeric_limits<uint32_t>::max());
}


bool operator==(ContextAccess const& lhs, ContextAccess const& rhs) {
  return lhs.depth() == rhs.depth() && lhs.index() == rhs.index() &&
         lhs.immutable() == rhs.immutable();
}


bool operator!=(ContextAccess const& lhs, ContextAccess const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(ContextAccess const& access) {
  return base::hash_combine(access.depth(), access.index(), access.immutable());
}


std::ostream& operator<<(std::ostream& os, ContextAccess const& access) {
  return os << access.depth() << ", " << access.index() << ", "
            << access.immutable();
}


ContextAccess const& ContextAccessOf(Operator const* op) {
  DCHECK(op->opcode() == IrOpcode::kJSLoadContext ||
         op->opcode() == IrOpcode::kJSStoreContext);
  return OpParameter<ContextAccess>(op);
}


bool operator==(NamedAccess const& lhs, NamedAccess const& rhs) {
  return lhs.name().location() == rhs.name().location() &&
         lhs.language_mode() == rhs.language_mode() &&
         lhs.feedback() == rhs.feedback();
}


bool operator!=(NamedAccess const& lhs, NamedAccess const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(NamedAccess const& p) {
  return base::hash_combine(p.name().location(), p.language_mode(),
                            p.feedback());
}


std::ostream& operator<<(std::ostream& os, NamedAccess const& p) {
  return os << Brief(*p.name()) << ", " << p.language_mode();
}


NamedAccess const& NamedAccessOf(const Operator* op) {
  DCHECK(op->opcode() == IrOpcode::kJSLoadNamed ||
         op->opcode() == IrOpcode::kJSStoreNamed);
  return OpParameter<NamedAccess>(op);
}


std::ostream& operator<<(std::ostream& os, PropertyAccess const& p) {
  return os << p.language_mode();
}


bool operator==(PropertyAccess const& lhs, PropertyAccess const& rhs) {
  return lhs.language_mode() == rhs.language_mode() &&
         lhs.feedback() == rhs.feedback();
}


bool operator!=(PropertyAccess const& lhs, PropertyAccess const& rhs) {
  return !(lhs == rhs);
}


PropertyAccess const& PropertyAccessOf(const Operator* op) {
  DCHECK(op->opcode() == IrOpcode::kJSLoadProperty ||
         op->opcode() == IrOpcode::kJSStoreProperty);
  return OpParameter<PropertyAccess>(op);
}


size_t hash_value(PropertyAccess const& p) {
  return base::hash_combine(p.language_mode(), p.feedback());
}


bool operator==(LoadGlobalParameters const& lhs,
                LoadGlobalParameters const& rhs) {
  return lhs.name().location() == rhs.name().location() &&
         lhs.feedback() == rhs.feedback() &&
         lhs.typeof_mode() == rhs.typeof_mode();
}


bool operator!=(LoadGlobalParameters const& lhs,
                LoadGlobalParameters const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(LoadGlobalParameters const& p) {
  return base::hash_combine(p.name().location(), p.typeof_mode());
}


std::ostream& operator<<(std::ostream& os, LoadGlobalParameters const& p) {
  return os << Brief(*p.name()) << ", " << p.typeof_mode();
}


const LoadGlobalParameters& LoadGlobalParametersOf(const Operator* op) {
  DCHECK_EQ(IrOpcode::kJSLoadGlobal, op->opcode());
  return OpParameter<LoadGlobalParameters>(op);
}


bool operator==(StoreGlobalParameters const& lhs,
                StoreGlobalParameters const& rhs) {
  return lhs.language_mode() == rhs.language_mode() &&
         lhs.name().location() == rhs.name().location() &&
         lhs.feedback() == rhs.feedback();
}


bool operator!=(StoreGlobalParameters const& lhs,
                StoreGlobalParameters const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(StoreGlobalParameters const& p) {
  return base::hash_combine(p.language_mode(), p.name().location(),
                            p.feedback());
}


std::ostream& operator<<(std::ostream& os, StoreGlobalParameters const& p) {
  return os << p.language_mode() << ", " << Brief(*p.name());
}


const StoreGlobalParameters& StoreGlobalParametersOf(const Operator* op) {
  DCHECK_EQ(IrOpcode::kJSStoreGlobal, op->opcode());
  return OpParameter<StoreGlobalParameters>(op);
}


CreateArgumentsType const& CreateArgumentsTypeOf(const Operator* op) {
  DCHECK_EQ(IrOpcode::kJSCreateArguments, op->opcode());
  return OpParameter<CreateArgumentsType>(op);
}


bool operator==(CreateArrayParameters const& lhs,
                CreateArrayParameters const& rhs) {
  return lhs.arity() == rhs.arity() &&
         lhs.site().location() == rhs.site().location();
}


bool operator!=(CreateArrayParameters const& lhs,
                CreateArrayParameters const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(CreateArrayParameters const& p) {
  return base::hash_combine(p.arity(), p.site().location());
}


std::ostream& operator<<(std::ostream& os, CreateArrayParameters const& p) {
  os << p.arity();
  if (!p.site().is_null()) os << ", " << Brief(*p.site());
  return os;
}


const CreateArrayParameters& CreateArrayParametersOf(const Operator* op) {
  DCHECK_EQ(IrOpcode::kJSCreateArray, op->opcode());
  return OpParameter<CreateArrayParameters>(op);
}


bool operator==(CreateClosureParameters const& lhs,
                CreateClosureParameters const& rhs) {
  return lhs.pretenure() == rhs.pretenure() &&
         lhs.shared_info().location() == rhs.shared_info().location();
}


bool operator!=(CreateClosureParameters const& lhs,
                CreateClosureParameters const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(CreateClosureParameters const& p) {
  return base::hash_combine(p.pretenure(), p.shared_info().location());
}


std::ostream& operator<<(std::ostream& os, CreateClosureParameters const& p) {
  return os << p.pretenure() << ", " << Brief(*p.shared_info());
}


const CreateClosureParameters& CreateClosureParametersOf(const Operator* op) {
  DCHECK_EQ(IrOpcode::kJSCreateClosure, op->opcode());
  return OpParameter<CreateClosureParameters>(op);
}


bool operator==(CreateLiteralParameters const& lhs,
                CreateLiteralParameters const& rhs) {
  return lhs.constant().location() == rhs.constant().location() &&
         lhs.length() == rhs.length() && lhs.flags() == rhs.flags() &&
         lhs.index() == rhs.index();
}


bool operator!=(CreateLiteralParameters const& lhs,
                CreateLiteralParameters const& rhs) {
  return !(lhs == rhs);
}


size_t hash_value(CreateLiteralParameters const& p) {
  return base::hash_combine(p.constant().location(), p.length(), p.flags(),
                            p.index());
}


std::ostream& operator<<(std::ostream& os, CreateLiteralParameters const& p) {
  return os << Brief(*p.constant()) << ", " << p.length() << ", " << p.flags()
            << ", " << p.index();
}


const CreateLiteralParameters& CreateLiteralParametersOf(const Operator* op) {
  DCHECK(op->opcode() == IrOpcode::kJSCreateLiteralArray ||
         op->opcode() == IrOpcode::kJSCreateLiteralObject ||
         op->opcode() == IrOpcode::kJSCreateLiteralRegExp);
  return OpParameter<CreateLiteralParameters>(op);
}

#define CACHED_OP_LIST(V)                                  \
  V(Equal, Operator::kNoProperties, 2, 1)                  \
  V(NotEqual, Operator::kNoProperties, 2, 1)               \
  V(StrictEqual, Operator::kNoThrow, 2, 1)                 \
  V(StrictNotEqual, Operator::kNoThrow, 2, 1)              \
  V(LessThan, Operator::kNoProperties, 2, 1)               \
  V(GreaterThan, Operator::kNoProperties, 2, 1)            \
  V(LessThanOrEqual, Operator::kNoProperties, 2, 1)        \
  V(GreaterThanOrEqual, Operator::kNoProperties, 2, 1)     \
  V(ToInteger, Operator::kNoProperties, 1, 1)              \
  V(ToLength, Operator::kNoProperties, 1, 1)               \
  V(ToName, Operator::kNoProperties, 1, 1)                 \
  V(ToNumber, Operator::kNoProperties, 1, 1)               \
  V(ToObject, Operator::kNoProperties, 1, 1)               \
  V(ToString, Operator::kNoProperties, 1, 1)               \
  V(Yield, Operator::kNoProperties, 1, 1)                  \
  V(Create, Operator::kEliminatable, 2, 1)                 \
  V(CreateIterResultObject, Operator::kEliminatable, 2, 1) \
  V(HasProperty, Operator::kNoProperties, 2, 1)            \
  V(TypeOf, Operator::kEliminatable, 1, 1)                 \
  V(InstanceOf, Operator::kNoProperties, 2, 1)             \
  V(ForInDone, Operator::kPure, 2, 1)                      \
  V(ForInNext, Operator::kNoProperties, 4, 1)              \
  V(ForInPrepare, Operator::kNoProperties, 1, 3)           \
  V(ForInStep, Operator::kPure, 1, 1)                      \
  V(LoadMessage, Operator::kNoThrow, 0, 1)                 \
  V(StoreMessage, Operator::kNoThrow, 1, 0)                \
  V(StackCheck, Operator::kNoProperties, 0, 0)             \
  V(CreateWithContext, Operator::kNoProperties, 2, 1)      \
  V(CreateModuleContext, Operator::kNoProperties, 2, 1)

struct JSOperatorGlobalCache final {
#define CACHED(Name, properties, value_input_count, value_output_count)  \
  struct Name##Operator final : public Operator {                        \
    Name##Operator()                                                     \
        : Operator(IrOpcode::kJS##Name, properties, "JS" #Name,          \
                   value_input_count, Operator::ZeroIfPure(properties),  \
                   Operator::ZeroIfEliminatable(properties),             \
                   value_output_count, Operator::ZeroIfPure(properties), \
                   Operator::ZeroIfNoThrow(properties)) {}               \
  };                                                                     \
  Name##Operator k##Name##Operator;
  CACHED_OP_LIST(CACHED)
#undef CACHED
};


static base::LazyInstance<JSOperatorGlobalCache>::type kCache =
    LAZY_INSTANCE_INITIALIZER;


JSOperatorBuilder::JSOperatorBuilder(Zone* zone)
    : cache_(kCache.Get()), zone_(zone) {}


#define CACHED(Name, properties, value_input_count, value_output_count) \
  const Operator* JSOperatorBuilder::Name() {                           \
    return &cache_.k##Name##Operator;                                   \
  }
CACHED_OP_LIST(CACHED)
#undef CACHED

const Operator* JSOperatorBuilder::BitwiseOr(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(  //--
      IrOpcode::kJSBitwiseOr, Operator::kNoProperties,  // opcode
      "JSBitwiseOr",                                    // name
      2, 1, 1, 1, 1, 2,                                 // inputs/outputs
      hints);                                           // parameter
}

const Operator* JSOperatorBuilder::BitwiseXor(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(   //--
      IrOpcode::kJSBitwiseXor, Operator::kNoProperties,  // opcode
      "JSBitwiseXor",                                    // name
      2, 1, 1, 1, 1, 2,                                  // inputs/outputs
      hints);                                            // parameter
}

const Operator* JSOperatorBuilder::BitwiseAnd(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(   //--
      IrOpcode::kJSBitwiseAnd, Operator::kNoProperties,  // opcode
      "JSBitwiseAnd",                                    // name
      2, 1, 1, 1, 1, 2,                                  // inputs/outputs
      hints);                                            // parameter
}

const Operator* JSOperatorBuilder::ShiftLeft(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(  //--
      IrOpcode::kJSShiftLeft, Operator::kNoProperties,  // opcode
      "JSShiftLeft",                                    // name
      2, 1, 1, 1, 1, 2,                                 // inputs/outputs
      hints);                                           // parameter
}

const Operator* JSOperatorBuilder::ShiftRight(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(   //--
      IrOpcode::kJSShiftRight, Operator::kNoProperties,  // opcode
      "JSShiftRight",                                    // name
      2, 1, 1, 1, 1, 2,                                  // inputs/outputs
      hints);                                            // parameter
}

const Operator* JSOperatorBuilder::ShiftRightLogical(
    BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(          //--
      IrOpcode::kJSShiftRightLogical, Operator::kNoProperties,  // opcode
      "JSShiftRightLogical",                                    // name
      2, 1, 1, 1, 1, 2,  // inputs/outputs
      hints);            // parameter
}

const Operator* JSOperatorBuilder::Add(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(  //--
      IrOpcode::kJSAdd, Operator::kNoProperties,        // opcode
      "JSAdd",                                          // name
      2, 1, 1, 1, 1, 2,                                 // inputs/outputs
      hints);                                           // parameter
}

const Operator* JSOperatorBuilder::Subtract(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(  //--
      IrOpcode::kJSSubtract, Operator::kNoProperties,   // opcode
      "JSSubtract",                                     // name
      2, 1, 1, 1, 1, 2,                                 // inputs/outputs
      hints);                                           // parameter
}

const Operator* JSOperatorBuilder::Multiply(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(  //--
      IrOpcode::kJSMultiply, Operator::kNoProperties,   // opcode
      "JSMultiply",                                     // name
      2, 1, 1, 1, 1, 2,                                 // inputs/outputs
      hints);                                           // parameter
}

const Operator* JSOperatorBuilder::Divide(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(  //--
      IrOpcode::kJSDivide, Operator::kNoProperties,     // opcode
      "JSDivide",                                       // name
      2, 1, 1, 1, 1, 2,                                 // inputs/outputs
      hints);                                           // parameter
}

const Operator* JSOperatorBuilder::Modulus(BinaryOperationHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<BinaryOperationHints>(  //--
      IrOpcode::kJSModulus, Operator::kNoProperties,    // opcode
      "JSModulus",                                      // name
      2, 1, 1, 1, 1, 2,                                 // inputs/outputs
      hints);                                           // parameter
}


const Operator* JSOperatorBuilder::ToBoolean(ToBooleanHints hints) {
  // TODO(turbofan): Cache most important versions of this operator.
  return new (zone()) Operator1<ToBooleanHints>(        //--
      IrOpcode::kJSToBoolean, Operator::kEliminatable,  // opcode
      "JSToBoolean",                                    // name
      1, 1, 0, 1, 1, 0,                                 // inputs/outputs
      hints);                                           // parameter
}

const Operator* JSOperatorBuilder::CallFunction(
    size_t arity, VectorSlotPair const& feedback,
    ConvertReceiverMode convert_mode, TailCallMode tail_call_mode) {
  CallFunctionParameters parameters(arity, feedback, tail_call_mode,
                                    convert_mode);
  return new (zone()) Operator1<CallFunctionParameters>(   // --
      IrOpcode::kJSCallFunction, Operator::kNoProperties,  // opcode
      "JSCallFunction",                                    // name
      parameters.arity(), 1, 1, 1, 1, 2,                   // inputs/outputs
      parameters);                                         // parameter
}


const Operator* JSOperatorBuilder::CallRuntime(Runtime::FunctionId id) {
  const Runtime::Function* f = Runtime::FunctionForId(id);
  return CallRuntime(f, f->nargs);
}


const Operator* JSOperatorBuilder::CallRuntime(Runtime::FunctionId id,
                                               size_t arity) {
  const Runtime::Function* f = Runtime::FunctionForId(id);
  return CallRuntime(f, arity);
}


const Operator* JSOperatorBuilder::CallRuntime(const Runtime::Function* f,
                                               size_t arity) {
  CallRuntimeParameters parameters(f->function_id, arity);
  DCHECK(f->nargs == -1 || f->nargs == static_cast<int>(parameters.arity()));
  return new (zone()) Operator1<CallRuntimeParameters>(   // --
      IrOpcode::kJSCallRuntime, Operator::kNoProperties,  // opcode
      "JSCallRuntime",                                    // name
      parameters.arity(), 1, 1, f->result_size, 1, 2,     // inputs/outputs
      parameters);                                        // parameter
}


const Operator* JSOperatorBuilder::CallConstruct(
    size_t arity, VectorSlotPair const& feedback) {
  CallConstructParameters parameters(arity, feedback);
  return new (zone()) Operator1<CallConstructParameters>(   // --
      IrOpcode::kJSCallConstruct, Operator::kNoProperties,  // opcode
      "JSCallConstruct",                                    // name
      parameters.arity(), 1, 1, 1, 1, 2,                    // counts
      parameters);                                          // parameter
}


const Operator* JSOperatorBuilder::ConvertReceiver(
    ConvertReceiverMode convert_mode) {
  return new (zone()) Operator1<ConvertReceiverMode>(    // --
      IrOpcode::kJSConvertReceiver, Operator::kNoThrow,  // opcode
      "JSConvertReceiver",                               // name
      1, 1, 1, 1, 1, 0,                                  // counts
      convert_mode);                                     // parameter
}

const Operator* JSOperatorBuilder::LoadNamed(Handle<Name> name,
                                             const VectorSlotPair& feedback) {
  NamedAccess access(SLOPPY, name, feedback);
  return new (zone()) Operator1<NamedAccess>(           // --
      IrOpcode::kJSLoadNamed, Operator::kNoProperties,  // opcode
      "JSLoadNamed",                                    // name
      2, 1, 1, 1, 1, 2,                                 // counts
      access);                                          // parameter
}

const Operator* JSOperatorBuilder::LoadProperty(
    VectorSlotPair const& feedback) {
  PropertyAccess access(SLOPPY, feedback);
  return new (zone()) Operator1<PropertyAccess>(           // --
      IrOpcode::kJSLoadProperty, Operator::kNoProperties,  // opcode
      "JSLoadProperty",                                    // name
      3, 1, 1, 1, 1, 2,                                    // counts
      access);                                             // parameter
}


const Operator* JSOperatorBuilder::StoreNamed(LanguageMode language_mode,
                                              Handle<Name> name,
                                              VectorSlotPair const& feedback) {
  NamedAccess access(language_mode, name, feedback);
  return new (zone()) Operator1<NamedAccess>(            // --
      IrOpcode::kJSStoreNamed, Operator::kNoProperties,  // opcode
      "JSStoreNamed",                                    // name
      3, 1, 1, 0, 1, 2,                                  // counts
      access);                                           // parameter
}


const Operator* JSOperatorBuilder::StoreProperty(
    LanguageMode language_mode, VectorSlotPair const& feedback) {
  PropertyAccess access(language_mode, feedback);
  return new (zone()) Operator1<PropertyAccess>(            // --
      IrOpcode::kJSStoreProperty, Operator::kNoProperties,  // opcode
      "JSStoreProperty",                                    // name
      4, 1, 1, 0, 1, 2,                                     // counts
      access);                                              // parameter
}


const Operator* JSOperatorBuilder::DeleteProperty(LanguageMode language_mode) {
  return new (zone()) Operator1<LanguageMode>(               // --
      IrOpcode::kJSDeleteProperty, Operator::kNoProperties,  // opcode
      "JSDeleteProperty",                                    // name
      2, 1, 1, 1, 1, 2,                                      // counts
      language_mode);                                        // parameter
}


const Operator* JSOperatorBuilder::LoadGlobal(const Handle<Name>& name,
                                              const VectorSlotPair& feedback,
                                              TypeofMode typeof_mode) {
  LoadGlobalParameters parameters(name, feedback, typeof_mode);
  return new (zone()) Operator1<LoadGlobalParameters>(   // --
      IrOpcode::kJSLoadGlobal, Operator::kNoProperties,  // opcode
      "JSLoadGlobal",                                    // name
      1, 1, 1, 1, 1, 2,                                  // counts
      parameters);                                       // parameter
}


const Operator* JSOperatorBuilder::StoreGlobal(LanguageMode language_mode,
                                               const Handle<Name>& name,
                                               const VectorSlotPair& feedback) {
  StoreGlobalParameters parameters(language_mode, feedback, name);
  return new (zone()) Operator1<StoreGlobalParameters>(   // --
      IrOpcode::kJSStoreGlobal, Operator::kNoProperties,  // opcode
      "JSStoreGlobal",                                    // name
      2, 1, 1, 0, 1, 2,                                   // counts
      parameters);                                        // parameter
}


const Operator* JSOperatorBuilder::LoadContext(size_t depth, size_t index,
                                               bool immutable) {
  ContextAccess access(depth, index, immutable);
  return new (zone()) Operator1<ContextAccess>(  // --
      IrOpcode::kJSLoadContext,                  // opcode
      Operator::kNoWrite | Operator::kNoThrow,   // flags
      "JSLoadContext",                           // name
      1, 1, 0, 1, 1, 0,                          // counts
      access);                                   // parameter
}


const Operator* JSOperatorBuilder::StoreContext(size_t depth, size_t index) {
  ContextAccess access(depth, index, false);
  return new (zone()) Operator1<ContextAccess>(  // --
      IrOpcode::kJSStoreContext,                 // opcode
      Operator::kNoRead | Operator::kNoThrow,    // flags
      "JSStoreContext",                          // name
      2, 1, 1, 0, 1, 0,                          // counts
      access);                                   // parameter
}


const Operator* JSOperatorBuilder::CreateArguments(CreateArgumentsType type) {
  return new (zone()) Operator1<CreateArgumentsType>(    // --
      IrOpcode::kJSCreateArguments, Operator::kNoThrow,  // opcode
      "JSCreateArguments",                               // name
      1, 1, 1, 1, 1, 0,                                  // counts
      type);                                             // parameter
}


const Operator* JSOperatorBuilder::CreateArray(size_t arity,
                                               Handle<AllocationSite> site) {
  // constructor, new_target, arg1, ..., argN
  int const value_input_count = static_cast<int>(arity) + 2;
  CreateArrayParameters parameters(arity, site);
  return new (zone()) Operator1<CreateArrayParameters>(   // --
      IrOpcode::kJSCreateArray, Operator::kNoProperties,  // opcode
      "JSCreateArray",                                    // name
      value_input_count, 1, 1, 1, 1, 2,                   // counts
      parameters);                                        // parameter
}


const Operator* JSOperatorBuilder::CreateClosure(
    Handle<SharedFunctionInfo> shared_info, PretenureFlag pretenure) {
  CreateClosureParameters parameters(shared_info, pretenure);
  return new (zone()) Operator1<CreateClosureParameters>(  // --
      IrOpcode::kJSCreateClosure, Operator::kNoThrow,      // opcode
      "JSCreateClosure",                                   // name
      0, 1, 1, 1, 1, 0,                                    // counts
      parameters);                                         // parameter
}

const Operator* JSOperatorBuilder::CreateLiteralArray(
    Handle<FixedArray> constant_elements, int literal_flags, int literal_index,
    int number_of_elements) {
  CreateLiteralParameters parameters(constant_elements, number_of_elements,
                                     literal_flags, literal_index);
  return new (zone()) Operator1<CreateLiteralParameters>(        // --
      IrOpcode::kJSCreateLiteralArray, Operator::kNoProperties,  // opcode
      "JSCreateLiteralArray",                                    // name
      1, 1, 1, 1, 1, 2,                                          // counts
      parameters);                                               // parameter
}

const Operator* JSOperatorBuilder::CreateLiteralObject(
    Handle<FixedArray> constant_properties, int literal_flags,
    int literal_index, int number_of_properties) {
  CreateLiteralParameters parameters(constant_properties, number_of_properties,
                                     literal_flags, literal_index);
  return new (zone()) Operator1<CreateLiteralParameters>(         // --
      IrOpcode::kJSCreateLiteralObject, Operator::kNoProperties,  // opcode
      "JSCreateLiteralObject",                                    // name
      1, 1, 1, 1, 1, 2,                                           // counts
      parameters);                                                // parameter
}


const Operator* JSOperatorBuilder::CreateLiteralRegExp(
    Handle<String> constant_pattern, int literal_flags, int literal_index) {
  CreateLiteralParameters parameters(constant_pattern, -1, literal_flags,
                                     literal_index);
  return new (zone()) Operator1<CreateLiteralParameters>(         // --
      IrOpcode::kJSCreateLiteralRegExp, Operator::kNoProperties,  // opcode
      "JSCreateLiteralRegExp",                                    // name
      1, 1, 1, 1, 1, 2,                                           // counts
      parameters);                                                // parameter
}


const Operator* JSOperatorBuilder::CreateFunctionContext(int slot_count) {
  return new (zone()) Operator1<int>(                               // --
      IrOpcode::kJSCreateFunctionContext, Operator::kNoProperties,  // opcode
      "JSCreateFunctionContext",                                    // name
      1, 1, 1, 1, 1, 2,                                             // counts
      slot_count);                                                  // parameter
}


const Operator* JSOperatorBuilder::CreateCatchContext(
    const Handle<String>& name) {
  return new (zone()) Operator1<Handle<String>>(                 // --
      IrOpcode::kJSCreateCatchContext, Operator::kNoProperties,  // opcode
      "JSCreateCatchContext",                                    // name
      2, 1, 1, 1, 1, 2,                                          // counts
      name);                                                     // parameter
}


const Operator* JSOperatorBuilder::CreateBlockContext(
    const Handle<ScopeInfo>& scpope_info) {
  return new (zone()) Operator1<Handle<ScopeInfo>>(              // --
      IrOpcode::kJSCreateBlockContext, Operator::kNoProperties,  // opcode
      "JSCreateBlockContext",                                    // name
      1, 1, 1, 1, 1, 2,                                          // counts
      scpope_info);                                              // parameter
}


const Operator* JSOperatorBuilder::CreateScriptContext(
    const Handle<ScopeInfo>& scpope_info) {
  return new (zone()) Operator1<Handle<ScopeInfo>>(               // --
      IrOpcode::kJSCreateScriptContext, Operator::kNoProperties,  // opcode
      "JSCreateScriptContext",                                    // name
      1, 1, 1, 1, 1, 2,                                           // counts
      scpope_info);                                               // parameter
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
