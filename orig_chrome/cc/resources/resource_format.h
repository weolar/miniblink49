// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_RESOURCE_FORMAT_H_
#define CC_RESOURCES_RESOURCE_FORMAT_H_

#include "base/logging.h"
#include "cc/base/cc_export.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/gpu_memory_buffer.h"

// TODO(prashant.n): Including third_party/khronos/GLES2/gl2.h causes
// redefinition errors as macros/functions defined in it conflict with
// macros/functions defined in ui/gl/gl_bindings.h. (http://crbug.com/512833).
typedef unsigned int GLenum;

namespace cc {

// Keep in sync with arrays below.
enum ResourceFormat {
    RGBA_8888,
    RGBA_4444,
    BGRA_8888,
    ALPHA_8,
    LUMINANCE_8,
    RGB_565,
    ETC1,
    RED_8,
    RESOURCE_FORMAT_MAX = RED_8,
};

SkColorType ResourceFormatToSkColorType(ResourceFormat format);

CC_EXPORT int BitsPerPixel(ResourceFormat format);
CC_EXPORT GLenum GLDataType(ResourceFormat format);
CC_EXPORT GLenum GLDataFormat(ResourceFormat format);
CC_EXPORT GLenum GLInternalFormat(ResourceFormat format);
CC_EXPORT gfx::BufferFormat BufferFormat(ResourceFormat format);

} // namespace cc

#endif // CC_RESOURCES_RESOURCE_FORMAT_H_
