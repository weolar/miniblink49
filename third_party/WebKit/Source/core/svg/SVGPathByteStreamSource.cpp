/*
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
#include "core/svg/SVGPathByteStreamSource.h"

namespace blink {

bool SVGPathByteStreamSource::hasMoreData() const
{
    return m_streamCurrent < m_streamEnd;
}

SVGPathSegType SVGPathByteStreamSource::peekSegmentType()
{
    ASSERT(hasMoreData());
    ASSERT(m_streamCurrent + sizeof(unsigned short) <= m_streamEnd);
    unsigned short commandBytes;
    memcpy(&commandBytes, m_streamCurrent, sizeof(commandBytes));
    return static_cast<SVGPathSegType>(commandBytes);
}

PathSegmentData SVGPathByteStreamSource::parseSegment()
{
    ASSERT(hasMoreData());
    PathSegmentData segment;
    segment.command = static_cast<SVGPathSegType>(readSVGSegmentType());

    switch (segment.command) {
    case PathSegCurveToCubicRel:
    case PathSegCurveToCubicAbs:
        segment.point1 = readFloatPoint();
        /* fall through */
    case PathSegCurveToCubicSmoothRel:
    case PathSegCurveToCubicSmoothAbs:
        segment.point2 = readFloatPoint();
        /* fall through */
    case PathSegMoveToRel:
    case PathSegMoveToAbs:
    case PathSegLineToRel:
    case PathSegLineToAbs:
    case PathSegCurveToQuadraticSmoothRel:
    case PathSegCurveToQuadraticSmoothAbs:
        segment.targetPoint = readFloatPoint();
        break;
    case PathSegLineToHorizontalRel:
    case PathSegLineToHorizontalAbs:
        segment.targetPoint.setX(readFloat());
        break;
    case PathSegLineToVerticalRel:
    case PathSegLineToVerticalAbs:
        segment.targetPoint.setY(readFloat());
        break;
    case PathSegClosePath:
        break;
    case PathSegCurveToQuadraticRel:
    case PathSegCurveToQuadraticAbs:
        segment.point1 = readFloatPoint();
        segment.targetPoint = readFloatPoint();
        break;
    case PathSegArcRel:
    case PathSegArcAbs: {
        segment.point1 = readFloatPoint(); // rx and ry
        segment.point2.setX(readFloat()); // angle
        segment.arcLarge = readFlag();
        segment.arcSweep = readFlag();
        segment.targetPoint = readFloatPoint();
        break;
    }
    default:
        ASSERT_NOT_REACHED();
    }
    return segment;
}

}
