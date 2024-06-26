// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/fx_coordinates.h"

#include <vector>

#include "testing/gtest/include/gtest/gtest.h"

TEST(CFX_FloatRect, GetBBox) {
  CFX_FloatRect rect = CFX_FloatRect::GetBBox(nullptr, 0);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);

  std::vector<CFX_PointF> data;
  data.emplace_back(0.0f, 0.0f);
  rect = CFX_FloatRect::GetBBox(data.data(), 0);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
  rect = CFX_FloatRect::GetBBox(data.data(), data.size());
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);

  data.emplace_back(2.5f, 6.2f);
  data.emplace_back(1.5f, 6.2f);
  rect = CFX_FloatRect::GetBBox(data.data(), 2);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(2.5f, rect.right);
  EXPECT_FLOAT_EQ(6.2f, rect.top);

  rect = CFX_FloatRect::GetBBox(data.data(), data.size());
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(2.5f, rect.right);
  EXPECT_FLOAT_EQ(6.2f, rect.top);

  data.emplace_back(2.5f, 6.3f);
  rect = CFX_FloatRect::GetBBox(data.data(), data.size());
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(2.5f, rect.right);
  EXPECT_FLOAT_EQ(6.3f, rect.top);

  data.emplace_back(-3.0f, 6.3f);
  rect = CFX_FloatRect::GetBBox(data.data(), data.size());
  EXPECT_FLOAT_EQ(-3.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(2.5f, rect.right);
  EXPECT_FLOAT_EQ(6.3f, rect.top);

  data.emplace_back(4.0f, -8.0f);
  rect = CFX_FloatRect::GetBBox(data.data(), data.size());
  EXPECT_FLOAT_EQ(-3.0f, rect.left);
  EXPECT_FLOAT_EQ(-8.0f, rect.bottom);
  EXPECT_FLOAT_EQ(4.0f, rect.right);
  EXPECT_FLOAT_EQ(6.3f, rect.top);
}

TEST(CFX_FloatRect, Normalize) {
  CFX_FloatRect rect;
  rect.Normalize();
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);

  rect = CFX_FloatRect(-1.0f, -3.0f, 4.5f, 3.2f);
  rect.Normalize();
  EXPECT_FLOAT_EQ(-1.0f, rect.left);
  EXPECT_FLOAT_EQ(-3.0f, rect.bottom);
  EXPECT_FLOAT_EQ(4.5f, rect.right);
  EXPECT_FLOAT_EQ(3.2f, rect.top);
  rect.Scale(-1.0f);
  rect.Normalize();
  EXPECT_FLOAT_EQ(-4.5f, rect.left);
  EXPECT_FLOAT_EQ(-3.2f, rect.bottom);
  EXPECT_FLOAT_EQ(1.0f, rect.right);
  EXPECT_FLOAT_EQ(3.0f, rect.top);
}

TEST(CFX_FloatRect, Scale) {
  CFX_FloatRect rect(-1.0f, -3.0f, 4.5f, 3.2f);
  rect.Scale(1.0f);
  EXPECT_FLOAT_EQ(-1.0f, rect.left);
  EXPECT_FLOAT_EQ(-3.0f, rect.bottom);
  EXPECT_FLOAT_EQ(4.5f, rect.right);
  EXPECT_FLOAT_EQ(3.2f, rect.top);
  rect.Scale(0.5f);
  EXPECT_FLOAT_EQ(-0.5, rect.left);
  EXPECT_FLOAT_EQ(-1.5, rect.bottom);
  EXPECT_FLOAT_EQ(2.25f, rect.right);
  EXPECT_FLOAT_EQ(1.6f, rect.top);
  rect.Scale(2.0f);
  EXPECT_FLOAT_EQ(-1.0f, rect.left);
  EXPECT_FLOAT_EQ(-3.0f, rect.bottom);
  EXPECT_FLOAT_EQ(4.5f, rect.right);
  EXPECT_FLOAT_EQ(3.2f, rect.top);
  rect.Scale(-1.0f);
  EXPECT_FLOAT_EQ(1.0f, rect.left);
  EXPECT_FLOAT_EQ(3.0f, rect.bottom);
  EXPECT_FLOAT_EQ(-4.5f, rect.right);
  EXPECT_FLOAT_EQ(-3.2f, rect.top);
  rect.Scale(0.0f);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
}

TEST(CFX_FloatRect, ScaleEmpty) {
  CFX_FloatRect rect;
  rect.Scale(1.0f);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
  rect.Scale(0.5f);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
  rect.Scale(2.0f);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
  rect.Scale(0.0f);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
}

TEST(CFX_FloatRect, ScaleFromCenterPoint) {
  CFX_FloatRect rect(-1.0f, -3.0f, 4.5f, 3.2f);
  rect.ScaleFromCenterPoint(1.0f);
  EXPECT_FLOAT_EQ(-1.0f, rect.left);
  EXPECT_FLOAT_EQ(-3.0f, rect.bottom);
  EXPECT_FLOAT_EQ(4.5f, rect.right);
  EXPECT_FLOAT_EQ(3.2f, rect.top);
  rect.ScaleFromCenterPoint(0.5f);
  EXPECT_FLOAT_EQ(0.375f, rect.left);
  EXPECT_FLOAT_EQ(-1.45f, rect.bottom);
  EXPECT_FLOAT_EQ(3.125f, rect.right);
  EXPECT_FLOAT_EQ(1.65f, rect.top);
  rect.ScaleFromCenterPoint(2.0f);
  EXPECT_FLOAT_EQ(-1.0f, rect.left);
  EXPECT_FLOAT_EQ(-3.0f, rect.bottom);
  EXPECT_FLOAT_EQ(4.5f, rect.right);
  EXPECT_FLOAT_EQ(3.2f, rect.top);
  rect.ScaleFromCenterPoint(-1.0f);
  EXPECT_FLOAT_EQ(4.5f, rect.left);
  EXPECT_FLOAT_EQ(3.2f, rect.bottom);
  EXPECT_FLOAT_EQ(-1.0f, rect.right);
  EXPECT_FLOAT_EQ(-3.0f, rect.top);
  rect.ScaleFromCenterPoint(0.0f);
  EXPECT_FLOAT_EQ(1.75f, rect.left);
  EXPECT_NEAR(0.1f, rect.bottom, 0.001f);
  EXPECT_FLOAT_EQ(1.75f, rect.right);
  EXPECT_NEAR(0.1f, rect.top, 0.001f);
}

TEST(CFX_FloatRect, ScaleFromCenterPointEmpty) {
  CFX_FloatRect rect;
  rect.ScaleFromCenterPoint(1.0f);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
  rect.ScaleFromCenterPoint(0.5f);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
  rect.ScaleFromCenterPoint(2.0f);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
  rect.ScaleFromCenterPoint(0.0f);
  EXPECT_FLOAT_EQ(0.0f, rect.left);
  EXPECT_FLOAT_EQ(0.0f, rect.bottom);
  EXPECT_FLOAT_EQ(0.0f, rect.right);
  EXPECT_FLOAT_EQ(0.0f, rect.top);
}

#ifndef NDEBUG
TEST(CFX_FloatRect, Print) {
  std::ostringstream os;
  CFX_FloatRect rect;
  os << rect;
  EXPECT_STREQ("rect[w 0 x h 0 (left 0, bot 0)]", os.str().c_str());

  os.str("");
  rect = CFX_FloatRect(10, 20, 14, 23);
  os << rect;
  EXPECT_STREQ("rect[w 4 x h 3 (left 10, bot 20)]", os.str().c_str());

  os.str("");
  rect = CFX_FloatRect(10.5, 20.5, 14.75, 23.75);
  os << rect;
  EXPECT_STREQ("rect[w 4.25 x h 3.25 (left 10.5, bot 20.5)]", os.str().c_str());
}

TEST(CFX_RectF, Print) {
  std::ostringstream os;
  CFX_RectF rect;
  os << rect;
  EXPECT_STREQ("rect[w 0 x h 0 (left 0, top 0)]", os.str().c_str());

  os.str("");
  rect = CFX_RectF(10, 20, 4, 3);
  os << rect;
  EXPECT_STREQ("rect[w 4 x h 3 (left 10, top 20)]", os.str().c_str());

  os.str("");
  rect = CFX_RectF(10.5, 20.5, 4.25, 3.25);
  os << rect;
  EXPECT_STREQ("rect[w 4.25 x h 3.25 (left 10.5, top 20.5)]", os.str().c_str());
}
#endif  // NDEBUG

TEST(CFX_Matrix, ReverseIdentity) {
  CFX_Matrix rev = CFX_Matrix().GetInverse();

  EXPECT_FLOAT_EQ(1.0, rev.a);
  EXPECT_FLOAT_EQ(0.0, rev.b);
  EXPECT_FLOAT_EQ(0.0, rev.c);
  EXPECT_FLOAT_EQ(1.0, rev.d);
  EXPECT_FLOAT_EQ(0.0, rev.e);
  EXPECT_FLOAT_EQ(0.0, rev.f);

  CFX_PointF expected(2, 3);
  CFX_PointF result = rev.Transform(CFX_Matrix().Transform(CFX_PointF(2, 3)));
  EXPECT_FLOAT_EQ(expected.x, result.x);
  EXPECT_FLOAT_EQ(expected.y, result.y);
}

TEST(CFX_Matrix, SetIdentity) {
  CFX_Matrix m;
  EXPECT_FLOAT_EQ(1.0f, m.a);
  EXPECT_FLOAT_EQ(0.0f, m.b);
  EXPECT_FLOAT_EQ(0.0f, m.c);
  EXPECT_FLOAT_EQ(1.0f, m.d);
  EXPECT_FLOAT_EQ(0.0f, m.e);
  EXPECT_FLOAT_EQ(0.0f, m.f);
  EXPECT_TRUE(m.IsIdentity());

  m.a = -1;
  EXPECT_FALSE(m.IsIdentity());

  m = CFX_Matrix();
  EXPECT_FLOAT_EQ(1.0f, m.a);
  EXPECT_FLOAT_EQ(0.0f, m.b);
  EXPECT_FLOAT_EQ(0.0f, m.c);
  EXPECT_FLOAT_EQ(1.0f, m.d);
  EXPECT_FLOAT_EQ(0.0f, m.e);
  EXPECT_FLOAT_EQ(0.0f, m.f);
  EXPECT_TRUE(m.IsIdentity());
}

TEST(CFX_Matrix, AsTuple) {
  CFX_Matrix m(1, 2, 3, 4, 5, 6);
  auto tuple = m.AsTuple();
  EXPECT_FLOAT_EQ(1.0f, std::get<0>(tuple));
  EXPECT_FLOAT_EQ(2.0f, std::get<1>(tuple));
  EXPECT_FLOAT_EQ(3.0f, std::get<2>(tuple));
  EXPECT_FLOAT_EQ(4.0f, std::get<3>(tuple));
  EXPECT_FLOAT_EQ(5.0f, std::get<4>(tuple));
  EXPECT_FLOAT_EQ(6.0f, std::get<5>(tuple));
}

TEST(CFX_Matrix, GetInverse) {
  static constexpr float data[6] = {3, 0, 2, 3, 1, 4};
  CFX_Matrix m(data);
  CFX_Matrix rev = m.GetInverse();

  EXPECT_FLOAT_EQ(0.33333334f, rev.a);
  EXPECT_FLOAT_EQ(0.0f, rev.b);
  EXPECT_FLOAT_EQ(-0.22222222f, rev.c);
  EXPECT_FLOAT_EQ(0.33333334f, rev.d);
  EXPECT_FLOAT_EQ(0.55555556f, rev.e);
  EXPECT_FLOAT_EQ(-1.3333334f, rev.f);

  CFX_PointF expected(2, 3);
  CFX_PointF result = rev.Transform(m.Transform(CFX_PointF(2, 3)));
  EXPECT_FLOAT_EQ(expected.x, result.x);
  EXPECT_FLOAT_EQ(expected.y, result.y);
}

// Note, I think these are a bug and the matrix should be the identity.
TEST(CFX_Matrix, GetInverseCR702041) {
  // The determinate is < std::numeric_limits<float>::epsilon()
  static constexpr float data[6] = {0.947368443f, -0.108947366f, -0.923076928f,
                                    0.106153846f, 18.0f,         787.929993f};
  CFX_Matrix m(data);
  CFX_Matrix rev = m.GetInverse();

  EXPECT_FLOAT_EQ(14247728.0f, rev.a);
  EXPECT_FLOAT_EQ(14622668.0f, rev.b);
  EXPECT_FLOAT_EQ(1.2389329e+08f, rev.c);
  EXPECT_FLOAT_EQ(1.2715364e+08f, rev.d);
  EXPECT_FLOAT_EQ(-9.7875698e+10f, rev.e);
  EXPECT_FLOAT_EQ(-1.0045138e+11f, rev.f);

  // Should be 2, 3
  CFX_PointF expected(0, 0);
  CFX_PointF result = rev.Transform(m.Transform(CFX_PointF(2, 3)));
  EXPECT_FLOAT_EQ(expected.x, result.x);
  EXPECT_FLOAT_EQ(expected.y, result.y);
}

TEST(CFX_Matrix, GetInverseCR714187) {
  // The determinate is < std::numeric_limits<float>::epsilon()
  static constexpr float data[6] = {0.000037f,  0.0f,        0.0f,
                                    -0.000037f, 182.413101f, 136.977646f};
  CFX_Matrix m(data);
  CFX_Matrix rev = m.GetInverse();

  EXPECT_FLOAT_EQ(27027.025f, rev.a);
  EXPECT_FLOAT_EQ(0.0f, rev.b);
  EXPECT_FLOAT_EQ(0.0f, rev.c);
  EXPECT_FLOAT_EQ(-27027.025f, rev.d);
  EXPECT_FLOAT_EQ(-4930083.5f, rev.e);
  EXPECT_FLOAT_EQ(3702098.2f, rev.f);

  // Should be 3 ....
  CFX_PointF expected(2, 2.75);
  CFX_PointF result = rev.Transform(m.Transform(CFX_PointF(2, 3)));
  EXPECT_FLOAT_EQ(expected.x, result.x);
  EXPECT_FLOAT_EQ(expected.y, result.y);
}

#define EXPECT_NEAR_FIVE_PLACES(a, b) EXPECT_NEAR((a), (b), 1e-5)

TEST(CFX_Matrix, ComposeTransformations) {
  // sin(FX_PI/2) and cos(FX_PI/2) have a tiny error and are not exactly 1.0f
  // and 0.0f. The rotation matrix is thus not perfect.

  CFX_Matrix rotate_90;
  rotate_90.Rotate(FX_PI / 2);
  EXPECT_NEAR_FIVE_PLACES(0.0f, rotate_90.a);
  EXPECT_NEAR_FIVE_PLACES(1.0f, rotate_90.b);
  EXPECT_NEAR_FIVE_PLACES(-1.0f, rotate_90.c);
  EXPECT_NEAR_FIVE_PLACES(0.0f, rotate_90.d);
  EXPECT_FLOAT_EQ(0.0f, rotate_90.e);
  EXPECT_FLOAT_EQ(0.0f, rotate_90.f);

  CFX_Matrix translate_23_11;
  translate_23_11.Translate(23, 11);
  EXPECT_FLOAT_EQ(1.0f, translate_23_11.a);
  EXPECT_FLOAT_EQ(0.0f, translate_23_11.b);
  EXPECT_FLOAT_EQ(0.0f, translate_23_11.c);
  EXPECT_FLOAT_EQ(1.0f, translate_23_11.d);
  EXPECT_FLOAT_EQ(23.0f, translate_23_11.e);
  EXPECT_FLOAT_EQ(11.0f, translate_23_11.f);

  CFX_Matrix scale_5_13;
  scale_5_13.Scale(5, 13);
  EXPECT_FLOAT_EQ(5.0f, scale_5_13.a);
  EXPECT_FLOAT_EQ(0.0f, scale_5_13.b);
  EXPECT_FLOAT_EQ(0.0f, scale_5_13.c);
  EXPECT_FLOAT_EQ(13.0f, scale_5_13.d);
  EXPECT_FLOAT_EQ(0.0, scale_5_13.e);
  EXPECT_FLOAT_EQ(0.0, scale_5_13.f);

  // Apply the transforms to points step by step.
  CFX_PointF origin_transformed(0, 0);
  CFX_PointF p_10_20_transformed(10, 20);

  origin_transformed = rotate_90.Transform(origin_transformed);
  EXPECT_FLOAT_EQ(0.0f, origin_transformed.x);
  EXPECT_FLOAT_EQ(0.0f, origin_transformed.y);
  p_10_20_transformed = rotate_90.Transform(p_10_20_transformed);
  EXPECT_FLOAT_EQ(-20.0f, p_10_20_transformed.x);
  EXPECT_FLOAT_EQ(10.0f, p_10_20_transformed.y);

  origin_transformed = translate_23_11.Transform(origin_transformed);
  EXPECT_FLOAT_EQ(23.0f, origin_transformed.x);
  EXPECT_FLOAT_EQ(11.0f, origin_transformed.y);
  p_10_20_transformed = translate_23_11.Transform(p_10_20_transformed);
  EXPECT_FLOAT_EQ(3.0f, p_10_20_transformed.x);
  EXPECT_FLOAT_EQ(21.0f, p_10_20_transformed.y);

  origin_transformed = scale_5_13.Transform(origin_transformed);
  EXPECT_FLOAT_EQ(115.0f, origin_transformed.x);
  EXPECT_FLOAT_EQ(143.0f, origin_transformed.y);
  p_10_20_transformed = scale_5_13.Transform(p_10_20_transformed);
  EXPECT_FLOAT_EQ(15.0f, p_10_20_transformed.x);
  EXPECT_FLOAT_EQ(273.0f, p_10_20_transformed.y);

  // Apply the transforms to points in the reverse order.
  origin_transformed = CFX_PointF(0, 0);
  p_10_20_transformed = CFX_PointF(10, 20);

  origin_transformed = scale_5_13.Transform(origin_transformed);
  EXPECT_FLOAT_EQ(0.0f, origin_transformed.x);
  EXPECT_FLOAT_EQ(0.0f, origin_transformed.y);
  p_10_20_transformed = scale_5_13.Transform(p_10_20_transformed);
  EXPECT_FLOAT_EQ(50.0f, p_10_20_transformed.x);
  EXPECT_FLOAT_EQ(260.0f, p_10_20_transformed.y);

  origin_transformed = translate_23_11.Transform(origin_transformed);
  EXPECT_FLOAT_EQ(23.0f, origin_transformed.x);
  EXPECT_FLOAT_EQ(11.0f, origin_transformed.y);
  p_10_20_transformed = translate_23_11.Transform(p_10_20_transformed);
  EXPECT_FLOAT_EQ(73.0f, p_10_20_transformed.x);
  EXPECT_FLOAT_EQ(271.0f, p_10_20_transformed.y);

  origin_transformed = rotate_90.Transform(origin_transformed);
  EXPECT_FLOAT_EQ(-11.0f, origin_transformed.x);
  EXPECT_FLOAT_EQ(23.0f, origin_transformed.y);
  p_10_20_transformed = rotate_90.Transform(p_10_20_transformed);
  EXPECT_FLOAT_EQ(-271.0f, p_10_20_transformed.x);
  EXPECT_FLOAT_EQ(73.0f, p_10_20_transformed.y);

  // Compose all transforms.
  CFX_Matrix m;
  m.Concat(rotate_90);
  m.Concat(translate_23_11);
  m.Concat(scale_5_13);
  EXPECT_NEAR_FIVE_PLACES(0.0f, m.a);
  EXPECT_NEAR_FIVE_PLACES(13.0f, m.b);
  EXPECT_NEAR_FIVE_PLACES(-5.0f, m.c);
  EXPECT_NEAR_FIVE_PLACES(0.0f, m.d);
  EXPECT_FLOAT_EQ(115.0, m.e);
  EXPECT_FLOAT_EQ(143.0, m.f);

  // Note how the results using the combined matrix are equal to the results
  // when applying the three original matrices step-by-step.
  origin_transformed = m.Transform(CFX_PointF(0, 0));
  EXPECT_FLOAT_EQ(115.0f, origin_transformed.x);
  EXPECT_FLOAT_EQ(143.0f, origin_transformed.y);

  p_10_20_transformed = m.Transform(CFX_PointF(10, 20));
  EXPECT_FLOAT_EQ(15.0f, p_10_20_transformed.x);
  EXPECT_FLOAT_EQ(273.0f, p_10_20_transformed.y);

  // Now compose all transforms prepending.
  m = CFX_Matrix();
  m = rotate_90 * m;
  m = translate_23_11 * m;
  m = scale_5_13 * m;
  EXPECT_NEAR_FIVE_PLACES(0.0f, m.a);
  EXPECT_NEAR_FIVE_PLACES(5.0f, m.b);
  EXPECT_NEAR_FIVE_PLACES(-13.0f, m.c);
  EXPECT_NEAR_FIVE_PLACES(0.0f, m.d);
  EXPECT_FLOAT_EQ(-11.0, m.e);
  EXPECT_FLOAT_EQ(23.0, m.f);

  // Note how the results using the combined matrix are equal to the results
  // when applying the three original matrices step-by-step in the reverse
  // order.
  origin_transformed = m.Transform(CFX_PointF(0, 0));
  EXPECT_FLOAT_EQ(-11.0f, origin_transformed.x);
  EXPECT_FLOAT_EQ(23.0f, origin_transformed.y);

  p_10_20_transformed = m.Transform(CFX_PointF(10, 20));
  EXPECT_FLOAT_EQ(-271.0f, p_10_20_transformed.x);
  EXPECT_FLOAT_EQ(73.0f, p_10_20_transformed.y);
}

TEST(CFX_Matrix, TransformRectForRectF) {
  CFX_Matrix rotate_90;
  rotate_90.Rotate(FX_PI / 2);

  CFX_Matrix scale_5_13;
  scale_5_13.Scale(5, 13);

  CFX_RectF rect(10.5f, 20.5f, 4.25f, 3.25f);
  rect = rotate_90.TransformRect(rect);
  EXPECT_FLOAT_EQ(-23.75f, rect.Left());
  EXPECT_FLOAT_EQ(10.5f, rect.Top());
  EXPECT_FLOAT_EQ(3.25f, rect.Width());
  EXPECT_FLOAT_EQ(4.25f, rect.Height());

  rect = scale_5_13.TransformRect(rect);
  EXPECT_FLOAT_EQ(-118.75f, rect.Left());
  EXPECT_FLOAT_EQ(136.5f, rect.Top());
  EXPECT_FLOAT_EQ(16.25f, rect.Width());
  EXPECT_FLOAT_EQ(55.25f, rect.Height());
}

TEST(CFX_Matrix, TransformRectForFloatRect) {
  CFX_Matrix rotate_90;
  rotate_90.Rotate(FX_PI / 2);

  CFX_Matrix scale_5_13;
  scale_5_13.Scale(5, 13);

  CFX_FloatRect rect(5.5f, 0.0f, 12.25f, 2.7f);
  rect = rotate_90.TransformRect(rect);
  EXPECT_FLOAT_EQ(-2.7f, rect.Left());
  EXPECT_FLOAT_EQ(5.5f, rect.Bottom());
  EXPECT_NEAR(0.0f, rect.Right(), 0.00001f);
  EXPECT_FLOAT_EQ(12.25f, rect.Top());

  rect = scale_5_13.TransformRect(rect);
  EXPECT_FLOAT_EQ(-13.5f, rect.Left());
  EXPECT_FLOAT_EQ(71.5f, rect.Bottom());
  EXPECT_NEAR(0.0f, rect.Right(), 0.00001f);
  EXPECT_FLOAT_EQ(159.25f, rect.Top());
}
