// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/surface_display_output_surface.h"

#include "cc/surfaces/onscreen_display_client.h"
#include "cc/surfaces/surface_id_allocator.h"
#include "cc/surfaces/surface_manager.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/ordered_simple_task_runner.h"
#include "cc/test/test_context_provider.h"
#include "cc/test/test_gpu_memory_buffer_manager.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class FakeOnscreenDisplayClient : public OnscreenDisplayClient {
    public:
        FakeOnscreenDisplayClient(
            SurfaceManager* manager,
            SharedBitmapManager* bitmap_manager,
            gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
            const RendererSettings& settings,
            scoped_refptr<base::SingleThreadTaskRunner> task_runner)
            : OnscreenDisplayClient(FakeOutputSurface::Create3d(),
                manager,
                bitmap_manager,
                gpu_memory_buffer_manager,
                settings,
                task_runner)
        {
            // Ownership is passed to another object later, store a pointer
            // to it now for future reference.
            fake_output_surface_ = static_cast<FakeOutputSurface*>(output_surface_.get());
        }

        FakeOutputSurface* output_surface() { return fake_output_surface_; }

    protected:
        FakeOutputSurface* fake_output_surface_;
    };

    class SurfaceDisplayOutputSurfaceTest : public testing::Test {
    public:
        SurfaceDisplayOutputSurfaceTest()
            : now_src_(new base::SimpleTestTickClock())
            , task_runner_(new OrderedSimpleTaskRunner(now_src_.get(), true))
            , allocator_(0)
            , display_size_(1920, 1080)
            , display_rect_(display_size_)
            , display_client_(&surface_manager_,
                  &bitmap_manager_,
                  &gpu_memory_buffer_manager_,
                  renderer_settings_,
                  task_runner_)
            , context_provider_(TestContextProvider::Create())
            , surface_display_output_surface_(&surface_manager_,
                  &allocator_,
                  context_provider_,
                  nullptr)
        {
            output_surface_ = display_client_.output_surface();
            display_client_.set_surface_output_surface(
                &surface_display_output_surface_);
            surface_display_output_surface_.set_display_client(&display_client_);
            surface_display_output_surface_.BindToClient(
                &surface_display_output_surface_client_);
            display_client_.display()->Resize(display_size_);

            EXPECT_FALSE(surface_display_output_surface_client_
                             .did_lose_output_surface_called());
        }

        ~SurfaceDisplayOutputSurfaceTest() override { }

        void SwapBuffersWithDamage(const gfx::Rect& damage_rect_)
        {
            scoped_ptr<RenderPass> render_pass(RenderPass::Create());
            render_pass->SetNew(RenderPassId(1, 1), display_rect_, damage_rect_,
                gfx::Transform());

            scoped_ptr<DelegatedFrameData> frame_data(new DelegatedFrameData);
            frame_data->render_pass_list.push_back(render_pass.Pass());

            CompositorFrame frame;
            frame.delegated_frame_data = frame_data.Pass();

            surface_display_output_surface_.SwapBuffers(&frame);
        }

        void SetUp() override
        {
            // Draw the first frame to start in an "unlocked" state.
            SwapBuffersWithDamage(display_rect_);

            EXPECT_EQ(0u, output_surface_->num_sent_frames());
            task_runner_->RunUntilIdle();
            EXPECT_EQ(1u, output_surface_->num_sent_frames());
        }

    protected:
        scoped_ptr<base::SimpleTestTickClock> now_src_;
        scoped_refptr<OrderedSimpleTaskRunner> task_runner_;
        SurfaceIdAllocator allocator_;

        const gfx::Size display_size_;
        const gfx::Rect display_rect_;
        FakeOutputSurface* output_surface_;
        SurfaceManager surface_manager_;
        TestSharedBitmapManager bitmap_manager_;
        TestGpuMemoryBufferManager gpu_memory_buffer_manager_;
        RendererSettings renderer_settings_;
        FakeOnscreenDisplayClient display_client_;

        scoped_refptr<TestContextProvider> context_provider_;

        FakeOutputSurfaceClient surface_display_output_surface_client_;
        SurfaceDisplayOutputSurface surface_display_output_surface_;
    };

    TEST_F(SurfaceDisplayOutputSurfaceTest, DamageTriggersSwapBuffers)
    {
        SwapBuffersWithDamage(display_rect_);
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
        task_runner_->RunUntilIdle();
        EXPECT_EQ(2u, output_surface_->num_sent_frames());
    }

    TEST_F(SurfaceDisplayOutputSurfaceTest, NoDamageDoesNotTriggerSwapBuffers)
    {
        SwapBuffersWithDamage(gfx::Rect());
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
        task_runner_->RunUntilIdle();
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
    }

    TEST_F(SurfaceDisplayOutputSurfaceTest, SuspendedDoesNotTriggerSwapBuffers)
    {
        SwapBuffersWithDamage(display_rect_);
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
        output_surface_->set_suspended_for_recycle(true);
        task_runner_->RunUntilIdle();
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
        SwapBuffersWithDamage(display_rect_);
        task_runner_->RunUntilIdle();
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
        output_surface_->set_suspended_for_recycle(false);
        SwapBuffersWithDamage(display_rect_);
        task_runner_->RunUntilIdle();
        EXPECT_EQ(2u, output_surface_->num_sent_frames());
    }

    TEST_F(SurfaceDisplayOutputSurfaceTest,
        LockingResourcesDoesNotIndirectlyCauseDamage)
    {
        surface_display_output_surface_.ForceReclaimResources();
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
        task_runner_->RunPendingTasks();
        EXPECT_EQ(1u, output_surface_->num_sent_frames());

        SwapBuffersWithDamage(gfx::Rect());
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
        task_runner_->RunUntilIdle();
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
    }

} // namespace
} // namespace cc
