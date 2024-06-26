// Copyright 2019 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/font/cpdf_cidfont.h"

#include <utility>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "testing/gtest/include/gtest/gtest.h"

class CPDF_CIDFontTest : public testing::Test {
 protected:
  void SetUp() override { CPDF_ModuleMgr::Get()->Init(); }

  void TearDown() override { CPDF_ModuleMgr::Destroy(); }
};

TEST_F(CPDF_CIDFontTest, BUG_920636) {
  CPDF_Document doc;
  CPDF_Dictionary font_dict;
  font_dict.SetNewFor<CPDF_Name>("Encoding", "Identity−H");

  {
    auto descendant_fonts = pdfium::MakeUnique<CPDF_Array>();
    {
      auto descendant_font = pdfium::MakeUnique<CPDF_Dictionary>();
      descendant_font->SetNewFor<CPDF_Name>("BaseFont", "CourierStd");
      descendant_fonts->Add(std::move(descendant_font));
    }
    font_dict.SetFor("DescendantFonts", std::move(descendant_fonts));
  }

  CPDF_CIDFont font(&doc, &font_dict);
  ASSERT_TRUE(font.Load());

  // It would be nice if we can test more values here. However, the glyph
  // indices are sometimes machine dependent.
  struct {
    uint32_t charcode;
    int glyph;
  } static constexpr kTestCases[] = {
      {0, 31},
      {256, 287},
      {34661, 34692},
  };

  for (const auto& test_case : kTestCases) {
    EXPECT_EQ(test_case.glyph,
              font.GlyphFromCharCode(test_case.charcode, nullptr));
  }
}
