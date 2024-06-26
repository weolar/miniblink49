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

#include "fxbarcode/datamatrix/BC_TextEncoder.h"

#include "core/fxcrt/fx_extension.h"
#include "fxbarcode/common/BC_CommonBitMatrix.h"
#include "fxbarcode/datamatrix/BC_C40Encoder.h"
#include "fxbarcode/datamatrix/BC_Encoder.h"
#include "fxbarcode/datamatrix/BC_EncoderContext.h"
#include "fxbarcode/datamatrix/BC_HighLevelEncoder.h"
#include "fxbarcode/datamatrix/BC_SymbolInfo.h"

CBC_TextEncoder::CBC_TextEncoder() = default;

CBC_TextEncoder::~CBC_TextEncoder() = default;

CBC_HighLevelEncoder::Encoding CBC_TextEncoder::GetEncodingMode() {
  return CBC_HighLevelEncoder::Encoding::TEXT;
}

int32_t CBC_TextEncoder::EncodeChar(wchar_t c, WideString* sb) {
  if (c == ' ') {
    *sb += (wchar_t)'\3';
    return 1;
  }
  if (FXSYS_IsDecimalDigit(c)) {
    *sb += (wchar_t)(c - 48 + 4);
    return 1;
  }
  if (c >= 'a' && c <= 'z') {
    *sb += (wchar_t)(c - 97 + 14);
    return 1;
  }
  if (c <= 0x1f) {
    *sb += (wchar_t)'\0';
    *sb += c;
    return 2;
  }
  if (c >= '!' && c <= '/') {
    *sb += (wchar_t)'\1';
    *sb += (wchar_t)(c - 33);
    return 2;
  }
  if (c >= ':' && c <= '@') {
    *sb += (wchar_t)'\1';
    *sb += (wchar_t)(c - 58 + 15);
    return 2;
  }
  if (c >= '[' && c <= '_') {
    *sb += (wchar_t)'\1';
    *sb += (wchar_t)(c - 91 + 22);
    return 2;
  }
  if (c == 0x0060) {
    *sb += (wchar_t)'\2';
    *sb += (wchar_t)(c - 96);
    return 2;
  }
  if (c >= 'A' && c <= 'Z') {
    *sb += (wchar_t)'\2';
    *sb += (wchar_t)(c - 65 + 1);
    return 2;
  }
  if (c >= '{' && c <= 0x007f) {
    *sb += (wchar_t)'\2';
    *sb += (wchar_t)(c - 123 + 27);
    return 2;
  }
  if (c < 0x0080)
    return 0;

  *sb += (wchar_t)'\1';
  *sb += (wchar_t)0x001e;
  int32_t encode_result = EncodeChar(c - 128, sb);
  return encode_result > 0 ? encode_result + 2 : 0;
}
