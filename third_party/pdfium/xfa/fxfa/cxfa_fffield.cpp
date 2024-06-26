// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_fffield.h"

#include <algorithm>

#include "xfa/fwl/cfwl_edit.h"
#include "xfa/fwl/cfwl_eventmouse.h"
#include "xfa/fwl/cfwl_messagekey.h"
#include "xfa/fwl/cfwl_messagekillfocus.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_messagemousewheel.h"
#include "xfa/fwl/cfwl_messagesetfocus.h"
#include "xfa/fwl/cfwl_picturebox.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffdocview.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_fwltheme.h"
#include "xfa/fxfa/cxfa_textlayout.h"
#include "xfa/fxfa/parser/cxfa_border.h"
#include "xfa/fxfa/parser/cxfa_calculate.h"
#include "xfa/fxfa/parser/cxfa_caption.h"
#include "xfa/fxfa/parser/cxfa_margin.h"
#include "xfa/fxfa/parser/cxfa_node.h"
#include "xfa/fxfa/parser/cxfa_script.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

namespace {

CXFA_FFField* ToField(CXFA_LayoutItem* widget) {
  return static_cast<CXFA_FFField*>(widget);
}

}  // namespace

CXFA_FFField::CXFA_FFField(CXFA_Node* pNode) : CXFA_FFWidget(pNode) {}

CXFA_FFField::~CXFA_FFField() = default;

CFX_RectF CXFA_FFField::GetBBox(uint32_t dwStatus, FocusOption focus) {
  if (focus == kDoNotDrawFocus)
    return CXFA_FFWidget::GetBBox(dwStatus, kDoNotDrawFocus);

  switch (m_pNode->GetFFWidgetType()) {
    case XFA_FFWidgetType::kButton:
    case XFA_FFWidgetType::kCheckButton:
    case XFA_FFWidgetType::kImageEdit:
    case XFA_FFWidgetType::kSignature:
    case XFA_FFWidgetType::kChoiceList:
      return GetRotateMatrix().TransformRect(m_rtUI);
    default:
      return CFX_RectF();
  }
}

void CXFA_FFField::RenderWidget(CXFA_Graphics* pGS,
                                const CFX_Matrix& matrix,
                                uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus))
    return;

  CFX_Matrix mtRotate = GetRotateMatrix();
  mtRotate.Concat(matrix);

  CXFA_FFWidget::RenderWidget(pGS, mtRotate, dwStatus);
  DrawBorder(pGS, m_pNode->GetUIBorder(), m_rtUI, mtRotate);
  RenderCaption(pGS, &mtRotate);
  DrawHighlight(pGS, &mtRotate, dwStatus, false);

  CFX_RectF rtWidget = m_pNormalWidget->GetWidgetRect();
  CFX_Matrix mt(1, 0, 0, 1, rtWidget.left, rtWidget.top);
  mt.Concat(mtRotate);
  GetApp()->GetFWLWidgetMgr()->OnDrawWidget(m_pNormalWidget.get(), pGS, mt);
}

void CXFA_FFField::DrawHighlight(CXFA_Graphics* pGS,
                                 CFX_Matrix* pMatrix,
                                 uint32_t dwStatus,
                                 bool bEllipse) {
  if (m_rtUI.IsEmpty() || !GetDoc()->GetXFADoc()->IsInteractive())
    return;
  if (!(dwStatus & XFA_WidgetStatus_Highlight) || !m_pNode->IsOpenAccess())
    return;

  CXFA_FFDoc* pDoc = GetDoc();
  pGS->SetFillColor(
      CXFA_GEColor(pDoc->GetDocEnvironment()->GetHighlightColor(pDoc)));
  CXFA_GEPath path;
  if (bEllipse)
    path.AddEllipse(m_rtUI);
  else
    path.AddRectangle(m_rtUI.left, m_rtUI.top, m_rtUI.width, m_rtUI.height);

  pGS->FillPath(&path, FXFILL_WINDING, pMatrix);
}

void CXFA_FFField::DrawFocus(CXFA_Graphics* pGS, CFX_Matrix* pMatrix) {
  if (!(m_dwStatus & XFA_WidgetStatus_Focused))
    return;

  pGS->SetStrokeColor(CXFA_GEColor(0xFF000000));

  static constexpr float kDashPattern[2] = {1, 1};
  pGS->SetLineDash(0.0f, kDashPattern, FX_ArraySize(kDashPattern));
  pGS->SetLineWidth(0);

  CXFA_GEPath path;
  path.AddRectangle(m_rtUI.left, m_rtUI.top, m_rtUI.width, m_rtUI.height);
  pGS->StrokePath(&path, pMatrix);
}

void CXFA_FFField::SetFWLThemeProvider() {
  if (m_pNormalWidget)
    m_pNormalWidget->SetThemeProvider(GetApp()->GetFWLTheme(GetDoc()));
}

bool CXFA_FFField::IsLoaded() {
  return m_pNormalWidget && CXFA_FFWidget::IsLoaded();
}

bool CXFA_FFField::LoadWidget() {
  SetFWLThemeProvider();
  m_pNode->LoadCaption(GetDoc());
  PerformLayout();
  return true;
}

void CXFA_FFField::SetEditScrollOffset() {
  XFA_FFWidgetType eType = m_pNode->GetFFWidgetType();
  if (eType != XFA_FFWidgetType::kTextEdit &&
      eType != XFA_FFWidgetType::kNumericEdit &&
      eType != XFA_FFWidgetType::kPasswordEdit) {
    return;
  }

  float fScrollOffset = 0;
  CXFA_FFField* pPrev = ToField(GetPrev());
  if (pPrev)
    fScrollOffset = -(m_pNode->GetUIMargin().top);

  while (pPrev) {
    fScrollOffset += pPrev->m_rtUI.height;
    pPrev = ToField(pPrev->GetPrev());
  }
  static_cast<CFWL_Edit*>(m_pNormalWidget.get())
      ->SetScrollOffset(fScrollOffset);
}

bool CXFA_FFField::PerformLayout() {
  CXFA_FFWidget::PerformLayout();
  CapPlacement();
  LayoutCaption();
  SetFWLRect();
  SetEditScrollOffset();
  if (m_pNormalWidget)
    m_pNormalWidget->Update();
  return true;
}

void CXFA_FFField::CapPlacement() {
  CFX_RectF rtWidget = GetRectWithoutRotate();
  CXFA_Margin* margin = m_pNode->GetMarginIfExists();
  if (margin) {
    CXFA_ContentLayoutItem* pItem = this;
    float fLeftInset = margin->GetLeftInset();
    float fRightInset = margin->GetRightInset();
    float fTopInset = margin->GetTopInset();
    float fBottomInset = margin->GetBottomInset();
    if (!pItem->GetPrev() && !pItem->GetNext()) {
      rtWidget.Deflate(fLeftInset, fTopInset, fRightInset, fBottomInset);
    } else {
      if (!pItem->GetPrev())
        rtWidget.Deflate(fLeftInset, fTopInset, fRightInset, 0);
      else if (!pItem->GetNext())
        rtWidget.Deflate(fLeftInset, 0, fRightInset, fBottomInset);
      else
        rtWidget.Deflate(fLeftInset, 0, fRightInset, 0);
    }
  }

  XFA_AttributeValue iCapPlacement = XFA_AttributeValue::Unknown;
  float fCapReserve = 0;
  CXFA_Caption* caption = m_pNode->GetCaptionIfExists();
  if (caption && !caption->IsHidden()) {
    iCapPlacement = caption->GetPlacementType();
    if ((iCapPlacement == XFA_AttributeValue::Top && GetPrev()) ||
        (iCapPlacement == XFA_AttributeValue::Bottom && GetNext())) {
      m_rtCaption = CFX_RectF();
    } else {
      fCapReserve = caption->GetReserve();
      if (iCapPlacement == XFA_AttributeValue::Top ||
          iCapPlacement == XFA_AttributeValue::Bottom) {
        fCapReserve = std::min(fCapReserve, rtWidget.height);
      } else {
        fCapReserve = std::min(fCapReserve, rtWidget.width);
      }
      CXFA_ContentLayoutItem* pItem = this;
      if (!pItem->GetPrev() && !pItem->GetNext()) {
        m_rtCaption = rtWidget;
      } else {
        pItem = pItem->GetFirst();
        m_rtCaption = pItem->GetRect(false);
        pItem = pItem->GetNext();
        while (pItem) {
          m_rtCaption.height += pItem->GetRect(false).Height();
          pItem = pItem->GetNext();
        }
        XFA_RectWithoutMargin(&m_rtCaption, margin);
      }

      CXFA_TextLayout* pCapTextLayout = m_pNode->GetCaptionTextLayout();
      if (fCapReserve <= 0 && pCapTextLayout) {
        CFX_SizeF minSize;
        CFX_SizeF maxSize;
        CFX_SizeF size = pCapTextLayout->CalcSize(minSize, maxSize);
        if (iCapPlacement == XFA_AttributeValue::Top ||
            iCapPlacement == XFA_AttributeValue::Bottom) {
          fCapReserve = size.height;
        } else {
          fCapReserve = size.width;
        }
      }
    }
  }

  m_rtUI = rtWidget;
  CXFA_Margin* capMargin = caption ? caption->GetMarginIfExists() : nullptr;
  switch (iCapPlacement) {
    case XFA_AttributeValue::Left: {
      m_rtCaption.width = fCapReserve;
      CapLeftRightPlacement(capMargin, rtWidget, iCapPlacement);
      m_rtUI.width -= fCapReserve;
      m_rtUI.left += fCapReserve;
      break;
    }
    case XFA_AttributeValue::Top: {
      m_rtCaption.height = fCapReserve;
      CapTopBottomPlacement(capMargin, rtWidget, iCapPlacement);
      m_rtUI.top += fCapReserve;
      m_rtUI.height -= fCapReserve;
      break;
    }
    case XFA_AttributeValue::Right: {
      m_rtCaption.left = m_rtCaption.right() - fCapReserve;
      m_rtCaption.width = fCapReserve;
      CapLeftRightPlacement(capMargin, rtWidget, iCapPlacement);
      m_rtUI.width -= fCapReserve;
      break;
    }
    case XFA_AttributeValue::Bottom: {
      m_rtCaption.top = m_rtCaption.bottom() - fCapReserve;
      m_rtCaption.height = fCapReserve;
      CapTopBottomPlacement(capMargin, rtWidget, iCapPlacement);
      m_rtUI.height -= fCapReserve;
      break;
    }
    case XFA_AttributeValue::Inline:
      break;
    default:
      break;
  }

  CXFA_Border* borderUI = m_pNode->GetUIBorder();
  if (borderUI) {
    CXFA_Margin* borderMargin = borderUI->GetMarginIfExists();
    XFA_RectWithoutMargin(&m_rtUI, borderMargin);
  }
  m_rtUI.Normalize();
}

void CXFA_FFField::CapTopBottomPlacement(const CXFA_Margin* margin,
                                         const CFX_RectF& rtWidget,
                                         XFA_AttributeValue iCapPlacement) {
  CFX_RectF rtUIMargin = m_pNode->GetUIMargin();
  m_rtCaption.left += rtUIMargin.left;
  if (margin) {
    XFA_RectWithoutMargin(&m_rtCaption, margin);
    if (m_rtCaption.height < 0)
      m_rtCaption.top += m_rtCaption.height;
  }

  float fWidth = rtUIMargin.left + rtUIMargin.width;
  float fHeight = m_rtCaption.height + rtUIMargin.top + rtUIMargin.height;
  if (fWidth > rtWidget.width)
    m_rtUI.width += fWidth - rtWidget.width;

  if (fHeight == XFA_DEFAULTUI_HEIGHT && m_rtUI.height < XFA_MINUI_HEIGHT) {
    m_rtUI.height = XFA_MINUI_HEIGHT;
    m_rtCaption.top += rtUIMargin.top + rtUIMargin.height;
  } else if (fHeight > rtWidget.height) {
    m_rtUI.height += fHeight - rtWidget.height;
    if (iCapPlacement == XFA_AttributeValue::Bottom)
      m_rtCaption.top += fHeight - rtWidget.height;
  }
}

void CXFA_FFField::CapLeftRightPlacement(const CXFA_Margin* margin,
                                         const CFX_RectF& rtWidget,
                                         XFA_AttributeValue iCapPlacement) {
  CFX_RectF rtUIMargin = m_pNode->GetUIMargin();
  m_rtCaption.top += rtUIMargin.top;
  m_rtCaption.height -= rtUIMargin.top;
  if (margin) {
    XFA_RectWithoutMargin(&m_rtCaption, margin);
    if (m_rtCaption.height < 0)
      m_rtCaption.top += m_rtCaption.height;
  }

  float fWidth = m_rtCaption.width + rtUIMargin.left + rtUIMargin.width;
  float fHeight = rtUIMargin.top + rtUIMargin.height;
  if (fWidth > rtWidget.width) {
    m_rtUI.width += fWidth - rtWidget.width;
    if (iCapPlacement == XFA_AttributeValue::Right)
      m_rtCaption.left += fWidth - rtWidget.width;
  }

  if (fHeight == XFA_DEFAULTUI_HEIGHT && m_rtUI.height < XFA_MINUI_HEIGHT) {
    m_rtUI.height = XFA_MINUI_HEIGHT;
    m_rtCaption.top += rtUIMargin.top + rtUIMargin.height;
  } else if (fHeight > rtWidget.height) {
    m_rtUI.height += fHeight - rtWidget.height;
  }
}

void CXFA_FFField::UpdateFWL() {
  if (m_pNormalWidget)
    m_pNormalWidget->Update();
}

uint32_t CXFA_FFField::UpdateUIProperty() {
  CXFA_Node* pUiNode = m_pNode->GetUIChildNode();
  if (pUiNode && pUiNode->GetElementType() == XFA_Element::DefaultUi)
    return FWL_STYLEEXT_EDT_ReadOnly;
  return 0;
}

void CXFA_FFField::SetFWLRect() {
  if (!m_pNormalWidget)
    return;

  CFX_RectF rtUi = m_rtUI;
  rtUi.width = std::max(rtUi.width, 1.0f);
  if (!GetDoc()->GetXFADoc()->IsInteractive()) {
    float fFontSize = m_pNode->GetFontSize();
    rtUi.height = std::max(rtUi.height, fFontSize);
  }
  m_pNormalWidget->SetWidgetRect(rtUi);
}

bool CXFA_FFField::OnMouseEnter() {
  if (!m_pNormalWidget)
    return false;

  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::Enter;
  TranslateFWLMessage(&ms);
  return true;
}

bool CXFA_FFField::OnMouseExit() {
  if (!m_pNormalWidget)
    return false;

  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::Leave;
  TranslateFWLMessage(&ms);
  return true;
}

CFX_PointF CXFA_FFField::FWLToClient(const CFX_PointF& point) {
  return m_pNormalWidget ? point - m_pNormalWidget->GetWidgetRect().TopLeft()
                         : point;
}

bool CXFA_FFField::AcceptsFocusOnButtonDown(uint32_t dwFlags,
                                            const CFX_PointF& point,
                                            FWL_MouseCommand command) {
  if (!m_pNormalWidget)
    return false;
  if (!m_pNode->IsOpenAccess() || !GetDoc()->GetXFADoc()->IsInteractive())
    return false;
  if (!PtInActiveRect(point))
    return false;

  return true;
}

void CXFA_FFField::OnLButtonDown(uint32_t dwFlags, const CFX_PointF& point) {
  SetButtonDown(true);
  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::LeftButtonDown;
  ms.m_dwFlags = dwFlags;
  ms.m_pos = FWLToClient(point);
  TranslateFWLMessage(&ms);
}

bool CXFA_FFField::OnLButtonUp(uint32_t dwFlags, const CFX_PointF& point) {
  if (!m_pNormalWidget)
    return false;
  if (!IsButtonDown())
    return false;

  SetButtonDown(false);
  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::LeftButtonUp;
  ms.m_dwFlags = dwFlags;
  ms.m_pos = FWLToClient(point);
  TranslateFWLMessage(&ms);
  return true;
}

bool CXFA_FFField::OnLButtonDblClk(uint32_t dwFlags, const CFX_PointF& point) {
  if (!m_pNormalWidget)
    return false;

  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::LeftButtonDblClk;
  ms.m_dwFlags = dwFlags;
  ms.m_pos = FWLToClient(point);
  TranslateFWLMessage(&ms);
  return true;
}

bool CXFA_FFField::OnMouseMove(uint32_t dwFlags, const CFX_PointF& point) {
  if (!m_pNormalWidget)
    return false;

  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::Move;
  ms.m_dwFlags = dwFlags;
  ms.m_pos = FWLToClient(point);
  TranslateFWLMessage(&ms);
  return true;
}

bool CXFA_FFField::OnMouseWheel(uint32_t dwFlags,
                                int16_t zDelta,
                                const CFX_PointF& point) {
  if (!m_pNormalWidget)
    return false;

  CFWL_MessageMouseWheel ms(nullptr, m_pNormalWidget.get());
  ms.m_dwFlags = dwFlags;
  ms.m_pos = FWLToClient(point);
  ms.m_delta = CFX_PointF(zDelta, 0);
  TranslateFWLMessage(&ms);
  return true;
}

void CXFA_FFField::OnRButtonDown(uint32_t dwFlags, const CFX_PointF& point) {
  SetButtonDown(true);

  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::RightButtonDown;
  ms.m_dwFlags = dwFlags;
  ms.m_pos = FWLToClient(point);
  TranslateFWLMessage(&ms);
}

bool CXFA_FFField::OnRButtonUp(uint32_t dwFlags, const CFX_PointF& point) {
  if (!m_pNormalWidget)
    return false;
  if (!IsButtonDown())
    return false;

  SetButtonDown(false);
  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::RightButtonUp;
  ms.m_dwFlags = dwFlags;
  ms.m_pos = FWLToClient(point);
  TranslateFWLMessage(&ms);
  return true;
}

bool CXFA_FFField::OnRButtonDblClk(uint32_t dwFlags, const CFX_PointF& point) {
  if (!m_pNormalWidget)
    return false;

  CFWL_MessageMouse ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_MouseCommand::RightButtonDblClk;
  ms.m_dwFlags = dwFlags;
  ms.m_pos = FWLToClient(point);
  TranslateFWLMessage(&ms);
  return true;
}

bool CXFA_FFField::OnSetFocus(CXFA_FFWidget* pOldWidget) {
  CXFA_FFWidget::OnSetFocus(pOldWidget);
  if (!m_pNormalWidget)
    return false;

  CFWL_MessageSetFocus ms(nullptr, m_pNormalWidget.get());
  TranslateFWLMessage(&ms);
  m_dwStatus |= XFA_WidgetStatus_Focused;
  InvalidateRect();
  return true;
}

bool CXFA_FFField::OnKillFocus(CXFA_FFWidget* pNewWidget) {
  if (!m_pNormalWidget)
    return CXFA_FFWidget::OnKillFocus(pNewWidget);

  CFWL_MessageKillFocus ms(nullptr, m_pNormalWidget.get());
  TranslateFWLMessage(&ms);
  m_dwStatus &= ~XFA_WidgetStatus_Focused;
  InvalidateRect();
  CXFA_FFWidget::OnKillFocus(pNewWidget);
  return true;
}

bool CXFA_FFField::OnKeyDown(uint32_t dwKeyCode, uint32_t dwFlags) {
  if (!m_pNormalWidget || !GetDoc()->GetXFADoc()->IsInteractive())
    return false;

  CFWL_MessageKey ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_KeyCommand::KeyDown;
  ms.m_dwFlags = dwFlags;
  ms.m_dwKeyCode = dwKeyCode;
  TranslateFWLMessage(&ms);
  return true;
}

bool CXFA_FFField::OnKeyUp(uint32_t dwKeyCode, uint32_t dwFlags) {
  if (!m_pNormalWidget || !GetDoc()->GetXFADoc()->IsInteractive())
    return false;

  CFWL_MessageKey ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_KeyCommand::KeyUp;
  ms.m_dwFlags = dwFlags;
  ms.m_dwKeyCode = dwKeyCode;
  TranslateFWLMessage(&ms);
  return true;
}

bool CXFA_FFField::OnChar(uint32_t dwChar, uint32_t dwFlags) {
  if (!GetDoc()->GetXFADoc()->IsInteractive())
    return false;
  if (dwChar == FWL_VKEY_Tab)
    return true;
  if (!m_pNormalWidget)
    return false;
  if (!m_pNode->IsOpenAccess())
    return false;

  CFWL_MessageKey ms(nullptr, m_pNormalWidget.get());
  ms.m_dwCmd = FWL_KeyCommand::Char;
  ms.m_dwFlags = dwFlags;
  ms.m_dwKeyCode = dwChar;
  TranslateFWLMessage(&ms);
  return true;
}

FWL_WidgetHit CXFA_FFField::OnHitTest(const CFX_PointF& point) {
  if (m_pNormalWidget &&
      m_pNormalWidget->HitTest(FWLToClient(point)) != FWL_WidgetHit::Unknown) {
    return FWL_WidgetHit::Client;
  }

  if (!GetRectWithoutRotate().Contains(point))
    return FWL_WidgetHit::Unknown;
  if (m_rtCaption.Contains(point))
    return FWL_WidgetHit::Titlebar;
  return FWL_WidgetHit::Border;
}

bool CXFA_FFField::OnSetCursor(const CFX_PointF& point) {
  return true;
}

bool CXFA_FFField::PtInActiveRect(const CFX_PointF& point) {
  return m_pNormalWidget && m_pNormalWidget->GetWidgetRect().Contains(point);
}

void CXFA_FFField::LayoutCaption() {
  CXFA_TextLayout* pCapTextLayout = m_pNode->GetCaptionTextLayout();
  if (!pCapTextLayout)
    return;

  float fHeight = pCapTextLayout->Layout(m_rtCaption.Size());
  m_rtCaption.height = std::max(m_rtCaption.height, fHeight);
}

void CXFA_FFField::RenderCaption(CXFA_Graphics* pGS, CFX_Matrix* pMatrix) {
  CXFA_TextLayout* pCapTextLayout = m_pNode->GetCaptionTextLayout();
  if (!pCapTextLayout)
    return;

  CXFA_Caption* caption = m_pNode->GetCaptionIfExists();
  if (!caption || !caption->IsVisible())
    return;

  if (!pCapTextLayout->IsLoaded())
    pCapTextLayout->Layout(m_rtCaption.Size());

  CFX_RectF rtClip = m_rtCaption;
  rtClip.Intersect(GetRectWithoutRotate());
  CFX_RenderDevice* pRenderDevice = pGS->GetRenderDevice();
  CFX_Matrix mt(1, 0, 0, 1, m_rtCaption.left, m_rtCaption.top);
  if (pMatrix) {
    rtClip = pMatrix->TransformRect(rtClip);
    mt.Concat(*pMatrix);
  }
  pCapTextLayout->DrawString(pRenderDevice, mt, rtClip, 0);
}

bool CXFA_FFField::ProcessCommittedData() {
  if (!m_pNode->IsOpenAccess())
    return false;
  if (!IsDataChanged())
    return false;

  m_pDocView->SetChangeMark();
  m_pDocView->AddValidateNode(m_pNode.Get());

  if (CalculateOverride() != 1)
    return false;
  return CommitData();
}

int32_t CXFA_FFField::CalculateOverride() {
  CXFA_Node* exclNode = m_pNode->GetExclGroupIfExists();
  if (!exclNode || !exclNode->IsWidgetReady())
    return CalculateNode(m_pNode.Get());
  if (CalculateNode(exclNode) == 0)
    return 0;

  CXFA_Node* pNode = exclNode->GetExclGroupFirstMember();
  if (!pNode)
    return 1;

  while (pNode) {
    if (!pNode->IsWidgetReady())
      return 1;
    if (CalculateNode(pNode) == 0)
      return 0;

    pNode = pNode->GetExclGroupNextMember(pNode);
  }
  return 1;
}

int32_t CXFA_FFField::CalculateNode(CXFA_Node* pNode) {
  CXFA_Calculate* calc = pNode->GetCalculateIfExists();
  if (!calc)
    return 1;

  XFA_VERSION version = GetDoc()->GetXFADoc()->GetCurVersionMode();
  switch (calc->GetOverride()) {
    case XFA_AttributeValue::Error: {
      if (version <= XFA_VERSION_204)
        return 1;

      IXFA_AppProvider* pAppProvider = GetApp()->GetAppProvider();
      if (pAppProvider) {
        pAppProvider->MsgBox(
            WideString::FromASCII("You are not allowed to modify this field."),
            WideString::FromASCII("Calculate Override"),
            static_cast<uint32_t>(AlertIcon::kWarning),
            static_cast<uint32_t>(AlertButton::kOK));
      }
      return 0;
    }
    case XFA_AttributeValue::Warning: {
      if (version <= XFA_VERSION_204) {
        CXFA_Script* script = calc->GetScriptIfExists();
        if (!script || script->GetExpression().IsEmpty())
          return 1;
      }

      if (pNode->IsUserInteractive())
        return 1;

      IXFA_AppProvider* pAppProvider = GetApp()->GetAppProvider();
      if (!pAppProvider)
        return 0;

      WideString wsMessage = calc->GetMessageText();
      if (!wsMessage.IsEmpty())
        wsMessage += L"\r\n";
      wsMessage +=
          WideString::FromASCII("Are you sure you want to modify this field?");

      if (pAppProvider->MsgBox(wsMessage,
                               WideString::FromASCII("Calculate Override"),
                               static_cast<uint32_t>(AlertIcon::kWarning),
                               static_cast<uint32_t>(AlertButton::kYesNo)) ==
          static_cast<uint32_t>(AlertReturn::kYes)) {
        pNode->SetFlag(XFA_NodeFlag_UserInteractive);
        return 1;
      }
      return 0;
    }
    case XFA_AttributeValue::Ignore:
      return 0;
    case XFA_AttributeValue::Disabled:
      pNode->SetFlag(XFA_NodeFlag_UserInteractive);
      return 1;
    default:
      return 1;
  }
}

bool CXFA_FFField::CommitData() {
  return false;
}

bool CXFA_FFField::IsDataChanged() {
  return false;
}

void CXFA_FFField::TranslateFWLMessage(CFWL_Message* pMessage) {
  GetApp()->GetFWLWidgetMgr()->OnProcessMessageToForm(pMessage);
}

void CXFA_FFField::OnProcessMessage(CFWL_Message* pMessage) {}

void CXFA_FFField::OnProcessEvent(CFWL_Event* pEvent) {
  switch (pEvent->GetType()) {
    case CFWL_Event::Type::Mouse: {
      CFWL_EventMouse* event = static_cast<CFWL_EventMouse*>(pEvent);
      if (event->m_dwCmd == FWL_MouseCommand::Enter) {
        CXFA_EventParam eParam;
        eParam.m_eType = XFA_EVENT_MouseEnter;
        eParam.m_pTarget = m_pNode.Get();
        m_pNode->ProcessEvent(GetDocView(), XFA_AttributeValue::MouseEnter,
                              &eParam);
      } else if (event->m_dwCmd == FWL_MouseCommand::Leave) {
        CXFA_EventParam eParam;
        eParam.m_eType = XFA_EVENT_MouseExit;
        eParam.m_pTarget = m_pNode.Get();
        m_pNode->ProcessEvent(GetDocView(), XFA_AttributeValue::MouseExit,
                              &eParam);
      } else if (event->m_dwCmd == FWL_MouseCommand::LeftButtonDown) {
        CXFA_EventParam eParam;
        eParam.m_eType = XFA_EVENT_MouseDown;
        eParam.m_pTarget = m_pNode.Get();
        m_pNode->ProcessEvent(GetDocView(), XFA_AttributeValue::MouseDown,
                              &eParam);
      } else if (event->m_dwCmd == FWL_MouseCommand::LeftButtonUp) {
        CXFA_EventParam eParam;
        eParam.m_eType = XFA_EVENT_MouseUp;
        eParam.m_pTarget = m_pNode.Get();
        m_pNode->ProcessEvent(GetDocView(), XFA_AttributeValue::MouseUp,
                              &eParam);
      }
      break;
    }
    case CFWL_Event::Type::Click: {
      CXFA_EventParam eParam;
      eParam.m_eType = XFA_EVENT_Click;
      eParam.m_pTarget = m_pNode.Get();
      m_pNode->ProcessEvent(GetDocView(), XFA_AttributeValue::Click, &eParam);
      break;
    }
    default:
      break;
  }
}

void CXFA_FFField::OnDrawWidget(CXFA_Graphics* pGraphics,
                                const CFX_Matrix& matrix) {}
