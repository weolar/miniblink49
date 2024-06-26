// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_FPDF_PARSER_UTILITY_H_
#define CORE_FPDFAPI_PARSER_FPDF_PARSER_UTILITY_H_

#include <ostream>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/retain_ptr.h"
#include "third_party/base/optional.h"

class CPDF_Dictionary;
class CPDF_Object;
class IFX_SeekableReadStream;

// Use the accessors below instead of directly accessing PDF_CharType.
extern const char PDF_CharType[256];

inline bool PDFCharIsWhitespace(uint8_t c) {
  return PDF_CharType[c] == 'W';
}
inline bool PDFCharIsNumeric(uint8_t c) {
  return PDF_CharType[c] == 'N';
}
inline bool PDFCharIsDelimiter(uint8_t c) {
  return PDF_CharType[c] == 'D';
}
inline bool PDFCharIsOther(uint8_t c) {
  return PDF_CharType[c] == 'R';
}

inline bool PDFCharIsLineEnding(uint8_t c) {
  return c == '\r' || c == '\n';
}

// On success, return a positive offset value to the PDF header. If the header
// cannot be found, or if there is an error reading from |pFile|, then return
// nullopt.
Optional<FX_FILESIZE> GetHeaderOffset(
    const RetainPtr<IFX_SeekableReadStream>& pFile);

int32_t GetDirectInteger(const CPDF_Dictionary* pDict, const ByteString& key);

ByteString PDF_NameDecode(ByteStringView orig);
ByteString PDF_NameEncode(const ByteString& orig);

std::ostream& operator<<(std::ostream& buf, const CPDF_Object* pObj);

#endif  // CORE_FPDFAPI_PARSER_FPDF_PARSER_UTILITY_H_
