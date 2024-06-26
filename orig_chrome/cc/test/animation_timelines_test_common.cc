// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/animation_timelines_test_common.h"

#include "cc/animation/animation_events.h"
#include "cc/animation/animation_id_provider.h"
#include "cc/animation/animation_player.h"
#include "cc/animation/animation_registrar.h"
#include "cc/animation/animation_timeline.h"
#include "cc/animation/element_animations.h"
#include "cc/output/filter_operation.h"
#include "cc/output/filter_operations.h"
#include "ui/gfx/transform.h"

namespace cc {

scoped_ptr<TestLayer> TestLayer::Create()
{
    return make_scoped_ptr(new TestLayer());
}

TestLayer::TestLayer()
{
    ClearMutatedProperties();
}

void TestLayer::ClearMutatedProperties()
{
    transform_x_ = 0;
    transform_y_ = 0;

    opacity_ = 0;
    brightness_ = 0;

    for (int i = 0; i <= Animation::LAST_TARGET_PROPERTY; ++i)
        mutated_properties_[i] = false;
}

TestHostClient::TestHostClient(ThreadInstance thread_instance)
    : host_(AnimationHost::Create(thread_instance))
    , mutators_need_commit_(false)
{
    host_->SetMutatorHostClient(this);
}

TestHostClient::~TestHostClient()
{
    host_->SetMutatorHostClient(nullptr);
}

void TestHostClient::ClearMutatedProperties()
{
    for (auto& kv : layers_in_pending_tree_)
        kv.second->ClearMutatedProperties();
    for (auto& kv : layers_in_active_tree_)
        kv.second->ClearMutatedProperties();
}

bool TestHostClient::IsLayerInTree(int layer_id,
    LayerTreeType tree_type) const
{
    return tree_type == LayerTreeType::ACTIVE
        ? layers_in_active_tree_.count(layer_id)
        : layers_in_pending_tree_.count(layer_id);
}

void TestHostClient::SetMutatorsNeedCommit()
{
    mutators_need_commit_ = true;
}

void TestHostClient::SetLayerFilterMutated(int layer_id,
    LayerTreeType tree_type,
    const FilterOperations& filters)
{
    for (unsigned i = 0; i < filters.size(); ++i) {
        const FilterOperation& filter = filters.at(i);
        if (filter.type() == FilterOperation::BRIGHTNESS) {
            TestLayer* layer = FindTestLayer(layer_id, tree_type);
            layer->set_brightness(filter.amount());
        }
    }
}

void TestHostClient::SetLayerOpacityMutated(int layer_id,
    LayerTreeType tree_type,
    float opacity)
{
    TestLayer* layer = FindTestLayer(layer_id, tree_type);
    layer->set_opacity(opacity);
}

void TestHostClient::SetLayerTransformMutated(int layer_id,
    LayerTreeType tree_type,
    const gfx::Transform& transform)
{
    TestLayer* layer = FindTestLayer(layer_id, tree_type);
    gfx::Vector2dF vec = transform.To2dTranslation();
    layer->set_transform(static_cast<int>(vec.x()), static_cast<int>(vec.y()));
}

void TestHostClient::SetLayerScrollOffsetMutated(
    int layer_id,
    LayerTreeType tree_type,
    const gfx::ScrollOffset& scroll_offset)
{
    TestLayer* layer = FindTestLayer(layer_id, tree_type);
    layer->set_scroll_offset(scroll_offset);
}

gfx::ScrollOffset TestHostClient::GetScrollOffsetForAnimation(
    int layer_id) const
{
    return gfx::ScrollOffset();
}

void TestHostClient::RegisterLayer(int layer_id, LayerTreeType tree_type)
{
    LayerIdToTestLayer& layers_in_tree = tree_type == LayerTreeType::ACTIVE
        ? layers_in_active_tree_
        : layers_in_pending_tree_;
    DCHECK(layers_in_tree.find(layer_id) == layers_in_tree.end());
    layers_in_tree.add(layer_id, TestLayer::Create());

    DCHECK(host_);
    host_->RegisterLayer(layer_id, tree_type);
}

void TestHostClient::UnregisterLayer(int layer_id, LayerTreeType tree_type)
{
    DCHECK(host_);
    host_->UnregisterLayer(layer_id, tree_type);

    LayerIdToTestLayer& layers_in_tree = tree_type == LayerTreeType::ACTIVE
        ? layers_in_active_tree_
        : layers_in_pending_tree_;
    auto kv = layers_in_tree.find(layer_id);
    DCHECK(kv != layers_in_tree.end());
    layers_in_tree.erase(kv);
}

bool TestHostClient::IsPropertyMutated(
    int layer_id,
    LayerTreeType tree_type,
    Animation::TargetProperty property) const
{
    TestLayer* layer = FindTestLayer(layer_id, tree_type);
    return layer->is_property_mutated(property);
}

void TestHostClient::ExpectFilterPropertyMutated(int layer_id,
    LayerTreeType tree_type,
    float brightness) const
{
    TestLayer* layer = FindTestLayer(layer_id, tree_type);
    EXPECT_TRUE(layer->is_property_mutated(Animation::OPACITY));
    EXPECT_EQ(brightness, layer->brightness());
}

void TestHostClient::ExpectOpacityPropertyMutated(int layer_id,
    LayerTreeType tree_type,
    float opacity) const
{
    TestLayer* layer = FindTestLayer(layer_id, tree_type);
    EXPECT_TRUE(layer->is_property_mutated(Animation::OPACITY));
    EXPECT_EQ(opacity, layer->opacity());
}

void TestHostClient::ExpectTransformPropertyMutated(int layer_id,
    LayerTreeType tree_type,
    int transform_x,
    int transform_y) const
{
    TestLayer* layer = FindTestLayer(layer_id, tree_type);
    EXPECT_TRUE(layer->is_property_mutated(Animation::OPACITY));
    EXPECT_EQ(transform_x, layer->transform_x());
    EXPECT_EQ(transform_y, layer->transform_y());
}

void TestHostClient::ExpectScrollOffsetPropertyMutated(
    int layer_id,
    LayerTreeType tree_type,
    const gfx::ScrollOffset& scroll_offset) const
{
    TestLayer* layer = FindTestLayer(layer_id, tree_type);
    EXPECT_TRUE(layer->is_property_mutated(Animation::OPACITY));
    EXPECT_EQ(scroll_offset, layer->scroll_offset());
}

TestLayer* TestHostClient::FindTestLayer(int layer_id,
    LayerTreeType tree_type) const
{
    const LayerIdToTestLayer& layers_in_tree = tree_type == LayerTreeType::ACTIVE
        ? layers_in_active_tree_
        : layers_in_pending_tree_;
    auto kv = layers_in_tree.find(layer_id);
    DCHECK(kv != layers_in_tree.end());
    DCHECK(kv->second);
    return kv->second;
}

TestAnimationDelegate::TestAnimationDelegate()
    : started_(false)
    , finished_(false)
{
}

void TestAnimationDelegate::NotifyAnimationStarted(
    base::TimeTicks monotonic_time,
    Animation::TargetProperty target_property,
    int group)
{
    started_ = true;
}
void TestAnimationDelegate::NotifyAnimationFinished(
    base::TimeTicks monotonic_time,
    Animation::TargetProperty target_property,
    int group)
{
    finished_ = true;
}

AnimationTimelinesTest::AnimationTimelinesTest()
    : client_(ThreadInstance::MAIN)
    , client_impl_(ThreadInstance::IMPL)
    , host_(nullptr)
    , host_impl_(nullptr)
    , timeline_id_(AnimationIdProvider::NextTimelineId())
    , player_id_(AnimationIdProvider::NextPlayerId())
    , layer_id_(1)
{
    host_ = client_.host();
    host_impl_ = client_impl_.host();
}

AnimationTimelinesTest::~AnimationTimelinesTest()
{
}

void AnimationTimelinesTest::SetUp()
{
    timeline_ = AnimationTimeline::Create(timeline_id_);
    player_ = AnimationPlayer::Create(player_id_);
}

void AnimationTimelinesTest::GetImplTimelineAndPlayerByID()
{
    timeline_impl_ = host_impl_->GetTimelineById(timeline_id_);
    EXPECT_TRUE(timeline_impl_);
    player_impl_ = timeline_impl_->GetPlayerById(player_id_);
    EXPECT_TRUE(player_impl_);
}

void AnimationTimelinesTest::ReleaseRefPtrs()
{
    player_ = nullptr;
    timeline_ = nullptr;
    player_impl_ = nullptr;
    timeline_impl_ = nullptr;
}

void AnimationTimelinesTest::AnimateLayersTransferEvents(
    base::TimeTicks time,
    unsigned expect_events)
{
    scoped_ptr<AnimationEventsVector> events = host_->animation_registrar()->CreateEvents();

    host_impl_->animation_registrar()->AnimateLayers(time);
    host_impl_->animation_registrar()->UpdateAnimationState(true, events.get());
    EXPECT_EQ(expect_events, events->size());

    host_->animation_registrar()->AnimateLayers(time);
    host_->animation_registrar()->UpdateAnimationState(true, nullptr);
    host_->animation_registrar()->SetAnimationEvents(events.Pass());
}

AnimationPlayer* AnimationTimelinesTest::GetPlayerForLayerId(int layer_id)
{
    const ElementAnimations* element_animations = host_->GetElementAnimationsForLayerId(layer_id);
    return element_animations ? element_animations->players_list().head()->value()
                              : nullptr;
}

AnimationPlayer* AnimationTimelinesTest::GetImplPlayerForLayerId(int layer_id)
{
    const ElementAnimations* element_animations = host_impl_->GetElementAnimationsForLayerId(layer_id);
    return element_animations ? element_animations->players_list().head()->value()
                              : nullptr;
}

} // namespace cc
