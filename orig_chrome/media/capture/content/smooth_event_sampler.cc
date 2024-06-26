// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/content/smooth_event_sampler.h"

#include <algorithm>

#include "base/trace_event/trace_event.h"

namespace media {

SmoothEventSampler::SmoothEventSampler(base::TimeDelta min_capture_period,
    int redundant_capture_goal)
    : redundant_capture_goal_(redundant_capture_goal)
    , overdue_sample_count_(0)
    , token_bucket_(base::TimeDelta::Max())
{
    SetMinCapturePeriod(min_capture_period);
}

void SmoothEventSampler::SetMinCapturePeriod(base::TimeDelta period)
{
    DCHECK_GT(period, base::TimeDelta());
    min_capture_period_ = period;
    token_bucket_capacity_ = period + period / 2;
    token_bucket_ = std::min(token_bucket_capacity_, token_bucket_);
}

void SmoothEventSampler::ConsiderPresentationEvent(base::TimeTicks event_time)
{
    DCHECK(!event_time.is_null());

    // Add tokens to the bucket based on advancement in time.  Then, re-bound the
    // number of tokens in the bucket.  Overflow occurs when there is too much
    // time between events (a common case), or when RecordSample() is not being
    // called often enough (a bug).  On the other hand, if RecordSample() is being
    // called too often (e.g., as a reaction to IsOverdueForSamplingAt()), the
    // bucket will underflow.
    if (!current_event_.is_null()) {
        if (current_event_ < event_time) {
            token_bucket_ += event_time - current_event_;
            if (token_bucket_ > token_bucket_capacity_)
                token_bucket_ = token_bucket_capacity_;
        }
        TRACE_COUNTER1("gpu.capture", "MirroringTokenBucketUsec",
            std::max<int64>(0, token_bucket_.InMicroseconds()));
    }
    current_event_ = event_time;
}

bool SmoothEventSampler::ShouldSample() const
{
    return token_bucket_ >= min_capture_period_;
}

void SmoothEventSampler::RecordSample()
{
    token_bucket_ -= min_capture_period_;
    if (token_bucket_ < base::TimeDelta())
        token_bucket_ = base::TimeDelta();
    TRACE_COUNTER1("gpu.capture", "MirroringTokenBucketUsec",
        std::max<int64>(0, token_bucket_.InMicroseconds()));

    if (HasUnrecordedEvent()) {
        last_sample_ = current_event_;
        overdue_sample_count_ = 0;
    } else {
        ++overdue_sample_count_;
    }
}

bool SmoothEventSampler::IsOverdueForSamplingAt(
    base::TimeTicks event_time) const
{
    DCHECK(!event_time.is_null());

    if (!HasUnrecordedEvent() && overdue_sample_count_ >= redundant_capture_goal_)
        return false; // Not dirty.

    if (last_sample_.is_null())
        return true;

    // If we're dirty but not yet old, then we've recently gotten updates, so we
    // won't request a sample just yet.
    base::TimeDelta dirty_interval = event_time - last_sample_;
    return dirty_interval >= base::TimeDelta::FromMilliseconds(OVERDUE_DIRTY_THRESHOLD_MILLIS);
}

bool SmoothEventSampler::HasUnrecordedEvent() const
{
    return !current_event_.is_null() && current_event_ != last_sample_;
}

} // namespace media
