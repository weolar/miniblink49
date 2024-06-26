// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/touch_noise/far_apart_taps_touch_noise_filter.h"

#include <stddef.h>

#include <cmath>

#include "base/logging.h"
#include "base/macros.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/stringprintf.h"

namespace ui {

namespace {

    // Minimum squared distance between taps to be considered far apart.
    const int kMinDistance2 = 1500 * 1500;

    // The minimum squared distance between taps to log to UMA.
    const int kUmaMinDistance2 = 1000 * 1000;

    // The maximum squared distance between taps to log to UMA.
    const int kUmaMaxDistance2 = 3000 * 3000;

    // Max time between taps considered.
    const int kMaxTapDeltaMs = 30;

    // Maximum squared movement of a touch to still be considered a tap.
    const int kMaxTapMovement2 = 20 * 20;

    // Returns the squared distance between (|x1|, |y1|) and (|x2|, |y2|).
    int Distance2(int x1, int y1, int x2, int y2)
    {
        int offset_x = x2 - x1;
        int offset_y = y2 - y1;
        return offset_x * offset_x + offset_y * offset_y;
    }

} // namespace

void FarApartTapsTouchNoiseFilter::Filter(
    const std::vector<InProgressTouchEvdev>& touches,
    base::TimeDelta time,
    std::bitset<kNumTouchEvdevSlots>* slots_with_noise)
{
    // Remove old taps.
    base::TimeDelta tap_cutoff = time - base::TimeDelta::FromMilliseconds(kMaxTapDeltaMs);
    for (size_t i = 0; i < arraysize(tracked_taps_); ++i) {
        if (tracked_taps_[i].start < tap_cutoff)
            tracked_taps_[i].Invalidate();
    }

    for (const InProgressTouchEvdev& touch : touches) {
        // Only look at slots with active touches.
        if (!touch.touching && !touch.was_touching)
            continue;

        size_t slot = touch.slot;
        if (!touch.was_touching) {
            // Track new finger info.
            tracked_taps_[slot] = Tap(time, touch.x, touch.y);
        } else if (tracked_taps_[slot].is_valid()) {
            // Check if this finger has moved too far to be considered a tap.
            if (kMaxTapMovement2 < Distance2(touch.x, touch.y, tracked_taps_[slot].x,
                    tracked_taps_[slot].y)) {
                tracked_taps_[slot].Invalidate();
            }
        }

        if (tracked_taps_[slot].is_valid()) {
            // Check distance from other tracked taps.
            int min_distance2 = -1;
            for (size_t i = 0; i < arraysize(tracked_taps_); ++i) {
                if (i == slot || !tracked_taps_[i].is_valid())
                    continue;

                int dist2 = Distance2(tracked_taps_[i].x, tracked_taps_[i].y, touch.x, touch.y);
                if (min_distance2 < 0 || dist2 < min_distance2)
                    min_distance2 = dist2;
            }

            // Log |min_distance2| to a UMA histogram to allow tuning of
            // |kMinDistance2|.
            if (min_distance2 > kUmaMinDistance2) {
                UMA_HISTOGRAM_CUSTOM_COUNTS(
                    "Ozone.TouchNoiseFilter.FarApartTapDistance", min_distance2,
                    kUmaMinDistance2, kUmaMaxDistance2, 50);
            }

            if (min_distance2 > kMinDistance2) {
                // The other finger should see this one on its next frame and also
                // get canceled.
                VLOG(2) << base::StringPrintf(
                    "Cancel tracking id %d %.0fpx from other current taps.",
                    touch.tracking_id, sqrt(min_distance2));
                slots_with_noise->set(slot);
            }
        }

        if (!touch.touching)
            tracked_taps_[slot].Invalidate();
    }
}

} // namespace ui
