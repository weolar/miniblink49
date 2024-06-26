// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/scheduler_test_common.h"

#include <string>

#include "base/logging.h"
#include "cc/debug/rendering_stats_instrumentation.h"

namespace cc {

void FakeDelayBasedTimeSourceClient::OnTimerTick()
{
    tick_called_ = true;
}

base::TimeTicks FakeDelayBasedTimeSource::Now() const { return now_; }

TestDelayBasedTimeSource::TestDelayBasedTimeSource(
    base::SimpleTestTickClock* now_src,
    base::TimeDelta interval,
    OrderedSimpleTaskRunner* task_runner)
    : DelayBasedTimeSource(interval, task_runner)
    , now_src_(now_src)
{
}

base::TimeTicks TestDelayBasedTimeSource::Now() const
{
    return now_src_->NowTicks();
}

std::string TestDelayBasedTimeSource::TypeString() const
{
    return "TestDelayBasedTimeSource";
}

TestDelayBasedTimeSource::~TestDelayBasedTimeSource()
{
}

void FakeBeginFrameSource::DidFinishFrame(size_t remaining_frames)
{
    remaining_frames_ = remaining_frames;
}

void FakeBeginFrameSource::AsValueInto(
    base::trace_event::TracedValue* dict) const
{
    dict->SetString("type", "FakeBeginFrameSource");
    BeginFrameSourceBase::AsValueInto(dict);
}

TestBackToBackBeginFrameSource::TestBackToBackBeginFrameSource(
    base::SimpleTestTickClock* now_src,
    base::SingleThreadTaskRunner* task_runner)
    : BackToBackBeginFrameSource(task_runner)
    , now_src_(now_src)
{
}

TestBackToBackBeginFrameSource::~TestBackToBackBeginFrameSource()
{
}

base::TimeTicks TestBackToBackBeginFrameSource::Now()
{
    return now_src_->NowTicks();
}

TestSyntheticBeginFrameSource::TestSyntheticBeginFrameSource(
    scoped_ptr<DelayBasedTimeSource> time_source)
    : SyntheticBeginFrameSource(time_source.Pass())
{
}

TestSyntheticBeginFrameSource::~TestSyntheticBeginFrameSource()
{
}

scoped_ptr<FakeCompositorTimingHistory> FakeCompositorTimingHistory::Create()
{
    scoped_ptr<RenderingStatsInstrumentation> rendering_stats_instrumentation = RenderingStatsInstrumentation::Create();
    return make_scoped_ptr(
        new FakeCompositorTimingHistory(rendering_stats_instrumentation.Pass()));
}

FakeCompositorTimingHistory::FakeCompositorTimingHistory(
    scoped_ptr<RenderingStatsInstrumentation> rendering_stats_instrumentation)
    : CompositorTimingHistory(CompositorTimingHistory::NULL_UMA,
        rendering_stats_instrumentation.get())
    , rendering_stats_instrumentation_owned_(
          rendering_stats_instrumentation.Pass())
{
}

FakeCompositorTimingHistory::~FakeCompositorTimingHistory()
{
}

void FakeCompositorTimingHistory::SetAllEstimatesTo(base::TimeDelta duration)
{
    begin_main_frame_to_commit_duration_ = duration;
    commit_to_ready_to_activate_duration_ = duration;
    prepare_tiles_duration_ = duration;
    activate_duration_ = duration;
    draw_duration_ = duration;
}

void FakeCompositorTimingHistory::SetBeginMainFrameToCommitDurationEstimate(
    base::TimeDelta duration)
{
    begin_main_frame_to_commit_duration_ = duration;
}

void FakeCompositorTimingHistory::SetCommitToReadyToActivateDurationEstimate(
    base::TimeDelta duration)
{
    commit_to_ready_to_activate_duration_ = duration;
}

void FakeCompositorTimingHistory::SetPrepareTilesDurationEstimate(
    base::TimeDelta duration)
{
    prepare_tiles_duration_ = duration;
}

void FakeCompositorTimingHistory::SetActivateDurationEstimate(
    base::TimeDelta duration)
{
    activate_duration_ = duration;
}

void FakeCompositorTimingHistory::SetDrawDurationEstimate(
    base::TimeDelta duration)
{
    draw_duration_ = duration;
}

base::TimeDelta
FakeCompositorTimingHistory::BeginMainFrameToCommitDurationEstimate() const
{
    return begin_main_frame_to_commit_duration_;
}

base::TimeDelta
FakeCompositorTimingHistory::CommitToReadyToActivateDurationEstimate() const
{
    return commit_to_ready_to_activate_duration_;
}

base::TimeDelta FakeCompositorTimingHistory::PrepareTilesDurationEstimate()
    const
{
    return prepare_tiles_duration_;
}

base::TimeDelta FakeCompositorTimingHistory::ActivateDurationEstimate() const
{
    return activate_duration_;
}

base::TimeDelta FakeCompositorTimingHistory::DrawDurationEstimate() const
{
    return draw_duration_;
}

scoped_ptr<TestScheduler> TestScheduler::Create(
    base::SimpleTestTickClock* now_src,
    SchedulerClient* client,
    const SchedulerSettings& settings,
    int layer_tree_host_id,
    OrderedSimpleTaskRunner* task_runner,
    BeginFrameSource* external_frame_source,
    scoped_ptr<CompositorTimingHistory> compositor_timing_history)
{
    scoped_ptr<TestSyntheticBeginFrameSource> synthetic_frame_source;
    if (!settings.use_external_begin_frame_source) {
        synthetic_frame_source = TestSyntheticBeginFrameSource::Create(
            now_src, task_runner, BeginFrameArgs::DefaultInterval());
    }
    scoped_ptr<TestBackToBackBeginFrameSource> unthrottled_frame_source = TestBackToBackBeginFrameSource::Create(now_src, task_runner);
    return make_scoped_ptr(new TestScheduler(
        now_src, client, settings, layer_tree_host_id, task_runner,
        external_frame_source, synthetic_frame_source.Pass(),
        unthrottled_frame_source.Pass(), compositor_timing_history.Pass()));
}

TestScheduler::TestScheduler(
    base::SimpleTestTickClock* now_src,
    SchedulerClient* client,
    const SchedulerSettings& scheduler_settings,
    int layer_tree_host_id,
    OrderedSimpleTaskRunner* task_runner,
    BeginFrameSource* external_frame_source,
    scoped_ptr<TestSyntheticBeginFrameSource> synthetic_frame_source,
    scoped_ptr<TestBackToBackBeginFrameSource> unthrottled_frame_source,
    scoped_ptr<CompositorTimingHistory> compositor_timing_history)
    : Scheduler(client,
        scheduler_settings,
        layer_tree_host_id,
        task_runner,
        external_frame_source,
        synthetic_frame_source.Pass(),
        unthrottled_frame_source.Pass(),
        compositor_timing_history.Pass())
    , now_src_(now_src)
{
}

base::TimeTicks TestScheduler::Now() const
{
    return now_src_->NowTicks();
}

TestScheduler::~TestScheduler()
{
}

} // namespace cc
