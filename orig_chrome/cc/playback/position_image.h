// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_POSITION_IMAGE_H_
#define CC_PLAYBACK_POSITION_IMAGE_H_

#include "third_party/skia/include/core/SkFilterQuality.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "third_party/skia/include/core/SkRect.h"

namespace cc {

struct PositionImage {
    PositionImage(const SkImage* image,
        const SkRect& image_rect,
        const SkMatrix& matrix,
        SkFilterQuality filter_quality)
        : image(image)
        , image_rect(image_rect)
        , matrix(matrix)
        , filter_quality(filter_quality)
    {
    }

    const SkImage* image;
    SkRect image_rect;
    SkMatrix matrix;
    SkFilterQuality filter_quality;
};

} // namespace cc

#endif // CC_PLAYBACK_POSITION_IMAGE_H_
