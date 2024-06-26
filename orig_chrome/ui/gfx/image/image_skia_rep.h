// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_IMAGE_IMAGE_SKIA_REP_H_
#define UI_GFX_IMAGE_IMAGE_SKIA_REP_H_

#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gfx_export.h"

namespace gfx {

// An ImageSkiaRep represents a bitmap and the scale factor it is intended for.
// 0.0f scale is used to indicate that this ImageSkiaRep is used for unscaled
// image (the image that only returns 1.0f scale image).
class GFX_EXPORT ImageSkiaRep {
public:
    // Create null bitmap.
    ImageSkiaRep();
    ~ImageSkiaRep();

    // Note: This is for testing purpose only.
    // Creates a bitmap with kARGB_8888_Config config with given |size| in DIP.
    // This allocates pixels in the bitmap. It is guaranteed that the data in the
    // bitmap are initialized but the actual values are undefined.
    // Specifying 0 scale means the image is for unscaled image. (unscaled()
    // returns truen, and scale() returns 1.0f;)
    ImageSkiaRep(const gfx::Size& size, float scale);

    // Creates a bitmap with given scale.
    // Adds ref to |src|.
    ImageSkiaRep(const SkBitmap& src, float scale);

    // Returns true if the backing bitmap is null.
    bool is_null() const { return bitmap_.isNull(); }

    // Get width and height of bitmap in DIP.
    int GetWidth() const;
    int GetHeight() const;

    // Get width and height of bitmap in pixels.
    int pixel_width() const { return bitmap_.width(); }
    int pixel_height() const { return bitmap_.height(); }
    Size pixel_size() const
    {
        return Size(pixel_width(), pixel_height());
    }

    // Retrieves the scale that the bitmap will be painted at.
    float scale() const { return unscaled() ? 1.0f : scale_; }

    bool unscaled() const { return scale_ == 0.0f; }

    // Mark the image to be used as scaled image.
    void SetScaled();

    // Returns backing bitmap.
    const SkBitmap& sk_bitmap() const { return bitmap_; }

private:
    SkBitmap bitmap_;

    float scale_;
};

} // namespace gfx

#endif // UI_GFX_IMAGE_IMAGE_SKIA_REP_H_
