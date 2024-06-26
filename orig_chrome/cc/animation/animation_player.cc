// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation_player.h"

#include "cc/animation/animation_delegate.h"
#include "cc/animation/animation_host.h"
#include "cc/animation/animation_timeline.h"
#include "cc/animation/element_animations.h"
#include "cc/animation/layer_animation_controller.h"

namespace cc {

scoped_refptr<AnimationPlayer> AnimationPlayer::Create(int id)
{
    return make_scoped_refptr(new AnimationPlayer(id));
}

AnimationPlayer::AnimationPlayer(int id)
    : animation_host_()
    , animation_timeline_()
    , element_animations_()
    , layer_animation_delegate_()
    , id_(id)
    , layer_id_(0)
{
    DCHECK(id_);
}

AnimationPlayer::~AnimationPlayer()
{
    DCHECK(!animation_timeline_);
    DCHECK(!element_animations_);
    DCHECK(!layer_id_);
}

scoped_refptr<AnimationPlayer> AnimationPlayer::CreateImplInstance() const
{
    scoped_refptr<AnimationPlayer> player = AnimationPlayer::Create(id());
    return player;
}

void AnimationPlayer::SetAnimationHost(AnimationHost* animation_host)
{
    animation_host_ = animation_host;
}

void AnimationPlayer::SetAnimationTimeline(AnimationTimeline* timeline)
{
    if (animation_timeline_ == timeline)
        return;

    // We need to unregister player to manage ElementAnimations and observers
    // properly.
    if (layer_id_ && element_animations_)
        UnregisterPlayer();

    animation_timeline_ = timeline;

    // Register player only if layer AND host attached.
    if (layer_id_ && animation_host_)
        RegisterPlayer();
}

void AnimationPlayer::AttachLayer(int layer_id)
{
    DCHECK_EQ(layer_id_, 0);
    DCHECK(layer_id);

    layer_id_ = layer_id;

    // Register player only if layer AND host attached.
    if (animation_host_)
        RegisterPlayer();
}

void AnimationPlayer::DetachLayer()
{
    DCHECK(layer_id_);

    if (animation_host_)
        UnregisterPlayer();

    layer_id_ = 0;
}

void AnimationPlayer::RegisterPlayer()
{
    DCHECK(layer_id_);
    DCHECK(animation_host_);
    DCHECK(!element_animations_);

    // Create LAC or re-use existing.
    animation_host_->RegisterPlayerForLayer(layer_id_, this);
    // Get local reference to shared LAC.
    BindElementAnimations();
}

void AnimationPlayer::UnregisterPlayer()
{
    DCHECK(layer_id_);
    DCHECK(animation_host_);
    DCHECK(element_animations_);

    UnbindElementAnimations();
    // Destroy LAC or release it if it's still needed.
    animation_host_->UnregisterPlayerForLayer(layer_id_, this);
}

void AnimationPlayer::BindElementAnimations()
{
    DCHECK(!element_animations_);
    element_animations_ = animation_host_->GetElementAnimationsForLayerId(layer_id_);
    DCHECK(element_animations_);

    // Pass all accumulated animations to LAC.
    for (auto it = animations_.begin(); it != animations_.end(); ++it)
        element_animations_->layer_animation_controller()->AddAnimation(
            animations_.take(it));
    if (!animations_.empty())
        SetNeedsCommit();
    animations_.clear();
}

void AnimationPlayer::UnbindElementAnimations()
{
    element_animations_ = nullptr;
    DCHECK(animations_.empty());
}

void AnimationPlayer::AddAnimation(scoped_ptr<Animation> animation)
{
    DCHECK_IMPLIES(
        animation->target_property() == Animation::SCROLL_OFFSET,
        animation_host_ && animation_host_->SupportsScrollAnimations());

    if (element_animations_) {
        element_animations_->layer_animation_controller()->AddAnimation(
            animation.Pass());
        SetNeedsCommit();
    } else {
        animations_.push_back(animation.Pass());
    }
}

void AnimationPlayer::PauseAnimation(int animation_id, double time_offset)
{
    DCHECK(element_animations_);
    element_animations_->layer_animation_controller()->PauseAnimation(
        animation_id, base::TimeDelta::FromSecondsD(time_offset));
    SetNeedsCommit();
}

void AnimationPlayer::RemoveAnimation(int animation_id)
{
    if (element_animations_) {
        element_animations_->layer_animation_controller()->RemoveAnimation(
            animation_id);
        SetNeedsCommit();
    } else {
        auto animations_to_remove = animations_.remove_if([animation_id](
                                                              Animation* animation) { return animation->id() == animation_id; });
        animations_.erase(animations_to_remove, animations_.end());
    }
}

void AnimationPlayer::PushPropertiesTo(AnimationPlayer* player_impl)
{
    if (!element_animations_) {
        if (player_impl->element_animations())
            player_impl->DetachLayer();
        return;
    }

    DCHECK(layer_id_);
    if (!player_impl->element_animations())
        player_impl->AttachLayer(layer_id_);
}

void AnimationPlayer::NotifyAnimationStarted(
    base::TimeTicks monotonic_time,
    Animation::TargetProperty target_property,
    int group)
{
    if (layer_animation_delegate_)
        layer_animation_delegate_->NotifyAnimationStarted(monotonic_time,
            target_property, group);
}

void AnimationPlayer::NotifyAnimationFinished(
    base::TimeTicks monotonic_time,
    Animation::TargetProperty target_property,
    int group)
{
    if (layer_animation_delegate_)
        layer_animation_delegate_->NotifyAnimationFinished(monotonic_time,
            target_property, group);
}

void AnimationPlayer::SetNeedsCommit()
{
    DCHECK(animation_host_);
    animation_host_->SetNeedsCommit();
}

} // namespace cc
