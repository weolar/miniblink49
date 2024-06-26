// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <mmintrin.h>
#endif

#include "media/base/simd/convert_yuv_to_rgb.h"
#include "media/base/yuv_convert.h"

namespace media {

void ConvertYUVAToARGB_MMX(const uint8* yplane,
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
    YUVType yuv_type)
{
    unsigned int y_shift = GetVerticalShift(yuv_type);
    for (int y = 0; y < height; ++y) {
        uint8* rgb_row = rgbframe + y * rgbstride;
        const uint8* y_ptr = yplane + y * ystride;
        const uint8* u_ptr = uplane + (y >> y_shift) * uvstride;
        const uint8* v_ptr = vplane + (y >> y_shift) * uvstride;
        const uint8* a_ptr = aplane + y * astride;

        ConvertYUVAToARGBRow_MMX(y_ptr,
            u_ptr,
            v_ptr,
            a_ptr,
            rgb_row,
            width,
            GetLookupTable(yuv_type));
    }

    EmptyRegisterState();
}

void ConvertYUVToRGB32_SSE(const uint8* yplane,
    const uint8* uplane,
    const uint8* vplane,
    uint8* rgbframe,
    int width,
    int height,
    int ystride,
    int uvstride,
    int rgbstride,
    YUVType yuv_type)
{
    unsigned int y_shift = GetVerticalShift(yuv_type);
    for (int y = 0; y < height; ++y) {
        uint8* rgb_row = rgbframe + y * rgbstride;
        const uint8* y_ptr = yplane + y * ystride;
        const uint8* u_ptr = uplane + (y >> y_shift) * uvstride;
        const uint8* v_ptr = vplane + (y >> y_shift) * uvstride;

        ConvertYUVToRGB32Row_SSE(y_ptr,
            u_ptr,
            v_ptr,
            rgb_row,
            width,
            GetLookupTable(yuv_type));
    }

    EmptyRegisterState();
}

} // namespace media
