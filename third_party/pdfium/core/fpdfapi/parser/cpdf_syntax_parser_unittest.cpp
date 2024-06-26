// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits>
#include <string>

#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fpdfapi/parser/cpdf_parser.h"
#include "core/fpdfapi/parser/cpdf_syntax_parser.h"
#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "core/fxcrt/fx_extension.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/utils/path_service.h"

TEST(cpdf_syntax_parser, ReadHexString) {
  {
    // Empty string.
    static const uint8_t data[] = "";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 0)));
    EXPECT_EQ("", parser.ReadHexString());
    EXPECT_EQ(0, parser.GetPos());
  }

  {
    // Blank string.
    static const uint8_t data[] = "  ";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 2)));
    EXPECT_EQ("", parser.ReadHexString());
    EXPECT_EQ(2, parser.GetPos());
  }

  {
    // Skips unknown characters.
    static const uint8_t data[] = "z12b";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 4)));
    EXPECT_EQ("\x12\xb0", parser.ReadHexString());
    EXPECT_EQ(4, parser.GetPos());
  }

  {
    // Skips unknown characters.
    static const uint8_t data[] = "*<&*#$^&@1";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 10)));
    EXPECT_EQ("\x10", parser.ReadHexString());
    EXPECT_EQ(10, parser.GetPos());
  }

  {
    // Skips unknown characters.
    static const uint8_t data[] = "\x80zab";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 4)));
    EXPECT_EQ("\xab", parser.ReadHexString());
    EXPECT_EQ(4, parser.GetPos());
  }

  {
    // Skips unknown characters.
    static const uint8_t data[] = "\xffzab";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 4)));
    EXPECT_EQ("\xab", parser.ReadHexString());
    EXPECT_EQ(4, parser.GetPos());
  }

  {
    // Regular conversion.
    static const uint8_t data[] = "1A2b>abcd";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 9)));
    EXPECT_EQ("\x1a\x2b", parser.ReadHexString());
    EXPECT_EQ(5, parser.GetPos());
  }

  {
    // Position out of bounds.
    static const uint8_t data[] = "12ab>";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 5)));
    parser.SetPos(5);
    EXPECT_EQ("", parser.ReadHexString());

    parser.SetPos(6);
    EXPECT_EQ("", parser.ReadHexString());

    parser.SetPos(-1);
    EXPECT_EQ("", parser.ReadHexString());

    parser.SetPos(std::numeric_limits<FX_FILESIZE>::max());
    EXPECT_EQ("", parser.ReadHexString());

    // Check string still parses when set to 0.
    parser.SetPos(0);
    EXPECT_EQ("\x12\xab", parser.ReadHexString());
  }

  {
    // Missing ending >.
    static const uint8_t data[] = "1A2b";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 4)));
    EXPECT_EQ("\x1a\x2b", parser.ReadHexString());
    EXPECT_EQ(4, parser.GetPos());
  }

  {
    // Missing ending >.
    static const uint8_t data[] = "12abz";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 5)));
    EXPECT_EQ("\x12\xab", parser.ReadHexString());
    EXPECT_EQ(5, parser.GetPos());
  }

  {
    // Uneven number of bytes.
    static const uint8_t data[] = "1A2>asdf";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 8)));
    EXPECT_EQ("\x1a\x20", parser.ReadHexString());
    EXPECT_EQ(4, parser.GetPos());
  }

  {
    // Uneven number of bytes.
    static const uint8_t data[] = "1A2zasdf";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 8)));
    EXPECT_EQ("\x1a\x2a\xdf", parser.ReadHexString());
    EXPECT_EQ(8, parser.GetPos());
  }

  {
    // Just ending character.
    static const uint8_t data[] = ">";
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::make_span(data, 1)));
    EXPECT_EQ("", parser.ReadHexString());
    EXPECT_EQ(1, parser.GetPos());
  }
}

TEST(cpdf_syntax_parser, GetInvalidReference) {
  // Data with a reference with number CPDF_Object::kInvalidObjNum
  static const uint8_t data[] = "4294967295 0 R";
  CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
      pdfium::make_span(data, 14)));
  std::unique_ptr<CPDF_Object> ref = parser.GetObjectBody(nullptr);
  EXPECT_FALSE(ref);
}

TEST(cpdf_syntax_parser, PeekNextWord) {
  static const uint8_t data[] = "    WORD ";
  CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(data));
  EXPECT_EQ("WORD", parser.PeekNextWord(nullptr));
  EXPECT_EQ("WORD", parser.GetNextWord(nullptr));
}
