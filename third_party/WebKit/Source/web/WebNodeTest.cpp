// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/web/WebNode.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/testing/DummyPageHolder.h"
#include "public/web/WebElement.h"
#include "public/web/WebElementCollection.h"
#include <gtest/gtest.h>

namespace blink {

class WebNodeTest : public testing::Test {
protected:
    Document& document() { return m_pageHolder->document(); }

private:
    void SetUp() override;

    OwnPtr<DummyPageHolder> m_pageHolder;
};

void WebNodeTest::SetUp()
{
    m_pageHolder = DummyPageHolder::create(IntSize(800, 600));
}

TEST_F(WebNodeTest, GetElementsByHTMLTagName)
{
    document().documentElement()->setInnerHTML("<body><LABEL></LABEL><svg xmlns='http://www.w3.org/2000/svg'><label></label></svg></body>", ASSERT_NO_EXCEPTION);
    WebNode node(document().documentElement());
    // WebNode::getElementsByHTMLTagName returns only HTML elements.
    WebElementCollection collection = node.getElementsByHTMLTagName("label");
    EXPECT_EQ(1u, collection.length());
    EXPECT_TRUE(collection.firstItem().hasHTMLTagName("label"));
    // The argument should be lower-case.
    collection = node.getElementsByHTMLTagName("LABEL");
    EXPECT_EQ(0u, collection.length());
}

} // namespace blink
