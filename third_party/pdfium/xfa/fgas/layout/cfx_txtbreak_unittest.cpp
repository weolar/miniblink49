// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fgas/layout/cfx_txtbreak.h"

#include <memory>
#include <utility>

#include "core/fxge/cfx_font.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fgas/font/cfgas_fontmgr.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fgas/layout/cfx_char.h"

class CFX_TxtBreakTest : public testing::Test {
 public:
  void SetUp() override {
    font_ =
        CFGAS_GEFont::LoadFont(L"Arial Black", 0, 0, GetGlobalFontManager());
    ASSERT_TRUE(font_.Get());
  }

  std::unique_ptr<CFX_TxtBreak> CreateBreak() {
    auto b = pdfium::MakeUnique<CFX_TxtBreak>();
    b->SetFont(font_);
    return b;
  }

 private:
  RetainPtr<CFGAS_GEFont> font_;
};

TEST_F(CFX_TxtBreakTest, BidiLine) {
  auto txt_break = CreateBreak();
  txt_break->SetLineBreakTolerance(1);
  txt_break->SetFontSize(12);

  WideString input = WideString::FromUTF8(ByteStringView("\xa\x0\xa\xa", 4));
  for (auto& ch : input)
    txt_break->AppendChar(ch);

  auto chars = txt_break->GetCurrentLineForTesting()->m_LineChars;
  CFX_Char::BidiLine(&chars, chars.size());
  EXPECT_EQ(3u, chars.size());
}
