// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/Range.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/Element.h"
#include "core/dom/NodeList.h"
#include "core/dom/Text.h"
#include "core/html/HTMLBodyElement.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLHtmlElement.h"
#include "platform/heap/Handle.h"
#include "wtf/Compiler.h"
#include "wtf/RefPtr.h"
#include "wtf/text/AtomicString.h"
#include <gtest/gtest.h>

namespace blink {

class RangeTest : public ::testing::Test {
protected:
    void SetUp() override;

    HTMLDocument& document() const;

private:
    RefPtrWillBePersistent<HTMLDocument> m_document;
};

void RangeTest::SetUp()
{
    m_document = HTMLDocument::create();
    RefPtrWillBeRawPtr<HTMLHtmlElement> html = HTMLHtmlElement::create(*m_document);
    html->appendChild(HTMLBodyElement::create(*m_document));
    m_document->appendChild(html.release());
}

HTMLDocument& RangeTest::document() const
{
    return *m_document;
}

TEST_F(RangeTest, SplitTextNodeRangeWithinText)
{
    document().body()->setInnerHTML("1234", ASSERT_NO_EXCEPTION);
    Text* oldText = toText(document().body()->firstChild());

    RefPtrWillBeRawPtr<Range> range04 = Range::create(document(), oldText, 0, oldText, 4);
    RefPtrWillBeRawPtr<Range> range02 = Range::create(document(), oldText, 0, oldText, 2);
    RefPtrWillBeRawPtr<Range> range22 = Range::create(document(), oldText, 2, oldText, 2);
    RefPtrWillBeRawPtr<Range> range24 = Range::create(document(), oldText, 2, oldText, 4);

    oldText->splitText(2, ASSERT_NO_EXCEPTION);
    Text* newText = toText(oldText->nextSibling());

    EXPECT_TRUE(range04->boundaryPointsValid());
    EXPECT_EQ(oldText, range04->startContainer());
    EXPECT_EQ(0, range04->startOffset());
    EXPECT_EQ(newText, range04->endContainer());
    EXPECT_EQ(2, range04->endOffset());

    EXPECT_TRUE(range02->boundaryPointsValid());
    EXPECT_EQ(oldText, range02->startContainer());
    EXPECT_EQ(0, range02->startOffset());
    EXPECT_EQ(oldText, range02->endContainer());
    EXPECT_EQ(2, range02->endOffset());

    // Our implementation always moves the boundary point at the separation point to the end of the original text node.
    EXPECT_TRUE(range22->boundaryPointsValid());
    EXPECT_EQ(oldText, range22->startContainer());
    EXPECT_EQ(2, range22->startOffset());
    EXPECT_EQ(oldText, range22->endContainer());
    EXPECT_EQ(2, range22->endOffset());

    EXPECT_TRUE(range24->boundaryPointsValid());
    EXPECT_EQ(oldText, range24->startContainer());
    EXPECT_EQ(2, range24->startOffset());
    EXPECT_EQ(newText, range24->endContainer());
    EXPECT_EQ(2, range24->endOffset());
}

TEST_F(RangeTest, SplitTextNodeRangeOutsideText)
{
    document().body()->setInnerHTML("<span id=\"outer\">0<span id=\"inner-left\">1</span>SPLITME<span id=\"inner-right\">2</span>3</span>", ASSERT_NO_EXCEPTION);

    Element* outer = document().getElementById(AtomicString::fromUTF8("outer"));
    Element* innerLeft = document().getElementById(AtomicString::fromUTF8("inner-left"));
    Element* innerRight = document().getElementById(AtomicString::fromUTF8("inner-right"));
    Text* oldText = toText(outer->childNodes()->item(2));

    RefPtrWillBeRawPtr<Range> rangeOuterOutside = Range::create(document(), outer, 0, outer, 5);
    RefPtrWillBeRawPtr<Range> rangeOuterInside = Range::create(document(), outer, 1, outer, 4);
    RefPtrWillBeRawPtr<Range> rangeOuterSurroundingText = Range::create(document(), outer, 2, outer, 3);
    RefPtrWillBeRawPtr<Range> rangeInnerLeft = Range::create(document(), innerLeft, 0, innerLeft, 1);
    RefPtrWillBeRawPtr<Range> rangeInnerRight = Range::create(document(), innerRight, 0, innerRight, 1);
    RefPtrWillBeRawPtr<Range> rangeFromTextToMiddleOfElement = Range::create(document(), oldText, 6, outer, 3);

    oldText->splitText(3, ASSERT_NO_EXCEPTION);
    Text* newText = toText(oldText->nextSibling());

    EXPECT_TRUE(rangeOuterOutside->boundaryPointsValid());
    EXPECT_EQ(outer, rangeOuterOutside->startContainer());
    EXPECT_EQ(0, rangeOuterOutside->startOffset());
    EXPECT_EQ(outer, rangeOuterOutside->endContainer());
    EXPECT_EQ(6, rangeOuterOutside->endOffset()); // Increased by 1 since a new node is inserted.

    EXPECT_TRUE(rangeOuterInside->boundaryPointsValid());
    EXPECT_EQ(outer, rangeOuterInside->startContainer());
    EXPECT_EQ(1, rangeOuterInside->startOffset());
    EXPECT_EQ(outer, rangeOuterInside->endContainer());
    EXPECT_EQ(5, rangeOuterInside->endOffset());

    EXPECT_TRUE(rangeOuterSurroundingText->boundaryPointsValid());
    EXPECT_EQ(outer, rangeOuterSurroundingText->startContainer());
    EXPECT_EQ(2, rangeOuterSurroundingText->startOffset());
    EXPECT_EQ(outer, rangeOuterSurroundingText->endContainer());
    EXPECT_EQ(4, rangeOuterSurroundingText->endOffset());

    EXPECT_TRUE(rangeInnerLeft->boundaryPointsValid());
    EXPECT_EQ(innerLeft, rangeInnerLeft->startContainer());
    EXPECT_EQ(0, rangeInnerLeft->startOffset());
    EXPECT_EQ(innerLeft, rangeInnerLeft->endContainer());
    EXPECT_EQ(1, rangeInnerLeft->endOffset());

    EXPECT_TRUE(rangeInnerRight->boundaryPointsValid());
    EXPECT_EQ(innerRight, rangeInnerRight->startContainer());
    EXPECT_EQ(0, rangeInnerRight->startOffset());
    EXPECT_EQ(innerRight, rangeInnerRight->endContainer());
    EXPECT_EQ(1, rangeInnerRight->endOffset());

    EXPECT_TRUE(rangeFromTextToMiddleOfElement->boundaryPointsValid());
    EXPECT_EQ(newText, rangeFromTextToMiddleOfElement->startContainer());
    EXPECT_EQ(3, rangeFromTextToMiddleOfElement->startOffset());
    EXPECT_EQ(outer, rangeFromTextToMiddleOfElement->endContainer());
    EXPECT_EQ(4, rangeFromTextToMiddleOfElement->endOffset());
}

} // namespace blink
