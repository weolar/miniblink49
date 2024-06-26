// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxbarcode/oned/BC_OnedCodaBarWriter.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace {

// 3 wide and 4 narrow modules per delimiter. One space between them.
constexpr int kModulesForDelimiters = (3 * 2 + 4) * 2 + 1;

// 2 wide and 5 narrow modules per number, '_' or '$'. 1 space between chars.
constexpr int kModulesPerNumber = 2 * 2 + 5 + 1;

// 3 wide and 4 narrow modules per number, '_' or '$'. 1 space between chars.
constexpr int kModulesPerPunctuation = 3 * 2 + 4 + 1;

TEST(OnedCodaBarWriterTest, Encode) {
  CBC_OnedCodaBarWriter writer;
  int32_t width;
  int32_t height;

  uint8_t* encoded = writer.Encode("", BCFORMAT_CODABAR, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ(kModulesForDelimiters, width);
  const char* expected =
      "# ##  #  # "  // A Start
      "#  #  # ##";  // B End
  for (size_t i = 0; i < strlen(expected); i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);

  encoded = writer.Encode("123", BCFORMAT_CODABAR, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ(kModulesForDelimiters + 3 * kModulesPerNumber, width);
  expected =
      "# ##  #  # "  // A Start
      "# # ##  # "   // 1
      "# #  # ## "   // 2
      "##  # # # "   // 3
      "#  #  # ##";  // B End
  for (size_t i = 0; i < strlen(expected); i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);

  encoded = writer.Encode("-$./:+", BCFORMAT_CODABAR, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ(kModulesForDelimiters + 2 * kModulesPerNumber +
                4 * kModulesPerPunctuation,
            width);
  expected =
      "# ##  #  # "  // A Start
      "# #  ## # "   // -
      "# ##  # # "   // $
      "## ## ## # "  // .
      "## ## # ## "  // /
      "## # ## ## "  // :
      "# ## ## ## "  // +
      "#  #  # ##";  // B End
  for (size_t i = 0; i < strlen(expected); i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);

  encoded = writer.Encode("456.987987987/001", BCFORMAT_CODABAR, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ(kModulesForDelimiters + 15 * kModulesPerNumber +
                2 * kModulesPerPunctuation,
            width);
  expected =
      "# ##  #  # "  // A Start
      "# ## #  # "   // 4
      "## # #  # "   // 5
      "#  # # ## "   // 6
      "## ## ## # "  // .
      "## #  # # "   // 9
      "#  ## # # "   // 8
      "#  # ## # "   // 7
      "## #  # # "   // 9
      "#  ## # # "   // 8
      "#  # ## # "   // 7
      "## #  # # "   // 9
      "#  ## # # "   // 8
      "#  # ## # "   // 7
      "## ## # ## "  // /
      "# # #  ## "   // 0
      "# # #  ## "   // 0
      "# # ##  # "   // 1
      "#  #  # ##";  // B End
  for (size_t i = 0; i < strlen(expected); i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);
}

TEST(OnedCodaBarWriterTest, SetDelimiters) {
  CBC_OnedCodaBarWriter writer;
  int32_t width;
  int32_t height;

  EXPECT_TRUE(writer.SetStartChar('A'));
  EXPECT_TRUE(writer.SetStartChar('B'));
  EXPECT_TRUE(writer.SetStartChar('C'));
  EXPECT_TRUE(writer.SetStartChar('D'));
  EXPECT_TRUE(writer.SetStartChar('E'));
  EXPECT_TRUE(writer.SetStartChar('N'));
  EXPECT_TRUE(writer.SetStartChar('T'));
  EXPECT_TRUE(writer.SetStartChar('*'));
  EXPECT_FALSE(writer.SetStartChar('V'));
  EXPECT_FALSE(writer.SetStartChar('0'));
  EXPECT_FALSE(writer.SetStartChar('\0'));
  EXPECT_FALSE(writer.SetStartChar('@'));

  EXPECT_TRUE(writer.SetEndChar('A'));
  EXPECT_TRUE(writer.SetEndChar('B'));
  EXPECT_TRUE(writer.SetEndChar('C'));
  EXPECT_TRUE(writer.SetEndChar('D'));
  EXPECT_TRUE(writer.SetEndChar('E'));
  EXPECT_TRUE(writer.SetEndChar('N'));
  EXPECT_TRUE(writer.SetEndChar('T'));
  EXPECT_TRUE(writer.SetEndChar('*'));
  EXPECT_FALSE(writer.SetEndChar('V'));
  EXPECT_FALSE(writer.SetEndChar('0'));
  EXPECT_FALSE(writer.SetEndChar('\0'));
  EXPECT_FALSE(writer.SetEndChar('@'));

  writer.SetStartChar('N');
  writer.SetEndChar('*');

  uint8_t* encoded = writer.Encode("987", BCFORMAT_CODABAR, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ(kModulesForDelimiters + 3 * kModulesPerNumber, width);
  const char* expected =
      "#  #  # ## "  // N (same as B) Start
      "## #  # # "   // 9
      "#  ## # # "   // 8
      "#  # ## # "   // 7
      "# #  #  ##";  // * (same as C) End
  for (size_t i = 0; i < strlen(expected); i++) {
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  }
  FX_Free(encoded);
}

}  // namespace
