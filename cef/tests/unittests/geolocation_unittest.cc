// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "base/synchronization/waitable_event.h"

#include "include/base/cef_bind.h"
#include "include/cef_geolocation.h"
#include "include/wrapper/cef_closure_task.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/test_handler.h"
#include "tests/unittests/test_util.h"

namespace {

const char* kTestOrigin = "http://tests/";
const char* kTestUrl = "http://tests/GeolocationTestHandler";
const char* kTestAllowUrl = "http://tests/GeolocationTestHandler.Allow";
const char* kTestDenyUrl = "http://tests/GeolocationTestHandler.Deny";
const char* kTestCancelUrl = "http://tests/GeolocationTestHandler.Cancel";

enum TestMode {
  TEST_ALLOW,
  TEST_DENY,
  TEST_CANCEL,
};

class GeolocationTestHandler : public TestHandler {
 public:
  GeolocationTestHandler(const TestMode& mode, bool async)
      : mode_(mode),
        async_(async),
        request_id_(-1) {
  }

  void RunTest() override {
    std::string html =
        "<html><head><script>"
        "navigator.geolocation.getCurrentPosition("
        // Success function
        "function() {"
            "window.location.href = '" + std::string(kTestAllowUrl) + "'; },"
        // Error function
        "function() {"
            "window.location.href = '" + std::string(kTestDenyUrl) + "';  });";
    if (mode_ == TEST_CANCEL)
      html += "window.location.href = '" + std::string(kTestCancelUrl) + "';";
    html += "</script></head><body>TEST START</body></html>";
    AddResource(kTestUrl, html, "text/html");

    std::string end_html = "<html><body>TEST END</body></html>";
    AddResource(kTestAllowUrl, end_html, "text/html");
    AddResource(kTestDenyUrl, end_html, "text/html");
    AddResource(kTestCancelUrl, end_html, "text/html");

    // Create the browser
    CreateBrowser(kTestUrl);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    std::string url = frame->GetURL();
    if (url != kTestUrl) {
      if (url == kTestAllowUrl)
        got_allow_.yes();
      else if (url == kTestDenyUrl)
        got_deny_.yes();
      else if (url == kTestCancelUrl)
        got_cancel_.yes();

      DestroyTest();
    }
  }

  void ExecuteCallback(CefRefPtr<CefGeolocationCallback> callback) {
    if (mode_ == TEST_ALLOW)
      callback->Continue(true);
    else if (mode_ == TEST_DENY)
      callback->Continue(false);
  }

  bool OnRequestGeolocationPermission(
      CefRefPtr<CefBrowser> browser,
      const CefString& requesting_url,
      int request_id,
      CefRefPtr<CefGeolocationCallback> callback) override {
    got_requestgeolocationpermission_.yes();

    EXPECT_TRUE(CefCurrentlyOn(TID_UI));

    EXPECT_STREQ(kTestOrigin, requesting_url.ToString().c_str());
    request_id_ = request_id;

    if (!async_) {
      ExecuteCallback(callback);
    } else {
      CefPostTask(TID_UI,
          base::Bind(&GeolocationTestHandler::ExecuteCallback, this, callback));
    }

    return true;
  }

  void OnCancelGeolocationPermission(
      CefRefPtr<CefBrowser> browser,
      const CefString& requesting_url,
      int request_id) override {
    got_cancelgeolocationpermission_.yes();

    EXPECT_TRUE(CefCurrentlyOn(TID_UI));

    EXPECT_STREQ(kTestOrigin, requesting_url.ToString().c_str());
    EXPECT_EQ(request_id, request_id_);
  }

  void DestroyTest() override {
    EXPECT_TRUE(got_requestgeolocationpermission_);
    if (mode_ == TEST_CANCEL)
      EXPECT_TRUE(got_cancelgeolocationpermission_);
    else
      EXPECT_FALSE(got_cancelgeolocationpermission_);

    TestHandler::DestroyTest();
  }

  TestMode mode_;
  bool async_;

  int request_id_;

  TrackCallback got_requestgeolocationpermission_;
  TrackCallback got_cancelgeolocationpermission_;
  TrackCallback got_allow_;
  TrackCallback got_cancel_;
  TrackCallback got_deny_;

  IMPLEMENT_REFCOUNTING(GeolocationTestHandler);
};

}  // namespace

TEST(GeolocationTest, HandlerAllow) {
  CefRefPtr<GeolocationTestHandler> handler =
      new GeolocationTestHandler(TEST_ALLOW, false);
  handler->ExecuteTest();
  EXPECT_TRUE(handler->got_allow_);
  ReleaseAndWaitForDestructor(handler);
}

TEST(GeolocationTest, HandlerAllowAsync) {
  CefRefPtr<GeolocationTestHandler> handler =
      new GeolocationTestHandler(TEST_ALLOW, true);
  handler->ExecuteTest();
  EXPECT_TRUE(handler->got_allow_);
  ReleaseAndWaitForDestructor(handler);
}

TEST(GeolocationTest, HandlerDeny) {
  CefRefPtr<GeolocationTestHandler> handler =
      new GeolocationTestHandler(TEST_DENY, false);
  handler->ExecuteTest();
  EXPECT_TRUE(handler->got_deny_);
  ReleaseAndWaitForDestructor(handler);
}

TEST(GeolocationTest, HandlerDenyAsync) {
  CefRefPtr<GeolocationTestHandler> handler =
      new GeolocationTestHandler(TEST_DENY, true);
  handler->ExecuteTest();
  EXPECT_TRUE(handler->got_deny_);
  ReleaseAndWaitForDestructor(handler);
}

TEST(GeolocationTest, HandlerCancel) {
  CefRefPtr<GeolocationTestHandler> handler =
      new GeolocationTestHandler(TEST_CANCEL, false);
  handler->ExecuteTest();
  EXPECT_TRUE(handler->got_cancel_);
  ReleaseAndWaitForDestructor(handler);
}


namespace {

class TestGetGeolocationCallback : public CefGetGeolocationCallback {
 public:
  explicit TestGetGeolocationCallback(base::WaitableEvent* event)
      : event_(event) {
  }

  void OnLocationUpdate(const CefGeoposition& position) override {
    EXPECT_TRUE(CefCurrentlyOn(TID_UI));
    EXPECT_EQ(position.error_code, GEOPOSITON_ERROR_NONE);
    EXPECT_NE(position.latitude, 0.0);
    EXPECT_NE(position.longitude, 0.0);
    EXPECT_NE(position.accuracy, 0.0);
    EXPECT_NE(position.timestamp.year, 0);
    event_->Signal();
  }

private:
  base::WaitableEvent* event_;

  IMPLEMENT_REFCOUNTING(TestGetGeolocationCallback);
};

}  // namespace

TEST(GeolocationTest, GetGeolocation) {
  base::WaitableEvent event(false, false);
  CefGetGeolocation(new TestGetGeolocationCallback(&event));
  event.Wait();
}
