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

#include "fxbarcode/qrcode/BC_QRCoderBitVector.h"

#include "core/fxcrt/fx_system.h"
#include "third_party/base/logging.h"

CBC_QRCoderBitVector::CBC_QRCoderBitVector() = default;

CBC_QRCoderBitVector::~CBC_QRCoderBitVector() = default;

int32_t CBC_QRCoderBitVector::At(size_t index) const {
  CHECK(index < m_sizeInBits);
  int32_t value = m_array[index >> 3] & 0xff;
  return (value >> (7 - (index & 0x7))) & 1;
}

size_t CBC_QRCoderBitVector::sizeInBytes() const {
  return (m_sizeInBits + 7) >> 3;
}

size_t CBC_QRCoderBitVector::Size() const {
  return m_sizeInBits;
}

void CBC_QRCoderBitVector::AppendBit(int32_t bit) {
  ASSERT(bit == 0 || bit == 1);
  int32_t numBitsInLastByte = m_sizeInBits & 0x7;
  if (numBitsInLastByte == 0) {
    AppendByte(0);
    m_sizeInBits -= 8;
  }
  m_array[m_sizeInBits >> 3] |= (bit << (7 - numBitsInLastByte));
  ++m_sizeInBits;
}

void CBC_QRCoderBitVector::AppendBits(int32_t value, int32_t numBits) {
  ASSERT(numBits > 0);
  ASSERT(numBits <= 32);

  int32_t numBitsLeft = numBits;
  while (numBitsLeft > 0) {
    if ((m_sizeInBits & 0x7) == 0 && numBitsLeft >= 8) {
      AppendByte(static_cast<int8_t>((value >> (numBitsLeft - 8)) & 0xff));
      numBitsLeft -= 8;
    } else {
      AppendBit((value >> (numBitsLeft - 1)) & 1);
      --numBitsLeft;
    }
  }
}

void CBC_QRCoderBitVector::AppendBitVector(const CBC_QRCoderBitVector* bits) {
  for (size_t i = 0; i < bits->Size(); i++)
    AppendBit(bits->At(i));
}

bool CBC_QRCoderBitVector::XOR(const CBC_QRCoderBitVector* other) {
  if (m_sizeInBits != other->Size())
    return false;

  const auto* pOther = other->GetArray();
  for (size_t i = 0; i < sizeInBytes(); ++i)
    m_array[i] ^= pOther[i];
  return true;
}

const uint8_t* CBC_QRCoderBitVector::GetArray() const {
  return m_array.data();
}

void CBC_QRCoderBitVector::AppendByte(int8_t value) {
  if ((m_sizeInBits >> 3) == m_array.size())
    m_array.push_back(0);
  m_array[m_sizeInBits >> 3] = value;
  m_sizeInBits += 8;
}
