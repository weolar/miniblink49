// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_pushbuttontp.h"

#include "xfa/fwl/cfwl_pushbutton.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/ifwl_themeprovider.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"

#define PUSHBUTTON_SIZE_Corner 2

CFWL_PushButtonTP::CFWL_PushButtonTP() : m_pThemeData(new PBThemeData) {
  SetThemeData();
}

CFWL_PushButtonTP::~CFWL_PushButtonTP() {}

void CFWL_PushButtonTP::DrawBackground(const CFWL_ThemeBackground& pParams) {
  switch (pParams.m_iPart) {
    case CFWL_Part::Border: {
      DrawBorder(pParams.m_pGraphics.Get(), pParams.m_rtPart, pParams.m_matrix);
      break;
    }
    case CFWL_Part::Background: {
      const CFX_RectF& rect = pParams.m_rtPart;
      float fRight = rect.right();
      float fBottom = rect.bottom();

      CXFA_GEPath strokePath;
      strokePath.MoveTo(
          CFX_PointF(rect.left + PUSHBUTTON_SIZE_Corner, rect.top));
      strokePath.LineTo(CFX_PointF(fRight - PUSHBUTTON_SIZE_Corner, rect.top));
      strokePath.LineTo(CFX_PointF(fRight, rect.top + PUSHBUTTON_SIZE_Corner));
      strokePath.LineTo(CFX_PointF(fRight, fBottom - PUSHBUTTON_SIZE_Corner));
      strokePath.LineTo(CFX_PointF(fRight - PUSHBUTTON_SIZE_Corner, fBottom));
      strokePath.LineTo(
          CFX_PointF(rect.left + PUSHBUTTON_SIZE_Corner, fBottom));
      strokePath.LineTo(
          CFX_PointF(rect.left, fBottom - PUSHBUTTON_SIZE_Corner));
      strokePath.LineTo(
          CFX_PointF(rect.left, rect.top + PUSHBUTTON_SIZE_Corner));
      strokePath.LineTo(
          CFX_PointF(rect.left + PUSHBUTTON_SIZE_Corner, rect.top));

      CXFA_GEPath fillPath;
      fillPath.AddSubpath(&strokePath);

      CXFA_Graphics* pGraphics = pParams.m_pGraphics.Get();
      pGraphics->SaveGraphState();

      CFX_RectF rtInner(rect);
      rtInner.Deflate(PUSHBUTTON_SIZE_Corner + 1, PUSHBUTTON_SIZE_Corner + 1,
                      PUSHBUTTON_SIZE_Corner, PUSHBUTTON_SIZE_Corner);
      fillPath.AddRectangle(rtInner.left, rtInner.top, rtInner.width,
                            rtInner.height);

      int32_t iColor = GetColorID(pParams.m_dwStates);
      FillSolidRect(pGraphics, m_pThemeData->clrEnd[iColor], rect,
                    pParams.m_matrix);

      pGraphics->SetStrokeColor(CXFA_GEColor(m_pThemeData->clrBorder[iColor]));
      pGraphics->StrokePath(&strokePath, &pParams.m_matrix);

      fillPath.Clear();
      fillPath.AddRectangle(rtInner.left, rtInner.top, rtInner.width,
                            rtInner.height);

      pGraphics->SetFillColor(CXFA_GEColor(m_pThemeData->clrFill[iColor]));
      pGraphics->FillPath(&fillPath, FXFILL_WINDING, &pParams.m_matrix);
      if (pParams.m_dwStates & CFWL_PartState_Focused) {
        rtInner.Inflate(1, 1, 0, 0);
        DrawFocus(pGraphics, rtInner, pParams.m_matrix);
      }
      pGraphics->RestoreGraphState();
      break;
    }
    default:
      break;
  }
}

void CFWL_PushButtonTP::SetThemeData() {
  m_pThemeData->clrBorder[0] = ArgbEncode(255, 0, 60, 116);
  m_pThemeData->clrBorder[1] = ArgbEncode(255, 0, 60, 116);
  m_pThemeData->clrBorder[2] = ArgbEncode(255, 0, 60, 116);
  m_pThemeData->clrBorder[3] = ArgbEncode(255, 0, 60, 116);
  m_pThemeData->clrBorder[4] = ArgbEncode(255, 201, 199, 186);
  m_pThemeData->clrStart[0] = ArgbEncode(255, 255, 255, 255);
  m_pThemeData->clrStart[1] = ArgbEncode(255, 209, 204, 193);
  m_pThemeData->clrStart[2] = ArgbEncode(255, 255, 240, 207);
  m_pThemeData->clrStart[3] = ArgbEncode(255, 206, 231, 255);
  m_pThemeData->clrStart[4] = ArgbEncode(255, 245, 244, 234);
  m_pThemeData->clrEnd[0] = ArgbEncode(255, 214, 208, 197);
  m_pThemeData->clrEnd[1] = ArgbEncode(255, 242, 241, 238);
  m_pThemeData->clrEnd[2] = ArgbEncode(255, 229, 151, 0);
  m_pThemeData->clrEnd[3] = ArgbEncode(255, 105, 130, 238);
  m_pThemeData->clrEnd[4] = ArgbEncode(255, 245, 244, 234);
  m_pThemeData->clrFill[0] = ArgbEncode(255, 255, 255, 255);
  m_pThemeData->clrFill[1] = ArgbEncode(255, 226, 225, 218);
  m_pThemeData->clrFill[2] = ArgbEncode(255, 255, 255, 255);
  m_pThemeData->clrFill[3] = ArgbEncode(255, 255, 255, 255);
  m_pThemeData->clrFill[4] = ArgbEncode(255, 245, 244, 234);
}

int32_t CFWL_PushButtonTP::GetColorID(uint32_t dwStates) const {
  int32_t color = 0;
  if (dwStates & CFWL_PartState_Disabled)
    color += 4;
  if (dwStates & CFWL_PartState_Default) {
    color += 3;
  } else {
    if (dwStates & CFWL_PartState_Hovered)
      color += 2;
    if (dwStates & CFWL_PartState_Pressed)
      color += 1;
  }
  return color;
}
