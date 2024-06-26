// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_WIN_SYSTEM_EVENT_STATE_LOOKUP_H_
#define UI_EVENTS_WIN_SYSTEM_EVENT_STATE_LOOKUP_H_

#include "ui/events/events_export.h"

namespace ui {
namespace win {

    // Returns true if the shift key is currently pressed.
    EVENTS_EXPORT bool IsShiftPressed();

    // Returns true if the ctrl key is currently pressed.
    EVENTS_EXPORT bool IsCtrlPressed();

    // Returns true if the alt key is currently pressed.
    EVENTS_EXPORT bool IsAltPressed();

    // Returns true if the altgr key is currently pressed.
    // Windows does not have specific key code and modifier bit and Alt+Ctrl key is
    // used as AltGr key in Windows.
    EVENTS_EXPORT bool IsAltGrPressed();

    // Returns true if the Windows key is currently pressed.
    EVENTS_EXPORT bool IsWindowsKeyPressed();

    // Returns true if the caps lock state is on.
    EVENTS_EXPORT bool IsCapsLockOn();

    // Returns true if the num lock state is on.
    EVENTS_EXPORT bool IsNumLockOn();

    // Returns true if the scroll lock state is on.
    EVENTS_EXPORT bool IsScrollLockOn();

} // namespace win
} // namespace ui

#endif // UI_EVENTS_WIN_SYSTEM_EVENT_STATE_LOOKUP_H_
