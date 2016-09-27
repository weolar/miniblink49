/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef FrameTestHelpers_h
#define FrameTestHelpers_h

#include "core/frame/Settings.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "public/platform/WebURLRequest.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebHistoryItem.h"
#include "public/web/WebRemoteFrameClient.h"
#include "public/web/WebViewClient.h"
#include "web/WebViewImpl.h"
#include "wtf/PassOwnPtr.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

namespace blink {

namespace FrameTestHelpers {

class TestWebFrameClient;

// Loads a url into the specified WebFrame for testing purposes. Pumps any
// pending resource requests, as well as waiting for the threaded parser to
// finish, before returning.
void loadFrame(WebFrame*, const std::string& url);
// Same as above, but for WebFrame::loadHTMLString().
void loadHTMLString(WebFrame*, const std::string& html, const WebURL& baseURL);
// Same as above, but for WebFrame::loadHistoryItem().
void loadHistoryItem(WebFrame*, const WebHistoryItem&, WebHistoryLoadType, WebURLRequest::CachePolicy);
// Same as above, but for WebFrame::reload().
void reloadFrame(WebFrame*);
void reloadFrameIgnoringCache(WebFrame*);

// Pumps pending resource requests while waiting for a frame to load. Don't use
// this. Use one of the above helpers.
void pumpPendingRequestsDoNotUse(WebFrame*);

class SettingOverrider {
public:
    virtual void overrideSettings(WebSettings*) = 0;
};

// Convenience class for handling the lifetime of a WebView and its associated mainframe in tests.
class WebViewHelper {
    WTF_MAKE_NONCOPYABLE(WebViewHelper);
public:
    WebViewHelper(SettingOverrider* = 0);
    ~WebViewHelper();

    // Creates and initializes the WebView. Implicitly calls reset() first. IF a
    // WebFrameClient or a WebViewClient are passed in, they must outlive the
    // WebViewHelper.
    WebViewImpl* initialize(bool enableJavascript = false, TestWebFrameClient* = 0, WebViewClient* = 0, void (*updateSettingsFunc)(WebSettings*) = 0);

    // Same as initialize() but also performs the initial load of the url. Only
    // returns once the load is complete.
    WebViewImpl* initializeAndLoad(const std::string& url, bool enableJavascript = false, TestWebFrameClient* = 0, WebViewClient* = 0, void (*updateSettingsFunc)(WebSettings*) = 0);

    void reset();

    WebView* webView() const { return m_webView; }
    WebViewImpl* webViewImpl() const { return m_webView; }

private:
    WebViewImpl* m_webView;
    SettingOverrider* m_settingOverrider;
};

// Minimal implementation of WebFrameClient needed for unit tests that load frames. Tests that load
// frames and need further specialization of WebFrameClient behavior should subclass this.
class TestWebFrameClient : public WebFrameClient {
public:
    TestWebFrameClient();

    WebFrame* createChildFrame(WebLocalFrame* parent, WebTreeScopeType, const WebString& frameName, WebSandboxFlags) override;
    void frameDetached(WebFrame*, DetachType) override;
    void didStartLoading(bool) override;
    void didStopLoading() override;

    bool isLoading() { return m_loadsInProgress > 0; }
    void waitForLoadToComplete();

private:
    int m_loadsInProgress;
};

// Minimal implementation of WebRemoteFrameClient needed for unit tests that load remote frames. Tests that load
// frames and need further specialization of WebFrameClient behavior should subclass this.
class TestWebRemoteFrameClient : public WebRemoteFrameClient {
public:
    TestWebRemoteFrameClient();

    WebRemoteFrame* frame() const { return m_frame; }

    // WebRemoteFrameClient overrides:
    void frameDetached(DetachType) override;
    void postMessageEvent(
        WebLocalFrame* sourceFrame,
        WebRemoteFrame* targetFrame,
        WebSecurityOrigin targetOrigin,
        WebDOMMessageEvent) override { }

private:
    WebRemoteFrame* const m_frame;
};

class TestWebViewClient : public WebViewClient {
public:
    virtual ~TestWebViewClient() { }
    void initializeLayerTreeView() override;
    WebLayerTreeView* layerTreeView() override { return m_layerTreeView.get(); }

private:
    OwnPtr<WebLayerTreeView> m_layerTreeView;
};

class UseMockScrollbarSettings {
public:
    UseMockScrollbarSettings()
    {
        Settings::setMockScrollbarsEnabled(true);
        RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(true);
        EXPECT_TRUE(ScrollbarTheme::theme()->usesOverlayScrollbars());
    }

    ~UseMockScrollbarSettings()
    {
        Settings::setMockScrollbarsEnabled(false);
        RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(false);
    }
};

} // namespace FrameTestHelpers
} // namespace blink

#endif // FrameTestHelpers_h
