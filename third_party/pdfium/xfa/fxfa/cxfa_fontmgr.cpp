// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxfa/cxfa_fontmgr.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fxge/cfx_fontmgr.h"
#include "core/fxge/cfx_gemodule.h"
#include "xfa/fgas/font/cfgas_defaultfontmanager.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fgas/font/fgas_fontutils.h"
#include "xfa/fxfa/cxfa_ffapp.h"
#include "xfa/fxfa/cxfa_ffdoc.h"

CXFA_FontMgr::CXFA_FontMgr() = default;

CXFA_FontMgr::~CXFA_FontMgr() = default;

RetainPtr<CFGAS_GEFont> CXFA_FontMgr::GetFont(CXFA_FFDoc* hDoc,
                                              WideStringView wsFontFamily,
                                              uint32_t dwFontStyles) {
  uint32_t dwHash = FX_HashCode_GetW(wsFontFamily, false);
  ByteString bsKey = ByteString::Format("%u%u%u", dwHash, dwFontStyles, 0xFFFF);
  auto iter = m_FontMap.find(bsKey);
  if (iter != m_FontMap.end())
    return iter->second;

  WideString wsEnglishName = FGAS_FontNameToEnglishName(wsFontFamily);

  CFGAS_PDFFontMgr* pMgr = hDoc->GetPDFFontMgr();
  RetainPtr<CFGAS_GEFont> pFont;
  if (pMgr) {
    pFont = pMgr->GetFont(wsEnglishName.AsStringView(), dwFontStyles, true);
    if (pFont)
      return pFont;
  }
  if (!pFont) {
    pFont = CFGAS_DefaultFontManager::GetFont(hDoc->GetApp()->GetFDEFontMgr(),
                                              wsFontFamily, dwFontStyles);
  }

  if (!pFont && pMgr) {
    pFont = pMgr->GetFont(wsEnglishName.AsStringView(), dwFontStyles, false);
    if (pFont)
      return pFont;
  }

  if (!pFont) {
    pFont = CFGAS_DefaultFontManager::GetDefaultFont(
        hDoc->GetApp()->GetFDEFontMgr(), wsFontFamily, dwFontStyles);
  }

  if (!pFont) {
    ByteString font_family =
        ByteString::Format("%ls", WideString(wsFontFamily).c_str());
    CPDF_Font* stock_font =
        CPDF_Font::GetStockFont(hDoc->GetPDFDoc(), font_family.AsStringView());
    if (stock_font)
      pFont = CFGAS_GEFont::LoadFont(stock_font->GetFont(),
                                     hDoc->GetApp()->GetFDEFontMgr());
  }

  if (pFont)
    m_FontMap[bsKey] = pFont;

  return pFont;
}
