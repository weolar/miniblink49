// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/Request.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/Document.h"
#include "core/frame/Frame.h"
#include "core/testing/DummyPageHolder.h"
#include "public/platform/WebServiceWorkerRequest.h"
#include "public/platform/WebURLRequest.h"
#include "wtf/HashMap.h"
#include "wtf/text/WTFString.h"
#include <gtest/gtest.h>

namespace blink {
namespace {

class ServiceWorkerRequestTest : public ::testing::Test {
public:
    ServiceWorkerRequestTest()
        : m_page(DummyPageHolder::create(IntSize(1, 1))) { }

    ScriptState* scriptState() { return ScriptState::forMainWorld(m_page->document().frame()); }
    ExecutionContext* executionContext() { return scriptState()->executionContext(); }

private:
    OwnPtr<DummyPageHolder> m_page;
};

TEST_F(ServiceWorkerRequestTest, FromString)
{
    TrackExceptionState exceptionState;

    KURL url(ParsedURLString, "http://www.example.com/");
    Request* request = Request::create(scriptState(), url, exceptionState);
    ASSERT_FALSE(exceptionState.hadException());
    ASSERT(request);
    EXPECT_EQ(url, request->url());
}

TEST_F(ServiceWorkerRequestTest, FromRequest)
{
    TrackExceptionState exceptionState;

    KURL url(ParsedURLString, "http://www.example.com/");
    Request* request1 = Request::create(scriptState(), url, exceptionState);
    ASSERT(request1);

    Request* request2 = Request::create(scriptState(), request1, exceptionState);
    ASSERT_FALSE(exceptionState.hadException());
    ASSERT(request2);
    EXPECT_EQ(url, request2->url());
}

TEST_F(ServiceWorkerRequestTest, FromAndToWebRequest)
{
    WebServiceWorkerRequest webRequest;

    const KURL url(ParsedURLString, "http://www.example.com/");
    const String method = "GET";
    struct {
        const char* key;
        const char* value;
    } headers[] = { {"X-Foo", "bar"}, {"X-Quux", "foop"}, {0, 0} };
    const String referrer = "http://www.referrer.com/";
    const WebReferrerPolicy referrerPolicy = WebReferrerPolicyAlways;
    const WebURLRequest::RequestContext context = WebURLRequest::RequestContextAudio;

    webRequest.setURL(url);
    webRequest.setMethod(method);
    webRequest.setRequestContext(context);
    for (int i = 0; headers[i].key; ++i)
        webRequest.setHeader(WebString::fromUTF8(headers[i].key), WebString::fromUTF8(headers[i].value));
    webRequest.setReferrer(referrer, referrerPolicy);

    Request* request = Request::create(executionContext(), webRequest);
    ASSERT(request);
    EXPECT_EQ(url, request->url());
    EXPECT_EQ(method, request->method());
    EXPECT_EQ("audio", request->context());
    EXPECT_EQ(referrer, request->referrer());

    Headers* requestHeaders = request->headers();

    WTF::HashMap<String, String> headersMap;
    for (int i = 0; headers[i].key; ++i)
        headersMap.add(headers[i].key, headers[i].value);
    EXPECT_EQ(headersMap.size(), requestHeaders->headerList()->size());
    for (WTF::HashMap<String, String>::iterator iter = headersMap.begin(); iter != headersMap.end(); ++iter) {
        TrackExceptionState exceptionState;
        EXPECT_EQ(iter->value, requestHeaders->get(iter->key, exceptionState));
        EXPECT_FALSE(exceptionState.hadException());
    }

    WebServiceWorkerRequest secondWebRequest;
    request->populateWebServiceWorkerRequest(secondWebRequest);
    EXPECT_EQ(url, KURL(secondWebRequest.url()));
    EXPECT_EQ(method, String(secondWebRequest.method()));
    EXPECT_EQ(context, secondWebRequest.requestContext());
    EXPECT_EQ(referrer, KURL(secondWebRequest.referrerUrl()));
    EXPECT_EQ(referrerPolicy, secondWebRequest.referrerPolicy());
    EXPECT_EQ(webRequest.headers(), secondWebRequest.headers());
}

TEST_F(ServiceWorkerRequestTest, ToWebRequestStripsURLFragment)
{
    TrackExceptionState exceptionState;
    String urlWithoutFragment = "http://www.example.com/";
    String url = urlWithoutFragment + "#fragment";
    Request* request = Request::create(scriptState(), url, exceptionState);
    ASSERT(request);

    WebServiceWorkerRequest webRequest;
    request->populateWebServiceWorkerRequest(webRequest);
    EXPECT_EQ(urlWithoutFragment, KURL(webRequest.url()));
}

} // namespace
} // namespace blink
