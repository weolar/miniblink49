// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_checkboxtp.h"

#include "core/fxge/cfx_pathdata.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fwl/cfwl_checkbox.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_themetext.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

namespace {

const int kSignPath = 100;

CFX_PointF ScaleBezierPoint(const CFX_PointF& point) {
  CFX_PointF scaled_point(point);
  scaled_point.x *= FX_BEZIER;
  scaled_point.y *= FX_BEZIER;
  return scaled_point;
}

}  // namespace

CFWL_CheckBoxTP::CFWL_CheckBoxTP() : m_pThemeData(new CKBThemeData) {
  SetThemeData();
}

CFWL_CheckBoxTP::~CFWL_CheckBoxTP() {
  if (m_pCheckPath)
    m_pCheckPath->Clear();
}

void CFWL_CheckBoxTP::Initialize() {
  CFWL_WidgetTP::Initialize();
  InitTTO();
}

void CFWL_CheckBoxTP::Finalize() {
  FinalizeTTO();
  CFWL_WidgetTP::Finalize();
}

void CFWL_CheckBoxTP::DrawText(const CFWL_ThemeText& pParams) {
  if (!m_pTextOut)
    return;

  m_pTextOut->SetTextColor(pParams.m_dwStates & CFWL_PartState_Disabled
                               ? FWLTHEME_CAPACITY_TextDisColor
                               : FWLTHEME_CAPACITY_TextColor);
  CFWL_WidgetTP::DrawText(pParams);
}

void CFWL_CheckBoxTP::DrawSignCheck(CXFA_Graphics* pGraphics,
                                    const CFX_RectF& rtSign,
                                    FX_ARGB argbFill,
                                    const CFX_Matrix& matrix) {
  if (!m_pCheckPath)
    InitCheckPath(rtSign.width);

  CFX_Matrix mt;
  mt.Translate(rtSign.left, rtSign.top);
  mt.Concat(matrix);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(CXFA_GEColor(argbFill));
  pGraphics->FillPath(m_pCheckPath.get(), FXFILL_WINDING, &mt);
  pGraphics->RestoreGraphState();
}

void CFWL_CheckBoxTP::DrawSignCircle(CXFA_Graphics* pGraphics,
                                     const CFX_RectF& rtSign,
                                     FX_ARGB argbFill,
                                     const CFX_Matrix& matrix) {
  CXFA_GEPath path;
  path.AddEllipse(rtSign);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(CXFA_GEColor(argbFill));
  pGraphics->FillPath(&path, FXFILL_WINDING, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_CheckBoxTP::DrawSignCross(CXFA_Graphics* pGraphics,
                                    const CFX_RectF& rtSign,
                                    FX_ARGB argbFill,
                                    const CFX_Matrix& matrix) {
  CXFA_GEPath path;
  float fRight = rtSign.right();
  float fBottom = rtSign.bottom();
  path.AddLine(rtSign.TopLeft(), CFX_PointF(fRight, fBottom));
  path.AddLine(CFX_PointF(rtSign.left, fBottom),
               CFX_PointF(fRight, rtSign.top));

  pGraphics->SaveGraphState();
  pGraphics->SetStrokeColor(CXFA_GEColor(argbFill));
  pGraphics->SetLineWidth(1.0f);
  pGraphics->StrokePath(&path, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_CheckBoxTP::DrawSignDiamond(CXFA_Graphics* pGraphics,
                                      const CFX_RectF& rtSign,
                                      FX_ARGB argbFill,
                                      const CFX_Matrix& matrix) {
  CXFA_GEPath path;
  float fWidth = rtSign.width;
  float fHeight = rtSign.height;
  float fBottom = rtSign.bottom();
  path.MoveTo(CFX_PointF(rtSign.left + fWidth / 2, rtSign.top));
  path.LineTo(CFX_PointF(rtSign.left, rtSign.top + fHeight / 2));
  path.LineTo(CFX_PointF(rtSign.left + fWidth / 2, fBottom));
  path.LineTo(CFX_PointF(rtSign.right(), rtSign.top + fHeight / 2));
  path.LineTo(CFX_PointF(rtSign.left + fWidth / 2, rtSign.top));

  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(CXFA_GEColor(argbFill));
  pGraphics->FillPath(&path, FXFILL_WINDING, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_CheckBoxTP::DrawSignSquare(CXFA_Graphics* pGraphics,
                                     const CFX_RectF& rtSign,
                                     FX_ARGB argbFill,
                                     const CFX_Matrix& matrix) {
  CXFA_GEPath path;
  path.AddRectangle(rtSign.left, rtSign.top, rtSign.width, rtSign.height);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(CXFA_GEColor(argbFill));
  pGraphics->FillPath(&path, FXFILL_WINDING, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_CheckBoxTP::DrawSignStar(CXFA_Graphics* pGraphics,
                                   const CFX_RectF& rtSign,
                                   FX_ARGB argbFill,
                                   const CFX_Matrix& matrix) {
  CXFA_GEPath path;
  float fBottom = rtSign.bottom();
  float fRadius =
      (rtSign.top - fBottom) / (1 + static_cast<float>(cos(FX_PI / 5.0f)));
  CFX_PointF ptCenter((rtSign.left + rtSign.right()) / 2.0f,
                      (rtSign.top + fBottom) / 2.0f);

  CFX_PointF points[5];
  float fAngel = FX_PI / 10.0f;
  for (int32_t i = 0; i < 5; i++) {
    points[i] =
        ptCenter + CFX_PointF(fRadius * static_cast<float>(cos(fAngel)),
                              fRadius * static_cast<float>(sin(fAngel)));
    fAngel += FX_PI * 2 / 5.0f;
  }

  path.MoveTo(points[0]);
  int32_t nNext = 0;
  for (int32_t j = 0; j < 5; j++) {
    nNext += 2;
    if (nNext >= 5)
      nNext -= 5;

    path.LineTo(points[nNext]);
  }
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(CXFA_GEColor(argbFill));
  pGraphics->FillPath(&path, FXFILL_WINDING, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_CheckBoxTP::SetThemeData() {
  uint32_t* pData = (uint32_t*)&m_pThemeData->clrBoxBk;

  *pData++ = 0;
  *pData++ = 0;
  *pData++ = ArgbEncode(255, 220, 220, 215),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 240, 207),
  *pData++ = ArgbEncode(255, 248, 179, 48),
  *pData++ = ArgbEncode(255, 176, 176, 167),
  *pData++ = ArgbEncode(255, 241, 239, 239),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 220, 220, 215),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 240, 207),
  *pData++ = ArgbEncode(255, 248, 179, 48),
  *pData++ = ArgbEncode(255, 176, 176, 167),
  *pData++ = ArgbEncode(255, 241, 239, 239),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 220, 220, 215),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 240, 207),
  *pData++ = ArgbEncode(255, 248, 179, 48),
  *pData++ = ArgbEncode(255, 176, 176, 167),
  *pData++ = ArgbEncode(255, 241, 239, 239),
  *pData++ = ArgbEncode(255, 255, 255, 255),
  *pData++ = ArgbEncode(255, 255, 255, 255);
  m_pThemeData->clrSignBorderNormal = ArgbEncode(255, 28, 81, 128);
  m_pThemeData->clrSignBorderDisable = ArgbEncode(255, 202, 200, 187);
  m_pThemeData->clrSignCheck = ArgbEncode(255, 28, 81, 128);
  m_pThemeData->clrSignNeutral = ArgbEncode(255, 28, 134, 26);
  m_pThemeData->clrSignNeutralNormal = ArgbEncode(255, 114, 192, 113);
  m_pThemeData->clrSignNeutralHover = ArgbEncode(255, 33, 161, 33);
  m_pThemeData->clrSignNeutralPressed = ArgbEncode(255, 28, 134, 26);
}

void CFWL_CheckBoxTP::InitCheckPath(float fCheckLen) {
  if (!m_pCheckPath) {
    m_pCheckPath = pdfium::MakeUnique<CXFA_GEPath>();

    float fWidth = kSignPath;
    float fHeight = -kSignPath;
    float fBottom = kSignPath;
    CFX_PointF pt1(fWidth / 15.0f, fBottom + fHeight * 2 / 5.0f);
    CFX_PointF pt2(fWidth / 4.5f, fBottom + fHeight / 16.0f);
    CFX_PointF pt3(fWidth / 3.0f, fBottom);
    CFX_PointF pt4(fWidth * 14 / 15.0f, fBottom + fHeight * 15 / 16.0f);
    CFX_PointF pt5(fWidth / 3.6f, fBottom + fHeight / 3.5f);
    CFX_PointF pt12(fWidth / 7.0f, fBottom + fHeight * 2 / 7.0f);
    CFX_PointF pt21(fWidth / 5.0f, fBottom + fHeight / 5.0f);
    CFX_PointF pt23(fWidth / 4.4f, fBottom + fHeight * 0 / 16.0f);
    CFX_PointF pt32(fWidth / 4.0f, fBottom);
    CFX_PointF pt34(fWidth * (1 / 7.0f + 7 / 15.0f),
                    fBottom + fHeight * 4 / 5.0f);
    CFX_PointF pt43(fWidth * (1 / 7.0f + 7 / 15.0f),
                    fBottom + fHeight * 4 / 5.0f);
    CFX_PointF pt45(fWidth * 7 / 15.0f, fBottom + fHeight * 8 / 7.0f);
    CFX_PointF pt54(fWidth / 3.4f, fBottom + fHeight / 3.5f);
    CFX_PointF pt51(fWidth / 3.6f, fBottom + fHeight / 4.0f);
    CFX_PointF pt15(fWidth / 3.5f, fBottom + fHeight * 3.5f / 5.0f);
    m_pCheckPath->MoveTo(pt1);

    CFX_PointF p1 = ScaleBezierPoint(pt12 - pt1);
    CFX_PointF p2 = ScaleBezierPoint(pt21 - pt2);
    m_pCheckPath->BezierTo(pt1 + p1, pt2 + p2, pt2);

    p1 = ScaleBezierPoint(pt23 - pt2);
    p2 = ScaleBezierPoint(pt32 - pt3);
    m_pCheckPath->BezierTo(pt2 + p1, pt3 + p2, pt3);

    p1 = ScaleBezierPoint(pt34 - pt3);
    p2 = ScaleBezierPoint(pt43 - pt4);
    m_pCheckPath->BezierTo(pt3 + p1, pt4 + p2, pt4);

    p1 = ScaleBezierPoint(pt45 - pt4);
    p2 = ScaleBezierPoint(pt54 - pt5);
    m_pCheckPath->BezierTo(pt4 + p1, pt5 + p2, pt5);

    p1 = ScaleBezierPoint(pt51 - pt5);
    p2 = ScaleBezierPoint(pt15 - pt1);
    m_pCheckPath->BezierTo(pt5 + p1, pt1 + p2, pt1);

    float fScale = fCheckLen / kSignPath;
    CFX_Matrix mt;
    mt.Scale(fScale, fScale);

    m_pCheckPath->TransformBy(mt);
  }
}

void CFWL_CheckBoxTP::DrawBackground(const CFWL_ThemeBackground& pParams) {
  if (pParams.m_iPart != CFWL_Part::CheckBox)
    return;

  if ((pParams.m_dwStates & CFWL_PartState_Checked) ||
      (pParams.m_dwStates & CFWL_PartState_Neutral)) {
    DrawCheckSign(pParams.m_pWidget, pParams.m_pGraphics.Get(),
                  pParams.m_rtPart, pParams.m_dwStates, pParams.m_matrix);
  }
}

void CFWL_CheckBoxTP::DrawCheckSign(CFWL_Widget* pWidget,
                                    CXFA_Graphics* pGraphics,
                                    const CFX_RectF& pRtBox,
                                    int32_t iState,
                                    const CFX_Matrix& matrix) {
  CFX_RectF rtSign(pRtBox);
  uint32_t dwColor = iState & CFWL_PartState_Neutral ? 0xFFA9A9A9 : 0xFF000000;

  uint32_t dwStyle = pWidget->GetStylesEx();
  rtSign.Deflate(rtSign.width / 4, rtSign.height / 4);
  switch (dwStyle & FWL_STYLEEXT_CKB_SignShapeMask) {
    case FWL_STYLEEXT_CKB_SignShapeCheck:
      DrawSignCheck(pGraphics, rtSign, dwColor, matrix);
      break;
    case FWL_STYLEEXT_CKB_SignShapeCircle:
      DrawSignCircle(pGraphics, rtSign, dwColor, matrix);
      break;
    case FWL_STYLEEXT_CKB_SignShapeCross:
      DrawSignCross(pGraphics, rtSign, dwColor, matrix);
      break;
    case FWL_STYLEEXT_CKB_SignShapeDiamond:
      DrawSignDiamond(pGraphics, rtSign, dwColor, matrix);
      break;
    case FWL_STYLEEXT_CKB_SignShapeSquare:
      DrawSignSquare(pGraphics, rtSign, dwColor, matrix);
      break;
    case FWL_STYLEEXT_CKB_SignShapeStar:
      DrawSignStar(pGraphics, rtSign, dwColor, matrix);
      break;
    default:
      break;
  }
}
