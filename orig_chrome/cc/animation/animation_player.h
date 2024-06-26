// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_ANIMATION_PLAYER_H_
#define CC_ANIMATION_ANIMATION_PLAYER_H_

#include "base/containers/linked_list.h"
#include "base/memory/ref_counted.h"
#include "base/time/time.h"
#include "cc/animation/animation.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_vector.h"

namespace cc {

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
class CC_EXPORT AnimationPlayer : public base::RefCounted<AnimationPlayer>,
                                  public base::LinkNode<AnimationPlayer> {
public:
    static scoped_refptr<AnimationPlayer> Create(int id);
    scoped_refptr<AnimationPlayer> CreateImplInstance() const;

    int id() const { return id_; }
    int layer_id() const { return layer_id_; }

    // Parent AnimationHost. AnimationPlayer can be detached from
    // AnimationTimeline.
    AnimationHost* animation_host() { return animation_host_; }
    const AnimationHost* animation_host() const { return animation_host_; }
    void SetAnimationHost(AnimationHost* animation_host);

    // Parent AnimationTimeline.
    AnimationTimeline* animation_timeline() { return animation_timeline_; }
    const AnimationTimeline* animation_timeline() const
    {
        return animation_timeline_;
    }
    void SetAnimationTimeline(AnimationTimeline* timeline);

    // ElementAnimations object where this player is listed.
    // ElementAnimations has a reference to shared LayerAnimationController.
    ElementAnimations* element_animations() const { return element_animations_; }

    void set_layer_animation_delegate(AnimationDelegate* delegate)
    {
        layer_animation_delegate_ = delegate;
    }

    void AttachLayer(int layer_id);
    void DetachLayer();

    void AddAnimation(scoped_ptr<Animation> animation);
    void PauseAnimation(int animation_id, double time_offset);
    void RemoveAnimation(int animation_id);

    void PushPropertiesTo(AnimationPlayer* player_impl);

    // AnimationDelegate routing.
    void NotifyAnimationStarted(base::TimeTicks monotonic_time,
        Animation::TargetProperty target_property,
        int group);
    void NotifyAnimationFinished(base::TimeTicks monotonic_time,
        Animation::TargetProperty target_property,
        int group);

private:
    friend class base::RefCounted<AnimationPlayer>;

    explicit AnimationPlayer(int id);
    ~AnimationPlayer();

    void SetNeedsCommit();

    void RegisterPlayer();
    void UnregisterPlayer();

    void BindElementAnimations();
    void UnbindElementAnimations();

    // We accumulate added animations in animations_ container
    // if element_animations_ is a nullptr. It allows us to add/remove animations
    // to non-attached AnimationPlayers.
    typedef ScopedPtrVector<Animation> AnimationList;
    AnimationList animations_;

    AnimationHost* animation_host_;
    AnimationTimeline* animation_timeline_;
    // element_animations isn't null if player attached to an element (layer).
    ElementAnimations* element_animations_;
    AnimationDelegate* layer_animation_delegate_;

    int id_;
    int layer_id_;

    DISALLOW_COPY_AND_ASSIGN(AnimationPlayer);
};

} // namespace cc

#endif // CC_ANIMATION_ANIMATION_PLAYER_H_
