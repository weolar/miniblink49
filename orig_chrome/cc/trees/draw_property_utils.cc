// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/draw_property_utils.h"

#include <vector>

#include "cc/base/math_util.h"
#include "cc/layers/draw_properties.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_impl.h"
#include "cc/layers/render_surface_draw_properties.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/property_tree.h"
#include "cc/trees/property_tree_builder.h"
#include "ui/gfx/geometry/rect_conversions.h"

namespace cc {

namespace {

    template <typename LayerType>
    void CalculateVisibleRects(const std::vector<LayerType*>& visible_layer_list,
        const ClipTree& clip_tree,
        const TransformTree& transform_tree)
    {
        for (auto& layer : visible_layer_list) {
            // TODO(ajuma): Compute content_scale rather than using it. Note that for
            // PictureLayer and PictureImageLayers, content_bounds == bounds and
            // content_scale_x == content_scale_y == 1.0, so once impl painting is on
            // everywhere, this code will be unnecessary.
            gfx::Size layer_bounds = layer->bounds();
            const bool has_clip = layer->clip_tree_index() > 0;
            const TransformNode* transform_node = transform_tree.Node(layer->transform_tree_index());
            if (has_clip) {
                const ClipNode* clip_node = clip_tree.Node(layer->clip_tree_index());
                const TransformNode* target_node = transform_tree.Node(transform_node->data.content_target_id);

                // The clip node stores clip rect in its target space. If required,
                // this clip rect should be mapped to the current layer's target space.
                gfx::Rect clip_rect_in_target_space;
                gfx::Rect combined_clip_rect_in_target_space;
                bool success = true;
                if (clip_node->data.target_id != target_node->id) {
                    gfx::Transform clip_to_target;
                    success = transform_tree.ComputeTransformWithDestinationSublayerScale(
                        clip_node->data.target_id, target_node->id, &clip_to_target);
                    if (!success) {
                        // An animated singular transform may become non-singular during the
                        // animation, so we still need to compute a visible rect. In this
                        // situation, we treat the entire layer as visible.
                        layer->set_visible_rect_from_property_trees(gfx::Rect(layer_bounds));
                        layer->set_clip_rect_in_target_space_from_property_trees(gfx::Rect());
                        continue;
                    }
                    DCHECK_LT(clip_node->data.target_id, target_node->id);
                    combined_clip_rect_in_target_space = gfx::ToEnclosingRect(MathUtil::ProjectClippedRect(
                        clip_to_target, clip_node->data.combined_clip_in_target_space));
                    clip_rect_in_target_space = gfx::ToEnclosingRect(MathUtil::ProjectClippedRect(
                        clip_to_target, clip_node->data.clip_in_target_space));
                } else {
                    clip_rect_in_target_space = gfx::ToEnclosingRect(clip_node->data.clip_in_target_space);
                    combined_clip_rect_in_target_space = gfx::ToEnclosingRect(clip_node->data.combined_clip_in_target_space);
                }

                if (!clip_rect_in_target_space.IsEmpty()) {
                    layer->set_clip_rect_in_target_space_from_property_trees(
                        clip_rect_in_target_space);
                } else {
                    layer->set_clip_rect_in_target_space_from_property_trees(gfx::Rect());
                }

                // The clip rect should be intersected with layer rect in target space.
                gfx::Transform content_to_target = transform_node->data.to_target;

                content_to_target.Translate(layer->offset_to_transform_parent().x(),
                    layer->offset_to_transform_parent().y());
                gfx::Rect layer_content_rect = gfx::Rect(layer_bounds);
                gfx::Rect layer_content_bounds_in_target_space = MathUtil::MapEnclosingClippedRect(content_to_target,
                    layer_content_rect);
                combined_clip_rect_in_target_space.Intersect(
                    layer_content_bounds_in_target_space);
                if (combined_clip_rect_in_target_space.IsEmpty()) {
                    layer->set_visible_rect_from_property_trees(gfx::Rect());
                    continue;
                }

                // If the layer is fully contained within the clip, treat it as fully
                // visible. Since clip_rect_in_target_space has already been intersected
                // with layer_content_bounds_in_target_space, the layer is fully contained
                // within the clip iff these rects are equal.
                if (combined_clip_rect_in_target_space == layer_content_bounds_in_target_space) {
                    layer->set_visible_rect_from_property_trees(gfx::Rect(layer_bounds));
                    continue;
                }

                gfx::Transform target_to_content;
                gfx::Transform target_to_layer;

                if (transform_node->data.ancestors_are_invertible) {
                    target_to_layer = transform_node->data.from_target;
                    success = true;
                } else {
                    success = transform_tree.ComputeTransformWithSourceSublayerScale(
                        target_node->id, transform_node->id, &target_to_layer);
                }

                if (!success) {
                    // An animated singular transform may become non-singular during the
                    // animation, so we still need to compute a visible rect. In this
                    // situation, we treat the entire layer as visible.
                    layer->set_visible_rect_from_property_trees(gfx::Rect(layer_bounds));
                    continue;
                }

                target_to_content.Translate(-layer->offset_to_transform_parent().x(),
                    -layer->offset_to_transform_parent().y());
                target_to_content.PreconcatTransform(target_to_layer);

                gfx::Rect visible_rect = MathUtil::ProjectEnclosingClippedRect(
                    target_to_content, combined_clip_rect_in_target_space);

                visible_rect.Intersect(gfx::Rect(layer_bounds));

                layer->set_visible_rect_from_property_trees(visible_rect);
            } else {
                layer->set_visible_rect_from_property_trees(gfx::Rect(layer_bounds));
                layer->set_clip_rect_in_target_space_from_property_trees(
                    gfx::Rect(layer_bounds));
            }
        }
    }

    template <typename LayerType>
    static bool IsRootLayerOfNewRenderingContext(LayerType* layer)
    {
        if (layer->parent())
            return !layer->parent()->Is3dSorted() && layer->Is3dSorted();
        return layer->Is3dSorted();
    }

    template <typename LayerType>
    static inline bool LayerIsInExisting3DRenderingContext(LayerType* layer)
    {
        return layer->Is3dSorted() && layer->parent() && layer->parent()->Is3dSorted() && layer->parent()->sorting_context_id() == layer->sorting_context_id();
    }

    template <typename LayerType>
    static bool TransformToScreenIsKnown(LayerType* layer,
        const TransformTree& tree)
    {
        const TransformNode* node = tree.Node(layer->transform_tree_index());
        return !node->data.to_screen_is_animated;
    }

    template <typename LayerType>
    static bool HasSingularTransform(LayerType* layer, const TransformTree& tree)
    {
        const TransformNode* node = tree.Node(layer->transform_tree_index());
        return !node->data.is_invertible || !node->data.ancestors_are_invertible;
    }

    template <typename LayerType>
    static bool IsLayerBackFaceVisible(LayerType* layer,
        const TransformTree& tree)
    {
        // The current W3C spec on CSS transforms says that backface visibility should
        // be determined differently depending on whether the layer is in a "3d
        // rendering context" or not. For Chromium code, we can determine whether we
        // are in a 3d rendering context by checking if the parent preserves 3d.

        if (LayerIsInExisting3DRenderingContext(layer))
            return DrawTransformFromPropertyTrees(layer, tree).IsBackFaceVisible();

        // In this case, either the layer establishes a new 3d rendering context, or
        // is not in a 3d rendering context at all.
        return layer->transform().IsBackFaceVisible();
    }

    template <typename LayerType>
    static bool IsSurfaceBackFaceVisible(LayerType* layer,
        const TransformTree& tree)
    {
        if (LayerIsInExisting3DRenderingContext(layer)) {
            const TransformNode* node = tree.Node(layer->transform_tree_index());
            // Draw transform as a contributing render surface.
            // TODO(enne): we shouldn't walk the tree during a tree walk.
            gfx::Transform surface_draw_transform;
            tree.ComputeTransform(node->id, node->data.target_id,
                &surface_draw_transform);
            return surface_draw_transform.IsBackFaceVisible();
        }

        if (IsRootLayerOfNewRenderingContext(layer))
            return layer->transform().IsBackFaceVisible();

        // If the render_surface is not part of a new or existing rendering context,
        // then the layers that contribute to this surface will decide back-face
        // visibility for themselves.
        return false;
    }

    template <typename LayerType>
    static bool IsAnimatingTransformToScreen(LayerType* layer,
        const TransformTree& tree)
    {
        const TransformNode* node = tree.Node(layer->transform_tree_index());
        return node->data.to_screen_is_animated;
    }

    static inline bool TransformToScreenIsKnown(Layer* layer,
        const TransformTree& tree)
    {
        return !IsAnimatingTransformToScreen(layer, tree);
    }

    static inline bool TransformToScreenIsKnown(LayerImpl* layer,
        const TransformTree& tree)
    {
        return true;
    }

    template <typename LayerType>
    static bool HasInvertibleOrAnimatedTransform(LayerType* layer)
    {
        return layer->transform_is_invertible() || layer->HasPotentiallyRunningTransformAnimation();
    }

    static inline bool SubtreeShouldBeSkipped(LayerImpl* layer,
        bool layer_is_drawn,
        const TransformTree& tree)
    {
        // If the layer transform is not invertible, it should not be drawn.
        // TODO(ajuma): Correctly process subtrees with singular transform for the
        // case where we may animate to a non-singular transform and wish to
        // pre-raster.
        if (!HasInvertibleOrAnimatedTransform(layer))
            return true;

        // When we need to do a readback/copy of a layer's output, we can not skip
        // it or any of its ancestors.
        if (layer->num_layer_or_descendants_with_copy_request() > 0)
            return false;

        // We cannot skip the the subtree if a descendant has a wheel or touch handler
        // or the hit testing code will break (it requires fresh transforms, etc).
        // Though we don't need visible rect for hit testing, we need render surface's
        // drawable content rect which depends on layer's drawable content rect which
        // in turn depends on layer's clip rect that is computed while computing
        // visible rects.
        if (layer->layer_or_descendant_has_input_handler())
            return false;

        // If the layer is not drawn, then skip it and its subtree.
        if (!layer_is_drawn)
            return true;

        if (layer->render_surface() && !layer->double_sided() && IsSurfaceBackFaceVisible(layer, tree))
            return true;

        // If layer is on the pending tree and opacity is being animated then
        // this subtree can't be skipped as we need to create, prioritize and
        // include tiles for this layer when deciding if tree can be activated.
        if (layer->layer_tree_impl()->IsPendingTree() && layer->HasPotentiallyRunningOpacityAnimation())
            return false;

        // If layer has a background filter, don't skip the layer, even it the
        // opacity is 0.
        if (!layer->background_filters().IsEmpty())
            return false;

        // The opacity of a layer always applies to its children (either implicitly
        // via a render surface or explicitly if the parent preserves 3D), so the
        // entire subtree can be skipped if this layer is fully transparent.
        return !layer->opacity();
    }

    static inline bool SubtreeShouldBeSkipped(Layer* layer,
        bool layer_is_drawn,
        const TransformTree& tree)
    {
        // If the layer transform is not invertible, it should not be drawn.
        if (!layer->transform_is_invertible() && !layer->HasPotentiallyRunningTransformAnimation())
            return true;

        // When we need to do a readback/copy of a layer's output, we can not skip
        // it or any of its ancestors.
        if (layer->num_layer_or_descendants_with_copy_request() > 0)
            return false;

        // If the layer is not drawn, then skip it and its subtree.
        if (!layer_is_drawn)
            return true;

        if (layer->has_render_surface() && !layer->double_sided() && !layer->HasPotentiallyRunningTransformAnimation() && IsSurfaceBackFaceVisible(layer, tree))
            return true;

        // If layer has a background filter, don't skip the layer, even it the
        // opacity is 0.
        if (!layer->background_filters().IsEmpty())
            return false;

        // If the opacity is being animated then the opacity on the main thread is
        // unreliable (since the impl thread may be using a different opacity), so it
        // should not be trusted.
        // In particular, it should not cause the subtree to be skipped.
        // Similarly, for layers that might animate opacity using an impl-only
        // animation, their subtree should also not be skipped.
        return !layer->opacity() && !layer->HasPotentiallyRunningOpacityAnimation() && !layer->OpacityCanAnimateOnImplThread();
    }

    template <typename LayerType>
    static bool LayerShouldBeSkipped(LayerType* layer,
        bool layer_is_drawn,
        const TransformTree& tree)
    {
        // Layers can be skipped if any of these conditions are met.
        //   - is not drawn due to it or one of its ancestors being hidden (or having
        //     no copy requests).
        //   - does not draw content.
        //   - is transparent.
        //   - has empty bounds
        //   - the layer is not double-sided, but its back face is visible.
        //
        // Some additional conditions need to be computed at a later point after the
        // recursion is finished.
        //   - the intersection of render_surface content and layer clip_rect is empty
        //   - the visible_layer_rect is empty
        //
        // Note, if the layer should not have been drawn due to being fully
        // transparent, we would have skipped the entire subtree and never made it
        // into this function, so it is safe to omit this check here.
        if (!layer_is_drawn)
            return true;

        if (!layer->DrawsContent() || layer->bounds().IsEmpty())
            return true;

        LayerType* backface_test_layer = layer;
        if (layer->use_parent_backface_visibility()) {
            DCHECK(layer->parent());
            DCHECK(!layer->parent()->use_parent_backface_visibility());
            backface_test_layer = layer->parent();
        }

        // The layer should not be drawn if (1) it is not double-sided and (2) the
        // back of the layer is known to be facing the screen.
        if (!backface_test_layer->double_sided() && TransformToScreenIsKnown(backface_test_layer, tree) && IsLayerBackFaceVisible(backface_test_layer, tree))
            return true;

        return false;
    }

    template <typename LayerType>
    void FindLayersThatNeedUpdates(
        LayerType* layer,
        const TransformTree& tree,
        bool subtree_is_visible_from_ancestor,
        typename LayerType::LayerListType* update_layer_list,
        std::vector<LayerType*>* visible_layer_list)
    {
        bool layer_is_drawn = layer->HasCopyRequest() || (subtree_is_visible_from_ancestor && !layer->hide_layer_and_subtree());

        if (layer->parent() && SubtreeShouldBeSkipped(layer, layer_is_drawn, tree))
            return;

        if (!LayerShouldBeSkipped(layer, layer_is_drawn, tree)) {
            visible_layer_list->push_back(layer);
            update_layer_list->push_back(layer);
        }

        // Append mask layers to the update layer list.  They don't have valid visible
        // rects, so need to get added after the above calculation.  Replica layers
        // don't need to be updated.
        if (LayerType* mask_layer = layer->mask_layer())
            update_layer_list->push_back(mask_layer);
        if (LayerType* replica_layer = layer->replica_layer()) {
            if (LayerType* mask_layer = replica_layer->mask_layer())
                update_layer_list->push_back(mask_layer);
        }

        for (size_t i = 0; i < layer->children().size(); ++i) {
            FindLayersThatNeedUpdates(layer->child_at(i), tree, layer_is_drawn,
                update_layer_list, visible_layer_list);
        }
    }

} // namespace

void ComputeClips(ClipTree* clip_tree, const TransformTree& transform_tree)
{
    if (!clip_tree->needs_update())
        return;
    for (int i = 0; i < static_cast<int>(clip_tree->size()); ++i) {
        ClipNode* clip_node = clip_tree->Node(i);

        if (clip_node->id == 0) {
            clip_node->data.combined_clip_in_target_space = clip_node->data.clip;
            clip_node->data.clip_in_target_space = clip_node->data.clip;
            continue;
        }
        const TransformNode* transform_node = transform_tree.Node(clip_node->data.transform_id);

        // Only descendants of a real clipping layer (i.e., not 0) may have their
        // clip adjusted due to intersecting with an ancestor clip.
        const bool is_clipped = clip_node->parent_id > 0;
        if (!is_clipped) {
            clip_node->data.clip_in_target_space = MathUtil::MapClippedRect(
                transform_node->data.to_target, clip_node->data.clip);
            clip_node->data.combined_clip_in_target_space = clip_node->data.clip_in_target_space;
            continue;
        }

        ClipNode* parent_clip_node = clip_tree->parent(clip_node);
        gfx::Transform parent_to_current;
        const TransformNode* parent_transform_node = transform_tree.Node(parent_clip_node->data.transform_id);
        bool success = true;

        // Clips must be combined in target space. We cannot, for example, combine
        // clips in the space of the child clip. The reason is non-affine
        // transforms. Say we have the following tree T->A->B->C, and B clips C, but
        // draw into target T. It may be the case that A applies a perspective
        // transform, and B and C are at different z positions. When projected into
        // target space, the relative sizes and positions of B and C can shift.
        // Since it's the relationship in target space that matters, that's where we
        // must combine clips. For each clip node, we save the clip rects in its
        // target space. So, we need to get the ancestor clip rect in the current
        // clip node's target space.
        gfx::RectF parent_combined_clip_in_target_space = parent_clip_node->data.combined_clip_in_target_space;
        if (parent_clip_node->data.target_id != clip_node->data.target_id) {
            success &= transform_tree.ComputeTransformWithDestinationSublayerScale(
                parent_clip_node->data.target_id, clip_node->data.target_id,
                &parent_to_current);
            if (parent_transform_node->data.sublayer_scale.x() > 0 && parent_transform_node->data.sublayer_scale.y() > 0)
                parent_to_current.Scale(
                    1.f / parent_transform_node->data.sublayer_scale.x(),
                    1.f / parent_transform_node->data.sublayer_scale.y());
            // If we can't compute a transform, it's because we had to use the inverse
            // of a singular transform. We won't draw in this case, so there's no need
            // to compute clips.
            if (!success)
                continue;
            parent_combined_clip_in_target_space = MathUtil::ProjectClippedRect(
                parent_to_current,
                parent_clip_node->data.combined_clip_in_target_space);
        }

        if (clip_node->data.use_only_parent_clip) {
            clip_node->data.combined_clip_in_target_space = parent_combined_clip_in_target_space;
            if (!clip_node->data.render_surface_is_clipped) {
                clip_node->data.clip_in_target_space = parent_combined_clip_in_target_space;
            } else {
                // Render Surface applies clip and the owning layer itself applies
                // no clip. So, clip_in_target_space is not used and hence we can set
                // it to an empty rect.
                clip_node->data.clip_in_target_space = gfx::RectF();
            }
        } else {
            gfx::Transform source_to_target;

            if (transform_node->data.content_target_id == clip_node->data.target_id) {
                source_to_target = transform_node->data.to_target;
            } else {
                success = transform_tree.ComputeTransformWithDestinationSublayerScale(
                    transform_node->id, clip_node->data.target_id, &source_to_target);
                // source_to_target computation should be successful as target is an
                // ancestor of the transform node.
                DCHECK(success);
            }

            gfx::RectF source_clip_in_target_space = MathUtil::MapClippedRect(source_to_target, clip_node->data.clip);

            if (!clip_node->data.layer_clipping_uses_only_local_clip) {
                gfx::RectF parent_clip_in_target_space = MathUtil::ProjectClippedRect(
                    parent_to_current, parent_clip_node->data.clip_in_target_space);
                clip_node->data.clip_in_target_space = gfx::IntersectRects(
                    parent_clip_in_target_space, source_clip_in_target_space);
            } else {
                clip_node->data.clip_in_target_space = source_clip_in_target_space;
            }

            if (clip_node->data.layer_visibility_uses_only_local_clip) {
                clip_node->data.combined_clip_in_target_space = source_clip_in_target_space;
            } else {
                clip_node->data.combined_clip_in_target_space = gfx::IntersectRects(
                    parent_combined_clip_in_target_space, source_clip_in_target_space);
            }
        }
    }
    clip_tree->set_needs_update(false);
}

void ComputeTransforms(TransformTree* transform_tree)
{
    if (!transform_tree->needs_update())
        return;
    for (int i = 1; i < static_cast<int>(transform_tree->size()); ++i)
        transform_tree->UpdateTransforms(i);
    transform_tree->set_needs_update(false);
}

void ComputeOpacities(EffectTree* effect_tree)
{
    if (!effect_tree->needs_update())
        return;
    for (int i = 1; i < static_cast<int>(effect_tree->size()); ++i)
        effect_tree->UpdateOpacities(i);
    effect_tree->set_needs_update(false);
}

template <typename LayerType>
void ComputeVisibleRectsUsingPropertyTreesInternal(
    LayerType* root_layer,
    PropertyTrees* property_trees,
    typename LayerType::LayerListType* update_layer_list)
{
    if (property_trees->transform_tree.needs_update())
        property_trees->clip_tree.set_needs_update(true);
    ComputeTransforms(&property_trees->transform_tree);
    ComputeClips(&property_trees->clip_tree, property_trees->transform_tree);
    ComputeOpacities(&property_trees->effect_tree);

    const bool subtree_is_visible_from_ancestor = true;
    std::vector<LayerType*> visible_layer_list;
    FindLayersThatNeedUpdates(root_layer, property_trees->transform_tree,
        subtree_is_visible_from_ancestor, update_layer_list,
        &visible_layer_list);
    CalculateVisibleRects<LayerType>(visible_layer_list,
        property_trees->clip_tree,
        property_trees->transform_tree);
}

void BuildPropertyTreesAndComputeVisibleRects(
    Layer* root_layer,
    const Layer* page_scale_layer,
    const Layer* inner_viewport_scroll_layer,
    const Layer* outer_viewport_scroll_layer,
    float page_scale_factor,
    float device_scale_factor,
    const gfx::Rect& viewport,
    const gfx::Transform& device_transform,
    PropertyTrees* property_trees,
    LayerList* update_layer_list)
{
    PropertyTreeBuilder::BuildPropertyTrees(
        root_layer, page_scale_layer, inner_viewport_scroll_layer,
        outer_viewport_scroll_layer, page_scale_factor, device_scale_factor,
        viewport, device_transform, property_trees);
    ComputeVisibleRectsUsingPropertyTrees(root_layer, property_trees,
        update_layer_list);
}

void BuildPropertyTreesAndComputeVisibleRects(
    LayerImpl* root_layer,
    const LayerImpl* page_scale_layer,
    const LayerImpl* inner_viewport_scroll_layer,
    const LayerImpl* outer_viewport_scroll_layer,
    float page_scale_factor,
    float device_scale_factor,
    const gfx::Rect& viewport,
    const gfx::Transform& device_transform,
    PropertyTrees* property_trees,
    LayerImplList* update_layer_list)
{
    PropertyTreeBuilder::BuildPropertyTrees(
        root_layer, page_scale_layer, inner_viewport_scroll_layer,
        outer_viewport_scroll_layer, page_scale_factor, device_scale_factor,
        viewport, device_transform, property_trees);
    ComputeVisibleRectsUsingPropertyTrees(root_layer, property_trees,
        update_layer_list);
}

void ComputeVisibleRectsUsingPropertyTrees(Layer* root_layer,
    PropertyTrees* property_trees,
    LayerList* update_layer_list)
{
    ComputeVisibleRectsUsingPropertyTreesInternal(root_layer, property_trees,
        update_layer_list);
}

void ComputeVisibleRectsUsingPropertyTrees(LayerImpl* root_layer,
    PropertyTrees* property_trees,
    LayerImplList* update_layer_list)
{
    ComputeVisibleRectsUsingPropertyTreesInternal(root_layer, property_trees,
        update_layer_list);
}

template <typename LayerType>
gfx::Transform DrawTransformFromPropertyTreesInternal(
    const LayerType* layer,
    const TransformNode* node)
{
    gfx::Transform xform;
    const bool owns_non_root_surface = layer->parent() && layer->has_render_surface();
    if (!owns_non_root_surface) {
        // If you're not the root, or you don't own a surface, you need to apply
        // your local offset.
        xform = node->data.to_target;
        if (layer->should_flatten_transform_from_property_tree())
            xform.FlattenTo2d();
        xform.Translate(layer->offset_to_transform_parent().x(),
            layer->offset_to_transform_parent().y());
    } else {
        // Surfaces need to apply their sublayer scale.
        xform.Scale(node->data.sublayer_scale.x(), node->data.sublayer_scale.y());
    }
    return xform;
}

gfx::Transform DrawTransformFromPropertyTrees(const Layer* layer,
    const TransformTree& tree)
{
    return DrawTransformFromPropertyTreesInternal(
        layer, tree.Node(layer->transform_tree_index()));
}

gfx::Transform DrawTransformFromPropertyTrees(const LayerImpl* layer,
    const TransformTree& tree)
{
    return DrawTransformFromPropertyTreesInternal(
        layer, tree.Node(layer->transform_tree_index()));
}

gfx::Transform SurfaceDrawTransform(const RenderSurfaceImpl* render_surface,
    const TransformTree& tree)
{
    const TransformNode* node = tree.Node(render_surface->TransformTreeIndex());
    gfx::Transform render_surface_transform;
    // The draw transform of root render surface is identity tranform.
    if (node->id == 1)
        return render_surface_transform;
    const TransformNode* target_node = tree.Node(node->data.target_id);
    tree.ComputeTransformWithDestinationSublayerScale(node->id, target_node->id,
        &render_surface_transform);
    if (node->data.sublayer_scale.x() != 0.0 && node->data.sublayer_scale.y() != 0.0)
        render_surface_transform.Scale(1.0 / node->data.sublayer_scale.x(),
            1.0 / node->data.sublayer_scale.y());
    return render_surface_transform;
}

bool SurfaceIsClipped(const RenderSurfaceImpl* render_surface,
    const ClipNode* clip_node)
{
    // If the render surface's owning layer doesn't form a clip node, it is not
    // clipped.
    if (render_surface->OwningLayerId() != clip_node->owner_id)
        return false;
    return clip_node->data.render_surface_is_clipped;
}

gfx::Rect SurfaceClipRect(const RenderSurfaceImpl* render_surface,
    const ClipNode* parent_clip_node,
    bool is_clipped)
{
    if (!is_clipped)
        return gfx::Rect();
    return gfx::ToEnclosingRect(parent_clip_node->data.clip_in_target_space);
}

gfx::Transform SurfaceScreenSpaceTransform(
    const RenderSurfaceImpl* render_surface,
    const TransformTree& tree)
{
    const TransformNode* node = tree.Node(render_surface->TransformTreeIndex());
    gfx::Transform screen_space_transform;
    // The screen space transform of root render surface is identity tranform.
    if (node->id == 1)
        return screen_space_transform;
    screen_space_transform = node->data.to_screen;
    if (node->data.sublayer_scale.x() != 0.0 && node->data.sublayer_scale.y() != 0.0)
        screen_space_transform.Scale(1.0 / node->data.sublayer_scale.x(),
            1.0 / node->data.sublayer_scale.y());
    return screen_space_transform;
}

template <typename LayerType>
gfx::Transform ScreenSpaceTransformFromPropertyTreesInternal(
    LayerType* layer,
    const TransformNode* node)
{
    gfx::Transform xform(1, 0, 0, 1, layer->offset_to_transform_parent().x(),
        layer->offset_to_transform_parent().y());
    gfx::Transform ssxform = node->data.to_screen;
    xform.ConcatTransform(ssxform);
    if (layer->should_flatten_transform_from_property_tree())
        xform.FlattenTo2d();
    return xform;
}

gfx::Transform ScreenSpaceTransformFromPropertyTrees(
    const Layer* layer,
    const TransformTree& tree)
{
    return ScreenSpaceTransformFromPropertyTreesInternal(
        layer, tree.Node(layer->transform_tree_index()));
}

gfx::Transform ScreenSpaceTransformFromPropertyTrees(
    const LayerImpl* layer,
    const TransformTree& tree)
{
    return ScreenSpaceTransformFromPropertyTreesInternal(
        layer, tree.Node(layer->transform_tree_index()));
}

float LayerDrawOpacity(const LayerImpl* layer, const EffectTree& tree)
{
    if (!layer->render_target())
        return 0.f;

    const EffectNode* target_node = tree.Node(layer->render_target()->effect_tree_index());
    const EffectNode* node = tree.Node(layer->effect_tree_index());
    if (node == target_node)
        return 1.f;

    float draw_opacity = 1.f;
    while (node != target_node) {
        draw_opacity *= node->data.opacity;
        node = tree.parent(node);
    }
    return draw_opacity;
}

float SurfaceDrawOpacity(RenderSurfaceImpl* render_surface,
    const EffectTree& tree)
{
    const EffectNode* node = tree.Node(render_surface->EffectTreeIndex());
    float target_opacity_tree_index = render_surface->TargetEffectTreeIndex();
    if (target_opacity_tree_index < 0)
        return node->data.screen_space_opacity;
    const EffectNode* target_node = tree.Node(target_opacity_tree_index);
    float draw_opacity = 1.f;
    while (node != target_node) {
        draw_opacity *= node->data.opacity;
        node = tree.parent(node);
    }
    return draw_opacity;
}

bool LayerCanUseLcdText(const LayerImpl* layer,
    bool layers_always_allowed_lcd_text,
    bool can_use_lcd_text,
    const TransformNode* transform_node,
    const EffectNode* effect_node)
{
    if (layers_always_allowed_lcd_text)
        return true;
    if (!can_use_lcd_text)
        return false;
    if (!layer->contents_opaque())
        return false;

    if (effect_node->data.screen_space_opacity != 1.f)
        return false;
    if (!transform_node->data.node_and_ancestors_have_only_integer_translation)
        return false;
    if (static_cast<int>(layer->offset_to_transform_parent().x()) != layer->offset_to_transform_parent().x())
        return false;
    if (static_cast<int>(layer->offset_to_transform_parent().y()) != layer->offset_to_transform_parent().y())
        return false;
    return true;
}

gfx::Rect LayerDrawableContentRect(
    const LayerImpl* layer,
    const gfx::Rect& layer_bounds_in_target_space,
    const gfx::Rect& clip_rect)
{
    if (layer->is_clipped())
        return IntersectRects(layer_bounds_in_target_space, clip_rect);

    return layer_bounds_in_target_space;
}

gfx::Transform ReplicaToSurfaceTransform(
    const RenderSurfaceImpl* render_surface,
    const TransformTree& tree)
{
    gfx::Transform replica_to_surface;
    if (!render_surface->HasReplica())
        return replica_to_surface;
    const LayerImpl* replica_layer = render_surface->ReplicaLayer();
    const TransformNode* surface_transform_node = tree.Node(render_surface->TransformTreeIndex());
    replica_to_surface.Scale(surface_transform_node->data.sublayer_scale.x(),
        surface_transform_node->data.sublayer_scale.y());
    replica_to_surface.Translate(replica_layer->offset_to_transform_parent().x(),
        replica_layer->offset_to_transform_parent().y());
    gfx::Transform replica_transform_node_to_surface;
    tree.ComputeTransform(replica_layer->transform_tree_index(),
        render_surface->TransformTreeIndex(),
        &replica_transform_node_to_surface);
    replica_to_surface.PreconcatTransform(replica_transform_node_to_surface);
    if (surface_transform_node->data.sublayer_scale.x() != 0 && surface_transform_node->data.sublayer_scale.y() != 0) {
        replica_to_surface.Scale(
            1.0 / surface_transform_node->data.sublayer_scale.x(),
            1.0 / surface_transform_node->data.sublayer_scale.y());
    }
    return replica_to_surface;
}

gfx::Rect LayerClipRect(const LayerImpl* layer,
    const gfx::Rect& layer_bounds_in_target_space)
{
    if (layer->is_clipped())
        return layer->clip_rect_in_target_space_from_property_trees();

    return layer_bounds_in_target_space;
}

void ComputeLayerDrawPropertiesUsingPropertyTrees(
    const LayerImpl* layer,
    const PropertyTrees* property_trees,
    bool layers_always_allowed_lcd_text,
    bool can_use_lcd_text,
    DrawProperties* draw_properties)
{
    draw_properties->visible_layer_rect = layer->visible_rect_from_property_trees();

    const TransformNode* transform_node = property_trees->transform_tree.Node(layer->transform_tree_index());
    const EffectNode* effect_node = property_trees->effect_tree.Node(layer->effect_tree_index());
    const ClipNode* clip_node = property_trees->clip_tree.Node(layer->clip_tree_index());

    draw_properties->target_space_transform = DrawTransformFromPropertyTreesInternal(layer, transform_node);
    draw_properties->screen_space_transform = ScreenSpaceTransformFromPropertyTreesInternal(layer, transform_node);
    draw_properties->screen_space_transform_is_animating = transform_node->data.to_screen_is_animated;
    if (layer->layer_tree_impl()
            ->settings()
            .layer_transforms_should_scale_layer_contents) {
        draw_properties->maximum_animation_contents_scale = transform_node->data.combined_maximum_animation_target_scale;
        draw_properties->starting_animation_contents_scale = transform_node->data.combined_starting_animation_scale;
    } else {
        draw_properties->maximum_animation_contents_scale = 0.f;
        draw_properties->starting_animation_contents_scale = 0.f;
    }

    draw_properties->opacity = LayerDrawOpacity(layer, property_trees->effect_tree);
    draw_properties->can_use_lcd_text = LayerCanUseLcdText(layer, layers_always_allowed_lcd_text,
        can_use_lcd_text, transform_node, effect_node);
    draw_properties->is_clipped = clip_node->data.layers_are_clipped;

    gfx::Rect bounds_in_target_space = MathUtil::MapEnclosingClippedRect(
        draw_properties->target_space_transform, gfx::Rect(layer->bounds()));
    draw_properties->clip_rect = LayerClipRect(layer, bounds_in_target_space);
    draw_properties->drawable_content_rect = LayerDrawableContentRect(
        layer, bounds_in_target_space, draw_properties->clip_rect);
}

void ComputeSurfaceDrawPropertiesUsingPropertyTrees(
    RenderSurfaceImpl* render_surface,
    const PropertyTrees* property_trees,
    RenderSurfaceDrawProperties* draw_properties)
{
    const ClipNode* clip_node = property_trees->clip_tree.Node(render_surface->ClipTreeIndex());

    draw_properties->is_clipped = SurfaceIsClipped(render_surface, clip_node);
    draw_properties->draw_opacity = SurfaceDrawOpacity(render_surface, property_trees->effect_tree);
    draw_properties->draw_transform = SurfaceDrawTransform(render_surface, property_trees->transform_tree);
    draw_properties->screen_space_transform = SurfaceScreenSpaceTransform(
        render_surface, property_trees->transform_tree);

    if (render_surface->HasReplica()) {
        gfx::Transform replica_to_surface = ReplicaToSurfaceTransform(
            render_surface, property_trees->transform_tree);
        draw_properties->replica_draw_transform = draw_properties->draw_transform * replica_to_surface;
        draw_properties->replica_screen_space_transform = draw_properties->screen_space_transform * replica_to_surface;
    } else {
        draw_properties->replica_draw_transform.MakeIdentity();
        draw_properties->replica_screen_space_transform.MakeIdentity();
    }

    draw_properties->clip_rect = SurfaceClipRect(
        render_surface, property_trees->clip_tree.parent(clip_node),
        draw_properties->is_clipped);
}

} // namespace cc
