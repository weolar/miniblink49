// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_MATH_UTIL_H_
#define CC_BASE_MATH_UTIL_H_

#include <algorithm>
#include <cmath>
#include <vector>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "ui/gfx/geometry/box_f.h"
#include "ui/gfx/geometry/point3_f.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/scroll_offset.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/transform.h"

namespace base {
class Value;
namespace trace_event {
    class TracedValue;
}
} // namespace base

namespace gfx {
class QuadF;
class Rect;
class RectF;
class SizeF;
class Transform;
class Vector2dF;
class Vector2d;
class Vector3dF;
}

namespace cc {

struct HomogeneousCoordinate {
    HomogeneousCoordinate(SkMScalar x, SkMScalar y, SkMScalar z, SkMScalar w)
    {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        vec[3] = w;
    }

    bool ShouldBeClipped() const { return w() <= 0.0; }

    gfx::PointF CartesianPoint2d() const
    {
        if (w() == SK_MScalar1)
            return gfx::PointF(x(), y());

        // For now, because this code is used privately only by MathUtil, it should
        // never be called when w == 0, and we do not yet need to handle that case.
        DCHECK(w());
        SkMScalar inv_w = SK_MScalar1 / w();
        return gfx::PointF(x() * inv_w, y() * inv_w);
    }

    gfx::Point3F CartesianPoint3d() const
    {
        if (w() == SK_MScalar1)
            return gfx::Point3F(x(), y(), z());

        // For now, because this code is used privately only by MathUtil, it should
        // never be called when w == 0, and we do not yet need to handle that case.
        DCHECK(w());
        SkMScalar inv_w = SK_MScalar1 / w();
        return gfx::Point3F(x() * inv_w, y() * inv_w, z() * inv_w);
    }

    SkMScalar x() const { return vec[0]; }
    SkMScalar y() const { return vec[1]; }
    SkMScalar z() const { return vec[2]; }
    SkMScalar w() const { return vec[3]; }

    SkMScalar vec[4];
};

class CC_EXPORT MathUtil {
public:
    static const double kPiDouble;
    static const float kPiFloat;

    static double Deg2Rad(double deg) { return deg * kPiDouble / 180.0; }
    static double Rad2Deg(double rad) { return rad * 180.0 / kPiDouble; }

    static float Deg2Rad(float deg) { return deg * kPiFloat / 180.0f; }
    static float Rad2Deg(float rad) { return rad * 180.0f / kPiFloat; }

    static float Round(float f)
    {
        return (f > 0.f) ? std::floor(f + 0.5f) : std::ceil(f - 0.5f);
    }
    static double Round(double d)
    {
        return (d > 0.0) ? std::floor(d + 0.5) : std::ceil(d - 0.5);
    }

    // Returns true if rounded up value does not overflow, false otherwise.
    template <typename T>
    static bool VerifyRoundup(T n, T mul)
    {
        return mul && (n <= (std::numeric_limits<T>::max() - (std::numeric_limits<T>::max() % mul)));
    }

    // Rounds up a given |n| to be a multiple of |mul|, but may overflow.
    // Examples:
    //    - RoundUp(123, 50) returns 150.
    //    - RoundUp(-123, 50) returns -100.
    template <typename T>
    static T UncheckedRoundUp(T n, T mul)
    {
        static_assert(std::numeric_limits<T>::is_integer,
            "T must be an integer type");
        DCHECK(VerifyRoundup(n, mul));
        return RoundUpInternal(n, mul);
    }

    // Similar to UncheckedRoundUp(), but dies with a CRASH() if rounding up a
    // given |n| overflows T.
    template <typename T>
    static T CheckedRoundUp(T n, T mul)
    {
        static_assert(std::numeric_limits<T>::is_integer,
            "T must be an integer type");
        CHECK(VerifyRoundup(n, mul));
        return RoundUpInternal(n, mul);
    }

    // Returns true if rounded down value does not underflow, false otherwise.
    template <typename T>
    static bool VerifyRoundDown(T n, T mul)
    {
        return mul && (n >= (std::numeric_limits<T>::min() - (std::numeric_limits<T>::min() % mul)));
    }

    // Rounds down a given |n| to be a multiple of |mul|, but may underflow.
    // Examples:
    //    - RoundDown(123, 50) returns 100.
    //    - RoundDown(-123, 50) returns -150.
    template <typename T>
    static T UncheckedRoundDown(T n, T mul)
    {
        static_assert(std::numeric_limits<T>::is_integer,
            "T must be an integer type");
        DCHECK(VerifyRoundDown(n, mul));
        return RoundDownInternal(n, mul);
    }

    // Similar to UncheckedRoundDown(), but dies with a CRASH() if rounding down a
    // given |n| underflows T.
    template <typename T>
    static T CheckedRoundDown(T n, T mul)
    {
        static_assert(std::numeric_limits<T>::is_integer,
            "T must be an integer type");
        CHECK(VerifyRoundDown(n, mul));
        return RoundDownInternal(n, mul);
    }

    template <typename T>
    static T ClampToRange(T value, T min, T max)
    {
        return std::min(std::max(value, min), max);
    }

    // Background: Existing transform code does not do the right thing in
    // MapRect / MapQuad / ProjectQuad when there is a perspective projection that
    // causes one of the transformed vertices to go to w < 0. In those cases, it
    // is necessary to perform clipping in homogeneous coordinates, after applying
    // the transform, before dividing-by-w to convert to cartesian coordinates.
    //
    // These functions return the axis-aligned rect that encloses the correctly
    // clipped, transformed polygon.
    static gfx::Rect MapEnclosingClippedRect(const gfx::Transform& transform,
        const gfx::Rect& rect);
    static gfx::RectF MapClippedRect(const gfx::Transform& transform,
        const gfx::RectF& rect);
    static gfx::Rect ProjectEnclosingClippedRect(const gfx::Transform& transform,
        const gfx::Rect& rect);
    static gfx::RectF ProjectClippedRect(const gfx::Transform& transform,
        const gfx::RectF& rect);

    // This function is only valid when the transform preserves 2d axis
    // alignment and the resulting rect will not be clipped.
    static gfx::Rect MapEnclosedRectWith2dAxisAlignedTransform(
        const gfx::Transform& transform,
        const gfx::Rect& rect);

    // Returns an array of vertices that represent the clipped polygon. After
    // returning, indexes from 0 to num_vertices_in_clipped_quad are valid in the
    // clipped_quad array. Note that num_vertices_in_clipped_quad may be zero,
    // which means the entire quad was clipped, and none of the vertices in the
    // array are valid.
    static void MapClippedQuad(const gfx::Transform& transform,
        const gfx::QuadF& src_quad,
        gfx::PointF clipped_quad[8],
        int* num_vertices_in_clipped_quad);
    static bool MapClippedQuad3d(const gfx::Transform& transform,
        const gfx::QuadF& src_quad,
        gfx::Point3F clipped_quad[8],
        int* num_vertices_in_clipped_quad);

    static gfx::RectF ComputeEnclosingRectOfVertices(const gfx::PointF vertices[],
        int num_vertices);
    static gfx::RectF ComputeEnclosingClippedRect(
        const HomogeneousCoordinate& h1,
        const HomogeneousCoordinate& h2,
        const HomogeneousCoordinate& h3,
        const HomogeneousCoordinate& h4);

    // NOTE: These functions do not do correct clipping against w = 0 plane, but
    // they correctly detect the clipped condition via the boolean clipped.
    static gfx::QuadF MapQuad(const gfx::Transform& transform,
        const gfx::QuadF& quad,
        bool* clipped);
    static gfx::QuadF MapQuad3d(const gfx::Transform& transform,
        const gfx::QuadF& q,
        gfx::Point3F* p,
        bool* clipped);
    static gfx::PointF MapPoint(const gfx::Transform& transform,
        const gfx::PointF& point,
        bool* clipped);
    static gfx::Point3F MapPoint(const gfx::Transform&,
        const gfx::Point3F&,
        bool* clipped);
    static gfx::QuadF ProjectQuad(const gfx::Transform& transform,
        const gfx::QuadF& quad,
        bool* clipped);
    static gfx::PointF ProjectPoint(const gfx::Transform& transform,
        const gfx::PointF& point,
        bool* clipped);
    // Identical to the above function, but coerces the homogeneous coordinate to
    // a 3d rather than a 2d point.
    static gfx::Point3F ProjectPoint3D(const gfx::Transform& transform,
        const gfx::PointF& point,
        bool* clipped);

    static gfx::Vector2dF ComputeTransform2dScaleComponents(const gfx::Transform&,
        float fallbackValue);

    // Makes a rect that has the same relationship to input_outer_rect as
    // scale_inner_rect has to scale_outer_rect. scale_inner_rect should be
    // contained within scale_outer_rect, and likewise the rectangle that is
    // returned will be within input_outer_rect at a similar relative, scaled
    // position.
    static gfx::RectF ScaleRectProportional(const gfx::RectF& input_outer_rect,
        const gfx::RectF& scale_outer_rect,
        const gfx::RectF& scale_inner_rect);

    // Returns the smallest angle between the given two vectors in degrees.
    // Neither vector is assumed to be normalized.
    static float SmallestAngleBetweenVectors(const gfx::Vector2dF& v1,
        const gfx::Vector2dF& v2);

    // Projects the |source| vector onto |destination|. Neither vector is assumed
    // to be normalized.
    static gfx::Vector2dF ProjectVector(const gfx::Vector2dF& source,
        const gfx::Vector2dF& destination);

    // Conversion to value.
    static scoped_ptr<base::Value> AsValue(const gfx::Size& s);
    static scoped_ptr<base::Value> AsValue(const gfx::Rect& r);
    static bool FromValue(const base::Value*, gfx::Rect* out_rect);
    static scoped_ptr<base::Value> AsValue(const gfx::PointF& q);

    static void AddToTracedValue(const char* name,
        const gfx::Size& s,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::SizeF& s,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::Rect& r,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::PointF& q,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::Point3F&,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::Vector2d& v,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::Vector2dF& v,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::ScrollOffset& v,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::QuadF& q,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::RectF& rect,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::Transform& transform,
        base::trace_event::TracedValue* res);
    static void AddToTracedValue(const char* name,
        const gfx::BoxF& box,
        base::trace_event::TracedValue* res);

    // Returns a base::Value representation of the floating point value.
    // If the value is inf, returns max double/float representation.
    static double AsDoubleSafely(double value);
    static float AsFloatSafely(float value);

    // Returns vector that x axis (1,0,0) transforms to under given transform.
    static gfx::Vector3dF GetXAxis(const gfx::Transform& transform);

    // Returns vector that y axis (0,1,0) transforms to under given transform.
    static gfx::Vector3dF GetYAxis(const gfx::Transform& transform);

private:
    template <typename T>
    static T RoundUpInternal(T n, T mul)
    {
        return (n > 0) ? ((n + mul - 1) / mul) * mul : (n / mul) * mul;
    }

    template <typename T>
    static T RoundDownInternal(T n, T mul)
    {
        return (n > 0) ? (n / mul) * mul : (n == 0) ? 0 : ((n - mul + 1) / mul) * mul;
    }
};

} // namespace cc

#endif // CC_BASE_MATH_UTIL_H_
