// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/js_define.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "core/fxcrt/fx_extension.h"
#include "fxjs/cjs_document.h"
#include "fxjs/cjs_object.h"
#include "fxjs/fx_date_helpers.h"

void JSDestructor(v8::Local<v8::Object> obj) {
  CFXJS_Engine::SetObjectPrivate(obj, nullptr);
}

double JS_DateParse(const WideString& str) {
  v8::Isolate* pIsolate = v8::Isolate::GetCurrent();
  v8::Isolate::Scope isolate_scope(pIsolate);
  v8::HandleScope scope(pIsolate);

  v8::Local<v8::Context> context = pIsolate->GetCurrentContext();

  // Use the built-in object method.
  v8::Local<v8::Value> v =
      context->Global()
          ->Get(context, v8::String::NewFromUtf8(pIsolate, "Date",
                                                 v8::NewStringType::kNormal)
                             .ToLocalChecked())
          .ToLocalChecked();
  if (v->IsObject()) {
    v8::Local<v8::Object> o = v->ToObject(context).ToLocalChecked();
    v = o->Get(context, v8::String::NewFromUtf8(pIsolate, "parse",
                                                v8::NewStringType::kNormal)
                            .ToLocalChecked())
            .ToLocalChecked();
    if (v->IsFunction()) {
      v8::Local<v8::Function> funC = v8::Local<v8::Function>::Cast(v);
      const int argc = 1;
      v8::Local<v8::Value> timeStr =
          v8::String::NewFromUtf8(pIsolate,
                                  FX_UTF8Encode(str.AsStringView()).c_str(),
                                  v8::NewStringType::kNormal)
              .ToLocalChecked();
      v8::Local<v8::Value> argv[argc] = {timeStr};
      v = funC->Call(context, context->Global(), argc, argv).ToLocalChecked();
      if (v->IsNumber()) {
        double date = v->ToNumber(context).ToLocalChecked()->Value();
        if (!std::isfinite(date))
          return date;
        return FX_LocalTime(date);
      }
    }
  }
  return 0;
}

std::vector<v8::Local<v8::Value>> ExpandKeywordParams(
    CJS_Runtime* pRuntime,
    const std::vector<v8::Local<v8::Value>>& originals,
    size_t nKeywords,
    ...) {
  ASSERT(nKeywords);

  std::vector<v8::Local<v8::Value>> result(nKeywords, v8::Local<v8::Value>());
  size_t size = std::min(originals.size(), nKeywords);
  for (size_t i = 0; i < size; ++i)
    result[i] = originals[i];

  if (originals.size() != 1 || !originals[0]->IsObject() ||
      originals[0]->IsArray()) {
    return result;
  }
  result[0] = v8::Local<v8::Value>();  // Make unknown.

  v8::Local<v8::Object> pObj = pRuntime->ToObject(originals[0]);
  va_list ap;
  va_start(ap, nKeywords);
  for (size_t i = 0; i < nKeywords; ++i) {
    const char* property = va_arg(ap, const char*);
    v8::Local<v8::Value> v8Value = pRuntime->GetObjectProperty(pObj, property);
    if (!v8Value->IsUndefined())
      result[i] = v8Value;
  }
  va_end(ap);

  return result;
}

bool IsExpandedParamKnown(v8::Local<v8::Value> value) {
  return !value.IsEmpty() &&
         (value->IsString() || value->IsNumber() || value->IsBoolean() ||
          value->IsDate() || value->IsObject() || value->IsNull() ||
          value->IsUndefined());
}
