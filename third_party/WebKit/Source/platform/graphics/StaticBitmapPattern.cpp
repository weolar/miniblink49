// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/StaticBitmapPattern.h"

#include "platform/graphics/skia/SkiaUtils.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkShader.h"

namespace blink {

PassRefPtr<Pattern> StaticBitmapPattern::create(PassRefPtr<Image> tileImage, RepeatMode repeatMode)
{
    return adoptRef(new StaticBitmapPattern(tileImage->skImage(), repeatMode));
}

StaticBitmapPattern::StaticBitmapPattern(PassRefPtr<SkImage> image, RepeatMode repeatMode)
: BitmapPatternBase(repeatMode, 0)
{
    if (image) {
        m_tileImage = image;
        adjustExternalMemoryAllocated(m_tileImage->width() * m_tileImage->height() * 4);
    }
}

StaticBitmapPattern::~StaticBitmapPattern() { }

PassRefPtr<SkShader> StaticBitmapPattern::createShader()
{
    // If we have no image, return null
    if (!m_tileImage) {
        return adoptRef(SkShader::CreateColorShader(SK_ColorTRANSPARENT));
    }

    SkMatrix localMatrix = affineTransformToSkMatrix(m_patternSpaceTransformation);

    if (isRepeatXY()) {
        return adoptRef(m_tileImage->newShader(SkShader::kRepeat_TileMode, SkShader::kRepeat_TileMode, &localMatrix));
    }

    return BitmapPatternBase::createShader();
}

SkImageInfo StaticBitmapPattern::getBitmapInfo()
{
    return SkImageInfo::MakeN32Premul(m_tileImage->width(), m_tileImage->height());
}

void StaticBitmapPattern::drawBitmapToCanvas(SkCanvas& canvas, SkPaint& paint)
{
    canvas.drawImage(m_tileImage.get(), SkIntToScalar(0), SkIntToScalar(0), &paint);
}

} // namespace
