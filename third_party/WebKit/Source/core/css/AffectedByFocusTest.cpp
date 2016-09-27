// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/HTMLNames.h"
#include "core/dom/Element.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/StyleEngine.h"
#include "core/frame/FrameView.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLElement.h"
#include "core/testing/DummyPageHolder.h"
#include <gtest/gtest.h>

namespace blink {

using namespace HTMLNames;

class AffectedByFocusTest : public ::testing::Test {
protected:
    struct ElementResult {
        const blink::HTMLQualifiedName tag;
        bool affectedBy;
        bool childrenOrSiblingsAffectedBy;
    };

    void SetUp() override;

    HTMLDocument& document() const { return *m_document; }

    void setHtmlInnerHTML(const char* htmlContent);

    void checkElements(ElementResult expected[], unsigned expectedCount) const;

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;

    HTMLDocument* m_document;
};

void AffectedByFocusTest::SetUp()
{
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600));
    m_document = toHTMLDocument(&m_dummyPageHolder->document());
    ASSERT(m_document);
}

void AffectedByFocusTest::setHtmlInnerHTML(const char* htmlContent)
{
    document().documentElement()->setInnerHTML(String::fromUTF8(htmlContent), ASSERT_NO_EXCEPTION);
    document().view()->updateAllLifecyclePhases();
}

void AffectedByFocusTest::checkElements(ElementResult expected[], unsigned expectedCount) const
{
    unsigned i = 0;
    HTMLElement* element = document().body();

    for (; element && i < expectedCount; element = Traversal<HTMLElement>::next(*element), ++i) {
        ASSERT_TRUE(element->hasTagName(expected[i].tag));
        ASSERT(element->computedStyle());
        ASSERT_EQ(expected[i].affectedBy, element->computedStyle()->affectedByFocus());
        ASSERT_EQ(expected[i].childrenOrSiblingsAffectedBy, element->childrenOrSiblingsAffectedByFocus());
    }

    ASSERT(!element && i == expectedCount);
}

// A global :focus rule in html.css currently causes every single element to be
// affectedByFocus. Check that all elements in a document with no :focus rules
// gets the affectedByFocus set on ComputedStyle and not childrenOrSiblingsAffectedByFocus.
TEST_F(AffectedByFocusTest, UAUniversalFocusRule)
{
    ElementResult expected[] = {
        { bodyTag, true, false },
        { divTag, true, false },
        { divTag, true, false },
        { divTag, true, false },
        { spanTag, true, false }
    };

    setHtmlInnerHTML("<body>"
        "<div><div></div></div>"
        "<div><span></span></div>"
        "</body>");

    checkElements(expected, sizeof(expected) / sizeof(ElementResult));
}

// ":focus div" will mark ascendants of all divs with childrenOrSiblingsAffectedByFocus.
TEST_F(AffectedByFocusTest, FocusedAscendant)
{
    ElementResult expected[] = {
        { bodyTag, true, true },
        { divTag, true, true },
        { divTag, true, false },
        { divTag, true, false },
        { spanTag, true, false }
    };

    setHtmlInnerHTML("<head>"
        "<style>:focus div { background-color: pink }</style>"
        "</head>"
        "<body>"
        "<div><div></div></div>"
        "<div><span></span></div>"
        "</body>");

    checkElements(expected, sizeof(expected) / sizeof(ElementResult));
}

// "body:focus div" will mark the body element with childrenOrSiblingsAffectedByFocus.
TEST_F(AffectedByFocusTest, FocusedAscendantWithType)
{
    ElementResult expected[] = {
        { bodyTag, true, true },
        { divTag, true, false },
        { divTag, true, false },
        { divTag, true, false },
        { spanTag, true, false }
    };

    setHtmlInnerHTML("<head>"
        "<style>body:focus div { background-color: pink }</style>"
        "</head>"
        "<body>"
        "<div><div></div></div>"
        "<div><span></span></div>"
        "</body>");

    checkElements(expected, sizeof(expected) / sizeof(ElementResult));
}

// ":not(body):focus div" should not mark the body element with childrenOrSiblingsAffectedByFocus.
// Note that currently ":focus:not(body)" does not do the same. Then the :focus is
// checked and the childrenOrSiblingsAffectedByFocus flag set before the negated type selector
// is found.
TEST_F(AffectedByFocusTest, FocusedAscendantWithNegatedType)
{
    ElementResult expected[] = {
        { bodyTag, true, false },
        { divTag, true, true },
        { divTag, true, false },
        { divTag, true, false },
        { spanTag, true, false }
    };

    setHtmlInnerHTML("<head>"
        "<style>:not(body):focus div { background-color: pink }</style>"
        "</head>"
        "<body>"
        "<div><div></div></div>"
        "<div><span></span></div>"
        "</body>");

    checkElements(expected, sizeof(expected) / sizeof(ElementResult));
}

// Checking current behavior for ":focus + div", but this is a BUG or at best
// sub-optimal. The focused element will also in this case get childrenOrSiblingsAffectedByFocus
// even if it's really a sibling. Effectively, the whole sub-tree of the focused
// element will have styles recalculated even though none of the children are
// affected. There are other mechanisms that makes sure the sibling also gets its
// styles recalculated.
TEST_F(AffectedByFocusTest, FocusedSibling)
{
    ElementResult expected[] = {
        { bodyTag, true, false },
        { divTag, true, true },
        { spanTag, true, false },
        { divTag, true, false }
    };

    setHtmlInnerHTML("<head>"
        "<style>:focus + div { background-color: pink }</style>"
        "</head>"
        "<body>"
        "<div>"
        "  <span></span>"
        "</div>"
        "<div></div>"
        "</body>");

    checkElements(expected, sizeof(expected) / sizeof(ElementResult));
}

TEST_F(AffectedByFocusTest, AffectedByFocusUpdate)
{
    // Check that when focussing the outer div in the document below, you only
    // get a single element style recalc.

    setHtmlInnerHTML("<style>:focus { border: 1px solid lime; }</style>"
        "<div id=d tabIndex=1>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "</div>");

    document().view()->updateAllLifecyclePhases();

    unsigned startCount = document().styleEngine().resolverAccessCount();

    document().getElementById("d")->focus();
    document().view()->updateAllLifecyclePhases();

    unsigned accessCount = document().styleEngine().resolverAccessCount() - startCount;

    ASSERT_EQ(1U, accessCount);
}

TEST_F(AffectedByFocusTest, ChildrenOrSiblingsAffectedByFocusUpdate)
{
    // Check that when focussing the outer div in the document below, you get a
    // style recalc for the whole subtree.

    setHtmlInnerHTML("<style>:focus div { border: 1px solid lime; }</style>"
        "<div id=d tabIndex=1>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "</div>");

    document().view()->updateAllLifecyclePhases();

    unsigned startCount = document().styleEngine().resolverAccessCount();

    document().getElementById("d")->focus();
    document().view()->updateAllLifecyclePhases();

    unsigned accessCount = document().styleEngine().resolverAccessCount() - startCount;

    ASSERT_EQ(11U, accessCount);
}

TEST_F(AffectedByFocusTest, InvalidationSetFocusUpdate)
{
    // Check that when focussing the outer div in the document below, you get a
    // style recalc for the outer div and the class=a div only.

    setHtmlInnerHTML("<style>:focus .a { border: 1px solid lime; }</style>"
        "<div id=d tabIndex=1>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div class='a'></div>"
        "</div>");

    document().view()->updateAllLifecyclePhases();

    unsigned startCount = document().styleEngine().resolverAccessCount();

    document().getElementById("d")->focus();
    document().view()->updateAllLifecyclePhases();

    unsigned accessCount = document().styleEngine().resolverAccessCount() - startCount;

    ASSERT_EQ(2U, accessCount);
}

TEST_F(AffectedByFocusTest, NoInvalidationSetFocusUpdate)
{
    // Check that when focussing the outer div in the document below, you get a
    // style recalc for the outer div only. The invalidation set for :focus will
    // include 'a', but the id=d div should be affectedByFocus, not childrenOrSiblingsAffectedByFocus.

    setHtmlInnerHTML("<style>#nomatch:focus .a { border: 1px solid lime; }</style>"
        "<div id=d tabIndex=1>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div></div>"
        "<div class='a'></div>"
        "</div>");

    document().view()->updateAllLifecyclePhases();

    unsigned startCount = document().styleEngine().resolverAccessCount();

    document().getElementById("d")->focus();
    document().view()->updateAllLifecyclePhases();

    unsigned accessCount = document().styleEngine().resolverAccessCount() - startCount;

    ASSERT_EQ(1U, accessCount);
}

} // namespace blink
