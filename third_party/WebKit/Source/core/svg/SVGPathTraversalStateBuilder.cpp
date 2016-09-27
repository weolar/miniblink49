/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/svg/SVGPathTraversalStateBuilder.h"

#include "core/svg/SVGPathSeg.h"
#include "platform/graphics/PathTraversalState.h"

namespace blink {

SVGPathTraversalStateBuilder::SVGPathTraversalStateBuilder(PathTraversalState::PathTraversalAction traversalAction, float desiredLength)
    : m_traversalState(traversalAction)
    , m_segmentIndex(0)
{
    m_traversalState.m_desiredLength = desiredLength;
}

void SVGPathTraversalStateBuilder::emitSegment(const PathSegmentData& segment)
{
    switch (segment.command) {
    case PathSegMoveToAbs:
        m_traversalState.m_totalLength += m_traversalState.moveTo(segment.targetPoint);
        break;
    case PathSegLineToAbs:
        m_traversalState.m_totalLength += m_traversalState.lineTo(segment.targetPoint);
        break;
    case PathSegClosePath:
        m_traversalState.m_totalLength += m_traversalState.closeSubpath();
        break;
    case PathSegCurveToCubicAbs:
        m_traversalState.m_totalLength += m_traversalState.cubicBezierTo(segment.point1, segment.point2, segment.targetPoint);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

bool SVGPathTraversalStateBuilder::continueConsuming()
{
    m_traversalState.processSegment();
    return !m_traversalState.m_success;
}

void SVGPathTraversalStateBuilder::incrementPathSegmentCount()
{
    ++m_segmentIndex;
}

float SVGPathTraversalStateBuilder::totalLength()
{
    return m_traversalState.m_totalLength;
}

FloatPoint SVGPathTraversalStateBuilder::currentPoint()
{
    return m_traversalState.m_current;
}

}
