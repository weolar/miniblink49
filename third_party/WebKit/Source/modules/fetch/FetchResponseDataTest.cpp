// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/FetchResponseData.h"

#include "core/dom/DOMArrayBuffer.h"
#include "modules/fetch/FetchHeaderList.h"
#include "platform/blob/BlobData.h"
#include "public/platform/WebServiceWorkerResponse.h"

#include <gtest/gtest.h>

namespace blink {

class FetchResponseDataTest : public ::testing::Test {
public:
    FetchResponseData* createInternalResponse()
    {
        FetchResponseData* internalResponse = FetchResponseData::create();
        internalResponse->setStatus(200);
        internalResponse->setURL(KURL(ParsedURLString, "http://www.example.com"));
        internalResponse->headerList()->append("set-cookie", "foo");
        internalResponse->headerList()->append("bar", "bar");
        internalResponse->headerList()->append("cache-control", "no-cache");
        return internalResponse;
    }

    void CheckHeaders(const WebServiceWorkerResponse& webResponse)
    {
        EXPECT_STREQ("foo", webResponse.getHeader("set-cookie").utf8().c_str());
        EXPECT_STREQ("bar", webResponse.getHeader("bar").utf8().c_str());
        EXPECT_STREQ("no-cache", webResponse.getHeader("cache-control").utf8().c_str());
    }
};

TEST_F(FetchResponseDataTest, ToWebServiceWorkerDefaultType)
{
    WebServiceWorkerResponse webResponse;
    FetchResponseData* internalResponse = createInternalResponse();

    internalResponse->populateWebServiceWorkerResponse(webResponse);
    EXPECT_EQ(WebServiceWorkerResponseTypeDefault, webResponse.responseType());
    CheckHeaders(webResponse);
}

TEST_F(FetchResponseDataTest, ToWebServiceWorkerBasicType)
{
    WebServiceWorkerResponse webResponse;
    FetchResponseData* internalResponse = createInternalResponse();
    FetchResponseData* basicResponseData = internalResponse->createBasicFilteredResponse();

    basicResponseData->populateWebServiceWorkerResponse(webResponse);
    EXPECT_EQ(WebServiceWorkerResponseTypeBasic, webResponse.responseType());
    CheckHeaders(webResponse);
}

TEST_F(FetchResponseDataTest, ToWebServiceWorkerCORSType)
{
    WebServiceWorkerResponse webResponse;
    FetchResponseData* internalResponse = createInternalResponse();
    FetchResponseData* corsResponseData = internalResponse->createCORSFilteredResponse();

    corsResponseData->populateWebServiceWorkerResponse(webResponse);
    EXPECT_EQ(WebServiceWorkerResponseTypeCORS, webResponse.responseType());
    CheckHeaders(webResponse);
}

TEST_F(FetchResponseDataTest, ToWebServiceWorkerOpaqueType)
{
    WebServiceWorkerResponse webResponse;
    FetchResponseData* internalResponse = createInternalResponse();
    FetchResponseData* opaqueResponseData = internalResponse->createOpaqueFilteredResponse();

    opaqueResponseData->populateWebServiceWorkerResponse(webResponse);
    EXPECT_EQ(WebServiceWorkerResponseTypeOpaque, webResponse.responseType());
    CheckHeaders(webResponse);
}

} // namespace WebCore
