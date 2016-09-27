// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/Response.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/Frame.h"
#include "core/testing/DummyPageHolder.h"
#include "modules/fetch/BodyStreamBuffer.h"
#include "modules/fetch/DataConsumerHandleTestUtil.h"
#include "modules/fetch/DataConsumerHandleUtil.h"
#include "modules/fetch/FetchResponseData.h"
#include "platform/blob/BlobData.h"
#include "platform/testing/UnitTestHelpers.h"
#include "public/platform/WebServiceWorkerResponse.h"
#include <gtest/gtest.h>

namespace blink {
namespace {

PassOwnPtr<WebServiceWorkerResponse> createTestWebServiceWorkerResponse()
{
    const KURL url(ParsedURLString, "http://www.webresponse.com/");
    const unsigned short status = 200;
    const String statusText = "the best status text";
    struct {
        const char* key;
        const char* value;
    } headers[] = { { "cache-control", "no-cache" }, { "set-cookie", "foop" }, { "foo", "bar" }, { 0, 0 } };

    OwnPtr<WebServiceWorkerResponse> webResponse = adoptPtr(new WebServiceWorkerResponse());
    webResponse->setURL(url);
    webResponse->setStatus(status);
    webResponse->setStatusText(statusText);
    webResponse->setResponseType(WebServiceWorkerResponseTypeDefault);
    for (int i = 0; headers[i].key; ++i)
        webResponse->setHeader(WebString::fromUTF8(headers[i].key), WebString::fromUTF8(headers[i].value));
    return webResponse.release();
}

class ServiceWorkerResponseTest : public ::testing::Test {
public:
    ServiceWorkerResponseTest()
        : m_page(DummyPageHolder::create(IntSize(1, 1))) { }

    ScriptState* scriptState() { return ScriptState::forMainWorld(m_page->document().frame()); }
    ExecutionContext* executionContext() { return scriptState()->executionContext(); }

private:
    OwnPtr<DummyPageHolder> m_page;
};


TEST_F(ServiceWorkerResponseTest, FromFetchResponseData)
{
    const KURL url(ParsedURLString, "http://www.response.com");

    FetchResponseData* fetchResponseData = FetchResponseData::create();
    fetchResponseData->setURL(url);

    Response* response = Response::create(executionContext(), fetchResponseData);
    ASSERT(response);
    EXPECT_EQ(url, response->url());
}

TEST_F(ServiceWorkerResponseTest, FromWebServiceWorkerResponse)
{
    OwnPtr<WebServiceWorkerResponse> webResponse = createTestWebServiceWorkerResponse();
    Response* response = Response::create(executionContext(), *webResponse);
    ASSERT(response);
    EXPECT_EQ(webResponse->url(), response->url());
    EXPECT_EQ(webResponse->status(), response->status());
    EXPECT_STREQ(webResponse->statusText().utf8().c_str(), response->statusText().utf8().data());

    Headers* responseHeaders = response->headers();

    WebVector<WebString> keys = webResponse->getHeaderKeys();
    EXPECT_EQ(keys.size(), responseHeaders->headerList()->size());
    for (size_t i = 0, max = keys.size(); i < max; ++i) {
        WebString key = keys[i];
        TrackExceptionState exceptionState;
        EXPECT_STREQ(webResponse->getHeader(key).utf8().c_str(), responseHeaders->get(key, exceptionState).utf8().data());
        EXPECT_FALSE(exceptionState.hadException());
    }
}

TEST_F(ServiceWorkerResponseTest, FromWebServiceWorkerResponseDefault)
{
    OwnPtr<WebServiceWorkerResponse> webResponse = createTestWebServiceWorkerResponse();
    webResponse->setResponseType(WebServiceWorkerResponseTypeDefault);
    Response* response = Response::create(executionContext(), *webResponse);

    Headers* responseHeaders = response->headers();
    TrackExceptionState exceptionState;
    EXPECT_STREQ("foop", responseHeaders->get("set-cookie", exceptionState).utf8().data());
    EXPECT_STREQ("bar", responseHeaders->get("foo", exceptionState).utf8().data());
    EXPECT_STREQ("no-cache", responseHeaders->get("cache-control", exceptionState).utf8().data());
    EXPECT_FALSE(exceptionState.hadException());
}

TEST_F(ServiceWorkerResponseTest, FromWebServiceWorkerResponseBasic)
{
    OwnPtr<WebServiceWorkerResponse> webResponse = createTestWebServiceWorkerResponse();
    webResponse->setResponseType(WebServiceWorkerResponseTypeBasic);
    Response* response = Response::create(executionContext(), *webResponse);

    Headers* responseHeaders = response->headers();
    TrackExceptionState exceptionState;
    EXPECT_STREQ("", responseHeaders->get("set-cookie", exceptionState).utf8().data());
    EXPECT_STREQ("bar", responseHeaders->get("foo", exceptionState).utf8().data());
    EXPECT_STREQ("no-cache", responseHeaders->get("cache-control", exceptionState).utf8().data());
    EXPECT_FALSE(exceptionState.hadException());
}

TEST_F(ServiceWorkerResponseTest, FromWebServiceWorkerResponseCORS)
{
    OwnPtr<WebServiceWorkerResponse> webResponse = createTestWebServiceWorkerResponse();
    webResponse->setResponseType(WebServiceWorkerResponseTypeCORS);
    Response* response = Response::create(executionContext(), *webResponse);

    Headers* responseHeaders = response->headers();
    TrackExceptionState exceptionState;
    EXPECT_STREQ("", responseHeaders->get("set-cookie", exceptionState).utf8().data());
    EXPECT_STREQ("", responseHeaders->get("foo", exceptionState).utf8().data());
    EXPECT_STREQ("no-cache", responseHeaders->get("cache-control", exceptionState).utf8().data());
    EXPECT_FALSE(exceptionState.hadException());
}

TEST_F(ServiceWorkerResponseTest, FromWebServiceWorkerResponseOpaque)
{
    OwnPtr<WebServiceWorkerResponse> webResponse = createTestWebServiceWorkerResponse();
    webResponse->setResponseType(WebServiceWorkerResponseTypeOpaque);
    Response* response = Response::create(executionContext(), *webResponse);

    Headers* responseHeaders = response->headers();
    TrackExceptionState exceptionState;
    EXPECT_STREQ("", responseHeaders->get("set-cookie", exceptionState).utf8().data());
    EXPECT_STREQ("", responseHeaders->get("foo", exceptionState).utf8().data());
    EXPECT_STREQ("", responseHeaders->get("cache-control", exceptionState).utf8().data());
    EXPECT_FALSE(exceptionState.hadException());
}

void loadInternalBufferAsString(Response* response, FetchDataLoader::Client* client)
{
    FetchDataLoader* fetchDataLoader = FetchDataLoader::createLoaderAsString();
    OwnPtr<DrainingBodyStreamBuffer> buffer = response->createInternalDrainingStream();
    buffer->startLoading(response->executionContext(), fetchDataLoader, client);
}

void checkResponseStream(Response* response, bool checkResponseBodyStreamBuffer)
{
    void* buffer = response->internalBufferForTest();
    if (checkResponseBodyStreamBuffer) {
        EXPECT_EQ(response->bufferForTest(), buffer);
    } else {
        EXPECT_FALSE(response->bufferForTest());
    }

    TrackExceptionState exceptionState;
    Response* clonedResponse = response->clone(exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    EXPECT_TRUE(response->internalBufferForTest());
    EXPECT_TRUE(clonedResponse->internalBufferForTest());
    EXPECT_NE(response->internalBufferForTest(), buffer);
    EXPECT_NE(clonedResponse->internalBufferForTest(), buffer);
    EXPECT_NE(response->internalBufferForTest(), clonedResponse->internalBufferForTest());
    if (checkResponseBodyStreamBuffer) {
        EXPECT_EQ(response->bufferForTest(), response->internalBufferForTest());
        EXPECT_EQ(clonedResponse->bufferForTest(), clonedResponse->internalBufferForTest());
    } else {
        EXPECT_FALSE(response->bufferForTest());
        EXPECT_FALSE(clonedResponse->bufferForTest());
    }
    DataConsumerHandleTestUtil::MockFetchDataLoaderClient* client1 = new DataConsumerHandleTestUtil::MockFetchDataLoaderClient();
    DataConsumerHandleTestUtil::MockFetchDataLoaderClient* client2 = new DataConsumerHandleTestUtil::MockFetchDataLoaderClient();
    EXPECT_CALL(*client1, didFetchDataLoadedString(String("Hello, world")));
    EXPECT_CALL(*client2, didFetchDataLoadedString(String("Hello, world")));

    loadInternalBufferAsString(response, client1);
    loadInternalBufferAsString(clonedResponse, client2);
    blink::testing::runPendingTasks();
}

BodyStreamBuffer* createHelloWorldBuffer()
{
    using Command = DataConsumerHandleTestUtil::Command;
    OwnPtr<DataConsumerHandleTestUtil::ReplayingHandle> src(DataConsumerHandleTestUtil::ReplayingHandle::create());
    src->add(Command(Command::Data, "Hello, "));
    src->add(Command(Command::Data, "world"));
    src->add(Command(Command::Done));
    return BodyStreamBuffer::create(createFetchDataConsumerHandleFromWebHandle(src.release()));
}

TEST_F(ServiceWorkerResponseTest, BodyStreamBufferCloneDefault)
{
    BodyStreamBuffer* buffer = createHelloWorldBuffer();
    FetchResponseData* fetchResponseData = FetchResponseData::createWithBuffer(buffer);
    fetchResponseData->setURL(KURL(ParsedURLString, "http://www.response.com"));
    Response* response = Response::create(executionContext(), fetchResponseData);
    EXPECT_EQ(response->internalBufferForTest(), buffer);
    checkResponseStream(response, true);
}

TEST_F(ServiceWorkerResponseTest, BodyStreamBufferCloneBasic)
{
    BodyStreamBuffer* buffer = createHelloWorldBuffer();
    FetchResponseData* fetchResponseData = FetchResponseData::createWithBuffer(buffer);
    fetchResponseData->setURL(KURL(ParsedURLString, "http://www.response.com"));
    fetchResponseData = fetchResponseData->createBasicFilteredResponse();
    Response* response = Response::create(executionContext(), fetchResponseData);
    EXPECT_EQ(response->internalBufferForTest(), buffer);
    checkResponseStream(response, true);
}

TEST_F(ServiceWorkerResponseTest, BodyStreamBufferCloneCORS)
{
    BodyStreamBuffer* buffer = createHelloWorldBuffer();
    FetchResponseData* fetchResponseData = FetchResponseData::createWithBuffer(buffer);
    fetchResponseData->setURL(KURL(ParsedURLString, "http://www.response.com"));
    fetchResponseData = fetchResponseData->createCORSFilteredResponse();
    Response* response = Response::create(executionContext(), fetchResponseData);
    EXPECT_EQ(response->internalBufferForTest(), buffer);
    checkResponseStream(response, true);
}

TEST_F(ServiceWorkerResponseTest, BodyStreamBufferCloneOpaque)
{
    BodyStreamBuffer* buffer = createHelloWorldBuffer();
    FetchResponseData* fetchResponseData = FetchResponseData::createWithBuffer(buffer);
    fetchResponseData->setURL(KURL(ParsedURLString, "http://www.response.com"));
    fetchResponseData = fetchResponseData->createOpaqueFilteredResponse();
    Response* response = Response::create(executionContext(), fetchResponseData);
    EXPECT_EQ(response->internalBufferForTest(), buffer);
    checkResponseStream(response, false);
}

TEST_F(ServiceWorkerResponseTest, BodyStreamBufferCloneError)
{
    BodyStreamBuffer* buffer = BodyStreamBuffer::create(createFetchDataConsumerHandleFromWebHandle(createUnexpectedErrorDataConsumerHandle()));
    FetchResponseData* fetchResponseData = FetchResponseData::createWithBuffer(buffer);
    fetchResponseData->setURL(KURL(ParsedURLString, "http://www.response.com"));
    Response* response = Response::create(executionContext(), fetchResponseData);
    TrackExceptionState exceptionState;
    Response* clonedResponse = response->clone(exceptionState);
    EXPECT_FALSE(exceptionState.hadException());

    DataConsumerHandleTestUtil::MockFetchDataLoaderClient* client1 = new DataConsumerHandleTestUtil::MockFetchDataLoaderClient();
    DataConsumerHandleTestUtil::MockFetchDataLoaderClient* client2 = new DataConsumerHandleTestUtil::MockFetchDataLoaderClient();
    EXPECT_CALL(*client1, didFetchDataLoadFailed());
    EXPECT_CALL(*client2, didFetchDataLoadFailed());

    loadInternalBufferAsString(response, client1);
    loadInternalBufferAsString(clonedResponse, client2);
    blink::testing::runPendingTasks();
}

} // namespace
} // namespace blink
