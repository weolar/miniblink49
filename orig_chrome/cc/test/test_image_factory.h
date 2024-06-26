// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_TEST_IMAGE_FACTORY_H_
#define CC_TEST_TEST_IMAGE_FACTORY_H_

#include "gpu/command_buffer/service/image_factory.h"

namespace cc {

class TestImageFactory : public gpu::ImageFactory {
public:
    TestImageFactory();
    ~TestImageFactory() override;

    // Overridden from gpu::ImageFactory:
    scoped_refptr<gfx::GLImage> CreateImageForGpuMemoryBuffer(
        const gfx::GpuMemoryBufferHandle& handle,
        const gfx::Size& size,
        gfx::BufferFormat format,
        unsigned internalformat,
        int client_id) override;

private:
    DISALLOW_COPY_AND_ASSIGN(TestImageFactory);
};

} // namespace cc

#endif // CC_TEST_TEST_IMAGE_FACTORY_H_
