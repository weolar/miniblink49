// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_date.h"

#include "xfa/fxfa/parser/cxfa_date.h"

CJX_Date::CJX_Date(CXFA_Date* node) : CJX_Content(node) {}

CJX_Date::~CJX_Date() = default;

bool CJX_Date::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_Date::defaultValue(CFXJSE_Value* pValue,
                            bool bSetting,
                            XFA_Attribute eAttribute) {
  ScriptSomDefaultValue(pValue, bSetting, eAttribute);
}

void CJX_Date::value(CFXJSE_Value* pValue,
                     bool bSetting,
                     XFA_Attribute eAttribute) {
  defaultValue(pValue, bSetting, eAttribute);
}
