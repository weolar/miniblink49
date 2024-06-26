// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_comb.h"

#include "xfa/fxfa/parser/cxfa_comb.h"

CJX_Comb::CJX_Comb(CXFA_Comb* node) : CJX_Node(node) {}

CJX_Comb::~CJX_Comb() = default;

bool CJX_Comb::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_Comb::numberOfCells(CFXJSE_Value* pValue,
                             bool bSetting,
                             XFA_Attribute eAttribute) {
  ScriptAttributeInteger(pValue, bSetting, eAttribute);
}
