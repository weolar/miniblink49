// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/PartPainter.h"

#include "core/layout/LayoutPart.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/RoundedInnerRectClipper.h"
#include "core/paint/ScrollableAreaPainter.h"
#include "core/paint/TransformRecorder.h"
#include "wtf/Optional.h"

namespace blink {

void PartPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (!m_layoutPart.shouldPaint(paintInfo, paintOffset))
        return;

    LayoutPoint adjustedPaintOffset = paintOffset + m_layoutPart.location();
    LayoutRect borderRect(adjustedPaintOffset, m_layoutPart.size());

    if (m_layoutPart.hasBoxDecorationBackground() && (paintInfo.phase == PaintPhaseForeground || paintInfo.phase == PaintPhaseSelection))
        BoxPainter(m_layoutPart).paintBoxDecorationBackground(paintInfo, adjustedPaintOffset);

    if (paintInfo.phase == PaintPhaseMask) {
        BoxPainter(m_layoutPart).paintMask(paintInfo, adjustedPaintOffset);
        return;
    }

    LayoutRect visualOverflowRect(m_layoutPart.visualOverflowRect());
    visualOverflowRect.moveBy(adjustedPaintOffset);

    if ((paintInfo.phase == PaintPhaseOutline || paintInfo.phase == PaintPhaseSelfOutline) && m_layoutPart.style()->hasOutline())
        ObjectPainter(m_layoutPart).paintOutline(paintInfo, borderRect, visualOverflowRect);

    if (paintInfo.phase != PaintPhaseForeground)
        return;

    {
        Optional<RoundedInnerRectClipper> clipper;
        if (m_layoutPart.style()->hasBorderRadius()) {
            if (borderRect.isEmpty())
                return;

            FloatRoundedRect roundedInnerRect = m_layoutPart.style()->getRoundedInnerBorderFor(borderRect,
                LayoutRectOutsets(
                    -(m_layoutPart.paddingTop() + m_layoutPart.borderTop()),
                    -(m_layoutPart.paddingRight() + m_layoutPart.borderRight()),
                    -(m_layoutPart.paddingBottom() + m_layoutPart.borderBottom()),
                    -(m_layoutPart.paddingLeft() + m_layoutPart.borderLeft())),
                true, true);
            clipper.emplace(m_layoutPart, paintInfo, borderRect, roundedInnerRect, ApplyToDisplayListIfEnabled);
        }

        if (m_layoutPart.widget())
            m_layoutPart.paintContents(paintInfo, paintOffset);
    }

    // Paint a partially transparent wash over selected widgets.
    if (m_layoutPart.isSelected() && !m_layoutPart.document().printing() && !LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutPart, paintInfo.phase)) {
        LayoutRect rect = m_layoutPart.localSelectionRect();
        rect.moveBy(adjustedPaintOffset);
        IntRect selectionRect = pixelSnappedIntRect(rect);
        LayoutObjectDrawingRecorder drawingRecorder(*paintInfo.context, m_layoutPart, paintInfo.phase, selectionRect);
        paintInfo.context->fillRect(selectionRect, m_layoutPart.selectionBackgroundColor());
    }

    if (m_layoutPart.canResize())
        ScrollableAreaPainter(*m_layoutPart.layer()->scrollableArea()).paintResizer(paintInfo.context, roundedIntPoint(adjustedPaintOffset), paintInfo.rect);
}

void PartPainter::paintContents(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    LayoutPoint adjustedPaintOffset = paintOffset + m_layoutPart.location();

    Widget* widget = m_layoutPart.widget();
    RELEASE_ASSERT(widget);

    // Tell the widget to paint now. This is the only time the widget is allowed
    // to paint itself. That way it will composite properly with z-indexed layers.
    IntPoint widgetLocation = widget->frameRect().location();
    IntPoint paintLocation(roundToInt(adjustedPaintOffset.x() + m_layoutPart.borderLeft() + m_layoutPart.paddingLeft()),
        roundToInt(adjustedPaintOffset.y() + m_layoutPart.borderTop() + m_layoutPart.paddingTop()));
    IntRect paintRect = paintInfo.rect;

    IntSize widgetPaintOffset = paintLocation - widgetLocation;
    // When painting widgets into compositing layers, tx and ty are relative to the enclosing compositing layer,
    // not the root. In this case, shift the CTM and adjust the paintRect to be root-relative to fix plugin drawing.
    TransformRecorder transform(*paintInfo.context, m_layoutPart,
        AffineTransform::translation(widgetPaintOffset.width(), widgetPaintOffset.height()));
    paintRect.move(-widgetPaintOffset);
    widget->paint(paintInfo.context, paintRect);
}

} // namespace blink
