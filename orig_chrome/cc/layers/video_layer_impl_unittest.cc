// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/video_layer_impl.h"

#include "cc/layers/video_frame_provider_client_impl.h"
#include "cc/output/context_provider.h"
#include "cc/output/output_surface.h"
#include "cc/quads/draw_quad.h"
#include "cc/quads/yuv_video_draw_quad.h"
#include "cc/test/fake_video_frame_provider.h"
#include "cc/test/layer_test_common.h"
#include "cc/trees/single_thread_proxy.h"
#include "media/base/video_frame.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    // NOTE: We cannot use DebugScopedSetImplThreadAndMainThreadBlocked in these
    // tests because it gets destroyed before the VideoLayerImpl is destroyed. This
    // causes a DCHECK in VideoLayerImpl's destructor to fail.
    static void DebugSetImplThreadAndMainThreadBlocked(Proxy* proxy)
    {
#if DCHECK_IS_ON()
        proxy->SetCurrentThreadIsImplThread(true);
        proxy->SetMainThreadBlocked(true);
#endif
    }

    TEST(VideoLayerImplTest, Occlusion)
    {
        gfx::Size layer_size(1000, 1000);
        gfx::Size viewport_size(1000, 1000);

        LayerTestCommon::LayerImplTest impl;
        DebugSetImplThreadAndMainThreadBlocked(impl.proxy());

        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
            media::PIXEL_FORMAT_YV12, gfx::Size(10, 10), gfx::Rect(10, 10),
            gfx::Size(10, 10), base::TimeDelta());
        FakeVideoFrameProvider provider;
        provider.set_frame(video_frame);

        VideoLayerImpl* video_layer_impl = impl.AddChildToRoot<VideoLayerImpl>(&provider, media::VIDEO_ROTATION_0);
        video_layer_impl->SetBounds(layer_size);
        video_layer_impl->SetDrawsContent(true);

        impl.CalcDrawProps(viewport_size);

        {
            SCOPED_TRACE("No occlusion");
            gfx::Rect occluded;
            impl.AppendQuadsWithOcclusion(video_layer_impl, occluded);

            LayerTestCommon::VerifyQuadsExactlyCoverRect(impl.quad_list(),
                gfx::Rect(layer_size));
            EXPECT_EQ(1u, impl.quad_list().size());
        }

        {
            SCOPED_TRACE("Full occlusion");
            gfx::Rect occluded(video_layer_impl->visible_layer_rect());
            impl.AppendQuadsWithOcclusion(video_layer_impl, occluded);

            LayerTestCommon::VerifyQuadsExactlyCoverRect(impl.quad_list(), gfx::Rect());
            EXPECT_EQ(impl.quad_list().size(), 0u);
        }

        {
            SCOPED_TRACE("Partial occlusion");
            gfx::Rect occluded(200, 0, 800, 1000);
            impl.AppendQuadsWithOcclusion(video_layer_impl, occluded);

            size_t partially_occluded_count = 0;
            LayerTestCommon::VerifyQuadsAreOccluded(
                impl.quad_list(), occluded, &partially_occluded_count);
            // The layer outputs one quad, which is partially occluded.
            EXPECT_EQ(1u, impl.quad_list().size());
            EXPECT_EQ(1u, partially_occluded_count);
        }
    }

    TEST(VideoLayerImplTest, OccludesOtherLayers)
    {
        gfx::Size layer_size(1000, 1000);
        gfx::Rect visible(layer_size);

        LayerTestCommon::LayerImplTest impl;
        impl.host_impl()->SetViewportSize(layer_size);
        DebugSetImplThreadAndMainThreadBlocked(impl.proxy());
        auto active_tree = impl.host_impl()->active_tree();

        // Create a video layer with no frame on top of another layer.
        scoped_ptr<LayerImpl> layer_impl = LayerImpl::Create(active_tree, 3);
        layer_impl->SetHasRenderSurface(true);
        layer_impl->SetBounds(layer_size);
        layer_impl->SetDrawsContent(true);
        const auto& draw_properties = layer_impl->draw_properties();

        FakeVideoFrameProvider provider;
        scoped_ptr<VideoLayerImpl> video_layer_impl = VideoLayerImpl::Create(
            active_tree, 4, &provider, media::VIDEO_ROTATION_0);
        video_layer_impl->SetBounds(layer_size);
        video_layer_impl->SetDrawsContent(true);
        video_layer_impl->SetContentsOpaque(true);

        layer_impl->AddChild(video_layer_impl.Pass());
        active_tree->SetRootLayer(layer_impl.Pass());

        active_tree->BuildPropertyTreesForTesting();

        active_tree->UpdateDrawProperties(false);

        // We don't have a frame yet, so the video doesn't occlude the layer below it.
        EXPECT_FALSE(draw_properties.occlusion_in_content_space.IsOccluded(visible));

        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
            media::PIXEL_FORMAT_YV12, gfx::Size(10, 10), gfx::Rect(10, 10),
            gfx::Size(10, 10), base::TimeDelta());
        provider.set_frame(video_frame);
        active_tree->set_needs_update_draw_properties();
        active_tree->UpdateDrawProperties(false);

        // We have a frame now, so the video occludes the layer below it.
        EXPECT_TRUE(draw_properties.occlusion_in_content_space.IsOccluded(visible));
    }

    TEST(VideoLayerImplTest, DidBecomeActiveShouldSetActiveVideoLayer)
    {
        LayerTestCommon::LayerImplTest impl;
        DebugSetImplThreadAndMainThreadBlocked(impl.proxy());

        FakeVideoFrameProvider provider;
        VideoLayerImpl* video_layer_impl = impl.AddChildToRoot<VideoLayerImpl>(&provider, media::VIDEO_ROTATION_0);

        VideoFrameProviderClientImpl* client = static_cast<VideoFrameProviderClientImpl*>(provider.client());
        ASSERT_TRUE(client);

        EXPECT_FALSE(client->ActiveVideoLayer());
        video_layer_impl->DidBecomeActive();
        EXPECT_EQ(video_layer_impl, client->ActiveVideoLayer());
    }

    TEST(VideoLayerImplTest, Rotated0)
    {
        gfx::Size layer_size(100, 50);
        gfx::Size viewport_size(1000, 500);

        LayerTestCommon::LayerImplTest impl;
        DebugSetImplThreadAndMainThreadBlocked(impl.proxy());

        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
            media::PIXEL_FORMAT_YV12, gfx::Size(20, 10), gfx::Rect(20, 10),
            gfx::Size(20, 10), base::TimeDelta());
        FakeVideoFrameProvider provider;
        provider.set_frame(video_frame);

        VideoLayerImpl* video_layer_impl = impl.AddChildToRoot<VideoLayerImpl>(&provider, media::VIDEO_ROTATION_0);
        video_layer_impl->SetBounds(layer_size);
        video_layer_impl->SetDrawsContent(true);

        impl.CalcDrawProps(viewport_size);
        gfx::Rect occluded;
        impl.AppendQuadsWithOcclusion(video_layer_impl, occluded);

        EXPECT_EQ(1u, impl.quad_list().size());

        gfx::Point3F p1(0, impl.quad_list().front()->rect.height(), 0);
        gfx::Point3F p2(impl.quad_list().front()->rect.width(), 0, 0);
        impl.quad_list()
            .front()
            ->shared_quad_state->quad_to_target_transform.TransformPoint(&p1);
        impl.quad_list()
            .front()
            ->shared_quad_state->quad_to_target_transform.TransformPoint(&p2);
        EXPECT_EQ(gfx::Point3F(0, 50, 0), p1);
        EXPECT_EQ(gfx::Point3F(100, 0, 0), p2);
    }

    TEST(VideoLayerImplTest, Rotated90)
    {
        gfx::Size layer_size(100, 50);
        gfx::Size viewport_size(1000, 500);

        LayerTestCommon::LayerImplTest impl;
        DebugSetImplThreadAndMainThreadBlocked(impl.proxy());

        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
            media::PIXEL_FORMAT_YV12, gfx::Size(20, 10), gfx::Rect(20, 10),
            gfx::Size(20, 10), base::TimeDelta());
        FakeVideoFrameProvider provider;
        provider.set_frame(video_frame);

        VideoLayerImpl* video_layer_impl = impl.AddChildToRoot<VideoLayerImpl>(&provider, media::VIDEO_ROTATION_90);
        video_layer_impl->SetBounds(layer_size);
        video_layer_impl->SetDrawsContent(true);

        impl.CalcDrawProps(viewport_size);
        gfx::Rect occluded;
        impl.AppendQuadsWithOcclusion(video_layer_impl, occluded);

        EXPECT_EQ(1u, impl.quad_list().size());

        gfx::Point3F p1(0, impl.quad_list().front()->rect.height(), 0);
        gfx::Point3F p2(impl.quad_list().front()->rect.width(), 0, 0);
        impl.quad_list()
            .front()
            ->shared_quad_state->quad_to_target_transform.TransformPoint(&p1);
        impl.quad_list()
            .front()
            ->shared_quad_state->quad_to_target_transform.TransformPoint(&p2);
        EXPECT_EQ(gfx::Point3F(0, 0, 0), p1);
        EXPECT_EQ(gfx::Point3F(100, 50, 0), p2);
    }

    TEST(VideoLayerImplTest, Rotated180)
    {
        gfx::Size layer_size(100, 50);
        gfx::Size viewport_size(1000, 500);

        LayerTestCommon::LayerImplTest impl;
        DebugSetImplThreadAndMainThreadBlocked(impl.proxy());

        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
            media::PIXEL_FORMAT_YV12, gfx::Size(20, 10), gfx::Rect(20, 10),
            gfx::Size(20, 10), base::TimeDelta());
        FakeVideoFrameProvider provider;
        provider.set_frame(video_frame);

        VideoLayerImpl* video_layer_impl = impl.AddChildToRoot<VideoLayerImpl>(&provider, media::VIDEO_ROTATION_180);
        video_layer_impl->SetBounds(layer_size);
        video_layer_impl->SetDrawsContent(true);

        impl.CalcDrawProps(viewport_size);
        gfx::Rect occluded;
        impl.AppendQuadsWithOcclusion(video_layer_impl, occluded);

        EXPECT_EQ(1u, impl.quad_list().size());

        gfx::Point3F p1(0, impl.quad_list().front()->rect.height(), 0);
        gfx::Point3F p2(impl.quad_list().front()->rect.width(), 0, 0);
        impl.quad_list()
            .front()
            ->shared_quad_state->quad_to_target_transform.TransformPoint(&p1);
        impl.quad_list()
            .front()
            ->shared_quad_state->quad_to_target_transform.TransformPoint(&p2);
        EXPECT_EQ(gfx::Point3F(100, 0, 0), p1);
        EXPECT_EQ(gfx::Point3F(0, 50, 0), p2);
    }

    TEST(VideoLayerImplTest, Rotated270)
    {
        gfx::Size layer_size(100, 50);
        gfx::Size viewport_size(1000, 500);

        LayerTestCommon::LayerImplTest impl;
        DebugSetImplThreadAndMainThreadBlocked(impl.proxy());

        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
            media::PIXEL_FORMAT_YV12, gfx::Size(20, 10), gfx::Rect(20, 10),
            gfx::Size(20, 10), base::TimeDelta());
        FakeVideoFrameProvider provider;
        provider.set_frame(video_frame);

        VideoLayerImpl* video_layer_impl = impl.AddChildToRoot<VideoLayerImpl>(&provider, media::VIDEO_ROTATION_270);
        video_layer_impl->SetBounds(layer_size);
        video_layer_impl->SetDrawsContent(true);

        impl.CalcDrawProps(viewport_size);
        gfx::Rect occluded;
        impl.AppendQuadsWithOcclusion(video_layer_impl, occluded);

        EXPECT_EQ(1u, impl.quad_list().size());

        gfx::Point3F p1(0, impl.quad_list().front()->rect.height(), 0);
        gfx::Point3F p2(impl.quad_list().front()->rect.width(), 0, 0);
        impl.quad_list()
            .front()
            ->shared_quad_state->quad_to_target_transform.TransformPoint(&p1);
        impl.quad_list()
            .front()
            ->shared_quad_state->quad_to_target_transform.TransformPoint(&p2);
        EXPECT_EQ(gfx::Point3F(100, 50, 0), p1);
        EXPECT_EQ(gfx::Point3F(0, 0, 0), p2);
    }

    void EmptyCallback(unsigned sync_point)
    {
    }

    TEST(VideoLayerImplTest, SoftwareVideoFrameGeneratesYUVQuad)
    {
        gfx::Size layer_size(1000, 1000);
        gfx::Size viewport_size(1000, 1000);

        LayerTestCommon::LayerImplTest impl;
        DebugSetImplThreadAndMainThreadBlocked(impl.proxy());

        gpu::MailboxHolder mailbox_holder;
        mailbox_holder.mailbox.name[0] = 1;

        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
            media::PIXEL_FORMAT_YV12, gfx::Size(20, 10), gfx::Rect(20, 10),
            gfx::Size(20, 10), base::TimeDelta());

        FakeVideoFrameProvider provider;
        provider.set_frame(video_frame);

        VideoLayerImpl* video_layer_impl = impl.AddChildToRoot<VideoLayerImpl>(&provider, media::VIDEO_ROTATION_0);
        video_layer_impl->SetBounds(layer_size);
        video_layer_impl->SetDrawsContent(true);

        gfx::Rect occluded;
        impl.AppendQuadsWithOcclusion(video_layer_impl, occluded);

        EXPECT_EQ(1u, impl.quad_list().size());
        const DrawQuad* draw_quad = impl.quad_list().ElementAt(0);
        ASSERT_EQ(DrawQuad::YUV_VIDEO_CONTENT, draw_quad->material);

        const YUVVideoDrawQuad* yuv_draw_quad = static_cast<const YUVVideoDrawQuad*>(draw_quad);
        EXPECT_EQ(yuv_draw_quad->uv_tex_size.height(),
            (yuv_draw_quad->ya_tex_size.height() + 1) / 2);
        EXPECT_EQ(yuv_draw_quad->uv_tex_size.width(),
            (yuv_draw_quad->ya_tex_size.width() + 1) / 2);
    }

    TEST(VideoLayerImplTest, NativeYUVFrameGeneratesYUVQuad)
    {
        gfx::Size layer_size(1000, 1000);
        gfx::Size viewport_size(1000, 1000);

        LayerTestCommon::LayerImplTest impl;
        DebugSetImplThreadAndMainThreadBlocked(impl.proxy());

        gpu::MailboxHolder mailbox_holder;
        mailbox_holder.mailbox.name[0] = 1;

        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::WrapYUV420NativeTextures(
            mailbox_holder, mailbox_holder, mailbox_holder,
            base::Bind(EmptyCallback), gfx::Size(10, 10), gfx::Rect(10, 10),
            gfx::Size(10, 10), base::TimeDelta());
        video_frame->metadata()->SetBoolean(media::VideoFrameMetadata::ALLOW_OVERLAY,
            true);
        FakeVideoFrameProvider provider;
        provider.set_frame(video_frame);

        VideoLayerImpl* video_layer_impl = impl.AddChildToRoot<VideoLayerImpl>(&provider, media::VIDEO_ROTATION_0);
        video_layer_impl->SetBounds(layer_size);
        video_layer_impl->SetDrawsContent(true);

        gfx::Rect occluded;
        impl.AppendQuadsWithOcclusion(video_layer_impl, occluded);

        EXPECT_EQ(1u, impl.quad_list().size());
        const DrawQuad* draw_quad = impl.quad_list().ElementAt(0);
        ASSERT_EQ(DrawQuad::YUV_VIDEO_CONTENT, draw_quad->material);

        const YUVVideoDrawQuad* yuv_draw_quad = static_cast<const YUVVideoDrawQuad*>(draw_quad);
        EXPECT_EQ(yuv_draw_quad->uv_tex_size.height(),
            (yuv_draw_quad->ya_tex_size.height() + 1) / 2);
        EXPECT_EQ(yuv_draw_quad->uv_tex_size.width(),
            (yuv_draw_quad->ya_tex_size.width() + 1) / 2);
    }

} // namespace
} // namespace cc
