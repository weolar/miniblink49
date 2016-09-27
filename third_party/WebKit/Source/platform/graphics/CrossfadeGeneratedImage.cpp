/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/graphics/CrossfadeGeneratedImage.h"

#include "platform/geometry/FloatRect.h"
#include "platform/graphics/GraphicsContext.h"

namespace blink {

CrossfadeGeneratedImage::CrossfadeGeneratedImage(Image* fromImage, Image* toImage, float percentage, IntSize crossfadeSize, const IntSize& size)
    : GeneratedImage(size)
    , m_fromImage(fromImage)
    , m_toImage(toImage)
    , m_percentage(percentage)
    , m_crossfadeSize(crossfadeSize)
{
}

void CrossfadeGeneratedImage::drawCrossfade(SkCanvas* canvas, const SkPaint& paint, ImageClampingMode clampMode)
{
    FloatRect fromImageRect(FloatPoint(), m_fromImage->size());
    FloatRect toImageRect(FloatPoint(), m_toImage->size());
    FloatRect destRect(FloatPoint(), m_crossfadeSize);

    // TODO(junov): The various effects encoded into paint should probably be applied here
    // instead of inside the layer.  This probably faulty behavior was maintained in order
    // to preserve pre-existing behavior while refactoring this code.  This should be
    // investigated further. crbug.com/472634
    SkPaint layerPaint;
    layerPaint.setXfermode(paint.getXfermode());
    SkAutoCanvasRestore ar(canvas, false);
    canvas->saveLayer(nullptr, &layerPaint);

    SkPaint imagePaint(paint);
    imagePaint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
    int imageAlpha = clampedAlphaForBlending(1 - m_percentage);
    imagePaint.setAlpha(imageAlpha > 255 ? 255 : imageAlpha);
    imagePaint.setAntiAlias(paint.isAntiAlias());
    // TODO(junov): This code should probably be propagating the RespectImageOrientationEnum
    // form CrossfadeGeneratedImage::draw. Code was written this way during refactoring to
    // avoid modifying existing behavior, but this warrants further investigation. crbug.com/472634
    m_fromImage->draw(canvas, imagePaint, destRect, fromImageRect, DoNotRespectImageOrientation, clampMode);
    imagePaint.setXfermodeMode(SkXfermode::kPlus_Mode);
    imageAlpha = clampedAlphaForBlending(m_percentage);
    imagePaint.setAlpha(imageAlpha > 255 ? 255 : imageAlpha);
    m_toImage->draw(canvas, imagePaint, destRect, toImageRect, DoNotRespectImageOrientation, clampMode);
}

void CrossfadeGeneratedImage::draw(SkCanvas* canvas, const SkPaint& paint, const FloatRect& dstRect, const FloatRect& srcRect, RespectImageOrientationEnum, ImageClampingMode clampMode)
{
    // Draw nothing if either of the images hasn't loaded yet.
    if (m_fromImage == Image::nullImage() || m_toImage == Image::nullImage())
        return;

    SkAutoCanvasRestore ar(canvas, true);
    canvas->clipRect(dstRect);
    canvas->translate(dstRect.x(), dstRect.y());
    if (dstRect.size() != srcRect.size())
        canvas->scale(dstRect.width() / srcRect.width(), dstRect.height() / srcRect.height());
    canvas->translate(-srcRect.x(), -srcRect.y());

    drawCrossfade(canvas, paint, clampMode);
    canvas->restore();
}

void CrossfadeGeneratedImage::drawTile(GraphicsContext* context, const FloatRect& srcRect)
{
    // Draw nothing if either of the images hasn't loaded yet.
    if (m_fromImage == Image::nullImage() || m_toImage == Image::nullImage())
        return;

    SkPaint paint = context->fillPaint();
    paint.setXfermodeMode(SkXfermode::kSrcOver_Mode);
    paint.setAntiAlias(context->shouldAntialias());
    FloatRect destRect(FloatPoint(), m_crossfadeSize);
    paint.setFilterQuality(context->computeFilterQuality(this, destRect, srcRect));
    drawCrossfade(context->canvas(), paint, ClampImageToSourceRect);
}

} // namespace blink
