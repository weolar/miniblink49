// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_TEST_COCOA_TEST_EVENT_UTILS_H_
#define UI_EVENTS_TEST_COCOA_TEST_EVENT_UTILS_H_

#include <utility>

#import <objc/objc-class.h>

#include "ui/events/keycodes/keyboard_codes.h"

namespace cocoa_test_event_utils {

// Create synthetic mouse events for testing. Currently these are very
// basic, flesh out as needed.  Points are all in window coordinates;
// where the window is not specified, coordinate system is undefined
// (but will be repeated when the event is queried).
NSEvent* MouseEventAtPoint(NSPoint point, NSEventType type,
    NSUInteger modifiers);
NSEvent* MouseEventWithType(NSEventType type, NSUInteger modifiers);
NSEvent* MouseEventAtPointInWindow(NSPoint point,
    NSEventType type,
    NSWindow* window,
    NSUInteger clickCount);
NSEvent* RightMouseDownAtPoint(NSPoint point);
NSEvent* RightMouseDownAtPointInWindow(NSPoint point, NSWindow* window);
NSEvent* LeftMouseDownAtPoint(NSPoint point);
NSEvent* LeftMouseDownAtPointInWindow(NSPoint point, NSWindow* window);

// Return a mouse down and an up event with the given |clickCount| at
// |view|'s midpoint.
std::pair<NSEvent*, NSEvent*> MouseClickInView(NSView* view,
    NSUInteger clickCount);

// Return a right mouse down and an up event with the given |clickCount| at
// |view|'s midpoint.
std::pair<NSEvent*, NSEvent*> RightMouseClickInView(NSView* view,
    NSUInteger clickCount);

// Returns a key event with the given character.
NSEvent* KeyEventWithCharacter(unichar c);

// Returns a key event with the given type and modifier flags.
NSEvent* KeyEventWithType(NSEventType event_type, NSUInteger modifiers);

// Returns a key event with the given key code, type, and modifier flags.
NSEvent* KeyEventWithKeyCode(unsigned short key_code,
    unichar c,
    NSEventType event_type,
    NSUInteger modifiers);

// Returns a mouse enter event.
NSEvent* EnterEvent();

// Returns a mouse exit event.
NSEvent* ExitEvent();

// Return an "other" event with the given type.
NSEvent* OtherEventWithType(NSEventType event_type);

// Time interval since system startup. Tests shouldn't rely on this.
NSTimeInterval TimeIntervalSinceSystemStartup();

// Creates a key event in a particular window.
NSEvent* SynthesizeKeyEvent(NSWindow* window,
    bool keyDown,
    ui::KeyboardCode keycode,
    NSUInteger flags);

} // namespace cocoa_test_event_utils

#endif // UI_EVENTS_TEST_COCOA_TEST_EVENT_UTILS_H_
