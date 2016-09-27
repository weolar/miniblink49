// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/ScrollbarPainter.h"

#include "core/layout/LayoutScrollbar.h"
#include "core/layout/LayoutScrollbarPart.h"
#include "core/paint/BlockPainter.h"
#include "core/paint/PaintInfo.h"
#include "platform/graphics/GraphicsContext.h"
namespace blink {

void ScrollbarPainter::paintPart(GraphicsContext* graphicsContext, ScrollbarPart partType, const IntRect& rect)
{
    LayoutScrollbarPart* partLayoutObject = m_layoutScrollbar.getPart(partType);
    if (!partLayoutObject)
        return;
    paintIntoRect(partLayoutObject, graphicsContext, m_layoutScrollbar.location(), LayoutRect(rect));
}

void ScrollbarPainter::paintIntoRect(LayoutScrollbarPart* layoutScrollbarPart, GraphicsContext* graphicsContext, const LayoutPoint& paintOffset, const LayoutRect& rect)
{
    // Make sure our dimensions match the rect.
    // FIXME: Setting these is a bad layering violation!
    layoutScrollbarPart->setLocation(rect.location() - toSize(paintOffset));
    layoutScrollbarPart->setWidth(rect.width());
    layoutScrollbarPart->setHeight(rect.height());

    // Now do the paint.
    PaintInfo paintInfo(graphicsContext, pixelSnappedIntRect(rect), PaintPhaseBlockBackground, PaintBehaviorNormal);
    BlockPainter blockPainter(*layoutScrollbarPart);
    blockPainter.paint(paintInfo, paintOffset);
    paintInfo.phase = PaintPhaseChildBlockBackgrounds;
    blockPainter.paint(paintInfo, paintOffset);
    paintInfo.phase = PaintPhaseFloat;
    blockPainter.paint(paintInfo, paintOffset);
    paintInfo.phase = PaintPhaseForeground;
    blockPainter.paint(paintInfo, paintOffset);
    paintInfo.phase = PaintPhaseOutline;
    blockPainter.paint(paintInfo, paintOffset);
}

} // namespace blink
