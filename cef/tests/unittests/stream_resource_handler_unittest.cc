// Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include <algorithm>
#include <cstdlib>
#include <string>

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "include/base/cef_bind.h"
#include "include/cef_stream.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/routing_test_handler.h"

namespace {

const char kTestUrl[] = "http://tests-srh/test.html";
const size_t kReadBlockSize = 1024U;  // 1k.

// The usual network buffer size is about 32k. Choose a value that's larger.
const size_t kReadDesiredSize = 100U * 1024U;  // 100k

class ReadHandler : public CefReadHandler {
 public:
  explicit ReadHandler(bool may_block)
    : may_block_(may_block),
      offset_(0),
      expected_result_(0) {
  }

  void CreateContent() {
    // To verify that the data transers successfully we're going to make a big
    // math problem.
    content_.reserve(kReadDesiredSize + 50U);
    content_ = "<html><body><script>var myratherlongvariablename=0;";

    while (content_.size() < kReadDesiredSize) {
      content_ += "myratherlongvariablename=myratherlongvariablename+1;";
      expected_result_++;
    }

    content_ += "window.testQuery({request:myratherlongvariablename+''});"
                "</script></body></html>";
  }

  int GetExpectedResult() const {
    return expected_result_;
  }

  size_t Read(void* ptr, size_t size, size_t n) override {
    EXPECT_EQ(1U, size);

    // Read the minimum of requested size, remaining size or kReadBlockSize.
    const size_t read_bytes =
        std::min(std::min(size * n, content_.size() - offset_), kReadBlockSize);
    if (read_bytes > 0) {
      memcpy(ptr, content_.c_str() + offset_, read_bytes);
      offset_ += read_bytes;
    }

    return read_bytes;
  }

  int Seek(int64 offset, int whence) override {
    EXPECT_TRUE(false);  // Not reached.
    return 0;
  }

  int64 Tell() override {
    EXPECT_TRUE(false);  // Not reached.
    return 0;
  }

  int Eof() override {
    EXPECT_TRUE(false);  // Not reached.
    return 0;
  }

  bool MayBlock() override {
    return may_block_;
  }

 private:
  const bool may_block_;
  std::string content_;
  size_t offset_;
  int expected_result_;

  IMPLEMENT_REFCOUNTING(ReadHandler);
};

class ReadTestHandler : public RoutingTestHandler {
 public:
  explicit ReadTestHandler(bool may_block)
      : may_block_(may_block),
        expected_result_(0) {}

  void RunTest() override {
    // Create the browser.
    CreateBrowser(kTestUrl);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  CefRefPtr<CefResourceHandler> GetResourceHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) override {
    got_resource_handler_.yes();

    const std::string& url = request->GetURL();
    EXPECT_STREQ(kTestUrl, url.c_str());

    CefRefPtr<ReadHandler> handler = new ReadHandler(may_block_);
    handler->CreateContent();
    expected_result_ = handler->GetExpectedResult();

    CefRefPtr<CefStreamReader> stream =
        CefStreamReader::CreateForHandler(handler.get());
    return new CefStreamResourceHandler("text/html", stream);
  }

  bool OnQuery(CefRefPtr<CefBrowser> browser,
               CefRefPtr<CefFrame> frame,
               int64 query_id,
               const CefString& request,
               bool persistent,
               CefRefPtr<Callback> callback) override {
    got_on_query_.yes();

    const int actual_result = atoi(request.ToString().c_str());
    EXPECT_EQ(expected_result_, actual_result);

    DestroyTestIfDone();

    return true;
  }

  void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                            bool isLoading,
                            bool canGoBack,
                            bool canGoForward) override {
    if (!isLoading) {
      got_on_loading_state_change_done_.yes();
      DestroyTestIfDone();
    }
  }

 private:
  void DestroyTestIfDone() {
    if (got_on_query_ && got_on_loading_state_change_done_)
      DestroyTest();
  }

  void DestroyTest() override {
    EXPECT_TRUE(got_resource_handler_);
    EXPECT_TRUE(got_on_query_);
    EXPECT_TRUE(got_on_loading_state_change_done_);
    RoutingTestHandler::DestroyTest();
  }

  const bool may_block_;

  int expected_result_;
  TrackCallback got_resource_handler_;
  TrackCallback got_on_query_;
  TrackCallback got_on_loading_state_change_done_;

  IMPLEMENT_REFCOUNTING(ReadTestHandler);
};

}  // namespace

TEST(StreamResourceHandlerTest, ReadWillBlock) {
  CefRefPtr<ReadTestHandler> handler = new ReadTestHandler(true);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(StreamResourceHandlerTest, ReadWontBlock) {
  CefRefPtr<ReadTestHandler> handler = new ReadTestHandler(false);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}
