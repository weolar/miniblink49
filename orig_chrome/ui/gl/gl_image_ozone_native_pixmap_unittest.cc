// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/buffer_types.h"
#include "ui/gl/gl_image_ozone_native_pixmap.h"
#include "ui/gl/test/gl_image_test_template.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/public/surface_factory_ozone.h"

namespace gl {
namespace {

    class GLImageOzoneNativePixmapTestDelegate {
    public:
        scoped_refptr<gl::GLImage> CreateSolidColorImage(
            const gfx::Size& size,
            const uint8_t color[4]) const
        {
            ui::SurfaceFactoryOzone* surface_factory = ui::OzonePlatform::GetInstance()->GetSurfaceFactoryOzone();
            scoped_refptr<ui::NativePixmap> pixmap = surface_factory->CreateNativePixmap(gfx::kNullAcceleratedWidget, size,
                gfx::BufferFormat::RGBA_8888,
                gfx::BufferUsage::SCANOUT);
            EXPECT_TRUE(pixmap != nullptr);
            scoped_refptr<gfx::GLImageOzoneNativePixmap> image(
                new gfx::GLImageOzoneNativePixmap(size, GL_RGBA));
            EXPECT_TRUE(image->Initialize(pixmap.get(), pixmap->GetBufferFormat()));
            return image;
        }

        unsigned GetTextureTarget() const { return GL_TEXTURE_2D; }
    };

    INSTANTIATE_TYPED_TEST_CASE_P(GLImageOzoneNativePixmap,
        GLImageTest,
        GLImageOzoneNativePixmapTestDelegate);

} // namespace
} // namespace gl
