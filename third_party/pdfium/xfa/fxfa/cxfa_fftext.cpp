// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_fftext.h"

#include "xfa/fwl/fwl_widgetdef.h"
#include "xfa/fwl/fwl_widgethit.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffdoc.h"
#include "xfa/fxfa/cxfa_ffpageview.h"
#include "xfa/fxfa/cxfa_ffwidget.h"
#include "xfa/fxfa/cxfa_linkuserdata.h"
#include "xfa/fxfa/cxfa_pieceline.h"
#include "xfa/fxfa/cxfa_textlayout.h"
#include "xfa/fxfa/cxfa_textpiece.h"
#include "xfa/fxfa/parser/cxfa_margin.h"
#include "xfa/fxgraphics/cxfa_graphics.h"

CXFA_FFText::CXFA_FFText(CXFA_Node* pNode) : CXFA_FFWidget(pNode) {}

CXFA_FFText::~CXFA_FFText() {}

void CXFA_FFText::RenderWidget(CXFA_Graphics* pGS,
                               const CFX_Matrix& matrix,
                               uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus))
    return;

  CFX_Matrix mtRotate = GetRotateMatrix();
  mtRotate.Concat(matrix);

  CXFA_FFWidget::RenderWidget(pGS, mtRotate, dwStatus);

  CXFA_TextLayout* pTextLayout = m_pNode->GetTextLayout();
  if (!pTextLayout)
    return;

  CFX_RenderDevice* pRenderDevice = pGS->GetRenderDevice();
  CFX_RectF rtText = GetRectWithoutRotate();
  CXFA_Margin* margin = m_pNode->GetMarginIfExists();
  if (margin) {
    CXFA_ContentLayoutItem* pItem = this;
    if (!pItem->GetPrev() && !pItem->GetNext()) {
      XFA_RectWithoutMargin(&rtText, margin);
    } else {
      float fTopInset = 0;
      float fBottomInset = 0;
      if (!pItem->GetPrev())
        fTopInset = margin->GetTopInset();
      else if (!pItem->GetNext())
        fBottomInset = margin->GetBottomInset();

      rtText.Deflate(margin->GetLeftInset(), fTopInset, margin->GetRightInset(),
                     fBottomInset);
    }
  }

  CFX_Matrix mt(1, 0, 0, 1, rtText.left, rtText.top);
  CFX_RectF rtClip = mtRotate.TransformRect(rtText);
  mt.Concat(mtRotate);
  pTextLayout->DrawString(pRenderDevice, mt, rtClip, GetIndex());
}

bool CXFA_FFText::IsLoaded() {
  CXFA_TextLayout* pTextLayout = m_pNode->GetTextLayout();
  return pTextLayout && !pTextLayout->m_bHasBlock;
}

bool CXFA_FFText::PerformLayout() {
  CXFA_FFWidget::PerformLayout();
  CXFA_TextLayout* pTextLayout = m_pNode->GetTextLayout();
  if (!pTextLayout)
    return false;
  if (!pTextLayout->m_bHasBlock)
    return true;

  pTextLayout->m_Blocks.clear();
  CXFA_ContentLayoutItem* pItem = this;
  if (!pItem->GetPrev() && !pItem->GetNext())
    return true;

  pItem = pItem->GetFirst();
  while (pItem) {
    CFX_RectF rtText = pItem->GetRect(false);
    CXFA_Margin* margin = m_pNode->GetMarginIfExists();
    if (margin) {
      if (!pItem->GetPrev())
        rtText.height -= margin->GetTopInset();
      else if (!pItem->GetNext())
        rtText.height -= margin->GetBottomInset();
    }
    pTextLayout->ItemBlocks(rtText, pItem->GetIndex());
    pItem = pItem->GetNext();
  }
  pTextLayout->m_bHasBlock = false;
  return true;
}

bool CXFA_FFText::AcceptsFocusOnButtonDown(uint32_t dwFlags,
                                           const CFX_PointF& point,
                                           FWL_MouseCommand command) {
  if (command != FWL_MouseCommand::LeftButtonDown)
    return false;

  if (!GetRectWithoutRotate().Contains(point))
    return false;

  const wchar_t* wsURLContent = GetLinkURLAtPoint(point);
  if (!wsURLContent)
    return false;

  return true;
}

void CXFA_FFText::OnLButtonDown(uint32_t dwFlags, const CFX_PointF& point) {
  SetButtonDown(true);
}

bool CXFA_FFText::OnMouseMove(uint32_t dwFlags, const CFX_PointF& point) {
  return GetRectWithoutRotate().Contains(point) && !!GetLinkURLAtPoint(point);
}

bool CXFA_FFText::OnLButtonUp(uint32_t dwFlags, const CFX_PointF& point) {
  if (!IsButtonDown())
    return false;

  SetButtonDown(false);
  const wchar_t* wsURLContent = GetLinkURLAtPoint(point);
  if (!wsURLContent)
    return false;

  CXFA_FFDoc* pDoc = GetDoc();
  pDoc->GetDocEnvironment()->GotoURL(pDoc, wsURLContent);
  return true;
}

FWL_WidgetHit CXFA_FFText::OnHitTest(const CFX_PointF& point) {
  if (!GetRectWithoutRotate().Contains(point))
    return FWL_WidgetHit::Unknown;
  if (!GetLinkURLAtPoint(point))
    return FWL_WidgetHit::Unknown;
  return FWL_WidgetHit::HyperLink;
}

const wchar_t* CXFA_FFText::GetLinkURLAtPoint(const CFX_PointF& point) {
  CXFA_TextLayout* pTextLayout = m_pNode->GetTextLayout();
  if (!pTextLayout)
    return nullptr;

  CFX_RectF rect = GetRectWithoutRotate();
  for (const auto& pPieceLine : *pTextLayout->GetPieceLines()) {
    for (const auto& pPiece : pPieceLine->m_textPieces) {
      if (pPiece->pLinkData &&
          pPiece->rtPiece.Contains(point - rect.TopLeft())) {
        return pPiece->pLinkData->GetLinkURL();
      }
    }
  }
  return nullptr;
}
