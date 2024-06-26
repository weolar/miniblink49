// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/layer_tree_host.h"

#include <algorithm>
#include <stack>
#include <string>

#include "base/atomic_sequence_num.h"
#include "base/auto_reset.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/location.h"
#include "base/metrics/histogram.h"
#include "base/single_thread_task_runner.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/thread_task_runner_handle.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/animation/animation_host.h"
#include "cc/animation/animation_registrar.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/base/math_util.h"
#include "cc/debug/devtools_instrumentation.h"
#include "cc/debug/frame_viewer_instrumentation.h"
#include "cc/debug/rendering_stats_instrumentation.h"
#include "cc/input/layer_selection_bound.h"
#include "cc/input/page_scale_animation.h"
#include "cc/input/top_controls_manager.h"
#include "cc/layers/heads_up_display_layer.h"
#include "cc/layers/heads_up_display_layer_impl.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_iterator.h"
#include "cc/layers/painted_scrollbar_layer.h"
#include "cc/resources/ui_resource_request.h"
#include "cc/scheduler/begin_frame_source.h"
#include "cc/trees/draw_property_utils.h"
#include "cc/trees/layer_tree_host_client.h"
#include "cc/trees/layer_tree_host_common.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/single_thread_proxy.h"
#include "cc/trees/thread_proxy.h"
#include "cc/trees/tree_synchronizer.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/geometry/vector2d_conversions.h"

namespace {
static base::StaticAtomicSequenceNumber s_layer_tree_host_sequence_number;
}

namespace cc {

LayerTreeHost::InitParams::InitParams()
{
}

LayerTreeHost::InitParams::~InitParams()
{
}

scoped_ptr<LayerTreeHost> LayerTreeHost::CreateThreaded(
    scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
    InitParams* params)
{
    DCHECK(params->main_task_runner.get());
    DCHECK(impl_task_runner.get());
    DCHECK(params->settings);
    scoped_ptr<LayerTreeHost> layer_tree_host(new LayerTreeHost(params));
    layer_tree_host->InitializeThreaded(
        params->main_task_runner, impl_task_runner,
        params->external_begin_frame_source.Pass());
    return layer_tree_host.Pass();
}

scoped_ptr<LayerTreeHost> LayerTreeHost::CreateSingleThreaded(
    LayerTreeHostSingleThreadClient* single_thread_client,
    InitParams* params)
{
    DCHECK(params->settings);
    scoped_ptr<LayerTreeHost> layer_tree_host(new LayerTreeHost(params));
    layer_tree_host->InitializeSingleThreaded(
        single_thread_client, params->main_task_runner,
        params->external_begin_frame_source.Pass());
    return layer_tree_host.Pass();
}

LayerTreeHost::LayerTreeHost(InitParams* params)
    //: micro_benchmark_controller_(this),
    : next_ui_resource_id_(1)
    , needs_full_tree_sync_(true)
    , needs_meta_info_recomputation_(true)
    , client_(params->client)
    , source_frame_number_(0)
    , meta_information_sequence_number_(1)
    , rendering_stats_instrumentation_(RenderingStatsInstrumentation::Create())
    , output_surface_lost_(true)
    , settings_(*params->settings)
    , debug_state_(settings_.initial_debug_state)
    , top_controls_shrink_blink_size_(false)
    , top_controls_height_(0.f)
    , top_controls_shown_ratio_(0.f)
    , hide_pinch_scrollbars_near_min_scale_(false)
    , device_scale_factor_(1.f)
    , visible_(true)
    , page_scale_factor_(1.f)
    , min_page_scale_factor_(1.f)
    , max_page_scale_factor_(1.f)
    , has_gpu_rasterization_trigger_(false)
    , content_is_suitable_for_gpu_rasterization_(true)
    , gpu_rasterization_histogram_recorded_(false)
    , background_color_(SK_ColorWHITE)
    , has_transparent_background_(false)
    , did_complete_scale_animation_(false)
    , in_paint_layer_contents_(false)
    , id_(s_layer_tree_host_sequence_number.GetNext() + 1)
    , next_commit_forces_redraw_(false)
    , shared_bitmap_manager_(params->shared_bitmap_manager)
    , gpu_memory_buffer_manager_(params->gpu_memory_buffer_manager)
    , task_graph_runner_(params->task_graph_runner)
    , surface_id_namespace_(0u)
    , next_surface_sequence_(1u)
{
    DCHECK(task_graph_runner_);

    if (settings_.accelerated_animation_enabled) {
        if (settings_.use_compositor_animation_timelines) {
            animation_host_ = AnimationHost::Create(ThreadInstance::MAIN);
            animation_host_->SetMutatorHostClient(this);
        } else {
            animation_registrar_ = AnimationRegistrar::Create();
        }
    }

    rendering_stats_instrumentation_->set_record_rendering_stats(
        debug_state_.RecordRenderingStats());
}

void LayerTreeHost::InitializeThreaded(
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
    scoped_ptr<BeginFrameSource> external_begin_frame_source)
{
    InitializeProxy(ThreadProxy::Create(this,
        main_task_runner,
        impl_task_runner,
        external_begin_frame_source.Pass()));
}

void LayerTreeHost::InitializeSingleThreaded(
    LayerTreeHostSingleThreadClient* single_thread_client,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    scoped_ptr<BeginFrameSource> external_begin_frame_source)
{
    InitializeProxy(
        SingleThreadProxy::Create(this,
            single_thread_client,
            main_task_runner,
            external_begin_frame_source.Pass()));
}

void LayerTreeHost::InitializeForTesting(scoped_ptr<Proxy> proxy_for_testing)
{
    InitializeProxy(proxy_for_testing.Pass());
}

void LayerTreeHost::InitializeProxy(scoped_ptr<Proxy> proxy)
{
    TRACE_EVENT0("cc", "LayerTreeHost::InitializeForReal");

    proxy_ = proxy.Pass();
    proxy_->Start();
    if (settings_.accelerated_animation_enabled) {
        if (animation_host_)
            animation_host_->SetSupportsScrollAnimations(
                proxy_->SupportsImplScrolling());
        else
            animation_registrar_->set_supports_scroll_animations(
                proxy_->SupportsImplScrolling());
    }
}

LayerTreeHost::~LayerTreeHost()
{
    TRACE_EVENT0("cc", "LayerTreeHost::~LayerTreeHost");

    if (animation_host_)
        animation_host_->SetMutatorHostClient(nullptr);

    if (root_layer_.get())
        root_layer_->SetLayerTreeHost(NULL);

    DCHECK(swap_promise_monitor_.empty());

    BreakSwapPromises(SwapPromise::COMMIT_FAILS);

    if (proxy_) {
        DCHECK(proxy_->IsMainThread());
        proxy_->Stop();
    }

    // We must clear any pointers into the layer tree prior to destroying it.
    RegisterViewportLayers(NULL, NULL, NULL, NULL);

    if (root_layer_.get()) {
        // The layer tree must be destroyed before the layer tree host. We've
        // made a contract with our animation controllers that the registrar
        // will outlive them, and we must make good.
        root_layer_ = NULL;
    }
}

void LayerTreeHost::SetLayerTreeHostClientReady()
{
    proxy_->SetLayerTreeHostClientReady();
}

void LayerTreeHost::WillBeginMainFrame()
{
    devtools_instrumentation::WillBeginMainThreadFrame(id(),
        source_frame_number());
    client_->WillBeginMainFrame();
}

void LayerTreeHost::DidBeginMainFrame()
{
    client_->DidBeginMainFrame();
}

void LayerTreeHost::BeginMainFrameNotExpectedSoon()
{
    client_->BeginMainFrameNotExpectedSoon();
}

void LayerTreeHost::BeginMainFrame(const BeginFrameArgs& args)
{
    client_->BeginMainFrame(args);
}

void LayerTreeHost::DidStopFlinging()
{
    proxy_->MainThreadHasStoppedFlinging();
}

void LayerTreeHost::Layout()
{
    client_->Layout();
}

// This function commits the LayerTreeHost to an impl tree. When modifying
// this function, keep in mind that the function *runs* on the impl thread! Any
// code that is logically a main thread operation, e.g. deletion of a Layer,
// should be delayed until the LayerTreeHost::CommitComplete, which will run
// after the commit, but on the main thread.
void LayerTreeHost::FinishCommitOnImplThread(LayerTreeHostImpl* host_impl)
{
    DCHECK(proxy_->IsImplThread());

    bool is_new_trace = false;
    TRACE_EVENT_IS_NEW_TRACE(&is_new_trace);
    if (is_new_trace && frame_viewer_instrumentation::IsTracingLayerTreeSnapshots() && root_layer()) {
        LayerTreeHostCommon::CallFunctionForSubtree(
            root_layer(), [](Layer* layer) { layer->DidBeginTracing(); });
    }

    LayerTreeImpl* sync_tree = host_impl->sync_tree();

    if (next_commit_forces_redraw_) {
        sync_tree->ForceRedrawNextActivation();
        next_commit_forces_redraw_ = false;
    }

    sync_tree->set_source_frame_number(source_frame_number());

    if (needs_full_tree_sync_) {
        sync_tree->SetRootLayer(TreeSynchronizer::SynchronizeTrees(
            root_layer(), sync_tree->DetachLayerTree(), sync_tree));
    }
    sync_tree->set_needs_full_tree_sync(needs_full_tree_sync_);
    needs_full_tree_sync_ = false;

    if (hud_layer_.get()) {
        LayerImpl* hud_impl = LayerTreeHostCommon::FindLayerInSubtree(
            sync_tree->root_layer(), hud_layer_->id());
        sync_tree->set_hud_layer(static_cast<HeadsUpDisplayLayerImpl*>(hud_impl));
    } else {
        sync_tree->set_hud_layer(NULL);
    }

    sync_tree->set_background_color(background_color_);
    sync_tree->set_has_transparent_background(has_transparent_background_);

    if (page_scale_layer_.get() && inner_viewport_scroll_layer_.get()) {
        sync_tree->SetViewportLayersFromIds(
            overscroll_elasticity_layer_.get() ? overscroll_elasticity_layer_->id()
                                               : Layer::INVALID_ID,
            page_scale_layer_->id(), inner_viewport_scroll_layer_->id(),
            outer_viewport_scroll_layer_.get() ? outer_viewport_scroll_layer_->id()
                                               : Layer::INVALID_ID);
        DCHECK(inner_viewport_scroll_layer_->IsContainerForFixedPositionLayers());
    } else {
        sync_tree->ClearViewportLayers();
    }

    sync_tree->RegisterSelection(selection_);

    // Setting property trees must happen before pushing the page scale.
    sync_tree->SetPropertyTrees(property_trees_);

    sync_tree->set_hide_pinch_scrollbars_near_min_scale(
        hide_pinch_scrollbars_near_min_scale_);

    sync_tree->PushPageScaleFromMainThread(
        page_scale_factor_, min_page_scale_factor_, max_page_scale_factor_);
    sync_tree->elastic_overscroll()->PushFromMainThread(elastic_overscroll_);
    if (sync_tree->IsActiveTree())
        sync_tree->elastic_overscroll()->PushPendingToActive();

    sync_tree->PassSwapPromises(&swap_promise_list_);

    sync_tree->set_top_controls_shrink_blink_size(
        top_controls_shrink_blink_size_);
    sync_tree->set_top_controls_height(top_controls_height_);
    sync_tree->PushTopControlsFromMainThread(top_controls_shown_ratio_);

    host_impl->SetHasGpuRasterizationTrigger(has_gpu_rasterization_trigger_);
    host_impl->SetContentIsSuitableForGpuRasterization(
        content_is_suitable_for_gpu_rasterization_);
    RecordGpuRasterizationHistogram();

    host_impl->SetViewportSize(device_viewport_size_);
    // TODO(senorblanco): Move this up so that it happens before GPU rasterization
    // properties are set, since those trigger an update of GPU rasterization
    // status, which depends on the device scale factor. (crbug.com/535700)
    sync_tree->SetDeviceScaleFactor(device_scale_factor_);
    host_impl->SetDebugState(debug_state_);
    if (pending_page_scale_animation_) {
        sync_tree->SetPendingPageScaleAnimation(
            pending_page_scale_animation_.Pass());
    }

    if (!ui_resource_request_queue_.empty()) {
        sync_tree->set_ui_resource_request_queue(ui_resource_request_queue_);
        ui_resource_request_queue_.clear();
    }

    DCHECK(!sync_tree->ViewportSizeInvalid());

    sync_tree->set_has_ever_been_drawn(false);

    {
        TRACE_EVENT0("cc", "LayerTreeHost::PushProperties");
        TreeSynchronizer::PushProperties(root_layer(), sync_tree->root_layer());

        if (animation_host_) {
            DCHECK(host_impl->animation_host());
            animation_host_->PushPropertiesTo(host_impl->animation_host());
        }
    }

    // This must happen after synchronizing property trees and after push
    // properties, which updates property tree indices.
    sync_tree->UpdatePropertyTreeScrollingAndAnimationFromMainThread();

    //micro_benchmark_controller_.ScheduleImplBenchmarks(host_impl);
}

void LayerTreeHost::WillCommit()
{
    OnCommitForSwapPromises();
    client_->WillCommit();
}

void LayerTreeHost::UpdateHudLayer()
{
    if (debug_state_.ShowHudInfo()) {
#ifndef NOT_QB_AERO
        if (!hud_layer_.get()) {
            LayerSettings hud_layer_settings;
            hud_layer_settings.use_compositor_animation_timelines = settings_.use_compositor_animation_timelines;
            hud_layer_ = HeadsUpDisplayLayer::Create(hud_layer_settings);
            hud_layer_->SetTopInset(hud_layer_top_inset_);
        }
#else
        if (!hud_layer_.get()) {
            LayerSettings hud_layer_settings;
            hud_layer_settings.use_compositor_animation_timelines = settings_.use_compositor_animation_timelines;
            hud_layer_ = HeadsUpDisplayLayer::Create(hud_layer_settings);
        }
#endif // NOT_QB_AERO
        if (root_layer_.get() && !hud_layer_->parent())
            root_layer_->AddChild(hud_layer_);
    } else if (hud_layer_.get()) {
        hud_layer_->RemoveFromParent();
        hud_layer_ = NULL;
    }
}

void LayerTreeHost::CommitComplete()
{
    source_frame_number_++;
    client_->DidCommit();
    if (did_complete_scale_animation_) {
        client_->DidCompletePageScaleAnimation();
        did_complete_scale_animation_ = false;
    }
}

void LayerTreeHost::SetOutputSurface(scoped_ptr<OutputSurface> surface)
{
    TRACE_EVENT0("cc", "LayerTreeHost::SetOutputSurface");
    DCHECK(output_surface_lost_);
    DCHECK(surface);

    DCHECK(!new_output_surface_);
    new_output_surface_ = surface.Pass();
    proxy_->SetOutputSurface(new_output_surface_.get());
}

scoped_ptr<OutputSurface> LayerTreeHost::ReleaseOutputSurface()
{
    DCHECK(!visible_);
    DCHECK(!output_surface_lost_);

    DidLoseOutputSurface();
    proxy_->ReleaseOutputSurface();
    return current_output_surface_.Pass();
}

void LayerTreeHost::RequestNewOutputSurface()
{
    client_->RequestNewOutputSurface();
}

void LayerTreeHost::DidInitializeOutputSurface()
{
    DCHECK(new_output_surface_);
    output_surface_lost_ = false;
    current_output_surface_ = new_output_surface_.Pass();
    client_->DidInitializeOutputSurface();
}

void LayerTreeHost::DidFailToInitializeOutputSurface()
{
    DCHECK(output_surface_lost_);
    DCHECK(new_output_surface_);
    // Note: It is safe to drop all output surface references here as
    // LayerTreeHostImpl will not keep a pointer to either the old or
    // new output surface after failing to initialize the new one.
    current_output_surface_ = nullptr;
    new_output_surface_ = nullptr;
    client_->DidFailToInitializeOutputSurface();
}

scoped_ptr<LayerTreeHostImpl> LayerTreeHost::CreateLayerTreeHostImpl(
    LayerTreeHostImplClient* client)
{
    DCHECK(proxy_->IsImplThread());
    scoped_ptr<LayerTreeHostImpl> host_impl = LayerTreeHostImpl::Create(
        settings_, client, proxy_.get(), rendering_stats_instrumentation_.get(),
        shared_bitmap_manager_, gpu_memory_buffer_manager_, task_graph_runner_,
        id_);
    host_impl->SetHasGpuRasterizationTrigger(has_gpu_rasterization_trigger_);
    host_impl->SetContentIsSuitableForGpuRasterization(
        content_is_suitable_for_gpu_rasterization_);
    shared_bitmap_manager_ = NULL;
    gpu_memory_buffer_manager_ = NULL;
    task_graph_runner_ = NULL;
    top_controls_manager_weak_ptr_ = host_impl->top_controls_manager()->AsWeakPtr();
    input_handler_weak_ptr_ = host_impl->AsWeakPtr();
    return host_impl.Pass();
}

void LayerTreeHost::DidLoseOutputSurface()
{
    TRACE_EVENT0("cc", "LayerTreeHost::DidLoseOutputSurface");
    DCHECK(proxy_->IsMainThread());

    if (output_surface_lost_)
        return;

    output_surface_lost_ = true;
    SetNeedsCommit();
}

void LayerTreeHost::FinishAllRendering()
{
    proxy_->FinishAllRendering();
}

void LayerTreeHost::SetDeferCommits(bool defer_commits)
{
    proxy_->SetDeferCommits(defer_commits);
}

void LayerTreeHost::SetNeedsDisplayOnAllLayers()
{
    std::stack<Layer*> layer_stack;
    layer_stack.push(root_layer());
    while (!layer_stack.empty()) {
        Layer* current_layer = layer_stack.top();
        layer_stack.pop();
        current_layer->SetNeedsDisplay();
        for (unsigned int i = 0; i < current_layer->children().size(); i++) {
            layer_stack.push(current_layer->child_at(i));
        }
    }
}

const RendererCapabilities& LayerTreeHost::GetRendererCapabilities() const
{
    return proxy_->GetRendererCapabilities();
}

void LayerTreeHost::SetNeedsAnimate()
{
    proxy_->SetNeedsAnimate();
    NotifySwapPromiseMonitorsOfSetNeedsCommit();
}

void LayerTreeHost::SetNeedsUpdateLayers()
{
    proxy_->SetNeedsUpdateLayers();
    NotifySwapPromiseMonitorsOfSetNeedsCommit();
}

void LayerTreeHost::SetPropertyTreesNeedRebuild()
{
    property_trees_.needs_rebuild = true;
    SetNeedsUpdateLayers();
}

void LayerTreeHost::SetNeedsCommit()
{
    proxy_->SetNeedsCommit();
    NotifySwapPromiseMonitorsOfSetNeedsCommit();
}

void LayerTreeHost::SetNeedsFullTreeSync()
{
    needs_full_tree_sync_ = true;
    needs_meta_info_recomputation_ = true;

    property_trees_.needs_rebuild = true;
    SetNeedsCommit();
}

void LayerTreeHost::SetNeedsMetaInfoRecomputation(bool needs_recomputation)
{
    needs_meta_info_recomputation_ = needs_recomputation;
}

void LayerTreeHost::SetNeedsRedraw()
{
    SetNeedsRedrawRect(gfx::Rect(device_viewport_size_));
}

void LayerTreeHost::SetNeedsRedrawRect(const gfx::Rect& damage_rect)
{
    proxy_->SetNeedsRedraw(damage_rect);
}

bool LayerTreeHost::CommitRequested() const
{
    return proxy_->CommitRequested();
}

bool LayerTreeHost::BeginMainFrameRequested() const
{
    return proxy_->BeginMainFrameRequested();
}

void LayerTreeHost::SetNextCommitWaitsForActivation()
{
    proxy_->SetNextCommitWaitsForActivation();
}

void LayerTreeHost::SetNextCommitForcesRedraw()
{
    next_commit_forces_redraw_ = true;
    proxy_->SetNeedsUpdateLayers();
}

void LayerTreeHost::SetAnimationEvents(
    scoped_ptr<AnimationEventsVector> events)
{
    DCHECK(proxy_->IsMainThread());
    if (animation_host_)
        animation_host_->SetAnimationEvents(events.Pass());
    else
        animation_registrar_->SetAnimationEvents(events.Pass());
}

void LayerTreeHost::SetRootLayer(scoped_refptr<Layer> root_layer)
{
    if (root_layer_.get() == root_layer.get())
        return;

    if (root_layer_.get())
        root_layer_->SetLayerTreeHost(NULL);
    root_layer_ = root_layer;
    if (root_layer_.get()) {
        DCHECK(!root_layer_->parent());
        root_layer_->SetLayerTreeHost(this);
    }

    if (hud_layer_.get())
        hud_layer_->RemoveFromParent();

    // Reset gpu rasterization flag.
    // This flag is sticky until a new tree comes along.
    content_is_suitable_for_gpu_rasterization_ = true;
    gpu_rasterization_histogram_recorded_ = false;

    SetNeedsFullTreeSync();
}

void LayerTreeHost::SetDebugState(const LayerTreeDebugState& debug_state)
{
    LayerTreeDebugState new_debug_state = LayerTreeDebugState::Unite(settings_.initial_debug_state, debug_state);

    if (LayerTreeDebugState::Equal(debug_state_, new_debug_state))
        return;

    debug_state_ = new_debug_state;

    rendering_stats_instrumentation_->set_record_rendering_stats(
        debug_state_.RecordRenderingStats());

    SetNeedsCommit();
}

void LayerTreeHost::SetHasGpuRasterizationTrigger(bool has_trigger)
{
    if (has_trigger == has_gpu_rasterization_trigger_)
        return;

    has_gpu_rasterization_trigger_ = has_trigger;
    TRACE_EVENT_INSTANT1("cc",
        "LayerTreeHost::SetHasGpuRasterizationTrigger",
        TRACE_EVENT_SCOPE_THREAD,
        "has_trigger",
        has_gpu_rasterization_trigger_);
}

void LayerTreeHost::SetViewportSize(const gfx::Size& device_viewport_size)
{
    if (device_viewport_size == device_viewport_size_)
        return;

    device_viewport_size_ = device_viewport_size;

    SetPropertyTreesNeedRebuild();
    SetNeedsCommit();
}

void LayerTreeHost::SetTopControlsHeight(float height, bool shrink)
{
    if (top_controls_height_ == height && top_controls_shrink_blink_size_ == shrink)
        return;

    top_controls_height_ = height;
    top_controls_shrink_blink_size_ = shrink;
    SetNeedsCommit();
}

void LayerTreeHost::SetTopControlsShownRatio(float ratio)
{
    if (top_controls_shown_ratio_ == ratio)
        return;

    top_controls_shown_ratio_ = ratio;
    SetNeedsCommit();
}

void LayerTreeHost::ApplyPageScaleDeltaFromImplSide(float page_scale_delta)
{
    DCHECK(CommitRequested());
    if (page_scale_delta == 1.f)
        return;
    page_scale_factor_ *= page_scale_delta;
    SetPropertyTreesNeedRebuild();
}

void LayerTreeHost::SetPageScaleFactorAndLimits(float page_scale_factor,
    float min_page_scale_factor,
    float max_page_scale_factor)
{
    if (page_scale_factor == page_scale_factor_ && min_page_scale_factor == min_page_scale_factor_ && max_page_scale_factor == max_page_scale_factor_)
        return;

    page_scale_factor_ = page_scale_factor;
    min_page_scale_factor_ = min_page_scale_factor;
    max_page_scale_factor_ = max_page_scale_factor;
    SetPropertyTreesNeedRebuild();
    SetNeedsCommit();
}

void LayerTreeHost::SetVisible(bool visible)
{
    if (visible_ == visible)
        return;
    visible_ = visible;
    proxy_->SetVisible(visible);
}

void LayerTreeHost::SetThrottleFrameProduction(bool throttle)
{
    proxy_->SetThrottleFrameProduction(throttle);
}

void LayerTreeHost::StartPageScaleAnimation(const gfx::Vector2d& target_offset,
    bool use_anchor,
    float scale,
    base::TimeDelta duration)
{
    pending_page_scale_animation_.reset(
        new PendingPageScaleAnimation(
            target_offset,
            use_anchor,
            scale,
            duration));

    SetNeedsCommit();
}

void LayerTreeHost::NotifyInputThrottledUntilCommit()
{
    proxy_->NotifyInputThrottledUntilCommit();
}

void LayerTreeHost::LayoutAndUpdateLayers()
{
    DCHECK(!proxy_->HasImplThread());
    // This function is only valid when not using the scheduler.
    DCHECK(!settings_.single_thread_proxy_scheduler);
    SingleThreadProxy* proxy = static_cast<SingleThreadProxy*>(proxy_.get());

    SetLayerTreeHostClientReady();
    proxy->LayoutAndUpdateLayers();
}

void LayerTreeHost::Composite(base::TimeTicks frame_begin_time)
{
    DCHECK(!proxy_->HasImplThread());
    // This function is only valid when not using the scheduler.
    DCHECK(!settings_.single_thread_proxy_scheduler);
    SingleThreadProxy* proxy = static_cast<SingleThreadProxy*>(proxy_.get());

    SetLayerTreeHostClientReady();
    proxy->CompositeImmediately(frame_begin_time);
}

bool LayerTreeHost::UpdateLayers()
{
    DCHECK(!output_surface_lost_);
    if (!root_layer())
        return false;
    DCHECK(!root_layer()->parent());
    bool result = DoUpdateLayers(root_layer());
    //micro_benchmark_controller_.DidUpdateLayers();
    return result || next_commit_forces_redraw_;
}

void LayerTreeHost::DidCompletePageScaleAnimation()
{
    did_complete_scale_animation_ = true;
}

static Layer* FindFirstScrollableLayer(Layer* layer)
{
    if (!layer)
        return NULL;

    if (layer->scrollable())
        return layer;

    for (size_t i = 0; i < layer->children().size(); ++i) {
        Layer* found = FindFirstScrollableLayer(layer->children()[i].get());
        if (found)
            return found;
    }

    return NULL;
}

void LayerTreeHost::RecordGpuRasterizationHistogram()
{
    // Gpu rasterization is only supported for Renderer compositors.
    // Checking for proxy_->HasImplThread() to exclude Browser compositors.
    if (gpu_rasterization_histogram_recorded_ || !proxy_->HasImplThread())
        return;

    // Record how widely gpu rasterization is enabled.
    // This number takes device/gpu whitelisting/backlisting into account.
    // Note that we do not consider the forced gpu rasterization mode, which is
    // mostly used for debugging purposes.
    UMA_HISTOGRAM_BOOLEAN("Renderer4.GpuRasterizationEnabled",
        settings_.gpu_rasterization_enabled);
    if (settings_.gpu_rasterization_enabled) {
        UMA_HISTOGRAM_BOOLEAN("Renderer4.GpuRasterizationTriggered",
            has_gpu_rasterization_trigger_);
        UMA_HISTOGRAM_BOOLEAN("Renderer4.GpuRasterizationSuitableContent",
            content_is_suitable_for_gpu_rasterization_);
        // Record how many pages actually get gpu rasterization when enabled.
        UMA_HISTOGRAM_BOOLEAN("Renderer4.GpuRasterizationUsed",
            (has_gpu_rasterization_trigger_ && content_is_suitable_for_gpu_rasterization_));
    }

    gpu_rasterization_histogram_recorded_ = true;
}

bool LayerTreeHost::UsingSharedMemoryResources()
{
    return GetRendererCapabilities().using_shared_memory_resources;
}

bool LayerTreeHost::DoUpdateLayers(Layer* root_layer)
{
    TRACE_EVENT1("cc", "LayerTreeHost::DoUpdateLayers", "source_frame_number",
        source_frame_number());

    UpdateHudLayer();

    Layer* root_scroll = FindFirstScrollableLayer(root_layer);
    Layer* page_scale_layer = page_scale_layer_.get();
    if (!page_scale_layer && root_scroll)
        page_scale_layer = root_scroll->parent();

    if (hud_layer_.get()) {
        hud_layer_->PrepareForCalculateDrawProperties(device_viewport_size(),
            device_scale_factor_);
    }

    bool can_render_to_separate_surface = true;

    TRACE_EVENT0("cc", "LayerTreeHost::UpdateLayers::CalcDrawProps");

    LayerTreeHostCommon::PreCalculateMetaInformation(root_layer);

    bool preserves_2d_axis_alignment = false;
    gfx::Transform identity_transform;
    LayerList update_layer_list;

    LayerTreeHostCommon::UpdateRenderSurfaces(
        root_layer, can_render_to_separate_surface, identity_transform,
        preserves_2d_axis_alignment);
    {
        TRACE_EVENT0(TRACE_DISABLED_BY_DEFAULT("cc.debug.cdp-perf"),
            "LayerTreeHostCommon::ComputeVisibleRectsWithPropertyTrees");
        BuildPropertyTreesAndComputeVisibleRects(
            root_layer, page_scale_layer, inner_viewport_scroll_layer_.get(),
            outer_viewport_scroll_layer_.get(), page_scale_factor_,
            device_scale_factor_, gfx::Rect(device_viewport_size_),
            identity_transform, &property_trees_, &update_layer_list);
    }

    for (const auto& layer : update_layer_list)
        layer->SavePaintProperties();

    base::AutoReset<bool> painting(&in_paint_layer_contents_, true);
    bool did_paint_content = false;
    for (const auto& layer : update_layer_list) {
        did_paint_content |= layer->Update();
        content_is_suitable_for_gpu_rasterization_ &= layer->IsSuitableForGpuRasterization();
    }
    return did_paint_content;
}

void LayerTreeHost::ApplyScrollAndScale(ScrollAndScaleSet* info)
{
    ScopedPtrVector<SwapPromise>::iterator it = info->swap_promises.begin();
    for (; it != info->swap_promises.end(); ++it) {
        scoped_ptr<SwapPromise> swap_promise(info->swap_promises.take(it));
        TRACE_EVENT_WITH_FLOW1("input,benchmark",
            "LatencyInfo.Flow",
            TRACE_ID_DONT_MANGLE(swap_promise->TraceId()),
            TRACE_EVENT_FLAG_FLOW_IN | TRACE_EVENT_FLAG_FLOW_OUT,
            "step", "Main thread scroll update");
        QueueSwapPromise(swap_promise.Pass());
    }

    gfx::Vector2dF inner_viewport_scroll_delta;
    gfx::Vector2dF outer_viewport_scroll_delta;

    if (root_layer_.get()) {
        for (size_t i = 0; i < info->scrolls.size(); ++i) {
            Layer* layer = LayerTreeHostCommon::FindLayerInSubtree(
                root_layer_.get(), info->scrolls[i].layer_id);
            if (!layer)
                continue;
            if (layer == outer_viewport_scroll_layer_.get()) {
                outer_viewport_scroll_delta += info->scrolls[i].scroll_delta;
            } else if (layer == inner_viewport_scroll_layer_.get()) {
                inner_viewport_scroll_delta += info->scrolls[i].scroll_delta;
            } else {
                layer->SetScrollOffsetFromImplSide(
                    gfx::ScrollOffsetWithDelta(layer->scroll_offset(),
                        info->scrolls[i].scroll_delta));
            }
            SetNeedsUpdateLayers();
        }
    }

    if (!inner_viewport_scroll_delta.IsZero() || !outer_viewport_scroll_delta.IsZero() || info->page_scale_delta != 1.f || !info->elastic_overscroll_delta.IsZero() || info->top_controls_delta) {
        // Preemptively apply the scroll offset and scale delta here before sending
        // it to the client.  If the client comes back and sets it to the same
        // value, then the layer can early out without needing a full commit.
        if (inner_viewport_scroll_layer_.get()) {
            inner_viewport_scroll_layer_->SetScrollOffsetFromImplSide(
                gfx::ScrollOffsetWithDelta(
                    inner_viewport_scroll_layer_->scroll_offset(),
                    inner_viewport_scroll_delta));
        }

        if (outer_viewport_scroll_layer_.get()) {
            outer_viewport_scroll_layer_->SetScrollOffsetFromImplSide(
                gfx::ScrollOffsetWithDelta(
                    outer_viewport_scroll_layer_->scroll_offset(),
                    outer_viewport_scroll_delta));
        }

        ApplyPageScaleDeltaFromImplSide(info->page_scale_delta);
        elastic_overscroll_ += info->elastic_overscroll_delta;
        // TODO(ccameron): pass the elastic overscroll here so that input events
        // may be translated appropriately.
        client_->ApplyViewportDeltas(
            inner_viewport_scroll_delta, outer_viewport_scroll_delta,
            info->elastic_overscroll_delta, info->page_scale_delta,
            info->top_controls_delta);
        SetNeedsUpdateLayers();
    }
}

void LayerTreeHost::SetDeviceScaleFactor(float device_scale_factor)
{
    if (device_scale_factor == device_scale_factor_)
        return;
    device_scale_factor_ = device_scale_factor;

    property_trees_.needs_rebuild = true;
    SetNeedsCommit();
}

void LayerTreeHost::UpdateTopControlsState(TopControlsState constraints,
    TopControlsState current,
    bool animate)
{
    // Top controls are only used in threaded mode.
    proxy_->ImplThreadTaskRunner()->PostTask(
        FROM_HERE,
        base::Bind(&TopControlsManager::UpdateTopControlsState,
            top_controls_manager_weak_ptr_,
            constraints,
            current,
            animate));
}

void LayerTreeHost::AnimateLayers(base::TimeTicks monotonic_time)
{
    if (!settings_.accelerated_animation_enabled)
        return;

    AnimationEventsVector events;
    if (animation_host_) {
        if (animation_host_->AnimateLayers(monotonic_time))
            animation_host_->UpdateAnimationState(true, &events);
    } else {
        if (animation_registrar_->AnimateLayers(monotonic_time))
            animation_registrar_->UpdateAnimationState(true, &events);
    }

    if (!events.empty())
        property_trees_.needs_rebuild = true;
}

UIResourceId LayerTreeHost::CreateUIResource(UIResourceClient* client)
{
    DCHECK(client);

    UIResourceId next_id = next_ui_resource_id_++;
    DCHECK(ui_resource_client_map_.find(next_id) == ui_resource_client_map_.end());

    bool resource_lost = false;
    UIResourceRequest request(UIResourceRequest::UI_RESOURCE_CREATE, next_id,
        client->GetBitmap(next_id, resource_lost));
    ui_resource_request_queue_.push_back(request);

    UIResourceClientData data;
    data.client = client;
    data.size = request.GetBitmap().GetSize();

    ui_resource_client_map_[request.GetId()] = data;
    return request.GetId();
}

// Deletes a UI resource.  May safely be called more than once.
void LayerTreeHost::DeleteUIResource(UIResourceId uid)
{
    UIResourceClientMap::iterator iter = ui_resource_client_map_.find(uid);
    if (iter == ui_resource_client_map_.end())
        return;

    UIResourceRequest request(UIResourceRequest::UI_RESOURCE_DELETE, uid);
    ui_resource_request_queue_.push_back(request);
    ui_resource_client_map_.erase(iter);
}

void LayerTreeHost::RecreateUIResources()
{
    for (UIResourceClientMap::iterator iter = ui_resource_client_map_.begin();
         iter != ui_resource_client_map_.end();
         ++iter) {
        UIResourceId uid = iter->first;
        const UIResourceClientData& data = iter->second;
        bool resource_lost = true;
        UIResourceRequest request(UIResourceRequest::UI_RESOURCE_CREATE, uid,
            data.client->GetBitmap(uid, resource_lost));
        ui_resource_request_queue_.push_back(request);
    }
}

// Returns the size of a resource given its id.
gfx::Size LayerTreeHost::GetUIResourceSize(UIResourceId uid) const
{
    UIResourceClientMap::const_iterator iter = ui_resource_client_map_.find(uid);
    if (iter == ui_resource_client_map_.end())
        return gfx::Size();

    const UIResourceClientData& data = iter->second;
    return data.size;
}

void LayerTreeHost::RegisterViewportLayers(
    scoped_refptr<Layer> overscroll_elasticity_layer,
    scoped_refptr<Layer> page_scale_layer,
    scoped_refptr<Layer> inner_viewport_scroll_layer,
    scoped_refptr<Layer> outer_viewport_scroll_layer)
{
    DCHECK_IMPLIES(inner_viewport_scroll_layer,
        inner_viewport_scroll_layer != outer_viewport_scroll_layer);
    overscroll_elasticity_layer_ = overscroll_elasticity_layer;
    page_scale_layer_ = page_scale_layer;
    inner_viewport_scroll_layer_ = inner_viewport_scroll_layer;
    outer_viewport_scroll_layer_ = outer_viewport_scroll_layer;
}

void LayerTreeHost::RegisterSelection(const LayerSelection& selection)
{
    if (selection_ == selection)
        return;

    selection_ = selection;
    SetNeedsCommit();
}

int LayerTreeHost::ScheduleMicroBenchmark(
    const std::string& benchmark_name,
    scoped_ptr<base::Value> value,
    const MicroBenchmark::DoneCallback& callback)
{
    DebugBreak();
    return 0;
    //   return micro_benchmark_controller_.ScheduleRun(
    //       benchmark_name, value.Pass(), callback);
}

bool LayerTreeHost::SendMessageToMicroBenchmark(int id,
    scoped_ptr<base::Value> value)
{
    //return micro_benchmark_controller_.SendMessage(id, value.Pass());
    DebugBreak();
    return false;
}

void LayerTreeHost::InsertSwapPromiseMonitor(SwapPromiseMonitor* monitor)
{
    swap_promise_monitor_.insert(monitor);
}

void LayerTreeHost::RemoveSwapPromiseMonitor(SwapPromiseMonitor* monitor)
{
    swap_promise_monitor_.erase(monitor);
}

void LayerTreeHost::NotifySwapPromiseMonitorsOfSetNeedsCommit()
{
    std::set<SwapPromiseMonitor*>::iterator it = swap_promise_monitor_.begin();
    for (; it != swap_promise_monitor_.end(); it++)
        (*it)->OnSetNeedsCommitOnMain();
}

void LayerTreeHost::QueueSwapPromise(scoped_ptr<SwapPromise> swap_promise)
{
    DCHECK(swap_promise);
    swap_promise_list_.push_back(swap_promise.Pass());
}

void LayerTreeHost::BreakSwapPromises(SwapPromise::DidNotSwapReason reason)
{
    for (auto* swap_promise : swap_promise_list_)
        swap_promise->DidNotSwap(reason);
    swap_promise_list_.clear();
}

void LayerTreeHost::OnCommitForSwapPromises()
{
    for (auto* swap_promise : swap_promise_list_)
        swap_promise->OnCommit();
}

void LayerTreeHost::set_surface_id_namespace(uint32_t id_namespace)
{
    surface_id_namespace_ = id_namespace;
}

SurfaceSequence LayerTreeHost::CreateSurfaceSequence()
{
    return SurfaceSequence(surface_id_namespace_, next_surface_sequence_++);
}

void LayerTreeHost::SetChildrenNeedBeginFrames(
    bool children_need_begin_frames) const
{
    proxy_->SetChildrenNeedBeginFrames(children_need_begin_frames);
}

void LayerTreeHost::SendBeginFramesToChildren(
    const BeginFrameArgs& args) const
{
    client_->SendBeginFramesToChildren(args);
}

void LayerTreeHost::SetAuthoritativeVSyncInterval(
    const base::TimeDelta& interval)
{
    proxy_->SetAuthoritativeVSyncInterval(interval);
}

void LayerTreeHost::RecordFrameTimingEvents(
    scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
    scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events)
{
    client_->RecordFrameTimingEvents(composite_events.Pass(),
        main_frame_events.Pass());
}

Layer* LayerTreeHost::LayerById(int id) const
{
    LayerIdMap::const_iterator iter = layer_id_map_.find(id);
    return iter != layer_id_map_.end() ? iter->second : NULL;
}

void LayerTreeHost::RegisterLayer(Layer* layer)
{
    DCHECK(!LayerById(layer->id()));
    DCHECK(!in_paint_layer_contents_);
    layer_id_map_[layer->id()] = layer;
    if (animation_host_)
        animation_host_->RegisterLayer(layer->id(), LayerTreeType::ACTIVE);
}

void LayerTreeHost::UnregisterLayer(Layer* layer)
{
    DCHECK(LayerById(layer->id()));
    DCHECK(!in_paint_layer_contents_);
    if (animation_host_)
        animation_host_->UnregisterLayer(layer->id(), LayerTreeType::ACTIVE);
    layer_id_map_.erase(layer->id());
}

bool LayerTreeHost::IsLayerInTree(int layer_id, LayerTreeType tree_type) const
{
    return tree_type == LayerTreeType::ACTIVE && LayerById(layer_id);
}

void LayerTreeHost::SetMutatorsNeedCommit()
{
    SetNeedsCommit();
}

void LayerTreeHost::SetLayerFilterMutated(int layer_id,
    LayerTreeType tree_type,
    const FilterOperations& filters)
{
    LayerAnimationValueObserver* layer = LayerById(layer_id);
    DCHECK(layer);
    layer->OnFilterAnimated(filters);
}

void LayerTreeHost::SetLayerOpacityMutated(int layer_id,
    LayerTreeType tree_type,
    float opacity)
{
    LayerAnimationValueObserver* layer = LayerById(layer_id);
    DCHECK(layer);
    layer->OnOpacityAnimated(opacity);
}

void LayerTreeHost::SetLayerTransformMutated(int layer_id,
    LayerTreeType tree_type,
    const gfx::Transform& transform)
{
    LayerAnimationValueObserver* layer = LayerById(layer_id);
    DCHECK(layer);
    layer->OnTransformAnimated(transform);
}

void LayerTreeHost::SetLayerScrollOffsetMutated(
    int layer_id,
    LayerTreeType tree_type,
    const gfx::ScrollOffset& scroll_offset)
{
    LayerAnimationValueObserver* layer = LayerById(layer_id);
    DCHECK(layer);
    layer->OnScrollOffsetAnimated(scroll_offset);
}

void LayerTreeHost::LayerTransformIsPotentiallyAnimatingChanged(
    int layer_id,
    LayerTreeType tree_type,
    bool is_animating)
{
    LayerAnimationValueObserver* layer = LayerById(layer_id);
    DCHECK(layer);
    layer->OnTransformIsPotentiallyAnimatingChanged(is_animating);
}

gfx::ScrollOffset LayerTreeHost::GetScrollOffsetForAnimation(
    int layer_id) const
{
    LayerAnimationValueProvider* layer = LayerById(layer_id);
    DCHECK(layer);
    return layer->ScrollOffsetForAnimation();
}

bool LayerTreeHost::ScrollOffsetAnimationWasInterrupted(
    const Layer* layer) const
{
    return animation_host_
        ? animation_host_->ScrollOffsetAnimationWasInterrupted(layer->id())
        : false;
}

bool LayerTreeHost::IsAnimatingFilterProperty(const Layer* layer) const
{
    return animation_host_
        ? animation_host_->IsAnimatingFilterProperty(layer->id(),
            LayerTreeType::ACTIVE)
        : false;
}

bool LayerTreeHost::IsAnimatingOpacityProperty(const Layer* layer) const
{
    return animation_host_
        ? animation_host_->IsAnimatingOpacityProperty(
            layer->id(), LayerTreeType::ACTIVE)
        : false;
}

bool LayerTreeHost::IsAnimatingTransformProperty(const Layer* layer) const
{
    return animation_host_
        ? animation_host_->IsAnimatingTransformProperty(
            layer->id(), LayerTreeType::ACTIVE)
        : false;
}

bool LayerTreeHost::HasPotentiallyRunningFilterAnimation(
    const Layer* layer) const
{
    return animation_host_
        ? animation_host_->HasPotentiallyRunningFilterAnimation(
            layer->id(), LayerTreeType::ACTIVE)
        : false;
}

bool LayerTreeHost::HasPotentiallyRunningOpacityAnimation(
    const Layer* layer) const
{
    return animation_host_
        ? animation_host_->HasPotentiallyRunningOpacityAnimation(
            layer->id(), LayerTreeType::ACTIVE)
        : false;
}

bool LayerTreeHost::HasPotentiallyRunningTransformAnimation(
    const Layer* layer) const
{
    return animation_host_
        ? animation_host_->HasPotentiallyRunningTransformAnimation(
            layer->id(), LayerTreeType::ACTIVE)
        : false;
}

bool LayerTreeHost::HasOnlyTranslationTransforms(const Layer* layer) const
{
    return animation_host_
        ? animation_host_->HasOnlyTranslationTransforms(
            layer->id(), LayerTreeType::ACTIVE)
        : false;
}

bool LayerTreeHost::MaximumTargetScale(const Layer* layer,
    float* max_scale) const
{
    return animation_host_
        ? animation_host_->MaximumTargetScale(
            layer->id(), LayerTreeType::ACTIVE, max_scale)
        : false;
}

bool LayerTreeHost::AnimationStartScale(const Layer* layer,
    float* start_scale) const
{
    return animation_host_
        ? animation_host_->AnimationStartScale(
            layer->id(), LayerTreeType::ACTIVE, start_scale)
        : false;
}

bool LayerTreeHost::HasAnyAnimationTargetingProperty(
    const Layer* layer,
    Animation::TargetProperty property) const
{
    return animation_host_
        ? animation_host_->HasAnyAnimationTargetingProperty(layer->id(),
            property)
        : false;
}

bool LayerTreeHost::AnimationsPreserveAxisAlignment(const Layer* layer) const
{
    return animation_host_
        ? animation_host_->AnimationsPreserveAxisAlignment(layer->id())
        : true;
}

bool LayerTreeHost::HasAnyAnimation(const Layer* layer) const
{
    return animation_host_ ? animation_host_->HasAnyAnimation(layer->id())
                           : false;
}

bool LayerTreeHost::HasActiveAnimation(const Layer* layer) const
{
    return animation_host_ ? animation_host_->HasActiveAnimation(layer->id())
                           : false;
}

#ifndef NOT_QB_AERO
void LayerTreeHost::SetHudLayerTopInset(int inset)
{
    hud_layer_top_inset_ = inset;
    if (hud_layer_.get())
        hud_layer_->SetTopInset(hud_layer_top_inset_);
}
#endif // NOT_QB_AERO

} // namespace cc
