// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/picture_layer.h"

#include "base/thread_task_runner_handle.h"
#include "cc/layers/content_layer_client.h"
#include "cc/layers/picture_layer_impl.h"
#include "cc/playback/display_item_list_settings.h"
#include "cc/test/fake_display_list_recording_source.h"
#include "cc/test/fake_layer_tree_host.h"
#include "cc/test/fake_picture_layer.h"
#include "cc/test/fake_picture_layer_impl.h"
#include "cc/test/fake_proxy.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "cc/trees/single_thread_proxy.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class MockContentLayerClient : public ContentLayerClient {
    public:
        void PaintContents(SkCanvas* canvas,
            const gfx::Rect& clip,
            PaintingControlSetting picture_control) override { }
        scoped_refptr<DisplayItemList> PaintContentsToDisplayList(
            const gfx::Rect& clip,
            PaintingControlSetting picture_control) override
        {
            return DisplayItemList::Create(clip, DisplayItemListSettings());
        }
        bool FillsBoundsCompletely() const override { return false; };
        size_t GetApproximateUnsharedMemoryUsage() const override { return 0; }
    };

    TEST(PictureLayerTest, NoTilesIfEmptyBounds)
    {
        MockContentLayerClient client;
        scoped_refptr<PictureLayer> layer = PictureLayer::Create(LayerSettings(), &client);
        layer->SetBounds(gfx::Size(10, 10));

        FakeLayerTreeHostClient host_client(FakeLayerTreeHostClient::DIRECT_3D);
        TestTaskGraphRunner task_graph_runner;
        scoped_ptr<FakeLayerTreeHost> host = FakeLayerTreeHost::Create(&host_client, &task_graph_runner);
        host->SetRootLayer(layer);
        layer->SetIsDrawable(true);
        layer->SavePaintProperties();
        layer->Update();

        EXPECT_EQ(0, host->source_frame_number());
        host->CommitComplete();
        EXPECT_EQ(1, host->source_frame_number());

        layer->SetBounds(gfx::Size(0, 0));
        layer->SavePaintProperties();
        // Intentionally skipping Update since it would normally be skipped on
        // a layer with empty bounds.

        FakeProxy proxy;
        {
            DebugScopedSetImplThread impl_thread(&proxy);

            TestSharedBitmapManager shared_bitmap_manager;
            FakeLayerTreeHostImpl host_impl(LayerTreeSettings(), &proxy,
                &shared_bitmap_manager, &task_graph_runner);
            host_impl.CreatePendingTree();
            scoped_ptr<FakePictureLayerImpl> layer_impl = FakePictureLayerImpl::Create(host_impl.pending_tree(), 1);

            layer->PushPropertiesTo(layer_impl.get());
            EXPECT_FALSE(layer_impl->CanHaveTilings());
            EXPECT_TRUE(layer_impl->bounds() == gfx::Size(0, 0));
            EXPECT_EQ(gfx::Size(), layer_impl->raster_source()->GetSize());
            EXPECT_FALSE(layer_impl->raster_source()->HasRecordings());
        }
    }

    TEST(PictureLayerTest, SuitableForGpuRasterization)
    {
        scoped_ptr<FakeDisplayListRecordingSource> recording_source_owned(
            new FakeDisplayListRecordingSource);
        FakeDisplayListRecordingSource* recording_source = recording_source_owned.get();

        MockContentLayerClient client;
        scoped_refptr<FakePictureLayer> layer = FakePictureLayer::CreateWithRecordingSource(
            LayerSettings(), &client, recording_source_owned.Pass());

        FakeLayerTreeHostClient host_client(FakeLayerTreeHostClient::DIRECT_3D);
        TestTaskGraphRunner task_graph_runner;
        scoped_ptr<FakeLayerTreeHost> host = FakeLayerTreeHost::Create(&host_client, &task_graph_runner);
        host->SetRootLayer(layer);

        // Update layers to initialize the recording source.
        gfx::Size layer_bounds(200, 200);
        gfx::Rect layer_rect(layer_bounds);
        Region invalidation(layer_rect);
        recording_source->UpdateAndExpandInvalidation(
            &client, &invalidation, layer_bounds, layer_rect, 1,
            RecordingSource::RECORD_NORMALLY);

        // Layer is suitable for gpu rasterization by default.
        EXPECT_TRUE(recording_source->IsSuitableForGpuRasterization());
        EXPECT_TRUE(layer->IsSuitableForGpuRasterization());

        // Veto gpu rasterization.
        recording_source->SetUnsuitableForGpuRasterization();
        EXPECT_FALSE(recording_source->IsSuitableForGpuRasterization());
        EXPECT_FALSE(layer->IsSuitableForGpuRasterization());
    }

    // PicturePile uses the source frame number as a unit for measuring invalidation
    // frequency. When a pile moves between compositors, the frame number increases
    // non-monotonically. This executes that code path under this scenario allowing
    // for the code to verify correctness with DCHECKs.
    TEST(PictureLayerTest, NonMonotonicSourceFrameNumber)
    {
        LayerTreeSettings settings;
        settings.single_thread_proxy_scheduler = false;
        settings.use_zero_copy = true;

        FakeLayerTreeHostClient host_client1(FakeLayerTreeHostClient::DIRECT_3D);
        FakeLayerTreeHostClient host_client2(FakeLayerTreeHostClient::DIRECT_3D);
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;

        MockContentLayerClient client;
        scoped_refptr<FakePictureLayer> layer = FakePictureLayer::Create(LayerSettings(), &client);

        LayerTreeHost::InitParams params;
        params.client = &host_client1;
        params.shared_bitmap_manager = &shared_bitmap_manager;
        params.settings = &settings;
        params.task_graph_runner = &task_graph_runner;
        params.main_task_runner = base::ThreadTaskRunnerHandle::Get();
        scoped_ptr<LayerTreeHost> host1 = LayerTreeHost::CreateSingleThreaded(&host_client1, &params);
        host_client1.SetLayerTreeHost(host1.get());

        params.client = &host_client2;
        scoped_ptr<LayerTreeHost> host2 = LayerTreeHost::CreateSingleThreaded(&host_client2, &params);
        host_client2.SetLayerTreeHost(host2.get());

        // The PictureLayer is put in one LayerTreeHost.
        host1->SetRootLayer(layer);
        // Do a main frame, record the picture layers.
        EXPECT_EQ(0, layer->update_count());
        layer->SetNeedsDisplay();
        host1->Composite(base::TimeTicks::Now());
        EXPECT_EQ(1, layer->update_count());
        EXPECT_EQ(1, host1->source_frame_number());

        // The source frame number in |host1| is now higher than host2.
        layer->SetNeedsDisplay();
        host1->Composite(base::TimeTicks::Now());
        EXPECT_EQ(2, layer->update_count());
        EXPECT_EQ(2, host1->source_frame_number());

        // Then moved to another LayerTreeHost.
        host1->SetRootLayer(nullptr);
        host2->SetRootLayer(layer);

        // Do a main frame, record the picture layers. The frame number has changed
        // non-monotonically.
        layer->SetNeedsDisplay();
        host2->Composite(base::TimeTicks::Now());
        EXPECT_EQ(3, layer->update_count());
        EXPECT_EQ(1, host2->source_frame_number());
    }

} // namespace
} // namespace cc
