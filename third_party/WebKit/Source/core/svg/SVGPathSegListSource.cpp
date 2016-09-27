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
#include "core/svg/SVGPathSegListSource.h"

#include "core/svg/SVGPathElement.h"
#include "core/svg/SVGPathSegArc.h"
#include "core/svg/SVGPathSegCurvetoCubic.h"
#include "core/svg/SVGPathSegCurvetoCubicSmooth.h"
#include "core/svg/SVGPathSegCurvetoQuadratic.h"
#include "core/svg/SVGPathSegLinetoHorizontal.h"
#include "core/svg/SVGPathSegLinetoVertical.h"

namespace blink {

SVGPathSegListSource::SVGPathSegListSource(SVGPathSegList::ConstIterator itBegin, SVGPathSegList::ConstIterator itEnd)
    : m_itCurrent(itBegin)
    , m_itEnd(itEnd)
{
}

bool SVGPathSegListSource::hasMoreData() const
{
    return m_itCurrent != m_itEnd;
}

SVGPathSegType SVGPathSegListSource::peekSegmentType()
{
    ASSERT(hasMoreData());
    return static_cast<SVGPathSegType>(m_itCurrent->pathSegType());
}

PathSegmentData SVGPathSegListSource::parseSegment()
{
    ASSERT(hasMoreData());
    PathSegmentData segment;
    RefPtrWillBeRawPtr<SVGPathSeg> pathSegment = *m_itCurrent;
    segment.command = static_cast<SVGPathSegType>(pathSegment->pathSegType());
    ++m_itCurrent;

    switch (segment.command) {
    case PathSegMoveToRel:
    case PathSegMoveToAbs:
    case PathSegLineToRel:
    case PathSegLineToAbs:
    case PathSegCurveToQuadraticSmoothRel:
    case PathSegCurveToQuadraticSmoothAbs: {
        SVGPathSegSingleCoordinate* singleCoordinate = static_cast<SVGPathSegSingleCoordinate*>(pathSegment.get());
        segment.targetPoint = FloatPoint(singleCoordinate->x(), singleCoordinate->y());
        break;
    }
    case PathSegLineToHorizontalRel:
    case PathSegLineToHorizontalAbs: {
        SVGPathSegLinetoHorizontal* horizontal = static_cast<SVGPathSegLinetoHorizontal*>(pathSegment.get());
        segment.targetPoint.setX(horizontal->x());
        break;
    }
    case PathSegLineToVerticalRel:
    case PathSegLineToVerticalAbs: {
        SVGPathSegLinetoVertical* vertical = static_cast<SVGPathSegLinetoVertical*>(pathSegment.get());
        segment.targetPoint.setY(vertical->y());
        break;
    }
    case PathSegClosePath:
        break;
    case PathSegCurveToCubicRel:
    case PathSegCurveToCubicAbs: {
        SVGPathSegCurvetoCubic* cubic = static_cast<SVGPathSegCurvetoCubic*>(pathSegment.get());
        segment.point1 = FloatPoint(cubic->x1(), cubic->y1());
        segment.point2 = FloatPoint(cubic->x2(), cubic->y2());
        segment.targetPoint = FloatPoint(cubic->x(), cubic->y());
        break;
    }
    case PathSegCurveToCubicSmoothRel:
    case PathSegCurveToCubicSmoothAbs: {
        SVGPathSegCurvetoCubicSmooth* cubicSmooth = static_cast<SVGPathSegCurvetoCubicSmooth*>(pathSegment.get());
        segment.point2 = FloatPoint(cubicSmooth->x2(), cubicSmooth->y2());
        segment.targetPoint = FloatPoint(cubicSmooth->x(), cubicSmooth->y());
        break;
    }
    case PathSegCurveToQuadraticRel:
    case PathSegCurveToQuadraticAbs: {
        SVGPathSegCurvetoQuadratic* quadratic = static_cast<SVGPathSegCurvetoQuadratic*>(pathSegment.get());
        segment.point1 = FloatPoint(quadratic->x1(), quadratic->y1());
        segment.targetPoint = FloatPoint(quadratic->x(), quadratic->y());
        break;
    }
    case PathSegArcRel:
    case PathSegArcAbs: {
        SVGPathSegArc* arcTo = static_cast<SVGPathSegArc*>(pathSegment.get());
        segment.point1 = FloatPoint(arcTo->r1(), arcTo->r2());
        segment.point2.setX(arcTo->angle());
        segment.arcLarge = arcTo->largeArcFlag();
        segment.arcSweep = arcTo->sweepFlag();
        segment.targetPoint = FloatPoint(arcTo->x(), arcTo->y());
        break;
    }
    default:
        ASSERT_NOT_REACHED();
    }
    return segment;
}

}
