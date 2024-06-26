// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2011 ZXing authors
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

#include "fxbarcode/oned/BC_OnedCodaBarWriter.h"

#include "fxbarcode/BC_Writer.h"
#include "fxbarcode/common/BC_CommonBitMatrix.h"
#include "fxbarcode/oned/BC_OneDimWriter.h"
#include "third_party/base/stl_util.h"

namespace {

const char kOnedCodaAlphabet[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                  '8', '9', '-', '$', ':', '/', '.', '+',
                                  'A', 'B', 'C', 'D', 'T', 'N'};

const int8_t kOnedCodaCharacterEncoding[] = {
    0x03, 0x06, 0x09, 0x60, 0x12, 0x42, 0x21, 0x24, 0x30, 0x48, 0x0c,
    0x18, 0x45, 0x51, 0x54, 0x15, 0x1A, 0x29, 0x0B, 0x0E, 0x1A, 0x29};
static_assert(FX_ArraySize(kOnedCodaCharacterEncoding) == 22, "Wrong size");
static_assert(FX_ArraySize(kOnedCodaCharacterEncoding) ==
                  FX_ArraySize(kOnedCodaAlphabet),
              "Wrong size");

const char kStartEndChars[] = {'A', 'B', 'C', 'D', 'T', 'N', '*', 'E',
                               'a', 'b', 'c', 'd', 't', 'n', 'e'};
const char kCOntentChars[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                              '8', '9', '-', '$', '/', ':', '+', '.'};

}  // namespace

CBC_OnedCodaBarWriter::CBC_OnedCodaBarWriter() = default;

CBC_OnedCodaBarWriter::~CBC_OnedCodaBarWriter() = default;

bool CBC_OnedCodaBarWriter::SetStartChar(char start) {
  if (!pdfium::ContainsValue(kStartEndChars, start))
    return false;

  m_chStart = start;
  return true;
}

bool CBC_OnedCodaBarWriter::SetEndChar(char end) {
  if (!pdfium::ContainsValue(kStartEndChars, end))
    return false;

  m_chEnd = end;
  return true;
}

void CBC_OnedCodaBarWriter::SetDataLength(int32_t length) {
  m_iDataLenth = length + 2;
}

bool CBC_OnedCodaBarWriter::SetTextLocation(BC_TEXT_LOC location) {
  if (location < BC_TEXT_LOC_NONE || location > BC_TEXT_LOC_BELOWEMBED) {
    return false;
  }
  m_locTextLoc = location;
  return true;
}

bool CBC_OnedCodaBarWriter::SetWideNarrowRatio(int8_t ratio) {
  if (ratio < 2 || ratio > 3)
    return false;

  m_iWideNarrRatio = ratio;
  return true;
}

bool CBC_OnedCodaBarWriter::FindChar(wchar_t ch, bool isContent) {
  if (ch > 0x7F)
    return false;

  char narrow_ch = static_cast<char>(ch);
  return pdfium::ContainsValue(kCOntentChars, narrow_ch) ||
         (isContent && pdfium::ContainsValue(kStartEndChars, narrow_ch));
}

bool CBC_OnedCodaBarWriter::CheckContentValidity(WideStringView contents) {
  return std::all_of(
      contents.begin(), contents.end(),
      [this](const wchar_t& ch) { return this->FindChar(ch, false); });
}

WideString CBC_OnedCodaBarWriter::FilterContents(WideStringView contents) {
  WideString filtercontents;
  filtercontents.Reserve(contents.GetLength());
  wchar_t ch;
  for (size_t index = 0; index < contents.GetLength(); index++) {
    ch = contents[index];
    if (ch > 175) {
      index++;
      continue;
    }
    if (!FindChar(ch, true))
      continue;
    filtercontents += ch;
  }
  return filtercontents;
}

uint8_t* CBC_OnedCodaBarWriter::EncodeWithHint(const ByteString& contents,
                                               BCFORMAT format,
                                               int32_t& outWidth,
                                               int32_t& outHeight,
                                               int32_t hints) {
  if (format != BCFORMAT_CODABAR)
    return nullptr;
  return CBC_OneDimWriter::EncodeWithHint(contents, format, outWidth, outHeight,
                                          hints);
}

uint8_t* CBC_OnedCodaBarWriter::EncodeImpl(const ByteString& contents,
                                           int32_t& outLength) {
  ByteString data = m_chStart + contents + m_chEnd;
  m_iContentLen = data.GetLength();
  uint8_t* result = FX_Alloc2D(uint8_t, m_iWideNarrRatio * 7, data.GetLength());
  char ch;
  int32_t position = 0;
  for (size_t index = 0; index < data.GetLength(); index++) {
    ch = data[index];
    if (((ch >= 'a') && (ch <= 'z'))) {
      ch = ch - 32;
    }
    switch (ch) {
      case 'T':
        ch = 'A';
        break;
      case 'N':
        ch = 'B';
        break;
      case '*':
        ch = 'C';
        break;
      case 'E':
        ch = 'D';
        break;
      default:
        break;
    }
    int8_t code = 0;
    for (size_t i = 0; i < FX_ArraySize(kOnedCodaAlphabet); i++) {
      if (ch == kOnedCodaAlphabet[i]) {
        code = kOnedCodaCharacterEncoding[i];
        break;
      }
    }
    uint8_t color = 1;
    int32_t counter = 0;
    int32_t bit = 0;
    while (bit < 7) {
      result[position] = color;
      position++;
      if (((code >> (6 - bit)) & 1) == 0 || counter == m_iWideNarrRatio - 1) {
        color = !color;
        bit++;
        counter = 0;
      } else {
        counter++;
      }
    }
    if (index < data.GetLength() - 1) {
      result[position] = 0;
      position++;
    }
  }
  outLength = position;
  return result;
}

WideString CBC_OnedCodaBarWriter::encodedContents(WideStringView contents) {
  WideString strStart(static_cast<wchar_t>(m_chStart));
  WideString strEnd(static_cast<wchar_t>(m_chEnd));
  return strStart + contents + strEnd;
}

bool CBC_OnedCodaBarWriter::RenderResult(WideStringView contents,
                                         uint8_t* code,
                                         int32_t codeLength) {
  return CBC_OneDimWriter::RenderResult(
      encodedContents(contents).AsStringView(), code, codeLength);
}
