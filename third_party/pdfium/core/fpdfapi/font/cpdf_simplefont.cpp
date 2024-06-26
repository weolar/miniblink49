// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/font/cpdf_simplefont.h"

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fxge/fx_freetype.h"
#include "third_party/base/numerics/safe_math.h"

namespace {

void GetPredefinedEncoding(const ByteString& value, int* basemap) {
  if (value == "WinAnsiEncoding")
    *basemap = PDFFONT_ENCODING_WINANSI;
  else if (value == "MacRomanEncoding")
    *basemap = PDFFONT_ENCODING_MACROMAN;
  else if (value == "MacExpertEncoding")
    *basemap = PDFFONT_ENCODING_MACEXPERT;
  else if (value == "PDFDocEncoding")
    *basemap = PDFFONT_ENCODING_PDFDOC;
}

}  // namespace

CPDF_SimpleFont::CPDF_SimpleFont(CPDF_Document* pDocument,
                                 CPDF_Dictionary* pFontDict)
    : CPDF_Font(pDocument, pFontDict) {
  memset(m_CharWidth, 0xff, sizeof(m_CharWidth));
  memset(m_GlyphIndex, 0xff, sizeof(m_GlyphIndex));
  for (size_t i = 0; i < FX_ArraySize(m_CharBBox); ++i)
    m_CharBBox[i] = FX_RECT(-1, -1, -1, -1);
}

CPDF_SimpleFont::~CPDF_SimpleFont() = default;

int CPDF_SimpleFont::GlyphFromCharCode(uint32_t charcode, bool* pVertGlyph) {
  if (pVertGlyph)
    *pVertGlyph = false;

  if (charcode > 0xff)
    return -1;

  int index = m_GlyphIndex[charcode];
  if (index == 0xffff || (index == 0 && IsTrueTypeFont()))
    return -1;

  return index;
}

void CPDF_SimpleFont::LoadCharMetrics(int charcode) {
  if (!m_Font.GetFace())
    return;

  if (charcode < 0 || charcode > 0xff) {
    return;
  }
  int glyph_index = m_GlyphIndex[charcode];
  if (glyph_index == 0xffff) {
    if (!m_pFontFile && charcode != 32) {
      LoadCharMetrics(32);
      m_CharBBox[charcode] = m_CharBBox[32];
      if (m_bUseFontWidth) {
        m_CharWidth[charcode] = m_CharWidth[32];
      }
    }
    return;
  }
  FXFT_Face face = m_Font.GetFace();
  int err = FXFT_Load_Glyph(
      face, glyph_index,
      FXFT_LOAD_NO_SCALE | FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH);
  if (err)
    return;

  int iHoriBearingX = FXFT_Get_Glyph_HoriBearingX(face);
  int iHoriBearingY = FXFT_Get_Glyph_HoriBearingY(face);
  m_CharBBox[charcode] =
      FX_RECT(TT2PDF(iHoriBearingX, face), TT2PDF(iHoriBearingY, face),
              TT2PDF(iHoriBearingX + FXFT_Get_Glyph_Width(face), face),
              TT2PDF(iHoriBearingY - FXFT_Get_Glyph_Height(face), face));

  if (m_bUseFontWidth) {
    int TT_Width = TT2PDF(FXFT_Get_Glyph_HoriAdvance(face), face);
    if (m_CharWidth[charcode] == 0xffff) {
      m_CharWidth[charcode] = TT_Width;
    } else if (TT_Width && !IsEmbedded()) {
      m_CharBBox[charcode].right =
          m_CharBBox[charcode].right * m_CharWidth[charcode] / TT_Width;
      m_CharBBox[charcode].left =
          m_CharBBox[charcode].left * m_CharWidth[charcode] / TT_Width;
    }
  }
}

void CPDF_SimpleFont::LoadPDFEncoding(bool bEmbedded, bool bTrueType) {
  const CPDF_Object* pEncoding = m_pFontDict->GetDirectObjectFor("Encoding");
  if (!pEncoding) {
    if (m_BaseFont == "Symbol") {
      m_BaseEncoding = bTrueType ? PDFFONT_ENCODING_MS_SYMBOL
                                 : PDFFONT_ENCODING_ADOBE_SYMBOL;
    } else if (!bEmbedded && m_BaseEncoding == PDFFONT_ENCODING_BUILTIN) {
      m_BaseEncoding = PDFFONT_ENCODING_WINANSI;
    }
    return;
  }
  if (pEncoding->IsName()) {
    if (m_BaseEncoding == PDFFONT_ENCODING_ADOBE_SYMBOL ||
        m_BaseEncoding == PDFFONT_ENCODING_ZAPFDINGBATS) {
      return;
    }
    if (FontStyleIsSymbolic(m_Flags) && m_BaseFont == "Symbol") {
      if (!bTrueType)
        m_BaseEncoding = PDFFONT_ENCODING_ADOBE_SYMBOL;
      return;
    }
    ByteString bsEncoding = pEncoding->GetString();
    if (bsEncoding.Compare("MacExpertEncoding") == 0) {
      bsEncoding = "WinAnsiEncoding";
    }
    GetPredefinedEncoding(bsEncoding, &m_BaseEncoding);
    return;
  }

  const CPDF_Dictionary* pDict = pEncoding->AsDictionary();
  if (!pDict)
    return;

  if (m_BaseEncoding != PDFFONT_ENCODING_ADOBE_SYMBOL &&
      m_BaseEncoding != PDFFONT_ENCODING_ZAPFDINGBATS) {
    ByteString bsEncoding = pDict->GetStringFor("BaseEncoding");
    if (bTrueType && bsEncoding.Compare("MacExpertEncoding") == 0)
      bsEncoding = "WinAnsiEncoding";
    GetPredefinedEncoding(bsEncoding, &m_BaseEncoding);
  }
  if ((!bEmbedded || bTrueType) && m_BaseEncoding == PDFFONT_ENCODING_BUILTIN)
    m_BaseEncoding = PDFFONT_ENCODING_STANDARD;

  const CPDF_Array* pDiffs = pDict->GetArrayFor("Differences");
  if (!pDiffs)
    return;

  m_CharNames.resize(256);
  uint32_t cur_code = 0;
  for (uint32_t i = 0; i < pDiffs->size(); i++) {
    const CPDF_Object* pElement = pDiffs->GetDirectObjectAt(i);
    if (!pElement)
      continue;

    const CPDF_Name* pName = pElement->AsName();
    if (pName) {
      if (cur_code < 256)
        m_CharNames[cur_code] = pName->GetString();
      cur_code++;
    } else {
      cur_code = pElement->GetInteger();
    }
  }
}

uint32_t CPDF_SimpleFont::GetCharWidthF(uint32_t charcode) {
  if (charcode > 0xff)
    charcode = 0;

  if (m_CharWidth[charcode] == 0xffff) {
    LoadCharMetrics(charcode);
    if (m_CharWidth[charcode] == 0xffff) {
      m_CharWidth[charcode] = 0;
    }
  }
  return m_CharWidth[charcode];
}

FX_RECT CPDF_SimpleFont::GetCharBBox(uint32_t charcode) {
  if (charcode > 0xff)
    charcode = 0;

  if (m_CharBBox[charcode].left == -1)
    LoadCharMetrics(charcode);

  return m_CharBBox[charcode];
}

bool CPDF_SimpleFont::LoadCommon() {
  const CPDF_Dictionary* pFontDesc = m_pFontDict->GetDictFor("FontDescriptor");
  if (pFontDesc) {
    LoadFontDescriptor(pFontDesc);
  }
  const CPDF_Array* pWidthArray = m_pFontDict->GetArrayFor("Widths");
  m_bUseFontWidth = !pWidthArray;
  if (pWidthArray) {
    if (pFontDesc && pFontDesc->KeyExist("MissingWidth")) {
      int MissingWidth = pFontDesc->GetIntegerFor("MissingWidth");
      for (int i = 0; i < 256; i++) {
        m_CharWidth[i] = MissingWidth;
      }
    }
    size_t width_start = m_pFontDict->GetIntegerFor("FirstChar", 0);
    size_t width_end = m_pFontDict->GetIntegerFor("LastChar", 0);
    if (width_start <= 255) {
      if (width_end == 0 || width_end >= width_start + pWidthArray->size())
        width_end = width_start + pWidthArray->size() - 1;
      if (width_end > 255)
        width_end = 255;
      for (size_t i = width_start; i <= width_end; i++)
        m_CharWidth[i] = pWidthArray->GetIntegerAt(i - width_start);
    }
  }
  if (m_pFontFile) {
    if (m_BaseFont.GetLength() > 8 && m_BaseFont[7] == '+')
      m_BaseFont = m_BaseFont.Right(m_BaseFont.GetLength() - 8);
  } else {
    LoadSubstFont();
  }
  if (!FontStyleIsSymbolic(m_Flags))
    m_BaseEncoding = PDFFONT_ENCODING_STANDARD;
  LoadPDFEncoding(!!m_pFontFile, m_Font.IsTTFont());
  LoadGlyphMap();
  m_CharNames.clear();
  if (!m_Font.GetFace())
    return true;

  if (FontStyleIsAllCaps(m_Flags)) {
    static const unsigned char kLowercases[][2] = {
        {'a', 'z'}, {0xe0, 0xf6}, {0xf8, 0xfd}};
    for (size_t range = 0; range < FX_ArraySize(kLowercases); ++range) {
      const auto& lower = kLowercases[range];
      for (int i = lower[0]; i <= lower[1]; ++i) {
        if (m_GlyphIndex[i] != 0xffff && m_pFontFile)
          continue;

        int j = i - 32;
        m_GlyphIndex[i] = m_GlyphIndex[j];
        if (m_CharWidth[j]) {
          m_CharWidth[i] = m_CharWidth[j];
          m_CharBBox[i] = m_CharBBox[j];
        }
      }
    }
  }
  CheckFontMetrics();
  return true;
}

void CPDF_SimpleFont::LoadSubstFont() {
  if (!m_bUseFontWidth && !FontStyleIsFixedPitch(m_Flags)) {
    int width = 0, i;
    for (i = 0; i < 256; i++) {
      if (m_CharWidth[i] == 0 || m_CharWidth[i] == 0xffff)
        continue;

      if (width == 0)
        width = m_CharWidth[i];
      else if (width != m_CharWidth[i])
        break;
    }
    if (i == 256 && width)
      m_Flags |= FXFONT_FIXED_PITCH;
  }
  pdfium::base::CheckedNumeric<int> safeStemV(m_StemV);
  if (m_StemV < 140)
    safeStemV *= 5;
  else
    safeStemV = safeStemV * 4 + 140;
  m_Font.LoadSubst(m_BaseFont, IsTrueTypeFont(), m_Flags,
                   safeStemV.ValueOrDefault(FXFONT_FW_NORMAL), m_ItalicAngle, 0,
                   false);
}

bool CPDF_SimpleFont::IsUnicodeCompatible() const {
  return m_BaseEncoding != PDFFONT_ENCODING_BUILTIN &&
         m_BaseEncoding != PDFFONT_ENCODING_ADOBE_SYMBOL &&
         m_BaseEncoding != PDFFONT_ENCODING_ZAPFDINGBATS;
}

WideString CPDF_SimpleFont::UnicodeFromCharCode(uint32_t charcode) const {
  WideString unicode = CPDF_Font::UnicodeFromCharCode(charcode);
  if (!unicode.IsEmpty())
    return unicode;
  wchar_t ret = m_Encoding.UnicodeFromCharCode((uint8_t)charcode);
  if (ret == 0)
    return WideString();
  return ret;
}

uint32_t CPDF_SimpleFont::CharCodeFromUnicode(wchar_t unicode) const {
  uint32_t ret = CPDF_Font::CharCodeFromUnicode(unicode);
  if (ret)
    return ret;
  return m_Encoding.CharCodeFromUnicode(unicode);
}

bool CPDF_SimpleFont::HasFontWidths() const {
  return !m_bUseFontWidth;
}
