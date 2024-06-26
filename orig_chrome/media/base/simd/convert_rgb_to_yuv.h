// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_SIMD_CONVERT_RGB_TO_YUV_H_
#define MEDIA_BASE_SIMD_CONVERT_RGB_TO_YUV_H_

#include "base/basictypes.h"
#include "media/base/yuv_convert.h"

namespace media {

// These methods are exported for testing purposes only.  Library users should
// only call the methods listed in yuv_convert.h.

MEDIA_EXPORT void ConvertRGB32ToYUV_SSSE3(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride);

MEDIA_EXPORT void ConvertRGB24ToYUV_SSSE3(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride);

MEDIA_EXPORT void ConvertRGB32ToYUV_SSE2(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride);

MEDIA_EXPORT void ConvertRGB32ToYUV_SSE2_Reference(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride);

MEDIA_EXPORT void ConvertRGB32ToYUV_C(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride);

MEDIA_EXPORT void ConvertRGB24ToYUV_C(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride);

} // namespace media

#endif // MEDIA_BASE_SIMD_CONVERT_RGB_TO_YUV_H_
