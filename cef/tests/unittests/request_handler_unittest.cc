// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "base/strings/stringprintf.h"

#include "include/base/cef_bind.h"
#include "include/cef_cookie.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/cefclient/browser/client_app_browser.h"
#include "tests/cefclient/renderer/client_app_renderer.h"
#include "tests/unittests/test_handler.h"
#include "tests/unittests/test_util.h"

using client::ClientAppBrowser;
using client::ClientAppRenderer;

namespace {

enum NetNotifyTestType {
  NNTT_NONE = 0,
  NNTT_NORMAL,
  NNTT_DELAYED_RENDERER,
  NNTT_DELAYED_BROWSER,
};

const char kNetNotifyOrigin1[] = "http://tests-netnotify1/";
const char kNetNotifyOrigin2[] = "http://tests-netnotify2/";
const char kNetNotifyMsg[] = "RequestHandlerTest.NetNotify";

bool g_net_notify_test = false;

// Browser side.
class NetNotifyBrowserTest : public ClientAppBrowser::Delegate {
 public:
  NetNotifyBrowserTest() {}

  void OnBeforeChildProcessLaunch(
      CefRefPtr<ClientAppBrowser> app,
      CefRefPtr<CefCommandLine> command_line) override {
    if (!g_net_notify_test)
      return;

    // Indicate to the render process that the test should be run.
    command_line->AppendSwitchWithValue("test", kNetNotifyMsg);
  }

 protected:
  IMPLEMENT_REFCOUNTING(NetNotifyBrowserTest);
};

// Browser side.
class NetNotifyTestHandler : public TestHandler {
 public:
  class RequestContextHandler : public CefRequestContextHandler {
   public:
    explicit RequestContextHandler(NetNotifyTestHandler* handler)
        : handler_(handler) {}

    CefRefPtr<CefCookieManager> GetCookieManager() override {
      EXPECT_TRUE(handler_);
      EXPECT_TRUE(CefCurrentlyOn(TID_IO));

      if (url_.find(handler_->url1_) == 0)
        handler_->got_get_cookie_manager1_.yes();
      else if (url_.find(handler_->url2_) == 0)
        handler_->got_get_cookie_manager2_.yes();
      else
        EXPECT_TRUE(false);  // Not reached

      return handler_->cookie_manager_;
    }

    void SetURL(const std::string& url) {
      url_ = url;
    }

    void Detach() {
      handler_ = NULL;
    }

   private:
    std::string url_;
    NetNotifyTestHandler* handler_;

    IMPLEMENT_REFCOUNTING(RequestContextHandler);
  };

  NetNotifyTestHandler(CompletionState* completion_state,
                       NetNotifyTestType test_type,
                       bool same_origin)
      : TestHandler(completion_state),
        test_type_(test_type),
        same_origin_(same_origin) {}

  void SetupTest() override {
    url1_ = base::StringPrintf("%snav1.html?t=%d",
        kNetNotifyOrigin1, test_type_);
    url2_ = base::StringPrintf("%snav2.html?t=%d",
        same_origin_ ? kNetNotifyOrigin1 : kNetNotifyOrigin2, test_type_);

    cookie_manager_ = CefCookieManager::CreateManager(CefString(), true, NULL);

    AddResource(url1_,
        "<html>"
        "<head><script>document.cookie='name1=value1';</script></head>"
        "<body>Nav1</body>"
        "</html>", "text/html");
    AddResource(url2_,
        "<html>"
        "<head><script>document.cookie='name2=value2';</script></head>"
        "<body>Nav2</body>"
        "</html>", "text/html");

    context_handler_ = new RequestContextHandler(this);
    context_handler_->SetURL(url1_);

    // Create the request context that will use an in-memory cache.
    CefRequestContextSettings settings;
    CefRefPtr<CefRequestContext> request_context =
        CefRequestContext::CreateContext(settings, context_handler_.get());

    // Create browser that loads the 1st URL.
    CreateBrowser(url1_, request_context);
  }

  void RunTest() override {
    // Navigate to the 2nd URL.
    context_handler_->SetURL(url2_);
    GetBrowser()->GetMainFrame()->LoadURL(url2_);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  cef_return_value_t OnBeforeResourceLoad(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      CefRefPtr<CefRequestCallback> callback) override {
    EXPECT_TRUE(CefCurrentlyOn(TID_IO));

    const std::string& url = request->GetURL();
    if (url.find(url1_) == 0)
      got_before_resource_load1_.yes();
    else if (url.find(url2_) == 0)
      got_before_resource_load2_.yes();
    else
      EXPECT_TRUE(false);  // Not reached

    return RV_CONTINUE;
  }

  CefRefPtr<CefResourceHandler> GetResourceHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) override {
    EXPECT_TRUE(CefCurrentlyOn(TID_IO));

    const std::string& url = request->GetURL();
    if (url.find(url1_) == 0)
      got_get_resource_handler1_.yes();
    else if (url.find(url2_) == 0)
      got_get_resource_handler2_.yes();
    else
      EXPECT_TRUE(false);  // Not reached

    return TestHandler::GetResourceHandler(browser,  frame, request);
  }

  bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      CefRefPtr<CefRequest> request,
                      bool is_redirect) override {
    std::string url = request->GetURL();

    // Check if the load has already been delayed.
    bool delay_loaded = (url.find("delayed=true") != std::string::npos);

    if (url.find(url1_) == 0) {
      got_before_browse1_.yes();
      EXPECT_FALSE(delay_loaded);
    } else if (url.find(url2_) == 0) {
      got_before_browse2_.yes();
      if (delay_loaded) {
        got_before_browse2_delayed_.yes();
      } else if (test_type_ == NNTT_DELAYED_RENDERER ||
                 test_type_ == NNTT_DELAYED_BROWSER) {
        got_before_browse2_will_delay_.yes();

        // Navigating cross-origin from the browser process will cause a new
        // render process to be created. We therefore need some information in
        // the request itself to tell us that the navigation has already been
        // delayed.
        url += "&delayed=true";

        if (test_type_ == NNTT_DELAYED_RENDERER) {
          // Load the URL from the render process.
          CefRefPtr<CefProcessMessage> message =
              CefProcessMessage::Create(kNetNotifyMsg);
          CefRefPtr<CefListValue> args = message->GetArgumentList();
          args->SetInt(0, test_type_);
          args->SetString(1, url);
          EXPECT_TRUE(browser->SendProcessMessage(PID_RENDERER, message));
        } else {
          // Load the URL from the browser process.
          browser->GetMainFrame()->LoadURL(url);
        }

        // Cancel the load.
        return true;
      }
    } else {
      EXPECT_TRUE(false);  // Not reached
    }

    // Allow the load to continue.
    return false;
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    const std::string& url = frame->GetURL();
    if (url.find(url1_) == 0) {
      got_load_end1_.yes();
      SetupCompleteIfDone();
    } else if (url.find(url2_) == 0) {
      got_load_end2_.yes();
      FinishTestIfDone();
    } else {
      EXPECT_TRUE(false);  // Not reached
    }
  }

  bool OnProcessMessageReceived(
      CefRefPtr<CefBrowser> browser,
      CefProcessId source_process,
      CefRefPtr<CefProcessMessage> message) override {
    if (message->GetName().ToString() == kNetNotifyMsg) {
      CefRefPtr<CefListValue> args = message->GetArgumentList();
      EXPECT_TRUE(args.get());

      std::string url = args->GetString(0);
      if (url.find(url1_) == 0) {
        got_process_message1_.yes();
        SetupCompleteIfDone();
      } else if (url.find(url2_) == 0) {
        got_process_message2_.yes();
        FinishTestIfDone();
      } else {
        EXPECT_TRUE(false);  // Not reached
      }

      return true;
    }

    // Message not handled.
    return false;
  }

 protected:
  void SetupCompleteIfDone() {
    if (got_load_end1_ && got_process_message1_)
      SetupComplete();
  }

  void FinishTestIfDone() {
    if (got_load_end2_ && got_process_message2_)
      FinishTest();
  }

  void FinishTest() {
    // Verify that cookies were set correctly.
    class TestVisitor : public CefCookieVisitor {
     public:
      explicit TestVisitor(NetNotifyTestHandler* handler)
          : handler_(handler) {
      }
      ~TestVisitor() override {
        // Destroy the test.
        CefPostTask(TID_UI,
            base::Bind(&NetNotifyTestHandler::DestroyTest, handler_));
      }

      bool Visit(const CefCookie& cookie, int count, int total,
                 bool& deleteCookie) override {
        const std::string& name = CefString(&cookie.name);
        const std::string& value = CefString(&cookie.value);
        if (name == "name1" && value == "value1")
          handler_->got_cookie1_.yes();
        else if (name == "name2" && value == "value2")
          handler_->got_cookie2_.yes();
        return true;
      }

     private:
      NetNotifyTestHandler* handler_;
      IMPLEMENT_REFCOUNTING(TestVisitor);
    };

    cookie_manager_->VisitAllCookies(new TestVisitor(this));
  }

  void DestroyTest() override {
    int browser_id = GetBrowser()->GetIdentifier();

    // Verify test expectations.
    EXPECT_TRUE(got_before_browse1_) << " browser " << browser_id;
    EXPECT_TRUE(got_load_end1_) << " browser " << browser_id;
    EXPECT_TRUE(got_before_resource_load1_) << " browser " << browser_id;
    EXPECT_TRUE(got_get_resource_handler1_) << " browser " << browser_id;
    EXPECT_TRUE(got_get_cookie_manager1_) << " browser " << browser_id;
    EXPECT_TRUE(got_cookie1_) << " browser " << browser_id;
    EXPECT_TRUE(got_process_message1_) << " browser " << browser_id;
    EXPECT_TRUE(got_before_browse2_) << " browser " << browser_id;
    EXPECT_TRUE(got_load_end2_) << " browser " << browser_id;
    EXPECT_TRUE(got_before_resource_load2_) << " browser " << browser_id;
    EXPECT_TRUE(got_get_resource_handler2_) << " browser " << browser_id;
    EXPECT_TRUE(got_get_cookie_manager2_) << " browser " << browser_id;
    EXPECT_TRUE(got_cookie2_) << " browser " << browser_id;
    EXPECT_TRUE(got_process_message2_) << " browser " << browser_id;

    if (test_type_ == NNTT_DELAYED_RENDERER ||
        test_type_ == NNTT_DELAYED_BROWSER) {
      EXPECT_TRUE(got_before_browse2_will_delay_) << " browser " << browser_id;
      EXPECT_TRUE(got_before_browse2_delayed_) << " browser " << browser_id;
    } else {
      EXPECT_FALSE(got_before_browse2_will_delay_) << " browser " << browser_id;
      EXPECT_FALSE(got_before_browse2_delayed_) << " browser " << browser_id;
    }

    context_handler_->Detach();
    context_handler_ = NULL;
    cookie_manager_ = NULL;
    
    TestHandler::DestroyTest();
  }

  NetNotifyTestType test_type_;
  bool same_origin_;
  std::string url1_;
  std::string url2_;

  CefRefPtr<RequestContextHandler> context_handler_;

  CefRefPtr<CefCookieManager> cookie_manager_;

  TrackCallback got_before_browse1_;
  TrackCallback got_load_end1_;
  TrackCallback got_before_resource_load1_;
  TrackCallback got_get_resource_handler1_;
  TrackCallback got_get_cookie_manager1_;
  TrackCallback got_cookie1_;
  TrackCallback got_process_message1_;
  TrackCallback got_before_browse2_;
  TrackCallback got_load_end2_;
  TrackCallback got_before_resource_load2_;
  TrackCallback got_get_resource_handler2_;
  TrackCallback got_get_cookie_manager2_;
  TrackCallback got_cookie2_;
  TrackCallback got_process_message2_;
  TrackCallback got_before_browse2_will_delay_;
  TrackCallback got_before_browse2_delayed_;

  IMPLEMENT_REFCOUNTING(NetNotifyTestHandler);
};

// Renderer side.
class NetNotifyRendererTest : public ClientAppRenderer::Delegate,
                              public CefLoadHandler {
 public:
  NetNotifyRendererTest()
      : run_test_(false) {}

  void OnRenderThreadCreated(
      CefRefPtr<ClientAppRenderer> app,
      CefRefPtr<CefListValue> extra_info) override {
    if (!g_net_notify_test) {
      // Check that the test should be run.
      CefRefPtr<CefCommandLine> command_line =
          CefCommandLine::GetGlobalCommandLine();
      const std::string& test = command_line->GetSwitchValue("test");
      if (test != kNetNotifyMsg)
        return;
    }

    run_test_ = true;
  }

  CefRefPtr<CefLoadHandler> GetLoadHandler(
      CefRefPtr<ClientAppRenderer> app) override {
    if (run_test_)
      return this;
    return NULL;
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    if (!run_test_)
      return;

    const std::string& url = frame->GetURL();

    // Continue in the browser process.
    CefRefPtr<CefProcessMessage> message =
        CefProcessMessage::Create(kNetNotifyMsg);
    CefRefPtr<CefListValue> args = message->GetArgumentList();
    args->SetString(0, url);
    EXPECT_TRUE(browser->SendProcessMessage(PID_BROWSER, message));
  }

  bool OnProcessMessageReceived(
        CefRefPtr<ClientAppRenderer> app,
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override {
    if (message->GetName().ToString() == kNetNotifyMsg) {
      CefRefPtr<CefListValue> args = message->GetArgumentList();
      EXPECT_TRUE(args.get());

      NetNotifyTestType test_type =
          static_cast<NetNotifyTestType>(args->GetInt(0));
      EXPECT_EQ(test_type, NNTT_DELAYED_RENDERER);

      const std::string& url = args->GetString(1);

      // Load the URL from the render process.
      browser->GetMainFrame()->LoadURL(url);
      return true;
    }

    // Message not handled.
    return false;
  }

 private:
  bool run_test_;

  IMPLEMENT_REFCOUNTING(NetNotifyRendererTest);
};

void RunNetNotifyTest(NetNotifyTestType test_type, bool same_origin) {
  g_net_notify_test = true;

  TestHandler::CompletionState completion_state(3);

  CefRefPtr<NetNotifyTestHandler> handler1 =
      new NetNotifyTestHandler(&completion_state, test_type, same_origin);
  CefRefPtr<NetNotifyTestHandler> handler2 =
      new NetNotifyTestHandler(&completion_state, test_type, same_origin);
  CefRefPtr<NetNotifyTestHandler> handler3 =
      new NetNotifyTestHandler(&completion_state, test_type, same_origin);

  TestHandler::Collection collection(&completion_state);
  collection.AddTestHandler(handler1.get());
  collection.AddTestHandler(handler2.get());
  collection.AddTestHandler(handler3.get());

  collection.ExecuteTests();

  ReleaseAndWaitForDestructor(handler1);
  ReleaseAndWaitForDestructor(handler2);
  ReleaseAndWaitForDestructor(handler3);

  g_net_notify_test = false;
}

}  // namespace

// Verify network notifications for multiple browsers existing simultaniously.
// URL loading is from the same origin and is not delayed.
TEST(RequestHandlerTest, NotificationsSameOriginDirect) {
  RunNetNotifyTest(NNTT_NORMAL, true);
}

// Verify network notifications for multiple browsers existing simultaniously.
// URL loading is from the same origin and is continued asynchronously from the
// render process.
TEST(RequestHandlerTest, NotificationsSameOriginDelayedRenderer) {
  RunNetNotifyTest(NNTT_DELAYED_RENDERER, true);
}

// Verify network notifications for multiple browsers existing simultaniously.
// URL loading is from the same origin and is continued asynchronously from the
// browser process.
TEST(RequestHandlerTest, NotificationsSameOriginDelayedBrowser) {
  RunNetNotifyTest(NNTT_DELAYED_BROWSER, true);
}

// Verify network notifications for multiple browsers existing simultaniously.
// URL loading is from a different origin and is not delayed.
TEST(RequestHandlerTest, NotificationsCrossOriginDirect) {
  RunNetNotifyTest(NNTT_NORMAL, false);
}

// Verify network notifications for multiple browsers existing simultaniously.
// URL loading is from a different origin and is continued asynchronously from
// the render process.
TEST(RequestHandlerTest, NotificationsCrossOriginDelayedRenderer) {
  RunNetNotifyTest(NNTT_DELAYED_RENDERER, false);
}

// Verify network notifications for multiple browsers existing simultaniously.
// URL loading is from a different origin and is continued asynchronously from
// the browser process.
TEST(RequestHandlerTest, NotificationsCrossOriginDelayedBrowser) {
  RunNetNotifyTest(NNTT_DELAYED_BROWSER, false);
}


namespace {

const char kResourceTestHtml[] = "http://test.com/resource.html";

class ResourceResponseTest : public TestHandler {
 public:
  enum TestMode {
    URL,
    HEADER,
    POST,
  };

  explicit ResourceResponseTest(TestMode mode)
      : browser_id_(0),
        main_request_id_(0U),
        sub_request_id_(0U) {
    if (mode == URL)
      resource_test_.reset(new UrlResourceTest);
    else if (mode == HEADER)
      resource_test_.reset(new HeaderResourceTest);
    else
      resource_test_.reset(new PostResourceTest);
  }

  void RunTest() override {
    AddResource(kResourceTestHtml, GetHtml(), "text/html");
    CreateBrowser(kResourceTestHtml);
    SetTestTimeout();
  }

  bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      CefRefPtr<CefRequest> request,
                      bool is_redirect) override {
    EXPECT_UI_THREAD();
    EXPECT_EQ(0, browser_id_);
    browser_id_ = browser->GetIdentifier();
    EXPECT_GT(browser_id_, 0);

    // This method is only called for the main resource.
    EXPECT_STREQ(kResourceTestHtml, request->GetURL().ToString().c_str());

    // Browser-side navigation no longer exposes the actual request information.
    EXPECT_EQ(0U, request->GetIdentifier());

    return false;
  }

  cef_return_value_t OnBeforeResourceLoad(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      CefRefPtr<CefRequestCallback> callback) override {
    EXPECT_IO_THREAD();
    EXPECT_EQ(browser_id_, browser->GetIdentifier());

    if (request->GetURL() == kResourceTestHtml) {
      // All loads of the main resource should keep the same request id.
      EXPECT_EQ(0U, main_request_id_);
      main_request_id_ = request->GetIdentifier();
      EXPECT_GT(main_request_id_, 0U);
      return RV_CONTINUE;
    }

    // All redirects of the sub-resource should keep the same request id.
    if (sub_request_id_ == 0U) {
      sub_request_id_ = request->GetIdentifier();
      EXPECT_GT(sub_request_id_, 0U);
    } else {
      EXPECT_EQ(sub_request_id_, request->GetIdentifier());
    }

    return resource_test_->OnBeforeResourceLoad(browser, frame, request) ?
        RV_CANCEL : RV_CONTINUE;
  }

  CefRefPtr<CefResourceHandler> GetResourceHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) override {
    EXPECT_IO_THREAD();
    EXPECT_EQ(browser_id_, browser->GetIdentifier());

    if (request->GetURL() == kResourceTestHtml) {
      EXPECT_EQ(main_request_id_, request->GetIdentifier());
      return TestHandler::GetResourceHandler(browser, frame, request);
    }

    EXPECT_EQ(sub_request_id_, request->GetIdentifier());
    return resource_test_->GetResourceHandler(browser, frame, request);
  }

  void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefRequest> request,
                          CefString& new_url) override {
    EXPECT_IO_THREAD();
    EXPECT_EQ(browser_id_, browser->GetIdentifier());
    EXPECT_EQ(sub_request_id_, request->GetIdentifier());

    resource_test_->OnResourceRedirect(browser, frame, request, new_url);
  }

  bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefRequest> request,
                          CefRefPtr<CefResponse> response) override {
    EXPECT_IO_THREAD();
    EXPECT_TRUE(browser.get());
    EXPECT_EQ(browser_id_, browser->GetIdentifier());

    EXPECT_TRUE(frame.get());
    EXPECT_TRUE(frame->IsMain());

    if (request->GetURL() == kResourceTestHtml) {
      EXPECT_EQ(main_request_id_, request->GetIdentifier());
      return false;
    }

    EXPECT_EQ(sub_request_id_, request->GetIdentifier());
    return resource_test_->OnResourceResponse(browser, frame, request,
                                              response);
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    EXPECT_UI_THREAD();
    EXPECT_EQ(browser_id_, browser->GetIdentifier());

    TestHandler::OnLoadEnd(browser, frame, httpStatusCode);
    DestroyTest();
  }

  void DestroyTest() override {
    resource_test_->CheckExpected();
    resource_test_.reset(NULL);

    TestHandler::DestroyTest();
  }

 private:
  std::string GetHtml() const {
    std::stringstream html;
    html << "<html><head>";

    const std::string& url = resource_test_->start_url();
    html << "<script type=\"text/javascript\" src=\""
          << url
          << "\"></script>";

    html << "</head><body><p>Main</p></body></html>";
    return html.str();
  }

  class ResourceTest {
   public:
    ResourceTest(const std::string& start_url,
                 size_t expected_resource_response_ct = 2U,
                 size_t expected_before_resource_load_ct = 1U,
                 size_t expected_resource_redirect_ct = 0U)
        : start_url_(start_url),
          resource_response_ct_(0U),
          expected_resource_response_ct_(expected_resource_response_ct),
          before_resource_load_ct_(0),
          expected_before_resource_load_ct_(expected_before_resource_load_ct),
          get_resource_handler_ct_(0U),
          resource_redirect_ct_(0U),
          expected_resource_redirect_ct_(expected_resource_redirect_ct) {
    }
    virtual ~ResourceTest() {
    }

    const std::string& start_url() const {
      return start_url_;
    }

    virtual bool OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefRequest> request) {
      before_resource_load_ct_++;
      return false;
    }

    virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request) {
      get_resource_handler_ct_++;

      const std::string& js_content = "<!-- -->";

      CefRefPtr<CefStreamReader> stream =
          CefStreamReader::CreateForData(const_cast<char*>(js_content.c_str()),
                                         js_content.size());

      return new CefStreamResourceHandler(200, "OK", "text/javascript",
                                          CefResponse::HeaderMap(), stream);
    }

    virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefString& new_url) {
      resource_redirect_ct_++;
    }

    bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request,
                            CefRefPtr<CefResponse> response) {
      EXPECT_TRUE(CheckUrl(request->GetURL()));

      // Verify the response returned by GetResourceHandler.
      EXPECT_EQ(200, response->GetStatus());
      EXPECT_STREQ("OK", response->GetStatusText().ToString().c_str());
      EXPECT_STREQ("text/javascript",
                    response->GetMimeType().ToString().c_str());

      if (resource_response_ct_++ == 0U) {
        // Always redirect at least one time.
        OnResourceReceived(browser, frame, request, response);
        return true;
      }

      OnRetryReceived(browser, frame, request, response);
      return (resource_response_ct_ < expected_resource_response_ct_);
    }

    virtual bool CheckUrl(const std::string& url) const {
      return (url == start_url_);
    }

    virtual void CheckExpected() {
      EXPECT_TRUE(got_resource_);
      EXPECT_TRUE(got_resource_retry_);

      EXPECT_EQ(expected_resource_response_ct_, resource_response_ct_);
      EXPECT_EQ(expected_resource_response_ct_, get_resource_handler_ct_);
      EXPECT_EQ(expected_before_resource_load_ct_, before_resource_load_ct_);
      EXPECT_EQ(expected_resource_redirect_ct_, resource_redirect_ct_);
    }

   protected:
    virtual void OnResourceReceived(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefRequest> request,
                                    CefRefPtr<CefResponse> response) {
      got_resource_.yes();
    }

    virtual void OnRetryReceived(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefRequest> request,
                                 CefRefPtr<CefResponse> response) {
      got_resource_retry_.yes();
    }

   private:
    std::string start_url_;

    size_t resource_response_ct_;
    size_t expected_resource_response_ct_;
    size_t before_resource_load_ct_;
    size_t expected_before_resource_load_ct_;
    size_t get_resource_handler_ct_;
    size_t resource_redirect_ct_;
    size_t expected_resource_redirect_ct_;

    TrackCallback got_resource_;
    TrackCallback got_resource_retry_;
  };

  class UrlResourceTest : public ResourceTest {
   public:
    UrlResourceTest()
        : ResourceTest("http://test.com/start_url.js", 3U, 2U, 1U) {
      redirect_url_ = "http://test.com/redirect_url.js";
    }

    bool CheckUrl(const std::string& url) const override {
      if (url == redirect_url_)
        return true;

      return ResourceTest::CheckUrl(url);
    }

    void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request,
                            CefString& new_url) override {
      ResourceTest::OnResourceRedirect(browser, frame, request, new_url);
      const std::string& old_url = request->GetURL();
      EXPECT_STREQ(start_url().c_str(), old_url.c_str());
      EXPECT_STREQ(redirect_url_.c_str(), new_url.ToString().c_str());
    }

   private:
    void OnResourceReceived(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request,
                            CefRefPtr<CefResponse> response) override {
      ResourceTest::OnResourceReceived(browser, frame, request, response);
      request->SetURL(redirect_url_);
    }

    void OnRetryReceived(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefRequest> request,
                         CefRefPtr<CefResponse> response) override {
      ResourceTest::OnRetryReceived(browser, frame, request, response);
      const std::string& new_url = request->GetURL();
      EXPECT_STREQ(redirect_url_.c_str(), new_url.c_str());
    }

    std::string redirect_url_;
  };

  class HeaderResourceTest : public ResourceTest {
   public:
    HeaderResourceTest()
        : ResourceTest("http://test.com/start_header.js") {
      expected_headers_.insert(std::make_pair("Test-Key1", "Value1"));
      expected_headers_.insert(std::make_pair("Test-Key2", "Value2"));
    }

   private:
    void OnResourceReceived(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request,
                            CefRefPtr<CefResponse> response) override {
      ResourceTest::OnResourceReceived(browser, frame, request, response);
      request->SetHeaderMap(expected_headers_);
    }

    void OnRetryReceived(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefRequest> request,
                         CefRefPtr<CefResponse> response) override {
      ResourceTest::OnRetryReceived(browser, frame, request, response);
      CefRequest::HeaderMap actual_headers;
      request->GetHeaderMap(actual_headers);
      TestMapEqual(expected_headers_, actual_headers, true);
    }

    CefRequest::HeaderMap expected_headers_;
  };

  class PostResourceTest : public ResourceTest {
   public:
    PostResourceTest()
        : ResourceTest("http://test.com/start_post.js") {
      CefRefPtr<CefPostDataElement> elem = CefPostDataElement::Create();
      const std::string data("Test Post Data");
      elem->SetToBytes(data.size(), data.c_str());

      expected_post_ = CefPostData::Create();
      expected_post_->AddElement(elem);
    }

   private:
    void OnResourceReceived(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefRequest> request,
                            CefRefPtr<CefResponse> response) override {
      ResourceTest::OnResourceReceived(browser, frame, request, response);
      request->SetPostData(expected_post_);
    }

    void OnRetryReceived(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefRequest> request,
                         CefRefPtr<CefResponse> response) override {
      ResourceTest::OnRetryReceived(browser, frame, request, response);
      CefRefPtr<CefPostData> actual_post = request->GetPostData();
      TestPostDataEqual(expected_post_, actual_post);
    }

    CefRefPtr<CefPostData> expected_post_;
  };

  int browser_id_;
  uint64 main_request_id_;
  uint64 sub_request_id_;
  scoped_ptr<ResourceTest> resource_test_;

  IMPLEMENT_REFCOUNTING(ResourceResponseTest);
};

}  // namespace

TEST(RequestHandlerTest, ResourceResponseURL) {
  CefRefPtr<ResourceResponseTest> handler =
      new ResourceResponseTest(ResourceResponseTest::URL);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(RequestHandlerTest, ResourceResponseHeader) {
  CefRefPtr<ResourceResponseTest> handler =
      new ResourceResponseTest(ResourceResponseTest::HEADER);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(RequestHandlerTest, ResourceResponsePost) {
  CefRefPtr<ResourceResponseTest> handler =
      new ResourceResponseTest(ResourceResponseTest::POST);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


namespace {

const char kResourceTestHtml2[] = "http://test.com/resource2.html";

class BeforeResourceLoadTest : public TestHandler {
 public:
  enum TestMode {
    CANCEL,
    CANCEL_ASYNC,
    CANCEL_NAV,
    CONTINUE,
    CONTINUE_ASYNC,
  };

  explicit BeforeResourceLoadTest(TestMode mode)
      : test_mode_(mode) {
  }

  void RunTest() override {
    AddResource(kResourceTestHtml, "<html><body>Test</body></html>",
                "text/html");
    AddResource(kResourceTestHtml2, "<html><body>Test2</body></html>",
                "text/html");
    CreateBrowser(kResourceTestHtml);
    SetTestTimeout();
  }

  cef_return_value_t OnBeforeResourceLoad(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      CefRefPtr<CefRequestCallback> callback) override {
    EXPECT_IO_THREAD();

    // Allow the 2nd navigation to continue.
    const std::string& url = request->GetURL();
    if (url == kResourceTestHtml2) {
      got_before_resource_load2_.yes();
      EXPECT_EQ(CANCEL_NAV, test_mode_);
      return RV_CONTINUE;
    }
    
    EXPECT_FALSE(got_before_resource_load_);
    got_before_resource_load_.yes();

    if (test_mode_ == CANCEL) {
      // Cancel immediately.
      return RV_CANCEL;
    } else if (test_mode_ == CONTINUE) {
      // Continue immediately.
      return RV_CONTINUE;
    } else {
      if (test_mode_ == CANCEL_NAV) {
        // Cancel the request by navigating to a new URL.
        browser->GetMainFrame()->LoadURL(kResourceTestHtml2);
      } else {
        // Continue or cancel asynchronously.
        CefPostTask(TID_UI,
            base::Bind(&CefRequestCallback::Continue, callback.get(),
                       test_mode_ == CONTINUE_ASYNC));
      }
      return RV_CONTINUE_ASYNC;
    }
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    EXPECT_UI_THREAD();

    EXPECT_FALSE(got_load_end_);
    got_load_end_.yes();

    const std::string& url = frame->GetURL();
    if (test_mode_ == CANCEL_NAV)
      EXPECT_STREQ(kResourceTestHtml2, url.data());
    else
      EXPECT_STREQ(kResourceTestHtml, url.data());

    TestHandler::OnLoadEnd(browser, frame, httpStatusCode);
    DestroyTest();
  }

  void OnLoadError(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   ErrorCode errorCode,
                   const CefString& errorText,
                   const CefString& failedUrl) override {
    EXPECT_UI_THREAD();

    EXPECT_FALSE(got_load_error_);
    got_load_error_.yes();

    const std::string& url = failedUrl;
    EXPECT_STREQ(kResourceTestHtml, url.data());

    TestHandler::OnLoadError(browser, frame, errorCode, errorText, failedUrl);
    if (test_mode_ != CANCEL_NAV)
      DestroyTest();
  }

  void DestroyTest() override {
    EXPECT_TRUE(got_before_resource_load_);

    if (test_mode_ == CANCEL_NAV)
      EXPECT_TRUE(got_before_resource_load2_);
    else
      EXPECT_FALSE(got_before_resource_load2_);

    if (test_mode_ == CONTINUE || test_mode_ == CONTINUE_ASYNC) {
      EXPECT_TRUE(got_load_end_);
      EXPECT_FALSE(got_load_error_);
    } else if (test_mode_ == CANCEL || test_mode_ == CANCEL_ASYNC) {
      EXPECT_FALSE(got_load_end_);
      EXPECT_TRUE(got_load_error_);
    }
    
    TestHandler::DestroyTest();
  }

 private:
  const TestMode test_mode_;

  TrackCallback got_before_resource_load_;
  TrackCallback got_before_resource_load2_;
  TrackCallback got_load_end_;
  TrackCallback got_load_error_;

  IMPLEMENT_REFCOUNTING(BeforeResourceLoadTest);
};

}  // namespace

TEST(RequestHandlerTest, BeforeResourceLoadCancel) {
  CefRefPtr<BeforeResourceLoadTest> handler =
      new BeforeResourceLoadTest(BeforeResourceLoadTest::CANCEL);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(RequestHandlerTest, BeforeResourceLoadCancelAsync) {
  CefRefPtr<BeforeResourceLoadTest> handler =
      new BeforeResourceLoadTest(BeforeResourceLoadTest::CANCEL_ASYNC);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(RequestHandlerTest, BeforeResourceLoadCancelNav) {
  CefRefPtr<BeforeResourceLoadTest> handler =
      new BeforeResourceLoadTest(BeforeResourceLoadTest::CANCEL_NAV);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(RequestHandlerTest, BeforeResourceLoadContinue) {
  CefRefPtr<BeforeResourceLoadTest> handler =
      new BeforeResourceLoadTest(BeforeResourceLoadTest::CONTINUE);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(RequestHandlerTest, BeforeResourceLoadContinueAsync) {
  CefRefPtr<BeforeResourceLoadTest> handler =
      new BeforeResourceLoadTest(BeforeResourceLoadTest::CONTINUE_ASYNC);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


// Entry point for creating request handler browser test objects.
// Called from client_app_delegates.cc.
void CreateRequestHandlerBrowserTests(
    ClientAppBrowser::DelegateSet& delegates) {
  delegates.insert(new NetNotifyBrowserTest);
}

// Entry point for creating request handler renderer test objects.
// Called from client_app_delegates.cc.
void CreateRequestHandlerRendererTests(
    ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new NetNotifyRendererTest);
}
