// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RASTER_TEXTURE_COMPRESSOR_ETC1_H_
#define CC_RASTER_TEXTURE_COMPRESSOR_ETC1_H_

#include "cc/raster/texture_compressor.h"

#include "base/compiler_specific.h"
#include "base/logging.h"

namespace cc {

template <typename T>
inline T clamp(T val, T min, T max)
{
    return val < min ? min : (val > max ? max : val);
}

inline uint8_t round_to_5_bits(float val)
{
    return clamp<uint8_t>(val * 31.0f / 255.0f + 0.5f, 0, 31);
}

inline uint8_t round_to_4_bits(float val)
{
    return clamp<uint8_t>(val * 15.0f / 255.0f + 0.5f, 0, 15);
}

union Color {
    struct BgraColorType {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    } channels;
    uint8_t components[4];
    uint32_t bits;
};

// Codeword tables.
// See: Table 3.17.2
ALIGNAS(16)
static const int16_t g_codeword_tables[8][4] = {
    { -8, -2, 2, 8 },
    { -17, -5, 5, 17 },
    { -29, -9, 9, 29 },
    { -42, -13, 13, 42 },
    { -60, -18, 18, 60 },
    { -80, -24, 24, 80 },
    { -106, -33, 33, 106 },
    { -183, -47, 47, 183 }
};

// Maps modifier indices to pixel index values.
// See: Table 3.17.3
static const uint8_t g_mod_to_pix[4] = { 3, 2, 0, 1 };

// The ETC1 specification index texels as follows:
// [a][e][i][m]     [ 0][ 4][ 8][12]
// [b][f][j][n] <-> [ 1][ 5][ 9][13]
// [c][g][k][o]     [ 2][ 6][10][14]
// [d][h][l][p]     [ 3][ 7][11][15]

// [ 0][ 1][ 2][ 3]     [ 0][ 1][ 4][ 5]
// [ 4][ 5][ 6][ 7] <-> [ 8][ 9][12][13]
// [ 8][ 9][10][11]     [ 2][ 3][ 6][ 7]
// [12][13][14][15]     [10][11][14][15]

// However, when extracting sub blocks from BGRA data the natural array
// indexing order ends up different:
// vertical0: [a][e][b][f]  horizontal0: [a][e][i][m]
//            [c][g][d][h]               [b][f][j][n]
// vertical1: [i][m][j][n]  horizontal1: [c][g][k][o]
//            [k][o][l][p]               [d][h][l][p]

// In order to translate from the natural array indices in a sub block to the
// indices (number) used by specification and hardware we use this table.
static const uint8_t g_idx_to_num[4][8] = {
    { 0, 4, 1, 5, 2, 6, 3, 7 }, // Vertical block 0.
    { 8, 12, 9, 13, 10, 14, 11, 15 }, // Vertical block 1.
    { 0, 4, 8, 12, 1, 5, 9, 13 }, // Horizontal block 0.
    { 2, 6, 10, 14, 3, 7, 11, 15 } // Horizontal block 1.
};

inline void WriteColors444(uint8_t* block,
    const Color& color0,
    const Color& color1)
{
    // Write output color for BGRA textures.
    block[0] = (color0.channels.r & 0xf0) | (color1.channels.r >> 4);
    block[1] = (color0.channels.g & 0xf0) | (color1.channels.g >> 4);
    block[2] = (color0.channels.b & 0xf0) | (color1.channels.b >> 4);
}

inline void WriteColors555(uint8_t* block,
    const Color& color0,
    const Color& color1)
{
    // Table for conversion to 3-bit two complement format.
    static const uint8_t two_compl_trans_table[8] = {
        4, // -4 (100b)
        5, // -3 (101b)
        6, // -2 (110b)
        7, // -1 (111b)
        0, //  0 (000b)
        1, //  1 (001b)
        2, //  2 (010b)
        3, //  3 (011b)
    };

    int16_t delta_r = static_cast<int16_t>(color1.channels.r >> 3) - (color0.channels.r >> 3);
    int16_t delta_g = static_cast<int16_t>(color1.channels.g >> 3) - (color0.channels.g >> 3);
    int16_t delta_b = static_cast<int16_t>(color1.channels.b >> 3) - (color0.channels.b >> 3);
    DCHECK_GE(delta_r, -4);
    DCHECK_LE(delta_r, 3);
    DCHECK_GE(delta_g, -4);
    DCHECK_LE(delta_g, 3);
    DCHECK_GE(delta_b, -4);
    DCHECK_LE(delta_b, 3);

    // Write output color for BGRA textures.
    block[0] = (color0.channels.r & 0xf8) | two_compl_trans_table[delta_r + 4];
    block[1] = (color0.channels.g & 0xf8) | two_compl_trans_table[delta_g + 4];
    block[2] = (color0.channels.b & 0xf8) | two_compl_trans_table[delta_b + 4];
}

inline void WriteCodewordTable(uint8_t* block,
    uint8_t sub_block_id,
    uint8_t table)
{
    DCHECK_LT(sub_block_id, 2);
    DCHECK_LT(table, 8);

    uint8_t shift = (2 + (3 - sub_block_id * 3));
    block[3] &= ~(0x07 << shift);
    block[3] |= table << shift;
}

inline void WritePixelData(uint8_t* block, uint32_t pixel_data)
{
    block[4] |= pixel_data >> 24;
    block[5] |= (pixel_data >> 16) & 0xff;
    block[6] |= (pixel_data >> 8) & 0xff;
    block[7] |= pixel_data & 0xff;
}

inline void WriteFlip(uint8_t* block, bool flip)
{
    block[3] &= ~0x01;
    block[3] |= static_cast<uint8_t>(flip);
}

inline void WriteDiff(uint8_t* block, bool diff)
{
    block[3] &= ~0x02;
    block[3] |= static_cast<uint8_t>(diff) << 1;
}

// Compress and rounds BGR888 into BGR444. The resulting BGR444 color is
// expanded to BGR888 as it would be in hardware after decompression. The
// actual 444-bit data is available in the four most significant bits of each
// channel.
inline Color MakeColor444(const float* bgr)
{
    uint8_t b4 = round_to_4_bits(bgr[0]);
    uint8_t g4 = round_to_4_bits(bgr[1]);
    uint8_t r4 = round_to_4_bits(bgr[2]);
    Color bgr444;
    bgr444.channels.b = (b4 << 4) | b4;
    bgr444.channels.g = (g4 << 4) | g4;
    bgr444.channels.r = (r4 << 4) | r4;
    // Added to distinguish between expanded 555 and 444 colors.
    bgr444.channels.a = 0x44;
    return bgr444;
}

// Compress and rounds BGR888 into BGR555. The resulting BGR555 color is
// expanded to BGR888 as it would be in hardware after decompression. The
// actual 555-bit data is available in the five most significant bits of each
// channel.
inline Color MakeColor555(const float* bgr)
{
    uint8_t b5 = round_to_5_bits(bgr[0]);
    uint8_t g5 = round_to_5_bits(bgr[1]);
    uint8_t r5 = round_to_5_bits(bgr[2]);
    Color bgr555;
    bgr555.channels.b = (b5 << 3) | (b5 >> 2);
    bgr555.channels.g = (g5 << 3) | (g5 >> 2);
    bgr555.channels.r = (r5 << 3) | (r5 >> 2);
    // Added to distinguish between expanded 555 and 444 colors.
    bgr555.channels.a = 0x55;
    return bgr555;
}

class CC_EXPORT TextureCompressorETC1 : public TextureCompressor {
public:
    TextureCompressorETC1() { }

    // Compress a texture using ETC1. Note that the |quality| parameter is
    // ignored. The current implementation does not support different quality
    // settings.
    void Compress(const uint8_t* src,
        uint8_t* dst,
        int width,
        int height,
        Quality quality) override;

private:
    DISALLOW_COPY_AND_ASSIGN(TextureCompressorETC1);
};

} // namespace cc

#endif // CC_RASTER_TEXTURE_COMPRESSOR_ETC1_H_
