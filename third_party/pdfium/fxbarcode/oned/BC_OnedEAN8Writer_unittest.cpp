// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxbarcode/oned/BC_OnedEAN8Writer.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace {

TEST(OnedEAN8WriterTest, Encode) {
  CBC_OnedEAN8Writer writer;
  writer.InitEANWriter();
  int32_t width;
  int32_t height;
  uint8_t* encoded;
  const char* expected;

  // EAN-8 barcodes encode 8-digit numbers into 67 modules in a unidimensional
  // disposition.
  encoded = writer.Encode("", BCFORMAT_EAN_8, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("123", BCFORMAT_EAN_8, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("1234567", BCFORMAT_EAN_8, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("123456789", BCFORMAT_EAN_8, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("12345670", BCFORMAT_EAN_8, width, height);
  EXPECT_NE(nullptr, encoded);
  EXPECT_EQ(1, height);
  EXPECT_EQ(67, width);

  expected =
      "# #"      // Start
      "  ##  #"  // 1 L
      "  #  ##"  // 2 L
      " #### #"  // 3 L
      " #   ##"  // 4 L
      " # # "    // Middle
      "#  ### "  // 5 R
      "# #    "  // 6 R
      "#   #  "  // 7 R
      "###  # "  // 0 R
      "# #";     // End
  for (int i = 0; i < 67; i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);

  encoded = writer.Encode("99441104", BCFORMAT_EAN_8, width, height);
  EXPECT_NE(nullptr, encoded);
  EXPECT_EQ(1, height);
  EXPECT_EQ(67, width);

  expected =
      "# #"      // Start
      "   # ##"  // 9 L
      "   # ##"  // 9 L
      " #   ##"  // 4 L
      " #   ##"  // 4 L
      " # # "    // Middle
      "##  ## "  // 1 R
      "##  ## "  // 1 R
      "###  # "  // 0 R
      "# ###  "  // 4 R
      "# #";     // End
  for (int i = 0; i < 67; i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);
}

TEST(OnedEAN8WriterTest, Checksum) {
  CBC_OnedEAN8Writer writer;
  writer.InitEANWriter();
  EXPECT_EQ(0, writer.CalcChecksum(""));
  EXPECT_EQ(6, writer.CalcChecksum("123"));
  EXPECT_EQ(0, writer.CalcChecksum("1234567"));
  EXPECT_EQ(4, writer.CalcChecksum("9944110"));
}

}  // namespace
