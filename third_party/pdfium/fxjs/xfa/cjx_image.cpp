// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_image.h"

#include "xfa/fxfa/parser/cxfa_image.h"

CJX_Image::CJX_Image(CXFA_Image* node) : CJX_Node(node) {}

CJX_Image::~CJX_Image() = default;

bool CJX_Image::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_Image::defaultValue(CFXJSE_Value* pValue,
                             bool bSetting,
                             XFA_Attribute eAttribute) {
  ScriptSomDefaultValue_Read(pValue, bSetting, eAttribute);
}

void CJX_Image::value(CFXJSE_Value* pValue,
                      bool bSetting,
                      XFA_Attribute eAttribute) {
  ScriptSomDefaultValue_Read(pValue, bSetting, eAttribute);
}
