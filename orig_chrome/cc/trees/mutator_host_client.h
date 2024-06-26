// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_MUTATOR_HOST_CLIENT_H_
#define CC_TREES_MUTATOR_HOST_CLIENT_H_

namespace gfx {
class Transform;
class ScrollOffset;
}

namespace cc {

class FilterOperations;
class Layer;

enum class LayerTreeType { ACTIVE,
    PENDING };

class MutatorHostClient {
public:
    virtual bool IsLayerInTree(int layer_id, LayerTreeType tree_type) const = 0;
    virtual void SetMutatorsNeedCommit() = 0;

    virtual void SetLayerFilterMutated(int layer_id,
        LayerTreeType tree_type,
        const FilterOperations& filters)
        = 0;
    virtual void SetLayerOpacityMutated(int layer_id,
        LayerTreeType tree_type,
        float opacity)
        = 0;
    virtual void SetLayerTransformMutated(int layer_id,
        LayerTreeType tree_type,
        const gfx::Transform& transform)
        = 0;
    virtual void SetLayerScrollOffsetMutated(
        int layer_id,
        LayerTreeType tree_type,
        const gfx::ScrollOffset& scroll_offset)
        = 0;

    virtual void LayerTransformIsPotentiallyAnimatingChanged(
        int layer_id,
        LayerTreeType tree_type,
        bool is_animating)
        = 0;

    virtual void ScrollOffsetAnimationFinished() = 0;
    virtual gfx::ScrollOffset GetScrollOffsetForAnimation(int layer_id) const = 0;
};

} // namespace cc

#endif // CC_TREES_MUTATOR_HOST_CLIENT_H_
