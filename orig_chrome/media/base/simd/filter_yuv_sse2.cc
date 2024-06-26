// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <emmintrin.h>
#include <mmintrin.h>
#endif

#include "media/base/simd/filter_yuv.h"

namespace media {

void FilterYUVRows_SSE2(uint8* dest,
    const uint8* src0,
    const uint8* src1,
    int width,
    uint8 fraction)
{
    int pixel = 0;

    // Process the unaligned bytes first.
    int unaligned_width = (16 - (reinterpret_cast<uintptr_t>(dest) & 15)) & 15;
    while (pixel < width && pixel < unaligned_width) {
        dest[pixel] = (src0[pixel] * (256 - fraction) + src1[pixel] * fraction) >> 8;
        ++pixel;
    }

    __m128i zero = _mm_setzero_si128();
    __m128i src1_fraction = _mm_set1_epi16(fraction);
    __m128i src0_fraction = _mm_set1_epi16(256 - fraction);
    const __m128i* src0_128 = reinterpret_cast<const __m128i*>(src0 + pixel);
    const __m128i* src1_128 = reinterpret_cast<const __m128i*>(src1 + pixel);
    __m128i* dest128 = reinterpret_cast<__m128i*>(dest + pixel);
    __m128i* end128 = reinterpret_cast<__m128i*>(
        reinterpret_cast<uintptr_t>(dest + width) & ~15);

    while (dest128 < end128) {
        __m128i src0 = _mm_loadu_si128(src0_128);
        __m128i src1 = _mm_loadu_si128(src1_128);
        __m128i src2 = _mm_unpackhi_epi8(src0, zero);
        __m128i src3 = _mm_unpackhi_epi8(src1, zero);
        src0 = _mm_unpacklo_epi8(src0, zero);
        src1 = _mm_unpacklo_epi8(src1, zero);
        src0 = _mm_mullo_epi16(src0, src0_fraction);
        src1 = _mm_mullo_epi16(src1, src1_fraction);
        src2 = _mm_mullo_epi16(src2, src0_fraction);
        src3 = _mm_mullo_epi16(src3, src1_fraction);
        src0 = _mm_add_epi16(src0, src1);
        src2 = _mm_add_epi16(src2, src3);
        src0 = _mm_srli_epi16(src0, 8);
        src2 = _mm_srli_epi16(src2, 8);
        src0 = _mm_packus_epi16(src0, src2);
        *dest128++ = src0;
        ++src0_128;
        ++src1_128;
        pixel += 16;
    }

    while (pixel < width) {
        dest[pixel] = (src0[pixel] * (256 - fraction) + src1[pixel] * fraction) >> 8;
        ++pixel;
    }
}

} // namespace media
