// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/simd/convert_rgb_to_yuv.h"

#include "build/build_config.h"
#include "media/base/simd/convert_rgb_to_yuv_ssse3.h"

namespace media {

void ConvertRGB32ToYUV_SSSE3(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride)
{
    for (; height >= 2; height -= 2) {
        ConvertARGBToYUVRow_SSSE3(rgbframe, yplane, uplane, vplane, width);
        rgbframe += rgbstride;
        yplane += ystride;

        ConvertARGBToYUVRow_SSSE3(rgbframe, yplane, NULL, NULL, width);
        rgbframe += rgbstride;
        yplane += ystride;

        uplane += uvstride;
        vplane += uvstride;
    }

    if (height)
        ConvertARGBToYUVRow_SSSE3(rgbframe, yplane, uplane, vplane, width);
}

void ConvertRGB24ToYUV_SSSE3(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride)
{
    for (; height >= 2; height -= 2) {
        ConvertRGBToYUVRow_SSSE3(rgbframe, yplane, uplane, vplane, width);
        rgbframe += rgbstride;
        yplane += ystride;

        ConvertRGBToYUVRow_SSSE3(rgbframe, yplane, NULL, NULL, width);
        rgbframe += rgbstride;
        yplane += ystride;

        uplane += uvstride;
        vplane += uvstride;
    }

    if (height)
        ConvertRGBToYUVRow_SSSE3(rgbframe, yplane, uplane, vplane, width);
}

} // namespace media
