// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/rendering_stats_instrumentation.h"

namespace cc {

// static
scoped_ptr<RenderingStatsInstrumentation>
RenderingStatsInstrumentation::Create()
{
    return make_scoped_ptr(new RenderingStatsInstrumentation());
}

RenderingStatsInstrumentation::RenderingStatsInstrumentation()
    : record_rendering_stats_(false)
{
}

RenderingStatsInstrumentation::~RenderingStatsInstrumentation() { }

RenderingStats RenderingStatsInstrumentation::impl_thread_rendering_stats()
{
    base::AutoLock scoped_lock(lock_);
    return impl_thread_rendering_stats_;
}

RenderingStats RenderingStatsInstrumentation::GetRenderingStats()
{
    base::AutoLock scoped_lock(lock_);
    RenderingStats rendering_stats;
    rendering_stats = impl_thread_rendering_stats_accu_;
    rendering_stats.Add(impl_thread_rendering_stats_);
    return rendering_stats;
}

void RenderingStatsInstrumentation::AccumulateAndClearImplThreadStats()
{
    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_accu_.Add(impl_thread_rendering_stats_);
    impl_thread_rendering_stats_ = RenderingStats();
}

base::TimeDelta RenderingStatsInstrumentation::StartRecording() const
{
    if (record_rendering_stats_) {
        if (base::ThreadTicks::IsSupported())
            return base::ThreadTicks::Now() - base::ThreadTicks();
        return base::TimeTicks::Now() - base::TimeTicks();
    }
    return base::TimeDelta();
}

base::TimeDelta RenderingStatsInstrumentation::EndRecording(
    base::TimeDelta start_time) const
{
    if (start_time != base::TimeDelta()) {
        if (base::ThreadTicks::IsSupported())
            return (base::ThreadTicks::Now() - base::ThreadTicks()) - start_time;
        return (base::TimeTicks::Now() - base::TimeTicks()) - start_time;
    }
    return base::TimeDelta();
}

void RenderingStatsInstrumentation::IncrementFrameCount(int64 count)
{
    if (!record_rendering_stats_)
        return;

    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_.frame_count += count;
}

void RenderingStatsInstrumentation::AddVisibleContentArea(int64 area)
{
    if (!record_rendering_stats_)
        return;

    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_.visible_content_area += area;
}

void RenderingStatsInstrumentation::AddApproximatedVisibleContentArea(
    int64 area)
{
    if (!record_rendering_stats_)
        return;

    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_.approximated_visible_content_area += area;
}

void RenderingStatsInstrumentation::AddCheckerboardedVisibleContentArea(
    int64 area)
{
    if (!record_rendering_stats_)
        return;

    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_.checkerboarded_visible_content_area += area;
}

void RenderingStatsInstrumentation::AddCheckerboardedNoRecordingContentArea(
    int64 area)
{
    if (!record_rendering_stats_)
        return;

    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_.checkerboarded_no_recording_content_area += area;
}

void RenderingStatsInstrumentation::AddCheckerboardedNeedsRasterContentArea(
    int64 area)
{
    if (!record_rendering_stats_)
        return;

    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_.checkerboarded_needs_raster_content_area += area;
}

void RenderingStatsInstrumentation::AddDrawDuration(
    base::TimeDelta draw_duration,
    base::TimeDelta draw_duration_estimate)
{
    if (!record_rendering_stats_)
        return;

    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_.draw_duration.Append(draw_duration);
    impl_thread_rendering_stats_.draw_duration_estimate.Append(
        draw_duration_estimate);
}

void RenderingStatsInstrumentation::AddBeginMainFrameToCommitDuration(
    base::TimeDelta begin_main_frame_to_commit_duration,
    base::TimeDelta begin_main_frame_to_commit_duration_estimate)
{
    if (!record_rendering_stats_)
        return;

    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_.begin_main_frame_to_commit_duration.Append(
        begin_main_frame_to_commit_duration);
    impl_thread_rendering_stats_.begin_main_frame_to_commit_duration_estimate
        .Append(begin_main_frame_to_commit_duration_estimate);
}

void RenderingStatsInstrumentation::AddCommitToActivateDuration(
    base::TimeDelta commit_to_activate_duration,
    base::TimeDelta commit_to_activate_duration_estimate)
{
    if (!record_rendering_stats_)
        return;

    base::AutoLock scoped_lock(lock_);
    impl_thread_rendering_stats_.commit_to_activate_duration.Append(
        commit_to_activate_duration);
    impl_thread_rendering_stats_.commit_to_activate_duration_estimate.Append(
        commit_to_activate_duration_estimate);
}

} // namespace cc
