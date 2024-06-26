// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_delta.h"

#include <vector>

#include "fxjs/js_resources.h"
#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/parser/cxfa_delta.h"

const CJX_MethodSpec CJX_Delta::MethodSpecs[] = {{"restore", restore_static}};

CJX_Delta::CJX_Delta(CXFA_Delta* delta) : CJX_Object(delta) {
  DefineMethods(MethodSpecs);
}

CJX_Delta::~CJX_Delta() {}

bool CJX_Delta::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

CJS_Result CJX_Delta::restore(CFX_V8* runtime,
                              const std::vector<v8::Local<v8::Value>>& params) {
  if (!params.empty())
    return CJS_Result::Failure(JSMessage::kParamError);

  return CJS_Result::Success();
}

void CJX_Delta::currentValue(CFXJSE_Value* pValue,
                             bool bSetting,
                             XFA_Attribute eAttribute) {}

void CJX_Delta::savedValue(CFXJSE_Value* pValue,
                           bool bSetting,
                           XFA_Attribute eAttribute) {}

void CJX_Delta::target(CFXJSE_Value* pValue,
                       bool bSetting,
                       XFA_Attribute eAttribute) {}
