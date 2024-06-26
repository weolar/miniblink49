// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/touch_noise/single_position_touch_noise_filter.h"

#include <inttypes.h>

#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "base/strings/stringprintf.h"

namespace ui {

namespace {

    // Max squared distance between fingers for the fingers to be considered in the
    // same position.
    const int kSamePositionMaxDistance2 = 2 * 2;

    // Max squared movement of a finger before it's no longer considered noise.
    const int kNoiseMaxMovement2 = 2 * 2;

    // Min duration in milliseconds after which touches in the same position are
    // considered noise.
    const int kMinDurationMs = 2000;

    // Max duration in milliseconds to check for common positions with previous
    // touches.
    const int kMaxDurationMs = 4000;

    // Returns the squared distance between (|x1|, |y1|) and (|x2|, |y2|)
    int Distance2(int x1, int y1, int x2, int y2)
    {
        int offset_x = x2 - x1;
        int offset_y = y2 - y1;
        return offset_x * offset_x + offset_y * offset_y;
    }

} // namespace

SinglePositionTouchNoiseFilter::SinglePositionTouchNoiseFilter()
    : tracked_touches_start_(0)
    , tracked_touches_end_(0)
{
    for (size_t i = 0; i < kNumTouchEvdevSlots; ++i)
        tracked_slots_[i] = kNumTrackedTouches;
}

void SinglePositionTouchNoiseFilter::Filter(
    const std::vector<InProgressTouchEvdev>& touches,
    base::TimeDelta time,
    std::bitset<kNumTouchEvdevSlots>* slots_with_noise)
{
    // Forget old touches which will no longer be considered for overlap.
    base::TimeDelta touch_cutoff = time - base::TimeDelta::FromMilliseconds(kMaxDurationMs);
    for (size_t i = tracked_touches_start_; i != tracked_touches_end_;
         i = (i + 1) % kNumTrackedTouches) {
        if (!tracked_touches_[i].valid)
            continue;
        if (tracked_touches_[i].end < touch_cutoff)
            StopTrackingTouch(i);
    }

    for (const InProgressTouchEvdev& touch : touches) {
        size_t slot = touch.slot;

        bool arrived = touch.touching && !touch.was_touching;
        bool departed = !touch.touching && touch.was_touching;
        if (departed)
            tracked_slots_[slot] = kNumTrackedTouches;
        if (!touch.touching)
            continue;

        // Track all new touches until they move too far.
        if (arrived)
            TrackTouch(touch, time);

        size_t t_ind = tracked_slots_[slot];
        if (t_ind != kNumTrackedTouches) {
            tracked_touches_[t_ind].end = time;
            // Stop tracking if touch moves more than sqrt(kNoiseMaxMovement2).
            if (Distance2(touch.x, touch.y, tracked_touches_[t_ind].x,
                    tracked_touches_[t_ind].y)
                > kNoiseMaxMovement2) {
                StopTrackingTouch(t_ind);
            } else {
                // Determine duration over which touches have been occuring in this
                // position.
                base::TimeDelta max_duration;
                size_t max_duration_index = 0;
                for (size_t i = tracked_touches_start_; i != tracked_touches_end_;
                     i = (i + 1) % kNumTrackedTouches) {
                    TrackedTouch* tracked_touch = &tracked_touches_[i];
                    if (!tracked_touch->valid)
                        continue;
                    if (Distance2(touch.x, touch.y, tracked_touch->x, tracked_touch->y) <= kSamePositionMaxDistance2) {
                        base::TimeDelta duration = time - tracked_touch->begin;
                        if (duration > max_duration) {
                            max_duration = duration;
                            max_duration_index = i;
                        }
                    }
                }

                if (max_duration_index != t_ind && max_duration > tracked_touches_[t_ind].same_location_touch_age) {
                    tracked_touches_[t_ind].same_location_touch_age = max_duration;
                }

                if (max_duration.InMilliseconds() > kMinDurationMs) {
                    VLOG(2) << base::StringPrintf(
                        "Cancel tracking id %d, in position occurring for %" PRId64 "ms",
                        touch.tracking_id, max_duration.InMilliseconds());
                    slots_with_noise->set(slot);
                }
            }
        }
    }
}

void SinglePositionTouchNoiseFilter::StopTrackingTouch(size_t index)
{
    if (tracked_touches_[index].same_location_touch_age.InMilliseconds() > 0) {
        // Log the age of the oldest touch which occurred at |touch|'s location.
        // This is logged to allow tuning of |kMinDurationMs|.
        UMA_HISTOGRAM_TIMES(
            "Ozone.TouchNoiseFilter.TouchesAtSinglePositionDuration",
            tracked_touches_[index].same_location_touch_age);
    }

    size_t slot = tracked_touches_[index].slot;
    if (tracked_slots_[slot] == index)
        tracked_slots_[slot] = kNumTrackedTouches;
    tracked_touches_[index].valid = false;

    // If first touch is canceled, remove all dead touches.
    if (index == tracked_touches_start_) {
        while (!tracked_touches_[tracked_touches_start_].valid && tracked_touches_start_ != tracked_touches_end_) {
            tracked_touches_start_ = (tracked_touches_start_ + 1) % kNumTrackedTouches;
        }
    }
}

void SinglePositionTouchNoiseFilter::TrackTouch(
    const InProgressTouchEvdev& touch,
    base::TimeDelta time)
{
    size_t index = tracked_touches_end_;
    tracked_touches_end_ = (tracked_touches_end_ + 1) % kNumTrackedTouches;
    // If we would reach the start touch index, we cannot track any more touches.
    if (tracked_touches_end_ == tracked_touches_start_) {
        tracked_touches_end_ = index;
        return;
    }

    tracked_touches_[index].valid = true;
    tracked_touches_[index].slot = touch.slot;
    tracked_touches_[index].x = touch.x;
    tracked_touches_[index].y = touch.y;
    tracked_touches_[index].begin = time;
    tracked_touches_[index].end = time;
    tracked_slots_[touch.slot] = index;
}

SinglePositionTouchNoiseFilter::SinglePositionTouchNoiseFilter::TrackedTouch::
    TrackedTouch()
    : valid(false)
    , slot(0)
    , x(0)
    , y(0)
{
}

SinglePositionTouchNoiseFilter::SinglePositionTouchNoiseFilter::TrackedTouch::
    ~TrackedTouch()
{
}

} // namespace ui
