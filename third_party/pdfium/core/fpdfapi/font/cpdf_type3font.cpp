// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/font/cpdf_type3font.h"

#include <algorithm>
#include <utility>

#include "core/fpdfapi/font/cpdf_type3char.h"
#include "core/fpdfapi/page/cpdf_form.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fxcrt/fx_system.h"
#include "third_party/base/ptr_util.h"

namespace {

constexpr int kMaxType3FormLevel = 4;

}  // namespace

CPDF_Type3Font::CPDF_Type3Font(CPDF_Document* pDocument,
                               CPDF_Dictionary* pFontDict)
    : CPDF_SimpleFont(pDocument, pFontDict) {
  memset(m_CharWidthL, 0, sizeof(m_CharWidthL));
}

CPDF_Type3Font::~CPDF_Type3Font() {}

bool CPDF_Type3Font::IsType3Font() const {
  return true;
}

const CPDF_Type3Font* CPDF_Type3Font::AsType3Font() const {
  return this;
}

CPDF_Type3Font* CPDF_Type3Font::AsType3Font() {
  return this;
}

bool CPDF_Type3Font::Load() {
  m_pFontResources = m_pFontDict->GetDictFor("Resources");
  const CPDF_Array* pMatrix = m_pFontDict->GetArrayFor("FontMatrix");
  float xscale = 1.0f;
  float yscale = 1.0f;
  if (pMatrix) {
    m_FontMatrix = pMatrix->GetMatrix();
    xscale = m_FontMatrix.a;
    yscale = m_FontMatrix.d;
  }

  const CPDF_Array* pBBox = m_pFontDict->GetArrayFor("FontBBox");
  if (pBBox) {
    CFX_FloatRect box(
        pBBox->GetNumberAt(0) * xscale, pBBox->GetNumberAt(1) * yscale,
        pBBox->GetNumberAt(2) * xscale, pBBox->GetNumberAt(3) * yscale);
    CPDF_Type3Char::TextUnitRectToGlyphUnitRect(&box);
    m_FontBBox = box.ToFxRect();
  }

  static constexpr size_t kCharLimit = FX_ArraySize(m_CharWidthL);
  int StartChar = m_pFontDict->GetIntegerFor("FirstChar");
  if (StartChar >= 0 && static_cast<size_t>(StartChar) < kCharLimit) {
    const CPDF_Array* pWidthArray = m_pFontDict->GetArrayFor("Widths");
    if (pWidthArray) {
      size_t count = std::min(pWidthArray->size(), kCharLimit);
      count = std::min(count, kCharLimit - StartChar);
      for (size_t i = 0; i < count; i++) {
        m_CharWidthL[StartChar + i] =
            FXSYS_round(CPDF_Type3Char::TextUnitToGlyphUnit(
                pWidthArray->GetNumberAt(i) * xscale));
      }
    }
  }
  m_pCharProcs = m_pFontDict->GetDictFor("CharProcs");
  if (m_pFontDict->GetDirectObjectFor("Encoding"))
    LoadPDFEncoding(false, false);
  return true;
}

void CPDF_Type3Font::LoadGlyphMap() {}

void CPDF_Type3Font::CheckType3FontMetrics() {
  CheckFontMetrics();
}

CPDF_Type3Char* CPDF_Type3Font::LoadChar(uint32_t charcode) {
  if (m_CharLoadingDepth >= kMaxType3FormLevel)
    return nullptr;

  auto it = m_CacheMap.find(charcode);
  if (it != m_CacheMap.end())
    return it->second.get();

  const char* name = GetAdobeCharName(m_BaseEncoding, m_CharNames, charcode);
  if (!name)
    return nullptr;

  CPDF_Stream* pStream =
      ToStream(m_pCharProcs ? m_pCharProcs->GetDirectObjectFor(name) : nullptr);
  if (!pStream)
    return nullptr;

  auto pNewChar =
      pdfium::MakeUnique<CPDF_Type3Char>(pdfium::MakeUnique<CPDF_Form>(
          m_pDocument.Get(),
          m_pFontResources ? m_pFontResources.Get() : m_pPageResources.Get(),
          pStream, nullptr));

  // This can trigger recursion into this method. The content of |m_CacheMap|
  // can change as a result. Thus after it returns, check the cache again for
  // a cache hit.
  m_CharLoadingDepth++;
  pNewChar->form()->ParseContent(nullptr, nullptr, pNewChar.get(), nullptr);
  m_CharLoadingDepth--;
  it = m_CacheMap.find(charcode);
  if (it != m_CacheMap.end())
    return it->second.get();

  pNewChar->Transform(m_FontMatrix);
  m_CacheMap[charcode] = std::move(pNewChar);
  CPDF_Type3Char* pCachedChar = m_CacheMap[charcode].get();
  if (pCachedChar->form()->GetPageObjectList()->empty())
    pCachedChar->ResetForm();
  return pCachedChar;
}

uint32_t CPDF_Type3Font::GetCharWidthF(uint32_t charcode) {
  if (charcode >= FX_ArraySize(m_CharWidthL))
    charcode = 0;

  if (m_CharWidthL[charcode])
    return m_CharWidthL[charcode];

  const CPDF_Type3Char* pChar = LoadChar(charcode);
  return pChar ? pChar->width() : 0;
}

FX_RECT CPDF_Type3Font::GetCharBBox(uint32_t charcode) {
  FX_RECT ret;
  const CPDF_Type3Char* pChar = LoadChar(charcode);
  if (pChar)
    ret = pChar->bbox();
  return ret;
}
