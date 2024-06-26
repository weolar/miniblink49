// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_VIDEO_TYPES_H_
#define MEDIA_BASE_VIDEO_TYPES_H_

#include <string>

#include "build/build_config.h"
#include "media/base/media_export.h"

namespace media {

// Pixel formats roughly based on FOURCC labels, see:
// http://www.fourcc.org/rgb.php and http://www.fourcc.org/yuv.php
// Logged to UMA, so never reuse values. Leave gaps if necessary.
// Ordered as planar, semi-planar, YUV-packed, and RGB formats.
enum VideoPixelFormat {
    PIXEL_FORMAT_UNKNOWN = 0, // Unknown or unspecified format value.
    PIXEL_FORMAT_I420 = 1, // 12bpp YUV planar 1x1 Y, 2x2 UV samples, a.k.a. YU12.
    PIXEL_FORMAT_YV12 = 2, // 12bpp YVU planar 1x1 Y, 2x2 VU samples.
    PIXEL_FORMAT_YV16 = 3, // 16bpp YVU planar 1x1 Y, 2x1 VU samples.
    PIXEL_FORMAT_YV12A = 4, // 20bpp YUVA planar 1x1 Y, 2x2 VU, 1x1 A samples.
    PIXEL_FORMAT_YV24 = 5, // 24bpp YUV planar, no subsampling.
    PIXEL_FORMAT_NV12 = 6, // 12bpp with Y plane followed by a 2x2 interleaved UV plane.
    PIXEL_FORMAT_NV21 = 7, // 12bpp with Y plane followed by a 2x2 interleaved VU plane.
    PIXEL_FORMAT_UYVY = 8, // 16bpp interleaved 2x1 U, 1x1 Y, 2x1 V, 1x1 Y samples.
    PIXEL_FORMAT_YUY2 = 9, // 16bpp interleaved 1x1 Y, 2x1 U, 1x1 Y, 2x1 V samples.
    PIXEL_FORMAT_ARGB = 10, // 32bpp ARGB, 1 plane.
    PIXEL_FORMAT_XRGB = 11, // 24bpp XRGB, 1 plane.
    PIXEL_FORMAT_RGB24 = 12, // 24bpp BGR, 1 plane.
    PIXEL_FORMAT_RGB32 = 13, // 32bpp BGRA, 1 plane.
    PIXEL_FORMAT_MJPEG = 14, // MJPEG compressed.
    // MediaTek proprietary format. MT21 is similar to NV21 except the memory
    // layout and pixel layout (swizzles). 12bpp with Y plane followed by a 2x2
    // interleaved VU plane. Each image contains two buffers -- Y plane and VU
    // plane. Two planes can be non-contiguous in memory. The starting addresses
    // of Y plane and VU plane are 4KB alignment.
    // Suppose image dimension is (width, height). For both Y plane and VU plane:
    // Row pitch = ((width+15)/16) * 16.
    // Plane size = Row pitch * (((height+31)/32)*32)
    PIXEL_FORMAT_MT21 = 15,
    // Please update UMA histogram enumeration when adding new formats here.
    PIXEL_FORMAT_MAX = PIXEL_FORMAT_MT21, // Must always be equal to largest entry logged.
};

// Color space or color range used for the pixels.
// Logged to UMA, so never reuse values. Leave gaps if necessary.
enum ColorSpace {
    COLOR_SPACE_UNSPECIFIED = 0, // In general this is Rec601.
    // The JPEG color space is the combination of Rec.601 and full range colors
    // (aka pc range colors).
    COLOR_SPACE_JPEG = 1,
    COLOR_SPACE_HD_REC709 = 2, // Rec709 "HD" color space.
    COLOR_SPACE_SD_REC601 = 3, // Rec601 "SD" color space.
    COLOR_SPACE_MAX = COLOR_SPACE_SD_REC601,
};

// Returns the name of a Format as a string.
MEDIA_EXPORT std::string VideoPixelFormatToString(VideoPixelFormat format);

// Returns true if |format| is a YUV format with multiple planes.
MEDIA_EXPORT bool IsYuvPlanar(VideoPixelFormat format);

} // namespace media

#endif // MEDIA_BASE_VIDEO_TYPES_H_
