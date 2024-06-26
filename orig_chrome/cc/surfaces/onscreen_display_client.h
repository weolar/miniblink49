// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_ONSCREEN_DISPLAY_CLIENT_H_
#define CC_SURFACES_ONSCREEN_DISPLAY_CLIENT_H_

#include "cc/surfaces/display_client.h"

#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/single_thread_task_runner.h"
#include "cc/surfaces/display.h"
#include "cc/surfaces/surfaces_export.h"

class VSyncParameterObserver;

namespace cc {
class BeginFrameSource;
class ContextProvider;
class DisplayScheduler;
class SurfaceManager;
class SurfaceDisplayOutputSurface;

// This class provides a DisplayClient implementation for drawing directly to an
// onscreen context.
class CC_SURFACES_EXPORT OnscreenDisplayClient
    : NON_EXPORTED_BASE(DisplayClient) {
public:
    OnscreenDisplayClient(
        scoped_ptr<OutputSurface> output_surface,
        SurfaceManager* manager,
        SharedBitmapManager* bitmap_manager,
        gpu::GpuMemoryBufferManager* gpu_memory_buffer_manager,
        const RendererSettings& settings,
        scoped_refptr<base::SingleThreadTaskRunner> task_runner);
    ~OnscreenDisplayClient() override;

    bool Initialize();
    Display* display() { return display_.get(); }
    void set_surface_output_surface(SurfaceDisplayOutputSurface* surface)
    {
        surface_display_output_surface_ = surface;
    }

    // DisplayClient implementation.
    void CommitVSyncParameters(base::TimeTicks timebase,
        base::TimeDelta interval) override;
    void OutputSurfaceLost() override;
    void SetMemoryPolicy(const ManagedMemoryPolicy& policy) override;

    bool output_surface_lost() { return output_surface_lost_; }

protected:
    scoped_ptr<OutputSurface> output_surface_;
    scoped_ptr<Display> display_;
    scoped_ptr<SyntheticBeginFrameSource> synthetic_frame_source_;
    scoped_ptr<BackToBackBeginFrameSource> unthrottled_frame_source_;
    scoped_ptr<DisplayScheduler> scheduler_;
    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
    SurfaceDisplayOutputSurface* surface_display_output_surface_;
    bool output_surface_lost_;
    bool disable_display_vsync_;

private:
    DISALLOW_COPY_AND_ASSIGN(OnscreenDisplayClient);
};

} // namespace cc

#endif // CC_SURFACES_ONSCREEN_DISPLAY_CLIENT_H_
