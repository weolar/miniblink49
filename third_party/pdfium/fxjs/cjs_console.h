// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_CJS_CONSOLE_H_
#define FXJS_CJS_CONSOLE_H_

#include <vector>

#include "fxjs/cjs_object.h"
#include "fxjs/js_define.h"

class CJS_Console final : public CJS_Object {
 public:
  static int GetObjDefnID();
  static void DefineJSObjects(CFXJS_Engine* pEngine);

  CJS_Console(v8::Local<v8::Object> pObject, CJS_Runtime* pRuntime);
  ~CJS_Console() override;

  JS_STATIC_METHOD(clear, CJS_Console);
  JS_STATIC_METHOD(hide, CJS_Console);
  JS_STATIC_METHOD(println, CJS_Console);
  JS_STATIC_METHOD(show, CJS_Console);

 private:
  static int ObjDefnID;
  static const char kName[];
  static const JSMethodSpec MethodSpecs[];

  CJS_Result clear(CJS_Runtime* pRuntime,
                   const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result hide(CJS_Runtime* pRuntime,
                  const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result println(CJS_Runtime* pRuntime,
                     const std::vector<v8::Local<v8::Value>>& params);
  CJS_Result show(CJS_Runtime* pRuntime,
                  const std::vector<v8::Local<v8::Value>>& params);
};

#endif  // FXJS_CJS_CONSOLE_H_
