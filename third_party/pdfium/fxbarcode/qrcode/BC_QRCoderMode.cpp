// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2007 ZXing authors
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

#include "fxbarcode/qrcode/BC_QRCoderMode.h"

#include <utility>

#include "core/fxcrt/fx_system.h"

CBC_QRCoderMode* CBC_QRCoderMode::sBYTE = nullptr;
CBC_QRCoderMode* CBC_QRCoderMode::sNUMERIC = nullptr;
CBC_QRCoderMode* CBC_QRCoderMode::sALPHANUMERIC = nullptr;
CBC_QRCoderMode* CBC_QRCoderMode::sKANJI = nullptr;
CBC_QRCoderMode* CBC_QRCoderMode::sECI = nullptr;
CBC_QRCoderMode* CBC_QRCoderMode::sGBK = nullptr;
CBC_QRCoderMode* CBC_QRCoderMode::sTERMINATOR = nullptr;
CBC_QRCoderMode* CBC_QRCoderMode::sFNC1_FIRST_POSITION = nullptr;
CBC_QRCoderMode* CBC_QRCoderMode::sFNC1_SECOND_POSITION = nullptr;
CBC_QRCoderMode* CBC_QRCoderMode::sSTRUCTURED_APPEND = nullptr;

CBC_QRCoderMode::CBC_QRCoderMode(std::vector<int32_t> charCountBits,
                                 int32_t bits)
    : m_characterCountBitsForVersions(std::move(charCountBits)), m_bits(bits) {}

CBC_QRCoderMode::~CBC_QRCoderMode() = default;

void CBC_QRCoderMode::Initialize() {
  sBYTE = new CBC_QRCoderMode({8, 16, 16}, 0x4);
  sALPHANUMERIC = new CBC_QRCoderMode({9, 11, 13}, 0x2);
  sECI = new CBC_QRCoderMode(std::vector<int32_t>(), 0x7);
  sKANJI = new CBC_QRCoderMode({8, 10, 12}, 0x8);
  sNUMERIC = new CBC_QRCoderMode({10, 12, 14}, 0x1);
  sGBK = new CBC_QRCoderMode({8, 10, 12}, 0x0D);
  sTERMINATOR = new CBC_QRCoderMode(std::vector<int32_t>(), 0x00);
  sFNC1_FIRST_POSITION = new CBC_QRCoderMode(std::vector<int32_t>(), 0x05);
  sFNC1_SECOND_POSITION = new CBC_QRCoderMode(std::vector<int32_t>(), 0x09);
  sSTRUCTURED_APPEND = new CBC_QRCoderMode(std::vector<int32_t>(), 0x03);
}

void CBC_QRCoderMode::Finalize() {
  delete sBYTE;
  sBYTE = nullptr;
  delete sALPHANUMERIC;
  sALPHANUMERIC = nullptr;
  delete sECI;
  sECI = nullptr;
  delete sKANJI;
  sKANJI = nullptr;
  delete sNUMERIC;
  sNUMERIC = nullptr;
  delete sGBK;
  sGBK = nullptr;
  delete sTERMINATOR;
  sTERMINATOR = nullptr;
  delete sFNC1_FIRST_POSITION;
  sFNC1_FIRST_POSITION = nullptr;
  delete sFNC1_SECOND_POSITION;
  sFNC1_SECOND_POSITION = nullptr;
  delete sSTRUCTURED_APPEND;
  sSTRUCTURED_APPEND = nullptr;
}

int32_t CBC_QRCoderMode::GetBits() const {
  return m_bits;
}

int32_t CBC_QRCoderMode::GetCharacterCountBits(int32_t number) const {
  if (m_characterCountBitsForVersions.empty())
    return 0;

  int32_t offset;
  if (number <= 9)
    offset = 0;
  else if (number <= 26)
    offset = 1;
  else
    offset = 2;

  int32_t result = m_characterCountBitsForVersions[offset];
  ASSERT(result != 0);
  return result;
}
