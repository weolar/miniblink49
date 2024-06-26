// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "orig_chrome/content/compositor/SoftwareOutputSurface.h"

#include "base/bind.h"
#include "cc/output/context_provider.h"
#include "cc/output/managed_memory_policy.h"
#include "cc/output/output_surface_client.h"
#include "gpu/command_buffer/common/gpu_memory_allocation.h"
#include "orig_chrome/content/compositor/BrowserContextProvider.h"
#include "orig_chrome/content/compositor/SoftwareOutputDevice.h"
#include "skia/ext/platform_canvas.h"

namespace content {

scoped_ptr<SoftwareOutputSurface> SoftwareOutputSurface::Create(WebPageOcBridge* webPageOcBridge)
{
    return scoped_ptr<SoftwareOutputSurface>(new SoftwareOutputSurface(webPageOcBridge));
}

SoftwareOutputSurface::SoftwareOutputSurface(WebPageOcBridge* webPageOcBridge)
    : OutputSurface(scoped_ptr<cc::SoftwareOutputDevice>(new SoftwareOutputDevice(webPageOcBridge)))
{
    Initialize();
}

SoftwareOutputSurface::~SoftwareOutputSurface()
{
}

void SoftwareOutputSurface::Initialize()
{
    capabilities_.max_frames_pending = 1;
    capabilities_.adjust_deadline_for_parent = false;
}

void SoftwareOutputSurface::SwapBuffers(cc::CompositorFrame* frame)
{
}

bool SoftwareOutputSurface::BindToClient(cc::OutputSurfaceClient* client)
{
    client->SetMemoryPolicy(cc::ManagedMemoryPolicy(
        128 * 1024 * 1024,
        gpu::MemoryAllocation::CUTOFF_ALLOW_NICE_TO_HAVE,
        base::SharedMemory::GetHandleLimit() / 3));
    return cc::OutputSurface::BindToClient(client);
}

void SoftwareOutputSurface::firePaintEvent(HDC hdc, const RECT& paintRect)
{
    if (!software_device())
        return;
    ((SoftwareOutputDevice*)software_device())->firePaintEvent(hdc, paintRect);
}

} // content