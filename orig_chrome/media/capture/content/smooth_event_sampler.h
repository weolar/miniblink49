// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAPTURE_SMOOTH_EVENT_SAMPLER_H_
#define MEDIA_CAPTURE_SMOOTH_EVENT_SAMPLER_H_

#include "base/time/time.h"
#include "media/base/media_export.h"

namespace media {

// Filters a sequence of events to achieve a target frequency.
class MEDIA_EXPORT SmoothEventSampler {
public:
    enum {
        // The maximum amount of time that can elapse before considering unchanged
        // content as dirty for the purposes of timer-based overdue sampling.  This
        // is the same value found in cc::FrameRateCounter.
        OVERDUE_DIRTY_THRESHOLD_MILLIS = 250 // 4 FPS
    };

    SmoothEventSampler(base::TimeDelta min_capture_period,
        int redundant_capture_goal);

    // Get/Set minimum capture period. When setting a new value, the state of the
    // sampler is retained so that sampling will continue smoothly.
    base::TimeDelta min_capture_period() const { return min_capture_period_; }
    void SetMinCapturePeriod(base::TimeDelta p);

    // Add a new event to the event history, and consider whether it ought to be
    // sampled. The event is not recorded as a sample until RecordSample() is
    // called.
    void ConsiderPresentationEvent(base::TimeTicks event_time);

    // Returns true if the last event considered should be sampled.
    bool ShouldSample() const;

    // Operates on the last event added by ConsiderPresentationEvent(), marking
    // it as sampled. After this point we are current in the stream of events, as
    // we have sampled the most recent event.
    void RecordSample();

    // Returns true if, at time |event_time|, sampling should occur because too
    // much time will have passed relative to the last event and/or sample.
    bool IsOverdueForSamplingAt(base::TimeTicks event_time) const;

    // Returns true if ConsiderPresentationEvent() has been called since the last
    // call to RecordSample().
    bool HasUnrecordedEvent() const;

private:
    base::TimeDelta min_capture_period_;
    const int redundant_capture_goal_;
    base::TimeDelta token_bucket_capacity_;

    base::TimeTicks current_event_;
    base::TimeTicks last_sample_;
    int overdue_sample_count_;
    base::TimeDelta token_bucket_;

    DISALLOW_COPY_AND_ASSIGN(SmoothEventSampler);
};

} // namespace media

#endif // MEDIA_CAPTURE_SMOOTH_EVENT_SAMPLER_H_
