/*
 * Copyright (C) Research In Motion Limited 2010, 2011. All rights reserved.
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
#include "core/svg/SVGPathBlender.h"

#include "core/svg/SVGPathConsumer.h"
#include "core/svg/SVGPathSeg.h"
#include "core/svg/SVGPathSource.h"
#include "platform/animation/AnimationUtilities.h"

namespace blink {

enum FloatBlendMode {
    BlendHorizontal,
    BlendVertical
};

class SVGPathBlender::BlendState {
public:
    BlendState(float progress, unsigned addTypesCount = 0)
        : m_progress(progress)
        , m_addTypesCount(addTypesCount)
        , m_isInFirstHalfOfAnimation(progress < 0.5f)
        , m_typesAreEqual(false)
        , m_fromIsAbsolute(false)
    {
    }

    bool blendSegments(const PathSegmentData& fromSeg, const PathSegmentData& toSeg, PathSegmentData&);

private:
    float blendAnimatedDimensonalFloat(float, float, FloatBlendMode);
    FloatPoint blendAnimatedFloatPointSameCoordinates(const FloatPoint& from, const FloatPoint& to);
    FloatPoint blendAnimatedFloatPoint(const FloatPoint& from, const FloatPoint& to);
    bool canBlend(const PathSegmentData& fromSeg, const PathSegmentData& toSeg);

    FloatPoint m_fromCurrentPoint;
    FloatPoint m_toCurrentPoint;

    float m_progress;
    unsigned m_addTypesCount;
    bool m_isInFirstHalfOfAnimation;
    // This is per-segment blend state corresponding to the 'from' and 'to'
    // segments currently being blended, and only used within blendSegments().
    bool m_typesAreEqual;
    bool m_fromIsAbsolute;
};

// Helper functions
static inline FloatPoint blendFloatPoint(const FloatPoint& a, const FloatPoint& b, float progress)
{
    return FloatPoint(blend(a.x(), b.x(), progress), blend(a.y(), b.y(), progress));
}

float SVGPathBlender::BlendState::blendAnimatedDimensonalFloat(float from, float to, FloatBlendMode blendMode)
{
    if (m_addTypesCount) {
        ASSERT(m_typesAreEqual);
        return from + to * m_addTypesCount;
    }

    if (m_typesAreEqual)
        return blend(from, to, m_progress);

    float fromValue = blendMode == BlendHorizontal ? m_fromCurrentPoint.x() : m_fromCurrentPoint.y();
    float toValue = blendMode == BlendHorizontal ? m_toCurrentPoint.x() : m_toCurrentPoint.y();

    // Transform toY to the coordinate mode of fromY
    float animValue = blend(from, m_fromIsAbsolute ? to + toValue : to - toValue, m_progress);

    // If we're in the first half of the animation, we should use the type of the from segment.
    if (m_isInFirstHalfOfAnimation)
        return animValue;

    // Transform the animated point to the coordinate mode, needed for the current progress.
    float currentValue = blend(fromValue, toValue, m_progress);
    return !m_fromIsAbsolute ? animValue + currentValue : animValue - currentValue;
}

FloatPoint SVGPathBlender::BlendState::blendAnimatedFloatPointSameCoordinates(const FloatPoint& fromPoint, const FloatPoint& toPoint)
{
    if (m_addTypesCount) {
        FloatPoint repeatedToPoint = toPoint;
        repeatedToPoint.scale(m_addTypesCount, m_addTypesCount);
        return fromPoint + repeatedToPoint;
    }
    return blendFloatPoint(fromPoint, toPoint, m_progress);
}

FloatPoint SVGPathBlender::BlendState::blendAnimatedFloatPoint(const FloatPoint& fromPoint, const FloatPoint& toPoint)
{
    if (m_typesAreEqual)
        return blendAnimatedFloatPointSameCoordinates(fromPoint, toPoint);

    // Transform toPoint to the coordinate mode of fromPoint
    FloatPoint animatedPoint = toPoint;
    if (m_fromIsAbsolute)
        animatedPoint += m_toCurrentPoint;
    else
        animatedPoint.move(-m_toCurrentPoint.x(), -m_toCurrentPoint.y());

    animatedPoint = blendFloatPoint(fromPoint, animatedPoint, m_progress);

    // If we're in the first half of the animation, we should use the type of the from segment.
    if (m_isInFirstHalfOfAnimation)
        return animatedPoint;

    // Transform the animated point to the coordinate mode, needed for the current progress.
    FloatPoint currentPoint = blendFloatPoint(m_fromCurrentPoint, m_toCurrentPoint, m_progress);
    if (!m_fromIsAbsolute)
        return animatedPoint + currentPoint;

    animatedPoint.move(-currentPoint.x(), -currentPoint.y());
    return animatedPoint;
}

bool SVGPathBlender::BlendState::canBlend(const PathSegmentData& fromSeg, const PathSegmentData& toSeg)
{
    // Update state first because we'll need it if we return true below.
    m_typesAreEqual = fromSeg.command == toSeg.command;
    m_fromIsAbsolute = isAbsolutePathSegType(fromSeg.command);

    // If the types are equal, they'll blend regardless of parameters.
    if (m_typesAreEqual)
        return true;

    // Addition require segments with the same type.
    if (m_addTypesCount)
        return false;

    // Allow the segments to differ in "relativeness".
    return toAbsolutePathSegType(fromSeg.command) == toAbsolutePathSegType(toSeg.command);
}

static void updateCurrentPoint(FloatPoint& currentPoint, const PathSegmentData& segment)
{
    switch (segment.command) {
    case PathSegMoveToRel:
    case PathSegLineToRel:
    case PathSegCurveToCubicRel:
    case PathSegCurveToQuadraticRel:
    case PathSegArcRel:
    case PathSegLineToHorizontalRel:
    case PathSegLineToVerticalRel:
    case PathSegCurveToCubicSmoothRel:
    case PathSegCurveToQuadraticSmoothRel:
        currentPoint += segment.targetPoint;
        break;
    case PathSegMoveToAbs:
    case PathSegLineToAbs:
    case PathSegCurveToCubicAbs:
    case PathSegCurveToQuadraticAbs:
    case PathSegArcAbs:
    case PathSegCurveToCubicSmoothAbs:
    case PathSegCurveToQuadraticSmoothAbs:
        currentPoint = segment.targetPoint;
        break;
    case PathSegLineToHorizontalAbs:
        currentPoint.setX(segment.targetPoint.x());
        break;
    case PathSegLineToVerticalAbs:
        currentPoint.setY(segment.targetPoint.y());
        break;
    case PathSegClosePath:
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

bool SVGPathBlender::BlendState::blendSegments(const PathSegmentData& fromSeg, const PathSegmentData& toSeg, PathSegmentData& blendedSegment)
{
    if (!canBlend(fromSeg, toSeg))
        return false;

    blendedSegment.command = m_isInFirstHalfOfAnimation ? fromSeg.command : toSeg.command;

    switch (toSeg.command) {
    case PathSegCurveToCubicRel:
    case PathSegCurveToCubicAbs:
        blendedSegment.point1 = blendAnimatedFloatPoint(fromSeg.point1, toSeg.point1);
        /* fall through */
    case PathSegCurveToCubicSmoothRel:
    case PathSegCurveToCubicSmoothAbs:
        blendedSegment.point2 = blendAnimatedFloatPoint(fromSeg.point2, toSeg.point2);
        /* fall through */
    case PathSegMoveToRel:
    case PathSegMoveToAbs:
    case PathSegLineToRel:
    case PathSegLineToAbs:
    case PathSegCurveToQuadraticSmoothRel:
    case PathSegCurveToQuadraticSmoothAbs:
        blendedSegment.targetPoint = blendAnimatedFloatPoint(fromSeg.targetPoint, toSeg.targetPoint);
        break;
    case PathSegLineToHorizontalRel:
    case PathSegLineToHorizontalAbs:
        blendedSegment.targetPoint.setX(blendAnimatedDimensonalFloat(fromSeg.targetPoint.x(), toSeg.targetPoint.x(), BlendHorizontal));
        break;
    case PathSegLineToVerticalRel:
    case PathSegLineToVerticalAbs:
        blendedSegment.targetPoint.setY(blendAnimatedDimensonalFloat(fromSeg.targetPoint.y(), toSeg.targetPoint.y(), BlendVertical));
        break;
    case PathSegClosePath:
        break;
    case PathSegCurveToQuadraticRel:
    case PathSegCurveToQuadraticAbs:
        blendedSegment.targetPoint = blendAnimatedFloatPoint(fromSeg.targetPoint, toSeg.targetPoint);
        blendedSegment.point1 = blendAnimatedFloatPoint(fromSeg.point1, toSeg.point1);
        break;
    case PathSegArcRel:
    case PathSegArcAbs:
        blendedSegment.targetPoint = blendAnimatedFloatPoint(fromSeg.targetPoint, toSeg.targetPoint);
        blendedSegment.point1 = blendAnimatedFloatPointSameCoordinates(fromSeg.arcRadii(), toSeg.arcRadii());
        blendedSegment.point2 = blendAnimatedFloatPointSameCoordinates(fromSeg.point2, toSeg.point2);
        if (m_addTypesCount) {
            blendedSegment.arcLarge = fromSeg.arcLarge || toSeg.arcLarge;
            blendedSegment.arcSweep = fromSeg.arcSweep || toSeg.arcSweep;
        } else {
            blendedSegment.arcLarge = m_isInFirstHalfOfAnimation ? fromSeg.arcLarge : toSeg.arcLarge;
            blendedSegment.arcSweep = m_isInFirstHalfOfAnimation ? fromSeg.arcSweep : toSeg.arcSweep;
        }
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    updateCurrentPoint(m_fromCurrentPoint, fromSeg);
    updateCurrentPoint(m_toCurrentPoint, toSeg);

    return true;
}

SVGPathBlender::SVGPathBlender(SVGPathSource* fromSource, SVGPathSource* toSource, SVGPathConsumer* consumer)
    : m_fromSource(fromSource)
    , m_toSource(toSource)
    , m_consumer(consumer)
{
    ASSERT(m_fromSource);
    ASSERT(m_toSource);
    ASSERT(m_consumer);
}

bool SVGPathBlender::addAnimatedPath(unsigned repeatCount)
{
    BlendState blendState(0, repeatCount);
    return blendAnimatedPath(blendState);
}

bool SVGPathBlender::blendAnimatedPath(float progress)
{
    BlendState blendState(progress);
    return blendAnimatedPath(blendState);
}

bool SVGPathBlender::blendAnimatedPath(BlendState& blendState)
{
    bool fromSourceIsEmpty = !m_fromSource->hasMoreData();
    while (m_toSource->hasMoreData()) {
        PathSegmentData toSeg = m_toSource->parseSegment();
        if (toSeg.command == PathSegUnknown)
            return false;

        PathSegmentData fromSeg;
        fromSeg.command = toSeg.command;

        if (m_fromSource->hasMoreData()) {
            fromSeg = m_fromSource->parseSegment();
            if (fromSeg.command == PathSegUnknown)
                return false;
        }

        PathSegmentData blendedSeg;
        if (!blendState.blendSegments(fromSeg, toSeg, blendedSeg))
            return false;

        m_consumer->emitSegment(blendedSeg);

        if (fromSourceIsEmpty)
            continue;
        if (m_fromSource->hasMoreData() != m_toSource->hasMoreData())
            return false;
    }
    return true;
}

}
