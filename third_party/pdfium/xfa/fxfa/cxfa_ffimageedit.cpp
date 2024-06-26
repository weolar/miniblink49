// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffimageedit.h"

#include <utility>

#include "core/fxge/dib/cfx_dibitmap.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_picturebox.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_fffield.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/parser/cxfa_border.h"
#include "xfa/fxfa/parser/cxfa_image.h"
#include "xfa/fxfa/parser/cxfa_para.h"
#include "xfa/fxfa/parser/cxfa_value.h"

CXFA_FFImageEdit::CXFA_FFImageEdit(CXFA_Node* pNode)
    : CXFA_FFField(pNode), m_pOldDelegate(nullptr) {}

CXFA_FFImageEdit::~CXFA_FFImageEdit() {
  m_pNode->SetImageEditImage(nullptr);
}

bool CXFA_FFImageEdit::LoadWidget() {
  auto pNew = pdfium::MakeUnique<CFWL_PictureBox>(GetFWLApp());
  CFWL_PictureBox* pPictureBox = pNew.get();
  m_pNormalWidget = std::move(pNew);
  m_pNormalWidget->SetLayoutItem(this);

  CFWL_NoteDriver* pNoteDriver =
      m_pNormalWidget->GetOwnerApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(m_pNormalWidget.get(),
                                   m_pNormalWidget.get());
  m_pOldDelegate = pPictureBox->GetDelegate();
  pPictureBox->SetDelegate(this);

  CXFA_FFField::LoadWidget();
  if (!m_pNode->GetImageEditImage())
    UpdateFWLData();

  return true;
}

void CXFA_FFImageEdit::RenderWidget(CXFA_Graphics* pGS,
                                    const CFX_Matrix& matrix,
                                    uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus))
    return;

  CFX_Matrix mtRotate = GetRotateMatrix();
  mtRotate.Concat(matrix);

  CXFA_FFWidget::RenderWidget(pGS, mtRotate, dwStatus);
  DrawBorder(pGS, m_pNode->GetUIBorder(), m_rtUI, mtRotate);
  RenderCaption(pGS, &mtRotate);
  RetainPtr<CFX_DIBitmap> pDIBitmap = m_pNode->GetImageEditImage();
  if (!pDIBitmap)
    return;

  CFX_RectF rtImage = m_pNormalWidget->GetWidgetRect();
  XFA_AttributeValue iHorzAlign = XFA_AttributeValue::Left;
  XFA_AttributeValue iVertAlign = XFA_AttributeValue::Top;
  CXFA_Para* para = m_pNode->GetParaIfExists();
  if (para) {
    iHorzAlign = para->GetHorizontalAlign();
    iVertAlign = para->GetVerticalAlign();
  }

  XFA_AttributeValue iAspect = XFA_AttributeValue::Fit;
  CXFA_Value* value = m_pNode->GetFormValueIfExists();
  if (value) {
    CXFA_Image* image = value->GetImageIfExists();
    if (image)
      iAspect = image->GetAspect();
  }

  XFA_DrawImage(pGS, rtImage, mtRotate, pDIBitmap, iAspect,
                m_pNode->GetImageEditDpi(), iHorzAlign, iVertAlign);
}

bool CXFA_FFImageEdit::AcceptsFocusOnButtonDown(uint32_t dwFlags,
                                                const CFX_PointF& point,
                                                FWL_MouseCommand command) {
  if (command != FWL_MouseCommand::LeftButtonDown)
    return CXFA_FFField::AcceptsFocusOnButtonDown(dwFlags, point, command);

  if (!m_pNode->IsOpenAccess())
    return false;
  if (!PtInActiveRect(point))
    return false;

  return true;
}

void CXFA_FFImageEdit::OnLButtonDown(uint32_t dwFlags,
                                     const CFX_PointF& point) {
  SetButtonDown(true);

  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::LeftButtonDown;
  ms.m_dwFlags = dwFlags;
  ms.m_pos = FWLToClient(point);
  TranslateFWLMessage(&ms);
}

void CXFA_FFImageEdit::SetFWLRect() {
  if (!m_pNormalWidget)
    return;

  CFX_RectF rtUIMargin = m_pNode->GetUIMargin();
  CFX_RectF rtImage(m_rtUI);
  rtImage.Deflate(rtUIMargin.left, rtUIMargin.top, rtUIMargin.width,
                  rtUIMargin.height);
  m_pNormalWidget->SetWidgetRect(rtImage);
}

bool CXFA_FFImageEdit::CommitData() {
  return true;
}

bool CXFA_FFImageEdit::UpdateFWLData() {
  m_pNode->SetImageEditImage(nullptr);
  m_pNode->LoadImageEditImage(GetDoc());
  return true;
}

void CXFA_FFImageEdit::OnProcessMessage(CFWL_Message* pMessage) {
  m_pOldDelegate->OnProcessMessage(pMessage);
}

void CXFA_FFImageEdit::OnProcessEvent(CFWL_Event* pEvent) {
  CXFA_FFField::OnProcessEvent(pEvent);
  m_pOldDelegate->OnProcessEvent(pEvent);
}

void CXFA_FFImageEdit::OnDrawWidget(CXFA_Graphics* pGraphics,
                                    const CFX_Matrix& matrix) {
  m_pOldDelegate->OnDrawWidget(pGraphics, matrix);
}

FormFieldType CXFA_FFImageEdit::GetFormFieldType() {
  return FormFieldType::kXFA_ImageField;
}
