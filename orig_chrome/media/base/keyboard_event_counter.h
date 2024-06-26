// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_KEYBOARD_EVENT_COUNTER_H_
#define MEDIA_BASE_KEYBOARD_EVENT_COUNTER_H_

#include <set>

#include "base/synchronization/lock.h"
#include "media/base/media_export.h"
#include "ui/events/event_constants.h"
#include "ui/events/keycodes/keyboard_codes.h"

namespace media {

// This class tracks the total number of keypresses based on the OnKeyboardEvent
// calls it receives from the client.
// Multiple key down events for the same key are counted as one keypress until
// the same key is released.
class MEDIA_EXPORT KeyboardEventCounter {
public:
    KeyboardEventCounter();
    ~KeyboardEventCounter();

    // Resets the count to 0. Must be called on the same thread as
    // OnKeyboardEvent.
    void Reset();

    // Returns the total number of keypresses since its creation or last Reset()
    // call. Can be called on any thread.
    size_t GetKeyPressCount() const;

    // The client should call this method on key down or key up events.
    // Must be called on a single thread.
    void OnKeyboardEvent(ui::EventType event, ui::KeyboardCode key_code);

private:
    // The set of keys currently held down.
    std::set<ui::KeyboardCode> pressed_keys_;

    size_t total_key_presses_;

    DISALLOW_COPY_AND_ASSIGN(KeyboardEventCounter);
};

} // namespace media

#endif // MEDIA_BASE_KEYBOARD_EVENT_COUNTER_H_
