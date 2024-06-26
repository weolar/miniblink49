// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cfxjse_value.h"

#include <math.h>

#include "fxjs/xfa/cfxjse_class.h"
#include "fxjs/xfa/cfxjse_context.h"
#include "fxjs/xfa/cfxjse_isolatetracker.h"

namespace {

double ftod(float fNumber) {
  static_assert(sizeof(float) == 4, "float of incorrect size");

  uint32_t nFloatBits = (uint32_t&)fNumber;
  uint8_t nExponent = (uint8_t)(nFloatBits >> 23);
  if (nExponent == 0 || nExponent == 255)
    return fNumber;

  int8_t nErrExp = nExponent - 150;
  if (nErrExp >= 0)
    return fNumber;

  double dwError = pow(2.0, nErrExp), dwErrorHalf = dwError / 2;
  double dNumber = fNumber, dNumberAbs = fabs(fNumber);
  double dNumberAbsMin = dNumberAbs - dwErrorHalf,
         dNumberAbsMax = dNumberAbs + dwErrorHalf;
  int32_t iErrPos = 0;
  if (floor(dNumberAbsMin) == floor(dNumberAbsMax)) {
    dNumberAbsMin = fmod(dNumberAbsMin, 1.0);
    dNumberAbsMax = fmod(dNumberAbsMax, 1.0);
    int32_t iErrPosMin = 1, iErrPosMax = 38;
    do {
      int32_t iMid = (iErrPosMin + iErrPosMax) / 2;
      double dPow = pow(10.0, iMid);
      if (floor(dNumberAbsMin * dPow) == floor(dNumberAbsMax * dPow)) {
        iErrPosMin = iMid + 1;
      } else {
        iErrPosMax = iMid;
      }
    } while (iErrPosMin < iErrPosMax);
    iErrPos = iErrPosMax;
  }
  double dPow = pow(10.0, iErrPos);
  return fNumber < 0 ? ceil(dNumber * dPow - 0.5) / dPow
                     : floor(dNumber * dPow + 0.5) / dPow;
}

}  // namespace

void FXJSE_ThrowMessage(ByteStringView utf8Message) {
  v8::Isolate* pIsolate = v8::Isolate::GetCurrent();
  ASSERT(pIsolate);

  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(pIsolate);
  v8::Local<v8::String> hMessage =
      v8::String::NewFromUtf8(pIsolate, utf8Message.unterminated_c_str(),
                              v8::NewStringType::kNormal,
                              utf8Message.GetLength())
          .ToLocalChecked();
  v8::Local<v8::Value> hError = v8::Exception::Error(hMessage);
  pIsolate->ThrowException(hError);
}

CFXJSE_Value::CFXJSE_Value(v8::Isolate* pIsolate) : m_pIsolate(pIsolate) {}

CFXJSE_Value::~CFXJSE_Value() {}

CFXJSE_HostObject* CFXJSE_Value::ToHostObject() const {
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> pValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  ASSERT(!pValue.IsEmpty());
  if (!pValue->IsObject())
    return nullptr;

  return FXJSE_RetrieveObjectBinding(pValue.As<v8::Object>());
}

void CFXJSE_Value::SetObject(CFXJSE_HostObject* lpObject,
                             CFXJSE_Class* pClass) {
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::FunctionTemplate> hClass =
      v8::Local<v8::FunctionTemplate>::New(GetIsolate(), pClass->m_hTemplate);
  v8::Local<v8::Object> hObject =
      hClass->InstanceTemplate()
          ->NewInstance(GetIsolate()->GetCurrentContext())
          .ToLocalChecked();
  FXJSE_UpdateObjectBinding(hObject, lpObject);
  m_hValue.Reset(GetIsolate(), hObject);
}

void CFXJSE_Value::SetArray(
    const std::vector<std::unique_ptr<CFXJSE_Value>>& values) {
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Array> hArrayObject =
      v8::Array::New(GetIsolate(), values.size());
  v8::Local<v8::Context> context = GetIsolate()->GetCurrentContext();
  uint32_t count = 0;
  for (auto& v : values) {
    if (v->IsEmpty())
      v->SetUndefined();
    hArrayObject
        ->Set(
            context, count++,
            v8::Local<v8::Value>::New(GetIsolate(), v.get()->DirectGetValue()))
        .FromJust();
  }
  m_hValue.Reset(GetIsolate(), hArrayObject);
}

void CFXJSE_Value::SetDate(double dDouble) {
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hDate =
      v8::Date::New(GetIsolate()->GetCurrentContext(), dDouble)
          .ToLocalChecked();
  m_hValue.Reset(GetIsolate(), hDate);
}

void CFXJSE_Value::SetFloat(float fFloat) {
  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> pValue = v8::Number::New(GetIsolate(), ftod(fFloat));
  m_hValue.Reset(GetIsolate(), pValue);
}

bool CFXJSE_Value::SetObjectProperty(ByteStringView szPropName,
                                     CFXJSE_Value* lpPropValue) {
  ASSERT(lpPropValue);
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hObject =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  if (!hObject->IsObject())
    return false;

  v8::Local<v8::String> hPropName =
      v8::String::NewFromUtf8(GetIsolate(), szPropName.unterminated_c_str(),
                              v8::NewStringType::kNormal,
                              szPropName.GetLength())
          .ToLocalChecked();
  v8::Local<v8::Value> hPropValue =
      v8::Local<v8::Value>::New(GetIsolate(), lpPropValue->DirectGetValue());
  return hObject.As<v8::Object>()
      ->Set(GetIsolate()->GetCurrentContext(), hPropName, hPropValue)
      .FromJust();
}

bool CFXJSE_Value::GetObjectProperty(ByteStringView szPropName,
                                     CFXJSE_Value* lpPropValue) {
  ASSERT(lpPropValue);
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hObject =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  if (!hObject->IsObject())
    return false;

  v8::Local<v8::String> hPropName =
      v8::String::NewFromUtf8(GetIsolate(), szPropName.unterminated_c_str(),
                              v8::NewStringType::kNormal,
                              szPropName.GetLength())
          .ToLocalChecked();
  v8::Local<v8::Value> hPropValue =
      hObject.As<v8::Object>()
          ->Get(GetIsolate()->GetCurrentContext(), hPropName)
          .ToLocalChecked();
  lpPropValue->ForceSetValue(hPropValue);
  return true;
}

bool CFXJSE_Value::SetObjectProperty(uint32_t uPropIdx,
                                     CFXJSE_Value* lpPropValue) {
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hObject =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  if (!hObject->IsObject())
    return false;

  v8::Local<v8::Value> hPropValue =
      v8::Local<v8::Value>::New(GetIsolate(), lpPropValue->DirectGetValue());
  return hObject.As<v8::Object>()
      ->Set(GetIsolate()->GetCurrentContext(), uPropIdx, hPropValue)
      .FromJust();
}

bool CFXJSE_Value::GetObjectPropertyByIdx(uint32_t uPropIdx,
                                          CFXJSE_Value* lpPropValue) {
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hObject =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  if (!hObject->IsObject())
    return false;

  v8::Local<v8::Value> hPropValue =
      hObject.As<v8::Object>()
          ->Get(GetIsolate()->GetCurrentContext(), uPropIdx)
          .ToLocalChecked();
  lpPropValue->ForceSetValue(hPropValue);
  return true;
}

bool CFXJSE_Value::DeleteObjectProperty(ByteStringView szPropName) {
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hObject =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  if (!hObject->IsObject())
    return false;

  v8::Local<v8::String> hPropName =
      v8::String::NewFromUtf8(GetIsolate(), szPropName.unterminated_c_str(),
                              v8::NewStringType::kNormal,
                              szPropName.GetLength())
          .ToLocalChecked();
  return hObject.As<v8::Object>()
      ->Delete(GetIsolate()->GetCurrentContext(), hPropName)
      .FromJust();
}

bool CFXJSE_Value::HasObjectOwnProperty(ByteStringView szPropName,
                                        bool bUseTypeGetter) {
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hObject =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  if (!hObject->IsObject())
    return false;

  v8::Local<v8::String> hKey =
      v8::String::NewFromUtf8(GetIsolate(), szPropName.unterminated_c_str(),
                              v8::NewStringType::kNormal,
                              szPropName.GetLength())
          .ToLocalChecked();
  return hObject.As<v8::Object>()
             ->HasRealNamedProperty(GetIsolate()->GetCurrentContext(), hKey)
             .FromJust() ||
         (bUseTypeGetter &&
          hObject.As<v8::Object>()
              ->HasOwnProperty(GetIsolate()->GetCurrentContext(), hKey)
              .FromMaybe(false));
}

bool CFXJSE_Value::SetObjectOwnProperty(ByteStringView szPropName,
                                        CFXJSE_Value* lpPropValue) {
  ASSERT(lpPropValue);
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hObject =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  if (!hObject->IsObject())
    return false;

  v8::Local<v8::String> hPropName =
      v8::String::NewFromUtf8(GetIsolate(), szPropName.unterminated_c_str(),
                              v8::NewStringType::kNormal,
                              szPropName.GetLength())
          .ToLocalChecked();
  v8::Local<v8::Value> pValue =
      v8::Local<v8::Value>::New(GetIsolate(), lpPropValue->m_hValue);
  return hObject.As<v8::Object>()
      ->DefineOwnProperty(GetIsolate()->GetCurrentContext(), hPropName, pValue)
      .FromMaybe(false);
}

bool CFXJSE_Value::SetFunctionBind(CFXJSE_Value* lpOldFunction,
                                   CFXJSE_Value* lpNewThis) {
  ASSERT(lpOldFunction);
  ASSERT(lpNewThis);

  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> rgArgs[2];
  v8::Local<v8::Value> hOldFunction =
      v8::Local<v8::Value>::New(GetIsolate(), lpOldFunction->DirectGetValue());
  if (hOldFunction.IsEmpty() || !hOldFunction->IsFunction())
    return false;

  rgArgs[0] = hOldFunction;
  v8::Local<v8::Value> hNewThis =
      v8::Local<v8::Value>::New(GetIsolate(), lpNewThis->DirectGetValue());
  if (hNewThis.IsEmpty())
    return false;

  rgArgs[1] = hNewThis;
  v8::Local<v8::String> hBinderFuncSource =
      v8::String::NewFromUtf8(GetIsolate(),
                              "(function (oldfunction, newthis) { return "
                              "oldfunction.bind(newthis); })",
                              v8::NewStringType::kNormal)
          .ToLocalChecked();
  v8::Local<v8::Context> hContext = GetIsolate()->GetCurrentContext();
  v8::Local<v8::Function> hBinderFunc =
      v8::Script::Compile(hContext, hBinderFuncSource)
          .ToLocalChecked()
          ->Run(hContext)
          .ToLocalChecked()
          .As<v8::Function>();
  v8::Local<v8::Value> hBoundFunction =
      hBinderFunc->Call(hContext, hContext->Global(), 2, rgArgs)
          .ToLocalChecked();
  if (hBoundFunction.IsEmpty() || !hBoundFunction->IsFunction())
    return false;

  m_hValue.Reset(GetIsolate(), hBoundFunction);
  return true;
}

bool CFXJSE_Value::IsEmpty() const {
  return m_hValue.IsEmpty();
}

bool CFXJSE_Value::IsUndefined() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsUndefined();
}

bool CFXJSE_Value::IsNull() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsNull();
}

bool CFXJSE_Value::IsBoolean() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsBoolean();
}

bool CFXJSE_Value::IsString() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsString();
}

bool CFXJSE_Value::IsNumber() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsNumber();
}

bool CFXJSE_Value::IsInteger() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsInt32();
}

bool CFXJSE_Value::IsObject() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsObject();
}

bool CFXJSE_Value::IsArray() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsArray();
}

bool CFXJSE_Value::IsFunction() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsFunction();
}

bool CFXJSE_Value::IsDate() const {
  if (IsEmpty())
    return false;

  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->IsDate();
}

bool CFXJSE_Value::ToBoolean() const {
  ASSERT(!IsEmpty());
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->BooleanValue(GetIsolate());
}

float CFXJSE_Value::ToFloat() const {
  ASSERT(!IsEmpty());
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return static_cast<float>(
      hValue->NumberValue(GetIsolate()->GetCurrentContext()).FromMaybe(0.0));
}

double CFXJSE_Value::ToDouble() const {
  ASSERT(!IsEmpty());
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return hValue->NumberValue(GetIsolate()->GetCurrentContext()).FromMaybe(0.0);
}

int32_t CFXJSE_Value::ToInteger() const {
  ASSERT(!IsEmpty());
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  return static_cast<int32_t>(
      hValue->NumberValue(GetIsolate()->GetCurrentContext()).FromMaybe(0.0));
}

ByteString CFXJSE_Value::ToString() const {
  ASSERT(!IsEmpty());
  CFXJSE_ScopeUtil_IsolateHandleRootContext scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::Local<v8::Value>::New(GetIsolate(), m_hValue);
  v8::Local<v8::String> hString =
      hValue->ToString(GetIsolate()->GetCurrentContext()).ToLocalChecked();
  v8::String::Utf8Value hStringVal(GetIsolate(), hString);
  return ByteString(*hStringVal);
}

void CFXJSE_Value::SetUndefined() {
  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue = v8::Undefined(GetIsolate());
  m_hValue.Reset(GetIsolate(), hValue);
}

void CFXJSE_Value::SetNull() {
  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue = v8::Null(GetIsolate());
  m_hValue.Reset(GetIsolate(), hValue);
}

void CFXJSE_Value::SetBoolean(bool bBoolean) {
  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue = v8::Boolean::New(GetIsolate(), !!bBoolean);
  m_hValue.Reset(GetIsolate(), hValue);
}

void CFXJSE_Value::SetInteger(int32_t nInteger) {
  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue = v8::Integer::New(GetIsolate(), nInteger);
  m_hValue.Reset(GetIsolate(), hValue);
}

void CFXJSE_Value::SetDouble(double dDouble) {
  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue = v8::Number::New(GetIsolate(), dDouble);
  m_hValue.Reset(GetIsolate(), hValue);
}

void CFXJSE_Value::SetString(ByteStringView szString) {
  CFXJSE_ScopeUtil_IsolateHandle scope(GetIsolate());
  v8::Local<v8::Value> hValue =
      v8::String::NewFromUtf8(GetIsolate(), szString.unterminated_c_str(),
                              v8::NewStringType::kNormal, szString.GetLength())
          .ToLocalChecked();
  m_hValue.Reset(GetIsolate(), hValue);
}
