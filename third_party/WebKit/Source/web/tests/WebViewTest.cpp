/*
 * Copyright (C) 2011, 2012 Google Inc. All rights reserved.
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
#include "public/web/WebView.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLIFrameElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/layout/LayoutView.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/page/Page.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/DeprecatedPaintLayerPainter.h"
#include "core/timing/DOMWindowPerformance.h"
#include "core/timing/Performance.h"
#include "core/timing/PerformanceCompositeTiming.h"
#include "platform/KeyboardCodes.h"
#include "platform/UserGestureIndicator.h"
#include "platform/geometry/IntSize.h"
#include "platform/graphics/Color.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/SkPictureBuilder.h"
#include "platform/testing/URLTestHelpers.h"
#include "platform/testing/UnitTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebClipboard.h"
#include "public/platform/WebDisplayMode.h"
#include "public/platform/WebDragData.h"
#include "public/platform/WebSize.h"
#include "public/platform/WebThread.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebAutofillClient.h"
#include "public/web/WebContentDetectionResult.h"
#include "public/web/WebDateTimeChooserCompletion.h"
#include "public/web/WebDeviceEmulationParams.h"
#include "public/web/WebDocument.h"
#include "public/web/WebDragOperation.h"
#include "public/web/WebElement.h"
#include "public/web/WebFrame.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebHitTestResult.h"
#include "public/web/WebInputEvent.h"
#include "public/web/WebScriptSource.h"
#include "public/web/WebSettings.h"
#include "public/web/WebTreeScopeType.h"
#include "public/web/WebViewClient.h"
#include "public/web/WebWidget.h"
#include "public/web/WebWidgetClient.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkBitmapDevice.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebSettingsImpl.h"
#include "web/WebViewImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

using blink::FrameTestHelpers::loadFrame;
using blink::URLTestHelpers::toKURL;
using blink::URLTestHelpers::registerMockedURLLoad;
using blink::testing::runPendingTasks;

namespace blink {

enum HorizontalScrollbarState {
    NoHorizontalScrollbar,
    VisibleHorizontalScrollbar,
};

enum VerticalScrollbarState {
    NoVerticalScrollbar,
    VisibleVerticalScrollbar,
};

class TestData {
public:
    void setWebView(WebView* webView) { m_webView = toWebViewImpl(webView); }
    void setSize(const WebSize& newSize) { m_size = newSize; }
    HorizontalScrollbarState horizontalScrollbarState() const
    {
        return m_webView->hasHorizontalScrollbar() ? VisibleHorizontalScrollbar: NoHorizontalScrollbar;
    }
    VerticalScrollbarState verticalScrollbarState() const
    {
        return m_webView->hasVerticalScrollbar() ? VisibleVerticalScrollbar : NoVerticalScrollbar;
    }
    int width() const { return m_size.width; }
    int height() const { return m_size.height; }

private:
    WebSize m_size;
    WebViewImpl* m_webView;
};

class AutoResizeWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    // WebViewClient methods
    void didAutoResize(const WebSize& newSize) override { m_testData.setSize(newSize); }

    // Local methods
    TestData& testData() { return m_testData; }

private:
    TestData m_testData;
};

class SaveImageFromDataURLWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    // WebViewClient methods
    void saveImageFromDataURL(const WebString& dataURL) override { m_dataURL = dataURL; }

    // Local methods
    const WebString& result() const { return m_dataURL; }
    void reset() { m_dataURL = WebString(); }

private:
    WebString m_dataURL;
};

class TapHandlingWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    // WebViewClient methods
    void didHandleGestureEvent(const WebGestureEvent& event, bool eventCancelled) override
    {
        if (event.type == WebInputEvent::GestureTap) {
            m_tapX = event.x;
            m_tapY = event.y;
        } else if (event.type == WebInputEvent::GestureLongPress) {
            m_longpressX = event.x;
            m_longpressY = event.y;
        }
    }

    // Local methods
    void reset()
    {
        m_tapX = -1;
        m_tapY = -1;
        m_longpressX = -1;
        m_longpressY = -1;
    }
    int tapX() { return m_tapX; }
    int tapY() { return m_tapY; }
    int longpressX() { return m_longpressX; }
    int longpressY() { return m_longpressY; }

private:
    int m_tapX;
    int m_tapY;
    int m_longpressX;
    int m_longpressY;
};

class DateTimeChooserWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    WebDateTimeChooserCompletion* chooserCompletion()
    {
        return m_chooserCompletion;
    }

    void clearChooserCompletion()
    {
        m_chooserCompletion = 0;
    }

    // WebViewClient methods
    bool openDateTimeChooser(const WebDateTimeChooserParams&, WebDateTimeChooserCompletion* chooser_completion) override
    {
        m_chooserCompletion = chooser_completion;
        return true;
    }

private:
    WebDateTimeChooserCompletion* m_chooserCompletion;

};

class WebViewTest : public ::testing::Test {
public:
    WebViewTest()
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

    void testAutoResize(const WebSize& minAutoResize, const WebSize& maxAutoResize,
                        const std::string& pageWidth, const std::string& pageHeight,
                        int expectedWidth, int expectedHeight,
                        HorizontalScrollbarState expectedHorizontalState, VerticalScrollbarState expectedVerticalState);

    void testTextInputType(WebTextInputType expectedType, const std::string& htmlFile);
    void testInputMode(const WebString& expectedInputMode, const std::string& htmlFile);

    std::string m_baseURL;
    FrameTestHelpers::WebViewHelper m_webViewHelper;
};

TEST_F(WebViewTest, SaveImageAt)
{
    SaveImageFromDataURLWebViewClient client;

    std::string url = m_baseURL + "image-with-data-url.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "image-with-data-url.html");
    WebViewImpl* webView = m_webViewHelper.initializeAndLoad(url, true, 0, &client);
    webView->resize(WebSize(400, 400));
    webView->layout();

    client.reset();
    webView->saveImageAt(WebPoint(1, 1));
    EXPECT_EQ(WebString::fromUTF8("data:image/gif;base64"
        ",R0lGODlhAQABAIAAAAUEBAAAACwAAAAAAQABAAACAkQBADs="), client.result());

    client.reset();
    webView->saveImageAt(WebPoint(1, 2));
    EXPECT_EQ(WebString(), client.result());

    webView->setPageScaleFactor(4);
    webView->setPinchViewportOffset(WebFloatPoint(1, 1));

    client.reset();
    webView->saveImageAt(WebPoint(3, 3));
    EXPECT_EQ(WebString::fromUTF8("data:image/gif;base64"
        ",R0lGODlhAQABAIAAAAUEBAAAACwAAAAAAQABAAACAkQBADs="), client.result());

    m_webViewHelper.reset(); // Explicitly reset to break dependency on locally scoped client.
};

TEST_F(WebViewTest, SaveImageWithImageMap)
{
    SaveImageFromDataURLWebViewClient client;

    std::string url = m_baseURL + "image-map.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "image-map.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url, true, 0, &client);
    webView->resize(WebSize(400, 400));

    client.reset();
    webView->saveImageAt(WebPoint(25, 25));
    EXPECT_EQ(WebString::fromUTF8("data:image/gif;base64"
        ",R0lGODlhAQABAIAAAAUEBAAAACwAAAAAAQABAAACAkQBADs="), client.result());

    client.reset();
    webView->saveImageAt(WebPoint(75, 25));
    EXPECT_EQ(WebString::fromUTF8("data:image/gif;base64"
        ",R0lGODlhAQABAIAAAAUEBAAAACwAAAAAAQABAAACAkQBADs="), client.result());

    client.reset();
    webView->saveImageAt(WebPoint(125, 25));
    EXPECT_EQ(WebString(), client.result());

    m_webViewHelper.reset(); // Explicitly reset to break dependency on locally scoped client.
}

TEST_F(WebViewTest, CopyImageAt)
{
    std::string url = m_baseURL + "canvas-copy-image.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "canvas-copy-image.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url, true, 0);
    webView->resize(WebSize(400, 400));

    uint64_t sequence = Platform::current()->clipboard()->sequenceNumber(WebClipboard::BufferStandard);

    webView->copyImageAt(WebPoint(50, 50));

    EXPECT_NE(sequence, Platform::current()->clipboard()->sequenceNumber(WebClipboard::BufferStandard));

    WebData data = Platform::current()->clipboard()->readImage(WebClipboard::Buffer());
    WebImage image = WebImage::fromData(data, WebSize());

    SkAutoLockPixels autoLock(image.getSkBitmap());
    EXPECT_EQ(SkColorSetARGB(255, 255, 0, 0), image.getSkBitmap().getColor(0, 0));
};

TEST_F(WebViewTest, CopyImageAtWithPinchZoom)
{
    std::string url = m_baseURL + "canvas-copy-image.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "canvas-copy-image.html");
    WebViewImpl* webView = m_webViewHelper.initializeAndLoad(url, true, 0);
    webView->resize(WebSize(400, 400));
    webView->layout();
    webView->setPageScaleFactor(2);
    webView->setPinchViewportOffset(WebFloatPoint(200, 200));

    uint64_t sequence = Platform::current()->clipboard()->sequenceNumber(WebClipboard::BufferStandard);

    webView->copyImageAt(WebPoint(0, 0));

    EXPECT_NE(sequence, Platform::current()->clipboard()->sequenceNumber(WebClipboard::BufferStandard));

    WebData data = Platform::current()->clipboard()->readImage(WebClipboard::Buffer());
    WebImage image = WebImage::fromData(data, WebSize());

    SkAutoLockPixels autoLock(image.getSkBitmap());
    EXPECT_EQ(SkColorSetARGB(255, 255, 0, 0), image.getSkBitmap().getColor(0, 0));
};

TEST_F(WebViewTest, CopyImageWithImageMap)
{
    SaveImageFromDataURLWebViewClient client;

    std::string url = m_baseURL + "image-map.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "image-map.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url, true, 0, &client);
    webView->resize(WebSize(400, 400));

    client.reset();
    webView->saveImageAt(WebPoint(25, 25));
    EXPECT_EQ(WebString::fromUTF8("data:image/gif;base64"
        ",R0lGODlhAQABAIAAAAUEBAAAACwAAAAAAQABAAACAkQBADs="), client.result());

    client.reset();
    webView->saveImageAt(WebPoint(75, 25));
    EXPECT_EQ(WebString::fromUTF8("data:image/gif;base64"
        ",R0lGODlhAQABAIAAAAUEBAAAACwAAAAAAQABAAACAkQBADs="), client.result());

    client.reset();
    webView->saveImageAt(WebPoint(125, 25));
    EXPECT_EQ(WebString(), client.result());

    m_webViewHelper.reset(); // Explicitly reset to break dependency on locally scoped client.
}

static bool hitTestIsContentEditable(WebView* view, int x, int y)
{
    WebPoint hitPoint(x, y);
    WebHitTestResult hitTestResult = view->hitTestResultAt(hitPoint);
    return hitTestResult.isContentEditable();
}

static std::string hitTestElementId(WebView* view, int x, int y)
{
    WebPoint hitPoint(x, y);
    WebHitTestResult hitTestResult = view->hitTestResultAt(hitPoint);
    return hitTestResult.node().to<WebElement>().getAttribute("id").utf8();
}

TEST_F(WebViewTest, HitTestContentEditableImageMaps)
{
    std::string url = m_baseURL + "content-editable-image-maps.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "content-editable-image-maps.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url, true, 0);
    webView->resize(WebSize(500, 500));

    EXPECT_EQ("areaANotEditable", hitTestElementId(webView, 25, 25));
    EXPECT_FALSE(hitTestIsContentEditable(webView, 25, 25));
    EXPECT_EQ("imageANotEditable", hitTestElementId(webView, 75, 25));
    EXPECT_FALSE(hitTestIsContentEditable(webView, 75, 25));

    EXPECT_EQ("areaBNotEditable", hitTestElementId(webView, 25, 125));
    EXPECT_FALSE(hitTestIsContentEditable(webView, 25, 125));
    EXPECT_EQ("imageBEditable", hitTestElementId(webView, 75, 125));
    EXPECT_TRUE(hitTestIsContentEditable(webView, 75, 125));

    EXPECT_EQ("areaCNotEditable", hitTestElementId(webView, 25, 225));
    EXPECT_FALSE(hitTestIsContentEditable(webView, 25, 225));
    EXPECT_EQ("imageCNotEditable", hitTestElementId(webView, 75, 225));
    EXPECT_FALSE(hitTestIsContentEditable(webView, 75, 225));

    EXPECT_EQ("areaDEditable", hitTestElementId(webView, 25, 325));
    EXPECT_TRUE(hitTestIsContentEditable(webView, 25, 325));
    EXPECT_EQ("imageDNotEditable", hitTestElementId(webView, 75, 325));
    EXPECT_FALSE(hitTestIsContentEditable(webView, 75, 325));
}

static std::string hitTestAbsoluteUrl(WebView* view, int x, int y)
{
    WebPoint hitPoint(x, y);
    WebHitTestResult hitTestResult = view->hitTestResultAt(hitPoint);
    return hitTestResult.absoluteImageURL().string().utf8();
}

static WebElement hitTestUrlElement(WebView* view, int x, int y)
{
    WebPoint hitPoint(x, y);
    WebHitTestResult hitTestResult = view->hitTestResultAt(hitPoint);
    return hitTestResult.urlElement();
}

TEST_F(WebViewTest, ImageMapUrls)
{
    std::string url = m_baseURL + "image-map.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "image-map.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url, true, 0);
    webView->resize(WebSize(400, 400));

    std::string imageUrl = "data:image/gif;base64,R0lGODlhAQABAIAAAAUEBAAAACwAAAAAAQABAAACAkQBADs=";

    EXPECT_EQ("area", hitTestElementId(webView, 25, 25));
    EXPECT_EQ("area", hitTestUrlElement(webView, 25, 25).getAttribute("id").utf8());
    EXPECT_EQ(imageUrl, hitTestAbsoluteUrl(webView, 25, 25));

    EXPECT_EQ("image", hitTestElementId(webView, 75, 25));
    EXPECT_TRUE(hitTestUrlElement(webView, 75, 25).isNull());
    EXPECT_EQ(imageUrl, hitTestAbsoluteUrl(webView, 75, 25));
}

TEST_F(WebViewTest, SetBaseBackgroundColor)
{
    const WebColor kWhite    = 0xFFFFFFFF;
    const WebColor kBlue     = 0xFF0000FF;
    const WebColor kDarkCyan = 0xFF227788;
    const WebColor kTranslucentPutty = 0x80BFB196;
    const WebColor kTransparent = 0x00000000;

    WebViewImpl* webView = m_webViewHelper.initialize();
    EXPECT_EQ(kWhite, webView->backgroundColor());

    webView->setBaseBackgroundColor(kBlue);
    EXPECT_EQ(kBlue, webView->backgroundColor());

    WebURL baseURL = URLTestHelpers::toKURL("http://example.com/");
    FrameTestHelpers::loadHTMLString(webView->mainFrame(), "<html><head><style>body {background-color:#227788}</style></head></html>", baseURL);
    EXPECT_EQ(kDarkCyan, webView->backgroundColor());

    FrameTestHelpers::loadHTMLString(webView->mainFrame(), "<html><head><style>body {background-color:rgba(255,0,0,0.5)}</style></head></html>", baseURL);
    // Expected: red (50% alpha) blended atop base of kBlue.
    EXPECT_EQ(0xFF7F0080, webView->backgroundColor());

    webView->setBaseBackgroundColor(kTranslucentPutty);
    // Expected: red (50% alpha) blended atop kTranslucentPutty. Note the alpha.
    EXPECT_EQ(0xBFE93B32, webView->backgroundColor());

    webView->setBaseBackgroundColor(kTransparent);
    FrameTestHelpers::loadHTMLString(webView->mainFrame(), "<html><head><style>body {background-color:transparent}</style></head></html>", baseURL);
    // Expected: transparent on top of kTransparent will still be transparent.
    EXPECT_EQ(kTransparent, webView->backgroundColor());

    LocalFrame* frame = webView->mainFrameImpl()->frame();
    // The detach() and dispose() calls are a hack to prevent this test
    // from violating invariants about frame state during navigation/detach.
    frame->document()->detach();

    // Creating a new frame view with the background color having 0 alpha.
    frame->createView(IntSize(1024, 768), Color::transparent, true);
    EXPECT_EQ(kTransparent, frame->view()->baseBackgroundColor());
    frame->view()->dispose();

    Color kTransparentRed(100, 0, 0, 0);
    frame->createView(IntSize(1024, 768), kTransparentRed, true);
    EXPECT_EQ(kTransparentRed, frame->view()->baseBackgroundColor());
    frame->view()->dispose();
}

TEST_F(WebViewTest, SetBaseBackgroundColorBeforeMainFrame)
{
    const WebColor kBlue = 0xFF0000FF;
    FrameTestHelpers::TestWebViewClient webViewClient;
    WebView* webView = WebViewImpl::create(&webViewClient);
    EXPECT_NE(kBlue, webView->backgroundColor());
    // webView does not have a frame yet, but we should still be able to set the background color.
    webView->setBaseBackgroundColor(kBlue);
    EXPECT_EQ(kBlue, webView->backgroundColor());
    WebLocalFrameImpl* frame = WebLocalFrameImpl::create(WebTreeScopeType::Document, nullptr);
    webView->setMainFrame(frame);
    webView->close();
    frame->close();
}

TEST_F(WebViewTest, SetBaseBackgroundColorAndBlendWithExistingContent)
{
    const WebColor kAlphaRed = 0x80FF0000;
    const WebColor kAlphaGreen = 0x8000FF00;
    const int kWidth = 100;
    const int kHeight = 100;

    WebView* webView = m_webViewHelper.initialize();

    // Set WebView background to green with alpha.
    webView->setBaseBackgroundColor(kAlphaGreen);
    webView->settings()->setShouldClearDocumentBackground(false);
    webView->resize(WebSize(kWidth, kHeight));
    webView->layout();

    // Set canvas background to red with alpha.
    SkBitmap bitmap;
    bitmap.allocN32Pixels(kWidth, kHeight);
    SkCanvas canvas(bitmap);
    canvas.clear(kAlphaRed);

    SkPictureBuilder pictureBuilder(FloatRect(0, 0, kWidth, kHeight));

    // Paint the root of the main frame in the way that CompositedLayerMapping would.
    FrameView* view = m_webViewHelper.webViewImpl()->mainFrameImpl()->frameView();
    DeprecatedPaintLayer* rootLayer = view->layoutView()->layer();
    LayoutRect paintRect(0, 0, kWidth, kHeight);
    DeprecatedPaintLayerPaintingInfo paintingInfo(rootLayer, paintRect, PaintBehaviorNormal, LayoutSize());
    DeprecatedPaintLayerPainter(*rootLayer).paintLayerContents(&pictureBuilder.context(), paintingInfo, PaintLayerPaintingCompositingAllPhases);

    pictureBuilder.endRecording()->playback(&canvas);

    // The result should be a blend of red and green.
    SkColor color = bitmap.getColor(kWidth / 2, kHeight / 2);
    EXPECT_TRUE(redChannel(color));
    EXPECT_TRUE(greenChannel(color));
}

TEST_F(WebViewTest, FocusIsInactive)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), "visible_iframe.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "visible_iframe.html");

    webView->setFocus(true);
    webView->setIsActive(true);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    EXPECT_TRUE(frame->frame()->document()->isHTMLDocument());

    HTMLDocument* document = toHTMLDocument(frame->frame()->document());
    EXPECT_TRUE(document->hasFocus());
    webView->setFocus(false);
    webView->setIsActive(false);
    EXPECT_FALSE(document->hasFocus());
    webView->setFocus(true);
    webView->setIsActive(true);
    EXPECT_TRUE(document->hasFocus());
    webView->setFocus(true);
    webView->setIsActive(false);
    EXPECT_FALSE(document->hasFocus());
    webView->setFocus(false);
    webView->setIsActive(true);
    EXPECT_FALSE(document->hasFocus());
}

TEST_F(WebViewTest, ActiveState)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), "visible_iframe.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "visible_iframe.html");

    ASSERT_TRUE(webView);

    webView->setIsActive(true);
    EXPECT_TRUE(webView->isActive());

    webView->setIsActive(false);
    EXPECT_FALSE(webView->isActive());

    webView->setIsActive(true);
    EXPECT_TRUE(webView->isActive());
}

TEST_F(WebViewTest, HitTestResultAtWithPageScale)
{
    std::string url = m_baseURL + "specify_size.html?" + "50px" + ":" + "50px";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "specify_size.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url, true, 0);
    webView->resize(WebSize(100, 100));
    WebPoint hitPoint(75, 75);

    // Image is at top left quandrant, so should not hit it.
    WebHitTestResult negativeResult = webView->hitTestResultAt(hitPoint);
    ASSERT_EQ(WebNode::ElementNode, negativeResult.node().nodeType());
    EXPECT_FALSE(negativeResult.node().to<WebElement>().hasHTMLTagName("img"));
    negativeResult.reset();

    // Scale page up 2x so image should occupy the whole viewport.
    webView->setPageScaleFactor(2.0f);
    WebHitTestResult positiveResult = webView->hitTestResultAt(hitPoint);
    ASSERT_EQ(WebNode::ElementNode, positiveResult.node().nodeType());
    EXPECT_TRUE(positiveResult.node().to<WebElement>().hasHTMLTagName("img"));
    positiveResult.reset();
}

TEST_F(WebViewTest, HitTestResultAtWithPageScaleAndPan)
{
    std::string url = m_baseURL + "specify_size.html?" + "50px" + ":" + "50px";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "specify_size.html");
    WebView* webView = m_webViewHelper.initialize(true);
    loadFrame(webView->mainFrame(), url);
    webView->resize(WebSize(100, 100));
    WebPoint hitPoint(75, 75);

    // Image is at top left quandrant, so should not hit it.
    WebHitTestResult negativeResult = webView->hitTestResultAt(hitPoint);
    ASSERT_EQ(WebNode::ElementNode, negativeResult.node().nodeType());
    EXPECT_FALSE(negativeResult.node().to<WebElement>().hasHTMLTagName("img"));
    negativeResult.reset();

    // Scale page up 2x so image should occupy the whole viewport.
    webView->setPageScaleFactor(2.0f);
    WebHitTestResult positiveResult = webView->hitTestResultAt(hitPoint);
    ASSERT_EQ(WebNode::ElementNode, positiveResult.node().nodeType());
    EXPECT_TRUE(positiveResult.node().to<WebElement>().hasHTMLTagName("img"));
    positiveResult.reset();

    // Pan around the zoomed in page so the image is not visible in viewport.
    webView->setPinchViewportOffset(WebFloatPoint(100, 100));
    WebHitTestResult negativeResult2 = webView->hitTestResultAt(hitPoint);
    ASSERT_EQ(WebNode::ElementNode, negativeResult2.node().nodeType());
    EXPECT_FALSE(negativeResult2.node().to<WebElement>().hasHTMLTagName("img"));
    negativeResult2.reset();
}

TEST_F(WebViewTest, HitTestResultForTapWithTapArea)
{
    std::string url = m_baseURL + "hit_test.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "hit_test.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url, true, 0);
    webView->resize(WebSize(100, 100));
    WebPoint hitPoint(55, 55);

    // Image is at top left quandrant, so should not hit it.
    WebHitTestResult negativeResult = webView->hitTestResultAt(hitPoint);
    ASSERT_EQ(WebNode::ElementNode, negativeResult.node().nodeType());
    EXPECT_FALSE(negativeResult.node().to<WebElement>().hasHTMLTagName("img"));
    negativeResult.reset();

    // The tap area is 20 by 20 square, centered at 55, 55.
    WebSize tapArea(20, 20);
    WebHitTestResult positiveResult = webView->hitTestResultForTap(hitPoint, tapArea);
    ASSERT_EQ(WebNode::ElementNode, positiveResult.node().nodeType());
    EXPECT_TRUE(positiveResult.node().to<WebElement>().hasHTMLTagName("img"));
    positiveResult.reset();

    // Move the hit point the image is just outside the tapped area now.
    hitPoint = WebPoint(61, 61);
    WebHitTestResult negativeResult2 = webView->hitTestResultForTap(hitPoint, tapArea);
    ASSERT_EQ(WebNode::ElementNode, negativeResult2.node().nodeType());
    EXPECT_FALSE(negativeResult2.node().to<WebElement>().hasHTMLTagName("img"));
    negativeResult2.reset();
}

TEST_F(WebViewTest, HitTestResultForTapWithTapAreaPageScaleAndPan)
{
    std::string url = m_baseURL + "hit_test.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "hit_test.html");
    WebView* webView = m_webViewHelper.initialize(true);
    loadFrame(webView->mainFrame(), url);
    webView->resize(WebSize(100, 100));
    WebPoint hitPoint(55, 55);

    // Image is at top left quandrant, so should not hit it.
    WebHitTestResult negativeResult = webView->hitTestResultAt(hitPoint);
    ASSERT_EQ(WebNode::ElementNode, negativeResult.node().nodeType());
    EXPECT_FALSE(negativeResult.node().to<WebElement>().hasHTMLTagName("img"));
    negativeResult.reset();

    // The tap area is 20 by 20 square, centered at 55, 55.
    WebSize tapArea(20, 20);
    WebHitTestResult positiveResult = webView->hitTestResultForTap(hitPoint, tapArea);
    ASSERT_EQ(WebNode::ElementNode, positiveResult.node().nodeType());
    EXPECT_TRUE(positiveResult.node().to<WebElement>().hasHTMLTagName("img"));
    positiveResult.reset();

    // Zoom in and pan around the page so the image is not visible in viewport.
    webView->setPageScaleFactor(2.0f);
    webView->setPinchViewportOffset(WebFloatPoint(100, 100));
    WebHitTestResult negativeResult2 = webView->hitTestResultForTap(hitPoint, tapArea);
    ASSERT_EQ(WebNode::ElementNode, negativeResult2.node().nodeType());
    EXPECT_FALSE(negativeResult2.node().to<WebElement>().hasHTMLTagName("img"));
    negativeResult2.reset();
}

void WebViewTest::testAutoResize(const WebSize& minAutoResize, const WebSize& maxAutoResize,
                                 const std::string& pageWidth, const std::string& pageHeight,
                                 int expectedWidth, int expectedHeight,
                                 HorizontalScrollbarState expectedHorizontalState, VerticalScrollbarState expectedVerticalState)
{
    AutoResizeWebViewClient client;
    std::string url = m_baseURL + "specify_size.html?" + pageWidth + ":" + pageHeight;
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "specify_size.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url, true, 0, &client);
    client.testData().setWebView(webView);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    FrameView* frameView = frame->frame()->view();
    frameView->layout();
    EXPECT_FALSE(frameView->layoutPending());
    EXPECT_FALSE(frameView->needsLayout());

    webView->enableAutoResizeMode(minAutoResize, maxAutoResize);
    EXPECT_TRUE(frameView->layoutPending());
    EXPECT_TRUE(frameView->needsLayout());
    frameView->layout();

    EXPECT_TRUE(frame->frame()->document()->isHTMLDocument());

    EXPECT_EQ(expectedWidth, client.testData().width());
    EXPECT_EQ(expectedHeight, client.testData().height());

    // Android disables main frame scrollbars.
#if !OS(ANDROID)
    EXPECT_EQ(expectedHorizontalState, client.testData().horizontalScrollbarState());
    EXPECT_EQ(expectedVerticalState, client.testData().verticalScrollbarState());
#endif

    m_webViewHelper.reset(); // Explicitly reset to break dependency on locally scoped client.
}

TEST_F(WebViewTest, AutoResizeMinimumSize)
{
    WebSize minAutoResize(91, 56);
    WebSize maxAutoResize(403, 302);
    std::string pageWidth = "91px";
    std::string pageHeight = "56px";
    int expectedWidth = 91;
    int expectedHeight = 56;
    testAutoResize(minAutoResize, maxAutoResize, pageWidth, pageHeight,
                   expectedWidth, expectedHeight, NoHorizontalScrollbar, NoVerticalScrollbar);
}

TEST_F(WebViewTest, AutoResizeHeightOverflowAndFixedWidth)
{
    WebSize minAutoResize(90, 95);
    WebSize maxAutoResize(90, 100);
    std::string pageWidth = "60px";
    std::string pageHeight = "200px";
    int expectedWidth = 90;
    int expectedHeight = 100;
    testAutoResize(minAutoResize, maxAutoResize, pageWidth, pageHeight,
                   expectedWidth, expectedHeight, NoHorizontalScrollbar, VisibleVerticalScrollbar);
}

TEST_F(WebViewTest, AutoResizeFixedHeightAndWidthOverflow)
{
    WebSize minAutoResize(90, 100);
    WebSize maxAutoResize(200, 100);
    std::string pageWidth = "300px";
    std::string pageHeight = "80px";
    int expectedWidth = 200;
    int expectedHeight = 100;
    testAutoResize(minAutoResize, maxAutoResize, pageWidth, pageHeight,
                   expectedWidth, expectedHeight, VisibleHorizontalScrollbar, NoVerticalScrollbar);
}

// Next three tests disabled for https://bugs.webkit.org/show_bug.cgi?id=92318 .
// It seems we can run three AutoResize tests, then the next one breaks.
TEST_F(WebViewTest, AutoResizeInBetweenSizes)
{
    WebSize minAutoResize(90, 95);
    WebSize maxAutoResize(200, 300);
    std::string pageWidth = "100px";
    std::string pageHeight = "200px";
    int expectedWidth = 100;
    int expectedHeight = 200;
    testAutoResize(minAutoResize, maxAutoResize, pageWidth, pageHeight,
                   expectedWidth, expectedHeight, NoHorizontalScrollbar, NoVerticalScrollbar);
}

TEST_F(WebViewTest, AutoResizeOverflowSizes)
{
    WebSize minAutoResize(90, 95);
    WebSize maxAutoResize(200, 300);
    std::string pageWidth = "300px";
    std::string pageHeight = "400px";
    int expectedWidth = 200;
    int expectedHeight = 300;
    testAutoResize(minAutoResize, maxAutoResize, pageWidth, pageHeight,
                   expectedWidth, expectedHeight, VisibleHorizontalScrollbar, VisibleVerticalScrollbar);
}

TEST_F(WebViewTest, AutoResizeMaxSize)
{
    WebSize minAutoResize(90, 95);
    WebSize maxAutoResize(200, 300);
    std::string pageWidth = "200px";
    std::string pageHeight = "300px";
    int expectedWidth = 200;
    int expectedHeight = 300;
    testAutoResize(minAutoResize, maxAutoResize, pageWidth, pageHeight,
                   expectedWidth, expectedHeight, NoHorizontalScrollbar, NoVerticalScrollbar);
}

void WebViewTest::testTextInputType(WebTextInputType expectedType, const std::string& htmlFile)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(htmlFile.c_str()));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + htmlFile);
    webView->setInitialFocus(false);
    EXPECT_EQ(expectedType, webView->textInputInfo().type);
}

TEST_F(WebViewTest, TextInputType)
{
    testTextInputType(WebTextInputTypeText, "input_field_default.html");
    testTextInputType(WebTextInputTypePassword, "input_field_password.html");
    testTextInputType(WebTextInputTypeEmail, "input_field_email.html");
    testTextInputType(WebTextInputTypeSearch, "input_field_search.html");
    testTextInputType(WebTextInputTypeNumber, "input_field_number.html");
    testTextInputType(WebTextInputTypeTelephone, "input_field_tel.html");
    testTextInputType(WebTextInputTypeURL, "input_field_url.html");
}

void WebViewTest::testInputMode(const WebString& expectedInputMode, const std::string& htmlFile)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(htmlFile.c_str()));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + htmlFile);
    webView->setInitialFocus(false);
    EXPECT_EQ(expectedInputMode, webView->textInputInfo().inputMode);
}

TEST_F(WebViewTest, InputMode)
{
    testInputMode(WebString(), "input_mode_default.html");
    testInputMode(WebString("unknown"), "input_mode_default_unknown.html");
    testInputMode(WebString("verbatim"), "input_mode_default_verbatim.html");
    testInputMode(WebString("verbatim"), "input_mode_type_text_verbatim.html");
    testInputMode(WebString("verbatim"), "input_mode_type_search_verbatim.html");
    testInputMode(WebString(), "input_mode_type_url_verbatim.html");
    testInputMode(WebString("verbatim"), "input_mode_textarea_verbatim.html");
}

TEST_F(WebViewTest, TextInputInfoWithReplacedElements)
{
    std::string url = m_baseURL + "div_with_image.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "div_with_image.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url);
    webView->setInitialFocus(false);
    WebTextInputInfo info = webView->textInputInfo();

    EXPECT_EQ("foo\xef\xbf\xbc", info.value.utf8());
}

TEST_F(WebViewTest, SetEditableSelectionOffsetsAndTextInputInfo)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html");
    webView->setInitialFocus(false);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setEditableSelectionOffsets(5, 13);
    EXPECT_EQ("56789abc", frame->selectionAsText());
    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ("0123456789abcdefghijklmnopqrstuvwxyz", info.value);
    EXPECT_EQ(5, info.selectionStart);
    EXPECT_EQ(13, info.selectionEnd);
    EXPECT_EQ(-1, info.compositionStart);
    EXPECT_EQ(-1, info.compositionEnd);

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("content_editable_populated.html"));
    webView = m_webViewHelper.initializeAndLoad(m_baseURL + "content_editable_populated.html");
    webView->setInitialFocus(false);
    frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setEditableSelectionOffsets(8, 19);
    EXPECT_EQ("89abcdefghi", frame->selectionAsText());
    info = webView->textInputInfo();
    EXPECT_EQ("0123456789abcdefghijklmnopqrstuvwxyz", info.value);
    EXPECT_EQ(8, info.selectionStart);
    EXPECT_EQ(19, info.selectionEnd);
    EXPECT_EQ(-1, info.compositionStart);
    EXPECT_EQ(-1, info.compositionEnd);
}

TEST_F(WebViewTest, ConfirmCompositionCursorPositionChange)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html");
    webView->setInitialFocus(false);

    // Set up a composition that needs to be committed.
    std::string compositionText("hello");

    WebVector<WebCompositionUnderline> emptyUnderlines;
    webView->setComposition(WebString::fromUTF8(compositionText.c_str()), emptyUnderlines, 3, 3);

    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ("hello", std::string(info.value.utf8().data()));
    EXPECT_EQ(3, info.selectionStart);
    EXPECT_EQ(3, info.selectionEnd);
    EXPECT_EQ(0, info.compositionStart);
    EXPECT_EQ(5, info.compositionEnd);

    webView->confirmComposition(WebWidget::KeepSelection);
    info = webView->textInputInfo();
    EXPECT_EQ(3, info.selectionStart);
    EXPECT_EQ(3, info.selectionEnd);
    EXPECT_EQ(-1, info.compositionStart);
    EXPECT_EQ(-1, info.compositionEnd);

    webView->setComposition(WebString::fromUTF8(compositionText.c_str()), emptyUnderlines, 3, 3);
    info = webView->textInputInfo();
    EXPECT_EQ("helhellolo", std::string(info.value.utf8().data()));
    EXPECT_EQ(6, info.selectionStart);
    EXPECT_EQ(6, info.selectionEnd);
    EXPECT_EQ(3, info.compositionStart);
    EXPECT_EQ(8, info.compositionEnd);

    webView->confirmComposition(WebWidget::DoNotKeepSelection);
    info = webView->textInputInfo();
    EXPECT_EQ(8, info.selectionStart);
    EXPECT_EQ(8, info.selectionEnd);
    EXPECT_EQ(-1, info.compositionStart);
    EXPECT_EQ(-1, info.compositionEnd);
}

TEST_F(WebViewTest, InsertNewLinePlacementAfterConfirmComposition)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("text_area_populated.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "text_area_populated.html");
    webView->setInitialFocus(false);

    WebVector<WebCompositionUnderline> emptyUnderlines;

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setEditableSelectionOffsets(4, 4);
    frame->setCompositionFromExistingText(8, 12, emptyUnderlines);

    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ("0123456789abcdefghijklmnopqrstuvwxyz", std::string(info.value.utf8().data()));
    EXPECT_EQ(4, info.selectionStart);
    EXPECT_EQ(4, info.selectionEnd);
    EXPECT_EQ(8, info.compositionStart);
    EXPECT_EQ(12, info.compositionEnd);

    webView->confirmComposition(WebWidget::KeepSelection);
    info = webView->textInputInfo();
    EXPECT_EQ(4, info.selectionStart);
    EXPECT_EQ(4, info.selectionEnd);
    EXPECT_EQ(-1, info.compositionStart);
    EXPECT_EQ(-1, info.compositionEnd);

    std::string compositionText("\n");
    webView->confirmComposition(WebString::fromUTF8(compositionText.c_str()));
    info = webView->textInputInfo();
    EXPECT_EQ(5, info.selectionStart);
    EXPECT_EQ(5, info.selectionEnd);
    EXPECT_EQ(-1, info.compositionStart);
    EXPECT_EQ(-1, info.compositionEnd);
    EXPECT_EQ("0123\n456789abcdefghijklmnopqrstuvwxyz", std::string(info.value.utf8().data()));
}

TEST_F(WebViewTest, ExtendSelectionAndDelete)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html");
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    webView->setInitialFocus(false);
    frame->setEditableSelectionOffsets(10, 10);
    frame->extendSelectionAndDelete(5, 8);
    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ("01234ijklmnopqrstuvwxyz", std::string(info.value.utf8().data()));
    EXPECT_EQ(5, info.selectionStart);
    EXPECT_EQ(5, info.selectionEnd);
    frame->extendSelectionAndDelete(10, 0);
    info = webView->textInputInfo();
    EXPECT_EQ("ijklmnopqrstuvwxyz", std::string(info.value.utf8().data()));
}

TEST_F(WebViewTest, SetCompositionFromExistingText)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html");
    webView->setInitialFocus(false);
    WebVector<WebCompositionUnderline> underlines(static_cast<size_t>(1));
    underlines[0] = WebCompositionUnderline(0, 4, 0, false, 0);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setEditableSelectionOffsets(4, 10);
    frame->setCompositionFromExistingText(8, 12, underlines);
    WebVector<WebCompositionUnderline> underlineResults = toWebViewImpl(webView)->compositionUnderlines();
    EXPECT_EQ(8u, underlineResults[0].startOffset);
    EXPECT_EQ(12u, underlineResults[0].endOffset);
    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ(4, info.selectionStart);
    EXPECT_EQ(10, info.selectionEnd);
    EXPECT_EQ(8, info.compositionStart);
    EXPECT_EQ(12, info.compositionEnd);
    WebVector<WebCompositionUnderline> emptyUnderlines;
    frame->setCompositionFromExistingText(0, 0, emptyUnderlines);
    info = webView->textInputInfo();
    EXPECT_EQ(4, info.selectionStart);
    EXPECT_EQ(10, info.selectionEnd);
    EXPECT_EQ(-1, info.compositionStart);
    EXPECT_EQ(-1, info.compositionEnd);
}

TEST_F(WebViewTest, SetCompositionFromExistingTextInTextArea)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("text_area_populated.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "text_area_populated.html");
    webView->setInitialFocus(false);
    WebVector<WebCompositionUnderline> underlines(static_cast<size_t>(1));
    underlines[0] = WebCompositionUnderline(0, 4, 0, false, 0);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setEditableSelectionOffsets(27, 27);
    std::string newLineText("\n");
    webView->confirmComposition(WebString::fromUTF8(newLineText.c_str()));
    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ("0123456789abcdefghijklmnopq\nrstuvwxyz", std::string(info.value.utf8().data()));

    frame->setEditableSelectionOffsets(31, 31);
    frame->setCompositionFromExistingText(30, 34, underlines);
    WebVector<WebCompositionUnderline> underlineResults = toWebViewImpl(webView)->compositionUnderlines();
    EXPECT_EQ(2u, underlineResults[0].startOffset);
    EXPECT_EQ(6u, underlineResults[0].endOffset);
    info = webView->textInputInfo();
    EXPECT_EQ("0123456789abcdefghijklmnopq\nrstuvwxyz", std::string(info.value.utf8().data()));
    EXPECT_EQ(31, info.selectionStart);
    EXPECT_EQ(31, info.selectionEnd);
    EXPECT_EQ(30, info.compositionStart);
    EXPECT_EQ(34, info.compositionEnd);

    std::string compositionText("yolo");
    webView->confirmComposition(WebString::fromUTF8(compositionText.c_str()));
    info = webView->textInputInfo();
    EXPECT_EQ("0123456789abcdefghijklmnopq\nrsyoloxyz", std::string(info.value.utf8().data()));
    EXPECT_EQ(34, info.selectionStart);
    EXPECT_EQ(34, info.selectionEnd);
    EXPECT_EQ(-1, info.compositionStart);
    EXPECT_EQ(-1, info.compositionEnd);
}

TEST_F(WebViewTest, SetEditableSelectionOffsetsKeepsComposition)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html");
    webView->setInitialFocus(false);

    std::string compositionTextFirst("hello ");
    std::string compositionTextSecond("world");
    WebVector<WebCompositionUnderline> emptyUnderlines;

    webView->confirmComposition(WebString::fromUTF8(compositionTextFirst.c_str()));
    webView->setComposition(WebString::fromUTF8(compositionTextSecond.c_str()), emptyUnderlines, 5, 5);

    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ("hello world", std::string(info.value.utf8().data()));
    EXPECT_EQ(11, info.selectionStart);
    EXPECT_EQ(11, info.selectionEnd);
    EXPECT_EQ(6, info.compositionStart);
    EXPECT_EQ(11, info.compositionEnd);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setEditableSelectionOffsets(6, 6);
    info = webView->textInputInfo();
    EXPECT_EQ("hello world", std::string(info.value.utf8().data()));
    EXPECT_EQ(6, info.selectionStart);
    EXPECT_EQ(6, info.selectionEnd);
    EXPECT_EQ(6, info.compositionStart);
    EXPECT_EQ(11, info.compositionEnd);

    frame->setEditableSelectionOffsets(8, 8);
    info = webView->textInputInfo();
    EXPECT_EQ("hello world", std::string(info.value.utf8().data()));
    EXPECT_EQ(8, info.selectionStart);
    EXPECT_EQ(8, info.selectionEnd);
    EXPECT_EQ(6, info.compositionStart);
    EXPECT_EQ(11, info.compositionEnd);

    frame->setEditableSelectionOffsets(11, 11);
    info = webView->textInputInfo();
    EXPECT_EQ("hello world", std::string(info.value.utf8().data()));
    EXPECT_EQ(11, info.selectionStart);
    EXPECT_EQ(11, info.selectionEnd);
    EXPECT_EQ(6, info.compositionStart);
    EXPECT_EQ(11, info.compositionEnd);

    frame->setEditableSelectionOffsets(6, 11);
    info = webView->textInputInfo();
    EXPECT_EQ("hello world", std::string(info.value.utf8().data()));
    EXPECT_EQ(6, info.selectionStart);
    EXPECT_EQ(11, info.selectionEnd);
    EXPECT_EQ(6, info.compositionStart);
    EXPECT_EQ(11, info.compositionEnd);

    frame->setEditableSelectionOffsets(2, 2);
    info = webView->textInputInfo();
    EXPECT_EQ("hello world", std::string(info.value.utf8().data()));
    EXPECT_EQ(2, info.selectionStart);
    EXPECT_EQ(2, info.selectionEnd);
    EXPECT_EQ(-1, info.compositionStart);
    EXPECT_EQ(-1, info.compositionEnd);
}

TEST_F(WebViewTest, IsSelectionAnchorFirst)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html");
    WebFrame* frame = webView->mainFrame();

    webView->setInitialFocus(false);
    frame->setEditableSelectionOffsets(4, 10);
    EXPECT_TRUE(webView->isSelectionAnchorFirst());
    WebRect anchor;
    WebRect focus;
    webView->selectionBounds(anchor, focus);
    frame->selectRange(WebPoint(focus.x, focus.y), WebPoint(anchor.x, anchor.y));
    EXPECT_FALSE(webView->isSelectionAnchorFirst());
}

TEST_F(WebViewTest, ExitingDeviceEmulationResetsPageScale)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("200-by-300.html"));
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(m_baseURL + "200-by-300.html");
    webViewImpl->resize(WebSize(200, 300));

    float pageScaleExpected = webViewImpl->pageScaleFactor();

    WebDeviceEmulationParams params;
    params.screenPosition = WebDeviceEmulationParams::Desktop;
    params.deviceScaleFactor = 0;
    params.fitToView = false;
    params.offset = WebFloatPoint();
    params.scale = 1;

    webViewImpl->enableDeviceEmulation(params);

    webViewImpl->setPageScaleFactor(2);

    webViewImpl->disableDeviceEmulation();

    EXPECT_EQ(pageScaleExpected, webViewImpl->pageScaleFactor());
}

TEST_F(WebViewTest, HistoryResetScrollAndScaleState)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("200-by-300.html"));
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(m_baseURL + "200-by-300.html");
    webViewImpl->resize(WebSize(100, 150));
    webViewImpl->layout();
    EXPECT_EQ(0, webViewImpl->mainFrame()->scrollOffset().width);
    EXPECT_EQ(0, webViewImpl->mainFrame()->scrollOffset().height);

    // Make the page scale and scroll with the given paremeters.
    webViewImpl->setPageScaleFactor(2.0f);
    webViewImpl->mainFrame()->setScrollOffset(WebSize(94, 111));
    EXPECT_EQ(2.0f, webViewImpl->pageScaleFactor());
    EXPECT_EQ(94, webViewImpl->mainFrame()->scrollOffset().width);
    EXPECT_EQ(111, webViewImpl->mainFrame()->scrollOffset().height);
    LocalFrame* mainFrameLocal = toLocalFrame(webViewImpl->page()->mainFrame());
    mainFrameLocal->loader().saveScrollState();
    EXPECT_EQ(2.0f, mainFrameLocal->loader().currentItem()->pageScaleFactor());
    EXPECT_EQ(94, mainFrameLocal->loader().currentItem()->scrollPoint().x());
    EXPECT_EQ(111, mainFrameLocal->loader().currentItem()->scrollPoint().y());

    // Confirm that resetting the page state resets the saved scroll position.
    webViewImpl->resetScrollAndScaleState();
    EXPECT_EQ(1.0f, webViewImpl->pageScaleFactor());
    EXPECT_EQ(0, webViewImpl->mainFrame()->scrollOffset().width);
    EXPECT_EQ(0, webViewImpl->mainFrame()->scrollOffset().height);
    EXPECT_EQ(1.0f, mainFrameLocal->loader().currentItem()->pageScaleFactor());
    EXPECT_EQ(0, mainFrameLocal->loader().currentItem()->scrollPoint().x());
    EXPECT_EQ(0, mainFrameLocal->loader().currentItem()->scrollPoint().y());
}

TEST_F(WebViewTest, BackForwardRestoreScroll)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("back_forward_restore_scroll.html"));
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(m_baseURL + "back_forward_restore_scroll.html");
    webViewImpl->resize(WebSize(640, 480));
    webViewImpl->layout();

    // Emulate a user scroll
    webViewImpl->mainFrame()->setScrollOffset(WebSize(0, 900));
    LocalFrame* mainFrameLocal = toLocalFrame(webViewImpl->page()->mainFrame());
    RefPtrWillBePersistent<HistoryItem> item1 = mainFrameLocal->loader().currentItem();

    // Click an anchor
    mainFrameLocal->loader().load(FrameLoadRequest(mainFrameLocal->document(), ResourceRequest(mainFrameLocal->document()->completeURL("#a"))));
    RefPtrWillBePersistent<HistoryItem> item2 = mainFrameLocal->loader().currentItem();

    // Go back, then forward, then back again.
    mainFrameLocal->loader().load(
        FrameLoadRequest(nullptr, FrameLoader::resourceRequestFromHistoryItem(
            item1.get(), UseProtocolCachePolicy)),
        FrameLoadTypeBackForward, item1.get(), HistorySameDocumentLoad);
    mainFrameLocal->loader().load(
        FrameLoadRequest(nullptr, FrameLoader::resourceRequestFromHistoryItem(
            item2.get(), UseProtocolCachePolicy)),
        FrameLoadTypeBackForward, item2.get(), HistorySameDocumentLoad);
    mainFrameLocal->loader().load(
        FrameLoadRequest(nullptr, FrameLoader::resourceRequestFromHistoryItem(
            item1.get(), UseProtocolCachePolicy)),
        FrameLoadTypeBackForward, item1.get(), HistorySameDocumentLoad);

    // Click a different anchor
    mainFrameLocal->loader().load(FrameLoadRequest(mainFrameLocal->document(), ResourceRequest(mainFrameLocal->document()->completeURL("#b"))));
    RefPtrWillBePersistent<HistoryItem> item3 = mainFrameLocal->loader().currentItem();

    // Go back, then forward. The scroll position should be properly set on the forward navigation.
    mainFrameLocal->loader().load(
        FrameLoadRequest(nullptr, FrameLoader::resourceRequestFromHistoryItem(
            item1.get(), UseProtocolCachePolicy)),
        FrameLoadTypeBackForward, item1.get(), HistorySameDocumentLoad);
    mainFrameLocal->loader().load(
        FrameLoadRequest(nullptr, FrameLoader::resourceRequestFromHistoryItem(
            item3.get(), UseProtocolCachePolicy)),
        FrameLoadTypeBackForward, item3.get(), HistorySameDocumentLoad);
    EXPECT_EQ(0, webViewImpl->mainFrame()->scrollOffset().width);
    EXPECT_GT(webViewImpl->mainFrame()->scrollOffset().height, 2000);
}

TEST_F(WebViewTest, EnterFullscreenResetScrollAndScaleState)
{
    FrameTestHelpers::TestWebViewClient client;
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("200-by-300.html"));
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(m_baseURL + "200-by-300.html", true, 0, &client);
    webViewImpl->resize(WebSize(100, 150));
    webViewImpl->layout();
    EXPECT_EQ(0, webViewImpl->mainFrame()->scrollOffset().width);
    EXPECT_EQ(0, webViewImpl->mainFrame()->scrollOffset().height);

    // Make the page scale and scroll with the given paremeters.
    webViewImpl->setPageScaleFactor(2.0f);
    webViewImpl->mainFrame()->setScrollOffset(WebSize(94, 111));
    webViewImpl->setPinchViewportOffset(WebFloatPoint(12, 20));
    EXPECT_EQ(2.0f, webViewImpl->pageScaleFactor());
    EXPECT_EQ(94, webViewImpl->mainFrame()->scrollOffset().width);
    EXPECT_EQ(111, webViewImpl->mainFrame()->scrollOffset().height);
    EXPECT_EQ(12, webViewImpl->pinchViewportOffset().x);
    EXPECT_EQ(20, webViewImpl->pinchViewportOffset().y);

    RefPtrWillBeRawPtr<Element> element = static_cast<PassRefPtrWillBeRawPtr<Element>>(webViewImpl->mainFrame()->document().body());
    webViewImpl->enterFullScreenForElement(element.get());
    webViewImpl->didEnterFullScreen();

    // Page scale factor must be 1.0 during fullscreen for elements to be sized
    // properly.
    EXPECT_EQ(1.0f, webViewImpl->pageScaleFactor());

    // Make sure fullscreen nesting doesn't disrupt scroll/scale saving.
    RefPtrWillBeRawPtr<Element> otherElement = static_cast<PassRefPtrWillBeRawPtr<Element>>(webViewImpl->mainFrame()->document().head());
    webViewImpl->enterFullScreenForElement(otherElement.get());

    // Confirm that exiting fullscreen restores the parameters.
    webViewImpl->didExitFullScreen();
    EXPECT_EQ(2.0f, webViewImpl->pageScaleFactor());
    EXPECT_EQ(94, webViewImpl->mainFrame()->scrollOffset().width);
    EXPECT_EQ(111, webViewImpl->mainFrame()->scrollOffset().height);
    EXPECT_EQ(12, webViewImpl->pinchViewportOffset().x);
    EXPECT_EQ(20, webViewImpl->pinchViewportOffset().y);

    m_webViewHelper.reset(); // Explicitly reset to break dependency on locally scoped client.
}

class PrintWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    PrintWebViewClient()
        : m_printCalled(false)
    {
    }

    // WebViewClient methods
    void printPage(WebLocalFrame*) override
    {
        m_printCalled = true;
    }

    bool printCalled() const { return m_printCalled; }

private:
    bool m_printCalled;
};


TEST_F(WebViewTest, PrintWithXHRInFlight)
{
    PrintWebViewClient client;
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("print_with_xhr_inflight.html"));
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(m_baseURL + "print_with_xhr_inflight.html", true, 0, &client);

    ASSERT_TRUE(toLocalFrame(webViewImpl->page()->mainFrame())->document()->loadEventFinished());
    EXPECT_TRUE(client.printCalled());
    m_webViewHelper.reset();
}

class DropTask : public WebThread::Task {
public:
    explicit DropTask(WebView* webView) : m_webView(webView)
    {
    }

    void run() override
    {
        const WebPoint clientPoint(0, 0);
        const WebPoint screenPoint(0, 0);
        m_webView->dragTargetDrop(clientPoint, screenPoint, 0);
    }

private:
    WebView* const m_webView;
};
static void DragAndDropURL(WebViewImpl* webView, const std::string& url)
{
    WebDragData dragData;
    dragData.initialize();

    WebDragData::Item item;
    item.storageType = WebDragData::Item::StorageTypeString;
    item.stringType = "text/uri-list";
    item.stringData = WebString::fromUTF8(url);
    dragData.addItem(item);

    const WebPoint clientPoint(0, 0);
    const WebPoint screenPoint(0, 0);
    webView->dragTargetDragEnter(dragData, clientPoint, screenPoint, WebDragOperationCopy, 0);
    Platform::current()->currentThread()->postTask(FROM_HERE, new DropTask(webView));
    FrameTestHelpers::pumpPendingRequestsDoNotUse(webView->mainFrame());
}

TEST_F(WebViewTest, DragDropURL)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), "foo.html");
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), "bar.html");

    const std::string fooUrl = m_baseURL + "foo.html";
    const std::string barUrl = m_baseURL + "bar.html";

    WebViewImpl* webView = m_webViewHelper.initializeAndLoad(fooUrl);

    ASSERT_TRUE(webView);

    // Drag and drop barUrl and verify that we've navigated to it.
    DragAndDropURL(webView, barUrl);
    EXPECT_EQ(barUrl, webView->mainFrame()->document().url().string().utf8());

    // Drag and drop fooUrl and verify that we've navigated back to it.
    DragAndDropURL(webView, fooUrl);
    EXPECT_EQ(fooUrl, webView->mainFrame()->document().url().string().utf8());

    // Disable navigation on drag-and-drop.
    webView->settingsImpl()->setNavigateOnDragDrop(false);

    // Attempt to drag and drop to barUrl and verify that no navigation has occurred.
    DragAndDropURL(webView, barUrl);
    EXPECT_EQ(fooUrl, webView->mainFrame()->document().url().string().utf8());
}

class ContentDetectorClient : public FrameTestHelpers::TestWebViewClient {
public:
    ContentDetectorClient() { reset(); }

    WebContentDetectionResult detectContentAround(const WebHitTestResult& hitTest) override
    {
        m_contentDetectionRequested = true;
        return m_contentDetectionResult;
    }

    void scheduleContentIntent(const WebURL& url) override
    {
        m_scheduledIntentURL = url;
    }

    void cancelScheduledContentIntents() override
    {
        m_pendingIntentsCancelled = true;
    }

    void reset()
    {
        m_contentDetectionRequested = false;
        m_pendingIntentsCancelled = false;
        m_scheduledIntentURL = WebURL();
        m_contentDetectionResult = WebContentDetectionResult();
    }

    bool contentDetectionRequested() const { return m_contentDetectionRequested; }
    bool pendingIntentsCancelled() const { return m_pendingIntentsCancelled; }
    const WebURL& scheduledIntentURL() const { return m_scheduledIntentURL; }
    void setContentDetectionResult(const WebContentDetectionResult& result) { m_contentDetectionResult = result; }

private:
    bool m_contentDetectionRequested;
    bool m_pendingIntentsCancelled;
    WebURL m_scheduledIntentURL;
    WebContentDetectionResult m_contentDetectionResult;
};

static bool tapElementById(WebView* webView, WebInputEvent::Type type, const WebString& id)
{
    ASSERT(webView);
    RefPtrWillBeRawPtr<Element> element = static_cast<PassRefPtrWillBeRawPtr<Element>>(webView->mainFrame()->document().getElementById(id));
    if (!element)
        return false;

    element->scrollIntoViewIfNeeded();

    // TODO(bokan): Technically incorrect, event positions should be in viewport space. crbug.com/371902.
    IntPoint center = element->screenRect().center();

    WebGestureEvent event;
    event.type = type;
    event.x = center.x();
    event.y = center.y();

    webView->handleInputEvent(event);
    runPendingTasks();
    return true;
}

TEST_F(WebViewTest, DetectContentAroundPosition)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("content_listeners.html"));

    ContentDetectorClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "content_listeners.html", true, 0, &client);
    webView->resize(WebSize(500, 300));
    webView->layout();
    runPendingTasks();

    WebString clickListener = WebString::fromUTF8("clickListener");
    WebString touchstartListener = WebString::fromUTF8("touchstartListener");
    WebString mousedownListener = WebString::fromUTF8("mousedownListener");
    WebString noListener = WebString::fromUTF8("noListener");
    WebString link = WebString::fromUTF8("link");

    // Ensure content detection is not requested for nodes listening to click,
    // mouse or touch events when we do simple taps.
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, clickListener));
    EXPECT_FALSE(client.contentDetectionRequested());
    client.reset();

    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, touchstartListener));
    EXPECT_FALSE(client.contentDetectionRequested());
    client.reset();

    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, mousedownListener));
    EXPECT_FALSE(client.contentDetectionRequested());
    client.reset();

    // Content detection should work normally without these event listeners.
    // The click listener in the body should be ignored as a special case.
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, noListener));
    EXPECT_TRUE(client.contentDetectionRequested());
    EXPECT_FALSE(client.scheduledIntentURL().isValid());

    WebURL intentURL = toKURL(m_baseURL);
    client.setContentDetectionResult(WebContentDetectionResult(WebRange(), WebString(), intentURL));
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, noListener));
    EXPECT_TRUE(client.scheduledIntentURL() == intentURL);

    // Tapping elsewhere should cancel the scheduled intent.
    WebGestureEvent event;
    event.type = WebInputEvent::GestureTap;
    webView->handleInputEvent(event);
    runPendingTasks();
    EXPECT_TRUE(client.pendingIntentsCancelled());

    m_webViewHelper.reset(); // Explicitly reset to break dependency on locally scoped client.
}

TEST_F(WebViewTest, ClientTapHandling)
{
    TapHandlingWebViewClient client;
    client.reset();
    WebView* webView = m_webViewHelper.initializeAndLoad("about:blank", true, 0, &client);
    WebGestureEvent event;
    event.type = WebInputEvent::GestureTap;
    event.x = 3;
    event.y = 8;
    webView->handleInputEvent(event);
    runPendingTasks();
    EXPECT_EQ(3, client.tapX());
    EXPECT_EQ(8, client.tapY());
    client.reset();
    event.type = WebInputEvent::GestureLongPress;
    event.x = 25;
    event.y = 7;
    webView->handleInputEvent(event);
    runPendingTasks();
    EXPECT_EQ(25, client.longpressX());
    EXPECT_EQ(7, client.longpressY());

    m_webViewHelper.reset(); // Explicitly reset to break dependency on locally scoped client.
}

TEST_F(WebViewTest, ClientTapHandlingNullWebViewClient)
{
    WebViewImpl* webView = WebViewImpl::create(nullptr);
    webView->setMainFrame(WebLocalFrame::create(WebTreeScopeType::Document, nullptr));
    WebGestureEvent event;
    event.type = WebInputEvent::GestureTap;
    event.x = 3;
    event.y = 8;
    EXPECT_FALSE(webView->handleInputEvent(event));
    webView->close();
}

#if OS(ANDROID)
TEST_F(WebViewTest, LongPressSelection)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("longpress_selection.html"));

    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "longpress_selection.html", true);
    webView->resize(WebSize(500, 300));
    webView->layout();
    runPendingTasks();

    WebString target = WebString::fromUTF8("target");
    WebString onselectstartfalse = WebString::fromUTF8("onselectstartfalse");
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());

    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureLongPress, onselectstartfalse));
    EXPECT_EQ("", std::string(frame->selectionAsText().utf8().data()));
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureLongPress, target));
    EXPECT_EQ("testword", std::string(frame->selectionAsText().utf8().data()));
}

TEST_F(WebViewTest, BlinkCaretOnTypingAfterLongPress)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("blink_caret_on_typing_after_long_press.html"));

    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "blink_caret_on_typing_after_long_press.html", true);
    webView->resize(WebSize(640, 480));
    webView->layout();
    runPendingTasks();

    WebString target = WebString::fromUTF8("target");
    WebLocalFrameImpl* mainFrame = toWebLocalFrameImpl(webView->mainFrame());

    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureLongPress, target));
    EXPECT_TRUE(mainFrame->frame()->selection().isCaretBlinkingSuspended());

    WebKeyboardEvent keyEvent;
    keyEvent.type = WebInputEvent::RawKeyDown;
    webView->handleInputEvent(keyEvent);
    keyEvent.type = WebInputEvent::KeyUp;
    webView->handleInputEvent(keyEvent);
    EXPECT_FALSE(mainFrame->frame()->selection().isCaretBlinkingSuspended());
}
#endif

TEST_F(WebViewTest, SelectionOnReadOnlyInput)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("selection_readonly.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "selection_readonly.html", true);
    webView->resize(WebSize(640, 480));
    webView->layout();
    runPendingTasks();

    std::string testWord = "This text should be selected.";

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    EXPECT_EQ(testWord, std::string(frame->selectionAsText().utf8().data()));

    size_t location;
    size_t length;
    EXPECT_TRUE(toWebViewImpl(webView)->caretOrSelectionRange(&location, &length));
    EXPECT_EQ(location, 0UL);
    EXPECT_EQ(length, testWord.length());
}

static void configueCompositingWebView(WebSettings* settings)
{
    settings->setAcceleratedCompositingEnabled(true);
    settings->setPreferCompositingToLCDTextEnabled(true);
}

TEST_F(WebViewTest, ShowPressOnTransformedLink)
{
    OwnPtr<FrameTestHelpers::TestWebViewClient> fakeCompositingWebViewClient = adoptPtr(new FrameTestHelpers::TestWebViewClient());
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initialize(true, 0, fakeCompositingWebViewClient.get(), &configueCompositingWebView);

    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));

    WebURL baseURL = URLTestHelpers::toKURL("http://example.com/");
    FrameTestHelpers::loadHTMLString(webViewImpl->mainFrame(), "<a href='http://www.test.com' style='position: absolute; left: 20px; top: 20px; width: 200px; transform:translateZ(0);'>A link to highlight</a>", baseURL);

    WebGestureEvent event;
    event.type = WebInputEvent::GestureShowPress;
    event.x = 20;
    event.y = 20;

    // Just make sure we don't hit any asserts.
    webViewImpl->handleInputEvent(event);
}

class MockAutofillClient : public WebAutofillClient {
public:
    MockAutofillClient()
        : m_ignoreTextChanges(false)
        , m_textChangesFromUserGesture(0)
        , m_textChangesWhileIgnored(0)
        , m_textChangesWhileNotIgnored(0)
        , m_userGestureNotificationsCount(0) {}

    ~MockAutofillClient() override {}

    void setIgnoreTextChanges(bool ignore) override { m_ignoreTextChanges = ignore; }
    void textFieldDidChange(const WebFormControlElement&) override
    {
        if (m_ignoreTextChanges)
            ++m_textChangesWhileIgnored;
        else
            ++m_textChangesWhileNotIgnored;

        if (UserGestureIndicator::processingUserGesture())
            ++m_textChangesFromUserGesture;
    }
    void firstUserGestureObserved() override { ++m_userGestureNotificationsCount; }

    void clearChangeCounts()
    {
        m_textChangesWhileIgnored = 0;
        m_textChangesWhileNotIgnored = 0;
    }

    int textChangesFromUserGesture() { return m_textChangesFromUserGesture; }
    int textChangesWhileIgnored() { return m_textChangesWhileIgnored; }
    int textChangesWhileNotIgnored() { return m_textChangesWhileNotIgnored; }
    int getUserGestureNotificationsCount() { return m_userGestureNotificationsCount; }

private:
    bool m_ignoreTextChanges;
    int m_textChangesFromUserGesture;
    int m_textChangesWhileIgnored;
    int m_textChangesWhileNotIgnored;
    int m_userGestureNotificationsCount;
};


TEST_F(WebViewTest, LosingFocusDoesNotTriggerAutofillTextChange)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    MockAutofillClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html");
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setAutofillClient(&client);
    webView->setInitialFocus(false);

    // Set up a composition that needs to be committed.
    WebVector<WebCompositionUnderline> emptyUnderlines;
    frame->setEditableSelectionOffsets(4, 10);
    frame->setCompositionFromExistingText(8, 12, emptyUnderlines);
    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ(4, info.selectionStart);
    EXPECT_EQ(10, info.selectionEnd);
    EXPECT_EQ(8, info.compositionStart);
    EXPECT_EQ(12, info.compositionEnd);

    // Clear the focus and track that the subsequent composition commit does not trigger a
    // text changed notification for autofill.
    client.clearChangeCounts();
    webView->setFocus(false);
    EXPECT_EQ(1, client.textChangesWhileIgnored());
    EXPECT_EQ(0, client.textChangesWhileNotIgnored());

    frame->setAutofillClient(0);
}

static void verifySelectionAndComposition(WebView* webView, int selectionStart, int selectionEnd, int compositionStart, int compositionEnd, const char* failMessage)
{
    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ(selectionStart, info.selectionStart) << failMessage;
    EXPECT_EQ(selectionEnd, info.selectionEnd) << failMessage;
    EXPECT_EQ(compositionStart, info.compositionStart) << failMessage;
    EXPECT_EQ(compositionEnd, info.compositionEnd) << failMessage;
}

TEST_F(WebViewTest, CompositionNotCancelledByBackspace)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("composition_not_cancelled_by_backspace.html"));
    MockAutofillClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "composition_not_cancelled_by_backspace.html");
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setAutofillClient(&client);
    webView->setInitialFocus(false);

    // Test both input elements.
    for (int i = 0; i < 2; ++i) {
        // Select composition and do sanity check.
        WebVector<WebCompositionUnderline> emptyUnderlines;
        frame->setEditableSelectionOffsets(6, 6);
        EXPECT_TRUE(webView->setComposition("fghij", emptyUnderlines, 0, 5));
        frame->setEditableSelectionOffsets(11, 11);
        verifySelectionAndComposition(webView, 11, 11, 6, 11, "initial case");

        // Press Backspace and verify composition didn't get cancelled. This is to verify the fix
        // for crbug.com/429916.
        WebKeyboardEvent keyEvent;
        keyEvent.windowsKeyCode = VKEY_BACK;
        keyEvent.setKeyIdentifierFromWindowsKeyCode();
        keyEvent.type = WebInputEvent::RawKeyDown;
        webView->handleInputEvent(keyEvent);

        frame->setEditableSelectionOffsets(6, 6);
        EXPECT_TRUE(webView->setComposition("fghi", emptyUnderlines, 0, 4));
        frame->setEditableSelectionOffsets(10, 10);
        verifySelectionAndComposition(webView, 10, 10, 6, 10, "after pressing Backspace");

        keyEvent.type = WebInputEvent::KeyUp;
        webView->handleInputEvent(keyEvent);

        webView->advanceFocus(false);
    }

    frame->setAutofillClient(0);
}

TEST_F(WebViewTest, ConfirmCompositionTriggersAutofillTextChange)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    MockAutofillClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html");
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setAutofillClient(&client);
    webView->setInitialFocus(false);

    // Set up a composition that needs to be committed.
    std::string compositionText("testingtext");

    WebVector<WebCompositionUnderline> emptyUnderlines;
    webView->setComposition(WebString::fromUTF8(compositionText.c_str()), emptyUnderlines, 0, compositionText.length());

    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ(0, info.selectionStart);
    EXPECT_EQ((int) compositionText.length(), info.selectionEnd);
    EXPECT_EQ(0, info.compositionStart);
    EXPECT_EQ((int) compositionText.length(), info.compositionEnd);

    client.clearChangeCounts();
    webView->confirmComposition();
    EXPECT_EQ(0, client.textChangesWhileIgnored());
    EXPECT_EQ(1, client.textChangesWhileNotIgnored());

    frame->setAutofillClient(0);
}

TEST_F(WebViewTest, SetCompositionFromExistingTextTriggersAutofillTextChange)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    MockAutofillClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html", true);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setAutofillClient(&client);
    webView->setInitialFocus(false);

    WebVector<WebCompositionUnderline> emptyUnderlines;

    client.clearChangeCounts();
    frame->setCompositionFromExistingText(8, 12, emptyUnderlines);

    WebTextInputInfo info = webView->textInputInfo();
    EXPECT_EQ("0123456789abcdefghijklmnopqrstuvwxyz", std::string(info.value.utf8().data()));
    EXPECT_EQ(8, info.compositionStart);
    EXPECT_EQ(12, info.compositionEnd);

    EXPECT_EQ(0, client.textChangesWhileIgnored());
    EXPECT_EQ(0, client.textChangesWhileNotIgnored());

    WebDocument document = webView->mainFrame()->document();
    EXPECT_EQ(WebString::fromUTF8("none"),  document.getElementById("inputEvent").firstChild().nodeValue());

    frame->setAutofillClient(0);
}

class ViewCreatingWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    ViewCreatingWebViewClient()
        : m_didFocusCalled(false)
    {
    }

    // WebViewClient methods
    WebView* createView(WebLocalFrame*, const WebURLRequest&, const WebWindowFeatures&, const WebString& name, WebNavigationPolicy, bool) override
    {
        return m_webViewHelper.initialize(true, 0, 0);
    }

    // WebWidgetClient methods
    void didFocus() override
    {
        m_didFocusCalled = true;
    }

    bool didFocusCalled() const { return m_didFocusCalled; }
    WebView* createdWebView() const { return m_webViewHelper.webView(); }

private:
    FrameTestHelpers::WebViewHelper m_webViewHelper;
    bool m_didFocusCalled;
};

TEST_F(WebViewTest, DoNotFocusCurrentFrameOnNavigateFromLocalFrame)
{
    ViewCreatingWebViewClient client;
    FrameTestHelpers::WebViewHelper m_webViewHelper;
    WebViewImpl* webViewImpl = m_webViewHelper.initialize(true, 0, &client);
    webViewImpl->page()->settings().setJavaScriptCanOpenWindowsAutomatically(true);

    WebURL baseURL = URLTestHelpers::toKURL("http://example.com/");
    FrameTestHelpers::loadHTMLString(webViewImpl->mainFrame(), "<html><body><iframe src=\"about:blank\"></iframe></body></html>", baseURL);

    // Make a request from a local frame.
    WebURLRequest webURLRequestWithTargetStart;
    webURLRequestWithTargetStart.initialize();
    LocalFrame* localFrame = toWebLocalFrameImpl(webViewImpl->mainFrame()->firstChild())->frame();
    FrameLoadRequest requestWithTargetStart(localFrame->document(), webURLRequestWithTargetStart.toResourceRequest(), "_top");
    localFrame->loader().load(requestWithTargetStart);
    EXPECT_FALSE(client.didFocusCalled());

    m_webViewHelper.reset(); // Remove dependency on locally scoped client.
}

TEST_F(WebViewTest, FocusExistingFrameOnNavigate)
{
    ViewCreatingWebViewClient client;
    FrameTestHelpers::WebViewHelper m_webViewHelper;
    WebViewImpl* webViewImpl = m_webViewHelper.initialize(true, 0, &client);
    webViewImpl->page()->settings().setJavaScriptCanOpenWindowsAutomatically(true);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewImpl->mainFrame());
    frame->setName("_start");

    // Make a request that will open a new window
    WebURLRequest webURLRequest;
    webURLRequest.initialize();
    FrameLoadRequest request(0, webURLRequest.toResourceRequest(), "_blank");
    toLocalFrame(webViewImpl->page()->mainFrame())->loader().load(request);
    ASSERT_TRUE(client.createdWebView());
    EXPECT_FALSE(client.didFocusCalled());

    // Make a request from the new window that will navigate the original window. The original window should be focused.
    WebURLRequest webURLRequestWithTargetStart;
    webURLRequestWithTargetStart.initialize();
    FrameLoadRequest requestWithTargetStart(0, webURLRequestWithTargetStart.toResourceRequest(), "_start");
    toLocalFrame(toWebViewImpl(client.createdWebView())->page()->mainFrame())->loader().load(requestWithTargetStart);
    EXPECT_TRUE(client.didFocusCalled());

    m_webViewHelper.reset(); // Remove dependency on locally scoped client.
}

TEST_F(WebViewTest, DispatchesFocusOutFocusInOnViewToggleFocus)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), "focusout_focusin_events.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "focusout_focusin_events.html", true, 0);

    webView->setFocus(true);
    webView->setFocus(false);
    webView->setFocus(true);

    WebElement element = webView->mainFrame()->document().getElementById("message");
    EXPECT_STREQ("focusoutfocusin", element.textContent().utf8().data());
}

TEST_F(WebViewTest, DispatchesDomFocusOutDomFocusInOnViewToggleFocus)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), "domfocusout_domfocusin_events.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "domfocusout_domfocusin_events.html", true, 0);

    webView->setFocus(true);
    webView->setFocus(false);
    webView->setFocus(true);

    WebElement element = webView->mainFrame()->document().getElementById("message");
    EXPECT_STREQ("DOMFocusOutDOMFocusIn", element.textContent().utf8().data());
}

#if !ENABLE(INPUT_MULTIPLE_FIELDS_UI)
static void openDateTimeChooser(WebView* webView, HTMLInputElement* inputElement)
{
    inputElement->focus();

    WebKeyboardEvent keyEvent;
    keyEvent.windowsKeyCode = VKEY_SPACE;
    keyEvent.type = WebInputEvent::RawKeyDown;
    keyEvent.setKeyIdentifierFromWindowsKeyCode();
    webView->handleInputEvent(keyEvent);

    keyEvent.type = WebInputEvent::KeyUp;
    webView->handleInputEvent(keyEvent);
}

TEST_F(WebViewTest, ChooseValueFromDateTimeChooser)
{
    DateTimeChooserWebViewClient client;
    std::string url = m_baseURL + "date_time_chooser.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "date_time_chooser.html");
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(url, true, 0, &client);

    Document* document = webViewImpl->mainFrameImpl()->frame()->document();

    HTMLInputElement* inputElement;

    inputElement = toHTMLInputElement(document->getElementById("date"));
    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(0);
    client.clearChooserCompletion();
    EXPECT_STREQ("1970-01-01", inputElement->value().utf8().data());

    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(std::numeric_limits<double>::quiet_NaN());
    client.clearChooserCompletion();
    EXPECT_STREQ("", inputElement->value().utf8().data());

    inputElement = toHTMLInputElement(document->getElementById("datetimelocal"));
    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(0);
    client.clearChooserCompletion();
    EXPECT_STREQ("1970-01-01T00:00", inputElement->value().utf8().data());

    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(std::numeric_limits<double>::quiet_NaN());
    client.clearChooserCompletion();
    EXPECT_STREQ("", inputElement->value().utf8().data());

    inputElement = toHTMLInputElement(document->getElementById("month"));
    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(0);
    client.clearChooserCompletion();
    EXPECT_STREQ("1970-01", inputElement->value().utf8().data());

    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(std::numeric_limits<double>::quiet_NaN());
    client.clearChooserCompletion();
    EXPECT_STREQ("", inputElement->value().utf8().data());

    inputElement = toHTMLInputElement(document->getElementById("time"));
    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(0);
    client.clearChooserCompletion();
    EXPECT_STREQ("00:00", inputElement->value().utf8().data());

    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(std::numeric_limits<double>::quiet_NaN());
    client.clearChooserCompletion();
    EXPECT_STREQ("", inputElement->value().utf8().data());

    inputElement = toHTMLInputElement(document->getElementById("week"));
    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(0);
    client.clearChooserCompletion();
    EXPECT_STREQ("1970-W01", inputElement->value().utf8().data());

    openDateTimeChooser(webViewImpl, inputElement);
    client.chooserCompletion()->didChooseValue(std::numeric_limits<double>::quiet_NaN());
    client.clearChooserCompletion();
    EXPECT_STREQ("", inputElement->value().utf8().data());

    // Clear the WebViewClient from the webViewHelper to avoid use-after-free in the
    // WebViewHelper destructor.
    m_webViewHelper.reset();
}
#endif

TEST_F(WebViewTest, DispatchesFocusBlurOnViewToggle)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), "focus_blur_events.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "focus_blur_events.html", true, 0);

    webView->setFocus(true);
    webView->setFocus(false);
    webView->setFocus(true);

    WebElement element = webView->mainFrame()->document().getElementById("message");
    // Expect not to see duplication of events.
    EXPECT_STREQ("blurfocus", element.textContent().utf8().data());
}

TEST_F(WebViewTest, SmartClipData)
{
    static const char* kExpectedClipText = "\nPrice 10,000,000won";
    static const char* kExpectedClipHtml =
        "<div id=\"div4\" style=\"padding: 10px; margin: 10px; border: 2px "
        "solid rgb(135, 206, 235); float: left; width: 190px; height: 30px; "
        "color: rgb(0, 0, 0); font-family: myahem; font-size: 8px; font-style: "
        "normal; font-variant: normal; font-weight: normal; letter-spacing: "
        "normal; line-height: normal; orphans: auto; text-align: start; "
        "text-indent: 0px; text-transform: none; white-space: normal; widows: "
        "1; word-spacing: 0px; -webkit-text-stroke-width: 0px;\">Air "
        "conditioner</div><div id=\"div5\" style=\"padding: 10px; margin: "
        "10px; border: 2px solid rgb(135, 206, 235); float: left; width: "
        "190px; height: 30px; color: rgb(0, 0, 0); font-family: myahem; "
        "font-size: 8px; font-style: normal; font-variant: normal; "
        "font-weight: normal; letter-spacing: normal; line-height: normal; "
        "orphans: auto; text-align: start; text-indent: 0px; text-transform: "
        "none; white-space: normal; widows: 1; word-spacing: 0px; "
        "-webkit-text-stroke-width: 0px;\">Price 10,000,000won</div>";
    WebString clipText;
    WebString clipHtml;
    WebRect clipRect;
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("Ahem.ttf"));
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("smartclip.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "smartclip.html");
    webView->resize(WebSize(500, 500));
    webView->layout();
    WebRect cropRect(300, 125, 152, 50);
    webView->extractSmartClipData(cropRect, clipText, clipHtml, clipRect);
    EXPECT_STREQ(kExpectedClipText, clipText.utf8().c_str());
    EXPECT_STREQ(kExpectedClipHtml, clipHtml.utf8().c_str());
}

TEST_F(WebViewTest, SmartClipDataWithPinchZoom)
{
    static const char* kExpectedClipText = "\nPrice 10,000,000won";
    static const char* kExpectedClipHtml =
        "<div id=\"div4\" style=\"padding: 10px; margin: 10px; border: 2px "
        "solid rgb(135, 206, 235); float: left; width: 190px; height: 30px; "
        "color: rgb(0, 0, 0); font-family: myahem; font-size: 8px; font-style: "
        "normal; font-variant: normal; font-weight: normal; letter-spacing: "
        "normal; line-height: normal; orphans: auto; text-align: start; "
        "text-indent: 0px; text-transform: none; white-space: normal; widows: "
        "1; word-spacing: 0px; -webkit-text-stroke-width: 0px;\">Air "
        "conditioner</div><div id=\"div5\" style=\"padding: 10px; margin: "
        "10px; border: 2px solid rgb(135, 206, 235); float: left; width: "
        "190px; height: 30px; color: rgb(0, 0, 0); font-family: myahem; "
        "font-size: 8px; font-style: normal; font-variant: normal; "
        "font-weight: normal; letter-spacing: normal; line-height: normal; "
        "orphans: auto; text-align: start; text-indent: 0px; text-transform: "
        "none; white-space: normal; widows: 1; word-spacing: 0px; "
        "-webkit-text-stroke-width: 0px;\">Price 10,000,000won</div>";
    WebString clipText;
    WebString clipHtml;
    WebRect clipRect;
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("Ahem.ttf"));
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("smartclip.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "smartclip.html");
    webView->resize(WebSize(500, 500));
    webView->layout();
    webView->setPageScaleFactor(1.5);
    webView->setPinchViewportOffset(WebFloatPoint(167, 100));
    WebRect cropRect(200, 38, 228, 75);
    webView->extractSmartClipData(cropRect, clipText, clipHtml, clipRect);
    EXPECT_STREQ(kExpectedClipText, clipText.utf8().c_str());
    EXPECT_STREQ(kExpectedClipHtml, clipHtml.utf8().c_str());
}

TEST_F(WebViewTest, SmartClipReturnsEmptyStringsWhenUserSelectIsNone)
{
    WebString clipText;
    WebString clipHtml;
    WebRect clipRect;
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("Ahem.ttf"));
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("smartclip_user_select_none.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "smartclip_user_select_none.html");
    webView->resize(WebSize(500, 500));
    webView->layout();
    WebRect cropRect(0, 0, 100, 100);
    webView->extractSmartClipData(cropRect, clipText, clipHtml, clipRect);
    EXPECT_STREQ("", clipText.utf8().c_str());
    EXPECT_STREQ("", clipHtml.utf8().c_str());
}

class CreateChildCounterFrameClient : public FrameTestHelpers::TestWebFrameClient {
public:
    CreateChildCounterFrameClient() : m_count(0) { }
    WebFrame* createChildFrame(WebLocalFrame* parent, WebTreeScopeType, const WebString& frameName, WebSandboxFlags) override;

    int count() const { return m_count; }

private:
    int m_count;
};

WebFrame* CreateChildCounterFrameClient::createChildFrame(WebLocalFrame* parent, WebTreeScopeType scope, const WebString& frameName, WebSandboxFlags sandboxFlags)
{
    ++m_count;
    return TestWebFrameClient::createChildFrame(parent, scope, frameName, sandboxFlags);
}

TEST_F(WebViewTest, ChangeDisplayMode)
{
    WebView* webView = m_webViewHelper.initializeAndLoad("about:blank", true);

    WebScriptSource source("document.querySelector('body').innerHTML = window.matchMedia('(display-mode: minimal-ui)').matches");

    webView->mainFrame()->executeScript(source);
    std::string content = webView->mainFrame()->contentAsText(5).utf8();
    EXPECT_EQ("false", content);

    webView->setDisplayMode(WebDisplayModeMinimalUi);
    webView->mainFrame()->executeScript(source);
    content = webView->mainFrame()->contentAsText(5).utf8();
    EXPECT_EQ("true", content);
}

TEST_F(WebViewTest, AddFrameInCloseUnload)
{
    CreateChildCounterFrameClient frameClient;
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("add_frame_in_unload.html"));
    m_webViewHelper.initializeAndLoad(m_baseURL + "add_frame_in_unload.html", true, &frameClient);
    m_webViewHelper.reset();
    EXPECT_EQ(0, frameClient.count());
}

TEST_F(WebViewTest, AddFrameInCloseURLUnload)
{
    CreateChildCounterFrameClient frameClient;
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("add_frame_in_unload.html"));
    m_webViewHelper.initializeAndLoad(m_baseURL + "add_frame_in_unload.html", true, &frameClient);
    m_webViewHelper.webViewImpl()->mainFrame()->dispatchUnloadEvent();
    EXPECT_EQ(0, frameClient.count());
    m_webViewHelper.reset();
}

TEST_F(WebViewTest, AddFrameInNavigateUnload)
{
    CreateChildCounterFrameClient frameClient;
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("add_frame_in_unload.html"));
    m_webViewHelper.initializeAndLoad(m_baseURL + "add_frame_in_unload.html", true, &frameClient);
    FrameTestHelpers::loadFrame(m_webViewHelper.webView()->mainFrame(), "about:blank");
    EXPECT_EQ(0, frameClient.count());
    m_webViewHelper.reset();
}

TEST_F(WebViewTest, AddFrameInChildInNavigateUnload)
{
    CreateChildCounterFrameClient frameClient;
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("add_frame_in_unload_wrapper.html"));
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("add_frame_in_unload.html"));
    m_webViewHelper.initializeAndLoad(m_baseURL + "add_frame_in_unload_wrapper.html", true, &frameClient);
    FrameTestHelpers::loadFrame(m_webViewHelper.webView()->mainFrame(), "about:blank");
    EXPECT_EQ(1, frameClient.count());
    m_webViewHelper.reset();
}

class TouchEventHandlerWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    // WebWidgetClient methods
    void hasTouchEventHandlers(bool state) override
    {
        m_hasTouchEventHandlerCount[state]++;
    }

    // Local methods
    TouchEventHandlerWebViewClient() : m_hasTouchEventHandlerCount()
    {
    }

    int getAndResetHasTouchEventHandlerCallCount(bool state)
    {
        int value = m_hasTouchEventHandlerCount[state];
        m_hasTouchEventHandlerCount[state] = 0;
        return value;
    }

private:
    int m_hasTouchEventHandlerCount[2];
};

// This test verifies that WebWidgetClient::hasTouchEventHandlers is called
// accordingly for various calls to EventHandlerRegistry::did{Add|Remove|
// RemoveAll}EventHandler(..., TouchEvent). Verifying that those calls are made
// correctly is the job of LayoutTests/fast/events/event-handler-count.html.
TEST_F(WebViewTest, HasTouchEventHandlers)
{
    TouchEventHandlerWebViewClient client;
    std::string url = m_baseURL + "has_touch_event_handlers.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "has_touch_event_handlers.html");
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(url, true, 0, &client);
    const EventHandlerRegistry::EventHandlerClass touchEvent = EventHandlerRegistry::TouchEvent;

    // The page is initialized with at least one no-handlers call.
    // In practice we get two such calls because WebViewHelper::initializeAndLoad first
    // initializes and empty frame, and then loads a document into it, so there are two
    // FrameLoader::commitProvisionalLoad calls.
    EXPECT_GE(client.getAndResetHasTouchEventHandlerCallCount(false), 1);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Adding the first document handler results in a has-handlers call.
    Document* document = webViewImpl->mainFrameImpl()->frame()->document();
    EventHandlerRegistry* registry = &document->frameHost()->eventHandlerRegistry();
    registry->didAddEventHandler(*document, touchEvent);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Adding another handler has no effect.
    registry->didAddEventHandler(*document, touchEvent);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Removing the duplicate handler has no effect.
    registry->didRemoveEventHandler(*document, touchEvent);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Removing the final handler results in a no-handlers call.
    registry->didRemoveEventHandler(*document, touchEvent);
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Adding a handler on a div results in a has-handlers call.
    Element* parentDiv = document->getElementById("parentdiv");
    ASSERT(parentDiv);
    registry->didAddEventHandler(*parentDiv, touchEvent);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Adding a duplicate handler on the div, clearing all document handlers
    // (of which there are none) and removing the extra handler on the div
    // all have no effect.
    registry->didAddEventHandler(*parentDiv, touchEvent);
    registry->didRemoveAllEventHandlers(*document);
    registry->didRemoveEventHandler(*parentDiv, touchEvent);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Removing the final handler on the div results in a no-handlers call.
    registry->didRemoveEventHandler(*parentDiv, touchEvent);
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Adding two handlers then clearing them in a single call results in a
    // has-handlers then no-handlers call.
    registry->didAddEventHandler(*parentDiv, touchEvent);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(true));
    registry->didAddEventHandler(*parentDiv, touchEvent);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));
    registry->didRemoveAllEventHandlers(*parentDiv);
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Adding a handler inside of a child iframe results in a has-handlers call.
    Element* childFrame = document->getElementById("childframe");
    ASSERT(childFrame);
    Document* childDocument = toHTMLIFrameElement(childFrame)->contentDocument();
    Element* childDiv = childDocument->getElementById("childdiv");
    ASSERT(childDiv);
    registry->didAddEventHandler(*childDiv, touchEvent);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Adding and clearing handlers in the parent doc or elsewhere in the child doc
    // has no impact.
    registry->didAddEventHandler(*document, touchEvent);
    registry->didAddEventHandler(*childFrame, touchEvent);
    registry->didAddEventHandler(*childDocument, touchEvent);
    registry->didRemoveAllEventHandlers(*document);
    registry->didRemoveAllEventHandlers(*childFrame);
    registry->didRemoveAllEventHandlers(*childDocument);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Removing the final handler inside the child frame results in a no-handlers call.
    registry->didRemoveAllEventHandlers(*childDiv);
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Adding a handler inside the child frame results in a has-handlers call.
    registry->didAddEventHandler(*childDocument, touchEvent);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Adding a handler in the parent document and removing the one in the frame
    // has no effect.
    registry->didAddEventHandler(*childFrame, touchEvent);
    registry->didRemoveEventHandler(*childDocument, touchEvent);
    registry->didRemoveAllEventHandlers(*childDocument);
    registry->didRemoveAllEventHandlers(*document);
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Now removing the handler in the parent document results in a no-handlers call.
    registry->didRemoveEventHandler(*childFrame, touchEvent);
    EXPECT_EQ(1, client.getAndResetHasTouchEventHandlerCallCount(false));
    EXPECT_EQ(0, client.getAndResetHasTouchEventHandlerCallCount(true));

    // Free the webView before the TouchEventHandlerWebViewClient gets freed.
    m_webViewHelper.reset();
}

// This test checks that deleting nodes which have only non-JS-registered touch
// handlers also removes them from the event handler registry. Note that this
// is different from detaching and re-attaching the same node, which is covered
// by layout tests under fast/events/.
TEST_F(WebViewTest, DeleteElementWithRegisteredHandler)
{
    std::string url = m_baseURL + "simple_div.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "simple_div.html");
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(url, true);

    RefPtrWillBePersistent<Document> document = webViewImpl->mainFrameImpl()->frame()->document();
    Element* div = document->getElementById("div");
    EventHandlerRegistry& registry = document->frameHost()->eventHandlerRegistry();

    registry.didAddEventHandler(*div, EventHandlerRegistry::ScrollEvent);
    EXPECT_TRUE(registry.hasEventHandlers(EventHandlerRegistry::ScrollEvent));

    TrackExceptionState exceptionState;
    div->remove(exceptionState);
#if ENABLE(OILPAN)
    // For oilpan we have to force a GC to ensure the event handlers have been removed when
    // checking below. We do a precise GC (collectAllGarbage does not scan the stack)
    // to ensure the div element dies. This is also why the Document is in a Persistent
    // since we want that to stay around.
    Heap::collectAllGarbage();
#endif
    EXPECT_FALSE(registry.hasEventHandlers(EventHandlerRegistry::ScrollEvent));
}

class NonUserInputTextUpdateWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    NonUserInputTextUpdateWebViewClient() : m_textIsUpdated(false) { }

    // WebWidgetClient methods
    void didUpdateTextOfFocusedElementByNonUserInput() override
    {
        m_textIsUpdated = true;
    }

    void reset()
    {
        m_textIsUpdated = false;
    }

    bool textIsUpdated() const
    {
        return m_textIsUpdated;
    }

private:
    int m_textIsUpdated;
};

// This test verifies the text input flags are correctly exposed to script.
TEST_F(WebViewTest, TextInputFlags)
{
    NonUserInputTextUpdateWebViewClient client;
    std::string url = m_baseURL + "text_input_flags.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "text_input_flags.html");
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(url, true, 0, &client);
    webViewImpl->setInitialFocus(false);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewImpl->mainFrame());
    HTMLDocument* document = toHTMLDocument(frame->frame()->document());

    // (A) <input>
    // (A.1) Verifies autocorrect/autocomplete/spellcheck flags are Off and
    // autocapitalize is set to none.
    HTMLInputElement* inputElement = toHTMLInputElement(document->getElementById("input"));
    document->setFocusedElement(inputElement);
    webViewImpl->setFocus(true);
    WebTextInputInfo info = webViewImpl->textInputInfo();
    EXPECT_EQ(
        WebTextInputFlagAutocompleteOff | WebTextInputFlagAutocorrectOff | WebTextInputFlagSpellcheckOff | WebTextInputFlagAutocapitalizeNone,
        info.flags);

    // (A.2) Verifies autocorrect/autocomplete/spellcheck flags are On and
    // autocapitalize is set to sentences.
    inputElement = toHTMLInputElement(document->getElementById("input2"));
    document->setFocusedElement(inputElement);
    webViewImpl->setFocus(true);
    info = webViewImpl->textInputInfo();
    EXPECT_EQ(
        WebTextInputFlagAutocompleteOn | WebTextInputFlagAutocorrectOn | WebTextInputFlagSpellcheckOn | WebTextInputFlagAutocapitalizeSentences,
        info.flags);

    // (B) <textarea> Verifies the default text input flags are
    // WebTextInputFlagAutocapitalizeSentences.
    HTMLTextAreaElement* textAreaElement = toHTMLTextAreaElement(document->getElementById("textarea"));
    document->setFocusedElement(textAreaElement);
    webViewImpl->setFocus(true);
    info = webViewImpl->textInputInfo();
    EXPECT_EQ(WebTextInputFlagAutocapitalizeSentences, info.flags);

    // Free the webView before freeing the NonUserInputTextUpdateWebViewClient.
    m_webViewHelper.reset();
}

// This test verifies that WebWidgetClient::didUpdateTextOfFocusedElementByNonUserInput is
// called iff value of a focused element is modified via script.
TEST_F(WebViewTest, NonUserInputTextUpdate)
{
    NonUserInputTextUpdateWebViewClient client;
    std::string url = m_baseURL + "non_user_input_text_update.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "non_user_input_text_update.html");
    WebViewImpl* webViewImpl = m_webViewHelper.initializeAndLoad(url, true, 0, &client);
    webViewImpl->setInitialFocus(false);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewImpl->mainFrame());
    HTMLDocument* document = toHTMLDocument(frame->frame()->document());

    // (A) <input>
    // (A.1) Focused and value is changed by script.
    client.reset();
    EXPECT_FALSE(client.textIsUpdated());

    HTMLInputElement* inputElement = toHTMLInputElement(document->getElementById("input"));
    document->setFocusedElement(inputElement);
    webViewImpl->setFocus(true);
    EXPECT_EQ(document->focusedElement(), static_cast<Element*>(inputElement));

    // Emulate value change from script.
    inputElement->setValue("testA");
    EXPECT_TRUE(client.textIsUpdated());
    WebTextInputInfo info = webViewImpl->textInputInfo();
    EXPECT_EQ("testA", std::string(info.value.utf8().data()));

    // (A.2) Focused and user input modifies value.
    client.reset();
    EXPECT_FALSE(client.textIsUpdated());

    WebVector<WebCompositionUnderline> emptyUnderlines;
    webViewImpl->setComposition(WebString::fromUTF8("2"), emptyUnderlines, 1, 1);
    webViewImpl->confirmComposition(WebWidget::KeepSelection);
    EXPECT_FALSE(client.textIsUpdated());
    info = webViewImpl->textInputInfo();
    EXPECT_EQ("testA2", std::string(info.value.utf8().data()));

    // (A.3) Unfocused and value is changed by script.
    client.reset();
    EXPECT_FALSE(client.textIsUpdated());
    document->setFocusedElement(nullptr);
    webViewImpl->setFocus(false);
    EXPECT_NE(document->focusedElement(), static_cast<Element*>(inputElement));
    inputElement->setValue("testA3");
    EXPECT_FALSE(client.textIsUpdated());

    // (B) <textarea>
    // (B.1) Focused and value is changed by script.
    client.reset();
    EXPECT_FALSE(client.textIsUpdated());
    HTMLTextAreaElement* textAreaElement = toHTMLTextAreaElement(document->getElementById("textarea"));
    document->setFocusedElement(textAreaElement);
    webViewImpl->setFocus(true);
    EXPECT_EQ(document->focusedElement(), static_cast<Element*>(textAreaElement));
    textAreaElement->setValue("testB");
    EXPECT_TRUE(client.textIsUpdated());
    info = webViewImpl->textInputInfo();
    EXPECT_EQ("testB", std::string(info.value.utf8().data()));

    // (B.2) Focused and user input modifies value.
    client.reset();
    EXPECT_FALSE(client.textIsUpdated());
    webViewImpl->setComposition(WebString::fromUTF8("2"), emptyUnderlines, 1, 1);
    webViewImpl->confirmComposition(WebWidget::KeepSelection);
    info = webViewImpl->textInputInfo();
    EXPECT_EQ("testB2", std::string(info.value.utf8().data()));

    // (B.3) Unfocused and value is changed by script.
    client.reset();
    EXPECT_FALSE(client.textIsUpdated());
    document->setFocusedElement(nullptr);
    webViewImpl->setFocus(false);
    EXPECT_NE(document->focusedElement(), static_cast<Element*>(textAreaElement));
    inputElement->setValue("testB3");
    EXPECT_FALSE(client.textIsUpdated());

    // Free the webView before freeing the NonUserInputTextUpdateWebViewClient.
    m_webViewHelper.reset();
}

// Check that the WebAutofillClient is correctly notified about first user
// gestures after load, following various input events.
TEST_F(WebViewTest, FirstUserGestureObservedKeyEvent)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("form.html"));
    MockAutofillClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "form.html", true);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setAutofillClient(&client);
    webView->setInitialFocus(false);

    EXPECT_EQ(0, client.getUserGestureNotificationsCount());

    WebKeyboardEvent keyEvent;
    keyEvent.windowsKeyCode = VKEY_SPACE;
    keyEvent.type = WebInputEvent::RawKeyDown;
    keyEvent.setKeyIdentifierFromWindowsKeyCode();
    webView->handleInputEvent(keyEvent);
    keyEvent.type = WebInputEvent::KeyUp;
    webView->handleInputEvent(keyEvent);

    EXPECT_EQ(1, client.getUserGestureNotificationsCount());
    frame->setAutofillClient(0);
}

TEST_F(WebViewTest, FirstUserGestureObservedMouseEvent)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("form.html"));
    MockAutofillClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "form.html", true);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setAutofillClient(&client);
    webView->setInitialFocus(false);

    EXPECT_EQ(0, client.getUserGestureNotificationsCount());

    WebMouseEvent mouseEvent;
    mouseEvent.button = WebMouseEvent::ButtonLeft;
    mouseEvent.x = 1;
    mouseEvent.y = 1;
    mouseEvent.clickCount = 1;
    mouseEvent.type = WebInputEvent::MouseDown;
    webView->handleInputEvent(mouseEvent);
    mouseEvent.type = WebInputEvent::MouseUp;
    webView->handleInputEvent(mouseEvent);

    EXPECT_EQ(1, client.getUserGestureNotificationsCount());
    frame->setAutofillClient(0);
}

TEST_F(WebViewTest, FirstUserGestureObservedGestureTap)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("longpress_selection.html"));
    MockAutofillClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "longpress_selection.html", true);
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->setAutofillClient(&client);
    webView->setInitialFocus(false);

    EXPECT_EQ(0, client.getUserGestureNotificationsCount());

    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target")));

    EXPECT_EQ(1, client.getUserGestureNotificationsCount());
    frame->setAutofillClient(0);
}

TEST_F(WebViewTest, CompositionIsUserGesture)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("input_field_populated.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "input_field_populated.html");
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    MockAutofillClient client;
    frame->setAutofillClient(&client);
    webView->setInitialFocus(false);

    EXPECT_TRUE(webView->setComposition(WebString::fromUTF8(std::string("hello").c_str()), WebVector<WebCompositionUnderline>(), 3, 3));
    EXPECT_EQ(1, client.textChangesFromUserGesture());
    EXPECT_FALSE(UserGestureIndicator::processingUserGesture());
    EXPECT_TRUE(frame->hasMarkedText());

    frame->setAutofillClient(0);
}

TEST_F(WebViewTest, CompareSelectAllToContentAsText)
{
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("longpress_selection.html"));
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + "longpress_selection.html", true);

    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());
    frame->executeScript(WebScriptSource(WebString::fromUTF8("document.execCommand('SelectAll', false, null)")));
    std::string actual = frame->selectionAsText().utf8();

    const int kMaxOutputCharacters = 1024;
    std::string expected = frame->contentAsText(kMaxOutputCharacters).utf8();
    EXPECT_EQ(expected, actual);
}

TEST_F(WebViewTest, AutoResizeSubtreeLayout)
{
    std::string url = m_baseURL + "subtree-layout.html";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "subtree-layout.html");
    WebView* webView = m_webViewHelper.initialize(true);

    webView->enableAutoResizeMode(WebSize(200, 200), WebSize(200, 200));
    loadFrame(webView->mainFrame(), url);

    FrameView* frameView = m_webViewHelper.webViewImpl()->mainFrameImpl()->frameView();

    // Auto-resizing used to ASSERT(needsLayout()) in LayoutBlockFlow::layout. This EXPECT is
    // merely a dummy. The real test is that we don't trigger asserts in debug builds.
    EXPECT_FALSE(frameView->needsLayout());
};

TEST_F(WebViewTest, PreferredSize)
{
    std::string url = m_baseURL + "specify_size.html?100px:100px";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "specify_size.html");
    WebView* webView = m_webViewHelper.initializeAndLoad(url, true);

    WebSize size = webView->contentsPreferredMinimumSize();
    EXPECT_EQ(100, size.width);
    EXPECT_EQ(100, size.height);

    webView->setZoomLevel(WebView::zoomFactorToZoomLevel(2.0));
    size = webView->contentsPreferredMinimumSize();
    EXPECT_EQ(200, size.width);
    EXPECT_EQ(200, size.height);

    // Verify that both width and height are rounded (in this case up)
    webView->setZoomLevel(WebView::zoomFactorToZoomLevel(0.9995));
    size = webView->contentsPreferredMinimumSize();
    EXPECT_EQ(100, size.width);
    EXPECT_EQ(100, size.height);

    // Verify that both width and height are rounded (in this case down)
    webView->setZoomLevel(WebView::zoomFactorToZoomLevel(1.0005));
    size = webView->contentsPreferredMinimumSize();
    EXPECT_EQ(100, size.width);
    EXPECT_EQ(100, size.height);

    url = m_baseURL + "specify_size.html?1.5px:1.5px";
    URLTestHelpers::registerMockedURLLoad(toKURL(url), "specify_size.html");
    webView = m_webViewHelper.initializeAndLoad(url, true);

    webView->setZoomLevel(WebView::zoomFactorToZoomLevel(1));
    size = webView->contentsPreferredMinimumSize();
    EXPECT_EQ(2, size.width);
    EXPECT_EQ(2, size.height);
}

class UnhandledTapWebViewClient : public FrameTestHelpers::TestWebViewClient {
public:
    void showUnhandledTapUIIfNeeded(const WebPoint& tappedPosition, const WebNode& tappedNode, bool pageChanged) override
    {
        m_wasCalled = true;
        m_tappedPosition = tappedPosition;
        m_tappedNode = tappedNode;
        m_pageChanged = pageChanged;
    }
    bool getWasCalled() const { return m_wasCalled; }
    int getTappedXPos() const { return m_tappedPosition.x(); }
    int getTappedYPos() const { return m_tappedPosition.y(); }
    bool isTappedNodeNull() const { return m_tappedNode.isNull(); }
    const WebNode& getWebNode() const { return m_tappedNode; }
    bool getPageChanged() const { return m_pageChanged; }
    void reset()
    {
        m_wasCalled = false;
        m_tappedPosition = IntPoint();
        m_tappedNode = WebNode();
        m_pageChanged = false;
    }
private:
    bool m_wasCalled = false;
    IntPoint m_tappedPosition;
    WebNode m_tappedNode;
    bool m_pageChanged = false;
};

TEST_F(WebViewTest, ShowUnhandledTapUIIfNeeded)
{
    std::string testFile = "show_unhandled_tap.html";
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("Ahem.ttf"));
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(testFile));
    UnhandledTapWebViewClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + testFile, true, 0, &client);
    webView->resize(WebSize(500, 300));
    webView->layout();
    runPendingTasks();

    // Scroll the bottom into view so we can distinguish window coordinates from document coordinates.
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("bottom")));
    EXPECT_TRUE(client.getWasCalled());
    EXPECT_EQ(64, client.getTappedXPos());
    EXPECT_EQ(278, client.getTappedYPos());
    EXPECT_FALSE(client.isTappedNodeNull());
    EXPECT_TRUE(client.getWebNode().isTextNode());

    // Test basic tap handling and notification.
    client.reset();
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target")));
    EXPECT_TRUE(client.getWasCalled());
    EXPECT_EQ(144, client.getTappedXPos());
    EXPECT_EQ(82, client.getTappedYPos());
    EXPECT_FALSE(client.isTappedNodeNull());
    EXPECT_TRUE(client.getWebNode().isTextNode());
    // Make sure the returned text node has the parent element that was our target.
    EXPECT_EQ(webView->mainFrame()->document().getElementById("target"), client.getWebNode().parentNode());

    // Test correct conversion of coordinates to viewport space under pinch-zoom.
    webView->setPageScaleFactor(2);
    webView->setPinchViewportOffset(WebFloatPoint(50, 20));
    client.reset();
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target")));
    EXPECT_TRUE(client.getWasCalled());
    EXPECT_EQ(188, client.getTappedXPos());
    EXPECT_EQ(124, client.getTappedYPos());

    m_webViewHelper.reset(); // Remove dependency on locally scoped client.
}

#define TEST_EACH_MOUSEEVENT(handler, EXPECT) \
frame->executeScript(WebScriptSource("setTest('mousedown-" handler "');")); \
EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target"))); \
EXPECT_##EXPECT(client.getPageChanged()); \
client.reset(); \
frame->executeScript(WebScriptSource("setTest('mouseup-" handler "');")); \
EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target"))); \
EXPECT_##EXPECT(client.getPageChanged()); \
client.reset(); \
frame->executeScript(WebScriptSource("setTest('mousemove-" handler "');")); \
EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target"))); \
EXPECT_##EXPECT(client.getPageChanged()); \
client.reset(); \
frame->executeScript(WebScriptSource("setTest('click-" handler "');")); \
EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target"))); \
EXPECT_##EXPECT(client.getPageChanged());

TEST_F(WebViewTest, ShowUnhandledTapUIIfNeededWithMutateDom)
{
    std::string testFile = "show_unhandled_tap.html";
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("Ahem.ttf"));
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(testFile));
    UnhandledTapWebViewClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + testFile, true, 0, &client);
    webView->resize(WebSize(500, 300));
    webView->layout();
    runPendingTasks();
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());

    // Test dom mutation.
    TEST_EACH_MOUSEEVENT("mutateDom", TRUE);

    // Test without any DOM mutation.
    client.reset();
    frame->executeScript(WebScriptSource("setTest('none');"));
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target")));
    EXPECT_FALSE(client.getPageChanged());

    m_webViewHelper.reset(); // Remove dependency on locally scoped client.
}

TEST_F(WebViewTest, ShowUnhandledTapUIIfNeededWithMutateStyle)
{
    std::string testFile = "show_unhandled_tap.html";
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("Ahem.ttf"));
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(testFile));
    UnhandledTapWebViewClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + testFile, true, 0, &client);
    webView->resize(WebSize(500, 300));
    webView->layout();
    runPendingTasks();
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());

    // Test style mutation.
    TEST_EACH_MOUSEEVENT("mutateStyle", TRUE);

    // Test checkbox:indeterminate style mutation.
    TEST_EACH_MOUSEEVENT("mutateIndeterminate", TRUE);

    // Test click div with :active style but it is not covered for now.
    client.reset();
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("style_active")));
    EXPECT_FALSE(client.getPageChanged());

    // Test without any style mutation.
    client.reset();
    frame->executeScript(WebScriptSource("setTest('none');"));
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target")));
    EXPECT_FALSE(client.getPageChanged());

    m_webViewHelper.reset(); // Remove dependency on locally scoped client.
}

TEST_F(WebViewTest, ShowUnhandledTapUIIfNeededWithPreventDefault)
{
    std::string testFile = "show_unhandled_tap.html";
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8("Ahem.ttf"));
    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(testFile));
    UnhandledTapWebViewClient client;
    WebView* webView = m_webViewHelper.initializeAndLoad(m_baseURL + testFile, true, 0, &client);
    webView->resize(WebSize(500, 300));
    webView->layout();
    runPendingTasks();
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webView->mainFrame());

    // Testswallowing.
    TEST_EACH_MOUSEEVENT("preventDefault", FALSE);

    // Test without any preventDefault.
    client.reset();
    frame->executeScript(WebScriptSource("setTest('none');"));
    EXPECT_TRUE(tapElementById(webView, WebInputEvent::GestureTap, WebString::fromUTF8("target")));
    EXPECT_TRUE(client.getWasCalled());

    m_webViewHelper.reset(); // Remove dependency on locally scoped client.
}

// Test 3 frames, all on the same layer (i.e. [1 2 3])
TEST_F(WebViewTest, TestPushFrameTimingRequestRectsToGraphicsLayer1)
{
    std::string url = m_baseURL + "frame_timing_inner.html?100px:100px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_inner.html?200px:200px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_inner.html?300px:300px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_1.html";
    registerMockedURLLoad(toKURL(url), "frame_timing_1.html");

    WebView* webView = m_webViewHelper.initialize(true);
    loadFrame(webView->mainFrame(), url);

    webView->resize(WebSize(800, 600));
    webView->layout();

    WebViewImpl* webViewImpl = toWebViewImpl(webView);

    Frame* frame = webViewImpl->page()->mainFrame();
    int64_t id = frame->frameID();

    EXPECT_EQ(3u, frame->tree().childCount());

    WebVector<std::pair<int64_t, WebRect>> frameTimingRequests =
        toLocalFrame(frame)->document()
        ->layoutView()->enclosingLayer()
        ->enclosingLayerForPaintInvalidationCrossingFrameBoundaries()
        ->graphicsLayerBacking()->platformLayer()->frameTimingRequests();

    EXPECT_EQ(4u, frameTimingRequests.size());
    EXPECT_EQ(id + 0, frameTimingRequests[0].first);
    EXPECT_EQ(WebRect(0, 0, 800, 600), frameTimingRequests[0].second);
    EXPECT_EQ(id + 1, frameTimingRequests[1].first);
    EXPECT_EQ(WebRect(2, 2, 100, 100), frameTimingRequests[1].second);
    EXPECT_EQ(id + 2, frameTimingRequests[2].first);
    EXPECT_EQ(WebRect(106, 2, 200, 200), frameTimingRequests[2].second);
    EXPECT_EQ(id + 3, frameTimingRequests[3].first);
    EXPECT_EQ(WebRect(310, 2, 300, 300), frameTimingRequests[3].second);
}

// Test 3 frames, where frame 2 is on a different GraphicsLayer (i.e. [1 2' 3])
TEST_F(WebViewTest, TestPushFrameTimingRequestRectsToGraphicsLayer2)
{
    std::string url = m_baseURL + "frame_timing_inner.html?100px:100px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_inner.html?200px:200px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_inner.html?300px:300px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_2.html";
    registerMockedURLLoad(toKURL(url), "frame_timing_2.html");

    WebView* webView = m_webViewHelper.initialize(true);
    loadFrame(webView->mainFrame(), url);

    webView->resize(WebSize(800, 600));
    webView->layout();

    WebViewImpl* webViewImpl = toWebViewImpl(webView);

    Frame* frame = webViewImpl->page()->mainFrame();
    int64_t id = frame->frameID();

    EXPECT_EQ(3u, frame->tree().childCount());

    const WebLayer* graphicsLayer1 =
        toLocalFrame(webViewImpl->page()->mainFrame())->document()
        ->layoutView()->enclosingLayer()
        ->enclosingLayerForPaintInvalidationCrossingFrameBoundaries()
        ->graphicsLayerBacking()->platformLayer();
    WebVector<std::pair<int64_t, WebRect>> frameTimingRequests1 =
        graphicsLayer1->frameTimingRequests();

    EXPECT_EQ(3u, frameTimingRequests1.size());
    EXPECT_EQ(id + 0, frameTimingRequests1[0].first);
    EXPECT_EQ(WebRect(0, 0, 800, 600), frameTimingRequests1[0].second);
    EXPECT_EQ(id + 1, frameTimingRequests1[1].first);
    EXPECT_EQ(WebRect(2, 2, 100, 100), frameTimingRequests1[1].second);
    EXPECT_EQ(id + 3, frameTimingRequests1[2].first);
    EXPECT_EQ(WebRect(310, 2, 300, 300), frameTimingRequests1[2].second);

    const WebLayer* graphicsLayer2 = nullptr;
    for (Frame* frame = webViewImpl->page()->mainFrame(); frame;
        frame = frame->tree().traverseNext()) {
        graphicsLayer2 = toLocalFrame(frame)->document()->layoutView()
            ->enclosingLayer()
            ->enclosingLayerForPaintInvalidationCrossingFrameBoundaries()
            ->graphicsLayerBacking()->platformLayer();
        if (graphicsLayer2 != graphicsLayer1)
            break;
    }
    WebVector<std::pair<int64_t, WebRect>> frameTimingRequests2 =
        graphicsLayer2->frameTimingRequests();
    EXPECT_EQ(1u, frameTimingRequests2.size());
    EXPECT_EQ(id + 2, frameTimingRequests2[0].first);
    EXPECT_EQ(WebRect(2, 2, 200, 200), frameTimingRequests2[0].second);
}


// Test nested frames (i.e. [1 2'[4 5'] 3])
TEST_F(WebViewTest, TestPushFrameTimingRequestRectsToGraphicsLayer3)
{
    std::string url = m_baseURL + "frame_timing_inner.html?100px:100px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_inner.html?200px:200px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_inner.html?300px:300px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_b.html";
    registerMockedURLLoad(toKURL(url), "frame_timing_b.html");
    url = m_baseURL + "frame_timing_3.html";
    registerMockedURLLoad(toKURL(url), "frame_timing_3.html");

    WebView* webView = m_webViewHelper.initialize(true);
    loadFrame(webView->mainFrame(), url);

    webView->resize(WebSize(800, 600));
    webView->layout();

    WebViewImpl* webViewImpl = toWebViewImpl(webView);

    Frame* frame = webViewImpl->page()->mainFrame();
    int64_t id = frame->frameID();

    EXPECT_EQ(3u, frame->tree().childCount());

    const WebLayer* graphicsLayer1 =
        toLocalFrame(webViewImpl->page()->mainFrame())->document()
        ->layoutView()->enclosingLayer()
        ->enclosingLayerForPaintInvalidationCrossingFrameBoundaries()
        ->graphicsLayerBacking()->platformLayer();
    WebVector<std::pair<int64_t, WebRect>> frameTimingRequests1 =
        graphicsLayer1->frameTimingRequests();

    EXPECT_EQ(3u, frameTimingRequests1.size());
    EXPECT_EQ(id + 0, frameTimingRequests1[0].first);
    EXPECT_EQ(WebRect(0, 0, 800, 600), frameTimingRequests1[0].second);
    EXPECT_EQ(WebRect(2, 2, 100, 100), frameTimingRequests1[1].second);
    EXPECT_EQ(WebRect(410, 2, 200, 200), frameTimingRequests1[2].second);

    const WebLayer* graphicsLayer2 = nullptr;
    for (Frame* frame = webViewImpl->page()->mainFrame(); frame;
        frame = frame->tree().traverseNext()) {
        graphicsLayer2 = toLocalFrame(frame)->document()->layoutView()
            ->enclosingLayer()
            ->enclosingLayerForPaintInvalidationCrossingFrameBoundaries()
            ->graphicsLayerBacking()->platformLayer();
        if (graphicsLayer2 != graphicsLayer1)
            break;
    }
    WebVector<std::pair<int64_t, WebRect>> frameTimingRequests2 =
        graphicsLayer2->frameTimingRequests();
    EXPECT_EQ(2u, frameTimingRequests2.size());
    EXPECT_EQ(WebRect(0, 0, 300, 300), frameTimingRequests2[0].second);
    EXPECT_EQ(WebRect(2, 2, 100, 100), frameTimingRequests2[1].second);

    const WebLayer* graphicsLayer3 = nullptr;
    for (Frame* frame = webViewImpl->page()->mainFrame(); frame;
        frame = frame->tree().traverseNext()) {
        graphicsLayer3 = toLocalFrame(frame)->document()->layoutView()
            ->enclosingLayer()
            ->enclosingLayerForPaintInvalidationCrossingFrameBoundaries()
            ->graphicsLayerBacking()->platformLayer();
        if (graphicsLayer3 != graphicsLayer1 && graphicsLayer3 != graphicsLayer2)
            break;
    }
    WebVector<std::pair<int64_t, WebRect>> frameTimingRequests3 =
        graphicsLayer3->frameTimingRequests();
    EXPECT_EQ(1u, frameTimingRequests3.size());
    EXPECT_EQ(WebRect(2, 2, 100, 100), frameTimingRequests3[0].second);
}

// Test 3 frames, all on the same layer (i.e. [1 2 3])
// Fails on android, see crbug.com/488381.
#if OS(ANDROID)
TEST_F(WebViewTest, DISABLED_TestRecordFrameTimingEvents)
#else
TEST_F(WebViewTest, TestRecordFrameTimingEvents)
#endif
{
    std::string url = m_baseURL + "frame_timing_inner.html?100px:100px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_inner.html?200px:200px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_inner.html?300px:300px";
    registerMockedURLLoad(toKURL(url), "frame_timing_inner.html");
    url = m_baseURL + "frame_timing_1.html";
    registerMockedURLLoad(toKURL(url), "frame_timing_1.html");

    WebView* webView = m_webViewHelper.initialize(true);
    loadFrame(webView->mainFrame(), url);

    webView->resize(WebSize(800, 600));
    webView->layout();

    WebViewImpl* webViewImpl = toWebViewImpl(webView);

    Frame* frame = webViewImpl->page()->mainFrame();
    int64_t id = frame->frameID();

    std::vector<WebFrameTimingEvent> compositePairs(3);
    compositePairs[0] = WebFrameTimingEvent(1, 2.0);
    compositePairs[1] = WebFrameTimingEvent(2, 3.0);
    compositePairs[2] = WebFrameTimingEvent(3, 4.0);
    WebVector<WebFrameTimingEvent> compositeEvents(compositePairs);
    webViewImpl->recordFrameTimingEvent(WebView::CompositeEvent, id, compositeEvents);
    PerformanceEntryVector composites = DOMWindowPerformance::performance(*frame->domWindow())->getEntriesByType("composite");
    PerformanceEntryVector renders = DOMWindowPerformance::performance(*frame->domWindow())->getEntriesByType("render");
    ASSERT_EQ(3ul, composites.size());
    ASSERT_EQ(0ul, renders.size());
    for (size_t i = 0; i < composites.size(); ++i) {
        double docTime = frame->domWindow()->document()->loader()->timing().monotonicTimeToZeroBasedDocumentTime(compositePairs[i].startTime) * 1000.0;
        ASSERT_EQ(docTime, composites[i]->startTime());
    }

    // Skip ahead to subframe 2.
    frame = frame->tree().traverseNext();
    frame = frame->tree().traverseNext();
    id += 2;

    std::vector<WebFrameTimingEvent> renderPairs(4);
    renderPairs[0] = WebFrameTimingEvent(4, 5.0, 6.0);
    renderPairs[1] =WebFrameTimingEvent(5, 6.0, 7.0);
    renderPairs[2] =WebFrameTimingEvent(6, 7.0, 8.0);
    renderPairs[3] =WebFrameTimingEvent(7, 8.0, 9.0);
    WebVector<WebFrameTimingEvent> renderEvents(renderPairs);
    webViewImpl->recordFrameTimingEvent(WebView::RenderEvent, id, renderEvents);
    composites = DOMWindowPerformance::performance(*frame->domWindow())->getEntriesByType("composite");
    renders = DOMWindowPerformance::performance(*frame->domWindow())->getEntriesByType("render");
    ASSERT_EQ(0ul, composites.size());
    ASSERT_EQ(4ul, renders.size());
    for (size_t i = 0; i < renders.size(); ++i) {
        double docStartTime = frame->domWindow()->document()->loader()->timing().monotonicTimeToZeroBasedDocumentTime(renderPairs[i].startTime) * 1000.0;
        ASSERT_DOUBLE_EQ(docStartTime, renders[i]->startTime());
        double docFinishTime = frame->domWindow()->document()->loader()->timing().monotonicTimeToZeroBasedDocumentTime(renderPairs[i].finishTime) * 1000.0;
        double docDuration = docFinishTime - docStartTime;
        ASSERT_DOUBLE_EQ(docDuration, renders[i]->duration());
    }
}

} // namespace blink
