// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/frame_rate_counter.h"

#include <algorithm>
#include <limits>

#include "base/metrics/histogram.h"
#include "cc/trees/proxy.h"

namespace cc {

// The following constants are measured in seconds.

// Two thresholds (measured in seconds) that describe what is considered to be a
// "no-op frame" that should not be counted.
// - if the frame is too fast, then given our compositor implementation, the
// frame probably was a no-op and did not draw.
// - if the frame is too slow, then there is probably not animating content, so
// we should not pollute the average.
static const double kFrameTooFast = 1.0 / 70.0;
static const double kFrameTooSlow = 1.5;

// If a frame takes longer than this threshold (measured in seconds) then we
// (naively) assume that it missed a screen refresh; that is, we dropped a
// frame.
// TODO(brianderson): Determine this threshold based on monitor refresh rate,
// crbug.com/138642.
static const double kDroppedFrameTime = 1.0 / 50.0;

// static
scoped_ptr<FrameRateCounter> FrameRateCounter::Create(bool has_impl_thread)
{
    return make_scoped_ptr(new FrameRateCounter(has_impl_thread));
}

base::TimeDelta FrameRateCounter::RecentFrameInterval(size_t n) const
{
    DCHECK_GT(n, 0u);
    DCHECK_LT(n, ring_buffer_.BufferSize());
    return ring_buffer_.ReadBuffer(n) - ring_buffer_.ReadBuffer(n - 1);
}

FrameRateCounter::FrameRateCounter(bool has_impl_thread)
    : has_impl_thread_(has_impl_thread)
    , dropped_frame_count_(0)
{
}

void FrameRateCounter::SaveTimeStamp(base::TimeTicks timestamp, bool software)
{
    ring_buffer_.SaveToBuffer(timestamp);

    // Check if frame interval can be computed.
    if (ring_buffer_.CurrentIndex() < 2)
        return;

    base::TimeDelta frame_interval_seconds = RecentFrameInterval(ring_buffer_.BufferSize() - 1);

    if (has_impl_thread_ && ring_buffer_.CurrentIndex() > 0) {
        if (software) {
            UMA_HISTOGRAM_CUSTOM_COUNTS(
                "Renderer4.SoftwareCompositorThreadImplDrawDelay",
                frame_interval_seconds.InMilliseconds(),
                1,
                120,
                60);
        } else {
            UMA_HISTOGRAM_CUSTOM_COUNTS("Renderer4.CompositorThreadImplDrawDelay",
                frame_interval_seconds.InMilliseconds(),
                1,
                120,
                60);
        }
    }

    if (!IsBadFrameInterval(frame_interval_seconds) && frame_interval_seconds.InSecondsF() > kDroppedFrameTime)
        dropped_frame_count_ += frame_interval_seconds.InSecondsF() / kDroppedFrameTime;
}

bool FrameRateCounter::IsBadFrameInterval(
    base::TimeDelta interval_between_consecutive_frames) const
{
    double delta = interval_between_consecutive_frames.InSecondsF();
    bool scheduler_allows_double_frames = !has_impl_thread_;
    bool interval_too_fast = scheduler_allows_double_frames ? delta < kFrameTooFast : delta <= 0.0;
    bool interval_too_slow = delta > kFrameTooSlow;
    return interval_too_fast || interval_too_slow;
}

void FrameRateCounter::GetMinAndMaxFPS(double* min_fps, double* max_fps) const
{
    *min_fps = std::numeric_limits<double>::max();
    *max_fps = 0.0;

    for (RingBufferType::Iterator it = --ring_buffer_.End(); it; --it) {
        base::TimeDelta delta = RecentFrameInterval(it.index() + 1);

        if (IsBadFrameInterval(delta))
            continue;

        DCHECK_GT(delta.InSecondsF(), 0.f);
        double fps = 1.0 / delta.InSecondsF();

        *min_fps = std::min(fps, *min_fps);
        *max_fps = std::max(fps, *max_fps);
    }

    if (*min_fps > *max_fps)
        *min_fps = *max_fps;
}

double FrameRateCounter::GetAverageFPS() const
{
    int frame_count = 0;
    double frame_times_total = 0.0;
    double average_fps = 0.0;

    // Walk backwards through the samples looking for a run of good frame
    // timings from which to compute the mean.
    //
    // Slow frames occur just because the user is inactive, and should be
    // ignored. Fast frames are ignored if the scheduler is in single-thread
    // mode in order to represent the true frame rate in spite of the fact that
    // the first few swapbuffers happen instantly which skews the statistics
    // too much for short lived animations.
    //
    // IsBadFrameInterval encapsulates the frame too slow/frame too fast logic.

    for (RingBufferType::Iterator it = --ring_buffer_.End();
         it && frame_times_total < 1.0;
         --it) {
        base::TimeDelta delta = RecentFrameInterval(it.index() + 1);

        if (!IsBadFrameInterval(delta)) {
            frame_count++;
            frame_times_total += delta.InSecondsF();
        } else if (frame_count) {
            break;
        }
    }

    if (frame_count) {
        DCHECK_GT(frame_times_total, 0.0);
        average_fps = frame_count / frame_times_total;
    }

    return average_fps;
}

} // namespace cc
