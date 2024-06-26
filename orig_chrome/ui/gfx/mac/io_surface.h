// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_MAC_IO_SURFACE_H_
#define UI_GFX_MAC_IO_SURFACE_H_

#include <IOSurface/IOSurface.h>
#include <mach/mach.h>

#include "base/mac/scoped_cftyperef.h"
#include "ui/gfx/buffer_types.h"
#include "ui/gfx/generic_shared_memory_id.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gfx_export.h"

namespace gfx {

namespace internal {

    struct IOSurfaceMachPortTraits {
        GFX_EXPORT static mach_port_t InvalidValue() { return MACH_PORT_NULL; }
        GFX_EXPORT static mach_port_t Retain(mach_port_t);
        GFX_EXPORT static void Release(mach_port_t);
    };

} // namespace internal

using IOSurfaceId = GenericSharedMemoryId;

// Helper function to create an IOSurface with a specified size and format.
GFX_EXPORT IOSurfaceRef CreateIOSurface(const Size& size, BufferFormat format);

// A scoper for handling Mach port names that are send rights for IOSurfaces.
// This scoper is both copyable and assignable, which will increase the kernel
// reference count of the right. On destruction, the reference count is
// decremented.
using ScopedRefCountedIOSurfaceMachPort = base::ScopedTypeRef<mach_port_t, internal::IOSurfaceMachPortTraits>;

} // namespace gfx

#endif // UI_GFX_MAC_IO_SURFACE_H_
