// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/scheduler/scheduler_state_machine.h"

#include "base/trace_event/trace_event.h"
#include "cc/scheduler/scheduler.h"
#include "cc/test/begin_frame_args_test.h"
#include "testing/gtest/include/gtest/gtest.h"

// Macro to compare two enum values and get nice output.
// Without:
//   Value of: actual()      Actual: 7
//   Expected: expected()  Which is: 0
// With:
//   Value of: actual()      Actual: "ACTION_ANIMATE"
//   Expected: expected()  Which is: "ACTION_NONE"
#define EXPECT_ENUM_EQ(enum_tostring, expected, actual)          \
    EXPECT_STREQ(SchedulerStateMachine::enum_tostring(expected), \
        SchedulerStateMachine::enum_tostring(actual))

#define EXPECT_IMPL_FRAME_STATE(expected)                 \
    EXPECT_ENUM_EQ(BeginImplFrameStateToString, expected, \
        state.begin_impl_frame_state())                   \
        << state.AsValue()->ToString()

#define EXPECT_MAIN_FRAME_STATE(expected)                 \
    EXPECT_ENUM_EQ(BeginMainFrameStateToString, expected, \
        state.BeginMainFrameState())

#define EXPECT_ACTION(expected)                                  \
    EXPECT_ENUM_EQ(ActionToString, expected, state.NextAction()) \
        << state.AsValue()->ToString()

#define EXPECT_ACTION_UPDATE_STATE(action)                                                                                                   \
    EXPECT_ACTION(action);                                                                                                                   \
    if (action == SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE || action == SchedulerStateMachine::ACTION_DRAW_AND_SWAP_FORCED) { \
        EXPECT_IMPL_FRAME_STATE(                                                                                                             \
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE);                                                                  \
    }                                                                                                                                        \
    WillPerformAction(&state, action);                                                                                                       \
    if (action == SchedulerStateMachine::ACTION_NONE) {                                                                                      \
        if (state.begin_impl_frame_state() == SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_BEGIN_FRAME_STARTING)                            \
            state.OnBeginImplFrameDeadlinePending();                                                                                         \
        if (state.begin_impl_frame_state() == SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE)                                 \
            state.OnBeginImplFrameIdle();                                                                                                    \
    }

#define SET_UP_STATE(state)                                           \
    state.SetCanStart();                                              \
    state.SetVisible(true);                                           \
    EXPECT_ACTION_UPDATE_STATE(                                       \
        SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION); \
    EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);   \
    state.CreateAndInitializeOutputSurfaceWithActivatedCommit();      \
    state.SetCanDraw(true);

namespace cc {

namespace {

    void WillPerformAction(SchedulerStateMachine* sm,
        SchedulerStateMachine::Action action)
    {
        switch (action) {
        case SchedulerStateMachine::ACTION_NONE:
            return;

        case SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE:
            sm->WillActivate();
            return;

        case SchedulerStateMachine::ACTION_ANIMATE:
            sm->WillAnimate();
            return;

        case SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME:
            sm->WillSendBeginMainFrame();
            return;

        case SchedulerStateMachine::ACTION_COMMIT: {
            bool commit_has_no_updates = false;
            sm->WillCommit(commit_has_no_updates);
            return;
        }

        case SchedulerStateMachine::ACTION_DRAW_AND_SWAP_FORCED:
        case SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE: {
            bool did_request_swap = true;
            sm->WillDraw(did_request_swap);
            return;
        }

        case SchedulerStateMachine::ACTION_DRAW_AND_SWAP_ABORT: {
            bool did_request_swap = false;
            sm->WillDraw(did_request_swap);
            return;
        }

        case SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION:
            sm->WillBeginOutputSurfaceCreation();
            return;

        case SchedulerStateMachine::ACTION_PREPARE_TILES:
            sm->WillPrepareTiles();
            return;

        case SchedulerStateMachine::ACTION_INVALIDATE_OUTPUT_SURFACE:
            sm->WillInvalidateOutputSurface();
            return;
        }
    }

    const SchedulerStateMachine::BeginImplFrameState all_begin_impl_frame_states[] = {
        SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_IDLE,
        SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_BEGIN_FRAME_STARTING,
        SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_BEGIN_FRAME,
        SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE,
    };

    const SchedulerStateMachine::BeginMainFrameState begin_main_frame_states[] = {
        SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE,
        SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT,
        SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_STARTED,
        SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_READY_TO_COMMIT,
        SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_WAITING_FOR_ACTIVATION,
        SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_WAITING_FOR_DRAW
    };

    // Exposes the protected state fields of the SchedulerStateMachine for testing
    class StateMachine : public SchedulerStateMachine {
    public:
        explicit StateMachine(const SchedulerSettings& scheduler_settings)
            : SchedulerStateMachine(scheduler_settings)
        {
        }

        void CreateAndInitializeOutputSurfaceWithActivatedCommit()
        {
            DidCreateAndInitializeOutputSurface();
            output_surface_state_ = OUTPUT_SURFACE_ACTIVE;
        }

        void SetBeginMainFrameState(BeginMainFrameState cs)
        {
            begin_main_frame_state_ = cs;
        }
        BeginMainFrameState BeginMainFrameState() const
        {
            return begin_main_frame_state_;
        }

        ForcedRedrawOnTimeoutState ForcedRedrawState() const
        {
            return forced_redraw_state_;
        }

        void SetBeginImplFrameState(BeginImplFrameState bifs)
        {
            begin_impl_frame_state_ = bifs;
        }

        BeginImplFrameState begin_impl_frame_state() const
        {
            return begin_impl_frame_state_;
        }

        OutputSurfaceState output_surface_state() const
        {
            return output_surface_state_;
        }

        void SetNeedsBeginMainFrameForTest(bool needs_begin_main_frame)
        {
            needs_begin_main_frame_ = needs_begin_main_frame;
        }

        bool NeedsCommit() const { return needs_begin_main_frame_; }

        void SetNeedsAnimateForTest(bool needs_animate)
        {
            needs_animate_ = needs_animate;
        }

        void SetNeedsRedraw(bool needs_redraw) { needs_redraw_ = needs_redraw; }

        void SetNeedsForcedRedrawForTimeout(bool b)
        {
            forced_redraw_state_ = FORCED_REDRAW_STATE_WAITING_FOR_COMMIT;
            active_tree_needs_first_draw_ = true;
        }
        bool NeedsForcedRedrawForTimeout() const
        {
            return forced_redraw_state_ != FORCED_REDRAW_STATE_IDLE;
        }

        void SetActiveTreeNeedsFirstDraw(bool needs_first_draw)
        {
            active_tree_needs_first_draw_ = needs_first_draw;
        }

        bool CanDraw() const { return can_draw_; }
        bool Visible() const { return visible_; }

        bool PendingActivationsShouldBeForced() const
        {
            return SchedulerStateMachine::PendingActivationsShouldBeForced();
        }

        void SetHasPendingTree(bool has_pending_tree)
        {
            has_pending_tree_ = has_pending_tree;
        }

        using SchedulerStateMachine::ProactiveBeginFrameWanted;
        using SchedulerStateMachine::ShouldTriggerBeginImplFrameDeadlineImmediately;
        using SchedulerStateMachine::WillCommit;
    };

    TEST(SchedulerStateMachineTest, BeginFrameNeeded)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        state.SetCanStart();
        state.SetVisible(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
        state.SetBeginMainFrameState(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);

        // Don't request BeginFrames if we are idle.
        state.SetNeedsRedraw(false);
        state.SetNeedsAnimateForTest(false);
        EXPECT_FALSE(state.BeginFrameNeeded());

        // Request BeginFrames if we are ready to draw.
        state.SetVisible(true);
        state.SetNeedsRedraw(true);
        state.SetNeedsAnimateForTest(false);
        EXPECT_TRUE(state.BeginFrameNeeded());

        // Don't background tick for needs_redraw.
        state.SetVisible(false);
        state.SetNeedsRedraw(true);
        state.SetNeedsAnimateForTest(false);
        EXPECT_FALSE(state.BeginFrameNeeded());

        // Proactively request BeginFrames when commit is pending.
        state.SetVisible(true);
        state.SetNeedsRedraw(false);
        state.SetNeedsAnimateForTest(false);
        state.SetNeedsBeginMainFrameForTest(true);
        EXPECT_TRUE(state.BeginFrameNeeded());

        // Don't request BeginFrames when commit is pending if
        // we are currently deferring commits.
        state.SetVisible(true);
        state.SetNeedsRedraw(false);
        state.SetNeedsAnimateForTest(false);
        state.SetNeedsBeginMainFrameForTest(true);
        state.SetDeferCommits(true);
        EXPECT_FALSE(state.BeginFrameNeeded());
    }

    TEST(SchedulerStateMachineTest, TestNextActionBeginsMainFrameIfNeeded)
    {
        SchedulerSettings default_scheduler_settings;

        // If no commit needed, do nothing.
        {
            StateMachine state(default_scheduler_settings);
            state.SetCanStart();
            state.SetVisible(true);
            EXPECT_ACTION_UPDATE_STATE(
                SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
            state.SetBeginMainFrameState(
                SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
            state.SetNeedsRedraw(false);

            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            EXPECT_FALSE(state.NeedsCommit());

            state.OnBeginImplFrame();
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

            state.OnBeginImplFrameDeadline();
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            EXPECT_FALSE(state.NeedsCommit());
        }

        // If commit requested but can_start is still false, do nothing.
        {
            StateMachine state(default_scheduler_settings);
            state.SetBeginMainFrameState(
                SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
            state.SetNeedsRedraw(false);
            state.SetVisible(true);
            state.SetNeedsBeginMainFrame();

            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            EXPECT_TRUE(state.NeedsCommit());

            state.OnBeginImplFrame();
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

            state.OnBeginImplFrameDeadline();
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            EXPECT_TRUE(state.NeedsCommit());
        }

        // If commit requested, begin a main frame.
        {
            StateMachine state(default_scheduler_settings);
            state.SetBeginMainFrameState(
                SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
            state.SetCanStart();
            state.SetVisible(true);
            EXPECT_ACTION_UPDATE_STATE(
                SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
            state.SetNeedsRedraw(false);
            state.SetNeedsBeginMainFrame();

            // Expect nothing to happen until after OnBeginImplFrame.
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
            EXPECT_IMPL_FRAME_STATE(SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_IDLE);
            EXPECT_TRUE(state.NeedsCommit());
            EXPECT_TRUE(state.BeginFrameNeeded());

            state.OnBeginImplFrame();
            EXPECT_ACTION_UPDATE_STATE(
                SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
            EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);
            EXPECT_FALSE(state.NeedsCommit());
        }

        // If commit requested and can't draw, still begin a main frame.
        {
            StateMachine state(default_scheduler_settings);
            state.SetBeginMainFrameState(
                SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
            state.SetCanStart();
            state.SetVisible(true);
            EXPECT_ACTION_UPDATE_STATE(
                SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
            state.SetNeedsRedraw(false);
            state.SetNeedsBeginMainFrame();
            state.SetCanDraw(false);

            // Expect nothing to happen until after OnBeginImplFrame.
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
            EXPECT_IMPL_FRAME_STATE(SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_IDLE);
            EXPECT_TRUE(state.BeginFrameNeeded());

            state.OnBeginImplFrame();
            EXPECT_ACTION_UPDATE_STATE(
                SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
            EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);
            EXPECT_FALSE(state.NeedsCommit());
        }
    }

    // Explicitly test main_frame_before_activation_enabled = true
    TEST(SchedulerStateMachineTest, MainFrameBeforeActivationEnabled)
    {
        SchedulerSettings scheduler_settings;
        scheduler_settings.main_frame_before_activation_enabled = true;
        StateMachine state(scheduler_settings);
        state.SetBeginMainFrameState(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
        SET_UP_STATE(state)
        state.SetNeedsRedraw(false);
        state.SetNeedsBeginMainFrame();

        EXPECT_TRUE(state.BeginFrameNeeded());

        // Commit to the pending tree.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);

        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Verify that the next commit starts while there is still a pending tree.
        state.SetNeedsBeginMainFrame();
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Verify the pending commit doesn't overwrite the pending
        // tree until the pending tree has been activated.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Verify NotifyReadyToActivate unblocks activation, commit, and
        // draw in that order.
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        EXPECT_TRUE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
    }

    TEST(SchedulerStateMachineTest,
        TestFailedDrawForAnimationCheckerboardSetsNeedsCommitAndDoesNotDrawAgain)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)
        state.SetNeedsRedraw(true);
        EXPECT_TRUE(state.RedrawPending());
        EXPECT_TRUE(state.BeginFrameNeeded());
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();

        // We're drawing now.
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        EXPECT_FALSE(state.RedrawPending());
        EXPECT_FALSE(state.CommitPending());

        // Failing the draw makes us require a commit.
        state.DidDrawIfPossibleCompleted(DRAW_ABORTED_CHECKERBOARD_ANIMATIONS);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_TRUE(state.RedrawPending());
        EXPECT_TRUE(state.CommitPending());
    }

    TEST(SchedulerStateMachineTest, TestFailedDrawForMissingHighResNeedsCommit)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)
        state.SetNeedsRedraw(true);
        EXPECT_TRUE(state.RedrawPending());
        EXPECT_TRUE(state.BeginFrameNeeded());

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_FALSE(state.RedrawPending());
        EXPECT_FALSE(state.CommitPending());

        // Missing high res content requires a commit (but not a redraw)
        state.DidDrawIfPossibleCompleted(DRAW_ABORTED_MISSING_HIGH_RES_CONTENT);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_FALSE(state.RedrawPending());
        EXPECT_TRUE(state.CommitPending());
    }

    TEST(SchedulerStateMachineTest,
        TestsetNeedsRedrawDuringFailedDrawDoesNotRemoveNeedsRedraw)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)
        state.SetNeedsRedraw(true);
        EXPECT_TRUE(state.RedrawPending());
        EXPECT_TRUE(state.BeginFrameNeeded());
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();

        // We're drawing now.
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_FALSE(state.RedrawPending());
        EXPECT_FALSE(state.CommitPending());

        // While still in the same BeginMainFrame callback on the main thread,
        // set needs redraw again. This should not redraw.
        state.SetNeedsRedraw(true);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Failing the draw for animation checkerboards makes us require a commit.
        state.DidDrawIfPossibleCompleted(DRAW_ABORTED_CHECKERBOARD_ANIMATIONS);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_TRUE(state.RedrawPending());
    }

    TEST(SchedulerStateMachineTest,
        TestFailedDrawsEventuallyForceDrawAfterNextCommit)
    {
        SchedulerSettings scheduler_settings;
        scheduler_settings.maximum_number_of_failed_draws_before_draw_is_forced = 1;
        StateMachine state(scheduler_settings);
        SET_UP_STATE(state)

        // Start a commit.
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.CommitPending());

        // Then initiate a draw.
        state.SetNeedsRedraw(true);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);

        // Fail the draw.
        state.DidDrawIfPossibleCompleted(DRAW_ABORTED_CHECKERBOARD_ANIMATIONS);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.BeginFrameNeeded());
        EXPECT_TRUE(state.RedrawPending());
        // But the commit is ongoing.
        EXPECT_TRUE(state.CommitPending());

        // Finish the commit. Note, we should not yet be forcing a draw, but should
        // continue the commit as usual.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.RedrawPending());

        // Activate so we're ready for a new main frame.
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.RedrawPending());

        // The redraw should be forced at the end of the next BeginImplFrame.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_FORCED);
        state.DidSwapBuffers();
        state.DidSwapBuffersComplete();
    }

    TEST(SchedulerStateMachineTest, TestFailedDrawsDoNotRestartForcedDraw)
    {
        SchedulerSettings scheduler_settings;
        int draw_limit = 1;
        scheduler_settings.maximum_number_of_failed_draws_before_draw_is_forced = draw_limit;
        StateMachine state(scheduler_settings);
        SET_UP_STATE(state)

        // Start a commit.
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.CommitPending());

        // Then initiate a draw.
        state.SetNeedsRedraw(true);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);

        // Fail the draw enough times to force a redraw,
        // then once more for good measure.
        for (int i = 0; i < draw_limit + 1; ++i)
            state.DidDrawIfPossibleCompleted(DRAW_ABORTED_CHECKERBOARD_ANIMATIONS);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.BeginFrameNeeded());
        EXPECT_TRUE(state.RedrawPending());
        // But the commit is ongoing.
        EXPECT_TRUE(state.CommitPending());
        EXPECT_TRUE(state.ForcedRedrawState() == SchedulerStateMachine::FORCED_REDRAW_STATE_WAITING_FOR_COMMIT);

        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.RedrawPending());
        EXPECT_FALSE(state.CommitPending());

        // Now force redraw should be in waiting for activation
        EXPECT_TRUE(state.ForcedRedrawState() == SchedulerStateMachine::FORCED_REDRAW_STATE_WAITING_FOR_ACTIVATION);

        // After failing additional draws, we should still be in a forced
        // redraw, but not back in WAITING_FOR_COMMIT.
        for (int i = 0; i < draw_limit + 1; ++i)
            state.DidDrawIfPossibleCompleted(DRAW_ABORTED_CHECKERBOARD_ANIMATIONS);
        EXPECT_TRUE(state.RedrawPending());
        EXPECT_TRUE(state.ForcedRedrawState() == SchedulerStateMachine::FORCED_REDRAW_STATE_WAITING_FOR_ACTIVATION);
    }

    TEST(SchedulerStateMachineTest, TestFailedDrawIsRetriedInNextBeginImplFrame)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // Start a draw.
        state.SetNeedsRedraw(true);
        EXPECT_TRUE(state.BeginFrameNeeded());
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_TRUE(state.RedrawPending());
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);

        // Failing the draw for animation checkerboards makes us require a commit.
        state.DidDrawIfPossibleCompleted(DRAW_ABORTED_CHECKERBOARD_ANIMATIONS);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.RedrawPending());

        // We should not be trying to draw again now, but we have a commit pending.
        EXPECT_TRUE(state.BeginFrameNeeded());
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // We should try to draw again at the end of the next BeginImplFrame on
        // the impl thread.
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest, TestDoestDrawTwiceInSameFrame)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)
        state.SetNeedsRedraw(true);

        // Draw the first frame.
        EXPECT_TRUE(state.BeginFrameNeeded());
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidDrawIfPossibleCompleted(DRAW_SUCCESS);
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Before the next BeginImplFrame, set needs redraw again.
        // This should not redraw until the next BeginImplFrame.
        state.SetNeedsRedraw(true);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Move to another frame. This should now draw.
        EXPECT_TRUE(state.BeginFrameNeeded());
        state.OnBeginImplFrame();

        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidDrawIfPossibleCompleted(DRAW_SUCCESS);
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // We just swapped, so we should proactively request another BeginImplFrame.
        EXPECT_TRUE(state.BeginFrameNeeded());
    }

    TEST(SchedulerStateMachineTest, TestNextActionDrawsOnBeginImplFrame)
    {
        SchedulerSettings default_scheduler_settings;

        // When not in BeginImplFrame deadline, or in BeginImplFrame deadline
        // but not visible, don't draw.
        size_t num_begin_main_frame_states = sizeof(begin_main_frame_states) / sizeof(SchedulerStateMachine::BeginMainFrameState);
        size_t num_begin_impl_frame_states = sizeof(all_begin_impl_frame_states) / sizeof(SchedulerStateMachine::BeginImplFrameState);
        for (size_t i = 0; i < num_begin_main_frame_states; ++i) {
            for (size_t j = 0; j < num_begin_impl_frame_states; ++j) {
                StateMachine state(default_scheduler_settings);
                state.SetCanStart();
                state.SetVisible(true);
                EXPECT_ACTION_UPDATE_STATE(
                    SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
                EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
                state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
                state.SetBeginMainFrameState(begin_main_frame_states[i]);
                state.SetBeginImplFrameState(all_begin_impl_frame_states[j]);
                bool visible = (all_begin_impl_frame_states[j] != SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE);
                state.SetVisible(visible);

                // Case 1: needs_begin_main_frame=false
                EXPECT_NE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE,
                    state.NextAction());

                // Case 2: needs_begin_main_frame=true
                state.SetNeedsBeginMainFrame();
                EXPECT_NE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE,
                    state.NextAction())
                    << state.AsValue()->ToString();
            }
        }

        // When in BeginImplFrame deadline we should always draw for SetNeedsRedraw
        // except if we're ready to commit, in which case we expect a commit first.
        for (size_t i = 0; i < num_begin_main_frame_states; ++i) {
            StateMachine state(default_scheduler_settings);
            state.SetCanStart();
            state.SetVisible(true);
            EXPECT_ACTION_UPDATE_STATE(
                SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
            EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
            state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
            state.SetCanDraw(true);
            state.SetBeginMainFrameState(begin_main_frame_states[i]);
            state.SetBeginImplFrameState(
                SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE);

            state.SetNeedsRedraw(true);

            SchedulerStateMachine::Action expected_action;
            if (begin_main_frame_states[i] == SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_READY_TO_COMMIT) {
                expected_action = SchedulerStateMachine::ACTION_COMMIT;
            } else {
                expected_action = SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE;
                EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
            }

            // Case 1: needs_begin_main_frame=false.
            EXPECT_ACTION(expected_action);

            // Case 2: needs_begin_main_frame=true.
            state.SetNeedsBeginMainFrame();
            EXPECT_ACTION(expected_action);
        }
    }

    TEST(SchedulerStateMachineTest, TestNoBeginMainFrameStatesRedrawWhenInvisible)
    {
        SchedulerSettings default_scheduler_settings;

        size_t num_begin_main_frame_states = sizeof(begin_main_frame_states) / sizeof(SchedulerStateMachine::BeginMainFrameState);
        for (size_t i = 0; i < num_begin_main_frame_states; ++i) {
            // There shouldn't be any drawing regardless of BeginImplFrame.
            for (size_t j = 0; j < 2; ++j) {
                StateMachine state(default_scheduler_settings);
                state.SetCanStart();
                state.SetVisible(true);
                EXPECT_ACTION_UPDATE_STATE(
                    SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
                EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
                state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
                state.SetBeginMainFrameState(begin_main_frame_states[i]);
                state.SetVisible(false);
                state.SetNeedsRedraw(true);
                if (j == 1) {
                    state.SetBeginImplFrameState(
                        SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE);
                }

                // Case 1: needs_begin_main_frame=false.
                EXPECT_NE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE,
                    state.NextAction());

                // Case 2: needs_begin_main_frame=true.
                state.SetNeedsBeginMainFrame();
                EXPECT_NE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE,
                    state.NextAction())
                    << state.AsValue()->ToString();
            }
        }
    }

    TEST(SchedulerStateMachineTest, TestCanRedraw_StopsDraw)
    {
        SchedulerSettings default_scheduler_settings;

        size_t num_begin_main_frame_states = sizeof(begin_main_frame_states) / sizeof(SchedulerStateMachine::BeginMainFrameState);
        for (size_t i = 0; i < num_begin_main_frame_states; ++i) {
            // There shouldn't be any drawing regardless of BeginImplFrame.
            for (size_t j = 0; j < 2; ++j) {
                StateMachine state(default_scheduler_settings);
                state.SetCanStart();
                state.SetVisible(true);
                EXPECT_ACTION_UPDATE_STATE(
                    SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
                EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
                state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
                state.SetBeginMainFrameState(begin_main_frame_states[i]);
                state.SetVisible(false);
                state.SetNeedsRedraw(true);
                if (j == 1)
                    state.OnBeginImplFrame();

                state.SetCanDraw(false);
                EXPECT_NE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE,
                    state.NextAction());
            }
        }
    }

    TEST(SchedulerStateMachineTest,
        TestCanRedrawWithWaitingForFirstDrawMakesProgress)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        state.SetCanStart();
        state.SetVisible(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();

        state.SetActiveTreeNeedsFirstDraw(true);
        state.SetNeedsBeginMainFrame();
        state.SetNeedsRedraw(true);
        state.SetCanDraw(false);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_ABORT);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_ABORT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest, TestSetNeedsBeginMainFrameIsNotLost)
    {
        SchedulerSettings scheduler_settings;
        StateMachine state(scheduler_settings);
        SET_UP_STATE(state)
        state.SetNeedsBeginMainFrame();

        EXPECT_TRUE(state.BeginFrameNeeded());

        // Begin the frame.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);

        // Now, while the frame is in progress, set another commit.
        state.SetNeedsBeginMainFrame();
        EXPECT_TRUE(state.NeedsCommit());

        // Let the frame finish.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_MAIN_FRAME_STATE(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_READY_TO_COMMIT);

        // Expect to commit regardless of BeginImplFrame state.
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_BEGIN_FRAME_STARTING);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_COMMIT);

        state.OnBeginImplFrameDeadlinePending();
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_BEGIN_FRAME);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_COMMIT);

        state.OnBeginImplFrameDeadline();
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_COMMIT);

        state.OnBeginImplFrameIdle();
        EXPECT_IMPL_FRAME_STATE(SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_IDLE);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_COMMIT);

        state.OnBeginImplFrame();
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_BEGIN_FRAME_STARTING);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_COMMIT);

        // Finish the commit and activate, then make sure we start the next commit
        // immediately and draw on the next BeginImplFrame.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameDeadline();

        EXPECT_TRUE(state.active_tree_needs_first_draw());
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidDrawIfPossibleCompleted(DRAW_SUCCESS);
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest, TestFullCycle)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // Start clean and set commit.
        state.SetNeedsBeginMainFrame();

        // Begin the frame.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);
        EXPECT_FALSE(state.NeedsCommit());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Tell the scheduler the frame finished.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_MAIN_FRAME_STATE(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_READY_TO_COMMIT);

        // Commit.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);

        // Activate.
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_TRUE(state.active_tree_needs_first_draw());
        EXPECT_TRUE(state.needs_redraw());

        // Expect to do nothing until BeginImplFrame deadline
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // At BeginImplFrame deadline, draw.
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidDrawIfPossibleCompleted(DRAW_SUCCESS);
        state.DidSwapBuffersComplete();

        // Should be synchronized, no draw needed, no action needed.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
        EXPECT_FALSE(state.needs_redraw());
    }

    TEST(SchedulerStateMachineTest, CommitWithoutDrawWithPendingTree)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // Start clean and set commit.
        state.SetNeedsBeginMainFrame();

        // Make a main frame, commit and activate it. But don't draw it.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);

        // Try to make a new main frame before drawing. Since we will commit it to a
        // pending tree and not clobber the active tree, we're able to start a new
        // begin frame and commit it.
        state.SetNeedsBeginMainFrame();
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
    }

    TEST(SchedulerStateMachineTest, DontCommitWithoutDrawWithoutPendingTree)
    {
        SchedulerSettings scheduler_settings;
        scheduler_settings.commit_to_active_tree = true;
        StateMachine state(scheduler_settings);
        SET_UP_STATE(state)

        // Start clean and set commit.
        state.SetNeedsBeginMainFrame();

        // Make a main frame, commit and activate it. But don't draw it.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);

        // Try to make a new main frame before drawing, but since we would clobber the
        // active tree, we will not do so.
        state.SetNeedsBeginMainFrame();
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest, TestFullCycleWithCommitToActive)
    {
        SchedulerSettings scheduler_settings;
        scheduler_settings.commit_to_active_tree = true;
        StateMachine state(scheduler_settings);
        SET_UP_STATE(state)

        // Start clean and set commit.
        state.SetNeedsBeginMainFrame();

        // Begin the frame.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);
        EXPECT_FALSE(state.NeedsCommit());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Tell the scheduler the frame finished.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_MAIN_FRAME_STATE(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_READY_TO_COMMIT);

        // Commit.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);

        // Now commit should wait for activation.
        EXPECT_MAIN_FRAME_STATE(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_WAITING_FOR_ACTIVATION);

        // No activation yet, so this commit is not drawn yet. Force to draw this
        // frame, and still block BeginMainFrame.
        state.SetNeedsRedraw(true);
        state.SetNeedsBeginMainFrame();
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Cannot BeginMainFrame yet since last commit is not yet activated and drawn.
        state.OnBeginImplFrame();
        EXPECT_MAIN_FRAME_STATE(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_WAITING_FOR_ACTIVATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Now activate sync tree.
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.active_tree_needs_first_draw());
        EXPECT_TRUE(state.needs_redraw());
        EXPECT_MAIN_FRAME_STATE(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_WAITING_FOR_DRAW);

        // Swap throttled. Do not draw.
        state.DidSwapBuffers();
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.DidSwapBuffersComplete();

        // Haven't draw since last commit, do not begin new main frame.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // At BeginImplFrame deadline, draw. This draws unblocks BeginMainFrame.
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidDrawIfPossibleCompleted(DRAW_SUCCESS);
        state.DidSwapBuffersComplete();

        // Now will be able to start main frame.
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
        EXPECT_FALSE(state.needs_redraw());
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
    }

    TEST(SchedulerStateMachineTest, TestFullCycleWithCommitRequestInbetween)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // Start clean and set commit.
        state.SetNeedsBeginMainFrame();

        // Begin the frame.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);
        EXPECT_FALSE(state.NeedsCommit());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Request another commit while the commit is in flight.
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Tell the scheduler the frame finished.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_MAIN_FRAME_STATE(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_READY_TO_COMMIT);

        // First commit and activate.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_TRUE(state.active_tree_needs_first_draw());
        EXPECT_TRUE(state.needs_redraw());

        // Expect to do nothing until BeginImplFrame deadline.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // At BeginImplFrame deadline, draw.
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidDrawIfPossibleCompleted(DRAW_SUCCESS);
        state.DidSwapBuffersComplete();

        // Should be synchronized, no draw needed, no action needed.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
        EXPECT_FALSE(state.needs_redraw());

        // Next BeginImplFrame should initiate second commit.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
    }

    TEST(SchedulerStateMachineTest, TestNoRequestCommitWhenInvisible)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        state.SetCanStart();
        state.SetVisible(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
        state.SetVisible(false);
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest, TestNoRequestOutputSurfaceWhenInvisible)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        state.SetCanStart();
        // We should not request an OutputSurface when we are still invisible.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.SetVisible(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
        state.SetVisible(false);
        state.DidLoseOutputSurface();
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.SetVisible(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
    }

    // See ThreadProxy::BeginMainFrame "EarlyOut_NotVisible" /
    // "EarlyOut_OutputSurfaceLost" cases.
    TEST(SchedulerStateMachineTest, TestAbortBeginMainFrameBecauseInvisible)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // Start clean and set commit.
        state.SetNeedsBeginMainFrame();

        // Begin the frame while visible.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);
        EXPECT_FALSE(state.NeedsCommit());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Become invisible and abort BeginMainFrame.
        state.SetVisible(false);
        state.BeginMainFrameAborted(CommitEarlyOutReason::ABORTED_NOT_VISIBLE);

        // NeedsCommit should now be true again because we never actually did a
        // commit.
        EXPECT_TRUE(state.NeedsCommit());

        // We should now be back in the idle state as if we never started the frame.
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // We shouldn't do anything on the BeginImplFrame deadline.
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Become visible again.
        state.SetVisible(true);

        // Although we have aborted on this frame and haven't cancelled the commit
        // (i.e. need another), don't send another BeginMainFrame yet.
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.NeedsCommit());

        // Start a new frame.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);

        // We should be starting the commit now.
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
    }

    // See ThreadProxy::BeginMainFrame "EarlyOut_NoUpdates" case.
    TEST(SchedulerStateMachineTest, TestAbortBeginMainFrameBecauseCommitNotNeeded)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        state.SetCanStart();
        state.SetVisible(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.DidCreateAndInitializeOutputSurface();
        state.SetCanDraw(true);

        // Get into a begin frame / commit state.
        state.SetNeedsBeginMainFrame();
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);
        EXPECT_FALSE(state.NeedsCommit());
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);

        // Abort the commit, true means that the BeginMainFrame was sent but there
        // was no work to do on the main thread.
        state.BeginMainFrameAborted(CommitEarlyOutReason::FINISHED_NO_UPDATES);

        // NeedsCommit should now be false because the commit was actually handled.
        EXPECT_FALSE(state.NeedsCommit());

        // Since the commit was aborted, we don't need to try and draw.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Verify another commit doesn't start on another frame either.
        EXPECT_FALSE(state.NeedsCommit());
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Verify another commit can start if requested, though.
        state.SetNeedsBeginMainFrame();
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_IDLE);
        state.OnBeginImplFrame();
        EXPECT_ACTION(SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
    }

    TEST(SchedulerStateMachineTest, TestFirstContextCreation)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        state.SetCanStart();
        state.SetVisible(true);
        state.SetCanDraw(true);

        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Check that the first init does not SetNeedsBeginMainFrame.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Check that a needs commit initiates a BeginMainFrame.
        state.SetNeedsBeginMainFrame();
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
    }

    TEST(SchedulerStateMachineTest, TestContextLostWhenCompletelyIdle)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        EXPECT_NE(SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION,
            state.NextAction());
        state.DidLoseOutputSurface();

        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Once context recreation begins, nothing should happen.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Recreate the context.
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();

        // When the context is recreated, we should begin a commit.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
    }

    TEST(SchedulerStateMachineTest,
        TestContextLostWhenIdleAndCommitRequestedWhileRecreating)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        EXPECT_NE(SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION,
            state.NextAction());
        state.DidLoseOutputSurface();
        EXPECT_EQ(state.output_surface_state(),
            SchedulerStateMachine::OUTPUT_SURFACE_LOST);

        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Once context recreation begins, nothing should happen.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // While context is recreating, commits shouldn't begin.
        state.SetNeedsBeginMainFrame();
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Recreate the context
        state.DidCreateAndInitializeOutputSurface();
        EXPECT_EQ(state.output_surface_state(),
            SchedulerStateMachine::OUTPUT_SURFACE_WAITING_FOR_FIRST_COMMIT);
        EXPECT_FALSE(state.RedrawPending());

        // When the context is recreated, we wait until the next BeginImplFrame
        // before starting.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // When the BeginFrame comes in we should begin a commit
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_MAIN_FRAME_STATE(SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);

        // Until that commit finishes, we shouldn't be drawing or animate.
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Finish the commit, which should make the surface active.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        EXPECT_EQ(state.output_surface_state(),
            SchedulerStateMachine::OUTPUT_SURFACE_WAITING_FOR_FIRST_ACTIVATION);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_EQ(state.output_surface_state(),
            SchedulerStateMachine::OUTPUT_SURFACE_ACTIVE);

        // Finishing the first commit after initializing an output surface should
        // automatically cause a redraw.
        EXPECT_TRUE(state.RedrawPending());
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_FALSE(state.RedrawPending());

        // Next frame as no work to do.
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Once the context is recreated, whether we draw should be based on
        // SetCanDraw if waiting on first draw after activate.
        state.SetNeedsRedraw(true);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.SetCanDraw(false);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);
        state.SetCanDraw(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Once the context is recreated, whether we draw should be based on
        // SetCanDraw if waiting on first draw after activate.
        state.SetNeedsRedraw(true);
        state.SetNeedsBeginMainFrame();
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        // Activate so we need the first draw
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.active_tree_needs_first_draw());
        EXPECT_TRUE(state.needs_redraw());

        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.SetCanDraw(false);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_ABORT);
        state.SetCanDraw(true);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
    }

    TEST(SchedulerStateMachineTest, TestContextLostWhileCommitInProgress)
    {
        SchedulerSettings scheduler_settings;
        StateMachine state(scheduler_settings);
        SET_UP_STATE(state)

        // Get a commit in flight.
        state.SetNeedsBeginMainFrame();

        // Set damage and expect a draw.
        state.SetNeedsRedraw(true);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Cause a lost context while the BeginMainFrame is in flight.
        state.DidLoseOutputSurface();

        // Ask for another draw. Expect nothing happens.
        state.SetNeedsRedraw(true);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);

        // Finish the frame, commit and activate.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);

        // We will abort the draw when the output surface is lost if we are
        // waiting for the first draw to unblock the main thread.
        EXPECT_TRUE(state.active_tree_needs_first_draw());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_ABORT);

        // Expect to begin context recreation only in BEGIN_IMPL_FRAME_STATE_IDLE
        EXPECT_IMPL_FRAME_STATE(SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_IDLE);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);

        state.OnBeginImplFrame();
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_BEGIN_FRAME_STARTING);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameDeadlinePending();
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_BEGIN_FRAME);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameDeadline();
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest,
        TestContextLostWhileCommitInProgressAndAnotherCommitRequested)
    {
        SchedulerSettings scheduler_settings;
        StateMachine state(scheduler_settings);
        SET_UP_STATE(state)

        // Get a commit in flight.
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Set damage and expect a draw.
        state.SetNeedsRedraw(true);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Cause a lost context while the BeginMainFrame is in flight.
        state.DidLoseOutputSurface();

        // Ask for another draw and also set needs commit. Expect nothing happens.
        state.SetNeedsRedraw(true);
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Finish the frame, and commit and activate.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_TRUE(state.active_tree_needs_first_draw());

        // Because the output surface is missing, we expect the draw to abort.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_ABORT);

        // Expect to begin context recreation only in BEGIN_IMPL_FRAME_STATE_IDLE
        EXPECT_IMPL_FRAME_STATE(SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_IDLE);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);

        state.OnBeginImplFrame();
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_BEGIN_FRAME_STARTING);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameDeadlinePending();
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_BEGIN_FRAME);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameDeadline();
        EXPECT_IMPL_FRAME_STATE(
            SchedulerStateMachine::BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameIdle();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);

        // After we get a new output surface, the commit flow should start.
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest, DontDrawBeforeCommitAfterLostOutputSurface)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        state.SetNeedsRedraw(true);

        // Cause a lost output surface, and restore it.
        state.DidLoseOutputSurface();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.DidCreateAndInitializeOutputSurface();

        EXPECT_FALSE(state.RedrawPending());
        state.OnBeginImplFrame();
        EXPECT_ACTION(SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
    }

    TEST(SchedulerStateMachineTest,
        TestPendingActivationsShouldBeForcedAfterLostOutputSurface)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        state.SetBeginMainFrameState(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);

        // Cause a lost context.
        state.DidLoseOutputSurface();

        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);

        EXPECT_TRUE(state.PendingActivationsShouldBeForced());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);

        EXPECT_TRUE(state.PendingDrawsShouldBeAborted());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_ABORT);
    }

    TEST(SchedulerStateMachineTest, TestNoBeginFrameNeededWhenInvisible)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        state.SetCanStart();
        state.SetVisible(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();

        EXPECT_FALSE(state.BeginFrameNeeded());
        state.SetNeedsRedraw(true);
        EXPECT_TRUE(state.BeginFrameNeeded());

        state.SetVisible(false);
        EXPECT_FALSE(state.BeginFrameNeeded());

        state.SetVisible(true);
        EXPECT_TRUE(state.BeginFrameNeeded());
    }

    TEST(SchedulerStateMachineTest, TestNoBeginMainFrameWhenInvisible)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        state.SetCanStart();
        state.SetVisible(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
        state.SetVisible(false);
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);
        EXPECT_FALSE(state.BeginFrameNeeded());

        // When become visible again, the needs commit should still be pending.
        state.SetVisible(true);
        EXPECT_TRUE(state.BeginFrameNeeded());
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
    }

    TEST(SchedulerStateMachineTest, TestFinishCommitWhenCommitInProgress)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        state.SetCanStart();
        state.SetVisible(true);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.CreateAndInitializeOutputSurfaceWithActivatedCommit();
        state.SetVisible(false);
        state.SetBeginMainFrameState(
            SchedulerStateMachine::BEGIN_MAIN_FRAME_STATE_SENT);
        state.SetNeedsBeginMainFrame();

        // After the commit completes, activation and draw happen immediately
        // because we are not visible.
        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_TRUE(state.active_tree_needs_first_draw());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_DRAW_AND_SWAP_ABORT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest, TestInitialActionsWhenContextLost)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)
        state.SetNeedsBeginMainFrame();
        state.DidLoseOutputSurface();

        // When we are visible, we normally want to begin output surface creation
        // as soon as possible.
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_BEGIN_OUTPUT_SURFACE_CREATION);

        state.DidCreateAndInitializeOutputSurface();
        EXPECT_EQ(state.output_surface_state(),
            SchedulerStateMachine::OUTPUT_SURFACE_WAITING_FOR_FIRST_COMMIT);

        // We should not send a BeginMainFrame when we are invisible, even if we've
        // lost the output surface and are trying to get the first commit, since the
        // main thread will just abort anyway.
        state.SetVisible(false);
        EXPECT_ACTION(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest, ReportIfNotDrawing)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)
        EXPECT_FALSE(state.PendingDrawsShouldBeAborted());

        state.SetCanDraw(false);
        state.SetVisible(true);
        EXPECT_TRUE(state.PendingDrawsShouldBeAborted());

        state.SetCanDraw(true);
        state.SetVisible(false);
        EXPECT_TRUE(state.PendingDrawsShouldBeAborted());

        state.SetCanDraw(false);
        state.SetVisible(false);
        EXPECT_TRUE(state.PendingDrawsShouldBeAborted());

        state.SetCanDraw(true);
        state.SetVisible(true);
        EXPECT_FALSE(state.PendingDrawsShouldBeAborted());
    }

    TEST(SchedulerStateMachineTest, ForceDrawForSynchronousCompositor)
    {
        SchedulerSettings scheduler_settings;
        scheduler_settings.using_synchronous_renderer_compositor = true;
        StateMachine state(scheduler_settings);
        SET_UP_STATE(state)
        EXPECT_FALSE(state.PendingDrawsShouldBeAborted());

        state.SetVisible(false);
        state.DidLoseOutputSurface();

        state.SetCanDraw(false);
        state.WillBeginOutputSurfaceCreation();
        state.DidCreateAndInitializeOutputSurface();
        EXPECT_TRUE(state.PendingDrawsShouldBeAborted());

        state.SetCanDraw(true);
        state.DidLoseOutputSurface();
        EXPECT_TRUE(state.PendingDrawsShouldBeAborted());

        state.SetCanDraw(true);
        state.WillBeginOutputSurfaceCreation();
        state.DidCreateAndInitializeOutputSurface();
        EXPECT_FALSE(state.PendingDrawsShouldBeAborted());

        state.SetCanDraw(false);
        state.DidLoseOutputSurface();
        EXPECT_TRUE(state.PendingDrawsShouldBeAborted());
    }

    TEST(SchedulerStateMachineTest,
        TestTriggerDeadlineImmediatelyAfterAbortedCommit)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // This test mirrors what happens during the first frame of a scroll gesture.
        // First we get the input event and a BeginFrame.
        state.OnBeginImplFrame();

        // As a response the compositor requests a redraw and a commit to tell the
        // main thread about the new scroll offset.
        state.SetNeedsRedraw(true);
        state.SetNeedsBeginMainFrame();

        // We should start the commit normally.
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Since only the scroll offset changed, the main thread will abort the
        // commit.
        state.BeginMainFrameAborted(CommitEarlyOutReason::FINISHED_NO_UPDATES);

        // Since the commit was aborted, we should draw right away instead of waiting
        // for the deadline.
        EXPECT_TRUE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());
    }

    void FinishPreviousCommitAndDrawWithoutExitingDeadline(
        StateMachine* state_ptr)
    {
        // Gross, but allows us to use macros below.
        StateMachine& state = *state_ptr;

        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.NotifyReadyToActivate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ACTIVATE_SYNC_TREE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        EXPECT_TRUE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
    }

    TEST(SchedulerStateMachineTest, TestImplLatencyTakesPriority)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // This test ensures that impl-draws are prioritized over main thread updates
        // in prefer impl latency mode.
        state.SetNeedsRedraw(true);
        state.SetNeedsBeginMainFrame();
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Verify the deadline is not triggered early until we enter
        // prefer impl latency mode.
        EXPECT_FALSE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());
        state.SetImplLatencyTakesPriority(true);
        EXPECT_TRUE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());

        // Trigger the deadline.
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
        state.DidSwapBuffers();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.DidSwapBuffersComplete();

        // Request a new commit and finish the previous one.
        state.SetNeedsBeginMainFrame();
        FinishPreviousCommitAndDrawWithoutExitingDeadline(&state);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.DidSwapBuffersComplete();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Finish the previous commit and draw it.
        FinishPreviousCommitAndDrawWithoutExitingDeadline(&state);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        // Verify we do not send another BeginMainFrame if was are swap throttled
        // and did not just swap.
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_FALSE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
    }

    TEST(SchedulerStateMachineTest,
        TestTriggerDeadlineImmediatelyOnLostOutputSurface)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        state.SetNeedsBeginMainFrame();

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_FALSE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());

        state.DidLoseOutputSurface();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        // The deadline should be triggered immediately when output surface is lost.
        EXPECT_TRUE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());
    }

    TEST(SchedulerStateMachineTest, TestTriggerDeadlineImmediatelyWhenInvisible)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        state.SetNeedsBeginMainFrame();

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_FALSE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());

        state.SetVisible(false);
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.ShouldTriggerBeginImplFrameDeadlineImmediately());
    }

    TEST(SchedulerStateMachineTest, TestSetNeedsAnimate)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // Test requesting an animation that, when run, causes us to draw.
        state.SetNeedsAnimate();
        EXPECT_TRUE(state.BeginFrameNeeded());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);

        state.OnBeginImplFrameDeadlinePending();
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
    }

    TEST(SchedulerStateMachineTest, TestAnimateBeforeCommit)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // Check that animations are updated before we start a commit.
        state.SetNeedsAnimate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.BeginFrameNeeded());

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);

        state.OnBeginImplFrameDeadlinePending();
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
    }

    TEST(SchedulerStateMachineTest, TestAnimateAfterCommitBeforeDraw)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // Check that animations are updated before we start a commit.
        state.SetNeedsAnimate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        state.SetNeedsBeginMainFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);
        EXPECT_TRUE(state.BeginFrameNeeded());

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);

        state.NotifyBeginMainFrameStarted();
        state.NotifyReadyToCommit();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_COMMIT);

        state.OnBeginImplFrameDeadlinePending();
        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
    }

    TEST(SchedulerStateMachineTest, TestSetNeedsAnimateAfterAnimate)
    {
        SchedulerSettings default_scheduler_settings;
        StateMachine state(default_scheduler_settings);
        SET_UP_STATE(state)

        // Test requesting an animation after we have already animated during this
        // frame.
        state.SetNeedsRedraw(true);
        EXPECT_TRUE(state.BeginFrameNeeded());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_ANIMATE);

        state.SetNeedsAnimate();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_DRAW_AND_SWAP_IF_POSSIBLE);
    }

    TEST(SchedulerStateMachineTest, TestForwardBeginFramesToChildren)
    {
        SchedulerSettings settings;
        StateMachine state(settings);
        SET_UP_STATE(state)

        EXPECT_FALSE(state.BeginFrameNeeded());
        state.SetChildrenNeedBeginFrames(true);
        EXPECT_TRUE(state.BeginFrameNeeded());
    }

    TEST(SchedulerStateMachineTest, TestDeferCommit)
    {
        SchedulerSettings settings;
        StateMachine state(settings);
        SET_UP_STATE(state)

        state.SetDeferCommits(true);

        state.SetNeedsBeginMainFrame();
        EXPECT_FALSE(state.BeginFrameNeeded());
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.OnBeginImplFrameDeadline();
        EXPECT_ACTION_UPDATE_STATE(SchedulerStateMachine::ACTION_NONE);

        state.SetDeferCommits(false);
        state.OnBeginImplFrame();
        EXPECT_ACTION_UPDATE_STATE(
            SchedulerStateMachine::ACTION_SEND_BEGIN_MAIN_FRAME);
    }

    TEST(SchedulerStateMachineTest, EarlyOutCommitWantsProactiveBeginFrame)
    {
        SchedulerSettings settings;
        StateMachine state(settings);
        SET_UP_STATE(state);

        EXPECT_FALSE(state.ProactiveBeginFrameWanted());
        bool commit_has_no_updates = true;
        state.WillCommit(commit_has_no_updates);
        EXPECT_TRUE(state.ProactiveBeginFrameWanted());
        state.OnBeginImplFrame();
        EXPECT_FALSE(state.ProactiveBeginFrameWanted());
    }

} // namespace
} // namespace cc
