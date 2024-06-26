// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/scheduler/compositor_timing_history.h"

#include "base/metrics/histogram.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/debug/rendering_stats_instrumentation.h"

namespace cc {

class CompositorTimingHistory::UMAReporter {
public:
    virtual ~UMAReporter() { }

    virtual void AddBeginMainFrameToCommitDuration(base::TimeDelta duration,
        base::TimeDelta estimate,
        bool affects_estimate)
        = 0;
    virtual void AddCommitToReadyToActivateDuration(base::TimeDelta duration,
        base::TimeDelta estimate,
        bool affects_estimate)
        = 0;
    virtual void AddPrepareTilesDuration(base::TimeDelta duration,
        base::TimeDelta estimate,
        bool affects_estimate)
        = 0;
    virtual void AddActivateDuration(base::TimeDelta duration,
        base::TimeDelta estimate,
        bool affects_estimate)
        = 0;
    virtual void AddDrawDuration(base::TimeDelta duration,
        base::TimeDelta estimate,
        bool affects_estimate)
        = 0;
};

namespace {

    // Using the 90th percentile will disable latency recovery
    // if we are missing the deadline approximately ~6 times per
    // second.
    // TODO(brianderson): Fine tune the percentiles below.
    const size_t kDurationHistorySize = 60;
    const double kBeginMainFrameToCommitEstimationPercentile = 90.0;
    const double kCommitToReadyToActivateEstimationPercentile = 90.0;
    const double kPrepareTilesEstimationPercentile = 90.0;
    const double kActivateEstimationPercentile = 90.0;
    const double kDrawEstimationPercentile = 90.0;

    const int kUmaDurationMinMicros = 1;
    const int64 kUmaDurationMaxMicros = 1 * base::Time::kMicrosecondsPerSecond;
    const size_t kUmaDurationBucketCount = 100;

    // Deprecated because they combine Browser and Renderer stats and have low
    // precision.
    // TODO(brianderson): Remove.
    void DeprecatedDrawDurationUMA(base::TimeDelta duration,
        base::TimeDelta estimate)
    {
        base::TimeDelta duration_overestimate;
        base::TimeDelta duration_underestimate;
        if (duration > estimate)
            duration_underestimate = duration - estimate;
        else
            duration_overestimate = estimate - duration;
        UMA_HISTOGRAM_CUSTOM_TIMES("Renderer.DrawDuration", duration,
            base::TimeDelta::FromMilliseconds(1),
            base::TimeDelta::FromMilliseconds(100), 50);
        UMA_HISTOGRAM_CUSTOM_TIMES("Renderer.DrawDurationUnderestimate",
            duration_underestimate,
            base::TimeDelta::FromMilliseconds(1),
            base::TimeDelta::FromMilliseconds(100), 50);
        UMA_HISTOGRAM_CUSTOM_TIMES("Renderer.DrawDurationOverestimate",
            duration_overestimate,
            base::TimeDelta::FromMilliseconds(1),
            base::TimeDelta::FromMilliseconds(100), 50);
    }

#define UMA_HISTOGRAM_CUSTOM_TIMES_MICROS(name, sample)        \
    UMA_HISTOGRAM_CUSTOM_COUNTS(name, sample.InMicroseconds(), \
        kUmaDurationMinMicros, kUmaDurationMaxMicros,          \
        kUmaDurationBucketCount);

#define REPORT_COMPOSITOR_TIMING_HISTORY_UMA(category, subcategory)                  \
    do {                                                                             \
        base::TimeDelta duration_overestimate;                                       \
        base::TimeDelta duration_underestimate;                                      \
        if (duration > estimate)                                                     \
            duration_underestimate = duration - estimate;                            \
        else                                                                         \
            duration_overestimate = estimate - duration;                             \
        UMA_HISTOGRAM_CUSTOM_TIMES_MICROS(                                           \
            "Scheduling." category "." subcategory "Duration", duration);            \
        UMA_HISTOGRAM_CUSTOM_TIMES_MICROS("Scheduling." category "." subcategory     \
                                          "Duration.Underestimate",                  \
            duration_underestimate);                                                 \
        UMA_HISTOGRAM_CUSTOM_TIMES_MICROS("Scheduling." category "." subcategory     \
                                          "Duration.Overestimate",                   \
            duration_overestimate);                                                  \
        if (!affects_estimate) {                                                     \
            UMA_HISTOGRAM_CUSTOM_TIMES_MICROS("Scheduling." category "." subcategory \
                                              "Duration.NotUsedForEstimate",         \
                duration);                                                           \
        }                                                                            \
    } while (false)

    class RendererUMAReporter : public CompositorTimingHistory::UMAReporter {
    public:
        ~RendererUMAReporter() override { }

        void AddBeginMainFrameToCommitDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Renderer", "BeginMainFrameToCommit");
        }

        void AddCommitToReadyToActivateDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Renderer", "CommitToReadyToActivate");
        }

        void AddPrepareTilesDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Renderer", "PrepareTiles");
        }

        void AddActivateDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Renderer", "Activate");
        }

        void AddDrawDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Renderer", "Draw");
            DeprecatedDrawDurationUMA(duration, estimate);
        }
    };

    class BrowserUMAReporter : public CompositorTimingHistory::UMAReporter {
    public:
        ~BrowserUMAReporter() override { }

        void AddBeginMainFrameToCommitDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Browser", "BeginMainFrameToCommit");
        }

        void AddCommitToReadyToActivateDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Browser", "CommitToReadyToActivate");
        }

        void AddPrepareTilesDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Browser", "PrepareTiles");
        }

        void AddActivateDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Browser", "Activate");
        }

        void AddDrawDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override
        {
            REPORT_COMPOSITOR_TIMING_HISTORY_UMA("Browser", "Draw");
            DeprecatedDrawDurationUMA(duration, estimate);
        }
    };

    class NullUMAReporter : public CompositorTimingHistory::UMAReporter {
    public:
        ~NullUMAReporter() override { }
        void AddPrepareTilesDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override { }
        void AddBeginMainFrameToCommitDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override { }
        void AddCommitToReadyToActivateDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override { }
        void AddActivateDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override { }
        void AddDrawDuration(base::TimeDelta duration,
            base::TimeDelta estimate,
            bool affects_estimate) override { }
    };

} // namespace

CompositorTimingHistory::CompositorTimingHistory(
    UMACategory uma_category,
    RenderingStatsInstrumentation* rendering_stats_instrumentation)
    : enabled_(false)
    , begin_main_frame_to_commit_duration_history_(kDurationHistorySize)
    , commit_to_ready_to_activate_duration_history_(kDurationHistorySize)
    , prepare_tiles_duration_history_(kDurationHistorySize)
    , activate_duration_history_(kDurationHistorySize)
    , draw_duration_history_(kDurationHistorySize)
    , uma_reporter_(CreateUMAReporter(uma_category))
    , rendering_stats_instrumentation_(rendering_stats_instrumentation)
{
}

CompositorTimingHistory::~CompositorTimingHistory()
{
}

scoped_ptr<CompositorTimingHistory::UMAReporter>
CompositorTimingHistory::CreateUMAReporter(UMACategory category)
{
    switch (category) {
    case RENDERER_UMA:
        return make_scoped_ptr(new RendererUMAReporter);
        break;
    case BROWSER_UMA:
        return make_scoped_ptr(new BrowserUMAReporter);
        break;
    case NULL_UMA:
        return make_scoped_ptr(new NullUMAReporter);
        break;
    }
    NOTREACHED();
    return make_scoped_ptr<CompositorTimingHistory::UMAReporter>(nullptr);
}

void CompositorTimingHistory::AsValueInto(
    base::trace_event::TracedValue* state) const
{
    state->SetDouble("begin_main_frame_to_commit_estimate_ms",
        BeginMainFrameToCommitDurationEstimate().InMillisecondsF());
    state->SetDouble("commit_to_ready_to_activate_estimate_ms",
        CommitToReadyToActivateDurationEstimate().InMillisecondsF());
    state->SetDouble("prepare_tiles_estimate_ms",
        PrepareTilesDurationEstimate().InMillisecondsF());
    state->SetDouble("activate_estimate_ms",
        ActivateDurationEstimate().InMillisecondsF());
    state->SetDouble("draw_estimate_ms",
        DrawDurationEstimate().InMillisecondsF());
}

base::TimeTicks CompositorTimingHistory::Now() const
{
    return base::TimeTicks::Now();
}

void CompositorTimingHistory::SetRecordingEnabled(bool enabled)
{
    enabled_ = enabled;
}

base::TimeDelta
CompositorTimingHistory::BeginMainFrameToCommitDurationEstimate() const
{
    return begin_main_frame_to_commit_duration_history_.Percentile(
        kBeginMainFrameToCommitEstimationPercentile);
}

base::TimeDelta
CompositorTimingHistory::CommitToReadyToActivateDurationEstimate() const
{
    return commit_to_ready_to_activate_duration_history_.Percentile(
        kCommitToReadyToActivateEstimationPercentile);
}

base::TimeDelta CompositorTimingHistory::PrepareTilesDurationEstimate() const
{
    return prepare_tiles_duration_history_.Percentile(
        kPrepareTilesEstimationPercentile);
}

base::TimeDelta CompositorTimingHistory::ActivateDurationEstimate() const
{
    return activate_duration_history_.Percentile(kActivateEstimationPercentile);
}

base::TimeDelta CompositorTimingHistory::DrawDurationEstimate() const
{
    return draw_duration_history_.Percentile(kDrawEstimationPercentile);
}

void CompositorTimingHistory::WillBeginMainFrame()
{
    DCHECK_EQ(base::TimeTicks(), begin_main_frame_sent_time_);
    begin_main_frame_sent_time_ = Now();
}

void CompositorTimingHistory::BeginMainFrameAborted()
{
    DidCommit();
}

void CompositorTimingHistory::DidCommit()
{
    DCHECK_NE(base::TimeTicks(), begin_main_frame_sent_time_);

    commit_time_ = Now();

    base::TimeDelta begin_main_frame_to_commit_duration = commit_time_ - begin_main_frame_sent_time_;

    // Before adding the new data point to the timing history, see what we would
    // have predicted for this frame. This allows us to keep track of the accuracy
    // of our predictions.
    base::TimeDelta begin_main_frame_to_commit_estimate = BeginMainFrameToCommitDurationEstimate();
    uma_reporter_->AddBeginMainFrameToCommitDuration(
        begin_main_frame_to_commit_duration, begin_main_frame_to_commit_estimate,
        enabled_);
    rendering_stats_instrumentation_->AddBeginMainFrameToCommitDuration(
        begin_main_frame_to_commit_duration, begin_main_frame_to_commit_estimate);

    if (enabled_) {
        begin_main_frame_to_commit_duration_history_.InsertSample(
            begin_main_frame_to_commit_duration);
    }

    begin_main_frame_sent_time_ = base::TimeTicks();
}

void CompositorTimingHistory::WillPrepareTiles()
{
    DCHECK_EQ(base::TimeTicks(), start_prepare_tiles_time_);
    start_prepare_tiles_time_ = Now();
}

void CompositorTimingHistory::DidPrepareTiles()
{
    DCHECK_NE(base::TimeTicks(), start_prepare_tiles_time_);

    base::TimeDelta prepare_tiles_duration = Now() - start_prepare_tiles_time_;
    uma_reporter_->AddPrepareTilesDuration(
        prepare_tiles_duration, PrepareTilesDurationEstimate(), enabled_);
    if (enabled_)
        prepare_tiles_duration_history_.InsertSample(prepare_tiles_duration);

    start_prepare_tiles_time_ = base::TimeTicks();
}

void CompositorTimingHistory::ReadyToActivate()
{
    // We only care about the first ready to activate signal
    // after a commit.
    if (commit_time_ == base::TimeTicks())
        return;

    base::TimeDelta time_since_commit = Now() - commit_time_;

    // Before adding the new data point to the timing history, see what we would
    // have predicted for this frame. This allows us to keep track of the accuracy
    // of our predictions.

    base::TimeDelta commit_to_ready_to_activate_estimate = CommitToReadyToActivateDurationEstimate();
    uma_reporter_->AddCommitToReadyToActivateDuration(
        time_since_commit, commit_to_ready_to_activate_estimate, enabled_);
    rendering_stats_instrumentation_->AddCommitToActivateDuration(
        time_since_commit, commit_to_ready_to_activate_estimate);

    if (enabled_) {
        commit_to_ready_to_activate_duration_history_.InsertSample(
            time_since_commit);
    }

    commit_time_ = base::TimeTicks();
}

void CompositorTimingHistory::WillActivate()
{
    DCHECK_EQ(base::TimeTicks(), start_activate_time_);
    start_activate_time_ = Now();
}

void CompositorTimingHistory::DidActivate()
{
    DCHECK_NE(base::TimeTicks(), start_activate_time_);
    base::TimeDelta activate_duration = Now() - start_activate_time_;

    uma_reporter_->AddActivateDuration(activate_duration,
        ActivateDurationEstimate(), enabled_);
    if (enabled_)
        activate_duration_history_.InsertSample(activate_duration);

    start_activate_time_ = base::TimeTicks();
}

void CompositorTimingHistory::WillDraw()
{
    DCHECK_EQ(base::TimeTicks(), start_draw_time_);
    start_draw_time_ = Now();
}

void CompositorTimingHistory::DidDraw()
{
    DCHECK_NE(base::TimeTicks(), start_draw_time_);
    base::TimeDelta draw_duration = Now() - start_draw_time_;

    // Before adding the new data point to the timing history, see what we would
    // have predicted for this frame. This allows us to keep track of the accuracy
    // of our predictions.
    base::TimeDelta draw_estimate = DrawDurationEstimate();
    rendering_stats_instrumentation_->AddDrawDuration(draw_duration,
        draw_estimate);

    uma_reporter_->AddDrawDuration(draw_duration, draw_estimate, enabled_);

    if (enabled_) {
        draw_duration_history_.InsertSample(draw_duration);
    }

    start_draw_time_ = base::TimeTicks();
}

} // namespace cc
