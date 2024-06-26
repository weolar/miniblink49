// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/delegated_renderer_layer.h"

#include "cc/layers/delegated_frame_provider.h"
#include "cc/layers/delegated_frame_resource_collection.h"
#include "cc/layers/solid_color_layer.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/test/fake_delegated_renderer_layer.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/fake_proxy.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class DelegatedRendererLayerTest : public testing::Test {
    public:
        DelegatedRendererLayerTest()
            : proxy_()
            , host_client_(FakeLayerTreeHostClient::DIRECT_3D)
        {
            LayerTreeSettings settings;
            settings.minimum_occlusion_tracking_size = gfx::Size();

            host_impl_ = FakeLayerTreeHost::Create(&host_client_, &task_graph_runner_, settings);
            host_impl_->SetViewportSize(gfx::Size(10, 10));
        }

    protected:
        FakeProxy proxy_;
        FakeLayerTreeHostClient host_client_;
        TestTaskGraphRunner task_graph_runner_;
        TestSharedBitmapManager shared_bitmap_manager_;
        scoped_ptr<LayerTreeHost> host_impl_;
    };

    class DelegatedRendererLayerTestSimple : public DelegatedRendererLayerTest {
    public:
        DelegatedRendererLayerTestSimple()
            : DelegatedRendererLayerTest()
        {
            scoped_ptr<RenderPass> root_pass(RenderPass::Create());
            root_pass->SetNew(
                RenderPassId(1, 1), gfx::Rect(1, 1), gfx::Rect(1, 1), gfx::Transform());
            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            frame_data->render_pass_list.push_back(root_pass.Pass());
            resources_ = new DelegatedFrameResourceCollection;
            provider_ = new DelegatedFrameProvider(resources_, frame_data.Pass());
            LayerSettings layer_settings;
            root_layer_ = SolidColorLayer::Create(layer_settings);
            layer_before_ = SolidColorLayer::Create(layer_settings);
            delegated_renderer_layer_ = FakeDelegatedRendererLayer::Create(layer_settings, provider_.get());
        }

    protected:
        scoped_refptr<Layer> root_layer_;
        scoped_refptr<Layer> layer_before_;
        scoped_refptr<DelegatedRendererLayer> delegated_renderer_layer_;
        scoped_refptr<DelegatedFrameResourceCollection> resources_;
        scoped_refptr<DelegatedFrameProvider> provider_;
    };

    TEST_F(DelegatedRendererLayerTestSimple, DelegatedManyDescendants)
    {
        EXPECT_EQ(0, root_layer_->NumDescendantsThatDrawContent());
        root_layer_->AddChild(layer_before_);
        EXPECT_EQ(0, root_layer_->NumDescendantsThatDrawContent());
        layer_before_->SetIsDrawable(true);
        EXPECT_EQ(1, root_layer_->NumDescendantsThatDrawContent());
        EXPECT_EQ(0, layer_before_->NumDescendantsThatDrawContent());
        layer_before_->AddChild(delegated_renderer_layer_);
        EXPECT_EQ(0, layer_before_->NumDescendantsThatDrawContent());
        EXPECT_EQ(0, delegated_renderer_layer_->NumDescendantsThatDrawContent());
        EXPECT_EQ(1, root_layer_->NumDescendantsThatDrawContent());
        delegated_renderer_layer_->SetIsDrawable(true);
        EXPECT_EQ(1000, delegated_renderer_layer_->NumDescendantsThatDrawContent());
        EXPECT_EQ(1001, layer_before_->NumDescendantsThatDrawContent());
        EXPECT_EQ(1002, root_layer_->NumDescendantsThatDrawContent());
    }

} // namespace
} // namespace cc
