// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_NATIVE_PIXMAP_HANDLE_OZONE_H_
#define UI_GFX_NATIVE_PIXMAP_HANDLE_OZONE_H_

#include <stdint.h>

#include "base/file_descriptor_posix.h"

namespace gfx {

struct NativePixmapHandle {
    // A file descriptor for the underlying memory object (usually dmabuf).
    base::FileDescriptor fd;

    // The stride to used when accessing the buffer via a memory mapping.
    int32_t stride = 0;
};

} // namespace gfx

#endif // UI_GFX_NATIVE_PIXMAP_HANDLE_OZONE_H_
