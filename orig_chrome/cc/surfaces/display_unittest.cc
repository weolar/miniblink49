// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/test/null_task_runner.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/copy_output_result.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/quads/render_pass.h"
#include "cc/resources/shared_bitmap_manager.h"
#include "cc/surfaces/display.h"
#include "cc/surfaces/display_client.h"
#include "cc/surfaces/surface.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_factory_client.h"
#include "cc/surfaces/surface_id_allocator.h"
#include "cc/surfaces/surface_manager.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/scheduler_test_common.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

using testing::AnyNumber;

namespace cc {
namespace {

    class EmptySurfaceFactoryClient : public SurfaceFactoryClient {
    public:
        void ReturnResources(const ReturnedResourceArray& resources) override { }
    };

    class TestSoftwareOutputDevice : public SoftwareOutputDevice {
    public:
        TestSoftwareOutputDevice() { }

        gfx::Rect damage_rect() const { return damage_rect_; }
        gfx::Size viewport_pixel_size() const { return viewport_pixel_size_; }
    };

    class DisplayTest : public testing::Test {
    public:
        DisplayTest()
            : factory_(&manager_, &empty_client_)
            , software_output_device_(nullptr)
            , task_runner_(new base::NullTaskRunner)
        {
        }

    protected:
        void SetUpContext(scoped_ptr<TestWebGraphicsContext3D> context)
        {
            if (context) {
                output_surface_ = FakeOutputSurface::Create3d(
                    TestContextProvider::Create(context.Pass()));
            } else {
                scoped_ptr<TestSoftwareOutputDevice> output_device(
                    new TestSoftwareOutputDevice);
                software_output_device_ = output_device.get();
                output_surface_ = FakeOutputSurface::CreateSoftware(output_device.Pass());
            }
            shared_bitmap_manager_.reset(new TestSharedBitmapManager);
            output_surface_ptr_ = output_surface_.get();
        }

        void SubmitCompositorFrame(RenderPassList* pass_list, SurfaceId surface_id)
        {
            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            pass_list->swap(frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> frame(new CompositorFrame);
            frame->delegated_frame_data = frame_data.Pass();

            factory_.SubmitCompositorFrame(surface_id, frame.Pass(),
                SurfaceFactory::DrawCallback());
        }

        SurfaceManager manager_;
        EmptySurfaceFactoryClient empty_client_;
        SurfaceFactory factory_;
        TestSoftwareOutputDevice* software_output_device_;
        scoped_ptr<FakeOutputSurface> output_surface_;
        FakeOutputSurface* output_surface_ptr_;
        FakeBeginFrameSource fake_begin_frame_source_;
        scoped_refptr<base::NullTaskRunner> task_runner_;
        scoped_ptr<SharedBitmapManager> shared_bitmap_manager_;
    };

    class TestDisplayClient : public DisplayClient {
    public:
        TestDisplayClient() { }
        ~TestDisplayClient() override { }

        void CommitVSyncParameters(base::TimeTicks timebase,
            base::TimeDelta interval) override { }
        void OutputSurfaceLost() override { }
        void SetMemoryPolicy(const ManagedMemoryPolicy& policy) override { }
    };

    class TestDisplayScheduler : public DisplayScheduler {
    public:
        TestDisplayScheduler(DisplaySchedulerClient* client,
            BeginFrameSource* begin_frame_source,
            base::NullTaskRunner* task_runner)
            : DisplayScheduler(client, begin_frame_source, task_runner, 1)
            , damaged(false)
            , display_resized_(false)
            , has_new_root_surface(false)
            , swapped(false)
        {
        }

        ~TestDisplayScheduler() override { }

        void DisplayResized() override { display_resized_ = true; }

        void SetNewRootSurface(SurfaceId root_surface_id) override
        {
            has_new_root_surface = true;
        }

        void SurfaceDamaged(SurfaceId surface_id) override
        {
            damaged = true;
            needs_draw_ = true;
        }

        void DidSwapBuffers() override { swapped = true; }

        void ResetDamageForTest()
        {
            damaged = false;
            display_resized_ = false;
            has_new_root_surface = false;
        }

        bool damaged;
        bool display_resized_;
        bool has_new_root_surface;
        bool swapped;
    };

    void CopyCallback(bool* called, scoped_ptr<CopyOutputResult> result)
    {
        *called = true;
    }

    // Check that frame is damaged and swapped only under correct conditions.
    TEST_F(DisplayTest, DisplayDamaged)
    {
        SetUpContext(nullptr);
        TestDisplayClient client;
        RendererSettings settings;
        settings.partial_swap_enabled = true;
        settings.finish_rendering_on_resize = true;
        Display display(&client, &manager_, shared_bitmap_manager_.get(), nullptr,
            settings);

        TestDisplayScheduler scheduler(&display, &fake_begin_frame_source_,
            task_runner_.get());
        display.Initialize(output_surface_.Pass(), &scheduler);

        SurfaceId surface_id(7u);
        EXPECT_FALSE(scheduler.damaged);
        EXPECT_FALSE(scheduler.has_new_root_surface);
        display.SetSurfaceId(surface_id, 1.f);
        EXPECT_FALSE(scheduler.damaged);
        EXPECT_FALSE(scheduler.display_resized_);
        EXPECT_TRUE(scheduler.has_new_root_surface);

        scheduler.ResetDamageForTest();
        display.Resize(gfx::Size(100, 100));
        EXPECT_FALSE(scheduler.damaged);
        EXPECT_TRUE(scheduler.display_resized_);
        EXPECT_FALSE(scheduler.has_new_root_surface);

        factory_.Create(surface_id);

        // First draw from surface should have full damage.
        RenderPassList pass_list;
        scoped_ptr<RenderPass> pass = RenderPass::Create();
        pass->output_rect = gfx::Rect(0, 0, 100, 100);
        pass->damage_rect = gfx::Rect(10, 10, 1, 1);
        pass->id = RenderPassId(1, 1);
        pass_list.push_back(pass.Pass());

        scheduler.ResetDamageForTest();
        SubmitCompositorFrame(&pass_list, surface_id);
        EXPECT_TRUE(scheduler.damaged);
        EXPECT_FALSE(scheduler.display_resized_);
        EXPECT_FALSE(scheduler.has_new_root_surface);

        EXPECT_FALSE(scheduler.swapped);
        EXPECT_EQ(0u, output_surface_ptr_->num_sent_frames());
        display.DrawAndSwap();
        EXPECT_TRUE(scheduler.swapped);
        EXPECT_EQ(1u, output_surface_ptr_->num_sent_frames());
        EXPECT_EQ(gfx::Size(100, 100),
            software_output_device_->viewport_pixel_size());
        EXPECT_EQ(gfx::Rect(0, 0, 100, 100), software_output_device_->damage_rect());

        {
            // Only damaged portion should be swapped.
            pass = RenderPass::Create();
            pass->output_rect = gfx::Rect(0, 0, 100, 100);
            pass->damage_rect = gfx::Rect(10, 10, 1, 1);
            pass->id = RenderPassId(1, 1);

            pass_list.push_back(pass.Pass());
            scheduler.ResetDamageForTest();
            SubmitCompositorFrame(&pass_list, surface_id);
            EXPECT_TRUE(scheduler.damaged);
            EXPECT_FALSE(scheduler.display_resized_);
            EXPECT_FALSE(scheduler.has_new_root_surface);

            scheduler.swapped = false;
            display.DrawAndSwap();
            EXPECT_TRUE(scheduler.swapped);
            EXPECT_EQ(2u, output_surface_ptr_->num_sent_frames());
            EXPECT_EQ(gfx::Size(100, 100),
                software_output_device_->viewport_pixel_size());
            EXPECT_EQ(gfx::Rect(10, 10, 1, 1), software_output_device_->damage_rect());
        }

        {
            // Pass has no damage so shouldn't be swapped.
            pass = RenderPass::Create();
            pass->output_rect = gfx::Rect(0, 0, 100, 100);
            pass->damage_rect = gfx::Rect(10, 10, 0, 0);
            pass->id = RenderPassId(1, 1);

            pass_list.push_back(pass.Pass());
            scheduler.ResetDamageForTest();
            SubmitCompositorFrame(&pass_list, surface_id);
            EXPECT_TRUE(scheduler.damaged);
            EXPECT_FALSE(scheduler.display_resized_);
            EXPECT_FALSE(scheduler.has_new_root_surface);

            scheduler.swapped = false;
            display.DrawAndSwap();
            EXPECT_TRUE(scheduler.swapped);
            EXPECT_EQ(2u, output_surface_ptr_->num_sent_frames());
        }

        {
            // Pass is wrong size so shouldn't be swapped.
            pass = RenderPass::Create();
            pass->output_rect = gfx::Rect(0, 0, 99, 99);
            pass->damage_rect = gfx::Rect(10, 10, 10, 10);
            pass->id = RenderPassId(1, 1);

            pass_list.push_back(pass.Pass());
            scheduler.ResetDamageForTest();
            SubmitCompositorFrame(&pass_list, surface_id);
            EXPECT_TRUE(scheduler.damaged);
            EXPECT_FALSE(scheduler.display_resized_);
            EXPECT_FALSE(scheduler.has_new_root_surface);

            scheduler.swapped = false;
            display.DrawAndSwap();
            EXPECT_TRUE(scheduler.swapped);
            EXPECT_EQ(2u, output_surface_ptr_->num_sent_frames());
        }

        {
            // Previous frame wasn't swapped, so next swap should have full damage.
            pass = RenderPass::Create();
            pass->output_rect = gfx::Rect(0, 0, 100, 100);
            pass->damage_rect = gfx::Rect(10, 10, 0, 0);
            pass->id = RenderPassId(1, 1);

            pass_list.push_back(pass.Pass());
            scheduler.ResetDamageForTest();
            SubmitCompositorFrame(&pass_list, surface_id);
            EXPECT_TRUE(scheduler.damaged);
            EXPECT_FALSE(scheduler.display_resized_);
            EXPECT_FALSE(scheduler.has_new_root_surface);

            scheduler.swapped = false;
            display.DrawAndSwap();
            EXPECT_TRUE(scheduler.swapped);
            EXPECT_EQ(3u, output_surface_ptr_->num_sent_frames());
            EXPECT_EQ(gfx::Rect(0, 0, 100, 100),
                software_output_device_->damage_rect());
        }

        {
            // Pass has copy output request so should be swapped.
            pass = RenderPass::Create();
            pass->output_rect = gfx::Rect(0, 0, 100, 100);
            pass->damage_rect = gfx::Rect(10, 10, 0, 0);
            bool copy_called = false;
            pass->copy_requests.push_back(CopyOutputRequest::CreateRequest(
                base::Bind(&CopyCallback, &copy_called)));
            pass->id = RenderPassId(1, 1);

            pass_list.push_back(pass.Pass());
            scheduler.ResetDamageForTest();
            SubmitCompositorFrame(&pass_list, surface_id);
            EXPECT_TRUE(scheduler.damaged);
            EXPECT_FALSE(scheduler.display_resized_);
            EXPECT_FALSE(scheduler.has_new_root_surface);

            scheduler.swapped = false;
            display.DrawAndSwap();
            EXPECT_TRUE(scheduler.swapped);
            EXPECT_EQ(4u, output_surface_ptr_->num_sent_frames());
            EXPECT_TRUE(copy_called);
        }

        // Pass has latency info so should be swapped.
        {
            pass = RenderPass::Create();
            pass->output_rect = gfx::Rect(0, 0, 100, 100);
            pass->damage_rect = gfx::Rect(10, 10, 0, 0);
            pass->id = RenderPassId(1, 1);

            pass_list.push_back(pass.Pass());
            scheduler.ResetDamageForTest();
            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            pass_list.swap(frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> frame(new CompositorFrame);
            frame->delegated_frame_data = frame_data.Pass();
            frame->metadata.latency_info.push_back(ui::LatencyInfo());

            factory_.SubmitCompositorFrame(surface_id, frame.Pass(),
                SurfaceFactory::DrawCallback());
            EXPECT_TRUE(scheduler.damaged);
            EXPECT_FALSE(scheduler.display_resized_);
            EXPECT_FALSE(scheduler.has_new_root_surface);

            scheduler.swapped = false;
            display.DrawAndSwap();
            EXPECT_TRUE(scheduler.swapped);
            EXPECT_EQ(5u, output_surface_ptr_->num_sent_frames());
        }

        // Resize should cause a swap if no frame was swapped at the previous size.
        {
            scheduler.swapped = false;
            display.Resize(gfx::Size(200, 200));
            EXPECT_FALSE(scheduler.swapped);
            EXPECT_EQ(5u, output_surface_ptr_->num_sent_frames());

            pass = RenderPass::Create();
            pass->output_rect = gfx::Rect(0, 0, 200, 200);
            pass->damage_rect = gfx::Rect(10, 10, 10, 10);
            pass->id = RenderPassId(1, 1);

            pass_list.push_back(pass.Pass());
            scheduler.ResetDamageForTest();
            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            pass_list.swap(frame_data->render_pass_list);

            scoped_ptr<CompositorFrame> frame(new CompositorFrame);
            frame->delegated_frame_data = frame_data.Pass();

            factory_.SubmitCompositorFrame(surface_id, frame.Pass(),
                SurfaceFactory::DrawCallback());
            EXPECT_TRUE(scheduler.damaged);
            EXPECT_FALSE(scheduler.display_resized_);
            EXPECT_FALSE(scheduler.has_new_root_surface);

            scheduler.swapped = false;
            display.Resize(gfx::Size(100, 100));
            EXPECT_TRUE(scheduler.swapped);
            EXPECT_EQ(6u, output_surface_ptr_->num_sent_frames());
        }

        factory_.Destroy(surface_id);
    }

    class MockedContext : public TestWebGraphicsContext3D {
    public:
        MOCK_METHOD0(shallowFinishCHROMIUM, void());
    };

    TEST_F(DisplayTest, Finish)
    {
        scoped_ptr<MockedContext> context(new MockedContext());
        MockedContext* context_ptr = context.get();
        SetUpContext(context.Pass());

        EXPECT_CALL(*context_ptr, shallowFinishCHROMIUM()).Times(0);
        TestDisplayClient client;
        RendererSettings settings;
        settings.partial_swap_enabled = true;
        settings.finish_rendering_on_resize = true;
        Display display(&client, &manager_, shared_bitmap_manager_.get(), nullptr,
            settings);

        TestDisplayScheduler scheduler(&display, &fake_begin_frame_source_,
            task_runner_.get());
        display.Initialize(output_surface_.Pass(), &scheduler);

        SurfaceId surface_id(7u);
        display.SetSurfaceId(surface_id, 1.f);

        display.Resize(gfx::Size(100, 100));
        factory_.Create(surface_id);

        {
            RenderPassList pass_list;
            scoped_ptr<RenderPass> pass = RenderPass::Create();
            pass->output_rect = gfx::Rect(0, 0, 100, 100);
            pass->damage_rect = gfx::Rect(10, 10, 1, 1);
            pass->id = RenderPassId(1, 1);
            pass_list.push_back(pass.Pass());

            SubmitCompositorFrame(&pass_list, surface_id);
        }

        display.DrawAndSwap();

        // First resize and draw shouldn't finish.
        testing::Mock::VerifyAndClearExpectations(context_ptr);

        EXPECT_CALL(*context_ptr, shallowFinishCHROMIUM());
        display.Resize(gfx::Size(150, 150));
        testing::Mock::VerifyAndClearExpectations(context_ptr);

        // Another resize without a swap doesn't need to finish.
        EXPECT_CALL(*context_ptr, shallowFinishCHROMIUM()).Times(0);
        display.Resize(gfx::Size(200, 200));
        testing::Mock::VerifyAndClearExpectations(context_ptr);

        EXPECT_CALL(*context_ptr, shallowFinishCHROMIUM()).Times(0);
        {
            RenderPassList pass_list;
            scoped_ptr<RenderPass> pass = RenderPass::Create();
            pass->output_rect = gfx::Rect(0, 0, 200, 200);
            pass->damage_rect = gfx::Rect(10, 10, 1, 1);
            pass->id = RenderPassId(1, 1);
            pass_list.push_back(pass.Pass());

            SubmitCompositorFrame(&pass_list, surface_id);
        }

        display.DrawAndSwap();

        testing::Mock::VerifyAndClearExpectations(context_ptr);

        EXPECT_CALL(*context_ptr, shallowFinishCHROMIUM());
        display.Resize(gfx::Size(250, 250));
        testing::Mock::VerifyAndClearExpectations(context_ptr);

        factory_.Destroy(surface_id);
    }

} // namespace
} // namespace cc
