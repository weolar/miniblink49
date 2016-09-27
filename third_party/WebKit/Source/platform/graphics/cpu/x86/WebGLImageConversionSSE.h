// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGLImageConversionSSE_h
#define WebGLImageConversionSSE_h

#if CPU(X86) || CPU(X86_64)

#include <emmintrin.h>

namespace blink {

namespace SIMD {

ALWAYS_INLINE void unpackOneRowOfBGRA8LittleToRGBA8(const uint32_t*& source, uint32_t*& destination, unsigned& pixelsPerRow)
{
    __m128i bgra, rgba;
    __m128i brMask = _mm_set1_epi32(0x00ff00ff);
    __m128i gaMask = _mm_set1_epi32(0xff00ff00);
    unsigned pixelsPerRowTrunc = (pixelsPerRow / 4) * 4;

    for (unsigned i = 0; i < pixelsPerRowTrunc; i += 4) {
        bgra = _mm_loadu_si128((const __m128i*)(source));
        rgba = _mm_shufflehi_epi16(_mm_shufflelo_epi16(bgra, 0xB1), 0xB1);

        rgba = _mm_or_si128(_mm_and_si128(rgba, brMask), _mm_and_si128(bgra, gaMask));
        _mm_storeu_si128((__m128i*)(destination), rgba);

        source += 4;
        destination += 4;
    }

    pixelsPerRow -= pixelsPerRowTrunc;
}

ALWAYS_INLINE void packOneRowOfRGBA8LittleToR8(const uint8_t*& source, uint8_t*& destination, unsigned& pixelsPerRow)
{
    float tmp[4];
    unsigned pixelsPerRowTrunc = (pixelsPerRow / 4) * 4;

    for (unsigned i = 0; i < pixelsPerRow; i += 4) {
        __m128 scale = _mm_set_ps(source[15] ? source[15] : 255,
            source[11] ? source[11] : 255,
            source[7] ? source[7] : 255,
            source[3] ? source[3] : 255);

        __m128 sourceR = _mm_set_ps(source[12], source[8], source[4], source[0]);
        sourceR = _mm_mul_ps(sourceR, _mm_div_ps(_mm_set1_ps(255), scale));

        _mm_storeu_ps(tmp, sourceR);
        destination[0] = static_cast<uint8_t>(tmp[0]);
        destination[1] = static_cast<uint8_t>(tmp[1]);
        destination[2] = static_cast<uint8_t>(tmp[2]);
        destination[3] = static_cast<uint8_t>(tmp[3]);

        source += 16;
        destination += 4;
    }

    pixelsPerRow -= pixelsPerRowTrunc;
}

ALWAYS_INLINE void packOneRowOfRGBA8LittleToRGBA8(const uint8_t*& source, uint8_t*& destination, unsigned& pixelsPerRow)
{
    float tmp[4];
    unsigned pixelsPerRowTrunc = (pixelsPerRow / 4) * 4;
    float scale;

    for (unsigned i = 0; i < pixelsPerRow; i++) {
        scale = source[3] ? source[3] : 255;
        __m128 sourceR = _mm_set_ps(0,
            source[2],
            source[1],
            source[0]);

        sourceR = _mm_mul_ps(sourceR, _mm_div_ps(_mm_set1_ps(255), _mm_set1_ps(scale)));

        _mm_storeu_ps(tmp, sourceR);
        destination[0] = static_cast<uint8_t>(tmp[0]);
        destination[1] = static_cast<uint8_t>(tmp[1]);
        destination[2] = static_cast<uint8_t>(tmp[2]);
        destination[3] = source[3];

        source += 4;
        destination += 4;
    }

    pixelsPerRow -= pixelsPerRowTrunc;
}

} // namespace SIMD
} // namespace blink

#endif // CPU(X86) || CPU(X86_64)

#endif // WebGLImageConversionSSE_h
