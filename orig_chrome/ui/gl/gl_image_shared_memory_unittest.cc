// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>
#include <stdint.h>

#include "base/memory/shared_memory.h"
#include "base/sys_info.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_image_shared_memory.h"
#include "ui/gl/test/gl_image_test_template.h"

namespace gl {
namespace {

    template <gfx::BufferFormat format>
    class GLImageSharedMemoryTestDelegate {
    public:
        scoped_refptr<gl::GLImage> CreateSolidColorImage(
            const gfx::Size& size,
            const uint8_t color[4]) const
        {
            DCHECK_EQ(NumberOfPlanesForBufferFormat(format), 1u);
            base::SharedMemory shared_memory;
            bool rv = shared_memory.CreateAndMapAnonymous(
                gfx::BufferSizeForBufferFormat(size, format));
            DCHECK(rv);
            GLImageTestSupport::SetBufferDataToColor(
                size.width(), size.height(),
                static_cast<int>(RowSizeForBufferFormat(size.width(), format, 0)), 0,
                format, color, reinterpret_cast<uint8_t*>(shared_memory.memory()));
            scoped_refptr<gl::GLImageSharedMemory> image(new gl::GLImageSharedMemory(
                size, gl::GLImageMemory::GetInternalFormatForTesting(format)));
            rv = image->Initialize(
                base::SharedMemory::DuplicateHandle(shared_memory.handle()),
                gfx::GenericSharedMemoryId(0), format, 0,
                gfx::RowSizeForBufferFormat(size.width(), format, 0));
            EXPECT_TRUE(rv);
            return image;
        }

        unsigned GetTextureTarget() const { return GL_TEXTURE_2D; }
    };

    using GLImageTestTypes = testing::Types<
        GLImageSharedMemoryTestDelegate<gfx::BufferFormat::RGBX_8888>,
        GLImageSharedMemoryTestDelegate<gfx::BufferFormat::RGBA_8888>,
        GLImageSharedMemoryTestDelegate<gfx::BufferFormat::BGRX_8888>,
        GLImageSharedMemoryTestDelegate<gfx::BufferFormat::BGRA_8888>>;

    INSTANTIATE_TYPED_TEST_CASE_P(GLImageSharedMemory,
        GLImageTest,
        GLImageTestTypes);

    INSTANTIATE_TYPED_TEST_CASE_P(GLImageSharedMemory,
        GLImageCopyTest,
        GLImageTestTypes);

    class GLImageSharedMemoryPoolTestDelegate {
    public:
        scoped_refptr<gl::GLImage> CreateSolidColorImage(
            const gfx::Size& size,
            const uint8_t color[4]) const
        {
            // Create a shared memory segment that holds an image with a stride that is
            // twice the row size and 2 pages larger than image.
            size_t stride = gfx::RowSizeForBufferFormat(
                                size.width(), gfx::BufferFormat::RGBA_8888, 0)
                * 2;
            size_t pool_size = stride * size.height() + base::SysInfo::VMAllocationGranularity() * 3;
            base::SharedMemory shared_memory;
            bool rv = shared_memory.CreateAndMapAnonymous(pool_size);
            DCHECK(rv);
            // Initialize memory to a value that is easy to recognize if test fails.
            memset(shared_memory.memory(), 0x55, pool_size);
            // Place buffer at a non-zero non-page-aligned offset in shared memory.
            size_t buffer_offset = 3 * base::SysInfo::VMAllocationGranularity() / 2;
            GLImageTestSupport::SetBufferDataToColor(
                size.width(), size.height(), static_cast<int>(stride), 0,
                gfx::BufferFormat::RGBA_8888, color,
                reinterpret_cast<uint8_t*>(shared_memory.memory()) + buffer_offset);
            scoped_refptr<gl::GLImageSharedMemory> image(
                new gl::GLImageSharedMemory(size, GL_RGBA));
            rv = image->Initialize(
                base::SharedMemory::DuplicateHandle(shared_memory.handle()),
                gfx::GenericSharedMemoryId(0), gfx::BufferFormat::RGBA_8888,
                buffer_offset, stride);
            EXPECT_TRUE(rv);
            return image;
        }

        unsigned GetTextureTarget() const { return GL_TEXTURE_2D; }
    };

    INSTANTIATE_TYPED_TEST_CASE_P(GLImageSharedMemoryPool,
        GLImageCopyTest,
        GLImageSharedMemoryPoolTestDelegate);

} // namespace
} // namespace gl
