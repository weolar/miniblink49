// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/math_util.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"
#include "ui/gfx/geometry/quad_f.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/vector2d_f.h"
#include "ui/gfx/geometry/vector3d_f.h"
#include "ui/gfx/transform.h"

namespace cc {

const double MathUtil::kPiDouble = 3.14159265358979323846;
const float MathUtil::kPiFloat = 3.14159265358979323846f;

static HomogeneousCoordinate ProjectHomogeneousPoint(
    const gfx::Transform& transform,
    const gfx::PointF& p)
{
    SkMScalar z = -(transform.matrix().get(2, 0) * p.x() + transform.matrix().get(2, 1) * p.y() + transform.matrix().get(2, 3)) / transform.matrix().get(2, 2);

    // In this case, the layer we are trying to project onto is perpendicular to
    // ray (point p and z-axis direction) that we are trying to project. This
    // happens when the layer is rotated so that it is infinitesimally thin, or
    // when it is co-planar with the camera origin -- i.e. when the layer is
    // invisible anyway.
    if (!std::isfinite(z))
        return HomogeneousCoordinate(0.0, 0.0, 0.0, 1.0);

    HomogeneousCoordinate result(p.x(), p.y(), z, 1.0);
    transform.matrix().mapMScalars(result.vec, result.vec);
    return result;
}

static HomogeneousCoordinate ProjectHomogeneousPoint(
    const gfx::Transform& transform,
    const gfx::PointF& p,
    bool* clipped)
{
    HomogeneousCoordinate h = ProjectHomogeneousPoint(transform, p);
    *clipped = h.w() <= 0;
    return h;
}

static HomogeneousCoordinate MapHomogeneousPoint(
    const gfx::Transform& transform,
    const gfx::Point3F& p)
{
    HomogeneousCoordinate result(p.x(), p.y(), p.z(), 1.0);
    transform.matrix().mapMScalars(result.vec, result.vec);
    return result;
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
    DCHECK_NE(h2.w(), h1.w());
    // Exactly one of h1 or h2 (but not both) must be on the negative side of the
    // w plane when this is called.
    DCHECK(h1.ShouldBeClipped() ^ h2.ShouldBeClipped());

    // ...or any positive non-zero small epsilon
    SkMScalar w = 0.00001f;
    SkMScalar t = (w - h1.w()) / (h2.w() - h1.w());

    SkMScalar x = (SK_MScalar1 - t) * h1.x() + t * h2.x();
    SkMScalar y = (SK_MScalar1 - t) * h1.y() + t * h2.y();
    SkMScalar z = (SK_MScalar1 - t) * h1.z() + t * h2.z();

    return HomogeneousCoordinate(x, y, z, w);
}

static inline void ExpandBoundsToIncludePoint(float* xmin,
    float* xmax,
    float* ymin,
    float* ymax,
    const gfx::PointF& p)
{
    *xmin = std::min(p.x(), *xmin);
    *xmax = std::max(p.x(), *xmax);
    *ymin = std::min(p.y(), *ymin);
    *ymax = std::max(p.y(), *ymax);
}

static inline void AddVertexToClippedQuad(const gfx::PointF& new_vertex,
    gfx::PointF clipped_quad[8],
    int* num_vertices_in_clipped_quad)
{
    clipped_quad[*num_vertices_in_clipped_quad] = new_vertex;
    (*num_vertices_in_clipped_quad)++;
}

static inline void AddVertexToClippedQuad3d(const gfx::Point3F& new_vertex,
    gfx::Point3F clipped_quad[8],
    int* num_vertices_in_clipped_quad)
{
    clipped_quad[*num_vertices_in_clipped_quad] = new_vertex;
    (*num_vertices_in_clipped_quad)++;
}

gfx::Rect MathUtil::MapEnclosingClippedRect(const gfx::Transform& transform,
    const gfx::Rect& src_rect)
{
    if (transform.IsIdentityOrIntegerTranslation()) {
        gfx::Vector2d offset(static_cast<int>(transform.matrix().getFloat(0, 3)),
            static_cast<int>(transform.matrix().getFloat(1, 3)));
        return src_rect + offset;
    }
    gfx::RectF mapped_rect = MapClippedRect(transform, gfx::RectF(src_rect));

    // gfx::ToEnclosingRect crashes if called on a RectF with any NaN coordinate.
    if (std::isnan(mapped_rect.x()) || std::isnan(mapped_rect.y()) || std::isnan(mapped_rect.right()) || std::isnan(mapped_rect.bottom()))
        return gfx::Rect();

    return gfx::ToEnclosingRect(mapped_rect);
}

gfx::RectF MathUtil::MapClippedRect(const gfx::Transform& transform,
    const gfx::RectF& src_rect)
{
    if (transform.IsIdentityOrTranslation()) {
        gfx::Vector2dF offset(transform.matrix().getFloat(0, 3),
            transform.matrix().getFloat(1, 3));
        return src_rect + offset;
    }

    // Apply the transform, but retain the result in homogeneous coordinates.

    SkMScalar quad[4 * 2]; // input: 4 x 2D points
    quad[0] = src_rect.x();
    quad[1] = src_rect.y();
    quad[2] = src_rect.right();
    quad[3] = src_rect.y();
    quad[4] = src_rect.right();
    quad[5] = src_rect.bottom();
    quad[6] = src_rect.x();
    quad[7] = src_rect.bottom();

    SkMScalar result[4 * 4]; // output: 4 x 4D homogeneous points
    transform.matrix().map2(quad, 4, result);

    HomogeneousCoordinate hc0(result[0], result[1], result[2], result[3]);
    HomogeneousCoordinate hc1(result[4], result[5], result[6], result[7]);
    HomogeneousCoordinate hc2(result[8], result[9], result[10], result[11]);
    HomogeneousCoordinate hc3(result[12], result[13], result[14], result[15]);
    return ComputeEnclosingClippedRect(hc0, hc1, hc2, hc3);
}

gfx::Rect MathUtil::ProjectEnclosingClippedRect(const gfx::Transform& transform,
    const gfx::Rect& src_rect)
{
    if (transform.IsIdentityOrIntegerTranslation()) {
        gfx::Vector2d offset(static_cast<int>(transform.matrix().getFloat(0, 3)),
            static_cast<int>(transform.matrix().getFloat(1, 3)));
        return src_rect + offset;
    }
    gfx::RectF projected_rect = ProjectClippedRect(transform, gfx::RectF(src_rect));

    // gfx::ToEnclosingRect crashes if called on a RectF with any NaN coordinate.
    if (std::isnan(projected_rect.x()) || std::isnan(projected_rect.y()) || std::isnan(projected_rect.right()) || std::isnan(projected_rect.bottom()))
        return gfx::Rect();

    return gfx::ToEnclosingRect(projected_rect);
}

gfx::RectF MathUtil::ProjectClippedRect(const gfx::Transform& transform,
    const gfx::RectF& src_rect)
{
    if (transform.IsIdentityOrTranslation()) {
        gfx::Vector2dF offset(transform.matrix().getFloat(0, 3),
            transform.matrix().getFloat(1, 3));
        return src_rect + offset;
    }

    // Perform the projection, but retain the result in homogeneous coordinates.
    gfx::QuadF q = gfx::QuadF(src_rect);
    HomogeneousCoordinate h1 = ProjectHomogeneousPoint(transform, q.p1());
    HomogeneousCoordinate h2 = ProjectHomogeneousPoint(transform, q.p2());
    HomogeneousCoordinate h3 = ProjectHomogeneousPoint(transform, q.p3());
    HomogeneousCoordinate h4 = ProjectHomogeneousPoint(transform, q.p4());

    return ComputeEnclosingClippedRect(h1, h2, h3, h4);
}

gfx::Rect MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(
    const gfx::Transform& transform,
    const gfx::Rect& rect)
{
    DCHECK(transform.Preserves2dAxisAlignment());

    if (transform.IsIdentityOrIntegerTranslation()) {
        gfx::Vector2d offset(static_cast<int>(transform.matrix().getFloat(0, 3)),
            static_cast<int>(transform.matrix().getFloat(1, 3)));
        return rect + offset;
    }
    if (transform.IsIdentityOrTranslation()) {
        gfx::Vector2dF offset(transform.matrix().getFloat(0, 3),
            transform.matrix().getFloat(1, 3));
        return gfx::ToEnclosedRect(gfx::RectF(rect) + offset);
    }

    SkMScalar quad[2 * 2]; // input: 2 x 2D points
    quad[0] = rect.x();
    quad[1] = rect.y();
    quad[2] = rect.right();
    quad[3] = rect.bottom();

    SkMScalar result[4 * 2]; // output: 2 x 4D homogeneous points
    transform.matrix().map2(quad, 2, result);

    HomogeneousCoordinate hc0(result[0], result[1], result[2], result[3]);
    HomogeneousCoordinate hc1(result[4], result[5], result[6], result[7]);
    DCHECK(!hc0.ShouldBeClipped());
    DCHECK(!hc1.ShouldBeClipped());

    gfx::PointF top_left(hc0.CartesianPoint2d());
    gfx::PointF bottom_right(hc1.CartesianPoint2d());
    return gfx::ToEnclosedRect(gfx::BoundingRect(top_left, bottom_right));
}

void MathUtil::MapClippedQuad(const gfx::Transform& transform,
    const gfx::QuadF& src_quad,
    gfx::PointF clipped_quad[8],
    int* num_vertices_in_clipped_quad)
{
    HomogeneousCoordinate h1 = MapHomogeneousPoint(transform, gfx::Point3F(src_quad.p1()));
    HomogeneousCoordinate h2 = MapHomogeneousPoint(transform, gfx::Point3F(src_quad.p2()));
    HomogeneousCoordinate h3 = MapHomogeneousPoint(transform, gfx::Point3F(src_quad.p3()));
    HomogeneousCoordinate h4 = MapHomogeneousPoint(transform, gfx::Point3F(src_quad.p4()));

    // The order of adding the vertices to the array is chosen so that
    // clockwise / counter-clockwise orientation is retained.

    *num_vertices_in_clipped_quad = 0;

    if (!h1.ShouldBeClipped()) {
        AddVertexToClippedQuad(
            h1.CartesianPoint2d(), clipped_quad, num_vertices_in_clipped_quad);
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
        AddVertexToClippedQuad(
            h3.CartesianPoint2d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h3.ShouldBeClipped() ^ h4.ShouldBeClipped()) {
        AddVertexToClippedQuad(
            ComputeClippedPointForEdge(h3, h4).CartesianPoint2d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    if (!h4.ShouldBeClipped()) {
        AddVertexToClippedQuad(
            h4.CartesianPoint2d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h4.ShouldBeClipped() ^ h1.ShouldBeClipped()) {
        AddVertexToClippedQuad(
            ComputeClippedPointForEdge(h4, h1).CartesianPoint2d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    DCHECK_LE(*num_vertices_in_clipped_quad, 8);
}

bool MathUtil::MapClippedQuad3d(const gfx::Transform& transform,
    const gfx::QuadF& src_quad,
    gfx::Point3F clipped_quad[8],
    int* num_vertices_in_clipped_quad)
{
    HomogeneousCoordinate h1 = MapHomogeneousPoint(transform, gfx::Point3F(src_quad.p1()));
    HomogeneousCoordinate h2 = MapHomogeneousPoint(transform, gfx::Point3F(src_quad.p2()));
    HomogeneousCoordinate h3 = MapHomogeneousPoint(transform, gfx::Point3F(src_quad.p3()));
    HomogeneousCoordinate h4 = MapHomogeneousPoint(transform, gfx::Point3F(src_quad.p4()));

    // The order of adding the vertices to the array is chosen so that
    // clockwise / counter-clockwise orientation is retained.

    *num_vertices_in_clipped_quad = 0;

    if (!h1.ShouldBeClipped()) {
        AddVertexToClippedQuad3d(
            h1.CartesianPoint3d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h1.ShouldBeClipped() ^ h2.ShouldBeClipped()) {
        AddVertexToClippedQuad3d(
            ComputeClippedPointForEdge(h1, h2).CartesianPoint3d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    if (!h2.ShouldBeClipped()) {
        AddVertexToClippedQuad3d(
            h2.CartesianPoint3d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h2.ShouldBeClipped() ^ h3.ShouldBeClipped()) {
        AddVertexToClippedQuad3d(
            ComputeClippedPointForEdge(h2, h3).CartesianPoint3d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    if (!h3.ShouldBeClipped()) {
        AddVertexToClippedQuad3d(
            h3.CartesianPoint3d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h3.ShouldBeClipped() ^ h4.ShouldBeClipped()) {
        AddVertexToClippedQuad3d(
            ComputeClippedPointForEdge(h3, h4).CartesianPoint3d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    if (!h4.ShouldBeClipped()) {
        AddVertexToClippedQuad3d(
            h4.CartesianPoint3d(), clipped_quad, num_vertices_in_clipped_quad);
    }

    if (h4.ShouldBeClipped() ^ h1.ShouldBeClipped()) {
        AddVertexToClippedQuad3d(
            ComputeClippedPointForEdge(h4, h1).CartesianPoint3d(),
            clipped_quad,
            num_vertices_in_clipped_quad);
    }

    DCHECK_LE(*num_vertices_in_clipped_quad, 8);
    return (*num_vertices_in_clipped_quad >= 4);
}

gfx::RectF MathUtil::ComputeEnclosingRectOfVertices(
    const gfx::PointF vertices[],
    int num_vertices)
{
    if (num_vertices < 2)
        return gfx::RectF();

    float xmin = std::numeric_limits<float>::max();
    float xmax = -std::numeric_limits<float>::max();
    float ymin = std::numeric_limits<float>::max();
    float ymax = -std::numeric_limits<float>::max();

    for (int i = 0; i < num_vertices; ++i)
        ExpandBoundsToIncludePoint(&xmin, &xmax, &ymin, &ymax, vertices[i]);

    return gfx::RectF(gfx::PointF(xmin, ymin),
        gfx::SizeF(xmax - xmin, ymax - ymin));
}

gfx::RectF MathUtil::ComputeEnclosingClippedRect(
    const HomogeneousCoordinate& h1,
    const HomogeneousCoordinate& h2,
    const HomogeneousCoordinate& h3,
    const HomogeneousCoordinate& h4)
{
    // This function performs clipping as necessary and computes the enclosing 2d
    // gfx::RectF of the vertices. Doing these two steps simultaneously allows us
    // to avoid the overhead of storing an unknown number of clipped vertices.

    // If no vertices on the quad are clipped, then we can simply return the
    // enclosing rect directly.
    bool something_clipped = h1.ShouldBeClipped() || h2.ShouldBeClipped() || h3.ShouldBeClipped() || h4.ShouldBeClipped();
    if (!something_clipped) {
        gfx::QuadF mapped_quad = gfx::QuadF(h1.CartesianPoint2d(),
            h2.CartesianPoint2d(),
            h3.CartesianPoint2d(),
            h4.CartesianPoint2d());
        return mapped_quad.BoundingBox();
    }

    bool everything_clipped = h1.ShouldBeClipped() && h2.ShouldBeClipped() && h3.ShouldBeClipped() && h4.ShouldBeClipped();
    if (everything_clipped)
        return gfx::RectF();

    float xmin = std::numeric_limits<float>::max();
    float xmax = -std::numeric_limits<float>::max();
    float ymin = std::numeric_limits<float>::max();
    float ymax = -std::numeric_limits<float>::max();

    if (!h1.ShouldBeClipped())
        ExpandBoundsToIncludePoint(&xmin, &xmax, &ymin, &ymax,
            h1.CartesianPoint2d());

    if (h1.ShouldBeClipped() ^ h2.ShouldBeClipped())
        ExpandBoundsToIncludePoint(&xmin,
            &xmax,
            &ymin,
            &ymax,
            ComputeClippedPointForEdge(h1, h2)
                .CartesianPoint2d());

    if (!h2.ShouldBeClipped())
        ExpandBoundsToIncludePoint(&xmin, &xmax, &ymin, &ymax,
            h2.CartesianPoint2d());

    if (h2.ShouldBeClipped() ^ h3.ShouldBeClipped())
        ExpandBoundsToIncludePoint(&xmin,
            &xmax,
            &ymin,
            &ymax,
            ComputeClippedPointForEdge(h2, h3)
                .CartesianPoint2d());

    if (!h3.ShouldBeClipped())
        ExpandBoundsToIncludePoint(&xmin, &xmax, &ymin, &ymax,
            h3.CartesianPoint2d());

    if (h3.ShouldBeClipped() ^ h4.ShouldBeClipped())
        ExpandBoundsToIncludePoint(&xmin,
            &xmax,
            &ymin,
            &ymax,
            ComputeClippedPointForEdge(h3, h4)
                .CartesianPoint2d());

    if (!h4.ShouldBeClipped())
        ExpandBoundsToIncludePoint(&xmin, &xmax, &ymin, &ymax,
            h4.CartesianPoint2d());

    if (h4.ShouldBeClipped() ^ h1.ShouldBeClipped())
        ExpandBoundsToIncludePoint(&xmin,
            &xmax,
            &ymin,
            &ymax,
            ComputeClippedPointForEdge(h4, h1)
                .CartesianPoint2d());

    return gfx::RectF(gfx::PointF(xmin, ymin),
        gfx::SizeF(xmax - xmin, ymax - ymin));
}

gfx::QuadF MathUtil::MapQuad(const gfx::Transform& transform,
    const gfx::QuadF& q,
    bool* clipped)
{
    if (transform.IsIdentityOrTranslation()) {
        gfx::QuadF mapped_quad(q);
        mapped_quad += gfx::Vector2dF(transform.matrix().getFloat(0, 3),
            transform.matrix().getFloat(1, 3));
        *clipped = false;
        return mapped_quad;
    }

    HomogeneousCoordinate h1 = MapHomogeneousPoint(transform, gfx::Point3F(q.p1()));
    HomogeneousCoordinate h2 = MapHomogeneousPoint(transform, gfx::Point3F(q.p2()));
    HomogeneousCoordinate h3 = MapHomogeneousPoint(transform, gfx::Point3F(q.p3()));
    HomogeneousCoordinate h4 = MapHomogeneousPoint(transform, gfx::Point3F(q.p4()));

    *clipped = h1.ShouldBeClipped() || h2.ShouldBeClipped() || h3.ShouldBeClipped() || h4.ShouldBeClipped();

    // Result will be invalid if clipped == true. But, compute it anyway just in
    // case, to emulate existing behavior.
    return gfx::QuadF(h1.CartesianPoint2d(),
        h2.CartesianPoint2d(),
        h3.CartesianPoint2d(),
        h4.CartesianPoint2d());
}

gfx::QuadF MathUtil::MapQuad3d(const gfx::Transform& transform,
    const gfx::QuadF& q,
    gfx::Point3F* p,
    bool* clipped)
{
    if (transform.IsIdentityOrTranslation()) {
        gfx::QuadF mapped_quad(q);
        mapped_quad += gfx::Vector2dF(transform.matrix().getFloat(0, 3),
            transform.matrix().getFloat(1, 3));
        *clipped = false;
        p[0] = gfx::Point3F(mapped_quad.p1().x(), mapped_quad.p1().y(), 0.0f);
        p[1] = gfx::Point3F(mapped_quad.p2().x(), mapped_quad.p2().y(), 0.0f);
        p[2] = gfx::Point3F(mapped_quad.p3().x(), mapped_quad.p3().y(), 0.0f);
        p[3] = gfx::Point3F(mapped_quad.p4().x(), mapped_quad.p4().y(), 0.0f);
        return mapped_quad;
    }

    HomogeneousCoordinate h1 = MapHomogeneousPoint(transform, gfx::Point3F(q.p1()));
    HomogeneousCoordinate h2 = MapHomogeneousPoint(transform, gfx::Point3F(q.p2()));
    HomogeneousCoordinate h3 = MapHomogeneousPoint(transform, gfx::Point3F(q.p3()));
    HomogeneousCoordinate h4 = MapHomogeneousPoint(transform, gfx::Point3F(q.p4()));

    *clipped = h1.ShouldBeClipped() || h2.ShouldBeClipped() || h3.ShouldBeClipped() || h4.ShouldBeClipped();

    // Result will be invalid if clipped == true. But, compute it anyway just in
    // case, to emulate existing behavior.
    p[0] = h1.CartesianPoint3d();
    p[1] = h2.CartesianPoint3d();
    p[2] = h3.CartesianPoint3d();
    p[3] = h4.CartesianPoint3d();

    return gfx::QuadF(h1.CartesianPoint2d(),
        h2.CartesianPoint2d(),
        h3.CartesianPoint2d(),
        h4.CartesianPoint2d());
}

gfx::PointF MathUtil::MapPoint(const gfx::Transform& transform,
    const gfx::PointF& p,
    bool* clipped)
{
    HomogeneousCoordinate h = MapHomogeneousPoint(transform, gfx::Point3F(p));

    if (h.w() > 0) {
        *clipped = false;
        return h.CartesianPoint2d();
    }

    // The cartesian coordinates will be invalid after dividing by w.
    *clipped = true;

    // Avoid dividing by w if w == 0.
    if (!h.w())
        return gfx::PointF();

    // This return value will be invalid because clipped == true, but (1) users of
    // this code should be ignoring the return value when clipped == true anyway,
    // and (2) this behavior is more consistent with existing behavior of WebKit
    // transforms if the user really does not ignore the return value.
    return h.CartesianPoint2d();
}

gfx::Point3F MathUtil::MapPoint(const gfx::Transform& transform,
    const gfx::Point3F& p,
    bool* clipped)
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
        return gfx::Point3F();

    // This return value will be invalid because clipped == true, but (1) users of
    // this code should be ignoring the return value when clipped == true anyway,
    // and (2) this behavior is more consistent with existing behavior of WebKit
    // transforms if the user really does not ignore the return value.
    return h.CartesianPoint3d();
}

gfx::QuadF MathUtil::ProjectQuad(const gfx::Transform& transform,
    const gfx::QuadF& q,
    bool* clipped)
{
    gfx::QuadF projected_quad;
    bool clipped_point;
    projected_quad.set_p1(ProjectPoint(transform, q.p1(), &clipped_point));
    *clipped = clipped_point;
    projected_quad.set_p2(ProjectPoint(transform, q.p2(), &clipped_point));
    *clipped |= clipped_point;
    projected_quad.set_p3(ProjectPoint(transform, q.p3(), &clipped_point));
    *clipped |= clipped_point;
    projected_quad.set_p4(ProjectPoint(transform, q.p4(), &clipped_point));
    *clipped |= clipped_point;

    return projected_quad;
}

gfx::PointF MathUtil::ProjectPoint(const gfx::Transform& transform,
    const gfx::PointF& p,
    bool* clipped)
{
    HomogeneousCoordinate h = ProjectHomogeneousPoint(transform, p, clipped);
    // Avoid dividing by w if w == 0.
    if (!h.w())
        return gfx::PointF();

    // This return value will be invalid if clipped == true, but (1) users of
    // this code should be ignoring the return value when clipped == true anyway,
    // and (2) this behavior is more consistent with existing behavior of WebKit
    // transforms if the user really does not ignore the return value.
    return h.CartesianPoint2d();
}

gfx::Point3F MathUtil::ProjectPoint3D(const gfx::Transform& transform,
    const gfx::PointF& p,
    bool* clipped)
{
    HomogeneousCoordinate h = ProjectHomogeneousPoint(transform, p, clipped);
    if (!h.w())
        return gfx::Point3F();
    return h.CartesianPoint3d();
}

gfx::RectF MathUtil::ScaleRectProportional(const gfx::RectF& input_outer_rect,
    const gfx::RectF& scale_outer_rect,
    const gfx::RectF& scale_inner_rect)
{
    gfx::RectF output_inner_rect = input_outer_rect;
    float scale_rect_to_input_scale_x = scale_outer_rect.width() / input_outer_rect.width();
    float scale_rect_to_input_scale_y = scale_outer_rect.height() / input_outer_rect.height();

    gfx::Vector2dF top_left_diff = scale_inner_rect.origin() - scale_outer_rect.origin();
    gfx::Vector2dF bottom_right_diff = scale_inner_rect.bottom_right() - scale_outer_rect.bottom_right();
    output_inner_rect.Inset(top_left_diff.x() / scale_rect_to_input_scale_x,
        top_left_diff.y() / scale_rect_to_input_scale_y,
        -bottom_right_diff.x() / scale_rect_to_input_scale_x,
        -bottom_right_diff.y() / scale_rect_to_input_scale_y);
    return output_inner_rect;
}

static inline bool NearlyZero(double value)
{
    return std::abs(value) < std::numeric_limits<double>::epsilon();
}

static inline float ScaleOnAxis(double a, double b, double c)
{
    if (NearlyZero(b) && NearlyZero(c))
        return std::abs(a);
    if (NearlyZero(a) && NearlyZero(c))
        return std::abs(b);
    if (NearlyZero(a) && NearlyZero(b))
        return std::abs(c);

    // Do the sqrt as a double to not lose precision.
    return static_cast<float>(std::sqrt(a * a + b * b + c * c));
}

gfx::Vector2dF MathUtil::ComputeTransform2dScaleComponents(
    const gfx::Transform& transform,
    float fallback_value)
{
    if (transform.HasPerspective())
        return gfx::Vector2dF(fallback_value, fallback_value);
    float x_scale = ScaleOnAxis(transform.matrix().getDouble(0, 0),
        transform.matrix().getDouble(1, 0),
        transform.matrix().getDouble(2, 0));
    float y_scale = ScaleOnAxis(transform.matrix().getDouble(0, 1),
        transform.matrix().getDouble(1, 1),
        transform.matrix().getDouble(2, 1));
    return gfx::Vector2dF(x_scale, y_scale);
}

float MathUtil::SmallestAngleBetweenVectors(const gfx::Vector2dF& v1,
    const gfx::Vector2dF& v2)
{
    double dot_product = gfx::DotProduct(v1, v2) / v1.Length() / v2.Length();
    // Clamp to compensate for rounding errors.
    dot_product = std::max(-1.0, std::min(1.0, dot_product));
    return static_cast<float>(Rad2Deg(std::acos(dot_product)));
}

gfx::Vector2dF MathUtil::ProjectVector(const gfx::Vector2dF& source,
    const gfx::Vector2dF& destination)
{
    float projected_length = gfx::DotProduct(source, destination) / destination.LengthSquared();
    return gfx::Vector2dF(projected_length * destination.x(),
        projected_length * destination.y());
}

scoped_ptr<base::Value> MathUtil::AsValue(const gfx::Size& s)
{
    scoped_ptr<base::DictionaryValue> res(new base::DictionaryValue());
    res->SetDouble("width", s.width());
    res->SetDouble("height", s.height());
    return res.Pass();
}

scoped_ptr<base::Value> MathUtil::AsValue(const gfx::Rect& r)
{
    scoped_ptr<base::ListValue> res(new base::ListValue());
    res->AppendInteger(r.x());
    res->AppendInteger(r.y());
    res->AppendInteger(r.width());
    res->AppendInteger(r.height());
    return res.Pass();
}

bool MathUtil::FromValue(const base::Value* raw_value, gfx::Rect* out_rect)
{
    const base::ListValue* value = nullptr;
    if (!raw_value->GetAsList(&value))
        return false;

    if (value->GetSize() != 4)
        return false;

    int x, y, w, h;
    bool ok = true;
    ok &= value->GetInteger(0, &x);
    ok &= value->GetInteger(1, &y);
    ok &= value->GetInteger(2, &w);
    ok &= value->GetInteger(3, &h);
    if (!ok)
        return false;

    *out_rect = gfx::Rect(x, y, w, h);
    return true;
}

scoped_ptr<base::Value> MathUtil::AsValue(const gfx::PointF& pt)
{
    scoped_ptr<base::ListValue> res(new base::ListValue());
    res->AppendDouble(pt.x());
    res->AppendDouble(pt.y());
    return res.Pass();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::Size& s,
    base::trace_event::TracedValue* res)
{
    res->BeginDictionary(name);
    res->SetDouble("width", s.width());
    res->SetDouble("height", s.height());
    res->EndDictionary();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::SizeF& s,
    base::trace_event::TracedValue* res)
{
    res->BeginDictionary(name);
    res->SetDouble("width", s.width());
    res->SetDouble("height", s.height());
    res->EndDictionary();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::Rect& r,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    res->AppendInteger(r.x());
    res->AppendInteger(r.y());
    res->AppendInteger(r.width());
    res->AppendInteger(r.height());
    res->EndArray();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::PointF& pt,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    res->AppendDouble(pt.x());
    res->AppendDouble(pt.y());
    res->EndArray();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::Point3F& pt,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    res->AppendDouble(pt.x());
    res->AppendDouble(pt.y());
    res->AppendDouble(pt.z());
    res->EndArray();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::Vector2d& v,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    res->AppendInteger(v.x());
    res->AppendInteger(v.y());
    res->EndArray();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::Vector2dF& v,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    res->AppendDouble(v.x());
    res->AppendDouble(v.y());
    res->EndArray();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::ScrollOffset& v,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    res->AppendDouble(v.x());
    res->AppendDouble(v.y());
    res->EndArray();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::QuadF& q,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    res->AppendDouble(q.p1().x());
    res->AppendDouble(q.p1().y());
    res->AppendDouble(q.p2().x());
    res->AppendDouble(q.p2().y());
    res->AppendDouble(q.p3().x());
    res->AppendDouble(q.p3().y());
    res->AppendDouble(q.p4().x());
    res->AppendDouble(q.p4().y());
    res->EndArray();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::RectF& rect,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    res->AppendDouble(rect.x());
    res->AppendDouble(rect.y());
    res->AppendDouble(rect.width());
    res->AppendDouble(rect.height());
    res->EndArray();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::Transform& transform,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    const SkMatrix44& m = transform.matrix();
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col)
            res->AppendDouble(m.getDouble(row, col));
    }
    res->EndArray();
}

void MathUtil::AddToTracedValue(const char* name,
    const gfx::BoxF& box,
    base::trace_event::TracedValue* res)
{
    res->BeginArray(name);
    res->AppendInteger(box.x());
    res->AppendInteger(box.y());
    res->AppendInteger(box.z());
    res->AppendInteger(box.width());
    res->AppendInteger(box.height());
    res->AppendInteger(box.depth());
    res->EndArray();
}

double MathUtil::AsDoubleSafely(double value)
{
    return std::min(value, std::numeric_limits<double>::max());
}

float MathUtil::AsFloatSafely(float value)
{
    return std::min(value, std::numeric_limits<float>::max());
}

gfx::Vector3dF MathUtil::GetXAxis(const gfx::Transform& transform)
{
    return gfx::Vector3dF(transform.matrix().getFloat(0, 0),
        transform.matrix().getFloat(1, 0),
        transform.matrix().getFloat(2, 0));
}

gfx::Vector3dF MathUtil::GetYAxis(const gfx::Transform& transform)
{
    return gfx::Vector3dF(transform.matrix().getFloat(0, 1),
        transform.matrix().getFloat(1, 1),
        transform.matrix().getFloat(2, 1));
}

} // namespace cc
