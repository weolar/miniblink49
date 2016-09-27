/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PageScaleConstraints.h"
#include "core/frame/Settings.h"
#include "core/page/Page.h"
#include "platform/Length.h"
#include "platform/geometry/IntPoint.h"
#include "platform/geometry/IntRect.h"
#include "platform/geometry/IntSize.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "platform/testing/URLTestHelpers.h"
#include "platform/testing/UnitTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebConsoleMessage.h"
#include "public/web/WebFrame.h"
#include "public/web/WebScriptSource.h"
#include "public/web/WebSettings.h"
#include "public/web/WebViewClient.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>
#include <vector>

namespace blink {

using blink::FrameTestHelpers::UseMockScrollbarSettings;
using blink::testing::runPendingTasks;

class ViewportTest : public ::testing::Test {
protected:
    ViewportTest()
        : m_baseURL("http://www.test.com/")
        , m_chromeURL("chrome://")
    {
    }

    ~ViewportTest() override
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

    void registerMockedHttpURLLoad(const std::string& fileName)
    {
        URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(fileName.c_str()));
    }

    void registerMockedChromeURLLoad(const std::string& fileName)
    {
        URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_chromeURL.c_str()), WebString::fromUTF8(fileName.c_str()));
    }

    void executeScript(WebFrame* frame, const WebString& code)
    {
        frame->executeScript(WebScriptSource(code));
        runPendingTasks();
    }

    std::string m_baseURL;
    std::string m_chromeURL;
};

static void setViewportSettings(WebSettings* settings)
{
    settings->setViewportEnabled(true);
    settings->setViewportMetaEnabled(true);
    settings->setMainFrameResizesAreOrientationChanges(true);
}

static PageScaleConstraints runViewportTest(Page* page, int initialWidth, int initialHeight)
{
    IntSize initialViewportSize(initialWidth, initialHeight);
    toLocalFrame(page->mainFrame())->view()->setFrameRect(IntRect(IntPoint::zero(), initialViewportSize));
    ViewportDescription description = page->viewportDescription();
    PageScaleConstraints constraints = description.resolve(initialViewportSize, Length(980, blink::Fixed));

    constraints.fitToContentsWidth(constraints.layoutSize.width(), initialWidth);
    constraints.resolveAutoInitialScale();
    return constraints;
}

TEST_F(ViewportTest, viewport1)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-1.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-1.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport2)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-2.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-2.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(0.32f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.32f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport3)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-3.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-3.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport4)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-4.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-4.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(160, constraints.layoutSize.width());
    EXPECT_EQ(176, constraints.layoutSize.height());
    EXPECT_NEAR(2.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(2.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport5)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-5.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-5.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(640, constraints.layoutSize.width());
    EXPECT_EQ(704, constraints.layoutSize.height());
    EXPECT_NEAR(0.5f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.5f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport6)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-6.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-6.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(200, constraints.layoutSize.width());
    EXPECT_EQ(220, constraints.layoutSize.height());
    EXPECT_NEAR(1.6f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.6f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport7)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-7.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-7.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1280, constraints.layoutSize.width());
    EXPECT_EQ(1408, constraints.layoutSize.height());
    EXPECT_NEAR(0.25f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport8)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-8.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-8.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1280, constraints.layoutSize.width());
    EXPECT_EQ(1408, constraints.layoutSize.height());
    EXPECT_NEAR(0.25f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport9)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-9.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-9.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1280, constraints.layoutSize.width());
    EXPECT_EQ(1408, constraints.layoutSize.height());
    EXPECT_NEAR(0.25f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport10)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-10.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-10.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1280, constraints.layoutSize.width());
    EXPECT_EQ(1408, constraints.layoutSize.height());
    EXPECT_NEAR(0.25f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport11)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-11.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-11.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.32f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.32f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(0.5f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport12)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-12.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-12.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(640, constraints.layoutSize.width());
    EXPECT_EQ(704, constraints.layoutSize.height());
    EXPECT_NEAR(0.5f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.5f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(0.5f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport13)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-13.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-13.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1280, constraints.layoutSize.width());
    EXPECT_EQ(1408, constraints.layoutSize.height());
    EXPECT_NEAR(0.25f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(0.5f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport14)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-14.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-14.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport15)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-15.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-15.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport16)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-16.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-16.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport17)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-17.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-17.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport18)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-18.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-18.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport19)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-19.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-19.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(160, constraints.layoutSize.width());
    EXPECT_EQ(176, constraints.layoutSize.height());
    EXPECT_NEAR(2.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(2.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport20)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-20.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-20.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(10.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport21)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-21.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-21.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(10.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport22)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-22.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-22.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(10.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport23)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-23.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-23.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(3.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(3.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(3.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport24)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-24.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-24.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(4.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(4.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(4.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport25)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-25.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-25.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(10.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport26)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-26.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-26.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(8.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(8.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(9.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport27)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-27.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-27.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.32f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.32f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport28)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-28.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-28.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(352, constraints.layoutSize.width());
    EXPECT_NEAR(387.2, constraints.layoutSize.height(), 0.01);
    EXPECT_NEAR(0.91f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.91f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport29)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-29.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-29.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(700, constraints.layoutSize.width());
    EXPECT_EQ(770, constraints.layoutSize.height());
    EXPECT_NEAR(0.46f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.46f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport30)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-30.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-30.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(200, constraints.layoutSize.width());
    EXPECT_EQ(220, constraints.layoutSize.height());
    EXPECT_NEAR(1.6f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.6f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport31)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-31.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-31.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(700, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport32)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-32.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-32.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(200, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport33)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-33.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-33.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(2.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport34)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-34.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-34.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(640, constraints.layoutSize.width());
    EXPECT_EQ(704, constraints.layoutSize.height());
    EXPECT_NEAR(0.5f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.5f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport35)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-35.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-35.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1280, constraints.layoutSize.width());
    EXPECT_EQ(1408, constraints.layoutSize.height());
    EXPECT_NEAR(0.25f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport36)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-36.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-36.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_NEAR(636.36, constraints.layoutSize.width(), 0.01f);
    EXPECT_EQ(700, constraints.layoutSize.height());
    EXPECT_NEAR(1.6f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.50f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport37)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-37.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-37.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport38)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-38.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-38.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(640, constraints.layoutSize.width());
    EXPECT_EQ(704, constraints.layoutSize.height());
    EXPECT_NEAR(0.5f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.5f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport39)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-39.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-39.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(200, constraints.layoutSize.width());
    EXPECT_EQ(700, constraints.layoutSize.height());
    EXPECT_NEAR(1.6f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.6f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport40)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-40.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-40.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(700, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(0.46f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.46f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport41)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-41.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-41.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1000, constraints.layoutSize.width());
    EXPECT_EQ(704, constraints.layoutSize.height());
    EXPECT_NEAR(0.5f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.32f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport42)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-42.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-42.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(1000, constraints.layoutSize.height());
    EXPECT_NEAR(2.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport43)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-43.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-43.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport44)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-44.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-44.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(10000, constraints.layoutSize.width());
    EXPECT_EQ(10000, constraints.layoutSize.height());
    EXPECT_NEAR(0.25f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport45)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-45.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-45.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(3200, constraints.layoutSize.width());
    EXPECT_EQ(3520, constraints.layoutSize.height());
    EXPECT_NEAR(0.1f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.1f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(0.1f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport46)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-46.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-46.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(32, constraints.layoutSize.width());
    EXPECT_NEAR(35.2, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(10.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport47)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-47.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-47.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(3000, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport48)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-48.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-48.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(3000, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport49)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-49.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-49.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport50)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-50.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-50.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport51)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-51.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-51.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport52)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-52.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-52.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport53)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-53.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-53.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport54)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-54.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-54.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport55)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-55.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-55.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport56)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-56.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-56.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport57)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-57.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-57.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport58)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-58.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-58.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(3200, constraints.layoutSize.width());
    EXPECT_EQ(3520, constraints.layoutSize.height());
    EXPECT_NEAR(0.1f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.1f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport59)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-59.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-59.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport60)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-60.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-60.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(32, constraints.layoutSize.width());
    EXPECT_NEAR(35.2, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(10.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport61)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-61.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-61.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport62)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-62.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-62.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport63)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-63.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-63.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport64)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-64.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-64.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport65)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-65.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-65.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(100, constraints.layoutSize.width());
    EXPECT_EQ(110, constraints.layoutSize.height());
    EXPECT_NEAR(3.2f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(3.2f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport66)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-66.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-66.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(100, constraints.layoutSize.width());
    EXPECT_EQ(110, constraints.layoutSize.height());
    EXPECT_NEAR(3.2f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(3.2f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport67)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-67.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-67.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport68)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-68.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-68.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport69)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-69.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-69.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(100, constraints.layoutSize.width());
    EXPECT_EQ(110, constraints.layoutSize.height());
    EXPECT_NEAR(3.2f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(3.2f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport70)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-70.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-70.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(100, constraints.layoutSize.width());
    EXPECT_EQ(110, constraints.layoutSize.height());
    EXPECT_NEAR(3.2f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(3.2f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport71)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-71.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-71.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport72)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-72.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-72.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(100, constraints.layoutSize.width());
    EXPECT_EQ(110, constraints.layoutSize.height());
    EXPECT_NEAR(3.2f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(3.2f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport73)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-73.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-73.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(100, constraints.layoutSize.width());
    EXPECT_EQ(110, constraints.layoutSize.height());
    EXPECT_NEAR(3.2f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(3.2f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport74)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-74.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-74.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(100, constraints.layoutSize.width());
    EXPECT_EQ(110, constraints.layoutSize.height());
    EXPECT_NEAR(3.2f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(3.2f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport75)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-75.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-75.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport76)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-76.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-76.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(32, constraints.layoutSize.width());
    EXPECT_NEAR(35.2, constraints.layoutSize.height(), 0.01);
    EXPECT_NEAR(10.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport77)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-77.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-77.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1280, constraints.layoutSize.width());
    EXPECT_EQ(1408, constraints.layoutSize.height());
    EXPECT_NEAR(0.25f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport78)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-78.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-78.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(100, constraints.layoutSize.width());
    EXPECT_EQ(110, constraints.layoutSize.height());
    EXPECT_NEAR(3.2f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(3.2f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport79)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-79.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-79.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport80)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-80.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-80.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport81)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-81.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-81.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(3000, constraints.layoutSize.width());
    EXPECT_EQ(3300, constraints.layoutSize.height());
    EXPECT_NEAR(0.25f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.25f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport82)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-82.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-82.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(400, constraints.layoutSize.width());
    EXPECT_EQ(440, constraints.layoutSize.height());
    EXPECT_NEAR(0.8f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.8f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport83)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-83.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-83.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport84)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-84.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-84.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_EQ(480, constraints.layoutSize.height());
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport85)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-85.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-85.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(540, constraints.layoutSize.width());
    EXPECT_EQ(594, constraints.layoutSize.height());
    EXPECT_NEAR(0.59f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.59f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport86)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-86.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-86.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_NEAR(457.14, constraints.layoutSize.width(), 0.01f);
    EXPECT_NEAR(502.86, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(0.7f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.7f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport87)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-87.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-87.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport88)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-88.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-88.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport90)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-90.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-90.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(700, constraints.layoutSize.width());
    EXPECT_EQ(770, constraints.layoutSize.height());
    EXPECT_NEAR(0.5f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.46f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport100)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-100.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-100.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(400, constraints.layoutSize.width());
    EXPECT_EQ(440, constraints.layoutSize.height());
    EXPECT_NEAR(0.8f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.8f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport101)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-101.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-101.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(400, constraints.layoutSize.width());
    EXPECT_EQ(440, constraints.layoutSize.height());
    EXPECT_NEAR(0.8f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.8f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport102)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-102.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-102.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(400, constraints.layoutSize.width());
    EXPECT_EQ(440, constraints.layoutSize.height());
    EXPECT_NEAR(0.8f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.8f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport103)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-103.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-103.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(400, constraints.layoutSize.width());
    EXPECT_EQ(440, constraints.layoutSize.height());
    EXPECT_NEAR(0.8f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.8f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport104)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-104.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-104.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport105)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-105.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-105.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport106)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-106.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-106.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport107)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-107.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-107.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport108)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-108.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-108.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport109)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-109.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-109.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport110)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-110.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-110.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport111)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-111.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-111.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport112)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-112.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-112.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(400, constraints.layoutSize.width());
    EXPECT_EQ(440, constraints.layoutSize.height());
    EXPECT_NEAR(0.8f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.8f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport113)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-113.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-113.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport114)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-114.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-114.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport115)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-115.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-115.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(400, constraints.layoutSize.width());
    EXPECT_EQ(440, constraints.layoutSize.height());
    EXPECT_NEAR(0.8f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.8f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport116)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-116.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-116.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(400, constraints.layoutSize.width());
    EXPECT_EQ(440, constraints.layoutSize.height());
    EXPECT_NEAR(0.8f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.8f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport117)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-117.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-117.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(400, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport118)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-118.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-118.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport119)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-119.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-119.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport120)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-120.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-120.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport121)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-121.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-121.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport122)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-122.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-122.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport123)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-123.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-123.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport124)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-124.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-124.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport125)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-125.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-125.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport126)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-126.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-126.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport127)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-127.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-127.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(64, constraints.layoutSize.width());
    EXPECT_NEAR(70.4, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport129)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-129.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-129.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(123, constraints.layoutSize.width());
    EXPECT_NEAR(135.3, constraints.layoutSize.height(), 0.01f);
    EXPECT_NEAR(2.60f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(2.60f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport130)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-130.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-130.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport131)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-131.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-131.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.maximumScale, 0.01f);
    EXPECT_FALSE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport132)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-132.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-132.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport133)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-133.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-133.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(10.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(10.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport134)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-134.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-134.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(160, constraints.layoutSize.width());
    EXPECT_EQ(176, constraints.layoutSize.height());
    EXPECT_NEAR(2.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(2.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport135)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-135.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-135.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport136)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-136.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-136.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport137)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-137.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-137.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewport138)
{
    registerMockedHttpURLLoad("viewport/viewport-138.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-138.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_NEAR(123.0f, constraints.layoutSize.width(), 0.01);
    EXPECT_NEAR(135.3f, constraints.layoutSize.height(), 0.01);
    EXPECT_NEAR(2.60f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(2.60f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyHandheldFriendly)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-handheldfriendly.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-handheldfriendly.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

static void setQuirkViewportSettings(WebSettings* settings)
{
    setViewportSettings(settings);

    // This quirk allows content attributes of meta viewport tags to be merged.
    settings->setViewportMetaMergeContentQuirk(true);
}

TEST_F(ViewportTest, viewportLegacyMergeQuirk1)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-merge-quirk-1.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-merge-quirk-1.html", true, 0, 0, setQuirkViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(640, constraints.layoutSize.width());
    EXPECT_EQ(704, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.maximumScale, 0.01f);
    EXPECT_FALSE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyMergeQuirk2)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-merge-quirk-2.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-merge-quirk-2.html", true, 0, 0, setQuirkViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    // This quirk allows content attributes of meta viewport tags to be merged.
    page->settings().setViewportMetaMergeContentQuirk(true);
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(500, constraints.layoutSize.width());
    EXPECT_EQ(550, constraints.layoutSize.height());
    EXPECT_NEAR(2.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(2.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(2.0f, constraints.maximumScale, 0.01f);
    EXPECT_FALSE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyMobileOptimizedMetaWithoutContent)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-mobileoptimized.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-mobileoptimized.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyMobileOptimizedMetaWith0)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-mobileoptimized-2.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-mobileoptimized-2.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyMobileOptimizedMetaWith400)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-mobileoptimized-2.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-mobileoptimized-2.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyOrdering2)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-ordering-2.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-ordering-2.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(300, constraints.layoutSize.width());
    EXPECT_EQ(330, constraints.layoutSize.height());
    EXPECT_NEAR(1.07f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.07f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyOrdering3)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-ordering-3.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-ordering-3.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(300, constraints.layoutSize.width());
    EXPECT_EQ(330, constraints.layoutSize.height());
    EXPECT_NEAR(1.07f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.07f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyOrdering4)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-ordering-4.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-ordering-4.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(300, constraints.layoutSize.width());
    EXPECT_EQ(330, constraints.layoutSize.height());
    EXPECT_NEAR(1.07f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.07f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyOrdering5)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-ordering-5.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-ordering-5.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyOrdering6)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-ordering-6.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-ordering-6.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyOrdering7)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-ordering-7.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-ordering-7.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(300, constraints.layoutSize.width());
    EXPECT_EQ(330, constraints.layoutSize.height());
    EXPECT_NEAR(1.07f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.07f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyOrdering8)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-ordering-8.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-ordering-8.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(300, constraints.layoutSize.width());
    EXPECT_EQ(330, constraints.layoutSize.height());
    EXPECT_NEAR(1.07f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.07f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyEmptyAtViewportDoesntOverrideViewportMeta)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-ordering-10.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-ordering-10.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 800, 600);

    EXPECT_EQ(5000, constraints.layoutSize.width());
}

TEST_F(ViewportTest, viewportLegacyDefaultValueChangedByXHTMLMP)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-xhtmlmp.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-xhtmlmp.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyDefaultValueChangedByXHTMLMPAndOverriddenByMeta)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-xhtmlmp-misplaced-doctype.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-xhtmlmp-misplaced-doctype.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(640, constraints.layoutSize.width());
    EXPECT_EQ(704, constraints.layoutSize.height());
    EXPECT_NEAR(0.5f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.5f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyXHTMLMPOrdering)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-xhtmlmp-ordering.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-xhtmlmp-ordering.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(640, constraints.layoutSize.width());
    EXPECT_EQ(704, constraints.layoutSize.height());
    EXPECT_NEAR(0.5f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.5f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLegacyXHTMLMPRemoveAndAdd)
{
    registerMockedHttpURLLoad("viewport/viewport-legacy-xhtmlmp.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-legacy-xhtmlmp.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);

    executeScript(webViewHelper.webViewImpl()->mainFrame(),
        "originalDoctype = document.doctype;"
        "document.removeChild(originalDoctype);");

    constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);

    executeScript(webViewHelper.webViewImpl()->mainFrame(),
        "document.insertBefore(originalDoctype, document.firstChild);");

    constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLimitsAdjustedForNoUserScale)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-limits-adjusted-for-no-user-scale.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-limits-adjusted-for-no-user-scale.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    EXPECT_FALSE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportLimitsAdjustedForNoUserScaleControl)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-limits-adjusted-for-no-user-scale-control.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-limits-adjusted-for-no-user-scale-control.html", true, 0, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();

    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportTriggersGpuRasterization)
{
    UseMockScrollbarSettings mockScrollbarSettings;

    registerMockedHttpURLLoad("viewport/viewport-gpu-rasterization.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-gpu-rasterization.html", true, 0, 0, setViewportSettings);
    webViewHelper.webView()->resize(WebSize(640, 480));
    EXPECT_TRUE(webViewHelper.webViewImpl()->matchesHeuristicsForGpuRasterizationForTesting());

    registerMockedHttpURLLoad("viewport/viewport-gpu-rasterization-expanded-heuristics.html");
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-gpu-rasterization-expanded-heuristics.html", true, 0, 0, setViewportSettings);
    webViewHelper.webView()->resize(WebSize(640, 480));
    EXPECT_TRUE(webViewHelper.webViewImpl()->matchesHeuristicsForGpuRasterizationForTesting());

    registerMockedHttpURLLoad("viewport/viewport-inferred-values-do-not-trigger-gpu-rasterization.html");
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-inferred-values-do-not-trigger-gpu-rasterization.html", true, 0, 0, setViewportSettings);
    webViewHelper.webView()->resize(WebSize(640, 480));
    EXPECT_FALSE(webViewHelper.webViewImpl()->matchesHeuristicsForGpuRasterizationForTesting());

    registerMockedHttpURLLoad("viewport/viewport-1.html");
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-1.html", true, 0, 0, setViewportSettings);
    webViewHelper.webView()->resize(WebSize(640, 480));
    EXPECT_FALSE(webViewHelper.webViewImpl()->matchesHeuristicsForGpuRasterizationForTesting());

    registerMockedHttpURLLoad("viewport/viewport-15.html");
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-15.html", true, 0, 0, setViewportSettings);
    webViewHelper.webView()->resize(WebSize(640, 480));
    EXPECT_FALSE(webViewHelper.webViewImpl()->matchesHeuristicsForGpuRasterizationForTesting());

    registerMockedHttpURLLoad("viewport/viewport-130.html");
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-130.html", true, 0, 0, setViewportSettings);
    webViewHelper.webView()->resize(WebSize(640, 480));
    EXPECT_FALSE(webViewHelper.webViewImpl()->matchesHeuristicsForGpuRasterizationForTesting());
}

class ConsoleMessageWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    virtual void didAddMessageToConsole(const WebConsoleMessage& msg, const WebString& sourceName, unsigned sourceLine, const WebString& stackTrace)
    {
        messages.push_back(msg);
    }

    std::vector<WebConsoleMessage> messages;
};

TEST_F(ViewportTest, viewportWarnings1)
{
    ConsoleMessageWebFrameClient webFrameClient;

    registerMockedHttpURLLoad("viewport/viewport-warnings-1.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-warnings-1.html", true, &webFrameClient, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_TRUE(webFrameClient.messages.empty());

    EXPECT_EQ(320, constraints.layoutSize.width());
    EXPECT_EQ(352, constraints.layoutSize.height());
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(2.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportWarnings2)
{
    ConsoleMessageWebFrameClient webFrameClient;

    registerMockedHttpURLLoad("viewport/viewport-warnings-2.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-warnings-2.html", true, &webFrameClient, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1U, webFrameClient.messages.size());
    EXPECT_EQ(WebConsoleMessage::LevelWarning, webFrameClient.messages[0].level);
    EXPECT_STREQ("The key \"wwidth\" is not recognized and ignored.", webFrameClient.messages[0].text.utf8().c_str());

    EXPECT_EQ(980, constraints.layoutSize.width());
    EXPECT_EQ(1078, constraints.layoutSize.height());
    EXPECT_NEAR(0.33f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(0.33f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportWarnings3)
{
    ConsoleMessageWebFrameClient webFrameClient;

    registerMockedHttpURLLoad("viewport/viewport-warnings-3.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-warnings-3.html", true, &webFrameClient, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1U, webFrameClient.messages.size());
    EXPECT_EQ(WebConsoleMessage::LevelWarning, webFrameClient.messages[0].level);
    EXPECT_STREQ("The value \"unrecognized-width\" for key \"width\" is invalid, and has been ignored.",
        webFrameClient.messages[0].text.utf8().c_str());

    EXPECT_NEAR(64.0f, constraints.layoutSize.width(), 0.01);
    EXPECT_NEAR(70.4f, constraints.layoutSize.height(), 0.01);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportWarnings4)
{
    ConsoleMessageWebFrameClient webFrameClient;

    registerMockedHttpURLLoad("viewport/viewport-warnings-4.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-warnings-4.html", true, &webFrameClient, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1U, webFrameClient.messages.size());
    EXPECT_EQ(WebConsoleMessage::LevelWarning, webFrameClient.messages[0].level);
    EXPECT_STREQ("The value \"123x456\" for key \"width\" was truncated to its numeric prefix.",
        webFrameClient.messages[0].text.utf8().c_str());

    EXPECT_NEAR(123.0f, constraints.layoutSize.width(), 0.01);
    EXPECT_NEAR(135.3f, constraints.layoutSize.height(), 0.01);
    EXPECT_NEAR(2.60f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(2.60f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportWarnings5)
{
    ConsoleMessageWebFrameClient webFrameClient;

    registerMockedHttpURLLoad("viewport/viewport-warnings-5.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-warnings-5.html", true, &webFrameClient, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(5U, webFrameClient.messages.size());

    EXPECT_EQ(WebConsoleMessage::LevelWarning, webFrameClient.messages[0].level);
    EXPECT_STREQ("The value \"device-width;\" for key \"width\" is invalid, and has been ignored.",
        webFrameClient.messages[0].text.utf8().c_str());

    EXPECT_EQ(WebConsoleMessage::LevelWarning, webFrameClient.messages[1].level);
    EXPECT_STREQ("The value \"1.0;\" for key \"initial-scale\" was truncated to its numeric prefix.",
        webFrameClient.messages[1].text.utf8().c_str());

    EXPECT_EQ(WebConsoleMessage::LevelWarning, webFrameClient.messages[2].level);
    EXPECT_STREQ("The value \"1.0;\" for key \"maximum-scale\" was truncated to its numeric prefix.",
        webFrameClient.messages[2].text.utf8().c_str());

    EXPECT_EQ(WebConsoleMessage::LevelWarning, webFrameClient.messages[3].level);
    EXPECT_STREQ("The value \"0;\" for key \"user-scalable\" was truncated to its numeric prefix.",
        webFrameClient.messages[3].text.utf8().c_str());

    EXPECT_EQ(WebConsoleMessage::LevelWarning, webFrameClient.messages[4].level);
    EXPECT_STREQ("Error parsing a meta element's content: ';' is not a valid key-value pair separator. Please use ',' instead.",
        webFrameClient.messages[4].text.utf8().c_str());

    EXPECT_NEAR(320.0f, constraints.layoutSize.width(), 0.01);
    EXPECT_NEAR(352.0f, constraints.layoutSize.height(), 0.01);
    EXPECT_NEAR(1.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(1.0f, constraints.maximumScale, 0.01f);
    EXPECT_FALSE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportWarnings6)
{
    ConsoleMessageWebFrameClient webFrameClient;

    registerMockedHttpURLLoad("viewport/viewport-warnings-6.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-warnings-6.html", true, &webFrameClient, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    PageScaleConstraints constraints = runViewportTest(page, 320, 352);

    EXPECT_EQ(1U, webFrameClient.messages.size());
    EXPECT_EQ(WebConsoleMessage::LevelWarning, webFrameClient.messages[0].level);
    EXPECT_STREQ("The value \"\" for key \"width\" is invalid, and has been ignored.",
        webFrameClient.messages[0].text.utf8().c_str());

    EXPECT_NEAR(64.0f, constraints.layoutSize.width(), 0.01);
    EXPECT_NEAR(70.4f, constraints.layoutSize.height(), 0.01);
    EXPECT_NEAR(5.0f, constraints.initialScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.minimumScale, 0.01f);
    EXPECT_NEAR(5.0f, constraints.maximumScale, 0.01f);
    EXPECT_TRUE(page->viewportDescription().userZoom);
}

TEST_F(ViewportTest, viewportWarnings7)
{
    ConsoleMessageWebFrameClient webFrameClient;

    registerMockedHttpURLLoad("viewport/viewport-warnings-7.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "viewport/viewport-warnings-7.html", true, &webFrameClient, 0, setViewportSettings);

    Page* page = webViewHelper.webViewImpl()->page();
    runViewportTest(page, 320, 352);

    EXPECT_EQ(0U, webFrameClient.messages.size());
}

} // namespace blink
