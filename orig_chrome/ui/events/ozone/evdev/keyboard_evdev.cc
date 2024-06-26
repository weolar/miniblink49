// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/keyboard_evdev.h"

#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"
#include "ui/events/event.h"
#include "ui/events/event_constants.h"
#include "ui/events/event_utils.h"
#include "ui/events/keycodes/dom/dom_code.h"
#include "ui/events/keycodes/dom/keycode_converter.h"
#include "ui/events/ozone/evdev/event_modifiers_evdev.h"
#include "ui/events/ozone/evdev/keyboard_util_evdev.h"
#include "ui/events/ozone/layout/keyboard_layout_engine.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/ozone/layout/layout_util.h"

namespace ui {

namespace {

    const int kRepeatDelayMs = 500;
    const int kRepeatIntervalMs = 50;

    int EventFlagToEvdevModifier(int flag)
    {
        switch (flag) {
        case EF_SHIFT_DOWN:
            return EVDEV_MODIFIER_SHIFT;
        case EF_CONTROL_DOWN:
            return EVDEV_MODIFIER_CONTROL;
        case EF_ALT_DOWN:
            return EVDEV_MODIFIER_ALT;
        case EF_COMMAND_DOWN:
            return EVDEV_MODIFIER_COMMAND;
        case EF_ALTGR_DOWN:
            return EVDEV_MODIFIER_ALTGR;
        case EF_MOD3_DOWN:
            return EVDEV_MODIFIER_MOD3;
        case EF_CAPS_LOCK_ON:
            return EVDEV_MODIFIER_CAPS_LOCK;
        case EF_LEFT_MOUSE_BUTTON:
            return EVDEV_MODIFIER_LEFT_MOUSE_BUTTON;
        case EF_MIDDLE_MOUSE_BUTTON:
            return EVDEV_MODIFIER_MIDDLE_MOUSE_BUTTON;
        case EF_RIGHT_MOUSE_BUTTON:
            return EVDEV_MODIFIER_RIGHT_MOUSE_BUTTON;
        case EF_BACK_MOUSE_BUTTON:
            return EVDEV_MODIFIER_BACK_MOUSE_BUTTON;
        case EF_FORWARD_MOUSE_BUTTON:
            return EVDEV_MODIFIER_FORWARD_MOUSE_BUTTON;
        default:
            return EVDEV_MODIFIER_NONE;
        }
    }

} // namespace

KeyboardEvdev::KeyboardEvdev(EventModifiersEvdev* modifiers,
    KeyboardLayoutEngine* keyboard_layout_engine,
    const EventDispatchCallback& callback)
    : callback_(callback)
    , modifiers_(modifiers)
    , keyboard_layout_engine_(keyboard_layout_engine)
    , weak_ptr_factory_(this)
{
    repeat_delay_ = base::TimeDelta::FromMilliseconds(kRepeatDelayMs);
    repeat_interval_ = base::TimeDelta::FromMilliseconds(kRepeatIntervalMs);
}

KeyboardEvdev::~KeyboardEvdev()
{
}

void KeyboardEvdev::OnKeyChange(unsigned int key,
    bool down,
    bool suppress_auto_repeat,
    base::TimeDelta timestamp,
    int device_id)
{
    if (key > KEY_MAX)
        return;

    bool was_down = key_state_.test(key);
    bool is_repeat = down && was_down;
    if (!down && !was_down)
        return; // Key already released.

    key_state_.set(key, down);
    UpdateKeyRepeat(key, down, suppress_auto_repeat, device_id);
    DispatchKey(key, down, is_repeat, timestamp, device_id);
}

void KeyboardEvdev::SetCapsLockEnabled(bool enabled)
{
    modifiers_->SetModifierLock(EVDEV_MODIFIER_CAPS_LOCK, enabled);
}

bool KeyboardEvdev::IsCapsLockEnabled()
{
    return (modifiers_->GetModifierFlags() & EF_CAPS_LOCK_ON) != 0;
}

bool KeyboardEvdev::IsAutoRepeatEnabled()
{
    return auto_repeat_enabled_;
}

void KeyboardEvdev::SetAutoRepeatEnabled(bool enabled)
{
    auto_repeat_enabled_ = enabled;
}

void KeyboardEvdev::SetAutoRepeatRate(const base::TimeDelta& delay,
    const base::TimeDelta& interval)
{
    repeat_delay_ = delay;
    repeat_interval_ = interval;
}

void KeyboardEvdev::GetAutoRepeatRate(base::TimeDelta* delay,
    base::TimeDelta* interval)
{
    *delay = repeat_delay_;
    *interval = repeat_interval_;
}

bool KeyboardEvdev::SetCurrentLayoutByName(const std::string& layout_name)
{
    bool result = keyboard_layout_engine_->SetCurrentLayoutByName(layout_name);
    RefreshModifiers();
    return result;
}

void KeyboardEvdev::UpdateModifier(int modifier_flag, bool down)
{
    if (modifier_flag == EF_NONE)
        return;

    int modifier = EventFlagToEvdevModifier(modifier_flag);
    if (modifier == EVDEV_MODIFIER_NONE)
        return;

    // TODO post-X11: Revise remapping to not use EF_MOD3_DOWN.
    // Currently EF_MOD3_DOWN means that the CapsLock key is currently down,
    // and EF_CAPS_LOCK_ON means the caps lock state is enabled (and the
    // key may or may not be down, but usually isn't). There does need to
    // to be two different flags, since the physical CapsLock key is subject
    // to remapping, but the caps lock state (which can be triggered in a
    // variety of ways) is not.
    if (modifier == EVDEV_MODIFIER_CAPS_LOCK)
        modifiers_->UpdateModifier(EVDEV_MODIFIER_MOD3, down);
    else
        modifiers_->UpdateModifier(modifier, down);
}

void KeyboardEvdev::RefreshModifiers()
{
    // Release all keyboard modifiers.
    modifiers_->ResetKeyboardModifiers();
    // Press modifiers for currently held keys.
    for (int key = 0; key < KEY_CNT; ++key) {
        if (!key_state_.test(key))
            continue;
        DomCode dom_code = KeycodeConverter::NativeKeycodeToDomCode(EvdevCodeToNativeCode(key));
        if (dom_code == DomCode::NONE)
            continue;
        DomKey dom_key;
        KeyboardCode keycode;
        if (!keyboard_layout_engine_->Lookup(dom_code, EF_NONE, &dom_key, &keycode))
            continue;
        int flag = ModifierDomKeyToEventFlag(dom_key);
        if (flag == EF_NONE)
            continue;
        UpdateModifier(flag, true);
    }
}

void KeyboardEvdev::UpdateKeyRepeat(unsigned int key,
    bool down,
    bool suppress_auto_repeat,
    int device_id)
{
    if (!auto_repeat_enabled_ || suppress_auto_repeat)
        StopKeyRepeat();
    else if (key != repeat_key_ && down)
        StartKeyRepeat(key, device_id);
    else if (key == repeat_key_ && !down)
        StopKeyRepeat();
}

void KeyboardEvdev::StartKeyRepeat(unsigned int key, int device_id)
{
    repeat_key_ = key;
    repeat_device_id_ = device_id;
    repeat_sequence_++;

    ScheduleKeyRepeat(repeat_delay_);
}

void KeyboardEvdev::StopKeyRepeat()
{
    repeat_key_ = KEY_RESERVED;
    repeat_sequence_++;
}

void KeyboardEvdev::ScheduleKeyRepeat(const base::TimeDelta& delay)
{
    base::ThreadTaskRunnerHandle::Get()->PostDelayedTask(
        FROM_HERE, base::Bind(&KeyboardEvdev::OnRepeatTimeout, weak_ptr_factory_.GetWeakPtr(), repeat_sequence_),
        delay);
}

void KeyboardEvdev::OnRepeatTimeout(unsigned int sequence)
{
    if (repeat_sequence_ != sequence)
        return;

    // Post a task behind any pending key releases in the message loop
    // FIFO. This ensures there's no spurious repeats during periods of UI
    // thread jank.
    base::ThreadTaskRunnerHandle::Get()->PostTask(
        FROM_HERE, base::Bind(&KeyboardEvdev::OnRepeatCommit, weak_ptr_factory_.GetWeakPtr(), repeat_sequence_));
}

void KeyboardEvdev::OnRepeatCommit(unsigned int sequence)
{
    if (repeat_sequence_ != sequence)
        return;

    DispatchKey(repeat_key_, true /* down */, true /* repeat */,
        EventTimeForNow(), repeat_device_id_);

    ScheduleKeyRepeat(repeat_interval_);
}

void KeyboardEvdev::DispatchKey(unsigned int key,
    bool down,
    bool repeat,
    base::TimeDelta timestamp,
    int device_id)
{
    DomCode dom_code = KeycodeConverter::NativeKeycodeToDomCode(EvdevCodeToNativeCode(key));
    if (dom_code == DomCode::NONE)
        return;
    int flags = modifiers_->GetModifierFlags();
    DomKey dom_key;
    KeyboardCode key_code;
    if (!keyboard_layout_engine_->Lookup(dom_code, flags, &dom_key, &key_code))
        return;
    if (!repeat) {
        int flag = ModifierDomKeyToEventFlag(dom_key);
        UpdateModifier(flag, down);
    }

    KeyEvent event(down ? ET_KEY_PRESSED : ET_KEY_RELEASED, key_code, dom_code,
        modifiers_->GetModifierFlags(), dom_key, timestamp);
    event.set_source_device_id(device_id);
    callback_.Run(&event);
}

} // namespace ui
