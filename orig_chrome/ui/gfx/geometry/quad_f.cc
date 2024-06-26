// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/geometry/quad_f.h"

#include <limits>

#include "base/strings/stringprintf.h"

namespace gfx {

void QuadF::operator=(const RectF& rect)
{
    p1_ = PointF(rect.x(), rect.y());
    p2_ = PointF(rect.right(), rect.y());
    p3_ = PointF(rect.right(), rect.bottom());
    p4_ = PointF(rect.x(), rect.bottom());
}

std::string QuadF::ToString() const
{
    return base::StringPrintf("%s;%s;%s;%s",
        p1_.ToString().c_str(),
        p2_.ToString().c_str(),
        p3_.ToString().c_str(),
        p4_.ToString().c_str());
}

static inline bool WithinEpsilon(float a, float b)
{
    return std::abs(a - b) < std::numeric_limits<float>::epsilon();
}

bool QuadF::IsRectilinear() const
{
    return (WithinEpsilon(p1_.x(), p2_.x()) && WithinEpsilon(p2_.y(), p3_.y()) && WithinEpsilon(p3_.x(), p4_.x()) && WithinEpsilon(p4_.y(), p1_.y())) || (WithinEpsilon(p1_.y(), p2_.y()) && WithinEpsilon(p2_.x(), p3_.x()) && WithinEpsilon(p3_.y(), p4_.y()) && WithinEpsilon(p4_.x(), p1_.x()));
}

bool QuadF::IsCounterClockwise() const
{
    // This math computes the signed area of the quad. Positive area
    // indicates the quad is clockwise; negative area indicates the quad is
    // counter-clockwise. Note carefully: this is backwards from conventional
    // math because our geometric space uses screen coordiantes with y-axis
    // pointing downards.
    // Reference: http://mathworld.wolfram.com/PolygonArea.html.
    // The equation can be written:
    // Signed area = determinant1 + determinant2 + determinant3 + determinant4
    // In practise, Refactoring the computation of adding determinants so that
    // reducing the number of operations. The equation is:
    // Signed area = element1 + element2 - element3 - element4

    float p24 = p2_.y() - p4_.y();
    float p31 = p3_.y() - p1_.y();

    // Up-cast to double so this cannot overflow.
    double element1 = static_cast<double>(p1_.x()) * p24;
    double element2 = static_cast<double>(p2_.x()) * p31;
    double element3 = static_cast<double>(p3_.x()) * p24;
    double element4 = static_cast<double>(p4_.x()) * p31;

    return element1 + element2 < element3 + element4;
}

static inline bool PointIsInTriangle(const PointF& point,
    const PointF& r1,
    const PointF& r2,
    const PointF& r3)
{
    // Compute the barycentric coordinates (u, v, w) of |point| relative to the
    // triangle (r1, r2, r3) by the solving the system of equations:
    //   1) point = u * r1 + v * r2 + w * r3
    //   2) u + v + w = 1
    // This algorithm comes from Christer Ericson's Real-Time Collision Detection.

    Vector2dF r31 = r1 - r3;
    Vector2dF r32 = r2 - r3;
    Vector2dF r3p = point - r3;

    float denom = r32.y() * r31.x() - r32.x() * r31.y();
    float u = (r32.y() * r3p.x() - r32.x() * r3p.y()) / denom;
    float v = (r31.x() * r3p.y() - r31.y() * r3p.x()) / denom;
    float w = 1.f - u - v;

    // Use the barycentric coordinates to test if |point| is inside the
    // triangle (r1, r2, r2).
    return (u >= 0) && (v >= 0) && (w >= 0);
}

bool QuadF::Contains(const PointF& point) const
{
    return PointIsInTriangle(point, p1_, p2_, p3_)
        || PointIsInTriangle(point, p1_, p3_, p4_);
}

void QuadF::Scale(float x_scale, float y_scale)
{
    p1_.Scale(x_scale, y_scale);
    p2_.Scale(x_scale, y_scale);
    p3_.Scale(x_scale, y_scale);
    p4_.Scale(x_scale, y_scale);
}

void QuadF::operator+=(const Vector2dF& rhs)
{
    p1_ += rhs;
    p2_ += rhs;
    p3_ += rhs;
    p4_ += rhs;
}

void QuadF::operator-=(const Vector2dF& rhs)
{
    p1_ -= rhs;
    p2_ -= rhs;
    p3_ -= rhs;
    p4_ -= rhs;
}

QuadF operator+(const QuadF& lhs, const Vector2dF& rhs)
{
    QuadF result = lhs;
    result += rhs;
    return result;
}

QuadF operator-(const QuadF& lhs, const Vector2dF& rhs)
{
    QuadF result = lhs;
    result -= rhs;
    return result;
}

} // namespace gfx
