// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/runtime/runtime-utils.h"

#include "src/arguments.h"
#include "src/bootstrapper.h"
#include "src/debug/debug.h"
#include "src/isolate-inl.h"
#include "src/messages.h"
#include "src/runtime/runtime.h"

namespace v8 {
namespace internal {


MaybeHandle<Object> Runtime::GetObjectProperty(Isolate* isolate,
                                               Handle<Object> object,
                                               Handle<Object> key,
                                               LanguageMode language_mode) {
  if (object->IsUndefined() || object->IsNull()) {
    THROW_NEW_ERROR(
        isolate,
        NewTypeError(MessageTemplate::kNonObjectPropertyLoad, key, object),
        Object);
  }

  bool success = false;
  LookupIterator it =
      LookupIterator::PropertyOrElement(isolate, object, key, &success);
  if (!success) return MaybeHandle<Object>();

  return Object::GetProperty(&it, language_mode);
}


static MaybeHandle<Object> KeyedGetObjectProperty(Isolate* isolate,
                                                  Handle<Object> receiver_obj,
                                                  Handle<Object> key_obj,
                                                  LanguageMode language_mode) {
  // Fast cases for getting named properties of the receiver JSObject
  // itself.
  //
  // The global proxy objects has to be excluded since LookupOwn on
  // the global proxy object can return a valid result even though the
  // global proxy object never has properties.  This is the case
  // because the global proxy object forwards everything to its hidden
  // prototype including own lookups.
  //
  // Additionally, we need to make sure that we do not cache results
  // for objects that require access checks.
  if (receiver_obj->IsJSObject()) {
    if (!receiver_obj->IsJSGlobalProxy() &&
        !receiver_obj->IsAccessCheckNeeded() && key_obj->IsName()) {
      DisallowHeapAllocation no_allocation;
      Handle<JSObject> receiver = Handle<JSObject>::cast(receiver_obj);
      Handle<Name> key = Handle<Name>::cast(key_obj);
      if (receiver->IsJSGlobalObject()) {
        // Attempt dictionary lookup.
        GlobalDictionary* dictionary = receiver->global_dictionary();
        int entry = dictionary->FindEntry(key);
        if (entry != GlobalDictionary::kNotFound) {
          DCHECK(dictionary->ValueAt(entry)->IsPropertyCell());
          PropertyCell* cell = PropertyCell::cast(dictionary->ValueAt(entry));
          if (cell->property_details().type() == DATA) {
            Object* value = cell->value();
            if (!value->IsTheHole()) return Handle<Object>(value, isolate);
            // If value is the hole (meaning, absent) do the general lookup.
          }
        }
      } else if (!receiver->HasFastProperties()) {
        // Attempt dictionary lookup.
        NameDictionary* dictionary = receiver->property_dictionary();
        int entry = dictionary->FindEntry(key);
        if ((entry != NameDictionary::kNotFound) &&
            (dictionary->DetailsAt(entry).type() == DATA)) {
          Object* value = dictionary->ValueAt(entry);
          return Handle<Object>(value, isolate);
        }
      }
    } else if (key_obj->IsSmi()) {
      // JSObject without a name key. If the key is a Smi, check for a
      // definite out-of-bounds access to elements, which is a strong indicator
      // that subsequent accesses will also call the runtime. Proactively
      // transition elements to FAST_*_ELEMENTS to avoid excessive boxing of
      // doubles for those future calls in the case that the elements would
      // become FAST_DOUBLE_ELEMENTS.
      Handle<JSObject> js_object = Handle<JSObject>::cast(receiver_obj);
      ElementsKind elements_kind = js_object->GetElementsKind();
      if (IsFastDoubleElementsKind(elements_kind)) {
        if (Smi::cast(*key_obj)->value() >= js_object->elements()->length()) {
          elements_kind = IsFastHoleyElementsKind(elements_kind)
                              ? FAST_HOLEY_ELEMENTS
                              : FAST_ELEMENTS;
          JSObject::TransitionElementsKind(js_object, elements_kind);
        }
      } else {
        DCHECK(IsFastSmiOrObjectElementsKind(elements_kind) ||
               !IsFastElementsKind(elements_kind));
      }
    }
  } else if (receiver_obj->IsString() && key_obj->IsSmi()) {
    // Fast case for string indexing using [] with a smi index.
    Handle<String> str = Handle<String>::cast(receiver_obj);
    int index = Handle<Smi>::cast(key_obj)->value();
    if (index >= 0 && index < str->length()) {
      Factory* factory = isolate->factory();
      return factory->LookupSingleCharacterStringFromCode(
          String::Flatten(str)->Get(index));
    }
  }

  // Fall back to GetObjectProperty.
  return Runtime::GetObjectProperty(isolate, receiver_obj, key_obj,
                                    language_mode);
}


MaybeHandle<Object> Runtime::DeleteObjectProperty(Isolate* isolate,
                                                  Handle<JSReceiver> receiver,
                                                  Handle<Object> key,
                                                  LanguageMode language_mode) {
  bool success = false;
  LookupIterator it = LookupIterator::PropertyOrElement(
      isolate, receiver, key, &success, LookupIterator::HIDDEN);
  if (!success) return MaybeHandle<Object>();

  return JSReceiver::DeleteProperty(&it, language_mode);
}


MaybeHandle<Object> Runtime::SetObjectProperty(Isolate* isolate,
                                               Handle<Object> object,
                                               Handle<Object> key,
                                               Handle<Object> value,
                                               LanguageMode language_mode) {
  if (object->IsUndefined() || object->IsNull()) {
    THROW_NEW_ERROR(
        isolate,
        NewTypeError(MessageTemplate::kNonObjectPropertyStore, key, object),
        Object);
  }

  // Check if the given key is an array index.
  bool success = false;
  LookupIterator it =
      LookupIterator::PropertyOrElement(isolate, object, key, &success);
  if (!success) return MaybeHandle<Object>();

  MAYBE_RETURN_NULL(Object::SetProperty(&it, value, language_mode,
                                        Object::MAY_BE_STORE_FROM_KEYED));
  return value;
}


RUNTIME_FUNCTION(Runtime_GetPrototype) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, obj, 0);
  return *Object::GetPrototype(isolate, obj);
}


RUNTIME_FUNCTION(Runtime_InternalSetPrototype) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(JSReceiver, obj, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, prototype, 1);
  MAYBE_RETURN(
      JSReceiver::SetPrototype(obj, prototype, false, Object::THROW_ON_ERROR),
      isolate->heap()->exception());
  return *obj;
}


RUNTIME_FUNCTION(Runtime_SetPrototype) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(JSReceiver, obj, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, prototype, 1);
  MAYBE_RETURN(
      JSReceiver::SetPrototype(obj, prototype, true, Object::THROW_ON_ERROR),
      isolate->heap()->exception());
  return *obj;
}


// Enumerator used as indices into the array returned from GetOwnProperty
enum PropertyDescriptorIndices {
  IS_ACCESSOR_INDEX,
  VALUE_INDEX,
  GETTER_INDEX,
  SETTER_INDEX,
  WRITABLE_INDEX,
  ENUMERABLE_INDEX,
  CONFIGURABLE_INDEX,
  DESCRIPTOR_SIZE
};


MUST_USE_RESULT static MaybeHandle<Object> GetOwnProperty(Isolate* isolate,
                                                          Handle<JSObject> obj,
                                                          Handle<Name> name) {
  Heap* heap = isolate->heap();
  Factory* factory = isolate->factory();

  PropertyAttributes attrs;
  // Get attributes.
  LookupIterator it = LookupIterator::PropertyOrElement(isolate, obj, name,
                                                        LookupIterator::HIDDEN);
  Maybe<PropertyAttributes> maybe = JSObject::GetPropertyAttributes(&it);

  if (!maybe.IsJust()) return MaybeHandle<Object>();
  attrs = maybe.FromJust();
  if (attrs == ABSENT) return factory->undefined_value();

  DCHECK(!isolate->has_pending_exception());
  Handle<FixedArray> elms = factory->NewFixedArray(DESCRIPTOR_SIZE);
  elms->set(ENUMERABLE_INDEX, heap->ToBoolean((attrs & DONT_ENUM) == 0));
  elms->set(CONFIGURABLE_INDEX, heap->ToBoolean((attrs & DONT_DELETE) == 0));

  bool is_accessor_pair = it.state() == LookupIterator::ACCESSOR &&
                          it.GetAccessors()->IsAccessorPair();
  elms->set(IS_ACCESSOR_INDEX, heap->ToBoolean(is_accessor_pair));

  if (is_accessor_pair) {
    Handle<AccessorPair> accessors =
        Handle<AccessorPair>::cast(it.GetAccessors());
    Handle<Object> getter(accessors->GetComponent(ACCESSOR_GETTER), isolate);
    Handle<Object> setter(accessors->GetComponent(ACCESSOR_SETTER), isolate);
    elms->set(GETTER_INDEX, *getter);
    elms->set(SETTER_INDEX, *setter);
  } else {
    Handle<Object> value;
    ASSIGN_RETURN_ON_EXCEPTION(isolate, value, Object::GetProperty(&it),
                               Object);
    elms->set(WRITABLE_INDEX, heap->ToBoolean((attrs & READ_ONLY) == 0));
    elms->set(VALUE_INDEX, *value);
  }

  return factory->NewJSArrayWithElements(elms);
}


// Returns an array with the property description:
//  if args[1] is not a property on args[0]
//          returns undefined
//  if args[1] is a data property on args[0]
//         [false, value, Writeable, Enumerable, Configurable]
//  if args[1] is an accessor on args[0]
//         [true, GetFunction, SetFunction, Enumerable, Configurable]
RUNTIME_FUNCTION(Runtime_GetOwnProperty) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, obj, 0);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 1);
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result,
                                     GetOwnProperty(isolate, obj, name));
  return *result;
}


RUNTIME_FUNCTION(Runtime_PreventExtensions) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(JSReceiver, obj, 0);
  if (JSReceiver::PreventExtensions(obj, Object::THROW_ON_ERROR).IsNothing())
    return isolate->heap()->exception();
  return *obj;
}


RUNTIME_FUNCTION(Runtime_IsExtensible) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, obj, 0);
  return isolate->heap()->ToBoolean(JSObject::IsExtensible(obj));
}


RUNTIME_FUNCTION(Runtime_OptimizeObjectForAddingMultipleProperties) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_SMI_ARG_CHECKED(properties, 1);
  // Conservative upper limit to prevent fuzz tests from going OOM.
  RUNTIME_ASSERT(properties <= 100000);
  if (object->HasFastProperties() && !object->IsJSGlobalProxy()) {
    JSObject::NormalizeProperties(object, KEEP_INOBJECT_PROPERTIES, properties,
                                  "OptimizeForAdding");
  }
  return *object;
}


RUNTIME_FUNCTION(Runtime_ObjectFreeze) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);

  // %ObjectFreeze is a fast path and these cases are handled elsewhere.
  RUNTIME_ASSERT(!object->HasSloppyArgumentsElements() &&
                 !object->map()->is_observed() && !object->IsJSProxy());

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result, JSObject::Freeze(object));
  return *result;
}


RUNTIME_FUNCTION(Runtime_ObjectSeal) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);

  // %ObjectSeal is a fast path and these cases are handled elsewhere.
  RUNTIME_ASSERT(!object->HasSloppyArgumentsElements() &&
                 !object->map()->is_observed() && !object->IsJSProxy());

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result, JSObject::Seal(object));
  return *result;
}


RUNTIME_FUNCTION(Runtime_LoadGlobalViaContext) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_SMI_ARG_CHECKED(slot, 0);

  // Go up context chain to the script context.
  Handle<Context> script_context(isolate->context()->script_context(), isolate);
  DCHECK(script_context->IsScriptContext());
  DCHECK(script_context->get(slot)->IsPropertyCell());

  // Lookup the named property on the global object.
  Handle<ScopeInfo> scope_info(script_context->scope_info(), isolate);
  Handle<Name> name(scope_info->ContextSlotName(slot), isolate);
  Handle<JSGlobalObject> global_object(script_context->global_object(),
                                       isolate);
  LookupIterator it(global_object, name, LookupIterator::HIDDEN);

  // Switch to fast mode only if there is a data property and it's not on
  // a hidden prototype.
  if (it.state() == LookupIterator::DATA &&
      it.GetHolder<Object>().is_identical_to(global_object)) {
    // Now update the cell in the script context.
    Handle<PropertyCell> cell = it.GetPropertyCell();
    script_context->set(slot, *cell);
  } else {
    // This is not a fast case, so keep this access in a slow mode.
    // Store empty_property_cell here to release the outdated property cell.
    script_context->set(slot, isolate->heap()->empty_property_cell());
  }

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result, Object::GetProperty(&it));
  return *result;
}


namespace {

Object* StoreGlobalViaContext(Isolate* isolate, int slot, Handle<Object> value,
                              LanguageMode language_mode) {
  // Go up context chain to the script context.
  Handle<Context> script_context(isolate->context()->script_context(), isolate);
  DCHECK(script_context->IsScriptContext());
  DCHECK(script_context->get(slot)->IsPropertyCell());

  // Lookup the named property on the global object.
  Handle<ScopeInfo> scope_info(script_context->scope_info(), isolate);
  Handle<Name> name(scope_info->ContextSlotName(slot), isolate);
  Handle<JSGlobalObject> global_object(script_context->global_object(),
                                       isolate);
  LookupIterator it(global_object, name, LookupIterator::HIDDEN);

  // Switch to fast mode only if there is a data property and it's not on
  // a hidden prototype.
  if (it.state() == LookupIterator::DATA &&
      it.GetHolder<Object>().is_identical_to(global_object)) {
    // Now update cell in the script context.
    Handle<PropertyCell> cell = it.GetPropertyCell();
    script_context->set(slot, *cell);
  } else {
    // This is not a fast case, so keep this access in a slow mode.
    // Store empty_property_cell here to release the outdated property cell.
    script_context->set(slot, isolate->heap()->empty_property_cell());
  }

  MAYBE_RETURN(Object::SetProperty(&it, value, language_mode,
                                   Object::CERTAINLY_NOT_STORE_FROM_KEYED),
               isolate->heap()->exception());
  return *value;
}

}  // namespace


RUNTIME_FUNCTION(Runtime_StoreGlobalViaContext_Sloppy) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_SMI_ARG_CHECKED(slot, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 1);

  return StoreGlobalViaContext(isolate, slot, value, SLOPPY);
}


RUNTIME_FUNCTION(Runtime_StoreGlobalViaContext_Strict) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_SMI_ARG_CHECKED(slot, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 1);

  return StoreGlobalViaContext(isolate, slot, value, STRICT);
}


RUNTIME_FUNCTION(Runtime_GetProperty) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 1);

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Runtime::GetObjectProperty(isolate, object, key, SLOPPY));
  return *result;
}


RUNTIME_FUNCTION(Runtime_GetPropertyStrong) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 1);

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Runtime::GetObjectProperty(isolate, object, key, STRONG));
  return *result;
}


// KeyedGetProperty is called from KeyedLoadIC::GenerateGeneric.
RUNTIME_FUNCTION(Runtime_KeyedGetProperty) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_HANDLE_CHECKED(Object, receiver_obj, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key_obj, 1);

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      KeyedGetObjectProperty(isolate, receiver_obj, key_obj, SLOPPY));
  return *result;
}


RUNTIME_FUNCTION(Runtime_KeyedGetPropertyStrong) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_HANDLE_CHECKED(Object, receiver_obj, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key_obj, 1);

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      KeyedGetObjectProperty(isolate, receiver_obj, key_obj, STRONG));
  return *result;
}


RUNTIME_FUNCTION(Runtime_AddNamedProperty) {
  HandleScope scope(isolate);
  RUNTIME_ASSERT(args.length() == 4);

  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 2);
  CONVERT_PROPERTY_ATTRIBUTES_CHECKED(attrs, 3);

#ifdef DEBUG
  uint32_t index = 0;
  DCHECK(!name->ToArrayIndex(&index));
  LookupIterator it(object, name, LookupIterator::OWN_SKIP_INTERCEPTOR);
  Maybe<PropertyAttributes> maybe = JSReceiver::GetPropertyAttributes(&it);
  if (!maybe.IsJust()) return isolate->heap()->exception();
  RUNTIME_ASSERT(!it.IsFound());
#endif

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      JSObject::SetOwnPropertyIgnoreAttributes(object, name, value, attrs));
  return *result;
}


// Adds an element to an array.
// This is used to create an indexed data property into an array.
RUNTIME_FUNCTION(Runtime_AddElement) {
  HandleScope scope(isolate);
  RUNTIME_ASSERT(args.length() == 3);

  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 2);

  uint32_t index = 0;
  CHECK(key->ToArrayIndex(&index));

#ifdef DEBUG
  LookupIterator it(isolate, object, index,
                    LookupIterator::OWN_SKIP_INTERCEPTOR);
  Maybe<PropertyAttributes> maybe = JSReceiver::GetPropertyAttributes(&it);
  if (!maybe.IsJust()) return isolate->heap()->exception();
  RUNTIME_ASSERT(!it.IsFound());

  if (object->IsJSArray()) {
    Handle<JSArray> array = Handle<JSArray>::cast(object);
    RUNTIME_ASSERT(!JSArray::WouldChangeReadOnlyLength(array, index));
  }
#endif

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      JSObject::SetOwnElementIgnoreAttributes(object, index, value, NONE));
  return *result;
}


RUNTIME_FUNCTION(Runtime_AppendElement) {
  HandleScope scope(isolate);
  RUNTIME_ASSERT(args.length() == 2);

  CONVERT_ARG_HANDLE_CHECKED(JSArray, array, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 1);

  uint32_t index;
  CHECK(array->length()->ToArrayIndex(&index));

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result, JSObject::AddDataElement(array, index, value, NONE));
  JSObject::ValidateElements(array);
  return *array;
}


RUNTIME_FUNCTION(Runtime_SetProperty) {
  HandleScope scope(isolate);
  RUNTIME_ASSERT(args.length() == 4);

  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 2);
  CONVERT_LANGUAGE_MODE_ARG_CHECKED(language_mode_arg, 3);
  LanguageMode language_mode = language_mode_arg;

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Runtime::SetObjectProperty(isolate, object, key, value, language_mode));
  return *result;
}


namespace {

// ES6 section 12.5.4.
Object* DeleteProperty(Isolate* isolate, Handle<Object> object,
                       Handle<Object> key, LanguageMode language_mode) {
  Handle<JSReceiver> receiver;
  if (!JSReceiver::ToObject(isolate, object).ToHandle(&receiver)) {
    THROW_NEW_ERROR_RETURN_FAILURE(
        isolate, NewTypeError(MessageTemplate::kUndefinedOrNullToObject));
  }
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Runtime::DeleteObjectProperty(isolate, receiver, key, language_mode));
  return *result;
}

}  // namespace


RUNTIME_FUNCTION(Runtime_DeleteProperty_Sloppy) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 1);
  return DeleteProperty(isolate, object, key, SLOPPY);
}


RUNTIME_FUNCTION(Runtime_DeleteProperty_Strict) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 1);
  return DeleteProperty(isolate, object, key, STRICT);
}


static Object* HasOwnPropertyImplementation(Isolate* isolate,
                                            Handle<JSObject> object,
                                            Handle<Name> key) {
  Maybe<bool> maybe = JSReceiver::HasOwnProperty(object, key);
  if (!maybe.IsJust()) return isolate->heap()->exception();
  if (maybe.FromJust()) return isolate->heap()->true_value();
  // Handle hidden prototypes.  If there's a hidden prototype above this thing
  // then we have to check it for properties, because they are supposed to
  // look like they are on this object.
  PrototypeIterator iter(isolate, object);
  if (!iter.IsAtEnd() &&
      Handle<JSObject>::cast(PrototypeIterator::GetCurrent(iter))
          ->map()
          ->is_hidden_prototype()) {
    // TODO(verwaest): The recursion is not necessary for keys that are array
    // indices. Removing this.
    // Casting to JSObject is fine because JSProxies are never used as
    // hidden prototypes.
    return HasOwnPropertyImplementation(
        isolate, Handle<JSObject>::cast(PrototypeIterator::GetCurrent(iter)),
        key);
  }
  RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);
  return isolate->heap()->false_value();
}


RUNTIME_FUNCTION(Runtime_HasOwnProperty) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0)
  CONVERT_ARG_HANDLE_CHECKED(Name, key, 1);

  uint32_t index;
  const bool key_is_array_index = key->AsArrayIndex(&index);

  // Only JS objects can have properties.
  if (object->IsJSObject()) {
    Handle<JSObject> js_obj = Handle<JSObject>::cast(object);
    // Fast case: either the key is a real named property or it is not
    // an array index and there are no interceptors or hidden
    // prototypes.
    Maybe<bool> maybe = Nothing<bool>();
    if (key_is_array_index) {
      maybe = JSObject::HasOwnElement(js_obj, index);
    } else {
      maybe = JSObject::HasRealNamedProperty(js_obj, key);
    }
    if (!maybe.IsJust()) return isolate->heap()->exception();
    DCHECK(!isolate->has_pending_exception());
    if (maybe.FromJust()) {
      return isolate->heap()->true_value();
    }
    Map* map = js_obj->map();
    if (!key_is_array_index && !map->has_named_interceptor() &&
        !HeapObject::cast(map->prototype())->map()->is_hidden_prototype()) {
      return isolate->heap()->false_value();
    }
    // Slow case.
    return HasOwnPropertyImplementation(isolate, Handle<JSObject>(js_obj),
                                        Handle<Name>(key));
  } else if (object->IsString() && key_is_array_index) {
    // Well, there is one exception:  Handle [] on strings.
    Handle<String> string = Handle<String>::cast(object);
    if (index < static_cast<uint32_t>(string->length())) {
      return isolate->heap()->true_value();
    }
  }
  return isolate->heap()->false_value();
}


// ES6 section 12.9.3, operator in.
RUNTIME_FUNCTION(Runtime_HasProperty) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 1);

  // Check that {object} is actually a receiver.
  if (!object->IsJSReceiver()) {
    THROW_NEW_ERROR_RETURN_FAILURE(
        isolate,
        NewTypeError(MessageTemplate::kInvalidInOperatorUse, key, object));
  }
  Handle<JSReceiver> receiver = Handle<JSReceiver>::cast(object);

  // Convert the {key} to a name.
  Handle<Name> name;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, name,
                                     Object::ToName(isolate, key));

  // Lookup the {name} on {receiver}.
  Maybe<bool> maybe = JSReceiver::HasProperty(receiver, name);
  if (!maybe.IsJust()) return isolate->heap()->exception();
  return isolate->heap()->ToBoolean(maybe.FromJust());
}


RUNTIME_FUNCTION(Runtime_IsPropertyEnumerable) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Name, key, 1);

  Maybe<PropertyAttributes> maybe =
      JSReceiver::GetOwnPropertyAttributes(object, key);
  if (!maybe.IsJust()) return isolate->heap()->exception();
  if (maybe.FromJust() == ABSENT) maybe = Just(DONT_ENUM);
  return isolate->heap()->ToBoolean((maybe.FromJust() & DONT_ENUM) == 0);
}


// Returns either a FixedArray or, if the given object has an enum cache that
// contains all enumerable properties of the object and its prototypes have
// none, the map of the object. This is used to speed up the check for
// deletions during a for-in.
RUNTIME_FUNCTION(Runtime_GetPropertyNamesFast) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);

  CONVERT_ARG_CHECKED(JSReceiver, raw_object, 0);

  if (raw_object->IsSimpleEnum()) return raw_object->map();

  HandleScope scope(isolate);
  Handle<JSReceiver> object(raw_object);
  Handle<FixedArray> content;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, content,
      JSReceiver::GetKeys(object, JSReceiver::INCLUDE_PROTOS));

  // Test again, since cache may have been built by preceding call.
  if (object->IsSimpleEnum()) return object->map();

  return *content;
}


// Return the names of the own named properties.
// args[0]: object
// args[1]: PropertyAttributes as int
RUNTIME_FUNCTION(Runtime_GetOwnPropertyNames) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  if (!args[0]->IsJSObject()) {
    return isolate->heap()->undefined_value();
  }
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_SMI_ARG_CHECKED(filter_value, 1);
  PropertyAttributes filter = static_cast<PropertyAttributes>(filter_value);

  // Find the number of own properties for each of the objects.
  int total_property_count = 0;
  for (PrototypeIterator iter(isolate, object,
                              PrototypeIterator::START_AT_RECEIVER);
       !iter.IsAtEnd(PrototypeIterator::END_AT_NON_HIDDEN); iter.Advance()) {
    // Casting to JSObject is fine because |object| is guaranteed to be one,
    // and we'll only look at hidden prototypes which are never JSProxies.
    Handle<JSObject> jsproto = PrototypeIterator::GetCurrent<JSObject>(iter);
    total_property_count += jsproto->NumberOfOwnProperties(filter);
  }

  // Allocate an array with storage for all the property names.
  Handle<FixedArray> names =
      isolate->factory()->NewFixedArray(total_property_count);

  // Get the property names.
  int next_copy_index = 0;
  int hidden_strings = 0;
  Handle<Object> hidden_string = isolate->factory()->hidden_string();
  for (PrototypeIterator iter(isolate, object,
                              PrototypeIterator::START_AT_RECEIVER);
       !iter.IsAtEnd(PrototypeIterator::END_AT_NON_HIDDEN); iter.Advance()) {
    // Casting to JSObject is fine because |object| is guaranteed to be one,
    // and we'll only look at hidden prototypes which are never JSProxies.
    Handle<JSObject> jsproto = PrototypeIterator::GetCurrent<JSObject>(iter);
    int own = jsproto->GetOwnPropertyNames(*names, next_copy_index, filter);
    // Names from hidden prototypes may already have been added
    // for inherited function template instances. Count the duplicates
    // and stub them out; the final copy pass at the end ignores holes.
    for (int j = next_copy_index; j < next_copy_index + own; j++) {
      Object* name_from_hidden_proto = names->get(j);
      if (isolate->IsInternallyUsedPropertyName(name_from_hidden_proto)) {
        hidden_strings++;
      } else {
        for (int k = 0; k < next_copy_index; k++) {
          Object* name = names->get(k);
          if (name_from_hidden_proto == name) {
            names->set(j, *hidden_string);
            hidden_strings++;
            break;
          }
        }
      }
    }
    next_copy_index += own;
  }

  CHECK_EQ(total_property_count, next_copy_index);

  if (object->IsAccessCheckNeeded() &&
      !isolate->MayAccess(handle(isolate->context()), object)) {
    for (int i = 0; i < total_property_count; i++) {
      Handle<Name> name(Name::cast(names->get(i)));
      if (name.is_identical_to(hidden_string)) continue;
      LookupIterator it(object, name, LookupIterator::HIDDEN_SKIP_INTERCEPTOR);
      if (!JSObject::AllCanRead(&it)) {
        names->set(i, *hidden_string);
        hidden_strings++;
      }
    }
  }

  // Filter out name of hidden properties object and
  // hidden prototype duplicates.
  if (hidden_strings > 0) {
    if (hidden_strings == total_property_count) {
      names = isolate->factory()->empty_fixed_array();
    } else {
      int i;
      for (i = 0; i < total_property_count; i++) {
        Object* name = names->get(i);
        if (name == *hidden_string) break;
      }
      int dest_pos = i;
      for (; i < total_property_count; i++) {
        Object* name = names->get(i);
        if (name == *hidden_string) continue;
        names->set(dest_pos++, name);
      }

      isolate->heap()->RightTrimFixedArray<Heap::CONCURRENT_TO_SWEEPER>(
          *names, hidden_strings);
    }
  }

  return *isolate->factory()->NewJSArrayWithElements(names);
}


// Return the names of the own indexed properties.
// args[0]: object
RUNTIME_FUNCTION(Runtime_GetOwnElementNames) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  if (!args[0]->IsJSObject()) {
    return isolate->heap()->undefined_value();
  }
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);

  // TODO(cbruni): implement proper prototype lookup like in GetOwnPropertyNames
  if (object->IsJSGlobalProxy()) {
    // All the elements are stored on the globa_object and not directly on the
    // global object proxy.
    PrototypeIterator iter(isolate, object,
                           PrototypeIterator::START_AT_PROTOTYPE);
    if (iter.IsAtEnd(PrototypeIterator::END_AT_NON_HIDDEN)) {
      return *isolate->factory()->NewJSArray(0);
    }
    // Casting to JSObject is fine because |object| is guaranteed to be one,
    // and we'll only look at hidden prototypes which are never JSProxies.
    object = PrototypeIterator::GetCurrent<JSObject>(iter);
  }

  int n = object->NumberOfOwnElements(NONE);
  Handle<FixedArray> names = isolate->factory()->NewFixedArray(n);
  object->GetOwnElementKeys(*names, NONE);
  return *isolate->factory()->NewJSArrayWithElements(names);
}


// Return information on whether an object has a named or indexed interceptor.
// args[0]: object
RUNTIME_FUNCTION(Runtime_GetInterceptorInfo) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  if (!args[0]->IsJSObject()) {
    return Smi::FromInt(0);
  }
  CONVERT_ARG_HANDLE_CHECKED(JSObject, obj, 0);

  int result = 0;
  if (obj->HasNamedInterceptor()) result |= 2;
  if (obj->HasIndexedInterceptor()) result |= 1;

  return Smi::FromInt(result);
}


// Return property names from named interceptor.
// args[0]: object
RUNTIME_FUNCTION(Runtime_GetNamedInterceptorPropertyNames) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, obj, 0);

  if (obj->HasNamedInterceptor()) {
    Handle<JSObject> result;
    if (JSObject::GetKeysForNamedInterceptor(obj, obj).ToHandle(&result)) {
      return *result;
    }
  }
  return isolate->heap()->undefined_value();
}


// Return element names from indexed interceptor.
// args[0]: object
RUNTIME_FUNCTION(Runtime_GetIndexedInterceptorElementNames) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, obj, 0);

  if (obj->HasIndexedInterceptor()) {
    Handle<JSObject> result;
    if (JSObject::GetKeysForIndexedInterceptor(obj, obj).ToHandle(&result)) {
      return *result;
    }
  }
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_OwnKeys) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(JSObject, raw_object, 0);
  Handle<JSObject> object(raw_object);

  Handle<FixedArray> contents;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, contents, JSReceiver::GetKeys(object, JSReceiver::OWN_ONLY,
                                             SKIP_SYMBOLS, CONVERT_TO_STRING));
  return *isolate->factory()->NewJSArrayWithElements(contents);
}


RUNTIME_FUNCTION(Runtime_ToFastProperties) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  if (object->IsJSObject() && !object->IsJSGlobalObject()) {
    JSObject::MigrateSlowToFast(Handle<JSObject>::cast(object), 0,
                                "RuntimeToFastProperties");
  }
  return *object;
}


RUNTIME_FUNCTION(Runtime_AllocateHeapNumber) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 0);
  return *isolate->factory()->NewHeapNumber(0);
}


static Object* Runtime_NewObjectHelper(Isolate* isolate,
                                       Handle<Object> constructor,
                                       Handle<Object> original_constructor,
                                       Handle<AllocationSite> site) {
  // If the constructor isn't a proper function we throw a type error.
  if (!constructor->IsJSFunction()) {
    THROW_NEW_ERROR_RETURN_FAILURE(
        isolate, NewTypeError(MessageTemplate::kNotConstructor, constructor));
  }

  Handle<JSFunction> function = Handle<JSFunction>::cast(constructor);

  CHECK(original_constructor->IsJSFunction());
  Handle<JSFunction> original_function =
      Handle<JSFunction>::cast(original_constructor);


  // Check that function is a constructor.
  if (!function->IsConstructor()) {
    THROW_NEW_ERROR_RETURN_FAILURE(
        isolate, NewTypeError(MessageTemplate::kNotConstructor, constructor));
  }

  Debug* debug = isolate->debug();
  // Handle stepping into constructors if step into is active.
  if (debug->StepInActive()) debug->HandleStepIn(function, true);

  // The function should be compiled for the optimization hints to be
  // available.
  Compiler::Compile(function, CLEAR_EXCEPTION);

  JSFunction::EnsureHasInitialMap(function);
  if (function->initial_map()->instance_type() == JS_FUNCTION_TYPE) {
    // The 'Function' function ignores the receiver object when
    // called using 'new' and creates a new JSFunction object that
    // is returned.
    return isolate->heap()->undefined_value();
  }

  Handle<Map> initial_map =
      JSFunction::EnsureDerivedHasInitialMap(original_function, function);

  Handle<JSObject> result =
      isolate->factory()->NewJSObjectFromMap(initial_map, NOT_TENURED, site);

  isolate->counters()->constructed_objects()->Increment();
  isolate->counters()->constructed_objects_runtime()->Increment();

  return *result;
}


RUNTIME_FUNCTION(Runtime_NewObject) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(Object, constructor, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, original_constructor, 1);
  return Runtime_NewObjectHelper(isolate, constructor, original_constructor,
                                 Handle<AllocationSite>::null());
}


RUNTIME_FUNCTION(Runtime_FinalizeInstanceSize) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);

  CONVERT_ARG_HANDLE_CHECKED(Map, initial_map, 0);
  initial_map->CompleteInobjectSlackTracking();

  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_GlobalProxy) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(JSFunction, function, 0);
  return function->context()->global_proxy();
}


RUNTIME_FUNCTION(Runtime_LookupAccessor) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(JSReceiver, receiver, 0);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 1);
  CONVERT_SMI_ARG_CHECKED(flag, 2);
  AccessorComponent component = flag == 0 ? ACCESSOR_GETTER : ACCESSOR_SETTER;
  if (!receiver->IsJSObject()) return isolate->heap()->undefined_value();
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      JSObject::GetAccessor(Handle<JSObject>::cast(receiver), name, component));
  return *result;
}


RUNTIME_FUNCTION(Runtime_LoadMutableDouble) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Smi, index, 1);
  RUNTIME_ASSERT((index->value() & 1) == 1);
  FieldIndex field_index =
      FieldIndex::ForLoadByFieldIndex(object->map(), index->value());
  if (field_index.is_inobject()) {
    RUNTIME_ASSERT(field_index.property_index() <
                   object->map()->GetInObjectProperties());
  } else {
    RUNTIME_ASSERT(field_index.outobject_array_index() <
                   object->properties()->length());
  }
  return *JSObject::FastPropertyAt(object, Representation::Double(),
                                   field_index);
}


RUNTIME_FUNCTION(Runtime_TryMigrateInstance) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  if (!object->IsJSObject()) return Smi::FromInt(0);
  Handle<JSObject> js_object = Handle<JSObject>::cast(object);
  if (!js_object->map()->is_deprecated()) return Smi::FromInt(0);
  // This call must not cause lazy deopts, because it's called from deferred
  // code where we can't handle lazy deopts for lack of a suitable bailout
  // ID. So we just try migration and signal failure if necessary,
  // which will also trigger a deopt.
  if (!JSObject::TryMigrateInstance(js_object)) return Smi::FromInt(0);
  return *object;
}


RUNTIME_FUNCTION(Runtime_IsJSGlobalProxy) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(Object, obj, 0);
  return isolate->heap()->ToBoolean(obj->IsJSGlobalProxy());
}


static bool IsValidAccessor(Handle<Object> obj) {
  return obj->IsUndefined() || obj->IsCallable() || obj->IsNull();
}


// Implements part of 8.12.9 DefineOwnProperty.
// There are 3 cases that lead here:
// Step 4b - define a new accessor property.
// Steps 9c & 12 - replace an existing data property with an accessor property.
// Step 12 - update an existing accessor property with an accessor or generic
//           descriptor.
RUNTIME_FUNCTION(Runtime_DefineAccessorPropertyUnchecked) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 5);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, obj, 0);
  RUNTIME_ASSERT(!obj->IsNull());
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, getter, 2);
  RUNTIME_ASSERT(IsValidAccessor(getter));
  CONVERT_ARG_HANDLE_CHECKED(Object, setter, 3);
  RUNTIME_ASSERT(IsValidAccessor(setter));
  CONVERT_PROPERTY_ATTRIBUTES_CHECKED(attrs, 4);

  RETURN_FAILURE_ON_EXCEPTION(
      isolate, JSObject::DefineAccessor(obj, name, getter, setter, attrs));
  return isolate->heap()->undefined_value();
}


// Implements part of 8.12.9 DefineOwnProperty.
// There are 3 cases that lead here:
// Step 4a - define a new data property.
// Steps 9b & 12 - replace an existing accessor property with a data property.
// Step 12 - update an existing data property with a data or generic
//           descriptor.
RUNTIME_FUNCTION(Runtime_DefineDataPropertyUnchecked) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 4);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 2);
  CONVERT_PROPERTY_ATTRIBUTES_CHECKED(attrs, 3);

  LookupIterator it = LookupIterator::PropertyOrElement(isolate, object, name,
                                                        LookupIterator::OWN);
  if (it.state() == LookupIterator::ACCESS_CHECK && !it.HasAccess()) {
    return isolate->heap()->undefined_value();
  }

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result, JSObject::DefineOwnPropertyIgnoreAttributes(
                           &it, value, attrs, JSObject::DONT_FORCE_FIELD));

  return *result;
}


// Return property without being observable by accessors or interceptors.
RUNTIME_FUNCTION(Runtime_GetDataProperty) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(JSReceiver, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 1);
  return *JSReceiver::GetDataProperty(object, name);
}


RUNTIME_FUNCTION(Runtime_HasFastPackedElements) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(HeapObject, obj, 0);
  return isolate->heap()->ToBoolean(
      IsFastPackedElementsKind(obj->map()->elements_kind()));
}


RUNTIME_FUNCTION(Runtime_ValueOf) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(Object, obj, 0);
  if (!obj->IsJSValue()) return obj;
  return JSValue::cast(obj)->value();
}


RUNTIME_FUNCTION(Runtime_SetValueOf) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_CHECKED(Object, obj, 0);
  CONVERT_ARG_CHECKED(Object, value, 1);
  if (!obj->IsJSValue()) return value;
  JSValue::cast(obj)->set_value(value);
  return value;
}


RUNTIME_FUNCTION(Runtime_JSValueGetValue) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(JSValue, obj, 0);
  return JSValue::cast(obj)->value();
}


RUNTIME_FUNCTION(Runtime_HeapObjectGetMap) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(HeapObject, obj, 0);
  return obj->map();
}


RUNTIME_FUNCTION(Runtime_MapGetInstanceType) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(Map, map, 0);
  return Smi::FromInt(map->instance_type());
}


RUNTIME_FUNCTION(Runtime_ObjectEquals) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_CHECKED(Object, obj1, 0);
  CONVERT_ARG_CHECKED(Object, obj2, 1);
  return isolate->heap()->ToBoolean(obj1 == obj2);
}


RUNTIME_FUNCTION(Runtime_IsSpecObject) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(Object, obj, 0);
  return isolate->heap()->ToBoolean(obj->IsSpecObject());
}


RUNTIME_FUNCTION(Runtime_IsStrong) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(Object, obj, 0);
  return isolate->heap()->ToBoolean(obj->IsJSReceiver() &&
                                    JSReceiver::cast(obj)->map()->is_strong());
}


RUNTIME_FUNCTION(Runtime_ClassOf) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(Object, obj, 0);
  if (!obj->IsJSReceiver()) return isolate->heap()->null_value();
  return JSReceiver::cast(obj)->class_name();
}


RUNTIME_FUNCTION(Runtime_DefineGetterPropertyUnchecked) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 4);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 1);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, getter, 2);
  CONVERT_PROPERTY_ATTRIBUTES_CHECKED(attrs, 3);

  RETURN_FAILURE_ON_EXCEPTION(
      isolate,
      JSObject::DefineAccessor(object, name, getter,
                               isolate->factory()->null_value(), attrs));
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_DefineSetterPropertyUnchecked) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 4);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Name, name, 1);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, setter, 2);
  CONVERT_PROPERTY_ATTRIBUTES_CHECKED(attrs, 3);

  RETURN_FAILURE_ON_EXCEPTION(
      isolate,
      JSObject::DefineAccessor(object, name, isolate->factory()->null_value(),
                               setter, attrs));
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_ToObject) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  Handle<JSReceiver> receiver;
  if (JSReceiver::ToObject(isolate, object).ToHandle(&receiver)) {
    return *receiver;
  }
  THROW_NEW_ERROR_RETURN_FAILURE(
      isolate, NewTypeError(MessageTemplate::kUndefinedOrNullToObject));
}


RUNTIME_FUNCTION(Runtime_ToPrimitive) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, input, 0);
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result,
                                     Object::ToPrimitive(input));
  return *result;
}


RUNTIME_FUNCTION(Runtime_ToPrimitive_Number) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, input, 0);
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result, Object::ToPrimitive(input, ToPrimitiveHint::kNumber));
  return *result;
}


RUNTIME_FUNCTION(Runtime_ToPrimitive_String) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, input, 0);
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result, Object::ToPrimitive(input, ToPrimitiveHint::kString));
  return *result;
}


RUNTIME_FUNCTION(Runtime_ToNumber) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, input, 0);
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result, Object::ToNumber(input));
  return *result;
}


RUNTIME_FUNCTION(Runtime_ToInteger) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, input, 0);
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result,
                                     Object::ToInteger(isolate, input));
  return *result;
}


RUNTIME_FUNCTION(Runtime_ToLength) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, input, 0);
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result,
                                     Object::ToLength(isolate, input));
  return *result;
}


RUNTIME_FUNCTION(Runtime_ToString) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, input, 0);
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result,
                                     Object::ToString(isolate, input));
  return *result;
}


RUNTIME_FUNCTION(Runtime_ToName) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, input, 0);
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result,
                                     Object::ToName(isolate, input));
  return *result;
}


RUNTIME_FUNCTION(Runtime_Equals) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, x, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, y, 1);
  Maybe<bool> result = Object::Equals(x, y);
  if (!result.IsJust()) return isolate->heap()->exception();
  // TODO(bmeurer): Change this at some point to return true/false instead.
  return Smi::FromInt(result.FromJust() ? EQUAL : NOT_EQUAL);
}


RUNTIME_FUNCTION(Runtime_StrictEquals) {
  SealHandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_CHECKED(Object, x, 0);
  CONVERT_ARG_CHECKED(Object, y, 1);
  // TODO(bmeurer): Change this at some point to return true/false instead.
  return Smi::FromInt(x->StrictEquals(y) ? EQUAL : NOT_EQUAL);
}


// TODO(bmeurer): Kill this special wrapper and use TF compatible LessThan,
// GreaterThan, etc. which return true or false.
RUNTIME_FUNCTION(Runtime_Compare) {
  HandleScope scope(isolate);
  DCHECK_EQ(3, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, x, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, y, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, ncr, 2);
  Maybe<ComparisonResult> result = Object::Compare(x, y);
  if (result.IsJust()) {
    switch (result.FromJust()) {
      case ComparisonResult::kLessThan:
        return Smi::FromInt(LESS);
      case ComparisonResult::kEqual:
        return Smi::FromInt(EQUAL);
      case ComparisonResult::kGreaterThan:
        return Smi::FromInt(GREATER);
      case ComparisonResult::kUndefined:
        return *ncr;
    }
    UNREACHABLE();
  }
  return isolate->heap()->exception();
}


// TODO(bmeurer): Kill this special wrapper and use TF compatible LessThan,
// GreaterThan, etc. which return true or false.
RUNTIME_FUNCTION(Runtime_Compare_Strong) {
  HandleScope scope(isolate);
  DCHECK_EQ(3, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, x, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, y, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, ncr, 2);
  Maybe<ComparisonResult> result = Object::Compare(x, y, Strength::STRONG);
  if (result.IsJust()) {
    switch (result.FromJust()) {
      case ComparisonResult::kLessThan:
        return Smi::FromInt(LESS);
      case ComparisonResult::kEqual:
        return Smi::FromInt(EQUAL);
      case ComparisonResult::kGreaterThan:
        return Smi::FromInt(GREATER);
      case ComparisonResult::kUndefined:
        return *ncr;
    }
    UNREACHABLE();
  }
  return isolate->heap()->exception();
}


RUNTIME_FUNCTION(Runtime_InstanceOf) {
  // ECMA-262, section 11.8.6, page 54.
  HandleScope shs(isolate);
  DCHECK_EQ(2, args.length());
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, callable, 1);
  // {callable} must have a [[Call]] internal method.
  if (!callable->IsCallable()) {
    THROW_NEW_ERROR_RETURN_FAILURE(
        isolate,
        NewTypeError(MessageTemplate::kInstanceofFunctionExpected, callable));
  }
  // If {object} is not a receiver, return false.
  if (!object->IsJSReceiver()) {
    return isolate->heap()->false_value();
  }
  // Check if {callable} is bound, if so, get [[BoundFunction]] from it and use
  // that instead of {callable}.
  if (callable->IsJSFunction()) {
    Handle<JSFunction> function = Handle<JSFunction>::cast(callable);
    if (function->shared()->bound()) {
      Handle<BindingsArray> bindings(function->function_bindings(), isolate);
      callable = handle(bindings->bound_function(), isolate);
    }
  }
  DCHECK(callable->IsCallable());
  // Get the "prototype" of {callable}; raise an error if it's not a receiver.
  Handle<Object> prototype;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, prototype,
      Object::GetProperty(callable, isolate->factory()->prototype_string()));
  if (!prototype->IsJSReceiver()) {
    THROW_NEW_ERROR_RETURN_FAILURE(
        isolate,
        NewTypeError(MessageTemplate::kInstanceofNonobjectProto, prototype));
  }
  // Return whether or not {prototype} is in the prototype chain of {object}.
  return isolate->heap()->ToBoolean(
      object->HasInPrototypeChain(isolate, *prototype));
}


RUNTIME_FUNCTION(Runtime_HasInPrototypeChain) {
  SealHandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_CHECKED(Object, object, 0);
  CONVERT_ARG_CHECKED(Object, prototype, 1);
  return isolate->heap()->ToBoolean(
      object->HasInPrototypeChain(isolate, prototype));
}


// ES6 section 7.4.7 CreateIterResultObject ( value, done )
RUNTIME_FUNCTION(Runtime_CreateIterResultObject) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, done, 1);
  return *isolate->factory()->NewJSIteratorResult(value, done);
}


RUNTIME_FUNCTION(Runtime_IsAccessCheckNeeded) {
  SealHandleScope shs(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_CHECKED(Object, object, 0);
  return isolate->heap()->ToBoolean(object->IsAccessCheckNeeded());
}


RUNTIME_FUNCTION(Runtime_ObjectDefineProperty) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(Object, o, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, name, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, attributes, 2);
  return JSReceiver::DefineProperty(isolate, o, name, attributes);
}


RUNTIME_FUNCTION(Runtime_ObjectDefineProperties) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);
  CONVERT_ARG_HANDLE_CHECKED(Object, o, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, properties, 1);
  return JSReceiver::DefineProperties(isolate, o, properties);
}
}  // namespace internal
}  // namespace v8
