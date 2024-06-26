// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// See the following specification for details on the ETC1 format:
// https://www.khronos.org/registry/gles/extensions/OES/OES_compressed_ETC1_RGB8_texture.txt

#include "cc/raster/texture_compressor_etc1.h"

#include <limits>
#include <string.h>

#include "base/logging.h"

// Defining the following macro will cause the error metric function to weigh
// each color channel differently depending on how the human eye can perceive
// them. This can give a slight improvement in image quality at the cost of a
// performance hit.
// #define USE_PERCEIVED_ERROR_METRIC

namespace cc {

namespace {

    // Constructs a color from a given base color and luminance value.
    inline Color MakeColor(const Color& base, int16_t lum)
    {
        int b = static_cast<int>(base.channels.b) + lum;
        int g = static_cast<int>(base.channels.g) + lum;
        int r = static_cast<int>(base.channels.r) + lum;
        Color color;
        color.channels.b = static_cast<uint8_t>(clamp(b, 0, 255));
        color.channels.g = static_cast<uint8_t>(clamp(g, 0, 255));
        color.channels.r = static_cast<uint8_t>(clamp(r, 0, 255));
        return color;
    }

    // Calculates the error metric for two colors. A small error signals that the
    // colors are similar to each other, a large error the signals the opposite.
    inline uint32_t GetColorError(const Color& u, const Color& v)
    {
#ifdef USE_PERCEIVED_ERROR_METRIC
        float delta_b = static_cast<float>(u.channels.b) - v.channels.b;
        float delta_g = static_cast<float>(u.channels.g) - v.channels.g;
        float delta_r = static_cast<float>(u.channels.r) - v.channels.r;
        return static_cast<uint32_t>(0.299f * delta_b * delta_b + 0.587f * delta_g * delta_g + 0.114f * delta_r * delta_r);
#else
        int delta_b = static_cast<int>(u.channels.b) - v.channels.b;
        int delta_g = static_cast<int>(u.channels.g) - v.channels.g;
        int delta_r = static_cast<int>(u.channels.r) - v.channels.r;
        return delta_b * delta_b + delta_g * delta_g + delta_r * delta_r;
#endif
    }

    void GetAverageColor(const Color* src, float* avg_color)
    {
        uint32_t sum_b = 0, sum_g = 0, sum_r = 0;

        for (unsigned int i = 0; i < 8; ++i) {
            sum_b += src[i].channels.b;
            sum_g += src[i].channels.g;
            sum_r += src[i].channels.r;
        }

        const float kInv8 = 1.0f / 8.0f;
        avg_color[0] = static_cast<float>(sum_b) * kInv8;
        avg_color[1] = static_cast<float>(sum_g) * kInv8;
        avg_color[2] = static_cast<float>(sum_r) * kInv8;
    }

    void ComputeLuminance(uint8_t* block,
        const Color* src,
        const Color& base,
        int sub_block_id,
        const uint8_t* idx_to_num_tab)
    {
        uint32_t best_tbl_err = std::numeric_limits<uint32_t>::max();
        uint8_t best_tbl_idx = 0;
        uint8_t best_mod_idx[8][8]; // [table][texel]

        // Try all codeword tables to find the one giving the best results for this
        // block.
        for (unsigned int tbl_idx = 0; tbl_idx < 8; ++tbl_idx) {
            // Pre-compute all the candidate colors; combinations of the base color and
            // all available luminance values.
            Color candidate_color[4]; // [modifier]
            for (unsigned int mod_idx = 0; mod_idx < 4; ++mod_idx) {
                int16_t lum = g_codeword_tables[tbl_idx][mod_idx];
                candidate_color[mod_idx] = MakeColor(base, lum);
            }

            uint32_t tbl_err = 0;

            for (unsigned int i = 0; i < 8; ++i) {
                // Try all modifiers in the current table to find which one gives the
                // smallest error.
                uint32_t best_mod_err = std::numeric_limits<uint32_t>::max();
                for (unsigned int mod_idx = 0; mod_idx < 4; ++mod_idx) {
                    const Color& color = candidate_color[mod_idx];

                    uint32_t mod_err = GetColorError(src[i], color);
                    if (mod_err < best_mod_err) {
                        best_mod_idx[tbl_idx][i] = mod_idx;
                        best_mod_err = mod_err;

                        if (mod_err == 0)
                            break; // We cannot do any better than this.
                    }
                }

                tbl_err += best_mod_err;
                if (tbl_err > best_tbl_err)
                    break; // We're already doing worse than the best table so skip.
            }

            if (tbl_err < best_tbl_err) {
                best_tbl_err = tbl_err;
                best_tbl_idx = tbl_idx;

                if (tbl_err == 0)
                    break; // We cannot do any better than this.
            }
        }

        WriteCodewordTable(block, sub_block_id, best_tbl_idx);

        uint32_t pix_data = 0;

        for (unsigned int i = 0; i < 8; ++i) {
            uint8_t mod_idx = best_mod_idx[best_tbl_idx][i];
            uint8_t pix_idx = g_mod_to_pix[mod_idx];

            uint32_t lsb = pix_idx & 0x1;
            uint32_t msb = pix_idx >> 1;

            // Obtain the texel number as specified in the standard.
            int texel_num = idx_to_num_tab[i];
            pix_data |= msb << (texel_num + 16);
            pix_data |= lsb << (texel_num);
        }

        WritePixelData(block, pix_data);
    }

    /**
 * Tries to compress the block under the assumption that it's a single color
 * block. If it's not the function will bail out without writing anything to
 * the destination buffer.
 */
    bool TryCompressSolidBlock(uint8_t* dst, const Color* src)
    {
        for (unsigned int i = 1; i < 16; ++i) {
            if (src[i].bits != src[0].bits)
                return false;
        }

        // Clear destination buffer so that we can "or" in the results.
        memset(dst, 0, 8);

        float src_color_float[3] = { static_cast<float>(src->channels.b),
            static_cast<float>(src->channels.g),
            static_cast<float>(src->channels.r) };
        Color base = MakeColor555(src_color_float);

        WriteDiff(dst, true);
        WriteFlip(dst, false);
        WriteColors555(dst, base, base);

        uint8_t best_tbl_idx = 0;
        uint8_t best_mod_idx = 0;
        uint32_t best_mod_err = std::numeric_limits<uint32_t>::max();

        // Try all codeword tables to find the one giving the best results for this
        // block.
        for (unsigned int tbl_idx = 0; tbl_idx < 8; ++tbl_idx) {
            // Try all modifiers in the current table to find which one gives the
            // smallest error.
            for (unsigned int mod_idx = 0; mod_idx < 4; ++mod_idx) {
                int16_t lum = g_codeword_tables[tbl_idx][mod_idx];
                const Color& color = MakeColor(base, lum);

                uint32_t mod_err = GetColorError(*src, color);
                if (mod_err < best_mod_err) {
                    best_tbl_idx = tbl_idx;
                    best_mod_idx = mod_idx;
                    best_mod_err = mod_err;

                    if (mod_err == 0)
                        break; // We cannot do any better than this.
                }
            }

            if (best_mod_err == 0)
                break;
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
        return true;
    }

    void CompressBlock(uint8_t* dst, const Color* ver_src, const Color* hor_src)
    {
        if (TryCompressSolidBlock(dst, ver_src))
            return;

        const Color* sub_block_src[4] = { ver_src, ver_src + 8, hor_src, hor_src + 8 };

        Color sub_block_avg[4];
        bool use_differential[2] = { true, true };

        // Compute the average color for each sub block and determine if differential
        // coding can be used.
        for (unsigned int i = 0, j = 1; i < 4; i += 2, j += 2) {
            float avg_color_0[3];
            GetAverageColor(sub_block_src[i], avg_color_0);
            Color avg_color_555_0 = MakeColor555(avg_color_0);

            float avg_color_1[3];
            GetAverageColor(sub_block_src[j], avg_color_1);
            Color avg_color_555_1 = MakeColor555(avg_color_1);

            for (unsigned int light_idx = 0; light_idx < 3; ++light_idx) {
                int u = avg_color_555_0.components[light_idx] >> 3;
                int v = avg_color_555_1.components[light_idx] >> 3;

                int component_diff = v - u;
                if (component_diff < -4 || component_diff > 3) {
                    use_differential[i / 2] = false;
                    sub_block_avg[i] = MakeColor444(avg_color_0);
                    sub_block_avg[j] = MakeColor444(avg_color_1);
                } else {
                    sub_block_avg[i] = avg_color_555_0;
                    sub_block_avg[j] = avg_color_555_1;
                }
            }
        }

        // Compute the error of each sub block before adjusting for luminance. These
        // error values are later used for determining if we should flip the sub
        // block or not.
        uint32_t sub_block_err[4] = { 0 };
        for (unsigned int i = 0; i < 4; ++i) {
            for (unsigned int j = 0; j < 8; ++j) {
                sub_block_err[i] += GetColorError(sub_block_avg[i], sub_block_src[i][j]);
            }
        }

        bool flip = sub_block_err[2] + sub_block_err[3] < sub_block_err[0] + sub_block_err[1];

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

        // Compute luminance for the first sub block.
        ComputeLuminance(dst, sub_block_src[sub_block_off_0],
            sub_block_avg[sub_block_off_0], 0,
            g_idx_to_num[sub_block_off_0]);
        // Compute luminance for the second sub block.
        ComputeLuminance(dst, sub_block_src[sub_block_off_1],
            sub_block_avg[sub_block_off_1], 1,
            g_idx_to_num[sub_block_off_1]);
    }

} // namespace

void TextureCompressorETC1::Compress(const uint8_t* src,
    uint8_t* dst,
    int width,
    int height,
    Quality quality)
{
    DCHECK_GE(width, 4);
    DCHECK_EQ((width & 3), 0);
    DCHECK_GE(height, 4);
    DCHECK_EQ((height & 3), 0);

    Color ver_blocks[16];
    Color hor_blocks[16];

    for (int y = 0; y < height; y += 4, src += width * 4 * 4) {
        for (int x = 0; x < width; x += 4, dst += 8) {
            const Color* row0 = reinterpret_cast<const Color*>(src + x * 4);
            const Color* row1 = row0 + width;
            const Color* row2 = row1 + width;
            const Color* row3 = row2 + width;

            memcpy(ver_blocks, row0, 8);
            memcpy(ver_blocks + 2, row1, 8);
            memcpy(ver_blocks + 4, row2, 8);
            memcpy(ver_blocks + 6, row3, 8);
            memcpy(ver_blocks + 8, row0 + 2, 8);
            memcpy(ver_blocks + 10, row1 + 2, 8);
            memcpy(ver_blocks + 12, row2 + 2, 8);
            memcpy(ver_blocks + 14, row3 + 2, 8);

            memcpy(hor_blocks, row0, 16);
            memcpy(hor_blocks + 4, row1, 16);
            memcpy(hor_blocks + 8, row2, 16);
            memcpy(hor_blocks + 12, row3, 16);

            CompressBlock(dst, ver_blocks, hor_blocks);
        }
    }
}

} // namespace cc
