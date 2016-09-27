// Copyright (c) 2015 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/temp_window_mac.h"

#include <Cocoa/Cocoa.h>

#include "include/base/cef_logging.h"
#include "include/cef_app.h"

namespace client {

namespace {

TempWindowMac* g_temp_window = NULL;

}  // namespace

TempWindowMac::TempWindowMac()
    : window_(nil) {
  DCHECK(!g_temp_window);
  g_temp_window = this;

  // Create a borderless non-visible 1x1 window.
  window_ = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 1, 1)
                                        styleMask:NSBorderlessWindowMask
                                          backing:NSBackingStoreBuffered
                                            defer:NO];
  CHECK(window_);
}

TempWindowMac::~TempWindowMac() {
  g_temp_window = NULL;
  DCHECK(window_);

  [window_ close];
}

// static
CefWindowHandle TempWindowMac::GetWindowHandle() {
  DCHECK(g_temp_window);
  return [g_temp_window->window_ contentView];
}

}  // namespace client
