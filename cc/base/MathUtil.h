#ifndef CC_BASE_MATH_UTIL_H_
#define CC_BASE_MATH_UTIL_H_

#include "third_party/skia/include/utils/SkMatrix44.h"
#include "third_party/WebKit/Source/platform/geometry/FloatRect.h"
#include "third_party/WebKit/Source/platform/geometry/FloatQuad.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint3D.h"

namespace cc {

struct HomogeneousCoordinate {
    HomogeneousCoordinate(SkMScalar x, SkMScalar y, SkMScalar z, SkMScalar w) {
        vec[0] = x;
        vec[1] = y;
        vec[2] = z;
        vec[3] = w;
    }

    bool ShouldBeClipped() const { return w() <= 0.0; }

    blink::FloatPoint CartesianPoint2d() const {
        if (w() == SK_MScalar1)
            return blink::FloatPoint(x(), y());

        // For now, because this code is used privately only by MathUtil, it should
        // never be called when w == 0, and we do not yet need to handle that case.
        ASSERT(w());
        SkMScalar inv_w = SK_MScalar1 / w();
        return blink::FloatPoint(x() * inv_w, y() * inv_w);
    }

    blink::FloatPoint3D CartesianPoint3d() const {
        if (w() == SK_MScalar1)
            return blink::FloatPoint3D(x(), y(), z());

        // For now, because this code is used privately only by MathUtil, it should
        // never be called when w == 0, and we do not yet need to handle that case.
        ASSERT(w());
        SkMScalar inv_w = SK_MScalar1 / w();
        return blink::FloatPoint3D(x() * inv_w, y() * inv_w, z() * inv_w);
    }

    SkMScalar x() const { return vec[0]; }
    SkMScalar y() const { return vec[1]; }
    SkMScalar z() const { return vec[2]; }
    SkMScalar w() const { return vec[3]; }

    SkMScalar vec[4];
};

class MathUtil {
public:
    static const double kPiDouble;
    static const float kPiFloat;

    static double Deg2Rad(double deg) { return deg * kPiDouble / 180.0; }
    static double Rad2Deg(double rad) { return rad * 180.0 / kPiDouble; }

    static float Deg2Rad(float deg) { return deg * kPiFloat / 180.0f; }
    static float Rad2Deg(float rad) { return rad * 180.0f / kPiFloat; }

    static float Round(float f) {
        return (f > 0.f) ? std::floor(f + 0.5f) : std::ceil(f - 0.5f);
    }
    static double Round(double d) {
        return (d > 0.0) ? std::floor(d + 0.5) : std::ceil(d - 0.5);
    }

    template <typename T> static T ClampToRange(T value, T min, T max) {
        return std::min(std::max(value, min), max);
    }

    // Returns an array of vertices that represent the clipped polygon. After
    // returning, indexes from 0 to num_vertices_in_clipped_quad are valid in the
    // clipped_quad array. Note that num_vertices_in_clipped_quad may be zero,
    // which means the entire quad was clipped, and none of the vertices in the
    // array are valid.
    static void MapClippedQuad(const SkMatrix44& transform,
        const blink::FloatQuad& src_quad,
        blink::FloatPoint clipped_quad[8],
        int* num_vertices_in_clipped_quad);

    static blink::FloatRect ComputeEnclosingRectOfVertices(const blink::FloatPoint vertices[], int num_vertices);

    static double DotProduct(const blink::FloatPoint& lhs, const blink::FloatPoint& rhs)
    {
        return static_cast<double>(lhs.x()) * rhs.x() + static_cast<double>(lhs.y()) * rhs.y();
    }

    static float DotProduct(const blink::FloatPoint3D& lhs, const blink::FloatPoint3D& rhs)
    {
        return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
    }

    // Return the cross product of two vectors.
    static blink::FloatPoint3D CrossProduct(const blink::FloatPoint3D& lhs, const blink::FloatPoint3D& rhs)
    {
        blink::FloatPoint3D result = lhs;
        return result.cross(rhs);
    }

    static blink::FloatPoint3D MapPoint(const SkMatrix44& transform, const blink::FloatPoint3D& p, bool* clipped);

    static blink::FloatRect ComputeEnclosingClippedRect(
        const HomogeneousCoordinate& h1, const HomogeneousCoordinate& h2,
        const HomogeneousCoordinate& h3, const HomogeneousCoordinate& h4);
    static blink::FloatRect MapClippedRect(const SkMatrix44& transform, const blink::FloatRect& srcRect);
};

}

#endif