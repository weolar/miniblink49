
#ifndef content_compositor_EmptyOutputSurface_h
#define content_compositor_EmptyOutputSurface_h

#include "base/memory/weak_ptr.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/output_surface.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_factory_client.h"
#include "cc/surfaces/surface_id_allocator.h"

namespace cc {
class SoftwareOutputDevice;
class RendererSettings;
class SharedBitmapManager;
class OnscreenDisplayClient;
class SurfaceDisplayOutputSurface;
}

namespace gfx {
enum class SwapResult;
}

namespace gpu {
class GpuMemoryBufferManager;
}

namespace content {

class ContextProviderCommandBuffer;

class EmptyOutputSurface
    : public cc::OutputSurface,
      public cc::SurfaceFactoryClient {

public:
    EmptyOutputSurface(
        const scoped_refptr<ContextProviderCommandBuffer>& contextProvider,
        const scoped_refptr<ContextProviderCommandBuffer>& workerContextProvider,
        cc::SharedBitmapManager* sharedBitmapManager,
        gpu::GpuMemoryBufferManager* gpuMemoryBufferManager,
        const cc::RendererSettings& rendererSettings);

    ~EmptyOutputSurface();

    // cc::OutputSurface implementation.
    cc::OverlayCandidateValidator* GetOverlayCandidateValidator() const override;

    virtual void SwapBuffers(cc::CompositorFrame* frame) override;
    virtual bool BindToClient(cc::OutputSurfaceClient* client) override;
    virtual bool SurfaceIsSuspendForRecycle() const override;

    // cc::OutputSurface implementation.

    // cc::SurfaceFactoryClient implementation
    virtual void ReturnResources(const cc::ReturnedResourceArray& resources) override;

    void surfaceDrawnImpl();
    void onSwapAck(cc::CompositorFrame* frame);

    static cc::SurfaceManager* m_surfaceManager;

private:
    scoped_ptr<cc::SurfaceDisplayOutputSurface> m_displayOutputSurface;
    scoped_ptr<cc::OnscreenDisplayClient> m_displayClient;

    base::WeakPtrFactory<EmptyOutputSurface> m_weakPtrs;
    scoped_ptr<cc::CompositorFrameAck> m_reviousFrameAck;

    bool skipped_frames_;
    gfx::Size current_frame_size_in_dip_;

    // State for rendering into a Surface.
    scoped_ptr<cc::SurfaceIdAllocator> id_allocator_;
    scoped_ptr<cc::SurfaceFactory> surface_factory_;
    cc::SurfaceId surface_id_;
    gfx::Size current_surface_size_;
    float current_scale_factor_;
    cc::ReturnedResourceArray surface_returned_resources_;
};

}

#endif // content_compositor_GpuOutputSurface_h
