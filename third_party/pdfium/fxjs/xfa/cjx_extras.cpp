// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_extras.h"

#include "xfa/fxfa/parser/cxfa_extras.h"

CJX_Extras::CJX_Extras(CXFA_Extras* node) : CJX_Node(node) {}

CJX_Extras::~CJX_Extras() = default;

bool CJX_Extras::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_Extras::type(CFXJSE_Value* pValue,
                      bool bSetting,
                      XFA_Attribute eAttribute) {}
