// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "tests/unittests/test_handler.h"
#include "tests/unittests/test_util.h"

namespace {

const char* kTestUrl = "http://tests/DialogTestHandler";

class DialogTestHandler : public TestHandler {
 public:
  struct TestConfig {
    explicit TestConfig(FileDialogMode dialog_mode)
      : mode(dialog_mode),
        title("Test Title"),
        default_file_name("Test File Name"),
        selected_accept_filter(1),  // Something other than 0 for testing.
        callback_async(false),
        callback_cancel(false) {
      accept_types.push_back("text/*");
      accept_types.push_back(".js");
      accept_types.push_back(".css");
    }

    FileDialogMode mode;
    CefString title;
    CefString default_file_name;
    std::vector<CefString> accept_types;
    int selected_accept_filter;

    bool callback_async;  // True if the callback should execute asynchronously.
    bool callback_cancel;  // True if the callback should cancel.
    std::vector<CefString> callback_paths;  // Resulting paths if not cancelled.
  };

  class Callback : public CefRunFileDialogCallback {
   public:
    explicit Callback(DialogTestHandler* handler)
        : handler_(handler) {
    }

    void OnFileDialogDismissed(
        int selected_accept_filter,
        const std::vector<CefString>& file_paths) override {
      handler_->got_onfiledialogdismissed_.yes();

      if (handler_->config_.callback_cancel) {
        EXPECT_EQ(0, selected_accept_filter);
        EXPECT_TRUE(file_paths.empty());
      } else {
        EXPECT_EQ(handler_->config_.selected_accept_filter,
                  selected_accept_filter);
        TestStringVectorEqual(handler_->config_.callback_paths, file_paths);
      }

      handler_->DestroyTest();
      handler_ = NULL;
    }

   private:
    DialogTestHandler* handler_;

    IMPLEMENT_REFCOUNTING(Callback);
  };

  explicit DialogTestHandler(const TestConfig& config)
      : config_(config) {
  }

  void RunTest() override {
    AddResource(kTestUrl, "<html><body>TEST</body></html>", "text/html");

    // Create the browser
    CreateBrowser(kTestUrl);

    // Time out the test after a reasonable period of time.
    SetTestTimeout();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefFrame> frame,
                 int httpStatusCode) override {
    browser->GetHost()->RunFileDialog(config_.mode,
                                      config_.title,
                                      config_.default_file_name,
                                      config_.accept_types,
                                      config_.selected_accept_filter,
                                      new Callback(this));
  }

  void ExecuteCallback(CefRefPtr<CefFileDialogCallback> callback) {
    if (config_.callback_cancel)
      callback->Cancel();
    else
      callback->Continue(config_.selected_accept_filter,
                         config_.callback_paths);
  }

  // CefDialogHandler
  bool OnFileDialog(
      CefRefPtr<CefBrowser> browser,
      FileDialogMode mode,
      const CefString& title,
      const CefString& default_file_name,
      const std::vector<CefString>& accept_types,
      int selected_accept_filter,
      CefRefPtr<CefFileDialogCallback> callback) override {
    got_onfiledialog_.yes();

    std::string url = browser->GetMainFrame()->GetURL();
    EXPECT_STREQ(kTestUrl, url.c_str());

    EXPECT_EQ(config_.mode, mode);
    EXPECT_STREQ(config_.title.ToString().c_str(), title.ToString().c_str());
    EXPECT_STREQ(config_.default_file_name.ToString().c_str(),
                 default_file_name.ToString().c_str());
    TestStringVectorEqual(config_.accept_types, accept_types);

    if (config_.callback_async) {
      CefPostTask(TID_UI,
          base::Bind(&DialogTestHandler::ExecuteCallback, this, callback));
    } else {
      ExecuteCallback(callback);
    }

    return true;
  }

  void DestroyTest() override {
    EXPECT_TRUE(got_onfiledialog_);
    EXPECT_TRUE(got_onfiledialogdismissed_);

    TestHandler::DestroyTest();
  }

  TestConfig config_;

  TrackCallback got_onfiledialog_;
  TrackCallback got_onfiledialogdismissed_;

  IMPLEMENT_REFCOUNTING(DialogTestHandler);
};

}  // namespace

// Test with all parameters empty.
TEST(DialogTest, FileEmptyParams) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN);
  config.title.clear();
  config.default_file_name.clear();
  config.accept_types.clear();
  config.callback_async = false;
  config.callback_cancel = false;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileAdditionalFlags) {
  DialogTestHandler::TestConfig config(
      static_cast<cef_file_dialog_mode_t>(FILE_DIALOG_OPEN |
                                          FILE_DIALOG_HIDEREADONLY_FLAG |
                                          FILE_DIALOG_OVERWRITEPROMPT_FLAG));
  config.title.clear();
  config.default_file_name.clear();
  config.accept_types.clear();
  config.callback_async = false;
  config.callback_cancel = false;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpen) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN);
  config.callback_async = false;
  config.callback_cancel = false;
  config.callback_paths.push_back("/path/to/file1.txt");

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenCancel) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN);
  config.callback_async = false;
  config.callback_cancel = true;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenAsync) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN);
  config.callback_async = true;
  config.callback_cancel = false;
  config.callback_paths.push_back("/path/to/file1.txt");

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenAsyncCancel) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN);
  config.callback_async = false;
  config.callback_cancel = true;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenMultiple) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN_MULTIPLE);
  config.callback_async = false;
  config.callback_cancel = false;
  config.callback_paths.push_back("/path/to/file1.txt");
  config.callback_paths.push_back("/path/to/file2.txt");

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenMultipleCancel) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN_MULTIPLE);
  config.callback_async = false;
  config.callback_cancel = true;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenMultipleAsync) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN_MULTIPLE);
  config.callback_async = true;
  config.callback_cancel = false;
  config.callback_paths.push_back("/path/to/file1.txt");
  config.callback_paths.push_back("/path/to/file2.txt");

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenMultipleAsyncCancel) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN_MULTIPLE);
  config.callback_async = false;
  config.callback_cancel = true;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenFolder) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN_FOLDER);
  config.callback_async = false;
  config.callback_cancel = false;
  config.callback_paths.push_back("/path/to/folder");

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenFolderCancel) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN_FOLDER);
  config.callback_async = false;
  config.callback_cancel = true;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenFolderAsync) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN_FOLDER);
  config.callback_async = true;
  config.callback_cancel = false;
  config.callback_paths.push_back("/path/to/folder");

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileOpenFolderAsyncCancel) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_OPEN_FOLDER);
  config.callback_async = false;
  config.callback_cancel = true;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileSave) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_SAVE);
  config.callback_async = false;
  config.callback_cancel = false;
  config.callback_paths.push_back("/path/to/file1.txt");

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileSaveCancel) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_SAVE);
  config.callback_async = false;
  config.callback_cancel = true;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileSaveAsync) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_SAVE);
  config.callback_async = true;
  config.callback_cancel = false;
  config.callback_paths.push_back("/path/to/file1.txt");

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}

TEST(DialogTest, FileSaveAsyncCancel) {
  DialogTestHandler::TestConfig config(FILE_DIALOG_SAVE);
  config.callback_async = false;
  config.callback_cancel = true;

  CefRefPtr<DialogTestHandler> handler = new DialogTestHandler(config);
  handler->ExecuteTest();
  ReleaseAndWaitForDestructor(handler);
}
