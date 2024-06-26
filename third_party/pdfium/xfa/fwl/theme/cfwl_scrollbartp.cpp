// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_scrollbartp.h"

#include "xfa/fwl/cfwl_scrollbar.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/ifwl_themeprovider.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

namespace {

const float kPawLength = 12.5f;

}  // namespace

CFWL_ScrollBarTP::CFWL_ScrollBarTP() : m_pThemeData(new SBThemeData) {
  SetThemeData();
}

CFWL_ScrollBarTP::~CFWL_ScrollBarTP() {}

void CFWL_ScrollBarTP::DrawBackground(const CFWL_ThemeBackground& pParams) {
  CFWL_Widget* pWidget = pParams.m_pWidget;
  FWLTHEME_STATE eState = FWLTHEME_STATE_Normal;
  if (pParams.m_dwStates & CFWL_PartState_Hovered)
    eState = FWLTHEME_STATE_Hover;
  else if (pParams.m_dwStates & CFWL_PartState_Pressed)
    eState = FWLTHEME_STATE_Pressed;
  else if (pParams.m_dwStates & CFWL_PartState_Disabled)
    eState = FWLTHEME_STATE_Disable;

  CXFA_Graphics* pGraphics = pParams.m_pGraphics.Get();
  bool bVert = !!pWidget->GetStylesEx();
  switch (pParams.m_iPart) {
    case CFWL_Part::ForeArrow: {
      DrawMaxMinBtn(pGraphics, pParams.m_rtPart,
                    bVert ? FWLTHEME_DIRECTION_Up : FWLTHEME_DIRECTION_Left,
                    eState, pParams.m_matrix);
      break;
    }
    case CFWL_Part::BackArrow: {
      DrawMaxMinBtn(pGraphics, pParams.m_rtPart,
                    bVert ? FWLTHEME_DIRECTION_Down : FWLTHEME_DIRECTION_Right,
                    eState, pParams.m_matrix);
      break;
    }
    case CFWL_Part::Thumb: {
      DrawThumbBtn(pGraphics, pParams.m_rtPart, bVert, eState, true,
                   pParams.m_matrix);
      break;
    }
    case CFWL_Part::LowerTrack: {
      DrawTrack(pGraphics, pParams.m_rtPart, bVert, eState, true,
                pParams.m_matrix);
      break;
    }
    case CFWL_Part::UpperTrack: {
      DrawTrack(pGraphics, pParams.m_rtPart, bVert, eState, false,
                pParams.m_matrix);
      break;
    }
    default:
      break;
  }
}

void CFWL_ScrollBarTP::DrawThumbBtn(CXFA_Graphics* pGraphics,
                                    const CFX_RectF& input_rect,
                                    bool bVert,
                                    FWLTHEME_STATE eState,
                                    bool bPawButton,
                                    const CFX_Matrix& matrix) {
  if (eState < FWLTHEME_STATE_Normal || eState > FWLTHEME_STATE_Disable)
    return;

  CFX_RectF rect = input_rect;
  if (bVert)
    rect.Deflate(1, 0);
  else
    rect.Deflate(0, 1);

  if (rect.IsEmpty(0.1f))
    return;

  FillSolidRect(pGraphics, m_pThemeData->clrBtnBK[eState - 1][1], rect, matrix);

  pGraphics->SaveGraphState();

  CXFA_GEPath path;
  path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
  pGraphics->SetStrokeColor(
      CXFA_GEColor(m_pThemeData->clrBtnBorder[eState - 1]));
  pGraphics->StrokePath(&path, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_ScrollBarTP::DrawPaw(CXFA_Graphics* pGraphics,
                               const CFX_RectF& rect,
                               bool bVert,
                               FWLTHEME_STATE eState,
                               const CFX_Matrix& matrix) {
  CXFA_GEPath path;
  if (bVert) {
    float fPawLen = kPawLength;
    if (rect.width / 2 <= fPawLen) {
      fPawLen = (rect.width - 6) / 2;
    }

    float fX = rect.left + rect.width / 4;
    float fY = rect.top + rect.height / 2;
    path.MoveTo(CFX_PointF(fX, fY - 4));
    path.LineTo(CFX_PointF(fX + fPawLen, fY - 4));
    path.MoveTo(CFX_PointF(fX, fY - 2));
    path.LineTo(CFX_PointF(fX + fPawLen, fY - 2));
    path.MoveTo(CFX_PointF(fX, fY));
    path.LineTo(CFX_PointF(fX + fPawLen, fY));
    path.MoveTo(CFX_PointF(fX, fY + 2));
    path.LineTo(CFX_PointF(fX + fPawLen, fY + 2));

    pGraphics->SetLineWidth(1);
    pGraphics->SetStrokeColor(
        CXFA_GEColor(m_pThemeData->clrPawColorLight[eState - 1]));
    pGraphics->StrokePath(&path, nullptr);
    fX++;

    path.Clear();
    path.MoveTo(CFX_PointF(fX, fY - 3));
    path.LineTo(CFX_PointF(fX + fPawLen, fY - 3));
    path.MoveTo(CFX_PointF(fX, fY - 1));
    path.LineTo(CFX_PointF(fX + fPawLen, fY - 1));
    path.MoveTo(CFX_PointF(fX, fY + 1));
    path.LineTo(CFX_PointF(fX + fPawLen, fY + 1));
    path.MoveTo(CFX_PointF(fX, fY + 3));
    path.LineTo(CFX_PointF(fX + fPawLen, fY + 3));

    pGraphics->SetLineWidth(1);
    pGraphics->SetStrokeColor(
        CXFA_GEColor(m_pThemeData->clrPawColorDark[eState - 1]));
    pGraphics->StrokePath(&path, &matrix);
  } else {
    float fPawLen = kPawLength;
    if (rect.height / 2 <= fPawLen) {
      fPawLen = (rect.height - 6) / 2;
    }

    float fX = rect.left + rect.width / 2;
    float fY = rect.top + rect.height / 4;
    path.MoveTo(CFX_PointF(fX - 4, fY));
    path.LineTo(CFX_PointF(fX - 4, fY + fPawLen));
    path.MoveTo(CFX_PointF(fX - 2, fY));
    path.LineTo(CFX_PointF(fX - 2, fY + fPawLen));
    path.MoveTo(CFX_PointF(fX, fY));
    path.LineTo(CFX_PointF(fX, fY + fPawLen));
    path.MoveTo(CFX_PointF(fX + 2, fY));
    path.LineTo(CFX_PointF(fX + 2, fY + fPawLen));

    pGraphics->SetLineWidth(1);
    pGraphics->SetStrokeColor(
        CXFA_GEColor(m_pThemeData->clrPawColorLight[eState - 1]));
    pGraphics->StrokePath(&path, &matrix);
    fY++;

    path.Clear();
    path.MoveTo(CFX_PointF(fX - 3, fY));
    path.LineTo(CFX_PointF(fX - 3, fY + fPawLen));
    path.MoveTo(CFX_PointF(fX - 1, fY));
    path.LineTo(CFX_PointF(fX - 1, fY + fPawLen));
    path.MoveTo(CFX_PointF(fX + 1, fY));
    path.LineTo(CFX_PointF(fX + 1, fY + fPawLen));
    path.MoveTo(CFX_PointF(fX + 3, fY));
    path.LineTo(CFX_PointF(fX + 3, fY + fPawLen));

    pGraphics->SetLineWidth(1);
    pGraphics->SetStrokeColor(
        CXFA_GEColor(m_pThemeData->clrPawColorDark[eState - 1]));
    pGraphics->StrokePath(&path, &matrix);
  }
}

void CFWL_ScrollBarTP::DrawTrack(CXFA_Graphics* pGraphics,
                                 const CFX_RectF& rect,
                                 bool bVert,
                                 FWLTHEME_STATE eState,
                                 bool bLowerTrack,
                                 const CFX_Matrix& matrix) {
  if (eState < FWLTHEME_STATE_Normal || eState > FWLTHEME_STATE_Disable)
    return;

  pGraphics->SaveGraphState();
  CXFA_GEPath path;
  float fRight = rect.right();
  float fBottom = rect.bottom();
  if (bVert) {
    path.AddRectangle(rect.left, rect.top, 1, rect.height);
    path.AddRectangle(fRight - 1, rect.top, 1, rect.height);
  } else {
    path.AddRectangle(rect.left, rect.top, rect.width, 1);
    path.AddRectangle(rect.left, fBottom - 1, rect.width, 1);
  }
  pGraphics->SetFillColor(CXFA_GEColor(ArgbEncode(255, 238, 237, 229)));
  pGraphics->FillPath(&path, FXFILL_WINDING, &matrix);
  path.Clear();
  path.AddRectangle(rect.left + 1, rect.top, rect.width - 2, rect.height);
  pGraphics->RestoreGraphState();
  FillSolidRect(pGraphics, m_pThemeData->clrTrackBKEnd, rect, matrix);
}

void CFWL_ScrollBarTP::DrawMaxMinBtn(CXFA_Graphics* pGraphics,
                                     const CFX_RectF& rect,
                                     FWLTHEME_DIRECTION eDict,
                                     FWLTHEME_STATE eState,
                                     const CFX_Matrix& matrix) {
  DrawTrack(pGraphics, rect,
            eDict == FWLTHEME_DIRECTION_Up || eDict == FWLTHEME_DIRECTION_Down,
            eState, true, matrix);
  CFX_RectF rtArrowBtn = rect;
  rtArrowBtn.Deflate(1, 1, 1, 1);
  DrawArrowBtn(pGraphics, rtArrowBtn, eDict, eState, matrix);
}

void CFWL_ScrollBarTP::SetThemeData() {
  m_pThemeData->clrPawColorLight[3] = ArgbEncode(0xff, 208, 223, 172);
  m_pThemeData->clrPawColorDark[3] = ArgbEncode(0xff, 140, 157, 115);
  m_pThemeData->clrBtnBK[3][0] = ArgbEncode(0xff, 164, 180, 139);
  m_pThemeData->clrBtnBK[3][1] = ArgbEncode(0xff, 141, 157, 115);
  m_pThemeData->clrBtnBorder[3] = ArgbEncode(0xff, 236, 233, 216);
  m_pThemeData->clrPawColorLight[0] = ArgbEncode(0xff, 238, 244, 254);
  m_pThemeData->clrPawColorDark[0] = ArgbEncode(0xff, 140, 176, 248);
  m_pThemeData->clrBtnBK[0][0] = ArgbEncode(0xff, 197, 213, 252);
  m_pThemeData->clrBtnBK[0][1] = ArgbEncode(0xff, 182, 205, 251);
  m_pThemeData->clrBtnBorder[0] = ArgbEncode(0xff, 148, 176, 221);
  m_pThemeData->clrPawColorLight[1] = ArgbEncode(0xff, 252, 253, 255);
  m_pThemeData->clrPawColorDark[1] = ArgbEncode(0xff, 156, 197, 255);
  m_pThemeData->clrBtnBK[1][0] = ArgbEncode(0xff, 216, 232, 255);
  m_pThemeData->clrBtnBK[1][1] = ArgbEncode(0xff, 204, 225, 255);
  m_pThemeData->clrBtnBorder[1] = ArgbEncode(0xff, 218, 230, 254);
  m_pThemeData->clrPawColorLight[2] = ArgbEncode(0xff, 207, 221, 253);
  m_pThemeData->clrPawColorDark[2] = ArgbEncode(0xff, 131, 158, 216);
  m_pThemeData->clrBtnBK[2][0] = ArgbEncode(0xff, 167, 190, 245);
  m_pThemeData->clrBtnBK[2][1] = ArgbEncode(0xff, 146, 179, 249);
  m_pThemeData->clrBtnBorder[2] = ArgbEncode(0xff, 124, 159, 211);
  m_pThemeData->clrTrackBKStart = ArgbEncode(0xff, 243, 241, 236);
  m_pThemeData->clrTrackBKEnd = ArgbEncode(0xff, 254, 254, 251);
}
