// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/quads/draw_polygon.h"

#include <vector>

#include "cc/output/bsp_compare_result.h"
#include "cc/quads/draw_quad.h"

namespace {
// This allows for some imperfection in the normal comparison when checking if
// two pieces of geometry are coplanar.
static const float coplanar_dot_epsilon = 0.001f;
// This threshold controls how "thick" a plane is. If a point's distance is
// <= |compare_threshold|, then it is considered on the plane. Only when this
// boundary is crossed do we consider doing splitting.
static const float compare_threshold = 1.0f;
// |split_threshold| is lower in this case because we want the points created
// during splitting to be well within the range of |compare_threshold| for
// comparison purposes. The splitting operation will produce intersection points
// that fit within a tighter distance to the splitting plane as a result of this
// value. By using a value >= |compare_threshold| we run the risk of creating
// points that SHOULD be intersecting the "thick plane", but actually fail to
// test positively for it because |split_threshold| allowed them to be outside
// this range.
// This is really supposd to be compare_threshold / 2.0f, but that would
// create another static initializer.
static const float split_threshold = 0.5f;

static const float normalized_threshold = 0.001f;
} // namespace

namespace cc {

DrawPolygon::DrawPolygon()
{
}

DrawPolygon::DrawPolygon(const DrawQuad* original,
    const std::vector<gfx::Point3F>& in_points,
    const gfx::Vector3dF& normal,
    int draw_order_index)
    : order_index_(draw_order_index)
    , original_ref_(original)
    , is_split_(true)
{
    for (size_t i = 0; i < in_points.size(); i++) {
        points_.push_back(in_points[i]);
    }
    normal_ = normal;
}

// This takes the original DrawQuad that this polygon should be based on,
// a visible content rect to make the 4 corner points from, and a transformation
// to move it and its normal into screen space.
DrawPolygon::DrawPolygon(const DrawQuad* original_ref,
    const gfx::RectF& visible_layer_rect,
    const gfx::Transform& transform,
    int draw_order_index)
    : normal_(0.0f, 0.0f, 1.0f)
    , order_index_(draw_order_index)
    , original_ref_(original_ref)
    , is_split_(false)
{
    gfx::Point3F points[8];
    int num_vertices_in_clipped_quad;
    gfx::QuadF send_quad(visible_layer_rect);

    // Doing this mapping here is very important, since we can't just transform
    // the points without clipping and not run into strange geometry issues when
    // crossing w = 0. At this point, in the constructor, we know that we're
    // working with a quad, so we can reuse the MathUtil::MapClippedQuad3d
    // function instead of writing a generic polygon version of it.
    MathUtil::MapClippedQuad3d(
        transform, send_quad, points, &num_vertices_in_clipped_quad);
    for (int i = 0; i < num_vertices_in_clipped_quad; i++) {
        points_.push_back(points[i]);
    }
    ApplyTransformToNormal(transform);
}

DrawPolygon::~DrawPolygon()
{
}

scoped_ptr<DrawPolygon> DrawPolygon::CreateCopy()
{
    scoped_ptr<DrawPolygon> new_polygon(new DrawPolygon());
    new_polygon->order_index_ = order_index_;
    new_polygon->original_ref_ = original_ref_;
    new_polygon->points_.reserve(points_.size());
    new_polygon->points_ = points_;
    new_polygon->normal_.set_x(normal_.x());
    new_polygon->normal_.set_y(normal_.y());
    new_polygon->normal_.set_z(normal_.z());
    return new_polygon.Pass();
}

float DrawPolygon::SignedPointDistance(const gfx::Point3F& point) const
{
    return gfx::DotProduct(point - points_[0], normal_);
}

// Checks whether or not shape a lies on the front or back side of b, or
// whether they should be considered coplanar. If on the back side, we
// say A_BEFORE_B because it should be drawn in that order.
// Assumes that layers are split and there are no intersecting planes.
BspCompareResult DrawPolygon::SideCompare(const DrawPolygon& a,
    const DrawPolygon& b)
{
    // Let's make sure that both of these are normalized.
    DCHECK_GE(normalized_threshold, std::abs(a.normal_.LengthSquared() - 1.0f));
    DCHECK_GE(normalized_threshold, std::abs(b.normal_.LengthSquared() - 1.0f));
    // Right away let's check if they're coplanar
    double dot = gfx::DotProduct(a.normal_, b.normal_);
    float sign = 0.0f;
    bool normal_match = false;
    // This check assumes that the normals are normalized.
    if (std::abs(dot) >= 1.0f - coplanar_dot_epsilon) {
        normal_match = true;
        // The normals are matching enough that we only have to test one point.
        sign = b.SignedPointDistance(a.points_[0]);
        // Is it on either side of the splitter?
        if (sign < -compare_threshold) {
            return BSP_BACK;
        }

        if (sign > compare_threshold) {
            return BSP_FRONT;
        }

        // No it wasn't, so the sign of the dot product of the normals
        // along with document order determines which side it goes on.
        if (dot >= 0.0f) {
            if (a.order_index_ < b.order_index_) {
                return BSP_COPLANAR_FRONT;
            }
            return BSP_COPLANAR_BACK;
        }

        if (a.order_index_ < b.order_index_) {
            return BSP_COPLANAR_BACK;
        }
        return BSP_COPLANAR_FRONT;
    }

    int pos_count = 0;
    int neg_count = 0;
    for (size_t i = 0; i < a.points_.size(); i++) {
        if (!normal_match || (normal_match && i > 0)) {
            sign = gfx::DotProduct(a.points_[i] - b.points_[0], b.normal_);
        }

        if (sign < -compare_threshold) {
            ++neg_count;
        } else if (sign > compare_threshold) {
            ++pos_count;
        }

        if (pos_count && neg_count) {
            return BSP_SPLIT;
        }
    }

    if (pos_count) {
        return BSP_FRONT;
    }
    return BSP_BACK;
}

static bool LineIntersectPlane(const gfx::Point3F& line_start,
    const gfx::Point3F& line_end,
    const gfx::Point3F& plane_origin,
    const gfx::Vector3dF& plane_normal,
    gfx::Point3F* intersection,
    float distance_threshold)
{
    gfx::Vector3dF start_to_origin_vector = plane_origin - line_start;
    gfx::Vector3dF end_to_origin_vector = plane_origin - line_end;

    double start_distance = gfx::DotProduct(start_to_origin_vector, plane_normal);
    double end_distance = gfx::DotProduct(end_to_origin_vector, plane_normal);

    // The case where one vertex lies on the thick-plane and the other
    // is outside of it.
    if (std::abs(start_distance) <= distance_threshold && std::abs(end_distance) > distance_threshold) {
        intersection->SetPoint(line_start.x(), line_start.y(), line_start.z());
        return true;
    }

    // This is the case where we clearly cross the thick-plane.
    if ((start_distance > distance_threshold && end_distance < -distance_threshold) || (start_distance < -distance_threshold && end_distance > distance_threshold)) {
        gfx::Vector3dF v = line_end - line_start;
        float total_distance = std::abs(start_distance) + std::abs(end_distance);
        float lerp_factor = std::abs(start_distance) / total_distance;

        intersection->SetPoint(line_start.x() + (v.x() * lerp_factor),
            line_start.y() + (v.y() * lerp_factor),
            line_start.z() + (v.z() * lerp_factor));

        return true;
    }
    return false;
}

// This function is separate from ApplyTransform because it is often unnecessary
// to transform the normal with the rest of the polygon.
// When drawing these polygons, it is necessary to move them back into layer
// space before sending them to OpenGL, which requires using ApplyTransform,
// but normal information is no longer needed after sorting.
void DrawPolygon::ApplyTransformToNormal(const gfx::Transform& transform)
{
    // Now we use the inverse transpose of |transform| to transform the normal.
    gfx::Transform inverse_transform;
    bool inverted = transform.GetInverse(&inverse_transform);
    DCHECK(inverted);
    if (!inverted)
        return;
    inverse_transform.Transpose();

    gfx::Point3F new_normal(normal_.x(), normal_.y(), normal_.z());
    inverse_transform.TransformPoint(&new_normal);
    // Make sure our normal is still normalized.
    normal_ = gfx::Vector3dF(new_normal.x(), new_normal.y(), new_normal.z());
    float normal_magnitude = normal_.Length();
    if (normal_magnitude != 0 && normal_magnitude != 1) {
        normal_.Scale(1.0f / normal_magnitude);
    }
}

void DrawPolygon::ApplyTransform(const gfx::Transform& transform)
{
    for (size_t i = 0; i < points_.size(); i++) {
        transform.TransformPoint(&points_[i]);
    }
}

// TransformToScreenSpace assumes we're moving a layer from its layer space
// into 3D screen space, which for sorting purposes requires the normal to
// be transformed along with the vertices.
void DrawPolygon::TransformToScreenSpace(const gfx::Transform& transform)
{
    ApplyTransform(transform);
    ApplyTransformToNormal(transform);
}

// In the case of TransformToLayerSpace, we assume that we are giving the
// inverse transformation back to the polygon to move it back into layer space
// but we can ignore the costly process of applying the inverse to the normal
// since we know the normal will just reset to its original state.
void DrawPolygon::TransformToLayerSpace(
    const gfx::Transform& inverse_transform)
{
    ApplyTransform(inverse_transform);
    normal_ = gfx::Vector3dF(0.0f, 0.0f, -1.0f);
}

bool DrawPolygon::Split(const DrawPolygon& splitter,
    scoped_ptr<DrawPolygon>* front,
    scoped_ptr<DrawPolygon>* back)
{
    gfx::Point3F intersections[2];
    std::vector<gfx::Point3F> out_points[2];
    // vertex_before stores the index of the vertex before its matching
    // intersection.
    // i.e. vertex_before[0] stores the vertex we saw before we crossed the plane
    // which resulted in the line/plane intersection giving us intersections[0].
    size_t vertex_before[2];
    size_t points_size = points_.size();
    size_t current_intersection = 0;

    size_t current_vertex = 0;
    // We will only have two intersection points because we assume all polygons
    // are convex.
    while (current_intersection < 2) {
        if (LineIntersectPlane(points_[(current_vertex % points_size)],
                points_[(current_vertex + 1) % points_size],
                splitter.points_[0],
                splitter.normal_,
                &intersections[current_intersection],
                split_threshold)) {
            vertex_before[current_intersection] = current_vertex % points_size;
            current_intersection++;
            // We found both intersection points so we're done already.
            if (current_intersection == 2) {
                break;
            }
        }
        if (current_vertex++ > (points_size)) {
            break;
        }
    }
    DCHECK_EQ(current_intersection, static_cast<size_t>(2));

    // Since we found both the intersection points, we can begin building the
    // vertex set for both our new polygons.
    size_t start1 = (vertex_before[0] + 1) % points_size;
    size_t start2 = (vertex_before[1] + 1) % points_size;
    size_t points_remaining = points_size;

    // First polygon.
    out_points[0].push_back(intersections[0]);
    DCHECK_GE(vertex_before[1], start1);
    for (size_t i = start1; i <= vertex_before[1]; i++) {
        out_points[0].push_back(points_[i]);
        --points_remaining;
    }
    out_points[0].push_back(intersections[1]);

    // Second polygon.
    out_points[1].push_back(intersections[1]);
    size_t index = start2;
    for (size_t i = 0; i < points_remaining; i++) {
        out_points[1].push_back(points_[index % points_size]);
        ++index;
    }
    out_points[1].push_back(intersections[0]);

    // Give both polygons the original splitting polygon's ID, so that they'll
    // still be sorted properly in co-planar instances.
    scoped_ptr<DrawPolygon> poly1(
        new DrawPolygon(original_ref_, out_points[0], normal_, order_index_));
    scoped_ptr<DrawPolygon> poly2(
        new DrawPolygon(original_ref_, out_points[1], normal_, order_index_));

    DCHECK_GE(poly1->points().size(), 3u);
    DCHECK_GE(poly2->points().size(), 3u);

    if (SideCompare(*poly1, splitter) == BSP_FRONT) {
        *front = poly1.Pass();
        *back = poly2.Pass();
    } else {
        *front = poly2.Pass();
        *back = poly1.Pass();
    }
    return true;
}

// This algorithm takes the first vertex in the polygon and uses that as a
// pivot point to fan out and create quads from the rest of the vertices.
// |offset| starts off as the second vertex, and then |op1| and |op2| indicate
// offset+1 and offset+2 respectively.
// After the first quad is created, the first vertex in the next quad is the
// same as all the rest, the pivot point. The second vertex in the next quad is
// the old |op2|, the last vertex added to the previous quad. This continues
// until all points are exhausted.
// The special case here is where there are only 3 points remaining, in which
// case we use the same values for vertex 3 and 4 to make a degenerate quad
// that represents a triangle.
void DrawPolygon::ToQuads2D(std::vector<gfx::QuadF>* quads) const
{
    if (points_.size() <= 2)
        return;

    gfx::PointF first(points_[0].x(), points_[0].y());
    size_t offset = 1;
    while (offset < points_.size() - 1) {
        size_t op1 = offset + 1;
        size_t op2 = offset + 2;
        if (op2 >= points_.size()) {
            // It's going to be a degenerate triangle.
            op2 = op1;
        }
        quads->push_back(
            gfx::QuadF(first,
                gfx::PointF(points_[offset].x(), points_[offset].y()),
                gfx::PointF(points_[op1].x(), points_[op1].y()),
                gfx::PointF(points_[op2].x(), points_[op2].y())));
        offset = op2;
    }
}

} // namespace cc
