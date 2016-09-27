// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/TablePainter.h"

#include "core/layout/LayoutTable.h"
#include "core/layout/LayoutTableSection.h"
#include "core/style/CollapsedBorderValue.h"
#include "core/paint/BoxClipper.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/ObjectPainter.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/ScopeRecorder.h"

namespace blink {

void TablePainter::paintObject(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    PaintPhase paintPhase = paintInfo.phase;
    if ((paintPhase == PaintPhaseBlockBackground || paintPhase == PaintPhaseChildBlockBackground) && m_layoutTable.hasBoxDecorationBackground() && m_layoutTable.style()->visibility() == VISIBLE)
        paintBoxDecorationBackground(paintInfo, paintOffset);

    if (paintPhase == PaintPhaseMask) {
        paintMask(paintInfo, paintOffset);
        return;
    }

    // We're done. We don't bother painting any children.
    if (paintPhase == PaintPhaseBlockBackground)
        return;

    // We don't paint our own background, but we do let the kids paint their backgrounds.
    if (paintPhase == PaintPhaseChildBlockBackgrounds)
        paintPhase = PaintPhaseChildBlockBackground;

    PaintInfo info(paintInfo);
    info.phase = paintPhase;
    info.updatePaintingRootForChildren(&m_layoutTable);

    for (LayoutObject* child = m_layoutTable.firstChild(); child; child = child->nextSibling()) {
        if (child->isBox() && !toLayoutBox(child)->hasSelfPaintingLayer() && (child->isTableSection() || child->isTableCaption())) {
            LayoutPoint childPoint = m_layoutTable.flipForWritingModeForChild(toLayoutBox(child), paintOffset);
            child->paint(info, childPoint);
        }
    }

    if (m_layoutTable.collapseBorders() && paintPhase == PaintPhaseChildBlockBackground && m_layoutTable.style()->visibility() == VISIBLE) {
        // Using our cached sorted styles, we then do individual passes,
        // painting each style of border from lowest precedence to highest precedence.
        info.phase = PaintPhaseCollapsedTableBorders;
        LayoutTable::CollapsedBorderValues collapsedBorders = m_layoutTable.collapsedBorders();
        size_t count = collapsedBorders.size();
        for (size_t i = 0; i < count; ++i) {
            ScopeRecorder scopeRecorder(*info.context, m_layoutTable);
            // FIXME: pass this value into children rather than storing temporarily on the LayoutTable object.
            m_layoutTable.setCurrentBorderValue(&collapsedBorders[i]);
            for (LayoutTableSection* section = m_layoutTable.bottomSection(); section; section = m_layoutTable.sectionAbove(section)) {
                LayoutPoint childPoint = m_layoutTable.flipForWritingModeForChild(section, paintOffset);
                section->paint(info, childPoint);
            }
        }
        m_layoutTable.setCurrentBorderValue(0);
    }

    // Paint outline.
    if ((paintPhase == PaintPhaseOutline || paintPhase == PaintPhaseSelfOutline) && m_layoutTable.style()->hasOutline() && m_layoutTable.style()->visibility() == VISIBLE) {
        LayoutRect overflowRect(m_layoutTable.visualOverflowRect());
        overflowRect.moveBy(paintOffset);
        ObjectPainter(m_layoutTable).paintOutline(paintInfo, LayoutRect(paintOffset, m_layoutTable.size()), overflowRect);
    }
}

void TablePainter::paintBoxDecorationBackground(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (!paintInfo.shouldPaintWithinRoot(&m_layoutTable))
        return;

    LayoutRect rect(paintOffset, m_layoutTable.size());
    m_layoutTable.subtractCaptionRect(rect);
    BoxPainter(m_layoutTable).paintBoxDecorationBackgroundWithRect(paintInfo, paintOffset, rect);
}

void TablePainter::paintMask(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (m_layoutTable.style()->visibility() != VISIBLE || paintInfo.phase != PaintPhaseMask)
        return;

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutTable, paintInfo.phase))
        return;

    LayoutRect rect(paintOffset, m_layoutTable.size());
    m_layoutTable.subtractCaptionRect(rect);
    LayoutObjectDrawingRecorder recorder(*paintInfo.context, m_layoutTable, paintInfo.phase, pixelSnappedIntRect(rect));
    BoxPainter(m_layoutTable).paintMaskImages(paintInfo, rect);
}

} // namespace blink
