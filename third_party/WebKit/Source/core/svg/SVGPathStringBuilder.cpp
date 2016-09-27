/*
 * Copyright (C) Research In Motion Limited 2010-2011. All rights reserved.
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
#include "core/svg/SVGPathStringBuilder.h"

#include "core/svg/SVGPathSeg.h"
#include "wtf/text/WTFString.h"

namespace blink {

String SVGPathStringBuilder::result()
{
    unsigned size = m_stringBuilder.length();
    if (!size)
        return String();

    // Remove trailing space.
    m_stringBuilder.resize(size - 1);
    return m_stringBuilder.toString();
}

static void appendFloat(StringBuilder& stringBuilder, float value)
{
    stringBuilder.append(' ');
    stringBuilder.appendNumber(value);
}

static void appendBool(StringBuilder& stringBuilder, bool value)
{
    stringBuilder.append(' ');
    stringBuilder.appendNumber(value);
}

static void appendPoint(StringBuilder& stringBuilder, const FloatPoint& point)
{
    appendFloat(stringBuilder, point.x());
    appendFloat(stringBuilder, point.y());
}

// TODO(fs): Centralized location for this (SVGPathSeg.h?)
static const char pathSegmentCharacter[] = {
    0, // PathSegUnknown
    'Z', // PathSegClosePath
    'M', // PathSegMoveToAbs
    'm', // PathSegMoveToRel
    'L', // PathSegLineToAbs
    'l', // PathSegLineToRel
    'C', // PathSegCurveToCubicAbs
    'c', // PathSegCurveToCubicRel
    'Q', // PathSegCurveToQuadraticAbs
    'q', // PathSegCurveToQuadraticRel
    'A', // PathSegArcAbs
    'a', // PathSegArcRel
    'H', // PathSegLineToHorizontalAbs
    'h', // PathSegLineToHorizontalRel
    'V', // PathSegLineToVerticalAbs
    'v', // PathSegLineToVerticalRel
    'S', // PathSegCurveToCubicSmoothAbs
    's', // PathSegCurveToCubicSmoothRel
    'T', // PathSegCurveToQuadraticSmoothAbs
    't', // PathSegCurveToQuadraticSmoothRel
};

void SVGPathStringBuilder::emitSegment(const PathSegmentData& segment)
{
    ASSERT(segment.command > PathSegUnknown && segment.command <= PathSegCurveToQuadraticSmoothRel);
    m_stringBuilder.append(pathSegmentCharacter[segment.command]);

    switch (segment.command) {
    case PathSegMoveToRel:
    case PathSegMoveToAbs:
    case PathSegLineToRel:
    case PathSegLineToAbs:
    case PathSegCurveToQuadraticSmoothRel:
    case PathSegCurveToQuadraticSmoothAbs:
        appendPoint(m_stringBuilder, segment.targetPoint);
        break;
    case PathSegLineToHorizontalRel:
    case PathSegLineToHorizontalAbs:
        appendFloat(m_stringBuilder, segment.targetPoint.x());
        break;
    case PathSegLineToVerticalRel:
    case PathSegLineToVerticalAbs:
        appendFloat(m_stringBuilder, segment.targetPoint.y());
        break;
    case PathSegClosePath:
        break;
    case PathSegCurveToCubicRel:
    case PathSegCurveToCubicAbs:
        appendPoint(m_stringBuilder, segment.point1);
        appendPoint(m_stringBuilder, segment.point2);
        appendPoint(m_stringBuilder, segment.targetPoint);
        break;
    case PathSegCurveToCubicSmoothRel:
    case PathSegCurveToCubicSmoothAbs:
        appendPoint(m_stringBuilder, segment.point2);
        appendPoint(m_stringBuilder, segment.targetPoint);
        break;
    case PathSegCurveToQuadraticRel:
    case PathSegCurveToQuadraticAbs:
        appendPoint(m_stringBuilder, segment.point1);
        appendPoint(m_stringBuilder, segment.targetPoint);
        break;
    case PathSegArcRel:
    case PathSegArcAbs:
        appendPoint(m_stringBuilder, segment.point1);
        appendFloat(m_stringBuilder, segment.point2.x());
        appendBool(m_stringBuilder, segment.arcLarge);
        appendBool(m_stringBuilder, segment.arcSweep);
        appendPoint(m_stringBuilder, segment.targetPoint);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
    m_stringBuilder.append(' ');
}

} // namespace blink
