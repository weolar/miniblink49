// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/pixel_test_software_output_device.h"

namespace cc {

void PixelTestSoftwareOutputDevice::Resize(const gfx::Size& pixel_size,
    float scale_factor)
{
    gfx::Size expanded_size(
        pixel_size.width() + surface_expansion_size_.width(),
        pixel_size.height() + surface_expansion_size_.height());
    SoftwareOutputDevice::Resize(expanded_size, scale_factor);
}

} // namespace cc
