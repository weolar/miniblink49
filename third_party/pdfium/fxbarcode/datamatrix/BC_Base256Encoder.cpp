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

#include "fxbarcode/datamatrix/BC_Base256Encoder.h"

#include "fxbarcode/datamatrix/BC_Encoder.h"
#include "fxbarcode/datamatrix/BC_EncoderContext.h"
#include "fxbarcode/datamatrix/BC_HighLevelEncoder.h"
#include "fxbarcode/datamatrix/BC_SymbolInfo.h"

namespace {

wchar_t Randomize255State(wchar_t ch, int32_t position) {
  int32_t pseudoRandom = ((149 * position) % 255) + 1;
  int32_t tempVariable = ch + pseudoRandom;
  if (tempVariable <= 255)
    return static_cast<wchar_t>(tempVariable);
  return static_cast<wchar_t>(tempVariable - 256);
}

}  // namespace

CBC_Base256Encoder::CBC_Base256Encoder() = default;

CBC_Base256Encoder::~CBC_Base256Encoder() = default;

CBC_HighLevelEncoder::Encoding CBC_Base256Encoder::GetEncodingMode() {
  return CBC_HighLevelEncoder::Encoding::BASE256;
}

bool CBC_Base256Encoder::Encode(CBC_EncoderContext* context) {
  WideString buffer;
  buffer.Reserve(context->getRemainingCharacters() + 1);
  buffer += L'\0';
  while (context->hasMoreCharacters()) {
    wchar_t c = context->getCurrentChar();
    buffer += c;
    context->m_pos++;
    CBC_HighLevelEncoder::Encoding newMode =
        CBC_HighLevelEncoder::LookAheadTest(context->m_msg, context->m_pos,
                                            GetEncodingMode());
    if (newMode != GetEncodingMode()) {
      context->SignalEncoderChange(newMode);
      break;
    }
  }
  size_t dataCount = buffer.GetLength() - 1;
  char buf[128];
  FXSYS_itoa(dataCount, buf, 10);
  buffer.SetAt(0, static_cast<wchar_t>(*buf) - '0');
  int32_t lengthFieldSize = 1;
  int32_t currentSize =
      context->getCodewordCount() + dataCount + lengthFieldSize;
  if (!context->UpdateSymbolInfo(currentSize))
    return false;

  bool mustPad = (context->m_symbolInfo->dataCapacity() - currentSize) > 0;
  if (context->hasMoreCharacters() || mustPad) {
    if (dataCount <= 249) {
      buffer.SetAt(0, static_cast<wchar_t>(dataCount));
    } else if (dataCount > 249 && dataCount <= 1555) {
      buffer.SetAt(0, static_cast<wchar_t>((dataCount / 250) + 249));
      buffer.Insert(1, static_cast<wchar_t>(dataCount % 250));
    } else {
      return false;
    }
  }
  for (const auto& c : buffer) {
    context->writeCodeword(
        Randomize255State(c, context->getCodewordCount() + 1));
  }
  return true;
}
