// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/software_output_device.h"

#include "base/logging.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/vsync_provider.h"

namespace cc {

SoftwareOutputDevice::SoftwareOutputDevice()
    : scale_factor_(1.f)
{
}

SoftwareOutputDevice::~SoftwareOutputDevice() { }

void SoftwareOutputDevice::Resize(const gfx::Size& viewport_pixel_size,
    float scale_factor)
{
    scale_factor_ = scale_factor;

    if (viewport_pixel_size_ == viewport_pixel_size)
        return;

    SkImageInfo info = SkImageInfo::MakeN32(viewport_pixel_size.width(),
        viewport_pixel_size.height(),
        kOpaque_SkAlphaType);
    viewport_pixel_size_ = viewport_pixel_size;
    surface_ = skia::AdoptRef(SkSurface::NewRaster(info));
}

SkCanvas* SoftwareOutputDevice::BeginPaint(const gfx::Rect& damage_rect)
{
    damage_rect_ = damage_rect;
    return surface_ ? surface_->getCanvas() : nullptr;
}

void SoftwareOutputDevice::EndPaint() { }

gfx::VSyncProvider* SoftwareOutputDevice::GetVSyncProvider()
{
    return vsync_provider_.get();
}

} // namespace cc
