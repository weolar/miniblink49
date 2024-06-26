// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/layer_quad.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/quad_f.h"

namespace cc {
namespace {

    TEST(LayerQuadTest, QuadFConversion)
    {
        gfx::PointF p1(-0.5f, -0.5f);
        gfx::PointF p2(0.5f, -0.5f);
        gfx::PointF p3(0.5f, 0.5f);
        gfx::PointF p4(-0.5f, 0.5f);

        gfx::QuadF quad_cw(p1, p2, p3, p4);
        LayerQuad layer_quad_cw(quad_cw);
        EXPECT_EQ(layer_quad_cw.ToQuadF(), quad_cw);

        gfx::QuadF quad_ccw(p1, p4, p3, p2);
        LayerQuad layer_quad_ccw(quad_ccw);
        EXPECT_EQ(layer_quad_ccw.ToQuadF(), quad_ccw);
    }

    TEST(LayerQuadTest, Inflate)
    {
        gfx::PointF p1(-0.5f, -0.5f);
        gfx::PointF p2(0.5f, -0.5f);
        gfx::PointF p3(0.5f, 0.5f);
        gfx::PointF p4(-0.5f, 0.5f);

        gfx::QuadF quad(p1, p2, p3, p4);
        LayerQuad layer_quad(quad);
        quad.Scale(2.f, 2.f);
        layer_quad.Inflate(0.5f);
        EXPECT_EQ(layer_quad.ToQuadF(), quad);
    }

    TEST(LayerQuadTest, Degenerate)
    {
        gfx::QuadF quad;
        gfx::PointF p1(1.0f, 1.0f);
        gfx::PointF p2(0.0f, 1.0f);
        gfx::PointF p3(1.0f, 0.0f);
        gfx::QuadF triangle(p1, p2, p3, p1);

        LayerQuad::Edge e1d(p1, p1);
        LayerQuad::Edge e2d(p2, p2);
        LayerQuad::Edge e2(p1, p2);
        LayerQuad::Edge e3(p2, p3);
        LayerQuad::Edge e4(p3, p1);
        EXPECT_TRUE(e1d.degenerate());
        EXPECT_TRUE(e2d.degenerate());
        EXPECT_FALSE(e2.degenerate());
        EXPECT_FALSE(e3.degenerate());
        EXPECT_FALSE(e4.degenerate());

        LayerQuad degenerate_quad(e1d, e2d, e2, e3);
        // With more than one degenerate edge, we expect the quad to be zero.
        EXPECT_EQ(quad, degenerate_quad.ToQuadF());

        LayerQuad triangle_quad(e1d, e2, e3, e4);
        // With only one degenerate edge, we expect the quad to be a triangle.
        EXPECT_EQ(triangle, triangle_quad.ToQuadF());
    }

} // namespace
} // namespace cc
