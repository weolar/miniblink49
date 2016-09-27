// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/SVGInlineFlowBoxPainter.h"

#include "core/layout/LayoutInline.h"
#include "core/layout/svg/line/SVGInlineFlowBox.h"
#include "core/layout/svg/line/SVGInlineTextBox.h"
#include "core/paint/InlinePainter.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/SVGInlineTextBoxPainter.h"
#include "core/paint/SVGPaintContext.h"

namespace blink {

void SVGInlineFlowBoxPainter::paintSelectionBackground(const PaintInfo& paintInfo)
{
    ASSERT(paintInfo.phase == PaintPhaseForeground || paintInfo.phase == PaintPhaseSelection);

    PaintInfo childPaintInfo(paintInfo);
    for (InlineBox* child = m_svgInlineFlowBox.firstChild(); child; child = child->nextOnLine()) {
        if (child->isSVGInlineTextBox())
            SVGInlineTextBoxPainter(*toSVGInlineTextBox(child)).paintSelectionBackground(childPaintInfo);
        else if (child->isSVGInlineFlowBox())
            SVGInlineFlowBoxPainter(*toSVGInlineFlowBox(child)).paintSelectionBackground(childPaintInfo);
    }
}

void SVGInlineFlowBoxPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ASSERT(paintInfo.phase == PaintPhaseForeground || paintInfo.phase == PaintPhaseSelection);

    {
        SVGPaintContext paintContext(m_svgInlineFlowBox.layoutObject(), paintInfo);
        if (paintContext.applyClipMaskAndFilterIfNecessary()) {
            for (InlineBox* child = m_svgInlineFlowBox.firstChild(); child; child = child->nextOnLine())
                child->paint(paintContext.paintInfo(), paintOffset, 0, 0);
        }
    }

    if (m_svgInlineFlowBox.layoutObject().isLayoutInline())
        InlinePainter(toLayoutInline(m_svgInlineFlowBox.layoutObject())).paintOutline(paintInfo, paintOffset);
}

} // namespace blink
