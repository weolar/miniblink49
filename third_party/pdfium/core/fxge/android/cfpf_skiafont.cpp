// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/android/cfpf_skiafont.h"

#include <algorithm>

#include "core/fxcrt/fx_system.h"
#include "core/fxge/android/cfpf_skiafontmgr.h"
#include "core/fxge/android/cfpf_skiapathfont.h"
#include "core/fxge/fx_freetype.h"

#define FPF_EM_ADJUST(em, a) (em == 0 ? (a) : (a)*1000 / em)

CFPF_SkiaFont::CFPF_SkiaFont(CFPF_SkiaFontMgr* pFontMgr,
                             const CFPF_SkiaPathFont* pFont,
                             uint32_t dwStyle,
                             uint8_t uCharset)
    : m_pFontMgr(pFontMgr),
      m_pFont(pFont),
      m_Face(m_pFontMgr->GetFontFace(m_pFont->path(), m_pFont->face_index())),
      m_dwStyle(dwStyle),
      m_uCharset(uCharset) {}

CFPF_SkiaFont::~CFPF_SkiaFont() {
  if (m_Face)
    FXFT_Done_Face(m_Face);
}

ByteString CFPF_SkiaFont::GetFamilyName() {
  if (!m_Face)
    return ByteString();
  return ByteString(FXFT_Get_Face_Family_Name(m_Face));
}

ByteString CFPF_SkiaFont::GetPsName() {
  if (!m_Face)
    return ByteString();
  return FXFT_Get_Postscript_Name(m_Face);
}

int32_t CFPF_SkiaFont::GetGlyphIndex(wchar_t wUnicode) {
  if (!m_Face)
    return wUnicode;
  if (FXFT_Select_Charmap(m_Face, FXFT_ENCODING_UNICODE))
    return 0;
  return FXFT_Get_Char_Index(m_Face, wUnicode);
}

int32_t CFPF_SkiaFont::GetGlyphWidth(int32_t iGlyphIndex) {
  if (!m_Face)
    return 0;
  if (FXFT_Load_Glyph(
          m_Face, iGlyphIndex,
          FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH)) {
    return 0;
  }
  return FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                       FXFT_Get_Glyph_HoriAdvance(m_Face));
}

int32_t CFPF_SkiaFont::GetAscent() const {
  if (!m_Face)
    return 0;
  return FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                       FXFT_Get_Face_Ascender(m_Face));
}

int32_t CFPF_SkiaFont::GetDescent() const {
  if (!m_Face)
    return 0;
  return FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                       FXFT_Get_Face_Descender(m_Face));
}

bool CFPF_SkiaFont::GetGlyphBBox(int32_t iGlyphIndex, FX_RECT& rtBBox) {
  if (!m_Face)
    return false;
  if (FXFT_Is_Face_Tricky(m_Face)) {
    if (FXFT_Set_Char_Size(m_Face, 0, 1000 * 64, 72, 72))
      return false;
    if (FXFT_Load_Glyph(m_Face, iGlyphIndex,
                        FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH)) {
      FXFT_Set_Pixel_Sizes(m_Face, 0, 64);
      return false;
    }
    FXFT_Glyph glyph;
    if (FXFT_Get_Glyph(m_Face->glyph, &glyph)) {
      FXFT_Set_Pixel_Sizes(m_Face, 0, 64);
      return false;
    }
    FXFT_BBox cbox;
    FXFT_Glyph_Get_CBox(glyph, FXFT_GLYPH_BBOX_PIXELS, &cbox);
    int32_t x_ppem = m_Face->size->metrics.x_ppem;
    int32_t y_ppem = m_Face->size->metrics.y_ppem;
    rtBBox.left = FPF_EM_ADJUST(x_ppem, cbox.xMin);
    rtBBox.right = FPF_EM_ADJUST(x_ppem, cbox.xMax);
    rtBBox.top = FPF_EM_ADJUST(y_ppem, cbox.yMax);
    rtBBox.bottom = FPF_EM_ADJUST(y_ppem, cbox.yMin);
    rtBBox.top = std::min(rtBBox.top, GetAscent());
    rtBBox.bottom = std::max(rtBBox.bottom, GetDescent());
    FXFT_Done_Glyph(glyph);
    return FXFT_Set_Pixel_Sizes(m_Face, 0, 64) == 0;
  }
  if (FXFT_Load_Glyph(
          m_Face, iGlyphIndex,
          FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH)) {
    return false;
  }
  rtBBox.left = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                              FXFT_Get_Glyph_HoriBearingX(m_Face));
  rtBBox.bottom = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                                FXFT_Get_Glyph_HoriBearingY(m_Face));
  rtBBox.right = FPF_EM_ADJUST(
      FXFT_Get_Face_UnitsPerEM(m_Face),
      FXFT_Get_Glyph_HoriBearingX(m_Face) + FXFT_Get_Glyph_Width(m_Face));
  rtBBox.top = FPF_EM_ADJUST(
      FXFT_Get_Face_UnitsPerEM(m_Face),
      FXFT_Get_Glyph_HoriBearingY(m_Face) - FXFT_Get_Glyph_Height(m_Face));
  return true;
}

bool CFPF_SkiaFont::GetBBox(FX_RECT& rtBBox) {
  if (!m_Face) {
    return false;
  }
  rtBBox.left = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                              FXFT_Get_Face_xMin(m_Face));
  rtBBox.top = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                             FXFT_Get_Face_yMin(m_Face));
  rtBBox.right = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                               FXFT_Get_Face_xMax(m_Face));
  rtBBox.bottom = FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                                FXFT_Get_Face_yMax(m_Face));
  return true;
}

int32_t CFPF_SkiaFont::GetHeight() const {
  if (!m_Face)
    return 0;
  return FPF_EM_ADJUST(FXFT_Get_Face_UnitsPerEM(m_Face),
                       FXFT_Get_Face_Height(m_Face));
}

int32_t CFPF_SkiaFont::GetItalicAngle() const {
  if (!m_Face)
    return 0;

  auto* info =
      static_cast<TT_Postscript*>(FT_Get_Sfnt_Table(m_Face, ft_sfnt_post));
  return info ? info->italicAngle : 0;
}

uint32_t CFPF_SkiaFont::GetFontData(uint32_t dwTable,
                                    uint8_t* pBuffer,
                                    uint32_t dwSize) {
  if (!m_Face)
    return 0;

  FT_ULong ulSize = pdfium::base::checked_cast<FT_ULong>(dwSize);
  if (FXFT_Load_Sfnt_Table(m_Face, dwTable, 0, pBuffer, &ulSize))
    return 0;
  return pdfium::base::checked_cast<uint32_t>(ulSize);
}
