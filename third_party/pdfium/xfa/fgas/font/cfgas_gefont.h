// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_FONT_CFGAS_GEFONT_H_
#define XFA_FGAS_FONT_CFGAS_GEFONT_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxcrt/retain_ptr.h"
#include "third_party/base/optional.h"
#include "xfa/fgas/font/cfgas_fontmgr.h"

class CFX_Font;
class CFX_UnicodeEncodingEx;

class CFGAS_GEFont final : public Retainable {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  static RetainPtr<CFGAS_GEFont> LoadFont(const wchar_t* pszFontFamily,
                                          uint32_t dwFontStyles,
                                          uint16_t wCodePage,
                                          CFGAS_FontMgr* pFontMgr);
  static RetainPtr<CFGAS_GEFont> LoadFont(CFX_Font* pExternalFont,
                                          CFGAS_FontMgr* pFontMgr);
  static RetainPtr<CFGAS_GEFont> LoadFont(
      std::unique_ptr<CFX_Font> pInternalFont,
      CFGAS_FontMgr* pFontMgr);

  uint32_t GetFontStyles() const;
  bool GetCharWidth(wchar_t wUnicode, int32_t* pWidth);
  int32_t GetGlyphIndex(wchar_t wUnicode);
  int32_t GetAscent() const;
  int32_t GetDescent() const;

  bool GetCharBBox(wchar_t wUnicode, FX_RECT* bbox);
  bool GetBBox(FX_RECT* bbox);

  RetainPtr<CFGAS_GEFont> GetSubstFont(int32_t iGlyphIndex);
  CFX_Font* GetDevFont() const { return m_pFont.Get(); }

  void SetLogicalFontStyle(uint32_t dwLogFontStyle) {
    m_dwLogFontStyle = dwLogFontStyle;
  }

 private:
  explicit CFGAS_GEFont(CFGAS_FontMgr* pFontMgr);
  ~CFGAS_GEFont() override;

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  bool LoadFontInternal(const wchar_t* pszFontFamily,
                        uint32_t dwFontStyles,
                        uint16_t wCodePage);
  bool LoadFontInternal(const uint8_t* pBuffer, int32_t length);
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  bool LoadFontInternal(std::unique_ptr<CFX_Font> pInternalFont);
  bool LoadFontInternal(CFX_Font* pExternalFont);
  bool InitFont();
  std::pair<int32_t, RetainPtr<CFGAS_GEFont>> GetGlyphIndexAndFont(
      wchar_t wUnicode,
      bool bRecursive);
  WideString GetFamilyName() const;

  Optional<uint32_t> m_dwLogFontStyle;
  MaybeOwned<CFX_Font> m_pFont;  // Must come before |m_pFontEncoding|.
  CFGAS_FontMgr::ObservedPtr const m_pFontMgr;
  std::unique_ptr<CFX_UnicodeEncodingEx> m_pFontEncoding;
  std::map<wchar_t, int32_t> m_CharWidthMap;
  std::map<wchar_t, FX_RECT> m_BBoxMap;
  std::vector<RetainPtr<CFGAS_GEFont>> m_SubstFonts;
  std::map<wchar_t, RetainPtr<CFGAS_GEFont>> m_FontMapper;
};

#endif  // XFA_FGAS_FONT_CFGAS_GEFONT_H_
