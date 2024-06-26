// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/image_factory.h"

#include "gpu/command_buffer/common/capabilities.h"
#include "ui/gl/gl_bindings.h"

namespace gpu {

ImageFactory::ImageFactory()
{
}

ImageFactory::~ImageFactory()
{
}

// static
gfx::BufferFormat ImageFactory::DefaultBufferFormatForImageFormat(
    unsigned internalformat)
{
    switch (internalformat) {
    case GL_RED:
        return gfx::BufferFormat::R_8;
    case GL_RGB:
        return gfx::BufferFormat::BGRX_8888;
    case GL_RGBA:
        return gfx::BufferFormat::RGBA_8888;
    case GL_BGRA_EXT:
        return gfx::BufferFormat::BGRA_8888;
    case GL_ATC_RGB_AMD:
        return gfx::BufferFormat::ATC;
    case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
        return gfx::BufferFormat::ATCIA;
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        return gfx::BufferFormat::DXT1;
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        return gfx::BufferFormat::DXT5;
    case GL_ETC1_RGB8_OES:
        return gfx::BufferFormat::ETC1;
    case GL_RGB_YUV_420_CHROMIUM:
        return gfx::BufferFormat::YUV_420;
    case GL_RGB_YCBCR_422_CHROMIUM:
        return gfx::BufferFormat::UYVY_422;
    default:
        NOTREACHED();
        return gfx::BufferFormat::RGBA_8888;
    }
}

// static
bool ImageFactory::IsImageFormatCompatibleWithGpuMemoryBufferFormat(
    unsigned internalformat,
    gfx::BufferFormat format)
{
    switch (format) {
    case gfx::BufferFormat::ATC:
    case gfx::BufferFormat::ATCIA:
    case gfx::BufferFormat::BGRA_8888:
    case gfx::BufferFormat::BGRX_8888:
    case gfx::BufferFormat::DXT1:
    case gfx::BufferFormat::DXT5:
    case gfx::BufferFormat::ETC1:
    case gfx::BufferFormat::R_8:
    case gfx::BufferFormat::RGBA_8888:
    case gfx::BufferFormat::RGBX_8888:
    case gfx::BufferFormat::YUV_420:
    case gfx::BufferFormat::YUV_420_BIPLANAR:
    case gfx::BufferFormat::UYVY_422:
        return format == DefaultBufferFormatForImageFormat(internalformat);
    case gfx::BufferFormat::RGBA_4444:
        return internalformat == GL_RGBA;
    }

    NOTREACHED();
    return false;
}

// static
bool ImageFactory::IsGpuMemoryBufferFormatSupported(
    gfx::BufferFormat format,
    const gpu::Capabilities& capabilities)
{
    switch (format) {
    case gfx::BufferFormat::ATC:
    case gfx::BufferFormat::ATCIA:
        return capabilities.texture_format_atc;
    case gfx::BufferFormat::BGRA_8888:
    case gfx::BufferFormat::BGRX_8888:
        return capabilities.texture_format_bgra8888;
    case gfx::BufferFormat::DXT1:
        return capabilities.texture_format_dxt1;
    case gfx::BufferFormat::DXT5:
        return capabilities.texture_format_dxt5;
    case gfx::BufferFormat::ETC1:
        return capabilities.texture_format_etc1;
    case gfx::BufferFormat::R_8:
        return capabilities.texture_rg;
    case gfx::BufferFormat::UYVY_422:
        return capabilities.image_ycbcr_422;
    case gfx::BufferFormat::RGBA_4444:
    case gfx::BufferFormat::RGBA_8888:
    case gfx::BufferFormat::RGBX_8888:
    case gfx::BufferFormat::YUV_420:
        return true;
    case gfx::BufferFormat::YUV_420_BIPLANAR:
        return false;
    }

    NOTREACHED();
    return false;
}

// static
bool ImageFactory::IsImageSizeValidForGpuMemoryBufferFormat(
    const gfx::Size& size,
    gfx::BufferFormat format)
{
    switch (format) {
    case gfx::BufferFormat::ATC:
    case gfx::BufferFormat::ATCIA:
    case gfx::BufferFormat::DXT1:
    case gfx::BufferFormat::DXT5:
    case gfx::BufferFormat::ETC1:
        // Compressed images must have a width and height that's evenly divisible
        // by the block size.
        return size.width() % 4 == 0 && size.height() % 4 == 0;
    case gfx::BufferFormat::R_8:
    case gfx::BufferFormat::RGBA_4444:
    case gfx::BufferFormat::RGBA_8888:
    case gfx::BufferFormat::RGBX_8888:
    case gfx::BufferFormat::BGRA_8888:
    case gfx::BufferFormat::BGRX_8888:
        return true;
    case gfx::BufferFormat::YUV_420:
    case gfx::BufferFormat::YUV_420_BIPLANAR:
        // U and V planes are subsampled by a factor of 2.
        return size.width() % 2 == 0 && size.height() % 2 == 0;
    case gfx::BufferFormat::UYVY_422:
        return size.width() % 2 == 0;
    }

    NOTREACHED();
    return false;
}

} // namespace gpu
