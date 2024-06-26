// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/texture_compressor_etc1_sse.h"

#include <emmintrin.h>

#include "base/compiler_specific.h"
#include "base/logging.h"
// Using this header for common functions such as Color handling
// and codeword table.
#include "cc/raster/texture_compressor_etc1.h"

namespace cc {

namespace {

    inline uint32_t SetETC1MaxError(uint32_t avg_error)
    {
        // ETC1 codeword table is sorted in ascending order.
        // Our algorithm will try to identify the index that generates the minimum
        // error.
        // The min error calculated during ComputeLuminance main loop will converge
        // towards that value.
        // We use this threshold to determine when it doesn't make sense to iterate
        // further through the array.
        return avg_error + avg_error / 2 + 384;
    }

    struct __sse_data {
        // This is used to store raw data.
        uint8_t* block;
        // This is used to store 8 bit packed values.
        __m128i* packed;
        // This is used to store 32 bit zero extended values into 4x4 arrays.
        __m128i* blue;
        __m128i* green;
        __m128i* red;
    };

    // Commonly used registers throughout the code.
    static const __m128i __sse_zero = _mm_set1_epi32(0);
    static const __m128i __sse_max_int = _mm_set1_epi32(0x7FFFFFFF);

    inline __m128i AddAndClamp(const __m128i x, const __m128i y)
    {
        static const __m128i color_max = _mm_set1_epi32(0xFF);
        return _mm_max_epi16(__sse_zero,
            _mm_min_epi16(_mm_add_epi16(x, y), color_max));
    }

    inline __m128i GetColorErrorSSE(const __m128i x, const __m128i y)
    {
        // Changed from _mm_mullo_epi32 (SSE4) to _mm_mullo_epi16 (SSE2).
        __m128i ret = _mm_sub_epi16(x, y);
        return _mm_mullo_epi16(ret, ret);
    }

    inline __m128i AddChannelError(const __m128i x,
        const __m128i y,
        const __m128i z)
    {
        return _mm_add_epi32(x, _mm_add_epi32(y, z));
    }

    inline uint32_t SumSSE(const __m128i x)
    {
        __m128i sum = _mm_add_epi32(x, _mm_shuffle_epi32(x, 0x4E));
        sum = _mm_add_epi32(sum, _mm_shuffle_epi32(sum, 0xB1));

        return _mm_cvtsi128_si32(sum);
    }

    inline uint32_t GetVerticalError(const __sse_data* data,
        const __m128i* blue_avg,
        const __m128i* green_avg,
        const __m128i* red_avg,
        uint32_t* verror)
    {
        __m128i error = __sse_zero;

        for (int i = 0; i < 4; i++) {
            error = _mm_add_epi32(error, GetColorErrorSSE(data->blue[i], blue_avg[0]));
            error = _mm_add_epi32(error, GetColorErrorSSE(data->green[i], green_avg[0]));
            error = _mm_add_epi32(error, GetColorErrorSSE(data->red[i], red_avg[0]));
        }

        error = _mm_add_epi32(error, _mm_shuffle_epi32(error, 0x4E));

        verror[0] = _mm_cvtsi128_si32(error);
        verror[1] = _mm_cvtsi128_si32(_mm_shuffle_epi32(error, 0xB1));

        return verror[0] + verror[1];
    }

    inline uint32_t GetHorizontalError(const __sse_data* data,
        const __m128i* blue_avg,
        const __m128i* green_avg,
        const __m128i* red_avg,
        uint32_t* verror)
    {
        __m128i error = __sse_zero;
        int first_index, second_index;

        for (int i = 0; i < 2; i++) {
            first_index = 2 * i;
            second_index = first_index + 1;

            error = _mm_add_epi32(
                error, GetColorErrorSSE(data->blue[first_index], blue_avg[i]));
            error = _mm_add_epi32(
                error, GetColorErrorSSE(data->blue[second_index], blue_avg[i]));
            error = _mm_add_epi32(
                error, GetColorErrorSSE(data->green[first_index], green_avg[i]));
            error = _mm_add_epi32(
                error, GetColorErrorSSE(data->green[second_index], green_avg[i]));
            error = _mm_add_epi32(error,
                GetColorErrorSSE(data->red[first_index], red_avg[i]));
            error = _mm_add_epi32(
                error, GetColorErrorSSE(data->red[second_index], red_avg[i]));
        }

        error = _mm_add_epi32(error, _mm_shuffle_epi32(error, 0x4E));

        verror[0] = _mm_cvtsi128_si32(error);
        verror[1] = _mm_cvtsi128_si32(_mm_shuffle_epi32(error, 0xB1));

        return verror[0] + verror[1];
    }

    inline void GetAvgColors(const __sse_data* data,
        float* output,
        bool* __sse_use_diff)
    {
        __m128i sum[2], tmp;

        // TODO(radu.velea): _mm_avg_epu8 on packed data maybe.

        // Compute avg red value.
        // [S0 S0 S1 S1]
        sum[0] = _mm_add_epi32(data->red[0], data->red[1]);
        sum[0] = _mm_add_epi32(sum[0], _mm_shuffle_epi32(sum[0], 0xB1));

        // [S2 S2 S3 S3]
        sum[1] = _mm_add_epi32(data->red[2], data->red[3]);
        sum[1] = _mm_add_epi32(sum[1], _mm_shuffle_epi32(sum[1], 0xB1));

        float hred[2], vred[2];
        hred[0] = (_mm_cvtsi128_si32(
                      _mm_add_epi32(sum[0], _mm_shuffle_epi32(sum[0], 0x4E))))
            / 8.0f;
        hred[1] = (_mm_cvtsi128_si32(
                      _mm_add_epi32(sum[1], _mm_shuffle_epi32(sum[1], 0x4E))))
            / 8.0f;

        tmp = _mm_add_epi32(sum[0], sum[1]);
        vred[0] = (_mm_cvtsi128_si32(tmp)) / 8.0f;
        vred[1] = (_mm_cvtsi128_si32(_mm_shuffle_epi32(tmp, 0x2))) / 8.0f;

        // Compute avg green value.
        // [S0 S0 S1 S1]
        sum[0] = _mm_add_epi32(data->green[0], data->green[1]);
        sum[0] = _mm_add_epi32(sum[0], _mm_shuffle_epi32(sum[0], 0xB1));

        // [S2 S2 S3 S3]
        sum[1] = _mm_add_epi32(data->green[2], data->green[3]);
        sum[1] = _mm_add_epi32(sum[1], _mm_shuffle_epi32(sum[1], 0xB1));

        float hgreen[2], vgreen[2];
        hgreen[0] = (_mm_cvtsi128_si32(
                        _mm_add_epi32(sum[0], _mm_shuffle_epi32(sum[0], 0x4E))))
            / 8.0f;
        hgreen[1] = (_mm_cvtsi128_si32(
                        _mm_add_epi32(sum[1], _mm_shuffle_epi32(sum[1], 0x4E))))
            / 8.0f;

        tmp = _mm_add_epi32(sum[0], sum[1]);
        vgreen[0] = (_mm_cvtsi128_si32(tmp)) / 8.0f;
        vgreen[1] = (_mm_cvtsi128_si32(_mm_shuffle_epi32(tmp, 0x2))) / 8.0f;

        // Compute avg blue value.
        // [S0 S0 S1 S1]
        sum[0] = _mm_add_epi32(data->blue[0], data->blue[1]);
        sum[0] = _mm_add_epi32(sum[0], _mm_shuffle_epi32(sum[0], 0xB1));

        // [S2 S2 S3 S3]
        sum[1] = _mm_add_epi32(data->blue[2], data->blue[3]);
        sum[1] = _mm_add_epi32(sum[1], _mm_shuffle_epi32(sum[1], 0xB1));

        float hblue[2], vblue[2];
        hblue[0] = (_mm_cvtsi128_si32(
                       _mm_add_epi32(sum[0], _mm_shuffle_epi32(sum[0], 0x4E))))
            / 8.0f;
        hblue[1] = (_mm_cvtsi128_si32(
                       _mm_add_epi32(sum[1], _mm_shuffle_epi32(sum[1], 0x4E))))
            / 8.0f;

        tmp = _mm_add_epi32(sum[0], sum[1]);
        vblue[0] = (_mm_cvtsi128_si32(tmp)) / 8.0f;
        vblue[1] = (_mm_cvtsi128_si32(_mm_shuffle_epi32(tmp, 0x2))) / 8.0f;

        // TODO(radu.velea): Return int's instead of floats, based on Quality.
        output[0] = vblue[0];
        output[1] = vgreen[0];
        output[2] = vred[0];

        output[3] = vblue[1];
        output[4] = vgreen[1];
        output[5] = vred[1];

        output[6] = hblue[0];
        output[7] = hgreen[0];
        output[8] = hred[0];

        output[9] = hblue[1];
        output[10] = hgreen[1];
        output[11] = hred[1];

        __m128i threshold_upper = _mm_set1_epi32(3);
        __m128i threshold_lower = _mm_set1_epi32(-4);

        __m128 factor_v = _mm_set1_ps(31.0f / 255.0f);
        __m128 rounding_v = _mm_set1_ps(0.5f);
        __m128 h_avg_0 = _mm_set_ps(hblue[0], hgreen[0], hred[0], 0);
        __m128 h_avg_1 = _mm_set_ps(hblue[1], hgreen[1], hred[1], 0);

        __m128 v_avg_0 = _mm_set_ps(vblue[0], vgreen[0], vred[0], 0);
        __m128 v_avg_1 = _mm_set_ps(vblue[1], vgreen[1], vred[1], 0);

        h_avg_0 = _mm_mul_ps(h_avg_0, factor_v);
        h_avg_1 = _mm_mul_ps(h_avg_1, factor_v);
        v_avg_0 = _mm_mul_ps(v_avg_0, factor_v);
        v_avg_1 = _mm_mul_ps(v_avg_1, factor_v);

        h_avg_0 = _mm_add_ps(h_avg_0, rounding_v);
        h_avg_1 = _mm_add_ps(h_avg_1, rounding_v);
        v_avg_0 = _mm_add_ps(v_avg_0, rounding_v);
        v_avg_1 = _mm_add_ps(v_avg_1, rounding_v);

        __m128i h_avg_0i = _mm_cvttps_epi32(h_avg_0);
        __m128i h_avg_1i = _mm_cvttps_epi32(h_avg_1);

        __m128i v_avg_0i = _mm_cvttps_epi32(v_avg_0);
        __m128i v_avg_1i = _mm_cvttps_epi32(v_avg_1);

        h_avg_0i = _mm_sub_epi32(h_avg_1i, h_avg_0i);
        v_avg_0i = _mm_sub_epi32(v_avg_1i, v_avg_0i);

        __sse_use_diff[0] = (0 == _mm_movemask_epi8(_mm_cmplt_epi32(v_avg_0i, threshold_lower)));
        __sse_use_diff[0] &= (0 == _mm_movemask_epi8(_mm_cmpgt_epi32(v_avg_0i, threshold_upper)));

        __sse_use_diff[1] = (0 == _mm_movemask_epi8(_mm_cmplt_epi32(h_avg_0i, threshold_lower)));
        __sse_use_diff[1] &= (0 == _mm_movemask_epi8(_mm_cmpgt_epi32(h_avg_0i, threshold_upper)));
    }

    void ComputeLuminance(uint8_t* block,
        const Color& base,
        const int sub_block_id,
        const uint8_t* idx_to_num_tab,
        const __sse_data* data,
        const uint32_t expected_error)
    {
        uint8_t best_tbl_idx = 0;
        uint32_t best_error = 0x7FFFFFFF;
        uint8_t best_mod_idx[8][8]; // [table][texel]

        const __m128i base_blue = _mm_set1_epi32(base.channels.b);
        const __m128i base_green = _mm_set1_epi32(base.channels.g);
        const __m128i base_red = _mm_set1_epi32(base.channels.r);

        __m128i test_red, test_blue, test_green, tmp, tmp_blue, tmp_green, tmp_red;
        __m128i block_error, mask;

        // This will have the minimum errors for each 4 pixels.
        __m128i first_half_min;
        __m128i second_half_min;

        // This will have the matching table index combo for each 4 pixels.
        __m128i first_half_pattern;
        __m128i second_half_pattern;

        const __m128i first_blue_data_block = data->blue[2 * sub_block_id];
        const __m128i first_green_data_block = data->green[2 * sub_block_id];
        const __m128i first_red_data_block = data->red[2 * sub_block_id];

        const __m128i second_blue_data_block = data->blue[2 * sub_block_id + 1];
        const __m128i second_green_data_block = data->green[2 * sub_block_id + 1];
        const __m128i second_red_data_block = data->red[2 * sub_block_id + 1];

        uint32_t min;
        // Fail early to increase speed.
        long delta = INT32_MAX;
        uint32_t last_min = INT32_MAX;

        const uint8_t shuffle_mask[] = {
            0x1B, 0x4E, 0xB1, 0xE4
        }; // Important they are sorted ascending.

        for (unsigned int tbl_idx = 0; tbl_idx < 8; ++tbl_idx) {
            tmp = _mm_set_epi32(
                g_codeword_tables[tbl_idx][3], g_codeword_tables[tbl_idx][2],
                g_codeword_tables[tbl_idx][1], g_codeword_tables[tbl_idx][0]);

            test_blue = AddAndClamp(tmp, base_blue);
            test_green = AddAndClamp(tmp, base_green);
            test_red = AddAndClamp(tmp, base_red);

            first_half_min = __sse_max_int;
            second_half_min = __sse_max_int;

            first_half_pattern = __sse_zero;
            second_half_pattern = __sse_zero;

            for (uint8_t imm8 : shuffle_mask) {
                switch (imm8) {
                case 0x1B:
                    tmp_blue = _mm_shuffle_epi32(test_blue, 0x1B);
                    tmp_green = _mm_shuffle_epi32(test_green, 0x1B);
                    tmp_red = _mm_shuffle_epi32(test_red, 0x1B);
                    break;
                case 0x4E:
                    tmp_blue = _mm_shuffle_epi32(test_blue, 0x4E);
                    tmp_green = _mm_shuffle_epi32(test_green, 0x4E);
                    tmp_red = _mm_shuffle_epi32(test_red, 0x4E);
                    break;
                case 0xB1:
                    tmp_blue = _mm_shuffle_epi32(test_blue, 0xB1);
                    tmp_green = _mm_shuffle_epi32(test_green, 0xB1);
                    tmp_red = _mm_shuffle_epi32(test_red, 0xB1);
                    break;
                case 0xE4:
                    tmp_blue = _mm_shuffle_epi32(test_blue, 0xE4);
                    tmp_green = _mm_shuffle_epi32(test_green, 0xE4);
                    tmp_red = _mm_shuffle_epi32(test_red, 0xE4);
                    break;
                default:
                    tmp_blue = test_blue;
                    tmp_green = test_green;
                    tmp_red = test_red;
                }

                tmp = _mm_set1_epi32(imm8);

                block_error = AddChannelError(GetColorErrorSSE(tmp_blue, first_blue_data_block),
                    GetColorErrorSSE(tmp_green, first_green_data_block),
                    GetColorErrorSSE(tmp_red, first_red_data_block));

                // Save winning pattern.
                first_half_pattern = _mm_max_epi16(
                    first_half_pattern,
                    _mm_and_si128(tmp, _mm_cmpgt_epi32(first_half_min, block_error)));
                // Should use _mm_min_epi32(first_half_min, block_error); from SSE4
                // otherwise we have a small performance penalty.
                mask = _mm_cmplt_epi32(block_error, first_half_min);
                first_half_min = _mm_add_epi32(_mm_and_si128(mask, block_error),
                    _mm_andnot_si128(mask, first_half_min));

                // Compute second part of the block.
                block_error = AddChannelError(GetColorErrorSSE(tmp_blue, second_blue_data_block),
                    GetColorErrorSSE(tmp_green, second_green_data_block),
                    GetColorErrorSSE(tmp_red, second_red_data_block));

                // Save winning pattern.
                second_half_pattern = _mm_max_epi16(
                    second_half_pattern,
                    _mm_and_si128(tmp, _mm_cmpgt_epi32(second_half_min, block_error)));
                // Should use _mm_min_epi32(second_half_min, block_error); from SSE4
                // otherwise we have a small performance penalty.
                mask = _mm_cmplt_epi32(block_error, second_half_min);
                second_half_min = _mm_add_epi32(_mm_and_si128(mask, block_error),
                    _mm_andnot_si128(mask, second_half_min));
            }

            first_half_min = _mm_add_epi32(first_half_min, second_half_min);
            first_half_min = _mm_add_epi32(first_half_min, _mm_shuffle_epi32(first_half_min, 0x4E));
            first_half_min = _mm_add_epi32(first_half_min, _mm_shuffle_epi32(first_half_min, 0xB1));

            min = _mm_cvtsi128_si32(first_half_min);

            delta = min - last_min;
            last_min = min;

            if (min < best_error) {
                best_tbl_idx = tbl_idx;
                best_error = min;

                best_mod_idx[tbl_idx][0] = (_mm_cvtsi128_si32(first_half_pattern) >> (0)) & 3;
                best_mod_idx[tbl_idx][4] = (_mm_cvtsi128_si32(second_half_pattern) >> (0)) & 3;

                best_mod_idx[tbl_idx][1] = (_mm_cvtsi128_si32(_mm_shuffle_epi32(first_half_pattern, 0x1)) >> (2)) & 3;
                best_mod_idx[tbl_idx][5] = (_mm_cvtsi128_si32(_mm_shuffle_epi32(second_half_pattern, 0x1)) >> (2)) & 3;

                best_mod_idx[tbl_idx][2] = (_mm_cvtsi128_si32(_mm_shuffle_epi32(first_half_pattern, 0x2)) >> (4)) & 3;
                best_mod_idx[tbl_idx][6] = (_mm_cvtsi128_si32(_mm_shuffle_epi32(second_half_pattern, 0x2)) >> (4)) & 3;

                best_mod_idx[tbl_idx][3] = (_mm_cvtsi128_si32(_mm_shuffle_epi32(first_half_pattern, 0x3)) >> (6)) & 3;
                best_mod_idx[tbl_idx][7] = (_mm_cvtsi128_si32(_mm_shuffle_epi32(second_half_pattern, 0x3)) >> (6)) & 3;

                if (best_error == 0) {
                    break;
                }
            } else if (delta > 0 && expected_error < min) {
                // The error is growing and is well beyond expected threshold.
                break;
            }
        }

        WriteCodewordTable(block, sub_block_id, best_tbl_idx);

        uint32_t pix_data = 0;
        uint8_t mod_idx;
        uint8_t pix_idx;
        uint32_t lsb;
        uint32_t msb;
        int texel_num;

        for (unsigned int i = 0; i < 8; ++i) {
            mod_idx = best_mod_idx[best_tbl_idx][i];
            pix_idx = g_mod_to_pix[mod_idx];

            lsb = pix_idx & 0x1;
            msb = pix_idx >> 1;

            // Obtain the texel number as specified in the standard.
            texel_num = idx_to_num_tab[i];
            pix_data |= msb << (texel_num + 16);
            pix_data |= lsb << (texel_num);
        }

        WritePixelData(block, pix_data);
    }

    void CompressBlock(uint8_t* dst, __sse_data* data)
    {
        // First 3 values are for vertical 1, second 3 vertical 2, third 3 horizontal
        // 1, last 3
        // horizontal 2.
        float __sse_avg_colors[12] = {
            0,
        };
        bool use_differential[2] = { true, true };
        GetAvgColors(data, __sse_avg_colors, use_differential);
        Color sub_block_avg[4];

        // TODO(radu.velea): Remove floating point operations and use only int's +
        // normal rounding and shifts for reduced Quality.
        for (int i = 0, j = 1; i < 4; i += 2, j += 2) {
            if (use_differential[i / 2] == false) {
                sub_block_avg[i] = MakeColor444(&__sse_avg_colors[i * 3]);
                sub_block_avg[j] = MakeColor444(&__sse_avg_colors[j * 3]);
            } else {
                sub_block_avg[i] = MakeColor555(&__sse_avg_colors[i * 3]);
                sub_block_avg[j] = MakeColor555(&__sse_avg_colors[j * 3]);
            }
        }

        __m128i red_avg[2], green_avg[2], blue_avg[2];

        // TODO(radu.velea): Perfect accuracy, maybe skip floating variables.
        blue_avg[0] = _mm_set_epi32(static_cast<int>(__sse_avg_colors[3]),
            static_cast<int>(__sse_avg_colors[3]),
            static_cast<int>(__sse_avg_colors[0]),
            static_cast<int>(__sse_avg_colors[0]));

        green_avg[0] = _mm_set_epi32(static_cast<int>(__sse_avg_colors[4]),
            static_cast<int>(__sse_avg_colors[4]),
            static_cast<int>(__sse_avg_colors[1]),
            static_cast<int>(__sse_avg_colors[1]));

        red_avg[0] = _mm_set_epi32(static_cast<int>(__sse_avg_colors[5]),
            static_cast<int>(__sse_avg_colors[5]),
            static_cast<int>(__sse_avg_colors[2]),
            static_cast<int>(__sse_avg_colors[2]));

        uint32_t vertical_error[2];
        GetVerticalError(data, blue_avg, green_avg, red_avg, vertical_error);

        // TODO(radu.velea): Perfect accuracy, maybe skip floating variables.
        blue_avg[0] = _mm_set1_epi32(static_cast<int>(__sse_avg_colors[6]));
        blue_avg[1] = _mm_set1_epi32(static_cast<int>(__sse_avg_colors[9]));

        green_avg[0] = _mm_set1_epi32(static_cast<int>(__sse_avg_colors[7]));
        green_avg[1] = _mm_set1_epi32(static_cast<int>(__sse_avg_colors[10]));

        red_avg[0] = _mm_set1_epi32(static_cast<int>(__sse_avg_colors[8]));
        red_avg[1] = _mm_set1_epi32(static_cast<int>(__sse_avg_colors[11]));

        uint32_t horizontal_error[2];
        GetHorizontalError(data, blue_avg, green_avg, red_avg, horizontal_error);

        bool flip = (horizontal_error[0] + horizontal_error[1]) < (vertical_error[0] + vertical_error[1]);
        uint32_t* expected_errors = flip ? horizontal_error : vertical_error;

        // Clear destination buffer so that we can "or" in the results.
        memset(dst, 0, 8);

        WriteDiff(dst, use_differential[!!flip]);
        WriteFlip(dst, flip);

        uint8_t sub_block_off_0 = flip ? 2 : 0;
        uint8_t sub_block_off_1 = sub_block_off_0 + 1;

        if (use_differential[!!flip]) {
            WriteColors555(dst, sub_block_avg[sub_block_off_0],
                sub_block_avg[sub_block_off_1]);
        } else {
            WriteColors444(dst, sub_block_avg[sub_block_off_0],
                sub_block_avg[sub_block_off_1]);
        }

        if (!flip) {
            // Transpose vertical data into horizontal lines.
            __m128i tmp;
            for (int i = 0; i < 4; i += 2) {
                tmp = data->blue[i];
                data->blue[i] = _mm_add_epi32(
                    _mm_move_epi64(data->blue[i]),
                    _mm_shuffle_epi32(_mm_move_epi64(data->blue[i + 1]), 0x4E));
                data->blue[i + 1] = _mm_add_epi32(
                    _mm_move_epi64(_mm_shuffle_epi32(tmp, 0x4E)),
                    _mm_shuffle_epi32(
                        _mm_move_epi64(_mm_shuffle_epi32(data->blue[i + 1], 0x4E)),
                        0x4E));

                tmp = data->green[i];
                data->green[i] = _mm_add_epi32(
                    _mm_move_epi64(data->green[i]),
                    _mm_shuffle_epi32(_mm_move_epi64(data->green[i + 1]), 0x4E));
                data->green[i + 1] = _mm_add_epi32(
                    _mm_move_epi64(_mm_shuffle_epi32(tmp, 0x4E)),
                    _mm_shuffle_epi32(
                        _mm_move_epi64(_mm_shuffle_epi32(data->green[i + 1], 0x4E)),
                        0x4E));

                tmp = data->red[i];
                data->red[i] = _mm_add_epi32(
                    _mm_move_epi64(data->red[i]),
                    _mm_shuffle_epi32(_mm_move_epi64(data->red[i + 1]), 0x4E));
                data->red[i + 1] = _mm_add_epi32(
                    _mm_move_epi64(_mm_shuffle_epi32(tmp, 0x4E)),
                    _mm_shuffle_epi32(
                        _mm_move_epi64(_mm_shuffle_epi32(data->red[i + 1], 0x4E)), 0x4E));
            }

            tmp = data->blue[1];
            data->blue[1] = data->blue[2];
            data->blue[2] = tmp;

            tmp = data->green[1];
            data->green[1] = data->green[2];
            data->green[2] = tmp;

            tmp = data->red[1];
            data->red[1] = data->red[2];
            data->red[2] = tmp;
        }

        // Compute luminance for the first sub block.
        ComputeLuminance(dst, sub_block_avg[sub_block_off_0], 0,
            g_idx_to_num[sub_block_off_0], data,
            SetETC1MaxError(expected_errors[0]));
        // Compute luminance for the second sub block.
        ComputeLuminance(dst, sub_block_avg[sub_block_off_1], 1,
            g_idx_to_num[sub_block_off_1], data,
            SetETC1MaxError(expected_errors[1]));
    }

    static void ExtractBlock(uint8_t* dst, const uint8_t* src, int width)
    {
        for (int j = 0; j < 4; ++j) {
            memcpy(&dst[j * 4 * 4], src, 4 * 4);
            src += width * 4;
        }
    }

    inline bool TransposeBlock(uint8_t* block, __m128i* transposed)
    {
        // This function transforms an incommig block of RGBA or GBRA pixels into 4
        // registers, each containing the data corresponding for a single channel.
        // Ex: transposed[0] will have all the R values for a RGBA block,
        // transposed[1] will have G, etc.
        // The values are packed as 8 bit unsigned values in the SSE registers.

        // Before doing any work we check if the block is solid.
        __m128i tmp3, tmp2, tmp1, tmp0;
        __m128i test_solid = _mm_set1_epi32(*((uint32_t*)block));
        uint16_t mask = 0xFFFF;

        // a0,a1,a2,...a7, ...a15
        transposed[0] = _mm_loadu_si128((__m128i*)(block));
        // b0, b1,b2,...b7.... b15
        transposed[1] = _mm_loadu_si128((__m128i*)(block + 16));
        // c0, c1,c2,...c7....c15
        transposed[2] = _mm_loadu_si128((__m128i*)(block + 32));
        // d0,d1,d2,...d7....d15
        transposed[3] = _mm_loadu_si128((__m128i*)(block + 48));

        for (int i = 0; i < 4; i++) {
            mask &= _mm_movemask_epi8(_mm_cmpeq_epi8(transposed[i], test_solid));
        }

        if (mask == 0xFFFF) {
            // Block is solid, no need to do any more work.
            return false;
        }

        // a0,b0, a1,b1, a2,b2, a3,b3,....a7,b7
        tmp0 = _mm_unpacklo_epi8(transposed[0], transposed[1]);
        // c0,d0, c1,d1, c2,d2, c3,d3,... c7,d7
        tmp1 = _mm_unpacklo_epi8(transposed[2], transposed[3]);
        // a8,b8, a9,b9, a10,b10, a11,b11,...a15,b15
        tmp2 = _mm_unpackhi_epi8(transposed[0], transposed[1]);
        // c8,d8, c9,d9, c10,d10, c11,d11,...c15,d15
        tmp3 = _mm_unpackhi_epi8(transposed[2], transposed[3]);

        // a0,a8, b0,b8,  a1,a9, b1,b9, ....a3,a11, b3,b11
        transposed[0] = _mm_unpacklo_epi8(tmp0, tmp2);
        // a4,a12, b4,b12, a5,a13, b5,b13,....a7,a15,b7,b15
        transposed[1] = _mm_unpackhi_epi8(tmp0, tmp2);
        // c0,c8, d0,d8, c1,c9, d1,d9.....d3,d11
        transposed[2] = _mm_unpacklo_epi8(tmp1, tmp3);
        // c4,c12,d4,d12, c5,c13, d5,d13,....d7,d15
        transposed[3] = _mm_unpackhi_epi8(tmp1, tmp3);

        // a0,a8, b0,b8, c0,c8, d0,d8, a1,a9, b1,b9, c1,c9, d1,d9
        tmp0 = _mm_unpacklo_epi32(transposed[0], transposed[2]);
        // a2,a10, b2,b10, c2,c10, d2,d10, a3,a11, b3,b11, c3,c11, d3,d11
        tmp1 = _mm_unpackhi_epi32(transposed[0], transposed[2]);
        // a4,a12, b4,b12, c4,c12, d4,d12, a5,a13, b5,b13, c5,c13, d5,d13
        tmp2 = _mm_unpacklo_epi32(transposed[1], transposed[3]);
        // a6,a14, b6,b14, c6,c14, d6,d14, a7,a15, b7,b15, c7,c15, d7,d15
        tmp3 = _mm_unpackhi_epi32(transposed[1], transposed[3]);

        // a0,a4, a8,a12, b0,b4, b8,b12,  c0,c4, c8,c12, d0,d4, d8,d12
        transposed[0] = _mm_unpacklo_epi8(tmp0, tmp2);
        // a1,a5, a9,a13, b1,b5, b9,b13,  c1,c5, c9,c13, d1,d5, d9,d13
        transposed[1] = _mm_unpackhi_epi8(tmp0, tmp2);
        // a2,a6, a10,a14, b2,b6, b10,b14, c2,c6, c10,c14, d2,d6, d10,d14
        transposed[2] = _mm_unpacklo_epi8(tmp1, tmp3);
        // a3,a7, a11,a15, b3,b7, b11,b15, c3,c7, c11,c15, d3,d7, d11,d15
        transposed[3] = _mm_unpackhi_epi8(tmp1, tmp3);

        return true;
    }

    inline void UnpackBlock(__m128i* packed,
        __m128i* red,
        __m128i* green,
        __m128i* blue,
        __m128i* alpha)
    {
        const __m128i zero = _mm_set1_epi8(0);
        __m128i tmp_low, tmp_high;

        // Unpack red.
        tmp_low = _mm_unpacklo_epi8(packed[0], zero);
        tmp_high = _mm_unpackhi_epi8(packed[0], zero);

        red[0] = _mm_unpacklo_epi16(tmp_low, zero);
        red[1] = _mm_unpackhi_epi16(tmp_low, zero);

        red[2] = _mm_unpacklo_epi16(tmp_high, zero);
        red[3] = _mm_unpackhi_epi16(tmp_high, zero);

        // Unpack green.
        tmp_low = _mm_unpacklo_epi8(packed[1], zero);
        tmp_high = _mm_unpackhi_epi8(packed[1], zero);

        green[0] = _mm_unpacklo_epi16(tmp_low, zero);
        green[1] = _mm_unpackhi_epi16(tmp_low, zero);

        green[2] = _mm_unpacklo_epi16(tmp_high, zero);
        green[3] = _mm_unpackhi_epi16(tmp_high, zero);

        // Unpack blue.
        tmp_low = _mm_unpacklo_epi8(packed[2], zero);
        tmp_high = _mm_unpackhi_epi8(packed[2], zero);

        blue[0] = _mm_unpacklo_epi16(tmp_low, zero);
        blue[1] = _mm_unpackhi_epi16(tmp_low, zero);

        blue[2] = _mm_unpacklo_epi16(tmp_high, zero);
        blue[3] = _mm_unpackhi_epi16(tmp_high, zero);

        // Unpack alpha - unused for ETC1.
        tmp_low = _mm_unpacklo_epi8(packed[3], zero);
        tmp_high = _mm_unpackhi_epi8(packed[3], zero);

        alpha[0] = _mm_unpacklo_epi16(tmp_low, zero);
        alpha[1] = _mm_unpackhi_epi16(tmp_low, zero);

        alpha[2] = _mm_unpacklo_epi16(tmp_high, zero);
        alpha[3] = _mm_unpackhi_epi16(tmp_high, zero);
    }

    inline void CompressSolid(uint8_t* dst, uint8_t* block)
    {
        // Clear destination buffer so that we can "or" in the results.
        memset(dst, 0, 8);

        const float src_color_float[3] = { static_cast<float>(block[0]),
            static_cast<float>(block[1]),
            static_cast<float>(block[2]) };
        const Color base = MakeColor555(src_color_float);
        const __m128i base_v = _mm_set_epi32(0, base.channels.r, base.channels.g, base.channels.b);

        const __m128i constant = _mm_set_epi32(0, block[2], block[1], block[0]);
        __m128i lum;
        __m128i colors[4];
        static const __m128i rgb = _mm_set_epi32(0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);

        WriteDiff(dst, true);
        WriteFlip(dst, false);

        WriteColors555(dst, base, base);

        uint8_t best_tbl_idx = 0;
        uint8_t best_mod_idx = 0;
        uint32_t best_mod_err = INT32_MAX;

        for (unsigned int tbl_idx = 0; tbl_idx < 8; ++tbl_idx) {
            lum = _mm_set_epi32(
                g_codeword_tables[tbl_idx][3], g_codeword_tables[tbl_idx][2],
                g_codeword_tables[tbl_idx][1], g_codeword_tables[tbl_idx][0]);
            colors[0] = AddAndClamp(base_v, _mm_shuffle_epi32(lum, 0x0));
            colors[1] = AddAndClamp(base_v, _mm_shuffle_epi32(lum, 0x55));
            colors[2] = AddAndClamp(base_v, _mm_shuffle_epi32(lum, 0xAA));
            colors[3] = AddAndClamp(base_v, _mm_shuffle_epi32(lum, 0xFF));

            for (int i = 0; i < 4; i++) {
                uint32_t mod_err = SumSSE(GetColorErrorSSE(constant, _mm_and_si128(colors[i], rgb)));
                colors[i] = _mm_and_si128(colors[i], rgb);
                if (mod_err < best_mod_err) {
                    best_tbl_idx = tbl_idx;
                    best_mod_idx = i;
                    best_mod_err = mod_err;

                    if (mod_err == 0) {
                        break; // We cannot do any better than this.
                    }
                }
            }
        }

        WriteCodewordTable(dst, 0, best_tbl_idx);
        WriteCodewordTable(dst, 1, best_tbl_idx);

        uint8_t pix_idx = g_mod_to_pix[best_mod_idx];
        uint32_t lsb = pix_idx & 0x1;
        uint32_t msb = pix_idx >> 1;

        uint32_t pix_data = 0;
        for (unsigned int i = 0; i < 2; ++i) {
            for (unsigned int j = 0; j < 8; ++j) {
                // Obtain the texel number as specified in the standard.
                int texel_num = g_idx_to_num[i][j];
                pix_data |= msb << (texel_num + 16);
                pix_data |= lsb << (texel_num);
            }
        }

        WritePixelData(dst, pix_data);
    }

} // namespace

void TextureCompressorETC1SSE::Compress(const uint8_t* src,
    uint8_t* dst,
    int width,
    int height,
    Quality quality)
{
    DCHECK_GE(width, 4);
    DCHECK_EQ((width & 3), 0);
    DCHECK_GE(height, 4);
    DCHECK_EQ((height & 3), 0);

    ALIGNAS(16)
    uint8_t block[64];
    __m128i packed[4];
    __m128i red[4], green[4], blue[4], alpha[4];
    __sse_data data;

    for (int y = 0; y < height; y += 4, src += width * 4 * 4) {
        for (int x = 0; x < width; x += 4, dst += 8) {
            ExtractBlock(block, src + x * 4, width);
            if (TransposeBlock(block, packed) == false) {
                CompressSolid(dst, block);
            } else {
                UnpackBlock(packed, blue, green, red, alpha);

                data.block = block;
                data.packed = packed;
                data.red = red;
                data.blue = blue;
                data.green = green;

                CompressBlock(dst, &data);
            }
        }
    }
}

} // namespace cc
