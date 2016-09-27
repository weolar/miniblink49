// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/web/WebDocument.h"

#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/StyleEngine.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLLinkElement.h"
#include "core/style/ComputedStyle.h"
#include "core/page/Page.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/Color.h"
#include "platform/testing/URLTestHelpers.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

using blink::FrameTestHelpers::WebViewHelper;
using blink::URLTestHelpers::toKURL;

const char* kDefaultOrigin = "https://example.test/";
const char* kManifestDummyFilePath = "manifest-dummy.html";

class WebDocumentTest : public ::testing::Test {
protected:
    static void SetUpTestCase();

    void loadURL(const std::string& url);
    Document* topDocument() const;
    WebDocument topWebDocument() const;

    WebViewHelper m_webViewHelper;
};

void WebDocumentTest::SetUpTestCase()
{
    URLTestHelpers::registerMockedURLLoad(toKURL(std::string(kDefaultOrigin) + kManifestDummyFilePath), WebString::fromUTF8(kManifestDummyFilePath));
}

void WebDocumentTest::loadURL(const std::string& url)
{
    m_webViewHelper.initializeAndLoad(url);
}

Document* WebDocumentTest::topDocument() const
{
    return toLocalFrame(m_webViewHelper.webViewImpl()->page()->mainFrame())->document();
}

WebDocument WebDocumentTest::topWebDocument() const
{
    return m_webViewHelper.webView()->mainFrame()->document();
}

TEST_F(WebDocumentTest, InsertStyleSheet)
{
    loadURL("about:blank");

    WebDocument webDoc = topWebDocument();
    Document* coreDoc = topDocument();

    webDoc.insertStyleSheet("body { color: green }");

    // Check insertStyleSheet did not cause a synchronous style recalc.
    unsigned accessCount = coreDoc->styleEngine().resolverAccessCount();
    ASSERT_EQ(0U, accessCount);

    HTMLElement* bodyElement = coreDoc->body();
    ASSERT(bodyElement);

    const ComputedStyle& styleBeforeInsertion = bodyElement->computedStyleRef();

    // Inserted stylesheet not yet applied.
    ASSERT_EQ(Color(0, 0, 0), styleBeforeInsertion.visitedDependentColor(CSSPropertyColor));

    // Apply inserted stylesheet.
    coreDoc->updateLayoutTreeIfNeeded();

    const ComputedStyle& styleAfterInsertion = bodyElement->computedStyleRef();

    // Inserted stylesheet applied.
    ASSERT_EQ(Color(0, 128, 0), styleAfterInsertion.visitedDependentColor(CSSPropertyColor));
}

TEST_F(WebDocumentTest, ManifestURL)
{
    loadURL(std::string(kDefaultOrigin) + kManifestDummyFilePath);

    WebDocument webDoc = topWebDocument();
    Document* document = topDocument();
    HTMLLinkElement* linkManifest = document->linkManifest();

    // No href attribute was set.
    ASSERT_EQ(linkManifest->href(), static_cast<KURL>(webDoc.manifestURL()));

    // Set to some absolute url.
    linkManifest->setAttribute(HTMLNames::hrefAttr, "http://example.com/manifest.json");
    ASSERT_EQ(linkManifest->href(), static_cast<KURL>(webDoc.manifestURL()));

    // Set to some relative url.
    linkManifest->setAttribute(HTMLNames::hrefAttr, "static/manifest.json");
    ASSERT_EQ(linkManifest->href(), static_cast<KURL>(webDoc.manifestURL()));
}

TEST_F(WebDocumentTest, ManifestUseCredentials)
{
    loadURL(std::string(kDefaultOrigin) + kManifestDummyFilePath);

    WebDocument webDoc = topWebDocument();
    Document* document = topDocument();
    HTMLLinkElement* linkManifest = document->linkManifest();

    // No crossorigin attribute was set so credentials shouldn't be used.
    ASSERT_FALSE(linkManifest->fastHasAttribute(HTMLNames::crossoriginAttr));
    ASSERT_FALSE(webDoc.manifestUseCredentials());

    // Crossorigin set to a random string shouldn't trigger using credentials.
    linkManifest->setAttribute(HTMLNames::crossoriginAttr, "foobar");
    ASSERT_FALSE(webDoc.manifestUseCredentials());

    // Crossorigin set to 'anonymous' shouldn't trigger using credentials.
    linkManifest->setAttribute(HTMLNames::crossoriginAttr, "anonymous");
    ASSERT_FALSE(webDoc.manifestUseCredentials());

    // Crossorigin set to 'use-credentials' should trigger using credentials.
    linkManifest->setAttribute(HTMLNames::crossoriginAttr, "use-credentials");
    ASSERT_TRUE(webDoc.manifestUseCredentials());
}

namespace {

const char* baseURLOriginA = "http://example.test:0/";
const char* baseURLOriginSubA = "http://subdomain.example.test:0/";
const char* baseURLOriginB = "http://not-example.test:0/";
const char* emptyFile = "first_party/empty.html";
const char* nestedData = "first_party/nested-data.html";
const char* nestedOriginA = "first_party/nested-originA.html";
const char* nestedOriginSubA = "first_party/nested-originSubA.html";
const char* nestedOriginAInOriginA = "first_party/nested-originA-in-originA.html";
const char* nestedOriginAInOriginB = "first_party/nested-originA-in-originB.html";
const char* nestedOriginB = "first_party/nested-originB.html";
const char* nestedOriginBInOriginA = "first_party/nested-originB-in-originA.html";
const char* nestedOriginBInOriginB = "first_party/nested-originB-in-originB.html";
const char* nestedSrcDoc = "first_party/nested-srcdoc.html";

KURL toOriginA(const char* file)
{
    return toKURL(std::string(baseURLOriginA) + file);
}

KURL toOriginSubA(const char* file)
{
    return toKURL(std::string(baseURLOriginSubA) + file);
}

KURL toOriginB(const char* file)
{
    return toKURL(std::string(baseURLOriginB) + file);
}

} // anonymous namespace

class WebDocumentFirstPartyTest : public WebDocumentTest {
public:
    static void SetUpTestCase();

protected:
    void load(const char*);
    Document* nestedDocument() const;
    Document* nestedNestedDocument() const;
};

void WebDocumentFirstPartyTest::SetUpTestCase()
{
    URLTestHelpers::registerMockedURLLoad(toOriginA(emptyFile), WebString::fromUTF8(emptyFile));
    URLTestHelpers::registerMockedURLLoad(toOriginA(nestedData), WebString::fromUTF8(nestedData));
    URLTestHelpers::registerMockedURLLoad(toOriginA(nestedOriginA), WebString::fromUTF8(nestedOriginA));
    URLTestHelpers::registerMockedURLLoad(toOriginA(nestedOriginSubA), WebString::fromUTF8(nestedOriginSubA));
    URLTestHelpers::registerMockedURLLoad(toOriginA(nestedOriginAInOriginA), WebString::fromUTF8(nestedOriginAInOriginA));
    URLTestHelpers::registerMockedURLLoad(toOriginA(nestedOriginAInOriginB), WebString::fromUTF8(nestedOriginAInOriginB));
    URLTestHelpers::registerMockedURLLoad(toOriginA(nestedOriginB), WebString::fromUTF8(nestedOriginB));
    URLTestHelpers::registerMockedURLLoad(toOriginA(nestedOriginBInOriginA), WebString::fromUTF8(nestedOriginBInOriginA));
    URLTestHelpers::registerMockedURLLoad(toOriginA(nestedOriginBInOriginB), WebString::fromUTF8(nestedOriginBInOriginB));
    URLTestHelpers::registerMockedURLLoad(toOriginA(nestedSrcDoc), WebString::fromUTF8(nestedSrcDoc));

    URLTestHelpers::registerMockedURLLoad(toOriginSubA(emptyFile), WebString::fromUTF8(emptyFile));

    URLTestHelpers::registerMockedURLLoad(toOriginB(emptyFile), WebString::fromUTF8(emptyFile));
    URLTestHelpers::registerMockedURLLoad(toOriginB(nestedOriginA), WebString::fromUTF8(nestedOriginA));
    URLTestHelpers::registerMockedURLLoad(toOriginB(nestedOriginB), WebString::fromUTF8(nestedOriginB));
}

void WebDocumentFirstPartyTest::load(const char* file)
{
    m_webViewHelper.initializeAndLoad(std::string(baseURLOriginA) + file);
}

Document* WebDocumentFirstPartyTest::nestedDocument() const
{
    return toLocalFrame(m_webViewHelper.webViewImpl()->page()->mainFrame()->tree().firstChild())->document();
}

Document* WebDocumentFirstPartyTest::nestedNestedDocument() const
{
    return toLocalFrame(m_webViewHelper.webViewImpl()->page()->mainFrame()->tree().firstChild()->tree().firstChild())->document();
}

TEST_F(WebDocumentFirstPartyTest, Empty)
{
    load(emptyFile);

    ASSERT_EQ(toOriginA(emptyFile), topDocument()->firstPartyForCookies());
}

TEST_F(WebDocumentFirstPartyTest, NestedOriginA)
{
    load(nestedOriginA);

    ASSERT_EQ(toOriginA(nestedOriginA), topDocument()->firstPartyForCookies());
    ASSERT_EQ(toOriginA(nestedOriginA), nestedDocument()->firstPartyForCookies());
}

TEST_F(WebDocumentFirstPartyTest, NestedOriginSubA)
{
    load(nestedOriginSubA);

    ASSERT_EQ(toOriginA(nestedOriginSubA), topDocument()->firstPartyForCookies());
    ASSERT_EQ(toOriginA(nestedOriginSubA), nestedDocument()->firstPartyForCookies());
}

TEST_F(WebDocumentFirstPartyTest, NestedOriginAInOriginA)
{
    load(nestedOriginAInOriginA);

    ASSERT_EQ(toOriginA(nestedOriginAInOriginA), topDocument()->firstPartyForCookies());
    ASSERT_EQ(toOriginA(nestedOriginAInOriginA), nestedDocument()->firstPartyForCookies());
    ASSERT_EQ(toOriginA(nestedOriginAInOriginA), nestedNestedDocument()->firstPartyForCookies());
}

TEST_F(WebDocumentFirstPartyTest, NestedOriginAInOriginB)
{
    load(nestedOriginAInOriginB);

    ASSERT_EQ(toOriginA(nestedOriginAInOriginB), topDocument()->firstPartyForCookies());
    ASSERT_EQ(SecurityOrigin::urlWithUniqueSecurityOrigin(), nestedDocument()->firstPartyForCookies());
    ASSERT_EQ(SecurityOrigin::urlWithUniqueSecurityOrigin(), nestedNestedDocument()->firstPartyForCookies());
}

TEST_F(WebDocumentFirstPartyTest, NestedOriginB)
{
    load(nestedOriginB);

    ASSERT_EQ(toOriginA(nestedOriginB), topDocument()->firstPartyForCookies());
    ASSERT_EQ(SecurityOrigin::urlWithUniqueSecurityOrigin(), nestedDocument()->firstPartyForCookies());
}

TEST_F(WebDocumentFirstPartyTest, NestedOriginBInOriginA)
{
    load(nestedOriginBInOriginA);

    ASSERT_EQ(toOriginA(nestedOriginBInOriginA), topDocument()->firstPartyForCookies());
    ASSERT_EQ(toOriginA(nestedOriginBInOriginA), nestedDocument()->firstPartyForCookies());
    ASSERT_EQ(SecurityOrigin::urlWithUniqueSecurityOrigin(), nestedNestedDocument()->firstPartyForCookies());
}

TEST_F(WebDocumentFirstPartyTest, NestedOriginBInOriginB)
{
    load(nestedOriginBInOriginB);

    ASSERT_EQ(toOriginA(nestedOriginBInOriginB), topDocument()->firstPartyForCookies());
    ASSERT_EQ(SecurityOrigin::urlWithUniqueSecurityOrigin(), nestedDocument()->firstPartyForCookies());
    ASSERT_EQ(SecurityOrigin::urlWithUniqueSecurityOrigin(), nestedNestedDocument()->firstPartyForCookies());
}

TEST_F(WebDocumentFirstPartyTest, NestedSrcdoc)
{
    load(nestedSrcDoc);

    ASSERT_EQ(toOriginA(nestedSrcDoc), topDocument()->firstPartyForCookies());
    ASSERT_EQ(toOriginA(nestedSrcDoc), nestedDocument()->firstPartyForCookies());
}

TEST_F(WebDocumentFirstPartyTest, NestedData)
{
    load(nestedData);

    ASSERT_EQ(toOriginA(nestedData), topDocument()->firstPartyForCookies());
    ASSERT_EQ(SecurityOrigin::urlWithUniqueSecurityOrigin(), nestedDocument()->firstPartyForCookies());
}

} // namespace blink
