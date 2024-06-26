// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_TOUCH_NOISE_TOUCH_NOISE_FINDER_H_
#define UI_EVENTS_OZONE_EVDEV_TOUCH_NOISE_TOUCH_NOISE_FINDER_H_

#include <stddef.h>

#include <bitset>
#include <vector>

#include "base/macros.h"
#include "base/time/time.h"
#include "ui/events/ozone/evdev/events_ozone_evdev_export.h"
#include "ui/events/ozone/evdev/touch_evdev_types.h"

namespace ui {
class TouchNoiseFilter;

// Finds touches which are likely touch noise.
class EVENTS_OZONE_EVDEV_EXPORT TouchNoiseFinder {
public:
    TouchNoiseFinder();
    ~TouchNoiseFinder();

    // Updates which ABS_MT_SLOTs are noise. |touches| should contain all of the
    // in-progress touches at |time| (including noisy touches).
    // |touches| should have at most one entry per ABS_MT_SLOT.
    void HandleTouches(const std::vector<InProgressTouchEvdev>& touches,
        base::TimeDelta time);

    // Returns whether the in-progress touch at ABS_MT_SLOT |slot| is noise.
    bool SlotHasNoise(size_t slot) const;

private:
    // Records how frequently noisy touches occur to UMA.
    void RecordUMA(bool had_noise, base::TimeDelta time);

    friend class TouchEventConverterEvdevTouchNoiseTest;

    // The slots which are noise.
    std::bitset<kNumTouchEvdevSlots> slots_with_noise_;

    // The time of the previous noise occurence in any of the slots.
    base::TimeDelta last_noise_time_;

    std::vector<TouchNoiseFilter*> filters_;

    DISALLOW_COPY_AND_ASSIGN(TouchNoiseFinder);
};

} // namespace ui

#endif // UI_EVENTS_OZONE_EVDEV_TOUCH_NOISE_TOUCH_NOISE_FINDER_H_
