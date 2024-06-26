// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/ui_resource_layer.h"

#include "base/thread_task_runner_handle.h"
#include "cc/resources/resource_provider.h"
#include "cc/resources/scoped_ui_resource.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/fake_layer_tree_host_client.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/geometry_test_utils.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/single_thread_proxy.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkBitmap.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::AtLeast;
using ::testing::Mock;

namespace cc {
namespace {

    class TestUIResourceLayer : public UIResourceLayer {
    public:
        static scoped_refptr<TestUIResourceLayer> Create(
            const LayerSettings& settings)
        {
            return make_scoped_refptr(new TestUIResourceLayer(settings));
        }

        UIResourceId GetUIResourceId()
        {
            if (ui_resource_holder_)
                return ui_resource_holder_->id();
            return 0;
        }

    protected:
        explicit TestUIResourceLayer(const LayerSettings& settings)
            : UIResourceLayer(settings)
        {
            SetIsDrawable(true);
        }
        ~TestUIResourceLayer() override { }
    };

    class UIResourceLayerTest : public testing::Test {
    public:
        UIResourceLayerTest()
            : fake_client_(FakeLayerTreeHostClient::DIRECT_3D)
        {
        }

    protected:
        void SetUp() override
        {
            layer_tree_host_ = FakeLayerTreeHost::Create(&fake_client_, &task_graph_runner_);
            layer_tree_host_->InitializeSingleThreaded(
                &fake_client_, base::ThreadTaskRunnerHandle::Get(), nullptr);
        }

        void TearDown() override
        {
            Mock::VerifyAndClearExpectations(layer_tree_host_.get());
        }

        FakeLayerTreeHostClient fake_client_;
        TestTaskGraphRunner task_graph_runner_;
        scoped_ptr<FakeLayerTreeHost> layer_tree_host_;
        LayerSettings layer_settings_;
    };

    TEST_F(UIResourceLayerTest, SetBitmap)
    {
        scoped_refptr<UIResourceLayer> test_layer = TestUIResourceLayer::Create(layer_settings_);
        ASSERT_TRUE(test_layer.get());
        test_layer->SetBounds(gfx::Size(100, 100));

        layer_tree_host_->SetRootLayer(test_layer);
        Mock::VerifyAndClearExpectations(layer_tree_host_.get());
        EXPECT_EQ(test_layer->layer_tree_host(), layer_tree_host_.get());

        test_layer->SavePaintProperties();
        test_layer->Update();

        EXPECT_FALSE(test_layer->DrawsContent());

        SkBitmap bitmap;
        bitmap.allocN32Pixels(10, 10);
        bitmap.setImmutable();

        test_layer->SetBitmap(bitmap);
        test_layer->Update();

        EXPECT_TRUE(test_layer->DrawsContent());
    }

    TEST_F(UIResourceLayerTest, SetUIResourceId)
    {
        scoped_refptr<TestUIResourceLayer> test_layer = TestUIResourceLayer::Create(layer_settings_);
        ASSERT_TRUE(test_layer.get());
        test_layer->SetBounds(gfx::Size(100, 100));

        layer_tree_host_->SetRootLayer(test_layer);
        Mock::VerifyAndClearExpectations(layer_tree_host_.get());
        EXPECT_EQ(test_layer->layer_tree_host(), layer_tree_host_.get());

        test_layer->SavePaintProperties();
        test_layer->Update();

        EXPECT_FALSE(test_layer->DrawsContent());

        bool is_opaque = false;
        scoped_ptr<ScopedUIResource> resource = ScopedUIResource::Create(
            layer_tree_host_.get(), UIResourceBitmap(gfx::Size(10, 10), is_opaque));
        test_layer->SetUIResourceId(resource->id());
        test_layer->Update();

        EXPECT_TRUE(test_layer->DrawsContent());

        // ID is preserved even when you set ID first and attach it to the tree.
        layer_tree_host_->SetRootLayer(nullptr);
        scoped_ptr<ScopedUIResource> shared_resource = ScopedUIResource::Create(
            layer_tree_host_.get(), UIResourceBitmap(gfx::Size(5, 5), is_opaque));
        test_layer->SetUIResourceId(shared_resource->id());
        layer_tree_host_->SetRootLayer(test_layer);
        EXPECT_EQ(shared_resource->id(), test_layer->GetUIResourceId());
        EXPECT_TRUE(test_layer->DrawsContent());
    }

    TEST_F(UIResourceLayerTest, BitmapClearedOnSetUIResourceId)
    {
        scoped_refptr<UIResourceLayer> test_layer = TestUIResourceLayer::Create(layer_settings_);
        ASSERT_TRUE(test_layer.get());
        test_layer->SetBounds(gfx::Size(100, 100));

        SkBitmap bitmap;
        bitmap.allocN32Pixels(10, 10);
        bitmap.setImmutable();
        ASSERT_FALSE(bitmap.isNull());
        ASSERT_TRUE(bitmap.pixelRef()->unique());

        test_layer->SetBitmap(bitmap);
        ASSERT_FALSE(bitmap.pixelRef()->unique());

        test_layer->SetUIResourceId(0);
        EXPECT_TRUE(bitmap.pixelRef()->unique());
    }

} // namespace
} // namespace cc
