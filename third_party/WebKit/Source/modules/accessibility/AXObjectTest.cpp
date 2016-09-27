// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/accessibility/AXObject.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/testing/DummyPageHolder.h"
#include <gtest/gtest.h>

namespace blink {

class AXObjectTest : public testing::Test {
protected:
    Document& document() { return m_pageHolder->document(); }

private:
    void SetUp() override;

    OwnPtr<DummyPageHolder> m_pageHolder;
};

void AXObjectTest::SetUp()
{
    m_pageHolder = DummyPageHolder::create(IntSize(800, 600));
}

TEST_F(AXObjectTest, IsARIAWidget)
{
    String testContent = "<body>"
        "<span id=\"plain\">plain</span><br>"
        "<span id=\"button\" role=\"button\">button</span><br>"
        "<span id=\"button-parent\" role=\"button\"><span>button-parent</span></span><br>"
        "<span id=\"button-caps\" role=\"BUTTON\">button-caps</span><br>"
        "<span id=\"button-second\" role=\"another-role button\">button-second</span><br>"
        "<span id=\"aria-bogus\" aria-bogus=\"bogus\">aria-bogus</span><br>"
        "<span id=\"aria-selected\" aria-selected>aria-selected</span><br>"
        "<span id=\"haspopup\" aria-haspopup=\"true\">aria-haspopup-true</span><br>"
        "<div id=\"focusable\" tabindex=\"1\">focusable</div><br>"
        "<div tabindex=\"2\"><div id=\"focusable-parent\">focusable-parent</div></div><br>"
        "</body>";

    document().documentElement()->setInnerHTML(testContent, ASSERT_NO_EXCEPTION);
    document().updateLayout();
    Element* root(document().documentElement());
    EXPECT_FALSE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("plain")));
    EXPECT_TRUE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("button")));
    EXPECT_TRUE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("button-parent")));
    EXPECT_TRUE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("button-caps")));
    EXPECT_TRUE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("button-second")));
    EXPECT_FALSE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("aria-bogus")));
    EXPECT_TRUE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("aria-selected")));
    EXPECT_TRUE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("haspopup")));
    EXPECT_TRUE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("focusable")));
    EXPECT_TRUE(AXObject::isInsideFocusableElementOrARIAWidget(*root->getElementById("focusable-parent")));
}

}
