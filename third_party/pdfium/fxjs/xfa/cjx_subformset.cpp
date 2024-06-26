// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_subformset.h"

#include "xfa/fxfa/parser/cxfa_subformset.h"

CJX_SubformSet::CJX_SubformSet(CXFA_SubformSet* node) : CJX_Container(node) {}

CJX_SubformSet::~CJX_SubformSet() = default;

bool CJX_SubformSet::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_SubformSet::instanceIndex(CFXJSE_Value* pValue,
                                   bool bSetting,
                                   XFA_Attribute eAttribute) {
  ScriptSomInstanceIndex(pValue, bSetting, eAttribute);
}
