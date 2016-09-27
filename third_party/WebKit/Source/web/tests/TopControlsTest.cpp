/*
 * Copyright (C) 2015 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "config.h"
#include "core/frame/TopControls.h"

#include "core/frame/FrameHost.h"
#include "core/frame/LocalFrame.h"
#include "core/layout/LayoutView.h"
#include "core/page/Page.h"
#include "platform/testing/URLTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebSettings.h"
#include "web/WebLocalFrameImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blink {

// These tests cover top controls scrolling on main-thread.
// The animation for completing a partial show/hide is done in compositor so
// it is not covered here.
class TopControlsTest : public testing::Test {
public:
    TopControlsTest()
        : m_baseURL("http://www.test.com/")
    {
        registerMockedHttpURLLoad("large-div.html");
        registerMockedHttpURLLoad("overflow-scrolling.html");
        registerMockedHttpURLLoad("iframe-scrolling.html");
        registerMockedHttpURLLoad("iframe-scrolling-inner.html");
    }

    ~TopControlsTest() override
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

    WebViewImpl* initialize(const std::string& pageName = "large-div.html")
    {
        // Load a page with large body and set viewport size to 400x400 to ensure
        // main frame is scrollable.
        m_helper.initializeAndLoad(m_baseURL + pageName, true, 0, 0, &configureSettings);

        webViewImpl()->resize(IntSize(400, 400));
        return webViewImpl();
    }

    static void configureSettings(WebSettings* settings)
    {
        settings->setJavaScriptEnabled(true);
        settings->setAcceleratedCompositingEnabled(true);
        settings->setPreferCompositingToLCDTextEnabled(true);
        // Android settings
        settings->setViewportEnabled(true);
        settings->setViewportMetaEnabled(true);
        settings->setShrinksViewportContentToFit(true);
        settings->setMainFrameResizesAreOrientationChanges(true);
    }

    void registerMockedHttpURLLoad(const std::string& fileName)
    {
        URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(fileName.c_str()));
    }

    WebGestureEvent generateEvent(WebInputEvent::Type type, int deltaX = 0, int deltaY = 0)
    {
        WebGestureEvent event;
        event.type = type;
        event.x = 100;
        event.y = 100;
        if (type == WebInputEvent::GestureScrollUpdate) {
            event.data.scrollUpdate.deltaX = deltaX;
            event.data.scrollUpdate.deltaY = deltaY;
        }
        return event;
    }

    void verticalScroll(float deltaY)
    {
        webViewImpl()->handleInputEvent(generateEvent(WebInputEvent::GestureScrollBegin));
        webViewImpl()->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, deltaY));
        webViewImpl()->handleInputEvent(generateEvent(WebInputEvent::GestureScrollEnd));
    }

    WebViewImpl* webViewImpl() const { return m_helper.webViewImpl(); }
    LocalFrame* frame() const { return m_helper.webViewImpl()->mainFrameImpl()->frame(); }
    PinchViewport& pinchViewport() const { return m_helper.webViewImpl()->page()->frameHost().pinchViewport(); }

private:
    std::string m_baseURL;
    FrameTestHelpers::WebViewHelper m_helper;

    // To prevent platform differences in content layout, use mock
    // scrollbars. This is especially needed for Mac, where the presence
    // or absence of a mouse will change frame sizes because of different
    // scrollbar themes.
    FrameTestHelpers::UseMockScrollbarSettings m_useMockScrollbars;
};

#define EXPECT_POINT_EQ(expected, actual)               \
    do {                                                \
        EXPECT_DOUBLE_EQ((expected).x(), (actual).x()); \
        EXPECT_DOUBLE_EQ((expected).y(), (actual).y()); \
    } while (false)

// Disable these tests on Mac OSX until further investigation.
// Local build on Mac is OK but the bot fails. This is not an issue as
// Top Controls are currently only used on Android.
#if OS(MACOSX)
#define MAYBE(test) DISABLED_##test
#else
#define MAYBE(test) test
#endif

// Scrolling down should hide top controls.
TEST_F(TopControlsTest, MAYBE(HideOnScrollDown))
{
    WebViewImpl* webView = initialize();
    // initialize top controls to be shown.
    webView->setTopControlsHeight(50.f, true);
    webView->topControls().setShownRatio(1);

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollBegin));
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());

    // Top controls should be scrolled partially and page should not scroll.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, -25.f));
    EXPECT_FLOAT_EQ(25.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 0), frame()->view()->scrollPosition());

    // Top controls should consume 30px and become hidden. Excess scroll should be consumed by the page.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, -40.f));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 15), frame()->view()->scrollPosition());

    // Only page should consume scroll
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, -20.f));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 35), frame()->view()->scrollPosition());
}

// Scrolling up should show top controls.
TEST_F(TopControlsTest, MAYBE(ShowOnScrollUp))
{
    WebViewImpl* webView = initialize();
    // initialize top controls to be hidden.
    webView->setTopControlsHeight(50.f, false);
    webView->topControls().setShownRatio(0);

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollBegin));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, 10.f));
    EXPECT_FLOAT_EQ(10.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 0), frame()->view()->scrollPosition());

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, 50.f));
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 0), frame()->view()->scrollPosition());
}

// Scrolling up after previous scroll downs should cause top controls to be
// shown only after all previously scrolled down amount is compensated.
TEST_F(TopControlsTest, MAYBE(ScrollDownThenUp))
{
    WebViewImpl* webView = initialize();
    // initialize top controls to be shown and position page at 100px.
    webView->setTopControlsHeight(50.f, true);
    webView->topControls().setShownRatio(1);
    frame()->view()->scrollableArea()->setScrollPosition(IntPoint(0, 100), ProgrammaticScroll);

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollBegin));
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());

    // Scroll down to completely hide top controls. Excess deltaY (100px) should be consumed by the page.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, -150.f));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 200), frame()->view()->scrollPosition());

    // Scroll up and ensure the top controls does not move until we recover 100px previously scrolled.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, 40.f));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 160), frame()->view()->scrollPosition());

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, 60.f));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 100), frame()->view()->scrollPosition());

    // Now we have hit the threshold so further scroll up should be consumed by top controls.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, 30.f));
    EXPECT_FLOAT_EQ(30.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 100), frame()->view()->scrollPosition());

    // Once top control is fully shown then page should consume any excess scroll.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, 70.f));
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 50), frame()->view()->scrollPosition());
}

// Scrolling down should always cause visible top controls to start hiding even
// if we have been scrolling up previously.
TEST_F(TopControlsTest, MAYBE(ScrollUpThenDown))
{
    WebViewImpl* webView = initialize();
    // initialize top controls to be hidden and position page at 100px.
    webView->setTopControlsHeight(50.f, false);
    webView->topControls().setShownRatio(0);
    frame()->view()->scrollableArea()->setScrollPosition(IntPoint(0, 100), ProgrammaticScroll);

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollBegin));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());

    // Scroll up to completely show top controls. Excess deltaY (50px) should be consumed by the page.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, 100.f));
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 50), frame()->view()->scrollPosition());

    // Scroll down and ensure only top controls is scrolled
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, -40.f));
    EXPECT_FLOAT_EQ(10.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 50), frame()->view()->scrollPosition());

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, -60.f));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 100), frame()->view()->scrollPosition());
}

// Top controls should not consume horizontal scroll.
TEST_F(TopControlsTest, MAYBE(HorizontalScroll))
{
    WebViewImpl* webView = initialize();
    // initialize top controls to be shown.
    webView->setTopControlsHeight(50.f, true);
    webView->topControls().setShownRatio(1);

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollBegin));
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());

    // Top controls should not consume horizontal scroll.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, -110.f, -100.f));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(110, 50), frame()->view()->scrollPosition());

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, -40.f, 0));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(150, 50), frame()->view()->scrollPosition());
}

// Page scale should not impact top controls scrolling
TEST_F(TopControlsTest, MAYBE(PageScaleHasNoImpact))
{
    WebViewImpl* webView = initialize();
    webViewImpl()->setDefaultPageScaleLimits(0.25f, 5);
    webView->setPageScaleFactor(2.0);

    // Initialize top controls to be shown.
    webView->setTopControlsHeight(50.f, true);
    webView->topControls().setShownRatio(1);

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollBegin));
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());

    // Top controls should be scrolled partially and page should not scroll.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, -20.f));
    EXPECT_FLOAT_EQ(30.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 0), frame()->view()->scrollableArea()->scrollPositionDouble());

    // Top controls should consume 30px and become hidden. Excess scroll should be consumed by the page at 2x scale.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, -70.f));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 20), frame()->view()->scrollableArea()->scrollPositionDouble());

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollEnd));

    // Change page scale and test.
    webView->setPageScaleFactor(0.5);

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollBegin));
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 20), frame()->view()->scrollableArea()->scrollPositionDouble());

    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, 50.f));
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 20), frame()->view()->scrollableArea()->scrollPositionDouble());

    // At 0.5x scale scrolling 10px should take us to the top of the page.
    webView->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, 0, 10.f));
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 0), frame()->view()->scrollableArea()->scrollPositionDouble());
}

// Some scroll deltas result in a shownRatio that can't be realized in a
// floating-point number. Make sure that if the top controls aren't fully
// scrolled, scrollBy doesn't return any excess delta. i.e. There should be no
// slippage between the content and top controls.
TEST_F(TopControlsTest, MAYBE(FloatingPointSlippage))
{
    WebViewImpl* webView = initialize();
    webViewImpl()->setDefaultPageScaleLimits(0.25f, 5);
    webView->setPageScaleFactor(2.0);

    // Initialize top controls to be shown.
    webView->setTopControlsHeight(50.f, true);
    webView->topControls().setShownRatio(1);

    webView->topControls().scrollBegin();
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());

    // This will result in a 20px scroll to the top controls so the show ratio
    // will be 30/50 == 0.6 which is not representible in a float. Make sure
    // that scroll still consumes the whole delta.
    FloatSize remainingDelta = webView->topControls().scrollBy(FloatSize(0, -10));
    EXPECT_EQ(0, remainingDelta.height());
}

// Scrollable subregions should scroll before top controls
TEST_F(TopControlsTest, MAYBE(ScrollableSubregionScrollFirst))
{
    WebViewImpl* webView = initialize("overflow-scrolling.html");
    webView->setTopControlsHeight(50.f, true);
    webView->topControls().setShownRatio(1);
    frame()->view()->scrollableArea()->setScrollPosition(IntPoint(0, 50), ProgrammaticScroll);

    // Test scroll down
    // Scroll down should scroll the overflow div first but top controls and main frame should not scroll.
    verticalScroll(-800.f);
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 50), frame()->view()->scrollPosition());

    // Continued scroll down should start hiding top controls but main frame should not scroll.
    verticalScroll(-40.f);
    EXPECT_FLOAT_EQ(10.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 50), frame()->view()->scrollPosition());

    // Continued scroll down should scroll down the main frame
    verticalScroll(-40.f);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 80), frame()->view()->scrollPosition());

    // Test scroll up
    // scroll up should scroll overflow div first
    verticalScroll(800.f);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 80), frame()->view()->scrollPosition());

    // Continued scroll up should start showing top controls but main frame should not scroll.
    verticalScroll(40.f);
    EXPECT_FLOAT_EQ(40.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 80), frame()->view()->scrollPosition());

    // Continued scroll down up scroll up the main frame
    verticalScroll(40.f);
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 50), frame()->view()->scrollPosition());
}


// Scrollable iframes should scroll before top controls
TEST_F(TopControlsTest, MAYBE(ScrollableIframeScrollFirst))
{
    WebViewImpl* webView = initialize("iframe-scrolling.html");
    webView->setTopControlsHeight(50.f, true);
    webView->topControls().setShownRatio(1);
    frame()->view()->scrollableArea()->setScrollPosition(IntPoint(0, 50), ProgrammaticScroll);

    // Test scroll down
    // Scroll down should scroll the iframe first but top controls and main frame should not scroll.
    verticalScroll(-800.f);
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 50), frame()->view()->scrollPosition());

    // Continued scroll down should start hiding top controls but main frame should not scroll.
    verticalScroll(-40.f);
    EXPECT_FLOAT_EQ(10.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 50), frame()->view()->scrollPosition());

    // Continued scroll down should scroll down the main frame
    verticalScroll(-40.f);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 80), frame()->view()->scrollPosition());

    // Test scroll up
    // scroll up should scroll iframe first
    verticalScroll(800.f);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 80), frame()->view()->scrollPosition());

    // Continued scroll up should start showing top controls but main frame should not scroll.
    verticalScroll(40.f);
    EXPECT_FLOAT_EQ(40.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 80), frame()->view()->scrollPosition());

    // Continued scroll down up scroll up the main frame
    verticalScroll(40.f);
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 50), frame()->view()->scrollPosition());
}

// Top controls visibility should remain consistent when height is changed.
TEST_F(TopControlsTest, MAYBE(HeightChangeMaintainsVisibility))
{
    WebViewImpl* webView = initialize();
    webView->setTopControlsHeight(20.f, false);
    webView->topControls().setShownRatio(0);

    webView->setTopControlsHeight(20.f, false);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());

    webView->setTopControlsHeight(40.f, false);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());

    // Scroll up to show top controls.
    verticalScroll(40.f);
    EXPECT_FLOAT_EQ(40.f, webView->topControls().contentOffset());

    // Changing height of a fully shown top controls should correctly adjust content offset
    webView->setTopControlsHeight(30.f, false);
    EXPECT_FLOAT_EQ(30.f, webView->topControls().contentOffset());
}

// Zero delta should not have any effect on top controls.
TEST_F(TopControlsTest, MAYBE(ZeroHeightMeansNoEffect))
{
    WebViewImpl* webView = initialize();
    webView->setTopControlsHeight(0, false);
    webView->topControls().setShownRatio(0);
    frame()->view()->scrollableArea()->setScrollPosition(IntPoint(0, 100), ProgrammaticScroll);

    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());

    verticalScroll(20.f);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 80), frame()->view()->scrollPosition());

    verticalScroll(-30.f);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 110), frame()->view()->scrollPosition());

    webView->topControls().setShownRatio(1);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());
}

// Top controls should not hide when scrolling up past limit
TEST_F(TopControlsTest, MAYBE(ScrollUpPastLimitDoesNotHide))
{
    WebViewImpl* webView = initialize();
    // Initialize top controls to be shown
    webView->setTopControlsHeight(50.f, true);
    webView->topControls().setShownRatio(1);
    // Use 2x scale so that both pinch viewport and frameview are scrollable
    webView->setPageScaleFactor(2.0);

    // Fully scroll frameview but pinchviewport remains scrollable
    webView->mainFrame()->setScrollOffset(WebSize(0, 10000));
    pinchViewport().setLocation(FloatPoint(0, 0));
    verticalScroll(-10.f);
    EXPECT_FLOAT_EQ(40, webView->topControls().contentOffset());

    webView->topControls().setShownRatio(1);
    // Fully scroll pinch veiwport but frameview remains scrollable
    webView->mainFrame()->setScrollOffset(WebSize(0, 0));
    pinchViewport().setLocation(FloatPoint(0, 10000));
    verticalScroll(-20.f);
    EXPECT_FLOAT_EQ(30, webView->topControls().contentOffset());

    webView->topControls().setShownRatio(1);
    // Fully scroll both frameview and pinch viewort
    webView->mainFrame()->setScrollOffset(WebSize(0, 10000));
    pinchViewport().setLocation(FloatPoint(0, 10000));
    verticalScroll(-30.f);
    // Top controls should not move because neither frameview nor pinch viewport
    // are scrollable
    EXPECT_FLOAT_EQ(50.f, webView->topControls().contentOffset());
}

// Top controls should honor its constraints
TEST_F(TopControlsTest, MAYBE(StateConstraints))
{
    WebViewImpl* webView = initialize();
    webView->setTopControlsHeight(50.f, false);
    frame()->view()->scrollableArea()->setScrollPosition(IntPoint(0, 100), ProgrammaticScroll);

    // Setting permitted state should not change content offset
    webView->updateTopControlsState(WebTopControlsShown, WebTopControlsShown, false);
    EXPECT_FLOAT_EQ(0.f, webView->topControls().contentOffset());

    // Showing is permitted
    webView->topControls().setShownRatio(1);
    EXPECT_FLOAT_EQ(50, webView->topControls().contentOffset());

    // Only shown state is permitted so controls cannot hide
    verticalScroll(-20.f);
    EXPECT_FLOAT_EQ(50, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 120), frame()->view()->scrollPosition());

    // Setting permitted state should not change content offset
    webView->updateTopControlsState(WebTopControlsHidden, WebTopControlsHidden, false);
    EXPECT_FLOAT_EQ(50, webView->topControls().contentOffset());

    // Hiding is permitted
    webView->topControls().setShownRatio(0);
    EXPECT_FLOAT_EQ(0, webView->topControls().contentOffset());

    // Only hidden state is permitted so controls cannot show
    verticalScroll(30.f);
    EXPECT_FLOAT_EQ(0, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 90), frame()->view()->scrollPosition());

    // Setting permitted state should not change content offset
    webView->updateTopControlsState(WebTopControlsBoth, WebTopControlsBoth, false);
    EXPECT_FLOAT_EQ(0, webView->topControls().contentOffset());

    // Both states are permitted so controls can either show or hide
    verticalScroll(50.f);
    EXPECT_FLOAT_EQ(50, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 90), frame()->view()->scrollPosition());

    verticalScroll(-50.f);
    EXPECT_FLOAT_EQ(0, webView->topControls().contentOffset());
    EXPECT_POINT_EQ(IntPoint(0, 90), frame()->view()->scrollPosition());
}

} // namespace blink
