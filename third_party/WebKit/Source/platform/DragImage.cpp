/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
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
#include "platform/DragImage.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/fonts/Font.h"
#include "platform/fonts/FontCache.h"
#include "platform/fonts/FontDescription.h"
#include "platform/fonts/FontMetrics.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/IntPoint.h"
#include "platform/graphics/BitmapImage.h"
#include "platform/graphics/Color.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/Image.h"
#include "platform/graphics/ImageBuffer.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "platform/graphics/paint/SkPictureBuilder.h"
#include "platform/text/BidiTextRun.h"
#include "platform/text/StringTruncator.h"
#include "platform/text/TextRun.h"
#include "platform/transforms/AffineTransform.h"
#include "platform/weborigin/KURL.h"
#include "skia/ext/image_operations.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkMatrix.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"

#include <algorithm>

namespace blink {

const float kDragLabelBorderX = 4;
// Keep border_y in synch with DragController::LinkDragBorderInset.
const float kDragLabelBorderY = 2;
const float kLabelBorderYOffset = 2;

const float kMaxDragLabelWidth = 300;
const float kMaxDragLabelStringWidth = (kMaxDragLabelWidth - 2 * kDragLabelBorderX);

const float kDragLinkLabelFontSize = 11;
const float kDragLinkUrlFontSize = 10;

PassOwnPtr<DragImage> DragImage::create(Image* image, RespectImageOrientationEnum shouldRespectImageOrientation, float deviceScaleFactor, InterpolationQuality interpolationQuality)
{
    if (!image)
        return nullptr;

    SkBitmap bitmap;
    if (!image->bitmapForCurrentFrame(&bitmap))
        return nullptr;

    if (image->isBitmapImage()) {
        ImageOrientation orientation = DefaultImageOrientation;
        BitmapImage* bitmapImage = toBitmapImage(image);
        IntSize sizeRespectingOrientation = bitmapImage->sizeRespectingOrientation();

        if (shouldRespectImageOrientation == RespectImageOrientation)
            orientation = bitmapImage->currentFrameOrientation();

        if (orientation != DefaultImageOrientation) {
            FloatRect destRect(FloatPoint(), sizeRespectingOrientation);
            if (orientation.usesWidthAsHeight())
                destRect = destRect.transposedRect();

            SkBitmap skBitmap;
            if (!skBitmap.tryAllocN32Pixels(sizeRespectingOrientation.width(), sizeRespectingOrientation.height()))
                return nullptr;

            skBitmap.eraseColor(SK_ColorTRANSPARENT);
            SkCanvas canvas(skBitmap);
            canvas.concat(affineTransformToSkMatrix(orientation.transformFromDefault(sizeRespectingOrientation)));
            canvas.drawBitmapRect(bitmap, 0, destRect);

            return adoptPtr(new DragImage(skBitmap, deviceScaleFactor, interpolationQuality));
        }
    }

    SkBitmap skBitmap;
    if (!bitmap.copyTo(&skBitmap, kN32_SkColorType))
        return nullptr;
    return adoptPtr(new DragImage(skBitmap, deviceScaleFactor, interpolationQuality));
}

static Font deriveDragLabelFont(int size, FontWeight fontWeight, const FontDescription& systemFont)
{
    FontDescription description = systemFont;
    description.setWeight(fontWeight);
    description.setSpecifiedSize(size);
    description.setComputedSize(size);
    Font result(description);
    result.update(nullptr);
    return result;
}

PassOwnPtr<DragImage> DragImage::create(const KURL& url, const String& inLabel, const FontDescription& systemFont, float deviceScaleFactor)
{
    const Font labelFont = deriveDragLabelFont(kDragLinkLabelFontSize, FontWeightBold, systemFont);
    const Font urlFont = deriveDragLabelFont(kDragLinkUrlFontSize, FontWeightNormal, systemFont);
    FontCachePurgePreventer fontCachePurgePreventer;

    bool drawURLString = true;
    bool clipURLString = false;
    bool clipLabelString = false;

    String urlString = url.string();
    String label = inLabel.stripWhiteSpace();
    if (label.isEmpty()) {
        drawURLString = false;
        label = urlString;
    }

    // First step is drawing the link drag image width.
    TextRun labelRun(label.impl());
    TextRun urlRun(urlString.impl());
    IntSize labelSize(labelFont.width(labelRun), labelFont.fontMetrics().ascent() + labelFont.fontMetrics().descent());

    if (labelSize.width() > kMaxDragLabelStringWidth) {
        labelSize.setWidth(kMaxDragLabelStringWidth);
        clipLabelString = true;
    }

    IntSize urlStringSize;
    IntSize imageSize(labelSize.width() + kDragLabelBorderX * 2, labelSize.height() + kDragLabelBorderY * 2);

    if (drawURLString) {
        urlStringSize.setWidth(urlFont.width(urlRun));
        urlStringSize.setHeight(urlFont.fontMetrics().ascent() + urlFont.fontMetrics().descent());
        imageSize.setHeight(imageSize.height() + urlStringSize.height());
        if (urlStringSize.width() > kMaxDragLabelStringWidth) {
            imageSize.setWidth(kMaxDragLabelWidth);
            clipURLString = true;
        } else
            imageSize.setWidth(std::max(labelSize.width(), urlStringSize.width()) + kDragLabelBorderX * 2);
    }

    // We now know how big the image needs to be, so we create and
    // fill the background
    IntSize scaledImageSize = imageSize;
    scaledImageSize.scale(deviceScaleFactor);
    OwnPtr<ImageBuffer> buffer(ImageBuffer::create(scaledImageSize));
    if (!buffer)
        return nullptr;

    buffer->canvas()->scale(deviceScaleFactor, deviceScaleFactor);

    const float DragLabelRadius = 5;

    IntRect rect(IntPoint(), imageSize);
    SkPaint backgroundPaint;
    backgroundPaint.setColor(SkColorSetRGB(140, 140, 140));
    SkRRect rrect;
    rrect.setRectXY(SkRect::MakeWH(imageSize.width(), imageSize.height()), DragLabelRadius, DragLabelRadius);
    buffer->canvas()->drawRRect(rrect, backgroundPaint);

    // Draw the text
    SkPaint textPaint;
    if (drawURLString) {
        if (clipURLString)
            urlString = StringTruncator::centerTruncate(urlString, imageSize.width() - (kDragLabelBorderX * 2.0f), urlFont);
        IntPoint textPos(kDragLabelBorderX, imageSize.height() - (kLabelBorderYOffset + urlFont.fontMetrics().descent()));
        TextRun textRun(urlString);
        urlFont.drawText(buffer->canvas(), TextRunPaintInfo(textRun), textPos, deviceScaleFactor, textPaint);
    }

    if (clipLabelString)
        label = StringTruncator::rightTruncate(label, imageSize.width() - (kDragLabelBorderX * 2.0f), labelFont);

    bool hasStrongDirectionality;
    TextRun textRun = textRunWithDirectionality(label, &hasStrongDirectionality);
    IntPoint textPos(kDragLabelBorderX, kDragLabelBorderY + labelFont.fontDescription().computedPixelSize());
    if (hasStrongDirectionality && textRun.direction() == RTL) {
        float textWidth = labelFont.width(textRun);
        int availableWidth = imageSize.width() - kDragLabelBorderX * 2;
        textPos.setX(availableWidth - ceilf(textWidth));
    }
    labelFont.drawBidiText(buffer->canvas(), TextRunPaintInfo(textRun), FloatPoint(textPos), Font::DoNotPaintIfFontNotReady, deviceScaleFactor, textPaint);

    RefPtr<Image> image = buffer->copyImage();
    return DragImage::create(image.get(), DoNotRespectImageOrientation, deviceScaleFactor);
}

DragImage::DragImage(const SkBitmap& bitmap, float resolutionScale, InterpolationQuality interpolationQuality)
    : m_bitmap(bitmap)
    , m_resolutionScale(resolutionScale)
    , m_interpolationQuality(interpolationQuality)
{
}

DragImage::~DragImage()
{
}

void DragImage::fitToMaxSize(const IntSize& srcSize, const IntSize& maxSize)
{
    float heightResizeRatio = 0.0f;
    float widthResizeRatio = 0.0f;
    float resizeRatio = -1.0f;
    IntSize originalSize = size();

    if (srcSize.width() > maxSize.width()) {
        widthResizeRatio = maxSize.width() / static_cast<float>(srcSize.width());
        resizeRatio = widthResizeRatio;
    }

    if (srcSize.height() > maxSize.height()) {
        heightResizeRatio = maxSize.height() / static_cast<float>(srcSize.height());
        if ((resizeRatio < 0.0f) || (resizeRatio > heightResizeRatio))
            resizeRatio = heightResizeRatio;
    }

    if (srcSize == originalSize) {
        if (resizeRatio > 0.0f)
            scale(resizeRatio, resizeRatio);
        return;
    }

    // The image was scaled in the webpage so at minimum we must account for that scaling
    float scaleX = srcSize.width() / static_cast<float>(originalSize.width());
    float scaleY = srcSize.height() / static_cast<float>(originalSize.height());
    if (resizeRatio > 0.0f) {
        scaleX *= resizeRatio;
        scaleY *= resizeRatio;
    }

    scale(scaleX, scaleY);
}

void DragImage::scale(float scaleX, float scaleY)
{
    skia::ImageOperations::ResizeMethod resizeMethod = m_interpolationQuality == InterpolationNone ? skia::ImageOperations::RESIZE_BOX : skia::ImageOperations::RESIZE_LANCZOS3;
    int imageWidth = scaleX * m_bitmap.width();
    int imageHeight = scaleY * m_bitmap.height();
#ifdef MINIBLINK_NOT_IMPLEMENTED
    m_bitmap = skia::ImageOperations::Resize(m_bitmap, resizeMethod, imageWidth, imageHeight);
#else
	SkBitmap bitmap;
	bitmap.allocN32Pixels(imageWidth, imageWidth);

	SkCanvas canvas(bitmap);
	canvas.drawBitmapRect(m_bitmap, nullptr, SkRect::MakeWH(imageWidth, imageHeight), nullptr);
	m_bitmap = bitmap;
#endif // MINIBLINK_NOT_IMPLEMENTED
//    notImplemented();
}

void DragImage::dissolveToFraction(float fraction)
{
    m_bitmap.setAlphaType(kPremul_SkAlphaType);
    SkAutoLockPixels lock(m_bitmap);

    for (int row = 0; row < m_bitmap.height(); ++row) {
        for (int column = 0; column < m_bitmap.width(); ++column) {
            uint32_t* pixel = m_bitmap.getAddr32(column, row);
            *pixel = SkPreMultiplyARGB(
                SkColorGetA(*pixel) * fraction,
                SkColorGetR(*pixel),
                SkColorGetG(*pixel),
                SkColorGetB(*pixel));
        }
    }
}

} // namespace blink
