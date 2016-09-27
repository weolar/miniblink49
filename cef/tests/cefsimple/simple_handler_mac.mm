// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefsimple/simple_handler.h"

#import <Cocoa/Cocoa.h>

#include "include/cef_browser.h"
#include "include/wrapper/cef_helpers.h"

void SimpleHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title) {
  CEF_REQUIRE_UI_THREAD();

  NSView* view = (NSView*)browser->GetHost()->GetWindowHandle();
  NSWindow* window = [view window];
  std::string titleStr(title);
  NSString* str = [NSString stringWithUTF8String:titleStr.c_str()];
  [window setTitle:str];
}
