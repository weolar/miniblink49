// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/math_util.h"

#include <cmath>

#include "cc/test/geometry_test_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/transform.h"

namespace cc {
namespace {

    TEST(MathUtilTest, ProjectionOfPerpendicularPlane)
    {
        // In this case, the m33() element of the transform becomes zero, which could
        // cause a divide-by-zero when projecting points/quads.

        gfx::Transform transform;
        transform.MakeIdentity();
        transform.matrix().set(2, 2, 0);

        gfx::RectF rect = gfx::RectF(0, 0, 1, 1);
        gfx::RectF projected_rect = MathUtil::ProjectClippedRect(transform, rect);

        EXPECT_EQ(0, projected_rect.x());
        EXPECT_EQ(0, projected_rect.y());
        EXPECT_TRUE(projected_rect.IsEmpty());
    }

    TEST(MathUtilTest, ProjectionOfAlmostPerpendicularPlane)
    {
        // In this case, the m33() element of the transform becomes almost zero, which
        // could cause a divide-by-zero when projecting points/quads.

        gfx::Transform transform;
        // The transform is from an actual test page:
        // [ +1.0000 +0.0000 -1.0000 +3144132.0000
        //   +0.0000 +1.0000 +0.0000 +0.0000
        //   +16331238407143424.0000 +0.0000 -0.0000 +51346917453137000267776.0000
        //   +0.0000 +0.0000 +0.0000 +1.0000 ]
        transform.MakeIdentity();
        transform.matrix().set(0, 2, static_cast<SkMScalar>(-1));
        transform.matrix().set(0, 3, static_cast<SkMScalar>(3144132.0));
        transform.matrix().set(2, 0, static_cast<SkMScalar>(16331238407143424.0));
        transform.matrix().set(2, 2, static_cast<SkMScalar>(-1e-33));
        transform.matrix().set(2, 3,
            static_cast<SkMScalar>(51346917453137000267776.0));

        gfx::RectF rect = gfx::RectF(0, 0, 1, 1);
        gfx::RectF projected_rect = MathUtil::ProjectClippedRect(transform, rect);

        EXPECT_EQ(0, projected_rect.x());
        EXPECT_EQ(0, projected_rect.y());
        EXPECT_TRUE(projected_rect.IsEmpty()) << projected_rect.ToString();
    }

    TEST(MathUtilTest, EnclosingClippedRectUsesCorrectInitialBounds)
    {
        HomogeneousCoordinate h1(-100, -100, 0, 1);
        HomogeneousCoordinate h2(-10, -10, 0, 1);
        HomogeneousCoordinate h3(10, 10, 0, -1);
        HomogeneousCoordinate h4(100, 100, 0, -1);

        // The bounds of the enclosing clipped rect should be -100 to -10 for both x
        // and y. However, if there is a bug where the initial xmin/xmax/ymin/ymax are
        // initialized to numeric_limits<float>::min() (which is zero, not -flt_max)
        // then the enclosing clipped rect will be computed incorrectly.
        gfx::RectF result = MathUtil::ComputeEnclosingClippedRect(h1, h2, h3, h4);

        // Due to floating point math in ComputeClippedPointForEdge this result
        // is fairly imprecise.  0.15f was empirically determined.
        EXPECT_RECT_NEAR(
            gfx::RectF(gfx::PointF(-100, -100), gfx::SizeF(90, 90)), result, 0.15f);
    }

    TEST(MathUtilTest, EnclosingRectOfVerticesUsesCorrectInitialBounds)
    {
        gfx::PointF vertices[3];
        int num_vertices = 3;

        vertices[0] = gfx::PointF(-10, -100);
        vertices[1] = gfx::PointF(-100, -10);
        vertices[2] = gfx::PointF(-30, -30);

        // The bounds of the enclosing rect should be -100 to -10 for both x and y.
        // However, if there is a bug where the initial xmin/xmax/ymin/ymax are
        // initialized to numeric_limits<float>::min() (which is zero, not -flt_max)
        // then the enclosing clipped rect will be computed incorrectly.
        gfx::RectF result = MathUtil::ComputeEnclosingRectOfVertices(vertices, num_vertices);

        EXPECT_FLOAT_RECT_EQ(gfx::RectF(gfx::PointF(-100, -100), gfx::SizeF(90, 90)),
            result);
    }

    TEST(MathUtilTest, SmallestAngleBetweenVectors)
    {
        gfx::Vector2dF x(1, 0);
        gfx::Vector2dF y(0, 1);
        gfx::Vector2dF test_vector(0.5, 0.5);

        // Orthogonal vectors are at an angle of 90 degress.
        EXPECT_EQ(90, MathUtil::SmallestAngleBetweenVectors(x, y));

        // A vector makes a zero angle with itself.
        EXPECT_EQ(0, MathUtil::SmallestAngleBetweenVectors(x, x));
        EXPECT_EQ(0, MathUtil::SmallestAngleBetweenVectors(y, y));
        EXPECT_EQ(0, MathUtil::SmallestAngleBetweenVectors(test_vector, test_vector));

        // Parallel but reversed vectors are at 180 degrees.
        EXPECT_FLOAT_EQ(180, MathUtil::SmallestAngleBetweenVectors(x, -x));
        EXPECT_FLOAT_EQ(180, MathUtil::SmallestAngleBetweenVectors(y, -y));
        EXPECT_FLOAT_EQ(
            180, MathUtil::SmallestAngleBetweenVectors(test_vector, -test_vector));

        // The test vector is at a known angle.
        EXPECT_FLOAT_EQ(
            45, std::floor(MathUtil::SmallestAngleBetweenVectors(test_vector, x)));
        EXPECT_FLOAT_EQ(
            45, std::floor(MathUtil::SmallestAngleBetweenVectors(test_vector, y)));
    }

    TEST(MathUtilTest, VectorProjection)
    {
        gfx::Vector2dF x(1, 0);
        gfx::Vector2dF y(0, 1);
        gfx::Vector2dF test_vector(0.3f, 0.7f);

        // Orthogonal vectors project to a zero vector.
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0, 0), MathUtil::ProjectVector(x, y));
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0, 0), MathUtil::ProjectVector(y, x));

        // Projecting a vector onto the orthonormal basis gives the corresponding
        // component of the vector.
        EXPECT_VECTOR_EQ(gfx::Vector2dF(test_vector.x(), 0),
            MathUtil::ProjectVector(test_vector, x));
        EXPECT_VECTOR_EQ(gfx::Vector2dF(0, test_vector.y()),
            MathUtil::ProjectVector(test_vector, y));

        // Finally check than an arbitrary vector projected to another one gives a
        // vector parallel to the second vector.
        gfx::Vector2dF target_vector(0.5, 0.2f);
        gfx::Vector2dF projected_vector = MathUtil::ProjectVector(test_vector, target_vector);
        EXPECT_EQ(projected_vector.x() / target_vector.x(),
            projected_vector.y() / target_vector.y());
    }

    TEST(MathUtilTest, MapEnclosedRectWith2dAxisAlignedTransform)
    {
        gfx::Rect input(1, 2, 3, 4);
        gfx::Rect output;
        gfx::Transform transform;

        // Identity.
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_EQ(input, output);

        // Integer translate.
        transform.Translate(2.0, 3.0);
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_EQ(gfx::Rect(3, 5, 3, 4), output);

        // Non-integer translate.
        transform.Translate(0.5, 0.5);
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_EQ(gfx::Rect(4, 6, 2, 3), output);

        // Scale.
        transform = gfx::Transform();
        transform.Scale(2.0, 3.0);
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_EQ(gfx::Rect(2, 6, 6, 12), output);

        // Rotate Z.
        transform = gfx::Transform();
        transform.Translate(1.0, 2.0);
        transform.RotateAboutZAxis(90.0);
        transform.Translate(-1.0, -2.0);
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_EQ(gfx::Rect(-3, 2, 4, 3), output);

        // Rotate X.
        transform = gfx::Transform();
        transform.RotateAboutXAxis(90.0);
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_TRUE(output.IsEmpty());

        transform = gfx::Transform();
        transform.RotateAboutXAxis(180.0);
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_EQ(gfx::Rect(1, -6, 3, 4), output);

        // Rotate Y.
        transform = gfx::Transform();
        transform.RotateAboutYAxis(90.0);
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_TRUE(output.IsEmpty());

        transform = gfx::Transform();
        transform.RotateAboutYAxis(180.0);
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_EQ(gfx::Rect(-4, 2, 3, 4), output);

        // Translate Z.
        transform = gfx::Transform();
        transform.ApplyPerspectiveDepth(10.0);
        transform.Translate3d(0.0, 0.0, 5.0);
        output = MathUtil::MapEnclosedRectWith2dAxisAlignedTransform(transform, input);
        EXPECT_EQ(gfx::Rect(2, 4, 6, 8), output);
    }

    TEST(MathUtilTest, MapEnclosingRectWithLargeTransforms)
    {
        gfx::Rect input(1, 2, 100, 200);
        gfx::Rect output;

        gfx::Transform large_x_scale;
        large_x_scale.Scale(SkDoubleToMScalar(1e37), 1.0);

        gfx::Transform infinite_x_scale;
        infinite_x_scale = large_x_scale * large_x_scale;

        gfx::Transform large_y_scale;
        large_y_scale.Scale(1.0, SkDoubleToMScalar(1e37));

        gfx::Transform infinite_y_scale;
        infinite_y_scale = large_y_scale * large_y_scale;

        gfx::Transform rotation;
        rotation.RotateAboutYAxis(170.0);

        int max_int = std::numeric_limits<int>::max();

        output = MathUtil::MapEnclosingClippedRect(large_x_scale, input);
        EXPECT_EQ(gfx::Rect(max_int, 2, 0, 200), output);

        output = MathUtil::MapEnclosingClippedRect(large_x_scale * rotation, input);
        EXPECT_EQ(gfx::Rect(), output);

        output = MathUtil::MapEnclosingClippedRect(infinite_x_scale, input);
        EXPECT_EQ(gfx::Rect(max_int, 2, 0, 200), output);

        output = MathUtil::MapEnclosingClippedRect(infinite_x_scale * rotation, input);
        EXPECT_EQ(gfx::Rect(), output);

        output = MathUtil::MapEnclosingClippedRect(large_y_scale, input);
        EXPECT_EQ(gfx::Rect(1, max_int, 100, 0), output);

        output = MathUtil::MapEnclosingClippedRect(large_y_scale * rotation, input);
        EXPECT_EQ(gfx::Rect(-100, max_int, 100, 0), output);

        output = MathUtil::MapEnclosingClippedRect(infinite_y_scale, input);
        EXPECT_EQ(gfx::Rect(1, max_int, 100, 0), output);

        output = MathUtil::MapEnclosingClippedRect(infinite_y_scale * rotation, input);
        EXPECT_EQ(gfx::Rect(), output);
    }

    TEST(MathUtilTest, ProjectEnclosingRectWithLargeTransforms)
    {
        gfx::Rect input(1, 2, 100, 200);
        gfx::Rect output;

        gfx::Transform large_x_scale;
        large_x_scale.Scale(SkDoubleToMScalar(1e37), 1.0);

        gfx::Transform infinite_x_scale;
        infinite_x_scale = large_x_scale * large_x_scale;

        gfx::Transform large_y_scale;
        large_y_scale.Scale(1.0, SkDoubleToMScalar(1e37));

        gfx::Transform infinite_y_scale;
        infinite_y_scale = large_y_scale * large_y_scale;

        gfx::Transform rotation;
        rotation.RotateAboutYAxis(170.0);

        int max_int = std::numeric_limits<int>::max();

        output = MathUtil::ProjectEnclosingClippedRect(large_x_scale, input);
        EXPECT_EQ(gfx::Rect(max_int, 2, 0, 200), output);

        output = MathUtil::ProjectEnclosingClippedRect(large_x_scale * rotation, input);
        EXPECT_EQ(gfx::Rect(), output);

        output = MathUtil::ProjectEnclosingClippedRect(infinite_x_scale, input);
        EXPECT_EQ(gfx::Rect(max_int, 2, 0, 200), output);

        output = MathUtil::ProjectEnclosingClippedRect(infinite_x_scale * rotation, input);
        EXPECT_EQ(gfx::Rect(), output);

        output = MathUtil::ProjectEnclosingClippedRect(large_y_scale, input);
        EXPECT_EQ(gfx::Rect(1, max_int, 100, 0), output);

        output = MathUtil::ProjectEnclosingClippedRect(large_y_scale * rotation, input);
        EXPECT_EQ(gfx::Rect(-103, max_int, 102, 0), output);

        output = MathUtil::ProjectEnclosingClippedRect(infinite_y_scale, input);
        EXPECT_EQ(gfx::Rect(1, max_int, 100, 0), output);

        output = MathUtil::ProjectEnclosingClippedRect(infinite_y_scale * rotation, input);
        EXPECT_EQ(gfx::Rect(), output);
    }

    TEST(MathUtilTest, RoundUp)
    {
        for (int multiplier = 1; multiplier <= 10; ++multiplier) {
            // Try attempts in descending order, so that we can
            // determine the correct value before it's needed.
            int correct;
            for (int attempt = 5 * multiplier; attempt >= -5 * multiplier; --attempt) {
                if ((attempt % multiplier) == 0)
                    correct = attempt;
                EXPECT_EQ(correct, MathUtil::UncheckedRoundUp(attempt, multiplier))
                    << "attempt=" << attempt << " multiplier=" << multiplier;
            }
        }

        for (unsigned multiplier = 1; multiplier <= 10; ++multiplier) {
            // Try attempts in descending order, so that we can
            // determine the correct value before it's needed.
            unsigned correct;
            for (unsigned attempt = 5 * multiplier; attempt > 0; --attempt) {
                if ((attempt % multiplier) == 0)
                    correct = attempt;
                EXPECT_EQ(correct, MathUtil::UncheckedRoundUp(attempt, multiplier))
                    << "attempt=" << attempt << " multiplier=" << multiplier;
            }
            EXPECT_EQ(0u, MathUtil::UncheckedRoundUp(0u, multiplier))
                << "attempt=0 multiplier=" << multiplier;
        }
    }

    TEST(MathUtilTest, RoundUpOverflow)
    {
        // Rounding up 123 by 50 is 150, which overflows int8_t, but fits in uint8_t.
        EXPECT_FALSE(MathUtil::VerifyRoundup<int8_t>(123, 50));
        EXPECT_TRUE(MathUtil::VerifyRoundup<uint8_t>(123, 50));
    }

    TEST(MathUtilTest, RoundDown)
    {
        for (int multiplier = 1; multiplier <= 10; ++multiplier) {
            // Try attempts in ascending order, so that we can
            // determine the correct value before it's needed.
            int correct;
            for (int attempt = -5 * multiplier; attempt <= 5 * multiplier; ++attempt) {
                if ((attempt % multiplier) == 0)
                    correct = attempt;
                EXPECT_EQ(correct, MathUtil::UncheckedRoundDown(attempt, multiplier))
                    << "attempt=" << attempt << " multiplier=" << multiplier;
            }
        }

        for (unsigned multiplier = 1; multiplier <= 10; ++multiplier) {
            // Try attempts in ascending order, so that we can
            // determine the correct value before it's needed.
            unsigned correct;
            for (unsigned attempt = 0; attempt <= 5 * multiplier; ++attempt) {
                if ((attempt % multiplier) == 0)
                    correct = attempt;
                EXPECT_EQ(correct, MathUtil::UncheckedRoundDown(attempt, multiplier))
                    << "attempt=" << attempt << " multiplier=" << multiplier;
            }
        }
    }

    TEST(MathUtilTest, RoundDownUnderflow)
    {
        // Rounding down -123 by 50 is -150, which underflows int8_t, but fits in
        // int16_t.
        EXPECT_FALSE(MathUtil::VerifyRoundDown<int8_t>(-123, 50));
        EXPECT_TRUE(MathUtil::VerifyRoundDown<int16_t>(-123, 50));
    }

} // namespace
} // namespace cc
