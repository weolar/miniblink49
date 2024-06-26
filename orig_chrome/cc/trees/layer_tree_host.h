// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_LAYER_TREE_HOST_H_
#define CC_TREES_LAYER_TREE_HOST_H_

#include <limits>
#include <set>
#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/cancelable_callback.h"
#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "cc/animation/animation_events.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/debug/frame_timing_tracker.h"
#include "cc/debug/micro_benchmark.h"
#include "cc/debug/micro_benchmark_controller.h"
#include "cc/input/input_handler.h"
#include "cc/input/layer_selection_bound.h"
#include "cc/input/scrollbar.h"
#include "cc/input/top_controls_state.h"
#include "cc/layers/layer_lists.h"
#include "cc/output/output_surface.h"
#include "cc/output/renderer_capabilities.h"
#include "cc/output/swap_promise.h"
#include "cc/resources/resource_format.h"
#include "cc/resources/scoped_ui_resource.h"
#include "cc/surfaces/surface_sequence.h"
#include "cc/trees/layer_tree_host_client.h"
#include "cc/trees/layer_tree_host_common.h"
#include "cc/trees/layer_tree_settings.h"
#include "cc/trees/mutator_host_client.h"
#include "cc/trees/proxy.h"
#include "cc/trees/swap_promise_monitor.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/gfx/geometry/rect.h"

namespace gpu {
class GpuMemoryBufferManager;
}

namespace cc {
class AnimationRegistrar;
class AnimationHost;
class BeginFrameSource;
class HeadsUpDisplayLayer;
class Layer;
class LayerTreeHostImpl;
class LayerTreeHostImplClient;
class LayerTreeHostSingleThreadClient;
class PropertyTrees;
class Region;
class RenderingStatsInstrumentation;
class ResourceProvider;
class ResourceUpdateQueue;
class SharedBitmapManager;
class TaskGraphRunner;
class TopControlsManager;
class UIResourceRequest;
struct PendingPageScaleAnimation;
struct RenderingStats;
struct ScrollAndScaleSet;

class CC_EXPORT LayerTreeHost : public MutatorHostClient {
public:
    // TODO(sad): InitParams should be a movable type so that it can be
    // std::move()d to the Create* functions.
    struct CC_EXPORT InitParams {
        LayerTreeHostClient* client = nullptr;
        SharedBitmapManager* shared_bitmap_manager = nullptr;
        gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager = nullptr;
        TaskGraphRunner* task_graph_runner = nullptr;
        LayerTreeSettings const* settings = nullptr;
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner;
        scoped_ptr<BeginFrameSource> external_begin_frame_source;

        InitParams();
        ~InitParams();
    };

    // The SharedBitmapManager will be used on the compositor thread.
    static scoped_ptr<LayerTreeHost> CreateThreaded(
        scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
        InitParams* params);

    static scoped_ptr<LayerTreeHost> CreateSingleThreaded(
        LayerTreeHostSingleThreadClient* single_thread_client,
        InitParams* params);
    virtual ~LayerTreeHost();

    void SetLayerTreeHostClientReady();

    // LayerTreeHost interface to Proxy.
    void WillBeginMainFrame();
    void DidBeginMainFrame();
    void BeginMainFrame(const BeginFrameArgs& args);
    void BeginMainFrameNotExpectedSoon();
    void AnimateLayers(base::TimeTicks monotonic_frame_begin_time);
    void DidStopFlinging();
    void Layout();
    void FinishCommitOnImplThread(LayerTreeHostImpl* host_impl);
    void WillCommit();
    void CommitComplete();
    void SetOutputSurface(scoped_ptr<OutputSurface> output_surface);
    scoped_ptr<OutputSurface> ReleaseOutputSurface();
    void RequestNewOutputSurface();
    void DidInitializeOutputSurface();
    void DidFailToInitializeOutputSurface();
    virtual scoped_ptr<LayerTreeHostImpl> CreateLayerTreeHostImpl(
        LayerTreeHostImplClient* client);
    void DidLoseOutputSurface();
    bool output_surface_lost() const { return output_surface_lost_; }
    void DidCommitAndDrawFrame() { client_->DidCommitAndDrawFrame(); }
    void DidCompleteSwapBuffers() { client_->DidCompleteSwapBuffers(); }
    bool UpdateLayers();

    // Called when the compositor completed page scale animation.
    void DidCompletePageScaleAnimation();

    LayerTreeHostClient* client() { return client_; }
    const base::WeakPtr<InputHandler>& GetInputHandler()
    {
        return input_handler_weak_ptr_;
    }

    void NotifyInputThrottledUntilCommit();

    void LayoutAndUpdateLayers();
    void Composite(base::TimeTicks frame_begin_time);

    void FinishAllRendering();

    void SetDeferCommits(bool defer_commits);

    int source_frame_number() const { return source_frame_number_; }

    int meta_information_sequence_number()
    {
        return meta_information_sequence_number_;
    }

    void IncrementMetaInformationSequenceNumber()
    {
        meta_information_sequence_number_++;
    }

    void SetNeedsDisplayOnAllLayers();

    void CollectRenderingStats(RenderingStats* stats) const;

    RenderingStatsInstrumentation* rendering_stats_instrumentation() const
    {
        return rendering_stats_instrumentation_.get();
    }

    const RendererCapabilities& GetRendererCapabilities() const;

    void SetNeedsAnimate();
    virtual void SetNeedsUpdateLayers();
    virtual void SetNeedsCommit();
    virtual void SetNeedsFullTreeSync();
    virtual void SetNeedsMetaInfoRecomputation(
        bool needs_meta_info_recomputation);
    void SetNeedsRedraw();
    void SetNeedsRedrawRect(const gfx::Rect& damage_rect);
    bool CommitRequested() const;
    bool BeginMainFrameRequested() const;

    void SetNextCommitWaitsForActivation();

    void SetNextCommitForcesRedraw();

    void SetAnimationEvents(scoped_ptr<AnimationEventsVector> events);

    void SetRootLayer(scoped_refptr<Layer> root_layer);
    Layer* root_layer() { return root_layer_.get(); }
    const Layer* root_layer() const { return root_layer_.get(); }
    const Layer* overscroll_elasticity_layer() const
    {
        return overscroll_elasticity_layer_.get();
    }
    const Layer* page_scale_layer() const { return page_scale_layer_.get(); }
    void RegisterViewportLayers(scoped_refptr<Layer> overscroll_elasticity_layer,
        scoped_refptr<Layer> page_scale_layer,
        scoped_refptr<Layer> inner_viewport_scroll_layer,
        scoped_refptr<Layer> outer_viewport_scroll_layer);
    Layer* inner_viewport_scroll_layer() const
    {
        return inner_viewport_scroll_layer_.get();
    }
    Layer* outer_viewport_scroll_layer() const
    {
        return outer_viewport_scroll_layer_.get();
    }

    void RegisterSelection(const LayerSelection& selection);

    const LayerTreeSettings& settings() const { return settings_; }

    void SetDebugState(const LayerTreeDebugState& debug_state);
    const LayerTreeDebugState& debug_state() const { return debug_state_; }

    bool has_gpu_rasterization_trigger() const
    {
        return has_gpu_rasterization_trigger_;
    }
    void SetHasGpuRasterizationTrigger(bool has_trigger);

    void SetViewportSize(const gfx::Size& device_viewport_size);
    void SetTopControlsHeight(float height, bool shrink);
    void SetTopControlsShownRatio(float ratio);

    void set_hide_pinch_scrollbars_near_min_scale(bool hide)
    {
        hide_pinch_scrollbars_near_min_scale_ = hide;
    }

    gfx::Size device_viewport_size() const { return device_viewport_size_; }

    void ApplyPageScaleDeltaFromImplSide(float page_scale_delta);
    void SetPageScaleFactorAndLimits(float page_scale_factor,
        float min_page_scale_factor,
        float max_page_scale_factor);
    float page_scale_factor() const { return page_scale_factor_; }
    gfx::Vector2dF elastic_overscroll() const { return elastic_overscroll_; }

    SkColor background_color() const { return background_color_; }
    void set_background_color(SkColor color) { background_color_ = color; }

    void set_has_transparent_background(bool transparent)
    {
        has_transparent_background_ = transparent;
    }

    void SetVisible(bool visible);
    bool visible() const { return visible_; }

    void SetThrottleFrameProduction(bool throttle);

    void StartPageScaleAnimation(const gfx::Vector2d& target_offset,
        bool use_anchor,
        float scale,
        base::TimeDelta duration);

    void ApplyScrollAndScale(ScrollAndScaleSet* info);
    void SetImplTransform(const gfx::Transform& transform);

    void SetDeviceScaleFactor(float device_scale_factor);
    float device_scale_factor() const { return device_scale_factor_; }

    void UpdateTopControlsState(TopControlsState constraints,
        TopControlsState current,
        bool animate);

    HeadsUpDisplayLayer* hud_layer() const { return hud_layer_.get(); }

    Proxy* proxy() const { return proxy_.get(); }
    AnimationRegistrar* animation_registrar() const
    {
        return animation_registrar_.get();
    }
    AnimationHost* animation_host() const { return animation_host_.get(); }

    bool in_paint_layer_contents() const { return in_paint_layer_contents_; }

    // CreateUIResource creates a resource given a bitmap.  The bitmap is
    // generated via an interface function, which is called when initializing the
    // resource and when the resource has been lost (due to lost context).  The
    // parameter of the interface is a single boolean, which indicates whether the
    // resource has been lost or not.  CreateUIResource returns an Id of the
    // resource, which is always positive.
    virtual UIResourceId CreateUIResource(UIResourceClient* client);
    // Deletes a UI resource.  May safely be called more than once.
    virtual void DeleteUIResource(UIResourceId id);
    // Put the recreation of all UI resources into the resource queue after they
    // were evicted on the impl thread.
    void RecreateUIResources();

    virtual gfx::Size GetUIResourceSize(UIResourceId id) const;

    bool UsingSharedMemoryResources();
    int id() const { return id_; }

    // Returns the id of the benchmark on success, 0 otherwise.
    int ScheduleMicroBenchmark(const std::string& benchmark_name,
        scoped_ptr<base::Value> value,
        const MicroBenchmark::DoneCallback& callback);
    // Returns true if the message was successfully delivered and handled.
    bool SendMessageToMicroBenchmark(int id, scoped_ptr<base::Value> value);

    // When a SwapPromiseMonitor is created on the main thread, it calls
    // InsertSwapPromiseMonitor() to register itself with LayerTreeHost.
    // When the monitor is destroyed, it calls RemoveSwapPromiseMonitor()
    // to unregister itself.
    void InsertSwapPromiseMonitor(SwapPromiseMonitor* monitor);
    void RemoveSwapPromiseMonitor(SwapPromiseMonitor* monitor);

    // Call this function when you expect there to be a swap buffer.
    // See swap_promise.h for how to use SwapPromise.
    void QueueSwapPromise(scoped_ptr<SwapPromise> swap_promise);

    void BreakSwapPromises(SwapPromise::DidNotSwapReason reason);

    size_t num_queued_swap_promises() const { return swap_promise_list_.size(); }

    void set_surface_id_namespace(uint32_t id_namespace);
    SurfaceSequence CreateSurfaceSequence();

    void SetChildrenNeedBeginFrames(bool children_need_begin_frames) const;
    void SendBeginFramesToChildren(const BeginFrameArgs& args) const;

    void SetAuthoritativeVSyncInterval(const base::TimeDelta& interval);

    PropertyTrees* property_trees() { return &property_trees_; }
    bool needs_meta_info_recomputation()
    {
        return needs_meta_info_recomputation_;
    }

    void RecordFrameTimingEvents(
        scoped_ptr<FrameTimingTracker::CompositeTimingSet> composite_events,
        scoped_ptr<FrameTimingTracker::MainFrameTimingSet> main_frame_events);

    Layer* LayerById(int id) const;
    void RegisterLayer(Layer* layer);
    void UnregisterLayer(Layer* layer);
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
    void ScrollOffsetAnimationFinished() override { }
    gfx::ScrollOffset GetScrollOffsetForAnimation(int layer_id) const override;

    bool ScrollOffsetAnimationWasInterrupted(const Layer* layer) const;
    bool IsAnimatingFilterProperty(const Layer* layer) const;
    bool IsAnimatingOpacityProperty(const Layer* layer) const;
    bool IsAnimatingTransformProperty(const Layer* layer) const;
    bool HasPotentiallyRunningFilterAnimation(const Layer* layer) const;
    bool HasPotentiallyRunningOpacityAnimation(const Layer* layer) const;
    bool HasPotentiallyRunningTransformAnimation(const Layer* layer) const;
    bool HasOnlyTranslationTransforms(const Layer* layer) const;
    bool MaximumTargetScale(const Layer* layer, float* max_scale) const;
    bool AnimationStartScale(const Layer* layer, float* start_scale) const;
    bool HasAnyAnimationTargetingProperty(
        const Layer* layer,
        Animation::TargetProperty property) const;
    bool AnimationsPreserveAxisAlignment(const Layer* layer) const;
    bool HasAnyAnimation(const Layer* layer) const;
    bool HasActiveAnimation(const Layer* layer) const;

protected:
    explicit LayerTreeHost(InitParams* params);
    void InitializeThreaded(
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner,
        scoped_ptr<BeginFrameSource> external_begin_frame_source);
    void InitializeSingleThreaded(
        LayerTreeHostSingleThreadClient* single_thread_client,
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_ptr<BeginFrameSource> external_begin_frame_source);
    void InitializeForTesting(scoped_ptr<Proxy> proxy_for_testing);
    void SetOutputSurfaceLostForTesting(bool is_lost)
    {
        output_surface_lost_ = is_lost;
    }

    // shared_bitmap_manager(), gpu_memory_buffer_manager(), and
    // task_graph_runner() return valid values only until the LayerTreeHostImpl is
    // created in CreateLayerTreeHostImpl().
    SharedBitmapManager* shared_bitmap_manager() const
    {
        return shared_bitmap_manager_;
    }
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager() const
    {
        return gpu_memory_buffer_manager_;
    }
    TaskGraphRunner* task_graph_runner() const { return task_graph_runner_; }

    //MicroBenchmarkController micro_benchmark_controller_;

    void OnCommitForSwapPromises();

private:
    void InitializeProxy(scoped_ptr<Proxy> proxy);

    bool DoUpdateLayers(Layer* root_layer);
    void UpdateHudLayer();

    bool AnimateLayersRecursive(Layer* current, base::TimeTicks time);

    struct UIResourceClientData {
        UIResourceClient* client;
        gfx::Size size;
    };

    typedef base::hash_map<UIResourceId, UIResourceClientData>
        UIResourceClientMap;
    UIResourceClientMap ui_resource_client_map_;
    int next_ui_resource_id_;

    typedef std::vector<UIResourceRequest> UIResourceRequestQueue;
    UIResourceRequestQueue ui_resource_request_queue_;

    void RecordGpuRasterizationHistogram();
    void CalculateLCDTextMetricsCallback(Layer* layer);

    void NotifySwapPromiseMonitorsOfSetNeedsCommit();

    void SetPropertyTreesNeedRebuild();

    bool needs_full_tree_sync_;
    bool needs_meta_info_recomputation_;

    LayerTreeHostClient* client_;
    scoped_ptr<Proxy> proxy_;

    int source_frame_number_;
    int meta_information_sequence_number_;
    scoped_ptr<RenderingStatsInstrumentation> rendering_stats_instrumentation_;

    // |current_output_surface_| can't be updated until we've successfully
    // initialized a new output surface. |new_output_surface_| contains the
    // new output surface that is currently being initialized. If initialization
    // is successful then |new_output_surface_| replaces
    // |current_output_surface_|.
    scoped_ptr<OutputSurface> new_output_surface_;
    scoped_ptr<OutputSurface> current_output_surface_;
    bool output_surface_lost_;

    scoped_refptr<Layer> root_layer_;
    scoped_refptr<HeadsUpDisplayLayer> hud_layer_;

    base::WeakPtr<InputHandler> input_handler_weak_ptr_;
    base::WeakPtr<TopControlsManager> top_controls_manager_weak_ptr_;

    const LayerTreeSettings settings_;
    LayerTreeDebugState debug_state_;

    gfx::Size device_viewport_size_;
    bool top_controls_shrink_blink_size_;
    float top_controls_height_;
    float top_controls_shown_ratio_;
    bool hide_pinch_scrollbars_near_min_scale_;
    float device_scale_factor_;

    bool visible_;

    float page_scale_factor_;
    float min_page_scale_factor_;
    float max_page_scale_factor_;
    gfx::Vector2dF elastic_overscroll_;
    bool has_gpu_rasterization_trigger_;
    bool content_is_suitable_for_gpu_rasterization_;
    bool gpu_rasterization_histogram_recorded_;

    SkColor background_color_;
    bool has_transparent_background_;

    scoped_ptr<AnimationRegistrar> animation_registrar_;
    scoped_ptr<AnimationHost> animation_host_;

    scoped_ptr<PendingPageScaleAnimation> pending_page_scale_animation_;

    // If set, then page scale animation has completed, but the client hasn't been
    // notified about it yet.
    bool did_complete_scale_animation_;

    bool in_paint_layer_contents_;

    int id_;
    bool next_commit_forces_redraw_;

    scoped_refptr<Layer> overscroll_elasticity_layer_;
    scoped_refptr<Layer> page_scale_layer_;
    scoped_refptr<Layer> inner_viewport_scroll_layer_;
    scoped_refptr<Layer> outer_viewport_scroll_layer_;

    LayerSelection selection_;

    SharedBitmapManager* shared_bitmap_manager_;
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager_;
    TaskGraphRunner* task_graph_runner_;

    ScopedPtrVector<SwapPromise> swap_promise_list_;
    std::set<SwapPromiseMonitor*> swap_promise_monitor_;

    PropertyTrees property_trees_;

    typedef base::hash_map<int, Layer*> LayerIdMap;
    LayerIdMap layer_id_map_;

    uint32_t surface_id_namespace_;
    uint32_t next_surface_sequence_;

    DISALLOW_COPY_AND_ASSIGN(LayerTreeHost);

#ifndef NOT_QB_AERO
public:
    // Set position inset of hud layer
    void SetHudLayerTopInset(int inset);

private:
    // The top inset of the root content layer to document rect.
    int hud_layer_top_inset_ = 0;
#endif // NOT_QB_AERO
};

} // namespace cc

#endif // CC_TREES_LAYER_TREE_HOST_H_
