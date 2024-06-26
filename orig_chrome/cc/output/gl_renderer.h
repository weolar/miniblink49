// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_GL_RENDERER_H_
#define CC_OUTPUT_GL_RENDERER_H_

#include "base/cancelable_callback.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_deque.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/output/direct_renderer.h"
#include "cc/output/gl_renderer_draw_cache.h"
#include "cc/output/program_binding.h"
#include "cc/output/renderer.h"
#include "cc/quads/debug_border_draw_quad.h"
#include "cc/quads/io_surface_draw_quad.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/tile_draw_quad.h"
#include "cc/quads/yuv_video_draw_quad.h"
#include "ui/gfx/geometry/quad_f.h"

class SkBitmap;

namespace gpu {
namespace gles2 {
    class GLES2Interface;
}
}

namespace cc {

class GLRendererShaderTest;
class OutputSurface;
class PictureDrawQuad;
class ScopedResource;
class StreamVideoDrawQuad;
class TextureDrawQuad;
class TextureMailboxDeleter;
class StaticGeometryBinding;
class DynamicGeometryBinding;
class ScopedEnsureFramebufferAllocation;

// Class that handles drawing of composited render layers using GL.
class CC_EXPORT GLRenderer : public DirectRenderer {
public:
    class ScopedUseGrContext;

    static scoped_ptr<GLRenderer> Create(
        RendererClient* client,
        const RendererSettings* settings,
        OutputSurface* output_surface,
        ResourceProvider* resource_provider,
        TextureMailboxDeleter* texture_mailbox_deleter,
        int highp_threshold_min);

    ~GLRenderer() override;

    const RendererCapabilitiesImpl& Capabilities() const override;

    // Waits for rendering to finish.
    void Finish() override;

    void SwapBuffers(const CompositorFrameMetadata& metadata) override;

    virtual bool IsContextLost();

protected:
    GLRenderer(RendererClient* client,
        const RendererSettings* settings,
        OutputSurface* output_surface,
        ResourceProvider* resource_provider,
        TextureMailboxDeleter* texture_mailbox_deleter,
        int highp_threshold_min);

    void DidChangeVisibility() override;

    bool IsBackbufferDiscarded() const { return is_backbuffer_discarded_; }

    const gfx::QuadF& SharedGeometryQuad() const { return shared_geometry_quad_; }
    const StaticGeometryBinding* SharedGeometry() const
    {
        return shared_geometry_.get();
    }

    void GetFramebufferPixelsAsync(const DrawingFrame* frame,
        const gfx::Rect& rect,
        scoped_ptr<CopyOutputRequest> request);
    void GetFramebufferTexture(unsigned texture_id,
        ResourceFormat texture_format,
        const gfx::Rect& device_rect);
    void ReleaseRenderPassTextures();
    enum BoundGeometry { NO_BINDING,
        SHARED_BINDING,
        CLIPPED_BINDING };
    void PrepareGeometry(BoundGeometry geometry_to_bind);
    void SetStencilEnabled(bool enabled);
    bool stencil_enabled() const { return stencil_shadow_; }
    void SetBlendEnabled(bool enabled);
    bool blend_enabled() const { return blend_shadow_; }

    void BindFramebufferToOutputSurface(DrawingFrame* frame) override;
    bool BindFramebufferToTexture(DrawingFrame* frame,
        const ScopedResource* resource,
        const gfx::Rect& target_rect) override;
    void SetScissorTestRect(const gfx::Rect& scissor_rect) override;
    void PrepareSurfaceForPass(DrawingFrame* frame,
        SurfaceInitializationMode initialization_mode,
        const gfx::Rect& render_pass_scissor) override;
    void DoDrawQuad(DrawingFrame* frame,
        const class DrawQuad*,
        const gfx::QuadF* draw_region) override;
    void BeginDrawingFrame(DrawingFrame* frame) override;
    void FinishDrawingFrame(DrawingFrame* frame) override;
    bool FlippedFramebuffer(const DrawingFrame* frame) const override;
    bool FlippedRootFramebuffer() const;
    void EnsureScissorTestEnabled() override;
    void EnsureScissorTestDisabled() override;
    void CopyCurrentRenderPassToBitmap(
        DrawingFrame* frame,
        scoped_ptr<CopyOutputRequest> request) override;
    void FinishDrawingQuadList() override;

    // Returns true if quad requires antialiasing and false otherwise.
    static bool ShouldAntialiasQuad(const gfx::QuadF& device_layer_quad,
        bool clipped,
        bool force_aa);

    // Inflate the quad and fill edge array for fragment shader.
    // |local_quad| is set to inflated quad. |edge| array is filled with
    // inflated quad's edge data.
    static void SetupQuadForClippingAndAntialiasing(
        const gfx::Transform& device_transform,
        const DrawQuad* quad,
        const gfx::QuadF* device_layer_quad,
        const gfx::QuadF* clip_region,
        gfx::QuadF* local_quad,
        float edge[24]);
    static void SetupRenderPassQuadForClippingAndAntialiasing(
        const gfx::Transform& device_transform,
        const RenderPassDrawQuad* quad,
        const gfx::QuadF* device_layer_quad,
        const gfx::QuadF* clip_region,
        gfx::QuadF* local_quad,
        float edge[24]);

private:
    friend class GLRendererShaderPixelTest;
    friend class GLRendererShaderTest;

    static void ToGLMatrix(float* gl_matrix, const gfx::Transform& transform);

    void DiscardPixels();
    void ClearFramebuffer(DrawingFrame* frame);
    void SetViewport();

    void DrawDebugBorderQuad(const DrawingFrame* frame,
        const DebugBorderDrawQuad* quad);
    static bool IsDefaultBlendMode(SkXfermode::Mode blend_mode)
    {
        return blend_mode == SkXfermode::kSrcOver_Mode;
    }
    bool CanApplyBlendModeUsingBlendFunc(SkXfermode::Mode blend_mode);
    void ApplyBlendModeUsingBlendFunc(SkXfermode::Mode blend_mode);
    void RestoreBlendFuncToDefault(SkXfermode::Mode blend_mode);

    gfx::Rect GetBackdropBoundingBoxForRenderPassQuad(
        DrawingFrame* frame,
        const RenderPassDrawQuad* quad,
        const gfx::Transform& contents_device_transform,
        const gfx::QuadF* clip_region,
        bool use_aa);
    scoped_ptr<ScopedResource> GetBackdropTexture(const gfx::Rect& bounding_rect);

    static bool ShouldApplyBackgroundFilters(const RenderPassDrawQuad* quad);
    skia::RefPtr<SkImage> ApplyBackgroundFilters(
        DrawingFrame* frame,
        const RenderPassDrawQuad* quad,
        ScopedResource* background_texture);

    void DrawRenderPassQuad(DrawingFrame* frame,
        const RenderPassDrawQuad* quadi,
        const gfx::QuadF* clip_region);
    void DrawSolidColorQuad(const DrawingFrame* frame,
        const SolidColorDrawQuad* quad,
        const gfx::QuadF* clip_region);
    void DrawStreamVideoQuad(const DrawingFrame* frame,
        const StreamVideoDrawQuad* quad,
        const gfx::QuadF* clip_region);
    void DrawTextureQuad(const DrawingFrame* frame,
        const TextureDrawQuad* quad,
        const gfx::QuadF* clip_region);
    void EnqueueTextureQuad(const DrawingFrame* frame,
        const TextureDrawQuad* quad,
        const gfx::QuadF* clip_region);
    void FlushTextureQuadCache(BoundGeometry flush_binding);
    void DrawIOSurfaceQuad(const DrawingFrame* frame,
        const IOSurfaceDrawQuad* quad,
        const gfx::QuadF* clip_region);
    void DrawTileQuad(const DrawingFrame* frame,
        const TileDrawQuad* quad,
        const gfx::QuadF* clip_region);
    void DrawContentQuad(const DrawingFrame* frame,
        const ContentDrawQuadBase* quad,
        ResourceId resource_id,
        const gfx::QuadF* clip_region);
    void DrawContentQuadAA(const DrawingFrame* frame,
        const ContentDrawQuadBase* quad,
        ResourceId resource_id,
        const gfx::Transform& device_transform,
        const gfx::QuadF& aa_quad,
        const gfx::QuadF* clip_region);
    void DrawContentQuadNoAA(const DrawingFrame* frame,
        const ContentDrawQuadBase* quad,
        ResourceId resource_id,
        const gfx::QuadF* clip_region);
    void DrawYUVVideoQuad(const DrawingFrame* frame,
        const YUVVideoDrawQuad* quad,
        const gfx::QuadF* clip_region);
    void DrawPictureQuad(const DrawingFrame* frame,
        const PictureDrawQuad* quad,
        const gfx::QuadF* clip_region);

    void SetShaderOpacity(float opacity, int alpha_location);
    void SetShaderQuadF(const gfx::QuadF& quad, int quad_location);
    void DrawQuadGeometryClippedByQuadF(const DrawingFrame* frame,
        const gfx::Transform& draw_transform,
        const gfx::RectF& quad_rect,
        const gfx::QuadF& clipping_region_quad,
        int matrix_location,
        const float uv[8]);
    void DrawQuadGeometry(const DrawingFrame* frame,
        const gfx::Transform& draw_transform,
        const gfx::RectF& quad_rect,
        int matrix_location);
    void SetUseProgram(unsigned program);

    bool UseScopedTexture(DrawingFrame* frame,
        const ScopedResource* resource,
        const gfx::Rect& viewport_rect);

    bool MakeContextCurrent();

    void InitializeSharedObjects();
    void CleanupSharedObjects();

    typedef base::Callback<void(scoped_ptr<CopyOutputRequest> copy_request,
        bool success)>
        AsyncGetFramebufferPixelsCleanupCallback;
    void FinishedReadback(unsigned source_buffer,
        unsigned query,
        const gfx::Size& size);

    void ReinitializeGLState();
    void RestoreGLState();
    void RestoreFramebuffer(DrawingFrame* frame);

    void DiscardBackbuffer() override;
    void EnsureBackbuffer() override;
    void EnforceMemoryPolicy();

    void ScheduleOverlays(DrawingFrame* frame);

#ifndef NOT_QB_UI
    // Added by yinzhou
    void CopyTextureToSoftwareDeviceIfNeeded(DrawingFrame* frame);
#endif //NOT_QB_UI

    typedef ScopedPtrVector<ResourceProvider::ScopedReadLockGL>
        OverlayResourceLockList;
    OverlayResourceLockList pending_overlay_resources_;
    OverlayResourceLockList in_use_overlay_resources_;
    OverlayResourceLockList previous_swap_overlay_resources_;

    RendererCapabilitiesImpl capabilities_;

    unsigned offscreen_framebuffer_id_;

    scoped_ptr<StaticGeometryBinding> shared_geometry_;
    scoped_ptr<DynamicGeometryBinding> clipped_geometry_;
    gfx::QuadF shared_geometry_quad_;

    // This block of bindings defines all of the programs used by the compositor
    // itself.  Add any new programs here to GLRendererShaderTest.

    // Tiled layer shaders.
    typedef ProgramBinding<VertexShaderTile, FragmentShaderRGBATexAlpha>
        TileProgram;
    typedef ProgramBinding<VertexShaderTileAA, FragmentShaderRGBATexClampAlphaAA>
        TileProgramAA;
    typedef ProgramBinding<VertexShaderTileAA,
        FragmentShaderRGBATexClampSwizzleAlphaAA>
        TileProgramSwizzleAA;
    typedef ProgramBinding<VertexShaderTile, FragmentShaderRGBATexOpaque>
        TileProgramOpaque;
    typedef ProgramBinding<VertexShaderTile, FragmentShaderRGBATexSwizzleAlpha>
        TileProgramSwizzle;
    typedef ProgramBinding<VertexShaderTile, FragmentShaderRGBATexSwizzleOpaque>
        TileProgramSwizzleOpaque;

    // Texture shaders.
    typedef ProgramBinding<VertexShaderPosTexTransform,
        FragmentShaderRGBATexVaryingAlpha>
        TextureProgram;
    typedef ProgramBinding<VertexShaderPosTexTransform,
        FragmentShaderRGBATexPremultiplyAlpha>
        NonPremultipliedTextureProgram;
    typedef ProgramBinding<VertexShaderPosTexTransform,
        FragmentShaderTexBackgroundVaryingAlpha>
        TextureBackgroundProgram;
    typedef ProgramBinding<VertexShaderPosTexTransform,
        FragmentShaderTexBackgroundPremultiplyAlpha>
        NonPremultipliedTextureBackgroundProgram;

    // Render surface shaders.
    typedef ProgramBinding<VertexShaderPosTexTransform,
        FragmentShaderRGBATexAlpha>
        RenderPassProgram;
    typedef ProgramBinding<VertexShaderPosTexTransform,
        FragmentShaderRGBATexAlphaMask>
        RenderPassMaskProgram;
    typedef ProgramBinding<VertexShaderQuadTexTransformAA,
        FragmentShaderRGBATexAlphaAA>
        RenderPassProgramAA;
    typedef ProgramBinding<VertexShaderQuadTexTransformAA,
        FragmentShaderRGBATexAlphaMaskAA>
        RenderPassMaskProgramAA;
    typedef ProgramBinding<VertexShaderPosTexTransform,
        FragmentShaderRGBATexColorMatrixAlpha>
        RenderPassColorMatrixProgram;
    typedef ProgramBinding<VertexShaderQuadTexTransformAA,
        FragmentShaderRGBATexAlphaMaskColorMatrixAA>
        RenderPassMaskColorMatrixProgramAA;
    typedef ProgramBinding<VertexShaderQuadTexTransformAA,
        FragmentShaderRGBATexAlphaColorMatrixAA>
        RenderPassColorMatrixProgramAA;
    typedef ProgramBinding<VertexShaderPosTexTransform,
        FragmentShaderRGBATexAlphaMaskColorMatrix>
        RenderPassMaskColorMatrixProgram;

    // Video shaders.
    typedef ProgramBinding<VertexShaderVideoTransform, FragmentShaderRGBATex>
        VideoStreamTextureProgram;
    typedef ProgramBinding<VertexShaderPosTexYUVStretchOffset,
        FragmentShaderYUVVideo>
        VideoYUVProgram;
    typedef ProgramBinding<VertexShaderPosTexYUVStretchOffset,
        FragmentShaderYUVAVideo>
        VideoYUVAProgram;

    // Special purpose / effects shaders.
    typedef ProgramBinding<VertexShaderPos, FragmentShaderColor>
        DebugBorderProgram;
    typedef ProgramBinding<VertexShaderQuad, FragmentShaderColor>
        SolidColorProgram;
    typedef ProgramBinding<VertexShaderQuadAA, FragmentShaderColorAA>
        SolidColorProgramAA;

    const TileProgram* GetTileProgram(
        TexCoordPrecision precision, SamplerType sampler);
    const TileProgramOpaque* GetTileProgramOpaque(
        TexCoordPrecision precision, SamplerType sampler);
    const TileProgramAA* GetTileProgramAA(
        TexCoordPrecision precision, SamplerType sampler);
    const TileProgramSwizzle* GetTileProgramSwizzle(
        TexCoordPrecision precision, SamplerType sampler);
    const TileProgramSwizzleOpaque* GetTileProgramSwizzleOpaque(
        TexCoordPrecision precision, SamplerType sampler);
    const TileProgramSwizzleAA* GetTileProgramSwizzleAA(
        TexCoordPrecision precision, SamplerType sampler);

    const RenderPassProgram* GetRenderPassProgram(TexCoordPrecision precision,
        BlendMode blend_mode);
    const RenderPassProgramAA* GetRenderPassProgramAA(TexCoordPrecision precision,
        BlendMode blend_mode);
    const RenderPassMaskProgram* GetRenderPassMaskProgram(
        TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode,
        bool mask_for_background);
    const RenderPassMaskProgramAA* GetRenderPassMaskProgramAA(
        TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode,
        bool mask_for_background);
    const RenderPassColorMatrixProgram* GetRenderPassColorMatrixProgram(
        TexCoordPrecision precision,
        BlendMode blend_mode);
    const RenderPassColorMatrixProgramAA* GetRenderPassColorMatrixProgramAA(
        TexCoordPrecision precision,
        BlendMode blend_mode);
    const RenderPassMaskColorMatrixProgram* GetRenderPassMaskColorMatrixProgram(
        TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode,
        bool mask_for_background);
    const RenderPassMaskColorMatrixProgramAA*
    GetRenderPassMaskColorMatrixProgramAA(TexCoordPrecision precision,
        SamplerType sampler,
        BlendMode blend_mode,
        bool mask_for_background);

    const TextureProgram* GetTextureProgram(TexCoordPrecision precision,
        SamplerType sampler);
    const NonPremultipliedTextureProgram* GetNonPremultipliedTextureProgram(
        TexCoordPrecision precision,
        SamplerType sampler);
    const TextureBackgroundProgram* GetTextureBackgroundProgram(
        TexCoordPrecision precision,
        SamplerType sampler);
    const NonPremultipliedTextureBackgroundProgram*
    GetNonPremultipliedTextureBackgroundProgram(TexCoordPrecision precision,
        SamplerType sampler);
    const TextureProgram* GetTextureIOSurfaceProgram(
        TexCoordPrecision precision);

    const VideoYUVProgram* GetVideoYUVProgram(TexCoordPrecision precision,
        SamplerType sampler);
    const VideoYUVAProgram* GetVideoYUVAProgram(TexCoordPrecision precision,
        SamplerType sampler);
    const VideoStreamTextureProgram* GetVideoStreamTextureProgram(
        TexCoordPrecision precision);

    const DebugBorderProgram* GetDebugBorderProgram();
    const SolidColorProgram* GetSolidColorProgram();
    const SolidColorProgramAA* GetSolidColorProgramAA();

    TileProgram
        tile_program_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    TileProgramOpaque
        tile_program_opaque_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    TileProgramAA
        tile_program_aa_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    TileProgramSwizzle tile_program_swizzle_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    TileProgramSwizzleOpaque
        tile_program_swizzle_opaque_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    TileProgramSwizzleAA tile_program_swizzle_aa_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];

    TextureProgram
        texture_program_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    NonPremultipliedTextureProgram
        nonpremultiplied_texture_program_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    TextureBackgroundProgram
        texture_background_program_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    NonPremultipliedTextureBackgroundProgram
        nonpremultiplied_texture_background_program_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    TextureProgram texture_io_surface_program_[LAST_TEX_COORD_PRECISION + 1];

    RenderPassProgram
        render_pass_program_[LAST_TEX_COORD_PRECISION + 1][LAST_BLEND_MODE + 1];
    RenderPassProgramAA render_pass_program_aa_[LAST_TEX_COORD_PRECISION + 1][LAST_BLEND_MODE + 1];
    RenderPassMaskProgram
        render_pass_mask_program_[LAST_TEX_COORD_PRECISION + 1]
                                 [LAST_SAMPLER_TYPE + 1]
                                 [LAST_BLEND_MODE + 1]
                                 [LAST_MASK_VALUE + 1];
    RenderPassMaskProgramAA
        render_pass_mask_program_aa_[LAST_TEX_COORD_PRECISION + 1]
                                    [LAST_SAMPLER_TYPE + 1]
                                    [LAST_BLEND_MODE + 1]
                                    [LAST_MASK_VALUE + 1];
    RenderPassColorMatrixProgram
        render_pass_color_matrix_program_[LAST_TEX_COORD_PRECISION + 1][LAST_BLEND_MODE + 1];
    RenderPassColorMatrixProgramAA
        render_pass_color_matrix_program_aa_[LAST_TEX_COORD_PRECISION + 1][LAST_BLEND_MODE + 1];
    RenderPassMaskColorMatrixProgram
        render_pass_mask_color_matrix_program_[LAST_TEX_COORD_PRECISION + 1]
                                              [LAST_SAMPLER_TYPE + 1]
                                              [LAST_BLEND_MODE + 1]
                                              [LAST_MASK_VALUE + 1];
    RenderPassMaskColorMatrixProgramAA
        render_pass_mask_color_matrix_program_aa_[LAST_TEX_COORD_PRECISION + 1]
                                                 [LAST_SAMPLER_TYPE + 1]
                                                 [LAST_BLEND_MODE + 1]
                                                 [LAST_MASK_VALUE + 1];

    VideoYUVProgram
        video_yuv_program_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    VideoYUVAProgram
        video_yuva_program_[LAST_TEX_COORD_PRECISION + 1][LAST_SAMPLER_TYPE + 1];
    VideoStreamTextureProgram
        video_stream_texture_program_[LAST_TEX_COORD_PRECISION + 1];

    DebugBorderProgram debug_border_program_;
    SolidColorProgram solid_color_program_;
    SolidColorProgramAA solid_color_program_aa_;

    gpu::gles2::GLES2Interface* gl_;
    gpu::ContextSupport* context_support_;

    TextureMailboxDeleter* texture_mailbox_deleter_;

    gfx::Rect swap_buffer_rect_;
    gfx::Rect scissor_rect_;
    bool is_backbuffer_discarded_;
    bool is_using_bind_uniform_;
    bool is_scissor_enabled_;
    bool scissor_rect_needs_reset_;
    bool stencil_shadow_;
    bool blend_shadow_;
    unsigned program_shadow_;
    TexturedQuadDrawCache draw_cache_;
    int highp_threshold_min_;
    int highp_threshold_cache_;

    struct PendingAsyncReadPixels;
    ScopedPtrVector<PendingAsyncReadPixels> pending_async_read_pixels_;

    scoped_ptr<ResourceProvider::ScopedWriteLockGL> current_framebuffer_lock_;

    class SyncQuery;
    ScopedPtrDeque<SyncQuery> pending_sync_queries_;
    ScopedPtrDeque<SyncQuery> available_sync_queries_;
    scoped_ptr<SyncQuery> current_sync_query_;
    bool use_sync_query_;
    bool use_blend_equation_advanced_;
    bool use_blend_equation_advanced_coherent_;

    SkBitmap on_demand_tile_raster_bitmap_;
    ResourceId on_demand_tile_raster_resource_id_;
    BoundGeometry bound_geometry_;
    DISALLOW_COPY_AND_ASSIGN(GLRenderer);

#if !defined(NOT_QB_AERO)
private:
    // Copy the bitmap of compisited web page and send to QBBrowser frame window.
    void TransferAeroEffectBackgroundBitmapIfNeeded(DrawingFrame* frame);
#endif // NOT_QB_AERO
};

} // namespace cc

#endif // CC_OUTPUT_GL_RENDERER_H_
