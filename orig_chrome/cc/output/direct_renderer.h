// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_DIRECT_RENDERER_H_
#define CC_OUTPUT_DIRECT_RENDERER_H_

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/containers/scoped_ptr_hash_map.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_deque.h"
#include "cc/output/overlay_processor.h"
#include "cc/output/renderer.h"
#include "cc/raster/task_graph_runner.h"
#include "cc/resources/resource_provider.h"
#include "cc/resources/scoped_resource.h"
#include "ui/gfx/geometry/quad_f.h"

namespace cc {

class DrawPolygon;
class ResourceProvider;

// This is the base class for code shared between the GL and software
// renderer implementations.  "Direct" refers to the fact that it does not
// delegate rendering to another compositor.
class CC_EXPORT DirectRenderer : public Renderer {
public:
    ~DirectRenderer() override;

    void DecideRenderPassAllocationsForFrame(
        const RenderPassList& render_passes_in_draw_order) override;
    bool HasAllocatedResourcesForTesting(RenderPassId id) const override;
    void DrawFrame(RenderPassList* render_passes_in_draw_order,
        float device_scale_factor,
        const gfx::Rect& device_viewport_rect,
        const gfx::Rect& device_clip_rect,
        bool disable_picture_quad_image_filtering) override;

    struct CC_EXPORT DrawingFrame {
        DrawingFrame();
        ~DrawingFrame();

        const RenderPassList* render_passes_in_draw_order;
        const RenderPass* root_render_pass;
        const RenderPass* current_render_pass;
        const ScopedResource* current_texture;

        gfx::Rect root_damage_rect;
        gfx::Rect device_viewport_rect;
        gfx::Rect device_clip_rect;

        gfx::Transform projection_matrix;
        gfx::Transform window_matrix;

        bool disable_picture_quad_image_filtering;

        OverlayCandidateList overlay_list;
    };

    void SetEnlargePassTextureAmountForTesting(const gfx::Vector2d& amount);
    void DoDrawPolygon(const DrawPolygon& poly,
        DrawingFrame* frame,
        const gfx::Rect& render_pass_scissor,
        bool use_render_pass_scissor);

protected:
    enum SurfaceInitializationMode {
        SURFACE_INITIALIZATION_MODE_PRESERVE,
        SURFACE_INITIALIZATION_MODE_SCISSORED_CLEAR,
        SURFACE_INITIALIZATION_MODE_FULL_SURFACE_CLEAR,
    };

    DirectRenderer(RendererClient* client,
        const RendererSettings* settings,
        OutputSurface* output_surface,
        ResourceProvider* resource_provider);

    static gfx::RectF QuadVertexRect();
    static void QuadRectTransform(gfx::Transform* quad_rect_transform,
        const gfx::Transform& quad_transform,
        const gfx::RectF& quad_rect);
    void InitializeViewport(DrawingFrame* frame,
        const gfx::Rect& draw_rect,
        const gfx::Rect& viewport_rect,
        const gfx::Size& surface_size);
    gfx::Rect MoveFromDrawToWindowSpace(const DrawingFrame* frame,
        const gfx::Rect& draw_rect) const;

    bool NeedDeviceClip(const DrawingFrame* frame) const;
    gfx::Rect DeviceClipRectInDrawSpace(const DrawingFrame* frame) const;
    gfx::Rect DeviceViewportRectInDrawSpace(const DrawingFrame* frame) const;
    gfx::Rect OutputSurfaceRectInDrawSpace(const DrawingFrame* frame) const;
    static gfx::Rect ComputeScissorRectForRenderPass(const DrawingFrame* frame);
    void SetScissorStateForQuad(const DrawingFrame* frame,
        const DrawQuad& quad,
        const gfx::Rect& render_pass_scissor,
        bool use_render_pass_scissor);
    bool ShouldSkipQuad(const DrawQuad& quad,
        const gfx::Rect& render_pass_scissor);
    void SetScissorTestRectInDrawSpace(const DrawingFrame* frame,
        const gfx::Rect& draw_space_rect);

    static gfx::Size RenderPassTextureSize(const RenderPass* render_pass);

    void FlushPolygons(ScopedPtrDeque<DrawPolygon>* poly_list,
        DrawingFrame* frame,
        const gfx::Rect& render_pass_scissor,
        bool use_render_pass_scissor);
    void DrawRenderPass(DrawingFrame* frame, const RenderPass* render_pass);
    bool UseRenderPass(DrawingFrame* frame, const RenderPass* render_pass);

    virtual void BindFramebufferToOutputSurface(DrawingFrame* frame) = 0;
    virtual bool BindFramebufferToTexture(DrawingFrame* frame,
        const ScopedResource* resource,
        const gfx::Rect& target_rect)
        = 0;
    virtual void SetScissorTestRect(const gfx::Rect& scissor_rect) = 0;
    virtual void PrepareSurfaceForPass(
        DrawingFrame* frame,
        SurfaceInitializationMode initialization_mode,
        const gfx::Rect& render_pass_scissor)
        = 0;
    // clip_region is a (possibly null) pointer to a quad in the same
    // space as the quad. When non-null only the area of the quad that overlaps
    // with clip_region will be drawn.
    virtual void DoDrawQuad(DrawingFrame* frame,
        const DrawQuad* quad,
        const gfx::QuadF* clip_region)
        = 0;
    virtual void BeginDrawingFrame(DrawingFrame* frame) = 0;
    virtual void FinishDrawingFrame(DrawingFrame* frame) = 0;
    virtual void FinishDrawingQuadList();
    virtual bool FlippedFramebuffer(const DrawingFrame* frame) const = 0;
    virtual void EnsureScissorTestEnabled() = 0;
    virtual void EnsureScissorTestDisabled() = 0;
    virtual void DiscardBackbuffer() { }
    virtual void EnsureBackbuffer() { }

    virtual void CopyCurrentRenderPassToBitmap(
        DrawingFrame* frame,
        scoped_ptr<CopyOutputRequest> request)
        = 0;

    base::ScopedPtrHashMap<RenderPassId, scoped_ptr<ScopedResource>>
        render_pass_textures_;
    OutputSurface* output_surface_;
    ResourceProvider* resource_provider_;
    scoped_ptr<OverlayProcessor> overlay_processor_;

    // For use in coordinate conversion, this stores the output rect, viewport
    // rect (= unflipped version of glViewport rect), the size of target
    // framebuffer, and the current window space viewport. During a draw, this
    // stores the values for the current render pass; in between draws, they
    // retain the values for the root render pass of the last draw.
    gfx::Rect current_draw_rect_;
    gfx::Rect current_viewport_rect_;
    gfx::Size current_surface_size_;
    gfx::Rect current_window_space_viewport_;

private:
    gfx::Vector2d enlarge_pass_texture_amount_;

    DISALLOW_COPY_AND_ASSIGN(DirectRenderer);
};

} // namespace cc

#endif // CC_OUTPUT_DIRECT_RENDERER_H_
