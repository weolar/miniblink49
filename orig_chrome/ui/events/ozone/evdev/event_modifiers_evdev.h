// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_EVENT_MODIFIERS_EVDEV_H_
#define UI_EVENTS_OZONE_EVDEV_EVENT_MODIFIERS_EVDEV_H_

#include "base/macros.h"
#include "ui/events/ozone/evdev/events_ozone_evdev_export.h"

namespace ui {

enum {
    EVDEV_MODIFIER_NONE,
    EVDEV_MODIFIER_SHIFT,
    EVDEV_MODIFIER_CONTROL,
    EVDEV_MODIFIER_ALT,
    EVDEV_MODIFIER_COMMAND,
    EVDEV_MODIFIER_ALTGR,
    EVDEV_MODIFIER_MOD3,
    EVDEV_MODIFIER_CAPS_LOCK,
    EVDEV_MODIFIER_LEFT_MOUSE_BUTTON,
    EVDEV_MODIFIER_MIDDLE_MOUSE_BUTTON,
    EVDEV_MODIFIER_RIGHT_MOUSE_BUTTON,
    EVDEV_MODIFIER_BACK_MOUSE_BUTTON,
    EVDEV_MODIFIER_FORWARD_MOUSE_BUTTON,
    EVDEV_NUM_MODIFIERS
};

// Modifier key state for Evdev.
//
// Chrome relies on the underlying OS to interpret modifier keys such as Shift,
// Ctrl, and Alt. The Linux input subsystem does not assign any special meaning
// to these keys, so this work must happen at a higher layer (normally X11 or
// the console driver). When using evdev directly, we must do it ourselves.
//
// The modifier state is shared between all input devices connected to the
// system. This is to support actions such as Shift-Clicking that use multiple
// devices.
//
// Normally a modifier is set if any of the keys or buttons assigned to it are
// currently pressed. However some keys toggle a persistent "lock" for the
// modifier instead, such as CapsLock. If a modifier is "locked" then its state
// is inverted until it is unlocked.
class EVENTS_OZONE_EVDEV_EXPORT EventModifiersEvdev {
public:
    EventModifiersEvdev();
    ~EventModifiersEvdev();

    // Record key press or release for regular modifier key (shift, alt, etc).
    void UpdateModifier(unsigned int modifier, bool down);

    // Record key press or release for locking modifier key (caps lock).
    void UpdateModifierLock(unsigned int modifier, bool down);

    // Directly set the state of a locking modifier key (caps lock).
    void SetModifierLock(unsigned int modifier, bool locked);

    // Return current flags to use for incoming events.
    int GetModifierFlags();

    // Release modifier keys.
    void ResetKeyboardModifiers();

    // Return the mask for the specified modifier.
    static int GetEventFlagFromModifier(unsigned int modifier);

private:
    // Count of keys pressed for each modifier.
    int modifiers_down_[EVDEV_NUM_MODIFIERS];

    // Mask of modifier flags currently "locked".
    int modifier_flags_locked_ = 0;

    // Mask of modifier flags currently active (nonzero keys pressed xor locked).
    int modifier_flags_ = 0;

    // Update modifier_flags_ from modifiers_down_ and modifier_flags_locked_.
    void UpdateFlags(unsigned int modifier);

    DISALLOW_COPY_AND_ASSIGN(EventModifiersEvdev);
};

} // namspace ui

#endif // UI_EVENTS_OZONE_EVDEV_EVENT_MODIFIERS_EVDEV_H_
