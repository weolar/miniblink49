// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/input/EventHandler.h"

#include "core/dom/Document.h"
#include "core/dom/Range.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLDocument.h"
#include "core/page/AutoscrollController.h"
#include "core/page/Page.h"
#include "core/testing/CoreTestHelpers.h"
#include "core/testing/DummyPageHolder.h"
#include "platform/PlatformMouseEvent.h"
#include <gtest/gtest.h>

namespace blink {

class EventHandlerTest : public ::testing::Test {
protected:
    void SetUp() override;

    Page& page() const { return m_dummyPageHolder->page(); }
    Document& document() const { return m_dummyPageHolder->document(); }

    void setHtmlInnerHTML(const char* htmlContent);

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
};

void EventHandlerTest::SetUp()
{
    m_dummyPageHolder = DummyPageHolder::create(IntSize(300, 400));
}

void EventHandlerTest::setHtmlInnerHTML(const char* htmlContent)
{
    document().documentElement()->setInnerHTML(String::fromUTF8(htmlContent), ASSERT_NO_EXCEPTION);
    document().view()->updateAllLifecyclePhases();
}

TEST_F(EventHandlerTest, dragSelectionAfterScroll)
{
    setHtmlInnerHTML("<style> body { margin: 0px; } .upper { width: 300px; height: 400px; }"
        ".lower { margin: 0px; width: 300px; height: 400px; } .line { display: block; width: 300px; height: 30px; } </style>"
        "<div class=\"upper\"></div>"
        "<div class=\"lower\">"
        "<span class=\"line\">Line 1</span><span class=\"line\">Line 2</span><span class=\"line\">Line 3</span><span class=\"line\">Line 4</span><span class=\"line\">Line 5</span>"
        "<span class=\"line\">Line 6</span><span class=\"line\">Line 7</span><span class=\"line\">Line 8</span><span class=\"line\">Line 9</span><span class=\"line\">Line 10</span>"
        "</div>");

    FrameView* frameView = document().view();
    frameView->scrollTo(DoublePoint(0, 400));

    PlatformMouseEvent mouseDownEvent(
        IntPoint(0, 0),
        IntPoint(100, 200),
        LeftButton,
        PlatformEvent::MousePressed,
        1,
        static_cast<PlatformEvent::Modifiers>(0),
        WTF::currentTime());
    document().frame()->eventHandler().handleMousePressEvent(mouseDownEvent);

    PlatformMouseEvent mouseMoveEvent(
        IntPoint(100, 50),
        IntPoint(200, 250),
        LeftButton,
        PlatformEvent::MouseMoved,
        1,
        static_cast<PlatformEvent::Modifiers>(0),
        WTF::currentTime());
    document().frame()->eventHandler().handleMouseMoveEvent(mouseMoveEvent);

    page().autoscrollController().animate(WTF::currentTime());
    page().animator().serviceScriptedAnimations(WTF::currentTime());

    PlatformMouseEvent mouseUpEvent(
        IntPoint(100, 50),
        IntPoint(200, 250),
        LeftButton,
        PlatformEvent::MouseReleased,
        1,
        static_cast<PlatformEvent::Modifiers>(0),
        WTF::currentTime());
    document().frame()->eventHandler().handleMouseReleaseEvent(mouseUpEvent);

    FrameSelection& selection = document().frame()->selection();
    ASSERT_TRUE(selection.isRange());
    RefPtrWillBeRawPtr<Range> range = selection.toNormalizedRange();
    ASSERT_TRUE(range.get());
    EXPECT_EQ("Line 1\nLine 2", range->text());
}

} // namespace blink
