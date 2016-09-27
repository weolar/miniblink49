// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/browser_window_std_mac.h"

#include <Cocoa/Cocoa.h>

#include "include/base/cef_logging.h"
#include "cefclient/browser/client_handler_std.h"
#include "cefclient/browser/main_message_loop.h"

namespace client {

BrowserWindowStdMac::BrowserWindowStdMac(Delegate* delegate,
                                         const std::string& startup_url)
    : BrowserWindow(delegate) {
  client_handler_ = new ClientHandlerStd(this, startup_url);
}

void BrowserWindowStdMac::CreateBrowser(
    ClientWindowHandle parent_handle,
    const CefRect& rect,
    const CefBrowserSettings& settings,
    CefRefPtr<CefRequestContext> request_context) {
  REQUIRE_MAIN_THREAD();

  CefWindowInfo window_info;
  window_info.SetAsChild(parent_handle,
                         rect.x, rect.y, rect.width, rect.height);

  CefBrowserHost::CreateBrowser(window_info, client_handler_,
                                client_handler_->startup_url(),
                                settings, request_context);
}

void BrowserWindowStdMac::GetPopupConfig(CefWindowHandle temp_handle,
                                         CefWindowInfo& windowInfo,
                                         CefRefPtr<CefClient>& client,
                                         CefBrowserSettings& settings) {
  // Note: This method may be called on any thread.
  // The window will be properly sized after the browser is created.
  windowInfo.SetAsChild(temp_handle, 0, 0, 0, 0);
  client = client_handler_;
}

void BrowserWindowStdMac::ShowPopup(ClientWindowHandle parent_handle,
                                    int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();

  NSView* browser_view = GetWindowHandle();

  // Re-parent |browser_view| to |parent_handle|.
  [browser_view removeFromSuperview];
  [parent_handle addSubview:browser_view];

  NSSize size = NSMakeSize(static_cast<int>(width), static_cast<int>(height));
  [browser_view setFrameSize:size];
}

void BrowserWindowStdMac::Show() {
  REQUIRE_MAIN_THREAD();

  if (browser_)
    browser_->GetHost()->SetWindowVisibility(true);
}

void BrowserWindowStdMac::Hide() {
  REQUIRE_MAIN_THREAD();

  if (browser_)
    browser_->GetHost()->SetWindowVisibility(false);
}

void BrowserWindowStdMac::SetBounds(int x, int y, size_t width, size_t height) {
  REQUIRE_MAIN_THREAD();
  // Nothing to do here. Cocoa will size the browser for us.
}

void BrowserWindowStdMac::SetFocus(bool focus) {
  REQUIRE_MAIN_THREAD();

  if (browser_)
    browser_->GetHost()->SetFocus(focus);
}

ClientWindowHandle BrowserWindowStdMac::GetWindowHandle() const {
  REQUIRE_MAIN_THREAD();

  if (browser_)
    return browser_->GetHost()->GetWindowHandle();
  return NULL;
}

}  // namespace client
