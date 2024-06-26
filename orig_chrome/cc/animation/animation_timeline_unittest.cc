// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation_timeline.h"

#include "cc/animation/animation_host.h"
#include "cc/animation/animation_id_provider.h"
#include "cc/animation/animation_player.h"
#include "cc/test/animation_test_common.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    TEST(AnimationTimelineTest, SyncPlayersAttachDetach)
    {
        scoped_ptr<AnimationHost> host(AnimationHost::Create(ThreadInstance::MAIN));
        scoped_ptr<AnimationHost> host_impl(
            AnimationHost::Create(ThreadInstance::IMPL));

        const int timeline_id = AnimationIdProvider::NextTimelineId();
        const int player_id = AnimationIdProvider::NextPlayerId();

        scoped_refptr<AnimationTimeline> timeline_impl(
            AnimationTimeline::Create(timeline_id));
        scoped_refptr<AnimationTimeline> timeline(
            AnimationTimeline::Create(timeline_id));

        host->AddAnimationTimeline(timeline.get());
        EXPECT_TRUE(timeline->animation_host());

        host_impl->AddAnimationTimeline(timeline_impl.get());
        EXPECT_TRUE(timeline_impl->animation_host());

        scoped_refptr<AnimationPlayer> player(AnimationPlayer::Create(player_id));
        timeline->AttachPlayer(player.get());
        EXPECT_TRUE(player->animation_timeline());

        EXPECT_FALSE(timeline_impl->GetPlayerById(player_id));

        timeline->PushPropertiesTo(timeline_impl.get());

        scoped_refptr<AnimationPlayer> player_impl = timeline_impl->GetPlayerById(player_id);
        EXPECT_TRUE(player_impl);
        EXPECT_EQ(player_impl->id(), player_id);
        EXPECT_TRUE(player_impl->animation_timeline());

        timeline->PushPropertiesTo(timeline_impl.get());
        EXPECT_EQ(player_impl, timeline_impl->GetPlayerById(player_id));

        timeline->DetachPlayer(player.get());
        EXPECT_FALSE(player->animation_timeline());

        timeline->PushPropertiesTo(timeline_impl.get());
        EXPECT_FALSE(timeline_impl->GetPlayerById(player_id));

        EXPECT_FALSE(player_impl->animation_timeline());
    }

    TEST(AnimationTimelineTest, ClearPlayers)
    {
        scoped_ptr<AnimationHost> host(AnimationHost::Create(ThreadInstance::MAIN));
        scoped_ptr<AnimationHost> host_impl(
            AnimationHost::Create(ThreadInstance::IMPL));

        const int timeline_id = AnimationIdProvider::NextTimelineId();
        const int player_id1 = AnimationIdProvider::NextPlayerId();
        const int player_id2 = AnimationIdProvider::NextPlayerId();

        scoped_refptr<AnimationTimeline> timeline_impl(
            AnimationTimeline::Create(timeline_id));
        scoped_refptr<AnimationTimeline> timeline(
            AnimationTimeline::Create(timeline_id));

        host->AddAnimationTimeline(timeline.get());
        host_impl->AddAnimationTimeline(timeline_impl.get());

        scoped_refptr<AnimationPlayer> player1(AnimationPlayer::Create(player_id1));
        timeline->AttachPlayer(player1.get());
        scoped_refptr<AnimationPlayer> player2(AnimationPlayer::Create(player_id2));
        timeline->AttachPlayer(player2.get());

        timeline->PushPropertiesTo(timeline_impl.get());

        EXPECT_TRUE(timeline_impl->GetPlayerById(player_id1));
        EXPECT_TRUE(timeline_impl->GetPlayerById(player_id2));

        timeline->ClearPlayers();
        EXPECT_FALSE(timeline->GetPlayerById(player_id1));
        EXPECT_FALSE(timeline->GetPlayerById(player_id2));

        timeline_impl->ClearPlayers();
        EXPECT_FALSE(timeline_impl->GetPlayerById(player_id1));
        EXPECT_FALSE(timeline_impl->GetPlayerById(player_id2));
    }

} // namespace
} // namespace cc
