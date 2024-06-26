// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SCHEDULER_SCHEDULER_STATE_MACHINE_H_
#define CC_SCHEDULER_SCHEDULER_STATE_MACHINE_H_

#include <string>

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/output/begin_frame_args.h"
#include "cc/scheduler/commit_earlyout_reason.h"
#include "cc/scheduler/draw_result.h"
#include "cc/scheduler/scheduler_settings.h"

namespace base {
namespace trace_event {
    class ConvertableToTraceFormat;
    class TracedValue;
}
class Value;
}

namespace cc {

// The SchedulerStateMachine decides how to coordinate main thread activites
// like painting/running javascript with rendering and input activities on the
// impl thread.
//
// The state machine tracks internal state but is also influenced by external
// state.  Internal state includes things like whether a frame has been
// requested, while external state includes things like the current time being
// near to the vblank time.
//
// The scheduler seperates "what to do next" from the updating of its internal
// state to make testing cleaner.
class CC_EXPORT SchedulerStateMachine {
public:
    // settings must be valid for the lifetime of this class.
    explicit SchedulerStateMachine(const SchedulerSettings& settings);

    enum OutputSurfaceState {
        OUTPUT_SURFACE_ACTIVE,
        OUTPUT_SURFACE_LOST,
        OUTPUT_SURFACE_CREATING,
        OUTPUT_SURFACE_WAITING_FOR_FIRST_COMMIT,
        OUTPUT_SURFACE_WAITING_FOR_FIRST_ACTIVATION,
    };
    static const char* OutputSurfaceStateToString(OutputSurfaceState state);

    // Note: BeginImplFrameState does not cycle through these states in a fixed
    // order on all platforms. It's up to the scheduler to set these correctly.
    // TODO(sunnyps): Rename the states to IDLE, ANIMATE, WAITING_FOR_DEADLINE and
    // DRAW.
    enum BeginImplFrameState {
        BEGIN_IMPL_FRAME_STATE_IDLE,
        BEGIN_IMPL_FRAME_STATE_BEGIN_FRAME_STARTING,
        BEGIN_IMPL_FRAME_STATE_INSIDE_BEGIN_FRAME,
        BEGIN_IMPL_FRAME_STATE_INSIDE_DEADLINE,
    };
    static const char* BeginImplFrameStateToString(BeginImplFrameState state);

    enum BeginImplFrameDeadlineMode {
        BEGIN_IMPL_FRAME_DEADLINE_MODE_NONE,
        BEGIN_IMPL_FRAME_DEADLINE_MODE_IMMEDIATE,
        BEGIN_IMPL_FRAME_DEADLINE_MODE_REGULAR,
        BEGIN_IMPL_FRAME_DEADLINE_MODE_LATE,
        BEGIN_IMPL_FRAME_DEADLINE_MODE_BLOCKED_ON_READY_TO_DRAW,
    };
    static const char* BeginImplFrameDeadlineModeToString(
        BeginImplFrameDeadlineMode mode);

    enum BeginMainFrameState {
        BEGIN_MAIN_FRAME_STATE_IDLE,
        BEGIN_MAIN_FRAME_STATE_SENT,
        BEGIN_MAIN_FRAME_STATE_STARTED,
        BEGIN_MAIN_FRAME_STATE_READY_TO_COMMIT,
        BEGIN_MAIN_FRAME_STATE_WAITING_FOR_ACTIVATION,
        BEGIN_MAIN_FRAME_STATE_WAITING_FOR_DRAW,
    };
    static const char* BeginMainFrameStateToString(BeginMainFrameState state);

    enum ForcedRedrawOnTimeoutState {
        FORCED_REDRAW_STATE_IDLE,
        FORCED_REDRAW_STATE_WAITING_FOR_COMMIT,
        FORCED_REDRAW_STATE_WAITING_FOR_ACTIVATION,
        FORCED_REDRAW_STATE_WAITING_FOR_DRAW,
    };
    static const char* ForcedRedrawOnTimeoutStateToString(
        ForcedRedrawOnTimeoutState state);

    bool CommitPending() const
    {
        return begin_main_frame_state_ == BEGIN_MAIN_FRAME_STATE_SENT || begin_main_frame_state_ == BEGIN_MAIN_FRAME_STATE_STARTED || begin_main_frame_state_ == BEGIN_MAIN_FRAME_STATE_READY_TO_COMMIT;
    }
    BeginMainFrameState begin_main_frame_state() const
    {
        return begin_main_frame_state_;
    }

    bool RedrawPending() const { return needs_redraw_; }
    bool PrepareTilesPending() const { return needs_prepare_tiles_; }

    enum Action {
        ACTION_NONE,
        ACTION_ANIMATE,
        ACTION_SEND_BEGIN_MAIN_FRAME,
        ACTION_COMMIT,
        ACTION_ACTIVATE_SYNC_TREE,
        ACTION_DRAW_AND_SWAP_IF_POSSIBLE,
        ACTION_DRAW_AND_SWAP_FORCED,
        ACTION_DRAW_AND_SWAP_ABORT,
        ACTION_BEGIN_OUTPUT_SURFACE_CREATION,
        ACTION_PREPARE_TILES,
        ACTION_INVALIDATE_OUTPUT_SURFACE,
    };
    static const char* ActionToString(Action action);

    scoped_refptr<base::trace_event::ConvertableToTraceFormat> AsValue() const;
    void AsValueInto(base::trace_event::TracedValue* dict) const;

    Action NextAction() const;
    void WillAnimate();
    void WillSendBeginMainFrame();
    void WillCommit(bool commit_had_no_updates);
    void WillActivate();
    void WillDraw(bool did_request_swap);
    void WillBeginOutputSurfaceCreation();
    void WillPrepareTiles();
    void WillInvalidateOutputSurface();

    // Indicates whether the impl thread needs a BeginImplFrame callback in order
    // to make progress.
    bool BeginFrameNeeded() const;

    // Indicates that the system has entered and left a BeginImplFrame callback.
    // The scheduler will not draw more than once in a given BeginImplFrame
    // callback nor send more than one BeginMainFrame message.
    void OnBeginImplFrame();
    void OnBeginImplFrameDeadlinePending();
    // Indicates that the scheduler has entered the draw phase. The scheduler
    // will not draw more than once in a single draw phase.
    // TODO(sunnyps): Rename OnBeginImplFrameDeadline to OnDraw or similar.
    void OnBeginImplFrameDeadline();
    void OnBeginImplFrameIdle();
    BeginImplFrameState begin_impl_frame_state() const
    {
        return begin_impl_frame_state_;
    }
    BeginImplFrameDeadlineMode CurrentBeginImplFrameDeadlineMode() const;

    // If the main thread didn't manage to produce a new frame in time for the
    // impl thread to draw, it is in a high latency mode.
    bool main_thread_missed_last_deadline() const;

    bool SwapThrottled() const;

    // Indicates whether the LayerTreeHostImpl is visible.
    void SetVisible(bool visible);
    bool visible() const { return visible_; }

    // Indicates that a redraw is required, either due to the impl tree changing
    // or the screen being damaged and simply needing redisplay.
    void SetNeedsRedraw();
    bool needs_redraw() const { return needs_redraw_; }

    void SetNeedsAnimate();
    bool needs_animate() const { return needs_animate_; }

    bool OnlyImplSideUpdatesExpected() const;

    // Indicates that prepare-tiles is required. This guarantees another
    // PrepareTiles will occur shortly (even if no redraw is required).
    void SetNeedsPrepareTiles();

    // Sets how many swaps can be pending to the OutputSurface.
    void SetMaxSwapsPending(int max);

    // If the scheduler attempted to draw and swap, this provides feedback
    // regarding whether or not the swap actually occured. We might skip the
    // swap when there is not damage, for example.
    void DidSwapBuffers();

    // Indicates whether a redraw is required because we are currently rendering
    // with a low resolution or checkerboarded tile.
    void SetSwapUsedIncompleteTile(bool used_incomplete_tile);

    // Notification from the OutputSurface that a swap has been consumed.
    void DidSwapBuffersComplete();

    int pending_swaps() const { return pending_swaps_; }

    // Indicates whether to prioritize impl thread latency (i.e., animation
    // smoothness) over new content activation.
    void SetImplLatencyTakesPriority(bool impl_latency_takes_priority);
    bool impl_latency_takes_priority() const
    {
        return impl_latency_takes_priority_;
    }

    // Indicates whether ACTION_DRAW_AND_SWAP_IF_POSSIBLE drew to the screen.
    void DidDrawIfPossibleCompleted(DrawResult result);

    // Indicates that a new begin main frame flow needs to be performed, either
    // to pull updates from the main thread to the impl, or to push deltas from
    // the impl thread to main.
    void SetNeedsBeginMainFrame();
    bool needs_begin_main_frame() const { return needs_begin_main_frame_; }

    // Call this only in response to receiving an ACTION_SEND_BEGIN_MAIN_FRAME
    // from NextAction.
    // Indicates that all painting is complete.
    void NotifyReadyToCommit();

    // Call this only in response to receiving an ACTION_SEND_BEGIN_MAIN_FRAME
    // from NextAction if the client rejects the BeginMainFrame message.
    void BeginMainFrameAborted(CommitEarlyOutReason reason);

    // Set that we can create the first OutputSurface and start the scheduler.
    void SetCanStart() { can_start_ = true; }
    // Allow access of the can_start_ state in tests.
    bool CanStartForTesting() const { return can_start_; }

    // Indicates production should be skipped to recover latency.
    void SetSkipNextBeginMainFrameToReduceLatency();

    // Indicates whether drawing would, at this time, make sense.
    // CanDraw can be used to suppress flashes or checkerboarding
    // when such behavior would be undesirable.
    void SetCanDraw(bool can);

    // Indicates that scheduled BeginMainFrame is started.
    void NotifyBeginMainFrameStarted();

    // Indicates that the pending tree is ready for activation.
    void NotifyReadyToActivate();

    // Indicates the active tree's visible tiles are ready to be drawn.
    void NotifyReadyToDraw();

    bool has_pending_tree() const { return has_pending_tree_; }
    bool active_tree_needs_first_draw() const
    {
        return active_tree_needs_first_draw_;
    }

    void DidPrepareTiles();
    void DidLoseOutputSurface();
    void DidCreateAndInitializeOutputSurface();
    bool HasInitializedOutputSurface() const;

    // True if we need to abort draws to make forward progress.
    bool PendingDrawsShouldBeAborted() const;

    bool CouldSendBeginMainFrame() const;

    void SetDeferCommits(bool defer_commits);

    void SetChildrenNeedBeginFrames(bool children_need_begin_frames);
    bool children_need_begin_frames() const
    {
        return children_need_begin_frames_;
    }

    void SetVideoNeedsBeginFrames(bool video_needs_begin_frames);
    bool video_needs_begin_frames() const { return video_needs_begin_frames_; }

protected:
    bool BeginFrameRequiredForAction() const;
    bool BeginFrameRequiredForChildren() const;
    bool BeginFrameNeededForVideo() const;
    bool ProactiveBeginFrameWanted() const;

    bool ShouldTriggerBeginImplFrameDeadlineImmediately() const;

    // True if we need to force activations to make forward progress.
    // TODO(sunnyps): Rename this to ShouldAbortCurrentFrame or similar.
    bool PendingActivationsShouldBeForced() const;

    // TODO(brianderson): Remove this once NPAPI support is removed.
    bool SendingBeginMainFrameMightCauseDeadlock() const;

    bool ShouldAnimate() const;
    bool ShouldBeginOutputSurfaceCreation() const;
    bool ShouldDraw() const;
    bool ShouldActivatePendingTree() const;
    bool ShouldSendBeginMainFrame() const;
    bool ShouldCommit() const;
    bool ShouldPrepareTiles() const;
    bool ShouldInvalidateOutputSurface() const;

    const SchedulerSettings settings_;

    OutputSurfaceState output_surface_state_;
    BeginImplFrameState begin_impl_frame_state_;
    BeginMainFrameState begin_main_frame_state_;
    ForcedRedrawOnTimeoutState forced_redraw_state_;

    // These are used for tracing only.
    int commit_count_;
    int current_frame_number_;
    int last_frame_number_animate_performed_;
    int last_frame_number_swap_performed_;
    int last_frame_number_swap_requested_;
    int last_frame_number_begin_main_frame_sent_;
    int last_frame_number_invalidate_output_surface_performed_;

    // These are used to ensure that an action only happens once per frame,
    // deadline, etc.
    bool animate_funnel_;
    bool request_swap_funnel_;
    bool send_begin_main_frame_funnel_;
    bool invalidate_output_surface_funnel_;
    // prepare_tiles_funnel_ is "filled" each time PrepareTiles is called
    // and "drained" on each BeginImplFrame. If the funnel gets too full,
    // we start throttling ACTION_PREPARE_TILES such that we average one
    // PrepareTiles per BeginImplFrame.
    int prepare_tiles_funnel_;

    int consecutive_checkerboard_animations_;
    int max_pending_swaps_;
    int pending_swaps_;
    int swaps_with_current_output_surface_;
    bool needs_redraw_;
    bool needs_animate_;
    bool needs_prepare_tiles_;
    bool needs_begin_main_frame_;
    bool visible_;
    bool can_start_;
    bool can_draw_;
    bool has_pending_tree_;
    bool pending_tree_is_ready_for_activation_;
    bool active_tree_needs_first_draw_;
    bool did_create_and_initialize_first_output_surface_;
    bool impl_latency_takes_priority_;
    bool main_thread_missed_last_deadline_;
    bool skip_next_begin_main_frame_to_reduce_latency_;
    bool children_need_begin_frames_;
    bool defer_commits_;
    bool video_needs_begin_frames_;
    bool last_commit_had_no_updates_;
    bool wait_for_ready_to_draw_;
    bool did_request_swap_in_last_frame_;
    bool did_perform_swap_in_last_draw_;

private:
    DISALLOW_COPY_AND_ASSIGN(SchedulerStateMachine);
};

} // namespace cc

#endif // CC_SCHEDULER_SCHEDULER_STATE_MACHINE_H_
