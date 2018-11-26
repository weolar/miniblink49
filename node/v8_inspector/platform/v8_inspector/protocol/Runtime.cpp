// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform\v8_inspector\protocol/Runtime.h"

#include "platform/inspector_protocol/DispatcherBase.h"
#include "platform/inspector_protocol/Parser.h"

namespace blink {
namespace protocol {
namespace Runtime {

// ------------- Enum values from types.

const char Metainfo::domainName[] = "Runtime";
const char Metainfo::commandPrefix[] = "Runtime.";

namespace UnserializableValueEnum {
const char* Infinity = "Infinity";
const char* NaN = "NaN";
const char* NegativeInfinity = "-Infinity";
const char* Negative0 = "-0";
} // namespace UnserializableValueEnum

const char* RemoteObject::TypeEnum::Object = "object";
const char* RemoteObject::TypeEnum::Function = "function";
const char* RemoteObject::TypeEnum::Undefined = "undefined";
const char* RemoteObject::TypeEnum::String = "string";
const char* RemoteObject::TypeEnum::Number = "number";
const char* RemoteObject::TypeEnum::Boolean = "boolean";
const char* RemoteObject::TypeEnum::Symbol = "symbol";

const char* RemoteObject::SubtypeEnum::Array = "array";
const char* RemoteObject::SubtypeEnum::Null = "null";
const char* RemoteObject::SubtypeEnum::Node = "node";
const char* RemoteObject::SubtypeEnum::Regexp = "regexp";
const char* RemoteObject::SubtypeEnum::Date = "date";
const char* RemoteObject::SubtypeEnum::Map = "map";
const char* RemoteObject::SubtypeEnum::Set = "set";
const char* RemoteObject::SubtypeEnum::Iterator = "iterator";
const char* RemoteObject::SubtypeEnum::Generator = "generator";
const char* RemoteObject::SubtypeEnum::Error = "error";
const char* RemoteObject::SubtypeEnum::Proxy = "proxy";
const char* RemoteObject::SubtypeEnum::Promise = "promise";
const char* RemoteObject::SubtypeEnum::Typedarray = "typedarray";

std::unique_ptr<RemoteObject> RemoteObject::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<RemoteObject> result(new RemoteObject());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* typeValue = object->get("type");
    errors->setName("type");
    result->m_type = ValueConversions<String16>::parse(typeValue, errors);
    protocol::Value* subtypeValue = object->get("subtype");
    if (subtypeValue) {
        errors->setName("subtype");
        result->m_subtype = ValueConversions<String16>::parse(subtypeValue, errors);
    }
    protocol::Value* classNameValue = object->get("className");
    if (classNameValue) {
        errors->setName("className");
        result->m_className = ValueConversions<String16>::parse(classNameValue, errors);
    }
    protocol::Value* valueValue = object->get("value");
    if (valueValue) {
        errors->setName("value");
        result->m_value = ValueConversions<protocol::Value>::parse(valueValue, errors);
    }
    protocol::Value* unserializableValueValue = object->get("unserializableValue");
    if (unserializableValueValue) {
        errors->setName("unserializableValue");
        result->m_unserializableValue = ValueConversions<String16>::parse(unserializableValueValue, errors);
    }
    protocol::Value* descriptionValue = object->get("description");
    if (descriptionValue) {
        errors->setName("description");
        result->m_description = ValueConversions<String16>::parse(descriptionValue, errors);
    }
    protocol::Value* objectIdValue = object->get("objectId");
    if (objectIdValue) {
        errors->setName("objectId");
        result->m_objectId = ValueConversions<String16>::parse(objectIdValue, errors);
    }
    protocol::Value* previewValue = object->get("preview");
    if (previewValue) {
        errors->setName("preview");
        result->m_preview = ValueConversions<protocol::Runtime::ObjectPreview>::parse(previewValue, errors);
    }
    protocol::Value* customPreviewValue = object->get("customPreview");
    if (customPreviewValue) {
        errors->setName("customPreview");
        result->m_customPreview = ValueConversions<protocol::Runtime::CustomPreview>::parse(customPreviewValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> RemoteObject::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("type", ValueConversions<String16>::serialize(m_type));
    if (m_subtype.isJust())
        result->setValue("subtype", ValueConversions<String16>::serialize(m_subtype.fromJust()));
    if (m_className.isJust())
        result->setValue("className", ValueConversions<String16>::serialize(m_className.fromJust()));
    if (m_value.isJust())
        result->setValue("value", ValueConversions<protocol::Value>::serialize(m_value.fromJust()));
    if (m_unserializableValue.isJust())
        result->setValue("unserializableValue", ValueConversions<String16>::serialize(m_unserializableValue.fromJust()));
    if (m_description.isJust())
        result->setValue("description", ValueConversions<String16>::serialize(m_description.fromJust()));
    if (m_objectId.isJust())
        result->setValue("objectId", ValueConversions<String16>::serialize(m_objectId.fromJust()));
    if (m_preview.isJust())
        result->setValue("preview", ValueConversions<protocol::Runtime::ObjectPreview>::serialize(m_preview.fromJust()));
    if (m_customPreview.isJust())
        result->setValue("customPreview", ValueConversions<protocol::Runtime::CustomPreview>::serialize(m_customPreview.fromJust()));
    return result;
}

std::unique_ptr<RemoteObject> RemoteObject::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

String16 RemoteObject::toJSONString() const
{
    return serialize()->toJSONString();
}

// static
std::unique_ptr<API::RemoteObject> API::RemoteObject::fromJSONString(const String16& json)
{
    ErrorSupport errors;
    std::unique_ptr<Value> value = parseJSON(json);
    if (!value)
        return nullptr;
    return protocol::Runtime::RemoteObject::parse(value.get(), &errors);
}

std::unique_ptr<CustomPreview> CustomPreview::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<CustomPreview> result(new CustomPreview());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* headerValue = object->get("header");
    errors->setName("header");
    result->m_header = ValueConversions<String16>::parse(headerValue, errors);
    protocol::Value* hasBodyValue = object->get("hasBody");
    errors->setName("hasBody");
    result->m_hasBody = ValueConversions<bool>::parse(hasBodyValue, errors);
    protocol::Value* formatterObjectIdValue = object->get("formatterObjectId");
    errors->setName("formatterObjectId");
    result->m_formatterObjectId = ValueConversions<String16>::parse(formatterObjectIdValue, errors);
    protocol::Value* bindRemoteObjectFunctionIdValue = object->get("bindRemoteObjectFunctionId");
    errors->setName("bindRemoteObjectFunctionId");
    result->m_bindRemoteObjectFunctionId = ValueConversions<String16>::parse(bindRemoteObjectFunctionIdValue, errors);
    protocol::Value* configObjectIdValue = object->get("configObjectId");
    if (configObjectIdValue) {
        errors->setName("configObjectId");
        result->m_configObjectId = ValueConversions<String16>::parse(configObjectIdValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> CustomPreview::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("header", ValueConversions<String16>::serialize(m_header));
    result->setValue("hasBody", ValueConversions<bool>::serialize(m_hasBody));
    result->setValue("formatterObjectId", ValueConversions<String16>::serialize(m_formatterObjectId));
    result->setValue("bindRemoteObjectFunctionId", ValueConversions<String16>::serialize(m_bindRemoteObjectFunctionId));
    if (m_configObjectId.isJust())
        result->setValue("configObjectId", ValueConversions<String16>::serialize(m_configObjectId.fromJust()));
    return result;
}

std::unique_ptr<CustomPreview> CustomPreview::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

const char* ObjectPreview::TypeEnum::Object = "object";
const char* ObjectPreview::TypeEnum::Function = "function";
const char* ObjectPreview::TypeEnum::Undefined = "undefined";
const char* ObjectPreview::TypeEnum::String = "string";
const char* ObjectPreview::TypeEnum::Number = "number";
const char* ObjectPreview::TypeEnum::Boolean = "boolean";
const char* ObjectPreview::TypeEnum::Symbol = "symbol";

const char* ObjectPreview::SubtypeEnum::Array = "array";
const char* ObjectPreview::SubtypeEnum::Null = "null";
const char* ObjectPreview::SubtypeEnum::Node = "node";
const char* ObjectPreview::SubtypeEnum::Regexp = "regexp";
const char* ObjectPreview::SubtypeEnum::Date = "date";
const char* ObjectPreview::SubtypeEnum::Map = "map";
const char* ObjectPreview::SubtypeEnum::Set = "set";
const char* ObjectPreview::SubtypeEnum::Iterator = "iterator";
const char* ObjectPreview::SubtypeEnum::Generator = "generator";
const char* ObjectPreview::SubtypeEnum::Error = "error";

std::unique_ptr<ObjectPreview> ObjectPreview::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<ObjectPreview> result(new ObjectPreview());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* typeValue = object->get("type");
    errors->setName("type");
    result->m_type = ValueConversions<String16>::parse(typeValue, errors);
    protocol::Value* subtypeValue = object->get("subtype");
    if (subtypeValue) {
        errors->setName("subtype");
        result->m_subtype = ValueConversions<String16>::parse(subtypeValue, errors);
    }
    protocol::Value* descriptionValue = object->get("description");
    if (descriptionValue) {
        errors->setName("description");
        result->m_description = ValueConversions<String16>::parse(descriptionValue, errors);
    }
    protocol::Value* overflowValue = object->get("overflow");
    errors->setName("overflow");
    result->m_overflow = ValueConversions<bool>::parse(overflowValue, errors);
    protocol::Value* propertiesValue = object->get("properties");
    errors->setName("properties");
    result->m_properties = ValueConversions<protocol::Array<protocol::Runtime::PropertyPreview>>::parse(propertiesValue, errors);
    protocol::Value* entriesValue = object->get("entries");
    if (entriesValue) {
        errors->setName("entries");
        result->m_entries = ValueConversions<protocol::Array<protocol::Runtime::EntryPreview>>::parse(entriesValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> ObjectPreview::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("type", ValueConversions<String16>::serialize(m_type));
    if (m_subtype.isJust())
        result->setValue("subtype", ValueConversions<String16>::serialize(m_subtype.fromJust()));
    if (m_description.isJust())
        result->setValue("description", ValueConversions<String16>::serialize(m_description.fromJust()));
    result->setValue("overflow", ValueConversions<bool>::serialize(m_overflow));
    result->setValue("properties", ValueConversions<protocol::Array<protocol::Runtime::PropertyPreview>>::serialize(m_properties.get()));
    if (m_entries.isJust())
        result->setValue("entries", ValueConversions<protocol::Array<protocol::Runtime::EntryPreview>>::serialize(m_entries.fromJust()));
    return result;
}

std::unique_ptr<ObjectPreview> ObjectPreview::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

const char* PropertyPreview::TypeEnum::Object = "object";
const char* PropertyPreview::TypeEnum::Function = "function";
const char* PropertyPreview::TypeEnum::Undefined = "undefined";
const char* PropertyPreview::TypeEnum::String = "string";
const char* PropertyPreview::TypeEnum::Number = "number";
const char* PropertyPreview::TypeEnum::Boolean = "boolean";
const char* PropertyPreview::TypeEnum::Symbol = "symbol";
const char* PropertyPreview::TypeEnum::Accessor = "accessor";

const char* PropertyPreview::SubtypeEnum::Array = "array";
const char* PropertyPreview::SubtypeEnum::Null = "null";
const char* PropertyPreview::SubtypeEnum::Node = "node";
const char* PropertyPreview::SubtypeEnum::Regexp = "regexp";
const char* PropertyPreview::SubtypeEnum::Date = "date";
const char* PropertyPreview::SubtypeEnum::Map = "map";
const char* PropertyPreview::SubtypeEnum::Set = "set";
const char* PropertyPreview::SubtypeEnum::Iterator = "iterator";
const char* PropertyPreview::SubtypeEnum::Generator = "generator";
const char* PropertyPreview::SubtypeEnum::Error = "error";

std::unique_ptr<PropertyPreview> PropertyPreview::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<PropertyPreview> result(new PropertyPreview());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* nameValue = object->get("name");
    errors->setName("name");
    result->m_name = ValueConversions<String16>::parse(nameValue, errors);
    protocol::Value* typeValue = object->get("type");
    errors->setName("type");
    result->m_type = ValueConversions<String16>::parse(typeValue, errors);
    protocol::Value* valueValue = object->get("value");
    if (valueValue) {
        errors->setName("value");
        result->m_value = ValueConversions<String16>::parse(valueValue, errors);
    }
    protocol::Value* valuePreviewValue = object->get("valuePreview");
    if (valuePreviewValue) {
        errors->setName("valuePreview");
        result->m_valuePreview = ValueConversions<protocol::Runtime::ObjectPreview>::parse(valuePreviewValue, errors);
    }
    protocol::Value* subtypeValue = object->get("subtype");
    if (subtypeValue) {
        errors->setName("subtype");
        result->m_subtype = ValueConversions<String16>::parse(subtypeValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> PropertyPreview::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("name", ValueConversions<String16>::serialize(m_name));
    result->setValue("type", ValueConversions<String16>::serialize(m_type));
    if (m_value.isJust())
        result->setValue("value", ValueConversions<String16>::serialize(m_value.fromJust()));
    if (m_valuePreview.isJust())
        result->setValue("valuePreview", ValueConversions<protocol::Runtime::ObjectPreview>::serialize(m_valuePreview.fromJust()));
    if (m_subtype.isJust())
        result->setValue("subtype", ValueConversions<String16>::serialize(m_subtype.fromJust()));
    return result;
}

std::unique_ptr<PropertyPreview> PropertyPreview::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<EntryPreview> EntryPreview::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<EntryPreview> result(new EntryPreview());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* keyValue = object->get("key");
    if (keyValue) {
        errors->setName("key");
        result->m_key = ValueConversions<protocol::Runtime::ObjectPreview>::parse(keyValue, errors);
    }
    protocol::Value* valueValue = object->get("value");
    errors->setName("value");
    result->m_value = ValueConversions<protocol::Runtime::ObjectPreview>::parse(valueValue, errors);
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> EntryPreview::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    if (m_key.isJust())
        result->setValue("key", ValueConversions<protocol::Runtime::ObjectPreview>::serialize(m_key.fromJust()));
    result->setValue("value", ValueConversions<protocol::Runtime::ObjectPreview>::serialize(m_value.get()));
    return result;
}

std::unique_ptr<EntryPreview> EntryPreview::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<PropertyDescriptor> PropertyDescriptor::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<PropertyDescriptor> result(new PropertyDescriptor());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* nameValue = object->get("name");
    errors->setName("name");
    result->m_name = ValueConversions<String16>::parse(nameValue, errors);
    protocol::Value* valueValue = object->get("value");
    if (valueValue) {
        errors->setName("value");
        result->m_value = ValueConversions<protocol::Runtime::RemoteObject>::parse(valueValue, errors);
    }
    protocol::Value* writableValue = object->get("writable");
    if (writableValue) {
        errors->setName("writable");
        result->m_writable = ValueConversions<bool>::parse(writableValue, errors);
    }
    protocol::Value* getValue = object->get("get");
    if (getValue) {
        errors->setName("get");
        result->m_get = ValueConversions<protocol::Runtime::RemoteObject>::parse(getValue, errors);
    }
    protocol::Value* setValue = object->get("set");
    if (setValue) {
        errors->setName("set");
        result->m_set = ValueConversions<protocol::Runtime::RemoteObject>::parse(setValue, errors);
    }
    protocol::Value* configurableValue = object->get("configurable");
    errors->setName("configurable");
    result->m_configurable = ValueConversions<bool>::parse(configurableValue, errors);
    protocol::Value* enumerableValue = object->get("enumerable");
    errors->setName("enumerable");
    result->m_enumerable = ValueConversions<bool>::parse(enumerableValue, errors);
    protocol::Value* wasThrownValue = object->get("wasThrown");
    if (wasThrownValue) {
        errors->setName("wasThrown");
        result->m_wasThrown = ValueConversions<bool>::parse(wasThrownValue, errors);
    }
    protocol::Value* isOwnValue = object->get("isOwn");
    if (isOwnValue) {
        errors->setName("isOwn");
        result->m_isOwn = ValueConversions<bool>::parse(isOwnValue, errors);
    }
    protocol::Value* symbolValue = object->get("symbol");
    if (symbolValue) {
        errors->setName("symbol");
        result->m_symbol = ValueConversions<protocol::Runtime::RemoteObject>::parse(symbolValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> PropertyDescriptor::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("name", ValueConversions<String16>::serialize(m_name));
    if (m_value.isJust())
        result->setValue("value", ValueConversions<protocol::Runtime::RemoteObject>::serialize(m_value.fromJust()));
    if (m_writable.isJust())
        result->setValue("writable", ValueConversions<bool>::serialize(m_writable.fromJust()));
    if (m_get.isJust())
        result->setValue("get", ValueConversions<protocol::Runtime::RemoteObject>::serialize(m_get.fromJust()));
    if (m_set.isJust())
        result->setValue("set", ValueConversions<protocol::Runtime::RemoteObject>::serialize(m_set.fromJust()));
    result->setValue("configurable", ValueConversions<bool>::serialize(m_configurable));
    result->setValue("enumerable", ValueConversions<bool>::serialize(m_enumerable));
    if (m_wasThrown.isJust())
        result->setValue("wasThrown", ValueConversions<bool>::serialize(m_wasThrown.fromJust()));
    if (m_isOwn.isJust())
        result->setValue("isOwn", ValueConversions<bool>::serialize(m_isOwn.fromJust()));
    if (m_symbol.isJust())
        result->setValue("symbol", ValueConversions<protocol::Runtime::RemoteObject>::serialize(m_symbol.fromJust()));
    return result;
}

std::unique_ptr<PropertyDescriptor> PropertyDescriptor::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<InternalPropertyDescriptor> InternalPropertyDescriptor::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<InternalPropertyDescriptor> result(new InternalPropertyDescriptor());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* nameValue = object->get("name");
    errors->setName("name");
    result->m_name = ValueConversions<String16>::parse(nameValue, errors);
    protocol::Value* valueValue = object->get("value");
    if (valueValue) {
        errors->setName("value");
        result->m_value = ValueConversions<protocol::Runtime::RemoteObject>::parse(valueValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> InternalPropertyDescriptor::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("name", ValueConversions<String16>::serialize(m_name));
    if (m_value.isJust())
        result->setValue("value", ValueConversions<protocol::Runtime::RemoteObject>::serialize(m_value.fromJust()));
    return result;
}

std::unique_ptr<InternalPropertyDescriptor> InternalPropertyDescriptor::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<CallArgument> CallArgument::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<CallArgument> result(new CallArgument());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* valueValue = object->get("value");
    if (valueValue) {
        errors->setName("value");
        result->m_value = ValueConversions<protocol::Value>::parse(valueValue, errors);
    }
    protocol::Value* unserializableValueValue = object->get("unserializableValue");
    if (unserializableValueValue) {
        errors->setName("unserializableValue");
        result->m_unserializableValue = ValueConversions<String16>::parse(unserializableValueValue, errors);
    }
    protocol::Value* objectIdValue = object->get("objectId");
    if (objectIdValue) {
        errors->setName("objectId");
        result->m_objectId = ValueConversions<String16>::parse(objectIdValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> CallArgument::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    if (m_value.isJust())
        result->setValue("value", ValueConversions<protocol::Value>::serialize(m_value.fromJust()));
    if (m_unserializableValue.isJust())
        result->setValue("unserializableValue", ValueConversions<String16>::serialize(m_unserializableValue.fromJust()));
    if (m_objectId.isJust())
        result->setValue("objectId", ValueConversions<String16>::serialize(m_objectId.fromJust()));
    return result;
}

std::unique_ptr<CallArgument> CallArgument::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<ExecutionContextDescription> ExecutionContextDescription::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<ExecutionContextDescription> result(new ExecutionContextDescription());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* idValue = object->get("id");
    errors->setName("id");
    result->m_id = ValueConversions<int>::parse(idValue, errors);
    protocol::Value* originValue = object->get("origin");
    errors->setName("origin");
    result->m_origin = ValueConversions<String16>::parse(originValue, errors);
    protocol::Value* nameValue = object->get("name");
    errors->setName("name");
    result->m_name = ValueConversions<String16>::parse(nameValue, errors);
    protocol::Value* auxDataValue = object->get("auxData");
    if (auxDataValue) {
        errors->setName("auxData");
        result->m_auxData = ValueConversions<protocol::DictionaryValue>::parse(auxDataValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> ExecutionContextDescription::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("id", ValueConversions<int>::serialize(m_id));
    result->setValue("origin", ValueConversions<String16>::serialize(m_origin));
    result->setValue("name", ValueConversions<String16>::serialize(m_name));
    if (m_auxData.isJust())
        result->setValue("auxData", ValueConversions<protocol::DictionaryValue>::serialize(m_auxData.fromJust()));
    return result;
}

std::unique_ptr<ExecutionContextDescription> ExecutionContextDescription::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<ExceptionDetails> ExceptionDetails::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<ExceptionDetails> result(new ExceptionDetails());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* textValue = object->get("text");
    errors->setName("text");
    result->m_text = ValueConversions<String16>::parse(textValue, errors);
    protocol::Value* scriptIdValue = object->get("scriptId");
    errors->setName("scriptId");
    result->m_scriptId = ValueConversions<String16>::parse(scriptIdValue, errors);
    protocol::Value* lineNumberValue = object->get("lineNumber");
    errors->setName("lineNumber");
    result->m_lineNumber = ValueConversions<int>::parse(lineNumberValue, errors);
    protocol::Value* columnNumberValue = object->get("columnNumber");
    errors->setName("columnNumber");
    result->m_columnNumber = ValueConversions<int>::parse(columnNumberValue, errors);
    protocol::Value* urlValue = object->get("url");
    if (urlValue) {
        errors->setName("url");
        result->m_url = ValueConversions<String16>::parse(urlValue, errors);
    }
    protocol::Value* stackTraceValue = object->get("stackTrace");
    if (stackTraceValue) {
        errors->setName("stackTrace");
        result->m_stackTrace = ValueConversions<protocol::Runtime::StackTrace>::parse(stackTraceValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> ExceptionDetails::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("text", ValueConversions<String16>::serialize(m_text));
    result->setValue("scriptId", ValueConversions<String16>::serialize(m_scriptId));
    result->setValue("lineNumber", ValueConversions<int>::serialize(m_lineNumber));
    result->setValue("columnNumber", ValueConversions<int>::serialize(m_columnNumber));
    if (m_url.isJust())
        result->setValue("url", ValueConversions<String16>::serialize(m_url.fromJust()));
    if (m_stackTrace.isJust())
        result->setValue("stackTrace", ValueConversions<protocol::Runtime::StackTrace>::serialize(m_stackTrace.fromJust()));
    return result;
}

std::unique_ptr<ExceptionDetails> ExceptionDetails::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<CallFrame> CallFrame::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<CallFrame> result(new CallFrame());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* functionNameValue = object->get("functionName");
    errors->setName("functionName");
    result->m_functionName = ValueConversions<String16>::parse(functionNameValue, errors);
    protocol::Value* scriptIdValue = object->get("scriptId");
    errors->setName("scriptId");
    result->m_scriptId = ValueConversions<String16>::parse(scriptIdValue, errors);
    protocol::Value* urlValue = object->get("url");
    errors->setName("url");
    result->m_url = ValueConversions<String16>::parse(urlValue, errors);
    protocol::Value* lineNumberValue = object->get("lineNumber");
    errors->setName("lineNumber");
    result->m_lineNumber = ValueConversions<int>::parse(lineNumberValue, errors);
    protocol::Value* columnNumberValue = object->get("columnNumber");
    errors->setName("columnNumber");
    result->m_columnNumber = ValueConversions<int>::parse(columnNumberValue, errors);
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> CallFrame::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("functionName", ValueConversions<String16>::serialize(m_functionName));
    result->setValue("scriptId", ValueConversions<String16>::serialize(m_scriptId));
    result->setValue("url", ValueConversions<String16>::serialize(m_url));
    result->setValue("lineNumber", ValueConversions<int>::serialize(m_lineNumber));
    result->setValue("columnNumber", ValueConversions<int>::serialize(m_columnNumber));
    return result;
}

std::unique_ptr<CallFrame> CallFrame::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<StackTrace> StackTrace::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<StackTrace> result(new StackTrace());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* descriptionValue = object->get("description");
    if (descriptionValue) {
        errors->setName("description");
        result->m_description = ValueConversions<String16>::parse(descriptionValue, errors);
    }
    protocol::Value* callFramesValue = object->get("callFrames");
    errors->setName("callFrames");
    result->m_callFrames = ValueConversions<protocol::Array<protocol::Runtime::CallFrame>>::parse(callFramesValue, errors);
    protocol::Value* parentValue = object->get("parent");
    if (parentValue) {
        errors->setName("parent");
        result->m_parent = ValueConversions<protocol::Runtime::StackTrace>::parse(parentValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> StackTrace::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    if (m_description.isJust())
        result->setValue("description", ValueConversions<String16>::serialize(m_description.fromJust()));
    result->setValue("callFrames", ValueConversions<protocol::Array<protocol::Runtime::CallFrame>>::serialize(m_callFrames.get()));
    if (m_parent.isJust())
        result->setValue("parent", ValueConversions<protocol::Runtime::StackTrace>::serialize(m_parent.fromJust()));
    return result;
}

std::unique_ptr<StackTrace> StackTrace::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

String16 StackTrace::toJSONString() const
{
    return serialize()->toJSONString();
}

// static
std::unique_ptr<API::StackTrace> API::StackTrace::fromJSONString(const String16& json)
{
    ErrorSupport errors;
    std::unique_ptr<Value> value = parseJSON(json);
    if (!value)
        return nullptr;
    return protocol::Runtime::StackTrace::parse(value.get(), &errors);
}

// ------------- Enum values from params.


namespace ConsoleAPICalled {
namespace TypeEnum {
const char* Log = "log";
const char* Debug = "debug";
const char* Info = "info";
const char* Error = "error";
const char* Warning = "warning";
const char* Dir = "dir";
const char* Dirxml = "dirxml";
const char* Table = "table";
const char* Trace = "trace";
const char* Clear = "clear";
const char* StartGroup = "startGroup";
const char* StartGroupCollapsed = "startGroupCollapsed";
const char* EndGroup = "endGroup";
const char* Assert = "assert";
const char* Profile = "profile";
const char* ProfileEnd = "profileEnd";
} // namespace TypeEnum
} // namespace ConsoleAPICalled

// ------------- Frontend notifications.

void Frontend::executionContextCreated(std::unique_ptr<protocol::Runtime::ExecutionContextDescription> context)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Runtime.executionContextCreated");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("context", ValueConversions<protocol::Runtime::ExecutionContextDescription>::serialize(context.get()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::executionContextDestroyed(int executionContextId)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Runtime.executionContextDestroyed");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("executionContextId", ValueConversions<int>::serialize(executionContextId));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::executionContextsCleared()
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Runtime.executionContextsCleared");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::exceptionThrown(int exceptionId, double timestamp, std::unique_ptr<protocol::Runtime::ExceptionDetails> details, const Maybe<protocol::Runtime::RemoteObject>& exception, const Maybe<int>& executionContextId)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Runtime.exceptionThrown");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("exceptionId", ValueConversions<int>::serialize(exceptionId));
    paramsObject->setValue("timestamp", ValueConversions<double>::serialize(timestamp));
    paramsObject->setValue("details", ValueConversions<protocol::Runtime::ExceptionDetails>::serialize(details.get()));
    if (exception.isJust())
        paramsObject->setValue("exception", ValueConversions<protocol::Runtime::RemoteObject>::serialize(exception.fromJust()));
    if (executionContextId.isJust())
        paramsObject->setValue("executionContextId", ValueConversions<int>::serialize(executionContextId.fromJust()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::exceptionRevoked(const String16& message, int exceptionId)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Runtime.exceptionRevoked");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("message", ValueConversions<String16>::serialize(message));
    paramsObject->setValue("exceptionId", ValueConversions<int>::serialize(exceptionId));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::consoleAPICalled(const String16& type, std::unique_ptr<protocol::Array<protocol::Runtime::RemoteObject>> args, int executionContextId, double timestamp, const Maybe<protocol::Runtime::StackTrace>& stackTrace)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Runtime.consoleAPICalled");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("type", ValueConversions<String16>::serialize(type));
    paramsObject->setValue("args", ValueConversions<protocol::Array<protocol::Runtime::RemoteObject>>::serialize(args.get()));
    paramsObject->setValue("executionContextId", ValueConversions<int>::serialize(executionContextId));
    paramsObject->setValue("timestamp", ValueConversions<double>::serialize(timestamp));
    if (stackTrace.isJust())
        paramsObject->setValue("stackTrace", ValueConversions<protocol::Runtime::StackTrace>::serialize(stackTrace.fromJust()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::inspectRequested(std::unique_ptr<protocol::Runtime::RemoteObject> object, std::unique_ptr<protocol::DictionaryValue> hints)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Runtime.inspectRequested");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("object", ValueConversions<protocol::Runtime::RemoteObject>::serialize(object.get()));
    paramsObject->setValue("hints", ValueConversions<protocol::DictionaryValue>::serialize(hints.get()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

// --------------------- Dispatcher.

class DispatcherImpl : public protocol::DispatcherBase {
public:
    DispatcherImpl(FrontendChannel* frontendChannel, Backend* backend)
        : DispatcherBase(frontendChannel)
        , m_backend(backend) {
        m_dispatchMap["Runtime.evaluate"] = &DispatcherImpl::evaluate;
        m_dispatchMap["Runtime.awaitPromise"] = &DispatcherImpl::awaitPromise;
        m_dispatchMap["Runtime.callFunctionOn"] = &DispatcherImpl::callFunctionOn;
        m_dispatchMap["Runtime.getProperties"] = &DispatcherImpl::getProperties;
        m_dispatchMap["Runtime.releaseObject"] = &DispatcherImpl::releaseObject;
        m_dispatchMap["Runtime.releaseObjectGroup"] = &DispatcherImpl::releaseObjectGroup;
        m_dispatchMap["Runtime.run"] = &DispatcherImpl::run;
        m_dispatchMap["Runtime.enable"] = &DispatcherImpl::enable;
        m_dispatchMap["Runtime.disable"] = &DispatcherImpl::disable;
        m_dispatchMap["Runtime.discardConsoleEntries"] = &DispatcherImpl::discardConsoleEntries;
        m_dispatchMap["Runtime.setCustomObjectFormatterEnabled"] = &DispatcherImpl::setCustomObjectFormatterEnabled;
        m_dispatchMap["Runtime.compileScript"] = &DispatcherImpl::compileScript;
        m_dispatchMap["Runtime.runScript"] = &DispatcherImpl::runScript;
    }
    ~DispatcherImpl() override { }
    void dispatch(int callId, const String16& method, std::unique_ptr<protocol::DictionaryValue> messageObject) override;

protected:
    using CallHandler = void (DispatcherImpl::*)(int callId, std::unique_ptr<DictionaryValue> messageObject, ErrorSupport* errors);
    using DispatchMap = protocol::HashMap<String16, CallHandler>;
    DispatchMap m_dispatchMap;

    void evaluate(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void awaitPromise(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void callFunctionOn(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void getProperties(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void releaseObject(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void releaseObjectGroup(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void run(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void enable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void disable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void discardConsoleEntries(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setCustomObjectFormatterEnabled(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void compileScript(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void runScript(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);

    Backend* m_backend;
};

void DispatcherImpl::dispatch(int callId, const String16& method, std::unique_ptr<protocol::DictionaryValue> messageObject)
{
    protocol::HashMap<String16, CallHandler>::iterator it = m_dispatchMap.find(method);
    if (it == m_dispatchMap.end()) {
        reportProtocolError(callId, MethodNotFound, "'" + method + "' wasn't found", nullptr);
        return;
    }

    protocol::ErrorSupport errors;
    (this->*(it->second))(callId, std::move(messageObject), &errors);
}


class EvaluateCallbackImpl : public Backend::EvaluateCallback, public DispatcherBase::Callback {
public:
    EvaluateCallbackImpl(std::unique_ptr<DispatcherBase::WeakPtr> backendImpl, int callId)
        : DispatcherBase::Callback(std::move(backendImpl), callId) { }

    void sendSuccess(std::unique_ptr<protocol::Runtime::RemoteObject> result, const Maybe<bool>& wasThrown, const Maybe<protocol::Runtime::ExceptionDetails>& exceptionDetails) override
    {
        std::unique_ptr<protocol::DictionaryValue> resultObject = DictionaryValue::create();
        resultObject->setValue("result", ValueConversions<protocol::Runtime::RemoteObject>::serialize(result.get()));
        if (wasThrown.isJust())
            resultObject->setValue("wasThrown", ValueConversions<bool>::serialize(wasThrown.fromJust()));
        if (exceptionDetails.isJust())
            resultObject->setValue("exceptionDetails", ValueConversions<protocol::Runtime::ExceptionDetails>::serialize(exceptionDetails.fromJust()));
        sendIfActive(std::move(resultObject), ErrorString());
    }

    void sendFailure(const ErrorString& error) override
    {
        DCHECK(error.length());
        sendIfActive(nullptr, error);
    }

};

void DispatcherImpl::evaluate(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* expressionValue = object ? object->get("expression") : nullptr;
    errors->setName("expression");
    String16 in_expression = ValueConversions<String16>::parse(expressionValue, errors);
    protocol::Value* objectGroupValue = object ? object->get("objectGroup") : nullptr;
    Maybe<String16> in_objectGroup;
    if (objectGroupValue) {
        errors->setName("objectGroup");
        in_objectGroup = ValueConversions<String16>::parse(objectGroupValue, errors);
    }
    protocol::Value* includeCommandLineAPIValue = object ? object->get("includeCommandLineAPI") : nullptr;
    Maybe<bool> in_includeCommandLineAPI;
    if (includeCommandLineAPIValue) {
        errors->setName("includeCommandLineAPI");
        in_includeCommandLineAPI = ValueConversions<bool>::parse(includeCommandLineAPIValue, errors);
    }
    protocol::Value* doNotPauseOnExceptionsAndMuteConsoleValue = object ? object->get("doNotPauseOnExceptionsAndMuteConsole") : nullptr;
    Maybe<bool> in_doNotPauseOnExceptionsAndMuteConsole;
    if (doNotPauseOnExceptionsAndMuteConsoleValue) {
        errors->setName("doNotPauseOnExceptionsAndMuteConsole");
        in_doNotPauseOnExceptionsAndMuteConsole = ValueConversions<bool>::parse(doNotPauseOnExceptionsAndMuteConsoleValue, errors);
    }
    protocol::Value* contextIdValue = object ? object->get("contextId") : nullptr;
    Maybe<int> in_contextId;
    if (contextIdValue) {
        errors->setName("contextId");
        in_contextId = ValueConversions<int>::parse(contextIdValue, errors);
    }
    protocol::Value* returnByValueValue = object ? object->get("returnByValue") : nullptr;
    Maybe<bool> in_returnByValue;
    if (returnByValueValue) {
        errors->setName("returnByValue");
        in_returnByValue = ValueConversions<bool>::parse(returnByValueValue, errors);
    }
    protocol::Value* generatePreviewValue = object ? object->get("generatePreview") : nullptr;
    Maybe<bool> in_generatePreview;
    if (generatePreviewValue) {
        errors->setName("generatePreview");
        in_generatePreview = ValueConversions<bool>::parse(generatePreviewValue, errors);
    }
    protocol::Value* userGestureValue = object ? object->get("userGesture") : nullptr;
    Maybe<bool> in_userGesture;
    if (userGestureValue) {
        errors->setName("userGesture");
        in_userGesture = ValueConversions<bool>::parse(userGestureValue, errors);
    }
    protocol::Value* awaitPromiseValue = object ? object->get("awaitPromise") : nullptr;
    Maybe<bool> in_awaitPromise;
    if (awaitPromiseValue) {
        errors->setName("awaitPromise");
        in_awaitPromise = ValueConversions<bool>::parse(awaitPromiseValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    std::unique_ptr<EvaluateCallbackImpl> callback(new EvaluateCallbackImpl(weakPtr(), callId));

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    m_backend->evaluate(in_expression,in_objectGroup,in_includeCommandLineAPI,in_doNotPauseOnExceptionsAndMuteConsole,in_contextId,in_returnByValue,in_generatePreview,in_userGesture,in_awaitPromise,std::move(callback));
}

class AwaitPromiseCallbackImpl : public Backend::AwaitPromiseCallback, public DispatcherBase::Callback {
public:
    AwaitPromiseCallbackImpl(std::unique_ptr<DispatcherBase::WeakPtr> backendImpl, int callId)
        : DispatcherBase::Callback(std::move(backendImpl), callId) { }

    void sendSuccess(std::unique_ptr<protocol::Runtime::RemoteObject> result, const Maybe<bool>& wasThrown, const Maybe<protocol::Runtime::ExceptionDetails>& exceptionDetails) override
    {
        std::unique_ptr<protocol::DictionaryValue> resultObject = DictionaryValue::create();
        resultObject->setValue("result", ValueConversions<protocol::Runtime::RemoteObject>::serialize(result.get()));
        if (wasThrown.isJust())
            resultObject->setValue("wasThrown", ValueConversions<bool>::serialize(wasThrown.fromJust()));
        if (exceptionDetails.isJust())
            resultObject->setValue("exceptionDetails", ValueConversions<protocol::Runtime::ExceptionDetails>::serialize(exceptionDetails.fromJust()));
        sendIfActive(std::move(resultObject), ErrorString());
    }

    void sendFailure(const ErrorString& error) override
    {
        DCHECK(error.length());
        sendIfActive(nullptr, error);
    }

};

void DispatcherImpl::awaitPromise(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* promiseObjectIdValue = object ? object->get("promiseObjectId") : nullptr;
    errors->setName("promiseObjectId");
    String16 in_promiseObjectId = ValueConversions<String16>::parse(promiseObjectIdValue, errors);
    protocol::Value* returnByValueValue = object ? object->get("returnByValue") : nullptr;
    Maybe<bool> in_returnByValue;
    if (returnByValueValue) {
        errors->setName("returnByValue");
        in_returnByValue = ValueConversions<bool>::parse(returnByValueValue, errors);
    }
    protocol::Value* generatePreviewValue = object ? object->get("generatePreview") : nullptr;
    Maybe<bool> in_generatePreview;
    if (generatePreviewValue) {
        errors->setName("generatePreview");
        in_generatePreview = ValueConversions<bool>::parse(generatePreviewValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    std::unique_ptr<AwaitPromiseCallbackImpl> callback(new AwaitPromiseCallbackImpl(weakPtr(), callId));

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    m_backend->awaitPromise(in_promiseObjectId,in_returnByValue,in_generatePreview,std::move(callback));
}

class CallFunctionOnCallbackImpl : public Backend::CallFunctionOnCallback, public DispatcherBase::Callback {
public:
    CallFunctionOnCallbackImpl(std::unique_ptr<DispatcherBase::WeakPtr> backendImpl, int callId)
        : DispatcherBase::Callback(std::move(backendImpl), callId) { }

    void sendSuccess(std::unique_ptr<protocol::Runtime::RemoteObject> result, const Maybe<bool>& wasThrown, const Maybe<protocol::Runtime::ExceptionDetails>& exceptionDetails) override
    {
        std::unique_ptr<protocol::DictionaryValue> resultObject = DictionaryValue::create();
        resultObject->setValue("result", ValueConversions<protocol::Runtime::RemoteObject>::serialize(result.get()));
        if (wasThrown.isJust())
            resultObject->setValue("wasThrown", ValueConversions<bool>::serialize(wasThrown.fromJust()));
        if (exceptionDetails.isJust())
            resultObject->setValue("exceptionDetails", ValueConversions<protocol::Runtime::ExceptionDetails>::serialize(exceptionDetails.fromJust()));
        sendIfActive(std::move(resultObject), ErrorString());
    }

    void sendFailure(const ErrorString& error) override
    {
        DCHECK(error.length());
        sendIfActive(nullptr, error);
    }

};

void DispatcherImpl::callFunctionOn(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* objectIdValue = object ? object->get("objectId") : nullptr;
    errors->setName("objectId");
    String16 in_objectId = ValueConversions<String16>::parse(objectIdValue, errors);
    protocol::Value* functionDeclarationValue = object ? object->get("functionDeclaration") : nullptr;
    errors->setName("functionDeclaration");
    String16 in_functionDeclaration = ValueConversions<String16>::parse(functionDeclarationValue, errors);
    protocol::Value* argumentsValue = object ? object->get("arguments") : nullptr;
    Maybe<protocol::Array<protocol::Runtime::CallArgument>> in_arguments;
    if (argumentsValue) {
        errors->setName("arguments");
        in_arguments = ValueConversions<protocol::Array<protocol::Runtime::CallArgument>>::parse(argumentsValue, errors);
    }
    protocol::Value* doNotPauseOnExceptionsAndMuteConsoleValue = object ? object->get("doNotPauseOnExceptionsAndMuteConsole") : nullptr;
    Maybe<bool> in_doNotPauseOnExceptionsAndMuteConsole;
    if (doNotPauseOnExceptionsAndMuteConsoleValue) {
        errors->setName("doNotPauseOnExceptionsAndMuteConsole");
        in_doNotPauseOnExceptionsAndMuteConsole = ValueConversions<bool>::parse(doNotPauseOnExceptionsAndMuteConsoleValue, errors);
    }
    protocol::Value* returnByValueValue = object ? object->get("returnByValue") : nullptr;
    Maybe<bool> in_returnByValue;
    if (returnByValueValue) {
        errors->setName("returnByValue");
        in_returnByValue = ValueConversions<bool>::parse(returnByValueValue, errors);
    }
    protocol::Value* generatePreviewValue = object ? object->get("generatePreview") : nullptr;
    Maybe<bool> in_generatePreview;
    if (generatePreviewValue) {
        errors->setName("generatePreview");
        in_generatePreview = ValueConversions<bool>::parse(generatePreviewValue, errors);
    }
    protocol::Value* userGestureValue = object ? object->get("userGesture") : nullptr;
    Maybe<bool> in_userGesture;
    if (userGestureValue) {
        errors->setName("userGesture");
        in_userGesture = ValueConversions<bool>::parse(userGestureValue, errors);
    }
    protocol::Value* awaitPromiseValue = object ? object->get("awaitPromise") : nullptr;
    Maybe<bool> in_awaitPromise;
    if (awaitPromiseValue) {
        errors->setName("awaitPromise");
        in_awaitPromise = ValueConversions<bool>::parse(awaitPromiseValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    std::unique_ptr<CallFunctionOnCallbackImpl> callback(new CallFunctionOnCallbackImpl(weakPtr(), callId));

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    m_backend->callFunctionOn(in_objectId,in_functionDeclaration,in_arguments,in_doNotPauseOnExceptionsAndMuteConsole,in_returnByValue,in_generatePreview,in_userGesture,in_awaitPromise,std::move(callback));
}

void DispatcherImpl::getProperties(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* objectIdValue = object ? object->get("objectId") : nullptr;
    errors->setName("objectId");
    String16 in_objectId = ValueConversions<String16>::parse(objectIdValue, errors);
    protocol::Value* ownPropertiesValue = object ? object->get("ownProperties") : nullptr;
    Maybe<bool> in_ownProperties;
    if (ownPropertiesValue) {
        errors->setName("ownProperties");
        in_ownProperties = ValueConversions<bool>::parse(ownPropertiesValue, errors);
    }
    protocol::Value* accessorPropertiesOnlyValue = object ? object->get("accessorPropertiesOnly") : nullptr;
    Maybe<bool> in_accessorPropertiesOnly;
    if (accessorPropertiesOnlyValue) {
        errors->setName("accessorPropertiesOnly");
        in_accessorPropertiesOnly = ValueConversions<bool>::parse(accessorPropertiesOnlyValue, errors);
    }
    protocol::Value* generatePreviewValue = object ? object->get("generatePreview") : nullptr;
    Maybe<bool> in_generatePreview;
    if (generatePreviewValue) {
        errors->setName("generatePreview");
        in_generatePreview = ValueConversions<bool>::parse(generatePreviewValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    std::unique_ptr<protocol::Array<protocol::Runtime::PropertyDescriptor>> out_result;
    Maybe<protocol::Array<protocol::Runtime::InternalPropertyDescriptor>> out_internalProperties;
    Maybe<protocol::Runtime::ExceptionDetails> out_exceptionDetails;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->getProperties(&error, in_objectId, in_ownProperties, in_accessorPropertiesOnly, in_generatePreview, &out_result, &out_internalProperties, &out_exceptionDetails);
    if (!error.length()) {
        result->setValue("result", ValueConversions<protocol::Array<protocol::Runtime::PropertyDescriptor>>::serialize(out_result.get()));
        if (out_internalProperties.isJust())
            result->setValue("internalProperties", ValueConversions<protocol::Array<protocol::Runtime::InternalPropertyDescriptor>>::serialize(out_internalProperties.fromJust()));
        if (out_exceptionDetails.isJust())
            result->setValue("exceptionDetails", ValueConversions<protocol::Runtime::ExceptionDetails>::serialize(out_exceptionDetails.fromJust()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::releaseObject(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* objectIdValue = object ? object->get("objectId") : nullptr;
    errors->setName("objectId");
    String16 in_objectId = ValueConversions<String16>::parse(objectIdValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->releaseObject(&error, in_objectId);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::releaseObjectGroup(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* objectGroupValue = object ? object->get("objectGroup") : nullptr;
    errors->setName("objectGroup");
    String16 in_objectGroup = ValueConversions<String16>::parse(objectGroupValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->releaseObjectGroup(&error, in_objectGroup);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::run(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->run(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::enable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->enable(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::disable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->disable(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::discardConsoleEntries(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->discardConsoleEntries(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::setCustomObjectFormatterEnabled(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* enabledValue = object ? object->get("enabled") : nullptr;
    errors->setName("enabled");
    bool in_enabled = ValueConversions<bool>::parse(enabledValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setCustomObjectFormatterEnabled(&error, in_enabled);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::compileScript(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* expressionValue = object ? object->get("expression") : nullptr;
    errors->setName("expression");
    String16 in_expression = ValueConversions<String16>::parse(expressionValue, errors);
    protocol::Value* sourceURLValue = object ? object->get("sourceURL") : nullptr;
    errors->setName("sourceURL");
    String16 in_sourceURL = ValueConversions<String16>::parse(sourceURLValue, errors);
    protocol::Value* persistScriptValue = object ? object->get("persistScript") : nullptr;
    errors->setName("persistScript");
    bool in_persistScript = ValueConversions<bool>::parse(persistScriptValue, errors);
    protocol::Value* executionContextIdValue = object ? object->get("executionContextId") : nullptr;
    Maybe<int> in_executionContextId;
    if (executionContextIdValue) {
        errors->setName("executionContextId");
        in_executionContextId = ValueConversions<int>::parse(executionContextIdValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    Maybe<String16> out_scriptId;
    Maybe<protocol::Runtime::ExceptionDetails> out_exceptionDetails;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->compileScript(&error, in_expression, in_sourceURL, in_persistScript, in_executionContextId, &out_scriptId, &out_exceptionDetails);
    if (!error.length()) {
        if (out_scriptId.isJust())
            result->setValue("scriptId", ValueConversions<String16>::serialize(out_scriptId.fromJust()));
        if (out_exceptionDetails.isJust())
            result->setValue("exceptionDetails", ValueConversions<protocol::Runtime::ExceptionDetails>::serialize(out_exceptionDetails.fromJust()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

class RunScriptCallbackImpl : public Backend::RunScriptCallback, public DispatcherBase::Callback {
public:
    RunScriptCallbackImpl(std::unique_ptr<DispatcherBase::WeakPtr> backendImpl, int callId)
        : DispatcherBase::Callback(std::move(backendImpl), callId) { }

    void sendSuccess(std::unique_ptr<protocol::Runtime::RemoteObject> result, const Maybe<bool>& wasThrown, const Maybe<protocol::Runtime::ExceptionDetails>& exceptionDetails) override
    {
        std::unique_ptr<protocol::DictionaryValue> resultObject = DictionaryValue::create();
        resultObject->setValue("result", ValueConversions<protocol::Runtime::RemoteObject>::serialize(result.get()));
        if (wasThrown.isJust())
            resultObject->setValue("wasThrown", ValueConversions<bool>::serialize(wasThrown.fromJust()));
        if (exceptionDetails.isJust())
            resultObject->setValue("exceptionDetails", ValueConversions<protocol::Runtime::ExceptionDetails>::serialize(exceptionDetails.fromJust()));
        sendIfActive(std::move(resultObject), ErrorString());
    }

    void sendFailure(const ErrorString& error) override
    {
        DCHECK(error.length());
        sendIfActive(nullptr, error);
    }

};

void DispatcherImpl::runScript(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* scriptIdValue = object ? object->get("scriptId") : nullptr;
    errors->setName("scriptId");
    String16 in_scriptId = ValueConversions<String16>::parse(scriptIdValue, errors);
    protocol::Value* executionContextIdValue = object ? object->get("executionContextId") : nullptr;
    Maybe<int> in_executionContextId;
    if (executionContextIdValue) {
        errors->setName("executionContextId");
        in_executionContextId = ValueConversions<int>::parse(executionContextIdValue, errors);
    }
    protocol::Value* objectGroupValue = object ? object->get("objectGroup") : nullptr;
    Maybe<String16> in_objectGroup;
    if (objectGroupValue) {
        errors->setName("objectGroup");
        in_objectGroup = ValueConversions<String16>::parse(objectGroupValue, errors);
    }
    protocol::Value* doNotPauseOnExceptionsAndMuteConsoleValue = object ? object->get("doNotPauseOnExceptionsAndMuteConsole") : nullptr;
    Maybe<bool> in_doNotPauseOnExceptionsAndMuteConsole;
    if (doNotPauseOnExceptionsAndMuteConsoleValue) {
        errors->setName("doNotPauseOnExceptionsAndMuteConsole");
        in_doNotPauseOnExceptionsAndMuteConsole = ValueConversions<bool>::parse(doNotPauseOnExceptionsAndMuteConsoleValue, errors);
    }
    protocol::Value* includeCommandLineAPIValue = object ? object->get("includeCommandLineAPI") : nullptr;
    Maybe<bool> in_includeCommandLineAPI;
    if (includeCommandLineAPIValue) {
        errors->setName("includeCommandLineAPI");
        in_includeCommandLineAPI = ValueConversions<bool>::parse(includeCommandLineAPIValue, errors);
    }
    protocol::Value* returnByValueValue = object ? object->get("returnByValue") : nullptr;
    Maybe<bool> in_returnByValue;
    if (returnByValueValue) {
        errors->setName("returnByValue");
        in_returnByValue = ValueConversions<bool>::parse(returnByValueValue, errors);
    }
    protocol::Value* generatePreviewValue = object ? object->get("generatePreview") : nullptr;
    Maybe<bool> in_generatePreview;
    if (generatePreviewValue) {
        errors->setName("generatePreview");
        in_generatePreview = ValueConversions<bool>::parse(generatePreviewValue, errors);
    }
    protocol::Value* awaitPromiseValue = object ? object->get("awaitPromise") : nullptr;
    Maybe<bool> in_awaitPromise;
    if (awaitPromiseValue) {
        errors->setName("awaitPromise");
        in_awaitPromise = ValueConversions<bool>::parse(awaitPromiseValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    std::unique_ptr<RunScriptCallbackImpl> callback(new RunScriptCallbackImpl(weakPtr(), callId));

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    m_backend->runScript(in_scriptId,in_executionContextId,in_objectGroup,in_doNotPauseOnExceptionsAndMuteConsole,in_includeCommandLineAPI,in_returnByValue,in_generatePreview,in_awaitPromise,std::move(callback));
}

// static
void Dispatcher::wire(UberDispatcher* dispatcher, Backend* backend)
{
    dispatcher->registerBackend("Runtime", wrapUnique(new DispatcherImpl(dispatcher->channel(), backend)));
}

} // Runtime
} // namespace protocol
} // namespace blink
