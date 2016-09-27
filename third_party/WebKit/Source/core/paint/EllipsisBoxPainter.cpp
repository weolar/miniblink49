// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/EllipsisBoxPainter.h"

#include "core/layout/TextRunConstructor.h"
#include "core/layout/line/EllipsisBox.h"
#include "core/layout/line/RootInlineBox.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/TextPainter.h"
#include "platform/graphics/GraphicsContextStateSaver.h"
#include "platform/graphics/paint/DrawingRecorder.h"

namespace blink {

void EllipsisBoxPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset, LayoutUnit lineTop, LayoutUnit lineBottom)
{
    const ComputedStyle& style = m_ellipsisBox.layoutObject().styleRef(m_ellipsisBox.isFirstLineStyle());
    paintEllipsis(paintInfo, paintOffset, lineTop, lineBottom, style);
}

void EllipsisBoxPainter::paintEllipsis(const PaintInfo& paintInfo, const LayoutPoint& paintOffset, LayoutUnit lineTop, LayoutUnit lineBottom, const ComputedStyle& style)
{
    bool isPrinting = m_ellipsisBox.layoutObject().document().printing();
    bool haveSelection = !isPrinting && paintInfo.phase != PaintPhaseTextClip && m_ellipsisBox.selectionState() != LayoutObject::SelectionNone;

    LayoutRect paintRect(m_ellipsisBox.logicalFrameRect());
    if (haveSelection)
        paintRect.unite(LayoutRect(m_ellipsisBox.selectionRect()));
    m_ellipsisBox.logicalRectToPhysicalRect(paintRect);
    paintRect.moveBy(paintOffset);

    GraphicsContext* context = paintInfo.context;
    DisplayItem::Type displayItemType = DisplayItem::paintPhaseToDrawingType(paintInfo.phase);
    if (DrawingRecorder::useCachedDrawingIfPossible(*context, m_ellipsisBox, displayItemType))
        return;

    DrawingRecorder recorder(*context, m_ellipsisBox, displayItemType, paintRect);

    LayoutPoint boxOrigin = m_ellipsisBox.locationIncludingFlipping();
    boxOrigin.moveBy(paintOffset);
    LayoutRect boxRect(boxOrigin, LayoutSize(m_ellipsisBox.logicalWidth(), m_ellipsisBox.virtualLogicalHeight()));

    GraphicsContextStateSaver stateSaver(*context);
    if (!m_ellipsisBox.isHorizontal())
        context->concatCTM(TextPainter::rotation(boxRect, TextPainter::Clockwise));

    const Font& font = style.font();

    if (haveSelection)
        paintSelection(context, boxOrigin, style, font);
    else if (paintInfo.phase == PaintPhaseSelection)
        return;

    bool usesTextAsClip = paintInfo.phase == PaintPhaseTextClip;
    TextPainter::Style textStyle = TextPainter::textPaintingStyle(m_ellipsisBox.layoutObject(), style, usesTextAsClip, isPrinting);
    if (haveSelection)
        textStyle = TextPainter::selectionPaintingStyle(m_ellipsisBox.layoutObject(), true, usesTextAsClip, isPrinting, textStyle);

    TextRun textRun = constructTextRun(&m_ellipsisBox.layoutObject(), font, m_ellipsisBox.ellipsisStr(), style, TextRun::AllowTrailingExpansion);
    LayoutPoint textOrigin(boxOrigin.x(), boxOrigin.y() + font.fontMetrics().ascent());
    TextPainter textPainter(context, font, textRun, textOrigin, boxRect, m_ellipsisBox.isHorizontal());
    textPainter.paint(0, m_ellipsisBox.ellipsisStr().length(), m_ellipsisBox.ellipsisStr().length(), textStyle);
}

void EllipsisBoxPainter::paintSelection(GraphicsContext* context, const LayoutPoint& boxOrigin, const ComputedStyle& style, const Font& font)
{
    Color textColor = m_ellipsisBox.layoutObject().resolveColor(style, CSSPropertyColor);
    Color c = m_ellipsisBox.layoutObject().selectionBackgroundColor();
    if (!c.alpha())
        return;

    // If the text color ends up being the same as the selection background, invert the selection
    // background.
    if (textColor == c)
        c = Color(0xff - c.red(), 0xff - c.green(), 0xff - c.blue());

    GraphicsContextStateSaver stateSaver(*context);
    LayoutUnit selectionBottom = m_ellipsisBox.root().selectionBottom();
    LayoutUnit top = m_ellipsisBox.root().selectionTop();
    LayoutUnit h = m_ellipsisBox.root().selectionHeight();
    const int deltaY = roundToInt(m_ellipsisBox.layoutObject().styleRef().isFlippedLinesWritingMode() ? selectionBottom - m_ellipsisBox.logicalBottom() : m_ellipsisBox.logicalTop() - top);
    const LayoutPoint localOrigin(boxOrigin.x(), boxOrigin.y() - deltaY);
    LayoutRect clipRect(localOrigin, LayoutSize(m_ellipsisBox.logicalWidth(), h));
    context->clip(clipRect);
    context->drawHighlightForText(font, constructTextRun(&m_ellipsisBox.layoutObject(), font, m_ellipsisBox.ellipsisStr(), style, TextRun::AllowTrailingExpansion), FloatPoint(localOrigin), h, c);
}

} // namespace blink
