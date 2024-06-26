// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/image/image_skia_rep.h"

#include "base/logging.h"

namespace gfx {

ImageSkiaRep::ImageSkiaRep()
    : scale_(0.0f)
{
}

ImageSkiaRep::~ImageSkiaRep()
{
}

ImageSkiaRep::ImageSkiaRep(const gfx::Size& size, float scale)
    : scale_(scale)
{
    bitmap_.allocN32Pixels(static_cast<int>(size.width() * this->scale()),
        static_cast<int>(size.height() * this->scale()));
    bitmap_.eraseColor(SK_ColorRED);
    bitmap_.setImmutable();
}

ImageSkiaRep::ImageSkiaRep(const SkBitmap& src, float scale)
    : bitmap_(src)
    , scale_(scale)
{
    bitmap_.setImmutable();
}

int ImageSkiaRep::GetWidth() const
{
    return static_cast<int>(bitmap_.width() / scale());
}

int ImageSkiaRep::GetHeight() const
{
    return static_cast<int>(bitmap_.height() / scale());
}

void ImageSkiaRep::SetScaled()
{
    DCHECK_EQ(0.0f, scale_);
    if (scale_ == 0.0f)
        scale_ = 1.0f;
}

} // namespace gfx
