// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/v8.h"

#include "src/bootstrapper.h"
#include "src/deoptimizer.h"
#include "src/lookup.h"
#include "src/lookup-inl.h"

namespace v8 {
namespace internal {


void LookupIterator::Next() {
  DCHECK_NE(JSPROXY, state_);
  DCHECK_NE(TRANSITION, state_);
  DisallowHeapAllocation no_gc;
  has_property_ = false;

  JSReceiver* holder = *holder_;
  Map* map = *holder_map_;

  // Perform lookup on current holder.
  state_ = LookupInHolder(map, holder);
  if (IsFound()) return;

  // Continue lookup if lookup on current holder failed.
  do {
    JSReceiver* maybe_holder = NextHolder(map);
    if (maybe_holder == nullptr) {
      if (interceptor_state_ == InterceptorState::kSkipNonMasking) {
        RestartLookupForNonMaskingInterceptors();
        return;
      }
      break;
    }
    holder = maybe_holder;
    map = holder->map();
    state_ = LookupInHolder(map, holder);
  } while (!IsFound());

  if (holder != *holder_) {
    holder_ = handle(holder, isolate_);
    holder_map_ = handle(map, isolate_);
  }
}


void LookupIterator::RestartLookupForNonMaskingInterceptors() {
  interceptor_state_ = InterceptorState::kProcessNonMasking;
  state_ = NOT_FOUND;
  property_details_ = PropertyDetails::Empty();
  number_ = DescriptorArray::kNotFound;
  holder_ = initial_holder_;
  holder_map_ = handle(holder_->map(), isolate_);
  Next();
}


Handle<JSReceiver> LookupIterator::GetRoot(Handle<Object> receiver,
                                           Isolate* isolate) {
  if (receiver->IsJSReceiver()) return Handle<JSReceiver>::cast(receiver);
  auto root = handle(receiver->GetRootMap(isolate)->prototype(), isolate);
  if (root->IsNull()) {
    unsigned int magic = 0xbbbbbbbb;
    isolate->PushStackTraceAndDie(magic, *receiver, NULL, magic);
  }
  return Handle<JSReceiver>::cast(root);
}


Handle<Map> LookupIterator::GetReceiverMap() const {
  if (receiver_->IsNumber()) return isolate_->factory()->heap_number_map();
  return handle(Handle<HeapObject>::cast(receiver_)->map(), isolate_);
}


Handle<JSObject> LookupIterator::GetStoreTarget() const {
  if (receiver_->IsJSGlobalProxy()) {
    PrototypeIterator iter(isolate(), receiver_);
    if (iter.IsAtEnd()) return Handle<JSGlobalProxy>::cast(receiver_);
    return Handle<JSGlobalObject>::cast(PrototypeIterator::GetCurrent(iter));
  }
  return Handle<JSObject>::cast(receiver_);
}


bool LookupIterator::HasAccess() const {
  DCHECK_EQ(ACCESS_CHECK, state_);
  return isolate_->MayAccess(GetHolder<JSObject>());
}


void LookupIterator::ReloadPropertyInformation() {
  state_ = BEFORE_PROPERTY;
  interceptor_state_ = InterceptorState::kUninitialized;
  state_ = LookupInHolder(*holder_map_, *holder_);
  DCHECK(IsFound() || holder_map_->is_dictionary_map());
}


void LookupIterator::ReloadHolderMap() {
  DCHECK_EQ(DATA, state_);
  DCHECK(IsElement());
  DCHECK(JSObject::cast(*holder_)->HasExternalArrayElements() ||
         JSObject::cast(*holder_)->HasFixedTypedArrayElements());
  if (*holder_map_ != holder_->map()) {
    holder_map_ = handle(holder_->map(), isolate_);
  }
}


void LookupIterator::PrepareForDataProperty(Handle<Object> value) {
  DCHECK(state_ == DATA || state_ == ACCESSOR);
  DCHECK(HolderIsReceiverOrHiddenPrototype());

  Handle<JSObject> holder = GetHolder<JSObject>();

  if (IsElement()) {
    ElementsKind old_kind = holder_map_->elements_kind();
    holder_map_ = Map::PrepareForDataElement(holder_map_, value);
    ElementsKind new_kind = holder_map_->elements_kind();
    if (new_kind != old_kind) {
      // TODO(verwaest): Handle element migration in MigrateToMap.
      JSObject::UpdateAllocationSite(holder, new_kind);
      if (IsFastDoubleElementsKind(old_kind) !=
          IsFastDoubleElementsKind(new_kind)) {
        uint32_t capacity = holder->elements()->length();
        ElementsAccessor* accessor = ElementsAccessor::ForKind(new_kind);
        accessor->GrowCapacityAndConvert(holder, capacity);
        // GrowCapacityAndConvert migrated the object. No reloading of property
        // infomation is necessary for elements.
        return;
      } else if (FLAG_trace_elements_transitions) {
        Handle<FixedArrayBase> elements(holder->elements());
        JSObject::PrintElementsTransition(stdout, holder, old_kind, elements,
                                          new_kind, elements);
      }
    }

    // Copy the backing store if it is copy-on-write.
    if (IsFastSmiOrObjectElementsKind(new_kind)) {
      JSObject::EnsureWritableFastElements(holder);
    }

  } else {
    if (holder_map_->is_dictionary_map()) return;
    holder_map_ =
        Map::PrepareForDataProperty(holder_map_, descriptor_number(), value);
  }

  JSObject::MigrateToMap(holder, holder_map_);
  ReloadPropertyInformation();
}


void LookupIterator::ReconfigureDataProperty(Handle<Object> value,
                                             PropertyAttributes attributes) {
  DCHECK(state_ == DATA || state_ == ACCESSOR);
  DCHECK(HolderIsReceiverOrHiddenPrototype());
  Handle<JSObject> holder = GetHolder<JSObject>();
  if (IsElement()) {
    DCHECK(!holder->HasExternalArrayElements());
    DCHECK(!holder->HasFixedTypedArrayElements());
    DCHECK(attributes != NONE || !holder->HasFastElements());
    Handle<FixedArrayBase> elements(holder->elements());
    holder->GetElementsAccessor()->Reconfigure(holder, elements, number_, value,
                                               attributes);
  } else if (holder_map_->is_dictionary_map()) {
    PropertyDetails details(attributes, v8::internal::DATA, 0,
                            PropertyCellType::kMutable);
    JSObject::SetNormalizedProperty(holder, name(), value, details);
  } else {
    holder_map_ = Map::ReconfigureExistingProperty(
        holder_map_, descriptor_number(), i::kData, attributes);
    holder_map_ =
        Map::PrepareForDataProperty(holder_map_, descriptor_number(), value);
    JSObject::MigrateToMap(holder, holder_map_);
  }

  ReloadPropertyInformation();
}


void LookupIterator::PrepareTransitionToDataProperty(
    Handle<Object> value, PropertyAttributes attributes,
    Object::StoreFromKeyed store_mode) {
  if (state_ == TRANSITION) return;
  DCHECK(state_ != LookupIterator::ACCESSOR ||
         (GetAccessors()->IsAccessorInfo() &&
          AccessorInfo::cast(*GetAccessors())->is_special_data_property()));
  DCHECK_NE(INTEGER_INDEXED_EXOTIC, state_);
  DCHECK(state_ == NOT_FOUND || !HolderIsReceiverOrHiddenPrototype());
  // Can only be called when the receiver is a JSObject. JSProxy has to be
  // handled via a trap. Adding properties to primitive values is not
  // observable.
  Handle<JSObject> receiver = GetStoreTarget();

  if (!isolate()->IsInternallyUsedPropertyName(name()) &&
      !receiver->map()->is_extensible()) {
    return;
  }

  auto transition = Map::TransitionToDataProperty(
      handle(receiver->map(), isolate_), name_, value, attributes, store_mode);
  state_ = TRANSITION;
  transition_ = transition;

  if (receiver->IsGlobalObject()) {
    // Install a property cell.
    InternalizeName();
    auto cell = GlobalObject::EnsurePropertyCell(
        Handle<GlobalObject>::cast(receiver), name());
    DCHECK(cell->value()->IsTheHole());
    transition_ = cell;
  } else if (!transition->is_dictionary_map()) {
    property_details_ = transition->GetLastDescriptorDetails();
    has_property_ = true;
  }
}


void LookupIterator::ApplyTransitionToDataProperty() {
  DCHECK_EQ(TRANSITION, state_);

  Handle<JSObject> receiver = GetStoreTarget();
  if (receiver->IsGlobalObject()) return;
  holder_ = receiver;
  holder_map_ = transition_map();
  JSObject::MigrateToMap(receiver, holder_map_);
  ReloadPropertyInformation();
}


void LookupIterator::Delete() {
  Handle<JSObject> holder = Handle<JSObject>::cast(holder_);
  if (IsElement()) {
    ElementsAccessor* accessor = holder->GetElementsAccessor();
    accessor->Delete(holder, number_);
  } else {
    PropertyNormalizationMode mode = holder->map()->is_prototype_map()
                                         ? KEEP_INOBJECT_PROPERTIES
                                         : CLEAR_INOBJECT_PROPERTIES;

    if (holder->HasFastProperties()) {
      JSObject::NormalizeProperties(holder, mode, 0, "DeletingProperty");
      holder_map_ = handle(holder->map(), isolate_);
      ReloadPropertyInformation();
    }
    // TODO(verwaest): Get rid of the name_ argument.
    JSObject::DeleteNormalizedProperty(holder, name_, number_);
    JSObject::ReoptimizeIfPrototype(holder);
  }
}


void LookupIterator::TransitionToAccessorProperty(
    AccessorComponent component, Handle<Object> accessor,
    PropertyAttributes attributes) {
  DCHECK(!accessor->IsNull());
  // Can only be called when the receiver is a JSObject. JSProxy has to be
  // handled via a trap. Adding properties to primitive values is not
  // observable.
  Handle<JSObject> receiver = GetStoreTarget();
  holder_ = receiver;
  holder_map_ =
      Map::TransitionToAccessorProperty(handle(receiver->map(), isolate_),
                                        name_, component, accessor, attributes);
  JSObject::MigrateToMap(receiver, holder_map_);

  ReloadPropertyInformation();

  if (!holder_map_->is_dictionary_map()) return;


  // Install the accessor into the dictionary-mode object.
  PropertyDetails details(attributes, ACCESSOR_CONSTANT, 0,
                          PropertyCellType::kMutable);
  Handle<AccessorPair> pair;
  if (state() == ACCESSOR && GetAccessors()->IsAccessorPair()) {
    pair = Handle<AccessorPair>::cast(GetAccessors());
    // If the component and attributes are identical, nothing has to be done.
    if (pair->get(component) == *accessor) {
      if (property_details().attributes() == attributes) return;
    } else {
      pair = AccessorPair::Copy(pair);
      pair->set(component, *accessor);
    }
  } else {
    pair = isolate()->factory()->NewAccessorPair();
    pair->set(component, *accessor);
  }
  JSObject::SetNormalizedProperty(receiver, name_, pair, details);

  JSObject::ReoptimizeIfPrototype(receiver);
  holder_map_ = handle(receiver->map(), isolate_);
  ReloadPropertyInformation();
}


bool LookupIterator::HolderIsReceiverOrHiddenPrototype() const {
  DCHECK(has_property_ || state_ == INTERCEPTOR || state_ == JSPROXY);
  return InternalHolderIsReceiverOrHiddenPrototype();
}

bool LookupIterator::InternalHolderIsReceiverOrHiddenPrototype() const {
  // Optimization that only works if configuration_ is not mutable.
  if (!check_prototype_chain()) return true;
  DisallowHeapAllocation no_gc;
  if (!receiver_->IsJSReceiver()) return false;
  Object* current = *receiver_;
  JSReceiver* holder = *holder_;
  // JSProxy do not occur as hidden prototypes.
  if (current->IsJSProxy()) {
    return JSReceiver::cast(current) == holder;
  }
  PrototypeIterator iter(isolate(), current,
                         PrototypeIterator::START_AT_RECEIVER);
  do {
    if (JSReceiver::cast(iter.GetCurrent()) == holder) return true;
    DCHECK(!current->IsJSProxy());
    iter.Advance();
  } while (!iter.IsAtEnd(PrototypeIterator::END_AT_NON_HIDDEN));
  return false;
}


Handle<Object> LookupIterator::FetchValue() const {
  Object* result = NULL;
  Handle<JSObject> holder = GetHolder<JSObject>();
  if (IsElement()) {
    // TODO(verwaest): Optimize.
    if (holder->IsStringObjectWithCharacterAt(index_)) {
      Handle<JSValue> js_value = Handle<JSValue>::cast(holder);
      Handle<String> string(String::cast(js_value->value()));
      return factory()->LookupSingleCharacterStringFromCode(
          String::Flatten(string)->Get(index_));
    }

    ElementsAccessor* accessor = holder->GetElementsAccessor();
    return accessor->Get(holder, index_);
  } else if (holder_map_->IsGlobalObjectMap()) {
    result = holder->global_dictionary()->ValueAt(number_);
    DCHECK(result->IsPropertyCell());
    result = PropertyCell::cast(result)->value();
  } else if (holder_map_->is_dictionary_map()) {
    result = holder->property_dictionary()->ValueAt(number_);
  } else if (property_details_.type() == v8::internal::DATA) {
    FieldIndex field_index = FieldIndex::ForDescriptor(*holder_map_, number_);
    return JSObject::FastPropertyAt(holder, property_details_.representation(),
                                    field_index);
  } else {
    result = holder_map_->instance_descriptors()->GetValue(number_);
  }
  return handle(result, isolate_);
}


int LookupIterator::GetAccessorIndex() const {
  DCHECK(has_property_);
  DCHECK(!holder_map_->is_dictionary_map());
  DCHECK_EQ(v8::internal::ACCESSOR_CONSTANT, property_details_.type());
  return descriptor_number();
}


int LookupIterator::GetConstantIndex() const {
  DCHECK(has_property_);
  DCHECK(!holder_map_->is_dictionary_map());
  DCHECK_EQ(v8::internal::DATA_CONSTANT, property_details_.type());
  DCHECK(!IsElement());
  return descriptor_number();
}


FieldIndex LookupIterator::GetFieldIndex() const {
  DCHECK(has_property_);
  DCHECK(!holder_map_->is_dictionary_map());
  DCHECK_EQ(v8::internal::DATA, property_details_.type());
  DCHECK(!IsElement());
  int index =
      holder_map_->instance_descriptors()->GetFieldIndex(descriptor_number());
  bool is_double = representation().IsDouble();
  return FieldIndex::ForPropertyIndex(*holder_map_, index, is_double);
}


Handle<HeapType> LookupIterator::GetFieldType() const {
  DCHECK(has_property_);
  DCHECK(!holder_map_->is_dictionary_map());
  DCHECK_EQ(v8::internal::DATA, property_details_.type());
  return handle(
      holder_map_->instance_descriptors()->GetFieldType(descriptor_number()),
      isolate_);
}


Handle<PropertyCell> LookupIterator::GetPropertyCell() const {
  DCHECK(!IsElement());
  Handle<JSObject> holder = GetHolder<JSObject>();
  Handle<GlobalObject> global = Handle<GlobalObject>::cast(holder);
  Object* value = global->global_dictionary()->ValueAt(dictionary_entry());
  DCHECK(value->IsPropertyCell());
  return handle(PropertyCell::cast(value));
}


Handle<Object> LookupIterator::GetAccessors() const {
  DCHECK_EQ(ACCESSOR, state_);
  return FetchValue();
}


Handle<Object> LookupIterator::GetDataValue() const {
  DCHECK_EQ(DATA, state_);
  Handle<Object> value = FetchValue();
  return value;
}


void LookupIterator::WriteDataValue(Handle<Object> value) {
  DCHECK_EQ(DATA, state_);
  Handle<JSObject> holder = GetHolder<JSObject>();
  if (IsElement()) {
    ElementsAccessor* accessor = holder->GetElementsAccessor();
    accessor->Set(holder->elements(), index_, *value);
  } else if (holder->IsGlobalObject()) {
    Handle<GlobalDictionary> property_dictionary =
        handle(holder->global_dictionary());
    PropertyCell::UpdateCell(property_dictionary, dictionary_entry(), value,
                             property_details_);
  } else if (holder_map_->is_dictionary_map()) {
    NameDictionary* property_dictionary = holder->property_dictionary();
    property_dictionary->ValueAtPut(dictionary_entry(), *value);
  } else if (property_details_.type() == v8::internal::DATA) {
    holder->WriteToField(descriptor_number(), *value);
  } else {
    DCHECK_EQ(v8::internal::DATA_CONSTANT, property_details_.type());
  }
}


bool LookupIterator::IsIntegerIndexedExotic(JSReceiver* holder) {
  DCHECK(exotic_index_state_ != ExoticIndexState::kNotExotic);
  // Currently typed arrays are the only such objects.
  if (!holder->IsJSTypedArray()) return false;
  if (exotic_index_state_ == ExoticIndexState::kExotic) return true;
  if (!InternalHolderIsReceiverOrHiddenPrototype()) {
    exotic_index_state_ = ExoticIndexState::kNotExotic;
    return false;
  }
  DCHECK(exotic_index_state_ == ExoticIndexState::kUninitialized);
  bool result = false;
  // Compute and cache result.
  if (IsElement()) {
    result = index_ >= JSTypedArray::cast(holder)->length_value();
  } else if (name()->IsString()) {
    Handle<String> name_string = Handle<String>::cast(name());
    if (name_string->length() != 0) {
      result = IsSpecialIndex(isolate_->unicode_cache(), *name_string);
    }
  }
  exotic_index_state_ =
      result ? ExoticIndexState::kExotic : ExoticIndexState::kNotExotic;
  return result;
}


void LookupIterator::InternalizeName() {
  if (name_->IsUniqueName()) return;
  name_ = factory()->InternalizeString(Handle<String>::cast(name_));
}


bool LookupIterator::HasInterceptor(Map* map) const {
  if (IsElement()) return map->has_indexed_interceptor();
  return map->has_named_interceptor();
}


Handle<InterceptorInfo> LookupIterator::GetInterceptor() const {
  DCHECK_EQ(INTERCEPTOR, state_);
  return handle(GetInterceptor(JSObject::cast(*holder_)), isolate_);
}


InterceptorInfo* LookupIterator::GetInterceptor(JSObject* holder) const {
  if (IsElement()) return holder->GetIndexedInterceptor();
  return holder->GetNamedInterceptor();
}


bool LookupIterator::SkipInterceptor(JSObject* holder) {
  auto info = GetInterceptor(holder);
  // TODO(dcarney): check for symbol/can_intercept_symbols here as well.
  if (info->non_masking()) {
    switch (interceptor_state_) {
      case InterceptorState::kUninitialized:
        interceptor_state_ = InterceptorState::kSkipNonMasking;
      // Fall through.
      case InterceptorState::kSkipNonMasking:
        return true;
      case InterceptorState::kProcessNonMasking:
        return false;
    }
  }
  return interceptor_state_ == InterceptorState::kProcessNonMasking;
}
}  // namespace internal
}  // namespace v8
