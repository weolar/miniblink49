// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/input/scroll_state.h"

#include "cc/layers/layer_impl.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {

class ScrollStateTest : public testing::Test {
};

TEST_F(ScrollStateTest, ConsumeDeltaNative)
{
    const float delta_x = 12.3f;
    const float delta_y = 3.9f;

    const float delta_x_to_consume = 1.2f;
    const float delta_y_to_consume = 2.3f;

    ScrollState scrollState(delta_x, delta_y, 0, 0, false /* should_propagate */,
        false /* delta_consumed_for_scroll_sequence */,
        false /* is_direct_manipulation */);
    EXPECT_FLOAT_EQ(delta_x, scrollState.delta_x());
    EXPECT_FLOAT_EQ(delta_y, scrollState.delta_y());
    EXPECT_FALSE(scrollState.delta_consumed_for_scroll_sequence());
    EXPECT_FALSE(scrollState.FullyConsumed());

    scrollState.ConsumeDelta(0, 0);
    EXPECT_FLOAT_EQ(delta_x, scrollState.delta_x());
    EXPECT_FLOAT_EQ(delta_y, scrollState.delta_y());
    EXPECT_FALSE(scrollState.delta_consumed_for_scroll_sequence());
    EXPECT_FALSE(scrollState.FullyConsumed());

    scrollState.ConsumeDelta(delta_x_to_consume, 0);
    EXPECT_FLOAT_EQ(delta_x - delta_x_to_consume, scrollState.delta_x());
    EXPECT_FLOAT_EQ(delta_y, scrollState.delta_y());
    EXPECT_TRUE(scrollState.delta_consumed_for_scroll_sequence());
    EXPECT_FALSE(scrollState.FullyConsumed());

    scrollState.ConsumeDelta(0, delta_y_to_consume);
    EXPECT_FLOAT_EQ(delta_x - delta_x_to_consume, scrollState.delta_x());
    EXPECT_FLOAT_EQ(delta_y - delta_y_to_consume, scrollState.delta_y());
    EXPECT_TRUE(scrollState.delta_consumed_for_scroll_sequence());
    EXPECT_FALSE(scrollState.FullyConsumed());

    scrollState.ConsumeDelta(scrollState.delta_x(), scrollState.delta_y());
    EXPECT_TRUE(scrollState.delta_consumed_for_scroll_sequence());
    EXPECT_TRUE(scrollState.FullyConsumed());
}

TEST_F(ScrollStateTest, CurrentNativeScrollingScrollable)
{
    ScrollState scrollState(0, 0, 0, 0, false, false, false);

    FakeImplProxy proxy;
    TestSharedBitmapManager shared_bitmap_manager;
    TestTaskGraphRunner task_graph_runner;
    FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
        &task_graph_runner);

    scoped_ptr<LayerImpl> layer_impl = LayerImpl::Create(host_impl.active_tree(), 1);
    scrollState.set_current_native_scrolling_layer(layer_impl.get());
    EXPECT_EQ(layer_impl, scrollState.current_native_scrolling_layer());
}

TEST_F(ScrollStateTest, FullyConsumed)
{
    ScrollState scrollState(1, 3, 0, 0, 0, false, false);
    EXPECT_FALSE(scrollState.FullyConsumed());

    scrollState.ConsumeDelta(1, 3);
    EXPECT_TRUE(scrollState.FullyConsumed());
}
} // namespace cc
