// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_KEYBOARD_EVDEV_H_
#define UI_EVENTS_OZONE_EVDEV_KEYBOARD_EVDEV_H_

#include <linux/input.h>

#include <bitset>

#include "base/macros.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "ui/events/ozone/evdev/event_device_util.h"
#include "ui/events/ozone/evdev/event_dispatch_callback.h"
#include "ui/events/ozone/evdev/events_ozone_evdev_export.h"
#include "ui/events/ozone/layout/keyboard_layout_engine.h"

namespace ui {

class EventModifiersEvdev;
enum class DomCode;

// Keyboard for evdev.
//
// This object is responsible for combining all attached keyboards into
// one logical keyboard, applying modifiers & implementing key repeat.
//
// It also currently also applies the layout.
class EVENTS_OZONE_EVDEV_EXPORT KeyboardEvdev {
public:
    KeyboardEvdev(EventModifiersEvdev* modifiers,
        KeyboardLayoutEngine* keyboard_layout_engine,
        const EventDispatchCallback& callback);
    ~KeyboardEvdev();

    // Handlers for raw key presses & releases.
    //
    // |code| is a Linux key code (from <linux/input.h>). |down| represents the
    // key state. |suppress_auto_repeat| prevents the event from triggering
    // auto-repeat, if enabled. |device_id| uniquely identifies the source
    // keyboard device.
    void OnKeyChange(unsigned int code,
        bool down,
        bool suppress_auto_repeat,
        base::TimeDelta timestamp,
        int device_id);

    // Handle Caps Lock modifier.
    void SetCapsLockEnabled(bool enabled);
    bool IsCapsLockEnabled();

    // Configuration for key repeat.
    bool IsAutoRepeatEnabled();
    void SetAutoRepeatEnabled(bool enabled);
    void SetAutoRepeatRate(const base::TimeDelta& delay,
        const base::TimeDelta& interval);
    void GetAutoRepeatRate(base::TimeDelta* delay, base::TimeDelta* interval);

    // Handle keyboard layout changes.
    bool SetCurrentLayoutByName(const std::string& layout_name);

private:
    void UpdateModifier(int modifier_flag, bool down);
    void RefreshModifiers();
    void UpdateCapsLockLed();
    void UpdateKeyRepeat(unsigned int key,
        bool down,
        bool suppress_auto_repeat,
        int device_id);
    void StartKeyRepeat(unsigned int key, int device_id);
    void StopKeyRepeat();
    void ScheduleKeyRepeat(const base::TimeDelta& delay);
    void OnRepeatTimeout(unsigned int sequence);
    void OnRepeatCommit(unsigned int sequence);
    void DispatchKey(unsigned int key,
        bool down,
        bool repeat,
        base::TimeDelta timestamp,
        int device_id);

    // Aggregated key state. There is only one bit of state per key; we do not
    // attempt to count presses of the same key on multiple keyboards.
    //
    // A key is down iff the most recent event pertaining to that key was a key
    // down event rather than a key up event. Therefore, a particular key position
    // can be considered released even if it is being depresssed on one or more
    // keyboards.
    std::bitset<KEY_CNT> key_state_;

    // Callback for dispatching events.
    EventDispatchCallback callback_;

    // Shared modifier state.
    EventModifiersEvdev* modifiers_;

    // Shared layout engine.
    KeyboardLayoutEngine* keyboard_layout_engine_;

    // Key repeat state.
    bool auto_repeat_enabled_ = true;
    unsigned int repeat_key_ = KEY_RESERVED;
    unsigned int repeat_sequence_ = 0;
    int repeat_device_id_ = 0;
    base::TimeDelta repeat_delay_;
    base::TimeDelta repeat_interval_;

    base::WeakPtrFactory<KeyboardEvdev> weak_ptr_factory_;

    DISALLOW_COPY_AND_ASSIGN(KeyboardEvdev);
};

} // namespace ui

#endif // UI_EVENTS_OZONE_EVDEV_KEYBOARD_EVDEV_H_
