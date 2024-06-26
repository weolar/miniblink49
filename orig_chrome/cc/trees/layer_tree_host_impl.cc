// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/layer_tree_host_impl.h"

#include <algorithm>
#include <limits>
#include <map>
#include <set>

#include "base/basictypes.h"
#include "base/containers/hash_tables.h"
#if USING_VC6RT != 1
#include "base/containers/small_map.h"
#endif
#include "base/json/json_writer.h"
#include "base/metrics/histogram.h"
#include "base/numerics/safe_conversions.h"
#include "base/stl_util.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/animation/animation_host.h"
#include "cc/animation/animation_id_provider.h"
#include "cc/animation/scroll_offset_animation_curve.h"
#include "cc/animation/scrollbar_animation_controller.h"
#include "cc/animation/timing_function.h"
#include "cc/base/histograms.h"
#include "cc/base/math_util.h"
#include "cc/debug/benchmark_instrumentation.h"
#include "cc/debug/debug_rect_history.h"
#include "cc/debug/devtools_instrumentation.h"
#include "cc/debug/frame_rate_counter.h"
#include "cc/debug/frame_viewer_instrumentation.h"
#include "cc/debug/rendering_stats_instrumentation.h"
#include "cc/debug/traced_value.h"
#include "cc/input/page_scale_animation.h"
#include "cc/input/scroll_elasticity_helper.h"
#include "cc/input/scroll_state.h"
#include "cc/input/top_controls_manager.h"
#include "cc/layers/append_quads_data.h"
#include "cc/layers/heads_up_display_layer_impl.h"
#include "cc/layers/layer_impl.h"
#include "cc/layers/layer_iterator.h"
#include "cc/layers/painted_scrollbar_layer_impl.h"
#include "cc/layers/render_surface_impl.h"
#include "cc/layers/scrollbar_layer_impl_base.h"
#include "cc/layers/viewport.h"
#include "cc/output/compositor_frame_metadata.h"
#include "cc/output/copy_output_request.h"
#include "cc/output/delegating_renderer.h"
#include "cc/output/gl_renderer.h"
#include "cc/output/software_renderer.h"
#include "cc/output/texture_mailbox_deleter.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/shared_quad_state.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/raster/bitmap_tile_task_worker_pool.h"
#include "cc/raster/gpu_rasterizer.h"
#include "cc/raster/gpu_tile_task_worker_pool.h"
#include "cc/raster/one_copy_tile_task_worker_pool.h"
#include "cc/raster/tile_task_worker_pool.h"
#include "cc/raster/zero_copy_tile_task_worker_pool.h"
#include "cc/resources/memory_history.h"
#include "cc/resources/resource_pool.h"
#include "cc/resources/ui_resource_bitmap.h"
#include "cc/scheduler/delay_based_time_source.h"
#include "cc/tiles/eviction_tile_priority_queue.h"
#include "cc/tiles/picture_layer_tiling.h"
#include "cc/tiles/raster_tile_priority_queue.h"
#include "cc/trees/damage_tracker.h"
#include "cc/trees/latency_info_swap_promise_monitor.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_common.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/single_thread_proxy.h"
#include "cc/trees/tree_synchronizer.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/scroll_offset.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/geometry/vector2d_conversions.h"

namespace cc {
namespace {

    // Small helper class that saves the current viewport location as the user sees
    // it and resets to the same location.
    class ViewportAnchor {
    public:
        ViewportAnchor(LayerImpl* inner_scroll, LayerImpl* outer_scroll)
            : inner_(inner_scroll)
            , outer_(outer_scroll)
        {
            viewport_in_content_coordinates_ = inner_->CurrentScrollOffset();

            if (outer_)
                viewport_in_content_coordinates_ += outer_->CurrentScrollOffset();
        }

        void ResetViewportToAnchoredPosition()
        {
            DCHECK(outer_);

            inner_->ClampScrollToMaxScrollOffset();
            outer_->ClampScrollToMaxScrollOffset();

            gfx::ScrollOffset viewport_location = inner_->CurrentScrollOffset() + outer_->CurrentScrollOffset();

            gfx::Vector2dF delta = viewport_in_content_coordinates_.DeltaFrom(viewport_location);

            delta = outer_->ScrollBy(delta);
            inner_->ScrollBy(delta);
        }

    private:
        LayerImpl* inner_;
        LayerImpl* outer_;
        gfx::ScrollOffset viewport_in_content_coordinates_;
    };

    void DidVisibilityChange(LayerTreeHostImpl* id, bool visible)
    {
        if (visible) {
            TRACE_EVENT_ASYNC_BEGIN1("cc", "LayerTreeHostImpl::SetVisible", id,
                "LayerTreeHostImpl", id);
            return;
        }

        TRACE_EVENT_ASYNC_END0("cc", "LayerTreeHostImpl::SetVisible", id);
    }

    size_t GetDefaultMemoryAllocationLimit()
    {
        // TODO(ccameron): (http://crbug.com/137094) This 64MB default is a straggler
        // from the old texture manager and is just to give us a default memory
        // allocation before we get a callback from the GPU memory manager. We
        // should probaby either:
        // - wait for the callback before rendering anything instead
        // - push this into the GPU memory manager somehow.
        return 64 * 1024 * 1024;
    }

} // namespace

LayerTreeHostImpl::FrameData::FrameData()
    : render_surface_layer_list(nullptr)
    , has_no_damage(false)
{
}

LayerTreeHostImpl::FrameData::~FrameData() { }

scoped_ptr<LayerTreeHostImpl> LayerTreeHostImpl::Create(
    const LayerTreeSettings& settings,
    LayerTreeHostImplClient* client,
    Proxy* proxy,
    RenderingStatsInstrumentation* rendering_stats_instrumentation,
    SharedBitmapManager* shared_bitmap_manager,
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
    TaskGraphRunner* task_graph_runner,
    int id)
{
    return make_scoped_ptr(new LayerTreeHostImpl(
        settings, client, proxy, rendering_stats_instrumentation,
        shared_bitmap_manager, gpu_memory_buffer_manager, task_graph_runner, id));
}

LayerTreeHostImpl::LayerTreeHostImpl(
    const LayerTreeSettings& settings,
    LayerTreeHostImplClient* client,
    Proxy* proxy,
    RenderingStatsInstrumentation* rendering_stats_instrumentation,
    SharedBitmapManager* shared_bitmap_manager,
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
    TaskGraphRunner* task_graph_runner,
    int id)
    : client_(client)
    , proxy_(proxy)
    , current_begin_frame_tracker_(BEGINFRAMETRACKER_FROM_HERE)
    , output_surface_(nullptr)
    , content_is_suitable_for_gpu_rasterization_(true)
    , has_gpu_rasterization_trigger_(false)
    , use_gpu_rasterization_(false)
    , use_msaa_(false)
    , gpu_rasterization_status_(GpuRasterizationStatus::OFF_DEVICE)
    , tree_resources_for_gpu_rasterization_dirty_(false)
    , input_handler_client_(NULL)
    , did_lock_scrolling_layer_(false)
    , wheel_scrolling_(false)
    , scroll_affects_scroll_handler_(false)
    , scroll_layer_id_when_mouse_over_scrollbar_(0)
    , tile_priorities_dirty_(false)
    , settings_(settings)
    , visible_(true)
    , cached_managed_memory_policy_(
          GetDefaultMemoryAllocationLimit(),
          gpu::MemoryAllocation::CUTOFF_ALLOW_EVERYTHING,
          ManagedMemoryPolicy::kDefaultNumResourcesLimit)
    , is_synchronous_single_threaded_(!proxy->HasImplThread() && !settings.single_thread_proxy_scheduler)
    ,
    // Must be initialized after is_synchronous_single_threaded_ and proxy_.
    tile_manager_(
        TileManager::Create(this,
            GetTaskRunner(),
            is_synchronous_single_threaded_
                ? std::numeric_limits<size_t>::max()
                : settings.scheduled_raster_task_limit))
    , pinch_gesture_active_(false)
    , pinch_gesture_end_should_clear_scrolling_layer_(false)
    , fps_counter_(FrameRateCounter::Create(proxy_->HasImplThread()))
    , memory_history_(MemoryHistory::Create())
    , debug_rect_history_(DebugRectHistory::Create())
    , texture_mailbox_deleter_(new TextureMailboxDeleter(GetTaskRunner()))
    , max_memory_needed_bytes_(0)
    , resourceless_software_draw_(false)
    , animation_registrar_()
    , rendering_stats_instrumentation_(rendering_stats_instrumentation)
    ,
    //micro_benchmark_controller_(this),
    shared_bitmap_manager_(shared_bitmap_manager)
    , gpu_memory_buffer_manager_(gpu_memory_buffer_manager)
    , task_graph_runner_(task_graph_runner)
    , id_(id)
    , requires_high_res_to_draw_(false)
    , is_likely_to_require_a_draw_(false)
    , frame_timing_tracker_(FrameTimingTracker::Create(this))
{
    if (settings.use_compositor_animation_timelines) {
        if (settings.accelerated_animation_enabled) {
            animation_host_ = AnimationHost::Create(ThreadInstance::IMPL);
            animation_host_->SetMutatorHostClient(this);
            animation_host_->SetSupportsScrollAnimations(
                proxy_->SupportsImplScrolling());
        }
    } else {
        animation_registrar_ = AnimationRegistrar::Create();
        animation_registrar_->set_supports_scroll_animations(
            proxy_->SupportsImplScrolling());
    }

    DCHECK(proxy_->IsImplThread());
    DidVisibilityChange(this, visible_);

    SetDebugState(settings.initial_debug_state);

    // LTHI always has an active tree.
    active_tree_ = LayerTreeImpl::create(this, new SyncedProperty<ScaleGroup>(),
        new SyncedTopControls, new SyncedElasticOverscroll);

    viewport_ = Viewport::Create(this);

    TRACE_EVENT_OBJECT_CREATED_WITH_ID(
        TRACE_DISABLED_BY_DEFAULT("cc.debug"), "cc::LayerTreeHostImpl", id_);

    top_controls_manager_ = TopControlsManager::Create(this,
        settings.top_controls_show_threshold,
        settings.top_controls_hide_threshold);
}

LayerTreeHostImpl::~LayerTreeHostImpl()
{
    DCHECK(proxy_->IsImplThread());
    TRACE_EVENT0("cc", "LayerTreeHostImpl::~LayerTreeHostImpl()");
    TRACE_EVENT_OBJECT_DELETED_WITH_ID(
        TRACE_DISABLED_BY_DEFAULT("cc.debug"), "cc::LayerTreeHostImpl", id_);

    if (input_handler_client_) {
        input_handler_client_->WillShutdown();
        input_handler_client_ = NULL;
    }
    if (scroll_elasticity_helper_)
        scroll_elasticity_helper_.reset();

    // The layer trees must be destroyed before the layer tree host. We've
    // made a contract with our animation controllers that the registrar
    // will outlive them, and we must make good.
    if (recycle_tree_)
        recycle_tree_->Shutdown();
    if (pending_tree_)
        pending_tree_->Shutdown();
    active_tree_->Shutdown();
    recycle_tree_ = nullptr;
    pending_tree_ = nullptr;
    active_tree_ = nullptr;

    if (animation_host_) {
        animation_host_->ClearTimelines();
        animation_host_->SetMutatorHostClient(nullptr);
    }

    CleanUpTileManager();
    renderer_ = nullptr;
    resource_provider_ = nullptr;

    if (output_surface_) {
        output_surface_->DetachFromClient();
        output_surface_ = nullptr;
    }
}

void LayerTreeHostImpl::BeginMainFrameAborted(CommitEarlyOutReason reason)
{
    // If the begin frame data was handled, then scroll and scale set was applied
    // by the main thread, so the active tree needs to be updated as if these sent
    // values were applied and committed.
    if (CommitEarlyOutHandledCommit(reason))
        active_tree_->ApplySentScrollAndScaleDeltasFromAbortedCommit();
}

void LayerTreeHostImpl::BeginCommit()
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::BeginCommit");

    // Ensure all textures are returned so partial texture updates can happen
    // during the commit.
    // TODO(ericrk): We should not need to ForceReclaimResources when using
    // Impl-side-painting as it doesn't upload during commits. However,
    // Display::Draw currently relies on resource being reclaimed to block drawing
    // between BeginCommit / Swap. See crbug.com/489515.
    if (output_surface_)
        output_surface_->ForceReclaimResources();

    if (!proxy_->CommitToActiveTree())
        CreatePendingTree();
}

void LayerTreeHostImpl::CommitComplete()
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::CommitComplete");

    if (proxy_->CommitToActiveTree()) {
        // We have to activate animations here or "IsActive()" is true on the layers
        // but the animations aren't activated yet so they get ignored by
        // UpdateDrawProperties.
        ActivateAnimations();
    }

    // Start animations before UpdateDrawProperties and PrepareTiles, as they can
    // change the results. When doing commit to the active tree, this must happen
    // after ActivateAnimations() in order for this ticking to be propogated to
    // layers on the active tree.
    Animate();

    // LayerTreeHost may have changed the GPU rasterization flags state, which
    // may require an update of the tree resources.
    UpdateTreeResourcesForGpuRasterizationIfNeeded();
    sync_tree()->set_needs_update_draw_properties();

    // We need an update immediately post-commit to have the opportunity to create
    // tilings.  Because invalidations may be coming from the main thread, it's
    // safe to do an update for lcd text at this point and see if lcd text needs
    // to be disabled on any layers.
    bool update_lcd_text = true;
    sync_tree()->UpdateDrawProperties(update_lcd_text);
    // Start working on newly created tiles immediately if needed.
    // TODO(vmpstr): Investigate always having PrepareTiles issue
    // NotifyReadyToActivate, instead of handling it here.
    bool did_prepare_tiles = PrepareTiles();
    if (!did_prepare_tiles) {
        NotifyReadyToActivate();

        // Ensure we get ReadyToDraw signal even when PrepareTiles not run. This
        // is important for SingleThreadProxy and impl-side painting case. For
        // STP, we commit to active tree and RequiresHighResToDraw, and set
        // Scheduler to wait for ReadyToDraw signal to avoid Checkerboard.
        if (proxy_->CommitToActiveTree())
            NotifyReadyToDraw();
    }

    //micro_benchmark_controller_.DidCompleteCommit();
}

bool LayerTreeHostImpl::CanDraw() const
{
    // Note: If you are changing this function or any other function that might
    // affect the result of CanDraw, make sure to call
    // client_->OnCanDrawStateChanged in the proper places and update the
    // NotifyIfCanDrawChanged test.

    if (!renderer_) {
        TRACE_EVENT_INSTANT0("cc", "LayerTreeHostImpl::CanDraw no renderer",
            TRACE_EVENT_SCOPE_THREAD);
        return false;
    }

    // Must have an OutputSurface if |renderer_| is not NULL.
    DCHECK(output_surface_);

    // TODO(boliu): Make draws without root_layer work and move this below
    // draw_and_swap_full_viewport_every_frame check. Tracked in crbug.com/264967.
    if (!active_tree_->root_layer()) {
        TRACE_EVENT_INSTANT0("cc", "LayerTreeHostImpl::CanDraw no root layer",
            TRACE_EVENT_SCOPE_THREAD);
        return false;
    }

    if (output_surface_->capabilities().draw_and_swap_full_viewport_every_frame)
        return true;

    if (DrawViewportSize().IsEmpty()) {
        TRACE_EVENT_INSTANT0("cc", "LayerTreeHostImpl::CanDraw empty viewport",
            TRACE_EVENT_SCOPE_THREAD);
        return false;
    }
    if (active_tree_->ViewportSizeInvalid()) {
        TRACE_EVENT_INSTANT0(
            "cc", "LayerTreeHostImpl::CanDraw viewport size recently changed",
            TRACE_EVENT_SCOPE_THREAD);
        return false;
    }
    if (EvictedUIResourcesExist()) {
        TRACE_EVENT_INSTANT0(
            "cc", "LayerTreeHostImpl::CanDraw UI resources evicted not recreated",
            TRACE_EVENT_SCOPE_THREAD);
        return false;
    }
    return true;
}

void LayerTreeHostImpl::Animate()
{
    DCHECK(proxy_->IsImplThread());
    base::TimeTicks monotonic_time = CurrentBeginFrameArgs().frame_time;

    // mithro(TODO): Enable these checks.
    // DCHECK(!current_begin_frame_tracker_.HasFinished());
    // DCHECK(monotonic_time == current_begin_frame_tracker_.Current().frame_time)
    //  << "Called animate with unknown frame time!?";

    if (input_handler_client_) {
        // This animates fling scrolls. But on Android WebView root flings are
        // controlled by the application, so the compositor does not animate them.
        bool ignore_fling = settings_.ignore_root_layer_flings && IsCurrentlyScrollingInnerViewport();
        if (!ignore_fling)
            input_handler_client_->Animate(monotonic_time);
    }

    AnimatePageScale(monotonic_time);
    AnimateLayers(monotonic_time);
    AnimateScrollbars(monotonic_time);
    AnimateTopControls(monotonic_time);

    // Animating stuff can change the root scroll offset, so inform the
    // synchronous input handler.
    UpdateRootLayerStateForSynchronousInputHandler();
}

bool LayerTreeHostImpl::PrepareTiles()
{
    if (!tile_priorities_dirty_)
        return false;

    client_->WillPrepareTiles();
    bool did_prepare_tiles = tile_manager_->PrepareTiles(global_tile_state_);
    if (did_prepare_tiles)
        tile_priorities_dirty_ = false;
    client_->DidPrepareTiles();
    return did_prepare_tiles;
}

void LayerTreeHostImpl::StartPageScaleAnimation(
    const gfx::Vector2d& target_offset,
    bool anchor_point,
    float page_scale,
    base::TimeDelta duration)
{
    if (!InnerViewportScrollLayer())
        return;

    gfx::ScrollOffset scroll_total = active_tree_->TotalScrollOffset();
    gfx::SizeF scaled_scrollable_size = active_tree_->ScrollableSize();
    gfx::SizeF viewport_size = gfx::SizeF(active_tree_->InnerViewportContainerLayer()->bounds());

    // Easing constants experimentally determined.
    scoped_ptr<TimingFunction> timing_function = CubicBezierTimingFunction::Create(.8, 0, .3, .9);

    // TODO(miletus) : Pass in ScrollOffset.
    page_scale_animation_ = PageScaleAnimation::Create(
        ScrollOffsetToVector2dF(scroll_total),
        active_tree_->current_page_scale_factor(), viewport_size,
        scaled_scrollable_size, timing_function.Pass());

    if (anchor_point) {
        gfx::Vector2dF anchor(target_offset);
        page_scale_animation_->ZoomWithAnchor(anchor,
            page_scale,
            duration.InSecondsF());
    } else {
        gfx::Vector2dF scaled_target_offset = target_offset;
        page_scale_animation_->ZoomTo(scaled_target_offset,
            page_scale,
            duration.InSecondsF());
    }

    SetNeedsAnimate();
    client_->SetNeedsCommitOnImplThread();
    client_->RenewTreePriority();
}

void LayerTreeHostImpl::SetNeedsAnimateInput()
{
    DCHECK_IMPLIES(IsCurrentlyScrollingInnerViewport(),
        !settings_.ignore_root_layer_flings);
    SetNeedsAnimate();
}

bool LayerTreeHostImpl::IsCurrentlyScrollingInnerViewport() const
{
    LayerImpl* scrolling_layer = CurrentlyScrollingLayer();
    if (!scrolling_layer)
        return false;
    return scrolling_layer == InnerViewportScrollLayer();
}

bool LayerTreeHostImpl::IsCurrentlyScrollingLayerAt(
    const gfx::Point& viewport_point,
    InputHandler::ScrollInputType type) const
{
    LayerImpl* scrolling_layer_impl = CurrentlyScrollingLayer();
    if (!scrolling_layer_impl)
        return false;

    gfx::PointF device_viewport_point = gfx::ScalePoint(gfx::PointF(viewport_point), active_tree_->device_scale_factor());

    LayerImpl* layer_impl = active_tree_->FindLayerThatIsHitByPoint(device_viewport_point);

    bool scroll_on_main_thread = false;
    LayerImpl* test_layer_impl = FindScrollLayerForDeviceViewportPoint(
        device_viewport_point, type, layer_impl, &scroll_on_main_thread, NULL);

    if (!test_layer_impl)
        return false;

    if (scrolling_layer_impl == test_layer_impl)
        return true;

    // For active scrolling state treat the inner/outer viewports interchangeably.
    if ((scrolling_layer_impl == InnerViewportScrollLayer() && test_layer_impl == OuterViewportScrollLayer()) || (scrolling_layer_impl == OuterViewportScrollLayer() && test_layer_impl == InnerViewportScrollLayer())) {
        return true;
    }

    return false;
}

bool LayerTreeHostImpl::HaveWheelEventHandlersAt(
    const gfx::Point& viewport_point)
{
    gfx::PointF device_viewport_point = gfx::ScalePoint(gfx::PointF(viewport_point), active_tree_->device_scale_factor());

    LayerImpl* layer_impl = active_tree_->FindLayerWithWheelHandlerThatIsHitByPoint(
        device_viewport_point);

    return layer_impl != NULL;
}

static LayerImpl* NextLayerInScrollOrder(LayerImpl* layer)
{
    if (layer->scroll_parent())
        return layer->scroll_parent();

    return layer->parent();
}

static ScrollBlocksOn EffectiveScrollBlocksOn(LayerImpl* layer)
{
    ScrollBlocksOn blocks = SCROLL_BLOCKS_ON_NONE;
    for (; layer; layer = NextLayerInScrollOrder(layer)) {
        blocks |= layer->scroll_blocks_on();
    }
    return blocks;
}

bool LayerTreeHostImpl::DoTouchEventsBlockScrollAt(
    const gfx::Point& viewport_point)
{
    gfx::PointF device_viewport_point = gfx::ScalePoint(gfx::PointF(viewport_point), active_tree_->device_scale_factor());

    // First check if scrolling at this point is required to block on any
    // touch event handlers.  Note that we must start at the innermost layer
    // (as opposed to only the layer found to contain a touch handler region
    // below) to ensure all relevant scroll-blocks-on values are applied.
    LayerImpl* layer_impl = active_tree_->FindLayerThatIsHitByPoint(device_viewport_point);
    ScrollBlocksOn blocking = EffectiveScrollBlocksOn(layer_impl);
    if (!(blocking & SCROLL_BLOCKS_ON_START_TOUCH))
        return false;

    // Now determine if there are actually any handlers at that point.
    // TODO(rbyers): Consider also honoring touch-action (crbug.com/347272).
    layer_impl = active_tree_->FindLayerThatIsHitByPointInTouchHandlerRegion(
        device_viewport_point);
    return layer_impl != NULL;
}

scoped_ptr<SwapPromiseMonitor>
LayerTreeHostImpl::CreateLatencyInfoSwapPromiseMonitor(
    ui::LatencyInfo* latency)
{
    DebugBreak();
    return nullptr;
    //   return make_scoped_ptr(
    //       new LatencyInfoSwapPromiseMonitor(latency, NULL, this));
}

ScrollElasticityHelper* LayerTreeHostImpl::CreateScrollElasticityHelper()
{
    DCHECK(!scroll_elasticity_helper_);
    if (settings_.enable_elastic_overscroll) {
        scroll_elasticity_helper_.reset(
            ScrollElasticityHelper::CreateForLayerTreeHostImpl(this));
    }
    return scroll_elasticity_helper_.get();
}

void LayerTreeHostImpl::QueueSwapPromiseForMainThreadScrollUpdate(
    scoped_ptr<SwapPromise> swap_promise)
{
    swap_promises_for_main_thread_scroll_update_.push_back(swap_promise.Pass());
}

void LayerTreeHostImpl::TrackDamageForAllSurfaces(
    LayerImpl* root_draw_layer,
    const LayerImplList& render_surface_layer_list)
{
    // For now, we use damage tracking to compute a global scissor. To do this, we
    // must compute all damage tracking before drawing anything, so that we know
    // the root damage rect. The root damage rect is then used to scissor each
    // surface.
    size_t render_surface_layer_list_size = render_surface_layer_list.size();
    for (size_t i = 0; i < render_surface_layer_list_size; ++i) {
        size_t surface_index = render_surface_layer_list_size - 1 - i;
        LayerImpl* render_surface_layer = render_surface_layer_list[surface_index];
        RenderSurfaceImpl* render_surface = render_surface_layer->render_surface();
        DCHECK(render_surface);
        render_surface->damage_tracker()->UpdateDamageTrackingState(
            render_surface->layer_list(),
            render_surface_layer->id(),
            render_surface->SurfacePropertyChangedOnlyFromDescendant(),
            render_surface->content_rect(),
            render_surface_layer->mask_layer(),
            render_surface_layer->filters());
    }
}

void LayerTreeHostImpl::FrameData::AsValueInto(
    base::trace_event::TracedValue* value) const
{
    //   value->SetBoolean("has_no_damage", has_no_damage);
    //
    //   // Quad data can be quite large, so only dump render passes if we select
    //   // cc.debug.quads.
    //   bool quads_enabled;
    //   TRACE_EVENT_CATEGORY_GROUP_ENABLED(
    //       TRACE_DISABLED_BY_DEFAULT("cc.debug.quads"), &quads_enabled);
    //   if (quads_enabled) {
    //     value->BeginArray("render_passes");
    //     for (size_t i = 0; i < render_passes.size(); ++i) {
    //       value->BeginDictionary();
    //       render_passes[i]->AsValueInto(value);
    //       value->EndDictionary();
    //     }
    //     value->EndArray();
    //   }
    DebugBreak();
}

void LayerTreeHostImpl::FrameData::AppendRenderPass(
    scoped_ptr<RenderPass> render_pass)
{
    render_passes_by_id[render_pass->id] = render_pass.get();
    render_passes.push_back(render_pass.Pass());
}

DrawMode LayerTreeHostImpl::GetDrawMode() const
{
    if (resourceless_software_draw_) {
        return DRAW_MODE_RESOURCELESS_SOFTWARE;
    } else if (output_surface_->context_provider()) {
        return DRAW_MODE_HARDWARE;
    } else {
        return DRAW_MODE_SOFTWARE;
    }
}

static void AppendQuadsForRenderSurfaceLayer(
    RenderPass* target_render_pass,
    LayerImpl* layer,
    const RenderPass* contributing_render_pass,
    AppendQuadsData* append_quads_data)
{
    RenderSurfaceImpl* surface = layer->render_surface();
    const gfx::Transform& draw_transform = surface->draw_transform();
    const Occlusion& occlusion = surface->occlusion_in_content_space();
    SkColor debug_border_color = surface->GetDebugBorderColor();
    float debug_border_width = surface->GetDebugBorderWidth();
    LayerImpl* mask_layer = layer->mask_layer();

    surface->AppendQuads(target_render_pass, draw_transform, occlusion,
        debug_border_color, debug_border_width, mask_layer,
        append_quads_data, contributing_render_pass->id);

    // Add replica after the surface so that it appears below the surface.
    if (layer->has_replica()) {
        const gfx::Transform& replica_draw_transform = surface->replica_draw_transform();
        Occlusion replica_occlusion = occlusion.GetOcclusionWithGivenDrawTransform(
            surface->replica_draw_transform());
        SkColor replica_debug_border_color = surface->GetReplicaDebugBorderColor();
        float replica_debug_border_width = surface->GetReplicaDebugBorderWidth();
        // TODO(danakj): By using the same RenderSurfaceImpl for both the
        // content and its reflection, it's currently not possible to apply a
        // separate mask to the reflection layer or correctly handle opacity in
        // reflections (opacity must be applied after drawing both the layer and its
        // reflection). The solution is to introduce yet another RenderSurfaceImpl
        // to draw the layer and its reflection in. For now we only apply a separate
        // reflection mask if the contents don't have a mask of their own.
        LayerImpl* replica_mask_layer = mask_layer ? mask_layer : layer->replica_layer()->mask_layer();

        surface->AppendQuads(target_render_pass, replica_draw_transform,
            replica_occlusion, replica_debug_border_color,
            replica_debug_border_width, replica_mask_layer,
            append_quads_data, contributing_render_pass->id);
    }
}

static void AppendQuadsToFillScreen(const gfx::Rect& root_scroll_layer_rect,
    RenderPass* target_render_pass,
    LayerImpl* root_layer,
    SkColor screen_background_color,
    const Region& fill_region)
{
    if (!root_layer || !SkColorGetA(screen_background_color))
        return;
    if (fill_region.IsEmpty())
        return;

    // Manually create the quad state for the gutter quads, as the root layer
    // doesn't have any bounds and so can't generate this itself.
    // TODO(danakj): Make the gutter quads generated by the solid color layer
    // (make it smarter about generating quads to fill unoccluded areas).

    gfx::Rect root_target_rect = root_layer->render_surface()->content_rect();
    float opacity = 1.f;
    int sorting_context_id = 0;
    SharedQuadState* shared_quad_state = target_render_pass->CreateAndAppendSharedQuadState();
    shared_quad_state->SetAll(gfx::Transform(),
        root_target_rect.size(),
        root_target_rect,
        root_target_rect,
        false,
        opacity,
        SkXfermode::kSrcOver_Mode,
        sorting_context_id);

    for (Region::Iterator fill_rects(fill_region); fill_rects.has_rect();
         fill_rects.next()) {
        gfx::Rect screen_space_rect = fill_rects.rect();
        gfx::Rect visible_screen_space_rect = screen_space_rect;
        // Skip the quad culler and just append the quads directly to avoid
        // occlusion checks.
        SolidColorDrawQuad* quad = target_render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        quad->SetNew(shared_quad_state,
            screen_space_rect,
            visible_screen_space_rect,
            screen_background_color,
            false);
    }
}

DrawResult LayerTreeHostImpl::CalculateRenderPasses(
    FrameData* frame)
{
    DCHECK(frame->render_passes.empty());
    DCHECK(CanDraw());
    DCHECK(active_tree_->root_layer());

    TrackDamageForAllSurfaces(active_tree_->root_layer(),
        *frame->render_surface_layer_list);

    // If the root render surface has no visible damage, then don't generate a
    // frame at all.
    RenderSurfaceImpl* root_surface = active_tree_->root_layer()->render_surface();
    bool root_surface_has_no_visible_damage = !root_surface->damage_tracker()->current_damage_rect().Intersects(
        root_surface->content_rect());
    bool root_surface_has_contributing_layers = !root_surface->layer_list().empty();
    bool hud_wants_to_draw_ = active_tree_->hud_layer() && active_tree_->hud_layer()->IsAnimatingHUDContents();
    if (root_surface_has_contributing_layers && root_surface_has_no_visible_damage && active_tree_->LayersWithCopyOutputRequest().empty() && !output_surface_->capabilities().can_force_reclaim_resources && !hud_wants_to_draw_) {
        TRACE_EVENT0("cc",
            "LayerTreeHostImpl::CalculateRenderPasses::EmptyDamageRect");
        frame->has_no_damage = true;
        DCHECK(!output_surface_->capabilities()
                    .draw_and_swap_full_viewport_every_frame);
        return DRAW_SUCCESS;
    }

    TRACE_EVENT_BEGIN2(
        "cc", "LayerTreeHostImpl::CalculateRenderPasses",
        "render_surface_layer_list.size()",
        static_cast<uint64>(frame->render_surface_layer_list->size()),
        "RequiresHighResToDraw", RequiresHighResToDraw());

    // Create the render passes in dependency order.
    size_t render_surface_layer_list_size = frame->render_surface_layer_list->size();
    for (size_t i = 0; i < render_surface_layer_list_size; ++i) {
        size_t surface_index = render_surface_layer_list_size - 1 - i;
        LayerImpl* render_surface_layer = (*frame->render_surface_layer_list)[surface_index];
        RenderSurfaceImpl* render_surface = render_surface_layer->render_surface();

        bool should_draw_into_render_pass = render_surface_layer->parent() == NULL || render_surface->contributes_to_drawn_surface() || render_surface_layer->HasCopyRequest();
        if (should_draw_into_render_pass)
            render_surface->AppendRenderPasses(frame);
    }

    // When we are displaying the HUD, change the root damage rect to cover the
    // entire root surface. This will disable partial-swap/scissor optimizations
    // that would prevent the HUD from updating, since the HUD does not cause
    // damage itself, to prevent it from messing with damage visualizations. Since
    // damage visualizations are done off the LayerImpls and RenderSurfaceImpls,
    // changing the RenderPass does not affect them.
    if (active_tree_->hud_layer()) {
        RenderPass* root_pass = frame->render_passes.back();
        root_pass->damage_rect = root_pass->output_rect;
    }

    // Because the active tree could be drawn again if this fails for some reason,
    // clear all of the copy request flags so that sanity checks for the counts
    // succeed.
    if (!active_tree_->LayersWithCopyOutputRequest().empty()) {
        LayerTreeHostCommon::CallFunctionForSubtree(
            active_tree_->root_layer(), [](LayerImpl* layer) {
                layer->set_num_layer_or_descendant_with_copy_request(0);
            });
    }

    // Grab this region here before iterating layers. Taking copy requests from
    // the layers while constructing the render passes will dirty the render
    // surface layer list and this unoccluded region, flipping the dirty bit to
    // true, and making us able to query for it without doing
    // UpdateDrawProperties again. The value inside the Region is not actually
    // changed until UpdateDrawProperties happens, so a reference to it is safe.
    const Region& unoccluded_screen_space_region = active_tree_->UnoccludedScreenSpaceRegion();

    // Typically when we are missing a texture and use a checkerboard quad, we
    // still draw the frame. However when the layer being checkerboarded is moving
    // due to an impl-animation, we drop the frame to avoid flashing due to the
    // texture suddenly appearing in the future.
    DrawResult draw_result = DRAW_SUCCESS;

    int layers_drawn = 0;

    const DrawMode draw_mode = GetDrawMode();

    int num_missing_tiles = 0;
    int num_incomplete_tiles = 0;
    int64 checkerboarded_no_recording_content_area = 0;
    int64 checkerboarded_needs_raster_content_area = 0;
    bool have_copy_request = false;
    bool have_missing_animated_tiles = false;

    LayerIterator end = LayerIterator::End(frame->render_surface_layer_list);
    for (LayerIterator it = LayerIterator::Begin(frame->render_surface_layer_list);
         it != end; ++it) {
        RenderPassId target_render_pass_id = it.target_render_surface_layer()->render_surface()->GetRenderPassId();
        RenderPass* target_render_pass = frame->render_passes_by_id[target_render_pass_id];

        AppendQuadsData append_quads_data;

        if (it.represents_target_render_surface()) {
            if (it->HasCopyRequest()) {
                have_copy_request = true;
                it->TakeCopyRequestsAndTransformToTarget(
                    &target_render_pass->copy_requests);
            }
        } else if (it.represents_contributing_render_surface() && it->render_surface()->contributes_to_drawn_surface()) {
            RenderPassId contributing_render_pass_id = it->render_surface()->GetRenderPassId();
            RenderPass* contributing_render_pass = frame->render_passes_by_id[contributing_render_pass_id];
            AppendQuadsForRenderSurfaceLayer(target_render_pass,
                *it,
                contributing_render_pass,
                &append_quads_data);
        } else if (it.represents_itself() && !it->visible_layer_rect().IsEmpty()) {
            bool occluded = it->draw_properties().occlusion_in_content_space.IsOccluded(
                it->visible_layer_rect());
            if (!occluded && it->WillDraw(draw_mode, resource_provider_.get())) {
                DCHECK_EQ(active_tree_, it->layer_tree_impl());

                frame->will_draw_layers.push_back(*it);

                if (it->HasContributingDelegatedRenderPasses()) {
                    RenderPassId contributing_render_pass_id = it->FirstContributingRenderPassId();
                    while (frame->render_passes_by_id.find(contributing_render_pass_id) != frame->render_passes_by_id.end()) {
                        RenderPass* render_pass = frame->render_passes_by_id[contributing_render_pass_id];

                        it->AppendQuads(render_pass, &append_quads_data);

                        contributing_render_pass_id = it->NextContributingRenderPassId(contributing_render_pass_id);
                    }
                }

                it->AppendQuads(target_render_pass, &append_quads_data);

                // For layers that represent themselves, add composite frame timing
                // requests if the visible rect intersects the requested rect.
                for (const auto& request : it->frame_timing_requests()) {
                    if (request.rect().Intersects(it->visible_layer_rect())) {
                        frame->composite_events.push_back(
                            FrameTimingTracker::FrameAndRectIds(
                                active_tree_->source_frame_number(), request.id()));
                    }
                }
            }

            ++layers_drawn;
        }

        rendering_stats_instrumentation_->AddVisibleContentArea(
            append_quads_data.visible_layer_area);
        rendering_stats_instrumentation_->AddApproximatedVisibleContentArea(
            append_quads_data.approximated_visible_content_area);
        rendering_stats_instrumentation_->AddCheckerboardedVisibleContentArea(
            append_quads_data.checkerboarded_visible_content_area);
        rendering_stats_instrumentation_->AddCheckerboardedNoRecordingContentArea(
            append_quads_data.checkerboarded_no_recording_content_area);
        rendering_stats_instrumentation_->AddCheckerboardedNeedsRasterContentArea(
            append_quads_data.checkerboarded_needs_raster_content_area);

        num_missing_tiles += append_quads_data.num_missing_tiles;
        num_incomplete_tiles += append_quads_data.num_incomplete_tiles;
        checkerboarded_no_recording_content_area += append_quads_data.checkerboarded_no_recording_content_area;
        checkerboarded_needs_raster_content_area += append_quads_data.checkerboarded_needs_raster_content_area;

        if (append_quads_data.num_missing_tiles) {
            bool layer_has_animating_transform = it->screen_space_transform_is_animating();
            if (layer_has_animating_transform)
                have_missing_animated_tiles = true;
        }
    }

    if (have_missing_animated_tiles)
        draw_result = DRAW_ABORTED_CHECKERBOARD_ANIMATIONS;

    // When we require high res to draw, abort the draw (almost) always. This does
    // not cause the scheduler to do a main frame, instead it will continue to try
    // drawing until we finally complete, so the copy request will not be lost.
    // TODO(weiliangc): Remove RequiresHighResToDraw. crbug.com/469175
    if (num_incomplete_tiles || num_missing_tiles) {
        if (RequiresHighResToDraw())
            draw_result = DRAW_ABORTED_MISSING_HIGH_RES_CONTENT;
    }

    // When this capability is set we don't have control over the surface the
    // compositor draws to, so even though the frame may not be complete, the
    // previous frame has already been potentially lost, so an incomplete frame is
    // better than nothing, so this takes highest precidence.
    if (output_surface_->capabilities().draw_and_swap_full_viewport_every_frame)
        draw_result = DRAW_SUCCESS;

#if DCHECK_IS_ON()
    for (const auto& render_pass : frame->render_passes) {
        for (const auto& quad : render_pass->quad_list)
            DCHECK(quad->shared_quad_state);
        DCHECK(frame->render_passes_by_id.find(render_pass->id) != frame->render_passes_by_id.end());
    }
#endif
    DCHECK(frame->render_passes.back()->output_rect.origin().IsOrigin());

    if (!active_tree_->has_transparent_background()) {
        frame->render_passes.back()->has_transparent_background = false;
        AppendQuadsToFillScreen(
            active_tree_->RootScrollLayerDeviceViewportBounds(),
            frame->render_passes.back(), active_tree_->root_layer(),
            active_tree_->background_color(), unoccluded_screen_space_region);
    }

    RemoveRenderPasses(frame);
    renderer_->DecideRenderPassAllocationsForFrame(frame->render_passes);

    // Any copy requests left in the tree are not going to get serviced, and
    // should be aborted.
    ScopedPtrVector<CopyOutputRequest> requests_to_abort;
    while (!active_tree_->LayersWithCopyOutputRequest().empty()) {
        LayerImpl* layer = active_tree_->LayersWithCopyOutputRequest().back();
        layer->TakeCopyRequestsAndTransformToTarget(&requests_to_abort);
    }
    for (size_t i = 0; i < requests_to_abort.size(); ++i)
        requests_to_abort[i]->SendEmptyResult();

    // If we're making a frame to draw, it better have at least one render pass.
    DCHECK(!frame->render_passes.empty());

    if (active_tree_->has_ever_been_drawn()) {
        UMA_HISTOGRAM_COUNTS_100(
            "Compositing.RenderPass.AppendQuadData.NumMissingTiles",
            num_missing_tiles);
        UMA_HISTOGRAM_COUNTS_100(
            "Compositing.RenderPass.AppendQuadData.NumIncompleteTiles",
            num_incomplete_tiles);
        UMA_HISTOGRAM_COUNTS(
            "Compositing.RenderPass.AppendQuadData."
            "CheckerboardedNoRecordingContentArea",
            checkerboarded_no_recording_content_area);
        UMA_HISTOGRAM_COUNTS(
            "Compositing.RenderPass.AppendQuadData."
            "CheckerboardedNeedRasterContentArea",
            checkerboarded_needs_raster_content_area);
    }

    // Should only have one render pass in resourceless software mode.
    DCHECK(draw_mode != DRAW_MODE_RESOURCELESS_SOFTWARE || frame->render_passes.size() == 1u)
        << frame->render_passes.size();

    TRACE_EVENT_END2("cc", "LayerTreeHostImpl::CalculateRenderPasses",
        "draw_result", draw_result, "missing tiles",
        num_missing_tiles);

    // Draw has to be successful to not drop the copy request layer.
    // When we have a copy request for a layer, we need to draw even if there
    // would be animating checkerboards, because failing under those conditions
    // triggers a new main frame, which may cause the copy request layer to be
    // destroyed.
    // TODO(weiliangc): Test copy request w/ output surface recreation. Would
    // trigger this DCHECK.
    DCHECK_IMPLIES(have_copy_request, draw_result == DRAW_SUCCESS);

    return draw_result;
}

void LayerTreeHostImpl::MainThreadHasStoppedFlinging()
{
    top_controls_manager_->MainThreadHasStoppedFlinging();
    if (input_handler_client_)
        input_handler_client_->MainThreadHasStoppedFlinging();
}

void LayerTreeHostImpl::DidAnimateScrollOffset()
{
    client_->SetNeedsCommitOnImplThread();
    client_->RenewTreePriority();
}

void LayerTreeHostImpl::SetViewportDamage(const gfx::Rect& damage_rect)
{
    viewport_damage_rect_.Union(damage_rect);
}

DrawResult LayerTreeHostImpl::PrepareToDraw(FrameData* frame)
{
    TRACE_EVENT1("cc",
        "LayerTreeHostImpl::PrepareToDraw",
        "SourceFrameNumber",
        active_tree_->source_frame_number());
    if (input_handler_client_)
        input_handler_client_->ReconcileElasticOverscrollAndRootScroll();

    UMA_HISTOGRAM_CUSTOM_COUNTS(
        "Compositing.NumActiveLayers",
        base::saturated_cast<int>(active_tree_->NumLayers()), 1, 400, 20);

    //   if (const char* client_name = GetClientNameForMetrics()) {
    //     size_t total_picture_memory = 0;
    //     for (const PictureLayerImpl* layer : active_tree()->picture_layers())
    //       total_picture_memory += layer->GetRasterSource()->GetPictureMemoryUsage();
    //     if (total_picture_memory != 0) {
    //       // GetClientNameForMetrics only returns one non-null value over the
    //       // lifetime of the process, so this histogram name is runtime constant.
    //       UMA_HISTOGRAM_COUNTS(
    //           base::StringPrintf("Compositing.%s.PictureMemoryUsageKb",
    //                              client_name),
    //           base::saturated_cast<int>(total_picture_memory / 1024));
    //     }
    //   }

    bool update_lcd_text = false;
    bool ok = active_tree_->UpdateDrawProperties(update_lcd_text);
    DCHECK(ok) << "UpdateDrawProperties failed during draw";

    // This will cause NotifyTileStateChanged() to be called for any tiles that
    // completed, which will add damage for visible tiles to the frame for them so
    // they appear as part of the current frame being drawn.
    tile_manager_->Flush();

    frame->render_surface_layer_list = &active_tree_->RenderSurfaceLayerList();
    frame->render_passes.clear();
    frame->render_passes_by_id.clear();
    frame->will_draw_layers.clear();
    frame->has_no_damage = false;

    if (active_tree_->root_layer()) {
        gfx::Rect device_viewport_damage_rect = viewport_damage_rect_;
        viewport_damage_rect_ = gfx::Rect();

        active_tree_->root_layer()->render_surface()->damage_tracker()->AddDamageNextUpdate(device_viewport_damage_rect);
    }

    DrawResult draw_result = CalculateRenderPasses(frame);
    if (draw_result != DRAW_SUCCESS) {
        DCHECK(!output_surface_->capabilities()
                    .draw_and_swap_full_viewport_every_frame);
        return draw_result;
    }

    // If we return DRAW_SUCCESS, then we expect DrawLayers() to be called before
    // this function is called again.
    return draw_result;
}

void LayerTreeHostImpl::RemoveRenderPasses(FrameData* frame)
{
    // There is always at least a root RenderPass.
    DCHECK_GE(frame->render_passes.size(), 1u);

    // A set of RenderPasses that we have seen.
    std::set<RenderPassId> pass_exists;
    // A set of RenderPassDrawQuads that we have seen (stored by the RenderPasses
    // they refer to).
#if USING_VC6RT != 1
    base::SmallMap<base::hash_map<RenderPassId, int>> pass_references;
#else
    base::hash_map<RenderPassId, int> pass_references; // weolar
#endif
    // Iterate RenderPasses in draw order, removing empty render passes (except
    // the root RenderPass).
    for (size_t i = 0; i < frame->render_passes.size(); ++i) {
        RenderPass* pass = frame->render_passes[i];

        // Remove orphan RenderPassDrawQuads.
        for (auto it = pass->quad_list.begin(); it != pass->quad_list.end();) {
            if (it->material != DrawQuad::RENDER_PASS) {
                ++it;
                continue;
            }
            const RenderPassDrawQuad* quad = RenderPassDrawQuad::MaterialCast(*it);
            // If the RenderPass doesn't exist, we can remove the quad.
            if (pass_exists.count(quad->render_pass_id)) {
                // Otherwise, save a reference to the RenderPass so we know there's a
                // quad using it.
                pass_references[quad->render_pass_id]++;
                ++it;
            } else {
                it = pass->quad_list.EraseAndInvalidateAllPointers(it);
            }
        }

        if (i == frame->render_passes.size() - 1) {
            // Don't remove the root RenderPass.
            break;
        }

        if (pass->quad_list.empty() && pass->copy_requests.empty()) {
            // Remove the pass and decrement |i| to counter the for loop's increment,
            // so we don't skip the next pass in the loop.
            frame->render_passes_by_id.erase(pass->id);
            frame->render_passes.erase(frame->render_passes.begin() + i);
            --i;
            continue;
        }

        pass_exists.insert(pass->id);
    }

    // Remove RenderPasses that are not referenced by any draw quads or copy
    // requests (except the root RenderPass).
    for (size_t i = 0; i < frame->render_passes.size() - 1; ++i) {
        // Iterating from the back of the list to the front, skipping over the
        // back-most (root) pass, in order to remove each qualified RenderPass, and
        // drop references to earlier RenderPasses allowing them to be removed to.
        RenderPass* pass = frame->render_passes[frame->render_passes.size() - 2 - i];
        if (!pass->copy_requests.empty())
            continue;
        if (pass_references[pass->id])
            continue;

        for (auto it = pass->quad_list.begin(); it != pass->quad_list.end(); ++it) {
            if (it->material != DrawQuad::RENDER_PASS)
                continue;
            const RenderPassDrawQuad* quad = RenderPassDrawQuad::MaterialCast(*it);
            pass_references[quad->render_pass_id]--;
        }

        frame->render_passes_by_id.erase(pass->id);
        frame->render_passes.erase(frame->render_passes.end() - 2 - i);
        --i;
    }
}

void LayerTreeHostImpl::EvictTexturesForTesting()
{
    UpdateTileManagerMemoryPolicy(ManagedMemoryPolicy(0));
}

void LayerTreeHostImpl::BlockNotifyReadyToActivateForTesting(bool block)
{
    NOTREACHED();
}

void LayerTreeHostImpl::ResetTreesForTesting()
{
    if (active_tree_)
        active_tree_->DetachLayerTree();
    active_tree_ = LayerTreeImpl::create(this, active_tree()->page_scale_factor(),
        active_tree()->top_controls_shown_ratio(),
        active_tree()->elastic_overscroll());
    if (pending_tree_)
        pending_tree_->DetachLayerTree();
    pending_tree_ = nullptr;
    if (recycle_tree_)
        recycle_tree_->DetachLayerTree();
    recycle_tree_ = nullptr;
}

size_t LayerTreeHostImpl::SourceAnimationFrameNumberForTesting() const
{
    return fps_counter_->current_frame_number();
}

void LayerTreeHostImpl::UpdateTileManagerMemoryPolicy(
    const ManagedMemoryPolicy& policy)
{
    if (!resource_pool_)
        return;

    global_tile_state_.hard_memory_limit_in_bytes = 0;
    global_tile_state_.soft_memory_limit_in_bytes = 0;
    if (visible_ && policy.bytes_limit_when_visible > 0) {
        global_tile_state_.hard_memory_limit_in_bytes = policy.bytes_limit_when_visible;
        global_tile_state_.soft_memory_limit_in_bytes = (static_cast<int64>(global_tile_state_.hard_memory_limit_in_bytes) * settings_.max_memory_for_prepaint_percentage) / 100;
    }
    global_tile_state_.memory_limit_policy = ManagedMemoryPolicy::PriorityCutoffToTileMemoryLimitPolicy(
        visible_ ? policy.priority_cutoff_when_visible : gpu::MemoryAllocation::CUTOFF_ALLOW_NOTHING);
    global_tile_state_.num_resources_limit = policy.num_resources_limit;

    if (output_surface_ && global_tile_state_.hard_memory_limit_in_bytes > 0) {
        // If |global_tile_state_.hard_memory_limit_in_bytes| is greater than 0, we
        // allow the worker context to retain allocated resources. Notify the worker
        // context. If the memory policy has become zero, we'll handle the
        // notification in NotifyAllTileTasksCompleted, after in-progress work
        // finishes.
        output_surface_->SetWorkerContextShouldAggressivelyFreeResources(
            false /* aggressively_free_resources */);
    }

    DCHECK(resource_pool_);
    resource_pool_->CheckBusyResources();
    // Soft limit is used for resource pool such that memory returns to soft
    // limit after going over.
    resource_pool_->SetResourceUsageLimits(
        global_tile_state_.soft_memory_limit_in_bytes,
        global_tile_state_.num_resources_limit);

    DidModifyTilePriorities();
}

void LayerTreeHostImpl::DidModifyTilePriorities()
{
    // Mark priorities as dirty and schedule a PrepareTiles().
    tile_priorities_dirty_ = true;
    client_->SetNeedsPrepareTilesOnImplThread();
}

scoped_ptr<RasterTilePriorityQueue> LayerTreeHostImpl::BuildRasterQueue(
    TreePriority tree_priority,
    RasterTilePriorityQueue::Type type)
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::BuildRasterQueue");

    return RasterTilePriorityQueue::Create(active_tree_->picture_layers(),
        pending_tree_
            ? pending_tree_->picture_layers()
            : std::vector<PictureLayerImpl*>(),
        tree_priority, type);
}

scoped_ptr<EvictionTilePriorityQueue> LayerTreeHostImpl::BuildEvictionQueue(
    TreePriority tree_priority)
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::BuildEvictionQueue");

    scoped_ptr<EvictionTilePriorityQueue> queue(new EvictionTilePriorityQueue);
    queue->Build(active_tree_->picture_layers(),
        pending_tree_ ? pending_tree_->picture_layers()
                      : std::vector<PictureLayerImpl*>(),
        tree_priority);
    return queue;
}

void LayerTreeHostImpl::SetIsLikelyToRequireADraw(
    bool is_likely_to_require_a_draw)
{
    // Proactively tell the scheduler that we expect to draw within each vsync
    // until we get all the tiles ready to draw. If we happen to miss a required
    // for draw tile here, then we will miss telling the scheduler each frame that
    // we intend to draw so it may make worse scheduling decisions.
    is_likely_to_require_a_draw_ = is_likely_to_require_a_draw;
}

void LayerTreeHostImpl::NotifyReadyToActivate()
{
    client_->NotifyReadyToActivate();
}

void LayerTreeHostImpl::NotifyReadyToDraw()
{
    // Tiles that are ready will cause NotifyTileStateChanged() to be called so we
    // don't need to schedule a draw here. Just stop WillBeginImplFrame() from
    // causing optimistic requests to draw a frame.
    is_likely_to_require_a_draw_ = false;

    client_->NotifyReadyToDraw();
}

void LayerTreeHostImpl::NotifyAllTileTasksCompleted()
{
    // The tile tasks started by the most recent call to PrepareTiles have
    // completed. Now is a good time to free resources if necessary.
    if (output_surface_ && global_tile_state_.hard_memory_limit_in_bytes == 0) {
        output_surface_->SetWorkerContextShouldAggressivelyFreeResources(
            true /* aggressively_free_resources */);
    }
}

void LayerTreeHostImpl::NotifyTileStateChanged(const Tile* tile)
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::NotifyTileStateChanged");

    if (active_tree_) {
        LayerImpl* layer_impl = active_tree_->FindActiveTreeLayerById(tile->layer_id());
        if (layer_impl)
            layer_impl->NotifyTileStateChanged(tile);
    }

    if (pending_tree_) {
        LayerImpl* layer_impl = pending_tree_->FindPendingTreeLayerById(tile->layer_id());
        if (layer_impl)
            layer_impl->NotifyTileStateChanged(tile);
    }

    // Check for a non-null active tree to avoid doing this during shutdown.
    if (active_tree_ && !client_->IsInsideDraw() && tile->required_for_draw()) {
        // The LayerImpl::NotifyTileStateChanged() should damage the layer, so this
        // redraw will make those tiles be displayed.
        SetNeedsRedraw();
    }
}

void LayerTreeHostImpl::SetMemoryPolicy(const ManagedMemoryPolicy& policy)
{
    SetManagedMemoryPolicy(policy);

    // This is short term solution to synchronously drop tile resources when
    // using synchronous compositing to avoid memory usage regression.
    // TODO(boliu): crbug.com/499004 to track removing this.
    if (!policy.bytes_limit_when_visible && resource_pool_ && settings_.using_synchronous_renderer_compositor) {
        ReleaseTreeResources();
        CleanUpTileManager();

        // Force a call to NotifyAllTileTasks completed - otherwise this logic may
        // be skipped if no work was enqueued at the time the tile manager was
        // destroyed.
        NotifyAllTileTasksCompleted();

        CreateTileManagerResources();
        RecreateTreeResources();
    }
}

void LayerTreeHostImpl::SetTreeActivationCallback(
    const base::Closure& callback)
{
    DCHECK(proxy_->IsImplThread());
    tree_activation_callback_ = callback;
}

void LayerTreeHostImpl::SetManagedMemoryPolicy(
    const ManagedMemoryPolicy& policy)
{
    if (cached_managed_memory_policy_ == policy)
        return;

    ManagedMemoryPolicy old_policy = ActualManagedMemoryPolicy();

    cached_managed_memory_policy_ = policy;
    ManagedMemoryPolicy actual_policy = ActualManagedMemoryPolicy();

    if (old_policy == actual_policy)
        return;

    if (!proxy_->HasImplThread()) {
        // In single-thread mode, this can be called on the main thread by
        // GLRenderer::OnMemoryAllocationChanged.
        DebugScopedSetImplThread impl_thread(proxy_);
        UpdateTileManagerMemoryPolicy(actual_policy);
    } else {
        DCHECK(proxy_->IsImplThread());
        UpdateTileManagerMemoryPolicy(actual_policy);
    }

    // If there is already enough memory to draw everything imaginable and the
    // new memory limit does not change this, then do not re-commit. Don't bother
    // skipping commits if this is not visible (commits don't happen when not
    // visible, there will almost always be a commit when this becomes visible).
    bool needs_commit = true;
    if (visible() && actual_policy.bytes_limit_when_visible >= max_memory_needed_bytes_ && old_policy.bytes_limit_when_visible >= max_memory_needed_bytes_ && actual_policy.priority_cutoff_when_visible == old_policy.priority_cutoff_when_visible) {
        needs_commit = false;
    }

    if (needs_commit)
        client_->SetNeedsCommitOnImplThread();
}

void LayerTreeHostImpl::SetExternalDrawConstraints(
    const gfx::Transform& transform,
    const gfx::Rect& viewport,
    const gfx::Rect& clip,
    const gfx::Rect& viewport_rect_for_tile_priority,
    const gfx::Transform& transform_for_tile_priority,
    bool resourceless_software_draw)
{
    gfx::Rect viewport_rect_for_tile_priority_in_view_space;
    if (!resourceless_software_draw) {
        gfx::Transform screen_to_view(gfx::Transform::kSkipInitialization);
        if (transform_for_tile_priority.GetInverse(&screen_to_view)) {
            // Convert from screen space to view space.
            viewport_rect_for_tile_priority_in_view_space = MathUtil::ProjectEnclosingClippedRect(
                screen_to_view, viewport_rect_for_tile_priority);
        }
    }

    if (external_transform_ != transform || external_viewport_ != viewport || resourceless_software_draw_ != resourceless_software_draw || viewport_rect_for_tile_priority_ != viewport_rect_for_tile_priority_in_view_space) {
        active_tree_->set_needs_update_draw_properties();
    }

    external_transform_ = transform;
    external_viewport_ = viewport;
    external_clip_ = clip;
    viewport_rect_for_tile_priority_ = viewport_rect_for_tile_priority_in_view_space;
    resourceless_software_draw_ = resourceless_software_draw;
}

void LayerTreeHostImpl::SetNeedsRedrawRect(const gfx::Rect& damage_rect)
{
    if (damage_rect.IsEmpty())
        return;
    NotifySwapPromiseMonitorsOfSetNeedsRedraw();
    client_->SetNeedsRedrawRectOnImplThread(damage_rect);
}

void LayerTreeHostImpl::DidSwapBuffers()
{
    client_->DidSwapBuffersOnImplThread();
}

void LayerTreeHostImpl::DidSwapBuffersComplete()
{
    client_->DidSwapBuffersCompleteOnImplThread();
}

void LayerTreeHostImpl::ReclaimResources(const CompositorFrameAck* ack)
{
    // TODO(piman): We may need to do some validation on this ack before
    // processing it.
    if (renderer_)
        renderer_->ReceiveSwapBuffersAck(*ack);

    // In OOM, we now might be able to release more resources that were held
    // because they were exported.
    if (resource_pool_) {
        resource_pool_->CheckBusyResources();
        resource_pool_->ReduceResourceUsage();
    }
    // If we're not visible, we likely released resources, so we want to
    // aggressively flush here to make sure those DeleteTextures make it to the
    // GPU process to free up the memory.
    if (output_surface_->context_provider() && !visible_) {
        output_surface_->context_provider()->ContextGL()->ShallowFlushCHROMIUM();
    }
}

void LayerTreeHostImpl::OnDraw()
{
    client_->OnDrawForOutputSurface();
}

void LayerTreeHostImpl::OnCanDrawStateChangedForTree()
{
    client_->OnCanDrawStateChanged(CanDraw());
}

CompositorFrameMetadata LayerTreeHostImpl::MakeCompositorFrameMetadata() const
{
    CompositorFrameMetadata metadata;
    metadata.device_scale_factor = active_tree_->device_scale_factor();
    metadata.page_scale_factor = active_tree_->current_page_scale_factor();
    metadata.scrollable_viewport_size = active_tree_->ScrollableViewportSize();
    metadata.root_layer_size = active_tree_->ScrollableSize();
    metadata.min_page_scale_factor = active_tree_->min_page_scale_factor();
    metadata.max_page_scale_factor = active_tree_->max_page_scale_factor();
    metadata.location_bar_offset = gfx::Vector2dF(0.f, top_controls_manager_->ControlsTopOffset());
    metadata.location_bar_content_translation = gfx::Vector2dF(0.f, top_controls_manager_->ContentTopOffset());
    metadata.root_background_color = active_tree_->background_color();

    active_tree_->GetViewportSelection(&metadata.selection);

    if (OuterViewportScrollLayer()) {
        metadata.root_overflow_x_hidden = !OuterViewportScrollLayer()->user_scrollable_horizontal();
        metadata.root_overflow_y_hidden = !OuterViewportScrollLayer()->user_scrollable_vertical();
    }

    if (!InnerViewportScrollLayer())
        return metadata;

    metadata.root_overflow_x_hidden |= !InnerViewportScrollLayer()->user_scrollable_horizontal();
    metadata.root_overflow_y_hidden |= !InnerViewportScrollLayer()->user_scrollable_vertical();

    // TODO(miletus) : Change the metadata to hold ScrollOffset.
    metadata.root_scroll_offset = gfx::ScrollOffsetToVector2dF(
        active_tree_->TotalScrollOffset());

    return metadata;
}

void LayerTreeHostImpl::DrawLayers(FrameData* frame)
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::DrawLayers");

    base::TimeTicks frame_begin_time = CurrentBeginFrameArgs().frame_time;
    DCHECK(CanDraw());

    if (!frame->composite_events.empty()) {
        frame_timing_tracker_->SaveTimeStamps(frame_begin_time,
            frame->composite_events);
    }

    if (frame->has_no_damage) {
        TRACE_EVENT_INSTANT0("cc", "EarlyOut_NoDamage", TRACE_EVENT_SCOPE_THREAD);
        DCHECK(!output_surface_->capabilities()
                    .draw_and_swap_full_viewport_every_frame);
        return;
    }

    DCHECK(!frame->render_passes.empty());

    fps_counter_->SaveTimeStamp(frame_begin_time,
        !output_surface_->context_provider());
    rendering_stats_instrumentation_->IncrementFrameCount(1);

    memory_history_->SaveEntry(tile_manager_->memory_stats_from_last_assign());

    if (debug_state_.ShowHudRects()) {
        debug_rect_history_->SaveDebugRectsForCurrentFrame(
            active_tree_->root_layer(),
            active_tree_->hud_layer(),
            *frame->render_surface_layer_list,
            debug_state_);
    }

    bool is_new_trace = false;
    TRACE_EVENT_IS_NEW_TRACE(&is_new_trace);
    if (is_new_trace) {
        if (pending_tree_) {
            LayerTreeHostCommon::CallFunctionForSubtree(
                pending_tree_->root_layer(),
                [](LayerImpl* layer) { layer->DidBeginTracing(); });
        }
        LayerTreeHostCommon::CallFunctionForSubtree(
            active_tree_->root_layer(),
            [](LayerImpl* layer) { layer->DidBeginTracing(); });
    }

    {
        TRACE_EVENT0("cc", "DrawLayers.FrameViewerTracing");
        TRACE_EVENT_OBJECT_SNAPSHOT_WITH_ID(
            frame_viewer_instrumentation::kCategoryLayerTree,
            "cc::LayerTreeHostImpl", id_, AsValueWithFrame(frame));
    }

    const DrawMode draw_mode = GetDrawMode();

    // Because the contents of the HUD depend on everything else in the frame, the
    // contents of its texture are updated as the last thing before the frame is
    // drawn.
    if (active_tree_->hud_layer()) {
        TRACE_EVENT0("cc", "DrawLayers.UpdateHudTexture");
        active_tree_->hud_layer()->UpdateHudTexture(draw_mode,
            resource_provider_.get());
    }

    if (draw_mode == DRAW_MODE_RESOURCELESS_SOFTWARE) {
        bool disable_picture_quad_image_filtering = IsActivelyScrolling() || (animation_host_ ? animation_host_->NeedsAnimateLayers() : animation_registrar_->needs_animate_layers());

        scoped_ptr<SoftwareRenderer> temp_software_renderer = SoftwareRenderer::Create(this, &settings_.renderer_settings,
            output_surface_, NULL);
        temp_software_renderer->DrawFrame(
            &frame->render_passes, active_tree_->device_scale_factor(),
            DeviceViewport(), DeviceClip(), disable_picture_quad_image_filtering);
    } else {
        renderer_->DrawFrame(&frame->render_passes,
            active_tree_->device_scale_factor(), DeviceViewport(),
            DeviceClip(), false);
    }
    // The render passes should be consumed by the renderer.
    DCHECK(frame->render_passes.empty());
    frame->render_passes_by_id.clear();

    // The next frame should start by assuming nothing has changed, and changes
    // are noted as they occur.
    // TODO(boliu): If we did a temporary software renderer frame, propogate the
    // damage forward to the next frame.
    for (size_t i = 0; i < frame->render_surface_layer_list->size(); i++) {
        (*frame->render_surface_layer_list)[i]->render_surface()->damage_tracker()->DidDrawDamagedArea();
    }
    active_tree_->root_layer()->ResetAllChangeTrackingForSubtree();

    active_tree_->set_has_ever_been_drawn(true);
    devtools_instrumentation::DidDrawFrame(id_);
    benchmark_instrumentation::IssueImplThreadRenderingStatsEvent(
        rendering_stats_instrumentation_->impl_thread_rendering_stats());
    rendering_stats_instrumentation_->AccumulateAndClearImplThreadStats();
}

void LayerTreeHostImpl::DidDrawAllLayers(const FrameData& frame)
{
    for (size_t i = 0; i < frame.will_draw_layers.size(); ++i)
        frame.will_draw_layers[i]->DidDraw(resource_provider_.get());

    for (auto& it : video_frame_controllers_)
        it->DidDrawFrame();
}

void LayerTreeHostImpl::FinishAllRendering()
{
    if (renderer_)
        renderer_->Finish();
}

int LayerTreeHostImpl::RequestedMSAASampleCount() const
{
    if (settings_.gpu_rasterization_msaa_sample_count == -1) {
        // Use the most up-to-date version of device_scale_factor that we have.
        float device_scale_factor = pending_tree_
            ? pending_tree_->device_scale_factor()
            : active_tree_->device_scale_factor();
        return device_scale_factor >= 2.0f ? 4 : 8;
    }

    return settings_.gpu_rasterization_msaa_sample_count;
}

bool LayerTreeHostImpl::CanUseGpuRasterization()
{
    if (!(output_surface_ && output_surface_->context_provider() && output_surface_->worker_context_provider()))
        return false;

    ContextProvider* context_provider = output_surface_->worker_context_provider();
    ContextProvider::ScopedContextLock scoped_context(context_provider);
    if (!context_provider->GrContext())
        return false;

    return true;
}

void LayerTreeHostImpl::UpdateGpuRasterizationStatus()
{
    bool use_gpu = false;
    bool use_msaa = false;
    bool using_msaa_for_complex_content = renderer() && RequestedMSAASampleCount() > 0 && GetRendererCapabilities().max_msaa_samples >= RequestedMSAASampleCount();
    if (settings_.gpu_rasterization_forced) {
        use_gpu = true;
        gpu_rasterization_status_ = GpuRasterizationStatus::ON_FORCED;
        use_msaa = !content_is_suitable_for_gpu_rasterization_ && using_msaa_for_complex_content;
        if (use_msaa) {
            gpu_rasterization_status_ = GpuRasterizationStatus::MSAA_CONTENT;
        }
    } else if (!settings_.gpu_rasterization_enabled) {
        gpu_rasterization_status_ = GpuRasterizationStatus::OFF_DEVICE;
    } else if (!has_gpu_rasterization_trigger_) {
        gpu_rasterization_status_ = GpuRasterizationStatus::OFF_VIEWPORT;
    } else if (content_is_suitable_for_gpu_rasterization_) {
        use_gpu = true;
        gpu_rasterization_status_ = GpuRasterizationStatus::ON;
    } else if (using_msaa_for_complex_content) {
        use_gpu = use_msaa = true;
        gpu_rasterization_status_ = GpuRasterizationStatus::MSAA_CONTENT;
    } else {
        gpu_rasterization_status_ = GpuRasterizationStatus::OFF_CONTENT;
    }

    if (use_gpu && !use_gpu_rasterization_) {
        if (!CanUseGpuRasterization()) {
            // If GPU rasterization is unusable, e.g. if GlContext could not
            // be created due to losing the GL context, force use of software
            // raster.
            use_gpu = false;
            use_msaa = false;
            gpu_rasterization_status_ = GpuRasterizationStatus::OFF_DEVICE;
        }
    }

    if (use_gpu == use_gpu_rasterization_ && use_msaa == use_msaa_)
        return;

    // Note that this must happen first, in case the rest of the calls want to
    // query the new state of |use_gpu_rasterization_|.
    use_gpu_rasterization_ = use_gpu;
    use_msaa_ = use_msaa;

    tree_resources_for_gpu_rasterization_dirty_ = true;
}

void LayerTreeHostImpl::UpdateTreeResourcesForGpuRasterizationIfNeeded()
{
    if (!tree_resources_for_gpu_rasterization_dirty_)
        return;

    // Clean up and replace existing tile manager with another one that uses
    // appropriate rasterizer. Only do this however if we already have a
    // resource pool, since otherwise we might not be able to create a new
    // one.
    ReleaseTreeResources();
    if (resource_pool_) {
        CleanUpTileManager();
        CreateTileManagerResources();
    }
    RecreateTreeResources();

    // We have released tilings for both active and pending tree.
    // We would not have any content to draw until the pending tree is activated.
    // Prevent the active tree from drawing until activation.
    SetRequiresHighResToDraw();

    tree_resources_for_gpu_rasterization_dirty_ = false;
}

const RendererCapabilitiesImpl&
LayerTreeHostImpl::GetRendererCapabilities() const
{
    CHECK(renderer_);
    return renderer_->Capabilities();
}

bool LayerTreeHostImpl::SwapBuffers(const LayerTreeHostImpl::FrameData& frame)
{
    ResetRequiresHighResToDraw();
    if (frame.has_no_damage) {
        active_tree()->BreakSwapPromises(SwapPromise::SWAP_FAILS);
        return false;
    }
    CompositorFrameMetadata metadata = MakeCompositorFrameMetadata();
    active_tree()->FinishSwapPromises(&metadata);
    for (auto& latency : metadata.latency_info) {
        TRACE_EVENT_WITH_FLOW1("input,benchmark",
            "LatencyInfo.Flow",
            TRACE_ID_DONT_MANGLE(latency.trace_id()),
            TRACE_EVENT_FLAG_FLOW_IN | TRACE_EVENT_FLAG_FLOW_OUT,
            "step", "SwapBuffers");
        // Only add the latency component once for renderer swap, not the browser
        // swap.
        if (!latency.FindLatency(ui::INPUT_EVENT_LATENCY_RENDERER_SWAP_COMPONENT, 0, nullptr)) {
            latency.AddLatencyNumber(ui::INPUT_EVENT_LATENCY_RENDERER_SWAP_COMPONENT, 0, 0);
        }
    }
    renderer_->SwapBuffers(metadata);
    return true;
}

void LayerTreeHostImpl::WillBeginImplFrame(const BeginFrameArgs& args)
{
    current_begin_frame_tracker_.Start(args);

    if (is_likely_to_require_a_draw_) {
        // Optimistically schedule a draw. This will let us expect the tile manager
        // to complete its work so that we can draw new tiles within the impl frame
        // we are beginning now.
        SetNeedsRedraw();
    }

    for (std::set<VideoFrameController *>::const_iterator it = video_frame_controllers_.begin(); it != video_frame_controllers_.end(); ++it) {
        VideoFrameController* i = *it;
        i->OnBeginFrame(args);
    }
}

void LayerTreeHostImpl::DidFinishImplFrame()
{
    current_begin_frame_tracker_.Finish();
}

void LayerTreeHostImpl::UpdateViewportContainerSizes()
{
    LayerImpl* inner_container = active_tree_->InnerViewportContainerLayer();
    LayerImpl* outer_container = active_tree_->OuterViewportContainerLayer();

    if (!inner_container)
        return;

    ViewportAnchor anchor(InnerViewportScrollLayer(), OuterViewportScrollLayer());

    float top_controls_layout_height = active_tree_->top_controls_shrink_blink_size()
        ? active_tree_->top_controls_height()
        : 0.f;
    float delta_from_top_controls = top_controls_layout_height - top_controls_manager_->ContentTopOffset();

    // Adjust the viewport layers by shrinking/expanding the container to account
    // for changes in the size (e.g. top controls) since the last resize from
    // Blink.
    gfx::Vector2dF amount_to_expand(
        0.f,
        delta_from_top_controls);
    inner_container->SetBoundsDelta(amount_to_expand);

    if (outer_container && !outer_container->BoundsForScrolling().IsEmpty()) {
        // Adjust the outer viewport container as well, since adjusting only the
        // inner may cause its bounds to exceed those of the outer, causing scroll
        // clamping.
        gfx::Vector2dF amount_to_expand_scaled = gfx::ScaleVector2d(
            amount_to_expand, 1.f / active_tree_->min_page_scale_factor());
        outer_container->SetBoundsDelta(amount_to_expand_scaled);
        active_tree_->InnerViewportScrollLayer()->SetBoundsDelta(
            amount_to_expand_scaled);

        anchor.ResetViewportToAnchoredPosition();
    }
}

void LayerTreeHostImpl::SynchronouslyInitializeAllTiles()
{
    // Only valid for the single-threaded non-scheduled/synchronous case
    // using the zero copy raster worker pool.
    single_thread_synchronous_task_graph_runner_->RunUntilIdle();
}

void LayerTreeHostImpl::DidLoseOutputSurface()
{
    if (resource_provider_)
        resource_provider_->DidLoseOutputSurface();
    client_->DidLoseOutputSurfaceOnImplThread();
}

bool LayerTreeHostImpl::HaveRootScrollLayer() const
{
    return !!InnerViewportScrollLayer();
}

LayerImpl* LayerTreeHostImpl::RootLayer() const
{
    return active_tree_->root_layer();
}

LayerImpl* LayerTreeHostImpl::InnerViewportScrollLayer() const
{
    return active_tree_->InnerViewportScrollLayer();
}

LayerImpl* LayerTreeHostImpl::OuterViewportScrollLayer() const
{
    return active_tree_->OuterViewportScrollLayer();
}

LayerImpl* LayerTreeHostImpl::CurrentlyScrollingLayer() const
{
    return active_tree_->CurrentlyScrollingLayer();
}

bool LayerTreeHostImpl::IsActivelyScrolling() const
{
    if (!CurrentlyScrollingLayer())
        return false;
    // On Android WebView root flings are controlled by the application,
    // so the compositor does not animate them and can't tell if they
    // are actually animating. So assume there are none.
    if (settings_.ignore_root_layer_flings && IsCurrentlyScrollingInnerViewport())
        return false;
    return did_lock_scrolling_layer_;
}

// Content layers can be either directly scrollable or contained in an outer
// scrolling layer which applies the scroll transform. Given a content layer,
// this function returns the associated scroll layer if any.
static LayerImpl* FindScrollLayerForContentLayer(LayerImpl* layer_impl)
{
    if (!layer_impl)
        return NULL;

    if (layer_impl->scrollable())
        return layer_impl;

    if (layer_impl->DrawsContent() && layer_impl->parent() && layer_impl->parent()->scrollable())
        return layer_impl->parent();

    return NULL;
}

void LayerTreeHostImpl::CreatePendingTree()
{
    CHECK(!pending_tree_);
    if (recycle_tree_)
        recycle_tree_.swap(pending_tree_);
    else
        pending_tree_ = LayerTreeImpl::create(this, active_tree()->page_scale_factor(),
            active_tree()->top_controls_shown_ratio(),
            active_tree()->elastic_overscroll());

    client_->OnCanDrawStateChanged(CanDraw());
    TRACE_EVENT_ASYNC_BEGIN0("cc", "PendingTree:waiting", pending_tree_.get());
}

void LayerTreeHostImpl::ActivateSyncTree()
{
    if (pending_tree_) {
        TRACE_EVENT_ASYNC_END0("cc", "PendingTree:waiting", pending_tree_.get());

        // Process any requests in the UI resource queue.  The request queue is
        // given in LayerTreeHost::FinishCommitOnImplThread.  This must take place
        // before the swap.
        pending_tree_->ProcessUIResourceRequestQueue();

        if (pending_tree_->needs_full_tree_sync()) {
            active_tree_->SetRootLayer(
                TreeSynchronizer::SynchronizeTrees(pending_tree_->root_layer(),
                    active_tree_->DetachLayerTree(),
                    active_tree_.get()));
        }
        TreeSynchronizer::PushProperties(pending_tree_->root_layer(),
            active_tree_->root_layer());
        pending_tree_->PushPropertiesTo(active_tree_.get());

        // Now that we've synced everything from the pending tree to the active
        // tree, rename the pending tree the recycle tree so we can reuse it on the
        // next sync.
        DCHECK(!recycle_tree_);
        pending_tree_.swap(recycle_tree_);

        UpdateViewportContainerSizes();

        // If we commit to the active tree directly, this is already done during
        // commit.
        ActivateAnimations();
    } else {
        active_tree_->ProcessUIResourceRequestQueue();
    }

    // bounds_delta isn't a pushed property, so the newly-pushed property tree
    // won't already account for current bounds_delta values.
    active_tree_->UpdatePropertyTreesForBoundsDelta();
    active_tree_->DidBecomeActive();
    client_->RenewTreePriority();
    // If we have any picture layers, then by activating we also modified tile
    // priorities.
    if (!active_tree_->picture_layers().empty())
        DidModifyTilePriorities();

    client_->OnCanDrawStateChanged(CanDraw());
    client_->DidActivateSyncTree();
    if (!tree_activation_callback_.is_null())
        tree_activation_callback_.Run();

    scoped_ptr<PendingPageScaleAnimation> pending_page_scale_animation = active_tree_->TakePendingPageScaleAnimation();
    if (pending_page_scale_animation) {
        StartPageScaleAnimation(
            pending_page_scale_animation->target_offset,
            pending_page_scale_animation->use_anchor,
            pending_page_scale_animation->scale,
            pending_page_scale_animation->duration);
    }
    // Activation can change the root scroll offset, so inform the synchronous
    // input handler.
    UpdateRootLayerStateForSynchronousInputHandler();
}

void LayerTreeHostImpl::SetVisible(bool visible)
{
    DCHECK(proxy_->IsImplThread());

    if (visible_ == visible)
        return;
    visible_ = visible;
    DidVisibilityChange(this, visible_);
    UpdateTileManagerMemoryPolicy(ActualManagedMemoryPolicy());

    // If we just became visible, we have to ensure that we draw high res tiles,
    // to prevent checkerboard/low res flashes.
    if (visible_)
        SetRequiresHighResToDraw();
    else
        EvictAllUIResources();

    // Call PrepareTiles to evict tiles when we become invisible.
    if (!visible)
        PrepareTiles();

    if (!renderer_)
        return;

    renderer_->SetVisible(visible);
}

void LayerTreeHostImpl::SetNeedsAnimate()
{
    NotifySwapPromiseMonitorsOfSetNeedsRedraw();
    client_->SetNeedsAnimateOnImplThread();
}

void LayerTreeHostImpl::SetNeedsRedraw()
{
    NotifySwapPromiseMonitorsOfSetNeedsRedraw();
    client_->SetNeedsRedrawOnImplThread();
}

ManagedMemoryPolicy LayerTreeHostImpl::ActualManagedMemoryPolicy() const
{
    ManagedMemoryPolicy actual = cached_managed_memory_policy_;
    if (debug_state_.rasterize_only_visible_content) {
        actual.priority_cutoff_when_visible = gpu::MemoryAllocation::CUTOFF_ALLOW_REQUIRED_ONLY;
    } else if (use_gpu_rasterization()) {
        actual.priority_cutoff_when_visible = gpu::MemoryAllocation::CUTOFF_ALLOW_NICE_TO_HAVE;
    }
    return actual;
}

size_t LayerTreeHostImpl::memory_allocation_limit_bytes() const
{
    return ActualManagedMemoryPolicy().bytes_limit_when_visible;
}

void LayerTreeHostImpl::ReleaseTreeResources()
{
    active_tree_->ReleaseResources();
    if (pending_tree_)
        pending_tree_->ReleaseResources();
    if (recycle_tree_)
        recycle_tree_->ReleaseResources();

    EvictAllUIResources();
}

void LayerTreeHostImpl::RecreateTreeResources()
{
    active_tree_->RecreateResources();
    if (pending_tree_)
        pending_tree_->RecreateResources();
    if (recycle_tree_)
        recycle_tree_->RecreateResources();
}

void LayerTreeHostImpl::CreateAndSetRenderer()
{
    DCHECK(!renderer_);
    DCHECK(output_surface_);
    DCHECK(resource_provider_);

    if (output_surface_->capabilities().delegated_rendering) {
        renderer_ = DelegatingRenderer::Create(this, &settings_.renderer_settings,
            output_surface_, resource_provider_.get());
    } else if (output_surface_->context_provider()) {
        renderer_ = GLRenderer::Create(
            this, &settings_.renderer_settings, output_surface_,
            resource_provider_.get(), texture_mailbox_deleter_.get(),
            settings_.renderer_settings.highp_threshold_min);
    } else if (output_surface_->software_device()) {
        renderer_ = SoftwareRenderer::Create(this, &settings_.renderer_settings,
            output_surface_, resource_provider_.get());
    }
    DCHECK(renderer_);

    renderer_->SetVisible(visible_);
    SetFullRootLayerDamage();

    // See note in LayerTreeImpl::UpdateDrawProperties.  Renderer needs to be
    // initialized to get max texture size.  Also, after releasing resources,
    // trees need another update to generate new ones.
    active_tree_->set_needs_update_draw_properties();
    if (pending_tree_)
        pending_tree_->set_needs_update_draw_properties();
    client_->UpdateRendererCapabilitiesOnImplThread();
}

void LayerTreeHostImpl::CreateTileManagerResources()
{
    CreateResourceAndTileTaskWorkerPool(&tile_task_worker_pool_, &resource_pool_);
    // TODO(vmpstr): Initialize tile task limit at ctor time.
    tile_manager_->SetResources(
        resource_pool_.get(), tile_task_worker_pool_->AsTileTaskRunner(),
        is_synchronous_single_threaded_ ? std::numeric_limits<size_t>::max()
                                        : settings_.scheduled_raster_task_limit);
    UpdateTileManagerMemoryPolicy(ActualManagedMemoryPolicy());
}

void LayerTreeHostImpl::CreateResourceAndTileTaskWorkerPool(
    scoped_ptr<TileTaskWorkerPool>* tile_task_worker_pool,
    scoped_ptr<ResourcePool>* resource_pool)
{
    DCHECK(GetTaskRunner());
    // TODO(vmpstr): Make this a DCHECK (or remove) when crbug.com/419086 is
    // resolved.
    CHECK(resource_provider_);

    // Pass the single-threaded synchronous task graph runner to the worker pool
    // if we're in synchronous single-threaded mode.
    TaskGraphRunner* task_graph_runner = task_graph_runner_;
    if (is_synchronous_single_threaded_) {
        DCHECK(!single_thread_synchronous_task_graph_runner_);
        single_thread_synchronous_task_graph_runner_.reset(new TaskGraphRunner);
        task_graph_runner = single_thread_synchronous_task_graph_runner_.get();
    }

    ContextProvider* context_provider = output_surface_->context_provider();
    if (!context_provider) {
        *resource_pool = ResourcePool::Create(resource_provider_.get(),
            GetTaskRunner(), GL_TEXTURE_2D);

        *tile_task_worker_pool = BitmapTileTaskWorkerPool::Create(
            GetTaskRunner(), task_graph_runner, resource_provider_.get());
        return;
    }

    if (use_gpu_rasterization_) {
        DCHECK(resource_provider_->output_surface()->worker_context_provider());

        *resource_pool = ResourcePool::Create(resource_provider_.get(),
            GetTaskRunner(), GL_TEXTURE_2D);

        int msaa_sample_count = use_msaa_ ? RequestedMSAASampleCount() : 0;

        *tile_task_worker_pool = GpuTileTaskWorkerPool::Create(
            GetTaskRunner(), task_graph_runner, context_provider,
            resource_provider_.get(), settings_.use_distance_field_text,
            msaa_sample_count);
        return;
    }

    DCHECK(GetRendererCapabilities().using_image);

    bool use_zero_copy = settings_.use_zero_copy;
    // TODO(reveman): Remove this when mojo supports worker contexts.
    // crbug.com/522440
    if (!resource_provider_->output_surface()->worker_context_provider()) {
        LOG(ERROR)
            << "Forcing zero-copy tile initialization as worker context is missing";
        use_zero_copy = true;
    }

    if (use_zero_copy) {
        *resource_pool = ResourcePool::Create(resource_provider_.get(), GetTaskRunner());

        *tile_task_worker_pool = ZeroCopyTileTaskWorkerPool::Create(
            GetTaskRunner(), task_graph_runner, resource_provider_.get(),
            settings_.renderer_settings.use_rgba_4444_textures);
        return;
    }

    *resource_pool = ResourcePool::Create(resource_provider_.get(),
        GetTaskRunner(), GL_TEXTURE_2D);

    int max_copy_texture_chromium_size = context_provider->ContextCapabilities()
                                             .gpu.max_copy_texture_chromium_size;

    *tile_task_worker_pool = OneCopyTileTaskWorkerPool::Create(
        GetTaskRunner(), task_graph_runner, context_provider,
        resource_provider_.get(), max_copy_texture_chromium_size,
        settings_.use_persistent_map_for_gpu_memory_buffers,
        settings_.max_staging_buffer_usage_in_bytes,
        settings_.renderer_settings.use_rgba_4444_textures);
}

void LayerTreeHostImpl::RecordMainFrameTiming(
    const BeginFrameArgs& start_of_main_frame_args,
    const BeginFrameArgs& expected_next_main_frame_args)
{
    std::vector<int64_t> request_ids;
    active_tree_->GatherFrameTimingRequestIds(&request_ids);
    if (request_ids.empty())
        return;

    base::TimeTicks start_time = start_of_main_frame_args.frame_time;
    base::TimeTicks end_time = expected_next_main_frame_args.frame_time;
    frame_timing_tracker_->SaveMainFrameTimeStamps(
        request_ids, start_time, end_time, active_tree_->source_frame_number());
}

void LayerTreeHostImpl::PostFrameTimingEvents(
    scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
    scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events)
{
    client_->PostFrameTimingEventsOnImplThread(composite_events.Pass(),
        main_frame_events.Pass());
}

void LayerTreeHostImpl::CleanUpTileManager()
{
    tile_manager_->FinishTasksAndCleanUp();
    resource_pool_ = nullptr;
    tile_task_worker_pool_ = nullptr;
    single_thread_synchronous_task_graph_runner_ = nullptr;
}

void LayerTreeHostImpl::ReleaseOutputSurface()
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::ReleaseOutputSurface");

    // Since we will create a new resource provider, we cannot continue to use
    // the old resources (i.e. render_surfaces and texture IDs). Clear them
    // before we destroy the old resource provider.
    ReleaseTreeResources();

    // Note: order is important here.
    renderer_ = nullptr;
    CleanUpTileManager();
    resource_provider_ = nullptr;

    // Detach from the old output surface and reset |output_surface_| pointer
    // as this surface is going to be destroyed independent of if binding the
    // new output surface succeeds or not.
    if (output_surface_) {
        output_surface_->DetachFromClient();
        output_surface_ = nullptr;
    }
}

bool LayerTreeHostImpl::InitializeRenderer(OutputSurface* output_surface)
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::InitializeRenderer");

    ReleaseOutputSurface();
    if (!output_surface->BindToClient(this)) {
        // Avoid recreating tree resources because we might not have enough
        // information to do this yet (eg. we don't have a TileManager at this
        // point).
        return false;
    }

    output_surface_ = output_surface;
    resource_provider_ = ResourceProvider::Create(
        output_surface_, shared_bitmap_manager_, gpu_memory_buffer_manager_,
        proxy_->blocking_main_thread_task_runner(),
        settings_.renderer_settings.highp_threshold_min,
        settings_.renderer_settings.texture_id_allocation_chunk_size,
        settings_.use_image_texture_targets);

    CreateAndSetRenderer();

    // Since the new renderer may be capable of MSAA, update status here.
    UpdateGpuRasterizationStatus();

    CreateTileManagerResources();
    RecreateTreeResources();

    // Initialize vsync parameters to sane values.
    const base::TimeDelta display_refresh_interval = base::TimeDelta::FromMicroseconds(
        base::Time::kMicrosecondsPerSecond / settings_.renderer_settings.refresh_rate);
    CommitVSyncParameters(base::TimeTicks(), display_refresh_interval);

    // TODO(brianderson): Don't use a hard-coded parent draw time.
    base::TimeDelta parent_draw_time = (!settings_.use_external_begin_frame_source && output_surface_->capabilities().adjust_deadline_for_parent)
        ? BeginFrameArgs::DefaultEstimatedParentDrawTime()
        : base::TimeDelta();
    client_->SetEstimatedParentDrawTime(parent_draw_time);

    int max_frames_pending = output_surface_->capabilities().max_frames_pending;
    if (max_frames_pending <= 0)
        max_frames_pending = OutputSurface::DEFAULT_MAX_FRAMES_PENDING;
    client_->SetMaxSwapsPendingOnImplThread(max_frames_pending);
    client_->OnCanDrawStateChanged(CanDraw());

    // There will not be anything to draw here, so set high res
    // to avoid checkerboards, typically when we are recovering
    // from lost context.
    SetRequiresHighResToDraw();

    return true;
}

void LayerTreeHostImpl::CommitVSyncParameters(base::TimeTicks timebase,
    base::TimeDelta interval)
{
    client_->CommitVSyncParameters(timebase, interval);
}

void LayerTreeHostImpl::SetViewportSize(const gfx::Size& device_viewport_size)
{
    if (device_viewport_size == device_viewport_size_)
        return;
    TRACE_EVENT_INSTANT2("cc", "LayerTreeHostImpl::SetViewportSize",
        TRACE_EVENT_SCOPE_THREAD, "width",
        device_viewport_size.width(), "height",
        device_viewport_size.height());

    if (pending_tree_)
        active_tree_->SetViewportSizeInvalid();

    device_viewport_size_ = device_viewport_size;

    UpdateViewportContainerSizes();
    client_->OnCanDrawStateChanged(CanDraw());
    SetFullRootLayerDamage();
    active_tree_->set_needs_update_draw_properties();
    active_tree_->property_trees()->clip_tree.SetViewportClip(
        gfx::RectF(gfx::SizeF(device_viewport_size)));
}

const gfx::Rect LayerTreeHostImpl::ViewportRectForTilePriority() const
{
    if (viewport_rect_for_tile_priority_.IsEmpty())
        return DeviceViewport();

    return viewport_rect_for_tile_priority_;
}

gfx::Size LayerTreeHostImpl::DrawViewportSize() const
{
    return DeviceViewport().size();
}

gfx::Rect LayerTreeHostImpl::DeviceViewport() const
{
    if (external_viewport_.IsEmpty())
        return gfx::Rect(device_viewport_size_);

    return external_viewport_;
}

gfx::Rect LayerTreeHostImpl::DeviceClip() const
{
    if (external_clip_.IsEmpty())
        return DeviceViewport();

    return external_clip_;
}

const gfx::Transform& LayerTreeHostImpl::DrawTransform() const
{
    return external_transform_;
}

void LayerTreeHostImpl::DidChangeTopControlsPosition()
{
    UpdateViewportContainerSizes();
    SetNeedsRedraw();
    SetNeedsAnimate();
    active_tree_->set_needs_update_draw_properties();
    SetFullRootLayerDamage();
}

float LayerTreeHostImpl::TopControlsHeight() const
{
    return active_tree_->top_controls_height();
}

void LayerTreeHostImpl::SetCurrentTopControlsShownRatio(float ratio)
{
    if (active_tree_->SetCurrentTopControlsShownRatio(ratio))
        DidChangeTopControlsPosition();
}

float LayerTreeHostImpl::CurrentTopControlsShownRatio() const
{
    return active_tree_->CurrentTopControlsShownRatio();
}

void LayerTreeHostImpl::BindToClient(InputHandlerClient* client)
{
    DCHECK(input_handler_client_ == NULL);
    input_handler_client_ = client;
}

LayerImpl* LayerTreeHostImpl::FindScrollLayerForDeviceViewportPoint(
    const gfx::PointF& device_viewport_point,
    InputHandler::ScrollInputType type,
    LayerImpl* layer_impl,
    bool* scroll_on_main_thread,
    bool* optional_has_ancestor_scroll_handler) const
{
    DCHECK(scroll_on_main_thread);

    ScrollBlocksOn block_mode = EffectiveScrollBlocksOn(layer_impl);

    // Walk up the hierarchy and look for a scrollable layer.
    LayerImpl* potentially_scrolling_layer_impl = NULL;
    for (; layer_impl; layer_impl = NextLayerInScrollOrder(layer_impl)) {
        // The content layer can also block attempts to scroll outside the main
        // thread.
        ScrollStatus status = layer_impl->TryScroll(device_viewport_point, type, block_mode);
        if (status == SCROLL_ON_MAIN_THREAD) {
            *scroll_on_main_thread = true;
            return NULL;
        }

        LayerImpl* scroll_layer_impl = FindScrollLayerForContentLayer(layer_impl);
        if (!scroll_layer_impl)
            continue;

        status = scroll_layer_impl->TryScroll(device_viewport_point, type, block_mode);
        // If any layer wants to divert the scroll event to the main thread, abort.
        if (status == SCROLL_ON_MAIN_THREAD) {
            *scroll_on_main_thread = true;
            return NULL;
        }

        if (optional_has_ancestor_scroll_handler && scroll_layer_impl->have_scroll_event_handlers())
            *optional_has_ancestor_scroll_handler = true;

        if (status == SCROLL_STARTED && !potentially_scrolling_layer_impl)
            potentially_scrolling_layer_impl = scroll_layer_impl;
    }

    // Falling back to the root scroll layer ensures generation of root overscroll
    // notifications while preventing scroll updates from being unintentionally
    // forwarded to the main thread. The inner viewport layer represents the
    // viewport during scrolling.
    if (!potentially_scrolling_layer_impl)
        potentially_scrolling_layer_impl = InnerViewportScrollLayer();

    // The inner viewport layer represents the viewport.
    if (potentially_scrolling_layer_impl == OuterViewportScrollLayer())
        potentially_scrolling_layer_impl = InnerViewportScrollLayer();

    // Animated wheel scrolls need to scroll the outer viewport layer, and do not
    // go through Viewport::ScrollBy which would normally handle the distribution.
    // NOTE: This will need refactoring if we want smooth scrolling on Android.
    if (type == ANIMATED_WHEEL && potentially_scrolling_layer_impl == InnerViewportScrollLayer()) {
        potentially_scrolling_layer_impl = OuterViewportScrollLayer();
    }

    return potentially_scrolling_layer_impl;
}

// Similar to LayerImpl::HasAncestor, but walks up the scroll parents.
static bool HasScrollAncestor(LayerImpl* child, LayerImpl* scroll_ancestor)
{
    DCHECK(scroll_ancestor);
    for (LayerImpl* ancestor = child; ancestor;
         ancestor = NextLayerInScrollOrder(ancestor)) {
        if (ancestor->scrollable())
            return ancestor == scroll_ancestor;
    }
    return false;
}

InputHandler::ScrollStatus LayerTreeHostImpl::ScrollBeginImpl(
    LayerImpl* scrolling_layer_impl,
    InputHandler::ScrollInputType type)
{
    if (!scrolling_layer_impl)
        return SCROLL_IGNORED;

    top_controls_manager_->ScrollBegin();

    active_tree_->SetCurrentlyScrollingLayer(scrolling_layer_impl);
    wheel_scrolling_ = (type == WHEEL || type == ANIMATED_WHEEL);
    client_->RenewTreePriority();
    UMA_HISTOGRAM_BOOLEAN("TryScroll.SlowScroll", false);
    return SCROLL_STARTED;
}

InputHandler::ScrollStatus LayerTreeHostImpl::RootScrollBegin(
    InputHandler::ScrollInputType type)
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::RootScrollBegin");

    DCHECK(!CurrentlyScrollingLayer());
    ClearCurrentlyScrollingLayer();

    return ScrollBeginImpl(InnerViewportScrollLayer(), type);
}

InputHandler::ScrollStatus LayerTreeHostImpl::ScrollBegin(
    const gfx::Point& viewport_point,
    InputHandler::ScrollInputType type)
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::ScrollBegin");

    DCHECK(!CurrentlyScrollingLayer());
    ClearCurrentlyScrollingLayer();

    gfx::PointF device_viewport_point = gfx::ScalePoint(gfx::PointF(viewport_point), active_tree_->device_scale_factor());
    LayerImpl* layer_impl = active_tree_->FindLayerThatIsHitByPoint(device_viewport_point);

    if (layer_impl) {
        LayerImpl* scroll_layer_impl = active_tree_->FindFirstScrollingLayerThatIsHitByPoint(
            device_viewport_point);
        if (scroll_layer_impl && !HasScrollAncestor(layer_impl, scroll_layer_impl))
            return SCROLL_UNKNOWN;
    }

    bool scroll_on_main_thread = false;
    LayerImpl* scrolling_layer_impl = FindScrollLayerForDeviceViewportPoint(
        device_viewport_point, type, layer_impl, &scroll_on_main_thread,
        &scroll_affects_scroll_handler_);

    if (scroll_on_main_thread) {
        UMA_HISTOGRAM_BOOLEAN("TryScroll.SlowScroll", true);
        return SCROLL_ON_MAIN_THREAD;
    }

    return ScrollBeginImpl(scrolling_layer_impl, type);
}

InputHandler::ScrollStatus LayerTreeHostImpl::ScrollAnimated(
    const gfx::Point& viewport_point,
    const gfx::Vector2dF& scroll_delta)
{
    if (LayerImpl* layer_impl = CurrentlyScrollingLayer()) {
        return ScrollAnimationUpdateTarget(layer_impl, scroll_delta)
            ? SCROLL_STARTED
            : SCROLL_IGNORED;
    }
    // ScrollAnimated is used for animated wheel scrolls. We find the first layer
    // that can scroll and set up an animation of its scroll offset. Note that
    // this does not currently go through the scroll customization and viewport
    // machinery that ScrollBy uses for non-animated wheel scrolls.
    InputHandler::ScrollStatus scroll_status = ScrollBegin(viewport_point, ANIMATED_WHEEL);
    if (scroll_status == SCROLL_STARTED) {
        gfx::Vector2dF pending_delta = scroll_delta;
        for (LayerImpl* layer_impl = CurrentlyScrollingLayer(); layer_impl;
             layer_impl = NextLayerInScrollOrder(layer_impl)) {
            if (!layer_impl->scrollable())
                continue;

            gfx::ScrollOffset current_offset = layer_impl->CurrentScrollOffset();
            gfx::ScrollOffset target_offset = ScrollOffsetWithDelta(current_offset, pending_delta);
            target_offset.SetToMax(gfx::ScrollOffset());
            target_offset.SetToMin(layer_impl->MaxScrollOffset());
            gfx::Vector2dF actual_delta = target_offset.DeltaFrom(current_offset);

            const float kEpsilon = 0.1f;
            bool can_layer_scroll = (std::abs(actual_delta.x()) > kEpsilon || std::abs(actual_delta.y()) > kEpsilon);

            if (!can_layer_scroll) {
                layer_impl->ScrollBy(actual_delta);
                pending_delta -= actual_delta;
                continue;
            }

            active_tree_->SetCurrentlyScrollingLayer(layer_impl);

            ScrollAnimationCreate(layer_impl, target_offset, current_offset);

            SetNeedsAnimate();
            return SCROLL_STARTED;
        }
    }
    ScrollEnd();
    return scroll_status;
}

gfx::Vector2dF LayerTreeHostImpl::ScrollLayerWithViewportSpaceDelta(
    LayerImpl* layer_impl,
    const gfx::PointF& viewport_point,
    const gfx::Vector2dF& viewport_delta)
{
    // Layers with non-invertible screen space transforms should not have passed
    // the scroll hit test in the first place.
    DCHECK(layer_impl->screen_space_transform().IsInvertible());
    gfx::Transform inverse_screen_space_transform(
        gfx::Transform::kSkipInitialization);
    bool did_invert = layer_impl->screen_space_transform().GetInverse(
        &inverse_screen_space_transform);
    // TODO(shawnsingh): With the advent of impl-side scrolling for non-root
    // layers, we may need to explicitly handle uninvertible transforms here.
    DCHECK(did_invert);

    float scale_from_viewport_to_screen_space = active_tree_->device_scale_factor();
    gfx::PointF screen_space_point = gfx::ScalePoint(viewport_point, scale_from_viewport_to_screen_space);

    gfx::Vector2dF screen_space_delta = viewport_delta;
    screen_space_delta.Scale(scale_from_viewport_to_screen_space);

    // First project the scroll start and end points to local layer space to find
    // the scroll delta in layer coordinates.
    bool start_clipped, end_clipped;
    gfx::PointF screen_space_end_point = screen_space_point + screen_space_delta;
    gfx::PointF local_start_point = MathUtil::ProjectPoint(inverse_screen_space_transform,
        screen_space_point,
        &start_clipped);
    gfx::PointF local_end_point = MathUtil::ProjectPoint(inverse_screen_space_transform,
        screen_space_end_point,
        &end_clipped);

    // In general scroll point coordinates should not get clipped.
    DCHECK(!start_clipped);
    DCHECK(!end_clipped);
    if (start_clipped || end_clipped)
        return gfx::Vector2dF();

    // Apply the scroll delta.
    gfx::ScrollOffset previous_offset = layer_impl->CurrentScrollOffset();
    layer_impl->ScrollBy(local_end_point - local_start_point);
    gfx::ScrollOffset scrolled = layer_impl->CurrentScrollOffset() - previous_offset;

    // Get the end point in the layer's content space so we can apply its
    // ScreenSpaceTransform.
    gfx::PointF actual_local_end_point = local_start_point + gfx::Vector2dF(scrolled.x(), scrolled.y());

    // Calculate the applied scroll delta in viewport space coordinates.
    gfx::PointF actual_screen_space_end_point = MathUtil::MapPoint(layer_impl->screen_space_transform(),
        actual_local_end_point, &end_clipped);
    DCHECK(!end_clipped);
    if (end_clipped)
        return gfx::Vector2dF();
    gfx::PointF actual_viewport_end_point = gfx::ScalePoint(actual_screen_space_end_point,
        1.f / scale_from_viewport_to_screen_space);
    return actual_viewport_end_point - viewport_point;
}

static gfx::Vector2dF ScrollLayerWithLocalDelta(
    LayerImpl* layer_impl,
    const gfx::Vector2dF& local_delta,
    float page_scale_factor)
{
    gfx::ScrollOffset previous_offset = layer_impl->CurrentScrollOffset();
    gfx::Vector2dF delta = local_delta;
    delta.Scale(1.f / page_scale_factor);
    layer_impl->ScrollBy(delta);
    gfx::ScrollOffset scrolled = layer_impl->CurrentScrollOffset() - previous_offset;
    gfx::Vector2dF consumed_scroll(scrolled.x(), scrolled.y());
    consumed_scroll.Scale(page_scale_factor);

    return consumed_scroll;
}

gfx::Vector2dF LayerTreeHostImpl::ScrollLayer(LayerImpl* layer_impl,
    const gfx::Vector2dF& delta,
    const gfx::Point& viewport_point,
    bool is_direct_manipulation)
{
    // Events representing direct manipulation of the screen (such as gesture
    // events) need to be transformed from viewport coordinates to local layer
    // coordinates so that the scrolling contents exactly follow the user's
    // finger. In contrast, events not representing direct manipulation of the
    // screen (such as wheel events) represent a fixed amount of scrolling so we
    // can just apply them directly, but the page scale factor is applied to the
    // scroll delta.
    if (is_direct_manipulation)
        return ScrollLayerWithViewportSpaceDelta(layer_impl, gfx::PointF(viewport_point), delta);
    float scale_factor = active_tree()->current_page_scale_factor();
    return ScrollLayerWithLocalDelta(layer_impl, delta, scale_factor);
}

void LayerTreeHostImpl::ApplyScroll(LayerImpl* layer,
    ScrollState* scroll_state)
{
    DCHECK(scroll_state);
    gfx::Point viewport_point(scroll_state->start_position_x(),
        scroll_state->start_position_y());
    const gfx::Vector2dF delta(scroll_state->delta_x(), scroll_state->delta_y());
    gfx::Vector2dF applied_delta;
    // TODO(tdresser): Use a more rational epsilon. See crbug.com/510550 for
    // details.
    const float kEpsilon = 0.1f;

    if (layer == InnerViewportScrollLayer()) {
        bool affect_top_controls = !wheel_scrolling_;
        Viewport::ScrollResult result = viewport()->ScrollBy(
            delta, viewport_point, scroll_state->is_direct_manipulation(),
            affect_top_controls);
        applied_delta = result.consumed_delta;
        scroll_state->set_caused_scroll(
            std::abs(result.content_scrolled_delta.x()) > kEpsilon,
            std::abs(result.content_scrolled_delta.y()) > kEpsilon);
        scroll_state->ConsumeDelta(applied_delta.x(), applied_delta.y());
    } else {
        applied_delta = ScrollLayer(layer, delta, viewport_point,
            scroll_state->is_direct_manipulation());
    }

    // If the layer wasn't able to move, try the next one in the hierarchy.
    bool scrolled = std::abs(applied_delta.x()) > kEpsilon;
    scrolled = scrolled || std::abs(applied_delta.y()) > kEpsilon;

    if (scrolled && layer != InnerViewportScrollLayer()) {
        // If the applied delta is within 45 degrees of the input
        // delta, bail out to make it easier to scroll just one layer
        // in one direction without affecting any of its parents.
        float angle_threshold = 45;
        if (MathUtil::SmallestAngleBetweenVectors(applied_delta, delta) < angle_threshold) {
            applied_delta = delta;
        } else {
            // Allow further movement only on an axis perpendicular to the direction
            // in which the layer moved.
            applied_delta = MathUtil::ProjectVector(delta, applied_delta);
        }
        scroll_state->set_caused_scroll(std::abs(applied_delta.x()) > kEpsilon,
            std::abs(applied_delta.y()) > kEpsilon);
        scroll_state->ConsumeDelta(applied_delta.x(), applied_delta.y());
    }

    if (!scrolled)
        return;

    scroll_state->set_current_native_scrolling_layer(layer);
}

InputHandlerScrollResult LayerTreeHostImpl::ScrollBy(
    const gfx::Point& viewport_point,
    const gfx::Vector2dF& scroll_delta)
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::ScrollBy");
    if (!CurrentlyScrollingLayer())
        return InputHandlerScrollResult();

    float initial_top_controls_offset = top_controls_manager_->ControlsTopOffset();
    ScrollState scroll_state(
        scroll_delta.x(), scroll_delta.y(), viewport_point.x(),
        viewport_point.y(), false /* should_propagate */,
        did_lock_scrolling_layer_ /* delta_consumed_for_scroll_sequence */,
        !wheel_scrolling_ /* is_direct_manipulation */);
    scroll_state.set_current_native_scrolling_layer(CurrentlyScrollingLayer());

    std::list<LayerImpl*> current_scroll_chain;
    for (LayerImpl* layer_impl = CurrentlyScrollingLayer(); layer_impl;
         layer_impl = NextLayerInScrollOrder(layer_impl)) {
        // Skip the outer viewport scroll layer so that we try to scroll the
        // viewport only once. i.e. The inner viewport layer represents the
        // viewport.
        if (!layer_impl->scrollable() || layer_impl == OuterViewportScrollLayer())
            continue;
        current_scroll_chain.push_front(layer_impl);
    }
    scroll_state.set_scroll_chain(current_scroll_chain);
    scroll_state.DistributeToScrollChainDescendant();

    active_tree_->SetCurrentlyScrollingLayer(
        scroll_state.current_native_scrolling_layer());
    did_lock_scrolling_layer_ = scroll_state.delta_consumed_for_scroll_sequence();

    bool did_scroll_x = scroll_state.caused_scroll_x();
    bool did_scroll_y = scroll_state.caused_scroll_y();
    bool did_scroll_content = did_scroll_x || did_scroll_y;
    if (did_scroll_content) {
        // If we are scrolling with an active scroll handler, forward latency
        // tracking information to the main thread so the delay introduced by the
        // handler is accounted for.
        if (scroll_affects_scroll_handler())
            NotifySwapPromiseMonitorsOfForwardingToMainThread();
        client_->SetNeedsCommitOnImplThread();
        SetNeedsRedraw();
        client_->RenewTreePriority();
    }

    // Scrolling along an axis resets accumulated root overscroll for that axis.
    if (did_scroll_x)
        accumulated_root_overscroll_.set_x(0);
    if (did_scroll_y)
        accumulated_root_overscroll_.set_y(0);
    gfx::Vector2dF unused_root_delta(scroll_state.delta_x(),
        scroll_state.delta_y());

    // When inner viewport is unscrollable, disable overscrolls.
    if (InnerViewportScrollLayer()) {
        if (!InnerViewportScrollLayer()->user_scrollable_horizontal())
            unused_root_delta.set_x(0);
        if (!InnerViewportScrollLayer()->user_scrollable_vertical())
            unused_root_delta.set_y(0);
    }

    accumulated_root_overscroll_ += unused_root_delta;

    bool did_scroll_top_controls = initial_top_controls_offset != top_controls_manager_->ControlsTopOffset();

    InputHandlerScrollResult scroll_result;
    scroll_result.did_scroll = did_scroll_content || did_scroll_top_controls;
    scroll_result.did_overscroll_root = !unused_root_delta.IsZero();
    scroll_result.accumulated_root_overscroll = accumulated_root_overscroll_;
    scroll_result.unused_scroll_delta = unused_root_delta;

    // Scrolling can change the root scroll offset, so inform the synchronous
    // input handler.
    UpdateRootLayerStateForSynchronousInputHandler();

    return scroll_result;
}

// This implements scrolling by page as described here:
// http://msdn.microsoft.com/en-us/library/windows/desktop/ms645601(v=vs.85).aspx#_win32_The_Mouse_Wheel
// for events with WHEEL_PAGESCROLL set.
bool LayerTreeHostImpl::ScrollVerticallyByPage(const gfx::Point& viewport_point,
    ScrollDirection direction)
{
    DCHECK(wheel_scrolling_);

    for (LayerImpl* layer_impl = CurrentlyScrollingLayer(); layer_impl;
         layer_impl = NextLayerInScrollOrder(layer_impl)) {
        // The inner viewport layer represents the viewport.
        if (!layer_impl->scrollable() || layer_impl == OuterViewportScrollLayer())
            continue;

        if (!layer_impl->HasScrollbar(VERTICAL))
            continue;

        float height = layer_impl->clip_height();

        // These magical values match WebKit and are designed to scroll nearly the
        // entire visible content height but leave a bit of overlap.
        float page = std::max(height * 0.875f, 1.f);
        if (direction == SCROLL_BACKWARD)
            page = -page;

        gfx::Vector2dF delta = gfx::Vector2dF(0.f, page);

        gfx::Vector2dF applied_delta = ScrollLayerWithLocalDelta(layer_impl, delta, 1.f);

        if (!applied_delta.IsZero()) {
            client_->SetNeedsCommitOnImplThread();
            SetNeedsRedraw();
            client_->RenewTreePriority();
            return true;
        }

        active_tree_->SetCurrentlyScrollingLayer(layer_impl);
    }

    return false;
}

void LayerTreeHostImpl::RequestUpdateForSynchronousInputHandler()
{
    UpdateRootLayerStateForSynchronousInputHandler();
}

void LayerTreeHostImpl::SetSynchronousInputHandlerRootScrollOffset(
    const gfx::ScrollOffset& root_offset)
{
    active_tree_->DistributeRootScrollOffset(root_offset);
    client_->SetNeedsCommitOnImplThread();
    // After applying the synchronous input handler's scroll offset, tell it what
    // we ended up with.
    UpdateRootLayerStateForSynchronousInputHandler();
    // No need to SetNeedsRedraw, this is for WebView and every frame has redraw
    // requested by the WebView embedder already.
}

void LayerTreeHostImpl::ClearCurrentlyScrollingLayer()
{
    active_tree_->ClearCurrentlyScrollingLayer();
    did_lock_scrolling_layer_ = false;
    scroll_affects_scroll_handler_ = false;
    accumulated_root_overscroll_ = gfx::Vector2dF();
}

void LayerTreeHostImpl::ScrollEnd()
{
    top_controls_manager_->ScrollEnd();
    ClearCurrentlyScrollingLayer();
}

InputHandler::ScrollStatus LayerTreeHostImpl::FlingScrollBegin()
{
    if (!CurrentlyScrollingLayer())
        return SCROLL_IGNORED;
    return SCROLL_STARTED;
}

float LayerTreeHostImpl::DeviceSpaceDistanceToLayer(
    const gfx::PointF& device_viewport_point,
    LayerImpl* layer_impl)
{
    if (!layer_impl)
        return std::numeric_limits<float>::max();

    gfx::Rect layer_impl_bounds(layer_impl->bounds());

    gfx::RectF device_viewport_layer_impl_bounds = MathUtil::MapClippedRect(
        layer_impl->screen_space_transform(), gfx::RectF(layer_impl_bounds));

    return device_viewport_layer_impl_bounds.ManhattanDistanceToPoint(
        device_viewport_point);
}

void LayerTreeHostImpl::MouseMoveAt(const gfx::Point& viewport_point)
{
    gfx::PointF device_viewport_point = gfx::ScalePoint(gfx::PointF(viewport_point), active_tree_->device_scale_factor());
    LayerImpl* layer_impl = active_tree_->FindLayerThatIsHitByPoint(device_viewport_point);
    if (HandleMouseOverScrollbar(layer_impl, device_viewport_point))
        return;

    if (scroll_layer_id_when_mouse_over_scrollbar_) {
        LayerImpl* scroll_layer_impl = active_tree_->LayerById(
            scroll_layer_id_when_mouse_over_scrollbar_);

        // The check for a null scroll_layer_impl below was added to see if it will
        // eliminate the crashes described in http://crbug.com/326635.
        // TODO(wjmaclean) Add a unit test if this fixes the crashes.
        ScrollbarAnimationController* animation_controller = scroll_layer_impl ? scroll_layer_impl->scrollbar_animation_controller()
                                                                               : NULL;
        if (animation_controller)
            animation_controller->DidMouseMoveOffScrollbar();
        scroll_layer_id_when_mouse_over_scrollbar_ = 0;
    }

    bool scroll_on_main_thread = false;
    LayerImpl* scroll_layer_impl = FindScrollLayerForDeviceViewportPoint(
        device_viewport_point, InputHandler::GESTURE, layer_impl,
        &scroll_on_main_thread, NULL);
    if (scroll_on_main_thread || !scroll_layer_impl)
        return;

    ScrollbarAnimationController* animation_controller = scroll_layer_impl->scrollbar_animation_controller();
    if (!animation_controller)
        return;

    // TODO(wjmaclean) Is it ok to choose distance from more than two scrollbars?
    float distance_to_scrollbar = std::numeric_limits<float>::max();
    for (LayerImpl::ScrollbarSet::iterator it = scroll_layer_impl->scrollbars()->begin();
         it != scroll_layer_impl->scrollbars()->end();
         ++it)
        distance_to_scrollbar = std::min(distance_to_scrollbar,
            DeviceSpaceDistanceToLayer(device_viewport_point, *it));

    animation_controller->DidMouseMoveNear(distance_to_scrollbar / active_tree_->device_scale_factor());
}

bool LayerTreeHostImpl::HandleMouseOverScrollbar(LayerImpl* layer_impl,
    const gfx::PointF& device_viewport_point)
{
    if (layer_impl && layer_impl->ToScrollbarLayer()) {
        int scroll_layer_id = layer_impl->ToScrollbarLayer()->ScrollLayerId();
        layer_impl = active_tree_->LayerById(scroll_layer_id);
        if (layer_impl && layer_impl->scrollbar_animation_controller()) {
            scroll_layer_id_when_mouse_over_scrollbar_ = scroll_layer_id;
            layer_impl->scrollbar_animation_controller()->DidMouseMoveNear(0);
        } else {
            scroll_layer_id_when_mouse_over_scrollbar_ = 0;
        }

        return true;
    }

    return false;
}

void LayerTreeHostImpl::PinchGestureBegin()
{
    pinch_gesture_active_ = true;
    client_->RenewTreePriority();
    pinch_gesture_end_should_clear_scrolling_layer_ = !CurrentlyScrollingLayer();
    active_tree_->SetCurrentlyScrollingLayer(
        active_tree_->InnerViewportScrollLayer());
    top_controls_manager_->PinchBegin();
}

void LayerTreeHostImpl::PinchGestureUpdate(float magnify_delta,
    const gfx::Point& anchor)
{
    TRACE_EVENT0("cc", "LayerTreeHostImpl::PinchGestureUpdate");
    if (!InnerViewportScrollLayer())
        return;
    viewport()->PinchUpdate(magnify_delta, anchor);
    client_->SetNeedsCommitOnImplThread();
    SetNeedsRedraw();
    client_->RenewTreePriority();
    // Pinching can change the root scroll offset, so inform the synchronous input
    // handler.
    UpdateRootLayerStateForSynchronousInputHandler();
}

void LayerTreeHostImpl::PinchGestureEnd()
{
    pinch_gesture_active_ = false;
    if (pinch_gesture_end_should_clear_scrolling_layer_) {
        pinch_gesture_end_should_clear_scrolling_layer_ = false;
        ClearCurrentlyScrollingLayer();
    }
    viewport()->PinchEnd();
    top_controls_manager_->PinchEnd();
    client_->SetNeedsCommitOnImplThread();
    // When a pinch ends, we may be displaying content cached at incorrect scales,
    // so updating draw properties and drawing will ensure we are using the right
    // scales that we want when we're not inside a pinch.
    active_tree_->set_needs_update_draw_properties();
    SetNeedsRedraw();
}

static void CollectScrollDeltas(ScrollAndScaleSet* scroll_info,
    LayerImpl* layer_impl)
{
    if (!layer_impl)
        return;

    gfx::ScrollOffset scroll_delta = layer_impl->PullDeltaForMainThread();

    if (!scroll_delta.IsZero()) {
        LayerTreeHostCommon::ScrollUpdateInfo scroll;
        scroll.layer_id = layer_impl->id();
        scroll.scroll_delta = gfx::Vector2d(scroll_delta.x(), scroll_delta.y());
        scroll_info->scrolls.push_back(scroll);
    }

    for (size_t i = 0; i < layer_impl->children().size(); ++i)
        CollectScrollDeltas(scroll_info, layer_impl->children()[i]);
}

scoped_ptr<ScrollAndScaleSet> LayerTreeHostImpl::ProcessScrollDeltas()
{
    scoped_ptr<ScrollAndScaleSet> scroll_info(new ScrollAndScaleSet());

    CollectScrollDeltas(scroll_info.get(), active_tree_->root_layer());
    scroll_info->page_scale_delta = active_tree_->page_scale_factor()->PullDeltaForMainThread();
    scroll_info->top_controls_delta = active_tree()->top_controls_shown_ratio()->PullDeltaForMainThread();
    scroll_info->elastic_overscroll_delta = active_tree_->elastic_overscroll()->PullDeltaForMainThread();
    scroll_info->swap_promises.swap(swap_promises_for_main_thread_scroll_update_);

    return scroll_info.Pass();
}

void LayerTreeHostImpl::SetFullRootLayerDamage()
{
    SetViewportDamage(gfx::Rect(DrawViewportSize()));
}

void LayerTreeHostImpl::ScrollViewportInnerFirst(gfx::Vector2dF scroll_delta)
{
    DCHECK(InnerViewportScrollLayer());
    LayerImpl* scroll_layer = InnerViewportScrollLayer();

    gfx::Vector2dF unused_delta = scroll_layer->ScrollBy(scroll_delta);
    if (!unused_delta.IsZero() && OuterViewportScrollLayer())
        OuterViewportScrollLayer()->ScrollBy(unused_delta);
}

void LayerTreeHostImpl::ScrollViewportBy(gfx::Vector2dF scroll_delta)
{
    DCHECK(InnerViewportScrollLayer());
    LayerImpl* scroll_layer = OuterViewportScrollLayer()
        ? OuterViewportScrollLayer()
        : InnerViewportScrollLayer();

    gfx::Vector2dF unused_delta = scroll_layer->ScrollBy(scroll_delta);

    if (!unused_delta.IsZero() && (scroll_layer == OuterViewportScrollLayer()))
        InnerViewportScrollLayer()->ScrollBy(unused_delta);
}

void LayerTreeHostImpl::AnimatePageScale(base::TimeTicks monotonic_time)
{
    if (!page_scale_animation_)
        return;

    gfx::ScrollOffset scroll_total = active_tree_->TotalScrollOffset();

    if (!page_scale_animation_->IsAnimationStarted())
        page_scale_animation_->StartAnimation(monotonic_time);

    active_tree_->SetPageScaleOnActiveTree(
        page_scale_animation_->PageScaleFactorAtTime(monotonic_time));
    gfx::ScrollOffset next_scroll = gfx::ScrollOffset(
        page_scale_animation_->ScrollOffsetAtTime(monotonic_time));

    ScrollViewportInnerFirst(next_scroll.DeltaFrom(scroll_total));
    SetNeedsRedraw();

    if (page_scale_animation_->IsAnimationCompleteAtTime(monotonic_time)) {
        page_scale_animation_ = nullptr;
        client_->SetNeedsCommitOnImplThread();
        client_->RenewTreePriority();
        client_->DidCompletePageScaleAnimationOnImplThread();
    } else {
        SetNeedsAnimate();
    }
}

void LayerTreeHostImpl::AnimateTopControls(base::TimeTicks time)
{
    if (!top_controls_manager_->animation())
        return;

    gfx::Vector2dF scroll = top_controls_manager_->Animate(time);

    if (top_controls_manager_->animation())
        SetNeedsAnimate();

    if (active_tree_->TotalScrollOffset().y() == 0.f)
        return;

    if (scroll.IsZero())
        return;

    ScrollViewportBy(gfx::ScaleVector2d(
        scroll, 1.f / active_tree_->current_page_scale_factor()));
    SetNeedsRedraw();
    client_->SetNeedsCommitOnImplThread();
    client_->RenewTreePriority();
}

void LayerTreeHostImpl::AnimateScrollbars(base::TimeTicks monotonic_time)
{
    if (scrollbar_animation_controllers_.empty())
        return;

    TRACE_EVENT0("cc", "LayerTreeHostImpl::AnimateScrollbars");
    std::set<ScrollbarAnimationController*> controllers_copy = scrollbar_animation_controllers_;
    for (auto& it : controllers_copy)
        it->Animate(monotonic_time);

    SetNeedsAnimate();
}

void LayerTreeHostImpl::AnimateLayers(base::TimeTicks monotonic_time)
{
    if (!settings_.accelerated_animation_enabled)
        return;

    bool animated = false;
    if (animation_host_) {
        if (animation_host_->AnimateLayers(monotonic_time))
            animated = true;
    } else {
        if (animation_registrar_->AnimateLayers(monotonic_time))
            animated = true;
    }

    // TODO(ajuma): Only do this if the animations are on the active tree, or if
    // they are on the pending tree waiting for some future time to start.
    if (animated)
        SetNeedsAnimate();
}

void LayerTreeHostImpl::UpdateAnimationState(bool start_ready_animations)
{
    if (!settings_.accelerated_animation_enabled)
        return;

    bool has_active_animations = false;
    scoped_ptr<AnimationEventsVector> events;

    if (animation_host_) {
        events = animation_host_->CreateEvents();
        has_active_animations = animation_host_->UpdateAnimationState(
            start_ready_animations, events.get());
    } else {
        events = animation_registrar_->CreateEvents();
        has_active_animations = animation_registrar_->UpdateAnimationState(
            start_ready_animations, events.get());
    }

    if (!events->empty())
        client_->PostAnimationEventsToMainThreadOnImplThread(events.Pass());

    if (has_active_animations)
        SetNeedsAnimate();
}

void LayerTreeHostImpl::ActivateAnimations()
{
    if (!settings_.accelerated_animation_enabled)
        return;

    bool activated = false;
    if (animation_host_) {
        if (animation_host_->ActivateAnimations())
            activated = true;
    } else {
        if (animation_registrar_->ActivateAnimations())
            activated = true;
    }

    if (activated) {
        SetNeedsAnimate();
        // Activating an animation changes layer draw properties, such as
        // screen_space_transform_is_animating, or changes transforms etc. So when
        // we see a new animation get activated, we need to update the draw
        // properties on the active tree.
        active_tree()->set_needs_update_draw_properties();
    }
}

std::string LayerTreeHostImpl::LayerTreeAsJson() const
{
    std::string str;
    if (active_tree_->root_layer()) {
        scoped_ptr<base::Value> json(active_tree_->root_layer()->LayerTreeAsJson());
        base::JSONWriter::WriteWithOptions(
            *json, base::JSONWriter::OPTIONS_PRETTY_PRINT, &str);
    }
    return str;
}

void LayerTreeHostImpl::StartAnimatingScrollbarAnimationController(
    ScrollbarAnimationController* controller)
{
    scrollbar_animation_controllers_.insert(controller);
    SetNeedsAnimate();
}

void LayerTreeHostImpl::StopAnimatingScrollbarAnimationController(
    ScrollbarAnimationController* controller)
{
    scrollbar_animation_controllers_.erase(controller);
}

void LayerTreeHostImpl::PostDelayedScrollbarAnimationTask(
    const base::Closure& task,
    base::TimeDelta delay)
{
    client_->PostDelayedAnimationTaskOnImplThread(task, delay);
}

void LayerTreeHostImpl::SetNeedsRedrawForScrollbarAnimation()
{
    SetNeedsRedraw();
}

void LayerTreeHostImpl::AddVideoFrameController(
    VideoFrameController* controller)
{
    bool was_empty = video_frame_controllers_.empty();
    video_frame_controllers_.insert(controller);
    if (current_begin_frame_tracker_.DangerousMethodHasStarted() && !current_begin_frame_tracker_.DangerousMethodHasFinished())
        controller->OnBeginFrame(current_begin_frame_tracker_.Current());
    if (was_empty)
        client_->SetVideoNeedsBeginFrames(true);
}

void LayerTreeHostImpl::RemoveVideoFrameController(
    VideoFrameController* controller)
{
    video_frame_controllers_.erase(controller);
    if (video_frame_controllers_.empty())
        client_->SetVideoNeedsBeginFrames(false);
}

void LayerTreeHostImpl::SetTreePriority(TreePriority priority)
{
    if (!tile_manager_)
        return;

    if (global_tile_state_.tree_priority == priority)
        return;
    global_tile_state_.tree_priority = priority;
    DidModifyTilePriorities();
}

TreePriority LayerTreeHostImpl::GetTreePriority() const
{
    return global_tile_state_.tree_priority;
}

BeginFrameArgs LayerTreeHostImpl::CurrentBeginFrameArgs() const
{
    // TODO(mithro): Replace call with current_begin_frame_tracker_.Current()
    // once all calls which happens outside impl frames are fixed.
    return current_begin_frame_tracker_.DangerousMethodCurrentOrLast();
}

base::TimeDelta LayerTreeHostImpl::CurrentBeginFrameInterval() const
{
    return current_begin_frame_tracker_.Interval();
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
LayerTreeHostImpl::AsValueWithFrame(FrameData* frame) const
{
    scoped_refptr<base::trace_event::TracedValue> state = new base::trace_event::TracedValue();
    AsValueWithFrameInto(frame, state.get());
    return state;
}

void LayerTreeHostImpl::AsValueWithFrameInto(
    FrameData* frame,
    base::trace_event::TracedValue* state) const
{
    if (this->pending_tree_) {
        state->BeginDictionary("activation_state");
        ActivationStateAsValueInto(state);
        state->EndDictionary();
    }
    MathUtil::AddToTracedValue("device_viewport_size", device_viewport_size_,
        state);

    std::vector<PrioritizedTile> prioritized_tiles;
    active_tree_->GetAllPrioritizedTilesForTracing(&prioritized_tiles);
    if (pending_tree_)
        pending_tree_->GetAllPrioritizedTilesForTracing(&prioritized_tiles);

    state->BeginArray("active_tiles");
    for (const auto& prioritized_tile : prioritized_tiles) {
        state->BeginDictionary();
        prioritized_tile.AsValueInto(state);
        state->EndDictionary();
    }
    state->EndArray();

    if (tile_manager_) {
        state->BeginDictionary("tile_manager_basic_state");
        tile_manager_->BasicStateAsValueInto(state);
        state->EndDictionary();
    }
    state->BeginDictionary("active_tree");
    active_tree_->AsValueInto(state);
    state->EndDictionary();
    if (pending_tree_) {
        state->BeginDictionary("pending_tree");
        pending_tree_->AsValueInto(state);
        state->EndDictionary();
    }
    if (frame) {
        state->BeginDictionary("frame");
        frame->AsValueInto(state);
        state->EndDictionary();
    }
}

void LayerTreeHostImpl::ActivationStateAsValueInto(
    base::trace_event::TracedValue* state) const
{
    TracedValue::SetIDRef(this, state, "lthi");
    if (tile_manager_) {
        state->BeginDictionary("tile_manager");
        tile_manager_->BasicStateAsValueInto(state);
        state->EndDictionary();
    }
}

void LayerTreeHostImpl::SetDebugState(
    const LayerTreeDebugState& new_debug_state)
{
    if (LayerTreeDebugState::Equal(debug_state_, new_debug_state))
        return;

    debug_state_ = new_debug_state;
    UpdateTileManagerMemoryPolicy(ActualManagedMemoryPolicy());
    SetFullRootLayerDamage();
}

void LayerTreeHostImpl::CreateUIResource(UIResourceId uid,
    const UIResourceBitmap& bitmap)
{
    DCHECK_GT(uid, 0);

    GLint wrap_mode = 0;
    switch (bitmap.GetWrapMode()) {
    case UIResourceBitmap::CLAMP_TO_EDGE:
        wrap_mode = GL_CLAMP_TO_EDGE;
        break;
    case UIResourceBitmap::REPEAT:
        wrap_mode = GL_REPEAT;
        break;
    }

    // Allow for multiple creation requests with the same UIResourceId.  The
    // previous resource is simply deleted.
    ResourceId id = ResourceIdForUIResource(uid);
    if (id)
        DeleteUIResource(uid);

    ResourceFormat format = resource_provider_->best_texture_format();
    switch (bitmap.GetFormat()) {
    case UIResourceBitmap::RGBA8:
        break;
    case UIResourceBitmap::ALPHA_8:
        format = ALPHA_8;
        break;
    case UIResourceBitmap::ETC1:
        format = ETC1;
        break;
    }
    id = resource_provider_->CreateResource(
        bitmap.GetSize(), wrap_mode, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        format);

    UIResourceData data;
    data.resource_id = id;
    data.size = bitmap.GetSize();
    data.opaque = bitmap.GetOpaque();

    ui_resource_map_[uid] = data;

    AutoLockUIResourceBitmap bitmap_lock(bitmap);
    resource_provider_->CopyToResource(id, bitmap_lock.GetPixels(),
        bitmap.GetSize());
    MarkUIResourceNotEvicted(uid);
}

void LayerTreeHostImpl::DeleteUIResource(UIResourceId uid)
{
    ResourceId id = ResourceIdForUIResource(uid);
    if (id) {
        resource_provider_->DeleteResource(id);
        ui_resource_map_.erase(uid);
    }
    MarkUIResourceNotEvicted(uid);
}

void LayerTreeHostImpl::EvictAllUIResources()
{
    if (ui_resource_map_.empty())
        return;

    for (UIResourceMap::const_iterator iter = ui_resource_map_.begin();
         iter != ui_resource_map_.end();
         ++iter) {
        evicted_ui_resources_.insert(iter->first);
        resource_provider_->DeleteResource(iter->second.resource_id);
    }
    ui_resource_map_.clear();

    client_->SetNeedsCommitOnImplThread();
    client_->OnCanDrawStateChanged(CanDraw());
    client_->RenewTreePriority();
}

ResourceId LayerTreeHostImpl::ResourceIdForUIResource(UIResourceId uid) const
{
    UIResourceMap::const_iterator iter = ui_resource_map_.find(uid);
    if (iter != ui_resource_map_.end())
        return iter->second.resource_id;
    return 0;
}

bool LayerTreeHostImpl::IsUIResourceOpaque(UIResourceId uid) const
{
    UIResourceMap::const_iterator iter = ui_resource_map_.find(uid);
    DCHECK(iter != ui_resource_map_.end());
    return iter->second.opaque;
}

bool LayerTreeHostImpl::EvictedUIResourcesExist() const
{
    return !evicted_ui_resources_.empty();
}

void LayerTreeHostImpl::MarkUIResourceNotEvicted(UIResourceId uid)
{
    std::set<UIResourceId>::iterator found_in_evicted = evicted_ui_resources_.find(uid);
    if (found_in_evicted == evicted_ui_resources_.end())
        return;
    evicted_ui_resources_.erase(found_in_evicted);
    if (evicted_ui_resources_.empty())
        client_->OnCanDrawStateChanged(CanDraw());
}

void LayerTreeHostImpl::ScheduleMicroBenchmark(
    scoped_ptr<MicroBenchmarkImpl> benchmark)
{
    //micro_benchmark_controller_.ScheduleRun(benchmark.Pass());
    DebugBreak();
}

void LayerTreeHostImpl::InsertSwapPromiseMonitor(SwapPromiseMonitor* monitor)
{
    swap_promise_monitor_.insert(monitor);
}

void LayerTreeHostImpl::RemoveSwapPromiseMonitor(SwapPromiseMonitor* monitor)
{
    swap_promise_monitor_.erase(monitor);
}

void LayerTreeHostImpl::NotifySwapPromiseMonitorsOfSetNeedsRedraw()
{
    std::set<SwapPromiseMonitor*>::iterator it = swap_promise_monitor_.begin();
    for (; it != swap_promise_monitor_.end(); it++)
        (*it)->OnSetNeedsRedrawOnImpl();
}

void LayerTreeHostImpl::NotifySwapPromiseMonitorsOfForwardingToMainThread()
{
    std::set<SwapPromiseMonitor*>::iterator it = swap_promise_monitor_.begin();
    for (; it != swap_promise_monitor_.end(); it++)
        (*it)->OnForwardScrollUpdateToMainThreadOnImpl();
}

void LayerTreeHostImpl::UpdateRootLayerStateForSynchronousInputHandler()
{
    if (!input_handler_client_)
        return;
    input_handler_client_->UpdateRootLayerStateForSynchronousInputHandler(
        active_tree_->TotalScrollOffset(), active_tree_->TotalMaxScrollOffset(),
        active_tree_->ScrollableSize(), active_tree_->current_page_scale_factor(),
        active_tree_->min_page_scale_factor(),
        active_tree_->max_page_scale_factor());
}

void LayerTreeHostImpl::ScrollAnimationCreate(
    LayerImpl* layer_impl,
    const gfx::ScrollOffset& target_offset,
    const gfx::ScrollOffset& current_offset)
{
    if (animation_host_)
        return animation_host_->ImplOnlyScrollAnimationCreate(
            layer_impl->id(), target_offset, current_offset);

    scoped_ptr<ScrollOffsetAnimationCurve> curve = ScrollOffsetAnimationCurve::Create(target_offset,
        EaseInOutTimingFunction::Create());
    curve->SetInitialValue(current_offset);

    scoped_ptr<Animation> animation = Animation::Create(
        curve.Pass(), AnimationIdProvider::NextAnimationId(),
        AnimationIdProvider::NextGroupId(), Animation::SCROLL_OFFSET);
    animation->set_is_impl_only(true);

    layer_impl->layer_animation_controller()->AddAnimation(animation.Pass());
}

bool LayerTreeHostImpl::ScrollAnimationUpdateTarget(
    LayerImpl* layer_impl,
    const gfx::Vector2dF& scroll_delta)
{
    if (animation_host_)
        return animation_host_->ImplOnlyScrollAnimationUpdateTarget(
            layer_impl->id(), scroll_delta, layer_impl->MaxScrollOffset(),
            CurrentBeginFrameArgs().frame_time);

    Animation* animation = layer_impl->layer_animation_controller()
        ? layer_impl->layer_animation_controller()->GetAnimation(
            Animation::SCROLL_OFFSET)
        : nullptr;
    if (!animation)
        return false;

    ScrollOffsetAnimationCurve* curve = animation->curve()->ToScrollOffsetAnimationCurve();

    gfx::ScrollOffset new_target = gfx::ScrollOffsetWithDelta(curve->target_value(), scroll_delta);
    new_target.SetToMax(gfx::ScrollOffset());
    new_target.SetToMin(layer_impl->MaxScrollOffset());

    curve->UpdateTarget(
        animation->TrimTimeToCurrentIteration(CurrentBeginFrameArgs().frame_time)
            .InSecondsF(),
        new_target);

    return true;
}

bool LayerTreeHostImpl::IsLayerInTree(int layer_id,
    LayerTreeType tree_type) const
{
    if (tree_type == LayerTreeType::ACTIVE) {
        return active_tree() ? active_tree()->LayerById(layer_id) != nullptr
                             : false;
    } else {
        if (pending_tree() && pending_tree()->LayerById(layer_id))
            return true;
        if (recycle_tree() && recycle_tree()->LayerById(layer_id))
            return true;

        return false;
    }
}

void LayerTreeHostImpl::SetMutatorsNeedCommit()
{
    SetNeedsCommit();
}

void LayerTreeHostImpl::SetTreeLayerFilterMutated(
    int layer_id,
    LayerTreeImpl* tree,
    const FilterOperations& filters)
{
    if (!tree)
        return;

    LayerAnimationValueObserver* layer = tree->LayerById(layer_id);
    if (layer)
        layer->OnFilterAnimated(filters);
}

void LayerTreeHostImpl::SetTreeLayerOpacityMutated(int layer_id,
    LayerTreeImpl* tree,
    float opacity)
{
    if (!tree)
        return;

    LayerAnimationValueObserver* layer = tree->LayerById(layer_id);
    if (layer)
        layer->OnOpacityAnimated(opacity);
}

void LayerTreeHostImpl::SetTreeLayerTransformMutated(
    int layer_id,
    LayerTreeImpl* tree,
    const gfx::Transform& transform)
{
    if (!tree)
        return;

    LayerAnimationValueObserver* layer = tree->LayerById(layer_id);
    if (layer)
        layer->OnTransformAnimated(transform);
}

void LayerTreeHostImpl::SetTreeLayerScrollOffsetMutated(
    int layer_id,
    LayerTreeImpl* tree,
    const gfx::ScrollOffset& scroll_offset)
{
    if (!tree)
        return;

    LayerAnimationValueObserver* layer = tree->LayerById(layer_id);
    if (layer)
        layer->OnScrollOffsetAnimated(scroll_offset);
}

void LayerTreeHostImpl::TreeLayerTransformIsPotentiallyAnimatingChanged(
    int layer_id,
    LayerTreeImpl* tree,
    bool is_animating)
{
    if (!tree)
        return;

    LayerAnimationValueObserver* layer = tree->LayerById(layer_id);
    if (layer)
        layer->OnTransformIsPotentiallyAnimatingChanged(is_animating);
}

void LayerTreeHostImpl::SetLayerFilterMutated(int layer_id,
    LayerTreeType tree_type,
    const FilterOperations& filters)
{
    if (tree_type == LayerTreeType::ACTIVE) {
        SetTreeLayerFilterMutated(layer_id, active_tree(), filters);
    } else {
        SetTreeLayerFilterMutated(layer_id, pending_tree(), filters);
        SetTreeLayerFilterMutated(layer_id, recycle_tree(), filters);
    }
}

void LayerTreeHostImpl::SetLayerOpacityMutated(int layer_id,
    LayerTreeType tree_type,
    float opacity)
{
    if (tree_type == LayerTreeType::ACTIVE) {
        SetTreeLayerOpacityMutated(layer_id, active_tree(), opacity);
    } else {
        SetTreeLayerOpacityMutated(layer_id, pending_tree(), opacity);
        SetTreeLayerOpacityMutated(layer_id, recycle_tree(), opacity);
    }
}

void LayerTreeHostImpl::SetLayerTransformMutated(
    int layer_id,
    LayerTreeType tree_type,
    const gfx::Transform& transform)
{
    if (tree_type == LayerTreeType::ACTIVE) {
        SetTreeLayerTransformMutated(layer_id, active_tree(), transform);
    } else {
        SetTreeLayerTransformMutated(layer_id, pending_tree(), transform);
        SetTreeLayerTransformMutated(layer_id, recycle_tree(), transform);
    }
}

void LayerTreeHostImpl::SetLayerScrollOffsetMutated(
    int layer_id,
    LayerTreeType tree_type,
    const gfx::ScrollOffset& scroll_offset)
{
    if (tree_type == LayerTreeType::ACTIVE) {
        SetTreeLayerScrollOffsetMutated(layer_id, active_tree(), scroll_offset);
    } else {
        SetTreeLayerScrollOffsetMutated(layer_id, pending_tree(), scroll_offset);
        SetTreeLayerScrollOffsetMutated(layer_id, recycle_tree(), scroll_offset);
    }
}

void LayerTreeHostImpl::LayerTransformIsPotentiallyAnimatingChanged(
    int layer_id,
    LayerTreeType tree_type,
    bool is_animating)
{
    if (tree_type == LayerTreeType::ACTIVE) {
        TreeLayerTransformIsPotentiallyAnimatingChanged(layer_id, active_tree(),
            is_animating);
    } else {
        TreeLayerTransformIsPotentiallyAnimatingChanged(layer_id, pending_tree(),
            is_animating);
    }
}

void LayerTreeHostImpl::ScrollOffsetAnimationFinished()
{
    ScrollEnd();
}

gfx::ScrollOffset LayerTreeHostImpl::GetScrollOffsetForAnimation(
    int layer_id) const
{
    if (active_tree()) {
        LayerAnimationValueProvider* layer = active_tree()->LayerById(layer_id);
        if (layer)
            return layer->ScrollOffsetForAnimation();
    }

    return gfx::ScrollOffset();
}

} // namespace cc
