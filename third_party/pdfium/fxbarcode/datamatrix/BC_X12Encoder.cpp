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

#include "fxbarcode/datamatrix/BC_X12Encoder.h"

#include "core/fxcrt/fx_extension.h"
#include "fxbarcode/common/BC_CommonBitMatrix.h"
#include "fxbarcode/datamatrix/BC_C40Encoder.h"
#include "fxbarcode/datamatrix/BC_Encoder.h"
#include "fxbarcode/datamatrix/BC_EncoderContext.h"
#include "fxbarcode/datamatrix/BC_HighLevelEncoder.h"
#include "fxbarcode/datamatrix/BC_SymbolInfo.h"

CBC_X12Encoder::CBC_X12Encoder() = default;

CBC_X12Encoder::~CBC_X12Encoder() = default;

CBC_HighLevelEncoder::Encoding CBC_X12Encoder::GetEncodingMode() {
  return CBC_HighLevelEncoder::Encoding::X12;
}

bool CBC_X12Encoder::Encode(CBC_EncoderContext* context) {
  WideString buffer;
  while (context->hasMoreCharacters()) {
    wchar_t c = context->getCurrentChar();
    context->m_pos++;
    if (EncodeChar(c, &buffer) <= 0)
      return false;

    size_t count = buffer.GetLength();
    if ((count % 3) == 0) {
      WriteNextTriplet(context, &buffer);
      CBC_HighLevelEncoder::Encoding newMode =
          CBC_HighLevelEncoder::LookAheadTest(context->m_msg, context->m_pos,
                                              GetEncodingMode());
      if (newMode != GetEncodingMode()) {
        context->SignalEncoderChange(newMode);
        break;
      }
    }
  }
  return HandleEOD(context, &buffer);
}

bool CBC_X12Encoder::HandleEOD(CBC_EncoderContext* context,
                               WideString* buffer) {
  if (!context->UpdateSymbolInfo())
    return false;

  int32_t available =
      context->m_symbolInfo->dataCapacity() - context->getCodewordCount();
  size_t count = buffer->GetLength();
  if (count == 2) {
    context->writeCodeword(CBC_HighLevelEncoder::X12_UNLATCH);
    context->m_pos -= 2;
    context->SignalEncoderChange(CBC_HighLevelEncoder::Encoding::ASCII);
  } else if (count == 1) {
    context->m_pos--;
    if (available > 1) {
      context->writeCodeword(CBC_HighLevelEncoder::X12_UNLATCH);
    }
    context->SignalEncoderChange(CBC_HighLevelEncoder::Encoding::ASCII);
  }
  return true;
}

int32_t CBC_X12Encoder::EncodeChar(wchar_t c, WideString* sb) {
  if (c == '\r')
    *sb += (wchar_t)'\0';
  else if (c == '*')
    *sb += (wchar_t)'\1';
  else if (c == '>')
    *sb += (wchar_t)'\2';
  else if (c == ' ')
    *sb += (wchar_t)'\3';
  else if (FXSYS_IsDecimalDigit(c))
    *sb += (wchar_t)(c - 48 + 4);
  else if (c >= 'A' && c <= 'Z')
    *sb += (wchar_t)(c - 65 + 14);
  else
    return 0;
  return 1;
}
