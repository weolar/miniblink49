// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_SOFTWARE_RENDERER_H_
#define CC_OUTPUT_SOFTWARE_RENDERER_H_

#include "base/basictypes.h"
#include "cc/base/cc_export.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/direct_renderer.h"

namespace cc {

class OutputSurface;
class RendererClient;
class ResourceProvider;
class SoftwareOutputDevice;

class CheckerboardDrawQuad;
class DebugBorderDrawQuad;
class PictureDrawQuad;
class RenderPassDrawQuad;
class SolidColorDrawQuad;
class TextureDrawQuad;
class TileDrawQuad;

class CC_EXPORT SoftwareRenderer : public DirectRenderer {
public:
    static scoped_ptr<SoftwareRenderer> Create(
        RendererClient* client,
        const RendererSettings* settings,
        OutputSurface* output_surface,
        ResourceProvider* resource_provider);

    ~SoftwareRenderer() override;
    const RendererCapabilitiesImpl& Capabilities() const override;
    void Finish() override;
    void SwapBuffers(const CompositorFrameMetadata& metadata) override;
    void DiscardBackbuffer() override;
    void EnsureBackbuffer() override;

protected:
    void BindFramebufferToOutputSurface(DrawingFrame* frame) override;
    bool BindFramebufferToTexture(DrawingFrame* frame,
        const ScopedResource* texture,
        const gfx::Rect& target_rect) override;
    void SetScissorTestRect(const gfx::Rect& scissor_rect) override;
    void PrepareSurfaceForPass(DrawingFrame* frame,
        SurfaceInitializationMode initialization_mode,
        const gfx::Rect& render_pass_scissor) override;

    void DoDrawQuad(DrawingFrame* frame,
        const DrawQuad* quad,
        const gfx::QuadF* draw_region) override;
    void BeginDrawingFrame(DrawingFrame* frame) override;
    void FinishDrawingFrame(DrawingFrame* frame) override;
    bool FlippedFramebuffer(const DrawingFrame* frame) const override;
    void EnsureScissorTestEnabled() override;
    void EnsureScissorTestDisabled() override;
    void CopyCurrentRenderPassToBitmap(
        DrawingFrame* frame,
        scoped_ptr<CopyOutputRequest> request) override;

    SoftwareRenderer(RendererClient* client,
        const RendererSettings* settings,
        OutputSurface* output_surface,
        ResourceProvider* resource_provider);

    void DidChangeVisibility() override;

private:
    void ClearCanvas(SkColor color);
    void ClearFramebuffer(DrawingFrame* frame);
    void SetClipRect(const gfx::Rect& rect);
    bool IsSoftwareResource(ResourceId resource_id) const;

    void DrawCheckerboardQuad(const DrawingFrame* frame,
        const CheckerboardDrawQuad* quad);
    void DrawDebugBorderQuad(const DrawingFrame* frame,
        const DebugBorderDrawQuad* quad);
    void DrawPictureQuad(const DrawingFrame* frame,
        const PictureDrawQuad* quad);
    void DrawRenderPassQuad(const DrawingFrame* frame,
        const RenderPassDrawQuad* quad);
    void DrawSolidColorQuad(const DrawingFrame* frame,
        const SolidColorDrawQuad* quad);
    void DrawTextureQuad(const DrawingFrame* frame,
        const TextureDrawQuad* quad);
    void DrawTileQuad(const DrawingFrame* frame,
        const TileDrawQuad* quad);
    void DrawUnsupportedQuad(const DrawingFrame* frame,
        const DrawQuad* quad);
    bool ShouldApplyBackgroundFilters(const RenderPassDrawQuad* quad) const;
    SkBitmap ApplyImageFilter(SkImageFilter* filter,
        const RenderPassDrawQuad* quad,
        const SkBitmap* to_filter) const;
    gfx::Rect GetBackdropBoundingBoxForRenderPassQuad(
        const DrawingFrame* frame,
        const RenderPassDrawQuad* quad,
        const gfx::Transform& contents_device_transform) const;
    SkBitmap GetBackdropBitmap(const gfx::Rect& bounding_rect) const;
    skia::RefPtr<SkShader> GetBackgroundFilterShader(
        const DrawingFrame* frame,
        const RenderPassDrawQuad* quad,
        SkShader::TileMode content_tile_mode) const;

    RendererCapabilitiesImpl capabilities_;
    bool is_scissor_enabled_;
    bool is_backbuffer_discarded_;
    gfx::Rect scissor_rect_;

    SoftwareOutputDevice* output_device_;
    SkCanvas* root_canvas_;
    SkCanvas* current_canvas_;
    SkPaint current_paint_;
    scoped_ptr<ResourceProvider::ScopedWriteLockSoftware>
        current_framebuffer_lock_;
    skia::RefPtr<SkCanvas> current_framebuffer_canvas_;

    DISALLOW_COPY_AND_ASSIGN(SoftwareRenderer);
};

} // namespace cc

#endif // CC_OUTPUT_SOFTWARE_RENDERER_H_
