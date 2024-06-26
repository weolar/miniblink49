// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_IMAGE_IMAGE_SKIA_SOURCE_H_
#define UI_GFX_IMAGE_IMAGE_SKIA_SOURCE_H_

#include <vector>

#include "ui/gfx/gfx_export.h"

namespace gfx {

class ImageSkiaRep;

class GFX_EXPORT ImageSkiaSource {
public:
    virtual ~ImageSkiaSource() { }

    // Returns the ImageSkiaRep for the given |scale|. ImageSkia caches the
    // returned ImageSkiaRep and calls this method only if it doesn't have
    // ImageSkiaRep for given |scale|. There is no need for the implementation to
    // cache the image.
    virtual gfx::ImageSkiaRep GetImageForScale(float scale) = 0;
};

} // namespace gfx

#endif // UI_GFX_IMAGE_IMAGE_SKIA_SOURCE_H_
