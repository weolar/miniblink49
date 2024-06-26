// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FONT_CPDF_SIMPLEFONT_H_
#define CORE_FPDFAPI_FONT_CPDF_SIMPLEFONT_H_

#include <vector>

#include "core/fpdfapi/font/cpdf_font.h"
#include "core/fpdfapi/font/cpdf_fontencoding.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"

class CPDF_SimpleFont : public CPDF_Font {
 public:
  CPDF_SimpleFont(CPDF_Document* pDocument, CPDF_Dictionary* pFontDict);
  ~CPDF_SimpleFont() override;

  // CPDF_Font
  uint32_t GetCharWidthF(uint32_t charcode) override;
  FX_RECT GetCharBBox(uint32_t charcode) override;
  int GlyphFromCharCode(uint32_t charcode, bool* pVertGlyph) override;
  bool IsUnicodeCompatible() const override;
  WideString UnicodeFromCharCode(uint32_t charcode) const override;
  uint32_t CharCodeFromUnicode(wchar_t Unicode) const override;

  CPDF_FontEncoding* GetEncoding() { return &m_Encoding; }

  bool HasFontWidths() const override;

 protected:
  virtual void LoadGlyphMap() = 0;

  bool LoadCommon();
  void LoadSubstFont();
  void LoadCharMetrics(int charcode);
  void LoadPDFEncoding(bool bEmbedded, bool bTrueType);

  CPDF_FontEncoding m_Encoding;
  int m_BaseEncoding = PDFFONT_ENCODING_BUILTIN;
  bool m_bUseFontWidth;
  std::vector<ByteString> m_CharNames;
  uint16_t m_GlyphIndex[256];
  uint16_t m_CharWidth[256];
  FX_RECT m_CharBBox[256];
};

#endif  // CORE_FPDFAPI_FONT_CPDF_SIMPLEFONT_H_
