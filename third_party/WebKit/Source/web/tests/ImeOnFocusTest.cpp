// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/Node.h"
#include "core/html/HTMLElement.h"
#include "platform/testing/URLTestHelpers.h"
#include "platform/testing/UnitTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebDocument.h"
#include "web/WebLocalFrameImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

using blink::FrameTestHelpers::loadFrame;
using blink::testing::runPendingTasks;
using blink::URLTestHelpers::registerMockedURLFromBaseURL;

namespace blink {

class ImeRequestTrackingWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    ImeRequestTrackingWebViewClient() :
        m_imeRequestCount(0)
    {
    }

    // WebWidgetClient methods
    void showImeIfNeeded() override
    {
        ++m_imeRequestCount;
    }

    // Local methds
    void reset()
    {
        m_imeRequestCount = 0;
    }

    int imeRequestCount()
    {
        return m_imeRequestCount;
    }

private:
    int m_imeRequestCount;
};

class ImeOnFocusTest : public ::testing::Test {
public:
    ImeOnFocusTest()
        : m_baseURL("http://www.test.com/")
    {
    }

    void TearDown() override
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

protected:
    void sendGestureTap(WebView*, IntPoint);
    void focus(const AtomicString& element);
    void runImeOnFocusTest(std::string fileName, int, IntPoint tapPoint = IntPoint(-1, -1), const AtomicString& focusElement = nullAtom, std::string frame = "");

    std::string m_baseURL;
    FrameTestHelpers::WebViewHelper m_webViewHelper;
    RefPtrWillBePersistent<Document> m_document;
};

void ImeOnFocusTest::sendGestureTap(WebView* webView, IntPoint clientPoint)
{
    WebGestureEvent webGestureEvent;
    webGestureEvent.type = WebInputEvent::GestureTap;
    webGestureEvent.x = clientPoint.x();
    webGestureEvent.y = clientPoint.y();
    webGestureEvent.globalX = clientPoint.x();
    webGestureEvent.globalY = clientPoint.y();
    webGestureEvent.data.tap.tapCount = 1;
    webGestureEvent.data.tap.width = 10;
    webGestureEvent.data.tap.height = 10;

    webView->handleInputEvent(webGestureEvent);
    runPendingTasks();
}

void ImeOnFocusTest::focus(const AtomicString& element)
{
    m_document->body()->getElementById(element)->focus();
}

void ImeOnFocusTest::runImeOnFocusTest(std::string fileName, int expectedImeRequestCount, IntPoint tapPoint, const AtomicString& focusElement, std::string frame)
{
    ImeRequestTrackingWebViewClient client;
    registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL), WebString::fromUTF8(fileName));
    WebViewImpl* webView = m_webViewHelper.initialize(true, 0, &client);
    webView->resize(WebSize(800, 1200));
    loadFrame(webView->mainFrame(), m_baseURL + fileName);
    m_document = m_webViewHelper.webViewImpl()->mainFrameImpl()->document().unwrap<Document>();

    if (!focusElement.isNull())
        focus(focusElement);
    EXPECT_EQ(0, client.imeRequestCount());

    if (tapPoint.x() >= 0 && tapPoint.y() >= 0)
        sendGestureTap(webView, tapPoint);

    if (!frame.empty()) {
        registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL), WebString::fromUTF8(frame));
        WebFrame* childFrame = webView->mainFrame()->firstChild();
        loadFrame(childFrame, m_baseURL + frame);
    }

    if (!focusElement.isNull())
        focus(focusElement);
    EXPECT_EQ(expectedImeRequestCount, client.imeRequestCount());

    m_webViewHelper.reset();
}

TEST_F(ImeOnFocusTest, OnLoad)
{
    runImeOnFocusTest("ime-on-focus-on-load.html", 0);
}

TEST_F(ImeOnFocusTest, OnAutofocus)
{
    runImeOnFocusTest("ime-on-focus-on-autofocus.html", 0);
}

TEST_F(ImeOnFocusTest, OnUserGesture)
{
    runImeOnFocusTest("ime-on-focus-on-user-gesture.html", 1, IntPoint(50, 50));
}

TEST_F(ImeOnFocusTest, AfterFirstGesture)
{
    runImeOnFocusTest("ime-on-focus-after-first-gesture.html", 1, IntPoint(50, 50), "input");
}

TEST_F(ImeOnFocusTest, AfterNavigationWithinPage)
{
    runImeOnFocusTest("ime-on-focus-after-navigation-within-page.html", 1, IntPoint(50, 50), "input");
}

TEST_F(ImeOnFocusTest, AfterFrameLoadOnGesture)
{
    runImeOnFocusTest("ime-on-focus-after-frame-load-on-gesture.html", 1, IntPoint(50, 50), "input", "frame.html");
}

} // namespace blink
