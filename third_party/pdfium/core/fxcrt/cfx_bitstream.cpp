// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/cfx_bitstream.h"

#include <limits>

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_system.h"

CFX_BitStream::CFX_BitStream(pdfium::span<const uint8_t> pData)
    : m_BitPos(0), m_BitSize(pData.size() * 8), m_pData(pData.data()) {
  ASSERT(pData.size() <= std::numeric_limits<uint32_t>::max() / 8);
}

CFX_BitStream::~CFX_BitStream() {}

void CFX_BitStream::ByteAlign() {
  m_BitPos = FxAlignToBoundary<8>(m_BitPos);
}

uint32_t CFX_BitStream::GetBits(uint32_t nBits) {
  ASSERT(nBits > 0);
  ASSERT(nBits <= 32);
  if (nBits > m_BitSize || m_BitPos > m_BitSize - nBits)
    return 0;

  const uint32_t bit_pos = m_BitPos % 8;
  uint32_t byte_pos = m_BitPos / 8;
  const uint8_t* data = m_pData.Get();
  uint8_t current_byte = data[byte_pos];

  if (nBits == 1) {
    int bit = (current_byte & (1 << (7 - bit_pos))) ? 1 : 0;
    m_BitPos++;
    return bit;
  }

  uint32_t bit_left = nBits;
  uint32_t result = 0;
  if (bit_pos) {
    uint32_t bits_readable = 8 - bit_pos;
    if (bits_readable >= bit_left) {
      result = (current_byte & (0xff >> bit_pos)) >> (bits_readable - bit_left);
      m_BitPos += bit_left;
      return result;
    }
    bit_left -= bits_readable;
    result = (current_byte & ((1 << bits_readable) - 1)) << bit_left;
    ++byte_pos;
  }
  while (bit_left >= 8) {
    bit_left -= 8;
    result |= data[byte_pos++] << bit_left;
  }
  if (bit_left)
    result |= data[byte_pos] >> (8 - bit_left);
  m_BitPos += nBits;
  return result;
}
