// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/screen.h"

#import <ApplicationServices/ApplicationServices.h>
#import <Cocoa/Cocoa.h>
#include <stdint.h>

#include <map>

#include "base/logging.h"
#include "base/mac/sdk_forward_declarations.h"
#include "base/macros.h"
#include "base/timer/timer.h"
#include "ui/gfx/display.h"
#include "ui/gfx/display_change_notifier.h"

namespace {

// The delay to handle the display configuration changes.
// See comments in ScreenMac::HandleDisplayReconfiguration.
const int64_t kConfigureDelayMs = 500;

gfx::Rect ConvertCoordinateSystem(NSRect ns_rect) {
  // Primary monitor is defined as the monitor with the menubar,
  // which is always at index 0.
  NSScreen* primary_screen = [[NSScreen screens] firstObject];
  float primary_screen_height = [primary_screen frame].size.height;
  gfx::Rect rect(NSRectToCGRect(ns_rect));
  rect.set_y(primary_screen_height - rect.y() - rect.height());
  return rect;
}

NSScreen* GetMatchingScreen(const gfx::Rect& match_rect) {
  // Default to the monitor with the current keyboard focus, in case
  // |match_rect| is not on any screen at all.
  NSScreen* max_screen = [NSScreen mainScreen];
  int max_area = 0;

  for (NSScreen* screen in [NSScreen screens]) {
    gfx::Rect monitor_area = ConvertCoordinateSystem([screen frame]);
    gfx::Rect intersection = gfx::IntersectRects(monitor_area, match_rect);
    int area = intersection.width() * intersection.height();
    if (area > max_area) {
      max_area = area;
      max_screen = screen;
    }
  }

  return max_screen;
}

gfx::Display GetDisplayForScreen(NSScreen* screen) {
  NSRect frame = [screen frame];

  CGDirectDisplayID display_id = [[[screen deviceDescription]
    objectForKey:@"NSScreenNumber"] unsignedIntValue];

  gfx::Display display(display_id, gfx::Rect(NSRectToCGRect(frame)));
  NSRect visible_frame = [screen visibleFrame];
  NSScreen* primary = [[NSScreen screens] firstObject];

  // Convert work area's coordinate systems.
  if ([screen isEqual:primary]) {
    gfx::Rect work_area = gfx::Rect(NSRectToCGRect(visible_frame));
    work_area.set_y(frame.size.height - visible_frame.origin.y -
                    visible_frame.size.height);
    display.set_work_area(work_area);
  } else {
    display.set_bounds(ConvertCoordinateSystem(frame));
    display.set_work_area(ConvertCoordinateSystem(visible_frame));
  }
  CGFloat scale;
  if ([screen respondsToSelector:@selector(backingScaleFactor)])
    scale = [screen backingScaleFactor];
  else
    scale = [screen userSpaceScaleFactor];

  if (gfx::Display::HasForceDeviceScaleFactor())
    scale = gfx::Display::GetForcedDeviceScaleFactor();

  display.set_device_scale_factor(scale);
  // CGDisplayRotation returns a double. Display::SetRotationAsDegree will
  // handle the unexpected situations were the angle is not a multiple of 90.
  display.SetRotationAsDegree(static_cast<int>(CGDisplayRotation(display_id)));
  return display;
}

class ScreenMac : public gfx::Screen {
 public:
  ScreenMac() {
    displays_ = BuildDisplaysFromQuartz();

    CGDisplayRegisterReconfigurationCallback(
        ScreenMac::DisplayReconfigurationCallBack, this);
  }

  ~ScreenMac() override {
    CGDisplayRemoveReconfigurationCallback(
        ScreenMac::DisplayReconfigurationCallBack, this);
  }

  gfx::Point GetCursorScreenPoint() override {
    NSPoint mouseLocation  = [NSEvent mouseLocation];
    // Flip coordinates to gfx (0,0 in top-left corner) using primary screen.
    NSScreen* screen = [[NSScreen screens] firstObject];
    mouseLocation.y = NSMaxY([screen frame]) - mouseLocation.y;
    return gfx::Point(mouseLocation.x, mouseLocation.y);
  }

  gfx::NativeWindow GetWindowUnderCursor() override {
    NOTIMPLEMENTED();
    return gfx::NativeWindow();
  }

  gfx::NativeWindow GetWindowAtScreenPoint(const gfx::Point& point) override {
    NOTIMPLEMENTED();
    return gfx::NativeWindow();
  }

  int GetNumDisplays() const override { return GetAllDisplays().size(); }

  std::vector<gfx::Display> GetAllDisplays() const override {
    return displays_;
  }

  gfx::Display GetDisplayNearestWindow(gfx::NativeView view) const override {
    NSWindow* window = nil;
#if !defined(USE_AURA)
    if (view)
      window = [view window];
#endif
    if (!window)
      return GetPrimaryDisplay();
    NSScreen* match_screen = [window screen];
    if (!match_screen)
      return GetPrimaryDisplay();
    return GetDisplayForScreen(match_screen);
  }

  gfx::Display GetDisplayNearestPoint(const gfx::Point& point) const override {
    NSPoint ns_point = NSPointFromCGPoint(point.ToCGPoint());

    NSArray* screens = [NSScreen screens];
    NSScreen* primary = [screens objectAtIndex:0];
    ns_point.y = NSMaxY([primary frame]) - ns_point.y;
    for (NSScreen* screen in screens) {
      if (NSMouseInRect(ns_point, [screen frame], NO))
        return GetDisplayForScreen(screen);
    }
    return GetPrimaryDisplay();
  }

  // Returns the display that most closely intersects the provided bounds.
  gfx::Display GetDisplayMatching(const gfx::Rect& match_rect) const override {
    NSScreen* match_screen = GetMatchingScreen(match_rect);
    return GetDisplayForScreen(match_screen);
  }

  // Returns the primary display.
  gfx::Display GetPrimaryDisplay() const override {
    // Primary display is defined as the display with the menubar,
    // which is always at index 0.
    NSScreen* primary = [[NSScreen screens] firstObject];
    gfx::Display display = GetDisplayForScreen(primary);
    return display;
  }

  void AddObserver(gfx::DisplayObserver* observer) override {
    change_notifier_.AddObserver(observer);
  }

  void RemoveObserver(gfx::DisplayObserver* observer) override {
    change_notifier_.RemoveObserver(observer);
  }

  static void DisplayReconfigurationCallBack(CGDirectDisplayID display,
                                             CGDisplayChangeSummaryFlags flags,
                                             void* userInfo) {
    if (flags & kCGDisplayBeginConfigurationFlag)
      return;

    static_cast<ScreenMac*>(userInfo)->HandleDisplayReconfiguration();
  }

  void HandleDisplayReconfiguration() {
    // Given that we need to rebuild the list of displays, we want to coalesce
    // the events. For that, we use a timer that will be reset every time we get
    // a new event and will be fulfilled kConfigureDelayMs after the latest.
    if (configure_timer_.get() && configure_timer_->IsRunning()) {
      configure_timer_->Reset();
      return;
    }

    configure_timer_.reset(new base::OneShotTimer());
    configure_timer_->Start(
        FROM_HERE,
        base::TimeDelta::FromMilliseconds(kConfigureDelayMs),
        this,
        &ScreenMac::ConfigureTimerFired);
  }

 private:
  void ConfigureTimerFired() {
    std::vector<gfx::Display> old_displays = displays_;
    displays_ = BuildDisplaysFromQuartz();

    change_notifier_.NotifyDisplaysChanged(old_displays, displays_);
  }

  std::vector<gfx::Display> BuildDisplaysFromQuartz() const {
    // Don't just return all online displays.  This would include displays
    // that mirror other displays, which are not desired in this list.  It's
    // tempting to use the count returned by CGGetActiveDisplayList, but active
    // displays exclude sleeping displays, and those are desired.

    // It would be ridiculous to have this many displays connected, but
    // CGDirectDisplayID is just an integer, so supporting up to this many
    // doesn't hurt.
    CGDirectDisplayID online_displays[128];
    CGDisplayCount online_display_count = 0;
    if (CGGetOnlineDisplayList(arraysize(online_displays),
                              online_displays,
                              &online_display_count) != kCGErrorSuccess) {
      return std::vector<gfx::Display>(1, GetPrimaryDisplay());
    }

    typedef std::map<int64_t, NSScreen*> ScreenIdsToScreensMap;
    ScreenIdsToScreensMap screen_ids_to_screens;
    for (NSScreen* screen in [NSScreen screens]) {
      NSDictionary* screen_device_description = [screen deviceDescription];
      int64_t screen_id = [[screen_device_description
          objectForKey:@"NSScreenNumber"] unsignedIntValue];
      screen_ids_to_screens[screen_id] = screen;
    }

    std::vector<gfx::Display> displays;
    for (CGDisplayCount online_display_index = 0;
        online_display_index < online_display_count;
        ++online_display_index) {
      CGDirectDisplayID online_display = online_displays[online_display_index];
      if (CGDisplayMirrorsDisplay(online_display) == kCGNullDirectDisplay) {
        // If this display doesn't mirror any other, include it in the list.
        // The primary display in a mirrored set will be counted, but those that
        // mirror it will not be.
        ScreenIdsToScreensMap::iterator foundScreen =
          screen_ids_to_screens.find(online_display);
        if (foundScreen != screen_ids_to_screens.end()) {
            displays.push_back(GetDisplayForScreen(foundScreen->second));
        }
      }
    }

    if (!displays.size())
      return std::vector<gfx::Display>(1, GetPrimaryDisplay());

    return displays;
  }

  // The displays currently attached to the device.
  std::vector<gfx::Display> displays_;

  // The timer to delay configuring outputs. See also the comments in
  // HandleDisplayReconfiguration().
  scoped_ptr<base::OneShotTimer> configure_timer_;

  gfx::DisplayChangeNotifier change_notifier_;

  DISALLOW_COPY_AND_ASSIGN(ScreenMac);
};

}  // namespace

namespace gfx {

#if !defined(USE_AURA)
Screen* CreateNativeScreen() {
  return new ScreenMac;
}
#endif

}
