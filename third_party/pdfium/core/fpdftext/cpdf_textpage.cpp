// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdftext/cpdf_textpage.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/page/cpdf_form.h"
#include "core/fpdfapi/page/cpdf_formobject.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/page/cpdf_textobject.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdftext/unicodenormalizationdata.h"
#include "core/fxcrt/fx_bidi.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_unicode.h"
#include "third_party/base/stl_util.h"

namespace {

constexpr float kDefaultFontSize = 1.0f;
constexpr float kSizeEpsilon = 0.01f;

const uint16_t* const g_UnicodeData_Normalization_Maps[] = {
    g_UnicodeData_Normalization_Map2, g_UnicodeData_Normalization_Map3,
    g_UnicodeData_Normalization_Map4};

float NormalizeThreshold(float threshold, int t1, int t2, int t3) {
  ASSERT(t1 < t2);
  ASSERT(t2 < t3);
  if (threshold < t1)
    return threshold / 2.0f;
  if (threshold < t2)
    return threshold / 4.0f;
  if (threshold < t3)
    return threshold / 5.0f;
  return threshold / 6.0f;
}

float CalculateBaseSpace(const CPDF_TextObject* pTextObj,
                         const CFX_Matrix& matrix) {
  const size_t nItems = pTextObj->CountItems();
  if (!pTextObj->m_TextState.GetCharSpace() || nItems < 3)
    return 0.0f;

  bool bAllChar = true;
  float spacing =
      matrix.TransformDistance(pTextObj->m_TextState.GetCharSpace());
  float baseSpace = spacing;
  for (size_t i = 0; i < nItems; ++i) {
    CPDF_TextObjectItem item;
    pTextObj->GetItemInfo(i, &item);
    if (item.m_CharCode == static_cast<uint32_t>(-1)) {
      float fontsize_h = pTextObj->m_TextState.GetFontSizeH();
      float kerning = -fontsize_h * item.m_Origin.x / 1000;
      baseSpace = std::min(baseSpace, kerning + spacing);
      bAllChar = false;
    }
  }
  if (baseSpace < 0.0 || (nItems == 3 && !bAllChar))
    return 0.0f;

  return baseSpace;
}

size_t Unicode_GetNormalization(wchar_t wch, wchar_t* pDst) {
  wch = wch & 0xFFFF;
  wchar_t wFind = g_UnicodeData_Normalization[wch];
  if (!wFind) {
    if (pDst)
      *pDst = wch;
    return 1;
  }
  if (wFind >= 0x8000) {
    const uint16_t* pMap = g_UnicodeData_Normalization_Map1 + (wFind - 0x8000);
    if (pDst)
      *pDst = *pMap;
    return 1;
  }

  wch = wFind & 0x0FFF;
  wFind >>= 12;
  const uint16_t* pMap = g_UnicodeData_Normalization_Maps[wFind - 2] + wch;
  if (wFind == 4)
    wFind = static_cast<wchar_t>(*pMap++);

  if (pDst) {
    wchar_t n = wFind;
    while (n--)
      *pDst++ = *pMap++;
  }
  return static_cast<size_t>(wFind);
}

float MaskPercentFilled(const std::vector<bool>& mask,
                        int32_t start,
                        int32_t end) {
  if (start >= end)
    return 0;
  float count = std::count_if(mask.begin() + start, mask.begin() + end,
                              [](bool r) { return r; });
  return count / (end - start);
}

bool IsHyphenCode(wchar_t c) {
  return c == 0x2D || c == 0xAD;
}

bool IsRectIntersect(const CFX_FloatRect& rect1, const CFX_FloatRect& rect2) {
  CFX_FloatRect rect = rect1;
  rect.Intersect(rect2);
  return !rect.IsEmpty();
}

bool IsRightToLeft(const CPDF_TextObject& text_obj, const CPDF_Font& font) {
  const size_t nItems = text_obj.CountItems();
  WideString str;
  str.Reserve(nItems);
  for (size_t i = 0; i < nItems; ++i) {
    CPDF_TextObjectItem item;
    text_obj.GetItemInfo(i, &item);
    if (item.m_CharCode == static_cast<uint32_t>(-1))
      continue;
    WideString wstrItem = font.UnicodeFromCharCode(item.m_CharCode);
    wchar_t wChar = !wstrItem.IsEmpty() ? wstrItem[0] : 0;
    if (wChar == 0)
      wChar = item.m_CharCode;
    if (wChar)
      str += wChar;
  }
  return CFX_BidiString(str).OverallDirection() == CFX_BidiChar::RIGHT;
}

uint32_t GetCharWidth(uint32_t charCode, CPDF_Font* pFont) {
  if (charCode == CPDF_Font::kInvalidCharCode)
    return 0;

  uint32_t w = pFont->GetCharWidthF(charCode);
  if (w > 0)
    return w;

  ByteString str;
  pFont->AppendChar(&str, charCode);
  w = pFont->GetStringWidth(str.AsStringView());
  if (w > 0)
    return w;

  FX_RECT rect = pFont->GetCharBBox(charCode);
  if (!rect.Valid())
    return 0;

  return std::max(rect.Width(), 0);
}

bool GenerateSpace(const CFX_PointF& pos,
                   float last_pos,
                   float this_width,
                   float last_width,
                   float threshold) {
  if (fabs(last_pos + last_width - pos.x) <= threshold)
    return false;

  float threshold_pos = threshold + last_width;
  float pos_difference = pos.x - last_pos;
  if (fabs(pos_difference) > threshold_pos)
    return true;
  if (pos.x < 0 && -threshold_pos > pos_difference)
    return true;
  return pos_difference > this_width + last_width;
}

bool EndHorizontalLine(const CFX_FloatRect& this_rect,
                       const CFX_FloatRect& prev_rect) {
  if (this_rect.Height() <= 4.5 || prev_rect.Height() <= 4.5)
    return false;

  float top = std::min(this_rect.top, prev_rect.top);
  float bottom = std::max(this_rect.bottom, prev_rect.bottom);
  return bottom >= top;
}

bool EndVerticalLine(const CFX_FloatRect& this_rect,
                     const CFX_FloatRect& prev_rect,
                     const CFX_FloatRect& curline_rect,
                     float this_fontsize,
                     float prev_fontsize) {
  if (this_rect.Width() <= this_fontsize * 0.1f ||
      prev_rect.Width() <= prev_fontsize * 0.1f) {
    return false;
  }

  float left = std::max(this_rect.left, curline_rect.left);
  float right = std::min(this_rect.right, curline_rect.right);
  return right <= left;
}

}  // namespace

PDFTEXT_Obj::PDFTEXT_Obj() {}

PDFTEXT_Obj::PDFTEXT_Obj(const PDFTEXT_Obj& that) = default;

PDFTEXT_Obj::~PDFTEXT_Obj() {}

FPDF_CHAR_INFO::FPDF_CHAR_INFO() {}

FPDF_CHAR_INFO::~FPDF_CHAR_INFO() {}

PAGECHAR_INFO::PAGECHAR_INFO() {}

PAGECHAR_INFO::PAGECHAR_INFO(const PAGECHAR_INFO&) = default;

PAGECHAR_INFO::~PAGECHAR_INFO() {}

CPDF_TextPage::CPDF_TextPage(const CPDF_Page* pPage, FPDFText_Direction flags)
    : m_pPage(pPage), m_parserflag(flags) {
  m_TextBuf.SetAllocStep(10240);
  const FX_RECT rect(0, 0, static_cast<int>(pPage->GetPageWidth()),
                     static_cast<int>(pPage->GetPageHeight()));
  m_DisplayMatrix = pPage->GetDisplayMatrix(rect, 0);
}

CPDF_TextPage::~CPDF_TextPage() {}

bool CPDF_TextPage::IsControlChar(const PAGECHAR_INFO& charInfo) {
  switch (charInfo.m_Unicode) {
    case 0x2:
    case 0x3:
    case 0x93:
    case 0x94:
    case 0x96:
    case 0x97:
    case 0x98:
    case 0xfffe:
      return charInfo.m_Flag != FPDFTEXT_CHAR_HYPHEN;
    default:
      return false;
  }
}

void CPDF_TextPage::ParseTextPage() {
  m_bIsParsed = false;
  m_TextBuf.Clear();
  m_CharList.clear();
  m_pPreTextObj = nullptr;
  ProcessObject();

  m_bIsParsed = true;
  m_CharIndex.clear();
  const int nCount = CountChars();
  if (nCount)
    m_CharIndex.push_back(0);

  for (int i = 0; i < nCount; ++i) {
    int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
    const PAGECHAR_INFO& charinfo = m_CharList[i];
    if (charinfo.m_Flag == FPDFTEXT_CHAR_GENERATED ||
        (charinfo.m_Unicode != 0 && !IsControlChar(charinfo)) ||
        (charinfo.m_Unicode == 0 && charinfo.m_CharCode != 0)) {
      if (indexSize % 2) {
        m_CharIndex.push_back(1);
      } else {
        if (indexSize <= 0)
          continue;
        m_CharIndex[indexSize - 1] += 1;
      }
    } else {
      if (indexSize % 2) {
        if (indexSize <= 0)
          continue;
        m_CharIndex[indexSize - 1] = i + 1;
      } else {
        m_CharIndex.push_back(i + 1);
      }
    }
  }
  int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
  if (indexSize % 2)
    m_CharIndex.erase(m_CharIndex.begin() + indexSize - 1);
}

int CPDF_TextPage::CountChars() const {
  return pdfium::CollectionSize<int>(m_CharList);
}

int CPDF_TextPage::CharIndexFromTextIndex(int TextIndex) const {
  int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
  int count = 0;
  for (int i = 0; i < indexSize; i += 2) {
    count += m_CharIndex[i + 1];
    if (count > TextIndex)
      return TextIndex - count + m_CharIndex[i + 1] + m_CharIndex[i];
  }
  return -1;
}

int CPDF_TextPage::TextIndexFromCharIndex(int CharIndex) const {
  int indexSize = pdfium::CollectionSize<int>(m_CharIndex);
  int count = 0;
  for (int i = 0; i < indexSize; i += 2) {
    count += m_CharIndex[i + 1];
    if (m_CharIndex[i + 1] + m_CharIndex[i] > CharIndex) {
      if (CharIndex - m_CharIndex[i] < 0)
        return -1;

      return CharIndex - m_CharIndex[i] + count - m_CharIndex[i + 1];
    }
  }
  return -1;
}

std::vector<CFX_FloatRect> CPDF_TextPage::GetRectArray(int start,
                                                       int nCount) const {
  std::vector<CFX_FloatRect> rects;
  if (start < 0 || nCount == 0 || !m_bIsParsed)
    return rects;

  const int nCharListSize = CountChars();
  if (start >= nCharListSize)
    return rects;

  if (nCount < 0 || start + nCount > nCharListSize)
    nCount = nCharListSize - start;
  ASSERT(nCount > 0);

  CPDF_TextObject* pCurObj = nullptr;
  CFX_FloatRect rect;
  int curPos = start;
  bool bFlagNewRect = true;
  while (nCount--) {
    PAGECHAR_INFO info_curchar = m_CharList[curPos++];
    if (info_curchar.m_Flag == FPDFTEXT_CHAR_GENERATED)
      continue;
    if (info_curchar.m_CharBox.Width() < kSizeEpsilon ||
        info_curchar.m_CharBox.Height() < kSizeEpsilon) {
      continue;
    }
    if (!pCurObj)
      pCurObj = info_curchar.m_pTextObj.Get();
    if (pCurObj != info_curchar.m_pTextObj) {
      rects.push_back(rect);
      pCurObj = info_curchar.m_pTextObj.Get();
      bFlagNewRect = true;
    }
    if (bFlagNewRect) {
      bFlagNewRect = false;
      rect = info_curchar.m_CharBox;
      rect.Normalize();
      continue;
    }
    info_curchar.m_CharBox.Normalize();
    rect.left = std::min(rect.left, info_curchar.m_CharBox.left);
    rect.right = std::max(rect.right, info_curchar.m_CharBox.right);
    rect.top = std::max(rect.top, info_curchar.m_CharBox.top);
    rect.bottom = std::min(rect.bottom, info_curchar.m_CharBox.bottom);
  }
  rects.push_back(rect);
  return rects;
}

int CPDF_TextPage::GetIndexAtPos(const CFX_PointF& point,
                                 const CFX_SizeF& tolerance) const {
  if (!m_bIsParsed)
    return -3;

  int pos;
  int NearPos = -1;
  double xdif = 5000;
  double ydif = 5000;
  const int nCount = CountChars();
  for (pos = 0; pos < nCount; ++pos) {
    PAGECHAR_INFO charinfo = m_CharList[pos];
    CFX_FloatRect charrect = charinfo.m_CharBox;
    if (charrect.Contains(point))
      break;

    if (tolerance.width <= 0 && tolerance.height <= 0)
      continue;

    CFX_FloatRect charRectExt;
    charrect.Normalize();
    charRectExt.left = charrect.left - tolerance.width / 2;
    charRectExt.right = charrect.right + tolerance.width / 2;
    charRectExt.top = charrect.top + tolerance.height / 2;
    charRectExt.bottom = charrect.bottom - tolerance.height / 2;
    if (!charRectExt.Contains(point))
      continue;

    double curXdif =
        std::min(fabs(point.x - charrect.left), fabs(point.x - charrect.right));
    double curYdif =
        std::min(fabs(point.y - charrect.bottom), fabs(point.y - charrect.top));
    if (curYdif + curXdif < xdif + ydif) {
      ydif = curYdif;
      xdif = curXdif;
      NearPos = pos;
    }
  }
  return pos < nCount ? pos : NearPos;
}

WideString CPDF_TextPage::GetTextByPredicate(
    const std::function<bool(const PAGECHAR_INFO&)>& predicate) const {
  if (!m_bIsParsed)
    return WideString();

  float posy = 0;
  bool IsContainPreChar = false;
  bool IsAddLineFeed = false;
  WideString strText;
  for (const auto& charinfo : m_CharList) {
    if (predicate(charinfo)) {
      if (fabs(posy - charinfo.m_Origin.y) > 0 && !IsContainPreChar &&
          IsAddLineFeed) {
        posy = charinfo.m_Origin.y;
        if (!strText.IsEmpty())
          strText += L"\r\n";
      }
      IsContainPreChar = true;
      IsAddLineFeed = false;
      if (charinfo.m_Unicode)
        strText += charinfo.m_Unicode;
    } else if (charinfo.m_Unicode == L' ') {
      if (IsContainPreChar) {
        strText += L' ';
        IsContainPreChar = false;
        IsAddLineFeed = false;
      }
    } else {
      IsContainPreChar = false;
      IsAddLineFeed = true;
    }
  }
  return strText;
}

WideString CPDF_TextPage::GetTextByRect(const CFX_FloatRect& rect) const {
  return GetTextByPredicate([&rect](const PAGECHAR_INFO& charinfo) {
    return IsRectIntersect(rect, charinfo.m_CharBox);
  });
}

WideString CPDF_TextPage::GetTextByObject(
    const CPDF_TextObject* pTextObj) const {
  return GetTextByPredicate([pTextObj](const PAGECHAR_INFO& charinfo) {
    return charinfo.m_pTextObj == pTextObj;
  });
}

void CPDF_TextPage::GetCharInfo(int index, FPDF_CHAR_INFO* info) const {
  if (!m_bIsParsed || !pdfium::IndexInBounds(m_CharList, index))
    return;

  const PAGECHAR_INFO& charinfo = m_CharList[index];
  info->m_Charcode = charinfo.m_CharCode;
  info->m_Origin = charinfo.m_Origin;
  info->m_Unicode = charinfo.m_Unicode;
  info->m_Flag = charinfo.m_Flag;
  info->m_CharBox = charinfo.m_CharBox;
  info->m_pTextObj = charinfo.m_pTextObj;
  bool bHasFont = charinfo.m_pTextObj && charinfo.m_pTextObj->GetFont();
  info->m_FontSize =
      bHasFont ? charinfo.m_pTextObj->GetFontSize() : kDefaultFontSize;
  info->m_Matrix = charinfo.m_Matrix;
}

WideString CPDF_TextPage::GetPageText(int start, int count) const {
  if (start < 0 || start >= CountChars() || count <= 0 || !m_bIsParsed ||
      m_CharList.empty() || m_TextBuf.GetLength() == 0) {
    return WideString();
  }

  const int count_chars = CountChars();
  int text_start = TextIndexFromCharIndex(start);

  // If the character at |start| is a non-printing character, then
  // TextIndexFromCharIndex will return -1, so scan ahead to the first printing
  // character.
  while (text_start < 0) {
    if (start >= count_chars)
      return WideString();
    start++;
    text_start = TextIndexFromCharIndex(start);
  }

  count = std::min(count, count_chars - start);

  int last = start + count - 1;
  int text_last = TextIndexFromCharIndex(last);

  // If the character at |last| is a non-printing character, then
  // TextIndexFromCharIndex will return -1, so scan back to the last printing
  // character.
  while (text_last < 0) {
    if (last < text_start)
      return WideString();

    last--;
    text_last = TextIndexFromCharIndex(last);
  }

  if (text_last < text_start)
    return WideString();

  int text_count = text_last - text_start + 1;

  return WideString(m_TextBuf.AsStringView().Mid(
      static_cast<size_t>(text_start), static_cast<size_t>(text_count)));
}

int CPDF_TextPage::CountRects(int start, int nCount) {
  if (!m_bIsParsed || start < 0)
    return -1;

  m_SelRects = GetRectArray(start, nCount);
  return pdfium::CollectionSize<int>(m_SelRects);
}

bool CPDF_TextPage::GetRect(int rectIndex, CFX_FloatRect* pRect) const {
  if (!m_bIsParsed || !pdfium::IndexInBounds(m_SelRects, rectIndex))
    return false;

  *pRect = m_SelRects[rectIndex];
  return true;
}

CPDF_TextPage::TextOrientation CPDF_TextPage::FindTextlineFlowOrientation()
    const {
  if (m_pPage->GetPageObjectList()->empty())
    return TextOrientation::Unknown;

  const int32_t nPageWidth = static_cast<int32_t>(m_pPage->GetPageWidth());
  const int32_t nPageHeight = static_cast<int32_t>(m_pPage->GetPageHeight());
  if (nPageWidth <= 0 || nPageHeight <= 0)
    return TextOrientation::Unknown;

  std::vector<bool> nHorizontalMask(nPageWidth);
  std::vector<bool> nVerticalMask(nPageHeight);
  float fLineHeight = 0.0f;
  int32_t nStartH = nPageWidth;
  int32_t nEndH = 0;
  int32_t nStartV = nPageHeight;
  int32_t nEndV = 0;
  for (const auto& pPageObj : *m_pPage->GetPageObjectList()) {
    if (!pPageObj->IsText())
      continue;

    int32_t minH = std::max(static_cast<int32_t>(pPageObj->GetRect().left), 0);
    int32_t maxH =
        std::min(static_cast<int32_t>(pPageObj->GetRect().right), nPageWidth);
    int32_t minV =
        std::max(static_cast<int32_t>(pPageObj->GetRect().bottom), 0);
    int32_t maxV =
        std::min(static_cast<int32_t>(pPageObj->GetRect().top), nPageHeight);
    if (minH >= maxH || minV >= maxV)
      continue;

    for (int32_t i = minH; i < maxH; ++i)
      nHorizontalMask[i] = true;
    for (int32_t i = minV; i < maxV; ++i)
      nVerticalMask[i] = true;

    nStartH = std::min(nStartH, minH);
    nEndH = std::max(nEndH, maxH);
    nStartV = std::min(nStartV, minV);
    nEndV = std::max(nEndV, maxV);

    if (fLineHeight <= 0.0f)
      fLineHeight = pPageObj->GetRect().Height();
  }
  const int32_t nDoubleLineHeight = 2 * fLineHeight;
  if ((nEndV - nStartV) < nDoubleLineHeight)
    return TextOrientation::Horizontal;
  if ((nEndH - nStartH) < nDoubleLineHeight)
    return TextOrientation::Vertical;

  const float nSumH = MaskPercentFilled(nHorizontalMask, nStartH, nEndH);
  if (nSumH > 0.8f)
    return TextOrientation::Horizontal;

  const float nSumV = MaskPercentFilled(nVerticalMask, nStartV, nEndV);
  if (nSumH > nSumV)
    return TextOrientation::Horizontal;
  if (nSumH < nSumV)
    return TextOrientation::Vertical;
  return TextOrientation::Unknown;
}

void CPDF_TextPage::AppendGeneratedCharacter(wchar_t unicode,
                                             const CFX_Matrix& formMatrix) {
  Optional<PAGECHAR_INFO> pGenerateChar = GenerateCharInfo(unicode);
  if (!pGenerateChar)
    return;

  m_TextBuf.AppendChar(unicode);
  if (!formMatrix.IsIdentity())
    pGenerateChar->m_Matrix = formMatrix;
  m_CharList.push_back(*pGenerateChar);
}

void CPDF_TextPage::ProcessObject() {
  if (m_pPage->GetPageObjectList()->empty())
    return;

  m_TextlineDir = FindTextlineFlowOrientation();
  const CPDF_PageObjectList* pObjList = m_pPage->GetPageObjectList();
  for (auto it = pObjList->begin(); it != pObjList->end(); ++it) {
    CPDF_PageObject* pObj = it->get();
    if (!pObj)
      continue;

    CFX_Matrix matrix;
    if (pObj->IsText())
      ProcessTextObject(pObj->AsText(), matrix, pObjList, it);
    else if (pObj->IsForm())
      ProcessFormObject(pObj->AsForm(), matrix);
  }
  for (const auto& obj : m_LineObj)
    ProcessTextObject(obj);

  m_LineObj.clear();
  CloseTempLine();
}

void CPDF_TextPage::ProcessFormObject(CPDF_FormObject* pFormObj,
                                      const CFX_Matrix& formMatrix) {
  const CPDF_PageObjectList* pObjectList =
      pFormObj->form()->GetPageObjectList();
  if (pObjectList->empty())
    return;

  CFX_Matrix curFormMatrix = pFormObj->form_matrix() * formMatrix;

  for (auto it = pObjectList->begin(); it != pObjectList->end(); ++it) {
    CPDF_PageObject* pPageObj = it->get();
    if (!pPageObj)
      continue;

    if (pPageObj->IsText())
      ProcessTextObject(pPageObj->AsText(), curFormMatrix, pObjectList, it);
    else if (pPageObj->IsForm())
      ProcessFormObject(pPageObj->AsForm(), curFormMatrix);
  }
}

void CPDF_TextPage::AddCharInfoByLRDirection(wchar_t wChar,
                                             const PAGECHAR_INFO& info) {
  PAGECHAR_INFO info2 = info;
  if (IsControlChar(info2)) {
    info2.m_Index = -1;
    m_CharList.push_back(info2);
    return;
  }

  info2.m_Index = m_TextBuf.GetLength();
  size_t nCount = 0;
  if (wChar >= 0xFB00 && wChar <= 0xFB06)
    nCount = Unicode_GetNormalization(wChar, nullptr);
  if (nCount == 0) {
    m_TextBuf.AppendChar(wChar);
    m_CharList.push_back(info2);
    return;
  }

  std::unique_ptr<wchar_t, FxFreeDeleter> pDst(FX_Alloc(wchar_t, nCount));
  Unicode_GetNormalization(wChar, pDst.get());
  for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
    info2.m_Unicode = pDst.get()[nIndex];
    info2.m_Flag = FPDFTEXT_CHAR_PIECE;
    m_TextBuf.AppendChar(info2.m_Unicode);
    m_CharList.push_back(info2);
  }
}

void CPDF_TextPage::AddCharInfoByRLDirection(wchar_t wChar,
                                             const PAGECHAR_INFO& info) {
  PAGECHAR_INFO info2 = info;
  if (IsControlChar(info2)) {
    info2.m_Index = -1;
    m_CharList.push_back(info2);
    return;
  }

  info2.m_Index = m_TextBuf.GetLength();
  wChar = FX_GetMirrorChar(wChar);
  size_t nCount = Unicode_GetNormalization(wChar, nullptr);
  if (nCount == 0) {
    info2.m_Unicode = wChar;
    m_TextBuf.AppendChar(info2.m_Unicode);
    m_CharList.push_back(info2);
    return;
  }

  std::unique_ptr<wchar_t, FxFreeDeleter> pDst(FX_Alloc(wchar_t, nCount));
  Unicode_GetNormalization(wChar, pDst.get());
  for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
    info2.m_Unicode = pDst.get()[nIndex];
    info2.m_Flag = FPDFTEXT_CHAR_PIECE;
    m_TextBuf.AppendChar(info2.m_Unicode);
    m_CharList.push_back(info2);
  }
}

void CPDF_TextPage::CloseTempLine() {
  if (m_TempCharList.empty())
    return;

  WideString str = m_TempTextBuf.MakeString();
  bool bPrevSpace = false;
  for (size_t i = 0; i < str.GetLength(); ++i) {
    if (str[i] != ' ') {
      bPrevSpace = false;
      continue;
    }
    if (bPrevSpace) {
      m_TempTextBuf.Delete(i, 1);
      m_TempCharList.erase(m_TempCharList.begin() + i);
      str.Delete(i);
      --i;
    }
    bPrevSpace = true;
  }
  CFX_BidiString bidi(str);
  if (m_parserflag == FPDFText_Direction::Right)
    bidi.SetOverallDirectionRight();
  CFX_BidiChar::Direction eCurrentDirection = bidi.OverallDirection();
  for (const auto& segment : bidi) {
    if (segment.direction == CFX_BidiChar::RIGHT ||
        (segment.direction == CFX_BidiChar::NEUTRAL &&
         eCurrentDirection == CFX_BidiChar::RIGHT)) {
      eCurrentDirection = CFX_BidiChar::RIGHT;
      for (int m = segment.start + segment.count; m > segment.start; --m)
        AddCharInfoByRLDirection(str[m - 1], m_TempCharList[m - 1]);
    } else {
      eCurrentDirection = CFX_BidiChar::LEFT;
      for (int m = segment.start; m < segment.start + segment.count; ++m)
        AddCharInfoByLRDirection(str[m], m_TempCharList[m]);
    }
  }
  m_TempCharList.clear();
  m_TempTextBuf.Delete(0, m_TempTextBuf.GetLength());
}

void CPDF_TextPage::ProcessTextObject(
    CPDF_TextObject* pTextObj,
    const CFX_Matrix& formMatrix,
    const CPDF_PageObjectList* pObjList,
    CPDF_PageObjectList::const_iterator ObjPos) {
  if (fabs(pTextObj->GetRect().Width()) < kSizeEpsilon)
    return;

  size_t count = m_LineObj.size();
  PDFTEXT_Obj Obj;
  Obj.m_pTextObj = pTextObj;
  Obj.m_formMatrix = formMatrix;
  if (count == 0) {
    m_LineObj.push_back(Obj);
    return;
  }
  if (IsSameAsPreTextObject(pTextObj, pObjList, ObjPos))
    return;

  PDFTEXT_Obj prev_Obj = m_LineObj[count - 1];
  size_t nItem = prev_Obj.m_pTextObj->CountItems();
  if (nItem == 0)
    return;

  CPDF_TextObjectItem item;
  prev_Obj.m_pTextObj->GetItemInfo(nItem - 1, &item);
  float prev_width =
      GetCharWidth(item.m_CharCode, prev_Obj.m_pTextObj->GetFont()) *
      prev_Obj.m_pTextObj->GetFontSize() / 1000;

  CFX_Matrix prev_matrix =
      prev_Obj.m_pTextObj->GetTextMatrix() * prev_Obj.m_formMatrix;
  prev_width = prev_matrix.TransformDistance(fabs(prev_width));
  pTextObj->GetItemInfo(0, &item);
  float this_width = GetCharWidth(item.m_CharCode, pTextObj->GetFont()) *
                     pTextObj->GetFontSize() / 1000;
  this_width = fabs(this_width);

  CFX_Matrix this_matrix = pTextObj->GetTextMatrix() * formMatrix;
  this_width = this_matrix.TransformDistance(fabs(this_width));

  float threshold = std::max(prev_width, this_width) / 4;
  CFX_PointF prev_pos = m_DisplayMatrix.Transform(
      prev_Obj.m_formMatrix.Transform(prev_Obj.m_pTextObj->GetPos()));
  CFX_PointF this_pos =
      m_DisplayMatrix.Transform(formMatrix.Transform(pTextObj->GetPos()));
  if (fabs(this_pos.y - prev_pos.y) > threshold * 2) {
    for (size_t i = 0; i < count; ++i)
      ProcessTextObject(m_LineObj[i]);
    m_LineObj.clear();
    m_LineObj.push_back(Obj);
    return;
  }

  for (size_t i = count; i > 0; --i) {
    PDFTEXT_Obj prev_text_obj = m_LineObj[i - 1];
    CFX_PointF new_prev_pos =
        m_DisplayMatrix.Transform(prev_text_obj.m_formMatrix.Transform(
            prev_text_obj.m_pTextObj->GetPos()));
    if (this_pos.x >= new_prev_pos.x) {
      m_LineObj.insert(m_LineObj.begin() + i, Obj);
      return;
    }
  }
  m_LineObj.insert(m_LineObj.begin(), Obj);
}

FPDFText_MarkedContent CPDF_TextPage::PreMarkedContent(PDFTEXT_Obj Obj) {
  CPDF_TextObject* pTextObj = Obj.m_pTextObj.Get();
  size_t nContentMarks = pTextObj->m_ContentMarks.CountItems();
  if (nContentMarks == 0)
    return FPDFText_MarkedContent::Pass;

  WideString actText;
  bool bExist = false;
  const CPDF_Dictionary* pDict = nullptr;
  for (size_t i = 0; i < nContentMarks; ++i) {
    const CPDF_ContentMarkItem* item = pTextObj->m_ContentMarks.GetItem(i);
    pDict = item->GetParam();
    if (!pDict)
      continue;
    const CPDF_String* temp = ToString(pDict->GetObjectFor("ActualText"));
    if (temp) {
      bExist = true;
      actText = temp->GetUnicodeText();
    }
  }
  if (!bExist)
    return FPDFText_MarkedContent::Pass;

  if (m_pPreTextObj) {
    const CPDF_ContentMarks& marks = m_pPreTextObj->m_ContentMarks;
    if (marks.CountItems() == nContentMarks &&
        marks.GetItem(nContentMarks - 1)->GetParam() == pDict) {
      return FPDFText_MarkedContent::Done;
    }
  }

  if (actText.IsEmpty())
    return FPDFText_MarkedContent::Pass;

  CPDF_Font* pFont = pTextObj->GetFont();
  bExist = false;
  for (size_t i = 0; i < actText.GetLength(); ++i) {
    if (pFont->CharCodeFromUnicode(actText[i]) != CPDF_Font::kInvalidCharCode) {
      bExist = true;
      break;
    }
  }
  if (!bExist)
    return FPDFText_MarkedContent::Pass;

  bExist = false;
  for (size_t i = 0; i < actText.GetLength(); ++i) {
    wchar_t wChar = actText[i];
    if ((wChar > 0x80 && wChar < 0xFFFD) || (wChar <= 0x80 && isprint(wChar))) {
      bExist = true;
      break;
    }
  }
  if (!bExist)
    return FPDFText_MarkedContent::Done;

  return FPDFText_MarkedContent::Delay;
}

void CPDF_TextPage::ProcessMarkedContent(PDFTEXT_Obj Obj) {
  CPDF_TextObject* pTextObj = Obj.m_pTextObj.Get();

  size_t nContentMarks = pTextObj->m_ContentMarks.CountItems();
  if (nContentMarks == 0)
    return;

  WideString actText;
  for (size_t n = 0; n < nContentMarks; ++n) {
    const CPDF_ContentMarkItem* item = pTextObj->m_ContentMarks.GetItem(n);
    const CPDF_Dictionary* pDict = item->GetParam();
    if (pDict)
      actText = pDict->GetUnicodeTextFor("ActualText");
  }
  if (actText.IsEmpty())
    return;

  CPDF_Font* pFont = pTextObj->GetFont();
  CFX_Matrix matrix = pTextObj->GetTextMatrix() * Obj.m_formMatrix;

  for (size_t k = 0; k < actText.GetLength(); ++k) {
    wchar_t wChar = actText[k];
    if (wChar <= 0x80 && !isprint(wChar))
      wChar = 0x20;
    if (wChar >= 0xFFFD)
      continue;

    PAGECHAR_INFO charinfo;
    charinfo.m_Origin = pTextObj->GetPos();
    charinfo.m_Index = m_TextBuf.GetLength();
    charinfo.m_Unicode = wChar;
    charinfo.m_CharCode = pFont->CharCodeFromUnicode(wChar);
    charinfo.m_Flag = FPDFTEXT_CHAR_PIECE;
    charinfo.m_pTextObj = pTextObj;
    charinfo.m_CharBox = pTextObj->GetRect();
    charinfo.m_Matrix = matrix;
    m_TempTextBuf.AppendChar(wChar);
    m_TempCharList.push_back(charinfo);
  }
}

void CPDF_TextPage::FindPreviousTextObject() {
  const PAGECHAR_INFO* pPrevCharInfo = GetPrevCharInfo();
  if (!pPrevCharInfo)
    return;

  if (pPrevCharInfo->m_pTextObj)
    m_pPreTextObj = pPrevCharInfo->m_pTextObj;
}

void CPDF_TextPage::SwapTempTextBuf(int32_t iCharListStartAppend,
                                    int32_t iBufStartAppend) {
  int32_t i = iCharListStartAppend;
  int32_t j = pdfium::CollectionSize<int32_t>(m_TempCharList) - 1;
  for (; i < j; ++i, --j) {
    std::swap(m_TempCharList[i], m_TempCharList[j]);
    std::swap(m_TempCharList[i].m_Index, m_TempCharList[j].m_Index);
  }
  wchar_t* pTempBuffer = m_TempTextBuf.GetBuffer();
  i = iBufStartAppend;
  j = m_TempTextBuf.GetLength() - 1;
  for (; i < j; ++i, --j)
    std::swap(pTempBuffer[i], pTempBuffer[j]);
}

void CPDF_TextPage::ProcessTextObject(PDFTEXT_Obj Obj) {
  CPDF_TextObject* pTextObj = Obj.m_pTextObj.Get();
  if (fabs(pTextObj->GetRect().Width()) < kSizeEpsilon)
    return;
  CFX_Matrix formMatrix = Obj.m_formMatrix;
  CPDF_Font* pFont = pTextObj->GetFont();
  CFX_Matrix matrix = pTextObj->GetTextMatrix() * formMatrix;

  FPDFText_MarkedContent ePreMKC = PreMarkedContent(Obj);
  if (ePreMKC == FPDFText_MarkedContent::Done) {
    m_pPreTextObj = pTextObj;
    m_perMatrix = formMatrix;
    return;
  }
  GenerateCharacter result = GenerateCharacter::None;
  if (m_pPreTextObj) {
    result = ProcessInsertObject(pTextObj, formMatrix);
    if (result == GenerateCharacter::LineBreak)
      m_CurlineRect = Obj.m_pTextObj->GetRect();
    else
      m_CurlineRect.Union(Obj.m_pTextObj->GetRect());

    switch (result) {
      case GenerateCharacter::None:
        break;
      case GenerateCharacter::Space: {
        Optional<PAGECHAR_INFO> pGenerateChar =
            GenerateCharInfo(TEXT_SPACE_CHAR);
        if (pGenerateChar) {
          if (!formMatrix.IsIdentity())
            pGenerateChar->m_Matrix = formMatrix;
          m_TempTextBuf.AppendChar(TEXT_SPACE_CHAR);
          m_TempCharList.push_back(*pGenerateChar);
        }
        break;
      }
      case GenerateCharacter::LineBreak:
        CloseTempLine();
        if (m_TextBuf.GetSize()) {
          AppendGeneratedCharacter(TEXT_RETURN_CHAR, formMatrix);
          AppendGeneratedCharacter(TEXT_LINEFEED_CHAR, formMatrix);
        }
        break;
      case GenerateCharacter::Hyphen:
        if (pTextObj->CountChars() == 1) {
          CPDF_TextObjectItem item;
          pTextObj->GetCharInfo(0, &item);
          WideString wstrItem =
              pTextObj->GetFont()->UnicodeFromCharCode(item.m_CharCode);
          if (wstrItem.IsEmpty())
            wstrItem += (wchar_t)item.m_CharCode;
          wchar_t curChar = wstrItem[0];
          if (IsHyphenCode(curChar))
            return;
        }
        while (m_TempTextBuf.GetSize() > 0 &&
               m_TempTextBuf.AsStringView()[m_TempTextBuf.GetLength() - 1] ==
                   0x20) {
          m_TempTextBuf.Delete(m_TempTextBuf.GetLength() - 1, 1);
          m_TempCharList.pop_back();
        }
        PAGECHAR_INFO* charinfo = &m_TempCharList.back();
        m_TempTextBuf.Delete(m_TempTextBuf.GetLength() - 1, 1);
        charinfo->m_Unicode = 0x2;
        charinfo->m_Flag = FPDFTEXT_CHAR_HYPHEN;
        m_TempTextBuf.AppendChar(0xfffe);
        break;
    }
  } else {
    m_CurlineRect = Obj.m_pTextObj->GetRect();
  }

  if (ePreMKC == FPDFText_MarkedContent::Delay) {
    ProcessMarkedContent(Obj);
    m_pPreTextObj = pTextObj;
    m_perMatrix = formMatrix;
    return;
  }
  m_pPreTextObj = pTextObj;
  m_perMatrix = formMatrix;
  float baseSpace = CalculateBaseSpace(pTextObj, matrix);

  const bool bR2L = IsRightToLeft(*pTextObj, *pFont);
  const bool bIsBidiAndMirrorInverse =
      bR2L && (matrix.a * matrix.d - matrix.b * matrix.c) < 0;
  int32_t iBufStartAppend = m_TempTextBuf.GetLength();
  int32_t iCharListStartAppend =
      pdfium::CollectionSize<int32_t>(m_TempCharList);

  float spacing = 0;
  const size_t nItems = pTextObj->CountItems();
  for (size_t i = 0; i < nItems; ++i) {
    CPDF_TextObjectItem item;
    PAGECHAR_INFO charinfo;
    pTextObj->GetItemInfo(i, &item);
    if (item.m_CharCode == static_cast<uint32_t>(-1)) {
      WideString str = m_TempTextBuf.MakeString();
      if (str.IsEmpty())
        str = m_TextBuf.AsStringView();
      if (str.IsEmpty() || str[str.GetLength() - 1] == TEXT_SPACE_CHAR)
        continue;

      float fontsize_h = pTextObj->m_TextState.GetFontSizeH();
      spacing = -fontsize_h * item.m_Origin.x / 1000;
      continue;
    }
    float charSpace = pTextObj->m_TextState.GetCharSpace();
    if (charSpace > 0.001)
      spacing += matrix.TransformDistance(charSpace);
    else if (charSpace < -0.001)
      spacing -= matrix.TransformDistance(fabs(charSpace));
    spacing -= baseSpace;
    if (spacing && i > 0) {
      float fontsize_h = pTextObj->m_TextState.GetFontSizeH();
      uint32_t space_charcode = pFont->CharCodeFromUnicode(' ');
      float threshold = 0;
      if (space_charcode != CPDF_Font::kInvalidCharCode)
        threshold = fontsize_h * pFont->GetCharWidthF(space_charcode) / 1000;
      if (threshold > fontsize_h / 3)
        threshold = 0;
      else
        threshold /= 2;
      if (threshold == 0) {
        threshold = static_cast<float>(GetCharWidth(item.m_CharCode, pFont));
        threshold = NormalizeThreshold(threshold, 300, 500, 700);
        threshold = fontsize_h * threshold / 1000;
      }
      if (threshold && (spacing && spacing >= threshold)) {
        charinfo.m_Unicode = TEXT_SPACE_CHAR;
        charinfo.m_Flag = FPDFTEXT_CHAR_GENERATED;
        charinfo.m_pTextObj = pTextObj;
        charinfo.m_Index = m_TextBuf.GetLength();
        m_TempTextBuf.AppendChar(TEXT_SPACE_CHAR);
        charinfo.m_CharCode = CPDF_Font::kInvalidCharCode;
        charinfo.m_Matrix = formMatrix;
        charinfo.m_Origin = matrix.Transform(item.m_Origin);
        charinfo.m_CharBox =
            CFX_FloatRect(charinfo.m_Origin.x, charinfo.m_Origin.y,
                          charinfo.m_Origin.x, charinfo.m_Origin.y);
        m_TempCharList.push_back(charinfo);
      }
      if (item.m_CharCode == CPDF_Font::kInvalidCharCode)
        continue;
    }
    spacing = 0;
    WideString wstrItem = pFont->UnicodeFromCharCode(item.m_CharCode);
    bool bNoUnicode = false;
    if (wstrItem.IsEmpty() && item.m_CharCode) {
      wstrItem += static_cast<wchar_t>(item.m_CharCode);
      bNoUnicode = true;
    }
    charinfo.m_Index = -1;
    charinfo.m_CharCode = item.m_CharCode;
    charinfo.m_Flag =
        bNoUnicode ? FPDFTEXT_CHAR_UNUNICODE : FPDFTEXT_CHAR_NORMAL;
    charinfo.m_pTextObj = pTextObj;
    charinfo.m_Origin = matrix.Transform(item.m_Origin);

    const FX_RECT rect =
        charinfo.m_pTextObj->GetFont()->GetCharBBox(charinfo.m_CharCode);
    const float fFontSize = pTextObj->GetFontSize() / 1000;
    charinfo.m_CharBox.top = rect.top * fFontSize + item.m_Origin.y;
    charinfo.m_CharBox.left = rect.left * fFontSize + item.m_Origin.x;
    charinfo.m_CharBox.right = rect.right * fFontSize + item.m_Origin.x;
    charinfo.m_CharBox.bottom = rect.bottom * fFontSize + item.m_Origin.y;
    if (fabsf(charinfo.m_CharBox.top - charinfo.m_CharBox.bottom) <
        kSizeEpsilon) {
      charinfo.m_CharBox.top =
          charinfo.m_CharBox.bottom + pTextObj->GetFontSize();
    }
    if (fabsf(charinfo.m_CharBox.right - charinfo.m_CharBox.left) <
        kSizeEpsilon) {
      charinfo.m_CharBox.right =
          charinfo.m_CharBox.left + pTextObj->GetCharWidth(charinfo.m_CharCode);
    }
    charinfo.m_CharBox = matrix.TransformRect(charinfo.m_CharBox);
    charinfo.m_Matrix = matrix;
    if (wstrItem.IsEmpty()) {
      charinfo.m_Unicode = 0;
      m_TempCharList.push_back(charinfo);
      m_TempTextBuf.AppendChar(0xfffe);
      continue;
    }
    int nTotal = wstrItem.GetLength();
    bool bDel = false;
    const int count = std::min(pdfium::CollectionSize<int>(m_TempCharList), 7);
    float threshold = charinfo.m_Matrix.TransformXDistance(
        static_cast<float>(TEXT_CHARRATIO_GAPDELTA) * pTextObj->GetFontSize());
    for (int n = pdfium::CollectionSize<int>(m_TempCharList);
         n > pdfium::CollectionSize<int>(m_TempCharList) - count; --n) {
      const PAGECHAR_INFO& charinfo1 = m_TempCharList[n - 1];
      CFX_PointF diff = charinfo1.m_Origin - charinfo.m_Origin;
      if (charinfo1.m_CharCode == charinfo.m_CharCode &&
          charinfo1.m_pTextObj->GetFont() == charinfo.m_pTextObj->GetFont() &&
          fabs(diff.x) < threshold && fabs(diff.y) < threshold) {
        bDel = true;
        break;
      }
    }
    if (!bDel) {
      for (int nIndex = 0; nIndex < nTotal; ++nIndex) {
        charinfo.m_Unicode = wstrItem[nIndex];
        if (charinfo.m_Unicode) {
          charinfo.m_Index = m_TextBuf.GetLength();
          m_TempTextBuf.AppendChar(charinfo.m_Unicode);
        } else {
          m_TempTextBuf.AppendChar(0xfffe);
        }
        m_TempCharList.push_back(charinfo);
      }
    } else if (i == 0) {
      WideString str = m_TempTextBuf.MakeString();
      if (!str.IsEmpty() && str[str.GetLength() - 1] == TEXT_SPACE_CHAR) {
        m_TempTextBuf.Delete(m_TempTextBuf.GetLength() - 1, 1);
        m_TempCharList.pop_back();
      }
    }
  }
  if (bIsBidiAndMirrorInverse)
    SwapTempTextBuf(iCharListStartAppend, iBufStartAppend);
}

CPDF_TextPage::TextOrientation CPDF_TextPage::GetTextObjectWritingMode(
    const CPDF_TextObject* pTextObj) const {
  size_t nChars = pTextObj->CountChars();
  if (nChars <= 1)
    return m_TextlineDir;

  CPDF_TextObjectItem first, last;
  pTextObj->GetCharInfo(0, &first);
  pTextObj->GetCharInfo(nChars - 1, &last);

  CFX_Matrix textMatrix = pTextObj->GetTextMatrix();
  first.m_Origin = textMatrix.Transform(first.m_Origin);
  last.m_Origin = textMatrix.Transform(last.m_Origin);

  static constexpr float kEpsilon = 0.0001f;
  float dX = fabs(last.m_Origin.x - first.m_Origin.x);
  float dY = fabs(last.m_Origin.y - first.m_Origin.y);
  if (dX <= kEpsilon && dY <= kEpsilon)
    return TextOrientation::Unknown;

  static constexpr float kThreshold = 0.0872f;
  CFX_VectorF v(dX, dY);
  v.Normalize();
  bool bXUnderThreshold = v.x <= kThreshold;
  if (v.y <= kThreshold)
    return bXUnderThreshold ? m_TextlineDir : TextOrientation::Horizontal;
  return bXUnderThreshold ? TextOrientation::Vertical : m_TextlineDir;
}

bool CPDF_TextPage::IsHyphen(wchar_t curChar) const {
  WideStringView curText = m_TempTextBuf.AsStringView();
  if (curText.IsEmpty())
    curText = m_TextBuf.AsStringView();

  if (curText.IsEmpty())
    return false;

  auto iter = curText.rbegin();
  for (; (iter + 1) != curText.rend() && *iter == 0x20; ++iter) {
    // Do nothing
  }

  if (!IsHyphenCode(*iter))
    return false;

  if ((iter + 1) != curText.rend()) {
    iter++;
    if (FXSYS_iswalpha(*iter) && FXSYS_iswalnum(curChar))
      return true;
  }

  const PAGECHAR_INFO* pPrevCharInfo = GetPrevCharInfo();
  return pPrevCharInfo && pPrevCharInfo->m_Flag == FPDFTEXT_CHAR_PIECE &&
         IsHyphenCode(pPrevCharInfo->m_Unicode);
}

const PAGECHAR_INFO* CPDF_TextPage::GetPrevCharInfo() const {
  if (!m_TempCharList.empty())
    return &m_TempCharList.back();
  return !m_CharList.empty() ? &m_CharList.back() : nullptr;
}

CPDF_TextPage::GenerateCharacter CPDF_TextPage::ProcessInsertObject(
    const CPDF_TextObject* pObj,
    const CFX_Matrix& formMatrix) {
  FindPreviousTextObject();
  TextOrientation WritingMode = GetTextObjectWritingMode(pObj);
  if (WritingMode == TextOrientation::Unknown)
    WritingMode = GetTextObjectWritingMode(m_pPreTextObj.Get());

  size_t nItem = m_pPreTextObj->CountItems();
  if (nItem == 0)
    return GenerateCharacter::None;

  CPDF_TextObjectItem PrevItem;
  m_pPreTextObj->GetItemInfo(nItem - 1, &PrevItem);

  CPDF_TextObjectItem item;
  pObj->GetItemInfo(0, &item);

  const CFX_FloatRect& this_rect = pObj->GetRect();
  const CFX_FloatRect& prev_rect = m_pPreTextObj->GetRect();

  WideString wstrItem = pObj->GetFont()->UnicodeFromCharCode(item.m_CharCode);
  if (wstrItem.IsEmpty())
    wstrItem += static_cast<wchar_t>(item.m_CharCode);
  wchar_t curChar = wstrItem[0];
  if (WritingMode == TextOrientation::Horizontal) {
    if (EndHorizontalLine(this_rect, prev_rect)) {
      return IsHyphen(curChar) ? GenerateCharacter::Hyphen
                               : GenerateCharacter::LineBreak;
    }
  } else if (WritingMode == TextOrientation::Vertical) {
    if (EndVerticalLine(this_rect, prev_rect, m_CurlineRect,
                        pObj->GetFontSize(), m_pPreTextObj->GetFontSize())) {
      return IsHyphen(curChar) ? GenerateCharacter::Hyphen
                               : GenerateCharacter::LineBreak;
    }
  }

  float last_pos = PrevItem.m_Origin.x;
  uint32_t nLastWidth =
      GetCharWidth(PrevItem.m_CharCode, m_pPreTextObj->GetFont());
  float last_width = nLastWidth * m_pPreTextObj->GetFontSize() / 1000;
  last_width = fabs(last_width);
  uint32_t nThisWidth = GetCharWidth(item.m_CharCode, pObj->GetFont());
  float this_width = fabs(nThisWidth * pObj->GetFontSize() / 1000);
  float threshold = std::max(last_width, this_width) / 4;

  CFX_Matrix prev_matrix = m_pPreTextObj->GetTextMatrix() * m_perMatrix;
  CFX_Matrix prev_reverse = prev_matrix.GetInverse();

  CFX_PointF pos = prev_reverse.Transform(formMatrix.Transform(pObj->GetPos()));
  if (last_width < this_width)
    threshold = prev_reverse.TransformDistance(threshold);

  bool bNewline = false;
  if (WritingMode == TextOrientation::Horizontal) {
    CFX_FloatRect rect = m_pPreTextObj->GetRect();
    float rect_height = rect.Height();
    rect.Normalize();
    if ((rect.IsEmpty() && rect_height > 5) ||
        ((pos.y > threshold * 2 || pos.y < threshold * -3) &&
         (fabs(pos.y) >= 1 || fabs(pos.y) > fabs(pos.x)))) {
      bNewline = true;
      if (nItem > 1) {
        CPDF_TextObjectItem tempItem;
        m_pPreTextObj->GetItemInfo(0, &tempItem);
        CFX_Matrix m = m_pPreTextObj->GetTextMatrix();
        if (PrevItem.m_Origin.x > tempItem.m_Origin.x &&
            m_DisplayMatrix.a > 0.9 && m_DisplayMatrix.b < 0.1 &&
            m_DisplayMatrix.c < 0.1 && m_DisplayMatrix.d < -0.9 && m.b < 0.1 &&
            m.c < 0.1) {
          CFX_FloatRect re(0, m_pPreTextObj->GetRect().bottom, 1000,
                           m_pPreTextObj->GetRect().top);
          if (re.Contains(pObj->GetPos())) {
            bNewline = false;
          } else {
            if (CFX_FloatRect(0, pObj->GetRect().bottom, 1000,
                              pObj->GetRect().top)
                    .Contains(m_pPreTextObj->GetPos())) {
              bNewline = false;
            }
          }
        }
      }
    }
  }
  if (bNewline) {
    return IsHyphen(curChar) ? GenerateCharacter::Hyphen
                             : GenerateCharacter::LineBreak;
  }

  if (pObj->CountChars() == 1 && IsHyphenCode(curChar) && IsHyphen(curChar))
    return GenerateCharacter::Hyphen;

  if (curChar == L' ')
    return GenerateCharacter::None;

  WideString PrevStr =
      m_pPreTextObj->GetFont()->UnicodeFromCharCode(PrevItem.m_CharCode);
  wchar_t preChar = PrevStr.Last();
  if (preChar == L' ')
    return GenerateCharacter::None;

  CFX_Matrix matrix = pObj->GetTextMatrix() * formMatrix;
  float threshold2 = static_cast<float>(std::max(nLastWidth, nThisWidth));
  threshold2 = NormalizeThreshold(threshold2, 400, 700, 800);
  if (nLastWidth >= nThisWidth) {
    threshold2 *= fabs(m_pPreTextObj->GetFontSize());
  } else {
    threshold2 *= fabs(pObj->GetFontSize());
    threshold2 = matrix.TransformDistance(threshold2);
    threshold2 = prev_reverse.TransformDistance(threshold2);
  }
  threshold2 /= 1000;
  if ((threshold2 < 1.4881 && threshold2 > 1.4879) ||
      (threshold2 < 1.39001 && threshold2 > 1.38999)) {
    threshold2 *= 1.5;
  }
  return GenerateSpace(pos, last_pos, this_width, last_width, threshold2)
             ? GenerateCharacter::Space
             : GenerateCharacter::None;
}

bool CPDF_TextPage::IsSameTextObject(CPDF_TextObject* pTextObj1,
                                     CPDF_TextObject* pTextObj2) {
  if (!pTextObj1 || !pTextObj2)
    return false;

  CFX_FloatRect rcPreObj = pTextObj2->GetRect();
  const CFX_FloatRect& rcCurObj = pTextObj1->GetRect();
  if (rcPreObj.IsEmpty() && rcCurObj.IsEmpty()) {
    float dbXdif = fabs(rcPreObj.left - rcCurObj.left);
    size_t nCount = m_CharList.size();
    if (nCount >= 2) {
      PAGECHAR_INFO perCharTemp = m_CharList[nCount - 2];
      float dbSpace = perCharTemp.m_CharBox.Width();
      if (dbXdif > dbSpace)
        return false;
    }
  }
  if (!rcPreObj.IsEmpty() || !rcCurObj.IsEmpty()) {
    rcPreObj.Intersect(rcCurObj);
    if (rcPreObj.IsEmpty())
      return false;
    if (fabs(rcPreObj.Width() - rcCurObj.Width()) > rcCurObj.Width() / 2) {
      return false;
    }
    if (pTextObj2->GetFontSize() != pTextObj1->GetFontSize())
      return false;
  }

  size_t nPreCount = pTextObj2->CountItems();
  if (nPreCount != pTextObj1->CountItems())
    return false;

  // If both objects have no items, consider them same.
  if (nPreCount == 0)
    return true;

  CPDF_TextObjectItem itemPer;
  CPDF_TextObjectItem itemCur;
  for (size_t i = 0; i < nPreCount; ++i) {
    pTextObj2->GetItemInfo(i, &itemPer);
    pTextObj1->GetItemInfo(i, &itemCur);
    if (itemCur.m_CharCode != itemPer.m_CharCode)
      return false;
  }

  CFX_PointF diff = pTextObj1->GetPos() - pTextObj2->GetPos();
  float font_size = pTextObj2->GetFontSize();
  float char_size = GetCharWidth(itemPer.m_CharCode, pTextObj2->GetFont());
  float max_pre_size =
      std::max(std::max(rcPreObj.Height(), rcPreObj.Width()), font_size);
  return fabs(diff.x) <= 0.9 * char_size * font_size / 1000 &&
         fabs(diff.y) <= max_pre_size / 8;
}

bool CPDF_TextPage::IsSameAsPreTextObject(
    CPDF_TextObject* pTextObj,
    const CPDF_PageObjectList* pObjList,
    CPDF_PageObjectList::const_iterator iter) {
  int i = 0;
  while (i < 5 && iter != pObjList->begin()) {
    --iter;
    CPDF_PageObject* pOtherObj = iter->get();
    if (pOtherObj == pTextObj || !pOtherObj->IsText())
      continue;
    if (IsSameTextObject(pOtherObj->AsText(), pTextObj))
      return true;
    ++i;
  }
  return false;
}

Optional<PAGECHAR_INFO> CPDF_TextPage::GenerateCharInfo(wchar_t unicode) {
  const PAGECHAR_INFO* pPrevCharInfo = GetPrevCharInfo();
  if (!pPrevCharInfo)
    return {};

  PAGECHAR_INFO info;
  info.m_Index = m_TextBuf.GetLength();
  info.m_CharCode = CPDF_Font::kInvalidCharCode;
  info.m_Unicode = unicode;
  info.m_Flag = FPDFTEXT_CHAR_GENERATED;

  int preWidth = 0;
  if (pPrevCharInfo->m_pTextObj && pPrevCharInfo->m_CharCode != -1) {
    preWidth = GetCharWidth(pPrevCharInfo->m_CharCode,
                            pPrevCharInfo->m_pTextObj->GetFont());
  }

  float fFontSize = pPrevCharInfo->m_pTextObj
                        ? pPrevCharInfo->m_pTextObj->GetFontSize()
                        : pPrevCharInfo->m_CharBox.Height();
  if (!fFontSize)
    fFontSize = kDefaultFontSize;

  info.m_Origin =
      CFX_PointF(pPrevCharInfo->m_Origin.x + preWidth * (fFontSize) / 1000,
                 pPrevCharInfo->m_Origin.y);
  info.m_CharBox = CFX_FloatRect(info.m_Origin.x, info.m_Origin.y,
                                 info.m_Origin.x, info.m_Origin.y);
  return info;
}
