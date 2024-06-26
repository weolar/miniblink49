// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxjs/xfa/cjx_boolean.h"

#include "fxjs/xfa/cfxjse_value.h"
#include "xfa/fxfa/parser/cxfa_boolean.h"

CJX_Boolean::CJX_Boolean(CXFA_Boolean* node) : CJX_Content(node) {}

CJX_Boolean::~CJX_Boolean() = default;

bool CJX_Boolean::DynamicTypeIs(TypeTag eType) const {
  return eType == static_type__ || ParentType__::DynamicTypeIs(eType);
}

void CJX_Boolean::defaultValue(CFXJSE_Value* pValue,
                               bool bSetting,
                               XFA_Attribute eAttribute) {
  if (!bSetting) {
    pValue->SetBoolean(GetContent(true).EqualsASCII("1"));
    return;
  }

  ByteString newValue;
  if (pValue && !(pValue->IsNull() || pValue->IsUndefined()))
    newValue = pValue->ToString();

  int32_t iValue = FXSYS_atoi(newValue.c_str());
  WideString wsNewValue(iValue == 0 ? L"0" : L"1");
  WideString wsFormatValue(wsNewValue);
  CXFA_Node* pContainerNode = ToNode(GetXFAObject())->GetContainerNode();
  if (pContainerNode)
    wsFormatValue = pContainerNode->GetFormatDataValue(wsNewValue);

  SetContent(wsNewValue, wsFormatValue, true, true, true);
}

void CJX_Boolean::value(CFXJSE_Value* pValue,
                        bool bSetting,
                        XFA_Attribute eAttribute) {
  defaultValue(pValue, bSetting, eAttribute);
}
