// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxbarcode/oned/BC_OnedCode39Writer.h"

#include <cstring>

#include "testing/gtest/include/gtest/gtest.h"

namespace {

// 3 wide and 6 narrow modules per char. 1 space between chars.
constexpr int MODULES_PER_CHAR = 3 * 3 + 6 + 1;

// '*' is added as the first and last char.
const int DELIMITER_CHARS = 2;

// Last char may serve as checksum.
const int CHECKSUM_CHARS = 1;

TEST(OnedCode39WriterTest, SetWideNarrowRatio) {
  // Code 39 barcodes encode strings of any size into modules in a
  // unidimensional disposition.
  // Each module is either: a narrow bar, a narrow space, a wide
  // bar, or a wide space. Accepted wide-to-narrow ratios are between 2 and 3.
  // This writer in particular only takes integer ratios, so it's either 2 or 3.
  CBC_OnedCode39Writer writer;
  EXPECT_FALSE(writer.SetWideNarrowRatio(0));
  EXPECT_FALSE(writer.SetWideNarrowRatio(1));
  EXPECT_TRUE(writer.SetWideNarrowRatio(2));
  EXPECT_TRUE(writer.SetWideNarrowRatio(3));
  EXPECT_FALSE(writer.SetWideNarrowRatio(4));
  EXPECT_FALSE(writer.SetWideNarrowRatio(100));

  writer.SetWideNarrowRatio(3);

  int32_t width;
  int32_t height;
  uint8_t* encoded;
  const char* expected;

  encoded = writer.Encode("PDFIUM", BCFORMAT_CODE_39, width, height);
  expected =
      "#   # ### ### # "  // * Start
      "# ### ### #   # "  // P
      "# # ###   # ### "  // D
      "# ### ###   # # "  // F
      "# ### #   ### # "  // I
      "###   # # # ### "  // U
      "### ### # #   # "  // M
      "#   # ### ### #";  // * End
  for (size_t i = 0; i < strlen(expected); i++)
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  FX_Free(encoded);

  writer.SetWideNarrowRatio(2);

  encoded = writer.Encode("PDFIUM", BCFORMAT_CODE_39, width, height);
  expected =
      "#  # ## ## # "  // * Start
      "# ## ## #  # "  // P
      "# # ##  # ## "  // D
      "# ## ##  # # "  // F
      "# ## #  ## # "  // I
      "##  # # # ## "  // U
      "## ## # #  # "  // M
      "#  # ## ## #";  // * End
  for (size_t i = 0; i < strlen(expected); i++)
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  FX_Free(encoded);
}

TEST(OnedCode39WriterTest, Encode) {
  CBC_OnedCode39Writer writer;
  int32_t width;
  int32_t height;
  uint8_t* encoded;
  const char* expected;

  encoded = writer.Encode("", BCFORMAT_CODE_39, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ((0 + DELIMITER_CHARS) * MODULES_PER_CHAR - 1, width);
  expected =
      "#   # ### ### # "  // * Start
      "#   # ### ### #";  // * End
  for (size_t i = 0; i < strlen(expected); i++)
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  FX_Free(encoded);

  encoded = writer.Encode("123", BCFORMAT_CODE_39, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ((3 + DELIMITER_CHARS) * MODULES_PER_CHAR - 1, width);
  expected =
      "#   # ### ### # "  // * Start
      "### #   # # ### "  // 1
      "# ###   # # ### "  // 2
      "### ###   # # # "  // 3
      "#   # ### ### #";  // * End
  for (size_t i = 0; i < strlen(expected); i++)
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  FX_Free(encoded);

  encoded = writer.Encode("PDFIUM", BCFORMAT_CODE_39, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ((6 + DELIMITER_CHARS) * MODULES_PER_CHAR - 1, width);
  expected =
      "#   # ### ### # "  // * Start
      "# ### ### #   # "  // P
      "# # ###   # ### "  // D
      "# ### ###   # # "  // F
      "# ### #   ### # "  // I
      "###   # # # ### "  // U
      "### ### # #   # "  // M
      "#   # ### ### #";  // * End
  for (size_t i = 0; i < strlen(expected); i++)
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  FX_Free(encoded);

  encoded = writer.Encode("A -$%./+Z", BCFORMAT_CODE_39, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ((9 + DELIMITER_CHARS) * MODULES_PER_CHAR - 1, width);
  expected =
      "#   # ### ### # "  // * Start
      "### # #   # ### "  // A
      "#   ### # ### # "  // Space
      "#   # # ### ### "  // -
      "#   #   #   # # "  // $
      "# #   #   #   # "  // %
      "###   # # ### # "  // .
      "#   #   # #   # "  // /
      "#   # #   #   # "  // +
      "#   ### ### # # "  // Z
      "#   # ### ### #";  // * End
  for (size_t i = 0; i < strlen(expected); i++)
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  FX_Free(encoded);
}

TEST(OnedCode39WriterTest, Checksum) {
  CBC_OnedCode39Writer writer;
  int32_t width;
  int32_t height;
  uint8_t* encoded;
  const char* expected;

  writer.SetCalcChecksum(true);

  encoded = writer.Encode("123", BCFORMAT_CODE_39, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ((3 + CHECKSUM_CHARS + DELIMITER_CHARS) * MODULES_PER_CHAR - 1,
            width);
  expected =
      "#   # ### ### # "  // * Start
      "### #   # # ### "  // 1 (1)
      "# ###   # # ### "  // 2 (2)
      "### ###   # # # "  // 3 (3)
      "# ###   ### # # "  // 6 (6 = (1 + 2 + 3) % 43)
      "#   # ### ### #";  // * End
  for (size_t i = 0; i < strlen(expected); i++)
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  FX_Free(encoded);

  encoded = writer.Encode("PDFIUM", BCFORMAT_CODE_39, width, height);
  EXPECT_EQ(1, height);
  EXPECT_EQ((6 + CHECKSUM_CHARS + DELIMITER_CHARS) * MODULES_PER_CHAR - 1,
            width);
  expected =
      "#   # ### ### # "  // * Start
      "# ### ### #   # "  // P (25)
      "# # ###   # ### "  // D (13)
      "# ### ###   # # "  // F (15)
      "# ### #   ### # "  // I (18)
      "###   # # # ### "  // U (30)
      "### ### # #   # "  // M (22)
      "###   # # ### # "  // . (37 = (25 + 13 + 15 + 18 + 30 + 22) % 43)
      "#   # ### ### #";  // * End
  for (size_t i = 0; i < strlen(expected); i++)
    EXPECT_EQ(expected[i] != ' ', !!encoded[i]) << i;
  FX_Free(encoded);
}

}  // namespace
