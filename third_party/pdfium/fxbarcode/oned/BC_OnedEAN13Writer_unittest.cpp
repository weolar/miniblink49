// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxbarcode/oned/BC_OnedEAN13Writer.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace {

TEST(OnedEAN13WriterTest, Encode) {
  CBC_OnedEAN13Writer writer;
  writer.InitEANWriter();
  int32_t width;
  int32_t height;
  uint8_t* encoded;
  const char* expected;

  // EAN-13 barcodes encode 13-digit numbers into 95 modules in a unidimensional
  // disposition.
  encoded = writer.Encode("", BCFORMAT_EAN_13, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("123", BCFORMAT_EAN_13, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("123456789012", BCFORMAT_EAN_13, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("12345678901234", BCFORMAT_EAN_13, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("1234567890128", BCFORMAT_EAN_13, width, height);
  EXPECT_NE(nullptr, encoded);
  EXPECT_EQ(1, height);
  EXPECT_EQ(95, width);

  expected =
      "# #"  // Start
      // 1 implicit by LLGLGG in next 6 digits
      "  #  ##"  // 2 L
      " #### #"  // 3 L
      "  ### #"  // 4 G
      " ##   #"  // 5 L
      "    # #"  // 6 G
      "  #   #"  // 7 G
      " # # "    // Middle
      "#  #   "  // 8 R
      "### #  "  // 9 R
      "###  # "  // 0 R
      "##  ## "  // 1 R
      "## ##  "  // 2 R
      "#  #   "  // 8 R
      "# #";     // End
  for (int i = 0; i < 95; i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);

  encoded = writer.Encode("7776665554440", BCFORMAT_EAN_13, width, height);
  EXPECT_NE(nullptr, encoded);
  EXPECT_EQ(1, height);
  EXPECT_EQ(95, width);

  expected =
      "# #"  // Start
      // 7 implicit by LGLGLG in next 6 digits
      " ### ##"  // 7 L
      "  #   #"  // 7 G
      " # ####"  // 6 L
      "    # #"  // 6 G
      " # ####"  // 6 L
      " ###  #"  // 5 G
      " # # "    // Middle
      "#  ### "  // 5 R
      "#  ### "  // 5 R
      "# ###  "  // 4 R
      "# ###  "  // 4 R
      "# ###  "  // 4 R
      "###  # "  // 0 R
      "# #";     // End
  for (int i = 0; i < 95; i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);
}

TEST(OnedEAN13WriterTest, Checksum) {
  CBC_OnedEAN13Writer writer;
  writer.InitEANWriter();
  EXPECT_EQ(0, writer.CalcChecksum(""));
  EXPECT_EQ(6, writer.CalcChecksum("123"));
  EXPECT_EQ(8, writer.CalcChecksum("123456789012"));
  EXPECT_EQ(0, writer.CalcChecksum("777666555444"));
}

}  // namespace
