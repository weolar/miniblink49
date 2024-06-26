// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_LAYER_IMPL_H_
#define CC_LAYERS_LAYER_IMPL_H_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/values.h"
#include "cc/animation/animation_delegate.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/animation/layer_animation_value_observer.h"
#include "cc/animation/layer_animation_value_provider.h"
#include "cc/base/cc_export.h"
#include "cc/base/region.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/base/synced_property.h"
#include "cc/debug/frame_timing_request.h"
#include "cc/input/input_handler.h"
#include "cc/input/scrollbar.h"
#include "cc/layers/draw_properties.h"
#include "cc/layers/layer_lists.h"
#include "cc/layers/layer_position_constraint.h"
#include "cc/layers/render_surface_impl.h"
#include "cc/layers/scroll_blocks_on.h"
#include "cc/output/filter_operations.h"
#include "cc/quads/shared_quad_state.h"
#include "cc/resources/resource_provider.h"
#include "cc/tiles/tile_priority.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkImageFilter.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "ui/gfx/geometry/point3_f.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/scroll_offset.h"
#include "ui/gfx/transform.h"

namespace base {
namespace trace_event {
    class ConvertableToTraceFormat;
    class TracedValue;
}
class DictionaryValue;
}

namespace cc {

class LayerTreeHostImpl;
class LayerTreeImpl;
class MicroBenchmarkImpl;
class Occlusion;
class EffectTree;
class PrioritizedTile;
class RenderPass;
class RenderPassId;
class Renderer;
class ScrollbarAnimationController;
class ScrollbarLayerImplBase;
class SimpleEnclosedRegion;
class Tile;
class TransformTree;
class ScrollState;

struct AppendQuadsData;

enum DrawMode {
    DRAW_MODE_NONE,
    DRAW_MODE_HARDWARE,
    DRAW_MODE_SOFTWARE,
    DRAW_MODE_RESOURCELESS_SOFTWARE
};

class CC_EXPORT LayerImpl : public LayerAnimationValueObserver,
                            public LayerAnimationValueProvider,
                            public AnimationDelegate {
public:
    typedef SyncedProperty<AdditionGroup<gfx::ScrollOffset>> SyncedScrollOffset;
    typedef LayerImplList RenderSurfaceListType;
    typedef LayerImplList LayerListType;
    typedef RenderSurfaceImpl RenderSurfaceType;

    enum RenderingContextConstants { NO_RENDERING_CONTEXT = 0 };

    static scoped_ptr<LayerImpl> Create(
        LayerTreeImpl* tree_impl,
        int id,
        scoped_refptr<SyncedScrollOffset> scroll_offset)
    {
        return make_scoped_ptr(new LayerImpl(tree_impl, id, scroll_offset));
    }

    static scoped_ptr<LayerImpl> Create(LayerTreeImpl* tree_impl, int id)
    {
        return make_scoped_ptr(new LayerImpl(tree_impl, id));
    }

    ~LayerImpl() override;

    int id() const { return layer_id_; }

    // LayerAnimationValueProvider implementation.
    gfx::ScrollOffset ScrollOffsetForAnimation() const override;

    // LayerAnimationValueObserver implementation.
    void OnFilterAnimated(const FilterOperations& filters) override;
    void OnOpacityAnimated(float opacity) override;
    void OnTransformAnimated(const gfx::Transform& transform) override;
    void OnScrollOffsetAnimated(const gfx::ScrollOffset& scroll_offset) override;
    void OnAnimationWaitingForDeletion() override;
    void OnTransformIsPotentiallyAnimatingChanged(bool is_animating) override;
    bool IsActive() const override;

    // AnimationDelegate implementation.
    void NotifyAnimationStarted(base::TimeTicks monotonic_time,
        Animation::TargetProperty target_property,
        int group) override {};
    void NotifyAnimationFinished(base::TimeTicks monotonic_time,
        Animation::TargetProperty target_property,
        int group) override;

    // Tree structure.
    LayerImpl* parent() { return parent_; }
    const LayerImpl* parent() const { return parent_; }
    const OwnedLayerImplList& children() const { return children_; }
    OwnedLayerImplList& children() { return children_; }
    LayerImpl* child_at(size_t index) const { return children_[index]; }
    void AddChild(scoped_ptr<LayerImpl> child);
    scoped_ptr<LayerImpl> RemoveChild(LayerImpl* child);
    void SetParent(LayerImpl* parent);

    // Warning: This does not preserve tree structure invariants.
    void ClearChildList();

    bool HasAncestor(const LayerImpl* ancestor) const;

    void SetScrollParent(LayerImpl* parent);

    LayerImpl* scroll_parent() { return scroll_parent_; }
    const LayerImpl* scroll_parent() const { return scroll_parent_; }

    void SetScrollChildren(std::set<LayerImpl*>* children);

    std::set<LayerImpl*>* scroll_children() { return scroll_children_.get(); }
    const std::set<LayerImpl*>* scroll_children() const
    {
        return scroll_children_.get();
    }

    void DistributeScroll(ScrollState* scroll_state);
    void ApplyScroll(ScrollState* scroll_state);

    void set_property_tree_sequence_number(int sequence_number) { }

    void SetTransformTreeIndex(int index);
    int transform_tree_index() const { return transform_tree_index_; }

    void SetClipTreeIndex(int index);
    int clip_tree_index() const { return clip_tree_index_; }

    void SetEffectTreeIndex(int index);
    int effect_tree_index() const { return effect_tree_index_; }

    void set_offset_to_transform_parent(const gfx::Vector2dF& offset)
    {
        offset_to_transform_parent_ = offset;
        SetNeedsPushProperties();
    }
    gfx::Vector2dF offset_to_transform_parent() const
    {
        return offset_to_transform_parent_;
    }

    const gfx::Rect& visible_rect_from_property_trees() const
    {
        return visible_rect_from_property_trees_;
    }
    void set_visible_rect_from_property_trees(const gfx::Rect& rect)
    {
        visible_rect_from_property_trees_ = rect;
    }

    const gfx::Rect& clip_rect_in_target_space_from_property_trees() const
    {
        return clip_rect_in_target_space_from_property_trees_;
    }
    void set_clip_rect_in_target_space_from_property_trees(
        const gfx::Rect& rect)
    {
        clip_rect_in_target_space_from_property_trees_ = rect;
    }

    void set_should_flatten_transform_from_property_tree(bool should_flatten)
    {
        should_flatten_transform_from_property_tree_ = should_flatten;
        SetNeedsPushProperties();
    }
    bool should_flatten_transform_from_property_tree() const
    {
        return should_flatten_transform_from_property_tree_;
    }

    bool is_clipped() const { return draw_properties_.is_clipped; }

    void UpdatePropertyTreeTransform();
    void UpdatePropertyTreeTransformIsAnimated(bool is_animated);
    void UpdatePropertyTreeOpacity();
    void UpdatePropertyTreeScrollOffset();

    // For compatibility with Layer.
    bool has_render_surface() const { return !!render_surface(); }

    void SetNumDescendantsThatDrawContent(int num_descendants);
    void SetClipParent(LayerImpl* ancestor);

    LayerImpl* clip_parent()
    {
        return clip_parent_;
    }
    const LayerImpl* clip_parent() const
    {
        return clip_parent_;
    }

    void SetClipChildren(std::set<LayerImpl*>* children);

    std::set<LayerImpl*>* clip_children() { return clip_children_.get(); }
    const std::set<LayerImpl*>* clip_children() const
    {
        return clip_children_.get();
    }

    void PassCopyRequests(ScopedPtrVector<CopyOutputRequest>* requests);
    // Can only be called when the layer has a copy request.
    void TakeCopyRequestsAndTransformToTarget(
        ScopedPtrVector<CopyOutputRequest>* request);
    bool HasCopyRequest() const { return !copy_requests_.empty(); }

    void SetMaskLayer(scoped_ptr<LayerImpl> mask_layer);
    LayerImpl* mask_layer() { return mask_layer_.get(); }
    const LayerImpl* mask_layer() const { return mask_layer_.get(); }
    scoped_ptr<LayerImpl> TakeMaskLayer();

    void SetReplicaLayer(scoped_ptr<LayerImpl> replica_layer);
    LayerImpl* replica_layer() { return replica_layer_.get(); }
    const LayerImpl* replica_layer() const { return replica_layer_.get(); }
    scoped_ptr<LayerImpl> TakeReplicaLayer();

    bool has_mask() const { return mask_layer_; }
    bool has_replica() const { return replica_layer_; }
    bool replica_has_mask() const
    {
        return replica_layer_ && (mask_layer_ || replica_layer_->mask_layer_);
    }

    LayerTreeImpl* layer_tree_impl() const { return layer_tree_impl_; }

    void PopulateSharedQuadState(SharedQuadState* state) const;
    void PopulateScaledSharedQuadState(SharedQuadState* state, float scale) const;
    // WillDraw must be called before AppendQuads. If WillDraw returns false,
    // AppendQuads and DidDraw will not be called. If WillDraw returns true,
    // DidDraw is guaranteed to be called before another WillDraw or before
    // the layer is destroyed. To enforce this, any class that overrides
    // WillDraw/DidDraw must call the base class version only if WillDraw
    // returns true.
    virtual bool WillDraw(DrawMode draw_mode,
        ResourceProvider* resource_provider);
    virtual void AppendQuads(RenderPass* render_pass,
        AppendQuadsData* append_quads_data) { }
    virtual void DidDraw(ResourceProvider* resource_provider);

    // Verify that the resource ids in the quad are valid.
    void ValidateQuadResources(DrawQuad* quad) const
    {
#if DCHECK_IS_ON()
        ValidateQuadResourcesInternal(quad);
#endif
    }

    virtual void GetContentsResourceId(ResourceId* resource_id,
        gfx::Size* resource_size) const;

    virtual bool HasDelegatedContent() const;
    virtual bool HasContributingDelegatedRenderPasses() const;
    virtual RenderPassId FirstContributingRenderPassId() const;
    virtual RenderPassId NextContributingRenderPassId(RenderPassId id) const;

    virtual void NotifyTileStateChanged(const Tile* tile) { }

    virtual ScrollbarLayerImplBase* ToScrollbarLayer();

    // Returns true if this layer has content to draw.
    void SetDrawsContent(bool draws_content);
    bool DrawsContent() const { return draws_content_; }

    int NumDescendantsThatDrawContent() const;
    void SetHideLayerAndSubtree(bool hide);
    bool hide_layer_and_subtree() const { return hide_layer_and_subtree_; }

    void SetTransformOrigin(const gfx::Point3F& transform_origin);
    gfx::Point3F transform_origin() const { return transform_origin_; }

    void SetBackgroundColor(SkColor background_color);
    SkColor background_color() const { return background_color_; }
    // If contents_opaque(), return an opaque color else return a
    // non-opaque color.  Tries to return background_color(), if possible.
    SkColor SafeOpaqueBackgroundColor() const;

    void SetFilters(const FilterOperations& filters);
    const FilterOperations& filters() const { return filters_; }
    bool FilterIsAnimating() const;
    bool HasPotentiallyRunningFilterAnimation() const;
    bool FilterIsAnimatingOnImplOnly() const;

    void SetBackgroundFilters(const FilterOperations& filters);
    const FilterOperations& background_filters() const
    {
        return background_filters_;
    }

    void SetMasksToBounds(bool masks_to_bounds);
    bool masks_to_bounds() const { return masks_to_bounds_; }

    void SetContentsOpaque(bool opaque);
    bool contents_opaque() const { return contents_opaque_; }

    void SetOpacity(float opacity);
    float opacity() const { return opacity_; }
    bool OpacityIsAnimating() const;
    bool HasPotentiallyRunningOpacityAnimation() const;
    bool OpacityIsAnimatingOnImplOnly() const;

    void SetBlendMode(SkXfermode::Mode);
    SkXfermode::Mode blend_mode() const { return blend_mode_; }
    void set_draw_blend_mode(SkXfermode::Mode blend_mode)
    {
        if (draw_blend_mode_ == blend_mode)
            return;
        draw_blend_mode_ = blend_mode;
        SetNeedsPushProperties();
    }
    SkXfermode::Mode draw_blend_mode() const { return draw_blend_mode_; }
    bool uses_default_blend_mode() const
    {
        return blend_mode_ == SkXfermode::kSrcOver_Mode;
    }

    void SetIsRootForIsolatedGroup(bool root);
    bool is_root_for_isolated_group() const
    {
        return is_root_for_isolated_group_;
    }

    void SetPosition(const gfx::PointF& position);
    gfx::PointF position() const { return position_; }

    void SetIsContainerForFixedPositionLayers(bool container)
    {
        is_container_for_fixed_position_layers_ = container;
    }
    // This is a non-trivial function in Layer.
    bool IsContainerForFixedPositionLayers() const
    {
        return is_container_for_fixed_position_layers_;
    }

    bool IsAffectedByPageScale() const { return is_affected_by_page_scale_; }
    void SetIsAffectedByPageScale(bool is_affected)
    {
        is_affected_by_page_scale_ = is_affected;
    }

    gfx::Vector2dF FixedContainerSizeDelta() const;

    void SetPositionConstraint(const LayerPositionConstraint& constraint)
    {
        position_constraint_ = constraint;
    }
    const LayerPositionConstraint& position_constraint() const
    {
        return position_constraint_;
    }

    void SetShouldFlattenTransform(bool flatten);
    bool should_flatten_transform() const { return should_flatten_transform_; }

    bool Is3dSorted() const { return sorting_context_id_ != 0; }

    void SetUseParentBackfaceVisibility(bool use)
    {
        use_parent_backface_visibility_ = use;
    }
    bool use_parent_backface_visibility() const
    {
        return use_parent_backface_visibility_;
    }

    bool ShowDebugBorders() const;

    // These invalidate the host's render surface layer list.  The caller
    // is responsible for calling set_needs_update_draw_properties on the tree
    // so that its list can be recreated.
    void ClearRenderSurfaceLayerList();
    void SetHasRenderSurface(bool has_render_surface);

    RenderSurfaceImpl* render_surface() const { return render_surface_.get(); }

    DrawProperties& draw_properties() { return draw_properties_; }
    const DrawProperties& draw_properties() const { return draw_properties_; }

    // The following are shortcut accessors to get various information from
    // draw_properties_
    const gfx::Transform& draw_transform() const
    {
        return draw_properties_.target_space_transform;
    }
    const gfx::Transform& screen_space_transform() const
    {
        return draw_properties_.screen_space_transform;
    }
    float draw_opacity() const { return draw_properties_.opacity; }
    bool screen_space_transform_is_animating() const
    {
        return draw_properties_.screen_space_transform_is_animating;
    }
    bool can_use_lcd_text() const { return draw_properties_.can_use_lcd_text; }
    gfx::Rect clip_rect() const { return draw_properties_.clip_rect; }
    gfx::Rect drawable_content_rect() const
    {
        return draw_properties_.drawable_content_rect;
    }
    gfx::Rect visible_layer_rect() const
    {
        return draw_properties_.visible_layer_rect;
    }
    LayerImpl* render_target()
    {
        DCHECK(!draw_properties_.render_target || draw_properties_.render_target->render_surface());
        return draw_properties_.render_target;
    }
    const LayerImpl* render_target() const
    {
        DCHECK(!draw_properties_.render_target || draw_properties_.render_target->render_surface());
        return draw_properties_.render_target;
    }

    size_t num_unclipped_descendants() const
    {
        return draw_properties_.num_unclipped_descendants;
    }

    // The client should be responsible for setting bounds, content bounds and
    // contents scale to appropriate values. LayerImpl doesn't calculate any of
    // them from the other values.

    void SetBounds(const gfx::Size& bounds);
    gfx::Size bounds() const;
    // Like bounds() but doesn't snap to int. Lossy on giant pages (e.g. millions
    // of pixels) due to use of single precision float.
    gfx::SizeF BoundsForScrolling() const;
    void SetBoundsDelta(const gfx::Vector2dF& bounds_delta);
    gfx::Vector2dF bounds_delta() const { return bounds_delta_; }

    void SetCurrentScrollOffset(const gfx::ScrollOffset& scroll_offset);
    void PushScrollOffsetFromMainThread(const gfx::ScrollOffset& scroll_offset);
    // This method is similar to PushScrollOffsetFromMainThread but will cause the
    // scroll offset given to clobber any scroll changes on the active tree in the
    // time until this value is pushed to the active tree.
    void PushScrollOffsetFromMainThreadAndClobberActiveValue(
        const gfx::ScrollOffset& scroll_offset);
    gfx::ScrollOffset PullDeltaForMainThread();
    gfx::ScrollOffset CurrentScrollOffset() const;
    gfx::ScrollOffset BaseScrollOffset() const;
    gfx::Vector2dF ScrollDelta() const;
    void SetScrollDelta(const gfx::Vector2dF& delta);

    gfx::ScrollOffset MaxScrollOffset() const;
    gfx::ScrollOffset ClampScrollOffsetToLimits(gfx::ScrollOffset offset) const;
    gfx::Vector2dF ClampScrollToMaxScrollOffset();
    void SetScrollbarPosition(ScrollbarLayerImplBase* scrollbar_layer,
        LayerImpl* scrollbar_clip_layer,
        bool on_resize) const;
    void SetScrollCompensationAdjustment(const gfx::Vector2dF& scroll_offset)
    {
        scroll_compensation_adjustment_ = scroll_offset;
    }
    gfx::Vector2dF ScrollCompensationAdjustment() const
    {
        return scroll_compensation_adjustment_;
    }

    // Returns the delta of the scroll that was outside of the bounds of the
    // initial scroll
    gfx::Vector2dF ScrollBy(const gfx::Vector2dF& scroll);

    void SetScrollClipLayer(int scroll_clip_layer_id);
    LayerImpl* scroll_clip_layer() const { return scroll_clip_layer_; }
    bool scrollable() const { return !!scroll_clip_layer_; }

    void set_user_scrollable_horizontal(bool scrollable)
    {
        user_scrollable_horizontal_ = scrollable;
    }
    bool user_scrollable_horizontal() const
    {
        return user_scrollable_horizontal_;
    }
    void set_user_scrollable_vertical(bool scrollable)
    {
        user_scrollable_vertical_ = scrollable;
    }
    bool user_scrollable_vertical() const { return user_scrollable_vertical_; }

    bool user_scrollable(ScrollbarOrientation orientation) const;

    void ApplySentScrollDeltasFromAbortedCommit();

    void SetShouldScrollOnMainThread(bool should_scroll_on_main_thread)
    {
        should_scroll_on_main_thread_ = should_scroll_on_main_thread;
    }
    bool should_scroll_on_main_thread() const
    {
        return should_scroll_on_main_thread_;
    }

    void SetHaveWheelEventHandlers(bool have_wheel_event_handlers)
    {
        have_wheel_event_handlers_ = have_wheel_event_handlers;
    }
    bool have_wheel_event_handlers() const { return have_wheel_event_handlers_; }

    void SetHaveScrollEventHandlers(bool have_scroll_event_handlers)
    {
        have_scroll_event_handlers_ = have_scroll_event_handlers;
    }
    bool have_scroll_event_handlers() const
    {
        return have_scroll_event_handlers_;
    }

    void SetNonFastScrollableRegion(const Region& region)
    {
        non_fast_scrollable_region_ = region;
    }
    const Region& non_fast_scrollable_region() const
    {
        return non_fast_scrollable_region_;
    }

    void SetTouchEventHandlerRegion(const Region& region)
    {
        touch_event_handler_region_ = region;
    }
    const Region& touch_event_handler_region() const
    {
        return touch_event_handler_region_;
    }

    void SetScrollBlocksOn(ScrollBlocksOn scroll_blocks_on)
    {
        scroll_blocks_on_ = scroll_blocks_on;
    }
    ScrollBlocksOn scroll_blocks_on() const { return scroll_blocks_on_; }

    InputHandler::ScrollStatus TryScroll(
        const gfx::PointF& screen_space_point,
        InputHandler::ScrollInputType type,
        ScrollBlocksOn effective_block_mode) const;

    void SetDoubleSided(bool double_sided);
    bool double_sided() const { return double_sided_; }

    void SetTransform(const gfx::Transform& transform);
    const gfx::Transform& transform() const { return transform_; }
    bool TransformIsAnimating() const;
    bool HasPotentiallyRunningTransformAnimation() const;
    bool TransformIsAnimatingOnImplOnly() const;
    bool HasOnlyTranslationTransforms() const;
    void SetTransformAndInvertibility(const gfx::Transform& transform,
        bool transform_is_invertible);
    bool transform_is_invertible() const { return transform_is_invertible_; }

    bool MaximumTargetScale(float* max_scale) const;
    bool AnimationStartScale(float* start_scale) const;

    // This includes all animations, even those that are finished but haven't yet
    // been deleted.
    bool HasAnyAnimationTargetingProperty(
        Animation::TargetProperty property) const;

    bool HasFilterAnimationThatInflatesBounds() const;
    bool HasTransformAnimationThatInflatesBounds() const;
    bool HasAnimationThatInflatesBounds() const;

    bool FilterAnimationBoundsForBox(const gfx::BoxF& box,
        gfx::BoxF* bounds) const;
    bool TransformAnimationBoundsForBox(const gfx::BoxF& box,
        gfx::BoxF* bounds) const;

    // Note this rect is in layer space (not content space).
    void SetUpdateRect(const gfx::Rect& update_rect);
    const gfx::Rect& update_rect() const { return update_rect_; }

    void AddDamageRect(const gfx::Rect& damage_rect);
    const gfx::Rect& damage_rect() const { return damage_rect_; }

    virtual base::DictionaryValue* LayerTreeAsJson() const;

    void SetStackingOrderChanged(bool stacking_order_changed);

    bool LayerPropertyChanged() const { return layer_property_changed_; }

    void ResetAllChangeTrackingForSubtree();

    LayerAnimationController* layer_animation_controller()
    {
        return layer_animation_controller_.get();
    }

    const LayerAnimationController* layer_animation_controller() const
    {
        return layer_animation_controller_.get();
    }

    virtual SimpleEnclosedRegion VisibleOpaqueRegion() const;

    virtual void DidBecomeActive();

    virtual void DidBeginTracing();

    // Release resources held by this layer. Called when the output surface
    // that rendered this layer was lost or a rendering mode switch has occured.
    virtual void ReleaseResources();

    // Recreate resources that are required after they were released by a
    // ReleaseResources call.
    virtual void RecreateResources();

    ScrollbarAnimationController* scrollbar_animation_controller() const
    {
        return scrollbar_animation_controller_.get();
    }

    typedef std::set<ScrollbarLayerImplBase*> ScrollbarSet;
    ScrollbarSet* scrollbars() { return scrollbars_.get(); }
    void ClearScrollbars();
    void AddScrollbar(ScrollbarLayerImplBase* layer);
    void RemoveScrollbar(ScrollbarLayerImplBase* layer);
    bool HasScrollbar(ScrollbarOrientation orientation) const;
    void ScrollbarParametersDidChange(bool on_resize);
    int clip_height()
    {
        return scroll_clip_layer_ ? scroll_clip_layer_->bounds().height() : 0;
    }

    virtual skia::RefPtr<SkPicture> GetPicture();

    virtual scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl);
    virtual void PushPropertiesTo(LayerImpl* layer);

    virtual void GetAllPrioritizedTilesForTracing(
        std::vector<PrioritizedTile>* prioritized_tiles) const;
    virtual void AsValueInto(base::trace_event::TracedValue* dict) const;

    virtual size_t GPUMemoryUsageInBytes() const;

    void SetNeedsPushProperties();
    void AddDependentNeedsPushProperties();
    void RemoveDependentNeedsPushProperties();
    bool parent_should_know_need_push_properties() const
    {
        return needs_push_properties() || descendant_needs_push_properties();
    }

    bool needs_push_properties() const { return needs_push_properties_; }
    bool descendant_needs_push_properties() const
    {
        return num_dependents_need_push_properties_ > 0;
    }

    virtual void RunMicroBenchmark(MicroBenchmarkImpl* benchmark);

    virtual void SetDebugInfo(
        scoped_refptr<base::trace_event::ConvertableToTraceFormat> other);

    bool IsDrawnRenderSurfaceLayerListMember() const;

    void Set3dSortingContextId(int id);
    int sorting_context_id() { return sorting_context_id_; }

    void SetFrameTimingRequests(
        const std::vector<FrameTimingRequest>& frame_timing_requests);
    const std::vector<FrameTimingRequest>& frame_timing_requests() const
    {
        return frame_timing_requests_;
    }
    void GatherFrameTimingRequestIds(std::vector<int64_t>* request_ids);

    SyncedScrollOffset* synced_scroll_offset() { return scroll_offset_.get(); }

    // Get the correct invalidation region instead of conservative Rect
    // for layers that provide it.
    virtual Region GetInvalidationRegion();

    virtual gfx::Rect GetEnclosingRectInTargetSpace() const;

    void set_visited(bool visited) { visited_ = visited; }

    bool visited() { return visited_; }

    void set_layer_or_descendant_is_drawn(bool layer_or_descendant_is_drawn)
    {
        layer_or_descendant_is_drawn_ = layer_or_descendant_is_drawn;
    }

    bool layer_or_descendant_is_drawn() { return layer_or_descendant_is_drawn_; }

    void set_layer_or_descendant_has_input_handler(
        bool layer_or_descendant_has_input_handler)
    {
        layer_or_descendant_has_input_handler_ = layer_or_descendant_has_input_handler;
    }

    bool layer_or_descendant_has_input_handler()
    {
        return layer_or_descendant_has_input_handler_;
    }

    void set_sorted_for_recursion(bool sorted_for_recursion)
    {
        sorted_for_recursion_ = sorted_for_recursion;
    }
    bool sorted_for_recursion() { return sorted_for_recursion_; }

    void set_num_layer_or_descendant_with_copy_request(
        int num_layer_or_descendants_with_copy_request)
    {
        num_layer_or_descendants_with_copy_request_ = num_layer_or_descendants_with_copy_request;
    }
    int num_layer_or_descendants_with_copy_request()
    {
        return num_layer_or_descendants_with_copy_request_;
    }

    void UpdatePropertyTreeForScrollingAndAnimationIfNeeded();

    float GetIdealContentsScale() const;

protected:
    LayerImpl(LayerTreeImpl* layer_impl,
        int id,
        scoped_refptr<SyncedScrollOffset> scroll_offset);
    LayerImpl(LayerTreeImpl* layer_impl, int id);

    // Get the color and size of the layer's debug border.
    virtual void GetDebugBorderProperties(SkColor* color, float* width) const;

    void AppendDebugBorderQuad(RenderPass* render_pass,
        const gfx::Size& bounds,
        const SharedQuadState* shared_quad_state,
        AppendQuadsData* append_quads_data) const;
    void AppendDebugBorderQuad(RenderPass* render_pass,
        const gfx::Size& bounds,
        const SharedQuadState* shared_quad_state,
        AppendQuadsData* append_quads_data,
        SkColor color,
        float width) const;

    void NoteLayerPropertyChanged();
    void NoteLayerPropertyChangedForSubtree();

    // Note carefully this does not affect the current layer.
    void NoteLayerPropertyChangedForDescendants();

    gfx::Rect GetScaledEnclosingRectInTargetSpace(float scale) const;

private:
    void ValidateQuadResourcesInternal(DrawQuad* quad) const;

    void PushScrollOffset(const gfx::ScrollOffset* scroll_offset);
    void DidUpdateScrollOffset();
    void NoteLayerPropertyChangedForDescendantsInternal();

    virtual const char* LayerTypeAsString() const;

    // Properties internal to LayerImpl
    LayerImpl* parent_;
    OwnedLayerImplList children_;

    LayerImpl* scroll_parent_;

    // Storing a pointer to a set rather than a set since this will be rarely
    // used. If this pointer turns out to be too heavy, we could have this (and
    // the scroll parent above) be stored in a LayerImpl -> scroll_info
    // map somewhere.
    scoped_ptr<std::set<LayerImpl*>> scroll_children_;

    LayerImpl* clip_parent_;
    scoped_ptr<std::set<LayerImpl*>> clip_children_;

    // mask_layer_ can be temporarily stolen during tree sync, we need this ID to
    // confirm newly assigned layer is still the previous one
    int mask_layer_id_;
    scoped_ptr<LayerImpl> mask_layer_;
    int replica_layer_id_; // ditto
    scoped_ptr<LayerImpl> replica_layer_;
    int layer_id_;
    LayerTreeImpl* layer_tree_impl_;

    // Properties dynamically changeable on active tree.
    scoped_refptr<SyncedScrollOffset> scroll_offset_;
    gfx::Vector2dF bounds_delta_;

    // Properties synchronized from the associated Layer.
    gfx::Point3F transform_origin_;
    gfx::Size bounds_;
    LayerImpl* scroll_clip_layer_;

    gfx::Vector2dF offset_to_transform_parent_;

    bool should_scroll_on_main_thread_ : 1;
    bool have_wheel_event_handlers_ : 1;
    bool have_scroll_event_handlers_ : 1;

    static_assert(SCROLL_BLOCKS_ON_MAX < (1 << 3), "ScrollBlocksOn too big");
    ScrollBlocksOn scroll_blocks_on_ : 3;

    bool user_scrollable_horizontal_ : 1;
    bool user_scrollable_vertical_ : 1;
    bool stacking_order_changed_ : 1;
    // Whether the "back" of this layer should draw.
    bool double_sided_ : 1;
    bool should_flatten_transform_ : 1;
    bool should_flatten_transform_from_property_tree_ : 1;

    // Tracks if drawing-related properties have changed since last redraw.
    bool layer_property_changed_ : 1;

    bool masks_to_bounds_ : 1;
    bool contents_opaque_ : 1;
    bool is_root_for_isolated_group_ : 1;
    bool use_parent_backface_visibility_ : 1;
    bool draws_content_ : 1;
    bool hide_layer_and_subtree_ : 1;

    // Cache transform_'s invertibility.
    bool transform_is_invertible_ : 1;

    // Set for the layer that other layers are fixed to.
    bool is_container_for_fixed_position_layers_ : 1;

    bool is_affected_by_page_scale_ : 1;

    Region non_fast_scrollable_region_;
    Region touch_event_handler_region_;
    SkColor background_color_;

    float opacity_;
    SkXfermode::Mode blend_mode_;
    // draw_blend_mode may be different than blend_mode_,
    // when a RenderSurface re-parents the layer's blend_mode.
    SkXfermode::Mode draw_blend_mode_;
    gfx::PointF position_;
    gfx::Transform transform_;

    LayerPositionConstraint position_constraint_;

    gfx::Vector2dF scroll_compensation_adjustment_;

    int num_descendants_that_draw_content_;

    gfx::Rect visible_rect_from_property_trees_;
    gfx::Rect clip_rect_in_target_space_from_property_trees_;
    int transform_tree_index_;
    int effect_tree_index_;
    int clip_tree_index_;

    // The global depth value of the center of the layer. This value is used
    // to sort layers from back to front.
    float draw_depth_;

    FilterOperations filters_;
    FilterOperations background_filters_;

protected:
    friend class TreeSynchronizer;

    // This flag is set when the layer needs to push properties to the active
    // side.
    bool needs_push_properties_;

    // The number of direct children or dependent layers that need to be recursed
    // to in order for them or a descendent of them to push properties to the
    // active side.
    int num_dependents_need_push_properties_;

    // Layers that share a sorting context id will be sorted together in 3d
    // space.  0 is a special value that means this layer will not be sorted and
    // will be drawn in paint order.
    int sorting_context_id_;

    DrawMode current_draw_mode_;

private:
    // Rect indicating what was repainted/updated during update.
    // Note that plugin layers bypass this and leave it empty.
    // This is in the layer's space.
    gfx::Rect update_rect_;

    // Denotes an area that is damaged and needs redraw. This is in the layer's
    // space.
    gfx::Rect damage_rect_;

    // Manages animations for this layer.
    scoped_refptr<LayerAnimationController> layer_animation_controller_;

    // Manages scrollbars for this layer
    scoped_ptr<ScrollbarAnimationController> scrollbar_animation_controller_;

    scoped_ptr<ScrollbarSet> scrollbars_;

    ScopedPtrVector<CopyOutputRequest> copy_requests_;

    // Group of properties that need to be computed based on the layer tree
    // hierarchy before layers can be drawn.
    DrawProperties draw_properties_;

    scoped_refptr<base::trace_event::ConvertableToTraceFormat> debug_info_;
    scoped_ptr<RenderSurfaceImpl> render_surface_;

    std::vector<FrameTimingRequest> frame_timing_requests_;
    int num_layer_or_descendants_with_copy_request_;
    bool frame_timing_requests_dirty_;
    bool visited_;
    bool layer_or_descendant_is_drawn_;
    // If true, the layer or one of its descendants has a wheel or touch handler.
    bool layer_or_descendant_has_input_handler_;
    bool sorted_for_recursion_;

    DISALLOW_COPY_AND_ASSIGN(LayerImpl);
};

} // namespace cc

#endif // CC_LAYERS_LAYER_IMPL_H_
