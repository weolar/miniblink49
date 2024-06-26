// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/scheduler/compositor_timing_history.h"

#include "cc/debug/rendering_stats_instrumentation.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class CompositorTimingHistoryTest;

    class TestCompositorTimingHistory : public CompositorTimingHistory {
    public:
        TestCompositorTimingHistory(CompositorTimingHistoryTest* test,
            RenderingStatsInstrumentation* rendering_stats)
            : CompositorTimingHistory(NULL_UMA, rendering_stats)
            , test_(test)
        {
        }

    protected:
        base::TimeTicks Now() const override;

        CompositorTimingHistoryTest* test_;

    private:
        DISALLOW_COPY_AND_ASSIGN(TestCompositorTimingHistory);
    };

    class CompositorTimingHistoryTest : public testing::Test {
    public:
        CompositorTimingHistoryTest()
            : rendering_stats_(RenderingStatsInstrumentation::Create())
            , timing_history_(this, rendering_stats_.get())
        {
            AdvanceNowBy(base::TimeDelta::FromMilliseconds(1));
            timing_history_.SetRecordingEnabled(true);
        }

        void AdvanceNowBy(base::TimeDelta delta) { now_ += delta; }

        base::TimeTicks Now() { return now_; }

    protected:
        scoped_ptr<RenderingStatsInstrumentation> rendering_stats_;
        TestCompositorTimingHistory timing_history_;
        base::TimeTicks now_;
    };

    base::TimeTicks TestCompositorTimingHistory::Now() const
    {
        return test_->Now();
    }

    TEST_F(CompositorTimingHistoryTest, AllSequentialCommit)
    {
        base::TimeDelta one_second = base::TimeDelta::FromSeconds(1);

        base::TimeDelta begin_main_frame_to_commit_duration = base::TimeDelta::FromMilliseconds(1);
        base::TimeDelta prepare_tiles_duration = base::TimeDelta::FromMilliseconds(2);
        base::TimeDelta prepare_tiles_end_to_ready_to_activate_duration = base::TimeDelta::FromMilliseconds(1);
        base::TimeDelta commit_to_ready_to_activate_duration = base::TimeDelta::FromMilliseconds(3);
        base::TimeDelta activate_duration = base::TimeDelta::FromMilliseconds(4);
        base::TimeDelta draw_duration = base::TimeDelta::FromMilliseconds(5);

        timing_history_.WillBeginMainFrame();
        AdvanceNowBy(begin_main_frame_to_commit_duration);
        // timing_history_.BeginMainFrameAborted();
        timing_history_.DidCommit();
        timing_history_.WillPrepareTiles();
        AdvanceNowBy(prepare_tiles_duration);
        timing_history_.DidPrepareTiles();
        AdvanceNowBy(prepare_tiles_end_to_ready_to_activate_duration);
        timing_history_.ReadyToActivate();
        // Do not count idle time between notification and actual activation.
        AdvanceNowBy(one_second);
        timing_history_.WillActivate();
        AdvanceNowBy(activate_duration);
        timing_history_.DidActivate();
        // Do not count idle time between activate and draw.
        AdvanceNowBy(one_second);
        timing_history_.WillDraw();
        AdvanceNowBy(draw_duration);
        timing_history_.DidDraw();

        EXPECT_EQ(begin_main_frame_to_commit_duration,
            timing_history_.BeginMainFrameToCommitDurationEstimate());
        EXPECT_EQ(commit_to_ready_to_activate_duration,
            timing_history_.CommitToReadyToActivateDurationEstimate());
        EXPECT_EQ(prepare_tiles_duration,
            timing_history_.PrepareTilesDurationEstimate());
        EXPECT_EQ(activate_duration, timing_history_.ActivateDurationEstimate());
        EXPECT_EQ(draw_duration, timing_history_.DrawDurationEstimate());
    }

    TEST_F(CompositorTimingHistoryTest, AllSequentialBeginMainFrameAborted)
    {
        base::TimeDelta one_second = base::TimeDelta::FromSeconds(1);

        base::TimeDelta begin_main_frame_to_commit_duration = base::TimeDelta::FromMilliseconds(1);
        base::TimeDelta prepare_tiles_duration = base::TimeDelta::FromMilliseconds(2);
        base::TimeDelta prepare_tiles_end_to_ready_to_activate_duration = base::TimeDelta::FromMilliseconds(1);
        base::TimeDelta commit_to_ready_to_activate_duration = base::TimeDelta::FromMilliseconds(3);
        base::TimeDelta activate_duration = base::TimeDelta::FromMilliseconds(4);
        base::TimeDelta draw_duration = base::TimeDelta::FromMilliseconds(5);

        timing_history_.WillBeginMainFrame();
        AdvanceNowBy(begin_main_frame_to_commit_duration);
        // BeginMainFrameAborted counts as a commit complete.
        timing_history_.BeginMainFrameAborted();
        timing_history_.WillPrepareTiles();
        AdvanceNowBy(prepare_tiles_duration);
        timing_history_.DidPrepareTiles();
        AdvanceNowBy(prepare_tiles_end_to_ready_to_activate_duration);
        timing_history_.ReadyToActivate();
        // Do not count idle time between notification and actual activation.
        AdvanceNowBy(one_second);
        timing_history_.WillActivate();
        AdvanceNowBy(activate_duration);
        timing_history_.DidActivate();
        // Do not count idle time between activate and draw.
        AdvanceNowBy(one_second);
        timing_history_.WillDraw();
        AdvanceNowBy(draw_duration);
        timing_history_.DidDraw();

        EXPECT_EQ(begin_main_frame_to_commit_duration,
            timing_history_.BeginMainFrameToCommitDurationEstimate());
        EXPECT_EQ(commit_to_ready_to_activate_duration,
            timing_history_.CommitToReadyToActivateDurationEstimate());
        EXPECT_EQ(prepare_tiles_duration,
            timing_history_.PrepareTilesDurationEstimate());
        EXPECT_EQ(activate_duration, timing_history_.ActivateDurationEstimate());
        EXPECT_EQ(draw_duration, timing_history_.DrawDurationEstimate());
    }

} // namespace
} // namespace cc
