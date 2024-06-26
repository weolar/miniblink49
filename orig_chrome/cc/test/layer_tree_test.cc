// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/layer_tree_test.h"

#include "base/command_line.h"
#include "base/location.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"
#include "cc/animation/animation.h"
#include "cc/animation/animation_host.h"
#include "cc/animation/animation_registrar.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/animation/timing_function.h"
#include "cc/base/switches.h"
#include "cc/input/input_handler.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_impl.h"
#include "cc/test/animation_test_common.h"
#include "cc/test/begin_frame_args_test.h"
#include "cc/test/fake_external_begin_frame_source.h"
#include "cc/test/fake_layer_tree_host_client.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/test_context_provider.h"
#include "cc/test/test_gpu_memory_buffer_manager.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/layer_tree_host_client.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/layer_tree_host_single_thread_client.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/single_thread_proxy.h"
#include "cc/trees/thread_proxy.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "ui/gfx/geometry/size_conversions.h"

namespace cc {

void CreateVirtualViewportLayers(Layer* root_layer,
    scoped_refptr<Layer> outer_scroll_layer,
    const gfx::Size& inner_bounds,
    const gfx::Size& outer_bounds,
    LayerTreeHost* host,
    const LayerSettings& layer_settings)
{
    scoped_refptr<Layer> inner_viewport_container_layer = Layer::Create(layer_settings);
    scoped_refptr<Layer> inner_viewport_scroll_layer = Layer::Create(layer_settings);
    scoped_refptr<Layer> outer_viewport_container_layer = Layer::Create(layer_settings);
    scoped_refptr<Layer> page_scale_layer = Layer::Create(layer_settings);

    root_layer->AddChild(inner_viewport_container_layer);
    inner_viewport_container_layer->AddChild(page_scale_layer);
    page_scale_layer->AddChild(inner_viewport_scroll_layer);
    inner_viewport_scroll_layer->AddChild(outer_viewport_container_layer);
    outer_viewport_container_layer->AddChild(outer_scroll_layer);

    inner_viewport_scroll_layer->SetScrollClipLayerId(
        inner_viewport_container_layer->id());
    outer_scroll_layer->SetScrollClipLayerId(
        outer_viewport_container_layer->id());

    inner_viewport_container_layer->SetBounds(inner_bounds);
    inner_viewport_scroll_layer->SetBounds(outer_bounds);
    outer_viewport_container_layer->SetBounds(outer_bounds);

    inner_viewport_scroll_layer->SetIsContainerForFixedPositionLayers(true);
    outer_scroll_layer->SetIsContainerForFixedPositionLayers(true);
    host->RegisterViewportLayers(NULL,
        root_layer,
        inner_viewport_scroll_layer,
        outer_scroll_layer);
}

void CreateVirtualViewportLayers(Layer* root_layer,
    const gfx::Size& inner_bounds,
    const gfx::Size& outer_bounds,
    const gfx::Size& scroll_bounds,
    LayerTreeHost* host,
    const LayerSettings& layer_settings)
{
    scoped_refptr<Layer> outer_viewport_scroll_layer = Layer::Create(layer_settings);

    outer_viewport_scroll_layer->SetBounds(scroll_bounds);
    outer_viewport_scroll_layer->SetIsDrawable(true);
    CreateVirtualViewportLayers(root_layer,
        outer_viewport_scroll_layer,
        inner_bounds,
        outer_bounds,
        host,
        layer_settings);
}

TestHooks::TestHooks() { }

TestHooks::~TestHooks() { }

DrawResult TestHooks::PrepareToDrawOnThread(
    LayerTreeHostImpl* host_impl,
    LayerTreeHostImpl::FrameData* frame_data,
    DrawResult draw_result)
{
    return draw_result;
}

void TestHooks::CreateResourceAndTileTaskWorkerPool(
    LayerTreeHostImpl* host_impl,
    scoped_ptr<TileTaskWorkerPool>* tile_task_worker_pool,
    scoped_ptr<ResourcePool>* resource_pool)
{
    host_impl->LayerTreeHostImpl::CreateResourceAndTileTaskWorkerPool(
        tile_task_worker_pool, resource_pool);
}

// Adapts ThreadProxy for test. Injects test hooks for testing.
class ThreadProxyForTest : public ThreadProxy {
public:
    static scoped_ptr<Proxy> Create(
        TestHooks* test_hooks,
        LayerTreeHost* host,
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
        scoped_ptr<BeginFrameSource> external_begin_frame_source)
    {
        return make_scoped_ptr(new ThreadProxyForTest(
            test_hooks,
            host,
            main_task_runner,
            impl_task_runner,
            external_begin_frame_source.Pass()));
    }

    ~ThreadProxyForTest() override { }

private:
    TestHooks* test_hooks_;

    void SetNeedsUpdateLayers() override
    {
        ThreadProxy::SetNeedsUpdateLayers();
        test_hooks_->DidSetNeedsUpdateLayers();
    }

    void ScheduledActionSendBeginMainFrame() override
    {
        test_hooks_->ScheduledActionWillSendBeginMainFrame();
        ThreadProxy::ScheduledActionSendBeginMainFrame();
        test_hooks_->ScheduledActionSendBeginMainFrame();
    }

    DrawResult ScheduledActionDrawAndSwapIfPossible() override
    {
        DrawResult result = ThreadProxy::ScheduledActionDrawAndSwapIfPossible();
        test_hooks_->ScheduledActionDrawAndSwapIfPossible();
        return result;
    }

    void ScheduledActionAnimate() override
    {
        ThreadProxy::ScheduledActionAnimate();
        test_hooks_->ScheduledActionAnimate();
    }

    void ScheduledActionCommit() override
    {
        ThreadProxy::ScheduledActionCommit();
        test_hooks_->ScheduledActionCommit();
    }

    void ScheduledActionBeginOutputSurfaceCreation() override
    {
        ThreadProxy::ScheduledActionBeginOutputSurfaceCreation();
        test_hooks_->ScheduledActionBeginOutputSurfaceCreation();
    }

    void ScheduledActionPrepareTiles() override
    {
        ThreadProxy::ScheduledActionPrepareTiles();
        test_hooks_->ScheduledActionPrepareTiles();
    }

    void ScheduledActionInvalidateOutputSurface() override
    {
        ThreadProxy::ScheduledActionInvalidateOutputSurface();
        test_hooks_->ScheduledActionInvalidateOutputSurface();
    }

    void SendBeginMainFrameNotExpectedSoon() override
    {
        ThreadProxy::SendBeginMainFrameNotExpectedSoon();
        test_hooks_->SendBeginMainFrameNotExpectedSoon();
    }

    ThreadProxyForTest(
        TestHooks* test_hooks,
        LayerTreeHost* host,
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
        scoped_ptr<BeginFrameSource> external_begin_frame_source)
        : ThreadProxy(host, main_task_runner,
            impl_task_runner,
            external_begin_frame_source.Pass())
        , test_hooks_(test_hooks)
    {
    }
};

// Adapts ThreadProxy for test. Injects test hooks for testing.
class SingleThreadProxyForTest : public SingleThreadProxy {
public:
    static scoped_ptr<Proxy> Create(
        TestHooks* test_hooks,
        LayerTreeHost* host,
        LayerTreeHostSingleThreadClient* client,
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_ptr<BeginFrameSource> external_begin_frame_source)
    {
        return make_scoped_ptr(new SingleThreadProxyForTest(
            test_hooks, host, client, main_task_runner,
            external_begin_frame_source.Pass()));
    }

    ~SingleThreadProxyForTest() override { }

private:
    TestHooks* test_hooks_;

    void ScheduledActionSendBeginMainFrame() override
    {
        test_hooks_->ScheduledActionWillSendBeginMainFrame();
        SingleThreadProxy::ScheduledActionSendBeginMainFrame();
        test_hooks_->ScheduledActionSendBeginMainFrame();
    }

    DrawResult ScheduledActionDrawAndSwapIfPossible() override
    {
        DrawResult result = SingleThreadProxy::ScheduledActionDrawAndSwapIfPossible();
        test_hooks_->ScheduledActionDrawAndSwapIfPossible();
        return result;
    }

    void ScheduledActionAnimate() override
    {
        SingleThreadProxy::ScheduledActionAnimate();
        test_hooks_->ScheduledActionAnimate();
    }

    void ScheduledActionCommit() override
    {
        SingleThreadProxy::ScheduledActionCommit();
        test_hooks_->ScheduledActionCommit();
    }

    void ScheduledActionBeginOutputSurfaceCreation() override
    {
        SingleThreadProxy::ScheduledActionBeginOutputSurfaceCreation();
        test_hooks_->ScheduledActionBeginOutputSurfaceCreation();
    }

    void ScheduledActionPrepareTiles() override
    {
        SingleThreadProxy::ScheduledActionPrepareTiles();
        test_hooks_->ScheduledActionPrepareTiles();
    }

    void ScheduledActionInvalidateOutputSurface() override
    {
        SingleThreadProxy::ScheduledActionInvalidateOutputSurface();
        test_hooks_->ScheduledActionInvalidateOutputSurface();
    }

    void SendBeginMainFrameNotExpectedSoon() override
    {
        SingleThreadProxy::SendBeginMainFrameNotExpectedSoon();
        test_hooks_->SendBeginMainFrameNotExpectedSoon();
    }

    SingleThreadProxyForTest(
        TestHooks* test_hooks,
        LayerTreeHost* host,
        LayerTreeHostSingleThreadClient* client,
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_ptr<BeginFrameSource> external_begin_frame_source)
        : SingleThreadProxy(host, client, main_task_runner,
            external_begin_frame_source.Pass())
        , test_hooks_(test_hooks)
    {
    }
};

// Adapts LayerTreeHostImpl for test. Runs real code, then invokes test hooks.
class LayerTreeHostImplForTesting : public LayerTreeHostImpl {
public:
    static scoped_ptr<LayerTreeHostImplForTesting> Create(
        TestHooks* test_hooks,
        const LayerTreeSettings& settings,
        LayerTreeHostImplClient* host_impl_client,
        Proxy* proxy,
        SharedBitmapManager* shared_bitmap_manager,
        gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
        TaskGraphRunner* task_graph_runner,
        RenderingStatsInstrumentation* stats_instrumentation)
    {
        return make_scoped_ptr(new LayerTreeHostImplForTesting(
            test_hooks, settings, host_impl_client, proxy, shared_bitmap_manager,
            gpu_memory_buffer_manager, task_graph_runner, stats_instrumentation));
    }

protected:
    LayerTreeHostImplForTesting(
        TestHooks* test_hooks,
        const LayerTreeSettings& settings,
        LayerTreeHostImplClient* host_impl_client,
        Proxy* proxy,
        SharedBitmapManager* shared_bitmap_manager,
        gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
        TaskGraphRunner* task_graph_runner,
        RenderingStatsInstrumentation* stats_instrumentation)
        : LayerTreeHostImpl(settings,
            host_impl_client,
            proxy,
            stats_instrumentation,
            shared_bitmap_manager,
            gpu_memory_buffer_manager,
            task_graph_runner,
            0)
        , test_hooks_(test_hooks)
        , block_notify_ready_to_activate_for_testing_(false)
        , notify_ready_to_activate_was_blocked_(false)
    {
    }

    void CreateResourceAndTileTaskWorkerPool(
        scoped_ptr<TileTaskWorkerPool>* tile_task_worker_pool,
        scoped_ptr<ResourcePool>* resource_pool) override
    {
        test_hooks_->CreateResourceAndTileTaskWorkerPool(
            this, tile_task_worker_pool, resource_pool);
    }

    void WillBeginImplFrame(const BeginFrameArgs& args) override
    {
        LayerTreeHostImpl::WillBeginImplFrame(args);
        test_hooks_->WillBeginImplFrameOnThread(this, args);
    }

    void DidFinishImplFrame() override
    {
        LayerTreeHostImpl::DidFinishImplFrame();
        test_hooks_->DidFinishImplFrameOnThread(this);
    }

    void BeginMainFrameAborted(CommitEarlyOutReason reason) override
    {
        LayerTreeHostImpl::BeginMainFrameAborted(reason);
        test_hooks_->BeginMainFrameAbortedOnThread(this, reason);
    }

    void BeginCommit() override
    {
        LayerTreeHostImpl::BeginCommit();
        test_hooks_->BeginCommitOnThread(this);
    }

    void CommitComplete() override
    {
        test_hooks_->WillCommitCompleteOnThread(this);
        LayerTreeHostImpl::CommitComplete();
        test_hooks_->CommitCompleteOnThread(this);
    }

    bool PrepareTiles() override
    {
        test_hooks_->WillPrepareTiles(this);
        return LayerTreeHostImpl::PrepareTiles();
    }

    DrawResult PrepareToDraw(FrameData* frame) override
    {
        DrawResult draw_result = LayerTreeHostImpl::PrepareToDraw(frame);
        return test_hooks_->PrepareToDrawOnThread(this, frame, draw_result);
    }

    void DrawLayers(FrameData* frame) override
    {
        LayerTreeHostImpl::DrawLayers(frame);
        test_hooks_->DrawLayersOnThread(this);
    }

    bool SwapBuffers(const LayerTreeHostImpl::FrameData& frame) override
    {
        bool result = LayerTreeHostImpl::SwapBuffers(frame);
        test_hooks_->SwapBuffersOnThread(this, result);
        return result;
    }

    void DidSwapBuffersComplete() override
    {
        LayerTreeHostImpl::DidSwapBuffersComplete();
        test_hooks_->SwapBuffersCompleteOnThread(this);
    }

    void ReclaimResources(const CompositorFrameAck* ack) override
    {
        LayerTreeHostImpl::ReclaimResources(ack);
    }

    void NotifyReadyToActivate() override
    {
        if (block_notify_ready_to_activate_for_testing_) {
            notify_ready_to_activate_was_blocked_ = true;
        } else {
            LayerTreeHostImpl::NotifyReadyToActivate();
            test_hooks_->NotifyReadyToActivateOnThread(this);
        }
    }

    void NotifyReadyToDraw() override
    {
        LayerTreeHostImpl::NotifyReadyToDraw();
        test_hooks_->NotifyReadyToDrawOnThread(this);
    }

    void NotifyAllTileTasksCompleted() override
    {
        LayerTreeHostImpl::NotifyAllTileTasksCompleted();
        test_hooks_->NotifyAllTileTasksCompleted(this);
    }

    void BlockNotifyReadyToActivateForTesting(bool block) override
    {
        CHECK(proxy()->ImplThreadTaskRunner())
            << "Not supported for single-threaded mode.";
        block_notify_ready_to_activate_for_testing_ = block;
        if (!block && notify_ready_to_activate_was_blocked_) {
            NotifyReadyToActivate();
            notify_ready_to_activate_was_blocked_ = false;
        }
    }

    void ActivateSyncTree() override
    {
        test_hooks_->WillActivateTreeOnThread(this);
        LayerTreeHostImpl::ActivateSyncTree();
        DCHECK(!pending_tree());
        test_hooks_->DidActivateTreeOnThread(this);
    }

    bool InitializeRenderer(OutputSurface* output_surface) override
    {
        bool success = LayerTreeHostImpl::InitializeRenderer(output_surface);
        test_hooks_->InitializedRendererOnThread(this, success);
        return success;
    }

    void SetVisible(bool visible) override
    {
        LayerTreeHostImpl::SetVisible(visible);
        test_hooks_->DidSetVisibleOnImplTree(this, visible);
    }

    void AnimateLayers(base::TimeTicks monotonic_time) override
    {
        test_hooks_->WillAnimateLayers(this, monotonic_time);
        LayerTreeHostImpl::AnimateLayers(monotonic_time);
        test_hooks_->AnimateLayers(this, monotonic_time);
    }

    void UpdateAnimationState(bool start_ready_animations) override
    {
        LayerTreeHostImpl::UpdateAnimationState(start_ready_animations);
        bool has_unfinished_animation = false;
        AnimationRegistrar* registrar = animation_registrar() ? animation_registrar()
                                                              : animation_host()->animation_registrar();
        for (const auto& it :
            registrar->active_animation_controllers_for_testing()) {
            if (it.second->HasActiveAnimation()) {
                has_unfinished_animation = true;
                break;
            }
        }
        test_hooks_->UpdateAnimationState(this, has_unfinished_animation);
    }

    void NotifyTileStateChanged(const Tile* tile) override
    {
        LayerTreeHostImpl::NotifyTileStateChanged(tile);
        test_hooks_->NotifyTileStateChangedOnThread(this, tile);
    }

private:
    TestHooks* test_hooks_;
    bool block_notify_ready_to_activate_for_testing_;
    bool notify_ready_to_activate_was_blocked_;
};

// Implementation of LayerTreeHost callback interface.
class LayerTreeHostClientForTesting : public LayerTreeHostClient,
                                      public LayerTreeHostSingleThreadClient {
public:
    static scoped_ptr<LayerTreeHostClientForTesting> Create(
        TestHooks* test_hooks)
    {
        return make_scoped_ptr(new LayerTreeHostClientForTesting(test_hooks));
    }
    ~LayerTreeHostClientForTesting() override { }

    void WillBeginMainFrame() override { test_hooks_->WillBeginMainFrame(); }

    void DidBeginMainFrame() override { test_hooks_->DidBeginMainFrame(); }

    void BeginMainFrame(const BeginFrameArgs& args) override
    {
        test_hooks_->BeginMainFrame(args);
    }

    void Layout() override { test_hooks_->Layout(); }

    void ApplyViewportDeltas(const gfx::Vector2dF& inner_delta,
        const gfx::Vector2dF& outer_delta,
        const gfx::Vector2dF& elastic_overscroll_delta,
        float page_scale,
        float top_controls_delta) override
    {
        test_hooks_->ApplyViewportDeltas(inner_delta, outer_delta,
            elastic_overscroll_delta, page_scale,
            top_controls_delta);
    }

    void RequestNewOutputSurface() override
    {
        test_hooks_->RequestNewOutputSurface();
    }

    void DidInitializeOutputSurface() override
    {
        test_hooks_->DidInitializeOutputSurface();
    }

    void SendBeginFramesToChildren(const BeginFrameArgs& args) override
    {
        test_hooks_->SendBeginFramesToChildren(args);
    }

    void DidFailToInitializeOutputSurface() override
    {
        test_hooks_->DidFailToInitializeOutputSurface();
        RequestNewOutputSurface();
    }

    void WillCommit() override { test_hooks_->WillCommit(); }

    void DidCommit() override { test_hooks_->DidCommit(); }

    void DidCommitAndDrawFrame() override
    {
        test_hooks_->DidCommitAndDrawFrame();
    }

    void DidCompleteSwapBuffers() override
    {
        test_hooks_->DidCompleteSwapBuffers();
    }

    void DidPostSwapBuffers() override { }
    void DidAbortSwapBuffers() override { }
    void ScheduleComposite() override { test_hooks_->ScheduleComposite(); }
    void DidCompletePageScaleAnimation() override { }
    void BeginMainFrameNotExpectedSoon() override { }

    void RecordFrameTimingEvents(
        scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
        scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events)
        override { }

private:
    explicit LayerTreeHostClientForTesting(TestHooks* test_hooks)
        : test_hooks_(test_hooks)
    {
    }

    TestHooks* test_hooks_;
};

// Adapts LayerTreeHost for test. Injects LayerTreeHostImplForTesting.
class LayerTreeHostForTesting : public LayerTreeHost {
public:
    static scoped_ptr<LayerTreeHostForTesting> Create(
        TestHooks* test_hooks,
        LayerTreeHostClientForTesting* client,
        SharedBitmapManager* shared_bitmap_manager,
        gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
        TaskGraphRunner* task_graph_runner,
        const LayerTreeSettings& settings,
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
        scoped_ptr<BeginFrameSource> external_begin_frame_source)
    {
        LayerTreeHost::InitParams params;
        params.client = client;
        params.shared_bitmap_manager = shared_bitmap_manager;
        params.gpu_memory_buffer_manager = gpu_memory_buffer_manager;
        params.task_graph_runner = task_graph_runner;
        params.settings = &settings;
        scoped_ptr<LayerTreeHostForTesting> layer_tree_host(
            new LayerTreeHostForTesting(test_hooks, &params));
        if (impl_task_runner.get()) {
            layer_tree_host->InitializeForTesting(
                ThreadProxyForTest::Create(test_hooks,
                    layer_tree_host.get(),
                    main_task_runner,
                    impl_task_runner,
                    external_begin_frame_source.Pass()));
        } else {
            layer_tree_host->InitializeForTesting(
                SingleThreadProxyForTest::Create(
                    test_hooks,
                    layer_tree_host.get(),
                    client,
                    main_task_runner,
                    external_begin_frame_source.Pass()));
        }
        return layer_tree_host.Pass();
    }

    scoped_ptr<LayerTreeHostImpl> CreateLayerTreeHostImpl(
        LayerTreeHostImplClient* host_impl_client) override
    {
        return LayerTreeHostImplForTesting::Create(
            test_hooks_, settings(), host_impl_client, proxy(),
            shared_bitmap_manager(), gpu_memory_buffer_manager(),
            task_graph_runner(), rendering_stats_instrumentation());
    }

    void SetNeedsCommit() override
    {
        if (!test_started_)
            return;
        LayerTreeHost::SetNeedsCommit();
    }

    void SetNeedsUpdateLayers() override
    {
        if (!test_started_)
            return;
        LayerTreeHost::SetNeedsUpdateLayers();
    }

    void set_test_started(bool started) { test_started_ = started; }

private:
    LayerTreeHostForTesting(TestHooks* test_hooks,
        LayerTreeHost::InitParams* params)
        : LayerTreeHost(params)
        , test_hooks_(test_hooks)
        , test_started_(false)
    {
    }

    TestHooks* test_hooks_;
    bool test_started_;
};

LayerTreeTest::LayerTreeTest()
    : output_surface_(nullptr)
    , external_begin_frame_source_(nullptr)
    , beginning_(false)
    , end_when_begin_returns_(false)
    , timed_out_(false)
    , scheduled_(false)
    , started_(false)
    , ended_(false)
    , delegating_renderer_(false)
    , verify_property_trees_(true)
    , timeout_seconds_(0)
    , weak_factory_(this)
{
    main_thread_weak_ptr_ = weak_factory_.GetWeakPtr();

    // Tests should timeout quickly unless --cc-layer-tree-test-no-timeout was
    // specified (for running in a debugger).
    base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
    if (!command_line->HasSwitch(switches::kCCLayerTreeTestNoTimeout))
        timeout_seconds_ = 5;
}

LayerTreeTest::~LayerTreeTest() { }

void LayerTreeTest::EndTest()
{
    if (ended_)
        return;
    ended_ = true;

    // For the case where we EndTest during BeginTest(), set a flag to indicate
    // that the test should end the second BeginTest regains control.
    if (beginning_) {
        end_when_begin_returns_ = true;
    } else {
        main_task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&LayerTreeTest::RealEndTest, main_thread_weak_ptr_));
    }
}

void LayerTreeTest::EndTestAfterDelayMs(int delay_milliseconds)
{
    main_task_runner_->PostDelayedTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::EndTest, main_thread_weak_ptr_),
        base::TimeDelta::FromMilliseconds(delay_milliseconds));
}

void LayerTreeTest::PostAddAnimationToMainThread(
    Layer* layer_to_receive_animation)
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchAddAnimation, main_thread_weak_ptr_,
            base::Unretained(layer_to_receive_animation), 0.000004));
}

void LayerTreeTest::PostAddInstantAnimationToMainThread(
    Layer* layer_to_receive_animation)
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchAddAnimation,
            main_thread_weak_ptr_,
            base::Unretained(layer_to_receive_animation),
            0.0));
}

void LayerTreeTest::PostAddLongAnimationToMainThread(
    Layer* layer_to_receive_animation)
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchAddAnimation,
            main_thread_weak_ptr_,
            base::Unretained(layer_to_receive_animation),
            1.0));
}

void LayerTreeTest::PostAddAnimationToMainThreadPlayer(
    AnimationPlayer* player_to_receive_animation)
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchAddAnimationToPlayer,
            main_thread_weak_ptr_,
            base::Unretained(player_to_receive_animation), 0.000004));
}

void LayerTreeTest::PostAddInstantAnimationToMainThreadPlayer(
    AnimationPlayer* player_to_receive_animation)
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchAddAnimationToPlayer,
            main_thread_weak_ptr_,
            base::Unretained(player_to_receive_animation), 0.0));
}

void LayerTreeTest::PostAddLongAnimationToMainThreadPlayer(
    AnimationPlayer* player_to_receive_animation)
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchAddAnimationToPlayer,
            main_thread_weak_ptr_,
            base::Unretained(player_to_receive_animation), 1.0));
}

void LayerTreeTest::PostSetDeferCommitsToMainThread(bool defer_commits)
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchSetDeferCommits,
            main_thread_weak_ptr_, defer_commits));
}

void LayerTreeTest::PostSetNeedsCommitToMainThread()
{
    main_task_runner_->PostTask(FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchSetNeedsCommit,
            main_thread_weak_ptr_));
}

void LayerTreeTest::PostSetNeedsUpdateLayersToMainThread()
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchSetNeedsUpdateLayers,
            main_thread_weak_ptr_));
}

void LayerTreeTest::PostSetNeedsRedrawToMainThread()
{
    main_task_runner_->PostTask(FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchSetNeedsRedraw,
            main_thread_weak_ptr_));
}

void LayerTreeTest::PostSetNeedsRedrawRectToMainThread(
    const gfx::Rect& damage_rect)
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchSetNeedsRedrawRect,
            main_thread_weak_ptr_,
            damage_rect));
}

void LayerTreeTest::PostSetVisibleToMainThread(bool visible)
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(
            &LayerTreeTest::DispatchSetVisible, main_thread_weak_ptr_, visible));
}

void LayerTreeTest::PostSetNextCommitForcesRedrawToMainThread()
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchSetNextCommitForcesRedraw,
            main_thread_weak_ptr_));
}

void LayerTreeTest::PostCompositeImmediatelyToMainThread()
{
    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DispatchCompositeImmediately,
            main_thread_weak_ptr_));
}

void LayerTreeTest::WillBeginTest()
{
    layer_tree_host_->SetLayerTreeHostClientReady();
}

void LayerTreeTest::DoBeginTest()
{
    client_ = LayerTreeHostClientForTesting::Create(this);

    scoped_ptr<FakeExternalBeginFrameSource> external_begin_frame_source;
    if (settings_.use_external_begin_frame_source) {
        external_begin_frame_source.reset(new FakeExternalBeginFrameSource(
            settings_.renderer_settings.refresh_rate));
        external_begin_frame_source_ = external_begin_frame_source.get();
    }

    DCHECK(!impl_thread_ || impl_thread_->task_runner().get());
    layer_tree_host_ = LayerTreeHostForTesting::Create(
        this, client_.get(), shared_bitmap_manager_.get(),
        gpu_memory_buffer_manager_.get(), task_graph_runner_.get(), settings_,
        base::ThreadTaskRunnerHandle::Get(),
        impl_thread_ ? impl_thread_->task_runner() : NULL,
        external_begin_frame_source.Pass());
    ASSERT_TRUE(layer_tree_host_);

    started_ = true;
    beginning_ = true;
    SetupTree();
    WillBeginTest();
    BeginTest();
    beginning_ = false;
    if (end_when_begin_returns_)
        RealEndTest();

    // Allow commits to happen once BeginTest() has had a chance to post tasks
    // so that those tasks will happen before the first commit.
    if (layer_tree_host_) {
        static_cast<LayerTreeHostForTesting*>(layer_tree_host_.get())
            ->set_test_started(true);
    }
}

void LayerTreeTest::SetupTree()
{
    if (!layer_tree_host_->root_layer()) {
        scoped_refptr<Layer> root_layer = Layer::Create(layer_settings_);
        root_layer->SetBounds(gfx::Size(1, 1));
        root_layer->SetIsDrawable(true);
        layer_tree_host_->SetRootLayer(root_layer);
    }

    gfx::Size root_bounds = layer_tree_host_->root_layer()->bounds();
    gfx::Size device_root_bounds = gfx::ScaleToCeiledSize(
        root_bounds, layer_tree_host_->device_scale_factor());
    layer_tree_host_->SetViewportSize(device_root_bounds);
}

void LayerTreeTest::Timeout()
{
    timed_out_ = true;
    EndTest();
}

void LayerTreeTest::RealEndTest()
{
    // TODO(mithro): Make this method only end when not inside an impl frame.
    if (layer_tree_host_ && !timed_out_ && proxy()->MainFrameWillHappenForTesting()) {
        main_task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&LayerTreeTest::RealEndTest, main_thread_weak_ptr_));
        return;
    }

    base::MessageLoop::current()->Quit();
}

void LayerTreeTest::DispatchAddAnimation(Layer* layer_to_receive_animation,
    double animation_duration)
{
    DCHECK(!proxy() || proxy()->IsMainThread());

    if (layer_to_receive_animation) {
        AddOpacityTransitionToLayer(
            layer_to_receive_animation, animation_duration, 0, 0.5, true);
    }
}

void LayerTreeTest::DispatchAddAnimationToPlayer(
    AnimationPlayer* player_to_receive_animation,
    double animation_duration)
{
    DCHECK(!proxy() || proxy()->IsMainThread());

    if (player_to_receive_animation) {
        AddOpacityTransitionToPlayer(player_to_receive_animation,
            animation_duration, 0, 0.5, true);
    }
}

void LayerTreeTest::DispatchSetDeferCommits(bool defer_commits)
{
    DCHECK(!proxy() || proxy()->IsMainThread());

    if (layer_tree_host_)
        layer_tree_host_->SetDeferCommits(defer_commits);
}

void LayerTreeTest::DispatchSetNeedsCommit()
{
    DCHECK(!proxy() || proxy()->IsMainThread());

    if (layer_tree_host_)
        layer_tree_host_->SetNeedsCommit();
}

void LayerTreeTest::DispatchSetNeedsUpdateLayers()
{
    DCHECK(!proxy() || proxy()->IsMainThread());

    if (layer_tree_host_)
        layer_tree_host_->SetNeedsUpdateLayers();
}

void LayerTreeTest::DispatchSetNeedsRedraw()
{
    DCHECK(!proxy() || proxy()->IsMainThread());

    if (layer_tree_host_)
        layer_tree_host_->SetNeedsRedraw();
}

void LayerTreeTest::DispatchSetNeedsRedrawRect(const gfx::Rect& damage_rect)
{
    DCHECK(!proxy() || proxy()->IsMainThread());

    if (layer_tree_host_)
        layer_tree_host_->SetNeedsRedrawRect(damage_rect);
}

void LayerTreeTest::DispatchSetVisible(bool visible)
{
    DCHECK(!proxy() || proxy()->IsMainThread());
    if (layer_tree_host_)
        layer_tree_host_->SetVisible(visible);
}

void LayerTreeTest::DispatchSetNextCommitForcesRedraw()
{
    DCHECK(!proxy() || proxy()->IsMainThread());

    if (layer_tree_host_)
        layer_tree_host_->SetNextCommitForcesRedraw();
}

void LayerTreeTest::DispatchCompositeImmediately()
{
    DCHECK(!proxy() || proxy()->IsMainThread());
    if (layer_tree_host_)
        layer_tree_host_->Composite(base::TimeTicks::Now());
}

void LayerTreeTest::RunTest(bool threaded, bool delegating_renderer)
{
    if (threaded) {
        impl_thread_.reset(new base::Thread("Compositor"));
        ASSERT_TRUE(impl_thread_->Start());
    }

    main_task_runner_ = base::ThreadTaskRunnerHandle::Get();

    shared_bitmap_manager_.reset(new TestSharedBitmapManager);
    gpu_memory_buffer_manager_.reset(new TestGpuMemoryBufferManager);
    task_graph_runner_.reset(new TestTaskGraphRunner);

    delegating_renderer_ = delegating_renderer;

    // Spend less time waiting for BeginFrame because the output is
    // mocked out.
    settings_.renderer_settings.refresh_rate = 200.0;
    settings_.background_animation_rate = 200.0;
    settings_.verify_property_trees = verify_property_trees_;
    InitializeSettings(&settings_);
    InitializeLayerSettings(&layer_settings_);

    main_task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&LayerTreeTest::DoBeginTest, base::Unretained(this)));

    if (timeout_seconds_) {
        timeout_.Reset(base::Bind(&LayerTreeTest::Timeout, base::Unretained(this)));
        main_task_runner_->PostDelayedTask(
            FROM_HERE,
            timeout_.callback(),
            base::TimeDelta::FromSeconds(timeout_seconds_));
    }

    base::MessageLoop::current()->Run();
    DestroyLayerTreeHost();

    timeout_.Cancel();

    ASSERT_FALSE(layer_tree_host_.get());
    client_ = nullptr;
    if (timed_out_) {
        FAIL() << "Test timed out";
        return;
    }
    AfterTest();
}

void LayerTreeTest::RequestNewOutputSurface()
{
    layer_tree_host_->SetOutputSurface(CreateOutputSurface());
}

scoped_ptr<OutputSurface> LayerTreeTest::CreateOutputSurface()
{
    scoped_ptr<FakeOutputSurface> output_surface = CreateFakeOutputSurface();
    DCHECK_EQ(delegating_renderer_,
        output_surface->capabilities().delegated_rendering);
    output_surface_ = output_surface.get();

    if (settings_.use_external_begin_frame_source) {
        DCHECK(external_begin_frame_source_);
        DCHECK(external_begin_frame_source_->is_ready());
    }
    return output_surface.Pass();
}

scoped_ptr<FakeOutputSurface> LayerTreeTest::CreateFakeOutputSurface()
{
    if (delegating_renderer_)
        return FakeOutputSurface::CreateDelegating3d();
    else
        return FakeOutputSurface::Create3d();
}

TestWebGraphicsContext3D* LayerTreeTest::TestContext()
{
    return static_cast<TestContextProvider*>(output_surface_->context_provider())
        ->TestContext3d();
}

int LayerTreeTest::LastCommittedSourceFrameNumber(LayerTreeHostImpl* impl)
    const
{
    if (impl->pending_tree())
        return impl->pending_tree()->source_frame_number();
    if (impl->active_tree())
        return impl->active_tree()->source_frame_number();
    // Source frames start at 0, so this is invalid.
    return -1;
}

void LayerTreeTest::DestroyLayerTreeHost()
{
    if (layer_tree_host_ && layer_tree_host_->root_layer())
        layer_tree_host_->root_layer()->SetLayerTreeHost(NULL);
    layer_tree_host_ = nullptr;
}

LayerTreeHost* LayerTreeTest::layer_tree_host()
{
    // We check for a null proxy here as we sometimes ask for the layer tree host
    // when the proxy does not exist, often for checking settings after a test has
    // completed. For example, LTHPixelResourceTest::RunPixelResourceTest. See
    // elsewhere in this file for other examples.
    DCHECK(!proxy() || proxy()->IsMainThread() || proxy()->IsMainThreadBlocked());
    return layer_tree_host_.get();
}

} // namespace cc
