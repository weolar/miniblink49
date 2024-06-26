// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/tree_synchronizer.h"

#include <algorithm>
#include <set>
#include <vector>

#include "base/format_macros.h"
#include "base/strings/stringprintf.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_impl.h"
#include "cc/test/animation_test_common.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/fake_rendering_stats_instrumentation.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/proxy.h"
#include "cc/trees/single_thread_proxy.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class MockLayerImpl : public LayerImpl {
    public:
        static scoped_ptr<MockLayerImpl> Create(LayerTreeImpl* tree_impl,
            int layer_id)
        {
            return make_scoped_ptr(new MockLayerImpl(tree_impl, layer_id));
        }
        ~MockLayerImpl() override
        {
            if (layer_impl_destruction_list_)
                layer_impl_destruction_list_->push_back(id());
        }

        void SetLayerImplDestructionList(std::vector<int>* list)
        {
            layer_impl_destruction_list_ = list;
        }

    private:
        MockLayerImpl(LayerTreeImpl* tree_impl, int layer_id)
            : LayerImpl(tree_impl, layer_id)
            , layer_impl_destruction_list_(NULL)
        {
        }

        std::vector<int>* layer_impl_destruction_list_;
    };

    class MockLayer : public Layer {
    public:
        static scoped_refptr<MockLayer> Create(
            const LayerSettings& settings,
            std::vector<int>* layer_impl_destruction_list)
        {
            return make_scoped_refptr(
                new MockLayer(settings, layer_impl_destruction_list));
        }

        scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override
        {
            return MockLayerImpl::Create(tree_impl, layer_id_);
        }

        void PushPropertiesTo(LayerImpl* layer_impl) override
        {
            Layer::PushPropertiesTo(layer_impl);

            MockLayerImpl* mock_layer_impl = static_cast<MockLayerImpl*>(layer_impl);
            mock_layer_impl->SetLayerImplDestructionList(layer_impl_destruction_list_);
        }

    private:
        explicit MockLayer(const LayerSettings& settings,
            std::vector<int>* layer_impl_destruction_list)
            : Layer(settings)
            , layer_impl_destruction_list_(layer_impl_destruction_list)
        {
        }
        ~MockLayer() override { }

        std::vector<int>* layer_impl_destruction_list_;
    };

    class FakeLayerAnimationController : public LayerAnimationController {
    public:
        static scoped_refptr<LayerAnimationController> Create()
        {
            return static_cast<LayerAnimationController*>(
                new FakeLayerAnimationController);
        }

        bool SynchronizedAnimations() const { return synchronized_animations_; }

    private:
        FakeLayerAnimationController()
            : LayerAnimationController(1)
            , synchronized_animations_(false)
        {
        }

        ~FakeLayerAnimationController() override { }

        void PushAnimationUpdatesTo(
            LayerAnimationController* controller_impl) override
        {
            LayerAnimationController::PushAnimationUpdatesTo(controller_impl);
            synchronized_animations_ = true;
        }

        bool synchronized_animations_;
    };

    void ExpectTreesAreIdentical(Layer* layer,
        LayerImpl* layer_impl,
        LayerTreeImpl* tree_impl)
    {
        ASSERT_TRUE(layer);
        ASSERT_TRUE(layer_impl);

        EXPECT_EQ(layer->id(), layer_impl->id());
        EXPECT_EQ(layer_impl->layer_tree_impl(), tree_impl);

        EXPECT_EQ(layer->non_fast_scrollable_region(),
            layer_impl->non_fast_scrollable_region());

        ASSERT_EQ(!!layer->mask_layer(), !!layer_impl->mask_layer());
        if (layer->mask_layer()) {
            SCOPED_TRACE("mask_layer");
            ExpectTreesAreIdentical(
                layer->mask_layer(), layer_impl->mask_layer(), tree_impl);
        }

        ASSERT_EQ(!!layer->replica_layer(), !!layer_impl->replica_layer());
        if (layer->replica_layer()) {
            SCOPED_TRACE("replica_layer");
            ExpectTreesAreIdentical(
                layer->replica_layer(), layer_impl->replica_layer(), tree_impl);
        }

        const LayerList& layer_children = layer->children();
        const OwnedLayerImplList& layer_impl_children = layer_impl->children();

        ASSERT_EQ(layer_children.size(), layer_impl_children.size());

        const std::set<Layer*>* layer_scroll_children = layer->scroll_children();
        const std::set<LayerImpl*>* layer_impl_scroll_children = layer_impl->scroll_children();

        ASSERT_EQ(!!layer_scroll_children, !!layer_impl_scroll_children);

        if (layer_scroll_children) {
            ASSERT_EQ(
                layer_scroll_children->size(),
                layer_impl_scroll_children->size());
        }

        const Layer* layer_scroll_parent = layer->scroll_parent();
        const LayerImpl* layer_impl_scroll_parent = layer_impl->scroll_parent();

        ASSERT_EQ(!!layer_scroll_parent, !!layer_impl_scroll_parent);

        if (layer_scroll_parent) {
            ASSERT_EQ(layer_scroll_parent->id(), layer_impl_scroll_parent->id());
            ASSERT_TRUE(layer_scroll_parent->scroll_children()->find(layer) != layer_scroll_parent->scroll_children()->end());
            ASSERT_TRUE(layer_impl_scroll_parent->scroll_children()->find(layer_impl) != layer_impl_scroll_parent->scroll_children()->end());
        }

        const std::set<Layer*>* layer_clip_children = layer->clip_children();
        const std::set<LayerImpl*>* layer_impl_clip_children = layer_impl->clip_children();

        ASSERT_EQ(!!layer_clip_children, !!layer_impl_clip_children);

        if (layer_clip_children)
            ASSERT_EQ(layer_clip_children->size(), layer_impl_clip_children->size());

        const Layer* layer_clip_parent = layer->clip_parent();
        const LayerImpl* layer_impl_clip_parent = layer_impl->clip_parent();

        ASSERT_EQ(!!layer_clip_parent, !!layer_impl_clip_parent);

        if (layer_clip_parent) {
            const std::set<LayerImpl*>* clip_children_impl = layer_impl_clip_parent->clip_children();
            const std::set<Layer*>* clip_children = layer_clip_parent->clip_children();
            ASSERT_EQ(layer_clip_parent->id(), layer_impl_clip_parent->id());
            ASSERT_TRUE(clip_children->find(layer) != clip_children->end());
            ASSERT_TRUE(clip_children_impl->find(layer_impl) != clip_children_impl->end());
        }

        for (size_t i = 0; i < layer_children.size(); ++i) {
            SCOPED_TRACE(base::StringPrintf("child layer %" PRIuS, i).c_str());
            ExpectTreesAreIdentical(
                layer_children[i].get(), layer_impl_children[i], tree_impl);
        }
    }

    class TreeSynchronizerTest : public testing::Test {
    public:
        TreeSynchronizerTest()
            : client_(FakeLayerTreeHostClient::DIRECT_3D)
            , host_(FakeLayerTreeHost::Create(&client_, &task_graph_runner_))
        {
        }

    protected:
        FakeLayerTreeHostClient client_;
        TestTaskGraphRunner task_graph_runner_;
        scoped_ptr<FakeLayerTreeHost> host_;
        LayerSettings layer_settings_;
    };

    // Attempts to synchronizes a null tree. This should not crash, and should
    // return a null tree.
    TEST_F(TreeSynchronizerTest, SyncNullTree)
    {
        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            static_cast<Layer*>(NULL), nullptr, host_->active_tree());

        EXPECT_TRUE(!layer_impl_tree_root.get());
    }

    // Constructs a very simple tree and synchronizes it without trying to reuse any
    // preexisting layers.
    TEST_F(TreeSynchronizerTest, SyncSimpleTreeFromEmpty)
    {
        scoped_refptr<Layer> layer_tree_root = Layer::Create(layer_settings_);
        layer_tree_root->AddChild(Layer::Create(layer_settings_));
        layer_tree_root->AddChild(Layer::Create(layer_settings_));

        host_->SetRootLayer(layer_tree_root);

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            layer_tree_root.get(), nullptr, host_->active_tree());

        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());
    }

    // Constructs a very simple tree and synchronizes it attempting to reuse some
    // layers
    TEST_F(TreeSynchronizerTest, SyncSimpleTreeReusingLayers)
    {
        std::vector<int> layer_impl_destruction_list;

        scoped_refptr<Layer> layer_tree_root = MockLayer::Create(layer_settings_, &layer_impl_destruction_list);
        layer_tree_root->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));
        layer_tree_root->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));

        host_->SetRootLayer(layer_tree_root);

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            layer_tree_root.get(), nullptr, host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        // We have to push properties to pick up the destruction list pointer.
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());

        // Add a new layer to the Layer side
        layer_tree_root->children()[0]->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));
        // Remove one.
        layer_tree_root->children()[1]->RemoveFromParent();
        int second_layer_impl_id = layer_impl_tree_root->children()[1]->id();

        // Synchronize again. After the sync the trees should be equivalent and we
        // should have created and destroyed one LayerImpl.
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        ASSERT_EQ(1u, layer_impl_destruction_list.size());
        EXPECT_EQ(second_layer_impl_id, layer_impl_destruction_list[0]);
    }

    // Constructs a very simple tree and checks that a stacking-order change is
    // tracked properly.
    TEST_F(TreeSynchronizerTest, SyncSimpleTreeAndTrackStackingOrderChange)
    {
        std::vector<int> layer_impl_destruction_list;

        // Set up the tree and sync once. child2 needs to be synced here, too, even
        // though we remove it to set up the intended scenario.
        scoped_refptr<Layer> layer_tree_root = MockLayer::Create(layer_settings_, &layer_impl_destruction_list);
        scoped_refptr<Layer> child2 = MockLayer::Create(layer_settings_, &layer_impl_destruction_list);
        layer_tree_root->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));
        layer_tree_root->AddChild(child2);

        host_->SetRootLayer(layer_tree_root);

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            layer_tree_root.get(), nullptr, host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        // We have to push properties to pick up the destruction list pointer.
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());

        layer_impl_tree_root->ResetAllChangeTrackingForSubtree();

        // re-insert the layer and sync again.
        child2->RemoveFromParent();
        layer_tree_root->AddChild(child2);
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());

        // Check that the impl thread properly tracked the change.
        EXPECT_FALSE(layer_impl_tree_root->LayerPropertyChanged());
        EXPECT_FALSE(layer_impl_tree_root->children()[0]->LayerPropertyChanged());
        EXPECT_TRUE(layer_impl_tree_root->children()[1]->LayerPropertyChanged());
    }

    TEST_F(TreeSynchronizerTest, SyncSimpleTreeAndProperties)
    {
        scoped_refptr<Layer> layer_tree_root = Layer::Create(layer_settings_);
        layer_tree_root->AddChild(Layer::Create(layer_settings_));
        layer_tree_root->AddChild(Layer::Create(layer_settings_));

        host_->SetRootLayer(layer_tree_root);

        // Pick some random properties to set. The values are not important, we're
        // just testing that at least some properties are making it through.
        gfx::PointF root_position = gfx::PointF(2.3f, 7.4f);
        layer_tree_root->SetPosition(root_position);

        float first_child_opacity = 0.25f;
        layer_tree_root->children()[0]->SetOpacity(first_child_opacity);

        gfx::Size second_child_bounds = gfx::Size(25, 53);
        layer_tree_root->children()[1]->SetBounds(second_child_bounds);
        layer_tree_root->children()[1]->SavePaintProperties();

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            layer_tree_root.get(), nullptr, host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());

        // Check that the property values we set on the Layer tree are reflected in
        // the LayerImpl tree.
        gfx::PointF root_layer_impl_position = layer_impl_tree_root->position();
        EXPECT_EQ(root_position.x(), root_layer_impl_position.x());
        EXPECT_EQ(root_position.y(), root_layer_impl_position.y());

        EXPECT_EQ(first_child_opacity,
            layer_impl_tree_root->children()[0]->opacity());

        gfx::Size second_layer_impl_child_bounds = layer_impl_tree_root->children()[1]->bounds();
        EXPECT_EQ(second_child_bounds.width(),
            second_layer_impl_child_bounds.width());
        EXPECT_EQ(second_child_bounds.height(),
            second_layer_impl_child_bounds.height());
    }

    TEST_F(TreeSynchronizerTest, ReuseLayerImplsAfterStructuralChange)
    {
        std::vector<int> layer_impl_destruction_list;

        // Set up a tree with this sort of structure:
        // root --- A --- B ---+--- C
        //                     |
        //                     +--- D
        scoped_refptr<Layer> layer_tree_root = MockLayer::Create(layer_settings_, &layer_impl_destruction_list);
        layer_tree_root->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));

        scoped_refptr<Layer> layer_a = layer_tree_root->children()[0].get();
        layer_a->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));

        scoped_refptr<Layer> layer_b = layer_a->children()[0].get();
        layer_b->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));

        scoped_refptr<Layer> layer_c = layer_b->children()[0].get();
        layer_b->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));
        scoped_refptr<Layer> layer_d = layer_b->children()[1].get();

        host_->SetRootLayer(layer_tree_root);

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            layer_tree_root.get(), nullptr, host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        // We have to push properties to pick up the destruction list pointer.
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());

        // Now restructure the tree to look like this:
        // root --- D ---+--- A
        //               |
        //               +--- C --- B
        layer_tree_root->RemoveAllChildren();
        layer_d->RemoveAllChildren();
        layer_tree_root->AddChild(layer_d);
        layer_a->RemoveAllChildren();
        layer_d->AddChild(layer_a);
        layer_c->RemoveAllChildren();
        layer_d->AddChild(layer_c);
        layer_b->RemoveAllChildren();
        layer_c->AddChild(layer_b);

        // After another synchronize our trees should match and we should not have
        // destroyed any LayerImpls
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        EXPECT_EQ(0u, layer_impl_destruction_list.size());
    }

    // Constructs a very simple tree, synchronizes it, then synchronizes to a
    // totally new tree. All layers from the old tree should be deleted.
    TEST_F(TreeSynchronizerTest, SyncSimpleTreeThenDestroy)
    {
        std::vector<int> layer_impl_destruction_list;

        scoped_refptr<Layer> old_layer_tree_root = MockLayer::Create(layer_settings_, &layer_impl_destruction_list);
        old_layer_tree_root->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));
        old_layer_tree_root->AddChild(
            MockLayer::Create(layer_settings_, &layer_impl_destruction_list));

        host_->SetRootLayer(old_layer_tree_root);

        int old_tree_root_layer_id = old_layer_tree_root->id();
        int old_tree_first_child_layer_id = old_layer_tree_root->children()[0]->id();
        int old_tree_second_child_layer_id = old_layer_tree_root->children()[1]->id();

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            old_layer_tree_root.get(), nullptr, host_->active_tree());
        ExpectTreesAreIdentical(old_layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        // We have to push properties to pick up the destruction list pointer.
        TreeSynchronizer::PushProperties(old_layer_tree_root.get(),
            layer_impl_tree_root.get());

        // Remove all children on the Layer side.
        old_layer_tree_root->RemoveAllChildren();

        // Synchronize again. After the sync all LayerImpls from the old tree should
        // be deleted.
        scoped_refptr<Layer> new_layer_tree_root = Layer::Create(layer_settings_);
        host_->SetRootLayer(new_layer_tree_root);
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(new_layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_->active_tree());
        ExpectTreesAreIdentical(new_layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        ASSERT_EQ(3u, layer_impl_destruction_list.size());

        EXPECT_TRUE(std::find(layer_impl_destruction_list.begin(),
                        layer_impl_destruction_list.end(),
                        old_tree_root_layer_id)
            != layer_impl_destruction_list.end());
        EXPECT_TRUE(std::find(layer_impl_destruction_list.begin(),
                        layer_impl_destruction_list.end(),
                        old_tree_first_child_layer_id)
            != layer_impl_destruction_list.end());
        EXPECT_TRUE(std::find(layer_impl_destruction_list.begin(),
                        layer_impl_destruction_list.end(),
                        old_tree_second_child_layer_id)
            != layer_impl_destruction_list.end());
    }

    // Constructs+syncs a tree with mask, replica, and replica mask layers.
    TEST_F(TreeSynchronizerTest, SyncMaskReplicaAndReplicaMaskLayers)
    {
        scoped_refptr<Layer> layer_tree_root = Layer::Create(layer_settings_);
        layer_tree_root->AddChild(Layer::Create(layer_settings_));
        layer_tree_root->AddChild(Layer::Create(layer_settings_));
        layer_tree_root->AddChild(Layer::Create(layer_settings_));

        // First child gets a mask layer.
        scoped_refptr<Layer> mask_layer = Layer::Create(layer_settings_);
        layer_tree_root->children()[0]->SetMaskLayer(mask_layer.get());

        // Second child gets a replica layer.
        scoped_refptr<Layer> replica_layer = Layer::Create(layer_settings_);
        layer_tree_root->children()[1]->SetReplicaLayer(replica_layer.get());

        // Third child gets a replica layer with a mask layer.
        scoped_refptr<Layer> replica_layer_with_mask = Layer::Create(layer_settings_);
        scoped_refptr<Layer> replica_mask_layer = Layer::Create(layer_settings_);
        replica_layer_with_mask->SetMaskLayer(replica_mask_layer.get());
        layer_tree_root->children()[2]->SetReplicaLayer(replica_layer_with_mask.get());

        host_->SetRootLayer(layer_tree_root);

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            layer_tree_root.get(), nullptr, host_->active_tree());

        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        // Remove the mask layer.
        layer_tree_root->children()[0]->SetMaskLayer(NULL);
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        // Remove the replica layer.
        layer_tree_root->children()[1]->SetReplicaLayer(NULL);
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());

        // Remove the replica mask.
        replica_layer_with_mask->SetMaskLayer(NULL);
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_->active_tree());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_->active_tree());
    }

    TEST_F(TreeSynchronizerTest, SynchronizeAnimations)
    {
        LayerTreeSettings settings;
        FakeProxy proxy;
        DebugScopedSetImplThread impl(&proxy);
        FakeRenderingStatsInstrumentation stats_instrumentation;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        scoped_ptr<LayerTreeHostImpl> host_impl = LayerTreeHostImpl::Create(
            settings, nullptr, &proxy, &stats_instrumentation, &shared_bitmap_manager,
            nullptr, &task_graph_runner, 0);

        scoped_refptr<Layer> layer_tree_root = Layer::Create(layer_settings_);
        host_->SetRootLayer(layer_tree_root);

        layer_tree_root->SetLayerAnimationControllerForTest(
            FakeLayerAnimationController::Create());

        EXPECT_FALSE(static_cast<FakeLayerAnimationController*>(
            layer_tree_root->layer_animation_controller())
                         ->SynchronizedAnimations());

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            layer_tree_root.get(), nullptr, host_->active_tree());
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_->active_tree());

        EXPECT_TRUE(static_cast<FakeLayerAnimationController*>(
            layer_tree_root->layer_animation_controller())
                        ->SynchronizedAnimations());
    }

    TEST_F(TreeSynchronizerTest, SynchronizeScrollParent)
    {
        LayerTreeSettings settings;
        FakeProxy proxy;
        DebugScopedSetImplThread impl(&proxy);
        FakeRenderingStatsInstrumentation stats_instrumentation;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        scoped_ptr<LayerTreeHostImpl> host_impl = LayerTreeHostImpl::Create(
            settings, nullptr, &proxy, &stats_instrumentation, &shared_bitmap_manager,
            nullptr, &task_graph_runner, 0);

        scoped_refptr<Layer> layer_tree_root = Layer::Create(layer_settings_);
        scoped_refptr<Layer> scroll_parent = Layer::Create(layer_settings_);
        layer_tree_root->AddChild(scroll_parent);
        layer_tree_root->AddChild(Layer::Create(layer_settings_));
        layer_tree_root->AddChild(Layer::Create(layer_settings_));

        host_->SetRootLayer(layer_tree_root);

        // First child is the second and third child's scroll parent.
        layer_tree_root->children()[1]->SetScrollParent(scroll_parent.get());
        layer_tree_root->children()[2]->SetScrollParent(scroll_parent.get());

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            layer_tree_root.get(), nullptr, host_impl->active_tree());
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());
        {
            SCOPED_TRACE("case one");
            ExpectTreesAreIdentical(layer_tree_root.get(),
                layer_impl_tree_root.get(),
                host_impl->active_tree());
        }

        // Remove the first scroll child.
        layer_tree_root->children()[1]->RemoveFromParent();
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_impl->active_tree());
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());
        {
            SCOPED_TRACE("case two");
            ExpectTreesAreIdentical(layer_tree_root.get(),
                layer_impl_tree_root.get(),
                host_impl->active_tree());
        }

        // Add an additional scroll layer.
        scoped_refptr<Layer> additional_scroll_child = Layer::Create(layer_settings_);
        layer_tree_root->AddChild(additional_scroll_child);
        additional_scroll_child->SetScrollParent(scroll_parent.get());
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_impl->active_tree());
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());
        {
            SCOPED_TRACE("case three");
            ExpectTreesAreIdentical(layer_tree_root.get(),
                layer_impl_tree_root.get(),
                host_impl->active_tree());
        }
    }

    TEST_F(TreeSynchronizerTest, SynchronizeClipParent)
    {
        LayerTreeSettings settings;
        FakeProxy proxy;
        DebugScopedSetImplThread impl(&proxy);
        FakeRenderingStatsInstrumentation stats_instrumentation;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        scoped_ptr<LayerTreeHostImpl> host_impl = LayerTreeHostImpl::Create(
            settings, nullptr, &proxy, &stats_instrumentation, &shared_bitmap_manager,
            nullptr, &task_graph_runner, 0);

        scoped_refptr<Layer> layer_tree_root = Layer::Create(layer_settings_);
        scoped_refptr<Layer> clip_parent = Layer::Create(layer_settings_);
        scoped_refptr<Layer> intervening = Layer::Create(layer_settings_);
        scoped_refptr<Layer> clip_child1 = Layer::Create(layer_settings_);
        scoped_refptr<Layer> clip_child2 = Layer::Create(layer_settings_);
        layer_tree_root->AddChild(clip_parent);
        clip_parent->AddChild(intervening);
        intervening->AddChild(clip_child1);
        intervening->AddChild(clip_child2);

        host_->SetRootLayer(layer_tree_root);

        // First child is the second and third child's scroll parent.
        clip_child1->SetClipParent(clip_parent.get());
        clip_child2->SetClipParent(clip_parent.get());

        scoped_ptr<LayerImpl> layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(
            layer_tree_root.get(), nullptr, host_impl->active_tree());
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_impl->active_tree());

        // Remove the first clip child.
        clip_child1->RemoveFromParent();
        clip_child1 = NULL;

        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_impl->active_tree());
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_impl->active_tree());

        // Add an additional clip child.
        scoped_refptr<Layer> additional_clip_child = Layer::Create(layer_settings_);
        intervening->AddChild(additional_clip_child);
        additional_clip_child->SetClipParent(clip_parent.get());
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_impl->active_tree());
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_impl->active_tree());

        // Remove the nearest clipping ancestor.
        clip_parent->RemoveFromParent();
        clip_parent = NULL;
        layer_impl_tree_root = TreeSynchronizer::SynchronizeTrees(layer_tree_root.get(),
            layer_impl_tree_root.Pass(),
            host_impl->active_tree());
        TreeSynchronizer::PushProperties(layer_tree_root.get(),
            layer_impl_tree_root.get());
        ExpectTreesAreIdentical(layer_tree_root.get(),
            layer_impl_tree_root.get(),
            host_impl->active_tree());

        // The clip children should have been unhooked.
        EXPECT_EQ(2u, intervening->children().size());
        EXPECT_FALSE(clip_child2->clip_parent());
        EXPECT_FALSE(additional_clip_child->clip_parent());
    }

} // namespace
} // namespace cc
