// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_AnimationPlayer_h
#define mc_animation_AnimationPlayer_h

#include "base/time/time.h"
#include "mc/animation/AnimationObj.h"
#include "mc/base/ScopedPtrVector.h"
#include "mc/base/LinkedList.h"
#include "third_party/WebKit/Source/wtf/RefCounted.h"
#include "third_party/WebKit/Source/wtf/PassRefPtr.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"

namespace mc {

class AnimationDelegate;
class AnimationHost;
class AnimationTimeline;
class ElementAnimations;
class LayerAnimationController;
enum class LayerTreeType;

// An AnimationPlayer owns all animations to be run on particular CC Layer.
// Multiple AnimationPlayers can be attached to one layer. In this case,
// they share common LayerAnimationController (temp solution) so the
// LayerAnimationController-to-Layer relationship stays the same (1:1, LACs
// have same IDs as their respective Layers).
// For now, the blink logic is responsible for handling of conflicting
// same-property animations.
// Each AnimationPlayer has its copy on the impl thread.
// This is a CC counterpart for blink::AnimationPlayer (in 1:1 relationship).
class AnimationPlayer : public WTF::RefCounted<AnimationPlayer>, public LinkNode<AnimationPlayer> {
public:
    static WTF::PassRefPtr<AnimationPlayer> create(int id);
    WTF::PassRefPtr<AnimationPlayer> createImplInstance() const;

    int id() const {
        return m_id;
    }
    int layerId() const {
        return m_layerId;
    }

    // Parent AnimationHost. AnimationPlayer can be detached from
    // AnimationTimeline.
    AnimationHost* getAnimationHost() {
        return m_animationHost;
    }
    const AnimationHost* getAnimationHost() const {
        return m_animationHost;
    }
    void setAnimationHost(AnimationHost* animation_host);

    // Parent AnimationTimeline.
    AnimationTimeline* getAnimationTimeline() {
        return m_animationTimeline;
    }
    const AnimationTimeline* getAnimationTimeline() const {
        return m_animationTimeline;
    }
    void setAnimationTimeline(AnimationTimeline* timeline);

    // ElementAnimations object where this player is listed.
    // ElementAnimations has a reference to shared LayerAnimationController.
    ElementAnimations* getElementAnimations() const 
    {
        return m_elementAnimations;
    }

    void setLayerAnimationDelegate(AnimationDelegate* delegate)
    {
        m_layerAnimationDelegate = delegate;
    }

    void attachLayer(int layer_id);
    void detachLayer();

    void addAnimation(WTF::PassOwnPtr<AnimationObj> animation);
    void pauseAnimation(int animation_id, double time_offset);
    void removeAnimation(int animation_id);

    void pushPropertiesTo(AnimationPlayer* player_impl);

    // AnimationDelegate routing.
    void notifyAnimationStarted(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group);
    void notifyAnimationFinished(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group);

private:
    friend class WTF::RefCounted<AnimationPlayer>;

    explicit AnimationPlayer(int id);
    ~AnimationPlayer();

    void setNeedsCommit();

    void registerPlayer();
    void unregisterPlayer();

    void bindElementAnimations();
    void unbindElementAnimations();

    // We accumulate added animations in m_animations container
    // if m_elementAnimations is a nullptr. It allows us to add/remove animations
    // to non-attached AnimationPlayers.
    typedef ScopedPtrVector<AnimationObj> AnimationList;
    AnimationList m_animations;

    AnimationHost* m_animationHost;
    AnimationTimeline* m_animationTimeline;
    // element_animations isn't null if player attached to an element (layer).
    ElementAnimations* m_elementAnimations;
    AnimationDelegate* m_layerAnimationDelegate;

    int m_id;
    int m_layerId;
};

}  // namespace cc

#endif  // CC_ANIMATION_ANIMATION_PLAYER_H_
