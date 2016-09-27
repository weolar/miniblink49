// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/LayoutTheme.h"

#include "core/dom/NodeComputedStyle.h"
#include "core/frame/FrameView.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLElement.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "core/style/ComputedStyle.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/graphics/Color.h"
#include <gtest/gtest.h>

namespace blink {

class LayoutThemeTest : public ::testing::Test {
protected:
    void SetUp() override;
    HTMLDocument& document() const { return *m_document; }
    void setHtmlInnerHTML(const char* htmlContent);

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
    HTMLDocument* m_document;
};

void LayoutThemeTest::SetUp()
{
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600));
    m_document = toHTMLDocument(&m_dummyPageHolder->document());
    ASSERT(m_document);
}

void LayoutThemeTest::setHtmlInnerHTML(const char* htmlContent)
{
    document().documentElement()->setInnerHTML(String::fromUTF8(htmlContent), ASSERT_NO_EXCEPTION);
    document().view()->updateAllLifecyclePhases();
}

inline Color outlineColor(Element* element)
{
    return element->computedStyle()->visitedDependentColor(CSSPropertyOutlineColor);
}

inline EBorderStyle outlineStyle(Element* element)
{
    return element->computedStyle()->outlineStyle();
}

TEST_F(LayoutThemeTest, ChangeFocusRingColor)
{
    setHtmlInnerHTML("<span id=span tabIndex=0>Span</span>");

    Element* span = document().getElementById(AtomicString("span"));
    EXPECT_NE(nullptr, span);
    EXPECT_NE(nullptr, span->layoutObject());

    Color customColor = makeRGB(123, 145, 167);

    // Checking unfocused style.
    EXPECT_EQ(BNONE, outlineStyle(span));
    EXPECT_NE(customColor, outlineColor(span));

    // Do focus.
    document().page()->focusController().setActive(true);
    document().page()->focusController().setFocused(true);
    span->focus();
    document().view()->updateAllLifecyclePhases();

    // Checking focused style.
    EXPECT_NE(BNONE, outlineStyle(span));
    EXPECT_NE(customColor, outlineColor(span));

    // Change focus ring color.
    LayoutTheme::theme().setCustomFocusRingColor(customColor);
    Page::platformColorsChanged();
    document().view()->updateAllLifecyclePhases();

    // Check that the focus ring color is updated.
    EXPECT_NE(BNONE, outlineStyle(span));
    EXPECT_EQ(customColor, outlineColor(span));
}

} // namespace blink
