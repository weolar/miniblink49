// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/BlockFlowPainter.h"

#include "core/layout/FloatingObjects.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/paint/ClipScope.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"

namespace blink {

void BlockFlowPainter::paintFloats(const PaintInfo& paintInfo, const LayoutPoint& paintOffset, bool preservePhase)
{
    if (!m_layoutBlockFlow.floatingObjects())
        return;

    const FloatingObjectSet& floatingObjectSet = m_layoutBlockFlow.floatingObjects()->set();
    FloatingObjectSetIterator end = floatingObjectSet.end();
    for (FloatingObjectSetIterator it = floatingObjectSet.begin(); it != end; ++it) {
        const FloatingObject& floatingObject = *it->get();
        // Only paint the object if our m_shouldPaint flag is set.
        if (floatingObject.shouldPaint() && !floatingObject.layoutObject()->hasSelfPaintingLayer()) {
            PaintInfo currentPaintInfo(paintInfo);
            currentPaintInfo.phase = preservePhase ? paintInfo.phase : PaintPhaseBlockBackground;
            // FIXME: LayoutPoint version of xPositionForFloatIncludingMargin would make this much cleaner.
            LayoutPoint childPoint = m_layoutBlockFlow.flipFloatForWritingModeForChild(
                floatingObject, LayoutPoint(paintOffset.x()
                + m_layoutBlockFlow.xPositionForFloatIncludingMargin(floatingObject) - floatingObject.layoutObject()->location().x(), paintOffset.y()
                + m_layoutBlockFlow.yPositionForFloatIncludingMargin(floatingObject) - floatingObject.layoutObject()->location().y()));
            floatingObject.layoutObject()->paint(currentPaintInfo, childPoint);
            if (!preservePhase) {
                currentPaintInfo.phase = PaintPhaseChildBlockBackgrounds;
                floatingObject.layoutObject()->paint(currentPaintInfo, childPoint);
                currentPaintInfo.phase = PaintPhaseFloat;
                floatingObject.layoutObject()->paint(currentPaintInfo, childPoint);
                currentPaintInfo.phase = PaintPhaseForeground;
                floatingObject.layoutObject()->paint(currentPaintInfo, childPoint);
                currentPaintInfo.phase = PaintPhaseOutline;
                floatingObject.layoutObject()->paint(currentPaintInfo, childPoint);
            }
        }
    }
}

void BlockFlowPainter::paintSelection(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ASSERT(paintInfo.phase == PaintPhaseForeground);
    if (!m_layoutBlockFlow.shouldPaintSelectionGaps())
        return;

    LayoutUnit lastTop = 0;
    LayoutUnit lastLeft = m_layoutBlockFlow.logicalLeftSelectionOffset(&m_layoutBlockFlow, lastTop);
    LayoutUnit lastRight = m_layoutBlockFlow.logicalRightSelectionOffset(&m_layoutBlockFlow, lastTop);

    LayoutRect bounds;
    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        bounds = m_layoutBlockFlow.visualOverflowRect();
        bounds.moveBy(paintOffset);
    }

    // Only create a DrawingRecorder and ClipScope if skipRecording is false. This logic is needed
    // because selectionGaps(...) needs to be called even when we do not record.
    bool skipRecording = LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutBlockFlow, DisplayItem::SelectionGap);
    Optional<DrawingRecorder> drawingRecorder;
    Optional<ClipScope> clipScope;
    if (!skipRecording) {
        drawingRecorder.emplace(*paintInfo.context, m_layoutBlockFlow, DisplayItem::SelectionGap, bounds);
        clipScope.emplace(paintInfo.context);
    }

    LayoutRect gapRectsBounds = m_layoutBlockFlow.selectionGaps(&m_layoutBlockFlow, paintOffset, LayoutSize(), lastTop, lastLeft, lastRight,
        skipRecording ? nullptr : &paintInfo,
        skipRecording ? nullptr : &(*clipScope));
    // TODO(wkorman): Rework below to process paint invalidation rects during layout rather than paint.
    if (!gapRectsBounds.isEmpty()) {
        DeprecatedPaintLayer* layer = m_layoutBlockFlow.enclosingLayer();
        gapRectsBounds.moveBy(-paintOffset);
        if (!m_layoutBlockFlow.hasLayer()) {
            LayoutRect localBounds(gapRectsBounds);
            m_layoutBlockFlow.flipForWritingMode(localBounds);
            gapRectsBounds = LayoutRect(m_layoutBlockFlow.localToContainerQuad(FloatRect(localBounds), layer->layoutObject()).enclosingBoundingBox());
            if (layer->layoutObject()->hasOverflowClip())
                gapRectsBounds.move(layer->layoutBox()->scrolledContentOffset());
        }
        layer->addBlockSelectionGapsBounds(gapRectsBounds);
    }
}

} // namespace blink
