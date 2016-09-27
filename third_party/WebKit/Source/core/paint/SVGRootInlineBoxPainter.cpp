// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/SVGRootInlineBoxPainter.h"

#include "core/layout/svg/line/SVGInlineFlowBox.h"
#include "core/layout/svg/line/SVGInlineTextBox.h"
#include "core/layout/svg/line/SVGRootInlineBox.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/SVGInlineFlowBoxPainter.h"
#include "core/paint/SVGInlineTextBoxPainter.h"
#include "core/paint/SVGPaintContext.h"

namespace blink {

void SVGRootInlineBoxPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ASSERT(paintInfo.phase == PaintPhaseForeground || paintInfo.phase == PaintPhaseSelection);

    bool isPrinting = m_svgRootInlineBox.layoutObject().document().printing();
    bool hasSelection = !isPrinting && m_svgRootInlineBox.selectionState() != LayoutObject::SelectionNone;

    PaintInfo paintInfoBeforeFiltering(paintInfo);
    if (hasSelection && !LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfoBeforeFiltering.context, m_svgRootInlineBox.layoutObject(), paintInfoBeforeFiltering.phase)) {
        LayoutObjectDrawingRecorder recorder(*paintInfoBeforeFiltering.context, m_svgRootInlineBox.layoutObject(), paintInfoBeforeFiltering.phase, paintInfoBeforeFiltering.rect);
        for (InlineBox* child = m_svgRootInlineBox.firstChild(); child; child = child->nextOnLine()) {
            if (child->isSVGInlineTextBox())
                SVGInlineTextBoxPainter(*toSVGInlineTextBox(child)).paintSelectionBackground(paintInfoBeforeFiltering);
            else if (child->isSVGInlineFlowBox())
                SVGInlineFlowBoxPainter(*toSVGInlineFlowBox(child)).paintSelectionBackground(paintInfoBeforeFiltering);
        }
    }

    SVGPaintContext paintContext(m_svgRootInlineBox.layoutObject(), paintInfoBeforeFiltering);
    if (paintContext.applyClipMaskAndFilterIfNecessary()) {
        for (InlineBox* child = m_svgRootInlineBox.firstChild(); child; child = child->nextOnLine())
            child->paint(paintContext.paintInfo(), paintOffset, 0, 0);
    }
}

} // namespace blink
