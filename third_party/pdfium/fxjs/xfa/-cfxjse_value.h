// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CFXJSE_VALUE_H_
#define FXJS_XFA_CFXJSE_VALUE_H_

#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"
#include "v8/include/v8.h"

class CFXJSE_Class;
class CFXJSE_HostObject;

class CFXJSE_Value {
 public:
  explicit CFXJSE_Value(v8::Isolate* pIsolate);
  ~CFXJSE_Value();

  bool IsEmpty() const;
  bool IsUndefined() const;
  bool IsNull() const;
  bool IsBoolean() const;
  bool IsString() const;
  bool IsNumber() const;
  bool IsInteger() const;
  bool IsObject() const;
  bool IsArray() const;
  bool IsFunction() const;
  bool IsDate() const;
  bool ToBoolean() const;
  float ToFloat() const;
  double ToDouble() const;
  int32_t ToInteger() const;
  ByteString ToString() const;
  WideString ToWideString() const {
    return WideString::FromUTF8(ToString().AsStringView());
  }
  CFXJSE_HostObject* ToHostObject() const;

  void SetUndefined();
  void SetNull();
  void SetBoolean(bool bBoolean);
  void SetInteger(int32_t nInteger);
  void SetDouble(double dDouble);
  void SetString(ByteStringView szString);
  void SetFloat(float fFloat);

  void SetObject(CFXJSE_HostObject* lpObject, CFXJSE_Class* pClass);
  void SetArray(const std::vector<std::unique_ptr<CFXJSE_Value>>& values);
  void SetDate(double dDouble);

  bool GetObjectProperty(ByteStringView szPropName, CFXJSE_Value* lpPropValue);
  bool SetObjectProperty(ByteStringView szPropName, CFXJSE_Value* lpPropValue);
  bool GetObjectPropertyByIdx(uint32_t uPropIdx, CFXJSE_Value* lpPropValue);
  bool SetObjectProperty(uint32_t uPropIdx, CFXJSE_Value* lpPropValue);
  bool DeleteObjectProperty(ByteStringView szPropName);
  bool HasObjectOwnProperty(ByteStringView szPropName, bool bUseTypeGetter);
  bool SetObjectOwnProperty(ByteStringView szPropName,
                            CFXJSE_Value* lpPropValue);
  bool SetFunctionBind(CFXJSE_Value* lpOldFunction, CFXJSE_Value* lpNewThis);

  v8::Isolate* GetIsolate() const { return m_pIsolate.Get(); }
  const v8::Global<v8::Value>& DirectGetValue() const { return m_hValue; }
  void ForceSetValue(v8::Local<v8::Value> hValue) {
    m_hValue.Reset(GetIsolate(), hValue);
  }
  void Assign(const CFXJSE_Value* lpValue) {
    ASSERT(lpValue);
    if (lpValue) {
      m_hValue.Reset(GetIsolate(), lpValue->m_hValue);
    } else {
      m_hValue.Reset();
    }
  }

 private:
  friend class CFXJSE_Class;
  friend class CFXJSE_Context;

  CFXJSE_Value() = delete;
  CFXJSE_Value(const CFXJSE_Value&) = delete;
  CFXJSE_Value& operator=(const CFXJSE_Value&) = delete;

  UnownedPtr<v8::Isolate> const m_pIsolate;
  v8::Global<v8::Value> m_hValue;
};

#endif  // FXJS_XFA_CFXJSE_VALUE_H_
