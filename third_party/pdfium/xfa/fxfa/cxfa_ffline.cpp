// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_ffline.h"

#include "xfa/fxfa/parser/cxfa_edge.h"
#include "xfa/fxfa/parser/cxfa_line.h"
#include "xfa/fxfa/parser/cxfa_value.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"
#include "xfa/fxgraphics/cxfa_graphics.h"

namespace {

CFX_GraphStateData::LineCap LineCapToFXGE(XFA_AttributeValue iLineCap) {
  switch (iLineCap) {
    case XFA_AttributeValue::Round:
      return CFX_GraphStateData::LineCapRound;
    case XFA_AttributeValue::Butt:
      return CFX_GraphStateData::LineCapButt;
    default:
      break;
  }
  return CFX_GraphStateData::LineCapSquare;
}

}  // namespace

CXFA_FFLine::CXFA_FFLine(CXFA_Node* pNode) : CXFA_FFWidget(pNode) {}

CXFA_FFLine::~CXFA_FFLine() {}

void CXFA_FFLine::GetRectFromHand(CFX_RectF& rect,
                                  XFA_AttributeValue iHand,
                                  float fLineWidth) {
  float fHalfWidth = fLineWidth / 2.0f;
  if (rect.height < 1.0f) {
    switch (iHand) {
      case XFA_AttributeValue::Left:
        rect.top -= fHalfWidth;
        break;
      case XFA_AttributeValue::Right:
        rect.top += fHalfWidth;
        break;
      case XFA_AttributeValue::Even:
        break;
      default:
        NOTREACHED();
        break;
    }
  } else if (rect.width < 1.0f) {
    switch (iHand) {
      case XFA_AttributeValue::Left:
        rect.left += fHalfWidth;
        break;
      case XFA_AttributeValue::Right:
        rect.left += fHalfWidth;
        break;
      case XFA_AttributeValue::Even:
        break;
      default:
        NOTREACHED();
        break;
    }
  } else {
    switch (iHand) {
      case XFA_AttributeValue::Left:
        rect.Inflate(fHalfWidth, fHalfWidth);
        break;
      case XFA_AttributeValue::Right:
        rect.Deflate(fHalfWidth, fHalfWidth);
        break;
      case XFA_AttributeValue::Even:
        break;
      default:
        NOTREACHED();
        break;
    }
  }
}

void CXFA_FFLine::RenderWidget(CXFA_Graphics* pGS,
                               const CFX_Matrix& matrix,
                               uint32_t dwStatus) {
  if (!IsMatchVisibleStatus(dwStatus))
    return;

  CXFA_Value* value = m_pNode->GetFormValueIfExists();
  if (!value)
    return;

  FX_ARGB lineColor = 0xFF000000;
  float fLineWidth = 1.0f;
  XFA_AttributeValue iStrokeType = XFA_AttributeValue::Unknown;
  XFA_AttributeValue iCap = XFA_AttributeValue::Unknown;

  CXFA_Line* line = value->GetLineIfExists();
  if (line) {
    CXFA_Edge* edge = line->GetEdgeIfExists();
    if (edge && !edge->IsVisible())
      return;

    if (edge) {
      lineColor = edge->GetColor();
      iStrokeType = edge->GetStrokeType();
      fLineWidth = edge->GetThickness();
      iCap = edge->GetCapType();
    }
  }

  CFX_Matrix mtRotate = GetRotateMatrix();
  mtRotate.Concat(matrix);

  CFX_RectF rtLine = GetRectWithoutRotate();
  CXFA_Margin* margin = m_pNode->GetMarginIfExists();
  XFA_RectWithoutMargin(&rtLine, margin);

  GetRectFromHand(rtLine, line ? line->GetHand() : XFA_AttributeValue::Left,
                  fLineWidth);
  CXFA_GEPath linePath;
  if (line && line->GetSlope() && rtLine.right() > 0.0f &&
      rtLine.bottom() > 0.0f) {
    linePath.AddLine(rtLine.TopRight(), rtLine.BottomLeft());
  } else {
    linePath.AddLine(rtLine.TopLeft(), rtLine.BottomRight());
  }

  pGS->SaveGraphState();
  pGS->SetLineWidth(fLineWidth);
  pGS->EnableActOnDash();
  XFA_StrokeTypeSetLineDash(pGS, iStrokeType, iCap);

  pGS->SetStrokeColor(CXFA_GEColor(lineColor));
  pGS->SetLineCap(LineCapToFXGE(iCap));
  pGS->StrokePath(&linePath, &mtRotate);
  pGS->RestoreGraphState();
}
