// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/layer.h"

#include "base/thread_task_runner_handle.h"
#include "cc/debug/lap_timer.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/fake_layer_tree_host_client.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/perf/perf_test.h"

namespace cc {
namespace {

    static const int kTimeLimitMillis = 3000;
    static const int kWarmupRuns = 5;
    static const int kTimeCheckInterval = 10;

    class LayerPerfTest : public testing::Test {
    public:
        LayerPerfTest()
            : host_impl_(&proxy_, &shared_bitmap_manager_, &task_graph_runner_)
            , fake_client_(FakeLayerTreeHostClient::DIRECT_3D)
            , timer_(kWarmupRuns,
                  base::TimeDelta::FromMilliseconds(kTimeLimitMillis),
                  kTimeCheckInterval)
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
            layer_tree_host_->SetRootLayer(nullptr);
            layer_tree_host_ = nullptr;
        }

        FakeImplProxy proxy_;
        TestSharedBitmapManager shared_bitmap_manager_;
        TestTaskGraphRunner task_graph_runner_;
        FakeLayerTreeHostImpl host_impl_;

        FakeLayerTreeHostClient fake_client_;
        scoped_ptr<FakeLayerTreeHost> layer_tree_host_;
        LapTimer timer_;
    };

    TEST_F(LayerPerfTest, PushPropertiesTo)
    {
        scoped_refptr<Layer> test_layer = Layer::Create(LayerSettings());
        scoped_ptr<LayerImpl> impl_layer = LayerImpl::Create(host_impl_.active_tree(), 1);

        layer_tree_host_->SetRootLayer(test_layer);

        float transform_origin_z = 0;
        bool scrollable = true;
        bool contents_opaque = true;
        bool double_sided = true;
        bool hide_layer_and_subtree = true;
        bool masks_to_bounds = true;

        // Properties changed.
        timer_.Reset();
        do {
            test_layer->SetNeedsDisplayRect(gfx::Rect(5, 5));
            test_layer->SetTransformOrigin(gfx::Point3F(0.f, 0.f, transform_origin_z));
            test_layer->SetContentsOpaque(contents_opaque);
            test_layer->SetDoubleSided(double_sided);
            test_layer->SetHideLayerAndSubtree(hide_layer_and_subtree);
            test_layer->SetMasksToBounds(masks_to_bounds);
            test_layer->SetScrollClipLayerId(scrollable ? test_layer->id()
                                                        : Layer::INVALID_ID);
            test_layer->PushPropertiesTo(impl_layer.get());

            transform_origin_z += 0.01f;
            scrollable = !scrollable;
            contents_opaque = !contents_opaque;
            double_sided = !double_sided;
            hide_layer_and_subtree = !hide_layer_and_subtree;
            masks_to_bounds = !masks_to_bounds;

            timer_.NextLap();
        } while (!timer_.HasTimeLimitExpired());

        perf_test::PrintResult("push_properties_to",
            "",
            "props_changed",
            timer_.LapsPerSecond(),
            "runs/s",
            true);

        // Properties didn't change.
        timer_.Reset();
        do {
            test_layer->PushPropertiesTo(impl_layer.get());
            timer_.NextLap();
        } while (!timer_.HasTimeLimitExpired());

        perf_test::PrintResult("push_properties_to",
            "",
            "props_didnt_change",
            timer_.LapsPerSecond(),
            "runs/s",
            true);
    }

} // namespace
} // namespace cc
