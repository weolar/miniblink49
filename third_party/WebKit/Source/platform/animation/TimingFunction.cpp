// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/animation/TimingFunction.h"

#include "platform/animation/CubicBezierControlPoints.h"
#include "wtf/MathExtras.h"

namespace blink {

String LinearTimingFunction::toString() const
{
    return "linear";
}

double LinearTimingFunction::evaluate(double fraction, double) const
{
    return fraction;
}

void LinearTimingFunction::range(double* minValue, double* maxValue) const
{
}

void LinearTimingFunction::partition(Vector<PartitionRegion>& regions) const
{
    regions.append(PartitionRegion(RangeHalf::Lower, 0.0, 0.5));
    regions.append(PartitionRegion(RangeHalf::Upper, 0.5, 1.0));
}

String CubicBezierTimingFunction::toString() const
{
    switch (this->subType()) {
    case CubicBezierTimingFunction::Ease:
        return "ease";
    case CubicBezierTimingFunction::EaseIn:
        return "ease-in";
    case CubicBezierTimingFunction::EaseOut:
        return "ease-out";
    case CubicBezierTimingFunction::EaseInOut:
        return "ease-in-out";
    case CubicBezierTimingFunction::Custom:
        return "cubic-bezier(" + String::numberToStringECMAScript(this->x1()) + ", " +
            String::numberToStringECMAScript(this->y1()) + ", " + String::numberToStringECMAScript(this->x2()) +
            ", " + String::numberToStringECMAScript(this->y2()) + ")";
    default:
        ASSERT_NOT_REACHED();
    }
    return "";
}

double CubicBezierTimingFunction::evaluate(double fraction, double accuracy) const
{
    if (!m_bezier)
        m_bezier = adoptPtr(new UnitBezier(m_x1, m_y1, m_x2, m_y2));
    return m_bezier->solve(fraction, accuracy);
}

// This works by taking taking the derivative of the cubic bezier, on the y
// axis. We can then solve for where the derivative is zero to find the min
// and max distace along the line. We the have to solve those in terms of time
// rather than distance on the x-axis
void CubicBezierTimingFunction::range(double* minValue, double* maxValue) const
{
    if (0 <= m_y1 && m_y2 < 1 && 0 <= m_y2 && m_y2 <= 1) {
        return;
    }

    double a = 3.0 * (m_y1 - m_y2) + 1.0;
    double b = 2.0 * (m_y2 - 2.0 * m_y1);
    double c = m_y1;

    if (std::abs(a) < std::numeric_limits<double>::epsilon()
        && std::abs(b) < std::numeric_limits<double>::epsilon()) {
        return;
    }

    double t1 = 0.0;
    double t2 = 0.0;

    if (std::abs(a) < std::numeric_limits<double>::epsilon()) {
        t1 = -c / b;
    } else {
        double discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            return;
        double discriminantSqrt = sqrt(discriminant);
        t1 = (-b + discriminantSqrt) / (2 * a);
        t2 = (-b - discriminantSqrt) / (2 * a);
    }

    double solution1 = 0.0;
    double solution2 = 0.0;

    // If the solution is in the range [0,1] then we include it, otherwise we
    // ignore it.
    if (!m_bezier)
        m_bezier = adoptPtr(new UnitBezier(m_x1, m_y1, m_x2, m_y2));

    // An interesting fact about these beziers is that they are only
    // actually evaluated in [0,1]. After that we take the tangent at that point
    // and linearly project it out.
    if (0 < t1 && t1 < 1)
        solution1 = m_bezier->sampleCurveY(t1);

    if (0 < t2 && t2 < 1)
        solution2 = m_bezier->sampleCurveY(t2);

    // Since our input values can be out of the range 0->1 so we must also
    // consider the minimum and maximum points.
    double solutionMin = m_bezier->solve(*minValue, std::numeric_limits<double>::epsilon());
    double solutionMax = m_bezier->solve(*maxValue, std::numeric_limits<double>::epsilon());
    *minValue = std::min(std::min(solutionMin, solutionMax), 0.0);
    *maxValue = std::max(std::max(solutionMin, solutionMax), 1.0);
    *minValue = std::min(std::min(*minValue, solution1), solution2);
    *maxValue = std::max(std::max(*maxValue, solution1), solution2);
}

size_t CubicBezierTimingFunction::findIntersections(double intersectionY, double& solution1, double& solution2, double& solution3) const
{
    size_t numberOfIntersections = 0;

    // Divide the bezier into a number of monotonically
    // increasing/decreasing segments, so each can intersect the
    // horizontal line at most once.
    Vector<CubicBezierControlPoints> monotonicSegments;

    CubicBezierControlPoints initialSegment = CubicBezierControlPoints(0, 0, m_x1, m_y1, m_x2, m_y2, 1, 1);

    // Find the curve's turning points, so we can split it into
    // monotonically increasing/decreasing segments.
    double turningPoint1 = 0.0;
    double turningPoint2 = 0.0;

    // Note the x values of each turning point, so we can discard
    // intersections at these points (since they don't actually
    // cross the horizontal line, but just touch it).
    if (!m_bezier)
        m_bezier = adoptPtr(new UnitBezier(m_x1, m_y1, m_x2, m_y2));
    double turningX1 = 0.0;
    double turningX2 = 0.0;

    size_t numberOfTurningPoints = initialSegment.findTurningPoints(turningPoint1, turningPoint2);
    switch (numberOfTurningPoints) {
    case 2:
        {
            // Split into three segments.
            CubicBezierControlPoints leftSegment = CubicBezierControlPoints();
            CubicBezierControlPoints middleSegment = CubicBezierControlPoints();
            CubicBezierControlPoints rightSegment = CubicBezierControlPoints();

            CubicBezierControlPoints tmpSegment = CubicBezierControlPoints();

            initialSegment.divide(turningPoint2, tmpSegment, rightSegment);
            tmpSegment.divide(turningPoint1 / turningPoint2, leftSegment, middleSegment);

            monotonicSegments.append(leftSegment);
            monotonicSegments.append(middleSegment);
            monotonicSegments.append(rightSegment);

            turningX1 = m_bezier->sampleCurveX(turningPoint1);
            turningX2 = m_bezier->sampleCurveX(turningPoint2);

            break;
        }
    case 1:
        {
            // Split into two segments.
            CubicBezierControlPoints leftSegment = CubicBezierControlPoints();
            CubicBezierControlPoints rightSegment = CubicBezierControlPoints();

            initialSegment.divide(turningPoint1, leftSegment, rightSegment);

            monotonicSegments.append(leftSegment);
            monotonicSegments.append(rightSegment);

            turningX1 = m_bezier->sampleCurveX(turningPoint1);

            break;
        }
    case 0:
        monotonicSegments.append(initialSegment);
        break;
    default:
        ASSERT_NOT_REACHED();
        return 0;
    }

    double intersectionX = 0.0;

    for (const auto& segment : monotonicSegments) {
        if (segment.findIntersection(intersectionY, intersectionX)) {
            // Ensure that this intersection isn't one of the turning
            // points!
            switch (numberOfTurningPoints) {
            case 2:
                if (std::abs(intersectionX - turningX2) < std::numeric_limits<double>::epsilon())
                    continue;
            case 1:
                if (std::abs(intersectionX - turningX1) < std::numeric_limits<double>::epsilon())
                    continue;
            }

            switch (numberOfIntersections) {
            case 0:
                solution1 = intersectionX;
                break;
            case 1:
                solution2 = intersectionX;
                break;
            case 2:
                solution3 = intersectionX;
                break;
            default:
                ASSERT_NOT_REACHED();
            }

            numberOfIntersections++;
        }
    }

    return numberOfIntersections;
}

void CubicBezierTimingFunction::partition(Vector<PartitionRegion>& regions) const
{
    double solution1 = 0.0;
    double solution2 = 0.0;
    double solution3 = 0.0;

    size_t numberOfIntersections = findIntersections(0.5, solution1, solution2, solution3);

    // A valid cubic bezier should only cross the horizontal line
    // 1 or 3 times.
    switch (numberOfIntersections) {
    case 1:
        regions.append(PartitionRegion(TimingFunction::RangeHalf::Lower, 0.0, solution1));
        regions.append(PartitionRegion(TimingFunction::RangeHalf::Upper, solution1, 1.0));
        break;
    case 3:
        regions.append(PartitionRegion(TimingFunction::RangeHalf::Lower, 0.0, solution1));
        regions.append(PartitionRegion(TimingFunction::RangeHalf::Upper, solution1, solution2));
        regions.append(PartitionRegion(TimingFunction::RangeHalf::Lower, solution2, solution3));
        regions.append(PartitionRegion(TimingFunction::RangeHalf::Upper, solution3, 1.0));
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }
}

String StepsTimingFunction::toString() const
{
    const char* positionString = nullptr;
    switch (stepAtPosition()) {
    case Start:
        positionString = "start";
        break;
    case Middle:
        positionString = "middle";
        break;
    case End:
        positionString = "end";
        break;
    }

    StringBuilder builder;
    if (this->numberOfSteps() == 1) {
        builder.append("step-");
        builder.append(positionString);
    } else {
        builder.append("steps(" + String::numberToStringECMAScript(this->numberOfSteps()) + ", ");
        builder.append(positionString);
        builder.append(')');
    }
    return builder.toString();
}

void StepsTimingFunction::range(double* minValue, double* maxValue) const
{
    *minValue = 0;
    *maxValue = 1;
}

double StepsTimingFunction::evaluate(double fraction, double) const
{
    double startOffset = 0;
    switch (m_stepAtPosition) {
    case Start:
        startOffset = 1;
        break;
    case Middle:
        startOffset = 0.5;
        break;
    case End:
        startOffset = 0;
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }
    return clampTo(floor((m_steps * fraction) + startOffset) / m_steps, 0.0, 1.0);
}

void StepsTimingFunction::partition(Vector<PartitionRegion>& regions) const
{
    double split = 0.0;

    if (m_steps % 2 == 0) {
        switch (m_stepAtPosition) {
        case Start:
            split = 0.5 - (1.0 / m_steps);
            break;
        case Middle:
            split = 0.5 - (0.5 / m_steps);
            break;
        case End:
            split = 0.5;
            break;
        default:
            ASSERT_NOT_REACHED();
            return;
        }
    } else {
        switch (m_stepAtPosition) {
        case Start:
            split = 0.5 - (0.5 / m_steps);
            break;
        case Middle:
            split = 0.5;
            break;
        case End:
            split = 0.5 + (0.5 / m_steps);
            break;
        default:
            ASSERT_NOT_REACHED();
            return;
        }
    }

    regions.append(PartitionRegion(TimingFunction::RangeHalf::Lower, 0.0, split));
    regions.append(PartitionRegion(TimingFunction::RangeHalf::Upper, split, 1.0));
}


// Equals operators
bool operator==(const LinearTimingFunction& lhs, const TimingFunction& rhs)
{
    return rhs.type() == TimingFunction::LinearFunction;
}

bool operator==(const CubicBezierTimingFunction& lhs, const TimingFunction& rhs)
{
    if (rhs.type() != TimingFunction::CubicBezierFunction)
        return false;

    const CubicBezierTimingFunction& ctf = toCubicBezierTimingFunction(rhs);
    if ((lhs.subType() == CubicBezierTimingFunction::Custom) && (ctf.subType() == CubicBezierTimingFunction::Custom))
        return (lhs.x1() == ctf.x1()) && (lhs.y1() == ctf.y1()) && (lhs.x2() == ctf.x2()) && (lhs.y2() == ctf.y2());

    return lhs.subType() == ctf.subType();
}

bool operator==(const StepsTimingFunction& lhs, const TimingFunction& rhs)
{
    if (rhs.type() != TimingFunction::StepsFunction)
        return false;

    const StepsTimingFunction& stf = toStepsTimingFunction(rhs);
    return (lhs.numberOfSteps() == stf.numberOfSteps()) && (lhs.stepAtPosition() == stf.stepAtPosition());
}

// The generic operator== *must* come after the
// non-generic operator== otherwise it will end up calling itself.
bool operator==(const TimingFunction& lhs, const TimingFunction& rhs)
{
    switch (lhs.type()) {
    case TimingFunction::LinearFunction: {
        const LinearTimingFunction& linear = toLinearTimingFunction(lhs);
        return (linear == rhs);
    }
    case TimingFunction::CubicBezierFunction: {
        const CubicBezierTimingFunction& cubic = toCubicBezierTimingFunction(lhs);
        return (cubic == rhs);
    }
    case TimingFunction::StepsFunction: {
        const StepsTimingFunction& step = toStepsTimingFunction(lhs);
        return (step == rhs);
    }
    default:
        ASSERT_NOT_REACHED();
    }
    return false;
}

// No need to define specific operator!= as they can all come via this function.
bool operator!=(const TimingFunction& lhs, const TimingFunction& rhs)
{
    return !(lhs == rhs);
}

} // namespace blink
