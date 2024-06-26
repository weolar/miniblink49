// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/buffer_format_util.h"

#include "base/logging.h"
#include "base/macros.h"
#include "base/numerics/safe_math.h"

namespace gfx {
namespace {

    const BufferFormat kBufferFormats[] = { BufferFormat::ATC,
        BufferFormat::ATCIA,
        BufferFormat::DXT1,
        BufferFormat::DXT5,
        BufferFormat::ETC1,
        BufferFormat::R_8,
        BufferFormat::RGBA_4444,
        BufferFormat::RGBX_8888,
        BufferFormat::RGBA_8888,
        BufferFormat::BGRX_8888,
        BufferFormat::BGRA_8888,
        BufferFormat::UYVY_422,
        BufferFormat::YUV_420_BIPLANAR,
        BufferFormat::YUV_420 };

    static_assert(arraysize(kBufferFormats) == (static_cast<int>(BufferFormat::LAST) + 1),
        "BufferFormat::LAST must be last value of kBufferFormats");

    bool RowSizeForBufferFormatChecked(
        size_t width, BufferFormat format, int plane, size_t* size_in_bytes)
    {
        base::CheckedNumeric<size_t> checked_size = width;
        switch (format) {
        case BufferFormat::ATCIA:
        case BufferFormat::DXT5:
            DCHECK_EQ(0, plane);
            *size_in_bytes = width;
            return true;
        case BufferFormat::ATC:
        case BufferFormat::DXT1:
        case BufferFormat::ETC1:
            DCHECK_EQ(0, plane);
            DCHECK_EQ(0u, width % 2);
            *size_in_bytes = width / 2;
            return true;
        case BufferFormat::R_8:
            checked_size += 3;
            if (!checked_size.IsValid())
                return false;
            *size_in_bytes = checked_size.ValueOrDie() & ~0x3;
            return true;
        case BufferFormat::RGBA_4444:
        case BufferFormat::UYVY_422:
            checked_size *= 2;
            if (!checked_size.IsValid())
                return false;
            *size_in_bytes = checked_size.ValueOrDie();
            return true;
        case BufferFormat::BGRX_8888:
        case BufferFormat::RGBX_8888:
        case BufferFormat::RGBA_8888:
        case BufferFormat::BGRA_8888:
            checked_size *= 4;
            if (!checked_size.IsValid())
                return false;
            *size_in_bytes = checked_size.ValueOrDie();
            return true;
        case BufferFormat::YUV_420:
            DCHECK_EQ(0u, width % 2);
            *size_in_bytes = width / SubsamplingFactorForBufferFormat(format, plane);
            return true;
        case BufferFormat::YUV_420_BIPLANAR:
            DCHECK_EQ(width % 2, 0u);
            *size_in_bytes = width;
            return true;
        }
        NOTREACHED();
        return false;
    }

} // namespace

std::vector<BufferFormat> GetBufferFormatsForTesting()
{
    return std::vector<BufferFormat>(kBufferFormats,
        kBufferFormats + arraysize(kBufferFormats));
}

size_t NumberOfPlanesForBufferFormat(BufferFormat format)
{
    switch (format) {
    case BufferFormat::ATC:
    case BufferFormat::ATCIA:
    case BufferFormat::DXT1:
    case BufferFormat::DXT5:
    case BufferFormat::ETC1:
    case BufferFormat::R_8:
    case BufferFormat::RGBA_4444:
    case BufferFormat::RGBX_8888:
    case BufferFormat::RGBA_8888:
    case BufferFormat::BGRX_8888:
    case BufferFormat::BGRA_8888:
    case BufferFormat::UYVY_422:
        return 1;
    case BufferFormat::YUV_420_BIPLANAR:
        return 2;
    case BufferFormat::YUV_420:
        return 3;
    }
    NOTREACHED();
    return 0;
}

size_t SubsamplingFactorForBufferFormat(BufferFormat format, int plane)
{
    switch (format) {
    case BufferFormat::ATC:
    case BufferFormat::ATCIA:
    case BufferFormat::DXT1:
    case BufferFormat::DXT5:
    case BufferFormat::ETC1:
    case BufferFormat::R_8:
    case BufferFormat::RGBA_4444:
    case BufferFormat::RGBX_8888:
    case BufferFormat::RGBA_8888:
    case BufferFormat::BGRX_8888:
    case BufferFormat::BGRA_8888:
    case BufferFormat::UYVY_422:
        return 1;
    case BufferFormat::YUV_420: {
        static size_t factor[] = { 1, 2, 2 };
        DCHECK_LT(static_cast<size_t>(plane), arraysize(factor));
        return factor[plane];
    }
    case BufferFormat::YUV_420_BIPLANAR: {
        static size_t factor[] = { 1, 2 };
        DCHECK_LT(static_cast<size_t>(plane), arraysize(factor));
        return factor[plane];
    }
    }
    NOTREACHED();
    return 0;
}

size_t RowSizeForBufferFormat(size_t width, BufferFormat format, int plane)
{
    size_t row_size = 0;
    bool valid = RowSizeForBufferFormatChecked(width, format, plane, &row_size);
    DCHECK(valid);
    return row_size;
}

size_t BufferSizeForBufferFormat(const Size& size, BufferFormat format)
{
    size_t buffer_size = 0;
    bool valid = BufferSizeForBufferFormatChecked(size, format, &buffer_size);
    DCHECK(valid);
    return buffer_size;
}

bool BufferSizeForBufferFormatChecked(const Size& size,
    BufferFormat format,
    size_t* size_in_bytes)
{
    base::CheckedNumeric<size_t> checked_size = 0;
    size_t num_planes = NumberOfPlanesForBufferFormat(format);
    for (size_t i = 0; i < num_planes; ++i) {
        size_t row_size = 0;
        if (!RowSizeForBufferFormatChecked(size.width(), format, i, &row_size))
            return false;
        base::CheckedNumeric<size_t> checked_plane_size = row_size;
        checked_plane_size *= size.height() / SubsamplingFactorForBufferFormat(format, i);
        if (!checked_plane_size.IsValid())
            return false;
        checked_size += checked_plane_size.ValueOrDie();
        if (!checked_size.IsValid())
            return false;
    }
    *size_in_bytes = checked_size.ValueOrDie();
    return true;
}

int BufferOffsetForBufferFormat(const Size& size,
    BufferFormat format,
    size_t plane)
{
    DCHECK_LT(plane, gfx::NumberOfPlanesForBufferFormat(format));
    switch (format) {
    case BufferFormat::ATC:
    case BufferFormat::ATCIA:
    case BufferFormat::DXT1:
    case BufferFormat::DXT5:
    case BufferFormat::ETC1:
    case BufferFormat::R_8:
    case BufferFormat::RGBA_4444:
    case BufferFormat::RGBX_8888:
    case BufferFormat::RGBA_8888:
    case BufferFormat::BGRX_8888:
    case BufferFormat::BGRA_8888:
    case BufferFormat::UYVY_422:
        return 0;
    case BufferFormat::YUV_420: {
        static size_t offset_in_2x2_sub_sampling_sizes[] = { 0, 4, 5 };
        DCHECK_LT(plane, arraysize(offset_in_2x2_sub_sampling_sizes));
        return offset_in_2x2_sub_sampling_sizes[plane] * (size.width() / 2 + size.height() / 2);
    }
    case gfx::BufferFormat::YUV_420_BIPLANAR: {
        static size_t offset_in_2x2_sub_sampling_sizes[] = { 0, 4 };
        DCHECK_LT(plane, arraysize(offset_in_2x2_sub_sampling_sizes));
        return offset_in_2x2_sub_sampling_sizes[plane] * (size.width() / 2 + size.height() / 2);
    }
    }
    NOTREACHED();
    return 0;
}

} // namespace gfx
