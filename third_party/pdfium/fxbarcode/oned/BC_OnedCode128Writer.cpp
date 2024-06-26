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

#include "fxbarcode/oned/BC_OnedCode128Writer.h"

#include <cctype>
#include <memory>

#include "fxbarcode/BC_Writer.h"
#include "fxbarcode/oned/BC_OneDimWriter.h"

namespace {

constexpr size_t kPatternSize = 7;

const int8_t CODE_PATTERNS[107][kPatternSize] = {
    {2, 1, 2, 2, 2, 2, 0}, {2, 2, 2, 1, 2, 2, 0}, {2, 2, 2, 2, 2, 1, 0},
    {1, 2, 1, 2, 2, 3, 0}, {1, 2, 1, 3, 2, 2, 0}, {1, 3, 1, 2, 2, 2, 0},
    {1, 2, 2, 2, 1, 3, 0}, {1, 2, 2, 3, 1, 2, 0}, {1, 3, 2, 2, 1, 2, 0},
    {2, 2, 1, 2, 1, 3, 0}, {2, 2, 1, 3, 1, 2, 0}, {2, 3, 1, 2, 1, 2, 0},
    {1, 1, 2, 2, 3, 2, 0}, {1, 2, 2, 1, 3, 2, 0}, {1, 2, 2, 2, 3, 1, 0},
    {1, 1, 3, 2, 2, 2, 0}, {1, 2, 3, 1, 2, 2, 0}, {1, 2, 3, 2, 2, 1, 0},
    {2, 2, 3, 2, 1, 1, 0}, {2, 2, 1, 1, 3, 2, 0}, {2, 2, 1, 2, 3, 1, 0},
    {2, 1, 3, 2, 1, 2, 0}, {2, 2, 3, 1, 1, 2, 0}, {3, 1, 2, 1, 3, 1, 0},
    {3, 1, 1, 2, 2, 2, 0}, {3, 2, 1, 1, 2, 2, 0}, {3, 2, 1, 2, 2, 1, 0},
    {3, 1, 2, 2, 1, 2, 0}, {3, 2, 2, 1, 1, 2, 0}, {3, 2, 2, 2, 1, 1, 0},
    {2, 1, 2, 1, 2, 3, 0}, {2, 1, 2, 3, 2, 1, 0}, {2, 3, 2, 1, 2, 1, 0},
    {1, 1, 1, 3, 2, 3, 0}, {1, 3, 1, 1, 2, 3, 0}, {1, 3, 1, 3, 2, 1, 0},
    {1, 1, 2, 3, 1, 3, 0}, {1, 3, 2, 1, 1, 3, 0}, {1, 3, 2, 3, 1, 1, 0},
    {2, 1, 1, 3, 1, 3, 0}, {2, 3, 1, 1, 1, 3, 0}, {2, 3, 1, 3, 1, 1, 0},
    {1, 1, 2, 1, 3, 3, 0}, {1, 1, 2, 3, 3, 1, 0}, {1, 3, 2, 1, 3, 1, 0},
    {1, 1, 3, 1, 2, 3, 0}, {1, 1, 3, 3, 2, 1, 0}, {1, 3, 3, 1, 2, 1, 0},
    {3, 1, 3, 1, 2, 1, 0}, {2, 1, 1, 3, 3, 1, 0}, {2, 3, 1, 1, 3, 1, 0},
    {2, 1, 3, 1, 1, 3, 0}, {2, 1, 3, 3, 1, 1, 0}, {2, 1, 3, 1, 3, 1, 0},
    {3, 1, 1, 1, 2, 3, 0}, {3, 1, 1, 3, 2, 1, 0}, {3, 3, 1, 1, 2, 1, 0},
    {3, 1, 2, 1, 1, 3, 0}, {3, 1, 2, 3, 1, 1, 0}, {3, 3, 2, 1, 1, 1, 0},
    {3, 1, 4, 1, 1, 1, 0}, {2, 2, 1, 4, 1, 1, 0}, {4, 3, 1, 1, 1, 1, 0},
    {1, 1, 1, 2, 2, 4, 0}, {1, 1, 1, 4, 2, 2, 0}, {1, 2, 1, 1, 2, 4, 0},
    {1, 2, 1, 4, 2, 1, 0}, {1, 4, 1, 1, 2, 2, 0}, {1, 4, 1, 2, 2, 1, 0},
    {1, 1, 2, 2, 1, 4, 0}, {1, 1, 2, 4, 1, 2, 0}, {1, 2, 2, 1, 1, 4, 0},
    {1, 2, 2, 4, 1, 1, 0}, {1, 4, 2, 1, 1, 2, 0}, {1, 4, 2, 2, 1, 1, 0},
    {2, 4, 1, 2, 1, 1, 0}, {2, 2, 1, 1, 1, 4, 0}, {4, 1, 3, 1, 1, 1, 0},
    {2, 4, 1, 1, 1, 2, 0}, {1, 3, 4, 1, 1, 1, 0}, {1, 1, 1, 2, 4, 2, 0},
    {1, 2, 1, 1, 4, 2, 0}, {1, 2, 1, 2, 4, 1, 0}, {1, 1, 4, 2, 1, 2, 0},
    {1, 2, 4, 1, 1, 2, 0}, {1, 2, 4, 2, 1, 1, 0}, {4, 1, 1, 2, 1, 2, 0},
    {4, 2, 1, 1, 1, 2, 0}, {4, 2, 1, 2, 1, 1, 0}, {2, 1, 2, 1, 4, 1, 0},
    {2, 1, 4, 1, 2, 1, 0}, {4, 1, 2, 1, 2, 1, 0}, {1, 1, 1, 1, 4, 3, 0},
    {1, 1, 1, 3, 4, 1, 0}, {1, 3, 1, 1, 4, 1, 0}, {1, 1, 4, 1, 1, 3, 0},
    {1, 1, 4, 3, 1, 1, 0}, {4, 1, 1, 1, 1, 3, 0}, {4, 1, 1, 3, 1, 1, 0},
    {1, 1, 3, 1, 4, 1, 0}, {1, 1, 4, 1, 3, 1, 0}, {3, 1, 1, 1, 4, 1, 0},
    {4, 1, 1, 1, 3, 1, 0}, {2, 1, 1, 4, 1, 2, 0}, {2, 1, 1, 2, 1, 4, 0},
    {2, 1, 1, 2, 3, 2, 0}, {2, 3, 3, 1, 1, 1, 2}};

const int32_t CODE_START_B = 104;
const int32_t CODE_START_C = 105;
const int32_t CODE_STOP = 106;

}  // namespace

CBC_OnedCode128Writer::CBC_OnedCode128Writer(BC_TYPE type)
    : m_codeFormat(type) {
  ASSERT(m_codeFormat == BC_CODE128_B || m_codeFormat == BC_CODE128_C);
}

CBC_OnedCode128Writer::~CBC_OnedCode128Writer() = default;

bool CBC_OnedCode128Writer::CheckContentValidity(WideStringView contents) {
  for (const auto& ch : contents) {
    int32_t patternIndex = static_cast<int32_t>(ch);
    if (patternIndex < 32 || patternIndex > 126 || patternIndex == 34)
      return false;
  }
  return true;
}

WideString CBC_OnedCode128Writer::FilterContents(WideStringView contents) {
  const wchar_t limit = m_codeFormat == BC_CODE128_B ? 126 : 106;

  WideString filtered;
  filtered.Reserve(contents.GetLength());
  for (size_t i = 0; i < contents.GetLength(); i++) {
    wchar_t ch = contents[i];
    if (ch > 175) {
      i++;
      continue;
    }
    if (ch >= 32 && ch <= limit)
      filtered += ch;
  }
  return filtered;
}

bool CBC_OnedCode128Writer::SetTextLocation(BC_TEXT_LOC location) {
  if (location < BC_TEXT_LOC_NONE || location > BC_TEXT_LOC_BELOWEMBED) {
    return false;
  }
  m_locTextLoc = location;
  return true;
}

uint8_t* CBC_OnedCode128Writer::EncodeWithHint(const ByteString& contents,
                                               BCFORMAT format,
                                               int32_t& outWidth,
                                               int32_t& outHeight,
                                               int32_t hints) {
  if (format != BCFORMAT_CODE_128)
    return nullptr;
  return CBC_OneDimWriter::EncodeWithHint(contents, format, outWidth, outHeight,
                                          hints);
}

uint8_t* CBC_OnedCode128Writer::EncodeImpl(const ByteString& contents,
                                           int32_t& outLength) {
  if (contents.GetLength() < 1 || contents.GetLength() > 80)
    return nullptr;

  std::vector<int32_t> patterns;
  int32_t checkSum = 0;
  if (m_codeFormat == BC_CODE128_B)
    checkSum = Encode128B(contents, &patterns);
  else
    checkSum = Encode128C(contents, &patterns);

  checkSum %= 103;
  patterns.push_back(checkSum);
  patterns.push_back(CODE_STOP);
  m_iContentLen = contents.GetLength() + 3;
  int32_t codeWidth = 0;
  for (const auto& patternIndex : patterns) {
    const int8_t* pattern = CODE_PATTERNS[patternIndex];
    for (size_t i = 0; i < kPatternSize; ++i)
      codeWidth += pattern[i];
  }
  outLength = codeWidth;
  std::unique_ptr<uint8_t, FxFreeDeleter> result(FX_Alloc(uint8_t, outLength));
  int32_t pos = 0;
  for (size_t i = 0; i < patterns.size(); ++i) {
    const int8_t* pattern = CODE_PATTERNS[patterns[i]];
    pos += AppendPattern(result.get(), pos, pattern, kPatternSize, true);
  }
  return result.release();
}

// static
int32_t CBC_OnedCode128Writer::Encode128B(const ByteString& contents,
                                          std::vector<int32_t>* patterns) {
  int32_t checkWeight = 1;
  patterns->push_back(CODE_START_B);
  int32_t checkSum = CODE_START_B * checkWeight;
  for (size_t position = 0; position < contents.GetLength(); position++) {
    int32_t patternIndex = contents[position] - ' ';
    patterns->push_back(patternIndex);
    checkSum += patternIndex * checkWeight++;
  }
  return checkSum;
}

// static
int32_t CBC_OnedCode128Writer::Encode128C(const ByteString& contents,
                                          std::vector<int32_t>* patterns) {
  int32_t checkWeight = 1;
  patterns->push_back(CODE_START_C);
  int32_t checkSum = CODE_START_C * checkWeight;
  size_t position = 0;
  while (position < contents.GetLength()) {
    int32_t patternIndex;
    char ch = contents[position];
    if (std::isdigit(ch)) {
      patternIndex = FXSYS_atoi(
          contents.Mid(position, contents.IsValidIndex(position + 1) ? 2 : 1)
              .c_str());
      ++position;
      if (position < contents.GetLength() && std::isdigit(contents[position]))
        ++position;
    } else {
      patternIndex = static_cast<int32_t>(ch);
      ++position;
    }
    patterns->push_back(patternIndex);
    checkSum += patternIndex * checkWeight++;
  }
  return checkSum;
}
