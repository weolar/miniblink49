// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_THREAD_PROXY_H_
#define CC_TREES_THREAD_PROXY_H_

#include <string>

#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "cc/animation/animation_events.h"
#include "cc/base/completion_event.h"
#include "cc/base/delayed_unique_notifier.h"
#include "cc/scheduler/commit_earlyout_reason.h"
#include "cc/scheduler/scheduler.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/proxy.h"
#include "cc/trees/threaded_channel.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace cc {

class BeginFrameSource;
class ChannelImpl;
class ChannelMain;
class ContextProvider;
class InputHandlerClient;
class LayerTreeHost;
class ProxyImpl;
class ProxyMain;
class Scheduler;
class ScopedThreadProxy;
class ThreadedChannel;

class CC_EXPORT ThreadProxy : public Proxy,
                              public ProxyMain,
                              public ProxyImpl,
                              NON_EXPORTED_BASE(LayerTreeHostImplClient),
                              NON_EXPORTED_BASE(SchedulerClient) {
public:
    static scoped_ptr<Proxy> Create(
        LayerTreeHost* layer_tree_host,
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
        scoped_ptr<BeginFrameSource> external_begin_frame_source);

    ~ThreadProxy() override;

    struct BeginMainFrameAndCommitState {
        BeginMainFrameAndCommitState();
        ~BeginMainFrameAndCommitState();

        unsigned int begin_frame_id;
        BeginFrameArgs begin_frame_args;
        scoped_ptr<ScrollAndScaleSet> scroll_info;
        size_t memory_allocation_limit_bytes;
        bool evicted_ui_resources;
    };

    // Commits between the main and impl threads are processed through a pipeline
    // with the following stages. For efficiency we can early out at any stage if
    // we decide that no further processing is necessary.
    enum CommitPipelineStage {
        NO_PIPELINE_STAGE,
        ANIMATE_PIPELINE_STAGE,
        UPDATE_LAYERS_PIPELINE_STAGE,
        COMMIT_PIPELINE_STAGE,
    };

    struct MainThreadOnly {
        MainThreadOnly(ThreadProxy* proxy, int layer_tree_host_id);
        ~MainThreadOnly();

        const int layer_tree_host_id;

        // The furthest pipeline stage which has been requested for the next
        // commit.
        CommitPipelineStage max_requested_pipeline_stage;
        // The commit pipeline stage that is currently being processed.
        CommitPipelineStage current_pipeline_stage;
        // The commit pipeline stage at which processing for the current commit
        // will stop. Only valid while we are executing the pipeline (i.e.,
        // |current_pipeline_stage| is set to a pipeline stage).
        CommitPipelineStage final_pipeline_stage;

        bool started;
        bool prepare_tiles_pending;
        bool defer_commits;

        RendererCapabilities renderer_capabilities_main_thread_copy;

        // TODO(khushalsagar): Make this scoped_ptr<ChannelMain> when ProxyMain
        // and ProxyImpl are split.
        ChannelMain* channel_main;

        base::WeakPtrFactory<ThreadProxy> weak_factory;
    };

    // Accessed on the main thread, or when main thread is blocked.
    struct MainThreadOrBlockedMainThread {
        explicit MainThreadOrBlockedMainThread(LayerTreeHost* host);
        ~MainThreadOrBlockedMainThread();

        LayerTreeHost* layer_tree_host;
        bool commit_waits_for_activation;
        bool main_thread_inside_commit;
    };

    struct CompositorThreadOnly {
        CompositorThreadOnly(
            ThreadProxy* proxy,
            int layer_tree_host_id,
            RenderingStatsInstrumentation* rendering_stats_instrumentation,
            scoped_ptr<BeginFrameSource> external_begin_frame_source);
        ~CompositorThreadOnly();

        const int layer_tree_host_id;

        scoped_ptr<Scheduler> scheduler;

        // Set when the main thread is waiting on a
        // ScheduledActionSendBeginMainFrame to be issued.
        CompletionEvent* begin_main_frame_sent_completion_event;

        // Set when the main thread is waiting on a commit to complete.
        CompletionEvent* commit_completion_event;

        // Set when the main thread is waiting on a pending tree activation.
        CompletionEvent* completion_event_for_commit_held_on_tree_activation;

        // Set when the next draw should post DidCommitAndDrawFrame to the main
        // thread.
        bool next_frame_is_newly_committed_frame;

        bool inside_draw;

        bool input_throttled_until_commit;

        // Whether a commit has been completed since the last time animations were
        // ticked. If this happens, we need to animate again.
        bool did_commit_after_animating;

        DelayedUniqueNotifier smoothness_priority_expiration_notifier;

        scoped_ptr<BeginFrameSource> external_begin_frame_source;

        RenderingStatsInstrumentation* rendering_stats_instrumentation;

        // Values used to keep track of frame durations. Used only in frame timing.
        BeginFrameArgs last_begin_main_frame_args;
        BeginFrameArgs last_processed_begin_main_frame_args;

        scoped_ptr<LayerTreeHostImpl> layer_tree_host_impl;

        ChannelImpl* channel_impl;

        base::WeakPtrFactory<ThreadProxy> weak_factory;
    };

    const MainThreadOnly& main() const;
    const MainThreadOrBlockedMainThread& blocked_main() const;
    const CompositorThreadOnly& impl() const;

    // Proxy implementation
    void FinishAllRendering() override;
    bool IsStarted() const override;
    bool CommitToActiveTree() const override;
    void SetOutputSurface(OutputSurface* output_surface) override;
    void SetLayerTreeHostClientReady() override;
    void SetVisible(bool visible) override;
    void SetThrottleFrameProduction(bool throttle) override;
    const RendererCapabilities& GetRendererCapabilities() const override;
    void SetNeedsAnimate() override;
    void SetNeedsUpdateLayers() override;
    void SetNeedsCommit() override;
    void SetNeedsRedraw(const gfx::Rect& damage_rect) override;
    void SetNextCommitWaitsForActivation() override;
    void NotifyInputThrottledUntilCommit() override;
    void SetDeferCommits(bool defer_commits) override;
    bool CommitRequested() const override;
    bool BeginMainFrameRequested() const override;
    void MainThreadHasStoppedFlinging() override;
    void Start() override;
    void Stop() override;
    bool SupportsImplScrolling() const override;
    bool MainFrameWillHappenForTesting() override;
    void SetChildrenNeedBeginFrames(bool children_need_begin_frames) override;
    void SetAuthoritativeVSyncInterval(const base::TimeDelta& interval) override;
    void ReleaseOutputSurface() override;

    // LayerTreeHostImplClient implementation
    void UpdateRendererCapabilitiesOnImplThread() override;
    void DidLoseOutputSurfaceOnImplThread() override;
    void CommitVSyncParameters(base::TimeTicks timebase,
        base::TimeDelta interval) override;
    void SetEstimatedParentDrawTime(base::TimeDelta draw_time) override;
    void SetMaxSwapsPendingOnImplThread(int max) override;
    void DidSwapBuffersOnImplThread() override;
    void DidSwapBuffersCompleteOnImplThread() override;
    void OnCanDrawStateChanged(bool can_draw) override;
    void NotifyReadyToActivate() override;
    void NotifyReadyToDraw() override;
    // Please call these 3 functions through
    // LayerTreeHostImpl's SetNeedsRedraw(), SetNeedsRedrawRect() and
    // SetNeedsAnimate().
    void SetNeedsRedrawOnImplThread() override;
    void SetNeedsRedrawRectOnImplThread(const gfx::Rect& dirty_rect) override;
    void SetNeedsAnimateOnImplThread() override;
    void SetNeedsPrepareTilesOnImplThread() override;
    void SetNeedsCommitOnImplThread() override;
    void SetVideoNeedsBeginFrames(bool needs_begin_frames) override;
    void PostAnimationEventsToMainThreadOnImplThread(
        scoped_ptr<AnimationEventsVector> queue) override;
    bool IsInsideDraw() override;
    void RenewTreePriority() override;
    void PostDelayedAnimationTaskOnImplThread(const base::Closure& task,
        base::TimeDelta delay) override;
    void DidActivateSyncTree() override;
    void WillPrepareTiles() override;
    void DidPrepareTiles() override;
    void DidCompletePageScaleAnimationOnImplThread() override;
    void OnDrawForOutputSurface() override;
    // This should only be called by LayerTreeHostImpl::PostFrameTimingEvents.
    void PostFrameTimingEventsOnImplThread(
        scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
        scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events)
        override;

    // SchedulerClient implementation
    void WillBeginImplFrame(const BeginFrameArgs& args) override;
    void DidFinishImplFrame() override;
    void ScheduledActionSendBeginMainFrame() override;
    DrawResult ScheduledActionDrawAndSwapIfPossible() override;
    DrawResult ScheduledActionDrawAndSwapForced() override;
    void ScheduledActionAnimate() override;
    void ScheduledActionCommit() override;
    void ScheduledActionActivateSyncTree() override;
    void ScheduledActionBeginOutputSurfaceCreation() override;
    void ScheduledActionPrepareTiles() override;
    void ScheduledActionInvalidateOutputSurface() override;
    void SendBeginFramesToChildren(const BeginFrameArgs& args) override;
    void SendBeginMainFrameNotExpectedSoon() override;

    // ProxyMain implementation
    base::WeakPtr<ProxyMain> GetMainWeakPtr() override;
    void SetChannel(scoped_ptr<ThreadedChannel> threaded_channel) override;
    void DidCompleteSwapBuffers() override;

    // ProxyImpl implementation
    base::WeakPtr<ProxyImpl> GetImplWeakPtr() override;
    void SetThrottleFrameProductionOnImpl(bool throttle) override;
    void SetLayerTreeHostClientReadyOnImpl() override;

protected:
    ThreadProxy(
        LayerTreeHost* layer_tree_host,
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
        scoped_ptr<BeginFrameSource> external_begin_frame_source);

private:
    // Called on main thread.
    void SetRendererCapabilitiesMainThreadCopy(
        const RendererCapabilities& capabilities);
    void BeginMainFrame(
        scoped_ptr<BeginMainFrameAndCommitState> begin_main_frame_state);
    void BeginMainFrameNotExpectedSoon();
    void DidCommitAndDrawFrame();
    void SetAnimationEvents(scoped_ptr<AnimationEventsVector> queue);
    void DidLoseOutputSurface();
    void RequestNewOutputSurface();
    void DidInitializeOutputSurface(bool success,
        const RendererCapabilities& capabilities);
    // Returns |true| if the request was actually sent, |false| if one was
    // already outstanding.
    bool SendCommitRequestToImplThreadIfNeeded(
        CommitPipelineStage required_stage);
    void DidCompletePageScaleAnimation();

    // Called on impl thread.
    struct SchedulerStateRequest;

    void StartCommitOnImplThread(CompletionEvent* completion);
    void BeginMainFrameAbortedOnImplThread(CommitEarlyOutReason reason);
    void FinishAllRenderingOnImplThread(CompletionEvent* completion);
    void InitializeImplOnImplThread(CompletionEvent* completion);
    void SetVisibleOnImplThread(CompletionEvent* completion, bool visible);
    void HasInitializedOutputSurfaceOnImplThread(
        CompletionEvent* completion,
        bool* has_initialized_output_surface);
    void DeleteContentsTexturesOnImplThread(CompletionEvent* completion);
    void InitializeOutputSurfaceOnImplThread(OutputSurface* output_surface);
    void ReleaseOutputSurfaceOnImplThread(CompletionEvent* completion);
    void FinishGLOnImplThread(CompletionEvent* completion);
    void LayerTreeHostClosedOnImplThread(CompletionEvent* completion);
    DrawResult DrawSwapInternal(bool forced_draw);
    void MainFrameWillHappenOnImplThreadForTesting(CompletionEvent* completion,
        bool* main_frame_will_happen);
    void SetSwapUsedIncompleteTileOnImplThread(bool used_incomplete_tile);
    void MainThreadHasStoppedFlingingOnImplThread();
    void SetInputThrottledUntilCommitOnImplThread(bool is_throttled);
    void SetDeferCommitsOnImplThread(bool defer_commits) const;
    void PostFrameTimingEvents(
        scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
        scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events);

    LayerTreeHost* layer_tree_host();
    const LayerTreeHost* layer_tree_host() const;

    // Use accessors instead of this variable directly.
    MainThreadOnly main_thread_only_vars_unsafe_;
    MainThreadOnly& main();

    // Use accessors instead of this variable directly.
    MainThreadOrBlockedMainThread main_thread_or_blocked_vars_unsafe_;
    MainThreadOrBlockedMainThread& blocked_main();

    // Use accessors instead of this variable directly.
    CompositorThreadOnly compositor_thread_vars_unsafe_;
    CompositorThreadOnly& impl();

    // TODO(khushalsagar): Remove this. Temporary variable to hold the channel.
    scoped_ptr<ThreadedChannel> threaded_channel_;

    base::WeakPtr<ThreadProxy> main_thread_weak_ptr_;
    base::WeakPtr<ThreadProxy> impl_thread_weak_ptr_;

    DISALLOW_COPY_AND_ASSIGN(ThreadProxy);
};

} // namespace cc

#endif // CC_TREES_THREAD_PROXY_H_
