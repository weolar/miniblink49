// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/layer.h"

#include <algorithm>

#include "base/atomic_sequence_num.h"
#include "base/location.h"
#include "base/metrics/histogram.h"
#include "base/single_thread_task_runner.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "cc/animation/animation.h"
#include "cc/animation/animation_events.h"
#include "cc/animation/animation_registrar.h"
#include "cc/animation/keyframed_animation_curve.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/base/simple_enclosed_region.h"
#include "cc/debug/frame_viewer_instrumentation.h"
#include "cc/layers/layer_client.h"
#include "cc/layers/layer_impl.h"
#include "cc/layers/scrollbar_layer_interface.h"
#include "cc/output/copy_output_request.h"
#include "cc/output/copy_output_result.h"
#include "cc/trees/draw_property_utils.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_impl.h"
#include "third_party/skia/include/core/SkImageFilter.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/vector2d_conversions.h"

namespace cc {

base::StaticAtomicSequenceNumber g_next_layer_id;

scoped_refptr<Layer> Layer::Create(const LayerSettings& settings) {
  return make_scoped_refptr(new Layer(settings));
}

Layer::Layer(const LayerSettings& settings)
    : needs_push_properties_(false),
      num_dependents_need_push_properties_(0),
      stacking_order_changed_(false),
      // Layer IDs start from 1.
      layer_id_(g_next_layer_id.GetNext() + 1),
      ignore_set_needs_commit_(false),
      sorting_context_id_(0),
      parent_(nullptr),
      layer_tree_host_(nullptr),
      scroll_clip_layer_id_(INVALID_ID),
      num_descendants_that_draw_content_(0),
      transform_tree_index_(-1),
      effect_tree_index_(-1),
      clip_tree_index_(-1),
      property_tree_sequence_number_(-1),
      num_layer_or_descendants_with_copy_request_(0),
      should_flatten_transform_from_property_tree_(false),
      should_scroll_on_main_thread_(false),
      have_wheel_event_handlers_(false),
      have_scroll_event_handlers_(false),
      user_scrollable_horizontal_(true),
      user_scrollable_vertical_(true),
      is_root_for_isolated_group_(false),
      is_container_for_fixed_position_layers_(false),
      is_drawable_(false),
      draws_content_(false),
      hide_layer_and_subtree_(false),
      masks_to_bounds_(false),
      contents_opaque_(false),
      double_sided_(true),
      should_flatten_transform_(true),
      use_parent_backface_visibility_(false),
      force_render_surface_(false),
      transform_is_invertible_(true),
      has_render_surface_(false),
      scroll_blocks_on_(SCROLL_BLOCKS_ON_NONE),
      background_color_(0),
      opacity_(1.f),
      blend_mode_(SkXfermode::kSrcOver_Mode),
      draw_blend_mode_(SkXfermode::kSrcOver_Mode),
      scroll_parent_(nullptr),
      layer_or_descendant_is_drawn_tracker_(0),
      sorted_for_recursion_tracker_(0),
      visited_tracker_(0),
      clip_parent_(nullptr),
      replica_layer_(nullptr),
      client_(nullptr),
      num_unclipped_descendants_(0),
      frame_timing_requests_dirty_(false) {
  if (!settings.use_compositor_animation_timelines) {
    layer_animation_controller_ = LayerAnimationController::Create(layer_id_);
    layer_animation_controller_->AddValueObserver(this);
    layer_animation_controller_->set_value_provider(this);
  }
}

Layer::~Layer() {
  // Our parent should be holding a reference to us so there should be no
  // way for us to be destroyed while we still have a parent.
  DCHECK(!parent());
  // Similarly we shouldn't have a layer tree host since it also keeps a
  // reference to us.
  DCHECK(!layer_tree_host());

  if (layer_animation_controller_) {
    layer_animation_controller_->RemoveValueObserver(this);
    layer_animation_controller_->remove_value_provider(this);
  }

  RemoveFromScrollTree();
  RemoveFromClipTree();

  // Remove the parent reference from all children and dependents.
  RemoveAllChildren();
  if (mask_layer_.get()) {
    DCHECK_EQ(this, mask_layer_->parent());
    mask_layer_->RemoveFromParent();
  }
  if (replica_layer_.get()) {
    DCHECK_EQ(this, replica_layer_->parent());
    replica_layer_->RemoveFromParent();
  }
}

void Layer::SetLayerTreeHost(LayerTreeHost* host) {
  if (layer_tree_host_ == host)
    return;

  if (layer_tree_host_) {
    layer_tree_host_->property_trees()->needs_rebuild = true;
    layer_tree_host_->UnregisterLayer(this);
  }
  if (host) {
    host->property_trees()->needs_rebuild = true;
    host->RegisterLayer(this);
  }

  InvalidatePropertyTreesIndices();

  layer_tree_host_ = host;

  // When changing hosts, the layer needs to commit its properties to the impl
  // side for the new host.
  SetNeedsPushProperties();

  for (size_t i = 0; i < children_.size(); ++i)
    children_[i]->SetLayerTreeHost(host);

  if (mask_layer_.get())
    mask_layer_->SetLayerTreeHost(host);
  if (replica_layer_.get())
    replica_layer_->SetLayerTreeHost(host);

  if (host)
    RegisterForAnimations(host->animation_registrar());

  bool has_any_animation = false;
  if (layer_animation_controller_)
    has_any_animation = layer_animation_controller_->has_any_animation();
  else if (layer_tree_host_)
    has_any_animation = layer_tree_host_->HasAnyAnimation(this);

  if (host && has_any_animation)
    host->SetNeedsCommit();
}

void Layer::SetNeedsUpdate() {
  if (layer_tree_host_ && !ignore_set_needs_commit_)
    layer_tree_host_->SetNeedsUpdateLayers();
}

void Layer::SetNeedsCommit() {
  if (!layer_tree_host_)
    return;

  SetNeedsPushProperties();
  layer_tree_host_->property_trees()->needs_rebuild = true;

  if (ignore_set_needs_commit_)
    return;

  layer_tree_host_->SetNeedsCommit();
}

void Layer::SetNeedsCommitNoRebuild() {
  if (!layer_tree_host_)
    return;

  SetNeedsPushProperties();

  if (ignore_set_needs_commit_)
    return;

  layer_tree_host_->SetNeedsCommit();
}

void Layer::SetNeedsFullTreeSync() {
  if (!layer_tree_host_)
    return;

  layer_tree_host_->SetNeedsFullTreeSync();
}

void Layer::SetNextCommitWaitsForActivation() {
  if (!layer_tree_host_)
    return;

  layer_tree_host_->SetNextCommitWaitsForActivation();
}

void Layer::SetNeedsPushProperties() {
  if (needs_push_properties_)
    return;
  if (!parent_should_know_need_push_properties() && parent_)
    parent_->AddDependentNeedsPushProperties();
  needs_push_properties_ = true;
}

void Layer::AddDependentNeedsPushProperties() {
  DCHECK_GE(num_dependents_need_push_properties_, 0);

  if (!parent_should_know_need_push_properties() && parent_)
    parent_->AddDependentNeedsPushProperties();

  num_dependents_need_push_properties_++;
}

void Layer::RemoveDependentNeedsPushProperties() {
  num_dependents_need_push_properties_--;
  DCHECK_GE(num_dependents_need_push_properties_, 0);

  if (!parent_should_know_need_push_properties() && parent_)
      parent_->RemoveDependentNeedsPushProperties();
}

bool Layer::IsPropertyChangeAllowed() const {
  if (!layer_tree_host_)
    return true;

  if (!layer_tree_host_->settings().strict_layer_property_change_checking)
    return true;

  return !layer_tree_host_->in_paint_layer_contents();
}

skia::RefPtr<SkPicture> Layer::GetPicture() const {
  return skia::RefPtr<SkPicture>();
}

void Layer::SetParent(Layer* layer) {
  DCHECK(!layer || !layer->HasAncestor(this));

  if (parent_should_know_need_push_properties()) {
    if (parent_)
      parent_->RemoveDependentNeedsPushProperties();
    if (layer)
      layer->AddDependentNeedsPushProperties();
  }

  if (parent_) {
    parent_->UpdateNumCopyRequestsForSubtree(
        -num_layer_or_descendants_with_copy_request_);
  }
  parent_ = layer;
  if (parent_) {
    parent_->UpdateNumCopyRequestsForSubtree(
        num_layer_or_descendants_with_copy_request_);
  }
  SetLayerTreeHost(parent_ ? parent_->layer_tree_host() : nullptr);

  if (!layer_tree_host_)
    return;

  layer_tree_host_->property_trees()->needs_rebuild = true;
}

void Layer::AddChild(scoped_refptr<Layer> child) {
  InsertChild(child, children_.size());
}

void Layer::InsertChild(scoped_refptr<Layer> child, size_t index) {
  DCHECK(IsPropertyChangeAllowed());
  child->RemoveFromParent();
  AddDrawableDescendants(child->NumDescendantsThatDrawContent() +
                         (child->DrawsContent() ? 1 : 0));
  child->SetParent(this);
  child->stacking_order_changed_ = true;

  index = std::min(index, children_.size());
  children_.insert(children_.begin() + index, child);
  SetNeedsFullTreeSync();
}

void Layer::RemoveFromParent() {
  DCHECK(IsPropertyChangeAllowed());
  if (parent_)
    parent_->RemoveChildOrDependent(this);
}

void Layer::RemoveChildOrDependent(Layer* child) {
  if (mask_layer_.get() == child) {
    mask_layer_->SetParent(nullptr);
    mask_layer_ = nullptr;
    SetNeedsFullTreeSync();
    return;
  }
  if (replica_layer_.get() == child) {
    replica_layer_->SetParent(nullptr);
    replica_layer_ = nullptr;
    SetNeedsFullTreeSync();
    return;
  }

  for (LayerList::iterator iter = children_.begin();
       iter != children_.end();
       ++iter) {
    if (iter->get() != child)
      continue;

    child->SetParent(nullptr);
    AddDrawableDescendants(-child->NumDescendantsThatDrawContent() -
                           (child->DrawsContent() ? 1 : 0));
    children_.erase(iter);
    SetNeedsFullTreeSync();
    return;
  }
}

void Layer::ReplaceChild(Layer* reference, scoped_refptr<Layer> new_layer) {
  DCHECK(reference);
  DCHECK_EQ(reference->parent(), this);
  DCHECK(IsPropertyChangeAllowed());

  if (reference == new_layer.get())
    return;

  // Find the index of |reference| in |children_|.
  auto reference_it =
      std::find_if(children_.begin(), children_.end(),
                   [reference](const scoped_refptr<Layer>& layer) {
                     return layer.get() == reference;
                   });
  DCHECK(reference_it != children_.end());
  size_t reference_index = reference_it - children_.begin();
  reference->RemoveFromParent();

  if (new_layer.get()) {
    new_layer->RemoveFromParent();
    InsertChild(new_layer, reference_index);
  }
}

void Layer::SetBounds(const gfx::Size& size) {
  DCHECK(IsPropertyChangeAllowed());
  if (bounds() == size)
    return;
  bounds_ = size;

  if (!layer_tree_host_)
    return;

  if (ClipNode* clip_node = layer_tree_host_->property_trees()->clip_tree.Node(
          clip_tree_index())) {
    if (clip_node->owner_id == id()) {
      clip_node->data.clip.set_size(gfx::SizeF(size));
      layer_tree_host_->property_trees()->clip_tree.set_needs_update(true);
    }
  }

  SetNeedsCommitNoRebuild();
}

Layer* Layer::RootLayer() {
  Layer* layer = this;
  while (layer->parent())
    layer = layer->parent();
  return layer;
}

void Layer::RemoveAllChildren() {
  DCHECK(IsPropertyChangeAllowed());
  while (children_.size()) {
    Layer* layer = children_[0].get();
    DCHECK_EQ(this, layer->parent());
    layer->RemoveFromParent();
  }
}

void Layer::SetChildren(const LayerList& children) {
  DCHECK(IsPropertyChangeAllowed());
  if (children == children_)
    return;

  RemoveAllChildren();
  for (size_t i = 0; i < children.size(); ++i)
    AddChild(children[i]);
}

bool Layer::HasAncestor(const Layer* ancestor) const {
  for (const Layer* layer = parent(); layer; layer = layer->parent()) {
    if (layer == ancestor)
      return true;
  }
  return false;
}

void Layer::RequestCopyOfOutput(
    scoped_ptr<CopyOutputRequest> request) {
  DCHECK(IsPropertyChangeAllowed());
  bool had_no_copy_requests = copy_requests_.empty();
  if (void* source = request->source()) {
    auto it = std::find_if(
        copy_requests_.begin(), copy_requests_.end(),
        [source](const CopyOutputRequest* x) { return x->source() == source; });
    if (it != copy_requests_.end())
      copy_requests_.erase(it);
  }
  if (request->IsEmpty())
    return;
  copy_requests_.push_back(request.Pass());
  if (had_no_copy_requests) {
    UpdateNumCopyRequestsForSubtree(1);
  }
  SetNeedsCommit();
}

void Layer::UpdateNumCopyRequestsForSubtree(int delta) {
  if (!delta)
    return;
  for (Layer* layer = this; layer; layer = layer->parent()) {
    layer->num_layer_or_descendants_with_copy_request_ += delta;
    layer->SetNeedsPushProperties();
    DCHECK_GE(layer->num_layer_or_descendants_with_copy_request_, 0);
  }
}

void Layer::SetBackgroundColor(SkColor background_color) {
  DCHECK(IsPropertyChangeAllowed());
  if (background_color_ == background_color)
    return;
  background_color_ = background_color;
  SetNeedsCommit();
}

SkColor Layer::SafeOpaqueBackgroundColor() const {
  SkColor color = background_color();
  if (SkColorGetA(color) == 255 && !contents_opaque()) {
    color = SK_ColorTRANSPARENT;
  } else if (SkColorGetA(color) != 255 && contents_opaque()) {
    for (const Layer* layer = parent(); layer;
         layer = layer->parent()) {
      color = layer->background_color();
      if (SkColorGetA(color) == 255)
        break;
    }
    if (SkColorGetA(color) != 255)
      color = layer_tree_host_->background_color();
    if (SkColorGetA(color) != 255)
      color = SkColorSetA(color, 255);
  }
  return color;
}

void Layer::SetMasksToBounds(bool masks_to_bounds) {
  DCHECK(IsPropertyChangeAllowed());
  if (masks_to_bounds_ == masks_to_bounds)
    return;
  masks_to_bounds_ = masks_to_bounds;
  SetNeedsCommit();
}

void Layer::SetMaskLayer(Layer* mask_layer) {
  DCHECK(IsPropertyChangeAllowed());
  if (mask_layer_.get() == mask_layer)
    return;
  if (mask_layer_.get()) {
    DCHECK_EQ(this, mask_layer_->parent());
    mask_layer_->RemoveFromParent();
  }
  mask_layer_ = mask_layer;
  if (mask_layer_.get()) {
    DCHECK(!mask_layer_->parent());
    mask_layer_->RemoveFromParent();
    mask_layer_->SetParent(this);
    mask_layer_->SetIsMask(true);
  }
  SetNeedsFullTreeSync();
}

void Layer::SetReplicaLayer(Layer* layer) {
  DCHECK(IsPropertyChangeAllowed());
  if (replica_layer_.get() == layer)
    return;
  if (replica_layer_.get()) {
    DCHECK_EQ(this, replica_layer_->parent());
    replica_layer_->RemoveFromParent();
  }
  replica_layer_ = layer;
  if (replica_layer_.get()) {
    DCHECK(!replica_layer_->parent());
    replica_layer_->RemoveFromParent();
    replica_layer_->SetParent(this);
  }
  SetNeedsFullTreeSync();
}

void Layer::SetFilters(const FilterOperations& filters) {
  DCHECK(IsPropertyChangeAllowed());
  if (filters_ == filters)
    return;
  filters_ = filters;
  SetNeedsCommit();
}

bool Layer::FilterIsAnimating() const {
  DCHECK(layer_tree_host_);
  return layer_animation_controller_
             ? layer_animation_controller_->IsCurrentlyAnimatingProperty(
                   Animation::FILTER,
                   LayerAnimationController::ObserverType::ACTIVE)
             : layer_tree_host_->IsAnimatingFilterProperty(this);
}

bool Layer::HasPotentiallyRunningFilterAnimation() const {
  if (layer_animation_controller_) {
    return layer_animation_controller_->IsPotentiallyAnimatingProperty(
        Animation::FILTER, LayerAnimationController::ObserverType::ACTIVE);
  }
  return layer_tree_host_->HasPotentiallyRunningFilterAnimation(this);
}

void Layer::SetBackgroundFilters(const FilterOperations& filters) {
  DCHECK(IsPropertyChangeAllowed());
  if (background_filters_ == filters)
    return;
  background_filters_ = filters;
  SetNeedsCommit();
}

void Layer::SetOpacity(float opacity) {
  DCHECK(IsPropertyChangeAllowed());
  if (opacity_ == opacity)
    return;
  opacity_ = opacity;
  SetNeedsCommit();
}

bool Layer::OpacityIsAnimating() const {
  DCHECK(layer_tree_host_);
  return layer_animation_controller_
             ? layer_animation_controller_->IsCurrentlyAnimatingProperty(
                   Animation::OPACITY,
                   LayerAnimationController::ObserverType::ACTIVE)
             : layer_tree_host_->IsAnimatingOpacityProperty(this);
}

bool Layer::HasPotentiallyRunningOpacityAnimation() const {
  if (layer_animation_controller_) {
    return layer_animation_controller_->IsPotentiallyAnimatingProperty(
        Animation::OPACITY, LayerAnimationController::ObserverType::ACTIVE);
  }
  return layer_tree_host_->HasPotentiallyRunningOpacityAnimation(this);
}

bool Layer::OpacityCanAnimateOnImplThread() const {
  return false;
}

void Layer::SetBlendMode(SkXfermode::Mode blend_mode) {
  DCHECK(IsPropertyChangeAllowed());
  if (blend_mode_ == blend_mode)
    return;

  // Allowing only blend modes that are defined in the CSS Compositing standard:
  // http://dev.w3.org/fxtf/compositing-1/#blending
  switch (blend_mode) {
    case SkXfermode::kSrcOver_Mode:
    case SkXfermode::kScreen_Mode:
    case SkXfermode::kOverlay_Mode:
    case SkXfermode::kDarken_Mode:
    case SkXfermode::kLighten_Mode:
    case SkXfermode::kColorDodge_Mode:
    case SkXfermode::kColorBurn_Mode:
    case SkXfermode::kHardLight_Mode:
    case SkXfermode::kSoftLight_Mode:
    case SkXfermode::kDifference_Mode:
    case SkXfermode::kExclusion_Mode:
    case SkXfermode::kMultiply_Mode:
    case SkXfermode::kHue_Mode:
    case SkXfermode::kSaturation_Mode:
    case SkXfermode::kColor_Mode:
    case SkXfermode::kLuminosity_Mode:
      // supported blend modes
      break;
    case SkXfermode::kClear_Mode:
    case SkXfermode::kSrc_Mode:
    case SkXfermode::kDst_Mode:
    case SkXfermode::kDstOver_Mode:
    case SkXfermode::kSrcIn_Mode:
    case SkXfermode::kDstIn_Mode:
    case SkXfermode::kSrcOut_Mode:
    case SkXfermode::kDstOut_Mode:
    case SkXfermode::kSrcATop_Mode:
    case SkXfermode::kDstATop_Mode:
    case SkXfermode::kXor_Mode:
    case SkXfermode::kPlus_Mode:
    case SkXfermode::kModulate_Mode:
      // Porter Duff Compositing Operators are not yet supported
      // http://dev.w3.org/fxtf/compositing-1/#porterduffcompositingoperators
      NOTREACHED();
      return;
  }

  blend_mode_ = blend_mode;
  SetNeedsCommit();
}

void Layer::SetIsRootForIsolatedGroup(bool root) {
  DCHECK(IsPropertyChangeAllowed());
  if (is_root_for_isolated_group_ == root)
    return;
  is_root_for_isolated_group_ = root;
  SetNeedsCommit();
}

void Layer::SetContentsOpaque(bool opaque) {
  DCHECK(IsPropertyChangeAllowed());
  if (contents_opaque_ == opaque)
    return;
  contents_opaque_ = opaque;
  SetNeedsCommit();
}

void Layer::SetPosition(const gfx::PointF& position) {
  DCHECK(IsPropertyChangeAllowed());
  if (position_ == position)
    return;
  position_ = position;

  if (!layer_tree_host_)
    return;

  if (TransformNode* transform_node =
          layer_tree_host_->property_trees()->transform_tree.Node(
              transform_tree_index())) {
    if (transform_node->owner_id == id()) {
      transform_node->data.update_post_local_transform(position,
                                                       transform_origin());
      transform_node->data.needs_local_transform_update = true;
      layer_tree_host_->property_trees()->transform_tree.set_needs_update(true);
      SetNeedsCommitNoRebuild();
      return;
    }
  }

  SetNeedsCommit();
}

bool Layer::IsContainerForFixedPositionLayers() const {
  if (!transform_.IsIdentityOrTranslation())
    return true;
  if (parent_ && !parent_->transform_.IsIdentityOrTranslation())
    return true;
  return is_container_for_fixed_position_layers_;
}

bool Are2dAxisAligned(const gfx::Transform& a,
                      const gfx::Transform& b,
                      bool* is_invertible) {
  if (a.IsScaleOrTranslation() && b.IsScaleOrTranslation()) {
    *is_invertible = b.IsInvertible();
    return true;
  }

  gfx::Transform inverse(gfx::Transform::kSkipInitialization);
  *is_invertible = b.GetInverse(&inverse);

  inverse *= a;
  return inverse.Preserves2dAxisAlignment();
}

void Layer::SetTransform(const gfx::Transform& transform) {
  DCHECK(IsPropertyChangeAllowed());
  if (transform_ == transform)
    return;

  if (layer_tree_host_) {
    if (TransformNode* transform_node =
            layer_tree_host_->property_trees()->transform_tree.Node(
                transform_tree_index())) {
      if (transform_node->owner_id == id()) {
        // We need to trigger a rebuild if we could have affected 2d axis
        // alignment. We'll check to see if transform and transform_ are axis
        // align with respect to one another.
        bool invertible = false;
        bool preserves_2d_axis_alignment =
            Are2dAxisAligned(transform_, transform, &invertible);
        transform_node->data.local = transform;
        transform_node->data.needs_local_transform_update = true;
        layer_tree_host_->property_trees()->transform_tree.set_needs_update(
            true);
        if (preserves_2d_axis_alignment)
          SetNeedsCommitNoRebuild();
        else
          SetNeedsCommit();
        transform_ = transform;
        transform_is_invertible_ = invertible;
        return;
      }
    }
  }

  transform_ = transform;
  transform_is_invertible_ = transform.IsInvertible();

  SetNeedsCommit();
}

void Layer::SetTransformOrigin(const gfx::Point3F& transform_origin) {
  DCHECK(IsPropertyChangeAllowed());
  if (transform_origin_ == transform_origin)
    return;
  transform_origin_ = transform_origin;

  if (!layer_tree_host_)
    return;

  if (TransformNode* transform_node =
          layer_tree_host_->property_trees()->transform_tree.Node(
              transform_tree_index())) {
    if (transform_node->owner_id == id()) {
      transform_node->data.update_pre_local_transform(transform_origin);
      transform_node->data.update_post_local_transform(position(),
                                                       transform_origin);
      transform_node->data.needs_local_transform_update = true;
      layer_tree_host_->property_trees()->transform_tree.set_needs_update(true);
      SetNeedsCommitNoRebuild();
      return;
    }
  }

  SetNeedsCommit();
}

bool Layer::AnimationsPreserveAxisAlignment() const {
  DCHECK(layer_tree_host_);
  return layer_animation_controller_
             ? layer_animation_controller_->AnimationsPreserveAxisAlignment()
             : layer_tree_host_->AnimationsPreserveAxisAlignment(this);
}

bool Layer::TransformIsAnimating() const {
  DCHECK(layer_tree_host_);
  return layer_animation_controller_
             ? layer_animation_controller_->IsCurrentlyAnimatingProperty(
                   Animation::TRANSFORM,
                   LayerAnimationController::ObserverType::ACTIVE)
             : layer_tree_host_->IsAnimatingTransformProperty(this);
}

bool Layer::HasPotentiallyRunningTransformAnimation() const {
  if (layer_animation_controller_) {
    return layer_animation_controller_->IsPotentiallyAnimatingProperty(
        Animation::TRANSFORM, LayerAnimationController::ObserverType::ACTIVE);
  }
  return layer_tree_host_->HasPotentiallyRunningTransformAnimation(this);
}

bool Layer::HasOnlyTranslationTransforms() const {
  if (layer_animation_controller_) {
    return layer_animation_controller_->HasOnlyTranslationTransforms(
        LayerAnimationController::ObserverType::ACTIVE);
  }
  return layer_tree_host_->HasOnlyTranslationTransforms(this);
}

bool Layer::MaximumTargetScale(float* max_scale) const {
  if (layer_animation_controller_) {
    return layer_animation_controller_->MaximumTargetScale(
        LayerAnimationController::ObserverType::ACTIVE, max_scale);
  }
  return layer_tree_host_->MaximumTargetScale(this, max_scale);
}

bool Layer::AnimationStartScale(float* start_scale) const {
  if (layer_animation_controller_) {
    return layer_animation_controller_->AnimationStartScale(
        LayerAnimationController::ObserverType::ACTIVE, start_scale);
  }
  return layer_tree_host_->AnimationStartScale(this, start_scale);
}

bool Layer::HasAnyAnimationTargetingProperty(
    Animation::TargetProperty property) const {
  if (layer_animation_controller_)
    return !!layer_animation_controller_->GetAnimation(property);

  return layer_tree_host_->HasAnyAnimationTargetingProperty(this, property);
}

bool Layer::ScrollOffsetAnimationWasInterrupted() const {
  DCHECK(layer_tree_host_);
  return layer_animation_controller_
             ? layer_animation_controller_
                   ->scroll_offset_animation_was_interrupted()
             : layer_tree_host_->ScrollOffsetAnimationWasInterrupted(this);
}

void Layer::SetScrollParent(Layer* parent) {
  DCHECK(IsPropertyChangeAllowed());
  if (scroll_parent_ == parent)
    return;

  if (scroll_parent_)
    scroll_parent_->RemoveScrollChild(this);

  scroll_parent_ = parent;

  if (scroll_parent_)
    scroll_parent_->AddScrollChild(this);

  SetNeedsCommit();
}

void Layer::AddScrollChild(Layer* child) {
  if (!scroll_children_)
    scroll_children_.reset(new std::set<Layer*>);
  scroll_children_->insert(child);
  SetNeedsCommit();
}

void Layer::RemoveScrollChild(Layer* child) {
  scroll_children_->erase(child);
  if (scroll_children_->empty())
    scroll_children_ = nullptr;
  SetNeedsCommit();
}

void Layer::SetClipParent(Layer* ancestor) {
  DCHECK(IsPropertyChangeAllowed());
  if (clip_parent_ == ancestor)
    return;

  if (clip_parent_)
    clip_parent_->RemoveClipChild(this);

  clip_parent_ = ancestor;

  if (clip_parent_)
    clip_parent_->AddClipChild(this);

  SetNeedsCommit();
  if (layer_tree_host_)
    layer_tree_host_->SetNeedsMetaInfoRecomputation(true);
}

void Layer::AddClipChild(Layer* child) {
  if (!clip_children_)
    clip_children_.reset(new std::set<Layer*>);
  clip_children_->insert(child);
  SetNeedsCommit();
}

void Layer::RemoveClipChild(Layer* child) {
  clip_children_->erase(child);
  if (clip_children_->empty())
    clip_children_ = nullptr;
  SetNeedsCommit();
}

void Layer::SetScrollOffset(const gfx::ScrollOffset& scroll_offset) {
  DCHECK(IsPropertyChangeAllowed());

  if (scroll_offset_ == scroll_offset)
    return;
  scroll_offset_ = scroll_offset;

  if (!layer_tree_host_)
    return;

  if (TransformNode* transform_node =
          layer_tree_host_->property_trees()->transform_tree.Node(
              transform_tree_index())) {
    if (transform_node->owner_id == id()) {
      transform_node->data.scroll_offset = CurrentScrollOffset();
      transform_node->data.needs_local_transform_update = true;
      layer_tree_host_->property_trees()->transform_tree.set_needs_update(true);
      SetNeedsCommitNoRebuild();
      return;
    }
  }

  SetNeedsCommit();
}

void Layer::SetScrollCompensationAdjustment(
    const gfx::Vector2dF& scroll_compensation_adjustment) {
  if (scroll_compensation_adjustment_ == scroll_compensation_adjustment)
    return;
  scroll_compensation_adjustment_ = scroll_compensation_adjustment;
  SetNeedsCommit();
}

gfx::Vector2dF Layer::ScrollCompensationAdjustment() const {
  return scroll_compensation_adjustment_;
}

void Layer::SetScrollOffsetFromImplSide(
    const gfx::ScrollOffset& scroll_offset) {
  DCHECK(IsPropertyChangeAllowed());
  // This function only gets called during a BeginMainFrame, so there
  // is no need to call SetNeedsUpdate here.
  DCHECK(layer_tree_host_ && layer_tree_host_->CommitRequested());
  if (scroll_offset_ == scroll_offset)
    return;
  scroll_offset_ = scroll_offset;
  SetNeedsPushProperties();

  bool needs_rebuild = true;
  if (TransformNode* transform_node =
          layer_tree_host_->property_trees()->transform_tree.Node(
              transform_tree_index())) {
    if (transform_node->owner_id == id()) {
      transform_node->data.scroll_offset = CurrentScrollOffset();
      transform_node->data.needs_local_transform_update = true;
      layer_tree_host_->property_trees()->transform_tree.set_needs_update(true);
      needs_rebuild = false;
    }
  }

  if (needs_rebuild)
    layer_tree_host_->property_trees()->needs_rebuild = true;

  if (!did_scroll_callback_.is_null())
    did_scroll_callback_.Run();
  // The callback could potentially change the layer structure:
  // "this" may have been destroyed during the process.
}

void Layer::SetScrollClipLayerId(int clip_layer_id) {
  DCHECK(IsPropertyChangeAllowed());
  if (scroll_clip_layer_id_ == clip_layer_id)
    return;
  scroll_clip_layer_id_ = clip_layer_id;
  SetNeedsCommit();
}

void Layer::SetUserScrollable(bool horizontal, bool vertical) {
  DCHECK(IsPropertyChangeAllowed());
  if (user_scrollable_horizontal_ == horizontal &&
      user_scrollable_vertical_ == vertical)
    return;
  user_scrollable_horizontal_ = horizontal;
  user_scrollable_vertical_ = vertical;
  SetNeedsCommit();
}

void Layer::SetShouldScrollOnMainThread(bool should_scroll_on_main_thread) {
  DCHECK(IsPropertyChangeAllowed());
  if (should_scroll_on_main_thread_ == should_scroll_on_main_thread)
    return;
  should_scroll_on_main_thread_ = should_scroll_on_main_thread;
  SetNeedsCommit();
}

void Layer::SetHaveWheelEventHandlers(bool have_wheel_event_handlers) {
  DCHECK(IsPropertyChangeAllowed());
  if (have_wheel_event_handlers_ == have_wheel_event_handlers)
    return;

  have_wheel_event_handlers_ = have_wheel_event_handlers;
  SetNeedsCommit();
}

void Layer::SetHaveScrollEventHandlers(bool have_scroll_event_handlers) {
  DCHECK(IsPropertyChangeAllowed());
  if (have_scroll_event_handlers_ == have_scroll_event_handlers)
    return;
  have_scroll_event_handlers_ = have_scroll_event_handlers;
  SetNeedsCommit();
}

void Layer::SetNonFastScrollableRegion(const Region& region) {
  DCHECK(IsPropertyChangeAllowed());
  if (non_fast_scrollable_region_ == region)
    return;
  non_fast_scrollable_region_ = region;
  SetNeedsCommit();
}

void Layer::SetTouchEventHandlerRegion(const Region& region) {
  DCHECK(IsPropertyChangeAllowed());
  if (touch_event_handler_region_ == region)
    return;

  touch_event_handler_region_ = region;
  SetNeedsCommit();
}

void Layer::SetScrollBlocksOn(ScrollBlocksOn scroll_blocks_on) {
  DCHECK(IsPropertyChangeAllowed());
  if (scroll_blocks_on_ == scroll_blocks_on)
    return;
  scroll_blocks_on_ = scroll_blocks_on;
  SetNeedsCommit();
}

void Layer::SetForceRenderSurface(bool force) {
  DCHECK(IsPropertyChangeAllowed());
  if (force_render_surface_ == force)
    return;
  force_render_surface_ = force;
  SetNeedsCommit();
}

void Layer::SetDoubleSided(bool double_sided) {
  DCHECK(IsPropertyChangeAllowed());
  if (double_sided_ == double_sided)
    return;
  double_sided_ = double_sided;
  SetNeedsCommit();
}

void Layer::Set3dSortingContextId(int id) {
  DCHECK(IsPropertyChangeAllowed());
  if (id == sorting_context_id_)
    return;
  sorting_context_id_ = id;
  SetNeedsCommit();
}

void Layer::SetTransformTreeIndex(int index) {
  DCHECK(IsPropertyChangeAllowed());
  if (transform_tree_index_ == index)
    return;
  transform_tree_index_ = index;
  SetNeedsPushProperties();
}

int Layer::transform_tree_index() const {
  if (!layer_tree_host_ ||
      layer_tree_host_->property_trees()->sequence_number !=
          property_tree_sequence_number_) {
    return -1;
  }
  return transform_tree_index_;
}

void Layer::SetClipTreeIndex(int index) {
  DCHECK(IsPropertyChangeAllowed());
  if (clip_tree_index_ == index)
    return;
  clip_tree_index_ = index;
  SetNeedsPushProperties();
}

int Layer::clip_tree_index() const {
  if (!layer_tree_host_ ||
      layer_tree_host_->property_trees()->sequence_number !=
          property_tree_sequence_number_) {
    return -1;
  }
  return clip_tree_index_;
}

void Layer::SetEffectTreeIndex(int index) {
  DCHECK(IsPropertyChangeAllowed());
  if (effect_tree_index_ == index)
    return;
  effect_tree_index_ = index;
  SetNeedsPushProperties();
}

int Layer::effect_tree_index() const {
  if (!layer_tree_host_ ||
      layer_tree_host_->property_trees()->sequence_number !=
          property_tree_sequence_number_) {
    return -1;
  }
  return effect_tree_index_;
}

void Layer::InvalidatePropertyTreesIndices() {
  int invalid_property_tree_index = -1;
  SetTransformTreeIndex(invalid_property_tree_index);
  SetClipTreeIndex(invalid_property_tree_index);
  SetEffectTreeIndex(invalid_property_tree_index);
}

void Layer::SetShouldFlattenTransform(bool should_flatten) {
  DCHECK(IsPropertyChangeAllowed());
  if (should_flatten_transform_ == should_flatten)
    return;
  should_flatten_transform_ = should_flatten;
  SetNeedsCommit();
}

void Layer::SetIsDrawable(bool is_drawable) {
  DCHECK(IsPropertyChangeAllowed());
  if (is_drawable_ == is_drawable)
    return;

  is_drawable_ = is_drawable;
  UpdateDrawsContent(HasDrawableContent());
}

void Layer::SetHideLayerAndSubtree(bool hide) {
  DCHECK(IsPropertyChangeAllowed());
  if (hide_layer_and_subtree_ == hide)
    return;

  hide_layer_and_subtree_ = hide;
  SetNeedsCommit();
}

void Layer::SetNeedsDisplayRect(const gfx::Rect& dirty_rect) {
  if (dirty_rect.IsEmpty())
    return;

  SetNeedsPushProperties();
  update_rect_.Union(dirty_rect);

  if (DrawsContent())
    SetNeedsUpdate();
}

bool Layer::DescendantIsFixedToContainerLayer() const {
  for (size_t i = 0; i < children_.size(); ++i) {
    if (children_[i]->position_constraint_.is_fixed_position() ||
        children_[i]->DescendantIsFixedToContainerLayer())
      return true;
  }
  return false;
}

void Layer::SetIsContainerForFixedPositionLayers(bool container) {
  if (is_container_for_fixed_position_layers_ == container)
    return;
  is_container_for_fixed_position_layers_ = container;

  if (layer_tree_host_ && layer_tree_host_->CommitRequested())
    return;

  // Only request a commit if we have a fixed positioned descendant.
  if (DescendantIsFixedToContainerLayer())
    SetNeedsCommit();
}

void Layer::SetPositionConstraint(const LayerPositionConstraint& constraint) {
  DCHECK(IsPropertyChangeAllowed());
  if (position_constraint_ == constraint)
    return;
  position_constraint_ = constraint;
  SetNeedsCommit();
}

static void RunCopyCallbackOnMainThread(scoped_ptr<CopyOutputRequest> request,
                                        scoped_ptr<CopyOutputResult> result) {
  request->SendResult(result.Pass());
}

static void PostCopyCallbackToMainThread(
    scoped_refptr<base::SingleThreadTaskRunner> main_thread_task_runner,
    scoped_ptr<CopyOutputRequest> request,
    scoped_ptr<CopyOutputResult> result) {
  main_thread_task_runner->PostTask(FROM_HERE,
                                    base::Bind(&RunCopyCallbackOnMainThread,
                                               base::Passed(&request),
                                               base::Passed(&result)));
}

void Layer::PushPropertiesTo(LayerImpl* layer) {
  DCHECK(layer_tree_host_);

  // If we did not SavePaintProperties() for the layer this frame, then push the
  // real property values, not the paint property values.
  bool use_paint_properties = paint_properties_.source_frame_number ==
                              layer_tree_host_->source_frame_number();

  layer->SetTransformOrigin(transform_origin_);
  layer->SetBackgroundColor(background_color_);
  layer->SetBounds(use_paint_properties ? paint_properties_.bounds
                                        : bounds_);

#if defined(NDEBUG)
  if (frame_viewer_instrumentation::IsTracingLayerTreeSnapshots())
    layer->SetDebugInfo(TakeDebugInfo());
#else
  layer->SetDebugInfo(TakeDebugInfo());
#endif

  layer->SetTransformTreeIndex(transform_tree_index());
  layer->SetEffectTreeIndex(effect_tree_index());
  layer->SetClipTreeIndex(clip_tree_index());
  layer->set_offset_to_transform_parent(offset_to_transform_parent_);
  layer->SetDoubleSided(double_sided_);
  layer->SetDrawsContent(DrawsContent());
  layer->SetHideLayerAndSubtree(hide_layer_and_subtree_);
  layer->SetHasRenderSurface(has_render_surface_);
  if (!layer->FilterIsAnimatingOnImplOnly() && !FilterIsAnimating())
    layer->SetFilters(filters_);
  DCHECK(!(FilterIsAnimating() && layer->FilterIsAnimatingOnImplOnly()));
  layer->SetBackgroundFilters(background_filters());
  layer->SetMasksToBounds(masks_to_bounds_);
  layer->SetShouldScrollOnMainThread(should_scroll_on_main_thread_);
  layer->SetHaveWheelEventHandlers(have_wheel_event_handlers_);
  layer->SetHaveScrollEventHandlers(have_scroll_event_handlers_);
  layer->SetNonFastScrollableRegion(non_fast_scrollable_region_);
  layer->SetTouchEventHandlerRegion(touch_event_handler_region_);
  layer->SetScrollBlocksOn(scroll_blocks_on_);
  layer->SetContentsOpaque(contents_opaque_);
  if (!layer->OpacityIsAnimatingOnImplOnly() && !OpacityIsAnimating())
    layer->SetOpacity(opacity_);
  DCHECK(!(OpacityIsAnimating() && layer->OpacityIsAnimatingOnImplOnly()));
  layer->SetBlendMode(blend_mode_);
  layer->SetIsRootForIsolatedGroup(is_root_for_isolated_group_);
  layer->SetPosition(position_);
  layer->SetIsContainerForFixedPositionLayers(
      IsContainerForFixedPositionLayers());
  layer->SetPositionConstraint(position_constraint_);
  layer->SetShouldFlattenTransform(should_flatten_transform_);
  layer->set_should_flatten_transform_from_property_tree(
      should_flatten_transform_from_property_tree_);
  layer->set_num_layer_or_descendant_with_copy_request(
      num_layer_or_descendants_with_copy_request_);
  layer->set_draw_blend_mode(draw_blend_mode_);
  layer->SetUseParentBackfaceVisibility(use_parent_backface_visibility_);
  if (!layer->TransformIsAnimatingOnImplOnly() && !TransformIsAnimating())
    layer->SetTransformAndInvertibility(transform_, transform_is_invertible_);
  DCHECK(!(TransformIsAnimating() && layer->TransformIsAnimatingOnImplOnly()));
  layer->Set3dSortingContextId(sorting_context_id_);
  layer->SetNumDescendantsThatDrawContent(num_descendants_that_draw_content_);

  layer->SetScrollClipLayer(scroll_clip_layer_id_);
  layer->set_user_scrollable_horizontal(user_scrollable_horizontal_);
  layer->set_user_scrollable_vertical(user_scrollable_vertical_);

  LayerImpl* scroll_parent = nullptr;
  if (scroll_parent_) {
    scroll_parent = layer->layer_tree_impl()->LayerById(scroll_parent_->id());
    DCHECK(scroll_parent);
  }

  layer->SetScrollParent(scroll_parent);
  if (scroll_children_) {
    std::set<LayerImpl*>* scroll_children = new std::set<LayerImpl*>;
    for (std::set<Layer*>::iterator it = scroll_children_->begin();
         it != scroll_children_->end();
         ++it) {
      DCHECK_EQ((*it)->scroll_parent(), this);
      LayerImpl* scroll_child =
          layer->layer_tree_impl()->LayerById((*it)->id());
      DCHECK(scroll_child);
      scroll_children->insert(scroll_child);
    }
    layer->SetScrollChildren(scroll_children);
  } else {
    layer->SetScrollChildren(nullptr);
  }

  LayerImpl* clip_parent = nullptr;
  if (clip_parent_) {
    clip_parent =
        layer->layer_tree_impl()->LayerById(clip_parent_->id());
    DCHECK(clip_parent);
  }

  layer->SetClipParent(clip_parent);
  if (clip_children_) {
    std::set<LayerImpl*>* clip_children = new std::set<LayerImpl*>;
    for (std::set<Layer*>::iterator it = clip_children_->begin();
        it != clip_children_->end(); ++it) {
      DCHECK_EQ((*it)->clip_parent(), this);
      LayerImpl* clip_child = layer->layer_tree_impl()->LayerById((*it)->id());
      DCHECK(clip_child);
      clip_children->insert(clip_child);
    }
    layer->SetClipChildren(clip_children);
  } else {
    layer->SetClipChildren(nullptr);
  }

  // When a scroll offset animation is interrupted the new scroll position on
  // the pending tree will clobber any impl-side scrolling occuring on the
  // active tree. To do so, avoid scrolling the pending tree along with it
  // instead of trying to undo that scrolling later.
  if (ScrollOffsetAnimationWasInterrupted())
    layer->PushScrollOffsetFromMainThreadAndClobberActiveValue(scroll_offset_);
  else
    layer->PushScrollOffsetFromMainThread(scroll_offset_);
  layer->SetScrollCompensationAdjustment(ScrollCompensationAdjustment());

  // Wrap the copy_requests_ in a PostTask to the main thread.
  bool had_copy_requests = !copy_requests_.empty();
  ScopedPtrVector<CopyOutputRequest> main_thread_copy_requests;
  for (ScopedPtrVector<CopyOutputRequest>::iterator it = copy_requests_.begin();
       it != copy_requests_.end();
       ++it) {
    scoped_refptr<base::SingleThreadTaskRunner> main_thread_task_runner =
        layer_tree_host()->proxy()->MainThreadTaskRunner();
    scoped_ptr<CopyOutputRequest> original_request = copy_requests_.take(it);
    const CopyOutputRequest& original_request_ref = *original_request;
    scoped_ptr<CopyOutputRequest> main_thread_request =
        CopyOutputRequest::CreateRelayRequest(
            original_request_ref,
            base::Bind(&PostCopyCallbackToMainThread,
                       main_thread_task_runner,
                       base::Passed(&original_request)));
    main_thread_copy_requests.push_back(main_thread_request.Pass());
  }
  if (!copy_requests_.empty() && layer_tree_host_)
    layer_tree_host_->property_trees()->needs_rebuild = true;
  if (had_copy_requests)
    UpdateNumCopyRequestsForSubtree(-1);

  copy_requests_.clear();
  layer->PassCopyRequests(&main_thread_copy_requests);

  // If the main thread commits multiple times before the impl thread actually
  // draws, then damage tracking will become incorrect if we simply clobber the
  // update_rect here. The LayerImpl's update_rect needs to accumulate (i.e.
  // union) any update changes that have occurred on the main thread.
  update_rect_.Union(layer->update_rect());
  layer->SetUpdateRect(update_rect_);

  layer->SetStackingOrderChanged(stacking_order_changed_);

  if (layer->layer_animation_controller() && layer_animation_controller_)
    layer_animation_controller_->PushAnimationUpdatesTo(
        layer->layer_animation_controller());

  if (frame_timing_requests_dirty_) {
    layer->SetFrameTimingRequests(frame_timing_requests_);
    frame_timing_requests_dirty_ = false;
  }

  bool is_page_scale_layer = this == layer_tree_host()->page_scale_layer();
  bool parent_affected =
      layer->parent() && layer->parent()->IsAffectedByPageScale();
  layer->SetIsAffectedByPageScale(is_page_scale_layer || parent_affected);

  // Reset any state that should be cleared for the next update.
  stacking_order_changed_ = false;
  update_rect_ = gfx::Rect();

  needs_push_properties_ = false;
  num_dependents_need_push_properties_ = 0;
}

scoped_ptr<LayerImpl> Layer::CreateLayerImpl(LayerTreeImpl* tree_impl) {
  return LayerImpl::Create(tree_impl, layer_id_,
                           new LayerImpl::SyncedScrollOffset);
}

bool Layer::DrawsContent() const {
  return draws_content_;
}

bool Layer::HasDrawableContent() const {
  return is_drawable_;
}

void Layer::UpdateDrawsContent(bool has_drawable_content) {
  bool draws_content = has_drawable_content;
  DCHECK(is_drawable_ || !has_drawable_content);
  if (draws_content == draws_content_)
    return;

  if (HasDelegatedContent()) {
    // Layers with delegated content need to be treated as if they have as
    // many children as the number of layers they own delegated quads for.
    // Since we don't know this number right now, we choose one that acts like
    // infinity for our purposes.
    AddDrawableDescendants(draws_content ? 1000 : -1000);
  }

  if (parent())
    parent()->AddDrawableDescendants(draws_content ? 1 : -1);

  draws_content_ = draws_content;
  SetNeedsCommit();
}

int Layer::NumDescendantsThatDrawContent() const {
  return num_descendants_that_draw_content_;
}

void Layer::SavePaintProperties() {
  DCHECK(layer_tree_host_);

  // TODO(reveman): Save all layer properties that we depend on not
  // changing until PushProperties() has been called. crbug.com/231016
  paint_properties_.bounds = bounds_;
  paint_properties_.source_frame_number =
      layer_tree_host_->source_frame_number();
}

bool Layer::Update() {
  DCHECK(layer_tree_host_);
  DCHECK_EQ(layer_tree_host_->source_frame_number(),
            paint_properties_.source_frame_number) <<
      "SavePaintProperties must be called for any layer that is painted.";
  return false;
}

bool Layer::IsSuitableForGpuRasterization() const {
  return true;
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
Layer::TakeDebugInfo() {
  if (client_)
    return client_->TakeDebugInfo();
  else
    return nullptr;
}

void Layer::SetHasRenderSurface(bool has_render_surface) {
  if (has_render_surface_ == has_render_surface)
    return;
  has_render_surface_ = has_render_surface;
  // We do not need SetNeedsCommit here, since this is only ever called
  // during a commit, from CalculateDrawProperties.
  SetNeedsPushProperties();
  layer_tree_host_->property_trees()->needs_rebuild = true;
}

gfx::ScrollOffset Layer::ScrollOffsetForAnimation() const {
  return CurrentScrollOffset();
}

// On<Property>Animated is called due to an ongoing accelerated animation.
// Since this animation is also being run on the compositor thread, there
// is no need to request a commit to push this value over, so the value is
// set directly rather than by calling Set<Property>.
void Layer::OnFilterAnimated(const FilterOperations& filters) {
  filters_ = filters;
}

void Layer::OnOpacityAnimated(float opacity) {
  if (opacity_ == opacity)
    return;
  opacity_ = opacity;
  // Changing the opacity may make a previously hidden layer visible, so a new
  // recording may be needed.
  SetNeedsUpdate();
  if (layer_tree_host_) {
    if (EffectNode* node = layer_tree_host_->property_trees()->effect_tree.Node(
            effect_tree_index())) {
      if (node->owner_id == id()) {
        node->data.opacity = opacity;
        layer_tree_host_->property_trees()->effect_tree.set_needs_update(true);
      }
    }
  }
}

void Layer::OnTransformAnimated(const gfx::Transform& transform) {
  if (transform_ == transform)
    return;
  transform_ = transform;
  transform_is_invertible_ = transform.IsInvertible();
  // Changing the transform may change the visible part of this layer, so a new
  // recording may be needed.
  SetNeedsUpdate();
  if (layer_tree_host_) {
    if (TransformNode* node =
            layer_tree_host_->property_trees()->transform_tree.Node(
                transform_tree_index())) {
      if (node->owner_id == id()) {
        node->data.local = transform;
        node->data.needs_local_transform_update = true;
        node->data.is_animated = true;
        layer_tree_host_->property_trees()->transform_tree.set_needs_update(
            true);
      }
    }
  }
}

void Layer::OnScrollOffsetAnimated(const gfx::ScrollOffset& scroll_offset) {
  // Do nothing. Scroll deltas will be sent from the compositor thread back
  // to the main thread in the same manner as during non-animated
  // compositor-driven scrolling.
}

void Layer::OnAnimationWaitingForDeletion() {
  // Animations are only deleted during PushProperties.
  SetNeedsPushProperties();
}

void Layer::OnTransformIsPotentiallyAnimatingChanged(bool is_animating) {
  if (!layer_tree_host_)
    return;
  TransformTree& transform_tree =
      layer_tree_host_->property_trees()->transform_tree;
  TransformNode* node = transform_tree.Node(transform_tree_index());
  if (!node)
    return;

  if (node->owner_id == id()) {
    node->data.is_animated = is_animating;
    if (is_animating) {
      float maximum_target_scale = 0.f;
      node->data.local_maximum_animation_target_scale =
          MaximumTargetScale(&maximum_target_scale) ? maximum_target_scale
                                                    : 0.f;

      float animation_start_scale = 0.f;
      node->data.local_starting_animation_scale =
          AnimationStartScale(&animation_start_scale) ? animation_start_scale
                                                      : 0.f;

      node->data.has_only_translation_animations =
          HasOnlyTranslationTransforms();

    } else {
      node->data.local_maximum_animation_target_scale = 0.f;
      node->data.local_starting_animation_scale = 0.f;
      node->data.has_only_translation_animations = true;
    }
    transform_tree.set_needs_update(true);
  }
}

bool Layer::IsActive() const {
  return true;
}

bool Layer::AddAnimation(scoped_ptr <Animation> animation) {
  DCHECK(layer_animation_controller_);
  if (!layer_animation_controller_->animation_registrar())
    return false;

  if (animation->target_property() == Animation::SCROLL_OFFSET &&
      !layer_animation_controller_->animation_registrar()
           ->supports_scroll_animations())
    return false;

  UMA_HISTOGRAM_BOOLEAN("Renderer.AnimationAddedToOrphanLayer",
                        !layer_tree_host_);
  layer_animation_controller_->AddAnimation(animation.Pass());
  SetNeedsCommit();
  return true;
}

void Layer::PauseAnimation(int animation_id, double time_offset) {
  DCHECK(layer_animation_controller_);
  layer_animation_controller_->PauseAnimation(
      animation_id, base::TimeDelta::FromSecondsD(time_offset));
  SetNeedsCommit();
}

void Layer::RemoveAnimation(int animation_id) {
  DCHECK(layer_animation_controller_);
  layer_animation_controller_->RemoveAnimation(animation_id);
  SetNeedsCommit();
}

void Layer::RemoveAnimation(int animation_id,
                            Animation::TargetProperty property) {
  DCHECK(layer_animation_controller_);
  layer_animation_controller_->RemoveAnimation(animation_id, property);
  SetNeedsCommit();
}

void Layer::SetLayerAnimationControllerForTest(
    scoped_refptr<LayerAnimationController> controller) {
  DCHECK(layer_animation_controller_);
  layer_animation_controller_->RemoveValueObserver(this);
  layer_animation_controller_ = controller;
  layer_animation_controller_->AddValueObserver(this);
  SetNeedsCommit();
}

bool Layer::HasActiveAnimation() const {
  DCHECK(layer_tree_host_);
  return layer_animation_controller_
             ? layer_animation_controller_->HasActiveAnimation()
             : layer_tree_host_->HasActiveAnimation(this);
}

void Layer::RegisterForAnimations(AnimationRegistrar* registrar) {
  if (layer_animation_controller_)
    layer_animation_controller_->SetAnimationRegistrar(registrar);
}

void Layer::AddLayerAnimationEventObserver(
    LayerAnimationEventObserver* animation_observer) {
  DCHECK(layer_animation_controller_);
  layer_animation_controller_->AddEventObserver(animation_observer);
}

void Layer::RemoveLayerAnimationEventObserver(
    LayerAnimationEventObserver* animation_observer) {
  DCHECK(layer_animation_controller_);
  layer_animation_controller_->RemoveEventObserver(animation_observer);
}

ScrollbarLayerInterface* Layer::ToScrollbarLayer() {
  return nullptr;
}

RenderingStatsInstrumentation* Layer::rendering_stats_instrumentation() const {
  return layer_tree_host_->rendering_stats_instrumentation();
}

void Layer::RemoveFromScrollTree() {
  if (scroll_children_.get()) {
    std::set<Layer*> copy = *scroll_children_;
    for (std::set<Layer*>::iterator it = copy.begin(); it != copy.end(); ++it)
      (*it)->SetScrollParent(nullptr);
  }

  DCHECK(!scroll_children_);
  SetScrollParent(nullptr);
}

void Layer::RemoveFromClipTree() {
  if (clip_children_.get()) {
    std::set<Layer*> copy = *clip_children_;
    for (std::set<Layer*>::iterator it = copy.begin(); it != copy.end(); ++it)
      (*it)->SetClipParent(nullptr);
  }

  DCHECK(!clip_children_);
  SetClipParent(nullptr);
}

void Layer::AddDrawableDescendants(int num) {
  DCHECK_GE(num_descendants_that_draw_content_, 0);
  DCHECK_GE(num_descendants_that_draw_content_ + num, 0);
  if (num == 0)
    return;
  num_descendants_that_draw_content_ += num;
  SetNeedsCommit();
  if (parent())
    parent()->AddDrawableDescendants(num);
}

void Layer::RunMicroBenchmark(MicroBenchmark* benchmark) {
  benchmark->RunOnLayer(this);
}

bool Layer::HasDelegatedContent() const {
  return false;
}

void Layer::SetFrameTimingRequests(
    const std::vector<FrameTimingRequest>& requests) {
  // TODO(vmpstr): Early out if there are no changes earlier in the call stack.
  if (requests == frame_timing_requests_)
    return;
  frame_timing_requests_ = requests;
  frame_timing_requests_dirty_ = true;
  SetNeedsCommit();
}

void Layer::DidBeginTracing() {
  // We'll be dumping layer trees as part of trace, so make sure
  // PushPropertiesTo() propagates layer debug info to the impl
  // side -- otherwise this won't happen for the the layers that
  // remain unchanged since tracing started.
  SetNeedsPushProperties();
}

void Layer::set_visited(bool visited) {
  visited_tracker_ =
      visited ? layer_tree_host()->meta_information_sequence_number() : 0;
}

bool Layer::visited() {
  return visited_tracker_ ==
         layer_tree_host()->meta_information_sequence_number();
}

void Layer::set_layer_or_descendant_is_drawn(
    bool layer_or_descendant_is_drawn) {
  layer_or_descendant_is_drawn_tracker_ =
      layer_or_descendant_is_drawn
          ? layer_tree_host()->meta_information_sequence_number()
          : 0;
}

bool Layer::layer_or_descendant_is_drawn() {
  return layer_or_descendant_is_drawn_tracker_ ==
         layer_tree_host()->meta_information_sequence_number();
}

void Layer::set_sorted_for_recursion(bool sorted_for_recursion) {
  sorted_for_recursion_tracker_ =
      sorted_for_recursion
          ? layer_tree_host()->meta_information_sequence_number()
          : 0;
}

bool Layer::sorted_for_recursion() {
  return sorted_for_recursion_tracker_ ==
         layer_tree_host()->meta_information_sequence_number();
}

gfx::Transform Layer::draw_transform() const {
  DCHECK_NE(transform_tree_index_, -1);
  return DrawTransformFromPropertyTrees(
      this, layer_tree_host_->property_trees()->transform_tree);
}

gfx::Transform Layer::screen_space_transform() const {
  DCHECK_NE(transform_tree_index_, -1);
  return ScreenSpaceTransformFromPropertyTrees(
      this, layer_tree_host_->property_trees()->transform_tree);
}

}  // namespace cc
