// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/cfx_bitstream.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

uint32_t ReferenceGetBits32(const uint8_t* pData, int bitpos, int nbits) {
  int result = 0;
  for (int i = 0; i < nbits; i++) {
    if (pData[(bitpos + i) / 8] & (1 << (7 - (bitpos + i) % 8)))
      result |= 1 << (nbits - i - 1);
  }
  return result;
}

}  // namespace

TEST(fxcrt, BitStream) {
  static const uint8_t kData[] = {0x00, 0x11, 0x22, 0x33,
                                  0x44, 0x55, 0x66, 0x77};
  CFX_BitStream bitstream(kData);

  // Initial state.
  EXPECT_FALSE(bitstream.IsEOF());
  EXPECT_EQ(0U, bitstream.GetPos());
  EXPECT_EQ(64U, bitstream.BitsRemaining());

  // Read, read, read!
  EXPECT_EQ(0x00U, bitstream.GetBits(8));
  EXPECT_EQ(8U, bitstream.GetPos());
  EXPECT_EQ(56U, bitstream.BitsRemaining());

  EXPECT_EQ(0x00U, bitstream.GetBits(1));
  EXPECT_EQ(9U, bitstream.GetPos());
  EXPECT_EQ(55U, bitstream.BitsRemaining());

  EXPECT_EQ(0x00U, bitstream.GetBits(2));
  EXPECT_EQ(11U, bitstream.GetPos());
  EXPECT_EQ(53U, bitstream.BitsRemaining());

  EXPECT_EQ(0x04U, bitstream.GetBits(3));
  EXPECT_EQ(14U, bitstream.GetPos());
  EXPECT_EQ(50U, bitstream.BitsRemaining());

  EXPECT_EQ(0x04U, bitstream.GetBits(4));
  EXPECT_EQ(18U, bitstream.GetPos());
  EXPECT_EQ(46U, bitstream.BitsRemaining());

  EXPECT_EQ(0x44U, bitstream.GetBits(7));
  EXPECT_EQ(25U, bitstream.GetPos());
  EXPECT_EQ(39U, bitstream.BitsRemaining());

  EXPECT_EQ(0xCDU, bitstream.GetBits(9));
  EXPECT_EQ(34U, bitstream.GetPos());
  EXPECT_EQ(30U, bitstream.BitsRemaining());

  EXPECT_EQ(0x08AAU, bitstream.GetBits(15));
  EXPECT_EQ(49U, bitstream.GetPos());
  EXPECT_EQ(15U, bitstream.BitsRemaining());

  // Cannot advance past the end.
  EXPECT_EQ(0x00U, bitstream.GetBits(16));
  EXPECT_EQ(49U, bitstream.GetPos());
  EXPECT_EQ(15U, bitstream.BitsRemaining());

  // Make sure SkipBits() works.
  bitstream.SkipBits(14);
  EXPECT_EQ(63U, bitstream.GetPos());
  EXPECT_EQ(1U, bitstream.BitsRemaining());
  bitstream.SkipBits(2);
  EXPECT_EQ(65U, bitstream.GetPos());
  EXPECT_EQ(0U, bitstream.BitsRemaining());
  EXPECT_TRUE(bitstream.IsEOF());

  // Make sure Rewind() works.
  bitstream.Rewind();
  EXPECT_FALSE(bitstream.IsEOF());
  EXPECT_EQ(0U, bitstream.GetPos());
  EXPECT_EQ(64U, bitstream.BitsRemaining());

  // Read some more.
  bitstream.SkipBits(5);
  EXPECT_EQ(5U, bitstream.GetPos());
  EXPECT_EQ(59U, bitstream.BitsRemaining());

  EXPECT_EQ(0x0448U, bitstream.GetBits(17));
  EXPECT_EQ(22U, bitstream.GetPos());
  EXPECT_EQ(42U, bitstream.BitsRemaining());

  // Make sure ByteAlign() works.
  bitstream.ByteAlign();
  EXPECT_EQ(24U, bitstream.GetPos());
  EXPECT_EQ(40U, bitstream.BitsRemaining());

  EXPECT_EQ(0x19A22AB3U, bitstream.GetBits(31));
  EXPECT_EQ(55U, bitstream.GetPos());
  EXPECT_EQ(9U, bitstream.BitsRemaining());

  // Do some bigger reads.
  bitstream.Rewind();
  EXPECT_EQ(0x112233U, bitstream.GetBits(32));
  EXPECT_EQ(32U, bitstream.GetPos());
  EXPECT_EQ(32U, bitstream.BitsRemaining());

  bitstream.Rewind();
  bitstream.SkipBits(31);
  EXPECT_EQ(0xA22AB33BU, bitstream.GetBits(32));
  EXPECT_EQ(63U, bitstream.GetPos());
  EXPECT_EQ(1U, bitstream.BitsRemaining());

  // Skip past the end.
  bitstream.SkipBits(1000);
  EXPECT_TRUE(bitstream.IsEOF());
  EXPECT_EQ(1063U, bitstream.GetPos());
  EXPECT_EQ(0U, bitstream.BitsRemaining());
}

TEST(fxcrt, BitStreamSameAsReferenceGetBits32) {
  static const unsigned char kData[] = {0xDE, 0x3F, 0xB1, 0x7C,
                                        0x12, 0x9A, 0x04, 0x56};
  CFX_BitStream bitstream(kData);
  for (int nbits = 1; nbits <= 32; ++nbits) {
    for (size_t bitpos = 0; bitpos < sizeof(kData) * 8 - nbits; ++bitpos) {
      bitstream.Rewind();
      bitstream.SkipBits(bitpos);
      EXPECT_EQ(bitstream.GetBits(nbits),
                ReferenceGetBits32(kData, bitpos, nbits));
    }
  }
}
