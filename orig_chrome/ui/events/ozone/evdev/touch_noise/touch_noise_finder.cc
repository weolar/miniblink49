// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/touch_noise/touch_noise_finder.h"

#include "base/metrics/histogram_macros.h"
#include "base/stl_util.h"
#include "ui/events/event_utils.h"
#include "ui/events/ozone/evdev/touch_noise/far_apart_taps_touch_noise_filter.h"
#include "ui/events/ozone/evdev/touch_noise/horizontally_aligned_touch_noise_filter.h"
#include "ui/events/ozone/evdev/touch_noise/single_position_touch_noise_filter.h"
#include "ui/events/ozone/evdev/touch_noise/touch_noise_filter.h"

namespace ui {

TouchNoiseFinder::TouchNoiseFinder()
    : last_noise_time_(ui::EventTimeForNow())
{
    filters_.push_back(new FarApartTapsTouchNoiseFilter);
    filters_.push_back(new HorizontallyAlignedTouchNoiseFilter);
    filters_.push_back(new SinglePositionTouchNoiseFilter);
}

TouchNoiseFinder::~TouchNoiseFinder()
{
    STLDeleteElements(&filters_);
}

void TouchNoiseFinder::HandleTouches(
    const std::vector<InProgressTouchEvdev>& touches,
    base::TimeDelta time)
{
    for (const InProgressTouchEvdev& touch : touches) {
        if (!touch.was_touching)
            slots_with_noise_.set(touch.slot, false);
    }

    bool had_noise = slots_with_noise_.any();

    for (TouchNoiseFilter* filter : filters_)
        filter->Filter(touches, time, &slots_with_noise_);

    RecordUMA(had_noise, time);
}

bool TouchNoiseFinder::SlotHasNoise(size_t slot) const
{
    return slots_with_noise_.test(slot);
}

void TouchNoiseFinder::RecordUMA(bool had_noise, base::TimeDelta time)
{
    if (slots_with_noise_.any()) {
        if (!had_noise) {
            UMA_HISTOGRAM_LONG_TIMES(
                "Ozone.TouchNoiseFilter.TimeSinceLastNoiseOccurrence",
                time - last_noise_time_);
        }

        last_noise_time_ = time;
    }
}

} // namespace ui
