// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/window_test.h"

#import <Cocoa/Cocoa.h>

namespace client {
namespace window_test {

namespace {

NSWindow* GetWindow(CefRefPtr<CefBrowser> browser) {
  NSView* view = (NSView*)browser->GetHost()->GetWindowHandle();
  return [view window];
}

}  // namespace

void SetPos(CefRefPtr<CefBrowser> browser, int x, int y, int width,
            int height) {
  NSWindow* window = GetWindow(browser);

  // Make sure the window isn't minimized or maximized.
  if ([window isMiniaturized])
    [window deminiaturize:nil];
  else if ([window isZoomed])
    [window performZoom:nil];

  // Retrieve information for the display that contains the window.
  NSScreen* screen = [window screen];
  if (screen == nil)
    screen = [NSScreen mainScreen];
  NSRect frame = [screen frame];
  NSRect visibleFrame = [screen visibleFrame];

  // Make sure the window is inside the display.
  CefRect display_rect(
      visibleFrame.origin.x,
      frame.size.height - visibleFrame.size.height - visibleFrame.origin.y,
      visibleFrame.size.width,
      visibleFrame.size.height);
  CefRect window_rect(x, y, width, height);
  ModifyBounds(display_rect, window_rect);

  NSRect newRect;
  newRect.origin.x = window_rect.x;
  newRect.origin.y = frame.size.height - window_rect.height - window_rect.y;
  newRect.size.width = window_rect.width;
  newRect.size.height = window_rect.height;
  [window setFrame:newRect display:YES];
}

void Minimize(CefRefPtr<CefBrowser> browser) {
  [GetWindow(browser) performMiniaturize:nil];
}

void Maximize(CefRefPtr<CefBrowser> browser) {
  [GetWindow(browser) performZoom:nil];
}

void Restore(CefRefPtr<CefBrowser> browser) {
  NSWindow* window = GetWindow(browser);
  if ([window isMiniaturized])
    [window deminiaturize:nil];
  else if ([window isZoomed])
    [window performZoom:nil];
}

}  // namespace window_test
}  // namespace client
