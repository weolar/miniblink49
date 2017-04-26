// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_JS_OPERATOR_H_
#define V8_COMPILER_JS_OPERATOR_H_

#include "src/runtime/runtime.h"

namespace v8 {
namespace internal {
namespace compiler {

// Forward declarations.
class Operator;
struct JSOperatorGlobalCache;


// Defines a pair of {TypeFeedbackVector} and {TypeFeedbackVectorSlot}, which
// is used to access the type feedback for a certain {Node}.
class VectorSlotPair {
 public:
  VectorSlotPair();
  VectorSlotPair(Handle<TypeFeedbackVector> vector, FeedbackVectorSlot slot)
      : vector_(vector), slot_(slot) {}

  bool IsValid() const { return !vector_.is_null() && !slot_.IsInvalid(); }

  Handle<TypeFeedbackVector> vector() const { return vector_; }
  FeedbackVectorSlot slot() const { return slot_; }

  int index() const;

 private:
  const Handle<TypeFeedbackVector> vector_;
  const FeedbackVectorSlot slot_;
};

bool operator==(VectorSlotPair const&, VectorSlotPair const&);
bool operator!=(VectorSlotPair const&, VectorSlotPair const&);

size_t hash_value(VectorSlotPair const&);


// The ConvertReceiverMode is used as parameter by JSConvertReceiver operators.
ConvertReceiverMode ConvertReceiverModeOf(const Operator* op);


// Defines whether tail call optimization is allowed.
enum class TailCallMode : unsigned { kAllow, kDisallow };

size_t hash_value(TailCallMode);

std::ostream& operator<<(std::ostream&, TailCallMode);


// Defines the arity and the call flags for a JavaScript function call. This is
// used as a parameter by JSCallFunction operators.
class CallFunctionParameters final {
 public:
  CallFunctionParameters(size_t arity, LanguageMode language_mode,
                         VectorSlotPair const& feedback,
                         TailCallMode tail_call_mode,
                         ConvertReceiverMode convert_mode)
      : bit_field_(ArityField::encode(arity) |
                   ConvertReceiverModeField::encode(convert_mode) |
                   LanguageModeField::encode(language_mode) |
                   TailCallModeField::encode(tail_call_mode)),
        feedback_(feedback) {}

  size_t arity() const { return ArityField::decode(bit_field_); }
  LanguageMode language_mode() const {
    return LanguageModeField::decode(bit_field_);
  }
  ConvertReceiverMode convert_mode() const {
    return ConvertReceiverModeField::decode(bit_field_);
  }
  TailCallMode tail_call_mode() const {
    return TailCallModeField::decode(bit_field_);
  }
  VectorSlotPair const& feedback() const { return feedback_; }

  bool operator==(CallFunctionParameters const& that) const {
    return this->bit_field_ == that.bit_field_ &&
           this->feedback_ == that.feedback_;
  }
  bool operator!=(CallFunctionParameters const& that) const {
    return !(*this == that);
  }

 private:
  friend size_t hash_value(CallFunctionParameters const& p) {
    return base::hash_combine(p.bit_field_, p.feedback_);
  }

  typedef BitField<size_t, 0, 27> ArityField;
  typedef BitField<ConvertReceiverMode, 27, 2> ConvertReceiverModeField;
  typedef BitField<LanguageMode, 29, 2> LanguageModeField;
  typedef BitField<TailCallMode, 31, 1> TailCallModeField;

  const uint32_t bit_field_;
  const VectorSlotPair feedback_;
};

size_t hash_value(CallFunctionParameters const&);

std::ostream& operator<<(std::ostream&, CallFunctionParameters const&);

const CallFunctionParameters& CallFunctionParametersOf(const Operator* op);


// Defines the arity and the ID for a runtime function call. This is used as a
// parameter by JSCallRuntime operators.
class CallRuntimeParameters final {
 public:
  CallRuntimeParameters(Runtime::FunctionId id, size_t arity)
      : id_(id), arity_(arity) {}

  Runtime::FunctionId id() const { return id_; }
  size_t arity() const { return arity_; }

 private:
  const Runtime::FunctionId id_;
  const size_t arity_;
};

bool operator==(CallRuntimeParameters const&, CallRuntimeParameters const&);
bool operator!=(CallRuntimeParameters const&, CallRuntimeParameters const&);

size_t hash_value(CallRuntimeParameters const&);

std::ostream& operator<<(std::ostream&, CallRuntimeParameters const&);

const CallRuntimeParameters& CallRuntimeParametersOf(const Operator* op);


// Defines the location of a context slot relative to a specific scope. This is
// used as a parameter by JSLoadContext and JSStoreContext operators and allows
// accessing a context-allocated variable without keeping track of the scope.
class ContextAccess final {
 public:
  ContextAccess(size_t depth, size_t index, bool immutable);

  size_t depth() const { return depth_; }
  size_t index() const { return index_; }
  bool immutable() const { return immutable_; }

 private:
  // For space reasons, we keep this tightly packed, otherwise we could just use
  // a simple int/int/bool POD.
  const bool immutable_;
  const uint16_t depth_;
  const uint32_t index_;
};

bool operator==(ContextAccess const&, ContextAccess const&);
bool operator!=(ContextAccess const&, ContextAccess const&);

size_t hash_value(ContextAccess const&);

std::ostream& operator<<(std::ostream&, ContextAccess const&);

ContextAccess const& ContextAccessOf(Operator const*);


// Defines the name for a dynamic variable lookup. This is used as a parameter
// by JSLoadDynamic and JSStoreDynamic operators.
class DynamicAccess final {
 public:
  DynamicAccess(const Handle<String>& name, TypeofMode typeof_mode);

  const Handle<String>& name() const { return name_; }
  TypeofMode typeof_mode() const { return typeof_mode_; }

 private:
  const Handle<String> name_;
  const TypeofMode typeof_mode_;
};

size_t hash_value(DynamicAccess const&);

bool operator==(DynamicAccess const&, DynamicAccess const&);
bool operator!=(DynamicAccess const&, DynamicAccess const&);

std::ostream& operator<<(std::ostream&, DynamicAccess const&);

DynamicAccess const& DynamicAccessOf(Operator const*);


// Defines the property of an object for a named access. This is
// used as a parameter by the JSLoadNamed and JSStoreNamed operators.
class NamedAccess final {
 public:
  NamedAccess(LanguageMode language_mode, Handle<Name> name,
              VectorSlotPair const& feedback)
      : name_(name), feedback_(feedback), language_mode_(language_mode) {}

  Handle<Name> name() const { return name_; }
  LanguageMode language_mode() const { return language_mode_; }
  VectorSlotPair const& feedback() const { return feedback_; }

 private:
  Handle<Name> const name_;
  VectorSlotPair const feedback_;
  LanguageMode const language_mode_;
};

bool operator==(NamedAccess const&, NamedAccess const&);
bool operator!=(NamedAccess const&, NamedAccess const&);

size_t hash_value(NamedAccess const&);

std::ostream& operator<<(std::ostream&, NamedAccess const&);

const NamedAccess& NamedAccessOf(const Operator* op);


// Defines the property being loaded from an object by a named load. This is
// used as a parameter by JSLoadGlobal operator.
class LoadGlobalParameters final {
 public:
  LoadGlobalParameters(const Handle<Name>& name, const VectorSlotPair& feedback,
                       TypeofMode typeof_mode)
      : name_(name), feedback_(feedback), typeof_mode_(typeof_mode) {}

  const Handle<Name>& name() const { return name_; }
  TypeofMode typeof_mode() const { return typeof_mode_; }

  const VectorSlotPair& feedback() const { return feedback_; }

 private:
  const Handle<Name> name_;
  const VectorSlotPair feedback_;
  const TypeofMode typeof_mode_;
};

bool operator==(LoadGlobalParameters const&, LoadGlobalParameters const&);
bool operator!=(LoadGlobalParameters const&, LoadGlobalParameters const&);

size_t hash_value(LoadGlobalParameters const&);

std::ostream& operator<<(std::ostream&, LoadGlobalParameters const&);

const LoadGlobalParameters& LoadGlobalParametersOf(const Operator* op);


// Defines the property being stored to an object by a named store. This is
// used as a parameter by JSStoreGlobal operator.
class StoreGlobalParameters final {
 public:
  StoreGlobalParameters(LanguageMode language_mode,
                        const VectorSlotPair& feedback,
                        const Handle<Name>& name)
      : language_mode_(language_mode), name_(name), feedback_(feedback) {}

  LanguageMode language_mode() const { return language_mode_; }
  const VectorSlotPair& feedback() const { return feedback_; }
  const Handle<Name>& name() const { return name_; }

 private:
  const LanguageMode language_mode_;
  const Handle<Name> name_;
  const VectorSlotPair feedback_;
};

bool operator==(StoreGlobalParameters const&, StoreGlobalParameters const&);
bool operator!=(StoreGlobalParameters const&, StoreGlobalParameters const&);

size_t hash_value(StoreGlobalParameters const&);

std::ostream& operator<<(std::ostream&, StoreGlobalParameters const&);

const StoreGlobalParameters& StoreGlobalParametersOf(const Operator* op);


// Defines the property of an object for a keyed access. This is used
// as a parameter by the JSLoadProperty and JSStoreProperty operators.
class PropertyAccess final {
 public:
  PropertyAccess(LanguageMode language_mode, VectorSlotPair const& feedback)
      : feedback_(feedback), language_mode_(language_mode) {}

  LanguageMode language_mode() const { return language_mode_; }
  VectorSlotPair const& feedback() const { return feedback_; }

 private:
  VectorSlotPair const feedback_;
  LanguageMode const language_mode_;
};

bool operator==(PropertyAccess const&, PropertyAccess const&);
bool operator!=(PropertyAccess const&, PropertyAccess const&);

size_t hash_value(PropertyAccess const&);

std::ostream& operator<<(std::ostream&, PropertyAccess const&);

PropertyAccess const& PropertyAccessOf(const Operator* op);


// Defines specifics about arguments object or rest parameter creation. This is
// used as a parameter by JSCreateArguments operators.
class CreateArgumentsParameters final {
 public:
  enum Type { kMappedArguments, kUnmappedArguments, kRestArray };
  CreateArgumentsParameters(Type type, int start_index)
      : type_(type), start_index_(start_index) {}

  Type type() const { return type_; }
  int start_index() const { return start_index_; }

 private:
  const Type type_;
  const int start_index_;
};

bool operator==(CreateArgumentsParameters const&,
                CreateArgumentsParameters const&);
bool operator!=(CreateArgumentsParameters const&,
                CreateArgumentsParameters const&);

size_t hash_value(CreateArgumentsParameters const&);

std::ostream& operator<<(std::ostream&, CreateArgumentsParameters const&);

const CreateArgumentsParameters& CreateArgumentsParametersOf(
    const Operator* op);


// Defines shared information for the closure that should be created. This is
// used as a parameter by JSCreateClosure operators.
class CreateClosureParameters final {
 public:
  CreateClosureParameters(Handle<SharedFunctionInfo> shared_info,
                          PretenureFlag pretenure)
      : shared_info_(shared_info), pretenure_(pretenure) {}

  Handle<SharedFunctionInfo> shared_info() const { return shared_info_; }
  PretenureFlag pretenure() const { return pretenure_; }

 private:
  const Handle<SharedFunctionInfo> shared_info_;
  const PretenureFlag pretenure_;
};

bool operator==(CreateClosureParameters const&, CreateClosureParameters const&);
bool operator!=(CreateClosureParameters const&, CreateClosureParameters const&);

size_t hash_value(CreateClosureParameters const&);

std::ostream& operator<<(std::ostream&, CreateClosureParameters const&);

const CreateClosureParameters& CreateClosureParametersOf(const Operator* op);


// Interface for building JavaScript-level operators, e.g. directly from the
// AST. Most operators have no parameters, thus can be globally shared for all
// graphs.
class JSOperatorBuilder final : public ZoneObject {
 public:
  explicit JSOperatorBuilder(Zone* zone);

  const Operator* Equal();
  const Operator* NotEqual();
  const Operator* StrictEqual();
  const Operator* StrictNotEqual();
  const Operator* LessThan(LanguageMode language_mode);
  const Operator* GreaterThan(LanguageMode language_mode);
  const Operator* LessThanOrEqual(LanguageMode language_mode);
  const Operator* GreaterThanOrEqual(LanguageMode language_mode);
  const Operator* BitwiseOr(LanguageMode language_mode);
  const Operator* BitwiseXor(LanguageMode language_mode);
  const Operator* BitwiseAnd(LanguageMode language_mode);
  const Operator* ShiftLeft(LanguageMode language_mode);
  const Operator* ShiftRight(LanguageMode language_mode);
  const Operator* ShiftRightLogical(LanguageMode language_mode);
  const Operator* Add(LanguageMode language_mode);
  const Operator* Subtract(LanguageMode language_mode);
  const Operator* Multiply(LanguageMode language_mode);
  const Operator* Divide(LanguageMode language_mode);
  const Operator* Modulus(LanguageMode language_mode);

  const Operator* UnaryNot();
  const Operator* ToBoolean();
  const Operator* ToNumber();
  const Operator* ToString();
  const Operator* ToName();
  const Operator* ToObject();
  const Operator* Yield();

  const Operator* Create();
  const Operator* CreateArguments(CreateArgumentsParameters::Type type,
                                  int start_index);
  const Operator* CreateClosure(Handle<SharedFunctionInfo> shared_info,
                                PretenureFlag pretenure);
  const Operator* CreateLiteralArray(int literal_flags);
  const Operator* CreateLiteralObject(int literal_flags);

  const Operator* CallFunction(
      size_t arity, LanguageMode language_mode,
      VectorSlotPair const& feedback = VectorSlotPair(),
      ConvertReceiverMode convert_mode = ConvertReceiverMode::kAny,
      TailCallMode tail_call_mode = TailCallMode::kDisallow);
  const Operator* CallRuntime(Runtime::FunctionId id, size_t arity);
  const Operator* CallConstruct(int arguments);

  const Operator* ConvertReceiver(ConvertReceiverMode convert_mode);

  const Operator* LoadProperty(LanguageMode language_mode,
                               VectorSlotPair const& feedback);
  const Operator* LoadNamed(LanguageMode language_mode, Handle<Name> name,
                            VectorSlotPair const& feedback);

  const Operator* StoreProperty(LanguageMode language_mode,
                                VectorSlotPair const& feedback);
  const Operator* StoreNamed(LanguageMode language_mode, Handle<Name> name,
                             VectorSlotPair const& feedback);

  const Operator* DeleteProperty(LanguageMode language_mode);

  const Operator* HasProperty();

  const Operator* LoadGlobal(const Handle<Name>& name,
                             const VectorSlotPair& feedback,
                             TypeofMode typeof_mode = NOT_INSIDE_TYPEOF);
  const Operator* StoreGlobal(LanguageMode language_mode,
                              const Handle<Name>& name,
                              const VectorSlotPair& feedback);

  const Operator* LoadContext(size_t depth, size_t index, bool immutable);
  const Operator* StoreContext(size_t depth, size_t index);

  const Operator* LoadDynamic(const Handle<String>& name,
                              TypeofMode typeof_mode);

  const Operator* TypeOf();
  const Operator* InstanceOf();

  const Operator* ForInDone();
  const Operator* ForInNext();
  const Operator* ForInPrepare();
  const Operator* ForInStep();

  const Operator* LoadMessage();
  const Operator* StoreMessage();

  const Operator* StackCheck();

  const Operator* CreateFunctionContext(int slot_count);
  const Operator* CreateCatchContext(const Handle<String>& name);
  const Operator* CreateWithContext();
  const Operator* CreateBlockContext(const Handle<ScopeInfo>& scpope_info);
  const Operator* CreateModuleContext();
  const Operator* CreateScriptContext(const Handle<ScopeInfo>& scpope_info);

 private:
  Zone* zone() const { return zone_; }

  const JSOperatorGlobalCache& cache_;
  Zone* const zone_;

  DISALLOW_COPY_AND_ASSIGN(JSOperatorBuilder);
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_JS_OPERATOR_H_
