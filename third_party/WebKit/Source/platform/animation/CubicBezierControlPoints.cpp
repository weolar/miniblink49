// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/animation/CubicBezierControlPoints.h"

#include "platform/animation/AnimationUtilities.h"
#include <algorithm>

namespace blink {

static inline double square(double x)
{
    return x*x;
}

// Divide the Bezier curve into two pieces at the given value of t using
// de Casteljau's algorithm.
void CubicBezierControlPoints::divide(double t, CubicBezierControlPoints& left, CubicBezierControlPoints& right) const
{
    double x0A = blend(x0, x1, t);
    double y0A = blend(y0, y1, t);

    double x1A = blend(x1, x2, t);
    double y1A = blend(y1, y2, t);

    double x2A = blend(x2, x3, t);
    double y2A = blend(y2, y3, t);

    double x0B = blend(x0A, x1A, t);
    double y0B = blend(y0A, y1A, t);

    double x1B = blend(x1A, x2A, t);
    double y1B = blend(y1A, y2A, t);

    double x0C = blend(x0B, x1B, t);
    double y0C = blend(y0B, y1B, t);

    left.x0 = x0;
    left.y0 = y0;

    left.x1 = x0A;
    left.y1 = y0A;

    left.x2 = x0B;
    left.y2 = y0B;

    left.x3 = x0C;
    left.y3 = y0C;

    right.x0 = x0C;
    right.y0 = y0C;

    right.x1 = x1B;
    right.y1 = y1B;

    right.x2 = x2A;
    right.y2 = y2A;

    right.x3 = x3;
    right.y3 = y3;
}

// Finds a value of t for which d^2y/dt^2 = 0 within the range (0,1).
bool CubicBezierControlPoints::findInflexionPoint(double& solution) const
{
    // Second derivative of the cubic bezier: solving at + b = 0.
    double a = 6 * (y3 - 3 * y2 + 3 * y1 - y0);
    double b = 6 * (y2 - 2 * y1 + y0);

    if (a != 0) {
        double t = -b / a;

        if (t > 0 && t < 1) {
            solution = t;
            return true;
        }
    }

    return false;
}

// Find values of t for which dy/dt = 0 and d^2y/dt^2 != 0 within the
// range (0,1).
size_t CubicBezierControlPoints::findTurningPoints(double& left, double& right) const
{
    // If the control points are strictly increasing/decreasing, there
    // can be no stationary points.
    if ((y0 < y1 && y1 < y2 && y2 < y3)
        || (y0 > y1 && y1 > y2 && y2 > y3))
        return 0;

    // Derivative of the cubic bezier: solving at^2 + bt + c = 0.
    double a = -3 * (y0 - 3 * y1 + 3 * y2 - y3);
    double b = 6 * (y0 - 2 * y1 + y2);
    double c = -3 * (y0 - y1);

    if (std::abs(a) < std::numeric_limits<double>::epsilon()
        && std::abs(b) < std::numeric_limits<double>::epsilon())
        return 0;

    if (std::abs(a) < std::numeric_limits<double>::epsilon()) {
        left = -c / b;
        return 1;
    }

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
        return 0;

    double discriminantSqrt = sqrt(discriminant);

    double solution1 = (-b + discriminantSqrt) / (2 * a);
    double solution2 = (-b - discriminantSqrt) / (2 * a);

    // Check if either of the solutions lie in (0,1).
    bool solution1Viable = (solution1 > 0) && (solution1 < 1);
    bool solution2Viable = (solution2 > 0) && (solution2 < 1);

    // If we find stationary points, ensure they are turning points,
    // i.e. they are not a point of inflexion.
    double inflexionPoint = 0.0;

    if (findInflexionPoint(inflexionPoint)) {
        if (solution1Viable && std::abs(solution1 - inflexionPoint) < std::numeric_limits<double>::epsilon())
            solution1Viable = false;

        if (solution2Viable && std::abs(solution2 - inflexionPoint) < std::numeric_limits<double>::epsilon())
            solution2Viable = false;
    }

    if (solution1Viable && solution2Viable) {
        if (std::abs(solution1 - solution2) < std::numeric_limits<double>::epsilon()) {
            left = solution1;
            return 1;
        }

        if (solution1 < solution2) {
            left = solution1;
            right = solution2;
        } else {
            left = solution2;
            right = solution1;
        }
        return 2;
    }

    if (solution1Viable) {
        left = solution1;
        return 1;
    }

    if (solution2Viable) {
        left = solution2;
        return 1;
    }

    return 0;
}

// Finds the intersection of the cubic bezier with the horizontal line
// y = intersectionY (assuming the curve is monotonically increasing/
// decreasing).
bool CubicBezierControlPoints::findIntersection(double intersectionY, double& intersectionX) const
{
    // If the last control point lies on the horizontal line, then use
    // its x value as the intersection.
    // This is done so that if a cubic bezier is divided at a point of
    // intersection, we only detect the intersection in leftPoints,
    // short-circuiting the return at the end of this method, and
    // avoiding finding the intersection again in rightPoints.
    if (std::abs(y3 - intersectionY) < std::numeric_limits<double>::epsilon()) {
        intersectionX = x3;
        return true;
    }

    // Ensure there are control points both above and below the
    // horizontal line, and thus there is actually a point of
    // intersection.
    double smallestY = std::min(y0, y3);
    double largestY = std::max(y0, y3);

    if (!(smallestY < intersectionY && largestY > intersectionY))
        return false;

    // If the line joining the first and last control points is
    // about the same length as the line joining all the control
    // points in sequence, then we can treat the bezier as a straight
    // line, and just find its intersection with the horizontal line.
    double straightDistance = sqrt(square(x0 - x3) + square(y0 - y3));
    double pointsDistance = sqrt(square(x0 - x1) + square(y0 - y1))
        + sqrt(square(x1 - x2) + square(y1 - y2))
        + sqrt(square(x2 - x3) + square(y2 - y3));

    if (std::abs(straightDistance - pointsDistance) < std::numeric_limits<double>::epsilon()) {
        // If the bezier approximates a different horizontal line,
        // there'll be no point of intersection.
        if (std::abs(y3 - y0) < std::numeric_limits<double>::epsilon())
            return false;

        intersectionX = (intersectionY - y0)*(x3 - x0) / (y3 - y0) + x0;
        return true;
    }

    // Divide the bezier into two smaller segments, and continue
    // searching for intersections in both of them.
    CubicBezierControlPoints leftSegment;
    CubicBezierControlPoints rightSegment;

    divide(0.5, leftSegment, rightSegment);

    return (leftSegment.findIntersection(intersectionY, intersectionX)
        || rightSegment.findIntersection(intersectionY, intersectionX));
}

} // namespace blink
