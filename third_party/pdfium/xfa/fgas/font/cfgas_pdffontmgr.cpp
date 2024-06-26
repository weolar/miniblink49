// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/font/cfgas_pdffontmgr.h"

#include <algorithm>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "xfa/fgas/font/cfgas_fontmgr.h"
#include "xfa/fgas/font/cfgas_gefont.h"

namespace {

// The 5 names per entry are: PsName, Normal, Bold, Italic, BoldItalic.
const char* const g_XFAPDFFontName[][5] = {
    {"Adobe PI Std", "AdobePIStd", "AdobePIStd", "AdobePIStd", "AdobePIStd"},
    {"Myriad Pro Light", "MyriadPro-Light", "MyriadPro-Semibold",
     "MyriadPro-LightIt", "MyriadPro-SemiboldIt"},
};

}  // namespace

CFGAS_PDFFontMgr::CFGAS_PDFFontMgr(CPDF_Document* pDoc, CFGAS_FontMgr* pFontMgr)
    : m_pDoc(pDoc), m_pFontMgr(pFontMgr) {
  ASSERT(pDoc);
  ASSERT(pFontMgr);
}

CFGAS_PDFFontMgr::~CFGAS_PDFFontMgr() {}

RetainPtr<CFGAS_GEFont> CFGAS_PDFFontMgr::FindFont(const ByteString& strPsName,
                                                   bool bBold,
                                                   bool bItalic,
                                                   bool bStrictMatch) {
  CPDF_Dictionary* pFontSetDict =
      m_pDoc->GetRoot()->GetDictFor("AcroForm")->GetDictFor("DR");
  if (!pFontSetDict)
    return nullptr;

  pFontSetDict = pFontSetDict->GetDictFor("Font");
  if (!pFontSetDict)
    return nullptr;

  ByteString name = strPsName;
  name.Remove(' ');

  CPDF_DictionaryLocker locker(pFontSetDict);
  for (const auto& it : locker) {
    const ByteString& key = it.first;
    CPDF_Object* pObj = it.second.get();
    if (!PsNameMatchDRFontName(name.AsStringView(), bBold, bItalic, key,
                               bStrictMatch)) {
      continue;
    }
    CPDF_Dictionary* pFontDict = ToDictionary(pObj->GetDirect());
    if (!pFontDict || pFontDict->GetStringFor("Type") != "Font")
      return nullptr;

    CPDF_Font* pPDFFont = m_pDoc->LoadFont(pFontDict);
    if (!pPDFFont)
      return nullptr;

    if (!pPDFFont->IsEmbedded())
      return nullptr;

    return CFGAS_GEFont::LoadFont(pPDFFont->GetFont(), m_pFontMgr.Get());
  }
  return nullptr;
}

RetainPtr<CFGAS_GEFont> CFGAS_PDFFontMgr::GetFont(WideStringView wsFontFamily,
                                                  uint32_t dwFontStyles,
                                                  bool bStrictMatch) {
  uint32_t dwHashCode = FX_HashCode_GetW(wsFontFamily, false);
  ByteString strKey = ByteString::Format("%u%u", dwHashCode, dwFontStyles);
  auto it = m_FontMap.find(strKey);
  if (it != m_FontMap.end())
    return it->second;

  ByteString bsPsName = WideString(wsFontFamily).ToDefANSI();
  bool bBold = FontStyleIsBold(dwFontStyles);
  bool bItalic = FontStyleIsItalic(dwFontStyles);
  ByteString strFontName = PsNameToFontName(bsPsName, bBold, bItalic);
  RetainPtr<CFGAS_GEFont> pFont =
      FindFont(strFontName, bBold, bItalic, bStrictMatch);
  if (pFont)
    m_FontMap[strKey] = pFont;

  return pFont;
}

ByteString CFGAS_PDFFontMgr::PsNameToFontName(const ByteString& strPsName,
                                              bool bBold,
                                              bool bItalic) {
  for (size_t i = 0; i < FX_ArraySize(g_XFAPDFFontName); ++i) {
    if (strPsName == g_XFAPDFFontName[i][0]) {
      size_t index = 1;
      if (bBold)
        ++index;
      if (bItalic)
        index += 2;
      return g_XFAPDFFontName[i][index];
    }
  }
  return strPsName;
}

bool CFGAS_PDFFontMgr::PsNameMatchDRFontName(ByteStringView bsPsName,
                                             bool bBold,
                                             bool bItalic,
                                             const ByteString& bsDRFontName,
                                             bool bStrictMatch) {
  ByteString bsDRName = bsDRFontName;
  bsDRName.Remove('-');
  size_t iPsLen = bsPsName.GetLength();
  auto nIndex = bsDRName.Find(bsPsName);
  if (nIndex.has_value() && !bStrictMatch)
    return true;

  if (!nIndex.has_value() || nIndex.value() != 0)
    return false;

  size_t iDifferLength = bsDRName.GetLength() - iPsLen;
  if (iDifferLength > 1 || (bBold || bItalic)) {
    auto iBoldIndex = bsDRName.Find("Bold");
    if (bBold != iBoldIndex.has_value())
      return false;

    if (iBoldIndex.has_value()) {
      iDifferLength = std::min(iDifferLength - 4,
                               bsDRName.GetLength() - iBoldIndex.value() - 4);
    }
    bool bItalicFont = true;
    if (bsDRName.Contains("Italic"))
      iDifferLength -= 6;
    else if (bsDRName.Contains("It"))
      iDifferLength -= 2;
    else if (bsDRName.Contains("Oblique"))
      iDifferLength -= 7;
    else
      bItalicFont = false;

    if (bItalic != bItalicFont)
      return false;

    if (iDifferLength > 1) {
      ByteString bsDRTailer = bsDRName.Right(iDifferLength);
      if (bsDRTailer == "MT" || bsDRTailer == "PSMT" ||
          bsDRTailer == "Regular" || bsDRTailer == "Reg") {
        return true;
      }
      if (iBoldIndex.has_value() || bItalicFont)
        return false;

      bool bMatch = false;
      switch (bsPsName[iPsLen - 1]) {
        case 'L':
          if (bsDRName.Right(5) == "Light")
            bMatch = true;

          break;
        case 'R':
          if (bsDRName.Right(7) == "Regular" || bsDRName.Right(3) == "Reg")
            bMatch = true;

          break;
        case 'M':
          if (bsDRName.Right(5) == "Medium")
            bMatch = true;
          break;
        default:
          break;
      }
      return bMatch;
    }
  }
  return true;
}
