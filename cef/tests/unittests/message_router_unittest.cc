// Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <cstdlib>
#include <set>
#include <vector>

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "base/strings/stringprintf.h"

#include "include/base/cef_bind.h"
#include "include/base/cef_weak_ptr.h"
#include "include/cef_v8.h"
#include "include/wrapper/cef_closure_task.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/routing_test_handler.h"
#include "tests/cefclient/renderer/client_app_renderer.h"

using client::ClientAppRenderer;

namespace {

const char kTestDomainRoot[] = "http://tests-mr";
const char kTestDomain1[] = "http://tests-mr1.com/";
const char kTestDomain2[] = "http://tests-mr2.com/";
const char kTestDomain3[] = "http://tests-mr3.com/";

const char kDoneMessageName[] = "mrtNotifyMsg";

const char kJSNotifyFunc[] = "mrtNotify";
const char kJSAssertTotalCountFunc[] = "mrtAssertTotalCount";
const char kJSAssertBrowserCountFunc[] = "mrtAssertBrowserCount";
const char kJSAssertContextCountFunc[] = "mrtAssertContextCount";

void SetRouterConfig(CefMessageRouterConfig& config) {
  config.js_query_function = "mrtQuery";
  config.js_cancel_function = "mrtQueryCancel";
}

// Handle the renderer side of the routing implementation.
class MRRenderDelegate : public ClientAppRenderer::Delegate {
 public:
  class V8HandlerImpl : public CefV8Handler {
   public:
    explicit V8HandlerImpl(CefRefPtr<MRRenderDelegate> delegate)
        : delegate_(delegate) {
    }

    bool Execute(const CefString& name,
                 CefRefPtr<CefV8Value> object,
                 const CefV8ValueList& arguments,
                 CefRefPtr<CefV8Value>& retval,
                 CefString& exception) override {
      const std::string& message_name = name;
      if (message_name == kJSNotifyFunc) {
        EXPECT_EQ(1U, arguments.size());
        EXPECT_TRUE(arguments[0]->IsString());

        const CefString& msg = arguments[0]->GetStringValue();
        CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
        CefRefPtr<CefBrowser> browser = context->GetBrowser();
        CefRefPtr<CefFrame> frame = context->GetFrame();
        const int64 frame_id = frame->GetIdentifier();

        CefRefPtr<CefProcessMessage> message =
            CefProcessMessage::Create(kDoneMessageName);
        CefRefPtr<CefListValue> args = message->GetArgumentList();
        args->SetInt(0, CefInt64GetLow(frame_id));
        args->SetInt(1, CefInt64GetHigh(frame_id));
        args->SetString(2, msg);
        EXPECT_TRUE(browser->SendProcessMessage(PID_BROWSER, message));
        return true;
      } else {
        EXPECT_EQ(1U, arguments.size());
        EXPECT_TRUE(arguments[0]->IsInt());

        const int expected_count = arguments[0]->GetIntValue();
        int actual_count = -1;

        CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
        CefRefPtr<CefBrowser> browser = context->GetBrowser();

        if (name == kJSAssertTotalCountFunc) {
          actual_count =
              delegate_->message_router_->GetPendingCount(NULL, NULL);
        } else if (name == kJSAssertBrowserCountFunc) {
          actual_count =
              delegate_->message_router_->GetPendingCount(browser, NULL);
        } else if (name == kJSAssertContextCountFunc) {
          actual_count =
              delegate_->message_router_->GetPendingCount(browser, context);
        }

        if (expected_count != actual_count) {
          const std::string& exceptionStr =
              base::StringPrintf("%s failed; expected %d, got %d",
                                 message_name.c_str(), expected_count,
                                 actual_count);
          exception = exceptionStr;
        }
      }

      return true;
    }

   private:
    CefRefPtr<MRRenderDelegate> delegate_;

    IMPLEMENT_REFCOUNTING(V8HandlerImpl);
  };

  MRRenderDelegate() {}

  void OnWebKitInitialized(CefRefPtr<ClientAppRenderer> app) override {
    // Create the renderer-side router for query handling.
    CefMessageRouterConfig config;
    SetRouterConfig(config);
    message_router_ = CefMessageRouterRendererSide::Create(config);
  }

  void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
                        CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context) override {
    const std::string& url = frame->GetURL();
    if (url.find(kTestDomainRoot) != 0)
      return;

    message_router_->OnContextCreated(browser,  frame, context);

    // Register function handlers with the 'window' object.
    CefRefPtr<CefV8Value> window = context->GetGlobal();

    CefRefPtr<V8HandlerImpl> handler = new V8HandlerImpl(this);
    CefV8Value::PropertyAttribute attributes =
        static_cast<CefV8Value::PropertyAttribute>(
            V8_PROPERTY_ATTRIBUTE_READONLY |
            V8_PROPERTY_ATTRIBUTE_DONTENUM |
            V8_PROPERTY_ATTRIBUTE_DONTDELETE);

    CefRefPtr<CefV8Value> notify_func =
        CefV8Value::CreateFunction(kJSNotifyFunc, handler.get());
    window->SetValue(kJSNotifyFunc, notify_func, attributes);

    CefRefPtr<CefV8Value> total_count_func =
        CefV8Value::CreateFunction(kJSAssertTotalCountFunc, handler.get());
    window->SetValue(kJSAssertTotalCountFunc, total_count_func, attributes);

    CefRefPtr<CefV8Value> browser_count_func =
        CefV8Value::CreateFunction(kJSAssertBrowserCountFunc, handler.get());
    window->SetValue(kJSAssertBrowserCountFunc, browser_count_func, attributes);

    CefRefPtr<CefV8Value> context_count_func =
        CefV8Value::CreateFunction(kJSAssertContextCountFunc, handler.get());
    window->SetValue(kJSAssertContextCountFunc, context_count_func, attributes);
  }

  void OnContextReleased(CefRefPtr<ClientAppRenderer> app,
                         CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefV8Context> context) override {
    const std::string& url = frame->GetURL();
    if (url.find(kTestDomainRoot) != 0)
      return;

    message_router_->OnContextReleased(browser,  frame, context);
  }

  bool OnProcessMessageReceived(
      CefRefPtr<ClientAppRenderer> app,
      CefRefPtr<CefBrowser> browser,
      CefProcessId source_process,
      CefRefPtr<CefProcessMessage> message) override {
    const std::string& url = browser->GetMainFrame()->GetURL();
    if (url.find(kTestDomainRoot) != 0)
      return false;

    return message_router_->OnProcessMessageReceived(
        browser, source_process, message);
  }

 private:
  CefRefPtr<CefMessageRouterRendererSide> message_router_;

  IMPLEMENT_REFCOUNTING(MRRenderDelegate);
};

}  // namespace

// Entry point for creating the test delegate.
// Called from client_app_delegates.cc.
void CreateMessageRouterRendererTests(
    ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new MRRenderDelegate);
}


namespace {

class MRTestHandler : public TestHandler {
 public:
  MRTestHandler() {
  }

  void RunTest() override {
    RunMRTest();

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
    if (!message_router_.get()) {
      // Create the browser-side router for query handling.
      CefMessageRouterConfig config;
      SetRouterConfig(config);
      message_router_ = CefMessageRouterBrowserSide::Create(config);
      AddHandlers(message_router_);
    }
    TestHandler::OnAfterCreated(browser);
  }

  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override{
    message_router_->OnBeforeClose(browser);
    TestHandler::OnBeforeClose(browser);
  }

  void OnRenderProcessTerminated(
      CefRefPtr<CefBrowser> browser,
      TerminationStatus status) override{
    message_router_->OnRenderProcessTerminated(browser);
  }

  // Only call this method if the navigation isn't canceled.
  bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      CefRefPtr<CefRequest> request,
                      bool is_redirect) override{
    message_router_->OnBeforeBrowse(browser, frame);
    return false;
  }

  // Returns true if the router handled the navigation.
  bool OnProcessMessageReceived(
      CefRefPtr<CefBrowser> browser,
      CefProcessId source_process,
      CefRefPtr<CefProcessMessage> message) override{
    const std::string& message_name = message->GetName();
    if (message_name == kDoneMessageName) {
      CefRefPtr<CefListValue> args = message->GetArgumentList();
      EXPECT_EQ(3U, args->GetSize());
      EXPECT_EQ(VTYPE_INT, args->GetType(0));
      EXPECT_EQ(VTYPE_INT, args->GetType(1));
      EXPECT_EQ(VTYPE_STRING, args->GetType(2));

      const int64 frame_id = CefInt64Set(args->GetInt(0), args->GetInt(1));
      CefRefPtr<CefFrame> frame = browser->GetFrame(frame_id);
      EXPECT_TRUE(frame.get());

      OnNotify(browser, frame, args->GetString(2));
      return true;
    }

    return message_router_->OnProcessMessageReceived(
        browser, source_process,  message);
  }

  CefRefPtr<CefMessageRouterBrowserSide> GetRouter() const {
    return message_router_;
  };

 protected:
  virtual void RunMRTest() =0;

  virtual void AddHandlers(
      CefRefPtr<CefMessageRouterBrowserSide> message_router) =0;

  virtual void OnNotify(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        const std::string& message) =0;

  bool AssertQueryCount(CefRefPtr<CefBrowser> browser,
                        CefMessageRouterBrowserSide::Handler* handler,
                        int expected_count) {
    int actual_count = message_router_->GetPendingCount(browser, handler);
    EXPECT_EQ(expected_count, actual_count);
    return (expected_count == actual_count);
  }

  void AssertMainBrowser(CefRefPtr<CefBrowser> browser) {
    EXPECT_TRUE(browser.get());
    EXPECT_EQ(GetBrowserId(), browser->GetIdentifier());
  }

 private:
  CefRefPtr<CefMessageRouterBrowserSide> message_router_;

  IMPLEMENT_REFCOUNTING(MRTestHandler);
};


// Implementation of MRTestHandler that loads a single page.
class SingleLoadTestHandler :
    public MRTestHandler,
    public CefMessageRouterBrowserSide::Handler {
 public:
  SingleLoadTestHandler()
      : main_url_(std::string(kTestDomain1) + "main.html") {}

  const std::string& GetMainURL() { return main_url_; }

 protected:
  void RunMRTest() override {
    AddOtherResources();
    AddResource(main_url_, GetMainHTML(), "text/html");

    CreateBrowser(main_url_, NULL);
  }

  void AddHandlers(
      CefRefPtr<CefMessageRouterBrowserSide> message_router) override {
    message_router->AddHandler(this, false);
  }

  virtual void AddOtherResources() {
  }

  virtual std::string GetMainHTML() =0;

  void AssertMainFrame(CefRefPtr<CefFrame> frame) {
    EXPECT_TRUE(frame.get());
    EXPECT_TRUE(frame->IsMain());
    EXPECT_STREQ(main_url_.c_str(), frame->GetURL().ToString().c_str());
  }

 private:
  const std::string main_url_;
};

// Used to verify that the test harness (bound functions) behave correctly.
class HarnessTestHandler : public SingleLoadTestHandler {
 public:
  HarnessTestHandler(bool test_success)
    : test_success_(test_success) {}

  std::string GetMainHTML() override {
    std::string html;
    if (test_success_) {
      // All assertions should pass.
      html =
          "<html><body><script>\n"
          "var fail_ct = 0;\n"
          "try { window.mrtAssertTotalCount(0); } catch (e) { fail_ct++; }\n"
          "try { window.mrtAssertBrowserCount(0); } catch (e) { fail_ct++; }\n"
          "try { window.mrtAssertContextCount(0); } catch (e) { fail_ct++; }\n"
          "window.mrtNotify('' + (fail_ct == 0));"
          "</script></body></html>";
    } else {
      // All assertions should fail.
      html =
          "<html><body><script>\n"
          "var fail_ct = 0;\n"
          "try { window.mrtAssertTotalCount(1); } catch (e) { fail_ct++; }\n"
          "try { window.mrtAssertBrowserCount(1); } catch (e) { fail_ct++; }\n"
          "try { window.mrtAssertContextCount(1); } catch (e) { fail_ct++; }\n"
          "window.mrtNotify('' + (fail_ct == 3));"
          "</script></body></html>";
    }
    return html;
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);

    got_done_.yes();
    EXPECT_STREQ("true", message.c_str());
    DestroyTest();
  }

  void DestroyTest() override {
    EXPECT_TRUE(got_done_);
    TestHandler::DestroyTest();
  }

 private:
  const bool test_success_;
  TrackCallback got_done_;
};

}  // namespace

// Verify that the test harness works with successful assertions.
TEST(MessageRouterTest, HarnessSuccess) {
  CefRefPtr<HarnessTestHandler> handler = new HarnessTestHandler(true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Verify that the test harness works with failed assertions.
TEST(MessageRouterTest, HarnessFailure) {
  CefRefPtr<HarnessTestHandler> handler = new HarnessTestHandler(false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


namespace {

const char kSingleQueryRequest[] = "request_context";
const char kSingleQueryResponse[] = "success_response";
const int kSingleQueryErrorCode = 5;
const char kSingleQueryErrorMessage[] = "error_message";

// Test a single query in a single page load.
class SingleQueryTestHandler : public SingleLoadTestHandler {
 public:
  enum TestType {
    SUCCESS,
    FAILURE,
    CANCEL,
  };

  SingleQueryTestHandler(TestType type, bool sync_callback)
      : test_type_(type),
        sync_callback_(sync_callback),
        query_id_(0) {}

  std::string GetMainHTML() override {
    std::string html;

    const std::string& errorCodeStr =
        base::StringPrintf("%d", kSingleQueryErrorCode);

    html = "<html><body><script>\n"
           // No requests should exist.
           "window.mrtAssertTotalCount(0);\n"
           "window.mrtAssertBrowserCount(0);\n"
           "window.mrtAssertContextCount(0);\n"
           // Send the query.
           "var request_id = window.mrtQuery({\n"
           "  request: '" + std::string(kSingleQueryRequest) + "',\n"
           "  persistent: false,\n"
           "  onSuccess: function(response) {\n"
                // Request should be removed before callback is executed.
           "    window.mrtAssertTotalCount(0);\n"
           "    window.mrtAssertBrowserCount(0);\n"
           "    window.mrtAssertContextCount(0);\n"
           "    if (response == '" + std::string(kSingleQueryResponse) + "')\n"
           "      window.mrtNotify('success');\n"
           "    else\n"
           "      window.mrtNotify('error-onSuccess');\n"
           "  },\n"
           "  onFailure: function(error_code, error_message) {\n"
                // Request should be removed before callback is executed.
           "    window.mrtAssertTotalCount(0);\n"
           "    window.mrtAssertBrowserCount(0);\n"
           "    window.mrtAssertContextCount(0);\n"
           "    if (error_code == " + errorCodeStr +
                    " && error_message == '" +
                    std::string(kSingleQueryErrorMessage) + "')\n"
           "      window.mrtNotify('failure');\n"
           "    else\n"
           "      window.mrtNotify('error-onFailure');\n"
           "  }\n"
           "});\n"
           // Request should exist.
           "window.mrtAssertTotalCount(1);\n"
           "window.mrtAssertBrowserCount(1);\n"
           "window.mrtAssertContextCount(1);\n";

    if (test_type_ == CANCEL) {
      html += "window.mrtQueryCancel(request_id);\n"
              // Request should be removed immediately.
              "window.mrtAssertTotalCount(0);\n"
              "window.mrtAssertBrowserCount(0);\n"
              "window.mrtAssertContextCount(0);\n"
              "window.mrtNotify('cancel');\n";
    }

    html += "</script></body></html>";
    return html;
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);

    // OnNotify only be called once.
    EXPECT_FALSE(got_notify_);
    got_notify_.yes();

    if (test_type_ == SUCCESS) {
      EXPECT_STREQ("success", message.c_str());
    } else if (test_type_ == FAILURE) {
      EXPECT_STREQ("failure", message.c_str());
    } else if (test_type_ == CANCEL) {
      EXPECT_STREQ("cancel", message.c_str());
    }

    DestroyTestIfDone();
  }

  void ExecuteCallback() {
    EXPECT_TRUE(callback_.get());
    if (test_type_ == SUCCESS) {
      callback_->Success(kSingleQueryResponse);
    } else if (test_type_ == FAILURE) {
      callback_->Failure(kSingleQueryErrorCode, kSingleQueryErrorMessage);
    } else {
      EXPECT_TRUE(false);  // Not reached.
    }
    callback_ = NULL;
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);
    EXPECT_NE(0, query_id);
    EXPECT_FALSE(persistent);
    EXPECT_STREQ(kSingleQueryRequest, request.ToString().c_str());

    got_on_query_.yes();

    query_id_ = query_id;
    callback_ = callback;

    if (test_type_ == SUCCESS || test_type_ == FAILURE) {
      if (sync_callback_) {
        ExecuteCallback();
      } else {
        CefPostTask(TID_UI,
            base::Bind(&SingleQueryTestHandler::ExecuteCallback, this));
      }
    }

    return true;
  }

  void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);
    EXPECT_EQ(test_type_, CANCEL);
    EXPECT_EQ(query_id_, query_id);
    EXPECT_TRUE(got_on_query_);
    EXPECT_TRUE(callback_.get());

    got_on_query_canceled_.yes();
    callback_ = NULL;

    DestroyTestIfDone();
  }

  void DestroyTestIfDone() {
    bool destroy_test = false;
    if (test_type_ == CANCEL)
      destroy_test = got_notify_ && got_on_query_canceled_;
    else
      destroy_test = got_notify_;
    if (destroy_test)
      DestroyTest();
  }

  void DestroyTest() override {
    EXPECT_TRUE(got_notify_);
    EXPECT_TRUE(got_on_query_);
    EXPECT_FALSE(callback_.get());

    if (test_type_ == CANCEL)
      EXPECT_TRUE(got_on_query_canceled_);
    else
      EXPECT_FALSE(got_on_query_canceled_);

    TestHandler::DestroyTest();
  }

 private:
  const TestType test_type_;
  const bool sync_callback_;

  int64 query_id_;
  CefRefPtr<Callback> callback_;

  TrackCallback got_on_query_;
  TrackCallback got_on_query_canceled_;
  TrackCallback got_notify_;
};

}  // namespace

// Test that a single query with successful result delivered synchronously.
TEST(MessageRouterTest, SingleQuerySuccessSyncCallback) {
  CefRefPtr<SingleQueryTestHandler> handler =
      new SingleQueryTestHandler(SingleQueryTestHandler::SUCCESS, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that a single query with successful result delivered asynchronously.
TEST(MessageRouterTest, SingleQuerySuccessAsyncCallback) {
  CefRefPtr<SingleQueryTestHandler> handler =
      new SingleQueryTestHandler(SingleQueryTestHandler::SUCCESS, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that a single query with failure result delivered synchronously.
TEST(MessageRouterTest, SingleQueryFailureSyncCallback) {
  CefRefPtr<SingleQueryTestHandler> handler =
      new SingleQueryTestHandler(SingleQueryTestHandler::FAILURE, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that a single query with failure result delivered asynchronously.
TEST(MessageRouterTest, SingleQueryFailureAsyncCallback) {
  CefRefPtr<SingleQueryTestHandler> handler =
      new SingleQueryTestHandler(SingleQueryTestHandler::FAILURE, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that a single query with cancellation.
TEST(MessageRouterTest, SingleQueryCancel) {
  CefRefPtr<SingleQueryTestHandler> handler =
      new SingleQueryTestHandler(SingleQueryTestHandler::CANCEL, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


namespace {

const int kSinglePersistentQueryResponseCount = 10;

// Test a single persistent query in a single page load.
class SinglePersistentQueryTestHandler : public SingleLoadTestHandler {
 public:
  enum TestType {
    SUCCESS,
    FAILURE,
  };

  SinglePersistentQueryTestHandler(TestType test_type, bool sync_callback)
      : test_type_(test_type),
        sync_callback_(sync_callback),
        query_id_(0) {}

  std::string GetMainHTML() override {
    std::string html;

    const std::string& responseCountStr =
        base::StringPrintf("%d", kSinglePersistentQueryResponseCount);
    const std::string& errorCodeStr =
        base::StringPrintf("%d", kSingleQueryErrorCode);

    html = "<html><body><script>\n"
           // No requests should exist.
           "window.mrtAssertTotalCount(0);\n"
           "window.mrtAssertBrowserCount(0);\n"
           "window.mrtAssertContextCount(0);\n"
           // Keep track of the number of responses.
           "var count = 0;\n"
           // Send the query.
           "var request_id = window.mrtQuery({\n"
           "  request: '" + std::string(kSingleQueryRequest) + "',\n"
           "  persistent: true,\n"
           "  onSuccess: function(response) {\n"
                // Request should not be removed.
           "    window.mrtAssertTotalCount(1);\n"
           "    window.mrtAssertBrowserCount(1);\n"
           "    window.mrtAssertContextCount(1);\n"
           "    if (response == '" + std::string(kSingleQueryResponse) + "') {\n"
           "      if (++count == " + responseCountStr + ") {\n"
           "        window.mrtNotify('success');\n"
           "        window.mrtQueryCancel(request_id);\n"
                    // Request should be removed immediately.
           "        window.mrtAssertTotalCount(0);\n"
           "        window.mrtAssertBrowserCount(0);\n"
           "        window.mrtAssertContextCount(0);\n"
           "      }\n"
           "    } else {\n"
           "      window.mrtNotify('error-onSuccess');\n"
           "    }\n"
           "  },\n"
           "  onFailure: function(error_code, error_message) {\n"
                // Request should be removed before callback is executed.
           "    window.mrtAssertTotalCount(0);\n"
           "    window.mrtAssertBrowserCount(0);\n"
           "    window.mrtAssertContextCount(0);\n"
           "    if (error_code == " + errorCodeStr +
                    " && error_message == '" +
                    std::string(kSingleQueryErrorMessage) + "') {\n"
           "      window.mrtNotify('failure');\n"
           "    } else {\n"
           "      window.mrtNotify('error-onFailure');\n"
           "    }\n"
           "  }\n"
           "});\n"
           // Request should exist.
           "window.mrtAssertTotalCount(1);\n"
           "window.mrtAssertBrowserCount(1);\n"
           "window.mrtAssertContextCount(1);\n";

    html += "</script></body></html>";
    return html;
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);

    if (test_type_ == SUCCESS)
      EXPECT_STREQ("success", message.c_str());
    else if (test_type_ == FAILURE)
      EXPECT_STREQ("failure", message.c_str());

    got_notify_.yes();

    DestroyTestIfDone();
  }

  void ExecuteCallback() {
    EXPECT_TRUE(callback_.get());
    if (test_type_ == SUCCESS) {
      callback_->Success(kSingleQueryResponse);
    } else {
      callback_->Failure(kSingleQueryErrorCode, kSingleQueryErrorMessage);
      callback_ = NULL;
    }
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);
    EXPECT_NE(0, query_id);
    EXPECT_TRUE(persistent);
    EXPECT_STREQ(kSingleQueryRequest, request.ToString().c_str());

    got_on_query_.yes();

    query_id_ = query_id;
    callback_ = callback;

    int repeat =
        (test_type_ == SUCCESS ? kSinglePersistentQueryResponseCount : 1);

    for (int i = 0; i < repeat; ++i) {
      if (sync_callback_) {
        ExecuteCallback();
      } else {
        CefPostTask(TID_UI,
            base::Bind(&SinglePersistentQueryTestHandler::ExecuteCallback,
                       this));
      }
    }

    return true;
  }

  void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);
    EXPECT_EQ(query_id_, query_id);
    EXPECT_TRUE(got_on_query_);
    EXPECT_TRUE(callback_.get());

    got_on_query_canceled_.yes();
    callback_ = NULL;

    DestroyTestIfDone();
  }

  void DestroyTestIfDone() {
    bool destroy_test = false;
    if (test_type_ == SUCCESS) {
      if (got_on_query_ && got_on_query_canceled_ && got_notify_)
        destroy_test = true;
    } else if (got_on_query_ && got_notify_) {
      destroy_test = true;
    }

    if (destroy_test)
      DestroyTest();
  }

  void DestroyTest() override {
    EXPECT_TRUE(got_notify_);
    EXPECT_TRUE(got_on_query_);
    EXPECT_FALSE(callback_.get());

    if (test_type_ == SUCCESS)
      EXPECT_TRUE(got_on_query_canceled_);
    else
      EXPECT_FALSE(got_on_query_canceled_);

    TestHandler::DestroyTest();
  }

 private:
  const TestType test_type_;
  const bool sync_callback_;

  int64 query_id_;
  CefRefPtr<Callback> callback_;

  TrackCallback got_on_query_;
  TrackCallback got_on_query_canceled_;
  TrackCallback got_notify_;
};

}  // namespace

// Test that a single query with successful result delivered synchronously.
TEST(MessageRouterTest, SinglePersistentQuerySuccessSyncCallback) {
  CefRefPtr<SinglePersistentQueryTestHandler> handler =
      new SinglePersistentQueryTestHandler(
          SinglePersistentQueryTestHandler::SUCCESS, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that a single query with successful result delivered asynchronously.
TEST(MessageRouterTest, SinglePersistentQuerySuccessAsyncCallback) {
  CefRefPtr<SinglePersistentQueryTestHandler> handler =
      new SinglePersistentQueryTestHandler(
          SinglePersistentQueryTestHandler::SUCCESS, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that a single query with failure result delivered synchronously.
TEST(MessageRouterTest, SinglePersistentQueryFailureSyncCallback) {
  CefRefPtr<SinglePersistentQueryTestHandler> handler =
      new SinglePersistentQueryTestHandler(
          SinglePersistentQueryTestHandler::FAILURE, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that a single query with failure result delivered asynchronously.
TEST(MessageRouterTest, SinglePersistentQueryFailureAsyncCallback) {
  CefRefPtr<SinglePersistentQueryTestHandler> handler =
      new SinglePersistentQueryTestHandler(
          SinglePersistentQueryTestHandler::FAILURE, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


namespace {

// Test a single unhandled query in a single page load.
class SingleUnhandledQueryTestHandler : public SingleLoadTestHandler {
 public:
  SingleUnhandledQueryTestHandler() {}

  std::string GetMainHTML() override {
    std::string html;

    html = "<html><body><script>\n"
           // No requests should exist.
           "window.mrtAssertTotalCount(0);\n"
           "window.mrtAssertBrowserCount(0);\n"
           "window.mrtAssertContextCount(0);\n"
           // Keep track of the number of responses.
           "var count = 0;\n"
           // Send the query.
           "var request_id = window.mrtQuery({\n"
           "  request: '" + std::string(kSingleQueryRequest) + "',\n"
           "  persistent: false,\n"
           "  onSuccess: function(response) {\n"
           "    window.mrtNotify('error-onSuccess');\n"
           "  },\n"
           "  onFailure: function(error_code, error_message) {\n"
                // Request should be removed before callback is executed.
           "    window.mrtAssertTotalCount(0);\n"
           "    window.mrtAssertBrowserCount(0);\n"
           "    window.mrtAssertContextCount(0);\n"
           "    if (error_code == -1 && "
                    "error_message == 'The query has been canceled') {\n"
           "      window.mrtNotify('failure');\n"
           "    } else {\n"
           "      window.mrtNotify('error-onFailure');\n"
           "    }\n"
           "  }\n"
           "});\n"
           // Request should exist.
           "window.mrtAssertTotalCount(1);\n"
           "window.mrtAssertBrowserCount(1);\n"
           "window.mrtAssertContextCount(1);\n";

    html += "</script></body></html>";
    return html;
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);
    EXPECT_STREQ("failure", message.c_str());

    got_notify_.yes();

    DestroyTest();
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);
    EXPECT_NE(0, query_id);
    EXPECT_FALSE(persistent);
    EXPECT_STREQ(kSingleQueryRequest, request.ToString().c_str());

    got_on_query_.yes();

    return false;
  }

  void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id) override {
    EXPECT_FALSE(true);  // Not reached.
  }

  void DestroyTest() override {
    EXPECT_TRUE(got_on_query_);
    EXPECT_TRUE(got_notify_);

    TestHandler::DestroyTest();
  }

 private:
  TrackCallback got_on_query_;
  TrackCallback got_notify_;
};

}  // namespace

// Test that a single unhandled query results in a call to onFailure.
TEST(MessageRouterTest, SingleUnhandledQuery) {
  CefRefPtr<SingleUnhandledQueryTestHandler> handler =
      new SingleUnhandledQueryTestHandler();
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


namespace {

const char kMultiQueryRequestId[] = "request_id";
const char kMultiQueryRepeatCt[] = "repeat_ct";
const char kMultiQueryRequest[] = "request";
const char kMultiQueryResponse[] = "response";
const char kMultiQuerySuccess[] = "success";
const char kMultiQueryError[] = "error";
const char kMultiQueryErrorMessage[] = "errormsg";
const int kMultiQueryPersistentResponseCount = 5;

// Generates HTML and verifies results for multiple simultanious queries.
class MultiQueryManager : public CefMessageRouterBrowserSide::Handler {
 public:
  enum TestType {
    // Initiates a non-persistent query with a successful response.
    // OnQuery and OnNotify will be called.
    SUCCESS,

    // Initiates a non-persistent query with a failure response.
    // OnQuery and OnNotify will be called.
    FAILURE,
    
    // Initiates a persistent query with multiple successful responses.
    // OnQuery, OnNotify and OnQueryCanceled will be called.
    PERSISTENT_SUCCESS,
    
    // Initiates a persistent query with multiple successful responses and one
    // failure response.
    // OnQuery and OnNotify will be called.
    PERSISTENT_FAILURE,

    // Initiates a non-persistent query that will be canceled via JavaScript.
    // No JavaScript callbacks will be executed.
    // OnQuery and OnQueryCanceled will be called.
    CANCEL,

    // Initiates a non-persistent query that will not be manually canceled.
    // No JavaScript callbacks will be executed.
    // OnQuery and OnQueryCanceled will be called.
    AUTOCANCEL,

    // Initiates a persistent query with multiple successful responses that will
    // not be manually canceled.
    // OnQuery, OnNotify and OnQueryCanceled will be called.
    PERSISTENT_AUTOCANCEL,
  };

  class Observer {
   public:
    // Called when all manual queries are complete.
    virtual void OnManualQueriesCompleted(MultiQueryManager* manager) {}

    // Called when all queries are complete.
    virtual void OnAllQueriesCompleted(MultiQueryManager* manager) {}
   protected:
    virtual ~Observer() {}
  };

  MultiQueryManager(const std::string& label,
                    bool synchronous,
                    int id_offset = 0)
      : label_(label),
        synchronous_(synchronous),
        id_offset_(id_offset),
        finalized_(false),
        running_(false),
        manual_total_(0),
        received_count_(0),
        manual_complete_count_(0),
        auto_complete_count_(0),
        will_cancel_by_removing_handler_(false),
        weak_ptr_factory_(this) {
  }

  virtual ~MultiQueryManager() {}

  void AddObserver(Observer* observer) {
    EXPECT_FALSE(running_);
    observer_set_.insert(observer);
  }

  void RemoveObserver(Observer* observer) {
    EXPECT_FALSE(running_);
    EXPECT_TRUE(observer_set_.erase(observer));
  }

  // Can be called from any thread, but should always be called from the same
  // thread.
  void AddTestQuery(TestType type) {
    EXPECT_FALSE(finalized_);
    test_query_vector_.push_back(TestQuery(type));
    if (!IsAuto(type))
      manual_total_++;
  }

  // Must be called after AddTestQuery and before the manager is used.
  void Finalize() {
    EXPECT_FALSE(finalized_);
    finalized_ = true;
  }

  // Call after all manual queries have completed if you intend to cancel auto
  // queries by removing the handler.
  void WillCancelByRemovingHandler() {
    EXPECT_TRUE(IsManualComplete());
    will_cancel_by_removing_handler_ = true;
  }

  std::string GetHTML(bool assert_total, bool assert_browser) const {
    EXPECT_TRUE(finalized_);
    EXPECT_FALSE(running_);

    std::string html;

    html = "<html><body>" + label_ + "<script>\n";

    // No requests should exist.
    if (assert_total)
      html += "window.mrtAssertTotalCount(0);\n";
    if (assert_browser)
      html += "window.mrtAssertBrowserCount(0);\n";
    html += "window.mrtAssertContextCount(0);\n";

    if (synchronous_) {
      // Run all of the queries synchronously. None will complete before the
      // last one begins.
      for (size_t i = 0; i < test_query_vector_.size(); ++i) {
        const TestQuery& query = test_query_vector_[i];
        html += GetQueryHTML(static_cast<int>(i), query);
      }

      const int total_ct = static_cast<int>(test_query_vector_.size());

      // Pending requests should match the total created.
      const std::string& total_val = GetIntString(total_ct);
      if (assert_total)
        html += "window.mrtAssertTotalCount(" + total_val + ");\n";
      if (assert_browser)
        html += "window.mrtAssertBrowserCount(" + total_val + ");\n";
      html += "window.mrtAssertContextCount(" + total_val + ");\n";

      int cancel_ct = 0;

      // Cancel all of the queries with type CANCEL.
      for (size_t i = 0; i < test_query_vector_.size(); ++i) {
        const TestQuery& query = test_query_vector_[i];
        if (query.type == CANCEL) {
          html += GetCancelHTML(static_cast<int>(i), query);
          cancel_ct++;
        }
      }

      if (cancel_ct > 0) {
        // Pending requests should match the total not canceled.
        const std::string& cancel_val = GetIntString(total_ct - cancel_ct);
        if (assert_total)
          html += "window.mrtAssertTotalCount(" + cancel_val + ");\n";
        if (assert_browser)
          html += "window.mrtAssertBrowserCount(" + cancel_val + ");\n";
        html += "window.mrtAssertContextCount(" + cancel_val + ");\n";
      }
    } else {
      // Run all of the queries asynchronously. Some may complete before
      // others begin.
      for (size_t i = 0; i < test_query_vector_.size(); ++i) {
        const TestQuery& query = test_query_vector_[i];

        const int index = static_cast<int>(i);

        // Each request is delayed by 10ms from the previous request.
        const std::string& delay_val = GetIntString(index);
        const std::string& query_html = GetQueryHTML(index, query);

        html += "window.setTimeout(function() {\n" + query_html;

        if (query.type == CANCEL) {
          // Cancel the query asynchronously with a 10ms delay.
          const std::string& request_id_var =
              GetIDString(kMultiQueryRequestId, index);
          html += "  window.setTimeout(function() {\n"
                  "    window.mrtQueryCancel(" + request_id_var + ");\n"
                  "  }, 1);\n";
        }

        html += "\n}, " + delay_val + ");\n";
      }
    }

    html += "</script></body></html>";

    return html;
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) {
    EXPECT_TRUE(finalized_);
    EXPECT_UI_THREAD();

    if (!running_)
      running_ = true;

    EXPECT_TRUE(browser.get());
    EXPECT_TRUE(frame.get());

    std::string value;
    int index = 0;
    EXPECT_TRUE(SplitIDString(message, &value, &index));

    TestQuery& query = test_query_vector_[index];

    // Verify that browser and frame are the same.
    EXPECT_EQ(query.browser_id, browser->GetIdentifier()) << index;
    EXPECT_EQ(query.frame_id, frame->GetIdentifier()) << index;

    // Verify a successful/expected result.
    if (will_cancel_by_removing_handler_) {
      // Auto queries receive an onFailure callback which will notify with error
      // when the handler is removed.
      EXPECT_STREQ(kMultiQueryError, value.c_str()) << index;
      EXPECT_TRUE(IsAuto(query.type)) << index;
      EXPECT_TRUE(query.got_query) << index;
      if (query.type == PERSISTENT_AUTOCANCEL)
        EXPECT_TRUE(query.got_success) << index;
      else
        EXPECT_FALSE(query.got_success) << index;

      query.got_error.yes();

      // There's a race between OnQueryCanceled and OnNotification. Only call
      // OnQueryCompleted a single time.
      if (query.got_query_canceled)
        OnQueryCompleted(query.type);
    } else {
      EXPECT_STREQ(kMultiQuerySuccess, value.c_str()) << index;
      EXPECT_TRUE(WillNotify(query.type)) << index;
      EXPECT_TRUE(query.got_query) << index;
      EXPECT_FALSE(query.got_query_canceled) << index;
      EXPECT_FALSE(query.got_success) << index;
    
      query.got_success.yes();

      // PERSISTENT_AUTOCANCEL doesn't call OnReceiveCompleted from OnQuery.
      if (query.type == PERSISTENT_AUTOCANCEL)
        OnReceiveCompleted(query.type);

      // Call OnQueryCompleted for types that don't get OnQueryCanceled.
      if (!WillCancel(query.type))
        OnQueryCompleted(query.type);
    }
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    EXPECT_TRUE(finalized_);
    EXPECT_UI_THREAD();

    if (!running_)
      running_ = true;

    EXPECT_TRUE(browser.get());
    EXPECT_TRUE(frame.get());
    EXPECT_NE(0, query_id);

    std::string value;
    int index = 0;
    EXPECT_TRUE(SplitIDString(request, &value, &index));

    TestQuery& query = test_query_vector_[index];

    if (IsPersistent(query.type))
      EXPECT_TRUE(persistent);
    else
      EXPECT_FALSE(persistent);

    // Verify expected request.
    EXPECT_STREQ(kMultiQueryRequest, value.c_str()) << index;

    // Verify that call order is correct.
    EXPECT_FALSE(query.got_query) << index;
    EXPECT_FALSE(query.got_query_canceled) << index;
    EXPECT_FALSE(query.got_success) << index;
    EXPECT_FALSE(query.got_error) << index;

    query.got_query.yes();

    query.browser_id = browser->GetIdentifier();
    query.frame_id = frame->GetIdentifier();

    if (query.type == SUCCESS) {
      // Send the single success response.
      callback->Success(GetIDString(kMultiQueryResponse, index));
    } else if (IsPersistent(query.type)) {
      // Send the required number of successful responses.
      const std::string& response = GetIDString(kMultiQueryResponse, index);
      for (int i = 0; i < kMultiQueryPersistentResponseCount; ++i)
        callback->Success(response);
    }

    if (WillFail(query.type)) {
      // Send the single failure response.
      callback->Failure(index, GetIDString(kMultiQueryErrorMessage, index));
    }
    
    if (WillCancel(query.type)) {
      // Hold onto the callback until the query is canceled.
      query.query_id = query_id;
      query.callback = callback;
    }

    // PERSISTENT_AUTOCANCEL will call OnReceiveCompleted once the success
    // notification is received.
    if (query.type != PERSISTENT_AUTOCANCEL)
      OnReceiveCompleted(query.type);

    return true;
  }

  void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id) override {
    EXPECT_TRUE(finalized_);
    EXPECT_UI_THREAD();

    if (!running_)
      running_ = true;

    EXPECT_TRUE(browser.get());
    EXPECT_TRUE(frame.get());
    EXPECT_NE(0, query_id);

    bool found = false;
    for (size_t i = 0; i < test_query_vector_.size(); ++i) {
      TestQuery& query = test_query_vector_[i];
      if (query.query_id == query_id) {
        // Verify that browser and frame are the same.
        EXPECT_EQ(query.browser_id, browser->GetIdentifier()) << i;
        EXPECT_EQ(query.frame_id, frame->GetIdentifier()) << i;

        // Verify a successful/expected result.
        EXPECT_TRUE(WillCancel(query.type)) << i;
        EXPECT_TRUE(query.callback.get()) << i;

        // Release the callback.
        query.callback = NULL;

        // Verify that call order is correct.
        EXPECT_TRUE(query.got_query) << i;

        if (query.type == CANCEL || query.type == AUTOCANCEL) {
          // No JavaScript onSuccess callback executes.
          EXPECT_FALSE(query.got_success) << i;
        } else {
          // JavaScript onSuccess does execute before cancellation.
          EXPECT_TRUE(query.got_success) << i;
        }

        query.got_query_canceled.yes();

        if (will_cancel_by_removing_handler_) {
          // There's a race between OnQueryCanceled and OnNotification. Only
          // call OnQueryCompleted a single time.
          if (query.got_error)
            OnQueryCompleted(query.type);
        } else {
          EXPECT_FALSE(query.got_error) << i;

          // Cancellation is always completion.
          OnQueryCompleted(query.type);
        }

        found = true;
        break;
      }
    }
    EXPECT_TRUE(found);
  }

  // Asserts that all queries have completed.
  void AssertAllComplete() const {
    EXPECT_TRUE(finalized_);
    EXPECT_FALSE(running_);
    EXPECT_UI_THREAD();

    for (size_t i = 0; i < test_query_vector_.size(); ++i) {
      const TestQuery& query = test_query_vector_[i];
      EXPECT_TRUE(query.got_query) << i;

      if (WillCancel(query.type))
        EXPECT_TRUE(query.got_query_canceled) << i;
      else
        EXPECT_FALSE(query.got_query_canceled) << i;

      if (WillNotify(query.type))
        EXPECT_TRUE(query.got_success) << i;
      else
        EXPECT_FALSE(query.got_success) << i;

      if (IsAuto(query.type) && will_cancel_by_removing_handler_)
        EXPECT_TRUE(query.got_error);
      else
        EXPECT_FALSE(query.got_error);

      EXPECT_FALSE(query.callback.get()) << i;
    }
  }

  // Returns true if all manual queries have completed.
  bool IsManualComplete() const {
    EXPECT_TRUE(finalized_);
    EXPECT_UI_THREAD();

    return (manual_complete_count_ == manual_total_);
  }

  // Returns true if all queries have completed.
  bool IsAllComplete() const {
    EXPECT_TRUE(finalized_);
    EXPECT_UI_THREAD();

    return (manual_complete_count_ + auto_complete_count_ ==
            static_cast<int>(test_query_vector_.size()));
  }

  bool HasAutoQueries() const {
    return (manual_total_ != static_cast<int>(test_query_vector_.size()));
  }

 private:
  struct TestQuery {
    explicit TestQuery(TestType test_type)
      : type(test_type),
        browser_id(0),
        frame_id(0),
        query_id(0) {}

    TestType type;

    // Set in OnQuery and verified in OnNotify or OnQueryCanceled.
    int browser_id;
    int64 frame_id;

    // Used when a query is canceled.
    int64 query_id;
    CefRefPtr<Callback> callback;
    
    TrackCallback got_query;
    TrackCallback got_query_canceled;
    TrackCallback got_success;
    TrackCallback got_error;
  };

  class NotifyTask : public CefTask {
   public:
    NotifyTask(base::WeakPtr<MultiQueryManager> weak_ptr,
               bool notify_all)
        : weak_ptr_(weak_ptr),
          notify_all_(notify_all) {}

    void Execute() override {
      if (weak_ptr_) {
        if (notify_all_)
          weak_ptr_->NotifyAllQueriesCompleted();
        else
          weak_ptr_->NotifyManualQueriesCompleted();
      }
    }

   private:
    base::WeakPtr<MultiQueryManager> weak_ptr_;
    const bool notify_all_;

    IMPLEMENT_REFCOUNTING(NotifyTask);
  };

  static bool IsAuto(TestType type) {
    return (type == AUTOCANCEL ||
            type == PERSISTENT_AUTOCANCEL);
  }

  static bool IsPersistent(TestType type) {
    return (type == PERSISTENT_SUCCESS ||
            type == PERSISTENT_FAILURE ||
            type == PERSISTENT_AUTOCANCEL);
  }

  static bool WillFail(TestType type) {
    return (type == FAILURE ||
            type == PERSISTENT_FAILURE);
  }

  static bool WillCancel(TestType type) {
    return (type == PERSISTENT_SUCCESS ||
            type == CANCEL ||
            type == AUTOCANCEL ||
            type == PERSISTENT_AUTOCANCEL);
  }

  static bool WillNotify(TestType type) {
    return (type == SUCCESS ||
            type == PERSISTENT_SUCCESS ||
            type == FAILURE ||
            type == PERSISTENT_FAILURE ||
            type == PERSISTENT_AUTOCANCEL);
  }

  void OnReceiveCompleted(TestType type) {
    const int total_count = static_cast<int>(test_query_vector_.size());
    if (++received_count_ == total_count && manual_total_ == 0) {
      // There aren't any manual queries so notify here.
      CefPostTask(TID_UI,
          new NotifyTask(weak_ptr_factory_.GetWeakPtr(), false));
    }
  }

  void OnQueryCompleted(TestType type) {
    const int total_count = static_cast<int>(test_query_vector_.size());
    EXPECT_LT(manual_complete_count_ + auto_complete_count_, total_count);
    EXPECT_LE(manual_complete_count_, manual_total_);

    const bool is_auto = IsAuto(type);
    if (is_auto)
      auto_complete_count_++;
    else if (++manual_complete_count_ == manual_total_) {
      CefPostTask(TID_UI,
          new NotifyTask(weak_ptr_factory_.GetWeakPtr(), false));
    }

    if (auto_complete_count_ + manual_complete_count_ == total_count) {
      running_ = false;
      CefPostTask(TID_UI,
          new NotifyTask(weak_ptr_factory_.GetWeakPtr(), true));
    }
  }

  void NotifyManualQueriesCompleted() {
    if (observer_set_.empty())
      return;
    
    // Use a copy of the set in case an Observer is removed while we're
    // iterating.
    ObserverSet observer_set = observer_set_;

    ObserverSet::const_iterator it = observer_set.begin();
    for (; it != observer_set.end(); ++it) {
      (*it)->OnManualQueriesCompleted(this);
    }
  }

  void NotifyAllQueriesCompleted() {
    if (observer_set_.empty())
      return;
    
    // Use a copy of the set in case an Observer is removed while we're
    // iterating.
    ObserverSet observer_set = observer_set_;

    ObserverSet::const_iterator it = observer_set.begin();
    for (; it != observer_set.end(); ++it) {
      (*it)->OnAllQueriesCompleted(this);
    }
  }

  std::string GetQueryHTML(const int index, const TestQuery& query) const {
    const std::string& request_id_var =
        GetIDString(kMultiQueryRequestId, index);
    const std::string& repeat_ct_var = GetIDString(kMultiQueryRepeatCt, index);
    const std::string& request_val =
        GetIDString(std::string(kMultiQueryRequest) + ":", index);
    const std::string& success_val =
        GetIDString(std::string(kMultiQuerySuccess) + ":", index);
    const std::string& error_val =
        GetIDString(std::string(kMultiQueryError) + ":", index);

    std::string html;

    const bool persistent = IsPersistent(query.type);

    if (persistent)
      html += "var " + repeat_ct_var + " = 0;\n";
    
    html += "var " + request_id_var + " = window.mrtQuery({\n"
            "  request: '" + request_val + "',\n"
            "  persistent: " + (persistent ? "true" : "false") + ",\n";

    if (query.type == SUCCESS) {
      const std::string& response_val =
          GetIDString(kMultiQueryResponse, index);
      
      html += "  onSuccess: function(response) {\n"
              "    if (response == '" + response_val + "')\n"
              "      window.mrtNotify('" + success_val + "');\n"
              "    else\n"
              "      window.mrtNotify('" + error_val + "');\n"
              "  },\n"
              "  onFailure: function(error_code, error_message) {\n"
              "    window.mrtNotify('" + error_val + "');\n"
              "  }\n";
    } else if (query.type == FAILURE) {
      const std::string& error_code_val = GetIntString(index);
      const std::string& error_message_val =
          GetIDString(kMultiQueryErrorMessage, index);

      html += "  onSuccess: function(response) {\n"
              "    window.mrtNotify('" + error_val + "');\n"
              "  },\n"
              "  onFailure: function(error_code, error_message) {\n"
              "    if (error_code == " + error_code_val +
                      " && error_message == '" + error_message_val + "')\n"
              "      window.mrtNotify('" + success_val + "');\n"
              "    else\n"
              "      window.mrtNotify('" + error_val + "');\n"
              "  }\n";
    } else if (query.type == PERSISTENT_SUCCESS ||
               query.type == PERSISTENT_AUTOCANCEL) {
      const std::string& response_val =
          GetIDString(kMultiQueryResponse, index);
      const std::string& repeat_ct =
          GetIntString(kMultiQueryPersistentResponseCount);
      
      html += "  onSuccess: function(response) {\n"
              "    if (response == '" + response_val + "') {\n"
                     // Should get repeat_ct number of successful responses.
              "      if (++" + repeat_ct_var + " == " + repeat_ct + ") {\n"
              "        window.mrtNotify('" + success_val + "');\n";

      if (query.type == PERSISTENT_SUCCESS) {
        // Manually cancel the request.
        html += "        window.mrtQueryCancel(" + request_id_var + ");\n";
      }

      html += "      }\n"
              "    } else {\n"
              "      window.mrtNotify('" + error_val + "');\n"
              "    }\n"
              "  },\n"
              "  onFailure: function(error_code, error_message) {\n"
              "    window.mrtNotify('" + error_val + "');\n"
              "  }\n";
    } else if (query.type == PERSISTENT_FAILURE) {
      const std::string& error_code_val = GetIntString(index);
      const std::string& error_message_val =
          GetIDString(kMultiQueryErrorMessage, index);
      const std::string& repeat_ct =
          GetIntString(kMultiQueryPersistentResponseCount);

      html += "  onSuccess: function(response) {\n"
                   // Should get some successful responses before failure.
              "    if (++" + repeat_ct_var + " > " + repeat_ct + ") {\n"
              "      window.mrtNotify('" + error_val + "');\n"
              "    }\n"
              "  },\n"
              "  onFailure: function(error_code, error_message) {\n"
              "    if (error_code == " + error_code_val +
                      " && error_message == '" + error_message_val + "'"
                      " && " + repeat_ct_var + " == " + repeat_ct + ")\n"
              "      window.mrtNotify('" + success_val + "');\n"
              "    else\n"
              "      window.mrtNotify('" + error_val + "');\n"
              "  }\n";
    } else if (query.type == CANCEL || query.type == AUTOCANCEL) {
      html += "  onSuccess: function(response) {\n"
              "    window.mrtNotify('" + error_val + "');\n"
              "  },\n"
              "  onFailure: function(error_code, error_message) {\n"
              "    window.mrtNotify('" + error_val + "');\n"
              "  }\n";
    }

    html += "});\n";

    return html;
  }

  std::string GetCancelHTML(const int index, const TestQuery& query) const {
    const std::string& request_id_var = GetIDString(kMultiQueryRequestId, index);
    return "window.mrtQueryCancel(" + request_id_var + ");\n";
  }

  std::string GetIDString(const std::string& prefix, int index) const {
    EXPECT_TRUE(!prefix.empty());
    return base::StringPrintf("%s%d", prefix.c_str(), GetIDFromIndex(index));
  }

  bool SplitIDString(const std::string& str,
                     std::string* value, int* index) const {
    size_t pos = str.find(':');
    if (pos != std::string::npos) {
      *value = str.substr(0, pos);
      *index = GetIndexFromID(atoi(str.substr(pos+1).c_str()));
      return (*index >= 0 &&
              *index < static_cast<int>(test_query_vector_.size()));
    }

    return false;
  }

  std::string GetIntString(int val) const {
    return base::StringPrintf("%d", val);
  }

  int GetIDFromIndex(int index) const { return id_offset_ + index; }
  int GetIndexFromID(int id) const { return id - id_offset_; }

  const std::string label_;
  const bool synchronous_;
  const int id_offset_;

  typedef std::vector<TestQuery> TestQueryVector;
  TestQueryVector test_query_vector_;

  typedef std::set<Observer*> ObserverSet;
  ObserverSet observer_set_;

  // Set to true after all queries have been added.
  bool finalized_;
  // Set to true while queries are pending.
  bool running_;

  // Total number of queries that will manually complete.
  int manual_total_;

  // Number of queries that have been received.
  int received_count_;

  // Number of queries that have completed successfully.
  int manual_complete_count_;
  int auto_complete_count_;

  // If true any pending queries will receive an onFailure callback in addition
  // to be canceled.
  bool will_cancel_by_removing_handler_;

  // Should always be the last member.
  base::WeakPtrFactory<MultiQueryManager> weak_ptr_factory_;
};

void MakeTestQueries(MultiQueryManager* manager, bool some,
                     int many_count = 200) {
  if (some) {
    // Test some queries of arbitrary types.
    // Use a hard-coded list so the behavior is deterministic across test runs.
    MultiQueryManager::TestType types[] = {
        MultiQueryManager::PERSISTENT_AUTOCANCEL,
        MultiQueryManager::SUCCESS,
        MultiQueryManager::AUTOCANCEL,
        MultiQueryManager::PERSISTENT_FAILURE,
        MultiQueryManager::CANCEL,
        MultiQueryManager::FAILURE,
        MultiQueryManager::AUTOCANCEL,
        MultiQueryManager::SUCCESS,
        MultiQueryManager::PERSISTENT_SUCCESS,
        MultiQueryManager::SUCCESS,
        MultiQueryManager::PERSISTENT_AUTOCANCEL,
        MultiQueryManager::CANCEL,
        MultiQueryManager::PERSISTENT_SUCCESS,
        MultiQueryManager::FAILURE,
    };
    for (size_t i = 0; i < sizeof(types) / sizeof(types[0]); ++i) {
      manager->AddTestQuery(types[i]);
    }
  } else {
    // Test every type of query.
    for (int i = 0; i < many_count; ++i) {
      MultiQueryManager::TestType type = MultiQueryManager::SUCCESS;
      switch (i % 7) {
        case 0:
          type = MultiQueryManager::SUCCESS;
          break;
        case 1:
          type = MultiQueryManager::FAILURE;
          break;
        case 2:
          type = MultiQueryManager::PERSISTENT_SUCCESS;
          break;
        case 3:
          type = MultiQueryManager::PERSISTENT_FAILURE;
          break;
        case 4:
          type = MultiQueryManager::CANCEL;
          break;
        case 5:
          type = MultiQueryManager::AUTOCANCEL;
          break;
        case 6:
          type = MultiQueryManager::PERSISTENT_AUTOCANCEL;
          break;
      }
      manager->AddTestQuery(type);
    }
  }
  manager->Finalize();
}

// Test multiple queries in a single page load with a single frame.
class MultiQuerySingleFrameTestHandler :
    public SingleLoadTestHandler,
    public MultiQueryManager::Observer {
 public:
  enum CancelType {
    CANCEL_BY_NAVIGATION,
    CANCEL_BY_REMOVING_HANDLER,
    CANCEL_BY_CLOSING_BROWSER,
  };

  MultiQuerySingleFrameTestHandler(
      bool synchronous,
      CancelType cancel_type = CANCEL_BY_NAVIGATION)
      : manager_(std::string(), synchronous),
        cancel_type_(cancel_type) {
    manager_.AddObserver(this);
  }

  std::string GetMainHTML() override {
    return manager_.GetHTML(true, true);
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);

    manager_.OnNotify(browser, frame, message);
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);

    return manager_.OnQuery(browser, frame, query_id, request, persistent,
                            callback);
  }

  void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);

    manager_.OnQueryCanceled(browser, frame, query_id);
  }

  void OnManualQueriesCompleted(MultiQueryManager* manager) override {
    EXPECT_EQ(manager, &manager_);
    if (manager_.HasAutoQueries()) {
      if (cancel_type_ == CANCEL_BY_NAVIGATION) {
        // Navigate somewhere else to terminate the auto queries.
        GetBrowser()->GetMainFrame()->LoadURL(
            std::string(kTestDomain1) + "cancel.html");
      } else if (cancel_type_ == CANCEL_BY_REMOVING_HANDLER) {
        // Change the expected behavior in the manager.
        manager_.WillCancelByRemovingHandler();
        GetRouter()->RemoveHandler(this);
        // All queries should be immediately canceled.
        AssertQueryCount(NULL, NULL, 0);
      } else if (cancel_type_ == CANCEL_BY_CLOSING_BROWSER) {
        // Change the expected behavior in the handler.
        SetSignalCompletionWhenAllBrowsersClose(false);
        GetBrowser()->GetHost()->CloseBrowser(false);
      }
    }
  }

  void OnAllQueriesCompleted(MultiQueryManager* manager) override {
    EXPECT_EQ(manager, &manager_);

    // All queries should be canceled.
    AssertQueryCount(NULL, NULL, 0);

    DestroyTest();

    if (!SignalCompletionWhenAllBrowsersClose()) {
      // Complete asynchronously so the call stack has a chance to unwind.
      CefPostTask(TID_UI,
          base::Bind(&MultiQuerySingleFrameTestHandler::TestComplete, this));
    }
  }

  void DestroyTest() override {
    manager_.AssertAllComplete();
    TestHandler::DestroyTest();
  }

  MultiQueryManager* GetManager() { return &manager_; }

 private:
  MultiQueryManager manager_;
  const CancelType cancel_type_;
};

}  // namespace

#define MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(name, type, synchronous) \
  TEST(MessageRouterTest, name) { \
    CefRefPtr<MultiQuerySingleFrameTestHandler> handler = \
        new MultiQuerySingleFrameTestHandler(synchronous); \
    MultiQueryManager* manager = handler->GetManager(); \
    manager->AddTestQuery(MultiQueryManager::type); \
    manager->Finalize(); \
    handler->ExecuteTest(); \
    ReleaseAndWaitForDestructor(handler); \
  }

// Test the query types individually.
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameSyncSuccess, SUCCESS, true);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameAsyncSuccess, SUCCESS, false);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameSyncFailure, FAILURE, true);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameAsyncFailure, FAILURE, false);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameSyncPersistentSuccess, PERSISTENT_SUCCESS, true);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameAsyncPersistentSuccess, PERSISTENT_SUCCESS, false);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameSyncPersistentFailure, PERSISTENT_FAILURE, true);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameAsyncPersistentFailure, PERSISTENT_FAILURE, false);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameCancel, CANCEL, true);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFrameAutoCancel, AUTOCANCEL, true);
MULTI_QUERY_SINGLE_FRAME_TYPE_TEST(
    MultiQuerySingleFramePersistentAutoCancel, PERSISTENT_AUTOCANCEL, true);

// Test that one frame can run some queries successfully in a synchronous
// manner.
TEST(MessageRouterTest, MultiQuerySingleFrameSyncSome) {
  CefRefPtr<MultiQuerySingleFrameTestHandler> handler =
      new MultiQuerySingleFrameTestHandler(true);
  MakeTestQueries(handler->GetManager(), true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that one frame can run some queries successfully in an asynchronous
// manner.
TEST(MessageRouterTest, MultiQuerySingleFrameAsyncSome) {
  CefRefPtr<MultiQuerySingleFrameTestHandler> handler =
      new MultiQuerySingleFrameTestHandler(false);
  MakeTestQueries(handler->GetManager(), true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that one frame can run many queries successfully in a synchronous
// manner.
TEST(MessageRouterTest, MultiQuerySingleFrameSyncMany) {
  CefRefPtr<MultiQuerySingleFrameTestHandler> handler =
      new MultiQuerySingleFrameTestHandler(true);
  MakeTestQueries(handler->GetManager(), false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that one frame can run many queries successfully in an asynchronous
// manner.
TEST(MessageRouterTest, MultiQuerySingleFrameAsyncMany) {
  CefRefPtr<MultiQuerySingleFrameTestHandler> handler =
      new MultiQuerySingleFrameTestHandler(false);
  MakeTestQueries(handler->GetManager(), false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that pending queries can be canceled by removing the handler.
TEST(MessageRouterTest, MultiQuerySingleFrameCancelByRemovingHandler) {
  CefRefPtr<MultiQuerySingleFrameTestHandler> handler =
      new MultiQuerySingleFrameTestHandler(false,
          MultiQuerySingleFrameTestHandler::CANCEL_BY_REMOVING_HANDLER);
  MakeTestQueries(handler->GetManager(), false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that pending queries can be canceled by closing the browser.
TEST(MessageRouterTest, MultiQuerySingleFrameCancelByClosingBrowser) {
  CefRefPtr<MultiQuerySingleFrameTestHandler> handler =
      new MultiQuerySingleFrameTestHandler(false,
          MultiQuerySingleFrameTestHandler::CANCEL_BY_CLOSING_BROWSER);
  MakeTestQueries(handler->GetManager(), false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


namespace {

// Test multiple handlers.
class MultiQueryMultiHandlerTestHandler :
    public SingleLoadTestHandler,
    public MultiQueryManager::Observer {
 public:
  class Handler : public CefMessageRouterBrowserSide::Handler {
   public:
    Handler(MultiQueryMultiHandlerTestHandler* test_handler,
            int index)
        : test_handler_(test_handler),
          index_(index),
          query_id_(0) {
    }

    bool OnQuery(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int64 query_id,
                 const CefString& request,
                 bool persistent,
                 CefRefPtr<Callback> callback) override {
      // Each handler only handles a single request.
      const std::string& handled_request =
        base::StringPrintf("%s:%d", kMultiQueryRequest, index_);
      if (request != handled_request)
        return false;

      // Verify that handlers are called in the correct order.
      if (index_ == 0) {
        EXPECT_FALSE(test_handler_->got_query0_);
        EXPECT_FALSE(test_handler_->got_query1_);
        EXPECT_FALSE(test_handler_->got_query2_);

        test_handler_->got_query0_.yes();
      } else if (index_ == 1) {
        EXPECT_TRUE(test_handler_->got_query0_);
        EXPECT_FALSE(test_handler_->got_query1_);
        EXPECT_FALSE(test_handler_->got_query2_);

        test_handler_->got_query1_.yes();
      } else if (index_ == 2) {
        EXPECT_TRUE(test_handler_->got_query0_);
        EXPECT_TRUE(test_handler_->got_query1_);
        EXPECT_FALSE(test_handler_->got_query2_);

        test_handler_->got_query2_.yes();
      }

      query_id_ = query_id;
      return test_handler_->OnQuery(browser, frame, query_id, request,
                                    persistent, callback);
    }

    void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int64 query_id) override {
      // Verify that the correct handler is called for cancellation.
      EXPECT_EQ(query_id_, query_id);

      if (index_ == 0) {
        EXPECT_FALSE(test_handler_->got_query_canceled0_);
        test_handler_->got_query_canceled0_.yes();
      } else if (index_ == 1) {
        EXPECT_FALSE(test_handler_->got_query_canceled1_);
        test_handler_->got_query_canceled1_.yes();
      } else if (index_ == 2) {
        EXPECT_FALSE(test_handler_->got_query_canceled2_);
        test_handler_->got_query_canceled2_.yes();
      }

      test_handler_->OnQueryCanceled(browser, frame, query_id);
    }

   private:
    MultiQueryMultiHandlerTestHandler* test_handler_;
    const int index_;
    int query_id_;
  };

  MultiQueryMultiHandlerTestHandler(bool synchronous,
                                    bool cancel_by_removing_handler)
     : manager_(std::string(), synchronous, 0),
       handler0_(this, 0),
       handler1_(this, 1),
       handler2_(this, 2),
       cancel_by_removing_handler_(cancel_by_removing_handler) {
    manager_.AddObserver(this);

    // Each handler will handle one of the queries.
    manager_.AddTestQuery(MultiQueryManager::PERSISTENT_AUTOCANCEL);
    manager_.AddTestQuery(MultiQueryManager::PERSISTENT_AUTOCANCEL);
    manager_.AddTestQuery(MultiQueryManager::PERSISTENT_AUTOCANCEL);
    manager_.Finalize();
  }

  std::string GetMainHTML() override {
    return manager_.GetHTML(true, true);
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);

    manager_.OnNotify(browser, frame, message);
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);

    return manager_.OnQuery(browser, frame, query_id, request, persistent,
                            callback);
  }

  void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id) override {
    AssertMainBrowser(browser);
    AssertMainFrame(frame);

    manager_.OnQueryCanceled(browser, frame, query_id);
  }

  void OnManualQueriesCompleted(MultiQueryManager* manager) override {
    EXPECT_EQ(manager, &manager_);

    EXPECT_TRUE(got_query0_);
    EXPECT_TRUE(got_query1_);
    EXPECT_TRUE(got_query2_);
    EXPECT_FALSE(got_query_canceled0_);
    EXPECT_FALSE(got_query_canceled1_);
    EXPECT_FALSE(got_query_canceled2_);

    EXPECT_TRUE(manager_.HasAutoQueries());

    CefRefPtr<CefMessageRouterBrowserSide> router = GetRouter();
    
    // Remove one handler to cancel a query.

    if (cancel_by_removing_handler_) {
      manager_.WillCancelByRemovingHandler();

      // Each query should be canceled as the handler is removed.
      EXPECT_TRUE(router->RemoveHandler(&handler1_));
      EXPECT_FALSE(got_query_canceled0_);
      EXPECT_TRUE(got_query_canceled1_);
      EXPECT_FALSE(got_query_canceled2_);

      EXPECT_TRUE(router->RemoveHandler(&handler2_));
      EXPECT_FALSE(got_query_canceled0_);
      EXPECT_TRUE(got_query_canceled2_);

      EXPECT_TRUE(router->RemoveHandler(&handler0_));
      EXPECT_TRUE(got_query_canceled0_);
    } else {
      GetBrowser()->GetMainFrame()->LoadURL(
          std::string(kTestDomain1) + "cancel.html");
    }
  }

  void OnAllQueriesCompleted(MultiQueryManager* manager) override {
    EXPECT_EQ(manager, &manager_);

    // All queries should be canceled.
    AssertQueryCount(NULL, NULL, 0);

    DestroyTest();
  }

  void DestroyTest() override {
    EXPECT_TRUE(got_query0_);
    EXPECT_TRUE(got_query1_);
    EXPECT_TRUE(got_query2_);
    EXPECT_TRUE(got_query_canceled0_);
    EXPECT_TRUE(got_query_canceled1_);
    EXPECT_TRUE(got_query_canceled2_);

    manager_.AssertAllComplete();
    TestHandler::DestroyTest();
  }

 protected:
  void AddHandlers(
      CefRefPtr<CefMessageRouterBrowserSide> message_router) override {
    // OnQuery call order will verify that the first/last ordering works as
    // expected.
    EXPECT_TRUE(message_router->AddHandler(&handler1_, true));
    EXPECT_TRUE(message_router->AddHandler(&handler0_, true));
    EXPECT_TRUE(message_router->AddHandler(&handler2_, false));

    // Can't add the same handler multiple times.
    EXPECT_FALSE(message_router->AddHandler(&handler1_, true));
  }

 private:
  MultiQueryManager manager_;
  Handler handler0_;
  Handler handler1_;
  Handler handler2_;

  const bool cancel_by_removing_handler_;

  TrackCallback got_query0_;
  TrackCallback got_query1_;
  TrackCallback got_query2_;

  TrackCallback got_query_canceled0_;
  TrackCallback got_query_canceled1_;
  TrackCallback got_query_canceled2_;
};

}  // namespace

// Test that multiple handlers behave correctly.
TEST(MessageRouterTest, MultiQueryMultiHandler) {
  CefRefPtr<MultiQueryMultiHandlerTestHandler> handler =
      new MultiQueryMultiHandlerTestHandler(false, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple handlers behave correctly. Cancel by removing the
// handlers.
TEST(MessageRouterTest, MultiQueryMultiHandlerCancelByRemovingHandler) {
  CefRefPtr<MultiQueryMultiHandlerTestHandler> handler =
      new MultiQueryMultiHandlerTestHandler(false, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


namespace {

// Map of managers on a per-URL basis.
class MultiQueryManagerMap :
    public CefMessageRouterBrowserSide::Handler,
    public MultiQueryManager::Observer {
 public:
  class Observer {
   public:
    // Called when all manual queries are complete.
    virtual void OnMapManualQueriesCompleted(MultiQueryManagerMap* map) {}

    // Called when all queries are complete.
    virtual void OnMapAllQueriesCompleted(MultiQueryManagerMap* map) {}
   protected:
    virtual ~Observer() {}
  };

  MultiQueryManagerMap()
      : finalized_(false),
        running_(false),
        manual_complete_count_(0),
        total_complete_count_(0) {
  }

  virtual ~MultiQueryManagerMap() {
    RemoveAllManagers();
  }

  void AddObserver(Observer* observer) {
    EXPECT_FALSE(running_);
    observer_set_.insert(observer);
  }

  void RemoveObserver(Observer* observer) {
    EXPECT_FALSE(running_);
    EXPECT_TRUE(observer_set_.erase(observer));
  }

  MultiQueryManager* CreateManager(const std::string& url, bool synchronous) {
    EXPECT_FALSE(finalized_);

    // The sub-frame resource should not already exist.
    URLManagerMap::const_iterator it = manager_map_.find(url);
    EXPECT_EQ(it, manager_map_.end());

    MultiQueryManager* manager =
        new MultiQueryManager(url, synchronous,
            static_cast<int>(manager_map_.size()) * 1000);
    manager->AddObserver(this);
    manager_map_.insert(std::make_pair(url, manager));

    return manager;
  }

  void Finalize() {
    EXPECT_FALSE(finalized_);
    finalized_ = true;
  }

  std::string GetMainHTML() const {
    EXPECT_TRUE(finalized_);
    EXPECT_FALSE(running_);

    std::string html = "<html><body>\n";

    URLManagerMap::const_iterator it = manager_map_.begin();
    for (; it != manager_map_.end(); ++it) {
      const std::string& name = GetNameForURL(it->first);
      html += "<iframe id=\"" + name + "\" src=\"" + it->first +
              "\"></iframe>\n";
    }

    html += "</body></html>";
    return html;
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) {
    EXPECT_TRUE(finalized_);
    if (!running_)
      running_ = true;

    MultiQueryManager* manager = GetManager(browser, frame);
    manager->OnNotify(browser, frame, message);
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    EXPECT_TRUE(finalized_);
    if (!running_)
      running_ = true;

    MultiQueryManager* manager = GetManager(browser, frame);
    return manager->OnQuery(browser, frame, query_id, request, persistent,
                            callback);
  }

  void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id) override {
    EXPECT_TRUE(finalized_);
    if (!running_)
      running_ = true;

    MultiQueryManager* manager = GetManager(browser, frame);
    manager->OnQueryCanceled(browser, frame, query_id);
  }

  void OnManualQueriesCompleted(MultiQueryManager* manager) override {
    const int size = static_cast<int>(manager_map_.size());
    EXPECT_LT(manual_complete_count_, size);
    if (++manual_complete_count_ == size) {
      running_ = false;

      // Notify observers.
      if (!observer_set_.empty()) {
        // Use a copy of the set in case an Observer is removed while we're
        // iterating.
        ObserverSet observer_set = observer_set_;

        ObserverSet::const_iterator it = observer_set.begin();
        for (; it != observer_set.end(); ++it) {
          (*it)->OnMapManualQueriesCompleted(this);
        }
      }
    }
  }

  void OnAllQueriesCompleted(MultiQueryManager* manager) override {
    const int size = static_cast<int>(manager_map_.size());
    EXPECT_LT(total_complete_count_, size);
    if (++total_complete_count_ == size) {
      running_ = false;

      // Notify observers.
      if (!observer_set_.empty()) {
        // Use a copy of the set in case an Observer is removed while we're
        // iterating.
        ObserverSet observer_set = observer_set_;

        ObserverSet::const_iterator it = observer_set.begin();
        for (; it != observer_set.end(); ++it) {
          (*it)->OnMapAllQueriesCompleted(this);
        }
      }
    }
  }

  bool AllComplete() const {
    EXPECT_TRUE(finalized_);

    URLManagerMap::const_iterator it = manager_map_.begin();
    for (; it != manager_map_.end(); ++it) {
      if (!it->second->IsAllComplete())
        return false;
    }
    return true;
  }

  void AssertAllComplete() const {
    EXPECT_TRUE(finalized_);
    EXPECT_FALSE(running_);

    URLManagerMap::const_iterator it = manager_map_.begin();
    for (; it != manager_map_.end(); ++it)
      it->second->AssertAllComplete();
  }

  bool HasAutoQueries() const {
    if (manager_map_.empty())
      return false;

    URLManagerMap::const_iterator it = manager_map_.begin();
    for (; it != manager_map_.end(); ++it) {
      if (it->second->HasAutoQueries())
        return true;
    }

    return false;
  }

  MultiQueryManager* GetManager(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame) const {
    const std::string& url = frame->GetURL();
    URLManagerMap::const_iterator it = manager_map_.find(url);
    EXPECT_NE(it, manager_map_.end());
    return it->second;
  }

  void RemoveAllManagers() {
    if (manager_map_.empty())
      return;

    URLManagerMap::const_iterator it = manager_map_.begin();
    for (; it != manager_map_.end(); ++it)
      delete it->second;
    manager_map_.clear();
  }

  std::string GetURLForManager(MultiQueryManager* manager) const {
    if (!manager_map_.empty()) {
      URLManagerMap::const_iterator it = manager_map_.begin();
      for (; it != manager_map_.end(); ++it) {
        if (it->second == manager)
          return it->first;
      }
    }
    return std::string();
  }

  static std::string GetNameForURL(const std::string& url) {
    // Extract the file name without extension.
    int pos1 = static_cast<int>(url.rfind("/"));
    int pos2 = static_cast<int>(url.rfind("."));
    EXPECT_TRUE(pos1 >= 0 && pos2 >= 0 && pos1 < pos2);
    return url.substr(pos1 + 1, pos2 - pos1 - 1);
  }

 private:
  // Map of page URL to MultiQueryManager instance.
  typedef std::map<std::string, MultiQueryManager*> URLManagerMap;
  URLManagerMap manager_map_;

  typedef std::set<Observer*> ObserverSet;
  ObserverSet observer_set_;

  // Set to true after all query managers have been added.
  bool finalized_;
  // Set to true while queries are pending.
  bool running_;

  // Number of managers that have completed.
  int manual_complete_count_;
  int total_complete_count_;
};

// Test multiple queries in a single page load with multiple frames.
class MultiQueryMultiFrameTestHandler :
    public SingleLoadTestHandler,
    public MultiQueryManagerMap::Observer {
 public:
  MultiQueryMultiFrameTestHandler(bool synchronous, bool cancel_with_subnav)
      : synchronous_(synchronous),
        cancel_with_subnav_(cancel_with_subnav) {
    manager_map_.AddObserver(this);
  }

  void AddOtherResources() override {
    AddSubFrameResource("sub1");
    AddSubFrameResource("sub2");
    AddSubFrameResource("sub3");
    manager_map_.Finalize();

    if (manager_map_.HasAutoQueries()) {
      cancel_url_ = std::string(kTestDomain1) + "cancel.html";
      AddResource(cancel_url_, "<html><body>cancel</body></html>", "text/html");
    }
  }

  std::string GetMainHTML() override {
    return manager_map_.GetMainHTML();
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) override {
    AssertMainBrowser(browser);
    EXPECT_FALSE(frame->IsMain());

    manager_map_.OnNotify(browser, frame, message);
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    AssertMainBrowser(browser);
    EXPECT_FALSE(frame->IsMain());

    return manager_map_.OnQuery(browser, frame, query_id, request, persistent,
                                callback);
  }

  void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id) override {
    AssertMainBrowser(browser);
    EXPECT_FALSE(frame->IsMain());

    manager_map_.OnQueryCanceled(browser, frame, query_id);
  }

  void OnMapManualQueriesCompleted(MultiQueryManagerMap* map) override {
    EXPECT_EQ(map, &manager_map_);
    if (manager_map_.HasAutoQueries()) {
      CefRefPtr<CefFrame> frame = GetBrowser()->GetMainFrame();

      // Navigate somewhere else to terminate the auto queries.
      if (cancel_with_subnav_) {
        // Navigate each subframe individually.
        const std::string js =
            "document.getElementById('sub1').src = '" + cancel_url_ + "';"
            "document.getElementById('sub2').src = '" + cancel_url_ + "';"
            "document.getElementById('sub3').src = '" + cancel_url_ + "';";

        frame->ExecuteJavaScript(js, frame->GetURL(), 0);
      } else {
        // Navigate the main frame.
        frame->LoadURL(cancel_url_);
      }
    }
  }

  void OnMapAllQueriesCompleted(MultiQueryManagerMap* map) override {
    EXPECT_EQ(map, &manager_map_);
    DestroyTest();
  }

  void DestroyTest() override {
    manager_map_.AssertAllComplete();
    TestHandler::DestroyTest();
  }

 private:
  void AddSubFrameResource(const std::string& name) {
    const std::string& url = std::string(kTestDomain1) + name + ".html";

    MultiQueryManager* manager = manager_map_.CreateManager(url, synchronous_);
    MakeTestQueries(manager, false, 100);

    const std::string& html = manager->GetHTML(false, false);
    AddResource(url, html, "text/html");
  }

  const bool synchronous_;
  const bool cancel_with_subnav_;

  MultiQueryManagerMap manager_map_;

  std::string cancel_url_;
};

}  // namespace

// Test that multiple frames can run many queries successfully in a synchronous
// manner.
TEST(MessageRouterTest, MultiQueryMultiFrameSync) {
  CefRefPtr<MultiQueryMultiFrameTestHandler> handler =
      new MultiQueryMultiFrameTestHandler(true, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple frames can run many queries successfully in an
// asynchronous manner.
TEST(MessageRouterTest, MultiQueryMultiFrameAsync) {
  CefRefPtr<MultiQueryMultiFrameTestHandler> handler =
      new MultiQueryMultiFrameTestHandler(false, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple frames can run many queries successfully in a synchronous
// manner. Cancel auto queries with sub-frame navigation.
TEST(MessageRouterTest, MultiQueryMultiFrameSyncSubnavCancel) {
  CefRefPtr<MultiQueryMultiFrameTestHandler> handler =
      new MultiQueryMultiFrameTestHandler(true, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple frames can run many queries successfully in an
// asynchronous manner. Cancel auto queries with sub-frame navigation.
TEST(MessageRouterTest, MultiQueryMultiFrameAsyncSubnavCancel) {
  CefRefPtr<MultiQueryMultiFrameTestHandler> handler =
      new MultiQueryMultiFrameTestHandler(false, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


namespace {

// Implementation of MRTestHandler that loads multiple pages and/or browsers and
// executes multiple queries.
class MultiQueryMultiLoadTestHandler :
    public MRTestHandler,
    public CefMessageRouterBrowserSide::Handler,
    public MultiQueryManagerMap::Observer,
    public MultiQueryManager::Observer {
 public:
  MultiQueryMultiLoadTestHandler(bool some, bool synchronous)
      : some_(some),
        synchronous_(synchronous) {
    manager_map_.AddObserver(this);
  }

  void OnNotify(CefRefPtr<CefBrowser> browser,
                CefRefPtr<CefFrame> frame,
                const std::string& message) override {
    manager_map_.OnNotify(browser, frame, message);
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    return manager_map_.OnQuery(browser, frame, query_id, request, persistent,
                                callback);
  }

  void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id) override {
    manager_map_.OnQueryCanceled(browser, frame, query_id);
  }

  void OnMapManualQueriesCompleted(MultiQueryManagerMap* map) override {
    EXPECT_EQ(map, &manager_map_);
    if (manager_map_.HasAutoQueries()) {
      // Navigate all browsers somewhere else to terminate the auto queries.
      BrowserMap map;
      GetAllBrowsers(&map);

      BrowserMap::const_iterator it = map.begin();
      for (; it != map.end(); ++it) {
        it->second->GetMainFrame()->LoadURL(cancel_url_);
      }
    }
  }

  void OnMapAllQueriesCompleted(MultiQueryManagerMap* map) override {
    EXPECT_EQ(map, &manager_map_);
    DestroyTest();
  }

  void DestroyTest() override {
    manager_map_.AssertAllComplete();
    TestHandler::DestroyTest();
  }

 protected:
  void AddHandlers(
      CefRefPtr<CefMessageRouterBrowserSide> message_router) override {
    message_router->AddHandler(this, false);
  }

  void AddManagedResource(const std::string& url,
                          bool assert_total,
                          bool assert_browser) {
    MultiQueryManager* manager = manager_map_.CreateManager(url, synchronous_);
    manager->AddObserver(this);
    MakeTestQueries(manager, some_, 75);

    const std::string& html = manager->GetHTML(assert_total, assert_browser);
    AddResource(url, html, "text/html");
  }

  void Finalize() {
    manager_map_.Finalize();

    if (manager_map_.HasAutoQueries()) {
      cancel_url_ = std::string(kTestDomain1) + "cancel.html";
      AddResource(cancel_url_, "<html><body>cancel</body></html>", "text/html");
    }
  }

  MultiQueryManagerMap manager_map_;

 private:
  const bool some_;
  const bool synchronous_;

  std::string cancel_url_;
};

// Test multiple browsers that send queries at the same time.
class MultiQueryMultiBrowserTestHandler : public MultiQueryMultiLoadTestHandler {
 public:
  MultiQueryMultiBrowserTestHandler(bool synchronous, bool same_origin)
      : MultiQueryMultiLoadTestHandler(false, synchronous),
        same_origin_(same_origin) {
  }

 protected:
  void RunMRTest() override {
    const std::string& url1 = std::string(kTestDomain1) + "browser1.html";
    const std::string& url2 =
        std::string(same_origin_ ? kTestDomain1 : kTestDomain2) +
        "browser2.html";
    const std::string& url3 =
        std::string(same_origin_ ? kTestDomain1 : kTestDomain3) +
        "browser3.html";

    AddManagedResource(url1, false, true);
    AddManagedResource(url2, false, true);
    AddManagedResource(url3, false, true);
    Finalize();

    // Create 2 browsers simultaniously.
    CreateBrowser(url1, NULL);
    CreateBrowser(url2, NULL);
    CreateBrowser(url3, NULL);
  }

 private:
  bool same_origin_;
};

}  // namespace

// Test that multiple browsers can query simultaniously from the same origin.
TEST(MessageRouterTest, MultiQueryMultiBrowserSameOriginSync) {
  CefRefPtr<MultiQueryMultiBrowserTestHandler> handler =
      new MultiQueryMultiBrowserTestHandler(true, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple browsers can query simultaniously from the same origin.
TEST(MessageRouterTest, MultiQueryMultiBrowserSameOriginAsync) {
  CefRefPtr<MultiQueryMultiBrowserTestHandler> handler =
      new MultiQueryMultiBrowserTestHandler(false, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple browsers can query simultaniously from different origins.
TEST(MessageRouterTest, MultiQueryMultiBrowserDifferentOriginSync) {
  CefRefPtr<MultiQueryMultiBrowserTestHandler> handler =
      new MultiQueryMultiBrowserTestHandler(true, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple browsers can query simultaniously from different origins.
TEST(MessageRouterTest, MultiQueryMultiBrowserDifferentOriginAsync) {
  CefRefPtr<MultiQueryMultiBrowserTestHandler> handler =
      new MultiQueryMultiBrowserTestHandler(false, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


namespace {

// Test multiple navigations that send queries sequentially.
class MultiQueryMultiNavigateTestHandler : public MultiQueryMultiLoadTestHandler {
 public:
  MultiQueryMultiNavigateTestHandler(bool synchronous, bool same_origin)
      : MultiQueryMultiLoadTestHandler(false, synchronous),
        same_origin_(same_origin) {
  }

  void OnManualQueriesCompleted(MultiQueryManager* manager) override {
    const std::string& url = manager_map_.GetURLForManager(manager);
    if (url == url1_)  // 2. Load the 2nd url.
      GetBrowser()->GetMainFrame()->LoadURL(url2_);
    else if (url == url2_)  // 3. Load the 3rd url.
      GetBrowser()->GetMainFrame()->LoadURL(url3_);
  }

 protected:
  void RunMRTest() override {
    url1_ = std::string(kTestDomain1) + "browser1.html";
    url2_ = std::string(same_origin_ ? kTestDomain1 : kTestDomain2) +
            "browser2.html";
    url3_ = std::string(same_origin_ ? kTestDomain1 : kTestDomain3) +
            "browser3.html";
    
    AddManagedResource(url1_, true, true);
    AddManagedResource(url2_, true, true);
    AddManagedResource(url3_, true, true);
    Finalize();

    // 1. Load the 1st url.
    CreateBrowser(url1_, NULL);
  }

 private:
  bool same_origin_;

  std::string url1_;
  std::string url2_;
  std::string url3_;
};

}  // namespace

// Test that multiple navigations can query from the same origin.
TEST(MessageRouterTest, MultiQueryMultiNavigateSameOriginSync) {
  CefRefPtr<MultiQueryMultiNavigateTestHandler> handler =
      new MultiQueryMultiNavigateTestHandler(true, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple navigations can query from the same origin.
TEST(MessageRouterTest, MultiQueryMultiNavigateSameOriginAsync) {
  CefRefPtr<MultiQueryMultiNavigateTestHandler> handler =
      new MultiQueryMultiNavigateTestHandler(false, true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple navigations can query from different origins.
TEST(MessageRouterTest, MultiQueryMultiNavigateDifferentOriginSync) {
  CefRefPtr<MultiQueryMultiNavigateTestHandler> handler =
      new MultiQueryMultiNavigateTestHandler(true, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that multiple navigations can query from different origins.
TEST(MessageRouterTest, MultiQueryMultiNavigateDifferentOriginAsync) {
  CefRefPtr<MultiQueryMultiNavigateTestHandler> handler =
      new MultiQueryMultiNavigateTestHandler(false, false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}
