// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/SVGImagePainter.h"

#include "core/layout/ImageQualityController.h"
#include "core/layout/LayoutImageResource.h"
#include "core/layout/svg/LayoutSVGImage.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/ObjectPainter.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/SVGPaintContext.h"
#include "core/paint/TransformRecorder.h"
#include "core/svg/SVGImageElement.h"
#include "platform/graphics/GraphicsContext.h"
#include "third_party/skia/include/core/SkPicture.h"

namespace blink {

void SVGImagePainter::paint(const PaintInfo& paintInfo)
{
    if (paintInfo.phase != PaintPhaseForeground
        || m_layoutSVGImage.style()->visibility() == HIDDEN
        || !m_layoutSVGImage.imageResource()->hasImage())
        return;

    FloatRect boundingBox = m_layoutSVGImage.paintInvalidationRectInLocalCoordinates();
    if (!paintInfo.intersectsCullRect(m_layoutSVGImage.localToParentTransform(), boundingBox))
        return;

    PaintInfo paintInfoBeforeFiltering(paintInfo);
    // Images cannot have children so do not call updateCullRectForSVGTransform.
    TransformRecorder transformRecorder(*paintInfoBeforeFiltering.context, m_layoutSVGImage, m_layoutSVGImage.localToParentTransform());
    {
        SVGPaintContext paintContext(m_layoutSVGImage, paintInfoBeforeFiltering);
        if (paintContext.applyClipMaskAndFilterIfNecessary() && !LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintContext.paintInfo().context, m_layoutSVGImage, paintContext.paintInfo().phase)) {
            LayoutObjectDrawingRecorder recorder(*paintContext.paintInfo().context, m_layoutSVGImage, paintContext.paintInfo().phase, boundingBox);
            // There's no need to cache a buffered SkPicture with slimming
            // paint because it's automatically done in the display list.
            if (m_layoutSVGImage.style()->svgStyle().bufferedRendering() != BR_STATIC || RuntimeEnabledFeatures::slimmingPaintEnabled()) {
                paintForeground(paintContext.paintInfo());
            } else {
                RefPtr<const SkPicture>& bufferedForeground = m_layoutSVGImage.bufferedForeground();
                if (!bufferedForeground) {
                    paintContext.paintInfo().context->beginRecording(m_layoutSVGImage.objectBoundingBox());
                    paintForeground(paintContext.paintInfo());
                    bufferedForeground = paintContext.paintInfo().context->endRecording();
                }

                paintContext.paintInfo().context->drawPicture(bufferedForeground.get());
            }
        }
    }

    if (m_layoutSVGImage.style()->outlineWidth()) {
        PaintInfo outlinePaintInfo(paintInfoBeforeFiltering);
        outlinePaintInfo.phase = PaintPhaseSelfOutline;
        LayoutRect layoutBoundingBox(boundingBox);
        LayoutRect visualOverflowRect = ObjectPainter::outlineBounds(layoutBoundingBox, m_layoutSVGImage.styleRef());
        ObjectPainter(m_layoutSVGImage).paintOutline(outlinePaintInfo, layoutBoundingBox, visualOverflowRect);
    }
}

void SVGImagePainter::paintForeground(const PaintInfo& paintInfo)
{
    RefPtr<Image> image = m_layoutSVGImage.imageResource()->image();
    FloatRect destRect = m_layoutSVGImage.objectBoundingBox();
    FloatRect srcRect(0, 0, image->width(), image->height());

    SVGImageElement* imageElement = toSVGImageElement(m_layoutSVGImage.element());
    imageElement->preserveAspectRatio()->currentValue()->transformRect(destRect, srcRect);

    InterpolationQuality interpolationQuality = InterpolationDefault;
    if (m_layoutSVGImage.style()->svgStyle().bufferedRendering() != BR_STATIC || RuntimeEnabledFeatures::slimmingPaintEnabled())
        interpolationQuality = ImageQualityController::imageQualityController()->chooseInterpolationQuality(paintInfo.context, &m_layoutSVGImage, image.get(), image.get(), LayoutSize(destRect.size()));

    InterpolationQuality previousInterpolationQuality = paintInfo.context->imageInterpolationQuality();
    paintInfo.context->setImageInterpolationQuality(interpolationQuality);
    paintInfo.context->drawImage(image.get(), destRect, srcRect, SkXfermode::kSrcOver_Mode);
    paintInfo.context->setImageInterpolationQuality(previousInterpolationQuality);
}

} // namespace blink
