// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/layer.h"

#include "base/thread_task_runner_handle.h"
#include "cc/animation/keyframed_animation_curve.h"
#include "cc/base/math_util.h"
#include "cc/layers/layer_impl.h"
#include "cc/output/copy_output_request.h"
#include "cc/output/copy_output_result.h"
#include "cc/test/animation_test_common.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_client.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/geometry_test_utils.h"
#include "cc/test/layer_test_common.h"
#include "cc/test/test_gpu_memory_buffer_manager.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/single_thread_proxy.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/transform.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::Mock;
using ::testing::StrictMock;

#define EXPECT_SET_NEEDS_FULL_TREE_SYNC(expect, code_to_test)                   \
    do {                                                                        \
        EXPECT_CALL(*layer_tree_host_, SetNeedsFullTreeSync()).Times((expect)); \
        code_to_test;                                                           \
        Mock::VerifyAndClearExpectations(layer_tree_host_.get());               \
    } while (false)

namespace cc {
namespace {

    class MockLayerTreeHost : public LayerTreeHost {
    public:
        MockLayerTreeHost(LayerTreeHostSingleThreadClient* single_thread_client,
            LayerTreeHost::InitParams* params)
            : LayerTreeHost(params)
        {
            InitializeSingleThreaded(single_thread_client,
                base::ThreadTaskRunnerHandle::Get(), nullptr);
        }

        MOCK_METHOD0(SetNeedsCommit, void());
        MOCK_METHOD0(SetNeedsUpdateLayers, void());
        MOCK_METHOD0(SetNeedsFullTreeSync, void());
    };

    class LayerTest : public testing::Test {
    public:
        LayerTest()
            : host_impl_(&proxy_, &shared_bitmap_manager_, &task_graph_runner_)
            , fake_client_(FakeLayerTreeHostClient::DIRECT_3D)
        {
        }

    protected:
        void SetUp() override
        {
            LayerTreeHost::InitParams params;
            LayerTreeSettings settings;
            params.client = &fake_client_;
            params.settings = &settings;
            params.task_graph_runner = &task_graph_runner_;
            layer_tree_host_.reset(
                new StrictMock<MockLayerTreeHost>(&fake_client_, &params));
        }

        void TearDown() override
        {
            Mock::VerifyAndClearExpectations(layer_tree_host_.get());
            EXPECT_CALL(*layer_tree_host_, SetNeedsFullTreeSync()).Times(AnyNumber());
            parent_ = nullptr;
            child1_ = nullptr;
            child2_ = nullptr;
            child3_ = nullptr;
            grand_child1_ = nullptr;
            grand_child2_ = nullptr;
            grand_child3_ = nullptr;

            layer_tree_host_->SetRootLayer(nullptr);
            layer_tree_host_ = nullptr;
        }

        void VerifyTestTreeInitialState() const
        {
            ASSERT_EQ(3U, parent_->children().size());
            EXPECT_EQ(child1_, parent_->children()[0]);
            EXPECT_EQ(child2_, parent_->children()[1]);
            EXPECT_EQ(child3_, parent_->children()[2]);
            EXPECT_EQ(parent_.get(), child1_->parent());
            EXPECT_EQ(parent_.get(), child2_->parent());
            EXPECT_EQ(parent_.get(), child3_->parent());

            ASSERT_EQ(2U, child1_->children().size());
            EXPECT_EQ(grand_child1_, child1_->children()[0]);
            EXPECT_EQ(grand_child2_, child1_->children()[1]);
            EXPECT_EQ(child1_.get(), grand_child1_->parent());
            EXPECT_EQ(child1_.get(), grand_child2_->parent());

            ASSERT_EQ(1U, child2_->children().size());
            EXPECT_EQ(grand_child3_, child2_->children()[0]);
            EXPECT_EQ(child2_.get(), grand_child3_->parent());

            ASSERT_EQ(0U, child3_->children().size());
        }

        void CreateSimpleTestTree()
        {
            parent_ = Layer::Create(layer_settings_);
            child1_ = Layer::Create(layer_settings_);
            child2_ = Layer::Create(layer_settings_);
            child3_ = Layer::Create(layer_settings_);
            grand_child1_ = Layer::Create(layer_settings_);
            grand_child2_ = Layer::Create(layer_settings_);
            grand_child3_ = Layer::Create(layer_settings_);

            EXPECT_CALL(*layer_tree_host_, SetNeedsFullTreeSync()).Times(AnyNumber());
            layer_tree_host_->SetRootLayer(parent_);

            parent_->AddChild(child1_);
            parent_->AddChild(child2_);
            parent_->AddChild(child3_);
            child1_->AddChild(grand_child1_);
            child1_->AddChild(grand_child2_);
            child2_->AddChild(grand_child3_);

            Mock::VerifyAndClearExpectations(layer_tree_host_.get());

            VerifyTestTreeInitialState();
        }

        FakeImplProxy proxy_;
        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        FakeLayerTreeHostImpl host_impl_;

        FakeLayerTreeHostClient fake_client_;
        scoped_ptr<StrictMock<MockLayerTreeHost>> layer_tree_host_;
        scoped_refptr<Layer> parent_;
        scoped_refptr<Layer> child1_;
        scoped_refptr<Layer> child2_;
        scoped_refptr<Layer> child3_;
        scoped_refptr<Layer> grand_child1_;
        scoped_refptr<Layer> grand_child2_;
        scoped_refptr<Layer> grand_child3_;

        LayerSettings layer_settings_;
    };

    TEST_F(LayerTest, BasicCreateAndDestroy)
    {
        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        ASSERT_TRUE(test_layer.get());

        EXPECT_CALL(*layer_tree_host_, SetNeedsCommit()).Times(0);
        test_layer->SetLayerTreeHost(layer_tree_host_.get());
        Mock::VerifyAndClearExpectations(layer_tree_host_.get());

        EXPECT_CALL(*layer_tree_host_, SetNeedsCommit()).Times(0);
        test_layer->SetLayerTreeHost(nullptr);
    }

    TEST_F(LayerTest, AddAndRemoveChild)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child = Layer::Create(layer_settings_);

        // Upon creation, layers should not have children or parent.
        ASSERT_EQ(0U, parent->children().size());
        EXPECT_FALSE(child->parent());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(parent));
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->AddChild(child));

        ASSERT_EQ(1U, parent->children().size());
        EXPECT_EQ(child.get(), parent->children()[0].get());
        EXPECT_EQ(parent.get(), child->parent());
        EXPECT_EQ(parent.get(), child->RootLayer());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(AtLeast(1), child->RemoveFromParent());
    }

    TEST_F(LayerTest, AddSameChildTwice)
    {
        EXPECT_CALL(*layer_tree_host_, SetNeedsFullTreeSync()).Times(AtLeast(1));

        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child = Layer::Create(layer_settings_);

        layer_tree_host_->SetRootLayer(parent);

        ASSERT_EQ(0u, parent->children().size());

        parent->AddChild(child);
        ASSERT_EQ(1u, parent->children().size());
        EXPECT_EQ(parent.get(), child->parent());

        parent->AddChild(child);
        ASSERT_EQ(1u, parent->children().size());
        EXPECT_EQ(parent.get(), child->parent());
    }

    TEST_F(LayerTest, InsertChild)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child1 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child2 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child3 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child4 = Layer::Create(layer_settings_);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(parent));

        ASSERT_EQ(0U, parent->children().size());

        // Case 1: inserting to empty list.
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child3, 0));
        ASSERT_EQ(1U, parent->children().size());
        EXPECT_EQ(child3, parent->children()[0]);
        EXPECT_EQ(parent.get(), child3->parent());

        // Case 2: inserting to beginning of list
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child1, 0));
        ASSERT_EQ(2U, parent->children().size());
        EXPECT_EQ(child1, parent->children()[0]);
        EXPECT_EQ(child3, parent->children()[1]);
        EXPECT_EQ(parent.get(), child1->parent());

        // Case 3: inserting to middle of list
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child2, 1));
        ASSERT_EQ(3U, parent->children().size());
        EXPECT_EQ(child1, parent->children()[0]);
        EXPECT_EQ(child2, parent->children()[1]);
        EXPECT_EQ(child3, parent->children()[2]);
        EXPECT_EQ(parent.get(), child2->parent());

        // Case 4: inserting to end of list
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child4, 3));

        ASSERT_EQ(4U, parent->children().size());
        EXPECT_EQ(child1, parent->children()[0]);
        EXPECT_EQ(child2, parent->children()[1]);
        EXPECT_EQ(child3, parent->children()[2]);
        EXPECT_EQ(child4, parent->children()[3]);
        EXPECT_EQ(parent.get(), child4->parent());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(nullptr));
    }

    TEST_F(LayerTest, InsertChildPastEndOfList)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child1 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child2 = Layer::Create(layer_settings_);

        ASSERT_EQ(0U, parent->children().size());

        // insert to an out-of-bounds index
        parent->InsertChild(child1, 53);

        ASSERT_EQ(1U, parent->children().size());
        EXPECT_EQ(child1, parent->children()[0]);

        // insert another child to out-of-bounds, when list is not already empty.
        parent->InsertChild(child2, 2459);

        ASSERT_EQ(2U, parent->children().size());
        EXPECT_EQ(child1, parent->children()[0]);
        EXPECT_EQ(child2, parent->children()[1]);
    }

    TEST_F(LayerTest, InsertSameChildTwice)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child1 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child2 = Layer::Create(layer_settings_);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(parent));

        ASSERT_EQ(0U, parent->children().size());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child1, 0));
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child2, 1));

        ASSERT_EQ(2U, parent->children().size());
        EXPECT_EQ(child1, parent->children()[0]);
        EXPECT_EQ(child2, parent->children()[1]);

        // Inserting the same child again should cause the child to be removed and
        // re-inserted at the new location.
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(AtLeast(1), parent->InsertChild(child1, 1));

        // child1 should now be at the end of the list.
        ASSERT_EQ(2U, parent->children().size());
        EXPECT_EQ(child2, parent->children()[0]);
        EXPECT_EQ(child1, parent->children()[1]);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(nullptr));
    }

    TEST_F(LayerTest, ReplaceChildWithNewChild)
    {
        CreateSimpleTestTree();
        scoped_refptr<Layer> child4 = Layer::Create(layer_settings_);

        EXPECT_FALSE(child4->parent());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(
            AtLeast(1), parent_->ReplaceChild(child2_.get(), child4));
        EXPECT_FALSE(parent_->NeedsDisplayForTesting());
        EXPECT_FALSE(child1_->NeedsDisplayForTesting());
        EXPECT_FALSE(child2_->NeedsDisplayForTesting());
        EXPECT_FALSE(child3_->NeedsDisplayForTesting());
        EXPECT_FALSE(child4->NeedsDisplayForTesting());

        ASSERT_EQ(static_cast<size_t>(3), parent_->children().size());
        EXPECT_EQ(child1_, parent_->children()[0]);
        EXPECT_EQ(child4, parent_->children()[1]);
        EXPECT_EQ(child3_, parent_->children()[2]);
        EXPECT_EQ(parent_.get(), child4->parent());

        EXPECT_FALSE(child2_->parent());
    }

    TEST_F(LayerTest, ReplaceChildWithNewChildThatHasOtherParent)
    {
        CreateSimpleTestTree();

        // create another simple tree with test_layer and child4.
        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child4 = Layer::Create(layer_settings_);
        test_layer->AddChild(child4);
        ASSERT_EQ(1U, test_layer->children().size());
        EXPECT_EQ(child4, test_layer->children()[0]);
        EXPECT_EQ(test_layer.get(), child4->parent());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(
            AtLeast(1), parent_->ReplaceChild(child2_.get(), child4));

        ASSERT_EQ(3U, parent_->children().size());
        EXPECT_EQ(child1_, parent_->children()[0]);
        EXPECT_EQ(child4, parent_->children()[1]);
        EXPECT_EQ(child3_, parent_->children()[2]);
        EXPECT_EQ(parent_.get(), child4->parent());

        // test_layer should no longer have child4,
        // and child2 should no longer have a parent.
        ASSERT_EQ(0U, test_layer->children().size());
        EXPECT_FALSE(child2_->parent());
    }

    TEST_F(LayerTest, DeleteRemovedScrollParent)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child1 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child2 = Layer::Create(layer_settings_);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(parent));

        ASSERT_EQ(0U, parent->children().size());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child1, 0));
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child2, 1));

        ASSERT_EQ(2U, parent->children().size());
        EXPECT_EQ(child1, parent->children()[0]);
        EXPECT_EQ(child2, parent->children()[1]);

        EXPECT_SET_NEEDS_COMMIT(2, child1->SetScrollParent(child2.get()));

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, child2->RemoveFromParent());

        child1->reset_needs_push_properties_for_testing();

        EXPECT_SET_NEEDS_COMMIT(1, child2 = nullptr);

        EXPECT_TRUE(child1->needs_push_properties());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(nullptr));
    }

    TEST_F(LayerTest, DeleteRemovedScrollChild)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child1 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child2 = Layer::Create(layer_settings_);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(parent));

        ASSERT_EQ(0U, parent->children().size());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child1, 0));
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, parent->InsertChild(child2, 1));

        ASSERT_EQ(2U, parent->children().size());
        EXPECT_EQ(child1, parent->children()[0]);
        EXPECT_EQ(child2, parent->children()[1]);

        EXPECT_SET_NEEDS_COMMIT(2, child1->SetScrollParent(child2.get()));

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, child1->RemoveFromParent());

        child2->reset_needs_push_properties_for_testing();

        EXPECT_SET_NEEDS_COMMIT(1, child1 = nullptr);

        EXPECT_TRUE(child2->needs_push_properties());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(nullptr));
    }

    TEST_F(LayerTest, ReplaceChildWithSameChild)
    {
        CreateSimpleTestTree();

        // SetNeedsFullTreeSync / SetNeedsCommit should not be called because its the
        // same child.
        EXPECT_CALL(*layer_tree_host_, SetNeedsCommit()).Times(0);
        EXPECT_CALL(*layer_tree_host_, SetNeedsFullTreeSync()).Times(0);
        parent_->ReplaceChild(child2_.get(), child2_);

        VerifyTestTreeInitialState();
    }

    TEST_F(LayerTest, RemoveAllChildren)
    {
        CreateSimpleTestTree();

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(AtLeast(3), parent_->RemoveAllChildren());

        ASSERT_EQ(0U, parent_->children().size());
        EXPECT_FALSE(child1_->parent());
        EXPECT_FALSE(child2_->parent());
        EXPECT_FALSE(child3_->parent());
    }

    TEST_F(LayerTest, SetChildren)
    {
        scoped_refptr<Layer> old_parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> new_parent = Layer::Create(layer_settings_);

        scoped_refptr<Layer> child1 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child2 = Layer::Create(layer_settings_);

        LayerList new_children;
        new_children.push_back(child1);
        new_children.push_back(child2);

        // Set up and verify initial test conditions: child1 has a parent, child2 has
        // no parent.
        old_parent->AddChild(child1);
        ASSERT_EQ(0U, new_parent->children().size());
        EXPECT_EQ(old_parent.get(), child1->parent());
        EXPECT_FALSE(child2->parent());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(
            1, layer_tree_host_->SetRootLayer(new_parent));

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(
            AtLeast(1), new_parent->SetChildren(new_children));

        ASSERT_EQ(2U, new_parent->children().size());
        EXPECT_EQ(new_parent.get(), child1->parent());
        EXPECT_EQ(new_parent.get(), child2->parent());

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(nullptr));
    }

    TEST_F(LayerTest, HasAncestor)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        EXPECT_FALSE(parent->HasAncestor(parent.get()));

        scoped_refptr<Layer> child = Layer::Create(layer_settings_);
        parent->AddChild(child);

        EXPECT_FALSE(child->HasAncestor(child.get()));
        EXPECT_TRUE(child->HasAncestor(parent.get()));
        EXPECT_FALSE(parent->HasAncestor(child.get()));

        scoped_refptr<Layer> child_child = Layer::Create(layer_settings_);
        child->AddChild(child_child);

        EXPECT_FALSE(child_child->HasAncestor(child_child.get()));
        EXPECT_TRUE(child_child->HasAncestor(parent.get()));
        EXPECT_TRUE(child_child->HasAncestor(child.get()));
        EXPECT_FALSE(parent->HasAncestor(child.get()));
        EXPECT_FALSE(parent->HasAncestor(child_child.get()));
    }

    TEST_F(LayerTest, GetRootLayerAfterTreeManipulations)
    {
        CreateSimpleTestTree();

        // For this test we don't care about SetNeedsFullTreeSync calls.
        EXPECT_CALL(*layer_tree_host_, SetNeedsFullTreeSync()).Times(AnyNumber());

        scoped_refptr<Layer> child4 = Layer::Create(layer_settings_);

        EXPECT_EQ(parent_.get(), parent_->RootLayer());
        EXPECT_EQ(parent_.get(), child1_->RootLayer());
        EXPECT_EQ(parent_.get(), child2_->RootLayer());
        EXPECT_EQ(parent_.get(), child3_->RootLayer());
        EXPECT_EQ(child4.get(), child4->RootLayer());
        EXPECT_EQ(parent_.get(), grand_child1_->RootLayer());
        EXPECT_EQ(parent_.get(), grand_child2_->RootLayer());
        EXPECT_EQ(parent_.get(), grand_child3_->RootLayer());

        child1_->RemoveFromParent();

        // |child1| and its children, grand_child1 and grand_child2 are now on a
        // separate subtree.
        EXPECT_EQ(parent_.get(), parent_->RootLayer());
        EXPECT_EQ(child1_.get(), child1_->RootLayer());
        EXPECT_EQ(parent_.get(), child2_->RootLayer());
        EXPECT_EQ(parent_.get(), child3_->RootLayer());
        EXPECT_EQ(child4.get(), child4->RootLayer());
        EXPECT_EQ(child1_.get(), grand_child1_->RootLayer());
        EXPECT_EQ(child1_.get(), grand_child2_->RootLayer());
        EXPECT_EQ(parent_.get(), grand_child3_->RootLayer());

        grand_child3_->AddChild(child4);

        EXPECT_EQ(parent_.get(), parent_->RootLayer());
        EXPECT_EQ(child1_.get(), child1_->RootLayer());
        EXPECT_EQ(parent_.get(), child2_->RootLayer());
        EXPECT_EQ(parent_.get(), child3_->RootLayer());
        EXPECT_EQ(parent_.get(), child4->RootLayer());
        EXPECT_EQ(child1_.get(), grand_child1_->RootLayer());
        EXPECT_EQ(child1_.get(), grand_child2_->RootLayer());
        EXPECT_EQ(parent_.get(), grand_child3_->RootLayer());

        child2_->ReplaceChild(grand_child3_.get(), child1_);

        // |grand_child3| gets orphaned and the child1 subtree gets planted back into
        // the tree under child2.
        EXPECT_EQ(parent_.get(), parent_->RootLayer());
        EXPECT_EQ(parent_.get(), child1_->RootLayer());
        EXPECT_EQ(parent_.get(), child2_->RootLayer());
        EXPECT_EQ(parent_.get(), child3_->RootLayer());
        EXPECT_EQ(grand_child3_.get(), child4->RootLayer());
        EXPECT_EQ(parent_.get(), grand_child1_->RootLayer());
        EXPECT_EQ(parent_.get(), grand_child2_->RootLayer());
        EXPECT_EQ(grand_child3_.get(), grand_child3_->RootLayer());
    }

    TEST_F(LayerTest, CheckSetNeedsDisplayCausesCorrectBehavior)
    {
        // The semantics for SetNeedsDisplay which are tested here:
        //   1. sets NeedsDisplay flag appropriately.
        //   2. indirectly calls SetNeedsUpdate, exactly once for each call to
        //      SetNeedsDisplay.

        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(
            1, layer_tree_host_->SetRootLayer(test_layer));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetIsDrawable(true));

        gfx::Size test_bounds = gfx::Size(501, 508);

        gfx::Rect dirty1 = gfx::Rect(10, 15, 1, 2);
        gfx::Rect dirty2 = gfx::Rect(20, 25, 3, 4);
        gfx::Rect out_of_bounds_dirty_rect = gfx::Rect(400, 405, 500, 502);

        // Before anything, test_layer should not be dirty.
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());

        // This is just initialization, but SetNeedsCommit behavior is verified anyway
        // to avoid warnings.
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetBounds(test_bounds));
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());

        // The real test begins here.
        test_layer->ResetNeedsDisplayForTesting();
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());

        // Case 1: Layer should accept dirty rects that go beyond its bounds.
        test_layer->ResetNeedsDisplayForTesting();
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());
        EXPECT_SET_NEEDS_UPDATE(
            1, test_layer->SetNeedsDisplayRect(out_of_bounds_dirty_rect));
        EXPECT_TRUE(test_layer->NeedsDisplayForTesting());
        test_layer->ResetNeedsDisplayForTesting();

        // Case 2: SetNeedsDisplay() without the dirty rect arg.
        test_layer->ResetNeedsDisplayForTesting();
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());
        EXPECT_SET_NEEDS_UPDATE(1, test_layer->SetNeedsDisplay());
        EXPECT_TRUE(test_layer->NeedsDisplayForTesting());
        test_layer->ResetNeedsDisplayForTesting();

        // Case 3: SetNeedsDisplay() with an empty rect.
        test_layer->ResetNeedsDisplayForTesting();
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());
        EXPECT_SET_NEEDS_COMMIT(0, test_layer->SetNeedsDisplayRect(gfx::Rect()));
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());

        // Case 4: SetNeedsDisplay() with a non-drawable layer
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetIsDrawable(false));
        test_layer->ResetNeedsDisplayForTesting();
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());
        EXPECT_SET_NEEDS_UPDATE(0, test_layer->SetNeedsDisplayRect(dirty1));
        EXPECT_TRUE(test_layer->NeedsDisplayForTesting());
    }

    TEST_F(LayerTest, CheckPropertyChangeCausesCorrectBehavior)
    {
        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(
            1, layer_tree_host_->SetRootLayer(test_layer));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetIsDrawable(true));

        scoped_refptr<Layer> dummy_layer1 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> dummy_layer2 = Layer::Create(layer_settings_);

        // sanity check of initial test condition
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());

        // Next, test properties that should call SetNeedsCommit (but not
        // SetNeedsDisplay). All properties need to be set to new values in order for
        // SetNeedsCommit to be called.
        EXPECT_SET_NEEDS_COMMIT(
            1, test_layer->SetTransformOrigin(gfx::Point3F(1.23f, 4.56f, 0.f)));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetBackgroundColor(SK_ColorLTGRAY));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetMasksToBounds(true));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetOpacity(0.5f));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetBlendMode(SkXfermode::kHue_Mode));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetIsRootForIsolatedGroup(true));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetContentsOpaque(true));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetPosition(gfx::PointF(4.f, 9.f)));
        // We can use any layer pointer here since we aren't syncing for real.
        EXPECT_SET_NEEDS_COMMIT(1,
            test_layer->SetScrollClipLayerId(test_layer->id()));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetUserScrollable(true, false));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetScrollOffset(gfx::ScrollOffset(10, 10)));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetShouldScrollOnMainThread(true));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetNonFastScrollableRegion(Region(gfx::Rect(1, 1, 2, 2))));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetHaveWheelEventHandlers(true));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetHaveScrollEventHandlers(true));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetTransform(gfx::Transform(0.0, 0.0, 0.0, 0.0, 0.0, 0.0)));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetDoubleSided(false));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetTouchEventHandlerRegion(gfx::Rect(10, 10)));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetForceRenderSurface(true));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetHideLayerAndSubtree(true));

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, test_layer->SetMaskLayer(dummy_layer1.get()));
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, test_layer->SetReplicaLayer(dummy_layer2.get()));

        // The above tests should not have caused a change to the needs_display flag.
        EXPECT_FALSE(test_layer->NeedsDisplayForTesting());

        // As layers are removed from the tree, they will cause a tree sync.
        EXPECT_CALL(*layer_tree_host_, SetNeedsFullTreeSync()).Times((AnyNumber()));
    }

    TEST_F(LayerTest, PushPropertiesAccumulatesUpdateRect)
    {
        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        scoped_ptr<LayerImpl> impl_layer = LayerImpl::Create(host_impl_.active_tree(), 1);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1,
            layer_tree_host_->SetRootLayer(test_layer));

        test_layer->SetNeedsDisplayRect(gfx::Rect(5, 5));
        test_layer->PushPropertiesTo(impl_layer.get());
        EXPECT_FLOAT_RECT_EQ(gfx::RectF(0.f, 0.f, 5.f, 5.f),
            impl_layer->update_rect());

        // The LayerImpl's update_rect() should be accumulated here, since we did not
        // do anything to clear it.
        test_layer->SetNeedsDisplayRect(gfx::Rect(10, 10, 5, 5));
        test_layer->PushPropertiesTo(impl_layer.get());
        EXPECT_FLOAT_RECT_EQ(gfx::RectF(0.f, 0.f, 15.f, 15.f),
            impl_layer->update_rect());

        // If we do clear the LayerImpl side, then the next update_rect() should be
        // fresh without accumulation.
        impl_layer->ResetAllChangeTrackingForSubtree();
        test_layer->SetNeedsDisplayRect(gfx::Rect(10, 10, 5, 5));
        test_layer->PushPropertiesTo(impl_layer.get());
        EXPECT_FLOAT_RECT_EQ(gfx::RectF(10.f, 10.f, 5.f, 5.f),
            impl_layer->update_rect());
    }

    TEST_F(LayerTest, PushPropertiesCausesLayerPropertyChangedForTransform)
    {
        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        scoped_ptr<LayerImpl> impl_layer = LayerImpl::Create(host_impl_.active_tree(), 1);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1,
            layer_tree_host_->SetRootLayer(test_layer));

        gfx::Transform transform;
        transform.Rotate(45.0);
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetTransform(transform));

        EXPECT_FALSE(impl_layer->LayerPropertyChanged());

        test_layer->PushPropertiesTo(impl_layer.get());

        EXPECT_TRUE(impl_layer->LayerPropertyChanged());
    }

    TEST_F(LayerTest, PushPropertiesCausesLayerPropertyChangedForOpacity)
    {
        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        scoped_ptr<LayerImpl> impl_layer = LayerImpl::Create(host_impl_.active_tree(), 1);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1,
            layer_tree_host_->SetRootLayer(test_layer));

        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetOpacity(0.5f));

        EXPECT_FALSE(impl_layer->LayerPropertyChanged());

        test_layer->PushPropertiesTo(impl_layer.get());

        EXPECT_TRUE(impl_layer->LayerPropertyChanged());
    }

    TEST_F(LayerTest,
        PushPropsDoesntCauseLayerPropertyChangedDuringImplOnlyTransformAnim)
    {
        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        scoped_ptr<LayerImpl> impl_layer = LayerImpl::Create(host_impl_.active_tree(), 1);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1,
            layer_tree_host_->SetRootLayer(test_layer));

        scoped_ptr<AnimationRegistrar> registrar = AnimationRegistrar::Create();
        impl_layer->layer_animation_controller()->SetAnimationRegistrar(
            registrar.get());

        AddAnimatedTransformToController(impl_layer->layer_animation_controller(),
            1.0,
            0,
            100);

        gfx::Transform transform;
        transform.Rotate(45.0);
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetTransform(transform));

        EXPECT_FALSE(impl_layer->LayerPropertyChanged());
        test_layer->PushPropertiesTo(impl_layer.get());
        EXPECT_TRUE(impl_layer->LayerPropertyChanged());

        impl_layer->ResetAllChangeTrackingForSubtree();
        AddAnimatedTransformToController(impl_layer->layer_animation_controller(),
            1.0,
            0,
            100);
        impl_layer->layer_animation_controller()
            ->GetAnimation(Animation::TRANSFORM)
            ->set_is_impl_only(true);
        transform.Rotate(45.0);
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetTransform(transform));

        EXPECT_FALSE(impl_layer->LayerPropertyChanged());
        test_layer->PushPropertiesTo(impl_layer.get());
        EXPECT_FALSE(impl_layer->LayerPropertyChanged());
    }

    TEST_F(LayerTest,
        PushPropsDoesntCauseLayerPropertyChangedDuringImplOnlyOpacityAnim)
    {
        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        scoped_ptr<LayerImpl> impl_layer = LayerImpl::Create(host_impl_.active_tree(), 1);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1,
            layer_tree_host_->SetRootLayer(test_layer));

        scoped_ptr<AnimationRegistrar> registrar = AnimationRegistrar::Create();
        impl_layer->layer_animation_controller()->SetAnimationRegistrar(
            registrar.get());

        AddOpacityTransitionToController(impl_layer->layer_animation_controller(),
            1.0,
            0.3f,
            0.7f,
            false);

        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetOpacity(0.5f));

        EXPECT_FALSE(impl_layer->LayerPropertyChanged());
        test_layer->PushPropertiesTo(impl_layer.get());
        EXPECT_TRUE(impl_layer->LayerPropertyChanged());

        impl_layer->ResetAllChangeTrackingForSubtree();
        AddOpacityTransitionToController(impl_layer->layer_animation_controller(),
            1.0,
            0.3f,
            0.7f,
            false);
        impl_layer->layer_animation_controller()
            ->GetAnimation(Animation::OPACITY)
            ->set_is_impl_only(true);
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetOpacity(0.75f));

        EXPECT_FALSE(impl_layer->LayerPropertyChanged());
        test_layer->PushPropertiesTo(impl_layer.get());
        EXPECT_FALSE(impl_layer->LayerPropertyChanged());
    }

    TEST_F(LayerTest,
        PushPropsDoesntCauseLayerPropertyChangedDuringImplOnlyFilterAnim)
    {
        scoped_refptr<Layer> test_layer = Layer::Create(layer_settings_);
        scoped_ptr<LayerImpl> impl_layer = LayerImpl::Create(host_impl_.active_tree(), 1);

        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1,
            layer_tree_host_->SetRootLayer(test_layer));

        scoped_ptr<AnimationRegistrar> registrar = AnimationRegistrar::Create();
        impl_layer->layer_animation_controller()->SetAnimationRegistrar(
            registrar.get());

        AddAnimatedFilterToController(
            impl_layer->layer_animation_controller(), 1.0, 1.f, 2.f);

        FilterOperations filters;
        filters.Append(FilterOperation::CreateBlurFilter(2.f));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetFilters(filters));

        EXPECT_FALSE(impl_layer->LayerPropertyChanged());
        test_layer->PushPropertiesTo(impl_layer.get());
        EXPECT_TRUE(impl_layer->LayerPropertyChanged());

        impl_layer->ResetAllChangeTrackingForSubtree();
        AddAnimatedFilterToController(
            impl_layer->layer_animation_controller(), 1.0, 1.f, 2.f);
        impl_layer->layer_animation_controller()
            ->GetAnimation(Animation::FILTER)
            ->set_is_impl_only(true);
        filters.Append(FilterOperation::CreateSepiaFilter(0.5f));
        EXPECT_SET_NEEDS_COMMIT(1, test_layer->SetFilters(filters));

        EXPECT_FALSE(impl_layer->LayerPropertyChanged());
        test_layer->PushPropertiesTo(impl_layer.get());
        EXPECT_FALSE(impl_layer->LayerPropertyChanged());
    }

    TEST_F(LayerTest, MaskAndReplicaHasParent)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child = Layer::Create(layer_settings_);
        scoped_refptr<Layer> mask = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica_mask = Layer::Create(layer_settings_);
        scoped_refptr<Layer> mask_replacement = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica_replacement = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica_mask_replacement = Layer::Create(layer_settings_);

        parent->AddChild(child);
        child->SetMaskLayer(mask.get());
        child->SetReplicaLayer(replica.get());
        replica->SetMaskLayer(replica_mask.get());

        EXPECT_EQ(parent.get(), child->parent());
        EXPECT_EQ(child.get(), mask->parent());
        EXPECT_EQ(child.get(), replica->parent());
        EXPECT_EQ(replica.get(), replica_mask->parent());

        replica->SetMaskLayer(replica_mask_replacement.get());
        EXPECT_EQ(nullptr, replica_mask->parent());
        EXPECT_EQ(replica.get(), replica_mask_replacement->parent());

        child->SetMaskLayer(mask_replacement.get());
        EXPECT_EQ(nullptr, mask->parent());
        EXPECT_EQ(child.get(), mask_replacement->parent());

        child->SetReplicaLayer(replica_replacement.get());
        EXPECT_EQ(nullptr, replica->parent());
        EXPECT_EQ(child.get(), replica_replacement->parent());

        EXPECT_EQ(replica.get(), replica->mask_layer()->parent());
    }

    TEST_F(LayerTest, CheckTransformIsInvertible)
    {
        scoped_refptr<Layer> layer = Layer::Create(layer_settings_);
        scoped_ptr<LayerImpl> impl_layer = LayerImpl::Create(host_impl_.active_tree(), 1);
        EXPECT_CALL(*layer_tree_host_, SetNeedsFullTreeSync()).Times(1);
        EXPECT_CALL(*layer_tree_host_, SetNeedsCommit()).Times(AnyNumber());
        layer_tree_host_->SetRootLayer(layer);

        EXPECT_TRUE(layer->transform_is_invertible());

        gfx::Transform singular_transform;
        singular_transform.Scale3d(
            SkDoubleToMScalar(1.0), SkDoubleToMScalar(1.0), SkDoubleToMScalar(0.0));

        layer->SetTransform(singular_transform);
        layer->PushPropertiesTo(impl_layer.get());

        EXPECT_FALSE(layer->transform_is_invertible());
        EXPECT_FALSE(impl_layer->transform_is_invertible());

        gfx::Transform rotation_transform;
        rotation_transform.RotateAboutZAxis(-45.0);

        layer->SetTransform(rotation_transform);
        layer->PushPropertiesTo(impl_layer.get());
        EXPECT_TRUE(layer->transform_is_invertible());
        EXPECT_TRUE(impl_layer->transform_is_invertible());

        Mock::VerifyAndClearExpectations(layer_tree_host_.get());
    }

    TEST_F(LayerTest, TransformIsInvertibleAnimation)
    {
        scoped_refptr<Layer> layer = Layer::Create(layer_settings_);
        scoped_ptr<LayerImpl> impl_layer = LayerImpl::Create(host_impl_.active_tree(), 1);
        EXPECT_CALL(*layer_tree_host_, SetNeedsFullTreeSync()).Times(1);
        EXPECT_CALL(*layer_tree_host_, SetNeedsCommit()).Times(AnyNumber());
        layer_tree_host_->SetRootLayer(layer);

        EXPECT_TRUE(layer->transform_is_invertible());

        gfx::Transform singular_transform;
        singular_transform.Scale3d(
            SkDoubleToMScalar(1.0), SkDoubleToMScalar(1.0), SkDoubleToMScalar(0.0));

        layer->SetTransform(singular_transform);
        layer->PushPropertiesTo(impl_layer.get());

        EXPECT_FALSE(layer->transform_is_invertible());
        EXPECT_FALSE(impl_layer->transform_is_invertible());

        gfx::Transform identity_transform;

        EXPECT_CALL(*layer_tree_host_, SetNeedsUpdateLayers()).Times(1);
        layer->SetTransform(identity_transform);
        layer->OnTransformAnimated(singular_transform);
        layer->PushPropertiesTo(impl_layer.get());
        EXPECT_FALSE(layer->transform_is_invertible());
        EXPECT_FALSE(impl_layer->transform_is_invertible());

        Mock::VerifyAndClearExpectations(layer_tree_host_.get());
    }

    class LayerTreeHostFactory {
    public:
        LayerTreeHostFactory()
            : client_(FakeLayerTreeHostClient::DIRECT_3D)
        {
        }

        scoped_ptr<LayerTreeHost> Create() { return Create(LayerTreeSettings()); }

        scoped_ptr<LayerTreeHost> Create(LayerTreeSettings settings)
        {
            LayerTreeHost::InitParams params;
            params.client = &client_;
            params.shared_bitmap_manager = &shared_bitmap_manager_;
            params.task_graph_runner = &task_graph_runner_;
            params.gpu_memory_buffer_manager = &gpu_memory_buffer_manager_;
            params.settings = &settings;
            params.main_task_runner = base::ThreadTaskRunnerHandle::Get();
            return LayerTreeHost::CreateSingleThreaded(&client_, &params);
        }

    private:
        FakeLayerTreeHostClient client_;
        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        TestGpuMemoryBufferManager gpu_memory_buffer_manager_;
    };

    void AssertLayerTreeHostMatchesForSubtree(Layer* layer, LayerTreeHost* host)
    {
        EXPECT_EQ(host, layer->layer_tree_host());

        for (size_t i = 0; i < layer->children().size(); ++i)
            AssertLayerTreeHostMatchesForSubtree(layer->children()[i].get(), host);

        if (layer->mask_layer())
            AssertLayerTreeHostMatchesForSubtree(layer->mask_layer(), host);

        if (layer->replica_layer())
            AssertLayerTreeHostMatchesForSubtree(layer->replica_layer(), host);
    }

    class LayerLayerTreeHostTest : public testing::Test {
    public:
    protected:
        LayerSettings layer_settings_;
    };

    TEST_F(LayerLayerTreeHostTest, EnteringTree)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child = Layer::Create(layer_settings_);
        scoped_refptr<Layer> mask = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica_mask = Layer::Create(layer_settings_);

        // Set up a detached tree of layers. The host pointer should be nil for these
        // layers.
        parent->AddChild(child);
        child->SetMaskLayer(mask.get());
        child->SetReplicaLayer(replica.get());
        replica->SetMaskLayer(replica_mask.get());

        AssertLayerTreeHostMatchesForSubtree(parent.get(), nullptr);

        LayerTreeHostFactory factory;
        scoped_ptr<LayerTreeHost> layer_tree_host = factory.Create();
        // Setting the root layer should set the host pointer for all layers in the
        // tree.
        layer_tree_host->SetRootLayer(parent.get());

        AssertLayerTreeHostMatchesForSubtree(parent.get(), layer_tree_host.get());

        // Clearing the root layer should also clear out the host pointers for all
        // layers in the tree.
        layer_tree_host->SetRootLayer(nullptr);

        AssertLayerTreeHostMatchesForSubtree(parent.get(), nullptr);
    }

    TEST_F(LayerLayerTreeHostTest, AddingLayerSubtree)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        LayerTreeHostFactory factory;
        scoped_ptr<LayerTreeHost> layer_tree_host = factory.Create();

        layer_tree_host->SetRootLayer(parent.get());

        EXPECT_EQ(parent->layer_tree_host(), layer_tree_host.get());

        // Adding a subtree to a layer already associated with a host should set the
        // host pointer on all layers in that subtree.
        scoped_refptr<Layer> child = Layer::Create(layer_settings_);
        scoped_refptr<Layer> grand_child = Layer::Create(layer_settings_);
        child->AddChild(grand_child);

        // Masks, replicas, and replica masks should pick up the new host too.
        scoped_refptr<Layer> child_mask = Layer::Create(layer_settings_);
        child->SetMaskLayer(child_mask.get());
        scoped_refptr<Layer> child_replica = Layer::Create(layer_settings_);
        child->SetReplicaLayer(child_replica.get());
        scoped_refptr<Layer> child_replica_mask = Layer::Create(layer_settings_);
        child_replica->SetMaskLayer(child_replica_mask.get());

        parent->AddChild(child);
        AssertLayerTreeHostMatchesForSubtree(parent.get(), layer_tree_host.get());

        layer_tree_host->SetRootLayer(nullptr);
    }

    TEST_F(LayerLayerTreeHostTest, ChangeHost)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child = Layer::Create(layer_settings_);
        scoped_refptr<Layer> mask = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica_mask = Layer::Create(layer_settings_);

        // Same setup as the previous test.
        parent->AddChild(child);
        child->SetMaskLayer(mask.get());
        child->SetReplicaLayer(replica.get());
        replica->SetMaskLayer(replica_mask.get());

        LayerTreeHostFactory factory;
        scoped_ptr<LayerTreeHost> first_layer_tree_host = factory.Create();
        first_layer_tree_host->SetRootLayer(parent.get());

        AssertLayerTreeHostMatchesForSubtree(parent.get(),
            first_layer_tree_host.get());

        // Now re-root the tree to a new host (simulating what we do on a context lost
        // event). This should update the host pointers for all layers in the tree.
        scoped_ptr<LayerTreeHost> second_layer_tree_host = factory.Create();
        second_layer_tree_host->SetRootLayer(parent.get());

        AssertLayerTreeHostMatchesForSubtree(parent.get(),
            second_layer_tree_host.get());

        second_layer_tree_host->SetRootLayer(nullptr);
    }

    TEST_F(LayerLayerTreeHostTest, ChangeHostInSubtree)
    {
        scoped_refptr<Layer> first_parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> first_child = Layer::Create(layer_settings_);
        scoped_refptr<Layer> second_parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> second_child = Layer::Create(layer_settings_);
        scoped_refptr<Layer> second_grand_child = Layer::Create(layer_settings_);

        // First put all children under the first parent and set the first host.
        first_parent->AddChild(first_child);
        second_child->AddChild(second_grand_child);
        first_parent->AddChild(second_child);

        LayerTreeHostFactory factory;
        scoped_ptr<LayerTreeHost> first_layer_tree_host = factory.Create();
        first_layer_tree_host->SetRootLayer(first_parent.get());

        AssertLayerTreeHostMatchesForSubtree(first_parent.get(),
            first_layer_tree_host.get());

        // Now reparent the subtree starting at second_child to a layer in a different
        // tree.
        scoped_ptr<LayerTreeHost> second_layer_tree_host = factory.Create();
        second_layer_tree_host->SetRootLayer(second_parent.get());

        second_parent->AddChild(second_child);

        // The moved layer and its children should point to the new host.
        EXPECT_EQ(second_layer_tree_host.get(), second_child->layer_tree_host());
        EXPECT_EQ(second_layer_tree_host.get(),
            second_grand_child->layer_tree_host());

        // Test over, cleanup time.
        first_layer_tree_host->SetRootLayer(nullptr);
        second_layer_tree_host->SetRootLayer(nullptr);
    }

    TEST_F(LayerLayerTreeHostTest, ReplaceMaskAndReplicaLayer)
    {
        scoped_refptr<Layer> parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> mask = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica = Layer::Create(layer_settings_);
        scoped_refptr<Layer> mask_child = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica_child = Layer::Create(layer_settings_);
        scoped_refptr<Layer> mask_replacement = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica_replacement = Layer::Create(layer_settings_);

        parent->SetMaskLayer(mask.get());
        parent->SetReplicaLayer(replica.get());
        mask->AddChild(mask_child);
        replica->AddChild(replica_child);

        LayerTreeHostFactory factory;
        scoped_ptr<LayerTreeHost> layer_tree_host = factory.Create();
        layer_tree_host->SetRootLayer(parent.get());

        AssertLayerTreeHostMatchesForSubtree(parent.get(), layer_tree_host.get());

        // Replacing the mask should clear out the old mask's subtree's host pointers.
        parent->SetMaskLayer(mask_replacement.get());
        EXPECT_EQ(nullptr, mask->layer_tree_host());
        EXPECT_EQ(nullptr, mask_child->layer_tree_host());

        // Same for replacing a replica layer.
        parent->SetReplicaLayer(replica_replacement.get());
        EXPECT_EQ(nullptr, replica->layer_tree_host());
        EXPECT_EQ(nullptr, replica_child->layer_tree_host());

        // Test over, cleanup time.
        layer_tree_host->SetRootLayer(nullptr);
    }

    TEST_F(LayerLayerTreeHostTest, DestroyHostWithNonNullRootLayer)
    {
        scoped_refptr<Layer> root = Layer::Create(layer_settings_);
        scoped_refptr<Layer> child = Layer::Create(layer_settings_);
        root->AddChild(child);
        LayerTreeHostFactory factory;
        scoped_ptr<LayerTreeHost> layer_tree_host = factory.Create();
        layer_tree_host->SetRootLayer(root);
    }

    static bool AddTestAnimation(Layer* layer)
    {
        scoped_ptr<KeyframedFloatAnimationCurve> curve = KeyframedFloatAnimationCurve::Create();
        curve->AddKeyframe(FloatKeyframe::Create(base::TimeDelta(), 0.3f, nullptr));
        curve->AddKeyframe(
            FloatKeyframe::Create(base::TimeDelta::FromSecondsD(1.0), 0.7f, nullptr));
        scoped_ptr<Animation> animation = Animation::Create(curve.Pass(), 0, 0, Animation::OPACITY);

        return layer->AddAnimation(animation.Pass());
    }

    TEST_F(LayerLayerTreeHostTest, ShouldNotAddAnimationWithoutAnimationRegistrar)
    {
        scoped_refptr<Layer> layer = Layer::Create(layer_settings_);

        // Case 1: without a LayerTreeHost and without an AnimationRegistrar, the
        // animation should not be accepted.
        EXPECT_FALSE(AddTestAnimation(layer.get()));

        scoped_ptr<AnimationRegistrar> registrar = AnimationRegistrar::Create();
        layer->RegisterForAnimations(registrar.get());

        // Case 2: with an AnimationRegistrar, the animation should be accepted.
        EXPECT_TRUE(AddTestAnimation(layer.get()));

        LayerTreeSettings settings;
        settings.accelerated_animation_enabled = false;
        LayerTreeHostFactory factory;
        scoped_ptr<LayerTreeHost> layer_tree_host = factory.Create(settings);
        layer_tree_host->SetRootLayer(layer);
        AssertLayerTreeHostMatchesForSubtree(layer.get(), layer_tree_host.get());

        // Case 3: with a LayerTreeHost where accelerated animation is disabled, the
        // animation should be rejected.
        EXPECT_FALSE(AddTestAnimation(layer.get()));
    }

    TEST_F(LayerTest, SafeOpaqueBackgroundColor)
    {
        LayerTreeHostFactory factory;
        scoped_ptr<LayerTreeHost> layer_tree_host = factory.Create();

        scoped_refptr<Layer> layer = Layer::Create(layer_settings_);
        layer_tree_host->SetRootLayer(layer);

        for (int contents_opaque = 0; contents_opaque < 2; ++contents_opaque) {
            for (int layer_opaque = 0; layer_opaque < 2; ++layer_opaque) {
                for (int host_opaque = 0; host_opaque < 2; ++host_opaque) {
                    layer->SetContentsOpaque(!!contents_opaque);
                    layer->SetBackgroundColor(layer_opaque ? SK_ColorRED
                                                           : SK_ColorTRANSPARENT);
                    layer_tree_host->set_background_color(
                        host_opaque ? SK_ColorRED : SK_ColorTRANSPARENT);

                    SkColor safe_color = layer->SafeOpaqueBackgroundColor();
                    if (contents_opaque) {
                        EXPECT_EQ(SkColorGetA(safe_color), 255u)
                            << "Flags: " << contents_opaque << ", " << layer_opaque << ", "
                            << host_opaque << "\n";
                    } else {
                        EXPECT_NE(SkColorGetA(safe_color), 255u)
                            << "Flags: " << contents_opaque << ", " << layer_opaque << ", "
                            << host_opaque << "\n";
                    }
                }
            }
        }
    }

    class DrawsContentChangeLayer : public Layer {
    public:
        static scoped_refptr<DrawsContentChangeLayer> Create(
            const LayerSettings& settings)
        {
            return make_scoped_refptr(new DrawsContentChangeLayer(settings));
        }

        void SetLayerTreeHost(LayerTreeHost* host) override
        {
            Layer::SetLayerTreeHost(host);
            SetFakeDrawsContent(!fake_draws_content_);
        }

        bool HasDrawableContent() const override
        {
            return fake_draws_content_ && Layer::HasDrawableContent();
        }

        void SetFakeDrawsContent(bool fake_draws_content)
        {
            fake_draws_content_ = fake_draws_content;
            UpdateDrawsContent(HasDrawableContent());
        }

    private:
        explicit DrawsContentChangeLayer(const LayerSettings& settings)
            : Layer(settings)
            , fake_draws_content_(false)
        {
        }
        ~DrawsContentChangeLayer() override { }

        bool fake_draws_content_;
    };

    TEST_F(LayerTest, DrawsContentChangedInSetLayerTreeHost)
    {
        scoped_refptr<Layer> root_layer = Layer::Create(layer_settings_);
        scoped_refptr<DrawsContentChangeLayer> becomes_not_draws_content = DrawsContentChangeLayer::Create(layer_settings_);
        scoped_refptr<DrawsContentChangeLayer> becomes_draws_content = DrawsContentChangeLayer::Create(layer_settings_);
        root_layer->SetIsDrawable(true);
        becomes_not_draws_content->SetIsDrawable(true);
        becomes_not_draws_content->SetFakeDrawsContent(true);
        EXPECT_EQ(0, root_layer->NumDescendantsThatDrawContent());
        root_layer->AddChild(becomes_not_draws_content);
        EXPECT_EQ(0, root_layer->NumDescendantsThatDrawContent());

        becomes_draws_content->SetIsDrawable(true);
        root_layer->AddChild(becomes_draws_content);
        EXPECT_EQ(1, root_layer->NumDescendantsThatDrawContent());
    }

    void ReceiveCopyOutputResult(int* result_count,
        scoped_ptr<CopyOutputResult> result)
    {
        ++(*result_count);
    }

    TEST_F(LayerTest, DedupesCopyOutputRequestsBySource)
    {
        scoped_refptr<Layer> layer = Layer::Create(layer_settings_);
        int result_count = 0;

        // Create identical requests without the source being set, and expect the
        // layer does not abort either one.
        scoped_ptr<CopyOutputRequest> request = CopyOutputRequest::CreateRequest(
            base::Bind(&ReceiveCopyOutputResult, &result_count));
        layer->RequestCopyOfOutput(request.Pass());
        EXPECT_EQ(0, result_count);
        request = CopyOutputRequest::CreateRequest(
            base::Bind(&ReceiveCopyOutputResult, &result_count));
        layer->RequestCopyOfOutput(request.Pass());
        EXPECT_EQ(0, result_count);

        // When the layer is destroyed, expect both requests to be aborted.
        layer = nullptr;
        EXPECT_EQ(2, result_count);

        layer = Layer::Create(layer_settings_);
        result_count = 0;

        // Create identical requests, but this time the source is being set.  Expect
        // the first request from |this| source aborts immediately when the second
        // request from |this| source is made.
        int did_receive_first_result_from_this_source = 0;
        request = CopyOutputRequest::CreateRequest(base::Bind(
            &ReceiveCopyOutputResult, &did_receive_first_result_from_this_source));
        request->set_source(this);
        layer->RequestCopyOfOutput(request.Pass());
        EXPECT_EQ(0, did_receive_first_result_from_this_source);
        // Make a request from a different source.
        int did_receive_result_from_different_source = 0;
        request = CopyOutputRequest::CreateRequest(base::Bind(
            &ReceiveCopyOutputResult, &did_receive_result_from_different_source));
        request->set_source(reinterpret_cast<void*>(0xdeadbee0));
        layer->RequestCopyOfOutput(request.Pass());
        EXPECT_EQ(0, did_receive_result_from_different_source);
        // Make a request without specifying the source.
        int did_receive_result_from_anonymous_source = 0;
        request = CopyOutputRequest::CreateRequest(base::Bind(
            &ReceiveCopyOutputResult, &did_receive_result_from_anonymous_source));
        layer->RequestCopyOfOutput(request.Pass());
        EXPECT_EQ(0, did_receive_result_from_anonymous_source);
        // Make the second request from |this| source.
        int did_receive_second_result_from_this_source = 0;
        request = CopyOutputRequest::CreateRequest(base::Bind(
            &ReceiveCopyOutputResult, &did_receive_second_result_from_this_source));
        request->set_source(this);
        layer->RequestCopyOfOutput(request.Pass()); // First request to be aborted.
        EXPECT_EQ(1, did_receive_first_result_from_this_source);
        EXPECT_EQ(0, did_receive_result_from_different_source);
        EXPECT_EQ(0, did_receive_result_from_anonymous_source);
        EXPECT_EQ(0, did_receive_second_result_from_this_source);

        // When the layer is destroyed, the other three requests should be aborted.
        layer = nullptr;
        EXPECT_EQ(1, did_receive_first_result_from_this_source);
        EXPECT_EQ(1, did_receive_result_from_different_source);
        EXPECT_EQ(1, did_receive_result_from_anonymous_source);
        EXPECT_EQ(1, did_receive_second_result_from_this_source);
    }

    TEST_F(LayerTest, AnimationSchedulesLayerUpdate)
    {
        scoped_refptr<Layer> layer = Layer::Create(layer_settings_);
        EXPECT_SET_NEEDS_FULL_TREE_SYNC(1, layer_tree_host_->SetRootLayer(layer));

        EXPECT_CALL(*layer_tree_host_, SetNeedsUpdateLayers()).Times(1);
        layer->OnOpacityAnimated(0.5f);
        Mock::VerifyAndClearExpectations(layer_tree_host_.get());

        EXPECT_CALL(*layer_tree_host_, SetNeedsUpdateLayers()).Times(1);
        gfx::Transform transform;
        transform.Rotate(45.0);
        layer->OnTransformAnimated(transform);
        Mock::VerifyAndClearExpectations(layer_tree_host_.get());

        // Scroll offset animation should not schedule a layer update since it is
        // handled similarly to normal compositor scroll updates.
        EXPECT_CALL(*layer_tree_host_, SetNeedsUpdateLayers()).Times(0);
        layer->OnScrollOffsetAnimated(gfx::ScrollOffset(10, 10));
        Mock::VerifyAndClearExpectations(layer_tree_host_.get());
    }

} // namespace
} // namespace cc
