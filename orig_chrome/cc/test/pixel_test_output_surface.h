// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_PIXEL_TEST_OUTPUT_SURFACE_H_
#define CC_TEST_PIXEL_TEST_OUTPUT_SURFACE_H_

#include "cc/output/output_surface.h"

namespace cc {

class PixelTestOutputSurface : public OutputSurface {
public:
    explicit PixelTestOutputSurface(
        scoped_refptr<ContextProvider> context_provider,
        scoped_refptr<ContextProvider> worker_context_provider,
        bool flipped_output_surface);
    explicit PixelTestOutputSurface(
        scoped_refptr<ContextProvider> context_provider,
        bool flipped_output_surface);
    explicit PixelTestOutputSurface(
        scoped_ptr<SoftwareOutputDevice> software_device);

    void Reshape(const gfx::Size& size, float scale_factor) override;
    bool HasExternalStencilTest() const override;
    void SwapBuffers(CompositorFrame* frame) override;

    void set_surface_expansion_size(const gfx::Size& surface_expansion_size)
    {
        surface_expansion_size_ = surface_expansion_size;
    }
    void set_has_external_stencil_test(bool has_test)
    {
        external_stencil_test_ = has_test;
    }

private:
    gfx::Size surface_expansion_size_;
    bool external_stencil_test_;
};

} // namespace cc

#endif // CC_TEST_PIXEL_TEST_OUTPUT_SURFACE_H_
