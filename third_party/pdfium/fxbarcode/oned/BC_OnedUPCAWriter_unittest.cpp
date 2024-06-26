// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxbarcode/oned/BC_OnedUPCAWriter.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace {

TEST(OnedUPCAWriterTest, Encode) {
  CBC_OnedUPCAWriter writer;
  writer.InitEANWriter();
  int32_t width;
  int32_t height;

  // UPCA barcodes encode 12-digit numbers into 95 modules in a unidimensional
  // disposition.
  uint8_t* encoded = writer.Encode("", BCFORMAT_UPC_A, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("123", BCFORMAT_UPC_A, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("12345678901", BCFORMAT_UPC_A, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("1234567890123", BCFORMAT_UPC_A, width, height);
  EXPECT_EQ(nullptr, encoded);
  FX_Free(encoded);

  encoded = writer.Encode("123456789012", BCFORMAT_UPC_A, width, height);
  const char* expected =
      "# #"      // Start
      "  ##  #"  // 1 L
      "  #  ##"  // 2 L
      " #### #"  // 3 L
      " #   ##"  // 4 L
      " ##   #"  // 5 L
      " # ####"  // 6 L
      " # # "    // Middle
      "#   #  "  // 7 R
      "#  #   "  // 8 R
      "### #  "  // 9 R
      "###  # "  // 0 R
      "##  ## "  // 1 R
      "## ##  "  // 2 R
      "# #";     // End
  EXPECT_NE(nullptr, encoded);
  EXPECT_EQ(1, height);
  EXPECT_EQ(static_cast<int32_t>(strlen(expected)), width);
  for (size_t i = 0; i < strlen(expected); i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);

  encoded = writer.Encode("777666555440", BCFORMAT_UPC_A, width, height);
  expected =
      "# #"      // Start
      " ### ##"  // 7 L
      " ### ##"  // 7 L
      " ### ##"  // 7 L
      " # ####"  // 6 L
      " # ####"  // 6 L
      " # ####"  // 6 L
      " # # "    // Middle
      "#  ### "  // 5 R
      "#  ### "  // 5 R
      "#  ### "  // 5 R
      "# ###  "  // 4 R
      "# ###  "  // 4 R
      "###  # "  // 0 R
      "# #";     // End
  EXPECT_NE(nullptr, encoded);
  EXPECT_EQ(1, height);
  EXPECT_EQ(static_cast<int32_t>(strlen(expected)), width);
  for (size_t i = 0; i < strlen(expected); i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);
}

TEST(OnedUPCAWriterTest, Checksum) {
  CBC_OnedUPCAWriter writer;
  writer.InitEANWriter();
  EXPECT_EQ(0, writer.CalcChecksum(""));
  EXPECT_EQ(6, writer.CalcChecksum("123"));
  EXPECT_EQ(2, writer.CalcChecksum("12345678901"));
  EXPECT_EQ(0, writer.CalcChecksum("77766655544"));
}

}  // namespace
