// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/display_scheduler.h"

#include "base/logging.h"
#include "base/test/null_task_runner.h"
#include "base/test/simple_test_tick_clock.h"
#include "base/trace_event/trace_event.h"
#include "cc/output/begin_frame_args.h"
#include "cc/surfaces/display.h"
#include "cc/test/scheduler_test_common.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    const int kMaxPendingSwaps = 1;

    class FakeDisplaySchedulerClient : public DisplaySchedulerClient {
    public:
        FakeDisplaySchedulerClient()
            : draw_and_swap_count_(0)
        {
        }

        ~FakeDisplaySchedulerClient() override { }

        bool DrawAndSwap() override
        {
            draw_and_swap_count_++;
            return true;
        }

        void Reset() { draw_and_swap_count_ = 0; }

        int draw_and_swap_count() const { return draw_and_swap_count_; }

    protected:
        int draw_and_swap_count_;
    };

    class TestDisplayScheduler : public DisplayScheduler {
    public:
        TestDisplayScheduler(DisplaySchedulerClient* client,
            BeginFrameSource* begin_frame_source,
            base::SingleThreadTaskRunner* task_runner,
            int max_pending_swaps)
            : DisplayScheduler(client,
                begin_frame_source,
                task_runner,
                max_pending_swaps)
            , scheduler_begin_frame_deadline_count_(0)
        {
        }

        base::TimeTicks DesiredBeginFrameDeadlineTimeForTest()
        {
            return DesiredBeginFrameDeadlineTime();
        }

        void BeginFrameDeadlineForTest() { OnBeginFrameDeadline(); }

        void ScheduleBeginFrameDeadline() override
        {
            scheduler_begin_frame_deadline_count_++;
            DisplayScheduler::ScheduleBeginFrameDeadline();
        }

        int scheduler_begin_frame_deadline_count()
        {
            return scheduler_begin_frame_deadline_count_;
        }

    protected:
        int scheduler_begin_frame_deadline_count_;
    };

    class DisplaySchedulerTest : public testing::Test {
    public:
        DisplaySchedulerTest()
            : now_src_(new base::SimpleTestTickClock())
            , task_runner_(new base::NullTaskRunner)
            , client_(new FakeDisplaySchedulerClient)
            , scheduler_(new TestDisplayScheduler(client_.get(),
                  &fake_begin_frame_source_,
                  task_runner_.get(),
                  kMaxPendingSwaps))
        {
            now_src_->Advance(base::TimeDelta::FromMicroseconds(10000));
        }

        ~DisplaySchedulerTest() override { }

        void SetUp() override { scheduler_->SetRootSurfaceResourcesLocked(false); }

        void BeginFrameForTest()
        {
            base::TimeTicks frame_time = now_src_->NowTicks();
            base::TimeDelta interval = BeginFrameArgs::DefaultInterval();
            base::TimeTicks deadline = frame_time + interval;
            fake_begin_frame_source_.TestOnBeginFrame(
                BeginFrameArgs::Create(BEGINFRAME_FROM_HERE, frame_time, deadline,
                    interval, BeginFrameArgs::NORMAL));
        }

    protected:
        base::SimpleTestTickClock& now_src() { return *now_src_; }
        FakeDisplaySchedulerClient& client() { return *client_; }
        DisplayScheduler& scheduler() { return *scheduler_; }

        FakeBeginFrameSource fake_begin_frame_source_;

        scoped_ptr<base::SimpleTestTickClock> now_src_;
        scoped_refptr<base::NullTaskRunner> task_runner_;
        scoped_ptr<FakeDisplaySchedulerClient> client_;
        scoped_ptr<TestDisplayScheduler> scheduler_;
    };

    TEST_F(DisplaySchedulerTest, ResizeHasLateDeadlineUntilNewRootSurface)
    {
        SurfaceId root_surface_id1(1);
        SurfaceId root_surface_id2(2);
        SurfaceId sid1(3);
        base::TimeTicks late_deadline;

        // Go trough an initial BeginFrame cycle with the root surface.
        BeginFrameForTest();
        scheduler_->SetNewRootSurface(root_surface_id1);
        scheduler_->BeginFrameDeadlineForTest();

        // Resize on the next begin frame cycle should cause the deadline to wait
        // for a new root surface.
        late_deadline = now_src().NowTicks() + BeginFrameArgs::DefaultInterval();
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        EXPECT_GT(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->DisplayResized();
        EXPECT_EQ(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->SetNewRootSurface(root_surface_id2);
        EXPECT_GE(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->BeginFrameDeadlineForTest();

        // Verify deadline goes back to normal after resize.
        late_deadline = now_src().NowTicks() + BeginFrameArgs::DefaultInterval();
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        EXPECT_GT(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->SurfaceDamaged(root_surface_id2);
        EXPECT_GE(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->BeginFrameDeadlineForTest();
    }

    TEST_F(DisplaySchedulerTest, ResizeHasLateDeadlineUntilDamagedSurface)
    {
        SurfaceId root_surface_id(1);
        SurfaceId sid1(2);
        base::TimeTicks late_deadline;

        // Go trough an initial BeginFrame cycle with the root surface.
        BeginFrameForTest();
        scheduler_->SetNewRootSurface(root_surface_id);
        scheduler_->BeginFrameDeadlineForTest();

        // Resize on the next begin frame cycle should cause the deadline to wait
        // for a new root surface.
        late_deadline = now_src().NowTicks() + BeginFrameArgs::DefaultInterval();
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        EXPECT_GT(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->DisplayResized();
        EXPECT_EQ(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->SurfaceDamaged(root_surface_id);
        EXPECT_GE(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->BeginFrameDeadlineForTest();

        // Verify deadline goes back to normal after resize.
        late_deadline = now_src().NowTicks() + BeginFrameArgs::DefaultInterval();
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        EXPECT_GT(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->SurfaceDamaged(root_surface_id);
        EXPECT_GE(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->BeginFrameDeadlineForTest();
    }

    TEST_F(DisplaySchedulerTest, SurfaceDamaged)
    {
        SurfaceId root_surface_id(0);
        SurfaceId sid1(1);
        SurfaceId sid2(2);

        // Set the root surface
        scheduler_->SetNewRootSurface(root_surface_id);

        // Get scheduler to detect surface 1 as active by drawing
        // two frames in a row with damage from surface 1.
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->BeginFrameDeadlineForTest();
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->BeginFrameDeadlineForTest();

        // Damage only from surface 2 (inactive) does not trigger deadline early.
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid2);
        EXPECT_LT(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());

        // Damage from surface 1 triggers deadline early.
        scheduler_->SurfaceDamaged(sid1);
        EXPECT_GE(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->BeginFrameDeadlineForTest();

        // Make both surface 1 and 2 active.
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid2);
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->BeginFrameDeadlineForTest();

        // Deadline doesn't trigger early until surface 1 and 2 are both damaged.
        BeginFrameForTest();
        EXPECT_LT(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->SurfaceDamaged(sid1);
        EXPECT_LT(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->SurfaceDamaged(sid2);
        EXPECT_GE(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->BeginFrameDeadlineForTest();

        // Make the system idle
        BeginFrameForTest();
        scheduler_->BeginFrameDeadlineForTest();
        BeginFrameForTest();
        scheduler_->BeginFrameDeadlineForTest();

        // Deadline should trigger early if child surfaces are idle and
        // we get damage on the root surface.
        BeginFrameForTest();
        EXPECT_LT(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->SurfaceDamaged(root_surface_id);
        EXPECT_GE(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->BeginFrameDeadlineForTest();
    }

    TEST_F(DisplaySchedulerTest, OutputSurfaceLost)
    {
        SurfaceId sid1(1);

        // DrawAndSwap normally.
        BeginFrameForTest();
        EXPECT_LT(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        EXPECT_EQ(0, client_->draw_and_swap_count());
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(1, client_->draw_and_swap_count());

        // Deadline triggers immediately on OutputSurfaceLost.
        BeginFrameForTest();
        EXPECT_LT(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->OutputSurfaceLost();
        EXPECT_GE(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());

        // Deadline does not DrawAndSwap after OutputSurfaceLost.
        EXPECT_EQ(1, client_->draw_and_swap_count());
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(1, client_->draw_and_swap_count());
    }

    TEST_F(DisplaySchedulerTest, ResizeCausesSwap)
    {
        SurfaceId sid1(1);

        // DrawAndSwap normally.
        BeginFrameForTest();
        EXPECT_LT(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        EXPECT_EQ(0, client_->draw_and_swap_count());
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(1, client_->draw_and_swap_count());

        scheduler_->DisplayResized();
        BeginFrameForTest();
        // DisplayResized should trigger a swap to happen.
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(2, client_->draw_and_swap_count());
    }

    TEST_F(DisplaySchedulerTest, RootSurfaceResourcesLocked)
    {
        SurfaceId sid1(1);
        base::TimeTicks late_deadline;

        // DrawAndSwap normally.
        BeginFrameForTest();
        EXPECT_LT(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        EXPECT_EQ(0, client_->draw_and_swap_count());
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(1, client_->draw_and_swap_count());

        // Deadline triggers late while root resources are locked.
        late_deadline = now_src().NowTicks() + BeginFrameArgs::DefaultInterval();
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        EXPECT_GT(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->SetRootSurfaceResourcesLocked(true);
        EXPECT_EQ(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());

        // Deadline does not DrawAndSwap while root resources are locked.
        EXPECT_EQ(1, client_->draw_and_swap_count());
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(1, client_->draw_and_swap_count());

        //  Deadline triggers normally when root resources are unlocked.
        late_deadline = now_src().NowTicks() + BeginFrameArgs::DefaultInterval();
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        EXPECT_EQ(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        scheduler_->SetRootSurfaceResourcesLocked(false);
        EXPECT_EQ(base::TimeTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());

        EXPECT_EQ(1, client_->draw_and_swap_count());
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(2, client_->draw_and_swap_count());
    }

    TEST_F(DisplaySchedulerTest, DidSwapBuffers)
    {
        SurfaceId sid1(1);
        SurfaceId sid2(2);

        // Get scheduler to detect surface 1 and 2 as active.
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->SurfaceDamaged(sid2);
        scheduler_->BeginFrameDeadlineForTest();
        BeginFrameForTest();
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->SurfaceDamaged(sid2);
        scheduler_->BeginFrameDeadlineForTest();

        // DrawAndSwap normally.
        BeginFrameForTest();
        EXPECT_LT(now_src().NowTicks(),
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        EXPECT_EQ(2, client_->draw_and_swap_count());
        scheduler_->SurfaceDamaged(sid1);
        scheduler_->SurfaceDamaged(sid2);
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(3, client_->draw_and_swap_count());
        scheduler_->DidSwapBuffers();

        // Deadline triggers late when swap throttled.
        base::TimeTicks late_deadline = now_src().NowTicks() + BeginFrameArgs::DefaultInterval();
        BeginFrameForTest();
        // Damage surface 1, but not surface 2 so we avoid triggering deadline
        // early because all surfaces are ready.
        scheduler_->SurfaceDamaged(sid1);
        EXPECT_EQ(late_deadline, scheduler_->DesiredBeginFrameDeadlineTimeForTest());

        // Don't draw and swap in deadline while swap throttled.
        EXPECT_EQ(3, client_->draw_and_swap_count());
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(3, client_->draw_and_swap_count());

        // Deadline triggers normally once not swap throttled.
        // Damage from previous BeginFrame should cary over, so don't damage again.
        base::TimeTicks expected_deadline = fake_begin_frame_source_.TestLastUsedBeginFrameArgs().deadline - BeginFrameArgs::DefaultEstimatedParentDrawTime();
        scheduler_->DidSwapBuffersComplete();
        BeginFrameForTest();
        EXPECT_EQ(expected_deadline,
            scheduler_->DesiredBeginFrameDeadlineTimeForTest());
        // Still waiting for surface 2. Once it updates, deadline should trigger
        // immediately again.
        scheduler_->SurfaceDamaged(sid2);
        EXPECT_EQ(scheduler_->DesiredBeginFrameDeadlineTimeForTest(),
            base::TimeTicks());
        // Draw and swap now that we aren't throttled.
        EXPECT_EQ(3, client_->draw_and_swap_count());
        scheduler_->BeginFrameDeadlineForTest();
        EXPECT_EQ(4, client_->draw_and_swap_count());
    }

    // This test verfies that we try to reschedule the deadline
    // after any event that may change what deadline we want.
    TEST_F(DisplaySchedulerTest, ScheduleBeginFrameDeadline)
    {
        SurfaceId root_surface_id(1);
        SurfaceId sid1(2);
        int count = 1;
        EXPECT_EQ(count++, scheduler_->scheduler_begin_frame_deadline_count());

        BeginFrameForTest();
        EXPECT_EQ(count++, scheduler_->scheduler_begin_frame_deadline_count());

        scheduler_->BeginFrameDeadlineForTest();
        scheduler_->DidSwapBuffers();
        BeginFrameForTest();
        EXPECT_EQ(count++, scheduler_->scheduler_begin_frame_deadline_count());

        scheduler_->DidSwapBuffersComplete();
        EXPECT_EQ(count++, scheduler_->scheduler_begin_frame_deadline_count());

        scheduler_->DisplayResized();
        EXPECT_EQ(count++, scheduler_->scheduler_begin_frame_deadline_count());

        scheduler_->SetNewRootSurface(root_surface_id);
        EXPECT_EQ(count++, scheduler_->scheduler_begin_frame_deadline_count());

        scheduler_->SurfaceDamaged(sid1);
        EXPECT_EQ(count++, scheduler_->scheduler_begin_frame_deadline_count());

        scheduler_->SetRootSurfaceResourcesLocked(true);
        EXPECT_EQ(count++, scheduler_->scheduler_begin_frame_deadline_count());

        scheduler_->OutputSurfaceLost();
        EXPECT_EQ(count++, scheduler_->scheduler_begin_frame_deadline_count());
    }

} // namespace
} // namespace cc
