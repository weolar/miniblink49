// Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <algorithm>

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "include/base/cef_bind.h"
#include "include/cef_origin_whitelist.h"
#include "include/cef_callback.h"
#include "include/cef_scheme.h"
#include "include/wrapper/cef_closure_task.h"
#include "tests/unittests/test_handler.h"
#include "tests/unittests/test_suite.h"

namespace {

class TestResults {
 public:
  TestResults()
    : status_code(0),
      sub_status_code(0),
      delay(0) {
  }

  void reset() {
    url.clear();
    html.clear();
    status_code = 0;
    redirect_url.clear();
    sub_url.clear();
    sub_html.clear();
    sub_status_code = 0;
    sub_allow_origin.clear();
    exit_url.clear();
    accept_language.clear();
    delay = 0;
    got_request.reset();
    got_read.reset();
    got_output.reset();
    got_redirect.reset();
    got_error.reset();
    got_sub_request.reset();
    got_sub_read.reset();
    got_sub_success.reset();
  }

  std::string url;
  std::string html;
  int status_code;

  // Used for testing redirects
  std::string redirect_url;

  // Used for testing XHR requests
  std::string sub_url;
  std::string sub_html;
  int sub_status_code;
  std::string sub_allow_origin;
  std::string sub_redirect_url;
  std::string exit_url;

  // Used for testing per-browser Accept-Language.
  std::string accept_language;

  // Delay for returning scheme handler results.
  int delay;

  TrackCallback
      got_request,
      got_read,
      got_output,
      got_redirect,
      got_error,
      got_sub_redirect,
      got_sub_request,
      got_sub_read,
      got_sub_success;
};

// Current scheme handler object. Used when destroying the test from
// ClientSchemeHandler::ProcessRequest().
class TestSchemeHandler;
TestSchemeHandler* g_current_handler = NULL;

class TestSchemeHandler : public TestHandler {
 public:
  explicit TestSchemeHandler(TestResults* tr)
    : test_results_(tr) {
    g_current_handler = this;
  }

  void PopulateBrowserSettings(CefBrowserSettings* settings) override {
    if (!test_results_->accept_language.empty()) {
      CefString(&settings->accept_language_list) =
          test_results_->accept_language;
    }
  }

  void RunTest() override {
    CreateBrowser(test_results_->url);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  // Necessary to make the method public in order to destroy the test from
  // ClientSchemeHandler::ProcessRequest().
  void DestroyTest() {
    TestHandler::DestroyTest();
  }

  cef_return_value_t OnBeforeResourceLoad(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      CefRefPtr<CefRequestCallback> callback) override {
    std::string newUrl = request->GetURL();
    if (!test_results_->exit_url.empty() &&
        newUrl.find(test_results_->exit_url) != std::string::npos) {
      // XHR tests use an exit URL to destroy the test.
      if (newUrl.find("SUCCESS") != std::string::npos)
        test_results_->got_sub_success.yes();
      DestroyTest();
      return RV_CANCEL;
    }

    if (!test_results_->sub_redirect_url.empty() &&
        newUrl == test_results_->sub_redirect_url) {
      test_results_->got_sub_redirect.yes();
      // Redirect to the sub URL.
      request->SetURL(test_results_->sub_url);
    } else if (newUrl == test_results_->redirect_url) {
      test_results_->got_redirect.yes();

      // No read should have occurred for the redirect.
      EXPECT_TRUE(test_results_->got_request);
      EXPECT_FALSE(test_results_->got_read);

      // Now loading the redirect URL.
      test_results_->url = test_results_->redirect_url;
      test_results_->redirect_url.clear();
    }

    return RV_CONTINUE;
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    std::string url = frame->GetURL();
    if (url == test_results_->url || test_results_->status_code != 200) {
      test_results_->got_output.yes();

      // Test that the status code is correct.
      EXPECT_EQ(httpStatusCode, test_results_->status_code);

      if (test_results_->sub_url.empty())
        DestroyTest();
    }
  }

  void OnLoadError(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   ErrorCode errorCode,
                   const CefString& errorText,
                   const CefString& failedUrl) override {
    test_results_->got_error.yes();
    DestroyTest();
  }

 protected:
  TestResults* test_results_;

  IMPLEMENT_REFCOUNTING(TestSchemeHandler);
};

class ClientSchemeHandler : public CefResourceHandler {
 public:
  explicit ClientSchemeHandler(TestResults* tr)
    : test_results_(tr),
      offset_(0),
      is_sub_(false),
      has_delayed_(false) {
  }

  bool ProcessRequest(CefRefPtr<CefRequest> request,
                      CefRefPtr<CefCallback> callback) override {
    EXPECT_TRUE(CefCurrentlyOn(TID_IO));

    bool handled = false;

    std::string url = request->GetURL();
    is_sub_ = (!test_results_->sub_url.empty() &&
               test_results_->sub_url == url);

    if (is_sub_) {
      test_results_->got_sub_request.yes();

      if (!test_results_->sub_html.empty())
        handled = true;
    } else {
      EXPECT_EQ(url, test_results_->url);

      test_results_->got_request.yes();

      if (!test_results_->html.empty())
        handled = true;
    }

    std::string accept_language;
    CefRequest::HeaderMap headerMap;
    CefRequest::HeaderMap::iterator headerIter;
    request->GetHeaderMap(headerMap);
    headerIter = headerMap.find("Accept-Language");
    if (headerIter != headerMap.end())
      accept_language = headerIter->second;
    EXPECT_TRUE(!accept_language.empty());

    if (!test_results_->accept_language.empty()) {
      // Value from CefBrowserSettings.accept_language set in
      // PopulateBrowserSettings().
      EXPECT_STREQ(test_results_->accept_language.data(),
                   accept_language.data());
    } else {
      // Value from CefSettings.accept_language set in
      // CefTestSuite::GetSettings().
      EXPECT_STREQ(CEF_SETTINGS_ACCEPT_LANGUAGE,
                   accept_language.data());
    }

    if (handled) {
      if (test_results_->delay > 0) {
        // Continue after the delay.
        CefPostDelayedTask(TID_IO,
            base::Bind(&CefCallback::Continue, callback.get()),
            test_results_->delay);
      } else {
        // Continue immediately.
        callback->Continue();
      }
      return true;
    }

    // Response was canceled.
    if (g_current_handler)
      g_current_handler->DestroyTest();
    return false;
  }

  void GetResponseHeaders(CefRefPtr<CefResponse> response,
                          int64& response_length,
                          CefString& redirectUrl) override {
    if (is_sub_) {
      response->SetStatus(test_results_->sub_status_code);

      if (!test_results_->sub_allow_origin.empty()) {
        // Set the Access-Control-Allow-Origin header to allow cross-domain
        // scripting.
        CefResponse::HeaderMap headers;
        headers.insert(std::make_pair("Access-Control-Allow-Origin",
                                      test_results_->sub_allow_origin));
        response->SetHeaderMap(headers);
      }

      if (!test_results_->sub_html.empty()) {
        response->SetMimeType("text/html");
        response_length = test_results_->sub_html.size();
      }
    } else if (!test_results_->redirect_url.empty()) {
      redirectUrl = test_results_->redirect_url;
    } else {
      response->SetStatus(test_results_->status_code);

      if (!test_results_->html.empty()) {
        response->SetMimeType("text/html");
        response_length = test_results_->html.size();
      }
    }
  }

  void Cancel() override {
    EXPECT_TRUE(CefCurrentlyOn(TID_IO));
  }

  bool ReadResponse(void* data_out,
                    int bytes_to_read,
                    int& bytes_read,
                    CefRefPtr<CefCallback> callback) override {
    EXPECT_TRUE(CefCurrentlyOn(TID_IO));

    if (test_results_->delay > 0) {
      if (!has_delayed_) {
        // Continue after a delay.
        CefPostDelayedTask(TID_IO,
            base::Bind(&ClientSchemeHandler::ContinueAfterDelay,
                       this, callback),
            test_results_->delay);
         bytes_read = 0;
         return true;
      }

      has_delayed_ = false;
    }

    std::string* data;

    if (is_sub_) {
      test_results_->got_sub_read.yes();
      data = &test_results_->sub_html;
    } else {
      test_results_->got_read.yes();
      data = &test_results_->html;
    }

    bool has_data = false;
    bytes_read = 0;

    size_t size = data->size();
    if (offset_ < size) {
      int transfer_size =
          std::min(bytes_to_read, static_cast<int>(size - offset_));
      memcpy(data_out, data->c_str() + offset_, transfer_size);
      offset_ += transfer_size;

      bytes_read = transfer_size;
      has_data = true;
    }

    return has_data;
  }

 private:
  void ContinueAfterDelay(CefRefPtr<CefCallback> callback) {
    has_delayed_ = true;
    callback->Continue();
  }

  TestResults* test_results_;
  size_t offset_;
  bool is_sub_;
  bool has_delayed_;

  IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
};

class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory {
 public:
  explicit ClientSchemeHandlerFactory(TestResults* tr)
    : test_results_(tr) {
  }

  CefRefPtr<CefResourceHandler> Create(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      const CefString& scheme_name,
      CefRefPtr<CefRequest> request) override {
    EXPECT_TRUE(CefCurrentlyOn(TID_IO));
    return new ClientSchemeHandler(test_results_);
  }

  TestResults* test_results_;

  IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
};

// Global test results object.
TestResults g_TestResults;

// If |domain| is empty the scheme will be registered as non-standard.
void RegisterTestScheme(const std::string& scheme, const std::string& domain) {
  g_TestResults.reset();

  EXPECT_TRUE(CefRegisterSchemeHandlerFactory(scheme, domain,
      new ClientSchemeHandlerFactory(&g_TestResults)));
  WaitForIOThread();
}

void ClearTestSchemes() {
  EXPECT_TRUE(CefClearSchemeHandlerFactories());
  WaitForIOThread();
}

struct XHRTestSettings {
  XHRTestSettings() 
      : synchronous(true) {}

  std::string url;
  std::string sub_url;
  std::string sub_allow_origin;
  std::string sub_redirect_url;
  bool synchronous;
};

void SetUpXHR(const XHRTestSettings& settings) {
  g_TestResults.sub_url = settings.sub_url;
  g_TestResults.sub_html = "SUCCESS";
  g_TestResults.sub_status_code = 200;
  g_TestResults.sub_allow_origin = settings.sub_allow_origin;
  g_TestResults.sub_redirect_url = settings.sub_redirect_url;

  std::string request_url;
  if (!settings.sub_redirect_url.empty())
    request_url = settings.sub_redirect_url;
  else
    request_url = settings.sub_url;

  g_TestResults.url = settings.url;
  std::stringstream ss;
  ss << "<html><head>"
        "<script language=\"JavaScript\">"
        "function onResult(val) {"
        "  document.location = \"http://tests/exit?result=\"+val;"
        "}"
        "function execXMLHttpRequest() {";
  if (settings.synchronous) {
    ss << "var result = 'FAILURE';"
          "try {"
          "  xhr = new XMLHttpRequest();"
          "  xhr.open(\"GET\", \"" << request_url.c_str() << "\", false);"
          "  xhr.send();"
          "  result = xhr.responseText;"
          "} catch(e) {}"
          "onResult(result)";
  } else {
    ss << "xhr = new XMLHttpRequest();"
          "xhr.open(\"GET\", \"" << request_url.c_str() << "\", true);"
          "xhr.onload = function(e) {"
          "  if (xhr.readyState === 4) {"
          "    if (xhr.status === 200) {"
          "      onResult(xhr.responseText);"
          "    } else {"
          "      console.log('XMLHttpRequest failed with status ' + xhr.status);"
          "      onResult('FAILURE');"
          "    }"
          "  }"
          "};"
          "xhr.onerror = function(e) {"
          "  console.log('XMLHttpRequest failed with error ' + e);"
          "  onResult('FAILURE');"
          "};"
          "xhr.send()";
  }
  ss << "}"
        "</script>"
        "</head><body onload=\"execXMLHttpRequest();\">"
        "Running execXMLHttpRequest..."
        "</body></html>";
  g_TestResults.html = ss.str();
  g_TestResults.status_code = 200;

  g_TestResults.exit_url = "http://tests/exit";
}

void SetUpXSS(const std::string& url, const std::string& sub_url,
              const std::string& domain = std::string()) {
  // 1. Load |url| which contains an iframe.
  // 2. The iframe loads |xss_url|.
  // 3. |xss_url| tries to call a JS function in |url|.
  // 4. |url| tries to call a JS function in |xss_url|.

  std::stringstream ss;
  std::string domain_line;
  if (!domain.empty())
    domain_line = "document.domain = '" + domain + "';";

  g_TestResults.sub_url = sub_url;
  ss << "<html><head>"
        "<script language=\"JavaScript\">" << domain_line <<
        "function getResult() {"
        "  return 'SUCCESS';"
        "}"
        "function execXSSRequest() {"
        "  var result = 'FAILURE';"
        "  try {"
        "    result = parent.getResult();"
        "  } catch(e) {}"
        "  document.location = \"http://tests/exit?result=\"+result;"
        "}"
        "</script>"
        "</head><body onload=\"execXSSRequest();\">"
        "Running execXSSRequest..."
        "</body></html>";
  g_TestResults.sub_html = ss.str();
  g_TestResults.sub_status_code = 200;

  g_TestResults.url = url;
  ss.str("");
  ss << "<html><head>"
        "<script language=\"JavaScript\">" << domain_line << ""
        "function getResult() {"
        "  try {"
        "    return document.getElementById('s').contentWindow.getResult();"
        "  } catch(e) {}"
        "  return 'FAILURE';"
        "}"
        "</script>"
        "</head><body>"
        "<iframe src=\"" << sub_url.c_str() << "\" id=\"s\">"
        "</body></html>";
  g_TestResults.html = ss.str();
  g_TestResults.status_code = 200;

  g_TestResults.exit_url = "http://tests/exit";
}

}  // namespace

// Test that scheme registration/unregistration works as expected.
TEST(SchemeHandlerTest, Registration) {
  RegisterTestScheme("customstd", "test");
  g_TestResults.url = "customstd://test/run.html";
  g_TestResults.html =
      "<html><head></head><body><h1>Success!</h1></body></html>";
  g_TestResults.status_code = 200;

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);

  // Unregister the handler.
  EXPECT_TRUE(CefRegisterSchemeHandlerFactory("customstd", "test", NULL));
  WaitForIOThread();

  g_TestResults.got_request.reset();
  g_TestResults.got_read.reset();
  g_TestResults.got_output.reset();
  handler->ExecuteTest();

  EXPECT_TRUE(g_TestResults.got_error);
  EXPECT_FALSE(g_TestResults.got_request);
  EXPECT_FALSE(g_TestResults.got_read);
  EXPECT_FALSE(g_TestResults.got_output);

  // Re-register the handler.
  EXPECT_TRUE(CefRegisterSchemeHandlerFactory("customstd", "test",
      new ClientSchemeHandlerFactory(&g_TestResults)));
  WaitForIOThread();

  g_TestResults.got_error.reset();
  handler->ExecuteTest();

  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that a custom standard scheme can return normal results.
TEST(SchemeHandlerTest, CustomStandardNormalResponse) {
  RegisterTestScheme("customstd", "test");
  g_TestResults.url = "customstd://test/run.html";
  g_TestResults.html =
      "<html><head></head><body><h1>Success!</h1></body></html>";
  g_TestResults.status_code = 200;

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that a custom standard scheme can return normal results with delayed
// responses.
TEST(SchemeHandlerTest, CustomStandardNormalResponseDelayed) {
  RegisterTestScheme("customstd", "test");
  g_TestResults.url = "customstd://test/run.html";
  g_TestResults.html =
      "<html><head></head><body><h1>Success!</h1></body></html>";
  g_TestResults.status_code = 200;
  g_TestResults.delay = 100;

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that a custom nonstandard scheme can return normal results.
TEST(SchemeHandlerTest, CustomNonStandardNormalResponse) {
  RegisterTestScheme("customnonstd", std::string());
  g_TestResults.url = "customnonstd:some%20value";
  g_TestResults.html =
      "<html><head></head><body><h1>Success!</h1></body></html>";
  g_TestResults.status_code = 200;

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that a custom standard scheme can return an error code.
TEST(SchemeHandlerTest, CustomStandardErrorResponse) {
  RegisterTestScheme("customstd", "test");
  g_TestResults.url = "customstd://test/run.html";
  g_TestResults.html =
      "<html><head></head><body><h1>404</h1></body></html>";
  g_TestResults.status_code = 404;

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that a custom nonstandard scheme can return an error code.
TEST(SchemeHandlerTest, CustomNonStandardErrorResponse) {
  RegisterTestScheme("customnonstd", std::string());
  g_TestResults.url = "customnonstd:some%20value";
  g_TestResults.html =
      "<html><head></head><body><h1>404</h1></body></html>";
  g_TestResults.status_code = 404;

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that custom standard scheme handling fails when the scheme name is
// incorrect.
TEST(SchemeHandlerTest, CustomStandardNameNotHandled) {
  RegisterTestScheme("customstd", "test");
  g_TestResults.url = "customstd2://test/run.html";

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_FALSE(g_TestResults.got_request);
  EXPECT_FALSE(g_TestResults.got_read);
  EXPECT_FALSE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that custom nonstandard scheme handling fails when the scheme name is
// incorrect.
TEST(SchemeHandlerTest, CustomNonStandardNameNotHandled) {
  RegisterTestScheme("customnonstd", std::string());
  g_TestResults.url = "customnonstd2:some%20value";

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_FALSE(g_TestResults.got_request);
  EXPECT_FALSE(g_TestResults.got_read);
  EXPECT_FALSE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that custom standard scheme handling fails when the domain name is
// incorrect.
TEST(SchemeHandlerTest, CustomStandardDomainNotHandled) {
  RegisterTestScheme("customstd", "test");
  g_TestResults.url = "customstd://noexist/run.html";

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_FALSE(g_TestResults.got_request);
  EXPECT_FALSE(g_TestResults.got_read);
  EXPECT_FALSE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that a custom standard scheme can return no response.
TEST(SchemeHandlerTest, CustomStandardNoResponse) {
  RegisterTestScheme("customstd", "test");
  g_TestResults.url = "customstd://test/run.html";

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_FALSE(g_TestResults.got_read);
  EXPECT_FALSE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that a custom nonstandard scheme can return no response.
TEST(SchemeHandlerTest, CustomNonStandardNoResponse) {
  RegisterTestScheme("customnonstd", std::string());
  g_TestResults.url = "customnonstd:some%20value";

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_FALSE(g_TestResults.got_read);
  EXPECT_FALSE(g_TestResults.got_output);

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate redirects.
TEST(SchemeHandlerTest, CustomStandardRedirect) {
  RegisterTestScheme("customstd", "test");
  g_TestResults.url = "customstd://test/run.html";
  g_TestResults.redirect_url = "customstd://test/redirect.html";
  g_TestResults.html =
      "<html><head></head><body><h1>Redirected</h1></body></html>";

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_redirect);

  ClearTestSchemes();
}

// Test that a custom nonstandard scheme can generate redirects.
TEST(SchemeHandlerTest, CustomNonStandardRedirect) {
  RegisterTestScheme("customnonstd", std::string());
  g_TestResults.url = "customnonstd:some%20value";
  g_TestResults.redirect_url = "customnonstd:some%20other%20value";
  g_TestResults.html =
      "<html><head></head><body><h1>Redirected</h1></body></html>";

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_redirect);

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate same origin XHR requests.
TEST(SchemeHandlerTest, CustomStandardXHRSameOriginSync) {
  RegisterTestScheme("customstd", "test");

  XHRTestSettings settings;
  settings.url = "customstd://test/run.html";
  settings.sub_url = "customstd://test/xhr.html";
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate same origin XHR requests.
TEST(SchemeHandlerTest, CustomStandardXHRSameOriginAsync) {
  RegisterTestScheme("customstd", "test");

  XHRTestSettings settings;
  settings.url = "customstd://test/run.html";
  settings.sub_url = "customstd://test/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom nonstandard scheme can generate same origin XHR requests.
TEST(SchemeHandlerTest, CustomNonStandardXHRSameOriginSync) {
  RegisterTestScheme("customnonstd", std::string());

  XHRTestSettings settings;
  settings.url = "customnonstd:some%20value";
  settings.sub_url = "customnonstd:xhr%20value";
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom nonstandard scheme can generate same origin XHR requests.
TEST(SchemeHandlerTest, CustomNonStandardXHRSameOriginAsync) {
  RegisterTestScheme("customnonstd", std::string());
  
  XHRTestSettings settings;
  settings.url = "customnonstd:some%20value";
  settings.sub_url = "customnonstd:xhr%20value";
  settings.synchronous = false;
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate same origin XSS requests.
TEST(SchemeHandlerTest, CustomStandardXSSSameOrigin) {
  RegisterTestScheme("customstd", "test");
  SetUpXSS("customstd://test/run.html",
           "customstd://test/iframe.html");

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom nonstandard scheme can generate same origin XSS requests.
TEST(SchemeHandlerTest, CustomNonStandardXSSSameOrigin) {
  RegisterTestScheme("customnonstd", std::string());
  SetUpXSS("customnonstd:some%20value",
           "customnonstd:xhr%20value");

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme cannot generate cross-domain XHR requests
// by default. Behavior should be the same as with HTTP.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginSync) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");

  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_FALSE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme cannot generate cross-domain XHR requests
// by default. Behavior should be the same as with HTTP.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginAsync) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");

  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_FALSE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme cannot generate cross-domain XSS requests
// by default.
TEST(SchemeHandlerTest, CustomStandardXSSDifferentOrigin) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");
  SetUpXSS("customstd://test1/run.html",
           "customstd://test2/iframe.html");

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_FALSE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that an HTTP scheme cannot generate cross-domain XHR requests by
// default.
TEST(SchemeHandlerTest, HttpXHRDifferentOriginSync) {
  RegisterTestScheme("http", "test1");
  RegisterTestScheme("http", "test2");

  XHRTestSettings settings;
  settings.url = "http://test1/run.html";
  settings.sub_url = "http://test2/xhr.html";
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_FALSE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that an HTTP scheme cannot generate cross-domain XHR requests by
// default.
TEST(SchemeHandlerTest, HttpXHRDifferentOriginAsync) {
  RegisterTestScheme("http", "test1");
  RegisterTestScheme("http", "test2");

  XHRTestSettings settings;
  settings.url = "http://test1/run.html";
  settings.sub_url = "http://test2/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_FALSE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that an HTTP scheme cannot generate cross-domain XSS requests by
// default.
TEST(SchemeHandlerTest, HttpXSSDifferentOrigin) {
  RegisterTestScheme("http", "test1");
  RegisterTestScheme("http", "test2");
  SetUpXSS("http://test1/run.html",
           "http://test2/xss.html");

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_FALSE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate cross-domain XHR requests
// when setting the Access-Control-Allow-Origin header. Should behave the same
// as HTTP.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginWithHeaderSync) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");
  
  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.sub_allow_origin = "customstd://test1";
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate cross-domain XHR requests
// when setting the Access-Control-Allow-Origin header. Should behave the same
// as HTTP.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginWithHeaderAsync) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");
  
  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.sub_allow_origin = "customstd://test1";
  settings.synchronous = false;
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate cross-domain XHR requests
// when using the cross-origin whitelist.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginWithWhitelistSync1) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");
  
  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      "test2", false));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Same as above but origin whitelist matches any domain.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginWithWhitelistSync2) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");
  
  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      CefString(), true));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Same as above but origin whitelist matches sub-domains.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginWithWhitelistSync3) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "a.test2.foo");
  
  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://a.test2.foo/xhr.html";
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      "test2.foo", true));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate cross-domain XHR requests
// when using the cross-origin whitelist.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginWithWhitelistAsync1) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");
  
  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      "test2", false));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Same as above but origin whitelist matches any domain.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginWithWhitelistAsync2) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");
  
  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      CefString(), true));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Same as above but origin whitelist matches sub-domains.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginWithWhitelistAsync3) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "a.test2.foo");
  
  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://a.test2.foo/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      "test2.foo", true));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Test that an HTTP scheme can generate cross-domain XHR requests when setting
// the Access-Control-Allow-Origin header.
TEST(SchemeHandlerTest, HttpXHRDifferentOriginWithHeaderSync) {
  RegisterTestScheme("http", "test1");
  RegisterTestScheme("http", "test2");

  XHRTestSettings settings;
  settings.url = "http://test1/run.html";
  settings.sub_url = "http://test2/xhr.html";
  settings.sub_allow_origin = "http://test1";
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that an HTTP scheme can generate cross-domain XHR requests when setting
// the Access-Control-Allow-Origin header.
TEST(SchemeHandlerTest, HttpXHRDifferentOriginWithHeaderAsync) {
  RegisterTestScheme("http", "test1");
  RegisterTestScheme("http", "test2");

  XHRTestSettings settings;
  settings.url = "http://test1/run.html";
  settings.sub_url = "http://test2/xhr.html";
  settings.sub_allow_origin = "http://test1";
  settings.synchronous = false;
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate cross-domain XSS requests
// when using document.domain.
TEST(SchemeHandlerTest, CustomStandardXSSDifferentOriginWithDomain) {
  RegisterTestScheme("customstd", "a.test.com");
  RegisterTestScheme("customstd", "b.test.com");
  SetUpXSS("customstd://a.test.com/run.html",
           "customstd://b.test.com/iframe.html",
           "test.com");

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that an HTTP scheme can generate cross-domain XSS requests when using
// document.domain.
TEST(SchemeHandlerTest, HttpXSSDifferentOriginWithDomain) {
  RegisterTestScheme("http", "a.test.com");
  RegisterTestScheme("http", "b.test.com");
  SetUpXSS("http://a.test.com/run.html",
           "http://b.test.com/iframe.html",
           "test.com");

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme cannot generate cross-domain XHR requests
// that perform redirects.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginRedirectSync) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");

  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.sub_redirect_url = "customstd://test1/xhr.html";
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_redirect);
  EXPECT_FALSE(g_TestResults.got_sub_request);
  EXPECT_FALSE(g_TestResults.got_sub_read);
  EXPECT_FALSE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme cannot generate cross-domain XHR requests
// that perform redirects.
TEST(SchemeHandlerTest, CustomStandardXHRDifferentOriginRedirectAsync) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");

  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.sub_redirect_url = "customstd://test1/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_redirect);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_FALSE(g_TestResults.got_sub_success);

  ClearTestSchemes();
}

// Test that a custom standard scheme cannot generate cross-domain XHR requests
// that perform redirects when using the cross-origin whitelist. This is due to
// an explicit check in SyncResourceHandler::OnRequestRedirected() and does not
// represent ideal behavior.
TEST(SchemeHandlerTest,
     CustomStandardXHRDifferentOriginRedirectWithWhitelistSync) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");

  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.sub_redirect_url = "customstd://test1/xhr.html";
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      "test2", false));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_redirect);
  EXPECT_FALSE(g_TestResults.got_sub_request);
  EXPECT_FALSE(g_TestResults.got_sub_read);
  EXPECT_FALSE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Test that a custom standard scheme can generate cross-domain XHR requests
// that perform redirects when using the cross-origin whitelist. This is
// because we add an "Access-Control-Allow-Origin" header internally in
// CefResourceDispatcherHostDelegate::OnRequestRedirected() for the redirect
// request.
TEST(SchemeHandlerTest,
     CustomStandardXHRDifferentOriginRedirectWithWhitelistAsync1) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");

  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.sub_redirect_url = "customstd://test1/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      "test2", false));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_redirect);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Same as above but origin whitelist matches any domain.
TEST(SchemeHandlerTest,
     CustomStandardXHRDifferentOriginRedirectWithWhitelistAsync2) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "test2");

  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://test2/xhr.html";
  settings.sub_redirect_url = "customstd://test1/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      CefString(), true));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_redirect);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Same as above but origin whitelist matches sub-domains.
TEST(SchemeHandlerTest,
     CustomStandardXHRDifferentOriginRedirectWithWhitelistAsync3) {
  RegisterTestScheme("customstd", "test1");
  RegisterTestScheme("customstd", "a.test2.foo");

  XHRTestSettings settings;
  settings.url = "customstd://test1/run.html";
  settings.sub_url = "customstd://a.test2.foo/xhr.html";
  settings.sub_redirect_url = "customstd://test1/xhr.html";
  settings.synchronous = false;
  SetUpXHR(settings);

  EXPECT_TRUE(CefAddCrossOriginWhitelistEntry("customstd://test1", "customstd",
      "test2.foo", true));
  WaitForUIThread();

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);
  EXPECT_TRUE(g_TestResults.got_sub_redirect);
  EXPECT_TRUE(g_TestResults.got_sub_request);
  EXPECT_TRUE(g_TestResults.got_sub_read);
  EXPECT_TRUE(g_TestResults.got_sub_success);

  EXPECT_TRUE(CefClearCrossOriginWhitelist());
  WaitForUIThread();

  ClearTestSchemes();
}

// Test per-browser setting of Accept-Language.
TEST(SchemeHandlerTest, AcceptLanguage) {
  RegisterTestScheme("customstd", "test");
  g_TestResults.url = "customstd://test/run.html";
  g_TestResults.html =
      "<html><head></head><body><h1>Success!</h1></body></html>";
  g_TestResults.status_code = 200;

  // Value that will be set via CefBrowserSettings.accept_language in
  // PopulateBrowserSettings().
  g_TestResults.accept_language = "uk";

  CefRefPtr<TestSchemeHandler> handler = new TestSchemeHandler(&g_TestResults);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);

  EXPECT_TRUE(g_TestResults.got_request);
  EXPECT_TRUE(g_TestResults.got_read);
  EXPECT_TRUE(g_TestResults.got_output);

  ClearTestSchemes();
}


// Entry point for registering custom schemes.
// Called from client_app_delegates.cc.
void RegisterSchemeHandlerCustomSchemes(
      CefRefPtr<CefSchemeRegistrar> registrar,
      std::vector<CefString>& cookiable_schemes) {
  // Add a custom standard scheme.
  registrar->AddCustomScheme("customstd", true, false, false);
  // Ad a custom non-standard scheme.
  registrar->AddCustomScheme("customnonstd", false, false, false);
}
