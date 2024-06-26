// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/screen.h"

#import <UIKit/UIKit.h>

#include "base/logging.h"
#include "ui/gfx/display.h"

namespace {

class ScreenIos : public gfx::Screen {
  gfx::Point GetCursorScreenPoint() override {
    NOTIMPLEMENTED();
    return gfx::Point(0, 0);
  }

  gfx::NativeWindow GetWindowUnderCursor() override {
    NOTIMPLEMENTED();
    return gfx::NativeWindow();
  }

  gfx::NativeWindow GetWindowAtScreenPoint(const gfx::Point& point) override {
    NOTIMPLEMENTED();
    return gfx::NativeWindow();
  }

  int GetNumDisplays() const override {
#if TARGET_IPHONE_SIMULATOR
    // UIScreen does not reliably return correct results on the simulator.
    return 1;
#else
    return [[UIScreen screens] count];
#endif
  }

  std::vector<gfx::Display> GetAllDisplays() const override {
    NOTIMPLEMENTED();
    return std::vector<gfx::Display>(1, GetPrimaryDisplay());
  }

  // Returns the display nearest the specified window.
  gfx::Display GetDisplayNearestWindow(gfx::NativeView view) const override {
    NOTIMPLEMENTED();
    return gfx::Display();
  }

  // Returns the the display nearest the specified point.
  gfx::Display GetDisplayNearestPoint(const gfx::Point& point) const override {
    NOTIMPLEMENTED();
    return gfx::Display();
  }

  // Returns the display that most closely intersects the provided bounds.
  gfx::Display GetDisplayMatching(const gfx::Rect& match_rect) const override {
    NOTIMPLEMENTED();
    return gfx::Display();
  }

  // Returns the primary display.
  gfx::Display GetPrimaryDisplay() const override {
    UIScreen* mainScreen = [UIScreen mainScreen];
    CHECK(mainScreen);
    gfx::Display display(0, gfx::Rect(mainScreen.bounds));
    display.set_device_scale_factor([mainScreen scale]);
    return display;
  }

  void AddObserver(gfx::DisplayObserver* observer) override {
    // no display change on iOS.
  }

  void RemoveObserver(gfx::DisplayObserver* observer) override {
    // no display change on iOS.
  }
};

}  // namespace

namespace gfx {

Screen* CreateNativeScreen() {
  return new ScreenIos;
}

}  // namespace gfx
