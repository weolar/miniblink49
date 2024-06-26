// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_OBJECT_H_
#define FXJS_CJS_OBJECT_H_

#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fxjs/cjs_runtime.h"
#include "third_party/base/span.h"

class CFXJS_Engine;

struct JSConstSpec {
  enum Type { Number = 0, String = 1 };

  const char* pName;
  Type eType;
  double number;
  const char* pStr;
};

struct JSPropertySpec {
  const char* pName;
  v8::AccessorGetterCallback pPropGet;
  v8::AccessorSetterCallback pPropPut;
};

struct JSMethodSpec {
  const char* pName;
  v8::FunctionCallback pMethodCall;
};

class CJS_Object {
 public:
  static void DefineConsts(CFXJS_Engine* pEngine,
                           int objId,
                           pdfium::span<const JSConstSpec> consts);
  static void DefineProps(CFXJS_Engine* pEngine,
                          int objId,
                          pdfium::span<const JSPropertySpec> consts);
  static void DefineMethods(CFXJS_Engine* pEngine,
                            int objId,
                            pdfium::span<const JSMethodSpec> consts);

  CJS_Object(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime);
  virtual ~CJS_Object();

  v8::Local<v8::Object> ToV8Object() { return m_pV8Object.Get(GetIsolate()); }
  v8::Isolate* GetIsolate() const { return m_pIsolate.Get(); }
  CJS_Runtime* GetRuntime() const { return m_pRuntime.Get(); }

 private:
  UnownedPtr<v8::Isolate> m_pIsolate;
  v8::Global<v8::Object> m_pV8Object;
  CJS_Runtime::ObservedPtr m_pRuntime;
};

#endif  // FXJS_CJS_OBJECT_H_
