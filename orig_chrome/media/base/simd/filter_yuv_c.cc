// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/simd/filter_yuv.h"

namespace media {

void FilterYUVRows_C(uint8* ybuf, const uint8* y0_ptr, const uint8* y1_ptr,
    int source_width, uint8 source_y_fraction)
{
    uint8 y1_fraction = source_y_fraction;
    uint16 y0_fraction = 256 - y1_fraction;
    uint8* end = ybuf + source_width;
    uint8* rounded_end = ybuf + (source_width & ~7);

    while (ybuf < rounded_end) {
        ybuf[0] = (y0_ptr[0] * y0_fraction + y1_ptr[0] * y1_fraction) >> 8;
        ybuf[1] = (y0_ptr[1] * y0_fraction + y1_ptr[1] * y1_fraction) >> 8;
        ybuf[2] = (y0_ptr[2] * y0_fraction + y1_ptr[2] * y1_fraction) >> 8;
        ybuf[3] = (y0_ptr[3] * y0_fraction + y1_ptr[3] * y1_fraction) >> 8;
        ybuf[4] = (y0_ptr[4] * y0_fraction + y1_ptr[4] * y1_fraction) >> 8;
        ybuf[5] = (y0_ptr[5] * y0_fraction + y1_ptr[5] * y1_fraction) >> 8;
        ybuf[6] = (y0_ptr[6] * y0_fraction + y1_ptr[6] * y1_fraction) >> 8;
        ybuf[7] = (y0_ptr[7] * y0_fraction + y1_ptr[7] * y1_fraction) >> 8;
        y0_ptr += 8;
        y1_ptr += 8;
        ybuf += 8;
    }

    while (ybuf < end) {
        ybuf[0] = (y0_ptr[0] * y0_fraction + y1_ptr[0] * y1_fraction) >> 8;
        ++ybuf;
        ++y0_ptr;
        ++y1_ptr;
    }
}

} // namespace media
