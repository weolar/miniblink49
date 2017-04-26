// Copyright 2011 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/property-descriptor.h"

#include "src/bootstrapper.h"
#include "src/factory.h"
#include "src/isolate-inl.h"
#include "src/lookup.h"
#include "src/objects-inl.h"

namespace v8 {
namespace internal {

// Helper function for ToPropertyDescriptor. Comments describe steps for
// "enumerable", other properties are handled the same way.
// Returns false if an exception was thrown.
bool GetPropertyIfPresent(Handle<Object> obj, Handle<String> name,
                          Handle<Object>* value) {
  LookupIterator it(obj, name);
  // 4. Let hasEnumerable be HasProperty(Obj, "enumerable").
  Maybe<PropertyAttributes> maybe_attr = JSReceiver::GetPropertyAttributes(&it);
  // 5. ReturnIfAbrupt(hasEnumerable).
  if (!maybe_attr.IsJust()) return false;
  // 6. If hasEnumerable is true, then
  if (maybe_attr.FromJust() != ABSENT) {
    // 6a. Let enum be ToBoolean(Get(Obj, "enumerable")).
    // 6b. ReturnIfAbrupt(enum).
    if (!JSObject::GetProperty(&it).ToHandle(value)) return false;
  }
  return true;
}


// Helper function for ToPropertyDescriptor. Handles the case of "simple"
// objects: nothing on the prototype chain, just own fast data properties.
// Must not have observable side effects, because the slow path will restart
// the entire conversion!
bool ToPropertyDescriptorFastPath(Isolate* isolate, Handle<Object> obj,
                                  PropertyDescriptor* desc) {
  if (!obj->IsJSObject()) return false;
  Map* map = Handle<JSObject>::cast(obj)->map();
  if (map->instance_type() != JS_OBJECT_TYPE) return false;
  if (map->is_access_check_needed()) return false;
  if (map->prototype() != *isolate->initial_object_prototype()) return false;
  // During bootstrapping, the object_function_prototype_map hasn't been
  // set up yet.
  if (isolate->bootstrapper()->IsActive()) return false;
  if (JSObject::cast(map->prototype())->map() !=
      isolate->native_context()->object_function_prototype_map()) {
    return false;
  }
  // TODO(jkummerow): support dictionary properties?
  if (map->is_dictionary_map()) return false;
  Handle<DescriptorArray> descs =
      Handle<DescriptorArray>(map->instance_descriptors());
  for (int i = 0; i < map->NumberOfOwnDescriptors(); i++) {
    PropertyDetails details = descs->GetDetails(i);
    Name* key = descs->GetKey(i);
    Handle<Object> value;
    switch (details.type()) {
      case DATA:
        value = JSObject::FastPropertyAt(Handle<JSObject>::cast(obj),
                                         details.representation(),
                                         FieldIndex::ForDescriptor(map, i));
        break;
      case DATA_CONSTANT:
        value = handle(descs->GetConstant(i), isolate);
        break;
      case ACCESSOR:
      case ACCESSOR_CONSTANT:
        // Bail out to slow path.
        return false;
    }
    Heap* heap = isolate->heap();
    if (key == heap->enumerable_string()) {
      desc->set_enumerable(value->BooleanValue());
    } else if (key == heap->configurable_string()) {
      desc->set_configurable(value->BooleanValue());
    } else if (key == heap->value_string()) {
      desc->set_value(value);
    } else if (key == heap->writable_string()) {
      desc->set_writable(value->BooleanValue());
    } else if (key == heap->get_string()) {
      // Bail out to slow path to throw an exception if necessary.
      if (!value->IsCallable()) return false;
      desc->set_get(value);
    } else if (key == heap->set_string()) {
      // Bail out to slow path to throw an exception if necessary.
      if (!value->IsCallable()) return false;
      desc->set_set(value);
    }
  }
  if ((desc->has_get() || desc->has_set()) &&
      (desc->has_value() || desc->has_writable())) {
    // Bail out to slow path to throw an exception.
    return false;
  }
  return true;
}


static void CreateDataProperty(Isolate* isolate, Handle<JSObject> object,
                               Handle<String> name, Handle<Object> value) {
  LookupIterator it(object, name);
  Maybe<bool> result = JSObject::CreateDataProperty(&it, value);
  CHECK(result.IsJust() && result.FromJust());
}


// ES6 6.2.4.4 "FromPropertyDescriptor"
Handle<Object> PropertyDescriptor::ToObject(Isolate* isolate) {
  DCHECK(!(PropertyDescriptor::IsAccessorDescriptor(this) &&
           PropertyDescriptor::IsDataDescriptor(this)));
  Factory* factory = isolate->factory();
  Handle<JSObject> result = factory->NewJSObject(isolate->object_function());
  if (has_value()) {
    CreateDataProperty(isolate, result, factory->value_string(), value());
  }
  if (has_writable()) {
    CreateDataProperty(isolate, result, factory->writable_string(),
                       factory->ToBoolean(writable()));
  }
  if (has_get()) {
    CreateDataProperty(isolate, result, factory->get_string(), get());
  }
  if (has_set()) {
    CreateDataProperty(isolate, result, factory->set_string(), set());
  }
  if (has_enumerable()) {
    CreateDataProperty(isolate, result, factory->enumerable_string(),
                       factory->ToBoolean(enumerable()));
  }
  if (has_configurable()) {
    CreateDataProperty(isolate, result, factory->configurable_string(),
                       factory->ToBoolean(configurable()));
  }
  return result;
}


// ES6 6.2.4.5
// Returns false in case of exception.
// static
bool PropertyDescriptor::ToPropertyDescriptor(Isolate* isolate,
                                              Handle<Object> obj,
                                              PropertyDescriptor* desc) {
  // 1. ReturnIfAbrupt(Obj).
  // 2. If Type(Obj) is not Object, throw a TypeError exception.
  if (!obj->IsSpecObject()) {
    isolate->Throw(*isolate->factory()->NewTypeError(
        MessageTemplate::kPropertyDescObject, obj));
    return false;
  }
  // 3. Let desc be a new Property Descriptor that initially has no fields.
  DCHECK(desc->is_empty());

  if (ToPropertyDescriptorFastPath(isolate, obj, desc)) {
    return true;
  }

  // TODO(jkummerow): Implement JSProxy support.
  // Specifically, instead of taking the attributes != ABSENT shortcut, we
  // have to implement proper HasProperty for proxies.
  if (!obj->IsJSProxy()) {
    {  // enumerable?
      Handle<Object> enumerable;
      // 4 through 6b.
      if (!GetPropertyIfPresent(obj, isolate->factory()->enumerable_string(),
                                &enumerable)) {
        return false;
      }
      // 6c. Set the [[Enumerable]] field of desc to enum.
      if (!enumerable.is_null()) {
        desc->set_enumerable(enumerable->BooleanValue());
      }
    }
    {  // configurable?
      Handle<Object> configurable;
      // 7 through 9b.
      if (!GetPropertyIfPresent(obj, isolate->factory()->configurable_string(),
                                &configurable)) {
        return false;
      }
      // 9c. Set the [[Configurable]] field of desc to conf.
      if (!configurable.is_null()) {
        desc->set_configurable(configurable->BooleanValue());
      }
    }
    {  // value?
      Handle<Object> value;
      // 10 through 12b.
      if (!GetPropertyIfPresent(obj, isolate->factory()->value_string(),
                                &value))
        return false;
      // 12c. Set the [[Value]] field of desc to value.
      if (!value.is_null()) desc->set_value(value);
    }
    {  // writable?
      Handle<Object> writable;
      // 13 through 15b.
      if (!GetPropertyIfPresent(obj, isolate->factory()->writable_string(),
                                &writable)) {
        return false;
      }
      // 15c. Set the [[Writable]] field of desc to writable.
      if (!writable.is_null()) desc->set_writable(writable->BooleanValue());
    }
    {  // getter?
      Handle<Object> getter;
      // 16 through 18b.
      if (!GetPropertyIfPresent(obj, isolate->factory()->get_string(), &getter))
        return false;
      if (!getter.is_null()) {
        // 18c. If IsCallable(getter) is false and getter is not undefined,
        // throw a TypeError exception.
        if (!getter->IsCallable() && !getter->IsUndefined()) {
          isolate->Throw(*isolate->factory()->NewTypeError(
              MessageTemplate::kObjectGetterCallable, getter));
          return false;
        }
        // 18d. Set the [[Get]] field of desc to getter.
        desc->set_get(getter);
      }
      {  // setter?
        Handle<Object> setter;
        // 19 through 21b.
        if (!GetPropertyIfPresent(obj, isolate->factory()->set_string(),
                                  &setter))
          return false;
        if (!setter.is_null()) {
          // 21c. If IsCallable(setter) is false and setter is not undefined,
          // throw a TypeError exception.
          if (!setter->IsCallable() && !setter->IsUndefined()) {
            isolate->Throw(*isolate->factory()->NewTypeError(
                MessageTemplate::kObjectSetterCallable, setter));
            return false;
          }
          // 21d. Set the [[Set]] field of desc to setter.
          desc->set_set(setter);
        }
      }
      // 22. If either desc.[[Get]] or desc.[[Set]] is present, then
      // 22a. If either desc.[[Value]] or desc.[[Writable]] is present,
      // throw a TypeError exception.
      if ((desc->has_get() || desc->has_set()) &&
          (desc->has_value() || desc->has_writable())) {
        isolate->Throw(*isolate->factory()->NewTypeError(
            MessageTemplate::kValueAndAccessor, obj));
        return false;
      }
    }
  } else {
    DCHECK(obj->IsJSProxy());
    // Having an UNIMPLEMENTED() here would upset ClusterFuzz, because
    // --harmony-proxies makes it possible to reach this branch.
    isolate->Throw(
        *isolate->factory()->NewTypeError(MessageTemplate::kUnsupported));
    return false;
  }
  // 23. Return desc.
  return true;
}


}  // namespace internal
}  // namespace v8
