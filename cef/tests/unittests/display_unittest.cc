// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <list>

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/test_handler.h"

namespace {

// How it works:
// 1. Load kTitleUrl1 (title should be kTitleStr1)
// 2. Load kTitleUrl2 (title should be kTitleStr2)
// 3. History back to kTitleUrl1 (title should be kTitleStr1)
// 4. History forward to kTitleUrl2 (title should be kTitleStr2)
// 5. Set title via JavaScript (title should be kTitleStr3)

const char kTitleUrl1[] = "http://tests-title/nav1.html";
const char kTitleUrl2[] = "http://tests-title/nav2.html";
const char kTitleStr1[] = "Title 1";
const char kTitleStr2[] = "Title 2";
const char kTitleStr3[] = "Title 3";

// Browser side.
class TitleTestHandler : public TestHandler {
 public:
  TitleTestHandler()
      : step_(0) {}

  void RunTest() override {
    // Add the resources that we will navigate to/from.
    AddResource(kTitleUrl1,
        "<html><head><title>" + std::string(kTitleStr1) +
        "</title></head>Nav1</html>", "text/html");
    AddResource(kTitleUrl2,
        "<html><head><title>" + std::string(kTitleStr2) +
        "</title></head>Nav2" +
        "<script>function setTitle() { window.document.title = '" +
        std::string(kTitleStr3) + "'; }</script>" +
        "</html>", "text/html");

    // Create the browser.
    CreateBrowser(kTitleUrl1);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnTitleChange(CefRefPtr<CefBrowser> browser,
                     const CefString& title) override {
    std::string title_str = title;
    if (step_ == 0 || step_ == 2) {
      EXPECT_STREQ(kTitleStr1, title_str.c_str());
    } else if (step_ == 1 || step_ == 3) {
      EXPECT_STREQ(kTitleStr2, title_str.c_str());
    } else if (step_ == 4) {
      // Ignore the unexpected notification of the page URL.
      // Related bug: http://crbug.com/331351
      if (title_str == &kTitleUrl2[7])
        return;

      EXPECT_STREQ(kTitleStr3, title_str.c_str());
    }

    got_title_[step_].yes();

    if (step_ == 4)
      DestroyTest();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    switch (step_++) {
      case 0:
        frame->LoadURL(kTitleUrl2);
        break;
      case 1:
        browser->GoBack();
        break;
      case 2:
        browser->GoForward();
        break;
      case 3:
        frame->ExecuteJavaScript("setTitle()", kTitleUrl2, 0);
        break;
      default:
        EXPECT_TRUE(false); // Not reached.
    }
  }

 private:
  void DestroyTest() override {
    for (int i = 0; i < 5; ++i)
      EXPECT_TRUE(got_title_[i]) << "step " << i;

    TestHandler::DestroyTest();
  }

  int step_;

  TrackCallback got_title_[5];

  IMPLEMENT_REFCOUNTING(TitleTestHandler);
};

}  // namespace

// Test title notifications.
TEST(DisplayTest, Title) {
  CefRefPtr<TitleTestHandler> handler = new TitleTestHandler();
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}
