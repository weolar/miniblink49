/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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
#include "core/layout/LayoutGeometryMap.h"

#include "core/dom/Document.h"
#include "core/layout/LayoutBox.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/testing/URLTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebFrameClient.h"
#include "web/WebLocalFrameImpl.h"
#include "web/tests/FloatPointTestHelpers.h"
#include "web/tests/FloatQuadTestHelpers.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

class MockWebFrameClient : public WebFrameClient {
};

class LayoutGeometryMapTest : public testing::Test {
public:
    LayoutGeometryMapTest()
        : m_baseURL("http://www.test.com/")
    {
    }

    void TearDown() override
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

protected:
    static LayoutBox* getFrameElement(const char* iframeName, WebView* webView, const WTF::AtomicString& elementId)
    {
        WebLocalFrameImpl* iframe = toWebLocalFrameImpl(webView->findFrameByName(WebString::fromUTF8(iframeName)));
        if (!iframe)
            return nullptr;
        LocalFrame* frame = iframe->frame();
        Document* doc = frame->document();
        Element* element = doc->getElementById(elementId);
        if (!element)
            return nullptr;
        return element->layoutBox();
    }

    static LayoutBox* getLayoutBox(WebView* webView, const WTF::AtomicString& elementId)
    {
        WebViewImpl* webViewImpl = toWebViewImpl(webView);
        if (!webViewImpl)
            return nullptr;
        LocalFrame* frame = webViewImpl->mainFrameImpl()->frame();
        Document* doc = frame->document();
        Element* element = doc->getElementById(elementId);
        if (!element)
            return nullptr;
        return element->layoutBox();
    }

    static const LayoutBoxModelObject* getLayoutContainer(WebView* webView, const WTF::AtomicString &elementId)
    {
        LayoutBox* rb = getLayoutBox(webView, elementId);
        if (!rb)
            return nullptr;
        DeprecatedPaintLayer* compositingLayer = rb->enclosingLayer()->enclosingLayerForPaintInvalidation();
        if (!compositingLayer)
            return nullptr;
        return compositingLayer->layoutObject();
    }

    static const LayoutBoxModelObject* getFrameLayoutContainer(const char* frameId, WebView* webView, const WTF::AtomicString &elementId)
    {
        LayoutBox* rb = getFrameElement(frameId, webView, elementId);
        if (!rb)
            return nullptr;
        DeprecatedPaintLayer* compositingLayer = rb->enclosingLayer()->enclosingLayerForPaintInvalidation();
        if (!compositingLayer)
            return nullptr;
        return compositingLayer->layoutObject();
    }

    static const FloatRect rectFromQuad(const FloatQuad& quad)
    {
        FloatRect rect;
        rect.setX(std::min(quad.p1().x(), std::min(quad.p2().x(), std::min(quad.p3().x(), quad.p4().x()))));
        rect.setY(std::min(quad.p1().y(), std::min(quad.p2().y(), std::min(quad.p3().y(), quad.p4().y()))));

        rect.setWidth(std::max(quad.p1().x(), std::max(quad.p2().x(), std::max(quad.p3().x(), quad.p4().x()))) - rect.x());
        rect.setHeight(std::max(quad.p1().y(), std::max(quad.p2().y(), std::max(quad.p3().y(), quad.p4().y()))) - rect.y());
        return rect;
    }

    void registerMockedHttpURLLoad(const std::string& fileName)
    {
        URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(m_baseURL.c_str()), WebString::fromUTF8(fileName.c_str()));
    }

    const std::string m_baseURL;
    MockWebFrameClient m_mockWebViewClient;
};

TEST_F(LayoutGeometryMapTest, SimpleGeometryMapTest)
{
    registerMockedHttpURLLoad("rgm_test.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebView* webView = webViewHelper.initializeAndLoad(m_baseURL + "rgm_test.html", true, 0, 0);
    webView->resize(WebSize(1000, 1000));
    webView->layout();

    // We are going test everything twice. Once with FloatPoints and once with
    // FloatRects. This is because LayoutGeometryMap treats both slightly
    // differently
    LayoutGeometryMap rgm;
    rgm.pushMappingsToAncestor(getLayoutBox(webView, "InitialDiv"), 0);
    FloatPoint point;
    FloatRect rect(0.0f, 0.0f, 1.0f, 2.0f);
    EXPECT_EQ(FloatPoint(8.0f, 8.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(8.0f, 8.0f, 1.0f, 2.0f)), rgm.mapToContainer(rect, nullptr));

    rgm.popMappingsToAncestor(static_cast<DeprecatedPaintLayer*>(nullptr));
    EXPECT_EQ(FloatPoint(0.0f, 0.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(0.0f, 0.0f, 1.0f, 2.0f)), rgm.mapToContainer(rect, nullptr));

    rgm.pushMappingsToAncestor(getLayoutBox(webView, "InnerDiv"), 0);
    EXPECT_EQ(FloatPoint(21.0f, 6.0f), rgm.mapToContainer(point, getLayoutBox(webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(21.0f, 6.0f, 1.0f, 2.0f)), rgm.mapToContainer(rect, getLayoutBox(webView, "CenterDiv")));

    rgm.pushMappingsToAncestor(getLayoutBox(webView, "OtherDiv"), getLayoutBox(webView, "InnerDiv"));
    EXPECT_EQ(FloatPoint(22.0f, 12.0f), rgm.mapToContainer(point, getLayoutBox(webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(22.0f, 12.0f, 1.0f, 2.0f)), rgm.mapToContainer(rect, getLayoutBox(webView, "CenterDiv")));

    EXPECT_EQ(FloatPoint(1.0f, 6.0f), rgm.mapToContainer(point, getLayoutBox(webView, "InnerDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(1.0f, 6.0f, 1.0f, 2.0f)), rgm.mapToContainer(rect, getLayoutBox(webView, "InnerDiv")));

    EXPECT_EQ(FloatPoint(50.0f, 44.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(50.0f, 44.0f, 1.0f, 2.0f)), rgm.mapToContainer(rect, nullptr));
}

// Fails on Windows due to crbug.com/391457. When run through the transform the
// position on windows differs by a pixel
#if OS(WIN)
TEST_F(LayoutGeometryMapTest, DISABLED_TransformedGeometryTest)
#else
TEST_F(LayoutGeometryMapTest, TransformedGeometryTest)
#endif
{
    registerMockedHttpURLLoad("rgm_transformed_test.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebView* webView = webViewHelper.initializeAndLoad(m_baseURL + "rgm_transformed_test.html", true, 0, 0);
    webView->resize(WebSize(1000, 1000));
    webView->layout();

    LayoutGeometryMap rgm;
    rgm.pushMappingsToAncestor(getLayoutBox(webView, "InitialDiv"), 0);
    FloatPoint point;
    const float rectWidth = 15.0f;
    const float scaleWidth = 2.0f;
    const float scaleHeight = 3.0f;
    FloatRect rect(0.0f, 0.0f, 15.0f, 25.0f);
    EXPECT_EQ(FloatPoint(8.0f, 8.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(8.0f, 8.0f, 15.0f, 25.0f)), rgm.mapToContainer(rect, nullptr));

    rgm.popMappingsToAncestor(static_cast<DeprecatedPaintLayer*>(nullptr));
    EXPECT_EQ(FloatPoint(0.0f, 0.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(0.0f, 0.0f, 15.0f, 25.0f)).boundingBox(), rgm.mapToContainer(rect, nullptr).boundingBox());

    rgm.pushMappingsToAncestor(getLayoutBox(webView, "InnerDiv"), 0);
    EXPECT_EQ(FloatPoint(523.0f, 6.0f), rgm.mapToContainer(point, getLayoutBox(webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(523.0f - rectWidth, 6.0f, 15.0f, 25.0f)).boundingBox(), rgm.mapToContainer(rect, getLayoutBox(webView, "CenterDiv")).boundingBox());

    rgm.pushMappingsToAncestor(getLayoutBox(webView, "OtherDiv"), getLayoutBox(webView, "InnerDiv"));
    EXPECT_EQ(FloatPoint(522.0f, 12.0f), rgm.mapToContainer(point, getLayoutBox(webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(522.0f - rectWidth, 12.0f, 15.0f, 25.0f)).boundingBox(), rgm.mapToContainer(rect, getLayoutBox(webView, "CenterDiv")).boundingBox());

    EXPECT_EQ(FloatPoint(1.0f, 6.0f), rgm.mapToContainer(point, getLayoutBox(webView, "InnerDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(1.0f, 6.0f, 15.0f, 25.0f)).boundingBox(), rgm.mapToContainer(rect, getLayoutBox(webView, "InnerDiv")).boundingBox());

    EXPECT_EQ(FloatPoint(821.0f, 31.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(821.0f - rectWidth * scaleWidth, 31.0f, 15.0f * scaleWidth, 25.0f * scaleHeight)).boundingBox(), rgm.mapToContainer(rect, nullptr).boundingBox());

    point = FloatPoint(10.0f, 25.0f);
    rect = FloatRect(10.0f, 25.0f, 15.0f, 25.0f);
    EXPECT_EQ(FloatPoint(512.0f, 37.0f), rgm.mapToContainer(point, getLayoutBox(webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(512.0f - rectWidth, 37.0f, 15.0f, 25.0f)).boundingBox(), rgm.mapToContainer(rect, getLayoutBox(webView, "CenterDiv")).boundingBox());

    EXPECT_EQ(FloatPoint(11.0f, 31.0f), rgm.mapToContainer(point, getLayoutBox(webView, "InnerDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(11.0f, 31.0f, 15.0f, 25.0f)).boundingBox(), rgm.mapToContainer(rect, getLayoutBox(webView, "InnerDiv")).boundingBox());

    EXPECT_EQ(FloatPoint(801.0f, 106.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(801.0f - rectWidth * scaleWidth, 106.0f, 15.0f * scaleWidth, 25.0f * scaleHeight)).boundingBox(), rgm.mapToContainer(rect, nullptr).boundingBox());
}

TEST_F(LayoutGeometryMapTest, FixedGeometryTest)
{
    registerMockedHttpURLLoad("rgm_fixed_position_test.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebView* webView = webViewHelper.initializeAndLoad(m_baseURL + "rgm_fixed_position_test.html", true, 0, 0);
    webView->resize(WebSize(1000, 1000));
    webView->layout();

    LayoutGeometryMap rgm;
    rgm.pushMappingsToAncestor(getLayoutBox(webView, "InitialDiv"), 0);
    FloatPoint point;
    FloatRect rect(0.0f, 0.0f, 15.0f, 25.0f);
    EXPECT_EQ(FloatPoint(8.0f, 8.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(8.0f, 8.0f, 15.0f, 25.0f)), rgm.mapToContainer(rect, nullptr));

    rgm.popMappingsToAncestor(static_cast<DeprecatedPaintLayer*>(nullptr));
    EXPECT_EQ(FloatPoint(0.0f, 0.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(0.0f, 0.0f, 15.0f, 25.0f)), rgm.mapToContainer(rect, nullptr));

    rgm.pushMappingsToAncestor(getLayoutBox(webView, "InnerDiv"), 0);
    EXPECT_EQ(FloatPoint(20.0f, 14.0f), rgm.mapToContainer(point, getLayoutContainer(webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(20.0f, 14.0f, 15.0f, 25.0f)), rgm.mapToContainer(rect, nullptr));

    rgm.pushMappingsToAncestor(getLayoutBox(webView, "OtherDiv"), getLayoutBox(webView, "InnerDiv"));
    EXPECT_EQ(FloatPoint(21.0f, 20.0f), rgm.mapToContainer(point, getLayoutContainer(webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(21.0f, 20.0f, 15.0f, 25.0f)), rgm.mapToContainer(rect, getLayoutContainer(webView, "CenterDiv")));

    EXPECT_EQ(FloatPoint(21.0f, 20.0f), rgm.mapToContainer(point, getLayoutContainer(webView, "InnerDiv")));
    EXPECT_EQ(FloatPoint(21.0f, 20.0f), rgm.mapToContainer(point, nullptr));

    point = FloatPoint(10.0f, 25.0f);
    rect = FloatRect(22.0f, 15.2f, 15.3f, 0.0f);
    EXPECT_EQ(FloatQuad(FloatRect(43.0f, 35.2f, 15.3f, 0.0f)), rgm.mapToContainer(rect, getLayoutContainer(webView, "CenterDiv")));

    EXPECT_EQ(FloatPoint(31.0f, 45.0f), rgm.mapToContainer(point, getLayoutContainer(webView, "InnerDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(43.0f, 35.2f, 15.3f, 0.0f)), rgm.mapToContainer(rect, getLayoutContainer(webView, "InnerDiv")));

    EXPECT_EQ(FloatPoint(31.0f, 45.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(43.0f, 35.2f, 15.3f, 0.0f)), rgm.mapToContainer(rect, nullptr));
}

TEST_F(LayoutGeometryMapTest, IframeTest)
{
    registerMockedHttpURLLoad("rgm_iframe_test.html");
    registerMockedHttpURLLoad("rgm_test.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebView* webView = webViewHelper.initializeAndLoad(m_baseURL + "rgm_iframe_test.html", true, 0, 0);
    webView->resize(WebSize(1000, 1000));
    webView->layout();

    LayoutGeometryMap rgm(TraverseDocumentBoundaries);
    LayoutGeometryMap rgmNoFrame;

    rgmNoFrame.pushMappingsToAncestor(getFrameElement("test_frame", webView, "InitialDiv"), 0);
    rgm.pushMappingsToAncestor(getFrameElement("test_frame", webView, "InitialDiv"), 0);
    FloatPoint point;
    FloatRect rect(0.0f, 0.0f, 1.0f, 2.0f);

    EXPECT_EQ(FloatPoint(8.0f, 8.0f), rgmNoFrame.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(8.0f, 8.0f, 1.0f, 2.0f)), rgmNoFrame.mapToContainer(rect, nullptr));

    // Our initial rect looks like: (0, 0, 1, 2)
    //        p0_____
    //          |   |
    //          |   |
    //          |   |
    //          |___|
    // When we rotate we get a rect of the form:
    //         p0_
    //          / -_
    //         /   /
    //        /   /
    //         -_/
    // So it is sensible that the minimum y is the same as for a point at 0, 0.
    // The minimum x should be p0.x - 2 * sin(30deg) = p0.x - 1.
    // That maximum x should likewise be p0.x + cos(30deg) = p0.x + 0.866.
    // And the maximum y should be p0.x + sin(30deg) + 2*cos(30deg)
    //      = p0.y + 2.232.
    EXPECT_NEAR(70.5244f, rgm.mapToContainer(point, nullptr).x(), 0.0001f);
    EXPECT_NEAR(-44.0237f, rgm.mapToContainer(point, nullptr).y(), 0.0001f);
    EXPECT_NEAR(69.5244f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).x(), 0.0001f);
    EXPECT_NEAR(-44.0237, rectFromQuad(rgm.mapToContainer(rect, nullptr)).y(), 0.0001f);
    EXPECT_NEAR(1.866, rectFromQuad(rgm.mapToContainer(rect, nullptr)).width(), 0.0001f);
    EXPECT_NEAR(2.232, rectFromQuad(rgm.mapToContainer(rect, nullptr)).height(), 0.0001f);

    rgm.popMappingsToAncestor(static_cast<DeprecatedPaintLayer*>(nullptr));
    rgmNoFrame.popMappingsToAncestor(static_cast<DeprecatedPaintLayer*>(nullptr));
    EXPECT_EQ(FloatPoint(0.0f, 0.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatPoint(0.0f, 0.0f), rgmNoFrame.mapToContainer(point, nullptr));

    rgm.pushMappingsToAncestor(getFrameElement("test_frame", webView, "InnerDiv"), 0);
    rgmNoFrame.pushMappingsToAncestor(getFrameElement("test_frame", webView, "InnerDiv"), 0);
    EXPECT_EQ(FloatPoint(21.0f, 6.0f), rgm.mapToContainer(point, getFrameLayoutContainer("test_frame", webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(21.0f, 6.0f, 1.0f, 2.0f)), rgm.mapToContainer(rect, getFrameLayoutContainer("test_frame", webView, "CenterDiv")));
    EXPECT_EQ(FloatPoint(21.0f, 6.0f), rgmNoFrame.mapToContainer(point, getFrameLayoutContainer("test_frame", webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(21.0f, 6.0f, 1.0f, 2.0f)), rgmNoFrame.mapToContainer(rect, getFrameLayoutContainer("test_frame", webView, "CenterDiv")));

    rgm.pushMappingsToAncestor(getFrameElement("test_frame", webView, "OtherDiv"), getFrameLayoutContainer("test_frame", webView, "InnerDiv"));
    rgmNoFrame.pushMappingsToAncestor(getFrameElement("test_frame", webView, "OtherDiv"), getFrameLayoutContainer("test_frame", webView, "InnerDiv"));
    EXPECT_EQ(FloatPoint(22.0f, 12.0f), rgm.mapToContainer(point, getFrameLayoutContainer("test_frame", webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(22.0f, 12.0f, 1.0f, 2.0f)), rgm.mapToContainer(rect, getFrameLayoutContainer("test_frame", webView, "CenterDiv")));
    EXPECT_EQ(FloatPoint(22.0f, 12.0f), rgmNoFrame.mapToContainer(point, getFrameLayoutContainer("test_frame", webView, "CenterDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(22.0f, 12.0f, 1.0f, 2.0f)), rgmNoFrame.mapToContainer(rect, getFrameLayoutContainer("test_frame", webView, "CenterDiv")));

    EXPECT_EQ(FloatPoint(1.0f, 6.0f), rgm.mapToContainer(point, getFrameLayoutContainer("test_frame", webView, "InnerDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(1.0f, 6.0f, 1.0f, 2.0f)), rgm.mapToContainer(rect, getFrameLayoutContainer("test_frame", webView, "InnerDiv")));
    EXPECT_EQ(FloatPoint(1.0f, 6.0f), rgmNoFrame.mapToContainer(point, getFrameLayoutContainer("test_frame", webView, "InnerDiv")));
    EXPECT_EQ(FloatQuad(FloatRect(1.0f, 6.0f, 1.0f, 2.0f)), rgmNoFrame.mapToContainer(rect, getFrameLayoutContainer("test_frame", webView, "InnerDiv")));

    EXPECT_NEAR(88.8975f, rgm.mapToContainer(point, nullptr).x(), 0.0001f);
    EXPECT_NEAR(8.1532f, rgm.mapToContainer(point, nullptr).y(), 0.0001f);
    EXPECT_NEAR(87.8975f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).x(), 0.0001f);
    EXPECT_NEAR(8.1532f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).y(), 0.0001f);
    EXPECT_NEAR(1.866, rectFromQuad(rgm.mapToContainer(rect, nullptr)).width(), 0.0001f);
    EXPECT_NEAR(2.232, rectFromQuad(rgm.mapToContainer(rect, nullptr)).height(), 0.0001f);

    EXPECT_EQ(FloatPoint(50.0f, 44.0f), rgmNoFrame.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(50.0f, 44.0f, 1.0f, 2.0f)), rgmNoFrame.mapToContainer(rect, nullptr));
}

TEST_F(LayoutGeometryMapTest, ColumnTest)
{
    registerMockedHttpURLLoad("rgm_column_test.html");
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebView* webView = webViewHelper.initializeAndLoad(m_baseURL + "rgm_column_test.html", true, 0, 0);
    webView->resize(WebSize(1000, 1000));
    webView->layout();

    // The document is 1000f wide (we resized to that size).
    // We have a 8px margin on either side of the document.
    // Between each column we have a 10px gap, and we have 3 columns.
    // The width of a given column is (1000 - 16 - 20)/3.
    // The total offset between any column and it's neighbour is width + 10px
    // (for the gap).
    float offset = (1000.0f - 16.0f - 20.0f) / 3.0f + 10.0f;

    LayoutGeometryMap rgm;
    rgm.pushMappingsToAncestor(getLayoutBox(webView, "A"), 0);
    FloatPoint point;
    FloatRect rect(0.0f, 0.0f, 5.0f, 3.0f);

    EXPECT_EQ(FloatPoint(8.0f, 8.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(8.0f, 8.0f, 5.0f, 3.0f)), rgm.mapToContainer(rect, nullptr));

    rgm.popMappingsToAncestor(static_cast<DeprecatedPaintLayer*>(nullptr));
    EXPECT_EQ(FloatPoint(0.0f, 0.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(0.0f, 0.0f, 5.0f, 3.0f)), rgm.mapToContainer(rect, nullptr));

    rgm.pushMappingsToAncestor(getLayoutBox(webView, "Col1"), 0);
    EXPECT_EQ(FloatPoint(8.0f, 8.0f), rgm.mapToContainer(point, nullptr));
    EXPECT_EQ(FloatQuad(FloatRect(8.0f, 8.0f, 5.0f, 3.0f)), rgm.mapToContainer(rect, nullptr));

    rgm.popMappingsToAncestor(static_cast<DeprecatedPaintLayer*>(nullptr));
    rgm.pushMappingsToAncestor(getLayoutBox(webView, "Col2"), nullptr);
    EXPECT_NEAR(8.0f + offset, rgm.mapToContainer(point, nullptr).x(), 0.1f);
    EXPECT_NEAR(8.0f, rgm.mapToContainer(point, nullptr).y(), 0.1f);
    EXPECT_NEAR(8.0f + offset, rectFromQuad(rgm.mapToContainer(rect, nullptr)).x(), 0.1f);
    EXPECT_NEAR(8.0f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).y(), 0.1f);
    EXPECT_EQ(5.0f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).width());
    EXPECT_EQ(3.0f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).height());

    rgm.popMappingsToAncestor(static_cast<DeprecatedPaintLayer*>(nullptr));
    rgm.pushMappingsToAncestor(getLayoutBox(webView, "Col3"), nullptr);
    EXPECT_NEAR(8.0f + offset * 2.0f, rgm.mapToContainer(point, nullptr).x(), 0.1f);
    EXPECT_NEAR(8.0f, rgm.mapToContainer(point, nullptr).y(), 0.1f);
    EXPECT_NEAR(8.0f + offset * 2.0f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).x(), 0.1f);
    EXPECT_NEAR(8.0f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).y(), 0.1f);
    EXPECT_EQ(5.0f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).width());
    EXPECT_EQ(3.0f, rectFromQuad(rgm.mapToContainer(rect, nullptr)).height());

}

} // namespace blink
