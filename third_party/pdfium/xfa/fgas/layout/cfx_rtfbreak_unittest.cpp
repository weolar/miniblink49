// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/layout/cfx_rtfbreak.h"

#include <memory>
#include <utility>

#include "core/fxge/cfx_font.h"
#include "core/fxge/cfx_gemodule.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fgas/font/cfgas_fontmgr.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fgas/layout/cfx_char.h"

class CFX_RTFBreakTest : public testing::Test {
 public:
  void SetUp() override {
    font_ =
        CFGAS_GEFont::LoadFont(L"Arial Black", 0, 0, GetGlobalFontManager());
    ASSERT_TRUE(font_.Get());
  }

  std::unique_ptr<CFX_RTFBreak> CreateBreak(int32_t args) {
    auto b = pdfium::MakeUnique<CFX_RTFBreak>(args);
    b->SetFont(font_);
    return b;
  }

 private:
  RetainPtr<CFGAS_GEFont> font_;
};

// As soon as you get one of the control characters the break is complete
// and must be consumed before you get any more characters ....

TEST_F(CFX_RTFBreakTest, AddChars) {
  auto b = CreateBreak(FX_LAYOUTSTYLE_ExpandTab);

  WideString str(L"Input String.");
  for (const auto& c : str)
    EXPECT_EQ(CFX_BreakType::None, b->AppendChar(c));

  EXPECT_EQ(CFX_BreakType::Paragraph, b->AppendChar(L'\n'));
  ASSERT_EQ(1, b->CountBreakPieces());
  EXPECT_EQ(str + L"\n", b->GetBreakPieceUnstable(0)->GetString());

  b->ClearBreakPieces();
  b->Reset();
  EXPECT_EQ(0, b->GetCurrentLineForTesting()->GetLineEnd());

  str = L"Second str.";
  for (const auto& c : str)
    EXPECT_EQ(CFX_BreakType::None, b->AppendChar(c));

  // Force the end of the break at the end of the string.
  b->EndBreak(CFX_BreakType::Paragraph);
  ASSERT_EQ(1, b->CountBreakPieces());
  EXPECT_EQ(str, b->GetBreakPieceUnstable(0)->GetString());
}

TEST_F(CFX_RTFBreakTest, ControlCharacters) {
  auto b = CreateBreak(FX_LAYOUTSTYLE_ExpandTab);
  EXPECT_EQ(CFX_BreakType::Line, b->AppendChar(L'\v'));
  EXPECT_EQ(CFX_BreakType::Page, b->AppendChar(L'\f'));
  // 0x2029 is the Paragraph Separator unicode character.
  EXPECT_EQ(CFX_BreakType::Paragraph, b->AppendChar(0x2029));
  EXPECT_EQ(CFX_BreakType::Paragraph, b->AppendChar(L'\n'));

  ASSERT_EQ(1, b->CountBreakPieces());
  EXPECT_EQ(L"\v", b->GetBreakPieceUnstable(0)->GetString());
}

TEST_F(CFX_RTFBreakTest, BidiLine) {
  auto rtf_break = CreateBreak(FX_LAYOUTSTYLE_ExpandTab);
  rtf_break->SetLineBreakTolerance(1);
  rtf_break->SetFontSize(12);

  WideString input = WideString::FromUTF8(ByteStringView("\xa\x0\xa\xa", 4));
  for (auto& ch : input)
    rtf_break->AppendChar(ch);

  auto chars = rtf_break->GetCurrentLineForTesting()->m_LineChars;
  CFX_Char::BidiLine(&chars, chars.size());
  EXPECT_EQ(3u, chars.size());
}
