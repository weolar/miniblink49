// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/fpdf_parser_utility.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

TEST(fpdf_parser_utility, PDF_NameDecode) {
  EXPECT_EQ("", PDF_NameDecode(""));
  EXPECT_EQ("A", PDF_NameDecode("A"));
  EXPECT_EQ("#", PDF_NameDecode("#"));
  EXPECT_EQ("#4", PDF_NameDecode("#4"));
  EXPECT_EQ("A", PDF_NameDecode("#41"));
  EXPECT_EQ("A1", PDF_NameDecode("#411"));
}

TEST(fpdf_parser_utility, PDF_NameEncode) {
  EXPECT_EQ("", PDF_NameEncode(""));
  EXPECT_EQ("A", PDF_NameEncode("A"));
  EXPECT_EQ("#23", PDF_NameEncode("#"));
  EXPECT_EQ("#20", PDF_NameEncode(" "));
  EXPECT_EQ("!@#23$#25^&*#28#29#3C#3E#5B#5D", PDF_NameEncode("!@#$%^&*()<>[]"));
  EXPECT_EQ("#C2", PDF_NameEncode("\xc2"));
  EXPECT_EQ("f#C2#A5", PDF_NameEncode("f\xc2\xa5"));
}
