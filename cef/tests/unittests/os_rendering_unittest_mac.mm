// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>

#include "os_rendering_unittest_mac.h"

#include "ui/events/keycodes/keyboard_code_conversion_mac.h"

#include "include/cef_base.h"

namespace osr_unittests {

CefWindowHandle GetFakeView() {
    NSScreen *mainScreen = [NSScreen mainScreen];
    NSRect screenRect = [mainScreen visibleFrame];
    NSView* fakeView = [[NSView alloc] initWithFrame: screenRect];
    return fakeView;
}

void GetKeyEvent(CefKeyEvent& event, ui::KeyboardCode keyCode, int modifiers) {
  unichar character;
  unichar unmodified_character;

  // TODO(port): translate modifiers from the input format to NSFlags
  // MacKeyCodeForWindowsKeyCode takes a NSUinteger as flags.
  int macKeyCode = ui::MacKeyCodeForWindowsKeyCode(keyCode,
                                                   modifiers,
                                                   &character,
                                                   &unmodified_character);

  event.native_key_code = macKeyCode;
  event.character = character;
  event.unmodified_character = unmodified_character;
}

}  // namespace osr_unittests
