// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/mac/io_surface.h"

#include <stddef.h>
#include <stdint.h>

#include "base/logging.h"
#include "base/mac/mach_logging.h"
#include "base/macros.h"
#include "ui/gfx/buffer_format_util.h"

namespace gfx {

namespace {

    void AddIntegerValue(CFMutableDictionaryRef dictionary,
        const CFStringRef key,
        int32_t value)
    {
        base::ScopedCFTypeRef<CFNumberRef> number(
            CFNumberCreate(NULL, kCFNumberSInt32Type, &value));
        CFDictionaryAddValue(dictionary, key, number.get());
    }

    int32_t BytesPerElement(gfx::BufferFormat format, int plane)
    {
        switch (format) {
        case gfx::BufferFormat::R_8:
            DCHECK_EQ(plane, 0);
            return 1;
        case gfx::BufferFormat::BGRA_8888:
        case gfx::BufferFormat::RGBA_8888:
            DCHECK_EQ(plane, 0);
            return 4;
        case gfx::BufferFormat::YUV_420_BIPLANAR:
            static int32_t bytes_per_element[] = { 1, 2 };
            DCHECK_LT(static_cast<size_t>(plane), arraysize(bytes_per_element));
            return bytes_per_element[plane];
        case gfx::BufferFormat::UYVY_422:
            DCHECK_EQ(plane, 0);
            return 2;
        case gfx::BufferFormat::ATC:
        case gfx::BufferFormat::ATCIA:
        case gfx::BufferFormat::DXT1:
        case gfx::BufferFormat::DXT5:
        case gfx::BufferFormat::ETC1:
        case gfx::BufferFormat::RGBA_4444:
        case gfx::BufferFormat::RGBX_8888:
        case gfx::BufferFormat::BGRX_8888:
        case gfx::BufferFormat::YUV_420:
            NOTREACHED();
            return 0;
        }

        NOTREACHED();
        return 0;
    }

    int32_t PixelFormat(gfx::BufferFormat format)
    {
        switch (format) {
        case gfx::BufferFormat::R_8:
            return 'L008';
        case gfx::BufferFormat::BGRA_8888:
        case gfx::BufferFormat::RGBA_8888:
            return 'BGRA';
        case gfx::BufferFormat::YUV_420_BIPLANAR:
            return '420v';
        case gfx::BufferFormat::UYVY_422:
            return '2vuy';
        case gfx::BufferFormat::ATC:
        case gfx::BufferFormat::ATCIA:
        case gfx::BufferFormat::DXT1:
        case gfx::BufferFormat::DXT5:
        case gfx::BufferFormat::ETC1:
        case gfx::BufferFormat::RGBA_4444:
        case gfx::BufferFormat::RGBX_8888:
        case gfx::BufferFormat::BGRX_8888:
        case gfx::BufferFormat::YUV_420:
            NOTREACHED();
            return 0;
        }

        NOTREACHED();
        return 0;
    }

} // namespace

namespace internal {

    // static
    mach_port_t IOSurfaceMachPortTraits::Retain(mach_port_t port)
    {
        kern_return_t kr = mach_port_mod_refs(mach_task_self(), port, MACH_PORT_RIGHT_SEND, 1);
        MACH_LOG_IF(ERROR, kr != KERN_SUCCESS, kr)
            << "IOSurfaceMachPortTraits::Retain mach_port_mod_refs";
        return port;
    }

    // static
    void IOSurfaceMachPortTraits::Release(mach_port_t port)
    {
        kern_return_t kr = mach_port_mod_refs(mach_task_self(), port, MACH_PORT_RIGHT_SEND, -1);
        MACH_LOG_IF(ERROR, kr != KERN_SUCCESS, kr)
            << "IOSurfaceMachPortTraits::Release mach_port_mod_refs";
    }

} // namespace internal

IOSurfaceRef CreateIOSurface(const gfx::Size& size, gfx::BufferFormat format)
{
    size_t num_planes = gfx::NumberOfPlanesForBufferFormat(format);
    base::ScopedCFTypeRef<CFMutableArrayRef> planes(CFArrayCreateMutable(
        kCFAllocatorDefault, num_planes, &kCFTypeArrayCallBacks));

    // Don't specify plane information unless there are indeed multiple planes
    // because DisplayLink drivers do not support this.
    // http://crbug.com/527556
    if (num_planes > 1) {
        for (size_t plane = 0; plane < num_planes; ++plane) {
            size_t factor = gfx::SubsamplingFactorForBufferFormat(format, plane);

            base::ScopedCFTypeRef<CFMutableDictionaryRef> plane_info(
                CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                    &kCFTypeDictionaryKeyCallBacks,
                    &kCFTypeDictionaryValueCallBacks));
            AddIntegerValue(plane_info, kIOSurfacePlaneWidth, size.width() / factor);
            AddIntegerValue(plane_info, kIOSurfacePlaneHeight,
                size.height() / factor);
            AddIntegerValue(plane_info, kIOSurfacePlaneBytesPerElement,
                BytesPerElement(format, plane));

            CFArrayAppendValue(planes, plane_info);
        }
    }

    base::ScopedCFTypeRef<CFMutableDictionaryRef> properties(
        CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
            &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks));
    AddIntegerValue(properties, kIOSurfaceWidth, size.width());
    AddIntegerValue(properties, kIOSurfaceHeight, size.height());
    AddIntegerValue(properties, kIOSurfacePixelFormat, PixelFormat(format));
    if (num_planes > 1) {
        CFDictionaryAddValue(properties, kIOSurfacePlaneInfo, planes);
    } else {
        AddIntegerValue(properties, kIOSurfaceBytesPerElement,
            BytesPerElement(format, 0));
    }

    IOSurfaceRef surface = IOSurfaceCreate(properties);

    // Zero-initialize the IOSurface. Calling IOSurfaceLock/IOSurfaceUnlock
    // appears to be sufficient. https://crbug.com/584760#c17
    IOReturn r = IOSurfaceLock(surface, 0, nullptr);
    DCHECK_EQ(kIOReturnSuccess, r);
    r = IOSurfaceUnlock(surface, 0, nullptr);
    DCHECK_EQ(kIOReturnSuccess, r);

    return surface;
}

} // namespace gfx
