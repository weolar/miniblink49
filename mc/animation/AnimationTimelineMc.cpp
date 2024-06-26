// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/AnimationTimeline.h"

#include <algorithm>

#include "mc/animation/AnimationPlayer.h"

namespace mc {

WTF::PassRefPtr<AnimationTimeline> AnimationTimeline::create(int id) 
{
    return adoptRef(new AnimationTimeline(id));
}

AnimationTimeline::AnimationTimeline(int id)
    : m_id(id)
    , m_animationHost()
    , m_isImplOnly(false) 
{
}

AnimationTimeline::~AnimationTimeline()
{
    for (auto& player : m_players)
        player->setAnimationTimeline(nullptr);
}

WTF::PassRefPtr<AnimationTimeline> AnimationTimeline::createImplInstance() const 
{
    WTF::PassRefPtr<AnimationTimeline> timeline = AnimationTimeline::create(id());
    return timeline;
}

void AnimationTimeline::setAnimationHost(AnimationHost* animation_host) 
{
    m_animationHost = animation_host;
    for (size_t i = 0; i < m_players.size(); ++i) {
        AnimationPlayer* player = m_players[i];
        player->setAnimationHost(animation_host);
    }        
}

void AnimationTimeline::attachPlayer(WTF::PassRefPtr<AnimationPlayer> player) 
{
    player->setAnimationHost(m_animationHost);

    player->setAnimationTimeline(this);
    m_players.push_back(player.leakRef());
}

void AnimationTimeline::detachPlayer(WTF::PassRefPtr<AnimationPlayer> player) 
{
    for (AnimationPlayerList::iterator iter = m_players.begin(); iter != m_players.end(); ++iter) {
        AnimationPlayer* it = *iter;
        if (it != player.get())
            continue;

        erasePlayers(iter, iter + 1);
        break;
    }

    player->setAnimationHost(nullptr);
}

AnimationPlayer* AnimationTimeline::getPlayerById(int playerId) const {
    for (size_t i = 0; i < m_players.size(); ++i) {
        AnimationPlayer* player = m_players[i];
        if (player->id() == playerId)
            return player;
    }
    return nullptr;
}

void AnimationTimeline::clearPlayers() {
    erasePlayers(m_players.begin(), m_players.end());
}

void AnimationTimeline::pushPropertiesTo(AnimationTimeline* timelineImpl) {
    pushAttachedPlayersToImplThread(timelineImpl);
    removeDetachedPlayersFromImplThread(timelineImpl);
    pushPropertiesToImplThread(timelineImpl);
}

void AnimationTimeline::pushAttachedPlayersToImplThread(AnimationTimeline* timelineImpl) const
{
    for (size_t i = 0; i < m_players.size(); ++i) {
        AnimationPlayer* player = m_players[i];
        AnimationPlayer* playerImpl = timelineImpl->getPlayerById(player->id());
        if (playerImpl)
            continue;

        WTF::PassRefPtr<AnimationPlayer> to_add = player->createImplInstance();
        timelineImpl->attachPlayer(to_add.get());
    }
}

void AnimationTimeline::removeDetachedPlayersFromImplThread(AnimationTimeline* timelineImpl) const 
{
    AnimationPlayerList& playersImpl = timelineImpl->m_players;

    auto toErase = std::partition(playersImpl.begin(), playersImpl.end(), [this](AnimationPlayerList::value_type playerImpl) {
        return getPlayerById(playerImpl->id());
    });

    timelineImpl->erasePlayers(toErase, playersImpl.end());
}

void AnimationTimeline::erasePlayers(AnimationPlayerList::iterator begin, AnimationPlayerList::iterator end) {
    for (AnimationPlayerList::iterator i = begin; i != end; ++i) {
        AnimationPlayer* player = *i;
        if (player->getElementAnimations())
            player->detachLayer();
        player->setAnimationTimeline(nullptr);
        player->deref();
    }

    m_players.erase(begin, end);
}

void AnimationTimeline::pushPropertiesToImplThread(AnimationTimeline* timelineImpl) 
{
    for (size_t i = 0; i < m_players.size(); ++i) {
        AnimationPlayer* player = m_players[i];
        AnimationPlayer* playerImpl = timelineImpl->getPlayerById(player->id());
        if (playerImpl)
            player->pushPropertiesTo(playerImpl);
    }
}

}  // namespace mc
