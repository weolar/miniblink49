// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/resource_format.h"

#include "third_party/khronos/GLES2/gl2.h"
#include "third_party/khronos/GLES2/gl2ext.h"

namespace cc {

SkColorType ResourceFormatToSkColorType(ResourceFormat format)
{
    switch (format) {
    case RGBA_4444:
        return kARGB_4444_SkColorType;
    case RGBA_8888:
    case BGRA_8888:
        return kN32_SkColorType;
    case ETC1:
    case ALPHA_8:
    case LUMINANCE_8:
    case RGB_565:
    case RED_8:
        NOTREACHED();
        break;
    }
    NOTREACHED();
    return kN32_SkColorType;
}

int BitsPerPixel(ResourceFormat format)
{
    switch (format) {
    case BGRA_8888:
    case RGBA_8888:
        return 32;
    case RGBA_4444:
    case RGB_565:
        return 16;
    case ALPHA_8:
    case LUMINANCE_8:
    case RED_8:
        return 8;
    case ETC1:
        return 4;
    }
    NOTREACHED();
    return 0;
}

GLenum GLDataType(ResourceFormat format)
{
    DCHECK_LE(format, RESOURCE_FORMAT_MAX);
    static const GLenum format_gl_data_type[] = {
        GL_UNSIGNED_BYTE, // RGBA_8888
        GL_UNSIGNED_SHORT_4_4_4_4, // RGBA_4444
        GL_UNSIGNED_BYTE, // BGRA_8888
        GL_UNSIGNED_BYTE, // ALPHA_8
        GL_UNSIGNED_BYTE, // LUMINANCE_8
        GL_UNSIGNED_SHORT_5_6_5, // RGB_565,
        GL_UNSIGNED_BYTE, // ETC1
        GL_UNSIGNED_BYTE // RED_8
    };
    static_assert(arraysize(format_gl_data_type) == (RESOURCE_FORMAT_MAX + 1),
        "format_gl_data_type does not handle all cases.");

    return format_gl_data_type[format];
}

GLenum GLDataFormat(ResourceFormat format)
{
    DCHECK_LE(format, RESOURCE_FORMAT_MAX);
    static const GLenum format_gl_data_format[] = {
        GL_RGBA, // RGBA_8888
        GL_RGBA, // RGBA_4444
        GL_BGRA_EXT, // BGRA_8888
        GL_ALPHA, // ALPHA_8
        GL_LUMINANCE, // LUMINANCE_8
        GL_RGB, // RGB_565
        GL_ETC1_RGB8_OES, // ETC1
        GL_RED_EXT // RED_8
    };
    static_assert(arraysize(format_gl_data_format) == (RESOURCE_FORMAT_MAX + 1),
        "format_gl_data_format does not handle all cases.");

    return format_gl_data_format[format];
}

GLenum GLInternalFormat(ResourceFormat format)
{
    return GLDataFormat(format);
}

gfx::BufferFormat BufferFormat(ResourceFormat format)
{
    switch (format) {
    case RGBA_8888:
        return gfx::BufferFormat::RGBA_8888;
    case BGRA_8888:
        return gfx::BufferFormat::BGRA_8888;
    case RGBA_4444:
        return gfx::BufferFormat::RGBA_4444;
    case ALPHA_8:
    case LUMINANCE_8:
    case RGB_565:
    case ETC1:
    case RED_8:
        break;
    }
    NOTREACHED();
    return gfx::BufferFormat::RGBA_8888;
}

} // namespace cc
