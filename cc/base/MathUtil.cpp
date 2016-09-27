
#include "cc/base/MathUtil.h"

using namespace blink;

namespace cc {

static HomogeneousCoordinate MapHomogeneousPoint(const SkMatrix44& transform, const FloatPoint3D& p)
{
    HomogeneousCoordinate result(p.x(), p.y(), p.z(), 1.0);
    transform.mapMScalars(result.vec, result.vec);
    return result;
}

static inline void AddVertexToClippedQuad(const FloatPoint& new_vertex,
    FloatPoint clipped_quad[8],
    int* num_vertices_in_clipped_quad)
{
    clipped_quad[*num_vertices_in_clipped_quad] = new_vertex;
    (*num_vertices_in_clipped_quad)++;
}

static HomogeneousCoordinate ComputeClippedPointForEdge(
    const HomogeneousCoordinate& h1,
    const HomogeneousCoordinate& h2)
{
    // Points h1 and h2 form a line in 4d, and any point on that line can be
    // represented as an interpolation between h1 and h2:
    //    p = (1-t) h1 + (t) h2
    //
    // We want to compute point p such that p.w == epsilon, where epsilon is a
    // small non-zero number. (but the smaller the number is, the higher the risk
    // of overflow)
    // To do this, we solve for t in the following equation:
    //    p.w = epsilon = (1-t) * h1.w + (t) * h2.w
    //
    // Once paramter t is known, the rest of p can be computed via
    //    p = (1-t) h1 + (t) h2.

    // Technically this is a special case of the following assertion, but its a
    // good idea to keep it an explicit sanity check here.
    ASSERT(h2.w() != h1.w());
    // Exactly one of h1 or h2 (but not both) must be on the negative side of the
    // w plane when this is called.
    ASSERT(h1.ShouldBeClipped() ^ h2.ShouldBeClipped());

    // ...or any positive non-zero small epsilon
    SkMScalar w = 0.00001f;
    SkMScalar t = (w - h1.w()) / (h2.w() - h1.w());

    SkMScalar x = (SK_MScalar1 - t) * h1.x() + t * h2.x();
    SkMScalar y = (SK_MScalar1 - t) * h1.y() + t * h2.y();
    SkMScalar z = (SK_MScalar1 - t) * h1.z() + t * h2.z();

    return HomogeneousCoordinate(x, y, z, w);
}

void MathUtil::MapClippedQuad(const SkMatrix44& transform,
    const FloatQuad& src_quad,
    FloatPoint clipped_quad[8],
    int* num_vertices_in_clipped_quad)
{
    HomogeneousCoordinate h1 =
        MapHomogeneousPoint(transform, FloatPoint3D(src_quad.p1()));
    HomogeneousCoordinate h2 =
        MapHomogeneousPoint(transform, FloatPoint3D(src_quad.p2()));
    HomogeneousCoordinate h3 =
        MapHomogeneousPoint(transform, FloatPoint3D(src_quad.p3()));
    HomogeneousCoordinate h4 =
        MapHomogeneousPoint(transform, FloatPoint3D(src_quad.p4()));

    // The order of adding the vertices to the array is chosen so that
    // clockwise / counter-clockwise orientation is retained.

    *num_vertices_in_clipped_quad = 0;

    if (!h1.ShouldBeClipped()) {
        AddVertexToClippedQuad(h1.CartesianPoint2d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h1.ShouldBeClipped() ^ h2.ShouldBeClipped()) {
        AddVertexToClippedQuad(
            ComputeClippedPointForEdge(h1, h2).CartesianPoint2d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    if (!h2.ShouldBeClipped()) {
        AddVertexToClippedQuad(
            h2.CartesianPoint2d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h2.ShouldBeClipped() ^ h3.ShouldBeClipped()) {
        AddVertexToClippedQuad(
            ComputeClippedPointForEdge(h2, h3).CartesianPoint2d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    if (!h3.ShouldBeClipped()) {
        AddVertexToClippedQuad(h3.CartesianPoint2d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h3.ShouldBeClipped() ^ h4.ShouldBeClipped()) {
        AddVertexToClippedQuad(
            ComputeClippedPointForEdge(h3, h4).CartesianPoint2d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    if (!h4.ShouldBeClipped()) {
        AddVertexToClippedQuad(h4.CartesianPoint2d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h4.ShouldBeClipped() ^ h1.ShouldBeClipped()) {
        AddVertexToClippedQuad(
            ComputeClippedPointForEdge(h4, h1).CartesianPoint2d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    ASSERT(*num_vertices_in_clipped_quad <= 8);
}

FloatPoint3D MathUtil::MapPoint(const SkMatrix44& transform, const FloatPoint3D& p, bool* clipped)
{
    HomogeneousCoordinate h = MapHomogeneousPoint(transform, p);

    if (h.w() > 0) {
        *clipped = false;
        return h.CartesianPoint3d();
    }

    // The cartesian coordinates will be invalid after dividing by w.
    *clipped = true;

    // Avoid dividing by w if w == 0.
    if (!h.w())
        return FloatPoint3D();

    // This return value will be invalid because clipped == true, but (1) users of
    // this code should be ignoring the return value when clipped == true anyway,
    // and (2) this behavior is more consistent with existing behavior of WebKit
    // transforms if the user really does not ignore the return value.
    return h.CartesianPoint3d();
}

static inline void ExpandBoundsToIncludePoint(float* xmin,
    float* xmax,
    float* ymin,
    float* ymax,
    const FloatPoint& p)
{
    *xmin = std::min(p.x(), *xmin);
    *xmax = std::max(p.x(), *xmax);
    *ymin = std::min(p.y(), *ymin);
    *ymax = std::max(p.y(), *ymax);
}

FloatRect MathUtil::ComputeEnclosingRectOfVertices(const FloatPoint vertices[], int num_vertices)
{
    if (num_vertices < 2)
        return FloatRect();

    float xmin = std::numeric_limits<float>::max();
    float xmax = -std::numeric_limits<float>::max();
    float ymin = std::numeric_limits<float>::max();
    float ymax = -std::numeric_limits<float>::max();

    for (int i = 0; i < num_vertices; ++i)
        ExpandBoundsToIncludePoint(&xmin, &xmax, &ymin, &ymax, vertices[i]);

    return FloatRect(FloatPoint(xmin, ymin), FloatSize(xmax - xmin, ymax - ymin));
}

}