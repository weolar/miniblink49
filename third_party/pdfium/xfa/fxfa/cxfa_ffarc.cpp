// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffarc.h"

#include "xfa/fxfa/parser/cxfa_arc.h"
#include "xfa/fxfa/parser/cxfa_value.h"

CXFA_FFArc::CXFA_FFArc(CXFA_Node* pNode) : CXFA_FFWidget(pNode) {}

CXFA_FFArc::~CXFA_FFArc() {}

void CXFA_FFArc::RenderWidget(CXFA_Graphics* pGS,
                              const CFX_Matrix& matrix,
                              uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus))
    return;

  CXFA_Value* value = m_pNode->GetFormValueIfExists();
  if (!value)
    return;

  CFX_RectF rtArc = GetRectWithoutRotate();
  CXFA_Margin* margin = m_pNode->GetMarginIfExists();
  XFA_RectWithoutMargin(&rtArc, margin);

  CFX_Matrix mtRotate = GetRotateMatrix();
  mtRotate.Concat(matrix);

  DrawBorder(pGS, value->GetArcIfExists(), rtArc, mtRotate);
}
