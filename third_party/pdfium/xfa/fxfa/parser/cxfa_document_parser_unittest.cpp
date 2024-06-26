// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/parser/cxfa_document_parser.h"

#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_document.h"

class CXFA_DocumentParserTest : public testing::Test {
 public:
  void SetUp() override {
    doc_ = pdfium::MakeUnique<CXFA_Document>(nullptr);
    parser_ = pdfium::MakeUnique<CXFA_DocumentParser>(doc_.get());
  }

  void TearDown() override {
    // Hold the XML tree until we cleanup the document.
    std::unique_ptr<CFX_XMLDocument> doc = parser_->GetXMLDoc();
    parser_ = nullptr;
    doc_ = nullptr;
  }

  CXFA_Document* GetDoc() const { return doc_.get(); }
  CXFA_DocumentParser* GetParser() const { return parser_.get(); }

 private:
  std::unique_ptr<CXFA_Document> doc_;
  std::unique_ptr<CXFA_DocumentParser> parser_;
};

TEST_F(CXFA_DocumentParserTest, XMLInstructionsScriptOff) {
  static const char input[] =
      "<config>\n"
      "<?originalXFAVersion http://www.xfa.org/schema/xfa-template/2.7 "
      "v2.7-scripting:0 ?>\n"
      "</config>";
  EXPECT_FALSE(GetDoc()->HasFlag(XFA_DOCFLAG_Scripting));

  auto stream = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
      pdfium::as_bytes(pdfium::make_span(input)));
  ASSERT_TRUE(GetParser()->Parse(stream, XFA_PacketType::Config));

  CXFA_Node* root = GetParser()->GetRootNode();
  ASSERT_TRUE(root != nullptr);
  EXPECT_FALSE(GetDoc()->HasFlag(XFA_DOCFLAG_Scripting));
}

TEST_F(CXFA_DocumentParserTest, XMLInstructionsScriptOn) {
  static const char input[] =
      "<config>\n"
      "<?originalXFAVersion http://www.xfa.org/schema/xfa-template/2.7 "
      "v2.7-scripting:1 ?>\n"
      "</config>";

  EXPECT_FALSE(GetDoc()->HasFlag(XFA_DOCFLAG_Scripting));

  auto stream = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
      pdfium::as_bytes(pdfium::make_span(input)));
  ASSERT_TRUE(GetParser()->Parse(stream, XFA_PacketType::Config));

  CXFA_Node* root = GetParser()->GetRootNode();
  ASSERT_TRUE(root != nullptr);
  EXPECT_TRUE(GetDoc()->HasFlag(XFA_DOCFLAG_Scripting));
}

TEST_F(CXFA_DocumentParserTest, XMLInstructionsStrictScope) {
  static const char input[] =
      "<config>"
      "<?acrobat JavaScript strictScoping ?>\n"
      "</config>";

  EXPECT_FALSE(GetDoc()->HasFlag(XFA_DOCFLAG_StrictScoping));

  auto stream = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
      pdfium::as_bytes(pdfium::make_span(input)));
  ASSERT_TRUE(GetParser()->Parse(stream, XFA_PacketType::Config));

  CXFA_Node* root = GetParser()->GetRootNode();
  ASSERT_TRUE(root != nullptr);
  EXPECT_TRUE(GetDoc()->HasFlag(XFA_DOCFLAG_StrictScoping));
}

TEST_F(CXFA_DocumentParserTest, XMLInstructionsStrictScopeBad) {
  static const char input[] =
      "<config>"
      "<?acrobat JavaScript otherScoping ?>\n"
      "</config>";

  EXPECT_FALSE(GetDoc()->HasFlag(XFA_DOCFLAG_StrictScoping));

  auto stream = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
      pdfium::as_bytes(pdfium::make_span(input)));
  ASSERT_TRUE(GetParser()->Parse(stream, XFA_PacketType::Config));

  CXFA_Node* root = GetParser()->GetRootNode();
  ASSERT_TRUE(root != nullptr);
  EXPECT_FALSE(GetDoc()->HasFlag(XFA_DOCFLAG_StrictScoping));
}

TEST_F(CXFA_DocumentParserTest, MultipleXMLInstructions) {
  static const char input[] =
      "<config>"
      "<?originalXFAVersion http://www.xfa.org/schema/xfa-template/2.7 "
      "v2.7-scripting:1 ?>\n"
      "<?acrobat JavaScript strictScoping ?>\n"
      "</config>";

  EXPECT_FALSE(GetDoc()->HasFlag(XFA_DOCFLAG_Scripting));
  EXPECT_FALSE(GetDoc()->HasFlag(XFA_DOCFLAG_StrictScoping));

  auto stream = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
      pdfium::as_bytes(pdfium::make_span(input)));
  ASSERT_TRUE(GetParser()->Parse(stream, XFA_PacketType::Config));

  CXFA_Node* root = GetParser()->GetRootNode();
  ASSERT_TRUE(root != nullptr);

  EXPECT_TRUE(GetDoc()->HasFlag(XFA_DOCFLAG_Scripting));
  EXPECT_TRUE(GetDoc()->HasFlag(XFA_DOCFLAG_StrictScoping));
}
