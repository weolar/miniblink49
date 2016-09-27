// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/Node.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/frame/FrameView.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLElement.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/geometry/IntSize.h"
#include "wtf/Compiler.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/StdLibExtras.h"
#include "wtf/Vector.h"
#include "wtf/testing/WTFTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

class ComposedTreeTraversalTest : public ::testing::Test {
protected:
    HTMLDocument& document() const;

    // Sets |mainHTML| to BODY element with |innerHTML| property and attaches
    // shadow root to child with |shadowHTML|, then update distribution for
    // calling member functions in |ComposedTreeTraversal|.
    void setupSampleHTML(const char* mainHTML, const char* shadowHTML, unsigned);

private:
    void SetUp() override;

    HTMLDocument* m_document;
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
};

void ComposedTreeTraversalTest::SetUp()
{
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600));
    m_document = toHTMLDocument(&m_dummyPageHolder->document());
    ASSERT(m_document);
}

HTMLDocument& ComposedTreeTraversalTest::document() const
{
    return *m_document;
}

void ComposedTreeTraversalTest::setupSampleHTML(const char* mainHTML, const char* shadowHTML, unsigned index)
{
    RefPtrWillBeRawPtr<Element> body = document().body();
    body->setInnerHTML(String::fromUTF8(mainHTML), ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> shadowHost = toElement(NodeTraversal::childAt(*body, index));
    RefPtrWillBeRawPtr<ShadowRoot> shadowRoot = shadowHost->createShadowRoot(ASSERT_NO_EXCEPTION);
    shadowRoot->setInnerHTML(String::fromUTF8(shadowHTML), ASSERT_NO_EXCEPTION);
    body->updateDistribution();
}

void testCommonAncestor(Node* expectedResult, const Node& nodeA, const Node& nodeB)
{
    Node* result1 = ComposedTreeTraversal::commonAncestor(nodeA, nodeB);
    EXPECT_EQ(expectedResult, result1) << "commonAncestor(" << nodeA.textContent() << "," << nodeB.textContent() << ")";
    Node* result2 = ComposedTreeTraversal::commonAncestor(nodeB, nodeA);
    EXPECT_EQ(expectedResult, result2) << "commonAncestor(" << nodeB.textContent() << "," << nodeA.textContent() << ")";
}

// Test case for
//  - childAt
//  - countChildren
//  - hasChildren
//  - index
//  - isDescendantOf
TEST_F(ComposedTreeTraversalTest, childAt)
{
    const char* mainHTML =
        "<div id='m0'>"
            "<span id='m00'>m00</span>"
            "<span id='m01'>m01</span>"
        "</div>";
    const char* shadowHTML =
        "<a id='s00'>s00</a>"
        "<content select='#m01'></content>"
        "<a id='s02'>s02</a>"
        "<a id='s03'><content select='#m00'></content></a>"
        "<a id='s04'>s04</a>";
    setupSampleHTML(mainHTML, shadowHTML, 0);

    RefPtrWillBeRawPtr<Element> body = document().body();
    RefPtrWillBeRawPtr<Element> m0 = body->querySelector("#m0", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m00 = m0->querySelector("#m00", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m01 = m0->querySelector("#m01", ASSERT_NO_EXCEPTION);

    RefPtrWillBeRawPtr<Element> shadowHost = m0;
    RefPtrWillBeRawPtr<ShadowRoot> shadowRoot = shadowHost->shadowRoot();
    RefPtrWillBeRawPtr<Element> s00 = shadowRoot->querySelector("#s00", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s02 = shadowRoot->querySelector("#s02", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s03 = shadowRoot->querySelector("#s03", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s04 = shadowRoot->querySelector("#s04", ASSERT_NO_EXCEPTION);

    const unsigned numberOfChildNodes = 5;
    Node* expectedChildNodes[5] = { s00.get(), m01.get(), s02.get(), s03.get(), s04.get() };

    ASSERT_EQ(numberOfChildNodes, ComposedTreeTraversal::countChildren(*shadowHost));
    EXPECT_TRUE(ComposedTreeTraversal::hasChildren(*shadowHost));

    for (unsigned index = 0; index < numberOfChildNodes; ++index) {
        Node* child = ComposedTreeTraversal::childAt(*shadowHost, index);
        EXPECT_EQ(expectedChildNodes[index], child)
            << "ComposedTreeTraversal::childAt(*shadowHost, " << index << ")";
        EXPECT_EQ(index, ComposedTreeTraversal::index(*child))
            << "ComposedTreeTraversal::index(ComposedTreeTraversal(*shadowHost, " << index << "))";
        EXPECT_TRUE(ComposedTreeTraversal::isDescendantOf(*child, *shadowHost))
            << "ComposedTreeTraversal::isDescendantOf(*ComposedTreeTraversal(*shadowHost, " << index << "), *shadowHost)";
    }
    EXPECT_EQ(nullptr, ComposedTreeTraversal::childAt(*shadowHost, numberOfChildNodes + 1))
        << "Out of bounds childAt() returns nullptr.";

    // Distribute node |m00| is child of node in shadow tree |s03|.
    EXPECT_EQ(m00.get(), ComposedTreeTraversal::childAt(*s03, 0));
}

// Test case for
//  - commonAncestor
//  - isDescendantOf
TEST_F(ComposedTreeTraversalTest, commonAncestor)
{
    // We build following composed tree:
    //             ____BODY___
    //             |    |     |
    //            m0    m1    m2       m1 is shadow host having m10, m11, m12.
    //            _|_   |   __|__
    //           |   |  |   |    |
    //          m00 m01 |   m20 m21
    //             _____|_____________
    //             |  |   |    |     |
    //            s10 s11 s12 s13  s14
    //                         |
    //                       __|__
    //                |      |    |
    //                m12    m10 m11 <-- distributed
    // where: each symbol consists with prefix, child index, child-child index.
    //  prefix "m" means node in main tree,
    //  prefix "d" means node in main tree and distributed
    //  prefix "s" means node in shadow tree
    const char* mainHTML =
        "<a id='m0'><b id='m00'>m00</b><b id='m01'>m01</b></a>"
        "<a id='m1'>"
            "<b id='m10'>m10</b>"
            "<b id='m11'>m11</b>"
            "<b id='m12'>m12</b>"
        "</a>"
        "<a id='m2'><b id='m20'>m20</b><b id='m21'>m21</b></a>";
    const char* shadowHTML =
        "<a id='s10'>s10</a>"
        "<a id='s11'><content select='#m12'></content></a>"
        "<a id='s12'>s12</a>"
        "<a id='s13'>"
            "<content select='#m10'></content>"
            "<content select='#m11'></content>"
        "</a>"
        "<a id='s14'>s14</a>";
    setupSampleHTML(mainHTML, shadowHTML, 1);
    RefPtrWillBeRawPtr<Element> body = document().body();
    RefPtrWillBeRawPtr<Element> m0 = body->querySelector("#m0", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m1 = body->querySelector("#m1", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m2 = body->querySelector("#m2", ASSERT_NO_EXCEPTION);

    RefPtrWillBeRawPtr<Element> m00 = body->querySelector("#m00", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m01 = body->querySelector("#m01", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m10 = body->querySelector("#m10", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m11 = body->querySelector("#m11", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m12 = body->querySelector("#m12", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m20 = body->querySelector("#m20", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m21 = body->querySelector("#m21", ASSERT_NO_EXCEPTION);

    RefPtrWillBeRawPtr<ShadowRoot> shadowRoot = m1->shadowRoot();
    RefPtrWillBeRawPtr<Element> s10 = shadowRoot->querySelector("#s10", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s11 = shadowRoot->querySelector("#s11", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s12 = shadowRoot->querySelector("#s12", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s13 = shadowRoot->querySelector("#s13", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s14 = shadowRoot->querySelector("#s14", ASSERT_NO_EXCEPTION);

    testCommonAncestor(body.get(), *m0, *m1);
    testCommonAncestor(body.get(), *m1, *m2);
    testCommonAncestor(body.get(), *m1, *m20);
    testCommonAncestor(body.get(), *s14, *m21);

    testCommonAncestor(m0.get(), *m0, *m0);
    testCommonAncestor(m0.get(), *m00, *m01);

    testCommonAncestor(m1.get(), *m1.get(), *m1);
    testCommonAncestor(m1.get(), *s10, *s14);
    testCommonAncestor(m1.get(), *s10, *m12);
    testCommonAncestor(m1.get(), *s12, *m12);
    testCommonAncestor(m1.get(), *m10, *m12);

    testCommonAncestor(m01.get(), *m01, *m01);
    testCommonAncestor(s11.get(), *s11, *m12);
    testCommonAncestor(s13.get(), *m10, *m11);

    s12->remove(ASSERT_NO_EXCEPTION);
    testCommonAncestor(s12.get(), *s12, *s12);
    testCommonAncestor(nullptr, *s12, *s11);
    testCommonAncestor(nullptr, *s12, *m01);
    testCommonAncestor(nullptr, *s12, *m20);

    m20->remove(ASSERT_NO_EXCEPTION);
    testCommonAncestor(m20.get(), *m20, *m20);
    testCommonAncestor(nullptr, *m20, *s12);
    testCommonAncestor(nullptr, *m20, *m1);
}

// Test case for
//  - nextSkippingChildren
//  - previousSkippingChildren
TEST_F(ComposedTreeTraversalTest, nextSkippingChildren)
{
    const char* mainHTML =
        "<div id='m0'>m0</div>"
        "<div id='m1'>"
            "<span id='m10'>m10</span>"
            "<span id='m11'>m11</span>"
        "</div>"
        "<div id='m2'>m2</div>";
    const char* shadowHTML =
        "<content select='#m11'></content>"
        "<a id='s11'>s11</a>"
        "<a id='s12'>"
            "<b id='s120'>s120</b>"
            "<content select='#m10'></content>"
        "</a>";
    setupSampleHTML(mainHTML, shadowHTML, 1);

    RefPtrWillBeRawPtr<Element> body = document().body();
    RefPtrWillBeRawPtr<Element> m0 = body->querySelector("#m0", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m1 = body->querySelector("#m1", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m2 = body->querySelector("#m2", ASSERT_NO_EXCEPTION);

    RefPtrWillBeRawPtr<Element> m10 = body->querySelector("#m10", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m11 = body->querySelector("#m11", ASSERT_NO_EXCEPTION);

    RefPtrWillBeRawPtr<ShadowRoot> shadowRoot = m1->shadowRoot();
    RefPtrWillBeRawPtr<Element> s11 = shadowRoot->querySelector("#s11", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s12 = shadowRoot->querySelector("#s12", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s120 = shadowRoot->querySelector("#s120", ASSERT_NO_EXCEPTION);

    // Main tree node to main tree node
    EXPECT_EQ(*m1, ComposedTreeTraversal::nextSkippingChildren(*m0));
    EXPECT_EQ(*m0, ComposedTreeTraversal::previousSkippingChildren(*m1));

    // Distribute node to main tree node
    EXPECT_EQ(*m2, ComposedTreeTraversal::nextSkippingChildren(*m10));
    EXPECT_EQ(*m1, ComposedTreeTraversal::previousSkippingChildren(*m2));

    // Distribute node to node in shadow tree
    EXPECT_EQ(*s11, ComposedTreeTraversal::nextSkippingChildren(*m11));
    EXPECT_EQ(*m11, ComposedTreeTraversal::previousSkippingChildren(*s11));

    // Node in shadow tree to distributed node
    EXPECT_EQ(*s11, ComposedTreeTraversal::nextSkippingChildren(*m11));
    EXPECT_EQ(*m11, ComposedTreeTraversal::previousSkippingChildren(*s11));

    EXPECT_EQ(*m10, ComposedTreeTraversal::nextSkippingChildren(*s120));
    EXPECT_EQ(*s120, ComposedTreeTraversal::previousSkippingChildren(*m10));

    // Node in shadow tree to main tree
    EXPECT_EQ(*m2, ComposedTreeTraversal::nextSkippingChildren(*s12));
    EXPECT_EQ(*m1, ComposedTreeTraversal::previousSkippingChildren(*m2));
}

// Test case for
//  - lastWithin
//  - lastWithinOrSelf
TEST_F(ComposedTreeTraversalTest, lastWithin)
{
    const char* mainHTML =
        "<div id='m0'>m0</div>"
        "<div id='m1'>"
            "<span id='m10'>m10</span>"
            "<span id='m11'>m11</span>"
            "<span id='m12'>m12</span>" // #m12 is not distributed.
        "</div>"
        "<div id='m2'></div>";
    const char* shadowHTML =
        "<content select='#m11'></content>"
        "<a id='s11'>s11</a>"
        "<a id='s12'>"
            "<content select='#m10'></content>"
        "</a>";
    setupSampleHTML(mainHTML, shadowHTML, 1);

    RefPtrWillBeRawPtr<Element> body = document().body();
    RefPtrWillBeRawPtr<Element> m0 = body->querySelector("#m0", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m1 = body->querySelector("#m1", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> m2 = body->querySelector("#m2", ASSERT_NO_EXCEPTION);

    RefPtrWillBeRawPtr<Element> m10 = body->querySelector("#m10", ASSERT_NO_EXCEPTION);

    RefPtrWillBeRawPtr<ShadowRoot> shadowRoot = m1->shadowRoot();
    RefPtrWillBeRawPtr<Element> s11 = shadowRoot->querySelector("#s11", ASSERT_NO_EXCEPTION);
    RefPtrWillBeRawPtr<Element> s12 = shadowRoot->querySelector("#s12", ASSERT_NO_EXCEPTION);

    EXPECT_EQ(m0->firstChild(), ComposedTreeTraversal::lastWithin(*m0));
    EXPECT_EQ(*m0->firstChild(), ComposedTreeTraversal::lastWithinOrSelf(*m0));

    EXPECT_EQ(m10->firstChild(), ComposedTreeTraversal::lastWithin(*m1));
    EXPECT_EQ(*m10->firstChild(), ComposedTreeTraversal::lastWithinOrSelf(*m1));

    EXPECT_EQ(nullptr, ComposedTreeTraversal::lastWithin(*m2));
    EXPECT_EQ(*m2, ComposedTreeTraversal::lastWithinOrSelf(*m2));

    EXPECT_EQ(s11->firstChild(), ComposedTreeTraversal::lastWithin(*s11));
    EXPECT_EQ(*s11->firstChild(), ComposedTreeTraversal::lastWithinOrSelf(*s11));

    EXPECT_EQ(m10->firstChild(), ComposedTreeTraversal::lastWithin(*s12));
    EXPECT_EQ(*m10->firstChild(), ComposedTreeTraversal::lastWithinOrSelf(*s12));
}

} // namespace blink
