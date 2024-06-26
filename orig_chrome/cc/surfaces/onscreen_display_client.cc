// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/onscreen_display_client.h"

#include "base/trace_event/trace_event.h"
#include "cc/output/output_surface.h"
#include "cc/scheduler/begin_frame_source.h"
#include "cc/surfaces/display_scheduler.h"
#include "cc/surfaces/surface_display_output_surface.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_manager.h"

namespace cc {

OnscreenDisplayClient::OnscreenDisplayClient(
    scoped_ptr<OutputSurface> output_surface,
    SurfaceManager* manager,
    SharedBitmapManager* bitmap_manager,
    gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
    const RendererSettings& settings,
    scoped_refptr<base::SingleThreadTaskRunner> task_runner)
    : output_surface_(output_surface.Pass())
    , display_(new Display(this,
          manager,
          bitmap_manager,
          gpu_memory_buffer_manager,
          settings))
    , task_runner_(task_runner)
    , output_surface_lost_(false)
    , disable_display_vsync_(settings.disable_display_vsync)
{
}

OnscreenDisplayClient::~OnscreenDisplayClient()
{
}

bool OnscreenDisplayClient::Initialize()
{
    int max_frames_pending = output_surface_ ? output_surface_->capabilities().max_frames_pending : 0;
    if (max_frames_pending <= 0)
        max_frames_pending = OutputSurface::DEFAULT_MAX_FRAMES_PENDING;

    BeginFrameSource* frame_source;
    if (disable_display_vsync_) {
        unthrottled_frame_source_ = BackToBackBeginFrameSource::Create(task_runner_.get());
        frame_source = unthrottled_frame_source_.get();
    } else {
        synthetic_frame_source_ = SyntheticBeginFrameSource::Create(
            task_runner_.get(), BeginFrameArgs::DefaultInterval());
        frame_source = synthetic_frame_source_.get();
    }

    scheduler_.reset(new DisplayScheduler(
        display_.get(), frame_source, task_runner_.get(), max_frames_pending));

    return display_->Initialize(output_surface_.Pass(), scheduler_.get());
}

void OnscreenDisplayClient::CommitVSyncParameters(base::TimeTicks timebase,
    base::TimeDelta interval)
{
    if (interval == base::TimeDelta()) {
        // TODO(brianderson): We should not be receiving 0 intervals.
        interval = BeginFrameArgs::DefaultInterval();
    }

    surface_display_output_surface_->ReceivedVSyncParameters(timebase, interval);
    if (synthetic_frame_source_.get())
        synthetic_frame_source_->OnUpdateVSyncParameters(timebase, interval);
}

void OnscreenDisplayClient::OutputSurfaceLost()
{
    output_surface_lost_ = true;
    surface_display_output_surface_->DidLoseOutputSurface();
}

void OnscreenDisplayClient::SetMemoryPolicy(const ManagedMemoryPolicy& policy)
{
    surface_display_output_surface_->SetMemoryPolicy(policy);
}

} // namespace cc
