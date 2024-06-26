// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SCHEDULER_SCHEDULER_H_
#define CC_SCHEDULER_SCHEDULER_H_

#include <deque>
#include <string>

#include "base/basictypes.h"
#include "base/cancelable_callback.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/base/cc_export.h"
#include "cc/output/begin_frame_args.h"
#include "cc/scheduler/begin_frame_source.h"
#include "cc/scheduler/begin_frame_tracker.h"
#include "cc/scheduler/delay_based_time_source.h"
#include "cc/scheduler/draw_result.h"
#include "cc/scheduler/scheduler_settings.h"
#include "cc/scheduler/scheduler_state_machine.h"

namespace base {
namespace trace_event {
    class ConvertableToTraceFormat;
}
class SingleThreadTaskRunner;
}

namespace cc {

class CompositorTimingHistory;

class SchedulerClient {
public:
    virtual void WillBeginImplFrame(const BeginFrameArgs& args) = 0;
    virtual void ScheduledActionSendBeginMainFrame() = 0;
    virtual DrawResult ScheduledActionDrawAndSwapIfPossible() = 0;
    virtual DrawResult ScheduledActionDrawAndSwapForced() = 0;
    virtual void ScheduledActionAnimate() = 0;
    virtual void ScheduledActionCommit() = 0;
    virtual void ScheduledActionActivateSyncTree() = 0;
    virtual void ScheduledActionBeginOutputSurfaceCreation() = 0;
    virtual void ScheduledActionPrepareTiles() = 0;
    virtual void ScheduledActionInvalidateOutputSurface() = 0;
    virtual void DidFinishImplFrame() = 0;
    virtual void SendBeginFramesToChildren(const BeginFrameArgs& args) = 0;
    virtual void SendBeginMainFrameNotExpectedSoon() = 0;

protected:
    virtual ~SchedulerClient() { }
};

class CC_EXPORT Scheduler : public BeginFrameObserverBase {
public:
    static scoped_ptr<Scheduler> Create(
        SchedulerClient* client,
        const SchedulerSettings& scheduler_settings,
        int layer_tree_host_id,
        base::SingleThreadTaskRunner* task_runner,
        BeginFrameSource* external_frame_source,
        scoped_ptr<CompositorTimingHistory> compositor_timing_history);

    ~Scheduler() override;

    // BeginFrameObserverBase
    bool OnBeginFrameDerivedImpl(const BeginFrameArgs& args) override;

    void OnDrawForOutputSurface();

    const SchedulerSettings& settings() const { return settings_; }

    void CommitVSyncParameters(base::TimeTicks timebase,
        base::TimeDelta interval);
    void SetEstimatedParentDrawTime(base::TimeDelta draw_time);

    void SetCanStart();

    void SetVisible(bool visible);
    void SetCanDraw(bool can_draw);
    void NotifyReadyToActivate();
    void NotifyReadyToDraw();
    void SetThrottleFrameProduction(bool throttle);

    void SetNeedsBeginMainFrame();

    void SetNeedsRedraw();

    void SetNeedsAnimate();

    void SetNeedsPrepareTiles();

    void SetMaxSwapsPending(int max);
    void DidSwapBuffers();
    void DidSwapBuffersComplete();

    void SetImplLatencyTakesPriority(bool impl_latency_takes_priority);

    void NotifyReadyToCommit();
    void BeginMainFrameAborted(CommitEarlyOutReason reason);
    void DidCommit();

    void WillPrepareTiles();
    void DidPrepareTiles();
    void DidLoseOutputSurface();
    void DidCreateAndInitializeOutputSurface();

    // Tests do not want to shut down until all possible BeginMainFrames have
    // occured to prevent flakiness.
    bool MainFrameForTestingWillHappen() const
    {
        return state_machine_.CommitPending() || state_machine_.CouldSendBeginMainFrame();
    }

    bool CommitPending() const { return state_machine_.CommitPending(); }
    bool RedrawPending() const { return state_machine_.RedrawPending(); }
    bool PrepareTilesPending() const
    {
        return state_machine_.PrepareTilesPending();
    }
    bool BeginImplFrameDeadlinePending() const
    {
        return !begin_impl_frame_deadline_task_.IsCancelled();
    }
    bool ImplLatencyTakesPriority() const
    {
        return state_machine_.impl_latency_takes_priority();
    }

    void NotifyBeginMainFrameStarted();

    base::TimeTicks LastBeginImplFrameTime();

    void SetDeferCommits(bool defer_commits);

    scoped_refptr<base::trace_event::ConvertableToTraceFormat> AsValue() const;
    void AsValueInto(base::trace_event::TracedValue* value) const override;

    void SetChildrenNeedBeginFrames(bool children_need_begin_frames);
    void SetVideoNeedsBeginFrames(bool video_needs_begin_frames);

    void SetAuthoritativeVSyncInterval(const base::TimeDelta& interval);

protected:
    Scheduler(SchedulerClient* client,
        const SchedulerSettings& scheduler_settings,
        int layer_tree_host_id,
        base::SingleThreadTaskRunner* task_runner,
        BeginFrameSource* external_frame_source,
        scoped_ptr<SyntheticBeginFrameSource> synthetic_frame_source,
        scoped_ptr<BackToBackBeginFrameSource> unthrottled_frame_source,
        scoped_ptr<CompositorTimingHistory> compositor_timing_history);

    // Virtual for testing.
    virtual base::TimeTicks Now() const;

    const SchedulerSettings settings_;
    SchedulerClient* client_;
    int layer_tree_host_id_;
    base::SingleThreadTaskRunner* task_runner_;
    BeginFrameSource* external_frame_source_;
    scoped_ptr<SyntheticBeginFrameSource> synthetic_frame_source_;
    scoped_ptr<BackToBackBeginFrameSource> unthrottled_frame_source_;

    scoped_ptr<BeginFrameSourceMultiplexer> frame_source_;
    bool throttle_frame_production_;

    base::TimeDelta authoritative_vsync_interval_;
    base::TimeTicks last_vsync_timebase_;

    scoped_ptr<CompositorTimingHistory> compositor_timing_history_;
    base::TimeDelta estimated_parent_draw_time_;

    std::deque<BeginFrameArgs> begin_retro_frame_args_;
    SchedulerStateMachine::BeginImplFrameDeadlineMode
        begin_impl_frame_deadline_mode_;
    BeginFrameTracker begin_impl_frame_tracker_;

    base::Closure begin_retro_frame_closure_;
    base::Closure begin_impl_frame_deadline_closure_;
    base::CancelableClosure begin_retro_frame_task_;
    base::CancelableClosure begin_impl_frame_deadline_task_;

    SchedulerStateMachine state_machine_;
    bool inside_process_scheduled_actions_;
    SchedulerStateMachine::Action inside_action_;

private:
    void ScheduleBeginImplFrameDeadline();
    void ScheduleBeginImplFrameDeadlineIfNeeded();
    void SetupNextBeginFrameIfNeeded();
    void PostBeginRetroFrameIfNeeded();
    void DrawAndSwapIfPossible();
    void DrawAndSwapForced();
    void ProcessScheduledActions();
    void UpdateCompositorTimingHistoryRecordingEnabled();
    bool ShouldRecoverMainLatency(const BeginFrameArgs& args) const;
    bool ShouldRecoverImplLatency(const BeginFrameArgs& args) const;
    bool CanCommitAndActivateBeforeDeadline(const BeginFrameArgs& args) const;
    void AdvanceCommitStateIfPossible();
    bool IsBeginMainFrameSentOrStarted() const;
    void BeginRetroFrame();
    void BeginImplFrameWithDeadline(const BeginFrameArgs& args);
    void BeginImplFrameSynchronous(const BeginFrameArgs& args);
    void BeginImplFrame(const BeginFrameArgs& args);
    void FinishImplFrame();
    void OnBeginImplFrameDeadline();
    void PollToAdvanceCommitState();

    base::TimeDelta EstimatedParentDrawTime()
    {
        return estimated_parent_draw_time_;
    }

    bool IsInsideAction(SchedulerStateMachine::Action action)
    {
        return inside_action_ == action;
    }

    BeginFrameSource* primary_frame_source()
    {
        if (settings_.use_external_begin_frame_source) {
            DCHECK(external_frame_source_);
            return external_frame_source_;
        }
        return synthetic_frame_source_.get();
    }

    base::WeakPtrFactory<Scheduler> weak_factory_;

    DISALLOW_COPY_AND_ASSIGN(Scheduler);
};

} // namespace cc

#endif // CC_SCHEDULER_SCHEDULER_H_
