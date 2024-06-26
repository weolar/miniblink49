// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CFXJSE_CONTEXT_H_
#define FXJS_XFA_CFXJSE_CONTEXT_H_

#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"
#include "v8/include/v8.h"

class CFXJS_Engine;
class CFXJSE_Class;
class CFXJSE_HostObject;
class CFXJSE_Value;
struct FXJSE_CLASS_DESCRIPTOR;

class CFXJSE_Context {
 public:
  static std::unique_ptr<CFXJSE_Context> Create(
      v8::Isolate* pIsolate,
      const FXJSE_CLASS_DESCRIPTOR* pGlobalClass,
      CFXJSE_HostObject* pGlobalObject);

  explicit CFXJSE_Context(v8::Isolate* pIsolate);
  ~CFXJSE_Context();

  v8::Isolate* GetIsolate() const { return m_pIsolate.Get(); }
  v8::Local<v8::Context> GetContext();
  std::unique_ptr<CFXJSE_Value> GetGlobalObject();
  void AddClass(std::unique_ptr<CFXJSE_Class> pClass);
  CFXJSE_Class* GetClassByName(ByteStringView szName) const;
  void EnableCompatibleMode();
  bool ExecuteScript(const char* szScript,
                     CFXJSE_Value* lpRetValue,
                     CFXJSE_Value* lpNewThisObject);

 private:
  CFXJSE_Context(const CFXJSE_Context&) = delete;
  CFXJSE_Context& operator=(const CFXJSE_Context&) = delete;

  v8::Global<v8::Context> m_hContext;
  UnownedPtr<v8::Isolate> m_pIsolate;
  std::vector<std::unique_ptr<CFXJSE_Class>> m_rgClasses;
};

void FXJSE_UpdateObjectBinding(v8::Local<v8::Object> hObject,
                               CFXJSE_HostObject* lpNewBinding);

CFXJSE_HostObject* FXJSE_RetrieveObjectBinding(v8::Local<v8::Object> hJSObject);

#endif  // FXJS_XFA_CFXJSE_CONTEXT_H_
