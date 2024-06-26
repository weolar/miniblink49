// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/layer_iterator.h"

#include <vector>

#include "cc/layers/layer.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/layer_tree_host_common.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/transform.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::Mock;

namespace cc {
namespace {

    class TestLayerImpl : public LayerImpl {
    public:
        static scoped_ptr<TestLayerImpl> Create(LayerTreeImpl* tree, int id)
        {
            return make_scoped_ptr(new TestLayerImpl(tree, id));
        }
        ~TestLayerImpl() override { }

        int count_representing_target_surface_;
        int count_representing_contributing_surface_;
        int count_representing_itself_;

    private:
        explicit TestLayerImpl(LayerTreeImpl* tree, int id)
            : LayerImpl(tree, id, new SyncedScrollOffset)
            , count_representing_target_surface_(-1)
            , count_representing_contributing_surface_(-1)
            , count_representing_itself_(-1)
        {
            SetBounds(gfx::Size(100, 100));
            SetPosition(gfx::Point());
            SetDrawsContent(true);
        }
    };

#define EXPECT_COUNT(layer, target, contrib, itself)                     \
    EXPECT_EQ(target, layer->count_representing_target_surface_);        \
    EXPECT_EQ(contrib, layer->count_representing_contributing_surface_); \
    EXPECT_EQ(itself, layer->count_representing_itself_);

    void ResetCounts(LayerImplList* render_surface_layer_list)
    {
        for (unsigned surface_index = 0;
             surface_index < render_surface_layer_list->size();
             ++surface_index) {
            TestLayerImpl* render_surface_layer = static_cast<TestLayerImpl*>(
                render_surface_layer_list->at(surface_index));
            RenderSurfaceImpl* render_surface = render_surface_layer->render_surface();

            render_surface_layer->count_representing_target_surface_ = -1;
            render_surface_layer->count_representing_contributing_surface_ = -1;
            render_surface_layer->count_representing_itself_ = -1;

            for (unsigned layer_index = 0;
                 layer_index < render_surface->layer_list().size();
                 ++layer_index) {
                TestLayerImpl* layer = static_cast<TestLayerImpl*>(
                    render_surface->layer_list()[layer_index]);

                layer->count_representing_target_surface_ = -1;
                layer->count_representing_contributing_surface_ = -1;
                layer->count_representing_itself_ = -1;
            }
        }
    }

    void IterateFrontToBack(LayerImplList* render_surface_layer_list)
    {
        ResetCounts(render_surface_layer_list);
        int count = 0;
        for (LayerIterator it = LayerIterator::Begin(render_surface_layer_list);
             it != LayerIterator::End(render_surface_layer_list); ++it, ++count) {
            TestLayerImpl* layer = static_cast<TestLayerImpl*>(*it);
            if (it.represents_target_render_surface())
                layer->count_representing_target_surface_ = count;
            if (it.represents_contributing_render_surface())
                layer->count_representing_contributing_surface_ = count;
            if (it.represents_itself())
                layer->count_representing_itself_ = count;
        }
    }

    class LayerIteratorTest : public testing::Test {
    public:
        LayerIteratorTest()
            : host_impl_(&proxy_, &shared_bitmap_manager_, &task_graph_runner_)
            , id_(1)
        {
        }

        scoped_ptr<TestLayerImpl> CreateLayer()
        {
            return TestLayerImpl::Create(host_impl_.active_tree(), id_++);
        }

    protected:
        FakeImplProxy proxy_;
        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        FakeLayerTreeHostImpl host_impl_;

        int id_;
    };

    TEST_F(LayerIteratorTest, EmptyTree)
    {
        LayerImplList render_surface_layer_list;

        IterateFrontToBack(&render_surface_layer_list);
    }

    TEST_F(LayerIteratorTest, SimpleTree)
    {
        scoped_ptr<TestLayerImpl> root_layer = CreateLayer();
        scoped_ptr<TestLayerImpl> first = CreateLayer();
        scoped_ptr<TestLayerImpl> second = CreateLayer();
        scoped_ptr<TestLayerImpl> third = CreateLayer();
        scoped_ptr<TestLayerImpl> fourth = CreateLayer();

        TestLayerImpl* root_ptr = root_layer.get();
        TestLayerImpl* first_ptr = first.get();
        TestLayerImpl* second_ptr = second.get();
        TestLayerImpl* third_ptr = third.get();
        TestLayerImpl* fourth_ptr = fourth.get();

        root_layer->AddChild(first.Pass());
        root_layer->AddChild(second.Pass());
        root_layer->AddChild(third.Pass());
        root_layer->AddChild(fourth.Pass());

        root_layer->SetHasRenderSurface(true);
        host_impl_.active_tree()->SetRootLayer(root_layer.Pass());

        LayerImplList render_surface_layer_list;
        LayerTreeHostCommon::CalcDrawPropsImplInputsForTesting inputs(
            root_ptr, root_ptr->bounds(), &render_surface_layer_list);
        LayerTreeHostCommon::CalculateDrawProperties(&inputs);

        IterateFrontToBack(&render_surface_layer_list);
        EXPECT_COUNT(root_ptr, 5, -1, 4);
        EXPECT_COUNT(first_ptr, -1, -1, 3);
        EXPECT_COUNT(second_ptr, -1, -1, 2);
        EXPECT_COUNT(third_ptr, -1, -1, 1);
        EXPECT_COUNT(fourth_ptr, -1, -1, 0);
    }

    TEST_F(LayerIteratorTest, ComplexTree)
    {
        scoped_ptr<TestLayerImpl> root_layer = CreateLayer();
        scoped_ptr<TestLayerImpl> root1 = CreateLayer();
        scoped_ptr<TestLayerImpl> root2 = CreateLayer();
        scoped_ptr<TestLayerImpl> root3 = CreateLayer();
        scoped_ptr<TestLayerImpl> root21 = CreateLayer();
        scoped_ptr<TestLayerImpl> root22 = CreateLayer();
        scoped_ptr<TestLayerImpl> root23 = CreateLayer();
        scoped_ptr<TestLayerImpl> root221 = CreateLayer();
        scoped_ptr<TestLayerImpl> root231 = CreateLayer();

        TestLayerImpl* root_ptr = root_layer.get();
        TestLayerImpl* root1_ptr = root1.get();
        TestLayerImpl* root2_ptr = root2.get();
        TestLayerImpl* root3_ptr = root3.get();
        TestLayerImpl* root21_ptr = root21.get();
        TestLayerImpl* root22_ptr = root22.get();
        TestLayerImpl* root23_ptr = root23.get();
        TestLayerImpl* root221_ptr = root221.get();
        TestLayerImpl* root231_ptr = root231.get();

        root22->AddChild(root221.Pass());
        root23->AddChild(root231.Pass());
        root2->AddChild(root21.Pass());
        root2->AddChild(root22.Pass());
        root2->AddChild(root23.Pass());
        root_layer->AddChild(root1.Pass());
        root_layer->AddChild(root2.Pass());
        root_layer->AddChild(root3.Pass());

        root_layer->SetHasRenderSurface(true);
        host_impl_.active_tree()->SetRootLayer(root_layer.Pass());

        LayerImplList render_surface_layer_list;
        LayerTreeHostCommon::CalcDrawPropsImplInputsForTesting inputs(
            root_ptr, root_ptr->bounds(), &render_surface_layer_list);
        LayerTreeHostCommon::CalculateDrawProperties(&inputs);

        IterateFrontToBack(&render_surface_layer_list);
        EXPECT_COUNT(root_ptr, 9, -1, 8);
        EXPECT_COUNT(root1_ptr, -1, -1, 7);
        EXPECT_COUNT(root2_ptr, -1, -1, 6);
        EXPECT_COUNT(root21_ptr, -1, -1, 5);
        EXPECT_COUNT(root22_ptr, -1, -1, 4);
        EXPECT_COUNT(root221_ptr, -1, -1, 3);
        EXPECT_COUNT(root23_ptr, -1, -1, 2);
        EXPECT_COUNT(root231_ptr, -1, -1, 1);
        EXPECT_COUNT(root3_ptr, -1, -1, 0);
    }

    TEST_F(LayerIteratorTest, ComplexTreeMultiSurface)
    {
        scoped_ptr<TestLayerImpl> root_layer = CreateLayer();
        scoped_ptr<TestLayerImpl> root1 = CreateLayer();
        scoped_ptr<TestLayerImpl> root2 = CreateLayer();
        scoped_ptr<TestLayerImpl> root3 = CreateLayer();
        scoped_ptr<TestLayerImpl> root21 = CreateLayer();
        scoped_ptr<TestLayerImpl> root22 = CreateLayer();
        scoped_ptr<TestLayerImpl> root23 = CreateLayer();
        scoped_ptr<TestLayerImpl> root221 = CreateLayer();
        scoped_ptr<TestLayerImpl> root231 = CreateLayer();

        TestLayerImpl* root_ptr = root_layer.get();
        TestLayerImpl* root1_ptr = root1.get();
        TestLayerImpl* root2_ptr = root2.get();
        TestLayerImpl* root3_ptr = root3.get();
        TestLayerImpl* root21_ptr = root21.get();
        TestLayerImpl* root22_ptr = root22.get();
        TestLayerImpl* root23_ptr = root23.get();
        TestLayerImpl* root221_ptr = root221.get();
        TestLayerImpl* root231_ptr = root231.get();

        root22->SetHasRenderSurface(true);
        root22->AddChild(root221.Pass());
        root23->SetHasRenderSurface(true);
        root23->AddChild(root231.Pass());
        root2->SetDrawsContent(false);
        root2->SetHasRenderSurface(true);
        root2->AddChild(root21.Pass());
        root2->AddChild(root22.Pass());
        root2->AddChild(root23.Pass());
        root_layer->AddChild(root1.Pass());
        root_layer->AddChild(root2.Pass());
        root_layer->AddChild(root3.Pass());

        root_layer->SetHasRenderSurface(true);
        host_impl_.active_tree()->SetRootLayer(root_layer.Pass());

        LayerImplList render_surface_layer_list;
        LayerTreeHostCommon::CalcDrawPropsImplInputsForTesting inputs(
            root_ptr, root_ptr->bounds(), &render_surface_layer_list);
        LayerTreeHostCommon::CalculateDrawProperties(&inputs);

        IterateFrontToBack(&render_surface_layer_list);
        EXPECT_COUNT(root_ptr, 14, -1, 13);
        EXPECT_COUNT(root1_ptr, -1, -1, 12);
        EXPECT_COUNT(root2_ptr, 10, 11, -1);
        EXPECT_COUNT(root21_ptr, -1, -1, 9);
        EXPECT_COUNT(root22_ptr, 7, 8, 6);
        EXPECT_COUNT(root221_ptr, -1, -1, 5);
        EXPECT_COUNT(root23_ptr, 3, 4, 2);
        EXPECT_COUNT(root231_ptr, -1, -1, 1);
        EXPECT_COUNT(root3_ptr, -1, -1, 0);
    }

} // namespace
} // namespace cc
