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

#include "platform/testing/URLTestHelpers.h"
#include "public/web/WebCache.h"
#include "public/web/WebDocument.h"
#include "public/web/WebElement.h"
#include "public/web/WebFrame.h"
#include "public/web/WebNode.h"
#include "public/web/WebNodeList.h"
#include "public/web/WebPrerendererClient.h"
#include "public/web/WebScriptSource.h"
#include "public/web/WebView.h"
#include "public/web/WebViewClient.h"
#include "web/tests/FrameTestHelpers.h"

#include "public/platform/Platform.h"
#include "public/platform/WebPrerender.h"
#include "public/platform/WebPrerenderingSupport.h"
#include "public/platform/WebString.h"
#include "public/platform/WebUnitTestSupport.h"
#include "wtf/OwnPtr.h"
#include <functional>
#include <gtest/gtest.h>
#include <list>

using namespace blink;
using blink::URLTestHelpers::toKURL;

namespace {

WebURL toWebURL(const char* url)
{
    return WebURL(toKURL(url));
}

class TestPrerendererClient : public WebPrerendererClient {
public:
    TestPrerendererClient() { }
    virtual ~TestPrerendererClient() { }

    void setExtraDataForNextPrerender(WebPrerender::ExtraData* extraData)
    {
        ASSERT(!m_extraData);
        m_extraData = adoptPtr(extraData);
    }

    WebPrerender releaseWebPrerender()
    {
        ASSERT(!m_webPrerenders.empty());
        WebPrerender retval(m_webPrerenders.front());
        m_webPrerenders.pop_front();
        return retval;
    }

    bool empty() const
    {
        return m_webPrerenders.empty();
    }

    void clear()
    {
        m_webPrerenders.clear();
    }

private:
    // From WebPrerendererClient:
    void willAddPrerender(WebPrerender* prerender) override
    {
        prerender->setExtraData(m_extraData.leakPtr());

        ASSERT(!prerender->isNull());
        m_webPrerenders.push_back(*prerender);
    }

    OwnPtr<WebPrerender::ExtraData> m_extraData;
    std::list<WebPrerender> m_webPrerenders;
};

class TestPrerenderingSupport : public WebPrerenderingSupport {
public:
    TestPrerenderingSupport()
    {
        initialize(this);
    }

    ~TestPrerenderingSupport() override
    {
        shutdown();
    }

    void clear()
    {
        m_addedPrerenders.clear();
        m_canceledPrerenders.clear();
        m_abandonedPrerenders.clear();
    }

    size_t totalCount() const
    {
        return m_addedPrerenders.size() + m_canceledPrerenders.size() + m_abandonedPrerenders.size();
    }

    size_t addCount(const WebPrerender& prerender) const
    {
        return std::count_if(m_addedPrerenders.begin(), m_addedPrerenders.end(), std::bind1st(WebPrerenderEqual(), prerender));
    }

    size_t cancelCount(const WebPrerender& prerender) const
    {
        return std::count_if(m_canceledPrerenders.begin(), m_canceledPrerenders.end(), std::bind1st(WebPrerenderEqual(), prerender));
    }

    size_t abandonCount(const WebPrerender& prerender) const
    {
        return std::count_if(m_abandonedPrerenders.begin(), m_abandonedPrerenders.end(), std::bind1st(WebPrerenderEqual(), prerender));
    }

private:
    class WebPrerenderEqual : public std::binary_function<WebPrerender, WebPrerender, bool> {
    public:
        bool operator()(const WebPrerender& first, const WebPrerender& second) const
        {
            return first.toPrerender() == second.toPrerender();
        }
    };

    // From WebPrerenderingSupport:
    void add(const WebPrerender& prerender) override
    {
        m_addedPrerenders.push_back(prerender);
    }

    void cancel(const WebPrerender& prerender) override
    {
        m_canceledPrerenders.push_back(prerender);
    }

    void abandon(const WebPrerender& prerender) override
    {
        m_abandonedPrerenders.push_back(prerender);
    }

    std::vector<WebPrerender> m_addedPrerenders;
    std::vector<WebPrerender> m_canceledPrerenders;
    std::vector<WebPrerender> m_abandonedPrerenders;
};

class PrerenderingTest : public testing::Test {
public:
    ~PrerenderingTest()
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

    void initialize(const char* baseURL, const char* fileName)
    {
        URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL), WebString::fromUTF8(fileName));
        const bool RunJavascript = true;
        m_webViewHelper.initialize(RunJavascript);
        m_webViewHelper.webView()->setPrerendererClient(&m_prerendererClient);

        FrameTestHelpers::loadFrame(m_webViewHelper.webView()->mainFrame(), std::string(baseURL) + fileName);
    }

    void navigateAway()
    {
        FrameTestHelpers::loadFrame(m_webViewHelper.webView()->mainFrame(), "about:blank");
    }

    void close()
    {
        m_webViewHelper.webView()->mainFrame()->collectGarbage();
        m_webViewHelper.reset();

        WebCache::clear();
    }

    WebElement console()
    {
        WebElement console = m_webViewHelper.webView()->mainFrame()->document().getElementById("console");
        ASSERT(console.nodeName() == "UL");
        return console;
    }

    unsigned consoleLength()
    {
        return console().childNodes().length() - 1;
    }

    std::string consoleAt(unsigned i)
    {
        ASSERT(consoleLength() > i);

        WebNode consoleListItem = console().childNodes().item(1 + i);
        ASSERT(consoleListItem.nodeName() == "LI");
        ASSERT(consoleListItem.hasChildNodes());

        WebNode textNode = consoleListItem.firstChild();
        ASSERT(textNode.nodeName() == "#text");

        return textNode.nodeValue().utf8().data();
    }

    void executeScript(const char* code)
    {
        m_webViewHelper.webView()->mainFrame()->executeScript(WebScriptSource(WebString::fromUTF8(code)));
    }

    TestPrerenderingSupport* prerenderingSupport()
    {
        return &m_prerenderingSupport;
    }

    TestPrerendererClient* prerendererClient()
    {
        return &m_prerendererClient;
    }

private:
    TestPrerenderingSupport m_prerenderingSupport;
    TestPrerendererClient m_prerendererClient;

    FrameTestHelpers::WebViewHelper m_webViewHelper;
};

TEST_F(PrerenderingTest, SinglePrerender)
{
    initialize("http://www.foo.com/", "prerender/single_prerender.html");

    WebPrerender webPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_FALSE(webPrerender.isNull());
    EXPECT_EQ(toWebURL("http://prerender.com/"), webPrerender.url());
    EXPECT_EQ(PrerenderRelTypePrerender, webPrerender.relTypes());

    EXPECT_EQ(1u, prerenderingSupport()->addCount(webPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->totalCount());

    webPrerender.didStartPrerender();
    EXPECT_EQ(1u, consoleLength());
    EXPECT_EQ("webkitprerenderstart", consoleAt(0));

    webPrerender.didSendDOMContentLoadedForPrerender();
    EXPECT_EQ(2u, consoleLength());
    EXPECT_EQ("webkitprerenderdomcontentloaded", consoleAt(1));

    webPrerender.didSendLoadForPrerender();
    EXPECT_EQ(3u, consoleLength());
    EXPECT_EQ("webkitprerenderload", consoleAt(2));

    webPrerender.didStopPrerender();
    EXPECT_EQ(4u, consoleLength());
    EXPECT_EQ("webkitprerenderstop", consoleAt(3));
}

TEST_F(PrerenderingTest, CancelPrerender)
{
    initialize("http://www.foo.com/", "prerender/single_prerender.html");

    WebPrerender webPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_FALSE(webPrerender.isNull());

    EXPECT_EQ(1u, prerenderingSupport()->addCount(webPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->totalCount());

    executeScript("removePrerender()");

    EXPECT_EQ(1u, prerenderingSupport()->cancelCount(webPrerender));
    EXPECT_EQ(2u, prerenderingSupport()->totalCount());
}

TEST_F(PrerenderingTest, AbandonPrerender)
{
    initialize("http://www.foo.com/", "prerender/single_prerender.html");

    WebPrerender webPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_FALSE(webPrerender.isNull());

    EXPECT_EQ(1u, prerenderingSupport()->addCount(webPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->totalCount());

    navigateAway();

    EXPECT_EQ(1u, prerenderingSupport()->abandonCount(webPrerender));
    EXPECT_EQ(2u, prerenderingSupport()->totalCount());

    // Check that the prerender does not emit an extra cancel when garbage-collecting everything.
    close();

    EXPECT_EQ(2u, prerenderingSupport()->totalCount());
}

TEST_F(PrerenderingTest, ExtraData)
{
    class TestExtraData : public WebPrerender::ExtraData {
    public:
        explicit TestExtraData(bool* alive) : m_alive(alive)
        {
            *alive = true;
        }

        ~TestExtraData() override { *m_alive = false; }

    private:
        bool* m_alive;
    };

    bool alive = false;
    {
        prerendererClient()->setExtraDataForNextPrerender(new TestExtraData(&alive));
        initialize("http://www.foo.com/", "prerender/single_prerender.html");
        EXPECT_TRUE(alive);

        WebPrerender webPrerender = prerendererClient()->releaseWebPrerender();

        executeScript("removePrerender()");
        close();
        prerenderingSupport()->clear();
    }
    EXPECT_FALSE(alive);
}

TEST_F(PrerenderingTest, TwoPrerenders)
{
    initialize("http://www.foo.com/", "prerender/multiple_prerenders.html");

    WebPrerender firstPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_FALSE(firstPrerender.isNull());
    EXPECT_EQ(toWebURL("http://first-prerender.com/"), firstPrerender.url());

    WebPrerender secondPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_FALSE(firstPrerender.isNull());
    EXPECT_EQ(toWebURL("http://second-prerender.com/"), secondPrerender.url());

    EXPECT_EQ(1u, prerenderingSupport()->addCount(firstPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->addCount(secondPrerender));
    EXPECT_EQ(2u, prerenderingSupport()->totalCount());

    firstPrerender.didStartPrerender();
    EXPECT_EQ(1u, consoleLength());
    EXPECT_EQ("first_webkitprerenderstart", consoleAt(0));

    secondPrerender.didStartPrerender();
    EXPECT_EQ(2u, consoleLength());
    EXPECT_EQ("second_webkitprerenderstart", consoleAt(1));
}

TEST_F(PrerenderingTest, TwoPrerendersRemovingFirstThenNavigating)
{
    initialize("http://www.foo.com/", "prerender/multiple_prerenders.html");

    WebPrerender firstPrerender = prerendererClient()->releaseWebPrerender();
    WebPrerender secondPrerender = prerendererClient()->releaseWebPrerender();

    EXPECT_EQ(1u, prerenderingSupport()->addCount(firstPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->addCount(secondPrerender));
    EXPECT_EQ(2u, prerenderingSupport()->totalCount());

    executeScript("removeFirstPrerender()");

    EXPECT_EQ(1u, prerenderingSupport()->cancelCount(firstPrerender));
    EXPECT_EQ(3u, prerenderingSupport()->totalCount());

    navigateAway();

    EXPECT_EQ(1u, prerenderingSupport()->abandonCount(secondPrerender));
    EXPECT_EQ(4u, prerenderingSupport()->totalCount());
}

TEST_F(PrerenderingTest, TwoPrerendersAddingThird)
{
    initialize("http://www.foo.com/", "prerender/multiple_prerenders.html");

    WebPrerender firstPrerender = prerendererClient()->releaseWebPrerender();
    WebPrerender secondPrerender = prerendererClient()->releaseWebPrerender();

    EXPECT_EQ(1u, prerenderingSupport()->addCount(firstPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->addCount(secondPrerender));
    EXPECT_EQ(2u, prerenderingSupport()->totalCount());

    executeScript("addThirdPrerender()");

    WebPrerender thirdPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_EQ(1u, prerenderingSupport()->addCount(thirdPrerender));
    EXPECT_EQ(3u, prerenderingSupport()->totalCount());
}

TEST_F(PrerenderingTest, ShortLivedClient)
{
    initialize("http://www.foo.com/", "prerender/single_prerender.html");

    WebPrerender webPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_FALSE(webPrerender.isNull());

    EXPECT_EQ(1u, prerenderingSupport()->addCount(webPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->totalCount());

    navigateAway();
    close();

    // This test passes if this next line doesn't crash.
    webPrerender.didStartPrerender();
}

TEST_F(PrerenderingTest, FastRemoveElement)
{
    initialize("http://www.foo.com/", "prerender/single_prerender.html");

    WebPrerender webPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_FALSE(webPrerender.isNull());

    EXPECT_EQ(1u, prerenderingSupport()->addCount(webPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->totalCount());

    // Race removing & starting the prerender against each other, as if the element was removed very quickly.
    executeScript("removePrerender()");
    EXPECT_FALSE(webPrerender.isNull());
    webPrerender.didStartPrerender();

    // The page should be totally disconnected from the Prerender at this point, so the console should not have updated.
    EXPECT_EQ(0u, consoleLength());
}

TEST_F(PrerenderingTest, MutateTarget)
{
    initialize("http://www.foo.com/", "prerender/single_prerender.html");

    WebPrerender webPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_FALSE(webPrerender.isNull());
    EXPECT_EQ(toWebURL("http://prerender.com/"), webPrerender.url());

    EXPECT_EQ(1u, prerenderingSupport()->addCount(webPrerender));
    EXPECT_EQ(0u, prerenderingSupport()->cancelCount(webPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->totalCount());

    // Change the href of this prerender, make sure this is treated as a remove and add.
    executeScript("mutateTarget()");
    EXPECT_EQ(1u, prerenderingSupport()->cancelCount(webPrerender));

    WebPrerender mutatedPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_EQ(toWebURL("http://mutated.com/"), mutatedPrerender.url());
    EXPECT_EQ(1u, prerenderingSupport()->addCount(webPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->addCount(mutatedPrerender));
    EXPECT_EQ(3u, prerenderingSupport()->totalCount());
}

TEST_F(PrerenderingTest, MutateRel)
{
    initialize("http://www.foo.com/", "prerender/single_prerender.html");

    WebPrerender webPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_FALSE(webPrerender.isNull());
    EXPECT_EQ(toWebURL("http://prerender.com/"), webPrerender.url());

    EXPECT_EQ(1u, prerenderingSupport()->addCount(webPrerender));
    EXPECT_EQ(0u, prerenderingSupport()->cancelCount(webPrerender));
    EXPECT_EQ(1u, prerenderingSupport()->totalCount());

    // Change the rel of this prerender, make sure this is treated as a remove.
    executeScript("mutateRel()");
    EXPECT_EQ(1u, prerenderingSupport()->cancelCount(webPrerender));
    EXPECT_EQ(2u, prerenderingSupport()->totalCount());
}

TEST_F(PrerenderingTest, RelNext)
{
    initialize("http://www.foo.com/", "prerender/rel_next_prerender.html");

    WebPrerender relNextOnly = prerendererClient()->releaseWebPrerender();
    EXPECT_EQ(toWebURL("http://rel-next-only.com/"), relNextOnly.url());
    EXPECT_EQ(PrerenderRelTypeNext, relNextOnly.relTypes());

    WebPrerender relNextAndPrerender = prerendererClient()->releaseWebPrerender();
    EXPECT_EQ(toWebURL("http://rel-next-and-prerender.com/"), relNextAndPrerender.url());
    EXPECT_EQ(static_cast<unsigned>(PrerenderRelTypeNext | PrerenderRelTypePrerender), relNextAndPrerender.relTypes());
}

} // namespace
