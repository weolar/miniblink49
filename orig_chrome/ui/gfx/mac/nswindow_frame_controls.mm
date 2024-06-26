// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ui/gfx/mac/nswindow_frame_controls.h"

#import "base/mac/mac_util.h"
#import "base/mac/sdk_forward_declarations.h"
#include "ui/gfx/geometry/size.h"

namespace {

// The value used to represent an unbounded width or height.
const int kUnboundedSize = 0;

void SetResizableStyleMask(NSWindow* window, bool resizable) {
  NSUInteger style_mask = [window styleMask];
  if (resizable)
    style_mask |= NSResizableWindowMask;
  else
    style_mask &= ~NSResizableWindowMask;
  [window setStyleMask:style_mask];
}

// Returns the level for windows that are configured to be always on top.
// This is not a constant because NSFloatingWindowLevel is a macro defined
// as a function call.
NSInteger AlwaysOnTopWindowLevel() {
  return NSFloatingWindowLevel;
}

}  // namespace

namespace gfx {

void SetNSWindowCanFullscreen(NSWindow* window, bool allow_fullscreen) {
  NSWindowCollectionBehavior behavior = [window collectionBehavior];
  if (allow_fullscreen)
    behavior |= NSWindowCollectionBehaviorFullScreenPrimary;
  else
    behavior &= ~NSWindowCollectionBehaviorFullScreenPrimary;
  [window setCollectionBehavior:behavior];
}

bool IsNSWindowAlwaysOnTop(NSWindow* window) {
  return [window level] == AlwaysOnTopWindowLevel();
}

void SetNSWindowAlwaysOnTop(NSWindow* window,
                            bool always_on_top) {
  [window setLevel:(always_on_top ? AlwaysOnTopWindowLevel()
                                  : NSNormalWindowLevel)];
  // Since always-on-top windows have a higher window level than
  // NSNormalWindowLevel, they will default to
  // NSWindowCollectionBehaviorTransient. Set the value explicitly here to match
  // normal windows.
  NSWindowCollectionBehavior behavior =
      [window collectionBehavior] | NSWindowCollectionBehaviorManaged;
  [window setCollectionBehavior:behavior];
}

void SetNSWindowVisibleOnAllWorkspaces(NSWindow* window, bool always_visible) {
  NSWindowCollectionBehavior behavior = [window collectionBehavior];
  if (always_visible)
    behavior |= NSWindowCollectionBehaviorCanJoinAllSpaces;
  else
    behavior &= ~NSWindowCollectionBehaviorCanJoinAllSpaces;
  [window setCollectionBehavior:behavior];
}

void ApplyNSWindowSizeConstraints(NSWindow* window,
                                  const gfx::Size& min_size,
                                  const gfx::Size& max_size,
                                  bool can_resize,
                                  bool can_fullscreen) {
  [window setContentMinSize:NSMakeSize(min_size.width(), min_size.height())];

  CGFloat max_width =
      max_size.width() == kUnboundedSize ? CGFLOAT_MAX : max_size.width();
  CGFloat max_height =
      max_size.height() == kUnboundedSize ? CGFLOAT_MAX : max_size.height();
  [window setContentMaxSize:NSMakeSize(max_width, max_height)];

  SetResizableStyleMask(window, can_resize);
  [window setShowsResizeIndicator:can_resize];

  // Set the window to participate in Lion Fullscreen mode. Setting this flag
  // has no effect on Snow Leopard or earlier. UI controls for fullscreen are
  // only shown for windows that have unbounded size.
  if (base::mac::IsOSLionOrLater())
    SetNSWindowCanFullscreen(window, can_fullscreen);

  [[window standardWindowButton:NSWindowZoomButton] setEnabled:can_fullscreen];
}

}  // namespace gfx
