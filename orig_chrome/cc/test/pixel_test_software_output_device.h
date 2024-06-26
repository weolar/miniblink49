// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_PIXEL_TEST_SOFTWARE_OUTPUT_DEVICE_H_
#define CC_TEST_PIXEL_TEST_SOFTWARE_OUTPUT_DEVICE_H_

#include "cc/output/software_output_device.h"

namespace cc {

class PixelTestSoftwareOutputDevice : public SoftwareOutputDevice {
public:
    void Resize(const gfx::Size& pixel_size, float scale_factor) override;

    void set_surface_expansion_size(const gfx::Size& surface_expansion_size)
    {
        surface_expansion_size_ = surface_expansion_size;
    }

private:
    gfx::Size surface_expansion_size_;
};

} // namespace cc

#endif // CC_TEST_PIXEL_TEST_SOFTWARE_OUTPUT_DEVICE_H_
