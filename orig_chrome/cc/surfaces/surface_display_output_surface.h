// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_SURFACE_DISPLAY_OUTPUT_SURFACE_H_
#define CC_SURFACES_SURFACE_DISPLAY_OUTPUT_SURFACE_H_

#include "cc/output/output_surface.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_factory_client.h"
#include "cc/surfaces/surface_id_allocator.h"
#include "cc/surfaces/surfaces_export.h"

namespace cc {
class Display;
class OnscreenDisplayClient;
class SurfaceManager;

// This class is maps a compositor OutputSurface to the surface system's Display
// concept, allowing a compositor client to submit frames for a native root
// window or physical display.
class CC_SURFACES_EXPORT SurfaceDisplayOutputSurface
    : public OutputSurface,
      public SurfaceFactoryClient {
public:
    // The underlying Display and SurfaceManager must outlive this class.
    SurfaceDisplayOutputSurface(
        SurfaceManager* surface_manager,
        SurfaceIdAllocator* allocator,
        const scoped_refptr<ContextProvider>& context_provider,
        const scoped_refptr<ContextProvider>& worker_context_provider);
    ~SurfaceDisplayOutputSurface() override;

    void set_display_client(OnscreenDisplayClient* display_client)
    {
        display_client_ = display_client;
    }
    SurfaceFactory* factory() { return &factory_; }
    void ReceivedVSyncParameters(base::TimeTicks timebase,
        base::TimeDelta interval);

    // OutputSurface implementation.
    void SwapBuffers(CompositorFrame* frame) override;
    bool BindToClient(OutputSurfaceClient* client) override;
    void ForceReclaimResources() override;

    // SurfaceFactoryClient implementation.
    void ReturnResources(const ReturnedResourceArray& resources) override;

private:
    void SwapBuffersComplete(SurfaceDrawStatus drawn);

    OnscreenDisplayClient* display_client_;
    SurfaceFactory factory_;
    gfx::Size display_size_;
    SurfaceId surface_id_;
    SurfaceIdAllocator* allocator_;

    DISALLOW_COPY_AND_ASSIGN(SurfaceDisplayOutputSurface);
};

} // namespace cc

#endif // CC_SURFACES_SURFACE_DISPLAY_OUTPUT_SURFACE_H_
