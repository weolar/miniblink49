// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits>
#include <vector>

#include "cc/output/bsp_compare_result.h"
#include "cc/quads/draw_polygon.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/transform.h"

namespace cc {
namespace {

#define CREATE_NEW_DRAW_POLYGON(name, points_vector, normal, polygon_id) \
    DrawPolygon name(NULL, points_vector, normal, polygon_id)

#define EXPECT_FLOAT_WITHIN_EPSILON_OF(a, b) \
    EXPECT_TRUE(std::abs(a - b) < std::numeric_limits<float>::epsilon());

#define EXPECT_POINT_EQ(point_a, point_b)      \
    EXPECT_FLOAT_EQ(point_a.x(), point_b.x()); \
    EXPECT_FLOAT_EQ(point_a.y(), point_b.y()); \
    EXPECT_FLOAT_EQ(point_a.z(), point_b.z());

    static void ValidatePoints(const DrawPolygon& polygon,
        const std::vector<gfx::Point3F>& points)
    {
        EXPECT_EQ(polygon.points().size(), points.size());
        for (size_t i = 0; i < points.size(); i++) {
            EXPECT_POINT_EQ(polygon.points()[i], points[i]);
        }
    }

    // Two quads are definitely not touching and so no split should occur.
    TEST(DrawPolygonSplitTest, NotTouchingNoSplit)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(0.0f, 10.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(0.0f, 0.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 0.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 10.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(5.0f, 10.0f, 5.0f));
        vertices_b.push_back(gfx::Point3F(5.0f, 0.0f, 15.0f));
        vertices_b.push_back(gfx::Point3F(5.0f, 0.0f, 15.0f));
        vertices_b.push_back(gfx::Point3F(5.0f, 10.0f, 5.0f));

        CREATE_NEW_DRAW_POLYGON(
            polygon_a, vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0);
        CREATE_NEW_DRAW_POLYGON(
            polygon_b, vertices_b, gfx::Vector3dF(-1.0f, 0.0f, 0.0f), 1);

        EXPECT_EQ(DrawPolygon::SideCompare(polygon_b, polygon_a), BSP_FRONT);
    }

    // One quad is resting against another, but doesn't cross its plane so no split
    // should occur.
    TEST(DrawPolygonSplitTest, BarelyTouchingNoSplit)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(0.0f, 10.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(0.0f, 0.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 0.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 10.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(5.0f, 10.0f, 0.0f));
        vertices_b.push_back(gfx::Point3F(5.0f, 0.0f, -10.0f));
        vertices_b.push_back(gfx::Point3F(5.0f, 0.0f, -10.0f));
        vertices_b.push_back(gfx::Point3F(5.0f, 10.0f, 0.0f));

        CREATE_NEW_DRAW_POLYGON(
            polygon_a, vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0);
        CREATE_NEW_DRAW_POLYGON(
            polygon_b, vertices_b, gfx::Vector3dF(-1.0f, 0.0f, 0.0f), 1);

        EXPECT_EQ(DrawPolygon::SideCompare(polygon_b, polygon_a), BSP_BACK);
    }

    // One quad intersects another and becomes two pieces.
    TEST(DrawPolygonSplitTest, BasicSplit)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(0.0f, 10.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(0.0f, 0.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 0.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 10.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(5.0f, 10.0f, -5.0f));
        vertices_b.push_back(gfx::Point3F(5.0f, 0.0f, -5.0f));
        vertices_b.push_back(gfx::Point3F(5.0f, 0.0f, 5.0f));
        vertices_b.push_back(gfx::Point3F(5.0f, 10.0f, 5.0f));

        CREATE_NEW_DRAW_POLYGON(
            polygon_a, vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0);
        CREATE_NEW_DRAW_POLYGON(
            polygon_b, vertices_b, gfx::Vector3dF(-1.0f, 0.0f, 0.0f), 1);

        EXPECT_EQ(DrawPolygon::SideCompare(polygon_b, polygon_a), BSP_SPLIT);

        scoped_ptr<DrawPolygon> front_polygon;
        scoped_ptr<DrawPolygon> back_polygon;
        polygon_b.Split(polygon_a, &front_polygon, &back_polygon);
        EXPECT_EQ(DrawPolygon::SideCompare(*front_polygon, polygon_a), BSP_FRONT);
        EXPECT_EQ(DrawPolygon::SideCompare(*back_polygon, polygon_a), BSP_BACK);

        std::vector<gfx::Point3F> test_points_a;
        test_points_a.push_back(gfx::Point3F(5.0f, 0.0f, 0.0f));
        test_points_a.push_back(gfx::Point3F(5.0f, 0.0f, 5.0f));
        test_points_a.push_back(gfx::Point3F(5.0f, 10.0f, 5.0f));
        test_points_a.push_back(gfx::Point3F(5.0f, 10.0f, 0.0f));
        std::vector<gfx::Point3F> test_points_b;
        test_points_b.push_back(gfx::Point3F(5.0f, 10.0f, 0.0f));
        test_points_b.push_back(gfx::Point3F(5.0f, 10.0f, -5.0f));
        test_points_b.push_back(gfx::Point3F(5.0f, 0.0f, -5.0f));
        test_points_b.push_back(gfx::Point3F(5.0f, 0.0f, 0.0f));
        ValidatePoints(*(front_polygon.get()), test_points_a);
        ValidatePoints(*(back_polygon.get()), test_points_b);

        EXPECT_EQ(front_polygon->points().size(), 4u);
        EXPECT_EQ(back_polygon->points().size(), 4u);
    }

    // In this test we cut the corner of a quad so that it creates a triangle and
    // a pentagon as a result.
    TEST(DrawPolygonSplitTest, AngledSplit)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(0.0f, 0.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(0.0f, 0.0f, 10.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 0.0f, 10.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 0.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(2.0f, 5.0f, 1.0f));
        vertices_b.push_back(gfx::Point3F(2.0f, -5.0f, 1.0f));
        vertices_b.push_back(gfx::Point3F(-1.0f, -5.0f, -2.0f));
        vertices_b.push_back(gfx::Point3F(-1.0f, 5.0f, -2.0f));

        CREATE_NEW_DRAW_POLYGON(
            polygon_a, vertices_a, gfx::Vector3dF(0.0f, 1.0f, 0.0f), 0);
        CREATE_NEW_DRAW_POLYGON(
            polygon_b, vertices_b, gfx::Vector3dF(0.707107f, 0.0f, -0.707107f), 1);

        EXPECT_EQ(DrawPolygon::SideCompare(polygon_a, polygon_b), BSP_SPLIT);

        scoped_ptr<DrawPolygon> front_polygon;
        scoped_ptr<DrawPolygon> back_polygon;
        polygon_a.Split(polygon_b, &front_polygon, &back_polygon);
        EXPECT_EQ(DrawPolygon::SideCompare(*front_polygon, polygon_b), BSP_FRONT);
        EXPECT_EQ(DrawPolygon::SideCompare(*back_polygon, polygon_b), BSP_BACK);

        EXPECT_EQ(front_polygon->points().size(), 3u);
        EXPECT_EQ(back_polygon->points().size(), 5u);

        std::vector<gfx::Point3F> test_points_a;
        test_points_a.push_back(gfx::Point3F(10.0f, 0.0f, 9.0f));
        test_points_a.push_back(gfx::Point3F(10.0f, 0.0f, 0.0f));
        test_points_a.push_back(gfx::Point3F(1.0f, 0.0f, 0.0f));
        std::vector<gfx::Point3F> test_points_b;
        test_points_b.push_back(gfx::Point3F(1.0f, 0.0f, 0.0f));
        test_points_b.push_back(gfx::Point3F(0.0f, 0.0f, 0.0f));
        test_points_b.push_back(gfx::Point3F(0.0f, 0.0f, 10.0f));
        test_points_b.push_back(gfx::Point3F(10.0f, 0.0f, 10.0f));
        test_points_b.push_back(gfx::Point3F(10.0f, 0.0f, 9.0f));

        ValidatePoints(*(front_polygon.get()), test_points_a);
        ValidatePoints(*(back_polygon.get()), test_points_b);
    }

    TEST(DrawPolygonTransformTest, TransformNormal)
    {
        // We give this polygon no actual vertices because we're not interested
        // in actually transforming any points, just the normal.
        std::vector<gfx::Point3F> vertices_a;
        CREATE_NEW_DRAW_POLYGON(
            polygon_a, vertices_a, gfx::Vector3dF(0.707107f, 0.0f, -0.707107f), 0);

        gfx::Transform transform;
        transform.RotateAboutYAxis(45.0);
        // This would transform the vertices as well, but we are transforming a
        // DrawPolygon with 0 vertices just to make sure our normal transformation
        // using the inverse tranpose matrix gives us the right result.
        polygon_a.TransformToScreenSpace(transform);

        // Note: We use EXPECT_FLOAT_WITHIN_EPSILON instead of EXPECT_FLOAT_EQUAL here
        // because some architectures (e.g., Arm64) employ a fused multiply-add
        // instruction which causes rounding asymmetry and reduces precision.
        // http://crbug.com/401117.
        EXPECT_FLOAT_WITHIN_EPSILON_OF(polygon_a.normal().x(), 0);
        EXPECT_FLOAT_WITHIN_EPSILON_OF(polygon_a.normal().y(), 0);
        EXPECT_FLOAT_WITHIN_EPSILON_OF(polygon_a.normal().z(), -1);
    }

} // namespace
} // namespace cc
