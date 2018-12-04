// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/converter.h"

#include "v8.h"

#include <windows.h>

using v8::ArrayBuffer;
using v8::Boolean;
using v8::External;
using v8::Function;
using v8::Int32;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Uint32;
using v8::Value;

namespace {

template <typename T, typename U>
bool FromMaybe(Maybe<T> maybe, U* out) {
  if (maybe.IsNothing())
    return false;
  *out = static_cast<U>(maybe.FromJust());
  return true;
}

}  // namespace

namespace gin {

Local<Value> Converter<bool>::ToV8(Isolate* isolate, bool val) {
  return Boolean::New(isolate, val).As<Value>();
}

bool Converter<bool>::FromV8(Isolate* isolate, Local<Value> val, bool* out) {
  return FromMaybe(val->BooleanValue(isolate->GetCurrentContext()), out);
}

Local<Value> Converter<int32_t>::ToV8(Isolate* isolate, int32_t val) {
  return Integer::New(isolate, val).As<Value>();
}

bool Converter<int32_t>::FromV8(Isolate* isolate,
                                Local<Value> val,
                                int32_t* out) {
  if (!val->IsInt32())
    return false;
  *out = val.As<Int32>()->Value();
  return true;
}

Local<Value> Converter<uint32_t>::ToV8(Isolate* isolate, uint32_t val) {
  return Integer::NewFromUnsigned(isolate, val).As<Value>();
}

bool Converter<uint32_t>::FromV8(Isolate* isolate,
                                 Local<Value> val,
                                 uint32_t* out) {
  if (!val->IsUint32())
    return false;
  *out = val.As<Uint32>()->Value();
  return true;
}

Local<Value> Converter<int64_t>::ToV8(Isolate* isolate, int64_t val) {
  return Number::New(isolate, static_cast<double>(val)).As<Value>();
}

bool Converter<int64_t>::FromV8(Isolate* isolate,
                                Local<Value> val,
                                int64_t* out) {
  if (!val->IsNumber())
    return false;
  // Even though IntegerValue returns int64_t, JavaScript cannot represent
  // the full precision of int64_t, which means some rounding might occur.
  return FromMaybe(val->IntegerValue(isolate->GetCurrentContext()), out);
}

Local<Value> Converter<uint64_t>::ToV8(Isolate* isolate, uint64_t val) {
  return Number::New(isolate, static_cast<double>(val)).As<Value>();
}

bool Converter<uint64_t>::FromV8(Isolate* isolate,
                                 Local<Value> val,
                                 uint64_t* out) {
  if (!val->IsNumber())
    return false;
  return FromMaybe(val->IntegerValue(isolate->GetCurrentContext()), out);
}

Local<Value> Converter<float>::ToV8(Isolate* isolate, float val) {
  return Number::New(isolate, val).As<Value>();
}

bool Converter<float>::FromV8(Isolate* isolate, Local<Value> val, float* out) {
  if (!val->IsNumber())
    return false;
  *out = static_cast<float>(val.As<Number>()->Value());
  return true;
}

Local<Value> Converter<double>::ToV8(Isolate* isolate, double val) {
  return Number::New(isolate, val).As<Value>();
}

bool Converter<double>::FromV8(Isolate* isolate,
                               Local<Value> val,
                               double* out) {
  if (!val->IsNumber())
    return false;
  *out = val.As<Number>()->Value();
  return true;
}

Local<Value> Converter<base::StringPiece>::ToV8(Isolate* isolate,
                                                const base::StringPiece& val) {
  return String::NewFromUtf8(isolate, val.data(),
                             v8::NewStringType::kNormal,
                             static_cast<uint32_t>(val.length()))
      .ToLocalChecked();
}

Local<Value> Converter<const char*>::ToV8(Isolate* isolate, const char* val) {
    return Converter<base::StringPiece>::ToV8(isolate, val);
}

Local<Value> Converter<std::string>::ToV8(Isolate* isolate,
                                          const std::string& val) {
  return Converter<base::StringPiece>::ToV8(isolate, val);
}

bool Converter<std::string>::FromV8(Isolate* isolate,
                                    Local<Value> val,
                                    std::string* out) {
  if (!val->IsString())
    return false;
  Local<String> str = Local<String>::Cast(val);
  int length = str->Utf8Length();
  out->resize(length);
  str->WriteUtf8(&(*out)[0], length, NULL, String::NO_NULL_TERMINATION);
  return true;
}

Local<Value> Converter<v8::Local<v8::String> >::ToV8(Isolate* isolate, Local<v8::String> val) {
    return val.As<Value>();
}

bool Converter<v8::Local<v8::String> >::FromV8(Isolate* isolate, Local<Value> val, v8::Local<v8::String>* out) {
    if (!val->IsString())
        return false;
    *out = Local<String>::Cast(val);
    return true;
}

bool Converter<Local<Function>>::FromV8(Isolate* isolate,
                                        Local<Value> val,
                                        Local<Function>* out) {
  if (!val->IsFunction())
    return false;
  *out = Local<Function>::Cast(val);
  return true;
}

Local<Value> Converter<Local<Object>>::ToV8(Isolate* isolate, Local<Object> val) {
  return val.As<Value>();
}

Local<Value> Converter<Local<v8::Primitive>>::ToV8(Isolate* isolate, Local<v8::Primitive> val) {
    return val.As<Value>();
}

bool Converter<Local<Object>>::FromV8(Isolate* isolate,
                                      Local<Value> val,
                                      Local<Object>* out) {
  if (!val->IsObject())
    return false;
  *out = Local<Object>::Cast(val);
  return true;
}

Local<Value> Converter<Local<ArrayBuffer>>::ToV8(Isolate* isolate,
                                                 Local<ArrayBuffer> val) {
  return val.As<Value>();
}

bool Converter<Local<ArrayBuffer>>::FromV8(Isolate* isolate,
                                           Local<Value> val,
                                           Local<ArrayBuffer>* out) {
  if (!val->IsArrayBuffer())
    return false;
  *out = Local<ArrayBuffer>::Cast(val);
  return true;
}

Local<Value> Converter<Local<External>>::ToV8(Isolate* isolate,
                                              Local<External> val) {
  return val.As<Value>();
}

bool Converter<Local<External>>::FromV8(Isolate* isolate,
                                        v8::Local<Value> val,
                                        Local<External>* out) {
  if (!val->IsExternal())
    return false;
  *out = Local<External>::Cast(val);
  return true;
}

Local<Value> Converter<Local<Value>>::ToV8(Isolate* isolate, Local<Value> val) {
  return val;
}

bool Converter<Local<Value>>::FromV8(Isolate* isolate,
                                     Local<Value> val,
                                     Local<Value>* out) {
  *out = val;
  return true;
}

Local<Value> Converter<Local<v8::Array>>::ToV8(Isolate* isolate, Local<v8::Array> val) {
    return val.As<Value>();
}

bool Converter<Local<v8::Array>>::FromV8(Isolate* isolate,
    Local<Value> val,
    Local<v8::Array>* out) {
    if (!val->IsArray())
        return false;
    *out = Local<v8::Array>::Cast(val);
    return true;
}

int v8ValueToType(Local<Value> v) {
    int type = 0;
    bool b = false;
    b = v->IsUndefined(); if (b) return 0;
    b = v->IsNull(); if (b) return 1;
    b = v->IsTrue(); if (b) return 2;
    b = v->IsFalse(); if (b) return 3;
    b = v->IsName(); if (b) return 4;
    b = v->IsString(); if (b) return 5;
    b = v->IsSymbol(); if (b) return 6;
    b = v->IsFunction(); if (b) return 7;
    b = v->IsArray(); if (b) return 8;
    b = v->IsObject(); if (b) return 9;
    b = v->IsBoolean(); if (b) return 10;
    b = v->IsNumber(); if (b) return 11;
    b = v->IsExternal(); if (b) return 12;
    b = v->IsInt32(); if (b) return 13;
    b = v->IsUint32(); if (b) return 14;
    b = v->IsDate(); if (b) return 15;
    b = v->IsArgumentsObject(); if (b) return 16;
    b = v->IsBooleanObject(); if (b) return 17;
    b = v->IsNumberObject(); if (b) return 18;
    b = v->IsStringObject(); if (b) return 19;
    b = v->IsSymbolObject(); if (b) return 20;
    b = v->IsNativeError(); if (b) return 21;
    b = v->IsRegExp(); if (b) return 22;
    b = v->IsGeneratorFunction(); if (b) return 23;
    b = v->IsGeneratorObject(); if (b) return 24;
    b = v->IsPromise(); if (b) return 25;
    b = v->IsMap(); if (b) return 26;
    b = v->IsSet(); if (b) return 27;
    b = v->IsMapIterator(); if (b) return 28;
    b = v->IsSetIterator(); if (b) return 29;
    b = v->IsWeakMap(); if (b) return 30;
    b = v->IsWeakSet(); if (b) return 31;
    b = v->IsArrayBuffer(); if (b) return 32;
    b = v->IsArrayBufferView(); if (b) return 33;
    b = v->IsTypedArray(); if (b) return 34;
    b = v->IsUint8Array(); if (b) return 35;
    b = v->IsUint8ClampedArray(); if (b) return 36;
    b = v->IsInt8Array(); if (b) return 37;
    b = v->IsUint16Array(); if (b) return 38;
    b = v->IsInt16Array(); if (b) return 39;
    b = v->IsUint32Array(); if (b) return 40;
    b = v->IsInt32Array(); if (b) return 41;
    b = v->IsFloat32Array(); if (b) return 42;
    b = v->IsFloat64Array(); if (b) return 43;
    b = v->IsDataView(); if (b) return 44;
    b = v->IsSharedArrayBuffer(); if (b) return 45;
    return -1;
}

Local<Value> Converter<base::DictionaryValue>::ToV8(Isolate* isolate, const base::DictionaryValue& val) {
    size_t size = val.size();
    Local<v8::Object> v8Ojb = v8::Object::New(isolate);

    bool boolVal = false;
    int intVal = 0;
    double doubleVal = 0;
    std::string strVal;
    const base::ListValue* listValue = nullptr;
    const base::DictionaryValue* dictionaryValue = nullptr;
    base::DictionaryValue::Iterator it(val);
    for (size_t i = 0; !it.IsAtEnd(); ++i, it.Advance()) {
        const base::Value* outValue = &it.value();
        std::string key = it.key();
        if (key.empty())
            key = "noElementToV8";

        Local<Value> v8Key = Converter<std::string>::ToV8(isolate, key);
        base::Value::Type type = outValue->GetType();
        switch (type) {
        case base::Value::TYPE_BOOLEAN:
            outValue->GetAsBoolean(&boolVal);
            v8Ojb->Set(v8Key, v8::Boolean::New(isolate, boolVal));
            break;
        case base::Value::TYPE_INTEGER:
            outValue->GetAsInteger(&intVal);
            v8Ojb->Set(v8Key, v8::Integer::New(isolate, intVal));
            break;
        case base::Value::TYPE_DOUBLE:
            outValue->GetAsDouble(&doubleVal);
            v8Ojb->Set(v8Key, v8::Number::New(isolate, doubleVal));
            break;
        case base::Value::TYPE_STRING:
            outValue->GetAsString(&strVal);
            v8Ojb->Set(v8Key, String::NewFromUtf8(isolate, strVal.c_str(), v8::NewStringType::kNormal, strVal.size()).ToLocalChecked());
            break;
        case base::Value::TYPE_LIST:
            outValue->GetAsList(&listValue);
            v8Ojb->Set(v8Key, Converter<base::ListValue>::ToV8(isolate, *listValue));
            break;
        case base::Value::TYPE_DICTIONARY:
            outValue->GetAsDictionary(&dictionaryValue);
            v8Ojb->Set(v8Key, Converter<base::DictionaryValue>::ToV8(isolate, *dictionaryValue));
            break;
        case base::Value::TYPE_NULL:
            v8Ojb->Set(v8Key, v8::Null(isolate));
            break;
        default:
            DebugBreak();
            v8Ojb->Set(v8Key, v8::Null(isolate));
            break;
        }
    }

    return v8Ojb.As<Value>();
}

bool Converter<base::DictionaryValue>::FromV8(Isolate* isolate, Local<Value> val, base::DictionaryValue* out) {
    if (!val->IsObject())
        return false;

    v8::Object* v8Obj = v8::Object::Cast(*val);

    Local<v8::Array> v8ObjProps = v8Obj->GetOwnPropertyNames();
    size_t size = v8ObjProps->Length();

    for (size_t i = 0; i < size; ++i) {
        Local<Value> keyNameValue = v8ObjProps->Get(i);
        Local<Value> outValue = v8Obj->Get(keyNameValue);

        std::string keyNameStr;
        if (!Converter<std::string>::FromV8(isolate, keyNameValue, &keyNameStr))
            return false;
        if (keyNameStr.empty())
            keyNameStr = "noElementFromV8";
        
        if (outValue->IsBoolean()) {
            Local<v8::Boolean> boolVal = outValue->ToBoolean(isolate);
            out->SetBoolean(keyNameStr, boolVal->Value());
        } else if (outValue->IsInt32()) {
            Local<v8::Int32> intVal = outValue->ToInt32(isolate);
            out->SetInteger(keyNameStr, intVal->Value());
        } else if (outValue->IsUint32()) {
            Local<v8::Uint32> intVal = outValue->ToUint32(isolate);
            out->SetInteger(keyNameStr, intVal->Value());
        } else if (outValue->IsNumber()) {
            Local<v8::Number> doubleVal = outValue->ToNumber(isolate);
            out->SetDouble(keyNameStr, doubleVal->Value());
        } else if (outValue->IsString()) {
            Local<v8::String> strVal = outValue->ToString(isolate);
            v8::String::Utf8Value utf8(strVal);
            out->SetString(keyNameStr, std::string(*utf8));
        } else if (outValue->IsArray()) {
            base::ListValue* arrayOut = new base::ListValue();
            if (!Converter<base::ListValue>::FromV8(isolate, outValue, arrayOut)) {
                delete arrayOut;            
                return false;
            }
            out->Set(keyNameStr, arrayOut);
        } else if (outValue->IsObject()) {
            base::DictionaryValue* dictionaryOut = new base::DictionaryValue();
            if (!Converter<base::DictionaryValue>::FromV8(isolate, outValue, dictionaryOut)) {
                delete dictionaryOut;
                return false;
            }
            out->Set(keyNameStr, dictionaryOut);
        } else if (outValue->IsUndefined()) {
            out->Set(keyNameStr, base::Value::CreateNullValue());
        } else if (outValue->IsNull()) {
            out->Set(keyNameStr, base::Value::CreateNullValue());
        } else {
            DebugBreak();
            int type = v8ValueToType(outValue);
            out->Set(keyNameStr, base::Value::CreateNullValue());
            return false;
        }
    }
    return true;
}

Local<Value> Converter<base::ListValue>::ToV8(Isolate* isolate, const base::ListValue& val) {
    size_t size = val.GetSize();
    Local<v8::Array> v8Arr = v8::Array::New(isolate, size);

    bool boolVal = false;
    int intVal = 0;
    double doubleVal = 0;
    std::string strVal;
    const base::ListValue* listValue = nullptr;
    const base::DictionaryValue* dictionaryValue = nullptr;
    for (size_t i = 0; i < size; ++i) {
        const base::Value* outValue;
        val.Get(i, &outValue);
        base::Value::Type type = outValue->GetType();
        switch (type) {
        case base::Value::TYPE_BOOLEAN:
            outValue->GetAsBoolean(&boolVal);
            v8Arr->Set(i, v8::Boolean::New(isolate, boolVal));
            break;
        case base::Value::TYPE_INTEGER:
            outValue->GetAsInteger(&intVal);
            v8Arr->Set(i, v8::Integer::New(isolate, intVal));
            break;
        case base::Value::TYPE_DOUBLE:
            outValue->GetAsDouble(&doubleVal);
            v8Arr->Set(i, v8::Number::New(isolate, doubleVal));
            break;
        case base::Value::TYPE_STRING:
            outValue->GetAsString(&strVal);
            v8Arr->Set(i, String::NewFromUtf8(isolate, strVal.c_str(), v8::NewStringType::kNormal, strVal.size()).ToLocalChecked());
            break;
        case base::Value::TYPE_LIST:
            outValue->GetAsList(&listValue);
            v8Arr->Set(i, ToV8(isolate, *listValue));
            break;
        case base::Value::TYPE_DICTIONARY:
            outValue->GetAsDictionary(&dictionaryValue);
            v8Arr->Set(i, Converter<base::DictionaryValue>::ToV8(isolate, *dictionaryValue));
            break;
        case base::Value::TYPE_NULL:
            v8Arr->Set(i, v8::Null(isolate));
            break;
        default:
            DebugBreak();
            v8Arr->Set(i, v8::Null(isolate));
            break;
        }
    }
    
    return v8Arr.As<Value>();
}

bool Converter<base::ListValue>::FromV8(Isolate* isolate, Local<Value> val, base::ListValue* out) {
    if (!val->IsArray())
        return false; 

    v8::Array* v8Arr = v8::Array::Cast(*val);
    size_t size = v8Arr->Length();
    for (size_t i = 0; i < size; ++i) {
        Local<Value> outValue = v8Arr->Get(i);
        if (outValue->IsBoolean()) {
            Local<v8::Boolean> boolVal = outValue->ToBoolean(isolate);
            out->AppendBoolean(boolVal->Value());
        } else if (outValue->IsInt32()) {
            Local<v8::Int32> intVal = outValue->ToInt32(isolate);
            out->AppendInteger(intVal->Value());
        } else if (outValue->IsUint32()) {
            Local<v8::Uint32> uintVal = outValue->ToUint32(isolate);
            out->AppendInteger(uintVal->Value());
        } else if (outValue->IsNumber()) {
            Local<v8::Number> doubleVal = outValue->ToNumber(isolate);
            out->AppendDouble(doubleVal->Value());
        } else if (outValue->IsString()) {
            Local<v8::String> strVal = outValue->ToString(isolate);
            v8::String::Utf8Value utf8(strVal);
            out->AppendString(std::string(*utf8));
        } else if (outValue->IsArray()) {
            base::ListValue* arrayOut = new base::ListValue();
            if (!FromV8(isolate, outValue, arrayOut)) {
                delete arrayOut;
                return false;
            }
            out->Append(arrayOut);
        } else if (outValue->IsNull() || outValue->IsUndefined()) {
            out->Append(base::Value::CreateNullValue());
        } else if (outValue->IsObject()) {
            base::DictionaryValue* dictionaryOut = new base::DictionaryValue();
            if (!Converter<base::DictionaryValue>::FromV8(isolate, outValue, dictionaryOut)) {
                delete dictionaryOut;
                return false;
            }
            out->Append(dictionaryOut);
        } else {
            DebugBreak();
            int type = v8ValueToType(outValue);
            out->Append(base::Value::CreateNullValue());
            return false;
        }
    }
    return true;
}

v8::Local<v8::String> StringToSymbol(v8::Isolate* isolate,
                                      const base::StringPiece& val) {
  return String::NewFromUtf8(isolate, val.data(),
                             v8::NewStringType::kInternalized,
                             static_cast<uint32_t>(val.length()))
      .ToLocalChecked();
}

std::string V8ToString(v8::Local<v8::Value> value) {
  if (value.IsEmpty())
    return std::string();
  std::string result;
  if (!ConvertFromV8(NULL, value, &result))
    return std::string();
  return result;
}

v8::Local<v8::Value> ConvertToV8(v8::Isolate* isolate, const base::ListValue& input) {
    return Converter<base::ListValue>::ToV8(isolate, input);
}

}  // namespace gin
