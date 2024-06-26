// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/font/cfgas_defaultfontmanager.h"

#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fgas/font/fgas_fontutils.h"

// static
RetainPtr<CFGAS_GEFont> CFGAS_DefaultFontManager::GetFont(
    CFGAS_FontMgr* pFontMgr,
    WideStringView wsFontFamily,
    uint32_t dwFontStyles) {
  WideString wsFontName(wsFontFamily);
  RetainPtr<CFGAS_GEFont> pFont =
      pFontMgr->LoadFont(wsFontName.c_str(), dwFontStyles, 0xFFFF);
  if (pFont)
    return pFont;

  const FGAS_FontInfo* pCurFont =
      FGAS_FontInfoByFontName(wsFontName.AsStringView());
  if (!pCurFont || !pCurFont->pReplaceFont)
    return pFont;

  uint32_t dwStyle = 0;
  // TODO(dsinclair): Why doesn't this check the other flags?
  if (FontStyleIsBold(dwFontStyles))
    dwStyle |= FXFONT_BOLD;
  if (FontStyleIsItalic(dwFontStyles))
    dwStyle |= FXFONT_ITALIC;

  const char* pReplace = pCurFont->pReplaceFont;
  int32_t iLength = strlen(pReplace);
  while (iLength > 0) {
    const char* pNameText = pReplace;
    while (*pNameText != ',' && iLength > 0) {
      pNameText++;
      iLength--;
    }
    WideString wsReplace =
        WideString::FromASCII(ByteStringView(pReplace, pNameText - pReplace));
    pFont = pFontMgr->LoadFont(wsReplace.c_str(), dwStyle, 0xFFFF);
    if (pFont)
      break;

    iLength--;
    pNameText++;
    pReplace = pNameText;
  }
  return pFont;
}

// static
RetainPtr<CFGAS_GEFont> CFGAS_DefaultFontManager::GetDefaultFont(
    CFGAS_FontMgr* pFontMgr,
    WideStringView wsFontFamily,
    uint32_t dwFontStyles) {
  RetainPtr<CFGAS_GEFont> pFont =
      pFontMgr->LoadFont(L"Arial Narrow", dwFontStyles, 0xFFFF);
  if (!pFont) {
    pFont = pFontMgr->LoadFont(static_cast<const wchar_t*>(nullptr),
                               dwFontStyles, 0xFFFF);
  }
  return pFont;
}
