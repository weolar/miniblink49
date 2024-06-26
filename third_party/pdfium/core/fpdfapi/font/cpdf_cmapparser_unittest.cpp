// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/font/cpdf_cmapparser.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace {

bool uint_ranges_equal(uint8_t* a, uint8_t* b, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (a[i] != b[i])
      return false;
  }
  return true;
}

}  // namespace

TEST(cpdf_cmapparser, GetCode) {
  CPDF_CMapParser parser(nullptr);

  EXPECT_EQ(0u, parser.GetCode(""));
  EXPECT_EQ(0u, parser.GetCode("<"));
  EXPECT_EQ(194u, parser.GetCode("<c2"));
  EXPECT_EQ(162u, parser.GetCode("<A2"));
  EXPECT_EQ(2802u, parser.GetCode("<Af2"));
  EXPECT_EQ(162u, parser.GetCode("<A2z"));

  EXPECT_EQ(12u, parser.GetCode("12"));
  EXPECT_EQ(12u, parser.GetCode("12d"));
  EXPECT_EQ(128u, parser.GetCode("128"));

  EXPECT_EQ(4294967295u, parser.GetCode("<FFFFFFFF"));

  // Overflow a uint32_t.
  EXPECT_EQ(0u, parser.GetCode("<100000000"));
}

TEST(cpdf_cmapparser, GetCodeRange) {
  CPDF_CMapParser parser(nullptr);
  CPDF_CMap::CodeRange range;

  // Must start with a <
  EXPECT_FALSE(parser.GetCodeRange(range, "", ""));
  EXPECT_FALSE(parser.GetCodeRange(range, "A", ""));

  // m_CharSize must be <= 4
  EXPECT_FALSE(parser.GetCodeRange(range, "<aaaaaaaaaa>", ""));
  EXPECT_EQ(5u, range.m_CharSize);

  EXPECT_TRUE(parser.GetCodeRange(range, "<12345678>", "<87654321>"));
  EXPECT_EQ(4u, range.m_CharSize);
  {
    uint8_t lower[4] = {18, 52, 86, 120};
    uint8_t upper[4] = {135, 101, 67, 33};
    EXPECT_TRUE(uint_ranges_equal(lower, range.m_Lower, range.m_CharSize));
    EXPECT_TRUE(uint_ranges_equal(upper, range.m_Upper, range.m_CharSize));
  }

  // Hex characters
  EXPECT_TRUE(parser.GetCodeRange(range, "<a1>", "<F3>"));
  EXPECT_EQ(1u, range.m_CharSize);
  EXPECT_EQ(161, range.m_Lower[0]);
  EXPECT_EQ(243, range.m_Upper[0]);

  // The second string should return 0's if it is shorter
  EXPECT_TRUE(parser.GetCodeRange(range, "<a1>", ""));
  EXPECT_EQ(1u, range.m_CharSize);
  EXPECT_EQ(161, range.m_Lower[0]);
  EXPECT_EQ(0, range.m_Upper[0]);
}
