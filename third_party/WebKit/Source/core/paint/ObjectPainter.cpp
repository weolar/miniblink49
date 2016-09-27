// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/ObjectPainter.h"

#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutTheme.h"
#include "core/paint/BoxBorderPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "core/style/BorderEdge.h"
#include "core/style/ComputedStyle.h"
#include "platform/geometry/LayoutPoint.h"

namespace blink {

LayoutRect ObjectPainter::outlineBounds(const LayoutRect& objectBounds, const ComputedStyle& style)
{
    LayoutRect outlineBounds(objectBounds);
    outlineBounds.inflate(style.outlineOutset());
    return outlineBounds;
}

void ObjectPainter::paintFocusRing(const PaintInfo& paintInfo, const ComputedStyle& style, const Vector<LayoutRect>& focusRingRects)
{
    ASSERT(style.outlineStyleIsAuto());
    Vector<IntRect> focusRingIntRects;
    for (size_t i = 0; i < focusRingRects.size(); ++i)
        focusRingIntRects.append(pixelSnappedIntRect(focusRingRects[i]));
    paintInfo.context->drawFocusRing(focusRingIntRects, style.outlineWidth(), style.outlineOffset(), m_layoutObject.resolveColor(style, CSSPropertyOutlineColor));
}

void ObjectPainter::paintOutline(const PaintInfo& paintInfo, const LayoutRect& objectBounds, const LayoutRect& visualOverflowBounds)
{
    const ComputedStyle& styleToUse = m_layoutObject.styleRef();
    if (!styleToUse.hasOutline())
        return;

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutObject, paintInfo.phase))
        return;

    LayoutObjectDrawingRecorder recorder(*paintInfo.context, m_layoutObject, paintInfo.phase, visualOverflowBounds);

    if (styleToUse.outlineStyleIsAuto()) {
        if (LayoutTheme::theme().shouldDrawDefaultFocusRing(&m_layoutObject)) {
            // Only paint the focus ring by hand if the theme isn't able to draw the focus ring.
            Vector<LayoutRect> focusRingRects;
            m_layoutObject.addFocusRingRects(focusRingRects, objectBounds.location());
            paintFocusRing(paintInfo, styleToUse, focusRingRects);
        }
        return;
    }

    if (styleToUse.outlineStyle() == BNONE)
        return;

    LayoutRect inner(pixelSnappedIntRect(objectBounds));
    inner.inflate(styleToUse.outlineOffset());

    LayoutRect outer(inner);
    int outlineWidth = styleToUse.outlineWidth();
    outer.inflate(outlineWidth);

    const BorderEdge commonEdgeInfo(outlineWidth,
        m_layoutObject.resolveColor(styleToUse, CSSPropertyOutlineColor), styleToUse.outlineStyle());
    BoxBorderPainter(styleToUse, outer, inner, commonEdgeInfo).paintBorder(paintInfo, outer);
}

void ObjectPainter::addPDFURLRectIfNeeded(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ASSERT(paintInfo.context->printing());
    if (m_layoutObject.isElementContinuation() || !m_layoutObject.node() || !m_layoutObject.node()->isLink() || m_layoutObject.styleRef().visibility() != VISIBLE)
        return;

    KURL url = toElement(m_layoutObject.node())->hrefURL();
    if (!url.isValid())
        return;

    Vector<LayoutRect> focusRingRects;
    m_layoutObject.addFocusRingRects(focusRingRects, paintOffset);
    IntRect rect = pixelSnappedIntRect(unionRect(focusRingRects));
    if (rect.isEmpty())
        return;

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutObject, DisplayItem::PrintedContentPDFURLRect))
        return;

    LayoutObjectDrawingRecorder recorder(*paintInfo.context, m_layoutObject, DisplayItem::PrintedContentPDFURLRect, rect);
    if (url.hasFragmentIdentifier() && equalIgnoringFragmentIdentifier(url, m_layoutObject.document().baseURL())) {
        String fragmentName = url.fragmentIdentifier();
        if (m_layoutObject.document().findAnchor(fragmentName))
            paintInfo.context->setURLFragmentForRect(fragmentName, rect);
        return;
    }
    paintInfo.context->setURLForRect(url, rect);
}

void ObjectPainter::drawLineForBoxSide(GraphicsContext* graphicsContext, int x1, int y1, int x2, int y2,
    BoxSide side, Color color, EBorderStyle style,
    int adjacentWidth1, int adjacentWidth2, bool antialias)
{
    int thickness;
    int length;
    if (side == BSTop || side == BSBottom) {
        thickness = y2 - y1;
        length = x2 - x1;
    } else {
        thickness = x2 - x1;
        length = y2 - y1;
    }

    // FIXME: We really would like this check to be an ASSERT as we don't want to draw empty borders. However
    // nothing guarantees that the following recursive calls to drawLineForBoxSide will have non-null dimensions.
    if (!thickness || !length)
        return;

    if (style == DOUBLE && thickness < 3)
        style = SOLID;

    switch (style) {
    case BNONE:
    case BHIDDEN:
        return;
    case DOTTED:
    case DASHED:
        drawDashedOrDottedBoxSide(graphicsContext, x1, y1, x2, y2, side,
            color, thickness, style, antialias);
        break;
    case DOUBLE:
        drawDoubleBoxSide(graphicsContext, x1, y1, x2, y2, length, side, color,
            thickness, adjacentWidth1, adjacentWidth2, antialias);
        break;
    case RIDGE:
    case GROOVE:
        drawRidgeOrGrooveBoxSide(graphicsContext, x1, y1, x2, y2, side, color,
            style, adjacentWidth1, adjacentWidth2, antialias);
        break;
    case INSET:
        // FIXME: Maybe we should lighten the colors on one side like Firefox.
        // https://bugs.webkit.org/show_bug.cgi?id=58608
        if (side == BSTop || side == BSLeft)
            color = color.dark();
        // fall through
    case OUTSET:
        if (style == OUTSET && (side == BSBottom || side == BSRight))
            color = color.dark();
        // fall through
    case SOLID:
        drawSolidBoxSide(graphicsContext, x1, y1, x2, y2, side, color, adjacentWidth1, adjacentWidth2, antialias);
        break;
    }
}

void ObjectPainter::drawDashedOrDottedBoxSide(GraphicsContext* graphicsContext, int x1, int y1, int x2, int y2,
    BoxSide side, Color color, int thickness, EBorderStyle style, bool antialias)
{
    if (thickness <= 0)
        return;

    bool wasAntialiased = graphicsContext->shouldAntialias();
    StrokeStyle oldStrokeStyle = graphicsContext->strokeStyle();
    graphicsContext->setShouldAntialias(antialias);
    graphicsContext->setStrokeColor(color);
    graphicsContext->setStrokeThickness(thickness);
    graphicsContext->setStrokeStyle(style == DASHED ? DashedStroke : DottedStroke);

    switch (side) {
    case BSBottom:
    case BSTop:
        graphicsContext->drawLine(IntPoint(x1, (y1 + y2) / 2), IntPoint(x2, (y1 + y2) / 2));
        break;
    case BSRight:
    case BSLeft:
        graphicsContext->drawLine(IntPoint((x1 + x2) / 2, y1), IntPoint((x1 + x2) / 2, y2));
        break;
    }
    graphicsContext->setShouldAntialias(wasAntialiased);
    graphicsContext->setStrokeStyle(oldStrokeStyle);
}

void ObjectPainter::drawDoubleBoxSide(GraphicsContext* graphicsContext, int x1, int y1, int x2, int y2,
    int length, BoxSide side, Color color, int thickness, int adjacentWidth1, int adjacentWidth2, bool antialias)
{
    int thirdOfThickness = (thickness + 1) / 3;
    ASSERT(thirdOfThickness);

    if (!adjacentWidth1 && !adjacentWidth2) {
        StrokeStyle oldStrokeStyle = graphicsContext->strokeStyle();
        graphicsContext->setStrokeStyle(NoStroke);
        graphicsContext->setFillColor(color);

        bool wasAntialiased = graphicsContext->shouldAntialias();
        graphicsContext->setShouldAntialias(antialias);

        switch (side) {
        case BSTop:
        case BSBottom:
            graphicsContext->drawRect(IntRect(x1, y1, length, thirdOfThickness));
            graphicsContext->drawRect(IntRect(x1, y2 - thirdOfThickness, length, thirdOfThickness));
            break;
        case BSLeft:
        case BSRight:
            graphicsContext->drawRect(IntRect(x1, y1, thirdOfThickness, length));
            graphicsContext->drawRect(IntRect(x2 - thirdOfThickness, y1, thirdOfThickness, length));
            break;
        }

        graphicsContext->setShouldAntialias(wasAntialiased);
        graphicsContext->setStrokeStyle(oldStrokeStyle);
        return;
    }

    int adjacent1BigThird = ((adjacentWidth1 > 0) ? adjacentWidth1 + 1 : adjacentWidth1 - 1) / 3;
    int adjacent2BigThird = ((adjacentWidth2 > 0) ? adjacentWidth2 + 1 : adjacentWidth2 - 1) / 3;

    switch (side) {
    case BSTop:
        drawLineForBoxSide(graphicsContext, x1 + std::max((-adjacentWidth1 * 2 + 1) / 3, 0),
            y1, x2 - std::max((-adjacentWidth2 * 2 + 1) / 3, 0), y1 + thirdOfThickness,
            side, color, SOLID, adjacent1BigThird, adjacent2BigThird, antialias);
        drawLineForBoxSide(graphicsContext, x1 + std::max((adjacentWidth1 * 2 + 1) / 3, 0),
            y2 - thirdOfThickness, x2 - std::max((adjacentWidth2 * 2 + 1) / 3, 0), y2,
            side, color, SOLID, adjacent1BigThird, adjacent2BigThird, antialias);
        break;
    case BSLeft:
        drawLineForBoxSide(graphicsContext, x1, y1 + std::max((-adjacentWidth1 * 2 + 1) / 3, 0),
            x1 + thirdOfThickness, y2 - std::max((-adjacentWidth2 * 2 + 1) / 3, 0),
            side, color, SOLID, adjacent1BigThird, adjacent2BigThird, antialias);
        drawLineForBoxSide(graphicsContext, x2 - thirdOfThickness, y1 + std::max((adjacentWidth1 * 2 + 1) / 3, 0),
            x2, y2 - std::max((adjacentWidth2 * 2 + 1) / 3, 0),
            side, color, SOLID, adjacent1BigThird, adjacent2BigThird, antialias);
        break;
    case BSBottom:
        drawLineForBoxSide(graphicsContext, x1 + std::max((adjacentWidth1 * 2 + 1) / 3, 0),
            y1, x2 - std::max((adjacentWidth2 * 2 + 1) / 3, 0), y1 + thirdOfThickness,
            side, color, SOLID, adjacent1BigThird, adjacent2BigThird, antialias);
        drawLineForBoxSide(graphicsContext, x1 + std::max((-adjacentWidth1 * 2 + 1) / 3, 0),
            y2 - thirdOfThickness, x2 - std::max((-adjacentWidth2 * 2 + 1) / 3, 0), y2,
            side, color, SOLID, adjacent1BigThird, adjacent2BigThird, antialias);
        break;
    case BSRight:
        drawLineForBoxSide(graphicsContext, x1, y1 + std::max((adjacentWidth1 * 2 + 1) / 3, 0),
            x1 + thirdOfThickness, y2 - std::max((adjacentWidth2 * 2 + 1) / 3, 0),
            side, color, SOLID, adjacent1BigThird, adjacent2BigThird, antialias);
        drawLineForBoxSide(graphicsContext, x2 - thirdOfThickness, y1 + std::max((-adjacentWidth1 * 2 + 1) / 3, 0),
            x2, y2 - std::max((-adjacentWidth2 * 2 + 1) / 3, 0),
            side, color, SOLID, adjacent1BigThird, adjacent2BigThird, antialias);
        break;
    default:
        break;
    }
}

void ObjectPainter::drawRidgeOrGrooveBoxSide(GraphicsContext* graphicsContext, int x1, int y1, int x2, int y2,
    BoxSide side, Color color, EBorderStyle style, int adjacentWidth1, int adjacentWidth2, bool antialias)
{
    EBorderStyle s1;
    EBorderStyle s2;
    if (style == GROOVE) {
        s1 = INSET;
        s2 = OUTSET;
    } else {
        s1 = OUTSET;
        s2 = INSET;
    }

    int adjacent1BigHalf = ((adjacentWidth1 > 0) ? adjacentWidth1 + 1 : adjacentWidth1 - 1) / 2;
    int adjacent2BigHalf = ((adjacentWidth2 > 0) ? adjacentWidth2 + 1 : adjacentWidth2 - 1) / 2;

    switch (side) {
    case BSTop:
        drawLineForBoxSide(graphicsContext, x1 + std::max(-adjacentWidth1, 0) / 2, y1, x2 - std::max(-adjacentWidth2, 0) / 2, (y1 + y2 + 1) / 2,
            side, color, s1, adjacent1BigHalf, adjacent2BigHalf, antialias);
        drawLineForBoxSide(graphicsContext, x1 + std::max(adjacentWidth1 + 1, 0) / 2, (y1 + y2 + 1) / 2, x2 - std::max(adjacentWidth2 + 1, 0) / 2, y2,
            side, color, s2, adjacentWidth1 / 2, adjacentWidth2 / 2, antialias);
        break;
    case BSLeft:
        drawLineForBoxSide(graphicsContext, x1, y1 + std::max(-adjacentWidth1, 0) / 2, (x1 + x2 + 1) / 2, y2 - std::max(-adjacentWidth2, 0) / 2,
            side, color, s1, adjacent1BigHalf, adjacent2BigHalf, antialias);
        drawLineForBoxSide(graphicsContext, (x1 + x2 + 1) / 2, y1 + std::max(adjacentWidth1 + 1, 0) / 2, x2, y2 - std::max(adjacentWidth2 + 1, 0) / 2,
            side, color, s2, adjacentWidth1 / 2, adjacentWidth2 / 2, antialias);
        break;
    case BSBottom:
        drawLineForBoxSide(graphicsContext, x1 + std::max(adjacentWidth1, 0) / 2, y1, x2 - std::max(adjacentWidth2, 0) / 2, (y1 + y2 + 1) / 2,
            side, color, s2, adjacent1BigHalf, adjacent2BigHalf, antialias);
        drawLineForBoxSide(graphicsContext, x1 + std::max(-adjacentWidth1 + 1, 0) / 2, (y1 + y2 + 1) / 2, x2 - std::max(-adjacentWidth2 + 1, 0) / 2, y2,
            side, color, s1, adjacentWidth1 / 2, adjacentWidth2 / 2, antialias);
        break;
    case BSRight:
        drawLineForBoxSide(graphicsContext, x1, y1 + std::max(adjacentWidth1, 0) / 2, (x1 + x2 + 1) / 2, y2 - std::max(adjacentWidth2, 0) / 2,
            side, color, s2, adjacent1BigHalf, adjacent2BigHalf, antialias);
        drawLineForBoxSide(graphicsContext, (x1 + x2 + 1) / 2, y1 + std::max(-adjacentWidth1 + 1, 0) / 2, x2, y2 - std::max(-adjacentWidth2 + 1, 0) / 2,
            side, color, s1, adjacentWidth1 / 2, adjacentWidth2 / 2, antialias);
        break;
    }
}

void ObjectPainter::drawSolidBoxSide(GraphicsContext* graphicsContext, int x1, int y1, int x2, int y2,
    BoxSide side, Color color, int adjacentWidth1, int adjacentWidth2, bool antialias)
{
    ASSERT(x2 >= x1);
    ASSERT(y2 >= y1);

    if (!adjacentWidth1 && !adjacentWidth2) {
        // Tweak antialiasing to match the behavior of fillPolygon();
        // this matters for rects in transformed contexts.
        bool wasAntialiased = graphicsContext->shouldAntialias();
        if (antialias != wasAntialiased)
            graphicsContext->setShouldAntialias(antialias);
        graphicsContext->fillRect(IntRect(x1, y1, x2 - x1, y2 - y1), color);
        if (antialias != wasAntialiased)
            graphicsContext->setShouldAntialias(wasAntialiased);
        return;
    }

    FloatPoint quad[4];
    switch (side) {
    case BSTop:
        quad[0] = FloatPoint(x1 + std::max(-adjacentWidth1, 0), y1);
        quad[1] = FloatPoint(x1 + std::max(adjacentWidth1, 0), y2);
        quad[2] = FloatPoint(x2 - std::max(adjacentWidth2, 0), y2);
        quad[3] = FloatPoint(x2 - std::max(-adjacentWidth2, 0), y1);
        break;
    case BSBottom:
        quad[0] = FloatPoint(x1 + std::max(adjacentWidth1, 0), y1);
        quad[1] = FloatPoint(x1 + std::max(-adjacentWidth1, 0), y2);
        quad[2] = FloatPoint(x2 - std::max(-adjacentWidth2, 0), y2);
        quad[3] = FloatPoint(x2 - std::max(adjacentWidth2, 0), y1);
        break;
    case BSLeft:
        quad[0] = FloatPoint(x1, y1 + std::max(-adjacentWidth1, 0));
        quad[1] = FloatPoint(x1, y2 - std::max(-adjacentWidth2, 0));
        quad[2] = FloatPoint(x2, y2 - std::max(adjacentWidth2, 0));
        quad[3] = FloatPoint(x2, y1 + std::max(adjacentWidth1, 0));
        break;
    case BSRight:
        quad[0] = FloatPoint(x1, y1 + std::max(adjacentWidth1, 0));
        quad[1] = FloatPoint(x1, y2 - std::max(adjacentWidth2, 0));
        quad[2] = FloatPoint(x2, y2 - std::max(-adjacentWidth2, 0));
        quad[3] = FloatPoint(x2, y1 + std::max(-adjacentWidth1, 0));
        break;
    }

    graphicsContext->fillPolygon(4, quad, color, antialias);
}

} // namespace blink
