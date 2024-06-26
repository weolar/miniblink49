// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/image/image_util.h"

#include <stdint.h>

#include <algorithm>

#include "base/memory/scoped_ptr.h"
#include "build/build_config.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/image/image_skia.h"

namespace gfx {

const uint32_t kMinimumVisibleOpacity = 12;

// The iOS implementations of the JPEG functions are in image_util_ios.mm.
#if !defined(OS_IOS)
Image ImageFrom1xJPEGEncodedData(const unsigned char* input,
    size_t input_size)
{
    scoped_ptr<SkBitmap> bitmap(gfx::JPEGCodec::Decode(input, input_size));
    if (bitmap.get())
        return Image::CreateFrom1xBitmap(*bitmap);

    return Image();
}

bool JPEG1xEncodedDataFromImage(const Image& image, int quality,
    std::vector<unsigned char>* dst)
{
    const gfx::ImageSkiaRep& image_skia_rep = image.AsImageSkia().GetRepresentation(1.0f);
    if (image_skia_rep.scale() != 1.0f)
        return false;

    const SkBitmap& bitmap = image_skia_rep.sk_bitmap();
    SkAutoLockPixels bitmap_lock(bitmap);

    if (!bitmap.readyToDraw())
        return false;

    return gfx::JPEGCodec::Encode(
        reinterpret_cast<unsigned char*>(bitmap.getAddr32(0, 0)),
        gfx::JPEGCodec::FORMAT_SkBitmap, bitmap.width(),
        bitmap.height(),
        static_cast<int>(bitmap.rowBytes()), quality,
        dst);
}
#endif // !defined(OS_IOS)

bool VisibleMargins(const ImageSkia& image, int* leading, int* trailing)
{
    *leading = 0;
    *trailing = std::max(1, image.width()) - 1;
    if (!image.HasRepresentation(1.0))
        return false;

    const ImageSkiaRep& rep = image.GetRepresentation(1.0);
    if (rep.is_null())
        return false;

    const SkBitmap& bitmap = rep.sk_bitmap();
    if (bitmap.isNull() || bitmap.width() == 0)
        return false;

    if (bitmap.isOpaque())
        return true;

    SkAutoLockPixels l(bitmap);
    int inner_min = bitmap.width();
    for (int x = 0; x < bitmap.width(); ++x) {
        for (int y = 0; y < bitmap.height(); ++y) {
            if (SkColorGetA(bitmap.getColor(x, y)) > kMinimumVisibleOpacity) {
                inner_min = x;
                break;
            }
        }
        if (inner_min < bitmap.width())
            break;
    }

    int inner_max = -1;
    for (int x = bitmap.width() - 1; x > inner_min; --x) {
        for (int y = 0; y < bitmap.height(); ++y) {
            if (SkColorGetA(bitmap.getColor(x, y)) > kMinimumVisibleOpacity) {
                inner_max = x;
                break;
            }
        }
        if (inner_max > -1)
            break;
    }

    if (inner_min == bitmap.width()) {
        *leading = bitmap.width() / 2;
        *trailing = bitmap.width() / 2 + 1;
        return true;
    }

    *leading = inner_min;
    *trailing = inner_max;
    return true;
}

} // namespace gfx
