// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>
#include <stdint.h>

#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/mac/io_surface.h"
#include "ui/gl/gl_image_io_surface.h"
#include "ui/gl/test/gl_image_test_template.h"

namespace gl {
namespace {

    template <gfx::BufferFormat format>
    class GLImageIOSurfaceTestDelegate {
    public:
        scoped_refptr<GLImage> CreateImage(const gfx::Size& size) const
        {
            scoped_refptr<GLImageIOSurface> image(new GLImageIOSurface(
                size, GLImageIOSurface::GetInternalFormatForTesting(format)));
            IOSurfaceRef surface_ref = gfx::CreateIOSurface(size, format);
            bool rv = image->Initialize(surface_ref, gfx::GenericSharedMemoryId(1), format);
            EXPECT_TRUE(rv);
            return image;
        }

        scoped_refptr<GLImage> CreateSolidColorImage(const gfx::Size& size,
            const uint8_t color[4]) const
        {
            scoped_refptr<GLImageIOSurface> image(new GLImageIOSurface(
                size, GLImageIOSurface::GetInternalFormatForTesting(format)));
            IOSurfaceRef surface_ref = gfx::CreateIOSurface(size, format);
            IOReturn status = IOSurfaceLock(surface_ref, 0, nullptr);
            EXPECT_NE(status, kIOReturnCannotLock);
            for (size_t plane = 0; plane < NumberOfPlanesForBufferFormat(format);
                 ++plane) {
                void* data = IOSurfaceGetBaseAddressOfPlane(surface_ref, plane);
                GLImageTestSupport::SetBufferDataToColor(
                    size.width(), size.height(),
                    IOSurfaceGetBytesPerRowOfPlane(surface_ref, plane), plane, format,
                    color, static_cast<uint8_t*>(data));
            }
            IOSurfaceUnlock(surface_ref, 0, nullptr);

            bool rv = image->Initialize(surface_ref, gfx::GenericSharedMemoryId(1), format);
            EXPECT_TRUE(rv);

            return image;
        }

        unsigned GetTextureTarget() const { return GL_TEXTURE_RECTANGLE_ARB; }
    };

    using GLImageTestTypes = testing::Types<
        GLImageIOSurfaceTestDelegate<gfx::BufferFormat::RGBA_8888>,
        GLImageIOSurfaceTestDelegate<gfx::BufferFormat::BGRA_8888>,
        GLImageIOSurfaceTestDelegate<gfx::BufferFormat::YUV_420_BIPLANAR>>;

    INSTANTIATE_TYPED_TEST_CASE_P(GLImageIOSurface, GLImageTest, GLImageTestTypes);

    using GLImageIOSurfaceTestTypes = testing::Types<GLImageIOSurfaceTestDelegate<gfx::BufferFormat::RGBA_8888>,
        GLImageIOSurfaceTestDelegate<gfx::BufferFormat::BGRA_8888>>;

    INSTANTIATE_TYPED_TEST_CASE_P(GLImageIOSurface,
        GLImageZeroInitializeTest,
        GLImageIOSurfaceTestTypes);

    INSTANTIATE_TYPED_TEST_CASE_P(
        GLImageIOSurface,
        GLImageCopyTest,
        GLImageIOSurfaceTestDelegate<gfx::BufferFormat::YUV_420_BIPLANAR>);

} // namespace
} // namespace gl
