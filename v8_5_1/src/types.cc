// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iomanip>

#include "src/types.h"

#include "src/handles-inl.h"
#include "src/ostreams.h"

namespace v8 {
namespace internal {


// NOTE: If code is marked as being a "shortcut", this means that removing
// the code won't affect the semantics of the surrounding function definition.

// static
bool Type::IsInteger(i::Object* x) {
  return x->IsNumber() && Type::IsInteger(x->Number());
}

// -----------------------------------------------------------------------------
// Range-related helper functions.

bool RangeType::Limits::IsEmpty() { return this->min > this->max; }

RangeType::Limits RangeType::Limits::Intersect(Limits lhs, Limits rhs) {
  DisallowHeapAllocation no_allocation;
  Limits result(lhs);
  if (lhs.min < rhs.min) result.min = rhs.min;
  if (lhs.max > rhs.max) result.max = rhs.max;
  return result;
}

RangeType::Limits RangeType::Limits::Union(Limits lhs, Limits rhs) {
  DisallowHeapAllocation no_allocation;
  if (lhs.IsEmpty()) return rhs;
  if (rhs.IsEmpty()) return lhs;
  Limits result(lhs);
  if (lhs.min > rhs.min) result.min = rhs.min;
  if (lhs.max < rhs.max) result.max = rhs.max;
  return result;
}

bool Type::Overlap(RangeType* lhs, RangeType* rhs) {
  DisallowHeapAllocation no_allocation;
  return !RangeType::Limits::Intersect(RangeType::Limits(lhs),
                                       RangeType::Limits(rhs))
              .IsEmpty();
}

bool Type::Contains(RangeType* lhs, RangeType* rhs) {
  DisallowHeapAllocation no_allocation;
  return lhs->Min() <= rhs->Min() && rhs->Max() <= lhs->Max();
}

bool Type::Contains(RangeType* lhs, ConstantType* rhs) {
  DisallowHeapAllocation no_allocation;
  return IsInteger(*rhs->Value()) &&
         lhs->Min() <= rhs->Value()->Number() &&
         rhs->Value()->Number() <= lhs->Max();
}

bool Type::Contains(RangeType* range, i::Object* val) {
  DisallowHeapAllocation no_allocation;
  return IsInteger(val) &&
         range->Min() <= val->Number() && val->Number() <= range->Max();
}


// -----------------------------------------------------------------------------
// Min and Max computation.

double Type::Min() {
  DCHECK(this->SemanticIs(Number()));
  if (this->IsBitset()) return BitsetType::Min(this->AsBitset());
  if (this->IsUnion()) {
    double min = +V8_INFINITY;
    for (int i = 0, n = this->AsUnion()->Length(); i < n; ++i) {
      min = std::min(min, this->AsUnion()->Get(i)->Min());
    }
    return min;
  }
  if (this->IsRange()) return this->AsRange()->Min();
  if (this->IsConstant()) return this->AsConstant()->Value()->Number();
  UNREACHABLE();
  return 0;
}

double Type::Max() {
  DCHECK(this->SemanticIs(Number()));
  if (this->IsBitset()) return BitsetType::Max(this->AsBitset());
  if (this->IsUnion()) {
    double max = -V8_INFINITY;
    for (int i = 0, n = this->AsUnion()->Length(); i < n; ++i) {
      max = std::max(max, this->AsUnion()->Get(i)->Max());
    }
    return max;
  }
  if (this->IsRange()) return this->AsRange()->Max();
  if (this->IsConstant()) return this->AsConstant()->Value()->Number();
  UNREACHABLE();
  return 0;
}


// -----------------------------------------------------------------------------
// Glb and lub computation.


// The largest bitset subsumed by this type.
Type::bitset BitsetType::Glb(Type* type) {
  DisallowHeapAllocation no_allocation;
  // Fast case.
  if (IsBitset(type)) {
    return type->AsBitset();
  } else if (type->IsUnion()) {
    SLOW_DCHECK(type->AsUnion()->Wellformed());
    return type->AsUnion()->Get(0)->BitsetGlb() |
           SEMANTIC(type->AsUnion()->Get(1)->BitsetGlb());  // Shortcut.
  } else if (type->IsRange()) {
    bitset glb = SEMANTIC(
        BitsetType::Glb(type->AsRange()->Min(), type->AsRange()->Max()));
    return glb | REPRESENTATION(type->BitsetLub());
  } else {
    return type->Representation();
  }
}


// The smallest bitset subsuming this type, possibly not a proper one.
Type::bitset BitsetType::Lub(Type* type) {
  DisallowHeapAllocation no_allocation;
  if (IsBitset(type)) return type->AsBitset();
  if (type->IsUnion()) {
    // Take the representation from the first element, which is always
    // a bitset.
    int bitset = type->AsUnion()->Get(0)->BitsetLub();
    for (int i = 0, n = type->AsUnion()->Length(); i < n; ++i) {
      // Other elements only contribute their semantic part.
      bitset |= SEMANTIC(type->AsUnion()->Get(i)->BitsetLub());
    }
    return bitset;
  }
  if (type->IsClass()) return type->AsClass()->Lub();
  if (type->IsConstant()) return type->AsConstant()->Lub();
  if (type->IsRange()) return type->AsRange()->Lub();
  if (type->IsContext()) return kInternal & kTaggedPointer;
  if (type->IsArray()) return kOtherObject;
  if (type->IsFunction()) return kFunction;
  if (type->IsTuple()) return kInternal;
  UNREACHABLE();
  return kNone;
}

Type::bitset BitsetType::Lub(i::Map* map) {
  DisallowHeapAllocation no_allocation;
  switch (map->instance_type()) {
    case STRING_TYPE:
    case ONE_BYTE_STRING_TYPE:
    case CONS_STRING_TYPE:
    case CONS_ONE_BYTE_STRING_TYPE:
    case SLICED_STRING_TYPE:
    case SLICED_ONE_BYTE_STRING_TYPE:
    case EXTERNAL_STRING_TYPE:
    case EXTERNAL_ONE_BYTE_STRING_TYPE:
    case EXTERNAL_STRING_WITH_ONE_BYTE_DATA_TYPE:
    case SHORT_EXTERNAL_STRING_TYPE:
    case SHORT_EXTERNAL_ONE_BYTE_STRING_TYPE:
    case SHORT_EXTERNAL_STRING_WITH_ONE_BYTE_DATA_TYPE:
      return kOtherString;
    case INTERNALIZED_STRING_TYPE:
    case ONE_BYTE_INTERNALIZED_STRING_TYPE:
    case EXTERNAL_INTERNALIZED_STRING_TYPE:
    case EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE:
    case EXTERNAL_INTERNALIZED_STRING_WITH_ONE_BYTE_DATA_TYPE:
    case SHORT_EXTERNAL_INTERNALIZED_STRING_TYPE:
    case SHORT_EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE:
    case SHORT_EXTERNAL_INTERNALIZED_STRING_WITH_ONE_BYTE_DATA_TYPE:
      return kInternalizedString;
    case SYMBOL_TYPE:
      return kSymbol;
    case ODDBALL_TYPE: {
      Heap* heap = map->GetHeap();
      if (map == heap->undefined_map()) return kUndefined;
      if (map == heap->null_map()) return kNull;
      if (map == heap->boolean_map()) return kBoolean;
      DCHECK(map == heap->the_hole_map() ||
             map == heap->uninitialized_map() ||
             map == heap->no_interceptor_result_sentinel_map() ||
             map == heap->termination_exception_map() ||
             map == heap->arguments_marker_map() ||
             map == heap->optimized_out_map());
      return kInternal & kTaggedPointer;
    }
    case HEAP_NUMBER_TYPE:
      return kNumber & kTaggedPointer;
    case SIMD128_VALUE_TYPE:
      return kSimd;
    case JS_OBJECT_TYPE:
    case JS_GLOBAL_OBJECT_TYPE:
    case JS_GLOBAL_PROXY_TYPE:
    case JS_SPECIAL_API_OBJECT_TYPE:
      if (map->is_undetectable()) return kOtherUndetectable;
      return kOtherObject;
    case JS_VALUE_TYPE:
    case JS_MESSAGE_OBJECT_TYPE:
    case JS_DATE_TYPE:
    case JS_CONTEXT_EXTENSION_OBJECT_TYPE:
    case JS_GENERATOR_OBJECT_TYPE:
    case JS_MODULE_TYPE:
    case JS_ARRAY_BUFFER_TYPE:
    case JS_ARRAY_TYPE:
    case JS_REGEXP_TYPE:  // TODO(rossberg): there should be a RegExp type.
    case JS_TYPED_ARRAY_TYPE:
    case JS_DATA_VIEW_TYPE:
    case JS_SET_TYPE:
    case JS_MAP_TYPE:
    case JS_SET_ITERATOR_TYPE:
    case JS_MAP_ITERATOR_TYPE:
    case JS_WEAK_MAP_TYPE:
    case JS_WEAK_SET_TYPE:
    case JS_PROMISE_TYPE:
    case JS_BOUND_FUNCTION_TYPE:
      DCHECK(!map->is_undetectable());
      return kOtherObject;
    case JS_FUNCTION_TYPE:
      DCHECK(!map->is_undetectable());
      return kFunction;
    case JS_PROXY_TYPE:
      DCHECK(!map->is_undetectable());
      return kProxy;
    case MAP_TYPE:
    case ALLOCATION_SITE_TYPE:
    case ACCESSOR_INFO_TYPE:
    case SHARED_FUNCTION_INFO_TYPE:
    case ACCESSOR_PAIR_TYPE:
    case FIXED_ARRAY_TYPE:
    case FIXED_DOUBLE_ARRAY_TYPE:
    case BYTE_ARRAY_TYPE:
    case BYTECODE_ARRAY_TYPE:
    case TRANSITION_ARRAY_TYPE:
    case FOREIGN_TYPE:
    case SCRIPT_TYPE:
    case CODE_TYPE:
    case PROPERTY_CELL_TYPE:
      return kInternal & kTaggedPointer;

    // Remaining instance types are unsupported for now. If any of them do
    // require bit set types, they should get kInternal & kTaggedPointer.
    case MUTABLE_HEAP_NUMBER_TYPE:
    case FREE_SPACE_TYPE:
#define FIXED_TYPED_ARRAY_CASE(Type, type, TYPE, ctype, size) \
  case FIXED_##TYPE##_ARRAY_TYPE:

      TYPED_ARRAYS(FIXED_TYPED_ARRAY_CASE)
#undef FIXED_TYPED_ARRAY_CASE
    case FILLER_TYPE:
    case ACCESS_CHECK_INFO_TYPE:
    case INTERCEPTOR_INFO_TYPE:
    case CALL_HANDLER_INFO_TYPE:
    case FUNCTION_TEMPLATE_INFO_TYPE:
    case OBJECT_TEMPLATE_INFO_TYPE:
    case SIGNATURE_INFO_TYPE:
    case TYPE_SWITCH_INFO_TYPE:
    case ALLOCATION_MEMENTO_TYPE:
    case CODE_CACHE_TYPE:
    case POLYMORPHIC_CODE_CACHE_TYPE:
    case TYPE_FEEDBACK_INFO_TYPE:
    case ALIASED_ARGUMENTS_ENTRY_TYPE:
    case BOX_TYPE:
    case DEBUG_INFO_TYPE:
    case BREAK_POINT_INFO_TYPE:
    case CELL_TYPE:
    case WEAK_CELL_TYPE:
    case PROTOTYPE_INFO_TYPE:
    case SLOPPY_BLOCK_WITH_EVAL_CONTEXT_EXTENSION_TYPE:
      UNREACHABLE();
      return kNone;
  }
  UNREACHABLE();
  return kNone;
}

Type::bitset BitsetType::Lub(i::Object* value) {
  DisallowHeapAllocation no_allocation;
  if (value->IsNumber()) {
    return Lub(value->Number()) &
        (value->IsSmi() ? kTaggedSigned : kTaggedPointer);
  }
  return Lub(i::HeapObject::cast(value)->map());
}

Type::bitset BitsetType::Lub(double value) {
  DisallowHeapAllocation no_allocation;
  if (i::IsMinusZero(value)) return kMinusZero;
  if (std::isnan(value)) return kNaN;
  if (IsUint32Double(value) || IsInt32Double(value)) return Lub(value, value);
  return kOtherNumber;
}


// Minimum values of plain numeric bitsets.
const BitsetType::Boundary BitsetType::BoundariesArray[] = {
    {kOtherNumber, kPlainNumber, -V8_INFINITY},
    {kOtherSigned32, kNegative32, kMinInt},
    {kNegative31, kNegative31, -0x40000000},
    {kUnsigned30, kUnsigned30, 0},
    {kOtherUnsigned31, kUnsigned31, 0x40000000},
    {kOtherUnsigned32, kUnsigned32, 0x80000000},
    {kOtherNumber, kPlainNumber, static_cast<double>(kMaxUInt32) + 1}};

const BitsetType::Boundary* BitsetType::Boundaries() { return BoundariesArray; }

size_t BitsetType::BoundariesSize() {
  // Windows doesn't like arraysize here.
  // return arraysize(BoundariesArray);
  return 7;
}

Type::bitset BitsetType::ExpandInternals(Type::bitset bits) {
  DisallowHeapAllocation no_allocation;
  if (!(bits & SEMANTIC(kPlainNumber))) return bits;  // Shortcut.
  const Boundary* boundaries = Boundaries();
  for (size_t i = 0; i < BoundariesSize(); ++i) {
    DCHECK(BitsetType::Is(boundaries[i].internal, boundaries[i].external));
    if (bits & SEMANTIC(boundaries[i].internal))
      bits |= SEMANTIC(boundaries[i].external);
  }
  return bits;
}

Type::bitset BitsetType::Lub(double min, double max) {
  DisallowHeapAllocation no_allocation;
  int lub = kNone;
  const Boundary* mins = Boundaries();

  for (size_t i = 1; i < BoundariesSize(); ++i) {
    if (min < mins[i].min) {
      lub |= mins[i-1].internal;
      if (max < mins[i].min) return lub;
    }
  }
  return lub | mins[BoundariesSize() - 1].internal;
}

Type::bitset BitsetType::NumberBits(bitset bits) {
  return SEMANTIC(bits & kPlainNumber);
}

Type::bitset BitsetType::Glb(double min, double max) {
  DisallowHeapAllocation no_allocation;
  int glb = kNone;
  const Boundary* mins = Boundaries();

  // If the range does not touch 0, the bound is empty.
  if (max < -1 || min > 0) return glb;

  for (size_t i = 1; i + 1 < BoundariesSize(); ++i) {
    if (min <= mins[i].min) {
      if (max + 1 < mins[i + 1].min) break;
      glb |= mins[i].external;
    }
  }
  // OtherNumber also contains float numbers, so it can never be
  // in the greatest lower bound.
  return glb & ~(SEMANTIC(kOtherNumber));
}

double BitsetType::Min(bitset bits) {
  DisallowHeapAllocation no_allocation;
  DCHECK(Is(SEMANTIC(bits), kNumber));
  const Boundary* mins = Boundaries();
  bool mz = SEMANTIC(bits & kMinusZero);
  for (size_t i = 0; i < BoundariesSize(); ++i) {
    if (Is(SEMANTIC(mins[i].internal), bits)) {
      return mz ? std::min(0.0, mins[i].min) : mins[i].min;
    }
  }
  if (mz) return 0;
  return std::numeric_limits<double>::quiet_NaN();
}

double BitsetType::Max(bitset bits) {
  DisallowHeapAllocation no_allocation;
  DCHECK(Is(SEMANTIC(bits), kNumber));
  const Boundary* mins = Boundaries();
  bool mz = SEMANTIC(bits & kMinusZero);
  if (BitsetType::Is(SEMANTIC(mins[BoundariesSize() - 1].internal), bits)) {
    return +V8_INFINITY;
  }
  for (size_t i = BoundariesSize() - 1; i-- > 0;) {
    if (Is(SEMANTIC(mins[i].internal), bits)) {
      return mz ?
          std::max(0.0, mins[i+1].min - 1) : mins[i+1].min - 1;
    }
  }
  if (mz) return 0;
  return std::numeric_limits<double>::quiet_NaN();
}


// -----------------------------------------------------------------------------
// Predicates.

bool Type::SimplyEquals(Type* that) {
  DisallowHeapAllocation no_allocation;
  if (this->IsClass()) {
    return that->IsClass()
        && *this->AsClass()->Map() == *that->AsClass()->Map();
  }
  if (this->IsConstant()) {
    return that->IsConstant()
        && *this->AsConstant()->Value() == *that->AsConstant()->Value();
  }
  if (this->IsContext()) {
    return that->IsContext()
        && this->AsContext()->Outer()->Equals(that->AsContext()->Outer());
  }
  if (this->IsArray()) {
    return that->IsArray()
        && this->AsArray()->Element()->Equals(that->AsArray()->Element());
  }
  if (this->IsFunction()) {
    if (!that->IsFunction()) return false;
    FunctionType* this_fun = this->AsFunction();
    FunctionType* that_fun = that->AsFunction();
    if (this_fun->Arity() != that_fun->Arity() ||
        !this_fun->Result()->Equals(that_fun->Result()) ||
        !this_fun->Receiver()->Equals(that_fun->Receiver())) {
      return false;
    }
    for (int i = 0, n = this_fun->Arity(); i < n; ++i) {
      if (!this_fun->Parameter(i)->Equals(that_fun->Parameter(i))) return false;
    }
    return true;
  }
  if (this->IsTuple()) {
    if (!that->IsTuple()) return false;
    TupleType* this_tuple = this->AsTuple();
    TupleType* that_tuple = that->AsTuple();
    if (this_tuple->Arity() != that_tuple->Arity()) {
      return false;
    }
    for (int i = 0, n = this_tuple->Arity(); i < n; ++i) {
      if (!this_tuple->Element(i)->Equals(that_tuple->Element(i))) return false;
    }
    return true;
  }
  UNREACHABLE();
  return false;
}

Type::bitset Type::Representation() {
  return REPRESENTATION(this->BitsetLub());
}


// Check if [this] <= [that].
bool Type::SlowIs(Type* that) {
  DisallowHeapAllocation no_allocation;

  // Fast bitset cases
  if (that->IsBitset()) {
    return BitsetType::Is(this->BitsetLub(), that->AsBitset());
  }

  if (this->IsBitset()) {
    return BitsetType::Is(this->AsBitset(), that->BitsetGlb());
  }

  // Check the representations.
  if (!BitsetType::Is(Representation(), that->Representation())) {
    return false;
  }

  // Check the semantic part.
  return SemanticIs(that);
}


// Check if SEMANTIC([this]) <= SEMANTIC([that]). The result of the method
// should be independent of the representation axis of the types.
bool Type::SemanticIs(Type* that) {
  DisallowHeapAllocation no_allocation;

  if (this == that) return true;

  if (that->IsBitset()) {
    return BitsetType::Is(SEMANTIC(this->BitsetLub()), that->AsBitset());
  }
  if (this->IsBitset()) {
    return BitsetType::Is(SEMANTIC(this->AsBitset()), that->BitsetGlb());
  }

  // (T1 \/ ... \/ Tn) <= T  if  (T1 <= T) /\ ... /\ (Tn <= T)
  if (this->IsUnion()) {
    for (int i = 0, n = this->AsUnion()->Length(); i < n; ++i) {
      if (!this->AsUnion()->Get(i)->SemanticIs(that)) return false;
    }
    return true;
  }

  // T <= (T1 \/ ... \/ Tn)  if  (T <= T1) \/ ... \/ (T <= Tn)
  if (that->IsUnion()) {
    for (int i = 0, n = that->AsUnion()->Length(); i < n; ++i) {
      if (this->SemanticIs(that->AsUnion()->Get(i))) return true;
      if (i > 1 && this->IsRange()) return false;  // Shortcut.
    }
    return false;
  }

  if (that->IsRange()) {
    return (this->IsRange() && Contains(that->AsRange(), this->AsRange())) ||
           (this->IsConstant() &&
            Contains(that->AsRange(), this->AsConstant()));
  }
  if (this->IsRange()) return false;

  return this->SimplyEquals(that);
}

// Most precise _current_ type of a value (usually its class).
Type* Type::NowOf(i::Object* value, Zone* zone) {
  if (value->IsSmi() ||
      i::HeapObject::cast(value)->map()->instance_type() == HEAP_NUMBER_TYPE) {
    return Of(value, zone);
  }
  return Class(i::handle(i::HeapObject::cast(value)->map()), zone);
}

bool Type::NowContains(i::Object* value) {
  DisallowHeapAllocation no_allocation;
  if (this->IsAny()) return true;
  if (value->IsHeapObject()) {
    i::Map* map = i::HeapObject::cast(value)->map();
    for (Iterator<i::Map> it = this->Classes(); !it.Done(); it.Advance()) {
      if (*it.Current() == map) return true;
    }
  }
  return this->Contains(value);
}

bool Type::NowIs(Type* that) {
  DisallowHeapAllocation no_allocation;

  // TODO(rossberg): this is incorrect for
  //   Union(Constant(V), T)->NowIs(Class(M))
  // but fuzzing does not cover that!
  if (this->IsConstant()) {
    i::Object* object = *this->AsConstant()->Value();
    if (object->IsHeapObject()) {
      i::Map* map = i::HeapObject::cast(object)->map();
      for (Iterator<i::Map> it = that->Classes(); !it.Done(); it.Advance()) {
        if (*it.Current() == map) return true;
      }
    }
  }
  return this->Is(that);
}


// Check if [this] contains only (currently) stable classes.
bool Type::NowStable() {
  DisallowHeapAllocation no_allocation;
  return !this->IsClass() || this->AsClass()->Map()->is_stable();
}


// Check if [this] and [that] overlap.
bool Type::Maybe(Type* that) {
  DisallowHeapAllocation no_allocation;

  // Take care of the representation part (and also approximate
  // the semantic part).
  if (!BitsetType::IsInhabited(this->BitsetLub() & that->BitsetLub()))
    return false;

  return SemanticMaybe(that);
}

bool Type::SemanticMaybe(Type* that) {
  DisallowHeapAllocation no_allocation;

  // (T1 \/ ... \/ Tn) overlaps T  if  (T1 overlaps T) \/ ... \/ (Tn overlaps T)
  if (this->IsUnion()) {
    for (int i = 0, n = this->AsUnion()->Length(); i < n; ++i) {
      if (this->AsUnion()->Get(i)->SemanticMaybe(that)) return true;
    }
    return false;
  }

  // T overlaps (T1 \/ ... \/ Tn)  if  (T overlaps T1) \/ ... \/ (T overlaps Tn)
  if (that->IsUnion()) {
    for (int i = 0, n = that->AsUnion()->Length(); i < n; ++i) {
      if (this->SemanticMaybe(that->AsUnion()->Get(i))) return true;
    }
    return false;
  }

  if (!BitsetType::SemanticIsInhabited(this->BitsetLub() & that->BitsetLub()))
    return false;

  if (this->IsBitset() && that->IsBitset()) return true;

  if (this->IsClass() != that->IsClass()) return true;

  if (this->IsRange()) {
    if (that->IsConstant()) {
      return Contains(this->AsRange(), that->AsConstant());
    }
    if (that->IsRange()) {
      return Overlap(this->AsRange(), that->AsRange());
    }
    if (that->IsBitset()) {
      bitset number_bits = BitsetType::NumberBits(that->AsBitset());
      if (number_bits == BitsetType::kNone) {
        return false;
      }
      double min = std::max(BitsetType::Min(number_bits), this->Min());
      double max = std::min(BitsetType::Max(number_bits), this->Max());
      return min <= max;
    }
  }
  if (that->IsRange()) {
    return that->SemanticMaybe(this);  // This case is handled above.
  }

  if (this->IsBitset() || that->IsBitset()) return true;

  return this->SimplyEquals(that);
}


// Return the range in [this], or [NULL].
Type* Type::GetRange() {
  DisallowHeapAllocation no_allocation;
  if (this->IsRange()) return this;
  if (this->IsUnion() && this->AsUnion()->Get(1)->IsRange()) {
    return this->AsUnion()->Get(1);
  }
  return NULL;
}

bool Type::Contains(i::Object* value) {
  DisallowHeapAllocation no_allocation;
  for (Iterator<i::Object> it = this->Constants(); !it.Done(); it.Advance()) {
    if (*it.Current() == value) return true;
  }
  if (IsInteger(value)) {
    Type* range = this->GetRange();
    if (range != NULL && Contains(range->AsRange(), value)) return true;
  }
  return BitsetType::New(BitsetType::Lub(value))->Is(this);
}

bool UnionType::Wellformed() {
  DisallowHeapAllocation no_allocation;
  // This checks the invariants of the union representation:
  // 1. There are at least two elements.
  // 2. The first element is a bitset, no other element is a bitset.
  // 3. At most one element is a range, and it must be the second one.
  // 4. No element is itself a union.
  // 5. No element (except the bitset) is a subtype of any other.
  // 6. If there is a range, then the bitset type does not contain
  //    plain number bits.
  DCHECK(this->Length() >= 2);  // (1)
  DCHECK(this->Get(0)->IsBitset());  // (2a)

  for (int i = 0; i < this->Length(); ++i) {
    if (i != 0) DCHECK(!this->Get(i)->IsBitset());  // (2b)
    if (i != 1) DCHECK(!this->Get(i)->IsRange());   // (3)
    DCHECK(!this->Get(i)->IsUnion());               // (4)
    for (int j = 0; j < this->Length(); ++j) {
      if (i != j && i != 0)
        DCHECK(!this->Get(i)->SemanticIs(this->Get(j)));  // (5)
    }
  }
  DCHECK(!this->Get(1)->IsRange() ||
         (BitsetType::NumberBits(this->Get(0)->AsBitset()) ==
          BitsetType::kNone));  // (6)
  return true;
}


// -----------------------------------------------------------------------------
// Union and intersection


static bool AddIsSafe(int x, int y) {
  return x >= 0 ?
      y <= std::numeric_limits<int>::max() - x :
      y >= std::numeric_limits<int>::min() - x;
}

Type* Type::Intersect(Type* type1, Type* type2, Zone* zone) {
  // Fast case: bit sets.
  if (type1->IsBitset() && type2->IsBitset()) {
    return BitsetType::New(type1->AsBitset() & type2->AsBitset());
  }

  // Fast case: top or bottom types.
  if (type1->IsNone() || type2->IsAny()) return type1;  // Shortcut.
  if (type2->IsNone() || type1->IsAny()) return type2;  // Shortcut.

  // Semi-fast case.
  if (type1->Is(type2)) return type1;
  if (type2->Is(type1)) return type2;

  // Slow case: create union.

  // Figure out the representation of the result first.
  // The rest of the method should not change this representation and
  // it should not make any decisions based on representations (i.e.,
  // it should only use the semantic part of types).
  const bitset representation =
      type1->Representation() & type2->Representation();

  // Semantic subtyping check - this is needed for consistency with the
  // semi-fast case above - we should behave the same way regardless of
  // representations. Intersection with a universal bitset should only update
  // the representations.
  if (type1->SemanticIs(type2)) {
    type2 = Any();
  } else if (type2->SemanticIs(type1)) {
    type1 = Any();
  }

  bitset bits =
      SEMANTIC(type1->BitsetGlb() & type2->BitsetGlb()) | representation;
  int size1 = type1->IsUnion() ? type1->AsUnion()->Length() : 1;
  int size2 = type2->IsUnion() ? type2->AsUnion()->Length() : 1;
  if (!AddIsSafe(size1, size2)) return Any();
  int size = size1 + size2;
  if (!AddIsSafe(size, 2)) return Any();
  size += 2;
  Type* result_type = UnionType::New(size, zone);
  UnionType* result = result_type->AsUnion();
  size = 0;

  // Deal with bitsets.
  result->Set(size++, BitsetType::New(bits));

  RangeType::Limits lims = RangeType::Limits::Empty();
  size = IntersectAux(type1, type2, result, size, &lims, zone);

  // If the range is not empty, then insert it into the union and
  // remove the number bits from the bitset.
  if (!lims.IsEmpty()) {
    size = UpdateRange(RangeType::New(lims, representation, zone), result, size,
                       zone);

    // Remove the number bits.
    bitset number_bits = BitsetType::NumberBits(bits);
    bits &= ~number_bits;
    result->Set(0, BitsetType::New(bits));
  }
  return NormalizeUnion(result_type, size, zone);
}

int Type::UpdateRange(Type* range, UnionType* result, int size, Zone* zone) {
  if (size == 1) {
    result->Set(size++, range);
  } else {
    // Make space for the range.
    result->Set(size++, result->Get(1));
    result->Set(1, range);
  }

  // Remove any components that just got subsumed.
  for (int i = 2; i < size; ) {
    if (result->Get(i)->SemanticIs(range)) {
      result->Set(i, result->Get(--size));
    } else {
      ++i;
    }
  }
  return size;
}

RangeType::Limits Type::ToLimits(bitset bits, Zone* zone) {
  bitset number_bits = BitsetType::NumberBits(bits);

  if (number_bits == BitsetType::kNone) {
    return RangeType::Limits::Empty();
  }

  return RangeType::Limits(BitsetType::Min(number_bits),
                           BitsetType::Max(number_bits));
}

RangeType::Limits Type::IntersectRangeAndBitset(Type* range, Type* bitset,
                                                Zone* zone) {
  RangeType::Limits range_lims(range->AsRange());
  RangeType::Limits bitset_lims = ToLimits(bitset->AsBitset(), zone);
  return RangeType::Limits::Intersect(range_lims, bitset_lims);
}

int Type::IntersectAux(Type* lhs, Type* rhs, UnionType* result, int size,
                       RangeType::Limits* lims, Zone* zone) {
  if (lhs->IsUnion()) {
    for (int i = 0, n = lhs->AsUnion()->Length(); i < n; ++i) {
      size =
          IntersectAux(lhs->AsUnion()->Get(i), rhs, result, size, lims, zone);
    }
    return size;
  }
  if (rhs->IsUnion()) {
    for (int i = 0, n = rhs->AsUnion()->Length(); i < n; ++i) {
      size =
          IntersectAux(lhs, rhs->AsUnion()->Get(i), result, size, lims, zone);
    }
    return size;
  }

  if (!BitsetType::SemanticIsInhabited(lhs->BitsetLub() & rhs->BitsetLub())) {
    return size;
  }

  if (lhs->IsRange()) {
    if (rhs->IsBitset()) {
      RangeType::Limits lim = IntersectRangeAndBitset(lhs, rhs, zone);

      if (!lim.IsEmpty()) {
        *lims = RangeType::Limits::Union(lim, *lims);
      }
      return size;
    }
    if (rhs->IsClass()) {
      *lims =
          RangeType::Limits::Union(RangeType::Limits(lhs->AsRange()), *lims);
    }
    if (rhs->IsConstant() && Contains(lhs->AsRange(), rhs->AsConstant())) {
      return AddToUnion(rhs, result, size, zone);
    }
    if (rhs->IsRange()) {
      RangeType::Limits lim = RangeType::Limits::Intersect(
          RangeType::Limits(lhs->AsRange()), RangeType::Limits(rhs->AsRange()));
      if (!lim.IsEmpty()) {
        *lims = RangeType::Limits::Union(lim, *lims);
      }
    }
    return size;
  }
  if (rhs->IsRange()) {
    // This case is handled symmetrically above.
    return IntersectAux(rhs, lhs, result, size, lims, zone);
  }
  if (lhs->IsBitset() || rhs->IsBitset()) {
    return AddToUnion(lhs->IsBitset() ? rhs : lhs, result, size, zone);
  }
  if (lhs->IsClass() != rhs->IsClass()) {
    return AddToUnion(lhs->IsClass() ? rhs : lhs, result, size, zone);
  }
  if (lhs->SimplyEquals(rhs)) {
    return AddToUnion(lhs, result, size, zone);
  }
  return size;
}


// Make sure that we produce a well-formed range and bitset:
// If the range is non-empty, the number bits in the bitset should be
// clear. Moreover, if we have a canonical range (such as Signed32),
// we want to produce a bitset rather than a range.
Type* Type::NormalizeRangeAndBitset(Type* range, bitset* bits, Zone* zone) {
  // Fast path: If the bitset does not mention numbers, we can just keep the
  // range.
  bitset number_bits = BitsetType::NumberBits(*bits);
  if (number_bits == 0) {
    return range;
  }

  // If the range is semantically contained within the bitset, return None and
  // leave the bitset untouched.
  bitset range_lub = SEMANTIC(range->BitsetLub());
  if (BitsetType::Is(range_lub, *bits)) {
    return None();
  }

  // Slow path: reconcile the bitset range and the range.
  double bitset_min = BitsetType::Min(number_bits);
  double bitset_max = BitsetType::Max(number_bits);

  double range_min = range->Min();
  double range_max = range->Max();

  // Remove the number bits from the bitset, they would just confuse us now.
  // NOTE: bits contains OtherNumber iff bits contains PlainNumber, in which
  // case we already returned after the subtype check above.
  *bits &= ~number_bits;

  if (range_min <= bitset_min && range_max >= bitset_max) {
    // Bitset is contained within the range, just return the range.
    return range;
  }

  if (bitset_min < range_min) {
    range_min = bitset_min;
  }
  if (bitset_max > range_max) {
    range_max = bitset_max;
  }
  return RangeType::New(range_min, range_max, BitsetType::kNone, zone);
}

Type* Type::Union(Type* type1, Type* type2, Zone* zone) {
  // Fast case: bit sets.
  if (type1->IsBitset() && type2->IsBitset()) {
    return BitsetType::New(type1->AsBitset() | type2->AsBitset());
  }

  // Fast case: top or bottom types.
  if (type1->IsAny() || type2->IsNone()) return type1;
  if (type2->IsAny() || type1->IsNone()) return type2;

  // Semi-fast case.
  if (type1->Is(type2)) return type2;
  if (type2->Is(type1)) return type1;

  // Figure out the representation of the result.
  // The rest of the method should not change this representation and
  // it should not make any decisions based on representations (i.e.,
  // it should only use the semantic part of types).
  const bitset representation =
      type1->Representation() | type2->Representation();

  // Slow case: create union.
  int size1 = type1->IsUnion() ? type1->AsUnion()->Length() : 1;
  int size2 = type2->IsUnion() ? type2->AsUnion()->Length() : 1;
  if (!AddIsSafe(size1, size2)) return Any();
  int size = size1 + size2;
  if (!AddIsSafe(size, 2)) return Any();
  size += 2;
  Type* result_type = UnionType::New(size, zone);
  UnionType* result = result_type->AsUnion();
  size = 0;

  // Compute the new bitset.
  bitset new_bitset = SEMANTIC(type1->BitsetGlb() | type2->BitsetGlb());

  // Deal with ranges.
  Type* range = None();
  Type* range1 = type1->GetRange();
  Type* range2 = type2->GetRange();
  if (range1 != NULL && range2 != NULL) {
    RangeType::Limits lims =
        RangeType::Limits::Union(RangeType::Limits(range1->AsRange()),
                                 RangeType::Limits(range2->AsRange()));
    Type* union_range = RangeType::New(lims, representation, zone);
    range = NormalizeRangeAndBitset(union_range, &new_bitset, zone);
  } else if (range1 != NULL) {
    range = NormalizeRangeAndBitset(range1, &new_bitset, zone);
  } else if (range2 != NULL) {
    range = NormalizeRangeAndBitset(range2, &new_bitset, zone);
  }
  new_bitset = SEMANTIC(new_bitset) | representation;
  Type* bits = BitsetType::New(new_bitset);
  result->Set(size++, bits);
  if (!range->IsNone()) result->Set(size++, range);

  size = AddToUnion(type1, result, size, zone);
  size = AddToUnion(type2, result, size, zone);
  return NormalizeUnion(result_type, size, zone);
}


// Add [type] to [result] unless [type] is bitset, range, or already subsumed.
// Return new size of [result].
int Type::AddToUnion(Type* type, UnionType* result, int size, Zone* zone) {
  if (type->IsBitset() || type->IsRange()) return size;
  if (type->IsUnion()) {
    for (int i = 0, n = type->AsUnion()->Length(); i < n; ++i) {
      size = AddToUnion(type->AsUnion()->Get(i), result, size, zone);
    }
    return size;
  }
  for (int i = 0; i < size; ++i) {
    if (type->SemanticIs(result->Get(i))) return size;
  }
  result->Set(size++, type);
  return size;
}

Type* Type::NormalizeUnion(Type* union_type, int size, Zone* zone) {
  UnionType* unioned = union_type->AsUnion();
  DCHECK(size >= 1);
  DCHECK(unioned->Get(0)->IsBitset());
  // If the union has just one element, return it.
  if (size == 1) {
    return unioned->Get(0);
  }
  bitset bits = unioned->Get(0)->AsBitset();
  // If the union only consists of a range, we can get rid of the union.
  if (size == 2 && SEMANTIC(bits) == BitsetType::kNone) {
    bitset representation = REPRESENTATION(bits);
    if (representation == unioned->Get(1)->Representation()) {
      return unioned->Get(1);
    }
    if (unioned->Get(1)->IsRange()) {
      return RangeType::New(unioned->Get(1)->AsRange()->Min(),
                            unioned->Get(1)->AsRange()->Max(),
                            unioned->Get(0)->AsBitset(), zone);
    }
  }
  unioned->Shrink(size);
  SLOW_DCHECK(unioned->Wellformed());
  return union_type;
}


// -----------------------------------------------------------------------------
// Component extraction

// static
Type* Type::Representation(Type* t, Zone* zone) {
  return BitsetType::New(t->Representation());
}


// static
Type* Type::Semantic(Type* t, Zone* zone) {
  return Intersect(t, BitsetType::New(BitsetType::kSemantic), zone);
}


// -----------------------------------------------------------------------------
// Iteration.

int Type::NumClasses() {
  DisallowHeapAllocation no_allocation;
  if (this->IsClass()) {
    return 1;
  } else if (this->IsUnion()) {
    int result = 0;
    for (int i = 0, n = this->AsUnion()->Length(); i < n; ++i) {
      if (this->AsUnion()->Get(i)->IsClass()) ++result;
    }
    return result;
  } else {
    return 0;
  }
}

int Type::NumConstants() {
  DisallowHeapAllocation no_allocation;
  if (this->IsConstant()) {
    return 1;
  } else if (this->IsUnion()) {
    int result = 0;
    for (int i = 0, n = this->AsUnion()->Length(); i < n; ++i) {
      if (this->AsUnion()->Get(i)->IsConstant()) ++result;
    }
    return result;
  } else {
    return 0;
  }
}

template <class T>
Type* Type::Iterator<T>::get_type() {
  DCHECK(!Done());
  return type_->IsUnion() ? type_->AsUnion()->Get(index_) : type_;
}


// C++ cannot specialise nested templates, so we have to go through this
// contortion with an auxiliary template to simulate it.
template <class T>
struct TypeImplIteratorAux {
  static bool matches(Type* type);
  static i::Handle<T> current(Type* type);
};

template <>
struct TypeImplIteratorAux<i::Map> {
  static bool matches(Type* type) { return type->IsClass(); }
  static i::Handle<i::Map> current(Type* type) {
    return type->AsClass()->Map();
  }
};

template <>
struct TypeImplIteratorAux<i::Object> {
  static bool matches(Type* type) { return type->IsConstant(); }
  static i::Handle<i::Object> current(Type* type) {
    return type->AsConstant()->Value();
  }
};

template <class T>
bool Type::Iterator<T>::matches(Type* type) {
  return TypeImplIteratorAux<T>::matches(type);
}

template <class T>
i::Handle<T> Type::Iterator<T>::Current() {
  return TypeImplIteratorAux<T>::current(get_type());
}

template <class T>
void Type::Iterator<T>::Advance() {
  DisallowHeapAllocation no_allocation;
  ++index_;
  if (type_->IsUnion()) {
    for (int n = type_->AsUnion()->Length(); index_ < n; ++index_) {
      if (matches(type_->AsUnion()->Get(index_))) return;
    }
  } else if (index_ == 0 && matches(type_)) {
    return;
  }
  index_ = -1;
}


// -----------------------------------------------------------------------------
// Printing.

const char* BitsetType::Name(bitset bits) {
  switch (bits) {
    case REPRESENTATION(kAny): return "Any";
    #define RETURN_NAMED_REPRESENTATION_TYPE(type, value) \
    case REPRESENTATION(k##type): return #type;
    REPRESENTATION_BITSET_TYPE_LIST(RETURN_NAMED_REPRESENTATION_TYPE)
    #undef RETURN_NAMED_REPRESENTATION_TYPE

    #define RETURN_NAMED_SEMANTIC_TYPE(type, value) \
    case SEMANTIC(k##type): return #type;
    SEMANTIC_BITSET_TYPE_LIST(RETURN_NAMED_SEMANTIC_TYPE)
    INTERNAL_BITSET_TYPE_LIST(RETURN_NAMED_SEMANTIC_TYPE)
    #undef RETURN_NAMED_SEMANTIC_TYPE

    default:
      return NULL;
  }
}

void BitsetType::Print(std::ostream& os,  // NOLINT
                       bitset bits) {
  DisallowHeapAllocation no_allocation;
  const char* name = Name(bits);
  if (name != NULL) {
    os << name;
    return;
  }

  // clang-format off
  static const bitset named_bitsets[] = {
#define BITSET_CONSTANT(type, value) REPRESENTATION(k##type),
    REPRESENTATION_BITSET_TYPE_LIST(BITSET_CONSTANT)
#undef BITSET_CONSTANT

#define BITSET_CONSTANT(type, value) SEMANTIC(k##type),
    INTERNAL_BITSET_TYPE_LIST(BITSET_CONSTANT)
    SEMANTIC_BITSET_TYPE_LIST(BITSET_CONSTANT)
#undef BITSET_CONSTANT
  };
  // clang-format on

  bool is_first = true;
  os << "(";
  for (int i(arraysize(named_bitsets) - 1); bits != 0 && i >= 0; --i) {
    bitset subset = named_bitsets[i];
    if ((bits & subset) == subset) {
      if (!is_first) os << " | ";
      is_first = false;
      os << Name(subset);
      bits -= subset;
    }
  }
  DCHECK(bits == 0);
  os << ")";
}

void Type::PrintTo(std::ostream& os, PrintDimension dim) {
  DisallowHeapAllocation no_allocation;
  if (dim != REPRESENTATION_DIM) {
    if (this->IsBitset()) {
      BitsetType::Print(os, SEMANTIC(this->AsBitset()));
    } else if (this->IsClass()) {
      os << "Class(" << static_cast<void*>(*this->AsClass()->Map()) << " < ";
      BitsetType::New(BitsetType::Lub(this))->PrintTo(os, dim);
      os << ")";
    } else if (this->IsConstant()) {
      os << "Constant(" << Brief(*this->AsConstant()->Value()) << ")";
    } else if (this->IsRange()) {
      std::ostream::fmtflags saved_flags = os.setf(std::ios::fixed);
      std::streamsize saved_precision = os.precision(0);
      os << "Range(" << this->AsRange()->Min() << ", " << this->AsRange()->Max()
         << ")";
      os.flags(saved_flags);
      os.precision(saved_precision);
    } else if (this->IsContext()) {
      os << "Context(";
      this->AsContext()->Outer()->PrintTo(os, dim);
      os << ")";
    } else if (this->IsUnion()) {
      os << "(";
      for (int i = 0, n = this->AsUnion()->Length(); i < n; ++i) {
        Type* type_i = this->AsUnion()->Get(i);
        if (i > 0) os << " | ";
        type_i->PrintTo(os, dim);
      }
      os << ")";
    } else if (this->IsArray()) {
      os << "Array(";
      AsArray()->Element()->PrintTo(os, dim);
      os << ")";
    } else if (this->IsFunction()) {
      if (!this->AsFunction()->Receiver()->IsAny()) {
        this->AsFunction()->Receiver()->PrintTo(os, dim);
        os << ".";
      }
      os << "(";
      for (int i = 0; i < this->AsFunction()->Arity(); ++i) {
        if (i > 0) os << ", ";
        this->AsFunction()->Parameter(i)->PrintTo(os, dim);
      }
      os << ")->";
      this->AsFunction()->Result()->PrintTo(os, dim);
    } else if (this->IsTuple()) {
      os << "<";
      for (int i = 0, n = this->AsTuple()->Arity(); i < n; ++i) {
        Type* type_i = this->AsTuple()->Element(i);
        if (i > 0) os << ", ";
        type_i->PrintTo(os, dim);
      }
      os << ">";
    } else {
      UNREACHABLE();
    }
  }
  if (dim == BOTH_DIMS) os << "/";
  if (dim != SEMANTIC_DIM) {
    BitsetType::Print(os, REPRESENTATION(this->BitsetLub()));
  }
}


#ifdef DEBUG
void Type::Print() {
  OFStream os(stdout);
  PrintTo(os);
  os << std::endl;
}
void BitsetType::Print(bitset bits) {
  OFStream os(stdout);
  Print(os, bits);
  os << std::endl;
}
#endif

BitsetType::bitset BitsetType::SignedSmall() {
  return i::SmiValuesAre31Bits() ? kSigned31 : kSigned32;
}

BitsetType::bitset BitsetType::UnsignedSmall() {
  return i::SmiValuesAre31Bits() ? kUnsigned30 : kUnsigned31;
}

#define CONSTRUCT_SIMD_TYPE(NAME, Name, name, lane_count, lane_type) \
  Type* Type::Name(Isolate* isolate, Zone* zone) {                   \
    return Class(i::handle(isolate->heap()->name##_map()), zone);    \
  }
SIMD128_TYPES(CONSTRUCT_SIMD_TYPE)
#undef CONSTRUCT_SIMD_TYPE

// -----------------------------------------------------------------------------
// Instantiations.

template class Type::Iterator<i::Map>;
template class Type::Iterator<i::Object>;

}  // namespace internal
}  // namespace v8
