// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffpushbutton.h"

#include <utility>

#include "third_party/base/ptr_util.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_pushbutton.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_fffield.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_textlayout.h"
#include "xfa/fxfa/cxfa_textprovider.h"
#include "xfa/fxfa/parser/cxfa_border.h"
#include "xfa/fxfa/parser/cxfa_button.h"
#include "xfa/fxfa/parser/cxfa_caption.h"
#include "xfa/fxfa/parser/cxfa_edge.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

CXFA_FFPushButton::CXFA_FFPushButton(CXFA_Node* pNode, CXFA_Button* button)
    : CXFA_FFField(pNode), button_(button) {}

CXFA_FFPushButton::~CXFA_FFPushButton() = default;

void CXFA_FFPushButton::RenderWidget(CXFA_Graphics* pGS,
                                     const CFX_Matrix& matrix,
                                     uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus))
    return;

  CFX_Matrix mtRotate = GetRotateMatrix();
  mtRotate.Concat(matrix);

  CXFA_FFWidget::RenderWidget(pGS, mtRotate, dwStatus);
  RenderHighlightCaption(pGS, &mtRotate);

  CFX_RectF rtWidget = GetRectWithoutRotate();
  CFX_Matrix mt(1, 0, 0, 1, rtWidget.left, rtWidget.top);
  mt.Concat(mtRotate);
  GetApp()->GetFWLWidgetMgr()->OnDrawWidget(m_pNormalWidget.get(), pGS, mt);
}

bool CXFA_FFPushButton::LoadWidget() {
  ASSERT(!m_pNormalWidget);
  auto pNew = pdfium::MakeUnique<CFWL_PushButton>(GetFWLApp());
  CFWL_PushButton* pPushButton = pNew.get();
  m_pOldDelegate = pPushButton->GetDelegate();
  pPushButton->SetDelegate(this);
  m_pNormalWidget = std::move(pNew);
  m_pNormalWidget->SetLayoutItem(this);

  CFWL_NoteDriver* pNoteDriver =
      m_pNormalWidget->GetOwnerApp()->GetNoteDriver();
  pNoteDriver->RegisterEventTarget(m_pNormalWidget.get(),
                                   m_pNormalWidget.get());
  m_pNormalWidget->LockUpdate();
  UpdateWidgetProperty();
  LoadHighlightCaption();
  m_pNormalWidget->UnlockUpdate();
  return CXFA_FFField::LoadWidget();
}

void CXFA_FFPushButton::UpdateWidgetProperty() {
  uint32_t dwStyleEx = 0;
  switch (button_->GetHighlight()) {
    case XFA_AttributeValue::Inverted:
      dwStyleEx = XFA_FWL_PSBSTYLEEXT_HiliteInverted;
      break;
    case XFA_AttributeValue::Outline:
      dwStyleEx = XFA_FWL_PSBSTYLEEXT_HiliteOutLine;
      break;
    case XFA_AttributeValue::Push:
      dwStyleEx = XFA_FWL_PSBSTYLEEXT_HilitePush;
      break;
    default:
      break;
  }
  m_pNormalWidget->ModifyStylesEx(dwStyleEx, 0xFFFFFFFF);
}

bool CXFA_FFPushButton::PerformLayout() {
  CXFA_FFWidget::PerformLayout();
  CFX_RectF rtWidget = GetRectWithoutRotate();

  m_rtUI = rtWidget;
  CXFA_Margin* margin = m_pNode->GetMarginIfExists();
  XFA_RectWithoutMargin(&rtWidget, margin);

  m_rtCaption = rtWidget;

  CXFA_Caption* caption = m_pNode->GetCaptionIfExists();
  CXFA_Margin* captionMargin = caption ? caption->GetMarginIfExists() : nullptr;
  XFA_RectWithoutMargin(&m_rtCaption, captionMargin);

  LayoutHighlightCaption();
  SetFWLRect();
  if (m_pNormalWidget)
    m_pNormalWidget->Update();

  return true;
}

float CXFA_FFPushButton::GetLineWidth() {
  CXFA_Border* border = m_pNode->GetBorderIfExists();
  if (border && border->GetPresence() == XFA_AttributeValue::Visible) {
    CXFA_Edge* edge = border->GetEdgeIfExists(0);
    return edge ? edge->GetThickness() : 0;
  }
  return 0;
}

FX_ARGB CXFA_FFPushButton::GetLineColor() {
  return 0xFF000000;
}

FX_ARGB CXFA_FFPushButton::GetFillColor() {
  return 0xFFFFFFFF;
}

void CXFA_FFPushButton::LoadHighlightCaption() {
  CXFA_Caption* caption = m_pNode->GetCaptionIfExists();
  if (!caption || caption->IsHidden())
    return;

  if (m_pNode->HasButtonRollover()) {
    if (!m_pRollProvider) {
      m_pRollProvider = pdfium::MakeUnique<CXFA_TextProvider>(
          m_pNode.Get(), XFA_TEXTPROVIDERTYPE_Rollover);
    }
    m_pRolloverTextLayout =
        pdfium::MakeUnique<CXFA_TextLayout>(GetDoc(), m_pRollProvider.get());
  }

  if (m_pNode->HasButtonDown()) {
    if (!m_pDownProvider) {
      m_pDownProvider = pdfium::MakeUnique<CXFA_TextProvider>(
          m_pNode.Get(), XFA_TEXTPROVIDERTYPE_Down);
    }
    m_pDownTextLayout =
        pdfium::MakeUnique<CXFA_TextLayout>(GetDoc(), m_pDownProvider.get());
  }
}

void CXFA_FFPushButton::LayoutHighlightCaption() {
  CFX_SizeF sz(m_rtCaption.width, m_rtCaption.height);
  LayoutCaption();
  if (m_pRolloverTextLayout)
    m_pRolloverTextLayout->Layout(sz);
  if (m_pDownTextLayout)
    m_pDownTextLayout->Layout(sz);
}

void CXFA_FFPushButton::RenderHighlightCaption(CXFA_Graphics* pGS,
                                               CFX_Matrix* pMatrix) {
  CXFA_TextLayout* pCapTextLayout = m_pNode->GetCaptionTextLayout();
  CXFA_Caption* caption = m_pNode->GetCaptionIfExists();
  if (!caption || !caption->IsVisible())
    return;

  CFX_RenderDevice* pRenderDevice = pGS->GetRenderDevice();
  CFX_RectF rtClip = m_rtCaption;
  rtClip.Intersect(GetRectWithoutRotate());
  CFX_Matrix mt(1, 0, 0, 1, m_rtCaption.left, m_rtCaption.top);
  if (pMatrix) {
    rtClip = pMatrix->TransformRect(rtClip);
    mt.Concat(*pMatrix);
  }

  uint32_t dwState = m_pNormalWidget->GetStates();
  if (m_pDownTextLayout && (dwState & FWL_STATE_PSB_Pressed) &&
      (dwState & FWL_STATE_PSB_Hovered)) {
    if (m_pDownTextLayout->DrawString(pRenderDevice, mt, rtClip, 0))
      return;
  } else if (m_pRolloverTextLayout && (dwState & FWL_STATE_PSB_Hovered)) {
    if (m_pRolloverTextLayout->DrawString(pRenderDevice, mt, rtClip, 0))
      return;
  }

  if (pCapTextLayout)
    pCapTextLayout->DrawString(pRenderDevice, mt, rtClip, 0);
}

void CXFA_FFPushButton::OnProcessMessage(CFWL_Message* pMessage) {
  m_pOldDelegate->OnProcessMessage(pMessage);
}

void CXFA_FFPushButton::OnProcessEvent(CFWL_Event* pEvent) {
  m_pOldDelegate->OnProcessEvent(pEvent);
  CXFA_FFField::OnProcessEvent(pEvent);
}

void CXFA_FFPushButton::OnDrawWidget(CXFA_Graphics* pGraphics,
                                     const CFX_Matrix& matrix) {
  if (m_pNormalWidget->GetStylesEx() & XFA_FWL_PSBSTYLEEXT_HiliteInverted) {
    if ((m_pNormalWidget->GetStates() & FWL_STATE_PSB_Pressed) &&
        (m_pNormalWidget->GetStates() & FWL_STATE_PSB_Hovered)) {
      CFX_RectF rtFill(0, 0, m_pNormalWidget->GetWidgetRect().Size());
      float fLineWith = GetLineWidth();
      rtFill.Deflate(fLineWith, fLineWith);
      CXFA_GEPath path;
      path.AddRectangle(rtFill.left, rtFill.top, rtFill.width, rtFill.height);
      pGraphics->SetFillColor(CXFA_GEColor(ArgbEncode(128, 128, 255, 255)));
      pGraphics->FillPath(&path, FXFILL_WINDING, &matrix);
    }
    return;
  }

  if (m_pNormalWidget->GetStylesEx() & XFA_FWL_PSBSTYLEEXT_HiliteOutLine) {
    if ((m_pNormalWidget->GetStates() & FWL_STATE_PSB_Pressed) &&
        (m_pNormalWidget->GetStates() & FWL_STATE_PSB_Hovered)) {
      float fLineWidth = GetLineWidth();
      pGraphics->SetStrokeColor(CXFA_GEColor(ArgbEncode(255, 128, 255, 255)));
      pGraphics->SetLineWidth(fLineWidth);

      CXFA_GEPath path;
      CFX_RectF rect = m_pNormalWidget->GetWidgetRect();
      path.AddRectangle(0, 0, rect.width, rect.height);
      pGraphics->StrokePath(&path, &matrix);
    }
  }
}

FormFieldType CXFA_FFPushButton::GetFormFieldType() {
  return FormFieldType::kXFA_PushButton;
}
