
#include "content/compositor/GpuOutputSurface.h"

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

cc::SurfaceManager* GpuOutputSurface::m_surfaceManager = nullptr;
static uint32_t s_nextSurfaceIdNamespace = 1u;

// class DelegatedFrameHost :
//     public cc::DelegatedFrameResourceCollectionClient,
//     public cc::SurfaceFactoryClient,
//     public base::SupportsWeakPtr<DelegatedFrameHost> {
//
//     DelegatedFrameHost()
//     {
//         id_allocator_ = createSurfaceIdAllocator();
//     }
//
//
//     // cc::DelegatedFrameProviderClient implementation.
//     virtual void UnusedResourcesAreAvailable() override
//     {
//
//     }
//
//     // cc::SurfaceFactoryClient implementation.
//     virtual void ReturnResources(const cc::ReturnedResourceArray& resources) override
//     {
//
//     }
//
//     virtual void SetBeginFrameSource(cc::SurfaceId surface_id, cc::BeginFrameSource* begin_frame_source) override
//     {
//
//     }
//
// private:
//
//     // Holds delegated resources that have been given to a DelegatedFrameProvider,
//     // and gives back resources when they are no longer in use for return to the
//     // renderer.
//     scoped_refptr<cc::DelegatedFrameResourceCollection> resource_collection_;
//
//     // Provides delegated frame updates to the cc::DelegatedRendererLayer.
//     scoped_refptr<cc::DelegatedFrameProvider> frame_provider_;
// };

static scoped_ptr<cc::SurfaceIdAllocator> createSurfaceIdAllocator()
{
    scoped_ptr<cc::SurfaceIdAllocator> allocator = make_scoped_ptr(new cc::SurfaceIdAllocator(s_nextSurfaceIdNamespace++));
    if (GpuOutputSurface::m_surfaceManager)
        allocator->RegisterSurfaceIdNamespace(GpuOutputSurface::m_surfaceManager);
    return allocator;
}

GpuOutputSurface::GpuOutputSurface(
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

    if (!m_surfaceManager)
        m_surfaceManager = new cc::SurfaceManager();

    if (!surface_factory_) {
        surface_factory_ = make_scoped_ptr(new cc::SurfaceFactory(m_surfaceManager, this));
    }

    id_allocator_ = createSurfaceIdAllocator();

    scoped_refptr<base::SingleThreadTaskRunner> task_runner = OrigChromeMgr::getInst()->getBlinkLoop()->task_runner();
    scoped_ptr<OutputSurface> outputSurface(this);

    m_displayClient = make_scoped_ptr(new cc::OnscreenDisplayClient(
        outputSurface.Pass(), m_surfaceManager,
        sharedBitmapManager, gpuMemoryBufferManager,
        rendererSettings, task_runner));

    m_displayOutputSurface.reset(new cc::SurfaceDisplayOutputSurface(m_surfaceManager, id_allocator_.get(), contextProvider, workerContextProvider));
    m_displayClient->set_surface_output_surface(m_displayOutputSurface.get());
    m_displayOutputSurface->set_display_client(m_displayClient.get());
    //m_displayClient->display()->Resize(compositor->size());
}

GpuOutputSurface::~GpuOutputSurface()
{
}

bool GpuOutputSurface::BindToClient(cc::OutputSurfaceClient* client)
{
    if (!client_)
        m_displayClient->Initialize();

    if (!OutputSurface::BindToClient(client))
        return false;
    return true;
}

static void swapAckStub(GpuOutputSurface* self, cc::CompositorFrame* frame)
{
    self->onSwapAck(frame);
}

void GpuOutputSurface::SwapBuffers(cc::CompositorFrame* frame)
{
    //     if (frame->gl_frame_data->sub_buffer_rect == gfx::Rect(frame->gl_frame_data->size)) {
    //         context_provider_->ContextSupport()->Swap();
    //     } else {
    //         context_provider_->ContextSupport()->PartialSwapBuffers(frame->gl_frame_data->sub_buffer_rect);
    //     }

    //DCHECK(!frame->delegated_frame_data);

    cc::CompositorFrame* frameCopy = new cc::CompositorFrame();
    frame->AssignTo(frameCopy);
    base::Closure closure = base::Bind(&swapAckStub, this, frameCopy);

    if (context_provider()) {
        gpu::gles2::GLES2Interface* context = context_provider()->ContextGL();
        context->Flush();
        uint32 sync_point = context->InsertSyncPointCHROMIUM();
        context_provider()->ContextSupport()->SignalSyncPoint(sync_point, closure);
    } else {
        //base::ThreadTaskRunnerHandle::Get()->PostTask(FROM_HERE, closure);
    }

    client_->DidSwapBuffers();
}

bool ShouldSkipFrame2(gfx::Size size_in_dip)
{
    //     // Should skip a frame only when another frame from the renderer is guaranteed
    //     // to replace it. Otherwise may cause hangs when the renderer is waiting for
    //     // the completion of latency infos (such as when taking a Snapshot.)
    //     if (can_lock_compositor_ == NO_PENDING_RENDERER_FRAME ||
    //         can_lock_compositor_ == NO_PENDING_COMMIT ||
    //         !resize_lock_.get())
    //         return false;
    //
    //     return size_in_dip != resize_lock_->expected_size();
    return false;
}

static void surfaceDrawn(GpuOutputSurface* self, cc::SurfaceDrawStatus state)
{
    self->surfaceDrawnImpl();
}

void GpuOutputSurface::onSwapAck(cc::CompositorFrame* frame)
{
    cc::DelegatedFrameData* frame_data = frame->delegated_frame_data.get();
    float frame_device_scale_factor = frame->metadata.device_scale_factor;

    //     gfx::Size frame_size = frame->gl_frame_data->size;

    cc::RenderPass* root_pass = frame_data->render_pass_list.back();
    gfx::Size frame_size = root_pass->output_rect.size();

    gfx::Rect damage_rect = root_pass->damage_rect;
    damage_rect.Intersect(gfx::Rect(frame_size));
    gfx::Rect damage_rect_in_dip = gfx::ConvertRectToDIP(frame_device_scale_factor, damage_rect);
    gfx::Size frame_size_in_dip = gfx::ConvertSizeToDIP(frame_device_scale_factor, frame_size);

    if (ShouldSkipFrame2(frame_size_in_dip)) {
        //         cc::CompositorFrameAck ack;
        //         cc::TransferableResource::ReturnResources(frame_data->resource_list, &ack.resources);
        //
        //         skipped_latency_info_list_.insert(skipped_latency_info_list_.end(), latency_info.begin(), latency_info.end());
        //         client_->DelegatedFrameHostSendCompositorSwapAck(output_surface_id, ack);
        skipped_frames_ = true;
        return;
    }

    cc::SurfaceManager* manager = m_surfaceManager;
    if (!surface_factory_) {
        surface_factory_ = make_scoped_ptr(new cc::SurfaceFactory(manager, this));
    }
    if (surface_id_.is_null() || frame_size != current_surface_size_ || frame_size_in_dip != current_frame_size_in_dip_) {
        if (!surface_id_.is_null())
            surface_factory_->Destroy(surface_id_);
        surface_id_ = id_allocator_->GenerateId();
        surface_factory_->Create(surface_id_);
        // manager must outlive compositors using it.
        //         client_->DelegatedFrameHostGetLayer()->SetShowSurface(
        //             surface_id_,
        //             base::Bind(&SatisfyCallback, base::Unretained(manager)),
        //             base::Bind(&RequireCallback, base::Unretained(manager)), frame_size,
        //             frame_device_scale_factor, frame_size_in_dip);
        current_surface_size_ = frame_size;
        current_scale_factor_ = frame_device_scale_factor;
    }
    //     scoped_ptr<cc::CompositorFrame> compositor_frame = make_scoped_ptr(new cc::CompositorFrame());
    //     compositor_frame->delegated_frame_data = frame_data.Pass();

    //     compositor_frame->metadata.latency_info.swap(skipped_latency_info_list_);
    //     compositor_frame->metadata.latency_info.insert(compositor_frame->metadata.latency_info.end(), latency_info.begin(), latency_info.end());
    //     compositor_frame->metadata.satisfies_sequences.swap(*satisfies_sequences);

    //     gfx::Size desired_size = client_->DelegatedFrameHostDesiredSizeInDIP();
    //     if (desired_size != frame_size_in_dip && !desired_size.IsEmpty())
    //         immediate_ack = true;

    current_frame_size_in_dip_ = frame_size_in_dip;

    m_reviousFrameAck.reset(new cc::CompositorFrameAck());
    cc::TransferableResource::ReturnResources(frame_data->resource_list, &m_reviousFrameAck->resources);
    m_reviousFrameAck->gl_frame_data = frame->gl_frame_data.Pass();

    cc::SurfaceFactory::DrawCallback ack_callback;
    //if (compositor_ && !immediate_ack) {
    ack_callback = base::Bind(&surfaceDrawn, this);
    //}
    surface_factory_->SubmitCompositorFrame(surface_id_, make_scoped_ptr(frame), ack_callback);
}

void GpuOutputSurface::surfaceDrawnImpl()
{
    // Ignore message if it's a stale one coming from a different output surface
    // (e.g. after a lost context).
    ReclaimResources(m_reviousFrameAck.get());
    client_->DidSwapBuffersComplete();
}

bool GpuOutputSurface::SurfaceIsSuspendForRecycle() const
{
    return false;
}

cc::OverlayCandidateValidator* GpuOutputSurface::GetOverlayCandidateValidator() const
{
    return nullptr;
}

void GpuOutputSurface::ReturnResources(const cc::ReturnedResourceArray& resources)
{
}

}