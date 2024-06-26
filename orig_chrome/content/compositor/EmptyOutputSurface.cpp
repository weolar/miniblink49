
#include "content/compositor/EmptyOutputSurface.h"

#include "base/bind.h"
#include "cc/blink/context_provider_web_context.h"
#include "cc/layers/delegated_frame_provider.h"
#include "cc/layers/delegated_frame_resource_collection.h"
#include "cc/output/compositor_frame_ack.h"
#include "cc/output/output_surface_client.h"
#include "cc/output/renderer_settings.h"
#include "cc/resources/shared_bitmap_manager.h"
#include "cc/scheduler/begin_frame_source.h"
#include "cc/surfaces/onscreen_display_client.h"
#include "cc/surfaces/surface_display_output_surface.h"
#include "cc/surfaces/surface_manager.h"
#include "content/OrigChromeMgr.h"
#include "gpu/command_buffer/client/context_support.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "gpu/command_buffer/client/gpu_memory_buffer_manager.h"
#include "ui/gfx/geometry/dip_util.h"

#include "content/gpu/ContextProviderCommandBuffer.h"

namespace content {

cc::SurfaceManager* EmptyOutputSurface::m_surfaceManager = nullptr;
static uint32_t s_nextSurfaceIdNamespace = 1u;

static scoped_ptr<cc::SurfaceIdAllocator> createSurfaceIdAllocator()
{
    scoped_ptr<cc::SurfaceIdAllocator> allocator = make_scoped_ptr(new cc::SurfaceIdAllocator(s_nextSurfaceIdNamespace++));
    if (EmptyOutputSurface::m_surfaceManager)
        allocator->RegisterSurfaceIdNamespace(EmptyOutputSurface::m_surfaceManager);
    return allocator;
}

EmptyOutputSurface::EmptyOutputSurface(
    const scoped_refptr<ContextProviderCommandBuffer>& contextProvider,
    const scoped_refptr<ContextProviderCommandBuffer>& workerContextProvider,
    cc::SharedBitmapManager* sharedBitmapManager,
    gpu::GpuMemoryBufferManager* gpuMemoryBufferManager,
    const cc::RendererSettings& rendererSettings)
    : OutputSurface(contextProvider, workerContextProvider)
    , m_weakPtrs(this)
{
    capabilities_.max_frames_pending = 1;
    capabilities_.adjust_deadline_for_parent = false;
    capabilities_.delegated_rendering = true;
    skipped_frames_ = false;
}

EmptyOutputSurface::~EmptyOutputSurface()
{
}

bool EmptyOutputSurface::BindToClient(cc::OutputSurfaceClient* client)
{
    if (!OutputSurface::BindToClient(client))
        return false;
    return true;
}

void EmptyOutputSurface::SwapBuffers(cc::CompositorFrame* frame)
{
    if (frame->gl_frame_data.get()) {
        if (frame->gl_frame_data->sub_buffer_rect == gfx::Rect(frame->gl_frame_data->size)) {
            context_provider_->ContextSupport()->Swap();
        } else {
            context_provider_->ContextSupport()->PartialSwapBuffers(frame->gl_frame_data->sub_buffer_rect);
        }
    }

    //DCHECK(!frame->delegated_frame_data);
    client_->DidSwapBuffers();
    client_->DidSwapBuffersComplete();
}

bool EmptyOutputSurface::SurfaceIsSuspendForRecycle() const
{
    return false;
}

cc::OverlayCandidateValidator* EmptyOutputSurface::GetOverlayCandidateValidator() const
{
    return nullptr;
}

void EmptyOutputSurface::ReturnResources(const cc::ReturnedResourceArray& resources)
{
}

}