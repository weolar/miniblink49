// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/layout/cfx_rtfbreak.h"

#include <algorithm>

#include "core/fxge/cfx_renderdevice.h"
#include "third_party/base/numerics/safe_math.h"
#include "third_party/base/stl_util.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fgas/layout/cfx_char.h"
#include "xfa/fgas/layout/fx_arabic.h"
#include "xfa/fgas/layout/fx_linebreak.h"

CFX_RTFBreak::CFX_RTFBreak(uint32_t dwLayoutStyles)
    : CFX_Break(dwLayoutStyles),
      m_bPagination(false),
      m_iAlignment(CFX_RTFLineAlignment::Left) {
  SetBreakStatus();
  m_bPagination = !!(m_dwLayoutStyles & FX_LAYOUTSTYLE_Pagination);
}

CFX_RTFBreak::~CFX_RTFBreak() {}

void CFX_RTFBreak::SetLineStartPos(float fLinePos) {
  int32_t iLinePos = FXSYS_round(fLinePos * kConversionFactor);
  iLinePos = std::min(iLinePos, m_iLineWidth);
  iLinePos = std::max(iLinePos, m_iLineStart);
  m_pCurLine->m_iStart = iLinePos;
}

void CFX_RTFBreak::AddPositionedTab(float fTabPos) {
  int32_t iTabPos = std::min(
      FXSYS_round(fTabPos * kConversionFactor) + m_iLineStart, m_iLineWidth);
  auto it = std::lower_bound(m_PositionedTabs.begin(), m_PositionedTabs.end(),
                             iTabPos);
  if (it != m_PositionedTabs.end() && *it == iTabPos)
    return;
  m_PositionedTabs.insert(it, iTabPos);
}

void CFX_RTFBreak::SetUserData(const RetainPtr<CXFA_TextUserData>& pUserData) {
  if (m_pUserData == pUserData)
    return;

  SetBreakStatus();
  m_pUserData = pUserData;
}

int32_t CFX_RTFBreak::GetLastPositionedTab() const {
  return m_PositionedTabs.empty() ? m_iLineStart : m_PositionedTabs.back();
}

bool CFX_RTFBreak::GetPositionedTab(int32_t* iTabPos) const {
  auto it = std::upper_bound(m_PositionedTabs.begin(), m_PositionedTabs.end(),
                             *iTabPos);
  if (it == m_PositionedTabs.end())
    return false;

  *iTabPos = *it;
  return true;
}

CFX_BreakType CFX_RTFBreak::AppendChar(wchar_t wch) {
  ASSERT(m_pFont);
  ASSERT(m_pCurLine);

  FX_CHARTYPE chartype = FX_GetCharType(wch);
  m_pCurLine->m_LineChars.emplace_back(wch, m_iHorizontalScale,
                                       m_iVerticalScale);
  CFX_Char* pCurChar = &m_pCurLine->m_LineChars.back();
  pCurChar->m_iFontSize = m_iFontSize;
  pCurChar->m_dwIdentity = m_dwIdentity;
  pCurChar->m_pUserData = m_pUserData;

  CFX_BreakType dwRet1 = CFX_BreakType::None;
  if (chartype != FX_CHARTYPE::kCombination &&
      GetUnifiedCharType(m_eCharType) != GetUnifiedCharType(chartype) &&
      m_eCharType != FX_CHARTYPE::kUnknown &&
      m_pCurLine->GetLineEnd() > m_iLineWidth + m_iTolerance &&
      (m_eCharType != FX_CHARTYPE::kSpace ||
       chartype != FX_CHARTYPE::kControl)) {
    dwRet1 = EndBreak(CFX_BreakType::Line);
    if (!m_pCurLine->m_LineChars.empty())
      pCurChar = &m_pCurLine->m_LineChars.back();
  }

  CFX_BreakType dwRet2 = CFX_BreakType::None;
  switch (chartype) {
    case FX_CHARTYPE::kTab:
      AppendChar_Tab(pCurChar);
      break;
    case FX_CHARTYPE::kControl:
      dwRet2 = AppendChar_Control(pCurChar);
      break;
    case FX_CHARTYPE::kCombination:
      AppendChar_Combination(pCurChar);
      break;
    case FX_CHARTYPE::kArabicAlef:
    case FX_CHARTYPE::kArabicSpecial:
    case FX_CHARTYPE::kArabicDistortion:
    case FX_CHARTYPE::kArabicNormal:
    case FX_CHARTYPE::kArabicForm:
    case FX_CHARTYPE::kArabic:
      dwRet2 = AppendChar_Arabic(pCurChar);
      break;
    case FX_CHARTYPE::kUnknown:
    case FX_CHARTYPE::kSpace:
    case FX_CHARTYPE::kNumeric:
    case FX_CHARTYPE::kNormal:
    default:
      dwRet2 = AppendChar_Others(pCurChar);
      break;
  }

  m_eCharType = chartype;
  return std::max(dwRet1, dwRet2);
}

void CFX_RTFBreak::AppendChar_Combination(CFX_Char* pCurChar) {
  FX_SAFE_INT32 iCharWidth = 0;
  int32_t iCharWidthOut;
  if (m_pFont->GetCharWidth(pCurChar->char_code(), &iCharWidthOut))
    iCharWidth = iCharWidthOut;

  iCharWidth *= m_iFontSize;
  iCharWidth *= m_iHorizontalScale;
  iCharWidth /= 100;
  CFX_Char* pLastChar = GetLastChar(0, false, true);
  if (pLastChar && pLastChar->GetCharType() > FX_CHARTYPE::kCombination)
    iCharWidth *= -1;
  else
    m_eCharType = FX_CHARTYPE::kCombination;

  int32_t iCharWidthValid = iCharWidth.ValueOrDefault(0);
  pCurChar->m_iCharWidth = iCharWidthValid;
  if (iCharWidthValid > 0) {
    pdfium::base::CheckedNumeric<int32_t> checked_width = m_pCurLine->m_iWidth;
    checked_width += iCharWidthValid;
    if (!checked_width.IsValid())
      return;

    m_pCurLine->m_iWidth = checked_width.ValueOrDie();
  }
}

void CFX_RTFBreak::AppendChar_Tab(CFX_Char* pCurChar) {
  if (!(m_dwLayoutStyles & FX_LAYOUTSTYLE_ExpandTab))
    return;

  int32_t& iLineWidth = m_pCurLine->m_iWidth;
  int32_t iCharWidth = iLineWidth;
  FX_SAFE_INT32 iSafeCharWidth;
  if (GetPositionedTab(&iCharWidth)) {
    iSafeCharWidth = iCharWidth;
  } else {
    // Tab width is >= 160000, so this part does not need to be checked.
    ASSERT(m_iTabWidth >= kMinimumTabWidth);
    iSafeCharWidth = iLineWidth / m_iTabWidth + 1;
    iSafeCharWidth *= m_iTabWidth;
  }
  iSafeCharWidth -= iLineWidth;

  iCharWidth = iSafeCharWidth.ValueOrDefault(0);

  pCurChar->m_iCharWidth = iCharWidth;
  iLineWidth += iCharWidth;
}

CFX_BreakType CFX_RTFBreak::AppendChar_Control(CFX_Char* pCurChar) {
  CFX_BreakType dwRet2 = CFX_BreakType::None;
  switch (pCurChar->char_code()) {
    case L'\v':
    case 0x2028:
      dwRet2 = CFX_BreakType::Line;
      break;
    case L'\f':
      dwRet2 = CFX_BreakType::Page;
      break;
    case 0x2029:
      dwRet2 = CFX_BreakType::Paragraph;
      break;
    default:
      if (pCurChar->char_code() == m_wParagraphBreakChar)
        dwRet2 = CFX_BreakType::Paragraph;
      break;
  }
  if (dwRet2 != CFX_BreakType::None)
    dwRet2 = EndBreak(dwRet2);

  return dwRet2;
}

CFX_BreakType CFX_RTFBreak::AppendChar_Arabic(CFX_Char* pCurChar) {
  CFX_Char* pLastChar = nullptr;
  wchar_t wForm;
  bool bAlef = false;
  if (m_eCharType >= FX_CHARTYPE::kArabicAlef &&
      m_eCharType <= FX_CHARTYPE::kArabicDistortion) {
    pLastChar = GetLastChar(1, false, true);
    if (pLastChar) {
      m_pCurLine->m_iWidth -= pLastChar->m_iCharWidth;
      CFX_Char* pPrevChar = GetLastChar(2, false, true);
      wForm = pdfium::arabic::GetFormChar(pLastChar, pPrevChar, pCurChar);
      bAlef = (wForm == 0xFEFF &&
               pLastChar->GetCharType() == FX_CHARTYPE::kArabicAlef);
      FX_SAFE_INT32 iCharWidth;
      int32_t iCharWidthOut;
      if (m_pFont->GetCharWidth(wForm, &iCharWidthOut) ||
          m_pFont->GetCharWidth(pLastChar->char_code(), &iCharWidthOut)) {
        iCharWidth = iCharWidthOut;
      } else {
        iCharWidth = m_iDefChar;
      }

      iCharWidth *= m_iFontSize;
      iCharWidth *= m_iHorizontalScale;
      iCharWidth /= 100;

      int iCharWidthValid = iCharWidth.ValueOrDefault(0);
      pLastChar->m_iCharWidth = iCharWidthValid;

      pdfium::base::CheckedNumeric<int32_t> checked_width =
          m_pCurLine->m_iWidth;
      checked_width += iCharWidthValid;
      if (!checked_width.IsValid())
        return CFX_BreakType::None;

      m_pCurLine->m_iWidth = checked_width.ValueOrDie();
      iCharWidth = 0;
    }
  }

  wForm = pdfium::arabic::GetFormChar(pCurChar, bAlef ? nullptr : pLastChar,
                                      nullptr);
  FX_SAFE_INT32 iCharWidth;
  int32_t iCharWidthOut;
  if (m_pFont->GetCharWidth(wForm, &iCharWidthOut) ||
      m_pFont->GetCharWidth(pCurChar->char_code(), &iCharWidthOut)) {
    iCharWidth = iCharWidthOut;
  } else {
    iCharWidth = m_iDefChar;
  }

  iCharWidth *= m_iFontSize;
  iCharWidth *= m_iHorizontalScale;
  iCharWidth /= 100;

  int iCharWidthValid = iCharWidth.ValueOrDefault(0);
  pCurChar->m_iCharWidth = iCharWidthValid;

  pdfium::base::CheckedNumeric<int32_t> checked_width = m_pCurLine->m_iWidth;
  checked_width += iCharWidthValid;
  if (!checked_width.IsValid())
    return CFX_BreakType::None;

  m_pCurLine->m_iWidth = checked_width.ValueOrDie();
  m_pCurLine->m_iArabicChars++;

  if (m_pCurLine->GetLineEnd() > m_iLineWidth + m_iTolerance)
    return EndBreak(CFX_BreakType::Line);
  return CFX_BreakType::None;
}

CFX_BreakType CFX_RTFBreak::AppendChar_Others(CFX_Char* pCurChar) {
  FX_CHARTYPE chartype = pCurChar->GetCharType();
  wchar_t wForm = pCurChar->char_code();
  FX_SAFE_INT32 iCharWidth;
  int32_t iCharWidthOut;
  if (m_pFont->GetCharWidth(wForm, &iCharWidthOut))
    iCharWidth = iCharWidthOut;
  else
    iCharWidth = m_iDefChar;

  iCharWidth *= m_iFontSize;
  iCharWidth *= m_iHorizontalScale;
  iCharWidth /= 100;
  iCharWidth += m_iCharSpace;

  int iCharWidthValid = iCharWidth.ValueOrDefault(0);
  pCurChar->m_iCharWidth = iCharWidthValid;

  pdfium::base::CheckedNumeric<int32_t> checked_width = m_pCurLine->m_iWidth;
  checked_width += iCharWidthValid;
  if (!checked_width.IsValid())
    return CFX_BreakType::None;

  m_pCurLine->m_iWidth = checked_width.ValueOrDie();
  if (chartype != FX_CHARTYPE::kSpace &&
      m_pCurLine->GetLineEnd() > m_iLineWidth + m_iTolerance) {
    return EndBreak(CFX_BreakType::Line);
  }
  return CFX_BreakType::None;
}

CFX_BreakType CFX_RTFBreak::EndBreak(CFX_BreakType dwStatus) {
  ASSERT(dwStatus != CFX_BreakType::None);

  ++m_dwIdentity;
  if (!m_pCurLine->m_LinePieces.empty()) {
    if (dwStatus != CFX_BreakType::Piece)
      m_pCurLine->m_LinePieces.back().m_dwStatus = dwStatus;
    return m_pCurLine->m_LinePieces.back().m_dwStatus;
  }

  if (HasLine()) {
    if (!m_Line[m_iReadyLineIndex].m_LinePieces.empty()) {
      if (dwStatus != CFX_BreakType::Piece)
        m_Line[m_iReadyLineIndex].m_LinePieces.back().m_dwStatus = dwStatus;
      return m_Line[m_iReadyLineIndex].m_LinePieces.back().m_dwStatus;
    }
    return CFX_BreakType::None;
  }

  if (m_pCurLine->m_LineChars.empty())
    return CFX_BreakType::None;

  CFX_Char* tc = m_pCurLine->GetChar(m_pCurLine->m_LineChars.size() - 1);
  tc->m_dwStatus = dwStatus;
  if (dwStatus == CFX_BreakType::Piece)
    return dwStatus;

  m_iReadyLineIndex = m_pCurLine == &m_Line[0] ? 0 : 1;
  CFX_BreakLine* pNextLine = &m_Line[1 - m_iReadyLineIndex];
  bool bAllChars = m_iAlignment == CFX_RTFLineAlignment::Justified ||
                   m_iAlignment == CFX_RTFLineAlignment::Distributed;

  if (!EndBreak_SplitLine(pNextLine, bAllChars, dwStatus)) {
    std::deque<FX_TPO> tpos;
    EndBreak_BidiLine(&tpos, dwStatus);
    if (!m_bPagination && m_iAlignment != CFX_RTFLineAlignment::Left)
      EndBreak_Alignment(tpos, bAllChars, dwStatus);
  }
  m_pCurLine = pNextLine;
  m_pCurLine->m_iStart = m_iLineStart;

  CFX_Char* pTC = GetLastChar(0, false, true);
  m_eCharType = pTC ? pTC->GetCharType() : FX_CHARTYPE::kUnknown;
  return dwStatus;
}

bool CFX_RTFBreak::EndBreak_SplitLine(CFX_BreakLine* pNextLine,
                                      bool bAllChars,
                                      CFX_BreakType dwStatus) {
  bool bDone = false;
  if (m_pCurLine->GetLineEnd() > m_iLineWidth + m_iTolerance) {
    const CFX_Char* tc =
        m_pCurLine->GetChar(m_pCurLine->m_LineChars.size() - 1);
    switch (tc->GetCharType()) {
      case FX_CHARTYPE::kTab:
      case FX_CHARTYPE::kControl:
      case FX_CHARTYPE::kSpace:
        break;
      default:
        SplitTextLine(m_pCurLine.Get(), pNextLine, !m_bPagination && bAllChars);
        bDone = true;
        break;
    }
  }

  if (!m_bPagination) {
    if (bAllChars && !bDone) {
      int32_t endPos = m_pCurLine->GetLineEnd();
      GetBreakPos(m_pCurLine->m_LineChars, bAllChars, true, &endPos);
    }
    return false;
  }

  const CFX_Char* pCurChars = m_pCurLine->m_LineChars.data();
  CFX_BreakPiece tp;
  tp.m_pChars = &m_pCurLine->m_LineChars;
  bool bNew = true;
  uint32_t dwIdentity = static_cast<uint32_t>(-1);
  int32_t iLast = pdfium::CollectionSize<int32_t>(m_pCurLine->m_LineChars) - 1;
  int32_t j = 0;
  for (int32_t i = 0; i <= iLast;) {
    const CFX_Char* pTC = pCurChars + i;
    if (bNew) {
      tp.m_iStartChar = i;
      tp.m_iStartPos += tp.m_iWidth;
      tp.m_iWidth = 0;
      tp.m_dwStatus = pTC->m_dwStatus;
      tp.m_iFontSize = pTC->m_iFontSize;
      tp.m_iHorizontalScale = pTC->horizonal_scale();
      tp.m_iVerticalScale = pTC->vertical_scale();
      dwIdentity = pTC->m_dwIdentity;
      tp.m_dwIdentity = dwIdentity;
      tp.m_pUserData = pTC->m_pUserData.As<CXFA_TextUserData>();
      j = i;
      bNew = false;
    }

    if (i == iLast || pTC->m_dwStatus != CFX_BreakType::None ||
        pTC->m_dwIdentity != dwIdentity) {
      tp.m_iChars = i - j;
      if (pTC->m_dwIdentity == dwIdentity) {
        tp.m_dwStatus = pTC->m_dwStatus;
        tp.m_iWidth += pTC->m_iCharWidth;
        tp.m_iChars += 1;
        ++i;
      }
      m_pCurLine->m_LinePieces.push_back(tp);
      bNew = true;
    } else {
      tp.m_iWidth += pTC->m_iCharWidth;
      ++i;
    }
  }
  return true;
}

void CFX_RTFBreak::EndBreak_BidiLine(std::deque<FX_TPO>* tpos,
                                     CFX_BreakType dwStatus) {
  CFX_Char* pTC;
  std::vector<CFX_Char>& chars = m_pCurLine->m_LineChars;
  if (!m_bPagination && m_pCurLine->m_iArabicChars > 0) {
    size_t iBidiNum = 0;
    for (size_t i = 0; i < m_pCurLine->m_LineChars.size(); ++i) {
      pTC = &chars[i];
      pTC->m_iBidiPos = static_cast<int32_t>(i);
      if (pTC->GetCharType() != FX_CHARTYPE::kControl)
        iBidiNum = i;
      if (i == 0)
        pTC->m_iBidiLevel = 1;
    }
    CFX_Char::BidiLine(&chars, iBidiNum + 1);
  } else {
    for (size_t i = 0; i < m_pCurLine->m_LineChars.size(); ++i) {
      pTC = &chars[i];
      pTC->m_iBidiLevel = 0;
      pTC->m_iBidiPos = 0;
      pTC->m_iBidiOrder = 0;
    }
  }

  CFX_BreakPiece tp;
  tp.m_dwStatus = CFX_BreakType::Piece;
  tp.m_iStartPos = m_pCurLine->m_iStart;
  tp.m_pChars = &chars;

  int32_t iBidiLevel = -1;
  int32_t iCharWidth;
  FX_TPO tpo;
  uint32_t dwIdentity = static_cast<uint32_t>(-1);
  int32_t i = 0;
  int32_t j = 0;
  int32_t iCount = pdfium::CollectionSize<int32_t>(m_pCurLine->m_LineChars);
  while (i < iCount) {
    pTC = &chars[i];
    if (iBidiLevel < 0) {
      iBidiLevel = pTC->m_iBidiLevel;
      iCharWidth = pTC->m_iCharWidth;
      tp.m_iWidth = iCharWidth < 1 ? 0 : iCharWidth;
      tp.m_iBidiLevel = iBidiLevel;
      tp.m_iBidiPos = pTC->m_iBidiOrder;
      tp.m_iFontSize = pTC->m_iFontSize;
      tp.m_iHorizontalScale = pTC->horizonal_scale();
      tp.m_iVerticalScale = pTC->vertical_scale();
      dwIdentity = pTC->m_dwIdentity;
      tp.m_dwIdentity = dwIdentity;
      tp.m_pUserData = pTC->m_pUserData.As<CXFA_TextUserData>();
      tp.m_dwStatus = CFX_BreakType::Piece;
      ++i;
    } else if (iBidiLevel != pTC->m_iBidiLevel ||
               pTC->m_dwIdentity != dwIdentity) {
      tp.m_iChars = i - tp.m_iStartChar;
      m_pCurLine->m_LinePieces.push_back(tp);

      tp.m_iStartPos += tp.m_iWidth;
      tp.m_iStartChar = i;
      tpo.index = j++;
      tpo.pos = tp.m_iBidiPos;
      tpos->push_back(tpo);
      iBidiLevel = -1;
    } else {
      iCharWidth = pTC->m_iCharWidth;
      if (iCharWidth > 0)
        tp.m_iWidth += iCharWidth;
      ++i;
    }
  }

  if (i > tp.m_iStartChar) {
    tp.m_dwStatus = dwStatus;
    tp.m_iChars = i - tp.m_iStartChar;
    m_pCurLine->m_LinePieces.push_back(tp);

    tpo.index = j;
    tpo.pos = tp.m_iBidiPos;
    tpos->push_back(tpo);
  }

  std::sort(tpos->begin(), tpos->end());
  int32_t iStartPos = m_pCurLine->m_iStart;
  for (const auto& it : *tpos) {
    CFX_BreakPiece& ttp = m_pCurLine->m_LinePieces[it.index];
    ttp.m_iStartPos = iStartPos;
    iStartPos += ttp.m_iWidth;
  }
}

void CFX_RTFBreak::EndBreak_Alignment(const std::deque<FX_TPO>& tpos,
                                      bool bAllChars,
                                      CFX_BreakType dwStatus) {
  int32_t iNetWidth = m_pCurLine->m_iWidth;
  int32_t iGapChars = 0;
  bool bFind = false;
  for (auto it = tpos.rbegin(); it != tpos.rend(); it++) {
    CFX_BreakPiece& ttp = m_pCurLine->m_LinePieces[it->index];
    if (!bFind)
      iNetWidth = ttp.GetEndPos();

    bool bArabic = FX_IsOdd(ttp.m_iBidiLevel);
    int32_t j = bArabic ? 0 : ttp.m_iChars - 1;
    while (j > -1 && j < ttp.m_iChars) {
      const CFX_Char* tc = ttp.GetChar(j);
      if (tc->m_eLineBreakType == FX_LINEBREAKTYPE::kDIRECT_BRK)
        ++iGapChars;

      if (!bFind || !bAllChars) {
        FX_CHARTYPE dwCharType = tc->GetCharType();
        if (dwCharType == FX_CHARTYPE::kSpace ||
            dwCharType == FX_CHARTYPE::kControl) {
          if (!bFind) {
            int32_t iCharWidth = tc->m_iCharWidth;
            if (bAllChars && iCharWidth > 0)
              iNetWidth -= iCharWidth;
          }
        } else {
          bFind = true;
          if (!bAllChars)
            break;
        }
      }
      j += bArabic ? 1 : -1;
    }
    if (!bAllChars && bFind)
      break;
  }

  int32_t iOffset = m_iLineWidth - iNetWidth;
  if (iGapChars > 0 && (m_iAlignment == CFX_RTFLineAlignment::Distributed ||
                        (m_iAlignment == CFX_RTFLineAlignment::Justified &&
                         dwStatus != CFX_BreakType::Paragraph))) {
    int32_t iStart = -1;
    for (const auto& tpo : tpos) {
      CFX_BreakPiece& ttp = m_pCurLine->m_LinePieces[tpo.index];
      if (iStart < 0)
        iStart = ttp.m_iStartPos;
      else
        ttp.m_iStartPos = iStart;

      for (int32_t j = 0; j < ttp.m_iChars; ++j) {
        CFX_Char* tc = ttp.GetChar(j);
        if (tc->m_eLineBreakType != FX_LINEBREAKTYPE::kDIRECT_BRK ||
            tc->m_iCharWidth < 0) {
          continue;
        }
        int32_t k = iOffset / iGapChars;
        tc->m_iCharWidth += k;
        ttp.m_iWidth += k;
        iOffset -= k;
        --iGapChars;
        if (iGapChars < 1)
          break;
      }
      iStart += ttp.m_iWidth;
    }
  } else if (m_iAlignment == CFX_RTFLineAlignment::Right ||
             m_iAlignment == CFX_RTFLineAlignment::Center) {
    if (m_iAlignment == CFX_RTFLineAlignment::Center)
      iOffset /= 2;
    if (iOffset > 0) {
      for (auto& ttp : m_pCurLine->m_LinePieces)
        ttp.m_iStartPos += iOffset;
    }
  }
}

int32_t CFX_RTFBreak::GetBreakPos(std::vector<CFX_Char>& tca,
                                  bool bAllChars,
                                  bool bOnlyBrk,
                                  int32_t* pEndPos) {
  int32_t iLength = pdfium::CollectionSize<int32_t>(tca) - 1;
  if (iLength < 1)
    return iLength;

  int32_t iBreak = -1;
  int32_t iBreakPos = -1;
  int32_t iIndirect = -1;
  int32_t iIndirectPos = -1;
  int32_t iLast = -1;
  int32_t iLastPos = -1;
  if (*pEndPos <= m_iLineWidth) {
    if (!bAllChars)
      return iLength;

    iBreak = iLength;
    iBreakPos = *pEndPos;
  }

  CFX_Char* pCharArray = tca.data();
  CFX_Char* pCur = pCharArray + iLength;
  --iLength;
  if (bAllChars)
    pCur->m_eLineBreakType = FX_LINEBREAKTYPE::kUNKNOWN;

  FX_BREAKPROPERTY nNext = FX_GetBreakProperty(pCur->char_code());
  int32_t iCharWidth = pCur->m_iCharWidth;
  if (iCharWidth > 0)
    *pEndPos -= iCharWidth;

  while (iLength >= 0) {
    pCur = pCharArray + iLength;
    FX_BREAKPROPERTY nCur = FX_GetBreakProperty(pCur->char_code());
    bool bNeedBreak = false;
    FX_LINEBREAKTYPE eType;
    if (nCur == FX_BREAKPROPERTY::kTB) {
      bNeedBreak = true;
      eType = nNext == FX_BREAKPROPERTY::kTB
                  ? FX_LINEBREAKTYPE::kPROHIBITED_BRK
                  : GetLineBreakTypeFromPair(nCur, nNext);
    } else {
      if (nCur == FX_BREAKPROPERTY::kSP)
        bNeedBreak = true;

      eType = nNext == FX_BREAKPROPERTY::kSP
                  ? FX_LINEBREAKTYPE::kPROHIBITED_BRK
                  : GetLineBreakTypeFromPair(nCur, nNext);
    }
    if (bAllChars)
      pCur->m_eLineBreakType = eType;

    if (!bOnlyBrk) {
      iCharWidth = pCur->m_iCharWidth;
      if (*pEndPos <= m_iLineWidth || bNeedBreak) {
        if (eType == FX_LINEBREAKTYPE::kDIRECT_BRK && iBreak < 0) {
          iBreak = iLength;
          iBreakPos = *pEndPos;
          if (!bAllChars)
            return iLength;
        } else if (eType == FX_LINEBREAKTYPE::kINDIRECT_BRK && iIndirect < 0) {
          iIndirect = iLength;
          iIndirectPos = *pEndPos;
        }
        if (iLast < 0) {
          iLast = iLength;
          iLastPos = *pEndPos;
        }
      }
      if (iCharWidth > 0)
        *pEndPos -= iCharWidth;
    }
    nNext = nCur;
    --iLength;
  }
  if (bOnlyBrk)
    return 0;

  if (iBreak > -1) {
    *pEndPos = iBreakPos;
    return iBreak;
  }
  if (iIndirect > -1) {
    *pEndPos = iIndirectPos;
    return iIndirect;
  }
  if (iLast > -1) {
    *pEndPos = iLastPos;
    return iLast;
  }
  return 0;
}

void CFX_RTFBreak::SplitTextLine(CFX_BreakLine* pCurLine,
                                 CFX_BreakLine* pNextLine,
                                 bool bAllChars) {
  ASSERT(pCurLine);
  ASSERT(pNextLine);

  if (pCurLine->m_LineChars.size() < 2)
    return;

  int32_t iEndPos = pCurLine->GetLineEnd();
  std::vector<CFX_Char>& curChars = pCurLine->m_LineChars;
  int32_t iCharPos = GetBreakPos(curChars, bAllChars, false, &iEndPos);
  if (iCharPos < 0)
    iCharPos = 0;

  ++iCharPos;
  if (iCharPos >= pdfium::CollectionSize<int32_t>(pCurLine->m_LineChars)) {
    pNextLine->Clear();
    curChars[iCharPos - 1].m_eLineBreakType = FX_LINEBREAKTYPE::kUNKNOWN;
    return;
  }

  pNextLine->m_LineChars =
      std::vector<CFX_Char>(curChars.begin() + iCharPos, curChars.end());
  curChars.erase(curChars.begin() + iCharPos, curChars.end());
  pNextLine->m_iStart = pCurLine->m_iStart;
  pNextLine->m_iWidth = pCurLine->GetLineEnd() - iEndPos;
  pCurLine->m_iWidth = iEndPos;
  curChars[iCharPos - 1].m_eLineBreakType = FX_LINEBREAKTYPE::kUNKNOWN;

  for (size_t i = 0; i < pNextLine->m_LineChars.size(); ++i) {
    if (pNextLine->m_LineChars[i].GetCharType() >= FX_CHARTYPE::kArabicAlef) {
      pCurLine->m_iArabicChars--;
      pNextLine->m_iArabicChars++;
    }
    pNextLine->m_LineChars[i].m_dwStatus = CFX_BreakType::None;
  }
}

int32_t CFX_RTFBreak::GetDisplayPos(const FX_RTFTEXTOBJ* pText,
                                    FXTEXT_CHARPOS* pCharPos,
                                    bool bCharCode) const {
  if (!pText || pText->iLength < 1)
    return 0;

  ASSERT(pText->pFont);
  ASSERT(pText->pRect);

  RetainPtr<CFGAS_GEFont> pFont = pText->pFont;
  CFX_RectF rtText(*pText->pRect);
  bool bRTLPiece = FX_IsOdd(pText->iBidiLevel);
  float fFontSize = pText->fFontSize;
  int32_t iFontSize = FXSYS_round(fFontSize * 20.0f);
  if (iFontSize == 0)
    return 0;

  int32_t iAscent = pFont->GetAscent();
  int32_t iDescent = pFont->GetDescent();
  int32_t iMaxHeight = iAscent - iDescent;
  float fFontHeight = fFontSize;
  float fAscent = fFontHeight * static_cast<float>(iAscent) /
                  static_cast<float>(iMaxHeight);
  wchar_t wPrev = 0xFEFF;
  wchar_t wNext;
  float fX = rtText.left;
  int32_t iHorScale = pText->iHorizontalScale;
  int32_t iVerScale = pText->iVerticalScale;
  if (bRTLPiece)
    fX = rtText.right();

  float fY = rtText.top + fAscent;
  int32_t iCount = 0;
  for (int32_t i = 0; i < pText->iLength; ++i) {
    wchar_t wch = pText->pStr[i];
    int32_t iWidth = pText->pWidths[i];
    FX_CHARTYPE dwCharType = FX_GetCharType(wch);
    if (iWidth == 0) {
      if (dwCharType == FX_CHARTYPE::kArabicAlef)
        wPrev = 0xFEFF;
      continue;
    }

    uint32_t iCharWidth = abs(iWidth);
    bool bEmptyChar = (dwCharType >= FX_CHARTYPE::kTab &&
                       dwCharType <= FX_CHARTYPE::kControl);
    if (!bEmptyChar)
      ++iCount;

    if (pCharPos) {
      iCharWidth /= iFontSize;
      wchar_t wForm = wch;
      if (dwCharType >= FX_CHARTYPE::kArabicAlef) {
        if (i + 1 < pText->iLength) {
          wNext = pText->pStr[i + 1];
          if (pText->pWidths[i + 1] < 0 && i + 2 < pText->iLength)
            wNext = pText->pStr[i + 2];
        } else {
          wNext = 0xFEFF;
        }
        wForm = pdfium::arabic::GetFormChar(wch, wPrev, wNext);
      } else if (bRTLPiece) {
        wForm = FX_GetMirrorChar(wch);
      }

      if (!bEmptyChar) {
        if (bCharCode) {
          pCharPos->m_GlyphIndex = wch;
        } else {
          pCharPos->m_GlyphIndex = pFont->GetGlyphIndex(wForm);
          if (pCharPos->m_GlyphIndex == 0xFFFF)
            pCharPos->m_GlyphIndex = pFont->GetGlyphIndex(wch);
        }
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
        pCharPos->m_ExtGID = pCharPos->m_GlyphIndex;
#endif
        pCharPos->m_FontCharWidth = iCharWidth;
      }

      float fCharWidth = fFontSize * iCharWidth / 1000.0f;
      if (bRTLPiece && dwCharType != FX_CHARTYPE::kCombination)
        fX -= fCharWidth;

      if (!bEmptyChar)
        pCharPos->m_Origin = CFX_PointF(fX, fY);
      if (!bRTLPiece && dwCharType != FX_CHARTYPE::kCombination)
        fX += fCharWidth;

      if (!bEmptyChar) {
        pCharPos->m_bGlyphAdjust = true;
        pCharPos->m_AdjustMatrix[0] = -1;
        pCharPos->m_AdjustMatrix[1] = 0;
        pCharPos->m_AdjustMatrix[2] = 0;
        pCharPos->m_AdjustMatrix[3] = 1;
        pCharPos->m_Origin.y += fAscent * iVerScale / 100.0f;
        pCharPos->m_Origin.y -= fAscent;

        if (iHorScale != 100 || iVerScale != 100) {
          pCharPos->m_AdjustMatrix[0] =
              pCharPos->m_AdjustMatrix[0] * iHorScale / 100.0f;
          pCharPos->m_AdjustMatrix[1] =
              pCharPos->m_AdjustMatrix[1] * iHorScale / 100.0f;
          pCharPos->m_AdjustMatrix[2] =
              pCharPos->m_AdjustMatrix[2] * iVerScale / 100.0f;
          pCharPos->m_AdjustMatrix[3] =
              pCharPos->m_AdjustMatrix[3] * iVerScale / 100.0f;
        }
        ++pCharPos;
      }
    }
    if (iWidth > 0)
      wPrev = wch;
  }
  return iCount;
}

FX_RTFTEXTOBJ::FX_RTFTEXTOBJ()
    : pFont(nullptr),
      pRect(nullptr),
      wLineBreakChar(L'\n'),
      fFontSize(12.0f),
      iLength(0),
      iBidiLevel(0),
      iHorizontalScale(100),
      iVerticalScale(100) {}

FX_RTFTEXTOBJ::~FX_RTFTEXTOBJ() {}
