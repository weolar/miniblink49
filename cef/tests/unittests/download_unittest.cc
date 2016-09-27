// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "base/files/file_util.h"
#include "base/files/scoped_temp_dir.h"

#include "include/cef_scheme.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/test_handler.h"

namespace {

const char kTestDomain[] = "test-download";
const char kTestEntryUrl[] = "http://test-download/test.html";
const char kTestDownloadUrl[] = "http://test-download/download.txt";
const char kTestFileName[] = "download_test.txt";
const char kTestContentDisposition[] =
    "attachment; filename=\"download_test.txt\"";
const char kTestMimeType[] = "text/plain";
const char kTestContent[] = "Download test text";

class DownloadSchemeHandler : public CefResourceHandler {
 public:
  explicit DownloadSchemeHandler(TrackCallback* got_download_request)
      : got_download_request_(got_download_request),
        offset_(0) {}

  bool ProcessRequest(CefRefPtr<CefRequest> request,
                      CefRefPtr<CefCallback> callback) override {
    std::string url = request->GetURL();
    if (url == kTestEntryUrl) {
      content_ = "<html><body>Download Test</body></html>";
      mime_type_ = "text/html";
    } else if (url == kTestDownloadUrl) {
      got_download_request_->yes();
      content_ = kTestContent;
      mime_type_ = kTestMimeType;
      content_disposition_ = kTestContentDisposition;
    } else {
      EXPECT_TRUE(false); // Not reached.
      return false;
    }
    
    callback->Continue();
    return true;
  }

  void GetResponseHeaders(CefRefPtr<CefResponse> response,
                          int64& response_length,
                          CefString& redirectUrl) override {
    response_length = content_.size();

    response->SetStatus(200);
    response->SetMimeType(mime_type_);

    if (!content_disposition_.empty()) {
      CefResponse::HeaderMap headerMap;
      response->GetHeaderMap(headerMap);
      headerMap.insert(
          std::make_pair("Content-Disposition", content_disposition_));
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
  TrackCallback* got_download_request_;
  std::string content_;
  std::string mime_type_;
  std::string content_disposition_;
  size_t offset_;

  IMPLEMENT_REFCOUNTING(DownloadSchemeHandler);
};

class DownloadSchemeHandlerFactory : public CefSchemeHandlerFactory {
 public:
  explicit DownloadSchemeHandlerFactory(TrackCallback* got_download_request)
    : got_download_request_(got_download_request) {}

  CefRefPtr<CefResourceHandler> Create(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      const CefString& scheme_name,
      CefRefPtr<CefRequest> request) override {
    return new DownloadSchemeHandler(got_download_request_);
  }

 private:
  TrackCallback* got_download_request_;

  IMPLEMENT_REFCOUNTING(DownloadSchemeHandlerFactory);
};

class DownloadTestHandler : public TestHandler {
 public:
  DownloadTestHandler() {}

  void RunTest() override {
    CefRegisterSchemeHandlerFactory("http", kTestDomain,
        new DownloadSchemeHandlerFactory(&got_download_request_));

    // Create a new temporary directory.
    EXPECT_TRUE(temp_dir_.CreateUniqueTempDir());
    test_path_ = temp_dir_.path().AppendASCII(kTestFileName);

    // Create the browser
    CreateBrowser(kTestEntryUrl);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    EXPECT_STREQ(kTestEntryUrl, frame->GetURL().ToString().c_str());

    // Begin the download.
    browser->GetHost()->StartDownload(kTestDownloadUrl);
  }

  void OnBeforeDownload(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefDownloadItem> download_item,
      const CefString& suggested_name,
      CefRefPtr<CefBeforeDownloadCallback> callback) override {
    EXPECT_TRUE(CefCurrentlyOn(TID_UI));
    EXPECT_FALSE(got_on_before_download_);

    got_on_before_download_.yes();

    EXPECT_TRUE(browser->IsSame(GetBrowser()));
    EXPECT_STREQ(kTestFileName, suggested_name.ToString().c_str());
    EXPECT_TRUE(download_item.get());
    EXPECT_TRUE(callback.get());

    download_id_ = download_item->GetId();
    EXPECT_LT(0U, download_id_);
    
    EXPECT_TRUE(download_item->IsValid());
    EXPECT_TRUE(download_item->IsInProgress());
    EXPECT_FALSE(download_item->IsComplete());
    EXPECT_FALSE(download_item->IsCanceled());
    EXPECT_EQ(static_cast<int64>(sizeof(kTestContent)-1),
        download_item->GetTotalBytes());
    EXPECT_EQ(0UL, download_item->GetFullPath().length());
    EXPECT_STREQ(kTestDownloadUrl, download_item->GetURL().ToString().c_str());
    EXPECT_EQ(0UL, download_item->GetSuggestedFileName().length());
    EXPECT_STREQ(kTestContentDisposition,
        download_item->GetContentDisposition().ToString().c_str());
    EXPECT_STREQ(kTestMimeType, download_item->GetMimeType().ToString().c_str());

    callback->Continue(test_path_.value(), false);
  }

  void OnDownloadUpdated(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefDownloadItem> download_item,
      CefRefPtr<CefDownloadItemCallback> callback) override {
    EXPECT_TRUE(CefCurrentlyOn(TID_UI));

    got_on_download_updated_.yes();

    EXPECT_TRUE(browser->IsSame(GetBrowser()));
    EXPECT_TRUE(download_item.get());
    EXPECT_TRUE(callback.get());

    if (got_on_before_download_)
      EXPECT_EQ(download_id_, download_item->GetId());

    EXPECT_LE(0LL, download_item->GetCurrentSpeed());
    EXPECT_LE(0, download_item->GetPercentComplete());

    EXPECT_TRUE(download_item->IsValid());
    EXPECT_FALSE(download_item->IsCanceled());
    EXPECT_STREQ(kTestDownloadUrl, download_item->GetURL().ToString().c_str());
    EXPECT_STREQ(kTestContentDisposition,
        download_item->GetContentDisposition().ToString().c_str());
    EXPECT_STREQ(kTestMimeType,
        download_item->GetMimeType().ToString().c_str());

    std::string full_path = download_item->GetFullPath();
    if (!full_path.empty()) {
      got_full_path_.yes();
      EXPECT_STREQ(CefString(test_path_.value()).ToString().c_str(),
          full_path.c_str());
    }

    if (download_item->IsComplete()) {
      EXPECT_FALSE(download_item->IsInProgress());
      EXPECT_EQ(100, download_item->GetPercentComplete());
      EXPECT_EQ(static_cast<int64>(sizeof(kTestContent)-1),
          download_item->GetReceivedBytes());
      EXPECT_EQ(static_cast<int64>(sizeof(kTestContent)-1),
          download_item->GetTotalBytes());

      DestroyTest();
    } else {
      EXPECT_TRUE(download_item->IsInProgress());
      EXPECT_LE(0LL, download_item->GetReceivedBytes());
    }
  }

  void DestroyTest() override {
    CefRegisterSchemeHandlerFactory("http", kTestDomain, NULL);

    EXPECT_TRUE(got_download_request_);
    EXPECT_TRUE(got_on_before_download_);
    EXPECT_TRUE(got_on_download_updated_);
    EXPECT_TRUE(got_full_path_);

    // Verify the file contents.
    std::string contents;
    EXPECT_TRUE(base::ReadFileToString(test_path_, &contents));
    EXPECT_STREQ(kTestContent, contents.c_str());

    EXPECT_TRUE(temp_dir_.Delete());

    TestHandler::DestroyTest();
  }

 private:
  base::ScopedTempDir temp_dir_;
  base::FilePath test_path_;
  uint32 download_id_;

  TrackCallback got_download_request_;
  TrackCallback got_on_before_download_;
  TrackCallback got_on_download_updated_;
  TrackCallback got_full_path_;

  IMPLEMENT_REFCOUNTING(DownloadTestHandler);
};

}  // namespace

// Verify that downloads work.
TEST(DownloadTest, Download) {
  CefRefPtr<DownloadTestHandler> handler = new DownloadTestHandler();
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}
