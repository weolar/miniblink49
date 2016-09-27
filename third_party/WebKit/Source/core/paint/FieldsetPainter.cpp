// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/FieldsetPainter.h"

#include "core/layout/LayoutFieldset.h"
#include "core/paint/BoxDecorationData.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/graphics/GraphicsContextStateSaver.h"

namespace blink {

void FieldsetPainter::paintBoxDecorationBackground(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (!paintInfo.shouldPaintWithinRoot(&m_layoutFieldset))
        return;

    LayoutRect paintRect(paintOffset, m_layoutFieldset.size());
    LayoutBox* legend = m_layoutFieldset.findInFlowLegend();
    if (!legend)
        return BoxPainter(m_layoutFieldset).paintBoxDecorationBackground(paintInfo, paintOffset);

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutFieldset, paintInfo.phase))
        return;

    // FIXME: We need to work with "rl" and "bt" block flow directions.  In those
    // cases the legend is embedded in the right and bottom borders respectively.
    // https://bugs.webkit.org/show_bug.cgi?id=47236
    if (m_layoutFieldset.style()->isHorizontalWritingMode()) {
        LayoutUnit yOff = (legend->location().y() > 0) ? LayoutUnit() : (legend->size().height() - m_layoutFieldset.borderTop()) / 2;
        paintRect.setHeight(paintRect.height() - yOff);
        paintRect.setY(paintRect.y() + yOff);
    } else {
        LayoutUnit xOff = (legend->location().x() > 0) ? LayoutUnit() : (legend->size().width() - m_layoutFieldset.borderLeft()) / 2;
        paintRect.setWidth(paintRect.width() - xOff);
        paintRect.setX(paintRect.x() + xOff);
    }

    LayoutObjectDrawingRecorder recorder(*paintInfo.context, m_layoutFieldset, paintInfo.phase, pixelSnappedIntRect(paintRect));
    BoxDecorationData boxDecorationData(m_layoutFieldset);

    if (boxDecorationData.bleedAvoidance == BackgroundBleedNone)
        BoxPainter::paintBoxShadow(paintInfo, paintRect, m_layoutFieldset.styleRef(), Normal);
    BoxPainter(m_layoutFieldset).paintFillLayers(paintInfo, boxDecorationData.backgroundColor, m_layoutFieldset.style()->backgroundLayers(), paintRect);
    BoxPainter::paintBoxShadow(paintInfo, paintRect, m_layoutFieldset.styleRef(), Inset);

    if (!boxDecorationData.hasBorderDecoration)
        return;

    // Create a clipping region around the legend and paint the border as normal
    GraphicsContext* graphicsContext = paintInfo.context;
    GraphicsContextStateSaver stateSaver(*graphicsContext);

    // FIXME: We need to work with "rl" and "bt" block flow directions.  In those
    // cases the legend is embedded in the right and bottom borders respectively.
    // https://bugs.webkit.org/show_bug.cgi?id=47236
    if (m_layoutFieldset.style()->isHorizontalWritingMode()) {
        LayoutUnit clipTop = paintRect.y();
        LayoutUnit clipHeight = max(static_cast<LayoutUnit>(m_layoutFieldset.style()->borderTopWidth()), legend->size().height() - ((legend->size().height() - m_layoutFieldset.borderTop()) / 2));
        graphicsContext->clipOut(pixelSnappedIntRect(paintRect.x() + legend->location().x(), clipTop, legend->size().width(), clipHeight));
    } else {
        LayoutUnit clipLeft = paintRect.x();
        LayoutUnit clipWidth = max(static_cast<LayoutUnit>(m_layoutFieldset.style()->borderLeftWidth()), legend->size().width());
        graphicsContext->clipOut(pixelSnappedIntRect(clipLeft, paintRect.y() + legend->location().y(), clipWidth, legend->size().height()));
    }

    BoxPainter::paintBorder(m_layoutFieldset, paintInfo, paintRect, m_layoutFieldset.styleRef());
}

void FieldsetPainter::paintMask(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (m_layoutFieldset.style()->visibility() != VISIBLE || paintInfo.phase != PaintPhaseMask)
        return;

    LayoutRect paintRect = LayoutRect(paintOffset, m_layoutFieldset.size());
    LayoutBox* legend = m_layoutFieldset.findInFlowLegend();
    if (!legend)
        return BoxPainter(m_layoutFieldset).paintMask(paintInfo, paintOffset);

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutFieldset, paintInfo.phase))
        return;

    // FIXME: We need to work with "rl" and "bt" block flow directions.  In those
    // cases the legend is embedded in the right and bottom borders respectively.
    // https://bugs.webkit.org/show_bug.cgi?id=47236
    if (m_layoutFieldset.style()->isHorizontalWritingMode()) {
        LayoutUnit yOff = (legend->location().y() > 0) ? LayoutUnit() : (legend->size().height() - m_layoutFieldset.borderTop()) / 2;
        paintRect.expand(0, -yOff);
        paintRect.move(0, yOff);
    } else {
        LayoutUnit xOff = (legend->location().x() > 0) ? LayoutUnit() : (legend->size().width() - m_layoutFieldset.borderLeft()) / 2;
        paintRect.expand(-xOff, 0);
        paintRect.move(xOff, 0);
    }

    LayoutObjectDrawingRecorder recorder(*paintInfo.context, m_layoutFieldset, paintInfo.phase, paintRect);
    BoxPainter(m_layoutFieldset).paintMaskImages(paintInfo, paintRect);
}

} // namespace blink
