// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FONT_CPDF_FONTENCODING_H_
#define CORE_FPDFAPI_FONT_CPDF_FONTENCODING_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/string_pool_template.h"
#include "core/fxcrt/weak_ptr.h"

#define PDFFONT_ENCODING_BUILTIN 0
#define PDFFONT_ENCODING_WINANSI 1
#define PDFFONT_ENCODING_MACROMAN 2
#define PDFFONT_ENCODING_MACEXPERT 3
#define PDFFONT_ENCODING_STANDARD 4
#define PDFFONT_ENCODING_ADOBE_SYMBOL 5
#define PDFFONT_ENCODING_ZAPFDINGBATS 6
#define PDFFONT_ENCODING_PDFDOC 7
#define PDFFONT_ENCODING_MS_SYMBOL 8

uint32_t FT_CharCodeFromUnicode(int encoding, wchar_t unicode);
wchar_t FT_UnicodeFromCharCode(int encoding, uint32_t charcode);

wchar_t PDF_UnicodeFromAdobeName(const char* name);
ByteString PDF_AdobeNameFromUnicode(wchar_t unicode);

const uint16_t* PDF_UnicodesForPredefinedCharSet(int encoding);
const char* PDF_CharNameFromPredefinedCharSet(int encoding, uint8_t charcode);

class CPDF_Object;

class CPDF_FontEncoding {
 public:
  static constexpr size_t kEncodingTableSize = 256;

  CPDF_FontEncoding();
  explicit CPDF_FontEncoding(int PredefinedEncoding);

  void LoadEncoding(CPDF_Object* pEncoding);

  bool IsIdentical(CPDF_FontEncoding* pAnother) const;

  wchar_t UnicodeFromCharCode(uint8_t charcode) const {
    return m_Unicodes[charcode];
  }
  int CharCodeFromUnicode(wchar_t unicode) const;

  void SetUnicode(uint8_t charcode, wchar_t unicode) {
    m_Unicodes[charcode] = unicode;
  }

  std::unique_ptr<CPDF_Object> Realize(WeakPtr<ByteStringPool> pPool);

 public:
  wchar_t m_Unicodes[kEncodingTableSize];
};

#endif  // CORE_FPDFAPI_FONT_CPDF_FONTENCODING_H_
