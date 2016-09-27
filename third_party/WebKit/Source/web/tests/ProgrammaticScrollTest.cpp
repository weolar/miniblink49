#include "config.h"

#include "core/frame/FrameView.h"
#include "core/layout/LayoutView.h"
#include "platform/testing/URLTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebFrame.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebHistoryItem.h"
#include "public/web/WebInputEvent.h"
#include "public/web/WebScriptSource.h"
#include "public/web/WebSettings.h"
#include "public/web/WebView.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

class MockWebFrameClient : public WebFrameClient {
};

class ProgrammaticScrollTest : public testing::Test {
public:
    ProgrammaticScrollTest()
        : m_baseURL("http://www.test.com/")
    {
    }

    void TearDown() override
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

protected:

    void registerMockedHttpURLLoad(const std::string& fileName)
    {
        URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(fileName.c_str()));
    }

    std::string m_baseURL;
    MockWebFrameClient m_mockWebFrameClient;
};

TEST_F(ProgrammaticScrollTest, RestoreScrollPositionAndViewStateWithScale)
{
    registerMockedHttpURLLoad("long_scroll.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    WebView* webView = webViewHelper.initializeAndLoad(m_baseURL + "long_scroll.html", true, 0, 0);
    webView->resize(WebSize(1000, 1000));
    webView->layout();

    WebViewImpl* webViewImpl = toWebViewImpl(webView);
    LocalFrame* frame = webViewImpl->mainFrameImpl()->frame();
    frame->loader().setLoadType(FrameLoadTypeBackForward);

    webViewImpl->setPageScaleFactor(3.0f);
    webViewImpl->mainFrame()->setScrollOffset(WebSize(0, 500));
    frame->view()->setWasScrolledByUser(false);
    frame->loader().currentItem()->setPageScaleFactor(2);
    frame->loader().currentItem()->setScrollPoint(WebPoint(0, 200));

    // Flip back the wasScrolledByUser flag which was set to true by setPageScaleFactor
    // because otherwise FrameLoader::restoreScrollPositionAndViewState does nothing.
    frame->view()->setWasScrolledByUser(false);
    frame->loader().restoreScrollPositionAndViewState();

    // Expect that both scroll and scale were restored.
    EXPECT_EQ(2.0f, webViewImpl->pageScaleFactor());
    EXPECT_EQ(200, webViewImpl->mainFrameImpl()->scrollOffset().height);
}

TEST_F(ProgrammaticScrollTest, RestoreScrollPositionAndViewStateWithoutScale)
{
    registerMockedHttpURLLoad("long_scroll.html");

    FrameTestHelpers::WebViewHelper webViewHelper;
    WebView* webView = webViewHelper.initializeAndLoad(m_baseURL + "long_scroll.html", true, 0, 0);
    webView->resize(WebSize(1000, 1000));
    webView->layout();

    WebViewImpl* webViewImpl = toWebViewImpl(webView);
    LocalFrame* frame = webViewImpl->mainFrameImpl()->frame();
    frame->loader().setLoadType(FrameLoadTypeBackForward);

    webViewImpl->setPageScaleFactor(3.0f);
    webViewImpl->mainFrame()->setScrollOffset(WebSize(0, 500));
    frame->view()->setWasScrolledByUser(false);
    frame->loader().currentItem()->setPageScaleFactor(0);
    frame->loader().currentItem()->setScrollPoint(WebPoint(0, 400));

    // FrameLoader::restoreScrollPositionAndViewState flows differently if scale is zero.
    frame->loader().restoreScrollPositionAndViewState();

    // Expect that only the scroll position was restored.
    EXPECT_EQ(3.0f, webViewImpl->pageScaleFactor());
    EXPECT_EQ(400, webViewImpl->mainFrameImpl()->scrollOffset().height);
}

} // namespace blink
