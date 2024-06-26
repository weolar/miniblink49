// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/display.h"

#include "base/thread_task_runner_handle.h"
#include "base/trace_event/trace_event.h"
#include "cc/debug/benchmark_instrumentation.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/compositor_frame_ack.h"
#include "cc/output/direct_renderer.h"
#include "cc/output/gl_renderer.h"
#include "cc/output/renderer_settings.h"
#include "cc/output/software_renderer.h"
#include "cc/output/texture_mailbox_deleter.h"
#include "cc/surfaces/display_client.h"
#include "cc/surfaces/display_scheduler.h"
#include "cc/surfaces/surface.h"
#include "cc/surfaces/surface_aggregator.h"
#include "cc/surfaces/surface_manager.h"
#include "gpu/command_buffer/client/gles2_interface.h"
#include "ui/gfx/buffer_types.h"

namespace cc {

Display::Display(DisplayClient* client,
    SurfaceManager* manager,
    SharedBitmapManager* bitmap_manager,
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
    const RendererSettings& settings)
    : client_(client)
    , manager_(manager)
    , bitmap_manager_(bitmap_manager)
    , gpu_memory_buffer_manager_(gpu_memory_buffer_manager)
    , settings_(settings)
    , device_scale_factor_(1.f)
    , swapped_since_resize_(false)
    , scheduler_(nullptr)
    , texture_mailbox_deleter_(new TextureMailboxDeleter(nullptr))
{
    manager_->AddObserver(this);
}

Display::~Display()
{
    manager_->RemoveObserver(this);
    if (aggregator_) {
        for (const auto& id_entry : aggregator_->previous_contained_surfaces()) {
            Surface* surface = manager_->GetSurfaceForId(id_entry.first);
            if (surface)
                surface->RunDrawCallbacks(SurfaceDrawStatus::DRAW_SKIPPED);
        }
    }
}

bool Display::Initialize(scoped_ptr<OutputSurface> output_surface,
    DisplayScheduler* scheduler)
{
    output_surface_ = output_surface.Pass();
    scheduler_ = scheduler;
    return output_surface_->BindToClient(this);
}

void Display::SetSurfaceId(SurfaceId id, float device_scale_factor)
{
    if (current_surface_id_ == id && device_scale_factor_ == device_scale_factor)
        return;

    TRACE_EVENT0("cc", "Display::SetSurfaceId");

    current_surface_id_ = id;
    device_scale_factor_ = device_scale_factor;

    UpdateRootSurfaceResourcesLocked();
    if (scheduler_)
        scheduler_->SetNewRootSurface(id);
}

void Display::Resize(const gfx::Size& size)
{
    if (size == current_surface_size_)
        return;

    TRACE_EVENT0("cc", "Display::Resize");

    // Need to ensure all pending swaps have executed before the window is
    // resized, or D3D11 will scale the swap output.
    if (settings_.finish_rendering_on_resize) {
        if (!swapped_since_resize_ && scheduler_)
            scheduler_->ForceImmediateSwapIfPossible();
        if (swapped_since_resize_ && output_surface_ && output_surface_->context_provider())
            output_surface_->context_provider()->ContextGL()->ShallowFinishCHROMIUM();
    }
    swapped_since_resize_ = false;
    current_surface_size_ = size;
    if (scheduler_)
        scheduler_->DisplayResized();
}

void Display::SetExternalClip(const gfx::Rect& clip)
{
    external_clip_ = clip;
}

void Display::InitializeRenderer()
{
    if (resource_provider_)
        return;

    scoped_ptr<ResourceProvider> resource_provider = ResourceProvider::Create(
        output_surface_.get(), bitmap_manager_, gpu_memory_buffer_manager_,
        nullptr, settings_.highp_threshold_min,
        settings_.texture_id_allocation_chunk_size,
        std::vector<unsigned>(static_cast<size_t>(gfx::BufferFormat::LAST) + 1,
            GL_TEXTURE_2D));
    if (!resource_provider)
        return;

    if (output_surface_->context_provider()) {
        scoped_ptr<GLRenderer> renderer = GLRenderer::Create(
            this, &settings_, output_surface_.get(), resource_provider.get(),
            texture_mailbox_deleter_.get(), settings_.highp_threshold_min);
        if (!renderer)
            return;
        renderer_ = renderer.Pass();
    } else {
        scoped_ptr<SoftwareRenderer> renderer = SoftwareRenderer::Create(
            this, &settings_, output_surface_.get(), resource_provider.get());
        if (!renderer)
            return;
        renderer_ = renderer.Pass();
    }

    resource_provider_ = resource_provider.Pass();
    // TODO(jbauman): Outputting an incomplete quad list doesn't work when using
    // overlays.
    bool output_partial_list = renderer_->Capabilities().using_partial_swap && !output_surface_->GetOverlayCandidateValidator();
    aggregator_.reset(new SurfaceAggregator(manager_, resource_provider_.get(),
        output_partial_list));
}

void Display::DidLoseOutputSurface()
{
    if (scheduler_)
        scheduler_->OutputSurfaceLost();
    // WARNING: The client may delete the Display in this method call. Do not
    // make any additional references to members after this call.
    client_->OutputSurfaceLost();
}

void Display::UpdateRootSurfaceResourcesLocked()
{
    Surface* surface = manager_->GetSurfaceForId(current_surface_id_);
    bool root_surface_resources_locked = !surface || !surface->GetEligibleFrame();
    if (scheduler_)
        scheduler_->SetRootSurfaceResourcesLocked(root_surface_resources_locked);
}

bool Display::DrawAndSwap()
{
    TRACE_EVENT0("cc", "Display::DrawAndSwap");

    if (current_surface_id_.is_null()) {
        TRACE_EVENT_INSTANT0("cc", "No root surface.", TRACE_EVENT_SCOPE_THREAD);
        return false;
    }

    InitializeRenderer();
    if (!output_surface_) {
        TRACE_EVENT_INSTANT0("cc", "No output surface", TRACE_EVENT_SCOPE_THREAD);
        return false;
    }

    scoped_ptr<CompositorFrame> frame = aggregator_->Aggregate(current_surface_id_);
    if (!frame) {
        TRACE_EVENT_INSTANT0("cc", "Empty aggregated frame.",
            TRACE_EVENT_SCOPE_THREAD);
        return false;
    }

    // Run callbacks early to allow pipelining.
    for (const auto& id_entry : aggregator_->previous_contained_surfaces()) {
        Surface* surface = manager_->GetSurfaceForId(id_entry.first);
        if (surface)
            surface->RunDrawCallbacks(SurfaceDrawStatus::DRAWN);
    }

    DelegatedFrameData* frame_data = frame->delegated_frame_data.get();

    frame->metadata.latency_info.insert(frame->metadata.latency_info.end(),
        stored_latency_info_.begin(),
        stored_latency_info_.end());
    stored_latency_info_.clear();
    bool have_copy_requests = false;
    for (const auto* pass : frame_data->render_pass_list) {
        have_copy_requests |= !pass->copy_requests.empty();
    }

    gfx::Size surface_size;
    bool have_damage = false;
    if (!frame_data->render_pass_list.empty()) {
        surface_size = frame_data->render_pass_list.back()->output_rect.size();
        have_damage = !frame_data->render_pass_list.back()->damage_rect.size().IsEmpty();
    }

    bool size_matches = surface_size == current_surface_size_;
    //   if (!size_matches)
    //     TRACE_EVENT_INSTANT0("cc", "Size missmatch.", TRACE_EVENT_SCOPE_THREAD);

    bool should_draw = !frame->metadata.latency_info.empty() || have_copy_requests || (have_damage && size_matches);

    // If the surface is suspended then the resources to be used by the draw are
    // likely destroyed.
    if (output_surface_->SurfaceIsSuspendForRecycle()) {
        TRACE_EVENT_INSTANT0("cc", "Surface is suspended for recycle.",
            TRACE_EVENT_SCOPE_THREAD);
        should_draw = false;
    }

    if (should_draw) {
        gfx::Rect device_viewport_rect = gfx::Rect(current_surface_size_);
        gfx::Rect device_clip_rect = external_clip_.IsEmpty() ? device_viewport_rect : external_clip_;
        bool disable_picture_quad_image_filtering = false;

        renderer_->DecideRenderPassAllocationsForFrame(
            frame_data->render_pass_list);
        renderer_->DrawFrame(&frame_data->render_pass_list, device_scale_factor_,
            device_viewport_rect, device_clip_rect,
            disable_picture_quad_image_filtering);
    } else {
        TRACE_EVENT_INSTANT0("cc", "Draw skipped.", TRACE_EVENT_SCOPE_THREAD);
    }

    bool should_swap = should_draw && size_matches;
    if (should_swap) {
        swapped_since_resize_ = true;
        for (auto& latency : frame->metadata.latency_info) {
            TRACE_EVENT_WITH_FLOW1("input,benchmark", "LatencyInfo.Flow",
                TRACE_ID_DONT_MANGLE(latency.trace_id()),
                TRACE_EVENT_FLAG_FLOW_IN | TRACE_EVENT_FLAG_FLOW_OUT,
                "step", "Display::DrawAndSwap");
        }
        benchmark_instrumentation::IssueDisplayRenderingStatsEvent();
        renderer_->SwapBuffers(frame->metadata);
    } else {
        if (have_damage && !size_matches)
            aggregator_->SetFullDamageForSurface(current_surface_id_);
        TRACE_EVENT_INSTANT0("cc", "Swap skipped.", TRACE_EVENT_SCOPE_THREAD);
        stored_latency_info_.insert(stored_latency_info_.end(),
            frame->metadata.latency_info.begin(),
            frame->metadata.latency_info.end());
        DidSwapBuffers();
        DidSwapBuffersComplete();
    }

    return true;
}

void Display::DidSwapBuffers()
{
    if (scheduler_)
        scheduler_->DidSwapBuffers();
}

void Display::DidSwapBuffersComplete()
{
    if (scheduler_)
        scheduler_->DidSwapBuffersComplete();
}

void Display::CommitVSyncParameters(base::TimeTicks timebase,
    base::TimeDelta interval)
{
    client_->CommitVSyncParameters(timebase, interval);
}

void Display::SetMemoryPolicy(const ManagedMemoryPolicy& policy)
{
    client_->SetMemoryPolicy(policy);
}

void Display::OnDraw()
{
    NOTREACHED();
}

void Display::SetNeedsRedrawRect(const gfx::Rect& damage_rect)
{
    aggregator_->SetFullDamageForSurface(current_surface_id_);
    if (scheduler_)
        scheduler_->SurfaceDamaged(current_surface_id_);
}

void Display::ReclaimResources(const CompositorFrameAck* ack)
{
    NOTREACHED();
}

void Display::SetExternalDrawConstraints(
    const gfx::Transform& transform,
    const gfx::Rect& viewport,
    const gfx::Rect& clip,
    const gfx::Rect& viewport_rect_for_tile_priority,
    const gfx::Transform& transform_for_tile_priority,
    bool resourceless_software_draw)
{
    NOTREACHED();
}

void Display::SetTreeActivationCallback(const base::Closure& callback)
{
    NOTREACHED();
}

void Display::SetFullRootLayerDamage()
{
    if (aggregator_ && !current_surface_id_.is_null())
        aggregator_->SetFullDamageForSurface(current_surface_id_);
}

void Display::OnSurfaceDamaged(SurfaceId surface_id, bool* changed)
{
    if (aggregator_ && aggregator_->previous_contained_surfaces().count(surface_id)) {
        Surface* surface = manager_->GetSurfaceForId(surface_id);
        if (surface) {
            const CompositorFrame* current_frame = surface->GetEligibleFrame();
            if (!current_frame || !current_frame->delegated_frame_data || !current_frame->delegated_frame_data->resource_list.size()) {
                aggregator_->ReleaseResources(surface_id);
            }
        }
        if (scheduler_)
            scheduler_->SurfaceDamaged(surface_id);
        *changed = true;
    } else if (surface_id == current_surface_id_) {
        if (scheduler_)
            scheduler_->SurfaceDamaged(surface_id);
        *changed = true;
    }

    if (surface_id == current_surface_id_)
        UpdateRootSurfaceResourcesLocked();
}

SurfaceId Display::CurrentSurfaceId()
{
    return current_surface_id_;
}

} // namespace cc
