/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "platform/testing/UnitTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebString.h"
#include "public/platform/WebThread.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebURLLoader.h"
#include "public/platform/WebURLLoaderClient.h"
#include "public/platform/WebURLRequest.h"
#include "public/platform/WebURLResponse.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebFrame.h"
#include "public/web/WebURLLoaderOptions.h"
#include "public/web/WebView.h"
#include "web/tests/FrameTestHelpers.h"
#include "wtf/text/CString.h"
#include "wtf/text/WTFString.h"
#include <gtest/gtest.h>

using blink::URLTestHelpers::toKURL;
using blink::testing::runPendingTasks;

namespace blink {

class AssociatedURLLoaderTest : public ::testing::Test,
                                public WebURLLoaderClient {
public:
    AssociatedURLLoaderTest()
        :  m_willSendRequest(false)
        ,  m_didSendData(false)
        ,  m_didReceiveResponse(false)
        ,  m_didReceiveData(false)
        ,  m_didReceiveCachedMetadata(false)
        ,  m_didFinishLoading(false)
        ,  m_didFail(false)
    {
        // Reuse one of the test files from WebFrameTest.
        m_baseFilePath = Platform::current()->unitTestSupport()->webKitRootDir();
        m_baseFilePath.append("/Source/web/tests/data/");
        m_frameFilePath = m_baseFilePath;
        m_frameFilePath.append("iframes_test.html");
    }

    KURL RegisterMockedUrl(const std::string& urlRoot, const WTF::String& filename)
    {
        WebURLResponse response;
        response.initialize();
        response.setMIMEType("text/html");
        WTF::String localPath = m_baseFilePath;
        localPath.append(filename);
        KURL url = toKURL(urlRoot + filename.utf8().data());
        Platform::current()->unitTestSupport()->registerMockedURL(url, response, localPath);
        return url;
    }

    void SetUp() override
    {
        m_helper.initialize();

        std::string urlRoot = "http://www.test.com/";
        KURL url = RegisterMockedUrl(urlRoot, "iframes_test.html");
        const char* iframeSupportFiles[] = {
            "invisible_iframe.html",
            "visible_iframe.html",
            "zero_sized_iframe.html",
        };
        for (size_t i = 0; i < arraysize(iframeSupportFiles); ++i) {
            RegisterMockedUrl(urlRoot, iframeSupportFiles[i]);
        }

        FrameTestHelpers::loadFrame(mainFrame(), url.string().utf8().data());

        Platform::current()->unitTestSupport()->unregisterMockedURL(url);
    }

    void TearDown() override
    {
        Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
    }

    void serveRequests()
    {
        Platform::current()->unitTestSupport()->serveAsynchronousMockedRequests();
    }

    PassOwnPtr<WebURLLoader> createAssociatedURLLoader(const WebURLLoaderOptions options = WebURLLoaderOptions())
    {
        return adoptPtr(mainFrame()->createAssociatedURLLoader(options));
    }

    // WebURLLoaderClient implementation.
    void willSendRequest(WebURLLoader* loader, WebURLRequest& newRequest, const WebURLResponse& redirectResponse) override
    {
        m_willSendRequest = true;
        EXPECT_EQ(m_expectedLoader, loader);
        EXPECT_EQ(m_expectedNewRequest.url(), newRequest.url());
        // Check that CORS simple headers are transferred to the new request.
        EXPECT_EQ(m_expectedNewRequest.httpHeaderField("accept"), newRequest.httpHeaderField("accept"));
        EXPECT_EQ(m_expectedRedirectResponse.url(), redirectResponse.url());
        EXPECT_EQ(m_expectedRedirectResponse.httpStatusCode(), redirectResponse.httpStatusCode());
        EXPECT_EQ(m_expectedRedirectResponse.mimeType(), redirectResponse.mimeType());
    }

    void didSendData(WebURLLoader* loader, unsigned long long bytesSent, unsigned long long totalBytesToBeSent) override
    {
        m_didSendData = true;
        EXPECT_EQ(m_expectedLoader, loader);
    }

    void didReceiveResponse(WebURLLoader* loader, const WebURLResponse& response) override
    {
        m_didReceiveResponse = true;
        m_actualResponse = WebURLResponse(response);
        EXPECT_EQ(m_expectedLoader, loader);
        EXPECT_EQ(m_expectedResponse.url(), response.url());
        EXPECT_EQ(m_expectedResponse.httpStatusCode(), response.httpStatusCode());
    }

    void didDownloadData(WebURLLoader* loader, int dataLength, int encodedDataLength) override
    {
        m_didDownloadData = true;
        EXPECT_EQ(m_expectedLoader, loader);
    }

    void didReceiveData(WebURLLoader* loader, const char* data, int dataLength, int encodedDataLength) override
    {
        m_didReceiveData = true;
        EXPECT_EQ(m_expectedLoader, loader);
        EXPECT_TRUE(data);
        EXPECT_GT(dataLength, 0);
    }

    void didReceiveCachedMetadata(WebURLLoader* loader, const char* data, int dataLength) override
    {
        m_didReceiveCachedMetadata = true;
        EXPECT_EQ(m_expectedLoader, loader);
    }

    void didFinishLoading(WebURLLoader* loader, double finishTime, int64_t encodedDataLength) override
    {
        m_didFinishLoading = true;
        EXPECT_EQ(m_expectedLoader, loader);
    }

    void didFail(WebURLLoader* loader, const WebURLError& error) override
    {
        m_didFail = true;
        EXPECT_EQ(m_expectedLoader, loader);
    }

    void CheckMethodFails(const char* unsafeMethod)
    {
        WebURLRequest request;
        request.initialize();
        request.setURL(toKURL("http://www.test.com/success.html"));
        request.setHTTPMethod(WebString::fromUTF8(unsafeMethod));
        WebURLLoaderOptions options;
        options.untrustedHTTP = true;
        CheckFails(request, options);
    }

    void CheckHeaderFails(const char* headerField)
    {
        CheckHeaderFails(headerField, "foo");
    }

    void CheckHeaderFails(const char* headerField, const char* headerValue)
    {
        WebURLRequest request;
        request.initialize();
        request.setURL(toKURL("http://www.test.com/success.html"));
        if (equalIgnoringCase(WebString::fromUTF8(headerField), "referer"))
            request.setHTTPReferrer(WebString::fromUTF8(headerValue), WebReferrerPolicyDefault);
        else
            request.setHTTPHeaderField(WebString::fromUTF8(headerField), WebString::fromUTF8(headerValue));
        WebURLLoaderOptions options;
        options.untrustedHTTP = true;
        CheckFails(request, options);
    }

    void CheckFails(const WebURLRequest& request, WebURLLoaderOptions options = WebURLLoaderOptions())
    {
        m_expectedLoader = createAssociatedURLLoader(options);
        EXPECT_TRUE(m_expectedLoader);
        m_didFail = false;
        m_expectedLoader->loadAsynchronously(request, this);
        // Failure should not be reported synchronously.
        EXPECT_FALSE(m_didFail);
        // Allow the loader to return the error.
        runPendingTasks();
        EXPECT_TRUE(m_didFail);
        EXPECT_FALSE(m_didReceiveResponse);
    }

    bool CheckAccessControlHeaders(const char* headerName, bool exposed)
    {
        std::string id("http://www.other.com/CheckAccessControlExposeHeaders_");
        id.append(headerName);
        if (exposed)
            id.append("-Exposed");
        id.append(".html");

        KURL url = toKURL(id);
        WebURLRequest request;
        request.initialize();
        request.setURL(url);

        WebString headerNameString(WebString::fromUTF8(headerName));
        m_expectedResponse = WebURLResponse();
        m_expectedResponse.initialize();
        m_expectedResponse.setMIMEType("text/html");
        m_expectedResponse.setHTTPStatusCode(200);
        m_expectedResponse.addHTTPHeaderField("Access-Control-Allow-Origin", "*");
        if (exposed)
            m_expectedResponse.addHTTPHeaderField("access-control-expose-headers", headerNameString);
        m_expectedResponse.addHTTPHeaderField(headerNameString, "foo");
        Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedResponse, m_frameFilePath);

        WebURLLoaderOptions options;
        options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl;
        m_expectedLoader = createAssociatedURLLoader(options);
        EXPECT_TRUE(m_expectedLoader);
        m_expectedLoader->loadAsynchronously(request, this);
        serveRequests();
        EXPECT_TRUE(m_didReceiveResponse);
        EXPECT_TRUE(m_didReceiveData);
        EXPECT_TRUE(m_didFinishLoading);

        return !m_actualResponse.httpHeaderField(headerNameString).isEmpty();
    }

    WebFrame* mainFrame() const { return m_helper.webView()->mainFrame(); }

protected:
    String m_baseFilePath;
    String m_frameFilePath;
    FrameTestHelpers::WebViewHelper m_helper;

    OwnPtr<WebURLLoader> m_expectedLoader;
    WebURLResponse m_actualResponse;
    WebURLResponse m_expectedResponse;
    WebURLRequest m_expectedNewRequest;
    WebURLResponse m_expectedRedirectResponse;
    bool m_willSendRequest;
    bool m_didSendData;
    bool m_didReceiveResponse;
    bool m_didDownloadData;
    bool m_didReceiveData;
    bool m_didReceiveCachedMetadata;
    bool m_didFinishLoading;
    bool m_didFail;
};

// Test a successful same-origin URL load.
TEST_F(AssociatedURLLoaderTest, SameOriginSuccess)
{
    KURL url = toKURL("http://www.test.com/SameOriginSuccess.html");
    WebURLRequest request;
    request.initialize();
    request.setURL(url);

    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(200);
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedResponse, m_frameFilePath);

    m_expectedLoader = createAssociatedURLLoader();
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);
    serveRequests();
    EXPECT_TRUE(m_didReceiveResponse);
    EXPECT_TRUE(m_didReceiveData);
    EXPECT_TRUE(m_didFinishLoading);
}

// Test that the same-origin restriction is the default.
TEST_F(AssociatedURLLoaderTest, SameOriginRestriction)
{
    // This is cross-origin since the frame was loaded from www.test.com.
    KURL url = toKURL("http://www.other.com/SameOriginRestriction.html");
    WebURLRequest request;
    request.initialize();
    request.setURL(url);
    CheckFails(request);
}

// Test a successful cross-origin load.
TEST_F(AssociatedURLLoaderTest, CrossOriginSuccess)
{
    // This is cross-origin since the frame was loaded from www.test.com.
    KURL url = toKURL("http://www.other.com/CrossOriginSuccess.html");
    WebURLRequest request;
    request.initialize();
    request.setURL(url);

    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(200);
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedResponse, m_frameFilePath);

    WebURLLoaderOptions options;
    options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyAllow;
    m_expectedLoader = createAssociatedURLLoader(options);
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);
    serveRequests();
    EXPECT_TRUE(m_didReceiveResponse);
    EXPECT_TRUE(m_didReceiveData);
    EXPECT_TRUE(m_didFinishLoading);
}

// Test a successful cross-origin load using CORS.
TEST_F(AssociatedURLLoaderTest, CrossOriginWithAccessControlSuccess)
{
    // This is cross-origin since the frame was loaded from www.test.com.
    KURL url = toKURL("http://www.other.com/CrossOriginWithAccessControlSuccess.html");
    WebURLRequest request;
    request.initialize();
    request.setURL(url);

    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(200);
    m_expectedResponse.addHTTPHeaderField("access-control-allow-origin", "*");
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedResponse, m_frameFilePath);

    WebURLLoaderOptions options;
    options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl;
    m_expectedLoader = createAssociatedURLLoader(options);
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);
    serveRequests();
    EXPECT_TRUE(m_didReceiveResponse);
    EXPECT_TRUE(m_didReceiveData);
    EXPECT_TRUE(m_didFinishLoading);
}

// Test an unsuccessful cross-origin load using CORS.
TEST_F(AssociatedURLLoaderTest, CrossOriginWithAccessControlFailure)
{
    // This is cross-origin since the frame was loaded from www.test.com.
    KURL url = toKURL("http://www.other.com/CrossOriginWithAccessControlFailure.html");
    WebURLRequest request;
    request.initialize();
    request.setURL(url);

    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(200);
    m_expectedResponse.addHTTPHeaderField("access-control-allow-origin", "*");
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedResponse, m_frameFilePath);

    WebURLLoaderOptions options;
    // Send credentials. This will cause the CORS checks to fail, because credentials can't be
    // sent to a server which returns the header "access-control-allow-origin" with "*" as its value.
    options.allowCredentials = true;
    options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl;
    m_expectedLoader = createAssociatedURLLoader(options);
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);

    // Failure should not be reported synchronously.
    EXPECT_FALSE(m_didFail);
    // The loader needs to receive the response, before doing the CORS check.
    serveRequests();
    EXPECT_TRUE(m_didFail);
    EXPECT_FALSE(m_didReceiveResponse);
}

// Test an unsuccessful cross-origin load using CORS.
TEST_F(AssociatedURLLoaderTest, CrossOriginWithAccessControlFailureBadStatusCode)
{
    // This is cross-origin since the frame was loaded from www.test.com.
    KURL url = toKURL("http://www.other.com/CrossOriginWithAccessControlFailure.html");
    WebURLRequest request;
    request.initialize();
    request.setURL(url);

    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(0);
    m_expectedResponse.addHTTPHeaderField("access-control-allow-origin", "*");
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedResponse, m_frameFilePath);

    WebURLLoaderOptions options;
    options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl;
    m_expectedLoader = createAssociatedURLLoader(options);
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);

    // Failure should not be reported synchronously.
    EXPECT_FALSE(m_didFail);
    // The loader needs to receive the response, before doing the CORS check.
    serveRequests();
    EXPECT_TRUE(m_didFail);
    EXPECT_FALSE(m_didReceiveResponse);
}

// Test a same-origin URL redirect and load.
TEST_F(AssociatedURLLoaderTest, RedirectSuccess)
{
    KURL url = toKURL("http://www.test.com/RedirectSuccess.html");
    char redirect[] = "http://www.test.com/RedirectSuccess2.html";  // Same-origin
    KURL redirectURL = toKURL(redirect);

    WebURLRequest request;
    request.initialize();
    request.setURL(url);

    m_expectedRedirectResponse = WebURLResponse();
    m_expectedRedirectResponse.initialize();
    m_expectedRedirectResponse.setMIMEType("text/html");
    m_expectedRedirectResponse.setHTTPStatusCode(301);
    m_expectedRedirectResponse.setHTTPHeaderField("Location", redirect);
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedRedirectResponse, m_frameFilePath);

    m_expectedNewRequest = WebURLRequest();
    m_expectedNewRequest.initialize();
    m_expectedNewRequest.setURL(redirectURL);

    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(200);
    Platform::current()->unitTestSupport()->registerMockedURL(redirectURL, m_expectedResponse, m_frameFilePath);

    m_expectedLoader = createAssociatedURLLoader();
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);
    serveRequests();
    EXPECT_TRUE(m_willSendRequest);
    EXPECT_TRUE(m_didReceiveResponse);
    EXPECT_TRUE(m_didReceiveData);
    EXPECT_TRUE(m_didFinishLoading);
}

// Test a cross-origin URL redirect without Access Control set.
TEST_F(AssociatedURLLoaderTest, RedirectCrossOriginFailure)
{
    KURL url = toKURL("http://www.test.com/RedirectCrossOriginFailure.html");
    char redirect[] = "http://www.other.com/RedirectCrossOriginFailure.html";  // Cross-origin
    KURL redirectURL;

    WebURLRequest request;
    request.initialize();
    request.setURL(url);

    m_expectedRedirectResponse = WebURLResponse();
    m_expectedRedirectResponse.initialize();
    m_expectedRedirectResponse.setMIMEType("text/html");
    m_expectedRedirectResponse.setHTTPStatusCode(301);
    m_expectedRedirectResponse.setHTTPHeaderField("Location", redirect);
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedRedirectResponse, m_frameFilePath);

    m_expectedNewRequest = WebURLRequest();
    m_expectedNewRequest.initialize();
    m_expectedNewRequest.setURL(redirectURL);

    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(200);
    Platform::current()->unitTestSupport()->registerMockedURL(redirectURL, m_expectedResponse, m_frameFilePath);

    m_expectedLoader = createAssociatedURLLoader();
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);

    serveRequests();
    EXPECT_FALSE(m_willSendRequest);
    EXPECT_FALSE(m_didReceiveResponse);
    EXPECT_FALSE(m_didReceiveData);
    EXPECT_FALSE(m_didFinishLoading);
}

// Test that a cross origin redirect response without CORS headers fails.
TEST_F(AssociatedURLLoaderTest, RedirectCrossOriginWithAccessControlFailure)
{
    KURL url = toKURL("http://www.test.com/RedirectCrossOriginWithAccessControlFailure.html");
    char redirect[] = "http://www.other.com/RedirectCrossOriginWithAccessControlFailure.html";  // Cross-origin
    KURL redirectURL = toKURL(redirect);

    WebURLRequest request;
    request.initialize();
    request.setURL(url);

    m_expectedRedirectResponse = WebURLResponse();
    m_expectedRedirectResponse.initialize();
    m_expectedRedirectResponse.setMIMEType("text/html");
    m_expectedRedirectResponse.setHTTPStatusCode(301);
    m_expectedRedirectResponse.setHTTPHeaderField("Location", redirect);
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedRedirectResponse, m_frameFilePath);

    m_expectedNewRequest = WebURLRequest();
    m_expectedNewRequest.initialize();
    m_expectedNewRequest.setURL(redirectURL);

    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(200);
    Platform::current()->unitTestSupport()->registerMockedURL(redirectURL, m_expectedResponse, m_frameFilePath);

    WebURLLoaderOptions options;
    options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl;
    m_expectedLoader = createAssociatedURLLoader(options);
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);

    serveRequests();
    // We should get a notification about access control check failure.
    EXPECT_FALSE(m_willSendRequest);
    EXPECT_FALSE(m_didReceiveResponse);
    EXPECT_TRUE(m_didReceiveData);
    EXPECT_TRUE(m_didFail);
}

// Test that a cross origin redirect response with CORS headers that allow the requesting origin succeeds.
TEST_F(AssociatedURLLoaderTest, RedirectCrossOriginWithAccessControlSuccess)
{
    KURL url = toKURL("http://www.test.com/RedirectCrossOriginWithAccessControlSuccess.html");
    char redirect[] = "http://www.other.com/RedirectCrossOriginWithAccessControlSuccess.html";  // Cross-origin
    KURL redirectURL = toKURL(redirect);

    WebURLRequest request;
    request.initialize();
    request.setURL(url);
    // Add a CORS simple header.
    request.setHTTPHeaderField("accept", "application/json");

    // Create a redirect response that allows the redirect to pass the access control checks.
    m_expectedRedirectResponse = WebURLResponse();
    m_expectedRedirectResponse.initialize();
    m_expectedRedirectResponse.setMIMEType("text/html");
    m_expectedRedirectResponse.setHTTPStatusCode(301);
    m_expectedRedirectResponse.setHTTPHeaderField("Location", redirect);
    m_expectedRedirectResponse.addHTTPHeaderField("access-control-allow-origin", "*");
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedRedirectResponse, m_frameFilePath);

    m_expectedNewRequest = WebURLRequest();
    m_expectedNewRequest.initialize();
    m_expectedNewRequest.setURL(redirectURL);
    m_expectedNewRequest.setHTTPHeaderField("accept", "application/json");

    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(200);
    m_expectedResponse.addHTTPHeaderField("access-control-allow-origin", "*");
    Platform::current()->unitTestSupport()->registerMockedURL(redirectURL, m_expectedResponse, m_frameFilePath);

    WebURLLoaderOptions options;
    options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl;
    m_expectedLoader = createAssociatedURLLoader(options);
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);
    serveRequests();
    // We should not receive a notification for the redirect.
    EXPECT_FALSE(m_willSendRequest);
    EXPECT_TRUE(m_didReceiveResponse);
    EXPECT_TRUE(m_didReceiveData);
    EXPECT_TRUE(m_didFinishLoading);
}

// Test that untrusted loads can't use a forbidden method.
TEST_F(AssociatedURLLoaderTest, UntrustedCheckMethods)
{
    // Check non-token method fails.
    CheckMethodFails("GET()");
    CheckMethodFails("POST\x0d\x0ax-csrf-token:\x20test1234");

    // Forbidden methods should fail regardless of casing.
    CheckMethodFails("CoNneCt");
    CheckMethodFails("TrAcK");
    CheckMethodFails("TrAcE");
}

// This test is flaky on Windows and Android. See <http://crbug.com/471645>.
#if OS(WIN) || OS(ANDROID)
#define MAYBE_UntrustedCheckHeaders DISABLED_UntrustedCheckHeaders
#else
#define MAYBE_UntrustedCheckHeaders UntrustedCheckHeaders
#endif

// Test that untrusted loads can't use a forbidden header field.
TEST_F(AssociatedURLLoaderTest, MAYBE_UntrustedCheckHeaders)
{
    // Check non-token header fails.
    CheckHeaderFails("foo()");

    // Check forbidden headers fail.
    CheckHeaderFails("accept-charset");
    CheckHeaderFails("accept-encoding");
    CheckHeaderFails("connection");
    CheckHeaderFails("content-length");
    CheckHeaderFails("cookie");
    CheckHeaderFails("cookie2");
    CheckHeaderFails("date");
    CheckHeaderFails("dnt");
    CheckHeaderFails("expect");
    CheckHeaderFails("host");
    CheckHeaderFails("keep-alive");
    CheckHeaderFails("origin");
    CheckHeaderFails("referer");
    CheckHeaderFails("te");
    CheckHeaderFails("trailer");
    CheckHeaderFails("transfer-encoding");
    CheckHeaderFails("upgrade");
    CheckHeaderFails("user-agent");
    CheckHeaderFails("via");

    CheckHeaderFails("proxy-");
    CheckHeaderFails("proxy-foo");
    CheckHeaderFails("sec-");
    CheckHeaderFails("sec-foo");

    // Check that validation is case-insensitive.
    CheckHeaderFails("AcCePt-ChArSeT");
    CheckHeaderFails("ProXy-FoO");

    // Check invalid header values.
    CheckHeaderFails("foo", "bar\x0d\x0ax-csrf-token:\x20test1234");
}

// Test that the loader filters response headers according to the CORS standard.
TEST_F(AssociatedURLLoaderTest, CrossOriginHeaderWhitelisting)
{
    // Test that whitelisted headers are returned without exposing them.
    EXPECT_TRUE(CheckAccessControlHeaders("cache-control", false));
    EXPECT_TRUE(CheckAccessControlHeaders("content-language", false));
    EXPECT_TRUE(CheckAccessControlHeaders("content-type", false));
    EXPECT_TRUE(CheckAccessControlHeaders("expires", false));
    EXPECT_TRUE(CheckAccessControlHeaders("last-modified", false));
    EXPECT_TRUE(CheckAccessControlHeaders("pragma", false));

    // Test that non-whitelisted headers aren't returned.
    EXPECT_FALSE(CheckAccessControlHeaders("non-whitelisted", false));

    // Test that Set-Cookie headers aren't returned.
    EXPECT_FALSE(CheckAccessControlHeaders("Set-Cookie", false));
    EXPECT_FALSE(CheckAccessControlHeaders("Set-Cookie2", false));

    // Test that exposed headers that aren't whitelisted are returned.
    EXPECT_TRUE(CheckAccessControlHeaders("non-whitelisted", true));

    // Test that Set-Cookie headers aren't returned, even if exposed.
    EXPECT_FALSE(CheckAccessControlHeaders("Set-Cookie", true));
}

// Test that the loader can allow non-whitelisted response headers for trusted CORS loads.
TEST_F(AssociatedURLLoaderTest, CrossOriginHeaderAllowResponseHeaders)
{
    WebURLRequest request;
    request.initialize();
    KURL url = toKURL("http://www.other.com/CrossOriginHeaderAllowResponseHeaders.html");
    request.setURL(url);

    WebString headerNameString(WebString::fromUTF8("non-whitelisted"));
    m_expectedResponse = WebURLResponse();
    m_expectedResponse.initialize();
    m_expectedResponse.setMIMEType("text/html");
    m_expectedResponse.setHTTPStatusCode(200);
    m_expectedResponse.addHTTPHeaderField("Access-Control-Allow-Origin", "*");
    m_expectedResponse.addHTTPHeaderField(headerNameString, "foo");
    Platform::current()->unitTestSupport()->registerMockedURL(url, m_expectedResponse, m_frameFilePath);

    WebURLLoaderOptions options;
    options.exposeAllResponseHeaders = true; // This turns off response whitelisting.
    options.crossOriginRequestPolicy = WebURLLoaderOptions::CrossOriginRequestPolicyUseAccessControl;
    m_expectedLoader = createAssociatedURLLoader(options);
    EXPECT_TRUE(m_expectedLoader);
    m_expectedLoader->loadAsynchronously(request, this);
    serveRequests();
    EXPECT_TRUE(m_didReceiveResponse);
    EXPECT_TRUE(m_didReceiveData);
    EXPECT_TRUE(m_didFinishLoading);

    EXPECT_FALSE(m_actualResponse.httpHeaderField(headerNameString).isEmpty());
}

} // namespace blink
