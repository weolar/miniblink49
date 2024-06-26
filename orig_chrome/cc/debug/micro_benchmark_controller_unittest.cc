// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "cc/debug/micro_benchmark.h"
#include "cc/debug/micro_benchmark_controller.h"
#include "cc/layers/layer.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/fake_proxy.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class MicroBenchmarkControllerTest : public testing::Test {
    public:
        MicroBenchmarkControllerTest()
            : layer_tree_host_client_(FakeLayerTreeHostClient::DIRECT_3D)
        {
        }

        void SetUp() override
        {
            impl_proxy_ = make_scoped_ptr(new FakeImplProxy);
            layer_tree_host_impl_ = make_scoped_ptr(new FakeLayerTreeHostImpl(
                impl_proxy_.get(), &shared_bitmap_manager_, &task_graph_runner_));

            layer_tree_host_ = FakeLayerTreeHost::Create(&layer_tree_host_client_,
                &task_graph_runner_);
            layer_tree_host_->SetRootLayer(Layer::Create(LayerSettings()));
            layer_tree_host_->InitializeForTesting(scoped_ptr<Proxy>(new FakeProxy));
        }

        void TearDown() override
        {
            layer_tree_host_impl_ = nullptr;
            layer_tree_host_ = nullptr;
            impl_proxy_ = nullptr;
        }

        FakeLayerTreeHostClient layer_tree_host_client_;
        TestTaskGraphRunner task_graph_runner_;
        TestSharedBitmapManager shared_bitmap_manager_;
        scoped_ptr<FakeLayerTreeHost> layer_tree_host_;
        scoped_ptr<FakeLayerTreeHostImpl> layer_tree_host_impl_;
        scoped_ptr<FakeImplProxy> impl_proxy_;
    };

    void Noop(scoped_ptr<base::Value> value)
    {
    }

    void IncrementCallCount(int* count, scoped_ptr<base::Value> value)
    {
        ++(*count);
    }

    TEST_F(MicroBenchmarkControllerTest, ScheduleFail)
    {
        int id = layer_tree_host_->ScheduleMicroBenchmark(
            "non_existant_benchmark", nullptr, base::Bind(&Noop));
        EXPECT_EQ(id, 0);
    }

    TEST_F(MicroBenchmarkControllerTest, CommitScheduled)
    {
        EXPECT_FALSE(layer_tree_host_->needs_commit());
        int id = layer_tree_host_->ScheduleMicroBenchmark(
            "unittest_only_benchmark", nullptr, base::Bind(&Noop));
        EXPECT_GT(id, 0);
        EXPECT_TRUE(layer_tree_host_->needs_commit());
    }

    TEST_F(MicroBenchmarkControllerTest, BenchmarkRan)
    {
        int run_count = 0;
        int id = layer_tree_host_->ScheduleMicroBenchmark(
            "unittest_only_benchmark",
            nullptr,
            base::Bind(&IncrementCallCount, base::Unretained(&run_count)));
        EXPECT_GT(id, 0);

        layer_tree_host_->SetOutputSurfaceLostForTesting(false);
        layer_tree_host_->UpdateLayers();

        EXPECT_EQ(1, run_count);
    }

    TEST_F(MicroBenchmarkControllerTest, MultipleBenchmarkRan)
    {
        int run_count = 0;
        int id = layer_tree_host_->ScheduleMicroBenchmark(
            "unittest_only_benchmark",
            nullptr,
            base::Bind(&IncrementCallCount, base::Unretained(&run_count)));
        EXPECT_GT(id, 0);
        id = layer_tree_host_->ScheduleMicroBenchmark(
            "unittest_only_benchmark",
            nullptr,
            base::Bind(&IncrementCallCount, base::Unretained(&run_count)));
        EXPECT_GT(id, 0);

        layer_tree_host_->SetOutputSurfaceLostForTesting(false);
        layer_tree_host_->UpdateLayers();

        EXPECT_EQ(2, run_count);

        id = layer_tree_host_->ScheduleMicroBenchmark(
            "unittest_only_benchmark",
            nullptr,
            base::Bind(&IncrementCallCount, base::Unretained(&run_count)));
        EXPECT_GT(id, 0);
        id = layer_tree_host_->ScheduleMicroBenchmark(
            "unittest_only_benchmark",
            nullptr,
            base::Bind(&IncrementCallCount, base::Unretained(&run_count)));
        EXPECT_GT(id, 0);

        layer_tree_host_->UpdateLayers();
        EXPECT_EQ(4, run_count);

        layer_tree_host_->UpdateLayers();
        EXPECT_EQ(4, run_count);
    }

    TEST_F(MicroBenchmarkControllerTest, BenchmarkImplRan)
    {
        int run_count = 0;
        scoped_ptr<base::DictionaryValue> settings(new base::DictionaryValue);
        settings->SetBoolean("run_benchmark_impl", true);

        // Schedule a main thread benchmark.
        int id = layer_tree_host_->ScheduleMicroBenchmark(
            "unittest_only_benchmark",
            settings.Pass(),
            base::Bind(&IncrementCallCount, base::Unretained(&run_count)));
        EXPECT_GT(id, 0);

        // Schedule impl benchmarks. In production code, this is run in commit.
        layer_tree_host_->GetMicroBenchmarkController()->ScheduleImplBenchmarks(
            layer_tree_host_impl_.get());

        // Now complete the commit (as if on the impl thread).
        layer_tree_host_impl_->CommitComplete();

        // Make sure all posted messages run.
        base::MessageLoop::current()->RunUntilIdle();

        EXPECT_EQ(1, run_count);
    }

    TEST_F(MicroBenchmarkControllerTest, SendMessage)
    {
        // Send valid message to invalid benchmark (id = 0)
        scoped_ptr<base::DictionaryValue> message(new base::DictionaryValue);
        message->SetBoolean("can_handle", true);
        bool message_handled = layer_tree_host_->SendMessageToMicroBenchmark(0, message.Pass());
        EXPECT_FALSE(message_handled);

        // Schedule a benchmark
        int run_count = 0;
        int id = layer_tree_host_->ScheduleMicroBenchmark(
            "unittest_only_benchmark",
            nullptr,
            base::Bind(&IncrementCallCount, base::Unretained(&run_count)));
        EXPECT_GT(id, 0);

        // Send valid message to valid benchmark
        message = make_scoped_ptr(new base::DictionaryValue);
        message->SetBoolean("can_handle", true);
        message_handled = layer_tree_host_->SendMessageToMicroBenchmark(id, message.Pass());
        EXPECT_TRUE(message_handled);

        // Send invalid message to valid benchmark
        message = make_scoped_ptr(new base::DictionaryValue);
        message->SetBoolean("can_handle", false);
        message_handled = layer_tree_host_->SendMessageToMicroBenchmark(id, message.Pass());
        EXPECT_FALSE(message_handled);
    }

} // namespace
} // namespace cc
