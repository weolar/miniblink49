// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_manifest.h"

#include <vector>

#include "fxjs/cfx_v8.h"
#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/parser/cxfa_manifest.h"

const CJX_MethodSpec CJX_Manifest::MethodSpecs[] = {
    {"evaluate", evaluate_static}};

CJX_Manifest::CJX_Manifest(CXFA_Manifest* manifest) : CJX_Node(manifest) {
  DefineMethods(MethodSpecs);
}

CJX_Manifest::~CJX_Manifest() {}

bool CJX_Manifest::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_Manifest::evaluate(
    CFX_V8* runtime,
    const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  return CJS_Result::Success(
      runtime->NewBoolean(GetXFANode()->IsWidgetReady()));
}

void CJX_Manifest::defaultValue(CFXJSE_Value* pValue,
                                bool bSetting,
                                XFA_Attribute eAttribute) {
  ScriptSomDefaultValue(pValue, bSetting, eAttribute);
}
