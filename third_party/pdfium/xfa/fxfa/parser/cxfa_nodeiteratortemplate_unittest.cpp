// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/parser/cxfa_nodeiteratortemplate.h"

#include <memory>
#include <vector>

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"

class CXFA_NodeIteratorTemplateTest : public testing::Test {
 public:
  class Node {
   public:
    class Strategy {
     public:
      static Node* GetFirstChild(Node* pNode) {
        return pNode && !pNode->children_.empty() ? pNode->children_.front()
                                                  : nullptr;
      }
      static Node* GetNextSibling(Node* pNode) {
        return pNode ? pNode->next_sibling_ : nullptr;
      }
      static Node* GetParent(Node* pNode) {
        return pNode ? pNode->parent_ : nullptr;
      }
    };

    explicit Node(Node* parent) : parent_(parent), next_sibling_(nullptr) {
      if (parent) {
        if (!parent->children_.empty())
          parent->children_.back()->next_sibling_ = this;
        parent->children_.push_back(this);
      }
    }

   private:
    Node* parent_;
    Node* next_sibling_;
    std::vector<Node*> children_;
  };

  using Iterator = CXFA_NodeIteratorTemplate<Node, Node::Strategy>;

  // Builds a tree along the lines of:
  //
  //   root
  //   |
  //   child1--child2
  //            |
  //            child3------------child7--child9
  //            |                 |
  //            child4--child6    child8
  //            |
  //            child5
  //
  void SetUp() override {
    root_ = pdfium::MakeUnique<Node>(nullptr);
    child1_ = pdfium::MakeUnique<Node>(root_.get());
    child2_ = pdfium::MakeUnique<Node>(root_.get());
    child3_ = pdfium::MakeUnique<Node>(child2_.get());
    child4_ = pdfium::MakeUnique<Node>(child3_.get());
    child5_ = pdfium::MakeUnique<Node>(child4_.get());
    child6_ = pdfium::MakeUnique<Node>(child3_.get());
    child7_ = pdfium::MakeUnique<Node>(child2_.get());
    child8_ = pdfium::MakeUnique<Node>(child7_.get());
    child9_ = pdfium::MakeUnique<Node>(child2_.get());
  }

  Node* root() const { return root_.get(); }
  Node* child1() const { return child1_.get(); }
  Node* child2() const { return child2_.get(); }
  Node* child3() const { return child3_.get(); }
  Node* child4() const { return child4_.get(); }
  Node* child5() const { return child5_.get(); }
  Node* child6() const { return child6_.get(); }
  Node* child7() const { return child7_.get(); }
  Node* child8() const { return child8_.get(); }
  Node* child9() const { return child9_.get(); }

 protected:
  std::unique_ptr<Node> root_;
  std::unique_ptr<Node> child1_;
  std::unique_ptr<Node> child2_;
  std::unique_ptr<Node> child3_;
  std::unique_ptr<Node> child4_;
  std::unique_ptr<Node> child5_;
  std::unique_ptr<Node> child6_;
  std::unique_ptr<Node> child7_;
  std::unique_ptr<Node> child8_;
  std::unique_ptr<Node> child9_;
};

TEST_F(CXFA_NodeIteratorTemplateTest, Empty) {
  Iterator iter(nullptr);
  EXPECT_EQ(nullptr, iter.GetRoot());
  EXPECT_EQ(nullptr, iter.GetCurrent());
  EXPECT_EQ(nullptr, iter.MoveToNext());
  EXPECT_EQ(nullptr, iter.MoveToPrev());
  EXPECT_EQ(nullptr, iter.SkipChildrenAndMoveToNext());
}

TEST_F(CXFA_NodeIteratorTemplateTest, Root) {
  Iterator iter(root());
  EXPECT_EQ(root(), iter.GetRoot());
  EXPECT_EQ(root(), iter.GetCurrent());
}

TEST_F(CXFA_NodeIteratorTemplateTest, Current) {
  Iterator iter(root());
  iter.SetCurrent(child1());
  EXPECT_EQ(root(), iter.GetRoot());
  EXPECT_EQ(child1(), iter.GetCurrent());
}

TEST_F(CXFA_NodeIteratorTemplateTest, CurrentOutsideRootDisallowed) {
  Iterator iter(child1());
  iter.SetCurrent(root());
  EXPECT_EQ(child1(), iter.GetRoot());
  EXPECT_EQ(nullptr, iter.GetCurrent());
}

TEST_F(CXFA_NodeIteratorTemplateTest, CurrentNull) {
  Iterator iter(root());
  EXPECT_EQ(child1(), iter.MoveToNext());

  iter.SetCurrent(nullptr);
  EXPECT_EQ(nullptr, iter.GetCurrent());

  EXPECT_EQ(nullptr, iter.MoveToNext());
  EXPECT_EQ(nullptr, iter.GetCurrent());
}

TEST_F(CXFA_NodeIteratorTemplateTest, MoveToPrev) {
  Iterator iter(root());
  iter.SetCurrent(child9());

  EXPECT_EQ(child8(), iter.MoveToPrev());
  EXPECT_EQ(child8(), iter.GetCurrent());

  EXPECT_EQ(child7(), iter.MoveToPrev());
  EXPECT_EQ(child7(), iter.GetCurrent());

  EXPECT_EQ(child6(), iter.MoveToPrev());
  EXPECT_EQ(child6(), iter.GetCurrent());

  EXPECT_EQ(child5(), iter.MoveToPrev());
  EXPECT_EQ(child5(), iter.GetCurrent());

  EXPECT_EQ(child4(), iter.MoveToPrev());
  EXPECT_EQ(child4(), iter.GetCurrent());

  EXPECT_EQ(child3(), iter.MoveToPrev());
  EXPECT_EQ(child3(), iter.GetCurrent());

  EXPECT_EQ(child2(), iter.MoveToPrev());
  EXPECT_EQ(child2(), iter.GetCurrent());

  EXPECT_EQ(child1(), iter.MoveToPrev());
  EXPECT_EQ(child1(), iter.GetCurrent());

  EXPECT_EQ(root(), iter.MoveToPrev());
  EXPECT_EQ(root(), iter.GetCurrent());

  EXPECT_EQ(nullptr, iter.MoveToPrev());
  EXPECT_EQ(root(), iter.GetCurrent());

  EXPECT_EQ(nullptr, iter.MoveToPrev());
  EXPECT_EQ(root(), iter.GetCurrent());
}

TEST_F(CXFA_NodeIteratorTemplateTest, MoveToNext) {
  Iterator iter(root());
  iter.SetCurrent(child2());

  EXPECT_EQ(child3(), iter.MoveToNext());
  EXPECT_EQ(child3(), iter.GetCurrent());

  EXPECT_EQ(child4(), iter.MoveToNext());
  EXPECT_EQ(child4(), iter.GetCurrent());

  EXPECT_EQ(child5(), iter.MoveToNext());
  EXPECT_EQ(child5(), iter.GetCurrent());

  EXPECT_EQ(child6(), iter.MoveToNext());
  EXPECT_EQ(child6(), iter.GetCurrent());

  EXPECT_EQ(child7(), iter.MoveToNext());
  EXPECT_EQ(child7(), iter.GetCurrent());

  EXPECT_EQ(child8(), iter.MoveToNext());
  EXPECT_EQ(child8(), iter.GetCurrent());

  EXPECT_EQ(child9(), iter.MoveToNext());
  EXPECT_EQ(child9(), iter.GetCurrent());

  EXPECT_EQ(nullptr, iter.MoveToNext());
  EXPECT_EQ(nullptr, iter.GetCurrent());

  EXPECT_EQ(nullptr, iter.MoveToNext());
  EXPECT_EQ(nullptr, iter.GetCurrent());
}

TEST_F(CXFA_NodeIteratorTemplateTest, SkipChildrenAndMoveToNext) {
  Iterator iter(root());
  iter.SetCurrent(child3());
  EXPECT_EQ(child7(), iter.SkipChildrenAndMoveToNext());
  EXPECT_EQ(child9(), iter.SkipChildrenAndMoveToNext());
  EXPECT_EQ(nullptr, iter.SkipChildrenAndMoveToNext());
}

TEST_F(CXFA_NodeIteratorTemplateTest, BackAndForth) {
  Iterator iter(root());
  EXPECT_EQ(child1(), iter.MoveToNext());
  EXPECT_EQ(child2(), iter.MoveToNext());
  EXPECT_EQ(child3(), iter.MoveToNext());
  EXPECT_EQ(child4(), iter.MoveToNext());
  EXPECT_EQ(child5(), iter.MoveToNext());
  EXPECT_EQ(child4(), iter.MoveToPrev());
  EXPECT_EQ(child3(), iter.MoveToPrev());
  EXPECT_EQ(child2(), iter.MoveToPrev());
  EXPECT_EQ(child1(), iter.MoveToPrev());
}

TEST_F(CXFA_NodeIteratorTemplateTest, NextFromBeforeTheBeginning) {
  Iterator iter(root());
  EXPECT_EQ(nullptr, iter.MoveToPrev());
  EXPECT_EQ(root(), iter.GetCurrent());
  EXPECT_EQ(child1(), iter.MoveToNext());
}

TEST_F(CXFA_NodeIteratorTemplateTest, PrevFromAfterTheEnd) {
  Iterator iter(root());
  iter.SetCurrent(child9());
  EXPECT_EQ(nullptr, iter.MoveToNext());
  EXPECT_EQ(child9(), iter.MoveToPrev());
}

TEST_F(CXFA_NodeIteratorTemplateTest, ChildAsRootPrev) {
  Iterator iter(child3());
  EXPECT_EQ(nullptr, iter.MoveToPrev());

  iter.SetCurrent(child4());
  EXPECT_EQ(child3(), iter.MoveToPrev());
  EXPECT_EQ(nullptr, iter.MoveToPrev());
}

TEST_F(CXFA_NodeIteratorTemplateTest, ChildAsRootNext) {
  Iterator iter(child3());
  iter.SetCurrent(child4());
  EXPECT_EQ(child5(), iter.MoveToNext());
  EXPECT_EQ(child6(), iter.MoveToNext());
  EXPECT_EQ(nullptr, iter.MoveToNext());
}
