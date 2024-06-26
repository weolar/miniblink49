// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This webpage shows layout of YV12 and other YUV formats
// http://www.fourcc.org/yuv.php
// The actual conversion is best described here
// http://en.wikipedia.org/wiki/YUV
// An article on optimizing YUV conversion using tables instead of multiplies
// http://lestourtereaux.free.fr/papers/data/yuvrgb.pdf
//
// YV12 is a full plane of Y and a half height, half width chroma planes
// YV16 is a full plane of Y and a full height, half width chroma planes
//
// ARGB pixel format is output, which on little endian is stored as BGRA.
// The alpha is set to 255, allowing the application to use RGBA or RGB32.

#include "media/base/yuv_convert.h"

#include "base/cpu.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/aligned_memory.h"
#include "base/memory/scoped_ptr.h"
#include "base/third_party/dynamic_annotations/dynamic_annotations.h"
#include "build/build_config.h"
#include "media/base/simd/convert_rgb_to_yuv.h"
#include "media/base/simd/convert_yuv_to_rgb.h"
#include "media/base/simd/filter_yuv.h"

#undef max
#undef min

#if defined(ARCH_CPU_X86_FAMILY)
#if defined(COMPILER_MSVC)
#include <intrin.h>
#else
#include <mmintrin.h>
#endif
#endif

// Assembly functions are declared without namespace.
extern "C" {
void EmptyRegisterState_MMX();
} // extern "C"

namespace media {

typedef void (*FilterYUVRowsProc)(uint8*,
    const uint8*,
    const uint8*,
    int,
    uint8);

typedef void (*ConvertRGBToYUVProc)(const uint8*,
    uint8*,
    uint8*,
    uint8*,
    int,
    int,
    int,
    int,
    int);

typedef void (*ConvertYUVToRGB32Proc)(const uint8*,
    const uint8*,
    const uint8*,
    uint8*,
    int,
    int,
    int,
    int,
    int,
    YUVType);

typedef void (*ConvertYUVAToARGBProc)(const uint8*,
    const uint8*,
    const uint8*,
    const uint8*,
    uint8*,
    int,
    int,
    int,
    int,
    int,
    int,
    YUVType);

typedef void (*ConvertYUVToRGB32RowProc)(const uint8*,
    const uint8*,
    const uint8*,
    uint8*,
    ptrdiff_t,
    const int16*);

typedef void (*ConvertYUVAToARGBRowProc)(const uint8*,
    const uint8*,
    const uint8*,
    const uint8*,
    uint8*,
    ptrdiff_t,
    const int16*);

typedef void (*ScaleYUVToRGB32RowProc)(const uint8*,
    const uint8*,
    const uint8*,
    uint8*,
    ptrdiff_t,
    ptrdiff_t,
    const int16*);

static FilterYUVRowsProc g_filter_yuv_rows_proc_ = NULL;
static ConvertYUVToRGB32RowProc g_convert_yuv_to_rgb32_row_proc_ = NULL;
static ScaleYUVToRGB32RowProc g_scale_yuv_to_rgb32_row_proc_ = NULL;
static ScaleYUVToRGB32RowProc g_linear_scale_yuv_to_rgb32_row_proc_ = NULL;
static ConvertRGBToYUVProc g_convert_rgb32_to_yuv_proc_ = NULL;
static ConvertRGBToYUVProc g_convert_rgb24_to_yuv_proc_ = NULL;
static ConvertYUVToRGB32Proc g_convert_yuv_to_rgb32_proc_ = NULL;
static ConvertYUVAToARGBProc g_convert_yuva_to_argb_proc_ = NULL;

static const int kYUVToRGBTableSize = 256 * 4 * 4 * sizeof(int16);

// base::AlignedMemory has a private operator new(), so wrap it in a struct so
// that we can put it in a LazyInstance::Leaky.
struct YUVToRGBTableWrapper {
    base::AlignedMemory<kYUVToRGBTableSize, 16> table;
};

typedef base::LazyInstance<YUVToRGBTableWrapper>::Leaky
    YUVToRGBTable;
static YUVToRGBTable g_table_rec601 = LAZY_INSTANCE_INITIALIZER;
static YUVToRGBTable g_table_jpeg = LAZY_INSTANCE_INITIALIZER;
static YUVToRGBTable g_table_rec709 = LAZY_INSTANCE_INITIALIZER;
static const int16* g_table_rec601_ptr = NULL;
static const int16* g_table_jpeg_ptr = NULL;
static const int16* g_table_rec709_ptr = NULL;

// Empty SIMD registers state after using them.
void EmptyRegisterStateStub() { }
#if defined(MEDIA_MMX_INTRINSICS_AVAILABLE)
void EmptyRegisterStateIntrinsic()
{
    _mm_empty();
}
#endif
typedef void (*EmptyRegisterStateProc)();
static EmptyRegisterStateProc g_empty_register_state_proc_ = NULL;

// Get the appropriate value to bitshift by for vertical indices.
int GetVerticalShift(YUVType type)
{
    switch (type) {
    case YV16:
        return 0;
    case YV12:
    case YV12J:
    case YV12HD:
        return 1;
    }
    NOTREACHED();
    return 0;
}

const int16* GetLookupTable(YUVType type)
{
    switch (type) {
    case YV12:
    case YV16:
        return g_table_rec601_ptr;
    case YV12J:
        return g_table_jpeg_ptr;
    case YV12HD:
        return g_table_rec709_ptr;
    }
    NOTREACHED();
    return NULL;
}

// Populates a pre-allocated lookup table from a YUV->RGB matrix.
const int16* PopulateYUVToRGBTable(const double matrix[3][3],
    bool full_range,
    int16* table)
{
    // We'll have 4 sub-tables that lie contiguous in memory, one for each of Y,
    // U, V and A.
    const int kNumTables = 4;
    // Each table has 256 rows (for all possible 8-bit values).
    const int kNumRows = 256;
    // Each row has 4 columns, for contributions to each of R, G, B and A.
    const int kNumColumns = 4;
    // Each element is a fixed-point (10.6) 16-bit signed value.
    const int kElementSize = sizeof(int16);

    // Sanity check that our constants here match the size of the statically
    // allocated tables.
    COMPILE_ASSERT(
        kNumTables * kNumRows * kNumColumns * kElementSize == kYUVToRGBTableSize,
        "YUV lookup table size doesn't match expectation.");

    // Y needs an offset of -16 for color ranges that ignore the lower 16 values,
    // U and V get -128 to put them in [-128, 127] from [0, 255].
    int offsets[3] = { (full_range ? 0 : -16), -128, -128 };

    for (int i = 0; i < kNumRows; ++i) {
        // Y, U, and V contributions to each of R, G, B and A.
        for (int j = 0; j < 3; ++j) {
#if defined(OS_ANDROID)
            // Android is RGBA.
            table[(j * kNumRows + i) * kNumColumns + 0] = matrix[j][0] * 64 * (i + offsets[j]) + 0.5;
            table[(j * kNumRows + i) * kNumColumns + 1] = matrix[j][1] * 64 * (i + offsets[j]) + 0.5;
            table[(j * kNumRows + i) * kNumColumns + 2] = matrix[j][2] * 64 * (i + offsets[j]) + 0.5;
#else
            // Other platforms are BGRA.
            table[(j * kNumRows + i) * kNumColumns + 0] = matrix[j][2] * 64 * (i + offsets[j]) + 0.5;
            table[(j * kNumRows + i) * kNumColumns + 1] = matrix[j][1] * 64 * (i + offsets[j]) + 0.5;
            table[(j * kNumRows + i) * kNumColumns + 2] = matrix[j][0] * 64 * (i + offsets[j]) + 0.5;
#endif
            // Alpha contributions from Y and V are always 0. U is set such that
            // all values result in a full '255' alpha value.
            table[(j * kNumRows + i) * kNumColumns + 3] = (j == 1) ? 256 * 64 - 1 : 0;
        }
        // And YUVA alpha is passed through as-is.
        for (int k = 0; k < kNumTables; ++k)
            table[((kNumTables - 1) * kNumRows + i) * kNumColumns + k] = i;
    }

    return table;
}

void InitializeCPUSpecificYUVConversions()
{
    DebugBreak();
    //     CHECK(!g_filter_yuv_rows_proc_);
    //     CHECK(!g_convert_yuv_to_rgb32_row_proc_);
    //     CHECK(!g_scale_yuv_to_rgb32_row_proc_);
    //     CHECK(!g_linear_scale_yuv_to_rgb32_row_proc_);
    //     CHECK(!g_convert_rgb32_to_yuv_proc_);
    //     CHECK(!g_convert_rgb24_to_yuv_proc_);
    //     CHECK(!g_convert_yuv_to_rgb32_proc_);
    //     CHECK(!g_convert_yuva_to_argb_proc_);
    //     CHECK(!g_empty_register_state_proc_);
    //
    //     g_filter_yuv_rows_proc_ = FilterYUVRows_C;
    //     g_convert_yuv_to_rgb32_row_proc_ = ConvertYUVToRGB32Row_C;
    //     g_scale_yuv_to_rgb32_row_proc_ = ScaleYUVToRGB32Row_C;
    //     g_linear_scale_yuv_to_rgb32_row_proc_ = LinearScaleYUVToRGB32Row_C;
    //     g_convert_rgb32_to_yuv_proc_ = ConvertRGB32ToYUV_C;
    //     g_convert_rgb24_to_yuv_proc_ = ConvertRGB24ToYUV_C;
    //     g_convert_yuv_to_rgb32_proc_ = ConvertYUVToRGB32_C;
    //     g_convert_yuva_to_argb_proc_ = ConvertYUVAToARGB_C;
    //     g_empty_register_state_proc_ = EmptyRegisterStateStub;
    //
    //     // Assembly code confuses MemorySanitizer. Also not available in iOS builds.
    // #if defined(ARCH_CPU_X86_FAMILY) && !defined(MEMORY_SANITIZER) && !defined(OS_IOS)
    //     g_convert_yuva_to_argb_proc_ = ConvertYUVAToARGB_MMX;
    //
    // #if defined(MEDIA_MMX_INTRINSICS_AVAILABLE)
    //     g_empty_register_state_proc_ = EmptyRegisterStateIntrinsic;
    // #else
    //     g_empty_register_state_proc_ = EmptyRegisterState_MMX;
    // #endif
    //
    //     g_convert_yuv_to_rgb32_row_proc_ = ConvertYUVToRGB32Row_SSE;
    //     g_convert_yuv_to_rgb32_proc_ = ConvertYUVToRGB32_SSE;
    //
    //     g_filter_yuv_rows_proc_ = FilterYUVRows_SSE2;
    //     g_convert_rgb32_to_yuv_proc_ = ConvertRGB32ToYUV_SSE2;
    //
    // #if defined(ARCH_CPU_X86_64)
    //     g_scale_yuv_to_rgb32_row_proc_ = ScaleYUVToRGB32Row_SSE2_X64;
    //
    //     // Technically this should be in the MMX section, but MSVC will optimize out
    //     // the export of LinearScaleYUVToRGB32Row_MMX, which is required by the unit
    //     // tests, if that decision can be made at compile time.  Since all X64 CPUs
    //     // have SSE2, we can hack around this by making the selection here.
    //     g_linear_scale_yuv_to_rgb32_row_proc_ = LinearScaleYUVToRGB32Row_MMX_X64;
    // #else
    //     g_scale_yuv_to_rgb32_row_proc_ = ScaleYUVToRGB32Row_SSE;
    //     g_linear_scale_yuv_to_rgb32_row_proc_ = LinearScaleYUVToRGB32Row_SSE;
    // #endif
    //
    //     base::CPU cpu;
    //     if (cpu.has_ssse3()) {
    //         g_convert_rgb24_to_yuv_proc_ = &ConvertRGB24ToYUV_SSSE3;
    //
    //         // TODO(hclam): Add ConvertRGB32ToYUV_SSSE3 when the cyan problem is solved.
    //         // See: crbug.com/100462
    //     }
    // #endif

    // Initialize YUV conversion lookup tables.

    // SD Rec601 YUV->RGB matrix, see http://www.fourcc.org/fccyvrgb.php
    const double kRec601ConvertMatrix[3][3] = {
        { 1.164, 1.164, 1.164 },
        { 0.0, -0.391, 2.018 },
        { 1.596, -0.813, 0.0 },
    };

    // JPEG table, values from above link.
    const double kJPEGConvertMatrix[3][3] = {
        { 1.0, 1.0, 1.0 },
        { 0.0, -0.34414, 1.772 },
        { 1.402, -0.71414, 0.0 },
    };

    // Rec709 "HD" color space, values from:
    // http://www.equasys.de/colorconversion.html
    const double kRec709ConvertMatrix[3][3] = {
        { 1.164, 1.164, 1.164 },
        { 0.0, -0.213, 2.112 },
        { 1.793, -0.533, 0.0 },
    };

    PopulateYUVToRGBTable(kRec601ConvertMatrix, false,
        g_table_rec601.Get().table.data_as<int16>());
    PopulateYUVToRGBTable(kJPEGConvertMatrix, true,
        g_table_jpeg.Get().table.data_as<int16>());
    PopulateYUVToRGBTable(kRec709ConvertMatrix, false,
        g_table_rec709.Get().table.data_as<int16>());
    g_table_rec601_ptr = g_table_rec601.Get().table.data_as<int16>();
    g_table_rec709_ptr = g_table_rec709.Get().table.data_as<int16>();
    g_table_jpeg_ptr = g_table_jpeg.Get().table.data_as<int16>();
}

// Empty SIMD registers state after using them.
void EmptyRegisterState() { g_empty_register_state_proc_(); }

// 16.16 fixed point arithmetic
const int kFractionBits = 16;
const int kFractionMax = 1 << kFractionBits;
const int kFractionMask = ((1 << kFractionBits) - 1);

// Scale a frame of YUV to 32 bit ARGB.
void ScaleYUVToRGB32(const uint8* y_buf,
    const uint8* u_buf,
    const uint8* v_buf,
    uint8* rgb_buf,
    int source_width,
    int source_height,
    int width,
    int height,
    int y_pitch,
    int uv_pitch,
    int rgb_pitch,
    YUVType yuv_type,
    Rotate view_rotate,
    ScaleFilter filter)
{
    // Handle zero sized sources and destinations.
    if ((yuv_type == YV12 && (source_width < 2 || source_height < 2)) || (yuv_type == YV16 && (source_width < 2 || source_height < 1)) || width == 0 || height == 0)
        return;

    const int16* lookup_table = GetLookupTable(yuv_type);

    // 4096 allows 3 buffers to fit in 12k.
    // Helps performance on CPU with 16K L1 cache.
    // Large enough for 3830x2160 and 30" displays which are 2560x1600.
    const int kFilterBufferSize = 4096;
    // Disable filtering if the screen is too big (to avoid buffer overflows).
    // This should never happen to regular users: they don't have monitors
    // wider than 4096 pixels.
    // TODO(fbarchard): Allow rotated videos to filter.
    if (source_width > kFilterBufferSize || view_rotate)
        filter = FILTER_NONE;

    unsigned int y_shift = GetVerticalShift(yuv_type);
    // Diagram showing origin and direction of source sampling.
    // ->0   4<-
    // 7       3
    //
    // 6       5
    // ->1   2<-
    // Rotations that start at right side of image.
    if ((view_rotate == ROTATE_180) || (view_rotate == ROTATE_270) || (view_rotate == MIRROR_ROTATE_0) || (view_rotate == MIRROR_ROTATE_90)) {
        y_buf += source_width - 1;
        u_buf += source_width / 2 - 1;
        v_buf += source_width / 2 - 1;
        source_width = -source_width;
    }
    // Rotations that start at bottom of image.
    if ((view_rotate == ROTATE_90) || (view_rotate == ROTATE_180) || (view_rotate == MIRROR_ROTATE_90) || (view_rotate == MIRROR_ROTATE_180)) {
        y_buf += (source_height - 1) * y_pitch;
        u_buf += ((source_height >> y_shift) - 1) * uv_pitch;
        v_buf += ((source_height >> y_shift) - 1) * uv_pitch;
        source_height = -source_height;
    }

    int source_dx = source_width * kFractionMax / width;

    if ((view_rotate == ROTATE_90) || (view_rotate == ROTATE_270)) {
        int tmp = height;
        height = width;
        width = tmp;
        tmp = source_height;
        source_height = source_width;
        source_width = tmp;
        int source_dy = source_height * kFractionMax / height;
        source_dx = ((source_dy >> kFractionBits) * y_pitch) << kFractionBits;
        if (view_rotate == ROTATE_90) {
            y_pitch = -1;
            uv_pitch = -1;
            source_height = -source_height;
        } else {
            y_pitch = 1;
            uv_pitch = 1;
        }
    }

    // Need padding because FilterRows() will write 1 to 16 extra pixels
    // after the end for SSE2 version.
    uint8 yuvbuf[16 + kFilterBufferSize * 3 + 16];
    uint8* ybuf = reinterpret_cast<uint8*>(reinterpret_cast<uintptr_t>(yuvbuf + 15) & ~15);
    uint8* ubuf = ybuf + kFilterBufferSize;
    uint8* vbuf = ubuf + kFilterBufferSize;

    // TODO(fbarchard): Fixed point math is off by 1 on negatives.

    // We take a y-coordinate in [0,1] space in the source image space, and
    // transform to a y-coordinate in [0,1] space in the destination image space.
    // Note that the coordinate endpoints lie on pixel boundaries, not on pixel
    // centers: e.g. a two-pixel-high image will have pixel centers at 0.25 and
    // 0.75.  The formula is as follows (in fixed-point arithmetic):
    //   y_dst = dst_height * ((y_src + 0.5) / src_height)
    //   dst_pixel = clamp([0, dst_height - 1], floor(y_dst - 0.5))
    // Implement this here as an accumulator + delta, to avoid expensive math
    // in the loop.
    int source_y_subpixel_accum = ((kFractionMax / 2) * source_height) / height - (kFractionMax / 2);
    int source_y_subpixel_delta = ((1 << kFractionBits) * source_height) / height;

    // TODO(fbarchard): Split this into separate function for better efficiency.
    for (int y = 0; y < height; ++y) {
        uint8* dest_pixel = rgb_buf + y * rgb_pitch;
        int source_y_subpixel = source_y_subpixel_accum;
        source_y_subpixel_accum += source_y_subpixel_delta;
        if (source_y_subpixel < 0)
            source_y_subpixel = 0;
        else if (source_y_subpixel > ((source_height - 1) << kFractionBits))
            source_y_subpixel = (source_height - 1) << kFractionBits;

        const uint8* y_ptr = NULL;
        const uint8* u_ptr = NULL;
        const uint8* v_ptr = NULL;
        // Apply vertical filtering if necessary.
        // TODO(fbarchard): Remove memcpy when not necessary.
        if (filter & media::FILTER_BILINEAR_V) {
            int source_y = source_y_subpixel >> kFractionBits;
            y_ptr = y_buf + source_y * y_pitch;
            u_ptr = u_buf + (source_y >> y_shift) * uv_pitch;
            v_ptr = v_buf + (source_y >> y_shift) * uv_pitch;

            // Vertical scaler uses 16.8 fixed point.
            uint8 source_y_fraction = (source_y_subpixel & kFractionMask) >> 8;
            if (source_y_fraction != 0) {
                g_filter_yuv_rows_proc_(
                    ybuf, y_ptr, y_ptr + y_pitch, source_width, source_y_fraction);
            } else {
                memcpy(ybuf, y_ptr, source_width);
            }
            y_ptr = ybuf;
            ybuf[source_width] = ybuf[source_width - 1];

            int uv_source_width = (source_width + 1) / 2;
            uint8 source_uv_fraction;

            // For formats with half-height UV planes, each even-numbered pixel row
            // should not interpolate, since the next row to interpolate from should
            // be a duplicate of the current row.
            if (y_shift && (source_y & 0x1) == 0)
                source_uv_fraction = 0;
            else
                source_uv_fraction = source_y_fraction;

            if (source_uv_fraction != 0) {
                g_filter_yuv_rows_proc_(
                    ubuf, u_ptr, u_ptr + uv_pitch, uv_source_width, source_uv_fraction);
                g_filter_yuv_rows_proc_(
                    vbuf, v_ptr, v_ptr + uv_pitch, uv_source_width, source_uv_fraction);
            } else {
                memcpy(ubuf, u_ptr, uv_source_width);
                memcpy(vbuf, v_ptr, uv_source_width);
            }
            u_ptr = ubuf;
            v_ptr = vbuf;
            ubuf[uv_source_width] = ubuf[uv_source_width - 1];
            vbuf[uv_source_width] = vbuf[uv_source_width - 1];
        } else {
            // Offset by 1/2 pixel for center sampling.
            int source_y = (source_y_subpixel + (kFractionMax / 2)) >> kFractionBits;
            y_ptr = y_buf + source_y * y_pitch;
            u_ptr = u_buf + (source_y >> y_shift) * uv_pitch;
            v_ptr = v_buf + (source_y >> y_shift) * uv_pitch;
        }
        if (source_dx == kFractionMax) { // Not scaled
            g_convert_yuv_to_rgb32_row_proc_(y_ptr, u_ptr, v_ptr, dest_pixel, width,
                lookup_table);
        } else {
            if (filter & FILTER_BILINEAR_H) {
                g_linear_scale_yuv_to_rgb32_row_proc_(y_ptr, u_ptr, v_ptr, dest_pixel,
                    width, source_dx,
                    lookup_table);
            } else {
                g_scale_yuv_to_rgb32_row_proc_(y_ptr, u_ptr, v_ptr, dest_pixel, width,
                    source_dx, lookup_table);
            }
        }
    }

    g_empty_register_state_proc_();
}

// Scale a frame of YV12 to 32 bit ARGB for a specific rectangle.
void ScaleYUVToRGB32WithRect(const uint8* y_buf,
    const uint8* u_buf,
    const uint8* v_buf,
    uint8* rgb_buf,
    int source_width,
    int source_height,
    int dest_width,
    int dest_height,
    int dest_rect_left,
    int dest_rect_top,
    int dest_rect_right,
    int dest_rect_bottom,
    int y_pitch,
    int uv_pitch,
    int rgb_pitch)
{
    // This routine doesn't currently support up-scaling.
    CHECK_LE(dest_width, source_width);
    CHECK_LE(dest_height, source_height);

    // Sanity-check the destination rectangle.
    DCHECK(dest_rect_left >= 0 && dest_rect_right <= dest_width);
    DCHECK(dest_rect_top >= 0 && dest_rect_bottom <= dest_height);
    DCHECK(dest_rect_right > dest_rect_left);
    DCHECK(dest_rect_bottom > dest_rect_top);

    const int16* lookup_table = GetLookupTable(YV12);

    // Fixed-point value of vertical and horizontal scale down factor.
    // Values are in the format 16.16.
    int y_step = kFractionMax * source_height / dest_height;
    int x_step = kFractionMax * source_width / dest_width;

    // Determine the coordinates of the rectangle in 16.16 coords.
    // NB: Our origin is the *center* of the top/left pixel, NOT its top/left.
    // If we're down-scaling by more than a factor of two, we start with a 50%
    // fraction to avoid degenerating to point-sampling - we should really just
    // fix the fraction at 50% for all pixels in that case.
    int source_left = dest_rect_left * x_step;
    int source_right = (dest_rect_right - 1) * x_step;
    if (x_step < kFractionMax * 2) {
        source_left += ((x_step - kFractionMax) / 2);
        source_right += ((x_step - kFractionMax) / 2);
    } else {
        source_left += kFractionMax / 2;
        source_right += kFractionMax / 2;
    }
    int source_top = dest_rect_top * y_step;
    if (y_step < kFractionMax * 2) {
        source_top += ((y_step - kFractionMax) / 2);
    } else {
        source_top += kFractionMax / 2;
    }

    // Determine the parts of the Y, U and V buffers to interpolate.
    int source_y_left = source_left >> kFractionBits;
    int source_y_right = std::min((source_right >> kFractionBits) + 2, source_width + 1);

    int source_uv_left = source_y_left / 2;
    int source_uv_right = std::min((source_right >> (kFractionBits + 1)) + 2,
        (source_width + 1) / 2);

    int source_y_width = source_y_right - source_y_left;
    int source_uv_width = source_uv_right - source_uv_left;

    // Determine number of pixels in each output row.
    int dest_rect_width = dest_rect_right - dest_rect_left;

    // Intermediate buffer for vertical interpolation.
    // 4096 bytes allows 3 buffers to fit in 12k, which fits in a 16K L1 cache,
    // and is bigger than most users will generally need.
    // The buffer is 16-byte aligned and padded with 16 extra bytes; some of the
    // FilterYUVRowsProcs have alignment requirements, and the SSE version can
    // write up to 16 bytes past the end of the buffer.
    const int kFilterBufferSize = 4096;
    const bool kAvoidUsingOptimizedFilter = source_width > kFilterBufferSize;
    uint8 yuv_temp[16 + kFilterBufferSize * 3 + 16];
    // memset() yuv_temp to 0 to avoid bogus warnings when running on Valgrind.
    //     if (RunningOnValgrind())
    //         memset(yuv_temp, 0, sizeof(yuv_temp));
    DebugBreak();
    uint8* y_temp = reinterpret_cast<uint8*>(
        reinterpret_cast<uintptr_t>(yuv_temp + 15) & ~15);
    uint8* u_temp = y_temp + kFilterBufferSize;
    uint8* v_temp = u_temp + kFilterBufferSize;

    // Move to the top-left pixel of output.
    rgb_buf += dest_rect_top * rgb_pitch;
    rgb_buf += dest_rect_left * 4;

    // For each destination row perform interpolation and color space
    // conversion to produce the output.
    for (int row = dest_rect_top; row < dest_rect_bottom; ++row) {
        // Round the fixed-point y position to get the current row.
        int source_row = source_top >> kFractionBits;
        int source_uv_row = source_row / 2;
        DCHECK(source_row < source_height);

        // Locate the first row for each plane for interpolation.
        const uint8* y0_ptr = y_buf + y_pitch * source_row + source_y_left;
        const uint8* u0_ptr = u_buf + uv_pitch * source_uv_row + source_uv_left;
        const uint8* v0_ptr = v_buf + uv_pitch * source_uv_row + source_uv_left;
        const uint8* y1_ptr = NULL;
        const uint8* u1_ptr = NULL;
        const uint8* v1_ptr = NULL;

        // Locate the second row for interpolation, being careful not to overrun.
        if (source_row + 1 >= source_height) {
            y1_ptr = y0_ptr;
        } else {
            y1_ptr = y0_ptr + y_pitch;
        }
        if (source_uv_row + 1 >= (source_height + 1) / 2) {
            u1_ptr = u0_ptr;
            v1_ptr = v0_ptr;
        } else {
            u1_ptr = u0_ptr + uv_pitch;
            v1_ptr = v0_ptr + uv_pitch;
        }

        if (!kAvoidUsingOptimizedFilter) {
            // Vertical scaler uses 16.8 fixed point.
            uint8 fraction = (source_top & kFractionMask) >> 8;
            g_filter_yuv_rows_proc_(
                y_temp + source_y_left, y0_ptr, y1_ptr, source_y_width, fraction);
            g_filter_yuv_rows_proc_(
                u_temp + source_uv_left, u0_ptr, u1_ptr, source_uv_width, fraction);
            g_filter_yuv_rows_proc_(
                v_temp + source_uv_left, v0_ptr, v1_ptr, source_uv_width, fraction);

            // Perform horizontal interpolation and color space conversion.
            // TODO(hclam): Use the MMX version after more testing.
            LinearScaleYUVToRGB32RowWithRange_C(y_temp, u_temp, v_temp, rgb_buf,
                dest_rect_width, source_left, x_step,
                lookup_table);
        } else {
            // If the frame is too large then we linear scale a single row.
            LinearScaleYUVToRGB32RowWithRange_C(y0_ptr, u0_ptr, v0_ptr, rgb_buf,
                dest_rect_width, source_left, x_step,
                lookup_table);
        }

        // Advance vertically in the source and destination image.
        source_top += y_step;
        rgb_buf += rgb_pitch;
    }

    g_empty_register_state_proc_();
}

void ConvertRGB32ToYUV(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride)
{
    g_convert_rgb32_to_yuv_proc_(rgbframe,
        yplane,
        uplane,
        vplane,
        width,
        height,
        rgbstride,
        ystride,
        uvstride);
}

void ConvertRGB24ToYUV(const uint8* rgbframe,
    uint8* yplane,
    uint8* uplane,
    uint8* vplane,
    int width,
    int height,
    int rgbstride,
    int ystride,
    int uvstride)
{
    g_convert_rgb24_to_yuv_proc_(rgbframe,
        yplane,
        uplane,
        vplane,
        width,
        height,
        rgbstride,
        ystride,
        uvstride);
}

void ConvertYUVToRGB32(const uint8* yplane,
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
    g_convert_yuv_to_rgb32_proc_(yplane,
        uplane,
        vplane,
        rgbframe,
        width,
        height,
        ystride,
        uvstride,
        rgbstride,
        yuv_type);
}

void ConvertYUVAToARGB(const uint8* yplane,
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
    g_convert_yuva_to_argb_proc_(yplane,
        uplane,
        vplane,
        aplane,
        rgbframe,
        width,
        height,
        ystride,
        uvstride,
        astride,
        rgbstride,
        yuv_type);
}

} // namespace media
