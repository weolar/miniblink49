// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_YUV_CONVERT_H_
#define MEDIA_BASE_YUV_CONVERT_H_

#include "base/basictypes.h"
#include "media/base/media_export.h"

// Visual Studio 2010 does not support MMX intrinsics on x64.
// Some win64 yuv_convert code paths use SSE+MMX yasm, so without rewriting
// them, we use yasm EmptyRegisterState_MMX in place of _mm_empty() or
// hide the versions implemented with heavy use of MMX intrinsics.
// TODO(wolenetz): Use MMX intrinsics when compiling win64 with Visual
// Studio 2012? http://crbug.com/173450
#if defined(ARCH_CPU_X86_FAMILY) && !(defined(ARCH_CPU_X86_64) && defined(COMPILER_MSVC))
#define MEDIA_MMX_INTRINSICS_AVAILABLE
#endif

namespace media {

// Type of YUV surface.
enum YUVType {
    YV16 = 0, // YV16 is half width and full height chroma channels.
    YV12 = 1, // YV12 is half width and half height chroma channels.
    YV12J = 2, // YV12J is the same as YV12, but in JPEG color range.
    YV12HD = 3, // YV12HD is the same as YV12, but in 'HD' Rec709 color space.
};

// Get the appropriate value to bitshift by for vertical indices.
MEDIA_EXPORT int GetVerticalShift(YUVType type);

// Get the appropriate lookup table for a given YUV format.
MEDIA_EXPORT const int16* GetLookupTable(YUVType type);

// Mirror means flip the image horizontally, as in looking in a mirror.
// Rotate happens after mirroring.
enum Rotate {
    ROTATE_0, // Rotation off.
    ROTATE_90, // Rotate clockwise.
    ROTATE_180, // Rotate upside down.
    ROTATE_270, // Rotate counter clockwise.
    MIRROR_ROTATE_0, // Mirror horizontally.
    MIRROR_ROTATE_90, // Mirror then Rotate clockwise.
    MIRROR_ROTATE_180, // Mirror vertically.
    MIRROR_ROTATE_270, // Transpose.
};

// Filter affects how scaling looks.
enum ScaleFilter {
    FILTER_NONE = 0, // No filter (point sampled).
    FILTER_BILINEAR_H = 1, // Bilinear horizontal filter.
    FILTER_BILINEAR_V = 2, // Bilinear vertical filter.
    FILTER_BILINEAR = 3, // Bilinear filter.
};

MEDIA_EXPORT void InitializeCPUSpecificYUVConversions();

// Convert a frame of YUV to 32 bit ARGB.
// Pass in YV16/YV12 depending on source format
MEDIA_EXPORT void ConvertYUVToRGB32(const uint8* yplane,
    const uint8* uplane,
    const uint8* vplane,
    uint8* rgbframe,
    int width,
    int height,
    int ystride,
    int uvstride,
    int rgbstride,
    YUVType yuv_type);

// Convert a frame of YUVA to 32 bit ARGB.
// Pass in YV12A
MEDIA_EXPORT void ConvertYUVAToARGB(const uint8* yplane,
    const uint8* uplane,
    const uint8* vplane,
    const uint8* aplane,
    uint8* rgbframe,
    int width,
    int height,
    int ystride,
    int uvstride,
    int astride,
    int rgbstride,
    YUVType yuv_type);

// Scale a frame of YUV to 32 bit ARGB.
// Supports rotation and mirroring.
MEDIA_EXPORT void ScaleYUVToRGB32(const uint8* yplane,
    const uint8* uplane,
    const uint8* vplane,
    uint8* rgbframe,
    int source_width,
    int source_height,
    int width,
    int height,
    int ystride,
    int uvstride,
    int rgbstride,
    YUVType yuv_type,
    Rotate view_rotate,
    ScaleFilter filter);

// Biliner Scale a frame of YV12 to 32 bits ARGB on a specified rectangle.
// |yplane|, etc and |rgbframe| should point to the top-left pixels of the
// source and destination buffers.
MEDIA_EXPORT void ScaleYUVToRGB32WithRect(const uint8* yplane,
    const uint8* uplane,
    const uint8* vplane,
    uint8* rgbframe,
    int source_width,
    int source_height,
    int dest_width,
    int dest_height,
    int dest_rect_left,
    int dest_rect_top,
    int dest_rect_right,
    int dest_rect_bottom,
    int ystride,
    int uvstride,
    int rgbstride);

MEDIA_EXPORT void ConvertRGB32ToYUV(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride);

MEDIA_EXPORT void ConvertRGB24ToYUV(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride);

// Empty SIMD register state after calling optimized scaler functions.
MEDIA_EXPORT void EmptyRegisterState();

} // namespace media

#endif // MEDIA_BASE_YUV_CONVERT_H_
