// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_ELEMENT_ANIMATIONS_H_
#define CC_ANIMATION_ELEMENT_ANIMATIONS_H_

#include "base/containers/linked_list.h"
#include "base/memory/ref_counted.h"
#include "cc/animation/animation_delegate.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/animation/layer_animation_value_provider.h"
#include "cc/base/cc_export.h"

namespace gfx {
class ScrollOffset;
class Transform;
}

namespace cc {

class AnimationHost;
class AnimationPlayer;
class FilterOperations;
class LayerAnimationController;
enum class LayerTreeType;

// An ElementAnimations owns a list of all AnimationPlayers, attached to
// the layer. Also, it owns LayerAnimationController instance (1:1
// relationship)
// ElementAnimations object redirects all events from LAC to the list
// of animation layers.
// This is a CC counterpart for blink::ElementAnimations (in 1:1 relationship).
// No pointer to/from respective blink::ElementAnimations object for now.
class CC_EXPORT ElementAnimations : public AnimationDelegate,
                                    public LayerAnimationValueProvider {
public:
    static scoped_ptr<ElementAnimations> Create(AnimationHost* host);
    ~ElementAnimations() override;

    int layer_id() const
    {
        return layer_animation_controller_ ? layer_animation_controller_->id() : 0;
    }

    // Parent AnimationHost.
    AnimationHost* animation_host() { return animation_host_; }
    const AnimationHost* animation_host() const { return animation_host_; }

    LayerAnimationController* layer_animation_controller() const
    {
        return layer_animation_controller_.get();
    }

    void CreateLayerAnimationController(int layer_id);
    void DestroyLayerAnimationController();

    void LayerRegistered(int layer_id, LayerTreeType tree_type);
    void LayerUnregistered(int layer_id, LayerTreeType tree_type);

    bool has_active_value_observer_for_testing() const
    {
        return active_value_observer_;
    }
    bool has_pending_value_observer_for_testing() const
    {
        return pending_value_observer_;
    }

    void AddPlayer(AnimationPlayer* player);
    void RemovePlayer(AnimationPlayer* player);
    bool IsEmpty() const;

    typedef base::LinkedList<AnimationPlayer> PlayersList;
    typedef base::LinkNode<AnimationPlayer> PlayersListNode;
    const PlayersList& players_list() const { return *players_list_.get(); }

    void PushPropertiesTo(ElementAnimations* element_animations_impl);

private:
    explicit ElementAnimations(AnimationHost* host);

    void SetFilterMutated(LayerTreeType tree_type,
        const FilterOperations& filters);
    void SetOpacityMutated(LayerTreeType tree_type, float opacity);
    void SetTransformMutated(LayerTreeType tree_type,
        const gfx::Transform& transform);
    void SetScrollOffsetMutated(LayerTreeType tree_type,
        const gfx::ScrollOffset& scroll_offset);
    void SetTransformIsPotentiallyAnimatingChanged(LayerTreeType tree_type,
        bool is_animating);

    void CreateActiveValueObserver();
    void DestroyActiveValueObserver();

    void CreatePendingValueObserver();
    void DestroyPendingValueObserver();

    // AnimationDelegate implementation
    void NotifyAnimationStarted(base::TimeTicks monotonic_time,
        Animation::TargetProperty target_property,
        int group) override;
    void NotifyAnimationFinished(base::TimeTicks monotonic_time,
        Animation::TargetProperty target_property,
        int group) override;

    // LayerAnimationValueProvider implementation.
    gfx::ScrollOffset ScrollOffsetForAnimation() const override;

    scoped_ptr<PlayersList> players_list_;

    class ValueObserver;
    scoped_ptr<ValueObserver> active_value_observer_;
    scoped_ptr<ValueObserver> pending_value_observer_;

    // LAC is owned by ElementAnimations (1:1 relationship).
    scoped_refptr<LayerAnimationController> layer_animation_controller_;
    AnimationHost* animation_host_;

    DISALLOW_COPY_AND_ASSIGN(ElementAnimations);
};

} // namespace cc

#endif // CC_ANIMATION_ELEMENT_ANIMATIONS_H_
