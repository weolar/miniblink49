/*
 * Copyright (C) 2002, 2003 The Karbon Developers
 * Copyright (C) 2006 Alexander Kellett <lypanov@kde.org>
 * Copyright (C) 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007, 2009 Apple Inc. All rights reserved.
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
#include "core/svg/SVGPathSegListBuilder.h"

#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGPathElement.h"
#include "core/svg/SVGPathSegArcAbs.h"
#include "core/svg/SVGPathSegArcRel.h"
#include "core/svg/SVGPathSegClosePath.h"
#include "core/svg/SVGPathSegCurvetoCubicAbs.h"
#include "core/svg/SVGPathSegCurvetoCubicRel.h"
#include "core/svg/SVGPathSegCurvetoCubicSmoothAbs.h"
#include "core/svg/SVGPathSegCurvetoCubicSmoothRel.h"
#include "core/svg/SVGPathSegCurvetoQuadraticAbs.h"
#include "core/svg/SVGPathSegCurvetoQuadraticRel.h"
#include "core/svg/SVGPathSegCurvetoQuadraticSmoothAbs.h"
#include "core/svg/SVGPathSegCurvetoQuadraticSmoothRel.h"
#include "core/svg/SVGPathSegLinetoAbs.h"
#include "core/svg/SVGPathSegLinetoHorizontalAbs.h"
#include "core/svg/SVGPathSegLinetoHorizontalRel.h"
#include "core/svg/SVGPathSegLinetoRel.h"
#include "core/svg/SVGPathSegLinetoVerticalAbs.h"
#include "core/svg/SVGPathSegLinetoVerticalRel.h"
#include "core/svg/SVGPathSegMovetoAbs.h"
#include "core/svg/SVGPathSegMovetoRel.h"

namespace blink {

SVGPathSegListBuilder::SVGPathSegListBuilder(SVGPathElement* pathElement, PassRefPtrWillBeRawPtr<SVGPathSegList> pathSegList)
    : m_pathElement(pathElement)
    , m_pathSegList(pathSegList)
{
    ASSERT(m_pathElement);
    ASSERT(m_pathSegList);
}

void SVGPathSegListBuilder::emitSegment(const PathSegmentData& segment)
{
    RefPtrWillBeRawPtr<SVGPathSeg> newPathSegment;
    switch (segment.command) {
    case PathSegMoveToRel:
        newPathSegment = SVGPathSegMovetoRel::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y());
        break;
    case PathSegMoveToAbs:
        newPathSegment = SVGPathSegMovetoAbs::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y());
        break;
    case PathSegLineToRel:
        newPathSegment = SVGPathSegLinetoRel::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y());
        break;
    case PathSegLineToAbs:
        newPathSegment = SVGPathSegLinetoAbs::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y());
        break;
    case PathSegLineToHorizontalRel:
        newPathSegment = SVGPathSegLinetoHorizontalRel::create(m_pathElement, segment.targetPoint.x());
        break;
    case PathSegLineToHorizontalAbs:
        newPathSegment = SVGPathSegLinetoHorizontalAbs::create(m_pathElement, segment.targetPoint.x());
        break;
    case PathSegLineToVerticalRel:
        newPathSegment = SVGPathSegLinetoVerticalRel::create(m_pathElement, segment.targetPoint.y());
        break;
    case PathSegLineToVerticalAbs:
        newPathSegment = SVGPathSegLinetoVerticalAbs::create(m_pathElement, segment.targetPoint.y());
        break;
    case PathSegClosePath:
        newPathSegment = SVGPathSegClosePath::create(m_pathElement);
        break;
    case PathSegCurveToCubicRel:
        newPathSegment = SVGPathSegCurvetoCubicRel::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y(), segment.point1.x(), segment.point1.y(), segment.point2.x(), segment.point2.y());
        break;
    case PathSegCurveToCubicAbs:
        newPathSegment = SVGPathSegCurvetoCubicAbs::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y(), segment.point1.x(), segment.point1.y(), segment.point2.x(), segment.point2.y());
        break;
    case PathSegCurveToCubicSmoothRel:
        newPathSegment = SVGPathSegCurvetoCubicSmoothRel::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y(), segment.point2.x(), segment.point2.y());
        break;
    case PathSegCurveToCubicSmoothAbs:
        newPathSegment = SVGPathSegCurvetoCubicSmoothAbs::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y(), segment.point2.x(), segment.point2.y());
        break;
    case PathSegCurveToQuadraticRel:
        newPathSegment = SVGPathSegCurvetoQuadraticRel::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y(), segment.point1.x(), segment.point1.y());
        break;
    case PathSegCurveToQuadraticAbs:
        newPathSegment = SVGPathSegCurvetoQuadraticAbs::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y(), segment.point1.x(), segment.point1.y());
        break;
    case PathSegCurveToQuadraticSmoothRel:
        newPathSegment = SVGPathSegCurvetoQuadraticSmoothRel::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y());
        break;
    case PathSegCurveToQuadraticSmoothAbs:
        newPathSegment = SVGPathSegCurvetoQuadraticSmoothAbs::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y());
        break;
    case PathSegArcRel:
        newPathSegment = SVGPathSegArcRel::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y(), segment.arcRadii().x(), segment.arcRadii().y(), segment.arcAngle(), segment.arcLarge, segment.arcSweep);
        break;
    case PathSegArcAbs:
        newPathSegment = SVGPathSegArcAbs::create(m_pathElement, segment.targetPoint.x(), segment.targetPoint.y(), segment.arcRadii().x(), segment.arcRadii().y(), segment.arcAngle(), segment.arcLarge, segment.arcSweep);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
    m_pathSegList->appendWithoutByteStreamSync(newPathSegment);
}

}
