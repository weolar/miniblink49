// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <vector>

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "base/files/scoped_temp_dir.h"
#include "base/synchronization/waitable_event.h"

#include "include/base/cef_bind.h"
#include "include/base/cef_logging.h"
#include "include/base/cef_ref_counted.h"
#include "include/cef_cookie.h"
#include "include/cef_scheme.h"
#include "include/wrapper/cef_closure_task.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/test_handler.h"
#include "tests/unittests/test_suite.h"

namespace {

const char* kTestUrl = "http://www.test.com/path/to/cookietest/foo.html";
const char* kTestDomain = "www.test.com";
const char* kTestPath = "/path/to/cookietest";

const int kIgnoreNumDeleted = -2;

typedef std::vector<CefCookie> CookieVector;

class TestCompletionCallback : public CefCompletionCallback {
 public:
  explicit TestCompletionCallback(base::WaitableEvent* event)
    : event_(event) {}

  void OnComplete() override {
    event_->Signal();
  }

 private:
  base::WaitableEvent* event_;

  IMPLEMENT_REFCOUNTING(TestCompletionCallback);
  DISALLOW_COPY_AND_ASSIGN(TestCompletionCallback);
};

class TestSetCookieCallback : public CefSetCookieCallback {
 public:
  TestSetCookieCallback(bool expected_success,
                       base::WaitableEvent* event)
    : expected_success_(expected_success),
      event_(event) {}

  void OnComplete(bool success) override {
    EXPECT_EQ(expected_success_, success);
    event_->Signal();
  }

 private:
  bool expected_success_;
  base::WaitableEvent* event_;

  IMPLEMENT_REFCOUNTING(TestSetCookieCallback);
  DISALLOW_COPY_AND_ASSIGN(TestSetCookieCallback);
};

class TestDeleteCookiesCallback : public CefDeleteCookiesCallback {
 public:
  TestDeleteCookiesCallback(int expected_num_deleted,
                            base::WaitableEvent* event)
    : expected_num_deleted_(expected_num_deleted),
      event_(event) {}

  void OnComplete(int num_deleted) override {
    if (expected_num_deleted_ != kIgnoreNumDeleted)
      EXPECT_EQ(expected_num_deleted_, num_deleted);
    event_->Signal();
  }

 private:
  int expected_num_deleted_;
  base::WaitableEvent* event_;

  IMPLEMENT_REFCOUNTING(TestDeleteCookiesCallback);
  DISALLOW_COPY_AND_ASSIGN(TestDeleteCookiesCallback);
};

class TestVisitor : public CefCookieVisitor {
 public:
  TestVisitor(CookieVector* cookies, bool deleteCookies,
              base::WaitableEvent* event)
    : cookies_(cookies),
      delete_cookies_(deleteCookies),
      event_(event) {
  }
  ~TestVisitor() override {
    event_->Signal();
  }

  bool Visit(const CefCookie& cookie, int count, int total,
             bool& deleteCookie) override {
    cookies_->push_back(cookie);
    if (delete_cookies_)
      deleteCookie = true;
    return true;
  }

  CookieVector* cookies_;
  bool delete_cookies_;
  base::WaitableEvent* event_;

  IMPLEMENT_REFCOUNTING(TestVisitor);
};

// Set the cookies.
void SetCookies(CefRefPtr<CefCookieManager> manager,
                const CefString& url, const CookieVector& cookies,
                bool expected_success,
                base::WaitableEvent& event) {
  CookieVector::const_iterator it = cookies.begin();
  for (; it != cookies.end(); ++it) {
    EXPECT_TRUE(manager->SetCookie(
        url, *it, new TestSetCookieCallback(expected_success, &event)));
    event.Wait();
  }
}

// Delete the cookie.
void DeleteCookies(CefRefPtr<CefCookieManager> manager,
                   const CefString& url, const CefString& cookie_name,
                   int expected_num_deleted,
                   base::WaitableEvent& event) {
  EXPECT_TRUE(manager->DeleteCookies(
      url, cookie_name,
      new TestDeleteCookiesCallback(expected_num_deleted, &event)));
  event.Wait();
}

// Create a test cookie. If |withDomain| is true a domain cookie will be
// created, otherwise a host cookie will be created.
void CreateCookie(CefRefPtr<CefCookieManager> manager,
                  CefCookie& cookie, bool withDomain,
                  bool sessionCookie,
                  base::WaitableEvent& event) {
  CefString(&cookie.name).FromASCII("my_cookie");
  CefString(&cookie.value).FromASCII("My Value");
  if (withDomain)
    CefString(&cookie.domain).FromASCII(kTestDomain);
  CefString(&cookie.path).FromASCII(kTestPath);
  if (!sessionCookie) {
    cookie.has_expires = true;
    cookie.expires.year = 2200;
    cookie.expires.month = 4;
    cookie.expires.day_of_week = 5;
    cookie.expires.day_of_month = 11;
  }

  CookieVector cookies;
  cookies.push_back(cookie);

  SetCookies(manager, kTestUrl, cookies, true, event);
}

// Retrieve the test cookie. If |withDomain| is true check that the cookie
// is a domain cookie, otherwise a host cookie. if |deleteCookies| is true
// the cookie will be deleted when it's retrieved.
void GetCookie(CefRefPtr<CefCookieManager> manager,
               const CefCookie& cookie, bool withDomain,
               base::WaitableEvent& event, bool deleteCookies) {
  CookieVector cookies;

  // Get the cookie and delete it.
  EXPECT_TRUE(manager->VisitUrlCookies(kTestUrl, false,
      new TestVisitor(&cookies, deleteCookies, &event)));
  event.Wait();

  EXPECT_EQ(1U, cookies.size());
  if (cookies.size() != 1U)
    return;

  const CefCookie& cookie_read = cookies[0];
  EXPECT_EQ(CefString(&cookie_read.name), "my_cookie");
  EXPECT_EQ(CefString(&cookie_read.value), "My Value");
  if (withDomain)
    EXPECT_EQ(CefString(&cookie_read.domain), ".www.test.com");
  else
    EXPECT_EQ(CefString(&cookie_read.domain), kTestDomain);
  EXPECT_EQ(CefString(&cookie_read.path), kTestPath);
  EXPECT_EQ(cookie.has_expires, cookie_read.has_expires);
  EXPECT_EQ(cookie.expires.year, cookie_read.expires.year);
  EXPECT_EQ(cookie.expires.month, cookie_read.expires.month);
  EXPECT_EQ(cookie.expires.day_of_week, cookie_read.expires.day_of_week);
  EXPECT_EQ(cookie.expires.day_of_month, cookie_read.expires.day_of_month);
  EXPECT_EQ(cookie.expires.hour, cookie_read.expires.hour);
  EXPECT_EQ(cookie.expires.minute, cookie_read.expires.minute);
  EXPECT_EQ(cookie.expires.second, cookie_read.expires.second);
  EXPECT_EQ(cookie.expires.millisecond, cookie_read.expires.millisecond);
}

// Visit URL cookies.
void VisitUrlCookies(CefRefPtr<CefCookieManager> manager,
                     const CefString& url,
                     bool includeHttpOnly,
                     CookieVector& cookies,
                     bool deleteCookies,
                     base::WaitableEvent& event) {
  EXPECT_TRUE(manager->VisitUrlCookies(url, includeHttpOnly,
      new TestVisitor(&cookies, deleteCookies, &event)));
  event.Wait();
}

// Visit all cookies.
void VisitAllCookies(CefRefPtr<CefCookieManager> manager,
                     CookieVector& cookies,
                     bool deleteCookies,
                     base::WaitableEvent& event) {
  EXPECT_TRUE(manager->VisitAllCookies(
      new TestVisitor(&cookies, deleteCookies, &event)));
  event.Wait();
}

// Verify that no cookies exist. If |withUrl| is true it will only check for
// cookies matching the URL.
void VerifyNoCookies(CefRefPtr<CefCookieManager> manager,
                     base::WaitableEvent& event, bool withUrl) {
  CookieVector cookies;

  // Verify that the cookie has been deleted.
  if (withUrl) {
    EXPECT_TRUE(manager->VisitUrlCookies(kTestUrl, false,
        new TestVisitor(&cookies, false, &event)));
  } else {
    EXPECT_TRUE(manager->VisitAllCookies(
        new TestVisitor(&cookies, false, &event)));
  }
  event.Wait();

  EXPECT_EQ(0U, cookies.size());
}

// Delete all system cookies.
void DeleteAllCookies(CefRefPtr<CefCookieManager> manager,
                      base::WaitableEvent& event) {
  DeleteCookies(manager, CefString(), CefString(), kIgnoreNumDeleted, event);
}

void TestDomainCookie(CefRefPtr<CefCookieManager> manager,
                      base::WaitableEvent& event) {
  CefCookie cookie;

  // Create a domain cookie.
  CreateCookie(manager, cookie, true, false, event);

  // Retrieve, verify and delete the domain cookie.
  GetCookie(manager, cookie, true, event, true);

  // Verify that the cookie was deleted.
  VerifyNoCookies(manager, event, true);
}

void TestHostCookie(CefRefPtr<CefCookieManager> manager,
                    base::WaitableEvent& event) {
  CefCookie cookie;

  // Create a host cookie.
  CreateCookie(manager, cookie, false, false, event);

  // Retrieve, verify and delete the host cookie.
  GetCookie(manager, cookie, false, event, true);

  // Verify that the cookie was deleted.
  VerifyNoCookies(manager, event, true);
}

void TestMultipleCookies(CefRefPtr<CefCookieManager> manager,
                         base::WaitableEvent& event) {
  std::stringstream ss;
  int i;

  CookieVector cookies;

  const int kNumCookies = 4;

  // Create the cookies.
  for (i = 0; i < kNumCookies; i++) {
    CefCookie cookie;

    ss << "my_cookie" << i;
    CefString(&cookie.name).FromASCII(ss.str().c_str());
    ss.str("");
    ss << "My Value " << i;
    CefString(&cookie.value).FromASCII(ss.str().c_str());
    ss.str("");

    cookies.push_back(cookie);
  }

  // Set the cookies.
  SetCookies(manager, kTestUrl, cookies, true, event);
  cookies.clear();

  // Get the cookies without deleting them.
  VisitUrlCookies(manager, kTestUrl, false, cookies, false, event);

  EXPECT_EQ((CookieVector::size_type)kNumCookies, cookies.size());

  CookieVector::const_iterator it = cookies.begin();
  for (i = 0; it != cookies.end(); ++it, ++i) {
    const CefCookie& cookie = *it;

    ss << "my_cookie" << i;
    EXPECT_EQ(CefString(&cookie.name), ss.str());
    ss.str("");
    ss << "My Value " << i;
    EXPECT_EQ(CefString(&cookie.value), ss.str());
    ss.str("");
  }

  cookies.clear();

  // Delete the 2nd cookie.
  DeleteCookies(manager, kTestUrl, CefString("my_cookie1"), -1, event);

  // Verify that the cookie has been deleted.
  VisitUrlCookies(manager, kTestUrl, false, cookies, false, event);

  EXPECT_EQ(3U, cookies.size());
  if (cookies.size() != 3U)
    return;

  EXPECT_EQ(CefString(&cookies[0].name), "my_cookie0");
  EXPECT_EQ(CefString(&cookies[1].name), "my_cookie2");
  EXPECT_EQ(CefString(&cookies[2].name), "my_cookie3");

  cookies.clear();

  // Delete the rest of the cookies.
  DeleteCookies(manager, kTestUrl, CefString(), 3, event);

  // Verify that the cookies have been deleted.
  VisitUrlCookies(manager, kTestUrl, false, cookies, false, event);

  EXPECT_EQ(0U, cookies.size());

  // Create the cookies.
  for (i = 0; i < kNumCookies; i++) {
    CefCookie cookie;

    ss << "my_cookie" << i;
    CefString(&cookie.name).FromASCII(ss.str().c_str());
    ss.str("");
    ss << "My Value " << i;
    CefString(&cookie.value).FromASCII(ss.str().c_str());
    ss.str("");

    cookies.push_back(cookie);
  }

  // Delete all of the cookies using the visitor.
  VisitUrlCookies(manager, kTestUrl, false, cookies, true, event);

  cookies.clear();

  // Verify that the cookies have been deleted.
  VisitUrlCookies(manager, kTestUrl, false, cookies, false, event);

  EXPECT_EQ(0U, cookies.size());
}

void TestAllCookies(CefRefPtr<CefCookieManager> manager,
                    base::WaitableEvent& event) {
  CookieVector cookies;

  // Delete all system cookies just in case something is left over from a
  // different test.
  DeleteAllCookies(manager, event);

  // Verify that all system cookies have been deleted.
  VisitAllCookies(manager, cookies, false, event);

  EXPECT_EQ(0U, cookies.size());

  // Create cookies with 2 separate hosts.
  CefCookie cookie1;
  const char* kUrl1 = "http://www.foo.com";
  CefString(&cookie1.name).FromASCII("my_cookie1");
  CefString(&cookie1.value).FromASCII("My Value 1");

  cookies.push_back(cookie1);
  SetCookies(manager,  kUrl1, cookies, true, event);
  cookies.clear();

  CefCookie cookie2;
  const char* kUrl2 = "http://www.bar.com";
  CefString(&cookie2.name).FromASCII("my_cookie2");
  CefString(&cookie2.value).FromASCII("My Value 2");

  cookies.push_back(cookie2);
  SetCookies(manager,  kUrl2, cookies, true, event);
  cookies.clear();

  // Verify that all system cookies can be retrieved.
  VisitAllCookies(manager, cookies, false, event);

  EXPECT_EQ(2U, cookies.size());
  if (cookies.size() != 2U)
    return;

  EXPECT_EQ(CefString(&cookies[0].name), "my_cookie1");
  EXPECT_EQ(CefString(&cookies[0].value), "My Value 1");
  EXPECT_EQ(CefString(&cookies[0].domain), "www.foo.com");
  EXPECT_EQ(CefString(&cookies[1].name), "my_cookie2");
  EXPECT_EQ(CefString(&cookies[1].value), "My Value 2");
  EXPECT_EQ(CefString(&cookies[1].domain), "www.bar.com");
  cookies.clear();

  // Verify that the cookies can be retrieved separately.
  VisitUrlCookies(manager, kUrl1, false, cookies, false, event);

  EXPECT_EQ(1U, cookies.size());
  if (cookies.size() != 1U)
    return;

  EXPECT_EQ(CefString(&cookies[0].name), "my_cookie1");
  EXPECT_EQ(CefString(&cookies[0].value), "My Value 1");
  EXPECT_EQ(CefString(&cookies[0].domain), "www.foo.com");
  cookies.clear();

  VisitUrlCookies(manager, kUrl2, false, cookies, false, event);

  EXPECT_EQ(1U, cookies.size());
  if (cookies.size() != 1U)
    return;

  EXPECT_EQ(CefString(&cookies[0].name), "my_cookie2");
  EXPECT_EQ(CefString(&cookies[0].value), "My Value 2");
  EXPECT_EQ(CefString(&cookies[0].domain), "www.bar.com");
  cookies.clear();

  // Delete all of the system cookies.
  DeleteAllCookies(manager, event);

  // Verify that all system cookies have been deleted.
  VerifyNoCookies(manager, event, false);
}

void TestChangeDirectory(CefRefPtr<CefCookieManager> manager,
                         base::WaitableEvent& event,
                         const CefString& original_dir) {
  CefCookie cookie;

  base::ScopedTempDir temp_dir;

  // Create a new temporary directory.
  EXPECT_TRUE(temp_dir.CreateUniqueTempDir());

  // Delete all of the system cookies.
  DeleteAllCookies(manager, event);

  // Set the new temporary directory as the storage location.
  EXPECT_TRUE(manager->SetStoragePath(temp_dir.path().value(), false, NULL));

  // Wait for the storage location change to complete on the IO thread.
  WaitForIOThread();

  // Verify that no cookies exist.
  VerifyNoCookies(manager, event, true);

  // Create a domain cookie.
  CreateCookie(manager, cookie, true, false, event);

  // Retrieve and verify the domain cookie.
  GetCookie(manager, cookie, true, event, false);

  // Restore the original storage location.
  EXPECT_TRUE(manager->SetStoragePath(original_dir, false, NULL));

  // Wait for the storage location change to complete on the IO thread.
  WaitForIOThread();

  // Verify that no cookies exist.
  VerifyNoCookies(manager, event, true);

  // Set the new temporary directory as the storage location.
  EXPECT_TRUE(manager->SetStoragePath(temp_dir.path().value(), false, NULL));

  // Wait for the storage location change to complete on the IO thread.
  WaitForIOThread();

  // Retrieve and verify the domain cookie that was set previously.
  GetCookie(manager, cookie, true, event, false);

  // Restore the original storage location.
  EXPECT_TRUE(manager->SetStoragePath(original_dir, false, NULL));

  // Wait for the storage location change to complete on the IO thread.
  WaitForIOThread();
}

}  // namespace

// Test creation of a domain cookie.
TEST(CookieTest, DomainCookieGlobal) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::GetGlobalManager(new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestDomainCookie(manager, event);
}

// Test creation of a domain cookie.
TEST(CookieTest, DomainCookieInMemory) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(CefString(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestDomainCookie(manager, event);
}

// Test creation of a domain cookie.
TEST(CookieTest, DomainCookieOnDisk) {
  base::ScopedTempDir temp_dir;

  // Create a new temporary directory.
  EXPECT_TRUE(temp_dir.CreateUniqueTempDir());

  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(temp_dir.path().value(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestDomainCookie(manager, event);
}

// Test creation of a host cookie.
TEST(CookieTest, HostCookieGlobal) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::GetGlobalManager(new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestHostCookie(manager, event);
}

// Test creation of a host cookie.
TEST(CookieTest, HostCookieInMemory) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(CefString(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestHostCookie(manager, event);
}

// Test creation of a host cookie.
TEST(CookieTest, HostCookieOnDisk) {
  base::ScopedTempDir temp_dir;

  // Create a new temporary directory.
  EXPECT_TRUE(temp_dir.CreateUniqueTempDir());

  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(temp_dir.path().value(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestHostCookie(manager, event);
}

// Test creation of multiple cookies.
TEST(CookieTest, MultipleCookiesGlobal) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::GetGlobalManager(new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestMultipleCookies(manager, event);
}

// Test creation of multiple cookies.
TEST(CookieTest, MultipleCookiesInMemory) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(CefString(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestMultipleCookies(manager, event);
}

// Test creation of multiple cookies.
TEST(CookieTest, MultipleCookiesOnDisk) {
  base::ScopedTempDir temp_dir;

  // Create a new temporary directory.
  EXPECT_TRUE(temp_dir.CreateUniqueTempDir());

  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(temp_dir.path().value(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestMultipleCookies(manager, event);
}

TEST(CookieTest, AllCookiesGlobal) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::GetGlobalManager(new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestAllCookies(manager, event);
}

TEST(CookieTest, AllCookiesInMemory) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(CefString(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestAllCookies(manager, event);
}

TEST(CookieTest, AllCookiesOnDisk) {
  base::ScopedTempDir temp_dir;

  // Create a new temporary directory.
  EXPECT_TRUE(temp_dir.CreateUniqueTempDir());

  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(temp_dir.path().value(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestAllCookies(manager, event);
}

TEST(CookieTest, ChangeDirectoryGlobal) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::GetGlobalManager(new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  std::string cache_path;
  CefTestSuite::GetCachePath(cache_path);

  TestChangeDirectory(manager, event, cache_path);
}

TEST(CookieTest, ChangeDirectoryCreated) {
  base::WaitableEvent event(false, false);

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(CefString(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  TestChangeDirectory(manager, event, CefString());
}

TEST(CookieTest, SessionCookieNoPersist) {
  base::ScopedTempDir temp_dir;
  base::WaitableEvent event(false, false);
  CefCookie cookie;

  // Create a new temporary directory.
  EXPECT_TRUE(temp_dir.CreateUniqueTempDir());

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(temp_dir.path().value(), false,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  // Create a session cookie.
  CreateCookie(manager, cookie, true, true, event);

  // Retrieve and verify the cookie.
  GetCookie(manager, cookie, true, event, false);

  // Flush the cookie store to disk.
  manager->FlushStore(new TestCompletionCallback(&event));
  event.Wait();
 
  // Create a new manager to read the same cookie store.
  manager = CefCookieManager::CreateManager(temp_dir.path().value(), false,
                                            new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  // Verify that the cookie doesn't exist.
  VerifyNoCookies(manager, event, true);
}

TEST(CookieTest, SessionCookieWillPersist) {
  base::ScopedTempDir temp_dir;
  base::WaitableEvent event(false, false);
  CefCookie cookie;

  // Create a new temporary directory.
  EXPECT_TRUE(temp_dir.CreateUniqueTempDir());

  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::CreateManager(temp_dir.path().value(), true,
                                      new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  // Create a session cookie.
  CreateCookie(manager, cookie, true, true, event);

  // Retrieve and verify the cookie.
  GetCookie(manager, cookie, true, event, false);

  // Flush the cookie store to disk.
  manager->FlushStore(new TestCompletionCallback(&event));
  event.Wait();
  
  // Create a new manager to read the same cookie store.
  manager = CefCookieManager::CreateManager(temp_dir.path().value(), true,
                                            new TestCompletionCallback(&event));
  event.Wait();
  EXPECT_TRUE(manager.get());

  // Verify that the cookie exists.
  GetCookie(manager, cookie, true, event, false);
}


namespace {

const char* kCookieJSUrl1 = "http://tests/cookie1.html";
const char* kCookieJSUrl2 = "http://tests/cookie2.html";

class CookieTestJSHandler : public TestHandler {
 public:
  class RequestContextHandler : public CefRequestContextHandler {
   public:
    explicit RequestContextHandler(CookieTestJSHandler* handler)
        : handler_(handler) {}

    CefRefPtr<CefCookieManager> GetCookieManager() override {
      EXPECT_TRUE(handler_);
      EXPECT_TRUE(CefCurrentlyOn(TID_IO));

      if (url_ == kCookieJSUrl1) {
        // Return the first cookie manager.
        handler_->got_cookie_manager1_.yes();
        return handler_->manager1_;
      } else {
        // Return the second cookie manager.
        handler_->got_cookie_manager2_.yes();
        return handler_->manager2_;
      }
    }

    void SetURL(const std::string& url) {
      url_ = url;
    }

    void Detach() {
      handler_ = NULL;
    }

   private:
    std::string url_;
    CookieTestJSHandler* handler_;

    IMPLEMENT_REFCOUNTING(RequestContextHandler);
  };

  CookieTestJSHandler() {}

  void RunTest() override {
    // Create new in-memory managers.
    manager1_ = CefCookieManager::CreateManager(CefString(), false, NULL);
    manager2_ = CefCookieManager::CreateManager(CefString(), false, NULL);

    std::string page =
        "<html><head>"
        "<script>"
        "document.cookie='name1=value1';"
        "</script>"
        "</head><body>COOKIE TEST1</body></html>";
    AddResource(kCookieJSUrl1, page, "text/html");

    page =
        "<html><head>"
        "<script>"
        "document.cookie='name2=value2';"
        "</script>"
        "</head><body>COOKIE TEST2</body></html>";
    AddResource(kCookieJSUrl2, page, "text/html");

    context_handler_ = new RequestContextHandler(this);
    context_handler_->SetURL(kCookieJSUrl1);

    // Create the request context that will use an in-memory cache.
    CefRequestContextSettings settings;
    CefRefPtr<CefRequestContext> request_context =
        CefRequestContext::CreateContext(settings, context_handler_.get());

    // Create the browser.
    CreateBrowser(kCookieJSUrl1, request_context);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    std::string url = frame->GetURL();
    if (url == kCookieJSUrl1) {
      got_load_end1_.yes();
      VerifyCookie(manager1_, url, "name1", "value1", got_cookie1_);

      // Go to the next URL
      context_handler_->SetURL(kCookieJSUrl2);
      frame->LoadURL(kCookieJSUrl2);
    } else {
      got_load_end2_.yes();
      VerifyCookie(manager2_, url, "name2", "value2", got_cookie2_);

      DestroyTest();
    }
  }

  void DestroyTest() override {
    context_handler_->Detach();
    context_handler_ = NULL;

    TestHandler::DestroyTest();
  }

  // Verify that the cookie was set successfully.
  void VerifyCookie(CefRefPtr<CefCookieManager> manager,
                    const std::string& url,
                    const std::string& name,
                    const std::string& value,
                    TrackCallback& callback) {
    base::WaitableEvent event(false, false);
    CookieVector cookies;

    // Get the cookie.
    VisitUrlCookies(manager, url, false, cookies, false, event);

    if (cookies.size() == 1U && CefString(&cookies[0].name) == name &&
        CefString(&cookies[0].value) == value) {
      callback.yes();
    }
  }

  CefRefPtr<RequestContextHandler> context_handler_;

  CefRefPtr<CefCookieManager> manager1_;
  CefRefPtr<CefCookieManager> manager2_;

  TrackCallback got_cookie_manager1_;
  TrackCallback got_cookie_manager2_;
  TrackCallback got_load_end1_;
  TrackCallback got_load_end2_;
  TrackCallback got_cookie1_;
  TrackCallback got_cookie2_;

  IMPLEMENT_REFCOUNTING(CookieTestJSHandler);
};

}  // namespace

// Verify use of multiple cookie managers vis JS.
TEST(CookieTest, GetCookieManagerJS) {
  CefRefPtr<CookieTestJSHandler> handler = new CookieTestJSHandler();
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_cookie_manager1_);
  EXPECT_TRUE(handler->got_cookie_manager2_);
  EXPECT_TRUE(handler->got_load_end1_);
  EXPECT_TRUE(handler->got_load_end2_);
  EXPECT_TRUE(handler->got_cookie1_);
  EXPECT_TRUE(handler->got_cookie2_);

  ReleaseAndWaitForDestructor(handler);
}


namespace {

class CookieTestSchemeHandler : public TestHandler {
 public:
  class SchemeHandler : public CefResourceHandler {
   public:
    explicit SchemeHandler(CookieTestSchemeHandler* handler)
        : handler_(handler),
          offset_(0) {}

    bool ProcessRequest(CefRefPtr<CefRequest> request,
                        CefRefPtr<CefCallback> callback) override {
      std::string url = request->GetURL();
      if (url == handler_->url1_) {
        content_ = "<html><body>COOKIE TEST1</body></html>";
        cookie_ = "name1=value1";
        handler_->got_process_request1_.yes();
      } else if (url == handler_->url2_) {
        content_ = "<html><body>COOKIE TEST2</body></html>";
        cookie_ = "name2=value2";
        handler_->got_process_request2_.yes();
      } else if (url == handler_->url3_) {
        content_ = "<html><body>COOKIE TEST3</body></html>";
        handler_->got_process_request3_.yes();

        // Verify that the cookie was passed in.
        CefRequest::HeaderMap headerMap;
        request->GetHeaderMap(headerMap);
        CefRequest::HeaderMap::iterator it = headerMap.find("Cookie");
        if (it != headerMap.end() && it->second == "name2=value2")
          handler_->got_process_request_cookie_.yes();

      }
      callback->Continue();
      return true;
    }

    void GetResponseHeaders(CefRefPtr<CefResponse> response,
                            int64& response_length,
                            CefString& redirectUrl) override {
      response_length = content_.size();

      response->SetStatus(200);
      response->SetMimeType("text/html");

      if (!cookie_.empty()) {
        CefResponse::HeaderMap headerMap;
        response->GetHeaderMap(headerMap);
        headerMap.insert(std::make_pair("Set-Cookie", cookie_));
        response->SetHeaderMap(headerMap);
      }
    }

    bool ReadResponse(void* data_out,
                      int bytes_to_read,
                      int& bytes_read,
                      CefRefPtr<CefCallback> callback) override {
      bool has_data = false;
      bytes_read = 0;

      size_t size = content_.size();
      if (offset_ < size) {
        int transfer_size =
            std::min(bytes_to_read, static_cast<int>(size - offset_));
        memcpy(data_out, content_.c_str() + offset_, transfer_size);
        offset_ += transfer_size;

        bytes_read = transfer_size;
        has_data = true;
      }

      return has_data;
    }

    void Cancel() override {
    }

   private:
    CookieTestSchemeHandler* handler_;
    std::string content_;
    size_t offset_;
    std::string cookie_;

    IMPLEMENT_REFCOUNTING(SchemeHandler);
  };

  class SchemeHandlerFactory : public CefSchemeHandlerFactory {
   public:
    explicit SchemeHandlerFactory(CookieTestSchemeHandler* handler)
        : handler_(handler) {}

    CefRefPtr<CefResourceHandler> Create(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& scheme_name,
        CefRefPtr<CefRequest> request) override {
      std::string url = request->GetURL();
      if (url == handler_->url3_) {
        // Verify that the cookie was not passed in.
        CefRequest::HeaderMap headerMap;
        request->GetHeaderMap(headerMap);
        CefRequest::HeaderMap::iterator it = headerMap.find("Cookie");
        if (it != headerMap.end() && it->second == "name2=value2")
          handler_->got_create_cookie_.yes();
      }
                                                 
      return new SchemeHandler(handler_);
    }

   private:
    CookieTestSchemeHandler* handler_;

    IMPLEMENT_REFCOUNTING(SchemeHandlerFactory);
  };

  class RequestContextHandler : public CefRequestContextHandler {
   public:
    explicit RequestContextHandler(CookieTestSchemeHandler* handler)
        : handler_(handler) {}

    CefRefPtr<CefCookieManager> GetCookieManager() override {
      EXPECT_TRUE(handler_);
      EXPECT_TRUE(CefCurrentlyOn(TID_IO));

      if (url_ == handler_->url1_) {
        // Return the first cookie manager.
        handler_->got_cookie_manager1_.yes();
        return handler_->manager1_;
      } else {
        // Return the second cookie manager.
        handler_->got_cookie_manager2_.yes();
        return handler_->manager2_;
      }
    }

    void SetURL(const std::string& url) {
      url_ = url;
    }

    void Detach() {
      handler_ = NULL;
    }

   private:
    std::string url_;
    CookieTestSchemeHandler* handler_;

    IMPLEMENT_REFCOUNTING(RequestContextHandler);
  };

  CookieTestSchemeHandler(const std::string& scheme) : scheme_(scheme) {
    url1_ = scheme + "://cookie-tests/cookie1.html";
    url2_ = scheme + "://cookie-tests/cookie2.html";
    url3_ = scheme + "://cookie-tests/cookie3.html";
  }

  void RunTest() override {
    // Create new in-memory managers.
    manager1_ = CefCookieManager::CreateManager(CefString(), false, NULL);
    manager2_ = CefCookieManager::CreateManager(CefString(), false, NULL);

    if (scheme_ != "http") {
      std::vector<CefString> schemes;
      schemes.push_back(scheme_);

      manager1_->SetSupportedSchemes(schemes, NULL);
      manager2_->SetSupportedSchemes(schemes, NULL);
    }

    context_handler_ = new RequestContextHandler(this);
    context_handler_->SetURL(url1_);

    // Create the request context that will use an in-memory cache.
    CefRequestContextSettings settings;
    CefRefPtr<CefRequestContext> request_context =
        CefRequestContext::CreateContext(settings, context_handler_.get());

    // Register the scheme handler.
    request_context->RegisterSchemeHandlerFactory(scheme_, "cookie-tests",
        new SchemeHandlerFactory(this));

    // Create the browser.
    CreateBrowser(url1_, request_context);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    std::string url = frame->GetURL();
    if (url == url1_) {
      got_load_end1_.yes();
      VerifyCookie(manager1_, url, "name1", "value1", got_cookie1_);

      // Go to the next URL
      context_handler_->SetURL(url2_);
      frame->LoadURL(url2_);
    } else if (url == url2_) {
      got_load_end2_.yes();
      VerifyCookie(manager2_, url, "name2", "value2", got_cookie2_);

      // Go to the next URL
      context_handler_->SetURL(url3_);
      frame->LoadURL(url3_);
    } else {
      got_load_end3_.yes();
      VerifyCookie(manager2_, url, "name2", "value2", got_cookie3_);

      // Unregister the scheme handler.
      browser->GetHost()->GetRequestContext()->RegisterSchemeHandlerFactory(
          scheme_, "cookie-tests", NULL);

      DestroyTest();
    }
  }

   void DestroyTest() override {
     context_handler_->Detach();
     context_handler_ = NULL;

     TestHandler::DestroyTest();
   }

  // Verify that the cookie was set successfully.
  void VerifyCookie(CefRefPtr<CefCookieManager> manager,
                    const std::string& url,
                    const std::string& name,
                    const std::string& value,
                    TrackCallback& callback) {
    base::WaitableEvent event(false, false);
    CookieVector cookies;

    // Get the cookie.
    VisitUrlCookies(manager, url, false, cookies, false, event);

    if (cookies.size() == 1U && CefString(&cookies[0].name) == name &&
        CefString(&cookies[0].value) == value) {
      callback.yes();
    }
  }

  std::string scheme_;
  std::string url1_;
  std::string url2_;
  std::string url3_;

  CefRefPtr<RequestContextHandler> context_handler_;

  CefRefPtr<CefCookieManager> manager1_;
  CefRefPtr<CefCookieManager> manager2_;

  TrackCallback got_process_request1_;
  TrackCallback got_process_request2_;
  TrackCallback got_process_request3_;
  TrackCallback got_create_cookie_;
  TrackCallback got_process_request_cookie_;
  TrackCallback got_cookie_manager1_;
  TrackCallback got_cookie_manager2_;
  TrackCallback got_load_end1_;
  TrackCallback got_load_end2_;
  TrackCallback got_load_end3_;
  TrackCallback got_cookie1_;
  TrackCallback got_cookie2_;
  TrackCallback got_cookie3_;

  IMPLEMENT_REFCOUNTING(CookieTestSchemeHandler);
};

}  // namespace

// Verify use of multiple cookie managers via HTTP.
TEST(CookieTest, GetCookieManagerHttp) {
  CefRefPtr<CookieTestSchemeHandler> handler =
      new CookieTestSchemeHandler("http");
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_process_request1_);
  EXPECT_TRUE(handler->got_process_request2_);
  EXPECT_TRUE(handler->got_process_request3_);
  EXPECT_FALSE(handler->got_create_cookie_);
  EXPECT_TRUE(handler->got_process_request_cookie_);
  EXPECT_TRUE(handler->got_cookie_manager1_);
  EXPECT_TRUE(handler->got_cookie_manager2_);
  EXPECT_TRUE(handler->got_load_end1_);
  EXPECT_TRUE(handler->got_load_end2_);
  EXPECT_TRUE(handler->got_load_end3_);
  EXPECT_TRUE(handler->got_cookie1_);
  EXPECT_TRUE(handler->got_cookie2_);
  EXPECT_TRUE(handler->got_cookie3_);

  ReleaseAndWaitForDestructor(handler);
}

// Verify use of multiple cookie managers via a custom scheme.
TEST(CookieTest, GetCookieManagerCustom) {
  CefRefPtr<CookieTestSchemeHandler> handler =
      new CookieTestSchemeHandler("ccustom");
  handler->ExecuteTest();

  EXPECT_TRUE(handler->got_process_request1_);
  EXPECT_TRUE(handler->got_process_request2_);
  EXPECT_TRUE(handler->got_process_request3_);
  EXPECT_FALSE(handler->got_create_cookie_);
  EXPECT_TRUE(handler->got_process_request_cookie_);
  EXPECT_TRUE(handler->got_cookie_manager1_);
  EXPECT_TRUE(handler->got_cookie_manager2_);
  EXPECT_TRUE(handler->got_load_end1_);
  EXPECT_TRUE(handler->got_load_end2_);
  EXPECT_TRUE(handler->got_load_end3_);
  EXPECT_TRUE(handler->got_cookie1_);
  EXPECT_TRUE(handler->got_cookie2_);
  EXPECT_TRUE(handler->got_cookie3_);

  ReleaseAndWaitForDestructor(handler);
}

// Entry point for registering custom schemes.
// Called from client_app_delegates.cc.
void RegisterCookieCustomSchemes(
      CefRefPtr<CefSchemeRegistrar> registrar,
      std::vector<CefString>& cookiable_schemes) {
  // Used by GetCookieManagerCustom test.
  registrar->AddCustomScheme("ccustom", true, false, false);
}
