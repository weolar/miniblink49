// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation_timeline.h"

#include <algorithm>

#include "cc/animation/animation_player.h"

namespace cc {

scoped_refptr<AnimationTimeline> AnimationTimeline::Create(int id)
{
    return make_scoped_refptr(new AnimationTimeline(id));
}

AnimationTimeline::AnimationTimeline(int id)
    : id_(id)
    , animation_host_()
    , is_impl_only_(false)
{
}

AnimationTimeline::~AnimationTimeline()
{
    for (auto& player : players_)
        player->SetAnimationTimeline(nullptr);
}

scoped_refptr<AnimationTimeline> AnimationTimeline::CreateImplInstance() const
{
    scoped_refptr<AnimationTimeline> timeline = AnimationTimeline::Create(id());
    return timeline;
}

void AnimationTimeline::SetAnimationHost(AnimationHost* animation_host)
{
    animation_host_ = animation_host;
    for (auto& player : players_)
        player->SetAnimationHost(animation_host);
}

void AnimationTimeline::AttachPlayer(scoped_refptr<AnimationPlayer> player)
{
    player->SetAnimationHost(animation_host_);

    player->SetAnimationTimeline(this);
    players_.push_back(player);
}

void AnimationTimeline::DetachPlayer(scoped_refptr<AnimationPlayer> player)
{
    for (AnimationPlayerList::iterator iter = players_.begin();
         iter != players_.end(); ++iter) {
        if (iter->get() != player)
            continue;

        ErasePlayers(iter, iter + 1);
        break;
    }

    player->SetAnimationHost(nullptr);
}

AnimationPlayer* AnimationTimeline::GetPlayerById(int player_id) const
{
    for (auto& player : players_)
        if (player->id() == player_id)
            return player.get();
    return nullptr;
}

void AnimationTimeline::ClearPlayers()
{
    ErasePlayers(players_.begin(), players_.end());
}

void AnimationTimeline::PushPropertiesTo(AnimationTimeline* timeline_impl)
{
    PushAttachedPlayersToImplThread(timeline_impl);
    RemoveDetachedPlayersFromImplThread(timeline_impl);
    PushPropertiesToImplThread(timeline_impl);
}

void AnimationTimeline::PushAttachedPlayersToImplThread(
    AnimationTimeline* timeline_impl) const
{
    for (auto& player : players_) {
        AnimationPlayer* player_impl = timeline_impl->GetPlayerById(player->id());
        if (player_impl)
            continue;

        scoped_refptr<AnimationPlayer> to_add = player->CreateImplInstance();
        timeline_impl->AttachPlayer(to_add.get());
    }
}

void AnimationTimeline::RemoveDetachedPlayersFromImplThread(
    AnimationTimeline* timeline_impl) const
{
    AnimationPlayerList& players_impl = timeline_impl->players_;

    auto to_erase = std::partition(players_impl.begin(), players_impl.end(),
        [this](AnimationPlayerList::value_type player_impl) {
            return GetPlayerById(player_impl->id());
        });

    timeline_impl->ErasePlayers(to_erase, players_impl.end());
}

void AnimationTimeline::ErasePlayers(AnimationPlayerList::iterator begin,
    AnimationPlayerList::iterator end)
{
    for (auto i = begin; i != end; ++i) {
        auto& player = *i;
        if (player->element_animations())
            player->DetachLayer();
        player->SetAnimationTimeline(nullptr);
    }

    players_.erase(begin, end);
}

void AnimationTimeline::PushPropertiesToImplThread(
    AnimationTimeline* timeline_impl)
{
    for (auto& player : players_) {
        AnimationPlayer* player_impl = timeline_impl->GetPlayerById(player->id());
        if (player_impl)
            player->PushPropertiesTo(player_impl);
    }
}

} // namespace cc
