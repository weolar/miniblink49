// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/AnimationPlayer.h"

#include "mc/animation/AnimationDelegate.h"
#include "mc/animation/AnimationHost.h"
#include "mc/animation/AnimationTimeline.h"
#include "mc/animation/ElementAnimations.h"
#include "mc/animation/LayerAnimationController.h"

namespace mc {

WTF::PassRefPtr<AnimationPlayer> AnimationPlayer::create(int id) {
    return new AnimationPlayer(id);
}

AnimationPlayer::AnimationPlayer(int id)
    : m_animationHost()
    , m_animationTimeline()
    , m_elementAnimations()
    , m_layerAnimationDelegate()
    , m_id(id)
    , m_layerId(0)
{
    ASSERT(m_id);
}

AnimationPlayer::~AnimationPlayer()
{
    ASSERT(!m_animationTimeline);
    ASSERT(!m_elementAnimations);
    ASSERT(!m_layerId);
}

WTF::PassRefPtr<AnimationPlayer> AnimationPlayer::createImplInstance() const
{
    WTF::PassRefPtr<AnimationPlayer> player = AnimationPlayer::create(id());
    return player;
}

void AnimationPlayer::setAnimationHost(AnimationHost* animationHost) 
{
    m_animationHost = animationHost;
}

void AnimationPlayer::setAnimationTimeline(AnimationTimeline* timeline) 
{
    if (m_animationTimeline == timeline)
        return;

    // We need to unregister player to manage ElementAnimations and observers
    // properly.
    if (m_layerId && m_elementAnimations)
        unregisterPlayer();

    m_animationTimeline = timeline;

    // Register player only if layer AND host attached.
    if (m_layerId && m_animationHost)
        registerPlayer();
}

void AnimationPlayer::attachLayer(int layer_id)
{
    ASSERT(m_layerId == 0);
    ASSERT(layer_id);

    m_layerId = layer_id;

    // Register player only if layer AND host attached.
    if (m_animationHost)
        registerPlayer();
}

void AnimationPlayer::detachLayer() 
{
    ASSERT(m_layerId);

    if (m_animationHost)
        unregisterPlayer();

    m_layerId = 0;
}

void AnimationPlayer::registerPlayer() 
{
    ASSERT(m_layerId);
    ASSERT(m_animationHost);
    ASSERT(!m_elementAnimations);

    // Create LAC or re-use existing.
    m_animationHost->registerPlayerForLayer(m_layerId, this);
    // Get local reference to shared LAC.
    bindElementAnimations();
}

void AnimationPlayer::unregisterPlayer()
{
    ASSERT(m_layerId);
    ASSERT(m_animationHost);
    ASSERT(m_elementAnimations);

    unbindElementAnimations();
    // Destroy LAC or release it if it's still needed.
    m_animationHost->unregisterPlayerForLayer(m_layerId, this);
}

void AnimationPlayer::bindElementAnimations() 
{
    ASSERT(!m_elementAnimations);
    m_elementAnimations = m_animationHost->getElementAnimationsForLayerId(m_layerId);
    ASSERT(m_elementAnimations);

    // Pass all accumulated animations to LAC.
    for (AnimationList::iterator it = m_animations.begin(); it != m_animations.end(); ++it)
        m_elementAnimations->getLayerAnimationController()->addAnimation(m_animations.take(it));
    if (!m_animations.empty())
        setNeedsCommit();
    m_animations.clear();
}

void AnimationPlayer::unbindElementAnimations()
{
    m_elementAnimations = nullptr;
    ASSERT(m_animations.empty());
}

void AnimationPlayer::addAnimation(WTF::PassOwnPtr<AnimationObj> animation)
{
    //ASSERT(animation->target_property() == AnimationObj::SCROLL_OFFSET, m_animationHost && m_animationHost->supportsScrollAnimations());

    if (m_elementAnimations) {
        m_elementAnimations->getLayerAnimationController()->addAnimation(animation);
        setNeedsCommit();
    } else {
        m_animations.push_back(animation.leakPtr());
    }
}

void AnimationPlayer::pauseAnimation(int animation_id, double time_offset) 
{
    ASSERT(m_elementAnimations);
    m_elementAnimations->getLayerAnimationController()->pauseAnimation(animation_id, base::TimeDelta::FromSecondsD(time_offset));
    setNeedsCommit();
}

void AnimationPlayer::removeAnimation(int animation_id)
{
    if (m_elementAnimations) {
        m_elementAnimations->getLayerAnimationController()->removeAnimation(animation_id);
        setNeedsCommit();
    } else {
        auto animations_to_remove = m_animations.remove_if([animation_id](AnimationObj* animation) { 
            return animation->id() == animation_id; 
        });
        m_animations.erase(animations_to_remove, m_animations.end());
    }
}

void AnimationPlayer::pushPropertiesTo(AnimationPlayer* player_impl) 
{
    if (!m_elementAnimations) {
        if (player_impl->getElementAnimations())
            player_impl->detachLayer();
        return;
    }

    ASSERT(m_layerId);
    if (!player_impl->getElementAnimations())
        player_impl->attachLayer(m_layerId);
}

void AnimationPlayer::notifyAnimationStarted(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group)
{
    if (m_layerAnimationDelegate)
        m_layerAnimationDelegate->notifyAnimationStarted(monotonic_time, target_property, group);
}

void AnimationPlayer::notifyAnimationFinished(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group)
{
    if (m_layerAnimationDelegate)
        m_layerAnimationDelegate->notifyAnimationFinished(monotonic_time, target_property, group);
}

void AnimationPlayer::setNeedsCommit() 
{
    ASSERT(m_animationHost);
    m_animationHost->setNeedsCommit();
}

}  // namespace cc
