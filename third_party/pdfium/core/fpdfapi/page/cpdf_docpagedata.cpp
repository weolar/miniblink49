// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_docpagedata.h"

#include <algorithm>
#include <memory>
#include <set>
#include <utility>

#include "core/fdrm/fx_crypt.h"
#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/font/cpdf_type1font.h"
#include "core/fpdfapi/page/cpdf_iccprofile.h"
#include "core/fpdfapi/page/cpdf_image.h"
#include "core/fpdfapi/page/cpdf_pagemodule.h"
#include "core/fpdfapi/page/cpdf_pattern.h"
#include "core/fpdfapi/page/cpdf_shadingpattern.h"
#include "core/fpdfapi/page/cpdf_tilingpattern.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

CPDF_DocPageData::CPDF_DocPageData(CPDF_Document* pPDFDoc)
    : m_bForceClear(false), m_pPDFDoc(pPDFDoc) {
  ASSERT(m_pPDFDoc);
}

CPDF_DocPageData::~CPDF_DocPageData() {
  Clear(false);
  Clear(true);

  for (auto& it : m_PatternMap)
    delete it.second;
  m_PatternMap.clear();

  for (auto& it : m_FontMap)
    delete it.second;
  m_FontMap.clear();

  for (auto& it : m_ColorSpaceMap)
    delete it.second;
  m_ColorSpaceMap.clear();
}

void CPDF_DocPageData::Clear(bool bForceRelease) {
  m_bForceClear = bForceRelease;

  // This is needed because if |bForceRelease| is true we will destroy any
  // pattern we see regardless of the ref-count. The tiling pattern owns a
  // Form object which owns a ShadingObject. The ShadingObject has an unowned
  // pointer to a ShadingPattern. The ShadingPattern is owned by the
  // DocPageData. So, we loop through and clear any tiling patterns before we
  // do the same for any shading patterns, otherwise we may free the
  // ShadingPattern before the ShadingObject and trigger an unowned pointer
  // probe warning.
  for (auto& it : m_PatternMap) {
    CPDF_CountedPattern* ptData = it.second;
    if (!ptData->get() || !ptData->get()->AsTilingPattern())
      continue;
    if (bForceRelease || ptData->use_count() < 2)
      ptData->clear();
  }

  for (auto& it : m_PatternMap) {
    CPDF_CountedPattern* ptData = it.second;
    if (!ptData->get())
      continue;
    if (bForceRelease || ptData->use_count() < 2)
      ptData->clear();
  }

  for (auto& it : m_FontMap) {
    CPDF_CountedFont* fontData = it.second;
    if (!fontData->get())
      continue;
    if (bForceRelease || fontData->use_count() < 2) {
      fontData->clear();
    }
  }

  for (auto& it : m_ColorSpaceMap) {
    CPDF_CountedColorSpace* csData = it.second;
    if (!csData->get())
      continue;
    if (bForceRelease || csData->use_count() < 2) {
      csData->get()->Release();
      csData->reset(nullptr);
    }
  }

  for (auto it = m_IccProfileMap.begin(); it != m_IccProfileMap.end();) {
    auto curr_it = it++;
    if (bForceRelease || curr_it->second->HasOneRef()) {
      for (auto hash_it = m_HashProfileMap.begin();
           hash_it != m_HashProfileMap.end(); ++hash_it) {
        if (curr_it->first == hash_it->second) {
          m_HashProfileMap.erase(hash_it);
          break;
        }
      }
      m_IccProfileMap.erase(curr_it);
    }
  }

  for (auto it = m_FontFileMap.begin(); it != m_FontFileMap.end();) {
    auto curr_it = it++;
    if (bForceRelease || curr_it->second->HasOneRef())
      m_FontFileMap.erase(curr_it);
  }

  m_ImageMap.clear();
}

CPDF_Font* CPDF_DocPageData::GetFont(CPDF_Dictionary* pFontDict) {
  if (!pFontDict)
    return nullptr;

  CPDF_CountedFont* pFontData = nullptr;
  auto it = m_FontMap.find(pFontDict);
  if (it != m_FontMap.end()) {
    pFontData = it->second;
    if (pFontData->get()) {
      return pFontData->AddRef();
    }
  }
  std::unique_ptr<CPDF_Font> pFont =
      CPDF_Font::Create(m_pPDFDoc.Get(), pFontDict);
  if (!pFont)
    return nullptr;

  if (pFontData) {
    pFontData->reset(std::move(pFont));
  } else {
    pFontData = new CPDF_CountedFont(std::move(pFont));
    m_FontMap[pFontDict] = pFontData;
  }
  return pFontData->AddRef();
}

CPDF_Font* CPDF_DocPageData::GetStandardFont(const ByteString& fontName,
                                             CPDF_FontEncoding* pEncoding) {
  if (fontName.IsEmpty())
    return nullptr;

  for (auto& it : m_FontMap) {
    CPDF_CountedFont* fontData = it.second;
    CPDF_Font* pFont = fontData->get();
    if (!pFont)
      continue;
    if (pFont->GetBaseFont() != fontName)
      continue;
    if (pFont->IsEmbedded())
      continue;
    if (!pFont->IsType1Font())
      continue;
    if (pFont->GetFontDict()->KeyExist("Widths"))
      continue;

    CPDF_Type1Font* pT1Font = pFont->AsType1Font();
    if (pEncoding && !pT1Font->GetEncoding()->IsIdentical(pEncoding))
      continue;

    return fontData->AddRef();
  }

  CPDF_Dictionary* pDict = m_pPDFDoc->NewIndirect<CPDF_Dictionary>();
  pDict->SetNewFor<CPDF_Name>("Type", "Font");
  pDict->SetNewFor<CPDF_Name>("Subtype", "Type1");
  pDict->SetNewFor<CPDF_Name>("BaseFont", fontName);
  if (pEncoding) {
    pDict->SetFor("Encoding",
                  pEncoding->Realize(m_pPDFDoc->GetByteStringPool()));
  }

  std::unique_ptr<CPDF_Font> pFont = CPDF_Font::Create(m_pPDFDoc.Get(), pDict);
  if (!pFont)
    return nullptr;

  CPDF_CountedFont* fontData = new CPDF_CountedFont(std::move(pFont));
  m_FontMap[pDict] = fontData;
  return fontData->AddRef();
}

void CPDF_DocPageData::ReleaseFont(const CPDF_Dictionary* pFontDict) {
  if (!pFontDict)
    return;

  auto it = m_FontMap.find(pFontDict);
  if (it == m_FontMap.end())
    return;

  CPDF_CountedFont* pFontData = it->second;
  if (!pFontData->get())
    return;

  pFontData->RemoveRef();
  if (pFontData->use_count() > 1)
    return;

  // We have font data only in m_FontMap cache. Clean it.
  pFontData->clear();
}

CPDF_ColorSpace* CPDF_DocPageData::GetColorSpace(
    const CPDF_Object* pCSObj,
    const CPDF_Dictionary* pResources) {
  std::set<const CPDF_Object*> visited;
  return GetColorSpaceGuarded(pCSObj, pResources, &visited);
}

CPDF_ColorSpace* CPDF_DocPageData::GetColorSpaceGuarded(
    const CPDF_Object* pCSObj,
    const CPDF_Dictionary* pResources,
    std::set<const CPDF_Object*>* pVisited) {
  std::set<const CPDF_Object*> visitedLocal;
  return GetColorSpaceInternal(pCSObj, pResources, pVisited, &visitedLocal);
}

CPDF_ColorSpace* CPDF_DocPageData::GetColorSpaceInternal(
    const CPDF_Object* pCSObj,
    const CPDF_Dictionary* pResources,
    std::set<const CPDF_Object*>* pVisited,
    std::set<const CPDF_Object*>* pVisitedInternal) {
  if (!pCSObj)
    return nullptr;

  if (pdfium::ContainsKey(*pVisitedInternal, pCSObj))
    return nullptr;

  pdfium::ScopedSetInsertion<const CPDF_Object*> insertion(pVisitedInternal,
                                                           pCSObj);

  if (pCSObj->IsName()) {
    ByteString name = pCSObj->GetString();
    CPDF_ColorSpace* pCS = CPDF_ColorSpace::ColorspaceFromName(name);
    if (!pCS && pResources) {
      const CPDF_Dictionary* pList = pResources->GetDictFor("ColorSpace");
      if (pList) {
        return GetColorSpaceInternal(pList->GetDirectObjectFor(name), nullptr,
                                     pVisited, pVisitedInternal);
      }
    }
    if (!pCS || !pResources)
      return pCS;

    const CPDF_Dictionary* pColorSpaces = pResources->GetDictFor("ColorSpace");
    if (!pColorSpaces)
      return pCS;

    const CPDF_Object* pDefaultCS = nullptr;
    switch (pCS->GetFamily()) {
      case PDFCS_DEVICERGB:
        pDefaultCS = pColorSpaces->GetDirectObjectFor("DefaultRGB");
        break;
      case PDFCS_DEVICEGRAY:
        pDefaultCS = pColorSpaces->GetDirectObjectFor("DefaultGray");
        break;
      case PDFCS_DEVICECMYK:
        pDefaultCS = pColorSpaces->GetDirectObjectFor("DefaultCMYK");
        break;
    }
    if (!pDefaultCS)
      return pCS;

    return GetColorSpaceInternal(pDefaultCS, nullptr, pVisited,
                                 pVisitedInternal);
  }

  const CPDF_Array* pArray = pCSObj->AsArray();
  if (!pArray || pArray->IsEmpty())
    return nullptr;

  if (pArray->size() == 1) {
    return GetColorSpaceInternal(pArray->GetDirectObjectAt(0), pResources,
                                 pVisited, pVisitedInternal);
  }

  CPDF_CountedColorSpace* csData = nullptr;
  auto it = m_ColorSpaceMap.find(pCSObj);
  if (it != m_ColorSpaceMap.end()) {
    csData = it->second;
    if (csData->get()) {
      return csData->AddRef();
    }
  }

  std::unique_ptr<CPDF_ColorSpace> pCS =
      CPDF_ColorSpace::Load(m_pPDFDoc.Get(), pArray, pVisited);
  if (!pCS)
    return nullptr;

  if (csData) {
    csData->reset(std::move(pCS));
  } else {
    csData = new CPDF_CountedColorSpace(std::move(pCS));
    m_ColorSpaceMap[pCSObj] = csData;
  }
  return csData->AddRef();
}

CPDF_ColorSpace* CPDF_DocPageData::GetCopiedColorSpace(
    const CPDF_Object* pCSObj) {
  if (!pCSObj)
    return nullptr;

  auto it = m_ColorSpaceMap.find(pCSObj);
  if (it != m_ColorSpaceMap.end())
    return it->second->AddRef();

  return nullptr;
}

void CPDF_DocPageData::ReleaseColorSpace(const CPDF_Object* pColorSpace) {
  if (!pColorSpace)
    return;

  auto it = m_ColorSpaceMap.find(pColorSpace);
  if (it == m_ColorSpaceMap.end())
    return;

  CPDF_CountedColorSpace* pCountedColorSpace = it->second;
  if (!pCountedColorSpace->get())
    return;

  pCountedColorSpace->RemoveRef();
  if (pCountedColorSpace->use_count() > 1)
    return;

  // We have item only in m_ColorSpaceMap cache. Clean it.
  pCountedColorSpace->get()->Release();
  pCountedColorSpace->reset(nullptr);
}

CPDF_Pattern* CPDF_DocPageData::GetPattern(CPDF_Object* pPatternObj,
                                           bool bShading,
                                           const CFX_Matrix& matrix) {
  if (!pPatternObj)
    return nullptr;

  CPDF_CountedPattern* ptData = nullptr;
  auto it = m_PatternMap.find(pPatternObj);
  if (it != m_PatternMap.end()) {
    ptData = it->second;
    if (ptData->get()) {
      return ptData->AddRef();
    }
  }
  std::unique_ptr<CPDF_Pattern> pPattern;
  if (bShading) {
    pPattern = pdfium::MakeUnique<CPDF_ShadingPattern>(
        m_pPDFDoc.Get(), pPatternObj, true, matrix);
  } else {
    CPDF_Dictionary* pDict = pPatternObj->GetDict();
    if (!pDict)
      return nullptr;

    int type = pDict->GetIntegerFor("PatternType");
    if (type == CPDF_Pattern::TILING) {
      pPattern = pdfium::MakeUnique<CPDF_TilingPattern>(m_pPDFDoc.Get(),
                                                        pPatternObj, matrix);
    } else if (type == CPDF_Pattern::SHADING) {
      pPattern = pdfium::MakeUnique<CPDF_ShadingPattern>(
          m_pPDFDoc.Get(), pPatternObj, false, matrix);
    } else {
      return nullptr;
    }
  }

  if (ptData) {
    ptData->reset(std::move(pPattern));
  } else {
    ptData = new CPDF_CountedPattern(std::move(pPattern));
    m_PatternMap[pPatternObj] = ptData;
  }
  return ptData->AddRef();
}

void CPDF_DocPageData::ReleasePattern(const CPDF_Object* pPatternObj) {
  if (!pPatternObj)
    return;

  auto it = m_PatternMap.find(pPatternObj);
  if (it == m_PatternMap.end())
    return;

  CPDF_CountedPattern* pPattern = it->second;
  if (!pPattern->get())
    return;

  pPattern->RemoveRef();
  if (pPattern->use_count() > 1)
    return;

  // We have item only in m_PatternMap cache. Clean it.
  pPattern->clear();
}

RetainPtr<CPDF_Image> CPDF_DocPageData::GetImage(uint32_t dwStreamObjNum) {
  ASSERT(dwStreamObjNum);
  auto it = m_ImageMap.find(dwStreamObjNum);
  if (it != m_ImageMap.end())
    return it->second;

  auto pImage = pdfium::MakeRetain<CPDF_Image>(m_pPDFDoc.Get(), dwStreamObjNum);
  m_ImageMap[dwStreamObjNum] = pImage;
  return pImage;
}

void CPDF_DocPageData::MaybePurgeImage(uint32_t dwStreamObjNum) {
  ASSERT(dwStreamObjNum);
  auto it = m_ImageMap.find(dwStreamObjNum);
  if (it != m_ImageMap.end() && it->second->HasOneRef())
    m_ImageMap.erase(it);
}

RetainPtr<CPDF_IccProfile> CPDF_DocPageData::GetIccProfile(
    const CPDF_Stream* pProfileStream) {
  if (!pProfileStream)
    return nullptr;

  auto it = m_IccProfileMap.find(pProfileStream);
  if (it != m_IccProfileMap.end())
    return it->second;

  auto pAccessor = pdfium::MakeRetain<CPDF_StreamAcc>(pProfileStream);
  pAccessor->LoadAllDataFiltered();

  uint8_t digest[20];
  CRYPT_SHA1Generate(pAccessor->GetData(), pAccessor->GetSize(), digest);

  ByteString bsDigest(digest, 20);
  auto hash_it = m_HashProfileMap.find(bsDigest);
  if (hash_it != m_HashProfileMap.end()) {
    auto it_copied_stream = m_IccProfileMap.find(hash_it->second);
    if (it_copied_stream != m_IccProfileMap.end())
      return it_copied_stream->second;
  }
  auto pProfile =
      pdfium::MakeRetain<CPDF_IccProfile>(pProfileStream, pAccessor->GetSpan());
  m_IccProfileMap[pProfileStream] = pProfile;
  m_HashProfileMap[bsDigest] = pProfileStream;
  return pProfile;
}

void CPDF_DocPageData::MaybePurgeIccProfile(const CPDF_Stream* pProfileStream) {
  ASSERT(pProfileStream);
  auto it = m_IccProfileMap.find(pProfileStream);
  if (it != m_IccProfileMap.end() && it->second->HasOneRef())
    m_IccProfileMap.erase(it);
}

RetainPtr<CPDF_StreamAcc> CPDF_DocPageData::GetFontFileStreamAcc(
    const CPDF_Stream* pFontStream) {
  ASSERT(pFontStream);
  auto it = m_FontFileMap.find(pFontStream);
  if (it != m_FontFileMap.end())
    return it->second;

  const CPDF_Dictionary* pFontDict = pFontStream->GetDict();
  int32_t org_size = pFontDict->GetIntegerFor("Length1") +
                     pFontDict->GetIntegerFor("Length2") +
                     pFontDict->GetIntegerFor("Length3");
  org_size = std::max(org_size, 0);

  auto pFontAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pFontStream);
  pFontAcc->LoadAllDataFilteredWithEstimatedSize(org_size);
  m_FontFileMap[pFontStream] = pFontAcc;
  return pFontAcc;
}

void CPDF_DocPageData::MaybePurgeFontFileStreamAcc(
    const CPDF_Stream* pFontStream) {
  if (!pFontStream)
    return;

  auto it = m_FontFileMap.find(pFontStream);
  if (it != m_FontFileMap.end() && it->second->HasOneRef())
    m_FontFileMap.erase(it);
}

CPDF_CountedColorSpace* CPDF_DocPageData::FindColorSpacePtr(
    const CPDF_Object* pCSObj) const {
  if (!pCSObj)
    return nullptr;

  auto it = m_ColorSpaceMap.find(pCSObj);
  return it != m_ColorSpaceMap.end() ? it->second : nullptr;
}

CPDF_CountedPattern* CPDF_DocPageData::FindPatternPtr(
    const CPDF_Object* pPatternObj) const {
  if (!pPatternObj)
    return nullptr;

  auto it = m_PatternMap.find(pPatternObj);
  return it != m_PatternMap.end() ? it->second : nullptr;
}
