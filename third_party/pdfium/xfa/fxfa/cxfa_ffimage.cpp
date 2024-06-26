// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffimage.h"

#include "core/fxge/dib/cfx_dibitmap.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/parser/cxfa_image.h"
#include "xfa/fxfa/parser/cxfa_para.h"
#include "xfa/fxfa/parser/cxfa_value.h"

CXFA_FFImage::CXFA_FFImage(CXFA_Node* pNode) : CXFA_FFWidget(pNode) {}

CXFA_FFImage::~CXFA_FFImage() {
  GetNode()->SetImageImage(nullptr);
}

bool CXFA_FFImage::IsLoaded() {
  return !!GetNode()->GetImageImage();
}

bool CXFA_FFImage::LoadWidget() {
  if (GetNode()->GetImageImage())
    return true;

  return GetNode()->LoadImageImage(GetDoc()) && CXFA_FFWidget::LoadWidget();
}

void CXFA_FFImage::RenderWidget(CXFA_Graphics* pGS,
                                const CFX_Matrix& matrix,
                                uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus))
    return;

  CFX_Matrix mtRotate = GetRotateMatrix();
  mtRotate.Concat(matrix);

  CXFA_FFWidget::RenderWidget(pGS, mtRotate, dwStatus);

  RetainPtr<CFX_DIBitmap> pDIBitmap = GetNode()->GetImageImage();
  if (!pDIBitmap)
    return;

  CFX_RectF rtImage = GetRectWithoutRotate();
  CXFA_Margin* margin = m_pNode->GetMarginIfExists();
  XFA_RectWithoutMargin(&rtImage, margin);

  XFA_AttributeValue iHorzAlign = XFA_AttributeValue::Left;
  XFA_AttributeValue iVertAlign = XFA_AttributeValue::Top;
  CXFA_Para* para = m_pNode->GetParaIfExists();
  if (para) {
    iHorzAlign = para->GetHorizontalAlign();
    iVertAlign = para->GetVerticalAlign();
  }

  auto* value = m_pNode->GetFormValueIfExists();
  CXFA_Image* image = value ? value->GetImageIfExists() : nullptr;
  if (image) {
    XFA_DrawImage(pGS, rtImage, mtRotate, pDIBitmap, image->GetAspect(),
                  m_pNode->GetImageDpi(), iHorzAlign, iVertAlign);
  }
}
