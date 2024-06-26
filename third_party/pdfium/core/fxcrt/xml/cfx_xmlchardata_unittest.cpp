// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/xml/cfx_xmlchardata.h"
#include "core/fxcrt/xml/cfx_xmldocument.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/string_write_stream.h"
#include "testing/test_support.h"

TEST(CFX_XMLCharDataTest, GetType) {
  CFX_XMLCharData data(L"My Data");
  EXPECT_EQ(FX_XMLNODE_CharData, data.GetType());
}

TEST(CFX_XMLCharDataTest, GetText) {
  CFX_XMLCharData data(L"My Data");
  EXPECT_EQ(L"My Data", data.GetText());
}

TEST(CFX_XMLCharDataTest, Clone) {
  CFX_XMLDocument doc;

  CFX_XMLCharData data(L"My Data");
  CFX_XMLNode* clone = data.Clone(&doc);
  EXPECT_TRUE(clone != nullptr);
  EXPECT_NE(&data, clone);
  ASSERT_EQ(FX_XMLNODE_CharData, clone->GetType());
  EXPECT_EQ(L"My Data", ToXMLCharData(clone)->GetText());
}

TEST(CFX_XMLCharDataTest, Save) {
  auto stream = pdfium::MakeRetain<StringWriteStream>();
  CFX_XMLCharData data(L"My Data");
  data.Save(stream);
  EXPECT_EQ("<![CDATA[My Data]]>", stream->ToString());
}
