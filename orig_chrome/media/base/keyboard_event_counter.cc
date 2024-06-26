// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/keyboard_event_counter.h"

#include "base/atomicops.h"
#include "base/logging.h"

namespace media {

KeyboardEventCounter::KeyboardEventCounter()
    : total_key_presses_(0)
{
}

KeyboardEventCounter::~KeyboardEventCounter() { }

void KeyboardEventCounter::Reset()
{
    pressed_keys_.clear();
    base::subtle::NoBarrier_Store(
        reinterpret_cast<base::subtle::AtomicWord*>(&total_key_presses_), 0);
}

void KeyboardEventCounter::OnKeyboardEvent(ui::EventType event,
    ui::KeyboardCode key_code)
{
    // Updates the pressed keys and the total count of key presses.
    if (event == ui::ET_KEY_PRESSED) {
        if (pressed_keys_.find(key_code) != pressed_keys_.end())
            return;
        pressed_keys_.insert(key_code);
        base::subtle::NoBarrier_AtomicIncrement(
            reinterpret_cast<base::subtle::AtomicWord*>(&total_key_presses_), 1);
    } else {
        DCHECK_EQ(ui::ET_KEY_RELEASED, event);
        pressed_keys_.erase(key_code);
    }
}

size_t KeyboardEventCounter::GetKeyPressCount() const
{
    return base::subtle::NoBarrier_Load(
        reinterpret_cast<const base::subtle::AtomicWord*>(&total_key_presses_));
}

} // namespace media
