// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_value.h"

#include "xfa/fxfa/parser/cxfa_value.h"

CJX_Value::CJX_Value(CXFA_Value* node) : CJX_Node(node) {}

CJX_Value::~CJX_Value() = default;

bool CJX_Value::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_Value::override(CFXJSE_Value* pValue,
                         bool bSetting,
                         XFA_Attribute eAttribute) {
  ScriptAttributeBool(pValue, bSetting, eAttribute);
}
