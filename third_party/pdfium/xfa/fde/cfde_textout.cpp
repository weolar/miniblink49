// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fde/cfde_textout.h"

#include <algorithm>
#include <utility>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxge/cfx_font.h"
#include "core/fxge/cfx_pathdata.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fgas/layout/cfx_txtbreak.h"

namespace {

bool TextAlignmentVerticallyCentered(const FDE_TextAlignment align) {
  return align == FDE_TextAlignment::kCenterLeft ||
         align == FDE_TextAlignment::kCenter ||
         align == FDE_TextAlignment::kCenterRight;
}

bool IsTextAlignmentTop(const FDE_TextAlignment align) {
  return align == FDE_TextAlignment::kTopLeft;
}

}  // namespace

// static
bool CFDE_TextOut::DrawString(CFX_RenderDevice* device,
                              FX_ARGB color,
                              const RetainPtr<CFGAS_GEFont>& pFont,
                              pdfium::span<FXTEXT_CHARPOS> pCharPos,
                              float fFontSize,
                              const CFX_Matrix* pMatrix) {
  ASSERT(pFont);
  ASSERT(!pCharPos.empty());

  CFX_Font* pFxFont = pFont->GetDevFont();
  if (FontStyleIsItalic(pFont->GetFontStyles()) && !pFxFont->IsItalic()) {
    for (auto& pos : pCharPos) {
      static constexpr float mc = 0.267949f;
      pos.m_AdjustMatrix[2] += mc * pos.m_AdjustMatrix[0];
      pos.m_AdjustMatrix[3] += mc * pos.m_AdjustMatrix[1];
    }
  }

#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
  uint32_t dwFontStyle = pFont->GetFontStyles();
  CFX_Font FxFont;
  auto SubstFxFont = pdfium::MakeUnique<CFX_SubstFont>();
  SubstFxFont->m_Weight = FontStyleIsBold(dwFontStyle) ? 700 : 400;
  SubstFxFont->m_ItalicAngle = FontStyleIsItalic(dwFontStyle) ? -12 : 0;
  SubstFxFont->m_WeightCJK = SubstFxFont->m_Weight;
  SubstFxFont->m_bItalicCJK = FontStyleIsItalic(dwFontStyle);
  FxFont.SetSubstFont(std::move(SubstFxFont));
#endif  // _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_

  RetainPtr<CFGAS_GEFont> pCurFont;
  FXTEXT_CHARPOS* pCurCP = nullptr;
  int32_t iCurCount = 0;
  for (auto& pos : pCharPos) {
    RetainPtr<CFGAS_GEFont> pSTFont =
        pFont->GetSubstFont(static_cast<int32_t>(pos.m_GlyphIndex));
    pos.m_GlyphIndex &= 0x00FFFFFF;
    pos.m_bFontStyle = false;
    if (pCurFont != pSTFont) {
      if (pCurFont) {
        pFxFont = pCurFont->GetDevFont();

        CFX_Font* font;
#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
        FxFont.SetFace(pFxFont->GetFace());
        font = &FxFont;
#else
        font = pFxFont;
#endif  // _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_

        device->DrawNormalText(iCurCount, pCurCP, font, -fFontSize, pMatrix,
                               color, FXTEXT_CLEARTYPE);
      }
      pCurFont = pSTFont;
      pCurCP = &pos;
      iCurCount = 1;
    } else {
      ++iCurCount;
    }
  }

  bool bRet = true;
  if (pCurFont && iCurCount) {
    pFxFont = pCurFont->GetDevFont();
    CFX_Font* font;
#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
    FxFont.SetFace(pFxFont->GetFace());
    font = &FxFont;
#else
    font = pFxFont;
#endif  // _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_

    bRet = device->DrawNormalText(iCurCount, pCurCP, font, -fFontSize, pMatrix,
                                  color, FXTEXT_CLEARTYPE);
  }

#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
  FxFont.SetFace(nullptr);
#endif  // _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_

  return bRet;
}

FDE_TTOPIECE::FDE_TTOPIECE() = default;

FDE_TTOPIECE::FDE_TTOPIECE(const FDE_TTOPIECE& that) = default;

FDE_TTOPIECE::~FDE_TTOPIECE() = default;

CFDE_TextOut::CFDE_TextOut()
    : m_pTxtBreak(pdfium::MakeUnique<CFX_TxtBreak>()), m_ttoLines(5) {}

CFDE_TextOut::~CFDE_TextOut() = default;

void CFDE_TextOut::SetFont(const RetainPtr<CFGAS_GEFont>& pFont) {
  ASSERT(pFont);
  m_pFont = pFont;
  m_pTxtBreak->SetFont(pFont);
}

void CFDE_TextOut::SetFontSize(float fFontSize) {
  ASSERT(fFontSize > 0);
  m_fFontSize = fFontSize;
  m_pTxtBreak->SetFontSize(fFontSize);
}

void CFDE_TextOut::SetStyles(const FDE_TextStyle& dwStyles) {
  m_Styles = dwStyles;

  m_dwTxtBkStyles = 0;
  if (m_Styles.single_line_)
    m_dwTxtBkStyles |= FX_LAYOUTSTYLE_SingleLine;

  m_pTxtBreak->SetLayoutStyles(m_dwTxtBkStyles);
}

void CFDE_TextOut::SetAlignment(FDE_TextAlignment iAlignment) {
  m_iAlignment = iAlignment;

  int32_t txtBreakAlignment = 0;
  switch (m_iAlignment) {
    case FDE_TextAlignment::kCenter:
      txtBreakAlignment = CFX_TxtLineAlignment_Center;
      break;
    case FDE_TextAlignment::kCenterRight:
      txtBreakAlignment = CFX_TxtLineAlignment_Right;
      break;
    case FDE_TextAlignment::kCenterLeft:
    case FDE_TextAlignment::kTopLeft:
      txtBreakAlignment = CFX_TxtLineAlignment_Left;
      break;
  }
  m_pTxtBreak->SetAlignment(txtBreakAlignment);
}

void CFDE_TextOut::SetLineSpace(float fLineSpace) {
  ASSERT(fLineSpace > 1.0f);
  m_fLineSpace = fLineSpace;
}

void CFDE_TextOut::SetLineBreakTolerance(float fTolerance) {
  m_fTolerance = fTolerance;
  m_pTxtBreak->SetLineBreakTolerance(m_fTolerance);
}

void CFDE_TextOut::CalcLogicSize(const WideString& str, CFX_SizeF* pSize) {
  CFX_RectF rtText(0.0f, 0.0f, pSize->width, pSize->height);
  CalcLogicSize(str, &rtText);
  *pSize = rtText.Size();
}

void CFDE_TextOut::CalcLogicSize(const WideString& str, CFX_RectF* pRect) {
  if (str.IsEmpty()) {
    pRect->width = 0.0f;
    pRect->height = 0.0f;
    return;
  }

  ASSERT(m_pFont);
  ASSERT(m_fFontSize >= 1.0f);

  if (!m_Styles.single_line_) {
    if (pRect->Width() < 1.0f)
      pRect->width = m_fFontSize * 1000.0f;

    m_pTxtBreak->SetLineWidth(pRect->Width());
  }

  m_iTotalLines = 0;
  float fWidth = 0.0f;
  float fHeight = 0.0f;
  float fStartPos = pRect->right();
  CFX_BreakType dwBreakStatus = CFX_BreakType::None;
  bool break_char_is_set = false;
  for (const wchar_t& wch : str) {
    if (!break_char_is_set && (wch == L'\n' || wch == L'\r')) {
      break_char_is_set = true;
      m_pTxtBreak->SetParagraphBreakChar(wch);
    }
    dwBreakStatus = m_pTxtBreak->AppendChar(wch);
    if (!CFX_BreakTypeNoneOrPiece(dwBreakStatus))
      RetrieveLineWidth(dwBreakStatus, &fStartPos, &fWidth, &fHeight);
  }

  dwBreakStatus = m_pTxtBreak->EndBreak(CFX_BreakType::Paragraph);
  if (!CFX_BreakTypeNoneOrPiece(dwBreakStatus))
    RetrieveLineWidth(dwBreakStatus, &fStartPos, &fWidth, &fHeight);

  m_pTxtBreak->Reset();
  float fInc = pRect->Height() - fHeight;
  if (TextAlignmentVerticallyCentered(m_iAlignment))
    fInc /= 2.0f;
  else if (IsTextAlignmentTop(m_iAlignment))
    fInc = 0.0f;

  pRect->left += fStartPos;
  pRect->top += fInc;
  pRect->width = std::min(fWidth, pRect->Width());
  pRect->height = fHeight;
  if (m_Styles.last_line_height_)
    pRect->height -= m_fLineSpace - m_fFontSize;
}

bool CFDE_TextOut::RetrieveLineWidth(CFX_BreakType dwBreakStatus,
                                     float* pStartPos,
                                     float* pWidth,
                                     float* pHeight) {
  if (CFX_BreakTypeNoneOrPiece(dwBreakStatus))
    return false;

  float fLineStep = (m_fLineSpace > m_fFontSize) ? m_fLineSpace : m_fFontSize;
  float fLineWidth = 0.0f;
  for (int32_t i = 0; i < m_pTxtBreak->CountBreakPieces(); i++) {
    const CFX_BreakPiece* pPiece = m_pTxtBreak->GetBreakPieceUnstable(i);
    fLineWidth += static_cast<float>(pPiece->m_iWidth) / 20000.0f;
    *pStartPos = std::min(*pStartPos,
                          static_cast<float>(pPiece->m_iStartPos) / 20000.0f);
  }
  m_pTxtBreak->ClearBreakPieces();

  if (dwBreakStatus == CFX_BreakType::Paragraph)
    m_pTxtBreak->Reset();
  if (!m_Styles.line_wrap_ && dwBreakStatus == CFX_BreakType::Line) {
    *pWidth += fLineWidth;
  } else {
    *pWidth = std::max(*pWidth, fLineWidth);
    *pHeight += fLineStep;
  }
  ++m_iTotalLines;
  return true;
}

void CFDE_TextOut::DrawLogicText(CFX_RenderDevice* device,
                                 WideStringView str,
                                 const CFX_RectF& rect) {
  ASSERT(m_pFont);
  ASSERT(m_fFontSize >= 1.0f);

  if (str.IsEmpty())
    return;
  if (rect.width < m_fFontSize || rect.height < m_fFontSize)
    return;

  float fLineWidth = rect.width;
  m_pTxtBreak->SetLineWidth(fLineWidth);
  m_ttoLines.clear();
  m_wsText.clear();

  LoadText(WideString(str), rect);
  Reload(rect);
  DoAlignment(rect);

  if (!device || m_ttoLines.empty())
    return;

  CFX_RectF rtClip = m_Matrix.TransformRect(CFX_RectF());
  device->SaveState();
  if (rtClip.Width() > 0.0f && rtClip.Height() > 0.0f)
    device->SetClip_Rect(rtClip);

  for (auto& line : m_ttoLines) {
    int32_t iPieces = line.GetSize();
    for (int32_t j = 0; j < iPieces; j++) {
      FDE_TTOPIECE* pPiece = line.GetPtrAt(j);
      if (!pPiece)
        continue;

      size_t szCount = GetDisplayPos(pPiece);
      if (szCount > 0) {
        CFDE_TextOut::DrawString(device, m_TxtColor, m_pFont,
                                 {m_CharPos.data(), szCount}, m_fFontSize,
                                 &m_Matrix);
      }
    }
  }
  device->RestoreState(false);
}

void CFDE_TextOut::LoadText(const WideString& str, const CFX_RectF& rect) {
  ASSERT(!str.IsEmpty());

  m_wsText = str;

  if (pdfium::CollectionSize<size_t>(m_CharWidths) < str.GetLength())
    m_CharWidths.resize(str.GetLength(), 0);

  float fLineStep = (m_fLineSpace > m_fFontSize) ? m_fLineSpace : m_fFontSize;
  float fLineStop = rect.bottom();
  m_fLinePos = rect.top;
  int32_t iStartChar = 0;
  int32_t iPieceWidths = 0;
  CFX_BreakType dwBreakStatus;
  bool bRet = false;
  for (const auto& wch : str) {
    dwBreakStatus = m_pTxtBreak->AppendChar(wch);
    if (CFX_BreakTypeNoneOrPiece(dwBreakStatus))
      continue;

    bool bEndofLine =
        RetrievePieces(dwBreakStatus, false, rect, &iStartChar, &iPieceWidths);
    if (bEndofLine &&
        (m_Styles.line_wrap_ || dwBreakStatus == CFX_BreakType::Paragraph ||
         dwBreakStatus == CFX_BreakType::Page)) {
      iPieceWidths = 0;
      ++m_iCurLine;
      m_fLinePos += fLineStep;
    }
    if (m_fLinePos + fLineStep > fLineStop) {
      int32_t iCurLine = bEndofLine ? m_iCurLine - 1 : m_iCurLine;
      m_ttoLines[iCurLine].SetNewReload(true);
      bRet = true;
      break;
    }
  }

  dwBreakStatus = m_pTxtBreak->EndBreak(CFX_BreakType::Paragraph);
  if (!CFX_BreakTypeNoneOrPiece(dwBreakStatus) && !bRet)
    RetrievePieces(dwBreakStatus, false, rect, &iStartChar, &iPieceWidths);

  m_pTxtBreak->ClearBreakPieces();
  m_pTxtBreak->Reset();
}

bool CFDE_TextOut::RetrievePieces(CFX_BreakType dwBreakStatus,
                                  bool bReload,
                                  const CFX_RectF& rect,
                                  int32_t* pStartChar,
                                  int32_t* pPieceWidths) {
  float fLineStep = (m_fLineSpace > m_fFontSize) ? m_fLineSpace : m_fFontSize;
  bool bNeedReload = false;
  int32_t iLineWidth = FXSYS_round(rect.Width() * 20000.0f);
  int32_t iCount = m_pTxtBreak->CountBreakPieces();
  for (int32_t i = 0; i < iCount; i++) {
    const CFX_BreakPiece* pPiece = m_pTxtBreak->GetBreakPieceUnstable(i);
    int32_t iPieceChars = pPiece->GetLength();
    int32_t iChar = *pStartChar;
    int32_t iWidth = 0;
    int32_t j = 0;
    for (; j < iPieceChars; j++) {
      const CFX_Char* pTC = pPiece->GetChar(j);
      int32_t iCurCharWidth = pTC->m_iCharWidth > 0 ? pTC->m_iCharWidth : 0;
      if (m_Styles.single_line_ || !m_Styles.line_wrap_) {
        if (iLineWidth - *pPieceWidths - iWidth < iCurCharWidth) {
          bNeedReload = true;
          break;
        }
      }
      iWidth += iCurCharWidth;
      m_CharWidths[iChar++] = iCurCharWidth;
    }

    if (j == 0 && !bReload) {
      m_ttoLines[m_iCurLine].SetNewReload(true);
    } else if (j > 0) {
      FDE_TTOPIECE ttoPiece;
      ttoPiece.iStartChar = *pStartChar;
      ttoPiece.iChars = j;
      ttoPiece.dwCharStyles = pPiece->m_dwCharStyles;
      ttoPiece.rtPiece = CFX_RectF(
          rect.left + static_cast<float>(pPiece->m_iStartPos) / 20000.0f,
          m_fLinePos, iWidth / 20000.0f, fLineStep);

      if (FX_IsOdd(pPiece->m_iBidiLevel))
        ttoPiece.dwCharStyles |= FX_TXTCHARSTYLE_OddBidiLevel;

      AppendPiece(ttoPiece, bNeedReload, (bReload && i == iCount - 1));
    }
    *pStartChar += iPieceChars;
    *pPieceWidths += iWidth;
  }
  m_pTxtBreak->ClearBreakPieces();

  return m_Styles.single_line_ || m_Styles.line_wrap_ || bNeedReload ||
         dwBreakStatus == CFX_BreakType::Paragraph;
}

void CFDE_TextOut::AppendPiece(const FDE_TTOPIECE& ttoPiece,
                               bool bNeedReload,
                               bool bEnd) {
  if (m_iCurLine >= pdfium::CollectionSize<int32_t>(m_ttoLines)) {
    CFDE_TTOLine ttoLine;
    ttoLine.SetNewReload(bNeedReload);

    m_iCurPiece = ttoLine.AddPiece(m_iCurPiece, ttoPiece);
    m_ttoLines.push_back(ttoLine);
    m_iCurLine = pdfium::CollectionSize<int32_t>(m_ttoLines) - 1;
  } else {
    CFDE_TTOLine* pLine = &m_ttoLines[m_iCurLine];
    pLine->SetNewReload(bNeedReload);

    m_iCurPiece = pLine->AddPiece(m_iCurPiece, ttoPiece);
    if (bEnd) {
      int32_t iPieces = pLine->GetSize();
      if (m_iCurPiece < iPieces)
        pLine->RemoveLast(iPieces - m_iCurPiece - 1);
    }
  }
  if (!bEnd && bNeedReload)
    m_iCurPiece = 0;
}

void CFDE_TextOut::Reload(const CFX_RectF& rect) {
  int i = 0;
  for (auto& line : m_ttoLines) {
    if (line.GetNewReload()) {
      m_iCurLine = i;
      m_iCurPiece = 0;
      ReloadLinePiece(&line, rect);
    }
    ++i;
  }
}

void CFDE_TextOut::ReloadLinePiece(CFDE_TTOLine* pLine, const CFX_RectF& rect) {
  const wchar_t* pwsStr = m_wsText.c_str();
  int32_t iPieceWidths = 0;

  FDE_TTOPIECE* pPiece = pLine->GetPtrAt(0);
  int32_t iStartChar = pPiece->iStartChar;
  int32_t iPieceCount = pLine->GetSize();
  int32_t iPieceIndex = 0;
  CFX_BreakType dwBreakStatus = CFX_BreakType::None;
  m_fLinePos = pPiece->rtPiece.top;
  while (iPieceIndex < iPieceCount) {
    int32_t iStar = iStartChar;
    int32_t iEnd = pPiece->iChars + iStar;
    while (iStar < iEnd) {
      dwBreakStatus = m_pTxtBreak->AppendChar(*(pwsStr + iStar));
      if (!CFX_BreakTypeNoneOrPiece(dwBreakStatus))
        RetrievePieces(dwBreakStatus, true, rect, &iStartChar, &iPieceWidths);

      ++iStar;
    }
    ++iPieceIndex;
    pPiece = pLine->GetPtrAt(iPieceIndex);
  }

  dwBreakStatus = m_pTxtBreak->EndBreak(CFX_BreakType::Paragraph);
  if (!CFX_BreakTypeNoneOrPiece(dwBreakStatus))
    RetrievePieces(dwBreakStatus, true, rect, &iStartChar, &iPieceWidths);

  m_pTxtBreak->Reset();
}

void CFDE_TextOut::DoAlignment(const CFX_RectF& rect) {
  if (m_ttoLines.empty())
    return;

  FDE_TTOPIECE* pFirstPiece = m_ttoLines.back().GetPtrAt(0);
  if (!pFirstPiece)
    return;

  float fInc = rect.bottom() - pFirstPiece->rtPiece.bottom();
  if (TextAlignmentVerticallyCentered(m_iAlignment))
    fInc /= 2.0f;
  else if (IsTextAlignmentTop(m_iAlignment))
    fInc = 0.0f;

  if (fInc < 1.0f)
    return;

  for (auto& line : m_ttoLines) {
    int32_t iPieces = line.GetSize();
    for (int32_t j = 0; j < iPieces; j++)
      line.GetPtrAt(j)->rtPiece.top += fInc;
  }
}

size_t CFDE_TextOut::GetDisplayPos(FDE_TTOPIECE* pPiece) {
  ASSERT(pPiece->iChars >= 0);

  if (pdfium::CollectionSize<int32_t>(m_CharPos) < pPiece->iChars)
    m_CharPos.resize(pPiece->iChars, FXTEXT_CHARPOS());

  CFX_TxtBreak::Run tr;
  tr.wsStr = m_wsText + pPiece->iStartChar;
  tr.pWidths = &m_CharWidths[pPiece->iStartChar];
  tr.iLength = pPiece->iChars;
  tr.pFont = m_pFont;
  tr.fFontSize = m_fFontSize;
  tr.dwStyles = m_dwTxtBkStyles;
  tr.dwCharStyles = pPiece->dwCharStyles;
  tr.pRect = &pPiece->rtPiece;

  return m_pTxtBreak->GetDisplayPos(&tr, m_CharPos.data());
}

CFDE_TextOut::CFDE_TTOLine::CFDE_TTOLine() : m_bNewReload(false) {}

CFDE_TextOut::CFDE_TTOLine::CFDE_TTOLine(const CFDE_TTOLine& ttoLine)
    : m_pieces(5) {
  m_bNewReload = ttoLine.m_bNewReload;
  m_pieces = ttoLine.m_pieces;
}

CFDE_TextOut::CFDE_TTOLine::~CFDE_TTOLine() {}

int32_t CFDE_TextOut::CFDE_TTOLine::AddPiece(int32_t index,
                                             const FDE_TTOPIECE& ttoPiece) {
  if (index >= pdfium::CollectionSize<int32_t>(m_pieces)) {
    m_pieces.push_back(ttoPiece);
    return pdfium::CollectionSize<int32_t>(m_pieces);
  }
  m_pieces[index] = ttoPiece;
  return index;
}

int32_t CFDE_TextOut::CFDE_TTOLine::GetSize() const {
  return pdfium::CollectionSize<int32_t>(m_pieces);
}

FDE_TTOPIECE* CFDE_TextOut::CFDE_TTOLine::GetPtrAt(int32_t index) {
  return pdfium::IndexInBounds(m_pieces, index) ? &m_pieces[index] : nullptr;
}

void CFDE_TextOut::CFDE_TTOLine::RemoveLast(int32_t icount) {
  if (icount < 0)
    return;
  m_pieces.erase(
      m_pieces.end() -
          std::min(icount, pdfium::CollectionSize<int32_t>(m_pieces)),
      m_pieces.end());
}
