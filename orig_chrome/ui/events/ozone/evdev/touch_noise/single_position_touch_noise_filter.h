// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_EVDEV_TOUCH_NOISE_SINGLE_POSITION_TOUCH_NOISE_FILTER_H_
#define UI_EVENTS_OZONE_EVDEV_TOUCH_NOISE_SINGLE_POSITION_TOUCH_NOISE_FILTER_H_

#include <stddef.h>

#include "base/macros.h"
#include "base/time/time.h"
#include "ui/events/ozone/evdev/touch_noise/touch_noise_filter.h"

namespace ui {

class SinglePositionTouchNoiseFilter : public TouchNoiseFilter {
public:
    SinglePositionTouchNoiseFilter();
    ~SinglePositionTouchNoiseFilter() override { }

    // TouchNoiseFilter:
    void Filter(const std::vector<InProgressTouchEvdev>& touches,
        base::TimeDelta time,
        std::bitset<kNumTouchEvdevSlots>* slots_with_noise) override;

private:
    struct TrackedTouch {
        TrackedTouch();
        ~TrackedTouch();

        bool valid;
        size_t slot;
        int x;
        int y;
        base::TimeDelta begin;
        base::TimeDelta end;

        // The age of the oldest touch at the same location as this TrackedTouch.
        // Logged to UMA when we stop tracking the TrackedTouch.
        base::TimeDelta same_location_touch_age;
    };

    void StopTrackingTouch(size_t index);
    void TrackTouch(const InProgressTouchEvdev& touch,
        base::TimeDelta time);

    // A mapping of slot to tracked touch index in |tracked_touches_|.
    size_t tracked_slots_[kNumTouchEvdevSlots];

    // A circular buffer of tracked touches
    // [|tracked_touches_start_|, |tracked_touches_end_|).
    static const int kNumTrackedTouches = 100;
    TrackedTouch tracked_touches_[kNumTrackedTouches];

    size_t tracked_touches_start_;
    size_t tracked_touches_end_;
};

} // namespace ui

#endif // UI_EVENTS_OZONE_EVDEV_TOUCH_NOISE_SINGLE_POSITION_TOUCH_NOISE_FILTER_H_
