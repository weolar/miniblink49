// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "include/base/cef_bind.h"
#include "base/files/scoped_temp_dir.h"
#include "include/cef_request_context.h"
#include "include/cef_request_context_handler.h"
#include "include/wrapper/cef_closure_task.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/test_handler.h"

TEST(RequestContextTest, GetGlobalContext) {
  CefRefPtr<CefRequestContext> context1 =
      CefRequestContext::GetGlobalContext();
  EXPECT_TRUE(context1.get());
  EXPECT_TRUE(context1->IsGlobal());
  EXPECT_TRUE(context1->IsSame(context1));
  EXPECT_TRUE(context1->IsSharingWith(context1));

  CefRefPtr<CefRequestContext> context2 =
      CefRequestContext::GetGlobalContext();
  EXPECT_TRUE(context2.get());
  EXPECT_TRUE(context2->IsGlobal());
  EXPECT_TRUE(context2->IsSame(context2));
  EXPECT_TRUE(context2->IsSharingWith(context2));

  EXPECT_TRUE(context1->IsSame(context2));
  EXPECT_TRUE(context2->IsSame(context1));
  EXPECT_TRUE(context1->IsSharingWith(context2));
  EXPECT_TRUE(context2->IsSharingWith(context1));
}

TEST(RequestContextTest, CreateContext) {
  class Handler : public CefRequestContextHandler {
   public:
    Handler() {}
    CefRefPtr<CefCookieManager> GetCookieManager() override { return NULL; }
   private:
    IMPLEMENT_REFCOUNTING(Handler);
  };

  CefRefPtr<CefRequestContextHandler> handler = new Handler();

  CefRequestContextSettings settings;

  CefRefPtr<CefRequestContext> context1 =
      CefRequestContext::CreateContext(settings, handler.get());
  EXPECT_TRUE(context1.get());
  EXPECT_FALSE(context1->IsGlobal());
  EXPECT_TRUE(context1->IsSame(context1));
  EXPECT_TRUE(context1->IsSharingWith(context1));
  EXPECT_EQ(context1->GetHandler().get(), handler.get());
  
  CefRefPtr<CefRequestContext> context2 =
      CefRequestContext::CreateContext(settings, handler.get());
  EXPECT_TRUE(context2.get());
  EXPECT_FALSE(context2->IsGlobal());
  EXPECT_TRUE(context2->IsSame(context2));
  EXPECT_TRUE(context2->IsSharingWith(context2));
  EXPECT_EQ(context2->GetHandler().get(), handler.get());

  EXPECT_FALSE(context1->IsSame(context2));
  EXPECT_FALSE(context1->IsSharingWith(context2));
  EXPECT_FALSE(context2->IsSame(context1));
  EXPECT_FALSE(context2->IsSharingWith(context1));
  
  CefRefPtr<CefRequestContext> context3 =
      CefRequestContext::GetGlobalContext();
  EXPECT_TRUE(context3.get());
  EXPECT_FALSE(context3->IsSame(context1));
  EXPECT_FALSE(context3->IsSharingWith(context1));
  EXPECT_FALSE(context3->IsSame(context2));
  EXPECT_FALSE(context3->IsSharingWith(context2));
  EXPECT_FALSE(context1->IsSame(context3));
  EXPECT_FALSE(context1->IsSharingWith(context3));
  EXPECT_FALSE(context2->IsSame(context3));
  EXPECT_FALSE(context2->IsSharingWith(context3));
}

TEST(RequestContextTest, CreateContextNoHandler) {
  CefRequestContextSettings settings;

  CefRefPtr<CefRequestContext> context1 =
      CefRequestContext::CreateContext(settings, NULL);
  EXPECT_TRUE(context1.get());
  EXPECT_FALSE(context1->IsGlobal());
  EXPECT_TRUE(context1->IsSame(context1));
  EXPECT_TRUE(context1->IsSharingWith(context1));
  EXPECT_FALSE(context1->GetHandler().get());
  
  CefRefPtr<CefRequestContext> context2 =
      CefRequestContext::CreateContext(settings, NULL);
  EXPECT_TRUE(context2.get());
  EXPECT_FALSE(context2->IsGlobal());
  EXPECT_TRUE(context2->IsSame(context2));
  EXPECT_TRUE(context2->IsSharingWith(context2));
  EXPECT_FALSE(context2->GetHandler().get());

  EXPECT_FALSE(context1->IsSame(context2));
  EXPECT_FALSE(context1->IsSharingWith(context2));
  EXPECT_FALSE(context2->IsSame(context1));
  EXPECT_FALSE(context2->IsSharingWith(context1));
  
  CefRefPtr<CefRequestContext> context3 =
      CefRequestContext::GetGlobalContext();
  EXPECT_TRUE(context3.get());
  EXPECT_FALSE(context3->IsSame(context1));
  EXPECT_FALSE(context3->IsSharingWith(context1));
  EXPECT_FALSE(context3->IsSame(context2));
  EXPECT_FALSE(context3->IsSharingWith(context2));
  EXPECT_FALSE(context1->IsSame(context3));
  EXPECT_FALSE(context1->IsSharingWith(context3));
  EXPECT_FALSE(context2->IsSame(context3));
  EXPECT_FALSE(context2->IsSharingWith(context3));
}

TEST(RequestContextTest, CreateContextSharedGlobal) {
  CefRequestContextSettings settings;

  CefRefPtr<CefRequestContext> context1 =
      CefRequestContext::GetGlobalContext();
  EXPECT_TRUE(context1.get());
  EXPECT_TRUE(context1->IsGlobal());
  EXPECT_TRUE(context1->IsSame(context1));
  EXPECT_TRUE(context1->IsSharingWith(context1));

  CefRefPtr<CefRequestContext> context2 =
      CefRequestContext::CreateContext(context1, NULL);
  EXPECT_TRUE(context2.get());
  EXPECT_FALSE(context2->IsGlobal());
  EXPECT_TRUE(context2->IsSame(context2));
  EXPECT_FALSE(context2->IsSame(context1));
  EXPECT_FALSE(context1->IsSame(context2));
  EXPECT_TRUE(context2->IsSharingWith(context2));
  EXPECT_TRUE(context2->IsSharingWith(context1));
  EXPECT_TRUE(context1->IsSharingWith(context2));

  CefRefPtr<CefRequestContext> context3 =
      CefRequestContext::CreateContext(context2, NULL);
  EXPECT_TRUE(context3.get());
  EXPECT_FALSE(context3->IsGlobal());
  EXPECT_TRUE(context3->IsSame(context3));
  EXPECT_FALSE(context3->IsSame(context2));
  EXPECT_FALSE(context3->IsSame(context1));
  EXPECT_FALSE(context1->IsSame(context3));
  EXPECT_FALSE(context2->IsSame(context3));
  EXPECT_TRUE(context3->IsSharingWith(context3));
  EXPECT_TRUE(context3->IsSharingWith(context2));
  EXPECT_TRUE(context3->IsSharingWith(context1));
  EXPECT_TRUE(context1->IsSharingWith(context3));
  EXPECT_TRUE(context2->IsSharingWith(context3));

  CefRefPtr<CefRequestContext> context4 =
      CefRequestContext::CreateContext(context1, NULL);
  EXPECT_TRUE(context4.get());
  EXPECT_FALSE(context4->IsGlobal());
  EXPECT_TRUE(context4->IsSame(context4));
  EXPECT_FALSE(context4->IsSame(context3));
  EXPECT_FALSE(context4->IsSame(context2));
  EXPECT_FALSE(context4->IsSame(context1));
  EXPECT_FALSE(context1->IsSame(context4));
  EXPECT_FALSE(context2->IsSame(context4));
  EXPECT_FALSE(context3->IsSame(context4));
  EXPECT_TRUE(context4->IsSharingWith(context4));
  EXPECT_TRUE(context4->IsSharingWith(context3));
  EXPECT_TRUE(context4->IsSharingWith(context2));
  EXPECT_TRUE(context4->IsSharingWith(context1));
  EXPECT_TRUE(context1->IsSharingWith(context4));
  EXPECT_TRUE(context2->IsSharingWith(context4));
  EXPECT_TRUE(context3->IsSharingWith(context4));
}

TEST(RequestContextTest, CreateContextSharedOnDisk) {
  base::ScopedTempDir tempdir;
  EXPECT_TRUE(tempdir.CreateUniqueTempDir());

  CefRequestContextSettings settings;
  CefString(&settings.cache_path) = tempdir.path().value();

  CefRefPtr<CefRequestContext> context1 =
      CefRequestContext::CreateContext(settings, NULL);
  EXPECT_TRUE(context1.get());
  EXPECT_FALSE(context1->IsGlobal());
  EXPECT_TRUE(context1->IsSame(context1));
  EXPECT_TRUE(context1->IsSharingWith(context1));

  CefRefPtr<CefRequestContext> context2 =
      CefRequestContext::CreateContext(context1, NULL);
  EXPECT_TRUE(context2.get());
  EXPECT_FALSE(context2->IsGlobal());
  EXPECT_TRUE(context2->IsSame(context2));
  EXPECT_FALSE(context2->IsSame(context1));
  EXPECT_FALSE(context1->IsSame(context2));
  EXPECT_TRUE(context2->IsSharingWith(context2));
  EXPECT_TRUE(context2->IsSharingWith(context1));
  EXPECT_TRUE(context1->IsSharingWith(context2));

  CefRefPtr<CefRequestContext> context3 =
      CefRequestContext::CreateContext(context2, NULL);
  EXPECT_TRUE(context3.get());
  EXPECT_FALSE(context3->IsGlobal());
  EXPECT_TRUE(context3->IsSame(context3));
  EXPECT_FALSE(context3->IsSame(context2));
  EXPECT_FALSE(context3->IsSame(context1));
  EXPECT_FALSE(context1->IsSame(context3));
  EXPECT_FALSE(context2->IsSame(context3));
  EXPECT_TRUE(context3->IsSharingWith(context3));
  EXPECT_TRUE(context3->IsSharingWith(context2));
  EXPECT_TRUE(context3->IsSharingWith(context1));
  EXPECT_TRUE(context1->IsSharingWith(context3));
  EXPECT_TRUE(context2->IsSharingWith(context3));

  CefRefPtr<CefRequestContext> context4 =
      CefRequestContext::CreateContext(context1, NULL);
  EXPECT_TRUE(context4.get());
  EXPECT_FALSE(context4->IsGlobal());
  EXPECT_TRUE(context4->IsSame(context4));
  EXPECT_FALSE(context4->IsSame(context3));
  EXPECT_FALSE(context4->IsSame(context2));
  EXPECT_FALSE(context4->IsSame(context1));
  EXPECT_FALSE(context1->IsSame(context4));
  EXPECT_FALSE(context2->IsSame(context4));
  EXPECT_FALSE(context3->IsSame(context4));
  EXPECT_TRUE(context4->IsSharingWith(context4));
  EXPECT_TRUE(context4->IsSharingWith(context3));
  EXPECT_TRUE(context4->IsSharingWith(context2));
  EXPECT_TRUE(context4->IsSharingWith(context1));
  EXPECT_TRUE(context1->IsSharingWith(context4));
  EXPECT_TRUE(context2->IsSharingWith(context4));
  EXPECT_TRUE(context3->IsSharingWith(context4));
}

namespace {

class CookieTestHandler : public TestHandler {
 public:
  class RequestContextHandler : public CefRequestContextHandler {
   public:
    explicit RequestContextHandler(CookieTestHandler* handler)
        : handler_(handler) {}

    CefRefPtr<CefCookieManager> GetCookieManager() override {
      EXPECT_TRUE(handler_);
      handler_->got_get_cookie_manager_.yes();
      return handler_->cookie_manager_;
    }

    void Detach() {
      handler_ = NULL;
    }

   private:
    CookieTestHandler* handler_;

    IMPLEMENT_REFCOUNTING(RequestContextHandler);
  };

  CookieTestHandler(const std::string& url)
      : url_(url) {}

  void RunTest() override {
    AddResource(url_,
        "<html>"
        "<head><script>document.cookie='name1=value1';</script></head>"
        "<body>Nav1</body>"
        "</html>", "text/html");

    CefRequestContextSettings settings;

    context_handler_ = new RequestContextHandler(this);
    context_ =
        CefRequestContext::CreateContext(settings, context_handler_.get());
    cookie_manager_ = CefCookieManager::CreateManager(CefString(), true, NULL);

    // Create browser that loads the 1st URL.
    CreateBrowser(url_, context_);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    CefRefPtr<CefRequestContext> context =
        browser->GetHost()->GetRequestContext();
    EXPECT_TRUE(context.get());
    EXPECT_TRUE(context->IsSame(context_));
    EXPECT_FALSE(context->IsGlobal());
    EXPECT_EQ(context->GetHandler().get(), context_handler_.get());

    FinishTest();
  }

 protected:
  void FinishTest() {
    // Verify that the cookie was set correctly.
    class TestVisitor : public CefCookieVisitor {
     public:
      explicit TestVisitor(CookieTestHandler* handler)
          : handler_(handler) {
      }
      ~TestVisitor() override {
        // Destroy the test.
        CefPostTask(TID_UI,
            base::Bind(&CookieTestHandler::DestroyTest, handler_));
      }

      bool Visit(const CefCookie& cookie, int count, int total,
                 bool& deleteCookie) override {
        const std::string& name = CefString(&cookie.name);
        const std::string& value = CefString(&cookie.value);
        if (name == "name1" && value == "value1")
          handler_->got_cookie_.yes();
        return true;
      }

     private:
      CookieTestHandler* handler_;
      IMPLEMENT_REFCOUNTING(TestVisitor);
    };

    cookie_manager_->VisitAllCookies(new TestVisitor(this));
  }

  void DestroyTest() override {
    // Verify test expectations.
    EXPECT_TRUE(got_get_cookie_manager_);
    EXPECT_TRUE(got_cookie_);

    context_handler_->Detach();
    context_handler_ = NULL;
    context_ = NULL;
    
    TestHandler::DestroyTest();
  }

  std::string url_;
  CefRefPtr<CefRequestContext> context_;
  CefRefPtr<RequestContextHandler> context_handler_;
  CefRefPtr<CefCookieManager> cookie_manager_;

  TrackCallback got_get_cookie_manager_;
  TrackCallback got_cookie_;

  IMPLEMENT_REFCOUNTING(CookieTestHandler);
};

}  // namespace

// Test that the cookie manager is retrieved via the associated request context.
TEST(RequestContextTest, GetCookieManager) {
  CefRefPtr<CookieTestHandler> handler =
      new CookieTestHandler(
          "http://tests-simple-rch.com/nav1.html");
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

namespace {

class PopupTestHandler : public TestHandler {
 public:
  class RequestContextHandler : public CefRequestContextHandler {
   public:
    explicit RequestContextHandler(PopupTestHandler* handler)
        : handler_(handler) {}

    CefRefPtr<CefCookieManager> GetCookieManager() override {
      EXPECT_TRUE(handler_);
      if (url_ == handler_->url_)
        handler_->got_get_cookie_manager1_.yes();
      else if (url_ == handler_->popup_url_)
        handler_->got_get_cookie_manager2_.yes();
      return handler_->cookie_manager_;
    }

    void SetURL(const std::string& url) {
      url_ = url;
    }

    void Detach() {
      handler_ = NULL;
    }

   private:
    PopupTestHandler* handler_;
    std::string url_;

    IMPLEMENT_REFCOUNTING(RequestContextHandler);
  };

  enum Mode {
    MODE_WINDOW_OPEN,
    MODE_TARGETED_LINK,
    MODE_NOREFERRER_LINK,
  };

  PopupTestHandler(bool same_origin,
                   Mode mode)
      : mode_(mode) {
    url_ = "http://tests-simple-rch1.com/nav1.html";
    if (same_origin)
      popup_url_ = "http://tests-simple-rch1.com/pop1.html";
    else
      popup_url_ = "http://tests-simple-rch2.com/pop1.html";
  }

  void RunTest() override {
    std::string link;
    if (mode_ == MODE_TARGETED_LINK) {
      link = "<a href=\"" + std::string(popup_url_) +
              "\" target=\"mytarget\"\">CLICK ME</a>";
    } else if (mode_ == MODE_NOREFERRER_LINK) {
      link = "<a href=\"" + std::string(popup_url_) +
              "\" rel=\"noreferrer\" target=\"_blank\"\">CLICK ME</a>";
    }

    AddResource(url_,
        "<html>"
        "<head><script>document.cookie='name1=value1';"
        "function doPopup() { window.open('" + std::string(popup_url_) + "'); }"
        "</script></head>"
        "<body><h1>" + link + "</h1></body>"
        "</html>", "text/html");

    AddResource(popup_url_,
        "<html>"
        "<head><script>document.cookie='name2=value2';</script></head>"
        "<body>Nav1</body>"
        "</html>", "text/html");

    CefRequestContextSettings settings;

    context_handler_ = new RequestContextHandler(this);
    context_handler_->SetURL(url_);
    context_ =
        CefRequestContext::CreateContext(settings, context_handler_.get());
    cookie_manager_ = CefCookieManager::CreateManager(CefString(), true, NULL);

    // Create browser that loads the 1st URL.
    CreateBrowser(url_, context_);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    CefRefPtr<CefRequestContext> context =
        browser->GetHost()->GetRequestContext();
    EXPECT_TRUE(context.get());
    EXPECT_TRUE(context->IsSame(context_));
    EXPECT_FALSE(context->IsGlobal());

    EXPECT_TRUE(frame->IsMain());

    const std::string& url = frame->GetURL();
    if (url == url_) {
      got_load_end1_.yes();
      context_handler_->SetURL(popup_url_);
      LaunchPopup(browser);
    } if (url == popup_url_) {
      got_load_end2_.yes();
      EXPECT_TRUE(browser->IsPopup());
      // Close the popup window.
      browser->GetHost()->CloseBrowser(true);
    }
  }

  bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     const CefString& target_url,
                     const CefString& target_frame_name,
                     cef_window_open_disposition_t target_disposition,
                     bool user_gesture,
                     const CefPopupFeatures& popupFeatures,
                     CefWindowInfo& windowInfo,
                     CefRefPtr<CefClient>& client,
                     CefBrowserSettings& settings,
                     bool* no_javascript_access) override {
    got_on_before_popup_.yes();

    const std::string& url = target_url;
    EXPECT_STREQ(url.c_str(), popup_url_.c_str());

    EXPECT_EQ(WOD_NEW_FOREGROUND_TAB, target_disposition);

    if (mode_ == MODE_WINDOW_OPEN)
      EXPECT_FALSE(user_gesture);
    else
      EXPECT_TRUE(user_gesture);

    return false;
  }

  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
    TestHandler::OnBeforeClose(browser);

    if (browser->IsPopup())
      FinishTest();
  }

 protected:
  void LaunchPopup(CefRefPtr<CefBrowser> browser) {
    if (mode_ == MODE_WINDOW_OPEN) {
      browser->GetMainFrame()->ExecuteJavaScript("doPopup()", url_, 0);
    } else if (mode_ == MODE_TARGETED_LINK ||
               mode_ == MODE_NOREFERRER_LINK) {
      CefMouseEvent mouse_event;
      mouse_event.x = 20;
      mouse_event.y = 20;
      mouse_event.modifiers = 0;
      browser->GetHost()->SendMouseClickEvent(
          mouse_event, MBT_LEFT, false, 1);
      browser->GetHost()->SendMouseClickEvent(
          mouse_event, MBT_LEFT, true, 1);
    } else {
      EXPECT_TRUE(false);  // Not reached.
    }
  }

  void FinishTest() {
    // Verify that the cookies were set correctly.
    class TestVisitor : public CefCookieVisitor {
     public:
      explicit TestVisitor(PopupTestHandler* handler)
          : handler_(handler) {
      }
      ~TestVisitor() override {
        // Destroy the test.
        CefPostTask(TID_UI,
            base::Bind(&PopupTestHandler::DestroyTest, handler_));
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
      PopupTestHandler* handler_;
      IMPLEMENT_REFCOUNTING(TestVisitor);
    };

    cookie_manager_->VisitAllCookies(new TestVisitor(this));
  }

  void DestroyTest() override {
    // Verify test expectations.
    EXPECT_TRUE(got_get_cookie_manager1_);
    EXPECT_TRUE(got_load_end1_);
    EXPECT_TRUE(got_on_before_popup_);
    EXPECT_TRUE(got_get_cookie_manager2_);
    EXPECT_TRUE(got_load_end2_);
    EXPECT_TRUE(got_cookie1_);
    EXPECT_TRUE(got_cookie2_);

    context_handler_->Detach();
    context_handler_ = NULL;
    context_ = NULL;
    
    TestHandler::DestroyTest();
  }

  std::string url_;
  std::string popup_url_;
  Mode mode_;

  CefRefPtr<CefRequestContext> context_;
  CefRefPtr<RequestContextHandler> context_handler_;
  CefRefPtr<CefCookieManager> cookie_manager_;

  TrackCallback got_get_cookie_manager1_;
  TrackCallback got_load_end1_;
  TrackCallback got_on_before_popup_;
  TrackCallback got_get_cookie_manager2_;
  TrackCallback got_load_end2_;
  TrackCallback got_cookie1_;
  TrackCallback got_cookie2_;

  IMPLEMENT_REFCOUNTING(PopupTestHandler);
};

}  // namespace

// Test that a popup created using window.open() will get the same request
// context as the parent browser.
TEST(RequestContextTest, WindowOpenSameOrigin) {
  CefRefPtr<PopupTestHandler> handler =
      new PopupTestHandler(true,
          PopupTestHandler::MODE_WINDOW_OPEN);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(RequestContextTest, WindowOpenDifferentOrigin) {
  CefRefPtr<PopupTestHandler> handler =
      new PopupTestHandler(false,
          PopupTestHandler::MODE_WINDOW_OPEN);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

// Test that a popup created using a targeted link will get the same request
// context as the parent browser.
TEST(RequestContextTest, TargetedLinkSameOrigin) {
  CefRefPtr<PopupTestHandler> handler =
      new PopupTestHandler(true,
          PopupTestHandler::MODE_TARGETED_LINK);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(RequestContextTest, TargetedLinkDifferentOrigin) {
  CefRefPtr<PopupTestHandler> handler =
      new PopupTestHandler(false,
          PopupTestHandler::MODE_TARGETED_LINK);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}


// Test that a popup created using a noreferrer link will get the same
// request context as the parent browser. A new render process will
// be created for the popup browser.
TEST(RequestContextTest, NoReferrerLinkSameOrigin) {
  CefRefPtr<PopupTestHandler> handler =
      new PopupTestHandler(true,
          PopupTestHandler::MODE_NOREFERRER_LINK);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(RequestContextTest, NoReferrerLinkDifferentOrigin) {
  CefRefPtr<PopupTestHandler> handler =
      new PopupTestHandler(false,
          PopupTestHandler::MODE_NOREFERRER_LINK);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}
