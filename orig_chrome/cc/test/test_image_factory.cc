// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/test_image_factory.h"

#include "ui/gl/gl_image_shared_memory.h"

namespace cc {

TestImageFactory::TestImageFactory()
{
}

TestImageFactory::~TestImageFactory()
{
}

scoped_refptr<gfx::GLImage> TestImageFactory::CreateImageForGpuMemoryBuffer(
    const gfx::GpuMemoryBufferHandle& handle,
    const gfx::Size& size,
    gfx::BufferFormat format,
    unsigned internalformat,
    int client_id)
{
    DCHECK_EQ(handle.type, gfx::SHARED_MEMORY_BUFFER);

    scoped_refptr<gfx::GLImageSharedMemory> image(
        new gfx::GLImageSharedMemory(size, internalformat));
    if (!image->Initialize(handle, format))
        return nullptr;

    return image;
}

} // namespace cc
