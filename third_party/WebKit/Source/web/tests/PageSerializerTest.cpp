/*
 * Copyright (c) 2013, Opera Software ASA. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Opera Software ASA nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/page/PageSerializer.h"

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8BindingForTesting.h"
#include "core/page/Page.h"
#include "platform/SerializedResource.h"
#include "platform/testing/URLTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebString.h"
#include "public/platform/WebThread.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebURLRequest.h"
#include "public/platform/WebURLResponse.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebSettings.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include "wtf/Vector.h"
#include <gtest/gtest.h>

using blink::URLTestHelpers::toKURL;
using blink::URLTestHelpers::registerMockedURLLoad;

namespace blink {

class PageSerializerTest : public testing::Test {
public:
    PageSerializerTest()
        : m_folder(WebString::fromUTF8("pageserializer/"))
        , m_baseUrl(toKURL("http://www.test.com"))
    {
    }

protected:
    void SetUp() override
    {
        // We want the images to load and JavaScript to be on.
        m_helper.initialize(true, 0, 0, &configureSettings);
    }

    void TearDown() override
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

    void setBaseFolder(const char* folder)
    {
        m_folder = WebString::fromUTF8(folder);
    }

    void setRewriteURLFolder(const char* folder)
    {
        m_rewriteFolder = folder;
    }

    void registerURL(const char* url, const char* file, const char* mimeType)
    {
        registerMockedURLLoad(KURL(m_baseUrl, url), WebString::fromUTF8(file), m_folder, WebString::fromUTF8(mimeType));
    }

    void registerURL(const char* file, const char* mimeType)
    {
        registerURL(file, file, mimeType);
    }

    void registerErrorURL(const char* file, int statusCode)
    {
        WebURLError error;
        error.reason = 0xdead + statusCode;
        error.domain = "PageSerializerTest";

        WebURLResponse response;
        response.initialize();
        response.setMIMEType("text/html");
        response.setHTTPStatusCode(statusCode);

        Platform::current()->unitTestSupport()->registerMockedErrorURL(KURL(m_baseUrl, file), response, error);
    }

    void registerRewriteURL(const char* fromURL, const char* toURL)
    {
        m_rewriteURLs.add(fromURL, toURL);
    }

    void serialize(const char* url)
    {
        FrameTestHelpers::loadFrame(m_helper.webView()->mainFrame(), KURL(m_baseUrl, url).string().utf8().data());
        PageSerializer serializer(&m_resources, nullptr);

        serializer.setRewriteURLFolder(m_rewriteFolder);
        for (const auto& rewriteURL: m_rewriteURLs)
            serializer.registerRewriteURL(rewriteURL.key, rewriteURL.value);

        serializer.serialize(m_helper.webViewImpl()->mainFrameImpl()->frame()->page());
    }

    Vector<SerializedResource>& getResources()
    {
        return m_resources;
    }


    const SerializedResource* getResource(const char* url, const char* mimeType)
    {
        KURL kURL = KURL(m_baseUrl, url);
        String mime(mimeType);
        for (size_t i = 0; i < m_resources.size(); ++i) {
            const SerializedResource& resource = m_resources[i];
            if (resource.url == kURL && !resource.data->isEmpty()
                && (mime.isNull() || equalIgnoringCase(resource.mimeType, mime)))
                return &resource;
        }
        return nullptr;
    }

    bool isSerialized(const char* url, const char* mimeType = 0)
    {
        return getResource(url, mimeType);
    }

    String getSerializedData(const char* url, const char* mimeType = 0)
    {
        const SerializedResource* resource = getResource(url, mimeType);
        if (resource)
            return String(resource->data->data(), resource->data->size());
        return String();
    }

private:
    static void configureSettings(WebSettings* settings)
    {
        settings->setImagesEnabled(true);
        settings->setLoadsImagesAutomatically(true);
        settings->setJavaScriptEnabled(true);
    }

    FrameTestHelpers::WebViewHelper m_helper;
    WebString m_folder;
    KURL m_baseUrl;
    Vector<SerializedResource> m_resources;
    HashMap<String, String> m_rewriteURLs;
    String m_rewriteFolder;
};

TEST_F(PageSerializerTest, HTMLElements)
{
    setBaseFolder("pageserializer/elements/");

    registerURL("elements.html", "text/html");
    registerURL("style.css", "style.css", "text/css");
    registerURL("copyright.html", "empty.txt", "text/html");
    registerURL("script.js", "empty.txt", "text/javascript");

    registerURL("bodyBackground.png", "image.png", "image/png");

    registerURL("imageSrc.png", "image.png", "image/png");

    registerURL("inputImage.png", "image.png", "image/png");

    registerURL("tableBackground.png", "image.png", "image/png");
    registerURL("trBackground.png", "image.png", "image/png");
    registerURL("tdBackground.png", "image.png", "image/png");

    registerURL("blockquoteCite.html", "empty.txt", "text/html");
    registerURL("qCite.html", "empty.txt", "text/html");
    registerURL("delCite.html", "empty.txt", "text/html");
    registerURL("insCite.html", "empty.txt", "text/html");

    registerErrorURL("nonExisting.png", 404);

    serialize("elements.html");

    EXPECT_EQ(8U, getResources().size());

    EXPECT_TRUE(isSerialized("elements.html", "text/html"));
    EXPECT_TRUE(isSerialized("style.css", "text/css"));
    EXPECT_TRUE(isSerialized("bodyBackground.png", "image/png"));
    EXPECT_TRUE(isSerialized("imageSrc.png", "image/png"));
    EXPECT_TRUE(isSerialized("inputImage.png", "image/png"));
    EXPECT_TRUE(isSerialized("tableBackground.png", "image/png"));
    EXPECT_TRUE(isSerialized("trBackground.png", "image/png"));
    EXPECT_TRUE(isSerialized("tdBackground.png", "image/png"));
    EXPECT_FALSE(isSerialized("nonExisting.png", "image/png"));
}

TEST_F(PageSerializerTest, Frames)
{
    setBaseFolder("pageserializer/frames/");

    registerURL("simple_frames.html", "text/html");
    registerURL("simple_frames_top.html", "text/html");
    registerURL("simple_frames_1.html", "text/html");
    registerURL("simple_frames_3.html", "text/html");

    registerURL("frame_1.png", "image.png", "image/png");
    registerURL("frame_2.png", "image.png", "image/png");
    registerURL("frame_3.png", "image.png", "image/png");
    registerURL("frame_4.png", "image.png", "image/png");

    serialize("simple_frames.html");

    EXPECT_EQ(8U, getResources().size());

    EXPECT_TRUE(isSerialized("simple_frames.html", "text/html"));
    EXPECT_TRUE(isSerialized("simple_frames_top.html", "text/html"));
    EXPECT_TRUE(isSerialized("simple_frames_1.html", "text/html"));
    EXPECT_TRUE(isSerialized("simple_frames_3.html", "text/html"));

    EXPECT_TRUE(isSerialized("frame_1.png", "image/png"));
    EXPECT_TRUE(isSerialized("frame_2.png", "image/png"));
    EXPECT_TRUE(isSerialized("frame_3.png", "image/png"));
    EXPECT_TRUE(isSerialized("frame_4.png", "image/png"));
}

TEST_F(PageSerializerTest, IFrames)
{
    setBaseFolder("pageserializer/frames/");

    registerURL("top_frame.html", "text/html");
    registerURL("simple_iframe.html", "text/html");
    registerURL("object_iframe.html", "text/html");
    registerURL("embed_iframe.html", "text/html");

    registerURL("top.png", "image.png", "image/png");
    registerURL("simple.png", "image.png", "image/png");
    registerURL("object.png", "image.png", "image/png");
    registerURL("embed.png", "image.png", "image/png");

    serialize("top_frame.html");

    EXPECT_EQ(8U, getResources().size());

    EXPECT_TRUE(isSerialized("top_frame.html", "text/html"));
    EXPECT_TRUE(isSerialized("simple_iframe.html", "text/html"));
    EXPECT_TRUE(isSerialized("object_iframe.html", "text/html"));
    EXPECT_TRUE(isSerialized("embed_iframe.html", "text/html"));

    EXPECT_TRUE(isSerialized("top.png", "image/png"));
    EXPECT_TRUE(isSerialized("simple.png", "image/png"));
    EXPECT_TRUE(isSerialized("object.png", "image/png"));
    EXPECT_TRUE(isSerialized("embed.png", "image/png"));
}

// Tests that when serializing a page with blank frames these are reported with their resources.
TEST_F(PageSerializerTest, BlankFrames)
{
    setBaseFolder("pageserializer/frames/");

    registerURL("blank_frames.html", "text/html");
    registerURL("red_background.png", "image.png", "image/png");
    registerURL("orange_background.png", "image.png", "image/png");
    registerURL("blue_background.png", "image.png", "image/png");

    serialize("blank_frames.html");

    EXPECT_EQ(7U, getResources().size());

    EXPECT_TRUE(isSerialized("http://www.test.com/red_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("http://www.test.com/orange_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("http://www.test.com/blue_background.png", "image/png"));
    // The blank frames should have got a magic URL.
    EXPECT_TRUE(isSerialized("wyciwyg://frame/0", "text/html"));
    EXPECT_TRUE(isSerialized("wyciwyg://frame/1", "text/html"));
    EXPECT_TRUE(isSerialized("wyciwyg://frame/2", "text/html"));
}

TEST_F(PageSerializerTest, CSS)
{
    setBaseFolder("pageserializer/css/");

    registerURL("css_test_page.html", "text/html");
    registerURL("link_styles.css", "text/css");
    registerURL("import_style_from_link.css", "text/css");
    registerURL("import_styles.css", "text/css");
    registerURL("do_not_serialize.png", "image.png", "image/png");
    registerURL("red_background.png", "image.png", "image/png");
    registerURL("orange_background.png", "image.png", "image/png");
    registerURL("yellow_background.png", "image.png", "image/png");
    registerURL("green_background.png", "image.png", "image/png");
    registerURL("blue_background.png", "image.png", "image/png");
    registerURL("purple_background.png", "image.png", "image/png");
    registerURL("pink_background.png", "image.png", "image/png");
    registerURL("brown_background.png", "image.png", "image/png");
    registerURL("ul-dot.png", "image.png", "image/png");
    registerURL("ol-dot.png", "image.png", "image/png");

    serialize("css_test_page.html");

    EXPECT_EQ(14U, getResources().size());

    EXPECT_FALSE(isSerialized("do_not_serialize.png", "image/png"));

    EXPECT_TRUE(isSerialized("css_test_page.html", "text/html"));
    EXPECT_TRUE(isSerialized("link_styles.css", "text/css"));
    EXPECT_TRUE(isSerialized("import_styles.css", "text/css"));
    EXPECT_TRUE(isSerialized("import_style_from_link.css", "text/css"));
    EXPECT_TRUE(isSerialized("red_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("orange_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("yellow_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("green_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("blue_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("purple_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("pink_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("brown_background.png", "image/png"));
    EXPECT_TRUE(isSerialized("ul-dot.png", "image/png"));
    EXPECT_TRUE(isSerialized("ol-dot.png", "image/png"));
}

TEST_F(PageSerializerTest, CSSImport)
{
    setBaseFolder("pageserializer/css/");

    registerURL("import.html", "text/html");
    registerURL("import/base.css", "text/css");
    registerURL("import/relative/red-background.css", "text/css");
    registerURL("import/absolute/green-header.css", "text/css");

    serialize("import.html");

    EXPECT_TRUE(isSerialized("import.html", "text/html"));
    EXPECT_TRUE(isSerialized("import/base.css", "text/css"));
    EXPECT_TRUE(isSerialized("import/relative/red-background.css", "text/css"));
    EXPECT_TRUE(isSerialized("import/absolute/green-header.css", "text/css"));
}

TEST_F(PageSerializerTest, XMLDeclaration)
{
    V8TestingScope scope(v8::Isolate::GetCurrent());
    setBaseFolder("pageserializer/xml/");

    registerURL("xmldecl.xml", "text/xml");
    serialize("xmldecl.xml");

    String expectedStart("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    EXPECT_TRUE(getSerializedData("xmldecl.xml").startsWith(expectedStart));
}

TEST_F(PageSerializerTest, DTD)
{
    setBaseFolder("pageserializer/dtd/");

    registerURL("html5.html", "text/html");
    serialize("html5.html");

    String expectedStart("<!DOCTYPE html>");
    EXPECT_TRUE(getSerializedData("html5.html").startsWith(expectedStart));
}

TEST_F(PageSerializerTest, Font)
{
    setBaseFolder("pageserializer/font/");

    registerURL("font.html", "text/html");
    registerURL("font.ttf", "application/octet-stream");

    serialize("font.html");

    EXPECT_TRUE(isSerialized("font.ttf", "application/octet-stream"));
}

TEST_F(PageSerializerTest, DataURI)
{
    setBaseFolder("pageserializer/datauri/");

    registerURL("page_with_data.html", "text/html");

    serialize("page_with_data.html");

    EXPECT_EQ(1U, getResources().size());
    EXPECT_TRUE(isSerialized("page_with_data.html", "text/html"));
}

TEST_F(PageSerializerTest, DataURIMorphing)
{
    setBaseFolder("pageserializer/datauri/");

    registerURL("page_with_morphing_data.html", "text/html");

    serialize("page_with_morphing_data.html");

    EXPECT_EQ(2U, getResources().size());
    EXPECT_TRUE(isSerialized("page_with_morphing_data.html", "text/html"));
}

TEST_F(PageSerializerTest, RewriteLinksSimple)
{
    setBaseFolder("pageserializer/rewritelinks/");
    setRewriteURLFolder("folder");

    registerURL("rewritelinks_simple.html", "text/html");
    registerURL("absolute.png", "image.png", "image/png");
    registerURL("relative.png", "image.png", "image/png");
    registerRewriteURL("http://www.test.com/absolute.png", "a.png");
    registerRewriteURL("http://www.test.com/relative.png", "b.png");

    serialize("rewritelinks_simple.html");

    EXPECT_EQ(3U, getResources().size());
    EXPECT_NE(getSerializedData("rewritelinks_simple.html", "text/html").find("\"folder/a.png\""), kNotFound);
    EXPECT_NE(getSerializedData("rewritelinks_simple.html", "text/html").find("\"folder/b.png\""), kNotFound);
}

TEST_F(PageSerializerTest, RewriteLinksBase)
{
    setBaseFolder("pageserializer/rewritelinks/");
    setRewriteURLFolder("folder");

    registerURL("rewritelinks_base.html", "text/html");
    registerURL("images/here/image.png", "image.png", "image/png");
    registerURL("images/here/or/in/here/image.png", "image.png", "image/png");
    registerURL("or/absolute.png", "image.png", "image/png");
    registerRewriteURL("http://www.test.com/images/here/image.png", "a.png");
    registerRewriteURL("http://www.test.com/images/here/or/in/here/image.png", "b.png");
    registerRewriteURL("http://www.test.com/or/absolute.png", "c.png");

    serialize("rewritelinks_base.html");

    EXPECT_EQ(4U, getResources().size());
    EXPECT_NE(getSerializedData("rewritelinks_base.html", "text/html").find("\"folder/a.png\""), kNotFound);
    EXPECT_NE(getSerializedData("rewritelinks_base.html", "text/html").find("\"folder/b.png\""), kNotFound);
    EXPECT_NE(getSerializedData("rewritelinks_base.html", "text/html").find("\"folder/c.png\""), kNotFound);
}

// Test that we don't regress https://bugs.webkit.org/show_bug.cgi?id=99105
TEST_F(PageSerializerTest, SVGImageDontCrash)
{
    setBaseFolder("pageserializer/svg/");

    registerURL("page_with_svg_image.html", "text/html");
    registerURL("green_rectangle.svg", "image/svg+xml");

    serialize("page_with_svg_image.html");

    EXPECT_EQ(2U, getResources().size());

    EXPECT_TRUE(isSerialized("green_rectangle.svg", "image/svg+xml"));
    EXPECT_GT(getSerializedData("green_rectangle.svg", "image/svg+xml").length(), 250U);
}

TEST_F(PageSerializerTest, DontIncludeErrorImage)
{
    setBaseFolder("pageserializer/image/");

    registerURL("page_with_img_error.html", "text/html");
    registerURL("error_image.png", "image/png");

    serialize("page_with_img_error.html");

    EXPECT_EQ(1U, getResources().size());
    EXPECT_TRUE(isSerialized("page_with_img_error.html", "text/html"));
    EXPECT_FALSE(isSerialized("error_image.png", "image/png"));
}

TEST_F(PageSerializerTest, NamespaceElementsDontCrash)
{
    setBaseFolder("pageserializer/namespace/");

    registerURL("namespace_element.html", "text/html");

    serialize("namespace_element.html");

    EXPECT_EQ(1U, getResources().size());
    EXPECT_TRUE(isSerialized("namespace_element.html", "text/html"));
    EXPECT_GT(getSerializedData("namespace_element.html", "text/html").length(), 0U);
}

} // namespace blink
