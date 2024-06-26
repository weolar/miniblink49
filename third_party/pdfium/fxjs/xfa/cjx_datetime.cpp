// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_datetime.h"

#include "xfa/fxfa/parser/cxfa_datetime.h"

CJX_DateTime::CJX_DateTime(CXFA_DateTime* node) : CJX_Node(node) {}

CJX_DateTime::~CJX_DateTime() = default;

bool CJX_DateTime::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_DateTime::defaultValue(CFXJSE_Value* pValue,
                                bool bSetting,
                                XFA_Attribute eAttribute) {
  ScriptSomDefaultValue(pValue, bSetting, eAttribute);
}

void CJX_DateTime::value(CFXJSE_Value* pValue,
                         bool bSetting,
                         XFA_Attribute eAttribute) {
  ScriptSomDefaultValue(pValue, bSetting, eAttribute);
}
