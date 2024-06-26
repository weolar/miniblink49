// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_integer.h"

#include "xfa/fxfa/parser/cxfa_integer.h"

CJX_Integer::CJX_Integer(CXFA_Integer* node) : CJX_Content(node) {}

CJX_Integer::~CJX_Integer() = default;

bool CJX_Integer::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_Integer::defaultValue(CFXJSE_Value* pValue,
                               bool bSetting,
                               XFA_Attribute eAttribute) {
  ScriptSomDefaultValue(pValue, bSetting, eAttribute);
}

void CJX_Integer::value(CFXJSE_Value* pValue,
                        bool bSetting,
                        XFA_Attribute eAttribute) {
  defaultValue(pValue, bSetting, eAttribute);
}
