// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/cxfa_textparser.h"

#include "testing/gtest/include/gtest/gtest.h"

class CXFA_TestTextParser final : public CXFA_TextParser {
 public:
  CXFA_TestTextParser() : CXFA_TextParser() {}

 private:
  // Add test cases as friends to access protected member functions.
  FRIEND_TEST(CXFA_TextParser, TagValidate);
};

TEST(CXFA_TextParser, TagValidate) {
  CXFA_TestTextParser parser;
  EXPECT_TRUE(parser.TagValidate(L"br"));
  EXPECT_TRUE(parser.TagValidate(L"Br"));
  EXPECT_TRUE(parser.TagValidate(L"BR"));
  EXPECT_TRUE(parser.TagValidate(L"a"));
  EXPECT_TRUE(parser.TagValidate(L"b"));
  EXPECT_TRUE(parser.TagValidate(L"i"));
  EXPECT_TRUE(parser.TagValidate(L"p"));
  EXPECT_TRUE(parser.TagValidate(L"li"));
  EXPECT_TRUE(parser.TagValidate(L"ol"));
  EXPECT_TRUE(parser.TagValidate(L"ul"));
  EXPECT_TRUE(parser.TagValidate(L"sub"));
  EXPECT_TRUE(parser.TagValidate(L"sup"));
  EXPECT_TRUE(parser.TagValidate(L"span"));
  EXPECT_TRUE(parser.TagValidate(L"body"));
  EXPECT_TRUE(parser.TagValidate(L"html"));

  EXPECT_FALSE(parser.TagValidate(L""));
  EXPECT_FALSE(parser.TagValidate(L"tml"));
  EXPECT_FALSE(parser.TagValidate(L"xhtml"));
  EXPECT_FALSE(parser.TagValidate(L"htmlx"));
}
