// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/layer_utils.h"

#include "cc/layers/layer_impl.h"
#include "cc/trees/layer_tree_host_common.h"
#include "ui/gfx/geometry/box_f.h"

namespace cc {

namespace {

    bool HasAnimationThatInflatesBounds(const LayerImpl& layer)
    {
        return layer.HasAnimationThatInflatesBounds();
    }

    bool HasFilterAnimationThatInflatesBounds(const LayerImpl& layer)
    {
        return layer.HasFilterAnimationThatInflatesBounds();
    }

    bool HasTransformAnimationThatInflatesBounds(const LayerImpl& layer)
    {
        return layer.HasTransformAnimationThatInflatesBounds();
    }

    inline bool HasAncestorTransformAnimation(const LayerImpl& layer)
    {
        return layer.screen_space_transform_is_animating();
    }

    inline bool HasAncestorFilterAnimation(const LayerImpl& layer)
    {
        for (const LayerImpl* current = &layer; current;
             current = current->parent()) {
            if (HasFilterAnimationThatInflatesBounds(*current))
                return true;
        }

        return false;
    }

} // namespace

bool LayerUtils::GetAnimationBounds(const LayerImpl& layer_in, gfx::BoxF* out)
{
    // We don't care about animated bounds for invisible layers.
    if (!layer_in.DrawsContent())
        return false;

    // We also don't care for layers that are not animated or a child of an
    // animated layer.
    if (!HasAncestorTransformAnimation(layer_in) && !HasAncestorFilterAnimation(layer_in))
        return false;

    // To compute the inflated bounds for a layer, we start by taking its bounds
    // and converting it to a 3d box, and then we transform or inflate it
    // repeatedly as we walk up the layer tree to the root.
    //
    // At each layer we apply the following transformations to the box:
    //   1) We translate so that the anchor point is the origin.
    //   2) We either apply the layer's transform or inflate if the layer's
    //      transform is animated.
    //   3) We undo the translation from step 1 and apply a second translation
    //      to account for the layer's position.
    //
    gfx::BoxF box(layer_in.bounds().width(), layer_in.bounds().height(), 0.f);

    // We want to inflate/transform the box as few times as possible. Each time
    // we do this, we have to make the box axis aligned again, so if we make many
    // small adjustments to the box by transforming it repeatedly rather than
    // once by the product of all these matrices, we will accumulate a bunch of
    // unnecessary inflation because of the the many axis-alignment fixes. This
    // matrix stores said product.
    gfx::Transform coalesced_transform;

    for (const LayerImpl* layer = &layer_in; layer; layer = layer->parent()) {
        int transform_origin_x = layer->transform_origin().x();
        int transform_origin_y = layer->transform_origin().y();
        int transform_origin_z = layer->transform_origin().z();

        gfx::PointF position = layer->position();
        if (layer->parent() && !HasAnimationThatInflatesBounds(*layer)) {
            // |composite_layer_transform| contains 1 - 4 mentioned above. We compute
            // it separately and apply afterwards because it's a bit more efficient
            // because post-multiplication appears a bit more expensive, so we want
            // to do it only once.
            gfx::Transform composite_layer_transform;

            composite_layer_transform.Translate3d(transform_origin_x + position.x(),
                transform_origin_y + position.y(),
                transform_origin_z);
            composite_layer_transform.PreconcatTransform(layer->transform());
            composite_layer_transform.Translate3d(
                -transform_origin_x, -transform_origin_y, -transform_origin_z);

            // Add this layer's contributions to the |coalesced_transform|.
            coalesced_transform.ConcatTransform(composite_layer_transform);
            continue;
        }

        // First, apply coalesced transform we've been building and reset it.
        coalesced_transform.TransformBox(&box);
        coalesced_transform.MakeIdentity();

        // We need to apply the inflation about the layer's anchor point. Rather
        // than doing this via transforms, we'll just shift the box directly.
        box.set_origin(box.origin() + gfx::Vector3dF(-transform_origin_x, -transform_origin_y, -transform_origin_z));

        // Perform the inflation
        if (HasFilterAnimationThatInflatesBounds(*layer)) {
            gfx::BoxF inflated;
            if (!layer->FilterAnimationBoundsForBox(box, &inflated))
                return false;
            box = inflated;
        }

        if (HasTransformAnimationThatInflatesBounds(*layer)) {
            gfx::BoxF inflated;
            if (!layer->TransformAnimationBoundsForBox(box, &inflated))
                return false;
            box = inflated;
        }

        // Apply step 3) mentioned above.
        box.set_origin(box.origin() + gfx::Vector3dF(transform_origin_x + position.x(), transform_origin_y + position.y(), transform_origin_z));
    }

    // If we've got an unapplied coalesced transform at this point, it must still
    // be applied.
    if (!coalesced_transform.IsIdentity())
        coalesced_transform.TransformBox(&box);

    *out = box;

    return true;
}

} // namespace cc
