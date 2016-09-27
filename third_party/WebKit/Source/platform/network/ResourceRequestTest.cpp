// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "ResourceRequest.h"

#include "platform/network/FormData.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/Referrer.h"
#include "public/platform/WebURLRequest.h"
#include "wtf/text/AtomicString.h"
#include <gtest/gtest.h>

namespace blink {

TEST(ResourceRequestTest, CrossThreadResourceRequestData)
{
    ResourceRequest original;
    original.setURL(KURL(ParsedURLString, "http://www.example.com/test.htm"));
    original.setCachePolicy(UseProtocolCachePolicy);
    original.setTimeoutInterval(10);
    original.setFirstPartyForCookies(KURL(ParsedURLString, "http://www.example.com/first_party.htm"));
    original.setRequestorOrigin(SecurityOrigin::create(KURL(ParsedURLString, "http://www.example.com/first_party.htm")));
    original.setHTTPMethod(AtomicString("GET", AtomicString::ConstructFromLiteral));
    original.setHTTPHeaderField(AtomicString("Foo"), AtomicString("Bar"));
    original.setHTTPHeaderField(AtomicString("Piyo"), AtomicString("Fuga"));
    original.setPriority(ResourceLoadPriorityLow, 20);

    RefPtr<FormData> originalBody(FormData::create("Test Body"));
    original.setHTTPBody(originalBody);
    original.setAllowStoredCredentials(false);
    original.setReportUploadProgress(false);
    original.setHasUserGesture(false);
    original.setDownloadToFile(false);
    original.setSkipServiceWorker(false);
    original.setFetchRequestMode(WebURLRequest::FetchRequestModeCORS);
    original.setFetchCredentialsMode(WebURLRequest::FetchCredentialsModeSameOrigin);
    original.setRequestorID(30);
    original.setRequestorProcessID(40);
    original.setAppCacheHostID(50);
    original.setRequestContext(WebURLRequest::RequestContextAudio);
    original.setFrameType(WebURLRequest::FrameTypeNested);
    original.setHTTPReferrer(Referrer("http://www.example.com/referrer.htm", ReferrerPolicyDefault));

    EXPECT_STREQ("http://www.example.com/test.htm", original.url().string().utf8().data());
    EXPECT_EQ(UseProtocolCachePolicy, original.cachePolicy());
    EXPECT_EQ(10, original.timeoutInterval());
    EXPECT_STREQ("http://www.example.com/first_party.htm", original.firstPartyForCookies().string().utf8().data());
    EXPECT_STREQ("www.example.com", original.requestorOrigin()->host().utf8().data());
    EXPECT_STREQ("GET", original.httpMethod().utf8().data());
    EXPECT_STREQ("Bar", original.httpHeaderFields().get("Foo").utf8().data());
    EXPECT_STREQ("Fuga", original.httpHeaderFields().get("Piyo").utf8().data());
    EXPECT_EQ(ResourceLoadPriorityLow, original.priority());
    EXPECT_STREQ("Test Body", original.httpBody()->flattenToString().utf8().data());
    EXPECT_FALSE(original.allowStoredCredentials());
    EXPECT_FALSE(original.reportUploadProgress());
    EXPECT_FALSE(original.hasUserGesture());
    EXPECT_FALSE(original.downloadToFile());
    EXPECT_FALSE(original.skipServiceWorker());
    EXPECT_EQ(WebURLRequest::FetchRequestModeCORS, original.fetchRequestMode());
    EXPECT_EQ(WebURLRequest::FetchCredentialsModeSameOrigin, original.fetchCredentialsMode());
    EXPECT_EQ(30, original.requestorID());
    EXPECT_EQ(40, original.requestorProcessID());
    EXPECT_EQ(50, original.appCacheHostID());
    EXPECT_EQ(WebURLRequest::RequestContextAudio, original.requestContext());
    EXPECT_EQ(WebURLRequest::FrameTypeNested, original.frameType());
    EXPECT_STREQ("http://www.example.com/referrer.htm", original.httpReferrer().utf8().data());
    EXPECT_EQ(ReferrerPolicyDefault, original.referrerPolicy());

    OwnPtr<CrossThreadResourceRequestData> data1(original.copyData());
    OwnPtr<ResourceRequest> copy1(ResourceRequest::adopt(data1.release()));

    EXPECT_STREQ("http://www.example.com/test.htm", copy1->url().string().utf8().data());
    EXPECT_EQ(UseProtocolCachePolicy, copy1->cachePolicy());
    EXPECT_EQ(10, copy1->timeoutInterval());
    EXPECT_STREQ("http://www.example.com/first_party.htm", copy1->firstPartyForCookies().string().utf8().data());
    EXPECT_STREQ("www.example.com", copy1->requestorOrigin()->host().utf8().data());
    EXPECT_STREQ("GET", copy1->httpMethod().utf8().data());
    EXPECT_STREQ("Bar", copy1->httpHeaderFields().get("Foo").utf8().data());
    EXPECT_EQ(ResourceLoadPriorityLow, copy1->priority());
    EXPECT_STREQ("Test Body", copy1->httpBody()->flattenToString().utf8().data());
    EXPECT_FALSE(copy1->allowStoredCredentials());
    EXPECT_FALSE(copy1->reportUploadProgress());
    EXPECT_FALSE(copy1->hasUserGesture());
    EXPECT_FALSE(copy1->downloadToFile());
    EXPECT_FALSE(copy1->skipServiceWorker());
    EXPECT_EQ(WebURLRequest::FetchRequestModeCORS, copy1->fetchRequestMode());
    EXPECT_EQ(WebURLRequest::FetchCredentialsModeSameOrigin, copy1->fetchCredentialsMode());
    EXPECT_EQ(30, copy1->requestorID());
    EXPECT_EQ(40, copy1->requestorProcessID());
    EXPECT_EQ(50, copy1->appCacheHostID());
    EXPECT_EQ(WebURLRequest::RequestContextAudio, copy1->requestContext());
    EXPECT_EQ(WebURLRequest::FrameTypeNested, copy1->frameType());
    EXPECT_STREQ("http://www.example.com/referrer.htm", copy1->httpReferrer().utf8().data());
    EXPECT_EQ(ReferrerPolicyDefault, copy1->referrerPolicy());

    copy1->setAllowStoredCredentials(true);
    copy1->setReportUploadProgress(true);
    copy1->setHasUserGesture(true);
    copy1->setDownloadToFile(true);
    copy1->setSkipServiceWorker(true);
    copy1->setFetchRequestMode(WebURLRequest::FetchRequestModeNoCORS);
    copy1->setFetchCredentialsMode(WebURLRequest::FetchCredentialsModeInclude);

    OwnPtr<CrossThreadResourceRequestData> data2(copy1->copyData());
    OwnPtr<ResourceRequest> copy2(ResourceRequest::adopt(data2.release()));
    EXPECT_TRUE(copy2->allowStoredCredentials());
    EXPECT_TRUE(copy2->reportUploadProgress());
    EXPECT_TRUE(copy2->hasUserGesture());
    EXPECT_TRUE(copy2->downloadToFile());
    EXPECT_TRUE(copy2->skipServiceWorker());
    EXPECT_EQ(WebURLRequest::FetchRequestModeNoCORS, copy1->fetchRequestMode());
    EXPECT_EQ(WebURLRequest::FetchCredentialsModeInclude, copy1->fetchCredentialsMode());
}

TEST(ResourceRequestTest, SetHasUserGesture)
{
    ResourceRequest original;
    EXPECT_FALSE(original.hasUserGesture());
    original.setHasUserGesture(true);
    EXPECT_TRUE(original.hasUserGesture());
    original.setHasUserGesture(false);
    EXPECT_TRUE(original.hasUserGesture());
}

} // namespace blink
