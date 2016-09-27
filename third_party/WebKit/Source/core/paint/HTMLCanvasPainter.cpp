// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/HTMLCanvasPainter.h"

#include "core/html/HTMLCanvasElement.h"
#include "core/layout/LayoutHTMLCanvas.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/geometry/LayoutPoint.h"
#include "platform/graphics/paint/ClipRecorder.h"

namespace blink {

void HTMLCanvasPainter::paintReplaced(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    GraphicsContext* context = paintInfo.context;
    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, m_layoutHTMLCanvas, paintInfo.phase))
        return;

    LayoutRect contentRect = m_layoutHTMLCanvas.contentBoxRect();
    contentRect.moveBy(paintOffset);
    LayoutRect paintRect = m_layoutHTMLCanvas.replacedContentRect();
    paintRect.moveBy(paintOffset);

    LayoutObjectDrawingRecorder drawingRecorder(*context, m_layoutHTMLCanvas, paintInfo.phase, contentRect);
#if ENABLE(ASSERT)
    // The drawing may be in display list mode or image mode, producing different pictures for the same result.
    drawingRecorder.setUnderInvalidationCheckingMode(DrawingDisplayItem::CheckBitmap);
#endif

    bool clip = !contentRect.contains(paintRect);
    if (clip) {
        context->save();
        context->clip(contentRect);
    }

    // FIXME: InterpolationNone should be used if ImageRenderingOptimizeContrast is set.
    // See bug for more details: crbug.com/353716.
    InterpolationQuality interpolationQuality = m_layoutHTMLCanvas.style()->imageRendering() == ImageRenderingOptimizeContrast ? InterpolationLow : CanvasDefaultInterpolationQuality;
    if (m_layoutHTMLCanvas.style()->imageRendering() == ImageRenderingPixelated)
        interpolationQuality = InterpolationNone;

    InterpolationQuality previousInterpolationQuality = context->imageInterpolationQuality();
    context->setImageInterpolationQuality(interpolationQuality);
    toHTMLCanvasElement(m_layoutHTMLCanvas.node())->paint(context, paintRect);
    context->setImageInterpolationQuality(previousInterpolationQuality);

    if (clip)
        context->restore();
}

} // namespace blink
