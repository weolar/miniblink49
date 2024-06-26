// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/parser/xfa_basic_data.h"

#include <utility>

#include "testing/gtest/include/gtest/gtest.h"

TEST(XFABasicDataTest, GetPacketByName) {
  Optional<XFA_PACKETINFO> result = XFA_GetPacketByName(L"");
  EXPECT_FALSE(result.has_value());

  result = XFA_GetPacketByName(L"nonesuch");
  EXPECT_FALSE(result.has_value());

  result = XFA_GetPacketByName(L"datasets");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(XFA_PacketType::Datasets, result.value().packet_type);

  result = XFA_GetPacketByName(L"sourceSet");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(XFA_PacketType::SourceSet, result.value().packet_type);
}

TEST(XFABasicDataTest, PacketToName) {
  XFA_PACKETINFO result = XFA_GetPacketByIndex(XFA_PacketType::Datasets);
  EXPECT_STREQ(L"datasets", result.name);

  result = XFA_GetPacketByIndex(XFA_PacketType::ConnectionSet);
  EXPECT_STREQ(L"connectionSet", result.name);
}

TEST(XFABasicDataTest, GetElementByName) {
  EXPECT_EQ(XFA_Element::Unknown, XFA_GetElementByName(L""));
  EXPECT_EQ(XFA_Element::Unknown, XFA_GetElementByName(L"nonesuch"));
  EXPECT_EQ(XFA_Element::ConnectionSet, XFA_GetElementByName(L"connectionSet"));
  EXPECT_EQ(XFA_Element::Items, XFA_GetElementByName(L"items"));

  // Internal elements are not retrievable by name.
  EXPECT_EQ(XFA_Element::Unknown, XFA_GetElementByName(L"model"));
}

TEST(XFABasicDataTest, ElementToName) {
  EXPECT_EQ("conformance", XFA_ElementToName(XFA_Element::Conformance));
  EXPECT_EQ("tagged", XFA_ElementToName(XFA_Element::Tagged));

  // Internal elements resolve back to real names.
  EXPECT_EQ("node", XFA_ElementToName(XFA_Element::Node));
}

TEST(XFABasicDataTest, GetAttributeByName) {
  Optional<XFA_ATTRIBUTEINFO> result = XFA_GetAttributeByName(L"");
  EXPECT_FALSE(result.has_value());

  result = XFA_GetAttributeByName(L"nonesuch");
  EXPECT_FALSE(result.has_value());

  result = XFA_GetAttributeByName(L"h");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(XFA_Attribute::H, result.value().attribute);

  result = XFA_GetAttributeByName(L"short");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(XFA_Attribute::Short, result.value().attribute);

  result = XFA_GetAttributeByName(L"decipherOnly");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(XFA_Attribute::DecipherOnly, result.value().attribute);
}

TEST(XFABasicDataTest, AttributeToName) {
  EXPECT_EQ("spaceBelow", XFA_AttributeToName(XFA_Attribute::SpaceBelow));
  EXPECT_EQ("decipherOnly", XFA_AttributeToName(XFA_Attribute::DecipherOnly));
}

TEST(XFABasicDataTest, GetAttributeValueByName) {
  Optional<XFA_AttributeValue> result = XFA_GetAttributeValueByName(L"");
  EXPECT_FALSE(result.has_value());

  result = XFA_GetAttributeValueByName(L"nonesuch");
  EXPECT_FALSE(result.has_value());

  result = XFA_GetAttributeValueByName(L"*");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(XFA_AttributeValue::Asterisk, result.value());

  result = XFA_GetAttributeValueByName(L"visible");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(XFA_AttributeValue::Visible, result.value());

  result = XFA_GetAttributeValueByName(L"lowered");
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(XFA_AttributeValue::Lowered, result.value());
}

TEST(XFABasicDataTest, AttributeValueToName) {
  EXPECT_EQ("rl-tb", XFA_AttributeValueToName(XFA_AttributeValue::Rl_tb));
  EXPECT_EQ("lowered", XFA_AttributeValueToName(XFA_AttributeValue::Lowered));
}
