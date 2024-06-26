// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_color.h"

#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/page/cpdf_patterncs.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fxcrt/fx_system.h"

CPDF_Color::CPDF_Color() = default;

CPDF_Color::CPDF_Color(const CPDF_Color& that) {
  *this = that;
}

CPDF_Color::~CPDF_Color() {
  ReleaseBuffer();
  ReleaseColorSpace();
}

bool CPDF_Color::IsPattern() const {
  return m_pCS && IsPatternInternal();
}

void CPDF_Color::ReleaseBuffer() {
  if (!m_pBuffer)
    return;

  if (IsPatternInternal()) {
    PatternValue* pvalue = reinterpret_cast<PatternValue*>(m_pBuffer);
    CPDF_Pattern* pPattern =
        pvalue->m_pCountedPattern ? pvalue->m_pCountedPattern->get() : nullptr;
    if (pPattern) {
      CPDF_DocPageData* pPageData = pPattern->document()->GetPageData();
      if (pPageData)
        pPageData->ReleasePattern(pPattern->pattern_obj());
    }
  }
  FX_Free(m_pBuffer);
  m_pBuffer = nullptr;
}

void CPDF_Color::ReleaseColorSpace() {
  if (!m_pCS)
    return;

  CPDF_Document* pDoc = m_pCS->GetDocument();
  if (!pDoc)
    return;

  auto* pPageData = pDoc->GetPageData();
  if (pPageData)
    pPageData->ReleaseColorSpace(m_pCS->GetArray());

  m_pCS = nullptr;
}

bool CPDF_Color::IsPatternInternal() const {
  return m_pCS->GetFamily() == PDFCS_PATTERN;
}

void CPDF_Color::SetColorSpace(CPDF_ColorSpace* pCS) {
  ASSERT(pCS);
  if (m_pCS == pCS) {
    if (!m_pBuffer)
      m_pBuffer = pCS->CreateBuf();

    ReleaseColorSpace();
    m_pCS = pCS;
    return;
  }
  ReleaseBuffer();
  ReleaseColorSpace();

  m_pCS = pCS;
  if (IsPatternInternal())
    m_pBuffer = pCS->CreateBuf();
  else
    m_pBuffer = pCS->CreateBufAndSetDefaultColor();
}

void CPDF_Color::SetValueForNonPattern(const std::vector<float>& values) {
  ASSERT(m_pBuffer);
  ASSERT(!IsPatternInternal());
  ASSERT(m_pCS->CountComponents() <= values.size());
  memcpy(m_pBuffer, values.data(), m_pCS->CountComponents() * sizeof(float));
}

void CPDF_Color::SetValueForPattern(CPDF_Pattern* pPattern,
                                    const std::vector<float>& values) {
  if (values.size() > kMaxPatternColorComps)
    return;

  if (!IsPattern()) {
    FX_Free(m_pBuffer);
    m_pCS = CPDF_ColorSpace::GetStockCS(PDFCS_PATTERN);
    m_pBuffer = m_pCS->CreateBuf();
  }

  CPDF_DocPageData* pDocPageData = nullptr;
  PatternValue* pvalue = reinterpret_cast<PatternValue*>(m_pBuffer);
  if (pvalue->m_pPattern) {
    pDocPageData = pvalue->m_pPattern->document()->GetPageData();
    pDocPageData->ReleasePattern(pvalue->m_pPattern->pattern_obj());
  }
  pvalue->m_nComps = values.size();
  pvalue->m_pPattern = pPattern;
  if (!values.empty())
    memcpy(pvalue->m_Comps, values.data(), values.size() * sizeof(float));

  pvalue->m_pCountedPattern = nullptr;
  if (pPattern) {
    if (!pDocPageData)
      pDocPageData = pPattern->document()->GetPageData();

    pvalue->m_pCountedPattern =
        pDocPageData->FindPatternPtr(pPattern->pattern_obj());
  }
}

CPDF_Color& CPDF_Color::operator=(const CPDF_Color& that) {
  if (this == &that)
    return *this;

  ReleaseBuffer();
  ReleaseColorSpace();
  m_pCS = that.m_pCS;
  if (!m_pCS)
    return *this;

  CPDF_Document* pDoc = m_pCS->GetDocument();
  const CPDF_Array* pArray = m_pCS->GetArray();
  if (pDoc && pArray) {
    m_pCS = pDoc->GetPageData()->GetCopiedColorSpace(pArray);
    if (!m_pCS)
      return *this;
  }
  m_pBuffer = m_pCS->CreateBuf();
  memcpy(m_pBuffer, that.m_pBuffer, m_pCS->GetBufSize());
  if (!IsPatternInternal())
    return *this;

  PatternValue* pValue = reinterpret_cast<PatternValue*>(m_pBuffer);
  CPDF_Pattern* pPattern = pValue->m_pPattern;
  if (!pPattern)
    return *this;

  pValue->m_pPattern = pPattern->document()->GetPageData()->GetPattern(
      pPattern->pattern_obj(), false, pPattern->parent_matrix());

  return *this;
}

uint32_t CPDF_Color::CountComponents() const {
  return m_pCS->CountComponents();
}

bool CPDF_Color::IsColorSpaceRGB() const {
  return m_pCS == CPDF_ColorSpace::GetStockCS(PDFCS_DEVICERGB);
}

bool CPDF_Color::GetRGB(int* R, int* G, int* B) const {
  if (!m_pBuffer)
    return false;

  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;
  bool result;
  if (IsPatternInternal()) {
    const CPDF_PatternCS* pPatternCS = m_pCS->AsPatternCS();
    const auto* pValue = reinterpret_cast<const PatternValue*>(m_pBuffer);
    result = pPatternCS->GetPatternRGB(*pValue, &r, &g, &b);
  } else {
    result = m_pCS->GetRGB(m_pBuffer, &r, &g, &b);
  }
  if (!result)
    return false;

  *R = static_cast<int32_t>(r * 255 + 0.5f);
  *G = static_cast<int32_t>(g * 255 + 0.5f);
  *B = static_cast<int32_t>(b * 255 + 0.5f);
  return true;
}

CPDF_Pattern* CPDF_Color::GetPattern() const {
  ASSERT(IsPattern());

  if (!m_pBuffer)
    return nullptr;

  PatternValue* pvalue = reinterpret_cast<PatternValue*>(m_pBuffer);
  return pvalue->m_pPattern;
}
