// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/message_loop/message_loop.h"
#include "cc/output/gl_renderer.h"
#include "cc/quads/draw_quad.h"
#include "cc/quads/picture_draw_quad.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/resources/video_resource_updater.h"
#include "cc/test/fake_display_list_raster_source.h"
#include "cc/test/fake_display_list_recording_source.h"
#include "cc/test/pixel_test.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "media/base/video_frame.h"
#include "third_party/skia/include/core/SkColorPriv.h"
#include "third_party/skia/include/core/SkImageFilter.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "third_party/skia/include/effects/SkColorFilterImageFilter.h"
#include "third_party/skia/include/effects/SkColorMatrixFilter.h"
#include "ui/gfx/geometry/rect_conversions.h"

using gpu::gles2::GLES2Interface;

namespace cc {
namespace {

#if !defined(OS_ANDROID)
    scoped_ptr<RenderPass> CreateTestRootRenderPass(RenderPassId id,
        const gfx::Rect& rect)
    {
        scoped_ptr<RenderPass> pass = RenderPass::Create();
        const gfx::Rect output_rect = rect;
        const gfx::Rect damage_rect = rect;
        const gfx::Transform transform_to_root_target;
        pass->SetNew(id, output_rect, damage_rect, transform_to_root_target);
        return pass.Pass();
    }

    scoped_ptr<RenderPass> CreateTestRenderPass(
        RenderPassId id,
        const gfx::Rect& rect,
        const gfx::Transform& transform_to_root_target)
    {
        scoped_ptr<RenderPass> pass = RenderPass::Create();
        const gfx::Rect output_rect = rect;
        const gfx::Rect damage_rect = rect;
        pass->SetNew(id, output_rect, damage_rect, transform_to_root_target);
        return pass.Pass();
    }

    SharedQuadState* CreateTestSharedQuadState(
        gfx::Transform quad_to_target_transform,
        const gfx::Rect& rect,
        RenderPass* render_pass)
    {
        const gfx::Size layer_bounds = rect.size();
        const gfx::Rect visible_layer_rect = rect;
        const gfx::Rect clip_rect = rect;
        const bool is_clipped = false;
        const float opacity = 1.0f;
        const SkXfermode::Mode blend_mode = SkXfermode::kSrcOver_Mode;
        int sorting_context_id = 0;
        SharedQuadState* shared_state = render_pass->CreateAndAppendSharedQuadState();
        shared_state->SetAll(quad_to_target_transform, layer_bounds,
            visible_layer_rect, clip_rect, is_clipped, opacity,
            blend_mode, sorting_context_id);
        return shared_state;
    }

    SharedQuadState* CreateTestSharedQuadStateClipped(
        gfx::Transform quad_to_target_transform,
        const gfx::Rect& rect,
        const gfx::Rect& clip_rect,
        RenderPass* render_pass)
    {
        const gfx::Size layer_bounds = rect.size();
        const gfx::Rect visible_layer_rect = clip_rect;
        const bool is_clipped = true;
        const float opacity = 1.0f;
        const SkXfermode::Mode blend_mode = SkXfermode::kSrcOver_Mode;
        int sorting_context_id = 0;
        SharedQuadState* shared_state = render_pass->CreateAndAppendSharedQuadState();
        shared_state->SetAll(quad_to_target_transform, layer_bounds,
            visible_layer_rect, clip_rect, is_clipped, opacity,
            blend_mode, sorting_context_id);
        return shared_state;
    }

    void CreateTestRenderPassDrawQuad(const SharedQuadState* shared_state,
        const gfx::Rect& rect,
        RenderPassId pass_id,
        RenderPass* render_pass)
    {
        RenderPassDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        quad->SetNew(shared_state,
            rect,
            rect,
            pass_id,
            0, // mask_resource_id
            gfx::Vector2dF(), // mask_uv_scale
            gfx::Size(), // mask_texture_size
            FilterOperations(), // foreground filters
            gfx::Vector2dF(), // filters scale
            FilterOperations()); // background filters
    }

    void CreateTestTwoColoredTextureDrawQuad(const gfx::Rect& rect,
        SkColor texel_color,
        SkColor texel_stripe_color,
        SkColor background_color,
        bool premultiplied_alpha,
        const SharedQuadState* shared_state,
        ResourceProvider* resource_provider,
        RenderPass* render_pass)
    {
        SkPMColor pixel_color = premultiplied_alpha
            ? SkPreMultiplyColor(texel_color)
            : SkPackARGB32NoCheck(SkColorGetA(texel_color),
                SkColorGetR(texel_color),
                SkColorGetG(texel_color),
                SkColorGetB(texel_color));
        SkPMColor pixel_stripe_color = premultiplied_alpha
            ? SkPreMultiplyColor(texel_stripe_color)
            : SkPackARGB32NoCheck(SkColorGetA(texel_stripe_color),
                SkColorGetR(texel_stripe_color),
                SkColorGetG(texel_stripe_color),
                SkColorGetB(texel_stripe_color));
        std::vector<uint32_t> pixels(rect.size().GetArea(), pixel_color);
        for (int i = rect.height() / 4; i < (rect.height() * 3 / 4); ++i) {
            for (int k = rect.width() / 4; k < (rect.width() * 3 / 4); ++k) {
                pixels[i * rect.width() + k] = pixel_stripe_color;
            }
        }
        ResourceId resource = resource_provider->CreateResource(
            rect.size(), GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            RGBA_8888);
        resource_provider->CopyToResource(
            resource, reinterpret_cast<uint8_t*>(&pixels.front()), rect.size());

        float vertex_opacity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        const gfx::PointF uv_top_left(0.0f, 0.0f);
        const gfx::PointF uv_bottom_right(1.0f, 1.0f);
        const bool flipped = false;
        const bool nearest_neighbor = false;
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_state, rect, gfx::Rect(), rect, resource,
            premultiplied_alpha, uv_top_left, uv_bottom_right,
            background_color, vertex_opacity, flipped, nearest_neighbor);
    }

    void CreateTestTextureDrawQuad(const gfx::Rect& rect,
        SkColor texel_color,
        SkColor background_color,
        bool premultiplied_alpha,
        const SharedQuadState* shared_state,
        ResourceProvider* resource_provider,
        RenderPass* render_pass)
    {
        SkPMColor pixel_color = premultiplied_alpha ? SkPreMultiplyColor(texel_color) : SkPackARGB32NoCheck(SkColorGetA(texel_color), SkColorGetR(texel_color), SkColorGetG(texel_color), SkColorGetB(texel_color));
        size_t num_pixels = static_cast<size_t>(rect.width()) * rect.height();
        std::vector<uint32_t> pixels(num_pixels, pixel_color);

        ResourceId resource = resource_provider->CreateResource(
            rect.size(), GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            RGBA_8888);
        resource_provider->CopyToResource(
            resource, reinterpret_cast<uint8_t*>(&pixels.front()), rect.size());

        float vertex_opacity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        const gfx::PointF uv_top_left(0.0f, 0.0f);
        const gfx::PointF uv_bottom_right(1.0f, 1.0f);
        const bool flipped = false;
        const bool nearest_neighbor = false;
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_state, rect, gfx::Rect(), rect, resource,
            premultiplied_alpha, uv_top_left, uv_bottom_right,
            background_color, vertex_opacity, flipped, nearest_neighbor);
    }

    void CreateTestYUVVideoDrawQuad_FromVideoFrame(
        const SharedQuadState* shared_state,
        scoped_refptr<media::VideoFrame> video_frame,
        uint8 alpha_value,
        const gfx::RectF& tex_coord_rect,
        RenderPass* render_pass,
        VideoResourceUpdater* video_resource_updater,
        const gfx::Rect& rect,
        const gfx::Rect& visible_rect,
        ResourceProvider* resource_provider)
    {
        const bool with_alpha = (video_frame->format() == media::PIXEL_FORMAT_YV12A);
        YUVVideoDrawQuad::ColorSpace color_space = YUVVideoDrawQuad::REC_601;
        int video_frame_color_space;
        if (video_frame->metadata()->GetInteger(
                media::VideoFrameMetadata::COLOR_SPACE, &video_frame_color_space)
            && video_frame_color_space == media::COLOR_SPACE_JPEG) {
            color_space = YUVVideoDrawQuad::JPEG;
        }

        const gfx::Rect opaque_rect(0, 0, 0, 0);

        if (with_alpha) {
            memset(video_frame->data(media::VideoFrame::kAPlane), alpha_value,
                video_frame->stride(media::VideoFrame::kAPlane) * video_frame->rows(media::VideoFrame::kAPlane));
        }

        VideoFrameExternalResources resources = video_resource_updater->CreateExternalResourcesFromVideoFrame(
            video_frame);

        EXPECT_EQ(VideoFrameExternalResources::YUV_RESOURCE, resources.type);
        EXPECT_EQ(media::VideoFrame::NumPlanes(video_frame->format()),
            resources.mailboxes.size());
        EXPECT_EQ(media::VideoFrame::NumPlanes(video_frame->format()),
            resources.release_callbacks.size());

        ResourceId y_resource = resource_provider->CreateResourceFromTextureMailbox(
            resources.mailboxes[media::VideoFrame::kYPlane],
            SingleReleaseCallbackImpl::Create(
                resources.release_callbacks[media::VideoFrame::kYPlane]));
        ResourceId u_resource = resource_provider->CreateResourceFromTextureMailbox(
            resources.mailboxes[media::VideoFrame::kUPlane],
            SingleReleaseCallbackImpl::Create(
                resources.release_callbacks[media::VideoFrame::kUPlane]));
        ResourceId v_resource = resource_provider->CreateResourceFromTextureMailbox(
            resources.mailboxes[media::VideoFrame::kVPlane],
            SingleReleaseCallbackImpl::Create(
                resources.release_callbacks[media::VideoFrame::kVPlane]));
        ResourceId a_resource = 0;
        if (with_alpha) {
            a_resource = resource_provider->CreateResourceFromTextureMailbox(
                resources.mailboxes[media::VideoFrame::kAPlane],
                SingleReleaseCallbackImpl::Create(
                    resources.release_callbacks[media::VideoFrame::kAPlane]));
        }

        const gfx::Size ya_tex_size = video_frame->coded_size();
        const gfx::Size uv_tex_size = media::VideoFrame::PlaneSize(
            video_frame->format(), media::VideoFrame::kUPlane,
            video_frame->coded_size());
        DCHECK(uv_tex_size == media::VideoFrame::PlaneSize(video_frame->format(), media::VideoFrame::kVPlane, video_frame->coded_size()));
        if (with_alpha) {
            DCHECK(ya_tex_size == media::VideoFrame::PlaneSize(video_frame->format(), media::VideoFrame::kAPlane, video_frame->coded_size()));
        }

        gfx::RectF ya_tex_coord_rect(tex_coord_rect.x() * ya_tex_size.width(),
            tex_coord_rect.y() * ya_tex_size.height(),
            tex_coord_rect.width() * ya_tex_size.width(),
            tex_coord_rect.height() * ya_tex_size.height());
        gfx::RectF uv_tex_coord_rect(tex_coord_rect.x() * uv_tex_size.width(),
            tex_coord_rect.y() * uv_tex_size.height(),
            tex_coord_rect.width() * uv_tex_size.width(),
            tex_coord_rect.height() * uv_tex_size.height());

        YUVVideoDrawQuad* yuv_quad = render_pass->CreateAndAppendDrawQuad<YUVVideoDrawQuad>();
        yuv_quad->SetNew(shared_state, rect, opaque_rect, visible_rect,
            ya_tex_coord_rect, uv_tex_coord_rect, ya_tex_size,
            uv_tex_size, y_resource, u_resource, v_resource, a_resource,
            color_space);
    }

    void CreateTestYUVVideoDrawQuad_Striped(
        const SharedQuadState* shared_state,
        media::VideoPixelFormat format,
        bool is_transparent,
        const gfx::RectF& tex_coord_rect,
        RenderPass* render_pass,
        VideoResourceUpdater* video_resource_updater,
        const gfx::Rect& rect,
        const gfx::Rect& visible_rect,
        ResourceProvider* resource_provider)
    {
        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
            format, rect.size(), rect, rect.size(), base::TimeDelta());

        // YUV values representing a striped pattern, for validating texture
        // coordinates for sampling.
        uint8_t y_value = 0;
        uint8_t u_value = 0;
        uint8_t v_value = 0;
        for (int i = 0; i < video_frame->rows(media::VideoFrame::kYPlane); ++i) {
            uint8_t* y_row = video_frame->data(media::VideoFrame::kYPlane) + video_frame->stride(media::VideoFrame::kYPlane) * i;
            for (int j = 0; j < video_frame->row_bytes(media::VideoFrame::kYPlane);
                 ++j) {
                y_row[j] = (y_value += 1);
            }
        }
        for (int i = 0; i < video_frame->rows(media::VideoFrame::kUPlane); ++i) {
            uint8_t* u_row = video_frame->data(media::VideoFrame::kUPlane) + video_frame->stride(media::VideoFrame::kUPlane) * i;
            uint8_t* v_row = video_frame->data(media::VideoFrame::kVPlane) + video_frame->stride(media::VideoFrame::kVPlane) * i;
            for (int j = 0; j < video_frame->row_bytes(media::VideoFrame::kUPlane);
                 ++j) {
                u_row[j] = (u_value += 3);
                v_row[j] = (v_value += 5);
            }
        }
        uint8 alpha_value = is_transparent ? 0 : 128;
        CreateTestYUVVideoDrawQuad_FromVideoFrame(
            shared_state, video_frame, alpha_value, tex_coord_rect, render_pass,
            video_resource_updater, rect, visible_rect, resource_provider);
    }

    // Creates a video frame of size background_size filled with yuv_background,
    // and then draws a foreground rectangle in a different color on top of
    // that. The foreground rectangle must have coordinates that are divisible
    // by 2 because YUV is a block format.
    void CreateTestYUVVideoDrawQuad_TwoColor(
        const SharedQuadState* shared_state,
        media::VideoPixelFormat format,
        media::ColorSpace color_space,
        bool is_transparent,
        const gfx::RectF& tex_coord_rect,
        const gfx::Size& background_size,
        const gfx::Rect& visible_rect,
        uint8 y_background,
        uint8 u_background,
        uint8 v_background,
        const gfx::Rect& foreground_rect,
        uint8 y_foreground,
        uint8 u_foreground,
        uint8 v_foreground,
        RenderPass* render_pass,
        VideoResourceUpdater* video_resource_updater,
        ResourceProvider* resource_provider)
    {
        const gfx::Rect rect(background_size);

        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(format, background_size, foreground_rect,
            foreground_rect.size(), base::TimeDelta());
        video_frame->metadata()->SetInteger(media::VideoFrameMetadata::COLOR_SPACE,
            color_space);

        int planes[] = { media::VideoFrame::kYPlane,
            media::VideoFrame::kUPlane,
            media::VideoFrame::kVPlane };
        uint8 yuv_background[] = { y_background, u_background, v_background };
        uint8 yuv_foreground[] = { y_foreground, u_foreground, v_foreground };
        int sample_size[] = { 1, 2, 2 };

        for (int i = 0; i < 3; ++i) {
            memset(video_frame->data(planes[i]), yuv_background[i],
                video_frame->stride(planes[i]) * video_frame->rows(planes[i]));
        }

        for (int i = 0; i < 3; ++i) {
            // Since yuv encoding uses block encoding, widths have to be divisible
            // by the sample size in order for this function to behave properly.
            DCHECK_EQ(foreground_rect.x() % sample_size[i], 0);
            DCHECK_EQ(foreground_rect.y() % sample_size[i], 0);
            DCHECK_EQ(foreground_rect.width() % sample_size[i], 0);
            DCHECK_EQ(foreground_rect.height() % sample_size[i], 0);

            gfx::Rect sample_rect(foreground_rect.x() / sample_size[i],
                foreground_rect.y() / sample_size[i],
                foreground_rect.width() / sample_size[i],
                foreground_rect.height() / sample_size[i]);
            for (int y = sample_rect.y(); y < sample_rect.bottom(); ++y) {
                for (int x = sample_rect.x(); x < sample_rect.right(); ++x) {
                    size_t offset = y * video_frame->stride(planes[i]) + x;
                    video_frame->data(planes[i])[offset] = yuv_foreground[i];
                }
            }
        }

        uint8 alpha_value = 255;
        CreateTestYUVVideoDrawQuad_FromVideoFrame(
            shared_state, video_frame, alpha_value, tex_coord_rect, render_pass,
            video_resource_updater, rect, visible_rect, resource_provider);
    }

    void CreateTestYUVVideoDrawQuad_Solid(
        const SharedQuadState* shared_state,
        media::VideoPixelFormat format,
        media::ColorSpace color_space,
        bool is_transparent,
        const gfx::RectF& tex_coord_rect,
        uint8 y,
        uint8 u,
        uint8 v,
        RenderPass* render_pass,
        VideoResourceUpdater* video_resource_updater,
        const gfx::Rect& rect,
        const gfx::Rect& visible_rect,
        ResourceProvider* resource_provider)
    {
        scoped_refptr<media::VideoFrame> video_frame = media::VideoFrame::CreateFrame(
            format, rect.size(), rect, rect.size(), base::TimeDelta());
        video_frame->metadata()->SetInteger(media::VideoFrameMetadata::COLOR_SPACE,
            color_space);

        // YUV values of a solid, constant, color. Useful for testing that color
        // space/color range are being handled properly.
        memset(video_frame->data(media::VideoFrame::kYPlane), y,
            video_frame->stride(media::VideoFrame::kYPlane) * video_frame->rows(media::VideoFrame::kYPlane));
        memset(video_frame->data(media::VideoFrame::kUPlane), u,
            video_frame->stride(media::VideoFrame::kUPlane) * video_frame->rows(media::VideoFrame::kUPlane));
        memset(video_frame->data(media::VideoFrame::kVPlane), v,
            video_frame->stride(media::VideoFrame::kVPlane) * video_frame->rows(media::VideoFrame::kVPlane));

        uint8 alpha_value = is_transparent ? 0 : 128;
        CreateTestYUVVideoDrawQuad_FromVideoFrame(
            shared_state, video_frame, alpha_value, tex_coord_rect, render_pass,
            video_resource_updater, rect, visible_rect, resource_provider);
    }

    typedef ::testing::Types<GLRenderer,
        SoftwareRenderer,
        GLRendererWithExpandedViewport,
        SoftwareRendererWithExpandedViewport>
        RendererTypes;
    TYPED_TEST_CASE(RendererPixelTest, RendererTypes);

    template <typename RendererType>
    class SoftwareRendererPixelTest : public RendererPixelTest<RendererType> {
    };

    typedef ::testing::Types<SoftwareRenderer, SoftwareRendererWithExpandedViewport>
        SoftwareRendererTypes;
    TYPED_TEST_CASE(SoftwareRendererPixelTest, SoftwareRendererTypes);

    template <typename RendererType>
    class FuzzyForSoftwareOnlyPixelComparator : public PixelComparator {
    public:
        explicit FuzzyForSoftwareOnlyPixelComparator(bool discard_alpha)
            : fuzzy_(discard_alpha)
            , exact_(discard_alpha)
        {
        }

        bool Compare(const SkBitmap& actual_bmp,
            const SkBitmap& expected_bmp) const override;

    private:
        FuzzyPixelOffByOneComparator fuzzy_;
        ExactPixelComparator exact_;
    };

    template <>
    bool FuzzyForSoftwareOnlyPixelComparator<SoftwareRenderer>::Compare(
        const SkBitmap& actual_bmp,
        const SkBitmap& expected_bmp) const
    {
        return fuzzy_.Compare(actual_bmp, expected_bmp);
    }

    template <>
    bool FuzzyForSoftwareOnlyPixelComparator<
        SoftwareRendererWithExpandedViewport>::Compare(const SkBitmap& actual_bmp,
        const SkBitmap& expected_bmp) const
    {
        return fuzzy_.Compare(actual_bmp, expected_bmp);
    }

    template <typename RendererType>
    bool FuzzyForSoftwareOnlyPixelComparator<RendererType>::Compare(
        const SkBitmap& actual_bmp,
        const SkBitmap& expected_bmp) const
    {
        return exact_.Compare(actual_bmp, expected_bmp);
    }

    TYPED_TEST(RendererPixelTest, SimpleGreenRect)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        SolidColorDrawQuad* color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(shared_state, rect, rect, SK_ColorGREEN, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("green.png")),
            ExactPixelComparator(true)));
    }

    TYPED_TEST(RendererPixelTest, SimpleGreenRect_NonRootRenderPass)
    {
        gfx::Rect rect(this->device_viewport_size_);
        gfx::Rect small_rect(100, 100);

        RenderPassId child_id(2, 1);
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_id, small_rect, gfx::Transform());

        SharedQuadState* child_shared_state = CreateTestSharedQuadState(gfx::Transform(), small_rect, child_pass.get());

        SolidColorDrawQuad* color_quad = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(child_shared_state, rect, rect, SK_ColorGREEN, false);

        RenderPassId root_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRenderPass(root_id, rect, gfx::Transform());

        SharedQuadState* root_shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, root_pass.get());

        CreateTestRenderPassDrawQuad(
            root_shared_state, small_rect, child_id, root_pass.get());

        RenderPass* child_pass_ptr = child_pass.get();

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        EXPECT_TRUE(this->RunPixelTestWithReadbackTarget(
            &pass_list,
            child_pass_ptr,
            base::FilePath(FILE_PATH_LITERAL("green_small.png")),
            ExactPixelComparator(true)));
    }

    TYPED_TEST(RendererPixelTest, PremultipliedTextureWithoutBackground)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        CreateTestTextureDrawQuad(gfx::Rect(this->device_viewport_size_),
            SkColorSetARGB(128, 0, 255, 0), // Texel color.
            SK_ColorTRANSPARENT, // Background color.
            true, // Premultiplied alpha.
            shared_state,
            this->resource_provider_.get(),
            pass.get());

        SolidColorDrawQuad* color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(shared_state, rect, rect, SK_ColorWHITE, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("green_alpha.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    TYPED_TEST(RendererPixelTest, PremultipliedTextureWithBackground)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* texture_quad_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());
        texture_quad_state->opacity = 0.8f;

        CreateTestTextureDrawQuad(gfx::Rect(this->device_viewport_size_),
            SkColorSetARGB(204, 120, 255, 120), // Texel color.
            SK_ColorGREEN, // Background color.
            true, // Premultiplied alpha.
            texture_quad_state,
            this->resource_provider_.get(),
            pass.get());

        SharedQuadState* color_quad_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());
        SolidColorDrawQuad* color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(color_quad_state, rect, rect, SK_ColorWHITE, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("green_alpha.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    template <typename QuadType>
    static const base::FilePath::CharType* IntersectingQuadImage()
    {
        return FILE_PATH_LITERAL("intersecting_blue_green_squares.png");
    }
    template <>
    const base::FilePath::CharType* IntersectingQuadImage<SolidColorDrawQuad>()
    {
        return FILE_PATH_LITERAL("intersecting_blue_green.png");
    }
    template <>
    const base::FilePath::CharType* IntersectingQuadImage<YUVVideoDrawQuad>()
    {
        return FILE_PATH_LITERAL("intersecting_blue_green_squares_video.png");
    }

    template <typename TypeParam>
    class IntersectingQuadPixelTest : public RendererPixelTest<TypeParam> {
    protected:
        void SetupQuadStateAndRenderPass()
        {
            // This sets up a pair of draw quads. They are both rotated
            // relative to the root plane, they are also rotated relative to each other.
            // The intersect in the middle at a non-perpendicular angle so that any
            // errors are hopefully magnified.
            // The quads should intersect correctly, as in the front quad should only
            // be partially in front of the back quad, and partially behind.

            viewport_rect_ = gfx::Rect(this->device_viewport_size_);
            quad_rect_ = gfx::Rect(0, 0, this->device_viewport_size_.width(),
                this->device_viewport_size_.height() / 2.0);

            RenderPassId id(1, 1);
            render_pass_ = CreateTestRootRenderPass(id, viewport_rect_);

            // Create the front quad rotated on the Z and Y axis.
            gfx::Transform trans;
            trans.Translate3d(0, 0, 0.707 * this->device_viewport_size_.width() / 2.0);
            trans.RotateAboutZAxis(45.0);
            trans.RotateAboutYAxis(45.0);
            front_quad_state_ = CreateTestSharedQuadState(trans, viewport_rect_, render_pass_.get());
            front_quad_state_->clip_rect = quad_rect_;
            // Make sure they end up in a 3d sorting context.
            front_quad_state_->sorting_context_id = 1;

            // Create the back quad, and rotate on just the y axis. This will intersect
            // the first quad partially.
            trans = gfx::Transform();
            trans.Translate3d(0, 0, -0.707 * this->device_viewport_size_.width() / 2.0);
            trans.RotateAboutYAxis(-45.0);
            back_quad_state_ = CreateTestSharedQuadState(trans, viewport_rect_, render_pass_.get());
            back_quad_state_->sorting_context_id = 1;
            back_quad_state_->clip_rect = quad_rect_;
        }
        template <typename T>
        void AppendBackgroundAndRunTest(const PixelComparator& comparator)
        {
            SharedQuadState* background_quad_state = CreateTestSharedQuadState(
                gfx::Transform(), viewport_rect_, render_pass_.get());
            SolidColorDrawQuad* background_quad = render_pass_->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            background_quad->SetNew(background_quad_state, viewport_rect_,
                viewport_rect_, SK_ColorWHITE, false);
            pass_list_.push_back(render_pass_.Pass());
            const base::FilePath::CharType* fileName = IntersectingQuadImage<T>();
            EXPECT_TRUE(
                this->RunPixelTest(&pass_list_, base::FilePath(fileName), comparator));
        }
        template <typename T>
        T* CreateAndAppendDrawQuad()
        {
            return render_pass_->CreateAndAppendDrawQuad<T>();
        }

        scoped_ptr<RenderPass> render_pass_;
        gfx::Rect viewport_rect_;
        SharedQuadState* front_quad_state_;
        SharedQuadState* back_quad_state_;
        gfx::Rect quad_rect_;
        RenderPassList pass_list_;
    };

    template <typename TypeParam>
    class IntersectingQuadGLPixelTest
        : public IntersectingQuadPixelTest<TypeParam> {
    public:
        void SetUp() override
        {
            IntersectingQuadPixelTest<TypeParam>::SetUp();
            video_resource_updater_.reset(
                new VideoResourceUpdater(this->output_surface_->context_provider(),
                    this->resource_provider_.get()));
            video_resource_updater2_.reset(
                new VideoResourceUpdater(this->output_surface_->context_provider(),
                    this->resource_provider_.get()));
        }

    protected:
        scoped_ptr<VideoResourceUpdater> video_resource_updater_;
        scoped_ptr<VideoResourceUpdater> video_resource_updater2_;
    };

    template <typename TypeParam>
    class IntersectingQuadSoftwareTest
        : public IntersectingQuadPixelTest<TypeParam> {
    };

    typedef ::testing::Types<SoftwareRenderer, SoftwareRendererWithExpandedViewport>
        SoftwareRendererTypes;
    typedef ::testing::Types<GLRenderer, GLRendererWithExpandedViewport>
        GLRendererTypes;

    TYPED_TEST_CASE(IntersectingQuadPixelTest, RendererTypes);
    TYPED_TEST_CASE(IntersectingQuadGLPixelTest, GLRendererTypes);
    TYPED_TEST_CASE(IntersectingQuadSoftwareTest, SoftwareRendererTypes);

    TYPED_TEST(IntersectingQuadPixelTest, SolidColorQuads)
    {
        this->SetupQuadStateAndRenderPass();

        SolidColorDrawQuad* quad = this->template CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        SolidColorDrawQuad* quad2 = this->template CreateAndAppendDrawQuad<SolidColorDrawQuad>();

        quad->SetNew(this->front_quad_state_, this->quad_rect_, this->quad_rect_,
            SK_ColorBLUE, false);
        quad2->SetNew(this->back_quad_state_, this->quad_rect_, this->quad_rect_,
            SK_ColorGREEN, false);
        SCOPED_TRACE("IntersectingSolidColorQuads");
        this->template AppendBackgroundAndRunTest<SolidColorDrawQuad>(
            FuzzyPixelComparator(false, 2.f, 0.f, 256.f, 256, 0.f));
    }

    template <typename TypeParam>
    SkColor GetColor(const SkColor& color)
    {
        return color;
    }

    template <>
    SkColor GetColor<GLRenderer>(const SkColor& color)
    {
        return SkColorSetARGB(SkColorGetA(color), SkColorGetB(color),
            SkColorGetG(color), SkColorGetR(color));
    }
    template <>
    SkColor GetColor<GLRendererWithExpandedViewport>(const SkColor& color)
    {
        return GetColor<GLRenderer>(color);
    }

    TYPED_TEST(IntersectingQuadPixelTest, TexturedQuads)
    {
        this->SetupQuadStateAndRenderPass();
        CreateTestTwoColoredTextureDrawQuad(
            this->quad_rect_, GetColor<TypeParam>(SkColorSetARGB(255, 0, 0, 0)),
            GetColor<TypeParam>(SkColorSetARGB(255, 0, 0, 255)), SK_ColorTRANSPARENT,
            true, this->front_quad_state_, this->resource_provider_.get(),
            this->render_pass_.get());
        CreateTestTwoColoredTextureDrawQuad(
            this->quad_rect_, GetColor<TypeParam>(SkColorSetARGB(255, 0, 255, 0)),
            GetColor<TypeParam>(SkColorSetARGB(255, 0, 0, 0)), SK_ColorTRANSPARENT,
            true, this->back_quad_state_, this->resource_provider_.get(),
            this->render_pass_.get());

        SCOPED_TRACE("IntersectingTexturedQuads");
        this->template AppendBackgroundAndRunTest<TextureDrawQuad>(
            FuzzyPixelComparator(false, 2.f, 0.f, 256.f, 256, 0.f));
    }

    TYPED_TEST(IntersectingQuadSoftwareTest, PictureQuads)
    {
        this->SetupQuadStateAndRenderPass();
        gfx::Rect outer_rect(this->quad_rect_);
        gfx::Rect inner_rect(this->quad_rect_.x() + (this->quad_rect_.width() / 4),
            this->quad_rect_.y() + (this->quad_rect_.height() / 4),
            this->quad_rect_.width() / 2,
            this->quad_rect_.height() / 2);

        SkPaint black_paint;
        black_paint.setColor(SK_ColorBLACK);
        SkPaint blue_paint;
        blue_paint.setColor(SK_ColorBLUE);
        SkPaint green_paint;
        green_paint.setColor(SK_ColorGREEN);

        scoped_ptr<FakeDisplayListRecordingSource> blue_recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            this->quad_rect_.size());
        blue_recording->add_draw_rect_with_paint(outer_rect, black_paint);
        blue_recording->add_draw_rect_with_paint(inner_rect, blue_paint);
        blue_recording->Rerecord();
        scoped_refptr<FakeDisplayListRasterSource> blue_raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(
            blue_recording.get(), false);

        PictureDrawQuad* blue_quad = this->render_pass_->template CreateAndAppendDrawQuad<PictureDrawQuad>();

        blue_quad->SetNew(this->front_quad_state_, this->quad_rect_, gfx::Rect(),
            this->quad_rect_, gfx::RectF(this->quad_rect_),
            this->quad_rect_.size(), false, RGBA_8888, this->quad_rect_,
            1.f, blue_raster_source);

        scoped_ptr<FakeDisplayListRecordingSource> green_recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            this->quad_rect_.size());
        green_recording->add_draw_rect_with_paint(outer_rect, green_paint);
        green_recording->add_draw_rect_with_paint(inner_rect, black_paint);
        green_recording->Rerecord();
        scoped_refptr<FakeDisplayListRasterSource> green_raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(
            green_recording.get(), false);

        PictureDrawQuad* green_quad = this->render_pass_->template CreateAndAppendDrawQuad<PictureDrawQuad>();
        green_quad->SetNew(this->back_quad_state_, this->quad_rect_, gfx::Rect(),
            this->quad_rect_, gfx::RectF(this->quad_rect_),
            this->quad_rect_.size(), false, RGBA_8888,
            this->quad_rect_, 1.f, green_raster_source);
        SCOPED_TRACE("IntersectingPictureQuadsPass");
        this->template AppendBackgroundAndRunTest<PictureDrawQuad>(
            FuzzyPixelComparator(false, 2.f, 0.f, 256.f, 256, 0.f));
    }

    TYPED_TEST(IntersectingQuadPixelTest, RenderPassQuads)
    {
        this->SetupQuadStateAndRenderPass();
        RenderPassId child_pass_id1(2, 2);
        RenderPassId child_pass_id2(2, 3);
        scoped_ptr<RenderPass> child_pass1 = CreateTestRenderPass(child_pass_id1, this->quad_rect_, gfx::Transform());
        SharedQuadState* child1_quad_state = CreateTestSharedQuadState(
            gfx::Transform(), this->quad_rect_, child_pass1.get());
        scoped_ptr<RenderPass> child_pass2 = CreateTestRenderPass(child_pass_id2, this->quad_rect_, gfx::Transform());
        SharedQuadState* child2_quad_state = CreateTestSharedQuadState(
            gfx::Transform(), this->quad_rect_, child_pass2.get());

        CreateTestTwoColoredTextureDrawQuad(
            this->quad_rect_, GetColor<TypeParam>(SkColorSetARGB(255, 0, 0, 0)),
            GetColor<TypeParam>(SkColorSetARGB(255, 0, 0, 255)), SK_ColorTRANSPARENT,
            true, child1_quad_state, this->resource_provider_.get(),
            child_pass1.get());
        CreateTestTwoColoredTextureDrawQuad(
            this->quad_rect_, GetColor<TypeParam>(SkColorSetARGB(255, 0, 255, 0)),
            GetColor<TypeParam>(SkColorSetARGB(255, 0, 0, 0)), SK_ColorTRANSPARENT,
            true, child2_quad_state, this->resource_provider_.get(),
            child_pass2.get());

        CreateTestRenderPassDrawQuad(this->front_quad_state_, this->quad_rect_,
            child_pass_id1, this->render_pass_.get());
        CreateTestRenderPassDrawQuad(this->back_quad_state_, this->quad_rect_,
            child_pass_id2, this->render_pass_.get());

        this->pass_list_.push_back(child_pass1.Pass());
        this->pass_list_.push_back(child_pass2.Pass());
        SCOPED_TRACE("IntersectingRenderQuadsPass");
        this->template AppendBackgroundAndRunTest<RenderPassDrawQuad>(
            FuzzyPixelComparator(false, 2.f, 0.f, 256.f, 256, 0.f));
    }

    TYPED_TEST(IntersectingQuadGLPixelTest, YUVVideoQuads)
    {
        this->SetupQuadStateAndRenderPass();
        gfx::Rect inner_rect(
            ((this->quad_rect_.x() + (this->quad_rect_.width() / 4)) & ~0xF),
            ((this->quad_rect_.y() + (this->quad_rect_.height() / 4)) & ~0xF),
            (this->quad_rect_.width() / 2) & ~0xF,
            (this->quad_rect_.height() / 2) & ~0xF);

        CreateTestYUVVideoDrawQuad_TwoColor(
            this->front_quad_state_, media::PIXEL_FORMAT_YV12,
            media::COLOR_SPACE_JPEG, false, gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f),
            this->quad_rect_.size(), this->quad_rect_, 0, 128, 128, inner_rect, 29,
            255, 107, this->render_pass_.get(), this->video_resource_updater_.get(),
            this->resource_provider_.get());

        CreateTestYUVVideoDrawQuad_TwoColor(
            this->back_quad_state_, media::PIXEL_FORMAT_YV12, media::COLOR_SPACE_JPEG,
            false, gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f), this->quad_rect_.size(),
            this->quad_rect_, 149, 43, 21, inner_rect, 0, 128, 128,
            this->render_pass_.get(), this->video_resource_updater2_.get(),
            this->resource_provider_.get());

        SCOPED_TRACE("IntersectingVideoQuads");
        this->template AppendBackgroundAndRunTest<YUVVideoDrawQuad>(
            FuzzyPixelOffByOneComparator(false));
    }

    // TODO(skaslev): The software renderer does not support non-premultplied alpha.
    TEST_F(GLRendererPixelTest, NonPremultipliedTextureWithoutBackground)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        CreateTestTextureDrawQuad(gfx::Rect(this->device_viewport_size_),
            SkColorSetARGB(128, 0, 255, 0), // Texel color.
            SK_ColorTRANSPARENT, // Background color.
            false, // Premultiplied alpha.
            shared_state,
            this->resource_provider_.get(),
            pass.get());

        SolidColorDrawQuad* color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(shared_state, rect, rect, SK_ColorWHITE, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("green_alpha.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    // TODO(skaslev): The software renderer does not support non-premultplied alpha.
    TEST_F(GLRendererPixelTest, NonPremultipliedTextureWithBackground)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* texture_quad_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());
        texture_quad_state->opacity = 0.8f;

        CreateTestTextureDrawQuad(gfx::Rect(this->device_viewport_size_),
            SkColorSetARGB(204, 120, 255, 120), // Texel color.
            SK_ColorGREEN, // Background color.
            false, // Premultiplied alpha.
            texture_quad_state,
            this->resource_provider_.get(),
            pass.get());

        SharedQuadState* color_quad_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());
        SolidColorDrawQuad* color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(color_quad_state, rect, rect, SK_ColorWHITE, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("green_alpha.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    class VideoGLRendererPixelTest : public GLRendererPixelTest {
    protected:
        void CreateEdgeBleedPass(media::VideoPixelFormat format,
            media::ColorSpace color_space,
            RenderPassList* pass_list)
        {
            gfx::Rect rect(200, 200);

            RenderPassId id(1, 1);
            scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

            // Scale the video up so that bilinear filtering kicks in to sample more
            // than just nearest neighbor would.
            gfx::Transform scale_by_2;
            scale_by_2.Scale(2.f, 2.f);
            gfx::Rect half_rect(100, 100);
            SharedQuadState* shared_state = CreateTestSharedQuadState(scale_by_2, half_rect, pass.get());

            gfx::Size background_size(200, 200);
            gfx::Rect green_rect(16, 20, 100, 100);
            gfx::RectF tex_coord_rect(
                static_cast<float>(green_rect.x()) / background_size.width(),
                static_cast<float>(green_rect.y()) / background_size.height(),
                static_cast<float>(green_rect.width()) / background_size.width(),
                static_cast<float>(green_rect.height()) / background_size.height());

            // YUV of (149,43,21) should be green (0,255,0) in RGB.
            // Create a video frame that has a non-green background rect, with a
            // green sub-rectangle that should be the only thing displayed in
            // the final image.  Bleeding will appear on all four sides of the video
            // if the tex coords are not clamped.
            CreateTestYUVVideoDrawQuad_TwoColor(
                shared_state, format, color_space, false, tex_coord_rect,
                background_size, gfx::Rect(background_size), 128, 128, 128, green_rect,
                149, 43, 21, pass.get(), video_resource_updater_.get(),
                resource_provider_.get());
            pass_list->push_back(pass.Pass());
        }

        void SetUp() override
        {
            GLRendererPixelTest::SetUp();
            video_resource_updater_.reset(new VideoResourceUpdater(
                output_surface_->context_provider(), resource_provider_.get()));
        }

        scoped_ptr<VideoResourceUpdater> video_resource_updater_;
    };

    TEST_F(VideoGLRendererPixelTest, SimpleYUVRect)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        CreateTestYUVVideoDrawQuad_Striped(shared_state, media::PIXEL_FORMAT_YV12,
            false, gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f),
            pass.get(), video_resource_updater_.get(),
            rect, rect, resource_provider_.get());

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(
            this->RunPixelTest(&pass_list,
                base::FilePath(FILE_PATH_LITERAL("yuv_stripes.png")),
                FuzzyPixelOffByOneComparator(true)));
    }

    TEST_F(VideoGLRendererPixelTest, ClippedYUVRect)
    {
        gfx::Rect viewport(this->device_viewport_size_);
        gfx::Rect draw_rect(this->device_viewport_size_.width() * 1.5,
            this->device_viewport_size_.height() * 1.5);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, viewport);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), viewport, pass.get());

        CreateTestYUVVideoDrawQuad_Striped(shared_state, media::PIXEL_FORMAT_YV12,
            false, gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f),
            pass.get(), video_resource_updater_.get(),
            draw_rect, viewport,
            resource_provider_.get());
        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list, base::FilePath(FILE_PATH_LITERAL("yuv_stripes_clipped.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    TEST_F(VideoGLRendererPixelTest, OffsetYUVRect)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        // Intentionally sets frame format to I420 for testing coverage.
        CreateTestYUVVideoDrawQuad_Striped(
            shared_state, media::PIXEL_FORMAT_I420, false,
            gfx::RectF(0.125f, 0.25f, 0.75f, 0.5f), pass.get(),
            video_resource_updater_.get(), rect, rect, resource_provider_.get());

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("yuv_stripes_offset.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    TEST_F(VideoGLRendererPixelTest, SimpleYUVRectBlack)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        // In MPEG color range YUV values of (15,128,128) should produce black.
        CreateTestYUVVideoDrawQuad_Solid(
            shared_state, media::PIXEL_FORMAT_YV12, media::COLOR_SPACE_UNSPECIFIED,
            false, gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f), 15, 128, 128, pass.get(),
            video_resource_updater_.get(), rect, rect, resource_provider_.get());

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        // If we didn't get black out of the YUV values above, then we probably have a
        // color range issue.
        EXPECT_TRUE(this->RunPixelTest(&pass_list,
            base::FilePath(FILE_PATH_LITERAL("black.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    TEST_F(VideoGLRendererPixelTest, SimpleYUVJRect)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        // YUV of (149,43,21) should be green (0,255,0) in RGB.
        CreateTestYUVVideoDrawQuad_Solid(
            shared_state, media::PIXEL_FORMAT_YV12, media::COLOR_SPACE_JPEG, false,
            gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f), 149, 43, 21, pass.get(),
            video_resource_updater_.get(), rect, rect, resource_provider_.get());

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(&pass_list,
            base::FilePath(FILE_PATH_LITERAL("green.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    // Test that a YUV video doesn't bleed outside of its tex coords when the
    // tex coord rect is only a partial subrectangle of the coded contents.
    TEST_F(VideoGLRendererPixelTest, YUVEdgeBleed)
    {
        RenderPassList pass_list;
        CreateEdgeBleedPass(media::PIXEL_FORMAT_YV12, media::COLOR_SPACE_JPEG,
            &pass_list);
        EXPECT_TRUE(this->RunPixelTest(&pass_list,
            base::FilePath(FILE_PATH_LITERAL("green.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    TEST_F(VideoGLRendererPixelTest, YUVAEdgeBleed)
    {
        RenderPassList pass_list;
        CreateEdgeBleedPass(media::PIXEL_FORMAT_YV12A, media::COLOR_SPACE_UNSPECIFIED,
            &pass_list);
        EXPECT_TRUE(this->RunPixelTest(&pass_list,
            base::FilePath(FILE_PATH_LITERAL("green.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    TEST_F(VideoGLRendererPixelTest, SimpleYUVJRectGrey)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        // Dark grey in JPEG color range (in MPEG, this is black).
        CreateTestYUVVideoDrawQuad_Solid(
            shared_state, media::PIXEL_FORMAT_YV12, media::COLOR_SPACE_JPEG, false,
            gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f), 15, 128, 128, pass.get(),
            video_resource_updater_.get(), rect, rect, resource_provider_.get());

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(
            this->RunPixelTest(&pass_list,
                base::FilePath(FILE_PATH_LITERAL("dark_grey.png")),
                FuzzyPixelOffByOneComparator(true)));
    }

    TEST_F(VideoGLRendererPixelTest, SimpleYUVARect)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        CreateTestYUVVideoDrawQuad_Striped(shared_state, media::PIXEL_FORMAT_YV12A,
            false, gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f),
            pass.get(), video_resource_updater_.get(),
            rect, rect, resource_provider_.get());

        SolidColorDrawQuad* color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(shared_state, rect, rect, SK_ColorWHITE, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("yuv_stripes_alpha.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    TEST_F(VideoGLRendererPixelTest, FullyTransparentYUVARect)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        CreateTestYUVVideoDrawQuad_Striped(shared_state, media::PIXEL_FORMAT_YV12A,
            true, gfx::RectF(0.0f, 0.0f, 1.0f, 1.0f),
            pass.get(), video_resource_updater_.get(),
            rect, rect, resource_provider_.get());

        SolidColorDrawQuad* color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        color_quad->SetNew(shared_state, rect, rect, SK_ColorBLACK, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("black.png")),
            ExactPixelComparator(true)));
    }

    TYPED_TEST(RendererPixelTest, FastPassColorFilterAlpha)
    {
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());
        shared_state->opacity = 0.5f;

        gfx::Rect blue_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);
        gfx::Rect yellow_rect(0,
            this->device_viewport_size_.height() / 2,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* yellow = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(shared_state, yellow_rect, yellow_rect, SK_ColorYELLOW, false);

        SharedQuadState* blank_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        SolidColorDrawQuad* white = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        white->SetNew(
            blank_state, viewport_rect, viewport_rect, SK_ColorWHITE, false);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(gfx::Transform(), pass_rect, root_pass.get());

        SkScalar matrix[20];
        float amount = 0.5f;
        matrix[0] = 0.213f + 0.787f * amount;
        matrix[1] = 0.715f - 0.715f * amount;
        matrix[2] = 1.f - (matrix[0] + matrix[1]);
        matrix[3] = matrix[4] = 0;
        matrix[5] = 0.213f - 0.213f * amount;
        matrix[6] = 0.715f + 0.285f * amount;
        matrix[7] = 1.f - (matrix[5] + matrix[6]);
        matrix[8] = matrix[9] = 0;
        matrix[10] = 0.213f - 0.213f * amount;
        matrix[11] = 0.715f - 0.715f * amount;
        matrix[12] = 1.f - (matrix[10] + matrix[11]);
        matrix[13] = matrix[14] = 0;
        matrix[15] = matrix[16] = matrix[17] = matrix[19] = 0;
        matrix[18] = 1;
        skia::RefPtr<SkColorFilter> colorFilter(
            skia::AdoptRef(SkColorMatrixFilter::Create(matrix)));
        skia::RefPtr<SkImageFilter> filter = skia::AdoptRef(SkColorFilterImageFilter::Create(colorFilter.get(), NULL));
        FilterOperations filters;
        filters.Append(FilterOperation::CreateReferenceFilter(filter));

        RenderPassDrawQuad* render_pass_quad = root_pass->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        render_pass_quad->SetNew(pass_shared_state,
            pass_rect,
            pass_rect,
            child_pass_id,
            0,
            gfx::Vector2dF(),
            gfx::Size(),
            filters,
            gfx::Vector2dF(),
            FilterOperations());

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        // This test has alpha=254 for the software renderer vs. alpha=255 for the gl
        // renderer so use a fuzzy comparator.
        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("blue_yellow_alpha.png")),
            FuzzyForSoftwareOnlyPixelComparator<TypeParam>(false)));
    }

    TYPED_TEST(RendererPixelTest, FastPassSaturateFilter)
    {
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());
        shared_state->opacity = 0.5f;

        gfx::Rect blue_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);
        gfx::Rect yellow_rect(0,
            this->device_viewport_size_.height() / 2,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* yellow = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(shared_state, yellow_rect, yellow_rect, SK_ColorYELLOW, false);

        SharedQuadState* blank_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        SolidColorDrawQuad* white = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        white->SetNew(
            blank_state, viewport_rect, viewport_rect, SK_ColorWHITE, false);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(gfx::Transform(), pass_rect, root_pass.get());

        FilterOperations filters;
        filters.Append(FilterOperation::CreateSaturateFilter(0.5f));

        RenderPassDrawQuad* render_pass_quad = root_pass->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        render_pass_quad->SetNew(pass_shared_state,
            pass_rect,
            pass_rect,
            child_pass_id,
            0,
            gfx::Vector2dF(),
            gfx::Size(),
            filters,
            gfx::Vector2dF(),
            FilterOperations());

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        // This test blends slightly differently with the software renderer vs. the gl
        // renderer so use a fuzzy comparator.
        EXPECT_TRUE(this->RunPixelTest(
            &pass_list, base::FilePath(FILE_PATH_LITERAL("blue_yellow_alpha.png")),
            FuzzyForSoftwareOnlyPixelComparator<TypeParam>(false)));
    }

    TYPED_TEST(RendererPixelTest, FastPassFilterChain)
    {
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());
        shared_state->opacity = 0.5f;

        gfx::Rect blue_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);
        gfx::Rect yellow_rect(0,
            this->device_viewport_size_.height() / 2,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* yellow = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(shared_state, yellow_rect, yellow_rect, SK_ColorYELLOW, false);

        SharedQuadState* blank_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        SolidColorDrawQuad* white = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        white->SetNew(
            blank_state, viewport_rect, viewport_rect, SK_ColorWHITE, false);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(gfx::Transform(), pass_rect, root_pass.get());

        FilterOperations filters;
        filters.Append(FilterOperation::CreateGrayscaleFilter(1.f));
        filters.Append(FilterOperation::CreateBrightnessFilter(0.5f));

        RenderPassDrawQuad* render_pass_quad = root_pass->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        render_pass_quad->SetNew(pass_shared_state,
            pass_rect,
            pass_rect,
            child_pass_id,
            0,
            gfx::Vector2dF(),
            gfx::Size(),
            filters,
            gfx::Vector2dF(),
            FilterOperations());

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        // This test blends slightly differently with the software renderer vs. the gl
        // renderer so use a fuzzy comparator.
        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("blue_yellow_filter_chain.png")),
            FuzzyForSoftwareOnlyPixelComparator<TypeParam>(false)));
    }

    TYPED_TEST(RendererPixelTest, FastPassColorFilterAlphaTranslation)
    {
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());
        shared_state->opacity = 0.5f;

        gfx::Rect blue_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);
        gfx::Rect yellow_rect(0,
            this->device_viewport_size_.height() / 2,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* yellow = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(shared_state, yellow_rect, yellow_rect, SK_ColorYELLOW, false);

        SharedQuadState* blank_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        SolidColorDrawQuad* white = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        white->SetNew(
            blank_state, viewport_rect, viewport_rect, SK_ColorWHITE, false);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(gfx::Transform(), pass_rect, root_pass.get());

        SkScalar matrix[20];
        float amount = 0.5f;
        matrix[0] = 0.213f + 0.787f * amount;
        matrix[1] = 0.715f - 0.715f * amount;
        matrix[2] = 1.f - (matrix[0] + matrix[1]);
        matrix[3] = 0;
        matrix[4] = 20.f;
        matrix[5] = 0.213f - 0.213f * amount;
        matrix[6] = 0.715f + 0.285f * amount;
        matrix[7] = 1.f - (matrix[5] + matrix[6]);
        matrix[8] = 0;
        matrix[9] = 200.f;
        matrix[10] = 0.213f - 0.213f * amount;
        matrix[11] = 0.715f - 0.715f * amount;
        matrix[12] = 1.f - (matrix[10] + matrix[11]);
        matrix[13] = 0;
        matrix[14] = 1.5f;
        matrix[15] = matrix[16] = matrix[17] = matrix[19] = 0;
        matrix[18] = 1;
        skia::RefPtr<SkColorFilter> colorFilter(
            skia::AdoptRef(SkColorMatrixFilter::Create(matrix)));
        skia::RefPtr<SkImageFilter> filter = skia::AdoptRef(SkColorFilterImageFilter::Create(colorFilter.get(), NULL));
        FilterOperations filters;
        filters.Append(FilterOperation::CreateReferenceFilter(filter));

        RenderPassDrawQuad* render_pass_quad = root_pass->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        render_pass_quad->SetNew(pass_shared_state,
            pass_rect,
            pass_rect,
            child_pass_id,
            0,
            gfx::Vector2dF(),
            gfx::Size(),
            filters,
            gfx::Vector2dF(),
            FilterOperations());

        RenderPassList pass_list;

        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        // This test has alpha=254 for the software renderer vs. alpha=255 for the gl
        // renderer so use a fuzzy comparator.
        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("blue_yellow_alpha_translate.png")),
            FuzzyForSoftwareOnlyPixelComparator<TypeParam>(false)));
    }

    TYPED_TEST(RendererPixelTest, EnlargedRenderPassTexture)
    {
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        gfx::Rect blue_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);
        gfx::Rect yellow_rect(0,
            this->device_viewport_size_.height() / 2,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* yellow = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(shared_state, yellow_rect, yellow_rect, SK_ColorYELLOW, false);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(gfx::Transform(), pass_rect, root_pass.get());
        CreateTestRenderPassDrawQuad(
            pass_shared_state, pass_rect, child_pass_id, root_pass.get());

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        this->renderer_->SetEnlargePassTextureAmountForTesting(gfx::Vector2d(50, 75));

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("blue_yellow.png")),
            ExactPixelComparator(true)));
    }

    TYPED_TEST(RendererPixelTest, EnlargedRenderPassTextureWithAntiAliasing)
    {
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        gfx::Rect blue_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);
        gfx::Rect yellow_rect(0,
            this->device_viewport_size_.height() / 2,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* yellow = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(shared_state, yellow_rect, yellow_rect, SK_ColorYELLOW, false);

        gfx::Transform aa_transform;
        aa_transform.Translate(0.5, 0.0);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(aa_transform, pass_rect, root_pass.get());
        CreateTestRenderPassDrawQuad(
            pass_shared_state, pass_rect, child_pass_id, root_pass.get());

        SharedQuadState* root_shared_state = CreateTestSharedQuadState(
            gfx::Transform(), viewport_rect, root_pass.get());
        SolidColorDrawQuad* background = root_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        background->SetNew(root_shared_state,
            gfx::Rect(this->device_viewport_size_),
            gfx::Rect(this->device_viewport_size_),
            SK_ColorWHITE,
            false);

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        this->renderer_->SetEnlargePassTextureAmountForTesting(gfx::Vector2d(50, 75));

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("blue_yellow_anti_aliasing.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    // This tests the case where we have a RenderPass with a mask, but the quad
    // for the masked surface does not include the full surface texture.
    TYPED_TEST(RendererPixelTest, RenderPassAndMaskWithPartialQuad)
    {
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);
        SharedQuadState* root_pass_shared_state = CreateTestSharedQuadState(
            gfx::Transform(), viewport_rect, root_pass.get());

        RenderPassId child_pass_id(2, 2);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, viewport_rect, transform_to_root);
        SharedQuadState* child_pass_shared_state = CreateTestSharedQuadState(
            gfx::Transform(), viewport_rect, child_pass.get());

        // The child render pass is just a green box.
        static const SkColor kCSSGreen = 0xff008000;
        SolidColorDrawQuad* green = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        green->SetNew(
            child_pass_shared_state, viewport_rect, viewport_rect, kCSSGreen, false);

        // Make a mask.
        gfx::Rect mask_rect = viewport_rect;
        SkBitmap bitmap;
        bitmap.allocPixels(
            SkImageInfo::MakeN32Premul(mask_rect.width(), mask_rect.height()));
        SkCanvas canvas(bitmap);
        SkPaint paint;
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(SkIntToScalar(4));
        paint.setColor(SK_ColorWHITE);
        canvas.clear(SK_ColorTRANSPARENT);
        gfx::Rect rect = mask_rect;
        while (!rect.IsEmpty()) {
            rect.Inset(6, 6, 4, 4);
            canvas.drawRect(
                SkRect::MakeXYWH(rect.x(), rect.y(), rect.width(), rect.height()),
                paint);
            rect.Inset(6, 6, 4, 4);
        }

        ResourceId mask_resource_id = this->resource_provider_->CreateResource(
            mask_rect.size(), GL_CLAMP_TO_EDGE,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE, RGBA_8888);
        {
            SkAutoLockPixels lock(bitmap);
            this->resource_provider_->CopyToResource(
                mask_resource_id, reinterpret_cast<uint8_t*>(bitmap.getPixels()),
                mask_rect.size());
        }

        // This RenderPassDrawQuad does not include the full |viewport_rect| which is
        // the size of the child render pass.
        gfx::Rect sub_rect = gfx::Rect(50, 50, 200, 100);
        EXPECT_NE(sub_rect.x(), child_pass->output_rect.x());
        EXPECT_NE(sub_rect.y(), child_pass->output_rect.y());
        EXPECT_NE(sub_rect.right(), child_pass->output_rect.right());
        EXPECT_NE(sub_rect.bottom(), child_pass->output_rect.bottom());

        // Set up a mask on the RenderPassDrawQuad.
        RenderPassDrawQuad* mask_quad = root_pass->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        mask_quad->SetNew(root_pass_shared_state,
            sub_rect,
            sub_rect,
            child_pass_id,
            mask_resource_id,
            gfx::Vector2dF(2.f, 1.f), // mask_uv_scale
            gfx::Size(mask_rect.size()), // mask_texture_size
            FilterOperations(), // foreground filters
            gfx::Vector2dF(), // filters scale
            FilterOperations()); // background filters

        // White background behind the masked render pass.
        SolidColorDrawQuad* white = root_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        white->SetNew(root_pass_shared_state,
            viewport_rect,
            viewport_rect,
            SK_ColorWHITE,
            false);

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("mask_bottom_right.png")),
            ExactPixelComparator(true)));
    }

    template <typename RendererType>
    class RendererPixelTestWithBackgroundFilter
        : public RendererPixelTest<RendererType> {
    protected:
        void SetUpRenderPassList()
        {
            gfx::Rect device_viewport_rect(this->device_viewport_size_);

            RenderPassId root_id(1, 1);
            scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_id, device_viewport_rect);
            root_pass->has_transparent_background = false;

            gfx::Transform identity_quad_to_target_transform;

            RenderPassId filter_pass_id(2, 1);
            gfx::Transform transform_to_root;
            scoped_ptr<RenderPass> filter_pass = CreateTestRenderPass(
                filter_pass_id, filter_pass_layer_rect_, transform_to_root);

            // A non-visible quad in the filtering render pass.
            {
                SharedQuadState* shared_state = CreateTestSharedQuadState(identity_quad_to_target_transform,
                    filter_pass_layer_rect_, filter_pass.get());
                SolidColorDrawQuad* color_quad = filter_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
                color_quad->SetNew(shared_state, filter_pass_layer_rect_,
                    filter_pass_layer_rect_, SK_ColorTRANSPARENT, false);
            }

            {
                SharedQuadState* shared_state = CreateTestSharedQuadState(filter_pass_to_target_transform_,
                    filter_pass_layer_rect_, filter_pass.get());
                RenderPassDrawQuad* filter_pass_quad = root_pass->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
                filter_pass_quad->SetNew(shared_state, filter_pass_layer_rect_,
                    filter_pass_layer_rect_, filter_pass_id,
                    0, // mask_resource_id
                    gfx::Vector2dF(), // mask_uv_scale
                    gfx::Size(), // mask_texture_size
                    FilterOperations(), // filters
                    gfx::Vector2dF(), // filters_scale
                    this->background_filters_);
            }

            const int kColumnWidth = device_viewport_rect.width() / 3;

            gfx::Rect left_rect = gfx::Rect(0, 0, kColumnWidth, 20);
            for (int i = 0; left_rect.y() < device_viewport_rect.height(); ++i) {
                SharedQuadState* shared_state = CreateTestSharedQuadState(
                    identity_quad_to_target_transform, left_rect, root_pass.get());
                SolidColorDrawQuad* color_quad = root_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
                color_quad->SetNew(
                    shared_state, left_rect, left_rect, SK_ColorGREEN, false);
                left_rect += gfx::Vector2d(0, left_rect.height() + 1);
            }

            gfx::Rect middle_rect = gfx::Rect(kColumnWidth + 1, 0, kColumnWidth, 20);
            for (int i = 0; middle_rect.y() < device_viewport_rect.height(); ++i) {
                SharedQuadState* shared_state = CreateTestSharedQuadState(
                    identity_quad_to_target_transform, middle_rect, root_pass.get());
                SolidColorDrawQuad* color_quad = root_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
                color_quad->SetNew(
                    shared_state, middle_rect, middle_rect, SK_ColorRED, false);
                middle_rect += gfx::Vector2d(0, middle_rect.height() + 1);
            }

            gfx::Rect right_rect = gfx::Rect((kColumnWidth + 1) * 2, 0, kColumnWidth, 20);
            for (int i = 0; right_rect.y() < device_viewport_rect.height(); ++i) {
                SharedQuadState* shared_state = CreateTestSharedQuadState(
                    identity_quad_to_target_transform, right_rect, root_pass.get());
                SolidColorDrawQuad* color_quad = root_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
                color_quad->SetNew(
                    shared_state, right_rect, right_rect, SK_ColorBLUE, false);
                right_rect += gfx::Vector2d(0, right_rect.height() + 1);
            }

            SharedQuadState* shared_state = CreateTestSharedQuadState(identity_quad_to_target_transform,
                device_viewport_rect, root_pass.get());
            SolidColorDrawQuad* background_quad = root_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            background_quad->SetNew(shared_state,
                device_viewport_rect,
                device_viewport_rect,
                SK_ColorWHITE,
                false);

            pass_list_.push_back(filter_pass.Pass());
            pass_list_.push_back(root_pass.Pass());
        }

        RenderPassList pass_list_;
        FilterOperations background_filters_;
        gfx::Transform filter_pass_to_target_transform_;
        gfx::Rect filter_pass_layer_rect_;
    };

    typedef ::testing::Types<GLRenderer, SoftwareRenderer>
        BackgroundFilterRendererTypes;
    TYPED_TEST_CASE(RendererPixelTestWithBackgroundFilter,
        BackgroundFilterRendererTypes);

    typedef RendererPixelTestWithBackgroundFilter<GLRenderer>
        GLRendererPixelTestWithBackgroundFilter;

    // TODO(skaslev): The software renderer does not support filters yet.
    TEST_F(GLRendererPixelTestWithBackgroundFilter, InvertFilter)
    {
        this->background_filters_.Append(
            FilterOperation::CreateInvertFilter(1.f));

        this->filter_pass_layer_rect_ = gfx::Rect(this->device_viewport_size_);
        this->filter_pass_layer_rect_.Inset(12, 14, 16, 18);

        this->SetUpRenderPassList();
        EXPECT_TRUE(this->RunPixelTest(
            &this->pass_list_,
            base::FilePath(FILE_PATH_LITERAL("background_filter.png")),
            ExactPixelComparator(true)));
    }

    class ExternalStencilPixelTest : public GLRendererPixelTest {
    protected:
        void ClearBackgroundToGreen()
        {
            GLES2Interface* gl = output_surface_->context_provider()->ContextGL();
            output_surface_->EnsureBackbuffer();
            output_surface_->Reshape(device_viewport_size_, 1);
            gl->ClearColor(0.f, 1.f, 0.f, 1.f);
            gl->Clear(GL_COLOR_BUFFER_BIT);
        }

        void PopulateStencilBuffer()
        {
            // Set two quadrants of the stencil buffer to 1.
            GLES2Interface* gl = output_surface_->context_provider()->ContextGL();
            output_surface_->EnsureBackbuffer();
            output_surface_->Reshape(device_viewport_size_, 1);
            gl->ClearStencil(0);
            gl->Clear(GL_STENCIL_BUFFER_BIT);
            gl->Enable(GL_SCISSOR_TEST);
            gl->ClearStencil(1);
            gl->Scissor(0,
                0,
                device_viewport_size_.width() / 2,
                device_viewport_size_.height() / 2);
            gl->Clear(GL_STENCIL_BUFFER_BIT);
            gl->Scissor(device_viewport_size_.width() / 2,
                device_viewport_size_.height() / 2,
                device_viewport_size_.width(),
                device_viewport_size_.height());
            gl->Clear(GL_STENCIL_BUFFER_BIT);
        }
    };

    TEST_F(ExternalStencilPixelTest, StencilTestEnabled)
    {
        ClearBackgroundToGreen();
        PopulateStencilBuffer();
        this->EnableExternalStencilTest();

        // Draw a blue quad that covers the entire device viewport. It should be
        // clipped to the bottom left and top right corners by the external stencil.
        gfx::Rect rect(this->device_viewport_size_);
        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);
        SharedQuadState* blue_shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());
        SolidColorDrawQuad* blue = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(blue_shared_state, rect, rect, SK_ColorBLUE, false);
        pass->has_transparent_background = false;
        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("four_blue_green_checkers.png")),
            ExactPixelComparator(true)));
    }

    TEST_F(ExternalStencilPixelTest, StencilTestDisabled)
    {
        PopulateStencilBuffer();

        // Draw a green quad that covers the entire device viewport. The stencil
        // buffer should be ignored.
        gfx::Rect rect(this->device_viewport_size_);
        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);
        SharedQuadState* green_shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());
        SolidColorDrawQuad* green = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        green->SetNew(green_shared_state, rect, rect, SK_ColorGREEN, false);
        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("green.png")),
            ExactPixelComparator(true)));
    }

    TEST_F(ExternalStencilPixelTest, RenderSurfacesIgnoreStencil)
    {
        // The stencil test should apply only to the final render pass.
        ClearBackgroundToGreen();
        PopulateStencilBuffer();
        this->EnableExternalStencilTest();

        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);
        root_pass->has_transparent_background = false;

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        gfx::Rect blue_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height());
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(gfx::Transform(), pass_rect, root_pass.get());
        CreateTestRenderPassDrawQuad(
            pass_shared_state, pass_rect, child_pass_id, root_pass.get());
        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("four_blue_green_checkers.png")),
            ExactPixelComparator(true)));
    }

    TEST_F(ExternalStencilPixelTest, DeviceClip)
    {
        ClearBackgroundToGreen();
        gfx::Rect clip_rect(gfx::Point(150, 150), gfx::Size(50, 50));
        this->ForceDeviceClip(clip_rect);

        // Draw a blue quad that covers the entire device viewport. It should be
        // clipped to the bottom right corner by the device clip.
        gfx::Rect rect(this->device_viewport_size_);
        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);
        SharedQuadState* blue_shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());
        SolidColorDrawQuad* blue = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(blue_shared_state, rect, rect, SK_ColorBLUE, false);
        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("green_with_blue_corner.png")),
            ExactPixelComparator(true)));
    }

    // Software renderer does not support anti-aliased edges.
    TEST_F(GLRendererPixelTest, AntiAliasing)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        gfx::Transform red_quad_to_target_transform;
        red_quad_to_target_transform.Rotate(10);
        SharedQuadState* red_shared_state = CreateTestSharedQuadState(red_quad_to_target_transform, rect, pass.get());

        SolidColorDrawQuad* red = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        red->SetNew(red_shared_state, rect, rect, SK_ColorRED, false);

        gfx::Transform yellow_quad_to_target_transform;
        yellow_quad_to_target_transform.Rotate(5);
        SharedQuadState* yellow_shared_state = CreateTestSharedQuadState(
            yellow_quad_to_target_transform, rect, pass.get());

        SolidColorDrawQuad* yellow = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(yellow_shared_state, rect, rect, SK_ColorYELLOW, false);

        gfx::Transform blue_quad_to_target_transform;
        SharedQuadState* blue_shared_state = CreateTestSharedQuadState(
            blue_quad_to_target_transform, rect, pass.get());

        SolidColorDrawQuad* blue = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(blue_shared_state, rect, rect, SK_ColorBLUE, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("anti_aliasing.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    // This test tests that anti-aliasing works for axis aligned quads.
    // Anti-aliasing is only supported in the gl renderer.
    TEST_F(GLRendererPixelTest, AxisAligned)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, rect, transform_to_root);

        gfx::Transform red_quad_to_target_transform;
        red_quad_to_target_transform.Translate(50, 50);
        red_quad_to_target_transform.Scale(0.5f + 1.0f / (rect.width() * 2.0f),
            0.5f + 1.0f / (rect.height() * 2.0f));
        SharedQuadState* red_shared_state = CreateTestSharedQuadState(red_quad_to_target_transform, rect, pass.get());

        SolidColorDrawQuad* red = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        red->SetNew(red_shared_state, rect, rect, SK_ColorRED, false);

        gfx::Transform yellow_quad_to_target_transform;
        yellow_quad_to_target_transform.Translate(25.5f, 25.5f);
        yellow_quad_to_target_transform.Scale(0.5f, 0.5f);
        SharedQuadState* yellow_shared_state = CreateTestSharedQuadState(
            yellow_quad_to_target_transform, rect, pass.get());

        SolidColorDrawQuad* yellow = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(yellow_shared_state, rect, rect, SK_ColorYELLOW, false);

        gfx::Transform blue_quad_to_target_transform;
        SharedQuadState* blue_shared_state = CreateTestSharedQuadState(
            blue_quad_to_target_transform, rect, pass.get());

        SolidColorDrawQuad* blue = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(blue_shared_state, rect, rect, SK_ColorBLUE, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("axis_aligned.png")),
            ExactPixelComparator(true)));
    }

    // This test tests that forcing anti-aliasing off works as expected.
    // Anti-aliasing is only supported in the gl renderer.
    TEST_F(GLRendererPixelTest, ForceAntiAliasingOff)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, rect, transform_to_root);

        gfx::Transform hole_quad_to_target_transform;
        hole_quad_to_target_transform.Translate(50, 50);
        hole_quad_to_target_transform.Scale(0.5f + 1.0f / (rect.width() * 2.0f),
            0.5f + 1.0f / (rect.height() * 2.0f));
        SharedQuadState* hole_shared_state = CreateTestSharedQuadState(
            hole_quad_to_target_transform, rect, pass.get());

        SolidColorDrawQuad* hole = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        hole->SetAll(
            hole_shared_state, rect, rect, rect, false, SK_ColorTRANSPARENT, true);

        gfx::Transform green_quad_to_target_transform;
        SharedQuadState* green_shared_state = CreateTestSharedQuadState(
            green_quad_to_target_transform, rect, pass.get());

        SolidColorDrawQuad* green = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        green->SetNew(green_shared_state, rect, rect, SK_ColorGREEN, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("force_anti_aliasing_off.png")),
            ExactPixelComparator(false)));
    }

    TEST_F(GLRendererPixelTest, AntiAliasingPerspective)
    {
        gfx::Rect rect(this->device_viewport_size_);

        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(RenderPassId(1, 1), rect);

        gfx::Rect red_rect(0, 0, 180, 500);
        gfx::Transform red_quad_to_target_transform(
            1.0f, 2.4520f, 10.6206f, 19.0f, 0.0f, 0.3528f, 5.9737f, 9.5f, 0.0f,
            -0.2250f, -0.9744f, 0.0f, 0.0f, 0.0225f, 0.0974f, 1.0f);
        SharedQuadState* red_shared_state = CreateTestSharedQuadState(
            red_quad_to_target_transform, red_rect, pass.get());
        SolidColorDrawQuad* red = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        red->SetNew(red_shared_state, red_rect, red_rect, SK_ColorRED, false);

        gfx::Rect green_rect(19, 7, 180, 10);
        SharedQuadState* green_shared_state = CreateTestSharedQuadState(gfx::Transform(), green_rect, pass.get());
        SolidColorDrawQuad* green = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        green->SetNew(
            green_shared_state, green_rect, green_rect, SK_ColorGREEN, false);

        SharedQuadState* blue_shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());
        SolidColorDrawQuad* blue = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(blue_shared_state, rect, rect, SK_ColorBLUE, false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("anti_aliasing_perspective.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    TYPED_TEST(SoftwareRendererPixelTest, PictureDrawQuadIdentityScale)
    {
        gfx::Rect viewport(this->device_viewport_size_);
        // TODO(enne): the renderer should figure this out on its own.
        ResourceFormat texture_format = RGBA_8888;
        bool nearest_neighbor = false;

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, viewport, transform_to_root);

        // One clipped blue quad in the lower right corner.  Outside the clip
        // is red, which should not appear.
        gfx::Rect blue_rect(gfx::Size(100, 100));
        gfx::Rect blue_clip_rect(gfx::Point(50, 50), gfx::Size(50, 50));

        scoped_ptr<FakeDisplayListRecordingSource> blue_recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            blue_rect.size());
        SkPaint red_paint;
        red_paint.setColor(SK_ColorRED);
        blue_recording->add_draw_rect_with_paint(blue_rect, red_paint);
        SkPaint blue_paint;
        blue_paint.setColor(SK_ColorBLUE);
        blue_recording->add_draw_rect_with_paint(blue_clip_rect, blue_paint);
        blue_recording->Rerecord();

        scoped_refptr<FakeDisplayListRasterSource> blue_raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(
            blue_recording.get(), false);

        gfx::Vector2d offset(viewport.bottom_right() - blue_rect.bottom_right());
        gfx::Transform blue_quad_to_target_transform;
        blue_quad_to_target_transform.Translate(offset.x(), offset.y());
        gfx::Rect blue_target_clip_rect = MathUtil::MapEnclosingClippedRect(
            blue_quad_to_target_transform, blue_clip_rect);
        SharedQuadState* blue_shared_state = CreateTestSharedQuadStateClipped(blue_quad_to_target_transform, blue_rect,
            blue_target_clip_rect, pass.get());

        PictureDrawQuad* blue_quad = pass->CreateAndAppendDrawQuad<PictureDrawQuad>();

        blue_quad->SetNew(blue_shared_state,
            viewport, // Intentionally bigger than clip.
            gfx::Rect(), viewport, gfx::RectF(viewport),
            viewport.size(), nearest_neighbor, texture_format, viewport,
            1.f, blue_raster_source.Pass());

        // One viewport-filling green quad.
        scoped_ptr<FakeDisplayListRecordingSource> green_recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            viewport.size());
        SkPaint green_paint;
        green_paint.setColor(SK_ColorGREEN);
        green_recording->add_draw_rect_with_paint(viewport, green_paint);
        green_recording->Rerecord();
        scoped_refptr<FakeDisplayListRasterSource> green_raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(
            green_recording.get(), false);

        gfx::Transform green_quad_to_target_transform;
        SharedQuadState* green_shared_state = CreateTestSharedQuadState(
            green_quad_to_target_transform, viewport, pass.get());

        PictureDrawQuad* green_quad = pass->CreateAndAppendDrawQuad<PictureDrawQuad>();
        green_quad->SetNew(green_shared_state, viewport, gfx::Rect(), viewport,
            gfx::RectF(0.f, 0.f, 1.f, 1.f), viewport.size(),
            nearest_neighbor, texture_format, viewport, 1.f,
            green_raster_source.Pass());

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("green_with_blue_corner.png")),
            ExactPixelComparator(true)));
    }

    // Not WithSkiaGPUBackend since that path currently requires tiles for opacity.
    TYPED_TEST(SoftwareRendererPixelTest, PictureDrawQuadOpacity)
    {
        gfx::Rect viewport(this->device_viewport_size_);
        ResourceFormat texture_format = RGBA_8888;
        bool nearest_neighbor = false;

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, viewport, transform_to_root);

        // One viewport-filling 0.5-opacity green quad.
        scoped_ptr<FakeDisplayListRecordingSource> green_recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            viewport.size());
        SkPaint green_paint;
        green_paint.setColor(SK_ColorGREEN);
        green_recording->add_draw_rect_with_paint(viewport, green_paint);
        green_recording->Rerecord();
        scoped_refptr<FakeDisplayListRasterSource> green_raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(
            green_recording.get(), false);

        gfx::Transform green_quad_to_target_transform;
        SharedQuadState* green_shared_state = CreateTestSharedQuadState(
            green_quad_to_target_transform, viewport, pass.get());
        green_shared_state->opacity = 0.5f;

        PictureDrawQuad* green_quad = pass->CreateAndAppendDrawQuad<PictureDrawQuad>();
        green_quad->SetNew(green_shared_state, viewport, gfx::Rect(), viewport,
            gfx::RectF(0, 0, 1, 1), viewport.size(), nearest_neighbor,
            texture_format, viewport, 1.f, green_raster_source.get());

        // One viewport-filling white quad.
        scoped_ptr<FakeDisplayListRecordingSource> white_recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            viewport.size());
        SkPaint white_paint;
        white_paint.setColor(SK_ColorWHITE);
        white_recording->add_draw_rect_with_paint(viewport, white_paint);
        white_recording->Rerecord();
        scoped_refptr<FakeDisplayListRasterSource> white_raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(
            white_recording.get(), false);

        gfx::Transform white_quad_to_target_transform;
        SharedQuadState* white_shared_state = CreateTestSharedQuadState(
            white_quad_to_target_transform, viewport, pass.get());

        PictureDrawQuad* white_quad = pass->CreateAndAppendDrawQuad<PictureDrawQuad>();
        white_quad->SetNew(white_shared_state, viewport, gfx::Rect(), viewport,
            gfx::RectF(0, 0, 1, 1), viewport.size(), nearest_neighbor,
            texture_format, viewport, 1.f, white_raster_source.Pass());

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("green_alpha.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

    template <typename TypeParam>
    bool IsSoftwareRenderer()
    {
        return false;
    }

    template <>
    bool IsSoftwareRenderer<SoftwareRenderer>()
    {
        return true;
    }

    template <>
    bool IsSoftwareRenderer<SoftwareRendererWithExpandedViewport>()
    {
        return true;
    }

    // If we disable image filtering, then a 2x2 bitmap should appear as four
    // huge sharp squares.
    TYPED_TEST(SoftwareRendererPixelTest, PictureDrawQuadDisableImageFiltering)
    {
        // We only care about this in software mode since bilinear filtering is
        // cheap in hardware.
        if (!IsSoftwareRenderer<TypeParam>())
            return;

        gfx::Rect viewport(this->device_viewport_size_);
        ResourceFormat texture_format = RGBA_8888;
        bool nearest_neighbor = false;

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, viewport, transform_to_root);

        skia::RefPtr<SkSurface> surface = skia::AdoptRef(SkSurface::NewRasterN32Premul(2, 2));
        ASSERT_NE(surface, nullptr);
        SkCanvas* canvas = surface->getCanvas();
        canvas->drawPoint(0, 0, SK_ColorGREEN);
        canvas->drawPoint(0, 1, SK_ColorBLUE);
        canvas->drawPoint(1, 0, SK_ColorBLUE);
        canvas->drawPoint(1, 1, SK_ColorGREEN);
        skia::RefPtr<SkImage> image = skia::AdoptRef(surface->newImageSnapshot());

        scoped_ptr<FakeDisplayListRecordingSource> recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            viewport.size());
        SkPaint paint;
        paint.setFilterQuality(kLow_SkFilterQuality);
        recording->add_draw_image_with_paint(image.get(), gfx::Point(), paint);
        recording->Rerecord();
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(recording.get(),
            false);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(quad_to_target_transform, viewport, pass.get());

        PictureDrawQuad* quad = pass->CreateAndAppendDrawQuad<PictureDrawQuad>();
        quad->SetNew(shared_state, viewport, gfx::Rect(), viewport,
            gfx::RectF(0, 0, 2, 2), viewport.size(), nearest_neighbor,
            texture_format, viewport, 1.f, raster_source.Pass());

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        this->disable_picture_quad_image_filtering_ = true;

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("four_blue_green_checkers.png")),
            ExactPixelComparator(true)));
    }

    // This disables filtering by setting |nearest_neighbor| on the PictureDrawQuad.
    TYPED_TEST(SoftwareRendererPixelTest, PictureDrawQuadNearestNeighbor)
    {
        gfx::Rect viewport(this->device_viewport_size_);
        ResourceFormat texture_format = RGBA_8888;
        bool nearest_neighbor = true;

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, viewport, transform_to_root);

        skia::RefPtr<SkSurface> surface = skia::AdoptRef(SkSurface::NewRasterN32Premul(2, 2));
        ASSERT_NE(surface, nullptr);
        SkCanvas* canvas = surface->getCanvas();
        canvas->drawPoint(0, 0, SK_ColorGREEN);
        canvas->drawPoint(0, 1, SK_ColorBLUE);
        canvas->drawPoint(1, 0, SK_ColorBLUE);
        canvas->drawPoint(1, 1, SK_ColorGREEN);
        skia::RefPtr<SkImage> image = skia::AdoptRef(surface->newImageSnapshot());

        scoped_ptr<FakeDisplayListRecordingSource> recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            viewport.size());
        SkPaint paint;
        paint.setFilterQuality(kLow_SkFilterQuality);
        recording->add_draw_image_with_paint(image.get(), gfx::Point(), paint);
        recording->Rerecord();
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(recording.get(),
            false);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(quad_to_target_transform, viewport, pass.get());

        PictureDrawQuad* quad = pass->CreateAndAppendDrawQuad<PictureDrawQuad>();
        quad->SetNew(shared_state, viewport, gfx::Rect(), viewport,
            gfx::RectF(0, 0, 2, 2), viewport.size(), nearest_neighbor,
            texture_format, viewport, 1.f, raster_source.Pass());

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("four_blue_green_checkers.png")),
            ExactPixelComparator(true)));
    }

    // This disables filtering by setting |nearest_neighbor| on the TileDrawQuad.
    TYPED_TEST(RendererPixelTest, TileDrawQuadNearestNeighbor)
    {
        gfx::Rect viewport(this->device_viewport_size_);
        bool swizzle_contents = true;
        bool nearest_neighbor = true;

        SkBitmap bitmap;
        bitmap.allocN32Pixels(2, 2);
        {
            SkAutoLockPixels lock(bitmap);
            SkCanvas canvas(bitmap);
            canvas.drawPoint(0, 0, SK_ColorGREEN);
            canvas.drawPoint(0, 1, SK_ColorBLUE);
            canvas.drawPoint(1, 0, SK_ColorBLUE);
            canvas.drawPoint(1, 1, SK_ColorGREEN);
        }

        gfx::Size tile_size(2, 2);
        ResourceId resource = this->resource_provider_->CreateResource(
            tile_size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            RGBA_8888);

        {
            SkAutoLockPixels lock(bitmap);
            this->resource_provider_->CopyToResource(
                resource, static_cast<uint8_t*>(bitmap.getPixels()), tile_size);
        }

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, viewport, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(quad_to_target_transform, viewport, pass.get());

        TileDrawQuad* quad = pass->CreateAndAppendDrawQuad<TileDrawQuad>();
        quad->SetNew(shared_state, viewport, gfx::Rect(), viewport, resource,
            gfx::RectF(gfx::Rect(tile_size)), tile_size, swizzle_contents,
            nearest_neighbor);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("four_blue_green_checkers.png")),
            ExactPixelComparator(true)));
    }

    // This disables filtering by setting |nearest_neighbor| to true on the
    // TextureDrawQuad.
    TYPED_TEST(SoftwareRendererPixelTest, TextureDrawQuadNearestNeighbor)
    {
        gfx::Rect viewport(this->device_viewport_size_);
        bool nearest_neighbor = true;

        SkBitmap bitmap;
        bitmap.allocN32Pixels(2, 2);
        {
            SkAutoLockPixels lock(bitmap);
            SkCanvas canvas(bitmap);
            canvas.drawPoint(0, 0, SK_ColorGREEN);
            canvas.drawPoint(0, 1, SK_ColorBLUE);
            canvas.drawPoint(1, 0, SK_ColorBLUE);
            canvas.drawPoint(1, 1, SK_ColorGREEN);
        }

        gfx::Size tile_size(2, 2);
        ResourceId resource = this->resource_provider_->CreateResource(
            tile_size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            RGBA_8888);

        {
            SkAutoLockPixels lock(bitmap);
            this->resource_provider_->CopyToResource(
                resource, static_cast<uint8_t*>(bitmap.getPixels()), tile_size);
        }

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, viewport, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(quad_to_target_transform, viewport, pass.get());

        float vertex_opacity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        TextureDrawQuad* quad = pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_state, viewport, gfx::Rect(), viewport, resource, false,
            gfx::PointF(0, 0), gfx::PointF(1, 1), SK_ColorBLACK,
            vertex_opacity, false, nearest_neighbor);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("four_blue_green_checkers.png")),
            FuzzyPixelComparator(false, 2.f, 0.f, 256.f, 256, 0.f)));
    }

    // This ensures filtering is enabled by setting |nearest_neighbor| to false on
    // the TextureDrawQuad.
    TYPED_TEST(SoftwareRendererPixelTest, TextureDrawQuadLinear)
    {
        gfx::Rect viewport(this->device_viewport_size_);
        bool nearest_neighbor = false;

        SkBitmap bitmap;
        bitmap.allocN32Pixels(2, 2);
        {
            SkAutoLockPixels lock(bitmap);
            SkCanvas canvas(bitmap);
            canvas.drawPoint(0, 0, SK_ColorGREEN);
            canvas.drawPoint(0, 1, SK_ColorBLUE);
            canvas.drawPoint(1, 0, SK_ColorBLUE);
            canvas.drawPoint(1, 1, SK_ColorGREEN);
        }

        gfx::Size tile_size(2, 2);
        ResourceId resource = this->resource_provider_->CreateResource(
            tile_size, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            RGBA_8888);

        {
            SkAutoLockPixels lock(bitmap);
            this->resource_provider_->CopyToResource(
                resource, static_cast<uint8_t*>(bitmap.getPixels()), tile_size);
        }

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, viewport, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(quad_to_target_transform, viewport, pass.get());

        float vertex_opacity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        TextureDrawQuad* quad = pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_state, viewport, gfx::Rect(), viewport, resource, false,
            gfx::PointF(0, 0), gfx::PointF(1, 1), SK_ColorBLACK,
            vertex_opacity, false, nearest_neighbor);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        // Allow for a small amount of error as the blending alogrithm used by Skia is
        // affected by the offset in the expanded rect.
        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("four_blue_green_checkers_linear.png")),
            FuzzyPixelComparator(false, 100.f, 0.f, 16.f, 16.f, 0.f)));
    }

    TYPED_TEST(SoftwareRendererPixelTest, PictureDrawQuadNonIdentityScale)
    {
        gfx::Rect viewport(this->device_viewport_size_);
        // TODO(enne): the renderer should figure this out on its own.
        ResourceFormat texture_format = RGBA_8888;
        bool nearest_neighbor = false;

        RenderPassId id(1, 1);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> pass = CreateTestRenderPass(id, viewport, transform_to_root);

        // As scaling up the blue checkerboards will cause sampling on the GPU,
        // a few extra "cleanup rects" need to be added to clobber the blending
        // to make the output image more clean.  This will also test subrects
        // of the layer.
        gfx::Transform green_quad_to_target_transform;
        gfx::Rect green_rect1(gfx::Point(80, 0), gfx::Size(20, 100));
        gfx::Rect green_rect2(gfx::Point(0, 80), gfx::Size(100, 20));

        scoped_ptr<FakeDisplayListRecordingSource> green_recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            viewport.size());

        SkPaint red_paint;
        red_paint.setColor(SK_ColorRED);
        green_recording->add_draw_rect_with_paint(viewport, red_paint);
        SkPaint green_paint;
        green_paint.setColor(SK_ColorGREEN);
        green_recording->add_draw_rect_with_paint(green_rect1, green_paint);
        green_recording->add_draw_rect_with_paint(green_rect2, green_paint);
        green_recording->Rerecord();
        scoped_refptr<FakeDisplayListRasterSource> green_raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(
            green_recording.get(), false);

        SharedQuadState* top_right_green_shared_quad_state = CreateTestSharedQuadState(green_quad_to_target_transform, viewport,
            pass.get());

        PictureDrawQuad* green_quad1 = pass->CreateAndAppendDrawQuad<PictureDrawQuad>();
        green_quad1->SetNew(
            top_right_green_shared_quad_state, green_rect1, gfx::Rect(), green_rect1,
            gfx::RectF(gfx::SizeF(green_rect1.size())), green_rect1.size(),
            nearest_neighbor, texture_format, green_rect1, 1.f, green_raster_source);

        PictureDrawQuad* green_quad2 = pass->CreateAndAppendDrawQuad<PictureDrawQuad>();
        green_quad2->SetNew(top_right_green_shared_quad_state, green_rect2,
            gfx::Rect(), green_rect2,
            gfx::RectF(gfx::SizeF(green_rect2.size())),
            green_rect2.size(), nearest_neighbor, texture_format,
            green_rect2, 1.f, green_raster_source.Pass());

        // Add a green clipped checkerboard in the bottom right to help test
        // interleaving picture quad content and solid color content.
        gfx::Rect bottom_right_rect(
            gfx::Point(viewport.width() / 2, viewport.height() / 2),
            gfx::Size(viewport.width() / 2, viewport.height() / 2));
        SharedQuadState* bottom_right_green_shared_state = CreateTestSharedQuadStateClipped(green_quad_to_target_transform, viewport,
            bottom_right_rect, pass.get());
        SolidColorDrawQuad* bottom_right_color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        bottom_right_color_quad->SetNew(bottom_right_green_shared_state,
            viewport,
            viewport,
            SK_ColorGREEN,
            false);

        // Add two blue checkerboards taking up the bottom left and top right,
        // but use content scales as content rects to make this happen.
        // The content is at a 4x content scale.
        gfx::Rect layer_rect(gfx::Size(20, 30));
        float contents_scale = 4.f;
        // Two rects that touch at their corners, arbitrarily placed in the layer.
        gfx::RectF blue_layer_rect1(gfx::PointF(5.5f, 9.0f), gfx::SizeF(2.5f, 2.5f));
        gfx::RectF blue_layer_rect2(gfx::PointF(8.0f, 6.5f), gfx::SizeF(2.5f, 2.5f));
        gfx::RectF union_layer_rect = blue_layer_rect1;
        union_layer_rect.Union(blue_layer_rect2);

        // Because scaling up will cause sampling outside the rects, add one extra
        // pixel of buffer at the final content scale.
        float inset = -1.f / contents_scale;
        blue_layer_rect1.Inset(inset, inset, inset, inset);
        blue_layer_rect2.Inset(inset, inset, inset, inset);

        scoped_ptr<FakeDisplayListRecordingSource> recording = FakeDisplayListRecordingSource::CreateFilledRecordingSource(
            layer_rect.size());

        Region outside(layer_rect);
        outside.Subtract(gfx::ToEnclosingRect(union_layer_rect));
        for (Region::Iterator iter(outside); iter.has_rect(); iter.next()) {
            recording->add_draw_rect_with_paint(iter.rect(), red_paint);
        }

        SkPaint blue_paint;
        blue_paint.setColor(SK_ColorBLUE);
        recording->add_draw_rectf_with_paint(blue_layer_rect1, blue_paint);
        recording->add_draw_rectf_with_paint(blue_layer_rect2, blue_paint);
        recording->Rerecord();
        scoped_refptr<FakeDisplayListRasterSource> raster_source = FakeDisplayListRasterSource::CreateFromRecordingSource(recording.get(),
            false);

        gfx::Rect content_rect(
            gfx::ScaleToEnclosingRect(layer_rect, contents_scale));
        gfx::Rect content_union_rect(
            gfx::ToEnclosingRect(gfx::ScaleRect(union_layer_rect, contents_scale)));

        // At a scale of 4x the rectangles with a width of 2.5 will take up 10 pixels,
        // so scale an additional 10x to make them 100x100.
        gfx::Transform quad_to_target_transform;
        quad_to_target_transform.Scale(10.0, 10.0);
        gfx::Rect quad_content_rect(gfx::Size(20, 20));
        SharedQuadState* blue_shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, quad_content_rect, pass.get());

        PictureDrawQuad* blue_quad = pass->CreateAndAppendDrawQuad<PictureDrawQuad>();
        blue_quad->SetNew(blue_shared_state, quad_content_rect, gfx::Rect(),
            quad_content_rect, gfx::RectF(quad_content_rect),
            content_union_rect.size(), nearest_neighbor, texture_format,
            content_union_rect, contents_scale, raster_source.Pass());

        // Fill left half of viewport with green.
        gfx::Transform half_green_quad_to_target_transform;
        gfx::Rect half_green_rect(gfx::Size(viewport.width() / 2, viewport.height()));
        SharedQuadState* half_green_shared_state = CreateTestSharedQuadState(
            half_green_quad_to_target_transform, half_green_rect, pass.get());
        SolidColorDrawQuad* half_color_quad = pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        half_color_quad->SetNew(half_green_shared_state,
            half_green_rect,
            half_green_rect,
            SK_ColorGREEN,
            false);

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("four_blue_green_checkers.png")),
            ExactPixelComparator(true)));
    }

    typedef RendererPixelTest<GLRendererWithFlippedSurface>
        GLRendererPixelTestWithFlippedOutputSurface;

    TEST_F(GLRendererPixelTestWithFlippedOutputSurface, ExplicitFlipTest)
    {
        // This draws a blue rect above a yellow rect with an inverted output surface.
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        gfx::Rect blue_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);
        gfx::Rect yellow_rect(0,
            this->device_viewport_size_.height() / 2,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* yellow = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(shared_state, yellow_rect, yellow_rect, SK_ColorYELLOW, false);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(gfx::Transform(), pass_rect, root_pass.get());
        CreateTestRenderPassDrawQuad(
            pass_shared_state, pass_rect, child_pass_id, root_pass.get());

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("blue_yellow_flipped.png")),
            ExactPixelComparator(true)));
    }

    TEST_F(GLRendererPixelTestWithFlippedOutputSurface, CheckChildPassUnflipped)
    {
        // This draws a blue rect above a yellow rect with an inverted output surface.
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        gfx::Rect blue_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);
        gfx::Rect yellow_rect(0,
            this->device_viewport_size_.height() / 2,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height() / 2);
        SolidColorDrawQuad* yellow = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        yellow->SetNew(shared_state, yellow_rect, yellow_rect, SK_ColorYELLOW, false);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(gfx::Transform(), pass_rect, root_pass.get());
        CreateTestRenderPassDrawQuad(
            pass_shared_state, pass_rect, child_pass_id, root_pass.get());

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        // Check that the child pass remains unflipped.
        EXPECT_TRUE(this->RunPixelTestWithReadbackTarget(
            &pass_list,
            pass_list.front(),
            base::FilePath(FILE_PATH_LITERAL("blue_yellow.png")),
            ExactPixelComparator(true)));
    }

    TEST_F(GLRendererPixelTest, CheckReadbackSubset)
    {
        gfx::Rect viewport_rect(this->device_viewport_size_);

        RenderPassId root_pass_id(1, 1);
        scoped_ptr<RenderPass> root_pass = CreateTestRootRenderPass(root_pass_id, viewport_rect);

        RenderPassId child_pass_id(2, 2);
        gfx::Rect pass_rect(this->device_viewport_size_);
        gfx::Transform transform_to_root;
        scoped_ptr<RenderPass> child_pass = CreateTestRenderPass(child_pass_id, pass_rect, transform_to_root);

        gfx::Transform quad_to_target_transform;
        SharedQuadState* shared_state = CreateTestSharedQuadState(
            quad_to_target_transform, viewport_rect, child_pass.get());

        // Draw a green quad full-size with a blue quad in the lower-right corner.
        gfx::Rect blue_rect(this->device_viewport_size_.width() * 3 / 4,
            this->device_viewport_size_.height() * 3 / 4,
            this->device_viewport_size_.width() * 3 / 4,
            this->device_viewport_size_.height() * 3 / 4);
        SolidColorDrawQuad* blue = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        blue->SetNew(shared_state, blue_rect, blue_rect, SK_ColorBLUE, false);
        gfx::Rect green_rect(0,
            0,
            this->device_viewport_size_.width(),
            this->device_viewport_size_.height());
        SolidColorDrawQuad* green = child_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
        green->SetNew(shared_state, green_rect, green_rect, SK_ColorGREEN, false);

        SharedQuadState* pass_shared_state = CreateTestSharedQuadState(gfx::Transform(), pass_rect, root_pass.get());
        CreateTestRenderPassDrawQuad(
            pass_shared_state, pass_rect, child_pass_id, root_pass.get());

        RenderPassList pass_list;
        pass_list.push_back(child_pass.Pass());
        pass_list.push_back(root_pass.Pass());

        // Check that the child pass remains unflipped.
        gfx::Rect capture_rect(this->device_viewport_size_.width() / 2,
            this->device_viewport_size_.height() / 2,
            this->device_viewport_size_.width() / 2,
            this->device_viewport_size_.height() / 2);
        EXPECT_TRUE(this->RunPixelTestWithReadbackTargetAndArea(
            &pass_list,
            pass_list.front(),
            base::FilePath(FILE_PATH_LITERAL("green_small_with_blue_corner.png")),
            ExactPixelComparator(true),
            &capture_rect));
    }

    TYPED_TEST(RendererPixelTest, WrapModeRepeat)
    {
        gfx::Rect rect(this->device_viewport_size_);

        RenderPassId id(1, 1);
        scoped_ptr<RenderPass> pass = CreateTestRootRenderPass(id, rect);

        SharedQuadState* shared_state = CreateTestSharedQuadState(gfx::Transform(), rect, pass.get());

        gfx::Size texture_size(4, 4);
        SkPMColor colors[4] = {
            SkPreMultiplyColor(SkColorSetARGB(255, 0, 255, 0)),
            SkPreMultiplyColor(SkColorSetARGB(255, 0, 128, 0)),
            SkPreMultiplyColor(SkColorSetARGB(255, 0, 64, 0)),
            SkPreMultiplyColor(SkColorSetARGB(255, 0, 0, 0)),
        };
        uint32_t pixels[16] = {
            colors[0],
            colors[0],
            colors[1],
            colors[1],
            colors[0],
            colors[0],
            colors[1],
            colors[1],
            colors[2],
            colors[2],
            colors[3],
            colors[3],
            colors[2],
            colors[2],
            colors[3],
            colors[3],
        };
        ResourceId resource = this->resource_provider_->CreateResource(
            texture_size, GL_REPEAT, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            RGBA_8888);
        this->resource_provider_->CopyToResource(
            resource, reinterpret_cast<uint8_t*>(pixels), texture_size);

        float vertex_opacity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        TextureDrawQuad* texture_quad = pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        texture_quad->SetNew(
            shared_state, gfx::Rect(this->device_viewport_size_), gfx::Rect(),
            gfx::Rect(this->device_viewport_size_), resource,
            true, // premultiplied_alpha
            gfx::PointF(0.0f, 0.0f), // uv_top_left
            gfx::PointF( // uv_bottom_right
                this->device_viewport_size_.width() / texture_size.width(),
                this->device_viewport_size_.height() / texture_size.height()),
            SK_ColorWHITE, vertex_opacity,
            false, // flipped
            false); // nearest_neighbor

        RenderPassList pass_list;
        pass_list.push_back(pass.Pass());

        EXPECT_TRUE(this->RunPixelTest(
            &pass_list,
            base::FilePath(FILE_PATH_LITERAL("wrap_mode_repeat.png")),
            FuzzyPixelOffByOneComparator(true)));
    }

#endif // !defined(OS_ANDROID)

} // namespace
} // namespace cc
