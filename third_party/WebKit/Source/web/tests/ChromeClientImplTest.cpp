/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#include "core/page/Page.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebInputEvent.h"
#include "public/web/WebLocalFrame.h"
#include "public/web/WebView.h"
#include "public/web/WebViewClient.h"
#include "web/WebViewImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

void setCurrentInputEventForTest(const WebInputEvent* event)
{
    WebViewImpl::m_currentInputEvent = event;
}

namespace {

class TestWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    explicit TestWebViewClient(WebNavigationPolicy* target) : m_target(target) { }
    ~TestWebViewClient() override { }

    void show(WebNavigationPolicy policy) override
    {
        *m_target = policy;
    }

private:
    WebNavigationPolicy* m_target;
};

class TestWebFrameClient : public WebFrameClient {
public:
    ~TestWebFrameClient() override { }
};

} // anonymous namespace

class GetNavigationPolicyTest : public testing::Test {
public:
    GetNavigationPolicyTest()
        : m_result(WebNavigationPolicyIgnore)
        , m_webViewClient(&m_result)
    {
    }

protected:
    void SetUp() override
    {
        m_webView = toWebViewImpl(WebView::create(&m_webViewClient));
        m_mainFrame = WebLocalFrame::create(WebTreeScopeType::Document, &m_webFrameClient);
        m_webView->setMainFrame(m_mainFrame);
        m_chromeClientImpl = toChromeClientImpl(&m_webView->page()->chromeClient());
        m_result = WebNavigationPolicyIgnore;
    }

    void TearDown() override
    {
        m_webView->close();
        m_mainFrame->close();
    }

    WebNavigationPolicy getNavigationPolicyWithMouseEvent(int modifiers, WebMouseEvent::Button button, bool asPopup)
    {
        WebMouseEvent event;
        event.modifiers = modifiers;
        event.type = WebInputEvent::MouseUp;
        event.button = button;
        setCurrentInputEventForTest(&event);
        m_chromeClientImpl->setScrollbarsVisible(!asPopup);
        m_chromeClientImpl->show(NavigationPolicyIgnore);
        setCurrentInputEventForTest(0);
        return m_result;
    }

    bool isNavigationPolicyPopup()
    {
        m_chromeClientImpl->show(NavigationPolicyIgnore);
        return m_result == WebNavigationPolicyNewPopup;
    }

protected:
    WebNavigationPolicy m_result;
    TestWebViewClient m_webViewClient;
    WebViewImpl* m_webView;
    WebFrame* m_mainFrame;
    TestWebFrameClient m_webFrameClient;
    ChromeClientImpl* m_chromeClientImpl;
};

TEST_F(GetNavigationPolicyTest, LeftClick)
{
    int modifiers = 0;
    WebMouseEvent::Button button = WebMouseEvent::ButtonLeft;
    bool asPopup = false;
    EXPECT_EQ(WebNavigationPolicyNewForegroundTab,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, LeftClickPopup)
{
    int modifiers = 0;
    WebMouseEvent::Button button = WebMouseEvent::ButtonLeft;
    bool asPopup = true;
    EXPECT_EQ(WebNavigationPolicyNewPopup,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, ShiftLeftClick)
{
    int modifiers = WebInputEvent::ShiftKey;
    WebMouseEvent::Button button = WebMouseEvent::ButtonLeft;
    bool asPopup = false;
    EXPECT_EQ(WebNavigationPolicyNewWindow,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, ShiftLeftClickPopup)
{
    int modifiers = WebInputEvent::ShiftKey;
    WebMouseEvent::Button button = WebMouseEvent::ButtonLeft;
    bool asPopup = true;
    EXPECT_EQ(WebNavigationPolicyNewPopup,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, ControlOrMetaLeftClick)
{
#if OS(MACOSX)
    int modifiers = WebInputEvent::MetaKey;
#else
    int modifiers = WebInputEvent::ControlKey;
#endif
    WebMouseEvent::Button button = WebMouseEvent::ButtonLeft;
    bool asPopup = false;
    EXPECT_EQ(WebNavigationPolicyNewBackgroundTab,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, ControlOrMetaLeftClickPopup)
{
#if OS(MACOSX)
    int modifiers = WebInputEvent::MetaKey;
#else
    int modifiers = WebInputEvent::ControlKey;
#endif
    WebMouseEvent::Button button = WebMouseEvent::ButtonLeft;
    bool asPopup = true;
    EXPECT_EQ(WebNavigationPolicyNewBackgroundTab,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, ControlOrMetaAndShiftLeftClick)
{
#if OS(MACOSX)
    int modifiers = WebInputEvent::MetaKey;
#else
    int modifiers = WebInputEvent::ControlKey;
#endif
    modifiers |= WebInputEvent::ShiftKey;
    WebMouseEvent::Button button = WebMouseEvent::ButtonLeft;
    bool asPopup = false;
    EXPECT_EQ(WebNavigationPolicyNewForegroundTab,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, ControlOrMetaAndShiftLeftClickPopup)
{
#if OS(MACOSX)
    int modifiers = WebInputEvent::MetaKey;
#else
    int modifiers = WebInputEvent::ControlKey;
#endif
    modifiers |= WebInputEvent::ShiftKey;
    WebMouseEvent::Button button = WebMouseEvent::ButtonLeft;
    bool asPopup = true;
    EXPECT_EQ(WebNavigationPolicyNewForegroundTab,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, MiddleClick)
{
    int modifiers = 0;
    bool asPopup = false;
    WebMouseEvent::Button button = WebMouseEvent::ButtonMiddle;
    EXPECT_EQ(WebNavigationPolicyNewBackgroundTab,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, MiddleClickPopup)
{
    int modifiers = 0;
    bool asPopup = true;
    WebMouseEvent::Button button = WebMouseEvent::ButtonMiddle;
    EXPECT_EQ(WebNavigationPolicyNewBackgroundTab,
        getNavigationPolicyWithMouseEvent(modifiers, button, asPopup));
}

TEST_F(GetNavigationPolicyTest, NoToolbarsForcesPopup)
{
    m_chromeClientImpl->setToolbarsVisible(false);
    EXPECT_TRUE(isNavigationPolicyPopup());
    m_chromeClientImpl->setToolbarsVisible(true);
    EXPECT_FALSE(isNavigationPolicyPopup());
}

TEST_F(GetNavigationPolicyTest, NoStatusbarForcesPopup)
{
    m_chromeClientImpl->setStatusbarVisible(false);
    EXPECT_TRUE(isNavigationPolicyPopup());
    m_chromeClientImpl->setStatusbarVisible(true);
    EXPECT_FALSE(isNavigationPolicyPopup());
}

TEST_F(GetNavigationPolicyTest, NoMenubarForcesPopup)
{
    m_chromeClientImpl->setMenubarVisible(false);
    EXPECT_TRUE(isNavigationPolicyPopup());
    m_chromeClientImpl->setMenubarVisible(true);
    EXPECT_FALSE(isNavigationPolicyPopup());
}

TEST_F(GetNavigationPolicyTest, NotResizableForcesPopup)
{
    m_chromeClientImpl->setResizable(false);
    EXPECT_TRUE(isNavigationPolicyPopup());
    m_chromeClientImpl->setResizable(true);
    EXPECT_FALSE(isNavigationPolicyPopup());
}

} // namespace blink
