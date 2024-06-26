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

#include "fxbarcode/datamatrix/BC_ASCIIEncoder.h"

#include "core/fxcrt/fx_extension.h"
#include "fxbarcode/datamatrix/BC_Encoder.h"
#include "fxbarcode/datamatrix/BC_EncoderContext.h"
#include "fxbarcode/datamatrix/BC_HighLevelEncoder.h"
#include "fxbarcode/datamatrix/BC_SymbolInfo.h"
#include "third_party/base/optional.h"

namespace {

Optional<wchar_t> EncodeASCIIDigits(wchar_t digit1, wchar_t digit2) {
  if (!FXSYS_IsDecimalDigit(digit1) || !FXSYS_IsDecimalDigit(digit2)) {
    // This could potentially return 0 as a sentinel value. Then this function
    // can just return wchar_t instead of Optional<wchar_t>.
    return {};
  }
  return static_cast<wchar_t>((digit1 - 48) * 10 + (digit2 - 48) + 130);
}

size_t DetermineConsecutiveDigitCount(const WideString& msg, size_t startpos) {
  size_t count = 0;
  for (size_t i = startpos; i < msg.GetLength(); ++i) {
    if (!FXSYS_IsDecimalDigit(msg[i]))
      break;
    ++count;
  }
  return count;
}

}  // namespace

CBC_ASCIIEncoder::CBC_ASCIIEncoder() = default;

CBC_ASCIIEncoder::~CBC_ASCIIEncoder() = default;

CBC_HighLevelEncoder::Encoding CBC_ASCIIEncoder::GetEncodingMode() {
  return CBC_HighLevelEncoder::Encoding::ASCII;
}

bool CBC_ASCIIEncoder::Encode(CBC_EncoderContext* context) {
  size_t n = DetermineConsecutiveDigitCount(context->m_msg, context->m_pos);
  if (n >= 2) {
    Optional<wchar_t> code = EncodeASCIIDigits(
        context->m_msg[context->m_pos], context->m_msg[context->m_pos + 1]);
    if (!code)
      return false;

    context->writeCodeword(*code);
    context->m_pos += 2;
    return true;
  }

  wchar_t c = context->getCurrentChar();
  CBC_HighLevelEncoder::Encoding newMode = CBC_HighLevelEncoder::LookAheadTest(
      context->m_msg, context->m_pos, GetEncodingMode());
  if (newMode != GetEncodingMode()) {
    switch (newMode) {
      case CBC_HighLevelEncoder::Encoding::BASE256:
        context->writeCodeword(CBC_HighLevelEncoder::LATCH_TO_BASE256);
        break;
      case CBC_HighLevelEncoder::Encoding::C40:
        context->writeCodeword(CBC_HighLevelEncoder::LATCH_TO_C40);
        break;
      case CBC_HighLevelEncoder::Encoding::X12:
        context->writeCodeword(CBC_HighLevelEncoder::LATCH_TO_ANSIX12);
        break;
      case CBC_HighLevelEncoder::Encoding::TEXT:
        context->writeCodeword(CBC_HighLevelEncoder::LATCH_TO_TEXT);
        break;
      case CBC_HighLevelEncoder::Encoding::EDIFACT:
        context->writeCodeword(CBC_HighLevelEncoder::LATCH_TO_EDIFACT);
        break;
      default:
        return false;
    }
    context->SignalEncoderChange(newMode);
    return true;
  }

  if (CBC_HighLevelEncoder::IsExtendedASCII(c)) {
    context->writeCodeword(CBC_HighLevelEncoder::UPPER_SHIFT);
    context->writeCodeword(static_cast<wchar_t>(c - 128 + 1));
  } else {
    context->writeCodeword(static_cast<wchar_t>(c + 1));
  }
  context->m_pos++;
  return true;
}
