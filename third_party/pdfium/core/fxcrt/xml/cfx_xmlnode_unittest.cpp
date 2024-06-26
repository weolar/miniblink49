// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/xml/cfx_xmlelement.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

TEST(CFX_XMLNodeTest, GetParent) {
  CFX_XMLElement node1(L"node");
  CFX_XMLElement node2(L"node2");
  CFX_XMLElement node3(L"node3");

  node1.AppendChild(&node2);
  node2.AppendChild(&node3);

  EXPECT_EQ(nullptr, node1.GetParent());
  EXPECT_EQ(&node1, node2.GetParent());
  EXPECT_EQ(&node2, node3.GetParent());
}

TEST(CFX_XMLNodeTest, GetRoot) {
  CFX_XMLElement node1(L"node");
  CFX_XMLElement node2(L"node2");
  CFX_XMLElement node3(L"node3");

  node1.AppendChild(&node2);
  node2.AppendChild(&node3);

  EXPECT_EQ(&node1, node1.GetRoot());
  EXPECT_EQ(&node1, node2.GetRoot());
  EXPECT_EQ(&node1, node3.GetRoot());
}

TEST(CFX_XMLNodeTest, GetChildren) {
  CFX_XMLElement node1(L"node");
  CFX_XMLElement node2(L"node2");
  CFX_XMLElement node3(L"node3");
  CFX_XMLElement node4(L"node4");

  node1.AppendChild(&node2);
  node1.AppendChild(&node4);
  node2.AppendChild(&node3);

  EXPECT_EQ(&node2, node1.GetFirstChild());

  EXPECT_EQ(&node4, node2.GetNextSibling());
  EXPECT_EQ(&node3, node2.GetFirstChild());

  EXPECT_TRUE(node3.GetNextSibling() == nullptr);
  EXPECT_TRUE(node3.GetFirstChild() == nullptr);

  EXPECT_TRUE(node4.GetNextSibling() == nullptr);
  EXPECT_TRUE(node4.GetFirstChild() == nullptr);
}

TEST(CFX_XMLNodeTest, DeleteChildren) {
  CFX_XMLElement node1(L"node");
  CFX_XMLElement node2(L"node2");
  CFX_XMLElement node3(L"node3");
  CFX_XMLElement node4(L"node4");

  node1.AppendChild(&node2);
  node1.AppendChild(&node4);
  node2.AppendChild(&node3);

  node1.DeleteChildren();
  EXPECT_TRUE(node1.GetFirstChild() == nullptr);
  EXPECT_TRUE(node2.GetParent() == nullptr);
  EXPECT_TRUE(node4.GetParent() == nullptr);

  // node2 and node4 should no longer be siblings.
  EXPECT_TRUE(node2.GetNextSibling() == nullptr);
  EXPECT_TRUE(node4.GetPrevSiblingForTesting() == nullptr);

  // Deleting children doesn't change deleted substructure
  EXPECT_EQ(&node3, node2.GetFirstChild());
  EXPECT_TRUE(node3.GetParent() == &node2);
}

TEST(CFX_XMLNodeTest, AddingChildren) {
  CFX_XMLElement node1(L"node");
  CFX_XMLElement node2(L"node2");
  CFX_XMLElement node3(L"node3");

  node1.AppendChild(&node2);
  node1.AppendChild(&node3);

  EXPECT_EQ(&node1, node2.GetParent());
  EXPECT_EQ(&node1, node3.GetParent());

  EXPECT_EQ(&node2, node1.GetFirstChild());
  EXPECT_EQ(&node3, node2.GetNextSibling());
  EXPECT_TRUE(node3.GetNextSibling() == nullptr);

  // Insert to negative appends.
  CFX_XMLElement node4(L"node4");
  node1.InsertChildNode(&node4, -1);
  EXPECT_EQ(&node1, node4.GetParent());
  EXPECT_EQ(&node4, node3.GetNextSibling());
  EXPECT_TRUE(node4.GetNextSibling() == nullptr);

  CFX_XMLElement node5(L"node5");
  node1.InsertChildNode(&node5, 1);
  EXPECT_EQ(&node1, node5.GetParent());
  EXPECT_EQ(&node2, node1.GetFirstChild());
  EXPECT_EQ(&node5, node2.GetNextSibling());
  EXPECT_EQ(&node3, node5.GetNextSibling());
  EXPECT_EQ(&node4, node3.GetNextSibling());
  EXPECT_TRUE(node4.GetNextSibling() == nullptr);
}

#ifndef NDEBUG
TEST(CFX_XMLNodeTest, DuplicateInsertOfNode) {
  CFX_XMLElement node1(L"node");
  CFX_XMLElement node2(L"node2");

  node1.AppendChild(&node2);
  EXPECT_DEATH(node1.AppendChild(&node2), "");
}
#endif  // NDEBUG

TEST(CFX_XMLNodeTest, RemovingMiddleChild) {
  CFX_XMLElement node1(L"node1");
  CFX_XMLElement node2(L"node2");
  CFX_XMLElement node3(L"node3");
  CFX_XMLElement node4(L"node4");

  node1.AppendChild(&node2);
  node1.AppendChild(&node3);
  node1.AppendChild(&node4);

  EXPECT_EQ(&node2, node1.GetFirstChild());
  EXPECT_EQ(&node3, node2.GetNextSibling());
  EXPECT_EQ(&node4, node3.GetNextSibling());
  EXPECT_TRUE(node4.GetNextSibling() == nullptr);

  node1.RemoveChildNode(&node3);
  EXPECT_TRUE(node3.GetParent() == nullptr);
  EXPECT_TRUE(node3.GetNextSibling() == nullptr);
  EXPECT_TRUE(node3.GetPrevSiblingForTesting() == nullptr);

  EXPECT_EQ(&node2, node1.GetFirstChild());
  EXPECT_EQ(&node4, node2.GetNextSibling());
  EXPECT_EQ(&node2, node4.GetPrevSiblingForTesting());
  EXPECT_TRUE(node4.GetNextSibling() == nullptr);
}

TEST(CFX_XMLNodeTest, RemovingFirstChild) {
  CFX_XMLElement node1(L"node1");
  CFX_XMLElement node2(L"node2");
  CFX_XMLElement node3(L"node3");
  CFX_XMLElement node4(L"node4");

  node1.AppendChild(&node2);
  node1.AppendChild(&node3);
  node1.AppendChild(&node4);

  EXPECT_EQ(&node2, node1.GetFirstChild());
  EXPECT_EQ(&node3, node2.GetNextSibling());
  EXPECT_EQ(&node4, node3.GetNextSibling());
  EXPECT_TRUE(node4.GetNextSibling() == nullptr);

  node1.RemoveChildNode(&node2);
  EXPECT_TRUE(node2.GetParent() == nullptr);
  EXPECT_TRUE(node2.GetNextSibling() == nullptr);
  EXPECT_TRUE(node2.GetPrevSiblingForTesting() == nullptr);

  EXPECT_EQ(&node3, node1.GetFirstChild());
  EXPECT_TRUE(node3.GetPrevSiblingForTesting() == nullptr);
  EXPECT_EQ(&node4, node3.GetNextSibling());
  EXPECT_TRUE(node4.GetNextSibling() == nullptr);
}

TEST(CFX_XMLNodeTest, RemovingLastChild) {
  CFX_XMLElement node1(L"node1");
  CFX_XMLElement node2(L"node2");
  CFX_XMLElement node3(L"node3");
  CFX_XMLElement node4(L"node4");

  node1.AppendChild(&node2);
  node1.AppendChild(&node3);
  node1.AppendChild(&node4);

  EXPECT_EQ(&node2, node1.GetFirstChild());
  EXPECT_EQ(&node3, node2.GetNextSibling());
  EXPECT_EQ(&node4, node3.GetNextSibling());
  EXPECT_TRUE(node4.GetNextSibling() == nullptr);

  node1.RemoveChildNode(&node4);
  EXPECT_TRUE(node4.GetParent() == nullptr);
  EXPECT_TRUE(node4.GetNextSibling() == nullptr);
  EXPECT_TRUE(node4.GetPrevSiblingForTesting() == nullptr);

  EXPECT_EQ(&node2, node1.GetFirstChild());
  EXPECT_EQ(&node3, node2.GetNextSibling());
  EXPECT_TRUE(node3.GetNextSibling() == nullptr);
}

TEST(CFX_XMLNodeTest, RemovingOnlyChild) {
  CFX_XMLElement node1(L"node1");
  CFX_XMLElement node2(L"node2");

  node1.AppendChild(&node2);

  EXPECT_EQ(&node2, node1.GetFirstChild());
  EXPECT_TRUE(node2.GetNextSibling() == nullptr);

  node1.RemoveChildNode(&node2);
  EXPECT_TRUE(node2.GetParent() == nullptr);

  EXPECT_TRUE(node1.GetFirstChild() == nullptr);
  EXPECT_TRUE(node2.GetNextSibling() == nullptr);
  EXPECT_TRUE(node2.GetPrevSiblingForTesting() == nullptr);
}

TEST(CFX_XMLNodeTest, RemoveMissingChild) {
  CFX_XMLElement node1(L"node1");
  CFX_XMLElement node2(L"node2");
  CFX_XMLElement node3(L"node3");

  node1.AppendChild(&node2);
  node1.RemoveChildNode(&node3);

  EXPECT_TRUE(node3.GetParent() == nullptr);
  EXPECT_EQ(&node2, node1.GetFirstChild());
  EXPECT_TRUE(node2.GetNextSibling() == nullptr);
}
