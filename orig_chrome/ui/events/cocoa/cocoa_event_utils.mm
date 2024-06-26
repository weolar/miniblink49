// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#import "ui/events/cocoa/cocoa_event_utils.h"

#include "ui/events/event_constants.h"
#include "ui/events/event_utils.h"

namespace {

bool IsLeftButtonEvent(NSEvent* event) {
  NSEventType type = [event type];
  return type == NSLeftMouseDown || type == NSLeftMouseDragged ||
         type == NSLeftMouseUp;
}

bool IsRightButtonEvent(NSEvent* event) {
  NSEventType type = [event type];
  return type == NSRightMouseDown || type == NSRightMouseDragged ||
         type == NSRightMouseUp;
}

bool IsMiddleButtonEvent(NSEvent* event) {
  if ([event buttonNumber] != 2)
    return false;

  NSEventType type = [event type];
  return type == NSOtherMouseDown || type == NSOtherMouseDragged ||
         type == NSOtherMouseUp;
}

}  // namespace

namespace ui {

int EventFlagsFromModifiers(NSUInteger modifiers) {
  int flags = 0;
  flags |= (modifiers & NSAlphaShiftKeyMask) ? ui::EF_CAPS_LOCK_ON : 0;
  flags |= (modifiers & NSShiftKeyMask) ? ui::EF_SHIFT_DOWN : 0;
  flags |= (modifiers & NSControlKeyMask) ? ui::EF_CONTROL_DOWN : 0;
  flags |= (modifiers & NSAlternateKeyMask) ? ui::EF_ALT_DOWN : 0;
  flags |= (modifiers & NSCommandKeyMask) ? ui::EF_COMMAND_DOWN : 0;
  return flags;
}

int EventFlagsFromNSEventWithModifiers(NSEvent* event, NSUInteger modifiers) {
  int flags = EventFlagsFromModifiers(modifiers);
  flags |= IsLeftButtonEvent(event) ? ui::EF_LEFT_MOUSE_BUTTON : 0;
  flags |= IsRightButtonEvent(event) ? ui::EF_RIGHT_MOUSE_BUTTON : 0;
  flags |= IsMiddleButtonEvent(event) ? ui::EF_MIDDLE_MOUSE_BUTTON : 0;
  return flags;
}

}  // namespace ui
