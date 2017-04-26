// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ostream>

#include "src/accessors.h"
#include "src/compilation-dependencies.h"
#include "src/compiler/access-info.h"
#include "src/field-index-inl.h"
#include "src/objects-inl.h"  // TODO(mstarzinger): Temporary cycle breaker!
#include "src/type-cache.h"
#include "src/types-inl.h"

namespace v8 {
namespace internal {
namespace compiler {

namespace {

bool CanInlineElementAccess(Handle<Map> map) {
  // TODO(bmeurer): IsJSObjectMap
  // TODO(bmeurer): !map->has_dictionary_elements()
  // TODO(bmeurer): !map->has_sloppy_arguments_elements()
  return map->IsJSArrayMap() && map->has_fast_elements() &&
         !map->has_indexed_interceptor() && !map->is_access_check_needed();
}


bool CanInlinePropertyAccess(Handle<Map> map) {
  // We can inline property access to prototypes of all primitives, except
  // the special Oddball ones that have no wrapper counterparts (i.e. Null,
  // Undefined and TheHole).
  STATIC_ASSERT(ODDBALL_TYPE == LAST_PRIMITIVE_TYPE);
  if (map->IsBooleanMap()) return true;
  if (map->instance_type() < LAST_PRIMITIVE_TYPE) return true;
  return map->IsJSObjectMap() && !map->is_dictionary_map() &&
         !map->has_named_interceptor() &&
         // TODO(verwaest): Whitelist contexts to which we have access.
         !map->is_access_check_needed();
}

}  // namespace


std::ostream& operator<<(std::ostream& os, AccessMode access_mode) {
  switch (access_mode) {
    case AccessMode::kLoad:
      return os << "Load";
    case AccessMode::kStore:
      return os << "Store";
  }
  UNREACHABLE();
  return os;
}


// static
PropertyAccessInfo PropertyAccessInfo::NotFound(Type* receiver_type,
                                                MaybeHandle<JSObject> holder) {
  return PropertyAccessInfo(holder, receiver_type);
}


// static
PropertyAccessInfo PropertyAccessInfo::DataConstant(
    Type* receiver_type, Handle<Object> constant,
    MaybeHandle<JSObject> holder) {
  return PropertyAccessInfo(holder, constant, receiver_type);
}


// static
PropertyAccessInfo PropertyAccessInfo::DataField(
    Type* receiver_type, FieldIndex field_index, Type* field_type,
    MaybeHandle<JSObject> holder, MaybeHandle<Map> transition_map) {
  return PropertyAccessInfo(holder, transition_map, field_index, field_type,
                            receiver_type);
}


ElementAccessInfo::ElementAccessInfo() : receiver_type_(Type::None()) {}


PropertyAccessInfo::PropertyAccessInfo()
    : kind_(kInvalid), receiver_type_(Type::None()), field_type_(Type::Any()) {}


PropertyAccessInfo::PropertyAccessInfo(MaybeHandle<JSObject> holder,
                                       Type* receiver_type)
    : kind_(kNotFound),
      receiver_type_(receiver_type),
      holder_(holder),
      field_type_(Type::Any()) {}


PropertyAccessInfo::PropertyAccessInfo(MaybeHandle<JSObject> holder,
                                       Handle<Object> constant,
                                       Type* receiver_type)
    : kind_(kDataConstant),
      receiver_type_(receiver_type),
      constant_(constant),
      holder_(holder),
      field_type_(Type::Any()) {}


PropertyAccessInfo::PropertyAccessInfo(MaybeHandle<JSObject> holder,
                                       MaybeHandle<Map> transition_map,
                                       FieldIndex field_index, Type* field_type,
                                       Type* receiver_type)
    : kind_(kDataField),
      receiver_type_(receiver_type),
      transition_map_(transition_map),
      holder_(holder),
      field_index_(field_index),
      field_type_(field_type) {}


AccessInfoFactory::AccessInfoFactory(CompilationDependencies* dependencies,
                                     Handle<Context> native_context, Zone* zone)
    : dependencies_(dependencies),
      native_context_(native_context),
      isolate_(native_context->GetIsolate()),
      type_cache_(TypeCache::Get()),
      zone_(zone) {}


bool AccessInfoFactory::ComputeElementAccessInfo(
    Handle<Map> map, AccessMode access_mode, ElementAccessInfo* access_info) {
  // Check if it is safe to inline element access for the {map}.
  if (!CanInlineElementAccess(map)) return false;

  // TODO(bmeurer): Add support for holey elements.
  ElementsKind elements_kind = map->elements_kind();
  if (IsHoleyElementsKind(elements_kind)) return false;

  // Certain (monomorphic) stores need a prototype chain check because shape
  // changes could allow callbacks on elements in the chain that are not
  // compatible with monomorphic keyed stores.
  MaybeHandle<JSObject> holder;
  if (access_mode == AccessMode::kStore && map->prototype()->IsJSObject()) {
    for (PrototypeIterator i(map); !i.IsAtEnd(); i.Advance()) {
      Handle<JSReceiver> prototype =
          PrototypeIterator::GetCurrent<JSReceiver>(i);
      if (!prototype->IsJSObject()) return false;
      holder = Handle<JSObject>::cast(prototype);
    }
  }

  *access_info =
      ElementAccessInfo(Type::Class(map, zone()), elements_kind, holder);
  return true;
}


bool AccessInfoFactory::ComputeElementAccessInfos(
    MapHandleList const& maps, AccessMode access_mode,
    ZoneVector<ElementAccessInfo>* access_infos) {
  for (Handle<Map> map : maps) {
    if (Map::TryUpdate(map).ToHandle(&map)) {
      ElementAccessInfo access_info;
      if (!ComputeElementAccessInfo(map, access_mode, &access_info)) {
        return false;
      }
      access_infos->push_back(access_info);
    }
  }
  return true;
}


bool AccessInfoFactory::ComputePropertyAccessInfo(
    Handle<Map> map, Handle<Name> name, AccessMode access_mode,
    PropertyAccessInfo* access_info) {
  // Check if it is safe to inline property access for the {map}.
  if (!CanInlinePropertyAccess(map)) return false;

  // Compute the receiver type.
  Handle<Map> receiver_map = map;

  // We support fast inline cases for certain JSObject getters.
  if (access_mode == AccessMode::kLoad &&
      LookupSpecialFieldAccessor(map, name, access_info)) {
    return true;
  }

  MaybeHandle<JSObject> holder;
  do {
    // Lookup the named property on the {map}.
    Handle<DescriptorArray> descriptors(map->instance_descriptors(), isolate());
    int const number = descriptors->SearchWithCache(*name, *map);
    if (number != DescriptorArray::kNotFound) {
      PropertyDetails const details = descriptors->GetDetails(number);
      if (access_mode == AccessMode::kStore) {
        // Don't bother optimizing stores to read-only properties.
        if (details.IsReadOnly()) {
          return false;
        }
        // Check for store to data property on a prototype.
        if (details.kind() == kData && !holder.is_null()) {
          // Store to property not found on the receiver but on a prototype, we
          // need to transition to a new data property.
          // Implemented according to ES6 section 9.1.9 [[Set]] (P, V, Receiver)
          return LookupTransition(receiver_map, name, holder, access_info);
        }
      }
      if (details.type() == DATA_CONSTANT) {
        *access_info = PropertyAccessInfo::DataConstant(
            Type::Class(receiver_map, zone()),
            handle(descriptors->GetValue(number), isolate()), holder);
        return true;
      } else if (details.type() == DATA) {
        int index = descriptors->GetFieldIndex(number);
        Representation field_representation = details.representation();
        FieldIndex field_index = FieldIndex::ForPropertyIndex(
            *map, index, field_representation.IsDouble());
        Type* field_type = Type::Tagged();
        if (field_representation.IsSmi()) {
          field_type = type_cache_.kSmi;
        } else if (field_representation.IsDouble()) {
          field_type = type_cache_.kFloat64;
        } else if (field_representation.IsHeapObject()) {
          // Extract the field type from the property details (make sure its
          // representation is TaggedPointer to reflect the heap object case).
          field_type = Type::Intersect(
              Type::Convert<HeapType>(
                  handle(descriptors->GetFieldType(number), isolate()), zone()),
              Type::TaggedPointer(), zone());
          if (field_type->Is(Type::None())) {
            // Store is not safe if the field type was cleared.
            if (access_mode == AccessMode::kStore) return false;

            // The field type was cleared by the GC, so we don't know anything
            // about the contents now.
            // TODO(bmeurer): It would be awesome to make this saner in the
            // runtime/GC interaction.
            field_type = Type::TaggedPointer();
          } else if (!Type::Any()->Is(field_type)) {
            // Add proper code dependencies in case of stable field map(s).
            Handle<Map> field_owner_map(map->FindFieldOwner(number), isolate());
            dependencies()->AssumeFieldType(field_owner_map);
          }
          DCHECK(field_type->Is(Type::TaggedPointer()));
        }
        *access_info = PropertyAccessInfo::DataField(
            Type::Class(receiver_map, zone()), field_index, field_type, holder);
        return true;
      } else {
        // TODO(bmeurer): Add support for accessors.
        return false;
      }
    }

    // Don't search on the prototype chain for special indices in case of
    // integer indexed exotic objects (see ES6 section 9.4.5).
    if (map->IsJSTypedArrayMap() && name->IsString() &&
        IsSpecialIndex(isolate()->unicode_cache(), String::cast(*name))) {
      return false;
    }

    // Don't lookup private symbols on the prototype chain.
    if (name->IsPrivate()) return false;

    // Walk up the prototype chain.
    if (!map->prototype()->IsJSObject()) {
      // Perform the implicit ToObject for primitives here.
      // Implemented according to ES6 section 7.3.2 GetV (V, P).
      Handle<JSFunction> constructor;
      if (Map::GetConstructorFunction(map, native_context())
              .ToHandle(&constructor)) {
        map = handle(constructor->initial_map(), isolate());
        DCHECK(map->prototype()->IsJSObject());
      } else if (map->prototype()->IsNull()) {
        // Store to property not found on the receiver or any prototype, we need
        // to transition to a new data property.
        // Implemented according to ES6 section 9.1.9 [[Set]] (P, V, Receiver)
        if (access_mode == AccessMode::kStore) {
          return LookupTransition(receiver_map, name, holder, access_info);
        }
        // The property was not found, return undefined or throw depending
        // on the language mode of the load operation.
        // Implemented according to ES6 section 9.1.8 [[Get]] (P, Receiver)
        *access_info = PropertyAccessInfo::NotFound(
            Type::Class(receiver_map, zone()), holder);
        return true;
      } else {
        return false;
      }
    }
    Handle<JSObject> map_prototype(JSObject::cast(map->prototype()), isolate());
    if (map_prototype->map()->is_deprecated()) {
      // Try to migrate the prototype object so we don't embed the deprecated
      // map into the optimized code.
      JSObject::TryMigrateInstance(map_prototype);
    }
    map = handle(map_prototype->map(), isolate());
    holder = map_prototype;
  } while (CanInlinePropertyAccess(map));
  return false;
}


bool AccessInfoFactory::ComputePropertyAccessInfos(
    MapHandleList const& maps, Handle<Name> name, AccessMode access_mode,
    ZoneVector<PropertyAccessInfo>* access_infos) {
  for (Handle<Map> map : maps) {
    if (Map::TryUpdate(map).ToHandle(&map)) {
      PropertyAccessInfo access_info;
      if (!ComputePropertyAccessInfo(map, name, access_mode, &access_info)) {
        return false;
      }
      access_infos->push_back(access_info);
    }
  }
  return true;
}


bool AccessInfoFactory::LookupSpecialFieldAccessor(
    Handle<Map> map, Handle<Name> name, PropertyAccessInfo* access_info) {
  // Check for special JSObject field accessors.
  int offset;
  if (Accessors::IsJSObjectFieldAccessor(map, name, &offset)) {
    FieldIndex field_index = FieldIndex::ForInObjectOffset(offset);
    Type* field_type = Type::Tagged();
    if (map->IsStringMap()) {
      DCHECK(Name::Equals(factory()->length_string(), name));
      // The String::length property is always a smi in the range
      // [0, String::kMaxLength].
      field_type = type_cache_.kStringLengthType;
    } else if (map->IsJSArrayMap()) {
      DCHECK(Name::Equals(factory()->length_string(), name));
      // The JSArray::length property is a smi in the range
      // [0, FixedDoubleArray::kMaxLength] in case of fast double
      // elements, a smi in the range [0, FixedArray::kMaxLength]
      // in case of other fast elements, and [0, kMaxUInt32] in
      // case of other arrays.
      if (IsFastDoubleElementsKind(map->elements_kind())) {
        field_type = type_cache_.kFixedDoubleArrayLengthType;
      } else if (IsFastElementsKind(map->elements_kind())) {
        field_type = type_cache_.kFixedArrayLengthType;
      } else {
        field_type = type_cache_.kJSArrayLengthType;
      }
    }
    *access_info = PropertyAccessInfo::DataField(Type::Class(map, zone()),
                                                 field_index, field_type);
    return true;
  }
  return false;
}


bool AccessInfoFactory::LookupTransition(Handle<Map> map, Handle<Name> name,
                                         MaybeHandle<JSObject> holder,
                                         PropertyAccessInfo* access_info) {
  // Check if the {map} has a data transition with the given {name}.
  if (map->unused_property_fields() == 0) return false;
  Handle<Map> transition_map;
  if (TransitionArray::SearchTransition(map, kData, name, NONE)
          .ToHandle(&transition_map)) {
    int const number = transition_map->LastAdded();
    PropertyDetails const details =
        transition_map->instance_descriptors()->GetDetails(number);
    // Don't bother optimizing stores to read-only properties.
    if (details.IsReadOnly()) return false;
    // TODO(bmeurer): Handle transition to data constant?
    if (details.type() != DATA) return false;
    int const index = details.field_index();
    Representation field_representation = details.representation();
    FieldIndex field_index = FieldIndex::ForPropertyIndex(
        *transition_map, index, field_representation.IsDouble());
    Type* field_type = Type::Tagged();
    if (field_representation.IsSmi()) {
      field_type = type_cache_.kSmi;
    } else if (field_representation.IsDouble()) {
      field_type = type_cache_.kFloat64;
    } else if (field_representation.IsHeapObject()) {
      // Extract the field type from the property details (make sure its
      // representation is TaggedPointer to reflect the heap object case).
      field_type = Type::Intersect(
          Type::Convert<HeapType>(
              handle(
                  transition_map->instance_descriptors()->GetFieldType(number),
                  isolate()),
              zone()),
          Type::TaggedPointer(), zone());
      if (field_type->Is(Type::None())) {
        // Store is not safe if the field type was cleared.
        return false;
      } else if (!Type::Any()->Is(field_type)) {
        // Add proper code dependencies in case of stable field map(s).
        Handle<Map> field_owner_map(transition_map->FindFieldOwner(number),
                                    isolate());
        dependencies()->AssumeFieldType(field_owner_map);
      }
      DCHECK(field_type->Is(Type::TaggedPointer()));
    }
    dependencies()->AssumeMapNotDeprecated(transition_map);
    *access_info =
        PropertyAccessInfo::DataField(Type::Class(map, zone()), field_index,
                                      field_type, holder, transition_map);
    return true;
  }
  return false;
}


Factory* AccessInfoFactory::factory() const { return isolate()->factory(); }

}  // namespace compiler
}  // namespace internal
}  // namespace v8
