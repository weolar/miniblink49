// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_image_memory.h"

#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "base/trace_event/trace_event.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_version_info.h"

using gfx::BufferFormat;

namespace gl {
namespace {

    bool ValidInternalFormat(unsigned internalformat)
    {
        switch (internalformat) {
        case GL_ATC_RGB_AMD:
        case GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        case GL_ETC1_RGB8_OES:
        case GL_RED:
        case GL_RGB:
        case GL_RGBA:
        case GL_BGRA_EXT:
            return true;
        default:
            return false;
        }
    }

    bool ValidFormat(BufferFormat format)
    {
        switch (format) {
        case BufferFormat::ATC:
        case BufferFormat::ATCIA:
        case BufferFormat::DXT1:
        case BufferFormat::DXT5:
        case BufferFormat::ETC1:
        case BufferFormat::R_8:
        case BufferFormat::RGBA_4444:
        case BufferFormat::RGBX_8888:
        case BufferFormat::RGBA_8888:
        case BufferFormat::BGRX_8888:
        case BufferFormat::BGRA_8888:
            return true;
        case BufferFormat::YUV_420:
        case BufferFormat::YUV_420_BIPLANAR:
        case BufferFormat::UYVY_422:
            return false;
        }

        NOTREACHED();
        return false;
    }

    bool IsCompressedFormat(BufferFormat format)
    {
        switch (format) {
        case BufferFormat::ATC:
        case BufferFormat::ATCIA:
        case BufferFormat::DXT1:
        case BufferFormat::DXT5:
        case BufferFormat::ETC1:
            return true;
        case BufferFormat::R_8:
        case BufferFormat::RGBA_4444:
        case BufferFormat::RGBX_8888:
        case BufferFormat::RGBA_8888:
        case BufferFormat::BGRX_8888:
        case BufferFormat::BGRA_8888:
            return false;
        case BufferFormat::YUV_420:
        case BufferFormat::YUV_420_BIPLANAR:
        case BufferFormat::UYVY_422:
            NOTREACHED();
            return false;
        }

        NOTREACHED();
        return false;
    }

    GLenum TextureFormat(BufferFormat format)
    {
        switch (format) {
        case BufferFormat::ATC:
            return GL_ATC_RGB_AMD;
        case BufferFormat::ATCIA:
            return GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD;
        case BufferFormat::DXT1:
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case BufferFormat::DXT5:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case BufferFormat::ETC1:
            return GL_ETC1_RGB8_OES;
        case BufferFormat::R_8:
            return GL_RED;
        case BufferFormat::RGBA_4444:
        case BufferFormat::RGBA_8888:
            return GL_RGBA;
        case BufferFormat::BGRA_8888:
            return GL_BGRA_EXT;
        case BufferFormat::RGBX_8888:
        case BufferFormat::BGRX_8888:
            return GL_RGB;
        case BufferFormat::YUV_420:
        case BufferFormat::YUV_420_BIPLANAR:
        case BufferFormat::UYVY_422:
            NOTREACHED();
            return 0;
        }

        NOTREACHED();
        return 0;
    }

    GLenum DataFormat(BufferFormat format)
    {
        switch (format) {
        case BufferFormat::RGBX_8888:
            return GL_RGBA;
        case BufferFormat::BGRX_8888:
            return GL_BGRA_EXT;
        case BufferFormat::RGBA_4444:
        case BufferFormat::RGBA_8888:
        case BufferFormat::BGRA_8888:
        case BufferFormat::R_8:
        case BufferFormat::ATC:
        case BufferFormat::ATCIA:
        case BufferFormat::DXT1:
        case BufferFormat::DXT5:
        case BufferFormat::ETC1:
            return TextureFormat(format);
        case BufferFormat::YUV_420:
        case BufferFormat::YUV_420_BIPLANAR:
        case BufferFormat::UYVY_422:
            NOTREACHED();
            return 0;
        }

        NOTREACHED();
        return 0;
    }

    GLenum DataType(BufferFormat format)
    {
        switch (format) {
        case BufferFormat::RGBA_4444:
            return GL_UNSIGNED_SHORT_4_4_4_4;
        case BufferFormat::RGBX_8888:
        case BufferFormat::RGBA_8888:
        case BufferFormat::BGRX_8888:
        case BufferFormat::BGRA_8888:
        case BufferFormat::R_8:
            return GL_UNSIGNED_BYTE;
        case BufferFormat::ATC:
        case BufferFormat::ATCIA:
        case BufferFormat::DXT1:
        case BufferFormat::DXT5:
        case BufferFormat::ETC1:
        case BufferFormat::YUV_420:
        case BufferFormat::YUV_420_BIPLANAR:
        case BufferFormat::UYVY_422:
            NOTREACHED();
            return 0;
        }

        NOTREACHED();
        return 0;
    }

    GLint DataRowLength(size_t stride, BufferFormat format)
    {
        switch (format) {
        case BufferFormat::RGBA_4444:
            return base::checked_cast<GLint>(stride) / 2;
        case BufferFormat::RGBX_8888:
        case BufferFormat::RGBA_8888:
        case BufferFormat::BGRX_8888:
        case BufferFormat::BGRA_8888:
            return base::checked_cast<GLint>(stride) / 4;
        case BufferFormat::R_8:
            return base::checked_cast<GLint>(stride);
        case BufferFormat::ATC:
        case BufferFormat::ATCIA:
        case BufferFormat::DXT1:
        case BufferFormat::DXT5:
        case BufferFormat::ETC1:
        case BufferFormat::YUV_420:
        case BufferFormat::YUV_420_BIPLANAR:
        case BufferFormat::UYVY_422:
            NOTREACHED();
            return 0;
        }

        NOTREACHED();
        return 0;
    }

    template <typename F>
    scoped_ptr<uint8_t[]> GLES2RGBData(const gfx::Size& size,
        BufferFormat format,
        size_t stride,
        const uint8_t* data,
        F const& data_to_rgb,
        GLenum* data_format,
        GLenum* data_type,
        GLint* data_row_length)
    {
        TRACE_EVENT2("gpu", "GLES2RGBData", "width", size.width(), "height",
            size.height());

        // Four-byte row alignment as specified by glPixelStorei with argument
        // GL_UNPACK_ALIGNMENT set to 4.
        size_t gles2_rgb_data_stride = (size.width() * 3 + 3) & ~3;
        scoped_ptr<uint8_t[]> gles2_rgb_data(
            new uint8_t[gles2_rgb_data_stride * size.height()]);

        for (int y = 0; y < size.height(); ++y) {
            for (int x = 0; x < size.width(); ++x) {
                data_to_rgb(&data[y * stride + x * 4],
                    &gles2_rgb_data[y * gles2_rgb_data_stride + x * 3]);
            }
        }

        *data_format = GL_RGB;
        *data_type = GL_UNSIGNED_BYTE;
        *data_row_length = size.width();
        return gles2_rgb_data.Pass();
    }

    scoped_ptr<uint8_t[]> GLES2Data(const gfx::Size& size,
        BufferFormat format,
        size_t stride,
        const uint8_t* data,
        GLenum* data_format,
        GLenum* data_type,
        GLint* data_row_length)
    {
        TRACE_EVENT2("gpu", "GLES2Data", "width", size.width(), "height",
            size.height());

        switch (format) {
        case BufferFormat::RGBX_8888:
            return GLES2RGBData(
                size, format, stride,
                data, [](const uint8_t* src, uint8_t* dst) {
                    dst[0] = src[0];
                    dst[1] = src[1];
                    dst[2] = src[2];
                },
                data_format, data_type, data_row_length);
        case BufferFormat::BGRX_8888:
            return GLES2RGBData(
                size, format, stride,
                data, [](const uint8_t* src, uint8_t* dst) {
                    dst[0] = src[2];
                    dst[1] = src[1];
                    dst[2] = src[0];
                },
                data_format, data_type, data_row_length);
        case BufferFormat::RGBA_4444:
        case BufferFormat::RGBA_8888:
        case BufferFormat::BGRA_8888:
        case BufferFormat::R_8: {
            size_t gles2_data_stride = RowSizeForBufferFormat(size.width(), format, 0);
            if (stride == gles2_data_stride)
                return nullptr; // No data conversion needed

            scoped_ptr<uint8_t[]> gles2_data(
                new uint8_t[gles2_data_stride * size.height()]);
            for (int y = 0; y < size.height(); ++y) {
                memcpy(&gles2_data[y * gles2_data_stride], &data[y * stride],
                    gles2_data_stride);
            }
            *data_row_length = size.width();
            return gles2_data.Pass();
        }
        case BufferFormat::ATC:
        case BufferFormat::ATCIA:
        case BufferFormat::DXT1:
        case BufferFormat::DXT5:
        case BufferFormat::ETC1:
            return nullptr; // No data conversion needed
        case BufferFormat::YUV_420:
        case BufferFormat::YUV_420_BIPLANAR:
        case BufferFormat::UYVY_422:
            NOTREACHED();
            return nullptr;
        }

        NOTREACHED();
        return nullptr;
    }

} // namespace

GLImageMemory::GLImageMemory(const gfx::Size& size, unsigned internalformat)
    : size_(size)
    , internalformat_(internalformat)
    , memory_(nullptr)
    , format_(BufferFormat::RGBA_8888)
    , stride_(0)
{
}

GLImageMemory::~GLImageMemory()
{
    DCHECK(!memory_);
}

bool GLImageMemory::Initialize(const unsigned char* memory,
    BufferFormat format,
    size_t stride)
{
    if (!ValidInternalFormat(internalformat_)) {
        LOG(ERROR) << "Invalid internalformat: " << internalformat_;
        return false;
    }

    if (!ValidFormat(format)) {
        LOG(ERROR) << "Invalid format: " << static_cast<int>(format);
        return false;
    }

    if (stride < RowSizeForBufferFormat(size_.width(), format, 0) || stride & 3) {
        LOG(ERROR) << "Invalid stride: " << stride;
        return false;
    }

    DCHECK(memory);
    DCHECK(!memory_);
    DCHECK(!IsCompressedFormat(format) || size_.width() % 4 == 0);
    DCHECK(!IsCompressedFormat(format) || size_.height() % 4 == 0);
    memory_ = memory;
    format_ = format;
    stride_ = stride;
    return true;
}

void GLImageMemory::Destroy(bool have_context)
{
    memory_ = nullptr;
}

gfx::Size GLImageMemory::GetSize()
{
    return size_;
}

unsigned GLImageMemory::GetInternalFormat()
{
    return internalformat_;
}

bool GLImageMemory::BindTexImage(unsigned target)
{
    return false;
}

bool GLImageMemory::CopyTexImage(unsigned target)
{
    TRACE_EVENT2("gpu", "GLImageMemory::CopyTexImage", "width", size_.width(),
        "height", size_.height());

    // GL_TEXTURE_EXTERNAL_OES is not a supported target.
    if (target == GL_TEXTURE_EXTERNAL_OES)
        return false;

    if (IsCompressedFormat(format_)) {
        glCompressedTexImage2D(
            target, 0, TextureFormat(format_), size_.width(), size_.height(), 0,
            static_cast<GLsizei>(BufferSizeForBufferFormat(size_, format_)),
            memory_);
    } else {
        GLenum data_format = DataFormat(format_);
        GLenum data_type = DataType(format_);
        GLint data_row_length = DataRowLength(stride_, format_);
        scoped_ptr<uint8_t[]> gles2_data;

        if (gfx::GLContext::GetCurrent()->GetVersionInfo()->is_es) {
            gles2_data = GLES2Data(size_, format_, stride_, memory_, &data_format,
                &data_type, &data_row_length);
        }

        if (data_row_length != size_.width())
            glPixelStorei(GL_UNPACK_ROW_LENGTH, data_row_length);

        glTexImage2D(target, 0, TextureFormat(format_), size_.width(),
            size_.height(), 0, data_format, data_type,
            gles2_data ? gles2_data.get() : memory_);

        if (data_row_length != size_.width())
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }

    return true;
}

bool GLImageMemory::CopyTexSubImage(unsigned target,
    const gfx::Point& offset,
    const gfx::Rect& rect)
{
    TRACE_EVENT2("gpu", "GLImageMemory::CopyTexSubImage", "width", rect.width(),
        "height", rect.height());

    // GL_TEXTURE_EXTERNAL_OES is not a supported target.
    if (target == GL_TEXTURE_EXTERNAL_OES)
        return false;

    // Sub width is not supported.
    if (rect.width() != size_.width())
        return false;

    const uint8_t* data = memory_ + rect.y() * stride_;
    if (IsCompressedFormat(format_)) {
        // Height must be a multiple of 4.
        if (rect.height() % 4)
            return false;

        glCompressedTexSubImage2D(
            target, 0, offset.x(), offset.y(), rect.width(), rect.height(),
            DataFormat(format_),
            static_cast<GLsizei>(BufferSizeForBufferFormat(rect.size(), format_)),
            data);
    } else {
        GLenum data_format = DataFormat(format_);
        GLenum data_type = DataType(format_);
        GLint data_row_length = DataRowLength(stride_, format_);
        scoped_ptr<uint8_t[]> gles2_data;

        if (gfx::GLContext::GetCurrent()->GetVersionInfo()->is_es) {
            gles2_data = GLES2Data(rect.size(), format_, stride_, data, &data_format,
                &data_type, &data_row_length);
        }

        if (data_row_length != rect.width())
            glPixelStorei(GL_UNPACK_ROW_LENGTH, data_row_length);

        glTexSubImage2D(target, 0, offset.x(), offset.y(), rect.width(),
            rect.height(), data_format, data_type,
            gles2_data ? gles2_data.get() : data);

        if (data_row_length != rect.width())
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    }

    return true;
}

bool GLImageMemory::ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
    int z_order,
    gfx::OverlayTransform transform,
    const gfx::Rect& bounds_rect,
    const gfx::RectF& crop_rect)
{
    return false;
}

// static
unsigned GLImageMemory::GetInternalFormatForTesting(BufferFormat format)
{
    DCHECK(ValidFormat(format));
    return TextureFormat(format);
}

} // namespace gl
