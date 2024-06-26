// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CFXJSE_ARGUMENTS_H_
#define FXJS_XFA_CFXJSE_ARGUMENTS_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"
#include "v8/include/v8.h"

class CFXJSE_Value;

class CFXJSE_Arguments {
 public:
  CFXJSE_Arguments(const v8::FunctionCallbackInfo<v8::Value>* pInfo,
                   CFXJSE_Value* pRetValue);
  ~CFXJSE_Arguments();

  int32_t GetLength() const;
  std::unique_ptr<CFXJSE_Value> GetValue(int32_t index) const;
  bool GetBoolean(int32_t index) const;
  int32_t GetInt32(int32_t index) const;
  float GetFloat(int32_t index) const;
  ByteString GetUTF8String(int32_t index) const;
  CFXJSE_Value* GetReturnValue() const;

 private:
  UnownedPtr<const v8::FunctionCallbackInfo<v8::Value>> const m_pInfo;
  UnownedPtr<CFXJSE_Value> const m_pRetValue;
};

#endif  // FXJS_XFA_CFXJSE_ARGUMENTS_H_
