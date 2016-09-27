// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_LAYER_H_
#define CC_LAYERS_LAYER_H_

#include <set>
#include <string>
#include <vector>

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/observer_list.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/animation/layer_animation_value_observer.h"
#include "cc/animation/layer_animation_value_provider.h"
#include "cc/base/cc_export.h"
#include "cc/base/region.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/debug/frame_timing_request.h"
#include "cc/debug/micro_benchmark.h"
#include "cc/layers/layer_lists.h"
#include "cc/layers/layer_position_constraint.h"
#include "cc/layers/paint_properties.h"
#include "cc/layers/scroll_blocks_on.h"
#include "cc/output/filter_operations.h"
#include "cc/trees/property_tree.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkImageFilter.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkXfermode.h"
#include "ui/gfx/geometry/point3_f.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/scroll_offset.h"
#include "ui/gfx/transform.h"

namespace gfx {
class BoxF;
}

namespace base {
namespace trace_event {
class ConvertableToTraceFormat;
}
}

namespace cc {

class Animation;
class AnimationDelegate;
struct AnimationEvent;
class CopyOutputRequest;
class LayerAnimationEventObserver;
class LayerClient;
class LayerImpl;
class LayerSettings;
class LayerTreeHost;
class LayerTreeHostCommon;
class LayerTreeImpl;
class LayerTreeSettings;
class RenderingStatsInstrumentation;
class ResourceUpdateQueue;
class ScrollbarLayerInterface;
class SimpleEnclosedRegion;
struct AnimationEvent;

// Base class for composited layers. Special layer types are derived from
// this class.
class CC_EXPORT Layer : public base::RefCounted<Layer>,
                        public LayerAnimationValueObserver,
                        public LayerAnimationValueProvider {
 public:
  typedef LayerList LayerListType;

  enum LayerIdLabels {
    INVALID_ID = -1,
  };

  static scoped_refptr<Layer> Create(const LayerSettings& settings);

  int id() const { return layer_id_; }

  Layer* RootLayer();
  Layer* parent() { return parent_; }
  const Layer* parent() const { return parent_; }
  void AddChild(scoped_refptr<Layer> child);
  void InsertChild(scoped_refptr<Layer> child, size_t index);
  void ReplaceChild(Layer* reference, scoped_refptr<Layer> new_layer);
  void RemoveFromParent();
  void RemoveAllChildren();
  void SetChildren(const LayerList& children);
  bool HasAncestor(const Layer* ancestor) const;

  const LayerList& children() const { return children_; }
  Layer* child_at(size_t index) { return children_[index].get(); }

  // This requests the layer and its subtree be rendered and given to the
  // callback. If the copy is unable to be produced (the layer is destroyed
  // first), then the callback is called with a nullptr/empty result. If the
  // request's source property is set, any prior uncommitted requests having the
  // same source will be aborted.
  void RequestCopyOfOutput(scoped_ptr<CopyOutputRequest> request);
  bool HasCopyRequest() const {
    return !copy_requests_.empty();
  }

  virtual void SetBackgroundColor(SkColor background_color);
  SkColor background_color() const { return background_color_; }
  // If contents_opaque(), return an opaque color else return a
  // non-opaque color.  Tries to return background_color(), if possible.
  SkColor SafeOpaqueBackgroundColor() const;

  // A layer's bounds are in logical, non-page-scaled pixels (however, the
  // root layer's bounds are in physical pixels).
  void SetBounds(const gfx::Size& bounds);
  gfx::Size bounds() const { return bounds_; }

  void SetMasksToBounds(bool masks_to_bounds);
  bool masks_to_bounds() const { return masks_to_bounds_; }

  void SetMaskLayer(Layer* mask_layer);
  Layer* mask_layer() { return mask_layer_.get(); }
  const Layer* mask_layer() const { return mask_layer_.get(); }

  virtual void SetNeedsDisplayRect(const gfx::Rect& dirty_rect);
  void SetNeedsDisplay() { SetNeedsDisplayRect(gfx::Rect(bounds())); }

  void SetOpacity(float opacity);
  float opacity() const { return opacity_; }
  bool OpacityIsAnimating() const;
  bool HasPotentiallyRunningOpacityAnimation() const;
  virtual bool OpacityCanAnimateOnImplThread() const;

  void SetBlendMode(SkXfermode::Mode blend_mode);
  SkXfermode::Mode blend_mode() const { return blend_mode_; }

  void set_draw_blend_mode(SkXfermode::Mode blend_mode) {
    if (draw_blend_mode_ == blend_mode)
      return;
    draw_blend_mode_ = blend_mode;
    SetNeedsPushProperties();
  }
  SkXfermode::Mode draw_blend_mode() const { return draw_blend_mode_; }

  bool uses_default_blend_mode() const {
    return blend_mode_ == SkXfermode::kSrcOver_Mode;
  }

  // A layer is root for an isolated group when it and all its descendants are
  // drawn over a black and fully transparent background, creating an isolated
  // group. It should be used along with SetBlendMode(), in order to restrict
  // layers within the group to blend with layers outside this group.
  void SetIsRootForIsolatedGroup(bool root);
  bool is_root_for_isolated_group() const {
    return is_root_for_isolated_group_;
  }

  void SetFilters(const FilterOperations& filters);
  const FilterOperations& filters() const { return filters_; }
  bool FilterIsAnimating() const;
  bool HasPotentiallyRunningFilterAnimation() const;

  // Background filters are filters applied to what is behind this layer, when
  // they are viewed through non-opaque regions in this layer. They are used
  // through the WebLayer interface, and are not exposed to HTML.
  void SetBackgroundFilters(const FilterOperations& filters);
  const FilterOperations& background_filters() const {
    return background_filters_;
  }

  virtual void SetContentsOpaque(bool opaque);
  bool contents_opaque() const { return contents_opaque_; }

  void SetPosition(const gfx::PointF& position);
  gfx::PointF position() const { return position_; }

  // A layer that is a container for fixed position layers cannot be both
  // scrollable and have a non-identity transform.
  void SetIsContainerForFixedPositionLayers(bool container);
  bool IsContainerForFixedPositionLayers() const;

  gfx::Vector2dF FixedContainerSizeDelta() const {
    return gfx::Vector2dF();
  }

  void SetPositionConstraint(const LayerPositionConstraint& constraint);
  const LayerPositionConstraint& position_constraint() const {
    return position_constraint_;
  }

  void SetTransform(const gfx::Transform& transform);
  const gfx::Transform& transform() const { return transform_; }
  bool TransformIsAnimating() const;
  bool HasPotentiallyRunningTransformAnimation() const;
  bool HasOnlyTranslationTransforms() const;
  bool AnimationsPreserveAxisAlignment() const;
  bool transform_is_invertible() const { return transform_is_invertible_; }

  bool MaximumTargetScale(float* max_scale) const;
  bool AnimationStartScale(float* start_scale) const;

  void SetTransformOrigin(const gfx::Point3F&);
  gfx::Point3F transform_origin() const { return transform_origin_; }

  bool HasAnyAnimationTargetingProperty(
      Animation::TargetProperty property) const;

  bool ScrollOffsetAnimationWasInterrupted() const;

  void SetScrollParent(Layer* parent);

  Layer* scroll_parent() { return scroll_parent_; }
  const Layer* scroll_parent() const { return scroll_parent_; }

  void AddScrollChild(Layer* child);
  void RemoveScrollChild(Layer* child);

  std::set<Layer*>* scroll_children() { return scroll_children_.get(); }
  const std::set<Layer*>* scroll_children() const {
    return scroll_children_.get();
  }

  void SetClipParent(Layer* ancestor);

  Layer* clip_parent() { return clip_parent_; }
  const Layer* clip_parent() const {
    return clip_parent_;
  }

  void AddClipChild(Layer* child);
  void RemoveClipChild(Layer* child);

  std::set<Layer*>* clip_children() { return clip_children_.get(); }
  const std::set<Layer*>* clip_children() const {
    return clip_children_.get();
  }

  // TODO(enne): Fix style here (and everywhere) once LayerImpl does the same.
  gfx::Transform draw_transform() const;
  gfx::Transform screen_space_transform() const;

  void set_num_unclipped_descendants(size_t descendants) {
    num_unclipped_descendants_ = descendants;
  }
  size_t num_unclipped_descendants() const {
    return num_unclipped_descendants_;
  }

  void SetScrollOffset(const gfx::ScrollOffset& scroll_offset);
  void SetScrollCompensationAdjustment(
      const gfx::Vector2dF& scroll_compensation_adjustment);
  gfx::Vector2dF ScrollCompensationAdjustment() const;

  gfx::ScrollOffset scroll_offset() const { return scroll_offset_; }
  void SetScrollOffsetFromImplSide(const gfx::ScrollOffset& scroll_offset);

  void SetScrollClipLayerId(int clip_layer_id);
  bool scrollable() const { return scroll_clip_layer_id_ != INVALID_ID; }

  void SetUserScrollable(bool horizontal, bool vertical);
  bool user_scrollable_horizontal() const {
    return user_scrollable_horizontal_;
  }
  bool user_scrollable_vertical() const { return user_scrollable_vertical_; }

  void SetShouldScrollOnMainThread(bool should_scroll_on_main_thread);
  bool should_scroll_on_main_thread() const {
    return should_scroll_on_main_thread_;
  }

  void SetHaveWheelEventHandlers(bool have_wheel_event_handlers);
  bool have_wheel_event_handlers() const { return have_wheel_event_handlers_; }

  void SetHaveScrollEventHandlers(bool have_scroll_event_handlers);
  bool have_scroll_event_handlers() const {
    return have_scroll_event_handlers_;
  }

  void SetNonFastScrollableRegion(const Region& non_fast_scrollable_region);
  const Region& non_fast_scrollable_region() const {
    return non_fast_scrollable_region_;
  }

  void SetTouchEventHandlerRegion(const Region& touch_event_handler_region);
  const Region& touch_event_handler_region() const {
    return touch_event_handler_region_;
  }

  void SetScrollBlocksOn(ScrollBlocksOn scroll_blocks_on);
  ScrollBlocksOn scroll_blocks_on() const { return scroll_blocks_on_; }

  void set_did_scroll_callback(const base::Closure& callback) {
    did_scroll_callback_ = callback;
  }

  void SetForceRenderSurface(bool force_render_surface);
  bool force_render_surface() const { return force_render_surface_; }

  gfx::Vector2dF ScrollDelta() const { return gfx::Vector2dF(); }
  gfx::ScrollOffset CurrentScrollOffset() const { return scroll_offset_; }

  void SetDoubleSided(bool double_sided);
  bool double_sided() const { return double_sided_; }

  void SetShouldFlattenTransform(bool flatten);
  bool should_flatten_transform() const { return should_flatten_transform_; }

  bool Is3dSorted() const { return sorting_context_id_ != 0; }

  void set_use_parent_backface_visibility(bool use) {
    use_parent_backface_visibility_ = use;
  }
  bool use_parent_backface_visibility() const {
    return use_parent_backface_visibility_;
  }

  virtual void SetLayerTreeHost(LayerTreeHost* host);

  virtual bool HasDelegatedContent() const;
  bool HasContributingDelegatedRenderPasses() const { return false; }

  void SetIsDrawable(bool is_drawable);

  void SetHideLayerAndSubtree(bool hide);
  bool hide_layer_and_subtree() const { return hide_layer_and_subtree_; }

  void SetReplicaLayer(Layer* layer);
  Layer* replica_layer() { return replica_layer_.get(); }
  const Layer* replica_layer() const { return replica_layer_.get(); }

  bool has_mask() const { return !!mask_layer_.get(); }
  bool has_replica() const { return !!replica_layer_.get(); }
  bool replica_has_mask() const {
    return replica_layer_.get() &&
           (mask_layer_.get() || replica_layer_->mask_layer_.get());
  }

  int NumDescendantsThatDrawContent() const;

  // This is only virtual for tests.
  // TODO(awoloszyn): Remove this once we no longer need it for tests
  virtual bool DrawsContent() const;

  // This methods typically need to be overwritten by derived classes.
  virtual void SavePaintProperties();
  // Returns true iff anything was updated that needs to be committed.
  virtual bool Update();
  virtual void SetIsMask(bool is_mask) {}
  virtual bool IsSuitableForGpuRasterization() const;

  virtual scoped_refptr<base::trace_event::ConvertableToTraceFormat>
  TakeDebugInfo();

  void SetLayerClient(LayerClient* client) { client_ = client; }

  virtual void PushPropertiesTo(LayerImpl* layer);

  LayerTreeHost* layer_tree_host() { return layer_tree_host_; }
  const LayerTreeHost* layer_tree_host() const { return layer_tree_host_; }

  bool AddAnimation(scoped_ptr<Animation> animation);
  void PauseAnimation(int animation_id, double time_offset);
  void RemoveAnimation(int animation_id);
  void RemoveAnimation(int animation_id, Animation::TargetProperty property);
  LayerAnimationController* layer_animation_controller() const {
    return layer_animation_controller_.get();
  }
  void SetLayerAnimationControllerForTest(
      scoped_refptr<LayerAnimationController> controller);

  void set_layer_animation_delegate(AnimationDelegate* delegate) {
    DCHECK(layer_animation_controller_);
    layer_animation_controller_->set_layer_animation_delegate(delegate);
  }

  bool HasActiveAnimation() const;
  void RegisterForAnimations(AnimationRegistrar* registrar);

  void AddLayerAnimationEventObserver(
      LayerAnimationEventObserver* animation_observer);
  void RemoveLayerAnimationEventObserver(
      LayerAnimationEventObserver* animation_observer);

  virtual ScrollbarLayerInterface* ToScrollbarLayer();

  virtual skia::RefPtr<SkPicture> GetPicture() const;

  // Constructs a LayerImpl of the correct runtime type for this Layer type.
  virtual scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl);

  bool NeedsDisplayForTesting() const { return !update_rect_.IsEmpty(); }
  void ResetNeedsDisplayForTesting() { update_rect_ = gfx::Rect(); }

  RenderingStatsInstrumentation* rendering_stats_instrumentation() const;

  const PaintProperties& paint_properties() const {
    return paint_properties_;
  }

  void SetNeedsPushProperties();
  bool needs_push_properties() const { return needs_push_properties_; }
  bool descendant_needs_push_properties() const {
    return num_dependents_need_push_properties_ > 0;
  }
  void reset_needs_push_properties_for_testing() {
    needs_push_properties_ = false;
  }

  virtual void RunMicroBenchmark(MicroBenchmark* benchmark);

  void Set3dSortingContextId(int id);
  int sorting_context_id() const { return sorting_context_id_; }

  void set_property_tree_sequence_number(int sequence_number) {
    property_tree_sequence_number_ = sequence_number;
  }

  void SetTransformTreeIndex(int index);
  int transform_tree_index() const;

  void SetClipTreeIndex(int index);
  int clip_tree_index() const;

  void SetEffectTreeIndex(int index);
  int effect_tree_index() const;

  void set_offset_to_transform_parent(gfx::Vector2dF offset) {
    if (offset_to_transform_parent_ == offset)
      return;
    offset_to_transform_parent_ = offset;
    SetNeedsPushProperties();
  }
  gfx::Vector2dF offset_to_transform_parent() const {
    return offset_to_transform_parent_;
  }

  // TODO(enne): Once LayerImpl only uses property trees, remove these
  // functions.
  const gfx::Rect& visible_rect_from_property_trees() const {
    return visible_layer_rect();
  }
  void set_visible_rect_from_property_trees(const gfx::Rect& rect) {
    set_visible_layer_rect(rect);
  }
  const gfx::Rect& clip_rect_in_target_space_from_property_trees() const {
    return clip_rect();
  }
  void set_clip_rect_in_target_space_from_property_trees(
      const gfx::Rect& rect) {
    set_clip_rect(rect);
  }
  // TODO(enne): This needs a different name.  It is a calculated value
  // from the property tree builder and not a synonym for "should
  // flatten transform".
  void set_should_flatten_transform_from_property_tree(bool should_flatten) {
    if (should_flatten_transform_from_property_tree_ == should_flatten)
      return;
    should_flatten_transform_from_property_tree_ = should_flatten;
    SetNeedsPushProperties();
  }
  bool should_flatten_transform_from_property_tree() const {
    return should_flatten_transform_from_property_tree_;
  }

  const gfx::Rect& visible_layer_rect() const { return visible_layer_rect_; }
  void set_visible_layer_rect(const gfx::Rect& rect) {
    visible_layer_rect_ = rect;
  }

  const gfx::Rect& clip_rect() const { return clip_rect_; }
  void set_clip_rect(const gfx::Rect& rect) { clip_rect_ = rect; }

  bool has_render_surface() const {
    return has_render_surface_;
  }

  // Sets new frame timing requests for this layer.
  void SetFrameTimingRequests(const std::vector<FrameTimingRequest>& requests);

  // Accessor for unit tests
  const std::vector<FrameTimingRequest>& FrameTimingRequests() const {
    return frame_timing_requests_;
  }

  void DidBeginTracing();
  // TODO(weiliangc): this should move to the effect tree.
  void set_num_layer_or_descendant_with_copy_request(
      int num_layer_or_descendants_with_copy_request) {
    num_layer_or_descendants_with_copy_request_ =
        num_layer_or_descendants_with_copy_request;
  }
  int num_layer_or_descendants_with_copy_request() {
    return num_layer_or_descendants_with_copy_request_;
  }

  void set_visited(bool visited);
  bool visited();
  void set_layer_or_descendant_is_drawn(bool layer_or_descendant_is_drawn);
  bool layer_or_descendant_is_drawn();
  void set_sorted_for_recursion(bool sorted_for_recursion);
  bool sorted_for_recursion();

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

 protected:
  friend class LayerImpl;
  friend class TreeSynchronizer;
  ~Layer() override;

  explicit Layer(const LayerSettings& settings);

  // These SetNeeds functions are in order of severity of update:
  //
  // Called when this layer has been modified in some way, but isn't sure
  // that it needs a commit yet.  It needs CalcDrawProperties and UpdateLayers
  // before it knows whether or not a commit is required.
  void SetNeedsUpdate();
  // Called when a property has been modified in a way that the layer
  // knows immediately that a commit is required.  This implies SetNeedsUpdate
  // as well as SetNeedsPushProperties to push that property.
  void SetNeedsCommit();
  // This is identical to SetNeedsCommit, but the former requests a rebuild of
  // the property trees.
  void SetNeedsCommitNoRebuild();
  // Called when there's been a change in layer structure.  Implies both
  // SetNeedsUpdate and SetNeedsCommit, but not SetNeedsPushProperties.
  void SetNeedsFullTreeSync();

  // Called when the next commit should wait until the pending tree is activated
  // before finishing the commit and unblocking the main thread. Used to ensure
  // unused resources on the impl thread are returned before commit completes.
  void SetNextCommitWaitsForActivation();

  // Will recalculate whether the layer draws content and set draws_content_
  // appropriately.
  void UpdateDrawsContent(bool has_drawable_content);
  virtual bool HasDrawableContent() const;

  // Called when the layer's number of drawable descendants changes.
  void AddDrawableDescendants(int num);

  void AddDependentNeedsPushProperties();
  void RemoveDependentNeedsPushProperties();
  bool parent_should_know_need_push_properties() const {
    return needs_push_properties() || descendant_needs_push_properties();
  }

  bool IsPropertyChangeAllowed() const;

  // This flag is set when the layer needs to push properties to the impl
  // side.
  bool needs_push_properties_;

  // The number of direct children or dependent layers that need to be recursed
  // to in order for them or a descendent of them to push properties to the impl
  // side.
  int num_dependents_need_push_properties_;

  // Tracks whether this layer may have changed stacking order with its
  // siblings.
  bool stacking_order_changed_;

  // The update rect is the region of the compositor resource that was
  // actually updated by the compositor. For layers that may do updating
  // outside the compositor's control (i.e. plugin layers), this information
  // is not available and the update rect will remain empty.
  // Note this rect is in layer space (not content space).
  gfx::Rect update_rect_;

  scoped_refptr<Layer> mask_layer_;

  int layer_id_;

  // When true, the layer is about to perform an update. Any commit requests
  // will be handled implicitly after the update completes.
  bool ignore_set_needs_commit_;

  // Layers that share a sorting context id will be sorted together in 3d
  // space.  0 is a special value that means this layer will not be sorted and
  // will be drawn in paint order.
  int sorting_context_id_;

 private:
  friend class base::RefCounted<Layer>;
  friend class LayerTreeHostCommon;
  void SetParent(Layer* layer);
  bool DescendantIsFixedToContainerLayer() const;

  // This should only be called during BeginMainFrame since it does not
  // trigger a Commit.
  void SetHasRenderSurface(bool has_render_surface);

  // This should only be called from RemoveFromParent().
  void RemoveChildOrDependent(Layer* child);

  // If this layer has a scroll parent, it removes |this| from its list of
  // scroll children.
  void RemoveFromScrollTree();

  // If this layer has a clip parent, it removes |this| from its list of clip
  // children.
  void RemoveFromClipTree();

  // When we detach or attach layer to new LayerTreeHost, all property trees'
  // indices becomes invalid.
  void InvalidatePropertyTreesIndices();

  void UpdateNumCopyRequestsForSubtree(int delta);

  LayerList children_;
  Layer* parent_;

  // Layer instances have a weak pointer to their LayerTreeHost.
  // This pointer value is nil when a Layer is not in a tree and is
  // updated via SetLayerTreeHost() if a layer moves between trees.
  LayerTreeHost* layer_tree_host_;

  scoped_refptr<LayerAnimationController> layer_animation_controller_;

  // Layer properties.
  gfx::Size bounds_;

  gfx::ScrollOffset scroll_offset_;
  gfx::Vector2dF scroll_compensation_adjustment_;
  // This variable indicates which ancestor layer (if any) whose size,
  // transformed relative to this layer, defines the maximum scroll offset for
  // this layer.
  int scroll_clip_layer_id_;
  int num_descendants_that_draw_content_;
  int transform_tree_index_;
  int effect_tree_index_;
  int clip_tree_index_;
  int property_tree_sequence_number_;
  int num_layer_or_descendants_with_copy_request_;
  gfx::Vector2dF offset_to_transform_parent_;
  bool should_flatten_transform_from_property_tree_ : 1;
  bool should_scroll_on_main_thread_ : 1;
  bool have_wheel_event_handlers_ : 1;
  bool have_scroll_event_handlers_ : 1;
  bool user_scrollable_horizontal_ : 1;
  bool user_scrollable_vertical_ : 1;
  bool is_root_for_isolated_group_ : 1;
  bool is_container_for_fixed_position_layers_ : 1;
  bool is_drawable_ : 1;
  bool draws_content_ : 1;
  bool hide_layer_and_subtree_ : 1;
  bool masks_to_bounds_ : 1;
  bool contents_opaque_ : 1;
  bool double_sided_ : 1;
  bool should_flatten_transform_ : 1;
  bool use_parent_backface_visibility_ : 1;
  bool force_render_surface_ : 1;
  bool transform_is_invertible_ : 1;
  bool has_render_surface_ : 1;
  ScrollBlocksOn scroll_blocks_on_ : 3;
  Region non_fast_scrollable_region_;
  Region touch_event_handler_region_;
  gfx::PointF position_;
  SkColor background_color_;
  float opacity_;
  SkXfermode::Mode blend_mode_;
  // draw_blend_mode may be different than blend_mode_,
  // when a RenderSurface re-parents the layer's blend_mode.
  SkXfermode::Mode draw_blend_mode_;
  FilterOperations filters_;
  FilterOperations background_filters_;
  LayerPositionConstraint position_constraint_;
  Layer* scroll_parent_;
  scoped_ptr<std::set<Layer*>> scroll_children_;

  // The following three variables are tracker variables. They are bools
  // wrapped inside an integer variable. If true, their value equals the
  // LayerTreeHost's meta_information_sequence_number. This wrapping of bools
  // inside ints is done to avoid a layer tree treewalk to reset their values.
  int layer_or_descendant_is_drawn_tracker_;
  int sorted_for_recursion_tracker_;
  int visited_tracker_;

  Layer* clip_parent_;
  scoped_ptr<std::set<Layer*>> clip_children_;

  gfx::Transform transform_;
  gfx::Point3F transform_origin_;

  // Replica layer used for reflections.
  scoped_refptr<Layer> replica_layer_;

  LayerClient* client_;

  ScopedPtrVector<CopyOutputRequest> copy_requests_;

  base::Closure did_scroll_callback_;

  PaintProperties paint_properties_;

  // These all act like draw properties, so don't need push properties.
  gfx::Rect visible_layer_rect_;
  gfx::Rect clip_rect_;
  size_t num_unclipped_descendants_;

  std::vector<FrameTimingRequest> frame_timing_requests_;
  bool frame_timing_requests_dirty_;

  DISALLOW_COPY_AND_ASSIGN(Layer);
};

}  // namespace cc

#endif  // CC_LAYERS_LAYER_H_
