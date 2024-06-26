// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_ElementAnimations_h
#define mc_animation_ElementAnimations_h

#include "mc/animation/AnimationDelegate.h"
#include "mc/animation/LayerAnimationController.h"
#include "mc/animation/LayerAnimationValueProvider.h"
#include "mc/animation/FilterOperations.h"
#include "mc/base/LinkedList.h"
#include "third_party/WebKit/Source/wtf/RefPtr.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"

namespace gfx {
class ScrollOffset;
class Transform;
}

namespace mc {

class AnimationHost;
class AnimationPlayer;
class LayerAnimationController;
enum class LayerTreeType;

// An ElementAnimations owns a list of all AnimationPlayers, attached to
// the layer. Also, it owns LayerAnimationController instance (1:1
// relationship)
// ElementAnimations object redirects all events from LAC to the list
// of animation layers.
// This is a CC counterpart for blink::ElementAnimations (in 1:1 relationship).
// No pointer to/from respective blink::ElementAnimations object for now.
class ElementAnimations : public AnimationDelegate, public LayerAnimationValueProvider {
public:
    static WTF::PassOwnPtr<ElementAnimations> create(AnimationHost* host);
    ~ElementAnimations() override;

    int getLayerId() const
    {
        return m_layerAnimationController ? m_layerAnimationController->id() : 0;
    }

    // Parent AnimationHost.
    AnimationHost* getAnimationHost() {  return m_animationHost; }
    const AnimationHost* getAnimationHost() const { return m_animationHost; }

    LayerAnimationController* getLayerAnimationController() const { return m_layerAnimationController.get(); }

    void createLayerAnimationController(int layerId);
    void destroyLayerAnimationController();

    void layerRegistered(int layerId, LayerTreeType treeType);
    void layerUnregistered(int layerId, LayerTreeType treeType);

    bool has_active_value_observer_for_testing() const {
        return m_activeValueObserver;
    }
    bool has_pending_value_observer_for_testing() const {
        return m_pendingValueObserver;
    }

    void addPlayer(AnimationPlayer* player);
    void removePlayer(AnimationPlayer* player);
    bool isEmpty() const;

    typedef LinkedList<AnimationPlayer> PlayersList;
    typedef LinkNode<AnimationPlayer> PlayersListNode;
    const PlayersList& playersList() const { return *m_playersList.get(); }

    void pushPropertiesTo(ElementAnimations* element_animations_impl);

private:
    explicit ElementAnimations(AnimationHost* host);

    void setFilterMutated(LayerTreeType treeType, const FilterOperations& filters);
    void setOpacityMutated(LayerTreeType treeType, float opacity);
    void setTransformMutated(LayerTreeType treeType, const SkMatrix44& transform);
    void setScrollOffsetMutated(LayerTreeType treeType, const blink::FloatPoint& scrollOffset);
    void setTransformIsPotentiallyAnimatingChanged(LayerTreeType treeType, bool is_animating);

    void createActiveValueObserver();
    void destroyActiveValueObserver();

    void createPendingValueObserver();
    void destroyPendingValueObserver();

    // AnimationDelegate implementation
    void notifyAnimationStarted(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group) override;
    void notifyAnimationFinished(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group) override;

    // LayerAnimationValueProvider implementation.
    blink::FloatPoint scrollOffsetForAnimation() const override;

    WTF::OwnPtr<PlayersList> m_playersList;

    class ValueObserver;
    WTF::OwnPtr<ValueObserver> m_activeValueObserver;
    WTF::OwnPtr<ValueObserver> m_pendingValueObserver;

    // LAC is owned by ElementAnimations (1:1 relationship).
    WTF::RefPtr<LayerAnimationController> m_layerAnimationController;
    AnimationHost* m_animationHost;
};

}  // namespace mc

#endif  // mc_animation_ElementAnimations_h
