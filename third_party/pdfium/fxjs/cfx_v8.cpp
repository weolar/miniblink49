// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cfx_v8.h"

#include "core/fxcrt/fx_memory.h"
#include "third_party/base/allocator/partition_allocator/partition_alloc.h"

CFX_V8::CFX_V8(v8::Isolate* isolate) : m_pIsolate(isolate) {}

CFX_V8::~CFX_V8() = default;

v8::Local<v8::Value> CFX_V8::GetObjectProperty(
    v8::Local<v8::Object> pObj,
    ByteStringView bsUTF8PropertyName) {
  if (pObj.IsEmpty())
    return v8::Local<v8::Value>();
  v8::Local<v8::Value> val;
  if (!pObj->Get(m_pIsolate->GetCurrentContext(), NewString(bsUTF8PropertyName))
           .ToLocal(&val))
    return v8::Local<v8::Value>();
  return val;
}

std::vector<WideString> CFX_V8::GetObjectPropertyNames(
    v8::Local<v8::Object> pObj) {
  if (pObj.IsEmpty())
    return std::vector<WideString>();

  v8::Local<v8::Array> val;
  v8::Local<v8::Context> context = m_pIsolate->GetCurrentContext();
  if (!pObj->GetPropertyNames(context).ToLocal(&val))
    return std::vector<WideString>();

  std::vector<WideString> result;
  for (uint32_t i = 0; i < val->Length(); ++i) {
    result.push_back(ToWideString(val->Get(context, i).ToLocalChecked()));
  }

  return result;
}

void CFX_V8::PutObjectProperty(v8::Local<v8::Object> pObj,
                               ByteStringView bsUTF8PropertyName,
                               v8::Local<v8::Value> pPut) {
  if (pObj.IsEmpty())
    return;
  pObj->Set(m_pIsolate->GetCurrentContext(), NewString(bsUTF8PropertyName),
            pPut)
      .FromJust();
}

void CFX_V8::DisposeIsolate() {
  if (m_pIsolate)
    m_pIsolate.Release()->Dispose();
}

v8::Local<v8::Array> CFX_V8::NewArray() {
  return v8::Array::New(GetIsolate());
}

v8::Local<v8::Object> CFX_V8::NewObject() {
  return v8::Object::New(GetIsolate());
}

unsigned CFX_V8::PutArrayElement(v8::Local<v8::Array> pArray,
                                 unsigned index,
                                 v8::Local<v8::Value> pValue) {
  if (pArray.IsEmpty())
    return 0;
  if (pArray->Set(m_pIsolate->GetCurrentContext(), index, pValue).IsNothing())
    return 0;
  return 1;
}

v8::Local<v8::Value> CFX_V8::GetArrayElement(v8::Local<v8::Array> pArray,
                                             unsigned index) {
  if (pArray.IsEmpty())
    return v8::Local<v8::Value>();
  v8::Local<v8::Value> val;
  if (!pArray->Get(m_pIsolate->GetCurrentContext(), index).ToLocal(&val))
    return v8::Local<v8::Value>();
  return val;
}

unsigned CFX_V8::GetArrayLength(v8::Local<v8::Array> pArray) {
  if (pArray.IsEmpty())
    return 0;
  return pArray->Length();
}

v8::Local<v8::Number> CFX_V8::NewNumber(int number) {
  return v8::Int32::New(GetIsolate(), number);
}

v8::Local<v8::Number> CFX_V8::NewNumber(double number) {
  return v8::Number::New(GetIsolate(), number);
}

v8::Local<v8::Number> CFX_V8::NewNumber(float number) {
  return v8::Number::New(GetIsolate(), (float)number);
}

v8::Local<v8::Boolean> CFX_V8::NewBoolean(bool b) {
  return v8::Boolean::New(GetIsolate(), b);
}

v8::Local<v8::String> CFX_V8::NewString(ByteStringView str) {
  v8::Isolate* pIsolate = m_pIsolate ? GetIsolate() : v8::Isolate::GetCurrent();
  return v8::String::NewFromUtf8(pIsolate, str.unterminated_c_str(),
                                 v8::NewStringType::kNormal, str.GetLength())
      .ToLocalChecked();
}

v8::Local<v8::String> CFX_V8::NewString(WideStringView str) {
  // Conversion from pdfium's wchar_t wide-strings to v8's uint16_t
  // wide-strings isn't handled by v8, so use UTF8 as a common
  // intermediate format.
  return NewString(FX_UTF8Encode(str).AsStringView());
}

v8::Local<v8::Value> CFX_V8::NewNull() {
  return v8::Null(GetIsolate());
}

v8::Local<v8::Value> CFX_V8::NewUndefined() {
  return v8::Undefined(GetIsolate());
}

v8::Local<v8::Date> CFX_V8::NewDate(double d) {
  return v8::Date::New(m_pIsolate->GetCurrentContext(), d)
      .ToLocalChecked()
      .As<v8::Date>();
}

int CFX_V8::ToInt32(v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return 0;
  v8::Local<v8::Context> context = m_pIsolate->GetCurrentContext();
  v8::MaybeLocal<v8::Int32> maybe_int32 = pValue->ToInt32(context);
  if (maybe_int32.IsEmpty())
    return 0;
  return maybe_int32.ToLocalChecked()->Value();
}

bool CFX_V8::ToBoolean(v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return false;
  return pValue->BooleanValue(m_pIsolate.Get());
}

double CFX_V8::ToDouble(v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return 0.0;
  v8::Local<v8::Context> context = m_pIsolate->GetCurrentContext();
  v8::MaybeLocal<v8::Number> maybe_number = pValue->ToNumber(context);
  if (maybe_number.IsEmpty())
    return 0.0;
  return maybe_number.ToLocalChecked()->Value();
}

WideString CFX_V8::ToWideString(v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return WideString();
  v8::Local<v8::Context> context = m_pIsolate->GetCurrentContext();
  v8::MaybeLocal<v8::String> maybe_string = pValue->ToString(context);
  if (maybe_string.IsEmpty())
    return WideString();
  v8::String::Utf8Value s(GetIsolate(), maybe_string.ToLocalChecked());
  return WideString::FromUTF8(ByteStringView(*s, s.length()));
}

ByteString CFX_V8::ToByteString(v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty())
    return ByteString();
  v8::Local<v8::Context> context = m_pIsolate->GetCurrentContext();
  v8::MaybeLocal<v8::String> maybe_string = pValue->ToString(context);
  if (maybe_string.IsEmpty())
    return ByteString();
  v8::String::Utf8Value s(GetIsolate(), maybe_string.ToLocalChecked());
  return ByteString(*s);
}

v8::Local<v8::Object> CFX_V8::ToObject(v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty() || !pValue->IsObject())
    return v8::Local<v8::Object>();
  v8::Local<v8::Context> context = m_pIsolate->GetCurrentContext();
  return pValue->ToObject(context).ToLocalChecked();
}

v8::Local<v8::Array> CFX_V8::ToArray(v8::Local<v8::Value> pValue) {
  if (pValue.IsEmpty() || !pValue->IsArray())
    return v8::Local<v8::Array>();
  v8::Local<v8::Context> context = m_pIsolate->GetCurrentContext();
  return v8::Local<v8::Array>::Cast(pValue->ToObject(context).ToLocalChecked());
}

void* CFX_V8ArrayBufferAllocator::Allocate(size_t length) {
  if (length > kMaxAllowedBytes)
    return nullptr;
  return GetArrayBufferPartitionAllocator().root()->AllocFlags(
      pdfium::base::PartitionAllocZeroFill, length, "CFX_V8ArrayBuffer");
}

void* CFX_V8ArrayBufferAllocator::AllocateUninitialized(size_t length) {
  if (length > kMaxAllowedBytes)
    return nullptr;
  return GetArrayBufferPartitionAllocator().root()->Alloc(length,
                                                          "CFX_V8ArrayBuffer");
}

void CFX_V8ArrayBufferAllocator::Free(void* data, size_t length) {
  GetArrayBufferPartitionAllocator().root()->Free(data);
}
