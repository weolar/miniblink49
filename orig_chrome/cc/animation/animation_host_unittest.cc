// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation_host.h"

#include "cc/animation/animation_id_provider.h"
#include "cc/animation/animation_timeline.h"
#include "cc/test/animation_test_common.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    // See AnimationPlayer tests on layer registration/unregistration in
    // animation_player_unittest.cc.

    TEST(AnimationHostTest, SyncTimelinesAddRemove)
    {
        scoped_ptr<AnimationHost> host(AnimationHost::Create(ThreadInstance::MAIN));
        scoped_ptr<AnimationHost> host_impl(
            AnimationHost::Create(ThreadInstance::IMPL));

        const int timeline_id = AnimationIdProvider::NextTimelineId();
        scoped_refptr<AnimationTimeline> timeline(
            AnimationTimeline::Create(timeline_id));
        host->AddAnimationTimeline(timeline.get());
        EXPECT_TRUE(timeline->animation_host());

        EXPECT_FALSE(host_impl->GetTimelineById(timeline_id));

        host->PushPropertiesTo(host_impl.get());

        scoped_refptr<AnimationTimeline> timeline_impl = host_impl->GetTimelineById(timeline_id);
        EXPECT_TRUE(timeline_impl);
        EXPECT_EQ(timeline_impl->id(), timeline_id);

        host->PushPropertiesTo(host_impl.get());
        EXPECT_EQ(timeline_impl, host_impl->GetTimelineById(timeline_id));

        host->RemoveAnimationTimeline(timeline.get());
        EXPECT_FALSE(timeline->animation_host());

        host->PushPropertiesTo(host_impl.get());
        EXPECT_FALSE(host_impl->GetTimelineById(timeline_id));

        EXPECT_FALSE(timeline_impl->animation_host());
    }

    TEST(AnimationHostTest, ImplOnlyTimeline)
    {
        scoped_ptr<AnimationHost> host(AnimationHost::Create(ThreadInstance::MAIN));
        scoped_ptr<AnimationHost> host_impl(
            AnimationHost::Create(ThreadInstance::IMPL));

        const int timeline_id1 = AnimationIdProvider::NextTimelineId();
        const int timeline_id2 = AnimationIdProvider::NextTimelineId();

        scoped_refptr<AnimationTimeline> timeline(
            AnimationTimeline::Create(timeline_id1));
        scoped_refptr<AnimationTimeline> timeline_impl(
            AnimationTimeline::Create(timeline_id2));
        timeline_impl->set_is_impl_only(true);

        host->AddAnimationTimeline(timeline.get());
        host_impl->AddAnimationTimeline(timeline_impl.get());

        host->PushPropertiesTo(host_impl.get());

        EXPECT_TRUE(host->GetTimelineById(timeline_id1));
        EXPECT_TRUE(host_impl->GetTimelineById(timeline_id2));
    }

} // namespace
} // namespace cc
