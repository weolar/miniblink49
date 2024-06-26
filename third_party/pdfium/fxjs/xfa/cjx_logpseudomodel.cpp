// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_logpseudomodel.h"

#include <vector>

#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/parser/cscript_logpseudomodel.h"

const CJX_MethodSpec CJX_LogPseudoModel::MethodSpecs[] = {
    {"message", message_static},
    {"traceEnabled", traceEnabled_static},
    {"traceActivate", traceActivate_static},
    {"traceDeactivate", traceDeactivate_static},
    {"trace", trace_static}};

CJX_LogPseudoModel::CJX_LogPseudoModel(CScript_LogPseudoModel* model)
    : CJX_Object(model) {
  DefineMethods(MethodSpecs);
}

CJX_LogPseudoModel::~CJX_LogPseudoModel() {}

bool CJX_LogPseudoModel::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_LogPseudoModel::message(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  // Uncomment to allow using xfa.log.message(""); from JS.
  // fprintf(stderr, "LOG\n");
  // for (auto& val : params) {
  //   v8::String::Utf8Value str(runtime->GetIsolate(), val);
  //   fprintf(stderr, "  %ls\n", WideString::FromUTF8(*str).c_str());
  // }

  return CJS_Result::Success();
}

CJS_Result CJX_LogPseudoModel::traceEnabled(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJX_LogPseudoModel::traceActivate(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJX_LogPseudoModel::traceDeactivate(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}

CJS_Result CJX_LogPseudoModel::trace(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  return CJS_Result::Success();
}
