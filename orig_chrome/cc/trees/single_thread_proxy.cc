// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/single_thread_proxy.h"

#include "base/auto_reset.h"
#include "base/profiler/scoped_tracker.h"
#include "base/trace_event/trace_event.h"
#include "cc/debug/benchmark_instrumentation.h"
#include "cc/debug/devtools_instrumentation.h"
#include "cc/output/context_provider.h"
#include "cc/output/output_surface.h"
#include "cc/quads/draw_quad.h"
#include "cc/scheduler/commit_earlyout_reason.h"
#include "cc/scheduler/compositor_timing_history.h"
#include "cc/scheduler/scheduler.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_single_thread_client.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/scoped_abort_remaining_swap_promises.h"

namespace cc {

scoped_ptr<Proxy> SingleThreadProxy::Create(
    LayerTreeHost* layer_tree_host,
    LayerTreeHostSingleThreadClient* client,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    scoped_ptr<BeginFrameSource> external_begin_frame_source)
{
    return make_scoped_ptr(new SingleThreadProxy(
        layer_tree_host,
        client,
        main_task_runner,
        external_begin_frame_source.Pass()));
}

SingleThreadProxy::SingleThreadProxy(
    LayerTreeHost* layer_tree_host,
    LayerTreeHostSingleThreadClient* client,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    scoped_ptr<BeginFrameSource> external_begin_frame_source)
    : Proxy(main_task_runner, NULL)
    , layer_tree_host_(layer_tree_host)
    , client_(client)
    , external_begin_frame_source_(external_begin_frame_source.Pass())
    , next_frame_is_newly_committed_frame_(false)
    ,
#if DCHECK_IS_ON()
    inside_impl_frame_(false)
    ,
#endif
    inside_draw_(false)
    , defer_commits_(false)
    , animate_requested_(false)
    , commit_requested_(false)
    , inside_synchronous_composite_(false)
    , output_surface_creation_requested_(false)
    , weak_factory_(this)
{
    TRACE_EVENT0("cc", "SingleThreadProxy::SingleThreadProxy");
    DCHECK(Proxy::IsMainThread());
    DCHECK(layer_tree_host);

    if (layer_tree_host->settings().single_thread_proxy_scheduler && !scheduler_on_impl_thread_) {
        SchedulerSettings scheduler_settings(
            layer_tree_host->settings().ToSchedulerSettings());
        scheduler_settings.commit_to_active_tree = CommitToActiveTree();

        scoped_ptr<CompositorTimingHistory> compositor_timing_history(
            new CompositorTimingHistory(
                CompositorTimingHistory::BROWSER_UMA,
                layer_tree_host->rendering_stats_instrumentation()));

        scheduler_on_impl_thread_ = Scheduler::Create(
            this, scheduler_settings, layer_tree_host_->id(),
            MainThreadTaskRunner(), external_begin_frame_source_.get(),
            compositor_timing_history.Pass());
    }
}

void SingleThreadProxy::Start()
{
    DebugScopedSetImplThread impl(this);
    layer_tree_host_impl_ = layer_tree_host_->CreateLayerTreeHostImpl(this);
}

SingleThreadProxy::~SingleThreadProxy()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::~SingleThreadProxy");
    DCHECK(Proxy::IsMainThread());
    // Make sure Stop() got called or never Started.
    DCHECK(!layer_tree_host_impl_);
}

void SingleThreadProxy::FinishAllRendering()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::FinishAllRendering");
    DCHECK(Proxy::IsMainThread());
    {
        DebugScopedSetImplThread impl(this);
        layer_tree_host_impl_->FinishAllRendering();
    }
}

bool SingleThreadProxy::IsStarted() const
{
    DCHECK(Proxy::IsMainThread());
    return layer_tree_host_impl_;
}

bool SingleThreadProxy::CommitToActiveTree() const
{
    // With SingleThreadProxy we skip the pending tree and commit directly to the
    // active tree.
    return true;
}

void SingleThreadProxy::SetLayerTreeHostClientReady()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::SetLayerTreeHostClientReady");
    // Scheduling is controlled by the embedder in the single thread case, so
    // nothing to do.
    DCHECK(Proxy::IsMainThread());
    DebugScopedSetImplThread impl(this);
    if (scheduler_on_impl_thread_) {
        scheduler_on_impl_thread_->SetCanStart();
        scheduler_on_impl_thread_->SetVisible(layer_tree_host_impl_->visible());
    }
}

void SingleThreadProxy::SetVisible(bool visible)
{
    TRACE_EVENT1("cc", "SingleThreadProxy::SetVisible", "visible", visible);
    DebugScopedSetImplThread impl(this);

    layer_tree_host_impl_->SetVisible(visible);

    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetVisible(layer_tree_host_impl_->visible());
}

void SingleThreadProxy::SetThrottleFrameProduction(bool throttle)
{
    TRACE_EVENT1("cc", "SingleThreadProxy::SetThrottleFrameProduction",
        "throttle", throttle);
    DebugScopedSetImplThread impl(this);
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetThrottleFrameProduction(throttle);
}

void SingleThreadProxy::RequestNewOutputSurface()
{
    DCHECK(Proxy::IsMainThread());
    DCHECK(layer_tree_host_->output_surface_lost());
    output_surface_creation_callback_.Cancel();
    if (output_surface_creation_requested_)
        return;
    output_surface_creation_requested_ = true;
    layer_tree_host_->RequestNewOutputSurface();
}

void SingleThreadProxy::ReleaseOutputSurface()
{
    // |layer_tree_host_| should already be aware of this.
    DCHECK(layer_tree_host_->output_surface_lost());

    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->DidLoseOutputSurface();
    return layer_tree_host_impl_->ReleaseOutputSurface();
}

void SingleThreadProxy::SetOutputSurface(OutputSurface* output_surface)
{
    DCHECK(Proxy::IsMainThread());
    DCHECK(layer_tree_host_->output_surface_lost());
    DCHECK(output_surface_creation_requested_);
    renderer_capabilities_for_main_thread_ = RendererCapabilities();

    bool success;
    {
        DebugScopedSetMainThreadBlocked main_thread_blocked(this);
        DebugScopedSetImplThread impl(this);
        success = layer_tree_host_impl_->InitializeRenderer(output_surface);
    }

    if (success) {
        layer_tree_host_->DidInitializeOutputSurface();
        if (scheduler_on_impl_thread_)
            scheduler_on_impl_thread_->DidCreateAndInitializeOutputSurface();
        else if (!inside_synchronous_composite_)
            SetNeedsCommit();
        output_surface_creation_requested_ = false;
    } else {
        // DidFailToInitializeOutputSurface is treated as a RequestNewOutputSurface,
        // and so output_surface_creation_requested remains true.
        layer_tree_host_->DidFailToInitializeOutputSurface();
    }
}

const RendererCapabilities& SingleThreadProxy::GetRendererCapabilities() const
{
    DCHECK(Proxy::IsMainThread());
    DCHECK(!layer_tree_host_->output_surface_lost());
    return renderer_capabilities_for_main_thread_;
}

void SingleThreadProxy::SetNeedsAnimate()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::SetNeedsAnimate");
    DCHECK(Proxy::IsMainThread());
    client_->ScheduleAnimation();
    if (animate_requested_)
        return;
    animate_requested_ = true;
    DebugScopedSetImplThread impl(this);
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetNeedsBeginMainFrame();
}

void SingleThreadProxy::SetNeedsUpdateLayers()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::SetNeedsUpdateLayers");
    DCHECK(Proxy::IsMainThread());
    SetNeedsCommit();
}

void SingleThreadProxy::DoCommit()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::DoCommit");
    DCHECK(Proxy::IsMainThread());

    // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509 is
    // fixed.
    tracked_objects::ScopedTracker tracking_profile1(
        FROM_HERE_WITH_EXPLICIT_FUNCTION("461509 SingleThreadProxy::DoCommit1"));
    layer_tree_host_->WillCommit();
    devtools_instrumentation::ScopedCommitTrace commit_task(
        layer_tree_host_->id());

    // Commit immediately.
    {
        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile2(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoCommit2"));
        DebugScopedSetMainThreadBlocked main_thread_blocked(this);
        DebugScopedSetImplThread impl(this);

        // This CapturePostTasks should be destroyed before CommitComplete() is
        // called since that goes out to the embedder, and we want the embedder
        // to receive its callbacks before that.
        commit_blocking_task_runner_.reset(new BlockingTaskRunner::CapturePostTasks(
            blocking_main_thread_task_runner()));

        layer_tree_host_impl_->BeginCommit();

        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile6(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoCommit6"));
        if (layer_tree_host_impl_->EvictedUIResourcesExist())
            layer_tree_host_->RecreateUIResources();

        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile7(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoCommit7"));
        layer_tree_host_->FinishCommitOnImplThread(layer_tree_host_impl_.get());

#if DCHECK_IS_ON()
        // In the single-threaded case, the scale and scroll deltas should never be
        // touched on the impl layer tree.
        scoped_ptr<ScrollAndScaleSet> scroll_info = layer_tree_host_impl_->ProcessScrollDeltas();
        DCHECK(!scroll_info->scrolls.size());
        DCHECK_EQ(1.f, scroll_info->page_scale_delta);
#endif

        if (scheduler_on_impl_thread_)
            scheduler_on_impl_thread_->DidCommit();

        layer_tree_host_impl_->CommitComplete();

        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile8(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoCommit8"));
        // Commit goes directly to the active tree, but we need to synchronously
        // "activate" the tree still during commit to satisfy any potential
        // SetNextCommitWaitsForActivation calls.  Unfortunately, the tree
        // might not be ready to draw, so DidActivateSyncTree must set
        // the flag to force the tree to not draw until textures are ready.
        NotifyReadyToActivate();
    }
}

void SingleThreadProxy::CommitComplete()
{
    // Commit complete happens on the main side after activate to satisfy any
    // SetNextCommitWaitsForActivation calls.
    DCHECK(!layer_tree_host_impl_->pending_tree())
        << "Activation is expected to have synchronously occurred by now.";
    DCHECK(commit_blocking_task_runner_);

    DebugScopedSetMainThread main(this);
    commit_blocking_task_runner_.reset();
    layer_tree_host_->CommitComplete();
    layer_tree_host_->DidBeginMainFrame();

    next_frame_is_newly_committed_frame_ = true;
}

void SingleThreadProxy::SetNeedsCommit()
{
    DCHECK(Proxy::IsMainThread());
    client_->ScheduleComposite();
    if (commit_requested_)
        return;
    commit_requested_ = true;
    DebugScopedSetImplThread impl(this);
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetNeedsBeginMainFrame();
}

void SingleThreadProxy::SetNeedsRedraw(const gfx::Rect& damage_rect)
{
    TRACE_EVENT0("cc", "SingleThreadProxy::SetNeedsRedraw");
    DCHECK(Proxy::IsMainThread());
    DebugScopedSetImplThread impl(this);
    client_->ScheduleComposite();
    SetNeedsRedrawRectOnImplThread(damage_rect);
}

void SingleThreadProxy::SetNextCommitWaitsForActivation()
{
    // Activation always forced in commit, so nothing to do.
    DCHECK(Proxy::IsMainThread());
}

void SingleThreadProxy::SetDeferCommits(bool defer_commits)
{
    DCHECK(Proxy::IsMainThread());
    // Deferring commits only makes sense if there's a scheduler.
    if (!scheduler_on_impl_thread_)
        return;
    if (defer_commits_ == defer_commits)
        return;

    if (defer_commits)
        TRACE_EVENT_ASYNC_BEGIN0("cc", "SingleThreadProxy::SetDeferCommits", this);
    else
        TRACE_EVENT_ASYNC_END0("cc", "SingleThreadProxy::SetDeferCommits", this);

    defer_commits_ = defer_commits;
    scheduler_on_impl_thread_->SetDeferCommits(defer_commits);
}

bool SingleThreadProxy::CommitRequested() const
{
    DCHECK(Proxy::IsMainThread());
    return commit_requested_;
}

bool SingleThreadProxy::BeginMainFrameRequested() const
{
    DCHECK(Proxy::IsMainThread());
    // If there is no scheduler, then there can be no pending begin frame,
    // as all frames are all manually initiated by the embedder of cc.
    if (!scheduler_on_impl_thread_)
        return false;
    return commit_requested_;
}

void SingleThreadProxy::Stop()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::stop");
    DCHECK(Proxy::IsMainThread());
    {
        DebugScopedSetMainThreadBlocked main_thread_blocked(this);
        DebugScopedSetImplThread impl(this);

        BlockingTaskRunner::CapturePostTasks blocked(
            blocking_main_thread_task_runner());
        scheduler_on_impl_thread_ = nullptr;
        layer_tree_host_impl_ = nullptr;
    }
    layer_tree_host_ = NULL;
}

void SingleThreadProxy::OnCanDrawStateChanged(bool can_draw)
{
    TRACE_EVENT1(
        "cc", "SingleThreadProxy::OnCanDrawStateChanged", "can_draw", can_draw);
    DCHECK(Proxy::IsImplThread());
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetCanDraw(can_draw);
}

void SingleThreadProxy::NotifyReadyToActivate()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::NotifyReadyToActivate");
    DebugScopedSetImplThread impl(this);
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->NotifyReadyToActivate();
}

void SingleThreadProxy::NotifyReadyToDraw()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::NotifyReadyToDraw");
    DebugScopedSetImplThread impl(this);
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->NotifyReadyToDraw();
}

void SingleThreadProxy::SetNeedsRedrawOnImplThread()
{
    client_->ScheduleComposite();
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetNeedsRedraw();
}

void SingleThreadProxy::SetNeedsAnimateOnImplThread()
{
    client_->ScheduleComposite();
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetNeedsAnimate();
}

void SingleThreadProxy::SetNeedsPrepareTilesOnImplThread()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::SetNeedsPrepareTilesOnImplThread");
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetNeedsPrepareTiles();
}

void SingleThreadProxy::SetNeedsRedrawRectOnImplThread(
    const gfx::Rect& damage_rect)
{
    layer_tree_host_impl_->SetViewportDamage(damage_rect);
    SetNeedsRedrawOnImplThread();
}

void SingleThreadProxy::SetNeedsCommitOnImplThread()
{
    client_->ScheduleComposite();
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetNeedsBeginMainFrame();
}

void SingleThreadProxy::SetVideoNeedsBeginFrames(bool needs_begin_frames)
{
    TRACE_EVENT1("cc", "SingleThreadProxy::SetVideoNeedsBeginFrames",
        "needs_begin_frames", needs_begin_frames);
    // In tests the layer tree is destroyed after the scheduler is.
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetVideoNeedsBeginFrames(needs_begin_frames);
}

void SingleThreadProxy::PostAnimationEventsToMainThreadOnImplThread(
    scoped_ptr<AnimationEventsVector> events)
{
    TRACE_EVENT0(
        "cc", "SingleThreadProxy::PostAnimationEventsToMainThreadOnImplThread");
    DCHECK(Proxy::IsImplThread());
    DebugScopedSetMainThread main(this);
    layer_tree_host_->SetAnimationEvents(events.Pass());
}

bool SingleThreadProxy::IsInsideDraw() { return inside_draw_; }

void SingleThreadProxy::DidActivateSyncTree()
{
    // Synchronously call to CommitComplete. Resetting
    // |commit_blocking_task_runner| would make sure all tasks posted during
    // commit/activation before CommitComplete.
    CommitComplete();
}

void SingleThreadProxy::WillPrepareTiles()
{
    DCHECK(Proxy::IsImplThread());
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->WillPrepareTiles();
}

void SingleThreadProxy::DidPrepareTiles()
{
    DCHECK(Proxy::IsImplThread());
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->DidPrepareTiles();
}

void SingleThreadProxy::DidCompletePageScaleAnimationOnImplThread()
{
    layer_tree_host_->DidCompletePageScaleAnimation();
}

void SingleThreadProxy::UpdateRendererCapabilitiesOnImplThread()
{
    DCHECK(IsImplThread());
    renderer_capabilities_for_main_thread_ = layer_tree_host_impl_->GetRendererCapabilities().MainThreadCapabilities();
}

void SingleThreadProxy::DidLoseOutputSurfaceOnImplThread()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::DidLoseOutputSurfaceOnImplThread");
    {
        DebugScopedSetMainThread main(this);
        // This must happen before we notify the scheduler as it may try to recreate
        // the output surface if already in BEGIN_IMPL_FRAME_STATE_IDLE.
        layer_tree_host_->DidLoseOutputSurface();
    }
    client_->DidAbortSwapBuffers();
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->DidLoseOutputSurface();
}

void SingleThreadProxy::CommitVSyncParameters(base::TimeTicks timebase,
    base::TimeDelta interval)
{
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->CommitVSyncParameters(timebase, interval);
}

void SingleThreadProxy::SetEstimatedParentDrawTime(base::TimeDelta draw_time)
{
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetEstimatedParentDrawTime(draw_time);
}

void SingleThreadProxy::SetMaxSwapsPendingOnImplThread(int max)
{
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->SetMaxSwapsPending(max);
}

void SingleThreadProxy::DidSwapBuffersOnImplThread()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::DidSwapBuffersOnImplThread");
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->DidSwapBuffers();
    client_->DidPostSwapBuffers();
}

void SingleThreadProxy::DidSwapBuffersCompleteOnImplThread()
{
    TRACE_EVENT0("cc,benchmark",
        "SingleThreadProxy::DidSwapBuffersCompleteOnImplThread");
    if (scheduler_on_impl_thread_)
        scheduler_on_impl_thread_->DidSwapBuffersComplete();
    layer_tree_host_->DidCompleteSwapBuffers();
}

void SingleThreadProxy::OnDrawForOutputSurface()
{
    NOTREACHED() << "Implemented by ThreadProxy for synchronous compositor.";
}

void SingleThreadProxy::PostFrameTimingEventsOnImplThread(
    scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
    scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events)
{
    layer_tree_host_->RecordFrameTimingEvents(composite_events.Pass(),
        main_frame_events.Pass());
}

void SingleThreadProxy::LayoutAndUpdateLayers()
{
    if (layer_tree_host_->output_surface_lost()) {
        RequestNewOutputSurface();
        // RequestNewOutputSurface could have synchronously created an output
        // surface, so check again before returning.
        if (layer_tree_host_->output_surface_lost())
            return;
    }

    layer_tree_host_->Layout();
    layer_tree_host_->UpdateLayers();
}

void SingleThreadProxy::CompositeImmediately(base::TimeTicks frame_begin_time)
{
    TRACE_EVENT0("cc,benchmark", "SingleThreadProxy::CompositeImmediately");
    DCHECK(Proxy::IsMainThread());
#if DCHECK_IS_ON()
    DCHECK(!inside_impl_frame_);
#endif
    base::AutoReset<bool> inside_composite(&inside_synchronous_composite_, true);

    if (layer_tree_host_->output_surface_lost()) {
        RequestNewOutputSurface();
        // RequestNewOutputSurface could have synchronously created an output
        // surface, so check again before returning.
        if (layer_tree_host_->output_surface_lost())
            return;
    }

    BeginFrameArgs begin_frame_args(BeginFrameArgs::Create(
        BEGINFRAME_FROM_HERE, frame_begin_time, base::TimeTicks(),
        BeginFrameArgs::DefaultInterval(), BeginFrameArgs::NORMAL));

    // Start the impl frame.
    {
        DebugScopedSetImplThread impl(this);
        WillBeginImplFrame(begin_frame_args);
    }

    // Run the "main thread" and get it to commit.
    {
#if DCHECK_IS_ON()
        DCHECK(inside_impl_frame_);
#endif
        DoBeginMainFrame(begin_frame_args);
        DoCommit();

        DCHECK_EQ(0u, layer_tree_host_->num_queued_swap_promises())
            << "Commit should always succeed and transfer promises.";
    }

    // Finish the impl frame.
    {
        DebugScopedSetImplThread impl(this);
        layer_tree_host_impl_->ActivateSyncTree();
        DCHECK(
            !layer_tree_host_impl_->active_tree()->needs_update_draw_properties());
        layer_tree_host_impl_->PrepareTiles();
        layer_tree_host_impl_->SynchronouslyInitializeAllTiles();

        // TODO(danakj): Don't do this last... we prepared the wrong things. D:
        layer_tree_host_impl_->Animate();

        LayerTreeHostImpl::FrameData frame;
        DoComposite(&frame);

        // DoComposite could abort, but because this is a synchronous composite
        // another draw will never be scheduled, so break remaining promises.
        layer_tree_host_impl_->active_tree()->BreakSwapPromises(
            SwapPromise::SWAP_FAILS);

        DidFinishImplFrame();
    }
}

bool SingleThreadProxy::SupportsImplScrolling() const
{
    return false;
}

bool SingleThreadProxy::ShouldComposite() const
{
    DCHECK(Proxy::IsImplThread());
    return layer_tree_host_impl_->visible() && layer_tree_host_impl_->CanDraw();
}

void SingleThreadProxy::ScheduleRequestNewOutputSurface()
{
    if (output_surface_creation_callback_.IsCancelled() && !output_surface_creation_requested_) {
        output_surface_creation_callback_.Reset(
            base::Bind(&SingleThreadProxy::RequestNewOutputSurface,
                weak_factory_.GetWeakPtr()));
        MainThreadTaskRunner()->PostTask(
            FROM_HERE, output_surface_creation_callback_.callback());
    }
}

DrawResult SingleThreadProxy::DoComposite(LayerTreeHostImpl::FrameData* frame)
{
    TRACE_EVENT0("cc", "SingleThreadProxy::DoComposite");
    DCHECK(!layer_tree_host_->output_surface_lost());

    DrawResult draw_result;
    bool draw_frame;
    {
        DebugScopedSetImplThread impl(this);
        base::AutoReset<bool> mark_inside(&inside_draw_, true);

        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile1(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoComposite1"));

        // We guard PrepareToDraw() with CanDraw() because it always returns a valid
        // frame, so can only be used when such a frame is possible. Since
        // DrawLayers() depends on the result of PrepareToDraw(), it is guarded on
        // CanDraw() as well.
        if (!ShouldComposite()) {
            return DRAW_ABORTED_CANT_DRAW;
        }

        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile2(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoComposite2"));
        draw_result = layer_tree_host_impl_->PrepareToDraw(frame);
        draw_frame = draw_result == DRAW_SUCCESS;
        if (draw_frame) {
            // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
            // is fixed.
            tracked_objects::ScopedTracker tracking_profile3(
                FROM_HERE_WITH_EXPLICIT_FUNCTION(
                    "461509 SingleThreadProxy::DoComposite3"));
            layer_tree_host_impl_->DrawLayers(frame);
        }
        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile4(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoComposite4"));
        layer_tree_host_impl_->DidDrawAllLayers(*frame);

        bool start_ready_animations = draw_frame;
        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile5(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoComposite5"));
        layer_tree_host_impl_->UpdateAnimationState(start_ready_animations);

        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile7(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoComposite7"));
    }

    if (draw_frame) {
        DebugScopedSetImplThread impl(this);

        // This CapturePostTasks should be destroyed before
        // DidCommitAndDrawFrame() is called since that goes out to the
        // embedder,
        // and we want the embedder to receive its callbacks before that.
        // NOTE: This maintains consistent ordering with the ThreadProxy since
        // the DidCommitAndDrawFrame() must be post-tasked from the impl thread
        // there as the main thread is not blocked, so any posted tasks inside
        // the swap buffers will execute first.
        DebugScopedSetMainThreadBlocked main_thread_blocked(this);

        BlockingTaskRunner::CapturePostTasks blocked(
            blocking_main_thread_task_runner());
        // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509
        // is fixed.
        tracked_objects::ScopedTracker tracking_profile8(
            FROM_HERE_WITH_EXPLICIT_FUNCTION(
                "461509 SingleThreadProxy::DoComposite8"));
        layer_tree_host_impl_->SwapBuffers(*frame);
    }
    // TODO(robliao): Remove ScopedTracker below once https://crbug.com/461509 is
    // fixed.
    tracked_objects::ScopedTracker tracking_profile9(
        FROM_HERE_WITH_EXPLICIT_FUNCTION(
            "461509 SingleThreadProxy::DoComposite9"));
    DidCommitAndDrawFrame();

    return draw_result;
}

void SingleThreadProxy::DidCommitAndDrawFrame()
{
    if (next_frame_is_newly_committed_frame_) {
        DebugScopedSetMainThread main(this);
        next_frame_is_newly_committed_frame_ = false;
        layer_tree_host_->DidCommitAndDrawFrame();
    }
}

bool SingleThreadProxy::MainFrameWillHappenForTesting()
{
    if (layer_tree_host_->output_surface_lost())
        return false;
    if (!scheduler_on_impl_thread_)
        return false;
    return scheduler_on_impl_thread_->MainFrameForTestingWillHappen();
}

void SingleThreadProxy::SetChildrenNeedBeginFrames(
    bool children_need_begin_frames)
{
    scheduler_on_impl_thread_->SetChildrenNeedBeginFrames(
        children_need_begin_frames);
}

void SingleThreadProxy::SetAuthoritativeVSyncInterval(
    const base::TimeDelta& interval)
{
    scheduler_on_impl_thread_->SetAuthoritativeVSyncInterval(interval);
}

void SingleThreadProxy::WillBeginImplFrame(const BeginFrameArgs& args)
{
#if DCHECK_IS_ON()
    DCHECK(!inside_impl_frame_)
        << "WillBeginImplFrame called while already inside an impl frame!";
    inside_impl_frame_ = true;
#endif
    layer_tree_host_impl_->WillBeginImplFrame(args);
}

void SingleThreadProxy::ScheduledActionSendBeginMainFrame()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::ScheduledActionSendBeginMainFrame");
    // Although this proxy is single-threaded, it's problematic to synchronously
    // have BeginMainFrame happen after ScheduledActionSendBeginMainFrame.  This
    // could cause a commit to occur in between a series of SetNeedsCommit calls
    // (i.e. property modifications) causing some to fall on one frame and some to
    // fall on the next.  Doing it asynchronously instead matches the semantics of
    // ThreadProxy::SetNeedsCommit where SetNeedsCommit will not cause a
    // synchronous commit.
#if DCHECK_IS_ON()
    DCHECK(inside_impl_frame_)
        << "BeginMainFrame should only be sent inside a BeginImplFrame";
#endif
    const BeginFrameArgs& begin_frame_args = layer_tree_host_impl_->CurrentBeginFrameArgs();

    MainThreadTaskRunner()->PostTask(
        FROM_HERE, base::Bind(&SingleThreadProxy::BeginMainFrame, weak_factory_.GetWeakPtr(), begin_frame_args));
}

void SingleThreadProxy::SendBeginMainFrameNotExpectedSoon()
{
    layer_tree_host_->BeginMainFrameNotExpectedSoon();
}

void SingleThreadProxy::BeginMainFrame(const BeginFrameArgs& begin_frame_args)
{
    commit_requested_ = false;
    animate_requested_ = false;

    if (defer_commits_) {
        TRACE_EVENT_INSTANT0("cc", "EarlyOut_DeferCommit",
            TRACE_EVENT_SCOPE_THREAD);
        BeginMainFrameAbortedOnImplThread(
            CommitEarlyOutReason::ABORTED_DEFERRED_COMMIT);
        return;
    }

    // This checker assumes NotifyReadyToCommit in this stack causes a synchronous
    // commit.
    ScopedAbortRemainingSwapPromises swap_promise_checker(layer_tree_host_);

    if (!layer_tree_host_->visible()) {
        TRACE_EVENT_INSTANT0("cc", "EarlyOut_NotVisible", TRACE_EVENT_SCOPE_THREAD);
        BeginMainFrameAbortedOnImplThread(
            CommitEarlyOutReason::ABORTED_NOT_VISIBLE);
        return;
    }

    if (layer_tree_host_->output_surface_lost()) {
        TRACE_EVENT_INSTANT0(
            "cc", "EarlyOut_OutputSurfaceLost", TRACE_EVENT_SCOPE_THREAD);
        BeginMainFrameAbortedOnImplThread(
            CommitEarlyOutReason::ABORTED_OUTPUT_SURFACE_LOST);
        return;
    }

    // Prevent new commits from being requested inside DoBeginMainFrame.
    // Note: We do not want to prevent SetNeedsAnimate from requesting
    // a commit here.
    commit_requested_ = true;

    DoBeginMainFrame(begin_frame_args);
}

void SingleThreadProxy::DoBeginMainFrame(
    const BeginFrameArgs& begin_frame_args)
{
    layer_tree_host_->WillBeginMainFrame();
    layer_tree_host_->BeginMainFrame(begin_frame_args);
    layer_tree_host_->AnimateLayers(begin_frame_args.frame_time);
    layer_tree_host_->Layout();

    // New commits requested inside UpdateLayers should be respected.
    commit_requested_ = false;

    layer_tree_host_->UpdateLayers();

    // TODO(enne): SingleThreadProxy does not support cancelling commits yet,
    // search for CommitEarlyOutReason::FINISHED_NO_UPDATES inside
    // thread_proxy.cc
    if (scheduler_on_impl_thread_) {
        scheduler_on_impl_thread_->NotifyBeginMainFrameStarted();
        scheduler_on_impl_thread_->NotifyReadyToCommit();
    }
}

void SingleThreadProxy::BeginMainFrameAbortedOnImplThread(
    CommitEarlyOutReason reason)
{
    DebugScopedSetImplThread impl(this);
    DCHECK(scheduler_on_impl_thread_->CommitPending());
    DCHECK(!layer_tree_host_impl_->pending_tree());

    layer_tree_host_impl_->BeginMainFrameAborted(reason);
    scheduler_on_impl_thread_->BeginMainFrameAborted(reason);
}

DrawResult SingleThreadProxy::ScheduledActionDrawAndSwapIfPossible()
{
    DebugScopedSetImplThread impl(this);
    LayerTreeHostImpl::FrameData frame;
    return DoComposite(&frame);
}

DrawResult SingleThreadProxy::ScheduledActionDrawAndSwapForced()
{
    NOTREACHED();
    return INVALID_RESULT;
}

void SingleThreadProxy::ScheduledActionCommit()
{
    DebugScopedSetMainThread main(this);
    DoCommit();
}

void SingleThreadProxy::ScheduledActionAnimate()
{
    TRACE_EVENT0("cc", "ScheduledActionAnimate");
    DebugScopedSetImplThread impl(this);
    layer_tree_host_impl_->Animate();
}

void SingleThreadProxy::ScheduledActionActivateSyncTree()
{
    DebugScopedSetImplThread impl(this);
    layer_tree_host_impl_->ActivateSyncTree();
}

void SingleThreadProxy::ScheduledActionBeginOutputSurfaceCreation()
{
    DebugScopedSetMainThread main(this);
    DCHECK(scheduler_on_impl_thread_);
    // If possible, create the output surface in a post task.  Synchronously
    // creating the output surface makes tests more awkward since this differs
    // from the ThreadProxy behavior.  However, sometimes there is no
    // task runner.
    if (Proxy::MainThreadTaskRunner()) {
        ScheduleRequestNewOutputSurface();
    } else {
        RequestNewOutputSurface();
    }
}

void SingleThreadProxy::ScheduledActionPrepareTiles()
{
    TRACE_EVENT0("cc", "SingleThreadProxy::ScheduledActionPrepareTiles");
    DebugScopedSetImplThread impl(this);
    layer_tree_host_impl_->PrepareTiles();
}

void SingleThreadProxy::ScheduledActionInvalidateOutputSurface()
{
    NOTREACHED();
}

void SingleThreadProxy::DidFinishImplFrame()
{
    layer_tree_host_impl_->DidFinishImplFrame();
#if DCHECK_IS_ON()
    DCHECK(inside_impl_frame_)
        << "DidFinishImplFrame called while not inside an impl frame!";
    inside_impl_frame_ = false;
#endif
}

void SingleThreadProxy::SendBeginFramesToChildren(const BeginFrameArgs& args)
{
    layer_tree_host_->SendBeginFramesToChildren(args);
}

} // namespace cc
