// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/ImagePainter.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLAreaElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/layout/LayoutImage.h"
#include "core/layout/LayoutReplaced.h"
#include "core/layout/TextRunConstructor.h"
#include "core/page/Page.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/geometry/LayoutPoint.h"
#include "platform/graphics/Path.h"

namespace blink {

void ImagePainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    m_layoutImage.LayoutReplaced::paint(paintInfo, paintOffset);

    if (paintInfo.phase == PaintPhaseOutline)
        paintAreaElementFocusRing(paintInfo);
}

void ImagePainter::paintAreaElementFocusRing(const PaintInfo& paintInfo)
{
    Document& document = m_layoutImage.document();

    if (document.printing() || !document.frame()->selection().isFocusedAndActive())
        return;

    Element* focusedElement = document.focusedElement();
    if (!isHTMLAreaElement(focusedElement))
        return;

    HTMLAreaElement& areaElement = toHTMLAreaElement(*focusedElement);
    if (areaElement.imageElement() != m_layoutImage.node())
        return;

    // Even if the theme handles focus ring drawing for entire elements, it won't do it for
    // an area within an image, so we don't call LayoutTheme::supportsFocusRing here.

    Path path = areaElement.computePath(&m_layoutImage);
    if (path.isEmpty())
        return;

    const ComputedStyle& areaElementStyle = *areaElement.ensureComputedStyle();
    int outlineWidth = areaElementStyle.outlineWidth();
    if (!outlineWidth)
        return;

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutImage, paintInfo.phase))
        return;

    IntRect focusRect = m_layoutImage.absoluteContentBox();
    LayoutObjectDrawingRecorder drawingRecorder(*paintInfo.context, m_layoutImage, paintInfo.phase, focusRect);

    // FIXME: Clip path instead of context when Skia pathops is ready.
    // https://crbug.com/251206

    paintInfo.context->save();
    paintInfo.context->clip(focusRect);
    paintInfo.context->drawFocusRing(path, outlineWidth,
        areaElementStyle.outlineOffset(),
        m_layoutImage.resolveColor(areaElementStyle, CSSPropertyOutlineColor));
    paintInfo.context->restore();
}

void ImagePainter::paintReplaced(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    LayoutUnit cWidth = m_layoutImage.contentWidth();
    LayoutUnit cHeight = m_layoutImage.contentHeight();

    GraphicsContext* context = paintInfo.context;

    if (!m_layoutImage.imageResource()->hasImage()) {
        if (paintInfo.phase == PaintPhaseSelection)
            return;
        if (cWidth > 2 && cHeight > 2) {
            if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, m_layoutImage, paintInfo.phase))
                return;
            // Draw an outline rect where the image should be.
            IntRect paintRect = pixelSnappedIntRect(LayoutRect(paintOffset.x() + m_layoutImage.borderLeft() + m_layoutImage.paddingLeft(), paintOffset.y() + m_layoutImage.borderTop() + m_layoutImage.paddingTop(), cWidth, cHeight));
            LayoutObjectDrawingRecorder drawingRecorder(*context, m_layoutImage, paintInfo.phase, paintRect);
            context->setStrokeStyle(SolidStroke);
            context->setStrokeColor(Color::lightGray);
            context->setFillColor(Color::transparent);
            context->drawRect(paintRect);
        }
    } else if (cWidth > 0 && cHeight > 0) {
        if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, m_layoutImage, paintInfo.phase))
            return;
        LayoutRect contentRect = m_layoutImage.contentBoxRect();
        contentRect.moveBy(paintOffset);
        LayoutRect paintRect = m_layoutImage.replacedContentRect();
        paintRect.moveBy(paintOffset);

        LayoutObjectDrawingRecorder drawingRecorder(*context, m_layoutImage, paintInfo.phase, contentRect);
        bool clip = !contentRect.contains(paintRect);
        if (clip) {
            context->save();
            context->clip(contentRect);
        }

        paintIntoRect(context, paintRect);

        if (clip)
            context->restore();
    }
}

void ImagePainter::paintIntoRect(GraphicsContext* context, const LayoutRect& rect)
{
    if (!m_layoutImage.imageResource()->hasImage() || m_layoutImage.imageResource()->errorOccurred())
        return; // FIXME: should we just ASSERT these conditions? (audit all callers).

    IntRect alignedRect = pixelSnappedIntRect(rect);
    if (alignedRect.width() <= 0 || alignedRect.height() <= 0)
        return;

    RefPtr<Image> image = m_layoutImage.imageResource()->image(alignedRect.width(), alignedRect.height());
    if (!image || image->isNull())
        return;

    // FIXME: why is interpolation quality selection not included in the Instrumentation reported cost of drawing an image?
    InterpolationQuality interpolationQuality = BoxPainter::chooseInterpolationQuality(m_layoutImage, context, image.get(), image.get(), LayoutSize(alignedRect.size()));

    TRACE_EVENT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "PaintImage", "data", InspectorPaintImageEvent::data(m_layoutImage));

    InterpolationQuality previousInterpolationQuality = context->imageInterpolationQuality();
    context->setImageInterpolationQuality(interpolationQuality);
    context->drawImage(image.get(), alignedRect, SkXfermode::kSrcOver_Mode, m_layoutImage.shouldRespectImageOrientation());
    context->setImageInterpolationQuality(previousInterpolationQuality);
}

} // namespace blink
