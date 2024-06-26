// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/layout/cfx_txtbreak.h"

#include <algorithm>

#include "core/fxge/cfx_renderdevice.h"
#include "third_party/base/stl_util.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fgas/layout/cfx_char.h"
#include "xfa/fgas/layout/fx_arabic.h"
#include "xfa/fgas/layout/fx_linebreak.h"

namespace {

bool IsCtrlCode(wchar_t wch) {
  FX_CHARTYPE dwRet = FX_GetCharType(wch);
  return dwRet == FX_CHARTYPE::kTab || dwRet == FX_CHARTYPE::kControl;
}

}  // namespace

CFX_TxtBreak::CFX_TxtBreak()
    : CFX_Break(FX_LAYOUTSTYLE_None),
      m_iAlignment(CFX_TxtLineAlignment_Left),
      m_iCombWidth(360000) {}

CFX_TxtBreak::~CFX_TxtBreak() {}

void CFX_TxtBreak::SetLineWidth(float fLineWidth) {
  m_iLineWidth = FXSYS_round(fLineWidth * kConversionFactor);
  ASSERT(m_iLineWidth >= 20000);
}

void CFX_TxtBreak::SetAlignment(int32_t iAlignment) {
  ASSERT(iAlignment >= CFX_TxtLineAlignment_Left);
  ASSERT(iAlignment <= CFX_TxtLineAlignment_Justified);
  m_iAlignment = iAlignment;
}

void CFX_TxtBreak::SetCombWidth(float fCombWidth) {
  m_iCombWidth = FXSYS_round(fCombWidth * kConversionFactor);
}

void CFX_TxtBreak::AppendChar_Combination(CFX_Char* pCurChar) {
  wchar_t wch = pCurChar->char_code();
  wchar_t wForm;
  FX_SAFE_INT32 iCharWidth = 0;
  pCurChar->m_iCharWidth = -1;
  if (m_bCombText) {
    iCharWidth = m_iCombWidth;
  } else {
    wForm = wch;
    CFX_Char* pLastChar = GetLastChar(0, false, false);
    if (pLastChar &&
        (pLastChar->m_dwCharStyles & FX_TXTCHARSTYLE_ArabicShadda) == 0) {
      bool bShadda = false;
      if (wch == 0x0651) {
        wchar_t wLast = pLastChar->char_code();
        if (wLast >= 0x064C && wLast <= 0x0650) {
          wForm = FX_GetArabicFromShaddaTable(wLast);
          bShadda = true;
        }
      } else if (wch >= 0x064C && wch <= 0x0650) {
        if (pLastChar->char_code() == 0x0651) {
          wForm = FX_GetArabicFromShaddaTable(wch);
          bShadda = true;
        }
      }
      if (bShadda) {
        pLastChar->m_dwCharStyles |= FX_TXTCHARSTYLE_ArabicShadda;
        pLastChar->m_iCharWidth = 0;
        pCurChar->m_dwCharStyles |= FX_TXTCHARSTYLE_ArabicShadda;
      }
    }
    int32_t iCharWidthOut;
    if (m_pFont->GetCharWidth(wForm, &iCharWidthOut))
      iCharWidth = iCharWidthOut;
    else
      iCharWidth = 0;

    iCharWidth *= m_iFontSize;
    iCharWidth *= m_iHorizontalScale;
    iCharWidth /= 100;
  }

  iCharWidth *= -1;
  pCurChar->m_iCharWidth = iCharWidth.ValueOrDefault(0);
}

void CFX_TxtBreak::AppendChar_Tab(CFX_Char* pCurChar) {
  m_eCharType = FX_CHARTYPE::kTab;
}

CFX_BreakType CFX_TxtBreak::AppendChar_Control(CFX_Char* pCurChar) {
  m_eCharType = FX_CHARTYPE::kControl;
  CFX_BreakType dwRet = CFX_BreakType::None;
  if (!m_bSingleLine) {
    wchar_t wch = pCurChar->char_code();
    switch (wch) {
      case L'\v':
      case 0x2028:
        dwRet = CFX_BreakType::Line;
        break;
      case L'\f':
        dwRet = CFX_BreakType::Page;
        break;
      case 0x2029:
        dwRet = CFX_BreakType::Paragraph;
        break;
      default:
        if (wch == m_wParagraphBreakChar)
          dwRet = CFX_BreakType::Paragraph;
        break;
    }
    if (dwRet != CFX_BreakType::None)
      dwRet = EndBreak(dwRet);
  }
  return dwRet;
}

CFX_BreakType CFX_TxtBreak::AppendChar_Arabic(CFX_Char* pCurChar) {
  FX_CHARTYPE chartype = pCurChar->GetCharType();
  int32_t& iLineWidth = m_pCurLine->m_iWidth;
  wchar_t wForm;
  CFX_Char* pLastChar = nullptr;
  bool bAlef = false;
  if (!m_bCombText && m_eCharType >= FX_CHARTYPE::kArabicAlef &&
      m_eCharType <= FX_CHARTYPE::kArabicDistortion) {
    FX_SAFE_INT32 iCharWidth = 0;
    pLastChar = GetLastChar(1, true, false);
    if (pLastChar) {
      if (pLastChar->m_iCharWidth > 0)
        iLineWidth -= pLastChar->m_iCharWidth;
      iCharWidth = pLastChar->m_iCharWidth;

      CFX_Char* pPrevChar = GetLastChar(2, true, false);
      wForm = pdfium::arabic::GetFormChar(pLastChar, pPrevChar, pCurChar);
      bAlef = (wForm == 0xFEFF &&
               pLastChar->GetCharType() == FX_CHARTYPE::kArabicAlef);
      int32_t iCharWidthOut;
      m_pFont->GetCharWidth(wForm, &iCharWidthOut);
      iCharWidth = iCharWidthOut;

      if (wForm == 0xFEFF)
        iCharWidth = m_iDefChar;

      iCharWidth *= m_iFontSize;
      iCharWidth *= m_iHorizontalScale;
      iCharWidth /= 100;

      int32_t iCharWidthValid = iCharWidth.ValueOrDefault(0);

      pLastChar->m_iCharWidth = iCharWidthValid;
      iLineWidth += iCharWidthValid;
    }
  }

  m_eCharType = chartype;
  wForm = pdfium::arabic::GetFormChar(pCurChar, bAlef ? nullptr : pLastChar,
                                      nullptr);
  FX_SAFE_INT32 iCharWidth;
  if (m_bCombText) {
    iCharWidth = m_iCombWidth;
  } else {
    int32_t iCharWidthOut;
    m_pFont->GetCharWidth(wForm, &iCharWidthOut);
    iCharWidth = iCharWidthOut;

    if (wForm == 0xFEFF)
      iCharWidth = m_iDefChar;

    iCharWidth *= m_iFontSize;
    iCharWidth *= m_iHorizontalScale;
    iCharWidth /= 100;
  }

  int32_t iCharWidthValid = iCharWidth.ValueOrDefault(0);
  pCurChar->m_iCharWidth = iCharWidthValid;
  iLineWidth += iCharWidthValid;
  m_pCurLine->m_iArabicChars++;
  if (!m_bSingleLine && iLineWidth > m_iLineWidth + m_iTolerance)
    return EndBreak(CFX_BreakType::Line);
  return CFX_BreakType::None;
}

CFX_BreakType CFX_TxtBreak::AppendChar_Others(CFX_Char* pCurChar) {
  FX_CHARTYPE chartype = pCurChar->GetCharType();
  int32_t& iLineWidth = m_pCurLine->m_iWidth;
  FX_SAFE_INT32 iCharWidth = 0;
  m_eCharType = chartype;
  wchar_t wch = pCurChar->char_code();
  wchar_t wForm = wch;

  if (m_bCombText) {
    iCharWidth = m_iCombWidth;
  } else {
    int32_t iCharWidthOut;
    if (m_pFont->GetCharWidth(wForm, &iCharWidthOut))
      iCharWidth = iCharWidthOut;
    else
      iCharWidth = m_iDefChar;

    iCharWidth *= m_iFontSize;
    iCharWidth *= m_iHorizontalScale;
    iCharWidth /= 100;
  }

  iCharWidth += m_iCharSpace;

  int32_t iCharWidthValid = iCharWidth.ValueOrDefault(0);
  pCurChar->m_iCharWidth = iCharWidthValid;
  iLineWidth += iCharWidthValid;
  if (!m_bSingleLine && chartype != FX_CHARTYPE::kSpace &&
      iLineWidth > m_iLineWidth + m_iTolerance) {
    return EndBreak(CFX_BreakType::Line);
  }

  return CFX_BreakType::None;
}

CFX_BreakType CFX_TxtBreak::AppendChar(wchar_t wch) {
  FX_CHARTYPE chartype = FX_GetCharType(wch);
  m_pCurLine->m_LineChars.emplace_back(wch, m_iHorizontalScale,
                                       m_iVerticalScale);
  CFX_Char* pCurChar = &m_pCurLine->m_LineChars.back();
  pCurChar->m_dwCharStyles = m_iAlignment | (1 << 8);

  CFX_BreakType dwRet1 = CFX_BreakType::None;
  if (chartype != FX_CHARTYPE::kCombination &&
      GetUnifiedCharType(m_eCharType) != GetUnifiedCharType(chartype) &&
      m_eCharType != FX_CHARTYPE::kUnknown &&
      m_pCurLine->m_iWidth > m_iLineWidth + m_iTolerance && !m_bSingleLine &&
      (m_eCharType != FX_CHARTYPE::kSpace ||
       chartype != FX_CHARTYPE::kControl)) {
    dwRet1 = EndBreak(CFX_BreakType::Line);
    if (!m_pCurLine->m_LineChars.empty())
      pCurChar = &m_pCurLine->m_LineChars.back();
  }

  CFX_BreakType dwRet2 = CFX_BreakType::None;
  if (wch == m_wParagraphBreakChar) {
    // This is handled in AppendChar_Control, but it seems like \n and \r
    // don't get matched as control characters so we go into AppendChar_other
    // and never detect the new paragraph ...
    dwRet2 = CFX_BreakType::Paragraph;
    EndBreak(dwRet2);
  } else {
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
  }
  return std::max(dwRet1, dwRet2);
}

bool CFX_TxtBreak::EndBreak_SplitLine(CFX_BreakLine* pNextLine,
                                      bool bAllChars) {
  bool bDone = false;
  CFX_Char* pTC;
  if (!m_bSingleLine && m_pCurLine->m_iWidth > m_iLineWidth + m_iTolerance) {
    pTC = m_pCurLine->GetChar(m_pCurLine->m_LineChars.size() - 1);
    switch (pTC->GetCharType()) {
      case FX_CHARTYPE::kTab:
      case FX_CHARTYPE::kControl:
      case FX_CHARTYPE::kSpace:
        break;
      default:
        SplitTextLine(m_pCurLine.Get(), pNextLine, bAllChars);
        bDone = true;
        break;
    }
  }

  CFX_BreakPiece tp;
  if (bAllChars && !bDone) {
    int32_t iEndPos = m_pCurLine->m_iWidth;
    GetBreakPos(&m_pCurLine->m_LineChars, bAllChars, true, &iEndPos);
  }
  return false;
}

void CFX_TxtBreak::EndBreak_BidiLine(std::deque<FX_TPO>* tpos,
                                     CFX_BreakType dwStatus) {
  CFX_BreakPiece tp;
  FX_TPO tpo;
  CFX_Char* pTC;
  std::vector<CFX_Char>& chars = m_pCurLine->m_LineChars;
  bool bDone = m_pCurLine->m_iArabicChars > 0;
  if (bDone) {
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
  }

  if (bDone) {
    tp.m_dwStatus = CFX_BreakType::Piece;
    tp.m_iStartPos = m_pCurLine->m_iStart;
    tp.m_pChars = &m_pCurLine->m_LineChars;
    int32_t iBidiLevel = -1;
    int32_t iCharWidth;
    int32_t i = 0;
    int32_t j = -1;
    int32_t iCount = pdfium::CollectionSize<int32_t>(m_pCurLine->m_LineChars);
    while (i < iCount) {
      pTC = &chars[i];
      if (iBidiLevel < 0) {
        iBidiLevel = pTC->m_iBidiLevel;
        tp.m_iWidth = 0;
        tp.m_iBidiLevel = iBidiLevel;
        tp.m_iBidiPos = pTC->m_iBidiOrder;
        tp.m_dwCharStyles = pTC->m_dwCharStyles;
        tp.m_iHorizontalScale = pTC->horizonal_scale();
        tp.m_iVerticalScale = pTC->vertical_scale();
        tp.m_dwStatus = CFX_BreakType::Piece;
      }
      if (iBidiLevel != pTC->m_iBidiLevel ||
          pTC->m_dwStatus != CFX_BreakType::None) {
        if (iBidiLevel == pTC->m_iBidiLevel) {
          tp.m_dwStatus = pTC->m_dwStatus;
          iCharWidth = pTC->m_iCharWidth;
          if (iCharWidth > 0)
            tp.m_iWidth += iCharWidth;

          i++;
        }
        tp.m_iChars = i - tp.m_iStartChar;
        m_pCurLine->m_LinePieces.push_back(tp);
        tp.m_iStartPos += tp.m_iWidth;
        tp.m_iStartChar = i;
        tpo.index = ++j;
        tpo.pos = tp.m_iBidiPos;
        tpos->push_back(tpo);
        iBidiLevel = -1;
      } else {
        iCharWidth = pTC->m_iCharWidth;
        if (iCharWidth > 0)
          tp.m_iWidth += iCharWidth;

        i++;
      }
    }
    if (i > tp.m_iStartChar) {
      tp.m_dwStatus = dwStatus;
      tp.m_iChars = i - tp.m_iStartChar;
      m_pCurLine->m_LinePieces.push_back(tp);
      tpo.index = ++j;
      tpo.pos = tp.m_iBidiPos;
      tpos->push_back(tpo);
    }
    if (j > -1) {
      if (j > 0) {
        std::sort(tpos->begin(), tpos->end());
        int32_t iStartPos = 0;
        for (i = 0; i <= j; i++) {
          tpo = (*tpos)[i];
          CFX_BreakPiece& ttp = m_pCurLine->m_LinePieces[tpo.index];
          ttp.m_iStartPos = iStartPos;
          iStartPos += ttp.m_iWidth;
        }
      }
      m_pCurLine->m_LinePieces[j].m_dwStatus = dwStatus;
    }
  } else {
    tp.m_dwStatus = dwStatus;
    tp.m_iStartPos = m_pCurLine->m_iStart;
    tp.m_iWidth = m_pCurLine->m_iWidth;
    tp.m_iStartChar = 0;
    tp.m_iChars = m_pCurLine->m_LineChars.size();
    tp.m_pChars = &m_pCurLine->m_LineChars;
    pTC = &chars[0];
    tp.m_dwCharStyles = pTC->m_dwCharStyles;
    tp.m_iHorizontalScale = pTC->horizonal_scale();
    tp.m_iVerticalScale = pTC->vertical_scale();
    m_pCurLine->m_LinePieces.push_back(tp);
    tpos->push_back({0, 0});
  }
}

void CFX_TxtBreak::EndBreak_Alignment(const std::deque<FX_TPO>& tpos,
                                      bool bAllChars,
                                      CFX_BreakType dwStatus) {
  int32_t iNetWidth = m_pCurLine->m_iWidth;
  int32_t iGapChars = 0;
  bool bFind = false;
  for (auto it = tpos.rbegin(); it != tpos.rend(); ++it) {
    CFX_BreakPiece& ttp = m_pCurLine->m_LinePieces[it->index];
    if (!bFind)
      iNetWidth = ttp.GetEndPos();

    bool bArabic = FX_IsOdd(ttp.m_iBidiLevel);
    int32_t j = bArabic ? 0 : ttp.m_iChars - 1;
    while (j > -1 && j < ttp.m_iChars) {
      const CFX_Char* pTC = ttp.GetChar(j);
      if (pTC->m_eLineBreakType == FX_LINEBREAKTYPE::kDIRECT_BRK)
        iGapChars++;
      if (!bFind || !bAllChars) {
        FX_CHARTYPE chartype = pTC->GetCharType();
        if (chartype == FX_CHARTYPE::kSpace ||
            chartype == FX_CHARTYPE::kControl) {
          if (!bFind && bAllChars && pTC->m_iCharWidth > 0)
            iNetWidth -= pTC->m_iCharWidth;
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
  if (iGapChars > 0 && m_iAlignment & CFX_TxtLineAlignment_Justified &&
      dwStatus != CFX_BreakType::Paragraph) {
    int32_t iStart = -1;
    for (auto& tpo : tpos) {
      CFX_BreakPiece& ttp = m_pCurLine->m_LinePieces[tpo.index];
      if (iStart < -1)
        iStart = ttp.m_iStartPos;
      else
        ttp.m_iStartPos = iStart;

      for (int32_t j = 0; j < ttp.m_iChars && iGapChars > 0; j++, iGapChars--) {
        CFX_Char* pTC = ttp.GetChar(j);
        if (pTC->m_eLineBreakType != FX_LINEBREAKTYPE::kDIRECT_BRK ||
            pTC->m_iCharWidth < 0) {
          continue;
        }
        int32_t k = iOffset / iGapChars;
        pTC->m_iCharWidth += k;
        ttp.m_iWidth += k;
        iOffset -= k;
      }
      iStart += ttp.m_iWidth;
    }
  } else if (m_iAlignment & CFX_TxtLineAlignment_Center ||
             m_iAlignment & CFX_TxtLineAlignment_Right) {
    if (m_iAlignment & CFX_TxtLineAlignment_Center &&
        !(m_iAlignment & CFX_TxtLineAlignment_Right)) {
      iOffset /= 2;
    }
    if (iOffset > 0) {
      for (auto& ttp : m_pCurLine->m_LinePieces)
        ttp.m_iStartPos += iOffset;
    }
  }
}

CFX_BreakType CFX_TxtBreak::EndBreak(CFX_BreakType dwStatus) {
  ASSERT(dwStatus != CFX_BreakType::None);

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

  m_pCurLine->m_LineChars.back().m_dwStatus = dwStatus;
  if (dwStatus == CFX_BreakType::Piece)
    return dwStatus;

  m_iReadyLineIndex = m_pCurLine == &m_Line[0] ? 0 : 1;
  CFX_BreakLine* pNextLine = &m_Line[1 - m_iReadyLineIndex];
  bool bAllChars = m_iAlignment > CFX_TxtLineAlignment_Right;
  if (!EndBreak_SplitLine(pNextLine, bAllChars)) {
    std::deque<FX_TPO> tpos;
    EndBreak_BidiLine(&tpos, dwStatus);
    if (m_iAlignment > CFX_TxtLineAlignment_Left)
      EndBreak_Alignment(tpos, bAllChars, dwStatus);
  }

  m_pCurLine = pNextLine;
  CFX_Char* pTC = GetLastChar(0, false, false);
  m_eCharType = pTC ? pTC->GetCharType() : FX_CHARTYPE::kUnknown;

  return dwStatus;
}

int32_t CFX_TxtBreak::GetBreakPos(std::vector<CFX_Char>* pChars,
                                  bool bAllChars,
                                  bool bOnlyBrk,
                                  int32_t* pEndPos) {
  std::vector<CFX_Char>& chars = *pChars;
  int32_t iLength = pdfium::CollectionSize<int32_t>(chars) - 1;
  if (iLength < 1)
    return iLength;

  int32_t iBreak = -1;
  int32_t iBreakPos = -1;
  int32_t iIndirect = -1;
  int32_t iIndirectPos = -1;
  int32_t iLast = -1;
  int32_t iLastPos = -1;
  if (m_bSingleLine || *pEndPos <= m_iLineWidth) {
    if (!bAllChars)
      return iLength;

    iBreak = iLength;
    iBreakPos = *pEndPos;
  }

  FX_LINEBREAKTYPE eType;
  FX_BREAKPROPERTY nCur;
  FX_BREAKPROPERTY nNext;
  CFX_Char* pCur = &chars[iLength--];
  if (bAllChars)
    pCur->m_eLineBreakType = FX_LINEBREAKTYPE::kUNKNOWN;

  nNext = FX_GetBreakProperty(pCur->char_code());
  int32_t iCharWidth = pCur->m_iCharWidth;
  if (iCharWidth > 0)
    *pEndPos -= iCharWidth;

  while (iLength >= 0) {
    pCur = &chars[iLength];
    nCur = FX_GetBreakProperty(pCur->char_code());
    if (nNext == FX_BREAKPROPERTY::kSP)
      eType = FX_LINEBREAKTYPE::kPROHIBITED_BRK;
    else
      eType = GetLineBreakTypeFromPair(nCur, nNext);
    if (bAllChars)
      pCur->m_eLineBreakType = eType;
    if (!bOnlyBrk) {
      if (m_bSingleLine || *pEndPos <= m_iLineWidth ||
          nCur == FX_BREAKPROPERTY::kSP) {
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
      iCharWidth = pCur->m_iCharWidth;
      if (iCharWidth > 0)
        *pEndPos -= iCharWidth;
    }
    nNext = nCur;
    iLength--;
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

void CFX_TxtBreak::SplitTextLine(CFX_BreakLine* pCurLine,
                                 CFX_BreakLine* pNextLine,
                                 bool bAllChars) {
  ASSERT(pCurLine);
  ASSERT(pNextLine);

  if (pCurLine->m_LineChars.size() < 2)
    return;

  int32_t iEndPos = pCurLine->m_iWidth;
  std::vector<CFX_Char>& curChars = pCurLine->m_LineChars;
  int32_t iCharPos = GetBreakPos(&curChars, bAllChars, false, &iEndPos);
  if (iCharPos < 0)
    iCharPos = 0;

  iCharPos++;
  if (iCharPos >= pdfium::CollectionSize<int32_t>(pCurLine->m_LineChars)) {
    pNextLine->Clear();
    CFX_Char* pTC = &curChars[iCharPos - 1];
    pTC->m_eLineBreakType = FX_LINEBREAKTYPE::kUNKNOWN;
    return;
  }

  pNextLine->m_LineChars =
      std::vector<CFX_Char>(curChars.begin() + iCharPos, curChars.end());
  curChars.erase(curChars.begin() + iCharPos, curChars.end());
  pCurLine->m_iWidth = iEndPos;
  CFX_Char* pTC = &curChars[iCharPos - 1];
  pTC->m_eLineBreakType = FX_LINEBREAKTYPE::kUNKNOWN;
  int32_t iWidth = 0;
  for (size_t i = 0; i < pNextLine->m_LineChars.size(); ++i) {
    if (pNextLine->m_LineChars[i].GetCharType() >= FX_CHARTYPE::kArabicAlef) {
      pCurLine->m_iArabicChars--;
      pNextLine->m_iArabicChars++;
    }
    iWidth += std::max(0, pNextLine->m_LineChars[i].m_iCharWidth);
    pNextLine->m_LineChars[i].m_dwStatus = CFX_BreakType::None;
  }
  pNextLine->m_iWidth = iWidth;
}

struct FX_FORMCHAR {
  uint16_t wch;
  uint16_t wForm;
  int32_t iWidth;
};

size_t CFX_TxtBreak::GetDisplayPos(const Run* pTxtRun,
                                   FXTEXT_CHARPOS* pCharPos) const {
  if (!pTxtRun || pTxtRun->iLength < 1)
    return 0;

  Engine* pEngine = pTxtRun->pEdtEngine;
  const wchar_t* pStr = pTxtRun->wsStr.c_str();
  int32_t* pWidths = pTxtRun->pWidths;
  int32_t iLength = pTxtRun->iLength - 1;
  RetainPtr<CFGAS_GEFont> pFont = pTxtRun->pFont;
  uint32_t dwStyles = pTxtRun->dwStyles;
  CFX_RectF rtText(*pTxtRun->pRect);
  bool bRTLPiece = (pTxtRun->dwCharStyles & FX_TXTCHARSTYLE_OddBidiLevel) != 0;
  float fFontSize = pTxtRun->fFontSize;
  int32_t iFontSize = FXSYS_round(fFontSize * 20.0f);
  int32_t iAscent = pFont->GetAscent();
  int32_t iDescent = pFont->GetDescent();
  int32_t iMaxHeight = iAscent - iDescent;
  float fFontHeight = fFontSize;
  float fAscent = fFontHeight * iAscent / iMaxHeight;
  float fX = rtText.left;
  float fY;
  float fCharWidth;
  int32_t iHorScale = pTxtRun->iHorizontalScale;
  int32_t iVerScale = pTxtRun->iVerticalScale;
  bool bSkipSpace = pTxtRun->bSkipSpace;
  FX_FORMCHAR formChars[3];
  float fYBase;

  if (bRTLPiece)
    fX = rtText.right();

  fYBase = rtText.top + (rtText.height - fFontSize) / 2.0f;
  fY = fYBase + fAscent;

  size_t szCount = 0;
  int32_t iNext = 0;
  wchar_t wPrev = 0xFEFF;
  wchar_t wNext = 0xFEFF;
  wchar_t wForm = 0xFEFF;
  wchar_t wLast = 0xFEFF;
  bool bShadda = false;
  bool bLam = false;
  for (int32_t i = 0; i <= iLength; i++) {
    int32_t iAbsolute = i + pTxtRun->iStart;
    int32_t iWidth;
    wchar_t wch;
    if (pEngine) {
      wch = pEngine->GetChar(iAbsolute);
      iWidth = pEngine->GetWidthOfChar(iAbsolute);
    } else {
      wch = *pStr++;
      iWidth = *pWidths++;
    }

    FX_CHARTYPE chartype = FX_GetCharType(wch);
    if (chartype == FX_CHARTYPE::kArabicAlef && iWidth == 0) {
      wPrev = 0xFEFF;
      wLast = wch;
      continue;
    }

    if (chartype >= FX_CHARTYPE::kArabicAlef) {
      if (i < iLength) {
        if (pEngine) {
          iNext = i + 1;
          while (iNext <= iLength) {
            int32_t iNextAbsolute = iNext + pTxtRun->iStart;
            wNext = pEngine->GetChar(iNextAbsolute);
            if (FX_GetCharType(wNext) != FX_CHARTYPE::kCombination)
              break;

            iNext++;
          }
          if (iNext > iLength)
            wNext = 0xFEFF;
        } else {
          int32_t j = -1;
          do {
            j++;
            if (i + j >= iLength)
              break;

            wNext = pStr[j];
          } while (FX_GetCharType(wNext) == FX_CHARTYPE::kCombination);
          if (i + j >= iLength)
            wNext = 0xFEFF;
        }
      } else {
        wNext = 0xFEFF;
      }

      wForm = pdfium::arabic::GetFormChar(wch, wPrev, wNext);
      bLam = (wPrev == 0x0644 && wch == 0x0644 && wNext == 0x0647);
    } else if (chartype == FX_CHARTYPE::kCombination) {
      wForm = wch;
      if (wch >= 0x064C && wch <= 0x0651) {
        if (bShadda) {
          wForm = 0xFEFF;
          bShadda = false;
        } else {
          wNext = 0xFEFF;
          if (pEngine) {
            iNext = i + 1;
            if (iNext <= iLength) {
              int32_t iNextAbsolute = iNext + pTxtRun->iStart;
              wNext = pEngine->GetChar(iNextAbsolute);
            }
          } else {
            if (i < iLength)
              wNext = *pStr;
          }
          if (wch == 0x0651) {
            if (wNext >= 0x064C && wNext <= 0x0650) {
              wForm = FX_GetArabicFromShaddaTable(wNext);
              bShadda = true;
            }
          } else {
            if (wNext == 0x0651) {
              wForm = FX_GetArabicFromShaddaTable(wch);
              bShadda = true;
            }
          }
        }
      } else {
        bShadda = false;
      }
    } else if (chartype == FX_CHARTYPE::kNumeric) {
      wForm = wch;
    } else if (wch == L'.') {
      wForm = wch;
    } else if (wch == L',') {
      wForm = wch;
    } else if (bRTLPiece) {
      wForm = FX_GetMirrorChar(wch);
    } else {
      wForm = wch;
    }
    if (chartype != FX_CHARTYPE::kCombination)
      bShadda = false;
    if (chartype < FX_CHARTYPE::kArabicAlef)
      bLam = false;

    bool bEmptyChar =
        (chartype >= FX_CHARTYPE::kTab && chartype <= FX_CHARTYPE::kControl);
    if (wForm == 0xFEFF)
      bEmptyChar = true;

    int32_t iForms = bLam ? 3 : 1;
    szCount += (bEmptyChar && bSkipSpace) ? 0 : iForms;
    if (!pCharPos) {
      if (iWidth > 0)
        wPrev = wch;
      wLast = wch;
      continue;
    }

    int32_t iCharWidth = iWidth;
    if (iCharWidth < 0)
      iCharWidth = -iCharWidth;

    iCharWidth /= iFontSize;
    formChars[0].wch = wch;
    formChars[0].wForm = wForm;
    formChars[0].iWidth = iCharWidth;
    if (bLam) {
      formChars[1].wForm = 0x0651;
      iCharWidth = 0;
      pFont->GetCharWidth(0x0651, &iCharWidth);
      formChars[1].iWidth = iCharWidth;
      formChars[2].wForm = 0x0670;
      iCharWidth = 0;
      pFont->GetCharWidth(0x0670, &iCharWidth);
      formChars[2].iWidth = iCharWidth;
    }

    for (int32_t j = 0; j < iForms; j++) {
      wForm = (wchar_t)formChars[j].wForm;
      iCharWidth = formChars[j].iWidth;
      if (j > 0) {
        chartype = FX_CHARTYPE::kCombination;
        wch = wForm;
        wLast = (wchar_t)formChars[j - 1].wForm;
      }
      if (!bEmptyChar || (bEmptyChar && !bSkipSpace)) {
        pCharPos->m_GlyphIndex = pFont->GetGlyphIndex(wForm);
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
        pCharPos->m_ExtGID = pCharPos->m_GlyphIndex;
#endif
        // TODO(npm): change widths in this method to unsigned to avoid implicit
        // cast in the following line.
        pCharPos->m_FontCharWidth = iCharWidth;
      }

      fCharWidth = fFontSize * iCharWidth / 1000.0f;
      if (bRTLPiece && chartype != FX_CHARTYPE::kCombination)
        fX -= fCharWidth;

      if (!bEmptyChar || (bEmptyChar && !bSkipSpace)) {
        pCharPos->m_Origin = CFX_PointF(fX, fY);

        if ((dwStyles & FX_LAYOUTSTYLE_CombText) != 0) {
          int32_t iFormWidth = iCharWidth;
          pFont->GetCharWidth(wForm, &iFormWidth);
          float fOffset = fFontSize * (iCharWidth - iFormWidth) / 2000.0f;
          pCharPos->m_Origin.x += fOffset;
        }

        if (chartype == FX_CHARTYPE::kCombination) {
          FX_RECT rtBBox;
          if (pFont->GetCharBBox(wForm, &rtBBox)) {
            pCharPos->m_Origin.y =
                fYBase + fFontSize - fFontSize * rtBBox.Height() / iMaxHeight;
          }
          if (wForm == wch && wLast != 0xFEFF) {
            if (FX_GetCharType(wLast) == FX_CHARTYPE::kCombination) {
              FX_RECT rtBox;
              if (pFont->GetCharBBox(wLast, &rtBox))
                pCharPos->m_Origin.y -= fFontSize * rtBox.Height() / iMaxHeight;
            }
          }
        }
      }
      if (!bRTLPiece && chartype != FX_CHARTYPE::kCombination)
        fX += fCharWidth;

      if (!bEmptyChar || (bEmptyChar && !bSkipSpace)) {
        pCharPos->m_bGlyphAdjust = true;
        pCharPos->m_AdjustMatrix[0] = -1;
        pCharPos->m_AdjustMatrix[1] = 0;
        pCharPos->m_AdjustMatrix[2] = 0;
        pCharPos->m_AdjustMatrix[3] = 1;

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
        pCharPos++;
      }
    }
    if (iWidth > 0)
      wPrev = static_cast<wchar_t>(formChars[0].wch);
    wLast = wch;
  }
  return szCount;
}

std::vector<CFX_RectF> CFX_TxtBreak::GetCharRects(const Run* pTxtRun,
                                                  bool bCharBBox) const {
  if (!pTxtRun || pTxtRun->iLength < 1)
    return std::vector<CFX_RectF>();

  Engine* pEngine = pTxtRun->pEdtEngine;
  const wchar_t* pStr = pTxtRun->wsStr.c_str();
  int32_t* pWidths = pTxtRun->pWidths;
  int32_t iLength = pTxtRun->iLength;
  CFX_RectF rect(*pTxtRun->pRect);
  float fFontSize = pTxtRun->fFontSize;
  float fScale = fFontSize / 1000.0f;
  RetainPtr<CFGAS_GEFont> pFont = pTxtRun->pFont;
  if (!pFont)
    bCharBBox = false;

  FX_RECT bbox;
  if (bCharBBox)
    bCharBBox = pFont->GetBBox(&bbox);

  float fLeft = std::max(0.0f, bbox.left * fScale);
  float fHeight = fabs(bbox.Height() * fScale);
  bool bRTLPiece = !!(pTxtRun->dwCharStyles & FX_TXTCHARSTYLE_OddBidiLevel);
  bool bSingleLine = !!(pTxtRun->dwStyles & FX_LAYOUTSTYLE_SingleLine);
  bool bCombText = !!(pTxtRun->dwStyles & FX_LAYOUTSTYLE_CombText);
  wchar_t wch;
  int32_t iCharSize;
  float fCharSize;
  float fStart = bRTLPiece ? rect.right() : rect.left;

  std::vector<CFX_RectF> rtArray(iLength);
  for (int32_t i = 0; i < iLength; i++) {
    int32_t iAbsolute = i + pTxtRun->iStart;
    if (pEngine) {
      wch = pEngine->GetChar(iAbsolute);
      iCharSize = pEngine->GetWidthOfChar(iAbsolute);
    } else {
      wch = *pStr++;
      iCharSize = *pWidths++;
    }
    fCharSize = static_cast<float>(iCharSize) / kConversionFactor;
    bool bRet = (!bSingleLine && IsCtrlCode(wch));
    if (!(wch == L'\v' || wch == L'\f' || wch == 0x2028 || wch == 0x2029 ||
          wch == L'\n')) {
      bRet = false;
    }
    if (bRet)
      fCharSize = fFontSize / 2.0f;
    rect.left = fStart;
    if (bRTLPiece) {
      rect.left -= fCharSize;
      fStart -= fCharSize;
    } else {
      fStart += fCharSize;
    }
    rect.width = fCharSize;

    if (bCharBBox && !bRet) {
      int32_t iCharWidth = 1000;
      pFont->GetCharWidth(wch, &iCharWidth);
      float fRTLeft = 0, fCharWidth = 0;
      if (iCharWidth > 0) {
        fCharWidth = iCharWidth * fScale;
        fRTLeft = fLeft;
        if (bCombText)
          fRTLeft = (rect.width - fCharWidth) / 2.0f;
      }
      CFX_RectF rtBBoxF;
      rtBBoxF.left = rect.left + fRTLeft;
      rtBBoxF.top = rect.top + (rect.height - fHeight) / 2.0f;
      rtBBoxF.width = fCharWidth;
      rtBBoxF.height = fHeight;
      rtBBoxF.top = std::max(rtBBoxF.top, 0.0f);
      rtArray[i] = rtBBoxF;
      continue;
    }
    rtArray[i] = rect;
  }
  return rtArray;
}

CFX_TxtBreak::Engine::~Engine() = default;

CFX_TxtBreak::Run::Run() = default;

CFX_TxtBreak::Run::~Run() = default;

CFX_TxtBreak::Run::Run(const CFX_TxtBreak::Run& other) = default;
