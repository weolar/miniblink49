// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_RENDERER_H_
#define CC_OUTPUT_RENDERER_H_

#include "base/basictypes.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/output/renderer_capabilities.h"
#include "cc/output/renderer_settings.h"
#include "ui/gfx/geometry/rect.h"

#if !defined(NOT_QB_AERO)
#include "ui/gfx/native_widget_types.h" // for gfx::AcceleratedWidget
#endif // NOT_QB_AERO

namespace cc {

class CompositorFrameAck;
class CompositorFrameMetadata;
class RenderPass;
class RenderPassId;
class ScopedResource;
class Task;

typedef ScopedPtrVector<RenderPass> RenderPassList;

struct RendererCapabilitiesImpl {
    RendererCapabilitiesImpl();
    ~RendererCapabilitiesImpl();

    // Capabilities copied to main thread.
    ResourceFormat best_texture_format;
    bool allow_partial_texture_updates;
    int max_texture_size;
    bool using_shared_memory_resources;

    // Capabilities used on compositor thread only.
    bool using_partial_swap;
    bool using_egl_image;
    bool using_image;
    bool using_discard_framebuffer;
    bool allow_rasterize_on_demand;
    int max_msaa_samples;

    RendererCapabilities MainThreadCapabilities() const;
};

class CC_EXPORT RendererClient {
public:
    virtual void SetFullRootLayerDamage() = 0;

#ifndef NOT_QB_AERO
    // For qb aero effect.
    // Get the target widget to apply qb aero effect;
    virtual gfx::AcceleratedWidget GetAcceleratedWidget() const
    {
        return nullptr;
    }
#endif //NOT_QB_AERO
};

class CC_EXPORT Renderer {
public:
    virtual ~Renderer() { }

    virtual const RendererCapabilitiesImpl& Capabilities() const = 0;

    virtual void DecideRenderPassAllocationsForFrame(
        const RenderPassList& render_passes_in_draw_order) { }
    virtual bool HasAllocatedResourcesForTesting(RenderPassId id) const;

    // This passes ownership of the render passes to the renderer. It should
    // consume them, and empty the list. The parameters here may change from frame
    // to frame and should not be cached.
    // The |device_viewport_rect| and |device_clip_rect| are in non-y-flipped
    // window space.
    virtual void DrawFrame(RenderPassList* render_passes_in_draw_order,
        float device_scale_factor,
        const gfx::Rect& device_viewport_rect,
        const gfx::Rect& device_clip_rect,
        bool disable_picture_quad_image_filtering)
        = 0;

    // Waits for rendering to finish.
    virtual void Finish() = 0;

    // Puts backbuffer onscreen.
    virtual void SwapBuffers(const CompositorFrameMetadata& metadata) = 0;
    virtual void ReceiveSwapBuffersAck(const CompositorFrameAck& ack) { }

    bool visible() const { return visible_; }
    void SetVisible(bool visible);

protected:
    Renderer(RendererClient* client, const RendererSettings* settings)
        : client_(client)
        , settings_(settings)
        , visible_(true)
    {
    }

    virtual void DidChangeVisibility() = 0;

    RendererClient* client_;
    const RendererSettings* settings_;
    bool visible_;

private:
    DISALLOW_COPY_AND_ASSIGN(Renderer);
};

} // namespace cc

#endif // CC_OUTPUT_RENDERER_H_
