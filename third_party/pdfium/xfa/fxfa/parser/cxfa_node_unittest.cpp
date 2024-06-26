// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/parser/cxfa_node.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/parser/cxfa_document.h"

namespace {

class TestNode final : public CXFA_Node {
 public:
  explicit TestNode(CXFA_Document* doc)
      : CXFA_Node(doc,
                  XFA_PacketType::Form,
                  (XFA_XDPPACKET_Template | XFA_XDPPACKET_Form),
                  XFA_ObjectType::Node,
                  XFA_Element::Node,
                  nullptr,
                  nullptr) {}

  ~TestNode() override = default;
};

}  // namespace

class CXFANodeTest : public testing::Test {
 public:
  void SetUp() override {
    doc_ = pdfium::MakeUnique<CXFA_Document>(nullptr);
    node_ = pdfium::MakeUnique<TestNode>(doc_.get());
  }

  void TearDown() override {
    node_ = nullptr;
    doc_ = nullptr;
  }

  CXFA_Document* GetDoc() const { return doc_.get(); }
  CXFA_Node* GetNode() const { return node_.get(); }

 private:
  std::unique_ptr<CXFA_Document> doc_;
  std::unique_ptr<TestNode> node_;
};

TEST_F(CXFANodeTest, InsertFirstChild) {
  EXPECT_EQ(nullptr, GetNode()->GetFirstChild());
  EXPECT_EQ(nullptr, GetNode()->GetLastChild());

  CXFA_Node* child =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child);

  EXPECT_EQ(GetNode(), child->GetParent());
  EXPECT_EQ(child, GetNode()->GetFirstChild());
  EXPECT_EQ(child, GetNode()->GetLastChild());
  EXPECT_EQ(nullptr, child->GetPrevSibling());
  EXPECT_EQ(nullptr, child->GetNextSibling());
}

TEST_F(CXFANodeTest, InsertChildByNegativeIndex) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child);

  EXPECT_EQ(GetNode(), child->GetParent());
  EXPECT_EQ(nullptr, child->GetNextSibling());
  EXPECT_EQ(child0, child->GetPrevSibling());
  EXPECT_EQ(child, child0->GetNextSibling());
  EXPECT_EQ(nullptr, child0->GetPrevSibling());

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child, GetNode()->GetLastChild());
}

TEST_F(CXFANodeTest, InsertChildByIndex) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child1);

  CXFA_Node* child2 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child2);

  CXFA_Node* child3 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child3);

  CXFA_Node* child =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(2, child);

  EXPECT_EQ(GetNode(), child->GetParent());

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child1, child0->GetNextSibling());
  EXPECT_EQ(child, child1->GetNextSibling());
  EXPECT_EQ(child2, child->GetNextSibling());
  EXPECT_EQ(child3, child2->GetNextSibling());
  EXPECT_EQ(nullptr, child3->GetNextSibling());

  EXPECT_EQ(child3, GetNode()->GetLastChild());
  EXPECT_EQ(child2, child3->GetPrevSibling());
  EXPECT_EQ(child, child2->GetPrevSibling());
  EXPECT_EQ(child1, child->GetPrevSibling());
  EXPECT_EQ(child0, child1->GetPrevSibling());
  EXPECT_EQ(nullptr, child0->GetPrevSibling());
}

TEST_F(CXFANodeTest, InsertChildIndexPastEnd) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child1);

  CXFA_Node* child =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(20, child);

  EXPECT_EQ(GetNode(), child->GetParent());
  EXPECT_EQ(nullptr, child->GetNextSibling());
  EXPECT_EQ(child1, child->GetPrevSibling());
  EXPECT_EQ(child, child1->GetNextSibling());

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child, GetNode()->GetLastChild());
}

TEST_F(CXFANodeTest, InsertFirstChildBeforeNullptr) {
  EXPECT_EQ(nullptr, GetNode()->GetFirstChild());
  EXPECT_EQ(nullptr, GetNode()->GetLastChild());

  CXFA_Node* child =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(child, nullptr);

  EXPECT_EQ(child, GetNode()->GetFirstChild());
  EXPECT_EQ(child, GetNode()->GetLastChild());
}

TEST_F(CXFANodeTest, InsertBeforeWithNullBefore) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child1);

  CXFA_Node* child =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(child, nullptr);

  EXPECT_EQ(GetNode(), child->GetParent());
  EXPECT_EQ(nullptr, child->GetNextSibling());
  EXPECT_EQ(child1, child->GetPrevSibling());
  EXPECT_EQ(child, child1->GetNextSibling());

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child, GetNode()->GetLastChild());
}

TEST_F(CXFANodeTest, InsertBeforeFirstChild) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child1);

  CXFA_Node* child =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(child, child0);

  EXPECT_EQ(GetNode(), child->GetParent());
  EXPECT_EQ(child0, child->GetNextSibling());
  EXPECT_EQ(nullptr, child->GetPrevSibling());
  EXPECT_EQ(child, child0->GetPrevSibling());

  EXPECT_EQ(child, GetNode()->GetFirstChild());
  EXPECT_EQ(child1, GetNode()->GetLastChild());
}

TEST_F(CXFANodeTest, InsertBefore) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child1);

  CXFA_Node* child2 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child2);

  CXFA_Node* child3 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child3);

  CXFA_Node* child =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(child, child2);

  EXPECT_EQ(GetNode(), child->GetParent());
  EXPECT_EQ(child2, child->GetNextSibling());
  EXPECT_EQ(child1, child->GetPrevSibling());
  EXPECT_EQ(child, child1->GetNextSibling());
  EXPECT_EQ(child, child2->GetPrevSibling());

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child3, GetNode()->GetLastChild());
}

TEST_F(CXFANodeTest, RemoveOnlyChild) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child0, GetNode()->GetLastChild());

  GetNode()->RemoveChild(child0, false);

  EXPECT_EQ(nullptr, GetNode()->GetFirstChild());
  EXPECT_EQ(nullptr, GetNode()->GetLastChild());

  EXPECT_EQ(nullptr, child0->GetParent());
  EXPECT_EQ(nullptr, child0->GetNextSibling());
  EXPECT_EQ(nullptr, child0->GetPrevSibling());
}

TEST_F(CXFANodeTest, RemoveFirstChild) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child1);

  CXFA_Node* child2 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child2);

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child2, GetNode()->GetLastChild());

  GetNode()->RemoveChild(child0, false);

  EXPECT_EQ(child1, GetNode()->GetFirstChild());
  EXPECT_EQ(child2, GetNode()->GetLastChild());

  EXPECT_EQ(nullptr, child1->GetPrevSibling());
  EXPECT_EQ(nullptr, child0->GetParent());
  EXPECT_EQ(nullptr, child0->GetNextSibling());
  EXPECT_EQ(nullptr, child0->GetPrevSibling());
}

TEST_F(CXFANodeTest, RemoveLastChild) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child1);

  CXFA_Node* child2 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child2);

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child2, GetNode()->GetLastChild());

  GetNode()->RemoveChild(child2, false);

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child1, GetNode()->GetLastChild());

  EXPECT_EQ(nullptr, child1->GetNextSibling());
  EXPECT_EQ(nullptr, child2->GetParent());
  EXPECT_EQ(nullptr, child2->GetNextSibling());
  EXPECT_EQ(nullptr, child2->GetPrevSibling());
}

TEST_F(CXFANodeTest, RemoveChild) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child1);

  CXFA_Node* child2 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child2);

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child2, GetNode()->GetLastChild());

  GetNode()->RemoveChild(child1, false);

  EXPECT_EQ(child0, GetNode()->GetFirstChild());
  EXPECT_EQ(child2, GetNode()->GetLastChild());

  EXPECT_EQ(child2, child0->GetNextSibling());
  EXPECT_EQ(child0, child2->GetPrevSibling());
  EXPECT_EQ(nullptr, child1->GetParent());
  EXPECT_EQ(nullptr, child1->GetNextSibling());
  EXPECT_EQ(nullptr, child1->GetPrevSibling());
}

TEST_F(CXFANodeTest, InsertChildWithParent) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  child0->InsertChild(-1, child1);

  EXPECT_DEATH_IF_SUPPORTED(GetNode()->InsertChild(0, child1), "");
}

TEST_F(CXFANodeTest, InsertNullChild) {
  EXPECT_DEATH_IF_SUPPORTED(GetNode()->InsertChild(0, nullptr), "");
}

TEST_F(CXFANodeTest, InsertBeforeWithNullChild) {
  EXPECT_DEATH_IF_SUPPORTED(GetNode()->InsertChild(nullptr, nullptr), "");
}

TEST_F(CXFANodeTest, InsertBeforeWithBeforeInAnotherParent) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  child0->InsertChild(-1, child1);

  CXFA_Node* child =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  EXPECT_DEATH_IF_SUPPORTED(GetNode()->InsertChild(child, child1), "");
}

TEST_F(CXFANodeTest, InsertBeforeWithNodeInAnotherParent) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  child0->InsertChild(-1, child1);

  EXPECT_DEATH_IF_SUPPORTED(GetNode()->InsertChild(child1, nullptr), "");
}

TEST_F(CXFANodeTest, RemoveChildNullptr) {
  EXPECT_DEATH_IF_SUPPORTED(GetNode()->RemoveChild(nullptr, false), "");
}

TEST_F(CXFANodeTest, RemoveChildAnotherParent) {
  CXFA_Node* child0 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  GetNode()->InsertChild(-1, child0);

  CXFA_Node* child1 =
      GetDoc()->CreateNode(XFA_PacketType::Form, XFA_Element::Ui);
  child0->InsertChild(-1, child1);

  EXPECT_DEATH_IF_SUPPORTED(GetNode()->RemoveChild(child1, false), "");
}
