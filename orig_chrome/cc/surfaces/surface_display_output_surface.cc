// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/surface_display_output_surface.h"

#include "base/bind.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/compositor_frame_ack.h"
#include "cc/surfaces/display.h"
#include "cc/surfaces/onscreen_display_client.h"
#include "cc/surfaces/surface.h"
#include "cc/surfaces/surface_manager.h"

namespace cc {

SurfaceDisplayOutputSurface::SurfaceDisplayOutputSurface(
    SurfaceManager* surface_manager,
    SurfaceIdAllocator* allocator,
    const scoped_refptr<ContextProvider>& context_provider,
    const scoped_refptr<ContextProvider>& worker_context_provider)
    : OutputSurface(context_provider, worker_context_provider)
    , display_client_(NULL)
    , factory_(surface_manager, this)
    , allocator_(allocator)
{
    factory_.set_needs_sync_points(false);
    capabilities_.delegated_rendering = true;
    capabilities_.max_frames_pending = 1;
    capabilities_.adjust_deadline_for_parent = true;
    capabilities_.can_force_reclaim_resources = true;
    // Display and SurfaceDisplayOutputSurface share a GL context, so sync
    // points aren't needed when passing resources between them.
    capabilities_.delegated_sync_points_required = false;
}

SurfaceDisplayOutputSurface::~SurfaceDisplayOutputSurface()
{
    client_ = NULL;
    if (!surface_id_.is_null()) {
        factory_.Destroy(surface_id_);
    }
}

void SurfaceDisplayOutputSurface::ReceivedVSyncParameters(
    base::TimeTicks timebase,
    base::TimeDelta interval)
{
    CommitVSyncParameters(timebase, interval);
}

void SurfaceDisplayOutputSurface::SwapBuffers(CompositorFrame* frame)
{
    gfx::Size frame_size = frame->delegated_frame_data->render_pass_list.back()->output_rect.size();
    if (frame_size.IsEmpty() || frame_size != display_size_) {
        if (!surface_id_.is_null()) {
            factory_.Destroy(surface_id_);
        }
        surface_id_ = allocator_->GenerateId();
        factory_.Create(surface_id_);
        display_size_ = frame_size;
    }
    display_client_->display()->SetSurfaceId(surface_id_,
        frame->metadata.device_scale_factor);

    client_->DidSwapBuffers();

    scoped_ptr<CompositorFrame> frame_copy(new CompositorFrame());
    frame->AssignTo(frame_copy.get());
    factory_.SubmitCompositorFrame(
        surface_id_, frame_copy.Pass(),
        base::Bind(&SurfaceDisplayOutputSurface::SwapBuffersComplete,
            base::Unretained(this)));
}

bool SurfaceDisplayOutputSurface::BindToClient(OutputSurfaceClient* client)
{
    DCHECK(client);
    DCHECK(display_client_);
    client_ = client;
    // Avoid initializing GL context here, as this should be sharing the
    // Display's context.
    return display_client_->Initialize();
}

void SurfaceDisplayOutputSurface::ForceReclaimResources()
{
    if (!surface_id_.is_null())
        factory_.SubmitCompositorFrame(surface_id_, nullptr,
            SurfaceFactory::DrawCallback());
}

void SurfaceDisplayOutputSurface::ReturnResources(
    const ReturnedResourceArray& resources)
{
    CompositorFrameAck ack;
    ack.resources = resources;
    if (client_)
        client_->ReclaimResources(&ack);
}

void SurfaceDisplayOutputSurface::SwapBuffersComplete(SurfaceDrawStatus drawn)
{
    if (client_ && !display_client_->output_surface_lost())
        client_->DidSwapBuffersComplete();
}

} // namespace cc
