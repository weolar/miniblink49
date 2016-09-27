// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/window_test.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "include/base/cef_bind.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include "cefclient/browser/main_message_loop.h"

namespace client {
namespace window_test {

namespace {

const char kTestUrl[] = "http://tests/window";
const char kMessagePositionName[] = "WindowTest.Position";
const char kMessageMinimizeName[] = "WindowTest.Minimize";
const char kMessageMaximizeName[] = "WindowTest.Maximize";
const char kMessageRestoreName[] = "WindowTest.Restore";

// Handle messages in the browser process.
class Handler : public CefMessageRouterBrowserSide::Handler {
 public:
  Handler() {}

  // Called due to cefBroadcast execution in window.html.
  virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
                       CefRefPtr<CefFrame> frame,
                       int64 query_id,
                       const CefString& request,
                       bool persistent,
                       CefRefPtr<Callback> callback) OVERRIDE {
    // Only handle messages from the test URL.
    const std::string& url = frame->GetURL();
    if (url.find(kTestUrl) != 0)
      return false;

    const std::string& message_name = request;
    if (message_name.find(kMessagePositionName) == 0) {
      // Parse the comma-delimited list of integer values.
      std::vector<int> vec;
      const std::string& vals =
          message_name.substr(sizeof(kMessagePositionName));
      std::stringstream ss(vals);
      int i;
      while (ss >> i) {
        vec.push_back(i);
        if (ss.peek() == ',')
          ss.ignore();
      }

      if (vec.size() == 4) {
        // Execute SetPos() on the main thread.
        MAIN_POST_CLOSURE(base::Bind(&SetPos, browser,
                                     vec[0], vec[1], vec[2], vec[3]));
      }
    } else if (message_name == kMessageMinimizeName) {
      // Execute Minimize() on the main thread.
      MAIN_POST_CLOSURE(base::Bind(&Minimize, browser));
    } else if (message_name == kMessageMaximizeName) {
      // Execute Maximize() on the main thread.
      MAIN_POST_CLOSURE(base::Bind(&Maximize, browser));
    } else if (message_name == kMessageRestoreName) {
      // Execute Restore() on the main thread.
      MAIN_POST_CLOSURE(base::Bind(&Restore, browser));
    } else {
      NOTREACHED();
    }

    callback->Success("");
    return true;
  }
};

}  // namespace

void ModifyBounds(const CefRect& display, CefRect& window) {
  window.x += display.x;
  window.y += display.y;

  if (window.x < display.x)
    window.x = display.x;
  if (window.y < display.y)
    window.y = display.y;
  if (window.width < 100)
    window.width = 100;
  else if (window.width >= display.width)
    window.width = display.width;
  if (window.height < 100)
    window.height = 100;
  else if (window.height >= display.height)
    window.height = display.height;
  if (window.x + window.width >= display.x + display.width)
    window.x = display.x + display.width - window.width;
  if (window.y + window.height >= display.y + display.height)
    window.y = display.y + display.height - window.height;
}

void CreateMessageHandlers(test_runner::MessageHandlerSet& handlers) {
  handlers.insert(new Handler());
}

}  // namespace window_test
}  // namespace client
