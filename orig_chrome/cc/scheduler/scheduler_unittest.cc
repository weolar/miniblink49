// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/scheduler/scheduler.h"

#include <string>
#include <vector>

#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/numerics/safe_conversions.h"
#include "base/run_loop.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "cc/test/begin_frame_args_test.h"
#include "cc/test/ordered_simple_task_runner.h"
#include "cc/test/scheduler_test_common.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define EXPECT_ACTION(action, client, action_index, expected_num_actions)        \
    do {                                                                         \
        EXPECT_EQ(expected_num_actions, client->num_actions_());                 \
        if (action_index >= 0) {                                                 \
            ASSERT_LT(action_index, client->num_actions_()) << scheduler_.get(); \
            EXPECT_STREQ(action, client->Action(action_index));                  \
        }                                                                        \
        for (int i = expected_num_actions; i < client->num_actions_(); ++i)      \
            ADD_FAILURE() << "Unexpected action: " << client->Action(i)          \
                          << " with state:\n"                                    \
                          << client->StateForAction(i);                          \
    } while (false)

#define EXPECT_NO_ACTION(client) EXPECT_ACTION("", client, -1, 0)

#define EXPECT_SINGLE_ACTION(action, client) \
    EXPECT_ACTION(action, client, 0, 1)

#define EXPECT_SCOPED(statements) \
    {                             \
        SCOPED_TRACE("");         \
        statements;               \
    }

namespace cc {
namespace {

    class FakeSchedulerClient : public SchedulerClient {
    public:
        FakeSchedulerClient()
            : automatic_swap_ack_(true)
            , scheduler_(nullptr)
        {
            Reset();
        }

        void Reset()
        {
            actions_.clear();
            states_.clear();
            draw_will_happen_ = true;
            swap_will_happen_if_draw_happens_ = true;
            num_draws_ = 0;
            begin_frame_args_sent_to_children_ = BeginFrameArgs();
        }

        void set_scheduler(TestScheduler* scheduler) { scheduler_ = scheduler; }

        bool needs_begin_frames()
        {
            return scheduler_->frame_source().NeedsBeginFrames();
        }
        int num_draws() const { return num_draws_; }
        int num_actions_() const { return static_cast<int>(actions_.size()); }
        const char* Action(int i) const { return actions_[i]; }
        std::string StateForAction(int i) const { return states_[i]->ToString(); }
        base::TimeTicks posted_begin_impl_frame_deadline() const
        {
            return posted_begin_impl_frame_deadline_;
        }

        int ActionIndex(const char* action) const
        {
            for (size_t i = 0; i < actions_.size(); i++)
                if (!strcmp(actions_[i], action))
                    return base::checked_cast<int>(i);
            return -1;
        }

        bool HasAction(const char* action) const
        {
            return ActionIndex(action) >= 0;
        }

        void SetDrawWillHappen(bool draw_will_happen)
        {
            draw_will_happen_ = draw_will_happen;
        }
        void SetSwapWillHappenIfDrawHappens(bool swap_will_happen_if_draw_happens)
        {
            swap_will_happen_if_draw_happens_ = swap_will_happen_if_draw_happens;
        }
        void SetAutomaticSwapAck(bool automatic_swap_ack)
        {
            automatic_swap_ack_ = automatic_swap_ack;
        }
        // SchedulerClient implementation.
        void WillBeginImplFrame(const BeginFrameArgs& args) override
        {
            PushAction("WillBeginImplFrame");
        }
        void DidFinishImplFrame() override { }

        void ScheduledActionSendBeginMainFrame() override
        {
            PushAction("ScheduledActionSendBeginMainFrame");
        }
        void ScheduledActionAnimate() override
        {
            PushAction("ScheduledActionAnimate");
        }
        DrawResult ScheduledActionDrawAndSwapIfPossible() override
        {
            PushAction("ScheduledActionDrawAndSwapIfPossible");
            num_draws_++;
            DrawResult result = draw_will_happen_ ? DRAW_SUCCESS : DRAW_ABORTED_CHECKERBOARD_ANIMATIONS;
            bool swap_will_happen = draw_will_happen_ && swap_will_happen_if_draw_happens_;
            if (swap_will_happen) {
                scheduler_->DidSwapBuffers();

                if (automatic_swap_ack_)
                    scheduler_->DidSwapBuffersComplete();
            }
            return result;
        }
        DrawResult ScheduledActionDrawAndSwapForced() override
        {
            PushAction("ScheduledActionDrawAndSwapForced");
            return DRAW_SUCCESS;
        }
        void ScheduledActionCommit() override
        {
            PushAction("ScheduledActionCommit");
            scheduler_->DidCommit();
        }
        void ScheduledActionActivateSyncTree() override
        {
            PushAction("ScheduledActionActivateSyncTree");
        }
        void ScheduledActionBeginOutputSurfaceCreation() override
        {
            PushAction("ScheduledActionBeginOutputSurfaceCreation");
        }
        void ScheduledActionPrepareTiles() override
        {
            PushAction("ScheduledActionPrepareTiles");
            scheduler_->WillPrepareTiles();
            scheduler_->DidPrepareTiles();
        }
        void ScheduledActionInvalidateOutputSurface() override
        {
            actions_.push_back("ScheduledActionInvalidateOutputSurface");
            states_.push_back(scheduler_->AsValue());
        }

        void SendBeginFramesToChildren(const BeginFrameArgs& args) override
        {
            begin_frame_args_sent_to_children_ = args;
        }

        void SendBeginMainFrameNotExpectedSoon() override
        {
            PushAction("SendBeginMainFrameNotExpectedSoon");
        }

        base::Callback<bool(void)> ImplFrameDeadlinePending(bool state)
        {
            return base::Bind(&FakeSchedulerClient::ImplFrameDeadlinePendingCallback,
                base::Unretained(this),
                state);
        }

        bool begin_frame_is_sent_to_children() const
        {
            return begin_frame_args_sent_to_children_.IsValid();
        }

        const BeginFrameArgs& begin_frame_args_sent_to_children() const
        {
            return begin_frame_args_sent_to_children_;
        }

        void PushAction(const char* description)
        {
            actions_.push_back(description);
            states_.push_back(scheduler_->AsValue());
        }

    protected:
        bool ImplFrameDeadlinePendingCallback(bool state)
        {
            return scheduler_->BeginImplFrameDeadlinePending() == state;
        }

        bool draw_will_happen_;
        bool swap_will_happen_if_draw_happens_;
        bool automatic_swap_ack_;
        int num_draws_;
        BeginFrameArgs begin_frame_args_sent_to_children_;
        base::TimeTicks posted_begin_impl_frame_deadline_;
        std::vector<const char*> actions_;
        std::vector<scoped_refptr<base::trace_event::ConvertableToTraceFormat>>
            states_;
        TestScheduler* scheduler_;
    };

    class FakeExternalBeginFrameSource : public BeginFrameSourceBase {
    public:
        explicit FakeExternalBeginFrameSource(FakeSchedulerClient* client)
            : client_(client)
        {
        }
        ~FakeExternalBeginFrameSource() override { }

        void OnNeedsBeginFramesChange(bool needs_begin_frames) override
        {
            if (needs_begin_frames) {
                client_->PushAction("SetNeedsBeginFrames(true)");
            } else {
                client_->PushAction("SetNeedsBeginFrames(false)");
            }
        }

        void TestOnBeginFrame(const BeginFrameArgs& args)
        {
            return CallOnBeginFrame(args);
        }

    private:
        FakeSchedulerClient* client_;
    };

    class SchedulerTest : public testing::Test {
    public:
        SchedulerTest()
            : now_src_(new base::SimpleTestTickClock())
            , task_runner_(new OrderedSimpleTaskRunner(now_src_.get(), true))
            , fake_external_begin_frame_source_(nullptr)
        {
            now_src_->Advance(base::TimeDelta::FromMicroseconds(10000));
            // A bunch of tests require NowTicks()
            // to be > BeginFrameArgs::DefaultInterval()
            now_src_->Advance(base::TimeDelta::FromMilliseconds(100));
            // Fail if we need to run 100 tasks in a row.
            task_runner_->SetRunTaskLimit(100);
        }

        ~SchedulerTest() override { }

    protected:
        TestScheduler* CreateScheduler()
        {
            if (scheduler_settings_.use_external_begin_frame_source) {
                fake_external_begin_frame_source_.reset(
                    new FakeExternalBeginFrameSource(client_.get()));
            }

            scoped_ptr<FakeCompositorTimingHistory> fake_compositor_timing_history = FakeCompositorTimingHistory::Create();
            fake_compositor_timing_history_ = fake_compositor_timing_history.get();

            scheduler_ = TestScheduler::Create(
                now_src_.get(), client_.get(), scheduler_settings_, 0,
                task_runner_.get(), fake_external_begin_frame_source_.get(),
                fake_compositor_timing_history.Pass());
            DCHECK(scheduler_);
            client_->set_scheduler(scheduler_.get());

            // Use large estimates by default to avoid latency recovery
            // in most tests.
            base::TimeDelta slow_duration = base::TimeDelta::FromSeconds(1);
            fake_compositor_timing_history_->SetAllEstimatesTo(slow_duration);

            return scheduler_.get();
        }

        void CreateSchedulerAndInitSurface()
        {
            CreateScheduler();
            EXPECT_SCOPED(InitializeOutputSurfaceAndFirstCommit());
        }

        void SetUpScheduler(bool initSurface)
        {
            SetUpScheduler(make_scoped_ptr(new FakeSchedulerClient), initSurface);
        }

        void SetUpScheduler(scoped_ptr<FakeSchedulerClient> client,
            bool initSurface)
        {
            client_ = client.Pass();
            if (initSurface)
                CreateSchedulerAndInitSurface();
            else
                CreateScheduler();
        }

        OrderedSimpleTaskRunner& task_runner() { return *task_runner_; }
        base::SimpleTestTickClock* now_src() { return now_src_.get(); }

        // As this function contains EXPECT macros, to allow debugging it should be
        // called inside EXPECT_SCOPED like so;
        //   EXPECT_SCOPED(client.InitializeOutputSurfaceAndFirstCommit(scheduler));
        void InitializeOutputSurfaceAndFirstCommit()
        {
            TRACE_EVENT0("cc",
                "SchedulerUnitTest::InitializeOutputSurfaceAndFirstCommit");
            DCHECK(scheduler_);

            // Check the client doesn't have any actions queued when calling this
            // function.
            EXPECT_NO_ACTION(client_);
            EXPECT_FALSE(client_->needs_begin_frames());

            // Start the initial output surface creation.
            EXPECT_FALSE(scheduler_->CanStart());
            scheduler_->SetCanStart();
            scheduler_->SetVisible(true);
            scheduler_->SetCanDraw(true);
            EXPECT_SINGLE_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_);

            client_->Reset();

            // We don't see anything happening until the first impl frame.
            scheduler_->DidCreateAndInitializeOutputSurface();
            scheduler_->SetNeedsBeginMainFrame();
            EXPECT_TRUE(client_->needs_begin_frames());
            EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
            client_->Reset();

            {
                SCOPED_TRACE("Do first frame to commit after initialize.");
                AdvanceFrame();

                scheduler_->NotifyBeginMainFrameStarted();
                scheduler_->NotifyReadyToCommit();
                scheduler_->NotifyReadyToActivate();
                scheduler_->NotifyReadyToDraw();

                EXPECT_FALSE(scheduler_->CommitPending());

                if (scheduler_settings_.using_synchronous_renderer_compositor) {
                    scheduler_->SetNeedsRedraw();
                    scheduler_->OnDrawForOutputSurface();
                } else {
                    // Run the posted deadline task.
                    EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
                    task_runner_->RunTasksWhile(client_->ImplFrameDeadlinePending(true));
                }

                EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
            }

            client_->Reset();

            {
                SCOPED_TRACE(
                    "Run second frame so Scheduler calls SetNeedsBeginFrame(false).");
                AdvanceFrame();

                if (!scheduler_settings_.using_synchronous_renderer_compositor) {
                    // Run the posted deadline task.
                    EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
                    task_runner_->RunTasksWhile(client_->ImplFrameDeadlinePending(true));
                }

                EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
            }

            EXPECT_FALSE(client_->needs_begin_frames());
            client_->Reset();
        }

        // As this function contains EXPECT macros, to allow debugging it should be
        // called inside EXPECT_SCOPED like so;
        //   EXPECT_SCOPED(client.AdvanceFrame());
        void AdvanceFrame()
        {
            TRACE_EVENT0(TRACE_DISABLED_BY_DEFAULT("cc.debug.scheduler.frames"),
                "FakeSchedulerClient::AdvanceFrame");
            // Consume any previous deadline first, if no deadline is currently
            // pending, ImplFrameDeadlinePending will return false straight away and we
            // will run no tasks.
            task_runner_->RunTasksWhile(client_->ImplFrameDeadlinePending(true));
            EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

            // Send the next BeginFrame message if using an external source, otherwise
            // it will be already in the task queue.
            if (scheduler_->settings().use_external_begin_frame_source && scheduler_->FrameProductionThrottled()) {
                EXPECT_TRUE(client_->needs_begin_frames());
                SendNextBeginFrame();
            }

            if (!scheduler_->settings().using_synchronous_renderer_compositor) {
                // Then run tasks until new deadline is scheduled.
                EXPECT_TRUE(task_runner_->RunTasksWhile(
                    client_->ImplFrameDeadlinePending(false)));
                EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
            }
        }

        BeginFrameArgs SendNextBeginFrame()
        {
            DCHECK(scheduler_->settings().use_external_begin_frame_source);
            // Creep the time forward so that any BeginFrameArgs is not equal to the
            // last one otherwise we violate the BeginFrameSource contract.
            now_src_->Advance(BeginFrameArgs::DefaultInterval());
            BeginFrameArgs args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, now_src());
            fake_external_begin_frame_source_->TestOnBeginFrame(args);
            return args;
        }

        FakeExternalBeginFrameSource* fake_external_begin_frame_source() const
        {
            return fake_external_begin_frame_source_.get();
        }

        void CheckMainFrameSkippedAfterLateCommit(bool expect_send_begin_main_frame);
        void ImplFrameSkippedAfterLateSwapAck(bool swap_ack_before_deadline);
        void ImplFrameIsNotSkippedAfterLateSwapAck();
        void BeginFramesNotFromClient(bool use_external_begin_frame_source,
            bool throttle_frame_production);
        void BeginFramesNotFromClient_SwapThrottled(
            bool use_external_begin_frame_source,
            bool throttle_frame_production);

        scoped_ptr<base::SimpleTestTickClock> now_src_;
        scoped_refptr<OrderedSimpleTaskRunner> task_runner_;
        scoped_ptr<FakeExternalBeginFrameSource> fake_external_begin_frame_source_;
        SchedulerSettings scheduler_settings_;
        scoped_ptr<FakeSchedulerClient> client_;
        scoped_ptr<TestScheduler> scheduler_;
        FakeCompositorTimingHistory* fake_compositor_timing_history_;
    };

    TEST_F(SchedulerTest, InitializeOutputSurfaceDoesNotBeginImplFrame)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(false);
        scheduler_->SetCanStart();
        scheduler_->SetVisible(true);
        scheduler_->SetCanDraw(true);

        EXPECT_SINGLE_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_);
        client_->Reset();
        scheduler_->DidCreateAndInitializeOutputSurface();
        EXPECT_NO_ACTION(client_);
    }

    TEST_F(SchedulerTest, SendBeginFramesToChildren)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        EXPECT_FALSE(client_->begin_frame_is_sent_to_children());
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        EXPECT_TRUE(client_->needs_begin_frames());

        scheduler_->SetChildrenNeedBeginFrames(true);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(client_->begin_frame_is_sent_to_children());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(client_->needs_begin_frames());
    }

    TEST_F(SchedulerTest, SendBeginFramesToChildrenWithoutCommit)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        EXPECT_FALSE(client_->needs_begin_frames());
        scheduler_->SetChildrenNeedBeginFrames(true);
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        EXPECT_TRUE(client_->needs_begin_frames());

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(client_->begin_frame_is_sent_to_children());
    }

    TEST_F(SchedulerTest, SendBeginFramesToChildrenDeadlineNotAdjusted)
    {
        // Set up client with specified estimates.
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        fake_compositor_timing_history_->SetBeginMainFrameToCommitDurationEstimate(
            base::TimeDelta::FromMilliseconds(2));
        fake_compositor_timing_history_->SetCommitToReadyToActivateDurationEstimate(
            base::TimeDelta::FromMilliseconds(4));
        fake_compositor_timing_history_->SetDrawDurationEstimate(
            base::TimeDelta::FromMilliseconds(1));

        EXPECT_FALSE(client_->needs_begin_frames());
        scheduler_->SetChildrenNeedBeginFrames(true);
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        EXPECT_TRUE(client_->needs_begin_frames());

        client_->Reset();

        BeginFrameArgs frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, now_src());
        fake_external_begin_frame_source()->TestOnBeginFrame(frame_args);

        EXPECT_TRUE(client_->begin_frame_is_sent_to_children());
        EXPECT_EQ(client_->begin_frame_args_sent_to_children().deadline,
            frame_args.deadline);
    }

    TEST_F(SchedulerTest, VideoNeedsBeginFrames)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetVideoNeedsBeginFrames(true);
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        EXPECT_TRUE(client_->needs_begin_frames());

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        // WillBeginImplFrame is responsible for sending BeginFrames to video.
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client_);

        client_->Reset();
        scheduler_->SetVideoNeedsBeginFrames(false);
        EXPECT_NO_ACTION(client_);

        client_->Reset();
        task_runner_->RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 0, 2);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 1, 2);
        EXPECT_FALSE(client_->needs_begin_frames());
    }

    TEST_F(SchedulerTest, RequestCommit)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // If we don't swap on the deadline, we wait for the next BeginFrame.
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_NO_ACTION(client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // NotifyReadyToCommit should trigger the commit.
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // NotifyReadyToActivate should trigger the activation.
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // BeginImplFrame should prepare the draw.
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // BeginImplFrame deadline should draw.
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // The following BeginImplFrame deadline should SetNeedsBeginFrame(false)
        // to avoid excessive toggles.
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 0, 2);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 1, 2);
        client_->Reset();
    }

    TEST_F(SchedulerTest, RequestCommitAfterSetDeferCommit)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetDeferCommits(true);

        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_NO_ACTION(client_);

        client_->Reset();
        task_runner().RunPendingTasks();
        // There are no pending tasks or actions.
        EXPECT_NO_ACTION(client_);
        EXPECT_FALSE(client_->needs_begin_frames());

        client_->Reset();
        scheduler_->SetDeferCommits(false);
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        // Start new BeginMainFrame after defer commit is off.
        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
    }

    TEST_F(SchedulerTest, DeferCommitWithRedraw)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetDeferCommits(true);

        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_NO_ACTION(client_);

        // The SetNeedsRedraw will override the SetDeferCommits(true), to allow a
        // begin frame to be needed.
        client_->Reset();
        scheduler_->SetNeedsRedraw();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        AdvanceFrame();
        // BeginMainFrame is not sent during the defer commit is on.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);

        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());

        client_->Reset();
        AdvanceFrame();
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client_);
    }

    TEST_F(SchedulerTest, RequestCommitAfterBeginMainFrameSent)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // Now SetNeedsBeginMainFrame again. Calling here means we need a second
        // commit.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_EQ(client_->num_actions_(), 0);
        client_->Reset();

        // Finish the first commit.
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Activate it.
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("ScheduledActionAnimate", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

        // Because we just swapped, the Scheduler should also request the next
        // BeginImplFrame from the OutputSurface.
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();
        // Since another commit is needed, the next BeginImplFrame should initiate
        // the second commit.
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Finishing the commit before the deadline should post a new deadline task
        // to trigger the deadline early.
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("ScheduledActionAnimate", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // On the next BeginImplFrame, verify we go back to a quiescent state and
        // no longer request BeginImplFrames.
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_FALSE(client_->needs_begin_frames());
        client_->Reset();
    }

    class SchedulerClientThatsetNeedsDrawInsideDraw : public FakeSchedulerClient {
    public:
        SchedulerClientThatsetNeedsDrawInsideDraw()
            : FakeSchedulerClient()
            , request_redraws_(false)
        {
        }

        void SetRequestRedrawsInsideDraw(bool enable) { request_redraws_ = enable; }

        DrawResult ScheduledActionDrawAndSwapIfPossible() override
        {
            // Only SetNeedsRedraw the first time this is called
            if (request_redraws_) {
                scheduler_->SetNeedsRedraw();
            }
            return FakeSchedulerClient::ScheduledActionDrawAndSwapIfPossible();
        }

        DrawResult ScheduledActionDrawAndSwapForced() override
        {
            NOTREACHED();
            return DRAW_SUCCESS;
        }

    private:
        bool request_redraws_;
    };

    // Tests for two different situations:
    // 1. the scheduler dropping SetNeedsRedraw requests that happen inside
    //    a ScheduledActionDrawAndSwap
    // 2. the scheduler drawing twice inside a single tick
    TEST_F(SchedulerTest, RequestRedrawInsideDraw)
    {
        SchedulerClientThatsetNeedsDrawInsideDraw* client = new SchedulerClientThatsetNeedsDrawInsideDraw;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(make_scoped_ptr(client).Pass(), true);
        client->SetRequestRedrawsInsideDraw(true);

        scheduler_->SetNeedsRedraw();
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());
        EXPECT_EQ(0, client->num_draws());

        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client->num_draws());
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());

        client->SetRequestRedrawsInsideDraw(false);

        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(2, client_->num_draws());
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());

        // We stop requesting BeginImplFrames after a BeginImplFrame where we don't
        // swap.
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(2, client->num_draws());
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(client->needs_begin_frames());
    }

    // Test that requesting redraw inside a failed draw doesn't lose the request.
    TEST_F(SchedulerTest, RequestRedrawInsideFailedDraw)
    {
        SchedulerClientThatsetNeedsDrawInsideDraw* client = new SchedulerClientThatsetNeedsDrawInsideDraw;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(make_scoped_ptr(client).Pass(), true);

        client->SetRequestRedrawsInsideDraw(true);
        client->SetDrawWillHappen(false);

        scheduler_->SetNeedsRedraw();
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());
        EXPECT_EQ(0, client->num_draws());

        // Fail the draw.
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client->num_draws());

        // We have a commit pending and the draw failed, and we didn't lose the redraw
        // request.
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());

        client->SetRequestRedrawsInsideDraw(false);

        // Fail the draw again.
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(2, client->num_draws());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());

        // Draw successfully.
        client->SetDrawWillHappen(true);
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(3, client->num_draws());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());
    }

    class SchedulerClientThatSetNeedsBeginMainFrameInsideDraw
        : public FakeSchedulerClient {
    public:
        SchedulerClientThatSetNeedsBeginMainFrameInsideDraw()
            : set_needs_commit_on_next_draw_(false)
        {
        }

        DrawResult ScheduledActionDrawAndSwapIfPossible() override
        {
            // Only SetNeedsBeginMainFrame the first time this is called
            if (set_needs_commit_on_next_draw_) {
                scheduler_->SetNeedsBeginMainFrame();
                set_needs_commit_on_next_draw_ = false;
            }
            return FakeSchedulerClient::ScheduledActionDrawAndSwapIfPossible();
        }

        DrawResult ScheduledActionDrawAndSwapForced() override
        {
            NOTREACHED();
            return DRAW_SUCCESS;
        }

        void SetNeedsBeginMainFrameOnNextDraw()
        {
            set_needs_commit_on_next_draw_ = true;
        }

    private:
        bool set_needs_commit_on_next_draw_;
    };

    // Tests for the scheduler infinite-looping on SetNeedsBeginMainFrame requests
    // that happen inside a ScheduledActionDrawAndSwap
    TEST_F(SchedulerTest, RequestCommitInsideDraw)
    {
        SchedulerClientThatSetNeedsBeginMainFrameInsideDraw* client = new SchedulerClientThatSetNeedsBeginMainFrameInsideDraw;

        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(make_scoped_ptr(client).Pass(), true);

        EXPECT_FALSE(client->needs_begin_frames());
        scheduler_->SetNeedsRedraw();
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_EQ(0, client->num_draws());
        EXPECT_TRUE(client->needs_begin_frames());

        client->SetNeedsBeginMainFrameOnNextDraw();
        EXPECT_SCOPED(AdvanceFrame());
        client->SetNeedsBeginMainFrameOnNextDraw();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client->num_draws());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(client->needs_begin_frames());
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();

        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(2, client->num_draws());

        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->CommitPending());
        EXPECT_TRUE(client->needs_begin_frames());

        // We stop requesting BeginImplFrames after a BeginImplFrame where we don't
        // swap.
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(2, client->num_draws());
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->CommitPending());
        EXPECT_FALSE(client->needs_begin_frames());
    }

    // Tests that when a draw fails then the pending commit should not be dropped.
    TEST_F(SchedulerTest, RequestCommitInsideFailedDraw)
    {
        SchedulerClientThatsetNeedsDrawInsideDraw* client = new SchedulerClientThatsetNeedsDrawInsideDraw;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(make_scoped_ptr(client).Pass(), true);

        client->SetDrawWillHappen(false);

        scheduler_->SetNeedsRedraw();
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());
        EXPECT_EQ(0, client->num_draws());

        // Fail the draw.
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client->num_draws());

        // We have a commit pending and the draw failed, and we didn't lose the commit
        // request.
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());

        // Fail the draw again.
        EXPECT_SCOPED(AdvanceFrame());

        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(2, client->num_draws());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());

        // Draw successfully.
        client->SetDrawWillHappen(true);
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(3, client->num_draws());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());
    }

    TEST_F(SchedulerTest, NoSwapWhenDrawFails)
    {
        SchedulerClientThatSetNeedsBeginMainFrameInsideDraw* client = new SchedulerClientThatSetNeedsBeginMainFrameInsideDraw;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(make_scoped_ptr(client).Pass(), true);

        scheduler_->SetNeedsRedraw();
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());
        EXPECT_EQ(0, client->num_draws());

        // Draw successfully, this starts a new frame.
        client->SetNeedsBeginMainFrameOnNextDraw();
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client->num_draws());

        scheduler_->SetNeedsRedraw();
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(client->needs_begin_frames());

        // Fail to draw, this should not start a frame.
        client->SetDrawWillHappen(false);
        client->SetNeedsBeginMainFrameOnNextDraw();
        EXPECT_SCOPED(AdvanceFrame());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(2, client->num_draws());
    }

    class SchedulerClientNeedsPrepareTilesInDraw : public FakeSchedulerClient {
    public:
        DrawResult ScheduledActionDrawAndSwapIfPossible() override
        {
            scheduler_->SetNeedsPrepareTiles();
            return FakeSchedulerClient::ScheduledActionDrawAndSwapIfPossible();
        }
    };

    // Test prepare tiles is independant of draws.
    TEST_F(SchedulerTest, PrepareTiles)
    {
        SchedulerClientNeedsPrepareTilesInDraw* client = new SchedulerClientNeedsPrepareTilesInDraw;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(make_scoped_ptr(client).Pass(), true);

        // Request both draw and prepare tiles. PrepareTiles shouldn't
        // be trigged until BeginImplFrame.
        client->Reset();
        scheduler_->SetNeedsPrepareTiles();
        scheduler_->SetNeedsRedraw();
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_TRUE(scheduler_->PrepareTilesPending());
        EXPECT_TRUE(client->needs_begin_frames());
        EXPECT_EQ(0, client->num_draws());
        EXPECT_FALSE(client->HasAction("ScheduledActionPrepareTiles"));
        EXPECT_FALSE(client->HasAction("ScheduledActionDrawAndSwapIfPossible"));

        // We have no immediate actions to perform, so the BeginImplFrame should post
        // the deadline task.
        client->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        // On the deadline, he actions should have occured in the right order.
        client->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client->num_draws());
        EXPECT_TRUE(client->HasAction("ScheduledActionDrawAndSwapIfPossible"));
        EXPECT_TRUE(client->HasAction("ScheduledActionPrepareTiles"));
        EXPECT_LT(client->ActionIndex("ScheduledActionDrawAndSwapIfPossible"),
            client->ActionIndex("ScheduledActionPrepareTiles"));
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

        // Request a draw. We don't need a PrepareTiles yet.
        client->Reset();
        scheduler_->SetNeedsRedraw();
        EXPECT_TRUE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        EXPECT_TRUE(client->needs_begin_frames());
        EXPECT_EQ(0, client->num_draws());

        // We have no immediate actions to perform, so the BeginImplFrame should post
        // the deadline task.
        client->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        // Draw. The draw will trigger SetNeedsPrepareTiles, and
        // then the PrepareTiles action will be triggered after the Draw.
        // Afterwards, neither a draw nor PrepareTiles are pending.
        client->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client->num_draws());
        EXPECT_TRUE(client->HasAction("ScheduledActionDrawAndSwapIfPossible"));
        EXPECT_TRUE(client->HasAction("ScheduledActionPrepareTiles"));
        EXPECT_LT(client->ActionIndex("ScheduledActionDrawAndSwapIfPossible"),
            client->ActionIndex("ScheduledActionPrepareTiles"));
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

        // We need a BeginImplFrame where we don't swap to go idle.
        client->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 0, 2);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_EQ(0, client->num_draws());

        // Now trigger a PrepareTiles outside of a draw. We will then need
        // a begin-frame for the PrepareTiles, but we don't need a draw.
        client->Reset();
        EXPECT_FALSE(client->needs_begin_frames());
        scheduler_->SetNeedsPrepareTiles();
        EXPECT_TRUE(client->needs_begin_frames());
        EXPECT_TRUE(scheduler_->PrepareTilesPending());
        EXPECT_FALSE(scheduler_->RedrawPending());

        // BeginImplFrame. There will be no draw, only PrepareTiles.
        client->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(0, client->num_draws());
        EXPECT_FALSE(client->HasAction("ScheduledActionDrawAndSwapIfPossible"));
        EXPECT_TRUE(client->HasAction("ScheduledActionPrepareTiles"));
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
    }

    // Test that PrepareTiles only happens once per frame.  If an external caller
    // initiates it, then the state machine should not PrepareTiles on that frame.
    TEST_F(SchedulerTest, PrepareTilesOncePerFrame)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // If DidPrepareTiles during a frame, then PrepareTiles should not occur
        // again.
        scheduler_->SetNeedsPrepareTiles();
        scheduler_->SetNeedsRedraw();
        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        EXPECT_TRUE(scheduler_->PrepareTilesPending());
        scheduler_->WillPrepareTiles();
        scheduler_->DidPrepareTiles(); // An explicit PrepareTiles.
        EXPECT_FALSE(scheduler_->PrepareTilesPending());

        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client_->num_draws());
        EXPECT_TRUE(client_->HasAction("ScheduledActionDrawAndSwapIfPossible"));
        EXPECT_FALSE(client_->HasAction("ScheduledActionPrepareTiles"));
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

        // Next frame without DidPrepareTiles should PrepareTiles with draw.
        scheduler_->SetNeedsPrepareTiles();
        scheduler_->SetNeedsRedraw();
        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client_->num_draws());
        EXPECT_TRUE(client_->HasAction("ScheduledActionDrawAndSwapIfPossible"));
        EXPECT_TRUE(client_->HasAction("ScheduledActionPrepareTiles"));
        EXPECT_LT(client_->ActionIndex("ScheduledActionDrawAndSwapIfPossible"),
            client_->ActionIndex("ScheduledActionPrepareTiles"));
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

        // If we get another DidPrepareTiles within the same frame, we should
        // not PrepareTiles on the next frame.
        scheduler_->WillPrepareTiles();
        scheduler_->DidPrepareTiles(); // An explicit PrepareTiles.
        scheduler_->SetNeedsPrepareTiles();
        scheduler_->SetNeedsRedraw();
        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        EXPECT_TRUE(scheduler_->PrepareTilesPending());

        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client_->num_draws());
        EXPECT_TRUE(client_->HasAction("ScheduledActionDrawAndSwapIfPossible"));
        EXPECT_FALSE(client_->HasAction("ScheduledActionPrepareTiles"));
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

        // If we get another DidPrepareTiles, we should not PrepareTiles on the next
        // frame. This verifies we don't alternate calling PrepareTiles once and
        // twice.
        EXPECT_TRUE(scheduler_->PrepareTilesPending());
        scheduler_->WillPrepareTiles();
        scheduler_->DidPrepareTiles(); // An explicit PrepareTiles.
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        scheduler_->SetNeedsPrepareTiles();
        scheduler_->SetNeedsRedraw();
        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        EXPECT_TRUE(scheduler_->PrepareTilesPending());

        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client_->num_draws());
        EXPECT_TRUE(client_->HasAction("ScheduledActionDrawAndSwapIfPossible"));
        EXPECT_FALSE(client_->HasAction("ScheduledActionPrepareTiles"));
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

        // Next frame without DidPrepareTiles should PrepareTiles with draw.
        scheduler_->SetNeedsPrepareTiles();
        scheduler_->SetNeedsRedraw();
        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client_->num_draws());
        EXPECT_TRUE(client_->HasAction("ScheduledActionDrawAndSwapIfPossible"));
        EXPECT_TRUE(client_->HasAction("ScheduledActionPrepareTiles"));
        EXPECT_LT(client_->ActionIndex("ScheduledActionDrawAndSwapIfPossible"),
            client_->ActionIndex("ScheduledActionPrepareTiles"));
        EXPECT_FALSE(scheduler_->RedrawPending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
    }

    TEST_F(SchedulerTest, PrepareTilesFunnelResetOnVisibilityChange)
    {
        scoped_ptr<SchedulerClientNeedsPrepareTilesInDraw> client = make_scoped_ptr(new SchedulerClientNeedsPrepareTilesInDraw);
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(client.Pass(), true);

        // Simulate a few visibility changes and associated PrepareTiles.
        for (int i = 0; i < 10; i++) {
            scheduler_->SetVisible(false);
            scheduler_->WillPrepareTiles();
            scheduler_->DidPrepareTiles();

            scheduler_->SetVisible(true);
            scheduler_->WillPrepareTiles();
            scheduler_->DidPrepareTiles();
        }

        client_->Reset();
        scheduler_->SetNeedsRedraw();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionPrepareTiles", client_, 1, 2);
    }

    TEST_F(SchedulerTest, TriggerBeginFrameDeadlineEarly)
    {
        SchedulerClientNeedsPrepareTilesInDraw* client = new SchedulerClientNeedsPrepareTilesInDraw;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(make_scoped_ptr(client).Pass(), true);

        scheduler_->SetNeedsRedraw();
        EXPECT_SCOPED(AdvanceFrame());

        // The deadline should be zero since there is no work other than drawing
        // pending.
        EXPECT_EQ(base::TimeTicks(), client->posted_begin_impl_frame_deadline());
    }

    TEST_F(SchedulerTest, WaitForReadyToDrawDoNotPostDeadline)
    {
        SchedulerClientNeedsPrepareTilesInDraw* client = new SchedulerClientNeedsPrepareTilesInDraw;
        scheduler_settings_.use_external_begin_frame_source = true;
        scheduler_settings_.commit_to_active_tree = true;
        SetUpScheduler(make_scoped_ptr(client).Pass(), true);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        // Begin new frame.
        EXPECT_SCOPED(AdvanceFrame());
        scheduler_->NotifyBeginMainFrameStarted();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);

        client_->Reset();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);

        client_->Reset();
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);

        // Scheduler won't post deadline in the mode.
        client_->Reset();
        task_runner().RunPendingTasks(); // Try to run posted deadline.
        // There is no posted deadline.
        EXPECT_NO_ACTION(client_);

        // Scheduler received ready to draw signal, and posted deadline.
        scheduler_->NotifyReadyToDraw();
        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_EQ(1, client_->num_draws());
        EXPECT_TRUE(client_->HasAction("ScheduledActionDrawAndSwapIfPossible"));
    }

    TEST_F(SchedulerTest, WaitForReadyToDrawCancelledWhenLostOutputSurface)
    {
        SchedulerClientNeedsPrepareTilesInDraw* client = new SchedulerClientNeedsPrepareTilesInDraw;
        scheduler_settings_.use_external_begin_frame_source = true;
        scheduler_settings_.commit_to_active_tree = true;
        SetUpScheduler(make_scoped_ptr(client).Pass(), true);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        // Begin new frame.
        EXPECT_SCOPED(AdvanceFrame());
        scheduler_->NotifyBeginMainFrameStarted();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);

        client_->Reset();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);

        client_->Reset();
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);

        // Scheduler won't post deadline in the mode.
        client_->Reset();
        task_runner().RunPendingTasks(); // Try to run posted deadline.
        // There is no posted deadline.
        EXPECT_NO_ACTION(client_);

        // Scheduler loses output surface, and stops waiting for ready to draw signal.
        client_->Reset();
        scheduler_->DidLoseOutputSurface();
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
    }

    void SchedulerTest::CheckMainFrameSkippedAfterLateCommit(
        bool expect_send_begin_main_frame)
    {
        // Impl thread hits deadline before commit finishes.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        EXPECT_ACTION("SetNeedsBeginFrames(true)", client_, 0, 5);
        EXPECT_ACTION("WillBeginImplFrame", client_, 1, 5);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 2, 5);
        EXPECT_ACTION("ScheduledActionCommit", client_, 3, 5);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 4, 5);
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());

        client_->Reset();
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_EQ(expect_send_begin_main_frame,
            scheduler_->MainThreadMissedLastDeadline());
        EXPECT_EQ(expect_send_begin_main_frame,
            client_->HasAction("ScheduledActionSendBeginMainFrame"));
    }

    TEST_F(SchedulerTest, MainFrameSkippedAfterLateCommit)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);

        bool expect_send_begin_main_frame = false;
        EXPECT_SCOPED(
            CheckMainFrameSkippedAfterLateCommit(expect_send_begin_main_frame));
    }

    TEST_F(SchedulerTest,
        MainFrameNotSkippedAfterLateCommitInPreferImplLatencyMode)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);
        scheduler_->SetImplLatencyTakesPriority(true);

        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);

        bool expect_send_begin_main_frame = true;
        EXPECT_SCOPED(
            CheckMainFrameSkippedAfterLateCommit(expect_send_begin_main_frame));
    }

    TEST_F(SchedulerTest,
        MainFrameNotSkippedAfterLateCommit_CommitEstimateTooLong)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetBeginMainFrameToCommitDurationEstimate(
            slow_duration);

        bool expect_send_begin_main_frame = true;
        EXPECT_SCOPED(
            CheckMainFrameSkippedAfterLateCommit(expect_send_begin_main_frame));
    }

    TEST_F(SchedulerTest,
        MainFrameNotSkippedAfterLateCommit_ReadyToActivateEstimateTooLong)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetCommitToReadyToActivateDurationEstimate(
            slow_duration);

        bool expect_send_begin_main_frame = true;
        EXPECT_SCOPED(
            CheckMainFrameSkippedAfterLateCommit(expect_send_begin_main_frame));
    }

    TEST_F(SchedulerTest,
        MainFrameNotSkippedAfterLateCommit_ActivateEstimateTooLong)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetActivateDurationEstimate(slow_duration);

        bool expect_send_begin_main_frame = true;
        EXPECT_SCOPED(
            CheckMainFrameSkippedAfterLateCommit(expect_send_begin_main_frame));
    }

    TEST_F(SchedulerTest, MainFrameNotSkippedAfterLateCommit_DrawEstimateTooLong)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetDrawDurationEstimate(slow_duration);

        bool expect_send_begin_main_frame = true;
        EXPECT_SCOPED(
            CheckMainFrameSkippedAfterLateCommit(expect_send_begin_main_frame));
    }

    void SchedulerTest::ImplFrameSkippedAfterLateSwapAck(
        bool swap_ack_before_deadline)
    {
        // To get into a high latency state, this test disables automatic swap acks.
        scheduler_->SetMaxSwapsPending(1);
        client_->SetAutomaticSwapAck(false);

        // Draw and swap for first BeginFrame
        client_->Reset();
        scheduler_->SetNeedsBeginMainFrame();
        scheduler_->SetNeedsRedraw();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        SendNextBeginFrame();
        EXPECT_ACTION("SetNeedsBeginFrames(true)", client_, 0, 4);
        EXPECT_ACTION("WillBeginImplFrame", client_, 1, 4);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 2, 4);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 3, 4);

        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionCommit", client_, 0, 4);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 1, 4);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 2, 4);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 3, 4);

        // Verify we skip every other frame if the swap ack consistently
        // comes back late.
        for (int i = 0; i < 10; i++) {
            // Not calling scheduler_->DidSwapBuffersComplete() until after next
            // BeginImplFrame puts the impl thread in high latency mode.
            client_->Reset();
            scheduler_->SetNeedsBeginMainFrame();
            scheduler_->SetNeedsRedraw();
            EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
            SendNextBeginFrame();
            // Verify that we skip the BeginImplFrame
            EXPECT_NO_ACTION(client_);
            EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
            EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());

            // Verify that we do not perform any actions after we are no longer
            // swap throttled.
            client_->Reset();
            if (swap_ack_before_deadline) {
                // It shouldn't matter if the swap ack comes back before the deadline...
                scheduler_->DidSwapBuffersComplete();
                task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
            } else {
                // ... or after the deadline.
                task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
                scheduler_->DidSwapBuffersComplete();
            }
            EXPECT_NO_ACTION(client_);

            // Verify that we start the next BeginImplFrame and continue normally
            // after having just skipped a BeginImplFrame.
            client_->Reset();
            EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
            SendNextBeginFrame();
            EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
            EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 3);
            EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 2, 3);

            client_->Reset();
            scheduler_->NotifyBeginMainFrameStarted();
            scheduler_->NotifyReadyToCommit();
            scheduler_->NotifyReadyToActivate();
            task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
            EXPECT_ACTION("ScheduledActionCommit", client_, 0, 4);
            EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 1, 4);
            EXPECT_ACTION("ScheduledActionAnimate", client_, 2, 4);
            EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 3, 4);
        }
    }

    TEST_F(SchedulerTest,
        ImplFrameSkippedAfterLateSwapAck_FastEstimates_SwapAckThenDeadline)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);

        bool swap_ack_before_deadline = true;
        EXPECT_SCOPED(ImplFrameSkippedAfterLateSwapAck(swap_ack_before_deadline));
    }

    TEST_F(SchedulerTest,
        ImplFrameSkippedAfterLateSwapAck_FastEstimates_DeadlineThenSwapAck)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);

        bool swap_ack_before_deadline = false;
        EXPECT_SCOPED(ImplFrameSkippedAfterLateSwapAck(swap_ack_before_deadline));
    }

    TEST_F(SchedulerTest,
        ImplFrameSkippedAfterLateSwapAck_ImplLatencyTakesPriority)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // Even if every estimate related to the main thread is slow, we should
        // still expect to recover impl thread latency if the draw is fast and we
        // are in impl latency takes priority.
        scheduler_->SetImplLatencyTakesPriority(true);
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(slow_duration);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetDrawDurationEstimate(fast_duration);

        bool swap_ack_before_deadline = false;
        EXPECT_SCOPED(ImplFrameSkippedAfterLateSwapAck(swap_ack_before_deadline));
    }

    TEST_F(SchedulerTest,
        ImplFrameSkippedAfterLateSwapAck_OnlyImplSideUpdatesExpected)
    {
        // This tests that we recover impl thread latency when there are no commits.
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // To get into a high latency state, this test disables automatic swap acks.
        scheduler_->SetMaxSwapsPending(1);
        client_->SetAutomaticSwapAck(false);

        // Even if every estimate related to the main thread is slow, we should
        // still expect to recover impl thread latency if there are no commits from
        // the main thread.
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(slow_duration);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetDrawDurationEstimate(fast_duration);

        // Draw and swap for first BeginFrame
        client_->Reset();
        scheduler_->SetNeedsRedraw();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        SendNextBeginFrame();
        EXPECT_ACTION("SetNeedsBeginFrames(true)", client_, 0, 3);
        EXPECT_ACTION("WillBeginImplFrame", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 2, 3);

        client_->Reset();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);

        // Verify we skip every other frame if the swap ack consistently
        // comes back late.
        for (int i = 0; i < 10; i++) {
            // Not calling scheduler_->DidSwapBuffersComplete() until after next
            // BeginImplFrame puts the impl thread in high latency mode.
            client_->Reset();
            scheduler_->SetNeedsRedraw();
            EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
            SendNextBeginFrame();
            // Verify that we skip the BeginImplFrame
            EXPECT_NO_ACTION(client_);
            EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
            EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());

            // Verify that we do not perform any actions after we are no longer
            // swap throttled.
            client_->Reset();
            scheduler_->DidSwapBuffersComplete();
            EXPECT_NO_ACTION(client_);

            // Verify that we start the next BeginImplFrame and continue normally
            // after having just skipped a BeginImplFrame.
            client_->Reset();
            EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
            SendNextBeginFrame();
            EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
            EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);

            client_->Reset();
            // Deadline should be immediate.
            EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
            task_runner().RunUntilTime(now_src_->NowTicks());
            EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
            EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
        }
    }

    void SchedulerTest::ImplFrameIsNotSkippedAfterLateSwapAck()
    {
        // To get into a high latency state, this test disables automatic swap acks.
        scheduler_->SetMaxSwapsPending(1);
        client_->SetAutomaticSwapAck(false);

        // Draw and swap for first BeginFrame
        client_->Reset();
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        SendNextBeginFrame();
        EXPECT_ACTION("SetNeedsBeginFrames(true)", client_, 0, 3);
        EXPECT_ACTION("WillBeginImplFrame", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 2, 3);

        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionCommit", client_, 0, 4);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 1, 4);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 2, 4);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 3, 4);

        // Verify impl thread consistently operates in high latency mode
        // without skipping any frames.
        for (int i = 0; i < 10; i++) {
            // Not calling scheduler_->DidSwapBuffersComplete() until after next frame
            // puts the impl thread in high latency mode.
            client_->Reset();
            scheduler_->SetNeedsBeginMainFrame();
            EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
            SendNextBeginFrame();
            EXPECT_SINGLE_ACTION("WillBeginImplFrame", client_);
            EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
            EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());

            client_->Reset();
            scheduler_->DidSwapBuffersComplete();
            scheduler_->NotifyBeginMainFrameStarted();
            scheduler_->NotifyReadyToCommit();
            scheduler_->NotifyReadyToActivate();
            task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));

            // Verify that we don't skip the actions of the BeginImplFrame
            EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 0, 5);
            EXPECT_ACTION("ScheduledActionCommit", client_, 1, 5);
            EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 2, 5);
            EXPECT_ACTION("ScheduledActionAnimate", client_, 3, 5);
            EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 4, 5);
        }
    }

    TEST_F(SchedulerTest,
        ImplFrameIsNotSkippedAfterLateSwapAck_CommitEstimateTooLong)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetBeginMainFrameToCommitDurationEstimate(
            slow_duration);
        EXPECT_SCOPED(ImplFrameIsNotSkippedAfterLateSwapAck());
    }

    TEST_F(SchedulerTest,
        ImplFrameIsNotSkippedAfterLateSwapAck_ReadyToActivateEstimateTooLong)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetCommitToReadyToActivateDurationEstimate(
            slow_duration);
        EXPECT_SCOPED(ImplFrameIsNotSkippedAfterLateSwapAck());
    }

    TEST_F(SchedulerTest,
        ImplFrameIsNotSkippedAfterLateSwapAck_ActivateEstimateTooLong)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetActivateDurationEstimate(slow_duration);
        EXPECT_SCOPED(ImplFrameIsNotSkippedAfterLateSwapAck());
    }

    TEST_F(SchedulerTest,
        ImplFrameIsNotSkippedAfterLateSwapAck_DrawEstimateTooLong)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);
        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetDrawDurationEstimate(slow_duration);
        EXPECT_SCOPED(ImplFrameIsNotSkippedAfterLateSwapAck());
    }

    TEST_F(SchedulerTest,
        MainFrameThenImplFrameSkippedAfterLateCommitAndLateSwapAck)
    {
        // Set up client with custom estimates.
        // This test starts off with expensive estimates to prevent latency recovery
        // initially, then lowers the estimates to enable it once both the main
        // and impl threads are in a high latency mode.
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        auto slow_duration = base::TimeDelta::FromSeconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(slow_duration);

        // To get into a high latency state, this test disables automatic swap acks.
        scheduler_->SetMaxSwapsPending(1);
        client_->SetAutomaticSwapAck(false);

        // Impl thread hits deadline before commit finishes to make
        // MainThreadMissedLastDeadline true
        client_->Reset();
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());

        EXPECT_ACTION("SetNeedsBeginFrames(true)", client_, 0, 5);
        EXPECT_ACTION("WillBeginImplFrame", client_, 1, 5);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 2, 5);
        EXPECT_ACTION("ScheduledActionCommit", client_, 3, 5);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 4, 5);

        // Draw and swap for first commit, start second commit.
        client_->Reset();
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();

        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 6);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 6);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 2, 6);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 3, 6);
        EXPECT_ACTION("ScheduledActionCommit", client_, 4, 6);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 5, 6);

        // Don't call scheduler_->DidSwapBuffersComplete() until after next frame
        // to put the impl thread in a high latency mode.
        client_->Reset();
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));

        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        // Note: BeginMainFrame and swap are skipped here because of
        // swap ack backpressure, not because of latency recovery.
        EXPECT_FALSE(client_->HasAction("ScheduledActionSendBeginMainFrame"));
        EXPECT_FALSE(client_->HasAction("ScheduledActionDrawAndSwapIfPossible"));
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());

        // Lower estimates so that the scheduler will attempt latency recovery.
        auto fast_duration = base::TimeDelta::FromMilliseconds(1);
        fake_compositor_timing_history_->SetAllEstimatesTo(fast_duration);

        // Now that both threads are in a high latency mode, make sure we
        // skip the BeginMainFrame, then the BeginImplFrame, but not both
        // at the same time.

        // Verify we skip BeginMainFrame first.
        client_->Reset();
        // Previous commit request is still outstanding.
        EXPECT_TRUE(scheduler_->NeedsBeginMainFrame());
        EXPECT_TRUE(scheduler_->SwapThrottled());
        SendNextBeginFrame();
        EXPECT_TRUE(scheduler_->MainThreadMissedLastDeadline());
        scheduler_->DidSwapBuffersComplete();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));

        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 2, 3);

        // Verify we skip the BeginImplFrame second.
        client_->Reset();
        // Previous commit request is still outstanding.
        EXPECT_TRUE(scheduler_->NeedsBeginMainFrame());
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        SendNextBeginFrame();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        scheduler_->DidSwapBuffersComplete();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());

        EXPECT_NO_ACTION(client_);

        // Then verify we operate in a low latency mode.
        client_->Reset();
        // Previous commit request is still outstanding.
        EXPECT_TRUE(scheduler_->NeedsBeginMainFrame());
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        SendNextBeginFrame();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());
        scheduler_->DidSwapBuffersComplete();
        EXPECT_FALSE(scheduler_->MainThreadMissedLastDeadline());

        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 6);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 6);
        EXPECT_ACTION("ScheduledActionCommit", client_, 2, 6);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 3, 6);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 4, 6);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 5, 6);
    }

    TEST_F(
        SchedulerTest,
        Deadlock_CommitMakesProgressWhileSwapTrottledAndActiveTreeNeedsFirstDraw)
    {
        // NPAPI plugins on Windows block the Browser UI thread on the Renderer main
        // thread. This prevents the scheduler from receiving any pending swap acks.

        scheduler_settings_.use_external_begin_frame_source = true;
        scheduler_settings_.main_frame_while_swap_throttled_enabled = true;
        SetUpScheduler(true);

        // Disables automatic swap acks so this test can force swap ack throttling
        // to simulate a blocked Browser ui thread.
        scheduler_->SetMaxSwapsPending(1);
        client_->SetAutomaticSwapAck(false);

        // Get a new active tree in main-thread high latency mode and put us
        // in a swap throttled state.
        client_->Reset();
        EXPECT_FALSE(scheduler_->CommitPending());
        scheduler_->SetNeedsBeginMainFrame();
        scheduler_->SetNeedsRedraw();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        EXPECT_FALSE(scheduler_->CommitPending());
        EXPECT_ACTION("SetNeedsBeginFrames(true)", client_, 0, 7);
        EXPECT_ACTION("WillBeginImplFrame", client_, 1, 7);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 2, 7);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 3, 7);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 4, 7);
        EXPECT_ACTION("ScheduledActionCommit", client_, 5, 7);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 6, 7);

        // Make sure that we can finish the next commit even while swap throttled.
        client_->Reset();
        EXPECT_FALSE(scheduler_->CommitPending());
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SCOPED(AdvanceFrame());
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 5);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 5);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 2, 5);
        EXPECT_ACTION("ScheduledActionCommit", client_, 3, 5);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 4, 5);

        // Make sure we do not send a BeginMainFrame while swap throttled and
        // we have both a pending tree and an active tree.
        client_->Reset();
        EXPECT_FALSE(scheduler_->CommitPending());
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_FALSE(scheduler_->CommitPending());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
    }

    TEST_F(SchedulerTest,
        Deadlock_NoBeginMainFrameWhileSwapTrottledAndPipelineFull)
    {
        // NPAPI plugins on Windows block the Browser UI thread on the Renderer main
        // thread. This prevents the scheduler from receiving any pending swap acks.

        // This particular test makes sure we do not send a BeginMainFrame while
        // swap trottled and we have a pending tree and active tree that
        // still needs to be drawn for the first time.

        scheduler_settings_.use_external_begin_frame_source = true;
        scheduler_settings_.main_frame_while_swap_throttled_enabled = true;
        scheduler_settings_.main_frame_before_activation_enabled = true;
        SetUpScheduler(true);

        // Disables automatic swap acks so this test can force swap ack throttling
        // to simulate a blocked Browser ui thread.
        scheduler_->SetMaxSwapsPending(1);
        client_->SetAutomaticSwapAck(false);

        // Start a new commit in main-thread high latency mode and hold off on
        // activation.
        client_->Reset();
        EXPECT_FALSE(scheduler_->CommitPending());
        scheduler_->SetNeedsBeginMainFrame();
        scheduler_->SetNeedsRedraw();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        scheduler_->DidSwapBuffersComplete();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_FALSE(scheduler_->CommitPending());
        EXPECT_ACTION("SetNeedsBeginFrames(true)", client_, 0, 6);
        EXPECT_ACTION("WillBeginImplFrame", client_, 1, 6);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 2, 6);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 3, 6);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 4, 6);
        EXPECT_ACTION("ScheduledActionCommit", client_, 5, 6);

        // Start another commit while we still have aa pending tree.
        // Enter a swap throttled state.
        client_->Reset();
        EXPECT_FALSE(scheduler_->CommitPending());
        scheduler_->SetNeedsBeginMainFrame();
        scheduler_->SetNeedsRedraw();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        scheduler_->NotifyBeginMainFrameStarted();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 4);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 4);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 2, 4);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 3, 4);

        // Can't commit yet because there's still a pending tree.
        client_->Reset();
        scheduler_->NotifyReadyToCommit();
        EXPECT_NO_ACTION(client_);

        // Activate the pending tree, which also unblocks the commit immediately.
        client_->Reset();
        scheduler_->NotifyReadyToActivate();
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionCommit", client_, 1, 2);

        // Make sure we do not send a BeginMainFrame while swap throttled and
        // we have both a pending tree and an active tree that still needs
        // it's first draw.
        client_->Reset();
        EXPECT_FALSE(scheduler_->CommitPending());
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_FALSE(scheduler_->CommitPending());
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
    }

    TEST_F(
        SchedulerTest,
        CommitMakesProgressWhenIdleAndHasPendingTreeAndActiveTreeNeedsFirstDraw)
    {
        // This verifies we don't block commits longer than we need to
        // for performance reasons - not deadlock reasons.

        // Since we are simulating a long commit, set up a client with draw duration
        // estimates that prevent skipping main frames to get to low latency mode.
        scheduler_settings_.use_external_begin_frame_source = true;
        scheduler_settings_.main_frame_while_swap_throttled_enabled = true;
        scheduler_settings_.main_frame_before_activation_enabled = true;
        SetUpScheduler(true);

        // Disables automatic swap acks so this test can force swap ack throttling
        // to simulate a blocked Browser ui thread.
        scheduler_->SetMaxSwapsPending(1);
        client_->SetAutomaticSwapAck(false);

        // Start a new commit in main-thread high latency mode and hold off on
        // activation.
        client_->Reset();
        EXPECT_FALSE(scheduler_->CommitPending());
        scheduler_->SetNeedsBeginMainFrame();
        scheduler_->SetNeedsRedraw();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        scheduler_->DidSwapBuffersComplete();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_FALSE(scheduler_->CommitPending());
        EXPECT_ACTION("SetNeedsBeginFrames(true)", client_, 0, 6);
        EXPECT_ACTION("WillBeginImplFrame", client_, 1, 6);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 2, 6);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 3, 6);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 4, 6);
        EXPECT_ACTION("ScheduledActionCommit", client_, 5, 6);

        // Start another commit while we still have an active tree.
        client_->Reset();
        EXPECT_FALSE(scheduler_->CommitPending());
        scheduler_->SetNeedsBeginMainFrame();
        scheduler_->SetNeedsRedraw();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(scheduler_->CommitPending());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        scheduler_->DidSwapBuffersComplete();
        scheduler_->NotifyBeginMainFrameStarted();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 4);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 4);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 2, 4);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 3, 4);

        // Can't commit yet because there's still a pending tree.
        client_->Reset();
        scheduler_->NotifyReadyToCommit();
        EXPECT_NO_ACTION(client_);

        // Activate the pending tree, which also unblocks the commit immediately
        // while we are in an idle state.
        client_->Reset();
        scheduler_->NotifyReadyToActivate();
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionCommit", client_, 1, 2);
    }

    TEST_F(SchedulerTest, BeginRetroFrame)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        // Create a BeginFrame with a long deadline to avoid race conditions.
        // This is the first BeginFrame, which will be handled immediately.
        BeginFrameArgs args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, now_src());
        args.deadline += base::TimeDelta::FromHours(1);
        fake_external_begin_frame_source()->TestOnBeginFrame(args);
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // Queue BeginFrames while we are still handling the previous BeginFrame.
        args.frame_time += base::TimeDelta::FromSeconds(1);
        fake_external_begin_frame_source()->TestOnBeginFrame(args);
        args.frame_time += base::TimeDelta::FromSeconds(1);
        fake_external_begin_frame_source()->TestOnBeginFrame(args);

        // If we don't swap on the deadline, we wait for the next BeginImplFrame.
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_NO_ACTION(client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // NotifyReadyToCommit should trigger the commit.
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // NotifyReadyToActivate should trigger the activation.
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // BeginImplFrame should prepare the draw.
        task_runner().RunPendingTasks(); // Run posted BeginRetroFrame.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // BeginImplFrame deadline should draw.
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 1);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // The following BeginImplFrame deadline should SetNeedsBeginFrame(false)
        // to avoid excessive toggles.
        task_runner().RunPendingTasks(); // Run posted BeginRetroFrame.
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 0, 2);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 1, 2);
        client_->Reset();
    }

    TEST_F(SchedulerTest, BeginRetroFrame_SwapThrottled)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetEstimatedParentDrawTime(base::TimeDelta::FromMicroseconds(1));

        // To test swap ack throttling, this test disables automatic swap acks.
        scheduler_->SetMaxSwapsPending(1);
        client_->SetAutomaticSwapAck(false);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        client_->Reset();
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // Queue BeginFrame while we are still handling the previous BeginFrame.
        SendNextBeginFrame();
        EXPECT_NO_ACTION(client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // NotifyReadyToCommit should trigger the pending commit.
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // NotifyReadyToActivate should trigger the activation and draw.
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // Swapping will put us into a swap throttled state.
        // Run posted deadline.
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionAnimate", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // While swap throttled, BeginRetroFrames should trigger BeginImplFrames
        // but not a BeginMainFrame or draw.
        scheduler_->SetNeedsBeginMainFrame();
        scheduler_->SetNeedsRedraw();
        // Run posted BeginRetroFrame.
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(false));
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // Let time pass sufficiently beyond the regular deadline but not beyond the
        // late deadline.
        now_src()->Advance(BeginFrameArgs::DefaultInterval() - base::TimeDelta::FromMicroseconds(1));
        task_runner().RunUntilTime(now_src()->NowTicks());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        // Take us out of a swap throttled state.
        scheduler_->DidSwapBuffersComplete();
        EXPECT_SINGLE_ACTION("ScheduledActionSendBeginMainFrame", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        // Verify that the deadline was rescheduled.
        task_runner().RunUntilTime(now_src()->NowTicks());
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();
    }

    TEST_F(SchedulerTest, RetroFrameDoesNotExpireTooEarly)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_TRUE(client_->needs_begin_frames());
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();

        client_->Reset();
        BeginFrameArgs retro_frame_args = SendNextBeginFrame();
        // This BeginFrame is queued up as a retro frame.
        EXPECT_NO_ACTION(client_);
        // The previous deadline is still pending.
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        // This main frame activating should schedule the (previous) deadline to
        // trigger immediately.
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        EXPECT_ACTION("ScheduledActionCommit", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 1, 2);

        client_->Reset();
        // The deadline task should trigger causing a draw.
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionAnimate", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 1, 2);

        // Keep animating.
        client_->Reset();
        scheduler_->SetNeedsAnimate();
        scheduler_->SetNeedsRedraw();
        EXPECT_NO_ACTION(client_);

        // Let's advance to the retro frame's deadline.
        now_src()->Advance(retro_frame_args.deadline - now_src()->NowTicks());

        // The retro frame hasn't expired yet.
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(false));
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        // This is an immediate deadline case.
        client_->Reset();
        task_runner().RunPendingTasks();
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
    }

    TEST_F(SchedulerTest, RetroFrameExpiresOnTime)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_TRUE(client_->needs_begin_frames());
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();

        client_->Reset();
        BeginFrameArgs retro_frame_args = SendNextBeginFrame();
        // This BeginFrame is queued up as a retro frame.
        EXPECT_NO_ACTION(client_);
        // The previous deadline is still pending.
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        // This main frame activating should schedule the (previous) deadline to
        // trigger immediately.
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        EXPECT_ACTION("ScheduledActionCommit", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 1, 2);

        client_->Reset();
        // The deadline task should trigger causing a draw.
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionAnimate", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 1, 2);

        // Keep animating.
        client_->Reset();
        scheduler_->SetNeedsAnimate();
        scheduler_->SetNeedsRedraw();
        EXPECT_NO_ACTION(client_);

        // Let's advance sufficiently past the retro frame's deadline.
        now_src()->Advance(retro_frame_args.deadline - now_src()->NowTicks() + base::TimeDelta::FromMicroseconds(1));

        // The retro frame should've expired.
        EXPECT_NO_ACTION(client_);
    }

    TEST_F(SchedulerTest, MissedFrameDoesNotExpireTooEarly)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_TRUE(client_->needs_begin_frames());
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        BeginFrameArgs missed_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, now_src());
        missed_frame_args.type = BeginFrameArgs::MISSED;

        // Advance to the deadline.
        now_src()->Advance(missed_frame_args.deadline - now_src()->NowTicks());

        // Missed frame is handled because it's on time.
        client_->Reset();
        fake_external_begin_frame_source_->TestOnBeginFrame(missed_frame_args);
        EXPECT_TRUE(
            task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(false)));
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
    }

    TEST_F(SchedulerTest, MissedFrameExpiresOnTime)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_TRUE(client_->needs_begin_frames());
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        BeginFrameArgs missed_frame_args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, now_src());
        missed_frame_args.type = BeginFrameArgs::MISSED;

        // Advance sufficiently past the deadline.
        now_src()->Advance(missed_frame_args.deadline - now_src()->NowTicks() + base::TimeDelta::FromMicroseconds(1));

        // Missed frame is dropped because it's too late.
        client_->Reset();
        fake_external_begin_frame_source_->TestOnBeginFrame(missed_frame_args);
        EXPECT_FALSE(
            task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(false)));
        EXPECT_NO_ACTION(client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
    }

    void SchedulerTest::BeginFramesNotFromClient(
        bool use_external_begin_frame_source,
        bool throttle_frame_production)
    {
        scheduler_settings_.use_external_begin_frame_source = use_external_begin_frame_source;
        scheduler_settings_.throttle_frame_production = throttle_frame_production;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame
        // without calling SetNeedsBeginFrame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_NO_ACTION(client_);
        client_->Reset();

        // When the client-driven BeginFrame are disabled, the scheduler posts it's
        // own BeginFrame tasks.
        task_runner().RunPendingTasks(); // Run posted BeginFrame.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // If we don't swap on the deadline, we wait for the next BeginFrame.
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_NO_ACTION(client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // NotifyReadyToCommit should trigger the commit.
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        client_->Reset();

        // NotifyReadyToActivate should trigger the activation.
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        client_->Reset();

        // BeginImplFrame should prepare the draw.
        task_runner().RunPendingTasks(); // Run posted BeginFrame.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // BeginImplFrame deadline should draw.
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 1);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // The following BeginImplFrame deadline should SetNeedsBeginFrame(false)
        // to avoid excessive toggles.
        task_runner().RunPendingTasks(); // Run posted BeginFrame.
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Make sure SetNeedsBeginFrame isn't called on the client
        // when the BeginFrame is no longer needed.
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_SINGLE_ACTION("SendBeginMainFrameNotExpectedSoon", client_);
        client_->Reset();
    }

    TEST_F(SchedulerTest, SyntheticBeginFrames)
    {
        bool use_external_begin_frame_source = false;
        bool throttle_frame_production = true;
        BeginFramesNotFromClient(use_external_begin_frame_source,
            throttle_frame_production);
    }

    TEST_F(SchedulerTest, VSyncThrottlingDisabled)
    {
        bool use_external_begin_frame_source = true;
        bool throttle_frame_production = false;
        BeginFramesNotFromClient(use_external_begin_frame_source,
            throttle_frame_production);
    }

    TEST_F(SchedulerTest, SyntheticBeginFrames_And_VSyncThrottlingDisabled)
    {
        bool use_external_begin_frame_source = false;
        bool throttle_frame_production = false;
        BeginFramesNotFromClient(use_external_begin_frame_source,
            throttle_frame_production);
    }

    void SchedulerTest::BeginFramesNotFromClient_SwapThrottled(
        bool use_external_begin_frame_source,
        bool throttle_frame_production)
    {
        scheduler_settings_.use_external_begin_frame_source = use_external_begin_frame_source;
        scheduler_settings_.throttle_frame_production = throttle_frame_production;
        SetUpScheduler(true);

        scheduler_->SetEstimatedParentDrawTime(base::TimeDelta::FromMicroseconds(1));

        // To test swap ack throttling, this test disables automatic swap acks.
        scheduler_->SetMaxSwapsPending(1);
        client_->SetAutomaticSwapAck(false);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        client_->Reset();
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_NO_ACTION(client_);
        client_->Reset();

        // Trigger the first BeginImplFrame and BeginMainFrame
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // NotifyReadyToCommit should trigger the pending commit.
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        client_->Reset();

        // NotifyReadyToActivate should trigger the activation and draw.
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        client_->Reset();

        // Swapping will put us into a swap throttled state.
        // Run posted deadline.
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionAnimate", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // While swap throttled, BeginFrames should trigger BeginImplFrames,
        // but not a BeginMainFrame or draw.
        scheduler_->SetNeedsBeginMainFrame();
        scheduler_->SetNeedsRedraw();
        EXPECT_SCOPED(AdvanceFrame()); // Run posted BeginFrame.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Let time pass sufficiently beyond the regular deadline but not beyond the
        // late deadline.
        now_src()->Advance(BeginFrameArgs::DefaultInterval() - base::TimeDelta::FromMicroseconds(1));
        task_runner().RunUntilTime(now_src()->NowTicks());
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        // Take us out of a swap throttled state.
        scheduler_->DidSwapBuffersComplete();
        EXPECT_SINGLE_ACTION("ScheduledActionSendBeginMainFrame", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Verify that the deadline was rescheduled.
        // We can't use RunUntilTime(now) here because the next frame is also
        // scheduled if throttle_frame_production = false.
        base::TimeTicks before_deadline = now_src()->NowTicks();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        base::TimeTicks after_deadline = now_src()->NowTicks();
        EXPECT_EQ(after_deadline, before_deadline);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();
    }

    TEST_F(SchedulerTest, SyntheticBeginFrames_SwapThrottled)
    {
        bool use_external_begin_frame_source = false;
        bool throttle_frame_production = true;
        BeginFramesNotFromClient_SwapThrottled(use_external_begin_frame_source,
            throttle_frame_production);
    }

    TEST_F(SchedulerTest, VSyncThrottlingDisabled_SwapThrottled)
    {
        bool use_external_begin_frame_source = true;
        bool throttle_frame_production = false;
        BeginFramesNotFromClient_SwapThrottled(use_external_begin_frame_source,
            throttle_frame_production);
    }

    TEST_F(SchedulerTest,
        SyntheticBeginFrames_And_VSyncThrottlingDisabled_SwapThrottled)
    {
        bool use_external_begin_frame_source = false;
        bool throttle_frame_production = false;
        BeginFramesNotFromClient_SwapThrottled(use_external_begin_frame_source,
            throttle_frame_production);
    }

    TEST_F(SchedulerTest, DidLoseOutputSurfaceAfterOutputSurfaceIsInitialized)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(false);

        scheduler_->SetCanStart();
        scheduler_->SetVisible(true);
        scheduler_->SetCanDraw(true);

        EXPECT_SINGLE_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_);
        client_->Reset();
        scheduler_->DidCreateAndInitializeOutputSurface();
        EXPECT_NO_ACTION(client_);

        scheduler_->DidLoseOutputSurface();
        EXPECT_SINGLE_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_);
    }

    TEST_F(SchedulerTest, DidLoseOutputSurfaceAfterBeginFrameStarted)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->DidLoseOutputSurface();
        // SetNeedsBeginFrames(false) is not called until the end of the frame.
        EXPECT_NO_ACTION(client_);

        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_ACTION("ScheduledActionCommit", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 1, 2);

        client_->Reset();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
    }

    TEST_F(SchedulerTest,
        DidLoseOutputSurfaceAfterBeginFrameStartedWithHighLatency)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->DidLoseOutputSurface();
        // Do nothing when impl frame is in deadine pending state.
        EXPECT_NO_ACTION(client_);

        client_->Reset();
        // Run posted deadline.
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        // OnBeginImplFrameDeadline didn't schedule output surface creation because
        // main frame is not yet completed.
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 0, 2);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

        // BeginImplFrame is not started.
        client_->Reset();
        task_runner().RunUntilTime(now_src()->NowTicks() + base::TimeDelta::FromMilliseconds(10));
        EXPECT_NO_ACTION(client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_ACTION("ScheduledActionCommit", client_, 0, 3);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_, 2, 3);
    }

    TEST_F(SchedulerTest, DidLoseOutputSurfaceAfterReadyToCommit)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);

        client_->Reset();
        scheduler_->DidLoseOutputSurface();
        // Sync tree should be forced to activate.
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);

        // SetNeedsBeginFrames(false) is not called until the end of the frame.
        client_->Reset();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
    }

    TEST_F(SchedulerTest, DidLoseOutputSurfaceAfterSetNeedsPrepareTiles)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsPrepareTiles();
        scheduler_->SetNeedsRedraw();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->DidLoseOutputSurface();
        // SetNeedsBeginFrames(false) is not called until the end of the frame.
        EXPECT_NO_ACTION(client_);

        client_->Reset();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionPrepareTiles", client_, 0, 4);
        EXPECT_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_, 1, 4);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 2, 4);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 3, 4);
    }

    TEST_F(SchedulerTest, DidLoseOutputSurfaceAfterBeginRetroFramePosted)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        // Create a BeginFrame with a long deadline to avoid race conditions.
        // This is the first BeginFrame, which will be handled immediately.
        client_->Reset();
        BeginFrameArgs args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, now_src());
        args.deadline += base::TimeDelta::FromHours(1);
        fake_external_begin_frame_source()->TestOnBeginFrame(args);
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());

        // Queue BeginFrames while we are still handling the previous BeginFrame.
        args.frame_time += base::TimeDelta::FromSeconds(1);
        fake_external_begin_frame_source()->TestOnBeginFrame(args);
        args.frame_time += base::TimeDelta::FromSeconds(1);
        fake_external_begin_frame_source()->TestOnBeginFrame(args);

        // If we don't swap on the deadline, we wait for the next BeginImplFrame.
        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_NO_ACTION(client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());

        // NotifyReadyToCommit should trigger the commit.
        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        EXPECT_TRUE(client_->needs_begin_frames());

        // NotifyReadyToActivate should trigger the activation.
        client_->Reset();
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        EXPECT_TRUE(client_->needs_begin_frames());

        client_->Reset();
        EXPECT_FALSE(scheduler_->IsBeginRetroFrameArgsEmpty());
        scheduler_->DidLoseOutputSurface();
        EXPECT_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
        EXPECT_TRUE(scheduler_->IsBeginRetroFrameArgsEmpty());

        // Posted BeginRetroFrame is aborted.
        client_->Reset();
        task_runner().RunPendingTasks();
        EXPECT_NO_ACTION(client_);
    }

    TEST_F(SchedulerTest, DidLoseOutputSurfaceDuringBeginRetroFrameRunning)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        // Create a BeginFrame with a long deadline to avoid race conditions.
        // This is the first BeginFrame, which will be handled immediately.
        client_->Reset();
        BeginFrameArgs args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, now_src());
        args.deadline += base::TimeDelta::FromHours(1);
        fake_external_begin_frame_source()->TestOnBeginFrame(args);
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());

        // Queue BeginFrames while we are still handling the previous BeginFrame.
        args.frame_time += base::TimeDelta::FromSeconds(1);
        fake_external_begin_frame_source()->TestOnBeginFrame(args);
        args.frame_time += base::TimeDelta::FromSeconds(1);
        fake_external_begin_frame_source()->TestOnBeginFrame(args);

        // If we don't swap on the deadline, we wait for the next BeginImplFrame.
        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_NO_ACTION(client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());

        // NotifyReadyToCommit should trigger the commit.
        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        EXPECT_TRUE(client_->needs_begin_frames());

        // NotifyReadyToActivate should trigger the activation.
        client_->Reset();
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        EXPECT_TRUE(client_->needs_begin_frames());

        // BeginImplFrame should prepare the draw.
        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted BeginRetroFrame.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());

        client_->Reset();
        EXPECT_FALSE(scheduler_->IsBeginRetroFrameArgsEmpty());
        scheduler_->DidLoseOutputSurface();
        EXPECT_NO_ACTION(client_);
        EXPECT_TRUE(scheduler_->IsBeginRetroFrameArgsEmpty());

        // BeginImplFrame deadline should abort drawing.
        client_->Reset();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_FALSE(client_->needs_begin_frames());

        // No more BeginRetroFrame because BeginRetroFrame queue is cleared.
        client_->Reset();
        task_runner().RunPendingTasks();
        EXPECT_NO_ACTION(client_);
    }

    TEST_F(SchedulerTest, DidLoseOutputSurfaceWithSyntheticBeginFrameSource)
    {
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        EXPECT_FALSE(scheduler_->frame_source().NeedsBeginFrames());
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_TRUE(scheduler_->frame_source().NeedsBeginFrames());

        client_->Reset();
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(scheduler_->frame_source().NeedsBeginFrames());

        // NotifyReadyToCommit should trigger the commit.
        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        EXPECT_TRUE(scheduler_->frame_source().NeedsBeginFrames());

        // NotifyReadyToActivate should trigger the activation.
        client_->Reset();
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        EXPECT_TRUE(scheduler_->frame_source().NeedsBeginFrames());

        client_->Reset();
        scheduler_->DidLoseOutputSurface();
        // SetNeedsBeginFrames(false) is not called until the end of the frame.
        EXPECT_NO_ACTION(client_);
        EXPECT_TRUE(scheduler_->frame_source().NeedsBeginFrames());

        client_->Reset();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_, 0, 2);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 1, 2);
        EXPECT_FALSE(scheduler_->frame_source().NeedsBeginFrames());
    }

    TEST_F(SchedulerTest, DidLoseOutputSurfaceWhenIdle)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);

        client_->Reset();
        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);

        client_->Reset();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));
        EXPECT_ACTION("ScheduledActionAnimate", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 1, 2);

        // Idle time between BeginFrames.
        client_->Reset();
        scheduler_->DidLoseOutputSurface();
        EXPECT_ACTION("ScheduledActionBeginOutputSurfaceCreation", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
    }

    TEST_F(SchedulerTest, ScheduledActionActivateAfterBecomingInvisible)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);

        client_->Reset();
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());

        client_->Reset();
        scheduler_->SetVisible(false);
        task_runner().RunPendingTasks(); // Run posted deadline.

        // Sync tree should be forced to activate.
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
    }

    // Tests to ensure frame sources can be successfully changed while drawing.
    TEST_F(SchedulerTest, SwitchFrameSourceToUnthrottled)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsRedraw should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsRedraw();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 1);
        scheduler_->SetNeedsRedraw();

        // Switch to an unthrottled frame source.
        scheduler_->SetThrottleFrameProduction(false);
        client_->Reset();

        // Unthrottled frame source will immediately begin a new frame.
        task_runner().RunPendingTasks(); // Run posted BeginFrame.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // If we don't swap on the deadline, we wait for the next BeginFrame.
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 1);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();
    }

    // Tests to ensure frame sources can be successfully changed while a frame
    // deadline is pending.
    TEST_F(SchedulerTest, SwitchFrameSourceToUnthrottledBeforeDeadline)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsRedraw should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsRedraw();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);

        // Switch to an unthrottled frame source before the frame deadline is hit.
        scheduler_->SetThrottleFrameProduction(false);
        client_->Reset();

        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();

        task_runner().RunPendingTasks(); // Run posted deadline and BeginFrame.
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 2);
        // Unthrottled frame source will immediately begin a new frame.
        EXPECT_ACTION("WillBeginImplFrame", client_, 1, 2);
        scheduler_->SetNeedsRedraw();
        client_->Reset();

        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("ScheduledActionAnimate", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();
    }

    // Tests to ensure that the active frame source can successfully be changed from
    // unthrottled to throttled.
    TEST_F(SchedulerTest, SwitchFrameSourceToThrottled)
    {
        scheduler_settings_.throttle_frame_production = false;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsRedraw();
        EXPECT_NO_ACTION(client_);
        client_->Reset();

        task_runner().RunPendingTasks(); // Run posted BeginFrame.
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 1);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Switch to a throttled frame source.
        scheduler_->SetThrottleFrameProduction(true);
        client_->Reset();

        // SetNeedsRedraw should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsRedraw();
        task_runner().RunPendingTasks();
        EXPECT_NO_ACTION(client_);
        client_->Reset();

        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 2);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_TRUE(client_->needs_begin_frames());
        client_->Reset();
        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 1);
    }

    // Tests to ensure that we send a BeginMainFrameNotExpectedSoon when expected.
    TEST_F(SchedulerTest, SendBeginMainFrameNotExpectedSoon)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        // SetNeedsBeginMainFrame should begin the frame on the next BeginImplFrame.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        // Trigger a frame draw.
        EXPECT_SCOPED(AdvanceFrame());
        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        task_runner().RunPendingTasks();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 6);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 6);
        EXPECT_ACTION("ScheduledActionCommit", client_, 2, 6);
        EXPECT_ACTION("ScheduledActionActivateSyncTree", client_, 3, 6);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 4, 6);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 5, 6);
        client_->Reset();

        // The following BeginImplFrame deadline should SetNeedsBeginFrame(false)
        // and send a SendBeginMainFrameNotExpectedSoon.
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client_);
        EXPECT_TRUE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        task_runner().RunPendingTasks(); // Run posted deadline.
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 0, 2);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 1, 2);
        client_->Reset();
    }

    TEST_F(SchedulerTest, SynchronousCompositorAnimation)
    {
        scheduler_settings_.using_synchronous_renderer_compositor = true;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsAnimate();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        // Next vsync.
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionInvalidateOutputSurface", client_, 2, 3);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Continue with animation.
        scheduler_->SetNeedsAnimate();
        EXPECT_NO_ACTION(client_);

        // Android onDraw.
        scheduler_->SetNeedsRedraw();
        scheduler_->OnDrawForOutputSurface();
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Next vsync.
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionInvalidateOutputSurface", client_, 2, 3);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Android onDraw.
        scheduler_->SetNeedsRedraw();
        scheduler_->OnDrawForOutputSurface();
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Idle on next vsync.
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();
    }

    TEST_F(SchedulerTest, SynchronousCompositorOnDrawDuringIdle)
    {
        scheduler_settings_.using_synchronous_renderer_compositor = true;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsRedraw();
        scheduler_->OnDrawForOutputSurface();
        EXPECT_ACTION("SetNeedsBeginFrames(true)", client_, 0, 3);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 2, 3);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Idle on next vsync.
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();
    }

    TEST_F(SchedulerTest, SynchronousCompositorCommit)
    {
        scheduler_settings_.using_synchronous_renderer_compositor = true;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        // Next vsync.
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        scheduler_->NotifyBeginMainFrameStarted();
        EXPECT_NO_ACTION(client_);

        // Next vsync.
        AdvanceFrame();
        EXPECT_SINGLE_ACTION("WillBeginImplFrame", client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        client_->Reset();

        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        client_->Reset();

        // Next vsync.
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionInvalidateOutputSurface", client_, 2, 3);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Android onDraw.
        scheduler_->SetNeedsRedraw();
        scheduler_->OnDrawForOutputSurface();
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        // Idle on next vsync.
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();
    }

    TEST_F(SchedulerTest, SynchronousCompositorDoubleCommitWithoutDraw)
    {
        scheduler_settings_.using_synchronous_renderer_compositor = true;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        // Next vsync.
        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        scheduler_->NotifyBeginMainFrameStarted();
        EXPECT_NO_ACTION(client_);

        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        client_->Reset();

        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        client_->Reset();

        // Ask for another commit.
        scheduler_->SetNeedsBeginMainFrame();

        AdvanceFrame();
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 4);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 4);
        EXPECT_ACTION("ScheduledActionSendBeginMainFrame", client_, 2, 4);
        EXPECT_ACTION("ScheduledActionInvalidateOutputSurface", client_, 3, 4);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();

        scheduler_->NotifyBeginMainFrameStarted();
        EXPECT_NO_ACTION(client_);

        // Allow new commit even though previous commit hasn't been drawn.
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        client_->Reset();
    }

    class SchedulerClientSetNeedsPrepareTilesOnDraw : public FakeSchedulerClient {
    public:
        SchedulerClientSetNeedsPrepareTilesOnDraw()
            : FakeSchedulerClient()
        {
        }

    protected:
        DrawResult ScheduledActionDrawAndSwapIfPossible() override
        {
            scheduler_->SetNeedsPrepareTiles();
            return FakeSchedulerClient::ScheduledActionDrawAndSwapIfPossible();
        }
    };

    TEST_F(SchedulerTest, SynchronousCompositorPrepareTilesOnDraw)
    {
        scheduler_settings_.using_synchronous_renderer_compositor = true;
        scheduler_settings_.use_external_begin_frame_source = true;

        scoped_ptr<FakeSchedulerClient> client = make_scoped_ptr(new SchedulerClientSetNeedsPrepareTilesOnDraw);
        SetUpScheduler(client.Pass(), true);

        scheduler_->SetNeedsRedraw();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        // Next vsync.
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionInvalidateOutputSurface", client_, 2, 3);
        client_->Reset();

        // Android onDraw.
        scheduler_->SetNeedsRedraw();
        scheduler_->OnDrawForOutputSurface();
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionPrepareTiles", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        client_->Reset();

        // Android onDraw.
        scheduler_->SetNeedsRedraw();
        scheduler_->OnDrawForOutputSurface();
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 0, 2);
        EXPECT_ACTION("ScheduledActionPrepareTiles", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        client_->Reset();

        // Next vsync.
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("SetNeedsBeginFrames(false)", client_, 1, 3);
        EXPECT_ACTION("SendBeginMainFrameNotExpectedSoon", client_, 2, 3);
        EXPECT_FALSE(client_->needs_begin_frames());
        client_->Reset();
    }

    TEST_F(SchedulerTest, SynchronousCompositorSendBeginMainFrameWhileIdle)
    {
        scheduler_settings_.using_synchronous_renderer_compositor = true;
        scheduler_settings_.use_external_begin_frame_source = true;

        SetUpScheduler(true);

        scheduler_->SetNeedsRedraw();
        EXPECT_SINGLE_ACTION("SetNeedsBeginFrames(true)", client_);
        client_->Reset();

        // Next vsync.
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionInvalidateOutputSurface", client_, 2, 3);
        client_->Reset();

        // Android onDraw.
        scheduler_->SetNeedsRedraw();
        scheduler_->OnDrawForOutputSurface();
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        client_->Reset();

        // Simulate SetNeedsBeginMainFrame due to input event.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("ScheduledActionSendBeginMainFrame", client_);
        client_->Reset();

        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        EXPECT_SINGLE_ACTION("ScheduledActionCommit", client_);
        client_->Reset();

        scheduler_->NotifyReadyToActivate();
        EXPECT_SINGLE_ACTION("ScheduledActionActivateSyncTree", client_);
        client_->Reset();

        // Next vsync.
        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_ACTION("WillBeginImplFrame", client_, 0, 3);
        EXPECT_ACTION("ScheduledActionAnimate", client_, 1, 3);
        EXPECT_ACTION("ScheduledActionInvalidateOutputSurface", client_, 2, 3);
        client_->Reset();

        // Android onDraw.
        scheduler_->SetNeedsRedraw();
        scheduler_->OnDrawForOutputSurface();
        EXPECT_SINGLE_ACTION("ScheduledActionDrawAndSwapIfPossible", client_);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        EXPECT_FALSE(scheduler_->PrepareTilesPending());
        client_->Reset();

        // Simulate SetNeedsBeginMainFrame due to input event.
        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SINGLE_ACTION("ScheduledActionSendBeginMainFrame", client_);
        client_->Reset();
    }

    TEST_F(SchedulerTest, SynchronousCompositorOnDrawWhenInvisible)
    {
        scheduler_settings_.using_synchronous_renderer_compositor = true;
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetVisible(false);

        scheduler_->SetNeedsRedraw();
        scheduler_->OnDrawForOutputSurface();
        // Action animate is the result of SetNeedsRedraw.
        EXPECT_ACTION("ScheduledActionAnimate", client_, 0, 2);
        // SynchronousCompositor has to draw regardless of visibility.
        EXPECT_ACTION("ScheduledActionDrawAndSwapIfPossible", client_, 1, 2);
        EXPECT_FALSE(scheduler_->BeginImplFrameDeadlinePending());
        client_->Reset();
    }

    TEST_F(SchedulerTest, AuthoritativeVSyncInterval)
    {
        SetUpScheduler(true);
        base::TimeDelta initial_interval = scheduler_->BeginImplFrameInterval();
        base::TimeDelta authoritative_interval = base::TimeDelta::FromMilliseconds(33);

        scheduler_->SetNeedsBeginMainFrame();
        EXPECT_SCOPED(AdvanceFrame());

        EXPECT_EQ(initial_interval, scheduler_->BeginImplFrameInterval());

        scheduler_->NotifyBeginMainFrameStarted();
        scheduler_->NotifyReadyToCommit();
        scheduler_->NotifyReadyToActivate();
        task_runner().RunTasksWhile(client_->ImplFrameDeadlinePending(true));

        scheduler_->SetAuthoritativeVSyncInterval(authoritative_interval);

        EXPECT_SCOPED(AdvanceFrame());

        // At the next BeginFrame, authoritative interval is used instead of previous
        // interval.
        EXPECT_NE(initial_interval, scheduler_->BeginImplFrameInterval());
        EXPECT_EQ(authoritative_interval, scheduler_->BeginImplFrameInterval());
    }

    TEST_F(SchedulerTest, ImplLatencyTakesPriority)
    {
        SetUpScheduler(true);
        scheduler_->SetImplLatencyTakesPriority(true);
        EXPECT_TRUE(scheduler_->ImplLatencyTakesPriority());

        scheduler_->SetImplLatencyTakesPriority(false);
        EXPECT_FALSE(scheduler_->ImplLatencyTakesPriority());
    }

    TEST_F(SchedulerTest, BeginFrameArgs_OnCriticalPath)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetImplLatencyTakesPriority(false);
        scheduler_->SetChildrenNeedBeginFrames(true);

        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(client_->begin_frame_is_sent_to_children());
        EXPECT_TRUE(client_->begin_frame_args_sent_to_children().on_critical_path);
    }

    TEST_F(SchedulerTest, BeginFrameArgs_NotOnCriticalPath)
    {
        scheduler_settings_.use_external_begin_frame_source = true;
        SetUpScheduler(true);

        scheduler_->SetImplLatencyTakesPriority(true);
        scheduler_->SetChildrenNeedBeginFrames(true);

        EXPECT_SCOPED(AdvanceFrame());
        EXPECT_TRUE(client_->begin_frame_is_sent_to_children());
        EXPECT_FALSE(client_->begin_frame_args_sent_to_children().on_critical_path);
    }

} // namespace
} // namespace cc
