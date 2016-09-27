// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/BitmapPatternBase.h"

#include "platform/graphics/skia/SkiaUtils.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkShader.h"

namespace blink {

BitmapPatternBase::BitmapPatternBase(RepeatMode repeatMode, int64_t externalMemoryAllocated) : Pattern(repeatMode, externalMemoryAllocated) { }

BitmapPatternBase::~BitmapPatternBase() { }

PassRefPtr<SkShader> BitmapPatternBase::createShader()
{
    // Skia does not have a "draw the tile only once" option. Clamp_TileMode
    // repeats the last line of the image after drawing one tile. To avoid
    // filling the space with arbitrary pixels, this workaround forces the
    // image to have a line of transparent pixels on the "repeated" edge(s),
    // thus causing extra space to be transparent filled.
    SkShader::TileMode tileModeX = isRepeatX() ? SkShader::kRepeat_TileMode : SkShader::kClamp_TileMode;
    SkShader::TileMode tileModeY = isRepeatY() ? SkShader::kRepeat_TileMode : SkShader::kClamp_TileMode;
    int expandW = isRepeatX() ? 0 : 1;
    int expandH = isRepeatY() ? 0 : 1;

    // Create a transparent bitmap 1 pixel wider and/or taller than the
    // original, then copy the orignal into it.
    // FIXME: Is there a better way to pad (not scale) an image in skia?
    SkImageInfo info = this->getBitmapInfo();
    // we explicitly require non-opaquness, since we are going to add a transparent strip.
    info = SkImageInfo::Make(info.width() + expandW, info.height() + expandH, info.colorType(), kPremul_SkAlphaType);

    SkBitmap bm2;
    bm2.allocPixels(info);
    bm2.eraseColor(SK_ColorTRANSPARENT);
    SkCanvas canvas(bm2);

    SkPaint paint;
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);

    this->drawBitmapToCanvas(canvas, paint);

    paint.setARGB(0x00, 0x00, 0x00, 0x00);
    paint.setStyle(SkPaint::kFill_Style);

    if (!isRepeatX())
        canvas.drawRect(SkRect::MakeXYWH(info.width() - 1, 0, 1, info.height()), paint);

    if (!isRepeatY())
        canvas.drawRect(SkRect::MakeXYWH(0, info.height() - 1, info.width(), 1), paint);

    bm2.setImmutable();

    adjustExternalMemoryAllocated(bm2.getSafeSize());

    SkMatrix localMatrix = affineTransformToSkMatrix(m_patternSpaceTransformation);

    return adoptRef(SkShader::CreateBitmapShader(bm2, tileModeX, tileModeY, &localMatrix));
}

} // namespace
