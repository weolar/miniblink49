// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_BUFFER_TYPES_H_
#define UI_GFX_BUFFER_TYPES_H_

namespace gfx {

// The format needs to be taken into account when mapping a buffer into the
// client's address space.
enum class BufferFormat {
    ATC,
    ATCIA,
    DXT1,
    DXT5,
    ETC1,
    R_8,
    RGBA_4444,
    RGBX_8888,
    RGBA_8888,
    BGRX_8888,
    BGRA_8888,
    YUV_420,
    YUV_420_BIPLANAR,
    UYVY_422,

    LAST = UYVY_422
};

// The usage mode affects how a buffer can be used. Only buffers created with
// *_CPU_READ_WRITE_* can be mapped into the client's address space and accessed
// by the CPU. *_CPU_READ_WRITE_PERSISTENT adds the additional condition that
// successive Map() calls (with Unmap() calls between) will return a pointer to
// the same memory contents. SCANOUT implies GPU_READ_WRITE.
// TODO(reveman): Add GPU_READ_WRITE for use-cases where SCANOUT is not
// required.
enum class BufferUsage {
    GPU_READ,
    SCANOUT,
    GPU_READ_CPU_READ_WRITE,
    // TODO(reveman): Merge this with GPU_READ_CPU_READ_WRITE when SurfaceTexture
    // backed buffers are single buffered and support it.
    GPU_READ_CPU_READ_WRITE_PERSISTENT,

    LAST = GPU_READ_CPU_READ_WRITE_PERSISTENT
};

} // namespace gfx

#endif // UI_GFX_BUFFER_TYPES_H_
