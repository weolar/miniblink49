// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/routing_test_handler.h"

namespace {

const char kLifeSpanUrl[] = "http://tests-life-span/test.html";
const char kUnloadDialogText[] = "Are you sure?";
const char kUnloadMsg[] = "LifeSpanTestHandler.Unload";

// Browser side.
class LifeSpanTestHandler : public RoutingTestHandler {
 public:
  struct Settings {
    Settings()
      : force_close(false),
        add_onunload_handler(false),
        allow_do_close(true),
        accept_before_unload_dialog(true) {}

    bool force_close;
    bool add_onunload_handler;
    bool allow_do_close;
    bool accept_before_unload_dialog;
  };

  explicit LifeSpanTestHandler(const Settings& settings)
      : settings_(settings),
        executing_delay_close_(false) {
    // By default no LifeSpan tests call DestroyTest().
    SetDestroyTestExpected(false);
  }

  void RunTest() override {
    // Add the resources that we will navigate to/from.
    std::string page = "<html><script>";

    page += "window.onunload = function() { window.testQuery({request:'" +
        std::string(kUnloadMsg) + "'}); };";

    if (settings_.add_onunload_handler) {
      page += "window.onbeforeunload = function() { return '" +
              std::string(kUnloadDialogText) + "'; };";
    }

    page += "</script><body>Page</body></html>";
    AddResource(kLifeSpanUrl, page, "text/html");

    // Create the browser.
    CreateBrowser(kLifeSpanUrl);

    // Intentionally don't call SetTestTimeout() for these tests.
  }

  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
    got_after_created_.yes();
    RoutingTestHandler::OnAfterCreated(browser);
  }

  bool DoClose(CefRefPtr<CefBrowser> browser) override {
    if (executing_delay_close_)
      return false;

    EXPECT_TRUE(browser->IsSame(GetBrowser()));

    got_do_close_.yes();

    if (!settings_.allow_do_close) {
      // The close will be canceled.
      ScheduleDelayClose();
    }

    return !settings_.allow_do_close;
  }

  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
    if (!executing_delay_close_) {
      got_before_close_.yes();
      EXPECT_TRUE(browser->IsSame(GetBrowser()));
    }

    RoutingTestHandler::OnBeforeClose(browser);
  }

  bool OnBeforeUnloadDialog(
      CefRefPtr<CefBrowser> browser,
      const CefString& message_text,
      bool is_reload,
      CefRefPtr<CefJSDialogCallback> callback) override {
    if (executing_delay_close_) {
      callback->Continue(true, CefString());
      return true;
    }

    EXPECT_TRUE(browser->IsSame(GetBrowser()));
    EXPECT_STREQ(kUnloadDialogText, message_text.ToString().c_str());
    EXPECT_FALSE(is_reload);
    EXPECT_TRUE(callback.get());

    if (!settings_.accept_before_unload_dialog) {
      // The close will be canceled.
      ScheduleDelayClose();
    }

    got_before_unload_dialog_.yes();
    callback->Continue(settings_.accept_before_unload_dialog, CefString());
    return true;
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    got_load_end_.yes();
    EXPECT_TRUE(browser->IsSame(GetBrowser()));

    // Attempt to close the browser.
    browser->GetHost()->CloseBrowser(settings_.force_close);
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    if (request.ToString() == kUnloadMsg) {
      if (!executing_delay_close_)
        got_unload_message_.yes();
    }
    callback->Success("");
    return true;
  }

  TrackCallback got_after_created_;
  TrackCallback got_do_close_;
  TrackCallback got_before_close_;
  TrackCallback got_before_unload_dialog_;
  TrackCallback got_unload_message_;
  TrackCallback got_load_end_;
  TrackCallback got_delay_close_;

 private:
  // Wait a bit to make sure no additional events are received and then close
  // the window.
  void ScheduleDelayClose() {
    // This test will call DestroyTest().
    SetDestroyTestExpected(true);

    CefPostDelayedTask(TID_UI,
        base::Bind(&LifeSpanTestHandler::DelayClose, this), 100);
  }

  void DelayClose() {
    got_delay_close_.yes();
    executing_delay_close_ = true;
    DestroyTest();
  }

  Settings settings_;

  // Forces the window to close (bypasses test conditions).
  bool executing_delay_close_;

  IMPLEMENT_REFCOUNTING(LifeSpanTestHandler);
};

}  // namespace

TEST(LifeSpanTest, DoCloseAllow) {
  LifeSpanTestHandler::Settings settings;
  settings.allow_do_close = true;
  CefRefPtr<LifeSpanTestHandler> handler = new LifeSpanTestHandler(settings);
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_after_created_);
  EXPECT_TRUE(handler->got_do_close_);
  EXPECT_TRUE(handler->got_before_close_);
  EXPECT_FALSE(handler->got_before_unload_dialog_);
  EXPECT_TRUE(handler->got_unload_message_);
  EXPECT_TRUE(handler->got_load_end_);
  EXPECT_FALSE(handler->got_delay_close_);

  ReleaseAndWaitForDestructor(handler);
}

TEST(LifeSpanTest, DoCloseAllowForce) {
  LifeSpanTestHandler::Settings settings;
  settings.allow_do_close = true;
  settings.force_close = true;
  CefRefPtr<LifeSpanTestHandler> handler = new LifeSpanTestHandler(settings);
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_after_created_);
  EXPECT_TRUE(handler->got_do_close_);
  EXPECT_TRUE(handler->got_before_close_);
  EXPECT_FALSE(handler->got_before_unload_dialog_);
  EXPECT_TRUE(handler->got_unload_message_);
  EXPECT_TRUE(handler->got_load_end_);
  EXPECT_FALSE(handler->got_delay_close_);

  ReleaseAndWaitForDestructor(handler);
}

TEST(LifeSpanTest, DoCloseDisallow) {
  LifeSpanTestHandler::Settings settings;
  settings.allow_do_close = false;
  CefRefPtr<LifeSpanTestHandler> handler = new LifeSpanTestHandler(settings);
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_after_created_);
  EXPECT_TRUE(handler->got_do_close_);
  EXPECT_FALSE(handler->got_before_close_);
  EXPECT_FALSE(handler->got_before_unload_dialog_);
  EXPECT_TRUE(handler->got_unload_message_);
  EXPECT_TRUE(handler->got_load_end_);
  EXPECT_TRUE(handler->got_delay_close_);

  ReleaseAndWaitForDestructor(handler);
}

TEST(LifeSpanTest, DoCloseDisallowForce) {
  LifeSpanTestHandler::Settings settings;
  settings.allow_do_close = false;
  settings.force_close = true;
  CefRefPtr<LifeSpanTestHandler> handler = new LifeSpanTestHandler(settings);
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_after_created_);
  EXPECT_TRUE(handler->got_do_close_);
  EXPECT_FALSE(handler->got_before_close_);
  EXPECT_FALSE(handler->got_before_unload_dialog_);
  EXPECT_TRUE(handler->got_unload_message_);
  EXPECT_TRUE(handler->got_load_end_);
  EXPECT_TRUE(handler->got_delay_close_);

  ReleaseAndWaitForDestructor(handler);
}

TEST(LifeSpanTest, DoCloseDisallowWithOnUnloadAllow) {
  LifeSpanTestHandler::Settings settings;
  settings.allow_do_close = false;
  settings.add_onunload_handler = true;
  settings.accept_before_unload_dialog = true;
  CefRefPtr<LifeSpanTestHandler> handler = new LifeSpanTestHandler(settings);
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_after_created_);
  EXPECT_TRUE(handler->got_do_close_);
  EXPECT_FALSE(handler->got_before_close_);
  EXPECT_TRUE(handler->got_before_unload_dialog_);
  EXPECT_TRUE(handler->got_unload_message_);
  EXPECT_TRUE(handler->got_load_end_);
  EXPECT_TRUE(handler->got_delay_close_);

  ReleaseAndWaitForDestructor(handler);
}

TEST(LifeSpanTest, DoCloseAllowWithOnUnloadForce) {
  LifeSpanTestHandler::Settings settings;
  settings.allow_do_close = true;
  settings.add_onunload_handler = true;
  settings.force_close = true;
  CefRefPtr<LifeSpanTestHandler> handler = new LifeSpanTestHandler(settings);
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_after_created_);
  EXPECT_TRUE(handler->got_do_close_);
  EXPECT_TRUE(handler->got_before_close_);
  EXPECT_FALSE(handler->got_before_unload_dialog_);
  EXPECT_TRUE(handler->got_unload_message_);
  EXPECT_TRUE(handler->got_load_end_);
  EXPECT_FALSE(handler->got_delay_close_);

  ReleaseAndWaitForDestructor(handler);
}

TEST(LifeSpanTest, DoCloseDisallowWithOnUnloadForce) {
  LifeSpanTestHandler::Settings settings;
  settings.allow_do_close = false;
  settings.add_onunload_handler = true;
  settings.force_close = true;
  CefRefPtr<LifeSpanTestHandler> handler = new LifeSpanTestHandler(settings);
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_after_created_);
  EXPECT_TRUE(handler->got_do_close_);
  EXPECT_FALSE(handler->got_before_close_);
  EXPECT_FALSE(handler->got_before_unload_dialog_);
  EXPECT_TRUE(handler->got_unload_message_);
  EXPECT_TRUE(handler->got_load_end_);
  EXPECT_TRUE(handler->got_delay_close_);

  ReleaseAndWaitForDestructor(handler);
}

TEST(LifeSpanTest, OnUnloadAllow) {
  LifeSpanTestHandler::Settings settings;
  settings.add_onunload_handler = true;
  settings.accept_before_unload_dialog = true;
  CefRefPtr<LifeSpanTestHandler> handler = new LifeSpanTestHandler(settings);
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_after_created_);
  EXPECT_TRUE(handler->got_do_close_);
  EXPECT_TRUE(handler->got_before_close_);
  EXPECT_TRUE(handler->got_before_unload_dialog_);
  EXPECT_TRUE(handler->got_unload_message_);
  EXPECT_TRUE(handler->got_load_end_);
  EXPECT_FALSE(handler->got_delay_close_);

  ReleaseAndWaitForDestructor(handler);
}

TEST(LifeSpanTest, OnUnloadDisallow) {
  LifeSpanTestHandler::Settings settings;
  settings.add_onunload_handler = true;
  settings.accept_before_unload_dialog = false;
  CefRefPtr<LifeSpanTestHandler> handler = new LifeSpanTestHandler(settings);
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_after_created_);
  EXPECT_FALSE(handler->got_do_close_);
  EXPECT_FALSE(handler->got_before_close_);
  EXPECT_TRUE(handler->got_before_unload_dialog_);
  EXPECT_FALSE(handler->got_unload_message_);
  EXPECT_TRUE(handler->got_load_end_);
  EXPECT_TRUE(handler->got_delay_close_);

  ReleaseAndWaitForDestructor(handler);
}
