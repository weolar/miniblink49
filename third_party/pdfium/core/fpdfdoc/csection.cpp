// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/csection.h"

#include <algorithm>

#include "core/fpdfdoc/cline.h"
#include "core/fpdfdoc/cpdf_variabletext.h"
#include "core/fpdfdoc/cpvt_wordinfo.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

CSection::CSection(CPDF_VariableText* pVT) : m_pVT(pVT) {
  ASSERT(m_pVT);
}

CSection::~CSection() = default;

void CSection::ResetLinePlace() {
  int32_t i = 0;
  for (auto& pLine : m_LineArray) {
    pLine->LinePlace = CPVT_WordPlace(SecPlace.nSecIndex, i, -1);
    ++i;
  }
}

CPVT_WordPlace CSection::AddWord(const CPVT_WordPlace& place,
                                 const CPVT_WordInfo& wordinfo) {
  int32_t nWordIndex = pdfium::clamp(
      place.nWordIndex, 0, pdfium::CollectionSize<int32_t>(m_WordArray));
  m_WordArray.insert(m_WordArray.begin() + nWordIndex,
                     pdfium::MakeUnique<CPVT_WordInfo>(wordinfo));
  return place;
}

CPVT_WordPlace CSection::AddLine(const CPVT_LineInfo& lineinfo) {
  m_LineArray.push_back(pdfium::MakeUnique<CLine>(lineinfo));
  return CPVT_WordPlace(SecPlace.nSecIndex, m_LineArray.size() - 1, -1);
}

CPVT_FloatRect CSection::Rearrange() {
  if (m_pVT->GetCharArray() > 0)
    return CTypeset(this).CharArray();
  return CTypeset(this).Typeset();
}

CFX_SizeF CSection::GetSectionSize(float fFontSize) {
  return CTypeset(this).GetEditSize(fFontSize);
}

CPVT_WordPlace CSection::GetBeginWordPlace() const {
  if (m_LineArray.empty())
    return SecPlace;
  return m_LineArray.front()->GetBeginWordPlace();
}

CPVT_WordPlace CSection::GetEndWordPlace() const {
  if (m_LineArray.empty())
    return SecPlace;
  return m_LineArray.back()->GetEndWordPlace();
}

CPVT_WordPlace CSection::GetPrevWordPlace(const CPVT_WordPlace& place) const {
  if (place.nLineIndex < 0)
    return GetBeginWordPlace();

  if (place.nLineIndex >= pdfium::CollectionSize<int32_t>(m_LineArray))
    return GetEndWordPlace();

  CLine* pLine = m_LineArray[place.nLineIndex].get();
  if (place.nWordIndex == pLine->m_LineInfo.nBeginWordIndex)
    return CPVT_WordPlace(place.nSecIndex, place.nLineIndex, -1);

  if (place.nWordIndex >= pLine->m_LineInfo.nBeginWordIndex)
    return pLine->GetPrevWordPlace(place);

  if (!pdfium::IndexInBounds(m_LineArray, place.nLineIndex - 1))
    return place;

  return m_LineArray[place.nLineIndex - 1]->GetEndWordPlace();
}

CPVT_WordPlace CSection::GetNextWordPlace(const CPVT_WordPlace& place) const {
  if (place.nLineIndex < 0)
    return GetBeginWordPlace();

  if (place.nLineIndex >= pdfium::CollectionSize<int32_t>(m_LineArray))
    return GetEndWordPlace();

  CLine* pLine = m_LineArray[place.nLineIndex].get();
  if (place.nWordIndex < pLine->m_LineInfo.nEndWordIndex)
    return pLine->GetNextWordPlace(place);

  if (!pdfium::IndexInBounds(m_LineArray, place.nLineIndex + 1))
    return place;

  return m_LineArray[place.nLineIndex + 1]->GetBeginWordPlace();
}

void CSection::UpdateWordPlace(CPVT_WordPlace& place) const {
  int32_t nLeft = 0;
  int32_t nRight = pdfium::CollectionSize<int32_t>(m_LineArray) - 1;
  int32_t nMid = (nLeft + nRight) / 2;
  while (nLeft <= nRight) {
    CLine* pLine = m_LineArray[nMid].get();
    if (place.nWordIndex < pLine->m_LineInfo.nBeginWordIndex) {
      nRight = nMid - 1;
      nMid = (nLeft + nRight) / 2;
    } else if (place.nWordIndex > pLine->m_LineInfo.nEndWordIndex) {
      nLeft = nMid + 1;
      nMid = (nLeft + nRight) / 2;
    } else {
      place.nLineIndex = nMid;
      return;
    }
  }
}

CPVT_WordPlace CSection::SearchWordPlace(const CFX_PointF& point) const {
  CPVT_WordPlace place = GetBeginWordPlace();
  bool bUp = true;
  bool bDown = true;
  int32_t nLeft = 0;
  int32_t nRight = pdfium::CollectionSize<int32_t>(m_LineArray) - 1;
  int32_t nMid = pdfium::CollectionSize<int32_t>(m_LineArray) / 2;
  while (nLeft <= nRight) {
    CLine* pLine = m_LineArray[nMid].get();
    float fTop = pLine->m_LineInfo.fLineY - pLine->m_LineInfo.fLineAscent -
                 m_pVT->GetLineLeading();
    float fBottom = pLine->m_LineInfo.fLineY - pLine->m_LineInfo.fLineDescent;
    if (IsFloatBigger(point.y, fTop))
      bUp = false;
    if (IsFloatSmaller(point.y, fBottom))
      bDown = false;
    if (IsFloatSmaller(point.y, fTop)) {
      nRight = nMid - 1;
      nMid = (nLeft + nRight) / 2;
      continue;
    }
    if (IsFloatBigger(point.y, fBottom)) {
      nLeft = nMid + 1;
      nMid = (nLeft + nRight) / 2;
      continue;
    }
    place = SearchWordPlace(
        point.x,
        CPVT_WordRange(pLine->GetNextWordPlace(pLine->GetBeginWordPlace()),
                       pLine->GetEndWordPlace()));
    place.nLineIndex = nMid;
    return place;
  }
  if (bUp)
    place = GetBeginWordPlace();
  if (bDown)
    place = GetEndWordPlace();
  return place;
}

CPVT_WordPlace CSection::SearchWordPlace(
    float fx,
    const CPVT_WordPlace& lineplace) const {
  if (!pdfium::IndexInBounds(m_LineArray, lineplace.nLineIndex))
    return GetBeginWordPlace();

  CLine* pLine = m_LineArray[lineplace.nLineIndex].get();
  return SearchWordPlace(
      fx - m_Rect.left,
      CPVT_WordRange(pLine->GetNextWordPlace(pLine->GetBeginWordPlace()),
                     pLine->GetEndWordPlace()));
}

CPVT_WordPlace CSection::SearchWordPlace(float fx,
                                         const CPVT_WordRange& range) const {
  CPVT_WordPlace wordplace = range.BeginPos;
  wordplace.nWordIndex = -1;

  int32_t nLeft = range.BeginPos.nWordIndex;
  int32_t nRight = range.EndPos.nWordIndex + 1;
  int32_t nMid = (nLeft + nRight) / 2;
  while (nLeft < nRight) {
    if (nMid == nLeft)
      break;
    if (nMid == nRight) {
      nMid--;
      break;
    }
    if (!pdfium::IndexInBounds(m_WordArray, nMid))
      break;
    CPVT_WordInfo* pWord = m_WordArray[nMid].get();
    if (fx > pWord->fWordX + m_pVT->GetWordWidth(*pWord) * VARIABLETEXT_HALF) {
      nLeft = nMid;
      nMid = (nLeft + nRight) / 2;
      continue;
    }
    nRight = nMid;
    nMid = (nLeft + nRight) / 2;
  }
  if (pdfium::IndexInBounds(m_WordArray, nMid)) {
    CPVT_WordInfo* pWord = m_WordArray[nMid].get();
    if (fx > pWord->fWordX + m_pVT->GetWordWidth(*pWord) * VARIABLETEXT_HALF)
      wordplace.nWordIndex = nMid;
  }
  return wordplace;
}

void CSection::ClearLeftWords(int32_t nWordIndex) {
  for (int32_t i = nWordIndex; i >= 0; i--) {
    if (pdfium::IndexInBounds(m_WordArray, i))
      m_WordArray.erase(m_WordArray.begin() + i);
  }
}

void CSection::ClearRightWords(int32_t nWordIndex) {
  int32_t sz = pdfium::CollectionSize<int32_t>(m_WordArray);
  for (int32_t i = sz - 1; i > nWordIndex; i--) {
    if (pdfium::IndexInBounds(m_WordArray, i))
      m_WordArray.erase(m_WordArray.begin() + i);
  }
}

void CSection::ClearMidWords(int32_t nBeginIndex, int32_t nEndIndex) {
  for (int32_t i = nEndIndex; i > nBeginIndex; i--) {
    if (pdfium::IndexInBounds(m_WordArray, i))
      m_WordArray.erase(m_WordArray.begin() + i);
  }
}

void CSection::ClearWords(const CPVT_WordRange& PlaceRange) {
  CPVT_WordPlace SecBeginPos = GetBeginWordPlace();
  CPVT_WordPlace SecEndPos = GetEndWordPlace();
  if (PlaceRange.BeginPos >= SecBeginPos) {
    if (PlaceRange.EndPos <= SecEndPos) {
      ClearMidWords(PlaceRange.BeginPos.nWordIndex,
                    PlaceRange.EndPos.nWordIndex);
    } else {
      ClearRightWords(PlaceRange.BeginPos.nWordIndex);
    }
  } else if (PlaceRange.EndPos <= SecEndPos) {
    ClearLeftWords(PlaceRange.EndPos.nWordIndex);
  } else {
    m_WordArray.clear();
  }
}

void CSection::ClearWord(const CPVT_WordPlace& place) {
  if (pdfium::IndexInBounds(m_WordArray, place.nWordIndex))
    m_WordArray.erase(m_WordArray.begin() + place.nWordIndex);
}
