// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/DetailsMarkerPainter.h"

#include "core/layout/LayoutDetailsMarker.h"
#include "core/paint/BlockPainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/geometry/LayoutPoint.h"
#include "platform/graphics/Path.h"

namespace blink {

void DetailsMarkerPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (paintInfo.phase != PaintPhaseForeground || m_layoutDetailsMarker.style()->visibility() != VISIBLE) {
        BlockPainter(m_layoutDetailsMarker).paint(paintInfo, paintOffset);
        return;
    }

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutDetailsMarker, paintInfo.phase))
        return;

    LayoutPoint boxOrigin(paintOffset + m_layoutDetailsMarker.location());
    LayoutRect overflowRect(m_layoutDetailsMarker.visualOverflowRect());
    overflowRect.moveBy(boxOrigin);

    if (!paintInfo.rect.intersects(pixelSnappedIntRect(overflowRect)))
        return;

    LayoutObjectDrawingRecorder layoutDrawingRecorder(*paintInfo.context, m_layoutDetailsMarker, paintInfo.phase, overflowRect);
    const Color color(m_layoutDetailsMarker.resolveColor(CSSPropertyColor));
    paintInfo.context->setStrokeColor(color);
    paintInfo.context->setStrokeStyle(SolidStroke);
    paintInfo.context->setStrokeThickness(1.0f);
    paintInfo.context->setFillColor(color);

    boxOrigin.move(m_layoutDetailsMarker.borderLeft() + m_layoutDetailsMarker.paddingLeft(), m_layoutDetailsMarker.borderTop() + m_layoutDetailsMarker.paddingTop());
    paintInfo.context->fillPath(getPath(boxOrigin));
}

static Path createPath(const FloatPoint* path)
{
    Path result;
    result.moveTo(FloatPoint(path[0].x(), path[0].y()));
    for (int i = 1; i < 4; ++i)
        result.addLineTo(FloatPoint(path[i].x(), path[i].y()));
    return result;
}

static Path createDownArrowPath()
{
    FloatPoint points[4] = { FloatPoint(0.0f, 0.07f), FloatPoint(0.5f, 0.93f), FloatPoint(1.0f, 0.07f), FloatPoint(0.0f, 0.07f) };
    return createPath(points);
}

static Path createUpArrowPath()
{
    FloatPoint points[4] = { FloatPoint(0.0f, 0.93f), FloatPoint(0.5f, 0.07f), FloatPoint(1.0f, 0.93f), FloatPoint(0.0f, 0.93f) };
    return createPath(points);
}

static Path createLeftArrowPath()
{
    FloatPoint points[4] = { FloatPoint(1.0f, 0.0f), FloatPoint(0.14f, 0.5f), FloatPoint(1.0f, 1.0f), FloatPoint(1.0f, 0.0f) };
    return createPath(points);
}

static Path createRightArrowPath()
{
    FloatPoint points[4] = { FloatPoint(0.0f, 0.0f), FloatPoint(0.86f, 0.5f), FloatPoint(0.0f, 1.0f), FloatPoint(0.0f, 0.0f) };
    return createPath(points);
}

Path DetailsMarkerPainter::getCanonicalPath() const
{
    switch (m_layoutDetailsMarker.orientation()) {
    case LayoutDetailsMarker::Left: return createLeftArrowPath();
    case LayoutDetailsMarker::Right: return createRightArrowPath();
    case LayoutDetailsMarker::Up: return createUpArrowPath();
    case LayoutDetailsMarker::Down: return createDownArrowPath();
    }

    return Path();
}

Path DetailsMarkerPainter::getPath(const LayoutPoint& origin) const
{
    Path result = getCanonicalPath();
    result.transform(AffineTransform().scale(m_layoutDetailsMarker.contentWidth().toFloat(), m_layoutDetailsMarker.contentHeight().toFloat()));
    result.translate(FloatSize(origin.x().toFloat(), origin.y().toFloat()));
    return result;
}

} // namespace paint
