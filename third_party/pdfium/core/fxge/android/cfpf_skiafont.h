// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_ANDROID_CFPF_SKIAFONT_H_
#define CORE_FXGE_ANDROID_CFPF_SKIAFONT_H_

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/fx_font.h"

class CFPF_SkiaFontMgr;
class CFPF_SkiaPathFont;

class CFPF_SkiaFont {
 public:
  CFPF_SkiaFont(CFPF_SkiaFontMgr* pFontMgr,
                const CFPF_SkiaPathFont* pFont,
                uint32_t dwStyle,
                uint8_t uCharset);
  ~CFPF_SkiaFont();

  bool IsValid() const { return !!m_Face; }

  ByteString GetFamilyName();
  ByteString GetPsName();
  uint32_t GetFontStyle() const { return m_dwStyle; }
  uint8_t GetCharset() const { return m_uCharset; }
  int32_t GetGlyphIndex(wchar_t wUnicode);
  int32_t GetGlyphWidth(int32_t iGlyphIndex);
  int32_t GetAscent() const;
  int32_t GetDescent() const;
  bool GetGlyphBBox(int32_t iGlyphIndex, FX_RECT& rtBBox);
  bool GetBBox(FX_RECT& rtBBox);
  int32_t GetHeight() const;
  int32_t GetItalicAngle() const;
  uint32_t GetFontData(uint32_t dwTable, uint8_t* pBuffer, uint32_t dwSize);

 private:
  UnownedPtr<CFPF_SkiaFontMgr> const m_pFontMgr;
  UnownedPtr<const CFPF_SkiaPathFont> const m_pFont;
  const FXFT_Face m_Face;
  const uint32_t m_dwStyle;
  const uint8_t m_uCharset;
};

#endif  // CORE_FXGE_ANDROID_CFPF_SKIAFONT_H_
