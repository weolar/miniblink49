/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
#include "public/web/WebFrame.h"

#include "SkBitmap.h"
#include "SkCanvas.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "bindings/core/v8/V8Node.h"
#include "core/clipboard/DataTransfer.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/css/resolver/ViewportStyleResolver.h"
#include "core/dom/DocumentMarkerController.h"
#include "core/dom/Fullscreen.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/Range.h"
#include "core/editing/Editor.h"
#include "core/editing/EphemeralRange.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/SpellChecker.h"
#include "core/editing/VisiblePosition.h"
#include "core/events/MouseEvent.h"
#include "core/fetch/FetchRequest.h"
#include "core/fetch/MemoryCache.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/RemoteFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLMediaElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutFullScreen.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/loader/DocumentThreadableLoader.h"
#include "core/loader/DocumentThreadableLoaderClient.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/loader/ThreadableLoader.h"
#include "core/page/Page.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/testing/NullExecutionContext.h"
#include "modules/mediastream/MediaStream.h"
#include "modules/mediastream/MediaStreamRegistry.h"
#include "platform/DragImage.h"
#include "platform/PlatformResourceLoader.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/UserGestureIndicator.h"
#include "platform/geometry/FloatRect.h"
#include "platform/network/ResourceError.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "platform/testing/URLTestHelpers.h"
#include "platform/testing/UnitTestHelpers.h"
#include "platform/weborigin/SchemeRegistry.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "public/platform/WebFloatRect.h"
#include "public/platform/WebSecurityOrigin.h"
#include "public/platform/WebThread.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebURLResponse.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebCache.h"
#include "public/web/WebConsoleMessage.h"
#include "public/web/WebDataSource.h"
#include "public/web/WebDocument.h"
#include "public/web/WebFindOptions.h"
#include "public/web/WebFormElement.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebHistoryItem.h"
#include "public/web/WebPrintParams.h"
#include "public/web/WebRange.h"
#include "public/web/WebRemoteFrame.h"
#include "public/web/WebScriptExecutionCallback.h"
#include "public/web/WebScriptSource.h"
#include "public/web/WebSearchableFormData.h"
#include "public/web/WebSecurityPolicy.h"
#include "public/web/WebSelection.h"
#include "public/web/WebSettings.h"
#include "public/web/WebSpellCheckClient.h"
#include "public/web/WebTextCheckingCompletion.h"
#include "public/web/WebTextCheckingResult.h"
#include "public/web/WebViewClient.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebRemoteFrameImpl.h"
#include "web/WebViewImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include "wtf/Forward.h"
#include "wtf/dtoa/utils.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <map>
#include <stdarg.h>
#include <v8.h>

using blink::URLTestHelpers::toKURL;
using blink::FrameTestHelpers::UseMockScrollbarSettings;
using blink::testing::runPendingTasks;
using testing::ElementsAre;
using testing::Mock;
using testing::_;

namespace blink {

::std::ostream& operator<<(::std::ostream& os, const WebFloatSize& size)
{
    return os << "WebFloatSize: ["
        << size.width<< ", " << size.height<< "]";
}

::std::ostream& operator<<(::std::ostream& os, const WebFloatPoint& point)
{
    return os << "WebFloatPoint: ["
        << point.x<< ", " << point.y<< "]";
}

const int touchPointPadding = 32;

#define EXPECT_RECT_EQ(expected, actual) \
    do { \
        EXPECT_EQ(expected.x(), actual.x()); \
        EXPECT_EQ(expected.y(), actual.y()); \
        EXPECT_EQ(expected.width(), actual.width()); \
        EXPECT_EQ(expected.height(), actual.height()); \
    } while (false)

#define EXPECT_POINT_EQ(expected, actual) \
    do { \
        EXPECT_EQ(expected.x(), actual.x()); \
        EXPECT_EQ(expected.y(), actual.y()); \
    } while (false)

#define EXPECT_FLOAT_POINT_EQ(expected, actual) \
    do { \
        EXPECT_FLOAT_EQ(expected.x(), actual.x()); \
        EXPECT_FLOAT_EQ(expected.y(), actual.y()); \
    } while (false)

class WebFrameTest : public ::testing::Test {
protected:
    WebFrameTest()
        : m_baseURL("http://internal.test/")
        , m_notBaseURL("http://external.test/")
        , m_chromeURL("chrome://")
    {
    }

    ~WebFrameTest() override
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


    void registerMockedHttpURLLoadWithCSP(const std::string& fileName, const std::string& csp, bool reportOnly = false)
    {
        WebURLResponse response;
        response.initialize();
        response.setMIMEType("text/html");
        response.addHTTPHeaderField(reportOnly ? WebString("Content-Security-Policy-Report-Only") : WebString("Content-Security-Policy"), WebString::fromUTF8(csp));
        std::string fullString = m_baseURL + fileName;
        URLTestHelpers::registerMockedURLLoadWithCustomResponse(toKURL(fullString.c_str()), WebString::fromUTF8(fileName.c_str()), WebString::fromUTF8(""), response);
    }

    void applyViewportStyleOverride(FrameTestHelpers::WebViewHelper* webViewHelper)
    {
        RefPtrWillBeRawPtr<StyleSheetContents> styleSheet = StyleSheetContents::create(CSSParserContext(UASheetMode, 0));
        styleSheet->parseString(loadResourceAsASCIIString("viewportAndroid.css"));
        OwnPtrWillBeRawPtr<RuleSet> ruleSet = RuleSet::create();
        ruleSet->addRulesFromSheet(styleSheet.get(), MediaQueryEvaluator("screen"));

        Document* document = toLocalFrame(webViewHelper->webViewImpl()->page()->mainFrame())->document();
        document->ensureStyleResolver().viewportStyleResolver()->collectViewportRules(ruleSet.get(), ViewportStyleResolver::UserAgentOrigin);
        document->ensureStyleResolver().viewportStyleResolver()->resolve();
    }

    static void configueCompositingWebView(WebSettings* settings)
    {
        settings->setAcceleratedCompositingEnabled(true);
        settings->setPreferCompositingToLCDTextEnabled(true);
    }

    static void configureAndroid(WebSettings* settings)
    {
        settings->setViewportMetaEnabled(true);
        settings->setViewportEnabled(true);
        settings->setMainFrameResizesAreOrientationChanges(true);
        settings->setShrinksViewportContentToFit(true);
    }

    static void configureLoadsImagesAutomatically(WebSettings* settings)
    {
        settings->setLoadsImagesAutomatically(true);
    }

    void initializeTextSelectionWebView(const std::string& url, FrameTestHelpers::WebViewHelper* webViewHelper)
    {
        webViewHelper->initializeAndLoad(url, true);
        webViewHelper->webView()->settings()->setDefaultFontSize(12);
        webViewHelper->webView()->resize(WebSize(640, 480));
    }

    PassOwnPtr<DragImage> nodeImageTestSetup(FrameTestHelpers::WebViewHelper* webViewHelper, const std::string& testcase)
    {
        registerMockedHttpURLLoad("nodeimage.html");
        webViewHelper->initializeAndLoad(m_baseURL + "nodeimage.html");
        webViewHelper->webView()->resize(WebSize(640, 480));
        webViewHelper->webView()->layout();
        RefPtrWillBeRawPtr<LocalFrame> frame = toLocalFrame(webViewHelper->webViewImpl()->page()->mainFrame());
        ASSERT(frame);
        Element* element = frame->document()->getElementById(testcase.c_str());
        return frame->nodeImage(*element);
    }

    std::string m_baseURL;
    std::string m_notBaseURL;
    std::string m_chromeURL;
};

enum ParameterizedWebFrameTestConfig {
    Default,
    RootLayerScrolls
};

class ParameterizedWebFrameTest
    : public WebFrameTest
    , public ::testing::WithParamInterface<ParameterizedWebFrameTestConfig>
    , public FrameTestHelpers::SettingOverrider {
public:

    void overrideSettings(WebSettings* settings)
    {
        switch (GetParam()) {
        case Default:
            break;
        case RootLayerScrolls:
            settings->setRootLayerScrolls(true);
            break;
        }
    }
};

// Friendly string for gtest failure messages.
void PrintTo(ParameterizedWebFrameTestConfig config, ::std::ostream* os)
{
    switch (config) {
    case Default:
        *os << "Default";
        break;
    case RootLayerScrolls:
        *os << "RootLayerScrolls";
        break;
    }
}

INSTANTIATE_TEST_CASE_P(All, ParameterizedWebFrameTest, ::testing::Values(
    ParameterizedWebFrameTestConfig::Default,
    ParameterizedWebFrameTestConfig::RootLayerScrolls));

TEST_P(ParameterizedWebFrameTest, ContentText)
{
    registerMockedHttpURLLoad("iframes_test.html");
    registerMockedHttpURLLoad("visible_iframe.html");
    registerMockedHttpURLLoad("invisible_iframe.html");
    registerMockedHttpURLLoad("zero_sized_iframe.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "iframes_test.html");

    // Now retrieve the frames text and test it only includes visible elements.
    std::string content = webViewHelper.webView()->mainFrame()->contentAsText(1024).utf8();
    EXPECT_NE(std::string::npos, content.find(" visible paragraph"));
    EXPECT_NE(std::string::npos, content.find(" visible iframe"));
    EXPECT_EQ(std::string::npos, content.find(" invisible pararaph"));
    EXPECT_EQ(std::string::npos, content.find(" invisible iframe"));
    EXPECT_EQ(std::string::npos, content.find("iframe with zero size"));
}

TEST_P(ParameterizedWebFrameTest, FrameForEnteredContext)
{
    registerMockedHttpURLLoad("iframes_test.html");
    registerMockedHttpURLLoad("visible_iframe.html");
    registerMockedHttpURLLoad("invisible_iframe.html");
    registerMockedHttpURLLoad("zero_sized_iframe.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "iframes_test.html", true);

    v8::HandleScope scope(v8::Isolate::GetCurrent());
    EXPECT_EQ(webViewHelper.webView()->mainFrame(), WebLocalFrame::frameForContext(webViewHelper.webView()->mainFrame()->mainWorldScriptContext()));
    EXPECT_EQ(webViewHelper.webView()->mainFrame()->firstChild(), WebLocalFrame::frameForContext(webViewHelper.webView()->mainFrame()->firstChild()->mainWorldScriptContext()));
}

class ScriptExecutionCallbackHelper : public WebScriptExecutionCallback {
public:
    explicit ScriptExecutionCallbackHelper(v8::Local<v8::Context> context)
        : m_didComplete(false)
        , m_context(context) { }
    ~ScriptExecutionCallbackHelper() { }

    bool didComplete() const { return m_didComplete; }
    const String& stringValue() const { return m_stringValue; }

private:
    void completed(const WebVector<v8::Local<v8::Value>>& values) override
    {
        m_didComplete = true;
        if (!values.isEmpty() && values[0]->IsString()) {
            m_stringValue = toCoreString(values[0]->ToString(m_context).ToLocalChecked());
        }
    }

    bool m_didComplete;
    String m_stringValue;
    v8::Local<v8::Context> m_context;
};

TEST_P(ParameterizedWebFrameTest, RequestExecuteScript)
{
    registerMockedHttpURLLoad("foo.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "foo.html", true);

    v8::HandleScope scope(v8::Isolate::GetCurrent());
    ScriptExecutionCallbackHelper callbackHelper(webViewHelper.webView()->mainFrame()->mainWorldScriptContext());
    webViewHelper.webView()->mainFrame()->toWebLocalFrame()->requestExecuteScriptAndReturnValue(WebScriptSource(WebString("'hello';")), false, &callbackHelper);
    runPendingTasks();
    EXPECT_TRUE(callbackHelper.didComplete());
    EXPECT_EQ("hello", callbackHelper.stringValue());
}

TEST_P(ParameterizedWebFrameTest, SuspendedRequestExecuteScript)
{
    registerMockedHttpURLLoad("foo.html");
    registerMockedHttpURLLoad("bar.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "foo.html", true);

    v8::HandleScope scope(v8::Isolate::GetCurrent());
    ScriptExecutionCallbackHelper callbackHelper(webViewHelper.webView()->mainFrame()->mainWorldScriptContext());

    // Suspend scheduled tasks so the script doesn't run.
    toWebLocalFrameImpl(webViewHelper.webView()->mainFrame())->frame()->document()->suspendScheduledTasks();
    webViewHelper.webView()->mainFrame()->toWebLocalFrame()->requestExecuteScriptAndReturnValue(WebScriptSource(WebString("'hello';")), false, &callbackHelper);
    runPendingTasks();
    EXPECT_FALSE(callbackHelper.didComplete());

    // If the frame navigates, pending scripts should be removed, but the callback should always be ran.
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "bar.html");
    EXPECT_TRUE(callbackHelper.didComplete());
    EXPECT_EQ(String(), callbackHelper.stringValue());
}

TEST_P(ParameterizedWebFrameTest, IframeScriptRemovesSelf)
{
    registerMockedHttpURLLoad("single_iframe.html");
    registerMockedHttpURLLoad("visible_iframe.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "single_iframe.html", true);

    v8::HandleScope scope(v8::Isolate::GetCurrent());
    ScriptExecutionCallbackHelper callbackHelper(webViewHelper.webView()->mainFrame()->mainWorldScriptContext());
    webViewHelper.webView()->mainFrame()->firstChild()->toWebLocalFrame()->requestExecuteScriptAndReturnValue(WebScriptSource(WebString("var iframe = window.top.document.getElementsByTagName('iframe')[0]; window.top.document.body.removeChild(iframe); 'hello';")), false, &callbackHelper);
    runPendingTasks();
    EXPECT_TRUE(callbackHelper.didComplete());
    EXPECT_EQ(String(), callbackHelper.stringValue());
}

TEST_P(ParameterizedWebFrameTest, FormWithNullFrame)
{
    registerMockedHttpURLLoad("form.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "form.html");

    WebVector<WebFormElement> forms;
    webViewHelper.webView()->mainFrame()->document().forms(forms);
    webViewHelper.reset();

    EXPECT_EQ(forms.size(), 1U);

    // This test passes if this doesn't crash.
    WebSearchableFormData searchableDataForm(forms[0]);
}

TEST_P(ParameterizedWebFrameTest, ChromePageJavascript)
{
    registerMockedChromeURLLoad("history.html");

    // Pass true to enable JavaScript.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_chromeURL + "history.html", true);

    // Try to run JS against the chrome-style URL.
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), "javascript:document.body.appendChild(document.createTextNode('Clobbered'))");

    // Required to see any updates in contentAsText.
    webViewHelper.webView()->layout();

    // Now retrieve the frame's text and ensure it was modified by running javascript.
    std::string content = webViewHelper.webView()->mainFrame()->contentAsText(1024).utf8();
    EXPECT_NE(std::string::npos, content.find("Clobbered"));
}

TEST_P(ParameterizedWebFrameTest, ChromePageNoJavascript)
{
    registerMockedChromeURLLoad("history.html");

    /// Pass true to enable JavaScript.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_chromeURL + "history.html", true);

    // Try to run JS against the chrome-style URL after prohibiting it.
    WebSecurityPolicy::registerURLSchemeAsNotAllowingJavascriptURLs("chrome");
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), "javascript:document.body.appendChild(document.createTextNode('Clobbered'))");

    // Required to see any updates in contentAsText.
    webViewHelper.webView()->layout();

    // Now retrieve the frame's text and ensure it wasn't modified by running javascript.
    std::string content = webViewHelper.webView()->mainFrame()->contentAsText(1024).utf8();
    EXPECT_EQ(std::string::npos, content.find("Clobbered"));
}

TEST_P(ParameterizedWebFrameTest, LocationSetHostWithMissingPort)
{
    std::string fileName = "print-location-href.html";
    registerMockedHttpURLLoad(fileName);
    URLTestHelpers::registerMockedURLLoad(toKURL("http://internal.test:0/" + fileName), WebString::fromUTF8(fileName));

    FrameTestHelpers::WebViewHelper webViewHelper(this);

    /// Pass true to enable JavaScript.
    webViewHelper.initializeAndLoad(m_baseURL + fileName, true);

    // Setting host to "hostname:" should be treated as "hostname:0".
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), "javascript:location.host = 'internal.test:'; void 0;");

    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), "javascript:document.body.textContent = location.href; void 0;");

    std::string content = webViewHelper.webView()->mainFrame()->contentAsText(1024).utf8();
    EXPECT_EQ("http://internal.test:0/" + fileName, content);
}

TEST_P(ParameterizedWebFrameTest, LocationSetEmptyPort)
{
    std::string fileName = "print-location-href.html";
    registerMockedHttpURLLoad(fileName);
    URLTestHelpers::registerMockedURLLoad(toKURL("http://internal.test:0/" + fileName), WebString::fromUTF8(fileName));

    FrameTestHelpers::WebViewHelper webViewHelper(this);

    /// Pass true to enable JavaScript.
    webViewHelper.initializeAndLoad(m_baseURL + fileName, true);

    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), "javascript:location.port = ''; void 0;");

    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), "javascript:document.body.textContent = location.href; void 0;");

    std::string content = webViewHelper.webView()->mainFrame()->contentAsText(1024).utf8();
    EXPECT_EQ("http://internal.test:0/" + fileName, content);
}

class EvaluateOnLoadWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    EvaluateOnLoadWebFrameClient() : m_executing(false), m_wasExecuted(false) { }

    void didClearWindowObject(WebLocalFrame* frame) override
    {
        EXPECT_FALSE(m_executing);
        m_wasExecuted = true;
        m_executing = true;
        v8::HandleScope handleScope(v8::Isolate::GetCurrent());
        frame->executeScriptAndReturnValue(WebScriptSource(WebString("window.someProperty = 42;")));
        m_executing = false;
    }

    bool m_executing;
    bool m_wasExecuted;
};

TEST_P(ParameterizedWebFrameTest, DidClearWindowObjectIsNotRecursive)
{
    EvaluateOnLoadWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true, &webFrameClient);
    EXPECT_TRUE(webFrameClient.m_wasExecuted);
}

class CSSCallbackWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    CSSCallbackWebFrameClient() : m_updateCount(0) { }
    void didMatchCSS(WebLocalFrame*, const WebVector<WebString>& newlyMatchingSelectors, const WebVector<WebString>& stoppedMatchingSelectors) override;

    std::map<WebLocalFrame*, std::set<std::string>> m_matchedSelectors;
    int m_updateCount;
};

void CSSCallbackWebFrameClient::didMatchCSS(WebLocalFrame* frame, const WebVector<WebString>& newlyMatchingSelectors, const WebVector<WebString>& stoppedMatchingSelectors)
{
    ++m_updateCount;
    std::set<std::string>& frameSelectors = m_matchedSelectors[frame];
    for (size_t i = 0; i < newlyMatchingSelectors.size(); ++i) {
        std::string selector = newlyMatchingSelectors[i].utf8();
        EXPECT_EQ(0U, frameSelectors.count(selector)) << selector;
        frameSelectors.insert(selector);
    }
    for (size_t i = 0; i < stoppedMatchingSelectors.size(); ++i) {
        std::string selector = stoppedMatchingSelectors[i].utf8();
        EXPECT_EQ(1U, frameSelectors.count(selector)) << selector;
        frameSelectors.erase(selector);
    }
}

class WebFrameCSSCallbackTest : public ::testing::Test {
protected:
    WebFrameCSSCallbackTest()
    {
        m_frame = m_helper.initializeAndLoad("about:blank", true, &m_client)->mainFrame()->toWebLocalFrame();
    }

    ~WebFrameCSSCallbackTest()
    {
        EXPECT_EQ(1U, m_client.m_matchedSelectors.size());
    }

    WebDocument doc() const
    {
        return m_frame->document();
    }

    int updateCount() const
    {
        return m_client.m_updateCount;
    }

    const std::set<std::string>& matchedSelectors()
    {
        return m_client.m_matchedSelectors[m_frame];
    }

    void loadHTML(const std::string& html)
    {
        FrameTestHelpers::loadHTMLString(m_frame, html, toKURL("about:blank"));
    }

    void executeScript(const WebString& code)
    {
        m_frame->executeScript(WebScriptSource(code));
        m_frame->view()->layout();
        runPendingTasks();
    }

    CSSCallbackWebFrameClient m_client;
    FrameTestHelpers::WebViewHelper m_helper;
    WebLocalFrame* m_frame;
};

TEST_F(WebFrameCSSCallbackTest, AuthorStyleSheet)
{
    loadHTML(
        "<style>"
        // This stylesheet checks that the internal property and value can't be
        // set by a stylesheet, only WebDocument::watchCSSSelectors().
        "div.initial_on { -internal-callback: none; }"
        "div.initial_off { -internal-callback: -internal-presence; }"
        "</style>"
        "<div class=\"initial_on\"></div>"
        "<div class=\"initial_off\"></div>");

    std::vector<WebString> selectors;
    selectors.push_back(WebString::fromUTF8("div.initial_on"));
    m_frame->document().watchCSSSelectors(WebVector<WebString>(selectors));
    m_frame->view()->layout();
    runPendingTasks();
    EXPECT_EQ(1, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre("div.initial_on"));

    // Check that adding a watched selector calls back for already-present nodes.
    selectors.push_back(WebString::fromUTF8("div.initial_off"));
    doc().watchCSSSelectors(WebVector<WebString>(selectors));
    m_frame->view()->layout();
    runPendingTasks();
    EXPECT_EQ(2, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre("div.initial_off", "div.initial_on"));

    // Check that we can turn off callbacks for certain selectors.
    doc().watchCSSSelectors(WebVector<WebString>());
    m_frame->view()->layout();
    runPendingTasks();
    EXPECT_EQ(3, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre());
}

TEST_F(WebFrameCSSCallbackTest, SharedComputedStyle)
{
    // Check that adding an element calls back when it matches an existing rule.
    std::vector<WebString> selectors;
    selectors.push_back(WebString::fromUTF8("span"));
    doc().watchCSSSelectors(WebVector<WebString>(selectors));

    executeScript(
        "i1 = document.createElement('span');"
        "i1.id = 'first_span';"
        "document.body.appendChild(i1)");
    EXPECT_EQ(1, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre("span"));

    // Adding a second element that shares a ComputedStyle shouldn't call back.
    // We use <span>s to avoid default style rules that can set
    // ComputedStyle::unique().
    executeScript(
        "i2 = document.createElement('span');"
        "i2.id = 'second_span';"
        "i1 = document.getElementById('first_span');"
        "i1.parentNode.insertBefore(i2, i1.nextSibling);");
    EXPECT_EQ(1, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre("span"));

    // Removing the first element shouldn't call back.
    executeScript(
        "i1 = document.getElementById('first_span');"
        "i1.parentNode.removeChild(i1);");
    EXPECT_EQ(1, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre("span"));

    // But removing the second element *should* call back.
    executeScript(
        "i2 = document.getElementById('second_span');"
        "i2.parentNode.removeChild(i2);");
    EXPECT_EQ(2, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre());
}

TEST_F(WebFrameCSSCallbackTest, CatchesAttributeChange)
{
    loadHTML("<span></span>");

    std::vector<WebString> selectors;
    selectors.push_back(WebString::fromUTF8("span[attr=\"value\"]"));
    doc().watchCSSSelectors(WebVector<WebString>(selectors));
    runPendingTasks();

    EXPECT_EQ(0, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre());

    executeScript(
        "document.querySelector('span').setAttribute('attr', 'value');");
    EXPECT_EQ(1, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre("span[attr=\"value\"]"));
}

TEST_F(WebFrameCSSCallbackTest, DisplayNone)
{
    loadHTML("<div style='display:none'><span></span></div>");

    std::vector<WebString> selectors;
    selectors.push_back(WebString::fromUTF8("span"));
    doc().watchCSSSelectors(WebVector<WebString>(selectors));
    runPendingTasks();

    EXPECT_EQ(0, updateCount()) << "Don't match elements in display:none trees.";

    executeScript(
        "d = document.querySelector('div');"
        "d.style.display = 'block';");
    EXPECT_EQ(1, updateCount()) << "Match elements when they become displayed.";
    EXPECT_THAT(matchedSelectors(), ElementsAre("span"));

    executeScript(
        "d = document.querySelector('div');"
        "d.style.display = 'none';");
    EXPECT_EQ(2, updateCount()) << "Unmatch elements when they become undisplayed.";
    EXPECT_THAT(matchedSelectors(), ElementsAre());

    executeScript(
        "s = document.querySelector('span');"
        "s.style.display = 'none';");
    EXPECT_EQ(2, updateCount()) << "No effect from no-display'ing a span that's already undisplayed.";

    executeScript(
        "d = document.querySelector('div');"
        "d.style.display = 'block';");
    EXPECT_EQ(2, updateCount()) << "No effect from displaying a div whose span is display:none.";

    executeScript(
        "s = document.querySelector('span');"
        "s.style.display = 'inline';");
    EXPECT_EQ(3, updateCount()) << "Now the span is visible and produces a callback.";
    EXPECT_THAT(matchedSelectors(), ElementsAre("span"));

    executeScript(
        "s = document.querySelector('span');"
        "s.style.display = 'none';");
    EXPECT_EQ(4, updateCount()) << "Undisplaying the span directly should produce another callback.";
    EXPECT_THAT(matchedSelectors(), ElementsAre());
}

TEST_F(WebFrameCSSCallbackTest, Reparenting)
{
    loadHTML(
        "<div id='d1'><span></span></div>"
        "<div id='d2'></div>");

    std::vector<WebString> selectors;
    selectors.push_back(WebString::fromUTF8("span"));
    doc().watchCSSSelectors(WebVector<WebString>(selectors));
    m_frame->view()->layout();
    runPendingTasks();

    EXPECT_EQ(1, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre("span"));

    executeScript(
        "s = document.querySelector('span');"
        "d2 = document.getElementById('d2');"
        "d2.appendChild(s);");
    EXPECT_EQ(1, updateCount()) << "Just moving an element that continues to match shouldn't send a spurious callback.";
    EXPECT_THAT(matchedSelectors(), ElementsAre("span"));
}

TEST_F(WebFrameCSSCallbackTest, MultiSelector)
{
    loadHTML("<span></span>");

    // Check that selector lists match as the whole list, not as each element
    // independently.
    std::vector<WebString> selectors;
    selectors.push_back(WebString::fromUTF8("span"));
    selectors.push_back(WebString::fromUTF8("span,p"));
    doc().watchCSSSelectors(WebVector<WebString>(selectors));
    m_frame->view()->layout();
    runPendingTasks();

    EXPECT_EQ(1, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre("span", "span, p"));
}

TEST_F(WebFrameCSSCallbackTest, InvalidSelector)
{
    loadHTML("<p><span></span></p>");

    // Build a list with one valid selector and one invalid.
    std::vector<WebString> selectors;
    selectors.push_back(WebString::fromUTF8("span"));
    selectors.push_back(WebString::fromUTF8("[")); // Invalid.
    selectors.push_back(WebString::fromUTF8("p span")); // Not compound.
    doc().watchCSSSelectors(WebVector<WebString>(selectors));
    m_frame->view()->layout();
    runPendingTasks();

    EXPECT_EQ(1, updateCount());
    EXPECT_THAT(matchedSelectors(), ElementsAre("span"))
        << "An invalid selector shouldn't prevent other selectors from matching.";
}

TEST_P(ParameterizedWebFrameTest, DispatchMessageEventWithOriginCheck)
{
    registerMockedHttpURLLoad("postmessage_test.html");

    // Pass true to enable JavaScript.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "postmessage_test.html", true);

    // Send a message with the correct origin.
    WebSecurityOrigin correctOrigin(WebSecurityOrigin::create(toKURL(m_baseURL)));
    WebDOMEvent event = webViewHelper.webView()->mainFrame()->document().createEvent("MessageEvent");
    WebDOMMessageEvent message = event.to<WebDOMMessageEvent>();
    WebSerializedScriptValue data(WebSerializedScriptValue::fromString("foo"));
    message.initMessageEvent("message", false, false, data, "http://origin.com", 0, "");
    webViewHelper.webView()->mainFrame()->dispatchMessageEventWithOriginCheck(correctOrigin, message);

    // Send another message with incorrect origin.
    WebSecurityOrigin incorrectOrigin(WebSecurityOrigin::create(toKURL(m_chromeURL)));
    webViewHelper.webView()->mainFrame()->dispatchMessageEventWithOriginCheck(incorrectOrigin, message);

    // Required to see any updates in contentAsText.
    webViewHelper.webView()->layout();

    // Verify that only the first addition is in the body of the page.
    std::string content = webViewHelper.webView()->mainFrame()->contentAsText(1024).utf8();
    EXPECT_NE(std::string::npos, content.find("Message 1."));
    EXPECT_EQ(std::string::npos, content.find("Message 2."));
}

TEST_P(ParameterizedWebFrameTest, PostMessageThenDetach)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank");

    RefPtrWillBeRawPtr<LocalFrame> frame = toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame());
    NonThrowableExceptionState exceptionState;
    frame->domWindow()->postMessage(SerializedScriptValueFactory::instance().create("message"), 0, "*", frame->localDOMWindow(), exceptionState);
    webViewHelper.reset();
    EXPECT_FALSE(exceptionState.hadException());

    // Success is not crashing.
    runPendingTasks();
}

namespace {

class FixedLayoutTestWebViewClient : public FrameTestHelpers::TestWebViewClient {
 public:
    WebScreenInfo screenInfo() override { return m_screenInfo; }

    WebScreenInfo m_screenInfo;
};

class FakeCompositingWebViewClient : public FixedLayoutTestWebViewClient {
};

// Viewport settings need to be set before the page gets loaded
void enableViewportSettings(WebSettings* settings)
{
    settings->setViewportMetaEnabled(true);
    settings->setViewportEnabled(true);
    settings->setMainFrameResizesAreOrientationChanges(true);
    settings->setShrinksViewportContentToFit(true);
}

// Helper function to set autosizing multipliers on a document.
bool setTextAutosizingMultiplier(Document* document, float multiplier)
{
    bool multiplierSet = false;
    for (LayoutObject* layoutObject = document->layoutView(); layoutObject; layoutObject = layoutObject->nextInPreOrder()) {
        if (layoutObject->style()) {
            layoutObject->mutableStyleRef().setTextAutosizingMultiplier(multiplier);

            EXPECT_EQ(multiplier, layoutObject->style()->textAutosizingMultiplier());
            multiplierSet = true;
        }
    }
    return multiplierSet;
}

// Helper function to check autosizing multipliers on a document.
bool checkTextAutosizingMultiplier(Document* document, float multiplier)
{
    bool multiplierChecked = false;
    for (LayoutObject* layoutObject = document->layoutView(); layoutObject; layoutObject = layoutObject->nextInPreOrder()) {
        if (layoutObject->style() && layoutObject->isText()) {
            EXPECT_EQ(multiplier, layoutObject->style()->textAutosizingMultiplier());
            multiplierChecked = true;
        }
    }
    return multiplierChecked;
}

} // anonymous namespace

TEST_P(ParameterizedWebFrameTest, ChangeInFixedLayoutResetsTextAutosizingMultipliers)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("fixed_layout.html");

    FixedLayoutTestWebViewClient client;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fixed_layout.html", true, 0, &client, enableViewportSettings);

    Document* document = toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->document();
    document->settings()->setTextAutosizingEnabled(true);
    EXPECT_TRUE(document->settings()->textAutosizingEnabled());
    webViewHelper.webViewImpl()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webViewImpl()->layout();

    EXPECT_TRUE(setTextAutosizingMultiplier(document, 2));

    ViewportDescription description = document->viewportDescription();
    // Choose a width that's not going match the viewport width of the loaded document.
    description.minWidth = Length(100, blink::Fixed);
    description.maxWidth = Length(100, blink::Fixed);
    webViewHelper.webViewImpl()->updatePageDefinedViewportConstraints(description);

    EXPECT_TRUE(checkTextAutosizingMultiplier(document, 1));
}

TEST_P(ParameterizedWebFrameTest, WorkingTextAutosizingMultipliers_VirtualViewport)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    const std::string htmlFile = "fixed_layout.html";
    registerMockedHttpURLLoad(htmlFile);

    FixedLayoutTestWebViewClient client;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + htmlFile, true, 0, &client, configureAndroid);

    Document* document = toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->document();
    document->settings()->setTextAutosizingEnabled(true);
    EXPECT_TRUE(document->settings()->textAutosizingEnabled());

    webViewHelper.webView()->resize(WebSize(490, 800));

    // Multiplier: 980 / 490 = 2.0
    EXPECT_TRUE(checkTextAutosizingMultiplier(document, 2.0));
}

TEST_P(ParameterizedWebFrameTest, PinchViewportSetSizeInvalidatesTextAutosizingMultipliers)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("iframe_reload.html");
    registerMockedHttpURLLoad("visible_iframe.html");

    FixedLayoutTestWebViewClient client;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "iframe_reload.html", true, 0, &client, enableViewportSettings);

    LocalFrame* mainFrame = toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame());
    Document* document = mainFrame->document();
    FrameView* frameView = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    document->settings()->setTextAutosizingEnabled(true);
    EXPECT_TRUE(document->settings()->textAutosizingEnabled());
    webViewHelper.webViewImpl()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webViewImpl()->layout();

    for (Frame* frame = mainFrame; frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;
        EXPECT_TRUE(setTextAutosizingMultiplier(toLocalFrame(frame)->document(), 2));
        for (LayoutObject* layoutObject = toLocalFrame(frame)->document()->layoutView(); layoutObject; layoutObject = layoutObject->nextInPreOrder()) {
            if (layoutObject->isText())
                EXPECT_FALSE(layoutObject->needsLayout());
        }
    }

    frameView->page()->frameHost().pinchViewport().setSize(IntSize(200, 200));

    for (Frame* frame = mainFrame; frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;
        for (LayoutObject* layoutObject = toLocalFrame(frame)->document()->layoutView(); layoutObject; layoutObject = layoutObject->nextInPreOrder()) {
            if (layoutObject->isText())
                EXPECT_TRUE(layoutObject->needsLayout());
        }
    }
}

TEST_P(ParameterizedWebFrameTest, ZeroHeightPositiveWidthNotIgnored)
{
    UseMockScrollbarSettings mockScrollbarSettings;

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 1280;
    int viewportHeight = 0;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());
    EXPECT_EQ(viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
}

TEST_P(ParameterizedWebFrameTest, DeviceScaleFactorUsesDefaultWithoutViewportTag)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("no_viewport_tag.html");

    int viewportWidth = 640;
    int viewportHeight = 480;

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 2;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "no_viewport_tag.html", true, 0, &client, enableViewportSettings);

    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    EXPECT_EQ(2, webViewHelper.webView()->deviceScaleFactor());

    // Device scale factor should be independent of page scale.
    webViewHelper.webView()->setDefaultPageScaleLimits(1, 2);
    webViewHelper.webView()->setPageScaleFactor(0.5);
    webViewHelper.webView()->layout();
    EXPECT_EQ(1, webViewHelper.webView()->pageScaleFactor());

    // Force the layout to happen before leaving the test.
    webViewHelper.webView()->mainFrame()->contentAsText(1024).utf8();
}

TEST_P(ParameterizedWebFrameTest, FixedLayoutInitializeAtMinimumScale)
{
    UseMockScrollbarSettings mockScrollbarSettings;

    registerMockedHttpURLLoad("fixed_layout.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    // Make sure we initialize to minimum scale, even if the window size
    // only becomes available after the load begins.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->setDefaultPageScaleLimits(0.25f, 5);
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "fixed_layout.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    int defaultFixedLayoutWidth = 980;
    float minimumPageScaleFactor = viewportWidth / (float) defaultFixedLayoutWidth;
    EXPECT_EQ(minimumPageScaleFactor, webViewHelper.webViewImpl()->pageScaleFactor());
    EXPECT_EQ(minimumPageScaleFactor, webViewHelper.webViewImpl()->minimumPageScaleFactor());

    // Assume the user has pinch zoomed to page scale factor 2.
    float userPinchPageScaleFactor = 2;
    webViewHelper.webView()->setPageScaleFactor(userPinchPageScaleFactor);
    webViewHelper.webView()->layout();

    // Make sure we don't reset to initial scale if the page continues to load.
    webViewHelper.webViewImpl()->didCommitLoad(false, false);
    webViewHelper.webViewImpl()->didChangeContentsSize();
    EXPECT_EQ(userPinchPageScaleFactor, webViewHelper.webView()->pageScaleFactor());

    // Make sure we don't reset to initial scale if the viewport size changes.
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight + 100));
    EXPECT_EQ(userPinchPageScaleFactor, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, WideDocumentInitializeAtMinimumScale)
{
    UseMockScrollbarSettings mockScrollbarSettings;

    registerMockedHttpURLLoad("wide_document.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    // Make sure we initialize to minimum scale, even if the window size
    // only becomes available after the load begins.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->setDefaultPageScaleLimits(0.25f, 5);
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "wide_document.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    int wideDocumentWidth = 1500;
    float minimumPageScaleFactor = viewportWidth / (float) wideDocumentWidth;
    EXPECT_EQ(minimumPageScaleFactor, webViewHelper.webViewImpl()->pageScaleFactor());
    EXPECT_EQ(minimumPageScaleFactor, webViewHelper.webViewImpl()->minimumPageScaleFactor());

    // Assume the user has pinch zoomed to page scale factor 2.
    float userPinchPageScaleFactor = 2;
    webViewHelper.webView()->setPageScaleFactor(userPinchPageScaleFactor);
    webViewHelper.webView()->layout();

    // Make sure we don't reset to initial scale if the page continues to load.
    webViewHelper.webViewImpl()->didCommitLoad(false, false);
    webViewHelper.webViewImpl()->didChangeContentsSize();
    EXPECT_EQ(userPinchPageScaleFactor, webViewHelper.webView()->pageScaleFactor());

    // Make sure we don't reset to initial scale if the viewport size changes.
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight + 100));
    EXPECT_EQ(userPinchPageScaleFactor, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, DelayedViewportInitialScale)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-auto-initial-scale.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-auto-initial-scale.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(0.25f, webViewHelper.webView()->pageScaleFactor());

    Document* document = toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->document();
    ViewportDescription description = document->viewportDescription();
    description.zoom = 2;
    document->setViewportDescription(description);
    webViewHelper.webView()->layout();
    EXPECT_EQ(2, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, setLoadWithOverviewModeToFalse)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-auto-initial-scale.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-auto-initial-scale.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setLoadWithOverviewMode(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    // The page must be displayed at 100% zoom.
    EXPECT_EQ(1.0f, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, SetLoadWithOverviewModeToFalseAndNoWideViewport)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("large-div.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "large-div.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setLoadWithOverviewMode(false);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    // The page must be displayed at 100% zoom, despite that it hosts a wide div element.
    EXPECT_EQ(1.0f, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, NoWideViewportIgnoresPageViewportWidth)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-auto-initial-scale.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-auto-initial-scale.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    // The page sets viewport width to 3000, but with UseWideViewport == false is must be ignored.
    EXPECT_EQ(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().width());
    EXPECT_EQ(viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().height());
}

TEST_P(ParameterizedWebFrameTest, NoWideViewportIgnoresPageViewportWidthButAccountsScale)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-wide-2x-initial-scale.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-wide-2x-initial-scale.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    // The page sets viewport width to 3000, but with UseWideViewport == false it must be ignored.
    // While the initial scale specified by the page must be accounted.
    EXPECT_EQ(viewportWidth / 2, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().width());
    EXPECT_EQ(viewportHeight / 2, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().height());
}

TEST_P(ParameterizedWebFrameTest, WideViewportSetsTo980WithoutViewportTag)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("no_viewport_tag.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "no_viewport_tag.html", true, 0, &client, enableViewportSettings);
    applyViewportStyleOverride(&webViewHelper);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(980, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().width());
    EXPECT_EQ(980.0 / viewportWidth * viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().height());
}

TEST_P(ParameterizedWebFrameTest, WideViewportSetsTo980WithXhtmlMp)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-legacy-xhtmlmp.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &client, enableViewportSettings);
    applyViewportStyleOverride(&webViewHelper);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(true);
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "viewport/viewport-legacy-xhtmlmp.html");

    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    EXPECT_EQ(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().width());
    EXPECT_EQ(viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().height());
}

TEST_P(ParameterizedWebFrameTest, NoWideViewportAndHeightInMeta)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-height-1000.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-height-1000.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().width());
}

TEST_P(ParameterizedWebFrameTest, WideViewportSetsTo980WithAutoWidth)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-2x-initial-scale.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-2x-initial-scale.html", true, 0, &client, enableViewportSettings);
    applyViewportStyleOverride(&webViewHelper);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(980, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().width());
    EXPECT_EQ(980.0 / viewportWidth * viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().height());
}

TEST_P(ParameterizedWebFrameTest, PageViewportInitialScaleOverridesLoadWithOverviewMode)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-wide-2x-initial-scale.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-wide-2x-initial-scale.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setLoadWithOverviewMode(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    // The page must be displayed at 200% zoom, as specified in its viewport meta tag.
    EXPECT_EQ(2.0f, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, setInitialPageScaleFactorPermanently)
{
    UseMockScrollbarSettings mockScrollbarSettings;

    registerMockedHttpURLLoad("fixed_layout.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    float enforcedPageScaleFactor = 2.0f;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fixed_layout.html", true, 0, &client, enableViewportSettings);
    applyViewportStyleOverride(&webViewHelper);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setLoadWithOverviewMode(false);
    webViewHelper.webView()->setInitialPageScaleOverride(enforcedPageScaleFactor);
    webViewHelper.webView()->layout();

    EXPECT_EQ(enforcedPageScaleFactor, webViewHelper.webView()->pageScaleFactor());

    int viewportWidth = 640;
    int viewportHeight = 480;
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    EXPECT_EQ(enforcedPageScaleFactor, webViewHelper.webView()->pageScaleFactor());

    webViewHelper.webView()->setInitialPageScaleOverride(-1);
    webViewHelper.webView()->layout();
    EXPECT_EQ(1.0, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, PermanentInitialPageScaleFactorOverridesLoadWithOverviewMode)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-auto-initial-scale.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;
    float enforcedPageScaleFactor = 0.5f;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-auto-initial-scale.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setLoadWithOverviewMode(false);
    webViewHelper.webView()->setInitialPageScaleOverride(enforcedPageScaleFactor);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(enforcedPageScaleFactor, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, PermanentInitialPageScaleFactorOverridesPageViewportInitialScale)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-wide-2x-initial-scale.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;
    float enforcedPageScaleFactor = 0.5f;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-wide-2x-initial-scale.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->setInitialPageScaleOverride(enforcedPageScaleFactor);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(enforcedPageScaleFactor, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, SmallPermanentInitialPageScaleFactorIsClobbered)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    const char* pages[] = {
        // These pages trigger the clobbering condition. There must be a matching item in "pageScaleFactors" array.
        "viewport-device-0.5x-initial-scale.html",
        "viewport-initial-scale-1.html",
        // These ones do not.
        "viewport-auto-initial-scale.html",
        "viewport-target-densitydpi-device-and-fixed-width.html"
    };
    float pageScaleFactors[] = { 0.5f, 1.0f };
    for (size_t i = 0; i < ARRAY_SIZE(pages); ++i)
        registerMockedHttpURLLoad(pages[i]);

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 400;
    int viewportHeight = 300;
    float enforcedPageScaleFactor = 0.75f;

    for (size_t i = 0; i < ARRAY_SIZE(pages); ++i) {
        for (int quirkEnabled = 0; quirkEnabled <= 1; ++quirkEnabled) {
            FrameTestHelpers::WebViewHelper webViewHelper(this);
            webViewHelper.initializeAndLoad(m_baseURL + pages[i], true, 0, &client, enableViewportSettings);
            applyViewportStyleOverride(&webViewHelper);
            webViewHelper.webView()->settings()->setClobberUserAgentInitialScaleQuirk(quirkEnabled);
            webViewHelper.webView()->setInitialPageScaleOverride(enforcedPageScaleFactor);
            webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

            float expectedPageScaleFactor = quirkEnabled && i < ARRAY_SIZE(pageScaleFactors) ? pageScaleFactors[i] : enforcedPageScaleFactor;
            EXPECT_EQ(expectedPageScaleFactor, webViewHelper.webView()->pageScaleFactor());
        }
    }
}

TEST_P(ParameterizedWebFrameTest, PermanentInitialPageScaleFactorAffectsLayoutWidth)
{
    UseMockScrollbarSettings mockScrollbarSettings;

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;
    float enforcedPageScaleFactor = 0.5;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->settings()->setLoadWithOverviewMode(false);
    webViewHelper.webView()->setInitialPageScaleOverride(enforcedPageScaleFactor);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(viewportWidth / enforcedPageScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().width());
    EXPECT_EQ(enforcedPageScaleFactor, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, DocumentElementClientHeightWorksWithWrapContentMode)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("0-by-0.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);

    webViewHelper.initializeAndLoad(m_baseURL + "0-by-0.html", true, 0, &client, configureAndroid);
    webViewHelper.webView()->settings()->setForceZeroLayoutHeight(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    LocalFrame* frame = webViewHelper.webViewImpl()->mainFrameImpl()->frame();
    Document* document = frame->document();
    EXPECT_EQ(viewportHeight, document->documentElement()->clientHeight());
    EXPECT_EQ(viewportWidth, document->documentElement()->clientWidth());
}

TEST_P(ParameterizedWebFrameTest, SetForceZeroLayoutHeightWorksWithWrapContentMode)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("0-by-0.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);

    webViewHelper.initializeAndLoad(m_baseURL + "0-by-0.html", true, 0, &client, configureAndroid);
    webViewHelper.webView()->settings()->setForceZeroLayoutHeight(true);
    DeprecatedPaintLayerCompositor* compositor = webViewHelper.webViewImpl()->compositor();
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
    EXPECT_EQ(0.0, compositor->containerLayer()->size().width());
    EXPECT_EQ(0.0, compositor->containerLayer()->size().height());

    webViewHelper.webView()->resize(WebSize(viewportWidth, 0));
    EXPECT_EQ(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
    EXPECT_EQ(viewportWidth, compositor->containerLayer()->size().width());
    EXPECT_EQ(0.0, compositor->containerLayer()->size().height());

    // Two flags PinchVirtualViewportEnabled and ForceZeroLayoutHeight will cause the following
    // resize of viewport height to be ignored by the outer viewport (the container layer of
    // LayerCompositor). The height of the pinchViewport, however, is not affected.
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    EXPECT_FALSE(webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->needsLayout());
    EXPECT_EQ(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
    EXPECT_EQ(viewportWidth, compositor->containerLayer()->size().width());
    EXPECT_EQ(viewportHeight, compositor->containerLayer()->size().height());

    LocalFrame* frame = webViewHelper.webViewImpl()->mainFrameImpl()->frame();
    PinchViewport& pinchViewport = frame->page()->frameHost().pinchViewport();
    EXPECT_EQ(viewportHeight, pinchViewport.containerLayer()->size().height());
    EXPECT_TRUE(pinchViewport.containerLayer()->platformLayer()->masksToBounds());
    EXPECT_FALSE(compositor->containerLayer()->platformLayer()->masksToBounds());
}

TEST_P(ParameterizedWebFrameTest, SetForceZeroLayoutHeight)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("200-by-300.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);

    webViewHelper.initializeAndLoad(m_baseURL + "200-by-300.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    EXPECT_LE(viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
    webViewHelper.webView()->settings()->setForceZeroLayoutHeight(true);
    EXPECT_TRUE(webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->needsLayout());

    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());

    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight * 2));
    EXPECT_FALSE(webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->needsLayout());
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());

    webViewHelper.webView()->resize(WebSize(viewportWidth * 2, viewportHeight));
    webViewHelper.webView()->layout();
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());

    webViewHelper.webView()->settings()->setForceZeroLayoutHeight(false);
    EXPECT_LE(viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
}

TEST_F(WebFrameTest, SetForceZeroLayoutHeightWorksWithRelayoutsWhenHeightChanged)
{
    // this unit test is an attempt to target a real world case where an app could
    // 1. call resize(width, 0) and setForceZeroLayoutHeight(true)
    // 2. load content (hoping that the viewport height would increase
    // as more content is added)
    // 3. fail to register touch events aimed at the loaded content
    // because the layout is only updated if either width or height is changed
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("button.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper;

    webViewHelper.initializeAndLoad(m_baseURL + "button.html", true, 0, &client, configureAndroid);
    // set view height to zero so that if the height of the view is not
    // successfully updated during later resizes touch events will fail
    // (as in not hit content included in the view)
    webViewHelper.webView()->resize(WebSize(viewportWidth, 0));
    webViewHelper.webView()->layout();

    webViewHelper.webView()->settings()->setForceZeroLayoutHeight(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    IntPoint hitPoint = IntPoint(30, 30); // button size is 100x100

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    Document* document = frame->frame()->document();
    Element* element = document->getElementById("tap_button");

    ASSERT_NE(nullptr, element);
    EXPECT_EQ(String("oldValue"), element->innerText());

    PlatformGestureEvent gestureEvent(PlatformEvent::Type::GestureTap, hitPoint, hitPoint, IntSize(0, 0), 0, false, false, false, false);
    webViewHelper.webViewImpl()->mainFrameImpl()->frame()->eventHandler().handleGestureEvent(gestureEvent);
    // when pressed, the button changes its own text to "updatedValue"
    EXPECT_EQ(String("updatedValue"), element->innerText());
}

TEST_P(ParameterizedWebFrameTest, SetForceZeroLayoutHeightWorksAcrossNavigations)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("200-by-300.html");
    registerMockedHttpURLLoad("large-div.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);

    webViewHelper.initializeAndLoad(m_baseURL + "200-by-300.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setForceZeroLayoutHeight(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "large-div.html");
    webViewHelper.webView()->layout();

    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
}

TEST_P(ParameterizedWebFrameTest, SetForceZeroLayoutHeightWithWideViewportQuirk)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("200-by-300.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);

    webViewHelper.initializeAndLoad(m_baseURL + "200-by-300.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(true);
    webViewHelper.webView()->settings()->setForceZeroLayoutHeight(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
}

TEST_P(ParameterizedWebFrameTest, WideViewportAndWideContentWithInitialScale)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("wide_document_width_viewport.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 600;
    int viewportHeight = 800;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(true);
    webViewHelper.webView()->settings()->setViewportMetaLayoutSizeQuirk(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "wide_document_width_viewport.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    int wideDocumentWidth = 800;
    float minimumPageScaleFactor = viewportWidth / (float) wideDocumentWidth;
    EXPECT_EQ(minimumPageScaleFactor, webViewHelper.webViewImpl()->pageScaleFactor());
    EXPECT_EQ(minimumPageScaleFactor, webViewHelper.webViewImpl()->minimumPageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, WideViewportQuirkClobbersHeight)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-height-1000.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 600;
    int viewportHeight = 800;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->settings()->setViewportMetaLayoutSizeQuirk(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "viewport-height-1000.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(800, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
    EXPECT_EQ(1, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, LayoutSize320Quirk)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport/viewport-30.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 600;
    int viewportHeight = 800;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(true);
    webViewHelper.webView()->settings()->setViewportMetaLayoutSizeQuirk(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "viewport/viewport-30.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(600, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());
    EXPECT_EQ(800, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
    EXPECT_EQ(1, webViewHelper.webView()->pageScaleFactor());

    // The magic number to snap to device-width is 320, so test that 321 is
    // respected.
    Document* document = toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->document();
    ViewportDescription description = document->viewportDescription();
    description.minWidth = Length(321, blink::Fixed);
    description.maxWidth = Length(321, blink::Fixed);
    document->setViewportDescription(description);
    webViewHelper.webView()->layout();
    EXPECT_EQ(321, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());

    description.minWidth = Length(320, blink::Fixed);
    description.maxWidth = Length(320, blink::Fixed);
    document->setViewportDescription(description);
    webViewHelper.webView()->layout();
    EXPECT_EQ(600, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());

    description = document->viewportDescription();
    description.maxHeight = Length(1000, blink::Fixed);
    document->setViewportDescription(description);
    webViewHelper.webView()->layout();
    EXPECT_EQ(1000, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());

    description.maxHeight = Length(320, blink::Fixed);
    document->setViewportDescription(description);
    webViewHelper.webView()->layout();
    EXPECT_EQ(800, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height());
}

TEST_P(ParameterizedWebFrameTest, ZeroValuesQuirk)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-zero-values.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setViewportMetaZeroValuesQuirk(true);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setViewportMetaLayoutSizeQuirk(true);
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "viewport-zero-values.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());
    EXPECT_EQ(1.0f, webViewHelper.webView()->pageScaleFactor());

    webViewHelper.webView()->settings()->setUseWideViewport(true);
    webViewHelper.webView()->layout();
    EXPECT_EQ(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());
    EXPECT_EQ(1.0f, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, OverflowHiddenDisablesScrolling)
{
    registerMockedHttpURLLoad("body-overflow-hidden.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &client);
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "body-overflow-hidden.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    FrameView* view = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    EXPECT_FALSE(view->userInputScrollable(VerticalScrollbar));
    EXPECT_FALSE(view->userInputScrollable(HorizontalScrollbar));
}

TEST_P(ParameterizedWebFrameTest, OverflowHiddenDisablesScrollingWithSetCanHaveScrollbars)
{
    registerMockedHttpURLLoad("body-overflow-hidden-short.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &client);
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "body-overflow-hidden-short.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    FrameView* view = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    EXPECT_FALSE(view->userInputScrollable(VerticalScrollbar));
    EXPECT_FALSE(view->userInputScrollable(HorizontalScrollbar));

    webViewHelper.webViewImpl()->mainFrameImpl()->setCanHaveScrollbars(true);
    EXPECT_FALSE(view->userInputScrollable(VerticalScrollbar));
    EXPECT_FALSE(view->userInputScrollable(HorizontalScrollbar));
}

TEST_F(WebFrameTest, IgnoreOverflowHiddenQuirk)
{
    registerMockedHttpURLLoad("body-overflow-hidden.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initialize(true, 0, &client);
    webViewHelper.webView()->settings()->setIgnoreMainFrameOverflowHiddenQuirk(true);
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "body-overflow-hidden.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    FrameView* view = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    EXPECT_TRUE(view->userInputScrollable(VerticalScrollbar));
}

TEST_P(ParameterizedWebFrameTest, NonZeroValuesNoQuirk)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-nonzero-values.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;
    float expectedPageScaleFactor = 0.5f;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setViewportMetaZeroValuesQuirk(true);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "viewport-nonzero-values.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(viewportWidth / expectedPageScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());
    EXPECT_EQ(expectedPageScaleFactor, webViewHelper.webView()->pageScaleFactor());

    webViewHelper.webView()->settings()->setUseWideViewport(true);
    webViewHelper.webView()->layout();
    EXPECT_EQ(viewportWidth / expectedPageScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width());
    EXPECT_EQ(expectedPageScaleFactor, webViewHelper.webView()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, setPageScaleFactorDoesNotLayout)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("fixed_layout.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    // Small viewport to ensure there are always scrollbars.
    int viewportWidth = 64;
    int viewportHeight = 48;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fixed_layout.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    int prevLayoutCount = webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutCount();
    webViewHelper.webViewImpl()->setPageScaleFactor(3);
    EXPECT_FALSE(webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->needsLayout());
    EXPECT_EQ(prevLayoutCount, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutCount());
}

TEST_P(ParameterizedWebFrameTest, setPageScaleFactorWithOverlayScrollbarsDoesNotLayout)
{
    UseMockScrollbarSettings mockScrollbarSettings;

    registerMockedHttpURLLoad("fixed_layout.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fixed_layout.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    int prevLayoutCount = webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutCount();
    webViewHelper.webViewImpl()->setPageScaleFactor(30);
    EXPECT_FALSE(webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->needsLayout());
    EXPECT_EQ(prevLayoutCount, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutCount());

}

TEST_P(ParameterizedWebFrameTest, pageScaleFactorWrittenToHistoryItem)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("fixed_layout.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fixed_layout.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    webViewHelper.webView()->setPageScaleFactor(3);
    EXPECT_EQ(3, toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->loader().currentItem()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, initialScaleWrittenToHistoryItem)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("fixed_layout.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->setDefaultPageScaleLimits(0.25f, 5);
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "fixed_layout.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    int defaultFixedLayoutWidth = 980;
    float minimumPageScaleFactor = viewportWidth / (float) defaultFixedLayoutWidth;
    EXPECT_EQ(minimumPageScaleFactor, toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->loader().currentItem()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, pageScaleFactorDoesntShrinkFrameView)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("large-div.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    // Small viewport to ensure there are always scrollbars.
    int viewportWidth = 64;
    int viewportHeight = 48;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "large-div.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    FrameView* view = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    int viewportWidthMinusScrollbar = viewportWidth;
    int viewportHeightMinusScrollbar = viewportHeight;

    if (view->verticalScrollbar() && !view->verticalScrollbar()->isOverlayScrollbar())
        viewportWidthMinusScrollbar -= 15;

    if (view->horizontalScrollbar() && !view->horizontalScrollbar()->isOverlayScrollbar())
        viewportHeightMinusScrollbar -= 15;

    webViewHelper.webView()->setPageScaleFactor(2);

    IntSize unscaledSize = view->visibleContentSize(IncludeScrollbars);
    EXPECT_EQ(viewportWidth, unscaledSize.width());
    EXPECT_EQ(viewportHeight, unscaledSize.height());

    IntSize unscaledSizeMinusScrollbar = view->visibleContentSize(ExcludeScrollbars);
    EXPECT_EQ(viewportWidthMinusScrollbar, unscaledSizeMinusScrollbar.width());
    EXPECT_EQ(viewportHeightMinusScrollbar, unscaledSizeMinusScrollbar.height());

    IntSize frameViewSize = view->visibleContentRect().size();
    EXPECT_EQ(viewportWidthMinusScrollbar, frameViewSize.width());
    EXPECT_EQ(viewportHeightMinusScrollbar, frameViewSize.height());
}

TEST_P(ParameterizedWebFrameTest, pageScaleFactorDoesNotApplyCssTransform)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("fixed_layout.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fixed_layout.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    webViewHelper.webView()->setPageScaleFactor(2);

    EXPECT_EQ(980, toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->contentLayoutObject()->unscaledDocumentRect().width());
    EXPECT_EQ(980, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->contentsSize().width());
}

TEST_P(ParameterizedWebFrameTest, targetDensityDpiHigh)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-target-densitydpi-high.html");

    FixedLayoutTestWebViewClient client;
    // high-dpi = 240
    float targetDpi = 240.0f;
    float deviceScaleFactors[] = { 1.0f, 4.0f / 3.0f, 2.0f };
    int viewportWidth = 640;
    int viewportHeight = 480;

    for (size_t i = 0; i < ARRAY_SIZE(deviceScaleFactors); ++i) {
        float deviceScaleFactor = deviceScaleFactors[i];
        float deviceDpi = deviceScaleFactor * 160.0f;
        client.m_screenInfo.deviceScaleFactor = deviceScaleFactor;

        FrameTestHelpers::WebViewHelper webViewHelper(this);
        webViewHelper.initializeAndLoad(m_baseURL + "viewport-target-densitydpi-high.html", true, 0, &client, enableViewportSettings);
        webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
        webViewHelper.webView()->settings()->setSupportDeprecatedTargetDensityDPI(true);
        webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

        // We need to account for the fact that logical pixels are unconditionally multiplied by deviceScaleFactor to produce
        // physical pixels.
        float densityDpiScaleRatio = deviceScaleFactor * targetDpi / deviceDpi;
        EXPECT_NEAR(viewportWidth * densityDpiScaleRatio, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width(), 1.0f);
        EXPECT_NEAR(viewportHeight * densityDpiScaleRatio, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height(), 1.0f);
        EXPECT_NEAR(1.0f / densityDpiScaleRatio, webViewHelper.webView()->pageScaleFactor(), 0.01f);
    }
}

TEST_P(ParameterizedWebFrameTest, targetDensityDpiDevice)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-target-densitydpi-device.html");

    float deviceScaleFactors[] = { 1.0f, 4.0f / 3.0f, 2.0f };

    FixedLayoutTestWebViewClient client;
    int viewportWidth = 640;
    int viewportHeight = 480;

    for (size_t i = 0; i < ARRAY_SIZE(deviceScaleFactors); ++i) {
        client.m_screenInfo.deviceScaleFactor = deviceScaleFactors[i];

        FrameTestHelpers::WebViewHelper webViewHelper(this);
        webViewHelper.initializeAndLoad(m_baseURL + "viewport-target-densitydpi-device.html", true, 0, &client, enableViewportSettings);
        webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
        webViewHelper.webView()->settings()->setSupportDeprecatedTargetDensityDPI(true);
        webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

        EXPECT_NEAR(viewportWidth * client.m_screenInfo.deviceScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width(), 1.0f);
        EXPECT_NEAR(viewportHeight * client.m_screenInfo.deviceScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height(), 1.0f);
        EXPECT_NEAR(1.0f / client.m_screenInfo.deviceScaleFactor, webViewHelper.webView()->pageScaleFactor(), 0.01f);
    }
}

TEST_P(ParameterizedWebFrameTest, targetDensityDpiDeviceAndFixedWidth)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-target-densitydpi-device-and-fixed-width.html");

    float deviceScaleFactors[] = { 1.0f, 4.0f / 3.0f, 2.0f };

    FixedLayoutTestWebViewClient client;
    int viewportWidth = 640;
    int viewportHeight = 480;

    for (size_t i = 0; i < ARRAY_SIZE(deviceScaleFactors); ++i) {
        client.m_screenInfo.deviceScaleFactor = deviceScaleFactors[i];

        FrameTestHelpers::WebViewHelper webViewHelper(this);
        webViewHelper.initializeAndLoad(m_baseURL + "viewport-target-densitydpi-device-and-fixed-width.html", true, 0, &client, enableViewportSettings);
        webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
        webViewHelper.webView()->settings()->setSupportDeprecatedTargetDensityDPI(true);
        webViewHelper.webView()->settings()->setUseWideViewport(true);
        webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

        EXPECT_NEAR(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width(), 1.0f);
        EXPECT_NEAR(viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height(), 1.0f);
        EXPECT_NEAR(1.0f, webViewHelper.webView()->pageScaleFactor(), 0.01f);
    }
}

TEST_P(ParameterizedWebFrameTest, NoWideViewportAndScaleLessThanOne)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-initial-scale-less-than-1.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1.33f;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-initial-scale-less-than-1.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setSupportDeprecatedTargetDensityDPI(true);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    EXPECT_NEAR(viewportWidth * client.m_screenInfo.deviceScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width(), 1.0f);
    EXPECT_NEAR(viewportHeight * client.m_screenInfo.deviceScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height(), 1.0f);
    EXPECT_NEAR(1.0f / client.m_screenInfo.deviceScaleFactor, webViewHelper.webView()->pageScaleFactor(), 0.01f);
}

TEST_P(ParameterizedWebFrameTest, NoWideViewportAndScaleLessThanOneWithDeviceWidth)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-initial-scale-less-than-1-device-width.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1.33f;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-initial-scale-less-than-1-device-width.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setSupportDeprecatedTargetDensityDPI(true);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    const float pageZoom = 0.25f;
    EXPECT_NEAR(viewportWidth * client.m_screenInfo.deviceScaleFactor / pageZoom, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width(), 1.0f);
    EXPECT_NEAR(viewportHeight * client.m_screenInfo.deviceScaleFactor / pageZoom, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height(), 1.0f);
    EXPECT_NEAR(1.0f / client.m_screenInfo.deviceScaleFactor, webViewHelper.webView()->pageScaleFactor(), 0.01f);
}

TEST_P(ParameterizedWebFrameTest, NoWideViewportAndNoViewportWithInitialPageScaleOverride)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("large-div.html");

    FixedLayoutTestWebViewClient client;
    int viewportWidth = 640;
    int viewportHeight = 480;
    float enforcedPageScaleFactor = 5.0f;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "large-div.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->setDefaultPageScaleLimits(0.25f, 5);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->setInitialPageScaleOverride(enforcedPageScaleFactor);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    EXPECT_NEAR(viewportWidth / enforcedPageScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width(), 1.0f);
    EXPECT_NEAR(viewportHeight / enforcedPageScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height(), 1.0f);
    EXPECT_NEAR(enforcedPageScaleFactor, webViewHelper.webView()->pageScaleFactor(), 0.01f);
}

TEST_P(ParameterizedWebFrameTest, NoUserScalableQuirkIgnoresViewportScale)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-initial-scale-and-user-scalable-no.html");

    FixedLayoutTestWebViewClient client;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-initial-scale-and-user-scalable-no.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setViewportMetaNonUserScalableQuirk(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    EXPECT_NEAR(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width(), 1.0f);
    EXPECT_NEAR(viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height(), 1.0f);
    EXPECT_NEAR(1.0f, webViewHelper.webView()->pageScaleFactor(), 0.01f);
}

TEST_P(ParameterizedWebFrameTest, NoUserScalableQuirkIgnoresViewportScaleForNonWideViewport)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-initial-scale-and-user-scalable-no.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1.33f;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-initial-scale-and-user-scalable-no.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setSupportDeprecatedTargetDensityDPI(true);
    webViewHelper.webView()->settings()->setViewportMetaNonUserScalableQuirk(true);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    EXPECT_NEAR(viewportWidth * client.m_screenInfo.deviceScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width(), 1.0f);
    EXPECT_NEAR(viewportHeight * client.m_screenInfo.deviceScaleFactor, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height(), 1.0f);
    EXPECT_NEAR(1.0f / client.m_screenInfo.deviceScaleFactor, webViewHelper.webView()->pageScaleFactor(), 0.01f);
}

TEST_P(ParameterizedWebFrameTest, NoUserScalableQuirkIgnoresViewportScaleForWideViewport)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("viewport-2x-initial-scale-non-user-scalable.html");

    FixedLayoutTestWebViewClient client;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "viewport-2x-initial-scale-non-user-scalable.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->settings()->setViewportMetaNonUserScalableQuirk(true);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(true);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_NEAR(viewportWidth, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().width(), 1.0f);
    EXPECT_NEAR(viewportHeight, webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutSize().height(), 1.0f);
    EXPECT_NEAR(1.0f, webViewHelper.webView()->pageScaleFactor(), 0.01f);
}

TEST_P(ParameterizedWebFrameTest, DesktopPageCanBeZoomedInWhenWideViewportIsTurnedOff)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("no_viewport_tag.html");

    FixedLayoutTestWebViewClient client;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "no_viewport_tag.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->setDefaultPageScaleLimits(0.25f, 5);
    webViewHelper.webView()->settings()->setWideViewportQuirkEnabled(true);
    webViewHelper.webView()->settings()->setUseWideViewport(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_NEAR(1.0f, webViewHelper.webViewImpl()->pageScaleFactor(), 0.01f);
    EXPECT_NEAR(1.0f, webViewHelper.webViewImpl()->minimumPageScaleFactor(), 0.01f);
    EXPECT_NEAR(5.0f, webViewHelper.webViewImpl()->maximumPageScaleFactor(), 0.01f);
}

class WebFrameResizeTest : public ParameterizedWebFrameTest {
protected:

    static FloatSize computeRelativeOffset(const IntPoint& absoluteOffset, const LayoutRect& rect)
    {
        FloatSize relativeOffset = FloatPoint(absoluteOffset) - FloatPoint(rect.location());
        relativeOffset.scale(1.f / rect.width(), 1.f / rect.height());
        return relativeOffset;
    }

    void testResizeYieldsCorrectScrollAndScale(const char* url,
                                               const float initialPageScaleFactor,
                                               const WebSize scrollOffset,
                                               const WebSize viewportSize,
                                               const bool shouldScaleRelativeToViewportWidth) {
        UseMockScrollbarSettings mockScrollbarSettings;
        registerMockedHttpURLLoad(url);

        const float aspectRatio = static_cast<float>(viewportSize.width) / viewportSize.height;

        FrameTestHelpers::WebViewHelper webViewHelper(this);
        webViewHelper.initializeAndLoad(m_baseURL + url, true, 0, 0, enableViewportSettings);
        webViewHelper.webViewImpl()->setDefaultPageScaleLimits(0.25f, 5);

        // Origin scrollOffsets preserved under resize.
        {
            webViewHelper.webViewImpl()->resize(WebSize(viewportSize.width, viewportSize.height));
            webViewHelper.webViewImpl()->setPageScaleFactor(initialPageScaleFactor);
            ASSERT_EQ(viewportSize, webViewHelper.webViewImpl()->size());
            ASSERT_EQ(initialPageScaleFactor, webViewHelper.webViewImpl()->pageScaleFactor());
            webViewHelper.webViewImpl()->resize(WebSize(viewportSize.height, viewportSize.width));
            float expectedPageScaleFactor = initialPageScaleFactor * (shouldScaleRelativeToViewportWidth ? 1 / aspectRatio : 1);
            EXPECT_NEAR(expectedPageScaleFactor, webViewHelper.webViewImpl()->pageScaleFactor(), 0.05f);
            EXPECT_EQ(WebSize(), webViewHelper.webViewImpl()->mainFrame()->scrollOffset());
        }

        // Resizing just the height should not affect pageScaleFactor or scrollOffset.
        {
            webViewHelper.webViewImpl()->resize(WebSize(viewportSize.width, viewportSize.height));
            webViewHelper.webViewImpl()->setPageScaleFactor(initialPageScaleFactor);
            webViewHelper.webViewImpl()->mainFrame()->setScrollOffset(scrollOffset);
            webViewHelper.webViewImpl()->layout();
            const WebSize expectedScrollOffset = webViewHelper.webViewImpl()->mainFrame()->scrollOffset();
            webViewHelper.webViewImpl()->resize(WebSize(viewportSize.width, viewportSize.height * 0.8f));
            EXPECT_EQ(initialPageScaleFactor, webViewHelper.webViewImpl()->pageScaleFactor());
            EXPECT_EQ(expectedScrollOffset, webViewHelper.webViewImpl()->mainFrame()->scrollOffset());
            webViewHelper.webViewImpl()->resize(WebSize(viewportSize.width, viewportSize.height * 0.8f));
            EXPECT_EQ(initialPageScaleFactor, webViewHelper.webViewImpl()->pageScaleFactor());
            EXPECT_EQ(expectedScrollOffset, webViewHelper.webViewImpl()->mainFrame()->scrollOffset());
        }
    }
};

INSTANTIATE_TEST_CASE_P(All, WebFrameResizeTest, ::testing::Values(
    ParameterizedWebFrameTestConfig::Default,
    ParameterizedWebFrameTestConfig::RootLayerScrolls));

TEST_P(WebFrameResizeTest, ResizeYieldsCorrectScrollAndScaleForWidthEqualsDeviceWidth)
{
    // With width=device-width, pageScaleFactor is preserved across resizes as
    // long as the content adjusts according to the device-width.
    const char* url = "resize_scroll_mobile.html";
    const float initialPageScaleFactor = 1;
    const WebSize scrollOffset(0, 50);
    const WebSize viewportSize(120, 160);
    const bool shouldScaleRelativeToViewportWidth = true;

    testResizeYieldsCorrectScrollAndScale(
        url, initialPageScaleFactor, scrollOffset, viewportSize, shouldScaleRelativeToViewportWidth);
}

TEST_P(WebFrameResizeTest, ResizeYieldsCorrectScrollAndScaleForMinimumScale)
{
    // This tests a scenario where minimum-scale is set to 1.0, but some element
    // on the page is slightly larger than the portrait width, so our "natural"
    // minimum-scale would be lower. In that case, we should stick to 1.0 scale
    // on rotation and not do anything strange.
    const char* url = "resize_scroll_minimum_scale.html";
    const float initialPageScaleFactor = 1;
    const WebSize scrollOffset(0, 0);
    const WebSize viewportSize(240, 320);
    const bool shouldScaleRelativeToViewportWidth = false;

    testResizeYieldsCorrectScrollAndScale(
        url, initialPageScaleFactor, scrollOffset, viewportSize, shouldScaleRelativeToViewportWidth);
}

TEST_P(WebFrameResizeTest, ResizeYieldsCorrectScrollAndScaleForFixedWidth)
{
    // With a fixed width, pageScaleFactor scales by the relative change in viewport width.
    const char* url = "resize_scroll_fixed_width.html";
    const float initialPageScaleFactor = 2;
    const WebSize scrollOffset(0, 200);
    const WebSize viewportSize(240, 320);
    const bool shouldScaleRelativeToViewportWidth = true;

    testResizeYieldsCorrectScrollAndScale(
        url, initialPageScaleFactor, scrollOffset, viewportSize, shouldScaleRelativeToViewportWidth);
}

TEST_P(WebFrameResizeTest, ResizeYieldsCorrectScrollAndScaleForFixedLayout)
{
    // With a fixed layout, pageScaleFactor scales by the relative change in viewport width.
    const char* url = "resize_scroll_fixed_layout.html";
    const float initialPageScaleFactor = 2;
    const WebSize scrollOffset(200, 400);
    const WebSize viewportSize(320, 240);
    const bool shouldScaleRelativeToViewportWidth = true;

    testResizeYieldsCorrectScrollAndScale(
        url, initialPageScaleFactor, scrollOffset, viewportSize, shouldScaleRelativeToViewportWidth);
}

TEST_P(ParameterizedWebFrameTest, pageScaleFactorUpdatesScrollbars)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("fixed_layout.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fixed_layout.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    FrameView* view = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    EXPECT_EQ(view->scrollSize(HorizontalScrollbar), view->contentsSize().width() - view->visibleContentRect().width());
    EXPECT_EQ(view->scrollSize(VerticalScrollbar), view->contentsSize().height() - view->visibleContentRect().height());

    webViewHelper.webView()->setPageScaleFactor(10);

    EXPECT_EQ(view->scrollSize(HorizontalScrollbar), view->contentsSize().width() - view->visibleContentRect().width());
    EXPECT_EQ(view->scrollSize(VerticalScrollbar), view->contentsSize().height() - view->visibleContentRect().height());
}

TEST_P(ParameterizedWebFrameTest, CanOverrideScaleLimits)
{
    UseMockScrollbarSettings mockScrollbarSettings;

    registerMockedHttpURLLoad("no_scale_for_you.html");

    FixedLayoutTestWebViewClient client;
    client.m_screenInfo.deviceScaleFactor = 1;
    int viewportWidth = 640;
    int viewportHeight = 480;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "no_scale_for_you.html", true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->setDefaultPageScaleLimits(0.25f, 5);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));

    EXPECT_EQ(2.0f, webViewHelper.webViewImpl()->minimumPageScaleFactor());
    EXPECT_EQ(2.0f, webViewHelper.webViewImpl()->maximumPageScaleFactor());

    webViewHelper.webView()->setIgnoreViewportTagScaleLimits(true);
    webViewHelper.webView()->layout();

    EXPECT_EQ(1.0f, webViewHelper.webViewImpl()->minimumPageScaleFactor());
    EXPECT_EQ(5.0f, webViewHelper.webViewImpl()->maximumPageScaleFactor());

    webViewHelper.webView()->setIgnoreViewportTagScaleLimits(false);
    webViewHelper.webView()->layout();

    EXPECT_EQ(2.0f, webViewHelper.webViewImpl()->minimumPageScaleFactor());
    EXPECT_EQ(2.0f, webViewHelper.webViewImpl()->maximumPageScaleFactor());
}

// Android doesn't have scrollbars on the main FrameView
#if OS(ANDROID)
TEST_F(WebFrameTest, DISABLED_updateOverlayScrollbarLayers)
#else
TEST_F(WebFrameTest, updateOverlayScrollbarLayers)
#endif
{
    UseMockScrollbarSettings mockScrollbarSettings;

    registerMockedHttpURLLoad("large-div.html");

    int viewWidth = 500;
    int viewHeight = 500;

    OwnPtr<FakeCompositingWebViewClient> fakeCompositingWebViewClient = adoptPtr(new FakeCompositingWebViewClient());
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initialize(true, 0, fakeCompositingWebViewClient.get(), &configueCompositingWebView);

    webViewHelper.webView()->resize(WebSize(viewWidth, viewHeight));
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "large-div.html");

    FrameView* view = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    EXPECT_TRUE(view->layoutView()->compositor()->layerForHorizontalScrollbar());
    EXPECT_TRUE(view->layoutView()->compositor()->layerForVerticalScrollbar());

    webViewHelper.webView()->resize(WebSize(viewWidth * 10, viewHeight * 10));
    webViewHelper.webView()->layout();
    EXPECT_FALSE(view->layoutView()->compositor()->layerForHorizontalScrollbar());
    EXPECT_FALSE(view->layoutView()->compositor()->layerForVerticalScrollbar());
}

void setScaleAndScrollAndLayout(WebViewImpl* webView, WebPoint scroll, float scale)
{
    webView->setPageScaleFactor(scale);
    webView->mainFrame()->setScrollOffset(WebSize(scroll.x, scroll.y));
    webView->layout();
}

void simulatePageScale(WebViewImpl* webViewImpl, float& scale)
{
    IntSize scrollDelta = webViewImpl->fakePageScaleAnimationTargetPositionForTesting() - webViewImpl->mainFrameImpl()->frameView()->scrollPosition();
    float scaleDelta = webViewImpl->fakePageScaleAnimationPageScaleForTesting() / webViewImpl->pageScaleFactor();
    webViewImpl->applyViewportDeltas(WebFloatSize(), FloatSize(scrollDelta), WebFloatSize(), scaleDelta, 0);
    scale = webViewImpl->pageScaleFactor();
}

void simulateMultiTargetZoom(WebViewImpl* webViewImpl, const WebRect& rect, float& scale)
{
    if (webViewImpl->zoomToMultipleTargetsRect(rect))
        simulatePageScale(webViewImpl, scale);
}

void simulateDoubleTap(WebViewImpl* webViewImpl, WebPoint& point, float& scale)
{
    webViewImpl->animateDoubleTapZoom(point);
    EXPECT_TRUE(webViewImpl->fakeDoubleTapAnimationPendingForTesting());
    simulatePageScale(webViewImpl, scale);
}

TEST_P(ParameterizedWebFrameTest, DivAutoZoomParamsTest)
{
    registerMockedHttpURLLoad("get_scale_for_auto_zoom_into_div_test.html");

    const float deviceScaleFactor = 2.0f;
    int viewportWidth = 640 / deviceScaleFactor;
    int viewportHeight = 1280 / deviceScaleFactor;
    float doubleTapZoomAlreadyLegibleRatio = 1.2f;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "get_scale_for_auto_zoom_into_div_test.html", false, 0, 0, configureAndroid);
    webViewHelper.webView()->setDeviceScaleFactor(deviceScaleFactor);
    webViewHelper.webView()->setDefaultPageScaleLimits(0.01f, 4);
    webViewHelper.webView()->setPageScaleFactor(0.5f);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    WebRect wideDiv(200, 100, 400, 150);
    WebRect tallDiv(200, 300, 400, 800);
    WebPoint doubleTapPointWide(wideDiv.x + 50, wideDiv.y + 50);
    WebPoint doubleTapPointTall(tallDiv.x + 50, tallDiv.y + 50);
    float scale;
    WebPoint scroll;

    float doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;

    // Test double-tap zooming into wide div.
    WebRect wideBlockBound = webViewHelper.webViewImpl()->computeBlockBound(doubleTapPointWide, false);
    webViewHelper.webViewImpl()->computeScaleAndScrollForBlockRect(WebPoint(doubleTapPointWide.x, doubleTapPointWide.y), wideBlockBound, touchPointPadding, doubleTapZoomAlreadyLegibleScale, scale, scroll);
    // The div should horizontally fill the screen (modulo margins), and
    // vertically centered (modulo integer rounding).
    EXPECT_NEAR(viewportWidth / (float) wideDiv.width, scale, 0.1);
    EXPECT_NEAR(wideDiv.x, scroll.x, 20);
    EXPECT_EQ(0, scroll.y);

    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), scroll, scale);

    // Test zoom out back to minimum scale.
    wideBlockBound = webViewHelper.webViewImpl()->computeBlockBound(doubleTapPointWide, false);
    webViewHelper.webViewImpl()->computeScaleAndScrollForBlockRect(WebPoint(doubleTapPointWide.x, doubleTapPointWide.y), wideBlockBound, touchPointPadding, doubleTapZoomAlreadyLegibleScale, scale, scroll);
    // FIXME: Looks like we are missing EXPECTs here.

    scale = webViewHelper.webViewImpl()->minimumPageScaleFactor();
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), scale);

    // Test double-tap zooming into tall div.
    WebRect tallBlockBound = webViewHelper.webViewImpl()->computeBlockBound(doubleTapPointTall, false);
    webViewHelper.webViewImpl()->computeScaleAndScrollForBlockRect(WebPoint(doubleTapPointTall.x, doubleTapPointTall.y), tallBlockBound, touchPointPadding, doubleTapZoomAlreadyLegibleScale, scale, scroll);
    // The div should start at the top left of the viewport.
    EXPECT_NEAR(viewportWidth / (float) tallDiv.width, scale, 0.1);
    EXPECT_NEAR(tallDiv.x, scroll.x, 20);
    EXPECT_NEAR(tallDiv.y, scroll.y, 20);
}

TEST_P(ParameterizedWebFrameTest, DivAutoZoomWideDivTest)
{
    registerMockedHttpURLLoad("get_wide_div_for_auto_zoom_test.html");

    const float deviceScaleFactor = 2.0f;
    int viewportWidth = 640 / deviceScaleFactor;
    int viewportHeight = 1280 / deviceScaleFactor;
    float doubleTapZoomAlreadyLegibleRatio = 1.2f;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "get_wide_div_for_auto_zoom_test.html", false, 0, 0, configureAndroid);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->setDeviceScaleFactor(deviceScaleFactor);
    webViewHelper.webView()->setPageScaleFactor(1.0f);
    webViewHelper.webView()->layout();

    webViewHelper.webViewImpl()->enableFakePageScaleAnimationForTesting(true);

    float doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;

    WebRect div(0, 100, viewportWidth, 150);
    WebPoint point(div.x + 50, div.y + 50);
    float scale;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);

    simulateDoubleTap(webViewHelper.webViewImpl(), point, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), point, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
}

TEST_P(ParameterizedWebFrameTest, DivAutoZoomVeryTallTest)
{
    // When a block is taller than the viewport and a zoom targets a lower part
    // of it, then we should keep the target point onscreen instead of snapping
    // back up the top of the block.
    registerMockedHttpURLLoad("very_tall_div.html");

    const float deviceScaleFactor = 2.0f;
    int viewportWidth = 640 / deviceScaleFactor;
    int viewportHeight = 1280 / deviceScaleFactor;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "very_tall_div.html", true, 0, 0, configureAndroid);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->setDeviceScaleFactor(deviceScaleFactor);
    webViewHelper.webView()->setPageScaleFactor(1.0f);
    webViewHelper.webView()->layout();

    WebRect div(200, 300, 400, 5000);
    WebPoint point(div.x + 50, div.y + 3000);
    float scale;
    WebPoint scroll;

    WebRect blockBound = webViewHelper.webViewImpl()->computeBlockBound(point, true);
    webViewHelper.webViewImpl()->computeScaleAndScrollForBlockRect(point, blockBound, 0, 1.0f, scale, scroll);
    EXPECT_EQ(scale, 1.0f);
    EXPECT_EQ(scroll.y, 2660);
}

TEST_F(WebFrameTest, DivAutoZoomMultipleDivsTest)
{
    registerMockedHttpURLLoad("get_multiple_divs_for_auto_zoom_test.html");

    const float deviceScaleFactor = 2.0f;
    int viewportWidth = 640 / deviceScaleFactor;
    int viewportHeight = 1280 / deviceScaleFactor;
    float doubleTapZoomAlreadyLegibleRatio = 1.2f;
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "get_multiple_divs_for_auto_zoom_test.html", false, 0, 0, configureAndroid);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->setDefaultPageScaleLimits(0.5f, 4);
    webViewHelper.webView()->setDeviceScaleFactor(deviceScaleFactor);
    webViewHelper.webView()->setPageScaleFactor(0.5f);
    webViewHelper.webView()->setMaximumLegibleScale(1.f);
    webViewHelper.webView()->layout();

    webViewHelper.webViewImpl()->enableFakePageScaleAnimationForTesting(true);

    WebRect topDiv(200, 100, 200, 150);
    WebRect bottomDiv(200, 300, 200, 150);
    WebPoint topPoint(topDiv.x + 50, topDiv.y + 50);
    WebPoint bottomPoint(bottomDiv.x + 50, bottomDiv.y + 50);
    float scale;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);

    // Test double tap on two different divs
    // After first zoom, we should go back to minimum page scale with a second double tap.
    simulateDoubleTap(webViewHelper.webViewImpl(), topPoint, scale);
    EXPECT_FLOAT_EQ(1, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), bottomPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);

    // If the user pinch zooms after double tap, a second double tap should zoom back to the div.
    simulateDoubleTap(webViewHelper.webViewImpl(), topPoint, scale);
    EXPECT_FLOAT_EQ(1, scale);
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 0.6f, 0);
    simulateDoubleTap(webViewHelper.webViewImpl(), bottomPoint, scale);
    EXPECT_FLOAT_EQ(1, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), bottomPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);

    // If we didn't yet get an auto-zoom update and a second double-tap arrives, should go back to minimum scale.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.1f, 0);
    webViewHelper.webViewImpl()->animateDoubleTapZoom(topPoint);
    EXPECT_TRUE(webViewHelper.webViewImpl()->fakeDoubleTapAnimationPendingForTesting());
    simulateDoubleTap(webViewHelper.webViewImpl(), bottomPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
}

TEST_F(WebFrameTest, DivAutoZoomScaleBoundsTest)
{
    registerMockedHttpURLLoad("get_scale_bounds_check_for_auto_zoom_test.html");

    int viewportWidth = 320;
    int viewportHeight = 480;
    float doubleTapZoomAlreadyLegibleRatio = 1.2f;
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "get_scale_bounds_check_for_auto_zoom_test.html", false, 0, 0, configureAndroid);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->setDeviceScaleFactor(1.5f);
    webViewHelper.webView()->setMaximumLegibleScale(1.f);
    webViewHelper.webView()->layout();

    webViewHelper.webViewImpl()->enableFakePageScaleAnimationForTesting(true);

    WebRect div(200, 100, 200, 150);
    WebPoint doubleTapPoint(div.x + 50, div.y + 50);
    float scale;

    // Test double tap scale bounds.
    // minimumPageScale < doubleTapZoomAlreadyLegibleScale < 1
    webViewHelper.webView()->setDefaultPageScaleLimits(0.5f, 4);
    webViewHelper.webView()->layout();
    float doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(1, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(1, scale);

    // Zoom in to reset double_tap_zoom_in_effect flag.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.1f, 0);
    // 1 < minimumPageScale < doubleTapZoomAlreadyLegibleScale
    webViewHelper.webView()->setDefaultPageScaleLimits(1.1f, 4);
    webViewHelper.webView()->layout();
    doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);

    // Zoom in to reset double_tap_zoom_in_effect flag.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.1f, 0);
    // minimumPageScale < 1 < doubleTapZoomAlreadyLegibleScale
    webViewHelper.webView()->setDefaultPageScaleLimits(0.95f, 4);
    webViewHelper.webView()->layout();
    doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);
}

TEST_F(WebFrameTest, DivAutoZoomScaleLegibleScaleTest)
{
    registerMockedHttpURLLoad("get_scale_bounds_check_for_auto_zoom_test.html");

    int viewportWidth = 320;
    int viewportHeight = 480;
    float doubleTapZoomAlreadyLegibleRatio = 1.2f;
    float maximumLegibleScaleFactor = 1.13f;
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "get_scale_bounds_check_for_auto_zoom_test.html", false, 0, 0, configureAndroid);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->setMaximumLegibleScale(maximumLegibleScaleFactor);
    webViewHelper.webView()->layout();

    webViewHelper.webViewImpl()->enableFakePageScaleAnimationForTesting(true);
    webViewHelper.webViewImpl()->page()->settings().setTextAutosizingEnabled(true);

    WebRect div(200, 100, 200, 150);
    WebPoint doubleTapPoint(div.x + 50, div.y + 50);
    float scale;

    // Test double tap scale bounds.
    // minimumPageScale < doubleTapZoomAlreadyLegibleScale < 1 < maximumLegibleScaleFactor
    float legibleScale = maximumLegibleScaleFactor;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    float doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    webViewHelper.webView()->setDefaultPageScaleLimits(0.5f, 4);
    webViewHelper.webView()->layout();
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(legibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(legibleScale, scale);

    // Zoom in to reset double_tap_zoom_in_effect flag.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.1f, 0);
    // 1 < maximumLegibleScaleFactor < minimumPageScale < doubleTapZoomAlreadyLegibleScale
    webViewHelper.webView()->setDefaultPageScaleLimits(1.0f, 4);
    webViewHelper.webView()->layout();
    doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);

    // Zoom in to reset double_tap_zoom_in_effect flag.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.1f, 0);
    // minimumPageScale < 1 < maximumLegibleScaleFactor < doubleTapZoomAlreadyLegibleScale
    webViewHelper.webView()->setDefaultPageScaleLimits(0.95f, 4);
    webViewHelper.webView()->layout();
    doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);

    // Zoom in to reset double_tap_zoom_in_effect flag.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.1f, 0);
    // minimumPageScale < 1 < doubleTapZoomAlreadyLegibleScale < maximumLegibleScaleFactor
    webViewHelper.webView()->setDefaultPageScaleLimits(0.9f, 4);
    webViewHelper.webView()->layout();
    doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(legibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(legibleScale, scale);
}


TEST_F(WebFrameTest, DivAutoZoomScaleFontScaleFactorTest)
{
    registerMockedHttpURLLoad("get_scale_bounds_check_for_auto_zoom_test.html");

    int viewportWidth = 320;
    int viewportHeight = 480;
    float doubleTapZoomAlreadyLegibleRatio = 1.2f;
    float accessibilityFontScaleFactor = 1.13f;
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "get_scale_bounds_check_for_auto_zoom_test.html", false, 0, 0, configureAndroid);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->setMaximumLegibleScale(1.f);
    webViewHelper.webView()->layout();

    webViewHelper.webViewImpl()->enableFakePageScaleAnimationForTesting(true);
    webViewHelper.webViewImpl()->page()->settings().setTextAutosizingEnabled(true);
    webViewHelper.webViewImpl()->page()->settings().setAccessibilityFontScaleFactor(accessibilityFontScaleFactor);

    WebRect div(200, 100, 200, 150);
    WebPoint doubleTapPoint(div.x + 50, div.y + 50);
    float scale;

    // Test double tap scale bounds.
    // minimumPageScale < doubleTapZoomAlreadyLegibleScale < 1 < accessibilityFontScaleFactor
    float legibleScale = accessibilityFontScaleFactor;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    float doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    webViewHelper.webView()->setDefaultPageScaleLimits(0.5f, 4);
    webViewHelper.webView()->layout();
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(legibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(legibleScale, scale);

    // Zoom in to reset double_tap_zoom_in_effect flag.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.1f, 0);
    // 1 < accessibilityFontScaleFactor < minimumPageScale < doubleTapZoomAlreadyLegibleScale
    webViewHelper.webView()->setDefaultPageScaleLimits(1.0f, 4);
    webViewHelper.webView()->layout();
    doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);

    // Zoom in to reset double_tap_zoom_in_effect flag.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.1f, 0);
    // minimumPageScale < 1 < accessibilityFontScaleFactor < doubleTapZoomAlreadyLegibleScale
    webViewHelper.webView()->setDefaultPageScaleLimits(0.95f, 4);
    webViewHelper.webView()->layout();
    doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(doubleTapZoomAlreadyLegibleScale, scale);

    // Zoom in to reset double_tap_zoom_in_effect flag.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.1f, 0);
    // minimumPageScale < 1 < doubleTapZoomAlreadyLegibleScale < accessibilityFontScaleFactor
    webViewHelper.webView()->setDefaultPageScaleLimits(0.9f, 4);
    webViewHelper.webView()->layout();
    doubleTapZoomAlreadyLegibleScale = webViewHelper.webViewImpl()->minimumPageScaleFactor() * doubleTapZoomAlreadyLegibleRatio;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(legibleScale, scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(webViewHelper.webViewImpl()->minimumPageScaleFactor(), scale);
    simulateDoubleTap(webViewHelper.webViewImpl(), doubleTapPoint, scale);
    EXPECT_FLOAT_EQ(legibleScale, scale);
}

TEST_P(ParameterizedWebFrameTest, BlockBoundTest)
{
    registerMockedHttpURLLoad("block_bound.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "block_bound.html", false, 0, 0, configureAndroid);

    IntRect rectBack = IntRect(0, 0, 200, 200);
    IntRect rectLeftTop = IntRect(10, 10, 80, 80);
    IntRect rectRightBottom = IntRect(110, 110, 80, 80);
    IntRect blockBound;

    blockBound = IntRect(webViewHelper.webViewImpl()->computeBlockBound(WebPoint(9, 9), true));
    EXPECT_RECT_EQ(rectBack, blockBound);

    blockBound = IntRect(webViewHelper.webViewImpl()->computeBlockBound(WebPoint(10, 10), true));
    EXPECT_RECT_EQ(rectLeftTop, blockBound);

    blockBound = IntRect(webViewHelper.webViewImpl()->computeBlockBound(WebPoint(50, 50), true));
    EXPECT_RECT_EQ(rectLeftTop, blockBound);

    blockBound = IntRect(webViewHelper.webViewImpl()->computeBlockBound(WebPoint(89, 89), true));
    EXPECT_RECT_EQ(rectLeftTop, blockBound);

    blockBound = IntRect(webViewHelper.webViewImpl()->computeBlockBound(WebPoint(90, 90), true));
    EXPECT_RECT_EQ(rectBack, blockBound);

    blockBound = IntRect(webViewHelper.webViewImpl()->computeBlockBound(WebPoint(109, 109), true));
    EXPECT_RECT_EQ(rectBack, blockBound);

    blockBound = IntRect(webViewHelper.webViewImpl()->computeBlockBound(WebPoint(110, 110), true));
    EXPECT_RECT_EQ(rectRightBottom, blockBound);
}

TEST_P(ParameterizedWebFrameTest, DivMultipleTargetZoomMultipleDivsTest)
{
    registerMockedHttpURLLoad("get_multiple_divs_for_auto_zoom_test.html");

    const float deviceScaleFactor = 2.0f;
    int viewportWidth = 640 / deviceScaleFactor;
    int viewportHeight = 1280 / deviceScaleFactor;
    float doubleTapZoomAlreadyLegibleRatio = 1.2f;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "get_multiple_divs_for_auto_zoom_test.html");
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->setDefaultPageScaleLimits(0.5f, 4);
    webViewHelper.webView()->setDeviceScaleFactor(deviceScaleFactor);
    webViewHelper.webView()->setPageScaleFactor(0.5f);
    webViewHelper.webView()->setMaximumLegibleScale(1.f);
    webViewHelper.webView()->layout();

    webViewHelper.webViewImpl()->enableFakePageScaleAnimationForTesting(true);

    WebRect viewportRect(0, 0, viewportWidth, viewportHeight);
    WebRect topDiv(200, 100, 200, 150);
    WebRect bottomDiv(200, 300, 200, 150);
    float scale;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), (webViewHelper.webViewImpl()->minimumPageScaleFactor()) * (1 + doubleTapZoomAlreadyLegibleRatio) / 2);

    simulateMultiTargetZoom(webViewHelper.webViewImpl(), topDiv, scale);
    EXPECT_FLOAT_EQ(1, scale);
    simulateMultiTargetZoom(webViewHelper.webViewImpl(), bottomDiv, scale);
    EXPECT_FLOAT_EQ(1, scale);
    simulateMultiTargetZoom(webViewHelper.webViewImpl(), viewportRect, scale);
    EXPECT_FLOAT_EQ(1, scale);
    webViewHelper.webViewImpl()->setPageScaleFactor(webViewHelper.webViewImpl()->minimumPageScaleFactor());
    simulateMultiTargetZoom(webViewHelper.webViewImpl(), topDiv, scale);
    EXPECT_FLOAT_EQ(1, scale);
}

TEST_F(WebFrameTest, DivScrollIntoEditableTest)
{
    registerMockedHttpURLLoad("get_scale_for_zoom_into_editable_test.html");

    const bool autoZoomToLegibleScale = true;
    int viewportWidth = 450;
    int viewportHeight = 300;
    float leftBoxRatio = 0.3f;
    int caretPadding = 10;
    float minReadableCaretHeight = 16.0f;
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "get_scale_for_zoom_into_editable_test.html");
    webViewHelper.webViewImpl()->page()->settings().setTextAutosizingEnabled(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->setDefaultPageScaleLimits(0.25f, 4);

    webViewHelper.webViewImpl()->enableFakePageScaleAnimationForTesting(true);

    WebRect editBoxWithText(200, 200, 250, 20);
    WebRect editBoxWithNoText(200, 250, 250, 20);

    // Test scrolling the focused node
    // The edit box is shorter and narrower than the viewport when legible.
    webViewHelper.webView()->advanceFocus(false);
    // Set the caret to the end of the input box.
    webViewHelper.webView()->mainFrame()->document().getElementById("EditBoxWithText").to<WebInputElement>().setSelectionRange(1000, 1000);
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), 1);
    WebRect rect, caret;
    webViewHelper.webViewImpl()->selectionBounds(caret, rect);

    // Set the page scale to be smaller than the minimal readable scale.
    float initialScale = minReadableCaretHeight / caret.height * 0.5f;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), initialScale);

    float scale;
    IntPoint scroll;
    bool needAnimation;
    webViewHelper.webViewImpl()->computeScaleAndScrollForFocusedNode(webViewHelper.webViewImpl()->focusedElement(), autoZoomToLegibleScale, scale, scroll, needAnimation);
    EXPECT_TRUE(needAnimation);
    // The edit box should be left aligned with a margin for possible label.
    int hScroll = editBoxWithText.x - leftBoxRatio * viewportWidth / scale;
    EXPECT_NEAR(hScroll, scroll.x(), 2);
    int vScroll = editBoxWithText.y - (viewportHeight / scale - editBoxWithText.height) / 2;
    EXPECT_NEAR(vScroll, scroll.y(), 2);
    EXPECT_NEAR(minReadableCaretHeight / caret.height, scale, 0.1);

    // The edit box is wider than the viewport when legible.
    viewportWidth = 200;
    viewportHeight = 150;
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), initialScale);
    webViewHelper.webViewImpl()->computeScaleAndScrollForFocusedNode(webViewHelper.webViewImpl()->focusedElement(), autoZoomToLegibleScale, scale, scroll, needAnimation);
    EXPECT_TRUE(needAnimation);
    // The caret should be right aligned since the caret would be offscreen when the edit box is left aligned.
    hScroll = caret.x + caret.width + caretPadding - viewportWidth / scale;
    EXPECT_NEAR(hScroll, scroll.x(), 2);
    EXPECT_NEAR(minReadableCaretHeight / caret.height, scale, 0.1);

    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), initialScale);
    // Move focus to edit box with text.
    webViewHelper.webView()->advanceFocus(false);
    webViewHelper.webViewImpl()->computeScaleAndScrollForFocusedNode(webViewHelper.webViewImpl()->focusedElement(), autoZoomToLegibleScale, scale, scroll, needAnimation);
    EXPECT_TRUE(needAnimation);
    // The edit box should be left aligned.
    hScroll = editBoxWithNoText.x;
    EXPECT_NEAR(hScroll, scroll.x(), 2);
    vScroll = editBoxWithNoText.y - (viewportHeight / scale - editBoxWithNoText.height) / 2;
    EXPECT_NEAR(vScroll, scroll.y(), 2);
    EXPECT_NEAR(minReadableCaretHeight / caret.height, scale, 0.1);

    // Move focus back to the first edit box.
    webViewHelper.webView()->advanceFocus(true);
    // Zoom out slightly.
    const float withinToleranceScale = scale * 0.9f;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), scroll, withinToleranceScale);
    // Move focus back to the second edit box.
    webViewHelper.webView()->advanceFocus(false);
    webViewHelper.webViewImpl()->computeScaleAndScrollForFocusedNode(webViewHelper.webViewImpl()->focusedElement(), autoZoomToLegibleScale, scale, scroll, needAnimation);
    // The scale should not be adjusted as the zoomed out scale was sufficiently close to the previously focused scale.
    EXPECT_FALSE(needAnimation);
}

TEST_F(WebFrameTest, DivScrollIntoEditablePreservePageScaleTest)
{
    registerMockedHttpURLLoad("get_scale_for_zoom_into_editable_test.html");

    const bool autoZoomToLegibleScale = true;
    const int viewportWidth = 450;
    const int viewportHeight = 300;
    const float minReadableCaretHeight = 16.0f;
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "get_scale_for_zoom_into_editable_test.html");
    webViewHelper.webViewImpl()->page()->settings().setTextAutosizingEnabled(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webViewImpl()->enableFakePageScaleAnimationForTesting(true);

    const WebRect editBoxWithText(200, 200, 250, 20);

    webViewHelper.webView()->advanceFocus(false);
    // Set the caret to the begining of the input box.
    webViewHelper.webView()->mainFrame()->document().getElementById("EditBoxWithText").to<WebInputElement>().setSelectionRange(0, 0);
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), 1);
    WebRect rect, caret;
    webViewHelper.webViewImpl()->selectionBounds(caret, rect);

    // Set the page scale to be twice as large as the minimal readable scale.
    float newScale = minReadableCaretHeight / caret.height * 2.0;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), newScale);

    float scale;
    IntPoint scroll;
    bool needAnimation;
    webViewHelper.webViewImpl()->computeScaleAndScrollForFocusedNode(webViewHelper.webViewImpl()->focusedElement(), autoZoomToLegibleScale, scale, scroll, needAnimation);
    EXPECT_TRUE(needAnimation);
    // Edit box and caret should be left alinged
    int hScroll = editBoxWithText.x;
    EXPECT_NEAR(hScroll, scroll.x(), 1);
    int vScroll = editBoxWithText.y - (viewportHeight / scale - editBoxWithText.height) / 2;
    EXPECT_NEAR(vScroll, scroll.y(), 1);
    // Page scale have to be unchanged
    EXPECT_EQ(newScale, scale);

    // Set page scale and scroll such that edit box will be under the screen
    newScale = 3.0;
    hScroll = 200;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(hScroll, 0), newScale);
    webViewHelper.webViewImpl()->computeScaleAndScrollForFocusedNode(webViewHelper.webViewImpl()->focusedElement(), autoZoomToLegibleScale, scale, scroll, needAnimation);
    EXPECT_TRUE(needAnimation);
    // Horizontal scroll have to be the same
    EXPECT_NEAR(hScroll, scroll.x(), 1);
    vScroll = editBoxWithText.y - (viewportHeight / scale - editBoxWithText.height) / 2;
    EXPECT_NEAR(vScroll, scroll.y(), 1);
    // Page scale have to be unchanged
    EXPECT_EQ(newScale, scale);
}

// Tests the scroll into view functionality when autoZoomeFocusedNodeToLegibleScale set
// to false. i.e. The path non-Android platforms take.
TEST_F(WebFrameTest, DivScrollIntoEditableTestZoomToLegibleScaleDisabled)
{
    registerMockedHttpURLLoad("get_scale_for_zoom_into_editable_test.html");

    const bool autoZoomToLegibleScale = false;
    int viewportWidth = 100;
    int viewportHeight = 100;
    float leftBoxRatio = 0.3f;
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "get_scale_for_zoom_into_editable_test.html");
    webViewHelper.webViewImpl()->page()->settings().setTextAutosizingEnabled(false);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->setDefaultPageScaleLimits(0.25f, 4);

    webViewHelper.webViewImpl()->enableFakePageScaleAnimationForTesting(true);

    WebRect editBoxWithText(200, 200, 250, 20);
    WebRect editBoxWithNoText(200, 250, 250, 20);

    // Test scrolling the focused node
    // Since we're zoomed out, the caret is considered too small to be legible and so we'd
    // normally zoom in. Make sure we don't change scale since the auto-zoom setting is off.

    // Focus the second empty textbox.
    webViewHelper.webView()->advanceFocus(false);
    webViewHelper.webView()->advanceFocus(false);

    // Set the page scale to be smaller than the minimal readable scale.
    float initialScale = 0.25f;
    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), WebPoint(0, 0), initialScale);

    float scale;
    IntPoint scroll;
    bool needAnimation;
    webViewHelper.webViewImpl()->computeScaleAndScrollForFocusedNode(webViewHelper.webViewImpl()->focusedElement(), autoZoomToLegibleScale, scale, scroll, needAnimation);

    // There should be no change in page scale.
    EXPECT_EQ(initialScale, scale);
    // The edit box should be left aligned with a margin for possible label.
    EXPECT_TRUE(needAnimation);
    int hScroll = editBoxWithNoText.x - leftBoxRatio * viewportWidth / scale;
    EXPECT_NEAR(hScroll, scroll.x(), 2);
    int vScroll = editBoxWithNoText.y - (viewportHeight / scale - editBoxWithNoText.height) / 2;
    EXPECT_NEAR(vScroll, scroll.y(), 2);

    setScaleAndScrollAndLayout(webViewHelper.webViewImpl(), scroll, scale);

    // Select the first textbox.
    webViewHelper.webView()->advanceFocus(true);
    WebRect rect, caret;
    webViewHelper.webViewImpl()->selectionBounds(caret, rect);
    webViewHelper.webViewImpl()->computeScaleAndScrollForFocusedNode(webViewHelper.webViewImpl()->focusedElement(), autoZoomToLegibleScale, scale, scroll, needAnimation);

    // There should be no change at all since the textbox is fully visible already.
    EXPECT_EQ(initialScale, scale);
    EXPECT_FALSE(needAnimation);
}

TEST_P(ParameterizedWebFrameTest, CharacterIndexAtPointWithPinchZoom)
{
    registerMockedHttpURLLoad("sometext.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "sometext.html");
    webViewHelper.webViewImpl()->resize(WebSize(640, 480));
    webViewHelper.webViewImpl()->layout();


    webViewHelper.webViewImpl()->setPageScaleFactor(2);
    webViewHelper.webViewImpl()->setPinchViewportOffset(WebFloatPoint(50, 60));

    WebRect baseRect;
    WebRect extentRect;

    WebFrame* mainFrame = webViewHelper.webViewImpl()->mainFrame();
    size_t ix = mainFrame->characterIndexForPoint(WebPoint(320, 388));

    EXPECT_EQ(2ul, ix);
}

TEST_P(ParameterizedWebFrameTest, FirstRectForCharacterRangeWithPinchZoom)
{
    registerMockedHttpURLLoad("textbox.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "textbox.html", true);
    webViewHelper.webViewImpl()->resize(WebSize(640, 480));

    WebFrame* mainFrame = webViewHelper.webViewImpl()->mainFrame();
    mainFrame->executeScript(WebScriptSource("selectRange();"));

    WebRect oldRect;
    mainFrame->firstRectForCharacterRange(0, 5, oldRect);

    WebFloatPoint pinchOffset(100, 130);
    float scale = 2;
    webViewHelper.webViewImpl()->setPageScaleFactor(scale);
    webViewHelper.webViewImpl()->setPinchViewportOffset(pinchOffset);

    WebRect baseRect;
    WebRect extentRect;

    WebRect rect;
    mainFrame->firstRectForCharacterRange(0, 5, rect);

    EXPECT_EQ((oldRect.x - pinchOffset.x) * scale, rect.x);
    EXPECT_EQ((oldRect.y - pinchOffset.y) * scale, rect.y);
    EXPECT_EQ(oldRect.width*scale, rect.width);
    EXPECT_EQ(oldRect.height*scale, rect.height);
}
class TestReloadDoesntRedirectWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    WebNavigationPolicy decidePolicyForNavigation(const NavigationPolicyInfo& info) override
    {
        EXPECT_FALSE(info.isRedirect);
        return WebNavigationPolicyCurrentTab;
    }
};

TEST_P(ParameterizedWebFrameTest, ReloadDoesntSetRedirect)
{
    // Test for case in http://crbug.com/73104. Reloading a frame very quickly
    // would sometimes call decidePolicyForNavigation with isRedirect=true
    registerMockedHttpURLLoad("form.html");

    TestReloadDoesntRedirectWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "form.html", false, &webFrameClient);

    webViewHelper.webView()->mainFrame()->reload(true);
    // start another reload before request is delivered.
    FrameTestHelpers::reloadFrameIgnoringCache(webViewHelper.webView()->mainFrame());
}

class ReloadWithOverrideURLTask : public WebThread::Task {
public:
    ReloadWithOverrideURLTask(WebFrame* frame, const KURL& url, bool ignoreCache)
        : m_frame(frame), m_url(url), m_ignoreCache(ignoreCache)
    {
    }

    void run() override
    {
        m_frame->reloadWithOverrideURL(m_url, m_ignoreCache);
    }

private:
    WebFrame* const m_frame;
    const KURL m_url;
    const bool m_ignoreCache;
};

class ClearScrollStateOnCommitWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    void didCommitProvisionalLoad(WebLocalFrame* frame, const WebHistoryItem&, WebHistoryCommitType) override
    {
        frame->view()->resetScrollAndScaleState();
    }
};

TEST_F(WebFrameTest, ReloadWithOverrideURLPreservesState)
{
    const std::string firstURL = "200-by-300.html";
    const std::string secondURL = "content-width-1000.html";
    const std::string thirdURL = "very_tall_div.html";
    const float pageScaleFactor = 1.1684f;
    const int pageWidth = 120;
    const int pageHeight = 100;

    registerMockedHttpURLLoad(firstURL);
    registerMockedHttpURLLoad(secondURL);
    registerMockedHttpURLLoad(thirdURL);

    FrameTestHelpers::WebViewHelper webViewHelper;
    ClearScrollStateOnCommitWebFrameClient client;
    webViewHelper.initializeAndLoad(m_baseURL + firstURL, true, &client);
    webViewHelper.webViewImpl()->resize(WebSize(pageWidth, pageHeight));
    webViewHelper.webViewImpl()->mainFrame()->setScrollOffset(WebSize(pageWidth / 4, pageHeight / 4));
    webViewHelper.webViewImpl()->setPageScaleFactor(pageScaleFactor);

    WebSize previousOffset = webViewHelper.webViewImpl()->mainFrame()->scrollOffset();
    float previousScale = webViewHelper.webViewImpl()->pageScaleFactor();

    // Reload the page and end up at the same url. State should be propagated.
    Platform::current()->currentThread()->postTask(
        FROM_HERE, new ReloadWithOverrideURLTask(webViewHelper.webViewImpl()->mainFrame(), toKURL(m_baseURL + firstURL), false));
    FrameTestHelpers::pumpPendingRequestsDoNotUse(webViewHelper.webViewImpl()->mainFrame());
    EXPECT_EQ(previousOffset.width, webViewHelper.webViewImpl()->mainFrame()->scrollOffset().width);
    EXPECT_EQ(previousOffset.height, webViewHelper.webViewImpl()->mainFrame()->scrollOffset().height);
    EXPECT_EQ(previousScale, webViewHelper.webViewImpl()->pageScaleFactor());

    // Reload the page using the cache. State should not be propagated.
    Platform::current()->currentThread()->postTask(
        FROM_HERE, new ReloadWithOverrideURLTask(webViewHelper.webViewImpl()->mainFrame(), toKURL(m_baseURL + secondURL), false));
    FrameTestHelpers::pumpPendingRequestsDoNotUse(webViewHelper.webViewImpl()->mainFrame());
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrame()->scrollOffset().width);
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrame()->scrollOffset().height);
    EXPECT_EQ(1.0f, webViewHelper.webViewImpl()->pageScaleFactor());

    // Reload the page while ignoring the cache. State should not be propagated.
    Platform::current()->currentThread()->postTask(
        FROM_HERE, new ReloadWithOverrideURLTask(webViewHelper.webViewImpl()->mainFrame(), toKURL(m_baseURL + thirdURL), true));
    FrameTestHelpers::pumpPendingRequestsDoNotUse(webViewHelper.webViewImpl()->mainFrame());
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrame()->scrollOffset().width);
    EXPECT_EQ(0, webViewHelper.webViewImpl()->mainFrame()->scrollOffset().height);
    EXPECT_EQ(1.0f, webViewHelper.webViewImpl()->pageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, ReloadWhileProvisional)
{
    // Test that reloading while the previous load is still pending does not cause the initial
    // request to get lost.
    registerMockedHttpURLLoad("fixed_layout.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize();
    WebURLRequest request;
    request.initialize();
    request.setURL(toKURL(m_baseURL + "fixed_layout.html"));
    webViewHelper.webView()->mainFrame()->loadRequest(request);
    // start reload before first request is delivered.
    FrameTestHelpers::reloadFrameIgnoringCache(webViewHelper.webView()->mainFrame());

    WebDataSource* dataSource = webViewHelper.webView()->mainFrame()->dataSource();
    ASSERT_TRUE(dataSource);
    EXPECT_EQ(toKURL(m_baseURL + "fixed_layout.html"), toKURL(dataSource->request().url().spec()));
}

TEST_P(ParameterizedWebFrameTest, AppendRedirects)
{
    const std::string firstURL = "about:blank";
    const std::string secondURL = "http://internal.test";

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(firstURL, true);

    WebDataSource* dataSource = webViewHelper.webView()->mainFrame()->dataSource();
    ASSERT_TRUE(dataSource);
    dataSource->appendRedirect(toKURL(secondURL));

    WebVector<WebURL> redirects;
    dataSource->redirectChain(redirects);
    ASSERT_EQ(2U, redirects.size());
    EXPECT_EQ(toKURL(firstURL), toKURL(redirects[0].spec().data()));
    EXPECT_EQ(toKURL(secondURL), toKURL(redirects[1].spec().data()));
}

TEST_P(ParameterizedWebFrameTest, IframeRedirect)
{
    registerMockedHttpURLLoad("iframe_redirect.html");
    registerMockedHttpURLLoad("visible_iframe.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "iframe_redirect.html", true);
    // Pump pending requests one more time. The test page loads script that navigates.
    FrameTestHelpers::pumpPendingRequestsDoNotUse(webViewHelper.webView()->mainFrame());

    WebFrame* iframe = webViewHelper.webView()->findFrameByName(WebString::fromUTF8("ifr"));
    ASSERT_TRUE(iframe);
    WebDataSource* iframeDataSource = iframe->dataSource();
    ASSERT_TRUE(iframeDataSource);
    WebVector<WebURL> redirects;
    iframeDataSource->redirectChain(redirects);
    ASSERT_EQ(2U, redirects.size());
    EXPECT_EQ(toKURL("about:blank"), toKURL(redirects[0].spec().data()));
    EXPECT_EQ(toKURL("http://internal.test/visible_iframe.html"), toKURL(redirects[1].spec().data()));
}

TEST_P(ParameterizedWebFrameTest, ClearFocusedNodeTest)
{
    registerMockedHttpURLLoad("iframe_clear_focused_node_test.html");
    registerMockedHttpURLLoad("autofocus_input_field_iframe.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "iframe_clear_focused_node_test.html", true);

    // Clear the focused node.
    webViewHelper.webView()->clearFocusedElement();

    // Now retrieve the FocusedNode and test it should be null.
    EXPECT_EQ(0, webViewHelper.webViewImpl()->focusedElement());
}

// Implementation of WebFrameClient that tracks the v8 contexts that are created
// and destroyed for verification.
class ContextLifetimeTestWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    struct Notification {
    public:
        Notification(WebLocalFrame* frame, v8::Local<v8::Context> context, int worldId)
            : frame(frame)
            , context(context->GetIsolate(), context)
            , worldId(worldId)
        {
        }

        ~Notification()
        {
            context.Reset();
        }

        bool Equals(Notification* other)
        {
            return other && frame == other->frame && context == other->context && worldId == other->worldId;
        }

        WebLocalFrame* frame;
        v8::Persistent<v8::Context> context;
        int worldId;
    };

    ~ContextLifetimeTestWebFrameClient() override
    {
        reset();
    }

    void reset()
    {
        for (size_t i = 0; i < createNotifications.size(); ++i)
            delete createNotifications[i];

        for (size_t i = 0; i < releaseNotifications.size(); ++i)
            delete releaseNotifications[i];

        createNotifications.clear();
        releaseNotifications.clear();
    }

    std::vector<Notification*> createNotifications;
    std::vector<Notification*> releaseNotifications;

 private:
    void didCreateScriptContext(WebLocalFrame* frame, v8::Local<v8::Context> context, int extensionGroup, int worldId) override
    {
        createNotifications.push_back(new Notification(frame, context, worldId));
    }

    void willReleaseScriptContext(WebLocalFrame* frame, v8::Local<v8::Context> context, int worldId) override
    {
        releaseNotifications.push_back(new Notification(frame, context, worldId));
    }
};

// TODO(aa): Deflake this test.
TEST_P(ParameterizedWebFrameTest, FLAKY_ContextNotificationsLoadUnload)
{
    v8::HandleScope handleScope(v8::Isolate::GetCurrent());

    registerMockedHttpURLLoad("context_notifications_test.html");
    registerMockedHttpURLLoad("context_notifications_test_frame.html");

    // Load a frame with an iframe, make sure we get the right create notifications.
    ContextLifetimeTestWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "context_notifications_test.html", true, &webFrameClient);

    WebFrame* mainFrame = webViewHelper.webView()->mainFrame();
    WebFrame* childFrame = mainFrame->firstChild();

    ASSERT_EQ(2u, webFrameClient.createNotifications.size());
    EXPECT_EQ(0u, webFrameClient.releaseNotifications.size());

    ContextLifetimeTestWebFrameClient::Notification* firstCreateNotification = webFrameClient.createNotifications[0];
    ContextLifetimeTestWebFrameClient::Notification* secondCreateNotification = webFrameClient.createNotifications[1];

    EXPECT_EQ(mainFrame, firstCreateNotification->frame);
    EXPECT_EQ(mainFrame->mainWorldScriptContext(), firstCreateNotification->context);
    EXPECT_EQ(0, firstCreateNotification->worldId);

    EXPECT_EQ(childFrame, secondCreateNotification->frame);
    EXPECT_EQ(childFrame->mainWorldScriptContext(), secondCreateNotification->context);
    EXPECT_EQ(0, secondCreateNotification->worldId);

    // Close the view. We should get two release notifications that are exactly the same as the create ones, in reverse order.
    webViewHelper.reset();

    ASSERT_EQ(2u, webFrameClient.releaseNotifications.size());
    ContextLifetimeTestWebFrameClient::Notification* firstReleaseNotification = webFrameClient.releaseNotifications[0];
    ContextLifetimeTestWebFrameClient::Notification* secondReleaseNotification = webFrameClient.releaseNotifications[1];

    ASSERT_TRUE(firstCreateNotification->Equals(secondReleaseNotification));
    ASSERT_TRUE(secondCreateNotification->Equals(firstReleaseNotification));
}

TEST_P(ParameterizedWebFrameTest, ContextNotificationsReload)
{
    v8::HandleScope handleScope(v8::Isolate::GetCurrent());

    registerMockedHttpURLLoad("context_notifications_test.html");
    registerMockedHttpURLLoad("context_notifications_test_frame.html");

    ContextLifetimeTestWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "context_notifications_test.html", true, &webFrameClient);

    // Refresh, we should get two release notifications and two more create notifications.
    FrameTestHelpers::reloadFrame(webViewHelper.webView()->mainFrame());
    ASSERT_EQ(4u, webFrameClient.createNotifications.size());
    ASSERT_EQ(2u, webFrameClient.releaseNotifications.size());

    // The two release notifications we got should be exactly the same as the first two create notifications.
    for (size_t i = 0; i < webFrameClient.releaseNotifications.size(); ++i) {
      EXPECT_TRUE(webFrameClient.releaseNotifications[i]->Equals(
          webFrameClient.createNotifications[webFrameClient.createNotifications.size() - 3 - i]));
    }

    // The last two create notifications should be for the current frames and context.
    WebFrame* mainFrame = webViewHelper.webView()->mainFrame();
    WebFrame* childFrame = mainFrame->firstChild();
    ContextLifetimeTestWebFrameClient::Notification* firstRefreshNotification = webFrameClient.createNotifications[2];
    ContextLifetimeTestWebFrameClient::Notification* secondRefreshNotification = webFrameClient.createNotifications[3];

    EXPECT_EQ(mainFrame, firstRefreshNotification->frame);
    EXPECT_EQ(mainFrame->mainWorldScriptContext(), firstRefreshNotification->context);
    EXPECT_EQ(0, firstRefreshNotification->worldId);

    EXPECT_EQ(childFrame, secondRefreshNotification->frame);
    EXPECT_EQ(childFrame->mainWorldScriptContext(), secondRefreshNotification->context);
    EXPECT_EQ(0, secondRefreshNotification->worldId);
}

TEST_P(ParameterizedWebFrameTest, ContextNotificationsIsolatedWorlds)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope handleScope(isolate);

    registerMockedHttpURLLoad("context_notifications_test.html");
    registerMockedHttpURLLoad("context_notifications_test_frame.html");

    ContextLifetimeTestWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "context_notifications_test.html", true, &webFrameClient);

    // Add an isolated world.
    webFrameClient.reset();

    int isolatedWorldId = 42;
    WebScriptSource scriptSource("hi!");
    int numSources = 1;
    int extensionGroup = 0;
    webViewHelper.webView()->mainFrame()->executeScriptInIsolatedWorld(isolatedWorldId, &scriptSource, numSources, extensionGroup);

    // We should now have a new create notification.
    ASSERT_EQ(1u, webFrameClient.createNotifications.size());
    ContextLifetimeTestWebFrameClient::Notification* notification = webFrameClient.createNotifications[0];
    ASSERT_EQ(isolatedWorldId, notification->worldId);
    ASSERT_EQ(webViewHelper.webView()->mainFrame(), notification->frame);

    // We don't have an API to enumarate isolated worlds for a frame, but we can at least assert that the context we got is *not* the main world's context.
    ASSERT_NE(webViewHelper.webView()->mainFrame()->mainWorldScriptContext(), v8::Local<v8::Context>::New(isolate, notification->context));

    webViewHelper.reset();

    // We should have gotten three release notifications (one for each of the frames, plus one for the isolated context).
    ASSERT_EQ(3u, webFrameClient.releaseNotifications.size());

    // And one of them should be exactly the same as the create notification for the isolated context.
    int matchCount = 0;
    for (size_t i = 0; i < webFrameClient.releaseNotifications.size(); ++i) {
      if (webFrameClient.releaseNotifications[i]->Equals(webFrameClient.createNotifications[0]))
        ++matchCount;
    }
    EXPECT_EQ(1, matchCount);
}

TEST_P(ParameterizedWebFrameTest, FindInPage)
{
    registerMockedHttpURLLoad("find.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "find.html");
    WebFrame* frame = webViewHelper.webView()->mainFrame();
    const int findIdentifier = 12345;
    WebFindOptions options;

    // Find in a <div> element.
    EXPECT_TRUE(frame->find(findIdentifier, WebString::fromUTF8("bar1"), options, false, 0));
    frame->stopFinding(false);
    WebRange range = frame->selectionRange();
    EXPECT_EQ(5, range.startOffset());
    EXPECT_EQ(9, range.endOffset());
    EXPECT_TRUE(frame->document().focusedElement().isNull());

    // Find in an <input> value.
    EXPECT_TRUE(frame->find(findIdentifier, WebString::fromUTF8("bar2"), options, false, 0));
    // Confirm stopFinding(false) sets the selection on the found text.
    frame->stopFinding(false);
    range = frame->selectionRange();
    ASSERT_FALSE(range.isNull());
    EXPECT_EQ(5, range.startOffset());
    EXPECT_EQ(9, range.endOffset());
    EXPECT_EQ(WebString::fromUTF8("INPUT"), frame->document().focusedElement().tagName());

    // Find in a <textarea> content.
    EXPECT_TRUE(frame->find(findIdentifier, WebString::fromUTF8("bar3"), options, false, 0));
    // Confirm stopFinding(false) sets the selection on the found text.
    frame->stopFinding(false);
    range = frame->selectionRange();
    ASSERT_FALSE(range.isNull());
    EXPECT_EQ(5, range.startOffset());
    EXPECT_EQ(9, range.endOffset());
    EXPECT_EQ(WebString::fromUTF8("TEXTAREA"), frame->document().focusedElement().tagName());

    // Find in a contentEditable element.
    EXPECT_TRUE(frame->find(findIdentifier, WebString::fromUTF8("bar4"), options, false, 0));
    // Confirm stopFinding(false) sets the selection on the found text.
    frame->stopFinding(false);
    range = frame->selectionRange();
    ASSERT_FALSE(range.isNull());
    EXPECT_EQ(0, range.startOffset());
    EXPECT_EQ(4, range.endOffset());
    // "bar4" is surrounded by <span>, but the focusable node should be the parent <div>.
    EXPECT_EQ(WebString::fromUTF8("DIV"), frame->document().focusedElement().tagName());

    // Find in <select> content.
    EXPECT_FALSE(frame->find(findIdentifier, WebString::fromUTF8("bar5"), options, false, 0));
    // If there are any matches, stopFinding will set the selection on the found text.
    // However, we do not expect any matches, so check that the selection is null.
    frame->stopFinding(false);
    range = frame->selectionRange();
    ASSERT_TRUE(range.isNull());
}

TEST_P(ParameterizedWebFrameTest, GetContentAsPlainText)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true);
    // We set the size because it impacts line wrapping, which changes the
    // resulting text value.
    webViewHelper.webView()->resize(WebSize(640, 480));
    WebFrame* frame = webViewHelper.webView()->mainFrame();

    // Generate a simple test case.
    const char simpleSource[] = "<div>Foo bar</div><div></div>baz";
    KURL testURL = toKURL("about:blank");
    FrameTestHelpers::loadHTMLString(frame, simpleSource, testURL);

    // Make sure it comes out OK.
    const std::string expected("Foo bar\nbaz");
    WebString text = frame->contentAsText(std::numeric_limits<size_t>::max());
    EXPECT_EQ(expected, text.utf8());

    // Try reading the same one with clipping of the text.
    const int length = 5;
    text = frame->contentAsText(length);
    EXPECT_EQ(expected.substr(0, length), text.utf8());

    // Now do a new test with a subframe.
    const char outerFrameSource[] = "Hello<iframe></iframe> world";
    FrameTestHelpers::loadHTMLString(frame, outerFrameSource, testURL);

    // Load something into the subframe.
    WebFrame* subframe = frame->firstChild();
    ASSERT_TRUE(subframe);
    FrameTestHelpers::loadHTMLString(subframe, "sub<p>text", testURL);

    text = frame->contentAsText(std::numeric_limits<size_t>::max());
    EXPECT_EQ("Hello world\n\nsub\ntext", text.utf8());

    // Get the frame text where the subframe separator falls on the boundary of
    // what we'll take. There used to be a crash in this case.
    text = frame->contentAsText(12);
    EXPECT_EQ("Hello world", text.utf8());
}

TEST_P(ParameterizedWebFrameTest, GetFullHtmlOfPage)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true);
    WebFrame* frame = webViewHelper.webView()->mainFrame();

    // Generate a simple test case.
    const char simpleSource[] = "<p>Hello</p><p>World</p>";
    KURL testURL = toKURL("about:blank");
    FrameTestHelpers::loadHTMLString(frame, simpleSource, testURL);

    WebString text = frame->contentAsText(std::numeric_limits<size_t>::max());
    EXPECT_EQ("Hello\n\nWorld", text.utf8());

    const std::string html = frame->contentAsMarkup().utf8();

    // Load again with the output html.
    FrameTestHelpers::loadHTMLString(frame, html, testURL);

    EXPECT_EQ(html, frame->contentAsMarkup().utf8());

    text = frame->contentAsText(std::numeric_limits<size_t>::max());
    EXPECT_EQ("Hello\n\nWorld", text.utf8());

    // Test selection check
    EXPECT_FALSE(frame->hasSelection());
    frame->executeCommand(WebString::fromUTF8("SelectAll"));
    EXPECT_TRUE(frame->hasSelection());
    frame->executeCommand(WebString::fromUTF8("Unselect"));
    EXPECT_FALSE(frame->hasSelection());
    WebString selectionHtml = frame->selectionAsMarkup();
    EXPECT_TRUE(selectionHtml.isEmpty());
}

class TestExecuteScriptDuringDidCreateScriptContext : public FrameTestHelpers::TestWebFrameClient {
public:
    void didCreateScriptContext(WebLocalFrame* frame, v8::Local<v8::Context> context, int extensionGroup, int worldId) override
    {
        frame->executeScript(WebScriptSource("window.history = 'replaced';"));
    }
};

TEST_P(ParameterizedWebFrameTest, ExecuteScriptDuringDidCreateScriptContext)
{
    registerMockedHttpURLLoad("hello_world.html");

    TestExecuteScriptDuringDidCreateScriptContext webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "hello_world.html", true, &webFrameClient);

    FrameTestHelpers::reloadFrame(webViewHelper.webView()->mainFrame());
}

class FindUpdateWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    FindUpdateWebFrameClient()
        : m_findResultsAreReady(false)
        , m_count(-1)
    {
    }

    void reportFindInPageMatchCount(int, int count, bool finalUpdate) override
    {
        m_count = count;
        if (finalUpdate)
            m_findResultsAreReady = true;
    }

    bool findResultsAreReady() const { return m_findResultsAreReady; }
    int count() const { return m_count; }

private:
    bool m_findResultsAreReady;
    int m_count;
};

TEST_P(ParameterizedWebFrameTest, FindInPageMatchRects)
{
    registerMockedHttpURLLoad("find_in_page.html");
    registerMockedHttpURLLoad("find_in_page_frame.html");

    FindUpdateWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "find_in_page.html", true, &client);
    webViewHelper.webView()->resize(WebSize(640, 480));
    webViewHelper.webView()->setMaximumLegibleScale(1.f);
    webViewHelper.webView()->layout();
    runPendingTasks();

    // Note that the 'result 19' in the <select> element is not expected to produce a match.
    static const char* kFindString = "result";
    static const int kFindIdentifier = 12345;
    static const int kNumResults = 19;

    WebFindOptions options;
    WebString searchText = WebString::fromUTF8(kFindString);
    WebLocalFrameImpl* mainFrame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    EXPECT_TRUE(mainFrame->find(kFindIdentifier, searchText, options, false, 0));

    mainFrame->resetMatchCount();

    for (WebFrame* frame = mainFrame; frame; frame = frame->traverseNext(false))
        frame->scopeStringMatches(kFindIdentifier, searchText, options, true);

    runPendingTasks();
    EXPECT_TRUE(client.findResultsAreReady());

    WebVector<WebFloatRect> webMatchRects;
    mainFrame->findMatchRects(webMatchRects);
    ASSERT_EQ(webMatchRects.size(), static_cast<size_t>(kNumResults));
    int rectsVersion = mainFrame->findMatchMarkersVersion();

    for (int resultIndex = 0; resultIndex < kNumResults; ++resultIndex) {
        FloatRect resultRect = static_cast<FloatRect>(webMatchRects[resultIndex]);

        // Select the match by the center of its rect.
        EXPECT_EQ(mainFrame->selectNearestFindMatch(resultRect.center(), 0), resultIndex + 1);

        // Check that the find result ordering matches with our expectations.
        Range* result = mainFrame->activeMatchFrame()->activeMatch();
        ASSERT_TRUE(result);
        result->setEnd(result->endContainer(), result->endOffset() + 3);
        EXPECT_EQ(result->text(), String::format("%s %02d", kFindString, resultIndex));

        // Verify that the expected match rect also matches the currently active match.
        // Compare the enclosing rects to prevent precision issues caused by CSS transforms.
        FloatRect activeMatch = mainFrame->activeFindMatchRect();
        EXPECT_EQ(enclosingIntRect(activeMatch), enclosingIntRect(resultRect));

        // The rects version should not have changed.
        EXPECT_EQ(mainFrame->findMatchMarkersVersion(), rectsVersion);
    }

    // All results after the first two ones should be below between them in find-in-page coordinates.
    // This is because results 2 to 9 are inside an iframe located between results 0 and 1. This applies to the fixed div too.
    EXPECT_TRUE(webMatchRects[0].y < webMatchRects[1].y);
    for (int resultIndex = 2; resultIndex < kNumResults; ++resultIndex) {
        EXPECT_TRUE(webMatchRects[0].y < webMatchRects[resultIndex].y);
        EXPECT_TRUE(webMatchRects[1].y > webMatchRects[resultIndex].y);
    }

    // Result 3 should be below both 2 and 4. This is caused by the CSS transform in the containing div.
    // If the transform doesn't work then 3 will be between 2 and 4.
    EXPECT_TRUE(webMatchRects[3].y > webMatchRects[2].y);
    EXPECT_TRUE(webMatchRects[3].y > webMatchRects[4].y);

    // Results 6, 7, 8 and 9 should be one below the other in that same order.
    // If overflow:scroll is not properly handled then result 8 would be below result 9 or
    // result 7 above result 6 depending on the scroll.
    EXPECT_TRUE(webMatchRects[6].y < webMatchRects[7].y);
    EXPECT_TRUE(webMatchRects[7].y < webMatchRects[8].y);
    EXPECT_TRUE(webMatchRects[8].y < webMatchRects[9].y);

    // Results 11, 12, 13 and 14 should be between results 10 and 15, as they are inside the table.
    EXPECT_TRUE(webMatchRects[11].y > webMatchRects[10].y);
    EXPECT_TRUE(webMatchRects[12].y > webMatchRects[10].y);
    EXPECT_TRUE(webMatchRects[13].y > webMatchRects[10].y);
    EXPECT_TRUE(webMatchRects[14].y > webMatchRects[10].y);
    EXPECT_TRUE(webMatchRects[11].y < webMatchRects[15].y);
    EXPECT_TRUE(webMatchRects[12].y < webMatchRects[15].y);
    EXPECT_TRUE(webMatchRects[13].y < webMatchRects[15].y);
    EXPECT_TRUE(webMatchRects[14].y < webMatchRects[15].y);

    // Result 11 should be above 12, 13 and 14 as it's in the table header.
    EXPECT_TRUE(webMatchRects[11].y < webMatchRects[12].y);
    EXPECT_TRUE(webMatchRects[11].y < webMatchRects[13].y);
    EXPECT_TRUE(webMatchRects[11].y < webMatchRects[14].y);

    // Result 11 should also be right to 12, 13 and 14 because of the colspan.
    EXPECT_TRUE(webMatchRects[11].x > webMatchRects[12].x);
    EXPECT_TRUE(webMatchRects[11].x > webMatchRects[13].x);
    EXPECT_TRUE(webMatchRects[11].x > webMatchRects[14].x);

    // Result 12 should be left to results 11, 13 and 14 in the table layout.
    EXPECT_TRUE(webMatchRects[12].x < webMatchRects[11].x);
    EXPECT_TRUE(webMatchRects[12].x < webMatchRects[13].x);
    EXPECT_TRUE(webMatchRects[12].x < webMatchRects[14].x);

    // Results 13, 12 and 14 should be one above the other in that order because of the rowspan
    // and vertical-align: middle by default.
    EXPECT_TRUE(webMatchRects[13].y < webMatchRects[12].y);
    EXPECT_TRUE(webMatchRects[12].y < webMatchRects[14].y);

    // Result 16 should be below result 15.
    EXPECT_TRUE(webMatchRects[15].y > webMatchRects[14].y);

    // Result 18 should be normalized with respect to the position:relative div, and not it's
    // immediate containing div. Consequently, result 18 should be above result 17.
    EXPECT_TRUE(webMatchRects[17].y > webMatchRects[18].y);

    // Resizing should update the rects version.
    webViewHelper.webView()->resize(WebSize(800, 600));
    webViewHelper.webView()->layout();
    runPendingTasks();
    EXPECT_TRUE(mainFrame->findMatchMarkersVersion() != rectsVersion);
}

TEST_P(ParameterizedWebFrameTest, FindInPageSkipsHiddenFrames)
{
    registerMockedHttpURLLoad("find_in_hidden_frame.html");

    FindUpdateWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "find_in_hidden_frame.html", true, &client);
    webViewHelper.webView()->resize(WebSize(640, 480));
    webViewHelper.webView()->layout();
    runPendingTasks();

    static const char* kFindString = "hello";
    static const int kFindIdentifier = 12345;
    static const int kNumResults = 1;

    WebFindOptions options;
    WebString searchText = WebString::fromUTF8(kFindString);
    WebLocalFrameImpl* mainFrame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    EXPECT_TRUE(mainFrame->find(kFindIdentifier, searchText, options, false, 0));

    mainFrame->resetMatchCount();

    for (WebFrame* frame = mainFrame; frame; frame = frame->traverseNext(false))
        frame->scopeStringMatches(kFindIdentifier, searchText, options, true);

    runPendingTasks();
    EXPECT_TRUE(client.findResultsAreReady());
    EXPECT_EQ(kNumResults, client.count());
}

TEST_P(ParameterizedWebFrameTest, FindOnDetachedFrame)
{
    registerMockedHttpURLLoad("find_in_page.html");
    registerMockedHttpURLLoad("find_in_page_frame.html");

    FindUpdateWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "find_in_page.html", true, &client);
    webViewHelper.webView()->resize(WebSize(640, 480));
    webViewHelper.webView()->layout();
    runPendingTasks();

    static const char* kFindString = "result";
    static const int kFindIdentifier = 12345;

    WebFindOptions options;
    WebString searchText = WebString::fromUTF8(kFindString);
    WebLocalFrameImpl* mainFrame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    RefPtrWillBeRawPtr<WebLocalFrameImpl> secondFrame = toWebLocalFrameImpl(mainFrame->traverseNext(false));
    RefPtrWillBeRawPtr<LocalFrame> holdSecondFrame(secondFrame->frame());

    // Detach the frame before finding.
    EXPECT_TRUE(mainFrame->document().getElementById("frame").remove());

    EXPECT_TRUE(mainFrame->find(kFindIdentifier, searchText, options, false, 0));
    EXPECT_FALSE(secondFrame->find(kFindIdentifier, searchText, options, false, 0));

    runPendingTasks();
    EXPECT_FALSE(client.findResultsAreReady());

    mainFrame->resetMatchCount();

    for (WebFrame* frame = mainFrame; frame; frame = frame->traverseNext(false))
        frame->scopeStringMatches(kFindIdentifier, searchText, options, true);

    runPendingTasks();
    EXPECT_TRUE(client.findResultsAreReady());
}

TEST_P(ParameterizedWebFrameTest, FindDetachFrameBeforeScopeStrings)
{
    registerMockedHttpURLLoad("find_in_page.html");
    registerMockedHttpURLLoad("find_in_page_frame.html");

    FindUpdateWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "find_in_page.html", true, &client);
    webViewHelper.webView()->resize(WebSize(640, 480));
    webViewHelper.webView()->layout();
    runPendingTasks();

    static const char* kFindString = "result";
    static const int kFindIdentifier = 12345;

    WebFindOptions options;
    WebString searchText = WebString::fromUTF8(kFindString);
    WebLocalFrameImpl* mainFrame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    WebLocalFrameImpl* secondFrame = toWebLocalFrameImpl(mainFrame->traverseNext(false));
    RefPtrWillBeRawPtr<LocalFrame> holdSecondFrame(secondFrame->frame());

    for (WebFrame* frame = mainFrame; frame; frame = frame->traverseNext(false))
        EXPECT_TRUE(frame->find(kFindIdentifier, searchText, options, false, 0));

    runPendingTasks();
    EXPECT_FALSE(client.findResultsAreReady());

    // Detach the frame between finding and scoping.
    EXPECT_TRUE(mainFrame->document().getElementById("frame").remove());

    mainFrame->resetMatchCount();

    for (WebFrame* frame = mainFrame; frame; frame = frame->traverseNext(false))
        frame->scopeStringMatches(kFindIdentifier, searchText, options, true);

    runPendingTasks();
    EXPECT_TRUE(client.findResultsAreReady());
}

TEST_P(ParameterizedWebFrameTest, FindDetachFrameWhileScopingStrings)
{
    registerMockedHttpURLLoad("find_in_page.html");
    registerMockedHttpURLLoad("find_in_page_frame.html");

    FindUpdateWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "find_in_page.html", true, &client);
    webViewHelper.webView()->resize(WebSize(640, 480));
    webViewHelper.webView()->layout();
    runPendingTasks();

    static const char* kFindString = "result";
    static const int kFindIdentifier = 12345;

    WebFindOptions options;
    WebString searchText = WebString::fromUTF8(kFindString);
    WebLocalFrameImpl* mainFrame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    WebLocalFrameImpl* secondFrame = toWebLocalFrameImpl(mainFrame->traverseNext(false));
    RefPtrWillBeRawPtr<LocalFrame> holdSecondFrame(secondFrame->frame());

    for (WebFrame* frame = mainFrame; frame; frame = frame->traverseNext(false))
        EXPECT_TRUE(frame->find(kFindIdentifier, searchText, options, false, 0));

    runPendingTasks();
    EXPECT_FALSE(client.findResultsAreReady());

    mainFrame->resetMatchCount();

    for (WebFrame* frame = mainFrame; frame; frame = frame->traverseNext(false))
        frame->scopeStringMatches(kFindIdentifier, searchText, options, true);

    // The first scopeStringMatches will have reset the state. Detach before it actually scopes.
    EXPECT_TRUE(mainFrame->document().getElementById("frame").remove());

    runPendingTasks();
    EXPECT_TRUE(client.findResultsAreReady());
}

TEST_P(ParameterizedWebFrameTest, ResetMatchCount)
{
    registerMockedHttpURLLoad("find_in_generated_frame.html");

    FindUpdateWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "find_in_generated_frame.html", true, &client);
    webViewHelper.webView()->resize(WebSize(640, 480));
    webViewHelper.webView()->layout();
    runPendingTasks();

    static const char* kFindString = "result";
    static const int kFindIdentifier = 12345;

    WebFindOptions options;
    WebString searchText = WebString::fromUTF8(kFindString);
    WebLocalFrameImpl* mainFrame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());

    // Check that child frame exists.
    EXPECT_TRUE(!!mainFrame->traverseNext(false));

    for (WebFrame* frame = mainFrame; frame; frame = frame->traverseNext(false)) {
        EXPECT_FALSE(frame->find(kFindIdentifier, searchText, options, false, 0));
    }

    runPendingTasks();
    EXPECT_FALSE(client.findResultsAreReady());

    mainFrame->resetMatchCount();
}

TEST_P(ParameterizedWebFrameTest, SetTickmarks)
{
    registerMockedHttpURLLoad("find.html");

    FindUpdateWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "find.html", true, &client);
    webViewHelper.webView()->resize(WebSize(640, 480));
    webViewHelper.webView()->layout();
    runPendingTasks();

    static const char* kFindString = "foo";
    static const int kFindIdentifier = 12345;

    WebFindOptions options;
    WebString searchText = WebString::fromUTF8(kFindString);
    WebLocalFrameImpl* mainFrame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    EXPECT_TRUE(mainFrame->find(kFindIdentifier, searchText, options, false, 0));

    mainFrame->resetMatchCount();
    mainFrame->scopeStringMatches(kFindIdentifier, searchText, options, true);

    runPendingTasks();
    EXPECT_TRUE(client.findResultsAreReady());

    // Get the tickmarks for the original find request.
    FrameView* frameView = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    RefPtrWillBeRawPtr<Scrollbar> scrollbar = frameView->createScrollbar(HorizontalScrollbar);
    Vector<IntRect> originalTickmarks;
    scrollbar->getTickmarks(originalTickmarks);
    EXPECT_EQ(4u, originalTickmarks.size());

    // Override the tickmarks.
    Vector<IntRect> overridingTickmarksExpected;
    overridingTickmarksExpected.append(IntRect(0, 0, 100, 100));
    overridingTickmarksExpected.append(IntRect(0, 20, 100, 100));
    overridingTickmarksExpected.append(IntRect(0, 30, 100, 100));
    mainFrame->setTickmarks(overridingTickmarksExpected);

    // Check the tickmarks are overriden correctly.
    Vector<IntRect> overridingTickmarksActual;
    scrollbar->getTickmarks(overridingTickmarksActual);
    EXPECT_EQ(overridingTickmarksExpected, overridingTickmarksActual);

    // Reset the tickmark behavior.
    Vector<IntRect> resetTickmarks;
    mainFrame->setTickmarks(resetTickmarks);

    // Check that the original tickmarks are returned
    Vector<IntRect> originalTickmarksAfterReset;
    scrollbar->getTickmarks(originalTickmarksAfterReset);
    EXPECT_EQ(originalTickmarks, originalTickmarksAfterReset);
}

static WebPoint topLeft(const WebRect& rect)
{
    return WebPoint(rect.x, rect.y);
}

static WebPoint bottomRightMinusOne(const WebRect& rect)
{
    // FIXME: If we don't subtract 1 from the x- and y-coordinates of the
    // selection bounds, selectRange() will select the *next* element. That's
    // strictly correct, as hit-testing checks the pixel to the lower-right of
    // the input coordinate, but it's a wart on the API.
    return WebPoint(rect.x + rect.width - 1, rect.y + rect.height - 1);
}

static WebRect elementBounds(WebFrame* frame, const WebString& id)
{
    return frame->document().getElementById(id).boundsInViewportSpace();
}

static std::string selectionAsString(WebFrame* frame)
{
    return frame->selectionAsText().utf8();
}

TEST_P(ParameterizedWebFrameTest, SelectRange)
{
    WebFrame* frame;
    WebRect startWebRect;
    WebRect endWebRect;

    registerMockedHttpURLLoad("select_range_basic.html");
    registerMockedHttpURLLoad("select_range_scroll.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "select_range_basic.html", &webViewHelper);
    frame = webViewHelper.webView()->mainFrame();
    EXPECT_EQ("Some test text for testing.", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);
    frame->executeCommand(WebString::fromUTF8("Unselect"));
    EXPECT_EQ("", selectionAsString(frame));
    frame->selectRange(topLeft(startWebRect), bottomRightMinusOne(endWebRect));
    EXPECT_EQ("Some test text for testing.", selectionAsString(frame));

    initializeTextSelectionWebView(m_baseURL + "select_range_scroll.html", &webViewHelper);
    frame = webViewHelper.webView()->mainFrame();
    EXPECT_EQ("Some offscreen test text for testing.", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);
    frame->executeCommand(WebString::fromUTF8("Unselect"));
    EXPECT_EQ("", selectionAsString(frame));
    frame->selectRange(topLeft(startWebRect), bottomRightMinusOne(endWebRect));
    EXPECT_EQ("Some offscreen test text for testing.", selectionAsString(frame));
}

TEST_P(ParameterizedWebFrameTest, SelectRangeInIframe)
{
    WebFrame* frame;
    WebRect startWebRect;
    WebRect endWebRect;

    registerMockedHttpURLLoad("select_range_iframe.html");
    registerMockedHttpURLLoad("select_range_basic.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "select_range_iframe.html", &webViewHelper);
    frame = webViewHelper.webView()->mainFrame();
    WebFrame* subframe = frame->firstChild();
    EXPECT_EQ("Some test text for testing.", selectionAsString(subframe));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);
    subframe->executeCommand(WebString::fromUTF8("Unselect"));
    EXPECT_EQ("", selectionAsString(subframe));
    subframe->selectRange(topLeft(startWebRect), bottomRightMinusOne(endWebRect));
    EXPECT_EQ("Some test text for testing.", selectionAsString(subframe));
}

TEST_P(ParameterizedWebFrameTest, SelectRangeDivContentEditable)
{
    WebFrame* frame;
    WebRect startWebRect;
    WebRect endWebRect;

    registerMockedHttpURLLoad("select_range_div_editable.html");

    // Select the middle of an editable element, then try to extend the selection to the top of the document.
    // The selection range should be clipped to the bounds of the editable element.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "select_range_div_editable.html", &webViewHelper);
    frame = webViewHelper.webView()->mainFrame();
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);

    frame->selectRange(bottomRightMinusOne(endWebRect), WebPoint(0, 0));
    EXPECT_EQ("16-char header. This text is initially selected.", selectionAsString(frame));

    // As above, but extending the selection to the bottom of the document.
    initializeTextSelectionWebView(m_baseURL + "select_range_div_editable.html", &webViewHelper);
    frame = webViewHelper.webView()->mainFrame();

    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);
    frame->selectRange(topLeft(startWebRect), bottomRightMinusOne(endWebRect));
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);

    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);
    frame->selectRange(topLeft(startWebRect), WebPoint(640, 480));
    EXPECT_EQ("This text is initially selected. 16-char footer.", selectionAsString(frame));
}

// positionForPoint returns the wrong values for contenteditable spans. See
// http://crbug.com/238334.
TEST_P(ParameterizedWebFrameTest, DISABLED_SelectRangeSpanContentEditable)
{
    WebFrame* frame;
    WebRect startWebRect;
    WebRect endWebRect;

    registerMockedHttpURLLoad("select_range_span_editable.html");

    // Select the middle of an editable element, then try to extend the selection to the top of the document.
    // The selection range should be clipped to the bounds of the editable element.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "select_range_span_editable.html", &webViewHelper);
    frame = webViewHelper.webView()->mainFrame();
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);

    frame->selectRange(bottomRightMinusOne(endWebRect), WebPoint(0, 0));
    EXPECT_EQ("16-char header. This text is initially selected.", selectionAsString(frame));

    // As above, but extending the selection to the bottom of the document.
    initializeTextSelectionWebView(m_baseURL + "select_range_span_editable.html", &webViewHelper);
    frame = webViewHelper.webView()->mainFrame();

    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);
    frame->selectRange(topLeft(startWebRect), bottomRightMinusOne(endWebRect));
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);

    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);
    frame->selectRange(topLeft(startWebRect), WebPoint(640, 480));
    EXPECT_EQ("This text is initially selected. 16-char footer.", selectionAsString(frame));
}

TEST_P(ParameterizedWebFrameTest, SelectRangeCanMoveSelectionStart)
{
    registerMockedHttpURLLoad("text_selection.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "text_selection.html", &webViewHelper);
    WebFrame* frame = webViewHelper.webView()->mainFrame();

    // Select second span. We can move the start to include the first span.
    frame->executeScript(WebScriptSource("selectElement('header_2');"));
    EXPECT_EQ("Header 2.", selectionAsString(frame));
    frame->selectRange(bottomRightMinusOne(elementBounds(frame, "header_2")), topLeft(elementBounds(frame, "header_1")));
    EXPECT_EQ("Header 1. Header 2.", selectionAsString(frame));

    // We can move the start and end together.
    frame->executeScript(WebScriptSource("selectElement('header_1');"));
    EXPECT_EQ("Header 1.", selectionAsString(frame));
    frame->selectRange(bottomRightMinusOne(elementBounds(frame, "header_1")), bottomRightMinusOne(elementBounds(frame, "header_1")));
    EXPECT_EQ("", selectionAsString(frame));
    // Selection is a caret, not empty.
    EXPECT_FALSE(frame->selectionRange().isNull());

    // We can move the start across the end.
    frame->executeScript(WebScriptSource("selectElement('header_1');"));
    EXPECT_EQ("Header 1.", selectionAsString(frame));
    frame->selectRange(bottomRightMinusOne(elementBounds(frame, "header_1")), bottomRightMinusOne(elementBounds(frame, "header_2")));
    EXPECT_EQ(" Header 2.", selectionAsString(frame));

    // Can't extend the selection part-way into an editable element.
    frame->executeScript(WebScriptSource("selectElement('footer_2');"));
    EXPECT_EQ("Footer 2.", selectionAsString(frame));
    frame->selectRange(bottomRightMinusOne(elementBounds(frame, "footer_2")), topLeft(elementBounds(frame, "editable_2")));
    EXPECT_EQ(" [ Footer 1. Footer 2.", selectionAsString(frame));

    // Can extend the selection completely across editable elements.
    frame->executeScript(WebScriptSource("selectElement('footer_2');"));
    EXPECT_EQ("Footer 2.", selectionAsString(frame));
    frame->selectRange(bottomRightMinusOne(elementBounds(frame, "footer_2")), topLeft(elementBounds(frame, "header_2")));
    EXPECT_EQ("Header 2. ] [ Editable 1. Editable 2. ] [ Footer 1. Footer 2.", selectionAsString(frame));

    // If the selection is editable text, we can't extend it into non-editable text.
    frame->executeScript(WebScriptSource("selectElement('editable_2');"));
    EXPECT_EQ("Editable 2.", selectionAsString(frame));
    frame->selectRange(bottomRightMinusOne(elementBounds(frame, "editable_2")), topLeft(elementBounds(frame, "header_2")));
    // positionForPoint returns the wrong values for contenteditable spans. See
    // http://crbug.com/238334.
    // EXPECT_EQ("[ Editable 1. Editable 2.", selectionAsString(frame));
}

TEST_P(ParameterizedWebFrameTest, SelectRangeCanMoveSelectionEnd)
{
    registerMockedHttpURLLoad("text_selection.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "text_selection.html", &webViewHelper);
    WebFrame* frame = webViewHelper.webView()->mainFrame();

    // Select first span. We can move the end to include the second span.
    frame->executeScript(WebScriptSource("selectElement('header_1');"));
    EXPECT_EQ("Header 1.", selectionAsString(frame));
    frame->selectRange(topLeft(elementBounds(frame, "header_1")), bottomRightMinusOne(elementBounds(frame, "header_2")));
    EXPECT_EQ("Header 1. Header 2.", selectionAsString(frame));

    // We can move the start and end together.
    frame->executeScript(WebScriptSource("selectElement('header_2');"));
    EXPECT_EQ("Header 2.", selectionAsString(frame));
    frame->selectRange(topLeft(elementBounds(frame, "header_2")), topLeft(elementBounds(frame, "header_2")));
    EXPECT_EQ("", selectionAsString(frame));
    // Selection is a caret, not empty.
    EXPECT_FALSE(frame->selectionRange().isNull());

    // We can move the end across the start.
    frame->executeScript(WebScriptSource("selectElement('header_2');"));
    EXPECT_EQ("Header 2.", selectionAsString(frame));
    frame->selectRange(topLeft(elementBounds(frame, "header_2")), topLeft(elementBounds(frame, "header_1")));
    EXPECT_EQ("Header 1. ", selectionAsString(frame));

    // Can't extend the selection part-way into an editable element.
    frame->executeScript(WebScriptSource("selectElement('header_1');"));
    EXPECT_EQ("Header 1.", selectionAsString(frame));
    frame->selectRange(topLeft(elementBounds(frame, "header_1")), bottomRightMinusOne(elementBounds(frame, "editable_1")));
    EXPECT_EQ("Header 1. Header 2. ] ", selectionAsString(frame));

    // Can extend the selection completely across editable elements.
    frame->executeScript(WebScriptSource("selectElement('header_1');"));
    EXPECT_EQ("Header 1.", selectionAsString(frame));
    frame->selectRange(topLeft(elementBounds(frame, "header_1")), bottomRightMinusOne(elementBounds(frame, "footer_1")));
    EXPECT_EQ("Header 1. Header 2. ] [ Editable 1. Editable 2. ] [ Footer 1.", selectionAsString(frame));

    // If the selection is editable text, we can't extend it into non-editable text.
    frame->executeScript(WebScriptSource("selectElement('editable_1');"));
    EXPECT_EQ("Editable 1.", selectionAsString(frame));
    frame->selectRange(topLeft(elementBounds(frame, "editable_1")), bottomRightMinusOne(elementBounds(frame, "footer_1")));
    // positionForPoint returns the wrong values for contenteditable spans. See
    // http://crbug.com/238334.
    // EXPECT_EQ("Editable 1. Editable 2. ]", selectionAsString(frame));
}

TEST_P(ParameterizedWebFrameTest, MoveRangeSelectionExtent)
{
    WebLocalFrameImpl* frame;
    WebRect startWebRect;
    WebRect endWebRect;

    registerMockedHttpURLLoad("move_range_selection_extent.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "move_range_selection_extent.html", &webViewHelper);
    frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);

    frame->moveRangeSelectionExtent(WebPoint(640, 480));
    EXPECT_EQ("This text is initially selected. 16-char footer.", selectionAsString(frame));

    frame->moveRangeSelectionExtent(WebPoint(0, 0));
    EXPECT_EQ("16-char header. ", selectionAsString(frame));

    // Reset with swapped base and extent.
    frame->selectRange(topLeft(endWebRect), bottomRightMinusOne(startWebRect));
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));

    frame->moveRangeSelectionExtent(WebPoint(640, 480));
    EXPECT_EQ(" 16-char footer.", selectionAsString(frame));

    frame->moveRangeSelectionExtent(WebPoint(0, 0));
    EXPECT_EQ("16-char header. This text is initially selected.", selectionAsString(frame));

    frame->executeCommand(WebString::fromUTF8("Unselect"));
    EXPECT_EQ("", selectionAsString(frame));
}

TEST_P(ParameterizedWebFrameTest, MoveRangeSelectionExtentCannotCollapse)
{
    WebLocalFrameImpl* frame;
    WebRect startWebRect;
    WebRect endWebRect;

    registerMockedHttpURLLoad("move_range_selection_extent.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "move_range_selection_extent.html", &webViewHelper);
    frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);

    frame->moveRangeSelectionExtent(bottomRightMinusOne(startWebRect));
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));

    // Reset with swapped base and extent.
    frame->selectRange(topLeft(endWebRect), bottomRightMinusOne(startWebRect));
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));

    frame->moveRangeSelectionExtent(bottomRightMinusOne(endWebRect));
    EXPECT_EQ("This text is initially selected.", selectionAsString(frame));
}

TEST_P(ParameterizedWebFrameTest, MoveRangeSelectionExtentScollsInputField)
{
    WebLocalFrameImpl* frame;
    WebRect startWebRect;
    WebRect endWebRect;

    registerMockedHttpURLLoad("move_range_selection_extent_input_field.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "move_range_selection_extent_input_field.html", &webViewHelper);
    frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    EXPECT_EQ("Length", selectionAsString(frame));
    webViewHelper.webView()->selectionBounds(startWebRect, endWebRect);

    EXPECT_EQ(0, frame->frame()->selection().rootEditableElement()->scrollLeft());
    frame->moveRangeSelectionExtent(WebPoint(endWebRect.x + 500, endWebRect.y));
    EXPECT_GE(frame->frame()->selection().rootEditableElement()->scrollLeft(), 1);
    EXPECT_EQ("Lengthy text goes here.", selectionAsString(frame));
}

static int computeOffset(LayoutObject* layoutObject, int x, int y)
{
    return VisiblePosition(layoutObject->positionForPoint(LayoutPoint(x, y))).deepEquivalent().computeOffsetInContainerNode();
}

// positionForPoint returns the wrong values for contenteditable spans. See
// http://crbug.com/238334.
TEST_P(ParameterizedWebFrameTest, DISABLED_PositionForPointTest)
{
    registerMockedHttpURLLoad("select_range_span_editable.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "select_range_span_editable.html", &webViewHelper);
    WebLocalFrameImpl* mainFrame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    LayoutObject* layoutObject = mainFrame->frame()->selection().rootEditableElement()->layoutObject();
    EXPECT_EQ(0, computeOffset(layoutObject, -1, -1));
    EXPECT_EQ(64, computeOffset(layoutObject, 1000, 1000));

    registerMockedHttpURLLoad("select_range_div_editable.html");
    initializeTextSelectionWebView(m_baseURL + "select_range_div_editable.html", &webViewHelper);
    mainFrame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    layoutObject = mainFrame->frame()->selection().rootEditableElement()->layoutObject();
    EXPECT_EQ(0, computeOffset(layoutObject, -1, -1));
    EXPECT_EQ(64, computeOffset(layoutObject, 1000, 1000));
}

#if !OS(MACOSX) && !OS(LINUX)
TEST_P(ParameterizedWebFrameTest, SelectRangeStaysHorizontallyAlignedWhenMoved)
{
    registerMockedHttpURLLoad("move_caret.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "move_caret.html", &webViewHelper);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());

    WebRect initialStartRect;
    WebRect initialEndRect;
    WebRect startRect;
    WebRect endRect;

    frame->executeScript(WebScriptSource("selectRange();"));
    webViewHelper.webView()->selectionBounds(initialStartRect, initialEndRect);
    WebPoint movedStart(topLeft(initialStartRect));

    movedStart.y += 40;
    frame->selectRange(movedStart, bottomRightMinusOne(initialEndRect));
    webViewHelper.webView()->selectionBounds(startRect, endRect);
    EXPECT_EQ(startRect, initialStartRect);
    EXPECT_EQ(endRect, initialEndRect);

    movedStart.y -= 80;
    frame->selectRange(movedStart, bottomRightMinusOne(initialEndRect));
    webViewHelper.webView()->selectionBounds(startRect, endRect);
    EXPECT_EQ(startRect, initialStartRect);
    EXPECT_EQ(endRect, initialEndRect);

    WebPoint movedEnd(bottomRightMinusOne(initialEndRect));

    movedEnd.y += 40;
    frame->selectRange(topLeft(initialStartRect), movedEnd);
    webViewHelper.webView()->selectionBounds(startRect, endRect);
    EXPECT_EQ(startRect, initialStartRect);
    EXPECT_EQ(endRect, initialEndRect);

    movedEnd.y -= 80;
    frame->selectRange(topLeft(initialStartRect), movedEnd);
    webViewHelper.webView()->selectionBounds(startRect, endRect);
    EXPECT_EQ(startRect, initialStartRect);
    EXPECT_EQ(endRect, initialEndRect);
}

TEST_P(ParameterizedWebFrameTest, MoveCaretStaysHorizontallyAlignedWhenMoved)
{
    WebLocalFrameImpl* frame;
    registerMockedHttpURLLoad("move_caret.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    initializeTextSelectionWebView(m_baseURL + "move_caret.html", &webViewHelper);
    frame = (WebLocalFrameImpl*)webViewHelper.webView()->mainFrame();

    WebRect initialStartRect;
    WebRect initialEndRect;
    WebRect startRect;
    WebRect endRect;

    frame->executeScript(WebScriptSource("selectCaret();"));
    webViewHelper.webView()->selectionBounds(initialStartRect, initialEndRect);
    WebPoint moveTo(topLeft(initialStartRect));

    moveTo.y += 40;
    frame->moveCaretSelection(moveTo);
    webViewHelper.webView()->selectionBounds(startRect, endRect);
    EXPECT_EQ(startRect, initialStartRect);
    EXPECT_EQ(endRect, initialEndRect);

    moveTo.y -= 80;
    frame->moveCaretSelection(moveTo);
    webViewHelper.webView()->selectionBounds(startRect, endRect);
    EXPECT_EQ(startRect, initialStartRect);
    EXPECT_EQ(endRect, initialEndRect);
}
#endif

class CompositedSelectionBoundsTestLayerTreeView : public WebLayerTreeView {
public:
    CompositedSelectionBoundsTestLayerTreeView() : m_selectionCleared(false) { }
    ~CompositedSelectionBoundsTestLayerTreeView() override { }

    void setRootLayer(const WebLayer&)  override { }
    void clearRootLayer()  override { }
    void setViewportSize(const WebSize& deviceViewportSize)  override { }
    WebSize deviceViewportSize() const  override { return WebSize(); }
    void setDeviceScaleFactor(float) override { }
    float deviceScaleFactor() const  override { return 1.f; }
    void setBackgroundColor(WebColor)  override { }
    void setHasTransparentBackground(bool)  override { }
    void setVisible(bool)  override { }
    void setPageScaleFactorAndLimits(float pageScaleFactor, float minimum, float maximum)  override { }
    void startPageScaleAnimation(const WebPoint& destination, bool useAnchor, float newPageScale, double durationSec)  override { }
    void setNeedsAnimate()  override { }
    void finishAllRendering()  override { }
    void registerSelection(const WebSelection& selection) override
    {
        m_selection = adoptPtr(new WebSelection(selection));
    }
    void clearSelection() override
    {
        m_selectionCleared = true;
        m_selection.clear();
    }

    bool getAndResetSelectionCleared()
    {
        bool selectionCleared  = m_selectionCleared;
        m_selectionCleared = false;
        return selectionCleared;
    }

    const WebSelection* selection() const { return m_selection.get(); }
    const WebSelectionBound* start() const { return m_selection ? &m_selection->start() : nullptr; }
    const WebSelectionBound* end() const { return m_selection ? &m_selection->end() : nullptr; }

private:
    bool m_selectionCleared;
    OwnPtr<WebSelection> m_selection;
};

class CompositedSelectionBoundsTestWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    ~CompositedSelectionBoundsTestWebViewClient() override {}
    WebLayerTreeView* layerTreeView() override { return &m_testLayerTreeView; }

    CompositedSelectionBoundsTestLayerTreeView& selectionLayerTreeView() { return m_testLayerTreeView; }

private:
    CompositedSelectionBoundsTestLayerTreeView m_testLayerTreeView;
};

class CompositedSelectionBoundsTest : public WebFrameTest {
protected:
    CompositedSelectionBoundsTest()
        : m_fakeSelectionLayerTreeView(m_fakeSelectionWebViewClient.selectionLayerTreeView())
    {
        RuntimeEnabledFeatures::setCompositedSelectionUpdateEnabled(true);
        registerMockedHttpURLLoad("Ahem.ttf");

        m_webViewHelper.initialize(true, 0, &m_fakeSelectionWebViewClient);
        m_webViewHelper.webView()->settings()->setDefaultFontSize(12);
        m_webViewHelper.webView()->setDefaultPageScaleLimits(1, 1);
        m_webViewHelper.webView()->resize(WebSize(640, 480));
    }

    void runTest(const char* testFile)
    {
        registerMockedHttpURLLoad(testFile);
        FrameTestHelpers::loadFrame(m_webViewHelper.webView()->mainFrame(), m_baseURL + testFile);
        m_webViewHelper.webView()->layout();

        const WebSelection* selection = m_fakeSelectionLayerTreeView.selection();
        const WebSelectionBound* selectStart = m_fakeSelectionLayerTreeView.start();
        const WebSelectionBound* selectEnd = m_fakeSelectionLayerTreeView.end();

        v8::HandleScope handleScope(v8::Isolate::GetCurrent());
        v8::Local<v8::Value> result = m_webViewHelper.webView()->mainFrame()->toWebLocalFrame()->executeScriptAndReturnValue(WebScriptSource("expectedResult"));
        if (result.IsEmpty() || (*result)->IsUndefined()) {
            EXPECT_FALSE(selection);
            EXPECT_FALSE(selectStart);
            EXPECT_FALSE(selectEnd);
            return;
        }

        ASSERT_TRUE(selection);
        ASSERT_TRUE(selectStart);
        ASSERT_TRUE(selectEnd);

        EXPECT_FALSE(selection->isNone());

        ASSERT_TRUE((*result)->IsArray());
        v8::Array& expectedResult = *v8::Array::Cast(*result);
        ASSERT_GE(expectedResult.Length(), 10u);

        blink::Node* layerOwnerNodeForStart = V8Node::toImplWithTypeCheck(v8::Isolate::GetCurrent(), expectedResult.Get(0));
        ASSERT_TRUE(layerOwnerNodeForStart);
        EXPECT_EQ(layerOwnerNodeForStart->layoutObject()->enclosingLayer()->enclosingLayerForPaintInvalidation()->graphicsLayerBacking()->platformLayer()->id(), selectStart->layerId);
        v8::Local<v8::Context> context = v8::Isolate::GetCurrent()->GetCurrentContext();
        EXPECT_EQ(expectedResult.Get(context, 1).ToLocalChecked().As<v8::Int32>()->Value(), selectStart->edgeTopInLayer.x);
        EXPECT_EQ(expectedResult.Get(context, 2).ToLocalChecked().As<v8::Int32>()->Value(), selectStart->edgeTopInLayer.y);
        EXPECT_EQ(expectedResult.Get(context, 3).ToLocalChecked().As<v8::Int32>()->Value(), selectStart->edgeBottomInLayer.x);

        blink::Node* layerOwnerNodeForEnd = V8Node::toImplWithTypeCheck(v8::Isolate::GetCurrent(), expectedResult.Get(context, 5).ToLocalChecked());

        ASSERT_TRUE(layerOwnerNodeForEnd);
        EXPECT_EQ(layerOwnerNodeForEnd->layoutObject()->enclosingLayer()->enclosingLayerForPaintInvalidation()->graphicsLayerBacking()->platformLayer()->id(), selectEnd->layerId);
        EXPECT_EQ(expectedResult.Get(context, 6).ToLocalChecked().As<v8::Int32>()->Value(), selectEnd->edgeTopInLayer.x);
        EXPECT_EQ(expectedResult.Get(context, 7).ToLocalChecked().As<v8::Int32>()->Value(), selectEnd->edgeTopInLayer.y);
        EXPECT_EQ(expectedResult.Get(context, 8).ToLocalChecked().As<v8::Int32>()->Value(), selectEnd->edgeBottomInLayer.x);

        // Platform differences can introduce small stylistic deviations in
        // y-axis positioning, the details of which aren't relevant to
        // selection behavior. However, such deviations from the expected value
        // should be consistent for the corresponding y coordinates.
        int yBottomEpsilon = 0;
        if (expectedResult.Length() == 13)
            yBottomEpsilon = expectedResult.Get(context, 12).ToLocalChecked().As<v8::Int32>()->Value();
        int yBottomDeviation = expectedResult.Get(context, 4).ToLocalChecked().As<v8::Int32>()->Value() - selectStart->edgeBottomInLayer.y;
        EXPECT_GE(yBottomEpsilon, std::abs(yBottomDeviation));
        EXPECT_EQ(yBottomDeviation, expectedResult.Get(context, 9).ToLocalChecked().As<v8::Int32>()->Value() - selectEnd->edgeBottomInLayer.y);

        if (expectedResult.Length() >= 12) {
            EXPECT_EQ(expectedResult.Get(context, 10).ToLocalChecked().As<v8::Boolean>()->Value(), m_fakeSelectionLayerTreeView.selection()->isEditable());
            EXPECT_EQ(expectedResult.Get(context, 11).ToLocalChecked().As<v8::Boolean>()->Value(), m_fakeSelectionLayerTreeView.selection()->isEmptyTextFormControl());
        }
    }

    void runTestWithMultipleFiles(const char* testFile, ...)
    {
        va_list auxFiles;
        va_start(auxFiles, testFile);
        while (const char* auxFile = va_arg(auxFiles, const char*))
            registerMockedHttpURLLoad(auxFile);
        va_end(auxFiles);

        runTest(testFile);
    }

    CompositedSelectionBoundsTestWebViewClient m_fakeSelectionWebViewClient;
    CompositedSelectionBoundsTestLayerTreeView& m_fakeSelectionLayerTreeView;
    FrameTestHelpers::WebViewHelper m_webViewHelper;
};

TEST_F(CompositedSelectionBoundsTest, None) { runTest("composited_selection_bounds_none.html"); }
TEST_F(CompositedSelectionBoundsTest, NoneReadonlyCaret) { runTest("composited_selection_bounds_none_readonly_caret.html"); }
TEST_F(CompositedSelectionBoundsTest, Basic) { runTest("composited_selection_bounds_basic.html"); }
TEST_F(CompositedSelectionBoundsTest, Transformed) { runTest("composited_selection_bounds_transformed.html"); }
TEST_F(CompositedSelectionBoundsTest, SplitLayer) { runTest("composited_selection_bounds_split_layer.html"); }
TEST_F(CompositedSelectionBoundsTest, EmptyLayer) { runTest("composited_selection_bounds_empty_layer.html"); }
TEST_F(CompositedSelectionBoundsTest, Iframe) { runTestWithMultipleFiles("composited_selection_bounds_iframe.html", "composited_selection_bounds_basic.html", nullptr); }
TEST_F(CompositedSelectionBoundsTest, DetachedFrame) { runTest("composited_selection_bounds_detached_frame.html"); }
TEST_F(CompositedSelectionBoundsTest, Editable) { runTest("composited_selection_bounds_editable.html"); }
TEST_F(CompositedSelectionBoundsTest, EditableDiv) { runTest("composited_selection_bounds_editable_div.html"); }
TEST_F(CompositedSelectionBoundsTest, EmptyEditableInput) { runTest("composited_selection_bounds_empty_editable_input.html"); }
TEST_F(CompositedSelectionBoundsTest, EmptyEditableArea) { runTest("composited_selection_bounds_empty_editable_area.html"); }

TEST_P(ParameterizedWebFrameTest, CompositedSelectionBoundsCleared)
{
    RuntimeEnabledFeatures::setCompositedSelectionUpdateEnabled(true);

    registerMockedHttpURLLoad("select_range_basic.html");
    registerMockedHttpURLLoad("select_range_scroll.html");

    int viewWidth = 500;
    int viewHeight = 500;

    CompositedSelectionBoundsTestWebViewClient fakeSelectionWebViewClient;
    CompositedSelectionBoundsTestLayerTreeView& fakeSelectionLayerTreeView = fakeSelectionWebViewClient.selectionLayerTreeView();

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, 0, &fakeSelectionWebViewClient);
    webViewHelper.webView()->settings()->setDefaultFontSize(12);
    webViewHelper.webView()->setDefaultPageScaleLimits(1, 1);
    webViewHelper.webView()->resize(WebSize(viewWidth, viewHeight));
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "select_range_basic.html");

    // The frame starts with a non-empty selection.
    WebFrame* frame = webViewHelper.webView()->mainFrame();
    ASSERT_TRUE(frame->hasSelection());
    EXPECT_FALSE(fakeSelectionLayerTreeView.getAndResetSelectionCleared());

    // The selection cleared notification should be triggered upon layout.
    frame->executeCommand(WebString::fromUTF8("Unselect"));
    ASSERT_FALSE(frame->hasSelection());
    EXPECT_FALSE(fakeSelectionLayerTreeView.getAndResetSelectionCleared());
    webViewHelper.webView()->layout();
    EXPECT_TRUE(fakeSelectionLayerTreeView.getAndResetSelectionCleared());

    frame->executeCommand(WebString::fromUTF8("SelectAll"));
    webViewHelper.webView()->layout();
    ASSERT_TRUE(frame->hasSelection());
    EXPECT_FALSE(fakeSelectionLayerTreeView.getAndResetSelectionCleared());

    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "select_range_scroll.html");
    ASSERT_TRUE(frame->hasSelection());
    EXPECT_FALSE(fakeSelectionLayerTreeView.getAndResetSelectionCleared());

    // Transitions between non-empty selections should not trigger a clearing.
    WebRect startWebRect;
    WebRect endWebRect;
    webViewHelper.webViewImpl()->selectionBounds(startWebRect, endWebRect);
    WebPoint movedEnd(bottomRightMinusOne(endWebRect));
    endWebRect.x -= 20;
    frame->selectRange(topLeft(startWebRect), movedEnd);
    webViewHelper.webView()->layout();
    ASSERT_TRUE(frame->hasSelection());
    EXPECT_FALSE(fakeSelectionLayerTreeView.getAndResetSelectionCleared());

    frame = webViewHelper.webView()->mainFrame();
    frame->executeCommand(WebString::fromUTF8("Unselect"));
    webViewHelper.webView()->layout();
    ASSERT_FALSE(frame->hasSelection());
    EXPECT_TRUE(fakeSelectionLayerTreeView.getAndResetSelectionCleared());
}

class DisambiguationPopupTestWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    bool didTapMultipleTargets(const WebSize&, const WebRect&, const WebVector<WebRect>& targetRects) override
    {
        EXPECT_GE(targetRects.size(), 2u);
        m_triggered = true;
        return true;
    }

    bool triggered() const { return m_triggered; }
    void resetTriggered() { m_triggered = false; }
    bool m_triggered;
};

static WebGestureEvent fatTap(int x, int y)
{
    WebGestureEvent event;
    event.type = WebInputEvent::GestureTap;
    event.x = x;
    event.y = y;
    event.data.tap.width = 50;
    event.data.tap.height = 50;
    return event;
}

TEST_P(ParameterizedWebFrameTest, DisambiguationPopup)
{
    const std::string htmlFile = "disambiguation_popup.html";
    registerMockedHttpURLLoad(htmlFile);

    DisambiguationPopupTestWebViewClient client;

    // Make sure we initialize to minimum scale, even if the window size
    // only becomes available after the load begins.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + htmlFile, true, 0, &client);
    webViewHelper.webView()->resize(WebSize(1000, 1000));
    webViewHelper.webView()->layout();

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(0, 0));
    EXPECT_FALSE(client.triggered());

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(200, 115));
    EXPECT_FALSE(client.triggered());

    for (int i = 0; i <= 46; i++) {
        client.resetTriggered();
        webViewHelper.webView()->handleInputEvent(fatTap(120, 230 + i * 5));

        int j = i % 10;
        if (j >= 7 && j <= 9)
            EXPECT_TRUE(client.triggered());
        else
            EXPECT_FALSE(client.triggered());
    }

    for (int i = 0; i <= 46; i++) {
        client.resetTriggered();
        webViewHelper.webView()->handleInputEvent(fatTap(10 + i * 5, 590));

        int j = i % 10;
        if (j >= 7 && j <= 9)
            EXPECT_TRUE(client.triggered());
        else
            EXPECT_FALSE(client.triggered());
    }

    // The same taps shouldn't trigger didTapMultipleTargets() after disabling the notification for
    // multi-target-tap.
    webViewHelper.webView()->settings()->setMultiTargetTapNotificationEnabled(false);

    for (int i = 0; i <= 46; i++) {
        client.resetTriggered();
        webViewHelper.webView()->handleInputEvent(fatTap(10 + i * 5, 590));
        EXPECT_FALSE(client.triggered());
    }
}

TEST_P(ParameterizedWebFrameTest, DisambiguationPopupNoContainer)
{
    registerMockedHttpURLLoad("disambiguation_popup_no_container.html");

    DisambiguationPopupTestWebViewClient client;

    // Make sure we initialize to minimum scale, even if the window size
    // only becomes available after the load begins.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "disambiguation_popup_no_container.html", true, 0, &client);
    webViewHelper.webView()->resize(WebSize(1000, 1000));
    webViewHelper.webView()->layout();

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(50, 50));
    EXPECT_FALSE(client.triggered());
}

TEST_P(ParameterizedWebFrameTest, DisambiguationPopupMobileSite)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    const std::string htmlFile = "disambiguation_popup_mobile_site.html";
    registerMockedHttpURLLoad(htmlFile);

    DisambiguationPopupTestWebViewClient client;

    // Make sure we initialize to minimum scale, even if the window size
    // only becomes available after the load begins.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + htmlFile, true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(1000, 1000));
    webViewHelper.webView()->layout();

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(0, 0));
    EXPECT_FALSE(client.triggered());

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(200, 115));
    EXPECT_FALSE(client.triggered());

    for (int i = 0; i <= 46; i++) {
        client.resetTriggered();
        webViewHelper.webView()->handleInputEvent(fatTap(120, 230 + i * 5));
        EXPECT_FALSE(client.triggered());
    }

    for (int i = 0; i <= 46; i++) {
        client.resetTriggered();
        webViewHelper.webView()->handleInputEvent(fatTap(10 + i * 5, 590));
        EXPECT_FALSE(client.triggered());
    }
}

TEST_P(ParameterizedWebFrameTest, DisambiguationPopupViewportSite)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    const std::string htmlFile = "disambiguation_popup_viewport_site.html";
    registerMockedHttpURLLoad(htmlFile);

    DisambiguationPopupTestWebViewClient client;

    // Make sure we initialize to minimum scale, even if the window size
    // only becomes available after the load begins.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + htmlFile, true, 0, &client, enableViewportSettings);
    webViewHelper.webView()->resize(WebSize(1000, 1000));
    webViewHelper.webView()->layout();

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(0, 0));
    EXPECT_FALSE(client.triggered());

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(200, 115));
    EXPECT_FALSE(client.triggered());

    for (int i = 0; i <= 46; i++) {
        client.resetTriggered();
        webViewHelper.webView()->handleInputEvent(fatTap(120, 230 + i * 5));
        EXPECT_FALSE(client.triggered());
    }

    for (int i = 0; i <= 46; i++) {
        client.resetTriggered();
        webViewHelper.webView()->handleInputEvent(fatTap(10 + i * 5, 590));
        EXPECT_FALSE(client.triggered());
    }
}

TEST_F(WebFrameTest, DisambiguationPopupPinchViewport)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    const std::string htmlFile = "disambiguation_popup_200_by_800.html";
    registerMockedHttpURLLoad(htmlFile);

    DisambiguationPopupTestWebViewClient client;

    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + htmlFile, true, 0, &client, configureAndroid);

    WebViewImpl* webViewImpl = webViewHelper.webViewImpl();
    ASSERT_TRUE(webViewImpl);
    LocalFrame* frame = webViewImpl->mainFrameImpl()->frame();
    ASSERT_TRUE(frame);

    webViewHelper.webView()->resize(WebSize(100, 200));

    // Scroll main frame to the bottom of the document
    webViewImpl->mainFrame()->setScrollOffset(WebSize(0, 400));
    EXPECT_POINT_EQ(IntPoint(0, 400), frame->view()->scrollPosition());

    webViewImpl->setPageScaleFactor(2.0);

    // Scroll pinch viewport to the top of the main frame.
    PinchViewport& pinchViewport = frame->page()->frameHost().pinchViewport();
    pinchViewport.setLocation(FloatPoint(0, 0));
    EXPECT_FLOAT_POINT_EQ(FloatPoint(0, 0), pinchViewport.location());

    // Tap at the top: there is nothing there.
    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(10, 60));
    EXPECT_FALSE(client.triggered());

    // Scroll pinch viewport to the bottom of the main frame.
    pinchViewport.setLocation(FloatPoint(0, 200));
    EXPECT_FLOAT_POINT_EQ(FloatPoint(0, 200), pinchViewport.location());

    // Now the tap with the same coordinates should hit two elements.
    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(10, 60));
    EXPECT_TRUE(client.triggered());

    // The same tap shouldn't trigger didTapMultipleTargets() after disabling the notification for
    // multi-target-tap.
    webViewHelper.webView()->settings()->setMultiTargetTapNotificationEnabled(false);
    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(10, 60));
    EXPECT_FALSE(client.triggered());
}

TEST_P(ParameterizedWebFrameTest, DisambiguationPopupBlacklist)
{
    const unsigned viewportWidth = 500;
    const unsigned viewportHeight = 1000;
    const unsigned divHeight = 100;
    const std::string htmlFile = "disambiguation_popup_blacklist.html";
    registerMockedHttpURLLoad(htmlFile);

    DisambiguationPopupTestWebViewClient client;

    // Make sure we initialize to minimum scale, even if the window size
    // only becomes available after the load begins.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + htmlFile, true, 0, &client);
    webViewHelper.webView()->resize(WebSize(viewportWidth, viewportHeight));
    webViewHelper.webView()->layout();

    // Click somewhere where the popup shouldn't appear.
    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(viewportWidth / 2, 0));
    EXPECT_FALSE(client.triggered());

    // Click directly in between two container divs with click handlers, with children that don't handle clicks.
    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(viewportWidth / 2, divHeight));
    EXPECT_TRUE(client.triggered());

    // The third div container should be blacklisted if you click on the link it contains.
    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(viewportWidth / 2, divHeight * 3.25));
    EXPECT_FALSE(client.triggered());
}

TEST_P(ParameterizedWebFrameTest, DisambiguationPopupPageScale)
{
    registerMockedHttpURLLoad("disambiguation_popup_page_scale.html");

    DisambiguationPopupTestWebViewClient client;

    // Make sure we initialize to minimum scale, even if the window size
    // only becomes available after the load begins.
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "disambiguation_popup_page_scale.html", true, 0, &client);
    webViewHelper.webView()->resize(WebSize(1000, 1000));
    webViewHelper.webView()->layout();

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(80, 80));
    EXPECT_TRUE(client.triggered());

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(230, 190));
    EXPECT_TRUE(client.triggered());

    webViewHelper.webView()->setPageScaleFactor(3.0f);
    webViewHelper.webView()->layout();

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(240, 240));
    EXPECT_TRUE(client.triggered());

    client.resetTriggered();
    webViewHelper.webView()->handleInputEvent(fatTap(690, 570));
    EXPECT_FALSE(client.triggered());
}

class TestSubstituteDataWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestSubstituteDataWebFrameClient()
        : m_commitCalled(false)
    {
    }

    virtual void didFailProvisionalLoad(WebLocalFrame* frame, const WebURLError& error, WebHistoryCommitType)
    {
        frame->loadHTMLString("This should appear", toKURL("data:text/html,chromewebdata"), error.unreachableURL, true);
    }

    virtual void didCommitProvisionalLoad(WebLocalFrame* frame, const WebHistoryItem&, WebHistoryCommitType)
    {
        if (frame->dataSource()->response().url() != WebURL(URLTestHelpers::toKURL("about:blank")))
            m_commitCalled = true;
    }

    bool commitCalled() const { return m_commitCalled; }

private:
    bool m_commitCalled;
};

TEST_P(ParameterizedWebFrameTest, ReplaceNavigationAfterHistoryNavigation)
{
    TestSubstituteDataWebFrameClient webFrameClient;

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true, &webFrameClient);
    WebFrame* frame = webViewHelper.webView()->mainFrame();

    // Load a url as a history navigation that will return an error. TestSubstituteDataWebFrameClient
    // will start a SubstituteData load in response to the load failure, which should get fully committed.
    // Due to https://bugs.webkit.org/show_bug.cgi?id=91685, FrameLoader::didReceiveData() wasn't getting
    // called in this case, which resulted in the SubstituteData document not getting displayed.
    WebURLError error;
    error.reason = 1337;
    error.domain = "WebFrameTest";
    std::string errorURL = "http://0.0.0.0";
    WebURLResponse response;
    response.initialize();
    response.setURL(URLTestHelpers::toKURL(errorURL));
    response.setMIMEType("text/html");
    response.setHTTPStatusCode(500);
    WebHistoryItem errorHistoryItem;
    errorHistoryItem.initialize();
    errorHistoryItem.setURLString(WebString::fromUTF8(errorURL.c_str(), errorURL.length()));
    Platform::current()->unitTestSupport()->registerMockedErrorURL(URLTestHelpers::toKURL(errorURL), response, error);
    FrameTestHelpers::loadHistoryItem(frame, errorHistoryItem, WebHistoryDifferentDocumentLoad, WebURLRequest::UseProtocolCachePolicy);

    WebString text = frame->contentAsText(std::numeric_limits<size_t>::max());
    EXPECT_EQ("This should appear", text.utf8());
    EXPECT_TRUE(webFrameClient.commitCalled());
}

class TestWillInsertBodyWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestWillInsertBodyWebFrameClient() : m_numBodies(0), m_didLoad(false)
    {
    }

    void didCommitProvisionalLoad(WebLocalFrame*, const WebHistoryItem&, WebHistoryCommitType) override
    {
        m_numBodies = 0;
        m_didLoad = true;
    }

    void didCreateDocumentElement(WebLocalFrame*) override
    {
        EXPECT_EQ(0, m_numBodies);
    }

    void willInsertBody(WebLocalFrame*) override
    {
        m_numBodies++;
    }

    int m_numBodies;
    bool m_didLoad;
};

TEST_P(ParameterizedWebFrameTest, HTMLDocument)
{
    registerMockedHttpURLLoad("clipped-body.html");

    TestWillInsertBodyWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "clipped-body.html", false, &webFrameClient);

    EXPECT_TRUE(webFrameClient.m_didLoad);
    EXPECT_EQ(1, webFrameClient.m_numBodies);
}

TEST_P(ParameterizedWebFrameTest, EmptyDocument)
{
    registerMockedHttpURLLoad("pageserializer/svg/green_rectangle.svg");

    TestWillInsertBodyWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(false, &webFrameClient);

    EXPECT_FALSE(webFrameClient.m_didLoad);
    EXPECT_EQ(1, webFrameClient.m_numBodies); // The empty document that a new frame starts with triggers this.
}

TEST_P(ParameterizedWebFrameTest, MoveCaretSelectionTowardsWindowPointWithNoSelection)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true);
    WebFrame* frame = webViewHelper.webView()->mainFrame();

    // This test passes if this doesn't crash.
    frame->moveCaretSelection(WebPoint(0, 0));
}

class SpellCheckClient : public WebSpellCheckClient {
public:
    explicit SpellCheckClient(uint32_t hash = 0) : m_numberOfTimesChecked(0), m_hash(hash) { }
    virtual ~SpellCheckClient() { }
    void requestCheckingOfText(const WebString&, const WebVector<uint32_t>&, const WebVector<unsigned>&, WebTextCheckingCompletion* completion) override
    {
        ++m_numberOfTimesChecked;
        Vector<WebTextCheckingResult> results;
        const int misspellingStartOffset = 1;
        const int misspellingLength = 8;
        results.append(WebTextCheckingResult(WebTextDecorationTypeSpelling, misspellingStartOffset, misspellingLength, WebString(), m_hash));
        completion->didFinishCheckingText(results);
    }
    int numberOfTimesChecked() const { return m_numberOfTimesChecked; }
private:
    int m_numberOfTimesChecked;
    uint32_t m_hash;
};

TEST_P(ParameterizedWebFrameTest, ReplaceMisspelledRange)
{
    registerMockedHttpURLLoad("spell.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "spell.html");
    SpellCheckClient spellcheck;
    webViewHelper.webView()->setSpellCheckClient(&spellcheck);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    Document* document = frame->frame()->document();
    Element* element = document->getElementById("data");

    webViewHelper.webView()->settings()->setAsynchronousSpellCheckingEnabled(true);
    webViewHelper.webView()->settings()->setUnifiedTextCheckerEnabled(true);
    webViewHelper.webView()->settings()->setEditingBehavior(WebSettings::EditingBehaviorWin);

    element->focus();
    NonThrowableExceptionState exceptionState;
    document->execCommand("InsertText", false, "_wellcome_.", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    const int allTextBeginOffset = 0;
    const int allTextLength = 11;
    frame->selectRange(WebRange::fromDocumentRange(frame, allTextBeginOffset, allTextLength));
    RefPtrWillBeRawPtr<Range> selectionRange = frame->frame()->selection().toNormalizedRange();

    EXPECT_EQ(1, spellcheck.numberOfTimesChecked());
    EXPECT_EQ(1U, document->markers().markersInRange(selectionRange.get(), DocumentMarker::Spelling).size());

    frame->replaceMisspelledRange("welcome");
    EXPECT_EQ("_welcome_.", frame->contentAsText(std::numeric_limits<size_t>::max()).utf8());
}

TEST_P(ParameterizedWebFrameTest, RemoveSpellingMarkers)
{
    registerMockedHttpURLLoad("spell.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "spell.html");
    SpellCheckClient spellcheck;
    webViewHelper.webView()->setSpellCheckClient(&spellcheck);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    Document* document = frame->frame()->document();
    Element* element = document->getElementById("data");

    webViewHelper.webView()->settings()->setAsynchronousSpellCheckingEnabled(true);
    webViewHelper.webView()->settings()->setUnifiedTextCheckerEnabled(true);
    webViewHelper.webView()->settings()->setEditingBehavior(WebSettings::EditingBehaviorWin);

    element->focus();
    NonThrowableExceptionState exceptionState;
    document->execCommand("InsertText", false, "_wellcome_.", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    frame->removeSpellingMarkers();

    const int allTextBeginOffset = 0;
    const int allTextLength = 11;
    frame->selectRange(WebRange::fromDocumentRange(frame, allTextBeginOffset, allTextLength));
    RefPtrWillBeRawPtr<Range> selectionRange = frame->frame()->selection().toNormalizedRange();

    EXPECT_EQ(0U, document->markers().markersInRange(selectionRange.get(), DocumentMarker::Spelling).size());
}

TEST_P(ParameterizedWebFrameTest, RemoveSpellingMarkersUnderWords)
{
    registerMockedHttpURLLoad("spell.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "spell.html");
    SpellCheckClient spellcheck;
    webViewHelper.webView()->setSpellCheckClient(&spellcheck);

    LocalFrame* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame())->frame();
    Document* document = frame->document();
    Element* element = document->getElementById("data");

    webViewHelper.webView()->settings()->setAsynchronousSpellCheckingEnabled(true);
    webViewHelper.webView()->settings()->setUnifiedTextCheckerEnabled(true);
    webViewHelper.webView()->settings()->setEditingBehavior(WebSettings::EditingBehaviorWin);

    element->focus();
    NonThrowableExceptionState exceptionState;
    document->execCommand("InsertText", false, " wellcome ", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    WebVector<uint32_t> documentMarkers1;
    webViewHelper.webView()->spellingMarkers(&documentMarkers1);
    EXPECT_EQ(1U, documentMarkers1.size());

    Vector<String> words;
    words.append("wellcome");
    frame->removeSpellingMarkersUnderWords(words);

    WebVector<uint32_t> documentMarkers2;
    webViewHelper.webView()->spellingMarkers(&documentMarkers2);
    EXPECT_EQ(0U, documentMarkers2.size());
}

TEST_P(ParameterizedWebFrameTest, MarkerHashIdentifiers)
{
    registerMockedHttpURLLoad("spell.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "spell.html");

    static const uint32_t kHash = 42;
    SpellCheckClient spellcheck(kHash);
    webViewHelper.webView()->setSpellCheckClient(&spellcheck);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    Document* document = frame->frame()->document();
    Element* element = document->getElementById("data");

    webViewHelper.webView()->settings()->setAsynchronousSpellCheckingEnabled(true);
    webViewHelper.webView()->settings()->setUnifiedTextCheckerEnabled(true);
    webViewHelper.webView()->settings()->setEditingBehavior(WebSettings::EditingBehaviorWin);

    element->focus();
    NonThrowableExceptionState exceptionState;
    document->execCommand("InsertText", false, "wellcome.", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    WebVector<uint32_t> documentMarkers;
    webViewHelper.webView()->spellingMarkers(&documentMarkers);
    EXPECT_EQ(1U, documentMarkers.size());
    EXPECT_EQ(kHash, documentMarkers[0]);
}

class StubbornSpellCheckClient : public WebSpellCheckClient {
public:
    StubbornSpellCheckClient() : m_completion(0) { }
    virtual ~StubbornSpellCheckClient() { }

    virtual void requestCheckingOfText(
        const WebString&,
        const WebVector<uint32_t>&,
        const WebVector<unsigned>&,
        WebTextCheckingCompletion* completion) override
    {
        m_completion = completion;
    }

    void kickNoResults()
    {
        kick(-1, -1, WebTextDecorationTypeSpelling);
    }

    void kick()
    {
        kick(1, 8, WebTextDecorationTypeSpelling);
    }

    void kickGrammar()
    {
        kick(1, 8, WebTextDecorationTypeGrammar);
    }

    void kickInvisibleSpellcheck()
    {
        kick(1, 8, WebTextDecorationTypeInvisibleSpellcheck);
    }

private:
    void kick(int misspellingStartOffset, int misspellingLength, WebTextDecorationType type)
    {
        if (!m_completion)
            return;
        Vector<WebTextCheckingResult> results;
        if (misspellingStartOffset >= 0 && misspellingLength > 0)
            results.append(WebTextCheckingResult(type, misspellingStartOffset, misspellingLength));
        m_completion->didFinishCheckingText(results);
        m_completion = 0;
    }

    WebTextCheckingCompletion* m_completion;
};

TEST_P(ParameterizedWebFrameTest, SlowSpellcheckMarkerPosition)
{
    registerMockedHttpURLLoad("spell.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "spell.html");

    StubbornSpellCheckClient spellcheck;
    webViewHelper.webView()->setSpellCheckClient(&spellcheck);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    WebInputElement webInputElement = frame->document().getElementById("data").to<WebInputElement>();
    Document* document = frame->frame()->document();
    Element* element = document->getElementById("data");

    webViewHelper.webView()->settings()->setAsynchronousSpellCheckingEnabled(true);
    webViewHelper.webView()->settings()->setUnifiedTextCheckerEnabled(true);
    webViewHelper.webView()->settings()->setEditingBehavior(WebSettings::EditingBehaviorWin);

    element->focus();
    NonThrowableExceptionState exceptionState;
    document->execCommand("InsertText", false, "wellcome ", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    webInputElement.setSelectionRange(0, 0);
    document->execCommand("InsertText", false, "he", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    spellcheck.kick();

    WebVector<uint32_t> documentMarkers;
    webViewHelper.webView()->spellingMarkers(&documentMarkers);
    EXPECT_EQ(0U, documentMarkers.size());
}

// This test verifies that cancelling spelling request does not cause a
// write-after-free when there's no spellcheck client set.
TEST_P(ParameterizedWebFrameTest, CancelSpellingRequestCrash)
{
    registerMockedHttpURLLoad("spell.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "spell.html");
    webViewHelper.webView()->setSpellCheckClient(0);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    Document* document = frame->frame()->document();
    Element* element = document->getElementById("data");

    webViewHelper.webView()->settings()->setAsynchronousSpellCheckingEnabled(true);
    webViewHelper.webView()->settings()->setUnifiedTextCheckerEnabled(true);
    webViewHelper.webView()->settings()->setEditingBehavior(WebSettings::EditingBehaviorWin);

    element->focus();
    frame->frame()->editor().replaceSelectionWithText("A", false, false);
    frame->frame()->spellChecker().cancelCheck();
}

TEST_P(ParameterizedWebFrameTest, SpellcheckResultErasesMarkers)
{
    registerMockedHttpURLLoad("spell.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "spell.html");

    StubbornSpellCheckClient spellcheck;
    webViewHelper.webView()->setSpellCheckClient(&spellcheck);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    WebInputElement webInputElement = frame->document().getElementById("data").to<WebInputElement>();
    Document* document = frame->frame()->document();
    Element* element = document->getElementById("data");

    webViewHelper.webView()->settings()->setAsynchronousSpellCheckingEnabled(true);
    webViewHelper.webView()->settings()->setUnifiedTextCheckerEnabled(true);
    webViewHelper.webView()->settings()->setEditingBehavior(WebSettings::EditingBehaviorWin);

    element->focus();
    NonThrowableExceptionState exceptionState;
    document->execCommand("InsertText", false, "welcome ", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());
    auto range = EphemeralRange::rangeOfContents(*element);
    document->markers().addMarker(range.startPosition(), range.endPosition(), DocumentMarker::Spelling);
    document->markers().addMarker(range.startPosition(), range.endPosition(), DocumentMarker::Grammar);
    document->markers().addMarker(range.startPosition(), range.endPosition(), DocumentMarker::InvisibleSpellcheck);
    EXPECT_EQ(3U, document->markers().markers().size());

    spellcheck.kickNoResults();
    EXPECT_EQ(0U, document->markers().markers().size());
}

TEST_P(ParameterizedWebFrameTest, SpellcheckResultsSavedInDocument)
{
    registerMockedHttpURLLoad("spell.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "spell.html");

    StubbornSpellCheckClient spellcheck;
    webViewHelper.webView()->setSpellCheckClient(&spellcheck);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame());
    WebInputElement webInputElement = frame->document().getElementById("data").to<WebInputElement>();
    Document* document = frame->frame()->document();
    Element* element = document->getElementById("data");

    webViewHelper.webView()->settings()->setAsynchronousSpellCheckingEnabled(true);
    webViewHelper.webView()->settings()->setUnifiedTextCheckerEnabled(true);
    webViewHelper.webView()->settings()->setEditingBehavior(WebSettings::EditingBehaviorWin);

    element->focus();
    NonThrowableExceptionState exceptionState;
    document->execCommand("InsertText", false, "wellcome ", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    spellcheck.kick();
    ASSERT_EQ(1U, document->markers().markers().size());
    ASSERT_NE(static_cast<DocumentMarker*>(0), document->markers().markers()[0]);
    EXPECT_EQ(DocumentMarker::Spelling, document->markers().markers()[0]->type());

    document->execCommand("InsertText", false, "wellcome ", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    spellcheck.kickGrammar();
    ASSERT_EQ(1U, document->markers().markers().size());
    ASSERT_NE(static_cast<DocumentMarker*>(0), document->markers().markers()[0]);
    EXPECT_EQ(DocumentMarker::Grammar, document->markers().markers()[0]->type());

    document->execCommand("InsertText", false, "wellcome ", exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    spellcheck.kickInvisibleSpellcheck();
    ASSERT_EQ(1U, document->markers().markers().size());
    ASSERT_NE(static_cast<DocumentMarker*>(0), document->markers().markers()[0]);
    EXPECT_EQ(DocumentMarker::InvisibleSpellcheck, document->markers().markers()[0]->type());
}

class TestAccessInitialDocumentWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestAccessInitialDocumentWebFrameClient() : m_didAccessInitialDocument(false)
    {
    }

    virtual void didAccessInitialDocument(WebLocalFrame* frame)
    {
        EXPECT_TRUE(!m_didAccessInitialDocument);
        m_didAccessInitialDocument = true;
    }

    bool m_didAccessInitialDocument;
};

TEST_P(ParameterizedWebFrameTest, DidAccessInitialDocumentBody)
{
    // FIXME: Why is this local webViewClient needed instead of the default
    // WebViewHelper one? With out it there's some mysterious crash in the
    // WebViewHelper destructor.
    FrameTestHelpers::TestWebViewClient webViewClient;
    TestAccessInitialDocumentWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, &webFrameClient, &webViewClient);
    runPendingTasks();
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Create another window that will try to access it.
    FrameTestHelpers::WebViewHelper newWebViewHelper(this);
    WebView* newView = newWebViewHelper.initialize(true);
    newView->mainFrame()->setOpener(webViewHelper.webView()->mainFrame());
    runPendingTasks();
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Access the initial document by modifying the body.
    newView->mainFrame()->executeScript(
        WebScriptSource("window.opener.document.body.innerHTML += 'Modified';"));
    runPendingTasks();
    EXPECT_TRUE(webFrameClient.m_didAccessInitialDocument);

    // Access the initial document again, to ensure we don't notify twice.
    newView->mainFrame()->executeScript(
        WebScriptSource("window.opener.document.body.innerHTML += 'Modified';"));
    runPendingTasks();
    EXPECT_TRUE(webFrameClient.m_didAccessInitialDocument);
}

TEST_P(ParameterizedWebFrameTest, DidAccessInitialDocumentNavigator)
{
    // FIXME: Why is this local webViewClient needed instead of the default
    // WebViewHelper one? With out it there's some mysterious crash in the
    // WebViewHelper destructor.
    FrameTestHelpers::TestWebViewClient webViewClient;
    TestAccessInitialDocumentWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, &webFrameClient, &webViewClient);
    runPendingTasks();
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Create another window that will try to access it.
    FrameTestHelpers::WebViewHelper newWebViewHelper(this);
    WebView* newView = newWebViewHelper.initialize(true);
    newView->mainFrame()->setOpener(webViewHelper.webView()->mainFrame());
    runPendingTasks();
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Access the initial document to get to the navigator object.
    newView->mainFrame()->executeScript(
        WebScriptSource("console.log(window.opener.navigator);"));
    runPendingTasks();
    EXPECT_TRUE(webFrameClient.m_didAccessInitialDocument);
}

TEST_P(ParameterizedWebFrameTest, DidAccessInitialDocumentViaJavascriptUrl)
{
    TestAccessInitialDocumentWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, &webFrameClient);
    runPendingTasks();
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Access the initial document from a javascript: URL.
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), "javascript:document.body.appendChild(document.createTextNode('Modified'))");
    EXPECT_TRUE(webFrameClient.m_didAccessInitialDocument);
}

// Fails on the WebKit XP (deps) bot. http://crbug.com/312192
#if OS(WIN)
TEST_P(ParameterizedWebFrameTest, DISABLED_DidAccessInitialDocumentBodyBeforeModalDialog)
#else
TEST_P(ParameterizedWebFrameTest, DidAccessInitialDocumentBodyBeforeModalDialog)
#endif
{
    // FIXME: Why is this local webViewClient needed instead of the default
    // WebViewHelper one? With out it there's some mysterious crash in the
    // WebViewHelper destructor.
    FrameTestHelpers::TestWebViewClient webViewClient;
    TestAccessInitialDocumentWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, &webFrameClient, &webViewClient);
    runPendingTasks();
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Create another window that will try to access it.
    FrameTestHelpers::WebViewHelper newWebViewHelper(this);
    WebView* newView = newWebViewHelper.initialize(true);
    newView->mainFrame()->setOpener(webViewHelper.webView()->mainFrame());
    runPendingTasks();
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Access the initial document by modifying the body. We normally set a
    // timer to notify the client.
    newView->mainFrame()->executeScript(
        WebScriptSource("window.opener.document.body.innerHTML += 'Modified';"));
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Make sure that a modal dialog forces us to notify right away.
    newView->mainFrame()->executeScript(
        WebScriptSource("window.opener.confirm('Modal');"));
    EXPECT_TRUE(webFrameClient.m_didAccessInitialDocument);

    // Ensure that we don't notify again later.
    runPendingTasks();
    EXPECT_TRUE(webFrameClient.m_didAccessInitialDocument);
}

// Fails on the WebKit XP (deps) bot. http://crbug.com/312192
#if OS(WIN)
TEST_P(ParameterizedWebFrameTest, DISABLED_DidWriteToInitialDocumentBeforeModalDialog)
#else
TEST_P(ParameterizedWebFrameTest, DidWriteToInitialDocumentBeforeModalDialog)
#endif
{
    // FIXME: Why is this local webViewClient needed instead of the default
    // WebViewHelper one? With out it there's some mysterious crash in the
    // WebViewHelper destructor.
    FrameTestHelpers::TestWebViewClient webViewClient;
    TestAccessInitialDocumentWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize(true, &webFrameClient, &webViewClient);
    runPendingTasks();
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Create another window that will try to access it.
    FrameTestHelpers::WebViewHelper newWebViewHelper(this);
    WebView* newView = newWebViewHelper.initialize(true);
    newView->mainFrame()->setOpener(webViewHelper.webView()->mainFrame());
    runPendingTasks();
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Access the initial document with document.write, which moves us past the
    // initial empty document state of the state machine. We normally set a
    // timer to notify the client.
    newView->mainFrame()->executeScript(
        WebScriptSource("window.opener.document.write('Modified'); window.opener.document.close();"));
    EXPECT_FALSE(webFrameClient.m_didAccessInitialDocument);

    // Make sure that a modal dialog forces us to notify right away.
    newView->mainFrame()->executeScript(
        WebScriptSource("window.opener.confirm('Modal');"));
    EXPECT_TRUE(webFrameClient.m_didAccessInitialDocument);

    // Ensure that we don't notify again later.
    runPendingTasks();
    EXPECT_TRUE(webFrameClient.m_didAccessInitialDocument);
}

class TestScrolledFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestScrolledFrameClient() { reset(); }
    void reset()
    {
        m_didScrollFrame = false;
    }
    bool wasFrameScrolled() const { return m_didScrollFrame; }

    // WebFrameClient:
    void didChangeScrollOffset(WebLocalFrame* frame) override
    {
        if (frame->parent())
            return;
        EXPECT_FALSE(m_didScrollFrame);
        FrameView* view = toWebLocalFrameImpl(frame)->frameView();
        // FrameView can be scrolled in FrameView::setFixedVisibleContentRect
        // which is called from LocalFrame::createView (before the frame is associated
        // with the the view).
        if (view)
            m_didScrollFrame = true;
    }
private:
    bool m_didScrollFrame;
};

TEST_F(WebFrameTest, CompositorScrollIsUserScrollLongPage)
{
    registerMockedHttpURLLoad("long_scroll.html");
    TestScrolledFrameClient client;

    // Make sure we initialize to minimum scale, even if the window size
    // only becomes available after the load begins.
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "long_scroll.html", true, &client);
    webViewHelper.webView()->resize(WebSize(1000, 1000));
    webViewHelper.webView()->layout();

    WebLocalFrameImpl* frameImpl = webViewHelper.webViewImpl()->mainFrameImpl();
    FrameView* view = frameImpl->frameView();

    EXPECT_FALSE(client.wasFrameScrolled());
    EXPECT_FALSE(view->wasScrolledByUser());

    // Do a compositor scroll, verify that this is counted as a user scroll.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(0, 1), WebFloatSize(), 1.7f, 0);
    EXPECT_TRUE(client.wasFrameScrolled());
    EXPECT_TRUE(view->wasScrolledByUser());

    client.reset();
    view->setWasScrolledByUser(false);

    // The page scale 1.0f and scroll.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(0, 1), WebFloatSize(), 1.0f, 0);
    EXPECT_TRUE(client.wasFrameScrolled());
    EXPECT_TRUE(view->wasScrolledByUser());
    client.reset();
    view->setWasScrolledByUser(false);

    // No scroll event if there is no scroll delta.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.0f, 0);
    EXPECT_FALSE(client.wasFrameScrolled());
    EXPECT_FALSE(view->wasScrolledByUser());
    client.reset();

    // Non zero page scale and scroll.
    webViewHelper.webViewImpl()->applyViewportDeltas(WebFloatSize(), WebFloatSize(9, 13), WebFloatSize(), 0.6f, 0);
    EXPECT_TRUE(client.wasFrameScrolled());
    EXPECT_TRUE(view->wasScrolledByUser());
    client.reset();
    view->setWasScrolledByUser(false);

    // Programmatic scroll.
    frameImpl->executeScript(WebScriptSource("window.scrollTo(0, 20);"));
    EXPECT_TRUE(client.wasFrameScrolled());
    EXPECT_FALSE(view->wasScrolledByUser());
    client.reset();

    // Programmatic scroll to same offset. No scroll event should be generated.
    frameImpl->executeScript(WebScriptSource("window.scrollTo(0, 20);"));
    EXPECT_FALSE(client.wasFrameScrolled());
    EXPECT_FALSE(view->wasScrolledByUser());
    client.reset();
}

TEST_P(ParameterizedWebFrameTest, FirstPartyForCookiesForRedirect)
{
    WTF::String filePath = Platform::current()->unitTestSupport()->webKitRootDir();
    filePath.append("/Source/web/tests/data/first_party.html");

    WebURL testURL(toKURL("http://internal.test/first_party_redirect.html"));
    char redirect[] = "http://internal.test/first_party.html";
    WebURL redirectURL(toKURL(redirect));
    WebURLResponse redirectResponse;
    redirectResponse.initialize();
    redirectResponse.setMIMEType("text/html");
    redirectResponse.setHTTPStatusCode(302);
    redirectResponse.setHTTPHeaderField("Location", redirect);
    Platform::current()->unitTestSupport()->registerMockedURL(testURL, redirectResponse, filePath);

    WebURLResponse finalResponse;
    finalResponse.initialize();
    finalResponse.setMIMEType("text/html");
    Platform::current()->unitTestSupport()->registerMockedURL(redirectURL, finalResponse, filePath);

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "first_party_redirect.html", true);
    EXPECT_TRUE(webViewHelper.webView()->mainFrame()->document().firstPartyForCookies() == redirectURL);
}

class TestNavigationPolicyWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:

    void didNavigateWithinPage(WebLocalFrame*, const WebHistoryItem&, WebHistoryCommitType) override
    {
        EXPECT_TRUE(false);
    }
};

TEST_P(ParameterizedWebFrameTest, SimulateFragmentAnchorMiddleClick)
{
    registerMockedHttpURLLoad("fragment_middle_click.html");
    TestNavigationPolicyWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fragment_middle_click.html", true, &client);

    Document* document = toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->document();
    KURL destination = document->url();
    destination.setFragmentIdentifier("test");

    RefPtrWillBeRawPtr<Event> event = MouseEvent::create(EventTypeNames::click, false, false,
        document->domWindow(), 0, 0, 0, 0, 0, 0, 0, false, false, false, false, 1, 0, nullptr, nullptr);
    FrameLoadRequest frameRequest(document, ResourceRequest(destination));
    frameRequest.setTriggeringEvent(event);
    toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->loader().load(frameRequest);
}

class TestNewWindowWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    virtual WebView* createView(WebLocalFrame*, const WebURLRequest&, const WebWindowFeatures&,
        const WebString&, WebNavigationPolicy, bool) override
    {
        EXPECT_TRUE(false);
        return 0;
    }
};

class TestNewWindowWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestNewWindowWebFrameClient()
        : m_decidePolicyCallCount(0)
    {
    }

    WebNavigationPolicy decidePolicyForNavigation(const NavigationPolicyInfo& info) override
    {
        m_decidePolicyCallCount++;
        return info.defaultPolicy;
    }

    int decidePolicyCallCount() const { return m_decidePolicyCallCount; }

private:
    int m_decidePolicyCallCount;
};

TEST_P(ParameterizedWebFrameTest, ModifiedClickNewWindow)
{
    registerMockedHttpURLLoad("ctrl_click.html");
    registerMockedHttpURLLoad("hello_world.html");
    TestNewWindowWebViewClient webViewClient;
    TestNewWindowWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "ctrl_click.html", true, &webFrameClient, &webViewClient);

    Document* document = toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->document();
    KURL destination = toKURL(m_baseURL + "hello_world.html");

    // ctrl+click event
    RefPtrWillBeRawPtr<Event> event = MouseEvent::create(EventTypeNames::click, false, false,
        document->domWindow(), 0, 0, 0, 0, 0, 0, 0, true, false, false, false, 0, 0, nullptr, nullptr);
    FrameLoadRequest frameRequest(document, ResourceRequest(destination));
    frameRequest.setTriggeringEvent(event);
    UserGestureIndicator gesture(DefinitelyProcessingUserGesture);
    toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->loader().load(frameRequest);
    FrameTestHelpers::pumpPendingRequestsDoNotUse(webViewHelper.webView()->mainFrame());

    // decidePolicyForNavigation should be called both for the original request and the ctrl+click.
    EXPECT_EQ(2, webFrameClient.decidePolicyCallCount());
}

TEST_P(ParameterizedWebFrameTest, BackToReload)
{
    registerMockedHttpURLLoad("fragment_middle_click.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fragment_middle_click.html", true);
    WebFrame* frame = webViewHelper.webView()->mainFrame();
    const FrameLoader& mainFrameLoader = webViewHelper.webViewImpl()->mainFrameImpl()->frame()->loader();
    RefPtrWillBePersistent<HistoryItem> firstItem = mainFrameLoader.currentItem();
    EXPECT_TRUE(firstItem);

    registerMockedHttpURLLoad("white-1x1.png");
    FrameTestHelpers::loadFrame(frame, m_baseURL + "white-1x1.png");
    EXPECT_NE(firstItem.get(), mainFrameLoader.currentItem());

    FrameTestHelpers::loadHistoryItem(frame, WebHistoryItem(firstItem.get()), WebHistoryDifferentDocumentLoad, WebURLRequest::UseProtocolCachePolicy);
    EXPECT_EQ(firstItem.get(), mainFrameLoader.currentItem());

    FrameTestHelpers::reloadFrame(frame);
    EXPECT_EQ(WebURLRequest::ReloadIgnoringCacheData, frame->dataSource()->request().cachePolicy());
}

TEST_P(ParameterizedWebFrameTest, BackDuringChildFrameReload)
{
    registerMockedHttpURLLoad("page_with_blank_iframe.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "page_with_blank_iframe.html", true);
    WebFrame* mainFrame = webViewHelper.webView()->mainFrame();
    const FrameLoader& mainFrameLoader = webViewHelper.webViewImpl()->mainFrameImpl()->frame()->loader();
    WebFrame* childFrame = mainFrame->firstChild();
    ASSERT_TRUE(childFrame);

    // Start a history navigation, then have a different frame commit a navigation.
    // In this case, reload an about:blank frame, which will commit synchronously.
    // After the history navigation completes, both the appropriate document url and
    // the current history item should reflect the history navigation.
    registerMockedHttpURLLoad("white-1x1.png");
    WebHistoryItem item;
    item.initialize();
    WebURL historyURL(toKURL(m_baseURL + "white-1x1.png"));
    item.setURLString(historyURL.string());
    mainFrame->loadHistoryItem(item, WebHistoryDifferentDocumentLoad, WebURLRequest::UseProtocolCachePolicy);

    FrameTestHelpers::reloadFrame(childFrame);
    EXPECT_EQ(item.urlString(), mainFrame->document().url().string());
    EXPECT_EQ(item.urlString(), WebString(mainFrameLoader.currentItem()->urlString()));
}

TEST_P(ParameterizedWebFrameTest, ReloadPost)
{
    registerMockedHttpURLLoad("reload_post.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "reload_post.html", true);
    WebFrame* frame = webViewHelper.webView()->mainFrame();

    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), "javascript:document.forms[0].submit()");
    // Pump requests one more time after the javascript URL has executed to
    // trigger the actual POST load request.
    FrameTestHelpers::pumpPendingRequestsDoNotUse(webViewHelper.webView()->mainFrame());
    EXPECT_EQ(WebString::fromUTF8("POST"), frame->dataSource()->request().httpMethod());

    FrameTestHelpers::reloadFrame(frame);
    EXPECT_EQ(WebURLRequest::ReloadIgnoringCacheData, frame->dataSource()->request().cachePolicy());
    EXPECT_EQ(WebNavigationTypeFormResubmitted, frame->dataSource()->navigationType());
}

TEST_P(ParameterizedWebFrameTest, LoadHistoryItemReload)
{
    registerMockedHttpURLLoad("fragment_middle_click.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fragment_middle_click.html", true);
    WebFrame* frame = webViewHelper.webView()->mainFrame();
    const FrameLoader& mainFrameLoader = webViewHelper.webViewImpl()->mainFrameImpl()->frame()->loader();
    RefPtrWillBePersistent<HistoryItem> firstItem = mainFrameLoader.currentItem();
    EXPECT_TRUE(firstItem);

    registerMockedHttpURLLoad("white-1x1.png");
    FrameTestHelpers::loadFrame(frame, m_baseURL + "white-1x1.png");
    EXPECT_NE(firstItem.get(), mainFrameLoader.currentItem());

    // Cache policy overrides should take.
    FrameTestHelpers::loadHistoryItem(frame, WebHistoryItem(firstItem), WebHistoryDifferentDocumentLoad, WebURLRequest::ReloadIgnoringCacheData);
    EXPECT_EQ(firstItem.get(), mainFrameLoader.currentItem());
    EXPECT_EQ(WebURLRequest::ReloadIgnoringCacheData, frame->dataSource()->request().cachePolicy());
}


class TestCachePolicyWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    explicit TestCachePolicyWebFrameClient(TestCachePolicyWebFrameClient* parentClient)
        : m_parentClient(parentClient)
        , m_policy(WebURLRequest::UseProtocolCachePolicy)
        , m_childClient(0)
        , m_willSendRequestCallCount(0)
        , m_childFrameCreationCount(0)
    {
    }

    void setChildWebFrameClient(TestCachePolicyWebFrameClient* client) { m_childClient = client; }
    WebURLRequest::CachePolicy cachePolicy() const { return m_policy; }
    int willSendRequestCallCount() const { return m_willSendRequestCallCount; }
    int childFrameCreationCount() const { return m_childFrameCreationCount; }

    virtual WebFrame* createChildFrame(WebLocalFrame* parent, WebTreeScopeType scope, const WebString&, WebSandboxFlags)
    {
        ASSERT(m_childClient);
        m_childFrameCreationCount++;
        WebFrame* frame = WebLocalFrame::create(scope, m_childClient);
        parent->appendChild(frame);
        return frame;
    }

    virtual void didStartLoading(bool toDifferentDocument)
    {
        if (m_parentClient) {
            m_parentClient->didStartLoading(toDifferentDocument);
            return;
        }
        TestWebFrameClient::didStartLoading(toDifferentDocument);
    }

    virtual void didStopLoading()
    {
        if (m_parentClient) {
            m_parentClient->didStopLoading();
            return;
        }
        TestWebFrameClient::didStopLoading();
    }

    void willSendRequest(WebLocalFrame* frame, unsigned, WebURLRequest& request, const WebURLResponse&) override
    {
        m_policy = request.cachePolicy();
        m_willSendRequestCallCount++;
    }

private:
    TestCachePolicyWebFrameClient* m_parentClient;

    WebURLRequest::CachePolicy m_policy;
    TestCachePolicyWebFrameClient* m_childClient;
    int m_willSendRequestCallCount;
    int m_childFrameCreationCount;
};

TEST_P(ParameterizedWebFrameTest, ReloadIframe)
{
    registerMockedHttpURLLoad("iframe_reload.html");
    registerMockedHttpURLLoad("visible_iframe.html");
    TestCachePolicyWebFrameClient mainClient(0);
    TestCachePolicyWebFrameClient childClient(&mainClient);
    mainClient.setChildWebFrameClient(&childClient);

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "iframe_reload.html", true, &mainClient);

    WebLocalFrameImpl* mainFrame = webViewHelper.webViewImpl()->mainFrameImpl();
    RefPtrWillBeRawPtr<WebLocalFrameImpl> childFrame = toWebLocalFrameImpl(mainFrame->firstChild());
    ASSERT_EQ(childFrame->client(), &childClient);
    EXPECT_EQ(mainClient.childFrameCreationCount(), 1);
    EXPECT_EQ(childClient.willSendRequestCallCount(), 1);
    EXPECT_EQ(childClient.cachePolicy(), WebURLRequest::UseProtocolCachePolicy);

    FrameTestHelpers::reloadFrame(mainFrame);

    // A new WebFrame should have been created, but the child WebFrameClient should be reused.
    ASSERT_NE(childFrame, toWebLocalFrameImpl(mainFrame->firstChild()));
    ASSERT_EQ(toWebLocalFrameImpl(mainFrame->firstChild())->client(), &childClient);

    EXPECT_EQ(mainClient.childFrameCreationCount(), 2);
    EXPECT_EQ(childClient.willSendRequestCallCount(), 2);
    EXPECT_EQ(childClient.cachePolicy(), WebURLRequest::ReloadIgnoringCacheData);
}

class TestSameDocumentWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestSameDocumentWebFrameClient()
        : m_frameLoadTypeSameSeen(false)
    {
    }

    virtual void willSendRequest(WebLocalFrame* frame, unsigned, WebURLRequest&, const WebURLResponse&)
    {
        if (toWebLocalFrameImpl(frame)->frame()->loader().loadType() == FrameLoadTypeSame)
            m_frameLoadTypeSameSeen = true;
    }

    bool frameLoadTypeSameSeen() const { return m_frameLoadTypeSameSeen; }

private:
    bool m_frameLoadTypeSameSeen;
};

TEST_P(ParameterizedWebFrameTest, NavigateToSame)
{
    registerMockedHttpURLLoad("navigate_to_same.html");
    TestSameDocumentWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "navigate_to_same.html", true, &client);
    EXPECT_FALSE(client.frameLoadTypeSameSeen());

    FrameLoadRequest frameRequest(0, ResourceRequest(toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->document()->url()));
    toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame())->loader().load(frameRequest);
    FrameTestHelpers::pumpPendingRequestsDoNotUse(webViewHelper.webView()->mainFrame());

    EXPECT_TRUE(client.frameLoadTypeSameSeen());
}

class TestSameDocumentWithImageWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestSameDocumentWithImageWebFrameClient()
        : m_numOfImageRequests(0)
    {
    }

    virtual void willSendRequest(WebLocalFrame* frame, unsigned, WebURLRequest& request, const WebURLResponse&)
    {
        if (request.requestContext() == WebURLRequest::RequestContextImage) {
            m_numOfImageRequests++;
            EXPECT_EQ(WebURLRequest::UseProtocolCachePolicy, request.cachePolicy());
        }
    }

    int numOfImageRequests() const { return m_numOfImageRequests; }

private:
    int m_numOfImageRequests;
};

TEST_P(ParameterizedWebFrameTest, NavigateToSameNoConditionalRequestForSubresource)
{
    registerMockedHttpURLLoad("foo_with_image.html");
    registerMockedHttpURLLoad("white-1x1.png");
    TestSameDocumentWithImageWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "foo_with_image.html", true, &client, 0, &configureLoadsImagesAutomatically);

    WebCache::clear();
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "foo_with_image.html");

    EXPECT_EQ(client.numOfImageRequests(), 2);
}

TEST_P(ParameterizedWebFrameTest, WebNodeImageContents)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank", true);
    WebFrame* frame = webViewHelper.webView()->mainFrame();

    static const char bluePNG[] = "<img src=\"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAAAGElEQVQYV2NkYPj/n4EIwDiqEF8oUT94AFIQE/cCn90IAAAAAElFTkSuQmCC\">";

    // Load up the image and test that we can extract the contents.
    KURL testURL = toKURL("about:blank");
    FrameTestHelpers::loadHTMLString(frame, bluePNG, testURL);

    WebNode node = frame->document().body().firstChild();
    EXPECT_TRUE(node.isElementNode());
    WebElement element = node.to<WebElement>();
    WebImage image = element.imageContents();
    ASSERT_FALSE(image.isNull());
    EXPECT_EQ(image.size().width, 10);
    EXPECT_EQ(image.size().height, 10);
//    FIXME: The rest of this test is disabled since the ImageDecodeCache state may be inconsistent when this test runs.
//    crbug.com/266088
//    SkBitmap bitmap = image.getSkBitmap();
//    SkAutoLockPixels locker(bitmap);
//    EXPECT_EQ(bitmap.getColor(0, 0), SK_ColorBLUE);
}

class TestStartStopCallbackWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestStartStopCallbackWebFrameClient()
        : m_startLoadingCount(0)
        , m_stopLoadingCount(0)
        , m_differentDocumentStartCount(0)
    {
    }

    void didStartLoading(bool toDifferentDocument) override
    {
        TestWebFrameClient::didStartLoading(toDifferentDocument);
        m_startLoadingCount++;
        if (toDifferentDocument)
            m_differentDocumentStartCount++;
    }

    void didStopLoading() override
    {
        TestWebFrameClient::didStopLoading();
        m_stopLoadingCount++;
    }

    int startLoadingCount() const { return m_startLoadingCount; }
    int stopLoadingCount() const { return m_stopLoadingCount; }
    int differentDocumentStartCount() const { return m_differentDocumentStartCount; }

private:
    int m_startLoadingCount;
    int m_stopLoadingCount;
    int m_differentDocumentStartCount;
};

TEST_P(ParameterizedWebFrameTest, PushStateStartsAndStops)
{
    registerMockedHttpURLLoad("push_state.html");
    TestStartStopCallbackWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "push_state.html", true, &client);

    EXPECT_EQ(client.startLoadingCount(), 2);
    EXPECT_EQ(client.stopLoadingCount(), 2);
    EXPECT_EQ(client.differentDocumentStartCount(), 1);
}

class TestDidNavigateCommitTypeWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestDidNavigateCommitTypeWebFrameClient()
        : m_lastCommitType(WebHistoryInertCommit)
    {
    }

    void didNavigateWithinPage(WebLocalFrame*, const WebHistoryItem&, WebHistoryCommitType type) override
    {
        m_lastCommitType = type;
    }

    WebHistoryCommitType lastCommitType() const { return m_lastCommitType; }

private:
    WebHistoryCommitType m_lastCommitType;
};

TEST_P(ParameterizedWebFrameTest, SameDocumentHistoryNavigationCommitType)
{
    registerMockedHttpURLLoad("push_state.html");
    TestDidNavigateCommitTypeWebFrameClient client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "push_state.html", true, &client);
    RefPtrWillBePersistent<HistoryItem> item = toLocalFrame(webViewImpl->page()->mainFrame())->loader().currentItem();
    runPendingTasks();

    toLocalFrame(webViewImpl->page()->mainFrame())->loader().load(
        FrameLoadRequest(nullptr, FrameLoader::resourceRequestFromHistoryItem(
            item.get(), UseProtocolCachePolicy)),
        FrameLoadTypeBackForward, item.get(), HistorySameDocumentLoad);
    EXPECT_EQ(WebBackForwardCommit, client.lastCommitType());
}

class TestHistoryWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    TestHistoryWebFrameClient()
    {
        m_replacesCurrentHistoryItem = false;
        m_frame = nullptr;
    }

    void didStartProvisionalLoad(WebLocalFrame* frame, double)
    {
        WebDataSource* ds = frame->provisionalDataSource();
        m_replacesCurrentHistoryItem = ds->replacesCurrentHistoryItem();
        m_frame = frame;
    }

    bool replacesCurrentHistoryItem() { return m_replacesCurrentHistoryItem; }
    WebFrame* frame() { return m_frame; }

private:
    bool m_replacesCurrentHistoryItem;
    WebFrame* m_frame;
};

// Tests that the first navigation in an initially blank subframe will result in
// a history entry being replaced and not a new one being added.
TEST_P(ParameterizedWebFrameTest, FirstBlankSubframeNavigation)
{
    registerMockedHttpURLLoad("history.html");
    registerMockedHttpURLLoad("find.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    TestHistoryWebFrameClient client;
    webViewHelper.initializeAndLoad("about:blank", true, &client);

    WebFrame* frame = webViewHelper.webView()->mainFrame();

    frame->executeScript(WebScriptSource(WebString::fromUTF8(
        "document.body.appendChild(document.createElement('iframe'))")));

    WebFrame* iframe = frame->firstChild();
    ASSERT_EQ(&client, toWebLocalFrameImpl(iframe)->client());
    EXPECT_EQ(iframe, client.frame());

    std::string url1 = m_baseURL + "history.html";
    FrameTestHelpers::loadFrame(iframe, url1);
    EXPECT_EQ(iframe, client.frame());
    EXPECT_EQ(url1, iframe->document().url().string().utf8());
    EXPECT_TRUE(client.replacesCurrentHistoryItem());

    std::string url2 = m_baseURL + "find.html";
    FrameTestHelpers::loadFrame(iframe, url2);
    EXPECT_EQ(iframe, client.frame());
    EXPECT_EQ(url2, iframe->document().url().string().utf8());
    EXPECT_FALSE(client.replacesCurrentHistoryItem());
}

// Tests that a navigation in a frame with a non-blank initial URL will create
// a new history item, unlike the case above.
TEST_P(ParameterizedWebFrameTest, FirstNonBlankSubframeNavigation)
{
    registerMockedHttpURLLoad("history.html");
    registerMockedHttpURLLoad("find.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    TestHistoryWebFrameClient client;
    webViewHelper.initializeAndLoad("about:blank", true, &client);

    WebFrame* frame = webViewHelper.webView()->mainFrame();

    std::string url1 = m_baseURL + "history.html";
    FrameTestHelpers::loadFrame(frame,
        "javascript:var f = document.createElement('iframe'); "
        "f.src = '" + url1 + "';"
        "document.body.appendChild(f)");

    WebFrame* iframe = frame->firstChild();
    EXPECT_EQ(iframe, client.frame());
    EXPECT_EQ(url1, iframe->document().url().string().utf8());

    std::string url2 = m_baseURL + "find.html";
    FrameTestHelpers::loadFrame(iframe, url2);
    EXPECT_EQ(iframe, client.frame());
    EXPECT_EQ(url2, iframe->document().url().string().utf8());
    EXPECT_FALSE(client.replacesCurrentHistoryItem());
}

// Test verifies that layout will change a layer's scrollable attibutes
TEST_F(WebFrameTest, overflowHiddenRewrite)
{
    registerMockedHttpURLLoad("non-scrollable.html");
    OwnPtr<FakeCompositingWebViewClient> fakeCompositingWebViewClient = adoptPtr(new FakeCompositingWebViewClient());
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initialize(true, 0, fakeCompositingWebViewClient.get(), &configueCompositingWebView);

    webViewHelper.webView()->resize(WebSize(100, 100));
    FrameTestHelpers::loadFrame(webViewHelper.webView()->mainFrame(), m_baseURL + "non-scrollable.html");

    DeprecatedPaintLayerCompositor* compositor =  webViewHelper.webViewImpl()->compositor();
    ASSERT_TRUE(compositor->scrollLayer());

    // Verify that the WebLayer is not scrollable initially.
    GraphicsLayer* scrollLayer = compositor->scrollLayer();
    WebLayer* webScrollLayer = scrollLayer->platformLayer();
    ASSERT_FALSE(webScrollLayer->userScrollableHorizontal());
    ASSERT_FALSE(webScrollLayer->userScrollableVertical());

    // Call javascript to make the layer scrollable, and verify it.
    WebLocalFrameImpl* frame = (WebLocalFrameImpl*)webViewHelper.webView()->mainFrame();
    frame->executeScript(WebScriptSource("allowScroll();"));
    webViewHelper.webView()->layout();
    ASSERT_TRUE(webScrollLayer->userScrollableHorizontal());
    ASSERT_TRUE(webScrollLayer->userScrollableVertical());
}

// Test that currentHistoryItem reflects the current page, not the provisional load.
TEST_P(ParameterizedWebFrameTest, CurrentHistoryItem)
{
    registerMockedHttpURLLoad("fixed_layout.html");
    std::string url = m_baseURL + "fixed_layout.html";

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initialize();
    WebFrame* frame = webViewHelper.webView()->mainFrame();
    const FrameLoader& mainFrameLoader = webViewHelper.webViewImpl()->mainFrameImpl()->frame()->loader();
    WebURLRequest request;
    request.initialize();
    request.setURL(toKURL(url));
    frame->loadRequest(request);

    // Before commit, there is no history item.
    EXPECT_FALSE(mainFrameLoader.currentItem());

    FrameTestHelpers::pumpPendingRequestsDoNotUse(frame);

    // After commit, there is.
    HistoryItem* item = mainFrameLoader.currentItem();
    ASSERT_TRUE(item);
    EXPECT_EQ(WTF::String(url.data()), item->urlString());
}

class FailCreateChildFrame : public FrameTestHelpers::TestWebFrameClient {
public:
    FailCreateChildFrame() : m_callCount(0) { }

    WebFrame* createChildFrame(WebLocalFrame* parent, WebTreeScopeType scope, const WebString& frameName, WebSandboxFlags sandboxFlags) override
    {
        ++m_callCount;
        return 0;
    }

    int callCount() const { return m_callCount; }

private:
    int m_callCount;
};

// Test that we don't crash if WebFrameClient::createChildFrame() fails.
TEST_P(ParameterizedWebFrameTest, CreateChildFrameFailure)
{
    registerMockedHttpURLLoad("create_child_frame_fail.html");
    FailCreateChildFrame client;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "create_child_frame_fail.html", true, &client);

    EXPECT_EQ(1, client.callCount());
}

TEST_P(ParameterizedWebFrameTest, fixedPositionInFixedViewport)
{
    UseMockScrollbarSettings mockScrollbarSettings;
    registerMockedHttpURLLoad("fixed-position-in-fixed-viewport.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "fixed-position-in-fixed-viewport.html", true, 0, 0, enableViewportSettings);

    WebView* webView = webViewHelper.webView();
    webView->resize(WebSize(100, 100));

    Document* document = toWebLocalFrameImpl(webView->mainFrame())->frame()->document();
    Element* bottomFixed = document->getElementById("bottom-fixed");
    Element* topBottomFixed = document->getElementById("top-bottom-fixed");
    Element* rightFixed = document->getElementById("right-fixed");
    Element* leftRightFixed = document->getElementById("left-right-fixed");

    // The layout viewport will hit the min-scale limit of 0.25, so it'll be 400x800.
    webView->resize(WebSize(100, 200));
    EXPECT_EQ(800, bottomFixed->offsetTop() + bottomFixed->offsetHeight());
    EXPECT_EQ(800, topBottomFixed->offsetHeight());

    // Now the layout viewport hits the content width limit of 500px so it'll be 500x500.
    webView->resize(WebSize(200, 200));
    EXPECT_EQ(500, rightFixed->offsetLeft() + rightFixed->offsetWidth());
    EXPECT_EQ(500, leftRightFixed->offsetWidth());
}

TEST_P(ParameterizedWebFrameTest, FrameViewMoveWithSetFrameRect)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank");
    webViewHelper.webViewImpl()->resize(WebSize(200, 200));
    webViewHelper.webViewImpl()->layout();

    FrameView* frameView = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    EXPECT_RECT_EQ(IntRect(0, 0, 200, 200), frameView->frameRect());
    frameView->setFrameRect(IntRect(100, 100, 200, 200));
    EXPECT_RECT_EQ(IntRect(100, 100, 200, 200), frameView->frameRect());
}

TEST_F(WebFrameTest, FrameViewScrollAccountsForTopControls)
{
    FakeCompositingWebViewClient client;
    registerMockedHttpURLLoad("long_scroll.html");
    UseMockScrollbarSettings mockScrollbarSettings;
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "long_scroll.html", true, 0, &client, configureAndroid);

    WebViewImpl* webView = webViewHelper.webViewImpl();
    FrameView* frameView = webViewHelper.webViewImpl()->mainFrameImpl()->frameView();

    float topControlsHeight = 40;
    webView->setTopControlsHeight(topControlsHeight, false);
    webView->resize(WebSize(100, 100));
    webView->setPageScaleFactor(2.0f);
    webView->layout();

    webView->mainFrame()->setScrollOffset(WebSize(0, 2000));
    EXPECT_POINT_EQ(IntPoint(0, 1900), IntPoint(frameView->scrollOffset()));

    // Simulate the top controls showing by 20px, thus shrinking the viewport
    // and allowing it to scroll an additional 20px.
    webView->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.0f, 20.0f / topControlsHeight);
    EXPECT_POINT_EQ(IntPoint(0, 1920), frameView->maximumScrollPosition());

    // Show more, make sure the scroll actually gets clamped.
    webView->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.0f, 20.0f / topControlsHeight);
    webView->mainFrame()->setScrollOffset(WebSize(0, 2000));
    EXPECT_POINT_EQ(IntPoint(0, 1940), IntPoint(frameView->scrollOffset()));

    // Hide until there's 10px showing.
    webView->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.0f, -30.0f / topControlsHeight);
    EXPECT_POINT_EQ(IntPoint(0, 1910), frameView->maximumScrollPosition());

    // Simulate a LayoutPart::resize. The frame is resized to accomodate
    // the top controls and Blink's view of the top controls matches that of
    // the CC
    webView->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.0f, 30.0f / topControlsHeight);
    webView->setTopControlsHeight(40.0f, true);
    webView->resize(WebSize(100, 60));
    webView->layout();
    EXPECT_POINT_EQ(IntPoint(0, 1940), frameView->maximumScrollPosition());

    // Now simulate hiding.
    webView->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.0f, -10.0f / topControlsHeight);
    EXPECT_POINT_EQ(IntPoint(0, 1930), frameView->maximumScrollPosition());

    // Reset to original state: 100px widget height, top controls fully hidden.
    webView->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.0f, -30.0f / topControlsHeight);
    webView->setTopControlsHeight(topControlsHeight, false);
    webView->resize(WebSize(100, 100));
    webView->layout();
    EXPECT_POINT_EQ(IntPoint(0, 1900), frameView->maximumScrollPosition());

    // Show the top controls by just 1px, since we're zoomed in to 2X, that
    // should allow an extra 0.5px of scrolling in the pinch viewport. Make
    // sure we're not losing any pixels when applying the adjustment on the
    // main frame.
    webView->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.0f, 1.0f / topControlsHeight);
    EXPECT_POINT_EQ(IntPoint(0, 1901), frameView->maximumScrollPosition());

    webView->applyViewportDeltas(WebFloatSize(), WebFloatSize(), WebFloatSize(), 1.0f, 2.0f / topControlsHeight);
    EXPECT_POINT_EQ(IntPoint(0, 1903), frameView->maximumScrollPosition());
}

TEST_P(ParameterizedWebFrameTest, FullscreenLayerSize)
{
    FakeCompositingWebViewClient client;
    registerMockedHttpURLLoad("fullscreen_div.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    int viewportWidth = 640;
    int viewportHeight = 480;
    client.m_screenInfo.rect.width = viewportWidth;
    client.m_screenInfo.rect.height = viewportHeight;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "fullscreen_div.html", true, 0, &client, configureAndroid);
    webViewImpl->resize(WebSize(viewportWidth, viewportHeight));
    webViewImpl->layout();

    Document* document = toWebLocalFrameImpl(webViewImpl->mainFrame())->frame()->document();
    UserGestureIndicator gesture(DefinitelyProcessingUserGesture);
    Element* divFullscreen = document->getElementById("div1");
    Fullscreen::from(*document).requestFullscreen(*divFullscreen, Fullscreen::PrefixedRequest);
    webViewImpl->didEnterFullScreen();
    webViewImpl->layout();
    ASSERT_TRUE(Fullscreen::isFullScreen(*document));

    // Verify that the element is sized to the viewport.
    LayoutFullScreen* fullscreenLayoutObject = Fullscreen::from(*document).fullScreenLayoutObject();
    EXPECT_EQ(viewportWidth, fullscreenLayoutObject->logicalWidth().toInt());
    EXPECT_EQ(viewportHeight, fullscreenLayoutObject->logicalHeight().toInt());

    // Verify it's updated after a device rotation.
    client.m_screenInfo.rect.width = viewportHeight;
    client.m_screenInfo.rect.height = viewportWidth;
    webViewImpl->resize(WebSize(viewportHeight, viewportWidth));
    webViewImpl->layout();
    EXPECT_EQ(viewportHeight, fullscreenLayoutObject->logicalWidth().toInt());
    EXPECT_EQ(viewportWidth, fullscreenLayoutObject->logicalHeight().toInt());
}

TEST_F(WebFrameTest, FullscreenLayerNonScrollable)
{
    FakeCompositingWebViewClient client;
    registerMockedHttpURLLoad("fullscreen_div.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    int viewportWidth = 640;
    int viewportHeight = 480;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "fullscreen_div.html", true, 0, &client, configureAndroid);
    webViewImpl->resize(WebSize(viewportWidth, viewportHeight));
    webViewImpl->layout();

    Document* document = toWebLocalFrameImpl(webViewImpl->mainFrame())->frame()->document();
    UserGestureIndicator gesture(DefinitelyProcessingUserGesture);
    Element* divFullscreen = document->getElementById("div1");
    Fullscreen::from(*document).requestFullscreen(*divFullscreen, Fullscreen::PrefixedRequest);
    webViewImpl->didEnterFullScreen();
    webViewImpl->layout();

    // Verify that the main frame is nonscrollable.
    ASSERT_TRUE(Fullscreen::isFullScreen(*document));
    WebLayer* webScrollLayer = webViewImpl->compositor()->scrollLayer()->platformLayer();
    ASSERT_FALSE(webScrollLayer->userScrollableHorizontal());
    ASSERT_FALSE(webScrollLayer->userScrollableVertical());

    // Verify that the main frame is scrollable upon exiting fullscreen.
    webViewImpl->didExitFullScreen();
    webViewImpl->layout();
    ASSERT_FALSE(Fullscreen::isFullScreen(*document));
    webScrollLayer = webViewImpl->compositor()->scrollLayer()->platformLayer();
    ASSERT_TRUE(webScrollLayer->userScrollableHorizontal());
    ASSERT_TRUE(webScrollLayer->userScrollableVertical());
}

TEST_P(ParameterizedWebFrameTest, FullscreenMainFrame)
{
    FakeCompositingWebViewClient client;
    registerMockedHttpURLLoad("fullscreen_div.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    int viewportWidth = 640;
    int viewportHeight = 480;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "fullscreen_div.html", true, 0, &client, configureAndroid);
    webViewImpl->resize(WebSize(viewportWidth, viewportHeight));
    webViewImpl->layout();

    Document* document = toWebLocalFrameImpl(webViewImpl->mainFrame())->frame()->document();
    UserGestureIndicator gesture(DefinitelyProcessingUserGesture);
    Fullscreen::from(*document).requestFullscreen(*document->documentElement(), Fullscreen::PrefixedRequest);
    webViewImpl->didEnterFullScreen();
    webViewImpl->layout();

    // Verify that the main frame is still scrollable.
    ASSERT_TRUE(Fullscreen::isFullScreen(*document));
    WebLayer* webScrollLayer = webViewImpl->compositor()->scrollLayer()->platformLayer();
    ASSERT_TRUE(webScrollLayer->scrollable());
    ASSERT_TRUE(webScrollLayer->userScrollableHorizontal());
    ASSERT_TRUE(webScrollLayer->userScrollableVertical());

    // Verify the main frame still behaves correctly after a resize.
    webViewImpl->resize(WebSize(viewportHeight, viewportWidth));
    ASSERT_TRUE(webScrollLayer->scrollable());
    ASSERT_TRUE(webScrollLayer->userScrollableHorizontal());
    ASSERT_TRUE(webScrollLayer->userScrollableVertical());
}

TEST_P(ParameterizedWebFrameTest, FullscreenSubframe)
{
    FakeCompositingWebViewClient client;
    registerMockedHttpURLLoad("fullscreen_iframe.html");
    registerMockedHttpURLLoad("fullscreen_div.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "fullscreen_iframe.html", true, 0, &client, configureAndroid);
    int viewportWidth = 640;
    int viewportHeight = 480;
    client.m_screenInfo.rect.width = viewportWidth;
    client.m_screenInfo.rect.height = viewportHeight;
    webViewImpl->resize(WebSize(viewportWidth, viewportHeight));
    webViewImpl->layout();

    Document* document = toWebLocalFrameImpl(webViewHelper.webView()->mainFrame()->firstChild())->frame()->document();
    UserGestureIndicator gesture(DefinitelyProcessingUserGesture);
    Element* divFullscreen = document->getElementById("div1");
    Fullscreen::from(*document).requestFullscreen(*divFullscreen, Fullscreen::PrefixedRequest);
    webViewImpl->didEnterFullScreen();
    webViewImpl->layout();

    // Verify that the element is sized to the viewport.
    LayoutFullScreen* fullscreenLayoutObject = Fullscreen::from(*document).fullScreenLayoutObject();
    EXPECT_EQ(viewportWidth, fullscreenLayoutObject->logicalWidth().toInt());
    EXPECT_EQ(viewportHeight, fullscreenLayoutObject->logicalHeight().toInt());

    // Verify it's updated after a device rotation.
    client.m_screenInfo.rect.width = viewportHeight;
    client.m_screenInfo.rect.height = viewportWidth;
    webViewImpl->resize(WebSize(viewportHeight, viewportWidth));
    webViewImpl->layout();
    EXPECT_EQ(viewportHeight, fullscreenLayoutObject->logicalWidth().toInt());
    EXPECT_EQ(viewportWidth, fullscreenLayoutObject->logicalHeight().toInt());
}

TEST_F(WebFrameTest, FullscreenMediaStreamVideo)
{
    RuntimeEnabledFeatures::setOverlayFullscreenVideoEnabled(true);
    FakeCompositingWebViewClient client;
    registerMockedHttpURLLoad("fullscreen_video.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "fullscreen_video.html", true, 0, &client, configureAndroid);
    int viewportWidth = 640;
    int viewportHeight = 480;
    client.m_screenInfo.rect.width = viewportWidth;
    client.m_screenInfo.rect.height = viewportHeight;
    webViewImpl->resize(WebSize(viewportWidth, viewportHeight));
    webViewImpl->layout();

    // Fake the video element as MediaStream
    RefPtrWillBeRawPtr<NullExecutionContext> context = adoptRefWillBeNoop(new NullExecutionContext());
    MediaStreamRegistry::registry().registerURL(0, toKURL(m_baseURL + "test.webm"), MediaStream::create(context.get()));
    Document* document = toWebLocalFrameImpl(webViewImpl->mainFrame())->frame()->document();
    UserGestureIndicator gesture(DefinitelyProcessingUserGesture);
    Element* videoFullscreen = document->getElementById("video");
    Fullscreen::from(*document).requestFullscreen(*videoFullscreen, Fullscreen::PrefixedRequest);
    webViewImpl->didEnterFullScreen();
    webViewImpl->layout();

    // Verify that the video layer is visible in fullscreen.
    DeprecatedPaintLayer* paintLayer =  videoFullscreen->layoutObject()->enclosingLayer();
    GraphicsLayer* graphicsLayer = paintLayer->graphicsLayerBacking();
    EXPECT_TRUE(graphicsLayer->contentsAreVisible());
    context->notifyContextDestroyed();
}

TEST_P(ParameterizedWebFrameTest, FullscreenWithTinyViewport)
{
    FakeCompositingWebViewClient client;
    registerMockedHttpURLLoad("viewport-tiny.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "viewport-tiny.html", true, 0, &client, configureAndroid);
    int viewportWidth = 384;
    int viewportHeight = 640;
    client.m_screenInfo.rect.width = viewportWidth;
    client.m_screenInfo.rect.height = viewportHeight;
    webViewImpl->resize(WebSize(viewportWidth, viewportHeight));
    webViewImpl->layout();

    LayoutView* layoutView = webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutView();
    EXPECT_EQ(320, layoutView->logicalWidth().floor());
    EXPECT_EQ(533, layoutView->logicalHeight().floor());
    EXPECT_FLOAT_EQ(1.2, webViewImpl->pageScaleFactor());
    EXPECT_FLOAT_EQ(1.2, webViewImpl->minimumPageScaleFactor());
    EXPECT_FLOAT_EQ(5.0, webViewImpl->maximumPageScaleFactor());

    Document* document = toWebLocalFrameImpl(webViewImpl->mainFrame())->frame()->document();
    UserGestureIndicator gesture(DefinitelyProcessingUserGesture);
    Fullscreen::from(*document).requestFullscreen(*document->documentElement(), Fullscreen::PrefixedRequest);
    webViewImpl->didEnterFullScreen();
    webViewImpl->layout();
    EXPECT_EQ(384, layoutView->logicalWidth().floor());
    EXPECT_EQ(640, layoutView->logicalHeight().floor());
    EXPECT_FLOAT_EQ(1.0, webViewImpl->pageScaleFactor());
    EXPECT_FLOAT_EQ(1.0, webViewImpl->minimumPageScaleFactor());
    EXPECT_FLOAT_EQ(1.0, webViewImpl->maximumPageScaleFactor());

    webViewImpl->didExitFullScreen();
    webViewImpl->layout();
    EXPECT_EQ(320, layoutView->logicalWidth().floor());
    EXPECT_EQ(533, layoutView->logicalHeight().floor());
    EXPECT_FLOAT_EQ(1.2, webViewImpl->pageScaleFactor());
    EXPECT_FLOAT_EQ(1.2, webViewImpl->minimumPageScaleFactor());
    EXPECT_FLOAT_EQ(5.0, webViewImpl->maximumPageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, FullscreenResizeWithTinyViewport)
{
    FakeCompositingWebViewClient client;
    registerMockedHttpURLLoad("viewport-tiny.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "viewport-tiny.html", true, 0, &client, configureAndroid);
    int viewportWidth = 384;
    int viewportHeight = 640;
    client.m_screenInfo.rect.width = viewportWidth;
    client.m_screenInfo.rect.height = viewportHeight;
    webViewImpl->resize(WebSize(viewportWidth, viewportHeight));
    webViewImpl->layout();

    LayoutView* layoutView = webViewHelper.webViewImpl()->mainFrameImpl()->frameView()->layoutView();
    Document* document = toWebLocalFrameImpl(webViewImpl->mainFrame())->frame()->document();
    UserGestureIndicator gesture(DefinitelyProcessingUserGesture);
    Fullscreen::from(*document).requestFullscreen(*document->documentElement(), Fullscreen::PrefixedRequest);
    webViewImpl->didEnterFullScreen();
    webViewImpl->layout();
    EXPECT_EQ(384, layoutView->logicalWidth().floor());
    EXPECT_EQ(640, layoutView->logicalHeight().floor());
    EXPECT_FLOAT_EQ(1.0, webViewImpl->pageScaleFactor());
    EXPECT_FLOAT_EQ(1.0, webViewImpl->minimumPageScaleFactor());
    EXPECT_FLOAT_EQ(1.0, webViewImpl->maximumPageScaleFactor());

    viewportWidth = 640;
    viewportHeight = 384;
    client.m_screenInfo.rect.width = viewportWidth;
    client.m_screenInfo.rect.height = viewportHeight;
    webViewImpl->resize(WebSize(viewportWidth, viewportHeight));
    webViewImpl->layout();
    EXPECT_EQ(640, layoutView->logicalWidth().floor());
    EXPECT_EQ(384, layoutView->logicalHeight().floor());
    EXPECT_FLOAT_EQ(1.0, webViewImpl->pageScaleFactor());
    EXPECT_FLOAT_EQ(1.0, webViewImpl->minimumPageScaleFactor());
    EXPECT_FLOAT_EQ(1.0, webViewImpl->maximumPageScaleFactor());

    webViewImpl->didExitFullScreen();
    webViewImpl->layout();
    EXPECT_EQ(320, layoutView->logicalWidth().floor());
    EXPECT_EQ(192, layoutView->logicalHeight().floor());
    EXPECT_FLOAT_EQ(2, webViewImpl->pageScaleFactor());
    EXPECT_FLOAT_EQ(2, webViewImpl->minimumPageScaleFactor());
    EXPECT_FLOAT_EQ(5.0, webViewImpl->maximumPageScaleFactor());
}

TEST_P(ParameterizedWebFrameTest, LayoutBlockPercentHeightDescendants)
{
    registerMockedHttpURLLoad("percent-height-descendants.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "percent-height-descendants.html");

    WebView* webView = webViewHelper.webView();
    webView->resize(WebSize(800, 800));
    webView->layout();

    Document* document = toWebLocalFrameImpl(webView->mainFrame())->frame()->document();
    LayoutBlock* container = toLayoutBlock(document->getElementById("container")->layoutObject());
    LayoutBox* percentHeightInAnonymous = toLayoutBox(document->getElementById("percent-height-in-anonymous")->layoutObject());
    LayoutBox* percentHeightDirectChild = toLayoutBox(document->getElementById("percent-height-direct-child")->layoutObject());

    EXPECT_TRUE(LayoutBlock::hasPercentHeightDescendant(percentHeightInAnonymous));
    EXPECT_TRUE(LayoutBlock::hasPercentHeightDescendant(percentHeightDirectChild));

    ASSERT_TRUE(container->percentHeightDescendants());
    ASSERT_TRUE(container->hasPercentHeightDescendants());
    EXPECT_EQ(2U, container->percentHeightDescendants()->size());
    EXPECT_TRUE(container->percentHeightDescendants()->contains(percentHeightInAnonymous));
    EXPECT_TRUE(container->percentHeightDescendants()->contains(percentHeightDirectChild));

    LayoutBlock* anonymousBlock = percentHeightInAnonymous->containingBlock();
    EXPECT_TRUE(anonymousBlock->isAnonymous());
    EXPECT_FALSE(anonymousBlock->hasPercentHeightDescendants());
}

TEST_P(ParameterizedWebFrameTest, HasVisibleContentOnVisibleFrames)
{
    registerMockedHttpURLLoad("visible_frames.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "visible_frames.html");
    for (WebFrame* frame = webViewImpl->mainFrameImpl()->traverseNext(false); frame; frame = frame->traverseNext(false)) {
        EXPECT_TRUE(frame->hasVisibleContent());
    }
}

TEST_P(ParameterizedWebFrameTest, HasVisibleContentOnHiddenFrames)
{
    registerMockedHttpURLLoad("hidden_frames.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "hidden_frames.html");
    for (WebFrame* frame = webViewImpl->mainFrameImpl()->traverseNext(false); frame; frame = frame->traverseNext(false)) {
        EXPECT_FALSE(frame->hasVisibleContent());
    }
}

class ManifestChangeWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    ManifestChangeWebFrameClient() : m_manifestChangeCount(0) { }
    void didChangeManifest(WebLocalFrame*) override
    {
        ++m_manifestChangeCount;
    }

    int manifestChangeCount() { return m_manifestChangeCount; }

private:
    int m_manifestChangeCount;
};

TEST_P(ParameterizedWebFrameTest, NotifyManifestChange)
{
    registerMockedHttpURLLoad("link-manifest-change.html");

    ManifestChangeWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "link-manifest-change.html", true, &webFrameClient);

    EXPECT_EQ(14, webFrameClient.manifestChangeCount());
}

static ResourcePtr<Resource> fetchManifest(Document* document, const KURL& url)
{
    FetchRequest fetchRequest = FetchRequest(ResourceRequest(url), FetchInitiatorInfo());
    fetchRequest.mutableResourceRequest().setRequestContext(WebURLRequest::RequestContextManifest);

    return RawResource::fetchSynchronously(fetchRequest, document->fetcher());
}

TEST_P(ParameterizedWebFrameTest, ManifestFetch)
{
    registerMockedHttpURLLoad("foo.html");
    registerMockedHttpURLLoad("link-manifest-fetch.json");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "foo.html");
    Document* document = toWebLocalFrameImpl(webViewHelper.webViewImpl()->mainFrame())->frame()->document();

    ResourcePtr<Resource> resource = fetchManifest(document, toKURL(m_baseURL + "link-manifest-fetch.json"));

    EXPECT_TRUE(resource->isLoaded());
}

TEST_P(ParameterizedWebFrameTest, ManifestCSPFetchAllow)
{
    URLTestHelpers::registerMockedURLLoad(toKURL(m_notBaseURL + "link-manifest-fetch.json"), "link-manifest-fetch.json");
    registerMockedHttpURLLoadWithCSP("foo.html", "manifest-src *");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "foo.html");
    Document* document = toWebLocalFrameImpl(webViewHelper.webViewImpl()->mainFrame())->frame()->document();

    ResourcePtr<Resource> resource = fetchManifest(document, toKURL(m_notBaseURL + "link-manifest-fetch.json"));

    EXPECT_TRUE(resource->isLoaded());
}

TEST_P(ParameterizedWebFrameTest, ManifestCSPFetchSelf)
{
    URLTestHelpers::registerMockedURLLoad(toKURL(m_notBaseURL + "link-manifest-fetch.json"), "link-manifest-fetch.json");
    registerMockedHttpURLLoadWithCSP("foo.html", "manifest-src 'self'");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "foo.html");
    Document* document = toWebLocalFrameImpl(webViewHelper.webViewImpl()->mainFrame())->frame()->document();

    ResourcePtr<Resource> resource = fetchManifest(document, toKURL(m_notBaseURL + "link-manifest-fetch.json"));

    EXPECT_EQ(0, resource.get()); // Fetching resource wasn't allowed.
}

TEST_P(ParameterizedWebFrameTest, ManifestCSPFetchSelfReportOnly)
{
    URLTestHelpers::registerMockedURLLoad(toKURL(m_notBaseURL + "link-manifest-fetch.json"), "link-manifest-fetch.json");
    registerMockedHttpURLLoadWithCSP("foo.html", "manifest-src 'self'", /* report only */ true);

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "foo.html");
    Document* document = toWebLocalFrameImpl(webViewHelper.webViewImpl()->mainFrame())->frame()->document();

    ResourcePtr<Resource> resource = fetchManifest(document, toKURL(m_notBaseURL + "link-manifest-fetch.json"));

    EXPECT_TRUE(resource->isLoaded());
}


class DefaultPresentationChangeWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    DefaultPresentationChangeWebFrameClient() : m_defaultPresentationChangeCount(0) { }
    void didChangeDefaultPresentation(WebLocalFrame*) override
    {
        ++m_defaultPresentationChangeCount;
    }

    int defaultPresentationChangeCount() { return m_defaultPresentationChangeCount; }

private:
    int m_defaultPresentationChangeCount;
};

TEST_P(ParameterizedWebFrameTest, NotifyDefaultPresentationChange)
{
    registerMockedHttpURLLoad("link-presentation-url-change.html");

    DefaultPresentationChangeWebFrameClient webFrameClient;
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "link-presentation-url-change.html", true, &webFrameClient);

    EXPECT_EQ(14, webFrameClient.defaultPresentationChangeCount());
}


TEST_P(ParameterizedWebFrameTest, ReloadBypassingCache)
{
    // Check that a reload ignoring cache on a frame will result in the cache
    // policy of the request being set to ReloadBypassingCache.
    registerMockedHttpURLLoad("foo.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad(m_baseURL + "foo.html", true);
    WebFrame* frame = webViewHelper.webView()->mainFrame();
    FrameTestHelpers::reloadFrameIgnoringCache(frame);
    EXPECT_EQ(WebURLRequest::ReloadBypassingCache, frame->dataSource()->request().cachePolicy());
}

static void nodeImageTestValidation(const IntSize& referenceBitmapSize, DragImage* dragImage)
{
    // Prepare the reference bitmap.
    SkBitmap bitmap;
    bitmap.allocN32Pixels(referenceBitmapSize.width(), referenceBitmapSize.height());
    SkCanvas canvas(bitmap);
    canvas.drawColor(SK_ColorGREEN);

    EXPECT_EQ(referenceBitmapSize.width(), dragImage->size().width());
    EXPECT_EQ(referenceBitmapSize.height(), dragImage->size().height());
    const SkBitmap& dragBitmap = dragImage->bitmap();
    SkAutoLockPixels lockPixel(dragBitmap);
    EXPECT_EQ(0, memcmp(bitmap.getPixels(), dragBitmap.getPixels(), bitmap.getSize()));
}

TEST_P(ParameterizedWebFrameTest, NodeImageTestCSSTransform)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    OwnPtr<DragImage> dragImage = nodeImageTestSetup(&webViewHelper, std::string("case-css-transform"));
    EXPECT_TRUE(dragImage);

    nodeImageTestValidation(IntSize(40, 40), dragImage.get());
}

TEST_P(ParameterizedWebFrameTest, NodeImageTestCSS3DTransform)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    OwnPtr<DragImage> dragImage = nodeImageTestSetup(&webViewHelper, std::string("case-css-3dtransform"));
    EXPECT_TRUE(dragImage);

    nodeImageTestValidation(IntSize(20, 40), dragImage.get());
}

TEST_P(ParameterizedWebFrameTest, NodeImageTestInlineBlock)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    OwnPtr<DragImage> dragImage = nodeImageTestSetup(&webViewHelper, std::string("case-inlineblock"));
    EXPECT_TRUE(dragImage);

    nodeImageTestValidation(IntSize(40, 40), dragImage.get());
}

TEST_P(ParameterizedWebFrameTest, NodeImageTestFloatLeft)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    OwnPtr<DragImage> dragImage = nodeImageTestSetup(&webViewHelper, std::string("case-float-left-overflow-hidden"));
    EXPECT_TRUE(dragImage);

    nodeImageTestValidation(IntSize(40, 40), dragImage.get());
}

// Crashes on Android: http://crbug.com/403804
#if OS(ANDROID)
TEST_P(ParameterizedWebFrameTest, DISABLED_PrintingBasic)
#else
TEST_P(ParameterizedWebFrameTest, PrintingBasic)
#endif
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("data:text/html,Hello, world.");

    WebFrame* frame = webViewHelper.webView()->mainFrame();

    WebPrintParams printParams;
    printParams.printContentArea.width = 500;
    printParams.printContentArea.height = 500;

    int pageCount = frame->printBegin(printParams);
    EXPECT_EQ(1, pageCount);
    frame->printEnd();
}

class ThemeColorTestWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    ThemeColorTestWebFrameClient()
        : m_didNotify(false)
    {
    }

    void reset()
    {
        m_didNotify = false;
    }

    bool didNotify() const
    {
        return m_didNotify;
    }

private:
    virtual void didChangeThemeColor()
    {
        m_didNotify = true;
    }

    bool m_didNotify;
};

TEST_P(ParameterizedWebFrameTest, ThemeColor)
{
    registerMockedHttpURLLoad("theme_color_test.html");
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    ThemeColorTestWebFrameClient client;
    webViewHelper.initializeAndLoad(m_baseURL + "theme_color_test.html", true, &client);
    EXPECT_TRUE(client.didNotify());
    WebLocalFrameImpl* frame = webViewHelper.webViewImpl()->mainFrameImpl();
    EXPECT_EQ(0xff0000ff, frame->document().themeColor());
    // Change color by rgb.
    client.reset();
    frame->executeScript(WebScriptSource("document.getElementById('tc1').setAttribute('content', 'rgb(0, 0, 0)');"));
    EXPECT_TRUE(client.didNotify());
    EXPECT_EQ(0xff000000, frame->document().themeColor());
    // Change color by hsl.
    client.reset();
    frame->executeScript(WebScriptSource("document.getElementById('tc1').setAttribute('content', 'hsl(240,100%, 50%)');"));
    EXPECT_TRUE(client.didNotify());
    EXPECT_EQ(0xff0000ff, frame->document().themeColor());
    // Change of second theme-color meta tag will not change frame's theme
    // color.
    client.reset();
    frame->executeScript(WebScriptSource("document.getElementById('tc2').setAttribute('content', '#00FF00');"));
    EXPECT_TRUE(client.didNotify());
    EXPECT_EQ(0xff0000ff, frame->document().themeColor());
}

// Make sure that an embedder-triggered detach with a remote frame parent
// doesn't leave behind dangling pointers.
TEST_P(ParameterizedWebFrameTest, EmbedderTriggeredDetachWithRemoteMainFrame)
{
    // FIXME: Refactor some of this logic into WebViewHelper to make it easier to
    // write tests with a top-level remote frame.
    FrameTestHelpers::TestWebViewClient viewClient;
    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebView* view = WebView::create(&viewClient);
    view->setMainFrame(remoteClient.frame());
    FrameTestHelpers::TestWebFrameClient childFrameClient;
    WebLocalFrame* childFrame = view->mainFrame()->toWebRemoteFrame()->createLocalChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, &childFrameClient, nullptr);

    // Purposely keep the LocalFrame alive so it's the last thing to be destroyed.
    RefPtrWillBePersistent<Frame> childCoreFrame = toCoreFrame(childFrame);
    view->close();
    childCoreFrame.clear();
}

class WebFrameSwapTest : public WebFrameTest {
protected:
    WebFrameSwapTest()
    {
        registerMockedHttpURLLoad("frame-a-b-c.html");
        registerMockedHttpURLLoad("subframe-a.html");
        registerMockedHttpURLLoad("subframe-b.html");
        registerMockedHttpURLLoad("subframe-c.html");
        registerMockedHttpURLLoad("subframe-hello.html");

        m_webViewHelper.initializeAndLoad(m_baseURL + "frame-a-b-c.html", true);
    }

    void reset() { m_webViewHelper.reset(); }
    WebFrame* mainFrame() const { return m_webViewHelper.webView()->mainFrame(); }

private:
    FrameTestHelpers::WebViewHelper m_webViewHelper;
};

TEST_F(WebFrameSwapTest, SwapMainFrame)
{
    WebRemoteFrame* remoteFrame = WebRemoteFrame::create(WebTreeScopeType::Document, nullptr);
    mainFrame()->swap(remoteFrame);

    FrameTestHelpers::TestWebFrameClient client;
    WebLocalFrame* localFrame = WebLocalFrame::create(WebTreeScopeType::Document, &client);
    localFrame->initializeToReplaceRemoteFrame(remoteFrame, "", WebSandboxFlags::None);
    remoteFrame->swap(localFrame);

    // Finally, make sure an embedder triggered load in the local frame swapped
    // back in works.
    FrameTestHelpers::loadFrame(localFrame, m_baseURL + "subframe-hello.html");
    std::string content = localFrame->contentAsText(1024).utf8();
    EXPECT_EQ("hello", content);

    // Manually reset to break WebViewHelper's dependency on the stack allocated
    // TestWebFrameClient.
    reset();
    remoteFrame->close();
}

void swapAndVerifyFirstChildConsistency(const char* const message, WebFrame* parent, WebFrame* newChild)
{
    SCOPED_TRACE(message);
    parent->firstChild()->swap(newChild);

    EXPECT_EQ(newChild, parent->firstChild());
    EXPECT_EQ(newChild->parent(), parent);
    EXPECT_EQ(newChild, parent->lastChild()->previousSibling()->previousSibling());
    EXPECT_EQ(newChild->nextSibling(), parent->lastChild()->previousSibling());
}

TEST_F(WebFrameSwapTest, SwapFirstChild)
{
    WebRemoteFrame* remoteFrame = WebRemoteFrame::create(WebTreeScopeType::Document, nullptr);
    swapAndVerifyFirstChildConsistency("local->remote", mainFrame(), remoteFrame);

    FrameTestHelpers::TestWebFrameClient client;
    WebLocalFrame* localFrame = WebLocalFrame::create(WebTreeScopeType::Document, &client);
    localFrame->initializeToReplaceRemoteFrame(remoteFrame, "", WebSandboxFlags::None);
    swapAndVerifyFirstChildConsistency("remote->local", mainFrame(), localFrame);

    // FIXME: This almost certainly fires more load events on the iframe element
    // than it should.
    // Finally, make sure an embedder triggered load in the local frame swapped
    // back in works.
    FrameTestHelpers::loadFrame(localFrame, m_baseURL + "subframe-hello.html");
    std::string content = localFrame->contentAsText(1024).utf8();
    EXPECT_EQ("hello", content);

    // Manually reset to break WebViewHelper's dependency on the stack allocated
    // TestWebFrameClient.
    reset();
    remoteFrame->close();
}

void swapAndVerifyMiddleChildConsistency(const char* const message, WebFrame* parent, WebFrame* newChild)
{
    SCOPED_TRACE(message);
    parent->firstChild()->nextSibling()->swap(newChild);

    EXPECT_EQ(newChild, parent->firstChild()->nextSibling());
    EXPECT_EQ(newChild, parent->lastChild()->previousSibling());
    EXPECT_EQ(newChild->parent(), parent);
    EXPECT_EQ(newChild, parent->firstChild()->nextSibling());
    EXPECT_EQ(newChild->previousSibling(), parent->firstChild());
    EXPECT_EQ(newChild, parent->lastChild()->previousSibling());
    EXPECT_EQ(newChild->nextSibling(), parent->lastChild());
}

TEST_F(WebFrameSwapTest, SwapMiddleChild)
{
    WebRemoteFrame* remoteFrame = WebRemoteFrame::create(WebTreeScopeType::Document, nullptr);
    swapAndVerifyMiddleChildConsistency("local->remote", mainFrame(), remoteFrame);

    FrameTestHelpers::TestWebFrameClient client;
    WebLocalFrame* localFrame = WebLocalFrame::create(WebTreeScopeType::Document, &client);
    localFrame->initializeToReplaceRemoteFrame(remoteFrame, "", WebSandboxFlags::None);
    swapAndVerifyMiddleChildConsistency("remote->local", mainFrame(), localFrame);

    // FIXME: This almost certainly fires more load events on the iframe element
    // than it should.
    // Finally, make sure an embedder triggered load in the local frame swapped
    // back in works.
    FrameTestHelpers::loadFrame(localFrame, m_baseURL + "subframe-hello.html");
    std::string content = localFrame->contentAsText(1024).utf8();
    EXPECT_EQ("hello", content);

    // Manually reset to break WebViewHelper's dependency on the stack allocated
    // TestWebFrameClient.
    reset();
    remoteFrame->close();
}

void swapAndVerifyLastChildConsistency(const char* const message, WebFrame* parent, WebFrame* newChild)
{
    SCOPED_TRACE(message);
    parent->lastChild()->swap(newChild);

    EXPECT_EQ(newChild, parent->lastChild());
    EXPECT_EQ(newChild->parent(), parent);
    EXPECT_EQ(newChild, parent->firstChild()->nextSibling()->nextSibling());
    EXPECT_EQ(newChild->previousSibling(), parent->firstChild()->nextSibling());
}

TEST_F(WebFrameSwapTest, SwapLastChild)
{
    WebRemoteFrame* remoteFrame = WebRemoteFrame::create(WebTreeScopeType::Document, nullptr);
    swapAndVerifyLastChildConsistency("local->remote", mainFrame(), remoteFrame);

    FrameTestHelpers::TestWebFrameClient client;
    WebLocalFrame* localFrame = WebLocalFrame::create(WebTreeScopeType::Document, &client);
    localFrame->initializeToReplaceRemoteFrame(remoteFrame, "", WebSandboxFlags::None);
    swapAndVerifyLastChildConsistency("remote->local", mainFrame(), localFrame);

    // FIXME: This almost certainly fires more load events on the iframe element
    // than it should.
    // Finally, make sure an embedder triggered load in the local frame swapped
    // back in works.
    FrameTestHelpers::loadFrame(localFrame, m_baseURL + "subframe-hello.html");
    std::string content = localFrame->contentAsText(1024).utf8();
    EXPECT_EQ("hello", content);

    // Manually reset to break WebViewHelper's dependency on the stack allocated
    // TestWebFrameClient.
    reset();
    remoteFrame->close();
}

void swapAndVerifySubframeConsistency(const char* const message, WebFrame* oldFrame, WebFrame* newFrame)
{
    SCOPED_TRACE(message);

    EXPECT_TRUE(oldFrame->firstChild());
    oldFrame->swap(newFrame);

    EXPECT_FALSE(newFrame->firstChild());
    EXPECT_FALSE(newFrame->lastChild());
}

TEST_F(WebFrameSwapTest, SwapParentShouldDetachChildren)
{
    WebRemoteFrame* remoteFrame = WebRemoteFrame::create(WebTreeScopeType::Document, nullptr);
    WebFrame* targetFrame = mainFrame()->firstChild()->nextSibling();
    EXPECT_TRUE(targetFrame);
    swapAndVerifySubframeConsistency("local->remote", targetFrame, remoteFrame);

    targetFrame = mainFrame()->firstChild()->nextSibling();
    EXPECT_TRUE(targetFrame);

    // Create child frames in the target frame before testing the swap.
    FrameTestHelpers::TestWebRemoteFrameClient remoteFrameClient;
    remoteFrame->createRemoteChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, &remoteFrameClient);

    FrameTestHelpers::TestWebFrameClient client;
    WebLocalFrame* localFrame = WebLocalFrame::create(WebTreeScopeType::Document, &client);
    localFrame->initializeToReplaceRemoteFrame(remoteFrame, "", WebSandboxFlags::None);
    swapAndVerifySubframeConsistency("remote->local", targetFrame, localFrame);

    // FIXME: This almost certainly fires more load events on the iframe element
    // than it should.
    // Finally, make sure an embedder triggered load in the local frame swapped
    // back in works.
    FrameTestHelpers::loadFrame(localFrame, m_baseURL + "subframe-hello.html");
    std::string content = localFrame->contentAsText(1024).utf8();
    EXPECT_EQ("hello", content);

    // Manually reset to break WebViewHelper's dependency on the stack allocated
    // TestWebFrameClient.
    reset();
    remoteFrame->close();
}

TEST_F(WebFrameSwapTest, SwapPreservesGlobalContext)
{
    v8::HandleScope scope(v8::Isolate::GetCurrent());
    v8::Local<v8::Value> windowTop = mainFrame()->executeScriptAndReturnValue(WebScriptSource("window"));
    ASSERT_TRUE(windowTop->IsObject());
    v8::Local<v8::Value> originalWindow = mainFrame()->executeScriptAndReturnValue(WebScriptSource(
        "document.querySelector('#frame2').contentWindow;"));
    ASSERT_TRUE(originalWindow->IsObject());

    // Make sure window reference stays the same when swapping to a remote frame.
    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebRemoteFrame* remoteFrame = remoteClient.frame();
    WebFrame* targetFrame = mainFrame()->firstChild()->nextSibling();
    targetFrame->swap(remoteFrame);
    remoteFrame->setReplicatedOrigin(SecurityOrigin::createUnique());
    v8::Local<v8::Value> remoteWindow = mainFrame()->executeScriptAndReturnValue(WebScriptSource(
        "document.querySelector('#frame2').contentWindow;"));
    EXPECT_TRUE(originalWindow->StrictEquals(remoteWindow));
    // Check that its view is consistent with the world.
    v8::Local<v8::Value> remoteWindowTop = mainFrame()->executeScriptAndReturnValue(WebScriptSource(
        "document.querySelector('#frame2').contentWindow.top;"));
    EXPECT_TRUE(windowTop->StrictEquals(remoteWindowTop));

    // Now check that remote -> local works too, since it goes through a different code path.
    FrameTestHelpers::TestWebFrameClient client;
    WebLocalFrame* localFrame = WebLocalFrame::create(WebTreeScopeType::Document, &client);
    localFrame->initializeToReplaceRemoteFrame(remoteFrame, "", WebSandboxFlags::None);
    remoteFrame->swap(localFrame);
    v8::Local<v8::Value> localWindow = mainFrame()->executeScriptAndReturnValue(WebScriptSource(
        "document.querySelector('#frame2').contentWindow;"));
    EXPECT_TRUE(originalWindow->StrictEquals(localWindow));
    v8::Local<v8::Value> localWindowTop = mainFrame()->executeScriptAndReturnValue(WebScriptSource(
        "document.querySelector('#frame2').contentWindow.top;"));
    EXPECT_TRUE(windowTop->StrictEquals(localWindowTop));

    // Manually reset to break WebViewHelper's dependency on the stack allocated
    // TestWebFrameClient.
    reset();
}

TEST_F(WebFrameSwapTest, SwapInitializesGlobal)
{
    v8::HandleScope scope(v8::Isolate::GetCurrent());

    v8::Local<v8::Value> windowTop = mainFrame()->executeScriptAndReturnValue(WebScriptSource("window"));
    ASSERT_TRUE(windowTop->IsObject());

    v8::Local<v8::Value> lastChild = mainFrame()->executeScriptAndReturnValue(WebScriptSource("saved = window[2]"));
    ASSERT_TRUE(lastChild->IsObject());

    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebRemoteFrame* remoteFrame = remoteClient.frame();
    mainFrame()->lastChild()->swap(remoteFrame);
    remoteFrame->setReplicatedOrigin(SecurityOrigin::createUnique());
    v8::Local<v8::Value> remoteWindowTop = mainFrame()->executeScriptAndReturnValue(WebScriptSource("saved.top"));
    EXPECT_TRUE(remoteWindowTop->IsObject());
    EXPECT_TRUE(windowTop->StrictEquals(remoteWindowTop));

    FrameTestHelpers::TestWebFrameClient client;
    WebLocalFrame* localFrame = WebLocalFrame::create(WebTreeScopeType::Document, &client);
    localFrame->initializeToReplaceRemoteFrame(remoteFrame, "", WebSandboxFlags::None);
    remoteFrame->swap(localFrame);
    v8::Local<v8::Value> localWindowTop = mainFrame()->executeScriptAndReturnValue(WebScriptSource("saved.top"));
    EXPECT_TRUE(localWindowTop->IsObject());
    EXPECT_TRUE(windowTop->StrictEquals(localWindowTop));

    reset();
}

TEST_F(WebFrameSwapTest, RemoteFramesAreIndexable)
{
    v8::HandleScope scope(v8::Isolate::GetCurrent());

    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebRemoteFrame* remoteFrame = remoteClient.frame();
    mainFrame()->lastChild()->swap(remoteFrame);
    remoteFrame->setReplicatedOrigin(SecurityOrigin::createUnique());
    v8::Local<v8::Value> remoteWindow = mainFrame()->executeScriptAndReturnValue(WebScriptSource("window[2]"));
    EXPECT_TRUE(remoteWindow->IsObject());
    v8::Local<v8::Value> windowLength = mainFrame()->executeScriptAndReturnValue(WebScriptSource("window.length"));
    ASSERT_TRUE(windowLength->IsInt32());
    EXPECT_EQ(3, windowLength.As<v8::Int32>()->Value());

    reset();
}

TEST_F(WebFrameSwapTest, RemoteFrameLengthAccess)
{
    v8::HandleScope scope(v8::Isolate::GetCurrent());

    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebRemoteFrame* remoteFrame = remoteClient.frame();
    mainFrame()->lastChild()->swap(remoteFrame);
    remoteFrame->setReplicatedOrigin(SecurityOrigin::createUnique());
    v8::Local<v8::Value> remoteWindowLength = mainFrame()->executeScriptAndReturnValue(WebScriptSource("window[2].length"));
    ASSERT_TRUE(remoteWindowLength->IsInt32());
    EXPECT_EQ(0, remoteWindowLength.As<v8::Int32>()->Value());

    reset();
}

TEST_F(WebFrameSwapTest, RemoteWindowNamedAccess)
{
    v8::HandleScope scope(v8::Isolate::GetCurrent());

    // FIXME: Once OOPIF unit test infrastructure is in place, test that named
    // window access on a remote window works. For now, just test that accessing
    // a named property doesn't crash.
    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebRemoteFrame* remoteFrame = remoteClient.frame();
    mainFrame()->lastChild()->swap(remoteFrame);
    remoteFrame->setReplicatedOrigin(SecurityOrigin::createUnique());
    v8::Local<v8::Value> remoteWindowProperty = mainFrame()->executeScriptAndReturnValue(WebScriptSource("window[2].foo"));
    EXPECT_TRUE(remoteWindowProperty.IsEmpty());

    reset();
}

// TODO(alexmos, dcheng): This test and some other OOPIF tests use
// very little of the test fixture support in WebFrameSwapTest.  We should
// clean these tests up.
TEST_F(WebFrameSwapTest, FramesOfRemoteParentAreIndexable)
{
    v8::HandleScope scope(v8::Isolate::GetCurrent());

    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebRemoteFrame* remoteParentFrame = remoteClient.frame();
    mainFrame()->swap(remoteParentFrame);
    remoteParentFrame->setReplicatedOrigin(SecurityOrigin::createUnique());

    FrameTestHelpers::TestWebFrameClient childFrameClient;
    WebLocalFrame* childFrame = remoteParentFrame->createLocalChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, &childFrameClient, nullptr);
    FrameTestHelpers::loadFrame(childFrame, m_baseURL + "subframe-hello.html");

    v8::Local<v8::Value> window = childFrame->executeScriptAndReturnValue(WebScriptSource("window"));
    v8::Local<v8::Value> childOfRemoteParent = childFrame->executeScriptAndReturnValue(WebScriptSource("parent.frames[0]"));
    EXPECT_TRUE(childOfRemoteParent->IsObject());
    EXPECT_TRUE(window->StrictEquals(childOfRemoteParent));

    v8::Local<v8::Value> windowLength = childFrame->executeScriptAndReturnValue(WebScriptSource("parent.frames.length"));
    ASSERT_TRUE(windowLength->IsInt32());
    EXPECT_EQ(1, windowLength.As<v8::Int32>()->Value());

    // Manually reset to break WebViewHelper's dependency on the stack allocated clients.
    reset();
}

// Check that frames with a remote parent don't crash while accessing window.frameElement.
TEST_F(WebFrameSwapTest, FrameElementInFramesWithRemoteParent)
{
    v8::HandleScope scope(v8::Isolate::GetCurrent());

    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebRemoteFrame* remoteParentFrame = remoteClient.frame();
    mainFrame()->swap(remoteParentFrame);
    remoteParentFrame->setReplicatedOrigin(SecurityOrigin::createUnique());

    FrameTestHelpers::TestWebFrameClient childFrameClient;
    WebLocalFrame* childFrame = remoteParentFrame->createLocalChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, &childFrameClient, nullptr);
    FrameTestHelpers::loadFrame(childFrame, m_baseURL + "subframe-hello.html");

    v8::Local<v8::Value> frameElement = childFrame->executeScriptAndReturnValue(WebScriptSource("window.frameElement"));
    // frameElement shouldn't be accessible cross-origin.
    EXPECT_TRUE(frameElement.IsEmpty());

    // Manually reset to break WebViewHelper's dependency on the stack allocated clients.
    reset();
}

class RemoteToLocalSwapWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    explicit RemoteToLocalSwapWebFrameClient(WebRemoteFrame* remoteFrame)
        : m_historyCommitType(WebHistoryInertCommit)
        , m_remoteFrame(remoteFrame)
    {
    }

    void didCommitProvisionalLoad(WebLocalFrame* frame, const WebHistoryItem&, WebHistoryCommitType historyCommitType) override
    {
        m_historyCommitType = historyCommitType;
        m_remoteFrame->swap(frame);
    }

    WebHistoryCommitType historyCommitType() const { return m_historyCommitType; }

    WebHistoryCommitType m_historyCommitType;
    WebRemoteFrame* m_remoteFrame;
};

// The commit type should be Initial if we are swapping a RemoteFrame to a
// LocalFrame as it is first being created.  This happens when another frame
// exists in the same process, such that we create the RemoteFrame before the
// first navigation occurs.
TEST_F(WebFrameSwapTest, HistoryCommitTypeAfterNewRemoteToLocalSwap)
{
    WebRemoteFrame* remoteFrame = WebRemoteFrame::create(WebTreeScopeType::Document, nullptr);
    WebFrame* targetFrame = mainFrame()->firstChild();
    ASSERT_TRUE(targetFrame);
    targetFrame->swap(remoteFrame);
    ASSERT_TRUE(mainFrame()->firstChild());
    ASSERT_EQ(mainFrame()->firstChild(), remoteFrame);

    RemoteToLocalSwapWebFrameClient client(remoteFrame);
    WebLocalFrame* localFrame = WebLocalFrame::create(WebTreeScopeType::Document, &client);
    localFrame->initializeToReplaceRemoteFrame(remoteFrame, "", WebSandboxFlags::None);
    FrameTestHelpers::loadFrame(localFrame, m_baseURL + "subframe-hello.html");
    EXPECT_EQ(WebInitialCommitInChildFrame, client.historyCommitType());

    // Manually reset to break WebViewHelper's dependency on the stack allocated
    // TestWebFrameClient.
    reset();
    remoteFrame->close();
}

// The commit type should be Standard if we are swapping a RemoteFrame to a
// LocalFrame after commits have already happened in the frame.  The browser
// process will inform us via setCommittedFirstRealLoad.
TEST_F(WebFrameSwapTest, HistoryCommitTypeAfterExistingRemoteToLocalSwap)
{
    WebRemoteFrame* remoteFrame = WebRemoteFrame::create(WebTreeScopeType::Document, nullptr);
    WebFrame* targetFrame = mainFrame()->firstChild();
    ASSERT_TRUE(targetFrame);
    targetFrame->swap(remoteFrame);
    ASSERT_TRUE(mainFrame()->firstChild());
    ASSERT_EQ(mainFrame()->firstChild(), remoteFrame);

    RemoteToLocalSwapWebFrameClient client(remoteFrame);
    WebLocalFrame* localFrame = WebLocalFrame::create(WebTreeScopeType::Document, &client);
    localFrame->initializeToReplaceRemoteFrame(remoteFrame, "", WebSandboxFlags::None);
    localFrame->setCommittedFirstRealLoad();
    FrameTestHelpers::loadFrame(localFrame, m_baseURL + "subframe-hello.html");
    EXPECT_EQ(WebStandardCommit, client.historyCommitType());

    // Manually reset to break WebViewHelper's dependency on the stack allocated
    // TestWebFrameClient.
    reset();
    remoteFrame->close();
}

class RemoteNavigationClient : public FrameTestHelpers::TestWebRemoteFrameClient {
public:
    void navigate(const WebURLRequest& request, bool shouldReplaceCurrentEntry) override
    {
        m_lastRequest = request;
    }

    const WebURLRequest& lastRequest() const { return m_lastRequest; }

private:
    WebURLRequest m_lastRequest;
};

TEST_F(WebFrameSwapTest, NavigateRemoteFrameViaLocation)
{
    RemoteNavigationClient client;
    WebRemoteFrame* remoteFrame = client.frame();
    WebFrame* targetFrame = mainFrame()->firstChild();
    ASSERT_TRUE(targetFrame);
    targetFrame->swap(remoteFrame);
    ASSERT_TRUE(mainFrame()->firstChild());
    ASSERT_EQ(mainFrame()->firstChild(), remoteFrame);

    remoteFrame->setReplicatedOrigin(WebSecurityOrigin::createFromString("http://127.0.0.1"));
    mainFrame()->executeScript(WebScriptSource(
        "document.getElementsByTagName('iframe')[0].contentWindow.location = 'data:text/html,hi'"));
    ASSERT_FALSE(client.lastRequest().isNull());
    EXPECT_EQ(WebURL(toKURL("data:text/html,hi")), client.lastRequest().url());

    // Manually reset to break WebViewHelper's dependency on the stack allocated
    // TestWebFrameClient.
    reset();
}

TEST_F(WebFrameSwapTest, WindowOpenOnRemoteFrame)
{
    RemoteNavigationClient remoteClient;
    WebRemoteFrame* remoteFrame = remoteClient.frame();
    mainFrame()->firstChild()->swap(remoteFrame);
    remoteFrame->setReplicatedOrigin(WebSecurityOrigin::createFromString("http://127.0.0.1"));

    KURL destination = toKURL("data:text/html:destination");
    ASSERT_TRUE(mainFrame()->isWebLocalFrame());
    ASSERT_TRUE(mainFrame()->firstChild()->isWebRemoteFrame());
    LocalDOMWindow* mainWindow = toWebLocalFrameImpl(mainFrame())->frame()->localDOMWindow();
    mainWindow->open(destination.string(), "frame1", "", mainWindow, mainWindow);
    ASSERT_FALSE(remoteClient.lastRequest().isNull());
    EXPECT_EQ(remoteClient.lastRequest().url(), WebURL(destination));

    reset();
}

class RemoteWindowCloseClient : public FrameTestHelpers::TestWebViewClient {
public:
    RemoteWindowCloseClient()
        : m_closed(false)
    {
    }

    void closeWidgetSoon() override
    {
        m_closed = true;
    }

    bool closed() const { return m_closed; }

private:
    bool m_closed;
};

TEST_F(WebFrameTest, WindowOpenRemoteClose)
{
    FrameTestHelpers::WebViewHelper mainWebView;
    mainWebView.initialize(true);

    // Create a remote window that will be closed later in the test.
    RemoteWindowCloseClient viewClient;
    FrameTestHelpers::TestWebRemoteFrameClient frameClient;
    WebRemoteFrame* webRemoteFrame = frameClient.frame();

    WebView* view = WebView::create(&viewClient);
    view->setMainFrame(webRemoteFrame);
    view->mainFrame()->setOpener(mainWebView.webView()->mainFrame());
    webRemoteFrame->setReplicatedOrigin(WebSecurityOrigin::createFromString("http://127.0.0.1"));

    LocalFrame* localFrame = toLocalFrame(toCoreFrame(mainWebView.webView()->mainFrame()));
    RemoteFrame* remoteFrame = toRemoteFrame(toCoreFrame(frameClient.frame()));

    // Attempt to close the window, which should fail as it isn't opened
    // by a script.
    remoteFrame->domWindow()->close(localFrame->document());
    EXPECT_FALSE(viewClient.closed());

    // Marking it as opened by a script should now allow it to be closed.
    remoteFrame->page()->setOpenedByDOM();
    remoteFrame->domWindow()->close(localFrame->document());
    EXPECT_TRUE(viewClient.closed());

    view->close();
}

class CommitTypeWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    explicit CommitTypeWebFrameClient()
        : m_historyCommitType(WebHistoryInertCommit)
    {
    }

    void didCommitProvisionalLoad(WebLocalFrame*, const WebHistoryItem&, WebHistoryCommitType historyCommitType) override
    {
        m_historyCommitType = historyCommitType;
    }

    WebHistoryCommitType historyCommitType() const { return m_historyCommitType; }

private:
    WebHistoryCommitType m_historyCommitType;
};

TEST_P(ParameterizedWebFrameTest, RemoteFrameInitialCommitType)
{
    FrameTestHelpers::TestWebViewClient viewClient;
    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebView* view = WebView::create(&viewClient);
    view->setMainFrame(remoteClient.frame());
    toRemoteFrame(toCoreFrame(view->mainFrame()))->securityContext()->setReplicatedOrigin(SecurityOrigin::create(toKURL(m_baseURL)));

    // If an iframe has a remote main frame, ensure the inital commit is correctly identified as WebInitialCommitInChildFrame.
    CommitTypeWebFrameClient childFrameClient;
    WebLocalFrame* childFrame = view->mainFrame()->toWebRemoteFrame()->createLocalChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, &childFrameClient, nullptr);
    registerMockedHttpURLLoad("foo.html");
    FrameTestHelpers::loadFrame(childFrame, m_baseURL + "foo.html");
    EXPECT_EQ(WebInitialCommitInChildFrame, childFrameClient.historyCommitType());
    view->close();
}

class MockDocumentThreadableLoaderClient : public DocumentThreadableLoaderClient {
public:
    MockDocumentThreadableLoaderClient() : m_failed(false) { }
    void didFail(const ResourceError&) override { m_failed = true;}

    void reset() { m_failed = false; }
    bool failed() { return m_failed; }

    bool m_failed;
};

// FIXME: This would be better as a unittest on DocumentThreadableLoader but it
// requires spin-up of a frame. It may be possible to remove that requirement
// and convert it to a unittest.
TEST_P(ParameterizedWebFrameTest, LoaderOriginAccess)
{
    FrameTestHelpers::WebViewHelper webViewHelper(this);
    webViewHelper.initializeAndLoad("about:blank");

    SchemeRegistry::registerURLSchemeAsDisplayIsolated("chrome");

    // Cross-origin request.
    KURL resourceUrl(ParsedURLString, "chrome://test.pdf");
    registerMockedChromeURLLoad("test.pdf");

    RefPtrWillBeRawPtr<LocalFrame> frame(toLocalFrame(webViewHelper.webViewImpl()->page()->mainFrame()));

    MockDocumentThreadableLoaderClient client;
    ThreadableLoaderOptions options;

    // First try to load the request with regular access. Should fail.
    options.crossOriginRequestPolicy = UseAccessControl;
    ResourceLoaderOptions resourceLoaderOptions;
    DocumentThreadableLoader::loadResourceSynchronously(
        *frame->document(), ResourceRequest(resourceUrl), client, options, resourceLoaderOptions);
    EXPECT_TRUE(client.failed());

    client.reset();
    // Try to load the request with cross origin access. Should succeed.
    options.crossOriginRequestPolicy = AllowCrossOriginRequests;
    DocumentThreadableLoader::loadResourceSynchronously(
        *frame->document(), ResourceRequest(resourceUrl), client, options, resourceLoaderOptions);
    EXPECT_FALSE(client.failed());
}

TEST_P(ParameterizedWebFrameTest, DetachRemoteFrame)
{
    FrameTestHelpers::TestWebViewClient viewClient;
    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebView* view = WebView::create(&viewClient);
    view->setMainFrame(remoteClient.frame());
    FrameTestHelpers::TestWebRemoteFrameClient childFrameClient;
    WebRemoteFrame* childFrame = view->mainFrame()->toWebRemoteFrame()->createRemoteChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, &childFrameClient);
    childFrame->detach();
    view->close();
}

class TestConsoleMessageWebFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    virtual void didAddMessageToConsole(const WebConsoleMessage& message, const WebString& sourceName, unsigned sourceLine, const WebString& stackTrace)
    {
        messages.push_back(message);
    }

    std::vector<WebConsoleMessage> messages;
};

TEST_P(ParameterizedWebFrameTest, CrossDomainAccessErrorsUseCallingWindow)
{
    registerMockedHttpURLLoad("hidden_frames.html");
    registerMockedChromeURLLoad("hello_world.html");

    FrameTestHelpers::WebViewHelper webViewHelper(this);
    TestConsoleMessageWebFrameClient webFrameClient;
    FrameTestHelpers::TestWebViewClient webViewClient;
    webViewHelper.initializeAndLoad(m_baseURL + "hidden_frames.html", true, &webFrameClient, &webViewClient);

    // Create another window with a cross-origin page, and point its opener to
    // first window.
    FrameTestHelpers::WebViewHelper popupWebViewHelper(this);
    TestConsoleMessageWebFrameClient popupWebFrameClient;
    WebView* popupView = popupWebViewHelper.initializeAndLoad(m_chromeURL + "hello_world.html", true, &popupWebFrameClient);
    popupView->mainFrame()->setOpener(webViewHelper.webView()->mainFrame());

    // Attempt a blocked navigation of an opener's subframe, and ensure that
    // the error shows up on the popup (calling) window's console, rather than
    // the target window.
    popupView->mainFrame()->executeScript(WebScriptSource("opener.frames[1].location.href='data:text/html,foo'"));
    EXPECT_TRUE(webFrameClient.messages.empty());
    ASSERT_EQ(1u, popupWebFrameClient.messages.size());
    EXPECT_TRUE(std::string::npos != popupWebFrameClient.messages[0].text.utf8().find("Unsafe JavaScript attempt to initiate navigation"));

    // Try setting a cross-origin iframe element's source to a javascript: URL,
    // and check that this error is also printed on the calling window.
    popupView->mainFrame()->executeScript(WebScriptSource("opener.document.querySelectorAll('iframe')[1].src='javascript:alert()'"));
    EXPECT_TRUE(webFrameClient.messages.empty());
    ASSERT_EQ(2u, popupWebFrameClient.messages.size());
    EXPECT_TRUE(std::string::npos != popupWebFrameClient.messages[1].text.utf8().find("Blocked a frame"));

    // Manually reset to break WebViewHelpers' dependencies on the stack
    // allocated WebFrameClients.
    webViewHelper.reset();
    popupWebViewHelper.reset();
}

TEST_P(ParameterizedWebFrameTest, CreateLocalChildWithPreviousSibling)
{
    FrameTestHelpers::TestWebViewClient viewClient;
    FrameTestHelpers::TestWebRemoteFrameClient remoteClient;
    WebView* view = WebView::create(&viewClient);
    view->setMainFrame(remoteClient.frame());
    WebRemoteFrame* parent = view->mainFrame()->toWebRemoteFrame();

    WebLocalFrame* secondFrame = parent->createLocalChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, nullptr, nullptr);
    WebLocalFrame* fourthFrame = parent->createLocalChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, nullptr, secondFrame);
    WebLocalFrame* thirdFrame = parent->createLocalChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, nullptr, secondFrame);
    WebLocalFrame* firstFrame = parent->createLocalChild(WebTreeScopeType::Document, "", WebSandboxFlags::None, nullptr, nullptr);

    EXPECT_EQ(firstFrame, parent->firstChild());
    EXPECT_EQ(nullptr, firstFrame->previousSibling());
    EXPECT_EQ(secondFrame, firstFrame->nextSibling());

    EXPECT_EQ(firstFrame, secondFrame->previousSibling());
    EXPECT_EQ(thirdFrame, secondFrame->nextSibling());

    EXPECT_EQ(secondFrame, thirdFrame->previousSibling());
    EXPECT_EQ(fourthFrame, thirdFrame->nextSibling());

    EXPECT_EQ(thirdFrame, fourthFrame->previousSibling());
    EXPECT_EQ(nullptr, fourthFrame->nextSibling());
    EXPECT_EQ(fourthFrame, parent->lastChild());

    EXPECT_EQ(parent, firstFrame->parent());
    EXPECT_EQ(parent, secondFrame->parent());
    EXPECT_EQ(parent, thirdFrame->parent());
    EXPECT_EQ(parent, fourthFrame->parent());

    view->close();
}

class OverscrollWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    MOCK_METHOD4(didOverscroll, void(const WebFloatSize&, const WebFloatSize&, const WebFloatPoint&, const WebFloatSize&));
};

class WebFrameOverscrollTest : public WebFrameTest {
protected:
    WebGestureEvent generateEvent(WebInputEvent::Type type, float deltaX = 0.0, float deltaY = 0.0)
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

    void ScrollByWheel(FrameTestHelpers::WebViewHelper* webViewHelper, int windowX, int windowY, float deltaX, float deltaY)
    {
        WebMouseWheelEvent event;
        event.type = WebInputEvent::MouseWheel;
        event.deltaX = deltaX;
        event.deltaY = deltaY;
        event.windowX = windowX;
        event.windowY = windowY;
        event.canScroll = true;
        webViewHelper->webViewImpl()->handleInputEvent(event);
    }

    void ScrollBegin(FrameTestHelpers::WebViewHelper* webViewHelper)
    {
        webViewHelper->webViewImpl()->handleInputEvent(generateEvent(WebInputEvent::GestureScrollBegin));
    }

    void ScrollUpdate(FrameTestHelpers::WebViewHelper* webViewHelper, float deltaX, float deltaY)
    {
        webViewHelper->webViewImpl()->handleInputEvent(generateEvent(WebInputEvent::GestureScrollUpdate, deltaX, deltaY));
    }

    void ScrollEnd(FrameTestHelpers::WebViewHelper* webViewHelper)
    {
        webViewHelper->webViewImpl()->handleInputEvent(generateEvent(WebInputEvent::GestureScrollEnd));
    }
};

TEST_F(WebFrameOverscrollTest, AccumulatedRootOverscrollAndUnsedDeltaValuesOnOverscroll)
{
    OverscrollWebViewClient client;
    registerMockedHttpURLLoad("overscroll/overscroll.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "overscroll/overscroll.html", true, 0, &client, configureAndroid);

    // Calculation of accumulatedRootOverscroll and unusedDelta on multiple scrollUpdate.
    ScrollBegin(&webViewHelper);
    EXPECT_CALL(client, didOverscroll(WebFloatSize(8, 16), WebFloatSize(8, 16), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, -308, -316);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(WebFloatSize(0, 13), WebFloatSize(8, 29), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 0, -13);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(WebFloatSize(20, 13), WebFloatSize(28, 42), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, -20, -13);
    Mock::VerifyAndClearExpectations(&client);

    // Overscroll is not reported.
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0, 1);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 1, 0);
    Mock::VerifyAndClearExpectations(&client);

    // Overscroll is reported.
    EXPECT_CALL(client, didOverscroll(WebFloatSize(0, -701), WebFloatSize(0, -701), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 0, 1000);
    Mock::VerifyAndClearExpectations(&client);

    // Overscroll is not reported.
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollEnd(&webViewHelper);
    Mock::VerifyAndClearExpectations(&client);
}

TEST_F(WebFrameOverscrollTest, AccumulatedOverscrollAndUnusedDeltaValuesOnDifferentAxesOverscroll)
{
    OverscrollWebViewClient client;
    registerMockedHttpURLLoad("overscroll/div-overscroll.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "overscroll/div-overscroll.html", true, 0, &client, configureAndroid);

    ScrollBegin(&webViewHelper);

    // Scroll the Div to the end.
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0, -316);
    Mock::VerifyAndClearExpectations(&client);

    // Now On Scrolling DIV, scroll is bubbled and root layer is over-scrolled.
    EXPECT_CALL(client, didOverscroll(WebFloatSize(0, 100), WebFloatSize(0, 100), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 0, -100);
    Mock::VerifyAndClearExpectations(&client);

    // Page scrolls vertically, but over-scrolls horizontally.
    EXPECT_CALL(client, didOverscroll(WebFloatSize(-100, 0), WebFloatSize(-100, 0), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 100, 50);
    Mock::VerifyAndClearExpectations(&client);

    // Scrolling up, Overscroll is not reported.
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0, -50);
    Mock::VerifyAndClearExpectations(&client);

    // Page scrolls horizontally, but over-scrolls vertically.
    EXPECT_CALL(client, didOverscroll(WebFloatSize(0, 100), WebFloatSize(0, 100), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, -100, -100);
    Mock::VerifyAndClearExpectations(&client);
}

TEST_F(WebFrameOverscrollTest, RootLayerOverscrolledOnInnerDivOverScroll)
{
    OverscrollWebViewClient client;
    registerMockedHttpURLLoad("overscroll/div-overscroll.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "overscroll/div-overscroll.html", true, 0, &client, configureAndroid);

    ScrollBegin(&webViewHelper);

    // Scroll the Div to the end.
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0, -316);
    Mock::VerifyAndClearExpectations(&client);

    // Now On Scrolling DIV, scroll is bubbled and root layer is over-scrolled.
    EXPECT_CALL(client, didOverscroll(WebFloatSize(0, 50), WebFloatSize(0, 50), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 0, -50);
    Mock::VerifyAndClearExpectations(&client);
}

TEST_F(WebFrameOverscrollTest, RootLayerOverscrolledOnInnerIFrameOverScroll)
{
    OverscrollWebViewClient client;
    registerMockedHttpURLLoad("overscroll/iframe-overscroll.html");
    registerMockedHttpURLLoad("overscroll/scrollable-iframe.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "overscroll/iframe-overscroll.html", true, 0, &client, configureAndroid);

    ScrollBegin(&webViewHelper);
    // Scroll the IFrame to the end.
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0, -320);
    Mock::VerifyAndClearExpectations(&client);

    // Now On Scrolling IFrame, scroll is bubbled and root layer is over-scrolled.
    EXPECT_CALL(client, didOverscroll(WebFloatSize(0, 50), WebFloatSize(0, 50), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 0, -50);
    Mock::VerifyAndClearExpectations(&client);
}

TEST_F(WebFrameOverscrollTest, NoOverscrollOnNonScrollableaxes)
{
    OverscrollWebViewClient client;
    registerMockedHttpURLLoad("overscroll/no-overscroll-on-nonscrollable-axes.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "overscroll/no-overscroll-on-nonscrollable-axes.html", true, 0, &client, configureAndroid);

    // Overscroll is not reported in all the directions.
    ScrollBegin(&webViewHelper);
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0, -1);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0, 1);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 1, 0);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, -1, 0);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 1, 1);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, -1, 1);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 1, -1);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, -1, -1);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollEnd(&webViewHelper);
    Mock::VerifyAndClearExpectations(&client);
}

TEST_F(WebFrameOverscrollTest, ScaledPageRootLayerOverscrolled)
{
    OverscrollWebViewClient client;
    registerMockedHttpURLLoad("overscroll/overscroll.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "overscroll/overscroll.html", true, 0, &client, configureAndroid);
    webViewImpl->setPageScaleFactor(3.0);

    // Calculation of accumulatedRootOverscroll and unusedDelta on scaled page.
    ScrollBegin(&webViewHelper);
    EXPECT_CALL(client, didOverscroll(WebFloatSize(0, -10), WebFloatSize(0, -10), WebFloatPoint(33, 33), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 0, 30);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(WebFloatSize(0, -10), WebFloatSize(0, -20), WebFloatPoint(33, 33), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 0, 30);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(WebFloatSize(-10, -10), WebFloatSize(-10, -30), WebFloatPoint(33, 33), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 30, 30);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(WebFloatSize(-10, 0), WebFloatSize(-20, -30), WebFloatPoint(33, 33), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 30, 0);
    Mock::VerifyAndClearExpectations(&client);

    // Overscroll is not reported.
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollEnd(&webViewHelper);
    Mock::VerifyAndClearExpectations(&client);
}

TEST_F(WebFrameOverscrollTest, NoOverscrollForSmallvalues)
{
    OverscrollWebViewClient client;
    registerMockedHttpURLLoad("overscroll/overscroll.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "overscroll/overscroll.html", true, 0, &client, configureAndroid);

    ScrollBegin(&webViewHelper);
    EXPECT_CALL(client, didOverscroll(WebFloatSize(-10, -10), WebFloatSize(-10, -10), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 10, 10);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(WebFloatSize(0, -0.10), WebFloatSize(-10, -10.10), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 0, 0.10);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(WebFloatSize(-0.10, 0), WebFloatSize(-10.10, -10.10), WebFloatPoint(100, 100), WebFloatSize()));
    ScrollUpdate(&webViewHelper, 0.10, 0);
    Mock::VerifyAndClearExpectations(&client);

    // For residual values overscrollDelta should be reset and didOverscroll shouldn't be called.
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0, 0.09);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0.09, 0.09);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0.09, 0);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, 0, -0.09);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, -0.09, -0.09);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollUpdate(&webViewHelper, -0.09, 0);
    Mock::VerifyAndClearExpectations(&client);

    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollEnd(&webViewHelper);
    Mock::VerifyAndClearExpectations(&client);
}

TEST_F(WebFrameOverscrollTest, ReportingLatestOverscrollForElasticOverscroll)
{
    OverscrollWebViewClient client;
    registerMockedHttpURLLoad("overscroll/overscroll.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    webViewHelper.initializeAndLoad(m_baseURL + "overscroll/overscroll.html", true, 0, &client, configureAndroid);

    // On disabling ReportWheelOverscroll, overscroll is not reported on MouseWheel.
    webViewHelper.webView()->settings()->setReportWheelOverscroll(false);
    EXPECT_CALL(client, didOverscroll(_, _, _, _)).Times(0);
    ScrollByWheel(&webViewHelper, 10, 10, 1000, 1000);
    Mock::VerifyAndClearExpectations(&client);

    // On enabling ReportWheelOverscroll, overscroll is reported on MouseWheel.
    webViewHelper.webView()->settings()->setReportWheelOverscroll(true);
    EXPECT_CALL(client, didOverscroll(WebFloatSize(-1000, -1000), WebFloatSize(-1000, -1000), WebFloatPoint(), WebFloatSize()));
    ScrollByWheel(&webViewHelper, 10, 10, 1000, 1000);
    Mock::VerifyAndClearExpectations(&client);
}

TEST_F(WebFrameTest, OrientationFrameDetach)
{
    RuntimeEnabledFeatures::setOrientationEventEnabled(true);
    registerMockedHttpURLLoad("orientation-frame-detach.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "orientation-frame-detach.html", true);
    webViewImpl->mainFrameImpl()->sendOrientationChangeEvent();
}

TEST_F(WebFrameTest, MaxFramesDetach)
{
    registerMockedHttpURLLoad("max-frames-detach.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(m_baseURL + "max-frames-detach.html", true);
    webViewImpl->mainFrameImpl()->collectGarbage();
}

} // namespace blink
