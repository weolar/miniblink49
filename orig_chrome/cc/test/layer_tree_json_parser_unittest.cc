// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/layer_tree_json_parser.h"

#include "cc/layers/layer.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/geometry_test_utils.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {

namespace {

    bool LayerTreesMatch(LayerImpl* const layer_impl,
        Layer* const layer)
    {
#define RETURN_IF_EXPECTATION_FAILS(exp)                                               \
    do {                                                                               \
        exp;                                                                           \
        if (testing::UnitTest::GetInstance()->current_test_info()->result()->Failed()) \
            return false;                                                              \
    } while (0)

        RETURN_IF_EXPECTATION_FAILS(EXPECT_EQ(layer_impl->children().size(),
            layer->children().size()));
        RETURN_IF_EXPECTATION_FAILS(EXPECT_EQ(layer_impl->bounds(), layer->bounds()));
        RETURN_IF_EXPECTATION_FAILS(
            EXPECT_EQ(layer_impl->position(), layer->position()));
        RETURN_IF_EXPECTATION_FAILS(EXPECT_TRANSFORMATION_MATRIX_EQ(
            layer_impl->transform(), layer->transform()));
        RETURN_IF_EXPECTATION_FAILS(EXPECT_EQ(layer_impl->contents_opaque(),
            layer->contents_opaque()));
        RETURN_IF_EXPECTATION_FAILS(EXPECT_EQ(layer_impl->scrollable(),
            layer->scrollable()));
        RETURN_IF_EXPECTATION_FAILS(EXPECT_FLOAT_EQ(layer_impl->opacity(),
            layer->opacity()));
        RETURN_IF_EXPECTATION_FAILS(EXPECT_EQ(layer_impl->have_wheel_event_handlers(),
            layer->have_wheel_event_handlers()));
        RETURN_IF_EXPECTATION_FAILS(
            EXPECT_EQ(layer_impl->have_scroll_event_handlers(),
                layer->have_scroll_event_handlers()));
        RETURN_IF_EXPECTATION_FAILS(
            EXPECT_EQ(layer_impl->touch_event_handler_region(),
                layer->touch_event_handler_region()));

        for (size_t i = 0; i < layer_impl->children().size(); ++i) {
            RETURN_IF_EXPECTATION_FAILS(EXPECT_TRUE(LayerTreesMatch(
                layer_impl->children()[i], layer->children()[i].get())));
        }

        return true;
#undef RETURN_IF_EXPECTATION_FAILS
    }

} // namespace

class LayerTreeJsonParserSanityCheck : public testing::Test {
};

TEST_F(LayerTreeJsonParserSanityCheck, Basic)
{
    FakeImplProxy proxy;
    TestSharedBitmapManager shared_bitmap_manager;
    TestTaskGraphRunner task_graph_runner;
    FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
        &task_graph_runner);
    LayerTreeImpl* tree = host_impl.active_tree();

    scoped_ptr<LayerImpl> root_impl(LayerImpl::Create(tree, 1));
    scoped_ptr<LayerImpl> parent(LayerImpl::Create(tree, 2));
    scoped_ptr<LayerImpl> child(LayerImpl::Create(tree, 3));

    root_impl->SetBounds(gfx::Size(100, 100));
    parent->SetBounds(gfx::Size(50, 50));
    child->SetBounds(gfx::Size(40, 40));

    parent->SetPosition(gfx::Point(25, 25));

    child->SetHaveWheelEventHandlers(true);
    child->SetHaveScrollEventHandlers(true);

    parent->AddChild(child.Pass());
    root_impl->AddChild(parent.Pass());
    tree->SetRootLayer(root_impl.Pass());

    std::string json = host_impl.LayerTreeAsJson();
    scoped_refptr<Layer> root = ParseTreeFromJson(json, NULL);
    ASSERT_TRUE(root.get());
    EXPECT_TRUE(LayerTreesMatch(host_impl.RootLayer(), root.get()));
}

TEST_F(LayerTreeJsonParserSanityCheck, EventHandlerRegions)
{
    FakeImplProxy proxy;
    TestSharedBitmapManager shared_bitmap_manager;
    TestTaskGraphRunner task_graph_runner;
    FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
        &task_graph_runner);
    LayerTreeImpl* tree = host_impl.active_tree();

    scoped_ptr<LayerImpl> root_impl(LayerImpl::Create(tree, 1));
    scoped_ptr<LayerImpl> touch_layer(LayerImpl::Create(tree, 2));

    root_impl->SetBounds(gfx::Size(100, 100));
    touch_layer->SetBounds(gfx::Size(50, 50));

    Region touch_region;
    touch_region.Union(gfx::Rect(10, 10, 20, 30));
    touch_region.Union(gfx::Rect(40, 10, 20, 20));
    touch_layer->SetTouchEventHandlerRegion(touch_region);

    root_impl->AddChild(touch_layer.Pass());
    tree->SetRootLayer(root_impl.Pass());

    std::string json = host_impl.LayerTreeAsJson();
    scoped_refptr<Layer> root = ParseTreeFromJson(json, NULL);
    ASSERT_TRUE(root.get());
    EXPECT_TRUE(LayerTreesMatch(host_impl.RootLayer(), root.get()));
}

} // namespace cc
