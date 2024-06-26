// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/video_resource_updater.h"

#include "cc/resources/resource_provider.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/fake_resource_provider.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_web_graphics_context_3d.h"
#include "cc/trees/blocking_task_runner.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "media/base/video_frame.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class WebGraphicsContext3DUploadCounter : public TestWebGraphicsContext3D {
    public:
        void texSubImage2D(GLenum target,
            GLint level,
            GLint xoffset,
            GLint yoffset,
            GLsizei width,
            GLsizei height,
            GLenum format,
            GLenum type,
            const void* pixels) override
        {
            ++upload_count_;
        }

        int UploadCount() { return upload_count_; }
        void ResetUploadCount() { upload_count_ = 0; }

    private:
        int upload_count_;
    };

    class SharedBitmapManagerAllocationCounter : public TestSharedBitmapManager {
    public:
        scoped_ptr<SharedBitmap> AllocateSharedBitmap(
            const gfx::Size& size) override
        {
            ++allocation_count_;
            return TestSharedBitmapManager::AllocateSharedBitmap(size);
        }

        int AllocationCount() { return allocation_count_; }
        void ResetAllocationCount() { allocation_count_ = 0; }

    private:
        int allocation_count_;
    };

    class VideoResourceUpdaterTest : public testing::Test {
    protected:
        VideoResourceUpdaterTest()
        {
            scoped_ptr<WebGraphicsContext3DUploadCounter> context3d(
                new WebGraphicsContext3DUploadCounter());

            context3d_ = context3d.get();

            output_surface3d_ = FakeOutputSurface::Create3d(context3d.Pass());
            CHECK(output_surface3d_->BindToClient(&client_));

            output_surface_software_ = FakeOutputSurface::CreateSoftware(
                make_scoped_ptr(new SoftwareOutputDevice));
            CHECK(output_surface_software_->BindToClient(&client_));

            shared_bitmap_manager_.reset(new SharedBitmapManagerAllocationCounter());
            resource_provider3d_ = FakeResourceProvider::Create(
                output_surface3d_.get(), shared_bitmap_manager_.get());

            resource_provider_software_ = FakeResourceProvider::Create(
                output_surface_software_.get(), shared_bitmap_manager_.get());
        }

        scoped_refptr<media::VideoFrame> CreateTestYUVVideoFrame()
        {
            const int kDimension = 10;
            gfx::Size size(kDimension, kDimension);
            static uint8 y_data[kDimension * kDimension] = { 0 };
            static uint8 u_data[kDimension * kDimension / 2] = { 0 };
            static uint8 v_data[kDimension * kDimension / 2] = { 0 };

            return media::VideoFrame::WrapExternalYuvData(
                media::PIXEL_FORMAT_YV16, // format
                size, // coded_size
                gfx::Rect(size), // visible_rect
                size, // natural_size
                size.width(), // y_stride
                size.width() / 2, // u_stride
                size.width() / 2, // v_stride
                y_data, // y_data
                u_data, // u_data
                v_data, // v_data
                base::TimeDelta()); // timestamp
        }

        static void ReleaseMailboxCB(unsigned sync_point) { }

        scoped_refptr<media::VideoFrame> CreateTestRGBAHardwareVideoFrame()
        {
            const int kDimension = 10;
            gfx::Size size(kDimension, kDimension);

            gpu::Mailbox mailbox;
            mailbox.name[0] = 51;

            const unsigned sync_point = 7;
            const unsigned target = GL_TEXTURE_2D;
            return media::VideoFrame::WrapNativeTexture(
                media::PIXEL_FORMAT_ARGB,
                gpu::MailboxHolder(mailbox, target, sync_point),
                base::Bind(&ReleaseMailboxCB),
                size, // coded_size
                gfx::Rect(size), // visible_rect
                size, // natural_size
                base::TimeDelta()); // timestamp
        }

        scoped_refptr<media::VideoFrame> CreateTestYUVHardareVideoFrame()
        {
            const int kDimension = 10;
            gfx::Size size(kDimension, kDimension);

            const int kPlanesNum = 3;
            gpu::Mailbox mailbox[kPlanesNum];
            for (int i = 0; i < kPlanesNum; ++i) {
                mailbox[i].name[0] = 50 + 1;
            }
            const unsigned sync_point = 7;
            const unsigned target = GL_TEXTURE_RECTANGLE_ARB;
            return media::VideoFrame::WrapYUV420NativeTextures(
                gpu::MailboxHolder(mailbox[media::VideoFrame::kYPlane], target,
                    sync_point),
                gpu::MailboxHolder(mailbox[media::VideoFrame::kUPlane], target,
                    sync_point),
                gpu::MailboxHolder(mailbox[media::VideoFrame::kVPlane], target,
                    sync_point),
                base::Bind(&ReleaseMailboxCB),
                size, // coded_size
                gfx::Rect(size), // visible_rect
                size, // natural_size
                base::TimeDelta()); // timestamp
        }

        WebGraphicsContext3DUploadCounter* context3d_;
        FakeOutputSurfaceClient client_;
        scoped_ptr<FakeOutputSurface> output_surface3d_;
        scoped_ptr<FakeOutputSurface> output_surface_software_;
        scoped_ptr<SharedBitmapManagerAllocationCounter> shared_bitmap_manager_;
        scoped_ptr<ResourceProvider> resource_provider3d_;
        scoped_ptr<ResourceProvider> resource_provider_software_;
    };

    TEST_F(VideoResourceUpdaterTest, SoftwareFrame)
    {
        VideoResourceUpdater updater(output_surface3d_->context_provider(),
            resource_provider3d_.get());
        scoped_refptr<media::VideoFrame> video_frame = CreateTestYUVVideoFrame();

        VideoFrameExternalResources resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::YUV_RESOURCE, resources.type);
    }

    TEST_F(VideoResourceUpdaterTest, ReuseResource)
    {
        VideoResourceUpdater updater(output_surface3d_->context_provider(),
            resource_provider3d_.get());
        scoped_refptr<media::VideoFrame> video_frame = CreateTestYUVVideoFrame();
        video_frame->set_timestamp(base::TimeDelta::FromSeconds(1234));

        // Allocate the resources for a YUV video frame.
        context3d_->ResetUploadCount();
        VideoFrameExternalResources resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::YUV_RESOURCE, resources.type);
        EXPECT_EQ(size_t(3), resources.mailboxes.size());
        EXPECT_EQ(size_t(3), resources.release_callbacks.size());
        EXPECT_EQ(size_t(0), resources.software_resources.size());
        // Expect exactly three texture uploads, one for each plane.
        EXPECT_EQ(3, context3d_->UploadCount());

        // Simulate the ResourceProvider releasing the resources back to the video
        // updater.
        for (ReleaseCallbackImpl& release_callback : resources.release_callbacks)
            release_callback.Run(0, false, nullptr);

        // Allocate resources for the same frame.
        context3d_->ResetUploadCount();
        resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::YUV_RESOURCE, resources.type);
        EXPECT_EQ(size_t(3), resources.mailboxes.size());
        EXPECT_EQ(size_t(3), resources.release_callbacks.size());
        // The data should be reused so expect no texture uploads.
        EXPECT_EQ(0, context3d_->UploadCount());
    }

    TEST_F(VideoResourceUpdaterTest, ReuseResourceNoDelete)
    {
        VideoResourceUpdater updater(output_surface3d_->context_provider(),
            resource_provider3d_.get());
        scoped_refptr<media::VideoFrame> video_frame = CreateTestYUVVideoFrame();
        video_frame->set_timestamp(base::TimeDelta::FromSeconds(1234));

        // Allocate the resources for a YUV video frame.
        context3d_->ResetUploadCount();
        VideoFrameExternalResources resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::YUV_RESOURCE, resources.type);
        EXPECT_EQ(size_t(3), resources.mailboxes.size());
        EXPECT_EQ(size_t(3), resources.release_callbacks.size());
        EXPECT_EQ(size_t(0), resources.software_resources.size());
        // Expect exactly three texture uploads, one for each plane.
        EXPECT_EQ(3, context3d_->UploadCount());

        // Allocate resources for the same frame.
        context3d_->ResetUploadCount();
        resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::YUV_RESOURCE, resources.type);
        EXPECT_EQ(size_t(3), resources.mailboxes.size());
        EXPECT_EQ(size_t(3), resources.release_callbacks.size());
        // The data should be reused so expect no texture uploads.
        EXPECT_EQ(0, context3d_->UploadCount());
    }

    TEST_F(VideoResourceUpdaterTest, SoftwareFrameSoftwareCompositor)
    {
        VideoResourceUpdater updater(nullptr, resource_provider_software_.get());
        scoped_refptr<media::VideoFrame> video_frame = CreateTestYUVVideoFrame();

        VideoFrameExternalResources resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::SOFTWARE_RESOURCE, resources.type);
    }

    TEST_F(VideoResourceUpdaterTest, ReuseResourceSoftwareCompositor)
    {
        VideoResourceUpdater updater(nullptr, resource_provider_software_.get());
        scoped_refptr<media::VideoFrame> video_frame = CreateTestYUVVideoFrame();
        video_frame->set_timestamp(base::TimeDelta::FromSeconds(1234));

        // Allocate the resources for a software video frame.
        shared_bitmap_manager_->ResetAllocationCount();
        VideoFrameExternalResources resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::SOFTWARE_RESOURCE, resources.type);
        EXPECT_EQ(size_t(0), resources.mailboxes.size());
        EXPECT_EQ(size_t(0), resources.release_callbacks.size());
        EXPECT_EQ(size_t(1), resources.software_resources.size());
        // Expect exactly one allocated shared bitmap.
        EXPECT_EQ(1, shared_bitmap_manager_->AllocationCount());

        // Simulate the ResourceProvider releasing the resource back to the video
        // updater.
        resources.software_release_callback.Run(0, false, nullptr);

        // Allocate resources for the same frame.
        shared_bitmap_manager_->ResetAllocationCount();
        resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::SOFTWARE_RESOURCE, resources.type);
        EXPECT_EQ(size_t(0), resources.mailboxes.size());
        EXPECT_EQ(size_t(0), resources.release_callbacks.size());
        EXPECT_EQ(size_t(1), resources.software_resources.size());
        // The data should be reused so expect no new allocations.
        EXPECT_EQ(0, shared_bitmap_manager_->AllocationCount());
    }

    TEST_F(VideoResourceUpdaterTest, ReuseResourceNoDeleteSoftwareCompositor)
    {
        VideoResourceUpdater updater(nullptr, resource_provider_software_.get());
        scoped_refptr<media::VideoFrame> video_frame = CreateTestYUVVideoFrame();
        video_frame->set_timestamp(base::TimeDelta::FromSeconds(1234));

        // Allocate the resources for a software video frame.
        shared_bitmap_manager_->ResetAllocationCount();
        VideoFrameExternalResources resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::SOFTWARE_RESOURCE, resources.type);
        EXPECT_EQ(size_t(0), resources.mailboxes.size());
        EXPECT_EQ(size_t(0), resources.release_callbacks.size());
        EXPECT_EQ(size_t(1), resources.software_resources.size());
        // Expect exactly one allocated shared bitmap.
        EXPECT_EQ(1, shared_bitmap_manager_->AllocationCount());

        // Allocate resources for the same frame.
        shared_bitmap_manager_->ResetAllocationCount();
        resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::SOFTWARE_RESOURCE, resources.type);
        EXPECT_EQ(size_t(0), resources.mailboxes.size());
        EXPECT_EQ(size_t(0), resources.release_callbacks.size());
        EXPECT_EQ(size_t(1), resources.software_resources.size());
        // The data should be reused so expect no new allocations.
        EXPECT_EQ(0, shared_bitmap_manager_->AllocationCount());
    }

    TEST_F(VideoResourceUpdaterTest, CreateForHardwarePlanes)
    {
        VideoResourceUpdater updater(output_surface3d_->context_provider(),
            resource_provider3d_.get());

        scoped_refptr<media::VideoFrame> video_frame = CreateTestRGBAHardwareVideoFrame();

        VideoFrameExternalResources resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::RGBA_RESOURCE, resources.type);
        EXPECT_EQ(1u, resources.mailboxes.size());
        EXPECT_EQ(1u, resources.release_callbacks.size());
        EXPECT_EQ(0u, resources.software_resources.size());

        video_frame = CreateTestYUVHardareVideoFrame();

        resources = updater.CreateExternalResourcesFromVideoFrame(video_frame);
        EXPECT_EQ(VideoFrameExternalResources::YUV_RESOURCE, resources.type);
        EXPECT_TRUE(resources.read_lock_fences_enabled);
        EXPECT_EQ(3u, resources.mailboxes.size());
        EXPECT_EQ(3u, resources.release_callbacks.size());
        EXPECT_EQ(0u, resources.software_resources.size());
    }
} // namespace
} // namespace cc
