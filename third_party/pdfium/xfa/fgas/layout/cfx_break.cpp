// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/layout/cfx_break.h"

#include <algorithm>
#include <vector>

#include "third_party/base/stl_util.h"
#include "xfa/fgas/font/cfgas_gefont.h"

const float CFX_Break::kConversionFactor = 20000.0f;
const int CFX_Break::kMinimumTabWidth = 160000;

CFX_Break::CFX_Break(uint32_t dwLayoutStyles)
    : m_dwLayoutStyles(dwLayoutStyles) {
  m_pCurLine = &m_Line[0];
}

CFX_Break::~CFX_Break() = default;

void CFX_Break::Reset() {
  m_eCharType = FX_CHARTYPE::kUnknown;
  m_Line[0].Clear();
  m_Line[1].Clear();
}

void CFX_Break::SetLayoutStyles(uint32_t dwLayoutStyles) {
  m_dwLayoutStyles = dwLayoutStyles;
  m_bSingleLine = (m_dwLayoutStyles & FX_LAYOUTSTYLE_SingleLine) != 0;
  m_bCombText = (m_dwLayoutStyles & FX_LAYOUTSTYLE_CombText) != 0;
}

void CFX_Break::SetHorizontalScale(int32_t iScale) {
  iScale = std::max(iScale, 0);
  if (m_iHorizontalScale == iScale)
    return;

  SetBreakStatus();
  m_iHorizontalScale = iScale;
}

void CFX_Break::SetVerticalScale(int32_t iScale) {
  if (iScale < 0)
    iScale = 0;
  if (m_iVerticalScale == iScale)
    return;

  SetBreakStatus();
  m_iVerticalScale = iScale;
}

void CFX_Break::SetFont(const RetainPtr<CFGAS_GEFont>& pFont) {
  if (!pFont || pFont == m_pFont)
    return;

  SetBreakStatus();
  m_pFont = pFont;
  FontChanged();
}

void CFX_Break::SetFontSize(float fFontSize) {
  int32_t iFontSize = FXSYS_round(fFontSize * 20.0f);
  if (m_iFontSize == iFontSize)
    return;

  SetBreakStatus();
  m_iFontSize = iFontSize;
  FontChanged();
}

void CFX_Break::SetBreakStatus() {
  ++m_dwIdentity;
  if (m_pCurLine->m_LineChars.empty())
    return;

  CFX_Char* tc = m_pCurLine->GetChar(m_pCurLine->m_LineChars.size() - 1);
  if (tc->m_dwStatus == CFX_BreakType::None)
    tc->m_dwStatus = CFX_BreakType::Piece;
}

FX_CHARTYPE CFX_Break::GetUnifiedCharType(FX_CHARTYPE chartype) const {
  return chartype >= FX_CHARTYPE::kArabicAlef ? FX_CHARTYPE::kArabic : chartype;
}

void CFX_Break::FontChanged() {
  m_iDefChar = 0;
  if (!m_pFont || m_wDefChar == 0xFEFF)
    return;

  m_pFont->GetCharWidth(m_wDefChar, &m_iDefChar);
  m_iDefChar *= m_iFontSize;
}

void CFX_Break::SetTabWidth(float fTabWidth) {
  // Note, the use of max here was only done in the TxtBreak code. Leaving this
  // in for the RTFBreak code for consistency. If we see issues with tab widths
  // we may need to fix this.
  m_iTabWidth =
      std::max(FXSYS_round(fTabWidth * kConversionFactor), kMinimumTabWidth);
}

void CFX_Break::SetDefaultChar(wchar_t wch) {
  m_wDefChar = wch;
  m_iDefChar = 0;
  if (m_wDefChar == 0xFEFF || !m_pFont)
    return;

  m_pFont->GetCharWidth(m_wDefChar, &m_iDefChar);
  if (m_iDefChar < 0)
    m_iDefChar = 0;
  else
    m_iDefChar *= m_iFontSize;
}

void CFX_Break::SetParagraphBreakChar(wchar_t wch) {
  if (wch != L'\r' && wch != L'\n')
    return;
  m_wParagraphBreakChar = wch;
}

void CFX_Break::SetLineBreakTolerance(float fTolerance) {
  m_iTolerance = FXSYS_round(fTolerance * kConversionFactor);
}

void CFX_Break::SetCharSpace(float fCharSpace) {
  m_iCharSpace = FXSYS_round(fCharSpace * kConversionFactor);
}

void CFX_Break::SetLineBoundary(float fLineStart, float fLineEnd) {
  if (fLineStart > fLineEnd)
    return;

  m_iLineStart = FXSYS_round(fLineStart * kConversionFactor);
  m_iLineWidth = FXSYS_round(fLineEnd * kConversionFactor);
  m_pCurLine->m_iStart = std::min(m_pCurLine->m_iStart, m_iLineWidth);
  m_pCurLine->m_iStart = std::max(m_pCurLine->m_iStart, m_iLineStart);
}

CFX_Char* CFX_Break::GetLastChar(int32_t index,
                                 bool bOmitChar,
                                 bool bRichText) const {
  std::vector<CFX_Char>& tca = m_pCurLine->m_LineChars;
  if (!pdfium::IndexInBounds(tca, index))
    return nullptr;

  int32_t iStart = pdfium::CollectionSize<int32_t>(tca) - 1;
  while (iStart > -1) {
    CFX_Char* pTC = &tca[iStart--];
    if (((bRichText && pTC->m_iCharWidth < 0) || bOmitChar) &&
        pTC->GetCharType() == FX_CHARTYPE::kCombination) {
      continue;
    }
    if (--index < 0)
      return pTC;
  }
  return nullptr;
}

int32_t CFX_Break::CountBreakPieces() const {
  return HasLine() ? pdfium::CollectionSize<int32_t>(
                         m_Line[m_iReadyLineIndex].m_LinePieces)
                   : 0;
}

const CFX_BreakPiece* CFX_Break::GetBreakPieceUnstable(int32_t index) const {
  if (!HasLine())
    return nullptr;
  if (!pdfium::IndexInBounds(m_Line[m_iReadyLineIndex].m_LinePieces, index))
    return nullptr;
  return &m_Line[m_iReadyLineIndex].m_LinePieces[index];
}

void CFX_Break::ClearBreakPieces() {
  if (HasLine())
    m_Line[m_iReadyLineIndex].Clear();
  m_iReadyLineIndex = -1;
}
