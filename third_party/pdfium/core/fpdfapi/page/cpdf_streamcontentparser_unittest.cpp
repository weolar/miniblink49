// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/page/cpdf_streamcontentparser.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(cpdf_streamcontentparser, PDF_FindKeyAbbreviation) {
  EXPECT_EQ(ByteStringView("BitsPerComponent"),
            CPDF_StreamContentParser::FindKeyAbbreviationForTesting(
                ByteStringView("BPC")));
  EXPECT_EQ(ByteStringView("Width"),
            CPDF_StreamContentParser::FindKeyAbbreviationForTesting(
                ByteStringView("W")));
  EXPECT_EQ(ByteStringView(""),
            CPDF_StreamContentParser::FindKeyAbbreviationForTesting(
                ByteStringView("")));
  EXPECT_EQ(ByteStringView(""),
            CPDF_StreamContentParser::FindKeyAbbreviationForTesting(
                ByteStringView("NoInList")));
  // Prefix should not match.
  EXPECT_EQ(ByteStringView(""),
            CPDF_StreamContentParser::FindKeyAbbreviationForTesting(
                ByteStringView("WW")));
}

TEST(cpdf_streamcontentparser, PDF_FindValueAbbreviation) {
  EXPECT_EQ(ByteStringView("DeviceGray"),
            CPDF_StreamContentParser::FindValueAbbreviationForTesting(
                ByteStringView("G")));
  EXPECT_EQ(ByteStringView("DCTDecode"),
            CPDF_StreamContentParser::FindValueAbbreviationForTesting(
                ByteStringView("DCT")));
  EXPECT_EQ(ByteStringView(""),
            CPDF_StreamContentParser::FindValueAbbreviationForTesting(
                ByteStringView("")));
  EXPECT_EQ(ByteStringView(""),
            CPDF_StreamContentParser::FindValueAbbreviationForTesting(
                ByteStringView("NoInList")));
  // Prefix should not match.
  EXPECT_EQ(ByteStringView(""),
            CPDF_StreamContentParser::FindValueAbbreviationForTesting(
                ByteStringView("II")));
}
