// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/web/WebEmbeddedWorker.h"

#include "platform/testing/URLTestHelpers.h"
#include "platform/testing/UnitTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebURLResponse.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebEmbeddedWorkerStartData.h"
#include "public/web/WebServiceWorkerContextClient.h"
#include "public/web/WebSettings.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace blink {
namespace {

class MockServiceWorkerContextClient
    : public WebServiceWorkerContextClient {
public:
    MockServiceWorkerContextClient() { }
    ~MockServiceWorkerContextClient() override { }
    MOCK_METHOD0(workerReadyForInspection, void());
    MOCK_METHOD0(workerContextFailedToStart, void());
    MOCK_METHOD1(createServiceWorkerNetworkProvider, WebServiceWorkerNetworkProvider*(WebDataSource*));
};

class WebEmbeddedWorkerImplFailureTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        m_mockClient = new MockServiceWorkerContextClient();
        m_worker = adoptPtr(WebEmbeddedWorker::create(m_mockClient, nullptr));

        WebURL invalidScriptURL = URLTestHelpers::toKURL("https://www.example.com/sw.js");
        WebURLResponse errorResponse;
        errorResponse.initialize();
        errorResponse.setMIMEType("text/html");
        errorResponse.setHTTPStatusCode(404);
        WebURLError error;
        error.reason = 1010;
        error.domain = "WebEmbeddedWorkerImplTest";
        Platform::current()->unitTestSupport()->registerMockedErrorURL(invalidScriptURL, errorResponse, error);

        m_startData.scriptURL = invalidScriptURL;
        m_startData.userAgent = WebString("dummy user agent");
        m_startData.pauseAfterDownloadMode = WebEmbeddedWorkerStartData::DontPauseAfterDownload;
        m_startData.waitForDebuggerMode = WebEmbeddedWorkerStartData::DontWaitForDebugger;
        m_startData.v8CacheOptions = WebSettings::V8CacheOptionsDefault;
    }

    void TearDown() override
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

    WebEmbeddedWorkerStartData m_startData;
    MockServiceWorkerContextClient* m_mockClient;
    OwnPtr<WebEmbeddedWorker> m_worker;
};

} // namespace

TEST_F(WebEmbeddedWorkerImplFailureTest, TerminateSoonAfterStart)
{
    EXPECT_CALL(*m_mockClient, workerReadyForInspection()).Times(1);
    m_worker->startWorkerContext(m_startData);
    ::testing::Mock::VerifyAndClearExpectations(m_mockClient);

    EXPECT_CALL(*m_mockClient, workerContextFailedToStart()).Times(1);
    m_worker->terminateWorkerContext();
    ::testing::Mock::VerifyAndClearExpectations(m_mockClient);
}

TEST_F(WebEmbeddedWorkerImplFailureTest, TerminateWhileLoadingScript)
{
    EXPECT_CALL(*m_mockClient, workerReadyForInspection()).Times(1);
    m_worker->startWorkerContext(m_startData);
    ::testing::Mock::VerifyAndClearExpectations(m_mockClient);

    EXPECT_CALL(*m_mockClient, createServiceWorkerNetworkProvider(::testing::_)).WillOnce(::testing::Return(nullptr));
    testing::runPendingTasks();
    ::testing::Mock::VerifyAndClearExpectations(m_mockClient);

    EXPECT_CALL(*m_mockClient, workerContextFailedToStart()).Times(1);
    m_worker->terminateWorkerContext();
    ::testing::Mock::VerifyAndClearExpectations(m_mockClient);
}

} // namespace blink
