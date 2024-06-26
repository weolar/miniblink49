// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/gl_renderer.h"

#include <set>

#include "base/location.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"
#include "cc/base/math_util.h"
#include "cc/output/compositor_frame_metadata.h"
#include "cc/output/copy_output_request.h"
#include "cc/output/copy_output_result.h"
#include "cc/output/overlay_strategy_single_on_top.h"
#include "cc/output/overlay_strategy_underlay.h"
#include "cc/output/texture_mailbox_deleter.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/resources/resource_provider.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/fake_output_surface.h"
#include "cc/test/fake_output_surface_client.h"
#include "cc/test/fake_renderer_client.h"
#include "cc/test/fake_resource_provider.h"
#include "cc/test/pixel_test.h"
#include "cc/test/render_pass_test_utils.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_web_graphics_context_3d.h"
#include "gpu/GLES2/gl2extchromium.h"
#include "gpu/command_buffer/client/context_support.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkImageFilter.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/effects/SkColorFilterImageFilter.h"
#include "third_party/skia/include/effects/SkColorMatrixFilter.h"
#include "ui/gfx/transform.h"

using testing::_;
using testing::AnyNumber;
using testing::Args;
using testing::AtLeast;
using testing::ElementsAre;
using testing::Expectation;
using testing::InSequence;
using testing::Mock;
using testing::Return;
using testing::StrictMock;

namespace cc {

class GLRendererTest : public testing::Test {
protected:
    RenderPass* root_render_pass() { return render_passes_in_draw_order_.back(); }

    RenderPassList render_passes_in_draw_order_;
};

#define EXPECT_PROGRAM_VALID(program_binding)          \
    do {                                               \
        EXPECT_TRUE((program_binding)->program());     \
        EXPECT_TRUE((program_binding)->initialized()); \
    } while (false)

static inline SkXfermode::Mode BlendModeToSkXfermode(BlendMode blend_mode)
{
    switch (blend_mode) {
    case BLEND_MODE_NONE:
    case BLEND_MODE_NORMAL:
        return SkXfermode::kSrcOver_Mode;
    case BLEND_MODE_SCREEN:
        return SkXfermode::kScreen_Mode;
    case BLEND_MODE_OVERLAY:
        return SkXfermode::kOverlay_Mode;
    case BLEND_MODE_DARKEN:
        return SkXfermode::kDarken_Mode;
    case BLEND_MODE_LIGHTEN:
        return SkXfermode::kLighten_Mode;
    case BLEND_MODE_COLOR_DODGE:
        return SkXfermode::kColorDodge_Mode;
    case BLEND_MODE_COLOR_BURN:
        return SkXfermode::kColorBurn_Mode;
    case BLEND_MODE_HARD_LIGHT:
        return SkXfermode::kHardLight_Mode;
    case BLEND_MODE_SOFT_LIGHT:
        return SkXfermode::kSoftLight_Mode;
    case BLEND_MODE_DIFFERENCE:
        return SkXfermode::kDifference_Mode;
    case BLEND_MODE_EXCLUSION:
        return SkXfermode::kExclusion_Mode;
    case BLEND_MODE_MULTIPLY:
        return SkXfermode::kMultiply_Mode;
    case BLEND_MODE_HUE:
        return SkXfermode::kHue_Mode;
    case BLEND_MODE_SATURATION:
        return SkXfermode::kSaturation_Mode;
    case BLEND_MODE_COLOR:
        return SkXfermode::kColor_Mode;
    case BLEND_MODE_LUMINOSITY:
        return SkXfermode::kLuminosity_Mode;
    }
    return SkXfermode::kSrcOver_Mode;
}

// Explicitly named to be a friend in GLRenderer for shader access.
class GLRendererShaderPixelTest : public GLRendererPixelTest {
public:
    void SetUp() override
    {
        GLRendererPixelTest::SetUp();
        ASSERT_FALSE(renderer()->IsContextLost());
    }

    void TearDown() override
    {
        GLRendererPixelTest::TearDown();
        ASSERT_FALSE(renderer()->IsContextLost());
    }

    void TestBasicShaders()
    {
        EXPECT_PROGRAM_VALID(renderer()->GetDebugBorderProgram());
        EXPECT_PROGRAM_VALID(renderer()->GetSolidColorProgram());
        EXPECT_PROGRAM_VALID(renderer()->GetSolidColorProgramAA());
    }

    void TestShadersWithPrecision(TexCoordPrecision precision)
    {
        EXPECT_PROGRAM_VALID(renderer()->GetTextureIOSurfaceProgram(precision));
        if (renderer()->Capabilities().using_egl_image)
            EXPECT_PROGRAM_VALID(renderer()->GetVideoStreamTextureProgram(precision));
        else
            EXPECT_FALSE(renderer()->GetVideoStreamTextureProgram(precision));
    }

    void TestShadersWithPrecisionAndBlend(TexCoordPrecision precision,
        BlendMode blend_mode)
    {
        EXPECT_PROGRAM_VALID(
            renderer()->GetRenderPassProgram(precision, blend_mode));
        EXPECT_PROGRAM_VALID(
            renderer()->GetRenderPassProgramAA(precision, blend_mode));
    }

    void TestShadersWithPrecisionAndSampler(TexCoordPrecision precision,
        SamplerType sampler)
    {
        if (!renderer()->Capabilities().using_egl_image && sampler == SAMPLER_TYPE_EXTERNAL_OES) {
            // This will likely be hit in tests due to usage of osmesa.
            return;
        }

        EXPECT_PROGRAM_VALID(renderer()->GetTextureProgram(precision, sampler));
        EXPECT_PROGRAM_VALID(
            renderer()->GetNonPremultipliedTextureProgram(precision, sampler));
        EXPECT_PROGRAM_VALID(
            renderer()->GetTextureBackgroundProgram(precision, sampler));
        EXPECT_PROGRAM_VALID(
            renderer()->GetNonPremultipliedTextureBackgroundProgram(precision,
                sampler));

        EXPECT_PROGRAM_VALID(renderer()->GetTileProgram(precision, sampler));
        EXPECT_PROGRAM_VALID(renderer()->GetTileProgramOpaque(precision, sampler));
        EXPECT_PROGRAM_VALID(renderer()->GetTileProgramAA(precision, sampler));
        EXPECT_PROGRAM_VALID(renderer()->GetTileProgramSwizzle(precision, sampler));
        EXPECT_PROGRAM_VALID(
            renderer()->GetTileProgramSwizzleOpaque(precision, sampler));
        EXPECT_PROGRAM_VALID(
            renderer()->GetTileProgramSwizzleAA(precision, sampler));
        EXPECT_PROGRAM_VALID(renderer()->GetVideoYUVProgram(precision, sampler));
        EXPECT_PROGRAM_VALID(renderer()->GetVideoYUVAProgram(precision, sampler));
    }

    void TestShadersWithMasks(TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode,
        bool mask_for_background)
    {
        if (!renderer()->Capabilities().using_egl_image && sampler == SAMPLER_TYPE_EXTERNAL_OES) {
            // This will likely be hit in tests due to usage of osmesa.
            return;
        }

        EXPECT_PROGRAM_VALID(renderer()->GetRenderPassMaskProgram(
            precision, sampler, blend_mode, mask_for_background));
        EXPECT_PROGRAM_VALID(renderer()->GetRenderPassMaskProgramAA(
            precision, sampler, blend_mode, mask_for_background));
        EXPECT_PROGRAM_VALID(renderer()->GetRenderPassMaskColorMatrixProgramAA(
            precision, sampler, blend_mode, mask_for_background));
        EXPECT_PROGRAM_VALID(renderer()->GetRenderPassMaskColorMatrixProgram(
            precision, sampler, blend_mode, mask_for_background));
    }
};

namespace {

#if !defined(OS_ANDROID) && !defined(OS_WIN)
    static const TexCoordPrecision kPrecisionList[] = { TEX_COORD_PRECISION_MEDIUM,
        TEX_COORD_PRECISION_HIGH };

    static const BlendMode kBlendModeList[LAST_BLEND_MODE + 1] = {
        BLEND_MODE_NONE,
        BLEND_MODE_NORMAL,
        BLEND_MODE_SCREEN,
        BLEND_MODE_OVERLAY,
        BLEND_MODE_DARKEN,
        BLEND_MODE_LIGHTEN,
        BLEND_MODE_COLOR_DODGE,
        BLEND_MODE_COLOR_BURN,
        BLEND_MODE_HARD_LIGHT,
        BLEND_MODE_SOFT_LIGHT,
        BLEND_MODE_DIFFERENCE,
        BLEND_MODE_EXCLUSION,
        BLEND_MODE_MULTIPLY,
        BLEND_MODE_HUE,
        BLEND_MODE_SATURATION,
        BLEND_MODE_COLOR,
        BLEND_MODE_LUMINOSITY,
    };

    static const SamplerType kSamplerList[] = {
        SAMPLER_TYPE_2D,
        SAMPLER_TYPE_2D_RECT,
        SAMPLER_TYPE_EXTERNAL_OES,
    };

    TEST_F(GLRendererShaderPixelTest, BasicShadersCompile)
    {
        TestBasicShaders();
    }

    class PrecisionShaderPixelTest
        : public GLRendererShaderPixelTest,
          public ::testing::WithParamInterface<TexCoordPrecision> {
    };

    TEST_P(PrecisionShaderPixelTest, ShadersCompile)
    {
        TestShadersWithPrecision(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(PrecisionShadersCompile,
        PrecisionShaderPixelTest,
        ::testing::ValuesIn(kPrecisionList));

    class PrecisionBlendShaderPixelTest
        : public GLRendererShaderPixelTest,
          public ::testing::WithParamInterface<
              std::tr1::tuple<TexCoordPrecision, BlendMode>> {
    };

    TEST_P(PrecisionBlendShaderPixelTest, ShadersCompile)
    {
        TestShadersWithPrecisionAndBlend(std::tr1::get<0>(GetParam()),
            std::tr1::get<1>(GetParam()));
    }

    INSTANTIATE_TEST_CASE_P(
        PrecisionBlendShadersCompile,
        PrecisionBlendShaderPixelTest,
        ::testing::Combine(::testing::ValuesIn(kPrecisionList),
            ::testing::ValuesIn(kBlendModeList)));

    class PrecisionSamplerShaderPixelTest
        : public GLRendererShaderPixelTest,
          public ::testing::WithParamInterface<
              std::tr1::tuple<TexCoordPrecision, SamplerType>> {
    };

    TEST_P(PrecisionSamplerShaderPixelTest, ShadersCompile)
    {
        TestShadersWithPrecisionAndSampler(std::tr1::get<0>(GetParam()),
            std::tr1::get<1>(GetParam()));
    }

    INSTANTIATE_TEST_CASE_P(PrecisionSamplerShadersCompile,
        PrecisionSamplerShaderPixelTest,
        ::testing::Combine(::testing::ValuesIn(kPrecisionList),
            ::testing::ValuesIn(kSamplerList)));

    class MaskShaderPixelTest
        : public GLRendererShaderPixelTest,
          public ::testing::WithParamInterface<
              std::tr1::tuple<TexCoordPrecision, SamplerType, BlendMode, bool>> {
    };

    TEST_P(MaskShaderPixelTest, ShadersCompile)
    {
        TestShadersWithMasks(
            std::tr1::get<0>(GetParam()), std::tr1::get<1>(GetParam()),
            std::tr1::get<2>(GetParam()), std::tr1::get<3>(GetParam()));
    }

    INSTANTIATE_TEST_CASE_P(MaskShadersCompile,
        MaskShaderPixelTest,
        ::testing::Combine(::testing::ValuesIn(kPrecisionList),
            ::testing::ValuesIn(kSamplerList),
            ::testing::ValuesIn(kBlendModeList),
            ::testing::Bool()));

#endif

    class FakeRendererGL : public GLRenderer {
    public:
        FakeRendererGL(RendererClient* client,
            const RendererSettings* settings,
            OutputSurface* output_surface,
            ResourceProvider* resource_provider)
            : GLRenderer(client,
                settings,
                output_surface,
                resource_provider,
                NULL,
                0)
        {
        }

        FakeRendererGL(RendererClient* client,
            const RendererSettings* settings,
            OutputSurface* output_surface,
            ResourceProvider* resource_provider,
            TextureMailboxDeleter* texture_mailbox_deleter)
            : GLRenderer(client,
                settings,
                output_surface,
                resource_provider,
                texture_mailbox_deleter,
                0)
        {
        }

        void SetOverlayProcessor(OverlayProcessor* processor)
        {
            overlay_processor_.reset(processor);
        }

        // GLRenderer methods.

        // Changing visibility to public.
        using GLRenderer::BeginDrawingFrame;
        using GLRenderer::DoDrawQuad;
        using GLRenderer::FinishDrawingQuadList;
        using GLRenderer::IsBackbufferDiscarded;
        using GLRenderer::stencil_enabled;
    };

    class GLRendererWithDefaultHarnessTest : public GLRendererTest {
    protected:
        GLRendererWithDefaultHarnessTest()
        {
            output_surface_ = FakeOutputSurface::Create3d(TestWebGraphicsContext3D::Create()).Pass();
            CHECK(output_surface_->BindToClient(&output_surface_client_));

            shared_bitmap_manager_.reset(new TestSharedBitmapManager());
            resource_provider_ = FakeResourceProvider::Create(
                output_surface_.get(), shared_bitmap_manager_.get());
            renderer_ = make_scoped_ptr(new FakeRendererGL(&renderer_client_,
                &settings_,
                output_surface_.get(),
                resource_provider_.get()));
        }

        void SwapBuffers() { renderer_->SwapBuffers(CompositorFrameMetadata()); }

        RendererSettings settings_;
        FakeOutputSurfaceClient output_surface_client_;
        scoped_ptr<FakeOutputSurface> output_surface_;
        FakeRendererClient renderer_client_;
        scoped_ptr<SharedBitmapManager> shared_bitmap_manager_;
        scoped_ptr<ResourceProvider> resource_provider_;
        scoped_ptr<FakeRendererGL> renderer_;
    };

    // Closing the namespace here so that GLRendererShaderTest can take advantage
    // of the friend relationship with GLRenderer and all of the mock classes
    // declared above it.
} // namespace

class GLRendererShaderTest : public GLRendererTest {
protected:
    GLRendererShaderTest()
    {
        output_surface_ = FakeOutputSurface::Create3d().Pass();
        CHECK(output_surface_->BindToClient(&output_surface_client_));

        shared_bitmap_manager_.reset(new TestSharedBitmapManager());
        resource_provider_ = FakeResourceProvider::Create(
            output_surface_.get(), shared_bitmap_manager_.get());
        renderer_.reset(new FakeRendererGL(&renderer_client_,
            &settings_,
            output_surface_.get(),
            resource_provider_.get()));
    }

    void TestRenderPassProgram(TexCoordPrecision precision,
        BlendMode blend_mode)
    {
        EXPECT_PROGRAM_VALID(
            &renderer_->render_pass_program_[precision][blend_mode]);
        EXPECT_EQ(renderer_->render_pass_program_[precision][blend_mode].program(),
            renderer_->program_shadow_);
    }

    void TestRenderPassColorMatrixProgram(TexCoordPrecision precision,
        BlendMode blend_mode)
    {
        EXPECT_PROGRAM_VALID(
            &renderer_->render_pass_color_matrix_program_[precision][blend_mode]);
        EXPECT_EQ(
            renderer_->render_pass_color_matrix_program_[precision][blend_mode]
                .program(),
            renderer_->program_shadow_);
    }

    void TestRenderPassMaskProgram(TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode)
    {
        EXPECT_PROGRAM_VALID(
            &renderer_->render_pass_mask_program_[precision]
                                                 [sampler]
                                                 [blend_mode]
                                                 [NO_MASK]);
        EXPECT_EQ(
            renderer_->render_pass_mask_program_[precision]
                                                [sampler]
                                                [blend_mode]
                                                [NO_MASK]
                                                    .program(),
            renderer_->program_shadow_);
    }

    void TestRenderPassMaskColorMatrixProgram(TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode)
    {
        EXPECT_PROGRAM_VALID(&renderer_->render_pass_mask_color_matrix_program_
                                  [precision][sampler][blend_mode][NO_MASK]);
        EXPECT_EQ(renderer_->render_pass_mask_color_matrix_program_
                      [precision][sampler][blend_mode][NO_MASK]
                          .program(),
            renderer_->program_shadow_);
    }

    void TestRenderPassProgramAA(TexCoordPrecision precision,
        BlendMode blend_mode)
    {
        EXPECT_PROGRAM_VALID(
            &renderer_->render_pass_program_aa_[precision][blend_mode]);
        EXPECT_EQ(
            renderer_->render_pass_program_aa_[precision][blend_mode].program(),
            renderer_->program_shadow_);
    }

    void TestRenderPassColorMatrixProgramAA(TexCoordPrecision precision,
        BlendMode blend_mode)
    {
        EXPECT_PROGRAM_VALID(
            &renderer_
                 ->render_pass_color_matrix_program_aa_[precision][blend_mode]);
        EXPECT_EQ(
            renderer_->render_pass_color_matrix_program_aa_[precision][blend_mode]
                .program(),
            renderer_->program_shadow_);
    }

    void TestRenderPassMaskProgramAA(TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode)
    {
        EXPECT_PROGRAM_VALID(
            &renderer_
                 ->render_pass_mask_program_aa_
                     [precision][sampler][blend_mode][NO_MASK]);
        EXPECT_EQ(
            renderer_->render_pass_mask_program_aa_[precision][sampler][blend_mode]
                                                   [NO_MASK]
                                                       .program(),
            renderer_->program_shadow_);
    }

    void TestRenderPassMaskColorMatrixProgramAA(TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode)
    {
        EXPECT_PROGRAM_VALID(&renderer_->render_pass_mask_color_matrix_program_aa_
                                  [precision][sampler][blend_mode][NO_MASK]);
        EXPECT_EQ(renderer_->render_pass_mask_color_matrix_program_aa_
                      [precision][sampler][blend_mode][NO_MASK]
                          .program(),
            renderer_->program_shadow_);
    }

    void TestSolidColorProgramAA()
    {
        EXPECT_PROGRAM_VALID(&renderer_->solid_color_program_aa_);
        EXPECT_EQ(renderer_->solid_color_program_aa_.program(),
            renderer_->program_shadow_);
    }

    RendererSettings settings_;
    FakeOutputSurfaceClient output_surface_client_;
    scoped_ptr<FakeOutputSurface> output_surface_;
    FakeRendererClient renderer_client_;
    scoped_ptr<SharedBitmapManager> shared_bitmap_manager_;
    scoped_ptr<ResourceProvider> resource_provider_;
    scoped_ptr<FakeRendererGL> renderer_;
};

namespace {

    // Test GLRenderer DiscardBackbuffer functionality:
    // Suggest discarding framebuffer when one exists and the renderer is not
    // visible.
    // Expected: it is discarded and damage tracker is reset.
    TEST_F(
        GLRendererWithDefaultHarnessTest,
        SuggestBackbufferNoShouldDiscardBackbufferAndDamageRootLayerIfNotVisible)
    {
        renderer_->SetVisible(false);
        EXPECT_EQ(1, renderer_client_.set_full_root_layer_damage_count());
        EXPECT_TRUE(renderer_->IsBackbufferDiscarded());
    }

    // Test GLRenderer DiscardBackbuffer functionality:
    // Suggest discarding framebuffer when one exists and the renderer is visible.
    // Expected: the allocation is ignored.
    TEST_F(GLRendererWithDefaultHarnessTest,
        SuggestBackbufferNoDoNothingWhenVisible)
    {
        renderer_->SetVisible(true);
        EXPECT_EQ(0, renderer_client_.set_full_root_layer_damage_count());
        EXPECT_FALSE(renderer_->IsBackbufferDiscarded());
    }

    // Test GLRenderer DiscardBackbuffer functionality:
    // Suggest discarding framebuffer when one does not exist.
    // Expected: it does nothing.
    TEST_F(GLRendererWithDefaultHarnessTest,
        SuggestBackbufferNoWhenItDoesntExistShouldDoNothing)
    {
        renderer_->SetVisible(false);
        EXPECT_EQ(1, renderer_client_.set_full_root_layer_damage_count());
        EXPECT_TRUE(renderer_->IsBackbufferDiscarded());

        EXPECT_EQ(1, renderer_client_.set_full_root_layer_damage_count());
        EXPECT_TRUE(renderer_->IsBackbufferDiscarded());
    }

    // Test GLRenderer DiscardBackbuffer functionality:
    // Begin drawing a frame while a framebuffer is discarded.
    // Expected: will recreate framebuffer.
    TEST_F(GLRendererWithDefaultHarnessTest,
        DiscardedBackbufferIsRecreatedForScopeDuration)
    {
        gfx::Rect viewport_rect(1, 1);
        renderer_->SetVisible(false);
        EXPECT_TRUE(renderer_->IsBackbufferDiscarded());
        EXPECT_EQ(1, renderer_client_.set_full_root_layer_damage_count());

        AddRenderPass(&render_passes_in_draw_order_,
            RenderPassId(1, 0),
            viewport_rect,
            gfx::Transform());

        renderer_->SetVisible(true);
        renderer_->DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);
        EXPECT_FALSE(renderer_->IsBackbufferDiscarded());

        SwapBuffers();
        EXPECT_EQ(1u, output_surface_->num_sent_frames());
    }

    TEST_F(GLRendererWithDefaultHarnessTest, ExternalStencil)
    {
        gfx::Rect viewport_rect(1, 1);
        EXPECT_FALSE(renderer_->stencil_enabled());

        output_surface_->set_has_external_stencil_test(true);

        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, RenderPassId(1, 0),
            viewport_rect, gfx::Transform());
        root_pass->has_transparent_background = false;

        renderer_->DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);
        EXPECT_TRUE(renderer_->stencil_enabled());
    }

    class ForbidSynchronousCallContext : public TestWebGraphicsContext3D {
    public:
        ForbidSynchronousCallContext() { }

        void getAttachedShaders(GLuint program,
            GLsizei max_count,
            GLsizei* count,
            GLuint* shaders) override
        {
            ADD_FAILURE();
        }
        GLint getAttribLocation(GLuint program, const GLchar* name) override
        {
            ADD_FAILURE();
            return 0;
        }
        void getBooleanv(GLenum pname, GLboolean* value) override { ADD_FAILURE(); }
        void getBufferParameteriv(GLenum target,
            GLenum pname,
            GLint* value) override
        {
            ADD_FAILURE();
        }
        GLenum getError() override
        {
            ADD_FAILURE();
            return GL_NO_ERROR;
        }
        void getFloatv(GLenum pname, GLfloat* value) override { ADD_FAILURE(); }
        void getFramebufferAttachmentParameteriv(GLenum target,
            GLenum attachment,
            GLenum pname,
            GLint* value) override
        {
            ADD_FAILURE();
        }
        void getIntegerv(GLenum pname, GLint* value) override
        {
            if (pname == GL_MAX_TEXTURE_SIZE) {
                // MAX_TEXTURE_SIZE is cached client side, so it's OK to query.
                *value = 1024;
            } else {
                ADD_FAILURE();
            }
        }

        // We allow querying the shader compilation and program link status in debug
        // mode, but not release.
        void getProgramiv(GLuint program, GLenum pname, GLint* value) override
        {
#ifndef NDEBUG
            *value = 1;
#else
            ADD_FAILURE();
#endif
        }

        void getShaderiv(GLuint shader, GLenum pname, GLint* value) override
        {
#ifndef NDEBUG
            *value = 1;
#else
            ADD_FAILURE();
#endif
        }

        void getRenderbufferParameteriv(GLenum target,
            GLenum pname,
            GLint* value) override
        {
            ADD_FAILURE();
        }

        void getShaderPrecisionFormat(GLenum shadertype,
            GLenum precisiontype,
            GLint* range,
            GLint* precision) override
        {
            ADD_FAILURE();
        }
        void getTexParameterfv(GLenum target, GLenum pname, GLfloat* value) override
        {
            ADD_FAILURE();
        }
        void getTexParameteriv(GLenum target, GLenum pname, GLint* value) override
        {
            ADD_FAILURE();
        }
        void getUniformfv(GLuint program, GLint location, GLfloat* value) override
        {
            ADD_FAILURE();
        }
        void getUniformiv(GLuint program, GLint location, GLint* value) override
        {
            ADD_FAILURE();
        }
        GLint getUniformLocation(GLuint program, const GLchar* name) override
        {
            ADD_FAILURE();
            return 0;
        }
        void getVertexAttribfv(GLuint index, GLenum pname, GLfloat* value) override
        {
            ADD_FAILURE();
        }
        void getVertexAttribiv(GLuint index, GLenum pname, GLint* value) override
        {
            ADD_FAILURE();
        }
        GLsizeiptr getVertexAttribOffset(GLuint index, GLenum pname) override
        {
            ADD_FAILURE();
            return 0;
        }
    };
    TEST_F(GLRendererTest, InitializationDoesNotMakeSynchronousCalls)
    {
        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(FakeOutputSurface::Create3d(
            scoped_ptr<TestWebGraphicsContext3D>(new ForbidSynchronousCallContext)));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());
    }

    class LoseContextOnFirstGetContext : public TestWebGraphicsContext3D {
    public:
        LoseContextOnFirstGetContext() { }

        void getProgramiv(GLuint program, GLenum pname, GLint* value) override
        {
            context_lost_ = true;
            *value = 0;
        }

        void getShaderiv(GLuint shader, GLenum pname, GLint* value) override
        {
            context_lost_ = true;
            *value = 0;
        }
    };

    TEST_F(GLRendererTest, InitializationWithQuicklyLostContextDoesNotAssert)
    {
        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(FakeOutputSurface::Create3d(
            scoped_ptr<TestWebGraphicsContext3D>(new LoseContextOnFirstGetContext)));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());
    }

    class ClearCountingContext : public TestWebGraphicsContext3D {
    public:
        ClearCountingContext() { test_capabilities_.gpu.discard_framebuffer = true; }

        MOCK_METHOD3(discardFramebufferEXT,
            void(GLenum target,
                GLsizei numAttachments,
                const GLenum* attachments));
        MOCK_METHOD1(clear, void(GLbitfield mask));
    };

    TEST_F(GLRendererTest, OpaqueBackground)
    {
        scoped_ptr<ClearCountingContext> context_owned(new ClearCountingContext);
        ClearCountingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());

        gfx::Rect viewport_rect(1, 1);
        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, RenderPassId(1, 0),
            viewport_rect, gfx::Transform());
        root_pass->has_transparent_background = false;

        // On DEBUG builds, render passes with opaque background clear to blue to
        // easily see regions that were not drawn on the screen.
        EXPECT_CALL(*context, discardFramebufferEXT(GL_FRAMEBUFFER, _, _))
            .With(Args<2, 1>(ElementsAre(GL_COLOR_EXT)))
            .Times(1);
#ifdef NDEBUG
        EXPECT_CALL(*context, clear(_)).Times(0);
#else
        EXPECT_CALL(*context, clear(_)).Times(1);
#endif
        renderer.DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);
        Mock::VerifyAndClearExpectations(context);
    }

    TEST_F(GLRendererTest, TransparentBackground)
    {
        scoped_ptr<ClearCountingContext> context_owned(new ClearCountingContext);
        ClearCountingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());

        gfx::Rect viewport_rect(1, 1);
        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, RenderPassId(1, 0),
            viewport_rect, gfx::Transform());
        root_pass->has_transparent_background = true;

        EXPECT_CALL(*context, discardFramebufferEXT(GL_FRAMEBUFFER, 1, _)).Times(1);
        EXPECT_CALL(*context, clear(_)).Times(1);
        renderer.DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);

        Mock::VerifyAndClearExpectations(context);
    }

    TEST_F(GLRendererTest, OffscreenOutputSurface)
    {
        scoped_ptr<ClearCountingContext> context_owned(new ClearCountingContext);
        ClearCountingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::CreateOffscreen(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());

        gfx::Rect viewport_rect(1, 1);
        AddRenderPass(&render_passes_in_draw_order_,
            RenderPassId(1, 0),
            viewport_rect,
            gfx::Transform());

        EXPECT_CALL(*context, discardFramebufferEXT(GL_FRAMEBUFFER, _, _))
            .With(Args<2, 1>(ElementsAre(GL_COLOR_ATTACHMENT0)))
            .Times(1);
        EXPECT_CALL(*context, clear(_)).Times(AnyNumber());
        renderer.DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);
        Mock::VerifyAndClearExpectations(context);
    }

    class VisibilityChangeIsLastCallTrackingContext
        : public TestWebGraphicsContext3D {
    public:
        VisibilityChangeIsLastCallTrackingContext()
            : last_call_was_set_visibility_(false)
        {
        }

        // TestWebGraphicsContext3D methods.
        void flush() override { last_call_was_set_visibility_ = false; }
        void deleteTexture(GLuint) override { last_call_was_set_visibility_ = false; }
        void deleteFramebuffer(GLuint) override
        {
            last_call_was_set_visibility_ = false;
        }
        void deleteQueryEXT(GLuint) override
        {
            last_call_was_set_visibility_ = false;
        }
        void deleteRenderbuffer(GLuint) override
        {
            last_call_was_set_visibility_ = false;
        }

        // Methods added for test.
        void set_last_call_was_visibility(bool visible)
        {
            DCHECK(last_call_was_set_visibility_ == false);
            last_call_was_set_visibility_ = true;
        }
        bool last_call_was_set_visibility() const
        {
            return last_call_was_set_visibility_;
        }

    private:
        bool last_call_was_set_visibility_;
    };

    TEST_F(GLRendererTest, VisibilityChangeIsLastCall)
    {
        scoped_ptr<VisibilityChangeIsLastCallTrackingContext> context_owned(
            new VisibilityChangeIsLastCallTrackingContext);
        VisibilityChangeIsLastCallTrackingContext* context = context_owned.get();

        scoped_refptr<TestContextProvider> provider = TestContextProvider::Create(context_owned.Pass());

        provider->support()->SetSurfaceVisibleCallback(base::Bind(
            &VisibilityChangeIsLastCallTrackingContext::set_last_call_was_visibility,
            base::Unretained(context)));

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(provider));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());

        gfx::Rect viewport_rect(1, 1);
        AddRenderPass(&render_passes_in_draw_order_,
            RenderPassId(1, 0),
            viewport_rect,
            gfx::Transform());

        // Ensure that the call to SetSurfaceVisible is the last call issue to the
        // GPU process, after glFlush is called, and after the RendererClient's
        // SetManagedMemoryPolicy is called. Plumb this tracking between both the
        // RenderClient and the Context by giving them both a pointer to a variable on
        // the stack.
        renderer.SetVisible(true);
        renderer.DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);
        renderer.SetVisible(false);
        EXPECT_TRUE(context->last_call_was_set_visibility());
    }

    class TextureStateTrackingContext : public TestWebGraphicsContext3D {
    public:
        TextureStateTrackingContext()
            : active_texture_(GL_INVALID_ENUM)
        {
            test_capabilities_.gpu.egl_image_external = true;
        }

        MOCK_METHOD1(waitSyncPoint, void(unsigned sync_point));
        MOCK_METHOD3(texParameteri, void(GLenum target, GLenum pname, GLint param));
        MOCK_METHOD4(drawElements,
            void(GLenum mode, GLsizei count, GLenum type, GLintptr offset));

        virtual void activeTexture(GLenum texture)
        {
            EXPECT_NE(texture, active_texture_);
            active_texture_ = texture;
        }

        GLenum active_texture() const { return active_texture_; }

    private:
        GLenum active_texture_;
    };

    TEST_F(GLRendererTest, ActiveTextureState)
    {
        scoped_ptr<TextureStateTrackingContext> context_owned(
            new TextureStateTrackingContext);
        TextureStateTrackingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());

        // During initialization we are allowed to set any texture parameters.
        EXPECT_CALL(*context, texParameteri(_, _, _)).Times(AnyNumber());

        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, RenderPassId(1, 1),
            gfx::Rect(100, 100), gfx::Transform());
        uint32_t mailbox_sync_point;
        AddOneOfEveryQuadType(root_pass, resource_provider.get(), RenderPassId(0, 0),
            &mailbox_sync_point);

        renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);

        // Set up expected texture filter state transitions that match the quads
        // created in AppendOneOfEveryQuadType().
        Mock::VerifyAndClearExpectations(context);
        {
            InSequence sequence;

            // The sync points for all quads are waited on first. This sync point is
            // for a texture quad drawn later in the frame.
            EXPECT_CALL(*context, waitSyncPoint(mailbox_sync_point)).Times(1);

            // yuv_quad is drawn with the default linear filter.
            EXPECT_CALL(*context, drawElements(_, _, _, _));

            // tile_quad is drawn with GL_NEAREST because it is not transformed or
            // scaled.
            EXPECT_CALL(
                *context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            EXPECT_CALL(
                *context,
                texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            EXPECT_CALL(*context, drawElements(_, _, _, _));

            // transformed_tile_quad uses GL_LINEAR.
            EXPECT_CALL(*context, drawElements(_, _, _, _));

            // scaled_tile_quad also uses GL_LINEAR.
            EXPECT_CALL(*context, drawElements(_, _, _, _));

            // The remaining quads also use GL_LINEAR because nearest neighbor
            // filtering is currently only used with tile quads.
            EXPECT_CALL(*context, drawElements(_, _, _, _)).Times(6);
        }

        gfx::Rect viewport_rect(100, 100);
        renderer.DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);
        Mock::VerifyAndClearExpectations(context);
    }

    class NoClearRootRenderPassMockContext : public TestWebGraphicsContext3D {
    public:
        MOCK_METHOD1(clear, void(GLbitfield mask));
        MOCK_METHOD4(drawElements,
            void(GLenum mode, GLsizei count, GLenum type, GLintptr offset));
    };

    TEST_F(GLRendererTest, ShouldClearRootRenderPass)
    {
        scoped_ptr<NoClearRootRenderPassMockContext> mock_context_owned(
            new NoClearRootRenderPassMockContext);
        NoClearRootRenderPassMockContext* mock_context = mock_context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(mock_context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        settings.should_clear_root_render_pass = false;

        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());

        gfx::Rect viewport_rect(10, 10);

        RenderPassId child_pass_id(2, 0);
        RenderPass* child_pass = AddRenderPass(&render_passes_in_draw_order_, child_pass_id, viewport_rect,
            gfx::Transform());
        AddQuad(child_pass, viewport_rect, SK_ColorBLUE);

        RenderPassId root_pass_id(1, 0);
        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id, viewport_rect,
            gfx::Transform());
        AddQuad(root_pass, viewport_rect, SK_ColorGREEN);

        AddRenderPassQuad(root_pass, child_pass);

#ifdef NDEBUG
        GLint clear_bits = GL_COLOR_BUFFER_BIT;
#else
        GLint clear_bits = GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
#endif

        // First render pass is not the root one, clearing should happen.
        EXPECT_CALL(*mock_context, clear(clear_bits)).Times(AtLeast(1));

        Expectation first_render_pass = EXPECT_CALL(*mock_context, drawElements(_, _, _, _)).Times(1);

        // The second render pass is the root one, clearing should be prevented.
        EXPECT_CALL(*mock_context, clear(clear_bits)).Times(0).After(first_render_pass);

        EXPECT_CALL(*mock_context, drawElements(_, _, _, _)).Times(AnyNumber()).After(first_render_pass);

        renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
        renderer.DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);

        // In multiple render passes all but the root pass should clear the
        // framebuffer.
        Mock::VerifyAndClearExpectations(&mock_context);
    }

    class ScissorTestOnClearCheckingContext : public TestWebGraphicsContext3D {
    public:
        ScissorTestOnClearCheckingContext()
            : scissor_enabled_(false)
        {
        }

        void clear(GLbitfield) override { EXPECT_FALSE(scissor_enabled_); }

        void enable(GLenum cap) override
        {
            if (cap == GL_SCISSOR_TEST)
                scissor_enabled_ = true;
        }

        void disable(GLenum cap) override
        {
            if (cap == GL_SCISSOR_TEST)
                scissor_enabled_ = false;
        }

    private:
        bool scissor_enabled_;
    };

    TEST_F(GLRendererTest, ScissorTestWhenClearing)
    {
        scoped_ptr<ScissorTestOnClearCheckingContext> context_owned(
            new ScissorTestOnClearCheckingContext);

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());
        EXPECT_FALSE(renderer.Capabilities().using_partial_swap);

        gfx::Rect viewport_rect(1, 1);

        gfx::Rect grand_child_rect(25, 25);
        RenderPassId grand_child_pass_id(3, 0);
        RenderPass* grand_child_pass = AddRenderPass(&render_passes_in_draw_order_, grand_child_pass_id,
            grand_child_rect, gfx::Transform());
        AddClippedQuad(grand_child_pass, grand_child_rect, SK_ColorYELLOW);

        gfx::Rect child_rect(50, 50);
        RenderPassId child_pass_id(2, 0);
        RenderPass* child_pass = AddRenderPass(&render_passes_in_draw_order_, child_pass_id, child_rect,
            gfx::Transform());
        AddQuad(child_pass, child_rect, SK_ColorBLUE);

        RenderPassId root_pass_id(1, 0);
        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id, viewport_rect,
            gfx::Transform());
        AddQuad(root_pass, viewport_rect, SK_ColorGREEN);

        AddRenderPassQuad(root_pass, child_pass);
        AddRenderPassQuad(child_pass, grand_child_pass);

        renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
        renderer.DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);
    }

    class DiscardCheckingContext : public TestWebGraphicsContext3D {
    public:
        DiscardCheckingContext()
            : discarded_(0)
        {
            set_have_post_sub_buffer(true);
            set_have_discard_framebuffer(true);
        }

        void discardFramebufferEXT(GLenum target,
            GLsizei numAttachments,
            const GLenum* attachments) override
        {
            ++discarded_;
        }

        int discarded() const { return discarded_; }
        void reset() { discarded_ = 0; }

    private:
        int discarded_;
    };

    class NonReshapableOutputSurface : public FakeOutputSurface {
    public:
        explicit NonReshapableOutputSurface(
            scoped_ptr<TestWebGraphicsContext3D> context3d)
            : FakeOutputSurface(TestContextProvider::Create(context3d.Pass()),
                false)
        {
            surface_size_ = gfx::Size(500, 500);
        }
        void Reshape(const gfx::Size& size, float scale_factor) override { }
        void set_fixed_size(const gfx::Size& size) { surface_size_ = size; }
    };

    TEST_F(GLRendererTest, NoDiscardOnPartialUpdates)
    {
        scoped_ptr<DiscardCheckingContext> context_owned(new DiscardCheckingContext);
        DiscardCheckingContext* context = context_owned.get();

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<NonReshapableOutputSurface> output_surface(
            new NonReshapableOutputSurface(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));
        output_surface->set_fixed_size(gfx::Size(100, 100));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        settings.partial_swap_enabled = true;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());
        EXPECT_TRUE(renderer.Capabilities().using_partial_swap);

        gfx::Rect viewport_rect(100, 100);
        gfx::Rect clip_rect(100, 100);

        {
            // Partial frame, should not discard.
            RenderPassId root_pass_id(1, 0);
            RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id,
                viewport_rect, gfx::Transform());
            AddQuad(root_pass, viewport_rect, SK_ColorGREEN);
            root_pass->damage_rect = gfx::Rect(2, 2, 3, 3);

            renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
            renderer.DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                clip_rect,
                false);
            EXPECT_EQ(0, context->discarded());
            context->reset();
        }
        {
            // Full frame, should discard.
            RenderPassId root_pass_id(1, 0);
            RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id,
                viewport_rect, gfx::Transform());
            AddQuad(root_pass, viewport_rect, SK_ColorGREEN);
            root_pass->damage_rect = root_pass->output_rect;

            renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
            renderer.DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                clip_rect,
                false);
            EXPECT_EQ(1, context->discarded());
            context->reset();
        }
        {
            // Full frame, external scissor is set, should not discard.
            output_surface->set_has_external_stencil_test(true);
            RenderPassId root_pass_id(1, 0);
            RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id,
                viewport_rect, gfx::Transform());
            AddQuad(root_pass, viewport_rect, SK_ColorGREEN);
            root_pass->damage_rect = root_pass->output_rect;
            root_pass->has_transparent_background = false;

            renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
            renderer.DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                clip_rect,
                false);
            EXPECT_EQ(0, context->discarded());
            context->reset();
            output_surface->set_has_external_stencil_test(false);
        }
        {
            // Full frame, clipped, should not discard.
            clip_rect = gfx::Rect(10, 10, 10, 10);
            RenderPassId root_pass_id(1, 0);
            RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id,
                viewport_rect, gfx::Transform());
            AddQuad(root_pass, viewport_rect, SK_ColorGREEN);
            root_pass->damage_rect = root_pass->output_rect;

            renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
            renderer.DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                clip_rect,
                false);
            EXPECT_EQ(0, context->discarded());
            context->reset();
        }
        {
            // Full frame, doesn't cover the surface, should not discard.
            viewport_rect = gfx::Rect(10, 10, 10, 10);
            RenderPassId root_pass_id(1, 0);
            RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id,
                viewport_rect, gfx::Transform());
            AddQuad(root_pass, viewport_rect, SK_ColorGREEN);
            root_pass->damage_rect = root_pass->output_rect;

            renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
            renderer.DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                clip_rect,
                false);
            EXPECT_EQ(0, context->discarded());
            context->reset();
        }
        {
            // Full frame, doesn't cover the surface (no offset), should not discard.
            clip_rect = gfx::Rect(100, 100);
            viewport_rect = gfx::Rect(50, 50);
            RenderPassId root_pass_id(1, 0);
            RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id,
                viewport_rect, gfx::Transform());
            AddQuad(root_pass, viewport_rect, SK_ColorGREEN);
            root_pass->damage_rect = root_pass->output_rect;

            renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
            renderer.DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                clip_rect,
                false);
            EXPECT_EQ(0, context->discarded());
            context->reset();
        }
    }

    class FlippedScissorAndViewportContext : public TestWebGraphicsContext3D {
    public:
        MOCK_METHOD4(viewport, void(GLint x, GLint y, GLsizei width, GLsizei height));
        MOCK_METHOD4(scissor, void(GLint x, GLint y, GLsizei width, GLsizei height));
    };

    TEST_F(GLRendererTest, ScissorAndViewportWithinNonreshapableSurface)
    {
        // In Android WebView, the OutputSurface is unable to respect reshape() calls
        // and maintains a fixed size. This test verifies that glViewport and
        // glScissor's Y coordinate is flipped correctly in this environment, and that
        // the glViewport can be at a nonzero origin within the surface.
        scoped_ptr<FlippedScissorAndViewportContext> context_owned(
            new FlippedScissorAndViewportContext);

        // We expect exactly one call to viewport on this context and exactly two
        // to scissor (one to scissor the clear, one to scissor the quad draw).
        EXPECT_CALL(*context_owned, viewport(10, 390, 100, 100));
        EXPECT_CALL(*context_owned, scissor(10, 390, 100, 100));
        EXPECT_CALL(*context_owned, scissor(30, 450, 20, 20));

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            new NonReshapableOutputSurface(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client,
            &settings,
            output_surface.get(),
            resource_provider.get());
        EXPECT_FALSE(renderer.Capabilities().using_partial_swap);

        gfx::Rect device_viewport_rect(10, 10, 100, 100);
        gfx::Rect viewport_rect(device_viewport_rect.size());
        gfx::Rect quad_rect = gfx::Rect(20, 20, 20, 20);

        RenderPassId root_pass_id(1, 0);
        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id, viewport_rect,
            gfx::Transform());
        AddClippedQuad(root_pass, quad_rect, SK_ColorGREEN);

        renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
        renderer.DrawFrame(&render_passes_in_draw_order_,
            1.f,
            device_viewport_rect,
            device_viewport_rect,
            false);
    }

    TEST_F(GLRendererTest, DrawFramePreservesFramebuffer)
    {
        // When using render-to-FBO to display the surface, all rendering is done
        // to a non-zero FBO. Make sure that the framebuffer is always restored to
        // the correct framebuffer during rendering, if changed.
        // Note: there is one path that will set it to 0, but that is after the render
        // has finished.
        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<FakeOutputSurface> output_surface(
            FakeOutputSurface::Create3d(TestWebGraphicsContext3D::Create().Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client, &settings, output_surface.get(),
            resource_provider.get());
        EXPECT_FALSE(renderer.Capabilities().using_partial_swap);

        gfx::Rect device_viewport_rect(0, 0, 100, 100);
        gfx::Rect viewport_rect(device_viewport_rect.size());
        gfx::Rect quad_rect = gfx::Rect(20, 20, 20, 20);

        RenderPassId root_pass_id(1, 0);
        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, root_pass_id, viewport_rect,
            gfx::Transform());
        AddClippedQuad(root_pass, quad_rect, SK_ColorGREEN);

        unsigned fbo;
        gpu::gles2::GLES2Interface* gl = output_surface->context_provider()->ContextGL();
        gl->GenFramebuffers(1, &fbo);
        output_surface->set_framebuffer(fbo);

        renderer.DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
        renderer.DrawFrame(&render_passes_in_draw_order_, 1.f, device_viewport_rect,
            device_viewport_rect, false);

        int bound_fbo;
        gl->GetIntegerv(GL_FRAMEBUFFER_BINDING, &bound_fbo);
        EXPECT_EQ(static_cast<int>(fbo), bound_fbo);
    }

    TEST_F(GLRendererShaderTest, DrawRenderPassQuadShaderPermutations)
    {
        gfx::Rect viewport_rect(1, 1);

        gfx::Rect child_rect(50, 50);
        RenderPassId child_pass_id(2, 0);
        RenderPass* child_pass;

        RenderPassId root_pass_id(1, 0);
        RenderPass* root_pass;

        ResourceId mask = resource_provider_->CreateResource(
            gfx::Size(20, 12), GL_CLAMP_TO_EDGE,
            ResourceProvider::TEXTURE_HINT_IMMUTABLE,
            resource_provider_->best_texture_format());
        resource_provider_->AllocateForTesting(mask);

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
        skia::RefPtr<SkColorFilter> color_filter(
            skia::AdoptRef(SkColorMatrixFilter::Create(matrix)));
        skia::RefPtr<SkImageFilter> filter = skia::AdoptRef(
            SkColorFilterImageFilter::Create(color_filter.get(), NULL));
        FilterOperations filters;
        filters.Append(FilterOperation::CreateReferenceFilter(filter));

        gfx::Transform transform_causing_aa;
        transform_causing_aa.Rotate(20.0);

        for (int i = 0; i <= LAST_BLEND_MODE; ++i) {
            BlendMode blend_mode = static_cast<BlendMode>(i);
            SkXfermode::Mode xfer_mode = BlendModeToSkXfermode(blend_mode);
            settings_.force_blending_with_shaders = (blend_mode != BLEND_MODE_NONE);
            // RenderPassProgram
            render_passes_in_draw_order_.clear();
            child_pass = AddRenderPass(&render_passes_in_draw_order_,
                child_pass_id,
                child_rect,
                gfx::Transform());

            root_pass = AddRenderPass(&render_passes_in_draw_order_,
                root_pass_id,
                viewport_rect,
                gfx::Transform());

            AddRenderPassQuad(root_pass,
                child_pass,
                0,
                FilterOperations(),
                gfx::Transform(),
                xfer_mode);

            renderer_->DecideRenderPassAllocationsForFrame(
                render_passes_in_draw_order_);
            renderer_->DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                viewport_rect,
                false);
            TestRenderPassProgram(TEX_COORD_PRECISION_MEDIUM, blend_mode);

            // RenderPassColorMatrixProgram
            render_passes_in_draw_order_.clear();

            child_pass = AddRenderPass(&render_passes_in_draw_order_,
                child_pass_id,
                child_rect,
                transform_causing_aa);

            root_pass = AddRenderPass(&render_passes_in_draw_order_,
                root_pass_id,
                viewport_rect,
                gfx::Transform());

            AddRenderPassQuad(
                root_pass, child_pass, 0, filters, gfx::Transform(), xfer_mode);

            renderer_->DecideRenderPassAllocationsForFrame(
                render_passes_in_draw_order_);
            renderer_->DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                viewport_rect,
                false);
            TestRenderPassColorMatrixProgram(TEX_COORD_PRECISION_MEDIUM, blend_mode);

            // RenderPassMaskProgram
            render_passes_in_draw_order_.clear();

            child_pass = AddRenderPass(&render_passes_in_draw_order_,
                child_pass_id,
                child_rect,
                gfx::Transform());

            root_pass = AddRenderPass(&render_passes_in_draw_order_,
                root_pass_id,
                viewport_rect,
                gfx::Transform());

            AddRenderPassQuad(root_pass,
                child_pass,
                mask,
                FilterOperations(),
                gfx::Transform(),
                xfer_mode);

            renderer_->DecideRenderPassAllocationsForFrame(
                render_passes_in_draw_order_);
            renderer_->DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                viewport_rect,
                false);
            TestRenderPassMaskProgram(TEX_COORD_PRECISION_MEDIUM, SAMPLER_TYPE_2D,
                blend_mode);

            // RenderPassMaskColorMatrixProgram
            render_passes_in_draw_order_.clear();

            child_pass = AddRenderPass(&render_passes_in_draw_order_,
                child_pass_id,
                child_rect,
                gfx::Transform());

            root_pass = AddRenderPass(&render_passes_in_draw_order_,
                root_pass_id,
                viewport_rect,
                gfx::Transform());

            AddRenderPassQuad(
                root_pass, child_pass, mask, filters, gfx::Transform(), xfer_mode);

            renderer_->DecideRenderPassAllocationsForFrame(
                render_passes_in_draw_order_);
            renderer_->DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                viewport_rect,
                false);
            TestRenderPassMaskColorMatrixProgram(TEX_COORD_PRECISION_MEDIUM,
                SAMPLER_TYPE_2D, blend_mode);

            // RenderPassProgramAA
            render_passes_in_draw_order_.clear();

            child_pass = AddRenderPass(&render_passes_in_draw_order_,
                child_pass_id,
                child_rect,
                transform_causing_aa);

            root_pass = AddRenderPass(&render_passes_in_draw_order_,
                root_pass_id,
                viewport_rect,
                gfx::Transform());

            AddRenderPassQuad(root_pass,
                child_pass,
                0,
                FilterOperations(),
                transform_causing_aa,
                xfer_mode);

            renderer_->DecideRenderPassAllocationsForFrame(
                render_passes_in_draw_order_);
            renderer_->DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                viewport_rect,
                false);
            TestRenderPassProgramAA(TEX_COORD_PRECISION_MEDIUM, blend_mode);

            // RenderPassColorMatrixProgramAA
            render_passes_in_draw_order_.clear();

            child_pass = AddRenderPass(&render_passes_in_draw_order_,
                child_pass_id,
                child_rect,
                transform_causing_aa);

            root_pass = AddRenderPass(&render_passes_in_draw_order_,
                root_pass_id,
                viewport_rect,
                gfx::Transform());

            AddRenderPassQuad(
                root_pass, child_pass, 0, filters, transform_causing_aa, xfer_mode);

            renderer_->DecideRenderPassAllocationsForFrame(
                render_passes_in_draw_order_);
            renderer_->DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                viewport_rect,
                false);
            TestRenderPassColorMatrixProgramAA(TEX_COORD_PRECISION_MEDIUM, blend_mode);

            // RenderPassMaskProgramAA
            render_passes_in_draw_order_.clear();

            child_pass = AddRenderPass(&render_passes_in_draw_order_,
                child_pass_id,
                child_rect,
                transform_causing_aa);

            root_pass = AddRenderPass(&render_passes_in_draw_order_,
                root_pass_id,
                viewport_rect,
                gfx::Transform());

            AddRenderPassQuad(root_pass,
                child_pass,
                mask,
                FilterOperations(),
                transform_causing_aa,
                xfer_mode);

            renderer_->DecideRenderPassAllocationsForFrame(
                render_passes_in_draw_order_);
            renderer_->DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                viewport_rect,
                false);
            TestRenderPassMaskProgramAA(TEX_COORD_PRECISION_MEDIUM, SAMPLER_TYPE_2D,
                blend_mode);

            // RenderPassMaskColorMatrixProgramAA
            render_passes_in_draw_order_.clear();

            child_pass = AddRenderPass(&render_passes_in_draw_order_,
                child_pass_id,
                child_rect,
                transform_causing_aa);

            root_pass = AddRenderPass(&render_passes_in_draw_order_,
                root_pass_id,
                viewport_rect,
                transform_causing_aa);

            AddRenderPassQuad(
                root_pass, child_pass, mask, filters, transform_causing_aa, xfer_mode);

            renderer_->DecideRenderPassAllocationsForFrame(
                render_passes_in_draw_order_);
            renderer_->DrawFrame(&render_passes_in_draw_order_,
                1.f,
                viewport_rect,
                viewport_rect,
                false);
            TestRenderPassMaskColorMatrixProgramAA(TEX_COORD_PRECISION_MEDIUM,
                SAMPLER_TYPE_2D, blend_mode);
        }
    }

    // At this time, the AA code path cannot be taken if the surface's rect would
    // project incorrectly by the given transform, because of w<0 clipping.
    TEST_F(GLRendererShaderTest, DrawRenderPassQuadSkipsAAForClippingTransform)
    {
        gfx::Rect child_rect(50, 50);
        RenderPassId child_pass_id(2, 0);
        RenderPass* child_pass;

        gfx::Rect viewport_rect(1, 1);
        RenderPassId root_pass_id(1, 0);
        RenderPass* root_pass;

        gfx::Transform transform_preventing_aa;
        transform_preventing_aa.ApplyPerspectiveDepth(40.0);
        transform_preventing_aa.RotateAboutYAxis(-20.0);
        transform_preventing_aa.Scale(30.0, 1.0);

        // Verify that the test transform and test rect actually do cause the clipped
        // flag to trigger. Otherwise we are not testing the intended scenario.
        bool clipped = false;
        MathUtil::MapQuad(transform_preventing_aa, gfx::QuadF(gfx::RectF(child_rect)),
            &clipped);
        ASSERT_TRUE(clipped);

        child_pass = AddRenderPass(&render_passes_in_draw_order_,
            child_pass_id,
            child_rect,
            transform_preventing_aa);

        root_pass = AddRenderPass(&render_passes_in_draw_order_,
            root_pass_id,
            viewport_rect,
            gfx::Transform());

        AddRenderPassQuad(root_pass,
            child_pass,
            0,
            FilterOperations(),
            transform_preventing_aa,
            SkXfermode::kSrcOver_Mode);

        renderer_->DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
        renderer_->DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);

        // If use_aa incorrectly ignores clipping, it will use the
        // RenderPassProgramAA shader instead of the RenderPassProgram.
        TestRenderPassProgram(TEX_COORD_PRECISION_MEDIUM, BLEND_MODE_NONE);
    }

    TEST_F(GLRendererShaderTest, DrawSolidColorShader)
    {
        gfx::Rect viewport_rect(1, 1);
        RenderPassId root_pass_id(1, 0);
        RenderPass* root_pass;

        gfx::Transform pixel_aligned_transform_causing_aa;
        pixel_aligned_transform_causing_aa.Translate(25.5f, 25.5f);
        pixel_aligned_transform_causing_aa.Scale(0.5f, 0.5f);

        root_pass = AddRenderPass(&render_passes_in_draw_order_,
            root_pass_id,
            viewport_rect,
            gfx::Transform());
        AddTransformedQuad(root_pass,
            viewport_rect,
            SK_ColorYELLOW,
            pixel_aligned_transform_causing_aa);

        renderer_->DecideRenderPassAllocationsForFrame(render_passes_in_draw_order_);
        renderer_->DrawFrame(&render_passes_in_draw_order_,
            1.f,
            viewport_rect,
            viewport_rect,
            false);

        TestSolidColorProgramAA();
    }

    class OutputSurfaceMockContext : public TestWebGraphicsContext3D {
    public:
        OutputSurfaceMockContext() { test_capabilities_.gpu.post_sub_buffer = true; }

        // Specifically override methods even if they are unused (used in conjunction
        // with StrictMock). We need to make sure that GLRenderer does not issue
        // framebuffer-related GLuint calls directly. Instead these are supposed to go
        // through the OutputSurface abstraction.
        MOCK_METHOD2(bindFramebuffer, void(GLenum target, GLuint framebuffer));
        MOCK_METHOD3(reshapeWithScaleFactor,
            void(int width, int height, float scale_factor));
        MOCK_METHOD4(drawElements,
            void(GLenum mode, GLsizei count, GLenum type, GLintptr offset));
    };

    class MockOutputSurface : public OutputSurface {
    public:
        MockOutputSurface()
            : OutputSurface(
                TestContextProvider::Create(scoped_ptr<TestWebGraphicsContext3D>(
                    new StrictMock<OutputSurfaceMockContext>)))
        {
            surface_size_ = gfx::Size(100, 100);
        }
        virtual ~MockOutputSurface() { }

        MOCK_METHOD0(EnsureBackbuffer, void());
        MOCK_METHOD0(DiscardBackbuffer, void());
        MOCK_METHOD2(Reshape, void(const gfx::Size& size, float scale_factor));
        MOCK_METHOD0(BindFramebuffer, void());
        MOCK_METHOD1(SwapBuffers, void(CompositorFrame* frame));
    };

    class MockOutputSurfaceTest : public GLRendererTest {
    protected:
        virtual void SetUp()
        {
            FakeOutputSurfaceClient output_surface_client_;
            CHECK(output_surface_.BindToClient(&output_surface_client_));

            shared_bitmap_manager_.reset(new TestSharedBitmapManager());
            resource_provider_ = FakeResourceProvider::Create(
                &output_surface_, shared_bitmap_manager_.get());

            renderer_.reset(new FakeRendererGL(&renderer_client_,
                &settings_,
                &output_surface_,
                resource_provider_.get()));
        }

        void SwapBuffers() { renderer_->SwapBuffers(CompositorFrameMetadata()); }

        void DrawFrame(float device_scale_factor,
            const gfx::Rect& device_viewport_rect)
        {
            RenderPassId render_pass_id(1, 0);
            RenderPass* render_pass = AddRenderPass(&render_passes_in_draw_order_, render_pass_id,
                device_viewport_rect, gfx::Transform());
            AddQuad(render_pass, device_viewport_rect, SK_ColorGREEN);

            EXPECT_CALL(output_surface_, EnsureBackbuffer()).WillRepeatedly(Return());

            EXPECT_CALL(output_surface_,
                Reshape(device_viewport_rect.size(), device_scale_factor))
                .Times(1);

            EXPECT_CALL(output_surface_, BindFramebuffer()).Times(1);

            EXPECT_CALL(*Context(), drawElements(_, _, _, _)).Times(1);

            renderer_->DecideRenderPassAllocationsForFrame(
                render_passes_in_draw_order_);
            renderer_->DrawFrame(&render_passes_in_draw_order_,
                device_scale_factor,
                device_viewport_rect,
                device_viewport_rect,
                false);
        }

        OutputSurfaceMockContext* Context()
        {
            return static_cast<OutputSurfaceMockContext*>(
                static_cast<TestContextProvider*>(output_surface_.context_provider())
                    ->TestContext3d());
        }

        RendererSettings settings_;
        FakeOutputSurfaceClient output_surface_client_;
        StrictMock<MockOutputSurface> output_surface_;
        scoped_ptr<SharedBitmapManager> shared_bitmap_manager_;
        scoped_ptr<ResourceProvider> resource_provider_;
        FakeRendererClient renderer_client_;
        scoped_ptr<FakeRendererGL> renderer_;
    };

    TEST_F(MockOutputSurfaceTest, DrawFrameAndSwap)
    {
        gfx::Rect device_viewport_rect(1, 1);
        DrawFrame(1.f, device_viewport_rect);

        EXPECT_CALL(output_surface_, SwapBuffers(_)).Times(1);
        renderer_->SwapBuffers(CompositorFrameMetadata());
    }

    TEST_F(MockOutputSurfaceTest, DrawFrameAndResizeAndSwap)
    {
        gfx::Rect device_viewport_rect(1, 1);

        DrawFrame(1.f, device_viewport_rect);
        EXPECT_CALL(output_surface_, SwapBuffers(_)).Times(1);
        renderer_->SwapBuffers(CompositorFrameMetadata());

        device_viewport_rect = gfx::Rect(2, 2);

        DrawFrame(2.f, device_viewport_rect);
        EXPECT_CALL(output_surface_, SwapBuffers(_)).Times(1);
        renderer_->SwapBuffers(CompositorFrameMetadata());

        DrawFrame(2.f, device_viewport_rect);
        EXPECT_CALL(output_surface_, SwapBuffers(_)).Times(1);
        renderer_->SwapBuffers(CompositorFrameMetadata());

        device_viewport_rect = gfx::Rect(1, 1);

        DrawFrame(1.f, device_viewport_rect);
        EXPECT_CALL(output_surface_, SwapBuffers(_)).Times(1);
        renderer_->SwapBuffers(CompositorFrameMetadata());
    }

    class GLRendererTestSyncPoint : public GLRendererPixelTest {
    protected:
        static void SyncPointCallback(int* callback_count)
        {
            ++(*callback_count);
            base::MessageLoop::current()->QuitWhenIdle();
        }

        static void OtherCallback(int* callback_count)
        {
            ++(*callback_count);
            base::MessageLoop::current()->QuitWhenIdle();
        }
    };

#if !defined(OS_ANDROID)
    TEST_F(GLRendererTestSyncPoint, SignalSyncPointOnLostContext)
    {
        int sync_point_callback_count = 0;
        int other_callback_count = 0;
        gpu::gles2::GLES2Interface* gl = output_surface_->context_provider()->ContextGL();
        gpu::ContextSupport* context_support = output_surface_->context_provider()->ContextSupport();

        uint32 sync_point = gl->InsertSyncPointCHROMIUM();

        gl->LoseContextCHROMIUM(GL_GUILTY_CONTEXT_RESET_ARB,
            GL_INNOCENT_CONTEXT_RESET_ARB);

        context_support->SignalSyncPoint(
            sync_point, base::Bind(&SyncPointCallback, &sync_point_callback_count));
        EXPECT_EQ(0, sync_point_callback_count);
        EXPECT_EQ(0, other_callback_count);

        // Make the sync point happen.
        gl->Finish();
        // Post a task after the sync point.
        base::ThreadTaskRunnerHandle::Get()->PostTask(
            FROM_HERE, base::Bind(&OtherCallback, &other_callback_count));

        base::MessageLoop::current()->Run();

        // The sync point shouldn't have happened since the context was lost.
        EXPECT_EQ(0, sync_point_callback_count);
        EXPECT_EQ(1, other_callback_count);
    }

    TEST_F(GLRendererTestSyncPoint, SignalSyncPoint)
    {
        int sync_point_callback_count = 0;
        int other_callback_count = 0;

        gpu::gles2::GLES2Interface* gl = output_surface_->context_provider()->ContextGL();
        gpu::ContextSupport* context_support = output_surface_->context_provider()->ContextSupport();

        uint32 sync_point = gl->InsertSyncPointCHROMIUM();

        context_support->SignalSyncPoint(
            sync_point, base::Bind(&SyncPointCallback, &sync_point_callback_count));
        EXPECT_EQ(0, sync_point_callback_count);
        EXPECT_EQ(0, other_callback_count);

        // Make the sync point happen.
        gl->Finish();
        // Post a task after the sync point.
        base::ThreadTaskRunnerHandle::Get()->PostTask(
            FROM_HERE, base::Bind(&OtherCallback, &other_callback_count));

        base::MessageLoop::current()->Run();

        // The sync point should have happened.
        EXPECT_EQ(1, sync_point_callback_count);
        EXPECT_EQ(1, other_callback_count);
    }
#endif // OS_ANDROID

    class TestOverlayProcessor : public OverlayProcessor {
    public:
        class Strategy : public OverlayProcessor::Strategy {
        public:
            Strategy() { }
            ~Strategy() override { }
            MOCK_METHOD3(Attempt,
                bool(RenderPassList* render_passes_in_draw_order,
                    OverlayCandidateList* candidates,
                    float device_scale_factor));
        };

        explicit TestOverlayProcessor(OutputSurface* surface)
            : OverlayProcessor(surface)
        {
        }
        ~TestOverlayProcessor() override { }
        void Initialize() override
        {
            strategy_ = new Strategy();
            strategies_.push_back(scoped_ptr<OverlayProcessor::Strategy>(strategy_));
        }

        Strategy* strategy_;
    };

    void MailboxReleased(unsigned sync_point,
        bool lost_resource,
        BlockingTaskRunner* main_thread_task_runner)
    {
    }

    void IgnoreCopyResult(scoped_ptr<CopyOutputResult> result)
    {
    }

    TEST_F(GLRendererTest, DontOverlayWithCopyRequests)
    {
        scoped_ptr<DiscardCheckingContext> context_owned(new DiscardCheckingContext);
        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_owned.Pass()));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());
        scoped_ptr<TextureMailboxDeleter> mailbox_deleter(
            new TextureMailboxDeleter(base::ThreadTaskRunnerHandle::Get()));

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client, &settings, output_surface.get(),
            resource_provider.get(), mailbox_deleter.get());

        TestOverlayProcessor* processor = new TestOverlayProcessor(output_surface.get());
        processor->Initialize();
        renderer.SetOverlayProcessor(processor);

        gfx::Rect viewport_rect(1, 1);
        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, RenderPassId(1, 0),
            viewport_rect, gfx::Transform());
        root_pass->has_transparent_background = false;
        root_pass->copy_requests.push_back(
            CopyOutputRequest::CreateRequest(base::Bind(&IgnoreCopyResult)));

        unsigned sync_point = 0;
        TextureMailbox mailbox = TextureMailbox(gpu::Mailbox::Generate(), GL_TEXTURE_2D, sync_point);
        scoped_ptr<SingleReleaseCallbackImpl> release_callback = SingleReleaseCallbackImpl::Create(base::Bind(&MailboxReleased));
        ResourceId resource_id = resource_provider->CreateResourceFromTextureMailbox(
            mailbox, release_callback.Pass());
        bool premultiplied_alpha = false;
        bool flipped = false;
        bool nearest_neighbor = false;
        float vertex_opacity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        TextureDrawQuad* overlay_quad = root_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        overlay_quad->SetNew(root_pass->CreateAndAppendSharedQuadState(),
            viewport_rect, viewport_rect, viewport_rect, resource_id,
            premultiplied_alpha, gfx::PointF(0, 0),
            gfx::PointF(1, 1), SK_ColorTRANSPARENT, vertex_opacity,
            flipped, nearest_neighbor);
        overlay_quad->set_allow_overlay(true);

        // DirectRenderer::DrawFrame calls into OverlayProcessor::ProcessForOverlays.
        // Attempt will be called for each strategy in OverlayProcessor. We have
        // added a fake strategy, so checking for Attempt calls checks if there was
        // any attempt to overlay, which there shouldn't be. We can't use the quad
        // list because the render pass is cleaned up by DrawFrame.
        EXPECT_CALL(*processor->strategy_, Attempt(_, _, _)).Times(0);
        renderer.DrawFrame(&render_passes_in_draw_order_, 1.f, viewport_rect,
            viewport_rect, false);
        Mock::VerifyAndClearExpectations(processor->strategy_);

        // Without a copy request Attempt() should be called once.
        root_pass = AddRenderPass(&render_passes_in_draw_order_, RenderPassId(1, 0),
            viewport_rect, gfx::Transform());
        root_pass->has_transparent_background = false;

        overlay_quad = root_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        overlay_quad->SetNew(root_pass->CreateAndAppendSharedQuadState(),
            viewport_rect, viewport_rect, viewport_rect, resource_id,
            premultiplied_alpha, gfx::PointF(0, 0),
            gfx::PointF(1, 1), SK_ColorTRANSPARENT, vertex_opacity,
            flipped, nearest_neighbor);

        EXPECT_CALL(*processor->strategy_, Attempt(_, _, _)).Times(1);
        renderer.DrawFrame(&render_passes_in_draw_order_, 1.f, viewport_rect,
            viewport_rect, false);
    }

    class SingleOverlayOnTopProcessor : public OverlayProcessor {
    public:
        class SingleOverlayValidator : public OverlayCandidateValidator {
        public:
            void GetStrategies(OverlayProcessor::StrategyList* strategies) override
            {
                strategies->push_back(scoped_ptr<OverlayProcessor::Strategy>(
                    new OverlayStrategyCommon(this, new OverlayStrategySingleOnTop)));
                strategies->push_back(scoped_ptr<OverlayProcessor::Strategy>(
                    new OverlayStrategyCommon(this, new OverlayStrategyUnderlay)));
            }

            void CheckOverlaySupport(OverlayCandidateList* surfaces) override
            {
                ASSERT_EQ(1U, surfaces->size());
                OverlayCandidate& candidate = surfaces->back();
                candidate.overlay_handled = true;
            }
        };

        explicit SingleOverlayOnTopProcessor(OutputSurface* surface)
            : OverlayProcessor(surface)
        {
        }

        void Initialize() override
        {
            strategies_.push_back(scoped_ptr<Strategy>(new OverlayStrategyCommon(
                &validator_, new OverlayStrategySingleOnTop)));
        }

        SingleOverlayValidator validator_;
    };

    class WaitSyncPointCountingContext : public TestWebGraphicsContext3D {
    public:
        MOCK_METHOD1(waitSyncPoint, void(unsigned sync_point));
    };

    class MockOverlayScheduler {
    public:
        MOCK_METHOD5(Schedule,
            void(int plane_z_order,
                gfx::OverlayTransform plane_transform,
                unsigned overlay_texture_id,
                const gfx::Rect& display_bounds,
                const gfx::RectF& uv_rect));
    };

    TEST_F(GLRendererTest, OverlaySyncPointsAreProcessed)
    {
        scoped_ptr<WaitSyncPointCountingContext> context_owned(
            new WaitSyncPointCountingContext);
        WaitSyncPointCountingContext* context = context_owned.get();

        MockOverlayScheduler overlay_scheduler;
        scoped_refptr<TestContextProvider> context_provider = TestContextProvider::Create(context_owned.Pass());
        context_provider->support()->SetScheduleOverlayPlaneCallback(base::Bind(
            &MockOverlayScheduler::Schedule, base::Unretained(&overlay_scheduler)));

        FakeOutputSurfaceClient output_surface_client;
        scoped_ptr<OutputSurface> output_surface(
            FakeOutputSurface::Create3d(context_provider));
        CHECK(output_surface->BindToClient(&output_surface_client));

        scoped_ptr<SharedBitmapManager> shared_bitmap_manager(
            new TestSharedBitmapManager());
        scoped_ptr<ResourceProvider> resource_provider = FakeResourceProvider::Create(
            output_surface.get(), shared_bitmap_manager.get());
        scoped_ptr<TextureMailboxDeleter> mailbox_deleter(
            new TextureMailboxDeleter(base::ThreadTaskRunnerHandle::Get()));

        RendererSettings settings;
        FakeRendererClient renderer_client;
        FakeRendererGL renderer(&renderer_client, &settings, output_surface.get(),
            resource_provider.get(), mailbox_deleter.get());

        SingleOverlayOnTopProcessor* processor = new SingleOverlayOnTopProcessor(output_surface.get());
        processor->Initialize();
        renderer.SetOverlayProcessor(processor);

        gfx::Rect viewport_rect(1, 1);
        RenderPass* root_pass = AddRenderPass(&render_passes_in_draw_order_, RenderPassId(1, 0),
            viewport_rect, gfx::Transform());
        root_pass->has_transparent_background = false;

        unsigned sync_point = 29;
        TextureMailbox mailbox = TextureMailbox(gpu::Mailbox::Generate(), GL_TEXTURE_2D, sync_point);
        scoped_ptr<SingleReleaseCallbackImpl> release_callback = SingleReleaseCallbackImpl::Create(base::Bind(&MailboxReleased));
        ResourceId resource_id = resource_provider->CreateResourceFromTextureMailbox(
            mailbox, release_callback.Pass());
        bool premultiplied_alpha = false;
        bool flipped = false;
        bool nearest_neighbor = false;
        float vertex_opacity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        gfx::PointF uv_top_left(0, 0);
        gfx::PointF uv_bottom_right(1, 1);

        TextureDrawQuad* overlay_quad = root_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        SharedQuadState* shared_state = root_pass->CreateAndAppendSharedQuadState();
        shared_state->SetAll(gfx::Transform(), viewport_rect.size(), viewport_rect,
            viewport_rect, false, 1, SkXfermode::kSrcOver_Mode, 0);
        overlay_quad->SetNew(shared_state, viewport_rect, viewport_rect,
            viewport_rect, resource_id, premultiplied_alpha,
            uv_top_left, uv_bottom_right, SK_ColorTRANSPARENT,
            vertex_opacity, flipped, nearest_neighbor);
        overlay_quad->set_allow_overlay(true);

        // Verify that overlay_quad actually gets turned into an overlay, and even
        // though it's not drawn, that its sync point is waited on.
        EXPECT_CALL(*context, waitSyncPoint(sync_point)).Times(1);
        EXPECT_CALL(overlay_scheduler,
            Schedule(1, gfx::OVERLAY_TRANSFORM_NONE, _, viewport_rect,
                BoundingRect(uv_top_left, uv_bottom_right)))
            .Times(1);

        renderer.DrawFrame(&render_passes_in_draw_order_, 1.f, viewport_rect,
            viewport_rect, false);
    }

} // namespace
} // namespace cc
