// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_LAYER_TREE_HOST_IMPL_H_
#define CC_TREES_LAYER_TREE_HOST_IMPL_H_

#include <set>
#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/containers/hash_tables.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/animation/animation_events.h"
#include "cc/animation/animation_registrar.h"
#include "cc/animation/scrollbar_animation_controller.h"
#include "cc/base/cc_export.h"
#include "cc/base/synced_property.h"
#include "cc/debug/frame_timing_tracker.h"
#include "cc/debug/micro_benchmark_controller_impl.h"
#include "cc/input/input_handler.h"
#include "cc/input/top_controls_manager_client.h"
#include "cc/layers/layer_lists.h"
#include "cc/layers/render_pass_sink.h"
#include "cc/output/begin_frame_args.h"
#include "cc/output/managed_memory_policy.h"
#include "cc/output/output_surface_client.h"
#include "cc/output/renderer.h"
#include "cc/quads/render_pass.h"
#include "cc/resources/resource_provider.h"
#include "cc/resources/ui_resource_client.h"
#include "cc/scheduler/begin_frame_tracker.h"
#include "cc/scheduler/commit_earlyout_reason.h"
#include "cc/scheduler/draw_result.h"
#include "cc/scheduler/video_frame_controller.h"
#include "cc/tiles/tile_manager.h"
#include "cc/trees/layer_tree_settings.h"
#include "cc/trees/mutator_host_client.h"
#include "cc/trees/proxy.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/rect.h"

namespace gfx {
class ScrollOffset;
}

namespace cc {

class AnimationHost;
class CompletionEvent;
class CompositorFrameMetadata;
class DebugRectHistory;
class EvictionTilePriorityQueue;
class FrameRateCounter;
class LayerImpl;
class LayerTreeImpl;
class MemoryHistory;
class PageScaleAnimation;
class PictureLayerImpl;
class RasterTilePriorityQueue;
class TileTaskWorkerPool;
class RenderPassDrawQuad;
class RenderingStatsInstrumentation;
class ResourcePool;
class ScrollElasticityHelper;
class ScrollbarLayerImplBase;
class SwapPromise;
class SwapPromiseMonitor;
class TextureMailboxDeleter;
class TopControlsManager;
class UIResourceBitmap;
class UIResourceRequest;
struct ScrollAndScaleSet;
class Viewport;

enum class GpuRasterizationStatus {
    ON,
    ON_FORCED,
    OFF_DEVICE,
    OFF_VIEWPORT,
    MSAA_CONTENT,
    OFF_CONTENT
};

// LayerTreeHost->Proxy callback interface.
class LayerTreeHostImplClient {
public:
    virtual void UpdateRendererCapabilitiesOnImplThread() = 0;
    virtual void DidLoseOutputSurfaceOnImplThread() = 0;
    virtual void CommitVSyncParameters(base::TimeTicks timebase,
        base::TimeDelta interval)
        = 0;
    virtual void SetEstimatedParentDrawTime(base::TimeDelta draw_time) = 0;
    virtual void SetMaxSwapsPendingOnImplThread(int max) = 0;
    virtual void DidSwapBuffersOnImplThread() = 0;
    virtual void DidSwapBuffersCompleteOnImplThread() = 0;
    virtual void OnCanDrawStateChanged(bool can_draw) = 0;
    virtual void NotifyReadyToActivate() = 0;
    virtual void NotifyReadyToDraw() = 0;
    // Please call these 3 functions through
    // LayerTreeHostImpl's SetNeedsRedraw(), SetNeedsRedrawRect() and
    // SetNeedsAnimate().
    virtual void SetNeedsRedrawOnImplThread() = 0;
    virtual void SetNeedsRedrawRectOnImplThread(const gfx::Rect& damage_rect) = 0;
    virtual void SetNeedsAnimateOnImplThread() = 0;
    virtual void SetNeedsCommitOnImplThread() = 0;
    virtual void SetNeedsPrepareTilesOnImplThread() = 0;
    virtual void SetVideoNeedsBeginFrames(bool needs_begin_frames) = 0;
    virtual void PostAnimationEventsToMainThreadOnImplThread(
        scoped_ptr<AnimationEventsVector> events)
        = 0;
    virtual bool IsInsideDraw() = 0;
    virtual void RenewTreePriority() = 0;
    virtual void PostDelayedAnimationTaskOnImplThread(const base::Closure& task,
        base::TimeDelta delay)
        = 0;
    virtual void DidActivateSyncTree() = 0;
    virtual void WillPrepareTiles() = 0;
    virtual void DidPrepareTiles() = 0;

    // Called when page scale animation has completed on the impl thread.
    virtual void DidCompletePageScaleAnimationOnImplThread() = 0;

    // Called when output surface asks for a draw.
    virtual void OnDrawForOutputSurface() = 0;

    virtual void PostFrameTimingEventsOnImplThread(
        scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
        scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events)
        = 0;

protected:
    virtual ~LayerTreeHostImplClient() { }
};

// LayerTreeHostImpl owns the LayerImpl trees as well as associated rendering
// state.
class CC_EXPORT LayerTreeHostImpl
    : public InputHandler,
      public RendererClient,
      public TileManagerClient,
      public OutputSurfaceClient,
      public TopControlsManagerClient,
      public ScrollbarAnimationControllerClient,
      public VideoFrameControllerClient,
      public MutatorHostClient,
      public base::SupportsWeakPtr<LayerTreeHostImpl> {
public:
    static scoped_ptr<LayerTreeHostImpl> Create(
        const LayerTreeSettings& settings,
        LayerTreeHostImplClient* client,
        Proxy* proxy,
        RenderingStatsInstrumentation* rendering_stats_instrumentation,
        SharedBitmapManager* shared_bitmap_manager,
        gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
        TaskGraphRunner* task_graph_runner,
        int id);
    ~LayerTreeHostImpl() override;

    // InputHandler implementation
    void BindToClient(InputHandlerClient* client) override;
    InputHandler::ScrollStatus ScrollBegin(
        const gfx::Point& viewport_point,
        InputHandler::ScrollInputType type) override;
    InputHandler::ScrollStatus RootScrollBegin(
        InputHandler::ScrollInputType type) override;
    InputHandler::ScrollStatus ScrollAnimated(
        const gfx::Point& viewport_point,
        const gfx::Vector2dF& scroll_delta) override;
    void ApplyScroll(LayerImpl* layer, ScrollState* scroll_state);
    InputHandlerScrollResult ScrollBy(
        const gfx::Point& viewport_point,
        const gfx::Vector2dF& scroll_delta) override;
    bool ScrollVerticallyByPage(const gfx::Point& viewport_point,
        ScrollDirection direction) override;
    void RequestUpdateForSynchronousInputHandler() override;
    void SetSynchronousInputHandlerRootScrollOffset(
        const gfx::ScrollOffset& root_offset) override;
    void ScrollEnd() override;
    InputHandler::ScrollStatus FlingScrollBegin() override;
    void MouseMoveAt(const gfx::Point& viewport_point) override;
    void PinchGestureBegin() override;
    void PinchGestureUpdate(float magnify_delta,
        const gfx::Point& anchor) override;
    void PinchGestureEnd() override;
    void StartPageScaleAnimation(const gfx::Vector2d& target_offset,
        bool anchor_point,
        float page_scale,
        base::TimeDelta duration);
    void SetNeedsAnimateInput() override;
    bool IsCurrentlyScrollingInnerViewport() const override;
    bool IsCurrentlyScrollingLayerAt(
        const gfx::Point& viewport_point,
        InputHandler::ScrollInputType type) const override;
    bool HaveWheelEventHandlersAt(const gfx::Point& viewport_point) override;
    bool DoTouchEventsBlockScrollAt(const gfx::Point& viewport_port) override;
    scoped_ptr<SwapPromiseMonitor> CreateLatencyInfoSwapPromiseMonitor(
        ui::LatencyInfo* latency) override;
    ScrollElasticityHelper* CreateScrollElasticityHelper() override;

    // TopControlsManagerClient implementation.
    float TopControlsHeight() const override;
    void SetCurrentTopControlsShownRatio(float offset) override;
    float CurrentTopControlsShownRatio() const override;
    void DidChangeTopControlsPosition() override;
    bool HaveRootScrollLayer() const override;

    void UpdateViewportContainerSizes();

    struct CC_EXPORT FrameData : public RenderPassSink {
        FrameData();
        ~FrameData() override;
        void AsValueInto(base::trace_event::TracedValue* value) const;

        std::vector<gfx::Rect> occluding_screen_space_rects;
        std::vector<gfx::Rect> non_occluding_screen_space_rects;
        std::vector<FrameTimingTracker::FrameAndRectIds> composite_events;
        RenderPassList render_passes;
        RenderPassIdHashMap render_passes_by_id;
        const LayerImplList* render_surface_layer_list;
        LayerImplList will_draw_layers;
        bool has_no_damage;

        // RenderPassSink implementation.
        void AppendRenderPass(scoped_ptr<RenderPass> render_pass) override;
    };

    virtual void BeginMainFrameAborted(CommitEarlyOutReason reason);
    virtual void BeginCommit();
    virtual void CommitComplete();
    virtual void Animate();
    virtual void UpdateAnimationState(bool start_ready_animations);
    void ActivateAnimations();
    void MainThreadHasStoppedFlinging();
    void DidAnimateScrollOffset();
    void SetViewportDamage(const gfx::Rect& damage_rect);

    void SetTreeLayerFilterMutated(int layer_id,
        LayerTreeImpl* tree,
        const FilterOperations& filters);
    void SetTreeLayerOpacityMutated(int layer_id,
        LayerTreeImpl* tree,
        float opacity);
    void SetTreeLayerTransformMutated(int layer_id,
        LayerTreeImpl* tree,
        const gfx::Transform& transform);
    void SetTreeLayerScrollOffsetMutated(int layer_id,
        LayerTreeImpl* tree,
        const gfx::ScrollOffset& scroll_offset);
    void TreeLayerTransformIsPotentiallyAnimatingChanged(int layer_id,
        LayerTreeImpl* tree,
        bool is_animating);

    // LayerTreeMutatorsClient implementation.
    bool IsLayerInTree(int layer_id, LayerTreeType tree_type) const override;
    void SetMutatorsNeedCommit() override;
    void SetLayerFilterMutated(int layer_id,
        LayerTreeType tree_type,
        const FilterOperations& filters) override;
    void SetLayerOpacityMutated(int layer_id,
        LayerTreeType tree_type,
        float opacity) override;
    void SetLayerTransformMutated(int layer_id,
        LayerTreeType tree_type,
        const gfx::Transform& transform) override;
    void SetLayerScrollOffsetMutated(
        int layer_id,
        LayerTreeType tree_type,
        const gfx::ScrollOffset& scroll_offset) override;
    void LayerTransformIsPotentiallyAnimatingChanged(int layer_id,
        LayerTreeType tree_type,
        bool is_animating) override;
    void ScrollOffsetAnimationFinished() override;
    gfx::ScrollOffset GetScrollOffsetForAnimation(int layer_id) const override;

    virtual bool PrepareTiles();

    // Returns DRAW_SUCCESS unless problems occured preparing the frame, and we
    // should try to avoid displaying the frame. If PrepareToDraw is called,
    // DidDrawAllLayers must also be called, regardless of whether DrawLayers is
    // called between the two.
    virtual DrawResult PrepareToDraw(FrameData* frame);
    virtual void DrawLayers(FrameData* frame);
    // Must be called if and only if PrepareToDraw was called.
    void DidDrawAllLayers(const FrameData& frame);

    const LayerTreeSettings& settings() const { return settings_; }

    // Evict all textures by enforcing a memory policy with an allocation of 0.
    void EvictTexturesForTesting();

    // When blocking, this prevents client_->NotifyReadyToActivate() from being
    // called. When disabled, it calls client_->NotifyReadyToActivate()
    // immediately if any notifications had been blocked while blocking.
    virtual void BlockNotifyReadyToActivateForTesting(bool block);

    // Resets all of the trees to an empty state.
    void ResetTreesForTesting();

    size_t SourceAnimationFrameNumberForTesting() const;

    DrawMode GetDrawMode() const;

    // Viewport size in draw space: this size is in physical pixels and is used
    // for draw properties, tilings, quads and render passes.
    gfx::Size DrawViewportSize() const;

    // Viewport rect in view space used for tiling prioritization.
    const gfx::Rect ViewportRectForTilePriority() const;

    // RendererClient implementation.
    void SetFullRootLayerDamage() override;

    // TileManagerClient implementation.
    void NotifyReadyToActivate() override;
    void NotifyReadyToDraw() override;
    void NotifyAllTileTasksCompleted() override;
    void NotifyTileStateChanged(const Tile* tile) override;
    scoped_ptr<RasterTilePriorityQueue> BuildRasterQueue(
        TreePriority tree_priority,
        RasterTilePriorityQueue::Type type) override;
    scoped_ptr<EvictionTilePriorityQueue> BuildEvictionQueue(
        TreePriority tree_priority) override;
    void SetIsLikelyToRequireADraw(bool is_likely_to_require_a_draw) override;

    // ScrollbarAnimationControllerClient implementation.
    void StartAnimatingScrollbarAnimationController(
        ScrollbarAnimationController* controller) override;
    void StopAnimatingScrollbarAnimationController(
        ScrollbarAnimationController* controller) override;
    void PostDelayedScrollbarAnimationTask(const base::Closure& task,
        base::TimeDelta delay) override;
    void SetNeedsRedrawForScrollbarAnimation() override;

    // VideoBeginFrameSource implementation.
    void AddVideoFrameController(VideoFrameController* controller) override;
    void RemoveVideoFrameController(VideoFrameController* controller) override;

    // OutputSurfaceClient implementation.
    void CommitVSyncParameters(base::TimeTicks timebase,
        base::TimeDelta interval) override;
    void SetNeedsRedrawRect(const gfx::Rect& rect) override;
    void SetExternalDrawConstraints(
        const gfx::Transform& transform,
        const gfx::Rect& viewport,
        const gfx::Rect& clip,
        const gfx::Rect& viewport_rect_for_tile_priority,
        const gfx::Transform& transform_for_tile_priority,
        bool resourceless_software_draw) override;
    void DidLoseOutputSurface() override;
    void DidSwapBuffers() override;
    void DidSwapBuffersComplete() override;
    void ReclaimResources(const CompositorFrameAck* ack) override;
    void SetMemoryPolicy(const ManagedMemoryPolicy& policy) override;
    void SetTreeActivationCallback(const base::Closure& callback) override;
    void OnDraw() override;

    // Called from LayerTreeImpl.
    void OnCanDrawStateChangedForTree();

    // Implementation.
    int id() const { return id_; }
    bool CanDraw() const;
    OutputSurface* output_surface() const { return output_surface_; }
    void ReleaseOutputSurface();

    std::string LayerTreeAsJson() const;

    void FinishAllRendering();
    int RequestedMSAASampleCount() const;

    virtual bool InitializeRenderer(OutputSurface* output_surface);
    TileManager* tile_manager() { return tile_manager_.get(); }

    void SetHasGpuRasterizationTrigger(bool flag)
    {
        has_gpu_rasterization_trigger_ = flag;
        UpdateGpuRasterizationStatus();
    }
    void SetContentIsSuitableForGpuRasterization(bool flag)
    {
        content_is_suitable_for_gpu_rasterization_ = flag;
        UpdateGpuRasterizationStatus();
    }
    bool CanUseGpuRasterization();
    void UpdateTreeResourcesForGpuRasterizationIfNeeded();
    bool use_gpu_rasterization() const { return use_gpu_rasterization_; }
    bool use_msaa() const { return use_msaa_; }

    GpuRasterizationStatus gpu_rasterization_status() const
    {
        return gpu_rasterization_status_;
    }

    bool create_low_res_tiling() const
    {
        return settings_.create_low_res_tiling && !use_gpu_rasterization_;
    }
    ResourcePool* resource_pool() { return resource_pool_.get(); }
    Renderer* renderer() { return renderer_.get(); }
    const RendererCapabilitiesImpl& GetRendererCapabilities() const;

    virtual bool SwapBuffers(const FrameData& frame);
    virtual void WillBeginImplFrame(const BeginFrameArgs& args);
    virtual void DidFinishImplFrame();
    void DidModifyTilePriorities();

    LayerTreeImpl* active_tree() { return active_tree_.get(); }
    const LayerTreeImpl* active_tree() const { return active_tree_.get(); }
    LayerTreeImpl* pending_tree() { return pending_tree_.get(); }
    const LayerTreeImpl* pending_tree() const { return pending_tree_.get(); }
    LayerTreeImpl* recycle_tree() { return recycle_tree_.get(); }
    const LayerTreeImpl* recycle_tree() const { return recycle_tree_.get(); }
    // Returns the tree LTH synchronizes with.
    LayerTreeImpl* sync_tree()
    {
        // TODO(enne): This is bogus.  It should return based on the value of
        // Proxy::CommitToActiveTree and not whether the pending tree exists.
        return pending_tree_ ? pending_tree_.get() : active_tree_.get();
    }
    virtual void CreatePendingTree();
    virtual void ActivateSyncTree();

    // Shortcuts to layers on the active tree.
    LayerImpl* RootLayer() const;
    LayerImpl* InnerViewportScrollLayer() const;
    LayerImpl* OuterViewportScrollLayer() const;
    LayerImpl* CurrentlyScrollingLayer() const;

    int scroll_layer_id_when_mouse_over_scrollbar() const
    {
        return scroll_layer_id_when_mouse_over_scrollbar_;
    }
    bool scroll_affects_scroll_handler() const
    {
        return scroll_affects_scroll_handler_;
    }
    void QueueSwapPromiseForMainThreadScrollUpdate(
        scoped_ptr<SwapPromise> swap_promise);

    bool IsActivelyScrolling() const;

    virtual void SetVisible(bool visible);
    bool visible() const { return visible_; }

    void SetNeedsCommit() { client_->SetNeedsCommitOnImplThread(); }
    void SetNeedsAnimate();
    void SetNeedsRedraw();

    ManagedMemoryPolicy ActualManagedMemoryPolicy() const;

    size_t memory_allocation_limit_bytes() const;

    void SetViewportSize(const gfx::Size& device_viewport_size);
    gfx::Size device_viewport_size() const { return device_viewport_size_; }

    const gfx::Transform& DrawTransform() const;

    scoped_ptr<ScrollAndScaleSet> ProcessScrollDeltas();

    void set_max_memory_needed_bytes(size_t bytes)
    {
        max_memory_needed_bytes_ = bytes;
    }

    FrameRateCounter* fps_counter()
    {
        return fps_counter_.get();
    }
    MemoryHistory* memory_history()
    {
        return memory_history_.get();
    }
    DebugRectHistory* debug_rect_history()
    {
        return debug_rect_history_.get();
    }
    ResourceProvider* resource_provider()
    {
        return resource_provider_.get();
    }
    TopControlsManager* top_controls_manager()
    {
        return top_controls_manager_.get();
    }
    const GlobalStateThatImpactsTilePriority& global_tile_state()
    {
        return global_tile_state_;
    }

    Proxy* proxy() const { return proxy_; }

    AnimationRegistrar* animation_registrar() const
    {
        return animation_registrar_.get();
    }
    AnimationHost* animation_host() const { return animation_host_.get(); }

    void SetDebugState(const LayerTreeDebugState& new_debug_state);
    const LayerTreeDebugState& debug_state() const { return debug_state_; }

    gfx::Vector2dF accumulated_root_overscroll() const
    {
        return accumulated_root_overscroll_;
    }

    bool pinch_gesture_active() const { return pinch_gesture_active_; }

    void SetTreePriority(TreePriority priority);
    TreePriority GetTreePriority() const;

    // TODO(mithro): Remove this methods which exposes the internal
    // BeginFrameArgs to external callers.
    virtual BeginFrameArgs CurrentBeginFrameArgs() const;

    // Expected time between two begin impl frame calls.
    base::TimeDelta CurrentBeginFrameInterval() const;

    void AsValueWithFrameInto(FrameData* frame,
        base::trace_event::TracedValue* value) const;
    scoped_refptr<base::trace_event::ConvertableToTraceFormat> AsValueWithFrame(
        FrameData* frame) const;
    void ActivationStateAsValueInto(base::trace_event::TracedValue* value) const;

    bool page_scale_animation_active() const { return !!page_scale_animation_; }

    virtual void CreateUIResource(UIResourceId uid,
        const UIResourceBitmap& bitmap);
    // Deletes a UI resource.  May safely be called more than once.
    virtual void DeleteUIResource(UIResourceId uid);
    void EvictAllUIResources();
    bool EvictedUIResourcesExist() const;

    virtual ResourceId ResourceIdForUIResource(UIResourceId uid) const;

    virtual bool IsUIResourceOpaque(UIResourceId uid) const;

    struct UIResourceData {
        ResourceId resource_id;
        gfx::Size size;
        bool opaque;
    };

    void ScheduleMicroBenchmark(scoped_ptr<MicroBenchmarkImpl> benchmark);

    CompositorFrameMetadata MakeCompositorFrameMetadata() const;
    // Viewport rectangle and clip in nonflipped window space.  These rects
    // should only be used by Renderer subclasses to populate glViewport/glClip
    // and their software-mode equivalents.
    gfx::Rect DeviceViewport() const;
    gfx::Rect DeviceClip() const;

    // When a SwapPromiseMonitor is created on the impl thread, it calls
    // InsertSwapPromiseMonitor() to register itself with LayerTreeHostImpl.
    // When the monitor is destroyed, it calls RemoveSwapPromiseMonitor()
    // to unregister itself.
    void InsertSwapPromiseMonitor(SwapPromiseMonitor* monitor);
    void RemoveSwapPromiseMonitor(SwapPromiseMonitor* monitor);

    // TODO(weiliangc): Replace RequiresHighResToDraw with scheduler waits for
    // ReadyToDraw. crbug.com/469175
    void SetRequiresHighResToDraw() { requires_high_res_to_draw_ = true; }
    void ResetRequiresHighResToDraw() { requires_high_res_to_draw_ = false; }
    bool RequiresHighResToDraw() const { return requires_high_res_to_draw_; }

    // Only valid for synchronous (non-scheduled) single-threaded case.
    void SynchronouslyInitializeAllTiles();

    virtual void CreateResourceAndTileTaskWorkerPool(
        scoped_ptr<TileTaskWorkerPool>* tile_task_worker_pool,
        scoped_ptr<ResourcePool>* resource_pool);

    bool prepare_tiles_needed() const { return tile_priorities_dirty_; }

    FrameTimingTracker* frame_timing_tracker()
    {
        return frame_timing_tracker_.get();
    }

    gfx::Vector2dF ScrollLayer(LayerImpl* layer_impl,
        const gfx::Vector2dF& delta,
        const gfx::Point& viewport_point,
        bool is_direct_manipulation);

    // Record main frame timing information.
    // |start_of_main_frame_args| is the BeginFrameArgs of the beginning of the
    // main frame (ie the frame that kicked off the main frame).
    // |expected_next_main_frame_args| is the BeginFrameArgs of the frame that
    // follows the completion of the main frame (whether it is activation or some
    // other completion, such as early out). Note that if there is a main frame
    // scheduled in that frame, then this BeginFrameArgs will become the main
    // frame args. However, if no such frame is scheduled, then this _would_ be
    // the main frame args if it was scheduled.
    void RecordMainFrameTiming(
        const BeginFrameArgs& start_of_main_frame_args,
        const BeginFrameArgs& expected_next_main_frame_args);

    // Post the given frame timing events to the requester.
    void PostFrameTimingEvents(
        scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
        scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events);

protected:
    LayerTreeHostImpl(
        const LayerTreeSettings& settings,
        LayerTreeHostImplClient* client,
        Proxy* proxy,
        RenderingStatsInstrumentation* rendering_stats_instrumentation,
        SharedBitmapManager* shared_bitmap_manager,
        gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
        TaskGraphRunner* task_graph_runner,
        int id);

    // Virtual for testing.
    virtual void AnimateLayers(base::TimeTicks monotonic_time);

    bool is_likely_to_require_a_draw() const
    {
        return is_likely_to_require_a_draw_;
    }

    // Removes empty or orphan RenderPasses from the frame.
    static void RemoveRenderPasses(FrameData* frame);

    LayerTreeHostImplClient* client_;
    Proxy* proxy_;

    BeginFrameTracker current_begin_frame_tracker_;

private:
    gfx::Vector2dF ScrollLayerWithViewportSpaceDelta(
        LayerImpl* layer_impl,
        const gfx::PointF& viewport_point,
        const gfx::Vector2dF& viewport_delta);

    void CreateAndSetRenderer();
    void CleanUpTileManager();
    void CreateTileManagerResources();
    void ReleaseTreeResources();
    void RecreateTreeResources();

    void UpdateGpuRasterizationStatus();

    Viewport* viewport() { return viewport_.get(); }

    // Scroll by preferring to move the outer viewport first, only moving the
    // inner if the outer is at its scroll extents.
    void ScrollViewportBy(gfx::Vector2dF scroll_delta);
    // Scroll by preferring to move the inner viewport first, only moving the
    // outer if the inner is at its scroll extents.
    void ScrollViewportInnerFirst(gfx::Vector2dF scroll_delta);

    InputHandler::ScrollStatus ScrollBeginImpl(
        LayerImpl* scrolling_layer_impl,
        InputHandler::ScrollInputType type);

    void AnimatePageScale(base::TimeTicks monotonic_time);
    void AnimateScrollbars(base::TimeTicks monotonic_time);
    void AnimateTopControls(base::TimeTicks monotonic_time);

    void TrackDamageForAllSurfaces(
        LayerImpl* root_draw_layer,
        const LayerImplList& render_surface_layer_list);

    void UpdateTileManagerMemoryPolicy(const ManagedMemoryPolicy& policy);

    // This function should only be called from PrepareToDraw, as DidDrawAllLayers
    // must be called if this helper function is called.  Returns DRAW_SUCCESS if
    // the frame should be drawn.
    DrawResult CalculateRenderPasses(FrameData* frame);

    void ClearCurrentlyScrollingLayer();

    bool HandleMouseOverScrollbar(LayerImpl* layer_impl,
        const gfx::PointF& device_viewport_point);

    LayerImpl* FindScrollLayerForDeviceViewportPoint(
        const gfx::PointF& device_viewport_point,
        InputHandler::ScrollInputType type,
        LayerImpl* layer_hit_by_point,
        bool* scroll_on_main_thread,
        bool* optional_has_ancestor_scroll_handler) const;
    float DeviceSpaceDistanceToLayer(const gfx::PointF& device_viewport_point,
        LayerImpl* layer_impl);
    void StartScrollbarFadeRecursive(LayerImpl* layer);
    void SetManagedMemoryPolicy(const ManagedMemoryPolicy& policy);

    void MarkUIResourceNotEvicted(UIResourceId uid);

    void NotifySwapPromiseMonitorsOfSetNeedsRedraw();
    void NotifySwapPromiseMonitorsOfForwardingToMainThread();

    void UpdateRootLayerStateForSynchronousInputHandler();

    void ScrollAnimationCreate(LayerImpl* layer_impl,
        const gfx::ScrollOffset& target_offset,
        const gfx::ScrollOffset& current_offset);
    bool ScrollAnimationUpdateTarget(LayerImpl* layer_impl,
        const gfx::Vector2dF& scroll_delta);

    base::SingleThreadTaskRunner* GetTaskRunner() const
    {
        DCHECK(proxy_);
        return proxy_->HasImplThread() ? proxy_->ImplThreadTaskRunner()
                                       : proxy_->MainThreadTaskRunner();
    }

    typedef base::hash_map<UIResourceId, UIResourceData>
        UIResourceMap;
    UIResourceMap ui_resource_map_;

    // Resources that were evicted by EvictAllUIResources. Resources are removed
    // from this when they are touched by a create or destroy from the UI resource
    // request queue.
    std::set<UIResourceId> evicted_ui_resources_;

    OutputSurface* output_surface_;

    scoped_ptr<ResourceProvider> resource_provider_;
    bool content_is_suitable_for_gpu_rasterization_;
    bool has_gpu_rasterization_trigger_;
    bool use_gpu_rasterization_;
    bool use_msaa_;
    GpuRasterizationStatus gpu_rasterization_status_;
    bool tree_resources_for_gpu_rasterization_dirty_;
    scoped_ptr<TileTaskWorkerPool> tile_task_worker_pool_;
    scoped_ptr<ResourcePool> resource_pool_;
    scoped_ptr<Renderer> renderer_;

    GlobalStateThatImpactsTilePriority global_tile_state_;

    // Tree currently being drawn.
    scoped_ptr<LayerTreeImpl> active_tree_;

    // In impl-side painting mode, tree with possibly incomplete rasterized
    // content. May be promoted to active by ActivatePendingTree().
    scoped_ptr<LayerTreeImpl> pending_tree_;

    // In impl-side painting mode, inert tree with layers that can be recycled
    // by the next sync from the main thread.
    scoped_ptr<LayerTreeImpl> recycle_tree_;

    InputHandlerClient* input_handler_client_;
    bool did_lock_scrolling_layer_;
    bool wheel_scrolling_;
    bool scroll_affects_scroll_handler_;
    int scroll_layer_id_when_mouse_over_scrollbar_;
    ScopedPtrVector<SwapPromise> swap_promises_for_main_thread_scroll_update_;

    // An object to implement the ScrollElasticityHelper interface and
    // hold all state related to elasticity. May be NULL if never requested.
    scoped_ptr<ScrollElasticityHelper> scroll_elasticity_helper_;

    bool tile_priorities_dirty_;

    const LayerTreeSettings settings_;
    LayerTreeDebugState debug_state_;
    bool visible_;
    ManagedMemoryPolicy cached_managed_memory_policy_;

    const bool is_synchronous_single_threaded_;
    scoped_ptr<TileManager> tile_manager_;

    gfx::Vector2dF accumulated_root_overscroll_;

    bool pinch_gesture_active_;
    bool pinch_gesture_end_should_clear_scrolling_layer_;

    scoped_ptr<TopControlsManager> top_controls_manager_;

    scoped_ptr<PageScaleAnimation> page_scale_animation_;

    scoped_ptr<FrameRateCounter> fps_counter_;
    scoped_ptr<MemoryHistory> memory_history_;
    scoped_ptr<DebugRectHistory> debug_rect_history_;

    scoped_ptr<TextureMailboxDeleter> texture_mailbox_deleter_;

    // The maximum memory that would be used by the prioritized resource
    // manager, if there were no limit on memory usage.
    size_t max_memory_needed_bytes_;

    // Viewport size passed in from the main thread, in physical pixels.  This
    // value is the default size for all concepts of physical viewport (draw
    // viewport, scrolling viewport and device viewport), but it can be
    // overridden.
    gfx::Size device_viewport_size_;

    // Optional top-level constraints that can be set by the OutputSurface.
    // - external_transform_ applies a transform above the root layer
    // - external_viewport_ is used DrawProperties, tile management and
    // glViewport/window projection matrix.
    // - external_clip_ specifies a top-level clip rect
    // - viewport_rect_for_tile_priority_ is the rect in view space used for
    // tiling priority.
    gfx::Transform external_transform_;
    gfx::Rect external_viewport_;
    gfx::Rect external_clip_;
    gfx::Rect viewport_rect_for_tile_priority_;
    bool resourceless_software_draw_;

    gfx::Rect viewport_damage_rect_;

    scoped_ptr<AnimationRegistrar> animation_registrar_;
    scoped_ptr<AnimationHost> animation_host_;
    std::set<ScrollbarAnimationController*> scrollbar_animation_controllers_;
    std::set<VideoFrameController*> video_frame_controllers_;

    RenderingStatsInstrumentation* rendering_stats_instrumentation_;
    //MicroBenchmarkControllerImpl micro_benchmark_controller_;
    scoped_ptr<TaskGraphRunner> single_thread_synchronous_task_graph_runner_;

    // Optional callback to notify of new tree activations.
    base::Closure tree_activation_callback_;

    SharedBitmapManager* shared_bitmap_manager_;
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager_;
    TaskGraphRunner* task_graph_runner_;
    int id_;

    std::set<SwapPromiseMonitor*> swap_promise_monitor_;

    bool requires_high_res_to_draw_;
    bool is_likely_to_require_a_draw_;

    scoped_ptr<FrameTimingTracker> frame_timing_tracker_;

    scoped_ptr<Viewport> viewport_;

    DISALLOW_COPY_AND_ASSIGN(LayerTreeHostImpl);
};

} // namespace cc

#endif // CC_TREES_LAYER_TREE_HOST_IMPL_H_
