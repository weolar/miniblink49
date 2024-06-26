// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_widgettp.h"

#include <algorithm>
#include <utility>

#include "third_party/base/ptr_util.h"
#include "xfa/fde/cfde_textout.h"
#include "xfa/fgas/font/cfgas_fontmgr.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_themepart.h"
#include "xfa/fwl/cfwl_themetext.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fwl/ifwl_themeprovider.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"
#include "xfa/fxgraphics/cxfa_geshading.h"

CFWL_WidgetTP::CFWL_WidgetTP()
    : m_dwRefCount(1), m_pFDEFont(nullptr), m_pColorData(nullptr) {}

CFWL_WidgetTP::~CFWL_WidgetTP() {}

void CFWL_WidgetTP::Initialize() {}

void CFWL_WidgetTP::Finalize() {
  if (m_pTextOut)
    FinalizeTTO();
}

void CFWL_WidgetTP::DrawBackground(const CFWL_ThemeBackground& pParams) {}

void CFWL_WidgetTP::DrawText(const CFWL_ThemeText& pParams) {
  if (!m_pTextOut)
    InitTTO();

  int32_t iLen = pParams.m_wsText.GetLength();
  if (iLen <= 0)
    return;

  CXFA_Graphics* pGraphics = pParams.m_pGraphics;
  m_pTextOut->SetStyles(pParams.m_dwTTOStyles);
  m_pTextOut->SetAlignment(pParams.m_iTTOAlign);

  CFX_Matrix matrix = pParams.m_matrix;
  matrix.Concat(*pGraphics->GetMatrix());
  m_pTextOut->SetMatrix(matrix);
  m_pTextOut->DrawLogicText(pGraphics->GetRenderDevice(),
                            WideStringView(pParams.m_wsText.c_str(), iLen),
                            pParams.m_rtPart);
}

const RetainPtr<CFGAS_GEFont>& CFWL_WidgetTP::GetFont() const {
  return m_pFDEFont;
}

void CFWL_WidgetTP::InitializeArrowColorData() {
  if (m_pColorData)
    return;

  m_pColorData = pdfium::MakeUnique<CColorData>();
  m_pColorData->clrBorder[0] = ArgbEncode(255, 202, 216, 249);
  m_pColorData->clrBorder[1] = ArgbEncode(255, 171, 190, 233);
  m_pColorData->clrBorder[2] = ArgbEncode(255, 135, 147, 219);
  m_pColorData->clrBorder[3] = ArgbEncode(255, 172, 168, 153);
  m_pColorData->clrStart[0] = ArgbEncode(255, 225, 234, 254);
  m_pColorData->clrStart[1] = ArgbEncode(255, 253, 255, 255);
  m_pColorData->clrStart[2] = ArgbEncode(255, 110, 142, 241);
  m_pColorData->clrStart[3] = ArgbEncode(255, 254, 254, 251);
  m_pColorData->clrEnd[0] = ArgbEncode(255, 175, 204, 251);
  m_pColorData->clrEnd[1] = ArgbEncode(255, 185, 218, 251);
  m_pColorData->clrEnd[2] = ArgbEncode(255, 210, 222, 235);
  m_pColorData->clrEnd[3] = ArgbEncode(255, 243, 241, 236);
  m_pColorData->clrSign[0] = ArgbEncode(255, 77, 97, 133);
  m_pColorData->clrSign[1] = ArgbEncode(255, 77, 97, 133);
  m_pColorData->clrSign[2] = ArgbEncode(255, 77, 97, 133);
  m_pColorData->clrSign[3] = ArgbEncode(255, 128, 128, 128);
}


void CFWL_WidgetTP::InitTTO() {
  if (m_pTextOut)
    return;

  m_pFDEFont = CFWL_FontManager::GetInstance()->FindFont(L"Helvetica", 0, 0);
  m_pTextOut = pdfium::MakeUnique<CFDE_TextOut>();
  m_pTextOut->SetFont(m_pFDEFont);
  m_pTextOut->SetFontSize(FWLTHEME_CAPACITY_FontSize);
  m_pTextOut->SetTextColor(FWLTHEME_CAPACITY_TextColor);
}

void CFWL_WidgetTP::FinalizeTTO() {
  m_pTextOut.reset();
}

void CFWL_WidgetTP::DrawBorder(CXFA_Graphics* pGraphics,
                               const CFX_RectF& rect,
                               const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;

  CXFA_GEPath path;
  path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
  path.AddRectangle(rect.left + 1, rect.top + 1, rect.width - 2,
                    rect.height - 2);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(CXFA_GEColor(ArgbEncode(255, 0, 0, 0)));
  pGraphics->FillPath(&path, FXFILL_ALTERNATE, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_WidgetTP::FillBackground(CXFA_Graphics* pGraphics,
                                   const CFX_RectF& rect,
                                   const CFX_Matrix& matrix) {
  FillSolidRect(pGraphics, FWLTHEME_COLOR_Background, rect, matrix);
}

void CFWL_WidgetTP::FillSolidRect(CXFA_Graphics* pGraphics,
                                  FX_ARGB fillColor,
                                  const CFX_RectF& rect,
                                  const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;

  CXFA_GEPath path;
  path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
  pGraphics->SaveGraphState();
  pGraphics->SetFillColor(CXFA_GEColor(fillColor));
  pGraphics->FillPath(&path, FXFILL_WINDING, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_WidgetTP::DrawFocus(CXFA_Graphics* pGraphics,
                              const CFX_RectF& rect,
                              const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;

  CXFA_GEPath path;
  path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
  pGraphics->SaveGraphState();
  pGraphics->SetStrokeColor(CXFA_GEColor(0xFF000000));
  static constexpr float kDashPattern[2] = {1, 1};
  pGraphics->SetLineDash(0.0f, kDashPattern, FX_ArraySize(kDashPattern));
  pGraphics->StrokePath(&path, &matrix);
  pGraphics->RestoreGraphState();
}

void CFWL_WidgetTP::DrawArrow(CXFA_Graphics* pGraphics,
                              const CFX_RectF& rect,
                              FWLTHEME_DIRECTION eDict,
                              FX_ARGB argSign,
                              const CFX_Matrix& matrix) {
  bool bVert =
      (eDict == FWLTHEME_DIRECTION_Up || eDict == FWLTHEME_DIRECTION_Down);
  float fLeft = (float)(((rect.width - (bVert ? 9 : 6)) / 2 + rect.left) + 0.5);
  float fTop = (float)(((rect.height - (bVert ? 6 : 9)) / 2 + rect.top) + 0.5);
  CXFA_GEPath path;
  switch (eDict) {
    case FWLTHEME_DIRECTION_Down: {
      path.MoveTo(CFX_PointF(fLeft, fTop + 1));
      path.LineTo(CFX_PointF(fLeft + 4, fTop + 5));
      path.LineTo(CFX_PointF(fLeft + 8, fTop + 1));
      path.LineTo(CFX_PointF(fLeft + 7, fTop));
      path.LineTo(CFX_PointF(fLeft + 4, fTop + 3));
      path.LineTo(CFX_PointF(fLeft + 1, fTop));
      break;
    }
    case FWLTHEME_DIRECTION_Up: {
      path.MoveTo(CFX_PointF(fLeft, fTop + 4));
      path.LineTo(CFX_PointF(fLeft + 4, fTop));
      path.LineTo(CFX_PointF(fLeft + 8, fTop + 4));
      path.LineTo(CFX_PointF(fLeft + 7, fTop + 5));
      path.LineTo(CFX_PointF(fLeft + 4, fTop + 2));
      path.LineTo(CFX_PointF(fLeft + 1, fTop + 5));
      break;
    }
    case FWLTHEME_DIRECTION_Right: {
      path.MoveTo(CFX_PointF(fLeft + 1, fTop));
      path.LineTo(CFX_PointF(fLeft + 5, fTop + 4));
      path.LineTo(CFX_PointF(fLeft + 1, fTop + 8));
      path.LineTo(CFX_PointF(fLeft, fTop + 7));
      path.LineTo(CFX_PointF(fLeft + 3, fTop + 4));
      path.LineTo(CFX_PointF(fLeft, fTop + 1));
      break;
    }
    case FWLTHEME_DIRECTION_Left: {
      path.MoveTo(CFX_PointF(fLeft, fTop + 4));
      path.LineTo(CFX_PointF(fLeft + 4, fTop));
      path.LineTo(CFX_PointF(fLeft + 5, fTop + 1));
      path.LineTo(CFX_PointF(fLeft + 2, fTop + 4));
      path.LineTo(CFX_PointF(fLeft + 5, fTop + 7));
      path.LineTo(CFX_PointF(fLeft + 4, fTop + 8));
      break;
    }
  }
  pGraphics->SetFillColor(CXFA_GEColor(argSign));
  pGraphics->FillPath(&path, FXFILL_WINDING, &matrix);
}

void CFWL_WidgetTP::DrawBtn(CXFA_Graphics* pGraphics,
                            const CFX_RectF& rect,
                            FWLTHEME_STATE eState,
                            const CFX_Matrix& matrix) {
  InitializeArrowColorData();
  FillSolidRect(pGraphics, m_pColorData->clrEnd[eState - 1], rect, matrix);

  CXFA_GEPath path;
  path.AddRectangle(rect.left, rect.top, rect.width, rect.height);
  pGraphics->SetStrokeColor(CXFA_GEColor(m_pColorData->clrBorder[eState - 1]));
  pGraphics->StrokePath(&path, &matrix);
}

void CFWL_WidgetTP::DrawArrowBtn(CXFA_Graphics* pGraphics,
                                 const CFX_RectF& rect,
                                 FWLTHEME_DIRECTION eDict,
                                 FWLTHEME_STATE eState,
                                 const CFX_Matrix& matrix) {
  DrawBtn(pGraphics, rect, eState, matrix);
  InitializeArrowColorData();
  DrawArrow(pGraphics, rect, eDict, m_pColorData->clrSign[eState - 1], matrix);
}

CFWL_FontData::CFWL_FontData() : m_dwStyles(0), m_dwCodePage(0) {}

CFWL_FontData::~CFWL_FontData() {}

bool CFWL_FontData::Equal(WideStringView wsFontFamily,
                          uint32_t dwFontStyles,
                          uint16_t wCodePage) {
  return m_wsFamily == wsFontFamily && m_dwStyles == dwFontStyles &&
         m_dwCodePage == wCodePage;
}

bool CFWL_FontData::LoadFont(WideStringView wsFontFamily,
                             uint32_t dwFontStyles,
                             uint16_t dwCodePage) {
  m_wsFamily = wsFontFamily;
  m_dwStyles = dwFontStyles;
  m_dwCodePage = dwCodePage;
  if (!m_pFontMgr) {
    m_pFontMgr = pdfium::MakeUnique<CFGAS_FontMgr>();
    if (!m_pFontMgr->EnumFonts())
      m_pFontMgr = nullptr;
  }

  // TODO(tsepez): check usage of c_str() below.
  m_pFont = CFGAS_GEFont::LoadFont(wsFontFamily.unterminated_c_str(),
                                   dwFontStyles, dwCodePage, m_pFontMgr.get());
  return !!m_pFont;
}

RetainPtr<CFGAS_GEFont> CFWL_FontData::GetFont() const {
  return m_pFont;
}

CFWL_FontManager* CFWL_FontManager::s_FontManager = nullptr;
CFWL_FontManager* CFWL_FontManager::GetInstance() {
  if (!s_FontManager)
    s_FontManager = new CFWL_FontManager;
  return s_FontManager;
}

void CFWL_FontManager::DestroyInstance() {
  delete s_FontManager;
  s_FontManager = nullptr;
}

CFWL_FontManager::CFWL_FontManager() {}

CFWL_FontManager::~CFWL_FontManager() {}

RetainPtr<CFGAS_GEFont> CFWL_FontManager::FindFont(WideStringView wsFontFamily,
                                                   uint32_t dwFontStyles,
                                                   uint16_t wCodePage) {
  for (const auto& pData : m_FontsArray) {
    if (pData->Equal(wsFontFamily, dwFontStyles, wCodePage))
      return pData->GetFont();
  }
  auto pFontData = pdfium::MakeUnique<CFWL_FontData>();
  if (!pFontData->LoadFont(wsFontFamily, dwFontStyles, wCodePage))
    return nullptr;

  m_FontsArray.push_back(std::move(pFontData));
  return m_FontsArray.back()->GetFont();
}

void FWLTHEME_Release() {
  CFWL_FontManager::DestroyInstance();
}
