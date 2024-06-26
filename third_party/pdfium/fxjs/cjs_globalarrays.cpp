// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/cjs_globalarrays.h"

#define GLOBAL_ARRAY(rt, name, ...)                                          \
  {                                                                          \
    static const wchar_t* const values[] = {__VA_ARGS__};                    \
    v8::Local<v8::Array> array = (rt)->NewArray();                           \
    v8::Local<v8::Context> ctx = (rt)->GetIsolate()->GetCurrentContext();    \
    for (size_t i = 0; i < FX_ArraySize(values); ++i)                        \
      array->Set(ctx, i, (rt)->NewString(values[i])).FromJust();             \
    (rt)->SetConstArray((name), array);                                      \
    (rt)->DefineGlobalConst(                                                 \
        (name), [](const v8::FunctionCallbackInfo<v8::Value>& info) {        \
          CJS_Object* pObj = CFXJS_Engine::GetObjectPrivate(info.Holder());  \
          CJS_Runtime* pCurrentRuntime = pObj->GetRuntime();                 \
          if (pCurrentRuntime)                                               \
            info.GetReturnValue().Set(pCurrentRuntime->GetConstArray(name)); \
        });                                                                  \
  }

// static
void CJS_GlobalArrays::DefineJSObjects(CJS_Runtime* pRuntime) {
  GLOBAL_ARRAY(pRuntime, L"RE_NUMBER_ENTRY_DOT_SEP", L"[+-]?\\d*\\.?\\d*");
  GLOBAL_ARRAY(pRuntime, L"RE_NUMBER_COMMIT_DOT_SEP",
               L"[+-]?\\d+(\\.\\d+)?",  // -1.0 or -1
               L"[+-]?\\.\\d+",         // -.1
               L"[+-]?\\d+\\.");        // -1.

  GLOBAL_ARRAY(pRuntime, L"RE_NUMBER_ENTRY_COMMA_SEP", L"[+-]?\\d*,?\\d*");
  GLOBAL_ARRAY(pRuntime, L"RE_NUMBER_COMMIT_COMMA_SEP",
               L"[+-]?\\d+([.,]\\d+)?",  // -1,0 or -1
               L"[+-]?[.,]\\d+",         // -,1
               L"[+-]?\\d+[.,]");        // -1,

  GLOBAL_ARRAY(pRuntime, L"RE_ZIP_ENTRY", L"\\d{0,5}");
  GLOBAL_ARRAY(pRuntime, L"RE_ZIP_COMMIT", L"\\d{5}");
  GLOBAL_ARRAY(pRuntime, L"RE_ZIP4_ENTRY", L"\\d{0,5}(\\.|[- ])?\\d{0,4}");
  GLOBAL_ARRAY(pRuntime, L"RE_ZIP4_COMMIT", L"\\d{5}(\\.|[- ])?\\d{4}");
  GLOBAL_ARRAY(pRuntime, L"RE_PHONE_ENTRY",
               // 555-1234 or 408 555-1234
               L"\\d{0,3}(\\.|[- ])?\\d{0,3}(\\.|[- ])?\\d{0,4}",

               // (408
               L"\\(\\d{0,3}",

               // (408) 555-1234
               // (allow the addition of parens as an afterthought)
               L"\\(\\d{0,3}\\)(\\.|[- ])?\\d{0,3}(\\.|[- ])?\\d{0,4}",

               // (408 555-1234
               L"\\(\\d{0,3}(\\.|[- ])?\\d{0,3}(\\.|[- ])?\\d{0,4}",

               // 408) 555-1234
               L"\\d{0,3}\\)(\\.|[- ])?\\d{0,3}(\\.|[- ])?\\d{0,4}",

               // international
               L"011(\\.|[- \\d])*");

  GLOBAL_ARRAY(
      pRuntime, L"RE_PHONE_COMMIT", L"\\d{3}(\\.|[- ])?\\d{4}",  // 555-1234
      L"\\d{3}(\\.|[- ])?\\d{3}(\\.|[- ])?\\d{4}",               // 408 555-1234
      L"\\(\\d{3}\\)(\\.|[- ])?\\d{3}(\\.|[- ])?\\d{4}",  // (408) 555-1234
      L"011(\\.|[- \\d])*");                              // international

  GLOBAL_ARRAY(pRuntime, L"RE_SSN_ENTRY",
               L"\\d{0,3}(\\.|[- ])?\\d{0,2}(\\.|[- ])?\\d{0,4}");

  GLOBAL_ARRAY(pRuntime, L"RE_SSN_COMMIT",
               L"\\d{3}(\\.|[- ])?\\d{2}(\\.|[- ])?\\d{4}");
}
