// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_DRAW_POLYGON_H_
#define CC_QUADS_DRAW_POLYGON_H_

#include <vector>

#include "cc/base/math_util.h"
#include "cc/output/bsp_compare_result.h"
#include "ui/gfx/geometry/point3_f.h"
#include "ui/gfx/geometry/quad_f.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/vector3d_f.h"
#include "ui/gfx/transform.h"

namespace cc {

class DrawQuad;

class CC_EXPORT DrawPolygon {
public:
    DrawPolygon();
    ~DrawPolygon();

    DrawPolygon(const DrawQuad* original_ref,
        const std::vector<gfx::Point3F>& in_points,
        const gfx::Vector3dF& normal,
        int draw_order_index = 0);
    DrawPolygon(const DrawQuad* original_ref,
        const gfx::RectF& visible_layer_rect,
        const gfx::Transform& transform,
        int draw_order_index = 0);

    // Split takes this DrawPolygon and splits it into two pieces that are on
    // either side of |splitter|. Any edges of this polygon that cross the plane
    // of |splitter| will have an intersection point that is shared by both
    // polygons on either side.
    // Split will only return true if it determines that we got back 2
    // intersection points. Only when it returns true will front and back both be
    // valid new polygons that are on opposite sides of the splitting plane.
    bool Split(const DrawPolygon& splitter,
        scoped_ptr<DrawPolygon>* front,
        scoped_ptr<DrawPolygon>* back);
    float SignedPointDistance(const gfx::Point3F& point) const;
    // Checks polygon a against polygon b and returns which side it lies on, or
    // whether it crosses (necessitating a split in the BSP tree).
    static BspCompareResult SideCompare(const DrawPolygon& a,
        const DrawPolygon& b);
    void ToQuads2D(std::vector<gfx::QuadF>* quads) const;
    void TransformToScreenSpace(const gfx::Transform& transform);
    void TransformToLayerSpace(const gfx::Transform& inverse_transform);

    const std::vector<gfx::Point3F>& points() const { return points_; }
    const gfx::Vector3dF& normal() const { return normal_; }
    const DrawQuad* original_ref() const { return original_ref_; }
    int order_index() const { return order_index_; }
    bool is_split() const { return is_split_; }
    scoped_ptr<DrawPolygon> CreateCopy();

private:
    void ApplyTransform(const gfx::Transform& transform);
    void ApplyTransformToNormal(const gfx::Transform& transform);

    std::vector<gfx::Point3F> points_;
    // Normalized, necessitated by distance calculations and tests of coplanarity.
    gfx::Vector3dF normal_;
    // This is an index that can be used to test whether a quad comes before or
    // after another in document order, useful for tie-breaking when it comes
    // to coplanar surfaces.
    int order_index_;
    // The pointer to the original quad, which gives us all the drawing info
    // we need.
    // This DrawQuad is owned by the caller and its lifetime must be preserved
    // as long as this DrawPolygon is alive.
    const DrawQuad* original_ref_;
    bool is_split_;
};

} // namespace cc

#endif // CC_QUADS_DRAW_POLYGON_H_
