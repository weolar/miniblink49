// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/content/feedback_signal_accumulator.h"

#include <algorithm>
#include <cmath>

namespace media {

FeedbackSignalAccumulator::FeedbackSignalAccumulator(base::TimeDelta half_life)
    : half_life_(half_life)
{
    DCHECK(half_life_ > base::TimeDelta());
}

void FeedbackSignalAccumulator::Reset(double starting_value,
    base::TimeTicks timestamp)
{
    DCHECK(!timestamp.is_null());
    average_ = update_value_ = prior_average_ = starting_value;
    reset_time_ = update_time_ = prior_update_time_ = timestamp;
}

bool FeedbackSignalAccumulator::Update(double value,
    base::TimeTicks timestamp)
{
    DCHECK(!reset_time_.is_null());

    if (timestamp < update_time_) {
        return false; // Not in chronological order.
    } else if (timestamp == update_time_) {
        if (timestamp == reset_time_) {
            // Edge case: Multiple updates at reset timestamp.
            average_ = update_value_ = prior_average_ = std::max(value, update_value_);
            return true;
        }
        if (value <= update_value_)
            return true;
        update_value_ = value;
    } else {
        prior_average_ = average_;
        prior_update_time_ = update_time_;
        update_value_ = value;
        update_time_ = timestamp;
    }

    const double elapsed_us = static_cast<double>((update_time_ - prior_update_time_).InMicroseconds());
    const double weight = elapsed_us / (elapsed_us + half_life_.InMicroseconds());
    average_ = weight * update_value_ + (1.0 - weight) * prior_average_;
    DCHECK(std::isfinite(average_));

    return true;
}

} // namespace media
