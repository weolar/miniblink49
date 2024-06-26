// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <set>
#include <vector>

#include "base/logging.h"
#include "cc/base/math_util.h"
#include "cc/trees/property_tree.h"

namespace cc {

template <typename T>
PropertyTree<T>::PropertyTree()
    : needs_update_(false)
{
    nodes_.push_back(T());
    back()->id = 0;
    back()->parent_id = -1;
}

template <typename T>
PropertyTree<T>::~PropertyTree()
{
}

TransformTree::TransformTree()
    : source_to_parent_updates_allowed_(true)
{
}

TransformTree::~TransformTree()
{
}

template <typename T>
int PropertyTree<T>::Insert(const T& tree_node, int parent_id)
{
    DCHECK_GT(nodes_.size(), 0u);
    nodes_.push_back(tree_node);
    T& node = nodes_.back();
    node.parent_id = parent_id;
    node.id = static_cast<int>(nodes_.size()) - 1;
    return node.id;
}

template <typename T>
void PropertyTree<T>::clear()
{
    nodes_.clear();
    nodes_.push_back(T());
    back()->id = 0;
    back()->parent_id = -1;
}

template class PropertyTree<TransformNode>;
template class PropertyTree<ClipNode>;
template class PropertyTree<EffectNode>;

TransformNodeData::TransformNodeData()
    : target_id(-1)
    , content_target_id(-1)
    , source_node_id(-1)
    , needs_local_transform_update(true)
    , is_invertible(true)
    , ancestors_are_invertible(true)
    , is_animated(false)
    , to_screen_is_animated(false)
    , has_only_translation_animations(true)
    , to_screen_has_scale_animation(false)
    , flattens_inherited_transform(false)
    , node_and_ancestors_are_flat(true)
    , node_and_ancestors_have_only_integer_translation(true)
    , scrolls(false)
    , needs_sublayer_scale(false)
    , affected_by_inner_viewport_bounds_delta_x(false)
    , affected_by_inner_viewport_bounds_delta_y(false)
    , affected_by_outer_viewport_bounds_delta_x(false)
    , affected_by_outer_viewport_bounds_delta_y(false)
    , layer_scale_factor(1.0f)
    , post_local_scale_factor(1.0f)
    , local_maximum_animation_target_scale(0.f)
    , local_starting_animation_scale(0.f)
    , combined_maximum_animation_target_scale(0.f)
    , combined_starting_animation_scale(0.f)
{
}

TransformNodeData::~TransformNodeData()
{
}

void TransformNodeData::update_pre_local_transform(
    const gfx::Point3F& transform_origin)
{
    pre_local.MakeIdentity();
    pre_local.Translate3d(-transform_origin.x(), -transform_origin.y(),
        -transform_origin.z());
}

void TransformNodeData::update_post_local_transform(
    const gfx::PointF& position,
    const gfx::Point3F& transform_origin)
{
    post_local.MakeIdentity();
    post_local.Scale(post_local_scale_factor, post_local_scale_factor);
    post_local.Translate3d(
        position.x() + source_offset.x() + transform_origin.x(),
        position.y() + source_offset.y() + transform_origin.y(),
        transform_origin.z());
}

ClipNodeData::ClipNodeData()
    : transform_id(-1)
    , target_id(-1)
    , use_only_parent_clip(false)
    , layer_clipping_uses_only_local_clip(false)
    , layer_visibility_uses_only_local_clip(false)
    , render_surface_is_clipped(false)
    , layers_are_clipped(false)
{
}

EffectNodeData::EffectNodeData()
    : opacity(1.f)
    , screen_space_opacity(1.f)
    , has_render_surface(false)
    , transform_id(0)
    , clip_id(0)
{
}

void TransformTree::clear()
{
    PropertyTree<TransformNode>::clear();

    nodes_affected_by_inner_viewport_bounds_delta_.clear();
    nodes_affected_by_outer_viewport_bounds_delta_.clear();
}

bool TransformTree::ComputeTransform(int source_id,
    int dest_id,
    gfx::Transform* transform) const
{
    transform->MakeIdentity();

    if (source_id == dest_id)
        return true;

    if (source_id > dest_id) {
        return CombineTransformsBetween(source_id, dest_id, transform);
    }

    return CombineInversesBetween(source_id, dest_id, transform);
}

bool TransformTree::ComputeTransformWithDestinationSublayerScale(
    int source_id,
    int dest_id,
    gfx::Transform* transform) const
{
    bool success = ComputeTransform(source_id, dest_id, transform);

    const TransformNode* dest_node = Node(dest_id);
    if (!dest_node->data.needs_sublayer_scale)
        return success;

    transform->matrix().postScale(dest_node->data.sublayer_scale.x(),
        dest_node->data.sublayer_scale.y(), 1.f);
    return success;
}

bool TransformTree::ComputeTransformWithSourceSublayerScale(
    int source_id,
    int dest_id,
    gfx::Transform* transform) const
{
    bool success = ComputeTransform(source_id, dest_id, transform);

    const TransformNode* source_node = Node(source_id);
    if (!source_node->data.needs_sublayer_scale)
        return success;

    if (source_node->data.sublayer_scale.x() == 0 || source_node->data.sublayer_scale.y() == 0)
        return false;

    transform->Scale(1.f / source_node->data.sublayer_scale.x(),
        1.f / source_node->data.sublayer_scale.y());
    return success;
}

bool TransformTree::Are2DAxisAligned(int source_id, int dest_id) const
{
    gfx::Transform transform;
    return ComputeTransform(source_id, dest_id, &transform) && transform.Preserves2dAxisAlignment();
}

bool TransformTree::NeedsSourceToParentUpdate(TransformNode* node)
{
    return (source_to_parent_updates_allowed() && node->parent_id != node->data.source_node_id);
}

void TransformTree::UpdateTransforms(int id)
{
    TransformNode* node = Node(id);
    TransformNode* parent_node = parent(node);
    TransformNode* target_node = Node(node->data.target_id);
    if (node->data.needs_local_transform_update || NeedsSourceToParentUpdate(node))
        UpdateLocalTransform(node);
    else
        UndoSnapping(node);
    UpdateScreenSpaceTransform(node, parent_node, target_node);
    UpdateSublayerScale(node);
    UpdateTargetSpaceTransform(node, target_node);
    UpdateAnimationProperties(node, parent_node);
    UpdateSnapping(node);
    UpdateNodeAndAncestorsHaveIntegerTranslations(node, parent_node);
}

bool TransformTree::IsDescendant(int desc_id, int source_id) const
{
    while (desc_id != source_id) {
        if (desc_id < 0)
            return false;
        desc_id = Node(desc_id)->parent_id;
    }
    return true;
}

bool TransformTree::CombineTransformsBetween(int source_id,
    int dest_id,
    gfx::Transform* transform) const
{
    DCHECK(source_id > dest_id);
    const TransformNode* current = Node(source_id);
    const TransformNode* dest = Node(dest_id);
    // Combine transforms to and from the screen when possible. Since flattening
    // is a non-linear operation, we cannot use this approach when there is
    // non-trivial flattening between the source and destination nodes. For
    // example, consider the tree R->A->B->C, where B flattens its inherited
    // transform, and A has a non-flat transform. Suppose C is the source and A is
    // the destination. The expected result is C * B. But C's to_screen
    // transform is C * B * flattened(A * R), and A's from_screen transform is
    // R^{-1} * A^{-1}. If at least one of A and R isn't flat, the inverse of
    // flattened(A * R) won't be R^{-1} * A{-1}, so multiplying C's to_screen and
    // A's from_screen will not produce the correct result.
    if (!dest || (dest->data.ancestors_are_invertible && dest->data.node_and_ancestors_are_flat)) {
        transform->ConcatTransform(current->data.to_screen);
        if (dest)
            transform->ConcatTransform(dest->data.from_screen);
        return true;
    }

    // Flattening is defined in a way that requires it to be applied while
    // traversing downward in the tree. We first identify nodes that are on the
    // path from the source to the destination (this is traversing upward), and
    // then we visit these nodes in reverse order, flattening as needed. We
    // early-out if we get to a node whose target node is the destination, since
    // we can then re-use the target space transform stored at that node. However,
    // we cannot re-use a stored target space transform if the destination has a
    // zero sublayer scale, since stored target space transforms have sublayer
    // scale baked in, but we need to compute an unscaled transform.
    std::vector<int> source_to_destination;
    source_to_destination.push_back(current->id);
    current = parent(current);
    bool destination_has_non_zero_sublayer_scale = dest->data.sublayer_scale.x() != 0.f && dest->data.sublayer_scale.y() != 0.f;
    DCHECK(destination_has_non_zero_sublayer_scale || !dest->data.ancestors_are_invertible);
    for (; current && current->id > dest_id; current = parent(current)) {
        if (destination_has_non_zero_sublayer_scale && current->data.target_id == dest_id && current->data.content_target_id == dest_id)
            break;
        source_to_destination.push_back(current->id);
    }

    gfx::Transform combined_transform;
    if (current->id > dest_id) {
        combined_transform = current->data.to_target;
        // The stored target space transform has sublayer scale baked in, but we
        // need the unscaled transform.
        combined_transform.Scale(1.0f / dest->data.sublayer_scale.x(),
            1.0f / dest->data.sublayer_scale.y());
    } else if (current->id < dest_id) {
        // We have reached the lowest common ancestor of the source and destination
        // nodes. This case can occur when we are transforming between a node
        // corresponding to a fixed-position layer (or its descendant) and the node
        // corresponding to the layer's render target. For example, consider the
        // layer tree R->T->S->F where F is fixed-position, S owns a render surface,
        // and T has a significant transform. This will yield the following
        // transform tree:
        //    R
        //    |
        //    T
        //   /|
        //  S F
        // In this example, T will have id 2, S will have id 3, and F will have id
        // 4. When walking up the ancestor chain from F, the first node with a
        // smaller id than S will be T, the lowest common ancestor of these nodes.
        // We compute the transform from T to S here, and then from F to T in the
        // loop below.
        DCHECK(IsDescendant(dest_id, current->id));
        CombineInversesBetween(current->id, dest_id, &combined_transform);
        DCHECK(combined_transform.IsApproximatelyIdentityOrTranslation(
            SkDoubleToMScalar(1e-4)));
    }

    size_t source_to_destination_size = source_to_destination.size();
    for (size_t i = 0; i < source_to_destination_size; ++i) {
        size_t index = source_to_destination_size - 1 - i;
        const TransformNode* node = Node(source_to_destination[index]);
        if (node->data.flattens_inherited_transform)
            combined_transform.FlattenTo2d();
        combined_transform.PreconcatTransform(node->data.to_parent);
    }

    transform->ConcatTransform(combined_transform);
    return true;
}

bool TransformTree::CombineInversesBetween(int source_id,
    int dest_id,
    gfx::Transform* transform) const
{
    DCHECK(source_id < dest_id);
    const TransformNode* current = Node(dest_id);
    const TransformNode* dest = Node(source_id);
    // Just as in CombineTransformsBetween, we can use screen space transforms in
    // this computation only when there isn't any non-trivial flattening
    // involved.
    if (current->data.ancestors_are_invertible && current->data.node_and_ancestors_are_flat) {
        transform->PreconcatTransform(current->data.from_screen);
        if (dest)
            transform->PreconcatTransform(dest->data.to_screen);
        return true;
    }

    // Inverting a flattening is not equivalent to flattening an inverse. This
    // means we cannot, for example, use the inverse of each node's to_parent
    // transform, flattening where needed. Instead, we must compute the transform
    // from the destination to the source, with flattening, and then invert the
    // result.
    gfx::Transform dest_to_source;
    CombineTransformsBetween(dest_id, source_id, &dest_to_source);
    gfx::Transform source_to_dest;
    bool all_are_invertible = dest_to_source.GetInverse(&source_to_dest);
    transform->PreconcatTransform(source_to_dest);
    return all_are_invertible;
}

void TransformTree::UpdateLocalTransform(TransformNode* node)
{
    gfx::Transform transform = node->data.post_local;
    if (NeedsSourceToParentUpdate(node)) {
        gfx::Transform to_parent;
        ComputeTransform(node->data.source_node_id, node->parent_id, &to_parent);
        node->data.source_to_parent = to_parent.To2dTranslation();
    }

    gfx::Vector2dF fixed_position_adjustment;
    if (node->data.affected_by_inner_viewport_bounds_delta_x)
        fixed_position_adjustment.set_x(inner_viewport_bounds_delta_.x());
    else if (node->data.affected_by_outer_viewport_bounds_delta_x)
        fixed_position_adjustment.set_x(outer_viewport_bounds_delta_.x());

    if (node->data.affected_by_inner_viewport_bounds_delta_y)
        fixed_position_adjustment.set_y(inner_viewport_bounds_delta_.y());
    else if (node->data.affected_by_outer_viewport_bounds_delta_y)
        fixed_position_adjustment.set_y(outer_viewport_bounds_delta_.y());

    transform.Translate(
        node->data.source_to_parent.x() - node->data.scroll_offset.x() + fixed_position_adjustment.x(),
        node->data.source_to_parent.y() - node->data.scroll_offset.y() + fixed_position_adjustment.y());
    transform.PreconcatTransform(node->data.local);
    transform.PreconcatTransform(node->data.pre_local);
    node->data.set_to_parent(transform);
    node->data.needs_local_transform_update = false;
}

void TransformTree::UpdateScreenSpaceTransform(TransformNode* node,
    TransformNode* parent_node,
    TransformNode* target_node)
{
    if (!parent_node) {
        node->data.to_screen = node->data.to_parent;
        node->data.ancestors_are_invertible = true;
        node->data.to_screen_is_animated = false;
        node->data.node_and_ancestors_are_flat = node->data.to_parent.IsFlat();
    } else {
        node->data.to_screen = parent_node->data.to_screen;
        if (node->data.flattens_inherited_transform)
            node->data.to_screen.FlattenTo2d();
        node->data.to_screen.PreconcatTransform(node->data.to_parent);
        node->data.ancestors_are_invertible = parent_node->data.ancestors_are_invertible;
        node->data.node_and_ancestors_are_flat = parent_node->data.node_and_ancestors_are_flat && node->data.to_parent.IsFlat();
    }

    if (!node->data.to_screen.GetInverse(&node->data.from_screen))
        node->data.ancestors_are_invertible = false;
}

void TransformTree::UpdateSublayerScale(TransformNode* node)
{
    // The sublayer scale depends on the screen space transform, so update it too.
    node->data.sublayer_scale = node->data.needs_sublayer_scale
        ? MathUtil::ComputeTransform2dScaleComponents(
            node->data.to_screen, node->data.layer_scale_factor)
        : gfx::Vector2dF(1.0f, 1.0f);
}

void TransformTree::UpdateTargetSpaceTransform(TransformNode* node,
    TransformNode* target_node)
{
    if (node->data.needs_sublayer_scale) {
        node->data.to_target.MakeIdentity();
        node->data.to_target.Scale(node->data.sublayer_scale.x(),
            node->data.sublayer_scale.y());
    } else {
        // In order to include the root transform for the root surface, we walk up
        // to the root of the transform tree in ComputeTransform.
        int target_id = target_node->id;
        ComputeTransformWithDestinationSublayerScale(node->id, target_id,
            &node->data.to_target);
    }

    if (!node->data.to_target.GetInverse(&node->data.from_target))
        node->data.ancestors_are_invertible = false;
}

void TransformTree::UpdateAnimationProperties(TransformNode* node,
    TransformNode* parent_node)
{
    bool ancestor_is_animating = false;
    bool ancestor_is_animating_scale = false;
    float ancestor_maximum_target_scale = 0.f;
    float ancestor_starting_animation_scale = 0.f;
    if (parent_node) {
        ancestor_is_animating = parent_node->data.to_screen_is_animated;
        ancestor_is_animating_scale = parent_node->data.to_screen_has_scale_animation;
        ancestor_maximum_target_scale = parent_node->data.combined_maximum_animation_target_scale;
        ancestor_starting_animation_scale = parent_node->data.combined_starting_animation_scale;
    }
    node->data.to_screen_is_animated = node->data.is_animated || ancestor_is_animating;
    node->data.to_screen_has_scale_animation = !node->data.has_only_translation_animations || ancestor_is_animating_scale;

    // Once we've failed to compute a maximum animated scale at an ancestor, we
    // continue to fail.
    bool failed_at_ancestor = ancestor_is_animating_scale && ancestor_maximum_target_scale == 0.f;

    // Computing maximum animated scale in the presence of non-scale/translation
    // transforms isn't supported.
    bool failed_for_non_scale_or_translation = !node->data.to_target.IsScaleOrTranslation();

    // We don't attempt to accumulate animation scale from multiple nodes with
    // scale animations, because of the risk of significant overestimation. For
    // example, one node might be increasing scale from 1 to 10 at the same time
    // as another node is decreasing scale from 10 to 1. Naively combining these
    // scales would produce a scale of 100.
    bool failed_for_multiple_scale_animations = ancestor_is_animating_scale && !node->data.has_only_translation_animations;

    if (failed_at_ancestor || failed_for_non_scale_or_translation || failed_for_multiple_scale_animations) {
        node->data.combined_maximum_animation_target_scale = 0.f;
        node->data.combined_starting_animation_scale = 0.f;

        // This ensures that descendants know we've failed to compute a maximum
        // animated scale.
        node->data.to_screen_has_scale_animation = true;
        return;
    }

    if (!node->data.to_screen_has_scale_animation) {
        node->data.combined_maximum_animation_target_scale = 0.f;
        node->data.combined_starting_animation_scale = 0.f;
        return;
    }

    // At this point, we know exactly one of this node or an ancestor is animating
    // scale.
    if (node->data.has_only_translation_animations) {
        // An ancestor is animating scale.
        gfx::Vector2dF local_scales = MathUtil::ComputeTransform2dScaleComponents(node->data.local, 0.f);
        float max_local_scale = std::max(local_scales.x(), local_scales.y());
        node->data.combined_maximum_animation_target_scale = max_local_scale * ancestor_maximum_target_scale;
        node->data.combined_starting_animation_scale = max_local_scale * ancestor_starting_animation_scale;
        return;
    }

    if (node->data.local_starting_animation_scale == 0.f || node->data.local_maximum_animation_target_scale == 0.f) {
        node->data.combined_maximum_animation_target_scale = 0.f;
        node->data.combined_starting_animation_scale = 0.f;
        return;
    }

    gfx::Vector2dF ancestor_scales = parent_node ? MathUtil::ComputeTransform2dScaleComponents(
                                         parent_node->data.to_target, 0.f)
                                                 : gfx::Vector2dF(1.f, 1.f);
    float max_ancestor_scale = std::max(ancestor_scales.x(), ancestor_scales.y());
    node->data.combined_maximum_animation_target_scale = max_ancestor_scale * node->data.local_maximum_animation_target_scale;
    node->data.combined_starting_animation_scale = max_ancestor_scale * node->data.local_starting_animation_scale;
}

void TransformTree::UndoSnapping(TransformNode* node)
{
    // to_parent transform has the scroll snap from previous frame baked in.
    // We need to undo it and use the un-snapped transform to compute current
    // target and screen space transforms.
    node->data.to_parent.Translate(-node->data.scroll_snap.x(),
        -node->data.scroll_snap.y());
}

void TransformTree::UpdateSnapping(TransformNode* node)
{
    if (!node->data.scrolls || node->data.to_screen_is_animated || !node->data.to_target.IsScaleOrTranslation()) {
        return;
    }

    // Scroll snapping must be done in target space (the pixels we care about).
    // This means we effectively snap the target space transform. If TT is the
    // target space transform and TT' is TT with its translation components
    // rounded, then what we're after is the scroll delta X, where TT * X = TT'.
    // I.e., we want a transform that will realize our scroll snap. It follows
    // that X = TT^-1 * TT'. We cache TT and TT^-1 to make this more efficient.
    gfx::Transform rounded = node->data.to_target;
    rounded.RoundTranslationComponents();
    gfx::Transform delta = node->data.from_target;
    delta *= rounded;

    DCHECK(delta.IsApproximatelyIdentityOrTranslation(SkDoubleToMScalar(1e-4)))
        << delta.ToString();

    gfx::Vector2dF translation = delta.To2dTranslation();

    // Now that we have our scroll delta, we must apply it to each of our
    // combined, to/from matrices.
    node->data.to_target = rounded;
    node->data.to_parent.Translate(translation.x(), translation.y());
    node->data.from_target.matrix().postTranslate(-translation.x(),
        -translation.y(), 0);
    node->data.to_screen.Translate(translation.x(), translation.y());
    node->data.from_screen.matrix().postTranslate(-translation.x(),
        -translation.y(), 0);

    node->data.scroll_snap = translation;
}

void TransformTree::SetInnerViewportBoundsDelta(gfx::Vector2dF bounds_delta)
{
    if (inner_viewport_bounds_delta_ == bounds_delta)
        return;

    inner_viewport_bounds_delta_ = bounds_delta;

    if (nodes_affected_by_inner_viewport_bounds_delta_.empty())
        return;

    set_needs_update(true);
    for (int i : nodes_affected_by_inner_viewport_bounds_delta_)
        Node(i)->data.needs_local_transform_update = true;
}

void TransformTree::SetOuterViewportBoundsDelta(gfx::Vector2dF bounds_delta)
{
    if (outer_viewport_bounds_delta_ == bounds_delta)
        return;

    outer_viewport_bounds_delta_ = bounds_delta;

    if (nodes_affected_by_outer_viewport_bounds_delta_.empty())
        return;

    set_needs_update(true);
    for (int i : nodes_affected_by_outer_viewport_bounds_delta_)
        Node(i)->data.needs_local_transform_update = true;
}

void TransformTree::AddNodeAffectedByInnerViewportBoundsDelta(int node_id)
{
    nodes_affected_by_inner_viewport_bounds_delta_.push_back(node_id);
}

void TransformTree::AddNodeAffectedByOuterViewportBoundsDelta(int node_id)
{
    nodes_affected_by_outer_viewport_bounds_delta_.push_back(node_id);
}

bool TransformTree::HasNodesAffectedByInnerViewportBoundsDelta() const
{
    return !nodes_affected_by_inner_viewport_bounds_delta_.empty();
}

bool TransformTree::HasNodesAffectedByOuterViewportBoundsDelta() const
{
    return !nodes_affected_by_outer_viewport_bounds_delta_.empty();
}

void EffectTree::UpdateOpacities(int id)
{
    EffectNode* node = Node(id);
    node->data.screen_space_opacity = node->data.opacity;

    EffectNode* parent_node = parent(node);
    if (parent_node)
        node->data.screen_space_opacity *= parent_node->data.screen_space_opacity;
}

void TransformTree::UpdateNodeAndAncestorsHaveIntegerTranslations(
    TransformNode* node,
    TransformNode* parent_node)
{
    node->data.node_and_ancestors_have_only_integer_translation = node->data.to_parent.IsIdentityOrIntegerTranslation();
    if (parent_node)
        node->data.node_and_ancestors_have_only_integer_translation = node->data.node_and_ancestors_have_only_integer_translation && parent_node->data.node_and_ancestors_have_only_integer_translation;
}

void ClipTree::SetViewportClip(gfx::RectF viewport_rect)
{
    if (size() < 2)
        return;
    ClipNode* node = Node(1);
    if (viewport_rect == node->data.clip)
        return;
    node->data.clip = viewport_rect;
    set_needs_update(true);
}

gfx::RectF ClipTree::ViewportClip()
{
    const unsigned long min_size = 1;
    DCHECK_GT(size(), min_size);
    return Node(1)->data.clip;
}

PropertyTrees::PropertyTrees()
    : needs_rebuild(true)
    , sequence_number(0)
{
}

} // namespace cc
