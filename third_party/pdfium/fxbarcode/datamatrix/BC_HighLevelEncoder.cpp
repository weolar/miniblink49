// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2006-2007 Jeremias Maerki.
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

#include "fxbarcode/datamatrix/BC_HighLevelEncoder.h"

#include <algorithm>
#include <array>
#include <limits>
#include <memory>
#include <vector>

#include "core/fxcrt/fx_extension.h"
#include "fxbarcode/common/BC_CommonBitMatrix.h"
#include "fxbarcode/datamatrix/BC_ASCIIEncoder.h"
#include "fxbarcode/datamatrix/BC_Base256Encoder.h"
#include "fxbarcode/datamatrix/BC_C40Encoder.h"
#include "fxbarcode/datamatrix/BC_EdifactEncoder.h"
#include "fxbarcode/datamatrix/BC_Encoder.h"
#include "fxbarcode/datamatrix/BC_EncoderContext.h"
#include "fxbarcode/datamatrix/BC_SymbolInfo.h"
#include "fxbarcode/datamatrix/BC_TextEncoder.h"
#include "fxbarcode/datamatrix/BC_X12Encoder.h"
#include "third_party/base/ptr_util.h"

namespace {

const wchar_t kPad = 129;
const wchar_t kMacro05 = 236;
const wchar_t kMacro06 = 237;
const wchar_t kMacro05Header[] =
    L"[)>\036"
    L"05";
const wchar_t kMacro06Header[] =
    L"[)>\036"
    L"06";
const wchar_t kMacroTrailer = 0x0004;

constexpr size_t kEncoderCount =
    static_cast<size_t>(CBC_HighLevelEncoder::Encoding::LAST) + 1;
static_assert(kEncoderCount == 6, "Bad encoder count");

wchar_t Randomize253State(wchar_t ch, int32_t codewordPosition) {
  int32_t pseudoRandom = ((149 * codewordPosition) % 253) + 1;
  int32_t tempVariable = ch + pseudoRandom;
  return tempVariable <= 254 ? static_cast<wchar_t>(tempVariable)
                             : static_cast<wchar_t>(tempVariable - 254);
}

int32_t FindMinimums(const std::array<float, kEncoderCount>& charCounts,
                     std::array<int32_t, kEncoderCount>* intCharCounts,
                     std::array<uint8_t, kEncoderCount>* mins) {
  int32_t min = std::numeric_limits<int32_t>::max();
  for (size_t i = 0; i < kEncoderCount; ++i) {
    int32_t current = static_cast<int32_t>(ceil(charCounts[i]));
    (*intCharCounts)[i] = current;
    if (min > current) {
      min = current;
      for (auto& m : *mins)
        m = 0;
    }
    if (min == current)
      (*mins)[i]++;
  }
  return min;
}

int32_t GetMinimumCount(const std::array<uint8_t, kEncoderCount>& mins) {
  int32_t count = 0;
  for (const auto& m : mins)
    count += m;
  return count;
}

bool IsNativeC40(wchar_t ch) {
  return (ch == ' ') || (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z');
}

bool IsNativeText(wchar_t ch) {
  return (ch == ' ') || (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z');
}

bool IsX12TermSep(wchar_t ch) {
  return (ch == '\r') || (ch == '*') || (ch == '>');
}

bool IsNativeX12(wchar_t ch) {
  return IsX12TermSep(ch) || (ch == ' ') || (ch >= '0' && ch <= '9') ||
         (ch >= 'A' && ch <= 'Z');
}

bool IsNativeEDIFACT(wchar_t ch) {
  return ch >= ' ' && ch <= '^';
}

size_t EncoderIndex(CBC_HighLevelEncoder::Encoding encoding) {
  ASSERT(encoding != CBC_HighLevelEncoder::Encoding::UNKNOWN);
  return static_cast<size_t>(encoding);
}

}  // namespace

// static
WideString CBC_HighLevelEncoder::EncodeHighLevel(const WideString& msg) {
  CBC_EncoderContext context(msg);
  if (context.HasCharactersOutsideISO88591Encoding())
    return WideString();

  if (msg.Last() == kMacroTrailer) {
    WideString left = msg.Left(6);
    if (left == kMacro05Header) {
      context.writeCodeword(kMacro05);
      context.setSkipAtEnd(2);
      context.m_pos += 6;
    } else if (left == kMacro06Header) {
      context.writeCodeword(kMacro06);
      context.setSkipAtEnd(2);
      context.m_pos += 6;
    }
  }

  std::vector<std::unique_ptr<CBC_Encoder>> encoders;
  encoders.push_back(pdfium::MakeUnique<CBC_ASCIIEncoder>());
  encoders.push_back(pdfium::MakeUnique<CBC_C40Encoder>());
  encoders.push_back(pdfium::MakeUnique<CBC_TextEncoder>());
  encoders.push_back(pdfium::MakeUnique<CBC_X12Encoder>());
  encoders.push_back(pdfium::MakeUnique<CBC_EdifactEncoder>());
  encoders.push_back(pdfium::MakeUnique<CBC_Base256Encoder>());
  Encoding encodingMode = Encoding::ASCII;
  while (context.hasMoreCharacters()) {
    if (!encoders[EncoderIndex(encodingMode)]->Encode(&context))
      return WideString();

    if (context.m_newEncoding != Encoding::UNKNOWN) {
      encodingMode = context.m_newEncoding;
      context.ResetEncoderSignal();
    }
  }
  size_t len = context.m_codewords.GetLength();
  if (!context.UpdateSymbolInfo())
    return WideString();

  size_t capacity = context.m_symbolInfo->dataCapacity();
  if (len < capacity) {
    if (encodingMode != Encoding::ASCII && encodingMode != Encoding::BASE256)
      context.writeCodeword(0x00fe);
  }
  WideString codewords = context.m_codewords;
  if (codewords.GetLength() < capacity)
    codewords += kPad;

  while (codewords.GetLength() < capacity)
    codewords += Randomize253State(kPad, codewords.GetLength() + 1);

  ASSERT(!codewords.IsEmpty());
  return codewords;
}

// static
CBC_HighLevelEncoder::Encoding CBC_HighLevelEncoder::LookAheadTest(
    const WideString& msg,
    size_t startpos,
    CBC_HighLevelEncoder::Encoding currentMode) {
  if (startpos >= msg.GetLength())
    return currentMode;

  std::array<float, kEncoderCount> charCounts;
  if (currentMode == Encoding::ASCII) {
    charCounts = {0, 1, 1, 1, 1, 1.25f};
  } else {
    charCounts = {1, 2, 2, 2, 2, 2.25f};
    charCounts[EncoderIndex(currentMode)] = 0;
  }

  size_t charsProcessed = 0;
  while (true) {
    if ((startpos + charsProcessed) == msg.GetLength()) {
      std::array<int32_t, kEncoderCount> intCharCounts;
      std::array<uint8_t, kEncoderCount> mins;
      int32_t min = FindMinimums(charCounts, &intCharCounts, &mins);
      if (intCharCounts[EncoderIndex(Encoding::ASCII)] == min)
        return Encoding::ASCII;
      const int32_t minCount = GetMinimumCount(mins);
      if (minCount == 1) {
        if (mins[EncoderIndex(Encoding::BASE256)] > 0)
          return Encoding::BASE256;
        if (mins[EncoderIndex(Encoding::EDIFACT)] > 0)
          return Encoding::EDIFACT;
        if (mins[EncoderIndex(Encoding::TEXT)] > 0)
          return Encoding::TEXT;
        if (mins[EncoderIndex(Encoding::X12)] > 0)
          return Encoding::X12;
      }
      return Encoding::C40;
    }

    wchar_t c = msg[startpos + charsProcessed];
    charsProcessed++;
    {
      auto& count = charCounts[EncoderIndex(Encoding::ASCII)];
      if (FXSYS_IsDecimalDigit(c))
        count += 0.5;
      else if (IsExtendedASCII(c))
        count = ceilf(count) + 2;
      else
        count = ceilf(count) + 1;
    }

    {
      auto& count = charCounts[EncoderIndex(Encoding::C40)];
      if (IsNativeC40(c))
        count += 2.0f / 3.0f;
      else if (IsExtendedASCII(c))
        count += 8.0f / 3.0f;
      else
        count += 4.0f / 3.0f;
    }

    {
      auto& count = charCounts[EncoderIndex(Encoding::TEXT)];
      if (IsNativeText(c))
        count += 2.0f / 3.0f;
      else if (IsExtendedASCII(c))
        count += 8.0f / 3.0f;
      else
        count += 4.0f / 3.0f;
    }

    {
      auto& count = charCounts[EncoderIndex(Encoding::X12)];
      if (IsNativeX12(c))
        count += 2.0f / 3.0f;
      else if (IsExtendedASCII(c))
        count += 13.0f / 3.0f;
      else
        count += 10.0f / 3.0f;
    }

    {
      auto& count = charCounts[EncoderIndex(Encoding::EDIFACT)];
      if (IsNativeEDIFACT(c))
        count += 3.0f / 4.0f;
      else if (IsExtendedASCII(c))
        count += 17.0f / 4.0f;
      else
        count += 13.0f / 4.0f;
    }

    charCounts[EncoderIndex(Encoding::BASE256)]++;
    if (charsProcessed < 4)
      continue;

    std::array<int32_t, kEncoderCount> intCharCounts;
    std::array<uint8_t, kEncoderCount> mins;
    FindMinimums(charCounts, &intCharCounts, &mins);
    int32_t minCount = GetMinimumCount(mins);
    int32_t ascii_count = intCharCounts[EncoderIndex(Encoding::ASCII)];
    int32_t c40_count = intCharCounts[EncoderIndex(Encoding::C40)];
    int32_t text_count = intCharCounts[EncoderIndex(Encoding::TEXT)];
    int32_t x12_count = intCharCounts[EncoderIndex(Encoding::X12)];
    int32_t editfact_count = intCharCounts[EncoderIndex(Encoding::EDIFACT)];
    int32_t base256_count = intCharCounts[EncoderIndex(Encoding::BASE256)];
    int32_t bet_min = std::min({base256_count, editfact_count, text_count});
    if (ascii_count < bet_min && ascii_count < c40_count &&
        ascii_count < x12_count) {
      return Encoding::ASCII;
    }
    if (base256_count < ascii_count ||
        (mins[EncoderIndex(Encoding::C40)] +
         mins[EncoderIndex(Encoding::TEXT)] +
         mins[EncoderIndex(Encoding::X12)] +
         mins[EncoderIndex(Encoding::EDIFACT)]) == 0) {
      return Encoding::BASE256;
    }
    if (minCount == 1) {
      if (mins[EncoderIndex(Encoding::EDIFACT)] > 0)
        return Encoding::EDIFACT;
      if (mins[EncoderIndex(Encoding::TEXT)] > 0)
        return Encoding::TEXT;
      if (mins[EncoderIndex(Encoding::X12)] > 0)
        return Encoding::X12;
    }
    if (c40_count + 1 < ascii_count && c40_count + 1 < bet_min) {
      if (c40_count < x12_count)
        return Encoding::C40;
      if (c40_count == x12_count) {
        size_t p = startpos + charsProcessed + 1;
        while (p < msg.GetLength()) {
          wchar_t tc = msg[p];
          if (IsX12TermSep(tc))
            return Encoding::X12;
          if (!IsNativeX12(tc))
            break;
          p++;
        }
        return Encoding::C40;
      }
    }
  }
}

// static
bool CBC_HighLevelEncoder::IsExtendedASCII(wchar_t ch) {
  return ch >= 128 && ch <= 255;
}
