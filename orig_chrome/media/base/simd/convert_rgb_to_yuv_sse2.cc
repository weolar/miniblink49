// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"
#include "media/base/simd/convert_rgb_to_yuv.h"

#if defined(COMPILER_MSVC)
#include <intrin.h>
#else
#include <emmintrin.h>
#include <mmintrin.h>
#endif

#if defined(COMPILER_MSVC)
#define SIMD_ALIGNED(var) __declspec(align(16)) var
#else
#define SIMD_ALIGNED(var) var __attribute__((aligned(16)))
#endif

namespace media {

#define FIX_SHIFT 12
#define FIX(x) ((x) * (1 << FIX_SHIFT))

// Define a convenient macro to do static cast.
#define INT16_FIX(x) static_cast<int16>(FIX(x))

// Android's pixel layout is RGBA, while other platforms
// are BGRA.
#if defined(OS_ANDROID)
SIMD_ALIGNED(const int16 ConvertRGBAToYUV_kTable[8 * 3]) = {
    INT16_FIX(0.257),
    INT16_FIX(0.504),
    INT16_FIX(0.098),
    0,
    INT16_FIX(0.257),
    INT16_FIX(0.504),
    INT16_FIX(0.098),
    0,
    -INT16_FIX(0.148),
    -INT16_FIX(0.291),
    INT16_FIX(0.439),
    0,
    -INT16_FIX(0.148),
    -INT16_FIX(0.291),
    INT16_FIX(0.439),
    0,
    INT16_FIX(0.439),
    -INT16_FIX(0.368),
    -INT16_FIX(0.071),
    0,
    INT16_FIX(0.439),
    -INT16_FIX(0.368),
    -INT16_FIX(0.071),
    0,
};
#else
SIMD_ALIGNED(const int16 ConvertRGBAToYUV_kTable[8 * 3]) = {
    INT16_FIX(0.098),
    INT16_FIX(0.504),
    INT16_FIX(0.257),
    0,
    INT16_FIX(0.098),
    INT16_FIX(0.504),
    INT16_FIX(0.257),
    0,
    INT16_FIX(0.439),
    -INT16_FIX(0.291),
    -INT16_FIX(0.148),
    0,
    INT16_FIX(0.439),
    -INT16_FIX(0.291),
    -INT16_FIX(0.148),
    0,
    -INT16_FIX(0.071),
    -INT16_FIX(0.368),
    INT16_FIX(0.439),
    0,
    -INT16_FIX(0.071),
    -INT16_FIX(0.368),
    INT16_FIX(0.439),
    0,
};
#endif

#undef INT16_FIX

// This is the final offset for the conversion from signed yuv values to
// unsigned values. It is arranged so that offset of 16 is applied to Y
// components and 128 is added to UV components for 2 pixels.
SIMD_ALIGNED(const int32 kYOffset[4]) = { 16, 16, 16, 16 };

static inline uint8 Clamp(int value)
{
    if (value < 0)
        return 0;
    if (value > 255)
        return 255;
    return static_cast<uint8>(value);
}

static inline uint8 RGBToY(int r, int g, int b)
{
    int y = ConvertRGBAToYUV_kTable[0] * b + ConvertRGBAToYUV_kTable[1] * g + ConvertRGBAToYUV_kTable[2] * r;
    y >>= FIX_SHIFT;
    return Clamp(y + 16);
}

static inline uint8 RGBToU(int r, int g, int b, int shift)
{
    int u = ConvertRGBAToYUV_kTable[8] * b + ConvertRGBAToYUV_kTable[9] * g + ConvertRGBAToYUV_kTable[10] * r;
    u >>= FIX_SHIFT + shift;
    return Clamp(u + 128);
}

static inline uint8 RGBToV(int r, int g, int b, int shift)
{
    int v = ConvertRGBAToYUV_kTable[16] * b + ConvertRGBAToYUV_kTable[17] * g + ConvertRGBAToYUV_kTable[18] * r;
    v >>= FIX_SHIFT + shift;
    return Clamp(v + 128);
}

#define CONVERT_Y(rgb_buf, y_buf) \
    b = *rgb_buf++;               \
    g = *rgb_buf++;               \
    r = *rgb_buf++;               \
    ++rgb_buf;                    \
    sum_b += b;                   \
    sum_g += g;                   \
    sum_r += r;                   \
    *y_buf++ = RGBToY(r, g, b);

static inline void ConvertRGBToYUV_V2H2(const uint8* rgb_buf_1,
    const uint8* rgb_buf_2,
    uint8* y_buf_1,
    uint8* y_buf_2,
    uint8* u_buf,
    uint8* v_buf)
{
    int sum_b = 0;
    int sum_g = 0;
    int sum_r = 0;
    int r, g, b;

    CONVERT_Y(rgb_buf_1, y_buf_1);
    CONVERT_Y(rgb_buf_1, y_buf_1);
    CONVERT_Y(rgb_buf_2, y_buf_2);
    CONVERT_Y(rgb_buf_2, y_buf_2);
    *u_buf++ = RGBToU(sum_r, sum_g, sum_b, 2);
    *v_buf++ = RGBToV(sum_r, sum_g, sum_b, 2);
}

static inline void ConvertRGBToYUV_V2H1(const uint8* rgb_buf_1,
    const uint8* rgb_buf_2,
    uint8* y_buf_1,
    uint8* y_buf_2,
    uint8* u_buf,
    uint8* v_buf)
{
    int sum_b = 0;
    int sum_g = 0;
    int sum_r = 0;
    int r, g, b;

    CONVERT_Y(rgb_buf_1, y_buf_1);
    CONVERT_Y(rgb_buf_2, y_buf_2);
    *u_buf++ = RGBToU(sum_r, sum_g, sum_b, 1);
    *v_buf++ = RGBToV(sum_r, sum_g, sum_b, 1);
}

static inline void ConvertRGBToYUV_V1H2(const uint8* rgb_buf,
    uint8* y_buf,
    uint8* u_buf,
    uint8* v_buf)
{
    int sum_b = 0;
    int sum_g = 0;
    int sum_r = 0;
    int r, g, b;

    CONVERT_Y(rgb_buf, y_buf);
    CONVERT_Y(rgb_buf, y_buf);
    *u_buf++ = RGBToU(sum_r, sum_g, sum_b, 1);
    *v_buf++ = RGBToV(sum_r, sum_g, sum_b, 1);
}

static inline void ConvertRGBToYUV_V1H1(const uint8* rgb_buf,
    uint8* y_buf,
    uint8* u_buf,
    uint8* v_buf)
{
    int sum_b = 0;
    int sum_g = 0;
    int sum_r = 0;
    int r, g, b;

    CONVERT_Y(rgb_buf, y_buf);
    *u_buf++ = RGBToU(r, g, b, 0);
    *v_buf++ = RGBToV(r, g, b, 0);
}

static void ConvertRGB32ToYUVRow_SSE2(const uint8* rgb_buf_1,
    const uint8* rgb_buf_2,
    uint8* y_buf_1,
    uint8* y_buf_2,
    uint8* u_buf,
    uint8* v_buf,
    int width)
{
    while (width >= 4) {
        // Name for the Y pixels:
        // Row 1: a b c d
        // Row 2: e f g h
        //
        // First row 4 pixels.
        __m128i rgb_row_1 = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(rgb_buf_1));
        __m128i zero_1 = _mm_xor_si128(rgb_row_1, rgb_row_1);

        __m128i y_table = _mm_load_si128(
            reinterpret_cast<const __m128i*>(ConvertRGBAToYUV_kTable));

        __m128i rgb_a_b = _mm_unpackhi_epi8(rgb_row_1, zero_1);
        rgb_a_b = _mm_madd_epi16(rgb_a_b, y_table);

        __m128i rgb_c_d = _mm_unpacklo_epi8(rgb_row_1, zero_1);
        rgb_c_d = _mm_madd_epi16(rgb_c_d, y_table);

        // Do a crazh shuffle so that we get:
        //  v------------ Multiply Add
        // BG: a b c d
        // A0: a b c d
        __m128i bg_abcd = _mm_castps_si128(
            _mm_shuffle_ps(
                _mm_castsi128_ps(rgb_c_d),
                _mm_castsi128_ps(rgb_a_b),
                (3 << 6) | (1 << 4) | (3 << 2) | 1));
        __m128i r_abcd = _mm_castps_si128(
            _mm_shuffle_ps(
                _mm_castsi128_ps(rgb_c_d),
                _mm_castsi128_ps(rgb_a_b),
                (2 << 6) | (2 << 2)));
        __m128i y_abcd = _mm_add_epi32(bg_abcd, r_abcd);

        // Down shift back to 8bits range.
        __m128i y_offset = _mm_load_si128(
            reinterpret_cast<const __m128i*>(kYOffset));
        y_abcd = _mm_srai_epi32(y_abcd, FIX_SHIFT);
        y_abcd = _mm_add_epi32(y_abcd, y_offset);
        y_abcd = _mm_packs_epi32(y_abcd, y_abcd);
        y_abcd = _mm_packus_epi16(y_abcd, y_abcd);
        *reinterpret_cast<uint32*>(y_buf_1) = _mm_cvtsi128_si32(y_abcd);
        y_buf_1 += 4;

        // Second row 4 pixels.
        __m128i rgb_row_2 = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(rgb_buf_2));
        __m128i zero_2 = _mm_xor_si128(rgb_row_2, rgb_row_2);
        __m128i rgb_e_f = _mm_unpackhi_epi8(rgb_row_2, zero_2);
        __m128i rgb_g_h = _mm_unpacklo_epi8(rgb_row_2, zero_2);

        // Add two rows together.
        __m128i rgb_ae_bf = _mm_add_epi16(_mm_unpackhi_epi8(rgb_row_1, zero_2), rgb_e_f);
        __m128i rgb_cg_dh = _mm_add_epi16(_mm_unpacklo_epi8(rgb_row_1, zero_2), rgb_g_h);

        // Multiply add like the previous row.
        rgb_e_f = _mm_madd_epi16(rgb_e_f, y_table);
        rgb_g_h = _mm_madd_epi16(rgb_g_h, y_table);

        __m128i bg_efgh = _mm_castps_si128(
            _mm_shuffle_ps(_mm_castsi128_ps(rgb_g_h),
                _mm_castsi128_ps(rgb_e_f),
                (3 << 6) | (1 << 4) | (3 << 2) | 1));
        __m128i r_efgh = _mm_castps_si128(
            _mm_shuffle_ps(_mm_castsi128_ps(rgb_g_h),
                _mm_castsi128_ps(rgb_e_f),
                (2 << 6) | (2 << 2)));
        __m128i y_efgh = _mm_add_epi32(bg_efgh, r_efgh);
        y_efgh = _mm_srai_epi32(y_efgh, FIX_SHIFT);
        y_efgh = _mm_add_epi32(y_efgh, y_offset);
        y_efgh = _mm_packs_epi32(y_efgh, y_efgh);
        y_efgh = _mm_packus_epi16(y_efgh, y_efgh);
        *reinterpret_cast<uint32*>(y_buf_2) = _mm_cvtsi128_si32(y_efgh);
        y_buf_2 += 4;

        __m128i rgb_ae_cg = _mm_castps_si128(
            _mm_shuffle_ps(_mm_castsi128_ps(rgb_cg_dh),
                _mm_castsi128_ps(rgb_ae_bf),
                (3 << 6) | (2 << 4) | (3 << 2) | 2));
        __m128i rgb_bf_dh = _mm_castps_si128(
            _mm_shuffle_ps(_mm_castsi128_ps(rgb_cg_dh),
                _mm_castsi128_ps(rgb_ae_bf),
                (1 << 6) | (1 << 2)));

        // This is a 2x2 subsampling for 2 pixels.
        __m128i rgb_abef_cdgh = _mm_add_epi16(rgb_ae_cg, rgb_bf_dh);

        // Do a multiply add with U table.
        __m128i u_a_b = _mm_madd_epi16(
            rgb_abef_cdgh,
            _mm_load_si128(
                reinterpret_cast<const __m128i*>(ConvertRGBAToYUV_kTable + 8)));
        u_a_b = _mm_add_epi32(_mm_shuffle_epi32(u_a_b, ((3 << 2) | 1)),
            _mm_shuffle_epi32(u_a_b, (2 << 2)));
        // Right shift 14 because of 12 from fixed point and 2 from subsampling.
        u_a_b = _mm_srai_epi32(u_a_b, FIX_SHIFT + 2);
        __m128i uv_offset = _mm_slli_epi32(y_offset, 3);
        u_a_b = _mm_add_epi32(u_a_b, uv_offset);
        u_a_b = _mm_packs_epi32(u_a_b, u_a_b);
        u_a_b = _mm_packus_epi16(u_a_b, u_a_b);
        *reinterpret_cast<uint16*>(u_buf) = static_cast<uint16>(_mm_extract_epi16(u_a_b, 0));
        u_buf += 2;

        __m128i v_a_b = _mm_madd_epi16(
            rgb_abef_cdgh,
            _mm_load_si128(
                reinterpret_cast<const __m128i*>(ConvertRGBAToYUV_kTable + 16)));
        v_a_b = _mm_add_epi32(_mm_shuffle_epi32(v_a_b, ((3 << 2) | 1)),
            _mm_shuffle_epi32(v_a_b, (2 << 2)));
        v_a_b = _mm_srai_epi32(v_a_b, FIX_SHIFT + 2);
        v_a_b = _mm_add_epi32(v_a_b, uv_offset);
        v_a_b = _mm_packs_epi32(v_a_b, v_a_b);
        v_a_b = _mm_packus_epi16(v_a_b, v_a_b);
        *reinterpret_cast<uint16*>(v_buf) = static_cast<uint16>(_mm_extract_epi16(v_a_b, 0));
        v_buf += 2;

        rgb_buf_1 += 16;
        rgb_buf_2 += 16;

        // Move forward by 4 pixels.
        width -= 4;
    }

    // Just use C code to convert the remaining pixels.
    if (width >= 2) {
        ConvertRGBToYUV_V2H2(rgb_buf_1, rgb_buf_2, y_buf_1, y_buf_2, u_buf, v_buf);
        rgb_buf_1 += 8;
        rgb_buf_2 += 8;
        y_buf_1 += 2;
        y_buf_2 += 2;
        ++u_buf;
        ++v_buf;
        width -= 2;
    }

    if (width)
        ConvertRGBToYUV_V2H1(rgb_buf_1, rgb_buf_2, y_buf_1, y_buf_2, u_buf, v_buf);
}

extern void ConvertRGB32ToYUV_SSE2(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride)
{
    while (height >= 2) {
        ConvertRGB32ToYUVRow_SSE2(rgbframe,
            rgbframe + rgbstride,
            yplane,
            yplane + ystride,
            uplane,
            vplane,
            width);
        rgbframe += 2 * rgbstride;
        yplane += 2 * ystride;
        uplane += uvstride;
        vplane += uvstride;
        height -= 2;
    }

    if (!height)
        return;

    // Handle the last row.
    while (width >= 2) {
        ConvertRGBToYUV_V1H2(rgbframe, yplane, uplane, vplane);
        rgbframe += 8;
        yplane += 2;
        ++uplane;
        ++vplane;
        width -= 2;
    }

    if (width)
        ConvertRGBToYUV_V1H1(rgbframe, yplane, uplane, vplane);
}

void ConvertRGB32ToYUV_SSE2_Reference(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride)
{
    while (height >= 2) {
        int i = 0;

        // Convert a 2x2 block.
        while (i + 2 <= width) {
            ConvertRGBToYUV_V2H2(rgbframe + i * 4,
                rgbframe + rgbstride + i * 4,
                yplane + i,
                yplane + ystride + i,
                uplane + i / 2,
                vplane + i / 2);
            i += 2;
        }

        // Convert the last pixel of two rows.
        if (i < width) {
            ConvertRGBToYUV_V2H1(rgbframe + i * 4,
                rgbframe + rgbstride + i * 4,
                yplane + i,
                yplane + ystride + i,
                uplane + i / 2,
                vplane + i / 2);
        }

        rgbframe += 2 * rgbstride;
        yplane += 2 * ystride;
        uplane += uvstride;
        vplane += uvstride;
        height -= 2;
    }

    if (!height)
        return;

    // Handle the last row.
    while (width >= 2) {
        ConvertRGBToYUV_V1H2(rgbframe, yplane, uplane, vplane);
        rgbframe += 8;
        yplane += 2;
        ++uplane;
        ++vplane;
        width -= 2;
    }

    // Handle the last pixel in the last row.
    if (width)
        ConvertRGBToYUV_V1H1(rgbframe, yplane, uplane, vplane);
}

} // namespace media
