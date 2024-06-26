// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/font/cfgas_gefont.h"

#include <memory>
#include <utility>

#include "core/fxcrt/fx_codepage.h"
#include "core/fxge/cfx_font.h"
#include "core/fxge/cfx_substfont.h"
#include "core/fxge/cfx_unicodeencodingex.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fgas/font/fgas_fontutils.h"

// static
RetainPtr<CFGAS_GEFont> CFGAS_GEFont::LoadFont(const wchar_t* pszFontFamily,
                                               uint32_t dwFontStyles,
                                               uint16_t wCodePage,
                                               CFGAS_FontMgr* pFontMgr) {
#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
  if (!pFontMgr)
    return nullptr;

  return pFontMgr->GetFontByCodePage(wCodePage, dwFontStyles, pszFontFamily);
#else
  auto pFont = pdfium::MakeRetain<CFGAS_GEFont>(pFontMgr);
  if (!pFont->LoadFontInternal(pszFontFamily, dwFontStyles, wCodePage))
    return nullptr;
  return pFont;
#endif
}

// static
RetainPtr<CFGAS_GEFont> CFGAS_GEFont::LoadFont(CFX_Font* pExternalFont,
                                               CFGAS_FontMgr* pFontMgr) {
  auto pFont = pdfium::MakeRetain<CFGAS_GEFont>(pFontMgr);
  if (!pFont->LoadFontInternal(pExternalFont))
    return nullptr;
  return pFont;
}

// static
RetainPtr<CFGAS_GEFont> CFGAS_GEFont::LoadFont(
    std::unique_ptr<CFX_Font> pInternalFont,
    CFGAS_FontMgr* pFontMgr) {
  auto pFont = pdfium::MakeRetain<CFGAS_GEFont>(pFontMgr);
  if (!pFont->LoadFontInternal(std::move(pInternalFont)))
    return nullptr;
  return pFont;
}

CFGAS_GEFont::CFGAS_GEFont(CFGAS_FontMgr* pFontMgr) : m_pFontMgr(pFontMgr) {}

CFGAS_GEFont::~CFGAS_GEFont() = default;

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
bool CFGAS_GEFont::LoadFontInternal(const wchar_t* pszFontFamily,
                                    uint32_t dwFontStyles,
                                    uint16_t wCodePage) {
  if (m_pFont)
    return false;
  ByteString csFontFamily;
  if (pszFontFamily)
    csFontFamily = WideString(pszFontFamily).ToDefANSI();

  int32_t iWeight =
      FontStyleIsBold(dwFontStyles) ? FXFONT_FW_BOLD : FXFONT_FW_NORMAL;
  m_pFont = pdfium::MakeUnique<CFX_Font>();
  if (FontStyleIsItalic(dwFontStyles) && FontStyleIsBold(dwFontStyles))
    csFontFamily += ",BoldItalic";
  else if (FontStyleIsBold(dwFontStyles))
    csFontFamily += ",Bold";
  else if (FontStyleIsItalic(dwFontStyles))
    csFontFamily += ",Italic";

  m_pFont->LoadSubst(csFontFamily, true, dwFontStyles, iWeight, 0, wCodePage,
                     false);
  return m_pFont->GetFace() && InitFont();
}
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

bool CFGAS_GEFont::LoadFontInternal(CFX_Font* pExternalFont) {
  if (m_pFont || !pExternalFont)
    return false;

  m_pFont = pExternalFont;
  return InitFont();
}

bool CFGAS_GEFont::LoadFontInternal(std::unique_ptr<CFX_Font> pInternalFont) {
  if (m_pFont || !pInternalFont)
    return false;

  m_pFont = std::move(pInternalFont);
  return InitFont();
}

bool CFGAS_GEFont::InitFont() {
  if (!m_pFont)
    return false;

  if (m_pFontEncoding)
    return true;

  m_pFontEncoding = FX_CreateFontEncodingEx(m_pFont.Get());
  return !!m_pFontEncoding;
}

WideString CFGAS_GEFont::GetFamilyName() const {
  if (!m_pFont->GetSubstFont() ||
      m_pFont->GetSubstFont()->m_Family.GetLength() == 0) {
    return WideString::FromDefANSI(m_pFont->GetFamilyName().AsStringView());
  }
  return WideString::FromDefANSI(
      m_pFont->GetSubstFont()->m_Family.AsStringView());
}

uint32_t CFGAS_GEFont::GetFontStyles() const {
  ASSERT(m_pFont);
  if (m_dwLogFontStyle.has_value())
    return m_dwLogFontStyle.value();

  uint32_t dwStyles = 0;
  auto* pSubstFont = m_pFont->GetSubstFont();
  if (pSubstFont) {
    if (pSubstFont->m_Weight == FXFONT_FW_BOLD)
      dwStyles |= FXFONT_BOLD;
    if (pSubstFont->m_bFlagItalic)
      dwStyles |= FXFONT_ITALIC;
  } else {
    if (m_pFont->IsBold())
      dwStyles |= FXFONT_BOLD;
    if (m_pFont->IsItalic())
      dwStyles |= FXFONT_ITALIC;
  }
  return dwStyles;
}

bool CFGAS_GEFont::GetCharWidth(wchar_t wUnicode, int32_t* pWidth) {
  auto it = m_CharWidthMap.find(wUnicode);
  *pWidth = it != m_CharWidthMap.end() ? it->second : 0;
  if (*pWidth == 65535)
    return false;

  if (*pWidth > 0)
    return true;

  RetainPtr<CFGAS_GEFont> pFont;
  int32_t iGlyph;
  std::tie(iGlyph, pFont) = GetGlyphIndexAndFont(wUnicode, true);
  if (iGlyph != 0xFFFF && pFont) {
    if (pFont.Get() == this) {
      *pWidth = m_pFont->GetGlyphWidth(iGlyph);
      if (*pWidth < 0)
        *pWidth = -1;
    } else if (pFont->GetCharWidth(wUnicode, pWidth)) {
      return true;
    }
  } else {
    *pWidth = -1;
  }

  m_CharWidthMap[wUnicode] = *pWidth;
  return *pWidth > 0;
}

bool CFGAS_GEFont::GetCharBBox(wchar_t wUnicode, FX_RECT* bbox) {
  auto it = m_BBoxMap.find(wUnicode);
  if (it != m_BBoxMap.end()) {
    *bbox = it->second;
    return true;
  }

  RetainPtr<CFGAS_GEFont> pFont;
  int32_t iGlyph;
  std::tie(iGlyph, pFont) = GetGlyphIndexAndFont(wUnicode, true);
  if (!pFont || iGlyph == 0xFFFF)
    return false;

  if (pFont.Get() != this)
    return pFont->GetCharBBox(wUnicode, bbox);

  FX_RECT rtBBox;
  if (!m_pFont->GetGlyphBBox(iGlyph, &rtBBox))
    return false;

  m_BBoxMap[wUnicode] = rtBBox;
  *bbox = rtBBox;
  return true;
}

bool CFGAS_GEFont::GetBBox(FX_RECT* bbox) {
  return m_pFont->GetBBox(bbox);
}

int32_t CFGAS_GEFont::GetGlyphIndex(wchar_t wUnicode) {
  int32_t glyph;
  RetainPtr<CFGAS_GEFont> font;
  std::tie(glyph, font) = GetGlyphIndexAndFont(wUnicode, true);
  return glyph;
}

std::pair<int32_t, RetainPtr<CFGAS_GEFont>> CFGAS_GEFont::GetGlyphIndexAndFont(
    wchar_t wUnicode,
    bool bRecursive) {
  int32_t iGlyphIndex = m_pFontEncoding->GlyphFromCharCode(wUnicode);
  if (iGlyphIndex > 0)
    return {iGlyphIndex, RetainPtr<CFGAS_GEFont>(this)};

  const FGAS_FONTUSB* pFontUSB = FGAS_GetUnicodeBitField(wUnicode);
  if (!pFontUSB)
    return {0xFFFF, nullptr};

  uint16_t wBitField = pFontUSB->wBitField;
  if (wBitField >= 128)
    return {0xFFFF, nullptr};

  auto it = m_FontMapper.find(wUnicode);
  if (it != m_FontMapper.end() && it->second && it->second.Get() != this) {
    RetainPtr<CFGAS_GEFont> font;
    std::tie(iGlyphIndex, font) =
        it->second->GetGlyphIndexAndFont(wUnicode, false);
    if (iGlyphIndex != 0xFFFF) {
      for (size_t i = 0; i < m_SubstFonts.size(); ++i) {
        if (m_SubstFonts[i] == it->second)
          return {(iGlyphIndex | ((i + 1) << 24)), it->second};
      }
    }
  }
  if (!m_pFontMgr || !bRecursive)
    return {0xFFFF, nullptr};

  WideString wsFamily = GetFamilyName();
  RetainPtr<CFGAS_GEFont> pFont =
      m_pFontMgr->GetFontByUnicode(wUnicode, GetFontStyles(), wsFamily.c_str());
#if _FX_PLATFORM_ != _FX_PLATFORM_WINDOWS_
  if (!pFont)
    pFont = m_pFontMgr->GetFontByUnicode(wUnicode, GetFontStyles(), nullptr);
#endif
  if (!pFont || pFont.Get() == this)  // Avoids direct cycles below.
    return {0xFFFF, nullptr};

  m_FontMapper[wUnicode] = pFont;
  m_SubstFonts.push_back(pFont);

  RetainPtr<CFGAS_GEFont> font;
  std::tie(iGlyphIndex, font) = pFont->GetGlyphIndexAndFont(wUnicode, false);
  if (iGlyphIndex == 0xFFFF)
    return {0xFFFF, nullptr};

  return {(iGlyphIndex | (m_SubstFonts.size() << 24)), pFont};
}

int32_t CFGAS_GEFont::GetAscent() const {
  return m_pFont->GetAscent();
}

int32_t CFGAS_GEFont::GetDescent() const {
  return m_pFont->GetDescent();
}

RetainPtr<CFGAS_GEFont> CFGAS_GEFont::GetSubstFont(int32_t iGlyphIndex) {
  iGlyphIndex = static_cast<uint32_t>(iGlyphIndex) >> 24;
  if (iGlyphIndex == 0)
    return RetainPtr<CFGAS_GEFont>(this);
  return m_SubstFonts[iGlyphIndex - 1];
}
