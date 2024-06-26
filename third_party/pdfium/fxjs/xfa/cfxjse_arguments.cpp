// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cfxjse_arguments.h"

#include "fxjs/xfa/cfxjse_context.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "third_party/base/ptr_util.h"

CFXJSE_Arguments::CFXJSE_Arguments(
    const v8::FunctionCallbackInfo<v8::Value>* pInfo,
    CFXJSE_Value* pRetValue)
    : m_pInfo(pInfo), m_pRetValue(pRetValue) {}

CFXJSE_Arguments::~CFXJSE_Arguments() {}

int32_t CFXJSE_Arguments::GetLength() const {
  return m_pInfo->Length();
}

std::unique_ptr<CFXJSE_Value> CFXJSE_Arguments::GetValue(int32_t index) const {
  auto pArgValue = pdfium::MakeUnique<CFXJSE_Value>(v8::Isolate::GetCurrent());
  pArgValue->ForceSetValue((*m_pInfo)[index]);
  return pArgValue;
}

bool CFXJSE_Arguments::GetBoolean(int32_t index) const {
  return (*m_pInfo)[index]->BooleanValue(m_pInfo->GetIsolate());
}

int32_t CFXJSE_Arguments::GetInt32(int32_t index) const {
  return static_cast<int32_t>(
      (*m_pInfo)[index]
          ->NumberValue(m_pInfo->GetIsolate()->GetCurrentContext())
          .FromMaybe(0.0));
}

float CFXJSE_Arguments::GetFloat(int32_t index) const {
  return static_cast<float>(
      (*m_pInfo)[index]
          ->NumberValue(m_pInfo->GetIsolate()->GetCurrentContext())
          .FromMaybe(0.0));
}

ByteString CFXJSE_Arguments::GetUTF8String(int32_t index) const {
  v8::Isolate* isolate = m_pInfo->GetIsolate();
  v8::Local<v8::Value> info = (*m_pInfo)[index];
  v8::Local<v8::String> hString =
      info->ToString(isolate->GetCurrentContext()).ToLocalChecked();
  v8::String::Utf8Value szStringVal(isolate, hString);
  return ByteString(*szStringVal);
}

CFXJSE_Value* CFXJSE_Arguments::GetReturnValue() const {
  return m_pRetValue.Get();
}
