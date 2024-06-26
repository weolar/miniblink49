// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2010 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fxbarcode/oned/BC_OnedCode39Writer.h"

#include <algorithm>
#include <memory>

#include "core/fxcrt/fx_extension.h"
#include "fxbarcode/BC_Writer.h"
#include "fxbarcode/common/BC_CommonBitMatrix.h"
#include "fxbarcode/oned/BC_OneDimWriter.h"

namespace {

const char kOnedCode39Alphabet[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', '-', '.', ' ', '*', '$', '/', '+', '%'};
constexpr size_t kOnedCode39AlphabetLen = FX_ArraySize(kOnedCode39Alphabet);

const char kOnedCode39Checksum[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', '-', '.', ' ', '$', '/', '+', '%'};
static_assert(FX_ArraySize(kOnedCode39Checksum) == 43, "Wrong size");

const int16_t kOnedCode39CharacterEncoding[] = {
    0x0034, 0x0121, 0x0061, 0x0160, 0x0031, 0x0130, 0x0070, 0x0025, 0x0124,
    0x0064, 0x0109, 0x0049, 0x0148, 0x0019, 0x0118, 0x0058, 0x000D, 0x010C,
    0x004C, 0x001C, 0x0103, 0x0043, 0x0142, 0x0013, 0x0112, 0x0052, 0x0007,
    0x0106, 0x0046, 0x0016, 0x0181, 0x00C1, 0x01C0, 0x0091, 0x0190, 0x00D0,
    0x0085, 0x0184, 0x00C4, 0x0094, 0x00A8, 0x00A2, 0x008A, 0x002A};
static_assert(FX_ArraySize(kOnedCode39CharacterEncoding) == 44, "Wrong size");

bool IsInOnedCode39Alphabet(wchar_t ch) {
  return FXSYS_IsDecimalDigit(ch) || (ch >= L'A' && ch <= L'Z') || ch == L'-' ||
         ch == L'.' || ch == L' ' || ch == L'*' || ch == L'$' || ch == L'/' ||
         ch == L'+' || ch == L'%';
}

}  // namespace

CBC_OnedCode39Writer::CBC_OnedCode39Writer() = default;

CBC_OnedCode39Writer::~CBC_OnedCode39Writer() = default;

bool CBC_OnedCode39Writer::CheckContentValidity(WideStringView contents) {
  return std::all_of(contents.begin(), contents.end(), IsInOnedCode39Alphabet);
}

WideString CBC_OnedCode39Writer::FilterContents(WideStringView contents) {
  WideString filtercontents;
  filtercontents.Reserve(contents.GetLength());
  for (size_t i = 0; i < contents.GetLength(); i++) {
    wchar_t ch = contents[i];
    if (ch == L'*' && (i == 0 || i == contents.GetLength() - 1)) {
      continue;
    }
    if (ch > 175) {
      i++;
      continue;
    }
    ch = FXSYS_ToUpperASCII(ch);
    if (IsInOnedCode39Alphabet(ch))
      filtercontents += ch;
  }
  return filtercontents;
}

WideString CBC_OnedCode39Writer::RenderTextContents(WideStringView contents) {
  WideString renderContents;
  for (size_t i = 0; i < contents.GetLength(); i++) {
    wchar_t ch = contents[i];
    if (ch == L'*' && (i == 0 || i == contents.GetLength() - 1)) {
      continue;
    }
    if (ch > 175) {
      i++;
      continue;
    }
    if (IsInOnedCode39Alphabet(FXSYS_ToUpperASCII(ch)))
      renderContents += ch;
  }
  return renderContents;
}

bool CBC_OnedCode39Writer::SetTextLocation(BC_TEXT_LOC location) {
  if (location < BC_TEXT_LOC_NONE || location > BC_TEXT_LOC_BELOWEMBED) {
    return false;
  }
  m_locTextLoc = location;
  return true;
}
bool CBC_OnedCode39Writer::SetWideNarrowRatio(int8_t ratio) {
  if (ratio < 2 || ratio > 3)
    return false;

  m_iWideNarrRatio = ratio;
  return true;
}

uint8_t* CBC_OnedCode39Writer::EncodeWithHint(const ByteString& contents,
                                              BCFORMAT format,
                                              int32_t& outWidth,
                                              int32_t& outHeight,
                                              int32_t hints) {
  if (format != BCFORMAT_CODE_39)
    return nullptr;
  return CBC_OneDimWriter::EncodeWithHint(contents, format, outWidth, outHeight,
                                          hints);
}

void CBC_OnedCode39Writer::ToIntArray(int16_t a, int8_t* toReturn) {
  for (int32_t i = 0; i < 9; i++) {
    toReturn[i] = (a & (1 << i)) == 0 ? 1 : m_iWideNarrRatio;
  }
}

char CBC_OnedCode39Writer::CalcCheckSum(const ByteString& contents) {
  if (contents.GetLength() > 80)
    return '*';

  int32_t checksum = 0;
  for (const auto& c : contents) {
    size_t j = 0;
    for (; j < kOnedCode39AlphabetLen; j++) {
      if (kOnedCode39Alphabet[j] == c) {
        if (c != '*')
          checksum += j;
        break;
      }
    }
    if (j >= kOnedCode39AlphabetLen)
      return '*';
  }
  return kOnedCode39Checksum[checksum % FX_ArraySize(kOnedCode39Checksum)];
}

uint8_t* CBC_OnedCode39Writer::EncodeImpl(const ByteString& contents,
                                          int32_t& outlength) {
  char checksum = CalcCheckSum(contents);
  if (checksum == '*')
    return nullptr;

  int8_t widths[9] = {0};
  int32_t wideStrideNum = 3;
  int32_t narrStrideNum = 9 - wideStrideNum;
  ByteString encodedContents = contents;
  if (m_bCalcChecksum)
    encodedContents += checksum;
  m_iContentLen = encodedContents.GetLength();
  int32_t codeWidth = (wideStrideNum * m_iWideNarrRatio + narrStrideNum) * 2 +
                      1 + m_iContentLen;
  for (size_t j = 0; j < m_iContentLen; j++) {
    for (size_t i = 0; i < kOnedCode39AlphabetLen; i++) {
      if (kOnedCode39Alphabet[i] != encodedContents[j])
        continue;

      ToIntArray(kOnedCode39CharacterEncoding[i], widths);
      for (size_t k = 0; k < 9; k++)
        codeWidth += widths[k];
    }
  }
  outlength = codeWidth;
  std::unique_ptr<uint8_t, FxFreeDeleter> result(FX_Alloc(uint8_t, codeWidth));
  ToIntArray(kOnedCode39CharacterEncoding[39], widths);
  int32_t pos = AppendPattern(result.get(), 0, widths, 9, true);

  int8_t narrowWhite[] = {1};
  pos += AppendPattern(result.get(), pos, narrowWhite, 1, false);

  for (int32_t l = m_iContentLen - 1; l >= 0; l--) {
    for (size_t i = 0; i < kOnedCode39AlphabetLen; i++) {
      if (kOnedCode39Alphabet[i] != encodedContents[l])
        continue;

      ToIntArray(kOnedCode39CharacterEncoding[i], widths);
      pos += AppendPattern(result.get(), pos, widths, 9, true);
    }
    pos += AppendPattern(result.get(), pos, narrowWhite, 1, false);
  }
  ToIntArray(kOnedCode39CharacterEncoding[39], widths);
  pos += AppendPattern(result.get(), pos, widths, 9, true);

  auto* result_ptr = result.get();
  for (int32_t i = 0; i < codeWidth / 2; i++) {
    result_ptr[i] ^= result_ptr[codeWidth - 1 - i];
    result_ptr[codeWidth - 1 - i] ^= result_ptr[i];
    result_ptr[i] ^= result_ptr[codeWidth - 1 - i];
  }
  return result.release();
}

bool CBC_OnedCode39Writer::encodedContents(WideStringView contents,
                                           WideString* result) {
  *result = WideString(contents);
  if (m_bCalcChecksum && m_bPrintChecksum) {
    WideString checksumContent = FilterContents(contents);
    ByteString str = checksumContent.ToUTF8();
    char checksum;
    checksum = CalcCheckSum(str);
    if (checksum == '*')
      return false;
    str += checksum;
    *result += checksum;
  }
  return true;
}

bool CBC_OnedCode39Writer::RenderResult(WideStringView contents,
                                        uint8_t* code,
                                        int32_t codeLength) {
  WideString encodedCon;
  if (!encodedContents(contents, &encodedCon))
    return false;
  return CBC_OneDimWriter::RenderResult(encodedCon.AsStringView(), code,
                                        codeLength);
}
