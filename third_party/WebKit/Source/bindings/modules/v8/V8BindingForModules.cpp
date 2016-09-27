/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "bindings/modules/v8/V8BindingForModules.h"

#include "bindings/core/v8/SerializedScriptValue.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8ArrayBufferView.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8Blob.h"
#include "bindings/core/v8/V8DOMStringList.h"
#include "bindings/core/v8/V8File.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "bindings/core/v8/V8Uint8Array.h"
#include "bindings/modules/v8/ToV8ForModules.h"
#include "bindings/modules/v8/V8IDBCursor.h"
#include "bindings/modules/v8/V8IDBCursorWithValue.h"
#include "bindings/modules/v8/V8IDBDatabase.h"
#include "bindings/modules/v8/V8IDBIndex.h"
#include "bindings/modules/v8/V8IDBKeyRange.h"
#include "bindings/modules/v8/V8IDBObjectStore.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMArrayBufferView.h"
#include "modules/indexeddb/IDBKey.h"
#include "modules/indexeddb/IDBKeyPath.h"
#include "modules/indexeddb/IDBKeyRange.h"
#include "modules/indexeddb/IDBTracing.h"
#include "modules/indexeddb/IDBValue.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/SharedBuffer.h"
#include "wtf/MathExtras.h"
#include "wtf/Vector.h"

namespace blink {

static v8::Local<v8::Value> deserializeIDBValueData(v8::Isolate*, const IDBValue*);
static v8::Local<v8::Value> deserializeIDBValue(v8::Isolate*, v8::Local<v8::Object> creationContext, const IDBValue*);
static v8::Local<v8::Value> deserializeIDBValueArray(v8::Isolate*, v8::Local<v8::Object> creationContext, const Vector<RefPtr<IDBValue>>*);

v8::Local<v8::Value> toV8(const IDBKeyPath& value, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    switch (value.type()) {
    case IDBKeyPath::NullType:
        return v8::Null(isolate);
    case IDBKeyPath::StringType:
        return v8String(isolate, value.string());
    case IDBKeyPath::ArrayType:
        return toV8(value.array(), creationContext, isolate);
    }
    ASSERT_NOT_REACHED();
    return v8::Undefined(isolate);
}

v8::Local<v8::Value> toV8(const IDBKey* key, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (!key) {
        // The IndexedDB spec requires that absent keys appear as attribute
        // values as undefined, rather than the more typical (for DOM) null.
        // This appears on the |upper| and |lower| attributes of IDBKeyRange.
        // Spec: http://www.w3.org/TR/IndexedDB/#idl-def-IDBKeyRange
        return v8Undefined();
    }

    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    switch (key->type()) {
    case IDBKey::InvalidType:
    case IDBKey::MinType:
        ASSERT_NOT_REACHED();
        return v8Undefined();
    case IDBKey::NumberType:
        return v8::Number::New(isolate, key->number());
    case IDBKey::StringType:
        return v8String(isolate, key->string());
    case IDBKey::BinaryType:
        // Experimental feature: binary keys
        // https://w3c.github.io/IndexedDB/#steps-to-convert-a-key-to-a-value
        return toV8(DOMArrayBuffer::create(reinterpret_cast<const unsigned char*>(key->binary()->data()), key->binary()->size()), creationContext, isolate);
    case IDBKey::DateType:
        return v8::Date::New(context, key->date()).ToLocalChecked();
    case IDBKey::ArrayType:
        {
            v8::Local<v8::Array> array = v8::Array::New(isolate, key->array().size());
            for (size_t i = 0; i < key->array().size(); ++i) {
                v8::Local<v8::Value> value = toV8(key->array()[i].get(), creationContext, isolate);
                if (value.IsEmpty())
                    value = v8::Undefined(isolate);
                if (!v8CallBoolean(array->CreateDataProperty(context, i, value)))
                    return v8Undefined();
            }
            return array;
        }
    }

    ASSERT_NOT_REACHED();
    return v8Undefined();
}

// IDBAny is a a variant type used to hold the values produced by the |result|
// attribute of IDBRequest and (as a convenience) the |source| attribute of
// IDBRequest and IDBCursor.
// TODO(jsbell): Once the custom wrapper linking between cursor and request
// is eliminated, remove the |source| support by using union types in the IDL.
v8::Local<v8::Value> toV8(const IDBAny* impl, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (!impl)
        return v8::Null(isolate);

    switch (impl->type()) {
    case IDBAny::UndefinedType:
        return v8::Undefined(isolate);
    case IDBAny::NullType:
        return v8::Null(isolate);
    case IDBAny::DOMStringListType:
        return toV8(impl->domStringList(), creationContext, isolate);
    case IDBAny::IDBCursorType: {
        // Ensure request wrapper is kept alive at least as long as the cursor wrapper,
        // so that event listeners are retained.
        // TODO(jsbell): Use [SetWrapperReferenceTo] instead.
        v8::Local<v8::Value> cursor = toV8(impl->idbCursor(), creationContext, isolate);
        v8::Local<v8::Value> request = toV8(impl->idbCursor()->request(), creationContext, isolate);

        // FIXME: Due to race at worker shutdown, V8 may return empty handles.
        if (!cursor.IsEmpty() && cursor->IsObject())
            V8HiddenValue::setHiddenValue(isolate, cursor.As<v8::Object>(), V8HiddenValue::idbCursorRequest(isolate), request);
        return cursor;
    }
    case IDBAny::IDBCursorWithValueType: {
        // Ensure request wrapper is kept alive at least as long as the cursor wrapper,
        // so that event listeners are retained.
        // TODO(jsbell): Use [SetWrapperReferenceTo] instead.
        v8::Local<v8::Value> cursor = toV8(impl->idbCursorWithValue(), creationContext, isolate);
        v8::Local<v8::Value> request = toV8(impl->idbCursorWithValue()->request(), creationContext, isolate);

        // FIXME: Due to race at worker shutdown, V8 may return empty handles.
        if (!cursor.IsEmpty() && cursor->IsObject())
            V8HiddenValue::setHiddenValue(isolate, cursor.As<v8::Object>(), V8HiddenValue::idbCursorRequest(isolate), request);
        return cursor;
    }
    case IDBAny::IDBDatabaseType:
        return toV8(impl->idbDatabase(), creationContext, isolate);
    case IDBAny::IDBIndexType:
        return toV8(impl->idbIndex(), creationContext, isolate);
    case IDBAny::IDBObjectStoreType:
        return toV8(impl->idbObjectStore(), creationContext, isolate);
    case IDBAny::IDBValueType:
        return deserializeIDBValue(isolate, creationContext, impl->value());
    case IDBAny::IDBValueArrayType:
        return deserializeIDBValueArray(isolate, creationContext, impl->values());
    case IDBAny::IntegerType:
        return v8::Number::New(isolate, impl->integer());
    case IDBAny::KeyType:
        return toV8(impl->key(), creationContext, isolate);
    }

    ASSERT_NOT_REACHED();
    return v8::Undefined(isolate);
}

static const size_t maximumDepth = 2000;

static IDBKey* createIDBKeyFromValue(v8::Isolate* isolate, v8::Local<v8::Value> value, Vector<v8::Local<v8::Array>>& stack, ExceptionState& exceptionState, bool allowExperimentalTypes = false)
{
    if (value->IsNumber() && !std::isnan(value.As<v8::Number>()->Value()))
        return IDBKey::createNumber(value.As<v8::Number>()->Value());
    if (value->IsString())
        return IDBKey::createString(toCoreString(value.As<v8::String>()));
    if (value->IsDate() && !std::isnan(value.As<v8::Date>()->ValueOf()))
        return IDBKey::createDate(value.As<v8::Date>()->ValueOf());
    if (allowExperimentalTypes || RuntimeEnabledFeatures::indexedDBExperimentalEnabled()) {
        // Experimental feature: binary keys
        // https://w3c.github.io/IndexedDB/#dfn-convert-a-value-to-a-key
        if (value->IsArrayBuffer()) {
            DOMArrayBuffer* buffer = V8ArrayBuffer::toImpl(value.As<v8::Object>());
            const char* start = static_cast<const char*>(buffer->data());
            size_t length = buffer->byteLength();
            return IDBKey::createBinary(SharedBuffer::create(start, length));
        }
        if (value->IsArrayBufferView()) {
            DOMArrayBufferView* view = V8ArrayBufferView::toImpl(value.As<v8::Object>());
            const char* start = static_cast<const char*>(view->baseAddress());
            size_t length = view->byteLength();
            return IDBKey::createBinary(SharedBuffer::create(start, length));
        }
    }
    if (value->IsArray()) {
        v8::Local<v8::Array> array = value.As<v8::Array>();

        if (stack.contains(array))
            return nullptr;
        if (stack.size() >= maximumDepth)
            return nullptr;
        stack.append(array);

        IDBKey::KeyArray subkeys;
        uint32_t length = array->Length();
        v8::TryCatch block;
        for (uint32_t i = 0; i < length; ++i) {
            v8::Local<v8::Value> item;
            if (!v8Call(array->Get(isolate->GetCurrentContext(), i), item, block)) {
                exceptionState.rethrowV8Exception(block.Exception());
                return nullptr;
            }
            IDBKey* subkey = createIDBKeyFromValue(isolate, item, stack, exceptionState, allowExperimentalTypes);
            if (!subkey)
                subkeys.append(IDBKey::createInvalid());
            else
                subkeys.append(subkey);
        }

        stack.removeLast();
        return IDBKey::createArray(subkeys);
    }
    return nullptr;
}

static IDBKey* createIDBKeyFromValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState, bool allowExperimentalTypes = false)
{
    Vector<v8::Local<v8::Array>> stack;
    if (IDBKey* key = createIDBKeyFromValue(isolate, value, stack, exceptionState, allowExperimentalTypes))
        return key;
    return IDBKey::createInvalid();
}

// Indexed DB key paths should apply to explicitly copied properties (that
// will be "own" properties when deserialized) as well as the following.
// http://www.w3.org/TR/IndexedDB/#key-path-construct
static bool isImplicitProperty(v8::Isolate* isolate, v8::Local<v8::Value> value, const String& name)
{
    if (value->IsString() && name == "length")
        return true;
    if (value->IsArray() && name == "length")
        return true;
    if (V8Blob::hasInstance(value, isolate))
        return name == "size" || name == "type";
    if (V8File::hasInstance(value, isolate))
        return name == "name" || name == "lastModified" || name =="lastModifiedDate";
    return false;
}

// Assumes a valid key path.
static Vector<String> parseKeyPath(const String& keyPath)
{
    Vector<String> elements;
    IDBKeyPathParseError error;
    IDBParseKeyPath(keyPath, elements, error);
    ASSERT(error == IDBKeyPathParseErrorNone);
    return elements;
}

static IDBKey* createIDBKeyFromValueAndKeyPath(v8::Isolate* isolate, v8::Local<v8::Value> v8Value, const String& keyPath, ExceptionState& exceptionState, bool allowExperimentalTypes)
{
    Vector<String> keyPathElements = parseKeyPath(keyPath);
    ASSERT(isolate->InContext());

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::TryCatch block;
    for (size_t i = 0; i < keyPathElements.size(); ++i) {
        if (v8Value->IsString() && keyPathElements[i] == "length") {
            int32_t length = v8Value.As<v8::String>()->Length();
            v8Value = v8::Number::New(isolate, length);
        } else if (!v8Value->IsObject()) {
            return nullptr;
        } else {
            v8::Local<v8::Object> object = v8Value.As<v8::Object>();
            v8::Local<v8::String> key = v8String(isolate, keyPathElements[i]);
            if (!v8CallBoolean(object->Has(context, key)))
                return nullptr;
            if (!v8Call(object->Get(context, key), v8Value, block)) {
                exceptionState.rethrowV8Exception(block.Exception());
                return nullptr;
            }
        }
    }
    return createIDBKeyFromValue(isolate, v8Value, exceptionState, allowExperimentalTypes);
}

static IDBKey* createIDBKeyFromValueAndKeyPath(v8::Isolate* isolate, v8::Local<v8::Value> value, const IDBKeyPath& keyPath, ExceptionState& exceptionState, bool allowExperimentalTypes = false)
{
    ASSERT(!keyPath.isNull());
    v8::HandleScope handleScope(isolate);
    if (keyPath.type() == IDBKeyPath::ArrayType) {
        IDBKey::KeyArray result;
        const Vector<String>& array = keyPath.array();
        for (size_t i = 0; i < array.size(); ++i) {
            IDBKey* key = createIDBKeyFromValueAndKeyPath(isolate, value, array[i], exceptionState, allowExperimentalTypes);
            if (!key)
                return nullptr;
            result.append(key);
        }
        return IDBKey::createArray(result);
    }

    ASSERT(keyPath.type() == IDBKeyPath::StringType);
    return createIDBKeyFromValueAndKeyPath(isolate, value, keyPath.string(), exceptionState, allowExperimentalTypes);
}

// Deserialize just the value data & blobInfo from the given IDBValue.
// Does not deserialize the key & keypath.
static v8::Local<v8::Value> deserializeIDBValueData(v8::Isolate* isolate, const IDBValue* value)
{
    ASSERT(isolate->InContext());
    if (value->isNull())
        return v8::Null(isolate);

    const SharedBuffer* valueData = value->data();
    RefPtr<SerializedScriptValue> serializedValue = SerializedScriptValueFactory::instance().createFromWireBytes(valueData->data(), valueData->size());
    return serializedValue->deserialize(isolate, nullptr, value->blobInfo());
}

// Deserialize the entire IDBValue (injecting key & keypath if present).
static v8::Local<v8::Value> deserializeIDBValue(v8::Isolate* isolate, v8::Local<v8::Object> creationContext, const IDBValue* value)
{
    ASSERT(isolate->InContext());
    if (value->isNull())
        return v8::Null(isolate);

    v8::Local<v8::Value> v8Value = deserializeIDBValueData(isolate, value);
    if (value->primaryKey()) {
        v8::Local<v8::Value> key = toV8(value->primaryKey(), creationContext, isolate);
        if (key.IsEmpty())
            return v8::Local<v8::Value>();
        bool injected = injectV8KeyIntoV8Value(isolate, key, v8Value, value->keyPath());
        ASSERT_UNUSED(injected, injected);
    }

    return v8Value;
}

static v8::Local<v8::Value> deserializeIDBValueArray(v8::Isolate* isolate, v8::Local<v8::Object> creationContext, const Vector<RefPtr<IDBValue>>* values)
{
    ASSERT(isolate->InContext());

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Array> array = v8::Array::New(isolate, values->size());
    for (size_t i = 0; i < values->size(); ++i) {
        v8::Local<v8::Value> v8Value = deserializeIDBValue(isolate, creationContext, values->at(i).get());
        if (v8Value.IsEmpty())
            v8Value = v8::Undefined(isolate);
        if (!v8CallBoolean(array->CreateDataProperty(context, i, v8Value)))
            return v8Undefined();
    }

    return array;
}

// This is only applied to deserialized values which were validated before
// serialization, so various assumptions/assertions can be made.
bool injectV8KeyIntoV8Value(v8::Isolate* isolate, v8::Local<v8::Value> key, v8::Local<v8::Value> value, const IDBKeyPath& keyPath)
{
    IDB_TRACE("injectIDBV8KeyIntoV8Value");
    ASSERT(isolate->InContext());

    ASSERT(keyPath.type() == IDBKeyPath::StringType);
    Vector<String> keyPathElements = parseKeyPath(keyPath.string());

    // The conbination of a key generator and an empty key path is forbidden by spec.
    if (!keyPathElements.size()) {
        ASSERT_NOT_REACHED();
        return false;
    }

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    // For an object o = {} which should have keypath 'a.b.c' and key k, this
    // populates o to be {a:{b:{}}}. This is only applied to deserialized
    // values which were validated before serialization, so various
    // assumptions can be made, e.g. there are no getters/setters on the
    // object itself (though there might be on the prototype chain).
    for (size_t i = 0; i < keyPathElements.size() - 1; ++i) {
        const String& keyPathElement = keyPathElements[i];
        ASSERT(value->IsObject());
        ASSERT(!isImplicitProperty(isolate, value, keyPathElement));
        v8::Local<v8::Object> object = value.As<v8::Object>();
        v8::Local<v8::String> property = v8String(isolate, keyPathElement);
        bool hasOwnProperty;
        if (!v8Call(object->HasOwnProperty(context, property), hasOwnProperty))
            return false;
        if (hasOwnProperty) {
            if (!object->Get(context, property).ToLocal(&value))
                return false;
        } else {
            value = v8::Object::New(isolate);
            if (!v8CallBoolean(object->CreateDataProperty(context, property, value)))
                return false;
        }
    }

    // Implicit properties don't need to be set. The caller is not required to
    // be aware of this, so this is an expected no-op. The caller can verify
    // that the value is correct via assertPrimaryKeyValidOrInjectable.
    if (isImplicitProperty(isolate, value, keyPathElements.last()))
        return true;

    // If it's not an implicit property of value, value must be an object.
    v8::Local<v8::Object> object = value.As<v8::Object>();
    v8::Local<v8::String> property = v8String(isolate, keyPathElements.last());
    if (!v8CallBoolean(object->CreateDataProperty(context, property, key)))
        return false;

    return true;
}

// Verify that an value can have an generated key inserted at the location
// specified by the key path (by injectV8KeyIntoV8Value) when the object is
// later deserialized.
bool canInjectIDBKeyIntoScriptValue(v8::Isolate* isolate, const ScriptValue& scriptValue, const IDBKeyPath& keyPath)
{
    IDB_TRACE("canInjectIDBKeyIntoScriptValue");
    ASSERT(keyPath.type() == IDBKeyPath::StringType);
    Vector<String> keyPathElements = parseKeyPath(keyPath.string());

    if (!keyPathElements.size())
        return false;

    v8::Local<v8::Value> current(scriptValue.v8Value());
    if (!current->IsObject())
        return false;

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    for (size_t i = 0; i < keyPathElements.size(); ++i) {
        const String& keyPathElement = keyPathElements[i];
        // Can't overwrite properties like array or string length.
        if (isImplicitProperty(isolate, current, keyPathElement))
            return false;
        // Can't set properties on non-objects.
        if (!current->IsObject())
            return false;
        v8::Local<v8::Object> object = current.As<v8::Object>();
        v8::Local<v8::String> property = v8String(isolate, keyPathElement);
        // If the value lacks an "own" property, it can be added - either as
        // an intermediate object or as the final value.
        bool hasOwnProperty;
        if (!v8Call(object->HasOwnProperty(context, property), hasOwnProperty))
            return false;
        if (!hasOwnProperty)
            return true;
        // Otherwise, get it and keep traversing.
        if (!object->Get(context, property).ToLocal(&current))
            return false;
    }
    return true;
}

ScriptValue deserializeScriptValue(ScriptState* scriptState, SerializedScriptValue* serializedValue, const Vector<WebBlobInfo>* blobInfo)
{
    v8::Isolate* isolate = scriptState->isolate();
    v8::HandleScope handleScope(isolate);
    if (serializedValue)
        return ScriptValue(scriptState, serializedValue->deserialize(isolate, nullptr, blobInfo));
    return ScriptValue(scriptState, v8::Null(isolate));
}

SQLValue NativeValueTraits<SQLValue>::nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    if (value.IsEmpty() || value->IsNull())
        return SQLValue();
    if (value->IsNumber())
        return SQLValue(value.As<v8::Number>()->Value());
    V8StringResource<> stringValue(value);
    if (!stringValue.prepare(exceptionState))
        return SQLValue();
    return SQLValue(stringValue);
}

IDBKey* NativeValueTraits<IDBKey*>::nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    return createIDBKeyFromValue(isolate, value, exceptionState);
}

IDBKey* NativeValueTraits<IDBKey*>::nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState, const IDBKeyPath& keyPath)
{
    IDB_TRACE("createIDBKeyFromValueAndKeyPath");
    return createIDBKeyFromValueAndKeyPath(isolate, value, keyPath, exceptionState);
}

IDBKeyRange* NativeValueTraits<IDBKeyRange*>::nativeValue(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState)
{
    return V8IDBKeyRange::toImplWithTypeCheck(isolate, value);
}

#if ENABLE(ASSERT)
// This assertion is used when a value has been retrieved from an object store
// with implicit keys (i.e. a key path). It verifies that either the value
// contains an implicit key matching the primary key (so it was correctly
// extracted when stored) or that the key can be inserted as an own property.
void assertPrimaryKeyValidOrInjectable(ScriptState* scriptState, const IDBValue* value)
{
    ScriptState::Scope scope(scriptState);
    v8::Isolate* isolate = scriptState->isolate();
    ScriptValue keyValue = ScriptValue::from(scriptState, value->primaryKey());
    ScriptValue scriptValue(scriptState, deserializeIDBValueData(isolate, value));

    // This assertion is about already persisted data, so allow experimental types.
    const bool allowExperimentalTypes = true;
    TrackExceptionState exceptionState;
    IDBKey* expectedKey = createIDBKeyFromValueAndKeyPath(isolate, scriptValue.v8Value(), value->keyPath(), exceptionState, allowExperimentalTypes);
    ASSERT(!exceptionState.hadException());
    if (expectedKey && expectedKey->isEqual(value->primaryKey()))
        return;

    bool injected = injectV8KeyIntoV8Value(isolate, keyValue.v8Value(), scriptValue.v8Value(), value->keyPath());
    ASSERT_UNUSED(injected, injected);
}
#endif

} // namespace blink
