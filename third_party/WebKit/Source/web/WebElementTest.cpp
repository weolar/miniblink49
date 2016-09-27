// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/web/WebElement.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/testing/DummyPageHolder.h"
#include <gtest/gtest.h>

namespace blink {

static const char s_blockWithContinuations[] =
    "<head> <style> form {display: inline;} </style> </head>"
    "<body>"
    "  <form>"
    "    <div id='testElement'>"
    "      <input type='password' id='password'/>"
    "    </div>"
    "  </form>"
    "</body>";

static const char s_emptyBlock[] =
    "<head> <style> form {display: inline;} </style> </head>"
    "<body> <form id='testElement'> </form> </body>";

static const char s_emptyInline[] =
    "<body> <span id='testElement'> </span> </body>";

static const char s_blockWithDisplayNone[] =
    "<head> <style> form {display: none;} </style> </head>"
    "<body>"
    "  <form id='testElement'>"
    "    <div>"
    "      <input type='password' id='password'/>"
    "    </div>"
    "  </form>"
    "</body>";

static const char s_blockWithContent[] =
    "<div id='testElement'>"
    "  <div>Hello</div> "
    "</div>";

static const char s_blockWithText[] =
    "<div id='testElement'>"
    "  <div>Hello</div> "
    "</div>";

static const char s_blockWithInlines[] =
    "<div id='testElement'>"
    "  <span>Hello</span> "
    "</div>";

static const char s_blockWithEmptyInlines[] =
    "<div id='testElement'>"
    "  <span></span> "
    "</div>";

class WebElementTest : public testing::Test {
protected:
    Document& document() { return m_pageHolder->document(); }
    void insertHTML(String html);
    WebElement testElement();

private:
    void SetUp() override;

    OwnPtr<DummyPageHolder> m_pageHolder;
};

void WebElementTest::insertHTML(String html)
{
    document().documentElement()->setInnerHTML(html, ASSERT_NO_EXCEPTION);
}

WebElement WebElementTest::testElement()
{
    return WebElement(document().getElementById("testElement"));
}

void WebElementTest::SetUp()
{
    m_pageHolder = DummyPageHolder::create(IntSize(800, 600));
}

TEST_F(WebElementTest, HasNonEmptyLayoutSize)
{
    insertHTML(s_emptyBlock);
    EXPECT_FALSE(testElement().hasNonEmptyLayoutSize());

    insertHTML(s_emptyInline);
    EXPECT_FALSE(testElement().hasNonEmptyLayoutSize());

    insertHTML(s_blockWithDisplayNone);
    EXPECT_FALSE(testElement().hasNonEmptyLayoutSize());

    insertHTML(s_blockWithEmptyInlines);
    EXPECT_FALSE(testElement().hasNonEmptyLayoutSize());

    insertHTML(s_blockWithContinuations);
    EXPECT_TRUE(testElement().hasNonEmptyLayoutSize());

    insertHTML(s_blockWithInlines);
    EXPECT_TRUE(testElement().hasNonEmptyLayoutSize());

    insertHTML(s_blockWithContent);
    EXPECT_TRUE(testElement().hasNonEmptyLayoutSize());

    insertHTML(s_blockWithText);
    EXPECT_TRUE(testElement().hasNonEmptyLayoutSize());

    insertHTML(s_emptyBlock);
    RefPtrWillBeRawPtr<ShadowRoot> root = document().getElementById("testElement")->createShadowRoot(ASSERT_NO_EXCEPTION);
    root->setInnerHTML("<div>Hello World</div>", ASSERT_NO_EXCEPTION);
    EXPECT_TRUE(testElement().hasNonEmptyLayoutSize());
}

} // namespace blink
