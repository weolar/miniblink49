// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/css/cfx_cssdeclaration.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

TEST(CFX_CSSDeclarationTest, HexEncodingParsing) {
  FX_ARGB color;

  // Length value invalid.
  EXPECT_FALSE(CFX_CSSDeclaration::ParseCSSColor(L"#000", 3, &color));
  EXPECT_FALSE(CFX_CSSDeclaration::ParseCSSColor(L"#000000", 5, &color));
  EXPECT_FALSE(CFX_CSSDeclaration::ParseCSSColor(L"#000000", 8, &color));

  // Invalid characters
  EXPECT_TRUE(CFX_CSSDeclaration::ParseCSSColor(L"#zxytlm", 7, &color));
  EXPECT_EQ(0, FXARGB_R(color));
  EXPECT_EQ(0, FXARGB_G(color));
  EXPECT_EQ(0, FXARGB_B(color));

  EXPECT_TRUE(CFX_CSSDeclaration::ParseCSSColor(L"#000", 4, &color));
  EXPECT_EQ(0, FXARGB_R(color));
  EXPECT_EQ(0, FXARGB_G(color));
  EXPECT_EQ(0, FXARGB_B(color));

  EXPECT_TRUE(CFX_CSSDeclaration::ParseCSSColor(L"#FFF", 4, &color));
  EXPECT_EQ(255, FXARGB_R(color));
  EXPECT_EQ(255, FXARGB_G(color));
  EXPECT_EQ(255, FXARGB_B(color));

  EXPECT_TRUE(CFX_CSSDeclaration::ParseCSSColor(L"#F0F0F0", 7, &color));
  EXPECT_EQ(240, FXARGB_R(color));
  EXPECT_EQ(240, FXARGB_G(color));
  EXPECT_EQ(240, FXARGB_B(color));

  // Upper and lower case characters.
  EXPECT_TRUE(CFX_CSSDeclaration::ParseCSSColor(L"#1b2F3c", 7, &color));
  EXPECT_EQ(27, FXARGB_R(color));
  EXPECT_EQ(47, FXARGB_G(color));
  EXPECT_EQ(60, FXARGB_B(color));
}

TEST(CFX_CSSDeclarationTest, RGBEncodingParsing) {
  FX_ARGB color;

  // Invalid input for rgb() syntax.
  EXPECT_FALSE(CFX_CSSDeclaration::ParseCSSColor(L"blahblahblah", 11, &color));

  EXPECT_TRUE(CFX_CSSDeclaration::ParseCSSColor(L"rgb(0, 0, 0)", 12, &color));
  EXPECT_EQ(0, FXARGB_R(color));
  EXPECT_EQ(0, FXARGB_G(color));
  EXPECT_EQ(0, FXARGB_B(color));

  EXPECT_TRUE(
      CFX_CSSDeclaration::ParseCSSColor(L"rgb(128,255,48)", 15, &color));
  EXPECT_EQ(128, FXARGB_R(color));
  EXPECT_EQ(255, FXARGB_G(color));
  EXPECT_EQ(48, FXARGB_B(color));
}
